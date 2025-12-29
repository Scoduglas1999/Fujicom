//! Alpaca Cover Calibrator API implementation

use crate::{AlpacaClient, AlpacaClientBuilder, AlpacaDevice, AlpacaDeviceType, AlpacaError, TimeoutConfig, RetryConfig};
use std::time::Duration;

/// Cover state enum
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum CoverStatus {
    NotPresent = 0,
    Closed = 1,
    Moving = 2,
    Open = 3,
    Unknown = 4,
    Error = 5,
}

impl From<i32> for CoverStatus {
    fn from(value: i32) -> Self {
        match value {
            0 => CoverStatus::NotPresent,
            1 => CoverStatus::Closed,
            2 => CoverStatus::Moving,
            3 => CoverStatus::Open,
            4 => CoverStatus::Unknown,
            _ => CoverStatus::Error,
        }
    }
}

impl std::fmt::Display for CoverStatus {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            CoverStatus::NotPresent => write!(f, "Not Present"),
            CoverStatus::Closed => write!(f, "Closed"),
            CoverStatus::Moving => write!(f, "Moving"),
            CoverStatus::Open => write!(f, "Open"),
            CoverStatus::Unknown => write!(f, "Unknown"),
            CoverStatus::Error => write!(f, "Error"),
        }
    }
}

/// Calibrator state enum
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum CalibratorStatus {
    NotPresent = 0,
    Off = 1,
    NotReady = 2,
    Ready = 3,
    Unknown = 4,
    Error = 5,
}

impl From<i32> for CalibratorStatus {
    fn from(value: i32) -> Self {
        match value {
            0 => CalibratorStatus::NotPresent,
            1 => CalibratorStatus::Off,
            2 => CalibratorStatus::NotReady,
            3 => CalibratorStatus::Ready,
            4 => CalibratorStatus::Unknown,
            _ => CalibratorStatus::Error,
        }
    }
}

impl std::fmt::Display for CalibratorStatus {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            CalibratorStatus::NotPresent => write!(f, "Not Present"),
            CalibratorStatus::Off => write!(f, "Off"),
            CalibratorStatus::NotReady => write!(f, "Not Ready"),
            CalibratorStatus::Ready => write!(f, "Ready"),
            CalibratorStatus::Unknown => write!(f, "Unknown"),
            CalibratorStatus::Error => write!(f, "Error"),
        }
    }
}

/// Alpaca Cover Calibrator client
pub struct AlpacaCoverCalibrator {
    client: AlpacaClient,
}

impl AlpacaCoverCalibrator {
    /// Create a new Alpaca cover calibrator client
    pub fn new(device: &AlpacaDevice) -> Self {
        assert_eq!(device.device_type, AlpacaDeviceType::CoverCalibrator);
        Self {
            client: AlpacaClient::new(device),
        }
    }

    /// Create a cover calibrator client with custom configuration
    pub fn with_config(
        device: &AlpacaDevice,
        timeout_config: TimeoutConfig,
        retry_config: RetryConfig,
    ) -> Self {
        assert_eq!(device.device_type, AlpacaDeviceType::CoverCalibrator);
        Self {
            client: AlpacaClient::with_config(device, timeout_config, retry_config),
        }
    }

