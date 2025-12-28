//! Temperature Compensation Instruction
//!
//! Monitors focuser temperature and adjusts focus position based on thermal coefficient
//! to compensate for focus drift during long imaging sessions.

use crate::instructions::{InstructionContext, InstructionResult};

/// Execute temperature compensation
///
/// This instruction monitors the focuser temperature and moves the focuser to compensate
/// for thermal expansion/contraction. Unlike autofocus, this uses the learned thermal
/// coefficient to make targeted moves without a full focus sweep.
pub async fn execute_temperature_compensation(
    config: &TemperatureCompensationConfig,
    ctx: &InstructionContext,
    progress_callback: Option<&(dyn Fn(f64, String) + Send + Sync)>,
) -> InstructionResult {
    let focuser_id = match ctx.focuser_id() {
        Ok(id) => id.to_string(),
        Err(e) => return e,
    };

    // Emit initial progress
    if let Some(cb) = progress_callback {
        cb(0.0, "Starting temperature compensation".to_string());
    }

    tracing::info!(
        "Temperature compensation: coefficient={} steps/°C, mode={:?}",
        config.thermal_coefficient,
        config.mode
    );

    // Emit progress for reading temperature
    if let Some(cb) = progress_callback {
        cb(10.0, "Reading focuser temperature...".to_string());
    }

    // Get current focuser temperature
    let current_temp = match ctx.device_ops.focuser_get_temperature(&focuser_id).await {
        Ok(Some(temp)) => temp,
        Ok(None) => {
            return InstructionResult::failure(
                "Focuser does not report temperature - temperature compensation requires a focuser with temperature sensor"
            );
        }
        Err(e) => {
            return InstructionResult::failure(format!("Failed to read focuser temperature: {}", e));
        }
    };

    tracing::info!("Current focuser temperature: {:.2}°C", current_temp);

    // Emit progress with temperature reading
    if let Some(cb) = progress_callback {
        cb(20.0, format!("Reading focuser temperature: {:.1}°C", current_temp));
    }

    // Check if we have trigger state with baseline temperature
    let trigger_state_lock = match &ctx.trigger_state {
        Some(lock) => lock,
        None => {
            return InstructionResult::failure(
                "Temperature compensation requires trigger state for baseline tracking"
            );
        }
    };

    let mut trigger_state = trigger_state_lock.write().await;

    // Get or set baseline temperature
    let baseline_temp = match trigger_state.baseline_temperature {
        Some(temp) => temp,
        None => {
            // First time - establish baseline
            tracing::info!("Establishing temperature baseline: {:.2}°C", current_temp);
            trigger_state.baseline_temperature = Some(current_temp);
            trigger_state.current_temperature = Some(current_temp);
            drop(trigger_state);

            return InstructionResult::success_with_message(format!(
                "Temperature baseline established at {:.2}°C",
                current_temp
            ));
        }
    };

    // Update current temperature in trigger state
    trigger_state.current_temperature = Some(current_temp);

    // Calculate temperature change
    let temp_delta = current_temp - baseline_temp;

    tracing::info!(
        "Temperature delta: {:.2}°C (baseline: {:.2}°C, current: {:.2}°C)",
        temp_delta,
        baseline_temp,
        current_temp
    );

    // Emit progress for calculating compensation
    if let Some(cb) = progress_callback {
        cb(40.0, format!("Calculating compensation: delta temp = {:.2}°C", temp_delta));
    }

    // Check if temperature change exceeds threshold
    if temp_delta.abs() < config.min_temp_change {
        tracing::debug!(
            "Temperature change ({:.2}°C) below threshold ({:.2}°C), no compensation needed",
            temp_delta.abs(),
            config.min_temp_change
        );
        drop(trigger_state);

        return InstructionResult::success_with_message(format!(
            "No compensation needed (delta: {:.2}°C)",
            temp_delta
        ));
    }

    // Calculate focus position change
    let position_delta = (temp_delta * config.thermal_coefficient).round() as i32;

    if position_delta.abs() < config.min_step_change {
        tracing::debug!(
            "Position change ({} steps) below threshold ({} steps), no compensation needed",
            position_delta.abs(),
            config.min_step_change
        );
        drop(trigger_state);

        return InstructionResult::success_with_message(format!(
            "No compensation needed (steps: {})",
            position_delta
        ));
    }

    tracing::info!(
        "Temperature compensation required: {} steps ({:.2}°C × {} steps/°C)",
        position_delta,
        temp_delta,
        config.thermal_coefficient
    );

    // Get current focuser position
    let current_position = match ctx.device_ops.focuser_get_position(&focuser_id).await {
        Ok(pos) => pos,
        Err(e) => {
            drop(trigger_state);
            return InstructionResult::failure(format!("Failed to get focuser position: {}", e));
        }
    };

    tracing::debug!("Current focuser position: {}", current_position);

    // Calculate new position based on mode
    let new_position = match config.mode {
        CompensationMode::Relative => {
            // Move relative to current position
            current_position + position_delta
        }
        CompensationMode::Absolute => {
            // Calculate absolute position from baseline
            // This requires storing baseline position in trigger state
            // For now, use relative mode behavior
            tracing::warn!("Absolute mode not yet implemented, using relative mode");
            current_position + position_delta
        }
    };

    tracing::info!("Moving focuser from {} to {} ({:+} steps)",
        current_position, new_position, position_delta);

    // Emit progress for moving focuser
    if let Some(cb) = progress_callback {
        cb(60.0, format!("Moving focuser by {:+} steps", position_delta));
    }

    // Drop the write lock before device operation
    drop(trigger_state);

    // Check for cancellation
    if let Some(result) = ctx.check_cancelled() {
        return result;
    }

    // Move focuser
    if let Err(e) = ctx.device_ops.focuser_move_to(&focuser_id, new_position).await {
        return InstructionResult::failure(format!("Failed to move focuser: {}", e));
    }

    // Wait for focuser to reach position
    let move_start = std::time::Instant::now();
    let timeout = std::time::Duration::from_secs(config.timeout_secs as u64);
    let mut poll_count: u32 = 0;

    // Emit progress for waiting
    if let Some(cb) = progress_callback {
        cb(70.0, "Waiting for focuser to reach position".to_string());
    }

    loop {
        // Check cancellation
        if ctx.cancellation_token.load(std::sync::atomic::Ordering::Relaxed) {
            tracing::warn!("Temperature compensation cancelled, halting focuser");
            let _ = ctx.device_ops.focuser_halt(&focuser_id).await;
            return InstructionResult::cancelled("Temperature compensation cancelled");
        }

        // Check if focuser stopped moving
        match ctx.device_ops.focuser_is_moving(&focuser_id).await {
            Ok(false) => {
                // Add small settling delay
                tokio::time::sleep(std::time::Duration::from_millis(100)).await;
                break;
            }
            Ok(true) => {
                // Still moving, update progress periodically
                poll_count += 1;
                if poll_count % 5 == 0 {
                    let elapsed_secs = move_start.elapsed().as_secs();
                    // Progress from 70-90% during movement based on time (assume ~30s typical move)
                    let move_progress = 70.0 + ((elapsed_secs as f64 / 30.0) * 20.0).min(20.0);
                    if let Some(cb) = progress_callback {
                        cb(move_progress, format!("Focuser moving... ({:.0}s)", elapsed_secs));
                    }
                }
            }
            Err(e) => {
                tracing::warn!("Error checking focuser status: {}", e);
                // Continue waiting - transient error
            }
        }

        // Check timeout
        if move_start.elapsed() > timeout {
            let _ = ctx.device_ops.focuser_halt(&focuser_id).await;
            return InstructionResult::failure(format!(
                "Focuser move timed out after {} seconds",
                config.timeout_secs
            ));
        }

        // Poll every 100ms
        tokio::time::sleep(std::time::Duration::from_millis(100)).await;
    }

    // Verify final position
    let final_position = match ctx.device_ops.focuser_get_position(&focuser_id).await {
        Ok(pos) => pos,
        Err(e) => {
            tracing::warn!("Failed to verify final position: {}", e);
            new_position // Use target position as fallback
        }
    };

    let position_error = (final_position - new_position).abs();
    if position_error > 10 {
        tracing::warn!(
            "Focuser position error: {} steps (target: {}, actual: {})",
            position_error,
            new_position,
            final_position
        );
    }

    // Optionally reset baseline after compensation
    if config.reset_baseline_after_move {
        let mut trigger_state = trigger_state_lock.write().await;
        trigger_state.reset_baseline_temperature();
        tracing::info!("Temperature baseline reset to {:.2}°C", current_temp);
    }

    // Emit final progress
    if let Some(cb) = progress_callback {
        cb(100.0, format!("Temperature compensation complete: {:+} steps", position_delta));
    }

    InstructionResult::success_with_message(format!(
        "Temperature compensation complete: moved {:+} steps ({:.2}°C change)",
        position_delta,
        temp_delta
    ))
}

