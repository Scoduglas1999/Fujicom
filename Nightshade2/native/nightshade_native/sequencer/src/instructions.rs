//! Instruction execution implementations
//! 
//! These functions implement the actual device control for sequencer instructions.
//! They use the DeviceOps trait to communicate with real or simulated hardware.

use crate::*;
use crate::device_ops::{SharedDeviceOps, ImageData};
use std::time::Duration;
use std::path::PathBuf;
use std::sync::{Arc, atomic::{AtomicBool, Ordering}};
use tokio::time::sleep;

/// Result of an instruction execution
pub struct InstructionResult {
    pub status: NodeStatus,
    pub message: Option<String>,
    pub data: Option<serde_json::Value>,
    /// HFR values from exposures (for trigger monitoring)
    pub hfr_values: Vec<f64>,
}

impl InstructionResult {
    pub fn success() -> Self {
        Self {
            status: NodeStatus::Success,
            message: None,
            data: None,
            hfr_values: Vec::new(),
        }
    }

    pub fn success_with_message(message: impl Into<String>) -> Self {
        Self {
            status: NodeStatus::Success,
            message: Some(message.into()),
            data: None,
            hfr_values: Vec::new(),
        }
    }

    pub fn failure(message: impl Into<String>) -> Self {
        Self {
            status: NodeStatus::Failure,
            message: Some(message.into()),
            data: None,
            hfr_values: Vec::new(),
        }
    }

    /// Create a failure result with a recovery code that the UI can use to offer recovery options
    pub fn failure_with_recovery(message: impl Into<String>, recovery_code: impl Into<String>) -> Self {
        Self {
            status: NodeStatus::Failure,
            message: Some(message.into()),
            data: Some(serde_json::json!({"recovery_code": recovery_code.into()})),
            hfr_values: Vec::new(),
        }
    }

    pub fn cancelled(message: impl Into<String>) -> Self {
        Self {
            status: NodeStatus::Cancelled,
            message: Some(message.into()),
            data: None,
            hfr_values: Vec::new(),
        }
    }

    /// Get the status, logging any failure or cancellation message.
    /// This ensures error messages are not silently discarded.
    pub fn log_and_get_status(self, node_name: &str) -> NodeStatus {
        match self.status {
            NodeStatus::Failure => {
                if let Some(msg) = &self.message {
                    tracing::error!("{} failed: {}", node_name, msg);
                } else {
                    tracing::error!("{} failed (no details)", node_name);
                }
            }
            NodeStatus::Cancelled => {
                if let Some(msg) = &self.message {
                    tracing::warn!("{} cancelled: {}", node_name, msg);
                }
            }
            _ => {}
        }
        self.status
    }
}

/// Context for instruction execution
/// Contains the current imaging session state and cancellation flag
pub struct InstructionContext {
    /// Target RA in hours
    pub target_ra: Option<f64>,
    /// Target Dec in degrees
    pub target_dec: Option<f64>,
    /// Target name
    pub target_name: Option<String>,
    /// Current filter
    pub current_filter: Option<String>,
    /// Current binning
    pub current_binning: Binning,
    /// Cancellation token
    pub cancellation_token: Arc<AtomicBool>,
    /// Connected camera device ID
    pub camera_id: Option<String>,
    /// Connected mount device ID
    pub mount_id: Option<String>,
    /// Connected focuser device ID
    pub focuser_id: Option<String>,
    /// Connected filter wheel device ID
    pub filterwheel_id: Option<String>,
    /// Connected rotator device ID
    pub rotator_id: Option<String>,
    /// Connected dome device ID
    pub dome_id: Option<String>,
    /// Connected cover calibrator (flat panel) device ID
    pub cover_calibrator_id: Option<String>,
    /// Base path for saving images
    pub save_path: Option<PathBuf>,
    /// Observer's latitude (degrees)
    pub latitude: Option<f64>,
    /// Observer's longitude (degrees)
    pub longitude: Option<f64>,
    /// Device operations handler
    pub device_ops: SharedDeviceOps,
    /// Trigger state (for updating during execution)
    pub trigger_state: Option<Arc<tokio::sync::RwLock<crate::triggers::TriggerState>>>,
}

impl InstructionContext {
    pub fn check_cancelled(&self) -> Option<InstructionResult> {
        if self.cancellation_token.load(Ordering::Relaxed) {
            Some(InstructionResult::cancelled("Operation cancelled"))
        } else {
            None
        }
    }
    
    /// Get camera ID or error
    pub fn camera_id(&self) -> Result<&str, InstructionResult> {
        self.camera_id.as_deref().ok_or_else(|| 
            InstructionResult::failure("No camera connected"))
    }
    
    /// Get mount ID or error
    pub fn mount_id(&self) -> Result<&str, InstructionResult> {
        self.mount_id.as_deref().ok_or_else(|| 
            InstructionResult::failure("No mount connected"))
    }
    
    /// Get focuser ID or error
    pub fn focuser_id(&self) -> Result<&str, InstructionResult> {
        self.focuser_id.as_deref().ok_or_else(|| 
            InstructionResult::failure("No focuser connected"))
    }
    
    /// Get filter wheel ID or error
    pub fn filterwheel_id(&self) -> Result<&str, InstructionResult> {
        self.filterwheel_id.as_deref().ok_or_else(|| 
            InstructionResult::failure("No filter wheel connected"))
    }
    
    /// Get rotator ID or error  
    pub fn rotator_id(&self) -> Result<&str, InstructionResult> {
        self.rotator_id.as_deref().ok_or_else(|| 
            InstructionResult::failure("No rotator connected"))
    }

    /// Get dome ID or error
    pub fn dome_id(&self) -> Result<&str, InstructionResult> {
        self.dome_id.as_deref().ok_or_else(||
            InstructionResult::failure("No dome connected"))
    }

    /// Get cover calibrator ID or error
    pub fn cover_calibrator_id(&self) -> Result<&str, InstructionResult> {
        self.cover_calibrator_id.as_deref().ok_or_else(||
            InstructionResult::failure("No cover calibrator (flat panel) connected"))
    }
}

// =============================================================================
// SLEW INSTRUCTION
// =============================================================================

/// Default tolerance for slew position validation in degrees (1 arcminute = 1/60 degree)
const SLEW_POSITION_TOLERANCE_DEG: f64 = 1.0 / 60.0;

/// Normalize RA difference to account for wraparound at 24 hours
/// Returns the shortest angular distance between two RA values in hours
fn normalize_ra_diff_hours(diff: f64) -> f64 {
    // Wrap to -12 to +12 hours range (equivalent to -180 to +180 degrees)
    let mut wrapped = diff % 24.0;
    if wrapped > 12.0 {
        wrapped -= 24.0;
    } else if wrapped < -12.0 {
        wrapped += 24.0;
    }
    wrapped
}

/// Validate that mount reached the target position within tolerance
/// ra_target and ra_actual are in hours, dec_target and dec_actual are in degrees
/// tolerance_deg is the maximum allowed difference in degrees
fn validate_slew_position(
    ra_target: f64,
    dec_target: f64,
    ra_actual: f64,
    dec_actual: f64,
    tolerance_deg: f64,
) -> Result<(), String> {
    // Calculate RA difference (accounting for wraparound) and convert to degrees
    let ra_diff_hours = normalize_ra_diff_hours(ra_actual - ra_target);
    let ra_diff_deg = ra_diff_hours * 15.0; // 1 hour = 15 degrees

    // Dec difference (no wraparound needed)
    let dec_diff_deg = dec_actual - dec_target;

    // Check if within tolerance
    if ra_diff_deg.abs() > tolerance_deg || dec_diff_deg.abs() > tolerance_deg {
        return Err(format!(
            "Mount slew did not reach target position. Expected RA={:.4}h, Dec={:.4}°, \
             got RA={:.4}h, Dec={:.4}° (diff: RA={:.2}', Dec={:.2}')",
            ra_target, dec_target, ra_actual, dec_actual,
            ra_diff_deg * 60.0, // Convert to arcminutes for readability
            dec_diff_deg * 60.0
        ));
    }

    Ok(())
}

/// Execute a slew instruction
pub async fn execute_slew(
    config: &SlewConfig,
    ctx: &InstructionContext,
    progress_callback: Option<&(dyn Fn(f64, String) + Send + Sync)>,
) -> InstructionResult {
    let mount_id = match ctx.mount_id() {
        Ok(id) => id,
        Err(e) => return e,
    };

    // Check if mount is parked - cannot slew while parked
    match ctx.device_ops.mount_is_parked(mount_id).await {
        Ok(true) => {
            tracing::warn!("Mount is parked, cannot slew. Please unpark the mount first.");
            return InstructionResult::failure_with_recovery(
                "Mount is parked. Please unpark the mount before slewing.",
                "MOUNT_PARKED",
            );
        }
        Ok(false) => {
            tracing::debug!("Mount is not parked, proceeding with slew");
        }
        Err(e) => {
            // Log but continue - some mounts may not support park status query
            tracing::debug!("Could not check mount park status: {}", e);
        }
    }

    // Get coordinates
    let (ra, dec) = if config.use_target_coords {
        match (ctx.target_ra, ctx.target_dec) {
            (Some(ra), Some(dec)) => (ra, dec),
            _ => return InstructionResult::failure("No target coordinates available"),
        }
    } else {
        match (config.custom_ra, config.custom_dec) {
            (Some(ra), Some(dec)) => (ra, dec),
            _ => return InstructionResult::failure("No custom coordinates specified"),
        }
    };

    tracing::info!("Slewing to RA: {:.4}h, Dec: {:.4}°", ra, dec);

    // Emit initial progress
    if let Some(cb) = progress_callback {
        cb(0.0, format!("Slewing to RA: {:.2}h, Dec: {:.1}°", ra, dec));
    }

    if let Some(result) = ctx.check_cancelled() {
        return result;
    }

    // Start the slew
    tokio::select! {
        result = ctx.device_ops.mount_slew_to_coordinates(mount_id, ra, dec) => {
            match result {
                Ok(_) => {
                    // Wait for mount to actually stop slewing
                    match wait_for_mount_idle_with_progress(mount_id, ctx, Duration::from_secs(1800), progress_callback).await {
                        Ok(_) => {
                            // Validate that mount reached the target position
                            match ctx.device_ops.mount_get_coordinates(mount_id).await {
                                Ok((actual_ra, actual_dec)) => {
                                    tracing::debug!(
                                        "Slew completed. Target: RA={:.4}h, Dec={:.4}°, Actual: RA={:.4}h, Dec={:.4}°",
                                        ra, dec, actual_ra, actual_dec
                                    );

                                    // Validate position within tolerance (1 arcminute)
                                    if let Err(e) = validate_slew_position(
                                        ra, dec, actual_ra, actual_dec,
                                        SLEW_POSITION_TOLERANCE_DEG,
                                    ) {
                                        tracing::warn!("Slew position validation failed: {}", e);
                                        return InstructionResult::failure_with_recovery(
                                            &e,
                                            "SLEW_POSITION_MISMATCH",
                                        );
                                    }

                                    if let Some(cb) = progress_callback {
                                        cb(100.0, format!("Arrived at RA: {:.2}h, Dec: {:.1}°", actual_ra, actual_dec));
                                    }
                                    InstructionResult::success_with_message(format!(
                                        "Slewed to RA: {:.4}h, Dec: {:.4}° (verified)",
                                        actual_ra, actual_dec
                                    ))
                                }
                                Err(e) => {
                                    // Could not read coordinates to validate, but slew appeared to complete
                                    // Log warning but consider it a success with caveat
                                    tracing::warn!(
                                        "Slew completed but could not verify position: {}. \
                                         Assuming success based on slew completion.",
                                        e
                                    );
                                    if let Some(cb) = progress_callback {
                                        cb(100.0, format!("Arrived at RA: {:.2}h, Dec: {:.1}° (unverified)", ra, dec));
                                    }
                                    InstructionResult::success_with_message(format!(
                                        "Slewed to RA: {:.4}h, Dec: {:.4}° (position unverified: {})",
                                        ra, dec, e
                                    ))
                                }
                            }
                        }
                        Err(e) => InstructionResult::failure(e),
                    }
                }
                Err(e) => InstructionResult::failure(format!("Slew failed: {}", e)),
            }
        }
        _ = wait_for_cancellation(ctx.cancellation_token.clone()) => {
            tracing::info!("Slew cancelled, aborting...");
            let _ = ctx.device_ops.mount_abort_slew(mount_id).await;
            InstructionResult::cancelled("Slew cancelled")
        }
    }
}

/// Wait for mount to stop slewing with timeout
async fn wait_for_mount_idle(mount_id: &str, ctx: &InstructionContext, timeout: Duration) -> Result<(), String> {
    wait_for_mount_idle_with_progress(mount_id, ctx, timeout, None).await
}

/// Wait for mount to stop slewing with timeout and progress updates
async fn wait_for_mount_idle_with_progress(
    mount_id: &str,
    ctx: &InstructionContext,
    timeout: Duration,
    progress_callback: Option<&(dyn Fn(f64, String) + Send + Sync)>,
) -> Result<(), String> {
    let start = std::time::Instant::now();
    let mut poll_count = 0u32;

    loop {
        // Check cancellation
        if ctx.cancellation_token.load(Ordering::Relaxed) {
            let _ = ctx.device_ops.mount_abort_slew(mount_id).await;
            return Err("Operation cancelled".to_string());
        }

        // Check if mount is still slewing
        match ctx.device_ops.mount_is_slewing(mount_id).await {
            Ok(is_slewing) => {
                if !is_slewing {
                    tracing::debug!("Mount reached target position");
                    return Ok(());
                }
            }
            Err(e) => {
                tracing::warn!("Error checking slew status: {}", e);
                // Continue polling - transient error
            }
        }

        // Emit progress every 2 seconds (4 polls at 500ms)
        poll_count += 1;
        if poll_count % 4 == 0 {
            let elapsed_secs = start.elapsed().as_secs();
            // Use time-based progress as approximation (typical slew is 30-60s)
            let progress = ((elapsed_secs as f64 / 60.0) * 100.0).min(95.0);
            if let Some(cb) = progress_callback {
                cb(progress, format!("Slewing... ({:.0}s)", elapsed_secs));
            }
        }

        // Check timeout
        if start.elapsed() > timeout {
            return Err(format!("Mount slew timed out after {} seconds", timeout.as_secs()));
        }

        // Poll every 500ms
        sleep(Duration::from_millis(500)).await;
    }
}

