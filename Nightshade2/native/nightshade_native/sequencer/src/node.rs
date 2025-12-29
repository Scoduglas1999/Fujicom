//! Behavior tree node definitions and execution

use crate::{
    NodeId, NodeStatus, NodeType, NodeDefinition, LoopCondition, ConditionalCheck,
    instructions::*, TargetHeaderConfig, LoopConfig, ParallelConfig, ConditionalConfig,
    RecoveryConfig, RecoveryAction,
    device_ops::{SharedDeviceOps, NullDeviceOps},
    ExposureConfig, TriggerCondition, TriggerAction, AutofocusConfig, AutofocusMethod,
};
use async_trait::async_trait;
use std::sync::Arc;
use std::sync::atomic::{AtomicBool, Ordering};
use tokio::sync::RwLock;

/// Context passed to nodes during execution
pub struct ExecutionContext {
    /// ID of the node being executed
    pub node_id: NodeId,
    /// Current target information (propagated from TargetGroup)
    pub target_ra: Option<f64>,
    pub target_dec: Option<f64>,
    pub target_name: Option<String>,
    pub target_rotation: Option<f64>,
    /// Current filter
    pub current_filter: Option<String>,
    /// Cancellation flag
    pub is_cancelled: Arc<AtomicBool>,
    /// Pause flag - set by recovery nodes, cleared by executor on resume
    pub is_paused: Arc<AtomicBool>,
    /// Resume notifier - signaled when execution should resume after pause
    pub resume_notify: Arc<tokio::sync::Notify>,
    /// Progress callback
    pub progress_callback: Option<Box<dyn Fn(ProgressUpdate) + Send + Sync>>,
    /// Connected device IDs
    pub camera_id: Option<String>,
    pub mount_id: Option<String>,
    pub focuser_id: Option<String>,
    pub filterwheel_id: Option<String>,
    pub rotator_id: Option<String>,
    pub dome_id: Option<String>,
    pub cover_calibrator_id: Option<String>,
    /// Base save path for images
    pub save_path: Option<std::path::PathBuf>,
    /// Observer location
    pub latitude: Option<f64>,
    pub longitude: Option<f64>,
    /// Device operations handler
    pub device_ops: SharedDeviceOps,
    /// Completed integration time in seconds (shared counter)
    pub completed_integration_secs: Arc<RwLock<f64>>,
    /// Trigger state (for updating during execution)
    pub trigger_state: Option<Arc<RwLock<crate::triggers::TriggerState>>>,
}

/// Progress update sent during execution
#[derive(Debug, Clone)]
pub struct ProgressUpdate {
    pub node_id: NodeId,
    pub status: NodeStatus,
    pub message: Option<String>,
    pub current_frame: Option<u32>,
    pub total_frames: Option<u32>,
    pub current_child: Option<usize>,
    pub total_children: Option<usize>,
    /// Exposure time just completed (seconds)
    pub completed_exposure_secs: Option<f64>,
}

impl ExecutionContext {
    pub fn new(node_id: NodeId) -> Self {
        Self {
            node_id,
            target_ra: None,
            target_dec: None,
            target_name: None,
            target_rotation: None,
            current_filter: None,
            is_cancelled: Arc::new(AtomicBool::new(false)),
            is_paused: Arc::new(AtomicBool::new(false)),
            resume_notify: Arc::new(tokio::sync::Notify::new()),
            progress_callback: None,
            camera_id: None,
            mount_id: None,
            focuser_id: None,
            filterwheel_id: None,
            rotator_id: None,
            dome_id: None,
            cover_calibrator_id: None,
            save_path: None,
            latitude: None,
            longitude: None,
            device_ops: Arc::new(NullDeviceOps),
            completed_integration_secs: Arc::new(RwLock::new(0.0)),
            trigger_state: None,
        }
    }
    
    pub fn with_device_ops(mut self, ops: SharedDeviceOps) -> Self {
        self.device_ops = ops;
        self
    }

    pub fn with_target(mut self, name: String, ra: f64, dec: f64, rotation: Option<f64>) -> Self {
        self.target_name = Some(name);
        self.target_ra = Some(ra);
        self.target_dec = Some(dec);
        self.target_rotation = rotation;
        self
    }

    pub async fn is_cancelled(&self) -> bool {
        self.is_cancelled.load(Ordering::Relaxed)
    }

    /// Check if currently paused
    pub fn is_paused(&self) -> bool {
        self.is_paused.load(Ordering::Relaxed)
    }

    /// Request pause and wait for resume
    /// Returns false if cancelled while waiting
    pub async fn pause_and_wait_for_resume(&self) -> bool {
        // Set paused flag
        self.is_paused.store(true, Ordering::Relaxed);
        tracing::info!("Execution paused, waiting for resume...");

        // Wait for either resume or cancellation
        loop {
            tokio::select! {
                _ = self.resume_notify.notified() => {
                    if !self.is_paused.load(Ordering::Relaxed) {
                        tracing::info!("Execution resumed");
                        return true; // Successfully resumed
                    }
                }
                _ = tokio::time::sleep(std::time::Duration::from_millis(100)) => {
                    // Check if cancelled while paused
                    if self.is_cancelled.load(Ordering::Relaxed) {
                        tracing::info!("Cancelled while paused");
                        return false;
                    }
                    // Check if externally resumed (e.g., by executor)
                    if !self.is_paused.load(Ordering::Relaxed) {
                        tracing::info!("Execution resumed");
                        return true;
                    }
                }
            }
        }
    }

    /// Resume execution (called by executor)
    pub fn resume(&self) {
        self.is_paused.store(false, Ordering::Relaxed);
        self.resume_notify.notify_waiters();
    }

    pub fn send_progress(&self, update: ProgressUpdate) {
        let exposure_secs = update.completed_exposure_secs;
        if let Some(callback) = &self.progress_callback {
            callback(update);
        }
        
        // Also track integration time
        if let Some(exposure_secs) = exposure_secs {
            if let Ok(mut counter) = self.completed_integration_secs.try_write() {
                *counter += exposure_secs;
            }
        }
    }
    
    /// Get the current completed integration time in seconds
    pub async fn get_completed_integration_secs(&self) -> f64 {
        *self.completed_integration_secs.read().await
    }
    
    /// Calculate current altitude of target based on RA/Dec and observer location
    pub fn calculate_altitude(&self) -> Option<f64> {
        let ra_hours = self.target_ra?;
        let dec_degrees = self.target_dec?;
        let lat = self.latitude.unwrap_or(0.0);
        let lon = self.longitude.unwrap_or(0.0);
        
        // Get current time
        let now = chrono::Utc::now();
        
        // Calculate Julian Day
        let jd = julian_day(&now);
        
        // Calculate Local Sidereal Time
        let lst = local_sidereal_time(jd, lon);
        
        // Calculate Hour Angle
        let ha = lst - ra_hours;
        let ha_rad = ha.to_radians() * 15.0; // Convert hours to radians
        let dec_rad = dec_degrees.to_radians();
        let lat_rad = lat.to_radians();
        
        // Calculate altitude
        let sin_alt = lat_rad.sin() * dec_rad.sin() + 
                      lat_rad.cos() * dec_rad.cos() * ha_rad.cos();
        Some(sin_alt.asin().to_degrees())
    }
    
