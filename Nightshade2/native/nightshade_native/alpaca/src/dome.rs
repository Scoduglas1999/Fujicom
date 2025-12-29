//! Alpaca Dome API implementation

use crate::{AlpacaClient, AlpacaClientBuilder, AlpacaDevice, AlpacaDeviceType, AlpacaError, TimeoutConfig, RetryConfig};
use std::time::Duration;

/// Shutter state enum
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ShutterStatus {
    Open = 0,
    Closed = 1,
    Opening = 2,
    Closing = 3,
    Error = 4,
}

impl From<i32> for ShutterStatus {
    fn from(value: i32) -> Self {
        match value {
            0 => ShutterStatus::Open,
            1 => ShutterStatus::Closed,
            2 => ShutterStatus::Opening,
            3 => ShutterStatus::Closing,
            _ => ShutterStatus::Error,
        }
    }
}

impl std::fmt::Display for ShutterStatus {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            ShutterStatus::Open => write!(f, "Open"),
            ShutterStatus::Closed => write!(f, "Closed"),
            ShutterStatus::Opening => write!(f, "Opening"),
            ShutterStatus::Closing => write!(f, "Closing"),
            ShutterStatus::Error => write!(f, "Error"),
        }
    }
}

/// Alpaca Dome client
pub struct AlpacaDome {
    client: AlpacaClient,
}

impl AlpacaDome {
    /// Create a new Alpaca dome client
    pub fn new(device: &AlpacaDevice) -> Self {
        assert_eq!(device.device_type, AlpacaDeviceType::Dome);
        Self {
            client: AlpacaClient::new(device),
        }
    }

    /// Create a dome client with custom configuration
    pub fn with_config(
        device: &AlpacaDevice,
        timeout_config: TimeoutConfig,
        retry_config: RetryConfig,
    ) -> Self {
        assert_eq!(device.device_type, AlpacaDeviceType::Dome);
        Self {
            client: AlpacaClient::with_config(device, timeout_config, retry_config),
        }
    }

    /// Create from server details
    pub fn from_server(base_url: &str, device_number: u32) -> Self {
        let device = AlpacaDevice {
            device_type: AlpacaDeviceType::Dome,
            device_number,
            server_name: String::new(),
            manufacturer: String::new(),
            device_name: String::new(),
            unique_id: String::new(),
            base_url: base_url.to_string(),
        };
        Self::new(&device)
    }

    /// Create a builder for custom configuration
    pub fn builder(device: AlpacaDevice) -> AlpacaClientBuilder {
        AlpacaClientBuilder::new(device)
    }

    /// Get access to the underlying client
    pub fn client(&self) -> &AlpacaClient {
        &self.client
    }

    /// Get the base URL for this device
    pub fn base_url(&self) -> &str {
        self.client.base_url()
    }

    /// Get the device number for this device
    pub fn device_number(&self) -> u32 {
        self.client.device_number()
    }

    // Connection methods

    pub async fn connect(&self) -> Result<(), String> {
        self.client.connect().await
    }

    pub async fn disconnect(&self) -> Result<(), String> {
        self.client.disconnect().await
    }

    pub async fn is_connected(&self) -> Result<bool, String> {
        self.client.is_connected().await
    }

    // Dome information

    pub async fn name(&self) -> Result<String, String> {
        self.client.get_name().await
    }

    pub async fn description(&self) -> Result<String, String> {
        self.client.get_description().await
    }

    // Position

    pub async fn altitude(&self) -> Result<f64, String> {
        self.client.get("altitude").await
    }

    pub async fn azimuth(&self) -> Result<f64, String> {
        self.client.get("azimuth").await
    }

    // Status

    pub async fn at_home(&self) -> Result<bool, String> {
        self.client.get("athome").await
    }

    pub async fn at_park(&self) -> Result<bool, String> {
        self.client.get("atpark").await
    }

