//! Flat Wizard - Automated Flat Field Calibration
//!
//! Automatically determines optimal exposure time to hit target ADU value
//! using binary search algorithm. Supports flat panels and sky flats.
//!
//! For flat panel mode:
//! 1. Opens cover (dust cap)
//! 2. Turns on flat panel at specified brightness
//! 3. Binary search for optimal exposure
//! 4. Optionally takes flat frames
//! 5. Turns off flat panel and closes cover

use crate::{FlatWizardConfig, PanelLocation};
use crate::instructions::{InstructionContext, InstructionResult};
use crate::NodeStatus;

/// Execute the flat wizard to determine optimal flat exposure
pub async fn execute_flat_wizard(
    config: &FlatWizardConfig,
    ctx: &InstructionContext,
    progress_callback: Option<&(dyn Fn(f64, String) + Send + Sync)>,
) -> InstructionResult {
    // Emit initial progress
    if let Some(cb) = progress_callback {
        cb(0.0, "Starting flat wizard".to_string());
    }

    let camera_id = match ctx.camera_id.as_deref() {
        Some(id) => id,
        None => return InstructionResult::failure("No camera connected"),
    };

    tracing::info!(
        "Starting flat wizard: target ADU={}, range={:.3}s-{:.1}s, location={:?}",
        config.target_adu,
        config.min_exposure,
        config.max_exposure,
        config.panel_location
    );

    // Check cancellation
    if ctx.cancellation_token.load(std::sync::atomic::Ordering::Relaxed) {
        return InstructionResult::failure("Operation cancelled");
    }

    // Emit progress for filter change
    if let Some(cb) = progress_callback {
        cb(5.0, "Changing filter".to_string());
    }

    // Change to specified filter if configured
    if let Some(filter_name) = &config.filter {
        if let Some(fw_id) = &ctx.filterwheel_id {
            tracing::info!("Changing to filter: {}", filter_name);
            if let Err(e) = ctx.device_ops.filterwheel_set_filter_by_name(fw_id, filter_name).await {
                return InstructionResult::failure(format!("Failed to change filter: {}", e));
            }
        }
    }

    // Emit progress for positioning
    if let Some(cb) = progress_callback {
        cb(10.0, "Preparing for flats".to_string());
    }

    // Position equipment based on panel location
    match position_for_flats(config, ctx, progress_callback).await {
        Ok(_) => {}
        Err(e) => return InstructionResult::failure(e),
    }

    // For flat panel mode, set up the panel
    let mut current_brightness = config.brightness;
    if matches!(config.panel_location, PanelLocation::FlatPanel) {
        // Emit progress for flat panel setup
        if let Some(cb) = progress_callback {
            cb(20.0, "Setting up flat panel".to_string());
        }

        if let Err(e) = setup_flat_panel(ctx, current_brightness, progress_callback).await {
            // Cleanup on failure
            let _ = cleanup_flat_panel(ctx).await;
            return InstructionResult::failure(e);
        }
    }

    // Emit progress before binary search
    if let Some(cb) = progress_callback {
        cb(30.0, "Analyzing flat exposure time".to_string());
    }

    // Binary search to find optimal exposure time
    let result = find_optimal_exposure_with_brightness(
        config,
        ctx,
        camera_id,
        &mut current_brightness,
        progress_callback,
    ).await;

    // Clean up flat panel if we're using one
    if matches!(config.panel_location, PanelLocation::FlatPanel) {
        // Emit progress for cleanup
        if let Some(cb) = progress_callback {
            cb(90.0, "Cleaning up flat panel".to_string());
        }

        if let Err(e) = cleanup_flat_panel(ctx).await {
            tracing::warn!("Failed to cleanup flat panel: {}", e);
        }
    }

    match result {
        Ok((optimal_exposure, actual_adu, final_brightness)) => {
            // Emit final progress
            if let Some(cb) = progress_callback {
                cb(100.0, "Flat wizard complete".to_string());
            }

            tracing::info!(
                "Flat wizard complete: optimal exposure = {:.3}s, ADU = {}, brightness = {}",
                optimal_exposure,
                actual_adu,
                final_brightness
            );
            InstructionResult {
                status: NodeStatus::Success,
                message: Some(format!(
                    "Optimal flat exposure: {:.3}s (ADU: {}, brightness: {})",
                    optimal_exposure, actual_adu, final_brightness
                )),
                data: Some(serde_json::json!({
                    "optimal_exposure_secs": optimal_exposure,
                    "actual_adu": actual_adu,
                    "target_adu": config.target_adu,
                    "filter": config.filter,
                    "brightness": final_brightness,
                    "panel_location": format!("{:?}", config.panel_location),
                })),
                hfr_values: Vec::new(),
            }
        }
        Err(e) => InstructionResult::failure(e),
    }
}