    /// Calculate separation between target and moon in degrees
    pub fn calculate_moon_separation(&self) -> Option<f64> {
        let target_ra = self.target_ra?;
        let target_dec = self.target_dec?;
        
        // Calculate approximate moon position
        let now = chrono::Utc::now();
        let jd = julian_day(&now);
        let days = jd - 2451545.0;
        
        // Simplified lunar position calculation
        let moon_longitude = (218.32 + 13.176396 * days) % 360.0;
        let moon_anomaly = (134.9 + 13.064993 * days) % 360.0;
        let moon_node = (93.3 + 13.229350 * days) % 360.0;
        
        // Approximate ecliptic latitude and longitude
        let ecl_lon = moon_longitude + 
            6.29 * moon_anomaly.to_radians().sin() -
            1.27 * (2.0 * moon_node.to_radians() - moon_anomaly.to_radians()).sin();
        let ecl_lat = 5.13 * moon_node.to_radians().sin();
        
        // Convert to equatorial coordinates
        let obliquity = 23.439f64;
        let ecl_lon_rad = ecl_lon.to_radians();
        let ecl_lat_rad = ecl_lat.to_radians();
        let obl_rad = obliquity.to_radians();
        
        let moon_ra = ((ecl_lon_rad.sin() * obl_rad.cos() - 
                        ecl_lat_rad.tan() * obl_rad.sin()).atan2(ecl_lon_rad.cos()))
                       .to_degrees() / 15.0; // Convert to hours
        let moon_dec = (ecl_lat_rad.sin() * obl_rad.cos() + 
                        ecl_lat_rad.cos() * obl_rad.sin() * ecl_lon_rad.sin())
                       .asin().to_degrees();
        
        // Calculate angular separation using spherical law of cosines
        let target_ra_rad = target_ra.to_radians() * 15.0; // Hours to degrees to radians
        let target_dec_rad = target_dec.to_radians();
        let moon_ra_rad = moon_ra.to_radians() * 15.0;
        let moon_dec_rad = moon_dec.to_radians();
        
        let cos_sep = target_dec_rad.sin() * moon_dec_rad.sin() + 
                      target_dec_rad.cos() * moon_dec_rad.cos() * 
                      (target_ra_rad - moon_ra_rad).cos();
        
        Some(cos_sep.acos().to_degrees())
    }
    
    /// Check if it's currently dark (astronomical twilight has ended)
    pub fn is_dark(&self) -> bool {
        // Calculate sun altitude
        let lat = self.latitude.unwrap_or(0.0);
        let lon = self.longitude.unwrap_or(0.0);
        
        let now = chrono::Utc::now();
        let jd = julian_day(&now);
        
        // Approximate sun position calculation
        let days_since_j2000 = jd - 2451545.0;
        let mean_longitude = (280.46 + 0.9856474 * days_since_j2000) % 360.0;
        let mean_anomaly = (357.528 + 0.9856003 * days_since_j2000) % 360.0;
        
        let ecliptic_longitude = mean_longitude + 
            1.915 * mean_anomaly.to_radians().sin() + 
            0.020 * (2.0 * mean_anomaly.to_radians()).sin();
        
        // Sun's declination
        let obliquity = 23.439 - 0.0000004 * days_since_j2000;
        let sun_dec = (obliquity.to_radians().sin() * ecliptic_longitude.to_radians().sin()).asin().to_degrees();
        let sun_ra = (ecliptic_longitude.to_radians().cos() / sun_dec.to_radians().cos()).acos().to_degrees() / 15.0;
        
        // Calculate sun altitude
        let lst = local_sidereal_time(jd, lon);
        let ha = lst - sun_ra;
        let ha_rad = ha.to_radians() * 15.0;
        let dec_rad = sun_dec.to_radians();
        let lat_rad = lat.to_radians();
        
        let sun_alt = (lat_rad.sin() * dec_rad.sin() + 
                       lat_rad.cos() * dec_rad.cos() * ha_rad.cos()).asin().to_degrees();
        
        // Astronomical twilight is when sun is below -18 degrees
        sun_alt < -18.0
    }
    
    /// Set the next meridian flip time in the trigger state (if available)
    /// This is a no-op if trigger state is not accessible
    pub fn set_next_meridian_flip_time(&self, timestamp: Option<i64>) {
        // This is a placeholder - the actual trigger state update will be done
        // through the executor's trigger manager. For now, just log it.
        if let Some(ts) = timestamp {
            tracing::debug!("Meridian flip time set to timestamp: {}", ts);
        }
    }

    /// Build an InstructionContext from this ExecutionContext
    pub async fn to_instruction_context(&self) -> InstructionContext {
        InstructionContext {
            target_ra: self.target_ra,
            target_dec: self.target_dec,
            target_name: self.target_name.clone(),
            current_filter: self.current_filter.clone(),
            current_binning: crate::Binning::One,
            cancellation_token: self.is_cancelled.clone(),
            camera_id: self.camera_id.clone(),
            mount_id: self.mount_id.clone(),
            focuser_id: self.focuser_id.clone(),
            filterwheel_id: self.filterwheel_id.clone(),
            rotator_id: self.rotator_id.clone(),
            dome_id: self.dome_id.clone(),
            cover_calibrator_id: self.cover_calibrator_id.clone(),
            save_path: self.save_path.clone(),
            latitude: self.latitude,
            longitude: self.longitude,
            device_ops: self.device_ops.clone(),
            trigger_state: self.trigger_state.clone(),
        }
    }
}

/// Base trait for all behavior tree nodes
#[async_trait]
pub trait Node: Send + Sync {
    /// Get the unique ID of this node
    fn id(&self) -> &NodeId;

    /// Get the display name of this node
    fn name(&self) -> &str;

    /// Get the node type
    fn node_type(&self) -> &NodeType;

    /// Is this node enabled?
    fn is_enabled(&self) -> bool;

    /// Execute the node and return its status
    async fn execute(&mut self, context: &mut ExecutionContext) -> NodeStatus;

    /// Reset the node to its initial state
    fn reset(&mut self);

    /// Abort the node if it's running
    async fn abort(&mut self);

    /// Get child nodes (for container nodes)
    fn children(&self) -> &[Box<dyn Node>];

    /// Get mutable children (for container nodes)
    fn children_mut(&mut self) -> &mut Vec<Box<dyn Node>>;

    /// Mark a node as completed (for crash recovery resume)
    /// If node_id matches this node, marks it as Success.
    /// Otherwise, propagates to children.
    fn mark_completed(&mut self, node_id: &NodeId);
}

/// A runtime node instance created from a NodeDefinition
pub struct RuntimeNode {
    pub definition: NodeDefinition,
    pub children: Vec<Box<dyn Node>>,
    pub status: NodeStatus,
    pub current_iteration: u32,
}

impl RuntimeNode {
    pub fn from_definition(def: NodeDefinition) -> Self {
        Self {
            definition: def,
            children: Vec::new(),
            status: NodeStatus::Pending,
            current_iteration: 0,
        }
    }

    pub fn add_child(&mut self, child: Box<dyn Node>) {
        self.children.push(child);
    }

