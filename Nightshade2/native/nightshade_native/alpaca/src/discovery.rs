//! Alpaca device discovery

use crate::{AlpacaDevice, AlpacaDeviceType, AlpacaError, ALPACA_DISCOVERY_PORT};
use serde::Deserialize;
use std::collections::HashSet;
use std::net::SocketAddr;
use std::time::Duration;
use tokio::net::UdpSocket;
use tokio::time::timeout;
use tracing::{debug, info, warn};

/// Discovery response from an Alpaca server
#[derive(Debug, Deserialize)]
#[serde(rename_all = "PascalCase")]
pub struct DiscoveryResponse {
    pub alpaca_port: u16,
}

/// Configured device from management API
#[derive(Debug, Deserialize)]
#[serde(rename_all = "PascalCase")]
pub struct ConfiguredDevice {
    pub device_name: String,
    pub device_type: String,
    pub device_number: u32,
    pub unique_id: String,
}

/// Discovery configuration
#[derive(Debug, Clone)]
pub struct DiscoveryConfig {
    /// Total discovery timeout
    pub discovery_timeout: Duration,
    /// Time to wait for individual responses
    pub response_wait: Duration,
    /// Timeout for HTTP requests to get device info
    pub http_timeout: Duration,
    /// Number of discovery broadcasts to send
    pub broadcast_count: u32,
    /// Delay between broadcasts
    pub broadcast_delay: Duration,
}

impl Default for DiscoveryConfig {
    fn default() -> Self {
        Self {
            discovery_timeout: Duration::from_secs(5),
            response_wait: Duration::from_millis(500),
            http_timeout: Duration::from_secs(10),
            broadcast_count: 3,
            broadcast_delay: Duration::from_millis(200),
        }
    }
}

impl DiscoveryConfig {
    /// Create a quick discovery config for fast scans
    pub fn quick() -> Self {
        Self {
            discovery_timeout: Duration::from_secs(2),
            response_wait: Duration::from_millis(200),
            http_timeout: Duration::from_secs(5),
            broadcast_count: 1,
            broadcast_delay: Duration::from_millis(100),
        }
    }

    /// Create a thorough discovery config for comprehensive scans
    pub fn thorough() -> Self {
        Self {
            discovery_timeout: Duration::from_secs(10),
            response_wait: Duration::from_millis(1000),
            http_timeout: Duration::from_secs(15),
            broadcast_count: 5,
            broadcast_delay: Duration::from_millis(500),
        }
    }
}

/// Discover Alpaca servers on the local network using async UDP
pub async fn discover_servers(timeout_duration: Duration) -> Vec<(String, u16)> {
    discover_servers_with_config(DiscoveryConfig {
        discovery_timeout: timeout_duration,
        ..Default::default()
    })
    .await
}

/// Discover Alpaca servers with custom configuration
pub async fn discover_servers_with_config(config: DiscoveryConfig) -> Vec<(String, u16)> {
    let mut servers = HashSet::new();

    // Create async UDP socket bound to any available port
    let socket = match UdpSocket::bind("0.0.0.0:0").await {
        Ok(s) => s,
        Err(e) => {
            warn!("Failed to bind UDP socket for discovery: {}", e);
            return Vec::new();
        }
    };

    // Enable broadcast
    if let Err(e) = socket.set_broadcast(true) {
        warn!("Failed to enable broadcast on discovery socket: {}", e);
        return Vec::new();
    }

    let broadcast_addr: SocketAddr = match format!("255.255.255.255:{}", ALPACA_DISCOVERY_PORT).parse() {
        Ok(addr) => addr,
        Err(e) => {
            warn!("Failed to parse broadcast address: {}", e);
            return Vec::new();
        }
    };

    // Send multiple discovery broadcasts
    let discovery_message = b"alpacadiscovery1";

    for broadcast_num in 0..config.broadcast_count {
        if let Err(e) = socket.send_to(discovery_message, broadcast_addr).await {
            warn!("Failed to send discovery broadcast {}: {}", broadcast_num + 1, e);
            continue;
        }
        debug!("Sent discovery broadcast {}/{}", broadcast_num + 1, config.broadcast_count);

        // Wait briefly between broadcasts
        if broadcast_num + 1 < config.broadcast_count {
            tokio::time::sleep(config.broadcast_delay).await;
        }
    }

    // Receive responses with timeout
    let mut buf = [0u8; 1024];
    let deadline = tokio::time::Instant::now() + config.discovery_timeout;

    loop {
        let remaining = deadline.saturating_duration_since(tokio::time::Instant::now());
        if remaining.is_zero() {
            break;
        }

        // Use the configured response wait time or remaining time, whichever is shorter
        let wait_time = remaining.min(config.response_wait);

        match timeout(wait_time, socket.recv_from(&mut buf)).await {
            Ok(Ok((len, addr))) => {
                if let Ok(response) = serde_json::from_slice::<DiscoveryResponse>(&buf[..len]) {
                    let server = (addr.ip().to_string(), response.alpaca_port);
                    if servers.insert(server.clone()) {
                        info!("Discovered Alpaca server at {}:{}", server.0, server.1);
                    }
                } else {
                    debug!("Received non-JSON response from {}", addr);
                }
            }
            Ok(Err(e)) => {
                debug!("Error receiving discovery response: {}", e);
            }
            Err(_) => {
                // Timeout on this receive, continue to check remaining time
                continue;
            }
        }
    }

    servers.into_iter().collect()
}