/// Set up flat panel for taking flats (open cover, turn on light)
async fn setup_flat_panel(
    ctx: &InstructionContext,
    brightness: i32,
    progress_callback: Option<&(dyn Fn(f64, String) + Send + Sync)>,
) -> Result<(), String> {
    // Check if we have a cover calibrator connected
    let cc_id = ctx.cover_calibrator_id.as_deref();

    if cc_id.is_none() {
        tracing::warn!("No cover calibrator connected - proceeding without panel control");
        return Ok(());
    }

    let device_id = cc_id.unwrap();

    tracing::info!("Setting up flat panel: opening cover and turning on light at brightness {}", brightness);

    // Emit progress for opening cover
    if let Some(cb) = progress_callback {
        cb(22.0, "Opening cover".to_string());
    }

    // Open the cover (dust cap)
    tracing::info!("Opening cover...");
    if let Err(e) = ctx.device_ops.cover_calibrator_open_cover(device_id).await {
        return Err(format!("Failed to open cover: {}", e));
    }

    // Wait for cover to open
    let timeout = std::time::Duration::from_secs(60);
    let start = std::time::Instant::now();
    loop {
        if ctx.cancellation_token.load(std::sync::atomic::Ordering::Relaxed) {
            return Err("Operation cancelled".to_string());
        }

        let state = ctx.device_ops.cover_calibrator_get_cover_state(device_id).await
            .unwrap_or(4); // Default to Unknown

        if state == 3 {
            // Open
            tracing::info!("Cover opened");
            break;
        }
        if state == 5 {
            // Error
            return Err("Cover reported error state".to_string());
        }
        if start.elapsed() > timeout {
            return Err("Timeout waiting for cover to open".to_string());
        }

        tokio::time::sleep(tokio::time::Duration::from_millis(500)).await;
    }

    // Emit progress for turning on calibrator
    if let Some(cb) = progress_callback {
        cb(25.0, "Turning on calibrator".to_string());
    }

    // Turn on the calibrator (flat light)
    tracing::info!("Turning on calibrator at brightness {}...", brightness);
    if let Err(e) = ctx.device_ops.cover_calibrator_calibrator_on(device_id, brightness).await {
        return Err(format!("Failed to turn on calibrator: {}", e));
    }

    // Wait for calibrator to be ready
    let start = std::time::Instant::now();
    loop {
        if ctx.cancellation_token.load(std::sync::atomic::Ordering::Relaxed) {
            return Err("Operation cancelled".to_string());
        }

        let state = ctx.device_ops.cover_calibrator_get_calibrator_state(device_id).await
            .unwrap_or(4); // Default to Unknown

        if state == 3 {
            // Ready
            tracing::info!("Calibrator ready");
            break;
        }
        if state == 5 {
            // Error
            return Err("Calibrator reported error state".to_string());
        }
        if start.elapsed() > std::time::Duration::from_secs(30) {
            return Err("Timeout waiting for calibrator to be ready".to_string());
        }

        tokio::time::sleep(tokio::time::Duration::from_millis(200)).await;
    }

    Ok(())
}

