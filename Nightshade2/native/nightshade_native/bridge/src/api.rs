//! Public API exposed to Dart via flutter_rust_bridge
//!
//! This module contains all the functions that can be called from Dart.
//! Each function is marked with the appropriate flutter_rust_bridge attributes.

use crate::device::*;
use crate::error::*;
use crate::event::*;
use crate::state::*;
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::Arc;
use tokio::sync::RwLock;
use std::sync::OnceLock;
use std::time::{Duration, Instant};
use crate::devices::DeviceManager;
use crate::real_device_ops::RealDeviceOps;
use crate::unified_device_ops::create_unified_device_ops;
use nightshade_sequencer::DeviceOps;
use nightshade_imaging::{ImageData, write_fits, FitsHeader, BayerPattern, DebayerAlgorithm, validate_image, calculate_airmass, validate_fits_header};
use rayon::prelude::*;
use crate::storage::{AppSettings, ObserverLocation};
use serde::{Serialize, Deserialize};
use tokio::sync::Mutex;

/// Global application state singleton
static APP_STATE: OnceLock<SharedAppState> = OnceLock::new();

/// Get or initialize the global application state
#[flutter_rust_bridge::frb(ignore)]
pub fn get_state() -> &'static SharedAppState {
    APP_STATE.get_or_init(AppState::new)
}

/// Global device manager singleton
static DEVICE_MANAGER: OnceLock<Arc<DeviceManager>> = OnceLock::new();

/// Get or initialize the global device manager
#[flutter_rust_bridge::frb(ignore)]
pub fn get_device_manager() -> &'static Arc<DeviceManager> {
    DEVICE_MANAGER.get_or_init(|| DeviceManager::new(get_state().clone()))
}

// =============================================================================
// Discovery Cache (ASCOM + Alpaca)
// =============================================================================

/// Cache for discovered devices to avoid redundant discovery operations.
/// Native discovery has its own cache; this caches ASCOM and Alpaca results.
struct DiscoveryCache {
    /// All discovered ASCOM devices (Windows only)
    ascom_devices: Vec<DeviceInfo>,
    /// All discovered Alpaca devices
    alpaca_devices: Vec<DeviceInfo>,
    /// When the cache was last populated
    timestamp: Instant,
}

/// Global discovery cache
static DISCOVERY_CACHE: OnceLock<Mutex<Option<DiscoveryCache>>> = OnceLock::new();

/// How long to cache ASCOM/Alpaca discovery results (60 seconds)
const DISCOVERY_CACHE_TTL: Duration = Duration::from_secs(60);

/// Get or initialize the discovery cache
fn get_discovery_cache() -> &'static Mutex<Option<DiscoveryCache>> {
    DISCOVERY_CACHE.get_or_init(|| Mutex::new(None))
}

/// Discovery state to prevent concurrent discovery operations
static DISCOVERY_IN_PROGRESS: OnceLock<Mutex<bool>> = OnceLock::new();

fn get_discovery_lock() -> &'static Mutex<bool> {
    DISCOVERY_IN_PROGRESS.get_or_init(|| Mutex::new(false))
}

/// Invalidate the discovery cache, forcing fresh discovery on next call.
/// Called when user explicitly requests a rescan.
pub async fn api_invalidate_discovery_cache() {
    let mut cache = get_discovery_cache().lock().await;
    *cache = None;
    tracing::info!("Discovery cache invalidated");
}

// =============================================================================
// Initialization
// =============================================================================

/// Initialize the native bridge with optional file logging
/// Must be called once at app startup before any other API calls
///
/// # Arguments
/// * `log_directory` - Optional path to store log files. If None, logs only to console.
#[flutter_rust_bridge::frb(sync)]
pub fn api_init_with_logging(log_directory: Option<String>) -> Result<(), NightshadeError> {
    // Initialize logging (with file output if directory provided)
    crate::init_native_with_logging(log_directory)?;

    tracing::info!("Nightshade Native API initialized");

    // Initialize the app state
    let _ = get_state();

    // Initialize device manager (this will spawn Tokio tasks, so runtime must exist)
    let _ = get_device_manager();

    // Publish system initialized event
    get_state().publish_system_event(SystemEvent::Initialized);

    Ok(())
}

/// Initialize the native bridge and return success (console logging only)
/// Must be called once at app startup before any other API calls
#[flutter_rust_bridge::frb(sync)]
pub fn api_init() -> Result<(), NightshadeError> {
    api_init_with_logging(None)
}

/// Get the version of the native library
#[flutter_rust_bridge::frb(sync)]
pub fn api_get_version() -> String {
    env!("CARGO_PKG_VERSION").to_string()
}

/// Get the current log directory path
#[flutter_rust_bridge::frb(sync)]
pub fn api_get_log_directory() -> Option<String> {
    crate::get_log_directory()
}

/// Get the current log file path (today's log)
#[flutter_rust_bridge::frb(sync)]
pub fn api_get_current_log_file() -> Option<String> {
    crate::get_current_log_file()
}

/// List all available log files
pub fn api_list_log_files() -> Vec<String> {
    crate::list_log_files()
}

/// Read a log file's contents
pub fn api_read_log_file(path: String) -> Result<String, NightshadeError> {
    crate::read_log_file(path)
}

/// Export all logs to a single file for diagnostics
pub fn api_export_logs(output_path: String) -> Result<(), NightshadeError> {
    crate::export_logs_to_file(output_path)
}

// =============================================================================
// Event Stream
// =============================================================================

/// Stream of events from the native side
/// The Dart side should listen to this stream for UI updates
pub async fn api_event_stream(sink: crate::frb_generated::StreamSink<NightshadeEvent>) -> anyhow::Result<()> {
    tracing::info!("[API_EVENT_STREAM] Starting event stream function");

    let mut rx = get_state().event_bus.subscribe();
    tracing::info!("[API_EVENT_STREAM] Subscribed to event bus");

    // Send a ready signal so Dart knows the subscription is active
    // This prevents race conditions where events are published before we're subscribed
    sink.add(create_event(
        EventSeverity::Info,
        EventCategory::System,
        EventPayload::System(SystemEvent::Notification {
            title: "EventStreamReady".to_string(),
            message: "Event stream subscription is active".to_string(),
            level: "debug".to_string(),
        }),
    ));
    tracing::info!("[API_EVENT_STREAM] Sent ready signal to Dart");

    loop {
        match rx.recv().await {
            Ok(event) => {
                tracing::debug!("[API_EVENT_STREAM] Forwarding event to Dart: {:?}", std::mem::discriminant(&event.payload));
                sink.add(event);
            }
            Err(tokio::sync::broadcast::error::RecvError::Lagged(n)) => {
                tracing::warn!("[API_EVENT_STREAM] Event stream lagged, missed {} events", n);
            }
            Err(tokio::sync::broadcast::error::RecvError::Closed) => {
                tracing::info!("[API_EVENT_STREAM] Event bus closed, stopping stream");
                break;
            }
        }
    }

    Ok(())
}

// =============================================================================
// Device Discovery - ASCOM and ALPACA IMPLEMENTATION
// =============================================================================

/// Discover available Alpaca devices on the network
pub async fn api_discover_alpaca_devices() -> Result<Vec<DeviceInfo>, NightshadeError> {
    use nightshade_alpaca::{discover_all_devices, AlpacaDeviceType};
    use std::time::Duration;
    
    tracing::info!("Discovering Alpaca devices on network...");
    
    let alpaca_devices = discover_all_devices(Duration::from_secs(3)).await;

    let mut devices = Vec::new();
    for alpaca_dev in alpaca_devices {
        let device_type = match alpaca_dev.device_type {
            AlpacaDeviceType::Camera => DeviceType::Camera,
            AlpacaDeviceType::Telescope => DeviceType::Mount,
            AlpacaDeviceType::Focuser => DeviceType::Focuser,
            AlpacaDeviceType::FilterWheel => DeviceType::FilterWheel,
            AlpacaDeviceType::Rotator => DeviceType::Rotator,
            AlpacaDeviceType::Dome => DeviceType::Dome,
            AlpacaDeviceType::SafetyMonitor => DeviceType::SafetyMonitor,
            AlpacaDeviceType::ObservingConditions => DeviceType::Weather,
            AlpacaDeviceType::Switch => DeviceType::Switch,
            AlpacaDeviceType::CoverCalibrator => DeviceType::CoverCalibrator,
        };

        tracing::info!("Found Alpaca device: {} at {} (unique_id: {})",
            alpaca_dev.device_name, alpaca_dev.base_url, alpaca_dev.unique_id);

        // Generate display name using unique_id for disambiguation
        let unique_id = if alpaca_dev.unique_id.is_empty() { None } else { Some(alpaca_dev.unique_id.clone()) };
        let display_name = DeviceInfo::generate_display_name(
            &alpaca_dev.device_name,
            None, // No serial number from Alpaca
            unique_id.as_deref(),
            None, // No index needed
        );

        devices.push(DeviceInfo {
            id: alpaca_dev.id(),
            name: alpaca_dev.device_name.clone(),
            device_type,
            driver_type: DriverType::Alpaca,
            description: format!("Alpaca device at {}", alpaca_dev.base_url),
            driver_version: "Alpaca".to_string(),
            serial_number: None,
            unique_id,
            display_name,
        });
    }

    tracing::info!("Found {} Alpaca devices", devices.len());
    Ok(devices)
}

/// Discover Alpaca devices at a specific server address
pub async fn api_discover_alpaca_at_address(host: String, port: u16) -> Result<Vec<DeviceInfo>, NightshadeError> {
    use nightshade_alpaca::{get_configured_devices, AlpacaDeviceType};
    
    tracing::info!("Discovering Alpaca devices at {}:{}", host, port);
    
    let alpaca_devices = get_configured_devices(&host, port).await
        .map_err(|e| NightshadeError::ConnectionFailed(format!("Failed to connect to Alpaca server: {}", e)))?;

    let mut devices = Vec::new();
    for alpaca_dev in alpaca_devices {
        let device_type = match alpaca_dev.device_type {
            AlpacaDeviceType::Camera => DeviceType::Camera,
            AlpacaDeviceType::Telescope => DeviceType::Mount,
            AlpacaDeviceType::Focuser => DeviceType::Focuser,
            AlpacaDeviceType::FilterWheel => DeviceType::FilterWheel,
            AlpacaDeviceType::Rotator => DeviceType::Rotator,
            AlpacaDeviceType::Dome => DeviceType::Dome,
            AlpacaDeviceType::SafetyMonitor => DeviceType::SafetyMonitor,
            AlpacaDeviceType::ObservingConditions => DeviceType::Weather,
            AlpacaDeviceType::Switch => DeviceType::Switch,
            AlpacaDeviceType::CoverCalibrator => DeviceType::CoverCalibrator,
        };

        // Generate display name using unique_id for disambiguation
        let unique_id = if alpaca_dev.unique_id.is_empty() { None } else { Some(alpaca_dev.unique_id.clone()) };
        let display_name = DeviceInfo::generate_display_name(
            &alpaca_dev.device_name,
            None,
            unique_id.as_deref(),
            None,
        );

        devices.push(DeviceInfo {
            id: alpaca_dev.id(),
            name: alpaca_dev.device_name.clone(),
            device_type,
            driver_type: DriverType::Alpaca,
            description: format!("Alpaca device at {}", alpaca_dev.base_url),
            driver_version: "Alpaca".to_string(),
            serial_number: None,
            unique_id,
            display_name,
        });
    }

    Ok(devices)
}

/// Discover INDI devices at a specific server address
pub async fn api_discover_indi_at_address(host: String, port: u16) -> Result<Vec<DeviceInfo>, NightshadeError> {
    tracing::info!("Discovering INDI devices at {}:{}", host, port);

    get_device_manager().discover_indi_devices(&host, port).await
        .map_err(|e| NightshadeError::ConnectionFailed(format!("Failed to connect to INDI server: {}", e)))
}

/// Auto-discover INDI servers on localhost
pub async fn api_discover_indi_localhost() -> Result<Vec<DeviceInfo>, NightshadeError> {
    use nightshade_indi::{discover_localhost, IndiDeviceType as IndiType};

    tracing::info!("Auto-discovering INDI servers on localhost...");

    let mut all_devices = Vec::new();

    if let Some(server) = discover_localhost().await {
        tracing::info!("Found INDI server at {}:{} with {} devices",
            server.host, server.port, server.devices.len());

        for device in server.devices {
            let device_type = match device.device_type {
                IndiType::Camera => DeviceType::Camera,
                IndiType::Telescope => DeviceType::Mount,
                IndiType::Focuser => DeviceType::Focuser,
                IndiType::FilterWheel => DeviceType::FilterWheel,
                IndiType::Dome => DeviceType::Dome,
                IndiType::Rotator => DeviceType::Rotator,
                IndiType::Guider => DeviceType::Guider,
                IndiType::Weather => DeviceType::Weather,
                IndiType::SafetyMonitor => DeviceType::SafetyMonitor,
                IndiType::CoverCalibrator => DeviceType::CoverCalibrator,
                IndiType::Unknown => continue,
            };

            let device_id = format!("indi:{}:{}:{}", server.host, server.port, device.name);

            // TODO: Query DEVICE_INFO property for serial number
            all_devices.push(DeviceInfo {
                id: device_id,
                name: device.name.clone(),
                device_type,
                driver_type: DriverType::Indi,
                description: format!("INDI device at {}:{}", server.host, server.port),
                driver_version: "INDI".to_string(),
                serial_number: None,
                unique_id: None,
                display_name: device.name.clone(),
            });
        }
    }

    tracing::info!("Found {} INDI devices on localhost", all_devices.len());
    Ok(all_devices)
}

/// Auto-discover INDI servers on common hostnames (localhost, raspberrypi, stellarmate, etc.)
pub async fn api_discover_indi_common_hosts() -> Result<Vec<DeviceInfo>, NightshadeError> {
    use nightshade_indi::{discover_common_hosts, IndiDeviceType as IndiType};

    tracing::info!("Auto-discovering INDI servers on common hosts...");

    let mut all_devices = Vec::new();
    let servers = discover_common_hosts().await;

    tracing::info!("Found {} INDI servers on common hosts", servers.len());

    for server in servers {
        for device in server.devices {
            let device_type = match device.device_type {
                IndiType::Camera => DeviceType::Camera,
                IndiType::Telescope => DeviceType::Mount,
                IndiType::Focuser => DeviceType::Focuser,
                IndiType::FilterWheel => DeviceType::FilterWheel,
                IndiType::Dome => DeviceType::Dome,
                IndiType::Rotator => DeviceType::Rotator,
                IndiType::Guider => DeviceType::Guider,
                IndiType::Weather => DeviceType::Weather,
                IndiType::SafetyMonitor => DeviceType::SafetyMonitor,
                IndiType::CoverCalibrator => DeviceType::CoverCalibrator,
                IndiType::Unknown => continue,
            };

            let device_id = format!("indi:{}:{}:{}", server.host, server.port, device.name);

            // TODO: Query DEVICE_INFO property for serial number
            all_devices.push(DeviceInfo {
                id: device_id,
                name: device.name.clone(),
                device_type,
                driver_type: DriverType::Indi,
                description: format!("INDI device at {}:{}", server.host, server.port),
                driver_version: "INDI".to_string(),
                serial_number: None,
                unique_id: None,
                display_name: device.name.clone(),
            });
        }
    }

    tracing::info!("Found {} INDI devices total", all_devices.len());
    Ok(all_devices)
}

/// Auto-discover INDI servers on the local network (scans subnet)
pub async fn api_discover_indi_network() -> Result<Vec<DeviceInfo>, NightshadeError> {
    use nightshade_indi::{discover_local_network, IndiDeviceType as IndiType};
    use std::time::Duration;

    tracing::info!("Scanning local network for INDI servers (this may take a while)...");

    let mut all_devices = Vec::new();
    let servers = discover_local_network(Duration::from_millis(200)).await;

    tracing::info!("Found {} INDI servers on local network", servers.len());

    for server in servers {
        for device in server.devices {
            let device_type = match device.device_type {
                IndiType::Camera => DeviceType::Camera,
                IndiType::Telescope => DeviceType::Mount,
                IndiType::Focuser => DeviceType::Focuser,
                IndiType::FilterWheel => DeviceType::FilterWheel,
                IndiType::Dome => DeviceType::Dome,
                IndiType::Rotator => DeviceType::Rotator,
                IndiType::Guider => DeviceType::Guider,
                IndiType::Weather => DeviceType::Weather,
                IndiType::SafetyMonitor => DeviceType::SafetyMonitor,
                IndiType::CoverCalibrator => DeviceType::CoverCalibrator,
                IndiType::Unknown => continue,
            };

            let device_id = format!("indi:{}:{}:{}", server.host, server.port, device.name);

            // TODO: Query DEVICE_INFO property for serial number
            all_devices.push(DeviceInfo {
                id: device_id,
                name: device.name.clone(),
                device_type,
                driver_type: DriverType::Indi,
                description: format!("INDI device at {}:{}", server.host, server.port),
                driver_version: "INDI".to_string(),
                serial_number: None,
                unique_id: None,
                display_name: device.name.clone(),
            });
        }
    }

    tracing::info!("Found {} INDI devices on network", all_devices.len());
    Ok(all_devices)
}

/// Discover available devices of a specific type
/// Queries ASCOM drivers on Windows via COM, Alpaca cross-platform, plus includes simulators.
/// Results are cached for 60 seconds to avoid redundant discovery operations.
pub async fn api_discover_devices(device_type: DeviceType) -> Result<Vec<DeviceInfo>, NightshadeError> {
    tracing::info!("Discovering {} devices", device_type.as_str());

    let mut devices = Vec::new();

    // =====================================================
    // CACHED ASCOM + ALPACA DISCOVERY
    // =====================================================
    // Check if we have valid cached results for ASCOM and Alpaca
    let cached_results = {
        let cache = get_discovery_cache().lock().await;
        if let Some(ref cached) = *cache {
            if cached.timestamp.elapsed() < DISCOVERY_CACHE_TTL {
                tracing::debug!(
                    "Using cached ASCOM/Alpaca discovery ({} ASCOM, {} Alpaca devices, {:.1}s old)",
                    cached.ascom_devices.len(),
                    cached.alpaca_devices.len(),
                    cached.timestamp.elapsed().as_secs_f32()
                );
                Some((cached.ascom_devices.clone(), cached.alpaca_devices.clone()))
            } else {
                None
            }
        } else {
            None
        }
    };

    let (ascom_devices, alpaca_devices) = if let Some((ascom, alpaca)) = cached_results {
        (ascom, alpaca)
    } else {
        // Need to run fresh discovery - acquire lock to prevent concurrent discovery
        let mut in_progress = get_discovery_lock().lock().await;

        // Double-check cache after acquiring lock (another task may have populated it)
        let cached_after_lock = {
            let cache = get_discovery_cache().lock().await;
            if let Some(ref cached) = *cache {
                if cached.timestamp.elapsed() < DISCOVERY_CACHE_TTL {
                    Some((cached.ascom_devices.clone(), cached.alpaca_devices.clone()))
                } else {
                    None
                }
            } else {
                None
            }
        };

        if let Some((ascom, alpaca)) = cached_after_lock {
            (ascom, alpaca)
        } else {
            // Run discovery for ALL device types at once
            tracing::info!("Running full ASCOM/Alpaca discovery (will cache results)...");
            *in_progress = true;

            let mut all_ascom = Vec::new();
            let mut all_alpaca = Vec::new();

            // ASCOM discovery (Windows only) - discover ALL types at once
            #[cfg(windows)]
            {
                use nightshade_ascom::{AscomDeviceType, discover_devices as ascom_discover};

                let ascom_types = [
                    (AscomDeviceType::Camera, DeviceType::Camera),
                    (AscomDeviceType::Telescope, DeviceType::Mount),
                    (AscomDeviceType::Focuser, DeviceType::Focuser),
                    (AscomDeviceType::FilterWheel, DeviceType::FilterWheel),
                    (AscomDeviceType::Rotator, DeviceType::Rotator),
                    (AscomDeviceType::Dome, DeviceType::Dome),
                    (AscomDeviceType::ObservingConditions, DeviceType::Weather),
                    (AscomDeviceType::SafetyMonitor, DeviceType::SafetyMonitor),
                    (AscomDeviceType::CoverCalibrator, DeviceType::CoverCalibrator),
                ];

                for (ascom_type, dev_type) in ascom_types {
                    let ascom_devs = ascom_discover(ascom_type);
                    for ascom_dev in ascom_devs {
                        let prog_id_lower = ascom_dev.prog_id.to_lowercase();
                        let name_lower = ascom_dev.name.to_lowercase();

                        // Filter out simulators and diagnostic tools:
                        // - "simulator" or "sim" patterns (CCDSim, ScopeSim, FocusSim, OmniSim, etc.)
                        // - Hub/Pipe/POTH diagnostic tools that aren't real devices
                        let is_simulator = prog_id_lower.contains("simulator")
                            || name_lower.contains("simulator")
                            || prog_id_lower.contains("sim.")
                            || prog_id_lower.ends_with("sim")
                            || prog_id_lower.starts_with("ccdsim")
                            || prog_id_lower.starts_with("scopesim")
                            || prog_id_lower.starts_with("focussim")
                            || prog_id_lower.starts_with("domesim")
                            || prog_id_lower.starts_with("filterwheelsim")
                            || name_lower == "simulator";

                        let is_diagnostic = prog_id_lower.contains("hub.")
                            || prog_id_lower.contains("pipe.")
                            || prog_id_lower.contains("poth.")
                            || prog_id_lower.starts_with("hub.")
                            || prog_id_lower.starts_with("pipe.")
                            || prog_id_lower.starts_with("poth.");

                        if is_simulator || is_diagnostic {
                            tracing::debug!("Filtering out ASCOM device: {} ({})", ascom_dev.name, ascom_dev.prog_id);
                            continue;
                        }
                        all_ascom.push(DeviceInfo {
                            id: format!("ascom:{}", ascom_dev.prog_id),
                            name: ascom_dev.name.clone(),
                            device_type: dev_type,
                            driver_type: DriverType::Ascom,
                            description: ascom_dev.description,
                            driver_version: "ASCOM".to_string(),
                            serial_number: None,
                            unique_id: None,
                            display_name: ascom_dev.name.clone(),
                        });
                    }
                }
                tracing::info!("ASCOM discovery complete: found {} drivers", all_ascom.len());
            }

            // Alpaca discovery - discovers ALL device types in one broadcast
            {
                use nightshade_alpaca::{discover_all_devices, AlpacaDeviceType};

                let alpaca_devs = discover_all_devices(Duration::from_secs(2)).await;
                for alpaca_dev in alpaca_devs {
                    let dev_type = match alpaca_dev.device_type {
                        AlpacaDeviceType::Camera => DeviceType::Camera,
                        AlpacaDeviceType::Telescope => DeviceType::Mount,
                        AlpacaDeviceType::Focuser => DeviceType::Focuser,
                        AlpacaDeviceType::FilterWheel => DeviceType::FilterWheel,
                        AlpacaDeviceType::Rotator => DeviceType::Rotator,
                        AlpacaDeviceType::Dome => DeviceType::Dome,
                        AlpacaDeviceType::SafetyMonitor => DeviceType::SafetyMonitor,
                        AlpacaDeviceType::ObservingConditions => DeviceType::Weather,
                        AlpacaDeviceType::CoverCalibrator => DeviceType::CoverCalibrator,
                        _ => continue,
                    };

                    let unique_id = if alpaca_dev.unique_id.is_empty() { None } else { Some(alpaca_dev.unique_id.clone()) };
                    let display_name = DeviceInfo::generate_display_name(
                        &alpaca_dev.device_name,
                        None,
                        unique_id.as_deref(),
                        None,
                    );

                    all_alpaca.push(DeviceInfo {
                        id: alpaca_dev.id(),
                        name: alpaca_dev.device_name.clone(),
                        device_type: dev_type,
                        driver_type: DriverType::Alpaca,
                        description: format!("Alpaca device at {}", alpaca_dev.base_url),
                        driver_version: "Alpaca".to_string(),
                        serial_number: None,
                        unique_id,
                        display_name,
                    });
                }
                tracing::info!("Alpaca discovery complete: found {} devices", all_alpaca.len());
            }

            // Cache the results
            {
                let mut cache = get_discovery_cache().lock().await;
                *cache = Some(DiscoveryCache {
                    ascom_devices: all_ascom.clone(),
                    alpaca_devices: all_alpaca.clone(),
                    timestamp: Instant::now(),
                });
            }

            *in_progress = false;
            (all_ascom, all_alpaca)
        }
    };

    // Filter ASCOM devices by requested type
    for dev in ascom_devices {
        if dev.device_type == device_type {
            devices.push(dev);
        }
    }

    // Filter Alpaca devices by requested type
    for dev in alpaca_devices {
        if dev.device_type == device_type {
            devices.push(dev);
        }
    }

    // =====================================================
    // NATIVE DRIVER DISCOVERY (Cross-platform, vendor SDKs)
    // Native discovery has its own cache in nightshade_native
    // =====================================================
    {
        use nightshade_native::{discover_devices as native_discover, DeviceType as NativeDeviceType};

        let native_device_type = match device_type {
            DeviceType::Camera => Some(NativeDeviceType::Camera),
            DeviceType::Mount => Some(NativeDeviceType::Mount),
            DeviceType::Focuser => Some(NativeDeviceType::Focuser),
            DeviceType::FilterWheel => Some(NativeDeviceType::FilterWheel),
            DeviceType::Rotator => Some(NativeDeviceType::Rotator),
            _ => None,
        };

        if let Some(native_type) = native_device_type {
            if let Ok(native_devices) = native_discover(native_type).await {
                for native_dev in native_devices {
                    tracing::info!("Found native device: {} ({})", native_dev.display_name, native_dev.vendor.as_str());
                    devices.push(DeviceInfo {
                        id: native_dev.id,
                        name: native_dev.name.clone(),
                        device_type,
                        driver_type: DriverType::Native,
                        description: format!("{} native driver", native_dev.vendor.as_str()),
                        driver_version: native_dev.sdk_version.unwrap_or_else(|| "Native".to_string()),
                        serial_number: native_dev.serial_number,
                        unique_id: None,
                        display_name: native_dev.display_name,
                    });
                }
            }
        }
    }

    // =====================================================
    // INDI DISCOVERY (Cross-platform)
    // =====================================================
    {
        let indi_devices = get_device_manager().get_all_indi_devices().await;
        for dev in indi_devices {
            if dev.device_type == device_type {
                tracing::info!("Found INDI device: {}", dev.name);
                devices.push(dev);
            }
        }
    }

    // =====================================================
    // PHD2 DISCOVERY (Guider only)
    // =====================================================
    if device_type == DeviceType::Guider {
        let is_running = nightshade_imaging::is_phd2_running();
        let is_installed = nightshade_imaging::is_phd2_installed();

        if is_running || is_installed {
            tracing::info!("Found PHD2 Guiding (Running: {}, Installed: {})", is_running, is_installed);
            devices.push(DeviceInfo {
                id: "phd2_guider".to_string(),
                name: "PHD2 Guiding".to_string(),
                device_type: DeviceType::Guider,
                driver_type: DriverType::Native,
                description: if is_running { "PHD2 Guiding (Running)" } else { "PHD2 Guiding (Installed)" }.to_string(),
                driver_version: "PHD2".to_string(),
                serial_number: None,
                unique_id: None,
                display_name: "PHD2 Guiding".to_string(),
            });
        }
    }

    // =====================================================
    // SIMULATOR DISCOVERY (Always available for debugging)
    // =====================================================
    if device_type == DeviceType::Camera {
        devices.push(DeviceInfo {
            id: "sim_camera_1".to_string(),
            name: "Simulated Camera".to_string(),
            device_type: DeviceType::Camera,
            driver_type: DriverType::Simulator,
            description: "Internal Simulator".to_string(),
            driver_version: "1.0.0".to_string(),
            serial_number: Some("SIM-123".to_string()),
            unique_id: Some("sim_camera_1".to_string()),
            display_name: "Simulated Camera".to_string(),
        });
    }

    Ok(devices)
}

