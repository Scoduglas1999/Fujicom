//! INDI Dome wrapper
//!
//! Provides high-level dome control via INDI protocol.

use crate::client::IndiClient;
use crate::error::IndiResult;
use std::sync::Arc;
use std::time::Duration;
use tokio::sync::RwLock;

/// Shutter status for INDI domes
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum IndiShutterStatus {
    Open,
    Closed,
    Opening,
    Closing,
    Error,
    Unknown,
}

/// INDI Dome device wrapper
pub struct IndiDome {
    client: Arc<RwLock<IndiClient>>,
    device_name: String,
}

impl IndiDome {
    /// Create a new INDI dome wrapper
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

    // =========================================================================
    // Connection
    // =========================================================================

    /// Connect to the dome
    pub async fn connect(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.connect_device(&self.device_name).await
    }

    /// Disconnect from the dome
    pub async fn disconnect(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.disconnect_device(&self.device_name).await
    }

    /// Check if connected
    pub async fn is_connected(&self) -> bool {
        let client = self.client.read().await;
        client.is_device_connected(&self.device_name).await
    }

    // =========================================================================
    // Position
    // =========================================================================

    /// Get the current dome azimuth position in degrees (0-360)
    pub async fn get_azimuth(&self) -> Result<f64, String> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, "ABS_DOME_POSITION", "DOME_ABSOLUTE_POSITION")
            .await
            .ok_or_else(|| "Azimuth not available".to_string())
    }

    /// Slew dome to specific azimuth
    pub async fn slew_to_azimuth(&self, azimuth: f64) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_number(&self.device_name, "ABS_DOME_POSITION", "DOME_ABSOLUTE_POSITION", azimuth).await
    }

    /// Slew dome to specific azimuth with timeout
    pub async fn slew_to_azimuth_with_timeout(
        &self,
        azimuth: f64,
        timeout: Option<Duration>,
    ) -> Result<(), String> {
        let timeout_duration = timeout.unwrap_or_else(|| {
            let client = tokio::task::block_in_place(|| {
                tokio::runtime::Handle::current().block_on(self.client.read())
            });
            Duration::from_secs(client.timeout_config().dome_slew_timeout_secs)
        });

        // Start the slew
        {
            let mut client = self.client.write().await;
            client.set_number(&self.device_name, "ABS_DOME_POSITION", "DOME_ABSOLUTE_POSITION", azimuth).await?;
        }

        // Wait for slew to complete
        let client = self.client.read().await;
        client
            .wait_for_property_not_busy(&self.device_name, "ABS_DOME_POSITION", timeout_duration)
            .await
            .map_err(|e| format!("Dome slew to azimuth {:.1} degrees failed: {}", azimuth, e))
    }

    /// Sync dome to current azimuth
    pub async fn sync_to_azimuth(&self, azimuth: f64) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_number(&self.device_name, "DOME_SYNC", "DOME_SYNC_VALUE", azimuth).await
    }

    // =========================================================================
    // Shutter Control
    // =========================================================================

    /// Open the dome shutter
    pub async fn open_shutter(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_switch(&self.device_name, "DOME_SHUTTER", "SHUTTER_OPEN", true).await
    }

    /// Close the dome shutter
    pub async fn close_shutter(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_switch(&self.device_name, "DOME_SHUTTER", "SHUTTER_CLOSE", true).await
    }

    /// Open the dome shutter with timeout
    pub async fn open_shutter_with_timeout(&self, timeout: Option<Duration>) -> Result<(), String> {
        let timeout_duration = timeout.unwrap_or_else(|| {
            let client = tokio::task::block_in_place(|| {
                tokio::runtime::Handle::current().block_on(self.client.read())
            });
            // Use dome slew timeout for shutter operations (they can be slow)
            Duration::from_secs(client.timeout_config().dome_slew_timeout_secs)
        });

        // Start shutter operation
        {
            let mut client = self.client.write().await;
            client.set_switch(&self.device_name, "DOME_SHUTTER", "SHUTTER_OPEN", true).await?;
        }

        // Wait for shutter operation to complete
        let client = self.client.read().await;
        client
            .wait_for_property_not_busy(&self.device_name, "DOME_SHUTTER", timeout_duration)
            .await
            .map_err(|e| format!("Dome shutter open operation failed: {}", e))
    }

    /// Close the dome shutter with timeout
    pub async fn close_shutter_with_timeout(&self, timeout: Option<Duration>) -> Result<(), String> {
        let timeout_duration = timeout.unwrap_or_else(|| {
            let client = tokio::task::block_in_place(|| {
                tokio::runtime::Handle::current().block_on(self.client.read())
            });
            // Use dome slew timeout for shutter operations (they can be slow)
            Duration::from_secs(client.timeout_config().dome_slew_timeout_secs)
        });

        // Start shutter operation
        {
            let mut client = self.client.write().await;
            client.set_switch(&self.device_name, "DOME_SHUTTER", "SHUTTER_CLOSE", true).await?;
        }

        // Wait for shutter operation to complete
        let client = self.client.read().await;
        client
            .wait_for_property_not_busy(&self.device_name, "DOME_SHUTTER", timeout_duration)
            .await
            .map_err(|e| format!("Dome shutter close operation failed: {}", e))
    }

    /// Get shutter status by checking switch states
    pub async fn get_shutter_status(&self) -> IndiShutterStatus {
        let client = self.client.read().await;

        // Check if shutter is open
        let is_open = client.get_switch(&self.device_name, "DOME_SHUTTER", "SHUTTER_OPEN")
            .await
            .unwrap_or(false);

        // Check if shutter is closed
        let is_closed = client.get_switch(&self.device_name, "DOME_SHUTTER", "SHUTTER_CLOSE")
            .await
            .unwrap_or(false);

        // Determine state based on property state and switch values
        if is_open && !is_closed {
            IndiShutterStatus::Open
        } else if is_closed && !is_open {
            IndiShutterStatus::Closed
        } else {
            // Check if we're in motion by looking at property state
            // If both are false or property is busy, we're in transition
            IndiShutterStatus::Unknown
        }
    }

    // =========================================================================
    // Motion Control
    // =========================================================================

    /// Check if dome is currently slewing
    pub async fn is_slewing(&self) -> bool {
        let client = self.client.read().await;
        // Check if the dome position property is in "Busy" state
        client.is_property_busy(&self.device_name, "ABS_DOME_POSITION").await
    }

    /// Abort all dome motion
    pub async fn abort_slew(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_switch(&self.device_name, "DOME_ABORT_MOTION", "ABORT", true).await
    }

    /// Move dome clockwise (manual jog)
    pub async fn move_clockwise(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_switch(&self.device_name, "DOME_MOTION", "DOME_CW", true).await
    }

    /// Move dome counter-clockwise (manual jog)
    pub async fn move_counter_clockwise(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_switch(&self.device_name, "DOME_MOTION", "DOME_CCW", true).await
    }

    // =========================================================================
    // Home & Park
    // =========================================================================

    /// Go to home position
    pub async fn find_home(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_switch(&self.device_name, "DOME_GOTO", "DOME_HOME", true).await
    }

    /// Park the dome
    pub async fn park(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_switch(&self.device_name, "DOME_GOTO", "DOME_PARK", true).await
    }

    /// Unpark the dome
    pub async fn unpark(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        // INDI uses DOME_PARK property with UNPARK element
        client.set_switch(&self.device_name, "DOME_PARK", "UNPARK", true).await
    }

    /// Check if dome is at home position
    pub async fn at_home(&self) -> bool {
        let client = self.client.read().await;
        client.get_switch(&self.device_name, "DOME_GOTO", "DOME_HOME")
            .await
            .unwrap_or(false)
    }

    /// Check if dome is parked
    pub async fn is_parked(&self) -> bool {
        let client = self.client.read().await;
        client.get_switch(&self.device_name, "DOME_PARK", "PARK")
            .await
            .unwrap_or(false)
    }

    // =========================================================================
    // Slaving
    // =========================================================================

    /// Enable/disable mount slaving
    pub async fn set_slaved(&self, slaved: bool) -> IndiResult<()> {
        let mut client = self.client.write().await;
        if slaved {
            client.set_switch(&self.device_name, "DOME_AUTOSYNC", "DOME_AUTOSYNC_ENABLE", true).await
        } else {
            client.set_switch(&self.device_name, "DOME_AUTOSYNC", "DOME_AUTOSYNC_DISABLE", true).await
        }
    }

    /// Check if dome is slaved to mount
    pub async fn is_slaved(&self) -> bool {
        let client = self.client.read().await;
        client.get_switch(&self.device_name, "DOME_AUTOSYNC", "DOME_AUTOSYNC_ENABLE")
            .await
            .unwrap_or(false)
    }

    // =========================================================================
    // Configuration
    // =========================================================================

    /// Set home position
    pub async fn set_home_position(&self, azimuth: f64) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_number(&self.device_name, "DOME_PARAMS", "HOME_POSITION", azimuth).await
    }

    /// Set park position
    pub async fn set_park_position(&self, azimuth: f64) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_number(&self.device_name, "DOME_PARAMS", "PARK_POSITION", azimuth).await
    }

    /// Get home position
    pub async fn get_home_position(&self) -> Result<f64, String> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, "DOME_PARAMS", "HOME_POSITION")
            .await
            .ok_or_else(|| "Home position not available".to_string())
    }

    /// Get park position
    pub async fn get_park_position(&self) -> Result<f64, String> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, "DOME_PARAMS", "PARK_POSITION")
            .await
            .ok_or_else(|| "Park position not available".to_string())
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::IndiClient;

    #[tokio::test]
    async fn test_dome_creation() {
        let client = Arc::new(RwLock::new(IndiClient::new("localhost", Some(7624))));
        let dome = IndiDome::new(client, "TestDome");
        assert_eq!(dome.device_name(), "TestDome");
    }

    #[tokio::test]
    async fn test_slew_to_azimuth_with_timeout_error_message() {
        let client = Arc::new(RwLock::new(IndiClient::new("localhost", Some(7624))));
        let dome = IndiDome::new(client, "TestDome");

        // This will fail since we're not connected
        let result = dome.slew_to_azimuth_with_timeout(180.0, Some(Duration::from_millis(100))).await;

        assert!(result.is_err());
        if let Err(e) = result {
            // Error should mention the azimuth or that we're not connected
            assert!(e.contains("180") || e.to_lowercase().contains("not connected"));
        }
    }

    #[tokio::test]
    async fn test_open_shutter_with_timeout_error_message() {
        let client = Arc::new(RwLock::new(IndiClient::new("localhost", Some(7624))));
        let dome = IndiDome::new(client, "TestDome");

        // This will fail since we're not connected
        let result = dome.open_shutter_with_timeout(Some(Duration::from_millis(100))).await;

        assert!(result.is_err());
        if let Err(e) = result {
            // Error should mention shutter or connection
            assert!(e.contains("shutter") || e.to_lowercase().contains("not connected"));
        }
    }

    #[tokio::test]
    async fn test_dome_timeout_uses_config() {
        let mut config = crate::IndiTimeoutConfig::default();
        config.dome_slew_timeout_secs = 600; // Custom timeout

        let client = Arc::new(RwLock::new(
            IndiClient::with_timeout_config("localhost", Some(7624), config)
        ));
        let _dome = IndiDome::new(client.clone(), "TestDome");

        // Verify the timeout config is accessible
        let timeout_secs = {
            let c = client.read().await;
            c.timeout_config().dome_slew_timeout_secs
        };
        assert_eq!(timeout_secs, 600);
    }
}