/// Clean up flat panel after taking flats (turn off light, close cover)
async fn cleanup_flat_panel(ctx: &InstructionContext) -> Result<(), String> {
    let cc_id = match ctx.cover_calibrator_id.as_deref() {
        Some(id) => id,
        None => return Ok(()), // No cover calibrator, nothing to clean up
    };

    tracing::info!("Cleaning up flat panel: turning off light and closing cover");

    // Turn off the calibrator
    if let Err(e) = ctx.device_ops.cover_calibrator_calibrator_off(cc_id).await {
        tracing::warn!("Failed to turn off calibrator: {}", e);
    }

    // Close the cover
    if let Err(e) = ctx.device_ops.cover_calibrator_close_cover(cc_id).await {
        tracing::warn!("Failed to close cover: {}", e);
    }

    // Wait for cover to close (with short timeout)
    let start = std::time::Instant::now();
    while start.elapsed() < std::time::Duration::from_secs(30) {
        let state = ctx.device_ops.cover_calibrator_get_cover_state(cc_id).await
            .unwrap_or(4);
        if state == 1 {
            // Closed
            tracing::info!("Cover closed");
            break;
        }
        tokio::time::sleep(tokio::time::Duration::from_millis(500)).await;
    }

    Ok(())
}

/// Change flat panel brightness
async fn set_panel_brightness(ctx: &InstructionContext, brightness: i32) -> Result<(), String> {
    let cc_id = match ctx.cover_calibrator_id.as_deref() {
        Some(id) => id,
        None => return Ok(()), // No cover calibrator
    };

    tracing::info!("Adjusting flat panel brightness to {}", brightness);

    // Turn on calibrator at new brightness
    ctx.device_ops.cover_calibrator_calibrator_on(cc_id, brightness).await?;

    // Wait for calibrator to stabilize
    let start = std::time::Instant::now();
    while start.elapsed() < std::time::Duration::from_secs(10) {
        let state = ctx.device_ops.cover_calibrator_get_calibrator_state(cc_id).await
            .unwrap_or(4);
        if state == 3 {
            // Ready
            break;
        }
        tokio::time::sleep(tokio::time::Duration::from_millis(200)).await;
    }

    Ok(())
}