// =============================================================================
// Device Connection
// =============================================================================

/// Connect to a device
pub async fn api_connect_device(device_type: DeviceType, device_id: String) -> Result<(), NightshadeError> {
    tracing::info!("Connecting to {} device: {}", device_type.as_str(), device_id);
    
    tracing::info!("Connecting to {} device: {}", device_type.as_str(), device_id);
    
    // Special handling for PHD2 auto-launch
    if device_id == "phd2_guider" {
        if !nightshade_imaging::is_phd2_running() {
            tracing::info!("PHD2 not running, attempting to launch...");
            if let Err(e) = nightshade_imaging::launch_phd2() {
                tracing::error!("Failed to launch PHD2: {}", e);
                return Err(NightshadeError::ConnectionFailed(format!("Failed to launch PHD2: {}", e)));
            }
            
            // Wait for it to start
            tracing::info!("Waiting for PHD2 to start...");
            let mut started = false;
            for _ in 0..20 { // Wait up to 10 seconds
                tokio::time::sleep(std::time::Duration::from_millis(500)).await;
                if nightshade_imaging::is_phd2_running() {
                    started = true;
                    break;
                }
            }
            
            if !started {
                return Err(NightshadeError::ConnectionFailed("Timed out waiting for PHD2 to start".to_string()));
            }
        }
    }
    
    // Check if device is registered in DeviceManager, if not, discover and register it
    let device_manager = get_device_manager();
    
    // Check if device is already registered
    let is_registered = device_manager.is_device_registered(&device_id).await;
    
    // If not registered, discover and register the device
    if !is_registered {
        tracing::info!("Device {} not registered, discovering and registering...", device_id);
        
        // Discover devices of this type to find the one we want
        let discovered_devices = api_discover_devices(device_type.clone()).await?;
        
        // Find the device we're trying to connect to
        if let Some(device_info) = discovered_devices.iter().find(|d| d.id == device_id) {
            // Register the device before connecting
            device_manager.register_device(device_info.clone(), false).await;
            tracing::info!("Registered device: {} ({})", device_info.name, device_id);
        } else {
            return Err(NightshadeError::ConnectionFailed(
                format!("Device {} not found during discovery", device_id)
            ));
        }
    }
    
    // Use the DeviceManager to handle the connection
    device_manager.connect_device(&device_id).await
        .map_err(|e| NightshadeError::ConnectionFailed(e))
}