/// Get configured devices from an Alpaca server
pub async fn get_configured_devices(server_ip: &str, port: u16) -> Result<Vec<AlpacaDevice>, String> {
    get_configured_devices_with_timeout(server_ip, port, Duration::from_secs(10)).await
}

/// Get configured devices from an Alpaca server with custom timeout
pub async fn get_configured_devices_with_timeout(
    server_ip: &str,
    port: u16,
    timeout_duration: Duration,
) -> Result<Vec<AlpacaDevice>, String> {
    let url = format!("http://{}:{}/management/v1/configureddevices", server_ip, port);

    let client = reqwest::Client::builder()
        .timeout(timeout_duration)
        .build()
        .map_err(|e| e.to_string())?;

    let response = client
        .get(&url)
        .send()
        .await
        .map_err(|e| e.to_string())?;

    if !response.status().is_success() {
        return Err(format!(
            "Failed to get configured devices: HTTP {}",
            response.status()
        ));
    }

    #[derive(Deserialize)]
    #[serde(rename_all = "PascalCase")]
    struct ApiResponse {
        value: Vec<ConfiguredDevice>,
    }

    let api_response: ApiResponse = response
        .json()
        .await
        .map_err(|e| e.to_string())?;

    let base_url = format!("http://{}:{}", server_ip, port);

    let devices: Vec<AlpacaDevice> = api_response
        .value
        .into_iter()
        .filter_map(|d| {
            let device_type = match d.device_type.to_lowercase().as_str() {
                "camera" => Some(AlpacaDeviceType::Camera),
                "telescope" => Some(AlpacaDeviceType::Telescope),
                "focuser" => Some(AlpacaDeviceType::Focuser),
                "filterwheel" => Some(AlpacaDeviceType::FilterWheel),
                "dome" => Some(AlpacaDeviceType::Dome),
                "rotator" => Some(AlpacaDeviceType::Rotator),
                "safetymonitor" => Some(AlpacaDeviceType::SafetyMonitor),
                "observingconditions" => Some(AlpacaDeviceType::ObservingConditions),
                "switch" => Some(AlpacaDeviceType::Switch),
                "covercalibrator" => Some(AlpacaDeviceType::CoverCalibrator),
                _ => {
                    debug!("Unknown device type: {}", d.device_type);
                    None
                }
            }?;

            Some(AlpacaDevice {
                device_type,
                device_number: d.device_number,
                server_name: server_ip.to_string(),
                manufacturer: String::new(),
                device_name: d.device_name,
                unique_id: d.unique_id,
                base_url: base_url.clone(),
            })
        })
        .collect();

    Ok(devices)
}

/// Discover all Alpaca devices on the network
pub async fn discover_all_devices(timeout_duration: Duration) -> Vec<AlpacaDevice> {
    discover_all_devices_with_config(DiscoveryConfig {
        discovery_timeout: timeout_duration,
        ..Default::default()
    })
    .await
}

/// Discover all Alpaca devices with custom configuration
pub async fn discover_all_devices_with_config(config: DiscoveryConfig) -> Vec<AlpacaDevice> {
    let mut all_devices = Vec::new();

    let servers = discover_servers_with_config(config.clone()).await;

    // Fetch devices from all servers in parallel
    let fetch_futures: Vec<_> = servers
        .iter()
        .map(|(ip, port)| {
            let ip = ip.clone();
            let port = *port;
            let timeout = config.http_timeout;
            async move {
                match get_configured_devices_with_timeout(&ip, port, timeout).await {
                    Ok(devices) => {
                        info!("Found {} devices at {}:{}", devices.len(), ip, port);
                        devices
                    }
                    Err(e) => {
                        warn!("Failed to get devices from {}:{}: {}", ip, port, e);
                        Vec::new()
                    }
                }
            }
        })
        .collect();

    // Execute all fetches in parallel
    let results = futures::future::join_all(fetch_futures).await;

    for devices in results {
        all_devices.extend(devices);
    }

    all_devices
}