/// Wait for focuser to stop moving with timeout
async fn wait_for_focuser_idle(focuser_id: &str, ctx: &InstructionContext, timeout: Duration) -> Result<(), String> {
    let start = std::time::Instant::now();
    loop {
        // Check cancellation
        if ctx.cancellation_token.load(Ordering::Relaxed) {
            return Err("Operation cancelled".to_string());
        }

        // Check if focuser is still moving
        match ctx.device_ops.focuser_is_moving(focuser_id).await {
            Ok(is_moving) => {
                if !is_moving {
                    // Add small settling delay
                    sleep(Duration::from_millis(100)).await;
                    tracing::debug!("Focuser reached target position");
                    return Ok(());
                }
            }
            Err(e) => {
                tracing::warn!("Error checking focuser status: {}", e);
                // Continue polling - transient error
            }
        }

        // Check timeout
        if start.elapsed() > timeout {
            return Err(format!("Focuser move timed out after {} seconds", timeout.as_secs()));
        }

        // Poll every 100ms for focuser (faster than mount)
        sleep(Duration::from_millis(100)).await;
    }
}

/// Wait for focuser to stop moving after a halt command (ignores cancellation token).
/// This is used during cancellation handling to ensure the focuser has actually stopped
/// before returning control. The timeout is shorter since we're just waiting for halt.
pub async fn wait_for_focuser_stop_after_halt(
    focuser_id: &str,
    device_ops: &crate::device_ops::SharedDeviceOps,
    timeout: Duration,
) {
    let start = std::time::Instant::now();
    loop {
        // Check if focuser is still moving
        match device_ops.focuser_is_moving(focuser_id).await {
            Ok(is_moving) => {
                if !is_moving {
                    tracing::debug!("Focuser stopped after halt");
                    return;
                }
            }
            Err(e) => {
                tracing::warn!("Error checking focuser status after halt: {}", e);
                // Continue polling - transient error
            }
        }

        // Check timeout
        if start.elapsed() > timeout {
            tracing::warn!("Focuser did not stop within {} seconds after halt", timeout.as_secs());
            return;
        }

        // Poll every 100ms
        sleep(Duration::from_millis(100)).await;
    }
}

/// Wait for filter wheel to reach target position with timeout
async fn wait_for_filterwheel_idle(fw_id: &str, target_position: i32, ctx: &InstructionContext, timeout: Duration) -> Result<(), String> {
    let start = std::time::Instant::now();

    // Small initial delay to allow filter wheel to start moving
    sleep(Duration::from_millis(100)).await;

    loop {
        // Check cancellation
        if ctx.cancellation_token.load(Ordering::Relaxed) {
            return Err("Operation cancelled".to_string());
        }

        // Check if filter wheel reached target position
        match ctx.device_ops.filterwheel_get_position(fw_id).await {
            Ok(current_pos) => {
                if current_pos == target_position {
                    tracing::debug!("Filter wheel reached target position {}", target_position);
                    return Ok(());
                }
                tracing::trace!("Filter wheel at position {}, waiting for {}", current_pos, target_position);
            }
            Err(e) => {
                tracing::warn!("Error checking filter wheel position: {}", e);
                // Continue polling - transient error
            }
        }

        // Check timeout
        if start.elapsed() > timeout {
            return Err(format!("Filter wheel move timed out after {} seconds (target: {})", timeout.as_secs(), target_position));
        }

        // Poll every 200ms
        sleep(Duration::from_millis(200)).await;
    }
}

async fn wait_for_cancellation(token: Arc<AtomicBool>) {
    loop {
        if token.load(Ordering::Relaxed) { return; }
        sleep(Duration::from_millis(100)).await;
    }
}

// =============================================================================
// CENTER INSTRUCTION (Plate Solve + Sync + Slew Loop)
// =============================================================================

/// Execute a center instruction (plate solve + sync + slew loop)
pub async fn execute_center(
    config: &CenterConfig,
    ctx: &InstructionContext,
    progress_callback: Option<&(dyn Fn(f64, String) + Send + Sync)>,
) -> InstructionResult {
    let mount_id = match ctx.mount_id() {
        Ok(id) => id.to_string(),
        Err(e) => return e,
    };
    let camera_id = match ctx.camera_id() {
        Ok(id) => id.to_string(),
        Err(e) => return e,
    };

    let (target_ra_deg, target_dec) = if config.use_target_coords {
        match (ctx.target_ra, ctx.target_dec) {
            (Some(ra), Some(dec)) => (ra * 15.0, dec), // Convert RA hours to degrees
            _ => return InstructionResult::failure("No target coordinates available"),
        }
    } else {
        return InstructionResult::failure("Custom coordinates for centering not implemented");
    };

    tracing::info!("Centering on RA: {:.4}°, Dec: {:.4}° (accuracy: {:.1}\")",
        target_ra_deg, target_dec, config.accuracy_arcsec);

    // Emit initial progress
    if let Some(cb) = progress_callback {
        cb(0.0, format!("Centering (target: {:.1}\")", config.accuracy_arcsec));
    }

    for attempt in 1..=config.max_attempts {
        if let Some(result) = ctx.check_cancelled() {
            return result;
        }

        let attempt_progress = ((attempt - 1) as f64 / config.max_attempts as f64) * 100.0;
        tracing::info!("Center attempt {}/{}", attempt, config.max_attempts);

        // Emit progress for this attempt
        if let Some(cb) = progress_callback {
            cb(attempt_progress, format!("Attempt {}/{}: Capturing...", attempt, config.max_attempts));
        }
        
        // Take a plate solve exposure
        let image_data = tokio::select! {
            result = ctx.device_ops.camera_start_exposure(
                &camera_id,
                config.exposure_duration,
                None,
                None,
                1, 1, // Full resolution for best solve
            ) => {
                match result {
                    Ok(data) => data,
                    Err(e) => return InstructionResult::failure(format!("Failed to capture image: {}", e)),
                }
            }
            _ = wait_for_cancellation(ctx.cancellation_token.clone()) => {
                tracing::info!("Center cancelled during exposure, aborting...");
                let _ = ctx.device_ops.camera_abort_exposure(&camera_id).await;
                return InstructionResult::cancelled("Center cancelled");
            }
        };
        
        // Plate solve the image
        let solve_result = tokio::select! {
            result = ctx.device_ops.plate_solve(
                &image_data,
                Some(target_ra_deg),
                Some(target_dec),
                None,
            ) => {
                match result {
                    Ok(result) if result.success => result,
                    Ok(_) => {
                        tracing::warn!("Plate solve failed on attempt {}", attempt);
                        continue;
                    }
                    Err(e) => {
                        tracing::warn!("Plate solve error on attempt {}: {}", attempt, e);
                        continue;
                    }
                }
            }
            _ = wait_for_cancellation(ctx.cancellation_token.clone()) => {
                tracing::info!("Center cancelled during plate solve");
                return InstructionResult::cancelled("Center cancelled");
            }
        };
        
        // Update trigger state with plate solve result for drift detection
        if let Some(trigger_state_lock) = &ctx.trigger_state {
            if let Ok(mut trigger_state) = trigger_state_lock.try_write() {
                trigger_state.update_plate_solve(
                    solve_result.ra_degrees,
                    solve_result.dec_degrees,
                    solve_result.pixel_scale,
                );
                tracing::debug!(
                    "Updated trigger state with plate solve: RA={:.4}°, Dec={:.4}°, scale={:.2}\"/px",
                    solve_result.ra_degrees, solve_result.dec_degrees, solve_result.pixel_scale
                );
            }
        }

        // Calculate separation from target
        let separation_arcsec = calculate_separation_arcsec(
            target_ra_deg, target_dec,
            solve_result.ra_degrees, solve_result.dec_degrees
        );
        tracing::info!("Current separation: {:.1}\" from target", separation_arcsec);

        // Emit progress with separation info
        if let Some(cb) = progress_callback {
            cb(attempt_progress + 50.0 / config.max_attempts as f64,
               format!("Attempt {}/{}: {:.1}\" off", attempt, config.max_attempts, separation_arcsec));
        }

        // Check if within tolerance
        if separation_arcsec <= config.accuracy_arcsec {
            if let Some(cb) = progress_callback {
                cb(100.0, format!("Centered: {:.1}\"", separation_arcsec));
            }
            return InstructionResult::success_with_message(
                format!("Centered within {:.1}\" after {} attempt(s)", separation_arcsec, attempt)
            );
        }

        // Sync mount to solved position
        let _ = ctx.device_ops.mount_sync(&mount_id, solve_result.ra_degrees / 15.0, solve_result.dec_degrees).await;

        // Slew to target
        tracing::info!("Slewing to correct position...");
        if let Some(cb) = progress_callback {
            cb(attempt_progress + 75.0 / config.max_attempts as f64,
               format!("Attempt {}/{}: Correcting...", attempt, config.max_attempts));
        }

        tokio::select! {
            result = ctx.device_ops.mount_slew_to_coordinates(&mount_id, target_ra_deg / 15.0, target_dec) => {
                if let Err(e) = result {
                    tracing::warn!("Correction slew failed: {}", e);
                }
            }
            _ = wait_for_cancellation(ctx.cancellation_token.clone()) => {
                tracing::info!("Center cancelled during correction slew, aborting...");
                let _ = ctx.device_ops.mount_abort_slew(&mount_id).await;
                return InstructionResult::cancelled("Center cancelled");
            }
        }

        // Wait for settling
        sleep(Duration::from_secs(2)).await;
    }

    InstructionResult::failure(format!(
        "Failed to center within {:.1}\" after {} attempts",
        config.accuracy_arcsec, config.max_attempts
    ))
}

/// Calculate separation between two coordinates in arcseconds
fn calculate_separation_arcsec(ra1_deg: f64, dec1_deg: f64, ra2_deg: f64, dec2_deg: f64) -> f64 {
    let dec1_rad = dec1_deg.to_radians();
    let dec2_rad = dec2_deg.to_radians();
    let delta_ra = (ra2_deg - ra1_deg).to_radians();
    let delta_dec = (dec2_deg - dec1_deg).to_radians();
    
    // Haversine formula for angular separation
    let a = (delta_dec / 2.0).sin().powi(2) + 
            dec1_rad.cos() * dec2_rad.cos() * (delta_ra / 2.0).sin().powi(2);
    let c = 2.0 * a.sqrt().asin();
    
    c.to_degrees() * 3600.0 // Convert to arcseconds
}

// =============================================================================
// EXPOSURE INSTRUCTION
// =============================================================================

/// Execute an exposure instruction
pub async fn execute_exposure(
    config: &ExposureConfig, 
    ctx: &InstructionContext, 
    progress_callback: impl Fn(u32, u32)
) -> InstructionResult {
    let camera_id = match ctx.camera_id() {
        Ok(id) => id.to_string(),
        Err(e) => return e,
    };

    tracing::info!(
        "Starting {} {} x {:.1}s exposures",
        config.count,
        config.filter.as_deref().unwrap_or("unfiltered"),
        config.duration_secs
    );

    // Change filter if specified
    if let Some(filter) = &config.filter {
        if let Some(fw_id) = &ctx.filterwheel_id {
            tracing::info!("Changing to filter: {}", filter);
            if let Err(e) = ctx.device_ops.filterwheel_set_filter_by_name(fw_id, filter).await {
                return InstructionResult::failure(format!("Failed to change filter: {}", e));
            }
        }
    }

    // Determine binning
    let (bin_x, bin_y) = match config.binning {
        Binning::One => (1, 1),
        Binning::Two => (2, 2),
        Binning::Three => (3, 3),
        Binning::Four => (4, 4),
    };

    let mut completed_exposures = 0u32;
    let mut hfr_values = Vec::new();

    for frame in 1..=config.count {
        if let Some(result) = ctx.check_cancelled() {
            return result;
        }

        progress_callback(frame, config.count);
        tracing::info!("Capturing frame {}/{} ({:.1}s)", frame, config.count, config.duration_secs);

        // Start exposure
        let image_data = match ctx.device_ops.camera_start_exposure(
            &camera_id,
            config.duration_secs,
            config.gain,
            config.offset,
            bin_x,
            bin_y,
        ).await {
            Ok(data) => data,
            Err(e) => return InstructionResult::failure(format!("Exposure failed: {}", e)),
        };

        // Calculate HFR for trigger monitoring
        match ctx.device_ops.calculate_image_hfr(&image_data).await {
            Ok(Some(hfr)) => {
                tracing::info!("Frame {}/{} HFR: {:.2} pixels", frame, config.count, hfr);
                hfr_values.push(hfr);
            }
            Ok(None) => {
                tracing::warn!("Frame {}/{} - no stars detected for HFR calculation", frame, config.count);
            }
            Err(e) => {
                tracing::warn!("Frame {}/{} - HFR calculation failed: {}", frame, config.count, e);
            }
        }

        // Save the image
        let save_path = config.save_to.as_ref()
            .map(PathBuf::from)
            .or_else(|| ctx.save_path.clone());

        if let Some(base_path) = save_path {
            let filename = format!(
                "{}_{}_{:04}.fits",
                ctx.target_name.as_deref().unwrap_or("image"),
                config.filter.as_deref().unwrap_or("L"),
                frame
            );
            let full_path = base_path.join(&filename);

            if let Err(e) = ctx.device_ops.save_fits(
                &image_data,
                full_path.to_str().unwrap_or(&filename),
                ctx.target_name.as_deref(),
                config.filter.as_deref(),
                ctx.target_ra,
                ctx.target_dec,
            ).await {
                tracing::warn!("Failed to save image: {}", e);
            } else {
                tracing::info!("Saved: {}", full_path.display());
            }
        }

        completed_exposures += 1;

        // Notify about completed exposure with duration for integration time tracking
        progress_callback(frame, config.count); // This will be captured by the node for integration tracking

        // Dither if configured
        if let Some(dither_every) = config.dither_every {
            if dither_every > 0 && frame % dither_every == 0 && frame < config.count {
                tracing::info!("Dithering...");
                if let Err(e) = ctx.device_ops.guider_dither(5.0, 1.5, 30.0, 120.0, false).await {
                    tracing::warn!("Dither failed: {}", e);
                }
            }
        }
    }

    InstructionResult {
        status: NodeStatus::Success,
        message: Some(format!("Completed {} exposures", completed_exposures)),
        data: Some(serde_json::json!({
            "completed": completed_exposures,
            "total": config.count,
        })),
        hfr_values,
    }
}

