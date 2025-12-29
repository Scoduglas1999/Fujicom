//! INDI Mount wrapper
//!
//! Provides high-level telescope mount control via INDI protocol.

use crate::client::IndiClient;
use crate::error::IndiResult;
use crate::protocol::standard_properties::*;
use crate::protocol::coord_elements::*;
use std::sync::Arc;
use std::time::Duration;
use tokio::sync::RwLock;

/// INDI Mount device wrapper
pub struct IndiMount {
    client: Arc<RwLock<IndiClient>>,
    device_name: String,
}

impl IndiMount {
    /// Create a new INDI mount wrapper
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

    /// Connect to the mount
    pub async fn connect(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.connect_device(&self.device_name).await
    }

    /// Disconnect from the mount
    pub async fn disconnect(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.disconnect_device(&self.device_name).await
    }

    /// Check if connected
    pub async fn is_connected(&self) -> bool {
        let client = self.client.read().await;
        client.is_device_connected(&self.device_name).await
    }

    /// Get current coordinates (RA in hours, Dec in degrees)
    pub async fn get_coordinates(&self) -> Result<(f64, f64), String> {
        let client = self.client.read().await;
        
        // Try J2000 coordinates first
        let ra = client.get_number(&self.device_name, EQUATORIAL_COORD, RA)
            .await
            .or_else(|| {
                // Fall back to EOD coordinates
                tokio::task::block_in_place(|| {
                    tokio::runtime::Handle::current().block_on(
                        client.get_number(&self.device_name, EQUATORIAL_EOD_COORD, RA)
                    )
                })
            })
            .ok_or_else(|| "RA not available".to_string())?;
        
        let dec = client.get_number(&self.device_name, EQUATORIAL_COORD, DEC)
            .await
            .or_else(|| {
                tokio::task::block_in_place(|| {
                    tokio::runtime::Handle::current().block_on(
                        client.get_number(&self.device_name, EQUATORIAL_EOD_COORD, DEC)
                    )
                })
            })
            .ok_or_else(|| "Dec not available".to_string())?;
        
        Ok((ra, dec))
    }

    /// Slew to coordinates (RA in hours, Dec in degrees)
    pub async fn slew_to_coordinates(&self, ra_hours: f64, dec_degrees: f64) -> IndiResult<()> {
        let mut client = self.client.write().await;

        // Set coordinate mode to SLEW
        client.set_switch(&self.device_name, ON_COORD_SET, "SLEW", true).await?;

        // Set target coordinates
        client.set_numbers(&self.device_name, EQUATORIAL_EOD_COORD, &[
            (RA, ra_hours),
            (DEC, dec_degrees),
        ]).await
    }

    /// Slew to coordinates with timeout (RA in hours, Dec in degrees)
    pub async fn slew_to_coordinates_with_timeout(
        &self,
        ra_hours: f64,
        dec_degrees: f64,
        timeout: Option<Duration>,
    ) -> Result<(), String> {
        let timeout_duration = timeout.unwrap_or_else(|| {
            let client = tokio::task::block_in_place(|| {
                tokio::runtime::Handle::current().block_on(self.client.read())
            });
            Duration::from_secs(client.timeout_config().mount_slew_timeout_secs)
        });

        // Start the slew
        {
            let mut client = self.client.write().await;
            client.set_switch(&self.device_name, ON_COORD_SET, "SLEW", true).await?;
            client.set_numbers(&self.device_name, EQUATORIAL_EOD_COORD, &[
                (RA, ra_hours),
                (DEC, dec_degrees),
            ]).await?;
        }

        // Wait for slew to complete
        let client = self.client.read().await;
        client
            .wait_for_property_not_busy(&self.device_name, EQUATORIAL_EOD_COORD, timeout_duration)
            .await
            .map_err(|e| format!("Mount slew to RA={:.4}h, Dec={:.4}° failed: {}", ra_hours, dec_degrees, e))
    }

    /// Sync to coordinates (RA in hours, Dec in degrees)
    pub async fn sync_to_coordinates(&self, ra_hours: f64, dec_degrees: f64) -> IndiResult<()> {
        let mut client = self.client.write().await;

        // Set coordinate mode to SYNC
        client.set_switch(&self.device_name, ON_COORD_SET, "SYNC", true).await?;

        // Set target coordinates
        client.set_numbers(&self.device_name, EQUATORIAL_EOD_COORD, &[
            (RA, ra_hours),
            (DEC, dec_degrees),
        ]).await
    }

    /// Abort slew
    pub async fn abort_slew(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_switch(&self.device_name, TELESCOPE_ABORT_MOTION, "ABORT", true).await
    }

    /// Park the mount
    pub async fn park(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_switch(&self.device_name, TELESCOPE_PARK, "PARK", true).await
    }

    /// Unpark the mount
    pub async fn unpark(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_switch(&self.device_name, TELESCOPE_PARK, "UNPARK", true).await
    }