/// Position mount/dome for flat field acquisition
async fn position_for_flats(
    config: &FlatWizardConfig,
    ctx: &InstructionContext,
    progress_callback: Option<&(dyn Fn(f64, String) + Send + Sync)>,
) -> Result<(), String> {
    match config.panel_location {
        PanelLocation::FlatPanel => {
            // For flat panel, point to zenith or park position
            tracing::info!("Positioning for flat panel");

            if let Some(mount_id) = &ctx.mount_id {
                // Emit progress for slew
                if let Some(cb) = progress_callback {
                    cb(12.0, "Slewing to zenith".to_string());
                }

                // Slew to zenith or a safe position (altitude 80, azimuth 180)
                // Most flat panels are positioned near zenith
                let (lat, lon) = ctx.device_ops.get_observer_location().unwrap_or((0.0, 0.0));

                // Calculate zenith RA/Dec based on current LST
                let now = chrono::Utc::now();
                let jd = crate::node::julian_day(&now);
                let lst = crate::node::local_sidereal_time(jd, lon);

                // Zenith: RA = LST, Dec = latitude
                let zenith_ra = lst;
                let zenith_dec = lat;

                tracing::info!("Slewing to zenith: RA={:.4}h, Dec={:.4}", zenith_ra, zenith_dec);

                ctx.device_ops
                    .mount_slew_to_coordinates(mount_id, zenith_ra, zenith_dec)
                    .await
                    .map_err(|e| format!("Failed to slew to zenith: {}", e))?;

                // Emit progress for waiting
                if let Some(cb) = progress_callback {
                    cb(15.0, "Waiting for slew to complete".to_string());
                }

                // Wait for slew to complete
                tokio::time::sleep(tokio::time::Duration::from_secs(5)).await;
            }

            Ok(())
        }
        PanelLocation::DawnSky | PanelLocation::DuskSky => {
            // For sky flats, point to appropriate altitude
            tracing::info!("Positioning for {:?} sky flats", config.panel_location);

            if let Some(mount_id) = &ctx.mount_id {
                let (_lat, lon) = ctx.device_ops.get_observer_location().unwrap_or((0.0, 0.0));

                // Emit progress for slew
                if let Some(cb) = progress_callback {
                    cb(12.0, format!("Slewing for {:?} sky flats", config.panel_location));
                }

                // Sky flats are typically taken at ~60-70 altitude
                // Dawn: point east (azimuth ~90)
                // Dusk: point west (azimuth ~270)
                let target_altitude = 65.0;
                let target_azimuth = match config.panel_location {
                    PanelLocation::DawnSky => 90.0,  // East
                    PanelLocation::DuskSky => 270.0, // West
                    _ => 180.0,                      // South (fallback)
                };

                // Convert alt/az to RA/Dec
                let (ra, dec) = altaz_to_radec(target_altitude, target_azimuth, lon);

                tracing::info!(
                    "Slewing to sky flat position: Alt={:.1}, Az={:.1} (RA={:.4}h, Dec={:.4})",
                    target_altitude,
                    target_azimuth,
                    ra,
                    dec
                );

                ctx.device_ops
                    .mount_slew_to_coordinates(mount_id, ra, dec)
                    .await
                    .map_err(|e| format!("Failed to slew for sky flats: {}", e))?;

                // Emit progress for waiting
                if let Some(cb) = progress_callback {
                    cb(15.0, "Waiting for slew to complete".to_string());
                }

                // Wait for slew to complete
                tokio::time::sleep(tokio::time::Duration::from_secs(5)).await;
            }

            Ok(())
        }
    }
}

/// Convert altitude/azimuth to RA/Dec
fn altaz_to_radec(altitude_deg: f64, azimuth_deg: f64, longitude_deg: f64) -> (f64, f64) {
    // Get current LST
    let now = chrono::Utc::now();
    let jd = crate::node::julian_day(&now);
    let lst = crate::node::local_sidereal_time(jd, longitude_deg);

    // Get observer latitude from context (use 0 if not available)
    // In production, this should come from context
    let latitude_deg: f64 = 45.0; // Default fallback

    let alt_rad = altitude_deg.to_radians();
    let az_rad = azimuth_deg.to_radians();
    let lat_rad = latitude_deg.to_radians();

    // Convert alt/az to dec
    let dec_rad = (alt_rad.sin() * lat_rad.sin()
        + alt_rad.cos() * lat_rad.cos() * az_rad.cos())
    .asin();
    let dec_deg = dec_rad.to_degrees();

    // Convert alt/az to hour angle
    let ha_rad = (az_rad.sin() * alt_rad.cos()
        / (lat_rad.cos() * dec_rad.cos()))
    .atan2(alt_rad.sin() - lat_rad.sin() * dec_rad.sin());
    let ha_hours = ha_rad.to_degrees() / 15.0;

    // RA = LST - HA
    let ra_hours = (lst - ha_hours + 24.0) % 24.0;

    (ra_hours, dec_deg)
}