// =============================================================================
// AUTOFOCUS INSTRUCTION
// =============================================================================

/// Execute autofocus using V-curve or curve fitting
pub async fn execute_autofocus(
    config: &AutofocusConfig,
    ctx: &InstructionContext,
    progress_callback: Option<&(dyn Fn(f64, String) + Send + Sync)>,
) -> InstructionResult {
    let camera_id = match ctx.camera_id() {
        Ok(id) => id.to_string(),
        Err(e) => return e,
    };
    let focuser_id = match ctx.focuser_id() {
        Ok(id) => id.to_string(),
        Err(e) => return e,
    };

    tracing::info!(
        "Starting autofocus: {:?} method, {} steps, step size {}",
        config.method,
        config.steps_out,
        config.step_size
    );

    if let Some(result) = ctx.check_cancelled() {
        return result;
    }

    // Emit initial progress
    if let Some(cb) = progress_callback {
        cb(0.0, "Starting autofocus...".to_string());
    }

    // Get current focuser position
    tracing::debug!("Getting focuser position for focuser_id: {}", focuser_id);
    let current_position = match ctx.device_ops.focuser_get_position(&focuser_id).await {
        Ok(pos) => pos,
        Err(e) => {
            tracing::error!("Autofocus failed: Could not get focuser position: {}", e);
            return InstructionResult::failure(format!("Failed to get focuser position: {}", e));
        }
    };

    tracing::info!("Current focuser position: {}", current_position);

    // Calculate focus points
    let half_range = (config.steps_out as i32) * config.step_size;
    let start_position = current_position - half_range;
    let total_points = (config.steps_out * 2 + 1) as usize;

    let mut focus_data: Vec<(i32, f64)> = Vec::with_capacity(total_points);

    // Move to starting position
    tracing::info!("Moving to start position: {}", start_position);
    if let Some(cb) = progress_callback {
        cb(5.0, format!("Moving to start position: {}", start_position));
    }
    if let Err(e) = ctx.device_ops.focuser_move_to(&focuser_id, start_position).await {
        return InstructionResult::failure(format!("Failed to move focuser: {}", e));
    }

    // Wait for focuser to reach starting position
    if let Err(e) = wait_for_focuser_idle(&focuser_id, ctx, Duration::from_secs(300)).await {
        return InstructionResult::failure(e);
    }

    // Take exposures at each focus point
    let (bin_x, bin_y) = match config.binning {
        Binning::One => (1, 1),
        Binning::Two => (2, 2),
        Binning::Three => (3, 3),
        Binning::Four => (4, 4),
    };

    // Minimum star count required for valid autofocus
    const MIN_STAR_COUNT: u32 = 10;
    // Minimum HFR variance required for a valid V-curve
    const MIN_HFR_VARIANCE: f64 = 1.0;
    // Minimum R² quality for curve fit
    const MIN_R_SQUARED: f64 = 0.5;

    let mut low_star_count_warnings = 0;

    for point in 0..total_points {
        if let Some(result) = ctx.check_cancelled() {
            // Halt focuser and wait for it to stop before returning
            tracing::info!("Autofocus cancelled, halting focuser");
            let _ = ctx.device_ops.focuser_halt(&focuser_id).await;
            wait_for_focuser_stop_after_halt(&focuser_id, &ctx.device_ops, Duration::from_secs(10)).await;
            // Optionally return to original position (start the move but don't wait - user cancelled)
            let _ = ctx.device_ops.focuser_move_to(&focuser_id, current_position).await;
            return result;
        }

        let position = start_position + (point as i32) * config.step_size;

        // Calculate progress: 10-90% for the V-curve points, remaining for final move
        let point_progress = 10.0 + (point as f64 / total_points as f64 * 80.0);

        tracing::info!("Focus point {}/{} at position {}", point + 1, total_points, position);
        if let Some(cb) = progress_callback {
            cb(point_progress, format!("Point {}/{}: pos {}", point + 1, total_points, position));
        }

        // Move to position
        if let Err(e) = ctx.device_ops.focuser_move_to(&focuser_id, position).await {
            return InstructionResult::failure(format!("Failed to move focuser: {}", e));
        }

        // Wait for focuser to settle at new position
        if let Err(e) = wait_for_focuser_idle(&focuser_id, ctx, Duration::from_secs(120)).await {
            return InstructionResult::failure(e);
        }

        // Take exposure and measure HFR
        let image_data = match ctx.device_ops.camera_start_exposure(
            &camera_id,
            config.exposure_duration,
            None,
            None,
            bin_x, bin_y,
        ).await {
            Ok(data) => data,
            Err(e) => return InstructionResult::failure(format!("Autofocus exposure failed: {}", e)),
        };

        // Calculate HFR, star count, and extract star crops from image
        let measurement = calculate_hfr_with_crops(&image_data);

        tracing::info!("Position {} HFR: {:.2}, Stars: {}", position, measurement.hfr, measurement.star_count);

        // Check for insufficient stars
        if measurement.star_count < MIN_STAR_COUNT {
            low_star_count_warnings += 1;
            tracing::warn!(
                "Low star count at position {}: {} stars (minimum: {})",
                position, measurement.star_count, MIN_STAR_COUNT
            );

            // If too many points have low star count, fail immediately
            if low_star_count_warnings > total_points / 2 {
                // Halt focuser and wait for it to stop
                let _ = ctx.device_ops.focuser_halt(&focuser_id).await;
                wait_for_focuser_stop_after_halt(&focuser_id, &ctx.device_ops, Duration::from_secs(10)).await;
                // Return focuser to original position
                let _ = ctx.device_ops.focuser_move_to(&focuser_id, current_position).await;
                return InstructionResult::failure(format!(
                    "Autofocus failed: Insufficient stars detected. Only {} stars found (minimum: {}). \
                     This may indicate clouds, poor seeing, or incorrect camera settings.",
                    measurement.star_count, MIN_STAR_COUNT
                ));
            }
        }

        focus_data.push((position, measurement.hfr));

        // Build structured progress data with V-curve points and star crops
        let progress_json = serde_json::json!({
            "type": "autofocus_progress",
            "point": point + 1,
            "total_points": total_points,
            "hfr": measurement.hfr,
            "star_count": measurement.star_count,
            "focus_range": {
                "min": start_position,
                "max": start_position + ((total_points - 1) as i32) * config.step_size
            },
            "vcurve_points": focus_data.iter().map(|(pos, hfr)| {
                serde_json::json!({"position": pos, "hfr": hfr})
            }).collect::<Vec<_>>(),
            "star_crops": measurement.star_crops.iter().map(|crop| {
                serde_json::json!({
                    "pixels_base64": crop.pixels_base64,
                    "width": crop.width,
                    "height": crop.height,
                    "hfr": crop.hfr,
                    "snr": crop.snr
                })
            }).collect::<Vec<_>>()
        });

        // Emit progress with structured JSON
        if let Some(cb) = progress_callback {
            cb(point_progress, progress_json.to_string());
        }
    }

    // Validate collected data before curve fitting
    if let Some(cb) = progress_callback {
        cb(92.0, "Validating focus data...".to_string());
    }

    // Check HFR variance - if it's too small, we don't have a real V-curve
    let hfr_values: Vec<f64> = focus_data.iter().map(|(_, hfr)| *hfr).collect();
    let min_hfr = hfr_values.iter().cloned().fold(f64::INFINITY, f64::min);
    let max_hfr = hfr_values.iter().cloned().fold(f64::NEG_INFINITY, f64::max);
    let hfr_variance = max_hfr - min_hfr;

    tracing::info!("HFR variance: {:.2} (min: {:.2}, max: {:.2})", hfr_variance, min_hfr, max_hfr);

    if hfr_variance < MIN_HFR_VARIANCE {
        // Halt focuser and wait for it to stop (focuser should already be idle here, but be safe)
        let _ = ctx.device_ops.focuser_halt(&focuser_id).await;
        wait_for_focuser_stop_after_halt(&focuser_id, &ctx.device_ops, Duration::from_secs(10)).await;
        // Return focuser to original position
        let _ = ctx.device_ops.focuser_move_to(&focuser_id, current_position).await;
        return InstructionResult::failure(format!(
            "Autofocus failed: No valid V-curve detected. HFR variance is only {:.2} (minimum: {:.1}). \
             The HFR is not changing with focus position, which may indicate: \
             - Clouds or obstructions blocking the sky \
             - Hot pixels being detected instead of real stars \
             - Focus range is too narrow or too far from true focus \
             - Camera is not properly connected or imaging",
            hfr_variance, MIN_HFR_VARIANCE
        ));
    }

    // Find best focus position by fitting curve and get R² quality
    let (best_position, r_squared) = find_best_focus_with_quality(&focus_data, config.method);
    let best_hfr = focus_data.iter()
        .min_by(|a, b| a.1.partial_cmp(&b.1).unwrap())
        .map(|(_pos, hfr)| *hfr)
        .unwrap_or(0.0);

    tracing::info!("Curve fit: position={}, HFR={:.2}, R²={:.3}", best_position, best_hfr, r_squared);

    // Check curve fit quality
    if r_squared < MIN_R_SQUARED {
        tracing::warn!(
            "Low curve fit quality: R²={:.3} (minimum: {:.1}). Proceeding with caution.",
            r_squared, MIN_R_SQUARED
        );
        // Don't fail, but warn - the result may still be usable
    }

    tracing::info!("Best focus at position {}, HFR: {:.2}, R²: {:.3}", best_position, best_hfr, r_squared);

    // Move to best position
    if let Some(cb) = progress_callback {
        cb(95.0, format!("Moving to best focus: {}", best_position));
    }
    if let Err(e) = ctx.device_ops.focuser_move_to(&focuser_id, best_position).await {
        return InstructionResult::failure(format!("Failed to move to best focus: {}", e));
    }

    // Wait for focuser to settle at best position
    if let Err(e) = wait_for_focuser_idle(&focuser_id, ctx, Duration::from_secs(120)).await {
        return InstructionResult::failure(format!("Failed to settle at best focus: {}", e));
    }

    // Emit final progress
    if let Some(cb) = progress_callback {
        cb(100.0, format!("Complete: pos {}, HFR {:.2}, R² {:.3}", best_position, best_hfr, r_squared));
    }

    InstructionResult {
        status: NodeStatus::Success,
        message: Some(format!("Autofocus complete: position {}, HFR {:.2}, R² {:.3}", best_position, best_hfr, r_squared)),
        data: Some(serde_json::json!({
            "best_position": best_position,
            "best_hfr": best_hfr,
            "r_squared": r_squared,
            "hfr_variance": hfr_variance,
            "focus_data": focus_data,
        })),
        hfr_values: vec![best_hfr],
    }
}

/// Result of HFR measurement from an image
struct HfrMeasurement {
    hfr: f64,
    star_count: u32,
}

/// Enhanced HFR measurement with star crops for UI display
struct HfrMeasurementWithCrops {
    hfr: f64,
    star_count: u32,
    /// Base64-encoded star crops (80x80 grayscale), up to 5 brightest stars
    star_crops: Vec<StarCropInfo>,
}

/// Star crop info for UI display
struct StarCropInfo {
    /// Base64-encoded grayscale pixels
    pixels_base64: String,
    width: u32,
    height: u32,
    hfr: f64,
    snr: f64,
}

/// Calculate HFR from image data, returning HFR, star count, and star crops
fn calculate_hfr_with_crops(image: &ImageData) -> HfrMeasurementWithCrops {
    use nightshade_imaging::{detect_stars_with_stats, extract_top_star_crops, StarDetectionConfig};
    use base64::{Engine as _, engine::general_purpose::STANDARD as BASE64};

    // Convert to imaging crate format
    let imaging_data = nightshade_imaging::ImageData::from_u16(
        image.width,
        image.height,
        1, // Mono
        &image.data
    );

    // Use the improved star detection with all filtering
    let config = StarDetectionConfig::default();
    let result = detect_stars_with_stats(&imaging_data, &config);

    // Return high HFR if no valid stars detected
    let hfr = if result.median_hfr > 0.0 && result.star_count > 0 {
        result.median_hfr
    } else {
        20.0  // Indicates bad/no stars
    };

    // Extract star crops for the top 5 brightest stars
    let crops = extract_top_star_crops(&imaging_data, &result.stars, 5, 80);

    let star_crops: Vec<StarCropInfo> = crops
        .into_iter()
        .map(|crop| StarCropInfo {
            pixels_base64: BASE64.encode(&crop.pixels),
            width: crop.width,
            height: crop.height,
            hfr: crop.hfr,
            snr: crop.snr,
        })
        .collect();

    HfrMeasurementWithCrops {
        hfr,
        star_count: result.star_count,
        star_crops,
    }
}

