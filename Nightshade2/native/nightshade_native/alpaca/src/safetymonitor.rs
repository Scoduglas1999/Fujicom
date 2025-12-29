//! Alpaca Safety Monitor API implementation
//!
//! The Safety Monitor interface provides a way to check if observing conditions
//! are safe for operation. This is typically used to check weather conditions,
//! power status, or other safety-critical parameters.

use crate::{AlpacaClient, AlpacaClientBuilder, AlpacaDevice, AlpacaDeviceType, AlpacaError, TimeoutConfig, RetryConfig};

/// Safety monitor status aggregate for parallel status query
#[derive(Debug, Clone)]
pub struct SafetyMonitorStatus {
    pub connected: bool,
    pub is_safe: bool,
}

/// Safety monitor device information
#[derive(Debug, Clone)]
pub struct SafetyMonitorInfo {
    pub name: String,
    pub description: String,
    pub driver_version: String,
    pub driver_info: String,
    pub interface_version: i32,
}

/// Alpaca Safety Monitor client
pub struct AlpacaSafetyMonitor {
    client: AlpacaClient,
}

impl AlpacaSafetyMonitor {
    /// Create a new Alpaca safety monitor client
    pub fn new(device: &AlpacaDevice) -> Self {
        assert_eq!(device.device_type, AlpacaDeviceType::SafetyMonitor);
        Self {
            client: AlpacaClient::new(device),
        }
    }

    /// Create a safety monitor client with custom configuration
    pub fn with_config(
        device: &AlpacaDevice,
        timeout_config: TimeoutConfig,
        retry_config: RetryConfig,
    ) -> Self {
        assert_eq!(device.device_type, AlpacaDeviceType::SafetyMonitor);
        Self {
            client: AlpacaClient::with_config(device, timeout_config, retry_config),
        }
    }

