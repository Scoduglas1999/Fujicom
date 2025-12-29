//! INDI Safety Monitor wrapper
//!
//! Provides safety monitoring via INDI protocol.
//!
//! INDI doesn't have a dedicated "SafetyMonitor" device type like ASCOM/Alpaca.
//! Instead, safety monitoring is typically implemented via:
//! - Weather devices (Weather interface with safety states)
//! - Watchdog devices (custom safety implementations)
//! - AUX devices with safety switches
//!
//! This wrapper provides a unified interface that can work with any of these.

use crate::client::IndiClient;
use crate::error::IndiResult;
use std::sync::Arc;
use tokio::sync::RwLock;

/// INDI Safety Monitor device wrapper
pub struct IndiSafetyMonitor {
    client: Arc<RwLock<IndiClient>>,
    device_name: String,
}

impl IndiSafetyMonitor {
    /// Create a new INDI safety monitor wrapper
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

    /// Connect to the safety monitor
    pub async fn connect(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.connect_device(&self.device_name).await
    }

    /// Disconnect from the safety monitor
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
    // Safety Status
    // =========================================================================

    /// Check if conditions are safe for observing
    ///
    /// This checks multiple possible safety indicators:
    /// 1. WEATHER_STATUS property (for Weather devices)
    /// 2. SAFETY_STATUS property (for dedicated safety monitors)
    /// 3. Individual weather parameters (cloud, rain, wind)
    pub async fn is_safe(&self) -> Result<bool, String> {
        let client = self.client.read().await;

        // First, try the standard WEATHER_STATUS property
        // This is the most common way INDI weather devices report safety
        if let Some(state) = client.get_light_state(&self.device_name, "WEATHER_STATUS", "WEATHER_SAFE").await {
            // INDI light states: Idle (0), Ok (1), Busy (2), Alert (3)
            // "Ok" or "Idle" means safe
            return Ok(state == 0 || state == 1);
        }

        // Try a generic SAFETY_STATUS property
        if let Some(is_safe) = client.get_switch(&self.device_name, "SAFETY_STATUS", "SAFE").await {
            return Ok(is_safe);
        }

        // Try AUX_SAFETY property (common for custom safety devices)
        if let Some(is_safe) = client.get_switch(&self.device_name, "AUX_SAFETY", "ENABLED").await {
            return Ok(is_safe);
        }

        // Check individual weather alerts if available
        // If any critical parameter is in alert state, consider unsafe
        let has_rain_alert = client.get_light_state(&self.device_name, "WEATHER_STATUS", "WEATHER_RAIN")
            .await
            .map(|s| s == 3) // Alert state
            .unwrap_or(false);

        let has_wind_alert = client.get_light_state(&self.device_name, "WEATHER_STATUS", "WEATHER_WIND")
            .await
            .map(|s| s == 3)
            .unwrap_or(false);

        let has_cloud_alert = client.get_light_state(&self.device_name, "WEATHER_STATUS", "WEATHER_CLOUDS")
            .await
            .map(|s| s == 3)
            .unwrap_or(false);

        if has_rain_alert || has_wind_alert || has_cloud_alert {
            return Ok(false);
        }

        // If we have no safety indicators, assume safe (fail-open)
        // The caller should check is_monitoring_available() first
        Ok(true)
    }

    /// Check if any safety monitoring is available
    pub async fn is_monitoring_available(&self) -> bool {
        let client = self.client.read().await;

        // Check for various safety-related properties
        client.has_property(&self.device_name, "WEATHER_STATUS").await
            || client.has_property(&self.device_name, "SAFETY_STATUS").await
            || client.has_property(&self.device_name, "AUX_SAFETY").await
    }

    // =========================================================================
    // Weather Parameters (if available)
    // =========================================================================

    /// Get temperature in Celsius (if available)
    pub async fn get_temperature(&self) -> Option<f64> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, "WEATHER_PARAMETERS", "WEATHER_TEMPERATURE").await
    }

    /// Get humidity percentage (if available)
    pub async fn get_humidity(&self) -> Option<f64> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, "WEATHER_PARAMETERS", "WEATHER_HUMIDITY").await
    }

    /// Get wind speed in m/s (if available)
    pub async fn get_wind_speed(&self) -> Option<f64> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, "WEATHER_PARAMETERS", "WEATHER_WIND_SPEED").await
    }

    /// Get cloud cover percentage (if available)
    pub async fn get_cloud_cover(&self) -> Option<f64> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, "WEATHER_PARAMETERS", "WEATHER_CLOUD_COVER").await
    }

    /// Get rain rate (if available)
    pub async fn get_rain_rate(&self) -> Option<f64> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, "WEATHER_PARAMETERS", "WEATHER_RAIN_RATE").await
    }

    /// Get dew point in Celsius (if available)
    pub async fn get_dew_point(&self) -> Option<f64> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, "WEATHER_PARAMETERS", "WEATHER_DEWPOINT").await
    }

    /// Get sky quality in mag/arcsec^2 (if available)
    pub async fn get_sky_quality(&self) -> Option<f64> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, "WEATHER_PARAMETERS", "WEATHER_SKY_QUALITY").await
    }

    /// Get sky temperature in Celsius (if available)
    pub async fn get_sky_temperature(&self) -> Option<f64> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, "WEATHER_PARAMETERS", "WEATHER_SKY_TEMPERATURE").await
    }

    /// Get barometric pressure in hPa (if available)
    pub async fn get_pressure(&self) -> Option<f64> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, "WEATHER_PARAMETERS", "WEATHER_PRESSURE").await
    }

    // =========================================================================
    // Alert States
    // =========================================================================

    /// Check if there's a rain alert
    pub async fn has_rain_alert(&self) -> bool {
        let client = self.client.read().await;
        client.get_light_state(&self.device_name, "WEATHER_STATUS", "WEATHER_RAIN")
            .await
            .map(|s| s == 3) // Alert state
            .unwrap_or(false)
    }

    /// Check if there's a wind alert
    pub async fn has_wind_alert(&self) -> bool {
        let client = self.client.read().await;
        client.get_light_state(&self.device_name, "WEATHER_STATUS", "WEATHER_WIND")
            .await
            .map(|s| s == 3)
            .unwrap_or(false)
    }

    /// Check if there's a cloud alert
    pub async fn has_cloud_alert(&self) -> bool {
        let client = self.client.read().await;
        client.get_light_state(&self.device_name, "WEATHER_STATUS", "WEATHER_CLOUDS")
            .await
            .map(|s| s == 3)
            .unwrap_or(false)
    }

    /// Check if there's a humidity alert
    pub async fn has_humidity_alert(&self) -> bool {
        let client = self.client.read().await;
        client.get_light_state(&self.device_name, "WEATHER_STATUS", "WEATHER_HUMIDITY")
            .await
            .map(|s| s == 3)
            .unwrap_or(false)
    }
}