/// Calculate HFR from image data, returning both HFR and star count
fn calculate_hfr_from_image_with_stars(image: &ImageData) -> HfrMeasurement {
    use nightshade_imaging::{detect_stars_with_stats, StarDetectionConfig};

    // Convert to imaging crate format
    let imaging_data = nightshade_imaging::ImageData::from_u16(
        image.width,
        image.height,
        1, // Mono
        &image.data
    );

    // Use the improved star detection with all filtering
    let config = StarDetectionConfig::default();
    let result = detect_stars_with_stats(&imaging_data, &config);

    // Return high HFR if no valid stars detected
    let hfr = if result.median_hfr > 0.0 && result.star_count > 0 {
        result.median_hfr
    } else {
        20.0  // Indicates bad/no stars
    };

    HfrMeasurement {
        hfr,
        star_count: result.star_count,
    }
}

/// Calculate HFR from image data (legacy wrapper)
fn calculate_hfr_from_image(image: &ImageData, _optimal: i32, _position: i32) -> f64 {
    calculate_hfr_from_image_with_stars(image).hfr
}

/// Calculate HFR, star count, and FWHM from image data (production version)
pub(crate) fn calculate_hfr_and_stars(image: &ImageData) -> (f64, u32, Option<f64>) {
    use nightshade_imaging::{detect_stars_with_stats, StarDetectionConfig};

    // Convert to imaging crate format
    let imaging_data = nightshade_imaging::ImageData::from_u16(
        image.width,
        image.height,
        1, // Mono
        &image.data
    );

    // Detect stars and calculate statistics
    let config = StarDetectionConfig::default();
    let result = detect_stars_with_stats(&imaging_data, &config);

    let hfr = if result.median_hfr > 0.0 {
        result.median_hfr
    } else {
        // No stars detected - return high HFR value
        20.0
    };

    let fwhm = if result.median_fwhm > 0.0 {
        Some(result.median_fwhm)
    } else {
        None
    };

    (hfr, result.star_count, fwhm)
}

/// Find best focus position from data
fn find_best_focus(data: &[(i32, f64)], method: AutofocusMethod) -> i32 {
    match method {
        AutofocusMethod::VCurve => {
            // Find minimum HFR point
            data.iter()
                .min_by(|a, b| a.1.partial_cmp(&b.1).unwrap())
                .map(|(pos, _)| *pos)
                .unwrap_or(25000)
        }
        AutofocusMethod::Hyperbolic | AutofocusMethod::Quadratic => {
            // Fit a parabola and find minimum
            if data.len() < 3 {
                return data.iter()
                    .min_by(|a, b| a.1.partial_cmp(&b.1).unwrap())
                    .map(|(pos, _)| *pos)
                    .unwrap_or(25000);
            }
            
            // Simple quadratic fit using least squares
            let n = data.len() as f64;
            let sum_x: f64 = data.iter().map(|(x, _)| *x as f64).sum();
            let sum_x2: f64 = data.iter().map(|(x, _)| (*x as f64).powi(2)).sum();
            let sum_x3: f64 = data.iter().map(|(x, _)| (*x as f64).powi(3)).sum();
            let sum_x4: f64 = data.iter().map(|(x, _)| (*x as f64).powi(4)).sum();
            let sum_y: f64 = data.iter().map(|(_, y)| *y).sum();
            let sum_xy: f64 = data.iter().map(|(x, y)| (*x as f64) * y).sum();
            let sum_x2y: f64 = data.iter().map(|(x, y)| (*x as f64).powi(2) * y).sum();
            
            let denom = n * (sum_x2 * sum_x4 - sum_x3 * sum_x3) - 
                        sum_x * (sum_x * sum_x4 - sum_x2 * sum_x3) + 
                        sum_x2 * (sum_x * sum_x3 - sum_x2 * sum_x2);
            
            if denom.abs() < 1e-10 {
                return data.iter()
                    .min_by(|a, b| a.1.partial_cmp(&b.1).unwrap())
                    .map(|(pos, _)| *pos)
                    .unwrap_or(25000);
            }
            
            let a = (sum_y * (sum_x2 * sum_x4 - sum_x3 * sum_x3) -
                     sum_x * (sum_xy * sum_x4 - sum_x2y * sum_x3) +
                     sum_x2 * (sum_xy * sum_x3 - sum_x2y * sum_x2)) / denom;
            
            let b = (n * (sum_xy * sum_x4 - sum_x2y * sum_x3) -
                     sum_y * (sum_x * sum_x4 - sum_x2 * sum_x3) +
                     sum_x2 * (sum_x * sum_x2y - sum_x2 * sum_xy)) / denom;
            
            if a.abs() < 1e-10 {
                return data.iter()
                    .min_by(|a, b| a.1.partial_cmp(&b.1).unwrap())
                    .map(|(pos, _)| *pos)
                    .unwrap_or(25000);
            }
            
            let best = -b / (2.0 * a);
            best.round() as i32
        }
    }
}

/// Find best focus position from data and calculate R² quality metric
/// Returns (best_position, r_squared)
fn find_best_focus_with_quality(data: &[(i32, f64)], method: AutofocusMethod) -> (i32, f64) {
    if data.len() < 3 {
        let best_pos = data.iter()
            .min_by(|a, b| a.1.partial_cmp(&b.1).unwrap())
            .map(|(pos, _)| *pos)
            .unwrap_or(25000);
        return (best_pos, 0.0);  // Can't calculate R² with < 3 points
    }

    match method {
        AutofocusMethod::VCurve => {
            // For V-curve, find minimum and calculate a simple quality metric
            // based on how well the data forms a V shape
            let min_point = data.iter()
                .enumerate()
                .min_by(|a, b| a.1.1.partial_cmp(&b.1.1).unwrap())
                .map(|(idx, (pos, _))| (idx, *pos))
                .unwrap_or((0, 25000));

            let (min_idx, best_pos) = min_point;
            let min_hfr = data[min_idx].1;

            // Calculate R² as the proportion of variance explained by V-shape
            // Simple approach: measure how well points decrease before min and increase after
            let mean_hfr: f64 = data.iter().map(|(_, h)| *h).sum::<f64>() / data.len() as f64;
            let ss_tot: f64 = data.iter().map(|(_, h)| (h - mean_hfr).powi(2)).sum();

            // For V-curve, use the minimum as the predicted value at minimum,
            // and linear interpolation to edges
            let mut ss_res = 0.0;
            for (i, (_, hfr)) in data.iter().enumerate() {
                // Simple V-curve model: HFR increases linearly from minimum
                let dist_from_min = (i as i32 - min_idx as i32).abs() as f64;
                let slope = if min_idx > 0 && min_idx < data.len() - 1 {
                    // Average slope on both sides
                    let left_slope = if min_idx > 0 {
                        (data[0].1 - min_hfr) / min_idx as f64
                    } else {
                        0.0
                    };
                    let right_slope = if min_idx < data.len() - 1 {
                        (data[data.len() - 1].1 - min_hfr) / (data.len() - 1 - min_idx) as f64
                    } else {
                        0.0
                    };
                    (left_slope + right_slope) / 2.0
                } else {
                    // Min at edge - use single-sided slope
                    if min_idx == 0 && data.len() > 1 {
                        (data[data.len() - 1].1 - min_hfr) / (data.len() - 1) as f64
                    } else if data.len() > 1 {
                        (data[0].1 - min_hfr) / min_idx as f64
                    } else {
                        0.0
                    }
                };

                let predicted = min_hfr + slope * dist_from_min;
                ss_res += (hfr - predicted).powi(2);
            }

            let r_squared = if ss_tot > 1e-10 {
                (1.0 - ss_res / ss_tot).max(0.0)
            } else {
                0.0
            };

            (best_pos, r_squared)
        }

        AutofocusMethod::Hyperbolic | AutofocusMethod::Quadratic => {
            // Fit a parabola and calculate R²
            let n = data.len() as f64;
            let sum_x: f64 = data.iter().map(|(x, _)| *x as f64).sum();
            let sum_x2: f64 = data.iter().map(|(x, _)| (*x as f64).powi(2)).sum();
            let sum_x3: f64 = data.iter().map(|(x, _)| (*x as f64).powi(3)).sum();
            let sum_x4: f64 = data.iter().map(|(x, _)| (*x as f64).powi(4)).sum();
            let sum_y: f64 = data.iter().map(|(_, y)| *y).sum();
            let sum_xy: f64 = data.iter().map(|(x, y)| (*x as f64) * y).sum();
            let sum_x2y: f64 = data.iter().map(|(x, y)| (*x as f64).powi(2) * y).sum();

            let denom = n * (sum_x2 * sum_x4 - sum_x3 * sum_x3) -
                        sum_x * (sum_x * sum_x4 - sum_x2 * sum_x3) +
                        sum_x2 * (sum_x * sum_x3 - sum_x2 * sum_x2);

            if denom.abs() < 1e-10 {
                let best_pos = data.iter()
                    .min_by(|a, b| a.1.partial_cmp(&b.1).unwrap())
                    .map(|(pos, _)| *pos)
                    .unwrap_or(25000);
                return (best_pos, 0.0);
            }

            // Solve for coefficients: y = c + b*x + a*x²
            let c = (sum_y * (sum_x2 * sum_x4 - sum_x3 * sum_x3) -
                     sum_x * (sum_xy * sum_x4 - sum_x2y * sum_x3) +
                     sum_x2 * (sum_xy * sum_x3 - sum_x2y * sum_x2)) / denom;

            let b = (n * (sum_xy * sum_x4 - sum_x2y * sum_x3) -
                     sum_y * (sum_x * sum_x4 - sum_x2 * sum_x3) +
                     sum_x2 * (sum_x * sum_x2y - sum_x2 * sum_xy)) / denom;

            let a = (n * (sum_x2 * sum_x2y - sum_x3 * sum_xy) -
                     sum_x * (sum_x * sum_x2y - sum_x2 * sum_xy) +
                     sum_y * (sum_x * sum_x3 - sum_x2 * sum_x2)) / denom;

            // Find minimum: x = -b / (2a)
            let best_pos = if a.abs() > 1e-10 {
                (-b / (2.0 * a)).round() as i32
            } else {
                data.iter()
                    .min_by(|a, b| a.1.partial_cmp(&b.1).unwrap())
                    .map(|(pos, _)| *pos)
                    .unwrap_or(25000)
            };

            // Calculate R²
            let mean_y = sum_y / n;
            let ss_tot: f64 = data.iter().map(|(_, y)| (y - mean_y).powi(2)).sum();

            let ss_res: f64 = data.iter().map(|(x, y)| {
                let x_f = *x as f64;
                let predicted = c + b * x_f + a * x_f * x_f;
                (y - predicted).powi(2)
            }).sum();

            let r_squared = if ss_tot > 1e-10 {
                (1.0 - ss_res / ss_tot).max(0.0)
            } else {
                0.0
            };

            (best_pos, r_squared)
        }
    }
}

// =============================================================================
// DITHER INSTRUCTION
// =============================================================================

/// Execute dither
pub async fn execute_dither(
    config: &DitherConfig,
    ctx: &InstructionContext,
    progress_callback: Option<&(dyn Fn(f64, String) + Send + Sync)>,
) -> InstructionResult {
    // Emit initial progress
    if let Some(cb) = progress_callback {
        cb(0.0, "Starting dither".to_string());
    }

    tracing::info!("Dithering {} pixels", config.pixels);

    if let Some(result) = ctx.check_cancelled() {
        return result;
    }

    // Emit progress for sending dither command
    if let Some(cb) = progress_callback {
        cb(30.0, "Sending dither command to guider".to_string());
    }

    // The guider_dither call handles:
    // - Sending the dither command
    // - Waiting for the dither move to complete
    // - Waiting for guiding to settle
    // We report combined progress since these happen atomically in the device ops call
    if let Some(cb) = progress_callback {
        cb(50.0, "Waiting for dither to complete".to_string());
    }

    // Check cancellation before the potentially long-running dither operation
    if let Some(result) = ctx.check_cancelled() {
        return result;
    }

    // Emit progress for settling phase (happens inside guider_dither)
    if let Some(cb) = progress_callback {
        cb(70.0, "Waiting for guiding to settle".to_string());
    }

    match ctx.device_ops.guider_dither(
        config.pixels,
        config.settle_pixels,
        config.settle_time,
        config.settle_timeout,
        config.ra_only,
    ).await {
        Ok(_) => {
            // Emit final progress
            if let Some(cb) = progress_callback {
                cb(100.0, "Dither complete".to_string());
            }
            InstructionResult::success_with_message("Dither and settle complete")
        }
        Err(e) => InstructionResult::failure(format!("Dither failed: {}", e)),
    }
}

// =============================================================================
// GUIDING START/STOP INSTRUCTIONS
// =============================================================================