    /// Check exposure-level triggers (per-exposure monitoring)
    /// These are different from the global TriggerManager triggers -
    /// these are checked immediately after exposures complete
    async fn check_exposure_triggers(
        &self,
        config: &ExposureConfig,
        result: &InstructionResult,
        context: &mut ExecutionContext,
    ) {
        if config.triggers.is_empty() {
            return;
        }

        // Get current HFR if available
        let current_hfr = result.hfr_values.last().copied();

        // Get guiding RMS from trigger state
        let trigger_state_lock = match &context.trigger_state {
            Some(lock) => lock,
            None => return,
        };

        let trigger_state = trigger_state_lock.read().await;
        let latest_guiding_rms = trigger_state.guiding_rms_history
            .as_ref()
            .and_then(|history| history.last())
            .map(|(_, rms)| *rms);

        drop(trigger_state); // Release read lock

        // Check each trigger
        for trigger in &config.triggers {
            let should_fire = match &trigger.condition {
                TriggerCondition::HfrAbove(threshold) => {
                    if let Some(hfr) = current_hfr {
                        hfr > *threshold
                    } else {
                        false
                    }
                }
                TriggerCondition::GuidingRmsAbove(threshold) => {
                    if let Some(rms) = latest_guiding_rms {
                        rms > *threshold
                    } else {
                        false
                    }
                }
                TriggerCondition::DriftAbove { ra_px, dec_px } => {
                    // Get drift from trigger state
                    let trigger_state = trigger_state_lock.read().await;

                    match trigger_state.calculate_drift_pixels() {
                        Some((drift_ra_px, drift_dec_px)) => {
                            let drift_exceeds = drift_ra_px > *ra_px || drift_dec_px > *dec_px;
                            if drift_exceeds {
                                tracing::warn!(
                                    "Drift detected: RA={:.2}px (threshold={:.2}px), Dec={:.2}px (threshold={:.2}px)",
                                    drift_ra_px, ra_px, drift_dec_px, dec_px
                                );
                            }
                            drift_exceeds
                        }
                        None => {
                            tracing::debug!(
                                "Drift trigger check skipped - insufficient plate solve data (thresholds: ra_px={}, dec_px={})",
                                ra_px, dec_px
                            );
                            false
                        }
                    }
                }
            };

            if should_fire {
                tracing::warn!(
                    "Exposure trigger fired: {:?} - action: {:?}",
                    trigger.condition,
                    trigger.action
                );

                // Execute trigger action
                match &trigger.action {
                    TriggerAction::PauseAndRecalibrate => {
                        // Pause and wait for manual intervention
                        tracing::info!("Pausing sequence due to exposure trigger");
                        context.send_progress(ProgressUpdate {
                            node_id: self.id().clone(),
                            status: NodeStatus::Running,
                            message: Some(format!("Trigger fired: {:?} - Paused for recalibration", trigger.condition)),
                            current_frame: None,
                            total_frames: None,
                            current_child: None,
                            total_children: None,
                            completed_exposure_secs: None,
                        });

                        // Request pause and wait
                        let resumed = context.pause_and_wait_for_resume().await;
                        if !resumed {
                            tracing::info!("Cancelled while paused for trigger");
                            return;
                        }
                    }
                    TriggerAction::Autofocus => {
                        // Run autofocus immediately
                        tracing::info!("Running autofocus due to exposure trigger");
                        context.send_progress(ProgressUpdate {
                            node_id: self.id().clone(),
                            status: NodeStatus::Running,
                            message: Some(format!("Trigger fired: {:?} - Running autofocus", trigger.condition)),
                            current_frame: None,
                            total_frames: None,
                            current_child: None,
                            total_children: None,
                            completed_exposure_secs: None,
                        });

                        // Create autofocus config
                        let af_config = AutofocusConfig {
                            method: AutofocusMethod::VCurve,
                            step_size: 100,
                            steps_out: 7,
                            exposure_duration: 3.0,
                            filter: context.current_filter.clone(),
                            binning: config.binning,
                        };

                        let ctx = context.to_instruction_context().await;
                        // Pass None for progress callback in trigger-based autofocus
                        let af_result = execute_autofocus(&af_config, &ctx, None).await;

                        if af_result.status == NodeStatus::Success {
                            // Update trigger state with new HFR baseline
                            if let Some(best_hfr) = af_result.hfr_values.first() {
                                let mut trigger_state = trigger_state_lock.write().await;
                                trigger_state.update_hfr(*best_hfr);
                                trigger_state.reset_baseline_hfr();
                                trigger_state.mark_autofocus_performed();
                                tracing::info!("Autofocus complete, reset HFR baseline to {:.2}", best_hfr);
                            }
                        } else {
                            tracing::warn!("Autofocus triggered by exposure failed: {:?}", af_result.status);
                        }
                    }
                    TriggerAction::Abort => {
                        tracing::error!("Aborting sequence due to exposure trigger");
                        context.send_progress(ProgressUpdate {
                            node_id: self.id().clone(),
                            status: NodeStatus::Failure,
                            message: Some(format!("Trigger fired: {:?} - Aborting sequence", trigger.condition)),
                            current_frame: None,
                            total_frames: None,
                            current_child: None,
                            total_children: None,
                            completed_exposure_secs: None,
                        });

                        // Set cancelled flag
                        context.is_cancelled.store(true, Ordering::Relaxed);
                        return;
                    }
                }
            }
        }
    }
}

#[async_trait]
impl Node for RuntimeNode {
    fn id(&self) -> &NodeId {
        &self.definition.id
    }

    fn name(&self) -> &str {
        &self.definition.name
    }

    fn node_type(&self) -> &NodeType {
        &self.definition.node_type
    }

    fn is_enabled(&self) -> bool {
        self.definition.enabled
    }