    /// Create from server details
    pub fn from_server(base_url: &str, device_number: u32) -> Self {
        let device = AlpacaDevice {
            device_type: AlpacaDeviceType::SafetyMonitor,
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

    // ============================================================
    // ASCOM Common Methods (all devices implement these)
    // ============================================================

    /// Connect to the device
    pub async fn connect(&self) -> Result<(), String> {
        self.client.connect().await
    }

    /// Connect with typed error
    pub async fn connect_typed(&self) -> Result<(), AlpacaError> {
        self.client.connect_typed().await
    }

    /// Disconnect from the device
    pub async fn disconnect(&self) -> Result<(), String> {
        self.client.disconnect().await
    }

    /// Disconnect with typed error
    pub async fn disconnect_typed(&self) -> Result<(), AlpacaError> {
        self.client.disconnect_typed().await
    }

    /// Check if the device is connected
    pub async fn is_connected(&self) -> Result<bool, String> {
        self.client.is_connected().await
    }

    /// Check if the device is connected (typed error)
    pub async fn is_connected_typed(&self) -> Result<bool, AlpacaError> {
        self.client.is_connected_typed().await
    }

    /// Validate connection is alive (quick check with short timeout)
    pub async fn validate_connection(&self) -> Result<bool, AlpacaError> {
        self.client.validate_connection().await
    }

    /// Send heartbeat and get round-trip time in milliseconds
    pub async fn heartbeat(&self) -> Result<u64, AlpacaError> {
        self.client.heartbeat().await
    }

    /// Get the device name
    pub async fn name(&self) -> Result<String, String> {
        self.client.get_name().await
    }

    /// Get the device name (typed error)
    pub async fn name_typed(&self) -> Result<String, AlpacaError> {
        self.client.get_name_typed().await
    }

    /// Get the device description
    pub async fn description(&self) -> Result<String, String> {
        self.client.get_description().await
    }

    /// Get the device description (typed error)
    pub async fn description_typed(&self) -> Result<String, AlpacaError> {
        self.client.get_description_typed().await
    }

    /// Get the driver version
    pub async fn driver_version(&self) -> Result<String, String> {
        self.client.get_driver_version().await
    }

    /// Get the driver version (typed error)
    pub async fn driver_version_typed(&self) -> Result<String, AlpacaError> {
        self.client.get_driver_version_typed().await
    }

    /// Get the driver info string
    pub async fn driver_info(&self) -> Result<String, String> {
        self.client.get_driver_info().await
    }

    /// Get the interface version (typically 1, 2, or 3 for ISafetyMonitorV1, V2, V3)
    pub async fn interface_version(&self) -> Result<i32, String> {
        self.client.get_interface_version().await
    }

    /// Get supported actions
    /// Returns a list of action names supported by the Action method
    pub async fn supported_actions(&self) -> Result<Vec<String>, String> {
        self.client.get_supported_actions().await
    }

    /// Invoke a device-specific action
    ///
    /// # Arguments
    /// * `action_name` - The name of the action to invoke
    /// * `action_parameters` - Optional parameters for the action
    pub async fn action(&self, action_name: &str, action_parameters: &str) -> Result<String, String> {
        self.client.put("action", &[
            ("Action", action_name),
            ("Parameters", action_parameters),
        ]).await
    }

    /// Send a command directly to the device
    ///
    /// # Arguments
    /// * `command` - The command string to send
    /// * `raw` - If true, the command is sent without interpretation
    pub async fn command_string(&self, command: &str, raw: bool) -> Result<String, String> {
        self.client.put("commandstring", &[
            ("Command", command),
            ("Raw", &raw.to_string()),
        ]).await
    }

    /// Send a blind command to the device (no response expected)
    ///
    /// # Arguments
    /// * `command` - The command string to send
    /// * `raw` - If true, the command is sent without interpretation
    pub async fn command_blind(&self, command: &str, raw: bool) -> Result<(), String> {
        self.client.put("commandblind", &[
            ("Command", command),
            ("Raw", &raw.to_string()),
        ]).await
    }

    /// Send a command and expect a boolean response
    ///
    /// # Arguments
    /// * `command` - The command string to send
    /// * `raw` - If true, the command is sent without interpretation
    pub async fn command_bool(&self, command: &str, raw: bool) -> Result<bool, String> {
        self.client.put("commandbool", &[
            ("Command", command),
            ("Raw", &raw.to_string()),
        ]).await
    }

    // ============================================================
    // Safety Monitor Specific Methods
    // ============================================================

    /// Indicates whether the monitored state is safe for use
    ///
    /// Returns true if conditions are safe, false otherwise.
    /// This is the primary property for determining if observing can proceed.
    pub async fn is_safe(&self) -> Result<bool, String> {
        self.client.get("issafe").await
    }

    /// Indicates whether the monitored state is safe for use (typed error)
    pub async fn is_safe_typed(&self) -> Result<bool, AlpacaError> {
        self.client.get_typed("issafe").await
    }

    // ============================================================
    // Parallel Status Methods
    // ============================================================

    /// Get comprehensive safety monitor status in a single parallel query
    pub async fn get_status(&self) -> Result<SafetyMonitorStatus, String> {
        let (connected, is_safe) = tokio::join!(
            self.is_connected(),
            self.is_safe(),
        );

        Ok(SafetyMonitorStatus {
            connected: connected?,
            is_safe: is_safe?,
        })
    }

    /// Get safety monitor status with typed errors
    pub async fn get_status_typed(&self) -> Result<SafetyMonitorStatus, AlpacaError> {
        let (connected, is_safe) = tokio::join!(
            self.is_connected_typed(),
            self.is_safe_typed(),
        );

        Ok(SafetyMonitorStatus {
            connected: connected?,
            is_safe: is_safe?,
        })
    }

    /// Get device information in a single parallel query
    pub async fn get_info(&self) -> Result<SafetyMonitorInfo, String> {
        let (name, description, driver_version, driver_info, interface_version) = tokio::join!(
            self.name(),
            self.description(),
            self.driver_version(),
            self.driver_info(),
            self.interface_version(),
        );

        Ok(SafetyMonitorInfo {
            name: name?,
            description: description?,
            driver_version: driver_version?,
            driver_info: driver_info.unwrap_or_default(),
            interface_version: interface_version?,
        })
    }

    // ============================================================
    // Utility Methods
    // ============================================================

    /// Wait for safe conditions with polling
    ///
    /// # Arguments
    /// * `poll_interval` - How often to check the safety status
    /// * `timeout` - Maximum time to wait for safe conditions
    ///
    /// # Returns
    /// Ok(true) if conditions became safe within the timeout
    /// Ok(false) if timeout was reached while still unsafe
    /// Err if there was a communication error
    pub async fn wait_for_safe(
        &self,
        poll_interval: std::time::Duration,
        timeout: std::time::Duration,
    ) -> Result<bool, AlpacaError> {
        let deadline = std::time::Instant::now() + timeout;

        loop {
            match self.is_safe_typed().await {
                Ok(true) => return Ok(true),
                Ok(false) => {
                    if std::time::Instant::now() >= deadline {
                        return Ok(false);
                    }
                    tokio::time::sleep(poll_interval).await;
                }
                Err(e) => return Err(e),
            }
        }
    }

    /// Monitor safety status with a callback
    ///
    /// This method continuously polls the safety status and calls the callback
    /// whenever the status changes. It runs until the cancellation token is triggered
    /// or an error occurs.
    ///
    /// # Arguments
    /// * `poll_interval` - How often to check the safety status
    /// * `on_change` - Callback function called with the new safety state
    /// * `cancel` - A future that when resolved will stop the monitoring
    pub async fn monitor_safety<F>(
        &self,
        poll_interval: std::time::Duration,
        mut on_change: F,
    ) -> Result<(), AlpacaError>
    where
        F: FnMut(bool),
    {
        let mut last_state: Option<bool> = None;

        loop {
            match self.is_safe_typed().await {
                Ok(current_state) => {
                    if last_state != Some(current_state) {
                        on_change(current_state);
                        last_state = Some(current_state);
                    }
                }
                Err(e) => return Err(e),
            }
            tokio::time::sleep(poll_interval).await;
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_safety_monitor_status_struct() {
        let status = SafetyMonitorStatus {
            connected: true,
            is_safe: true,
        };
        assert!(status.connected);
        assert!(status.is_safe);
    }

    #[test]
    fn test_safety_monitor_info_struct() {
        let info = SafetyMonitorInfo {
            name: "Test Monitor".to_string(),
            description: "A test safety monitor".to_string(),
            driver_version: "1.0.0".to_string(),
            driver_info: "Test driver".to_string(),
            interface_version: 1,
        };
        assert_eq!(info.name, "Test Monitor");
        assert_eq!(info.interface_version, 1);
    }
}
