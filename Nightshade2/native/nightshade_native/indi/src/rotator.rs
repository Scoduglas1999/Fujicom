//! INDI Rotator wrapper
//!
//! Provides high-level rotator control via INDI protocol.

use crate::client::IndiClient;
use crate::error::IndiResult;
use std::sync::Arc;
use std::time::Duration;
use tokio::sync::RwLock;

/// INDI Rotator device wrapper
pub struct IndiRotator {
    client: Arc<RwLock<IndiClient>>,
    device_name: String,
}

impl IndiRotator {
    /// Create a new INDI rotator wrapper
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

    /// Connect to the rotator
    pub async fn connect(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.connect_device(&self.device_name).await
    }

    /// Disconnect from the rotator
    pub async fn disconnect(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.disconnect_device(&self.device_name).await
    }

    /// Check if connected
    pub async fn is_connected(&self) -> bool {
        let client = self.client.read().await;
        client.is_device_connected(&self.device_name).await
    }

    /// Move to angle
    pub async fn move_to(&self, angle: f64) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_number(&self.device_name, "ABS_ROTATOR_ANGLE", "ANGLE", angle).await
    }

    /// Move to angle with timeout
    pub async fn move_to_with_timeout(&self, angle: f64, timeout: Option<Duration>) -> Result<(), String> {
        let timeout_duration = timeout.unwrap_or_else(|| {
            let client = tokio::task::block_in_place(|| {
                tokio::runtime::Handle::current().block_on(self.client.read())
            });
            Duration::from_secs(client.timeout_config().rotator_move_timeout_secs)
        });

        // Start the move
        {
            let mut client = self.client.write().await;
            client.set_number(&self.device_name, "ABS_ROTATOR_ANGLE", "ANGLE", angle).await?;
        }

        // Wait for move to complete
        let client = self.client.read().await;
        client
            .wait_for_property_not_busy(&self.device_name, "ABS_ROTATOR_ANGLE", timeout_duration)
            .await
            .map_err(|e| format!("Rotator move to angle {:.1} degrees failed: {}", angle, e))
    }

    /// Get current angle
    pub async fn get_angle(&self) -> Result<f64, String> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, "ABS_ROTATOR_ANGLE", "ANGLE")
            .await
            .ok_or_else(|| "Angle not available".to_string())
    }

    /// Abort motion
    pub async fn abort_motion(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_switch(&self.device_name, "ROTATOR_ABORT_MOTION", "ABORT", true).await
    }

    /// Check if rotator is currently moving
    pub async fn is_moving(&self) -> bool {
        let client = self.client.read().await;
        client.is_property_busy(&self.device_name, "ABS_ROTATOR_ANGLE").await
    }

    /// Reverse direction
    pub async fn set_reverse(&self, reverse: bool) -> IndiResult<()> {
        let mut client = self.client.write().await;
        if reverse {
            client.set_switch(&self.device_name, "ROTATOR_REVERSE", "INDI_ENABLED", true).await
        } else {
            client.set_switch(&self.device_name, "ROTATOR_REVERSE", "INDI_DISABLED", true).await
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::IndiClient;

    #[tokio::test]
    async fn test_rotator_creation() {
        let client = Arc::new(RwLock::new(IndiClient::new("localhost", Some(7624))));
        let rotator = IndiRotator::new(client, "TestRotator");
        assert_eq!(rotator.device_name(), "TestRotator");
    }

    #[tokio::test]
    async fn test_move_to_with_timeout_error_message() {
        let client = Arc::new(RwLock::new(IndiClient::new("localhost", Some(7624))));
        let rotator = IndiRotator::new(client, "TestRotator");

        // This will fail since we're not connected
        let result = rotator.move_to_with_timeout(90.0, Some(Duration::from_millis(100))).await;

        assert!(result.is_err());
        if let Err(e) = result {
            // Error should mention the angle or that we're not connected
            assert!(e.contains("90") || e.to_lowercase().contains("not connected"));
        }
    }

    #[tokio::test]
    async fn test_rotator_timeout_uses_config() {
        let mut config = crate::IndiTimeoutConfig::default();
        config.rotator_move_timeout_secs = 240; // Custom timeout

        let client = Arc::new(RwLock::new(
            IndiClient::with_timeout_config("localhost", Some(7624), config)
        ));
        let _rotator = IndiRotator::new(client.clone(), "TestRotator");

        // Verify the timeout config is accessible
        let timeout_secs = {
            let c = client.read().await;
            c.timeout_config().rotator_move_timeout_secs
        };
        assert_eq!(timeout_secs, 240);
    }
}