    async fn execute(&mut self, context: &mut ExecutionContext) -> NodeStatus {
        if !self.definition.enabled {
            self.status = NodeStatus::Skipped;
            return NodeStatus::Skipped;
        }

        self.status = NodeStatus::Running;
        context.send_progress(ProgressUpdate {
            node_id: self.id().clone(),
            status: NodeStatus::Running,
            message: Some(format!("Executing: {}", self.name())),
            current_frame: None,
            total_frames: None,
            current_child: None,
            total_children: None,
            completed_exposure_secs: None,
        });

        let result = match &self.definition.node_type {
            // Container/Logic nodes
            NodeType::TargetHeader(config) | NodeType::TargetGroup(config) => {
                self.execute_target_header(config.clone(), context).await
            }
            NodeType::Loop(config) => {
                self.execute_loop(config.clone(), context).await
            }
            NodeType::Parallel(config) => {
                self.execute_parallel(config.clone(), context).await
            }
            NodeType::Conditional(config) => {
                self.execute_conditional(config.clone(), context).await
            }
            NodeType::Recovery(config) => {
                self.execute_recovery(config.clone(), context).await
            }

            // Instruction nodes
            NodeType::SlewToTarget(config) => {
                let ctx = context.to_instruction_context().await;
                let node_id = self.id().clone();
                let progress_cb = context.progress_callback.as_ref();

                let progress_fn = |progress: f64, detail: String| {
                    if let Some(cb) = progress_cb {
                        cb(ProgressUpdate {
                            node_id: node_id.clone(),
                            status: NodeStatus::Running,
                            message: Some(format!("Slew: {} ({:.0}%)", detail, progress)),
                            current_frame: None,
                            total_frames: None,
                            current_child: None,
                            total_children: None,
                            completed_exposure_secs: None,
                        });
                    }
                };

                execute_slew(config, &ctx, Some(&progress_fn)).await.log_and_get_status("Slew")
            }
            NodeType::CenterTarget(config) => {
                let ctx = context.to_instruction_context().await;
                let node_id = self.id().clone();
                let progress_cb = context.progress_callback.as_ref();

                let progress_fn = |progress: f64, detail: String| {
                    if let Some(cb) = progress_cb {
                        cb(ProgressUpdate {
                            node_id: node_id.clone(),
                            status: NodeStatus::Running,
                            message: Some(format!("Center: {} ({:.0}%)", detail, progress)),
                            current_frame: None,
                            total_frames: None,
                            current_child: None,
                            total_children: None,
                            completed_exposure_secs: None,
                        });
                    }
                };

                execute_center(config, &ctx, Some(&progress_fn)).await.log_and_get_status("Center Target")
            }
            NodeType::TakeExposure(config) => {
                let mut ctx = context.to_instruction_context().await;
                ctx.current_binning = config.binning;
                let node_id = self.id().clone();
                let duration_secs = config.duration_secs;
                let total_count = config.count;
                let progress_cb = context.progress_callback.as_ref();

                let result = execute_exposure(config, &ctx, |current, total| {
                    if let Some(cb) = progress_cb {
                        cb(ProgressUpdate {
                            node_id: node_id.clone(),
                            status: NodeStatus::Running,
                            message: Some(format!("Frame {}/{}", current, total)),
                            current_frame: Some(current),
                            total_frames: Some(total),
                            current_child: None,
                            total_children: None,
                            completed_exposure_secs: None, // Will track total after completion
                        });
                    }
                }).await;

                // Track total integration time after exposure sequence completes
                if result.status == NodeStatus::Success {
                    let total_exposure_time = duration_secs * total_count as f64;
                    if let Ok(mut counter) = context.completed_integration_secs.try_write() {
                        *counter += total_exposure_time;
                    }

                    // Update trigger state with HFR values and exposure counts
                    if let Some(trigger_state_lock) = &context.trigger_state {
                        if let Ok(mut trigger_state) = trigger_state_lock.try_write() {
                            // Update HFR tracking
                            if let Some(median_hfr) = result.hfr_values.iter().copied().fold(None, |acc, val| {
                                Some(acc.map_or(val, |a| (a + val) / 2.0))
                            }) {
                                trigger_state.update_hfr(median_hfr);
                                tracing::debug!("Updated trigger state HFR: {:.2}", median_hfr);
                            }

                            // Increment exposure count for periodic triggers
                            for _ in 0..total_count {
                                trigger_state.increment_exposure_count();
                            }
                            tracing::debug!("Updated trigger state exposure count: {}", trigger_state.completed_exposures);
                        }
                    }

                    // Check exposure triggers defined in config
                    self.check_exposure_triggers(config, &result, context).await;

                    // Also send a progress update with the completed exposure time
                    context.send_progress(ProgressUpdate {
                        node_id: self.id().clone(),
                        status: NodeStatus::Success,
                        message: Some(format!("Completed {} exposures ({:.0}s)", total_count, total_exposure_time)),
                        current_frame: Some(total_count),
                        total_frames: Some(total_count),
                        current_child: None,
                        total_children: None,
                        completed_exposure_secs: Some(total_exposure_time),
                    });
                } else if result.status == NodeStatus::Failure {
                    // Log failure message so it's not silently discarded
                    if let Some(msg) = &result.message {
                        tracing::error!("Exposure failed: {}", msg);
                    }
                }

                result.status
            }
            NodeType::Autofocus(config) => {
                let ctx = context.to_instruction_context().await;
                let node_id = self.id().clone();
                let progress_cb = context.progress_callback.as_ref();

                let progress_fn = |progress: f64, detail: String| {
                    if let Some(cb) = progress_cb {
                        cb(ProgressUpdate {
                            node_id: node_id.clone(),
                            status: NodeStatus::Running,
                            message: Some(format!("Autofocus: {} ({:.0}%)", detail, progress)),
                            current_frame: None,
                            total_frames: None,
                            current_child: None,
                            total_children: None,
                            completed_exposure_secs: None,
                        });
                    }
                };

                let result = execute_autofocus(config, &ctx, Some(&progress_fn)).await;

                // Update trigger state after autofocus completes
                if result.status == NodeStatus::Success {
                    if let Some(trigger_state_lock) = &context.trigger_state {
                        if let Ok(mut trigger_state) = trigger_state_lock.try_write() {
                            // Update HFR baseline after successful autofocus
                            if let Some(best_hfr) = result.hfr_values.first() {
                                trigger_state.update_hfr(*best_hfr);
                                trigger_state.reset_baseline_hfr();
                                tracing::debug!("Reset HFR baseline to {:.2} after autofocus", best_hfr);
                            }

                            // Mark that autofocus was performed
                            trigger_state.mark_autofocus_performed();
                            tracing::debug!("Marked autofocus performed at exposure {}", trigger_state.completed_exposures);
                        }
                    }
                } else if result.status == NodeStatus::Failure {
                    if let Some(msg) = &result.message {
                        tracing::error!("Autofocus failed: {}", msg);
                    }
                }

                result.status
            }
            NodeType::TemperatureCompensation(config) => {
                let ctx = context.to_instruction_context().await;
                let node_id = self.id().clone();
                let progress_cb = context.progress_callback.as_ref();

                let progress_fn = |progress: f64, detail: String| {
                    if let Some(cb) = progress_cb {
                        cb(ProgressUpdate {
                            node_id: node_id.clone(),
                            status: NodeStatus::Running,
                            message: Some(format!("Temp Comp: {} ({:.0}%)", detail, progress)),
                            current_frame: None,
                            total_frames: None,
                            current_child: None,
                            total_children: None,
                            completed_exposure_secs: None,
                        });
                    }
                };

                crate::temperature_compensation::execute_temperature_compensation(config, &ctx, Some(&progress_fn)).await.log_and_get_status("Temperature Compensation")
            }
            NodeType::Dither(config) => {
                let ctx = context.to_instruction_context().await;
                let node_id = self.id().clone();
                let progress_cb = context.progress_callback.as_ref();

                let progress_fn = |progress: f64, detail: String| {
                    if let Some(cb) = progress_cb {
                        cb(ProgressUpdate {
                            node_id: node_id.clone(),
                            status: NodeStatus::Running,
                            message: Some(format!("Dither: {} ({:.0}%)", detail, progress)),
                            current_frame: None,
                            total_frames: None,
                            current_child: None,
                            total_children: None,
                            completed_exposure_secs: None,
                        });
                    }
                };

                let result = execute_dither(config, &ctx, Some(&progress_fn)).await;

                // Update trigger state after dither completes
                if result.status == NodeStatus::Success {
                    if let Some(trigger_state_lock) = &context.trigger_state {
                        if let Ok(mut trigger_state) = trigger_state_lock.try_write() {
                            // Mark that dither was performed
                            trigger_state.mark_dither_performed();
                            tracing::debug!("Marked dither performed at exposure {}", trigger_state.completed_exposures);
                        }
                    }
                } else if result.status == NodeStatus::Failure {
                    if let Some(msg) = &result.message {
                        tracing::error!("Dither failed: {}", msg);
                    }
                }

                result.status
            }
            NodeType::StartGuiding(config) => {
                let ctx = context.to_instruction_context().await;
                let node_id = self.id().clone();
                let progress_cb = context.progress_callback.as_ref();

                let progress_fn = |progress: f64, detail: String| {
                    if let Some(cb) = progress_cb {
                        cb(ProgressUpdate {
                            node_id: node_id.clone(),
                            status: NodeStatus::Running,
                            message: Some(format!("Start Guiding: {} ({:.0}%)", detail, progress)),
                            current_frame: None,
                            total_frames: None,
                            current_child: None,
                            total_children: None,
                            completed_exposure_secs: None,
                        });
                    }
                };

                execute_start_guiding(config, &ctx, Some(&progress_fn)).await.log_and_get_status("Start Guiding")
            }
            NodeType::StopGuiding => {
                let ctx = context.to_instruction_context().await;
                let node_id = self.id().clone();
                let progress_cb = context.progress_callback.as_ref();

                let progress_fn = |progress: f64, detail: String| {
                    if let Some(cb) = progress_cb {
                        cb(ProgressUpdate {
                            node_id: node_id.clone(),
                            status: NodeStatus::Running,
                            message: Some(format!("Stop Guiding: {} ({:.0}%)", detail, progress)),
                            current_frame: None,
                            total_frames: None,
                            current_child: None,
                            total_children: None,
                            completed_exposure_secs: None,
                        });
                    }
                };

                execute_stop_guiding(&ctx, Some(&progress_fn)).await.log_and_get_status("Stop Guiding")
            }
            NodeType::ChangeFilter(config) => {
                let ctx = context.to_instruction_context().await;
                let node_id = self.id().clone();
                let progress_cb = context.progress_callback.as_ref();

                let progress_fn = |progress: f64, detail: String| {
                    if let Some(cb) = progress_cb {
                        cb(ProgressUpdate {
                            node_id: node_id.clone(),
                            status: NodeStatus::Running,
                            message: Some(format!("Filter: {} ({:.0}%)", detail, progress)),
                            current_frame: None,
                            total_frames: None,
                            current_child: None,
                            total_children: None,
                            completed_exposure_secs: None,
                        });
                    }
                };

                let result = execute_filter_change(config, &ctx, Some(&progress_fn)).await;
                if result.status == NodeStatus::Success {
                    context.current_filter = Some(config.filter_name.clone());
                } else if result.status == NodeStatus::Failure {
                    if let Some(msg) = &result.message {
                        tracing::error!("Change Filter failed: {}", msg);
                    }
                }
                result.status
            }
            NodeType::CoolCamera(config) => {
                let ctx = context.to_instruction_context().await;
                let node_id = self.id().clone();
                let progress_cb = context.progress_callback.as_ref();

                let progress_fn = |progress: f64, detail: String| {
                    if let Some(cb) = progress_cb {
                        cb(ProgressUpdate {
                            node_id: node_id.clone(),
                            status: NodeStatus::Running,
                            message: Some(format!("Cool Camera: {} ({:.0}%)", detail, progress)),
                            current_frame: None,
                            total_frames: None,
                            current_child: None,
                            total_children: None,
                            completed_exposure_secs: None,
                        });
                    }
                };

                execute_cool_camera(config, &ctx, Some(&progress_fn)).await.log_and_get_status("Cool Camera")
            }
            NodeType::WarmCamera(config) => {
                let ctx = context.to_instruction_context().await;
                let node_id = self.id().clone();
                let progress_cb = context.progress_callback.as_ref();

                let progress_fn = |progress: f64, detail: String| {
                    if let Some(cb) = progress_cb {
                        cb(ProgressUpdate {
                            node_id: node_id.clone(),
                            status: NodeStatus::Running,
                            message: Some(format!("Warm Camera: {} ({:.0}%)", detail, progress)),
                            current_frame: None,
                            total_frames: None,
                            current_child: None,
                            total_children: None,
                            completed_exposure_secs: None,
                        });
                    }
                };

                execute_warm_camera(config, &ctx, Some(&progress_fn)).await.log_and_get_status("Warm Camera")
            }
            NodeType::MoveRotator(config) => {
                let ctx = context.to_instruction_context().await;
                execute_rotator_move(config, &ctx).await.log_and_get_status("Move Rotator")
            }
            NodeType::Park => {
                let ctx = context.to_instruction_context().await;
                execute_park(&ctx).await.log_and_get_status("Park")
            }
            NodeType::Unpark => {
                let ctx = context.to_instruction_context().await;
                execute_unpark(&ctx).await.log_and_get_status("Unpark")
            }
            NodeType::WaitForTime(config) => {
                let ctx = context.to_instruction_context().await;
                let node_id = self.id().clone();
                let progress_cb = context.progress_callback.as_ref();

                let progress_fn = |progress: f64, detail: String| {
                    if let Some(cb) = progress_cb {
                        cb(ProgressUpdate {
                            node_id: node_id.clone(),
                            status: NodeStatus::Running,
                            message: Some(format!("Wait: {} ({:.0}%)", detail, progress)),
                            current_frame: None,
                            total_frames: None,
                            current_child: None,
                            total_children: None,
                            completed_exposure_secs: None,
                        });
                    }
                };

                execute_wait_time(config, &ctx, Some(&progress_fn)).await.log_and_get_status("Wait For Time")
            }
            NodeType::Delay(config) => {
                let ctx = context.to_instruction_context().await;
                let node_id = self.id().clone();
                let progress_cb = context.progress_callback.as_ref();

                let progress_fn = |progress: f64, detail: String| {
                    if let Some(cb) = progress_cb {
                        cb(ProgressUpdate {
                            node_id: node_id.clone(),
                            status: NodeStatus::Running,
                            message: Some(format!("Delay: {} ({:.0}%)", detail, progress)),
                            current_frame: None,
                            total_frames: None,
                            current_child: None,
                            total_children: None,
                            completed_exposure_secs: None,
                        });
                    }
                };

                execute_delay(config, &ctx, Some(&progress_fn)).await.log_and_get_status("Delay")
            }
            NodeType::Notification(config) => {
                let ctx = context.to_instruction_context().await;
                execute_notification(config, &ctx).await.log_and_get_status("Notification")
            }
            NodeType::RunScript(config) => {
                let ctx = context.to_instruction_context().await;
                execute_script(config, &ctx).await.log_and_get_status("Run Script")
            }
            NodeType::PolarAlignment(config) => {
                let ctx = context.to_instruction_context().await;
                let node_id = self.id().clone();
                let progress_cb = context.progress_callback.as_ref();

                execute_polar_alignment(config, &ctx, |msg, _progress| {
                    if let Some(cb) = progress_cb {
                        cb(ProgressUpdate {
                            node_id: node_id.clone(),
                            status: NodeStatus::Running,
                            message: Some(msg),
                            current_frame: None,
                            total_frames: None,
                            current_child: None,
                            total_children: None,
                            completed_exposure_secs: None,
                        });
                    }
                }).await.log_and_get_status("Polar Alignment")
            }
            NodeType::MeridianFlip(config) => {
                let ctx = context.to_instruction_context().await;
                let node_id = self.id().clone();
                let progress_cb = context.progress_callback.as_ref();

                let progress_fn = |progress: f64, detail: String| {
                    if let Some(cb) = progress_cb {
                        cb(ProgressUpdate {
                            node_id: node_id.clone(),
                            status: NodeStatus::Running,
                            message: Some(format!("Meridian Flip: {} ({:.0}%)", detail, progress)),
                            current_frame: None,
                            total_frames: None,
                            current_child: None,
                            total_children: None,
                            completed_exposure_secs: None,
                        });
                    }
                };

                execute_meridian_flip(config, &ctx, Some(&progress_fn)).await.log_and_get_status("Meridian Flip")
            }
            NodeType::OpenDome(config) => {
                let ctx = context.to_instruction_context().await;
                execute_open_dome(config, &ctx).await.log_and_get_status("Open Dome")
            }
            NodeType::CloseDome(config) => {
                let ctx = context.to_instruction_context().await;
                execute_close_dome(config, &ctx).await.log_and_get_status("Close Dome")
            }
            NodeType::ParkDome(config) => {
                let ctx = context.to_instruction_context().await;
                execute_park_dome(config, &ctx).await.log_and_get_status("Park Dome")
            }
            NodeType::Mosaic(config) => {
                let ctx = context.to_instruction_context().await;
                execute_mosaic(config, &ctx).await.log_and_get_status("Mosaic")
            }
            NodeType::FlatWizard(config) => {
                let ctx = context.to_instruction_context().await;
                crate::flat_wizard::execute_flat_wizard(config, &ctx).await.log_and_get_status("Flat Wizard")
            }
            NodeType::OpenCover(config) => {
                let ctx = context.to_instruction_context().await;
                execute_open_cover(config, &ctx).await.log_and_get_status("Open Cover")
            }
            NodeType::CloseCover(config) => {
                let ctx = context.to_instruction_context().await;
                execute_close_cover(config, &ctx).await.log_and_get_status("Close Cover")
            }
            NodeType::CalibratorOn(config) => {
                let ctx = context.to_instruction_context().await;
                execute_calibrator_on(config, &ctx).await.log_and_get_status("Calibrator On")
            }
            NodeType::CalibratorOff(config) => {
                let ctx = context.to_instruction_context().await;
                execute_calibrator_off(config, &ctx).await.log_and_get_status("Calibrator Off")
            }
        };

        self.status = result;
        context.send_progress(ProgressUpdate {
            node_id: self.id().clone(),
            status: result,
            message: Some(format!("Completed: {}", self.name())),
            current_frame: None,
            total_frames: None,
            current_child: None,
            total_children: None,
            completed_exposure_secs: None,
        });

        result
    }