/// Discover a specific type of device on the network
pub async fn discover_devices_of_type(
    device_type: AlpacaDeviceType,
    timeout_duration: Duration,
) -> Vec<AlpacaDevice> {
    discover_all_devices(timeout_duration)
        .await
        .into_iter()
        .filter(|d| d.device_type == device_type)
        .collect()
}

/// Check if a specific server is reachable
pub async fn ping_server(server_ip: &str, port: u16) -> Result<Duration, AlpacaError> {
    let url = format!("http://{}:{}/management/v1/description", server_ip, port);
    let start = std::time::Instant::now();

    let client = reqwest::Client::builder()
        .timeout(Duration::from_secs(5))
        .build()
        .map_err(|e| AlpacaError::RequestFailed(e.to_string()))?;

    match client.get(&url).send().await {
        Ok(response) => {
            if response.status().is_success() {
                Ok(start.elapsed())
            } else {
                Err(AlpacaError::HttpError {
                    status: response.status().as_u16(),
                    message: "Server not responding correctly".to_string(),
                })
            }
        }
        Err(e) => {
            if e.is_timeout() {
                Err(AlpacaError::timeout("server configuration query", 5000))
            } else if e.is_connect() {
                Err(AlpacaError::connection_refused(url, e.to_string()))
            } else {
                Err(AlpacaError::RequestFailed(e.to_string()))
            }
        }
    }
}

/// Get server description
#[derive(Debug, Deserialize)]
#[serde(rename_all = "PascalCase")]
pub struct ServerDescription {
    pub server_name: String,
    pub manufacturer: String,
    pub manufacturer_version: String,
    pub location: String,
}

/// Get the description of an Alpaca server
pub async fn get_server_description(
    server_ip: &str,
    port: u16,
) -> Result<ServerDescription, AlpacaError> {
    let url = format!("http://{}:{}/management/v1/description", server_ip, port);

    let client = reqwest::Client::builder()
        .timeout(Duration::from_secs(10))
        .build()
        .map_err(|e| AlpacaError::RequestFailed(e.to_string()))?;

    let response = client
        .get(&url)
        .send()
        .await
        .map_err(|e| {
            if e.is_timeout() {
                AlpacaError::timeout("server description query", 10000)
            } else if e.is_connect() {
                AlpacaError::connection_refused(&url, e.to_string())
            } else {
                AlpacaError::RequestFailed(e.to_string())
            }
        })?;

    if !response.status().is_success() {
        return Err(AlpacaError::HttpError {
            status: response.status().as_u16(),
            message: "Failed to get server description".to_string(),
        });
    }

    #[derive(Deserialize)]
    #[serde(rename_all = "PascalCase")]
    struct ApiResponse {
        value: ServerDescription,
    }

    let api_response: ApiResponse = response
        .json()
        .await
        .map_err(|e| AlpacaError::ParseError(e.to_string()))?;

    Ok(api_response.value)
}

/// Get supported API versions from a server
pub async fn get_api_versions(server_ip: &str, port: u16) -> Result<Vec<u32>, AlpacaError> {
    let url = format!("http://{}:{}/management/apiversions", server_ip, port);

    let client = reqwest::Client::builder()
        .timeout(Duration::from_secs(5))
        .build()
        .map_err(|e| AlpacaError::RequestFailed(e.to_string()))?;

    let response = client
        .get(&url)
        .send()
        .await
        .map_err(|e| {
            if e.is_timeout() {
                AlpacaError::timeout("API versions query", 5000)
            } else if e.is_connect() {
                AlpacaError::connection_refused(&url, e.to_string())
            } else {
                AlpacaError::RequestFailed(e.to_string())
            }
        })?;

    if !response.status().is_success() {
        return Err(AlpacaError::HttpError {
            status: response.status().as_u16(),
            message: "Failed to get API versions".to_string(),
        });
    }

    #[derive(Deserialize)]
    #[serde(rename_all = "PascalCase")]
    struct ApiResponse {
        value: Vec<u32>,
    }

    let api_response: ApiResponse = response
        .json()
        .await
        .map_err(|e| AlpacaError::ParseError(e.to_string()))?;

    Ok(api_response.value)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_discovery_config_defaults() {
        let config = DiscoveryConfig::default();
        assert_eq!(config.discovery_timeout, Duration::from_secs(5));
        assert_eq!(config.broadcast_count, 3);
    }

    #[test]
    fn test_discovery_config_quick() {
        let config = DiscoveryConfig::quick();
        assert_eq!(config.discovery_timeout, Duration::from_secs(2));
        assert_eq!(config.broadcast_count, 1);
    }

    #[test]
    fn test_discovery_config_thorough() {
        let config = DiscoveryConfig::thorough();
        assert_eq!(config.discovery_timeout, Duration::from_secs(10));
        assert_eq!(config.broadcast_count, 5);
    }
}
