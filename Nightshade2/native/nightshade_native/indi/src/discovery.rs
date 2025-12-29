//! INDI device discovery
//!
//! Provides network scanning for INDI servers and device enumeration.

use crate::{IndiClient, IndiProperty, INDI_DEFAULT_PORT};
use std::net::{SocketAddr, TcpStream};
use std::time::Duration;
use mdns_sd::{ServiceDaemon, ServiceEvent};

/// Discovered INDI server information
#[derive(Debug, Clone)]
pub struct IndiServer {
    pub host: String,
    pub port: u16,
    pub devices: Vec<IndiDeviceInfo>,
}

/// Discovered INDI device information
#[derive(Debug, Clone)]
pub struct IndiDeviceInfo {
    pub name: String,
    pub device_type: IndiDeviceType,
    pub properties: Vec<String>,
}

/// INDI device type (inferred from properties)
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum IndiDeviceType {
    Camera,
    Telescope,
    Focuser,
    FilterWheel,
    Dome,
    Rotator,
    Guider,
    Weather,
    SafetyMonitor,
    CoverCalibrator,
    Unknown,
}

/// Scan localhost for INDI server on default port
pub async fn discover_localhost() -> Option<IndiServer> {
    discover_server("127.0.0.1", INDI_DEFAULT_PORT).await
}

/// Scan a specific host:port for INDI server
pub async fn discover_server(host: &str, port: u16) -> Option<IndiServer> {
    // First, try a quick TCP connect to check if server is listening
    let addr = format!("{}:{}", host, port);
    if !check_port_open(&addr, Duration::from_millis(500)) {
        return None;
    }

    // Server is listening, try to connect and enumerate devices
    let mut client = IndiClient::new(host, Some(port));
    if client.connect().await.is_err() {
        return None;
    }

    // Wait a short time for device definitions to come in
    tokio::time::sleep(Duration::from_millis(1500)).await;

    // Get discovered devices
    let devices = client.get_devices().await;
    let mut device_infos = Vec::new();

    for device in devices {
        let properties = client.get_properties(&device.name).await;
        let device_type = infer_device_type(&properties);
        let property_names: Vec<String> = properties.iter().map(|p| p.name.clone()).collect();

        device_infos.push(IndiDeviceInfo {
            name: device.name,
            device_type,
            properties: property_names,
        });
    }

    // Disconnect
    let _ = client.disconnect().await;

    Some(IndiServer {
        host: host.to_string(),
        port,
        devices: device_infos,
    })
}

/// Scan common hosts for INDI servers
pub async fn discover_common_hosts() -> Vec<IndiServer> {
    let mut servers = Vec::new();

    // Common hosts to check
    let hosts = vec![
        "127.0.0.1",
        "localhost",
        "indiserver",
        "raspberrypi",
        "stellarmate",
        "astroberry",
    ];

    for host in hosts {
        if let Some(server) = discover_server(host, INDI_DEFAULT_PORT).await {
            servers.push(server);
        }
    }

    servers
}