/// Configuration for temperature compensation
#[derive(Debug, Clone, serde::Serialize, serde::Deserialize)]
pub struct TemperatureCompensationConfig {
    /// Thermal coefficient in steps per degree Celsius
    /// Positive: focus moves outward when temperature increases
    /// Negative: focus moves inward when temperature increases
    /// Typical values: -5 to -50 steps/°C for most systems
    pub thermal_coefficient: f64,

    /// Minimum temperature change (°C) to trigger compensation
    /// Below this threshold, no move is made
    #[serde(default = "default_min_temp_change")]
    pub min_temp_change: f64,

    /// Minimum step change to trigger compensation
    /// Below this threshold, no move is made (even if temp changed)
    #[serde(default = "default_min_step_change")]
    pub min_step_change: i32,

    /// Compensation mode
    #[serde(default)]
    pub mode: CompensationMode,

    /// Reset temperature baseline after successful move
    /// If true, next compensation will be relative to current temp
    /// If false, compensation accumulates from original baseline
    #[serde(default = "default_reset_baseline")]
    pub reset_baseline_after_move: bool,

    /// Timeout for focuser move (seconds)
    #[serde(default = "default_timeout_secs")]
    pub timeout_secs: u32,
}

impl Default for TemperatureCompensationConfig {
    fn default() -> Self {
        Self {
            thermal_coefficient: -20.0, // Typical value for many systems
            min_temp_change: 0.5,        // 0.5°C minimum change
            min_step_change: 5,          // 5 steps minimum move
            mode: CompensationMode::Relative,
            reset_baseline_after_move: true,
            timeout_secs: 120,
        }
    }
}