    fn reset(&mut self) {
        self.status = NodeStatus::Pending;
        self.current_iteration = 0;
        for child in &mut self.children {
            child.reset();
        }
    }

    async fn abort(&mut self) {
        self.status = NodeStatus::Cancelled;
        for child in &mut self.children {
            child.abort().await;
        }
    }

    fn children(&self) -> &[Box<dyn Node>] {
        &self.children
    }

    fn children_mut(&mut self) -> &mut Vec<Box<dyn Node>> {
        &mut self.children
    }

    fn mark_completed(&mut self, node_id: &NodeId) {
        if self.id() == node_id {
            self.status = NodeStatus::Success;
        } else {
            // Propagate to children
            for child in &mut self.children {
                child.mark_completed(node_id);
            }
        }
    }
}

impl RuntimeNode {
    /// Execute a target header node (root node for each target)
    async fn execute_target_header(&mut self, config: TargetHeaderConfig, context: &mut ExecutionContext) -> NodeStatus {
        // Set target context for child nodes
        context.target_name = Some(config.target_name.clone());
        context.target_ra = Some(config.ra_hours);
        context.target_dec = Some(config.dec_degrees);
        context.target_rotation = config.rotation;

        let display_name = config.display_name();
        tracing::info!("Starting target: {} (RA: {:.4}h, Dec: {:.4}°)",
            display_name, config.ra_hours, config.dec_degrees);

        // Check time constraints
        let now = chrono::Utc::now().timestamp();

        // Check start_after constraint
        if let Some(start_after) = config.start_after {
            if now < start_after {
                let wait_secs = start_after - now;
                tracing::info!("Target {} has start_after constraint, waiting {} seconds",
                    display_name, wait_secs);
                // Wait until start time
                tokio::time::sleep(tokio::time::Duration::from_secs(wait_secs as u64)).await;
            }
        }

        // Check end_before constraint
        if let Some(end_before) = config.end_before {
            if now >= end_before {
                tracing::warn!("Target {} has passed its end_before time, skipping",
                    display_name);
                return NodeStatus::Skipped;
            }
        }

        // Update trigger state with target coordinates for drift detection
        if let Some(trigger_state_lock) = &context.trigger_state {
            if let Ok(mut trigger_state) = trigger_state_lock.try_write() {
                let target_ra_degrees = config.ra_hours * 15.0; // Convert hours to degrees
                trigger_state.set_target(target_ra_degrees, config.dec_degrees);
                tracing::debug!("Updated trigger state with target: RA={:.4}°, Dec={:.4}°",
                    target_ra_degrees, config.dec_degrees);
            }
        }

        // Calculate and update meridian flip time for trigger system
        if let (Some(_lat), Some(lon)) = (context.latitude, context.longitude) {
            let now = chrono::Utc::now();
            let meridian_crossing = crate::meridian::calculate_meridian_crossing(
                config.ra_hours,
                lon,
                now
            );

            tracing::debug!("Target {} meridian crossing at {}",
                display_name, meridian_crossing);

            // Store as Unix timestamp for trigger comparison
            context.set_next_meridian_flip_time(Some(meridian_crossing.timestamp()));
        }

        // Check altitude if configured
        if let Some(min_alt) = config.min_altitude {
            let current_alt = context.calculate_altitude().unwrap_or(90.0);
            if current_alt < min_alt {
                tracing::warn!("Target {} is below minimum altitude ({:.1}° < {:.1}°)",
                    display_name, current_alt, min_alt);
                return NodeStatus::Skipped;
            }
        }

        if let Some(max_alt) = config.max_altitude {
            let current_alt = context.calculate_altitude().unwrap_or(0.0);
            if current_alt > max_alt {
                tracing::warn!("Target {} is above maximum altitude ({:.1}° > {:.1}°)",
                    display_name, current_alt, max_alt);
                return NodeStatus::Skipped;
            }
        }

        // Execute children in sequence
        self.execute_children_sequential(context).await
    }

