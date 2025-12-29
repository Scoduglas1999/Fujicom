//! Alpaca Focuser API implementation

use crate::{AlpacaClient, AlpacaClientBuilder, AlpacaDevice, AlpacaDeviceType, AlpacaError, TimeoutConfig, RetryConfig};

/// Focuser status aggregate for parallel status query
#[derive(Debug, Clone)]
pub struct FocuserStatus {
    pub connected: bool,
    pub position: i32,
    pub is_moving: bool,
    pub temperature: Option<f64>,
    pub temp_comp: Option<bool>,
}

/// Focuser capabilities
#[derive(Debug, Clone)]
pub struct FocuserCapabilities {
    pub absolute: bool,
    pub max_step: i32,
    pub max_increment: i32,
    pub step_size: Option<f64>,
    pub temp_comp_available: bool,
}

/// Alpaca Focuser client
pub struct AlpacaFocuser {
    client: AlpacaClient,
}

impl AlpacaFocuser {
    /// Create a new Alpaca focuser client
    pub fn new(device: &AlpacaDevice) -> Self {
        assert_eq!(device.device_type, AlpacaDeviceType::Focuser);
        Self {
            client: AlpacaClient::new(device),
        }
    }

    /// Create a focuser client with custom configuration
    pub fn with_config(
        device: &AlpacaDevice,
        timeout_config: TimeoutConfig,
        retry_config: RetryConfig,
    ) -> Self {
        assert_eq!(device.device_type, AlpacaDeviceType::Focuser);
        Self {
            client: AlpacaClient::with_config(device, timeout_config, retry_config),
        }
    }

    /// Create from server details
    pub fn from_server(base_url: &str, device_number: u32) -> Self {
        let device = AlpacaDevice {
            device_type: AlpacaDeviceType::Focuser,
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

    /// Validate connection is alive
    pub async fn validate_connection(&self) -> Result<bool, AlpacaError> {
        self.client.validate_connection().await
    }

    /// Send heartbeat and get round-trip time
    pub async fn heartbeat(&self) -> Result<u64, AlpacaError> {
        self.client.heartbeat().await
    }

    // Focuser information

    pub async fn name(&self) -> Result<String, String> {
        self.client.get_name().await
    }

    pub async fn description(&self) -> Result<String, String> {
        self.client.get_description().await
    }

    // Position

    /// Get the current focuser position
    pub async fn position(&self) -> Result<i32, String> {
        self.client.get("position").await
    }

    /// Get maximum focuser position (steps)
    pub async fn max_step(&self) -> Result<i32, String> {
        self.client.get("maxstep").await
    }

    /// Get maximum increment per move
    pub async fn max_increment(&self) -> Result<i32, String> {
        self.client.get("maxincrement").await
    }

    /// Get step size in microns (if available)
    pub async fn step_size(&self) -> Result<f64, String> {
        self.client.get("stepsize").await
    }

    // Status

    /// Check if focuser is currently moving
    pub async fn is_moving(&self) -> Result<bool, String> {
        self.client.get("ismoving").await
    }

    /// Check if focuser supports absolute positioning
    pub async fn absolute(&self) -> Result<bool, String> {
        self.client.get("absolute").await
    }

    // Temperature compensation

    /// Check if temperature compensation is available
    pub async fn temp_comp_available(&self) -> Result<bool, String> {
        self.client.get("tempcompavailable").await
    }

    /// Check if temperature compensation is active
    pub async fn temp_comp(&self) -> Result<bool, String> {
        self.client.get("tempcomp").await
    }

    /// Enable or disable temperature compensation
    pub async fn set_temp_comp(&self, enabled: bool) -> Result<(), String> {
        self.client.put("tempcomp", &[("TempComp", &enabled.to_string())]).await
    }

    /// Get the current temperature from the focuser (if available)
    pub async fn temperature(&self) -> Result<f64, String> {
        self.client.get("temperature").await
    }

    // Movement commands

    /// Move the focuser to an absolute position
    /// Note: This uses standard timeout. For long moves, use move_to_typed() instead.
    pub async fn move_to(&self, position: i32) -> Result<(), String> {
        self.client.put("move", &[("Position", &position.to_string())]).await
    }

    /// Move the focuser to an absolute position with long timeout
    /// Uses extended timeout appropriate for focuser operations that may take
    /// significant time, especially for full travel moves on slow focusers.
    pub async fn move_to_typed(&self, position: i32) -> Result<(), AlpacaError> {
        self.client.put_long("move", &[("Position", &position.to_string())]).await
    }

    /// Halt any focuser motion
    pub async fn halt(&self) -> Result<(), String> {
        self.client.put("halt", &[]).await
    }

    /// Wait for the focuser to complete its current move operation
    /// Returns Ok(true) if move completed, Ok(false) if timeout reached
    pub async fn wait_for_move_complete(
        &self,
        poll_interval: std::time::Duration,
        timeout: std::time::Duration,
    ) -> Result<bool, AlpacaError> {
        let deadline = std::time::Instant::now() + timeout;

        loop {
            match self.is_moving().await {
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

    /// Move focuser to position and wait for completion
    /// Initiates the move and polls until complete or timeout
    /// Returns Ok(true) if move completed successfully, Ok(false) if timeout reached
    pub async fn move_to_and_wait(
        &self,
        position: i32,
        poll_interval: std::time::Duration,
        timeout: std::time::Duration,
    ) -> Result<bool, AlpacaError> {
        // Initiate the move with long timeout
        self.move_to_typed(position).await?;
        // Wait for completion
        self.wait_for_move_complete(poll_interval, timeout).await
    }

    // Parallel status methods

    /// Get comprehensive focuser status in a single parallel query
    pub async fn get_status(&self) -> Result<FocuserStatus, String> {
        let (connected, position, is_moving, temperature, temp_comp) = tokio::join!(
            self.is_connected(),
            self.position(),
            self.is_moving(),
            self.temperature(),
            self.temp_comp(),
        );

        Ok(FocuserStatus {
            connected: connected?,
            position: position?,
            is_moving: is_moving?,
            temperature: temperature.ok(),
            temp_comp: temp_comp.ok(),
        })
    }

    /// Get focuser capabilities in a single parallel query
    pub async fn get_capabilities(&self) -> Result<FocuserCapabilities, String> {
        let (absolute, max_step, max_increment, step_size, temp_comp_available) = tokio::join!(
            self.absolute(),
            self.max_step(),
            self.max_increment(),
            self.step_size(),
            self.temp_comp_available(),
        );

        Ok(FocuserCapabilities {
            absolute: absolute?,
            max_step: max_step?,
            max_increment: max_increment?,
            step_size: step_size.ok(),
            temp_comp_available: temp_comp_available?,
        })
    }

    /// Wait for focuser to stop moving
    pub async fn wait_for_idle(
        &self,
        poll_interval: std::time::Duration,
        timeout: std::time::Duration,
    ) -> Result<bool, AlpacaError> {
        let deadline = std::time::Instant::now() + timeout;

        loop {
            match self.is_moving().await {
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
}