/// Discover INDI servers via mDNS/Bonjour
///
/// Searches for INDI servers advertising themselves via mDNS with service type "_indi._tcp.local."
/// Returns discovered servers with their host and port information.
///
/// # Arguments
/// * `timeout` - How long to listen for mDNS responses
///
/// # Example
/// ```no_run
/// use std::time::Duration;
/// # async fn example() {
/// let servers = nightshade_indi::discover_mdns(Duration::from_secs(5)).await;
/// for server in servers {
///     println!("Found INDI server at {}:{}", server.host, server.port);
/// }
/// # }
/// ```
pub async fn discover_mdns(timeout: Duration) -> Vec<IndiServer> {
    let mut discovered_servers = Vec::new();

    // Create mDNS service daemon
    let mdns = match ServiceDaemon::new() {
        Ok(d) => d,
        Err(e) => {
            tracing::warn!("Failed to create mDNS daemon: {}. mDNS discovery unavailable.", e);
            return discovered_servers;
        }
    };

    // Service type for INDI servers
    let service_type = "_indi._tcp.local.";

    // Browse for INDI services
    let receiver = match mdns.browse(service_type) {
        Ok(r) => r,
        Err(e) => {
            tracing::warn!("Failed to browse mDNS services: {}. mDNS discovery unavailable.", e);
            return discovered_servers;
        }
    };

    tracing::info!("Searching for INDI servers via mDNS for {:?}...", timeout);

    let start = std::time::Instant::now();
    let mut discovered_addresses = std::collections::HashSet::new();

    // Listen for mDNS responses
    while start.elapsed() < timeout {
        let remaining = timeout - start.elapsed();

        match tokio::time::timeout(remaining, tokio::task::spawn_blocking({
            let receiver = receiver.clone();
            move || receiver.recv()
        })).await {
            Ok(Ok(Ok(event))) => {
                match event {
                    ServiceEvent::ServiceResolved(info) => {
                        tracing::debug!("mDNS service resolved: {:?}", info);

                        // Extract host and port
                        let _host = info.get_hostname().to_string();
                        let port = info.get_port();

                        // Use first available address
                        if let Some(addr) = info.get_addresses().iter().next() {
                            let host_ip = addr.to_string();
                            let key = format!("{}:{}", host_ip, port);

                            // Avoid duplicates
                            if !discovered_addresses.contains(&key) {
                                discovered_addresses.insert(key);

                                tracing::info!(
                                    "Found INDI server via mDNS: {} ({}:{})",
                                    info.get_fullname(),
                                    host_ip,
                                    port
                                );

                                // Try to connect and enumerate devices
                                if let Some(server) = discover_server(&host_ip, port).await {
                                    discovered_servers.push(server);
                                } else {
                                    // Even if we can't enumerate, add the server info
                                    discovered_servers.push(IndiServer {
                                        host: host_ip,
                                        port,
                                        devices: Vec::new(),
                                    });
                                }
                            }
                        }
                    }
                    ServiceEvent::SearchStarted(_) => {
                        tracing::debug!("mDNS search started");
                    }
                    ServiceEvent::ServiceFound(ty, fullname) => {
                        tracing::debug!("mDNS service found: {} ({})", fullname, ty);
                    }
                    _ => {}
                }
            }
            Ok(Ok(Err(e))) => {
                tracing::warn!("mDNS receive error: {}", e);
                break;
            }
            Ok(Err(e)) => {
                tracing::warn!("mDNS task error: {}", e);
                break;
            }
            Err(_) => {
                // Timeout - this is expected, we've reached our search duration
                break;
            }
        }
    }

    // Shutdown mDNS daemon
    if let Err(e) = mdns.shutdown() {
        tracing::warn!("Failed to shutdown mDNS daemon: {}", e);
    }

    tracing::info!("mDNS discovery complete. Found {} INDI server(s).", discovered_servers.len());
    discovered_servers
}

/// Scan local subnet for INDI servers (192.168.x.x range)
pub async fn discover_local_network(timeout: Duration) -> Vec<IndiServer> {
    let mut servers = Vec::new();

    // Get local IP addresses to determine subnets to scan
    let subnets = get_local_subnets();

    for subnet in subnets {
        // Scan the subnet in parallel batches
        let mut handles = Vec::new();

        for i in 1..=254u8 {
            let ip = format!("{}.{}.{}.{}", subnet.0, subnet.1, subnet.2, i);
            let timeout = timeout;

            handles.push(tokio::spawn(async move {
                let addr = format!("{}:{}", ip, INDI_DEFAULT_PORT);
                if check_port_open(&addr, timeout) {
                    discover_server(&ip, INDI_DEFAULT_PORT).await
                } else {
                    None
                }
            }));

            // Batch processing to avoid overwhelming the system
            if handles.len() >= 50 {
                for handle in handles.drain(..) {
                    if let Ok(Some(server)) = handle.await {
                        servers.push(server);
                    }
                }
            }
        }

        // Process remaining handles
        for handle in handles {
            if let Ok(Some(server)) = handle.await {
                servers.push(server);
            }
        }
    }

    servers
}

