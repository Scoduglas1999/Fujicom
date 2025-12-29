//! Autofocus instruction implementation
//!
//! Production-ready autofocus implementation using the autofocus module

use crate::*;
use crate::device_ops::{SharedDeviceOps, ImageData};
use crate::autofocus::{VCurveAutofocus, BacklashCompensation, FocusDataPoint};
use crate::instructions::{InstructionResult, InstructionContext, wait_for_focuser_idle, wait_for_focuser_stop_after_halt};
use std::time::Duration;
use tokio::time::sleep;

/// Execute autofocus using V-curve or curve fitting with backlash compensation
pub async fn execute_autofocus_complete(config: &AutofocusConfig, ctx: &InstructionContext) -> InstructionResult {
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

    // Get current focuser position
    let current_position = match ctx.device_ops.focuser_get_position(&focuser_id).await {
        Ok(pos) => pos,
        Err(e) => return InstructionResult::failure(format!("Failed to get focuser position: {}", e)),
    };

    tracing::info!("Current focuser position: {}", current_position);

    // Get current temperature for prediction and recording
    let current_temperature = ctx.device_ops.focuser_get_temperature(&focuser_id).await.ok();

    // Initialize autofocus engine
    let af_config = crate::autofocus::AutofocusConfig {
        method: config.method,
        step_size: config.step_size,
        steps_out: config.steps_out,
        exposure_duration: config.exposure_duration,
        backlash_compensation: 50,  // TODO: Make configurable via settings
        use_temperature_prediction: true,
        max_star_count_change: Some(0.5),  // Reject data if star count changes >50%
        outlier_rejection_sigma: 3.0,
    };

    let af_engine = VCurveAutofocus::new(af_config.clone());
    let backlash = BacklashCompensation::new(af_config.backlash_compensation);

    // Calculate sweep positions
    let positions = af_engine.calculate_positions(current_position);
    let total_points = positions.len();

    tracing::info!("Autofocus sweep: {} positions from {} to {}",
        total_points, positions[0], positions[total_points - 1]);

    // Move to starting position with backlash compensation
    let start_position = positions[0];
    if backlash.is_needed(current_position, start_position) {
        let (intermediate, final_pos) = backlash.calculate_approach(current_position, start_position);

        if let Some(overshoot) = intermediate {
            tracing::info!("Applying backlash compensation: {} -> {} -> {}",
                current_position, overshoot, final_pos);

            if let Err(e) = ctx.device_ops.focuser_move_to(&focuser_id, overshoot).await {
                return InstructionResult::failure(format!("Failed to move focuser (backlash): {}", e));
            }
            if let Err(e) = wait_for_focuser_idle(&focuser_id, ctx, Duration::from_secs(120)).await {
                return InstructionResult::failure(e);
            }
        }

        if let Err(e) = ctx.device_ops.focuser_move_to(&focuser_id, final_pos).await {
            return InstructionResult::failure(format!("Failed to move focuser: {}", e));
        }
    } else {
        tracing::info!("Moving to start position: {}", start_position);
        if let Err(e) = ctx.device_ops.focuser_move_to(&focuser_id, start_position).await {
            return InstructionResult::failure(format!("Failed to move focuser: {}", e));
        }
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

    let mut focus_data_points: Vec<FocusDataPoint> = Vec::with_capacity(total_points);
    let mut reference_star_count: Option<u32> = None;

    for (point_idx, &position) in positions.iter().enumerate() {
        if let Some(result) = ctx.check_cancelled() {
            // Halt focuser and wait for it to stop before returning
            tracing::info!("Autofocus cancelled, halting focuser");
            let _ = ctx.device_ops.focuser_halt(&focuser_id).await;
            wait_for_focuser_stop_after_halt(&focuser_id, &ctx.device_ops, Duration::from_secs(10)).await;
            // Optionally return to original position (start the move but don't wait - user cancelled)
            let _ = ctx.device_ops.focuser_move_to(&focuser_id, current_position).await;
            return result;
        }

        tracing::info!("Focus point {}/{} at position {}", point_idx + 1, total_points, position);

        // Move to position (no backlash needed for sequential inward moves)
        if let Err(e) = ctx.device_ops.focuser_move_to(&focuser_id, position).await {
            return InstructionResult::failure(format!("Failed to move focuser: {}", e));
        }

        // Wait for focuser to settle
        if let Err(e) = wait_for_focuser_idle(&focuser_id, ctx, Duration::from_secs(120)).await {
            return InstructionResult::failure(e);
        }

        // Small settling delay for vibration damping
        sleep(Duration::from_millis(500)).await;

        // Take exposure
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

        // Calculate HFR and star count from image
        let (hfr, star_count, fwhm) = calculate_hfr_and_stars(&image_data);

        // Check for dramatic star count changes (clouds, tracking issues, etc.)
        if let Some(ref_count) = reference_star_count {
            let count_change = ((star_count as f64 - ref_count as f64) / ref_count as f64).abs();
            if count_change > af_config.max_star_count_change.unwrap_or(0.5) {
                tracing::warn!(
                    "Star count changed by {:.1}% ({} -> {}), possible clouds or tracking issue",
                    count_change * 100.0, ref_count, star_count
                );
            }
        } else {
            reference_star_count = Some(star_count);
        }

        tracing::info!("Position {}: HFR = {:.2}, Stars = {}, FWHM = {:.2}",
            position, hfr, star_count, fwhm.unwrap_or(0.0));

        focus_data_points.push(FocusDataPoint {
            position,
            hfr,
            fwhm,
            star_count,
        });
    }

    // Find best focus using curve fitting
    let af_result = match af_engine.find_best_focus(focus_data_points) {
        Ok(mut result) => {
            result.temperature_celsius = current_temperature;
            result
        },
        Err(e) => {
            return InstructionResult::failure(format!("Autofocus curve fitting failed: {}", e));
        }
    };

    let best_position = af_result.best_position;
    let best_hfr = af_result.best_hfr;
    let curve_quality = af_result.curve_fit_quality;

    tracing::info!(
        "Autofocus complete: position = {}, HFR = {:.2}, R² = {:.3}",
        best_position, best_hfr, curve_quality
    );

    // Move to best position with backlash compensation
    let last_position = positions[positions.len() - 1];
    if backlash.is_needed(last_position, best_position) {
        let (intermediate, final_pos) = backlash.calculate_approach(last_position, best_position);

        if let Some(overshoot) = intermediate {
            tracing::info!("Final move with backlash: overshoot to {}, then {}", overshoot, final_pos);

            if let Err(e) = ctx.device_ops.focuser_move_to(&focuser_id, overshoot).await {
                return InstructionResult::failure(format!("Failed to move focuser (final backlash): {}", e));
            }
            if let Err(e) = wait_for_focuser_idle(&focuser_id, ctx, Duration::from_secs(120)).await {
                return InstructionResult::failure(e);
            }
        }

        if let Err(e) = ctx.device_ops.focuser_move_to(&focuser_id, final_pos).await {
            return InstructionResult::failure(format!("Failed to move to best focus: {}", e));
        }
    } else {
        if let Err(e) = ctx.device_ops.focuser_move_to(&focuser_id, best_position).await {
            return InstructionResult::failure(format!("Failed to move to best focus: {}", e));
        }
    }

    // Wait for focuser to settle at best position
    if let Err(e) = wait_for_focuser_idle(&focuser_id, ctx, Duration::from_secs(120)).await {
        return InstructionResult::failure(format!("Failed to settle at best focus: {}", e));
    }

    InstructionResult {
        status: NodeStatus::Success,
        message: Some(format!(
            "Autofocus complete: position {}, HFR {:.2}, R² {:.3}",
            best_position, best_hfr, curve_quality
        )),
        data: Some(serde_json::to_value(&af_result).unwrap_or_default()),
    }
}

/// Calculate HFR, star count, and FWHM from image data
fn calculate_hfr_and_stars(image: &ImageData) -> (f64, u32, Option<f64>) {
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
        // No stars detected - return high HFR value to indicate bad focus
        20.0
    };

    let fwhm = if result.median_fwhm > 0.0 {
        Some(result.median_fwhm)
    } else {
        None
    };

    (hfr, result.star_count, fwhm)
}