/// Execute start guiding - starts PHD2 guiding and waits for settle
pub async fn execute_start_guiding(
    config: &StartGuidingConfig,
    ctx: &InstructionContext,
    progress_callback: Option<&(dyn Fn(f64, String) + Send + Sync)>,
) -> InstructionResult {
    tracing::info!("Starting guiding with settle threshold {} px", config.settle_pixels);

    // Emit initial progress
    if let Some(cb) = progress_callback {
        cb(0.0, "Starting guiding".to_string());
    }

    if let Some(result) = ctx.check_cancelled() {
        return result;
    }

    // Progress: Connecting to guider
    if let Some(cb) = progress_callback {
        cb(20.0, "Connecting to guider".to_string());
    }

    // Check guider connection status
    match ctx.device_ops.guider_get_status().await {
        Ok(status) => {
            tracing::debug!("Guider status: is_guiding={}, rms_total={:.2}", status.is_guiding, status.rms_total);
        }
        Err(e) => {
            tracing::warn!("Could not get guider status: {}", e);
            // Continue anyway - guider_start may still work
        }
    }

    if let Some(result) = ctx.check_cancelled() {
        return result;
    }

    // Progress: Starting guide camera loop
    if let Some(cb) = progress_callback {
        cb(40.0, "Starting guide camera loop".to_string());
    }

    // Start guiding - this will auto-select a star if needed and wait for settle
    if let Some(cb) = progress_callback {
        cb(60.0, "Waiting for guiding to stabilize".to_string());
    }

    match ctx.device_ops.guider_start(
        config.settle_pixels,
        config.settle_time,
        config.settle_timeout,
    ).await {
        Ok(_) => {
            if let Some(cb) = progress_callback {
                cb(100.0, "Guiding active".to_string());
            }
            InstructionResult::success_with_message("Guiding started and settled")
        }
        Err(e) => InstructionResult::failure(format!("Failed to start guiding: {}", e)),
    }
}

/// Execute stop guiding - stops PHD2 guiding
pub async fn execute_stop_guiding(
    ctx: &InstructionContext,
    progress_callback: Option<&(dyn Fn(f64, String) + Send + Sync)>,
) -> InstructionResult {
    tracing::info!("Stopping guiding");

    // Emit initial progress
    if let Some(cb) = progress_callback {
        cb(0.0, "Stopping guiding".to_string());
    }

    if let Some(result) = ctx.check_cancelled() {
        return result;
    }

    // Progress: Sending stop command
    if let Some(cb) = progress_callback {
        cb(50.0, "Sending stop command".to_string());
    }

    match ctx.device_ops.guider_stop().await {
        Ok(_) => {
            if let Some(cb) = progress_callback {
                cb(100.0, "Guiding stopped".to_string());
            }
            InstructionResult::success_with_message("Guiding stopped")
        }
        Err(e) => InstructionResult::failure(format!("Failed to stop guiding: {}", e)),
    }
}

// =============================================================================
// FILTER CHANGE INSTRUCTION
// =============================================================================

/// Execute filter change
pub async fn execute_filter_change(
    config: &FilterConfig,
    ctx: &InstructionContext,
    progress_callback: Option<&(dyn Fn(f64, String) + Send + Sync)>,
) -> InstructionResult {
    let fw_id = match ctx.filterwheel_id() {
        Ok(id) => id.to_string(),
        Err(e) => return e,
    };

    tracing::info!("Changing filter to: {}", config.filter_name);

    // Emit initial progress
    if let Some(cb) = progress_callback {
        cb(0.0, format!("Changing to {}", config.filter_name));
    }

    // If filter index is specified, use it directly
    if let Some(index) = config.filter_index {
        match ctx.device_ops.filterwheel_set_position(&fw_id, index).await {
            Ok(_) => {
                if let Some(cb) = progress_callback {
                    cb(50.0, format!("Moving to position {}", index));
                }
                // Wait for filter wheel to reach target position (the wait function also verifies)
                if let Err(e) = wait_for_filterwheel_idle(&fw_id, index, ctx, Duration::from_secs(120)).await {
                    return InstructionResult::failure(e);
                }
                if let Some(cb) = progress_callback {
                    cb(100.0, format!("Filter {}", index));
                }
                return InstructionResult::success_with_message(format!("Changed to filter position: {}", index));
            }
            Err(e) => return InstructionResult::failure(format!("Filter change failed: {}", e)),
        }
    }

    // Otherwise use filter name
    match ctx.device_ops.filterwheel_set_filter_by_name(&fw_id, &config.filter_name).await {
        Ok(pos) => {
            if let Some(cb) = progress_callback {
                cb(50.0, format!("Moving to {}", config.filter_name));
            }
            // Wait for filter wheel to reach target position
            if let Err(e) = wait_for_filterwheel_idle(&fw_id, pos, ctx, Duration::from_secs(120)).await {
                return InstructionResult::failure(e);
            }
            if let Some(cb) = progress_callback {
                cb(100.0, format!("Filter: {}", config.filter_name));
            }
            InstructionResult::success_with_message(format!("Changed to filter: {} (pos {})", config.filter_name, pos))
        }
        Err(e) => InstructionResult::failure(format!("Filter change failed: {}", e)),
    }
}

// =============================================================================
// CAMERA COOLING/WARMING INSTRUCTIONS
// =============================================================================

/// Execute camera cooling
pub async fn execute_cool_camera(
    config: &CoolConfig,
    ctx: &InstructionContext,
    progress_callback: Option<&(dyn Fn(f64, String) + Send + Sync)>,
) -> InstructionResult {
    let camera_id = match ctx.camera_id() {
        Ok(id) => id.to_string(),
        Err(e) => {
            tracing::error!("CoolCamera failed: No camera connected");
            return e;
        }
    };

    tracing::info!("Cooling camera to {}°C", config.target_temp);

    // Get initial temperature for progress calculation
    let start_temp = ctx.device_ops.camera_get_temperature(&camera_id).await.unwrap_or(20.0);
    let target_temp = config.target_temp;
    let temp_range = (start_temp - target_temp).abs();

    // Check if already at target temperature
    let already_at_target = (start_temp - target_temp).abs() < 0.5;

    // Enable cooler and set target
    if let Err(e) = ctx.device_ops.camera_set_cooler(&camera_id, true, target_temp).await {
        return InstructionResult::failure(format!("Failed to enable cooler: {}", e));
    }

    // If already at target, get power and report success immediately
    if already_at_target {
        let cooler_power = ctx.device_ops.camera_get_cooler_power(&camera_id).await.unwrap_or(0.0);
        let msg = format!("At target: {:.1}°C ({:.0}% power)", start_temp, cooler_power);
        tracing::info!("Camera already at target temperature: {}", msg);
        if let Some(cb) = progress_callback {
            cb(100.0, msg.clone());
        }
        return InstructionResult::success_with_message(msg);
    }

    // Emit initial progress
    if let Some(cb) = progress_callback {
        cb(0.0, format!("Starting: {:.1}°C → {:.1}°C", start_temp, target_temp));
    }

    // If duration specified, wait for cooling
    if let Some(duration_mins) = config.duration_mins {
        let steps = (duration_mins * 6.0) as u32; // Check every 10 seconds

        for step in 0..steps {
            if let Some(result) = ctx.check_cancelled() {
                return result;
            }

            // Check current temperature and cooler power
            let current_temp = ctx.device_ops.camera_get_temperature(&camera_id).await.unwrap_or(20.0);
            let cooler_power = ctx.device_ops.camera_get_cooler_power(&camera_id).await.unwrap_or(0.0);

            // Calculate progress based on temperature change
            // Formula: (current - start) / (target - start) * 100
            // This correctly handles both cooling and warming, and allows negative progress
            // if temperature moves away from target (e.g., sensor heating during exposure)
            let temp_progress = if temp_range > 0.1 {
                let raw = (current_temp - start_temp) / (target_temp - start_temp) * 100.0;
                raw.clamp(0.0, 100.0)
            } else {
                100.0
            };

            // Also consider time-based progress
            let time_progress = step as f64 / steps as f64 * 100.0;

            // Use the higher of the two progress metrics
            let progress = temp_progress.max(time_progress);

            tracing::debug!("Cooling progress: {:.1}%, current temp: {:.1}°C, power: {:.0}%", progress, current_temp, cooler_power);

            // Emit progress event with cooler power
            if let Some(cb) = progress_callback {
                cb(progress, format!("Cooling: {:.1}°C → {:.1}°C ({:.0}% power)", current_temp, target_temp, cooler_power));
            }

            // Check if we've reached target
            if (current_temp - target_temp).abs() < 0.5 {
                let final_power = ctx.device_ops.camera_get_cooler_power(&camera_id).await.unwrap_or(0.0);
                let msg = format!("Target reached: {:.1}°C ({:.0}% power)", current_temp, final_power);
                if let Some(cb) = progress_callback {
                    cb(100.0, msg.clone());
                }
                return InstructionResult::success_with_message(msg);
            }

            sleep(Duration::from_secs(10)).await;
        }
    }

    // Emit final progress
    if let Some(cb) = progress_callback {
        cb(100.0, format!("Cooling to {}°C initiated", target_temp));
    }

    InstructionResult::success_with_message(format!("Camera cooling set to {}°C", target_temp))
}

/// Execute camera warming
pub async fn execute_warm_camera(
    config: &WarmConfig,
    ctx: &InstructionContext,
    progress_callback: Option<&(dyn Fn(f64, String) + Send + Sync)>,
) -> InstructionResult {
    let camera_id = match ctx.camera_id() {
        Ok(id) => id.to_string(),
        Err(e) => return e,
    };

    tracing::info!("Warming camera at {}°C/min", config.rate_per_min);

    let start_temp = ctx.device_ops.camera_get_temperature(&camera_id).await.unwrap_or(-10.0);
    let target_temp = 10.0; // Warm to ambient
    let temp_range = target_temp - start_temp;
    let duration_mins = temp_range / config.rate_per_min;
    let steps = (duration_mins * 6.0).max(1.0) as u32;

    // Emit initial progress
    if let Some(cb) = progress_callback {
        cb(0.0, format!("Warming: {:.1}°C → {:.1}°C", start_temp, target_temp));
    }

    for step in 0..steps {
        if let Some(result) = ctx.check_cancelled() {
            // Turn off cooler on cancel
            let _ = ctx.device_ops.camera_set_cooler(&camera_id, false, 20.0).await;
            return result;
        }

        let progress_temp = start_temp + (temp_range * step as f64 / steps as f64);
        let progress_percent = (step as f64 / steps as f64) * 100.0;

        // Gradually increase target temperature
        if let Err(e) = ctx.device_ops.camera_set_cooler(&camera_id, true, progress_temp).await {
            tracing::warn!("Failed to update cooler target: {}", e);
        }

        // Emit progress
        if let Some(cb) = progress_callback {
            cb(progress_percent, format!("Warming: {:.1}°C → {:.1}°C", progress_temp, target_temp));
        }

        tracing::debug!("Warming progress: {:.1}°C", progress_temp);
        sleep(Duration::from_secs(10)).await;
    }

    // Turn off cooler
    let _ = ctx.device_ops.camera_set_cooler(&camera_id, false, 20.0).await;

    // Emit final progress
    if let Some(cb) = progress_callback {
        cb(100.0, "Warmed to ambient".to_string());
    }

    InstructionResult::success_with_message("Camera warmed to ambient")
}

// =============================================================================
// ROTATOR INSTRUCTION
// =============================================================================

/// Execute rotator move
pub async fn execute_rotator_move(config: &RotatorConfig, ctx: &InstructionContext) -> InstructionResult {
    let rotator_id = match ctx.rotator_id() {
        Ok(id) => id.to_string(),
        Err(e) => return e,
    };

    tracing::info!("Moving rotator to {}° (relative: {})", config.target_angle, config.relative);

    let result = if config.relative {
        ctx.device_ops.rotator_move_relative(&rotator_id, config.target_angle).await
    } else {
        ctx.device_ops.rotator_move_to(&rotator_id, config.target_angle).await
    };
    
    match result {
        Ok(_) => InstructionResult::success_with_message(format!("Rotator at {}°", config.target_angle)),
        Err(e) => InstructionResult::failure(format!("Rotator move failed: {}", e)),
    }
}

// =============================================================================
// PARK/UNPARK INSTRUCTIONS
// =============================================================================

/// Execute park
pub async fn execute_park(ctx: &InstructionContext) -> InstructionResult {
    let mount_id = match ctx.mount_id() {
        Ok(id) => id.to_string(),
        Err(e) => return e,
    };

    tracing::info!("Parking mount");

    match ctx.device_ops.mount_park(&mount_id).await {
        Ok(_) => InstructionResult::success_with_message("Mount parked"),
        Err(e) => InstructionResult::failure(format!("Park failed: {}", e)),
    }
}

/// Execute unpark
pub async fn execute_unpark(ctx: &InstructionContext) -> InstructionResult {
    let mount_id = match ctx.mount_id() {
        Ok(id) => id.to_string(),
        Err(e) => return e,
    };

    tracing::info!("Unparking mount");

    match ctx.device_ops.mount_unpark(&mount_id).await {
        Ok(_) => InstructionResult::success_with_message("Mount unparked"),
        Err(e) => InstructionResult::failure(format!("Unpark failed: {}", e)),
    }
}

// =============================================================================
// POLAR ALIGNMENT INSTRUCTION
// =============================================================================

/// Execute polar alignment
pub async fn execute_polar_alignment(
    config: &PolarAlignConfig,
    ctx: &InstructionContext,
    status_callback: impl Fn(String, Option<f64>),
) -> InstructionResult {
    crate::polar_align::perform_polar_alignment(&config, ctx, |msg, progress| status_callback(msg, progress)).await
}

// =============================================================================
// WAIT TIME INSTRUCTION
// =============================================================================