    /// Execute a loop node
    async fn execute_loop(&mut self, config: LoopConfig, context: &mut ExecutionContext) -> NodeStatus {
        self.current_iteration = 0;

        // Determine max iterations based on condition
        let max_iterations = match config.condition {
            LoopCondition::Count => config.iterations.unwrap_or(1),
            _ => u32::MAX, // Other conditions are checked dynamically
        };

        loop {
            if context.is_cancelled().await {
                return NodeStatus::Cancelled;
            }

            // Check loop condition
            let should_continue = match config.condition {
                LoopCondition::Count => self.current_iteration < max_iterations,
                LoopCondition::UntilTime => {
                    if let Some(until) = config.condition_value {
                        chrono::Utc::now().timestamp() < (until as i64)
                    } else {
                        false
                    }
                }
                LoopCondition::AltitudeBelow => {
                    if let Some(threshold) = config.condition_value {
                        let current_alt = context.calculate_altitude().unwrap_or(90.0);
                        current_alt >= threshold
                    } else {
                        false
                    }
                }
                LoopCondition::AltitudeAbove => {
                    if let Some(threshold) = config.condition_value {
                        let current_alt = context.calculate_altitude().unwrap_or(0.0);
                        current_alt <= threshold
                    } else {
                        false
                    }
                }
                LoopCondition::IntegrationTime => {
                    if let Some(target_secs) = config.condition_value {
                        // Get actual completed integration time from context
                        let integrated_secs = context.get_completed_integration_secs().await;
                        integrated_secs < target_secs
                    } else {
                        false
                    }
                }
                LoopCondition::Forever => true,
                LoopCondition::WhileDark => {
                    context.is_dark()
                }
            };

            if !should_continue {
                break;
            }

            self.current_iteration += 1;
            tracing::info!("Loop iteration {}", self.current_iteration);

            let total_children = match config.condition {
                LoopCondition::Count => Some(max_iterations as usize),
                _ => None,
            };

            context.send_progress(ProgressUpdate {
                node_id: self.id().clone(),
                status: NodeStatus::Running,
                message: Some(format!("Loop iteration {}", self.current_iteration)),
                current_frame: None,
                total_frames: None,
                current_child: Some(self.current_iteration as usize),
                total_children,
                completed_exposure_secs: None,
            });

            // Reset children for this iteration
            for child in &mut self.children {
                child.reset();
            }

            // Execute children
            let result = self.execute_children_sequential(context).await;
            if result == NodeStatus::Failure || result == NodeStatus::Cancelled {
                return result;
            }
        }

        NodeStatus::Success
    }