/// Quick TCP port check
fn check_port_open(addr: &str, timeout: Duration) -> bool {
    let addr: SocketAddr = match addr.parse() {
        Ok(a) => a,
        Err(_) => {
            // Try resolving hostname
            if let Ok(addrs) = std::net::ToSocketAddrs::to_socket_addrs(addr) {
                if let Some(a) = addrs.into_iter().next() {
                    a
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }
    };

    TcpStream::connect_timeout(&addr, timeout).is_ok()
}

/// Infer device type from its properties
fn infer_device_type(properties: &[IndiProperty]) -> IndiDeviceType {
    let prop_names: Vec<&str> = properties.iter().map(|p| p.name.as_str()).collect();

    // Check for camera-specific properties
    if prop_names.iter().any(|p| {
        *p == "CCD_EXPOSURE" || *p == "CCD_INFO" || *p == "CCD_FRAME" || *p == "CCD1"
    }) {
        return IndiDeviceType::Camera;
    }

    // Check for mount/telescope properties
    if prop_names.iter().any(|p| {
        *p == "EQUATORIAL_EOD_COORD"
            || *p == "ON_COORD_SET"
            || *p == "TELESCOPE_TRACK_MODE"
            || *p == "TELESCOPE_MOTION_NS"
    }) {
        return IndiDeviceType::Telescope;
    }

    // Check for focuser properties
    if prop_names.iter().any(|p| {
        *p == "ABS_FOCUS_POSITION"
            || *p == "REL_FOCUS_POSITION"
            || *p == "FOCUS_MOTION"
    }) {
        return IndiDeviceType::Focuser;
    }

    // Check for filter wheel properties
    if prop_names.iter().any(|p| *p == "FILTER_SLOT" || *p == "FILTER_NAME") {
        return IndiDeviceType::FilterWheel;
    }

    // Check for dome properties
    if prop_names.iter().any(|p| {
        *p == "DOME_SHUTTER" || *p == "DOME_MOTION" || *p == "ABS_DOME_POSITION"
    }) {
        return IndiDeviceType::Dome;
    }

    // Check for rotator properties
    if prop_names.iter().any(|p| *p == "ABS_ROTATOR_ANGLE" || *p == "ROTATOR_ANGLE") {
        return IndiDeviceType::Rotator;
    }

    // Check for guider properties
    if prop_names.iter().any(|p| *p == "TELESCOPE_TIMED_GUIDE_NS" || *p == "TELESCOPE_TIMED_GUIDE_WE")
    {
        return IndiDeviceType::Guider;
    }

    // Check for safety monitor properties (before weather since safety may include weather)
    if prop_names.iter().any(|p| {
        *p == "SAFETY_STATUS" || *p == "AUX_SAFETY"
    }) {
        return IndiDeviceType::SafetyMonitor;
    }

    // Check for weather properties
    if prop_names.iter().any(|p| {
        *p == "WEATHER_STATUS" || *p == "WEATHER_PARAMETERS"
    }) {
        // Weather devices with safety status are primarily safety monitors
        if prop_names.iter().any(|p| p.contains("SAFE")) {
            return IndiDeviceType::SafetyMonitor;
        }
        return IndiDeviceType::Weather;
    }

    // Check for cover calibrator / flat panel properties
    // INDI uses: CAP_PARK (dust cap), FLAT_LIGHT_CONTROL, FLAT_LIGHT_INTENSITY
    if prop_names.iter().any(|p| {
        *p == "CAP_PARK"
            || *p == "FLAT_LIGHT_CONTROL"
            || *p == "FLAT_LIGHT_INTENSITY"
            || *p == "DUSTCAP_CONTROL"
            || *p == "LIGHTBOX_BRIGHTNESS"
    }) {
        return IndiDeviceType::CoverCalibrator;
    }

    IndiDeviceType::Unknown
}

/// Get local subnet prefixes (first 3 octets of local IPs)
fn get_local_subnets() -> Vec<(u8, u8, u8)> {
    let mut subnets = Vec::new();

    // Common private network ranges to check
    // This is a simplified approach - in production you might use
    // platform-specific APIs to get actual local IP addresses

    // Add common default subnets
    subnets.push((192, 168, 1));
    subnets.push((192, 168, 0));
    subnets.push((10, 0, 0));

    subnets
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn test_discover_localhost() {
        // This test only passes if an INDI server is running locally
        let result = discover_localhost().await;
        println!("Localhost discovery result: {:?}", result);
    }
}