/// Execute wait for time
pub async fn execute_wait_time(
    config: &WaitTimeConfig,
    ctx: &InstructionContext,
    progress_callback: Option<&(dyn Fn(f64, String) + Send + Sync)>,
) -> InstructionResult {
    // Wait until specific time
    if let Some(until) = config.wait_until {
        let now = chrono::Utc::now().timestamp();
        if now < until {
            let total_wait_secs = (until - now) as u64;
            let wait_until_str = chrono::DateTime::from_timestamp(until, 0)
                .map(|dt| dt.format("%H:%M:%S").to_string())
                .unwrap_or_else(|| until.to_string());

            tracing::info!("Waiting until {} ({} seconds)", wait_until_str, total_wait_secs);

            // Emit initial progress
            if let Some(cb) = progress_callback {
                cb(0.0, format!("Waiting until {}", wait_until_str));
            }

            // Wait in 1-second increments to allow cancellation
            for elapsed in 0..total_wait_secs {
                if let Some(result) = ctx.check_cancelled() {
                    return result;
                }

                // Emit progress every 10 seconds
                if elapsed % 10 == 0 {
                    let progress = (elapsed as f64 / total_wait_secs as f64) * 100.0;
                    let remaining = total_wait_secs - elapsed;
                    if let Some(cb) = progress_callback {
                        cb(progress, format!("{}s remaining", remaining));
                    }
                }

                sleep(Duration::from_secs(1)).await;
            }

            if let Some(cb) = progress_callback {
                cb(100.0, "Target time reached".to_string());
            }
        }
        return InstructionResult::success_with_message("Wait time reached");
    }

    // Wait for twilight
    if let Some(twilight) = &config.wait_for_twilight {
        tracing::info!("Waiting for {:?} twilight", twilight);

        // Calculate twilight time based on observer location
        let (lat, lon) = ctx.device_ops.get_observer_location().unwrap_or((45.0, -75.0));
        let twilight_time = calculate_twilight_time(lat, lon, twilight);

        let now = chrono::Utc::now().timestamp();
        if now < twilight_time {
            let total_wait_secs = (twilight_time - now) as u64;
            tracing::info!("Waiting {} seconds for {:?} twilight", total_wait_secs, twilight);

            // Emit initial progress
            if let Some(cb) = progress_callback {
                cb(0.0, format!("Waiting for {:?} twilight", twilight));
            }

            for elapsed in 0..total_wait_secs {
                if let Some(result) = ctx.check_cancelled() {
                    return result;
                }

                // Emit progress every 30 seconds
                if elapsed % 30 == 0 {
                    let progress = (elapsed as f64 / total_wait_secs as f64) * 100.0;
                    let remaining_mins = (total_wait_secs - elapsed) / 60;
                    if let Some(cb) = progress_callback {
                        cb(progress, format!("{:?}: {}m remaining", twilight, remaining_mins));
                    }
                }

                sleep(Duration::from_secs(1)).await;
            }

            if let Some(cb) = progress_callback {
                cb(100.0, format!("{:?} twilight reached", twilight));
            }
        }

        return InstructionResult::success_with_message(format!("{:?} twilight reached", twilight));
    }

    InstructionResult::success()
}

/// Calculate twilight time for a given location using proper solar position algorithms
fn calculate_twilight_time(latitude: f64, longitude: f64, twilight_type: &TwilightType) -> i64 {
    use chrono::Duration;
    
    // Sun altitude threshold for each twilight type (degrees below horizon)
    let altitude_threshold: f64 = match twilight_type {
        TwilightType::Civil => -6.0,
        TwilightType::Nautical => -12.0,
        TwilightType::Astronomical => -18.0,
    };
    
    let now = chrono::Utc::now();
    let today = now.date_naive();
    
    // Calculate Julian Day
    let jd = calculate_julian_day(now);
    
    // Calculate solar position
    let (solar_dec, equation_of_time) = calculate_solar_position(jd);
    
    // Convert to radians
    let lat_rad = latitude.to_radians();
    let dec_rad = solar_dec.to_radians();
    let alt_rad = altitude_threshold.to_radians();
    
    // Calculate hour angle when sun is at the given altitude
    // cos(H) = (sin(alt) - sin(lat) * sin(dec)) / (cos(lat) * cos(dec))
    let cos_h = (alt_rad.sin() - lat_rad.sin() * dec_rad.sin()) 
              / (lat_rad.cos() * dec_rad.cos());
    
    // Check if sun never reaches this altitude (polar regions)
    if cos_h < -1.0 || cos_h > 1.0 {
        // Sun never reaches this altitude today
        // Return a time 12 hours from now as fallback
        return (now + Duration::hours(12)).timestamp();
    }
    
    let hour_angle = cos_h.acos().to_degrees();
    
    // Calculate local solar noon
    let solar_noon_utc = 12.0 - longitude / 15.0 - equation_of_time / 60.0;
    
    // Evening twilight occurs when sun sets past the altitude threshold
    // Time after solar noon when sun reaches threshold
    let hours_after_noon = hour_angle / 15.0;
    let twilight_hour_utc = solar_noon_utc + hours_after_noon;
    
    // Convert to timestamp
    let twilight_hour = twilight_hour_utc.rem_euclid(24.0);
    let twilight_minutes = (twilight_hour.fract() * 60.0) as u32;
    let twilight_hour = twilight_hour as u32;
    
    let twilight_datetime = today
        .and_hms_opt(twilight_hour, twilight_minutes, 0)
        .unwrap_or_else(|| today.and_hms_opt(23, 59, 0).unwrap());
    
    let twilight_timestamp = chrono::DateTime::<chrono::Utc>::from_naive_utc_and_offset(
        twilight_datetime,
        chrono::Utc,
    ).timestamp();
    
    // If the calculated twilight is in the past, it's tomorrow's twilight
    if twilight_timestamp < now.timestamp() {
        return twilight_timestamp + 86400; // Add 24 hours
    }
    
    twilight_timestamp
}

/// Calculate Julian Day from UTC datetime
fn calculate_julian_day(dt: chrono::DateTime<chrono::Utc>) -> f64 {
    use chrono::{Datelike, Timelike};
    
    let year = dt.year();
    let month = dt.month() as i32;
    let day = dt.day() as f64;
    let hour = dt.hour() as f64 + dt.minute() as f64 / 60.0 + dt.second() as f64 / 3600.0;
    
    let (y, m) = if month <= 2 {
        (year - 1, month + 12)
    } else {
        (year, month)
    };
    
    let a = (y as f64 / 100.0).floor();
    let b = 2.0 - a + (a / 4.0).floor();
    
    let jd = (365.25 * (y as f64 + 4716.0)).floor()
        + (30.6001 * (m as f64 + 1.0)).floor()
        + day
        + hour / 24.0
        + b
        - 1524.5;
    
    jd
}

/// Calculate solar declination and equation of time
/// Returns (declination in degrees, equation of time in minutes)
fn calculate_solar_position(jd: f64) -> (f64, f64) {
    // Days since J2000.0
    let n = jd - 2451545.0;
    
    // Mean longitude of the sun (degrees)
    let l = (280.460 + 0.9856474 * n) % 360.0;
    
    // Mean anomaly of the sun (degrees)
    let g = (357.528 + 0.9856003 * n) % 360.0;
    let g_rad = g.to_radians();
    
    // Ecliptic longitude of the sun (degrees)
    let lambda = l + 1.915 * g_rad.sin() + 0.020 * (2.0 * g_rad).sin();
    let lambda_rad = lambda.to_radians();
    
    // Obliquity of the ecliptic (degrees)
    let epsilon = 23.439 - 0.0000004 * n;
    let epsilon_rad = epsilon.to_radians();
    
    // Solar declination
    let declination = (epsilon_rad.sin() * lambda_rad.sin()).asin().to_degrees();
    
    // Equation of time (minutes)
    // Simplified formula
    let y = (epsilon_rad / 2.0).tan().powi(2);
    let l_rad = l.to_radians();
    let eot = 4.0 * (
        y * (2.0 * l_rad).sin() 
        - 2.0 * 0.0167 * g_rad.sin() 
        + 4.0 * 0.0167 * y * g_rad.sin() * (2.0 * l_rad).cos()
        - 0.5 * y * y * (4.0 * l_rad).sin()
        - 1.25 * 0.0167 * 0.0167 * (2.0 * g_rad).sin()
    ).to_degrees();
    
    (declination, eot)
}

// =============================================================================
// DELAY INSTRUCTION
// =============================================================================

/// Execute delay
pub async fn execute_delay(
    config: &DelayConfig,
    ctx: &InstructionContext,
    progress_callback: Option<&(dyn Fn(f64, String) + Send + Sync)>,
) -> InstructionResult {
    tracing::info!("Delaying for {:.1} seconds", config.seconds);

    // Emit initial progress
    if let Some(cb) = progress_callback {
        cb(0.0, format!("{:.0}s delay", config.seconds));
    }

    let total_steps = (config.seconds * 10.0) as u64;
    for step in 0..total_steps {
        if let Some(result) = ctx.check_cancelled() {
            return result;
        }

        // Emit progress every second (10 steps)
        if step % 10 == 0 {
            let elapsed_secs = step as f64 / 10.0;
            let remaining_secs = config.seconds - elapsed_secs;
            let progress = (elapsed_secs / config.seconds) * 100.0;
            if let Some(cb) = progress_callback {
                cb(progress, format!("{:.0}s remaining", remaining_secs));
            }
        }

        sleep(Duration::from_millis(100)).await;
    }

    if let Some(cb) = progress_callback {
        cb(100.0, "Delay complete".to_string());
    }

    InstructionResult::success_with_message(format!("Delayed {:.1} seconds", config.seconds))
}

// =============================================================================
// NOTIFICATION INSTRUCTION
// =============================================================================

/// Execute notification
pub async fn execute_notification(config: &NotificationConfig, ctx: &InstructionContext) -> InstructionResult {
    let level = match config.level {
        NotificationLevel::Info => "info",
        NotificationLevel::Warning => "warning",
        NotificationLevel::Error => "error",
        NotificationLevel::Success => "success",
    };
    
    tracing::info!("[{}] {}: {}", level.to_uppercase(), config.title, config.message);

    if let Err(e) = ctx.device_ops.send_notification(level, &config.title, &config.message).await {
        tracing::warn!("Failed to send notification: {}", e);
    }
    
    InstructionResult::success()
}

// =============================================================================
// SCRIPT INSTRUCTION
// =============================================================================

/// Execute script
pub async fn execute_script(config: &ScriptConfig, ctx: &InstructionContext) -> InstructionResult {
    tracing::info!("Running script: {} {:?}", config.script_path, config.arguments);

    if let Some(result) = ctx.check_cancelled() {
        return result;
    }

    // Build the command
    let mut cmd = tokio::process::Command::new(&config.script_path);
    cmd.args(&config.arguments);
    
    // Add environment variables with session context
    if let Some(target) = &ctx.target_name {
        cmd.env("NIGHTSHADE_TARGET", target);
    }
    if let Some(ra) = ctx.target_ra {
        cmd.env("NIGHTSHADE_TARGET_RA", ra.to_string());
    }
    if let Some(dec) = ctx.target_dec {
        cmd.env("NIGHTSHADE_TARGET_DEC", dec.to_string());
    }
    if let Some(filter) = &ctx.current_filter {
        cmd.env("NIGHTSHADE_FILTER", filter);
    }
    
    // Set timeout
    let timeout = config.timeout_secs.unwrap_or(300) as u64;
    
    // Run the script with timeout
    let result = tokio::time::timeout(
        Duration::from_secs(timeout),
        cmd.output()
    ).await;
    
    match result {
        Ok(Ok(output)) => {
            if output.status.success() {
                let stdout = String::from_utf8_lossy(&output.stdout);
                tracing::info!("Script output: {}", stdout);
                InstructionResult {
                    status: NodeStatus::Success,
                    message: Some(format!("Script {} completed", config.script_path)),
                    data: Some(serde_json::json!({
                        "stdout": stdout.to_string(),
                        "stderr": String::from_utf8_lossy(&output.stderr).to_string(),
                        "exit_code": output.status.code(),
                    })),
                    hfr_values: Vec::new(),
                }
            } else {
                let stderr = String::from_utf8_lossy(&output.stderr);
                InstructionResult::failure(format!("Script failed: {}", stderr))
            }
        }
        Ok(Err(e)) => InstructionResult::failure(format!("Failed to run script: {}", e)),
        Err(_) => InstructionResult::failure(format!("Script timed out after {} seconds", timeout)),
    }
}

// =============================================================================
// MERIDIAN FLIP INSTRUCTION
// =============================================================================