    /// Execute a parallel node with true concurrent execution
    async fn execute_parallel(&mut self, config: ParallelConfig, context: &mut ExecutionContext) -> NodeStatus {
        use std::sync::atomic::AtomicUsize;
        use tokio::sync::Mutex as TokioMutex;

        let total_children = self.children.len();
        if total_children == 0 {
            return NodeStatus::Success;
        }

        let required = config.required_successes.unwrap_or(total_children);
        let node_id = self.id().clone();

        // Send initial progress
        context.send_progress(ProgressUpdate {
            node_id: node_id.clone(),
            status: NodeStatus::Running,
            message: Some(format!("Running {} parallel branches", total_children)),
            current_frame: None,
            total_frames: None,
            current_child: Some(0),
            total_children: Some(total_children),
            completed_exposure_secs: None,
        });

        // Create shared state for tracking results
        let success_count = Arc::new(AtomicUsize::new(0));
        let cancelled = Arc::new(AtomicBool::new(false));

        // Take ownership of children and wrap in Mutex for concurrent access
        let children = std::mem::take(&mut self.children);
        let children: Vec<Arc<TokioMutex<Box<dyn Node>>>> = children
            .into_iter()
            .map(|c| Arc::new(TokioMutex::new(c)))
            .collect();

        // Create shared context values
        let is_cancelled = context.is_cancelled.clone();
        let is_paused = context.is_paused.clone();
        let resume_notify = context.resume_notify.clone();
        let device_ops = context.device_ops.clone();
        let completed_integration = context.completed_integration_secs.clone();
        let target_ra = context.target_ra;
        let target_dec = context.target_dec;
        let target_name = context.target_name.clone();
        let target_rotation = context.target_rotation;
        let current_filter = context.current_filter.clone();
        let camera_id = context.camera_id.clone();
        let mount_id = context.mount_id.clone();
        let focuser_id = context.focuser_id.clone();
        let filterwheel_id = context.filterwheel_id.clone();
        let rotator_id = context.rotator_id.clone();
        let dome_id = context.dome_id.clone();
        let cover_calibrator_id = context.cover_calibrator_id.clone();
        let save_path = context.save_path.clone();
        let latitude = context.latitude;
        let longitude = context.longitude;

        // Spawn tasks for each child
        let handles: Vec<_> = children.iter().enumerate().map(|(i, child)| {
            let child = child.clone();
            let success_count = success_count.clone();
            let cancelled = cancelled.clone();
            let is_cancelled = is_cancelled.clone();
            let is_paused = is_paused.clone();
            let resume_notify = resume_notify.clone();
            let device_ops = device_ops.clone();
            let completed_integration = completed_integration.clone();
            let node_id = node_id.clone();
            let target_name = target_name.clone();
            let current_filter = current_filter.clone();
            let camera_id = camera_id.clone();
            let mount_id = mount_id.clone();
            let focuser_id = focuser_id.clone();
            let filterwheel_id = filterwheel_id.clone();
            let rotator_id = rotator_id.clone();
            let dome_id = dome_id.clone();
            let cover_calibrator_id = cover_calibrator_id.clone();
            let save_path = save_path.clone();

            tokio::spawn(async move {
                // Check for cancellation before starting
                if is_cancelled.load(Ordering::Relaxed) || cancelled.load(Ordering::Relaxed) {
                    return (i, NodeStatus::Cancelled);
                }

                // Create branch-specific context
                let mut branch_context = ExecutionContext {
                    node_id: format!("{}_branch_{}", node_id, i),
                    target_ra,
                    target_dec,
                    target_name,
                    target_rotation,
                    current_filter,
                    is_cancelled: is_cancelled.clone(),
                    is_paused,
                    resume_notify,
                    progress_callback: None,
                    camera_id,
                    mount_id,
                    focuser_id,
                    filterwheel_id,
                    rotator_id,
                    dome_id,
                    cover_calibrator_id,
                    save_path,
                    latitude,
                    longitude,
                    device_ops,
                    completed_integration_secs: completed_integration,
                    trigger_state: None,
                };

                // Execute the child with mutex guard
                let mut child_guard = child.lock().await;
                let result = child_guard.execute(&mut branch_context).await;

                match result {
                    NodeStatus::Success => {
                        success_count.fetch_add(1, Ordering::Relaxed);
                    }
                    NodeStatus::Cancelled => {
                        cancelled.store(true, Ordering::Relaxed);
                    }
                    _ => {}
                }

                (i, result)
            })
        }).collect();

        // Wait for all tasks to complete
        let _results: Vec<_> = futures::future::join_all(handles)
            .await
            .into_iter()
            .filter_map(|r| r.ok())
            .collect();

        // Restore children from mutex wrappers
        // All spawned tasks have completed, so Arc::try_unwrap should succeed
        let mut restored_children = Vec::with_capacity(children.len());
        for child_mutex in children {
            match Arc::try_unwrap(child_mutex) {
                Ok(mutex) => {
                    restored_children.push(mutex.into_inner());
                }
                Err(_) => {
                    // This should never happen since all tasks completed
                    tracing::error!("Failed to restore child from parallel execution - this is a bug");
                    // Can't recover the child, leave it out
                }
            }
        }
        self.children = restored_children;

        // Check for cancellation
        if is_cancelled.load(Ordering::Relaxed) || cancelled.load(Ordering::Relaxed) {
            return NodeStatus::Cancelled;
        }

        // Check results
        let successes = success_count.load(Ordering::Relaxed);

        context.send_progress(ProgressUpdate {
            node_id: node_id.clone(),
            status: if successes >= required { NodeStatus::Success } else { NodeStatus::Failure },
            message: Some(format!("{}/{} branches succeeded", successes, total_children)),
            current_frame: None,
            total_frames: None,
            current_child: Some(total_children),
            total_children: Some(total_children),
            completed_exposure_secs: None,
        });

        if successes >= required {
            NodeStatus::Success
        } else {
            tracing::warn!("Parallel node: only {}/{} branches succeeded, required {}",
                successes, total_children, required);
            NodeStatus::Failure
        }
    }

    /// Execute a conditional node
    async fn execute_conditional(&mut self, config: ConditionalConfig, context: &mut ExecutionContext) -> NodeStatus {
        let condition_met = match &config.condition {
            ConditionalCheck::Always => true,
            ConditionalCheck::AltitudeAbove(min_alt) => {
                let current_alt = context.calculate_altitude().unwrap_or(0.0);
                current_alt > *min_alt
            }
            ConditionalCheck::TimeAfter(after) => {
                chrono::Utc::now().timestamp() > *after
            }
            ConditionalCheck::GuidingRmsBelow(threshold) => {
                // Get guiding RMS from PHD2
                match context.device_ops.guider_get_status().await {
                    Ok(status) => status.rms_total < *threshold,
                    Err(_) => true, // If guider not available, pass condition
                }
            }
            ConditionalCheck::HfrBelow(threshold) => {
                // HFR requires recent image analysis - use reasonable default
                // In production, this would check last captured image stats
                let current_hfr = 2.5; // Reasonable default for typical seeing
                current_hfr < *threshold
            }
            ConditionalCheck::WeatherSafe => {
                // Weather safety requires weather station - assume safe if not available
                // In production, would check connected weather device
                true
            }
            ConditionalCheck::MoonSeparationAbove(degrees) => {
                // Calculate moon separation from target
                context.calculate_moon_separation().map_or(true, |sep| sep > *degrees)
            }
            ConditionalCheck::SafetyMonitorSafe => {
                // In production, check actual safety monitor
                // For now, assume safe
                true
            }
        };

        if condition_met {
            self.execute_children_sequential(context).await
        } else {
            tracing::info!("Conditional check failed, skipping children");
            NodeStatus::Skipped
        }
    }

    /// Execute a recovery node
    async fn execute_recovery(&mut self, config: RecoveryConfig, context: &mut ExecutionContext) -> NodeStatus {
        let mut attempts = 0;
        let max_attempts = config.max_retries.max(1);

        loop {
            attempts += 1;
            tracing::info!("Execution attempt {}/{}", attempts, max_attempts);

            // Reset children
            for child in &mut self.children {
                child.reset();
            }

            let result = self.execute_children_sequential(context).await;

            if result == NodeStatus::Success || result == NodeStatus::Cancelled {
                return result;
            }

            if attempts >= max_attempts {
                tracing::warn!("Max recovery attempts reached");
                return match config.recovery_action {
                    RecoveryAction::Continue => NodeStatus::Success,
                    RecoveryAction::NextTarget => NodeStatus::Skipped,
                    RecoveryAction::ParkAndAbort => {
                        let ctx = context.to_instruction_context().await;
                        let _ = execute_park(&ctx).await;
                        NodeStatus::Failure
                    }
                    _ => NodeStatus::Failure,
                };
            }

            // Execute recovery action
            match &config.recovery_action {
                RecoveryAction::Retry { .. } => {
                    tracing::info!("Retrying...");
                }
                RecoveryAction::Autofocus => {
                    tracing::info!("Running recovery autofocus...");
                    let ctx = context.to_instruction_context().await;
                    // Pass None for progress callback in recovery autofocus
                    let _ = execute_autofocus(&crate::AutofocusConfig::default(), &ctx, None).await;
                }
                RecoveryAction::Pause => {
                    tracing::info!("Pausing for manual intervention...");
                    // Wait for user to resume execution
                    if !context.pause_and_wait_for_resume().await {
                        // Cancelled while paused
                        return NodeStatus::Cancelled;
                    }
                    // Resumed - continue with retry
                    tracing::info!("Resumed after pause, retrying...");
                }
                _ => {}
            }
        }
    }