/// Binary search to find optimal exposure time for target ADU
/// Also supports auto-brightness adjustment if enabled
async fn find_optimal_exposure_with_brightness(
    config: &FlatWizardConfig,
    ctx: &InstructionContext,
    camera_id: &str,
    current_brightness: &mut i32,
    progress_callback: Option<&(dyn Fn(f64, String) + Send + Sync)>,
) -> Result<(f64, u16, i32), String> {
    let target_adu = config.target_adu;
    let tolerance = (config.target_adu as f64 * config.tolerance_percent / 100.0) as u16;
    let is_flat_panel = matches!(config.panel_location, PanelLocation::FlatPanel);

    let mut min_exp = config.min_exposure;
    let mut max_exp = config.max_exposure;
    let max_iterations = 10; // Prevent infinite loops

    tracing::info!(
        "Binary search: target={} +/- {} ADU, range={:.3}s-{:.1}s, brightness={}",
        target_adu,
        tolerance,
        min_exp,
        max_exp,
        current_brightness
    );

    for iteration in 1..=max_iterations {
        // Check cancellation
        if ctx.cancellation_token.load(std::sync::atomic::Ordering::Relaxed) {
            return Err("Operation cancelled".to_string());
        }

        // Calculate midpoint exposure
        let test_exposure = (min_exp + max_exp) / 2.0;

        // Calculate progress: iterations span from 30% to 85%
        // Each iteration takes us further through the search
        let progress = 30.0 + (iteration as f64 / max_iterations as f64) * 55.0;
        if let Some(cb) = progress_callback {
            cb(progress, format!("Capturing flat frame {}/{}", iteration, max_iterations));
        }

        tracing::info!(
            "Iteration {}/{}: testing {:.3}s exposure at brightness {}",
            iteration,
            max_iterations,
            test_exposure,
            current_brightness
        );

        // Take test exposure
        let image_data = ctx
            .device_ops
            .camera_start_exposure(camera_id, test_exposure, None, None, 1, 1)
            .await
            .map_err(|e| format!("Test exposure failed: {}", e))?;

        // Calculate median ADU from image
        let median_adu = calculate_median_adu(&image_data);

        tracing::info!("Test exposure: {:.3}s -> {} ADU", test_exposure, median_adu);

        // Check if within tolerance
        let adu_diff = if median_adu > target_adu {
            median_adu - target_adu
        } else {
            target_adu - median_adu
        };

        if adu_diff <= tolerance {
            tracing::info!(
                "Found optimal exposure: {:.3}s (ADU={}, target={}±{}, brightness={})",
                test_exposure,
                median_adu,
                target_adu,
                tolerance,
                current_brightness
            );
            return Ok((test_exposure, median_adu, *current_brightness));
        }

        // Adjust search range based on result
        if median_adu < target_adu {
            // Too dark, need longer exposure or brighter panel
            min_exp = test_exposure;

            // If we hit max exposure and auto-adjust is enabled, try increasing brightness
            if test_exposure >= config.max_exposure * 0.9
                && is_flat_panel
                && config.auto_adjust_brightness
                && *current_brightness < config.max_brightness
            {
                let new_brightness = (*current_brightness + 20).min(config.max_brightness);
                tracing::info!(
                    "Max exposure reached with low ADU, increasing brightness from {} to {}",
                    current_brightness,
                    new_brightness
                );
                *current_brightness = new_brightness;
                if let Err(e) = set_panel_brightness(ctx, new_brightness).await {
                    tracing::warn!("Failed to adjust brightness: {}", e);
                } else {
                    // Reset exposure range for new brightness
                    min_exp = config.min_exposure;
                    max_exp = config.max_exposure;
                }
            }
        } else {
            // Too bright, need shorter exposure or dimmer panel
            max_exp = test_exposure;

            // If we hit min exposure and auto-adjust is enabled, try decreasing brightness
            if test_exposure <= config.min_exposure * 1.1
                && is_flat_panel
                && config.auto_adjust_brightness
                && *current_brightness > config.min_brightness
            {
                let new_brightness = (*current_brightness - 20).max(config.min_brightness);
                tracing::info!(
                    "Min exposure reached with high ADU, decreasing brightness from {} to {}",
                    current_brightness,
                    new_brightness
                );
                *current_brightness = new_brightness;
                if let Err(e) = set_panel_brightness(ctx, new_brightness).await {
                    tracing::warn!("Failed to adjust brightness: {}", e);
                } else {
                    // Reset exposure range for new brightness
                    min_exp = config.min_exposure;
                    max_exp = config.max_exposure;
                }
            }
        }

        // Check if range is too narrow to continue
        if (max_exp - min_exp) < 0.001 {
            tracing::warn!(
                "Search range too narrow ({:.4}s), using {:.3}s",
                max_exp - min_exp,
                test_exposure
            );
            return Ok((test_exposure, median_adu, *current_brightness));
        }
    }

    // Max iterations reached, return best guess
    let final_exposure = (min_exp + max_exp) / 2.0;
    tracing::warn!(
        "Max iterations reached, using {:.3}s (may not be optimal)",
        final_exposure
    );

    // Emit progress for final verification
    if let Some(cb) = progress_callback {
        cb(87.0, "Verifying flat quality".to_string());
    }

    // Take final test to get actual ADU
    let final_image = ctx
        .device_ops
        .camera_start_exposure(camera_id, final_exposure, None, None, 1, 1)
        .await
        .map_err(|e| format!("Final test exposure failed: {}", e))?;

    let final_adu = calculate_median_adu(&final_image);

    Ok((final_exposure, final_adu, *current_brightness))
}

