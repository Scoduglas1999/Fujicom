//! Alpaca Filter Wheel API implementation

use crate::{AlpacaClient, AlpacaClientBuilder, AlpacaDevice, AlpacaDeviceType, AlpacaError, TimeoutConfig, RetryConfig};

/// Filter wheel status aggregate for parallel status query
#[derive(Debug, Clone)]
pub struct FilterWheelStatus {
    pub connected: bool,
    pub position: i32,
    pub is_moving: bool,
}

/// Filter wheel information including names and offsets
#[derive(Debug, Clone)]
pub struct FilterWheelInfo {
    pub names: Vec<String>,
    pub focus_offsets: Vec<i32>,
}

/// Alpaca Filter Wheel client
pub struct AlpacaFilterWheel {
    client: AlpacaClient,
}

impl AlpacaFilterWheel {
    /// Create a new Alpaca filter wheel client
    pub fn new(device: &AlpacaDevice) -> Self {
        assert_eq!(device.device_type, AlpacaDeviceType::FilterWheel);
        Self {
            client: AlpacaClient::new(device),
        }
    }

    /// Create a filter wheel client with custom configuration
    pub fn with_config(
        device: &AlpacaDevice,
        timeout_config: TimeoutConfig,
        retry_config: RetryConfig,
    ) -> Self {
        assert_eq!(device.device_type, AlpacaDeviceType::FilterWheel);
        Self {
            client: AlpacaClient::with_config(device, timeout_config, retry_config),
        }
    }

    /// Create from server details
    pub fn from_server(base_url: &str, device_number: u32) -> Self {
        let device = AlpacaDevice {
            device_type: AlpacaDeviceType::FilterWheel,
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

    // Filter Wheel information

    pub async fn name(&self) -> Result<String, String> {
        self.client.get_name().await
    }

    pub async fn description(&self) -> Result<String, String> {
        self.client.get_description().await
    }

    // Position

    /// Get the current filter position (0-based)
    /// Returns -1 if the wheel is moving
    pub async fn position(&self) -> Result<i32, String> {
        self.client.get("position").await
    }

    /// Set the filter wheel position (0-based)
    pub async fn set_position(&self, position: i32) -> Result<(), String> {
        self.client.put("position", &[("Position", &position.to_string())]).await
    }

    /// Check if filter wheel is currently moving
    /// Position returns -1 during movement, so we check for that
    pub async fn is_moving(&self) -> Result<bool, String> {
        let pos = self.position().await?;
        Ok(pos == -1)
    }

    // Filter information

    /// Get the filter names
    pub async fn names(&self) -> Result<Vec<String>, String> {
        self.client.get("names").await
    }

    /// Get the focus offsets for each filter
    pub async fn focus_offsets(&self) -> Result<Vec<i32>, String> {
        self.client.get("focusoffsets").await
    }

    // Parallel status methods

    /// Get comprehensive filter wheel status in a single query
    pub async fn get_status(&self) -> Result<FilterWheelStatus, String> {
        let (connected, position) = tokio::join!(
            self.is_connected(),
            self.position(),
        );

        let pos = position?;
        Ok(FilterWheelStatus {
            connected: connected?,
            position: pos,
            is_moving: pos == -1,
        })
    }

    /// Get filter wheel configuration in a single parallel query
    pub async fn get_filter_info(&self) -> Result<FilterWheelInfo, String> {
        let (names, focus_offsets) = tokio::join!(
            self.names(),
            self.focus_offsets(),
        );

        Ok(FilterWheelInfo {
            names: names?,
            focus_offsets: focus_offsets?,
        })
    }

    /// Wait for filter wheel to stop moving
    pub async fn wait_for_idle(
        &self,
        poll_interval: std::time::Duration,
        timeout: std::time::Duration,
    ) -> Result<bool, AlpacaError> {
        let deadline = std::time::Instant::now() + timeout;

        loop {
            match self.position().await {
                Ok(pos) if pos >= 0 => return Ok(true),
                Ok(_) => {
                    // Position is -1 (moving)
                    if std::time::Instant::now() >= deadline {
                        return Ok(false);
                    }
                    tokio::time::sleep(poll_interval).await;
                }
                Err(e) => return Err(AlpacaError::OperationFailed(e)),
            }
        }
    }

    /// Set filter by name (convenience method)
    pub async fn set_filter_by_name(&self, filter_name: &str) -> Result<(), String> {
        let names = self.names().await?;
        for (idx, name) in names.iter().enumerate() {
            if name.eq_ignore_ascii_case(filter_name) {
                return self.set_position(idx as i32).await;
            }
        }
        Err(format!("Filter '{}' not found", filter_name))
    }

    /// Get the current filter name
    pub async fn current_filter_name(&self) -> Result<String, String> {
        let pos = self.position().await?;
        if pos < 0 {
            return Err("Filter wheel is moving".to_string());
        }
        let names = self.names().await?;
        names.get(pos as usize)
            .cloned()
            .ok_or_else(|| format!("Invalid position: {}", pos))
    }

    /// Get the focus offset for the current filter
    pub async fn current_focus_offset(&self) -> Result<i32, String> {
        let pos = self.position().await?;
        if pos < 0 {
            return Err("Filter wheel is moving".to_string());
        }
        let offsets = self.focus_offsets().await?;
        offsets.get(pos as usize)
            .copied()
            .ok_or_else(|| format!("Invalid position: {}", pos))
    }
}