    /// Check if parked
    pub async fn is_parked(&self) -> bool {
        let client = self.client.read().await;
        client.get_switch(&self.device_name, TELESCOPE_PARK, "PARK")
            .await
            .unwrap_or(false)
    }

    /// Set tracking state
    pub async fn set_tracking(&self, enabled: bool) -> IndiResult<()> {
        let mut client = self.client.write().await;
        if enabled {
            client.set_switch(&self.device_name, TELESCOPE_TRACK_STATE, "TRACK_ON", true).await
        } else {
            client.set_switch(&self.device_name, TELESCOPE_TRACK_STATE, "TRACK_OFF", true).await
        }
    }

    /// Check if tracking
    pub async fn is_tracking(&self) -> bool {
        let client = self.client.read().await;
        client.get_switch(&self.device_name, TELESCOPE_TRACK_STATE, "TRACK_ON")
            .await
            .unwrap_or(false)
    }

    /// Check if slewing
    pub async fn is_slewing(&self) -> bool {
        let client = self.client.read().await;
        // Mount is slewing if the EQUATORIAL_EOD_COORD property is in Busy state
        client.is_property_busy(&self.device_name, EQUATORIAL_EOD_COORD).await
    }

    /// Move north
    pub async fn move_north(&self, start: bool) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_switch(&self.device_name, TELESCOPE_MOTION_NS, "MOTION_NORTH", start).await
    }

    /// Move south
    pub async fn move_south(&self, start: bool) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_switch(&self.device_name, TELESCOPE_MOTION_NS, "MOTION_SOUTH", start).await
    }

    /// Move west
    pub async fn move_west(&self, start: bool) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_switch(&self.device_name, TELESCOPE_MOTION_WE, "MOTION_WEST", start).await
    }

    /// Move east
    pub async fn move_east(&self, start: bool) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_switch(&self.device_name, TELESCOPE_MOTION_WE, "MOTION_EAST", start).await
    }

    /// Set slew rate (0-4 typically, where 0 is slowest)
    pub async fn set_slew_rate(&self, rate: i32) -> IndiResult<()> {
        let mut client = self.client.write().await;
        // Different mounts use different switch names, try common patterns
        let rate_names = ["1x", "2x", "4x", "8x", "16x", "32x", "64x", "MAX"];
        let rate_idx = (rate as usize).min(rate_names.len() - 1);

        // Try numbered rate first
        if client.set_switch(&self.device_name, TELESCOPE_SLEW_RATE, rate_names[rate_idx], true).await.is_ok() {
            return Ok(());
        }

        // Try SLEWMODE pattern
        let mode = format!("SLEW{}", rate);
        client.set_switch(&self.device_name, "SLEWMODE", &mode, true).await
    }

    /// Get horizontal coordinates (Altitude, Azimuth)
    pub async fn get_horizontal_coordinates(&self) -> Result<(f64, f64), String> {
        let client = self.client.read().await;
        let alt = client.get_number(&self.device_name, HORIZONTAL_COORD, ALT)
            .await
            .ok_or_else(|| "Altitude not available".to_string())?;
        let az = client.get_number(&self.device_name, HORIZONTAL_COORD, AZ)
            .await
            .ok_or_else(|| "Azimuth not available".to_string())?;
        Ok((alt, az))
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::IndiClient;

    #[tokio::test]
    async fn test_mount_creation() {
        let client = Arc::new(RwLock::new(IndiClient::new("localhost", Some(7624))));
        let mount = IndiMount::new(client, "TestMount");
        assert_eq!(mount.device_name(), "TestMount");
    }

    #[tokio::test]
    async fn test_slew_with_timeout_error_message() {
        let client = Arc::new(RwLock::new(IndiClient::new("localhost", Some(7624))));
        let mount = IndiMount::new(client, "TestMount");

        // This will fail since we're not connected
        let result = mount.slew_to_coordinates_with_timeout(10.5, 45.0, Some(Duration::from_millis(100))).await;

        assert!(result.is_err());
        if let Err(e) = result {
            // Error should mention either the coordinates or that we're not connected
            assert!(e.contains("RA=10.5") || e.to_lowercase().contains("not connected"));
        }
    }

    #[tokio::test]
    async fn test_mount_timeout_uses_config() {
        let mut config = crate::IndiTimeoutConfig::default();
        config.mount_slew_timeout_secs = 600; // Custom timeout

        let client = Arc::new(RwLock::new(
            IndiClient::with_timeout_config("localhost", Some(7624), config)
        ));
        let _mount = IndiMount::new(client.clone(), "TestMount");

        // Verify the timeout config is accessible
        let timeout_secs = {
            let c = client.read().await;
            c.timeout_config().mount_slew_timeout_secs
        };
        assert_eq!(timeout_secs, 600);
    }
}