    /// Execute children in sequence
    async fn execute_children_sequential(&mut self, context: &mut ExecutionContext) -> NodeStatus {
        let total = self.children.len();
        let node_id = self.id().clone();

        tracing::debug!("execute_children_sequential: node {} has {} children", node_id, total);

        if total == 0 {
            tracing::warn!("Node {} has no children to execute", node_id);
            return NodeStatus::Success;
        }

        tracing::info!("About to enter for loop with {} children", total);

        for (i, child) in self.children.iter_mut().enumerate() {
            tracing::info!("FOR LOOP ENTERED: iteration {} of {}", i, total);

            if context.is_cancelled().await {
                tracing::debug!("Execution cancelled before child {}", i);
                return NodeStatus::Cancelled;
            }

            tracing::info!("Executing child {}/{}: '{}' (id={})",
                i + 1, total, child.name(), child.id());

            context.send_progress(ProgressUpdate {
                node_id: node_id.clone(),
                status: NodeStatus::Running,
                message: Some(format!("Step {}/{}: {}", i + 1, total, child.name())),
                current_frame: None,
                total_frames: None,
                current_child: Some(i),
                total_children: Some(total),
                completed_exposure_secs: None,
            });

            let result = child.execute(context).await;

            tracing::info!("Child '{}' completed with status: {:?}", child.name(), result);

            if result == NodeStatus::Failure || result == NodeStatus::Cancelled {
                return result;
            }
        }

        NodeStatus::Success
    }
}

// ============================================================================
// Astronomical Helper Functions
// ============================================================================

/// Calculate Julian Day from a chrono DateTime
pub fn julian_day(dt: &chrono::DateTime<chrono::Utc>) -> f64 {
    use chrono::{Datelike, Timelike};
    let year = dt.year();
    let month = dt.month();
    let day = dt.day();
    let hour = dt.hour();
    let minute = dt.minute();
    let second = dt.second();

    let (y, m) = if month <= 2 {
        (year - 1, month + 12)
    } else {
        (year, month)
    };

    let a = y / 100;
    let b = 2 - a + a / 4;

    let jd = (365.25 * (y as f64 + 4716.0)).floor() +
             (30.6001 * (m as f64 + 1.0)).floor() +
             day as f64 + b as f64 - 1524.5;
    
    let time_fraction = (hour as f64 + minute as f64 / 60.0 + second as f64 / 3600.0) / 24.0;
    
    jd + time_fraction
}

pub fn local_sidereal_time(jd: f64, longitude: f64) -> f64 {
    let t = (jd - 2451545.0) / 36525.0;

    // Greenwich Mean Sidereal Time in degrees
    let gmst = 280.46061837 + 360.98564736629 * (jd - 2451545.0) +
               0.000387933 * t * t - t * t * t / 38710000.0;

    let lst = (gmst + longitude) % 360.0;
    if lst < 0.0 { (lst + 360.0) / 15.0 } else { lst / 15.0 }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_execution_context_creation() {
        let ctx = ExecutionContext::new("test_node".to_string());
        assert_eq!(ctx.node_id, "test_node");
        assert!(ctx.target_ra.is_none());
        assert!(ctx.target_dec.is_none());
        assert!(ctx.camera_id.is_none());
    }

    #[test]
    fn test_execution_context_with_target() {
        let ctx = ExecutionContext::new("test_node".to_string())
            .with_target("M31".to_string(), 10.68, 41.27, Some(45.0));

        assert_eq!(ctx.target_name, Some("M31".to_string()));
        assert_eq!(ctx.target_ra, Some(10.68));
        assert_eq!(ctx.target_dec, Some(41.27));
        assert_eq!(ctx.target_rotation, Some(45.0));
    }

    #[test]
    fn test_execution_context_cancellation() {
        let ctx = ExecutionContext::new("test_node".to_string());

        assert!(!ctx.is_cancelled.load(Ordering::Relaxed));
        ctx.is_cancelled.store(true, Ordering::Relaxed);
        assert!(ctx.is_cancelled.load(Ordering::Relaxed));
    }

    #[test]
    fn test_execution_context_pause() {
        let ctx = ExecutionContext::new("test_node".to_string());

        assert!(!ctx.is_paused.load(Ordering::Relaxed));
        ctx.is_paused.store(true, Ordering::Relaxed);
        assert!(ctx.is_paused.load(Ordering::Relaxed));
    }

    #[test]
    fn test_progress_update_creation() {
        let update = ProgressUpdate {
            node_id: "node1".to_string(),
            status: NodeStatus::Running,
            message: Some("Capturing frame".to_string()),
            current_frame: Some(5),
            total_frames: Some(10),
            current_child: None,
            total_children: None,
            completed_exposure_secs: Some(60.0),
        };

        assert_eq!(update.node_id, "node1");
        assert_eq!(update.status, NodeStatus::Running);
        assert_eq!(update.current_frame, Some(5));
        assert_eq!(update.total_frames, Some(10));
        assert_eq!(update.completed_exposure_secs, Some(60.0));
    }

    #[test]
    fn test_julian_day_calculation() {
        // Test J2000 epoch: January 1, 2000 at 12:00 UT
        use chrono::{DateTime, Utc, TimeZone};

        let dt = Utc.with_ymd_and_hms(2000, 1, 1, 12, 0, 0).unwrap();

        let jd = julian_day(&dt);
        // J2000 epoch should be exactly 2451545.0
        assert!((jd - 2451545.0).abs() < 0.001);
    }

    #[test]
    fn test_julian_day_another_epoch() {
        // Test another known date
        use chrono::{DateTime, Utc, TimeZone};

        let dt = Utc.with_ymd_and_hms(2024, 1, 1, 0, 0, 0).unwrap();

        let jd = julian_day(&dt);
        // JD for Jan 1, 2024 at 0:00 UT should be approximately 2460310.5
        assert!((jd - 2460310.5).abs() < 0.1);
    }

    #[test]
    fn test_local_sidereal_time() {
        // At J2000 epoch at Greenwich (longitude 0), GMST should be close to 18.697 hours
        let jd = 2451545.0;
        let lst = local_sidereal_time(jd, 0.0);

        // LST at J2000 at Greenwich should be approximately 18.697 hours
        assert!(lst > 18.0 && lst < 19.0);
    }

    #[test]
    fn test_local_sidereal_time_with_longitude() {
        let jd = 2451545.0;

        // LST should increase eastward
        let lst_greenwich = local_sidereal_time(jd, 0.0);
        let lst_east = local_sidereal_time(jd, 15.0); // 15 degrees east = 1 hour difference

        // East should be about 1 hour ahead (difference should be ~1 hour)
        let diff = lst_east - lst_greenwich;
        assert!((diff - 1.0).abs() < 0.1 || (diff + 23.0).abs() < 0.1); // Handle wrap
    }
}
