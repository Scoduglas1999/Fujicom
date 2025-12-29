//! INDI Focuser wrapper
//!
//! Provides high-level focuser control via INDI protocol.

use crate::client::IndiClient;
use crate::error::IndiResult;
use crate::protocol::standard_properties::*;
use std::sync::Arc;
use std::time::Duration;
use tokio::sync::RwLock;

/// INDI Focuser device wrapper
pub struct IndiFocuser {
    client: Arc<RwLock<IndiClient>>,
    device_name: String,
}

impl IndiFocuser {
    /// Create a new INDI focuser wrapper
    pub fn new(client: Arc<RwLock<IndiClient>>, device_name: &str) -> Self {
        Self {
            client,
            device_name: device_name.to_string(),
        }
    }

    /// Get the device name
    pub fn device_name(&self) -> &str {
        &self.device_name
    }

    /// Connect to the focuser
    pub async fn connect(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.connect_device(&self.device_name).await
    }

    /// Disconnect from the focuser
    pub async fn disconnect(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.disconnect_device(&self.device_name).await
    }

    /// Check if connected
    pub async fn is_connected(&self) -> bool {
        let client = self.client.read().await;
        client.is_device_connected(&self.device_name).await
    }

    /// Move to absolute position
    pub async fn move_to(&self, position: i32) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_number(&self.device_name, ABS_FOCUS_POSITION, "FOCUS_ABSOLUTE_POSITION", position as f64).await
    }

    /// Move to absolute position with timeout
    pub async fn move_to_with_timeout(&self, position: i32, timeout: Option<Duration>) -> Result<(), String> {
        // Read config outside the closure - async-friendly
        let timeout_duration = if let Some(t) = timeout {
            t
        } else {
            let client = self.client.read().await;
            Duration::from_secs(client.timeout_config().focuser_move_timeout_secs)
        };

        // Start the move
        {
            let mut client = self.client.write().await;
            client.set_number(&self.device_name, ABS_FOCUS_POSITION, "FOCUS_ABSOLUTE_POSITION", position as f64).await?;
        }

        // Wait for move to complete
        let client = self.client.read().await;
        client
            .wait_for_property_not_busy(&self.device_name, ABS_FOCUS_POSITION, timeout_duration)
            .await
            .map_err(|e| format!("Focuser move to position {} failed: {}", position, e))
    }

    /// Move relative (inward/outward)
    pub async fn move_relative(&self, steps: i32) -> IndiResult<()> {
        let mut client = self.client.write().await;

        // Determine direction
        let direction = if steps > 0 { "FOCUS_IN" } else { "FOCUS_OUT" };
        let abs_steps = steps.abs();

        // Set direction switch
        client.set_switch(&self.device_name, FOCUS_MOTION, direction, true).await?;

        // Set relative steps
        client.set_number(&self.device_name, REL_FOCUS_POSITION, "FOCUS_RELATIVE_POSITION", abs_steps as f64).await
    }

    /// Move relative with timeout (inward/outward)
    pub async fn move_relative_with_timeout(&self, steps: i32, timeout: Option<Duration>) -> Result<(), String> {
        // Read config outside the closure - async-friendly
        let timeout_duration = if let Some(t) = timeout {
            t
        } else {
            let client = self.client.read().await;
            Duration::from_secs(client.timeout_config().focuser_move_timeout_secs)
        };

        // Start the move
        {
            let mut client = self.client.write().await;
            let direction = if steps > 0 { "FOCUS_IN" } else { "FOCUS_OUT" };
            let abs_steps = steps.abs();
            client.set_switch(&self.device_name, FOCUS_MOTION, direction, true).await?;
            client.set_number(&self.device_name, REL_FOCUS_POSITION, "FOCUS_RELATIVE_POSITION", abs_steps as f64).await?;
        }

        // Wait for move to complete
        let client = self.client.read().await;
        client
            .wait_for_property_not_busy(&self.device_name, REL_FOCUS_POSITION, timeout_duration)
            .await
            .map_err(|e| format!("Focuser relative move by {} steps failed: {}", steps, e))
    }

    /// Abort motion
    pub async fn abort_motion(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_switch(&self.device_name, FOCUS_ABORT_MOTION, "ABORT", true).await
    }

    /// Get current position
    pub async fn get_position(&self) -> Result<i32, String> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, ABS_FOCUS_POSITION, "FOCUS_ABSOLUTE_POSITION")
            .await
            .map(|p| p as i32)
            .ok_or_else(|| "Position not available".to_string())
    }

    /// Check if moving
    pub async fn is_moving(&self) -> bool {
        let client = self.client.read().await;
        if let Some(state) = client.get_property_state(&self.device_name, ABS_FOCUS_POSITION).await {
            return state == crate::IndiPropertyState::Busy;
        }
        false 
    }

    /// Get temperature
    pub async fn get_temperature(&self) -> Result<f64, String> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, FOCUS_TEMPERATURE, "TEMPERATURE")
            .await
            .ok_or_else(|| "Temperature not available".to_string())
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::IndiClient;

    #[tokio::test]
    async fn test_focuser_creation() {
        let client = Arc::new(RwLock::new(IndiClient::new("localhost", Some(7624))));
        let focuser = IndiFocuser::new(client, "TestFocuser");
        assert_eq!(focuser.device_name(), "TestFocuser");
    }

    #[tokio::test]
    async fn test_move_to_with_timeout_error_message() {
        let client = Arc::new(RwLock::new(IndiClient::new("localhost", Some(7624))));
        let focuser = IndiFocuser::new(client, "TestFocuser");

        // This will fail since we're not connected
        let result = focuser.move_to_with_timeout(5000, Some(Duration::from_millis(100))).await;

        assert!(result.is_err());
        if let Err(e) = result {
            // Error should mention either the position or that we're not connected
            assert!(e.contains("position 5000") || e.to_lowercase().contains("not connected"));
        }
    }

    #[tokio::test]
    async fn test_move_relative_with_timeout_error_message() {
        let client = Arc::new(RwLock::new(IndiClient::new("localhost", Some(7624))));
        let focuser = IndiFocuser::new(client, "TestFocuser");

        // This will fail since we're not connected
        let result = focuser.move_relative_with_timeout(100, Some(Duration::from_millis(100))).await;

        assert!(result.is_err());
        if let Err(e) = result {
            // Error should mention either the steps or that we're not connected
            assert!(e.contains("100 steps") || e.to_lowercase().contains("not connected"));
        }
    }

    #[tokio::test]
    async fn test_focuser_timeout_uses_config() {
        let mut config = crate::IndiTimeoutConfig::default();
        config.focuser_move_timeout_secs = 240; // Custom timeout

        let client = Arc::new(RwLock::new(
            IndiClient::with_timeout_config("localhost", Some(7624), config)
        ));
        let _focuser = IndiFocuser::new(client.clone(), "TestFocuser");

        // Verify the timeout config is accessible
        let timeout_secs = {
            let c = client.read().await;
            c.timeout_config().focuser_move_timeout_secs
        };
        assert_eq!(timeout_secs, 240);
    }
}