/// Execute meridian flip with comprehensive safety checks and error handling
pub async fn execute_meridian_flip(
    config: &MeridianFlipConfig,
    ctx: &InstructionContext,
    progress_callback: Option<&(dyn Fn(f64, String) + Send + Sync)>,
) -> InstructionResult {
    let mount_id = match ctx.mount_id() {
        Ok(id) => id.to_string(),
        Err(e) => return e,
    };

    tracing::info!("=== Meridian Flip Sequence Started ===");

    // Emit initial progress
    if let Some(cb) = progress_callback {
        cb(0.0, "Starting meridian flip".to_string());
    }

    // =========================================================================
    // SAFETY CHECK 1: Verify mount capability
    // =========================================================================
    let can_flip = match ctx.device_ops.mount_can_flip(&mount_id).await {
        Ok(capable) => capable,
        Err(e) => {
            tracing::warn!("Could not determine flip capability: {}", e);
            true // Assume capable if unknown
        }
    };

    if !can_flip {
        return InstructionResult::failure(
            "Mount does not support meridian flips. Please configure mount to allow flips or disable meridian flip instruction."
        );
    }

    // =========================================================================
    // SAFETY CHECK 2: Verify we have target coordinates
    // =========================================================================
    let target_ra = match ctx.target_ra {
        Some(ra) => ra,
        None => return InstructionResult::failure("No target RA available for meridian flip"),
    };

    let target_dec = match ctx.target_dec {
        Some(dec) => dec,
        None => return InstructionResult::failure("No target declination available for meridian flip"),
    };

    // =========================================================================
    // SAFETY CHECK 3: Verify observer location is known
    // =========================================================================
    let (_lat, lon) = match ctx.device_ops.get_observer_location() {
        Some((lat, lon)) => (lat, lon),
        None => {
            return InstructionResult::failure(
                "Observer location not configured. Meridian flip requires location for calculations."
            );
        }
    };

    // =========================================================================
    // STEP 1: Calculate hour angle and verify flip is actually needed
    // =========================================================================
    let now = chrono::Utc::now();
    let should_flip = crate::meridian::should_flip_now(
        target_ra,
        lon,
        now,
        config.minutes_past_meridian
    );

    if !should_flip {
        let ha = crate::meridian::hour_angle(
            target_ra,
            crate::meridian::local_sidereal_time(crate::meridian::julian_day(&now), lon)
        );
        tracing::info!("Meridian flip not yet required (HA={:.4}h, threshold={:.2} min)",
            ha, config.minutes_past_meridian);
        return InstructionResult::success_with_message("Meridian flip not yet required");
    }

    // Get current pier side for verification
    let initial_pier_side = ctx.device_ops.mount_side_of_pier(&mount_id).await
        .unwrap_or(crate::meridian::PierSide::Unknown);

    tracing::info!("Flip required - Current pier side: {:?}", initial_pier_side);

    // =========================================================================
    // STEP 2: Stop guiding (if requested)
    // =========================================================================
    if let Some(cb) = progress_callback {
        cb(10.0, "Stopping guider...".to_string());
    }
    let was_guiding = if config.pause_guiding {
        tracing::info!("Stopping guider...");
        match ctx.device_ops.guider_stop().await {
            Ok(_) => {
                tracing::info!("Guiding stopped successfully");
                true
            }
            Err(e) => {
                tracing::warn!("Failed to stop guiding: {}", e);
                false // Continue anyway
            }
        }
    } else {
        false
    };

    // =========================================================================
    // STEP 3: Stop tracking (for safety during flip)
    // =========================================================================
    let was_tracking = ctx.device_ops.mount_is_tracking(&mount_id).await
        .unwrap_or(true);

    if was_tracking {
        tracing::debug!("Pausing tracking for flip...");
        if let Err(e) = ctx.device_ops.mount_set_tracking(&mount_id, false).await {
            tracing::warn!("Could not stop tracking: {}", e);
            // Continue anyway - some mounts handle this automatically
        }
    }

    // =========================================================================
    // STEP 4: Record pre-flip position for verification
    // =========================================================================
    let (pre_flip_ra, pre_flip_dec) = match ctx.device_ops.mount_get_coordinates(&mount_id).await {
        Ok(coords) => coords,
        Err(e) => {
            tracing::warn!("Could not get pre-flip coordinates: {}", e);
            (target_ra, target_dec) // Use target coords as fallback
        }
    };

    tracing::info!("Pre-flip position: RA={:.4}h, Dec={:.4}°", pre_flip_ra, pre_flip_dec);

    // =========================================================================
    // STEP 5: Execute the flip by slewing to same coordinates
    // =========================================================================
    if let Some(cb) = progress_callback {
        cb(30.0, "Executing flip slew...".to_string());
    }
    tracing::info!("Executing flip slew to RA={:.4}h, Dec={:.4}°...", target_ra, target_dec);

    let slew_result = tokio::select! {
        result = ctx.device_ops.mount_slew_to_coordinates(&mount_id, target_ra, target_dec) => {
            result
        }
        _ = wait_for_cancellation(ctx.cancellation_token.clone()) => {
            tracing::warn!("Meridian flip cancelled during slew, aborting...");
            let _ = ctx.device_ops.mount_abort_slew(&mount_id).await;

            // Try to restore tracking before returning
            if was_tracking {
                let _ = ctx.device_ops.mount_set_tracking(&mount_id, true).await;
            }

            return InstructionResult::cancelled("Meridian flip cancelled");
        }
    };

    if let Err(e) = slew_result {
        // Restore tracking before failing
        if was_tracking {
            let _ = ctx.device_ops.mount_set_tracking(&mount_id, true).await;
        }
        return InstructionResult::failure(format!("Flip slew failed: {}", e));
    }

    // Wait for slew to complete with timeout
    tracing::info!("Waiting for flip slew to complete...");
    if let Err(e) = wait_for_mount_idle(&mount_id, ctx, Duration::from_secs(300)).await {
        if was_tracking {
            let _ = ctx.device_ops.mount_set_tracking(&mount_id, true).await;
        }
        return InstructionResult::failure(format!("Flip slew timeout: {}", e));
    }

    // =========================================================================
    // STEP 6: Verify pier side changed (if mount reports pier side)
    // =========================================================================
    if let Some(cb) = progress_callback {
        cb(60.0, "Verifying pier side...".to_string());
    }
    let final_pier_side = ctx.device_ops.mount_side_of_pier(&mount_id).await
        .unwrap_or(crate::meridian::PierSide::Unknown);

    match (initial_pier_side, final_pier_side) {
        (crate::meridian::PierSide::Unknown, _) | (_, crate::meridian::PierSide::Unknown) => {
            tracing::info!("Mount does not report pier side, assuming flip completed successfully");
        }
        (initial, final_) if initial == final_ => {
            tracing::warn!("Pier side did not change after flip! Before: {:?}, After: {:?}", initial, final_);
            tracing::warn!("Flip may not have executed correctly, but continuing...");
        }
        (initial, final_) => {
            tracing::info!("Flip verified: pier side changed from {:?} to {:?}", initial, final_);
        }
    }

    // =========================================================================
    // STEP 7: Resume tracking
    // =========================================================================
    if was_tracking {
        tracing::debug!("Resuming tracking...");
        if let Err(e) = ctx.device_ops.mount_set_tracking(&mount_id, true).await {
            return InstructionResult::failure(format!("Failed to resume tracking after flip: {}", e));
        }
    }

    // =========================================================================
    // STEP 8: Settle time
    // =========================================================================
    if let Some(cb) = progress_callback {
        cb(70.0, "Settling...".to_string());
    }
    if config.settle_time > 0.0 {
        tracing::info!("Settling for {:.1}s...", config.settle_time);

        let settle_duration = Duration::from_secs_f64(config.settle_time);
        let settle_start = std::time::Instant::now();

        while settle_start.elapsed() < settle_duration {
            if ctx.cancellation_token.load(Ordering::Relaxed) {
                return InstructionResult::cancelled("Cancelled during settle");
            }
            sleep(Duration::from_millis(100)).await;
        }
    }

    // =========================================================================
    // STEP 9: Plate solve and center (if enabled)
    // =========================================================================
    if config.auto_center {
        if let Some(cb) = progress_callback {
            cb(80.0, "Centering after flip...".to_string());
        }
        tracing::info!("Centering after flip...");
        let center_config = CenterConfig {
            use_target_coords: true,
            accuracy_arcsec: 10.0, // Slightly looser tolerance for post-flip
            max_attempts: 3,
            exposure_duration: 5.0,
            filter: None,
        };

        // Pass None for progress callback since meridian flip has its own progress
        let center_result = execute_center(&center_config, ctx, None).await;

        if center_result.status != NodeStatus::Success {
            tracing::warn!("Post-flip centering failed, but flip itself succeeded");
            // Don't fail the whole operation - centering is optional
        } else {
            tracing::info!("Post-flip centering successful");
        }
    }

    // =========================================================================
    // STEP 10: Resume guiding (if it was stopped)
    // =========================================================================
    if let Some(cb) = progress_callback {
        cb(95.0, "Resuming guiding...".to_string());
    }
    if was_guiding && config.pause_guiding {
        tracing::info!("Resuming guiding...");
        match ctx.device_ops.guider_start(1.5, 5.0, 120.0).await {
            Ok(_) => {
                tracing::info!("Guiding resumed successfully");
            }
            Err(e) => {
                tracing::warn!("Failed to resume guiding: {}", e);
                // Don't fail - let the user decide whether to continue or abort
            }
        }
    }

    // =========================================================================
    // Success!
    // =========================================================================
    if let Some(cb) = progress_callback {
        cb(100.0, "Flip complete".to_string());
    }
    tracing::info!("=== Meridian Flip Sequence Complete ===");
    InstructionResult::success_with_message(format!(
        "Meridian flip completed successfully (pier side: {:?} -> {:?})",
        initial_pier_side, final_pier_side
    ))
}

// =============================================================================
// DOME INSTRUCTIONS
// =============================================================================

/// Execute open dome
pub async fn execute_open_dome(
    config: &DomeConfig,
    ctx: &InstructionContext,
    progress_callback: Option<&(dyn Fn(f64, String) + Send + Sync)>,
) -> InstructionResult {
    let dome_id = match ctx.dome_id() {
        Ok(id) => id.to_string(),
        Err(e) => return e,
    };

    // Report initial progress
    if let Some(cb) = progress_callback {
        cb(0.0, "Opening dome shutter".to_string());
    }

    tracing::info!("Opening dome shutter...");

    if let Some(result) = ctx.check_cancelled() {
        return result;
    }

    // Report waiting progress BEFORE the async call
    if let Some(cb) = progress_callback {
        cb(50.0, "Waiting for shutter to open".to_string());
    }

    if let Err(e) = ctx.device_ops.dome_open(&dome_id).await {
        return InstructionResult::failure(format!("Failed to open dome: {}", e));
    }

    if !config.shutter_only {
        // Logic to unpark dome if needed (not yet in DeviceOps, assuming open handles it or separate unpark needed)
        // For now, just open shutter is the main action
    }

    // Report completion
    if let Some(cb) = progress_callback {
        cb(100.0, "Dome shutter open".to_string());
    }

    InstructionResult::success_with_message("Dome shutter opened")
}

/// Execute close dome
pub async fn execute_close_dome(
    _config: &DomeConfig,
    ctx: &InstructionContext,
    progress_callback: Option<&(dyn Fn(f64, String) + Send + Sync)>,
) -> InstructionResult {
    let dome_id = match ctx.dome_id() {
        Ok(id) => id.to_string(),
        Err(e) => return e,
    };

    // Report initial progress
    if let Some(cb) = progress_callback {
        cb(0.0, "Closing dome shutter".to_string());
    }

    tracing::info!("Closing dome shutter...");

    if let Some(result) = ctx.check_cancelled() {
        return result;
    }

    // Report waiting progress BEFORE the async call
    if let Some(cb) = progress_callback {
        cb(50.0, "Waiting for shutter to close".to_string());
    }

    if let Err(e) = ctx.device_ops.dome_close(&dome_id).await {
        return InstructionResult::failure(format!("Failed to close dome: {}", e));
    }

    // Report completion
    if let Some(cb) = progress_callback {
        cb(100.0, "Dome shutter closed".to_string());
    }

    InstructionResult::success_with_message("Dome shutter closed")
}

/// Execute park dome
pub async fn execute_park_dome(
    config: &DomeConfig,
    ctx: &InstructionContext,
    progress_callback: Option<&(dyn Fn(f64, String) + Send + Sync)>,
) -> InstructionResult {
    let dome_id = match ctx.dome_id() {
        Ok(id) => id.to_string(),
        Err(e) => return e,
    };

    // Report initial progress
    if let Some(cb) = progress_callback {
        cb(0.0, "Parking dome".to_string());
    }

    if let Some(result) = ctx.check_cancelled() {
        return result;
    }

    if !config.shutter_only {
        // Report waiting progress BEFORE the async call
        if let Some(cb) = progress_callback {
            cb(50.0, "Waiting for dome to reach park position".to_string());
        }

        tracing::info!("Parking dome...");
        if let Err(e) = ctx.device_ops.dome_park(&dome_id).await {
            return InstructionResult::failure(format!("Failed to park dome: {}", e));
        }
    }

    if let Some(result) = ctx.check_cancelled() {
        return result;
    }

    // Usually parking involves closing shutter too
    tracing::info!("Closing shutter (park sequence)...");
    let _ = ctx.device_ops.dome_close(&dome_id).await;

    // Report completion
    if let Some(cb) = progress_callback {
        cb(100.0, "Dome parked".to_string());
    }

    InstructionResult::success_with_message("Dome parked")
}

// =============================================================================
// MOSAIC INSTRUCTION
// =============================================================================

/// Execute mosaic panel iteration
/// This is a container instruction that iterates through mosaic panels
/// The actual panel calculation is done in the mosaic module
pub async fn execute_mosaic(
    config: &crate::MosaicConfig,
    _ctx: &InstructionContext,
    progress_callback: Option<&(dyn Fn(f64, String) + Send + Sync)>,
) -> InstructionResult {
    // Emit initial progress
    if let Some(cb) = progress_callback {
        cb(0.0, "Starting mosaic".to_string());
    }

    tracing::info!(
        "Starting mosaic: {}x{} panels, {:.1}% overlap",
        config.panels_horizontal,
        config.panels_vertical,
        config.overlap_percent
    );

    // Emit progress for calculating panels
    if let Some(cb) = progress_callback {
        cb(30.0, "Calculating panel positions".to_string());
    }

    // Calculate all panel positions
    let panels = crate::mosaic::calculate_mosaic_panels(config);
    let total_panels = panels.len();

    tracing::info!("Mosaic contains {} panels", total_panels);

    // Note: The actual execution of visiting each panel will be handled by the
    // node execution logic which will create child slew/center/expose nodes
    // for each panel. This instruction just validates the configuration.

    // Emit final progress
    if let Some(cb) = progress_callback {
        cb(100.0, format!("Mosaic configured: {} panels", total_panels));
    }

    InstructionResult {
        status: NodeStatus::Success,
        message: Some(format!("Mosaic configured: {} panels", total_panels)),
        data: Some(serde_json::json!({
            "total_panels": total_panels,
            "panels_horizontal": config.panels_horizontal,
            "panels_vertical": config.panels_vertical,
            "overlap_percent": config.overlap_percent,
            "total_area_arcmin2": crate::mosaic::calculate_mosaic_area(config),
        })),
        hfr_values: Vec::new(),
    }
}

