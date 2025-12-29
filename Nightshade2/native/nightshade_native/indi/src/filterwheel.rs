//! INDI Filter Wheel wrapper
//!
//! Provides high-level filter wheel control via INDI protocol.

use crate::client::IndiClient;
use crate::error::IndiResult;
use crate::protocol::standard_properties::*;
use std::sync::Arc;
use std::time::Duration;
use tokio::sync::RwLock;

/// INDI Filter Wheel device wrapper
pub struct IndiFilterWheel {
    client: Arc<RwLock<IndiClient>>,
    device_name: String,
}

impl IndiFilterWheel {
    /// Create a new INDI filter wheel wrapper
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

    /// Connect to the filter wheel
    pub async fn connect(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.connect_device(&self.device_name).await
    }

    /// Disconnect from the filter wheel
    pub async fn disconnect(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.disconnect_device(&self.device_name).await
    }

    /// Check if connected
    pub async fn is_connected(&self) -> bool {
        let client = self.client.read().await;
        client.is_device_connected(&self.device_name).await
    }

    /// Set filter slot (1-based)
    pub async fn set_slot(&self, slot: i32) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_number(&self.device_name, FILTER_SLOT, "FILTER_SLOT_VALUE", slot as f64).await
    }

    /// Set filter slot with timeout (1-based)
    pub async fn set_slot_with_timeout(&self, slot: i32, timeout: Option<Duration>) -> Result<(), String> {
        let timeout_duration = timeout.unwrap_or_else(|| {
            let client = tokio::task::block_in_place(|| {
                tokio::runtime::Handle::current().block_on(self.client.read())
            });
            Duration::from_secs(client.timeout_config().filter_change_timeout_secs)
        });

        // Start the filter change
        {
            let mut client = self.client.write().await;
            client.set_number(&self.device_name, FILTER_SLOT, "FILTER_SLOT_VALUE", slot as f64).await?;
        }

        // Wait for filter change to complete
        let client = self.client.read().await;
        client
            .wait_for_property_not_busy(&self.device_name, FILTER_SLOT, timeout_duration)
            .await
            .map_err(|e| format!("Filter wheel change to slot {} failed: {}", slot, e))
    }

    /// Get current filter slot (1-based)
    pub async fn get_slot(&self) -> Result<i32, String> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, FILTER_SLOT, "FILTER_SLOT_VALUE")
            .await
            .map(|s| s as i32)
            .ok_or_else(|| "Filter slot not available".to_string())
    }

    /// Get filter names
    /// Get filter names
    pub async fn get_names(&self) -> Result<Vec<String>, String> {
        let client = self.client.read().await;
        let props = client.get_properties(&self.device_name).await;
        
        if let Some(prop) = props.iter().find(|p| p.name == FILTER_NAME) {
            let mut names = Vec::new();
            for elem in &prop.elements {
                if let Some(val) = client.get_property_value(&self.device_name, FILTER_NAME, elem).await {
                    names.push(val);
                } else {
                    names.push(elem.clone());
                }
            }
            return Ok(names);
        }

        Ok(Vec::new())
    }

    /// Check if filter wheel is currently moving
    pub async fn is_moving(&self) -> bool {
        let client = self.client.read().await;
        client.is_property_busy(&self.device_name, FILTER_SLOT).await
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::IndiClient;

    #[tokio::test]
    async fn test_filterwheel_creation() {
        let client = Arc::new(RwLock::new(IndiClient::new("localhost", Some(7624))));
        let fw = IndiFilterWheel::new(client, "TestFilterWheel");
        assert_eq!(fw.device_name(), "TestFilterWheel");
    }

    #[tokio::test]
    async fn test_set_slot_with_timeout_error_message() {
        let client = Arc::new(RwLock::new(IndiClient::new("localhost", Some(7624))));
        let fw = IndiFilterWheel::new(client, "TestFilterWheel");

        // This will fail since we're not connected
        let result = fw.set_slot_with_timeout(3, Some(Duration::from_millis(100))).await;

        assert!(result.is_err());
        if let Err(e) = result {
            // Error should mention either the slot or that we're not connected
            assert!(e.contains("slot 3") || e.to_lowercase().contains("not connected"));
        }
    }

    #[tokio::test]
    async fn test_filterwheel_timeout_uses_config() {
        let mut config = crate::IndiTimeoutConfig::default();
        config.filter_change_timeout_secs = 120; // Custom timeout

        let client = Arc::new(RwLock::new(
            IndiClient::with_timeout_config("localhost", Some(7624), config)
        ));
        let _fw = IndiFilterWheel::new(client.clone(), "TestFilterWheel");

        // Verify the timeout config is accessible
        let timeout_secs = {
            let c = client.read().await;
            c.timeout_config().filter_change_timeout_secs
        };
        assert_eq!(timeout_secs, 120);
    }
}