    /// Create from server details
    pub fn from_server(base_url: &str, device_number: u32) -> Self {
        let device = AlpacaDevice {
            device_type: AlpacaDeviceType::CoverCalibrator,
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

    // Cover Calibrator information

    pub async fn name(&self) -> Result<String, String> {
        self.client.get_name().await
    }

    pub async fn description(&self) -> Result<String, String> {
        self.client.get_description().await
    }

    // Status

    pub async fn cover_state(&self) -> Result<CoverStatus, String> {
        let state: i32 = self.client.get("coverstate").await?;
        Ok(CoverStatus::from(state))
    }

    pub async fn calibrator_state(&self) -> Result<CalibratorStatus, String> {
        let state: i32 = self.client.get("calibratorstate").await?;
        Ok(CalibratorStatus::from(state))
    }

    pub async fn brightness(&self) -> Result<i32, String> {
        self.client.get("brightness").await
    }

    pub async fn max_brightness(&self) -> Result<i32, String> {
        self.client.get("maxbrightness").await
    }

    // Cover control

    /// Open the dust cover
    /// Uses long timeout as motorized covers can take time to open
    pub async fn open_cover(&self) -> Result<(), String> {
        self.open_cover_typed().await.map_err(|e| e.to_string())
    }

    /// Open cover with typed error handling and long timeout
    pub async fn open_cover_typed(&self) -> Result<(), AlpacaError> {
        self.client.put_long("opencover", &[]).await
    }

    /// Close the dust cover
    /// Uses long timeout as motorized covers can take time to close
    pub async fn close_cover(&self) -> Result<(), String> {
        self.close_cover_typed().await.map_err(|e| e.to_string())
    }

    /// Close cover with typed error handling and long timeout
    pub async fn close_cover_typed(&self) -> Result<(), AlpacaError> {
        self.client.put_long("closecover", &[]).await
    }

    pub async fn halt_cover(&self) -> Result<(), String> {
        self.client.put("haltcover", &[]).await
    }

    // Calibrator control

    pub async fn calibrator_on(&self, brightness: i32) -> Result<(), String> {
        self.client.put("calibratoron", &[("Brightness", &brightness.to_string())]).await
    }

    pub async fn calibrator_off(&self) -> Result<(), String> {
        self.client.put("calibratoroff", &[]).await
    }

    // Wait methods

    /// Wait for cover to reach a target state
    pub async fn wait_for_cover_state(
        &self,
        target_state: CoverStatus,
        poll_interval: Duration,
        timeout: Duration,
    ) -> Result<bool, AlpacaError> {
        let deadline = std::time::Instant::now() + timeout;

        loop {
            match self.cover_state().await {
                Ok(state) => {
                    if state == target_state {
                        return Ok(true);
                    }
                    if state == CoverStatus::Error {
                        return Err(AlpacaError::OperationFailed("Cover in error state".to_string()));
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

    /// Wait for calibrator to reach ready state
    pub async fn wait_for_calibrator_ready(
        &self,
        poll_interval: Duration,
        timeout: Duration,
    ) -> Result<bool, AlpacaError> {
        let deadline = std::time::Instant::now() + timeout;

        loop {
            match self.calibrator_state().await {
                Ok(CalibratorStatus::Ready) => return Ok(true),
                Ok(CalibratorStatus::Error) => {
                    return Err(AlpacaError::OperationFailed("Calibrator in error state".to_string()));
                }
                Ok(_) => {
                    if std::time::Instant::now() >= deadline {
                        return Ok(false);
                    }
                    tokio::time::sleep(poll_interval).await;
                }
                Err(e) => return Err(AlpacaError::OperationFailed(e)),
            }
        }
    }

    /// Open cover and wait for completion
    pub async fn open_cover_and_wait(
        &self,
        poll_interval: Duration,
        timeout: Duration,
    ) -> Result<bool, AlpacaError> {
        self.open_cover_typed().await?;
        self.wait_for_cover_state(CoverStatus::Open, poll_interval, timeout).await
    }

    /// Close cover and wait for completion
    pub async fn close_cover_and_wait(
        &self,
        poll_interval: Duration,
        timeout: Duration,
    ) -> Result<bool, AlpacaError> {
        self.close_cover_typed().await?;
        self.wait_for_cover_state(CoverStatus::Closed, poll_interval, timeout).await
    }

    /// Get comprehensive cover calibrator status
    pub async fn get_status(&self) -> Result<CoverCalibratorStatus, String> {
        let (cover_state, calibrator_state, brightness) = tokio::join!(
            self.cover_state(),
            self.calibrator_state(),
            self.brightness(),
        );

        Ok(CoverCalibratorStatus {
            cover_state: cover_state?,
            calibrator_state: calibrator_state?,
            brightness: brightness.ok(),
        })
    }
}

/// Cover calibrator status aggregate
#[derive(Debug, Clone)]
pub struct CoverCalibratorStatus {
    pub cover_state: CoverStatus,
    pub calibrator_state: CalibratorStatus,
    pub brightness: Option<i32>,
}