    pub async fn shutter_status(&self) -> Result<ShutterStatus, String> {
        let status: i32 = self.client.get("shutterstatus").await?;
        Ok(ShutterStatus::from(status))
    }

    pub async fn slewing(&self) -> Result<bool, String> {
        self.client.get("slewing").await
    }

    pub async fn slaved(&self) -> Result<bool, String> {
        self.client.get("slaved").await
    }

    pub async fn set_slaved(&self, slaved: bool) -> Result<(), String> {
        self.client.put("slaved", &[("Slaved", &slaved.to_string())]).await
    }

    // Capabilities

    pub async fn can_find_home(&self) -> Result<bool, String> {
        self.client.get("canfindhome").await
    }

    pub async fn can_park(&self) -> Result<bool, String> {
        self.client.get("canpark").await
    }

    pub async fn can_set_altitude(&self) -> Result<bool, String> {
        self.client.get("cansetaltitude").await
    }

    pub async fn can_set_azimuth(&self) -> Result<bool, String> {
        self.client.get("cansetazimuth").await
    }

    pub async fn can_set_shutter(&self) -> Result<bool, String> {
        self.client.get("cansetshutter").await
    }

    pub async fn can_slave(&self) -> Result<bool, String> {
        self.client.get("canslave").await
    }

    pub async fn can_sync_azimuth(&self) -> Result<bool, String> {
        self.client.get("cansyncazimuth").await
    }

    // Movement commands

    pub async fn abort_slew(&self) -> Result<(), String> {
        self.client.put("abortslew", &[]).await
    }

    /// Close the dome shutter
    /// Uses long timeout as shutter operations can take several minutes
    pub async fn close_shutter(&self) -> Result<(), String> {
        self.close_shutter_typed().await.map_err(|e| e.to_string())
    }

    /// Close shutter with typed error handling and long timeout
    pub async fn close_shutter_typed(&self) -> Result<(), AlpacaError> {
        self.client.put_long("closeshutter", &[]).await
    }

    /// Find the dome home position
    /// Uses very long timeout as homing can take several minutes
    pub async fn find_home(&self) -> Result<(), String> {
        self.find_home_typed().await.map_err(|e| e.to_string())
    }

    /// Find home with typed error handling and very long timeout
    pub async fn find_home_typed(&self) -> Result<(), AlpacaError> {
        self.client.put_very_long("findhome", &[]).await
    }

    /// Open the dome shutter
    /// Uses long timeout as shutter operations can take several minutes
    pub async fn open_shutter(&self) -> Result<(), String> {
        self.open_shutter_typed().await.map_err(|e| e.to_string())
    }

    /// Open shutter with typed error handling and long timeout
    pub async fn open_shutter_typed(&self) -> Result<(), AlpacaError> {
        self.client.put_long("openshutter", &[]).await
    }

    /// Park the dome
    /// Uses long timeout as parking can take several minutes
    pub async fn park(&self) -> Result<(), String> {
        self.park_typed().await.map_err(|e| e.to_string())
    }

    /// Park with typed error handling and long timeout
    pub async fn park_typed(&self) -> Result<(), AlpacaError> {
        self.client.put_long("park", &[]).await
    }

    pub async fn set_park(&self) -> Result<(), String> {
        self.client.put("setpark", &[]).await
    }

    /// Slew dome to altitude
    /// Uses long timeout as dome rotation can take time
    pub async fn slew_to_altitude(&self, altitude: f64) -> Result<(), String> {
        self.slew_to_altitude_typed(altitude).await.map_err(|e| e.to_string())
    }

    /// Slew to altitude with typed error handling and long timeout
    pub async fn slew_to_altitude_typed(&self, altitude: f64) -> Result<(), AlpacaError> {
        self.client.put_long("slewtoaltitude", &[("Altitude", &altitude.to_string())]).await
    }