/// Disconnect from a device
pub async fn api_disconnect_device(device_type: DeviceType, device_id: String) -> Result<(), NightshadeError> {
    tracing::info!("Disconnecting from {} device: {}", device_type.as_str(), device_id);
    
    tracing::info!("Disconnecting from {} device: {}", device_type.as_str(), device_id);
    
    // Use the DeviceManager to handle disconnection
    get_device_manager().disconnect_device(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Check if a device is connected
pub async fn api_is_device_connected(device_type: DeviceType, device_id: String) -> bool {
    get_state().is_device_connected(device_type, &device_id).await
}

/// Get list of connected devices
pub async fn api_get_connected_devices() -> Vec<DeviceInfo> {
    let state = get_state();
    let mut devices = Vec::new();

    for device_type in [
        DeviceType::Camera,
        DeviceType::Mount,
        DeviceType::Focuser,
        DeviceType::FilterWheel,
        DeviceType::Guider,
        DeviceType::Rotator,
        DeviceType::Dome,
        DeviceType::Weather,
    ] {
        devices.extend(state.get_devices(device_type).await);
    }

    devices
}

// =============================================================================
// Device Heartbeat Monitoring
// =============================================================================

/// Start heartbeat monitoring for a device
///
/// This will poll the device status at the specified interval and emit
/// a Disconnected event if the device becomes unresponsive.
///
/// # Arguments
/// * `device_type` - The type of device to monitor
/// * `device_id` - The unique identifier for the device
/// * `interval_ms` - Heartbeat interval in milliseconds (recommended: 10000)
pub async fn api_start_device_heartbeat(
    device_type: DeviceType,
    device_id: String,
    interval_ms: u64,
) -> Result<(), NightshadeError> {
    tracing::info!(
        "Starting heartbeat monitoring for {} device: {} (interval: {}ms)",
        device_type.as_str(),
        device_id,
        interval_ms
    );

    get_device_manager()
        .start_heartbeat(&device_id, std::time::Duration::from_millis(interval_ms))
        .await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Stop heartbeat monitoring for a device
///
/// # Arguments
/// * `device_id` - The unique identifier for the device
pub async fn api_stop_device_heartbeat(device_id: String) -> Result<(), NightshadeError> {
    tracing::info!("Stopping heartbeat monitoring for device: {}", device_id);

    get_device_manager()
        .stop_heartbeat(&device_id)
        .await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Check device health status
///
/// Returns the last successful communication timestamp and whether
/// the device is currently responding to heartbeat checks.
///
/// # Arguments
/// * `device_id` - The unique identifier for the device
///
/// # Returns
/// A tuple of (last_successful_timestamp_ms, is_healthy)
pub async fn api_get_device_health(device_id: String) -> Result<(i64, bool), NightshadeError> {
    get_device_manager()
        .get_device_health(&device_id)
        .await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

// =============================================================================
// Camera Control (Simulator implementation for now)
// =============================================================================

/// Simulated camera state
static SIM_CAMERA: OnceLock<Arc<RwLock<SimulatedCamera>>> = OnceLock::new();

#[flutter_rust_bridge::frb]
pub struct SimulatedCamera {
    pub status: CameraStatus,
}

impl Default for SimulatedCamera {
    fn default() -> Self {
        Self {
            status: CameraStatus {
                connected: false,
                state: CameraState::Idle,
                sensor_temp: Some(20.0),
                cooler_power: Some(0.0),
                target_temp: Some(-10.0),
                cooler_on: false,
                gain: 100,
                offset: 10,
                bin_x: 1,
                bin_y: 1,
                sensor_width: 4144,
                sensor_height: 2822,
                pixel_size_x: 3.76,
                pixel_size_y: 3.76,
                max_adu: 65535,
                can_cool: true,
                can_set_gain: true,
                can_set_offset: true,
            },
        }
    }
}

fn get_sim_camera() -> &'static Arc<RwLock<SimulatedCamera>> {
    SIM_CAMERA.get_or_init(|| Arc::new(RwLock::new(SimulatedCamera::default())))
}

/// Get camera status
pub async fn api_get_camera_status(device_id: String) -> Result<CameraStatus, NightshadeError> {
    // Handle simulator devices with local simulated state
    if device_id.starts_with("sim_") {
        let camera = get_sim_camera().read().await;
        return Ok(camera.status.clone());
    }

    // Route real devices through the DeviceManager
    let mgr = get_device_manager();
    mgr.camera_get_status(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Set camera cooling target
pub async fn api_set_camera_cooler(device_id: String, enabled: u8, target_temp: Option<f64>) -> Result<(), NightshadeError> {
    // Handle simulator devices with local simulated state
    if device_id.starts_with("sim_") {
        let mut camera = get_sim_camera().write().await;
        camera.status.cooler_on = enabled != 0;
        if let Some(temp) = target_temp {
            camera.status.target_temp = Some(temp);
        }
        tracing::info!("Simulator camera cooler: enabled={}, target={:?}", enabled, target_temp);
        return Ok(());
    }

    // Route real devices through the DeviceManager
    tracing::info!("Setting camera cooler for {}: enabled={}, target={:?}", device_id, enabled, target_temp);
    let mgr = get_device_manager();
    mgr.camera_set_cooler(&device_id, enabled != 0, target_temp).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Set camera gain
pub async fn api_set_camera_gain(device_id: String, gain: i32) -> Result<(), NightshadeError> {
    // Handle simulator devices with local simulated state
    if device_id.starts_with("sim_") {
        let mut camera = get_sim_camera().write().await;
        camera.status.gain = gain;
        tracing::info!("Simulator camera gain set to: {}", gain);
        return Ok(());
    }

    // Route real devices through the DeviceManager
    tracing::info!("Setting camera gain for {}: {}", device_id, gain);
    let mgr = get_device_manager();
    mgr.camera_set_gain(&device_id, gain).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Set camera offset
pub async fn api_set_camera_offset(device_id: String, offset: i32) -> Result<(), NightshadeError> {
    // Handle simulator devices with local simulated state
    if device_id.starts_with("sim_") {
        let mut camera = get_sim_camera().write().await;
        camera.status.offset = offset;
        tracing::info!("Simulator camera offset set to: {}", offset);
        return Ok(());
    }

    // Route real devices through the DeviceManager
    tracing::info!("Setting camera offset for {}: {}", device_id, offset);
    let mgr = get_device_manager();
    mgr.camera_set_offset(&device_id, offset).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

// =============================================================================
// Camera Exposure Control (Real Cameras)
// =============================================================================

/// Start camera exposure
/// This delegates to api_camera_start_exposure which handles the full exposure
/// workflow including waiting for completion, image processing, and storage.
pub async fn start_exposure(
    device_id: String,
    duration_secs: f64,
    gain: i32,
    offset: i32,
    bin_x: i32,
    bin_y: i32,
) -> Result<(), NightshadeError> {
    tracing::info!("API: start_exposure called for {} duration={}", device_id, duration_secs);

    // Delegate to the full implementation which handles:
    // - Starting the exposure
    // - Publishing progress events
    // - Waiting for completion
    // - Downloading and processing the image
    // - Storing the result for get_last_image()
    api_camera_start_exposure(device_id, duration_secs, gain, offset, bin_x, bin_y).await
}

/// Abort/cancel camera exposure
pub async fn cancel_exposure(device_id: String) -> Result<(), NightshadeError> {
    tracing::info!("API: cancel_exposure called for {}", device_id);

    let mgr = get_device_manager();
    mgr.camera_abort_exposure(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))?;

    // Publish ExposureCancelled event
    let state = get_state();
    let event = crate::event::create_event(
        crate::event::EventSeverity::Info,
        crate::event::EventCategory::Imaging,
        crate::event::EventPayload::Imaging(crate::event::ImagingEvent::ExposureCancelled),
    );
    state.event_bus.publish(event);

    Ok(())
}

/// Download last image from camera
/// Returns the image stored by start_exposure/api_camera_start_exposure
pub async fn get_last_image() -> Result<Option<CapturedImageResult>, NightshadeError> {
    tracing::info!("API: get_last_image called");

    // Return the stored image from the last exposure
    let storage = get_last_image_storage().read().await;
    match &*storage {
        Some(image) => {
            tracing::info!("Returning stored image: {}x{}", image.width, image.height);
            Ok(Some(image.clone()))
        }
        None => {
            tracing::warn!("No image available - exposure may not have completed");
            Ok(None)
        }
    }
}

/// Get camera status
pub async fn get_camera_status(device_id: String) -> Result<CameraStatus, NightshadeError> {
    let mgr = get_device_manager();
    mgr.camera_get_status(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Set camera cooler
pub async fn set_camera_cooler(device_id: String, enabled: u8, target_temp: Option<f64>) -> Result<(), NightshadeError> {
    let mgr = get_device_manager();
    mgr.camera_set_cooler(&device_id, enabled != 0, target_temp).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

// =============================================================================
// Mount Control
// =============================================================================

/// Slew mount to coordinates
pub async fn mount_slew(device_id: String, ra: f64, dec: f64) -> Result<(), NightshadeError> {
    let mgr = get_device_manager();
    mgr.mount_slew(&device_id, ra, dec).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Sync mount to coordinates
pub async fn mount_sync(device_id: String, ra: f64, dec: f64) -> Result<(), NightshadeError> {
    let mgr = get_device_manager();
    mgr.mount_sync(&device_id, ra, dec).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Park mount
pub async fn mount_park(device_id: String) -> Result<(), NightshadeError> {
    let mgr = get_device_manager();
    mgr.mount_park(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Unpark mount
pub async fn mount_unpark(device_id: String) -> Result<(), NightshadeError> {
    let mgr = get_device_manager();
    mgr.mount_unpark(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Get mount coordinates
pub async fn mount_get_coordinates(device_id: String) -> Result<(f64, f64), NightshadeError> {
    let mgr = get_device_manager();
    mgr.mount_get_coordinates(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Abort mount slew
pub async fn mount_abort(device_id: String) -> Result<(), NightshadeError> {
    let mgr = get_device_manager();
    mgr.mount_abort(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Set mount tracking
pub async fn mount_set_tracking(device_id: String, enabled: u8) -> Result<(), NightshadeError> {
    let mgr = get_device_manager();
    mgr.mount_set_tracking(&device_id, enabled != 0).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Set mount tracking rate (0=Sidereal, 1=Lunar, 2=Solar, 3=King)
pub async fn mount_set_tracking_rate(device_id: String, rate: i32) -> Result<(), NightshadeError> {
    let mgr = get_device_manager();
    mgr.mount_set_tracking_rate(&device_id, rate).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Pulse guide mount
pub async fn mount_pulse_guide(device_id: String, direction: String, duration_ms: u32) -> Result<(), NightshadeError> {
    let mgr = get_device_manager();
    mgr.mount_pulse_guide(&device_id, direction, duration_ms).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Get mount status
pub async fn mount_get_status(device_id: String) -> Result<MountStatus, NightshadeError> {
    let mgr = get_device_manager();
    mgr.mount_get_status(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Get mount tracking rate (0=Sidereal, 1=Lunar, 2=Solar, 3=King)
pub async fn mount_get_tracking_rate(device_id: String) -> Result<i32, NightshadeError> {
    let mgr = get_device_manager();
    mgr.mount_get_tracking_rate(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Move mount axis at specified rate (degrees/second)
/// axis: 0=RA/Azimuth (primary), 1=Dec/Altitude (secondary)
/// rate: degrees per second (positive = N/E, negative = S/W), 0 to stop
pub async fn mount_move_axis(device_id: String, axis: i32, rate: f64) -> Result<(), NightshadeError> {
    let mgr = get_device_manager();
    mgr.mount_move_axis(&device_id, axis, rate).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

// =============================================================================
// Focuser Control
// =============================================================================

/// Move focuser to absolute position
pub async fn focuser_move_abs(device_id: String, position: i32) -> Result<(), NightshadeError> {
    let mgr = get_device_manager();
    mgr.focuser_move_abs(&device_id, position).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Move focuser relative
pub async fn focuser_move_rel(device_id: String, steps: i32) -> Result<(), NightshadeError> {
    let mgr = get_device_manager();
    mgr.focuser_move_rel(&device_id, steps).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Halt focuser
pub async fn focuser_halt(device_id: String) -> Result<(), NightshadeError> {
    let mgr = get_device_manager();
    mgr.focuser_halt(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Get focuser position
pub async fn focuser_get_position(device_id: String) -> Result<i32, NightshadeError> {
    let mgr = get_device_manager();
    mgr.focuser_get_position(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Get focuser temperature
pub async fn focuser_get_temp(device_id: String) -> Result<Option<f64>, NightshadeError> {
    let mgr = get_device_manager();
    mgr.focuser_get_temp(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Get focuser details (max pos, step size)
pub async fn focuser_get_details(device_id: String) -> Result<(i32, f64), NightshadeError> {
    let mgr = get_device_manager();
    mgr.focuser_get_details(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

// =============================================================================
// Filter Wheel Control
// =============================================================================

/// Set filter wheel position
pub async fn filter_wheel_set_position(device_id: String, position: i32) -> Result<(), NightshadeError> {
    if device_id.starts_with("sim_") {
        let mut fw = get_sim_filterwheel().write().await;
        fw.status.position = position;
        Ok(())
    } else {
        let mgr = get_device_manager();
        mgr.filter_wheel_set_position(&device_id, position).await
            .map_err(|e| NightshadeError::OperationFailed(e))
    }
}

/// Get filter wheel position
pub async fn filter_wheel_get_position(device_id: String) -> Result<i32, NightshadeError> {
    if device_id.starts_with("sim_") {
        let fw = get_sim_filterwheel().read().await;
        Ok(fw.status.position)
    } else {
        let mgr = get_device_manager();
        mgr.filter_wheel_get_position(&device_id).await
            .map_err(|e| NightshadeError::OperationFailed(e))
    }
}

/// Get filter wheel configuration (count, names)
pub async fn filter_wheel_get_config(device_id: String) -> Result<(i32, Vec<String>), NightshadeError> {
    if device_id.starts_with("sim_") {
        let fw = get_sim_filterwheel().read().await;
        Ok((fw.status.filter_count, fw.status.filter_names.clone()))
    } else {
        let mgr = get_device_manager();
        mgr.filter_wheel_get_config(&device_id).await
            .map_err(|e| NightshadeError::OperationFailed(e))
    }
}


/// Set camera gain
pub async fn set_camera_gain(device_id: String, gain: i32) -> Result<(), NightshadeError> {
    let mgr = get_device_manager();
    mgr.camera_set_gain(&device_id, gain).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Set camera offset
pub async fn set_camera_offset(device_id: String, offset: i32) -> Result<(), NightshadeError> {
    let mgr = get_device_manager();
    mgr.camera_set_offset(&device_id, offset).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

// =============================================================================
// Camera Binning (Legacy API - keeping for compatibility)
// =============================================================================


/// Set camera binning
pub async fn api_set_camera_binning(device_id: String, bin_x: i32, bin_y: i32) -> Result<(), NightshadeError> {
    if device_id.starts_with("sim_") {
        let mut camera = get_sim_camera().write().await;
        camera.status.bin_x = bin_x;
        camera.status.bin_y = bin_y;
        tracing::info!("Camera binning set to: {}x{}", bin_x, bin_y);
        Ok(())
    } else {
        // Real devices - binning is typically set as part of exposure parameters
        // For now, log and succeed since binning is usually applied at exposure time
        tracing::info!("Camera binning request: {}x{} for device {}", bin_x, bin_y, device_id);
        Ok(())
    }
}

// =============================================================================
// Dome Control
// =============================================================================

/// Get dome status
pub async fn api_get_dome_status(device_id: String) -> Result<DomeStatus, NightshadeError> {
    let mgr = get_device_manager();
    mgr.dome_get_status(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Open dome shutter
pub async fn api_dome_open_shutter(device_id: String) -> Result<(), NightshadeError> {
    let mgr = get_device_manager();
    mgr.dome_open_shutter(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Close dome shutter
pub async fn api_dome_close_shutter(device_id: String) -> Result<(), NightshadeError> {
    let mgr = get_device_manager();
    mgr.dome_close_shutter(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Slew dome to azimuth
pub async fn api_dome_slew_to_azimuth(device_id: String, azimuth: f64) -> Result<(), NightshadeError> {
    let mgr = get_device_manager();
    mgr.dome_slew_to_azimuth(&device_id, azimuth).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Park dome
pub async fn api_dome_park(device_id: String) -> Result<(), NightshadeError> {
    let mgr = get_device_manager();
    mgr.dome_park(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Get dome azimuth
pub async fn api_dome_get_azimuth(device_id: String) -> Result<f64, NightshadeError> {
    let mgr = get_device_manager();
    mgr.dome_get_azimuth(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Get dome shutter status
pub async fn api_dome_get_shutter_status(device_id: String) -> Result<i32, NightshadeError> {
    let mgr = get_device_manager();
    mgr.dome_get_shutter_status(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Check if dome is slewing
pub async fn api_dome_is_slewing(device_id: String) -> Result<bool, NightshadeError> {
    let mgr = get_device_manager();
    mgr.dome_is_slewing(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

// =============================================================================
// Cover Calibrator Control (Flat Panel / Dust Cover)
// =============================================================================

/// Open cover calibrator dust cover
pub async fn api_cover_calibrator_open_cover(device_id: String) -> Result<(), NightshadeError> {
    let mgr = get_device_manager();
    mgr.cover_calibrator_open_cover(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Close cover calibrator dust cover
pub async fn api_cover_calibrator_close_cover(device_id: String) -> Result<(), NightshadeError> {
    let mgr = get_device_manager();
    mgr.cover_calibrator_close_cover(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Halt cover calibrator cover movement
pub async fn api_cover_calibrator_halt_cover(device_id: String) -> Result<(), NightshadeError> {
    let mgr = get_device_manager();
    mgr.cover_calibrator_halt_cover(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Turn on cover calibrator light at specified brightness
pub async fn api_cover_calibrator_calibrator_on(device_id: String, brightness: i32) -> Result<(), NightshadeError> {
    let mgr = get_device_manager();
    mgr.cover_calibrator_calibrator_on(&device_id, brightness).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Turn off cover calibrator light
pub async fn api_cover_calibrator_calibrator_off(device_id: String) -> Result<(), NightshadeError> {
    let mgr = get_device_manager();
    mgr.cover_calibrator_calibrator_off(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Get cover calibrator cover state (0=NotPresent, 1=Closed, 2=Moving, 3=Open, 4=Unknown, 5=Error)
pub async fn api_cover_calibrator_get_cover_state(device_id: String) -> Result<i32, NightshadeError> {
    let mgr = get_device_manager();
    mgr.cover_calibrator_get_cover_state(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Get cover calibrator calibrator state (0=NotPresent, 1=Off, 2=NotReady, 3=Ready, 4=Unknown, 5=Error)
pub async fn api_cover_calibrator_get_calibrator_state(device_id: String) -> Result<i32, NightshadeError> {
    let mgr = get_device_manager();
    mgr.cover_calibrator_get_calibrator_state(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Get cover calibrator current brightness
pub async fn api_cover_calibrator_get_brightness(device_id: String) -> Result<i32, NightshadeError> {
    let mgr = get_device_manager();
    mgr.cover_calibrator_get_brightness(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Get cover calibrator maximum brightness
pub async fn api_cover_calibrator_get_max_brightness(device_id: String) -> Result<i32, NightshadeError> {
    let mgr = get_device_manager();
    mgr.cover_calibrator_get_max_brightness(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Get cover calibrator full status
pub async fn api_cover_calibrator_get_status(device_id: String) -> Result<crate::device::CoverCalibratorStatus, NightshadeError> {
    let mgr = get_device_manager();
    mgr.cover_calibrator_get_status(&device_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

// =============================================================================
// Mount Control (Simulator implementation for now)
// =============================================================================

/// Simulated mount state
static SIM_MOUNT: OnceLock<Arc<RwLock<SimulatedMount>>> = OnceLock::new();

#[flutter_rust_bridge::frb]
pub struct SimulatedMount {
    pub status: MountStatus,
}

impl Default for SimulatedMount {
    fn default() -> Self {
        Self {
            status: MountStatus {
                connected: false,
                tracking: false,
                slewing: false,
                parked: true,
                at_home: false,
                side_of_pier: PierSide::Unknown,
                right_ascension: 0.0,
                declination: 0.0,
                altitude: 0.0,
                azimuth: 0.0,
                sidereal_time: 0.0,
                tracking_rate: TrackingRate::Sidereal,
                can_park: true,
                can_slew: true,
                can_sync: true,
                can_pulse_guide: true,
                can_set_tracking_rate: true,
            },
        }
    }
}

fn get_sim_mount() -> &'static Arc<RwLock<SimulatedMount>> {
    SIM_MOUNT.get_or_init(|| Arc::new(RwLock::new(SimulatedMount::default())))
}

/// Get mount status
pub async fn api_get_mount_status(device_id: String) -> Result<MountStatus, NightshadeError> {
    if device_id.starts_with("sim_") {
        let mount = get_sim_mount().read().await;
        Ok(mount.status.clone())
    } else {
        // Route real devices through DeviceManager
        let mgr = get_device_manager();
        mgr.mount_get_status(&device_id).await
            .map_err(|e| NightshadeError::OperationFailed(e))
    }
}

/// Slew mount to coordinates
pub async fn api_mount_slew_to_coordinates(device_id: String, ra: f64, dec: f64) -> Result<(), NightshadeError> {
    if device_id.starts_with("sim_") {
        tracing::info!("Slewing to RA: {:.4}h, Dec: {:.4}°", ra, dec);

        {
            let mut mount = get_sim_mount().write().await;
            mount.status.slewing = true;
        }

        // Simulate slew time
        tokio::time::sleep(tokio::time::Duration::from_secs(2)).await;

        {
            let mut mount = get_sim_mount().write().await;
            mount.status.slewing = false;
            mount.status.right_ascension = ra;
            mount.status.declination = dec;
            mount.status.parked = false;
        }

        tracing::info!("Slew complete");
        Ok(())
    } else {
        // Route real devices through DeviceManager
        let mgr = get_device_manager();
        mgr.mount_slew(&device_id, ra, dec).await
            .map_err(|e| NightshadeError::OperationFailed(e))
    }
}

/// Sync mount to coordinates
pub async fn api_mount_sync_to_coordinates(device_id: String, ra: f64, dec: f64) -> Result<(), NightshadeError> {
    if device_id.starts_with("sim_") {
        tracing::info!("Syncing to RA: {:.4}h, Dec: {:.4}°", ra, dec);

        let mut mount = get_sim_mount().write().await;
        mount.status.right_ascension = ra;
        mount.status.declination = dec;

        Ok(())
    } else {
        // Route real devices through DeviceManager
        let mgr = get_device_manager();
        mgr.mount_sync(&device_id, ra, dec).await
            .map_err(|e| NightshadeError::OperationFailed(e))
    }
}

/// Park the mount
pub async fn api_mount_park(device_id: String) -> Result<(), NightshadeError> {
    if device_id.starts_with("sim_") {
        tracing::info!("Parking mount");

        {
            let mut mount = get_sim_mount().write().await;
            mount.status.slewing = true;
        }

        // Simulate park time
        tokio::time::sleep(tokio::time::Duration::from_secs(2)).await;

        {
            let mut mount = get_sim_mount().write().await;
            mount.status.slewing = false;
            mount.status.parked = true;
            mount.status.tracking = false;
        }

        tracing::info!("Mount parked");
        Ok(())
    } else {
        // Route real devices through DeviceManager
        let mgr = get_device_manager();
        mgr.mount_park(&device_id).await
            .map_err(|e| NightshadeError::OperationFailed(e))
    }
}

/// Unpark the mount
pub async fn api_mount_unpark(device_id: String) -> Result<(), NightshadeError> {
    if device_id.starts_with("sim_") {
        let mut mount = get_sim_mount().write().await;
        mount.status.parked = false;

        tracing::info!("Mount unparked");
        Ok(())
    } else {
        // Route real devices through DeviceManager
        let mgr = get_device_manager();
        mgr.mount_unpark(&device_id).await
            .map_err(|e| NightshadeError::OperationFailed(e))
    }
}

/// Set mount tracking
pub async fn api_mount_set_tracking(device_id: String, enabled: u8) -> Result<(), NightshadeError> {
    if device_id.starts_with("sim_") {
        let mut mount = get_sim_mount().write().await;
        mount.status.tracking = enabled != 0;

        tracing::info!("Mount tracking: {}", enabled);
        Ok(())
    } else {
        // Route real devices through DeviceManager
        let mgr = get_device_manager();
        mgr.mount_set_tracking(&device_id, enabled != 0).await
            .map_err(|e| NightshadeError::OperationFailed(e))
    }
}

/// Pulse guide the mount in a direction for a duration
pub async fn api_mount_pulse_guide(device_id: String, direction: String, duration_ms: i32) -> Result<(), NightshadeError> {
    tracing::info!("Pulse guiding {} for {}ms in direction {}", device_id, duration_ms, direction);

    // Validate direction
    match direction.to_lowercase().as_str() {
        "north" | "n" | "south" | "s" | "east" | "e" | "west" | "w" => {},
        _ => return Err(NightshadeError::InvalidParameter(format!("Unknown direction: {}", direction))),
    };

    // For simulator, just wait the duration
    if device_id.starts_with("sim_") {
        tokio::time::sleep(std::time::Duration::from_millis(duration_ms as u64)).await;
        tracing::info!("Pulse guide complete");
        return Ok(());
    }

    // Route real devices through DeviceManager
    let mgr = get_device_manager();
    mgr.mount_pulse_guide(&device_id, direction, duration_ms as u32).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

// =============================================================================
// Focuser Control (Simulator implementation for now)
// =============================================================================

/// Simulated focuser state
static SIM_FOCUSER: OnceLock<Arc<RwLock<SimulatedFocuser>>> = OnceLock::new();

#[flutter_rust_bridge::frb]
pub struct SimulatedFocuser {
    pub status: FocuserStatus,
}

impl Default for SimulatedFocuser {
    fn default() -> Self {
        Self {
            status: FocuserStatus {
                connected: false,
                position: 25000,
                moving: false,
                temperature: Some(20.0),
                max_position: 50000,
                step_size: 1.0,
                is_absolute: true,
                has_temperature: true,
            },
        }
    }
}

#[flutter_rust_bridge::frb(ignore)]
pub fn get_sim_focuser() -> &'static Arc<RwLock<SimulatedFocuser>> {
    SIM_FOCUSER.get_or_init(|| Arc::new(RwLock::new(SimulatedFocuser::default())))
}

/// Get focuser status
pub async fn api_get_focuser_status(device_id: String) -> Result<FocuserStatus, NightshadeError> {
    if device_id.starts_with("sim_") {
        let focuser = get_sim_focuser().read().await;
        Ok(focuser.status.clone())
    } else {
        // Route real devices through DeviceManager
        let mgr = get_device_manager();

        // Get all focuser status components
        let position = mgr.focuser_get_position(&device_id).await
            .map_err(|e| NightshadeError::OperationFailed(e))?;
        let moving = mgr.focuser_is_moving(&device_id).await
            .unwrap_or(false);
        let temperature = mgr.focuser_get_temp(&device_id).await
            .unwrap_or(None);
        let (max_position, step_size) = match mgr.focuser_get_details(&device_id).await {
            Ok(details) => details,
            Err(e) => {
                tracing::warn!("Failed to get focuser details for {}: {:?}, using defaults", device_id, e);
                (100000, 1.0)
            }
        };

        Ok(FocuserStatus {
            connected: true,
            position,
            moving,
            temperature,
            max_position,
            step_size,
            is_absolute: true,
            has_temperature: temperature.is_some(),
        })
    }
}

/// Move focuser to position
pub async fn api_focuser_move_to(device_id: String, position: i32) -> Result<(), NightshadeError> {
    if device_id.starts_with("sim_") {
        tracing::info!("Moving simulator focuser to position: {}", position);
        
        {
            let mut focuser = get_sim_focuser().write().await;
            focuser.status.moving = true;
        }
        
        // Simulate move time based on distance
        let current_pos = {
            let focuser = get_sim_focuser().read().await;
            focuser.status.position
        };
        let distance = (position - current_pos).abs();
        let move_time = (distance as f64 / 1000.0).max(0.5);
        
        tokio::time::sleep(tokio::time::Duration::from_secs_f64(move_time)).await;
        
        {
            let mut focuser = get_sim_focuser().write().await;
            focuser.status.moving = false;
            focuser.status.position = position;
        }
        
        tracing::info!("Focuser move complete");
        Ok(())
    } else {
        // Real device - use DeviceManager for proper driver routing
        let mgr = get_device_manager();
        mgr.focuser_move_abs(&device_id, position).await
            .map_err(|e| NightshadeError::OperationFailed(e))
    }
}

/// Move focuser by relative amount
pub async fn api_focuser_move_relative(device_id: String, delta: i32) -> Result<(), NightshadeError> {
    if device_id.starts_with("sim_") {
        let current_pos = {
            let focuser = get_sim_focuser().read().await;
            focuser.status.position
        };
        api_focuser_move_to(device_id, current_pos + delta).await
    } else {
        // Route real devices through DeviceManager
        let mgr = get_device_manager();
        mgr.focuser_move_rel(&device_id, delta).await
            .map_err(|e| NightshadeError::OperationFailed(e))
    }
}

/// Halt focuser
pub async fn api_focuser_halt(device_id: String) -> Result<(), NightshadeError> {
    if device_id.starts_with("sim_") {
        // For simulator, just stop moving
        let mut focuser = get_sim_focuser().write().await;
        focuser.status.moving = false;
        Ok(())
    } else {
        // Route real devices through DeviceManager
        let mgr = get_device_manager();
        mgr.focuser_halt(&device_id).await
            .map_err(|e| NightshadeError::OperationFailed(e))
    }
}

// =============================================================================
// Filter Wheel Control (Simulator implementation for now)
// =============================================================================

/// Simulated filter wheel state
static SIM_FILTERWHEEL: OnceLock<Arc<RwLock<SimulatedFilterWheel>>> = OnceLock::new();

#[flutter_rust_bridge::frb]
pub struct SimulatedFilterWheel {
    pub status: FilterWheelStatus,
}

impl Default for SimulatedFilterWheel {
    fn default() -> Self {
        Self {
            status: FilterWheelStatus {
                connected: false,
                position: 1,
                moving: false,
                filter_count: 7,
                filter_names: vec![
                    "L".to_string(),
                    "R".to_string(),
                    "G".to_string(),
                    "B".to_string(),
                    "Ha".to_string(),
                    "OIII".to_string(),
                    "SII".to_string(),
                ],
            },
        }
    }
}

fn get_sim_filterwheel() -> &'static Arc<RwLock<SimulatedFilterWheel>> {
    SIM_FILTERWHEEL.get_or_init(|| Arc::new(RwLock::new(SimulatedFilterWheel::default())))
}

/// Get filter wheel status
pub async fn api_get_filterwheel_status(device_id: String) -> Result<FilterWheelStatus, NightshadeError> {
    if device_id.starts_with("sim_") {
        let fw = get_sim_filterwheel().read().await;
        Ok(fw.status.clone())
    } else {
        // Real device - use DeviceManager for proper driver routing
        let mgr = get_device_manager();
        let position = mgr.filter_wheel_get_position(&device_id).await
            .map_err(|e| NightshadeError::OperationFailed(e))?;
        let is_moving = mgr.filter_wheel_is_moving(&device_id).await
            .unwrap_or(false);
        let (filter_count, filter_names) = mgr.filter_wheel_get_config(&device_id).await
            .map_err(|e| NightshadeError::OperationFailed(e))?;

        Ok(FilterWheelStatus {
            connected: true,
            position,
            moving: is_moving,
            filter_count,
            filter_names,
        })
    }
}

/// Set filter wheel position
pub async fn api_filterwheel_set_position(device_id: String, position: i32) -> Result<(), NightshadeError> {
    if device_id.starts_with("sim_") {
        let mut fw = get_sim_filterwheel().write().await;

        // Simulate move
        fw.status.moving = true;
        fw.status.position = -1; // Unknown while moving

        // Instant move for sim
        fw.status.moving = false;
        fw.status.position = position;

        Ok(())
    } else {
        // Real device - use DeviceManager for proper driver routing
        let mgr = get_device_manager();
        mgr.filter_wheel_set_position(&device_id, position).await
            .map_err(|e| NightshadeError::OperationFailed(e))
    }
}

/// Get filter names
pub async fn api_filterwheel_get_names(device_id: String) -> Result<Vec<String>, NightshadeError> {
    if device_id.starts_with("sim_") {
        let fw = get_sim_filterwheel().read().await;
        Ok(fw.status.filter_names.clone())
    } else {
        // Real device - use DeviceManager for proper driver routing
        let mgr = get_device_manager();
        let (_, filter_names) = mgr.filter_wheel_get_config(&device_id).await
            .map_err(|e| NightshadeError::OperationFailed(e))?;
        Ok(filter_names)
    }
}

/// Set filter by name
pub async fn api_filterwheel_set_by_name(device_id: String, name: String) -> Result<(), NightshadeError> {
    if device_id.starts_with("sim_") {
        let position = {
            let fw = get_sim_filterwheel().read().await;
            fw.status.filter_names.iter().position(|n| n == &name)
        };

        if let Some(pos) = position {
            api_filterwheel_set_position(device_id, (pos + 1) as i32).await
        } else {
            Err(NightshadeError::OperationFailed(format!("Filter {} not found", name)))
        }
    } else {
        // Real device - find position by name and use DeviceManager
        let mgr = get_device_manager();
        let (_, filter_names) = mgr.filter_wheel_get_config(&device_id).await
            .map_err(|e| NightshadeError::OperationFailed(e))?;

        let position = filter_names.iter()
            .position(|n| n.eq_ignore_ascii_case(&name))
            .ok_or_else(|| NightshadeError::OperationFailed(format!("Filter '{}' not found", name)))?;

        // Filter positions are 1-indexed in ASCOM/Alpaca
        mgr.filter_wheel_set_position(&device_id, (position + 1) as i32).await
            .map_err(|e| NightshadeError::OperationFailed(e))
    }
}

// =============================================================================
// Rotator Control (Simulator implementation for now)
// =============================================================================

/// Simulated rotator state
static SIM_ROTATOR: OnceLock<Arc<RwLock<SimulatedRotator>>> = OnceLock::new();

#[flutter_rust_bridge::frb]
pub struct SimulatedRotator {
    pub status: RotatorStatus,
}

impl Default for SimulatedRotator {
    fn default() -> Self {
        Self {
            status: RotatorStatus {
                connected: false,
                position: 0.0,
                moving: false,
                mechanical_position: 0.0,
                is_moving: false,
                can_reverse: true,
            },
        }
    }
}

#[flutter_rust_bridge::frb(ignore)]
pub fn get_sim_rotator() -> &'static Arc<RwLock<SimulatedRotator>> {
    SIM_ROTATOR.get_or_init(|| Arc::new(RwLock::new(SimulatedRotator::default())))
}

/// Get rotator status
pub async fn api_get_rotator_status(device_id: String) -> Result<RotatorStatus, NightshadeError> {
    if device_id.starts_with("sim_") {
        let rotator = get_sim_rotator().read().await;
        Ok(rotator.status.clone())
    } else {
        // Route real devices through DeviceManager
        let mgr = get_device_manager();

        let position = mgr.rotator_get_position(&device_id).await
            .map_err(|e| NightshadeError::OperationFailed(e))?;
        let is_moving = mgr.rotator_is_moving(&device_id).await
            .unwrap_or(false);

        Ok(RotatorStatus {
            connected: true,
            position,
            moving: is_moving,
            mechanical_position: position,
            is_moving,
            can_reverse: true, // Could query from device if supported
        })
    }
}

/// Move rotator to angle
pub async fn api_rotator_move_to(device_id: String, angle: f64) -> Result<(), NightshadeError> {
    if device_id.starts_with("sim_") {
        tracing::info!("Moving simulator rotator to {}°", angle);

        {
            let mut rotator = get_sim_rotator().write().await;
            rotator.status.moving = true;
            rotator.status.is_moving = true;
        }

        // Simulate move time
        tokio::time::sleep(tokio::time::Duration::from_secs(2)).await;

        {
            let mut rotator = get_sim_rotator().write().await;
            rotator.status.moving = false;
            rotator.status.is_moving = false;
            rotator.status.position = angle;
            rotator.status.mechanical_position = angle;
        }

        Ok(())
    } else {
        // Real device - use DeviceManager for proper driver routing
        let mgr = get_device_manager();
        mgr.rotator_move_absolute(&device_id, angle).await
            .map_err(|e| NightshadeError::OperationFailed(e))
    }
}

/// Move rotator relative
pub async fn api_rotator_move_relative(device_id: String, delta: f64) -> Result<(), NightshadeError> {
    if device_id.starts_with("sim_") {
        let current = {
            let rotator = get_sim_rotator().read().await;
            rotator.status.position
        };
        let target = (current + delta) % 360.0;
        let target = if target < 0.0 { target + 360.0 } else { target };

        api_rotator_move_to(device_id, target).await
    } else {
        // Real device - calculate target angle and use DeviceManager
        let mgr = get_device_manager();
        let current = mgr.rotator_get_position(&device_id).await
            .map_err(|e| NightshadeError::OperationFailed(e))?;
        let target = (current + delta) % 360.0;
        let target = if target < 0.0 { target + 360.0 } else { target };
        mgr.rotator_move_absolute(&device_id, target).await
            .map_err(|e| NightshadeError::OperationFailed(e))
    }
}

/// Halt rotator
pub async fn api_rotator_halt(device_id: String) -> Result<(), NightshadeError> {
    if device_id.starts_with("sim_") {
        // For simulator, just stop moving
        let mut rotator = get_sim_rotator().write().await;
        rotator.status.moving = false;
        rotator.status.is_moving = false;
        Ok(())
    } else {
        // Route real devices through DeviceManager
        let mgr = get_device_manager();
        mgr.rotator_halt(&device_id).await
            .map_err(|e| NightshadeError::OperationFailed(e))
    }
}

// =============================================================================
// Camera Exposure & Image Capture
// =============================================================================

/// Global cancellation token for autofocus
static AUTOFOCUS_CANCEL_TOKEN: OnceLock<Arc<AtomicBool>> = OnceLock::new();

fn get_autofocus_cancel_token() -> &'static Arc<AtomicBool> {
    // In a real app, we might want to support multiple concurrent operations,
    // but for now we use a single global token that we replace on each run.
    // Note: OnceLock is immutable once set. We need a Mutex or RwLock if we want to swap tokens,
    // OR we assume the token is persistent and we just reset its value.
    // Resetting value is safer for OnceLock.
    AUTOFOCUS_CANCEL_TOKEN.get_or_init(|| Arc::new(AtomicBool::new(false)))
}

/// Autofocus configuration for API
#[derive(Debug, Clone)]
pub struct AutofocusConfigApi {
    pub exposure_time: f64,
    pub step_size: i32,
    pub steps_out: i32,
    pub method: String, // "VCurve", "Hyperbolic", "Parabolic"
    pub binning: i32,
}

/// A single focus data point (position and HFR)
#[derive(Debug, Clone)]
pub struct FocusDataPoint {
    pub position: i32,
    pub hfr: f64,
    pub fwhm: Option<f64>,
    pub star_count: u32,
}

/// Autofocus result containing all data for display and analysis
#[derive(Debug, Clone)]
pub struct AutofocusResultApi {
    pub best_position: i32,
    pub best_hfr: f64,
    pub focus_data: Vec<FocusDataPoint>,
    pub method: String,
    pub temperature: Option<f64>,
    pub timestamp: i64,
    pub curve_fit_quality: f64,
    pub backlash_applied: bool,
}

/// Run autofocus
pub async fn api_run_autofocus(
    device_id: String, // Focuser ID
    camera_id: String,
    config: AutofocusConfigApi
) -> Result<AutofocusResultApi, NightshadeError> {
    tracing::info!("Starting autofocus with camera {} and focuser {}", camera_id, device_id);

    use nightshade_sequencer::instructions::{execute_autofocus, InstructionContext};
    use nightshade_sequencer::{AutofocusConfig, AutofocusMethod, Binning, NodeStatus};
    use crate::real_device_ops::RealDeviceOps;
    use std::time::{SystemTime, UNIX_EPOCH};

    // Reset cancellation token
    let cancel_token = get_autofocus_cancel_token();
    cancel_token.store(false, Ordering::Relaxed);

    // Store the method string for result
    let method_str = config.method.clone();

    // Map method string to enum
    let method = match config.method.as_str() {
        "Hyperbolic" => AutofocusMethod::Hyperbolic,
        "Parabolic" => AutofocusMethod::Quadratic,
        _ => AutofocusMethod::VCurve,
    };

    // Map binning
    let binning = match config.binning {
        2 => Binning::Two,
        3 => Binning::Three,
        4 => Binning::Four,
        _ => Binning::One,
    };

    let af_config = AutofocusConfig {
        exposure_duration: config.exposure_time,
        step_size: config.step_size,
        steps_out: config.steps_out as u32,
        method,
        binning,
        filter: None, // Optional: add filter support
    };

    // Create context - use UnifiedDeviceOps which routes through DeviceManager
    let device_ops = create_unified_device_ops();

    // Try to get focuser temperature before autofocus
    let temperature = device_ops.focuser_get_temperature(&device_id).await.ok().flatten();

    let ctx = InstructionContext {
        target_ra: None,
        target_dec: None,
        target_name: None,
        current_filter: None,
        current_binning: Binning::One,
        cancellation_token: cancel_token.clone(),
        camera_id: Some(camera_id),
        mount_id: None,
        focuser_id: Some(device_id),
        filterwheel_id: None,
        dome_id: None,
        rotator_id: None,
        cover_calibrator_id: None,
        save_path: None,
        latitude: None,
        longitude: None,
        device_ops,
        trigger_state: None,
    };

    // Execute (no progress callback when called directly from API)
    let result = execute_autofocus(&af_config, &ctx, None).await;

    // Get current timestamp
    let timestamp = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .map(|d| d.as_secs() as i64)
        .unwrap_or(0);

    match result.status {
        NodeStatus::Success => {
            // Extract AutofocusResult from serde_json::Value
            if let Some(data) = result.data {
                // Try to deserialize as AutofocusResult
                if let Ok(af_result) = serde_json::from_value::<nightshade_sequencer::AutofocusResult>(data) {
                    // Convert to API result
                    let focus_data: Vec<FocusDataPoint> = af_result.data_points.iter().map(|dp| {
                        FocusDataPoint {
                            position: dp.position,
                            hfr: dp.hfr,
                            fwhm: dp.fwhm,
                            star_count: dp.star_count,
                        }
                    }).collect();

                    return Ok(AutofocusResultApi {
                        best_position: af_result.best_position,
                        best_hfr: af_result.best_hfr,
                        focus_data,
                        method: method_str,
                        temperature: af_result.temperature_celsius,
                        timestamp,
                        curve_fit_quality: af_result.curve_fit_quality,
                        backlash_applied: af_result.backlash_applied,
                    });
                }
            }

            // Fallback if no data or deserialization failed
            Ok(AutofocusResultApi {
                best_position: 0,
                best_hfr: 0.0,
                focus_data: vec![],
                method: method_str,
                temperature,
                timestamp,
                curve_fit_quality: 0.0,
                backlash_applied: false,
            })
        },
        NodeStatus::Failure => {
            Err(NightshadeError::OperationFailed(result.message.unwrap_or("Autofocus failed".to_string())))
        },
        NodeStatus::Cancelled => {
            Err(NightshadeError::Cancelled)
        },
        _ => Err(NightshadeError::OperationFailed("Unknown error".to_string())),
    }
}

/// Cancel autofocus
pub async fn api_cancel_autofocus() -> Result<(), NightshadeError> {
    tracing::info!("Cancelling autofocus...");
    let cancel_token = get_autofocus_cancel_token();
    cancel_token.store(true, Ordering::Relaxed);
    Ok(())
}

// =============================================================================
// Camera Exposure & Image Capture
// =============================================================================

/// Captured image result containing display-ready data
#[derive(Debug, Clone)]
pub struct CapturedImageResult {
    pub width: u32,
    pub height: u32,
    pub display_data: Vec<u8>,  // RGB (width*height*3) if is_color=true, grayscale (width*height) if is_color=false
    pub histogram: Vec<u32>,    // 256-bin histogram
    pub stats: ImageStatsResult,
    pub exposure_time: f64,
    pub timestamp: String,
    pub is_color: bool,  // true if display_data is RGB, false if grayscale
}

/// Image statistics
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ImageStatsResult {
    pub min: f64,
    pub max: f64,
    pub mean: f64,
    pub median: f64,
    pub std_dev: f64,
    pub hfr: Option<f64>,
    pub star_count: u32,
}

/// Last captured image storage
static LAST_CAPTURED_IMAGE: OnceLock<Arc<RwLock<Option<CapturedImageResult>>>> = OnceLock::new();

fn get_last_image_storage() -> &'static Arc<RwLock<Option<CapturedImageResult>>> {
    LAST_CAPTURED_IMAGE.get_or_init(|| Arc::new(RwLock::new(None)))
}

/// Raw image data storage (u16 data for FITS saving)
static LAST_RAW_IMAGE_DATA: OnceLock<Arc<RwLock<Option<Vec<u16>>>>> = OnceLock::new();

fn get_last_raw_data_storage() -> &'static Arc<RwLock<Option<Vec<u16>>>> {
    LAST_RAW_IMAGE_DATA.get_or_init(|| Arc::new(RwLock::new(None)))
}

/// Raw image info with metadata - used by sequencer for actual image analysis
/// This preserves the original 16-bit sensor data needed for HFR calculation, plate solving, etc.
#[derive(Debug, Clone)]
pub struct RawImageInfo {
    pub width: u32,
    pub height: u32,
    pub data: Vec<u16>,              // Raw 16-bit sensor data
    pub sensor_type: Option<String>, // "Monochrome" or "Color"
    pub bayer_offset: Option<(i32, i32)>, // Bayer pattern offset for color sensors
}

/// Raw image info storage
static LAST_RAW_IMAGE_INFO: OnceLock<Arc<RwLock<Option<RawImageInfo>>>> = OnceLock::new();

fn get_last_raw_image_info_storage() -> &'static Arc<RwLock<Option<RawImageInfo>>> {
    LAST_RAW_IMAGE_INFO.get_or_init(|| Arc::new(RwLock::new(None)))
}

/// Start a camera exposure
/// Returns progress updates via events, final image available via api_get_last_image
pub async fn api_camera_start_exposure(
    device_id: String,
    duration_secs: f64,
    gain: i32,
    offset: i32,
    bin_x: i32,
    bin_y: i32,
) -> Result<(), NightshadeError> {
    tracing::info!("Starting {}s exposure with gain={}, offset={}, bin={}x{}", 
        duration_secs, gain, offset, bin_x, bin_y);
    
    // Check if simulator or real device
    if device_id.starts_with("sim_") {
        // Simulator path (existing code)
        // Update camera state to exposing
        {
            let mut camera = get_sim_camera().write().await;
            camera.status.state = CameraState::Exposing;
        }
        
        // Publish exposure started event
        get_state().publish_imaging_event(
            ImagingEvent::ExposureStarted {
                duration_secs,
                frame_type: crate::device::FrameType::Light,
            },
            EventSeverity::Info,
        );
        
        // Simulate exposure with progress updates
        let start_time = std::time::Instant::now();
        let duration = std::time::Duration::from_secs_f64(duration_secs);
        
        while start_time.elapsed() < duration {
            let progress = start_time.elapsed().as_secs_f64() / duration_secs;
            get_state().publish_imaging_event(
                ImagingEvent::ExposureProgress {
                    progress,
                    remaining_secs: duration_secs - start_time.elapsed().as_secs_f64(),
                },
                EventSeverity::Info,
            );
            tokio::time::sleep(tokio::time::Duration::from_millis(100)).await;
        }
        
        // Update camera state to reading
        {
            let mut camera = get_sim_camera().write().await;
            camera.status.state = CameraState::Reading;
        }
        
        // Generate simulated image
        let sensor_width = 4144 / bin_x as u32;
        let sensor_height = 2822 / bin_y as u32;

        let (raw_data, display_data, histogram, stats, star_count) =
            generate_simulated_image(sensor_width, sensor_height, gain, duration_secs);

        // Store the raw image data (legacy)
        {
            let mut raw_storage = get_last_raw_data_storage().write().await;
            *raw_storage = Some(raw_data.clone());
        }

        // Store the raw image info with metadata (used by sequencer)
        {
            let mut raw_info_storage = get_last_raw_image_info_storage().write().await;
            *raw_info_storage = Some(RawImageInfo {
                width: sensor_width,
                height: sensor_height,
                data: raw_data,
                sensor_type: Some("Monochrome".to_string()), // Simulated camera is mono
                bayer_offset: None,
            });
        }

        // Store the captured image
        {
            let mut storage = get_last_image_storage().write().await;
            *storage = Some(CapturedImageResult {
                width: sensor_width,
                height: sensor_height,
                display_data,
                histogram,
                stats: ImageStatsResult {
                    min: stats.min,
                    max: stats.max,
                    mean: stats.mean,
                    median: stats.median,
                    std_dev: stats.std_dev,
                    hfr: Some(2.5 + (rand::random::<f64>() - 0.5) * 0.5), // Simulated HFR
                    star_count,
                },
                exposure_time: duration_secs,
                timestamp: chrono::Utc::now().format("%Y-%m-%dT%H:%M:%S").to_string(),
                is_color: false,  // Simulated images are grayscale
            });
        }
        
        // Update camera state back to idle
        {
            let mut camera = get_sim_camera().write().await;
            camera.status.state = CameraState::Idle;
        }
        
        // Publish exposure complete event
        get_state().publish_imaging_event(
            ImagingEvent::ExposureComplete {
                success: true,
            },
            EventSeverity::Info,
        );
        
        tracing::info!("Exposure complete");
        Ok(())
    } else {
        // Real camera path - use UnifiedDeviceOps which routes through DeviceManager
        // Events (ExposureStarted, ExposureProgress, ExposureComplete) are published by UnifiedDeviceOps
        let device_ops = create_unified_device_ops();

        // Start exposure and get raw data (blocks until complete, events published by UnifiedDeviceOps)
        let seq_image = device_ops.camera_start_exposure(
            &device_id,
            duration_secs,
            Some(gain),
            Some(offset),
            bin_x,
            bin_y,
        ).await.map_err(|e| NightshadeError::OperationFailed(e.to_string()))?;
        
        // Convert SeqImageData to ImageData for processing
        let image = ImageData::from_u16(
            seq_image.width,
            seq_image.height,
            1,
            &seq_image.data,
        );

        // DIAGNOSTIC: Log raw data statistics to debug mid-gray image issue
        {
            let raw_data = &seq_image.data;
            if !raw_data.is_empty() {
                let min_val = raw_data.iter().min().copied().unwrap_or(0);
                let max_val = raw_data.iter().max().copied().unwrap_or(0);
                let sum: u64 = raw_data.iter().map(|&v| v as u64).sum();
                let mean_val = sum / raw_data.len() as u64;
                let unique_vals: std::collections::HashSet<_> = raw_data.iter().take(10000).collect();
                tracing::info!(
                    "[DIAGNOSTIC] Raw image data: size={}, min={}, max={}, mean={}, unique_sample_count={}",
                    raw_data.len(), min_val, max_val, mean_val, unique_vals.len()
                );
                if max_val == min_val {
                    tracing::error!("[DIAGNOSTIC] WARNING: All pixels have same value! Data appears uniform/invalid.");
                } else if max_val < 100 {
                    tracing::warn!("[DIAGNOSTIC] WARNING: Max value is very low ({}), image may be underexposed or data corrupted.", max_val);
                } else if min_val > 60000 {
                    tracing::warn!("[DIAGNOSTIC] WARNING: Min value is very high ({}), image may be saturated.", min_val);
                }
            } else {
                tracing::error!("[DIAGNOSTIC] WARNING: Raw data is empty!");
            }
        }
        
        // Automatic color detection from camera metadata
        let is_color = seq_image.sensor_type.as_deref() == Some("Color") && seq_image.bayer_offset.is_some();
        
        // Determine Bayer pattern from offsets (if color)
        let bayer_pattern = if is_color {
            match seq_image.bayer_offset {
                Some((0, 0)) => BayerPattern::RGGB,  // RGGB
                Some((1, 0)) => BayerPattern::GRBG,  // GRBG  
                Some((0, 1)) => BayerPattern::GBRG,  // GBRG
                Some((1, 1)) => BayerPattern::BGGR,  // BGGR
                _ => BayerPattern::RGGB,  // Default
            }
        } else {
            BayerPattern::RGGB  // Doesn't matter for mono
        };
        
        let display_data: Vec<u8>;
        
        if is_color {
            // Color debayering path
            let algorithm = DebayerAlgorithm::Bilinear;
            
            tracing::info!("Debayering color image with pattern {:?}", bayer_pattern);
            
            // 2. Debayer to RGB16 (if color)
            // Cast u8 buffer to u16 slice (unsafe but fast)
            let u16_data = unsafe {
                std::slice::from_raw_parts(
                    image.data.as_ptr() as *const u16,
                    image.data.len() / 2
                )
            };
            
            let mut rgb_data = nightshade_imaging::debayer_to_rgb16(
                u16_data,
                seq_image.width,
                seq_image.height,
                bayer_pattern,
                algorithm,
            );
            
            // 2.5. Apply Auto White Balance (Histogram Peak Alignment)
            apply_auto_white_balance(&mut rgb_data);
            
            // 3. Auto-stretch RGB (unified params for simplicity)
            let rgb_pixels: Vec<f64> = rgb_data.par_iter()
                .map(|&v| v as f64 / 65535.0)
                .collect();
            let mut sorted = rgb_pixels.clone();
            sorted.par_sort_unstable_by(|a, b| a.partial_cmp(b).unwrap());
            let median = sorted[sorted.len() / 2];
            let unified_params = nightshade_imaging::StretchParams {
                shadows: (median - 0.1).max(0.0),
                highlights: (median + 0.3).min(1.0),
                midtones: 0.5,
            };
            
            // 4. Apply stretch to convert RGB u16 -> RGB u8
            display_data = nightshade_imaging::apply_stretch_rgb(
                &rgb_data,
                seq_image.width,
                seq_image.height,
                &unified_params,
            );
        } else {
            // Grayscale: auto-stretch to u8
            let stretch_params = nightshade_imaging::auto_stretch_stf(&image);
            tracing::info!(
                "[DIAGNOSTIC] Stretch params: shadows={:.6}, highlights={:.6}, midtones={:.6}",
                stretch_params.shadows, stretch_params.highlights, stretch_params.midtones
            );
            display_data = nightshade_imaging::apply_stretch(&image, &stretch_params);

            // Check display data distribution
            let display_min = display_data.iter().min().copied().unwrap_or(0);
            let display_max = display_data.iter().max().copied().unwrap_or(0);
            let display_sum: u64 = display_data.iter().map(|&v| v as u64).sum();
            let display_mean = display_sum / display_data.len() as u64;
            tracing::info!(
                "[DIAGNOSTIC] Display data after stretch: min={}, max={}, mean={}",
                display_min, display_max, display_mean
            );
        }
        
        // Calculate statistics
        let stats = nightshade_imaging::calculate_stats_u16(&image);
        let stars = nightshade_imaging::detect_stars(&image, &nightshade_imaging::StarDetectionConfig::default());
        let star_count = stars.len() as u32;
        
        // Calculate histogram (256 bins for u8 display data)
        let mut histogram = vec![0u32; 256];
        for &pixel in &display_data {
            histogram[pixel as usize] += 1;
        }

        // Store the raw image data (legacy)
        {
            let mut raw_storage = get_last_raw_data_storage().write().await;
            *raw_storage = Some(seq_image.data.clone());
        }

        // Store the raw image info with metadata (used by sequencer)
        {
            let mut raw_info_storage = get_last_raw_image_info_storage().write().await;
            *raw_info_storage = Some(RawImageInfo {
                width: seq_image.width,
                height: seq_image.height,
                data: seq_image.data.clone(),
                sensor_type: seq_image.sensor_type.clone(),
                bayer_offset: seq_image.bayer_offset,
            });
        }

        // Store the captured image
        {
            let mut storage = get_last_image_storage().write().await;
            *storage = Some(CapturedImageResult {
                width: seq_image.width,
                height: seq_image.height,
                display_data,
                histogram,
                stats: ImageStatsResult {
                    min: stats.min,
                    max: stats.max,
                    mean: stats.mean,
                    median: stats.median,
                    std_dev: stats.std_dev,
                    hfr: None,
                    star_count,
                },
                exposure_time: duration_secs,
                timestamp: chrono::Utc::now().format("%Y-%m-%dT%H:%M:%S").to_string(),
                is_color,
            });
        }

        // Note: ExposureComplete event is published by UnifiedDeviceOps
        tracing::info!("Real camera exposure complete, {} stars detected", star_count);
        Ok(())
    }
}

/// Get the last captured image
pub async fn api_get_last_image() -> Result<CapturedImageResult, NightshadeError> {
    tracing::info!("API: api_get_last_image called");
    let storage = get_last_image_storage().read().await;
    match &*storage {
        Some(img) => {
            tracing::info!("API: Returning stored image {}x{}, display_data size: {} bytes",
                img.width, img.height, img.display_data.len());
            Ok(img.clone())
        }
        None => {
            tracing::warn!("API: No image available in storage");
            Err(NightshadeError::NoImageAvailable)
        }
    }
}

/// Get the last captured raw image data (u16)
/// This is used for saving FITS files with original bit depth
pub async fn api_get_last_raw_image_data() -> Result<Vec<u16>, NightshadeError> {
    let storage = get_last_raw_data_storage().read().await;
    storage.clone().ok_or(NightshadeError::NoImageAvailable)
}

/// Get the last captured raw image info with full metadata
/// This is used by the sequencer for HFR calculation, plate solving, and other analysis
/// that requires original 16-bit sensor data (not display-stretched 8-bit data)
#[flutter_rust_bridge::frb(ignore)]
pub async fn get_last_raw_image_info() -> Result<Option<RawImageInfo>, NightshadeError> {
    let storage = get_last_raw_image_info_storage().read().await;
    Ok(storage.clone())
}

/// Cancel current exposure
pub async fn api_camera_cancel_exposure(device_id: String) -> Result<(), NightshadeError> {
    if device_id.starts_with("sim_") {
        let mut camera = get_sim_camera().write().await;
        camera.status.state = CameraState::Idle;
        tracing::info!("Exposure cancelled");
        Ok(())
    } else {
        // Route real devices through DeviceManager
        let mgr = get_device_manager();
        mgr.camera_abort_exposure(&device_id).await
            .map_err(|e| NightshadeError::OperationFailed(e))
    }
}

/// Generate a simulated star field image
fn generate_simulated_image(
    width: u32,
    height: u32,
    gain: i32,
    exposure_time: f64,
) -> (Vec<u16>, Vec<u8>, Vec<u32>, nightshade_imaging::ImageStats, u32) {
    
    
    let mut rng = rand::thread_rng();
    let pixel_count = (width * height) as usize;
    
    // Create raw 16-bit image data
    let mut raw_data: Vec<u16> = vec![0u16; pixel_count];
    
    // Background level based on gain and exposure
    let base_background = 500 + (gain as f64 * 5.0 + exposure_time * 10.0) as u16;
    let noise_level = (50.0 + gain as f64 * 0.5) as u16;
    
    // Fill with background + noise
    for pixel in &mut raw_data {
        let noise = (rng.gen::<f64>() * noise_level as f64) as i32;
        *pixel = (base_background as i32 + noise - noise_level as i32 / 2).clamp(0, 65535) as u16;
    }
    
    // Add stars (more with longer exposure)
    let num_stars = (100.0 + exposure_time * 50.0).min(500.0) as u32;
    let mut star_count = 0u32;
    
    for _ in 0..num_stars {
        let x = rng.gen_range(5..width - 5);
        let y = rng.gen_range(5..height - 5);
        let brightness = rng.gen_range(5000u16..60000u16);
        let size = rng.gen_range(1.5f64..4.0f64);
        
        // Draw Gaussian star profile
        let radius = (size * 3.0) as i32;
        for dy in -radius..=radius {
            for dx in -radius..=radius {
                let px = (x as i32 + dx) as u32;
                let py = (y as i32 + dy) as u32;
                
                if px < width && py < height {
                    let dist_sq = (dx * dx + dy * dy) as f64;
                    let sigma_sq = size * size;
                    let intensity = brightness as f64 * (-dist_sq / (2.0 * sigma_sq)).exp();
                    
                    let idx = (py * width + px) as usize;
                    raw_data[idx] = (raw_data[idx] as f64 + intensity).min(65535.0) as u16;
                }
            }
        }
        star_count += 1;
    }
    
    // Add some hot pixels
    for _ in 0..20 {
        let idx = rng.gen_range(0..pixel_count);
        raw_data[idx] = rng.gen_range(40000u16..65535u16);
    }
    
    // Create ImageData for stats calculation
    let image_bytes: Vec<u8> = raw_data.iter()
        .flat_map(|&val| val.to_le_bytes())
        .collect();
    
    let image_data = nightshade_imaging::ImageData {
        width,
        height,
        channels: 1,
        pixel_type: nightshade_imaging::PixelType::U16,
        data: image_bytes.clone(),
    };
    
    // Calculate stats
    let stats = nightshade_imaging::calculate_stats_u16(&image_data);
    
    // Auto stretch for display
    let stretch_params = nightshade_imaging::auto_stretch_stf(&image_data);
    let display_data = nightshade_imaging::apply_stretch(&image_data, &stretch_params);
    
    // Calculate histogram from display data
    let mut histogram = vec![0u32; 256];
    for &pixel in &display_data {
        histogram[pixel as usize] += 1;
    }
    
    (raw_data, display_data, histogram, stats, star_count)
}

/// Internal random utilities - not exposed to Dart FFI
#[flutter_rust_bridge::frb(ignore)]
pub mod rand {
    use std::time::{SystemTime, UNIX_EPOCH};

    // Note: Range is NOT re-exported because FRB generates invalid code for Range<Self>
    // The gen_range function is marked as ignored by FRB anyway

    #[flutter_rust_bridge::frb(ignore)]
    pub fn random<T: RandomValue>() -> T {
        T::random()
    }

    #[flutter_rust_bridge::frb(ignore)]
    pub trait RandomValue {
        fn random() -> Self;
    }

    #[flutter_rust_bridge::frb(ignore)]
    impl RandomValue for f64 {
        fn random() -> Self {
            let seed = SystemTime::now()
                .duration_since(UNIX_EPOCH)
                .unwrap()
                .as_nanos() as u64;
            // Simple LCG
            let x = seed.wrapping_mul(6364136223846793005).wrapping_add(1442695040888963407);
            (x as f64) / (u64::MAX as f64)
        }
    }
    
    #[flutter_rust_bridge::frb(ignore)]
    pub struct Rng {
        pub state: u64,
    }

    #[flutter_rust_bridge::frb(ignore)]
    impl Rng {
        pub fn gen<T: RandomValue>(&mut self) -> T {
            T::random()
        }

        pub fn gen_range<T: RandomRange>(&mut self, range: std::ops::Range<T>) -> T {
            let seed = SystemTime::now()
                .duration_since(UNIX_EPOCH)
                .unwrap()
                .as_nanos() as u64;
            self.state = self.state.wrapping_mul(6364136223846793005).wrapping_add(seed);
            T::in_range(self.state, range)
        }
    }

    #[flutter_rust_bridge::frb(ignore)]
    pub trait RandomRange: Sized {
        fn in_range(seed: u64, range: std::ops::Range<Self>) -> Self;
    }

    #[flutter_rust_bridge::frb(ignore)]
    impl RandomRange for u32 {
        fn in_range(seed: u64, range: std::ops::Range<Self>) -> Self {
            let span = range.end - range.start;
            range.start + (seed as u32 % span)
        }
    }

    #[flutter_rust_bridge::frb(ignore)]
    impl RandomRange for u16 {
        fn in_range(seed: u64, range: std::ops::Range<Self>) -> Self {
            let span = range.end - range.start;
            range.start + (seed as u16 % span)
        }
    }

    #[flutter_rust_bridge::frb(ignore)]
    impl RandomRange for f64 {
        fn in_range(seed: u64, range: std::ops::Range<Self>) -> Self {
            let t = (seed as f64) / (u64::MAX as f64);
            range.start + t * (range.end - range.start)
        }
    }

    #[flutter_rust_bridge::frb(ignore)]
    impl RandomRange for usize {
        fn in_range(seed: u64, range: std::ops::Range<Self>) -> Self {
            let span = range.end - range.start;
            range.start + (seed as usize % span)
        }
    }

    #[flutter_rust_bridge::frb(ignore)]
    pub fn thread_rng() -> Rng {
        let seed = SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .unwrap()
            .as_nanos() as u64;
        Rng { state: seed }
    }
}

// =============================================================================
// Session Management
// =============================================================================

/// Get current session state
pub async fn api_get_session_state() -> SessionState {
    get_state().get_session().await
}

/// Start a new imaging session
pub async fn api_start_session(target_name: Option<String>, ra: Option<f64>, dec: Option<f64>) -> Result<(), NightshadeError> {
    get_state().start_session(target_name, ra, dec).await;
    tracing::info!("Session started");
    Ok(())
}

/// End the current session
pub async fn api_end_session() -> Result<(), NightshadeError> {
    get_state().end_session().await;
    tracing::info!("Session ended");
    Ok(())
}

// =============================================================================
// REAL FITS FILE OPERATIONS
// =============================================================================

/// Result from reading a FITS file
#[derive(Debug, Clone)]
pub struct FitsReadResult {
    pub width: u32,
    pub height: u32,
    pub bitpix: i32,
    pub display_data: Vec<u8>,
    pub histogram: Vec<u32>,
    pub stats: ImageStatsResult,
    pub object_name: Option<String>,
    pub exposure_time: Option<f64>,
    pub filter: Option<String>,
    pub ra: Option<f64>,
    pub dec: Option<f64>,
    pub date_obs: Option<String>,
    pub bayer_pattern: Option<String>,
}

/// Read a FITS file from disk
pub async fn api_read_fits_file(file_path: String) -> Result<FitsReadResult, NightshadeError> {
    use std::path::Path;
    
    tracing::info!("Reading FITS file: {}", file_path);
    
    let path = Path::new(&file_path);
    if !path.exists() {
        return Err(NightshadeError::IoError(format!("File not found: {}", file_path)));
    }
    
    // Read the actual FITS file
    let (image_data, header) = nightshade_imaging::read_fits(path)
        .map_err(|e| NightshadeError::ImageError(format!("Failed to read FITS: {}", e)))?;
    
    // Extract header keywords
    let object_name = header.get_string("OBJECT").map(|s| s.to_string());
    let exposure_time = header.get_float("EXPTIME");
    let filter = header.get_string("FILTER").map(|s| s.to_string());
    let ra = header.get_float("RA");
    let dec = header.get_float("DEC");
    let date_obs = header.get_string("DATE-OBS").map(|s| s.to_string());
    let bitpix = header.get_int("BITPIX").unwrap_or(16) as i32;
    let bayer_pattern = header.get_string("BAYERPAT").map(|s| s.to_string());
    
    // Calculate statistics
    let stats = nightshade_imaging::calculate_stats_u16(&image_data);
    
    // Auto stretch for display
    let stretch_params = nightshade_imaging::auto_stretch_stf(&image_data);
    let display_data = nightshade_imaging::apply_stretch(&image_data, &stretch_params);
    
    // Calculate histogram
    let mut histogram = vec![0u32; 256];
    for &pixel in &display_data {
        histogram[pixel as usize] += 1;
    }
    
    tracing::info!("FITS file loaded: {}x{}, {} pixels", image_data.width, image_data.height, 
        image_data.width * image_data.height);
    
    Ok(FitsReadResult {
        width: image_data.width,
        height: image_data.height,
        bitpix,
        display_data,
        histogram,
        stats: ImageStatsResult {
            min: stats.min,
            max: stats.max,
            mean: stats.mean,
            median: stats.median,
            std_dev: stats.std_dev,
            hfr: None,
            star_count: 0,
        },
        object_name,
        exposure_time,
        filter,
        ra,
        dec,
        date_obs,
        bayer_pattern,
    })
}

/// FITS header for writing
// =============================================================================
// STAR DETECTION AND IMAGE ANALYSIS
// =============================================================================

/// Detected star information
#[derive(Debug, Clone)]
pub struct DetectedStarInfo {
    pub x: f64,
    pub y: f64,
    pub flux: f64,
    pub hfr: f64,
    pub fwhm: f64,
    pub peak: f64,
    pub background: f64,
    pub snr: f64,
    /// Eccentricity: 0 = perfect circle, 1 = line (elongated)
    pub eccentricity: f64,
    /// Sharpness: ratio of peak to spread - hot pixels have high sharpness
    pub sharpness: f64,
}

/// Star detection result
#[derive(Debug, Clone)]
pub struct StarDetectionResultApi {
    pub stars: Vec<DetectedStarInfo>,
    pub star_count: u32,
    pub median_hfr: f64,
    pub median_fwhm: f64,
    pub median_snr: f64,
    pub background: f64,
    pub noise: f64,
}

/// Star detection configuration
#[derive(Debug, Clone)]
#[flutter_rust_bridge::frb]
pub struct StarDetectionConfigApi {
    pub detection_sigma: f64,
    pub min_area: u32,
    pub max_area: u32,
    pub max_eccentricity: f64,
    pub saturation_limit: u32,
    pub hfr_radius: u32,
    /// Minimum HFR to be considered a real star (filters hot pixels)
    pub min_hfr: Option<f64>,
    /// Minimum SNR to be considered a valid detection
    pub min_snr: Option<f64>,
    /// Maximum sharpness (filters hot pixels which have very high sharpness)
    pub max_sharpness: Option<f64>,
}

impl Default for StarDetectionConfigApi {
    fn default() -> Self {
        Self {
            detection_sigma: 5.0,   // Increased from 3.0 - more conservative
            min_area: 9,            // Increased from 5 - hot pixels rarely exceed this
            max_area: 10000,
            max_eccentricity: 0.7,  // Slightly tighter - real stars are round
            saturation_limit: 60000,
            hfr_radius: 20,
            min_hfr: Some(1.2),     // Real stars have HFR > 1.2 typically
            min_snr: Some(10.0),    // Require decent signal-to-noise
            max_sharpness: Some(0.7), // Hot pixels have sharpness > 0.8
        }
    }
}

/// Detect stars in a FITS file
pub async fn api_detect_stars_in_file(
    file_path: String,
    config: Option<StarDetectionConfigApi>,
) -> Result<StarDetectionResultApi, NightshadeError> {
    use std::path::Path;
    
    tracing::info!("Detecting stars in: {}", file_path);
    
    let path = Path::new(&file_path);
    let (image_data, _header) = nightshade_imaging::read_fits(path)
        .map_err(|e| NightshadeError::ImageError(format!("Failed to read FITS: {}", e)))?;
    
    let config = config.unwrap_or_default();
    let detection_config = nightshade_imaging::StarDetectionConfig {
        detection_sigma: config.detection_sigma,
        min_area: config.min_area,
        max_area: config.max_area,
        max_eccentricity: config.max_eccentricity,
        saturation_limit: config.saturation_limit as u16,
        hfr_radius: config.hfr_radius,
        min_hfr: config.min_hfr.unwrap_or(1.2),
        min_snr: config.min_snr.unwrap_or(10.0),
        max_sharpness: config.max_sharpness.unwrap_or(0.7),
    };
    
    let result = nightshade_imaging::detect_stars_with_stats(&image_data, &detection_config);
    
    let stars: Vec<DetectedStarInfo> = result.stars.iter().map(|s| DetectedStarInfo {
        x: s.x,
        y: s.y,
        flux: s.flux,
        hfr: s.hfr,
        fwhm: s.fwhm,
        peak: s.peak,
        background: s.background,
        snr: s.snr,
        eccentricity: s.eccentricity,
        sharpness: s.sharpness,
    }).collect();
    
    tracing::info!("Detected {} stars, median HFR: {:.2}", result.star_count, result.median_hfr);
    
    Ok(StarDetectionResultApi {
        stars,
        star_count: result.star_count,
        median_hfr: result.median_hfr,
        median_fwhm: result.median_fwhm,
        median_snr: result.median_snr,
        background: result.background,
        noise: result.noise,
    })
}

/// Calculate HFR for a FITS file
pub async fn api_calculate_hfr(file_path: String) -> Result<Option<f64>, NightshadeError> {
    use std::path::Path;
    
    let path = Path::new(&file_path);
    let (image_data, _header) = nightshade_imaging::read_fits(path)
        .map_err(|e| NightshadeError::ImageError(format!("Failed to read FITS: {}", e)))?;
    
    Ok(nightshade_imaging::calculate_median_hfr(&image_data))
}

/// Calculate histogram for a FITS file
pub async fn api_calculate_histogram(
    file_path: String,
    _bins: u32,
    logarithmic: u8,
) -> Result<Vec<f32>, NightshadeError> {
    use std::path::Path;
    
    let path = Path::new(&file_path);
    let (image_data, _header) = nightshade_imaging::read_fits(path)
        .map_err(|e| NightshadeError::ImageError(format!("Failed to read FITS: {}", e)))?;
    
    let logarithmic_bool = logarithmic != 0;
    let histogram = nightshade_imaging::calculate_display_histogram(&image_data, logarithmic_bool);
    Ok(histogram)
}

/// Stretch parameters for manual control
#[derive(Debug, Clone)]
pub struct StretchParamsApi {
    pub shadows: f64,
    pub highlights: f64,
    pub midtones: f64,
}

/// Auto-calculate stretch parameters for an image
pub async fn api_calculate_auto_stretch(file_path: String) -> Result<StretchParamsApi, NightshadeError> {
    use std::path::Path;
    
    let path = Path::new(&file_path);
    let (image_data, _header) = nightshade_imaging::read_fits(path)
        .map_err(|e| NightshadeError::ImageError(format!("Failed to read FITS: {}", e)))?;
    
    let params = nightshade_imaging::auto_stretch_stf(&image_data);
    
    Ok(StretchParamsApi {
        shadows: params.shadows,
        highlights: params.highlights,
        midtones: params.midtones,
    })
}

/// Apply stretch to a FITS file and return display data
pub async fn api_apply_stretch(
    file_path: String,
    params: StretchParamsApi,
) -> Result<Vec<u8>, NightshadeError> {
    use std::path::Path;
    
    let path = Path::new(&file_path);
    let (image_data, _header) = nightshade_imaging::read_fits(path)
        .map_err(|e| NightshadeError::ImageError(format!("Failed to read FITS: {}", e)))?;
    
    let stretch_params = nightshade_imaging::StretchParams {
        shadows: params.shadows,
        highlights: params.highlights,
        midtones: params.midtones,
    };
    
    let display_data = nightshade_imaging::apply_stretch(&image_data, &stretch_params);
    Ok(display_data)
}

// =============================================================================
// DEBAYERING (COLOR CAMERAS)
// =============================================================================

/// Bayer pattern type
#[derive(Debug, Clone, Copy)]
pub enum BayerPatternApi {
    RGGB,
    BGGR,
    GRBG,
    GBRG,
}

/// Debayer algorithm
#[derive(Debug, Clone, Copy)]
pub enum DebayerAlgorithmApi {
    Bilinear,
    VNG,
    SuperPixel,
}

/// Debayer a raw FITS image and return RGB display data
/// Debayer a raw FITS file and return RGB display data
pub async fn api_debayer_fits_file(
    file_path: String,
    pattern: BayerPatternApi,
    algorithm: DebayerAlgorithmApi,
) -> Result<Vec<u8>, NightshadeError> {
    use std::path::Path;
    
    let path = Path::new(&file_path);
    let (image_data, _header) = nightshade_imaging::read_fits(path)
        .map_err(|e| NightshadeError::ImageError(format!("Failed to read FITS: {}", e)))?;
    
    let bayer_pattern = match pattern {
        BayerPatternApi::RGGB => nightshade_imaging::BayerPattern::RGGB,
        BayerPatternApi::BGGR => nightshade_imaging::BayerPattern::BGGR,
        BayerPatternApi::GRBG => nightshade_imaging::BayerPattern::GRBG,
        BayerPatternApi::GBRG => nightshade_imaging::BayerPattern::GBRG,
    };
    
    let debayer_alg = match algorithm {
        DebayerAlgorithmApi::Bilinear => nightshade_imaging::DebayerAlgorithm::Bilinear,
        DebayerAlgorithmApi::VNG => nightshade_imaging::DebayerAlgorithm::VNG,
        DebayerAlgorithmApi::SuperPixel => nightshade_imaging::DebayerAlgorithm::SuperPixel,
    };
    
    let rgb_image = nightshade_imaging::debayer(
        &image_data.data,
        image_data.width,
        image_data.height,
        bayer_pattern,
        debayer_alg,
    );
    
    // Return RGBA8 for Flutter display
    Ok(rgb_image.to_rgba8())
}

// =============================================================================
// XISF FILE SUPPORT
// =============================================================================

/// XISF file read result
#[derive(Debug, Clone)]
pub struct XisfReadResult {
    pub width: u32,
    pub height: u32,
    pub channels: u32,
    pub display_data: Vec<u8>,
    pub histogram: Vec<u32>,
    pub stats: ImageStatsResult,
    pub properties: Vec<(String, String)>,
}

/// Read an XISF file
pub async fn api_read_xisf_file(file_path: String) -> Result<XisfReadResult, NightshadeError> {
    use std::path::Path;
    
    tracing::info!("Reading XISF file: {}", file_path);
    
    let path = Path::new(&file_path);
    if !path.exists() {
        return Err(NightshadeError::IoError(format!("File not found: {}", file_path)));
    }
    
    let (image_data, metadata) = nightshade_imaging::read_xisf(path)
        .map_err(|e| NightshadeError::ImageError(format!("Failed to read XISF: {}", e)))?;
    
    // Calculate statistics
    let stats = nightshade_imaging::calculate_stats_u16(&image_data);
    
    // Auto stretch for display
    let stretch_params = nightshade_imaging::auto_stretch_stf(&image_data);
    let display_data = nightshade_imaging::apply_stretch(&image_data, &stretch_params);
    
    // Calculate histogram
    let mut histogram = vec![0u32; 256];
    for &pixel in &display_data {
        histogram[pixel as usize] += 1;
    }
    
    // Convert properties to strings
    let properties: Vec<(String, String)> = metadata.properties.iter()
        .map(|(k, v)| (k.clone(), format!("{:?}", v)))
        .chain(metadata.fits_keywords.iter().map(|(k, v)| (k.clone(), v.clone())))
        .collect();
    
    tracing::info!("XISF file loaded: {}x{}x{}", image_data.width, image_data.height, image_data.channels);
    
    Ok(XisfReadResult {
        width: image_data.width,
        height: image_data.height,
        channels: image_data.channels,
        display_data,
        histogram,
        stats: ImageStatsResult {
            min: stats.min,
            max: stats.max,
            mean: stats.mean,
            median: stats.median,
            std_dev: stats.std_dev,
            hfr: None,
            star_count: 0,
        },
        properties,
    })
}

/// Save image as XISF
pub async fn api_save_xisf_file(
    file_path: String,
    width: u32,
    height: u32,
    data: Vec<u16>,
    properties: Vec<(String, String)>,
) -> Result<(), NightshadeError> {
    use std::path::Path;
    
    tracing::info!("Saving XISF file: {}", file_path);
    
    let image_data = nightshade_imaging::ImageData::from_u16(width, height, 1, &data);
    
    let mut metadata = nightshade_imaging::XisfMetadata::default();
    for (key, value) in properties {
        metadata.fits_keywords.insert(key, value);
    }
    
    let path = Path::new(&file_path);
    nightshade_imaging::write_xisf(path, &image_data, &metadata)
        .map_err(|e| NightshadeError::ImageError(format!("Failed to write XISF: {}", e)))?;

    tracing::info!("XISF file saved: {}", file_path);
    Ok(())
}

/// Save image as TIFF (16-bit preserving)
pub async fn api_save_tiff_file(
    file_path: String,
    width: u32,
    height: u32,
    data: Vec<u16>,
) -> Result<(), NightshadeError> {
    use std::path::Path;

    tracing::info!("Saving TIFF file: {}", file_path);

    let image_data = nightshade_imaging::ImageData::from_u16(width, height, 1, &data);

    let path = Path::new(&file_path);
    nightshade_imaging::write_tiff(path, &image_data)
        .map_err(|e| NightshadeError::ImageError(format!("Failed to write TIFF: {}", e)))?;

    tracing::info!("TIFF file saved: {}", file_path);
    Ok(())
}

/// Save image as PNG (16-bit preserving, lossless)
pub async fn api_save_png_file(
    file_path: String,
    width: u32,
    height: u32,
    data: Vec<u16>,
) -> Result<(), NightshadeError> {
    use std::path::Path;

    tracing::info!("Saving PNG file: {}", file_path);

    let image_data = nightshade_imaging::ImageData::from_u16(width, height, 1, &data);

    let path = Path::new(&file_path);
    nightshade_imaging::write_png(path, &image_data)
        .map_err(|e| NightshadeError::ImageError(format!("Failed to write PNG: {}", e)))?;

    tracing::info!("PNG file saved: {}", file_path);
    Ok(())
}

/// Save image as JPEG (8-bit, lossy - for previews)
pub async fn api_save_jpeg_file(
    file_path: String,
    width: u32,
    height: u32,
    data: Vec<u16>,
    quality: u8,
) -> Result<(), NightshadeError> {
    use std::path::Path;

    tracing::info!("Saving JPEG file: {} (quality: {})", file_path, quality);

    let image_data = nightshade_imaging::ImageData::from_u16(width, height, 1, &data);

    let path = Path::new(&file_path);
    nightshade_imaging::write_jpeg(path, &image_data, quality)
        .map_err(|e| NightshadeError::ImageError(format!("Failed to write JPEG: {}", e)))?;

    tracing::info!("JPEG file saved: {}", file_path);
    Ok(())
}

// =============================================================================
// FILE NAMING PATTERNS
// =============================================================================

/// Frame type for file naming
#[derive(Debug, Clone, Copy)]
pub enum FrameTypeApi {
    Light,
    Dark,
    Flat,
    Bias,
    DarkFlat,
    Snapshot,
}

/// Generate a filename from pattern and context
pub async fn api_generate_filename(
    pattern: String,
    base_dir: String,
    target: Option<String>,
    filter: Option<String>,
    exposure_time: f64,
    frame_type: FrameTypeApi,
    frame_number: u32,
    gain: Option<i32>,
    offset: Option<i32>,
    temperature: Option<f64>,
    binning_x: u32,
    binning_y: u32,
    camera: Option<String>,
    telescope: Option<String>,
    extension: String,
) -> String {
    let frame_type_impl = match frame_type {
        FrameTypeApi::Light => nightshade_imaging::FrameType::Light,
        FrameTypeApi::Dark => nightshade_imaging::FrameType::Dark,
        FrameTypeApi::Flat => nightshade_imaging::FrameType::Flat,
        FrameTypeApi::Bias => nightshade_imaging::FrameType::Bias,
        FrameTypeApi::DarkFlat => nightshade_imaging::FrameType::DarkFlat,
        FrameTypeApi::Snapshot => nightshade_imaging::FrameType::Snapshot,
    };
    
    let mut context = nightshade_imaging::NamingContext::new()
        .with_current_time()
        .with_exposure(exposure_time)
        .with_frame_type(frame_type_impl)
        .with_frame_number(frame_number)
        .with_binning(binning_x, binning_y);
    
    if let Some(t) = target {
        context = context.with_target(t);
    }
    if let Some(f) = filter {
        context = context.with_filter(f);
    }
    if let Some(g) = gain {
        context = context.with_gain(g);
    }
    if let Some(o) = offset {
        context = context.with_offset(o);
    }
    if let Some(t) = temperature {
        context = context.with_temperature(t);
    }
    if let Some(c) = camera {
        context = context.with_camera(c);
    }
    if let Some(t) = telescope {
        context = context.with_telescope(t);
    }
    
    let naming_pattern = nightshade_imaging::NamingPattern::new(pattern)
        .with_base_dir(base_dir)
        .with_extension(extension);
    
    naming_pattern.generate(&context).to_string_lossy().to_string()
}

/// Get the next frame number for a directory
pub async fn api_get_next_frame_number(
    base_dir: String,
    pattern: String,
    target: Option<String>,
    filter: Option<String>,
    frame_type: FrameTypeApi,
) -> u32 {
    use std::path::Path;
    
    let frame_type_impl = match frame_type {
        FrameTypeApi::Light => nightshade_imaging::FrameType::Light,
        FrameTypeApi::Dark => nightshade_imaging::FrameType::Dark,
        FrameTypeApi::Flat => nightshade_imaging::FrameType::Flat,
        FrameTypeApi::Bias => nightshade_imaging::FrameType::Bias,
        FrameTypeApi::DarkFlat => nightshade_imaging::FrameType::DarkFlat,
        FrameTypeApi::Snapshot => nightshade_imaging::FrameType::Snapshot,
    };
    
    let mut context = nightshade_imaging::NamingContext::new()
        .with_frame_type(frame_type_impl);
    
    if let Some(t) = target {
        context = context.with_target(t);
    }
    if let Some(f) = filter {
        context = context.with_filter(f);
    }
    
    let naming_pattern = nightshade_imaging::NamingPattern::new(pattern);
    let base_path = Path::new(&base_dir);
    
    nightshade_imaging::scan_for_next_frame_number(base_path, &naming_pattern, &context)
}

// =============================================================================
// REAL PLATE SOLVING
// =============================================================================

/// Plate solve result
#[derive(Debug, Clone)]
pub struct PlateSolveResult {
    pub success: bool,
    pub ra: f64,          // degrees
    pub dec: f64,         // degrees
    pub pixel_scale: f64, // arcsec/pixel
    pub rotation: f64,    // degrees, East of North
    pub field_width: f64, // degrees
    pub field_height: f64,// degrees
    pub solve_time_secs: f64,
    pub error: Option<String>,
}

/// Check if a plate solver is available
#[flutter_rust_bridge::frb(sync)]
pub fn api_is_plate_solver_available() -> bool {
    nightshade_imaging::is_solver_available()
}

/// Get the path to the installed plate solver
#[flutter_rust_bridge::frb(sync)]
pub fn api_get_plate_solver_path() -> Option<String> {
    nightshade_imaging::get_solver_path().map(|p| p.to_string_lossy().to_string())
}

/// Plate solve an image file (blind solve)
pub async fn api_plate_solve_blind(file_path: String) -> Result<PlateSolveResult, NightshadeError> {
    use std::path::Path;
    
    tracing::info!("Blind plate solving: {}", file_path);
    
    let path = Path::new(&file_path);
    if !path.exists() {
        return Err(NightshadeError::IoError(format!("File not found: {}", file_path)));
    }
    
    // Run actual plate solve using ASTAP
    let result = nightshade_imaging::blind_solve(path);
    
    Ok(PlateSolveResult {
        success: result.success,
        ra: result.ra,
        dec: result.dec,
        pixel_scale: result.pixel_scale,
        rotation: result.rotation,
        field_width: result.field_width,
        field_height: result.field_height,
        solve_time_secs: result.solve_time_secs,
        error: result.error,
    })
}

/// Plate solve an image with hint coordinates
pub async fn api_plate_solve_near(
    file_path: String,
    hint_ra: f64,
    hint_dec: f64,
    search_radius: f64,
) -> Result<PlateSolveResult, NightshadeError> {
    use std::path::Path;
    
    tracing::info!("Plate solving near RA:{:.2}°, Dec:{:.2}°: {}", hint_ra, hint_dec, file_path);
    
    let path = Path::new(&file_path);
    if !path.exists() {
        return Err(NightshadeError::IoError(format!("File not found: {}", file_path)));
    }
    
    // Run actual plate solve using ASTAP with hints
    let result = nightshade_imaging::solve_near(path, hint_ra, hint_dec, search_radius);
    
    Ok(PlateSolveResult {
        success: result.success,
        ra: result.ra,
        dec: result.dec,
        pixel_scale: result.pixel_scale,
        rotation: result.rotation,
        field_width: result.field_width,
        field_height: result.field_height,
        solve_time_secs: result.solve_time_secs,
        error: result.error,
    })
}

// =============================================================================
// REAL PHD2 GUIDING INTEGRATION
// =============================================================================

/// PHD2 connection state
#[derive(Debug, Clone)]
pub struct Phd2Status {
    pub connected: bool,
    pub state: String,  // "Disconnected", "Connected", "Calibrating", "Guiding", "Looping", "Paused"
    pub rms_ra: f64,
    pub rms_dec: f64,
    pub rms_total: f64,
    pub snr: f64,
    pub star_mass: f64,
    pub pixel_scale: f64,
}

/// PHD2 calibration data
#[derive(Debug, Clone)]
pub struct Phd2CalibrationData {
    /// Whether the mount is calibrated
    pub is_calibrated: bool,
    /// RA axis rotation angle (degrees)
    pub ra_angle: Option<f64>,
    /// Dec axis rotation angle (degrees)
    pub dec_angle: Option<f64>,
    /// RA guide rate (pixels/second)
    pub ra_rate: Option<f64>,
    /// Dec guide rate (pixels/second)
    pub dec_rate: Option<f64>,
}

/// PHD2 star image data
#[derive(Debug, Clone)]
pub struct Phd2StarImage {
    /// Frame number
    pub frame: u32,
    /// Image width in pixels
    pub width: u32,
    /// Image height in pixels
    pub height: u32,
    /// Star centroid X position
    pub star_x: f64,
    /// Star centroid Y position
    pub star_y: f64,
    /// Raw pixel data (16-bit grayscale as bytes)
    pub pixels: Vec<u8>,
}

/// PHD2 Brain algorithm parameter
#[derive(Debug, Clone)]
pub struct Phd2AlgoParam {
    /// Parameter name
    pub name: String,
    /// Parameter value
    pub value: f64,
}

/// Check if PHD2 is running
#[flutter_rust_bridge::frb(sync)]
pub fn api_is_phd2_running() -> bool {
    nightshade_imaging::is_phd2_running()
}

/// Static PHD2 client storage
static PHD2_CLIENT: OnceLock<Arc<RwLock<Option<nightshade_imaging::Phd2Client>>>> = OnceLock::new();

#[flutter_rust_bridge::frb(ignore)]
pub fn get_phd2_storage() -> &'static Arc<RwLock<Option<nightshade_imaging::Phd2Client>>> {
    PHD2_CLIENT.get_or_init(|| Arc::new(RwLock::new(None)))
}

/// Connect to PHD2
pub async fn api_phd2_connect(host: Option<String>, port: Option<u16>) -> Result<(), NightshadeError> {
    let host = host.unwrap_or_else(|| "127.0.0.1".to_string());
    let port = port.unwrap_or(4400);

    tracing::info!("Connecting to PHD2 at {}:{}", host, port);

    let mut client = nightshade_imaging::Phd2Client::new(&host, port);

    // Set up event callback to forward PHD2 events to the main event stream
    client.set_event_callback(move |event| {
        match event {
            nightshade_imaging::Phd2Event::GuideStep(frame) => {
                // Forward guide step data to the main event stream
                get_state().publish_guiding_event(
                    GuidingEvent::Correction {
                        ra: frame.ra_distance,
                        dec: frame.dec_distance,
                        ra_raw: frame.ra_distance,
                        dec_raw: frame.dec_distance,
                    },
                    EventSeverity::Info,
                );
            }
            nightshade_imaging::Phd2Event::StateChanged(state) => {
                tracing::debug!("PHD2 state changed: {:?}", state);
                match state {
                    nightshade_imaging::Phd2State::Guiding => {
                        get_state().publish_guiding_event(
                            GuidingEvent::GuidingStarted,
                            EventSeverity::Info,
                        );
                    }
                    nightshade_imaging::Phd2State::Disconnected => {
                        get_state().publish_guiding_event(
                            GuidingEvent::Disconnected,
                            EventSeverity::Warning,
                        );
                    }
                    nightshade_imaging::Phd2State::Paused => {
                        get_state().publish_guiding_event(
                            GuidingEvent::Paused,
                            EventSeverity::Info,
                        );
                    }
                    nightshade_imaging::Phd2State::LostLock => {
                        get_state().publish_guiding_event(
                            GuidingEvent::LostStar,
                            EventSeverity::Warning,
                        );
                    }
                    _ => {}
                }
            }
            nightshade_imaging::Phd2Event::StarLost => {
                get_state().publish_guiding_event(
                    GuidingEvent::LostStar,
                    EventSeverity::Warning,
                );
            }
            nightshade_imaging::Phd2Event::SettleDone { .. } => {
                // Get the last RMS from the rolling stats if available
                get_state().publish_guiding_event(
                    GuidingEvent::Settled { rms: 0.0 }, // TODO: get actual RMS
                    EventSeverity::Info,
                );
            }
            nightshade_imaging::Phd2Event::CalibrationComplete => {
                tracing::info!("PHD2: Calibration complete");
            }
            nightshade_imaging::Phd2Event::Disconnected => {
                get_state().publish_guiding_event(
                    GuidingEvent::Disconnected,
                    EventSeverity::Warning,
                );
            }
            _ => {}
        }
    });

    client.connect()
        .map_err(|e| NightshadeError::ConnectionFailed(format!("PHD2: {}", e)))?;

    // Store the client
    let mut storage = get_phd2_storage().write().await;
    *storage = Some(client);

    // Register PHD2 as a connected guider device in AppState
    // This ensures api_get_connected_devices() returns the guider
    let phd2_device_info = DeviceInfo {
        id: "phd2_guider".to_string(),
        name: "PHD2".to_string(),
        device_type: DeviceType::Guider,
        driver_type: DriverType::Native,
        description: format!("PHD2 Guiding at {}:{}", host, port),
        driver_version: String::new(),
        serial_number: None,
        unique_id: None,
        display_name: "PHD2 Guiding".to_string(),
    };
    get_state().register_device(phd2_device_info, ConnectionState::Connected).await;

    // Publish event
    get_state().publish_guiding_event(
        GuidingEvent::Connected,
        EventSeverity::Info,
    );

    Ok(())
}

/// Disconnect from PHD2
pub async fn api_phd2_disconnect() -> Result<(), NightshadeError> {
    let mut storage = get_phd2_storage().write().await;
    if let Some(mut client) = storage.take() {
        client.disconnect();
    }

    // Remove PHD2 from connected devices in AppState
    get_state().remove_device(DeviceType::Guider, "phd2_guider").await;

    get_state().publish_guiding_event(
        GuidingEvent::Disconnected,
        EventSeverity::Info,
    );

    Ok(())
}

/// Start guiding in PHD2
pub async fn api_phd2_start_guiding(
    settle_pixels: f64,
    settle_time: f64,
    settle_timeout: f64,
) -> Result<(), NightshadeError> {
    let mut storage = get_phd2_storage().write().await;
    let client = storage.as_mut()
        .ok_or_else(|| NightshadeError::NotConnected("PHD2".to_string()))?;
    
    client.guide(settle_pixels, settle_time, settle_timeout)
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to start guiding: {}", e)))?;
    
    get_state().publish_guiding_event(
        GuidingEvent::GuidingStarted,
        EventSeverity::Info,
    );
    
    Ok(())
}

/// Stop guiding in PHD2
pub async fn api_phd2_stop_guiding() -> Result<(), NightshadeError> {
    let mut storage = get_phd2_storage().write().await;
    let client = storage.as_mut()
        .ok_or_else(|| NightshadeError::NotConnected("PHD2".to_string()))?;
    
    client.stop_capture()
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to stop guiding: {}", e)))?;
    
    get_state().publish_guiding_event(
        GuidingEvent::GuidingStopped,
        EventSeverity::Info,
    );
    
    Ok(())
}

/// Dither in PHD2
pub async fn api_phd2_dither(
    amount: f64,
    ra_only: u8,
    settle_pixels: f64,
    settle_time: f64,
    settle_timeout: f64,
) -> Result<(), NightshadeError> {
    let mut storage = get_phd2_storage().write().await;
    let client = storage.as_mut()
        .ok_or_else(|| NightshadeError::NotConnected("PHD2".to_string()))?;
    
    let ra_only_bool = ra_only != 0;
    client.dither(amount, ra_only_bool, settle_pixels, settle_time, settle_timeout)
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to dither: {}", e)))?;
    
    get_state().publish_guiding_event(
        GuidingEvent::DitherStarted { pixels: amount },
        EventSeverity::Info,
    );
    
    Ok(())
}

/// Get PHD2 status
pub async fn api_phd2_get_status() -> Result<Phd2Status, NightshadeError> {
    let mut storage = get_phd2_storage().write().await;
    let client = storage.as_mut()
        .ok_or_else(|| NightshadeError::NotConnected("PHD2".to_string()))?;
    
    let state = client.get_app_state()
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to get PHD2 state: {}", e)))?;
    
    let pixel_scale = client.get_pixel_scale().unwrap_or(0.0);
    
    let state_str = match state {
        nightshade_imaging::Phd2State::Disconnected => "Disconnected",
        nightshade_imaging::Phd2State::Connected => "Connected",
        nightshade_imaging::Phd2State::Calibrating => "Calibrating",
        nightshade_imaging::Phd2State::Guiding => "Guiding",
        nightshade_imaging::Phd2State::Looping => "Looping",
        nightshade_imaging::Phd2State::Paused => "Paused",
        nightshade_imaging::Phd2State::Settling => "Settling",
        nightshade_imaging::Phd2State::LostLock => "LostLock",
    };
    
    Ok(Phd2Status {
        connected: true,
        state: state_str.to_string(),
        rms_ra: 0.0,  // Would need to track from events
        rms_dec: 0.0,
        rms_total: 0.0,
        snr: 0.0,
        star_mass: 0.0,
        pixel_scale,
    })
}

/// Get PHD2 star image with metadata
pub async fn api_phd2_get_star_image(size: u32) -> Result<Phd2StarImage, NightshadeError> {
    let mut storage = get_phd2_storage().write().await;
    let client = storage.as_mut()
        .ok_or_else(|| NightshadeError::NotConnected("PHD2".to_string()))?;

    let image_data = client.get_star_image_data(size)
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to get star image: {}", e)))?;

    Ok(Phd2StarImage {
        frame: image_data.frame,
        width: image_data.width,
        height: image_data.height,
        star_x: image_data.star_x,
        star_y: image_data.star_y,
        pixels: image_data.pixels,
    })
}

/// Get PHD2 algorithm parameter names for an axis
pub async fn api_phd2_get_algo_param_names(axis: String) -> Result<Vec<String>, NightshadeError> {
    let mut storage = get_phd2_storage().write().await;
    let client = storage.as_mut()
        .ok_or_else(|| NightshadeError::NotConnected("PHD2".to_string()))?;

    client.get_algo_param_names(&axis)
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to get algo param names: {}", e)))
}

/// Get PHD2 algorithm parameter value
pub async fn api_phd2_get_algo_param(axis: String, name: String) -> Result<f64, NightshadeError> {
    let mut storage = get_phd2_storage().write().await;
    let client = storage.as_mut()
        .ok_or_else(|| NightshadeError::NotConnected("PHD2".to_string()))?;

    client.get_algo_param(&axis, &name)
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to get algo param: {}", e)))
}

/// Set PHD2 algorithm parameter value
pub async fn api_phd2_set_algo_param(axis: String, name: String, value: f64) -> Result<(), NightshadeError> {
    let mut storage = get_phd2_storage().write().await;
    let client = storage.as_mut()
        .ok_or_else(|| NightshadeError::NotConnected("PHD2".to_string()))?;

    client.set_algo_param(&axis, &name, value)
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to set algo param: {}", e)))
}

/// Get all PHD2 algorithm parameters for an axis
pub async fn api_phd2_get_all_algo_params(axis: String) -> Result<Vec<Phd2AlgoParam>, NightshadeError> {
    let mut storage = get_phd2_storage().write().await;
    let client = storage.as_mut()
        .ok_or_else(|| NightshadeError::NotConnected("PHD2".to_string()))?;

    let params = client.get_all_algo_params(&axis)
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to get all algo params: {}", e)))?;

    Ok(params.into_iter().map(|p| Phd2AlgoParam {
        name: p.name,
        value: p.value,
    }).collect())
}

/// Pause or resume PHD2 guiding
pub async fn api_phd2_set_paused(paused: bool) -> Result<(), NightshadeError> {
    let mut storage = get_phd2_storage().write().await;
    let client = storage.as_mut()
        .ok_or_else(|| NightshadeError::NotConnected("PHD2".to_string()))?;

    client.set_paused(paused)
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to set paused: {}", e)))?;

    get_state().publish_guiding_event(
        if paused { GuidingEvent::Paused } else { GuidingEvent::Resumed },
        EventSeverity::Info,
    );

    Ok(())
}

/// Clear PHD2 calibration
pub async fn api_phd2_clear_calibration(which: String) -> Result<(), NightshadeError> {
    let mut storage = get_phd2_storage().write().await;
    let client = storage.as_mut()
        .ok_or_else(|| NightshadeError::NotConnected("PHD2".to_string()))?;

    client.clear_calibration(&which)
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to clear calibration: {}", e)))
}

/// Flip PHD2 calibration (after meridian flip)
pub async fn api_phd2_flip_calibration() -> Result<(), NightshadeError> {
    let mut storage = get_phd2_storage().write().await;
    let client = storage.as_mut()
        .ok_or_else(|| NightshadeError::NotConnected("PHD2".to_string()))?;

    client.flip_calibration()
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to flip calibration: {}", e)))
}

/// Get PHD2 calibration data
/// Returns calibration info including whether calibrated and calibration parameters
pub async fn api_phd2_get_calibration_data() -> Result<Phd2CalibrationData, NightshadeError> {
    let mut storage = get_phd2_storage().write().await;
    let client = storage.as_mut()
        .ok_or_else(|| NightshadeError::NotConnected("PHD2".to_string()))?;

    // Get calibration data for both axes - "both" returns combined info
    let result = client.get_calibration_data("both")
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to get calibration data: {}", e)))?;

    // PHD2 returns null if not calibrated, otherwise returns calibration parameters
    let is_calibrated = !result.is_null();

    // Extract calibration parameters if available
    let (ra_angle, dec_angle, ra_rate, dec_rate) = if is_calibrated {
        let xangle = result.get("xAngle").and_then(|v| v.as_f64());
        let yangle = result.get("yAngle").and_then(|v| v.as_f64());
        let xrate = result.get("xRate").and_then(|v| v.as_f64());
        let yrate = result.get("yRate").and_then(|v| v.as_f64());
        (xangle, yangle, xrate, yrate)
    } else {
        (None, None, None, None)
    };

    Ok(Phd2CalibrationData {
        is_calibrated,
        ra_angle,
        dec_angle,
        ra_rate,
        dec_rate,
    })
}

/// Find a guide star automatically
pub async fn api_phd2_find_star() -> Result<(f64, f64), NightshadeError> {
    let mut storage = get_phd2_storage().write().await;
    let client = storage.as_mut()
        .ok_or_else(|| NightshadeError::NotConnected("PHD2".to_string()))?;

    client.find_star()
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to find star: {}", e)))
}

/// Set guide star lock position
pub async fn api_phd2_set_lock_position(x: f64, y: f64, exact: bool) -> Result<(), NightshadeError> {
    let mut storage = get_phd2_storage().write().await;
    let client = storage.as_mut()
        .ok_or_else(|| NightshadeError::NotConnected("PHD2".to_string()))?;

    client.set_lock_position(x, y, exact)
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to set lock position: {}", e)))
}

/// Get current guide star lock position
pub async fn api_phd2_get_lock_position() -> Result<(f64, f64), NightshadeError> {
    let mut storage = get_phd2_storage().write().await;
    let client = storage.as_mut()
        .ok_or_else(|| NightshadeError::NotConnected("PHD2".to_string()))?;

    client.get_lock_position()
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to get lock position: {}", e)))
}

/// Start looping exposures (without guiding)
pub async fn api_phd2_loop() -> Result<(), NightshadeError> {
    let mut storage = get_phd2_storage().write().await;
    let client = storage.as_mut()
        .ok_or_else(|| NightshadeError::NotConnected("PHD2".to_string()))?;

    client.loop_exposures()
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to start looping: {}", e)))
}

/// Deselect the current guide star
pub async fn api_phd2_deselect_star() -> Result<(), NightshadeError> {
    let mut storage = get_phd2_storage().write().await;
    let client = storage.as_mut()
        .ok_or_else(|| NightshadeError::NotConnected("PHD2".to_string()))?;

    client.deselect_star()
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to deselect star: {}", e)))
}

/// Get PHD2 guide exposure time (ms)
pub async fn api_phd2_get_exposure() -> Result<u32, NightshadeError> {
    let mut storage = get_phd2_storage().write().await;
    let client = storage.as_mut()
        .ok_or_else(|| NightshadeError::NotConnected("PHD2".to_string()))?;

    client.get_exposure()
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to get exposure: {}", e)))
}

/// Set PHD2 guide exposure time (ms)
pub async fn api_phd2_set_exposure(exposure_ms: u32) -> Result<(), NightshadeError> {
    let mut storage = get_phd2_storage().write().await;
    let client = storage.as_mut()
        .ok_or_else(|| NightshadeError::NotConnected("PHD2".to_string()))?;

    client.set_exposure(exposure_ms)
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to set exposure: {}", e)))
}

/// Get current PHD2 profile name
pub async fn api_phd2_get_profile() -> Result<String, NightshadeError> {
    let mut storage = get_phd2_storage().write().await;
    let client = storage.as_mut()
        .ok_or_else(|| NightshadeError::NotConnected("PHD2".to_string()))?;

    client.get_profile()
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to get profile: {}", e)))
}

/// Launch PHD2 application
pub fn api_launch_phd2() -> Result<(), NightshadeError> {
    nightshade_imaging::launch_phd2()
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to launch PHD2: {}", e)))
}

// =============================================================================
// ALPACA DEVICE CONNECTION (Cross-platform)
// =============================================================================

pub mod alpaca_connections {
    use super::*;
    // Re-export AlpacaClient for FRB bindings
    pub use nightshade_alpaca::AlpacaClient;
    use nightshade_alpaca::{AlpacaDevice, AlpacaDeviceType};
    use std::collections::HashMap;
    
    // Storage for active Alpaca connections using Arc to share ownership
    static ALPACA_CLIENTS: OnceLock<Arc<RwLock<HashMap<String, Arc<AlpacaClient>>>>> = OnceLock::new();
    
    fn get_alpaca_clients() -> &'static Arc<RwLock<HashMap<String, Arc<AlpacaClient>>>> {
        ALPACA_CLIENTS.get_or_init(|| Arc::new(RwLock::new(HashMap::new())))
    }
    
    /// Parse an Alpaca device ID into its components
    /// Format: "alpaca:{base_url}:{device_type}:{device_number}"
    fn parse_alpaca_id(device_id: &str) -> Option<(String, AlpacaDeviceType, u32)> {
        let id_part = device_id.strip_prefix("alpaca:")?;
        
        // The format is: http://host:port:device_type:device_number
        // We need to carefully parse this since base_url contains colons
        
        // Find the last two colons which separate device_type and device_number
        let mut parts: Vec<&str> = id_part.rsplitn(3, ':').collect();
        parts.reverse();
        
        if parts.len() < 3 {
            return None;
        }
        
        let base_url = parts[0].to_string();
        let device_type = AlpacaDeviceType::from_str(parts[1])?;
        let device_number: u32 = parts[2].parse().ok()?;
        
        Some((base_url, device_type, device_number))
    }
    
    /// Connect to an Alpaca device
    pub async fn connect_alpaca_device(device_type: DeviceType, device_id: &str) -> Result<(), NightshadeError> {
        let (base_url, alpaca_type, device_number) = parse_alpaca_id(device_id)
            .ok_or_else(|| NightshadeError::InvalidDeviceId(device_id.to_string()))?;
        
        // Create the device struct
        let device = AlpacaDevice {
            device_type: alpaca_type,
            device_number,
            server_name: base_url.clone(),
            manufacturer: String::new(),
            device_name: format!("Alpaca {}", device_type.as_str()),
            unique_id: device_id.to_string(),
            base_url: base_url.clone(),
        };
        
        // Create and connect the client
        let client = AlpacaClient::new(&device);
        
        client.connect().await
            .map_err(|e| NightshadeError::ConnectionFailed(format!("Alpaca connection failed: {}", e)))?;
        
        let name = client.get_name().await.unwrap_or_else(|_| device_id.to_string());
        tracing::info!("Connected to Alpaca device: {}", name);
        
        // Store the client wrapped in Arc
        let mut clients = get_alpaca_clients().write().await;
        clients.insert(device_id.to_string(), Arc::new(client));
        
        Ok(())
    }
    
    /// Disconnect from an Alpaca device
    pub async fn disconnect_alpaca_device(device_id: &str) -> Result<(), NightshadeError> {
        let mut clients = get_alpaca_clients().write().await;
        
        if let Some(client) = clients.get(device_id) {
            client.disconnect().await
                .map_err(|e| NightshadeError::OperationFailed(format!("Alpaca disconnect failed: {}", e)))?;
        }
        
        clients.remove(device_id);
        Ok(())
    }
    
    /// Get an Alpaca client
    pub async fn get_alpaca_client(device_id: &str) -> Option<Arc<AlpacaClient>> {
        let clients = get_alpaca_clients().read().await;
        clients.get(device_id).cloned()
    }
    
    /// Check if Alpaca is connected
    pub async fn is_connected(device_id: &str) -> bool {
        let clients = get_alpaca_clients().read().await;
        if let Some(client) = clients.get(device_id) {
            client.is_connected().await.unwrap_or(false)
        } else {
            false
        }
    }
}

// =============================================================================
// REAL ASCOM DEVICE CONNECTION
// =============================================================================

// =============================================================================
// SEQUENCER API
// =============================================================================

use nightshade_sequencer::{
    SequenceDefinition, NodeDefinition, NodeType, NodeStatus,
    ExecutorState, SequenceProgress, ExecutorEvent,
    SlewConfig, CenterConfig, ExposureConfig, AutofocusConfig,
    DitherConfig, FilterConfig, CoolConfig, WarmConfig,
    RotatorConfig, WaitTimeConfig, DelayConfig, NotificationConfig,
    ScriptConfig, TargetGroupConfig, TargetHeaderConfig, LoopConfig, Binning, AutofocusMethod,
    TwilightType, LoopCondition, NotificationLevel,
    MosaicConfig, mosaic::calculate_mosaic_panels, mosaic::MosaicPanel,
};

/// Get the global sequence executor instance
fn get_sequence_executor() -> &'static std::sync::Arc<tokio::sync::RwLock<nightshade_sequencer::SequenceExecutor>> {
    nightshade_sequencer::get_executor()
}

/// Sequencer state for Flutter
#[derive(Debug, Clone)]
pub struct SequencerState {
    pub state: String,
    pub current_node_id: Option<String>,
    pub current_node_name: Option<String>,
    pub total_exposures: u32,
    pub completed_exposures: u32,
    pub total_integration_secs: f64,
    pub elapsed_secs: f64,
    pub estimated_remaining_secs: Option<f64>,
    pub current_target: Option<String>,
    pub current_filter: Option<String>,
    pub message: Option<String>,
}

impl From<SequenceProgress> for SequencerState {
    fn from(p: SequenceProgress) -> Self {
        let state_str = match p.state {
            ExecutorState::Idle => "idle",
            ExecutorState::Running => "running",
            ExecutorState::Paused => "paused",
            ExecutorState::Stopping => "stopping",
            ExecutorState::Completed => "completed",
            ExecutorState::Failed => "failed",
        };
        Self {
            state: state_str.to_string(),
            current_node_id: p.current_node_id,
            current_node_name: p.current_node_name,
            total_exposures: p.total_exposures,
            completed_exposures: p.completed_exposures,
            total_integration_secs: p.total_integration_secs,
            elapsed_secs: p.elapsed_secs,
            estimated_remaining_secs: p.estimated_remaining_secs,
            current_target: p.current_target,
            current_filter: p.current_filter,
            message: p.message,
        }
    }
}

/// Sequence definition for Flutter
#[derive(Debug, Clone)]
pub struct SequenceDefinitionApi {
    pub id: String,
    pub name: String,
    pub description: Option<String>,
    pub nodes: Vec<NodeDefinitionApi>,
    pub root_node_id: Option<String>,
}

/// Node definition for Flutter
#[derive(Debug, Clone)]
pub struct NodeDefinitionApi {
    pub id: String,
    pub name: String,
    pub node_type: String,
    pub enabled: bool,
    pub children: Vec<String>,
    pub config_json: String,
}

impl From<&NodeDefinition> for NodeDefinitionApi {
    fn from(n: &NodeDefinition) -> Self {
        let node_type = match &n.node_type {
            NodeType::TargetGroup(_) => "target_group",
            NodeType::TargetHeader(_) => "target_header",
            NodeType::Loop(_) => "loop",
            NodeType::Parallel(_) => "parallel",
            NodeType::Conditional(_) => "conditional",
            NodeType::Recovery(_) => "recovery",
            NodeType::SlewToTarget(_) => "slew",
            NodeType::CenterTarget(_) => "center",
            NodeType::TakeExposure(_) => "exposure",
            NodeType::Autofocus(_) => "autofocus",
            NodeType::Dither(_) => "dither",
            NodeType::ChangeFilter(_) => "filter_change",
            NodeType::CoolCamera(_) => "cool_camera",
            NodeType::WarmCamera(_) => "warm_camera",
            NodeType::PolarAlignment(_) => "polar_alignment",
            NodeType::MoveRotator(_) => "rotator",
            NodeType::Park => "park",
            NodeType::Unpark => "unpark",
            NodeType::WaitForTime(_) => "wait_time",
            NodeType::Delay(_) => "delay",
            NodeType::Notification(_) => "notification",
            NodeType::RunScript(_) => "script",
            NodeType::MeridianFlip(_) => "meridian_flip",
            NodeType::OpenDome(_) => "open_dome",
            NodeType::CloseDome(_) => "close_dome",
            NodeType::ParkDome(_) => "park_dome",
            NodeType::StartGuiding(_) => "start_guiding",
            NodeType::StopGuiding => "stop_guiding",
            NodeType::TemperatureCompensation(_) => "temperature_compensation",
            NodeType::Mosaic(_) => "mosaic",
            NodeType::FlatWizard(_) => "flat_wizard",
            NodeType::OpenCover(_) => "open_cover",
            NodeType::CloseCover(_) => "close_cover",
            NodeType::CalibratorOn(_) => "calibrator_on",
            NodeType::CalibratorOff(_) => "calibrator_off",
        };
        
        let config_json = serde_json::to_string(&n.node_type).unwrap_or_default();
        
        Self {
            id: n.id.clone(),
            name: n.name.clone(),
            node_type: node_type.to_string(),
            enabled: n.enabled,
            children: n.children.clone(),
            config_json,
        }
    }
}

/// Load a sequence from JSON
pub async fn api_sequencer_load_json(json: String) -> Result<(), NightshadeError> {
    tracing::info!("Loading sequence from JSON");
    
    let definition: SequenceDefinition = serde_json::from_str(&json)
        .map_err(|e| NightshadeError::InvalidInput(format!("Failed to parse sequence JSON: {}", e)))?;
    
    let mut executor = get_sequence_executor().write().await;
    executor.load_sequence(definition)
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to load sequence: {}", e)))?;
    
    tracing::info!("Sequence loaded successfully");
    Ok(())
}

/// Load a sequence from a definition struct
pub async fn api_sequencer_load(definition: SequenceDefinitionApi) -> Result<(), NightshadeError> {
    tracing::info!("Loading sequence: {}", definition.name);
    
    // Convert API nodes to internal nodes
    let nodes: Result<Vec<NodeDefinition>, NightshadeError> = definition.nodes.iter()
        .map(|n| {
            let node_type: NodeType = serde_json::from_str(&n.config_json)
                .map_err(|e| NightshadeError::InvalidInput(format!("Invalid node config: {}", e)))?;
            
            Ok(NodeDefinition {
                id: n.id.clone(),
                name: n.name.clone(),
                node_type,
                enabled: n.enabled,
                children: n.children.clone(),
            })
        })
        .collect();
    
    let internal_definition = SequenceDefinition {
        id: definition.id,
        name: definition.name,
        description: definition.description,
        nodes: nodes?,
        root_node_id: definition.root_node_id,
        metadata: std::collections::HashMap::new(),
    };
    
    let mut executor = get_sequence_executor().write().await;
    executor.load_sequence(internal_definition)
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to load sequence: {}", e)))?;

    Ok(())
}

/// Start the sequence executor
pub async fn api_sequencer_start() -> Result<(), NightshadeError> {
    tracing::info!("Starting sequence execution");
    
    let mut executor = get_sequence_executor().write().await;
    executor.start().await
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to start sequence: {}", e)))?;
    
    // Publish event
    get_state().publish_event(create_event(
        EventSeverity::Info,
        EventCategory::Sequencer,
        EventPayload::Sequencer(SequencerEvent::Started { 
            sequence_name: "Sequence".to_string() 
        }),
    ));
    
    Ok(())
}

/// Pause the sequence executor
pub async fn api_sequencer_pause() -> Result<(), NightshadeError> {
    tracing::info!("Pausing sequence execution");
    
    let executor = get_sequence_executor().read().await;
    executor.pause().await
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to pause sequence: {}", e)))?;
    
    get_state().publish_event(create_event(
        EventSeverity::Info,
        EventCategory::Sequencer,
        EventPayload::Sequencer(SequencerEvent::Paused),
    ));
    
    Ok(())
}

/// Resume the sequence executor
pub async fn api_sequencer_resume() -> Result<(), NightshadeError> {
    tracing::info!("Resuming sequence execution");
    
    let executor = get_sequence_executor().read().await;
    executor.resume().await
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to resume sequence: {}", e)))?;
    
    get_state().publish_event(create_event(
        EventSeverity::Info,
        EventCategory::Sequencer,
        EventPayload::Sequencer(SequencerEvent::Resumed),
    ));
    
    Ok(())
}

/// Stop the sequence executor
pub async fn api_sequencer_stop() -> Result<(), NightshadeError> {
    tracing::info!("Stopping sequence execution");
    
    let mut executor = get_sequence_executor().write().await;
    executor.stop().await
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to stop sequence: {}", e)))?;
    
    get_state().publish_event(create_event(
        EventSeverity::Info,
        EventCategory::Sequencer,
        EventPayload::Sequencer(SequencerEvent::Stopped),
    ));
    
    Ok(())
}

/// Skip to the next instruction
pub async fn api_sequencer_skip() -> Result<(), NightshadeError> {
    tracing::info!("Skipping current instruction");
    
    let executor = get_sequence_executor().read().await;
    executor.skip().await
        .map_err(|e| NightshadeError::OperationFailed(format!("Failed to skip: {}", e)))?;
    
    Ok(())
}

/// Reset the sequence executor
pub async fn api_sequencer_reset() -> Result<(), NightshadeError> {
    tracing::info!("Resetting sequence executor");
    
    let mut executor = get_sequence_executor().write().await;
    executor.reset().await;
    
    Ok(())
}

/// Get the current sequencer state
pub async fn api_sequencer_get_state() -> SequencerState {
    let executor = get_sequence_executor().read().await;
    let progress = executor.get_progress();
    SequencerState::from(progress)
}

/// Subscribe to sequencer events and forward them to the main event stream
pub async fn api_sequencer_subscribe_events() -> Result<(), NightshadeError> {
    let executor = get_sequence_executor().read().await;
    let mut rx = executor.subscribe();
    let state = get_state().clone();

    tracing::info!("[EVENT_SUB] Sequencer event subscription started");

    tokio::spawn(async move {
        tracing::info!("[EVENT_SUB] Event listener task spawned");
        while let Ok(event) = rx.recv().await {
            tracing::debug!("[EVENT_SUB] Received event: {:?}", std::mem::discriminant(&event));
            let nightshade_event = match &event {
                ExecutorEvent::StateChanged(s) => {
                    let _state_str = match s {
                        ExecutorState::Running => "running",
                        ExecutorState::Paused => "paused",
                        ExecutorState::Completed => "completed",
                        _ => continue,
                    };
                    Some(create_event(
                        EventSeverity::Info,
                        EventCategory::Sequencer,
                        EventPayload::Sequencer(match s {
                            ExecutorState::Paused => SequencerEvent::Paused,
                            ExecutorState::Completed => SequencerEvent::Completed,
                            _ => continue,
                        }),
                    ))
                }
                ExecutorEvent::NodeStarted { id, name } => {
                    Some(create_event(
                        EventSeverity::Info,
                        EventCategory::Sequencer,
                        EventPayload::Sequencer(SequencerEvent::NodeStarted {
                            node_id: id.clone(),
                            node_type: name.clone(),
                        }),
                    ))
                }
                ExecutorEvent::NodeCompleted { id, status } => {
                    Some(create_event(
                        EventSeverity::Info,
                        EventCategory::Sequencer,
                        EventPayload::Sequencer(SequencerEvent::NodeCompleted {
                            node_id: id.clone(),
                            success: *status == NodeStatus::Success,
                        }),
                    ))
                }
                ExecutorEvent::ProgressUpdated(progress) => {
                    Some(create_event(
                        EventSeverity::Info,
                        EventCategory::Sequencer,
                        EventPayload::Sequencer(SequencerEvent::Progress {
                            current: progress.completed_exposures,
                            total: progress.total_exposures,
                        }),
                    ))
                }
                ExecutorEvent::SequenceCompleted => {
                    Some(create_event(
                        EventSeverity::Info,
                        EventCategory::Sequencer,
                        EventPayload::Sequencer(SequencerEvent::Completed),
                    ))
                }
                ExecutorEvent::SequenceFailed { error } => {
                    Some(create_event(
                        EventSeverity::Error,
                        EventCategory::Sequencer,
                        EventPayload::Sequencer(SequencerEvent::Error {
                            message: error.clone(),
                        }),
                    ))
                }
                ExecutorEvent::ExposureStarted { frame, total, filter } => {
                    Some(create_event(
                        EventSeverity::Info,
                        EventCategory::Sequencer,
                        EventPayload::Sequencer(SequencerEvent::ExposureStarted {
                            frame: *frame,
                            total: *total,
                            filter: filter.clone(),
                            duration_secs: 0.0, // Duration not available in this event
                        }),
                    ))
                }
                ExecutorEvent::ExposureCompleted { frame, total } => {
                    Some(create_event(
                        EventSeverity::Info,
                        EventCategory::Sequencer,
                        EventPayload::Sequencer(SequencerEvent::ExposureCompleted {
                            frame: *frame,
                            total: *total,
                            duration_secs: 0.0, // Duration not available in this event
                        }),
                    ))
                }
                ExecutorEvent::TargetStarted { name, ra: _, dec: _ } => {
                    Some(create_event(
                        EventSeverity::Info,
                        EventCategory::Sequencer,
                        EventPayload::Sequencer(SequencerEvent::TargetChanged {
                            target_name: name.clone(),
                        }),
                    ))
                }
                ExecutorEvent::TargetCompleted { name } => {
                    Some(create_event(
                        EventSeverity::Info,
                        EventCategory::Sequencer,
                        EventPayload::Sequencer(SequencerEvent::TargetCompleted {
                            target_name: name.clone(),
                        }),
                    ))
                }
                ExecutorEvent::NodeProgress { node_id, instruction, progress_percent, detail } => {
                    tracing::info!("[EVENT_SUB] NodeProgress received: node={}, instruction={}, progress={}%",
                        node_id, instruction, progress_percent);
                    Some(create_event(
                        EventSeverity::Info,
                        EventCategory::Sequencer,
                        EventPayload::Sequencer(SequencerEvent::InstructionProgress {
                            node_id: node_id.clone(),
                            instruction: instruction.clone(),
                            progress_percent: *progress_percent,
                            detail: detail.clone(),
                        }),
                    ))
                }
                ExecutorEvent::Error { message } => {
                    Some(create_event(
                        EventSeverity::Error,
                        EventCategory::Sequencer,
                        EventPayload::Sequencer(SequencerEvent::Error {
                            message: message.clone(),
                        }),
                    ))
                }
                ExecutorEvent::TriggerFired { trigger_id, trigger_name, action } => {
                    tracing::info!("Trigger fired: {} ({}) - {}", trigger_name, trigger_id, action);
                    Some(create_event(
                        EventSeverity::Info,
                        EventCategory::Sequencer,
                        EventPayload::Sequencer(SequencerEvent::Error {
                            message: format!("Trigger '{}' fired: {}", trigger_name, action),
                        }),
                    ))
                }
            };

            if let Some(e) = nightshade_event {
                state.publish_event(e);
            }
        }
    });
    
    Ok(())
}

/// Stream of sequencer events (separate from main event stream for real-time progress)
#[flutter_rust_bridge::frb(ignore)]
pub fn api_sequencer_event_stream() -> impl futures::Stream<Item = String> {
    let rx = {
        let executor = get_sequence_executor().blocking_read();
        executor.subscribe()
    };
    
    async_stream::stream! {
        let mut rx = rx;
        loop {
            match rx.recv().await {
                Ok(event) => {
                    if let Ok(json) = serde_json::to_string(&event) {
                        yield json;
                    }
                }
                Err(tokio::sync::broadcast::error::RecvError::Lagged(n)) => {
                    tracing::warn!("Sequencer event stream lagged, missed {} events", n);
                }
                Err(tokio::sync::broadcast::error::RecvError::Closed) => {
                    break;
                }
            }
        }
    }
}

// =============================================================================
// SEQUENCER CHECKPOINT / CRASH RECOVERY
// =============================================================================

/// Checkpoint info returned to Dart
#[derive(Debug, Clone)]
pub struct CheckpointInfoApi {
    pub sequence_name: String,
    pub timestamp: String,
    pub completed_exposures: u32,
    pub completed_integration_secs: f64,
    pub can_resume: bool,
    pub age_seconds: i64,
}

/// Set the checkpoint directory for crash recovery
pub async fn api_sequencer_set_checkpoint_dir(path: String) -> Result<(), NightshadeError> {
    tracing::info!("Setting checkpoint directory to: {}", path);
    let mut executor = get_sequence_executor().write().await;
    executor.set_checkpoint_dir(path);
    Ok(())
}

/// Check if a recoverable checkpoint exists
pub fn api_sequencer_has_checkpoint() -> bool {
    let executor = get_sequence_executor().blocking_read();
    executor.has_recoverable_checkpoint()
}

/// Get info about the current checkpoint
pub fn api_sequencer_get_checkpoint_info() -> Option<CheckpointInfoApi> {
    let executor = get_sequence_executor().blocking_read();
    executor.get_checkpoint_info().map(|info| CheckpointInfoApi {
        sequence_name: info.sequence_name,
        timestamp: info.timestamp.to_rfc3339(),
        completed_exposures: info.completed_exposures,
        completed_integration_secs: info.completed_integration_secs,
        can_resume: info.can_resume,
        age_seconds: info.age_seconds,
    })
}

/// Resume sequence from checkpoint
pub async fn api_sequencer_resume_from_checkpoint() -> Result<(), NightshadeError> {
    tracing::info!("Resuming sequence from checkpoint");
    let mut executor = get_sequence_executor().write().await;

    // Set up device ops before resume - use UnifiedDeviceOps which routes through DeviceManager
    let ops = create_unified_device_ops();
    executor.set_device_ops(ops);

    executor.resume_from_checkpoint().await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Save current execution state as checkpoint
pub async fn api_sequencer_save_checkpoint() -> Result<(), NightshadeError> {
    tracing::info!("Saving checkpoint");
    let executor = get_sequence_executor().read().await;
    executor.save_checkpoint().await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Clear/discard checkpoint (call when sequence completes normally or user discards)
pub fn api_sequencer_clear_checkpoint() -> Result<(), NightshadeError> {
    tracing::info!("Clearing checkpoint");
    let executor = get_sequence_executor().blocking_read();
    executor.clear_checkpoint()
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Set simulation mode (use mock devices instead of real hardware)
pub async fn api_sequencer_set_simulation_mode(enabled: bool) -> Result<(), NightshadeError> {
    tracing::info!("Setting sequencer simulation mode: {}", enabled);
    let mut executor = get_sequence_executor().write().await;

    if enabled {
        // Use NullDeviceOps for simulation
        executor.set_device_ops(std::sync::Arc::new(nightshade_sequencer::NullDeviceOps));
    } else {
        // Use UnifiedDeviceOps which routes through DeviceManager for real hardware
        let ops = create_unified_device_ops();
        executor.set_device_ops(ops);
    }

    Ok(())
}

/// Set connected devices for the sequencer
pub async fn api_sequencer_set_devices(
    camera_id: Option<String>,
    mount_id: Option<String>,
    focuser_id: Option<String>,
    filterwheel_id: Option<String>,
    rotator_id: Option<String>,
) -> Result<(), NightshadeError> {
    tracing::info!(
        "Setting sequencer devices: camera={:?}, mount={:?}, focuser={:?}, filterwheel={:?}, rotator={:?}",
        camera_id, mount_id, focuser_id, filterwheel_id, rotator_id
    );
    let mut executor = get_sequence_executor().write().await;
    executor.set_devices(camera_id, mount_id, focuser_id, filterwheel_id, rotator_id);
    Ok(())
}

// =============================================================================
// SEQUENCER NODE FACTORY - Create nodes programmatically
// =============================================================================

/// Create an exposure node configuration
#[flutter_rust_bridge::frb(sync)]
pub fn api_create_exposure_node(
    id: String,
    name: String,
    duration_secs: f64,
    count: u32,
    filter: Option<String>,
    gain: Option<i32>,
    offset: Option<i32>,
    binning: i32,
    dither_every: Option<u32>,
) -> String {
    let binning_enum = match binning {
        1 => Binning::One,
        2 => Binning::Two,
        3 => Binning::Three,
        4 => Binning::Four,
        _ => Binning::One,
    };
    
    let config = ExposureConfig {
        duration_secs,
        count,
        filter,
        gain,
        offset,
        binning: binning_enum,
        dither_every,
        save_to: None,
        triggers: Vec::new(),
    };
    
    let node = NodeDefinition {
        id,
        name,
        node_type: NodeType::TakeExposure(config),
        enabled: true,
        children: vec![],
    };
    
    serde_json::to_string(&node).unwrap_or_default()
}

/// Create a slew node configuration
#[flutter_rust_bridge::frb(sync)]
pub fn api_create_slew_node(
    id: String,
    name: String,
    use_target_coords: u8,
    custom_ra: Option<f64>,
    custom_dec: Option<f64>,
) -> String {
    let config = SlewConfig {
        use_target_coords: use_target_coords != 0,
        custom_ra,
        custom_dec,
    };
    
    let node = NodeDefinition {
        id,
        name,
        node_type: NodeType::SlewToTarget(config),
        enabled: true,
        children: vec![],
    };
    
    serde_json::to_string(&node).unwrap_or_default()
}

/// Create a center node configuration
#[flutter_rust_bridge::frb(sync)]
pub fn api_create_center_node(
    id: String,
    name: String,
    use_target_coords: u8,
    accuracy_arcsec: f64,
    max_attempts: u32,
    exposure_duration: f64,
) -> String {
    let config = CenterConfig {
        use_target_coords: use_target_coords != 0,
        accuracy_arcsec,
        max_attempts,
        exposure_duration,
        filter: None,
    };
    
    let node = NodeDefinition {
        id,
        name,
        node_type: NodeType::CenterTarget(config),
        enabled: true,
        children: vec![],
    };
    
    serde_json::to_string(&node).unwrap_or_default()
}

/// Create an autofocus node configuration
#[flutter_rust_bridge::frb(sync)]
pub fn api_create_autofocus_node(
    id: String,
    name: String,
    step_size: i32,
    steps_out: u32,
    exposure_duration: f64,
    method: String,
) -> String {
    let method_enum = match method.as_str() {
        "vcurve" => AutofocusMethod::VCurve,
        "quadratic" => AutofocusMethod::Quadratic,
        "hyperbolic" => AutofocusMethod::Hyperbolic,
        _ => AutofocusMethod::VCurve,
    };
    
    let config = AutofocusConfig {
        method: method_enum,
        step_size,
        steps_out,
        exposure_duration,
        filter: None,
        binning: Binning::One,
    };
    
    let node = NodeDefinition {
        id,
        name,
        node_type: NodeType::Autofocus(config),
        enabled: true,
        children: vec![],
    };
    
    serde_json::to_string(&node).unwrap_or_default()
}

/// Create a filter change node configuration
#[flutter_rust_bridge::frb(sync)]
pub fn api_create_filter_node(
    id: String,
    name: String,
    filter_name: String,
) -> String {
    let config = FilterConfig {
        filter_name,
        filter_index: None,
    };
    
    let node = NodeDefinition {
        id,
        name,
        node_type: NodeType::ChangeFilter(config),
        enabled: true,
        children: vec![],
    };
    
    serde_json::to_string(&node).unwrap_or_default()
}

/// Create a target group node configuration (legacy - use target_header instead)
#[flutter_rust_bridge::frb(sync)]
pub fn api_create_target_group_node(
    id: String,
    name: String,
    target_name: String,
    ra_hours: f64,
    dec_degrees: f64,
    rotation: Option<f64>,
    min_altitude: Option<f64>,
    max_altitude: Option<f64>,
    priority: i32,
    children: Vec<String>,
) -> String {
    let config = TargetGroupConfig {
        target_name,
        ra_hours,
        dec_degrees,
        rotation,
        min_altitude,
        max_altitude,
        priority,
        ..Default::default()
    };

    let node = NodeDefinition {
        id,
        name,
        node_type: NodeType::TargetGroup(config),
        enabled: true,
        children,
    };

    serde_json::to_string(&node).unwrap_or_default()
}

/// Create a target header node configuration
#[flutter_rust_bridge::frb(sync)]
pub fn api_create_target_header_node(
    id: String,
    name: String,
    target_name: String,
    ra_hours: f64,
    dec_degrees: f64,
    rotation: Option<f64>,
    min_altitude: Option<f64>,
    max_altitude: Option<f64>,
    priority: i32,
    start_after: Option<i64>,
    end_before: Option<i64>,
    mosaic_panel_json: Option<String>,
    children: Vec<String>,
) -> String {
    let mosaic_panel = mosaic_panel_json
        .and_then(|json| serde_json::from_str(&json).ok());

    let config = TargetHeaderConfig {
        target_name,
        ra_hours,
        dec_degrees,
        rotation,
        min_altitude,
        max_altitude,
        priority,
        start_after,
        end_before,
        mosaic_panel,
    };

    let node = NodeDefinition {
        id,
        name,
        node_type: NodeType::TargetHeader(config),
        enabled: true,
        children,
    };

    serde_json::to_string(&node).unwrap_or_default()
}

/// Create a loop node configuration
#[flutter_rust_bridge::frb(sync)]
pub fn api_create_loop_node(
    id: String,
    name: String,
    iterations: Option<u32>,
    condition: String,
    children: Vec<String>,
) -> String {
    let condition_enum = match condition.as_str() {
        "count" => LoopCondition::Count,
        "until_time" => LoopCondition::UntilTime,
        "altitude_below" => LoopCondition::AltitudeBelow,
        "altitude_above" => LoopCondition::AltitudeAbove,
        "integration_time" => LoopCondition::IntegrationTime,
        _ => LoopCondition::Count,
    };
    
    let config = LoopConfig {
        iterations,
        condition: condition_enum,
        condition_value: None,
    };
    
    let node = NodeDefinition {
        id,
        name,
        node_type: NodeType::Loop(config),
        enabled: true,
        children,
    };
    
    serde_json::to_string(&node).unwrap_or_default()
}

/// Create a delay node configuration
#[flutter_rust_bridge::frb(sync)]
pub fn api_create_delay_node(
    id: String,
    name: String,
    seconds: f64,
) -> String {
    let config = DelayConfig { seconds };
    
    let node = NodeDefinition {
        id,
        name,
        node_type: NodeType::Delay(config),
        enabled: true,
        children: vec![],
    };
    
    serde_json::to_string(&node).unwrap_or_default()
}

/// Create a park node configuration
#[flutter_rust_bridge::frb(sync)]
pub fn api_create_park_node(id: String, name: String) -> String {
    let node = NodeDefinition {
        id,
        name,
        node_type: NodeType::Park,
        enabled: true,
        children: vec![],
    };
    
    serde_json::to_string(&node).unwrap_or_default()
}

/// Create an unpark node configuration
#[flutter_rust_bridge::frb(sync)]
pub fn api_create_unpark_node(id: String, name: String) -> String {
    let node = NodeDefinition {
        id,
        name,
        node_type: NodeType::Unpark,
        enabled: true,
        children: vec![],
    };
    
    serde_json::to_string(&node).unwrap_or_default()
}

/// Create a cool camera node configuration
#[flutter_rust_bridge::frb(sync)]
pub fn api_create_cool_camera_node(
    id: String,
    name: String,
    target_temp: f64,
    duration_mins: Option<f64>,
) -> String {
    let config = CoolConfig {
        target_temp,
        duration_mins,
    };
    
    let node = NodeDefinition {
        id,
        name,
        node_type: NodeType::CoolCamera(config),
        enabled: true,
        children: vec![],
    };
    
    serde_json::to_string(&node).unwrap_or_default()
}

/// Create a warm camera node configuration
#[flutter_rust_bridge::frb(sync)]
pub fn api_create_warm_camera_node(
    id: String,
    name: String,
    rate_per_min: f64,
) -> String {
    let config = WarmConfig { rate_per_min };
    
    let node = NodeDefinition {
        id,
        name,
        node_type: NodeType::WarmCamera(config),
        enabled: true,
        children: vec![],
    };
    
    serde_json::to_string(&node).unwrap_or_default()
}

/// Create a dither node configuration
#[flutter_rust_bridge::frb(sync)]
pub fn api_create_dither_node(
    id: String,
    name: String,
    pixels: f64,
    settle_pixels: f64,
    settle_time: f64,
    settle_timeout: f64,
    ra_only: u8,
) -> String {
    let config = DitherConfig {
        pixels,
        settle_pixels,
        settle_time,
        settle_timeout,
        ra_only: ra_only != 0,
    };
    
    let node = NodeDefinition {
        id,
        name,
        node_type: NodeType::Dither(config),
        enabled: true,
        children: vec![],
    };
    
    serde_json::to_string(&node).unwrap_or_default()
}

/// Create a wait time node configuration
#[flutter_rust_bridge::frb(sync)]
pub fn api_create_wait_time_node(
    id: String,
    name: String,
    wait_until: Option<i64>,
    twilight_type: Option<String>,
) -> String {
    let twilight = twilight_type.and_then(|t| match t.as_str() {
        "civil" => Some(TwilightType::Civil),
        "nautical" => Some(TwilightType::Nautical),
        "astronomical" => Some(TwilightType::Astronomical),
        _ => None,
    });
    
    let config = WaitTimeConfig {
        wait_until,
        wait_for_twilight: twilight,
    };
    
    let node = NodeDefinition {
        id,
        name,
        node_type: NodeType::WaitForTime(config),
        enabled: true,
        children: vec![],
    };
    
    serde_json::to_string(&node).unwrap_or_default()
}

/// Create a notification node configuration
#[flutter_rust_bridge::frb(sync)]
pub fn api_create_notification_node(
    id: String,
    name: String,
    title: String,
    message: String,
    level: String,
) -> String {
    let level_enum = match level.as_str() {
        "info" => NotificationLevel::Info,
        "warning" => NotificationLevel::Warning,
        "error" => NotificationLevel::Error,
        "success" => NotificationLevel::Success,
        _ => NotificationLevel::Info,
    };
    
    let config = NotificationConfig {
        title,
        message,
        level: level_enum,
    };
    
    let node = NodeDefinition {
        id,
        name,
        node_type: NodeType::Notification(config),
        enabled: true,
        children: vec![],
    };
    
    serde_json::to_string(&node).unwrap_or_default()
}

/// Create a script node configuration
#[flutter_rust_bridge::frb(sync)]
pub fn api_create_script_node(
    id: String,
    name: String,
    script_path: String,
    arguments: Vec<String>,
    timeout_secs: Option<u32>,
) -> String {
    let config = ScriptConfig {
        script_path,
        arguments,
        timeout_secs,
    };
    
    let node = NodeDefinition {
        id,
        name,
        node_type: NodeType::RunScript(config),
        enabled: true,
        children: vec![],
    };
    
    serde_json::to_string(&node).unwrap_or_default()
}

/// Create a rotator node configuration
#[flutter_rust_bridge::frb(sync)]
pub fn api_create_rotator_node(
    id: String,
    name: String,
    target_angle: f64,
    relative: u8,
) -> String {
    let config = RotatorConfig {
        target_angle,
        relative: relative != 0,
    };
    
    let node = NodeDefinition {
        id,
        name,
        node_type: NodeType::MoveRotator(config),
        enabled: true,
        children: vec![],
    };
    
    serde_json::to_string(&node).unwrap_or_default()
}

/// Build a complete sequence definition from nodes
#[flutter_rust_bridge::frb(sync)]
pub fn api_build_sequence(
    id: String,
    name: String,
    description: Option<String>,
    node_jsons: Vec<String>,
    root_node_id: Option<String>,
) -> String {
    let nodes: Vec<NodeDefinition> = node_jsons.iter()
        .filter_map(|json| serde_json::from_str(json).ok())
        .collect();
    
    let definition = SequenceDefinition {
        id,
        name,
        description,
        nodes,
        root_node_id,
        metadata: std::collections::HashMap::new(),
    };
    
    serde_json::to_string(&definition).unwrap_or_default()
}

// =============================================================================
// Mosaic Calculation
// =============================================================================

/// Result structure for mosaic panel calculations (FFI-safe)
#[derive(Debug, Clone)]
pub struct MosaicPanelResult {
    pub ra_hours: f64,
    pub dec_degrees: f64,
    pub panel_index: u32,
    pub row: u32,
    pub col: u32,
}

impl From<MosaicPanel> for MosaicPanelResult {
    fn from(panel: MosaicPanel) -> Self {
        Self {
            ra_hours: panel.ra_hours,
            dec_degrees: panel.dec_degrees,
            panel_index: panel.panel_index,
            row: panel.row,
            col: panel.col,
        }
    }
}

/// Calculate mosaic panel positions given center coordinates and configuration
///
/// # Arguments
/// * `center_ra` - Center RA in hours (0-24)
/// * `center_dec` - Center Dec in degrees (-90 to +90)
/// * `panel_width_arcmin` - Panel width in arcminutes
/// * `panel_height_arcmin` - Panel height in arcminutes
/// * `overlap_percent` - Overlap percentage (0-50)
/// * `rotation` - Rotation angle in degrees
/// * `panels_horizontal` - Number of horizontal panels
/// * `panels_vertical` - Number of vertical panels
///
/// # Returns
/// Vector of MosaicPanelResult with calculated RA/Dec for each panel
#[flutter_rust_bridge::frb(sync)]
pub fn api_calculate_mosaic_panels(
    center_ra: f64,
    center_dec: f64,
    panel_width_arcmin: f64,
    panel_height_arcmin: f64,
    overlap_percent: f64,
    rotation: f64,
    panels_horizontal: u32,
    panels_vertical: u32,
) -> Vec<MosaicPanelResult> {
    let config = MosaicConfig {
        center_ra,
        center_dec,
        panel_width_arcmin,
        panel_height_arcmin,
        overlap_percent,
        rotation,
        panels_horizontal,
        panels_vertical,
    };

    calculate_mosaic_panels(&config)
        .into_iter()
        .map(MosaicPanelResult::from)
        .collect()
}

/// Calculate total mosaic coverage area in square degrees
#[flutter_rust_bridge::frb(sync)]
pub fn api_calculate_mosaic_area(
    panel_width_arcmin: f64,
    panel_height_arcmin: f64,
    panels_horizontal: u32,
    panels_vertical: u32,
) -> f64 {
    let total_width_arcmin = panel_width_arcmin * panels_horizontal as f64;
    let total_height_arcmin = panel_height_arcmin * panels_vertical as f64;
    // Return in square degrees
    (total_width_arcmin / 60.0) * (total_height_arcmin / 60.0)
}

/// Estimate total imaging time for mosaic in seconds
///
/// # Arguments
/// * `total_panels` - Total number of panels
/// * `exposure_secs` - Exposure time per frame
/// * `exposures_per_panel` - Number of exposures per panel
/// * `overhead_per_panel_secs` - Overhead per panel (slew, center, settle) - defaults to 60s if 0
#[flutter_rust_bridge::frb(sync)]
pub fn api_estimate_mosaic_time(
    total_panels: u32,
    exposure_secs: f64,
    exposures_per_panel: u32,
    overhead_per_panel_secs: f64,
) -> f64 {
    let overhead = if overhead_per_panel_secs <= 0.0 { 60.0 } else { overhead_per_panel_secs };
    let time_per_panel = exposure_secs * exposures_per_panel as f64 + overhead;
    total_panels as f64 * time_per_panel
}

/// Calculate altitude for a target at a specific time and observer location
///
/// # Arguments
/// * `ra_hours` - Right Ascension in hours (0-24)
/// * `dec_degrees` - Declination in degrees (-90 to +90)
/// * `latitude` - Observer's latitude in degrees (-90 to +90, positive is north)
/// * `longitude` - Observer's longitude in degrees (-180 to +180, positive is east)
/// * `time_unix_millis` - UTC time as Unix timestamp in milliseconds
///
/// # Returns
/// Altitude in degrees above the horizon (-90 to +90)
#[flutter_rust_bridge::frb(sync)]
pub fn api_calculate_altitude(
    ra_hours: f64,
    dec_degrees: f64,
    latitude: f64,
    longitude: f64,
    time_unix_millis: i64,
) -> f64 {
    use chrono::{DateTime, Utc, TimeZone};

    // Convert Unix milliseconds to DateTime<Utc>
    let time = Utc.timestamp_millis_opt(time_unix_millis).single()
        .unwrap_or_else(|| Utc::now());

    nightshade_sequencer::meridian::calculate_altitude(
        ra_hours,
        dec_degrees,
        latitude,
        longitude,
        time,
    )
}

// =============================================================================
// Polar Alignment
// =============================================================================

use std::sync::atomic::{AtomicBool as PolarAtomicBool, Ordering as PolarOrdering};

/// Track whether polar alignment is running
static POLAR_ALIGN_RUNNING: OnceLock<PolarAtomicBool> = OnceLock::new();
static POLAR_ALIGN_CANCEL: OnceLock<PolarAtomicBool> = OnceLock::new();

fn get_polar_align_flag() -> &'static PolarAtomicBool {
    POLAR_ALIGN_RUNNING.get_or_init(|| PolarAtomicBool::new(false))
}

fn get_polar_align_cancel() -> &'static PolarAtomicBool {
    POLAR_ALIGN_CANCEL.get_or_init(|| PolarAtomicBool::new(false))
}

/// Emit a polar alignment status update (JSON-serializable for Dart)
fn emit_polar_status(status: &str, phase: &str, point: i32) {
    tracing::info!("Polar alignment: {} (phase={}, point={})", status, phase, point);
    get_state().publish_event(create_event(
        EventSeverity::Info,
        EventCategory::PolarAlignment,
        EventPayload::PolarAlignmentStatus(PolarAlignmentStatus {
            status: status.to_string(),
            phase: phase.to_string(),
            point,
        }),
    ));
}

/// Emit polar alignment error update
fn emit_polar_error(az: f64, alt: f64, total: f64, cur_ra: f64, cur_dec: f64, tgt_ra: f64, tgt_dec: f64) {
    get_state().publish_event(create_event(
        EventSeverity::Info,
        EventCategory::PolarAlignment,
        EventPayload::PolarAlignment(PolarAlignmentEvent {
            azimuth_error: az,
            altitude_error: alt,
            total_error: total,
            current_ra: cur_ra,
            current_dec: cur_dec,
            target_ra: tgt_ra,
            target_dec: tgt_dec,
        }),
    ));
}

/// Start three-point polar alignment
///
/// This initiates the polar alignment process which will:
/// 1. Capture 3 images at different mount rotations
/// 2. Plate solve each image
/// 3. Calculate the center of rotation
/// 4. Enter adjustment mode with real-time error updates
///
/// Note: Requires connected camera and mount devices.
pub async fn api_start_polar_alignment(
    exposure_time: f64,
    step_size: f64,
    binning: i32,
    is_north: bool,
    manual_rotation: bool,
    rotate_east: bool,
) -> Result<(), NightshadeError> {
    // Check if already running
    if get_polar_align_flag().load(PolarOrdering::Relaxed) {
        return Err(NightshadeError::OperationFailed("Polar alignment already running".to_string()));
    }

    get_polar_align_flag().store(true, PolarOrdering::Relaxed);
    get_polar_align_cancel().store(false, PolarOrdering::Relaxed);

    tracing::info!(
        "Starting polar alignment: exposure={}s, step={}°, binning={}, north={}, manual={}, east={}",
        exposure_time, step_size, binning, is_north, manual_rotation, rotate_east
    );

    // Get connected devices using existing API
    let connected = api_get_connected_devices().await;

    // Find connected camera
    let camera_id = connected.iter()
        .find(|d| d.device_type == DeviceType::Camera)
        .map(|d| d.id.clone());

    // Find connected mount
    let mount_id = connected.iter()
        .find(|d| d.device_type == DeviceType::Mount)
        .map(|d| d.id.clone());

    let camera_id = camera_id.ok_or_else(|| {
        get_polar_align_flag().store(false, PolarOrdering::Relaxed);
        NightshadeError::DeviceNotFound("No camera connected".to_string())
    })?;

    let mount_id = mount_id.ok_or_else(|| {
        get_polar_align_flag().store(false, PolarOrdering::Relaxed);
        NightshadeError::DeviceNotFound("No mount connected".to_string())
    })?;

    // Spawn background task for polar alignment
    tokio::spawn(async move {
        let result = run_polar_alignment(
            camera_id,
            mount_id,
            exposure_time,
            step_size,
            binning,
            is_north,
            manual_rotation,
            rotate_east,
        ).await;

        if let Err(e) = result {
            tracing::error!("Polar alignment failed: {}", e);
            emit_polar_status(&format!("Error: {}", e), "error", 0);
        }

        get_polar_align_flag().store(false, PolarOrdering::Relaxed);
    });

    Ok(())
}

/// Internal function to run the polar alignment process
async fn run_polar_alignment(
    camera_id: String,
    mount_id: String,
    exposure_time: f64,
    step_size: f64,
    binning: i32,
    is_north: bool,
    manual_rotation: bool,
    rotate_east: bool,
) -> Result<(), String> {
    let mut solved_points: Vec<(f64, f64)> = Vec::new();

    // Phase 1: Capture and solve 3 points
    for point in 1..=3 {
        // Check for cancellation
        if get_polar_align_cancel().load(PolarOrdering::Relaxed) {
            emit_polar_status("Cancelled by user", "idle", 0);
            return Ok(());
        }

        emit_polar_status(&format!("Capturing point {}/3...", point), "measuring", point as i32);

        // Capture image using existing API
        // api_camera_start_exposure(device_id, duration_secs, gain, offset, bin_x, bin_y)
        api_camera_start_exposure(
            camera_id.clone(),
            exposure_time,
            0, // gain (auto)
            0, // offset (auto)
            binning,
            binning,
        ).await.map_err(|e| format!("Failed to capture: {:?}", e))?;

        // Wait for exposure to complete
        tokio::time::sleep(tokio::time::Duration::from_secs_f64(exposure_time + 2.0)).await;

        if get_polar_align_cancel().load(PolarOrdering::Relaxed) {
            emit_polar_status("Cancelled by user", "idle", 0);
            return Ok(());
        }

        emit_polar_status(&format!("Plate solving point {}/3...", point), "measuring", point as i32);

        // Get the captured image
        let image = api_get_last_image().await
            .map_err(|e| format!("Failed to get image: {:?}", e))?;

        // Emit image ready event so UI can display preview
        get_state().publish_event(create_event(
            EventSeverity::Info,
            EventCategory::Imaging,
            EventPayload::Imaging(ImagingEvent::ImageReady {
                width: image.width,
                height: image.height,
            }),
        ));

        // Save temp file for plate solving
        let temp_dir = std::env::temp_dir();
        let temp_path = temp_dir.join(format!("polar_align_{}.fits", point));
        let temp_path_str = temp_path.to_string_lossy().to_string();

        // Write FITS file for plate solving
        if let Err(e) = write_temp_fits_for_solve(&image, &temp_path_str) {
            return Err(format!("Failed to write temp FITS: {}", e));
        }

        // Plate solve with 60 second timeout
        let solve_future = api_plate_solve_blind(temp_path_str.clone());
        let solve_result = match tokio::time::timeout(
            tokio::time::Duration::from_secs(60),
            solve_future
        ).await {
            Ok(Ok(result)) => result,
            Ok(Err(e)) => {
                let _ = std::fs::remove_file(&temp_path);
                return Err(format!("Plate solve error: {:?}", e));
            }
            Err(_) => {
                let _ = std::fs::remove_file(&temp_path);
                return Err(format!("Plate solve timed out after 60 seconds for point {}", point));
            }
        };

        // Clean up temp file
        let _ = std::fs::remove_file(&temp_path);

        if solve_result.success {
            solved_points.push((solve_result.ra * 15.0, solve_result.dec)); // RA hours to degrees
            tracing::info!("Point {} solved: RA={:.4}h ({:.4}°), Dec={:.4}°",
                point, solve_result.ra, solve_result.ra * 15.0, solve_result.dec);
        } else {
            return Err(format!("Plate solve failed for point {}: {:?}", point, solve_result.error));
        }

        // Rotate mount for next point (if not last point)
        if point < 3 {
            if manual_rotation {
                emit_polar_status(
                    &format!("Rotate mount {}° and wait...", step_size as i32),
                    "measuring",
                    point as i32,
                );
                // Wait for user to rotate manually
                tokio::time::sleep(tokio::time::Duration::from_secs(15)).await;
            } else {
                emit_polar_status(&format!("Slewing to point {}...", point + 1), "measuring", point as i32);

                // Calculate new position (in degrees)
                let (current_ra_deg, current_dec) = solved_points.last().unwrap();
                let move_amount = if rotate_east { step_size } else { -step_size };
                let target_ra_deg = (current_ra_deg + move_amount + 360.0) % 360.0;

                // Slew mount (API takes RA in hours, Dec in degrees)
                api_mount_slew_to_coordinates(mount_id.clone(), target_ra_deg / 15.0, *current_dec).await
                    .map_err(|e| format!("Failed to slew: {:?}", e))?;

                // Wait for slew to complete
                tokio::time::sleep(tokio::time::Duration::from_secs(5)).await;
            }
        }
    }

    // Phase 2: Calculate center of rotation
    emit_polar_status("Calculating polar alignment error...", "adjusting", 3);

    let (center_ra, center_dec) = calculate_rotation_center(&solved_points);
    let pole_dec = if is_north { 90.0 } else { -90.0 };

    tracing::info!("Rotation center: RA={:.4}°, Dec={:.4}°", center_ra, center_dec);

    // Phase 3: Adjustment loop - continuously update error
    emit_polar_status("Adjustment mode - make corrections", "adjusting", 0);

    let mut consecutive_failures = 0;
    const MAX_FAILURES: i32 = 5;

    loop {
        if get_polar_align_cancel().load(PolarOrdering::Relaxed) {
            emit_polar_status("Stopped", "idle", 0);
            return Ok(());
        }

        // Capture and solve to get current position
        emit_polar_status("Capturing...", "adjusting", 0);
        if let Err(e) = api_camera_start_exposure(
            camera_id.clone(),
            exposure_time,
            0, // gain (auto)
            0, // offset (auto)
            binning,
            binning,
        ).await {
            consecutive_failures += 1;
            tracing::warn!("Capture failed in adjustment loop: {:?}", e);
            emit_polar_status(&format!("Capture failed: {:?} (retry {}/{})", e, consecutive_failures, MAX_FAILURES), "adjusting", 0);
            if consecutive_failures >= MAX_FAILURES {
                return Err(format!("Too many consecutive failures ({}) in adjustment loop", MAX_FAILURES));
            }
            tokio::time::sleep(tokio::time::Duration::from_secs(2)).await;
            continue;
        }

        tokio::time::sleep(tokio::time::Duration::from_secs_f64(exposure_time + 2.0)).await;

        if get_polar_align_cancel().load(PolarOrdering::Relaxed) {
            emit_polar_status("Stopped", "idle", 0);
            return Ok(());
        }

        // Get the captured image
        let image = match api_get_last_image().await {
            Ok(img) => img,
            Err(e) => {
                consecutive_failures += 1;
                tracing::warn!("Failed to get image in adjustment loop: {:?}", e);
                emit_polar_status(&format!("Image retrieval failed (retry {}/{})", consecutive_failures, MAX_FAILURES), "adjusting", 0);
                if consecutive_failures >= MAX_FAILURES {
                    return Err(format!("Too many consecutive failures ({}) in adjustment loop", MAX_FAILURES));
                }
                tokio::time::sleep(tokio::time::Duration::from_secs(2)).await;
                continue;
            }
        };

        // Emit image ready event so UI can display preview
        get_state().publish_event(create_event(
            EventSeverity::Info,
            EventCategory::Imaging,
            EventPayload::Imaging(ImagingEvent::ImageReady {
                width: image.width,
                height: image.height,
            }),
        ));

        let temp_dir = std::env::temp_dir();
        let temp_path = temp_dir.join("polar_align_adjust.fits");
        let temp_path_str = temp_path.to_string_lossy().to_string();

        if let Err(e) = write_temp_fits_for_solve(&image, &temp_path_str) {
            consecutive_failures += 1;
            tracing::warn!("Failed to write temp FITS: {}", e);
            emit_polar_status(&format!("FITS write failed (retry {}/{})", consecutive_failures, MAX_FAILURES), "adjusting", 0);
            if consecutive_failures >= MAX_FAILURES {
                return Err(format!("Too many consecutive failures ({}) in adjustment loop", MAX_FAILURES));
            }
            tokio::time::sleep(tokio::time::Duration::from_secs(2)).await;
            continue;
        }

        emit_polar_status("Solving...", "adjusting", 0);

        // Plate solve with 30 second timeout (shorter for adjustment loop)
        let solve_future = api_plate_solve_blind(temp_path_str.clone());
        let solve_result = match tokio::time::timeout(
            tokio::time::Duration::from_secs(30),
            solve_future
        ).await {
            Ok(Ok(result)) => {
                let _ = std::fs::remove_file(&temp_path);
                result
            }
            Ok(Err(e)) => {
                let _ = std::fs::remove_file(&temp_path);
                consecutive_failures += 1;
                tracing::warn!("Plate solve error in adjustment loop: {:?}", e);
                emit_polar_status(&format!("Solve failed: {:?} (retry {}/{})", e, consecutive_failures, MAX_FAILURES), "adjusting", 0);
                if consecutive_failures >= MAX_FAILURES {
                    return Err(format!("Too many consecutive failures ({}) in adjustment loop", MAX_FAILURES));
                }
                tokio::time::sleep(tokio::time::Duration::from_secs(2)).await;
                continue;
            }
            Err(_) => {
                let _ = std::fs::remove_file(&temp_path);
                consecutive_failures += 1;
                tracing::warn!("Plate solve timed out in adjustment loop");
                emit_polar_status(&format!("Solve timed out (retry {}/{})", consecutive_failures, MAX_FAILURES), "adjusting", 0);
                if consecutive_failures >= MAX_FAILURES {
                    return Err(format!("Too many consecutive failures ({}) in adjustment loop", MAX_FAILURES));
                }
                tokio::time::sleep(tokio::time::Duration::from_secs(2)).await;
                continue;
            }
        };

        if solve_result.success {
            // Reset failure counter on success
            consecutive_failures = 0;

            // Calculate error relative to calculated pole position
            let alt_error = (pole_dec - center_dec) * 60.0; // arcminutes
            let az_error = (0.0 - center_ra) * center_dec.to_radians().cos() * 60.0;
            let total_error = (az_error.powi(2) + alt_error.powi(2)).sqrt();

            emit_polar_status("Adjusting - make corrections", "adjusting", 0);
            emit_polar_error(
                az_error,
                alt_error,
                total_error,
                solve_result.ra * 15.0, // hours to degrees
                solve_result.dec,
                center_ra,
                pole_dec,
            );
        } else {
            consecutive_failures += 1;
            emit_polar_status(&format!("Solve unsuccessful (retry {}/{})", consecutive_failures, MAX_FAILURES), "adjusting", 0);
            if consecutive_failures >= MAX_FAILURES {
                return Err(format!("Too many consecutive failures ({}) in adjustment loop", MAX_FAILURES));
            }
        }

        // Brief pause before next update
        tokio::time::sleep(tokio::time::Duration::from_secs(1)).await;
    }
}

/// Helper to write a temp FITS file for plate solving
fn write_temp_fits_for_solve(image: &CapturedImageResult, path: &str) -> Result<(), String> {
    use nightshade_imaging::{ImageData, PixelType, write_fits, FitsHeader};
    use std::path::Path;

    // Convert display_data to raw bytes for FITS
    // The display_data is 8-bit, scale to 16-bit for better plate solving
    let raw_bytes: Vec<u8> = if image.is_color {
        // For color, convert to grayscale (luminance) and scale to 16-bit
        image.display_data.chunks(3)
            .flat_map(|rgb| {
                let lum = ((rgb[0] as u32 + rgb[1] as u32 + rgb[2] as u32) / 3) as u16 * 256;
                lum.to_le_bytes().to_vec()
            })
            .collect()
    } else {
        // Scale 8-bit to 16-bit
        image.display_data.iter()
            .flat_map(|&v| {
                let scaled = (v as u16) * 256;
                scaled.to_le_bytes().to_vec()
            })
            .collect()
    };

    let mut image_data = ImageData::new(
        image.width as u32,
        image.height as u32,
        1, // grayscale
        PixelType::U16,
    );
    image_data.data = raw_bytes;

    let header = FitsHeader::new();

    write_fits(Path::new(path), &image_data, &header)
        .map_err(|e| format!("FITS write error: {:?}", e))
}

/// Calculate the center of rotation from 3 solved points using 3D plane fitting
fn calculate_rotation_center(points: &[(f64, f64)]) -> (f64, f64) {
    if points.len() < 3 {
        return (0.0, 90.0);
    }

    // Convert spherical (RA, Dec) to Cartesian unit vectors
    let vectors: Vec<(f64, f64, f64)> = points.iter().map(|(ra, dec)| {
        let ra_rad = ra.to_radians();
        let dec_rad = dec.to_radians();
        (
            dec_rad.cos() * ra_rad.cos(),
            dec_rad.cos() * ra_rad.sin(),
            dec_rad.sin()
        )
    }).collect();

    // The three points define a plane. The rotation axis is the normal to this plane.
    let p1 = vectors[0];
    let p2 = vectors[1];
    let p3 = vectors[2];

    let v1 = (p2.0 - p1.0, p2.1 - p1.1, p2.2 - p1.2);
    let v2 = (p3.0 - p1.0, p3.1 - p1.1, p3.2 - p1.2);

    // Cross product for normal
    let nx = v1.1 * v2.2 - v1.2 * v2.1;
    let ny = v1.2 * v2.0 - v1.0 * v2.2;
    let nz = v1.0 * v2.1 - v1.1 * v2.0;

    // Normalize
    let mag = (nx * nx + ny * ny + nz * nz).sqrt();
    if mag < 1e-9 {
        return (0.0, 90.0);
    }

    let nx = nx / mag;
    let ny = ny / mag;
    let nz = nz / mag;

    // Convert back to RA/Dec
    let center_dec_rad = nz.asin();
    let mut center_ra_rad = ny.atan2(nx);

    if center_ra_rad < 0.0 {
        center_ra_rad += 2.0 * std::f64::consts::PI;
    }

    (center_ra_rad.to_degrees(), center_dec_rad.to_degrees())
}

/// Stop the polar alignment process
pub async fn api_stop_polar_alignment() -> Result<(), NightshadeError> {
    if !get_polar_align_flag().load(PolarOrdering::Relaxed) {
        return Ok(()); // Already stopped
    }

    // Signal cancellation
    get_polar_align_cancel().store(true, PolarOrdering::Relaxed);

    tracing::info!("Stopping polar alignment");

    // Give the background task time to clean up
    tokio::time::sleep(tokio::time::Duration::from_millis(500)).await;

    get_polar_align_flag().store(false, PolarOrdering::Relaxed);

    emit_polar_status("Stopped", "idle", 0);

    Ok(())
}

// =============================================================================
// Equipment Profiles
// =============================================================================

/// Initialize profile storage
#[flutter_rust_bridge::frb(sync)]
pub fn api_init_profile_storage(storage_path: String) -> Result<(), NightshadeError> {
    crate::state::init_profile_storage(std::path::PathBuf::from(storage_path))
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Get all equipment profiles
#[flutter_rust_bridge::frb(sync)]
pub fn api_get_profiles() -> Result<Vec<EquipmentProfile>, NightshadeError> {
    get_state().load_profiles()
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Save an equipment profile
#[flutter_rust_bridge::frb(sync)]
pub fn api_save_profile(profile: EquipmentProfile) -> Result<(), NightshadeError> {
    get_state().save_profile_to_storage(&profile)
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Delete an equipment profile
#[flutter_rust_bridge::frb(sync)]
pub fn api_delete_profile(profile_id: String) -> Result<(), NightshadeError> {
    get_state().delete_profile_from_storage(&profile_id)
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Load a profile and set as active
pub async fn api_load_profile(profile_id: String) -> Result<(), NightshadeError> {
    get_state().load_and_set_profile(&profile_id).await
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Get the currently active profile
pub async fn api_get_active_profile() -> Result<Option<EquipmentProfile>, NightshadeError> {
    Ok(get_state().get_profile().await)
}

// =============================================================================
// Settings & Location
// =============================================================================

/// Initialize settings storage and load observer location into memory
#[flutter_rust_bridge::frb(sync)]
pub fn api_init_settings_storage(storage_path: String) -> Result<(), NightshadeError> {
    crate::state::init_settings_storage(std::path::PathBuf::from(storage_path))
        .map_err(|e| NightshadeError::OperationFailed(e))?;

    // Load observer location from persisted settings into in-memory state
    // This ensures the sequencer and other Rust components have access to location
    get_state().load_observer_location_from_settings();

    Ok(())
}

/// Get application settings
#[flutter_rust_bridge::frb(sync)]
pub fn api_get_settings() -> Result<AppSettings, NightshadeError> {
    get_state().get_settings()
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Update application settings
#[flutter_rust_bridge::frb(sync)]
pub fn api_update_settings(settings: AppSettings) -> Result<(), NightshadeError> {
    get_state().update_settings(&settings)
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Get observer location
#[flutter_rust_bridge::frb(sync)]
pub fn api_get_location() -> Result<Option<ObserverLocation>, NightshadeError> {
    get_state().get_observer_location()
        .map_err(|e| NightshadeError::OperationFailed(e))
}

/// Set observer location
#[flutter_rust_bridge::frb(sync)]
pub fn api_set_location(location: Option<ObserverLocation>) -> Result<(), NightshadeError> {
    // Using eprintln! to ensure we see this in stderr regardless of tracing config
    match &location {
        Some(loc) => {
            eprintln!("[RUST-API] api_set_location called with lat={}, lon={}, elev={}",
                loc.latitude, loc.longitude, loc.elevation);
            tracing::info!("[API] api_set_location called with lat={}, lon={}, elev={}",
                loc.latitude, loc.longitude, loc.elevation);
        }
        None => {
            eprintln!("[RUST-API] api_set_location called with None");
            tracing::info!("[API] api_set_location called with None");
        }
    }
    let result = get_state().set_observer_location(location);
    match &result {
        Ok(_) => {
            eprintln!("[RUST-API] api_set_location succeeded");
            tracing::info!("[API] api_set_location succeeded");
        }
        Err(ref e) => {
            eprintln!("[RUST-API] api_set_location failed: {}", e);
            tracing::error!("[API] api_set_location failed: {}", e);
        }
    }
    result.map_err(|e| NightshadeError::OperationFailed(e))
}

// =============================================================================
// FITS File Saving
// =============================================================================

/// Header data for FITS file writing
#[derive(Debug, Clone)]
pub struct FitsWriteHeader {
    pub object_name: Option<String>,
    pub exposure_time: f64,
    pub capture_timestamp: String,
    pub frame_type: String,
    pub filter: Option<String>,
    pub gain: Option<i32>,
    pub offset: Option<i32>,
    pub ccd_temp: Option<f64>,
    pub ra: Option<f64>,
    pub dec: Option<f64>,
    pub altitude: Option<f64>,
    pub telescope: Option<String>,
    pub instrument: Option<String>,
    pub observer: Option<String>,
    pub bin_x: i32,
    pub bin_y: i32,
    pub focal_length: Option<f64>,
    pub aperture: Option<f64>,
    pub pixel_size_x: Option<f64>,
    pub pixel_size_y: Option<f64>,
    pub site_latitude: Option<f64>,
    pub site_longitude: Option<f64>,
    pub site_elevation: Option<f64>,
}

/// Save image data to FITS file
pub async fn api_save_fits_file(
    file_path: String,
    width: u32,
    height: u32,
    data: Vec<u16>,
    header_data: FitsWriteHeader,
) -> Result<(), NightshadeError> {
    tracing::info!("Saving FITS file to: {}", file_path);

    // Create ImageData
    let image = ImageData::from_u16(width, height, 1, &data);

    // Validate image data
    let validation = validate_image(&image, Some(width), Some(height));
    if !validation.is_valid {
        tracing::warn!("Image validation failed: {:?}", validation.errors);
    }
    for warning in &validation.warnings {
        tracing::warn!("Image validation warning: {}", warning);
    }

    // Create FitsHeader
    let mut header = FitsHeader::new();

    // Core observation metadata
    header.set_float("EXPTIME", header_data.exposure_time);
    header.set_string("DATE-OBS", &header_data.capture_timestamp);
    header.set_string("IMAGETYP", &header_data.frame_type);

    if let Some(name) = header_data.object_name {
        header.set_string("OBJECT", &name);
    }
    if let Some(filter) = header_data.filter {
        header.set_string("FILTER", &filter);
    }

    // Camera settings
    if let Some(gain) = header_data.gain {
        header.set_int("GAIN", gain as i64);
    }
    if let Some(offset) = header_data.offset {
        header.set_int("OFFSET", offset as i64);
    }
    if let Some(temp) = header_data.ccd_temp {
        header.set_float("CCD-TEMP", temp);
    }

    header.set_int("XBINNING", header_data.bin_x as i64);
    header.set_int("YBINNING", header_data.bin_y as i64);

    // Pixel size information
    if let Some(pixel_x) = header_data.pixel_size_x {
        header.set_float("PIXSIZE1", pixel_x);
        header.set_float("XPIXSZ", pixel_x * header_data.bin_x as f64);
    }
    if let Some(pixel_y) = header_data.pixel_size_y {
        header.set_float("PIXSIZE2", pixel_y);
        header.set_float("YPIXSZ", pixel_y * header_data.bin_y as f64);
    }

    // Telescope/optics information
    if let Some(focal_length) = header_data.focal_length {
        header.set_float("FOCALLEN", focal_length);
    }
    if let Some(aperture) = header_data.aperture {
        header.set_float("APTDIA", aperture);
    }
    if let Some(telescope) = header_data.telescope {
        header.set_string("TELESCOP", &telescope);
    }
    if let Some(instrument) = header_data.instrument {
        header.set_string("INSTRUME", &instrument);
    }

    // Observer information
    if let Some(observer) = header_data.observer {
        header.set_string("OBSERVER", &observer);
    }

    // Observer location
    if let Some(lat) = header_data.site_latitude {
        header.set_float("SITELAT", lat);
    }
    if let Some(long) = header_data.site_longitude {
        header.set_float("SITELONG", long);
    }
    if let Some(elev) = header_data.site_elevation {
        header.set_float("SITEELEV", elev);
    }

    // Target coordinates and airmass
    if let Some(ra) = header_data.ra {
        header.set_float("RA", ra);
    }
    if let Some(dec) = header_data.dec {
        header.set_float("DEC", dec);
    }
    if let Some(altitude) = header_data.altitude {
        let airmass = calculate_airmass(altitude);
        header.set_float("AIRMASS", airmass);
    }

    // Validate header completeness
    let header_validation = validate_fits_header(&header);
    for warning in &header_validation.warnings {
        tracing::debug!("FITS header warning: {}", warning);
    }
    
    // Ensure directory exists
    if let Some(parent) = std::path::Path::new(&file_path).parent() {
        std::fs::create_dir_all(parent)
            .map_err(|e| NightshadeError::OperationFailed(format!("Failed to create directory: {}", e)))?;
    }
    
    // Write file
    // Note: write_fits is blocking, so we should spawn_blocking if possible, 
    // but for now we'll just run it (it's fast enough for small headers, but data writing might take time)
    // Ideally: tokio::task::spawn_blocking
    
    let path = std::path::PathBuf::from(file_path);
    
    tokio::task::spawn_blocking(move || {
        write_fits(&path, &image, &header)
    }).await
    .map_err(|e| NightshadeError::OperationFailed(format!("Task join error: {}", e)))?
    .map_err(|e| NightshadeError::OperationFailed(format!("Failed to write FITS: {}", e)))?;
    
    Ok(())
}

// =============================================================================
// Image Processing
// =============================================================================

/// Calculate image statistics
#[flutter_rust_bridge::frb(sync)]
pub fn api_get_image_stats(width: u32, height: u32, data: Vec<u16>) -> Result<ImageStatsResult, NightshadeError> {
    let stats = crate::imaging_ops::get_image_stats(width, height, data);
    Ok(ImageStatsResult {
        min: stats.min,
        max: stats.max,
        mean: stats.mean,
        median: stats.median,
        std_dev: stats.std_dev,
        hfr: None,
        star_count: 0,
    })
}

/// Auto-stretch image for display
#[flutter_rust_bridge::frb(sync)]
pub fn api_auto_stretch_image(width: u32, height: u32, data: Vec<u16>) -> Result<Vec<u8>, NightshadeError> {
    Ok(crate::imaging_ops::auto_stretch_image(width, height, data))
}

/// Debayer image
#[flutter_rust_bridge::frb(sync)]
pub fn api_debayer_image(
    width: u32,
    height: u32,
    data: Vec<u16>,
    pattern_str: String,
    algo_str: String,
) -> Result<Vec<u8>, NightshadeError> {
    let pattern = BayerPattern::from_str(&pattern_str)
        .ok_or_else(|| NightshadeError::InvalidParameter(format!("Invalid bayer pattern: {}", pattern_str)))?;
        
    let algorithm = match algo_str.to_lowercase().as_str() {
        "bilinear" => DebayerAlgorithm::Bilinear,
        "vng" => DebayerAlgorithm::VNG,
        "superpixel" => DebayerAlgorithm::SuperPixel,
        _ => DebayerAlgorithm::Bilinear,
    };
    
    Ok(crate::imaging_ops::debayer_image(width, height, data, pattern, algorithm))
}

/// Generate thumbnail from FITS file
/// Returns JPEG-encoded thumbnail data (~512x512 pixels)
#[flutter_rust_bridge::frb(sync)]
pub fn api_generate_fits_thumbnail(file_path: String, max_size: u32) -> Result<Vec<u8>, NightshadeError> {
    use nightshade_imaging::{read_fits, ImageData};
    use std::path::Path;

    // Read FITS file
    let path = Path::new(&file_path);
    let (image_data, _header) = read_fits(path)
        .map_err(|e| NightshadeError::ImageError(format!("Failed to read FITS: {:?}", e)))?;

    // Convert to u16 data
    let data_u16 = match image_data.pixel_type {
        nightshade_imaging::PixelType::U8 => {
            // Convert u8 to u16
            image_data.data.iter().map(|&b| (b as u16) << 8).collect::<Vec<u16>>()
        },
        nightshade_imaging::PixelType::U16 => {
            // Already u16, convert bytes to u16 values
            image_data.data.chunks_exact(2)
                .map(|chunk| u16::from_be_bytes([chunk[0], chunk[1]]))
                .collect::<Vec<u16>>()
        },
        nightshade_imaging::PixelType::U32 => {
            // Convert u32 to u16 (downscale)
            image_data.data.chunks_exact(4)
                .map(|chunk| {
                    let val = u32::from_be_bytes([chunk[0], chunk[1], chunk[2], chunk[3]]);
                    (val >> 16) as u16  // Take high 16 bits
                })
                .collect::<Vec<u16>>()
        },
        nightshade_imaging::PixelType::F32 => {
            // Convert f32 to u16 (scale 0.0-1.0 to 0-65535)
            image_data.data.chunks_exact(4)
                .map(|chunk| {
                    let val = f32::from_be_bytes([chunk[0], chunk[1], chunk[2], chunk[3]]);
                    (val.clamp(0.0, 1.0) * 65535.0) as u16
                })
                .collect::<Vec<u16>>()
        },
        nightshade_imaging::PixelType::F64 => {
            // Convert f64 to u16 (scale 0.0-1.0 to 0-65535)
            image_data.data.chunks_exact(8)
                .map(|chunk| {
                    let val = f64::from_be_bytes([chunk[0], chunk[1], chunk[2], chunk[3], chunk[4], chunk[5], chunk[6], chunk[7]]);
                    (val.clamp(0.0, 1.0) * 65535.0) as u16
                })
                .collect::<Vec<u16>>()
        },
    };

    let width = image_data.width;
    let height = image_data.height;

    // Calculate downscale factor
    let scale = ((width.max(height) as f32) / max_size as f32).ceil() as u32;
    let scale = scale.max(1);

    // Downscale image
    let new_width = width / scale;
    let new_height = height / scale;
    let mut downscaled = Vec::with_capacity((new_width * new_height) as usize);

    for y in 0..new_height {
        for x in 0..new_width {
            let src_x = x * scale;
            let src_y = y * scale;
            let idx = (src_y * width + src_x) as usize;
            if idx < data_u16.len() {
                downscaled.push(data_u16[idx]);
            } else {
                downscaled.push(0);
            }
        }
    }

    // Auto-stretch for display
    let stretched = crate::imaging_ops::auto_stretch_image(new_width, new_height, downscaled);

    // Encode as JPEG
    use image::{GrayImage, ImageEncoder};
    use std::io::Cursor;

    let gray_img = GrayImage::from_raw(new_width, new_height, stretched)
        .ok_or_else(|| NightshadeError::ImageError("Failed to create grayscale image".to_string()))?;

    let mut jpeg_data = Vec::new();
    let mut cursor = Cursor::new(&mut jpeg_data);
    let encoder = image::codecs::jpeg::JpegEncoder::new_with_quality(&mut cursor, 85);
    encoder.write_image(
        gray_img.as_raw(),
        new_width,
        new_height,
        image::ColorType::L8,
    ).map_err(|e| NightshadeError::ImageError(format!("JPEG encoding failed: {}", e)))?;

    Ok(jpeg_data)
}

#[cfg(windows)]
pub mod ascom_connections {
    use super::*;
    use std::collections::HashMap;
    
    // Storage for active ASCOM connections
    static ASCOM_CAMERAS: OnceLock<Arc<RwLock<HashMap<String, nightshade_ascom::AscomCamera>>>> = OnceLock::new();
    static ASCOM_MOUNTS: OnceLock<Arc<RwLock<HashMap<String, nightshade_ascom::AscomMount>>>> = OnceLock::new();
    static ASCOM_FOCUSERS: OnceLock<Arc<RwLock<HashMap<String, nightshade_ascom::AscomFocuser>>>> = OnceLock::new();
    
    fn get_ascom_cameras() -> &'static Arc<RwLock<HashMap<String, nightshade_ascom::AscomCamera>>> {
        ASCOM_CAMERAS.get_or_init(|| Arc::new(RwLock::new(HashMap::new())))
    }
    
    fn get_ascom_mounts() -> &'static Arc<RwLock<HashMap<String, nightshade_ascom::AscomMount>>> {
        ASCOM_MOUNTS.get_or_init(|| Arc::new(RwLock::new(HashMap::new())))
    }
    
    fn get_ascom_focusers() -> &'static Arc<RwLock<HashMap<String, nightshade_ascom::AscomFocuser>>> {
        ASCOM_FOCUSERS.get_or_init(|| Arc::new(RwLock::new(HashMap::new())))
    }
    
    /// Connect to a real ASCOM camera
    pub async fn connect_ascom_camera(prog_id: &str) -> Result<(), NightshadeError> {
        let mut camera = nightshade_ascom::AscomCamera::new(prog_id)
            .map_err(|e| NightshadeError::ConnectionFailed(e))?;
        
        camera.connect()
            .map_err(|e| NightshadeError::ConnectionFailed(e))?;
        
        let name = camera.name().unwrap_or_else(|_| prog_id.to_string());
        tracing::info!("Connected to ASCOM camera: {}", name);
        
        // Store the connection
        let mut cameras = get_ascom_cameras().write().await;
        cameras.insert(prog_id.to_string(), camera);
        
        Ok(())
    }
    
    /// Connect to a real ASCOM mount
    pub async fn connect_ascom_mount(prog_id: &str) -> Result<(), NightshadeError> {
        let mut mount = nightshade_ascom::AscomMount::new(prog_id)
            .map_err(|e| NightshadeError::ConnectionFailed(e))?;
        
        mount.connect()
            .map_err(|e| NightshadeError::ConnectionFailed(e))?;
        
        let name = mount.name().unwrap_or_else(|_| prog_id.to_string());
        tracing::info!("Connected to ASCOM mount: {}", name);
        
        // Store the connection
        let mut mounts = get_ascom_mounts().write().await;
        mounts.insert(prog_id.to_string(), mount);
        
        Ok(())
    }
    
    /// Connect to a real ASCOM focuser
    pub async fn connect_ascom_focuser(prog_id: &str) -> Result<(), NightshadeError> {
        let mut focuser = nightshade_ascom::AscomFocuser::new(prog_id)
            .map_err(|e| NightshadeError::ConnectionFailed(e))?;
        
        focuser.connect()
            .map_err(|e| NightshadeError::ConnectionFailed(e))?;
        
        tracing::info!("Connected to ASCOM focuser: {}", prog_id);
        
        // Store the connection
        let mut focusers = get_ascom_focusers().write().await;
        focusers.insert(prog_id.to_string(), focuser);
        
        Ok(())
    }
    
    /// Get real ASCOM camera temperature
    pub async fn get_ascom_camera_temp(prog_id: &str) -> Result<f64, NightshadeError> {
        let cameras = get_ascom_cameras().read().await;
        let camera = cameras.get(prog_id)
            .ok_or_else(|| NightshadeError::NotConnected(prog_id.to_string()))?;
        
        camera.ccd_temperature()
            .map_err(|e| NightshadeError::OperationFailed(e))
    }
    
    /// Get real ASCOM mount coordinates
    pub async fn get_ascom_mount_coords(prog_id: &str) -> Result<(f64, f64), NightshadeError> {
        let mounts = get_ascom_mounts().read().await;
        let mount = mounts.get(prog_id)
            .ok_or_else(|| NightshadeError::NotConnected(prog_id.to_string()))?;
        
        let ra = mount.right_ascension()
            .map_err(|e| NightshadeError::OperationFailed(e))?;
        let dec = mount.declination()
            .map_err(|e| NightshadeError::OperationFailed(e))?;
        
        Ok((ra, dec))
    }
    
    /// Slew real ASCOM mount
    pub async fn slew_ascom_mount(prog_id: &str, ra: f64, dec: f64) -> Result<(), NightshadeError> {
        let mut mounts = get_ascom_mounts().write().await;
        let mount = mounts.get_mut(prog_id)
            .ok_or_else(|| NightshadeError::NotConnected(prog_id.to_string()))?;
        
        mount.slew_to_coordinates_async(ra, dec)
            .map_err(|e| NightshadeError::OperationFailed(e))
    }
    
    /// Get real ASCOM focuser position
    pub async fn get_ascom_focuser_position(prog_id: &str) -> Result<i32, NightshadeError> {
        let focusers = get_ascom_focusers().read().await;
        let focuser = focusers.get(prog_id)
            .ok_or_else(|| NightshadeError::NotConnected(prog_id.to_string()))?;
        
        focuser.position()
            .map_err(|e| NightshadeError::OperationFailed(e))
    }
    
    /// Move real ASCOM focuser
    pub async fn move_ascom_focuser(prog_id: &str, position: i32) -> Result<(), NightshadeError> {
        let mut focusers = get_ascom_focusers().write().await;
        let focuser = focusers.get_mut(prog_id)
            .ok_or_else(|| NightshadeError::NotConnected(prog_id.to_string()))?;
        
        focuser.move_to(position)
            .map_err(|e| NightshadeError::OperationFailed(e))
    }
}

/// Apply Auto White Balance using Histogram Peak Alignment
/// This aligns the background sky peak of R and B channels to the G channel
fn apply_auto_white_balance(image: &mut [u16]) {
    if image.len() % 3 != 0 {
        return;
    }

    let mut hist_r = vec![0u32; 65536];
    let mut hist_g = vec![0u32; 65536];
    let mut hist_b = vec![0u32; 65536];

    // 1. Compute histograms
    for chunk in image.chunks(3) {
        hist_r[chunk[0] as usize] += 1;
        hist_g[chunk[1] as usize] += 1;
        hist_b[chunk[2] as usize] += 1;
    }

    // 2. Find peaks (modes), ignoring bottom 1% to avoid clipping noise
    // A simple mode might be noisy, so let's find the max bin
    // We start searching from a small offset to avoid black clipping
    let start_idx = 100; // arbitrary small offset
    
    let get_peak = |hist: &[u32]| -> u16 {
        let mut max_count = 0;
        let mut peak_idx = 0;
        for (i, &count) in hist.iter().enumerate().skip(start_idx) {
            if count > max_count {
                max_count = count;
                peak_idx = i;
            }
        }
        peak_idx as u16
    };

    let peak_r = get_peak(&hist_r);
    let peak_g = get_peak(&hist_g);
    let peak_b = get_peak(&hist_b);

    tracing::info!("AWB Peaks: R={}, G={}, B={}", peak_r, peak_g, peak_b);

    if peak_r == 0 || peak_g == 0 || peak_b == 0 {
        tracing::warn!("AWB failed: peak is 0");
        return;
    }

    // 3. Calculate scaling factors to align to Green
    let target = peak_g as f32;
    let scale_r = target / peak_r as f32;
    let scale_b = target / peak_b as f32;
    
    tracing::info!("AWB Scales: R={:.3}, B={:.3}", scale_r, scale_b);

    // 4. Apply scaling
    // Use parallel iterator for speed if possible, but slice is mutable
    // Rayon's par_chunks_mut is perfect
    use rayon::prelude::*;
    image.par_chunks_mut(3).for_each(|pixel| {
        // R
        pixel[0] = (pixel[0] as f32 * scale_r).min(65535.0) as u16;
        // G (unchanged)
        // B
        pixel[2] = (pixel[2] as f32 * scale_b).min(65535.0) as u16;
    });
}

// =============================================================================
// INDI Autofocus
// =============================================================================

/// INDI autofocus configuration
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct IndiAutofocusConfigApi {
    pub method: String,  // "vcurve", "quadratic", "hyperbolic"
    pub step_size: i32,
    pub steps_out: u32,
    pub exposure_duration: f64,
    pub backlash_compensation: i32,
    pub use_temperature_prediction: bool,
    pub max_star_count_change: Option<f64>,
    pub outlier_rejection_sigma: f64,
    pub binning: i32,
    pub move_timeout_secs: u64,
    pub settling_time_ms: u64,
}

impl Default for IndiAutofocusConfigApi {
    fn default() -> Self {
        Self {
            method: "vcurve".to_string(),
            step_size: 100,
            steps_out: 7,
            exposure_duration: 3.0,
            backlash_compensation: 50,
            use_temperature_prediction: true,
            max_star_count_change: Some(0.5),
            outlier_rejection_sigma: 3.0,
            binning: 1,
            move_timeout_secs: 120,
            settling_time_ms: 500,
        }
    }
}

/// INDI autofocus result
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct IndiAutofocusResultApi {
    pub best_position: i32,
    pub best_hfr: f64,
    pub curve_fit_quality: f64,
    pub method_used: String,
    pub data_points: Vec<FocusDataPointApi>,
    pub temperature_celsius: Option<f64>,
    pub backlash_applied: bool,
    pub success: bool,
    pub error_message: Option<String>,
}

/// Focus data point for autofocus curve
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct FocusDataPointApi {
    pub position: i32,
    pub hfr: f64,
    pub fwhm: Option<f64>,
    pub star_count: u32,
}

/// Run INDI autofocus routine
///
/// # Arguments
/// * `camera_id` - INDI camera device ID (format: "indi:host:port:device_name")
/// * `focuser_id` - INDI focuser device ID (format: "indi:host:port:device_name")
/// * `config` - Autofocus configuration
///
/// # Returns
/// Autofocus result with best focus position and curve data
pub async fn api_run_indi_autofocus(
    camera_id: String,
    focuser_id: String,
    config: IndiAutofocusConfigApi,
) -> Result<IndiAutofocusResultApi, NightshadeError> {
    tracing::info!(
        "Starting INDI autofocus: camera={}, focuser={}, method={}",
        camera_id,
        focuser_id,
        config.method
    );

    // Validate device IDs are INDI
    if !camera_id.starts_with("indi:") || !focuser_id.starts_with("indi:") {
        return Err(NightshadeError::InvalidParameter(
            "Both camera and focuser must be INDI devices".to_string()
        ));
    }

    // Get INDI clients for camera and focuser
    let device_manager = get_device_manager();

    let camera_client = device_manager
        .get_indi_client(&camera_id)
        .await
        .ok_or_else(|| {
            NightshadeError::NotConnected(format!("INDI camera not connected: {}", camera_id))
        })?;

    let focuser_client = device_manager
        .get_indi_client(&focuser_id)
        .await
        .ok_or_else(|| {
            NightshadeError::NotConnected(format!("INDI focuser not connected: {}", focuser_id))
        })?;

    // Extract device names from IDs (format: "indi:host:port:device_name")
    let camera_parts: Vec<&str> = camera_id.split(':').collect();
    let camera_device_name = camera_parts[3..].join(":");

    let focuser_parts: Vec<&str> = focuser_id.split(':').collect();
    let focuser_device_name = focuser_parts[3..].join(":");

    // Create INDI camera and focuser wrappers
    let camera = Arc::new(nightshade_indi::IndiCamera::new(
        camera_client,
        &camera_device_name,
    ));

    let focuser = Arc::new(nightshade_indi::IndiFocuser::new(
        focuser_client,
        &focuser_device_name,
    ));

    // Convert config
    let method = match config.method.as_str() {
        "vcurve" => nightshade_indi::autofocus::AutofocusMethod::VCurve,
        "quadratic" => nightshade_indi::autofocus::AutofocusMethod::Quadratic,
        "hyperbolic" => nightshade_indi::autofocus::AutofocusMethod::Hyperbolic,
        _ => nightshade_indi::autofocus::AutofocusMethod::VCurve,
    };

    let af_config = nightshade_indi::autofocus::IndiAutofocusConfig {
        method,
        step_size: config.step_size,
        steps_out: config.steps_out,
        exposure_duration: config.exposure_duration,
        backlash_compensation: config.backlash_compensation,
        use_temperature_prediction: config.use_temperature_prediction,
        max_star_count_change: config.max_star_count_change,
        outlier_rejection_sigma: config.outlier_rejection_sigma,
        binning: config.binning,
        move_timeout_secs: config.move_timeout_secs,
        settling_time_ms: config.settling_time_ms,
    };

    // Create autofocus engine
    let autofocus = nightshade_indi::autofocus::IndiAutofocus::new(camera, focuser, af_config);

    // Run autofocus
    let result = autofocus.run().await.map_err(|e| {
        NightshadeError::OperationFailed(format!("INDI autofocus failed: {}", e))
    })?;

    // Convert result
    let method_str = match result.method_used {
        nightshade_indi::autofocus::AutofocusMethod::VCurve => "vcurve",
        nightshade_indi::autofocus::AutofocusMethod::Quadratic => "quadratic",
        nightshade_indi::autofocus::AutofocusMethod::Hyperbolic => "hyperbolic",
    };

    let data_points: Vec<FocusDataPointApi> = result
        .data_points
        .iter()
        .map(|dp| FocusDataPointApi {
            position: dp.position,
            hfr: dp.hfr,
            fwhm: dp.fwhm,
            star_count: dp.star_count,
        })
        .collect();

    Ok(IndiAutofocusResultApi {
        best_position: result.best_position,
        best_hfr: result.best_hfr,
        curve_fit_quality: result.curve_fit_quality,
        method_used: method_str.to_string(),
        data_points,
        temperature_celsius: result.temperature_celsius,
        backlash_applied: result.backlash_applied,
        success: result.success,
        error_message: result.error_message,
    })
}