fn default_min_temp_change() -> f64 {
    0.5
}

fn default_min_step_change() -> i32 {
    5
}

fn default_reset_baseline() -> bool {
    true
}

fn default_timeout_secs() -> u32 {
    120
}

/// Temperature compensation mode
#[derive(Debug, Clone, Copy, serde::Serialize, serde::Deserialize, PartialEq, Eq)]
pub enum CompensationMode {
    /// Move relative to current position based on temp delta
    Relative,
    /// Calculate absolute position from baseline (requires baseline position tracking)
    Absolute,
}

impl Default for CompensationMode {
    fn default() -> Self {
        Self::Relative
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_compensation_config_defaults() {
        let config = TemperatureCompensationConfig::default();
        assert_eq!(config.thermal_coefficient, -20.0);
        assert_eq!(config.min_temp_change, 0.5);
        assert_eq!(config.min_step_change, 5);
        assert_eq!(config.mode, CompensationMode::Relative);
        assert!(config.reset_baseline_after_move);
    }

    #[test]
    fn test_compensation_calculation() {
        let config = TemperatureCompensationConfig {
            thermal_coefficient: -20.0,
            min_temp_change: 0.5,
            min_step_change: 5,
            mode: CompensationMode::Relative,
            reset_baseline_after_move: true,
            timeout_secs: 120,
        };

        // 2°C temperature drop should cause +40 steps movement
        let temp_delta = -2.0;
        let expected_steps = (temp_delta * config.thermal_coefficient).round() as i32;
        assert_eq!(expected_steps, 40);

        // Small temperature change below threshold
        let small_delta = 0.3;
        assert!(small_delta.abs() < config.min_temp_change);
    }

    #[test]
    fn test_serde() {
        let config = TemperatureCompensationConfig::default();
        let json = serde_json::to_string(&config).unwrap();
        let deserialized: TemperatureCompensationConfig = serde_json::from_str(&json).unwrap();
        assert_eq!(config.thermal_coefficient, deserialized.thermal_coefficient);
    }
}