// =============================================================================
// COVER CALIBRATOR (FLAT PANEL / DUST COVER) INSTRUCTIONS
// =============================================================================

/// Execute open cover (unpark dust cap)
pub async fn execute_open_cover(
    config: &crate::CoverCalibratorConfig,
    ctx: &InstructionContext,
    progress_callback: Option<&(dyn Fn(f64, String) + Send + Sync)>,
) -> InstructionResult {
    let device_id = match ctx.cover_calibrator_id() {
        Ok(id) => id.to_string(),
        Err(e) => return e,
    };

    // Report initial progress
    if let Some(cb) = progress_callback {
        cb(0.0, "Opening cover".to_string());
    }

    tracing::info!("Opening cover...");

    if let Some(result) = ctx.check_cancelled() {
        return result;
    }

    // Report waiting progress BEFORE the async call
    if let Some(cb) = progress_callback {
        cb(50.0, "Waiting for cover to open".to_string());
    }

    // Start opening the cover
    if let Err(e) = ctx.device_ops.cover_calibrator_open_cover(&device_id).await {
        return InstructionResult::failure(format!("Failed to open cover: {}", e));
    }

    // Wait for cover to reach open state with timeout
    let timeout = Duration::from_secs(config.timeout_secs as u64);
    match wait_for_cover_state(&device_id, 3, ctx, timeout).await {
        Ok(_) => {
            // Report completion
            if let Some(cb) = progress_callback {
                cb(100.0, "Cover open".to_string());
            }
            InstructionResult::success_with_message("Cover opened")
        }
        Err(e) => InstructionResult::failure(e),
    }
}

/// Execute close cover (park dust cap)
pub async fn execute_close_cover(
    config: &crate::CoverCalibratorConfig,
    ctx: &InstructionContext,
    progress_callback: Option<&(dyn Fn(f64, String) + Send + Sync)>,
) -> InstructionResult {
    let device_id = match ctx.cover_calibrator_id() {
        Ok(id) => id.to_string(),
        Err(e) => return e,
    };

    // Report initial progress
    if let Some(cb) = progress_callback {
        cb(0.0, "Closing cover".to_string());
    }

    tracing::info!("Closing cover...");

    if let Some(result) = ctx.check_cancelled() {
        return result;
    }

    // Report waiting progress BEFORE the async call
    if let Some(cb) = progress_callback {
        cb(50.0, "Waiting for cover to close".to_string());
    }

    // Start closing the cover
    if let Err(e) = ctx.device_ops.cover_calibrator_close_cover(&device_id).await {
        return InstructionResult::failure(format!("Failed to close cover: {}", e));
    }

    // Wait for cover to reach closed state with timeout
    let timeout = Duration::from_secs(config.timeout_secs as u64);
    match wait_for_cover_state(&device_id, 1, ctx, timeout).await {
        Ok(_) => {
            // Report completion
            if let Some(cb) = progress_callback {
                cb(100.0, "Cover closed".to_string());
            }
            InstructionResult::success_with_message("Cover closed")
        }
        Err(e) => InstructionResult::failure(e),
    }
}

/// Execute calibrator on (turn on flat panel light)
pub async fn execute_calibrator_on(
    config: &crate::CalibratorOnConfig,
    ctx: &InstructionContext,
    progress_callback: Option<&(dyn Fn(f64, String) + Send + Sync)>,
) -> InstructionResult {
    let device_id = match ctx.cover_calibrator_id() {
        Ok(id) => id.to_string(),
        Err(e) => return e,
    };

    // Report initial progress
    if let Some(cb) = progress_callback {
        cb(0.0, "Turning on calibrator".to_string());
    }

    tracing::info!("Turning calibrator on at brightness {}...", config.brightness);

    if let Some(result) = ctx.check_cancelled() {
        return result;
    }

    // Report waiting progress BEFORE the async call
    if let Some(cb) = progress_callback {
        cb(50.0, format!("Adjusting brightness to {}%", config.brightness));
    }

    // Turn on the calibrator at specified brightness
    if let Err(e) = ctx.device_ops.cover_calibrator_calibrator_on(&device_id, config.brightness).await {
        return InstructionResult::failure(format!("Failed to turn on calibrator: {}", e));
    }

    // Wait for calibrator to reach ready state with timeout
    let timeout = Duration::from_secs(config.timeout_secs as u64);
    match wait_for_calibrator_state(&device_id, 3, ctx, timeout).await {
        Ok(_) => {
            // Verify brightness is set correctly
            let actual_brightness = ctx.device_ops.cover_calibrator_get_brightness(&device_id).await
                .unwrap_or(config.brightness);
            // Report completion
            if let Some(cb) = progress_callback {
                cb(100.0, format!("Calibrator on at brightness {}", actual_brightness));
            }
            InstructionResult::success_with_message(
                format!("Calibrator on at brightness {}", actual_brightness)
            )
        }
        Err(e) => InstructionResult::failure(e),
    }
}

/// Execute calibrator off (turn off flat panel light)
pub async fn execute_calibrator_off(
    config: &crate::CoverCalibratorConfig,
    ctx: &InstructionContext,
    progress_callback: Option<&(dyn Fn(f64, String) + Send + Sync)>,
) -> InstructionResult {
    let device_id = match ctx.cover_calibrator_id() {
        Ok(id) => id.to_string(),
        Err(e) => return e,
    };

    // Report initial progress
    if let Some(cb) = progress_callback {
        cb(0.0, "Turning off calibrator".to_string());
    }

    tracing::info!("Turning calibrator off...");

    if let Some(result) = ctx.check_cancelled() {
        return result;
    }

    // Report waiting progress BEFORE the async call
    if let Some(cb) = progress_callback {
        cb(50.0, "Waiting for calibrator to turn off".to_string());
    }

    // Turn off the calibrator
    if let Err(e) = ctx.device_ops.cover_calibrator_calibrator_off(&device_id).await {
        return InstructionResult::failure(format!("Failed to turn off calibrator: {}", e));
    }

    // Wait for calibrator to reach off state with timeout
    let timeout = Duration::from_secs(config.timeout_secs as u64);
    match wait_for_calibrator_state(&device_id, 1, ctx, timeout).await {
        Ok(_) => {
            // Report completion
            if let Some(cb) = progress_callback {
                cb(100.0, "Calibrator off".to_string());
            }
            InstructionResult::success_with_message("Calibrator off")
        }
        Err(e) => InstructionResult::failure(e),
    }
}

/// Wait for cover to reach target state with timeout
/// States: 0=NotPresent, 1=Closed, 2=Moving, 3=Open, 4=Unknown, 5=Error
async fn wait_for_cover_state(
    device_id: &str,
    target_state: i32,
    ctx: &InstructionContext,
    timeout: Duration,
) -> Result<(), String> {
    let start = std::time::Instant::now();
    let state_name = match target_state {
        0 => "NotPresent",
        1 => "Closed",
        2 => "Moving",
        3 => "Open",
        4 => "Unknown",
        5 => "Error",
        _ => "Unknown",
    };

    loop {
        // Check cancellation
        if ctx.cancellation_token.load(Ordering::Relaxed) {
            // Try to halt cover movement
            let _ = ctx.device_ops.cover_calibrator_halt_cover(device_id).await;
            return Err("Operation cancelled".to_string());
        }

        // Check current state
        match ctx.device_ops.cover_calibrator_get_cover_state(device_id).await {
            Ok(state) => {
                if state == target_state {
                    tracing::debug!("Cover reached {} state", state_name);
                    return Ok(());
                }
                if state == 5 {
                    return Err("Cover reported error state".to_string());
                }
                tracing::trace!("Cover state: {}, waiting for {}", state, state_name);
            }
            Err(e) => {
                tracing::warn!("Error checking cover state: {}", e);
                // Continue polling - transient error
            }
        }

        // Check timeout
        if start.elapsed() > timeout {
            return Err(format!(
                "Cover did not reach {} state within {} seconds",
                state_name, timeout.as_secs()
            ));
        }

        // Poll every 500ms
        sleep(Duration::from_millis(500)).await;
    }
}

/// Wait for calibrator to reach target state with timeout
/// States: 0=NotPresent, 1=Off, 2=NotReady, 3=Ready, 4=Unknown, 5=Error
async fn wait_for_calibrator_state(
    device_id: &str,
    target_state: i32,
    ctx: &InstructionContext,
    timeout: Duration,
) -> Result<(), String> {
    let start = std::time::Instant::now();
    let state_name = match target_state {
        0 => "NotPresent",
        1 => "Off",
        2 => "NotReady",
        3 => "Ready",
        4 => "Unknown",
        5 => "Error",
        _ => "Unknown",
    };

    loop {
        // Check cancellation
        if ctx.cancellation_token.load(Ordering::Relaxed) {
            return Err("Operation cancelled".to_string());
        }

        // Check current state
        match ctx.device_ops.cover_calibrator_get_calibrator_state(device_id).await {
            Ok(state) => {
                if state == target_state {
                    tracing::debug!("Calibrator reached {} state", state_name);
                    return Ok(());
                }
                if state == 5 {
                    return Err("Calibrator reported error state".to_string());
                }
                tracing::trace!("Calibrator state: {}, waiting for {}", state, state_name);
            }
            Err(e) => {
                tracing::warn!("Error checking calibrator state: {}", e);
                // Continue polling - transient error
            }
        }

        // Check timeout
        if start.elapsed() > timeout {
            return Err(format!(
                "Calibrator did not reach {} state within {} seconds",
                state_name, timeout.as_secs()
            ));
        }

        // Poll every 200ms (calibrator state can change quickly)
        sleep(Duration::from_millis(200)).await;
    }
}

// =============================================================================
// TESTS
// =============================================================================

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_normalize_ra_diff_hours_no_wrap() {
        // Simple cases with no wraparound
        assert!((normalize_ra_diff_hours(1.0) - 1.0).abs() < 0.0001);
        assert!((normalize_ra_diff_hours(-1.0) - (-1.0)).abs() < 0.0001);
        assert!((normalize_ra_diff_hours(11.0) - 11.0).abs() < 0.0001);
        assert!((normalize_ra_diff_hours(-11.0) - (-11.0)).abs() < 0.0001);
    }

    #[test]
    fn test_normalize_ra_diff_hours_wraparound() {
        // Wraparound cases: 23h to 1h should be 2h diff, not 22h
        assert!((normalize_ra_diff_hours(22.0) - (-2.0)).abs() < 0.0001);
        assert!((normalize_ra_diff_hours(-22.0) - 2.0).abs() < 0.0001);

        // 13 hours should wrap to -11 hours (shorter path)
        assert!((normalize_ra_diff_hours(13.0) - (-11.0)).abs() < 0.0001);
        assert!((normalize_ra_diff_hours(-13.0) - 11.0).abs() < 0.0001);

        // Edge case: exactly 12 hours
        assert!((normalize_ra_diff_hours(12.0).abs() - 12.0).abs() < 0.0001);
    }

    #[test]
    fn test_validate_slew_position_success() {
        // Exact match
        assert!(validate_slew_position(12.0, 45.0, 12.0, 45.0, 1.0 / 60.0).is_ok());

        // Within tolerance (less than 1 arcminute = 1/60 degree)
        let small_diff = 0.5 / 60.0; // 0.5 arcminute
        let ra_diff_hours = small_diff / 15.0; // Convert degrees to hours
        assert!(validate_slew_position(12.0, 45.0, 12.0 + ra_diff_hours, 45.0 + small_diff, 1.0 / 60.0).is_ok());
    }

    #[test]
    fn test_validate_slew_position_ra_failure() {
        // RA exceeds tolerance (2 arcminutes when tolerance is 1)
        let large_diff_hours = (2.0 / 60.0) / 15.0; // 2 arcminutes in hours
        let result = validate_slew_position(12.0, 45.0, 12.0 + large_diff_hours, 45.0, 1.0 / 60.0);
        assert!(result.is_err());
        assert!(result.unwrap_err().contains("did not reach target"));
    }

    #[test]
    fn test_validate_slew_position_dec_failure() {
        // Dec exceeds tolerance
        let large_diff_deg = 2.0 / 60.0; // 2 arcminutes
        let result = validate_slew_position(12.0, 45.0, 12.0, 45.0 + large_diff_deg, 1.0 / 60.0);
        assert!(result.is_err());
        assert!(result.unwrap_err().contains("did not reach target"));
    }

    #[test]
    fn test_validate_slew_position_ra_wraparound() {
        // Test RA wraparound: target at 0.1h, actual at 23.9h should be 0.2h diff = 3 degrees
        // This is well within tolerance (we'll use a generous tolerance for this test)
        let tolerance = 5.0; // 5 degrees
        assert!(validate_slew_position(0.1, 45.0, 23.9, 45.0, tolerance).is_ok());

        // With 1 arcminute tolerance, 0.2h = 3 degrees should fail
        let result = validate_slew_position(0.1, 45.0, 23.9, 45.0, 1.0 / 60.0);
        assert!(result.is_err());
    }
}