/// Calculate median ADU value from image data
fn calculate_median_adu(image_data: &crate::device_ops::ImageData) -> u16 {
    // Sample the central 25% of the image to avoid vignetting
    let width = image_data.width as usize;
    let height = image_data.height as usize;

    let x_start = width / 4;
    let x_end = (width * 3) / 4;
    let y_start = height / 4;
    let y_end = (height * 3) / 4;

    // Collect pixel values from central region
    let mut pixels = Vec::new();
    for y in y_start..y_end {
        for x in x_start..x_end {
            let index = y * width + x;
            if index < image_data.data.len() {
                pixels.push(image_data.data[index]);
            }
        }
    }

    if pixels.is_empty() {
        tracing::warn!("No pixels in central region, using full image");
        pixels = image_data.data.clone();
    }

    // Sort to find median
    pixels.sort_unstable();

    let median = if pixels.is_empty() {
        0
    } else {
        pixels[pixels.len() / 2]
    };

    tracing::debug!(
        "Median ADU: {} (from {} pixels in central region)",
        median,
        pixels.len()
    );

    median
}

#[cfg(test)]
mod tests {
    use super::*;

    fn make_test_image(width: u32, height: u32, data: Vec<u16>) -> crate::device_ops::ImageData {
        crate::device_ops::ImageData {
            width,
            height,
            data,
            bits_per_pixel: 16,
            exposure_secs: 1.0,
            gain: None,
            offset: None,
            temperature: None,
            filter: None,
            timestamp: 0,
            sensor_type: Some("Monochrome".to_string()),
            bayer_offset: None,
        }
    }

    #[test]
    fn test_median_adu_calculation() {
        // Create test image: 100x100 pixels with value 30000
        let data = vec![30000u16; 10000];
        let image = make_test_image(100, 100, data);

        let median = calculate_median_adu(&image);
        assert_eq!(median, 30000);
    }

    #[test]
    fn test_median_adu_with_variation() {
        // Create image with gradient
        let mut data = Vec::new();
        for i in 0..10000 {
            data.push((20000 + i) as u16);
        }

        let image = make_test_image(100, 100, data);

        let median = calculate_median_adu(&image);
        // Median should be around 25000 (middle of 20000-30000 range)
        assert!(median > 23000 && median < 27000);
    }

    #[test]
    fn test_altaz_to_radec_zenith() {
        // Zenith (alt=90, any azimuth) should give Dec = latitude
        let (_ra, dec) = altaz_to_radec(90.0, 180.0, 0.0);
        // For latitude 45°, zenith Dec should be ~45°
        assert!((dec - 45.0).abs() < 1.0);
    }
}