    /// Slew dome to azimuth
    /// Uses very long timeout as full rotation can take many minutes
    pub async fn slew_to_azimuth(&self, azimuth: f64) -> Result<(), String> {
        self.slew_to_azimuth_typed(azimuth).await.map_err(|e| e.to_string())
    }

    /// Slew to azimuth with typed error handling and very long timeout
    pub async fn slew_to_azimuth_typed(&self, azimuth: f64) -> Result<(), AlpacaError> {
        self.client.put_very_long("slewtoazimuth", &[("Azimuth", &azimuth.to_string())]).await
    }

    pub async fn sync_to_azimuth(&self, azimuth: f64) -> Result<(), String> {
        self.client.put("synctoazimuth", &[("Azimuth", &azimuth.to_string())]).await
    }

    /// Wait for dome to stop slewing with configurable timeout
    pub async fn wait_for_slew_complete(
        &self,
        poll_interval: Duration,
        timeout: Duration,
    ) -> Result<bool, AlpacaError> {
        let deadline = std::time::Instant::now() + timeout;

        loop {
            match self.slewing().await {
                Ok(false) => return Ok(true),
                Ok(true) => {
                    if std::time::Instant::now() >= deadline {
                        return Ok(false);
                    }
                    tokio::time::sleep(poll_interval).await;
                }
                Err(e) => return Err(AlpacaError::OperationFailed(e)),
            }
        }
    }

    /// Wait for shutter to reach a target state
    pub async fn wait_for_shutter_state(
        &self,
        target_state: ShutterStatus,
        poll_interval: Duration,
        timeout: Duration,
    ) -> Result<bool, AlpacaError> {
        let deadline = std::time::Instant::now() + timeout;

        loop {
            match self.shutter_status().await {
                Ok(state) => {
                    if state == target_state {
                        return Ok(true);
                    }
                    if state == ShutterStatus::Error {
                        return Err(AlpacaError::OperationFailed("Shutter in error state".to_string()));
                    }
                    if std::time::Instant::now() >= deadline {
                        return Ok(false);
                    }
                    tokio::time::sleep(poll_interval).await;
                }
                Err(e) => return Err(AlpacaError::OperationFailed(e)),
            }
        }
    }

    /// Open shutter and wait for completion
    pub async fn open_shutter_and_wait(
        &self,
        poll_interval: Duration,
        timeout: Duration,
    ) -> Result<bool, AlpacaError> {
        // Start opening
        self.open_shutter_typed().await?;
        // Wait for open state
        self.wait_for_shutter_state(ShutterStatus::Open, poll_interval, timeout).await
    }

    /// Close shutter and wait for completion
    pub async fn close_shutter_and_wait(
        &self,
        poll_interval: Duration,
        timeout: Duration,
    ) -> Result<bool, AlpacaError> {
        // Start closing
        self.close_shutter_typed().await?;
        // Wait for closed state
        self.wait_for_shutter_state(ShutterStatus::Closed, poll_interval, timeout).await
    }

    // Status aggregation

    /// Get comprehensive dome status
    pub async fn get_status(&self) -> Result<DomeStatus, String> {
        // Query all status properties in parallel for efficiency
        let (shutter_status, azimuth, altitude, slewing, at_home, at_park, slaved) = tokio::join!(
            self.shutter_status(),
            self.azimuth(),
            self.altitude(),
            self.slewing(),
            self.at_home(),
            self.at_park(),
            self.slaved()
        );

        Ok(DomeStatus {
            shutter_status: shutter_status?,
            azimuth: azimuth?,
            altitude: altitude.ok(),
            slewing: slewing?,
            at_home: at_home?,
            at_park: at_park?,
            slaved: slaved?,
        })
    }
}

/// Dome status aggregate
#[derive(Debug, Clone)]
pub struct DomeStatus {
    pub shutter_status: ShutterStatus,
    pub azimuth: f64,
    pub altitude: Option<f64>,
    pub slewing: bool,
    pub at_home: bool,
    pub at_park: bool,
    pub slaved: bool,
}
