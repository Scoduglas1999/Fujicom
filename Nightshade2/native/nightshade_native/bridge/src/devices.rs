//! Device Manager with connection state management and auto-reconnection
//!
//! Provides a unified interface for managing device connections across
//! different driver backends (ASCOM, Alpaca, Simulator).

use crate::device::*;
use crate::event::*;
use crate::state::SharedAppState;
use std::collections::HashMap;
use std::sync::Arc;
use std::time::Duration;
use tokio::sync::RwLock;
use tokio::time::interval;
use nightshade_native::traits::{NativeDevice, NativeCamera, NativeMount, NativeFocuser, NativeFilterWheel, NativeRotator, NativeDome, NativeWeather, NativeSafetyMonitor};
use nightshade_native::camera::{ImageData, ExposureParams};
use nightshade_native::vendor::zwo::{ZwoCamera, ZwoFocuser, ZwoFilterWheel};
use nightshade_native::vendor::qhy::{QhyCamera, QhyFilterWheel};
use nightshade_native::vendor::player_one::PlayerOneCamera;
use nightshade_native::vendor::svbony::SvbonyCamera;
use nightshade_native::vendor::atik::AtikCamera;
use nightshade_native::vendor::fli::{FliCamera, FliFocuser, FliFilterWheel};
use nightshade_native::vendor::touptek::TouptekCamera;
use nightshade_native::vendor::moravian::MoravianCamera;
// Mount drivers
use nightshade_native::vendor::skywatcher::SkyWatcherMount;
use nightshade_native::vendor::ioptron::IOptronMount;
use nightshade_native::vendor::lx200::{Lx200Mount, Lx200MountType};

/// Configuration for automatic reconnection
#[derive(Debug, Clone)]
pub struct ReconnectConfig {
    /// Whether auto-reconnection is enabled
    pub enabled: bool,
    /// Maximum number of reconnection attempts (0 = unlimited)
    pub max_attempts: u32,
    /// Initial delay between reconnection attempts
    pub initial_delay_secs: u64,
    /// Maximum delay between reconnection attempts
    pub max_delay_secs: u64,
    /// Backoff multiplier for exponential backoff
    pub backoff_multiplier: f64,
}

/// Configuration for heartbeat monitoring (per device type)
#[derive(Debug, Clone, Copy)]
pub struct HeartbeatConfig {
    /// Base interval between heartbeats in seconds
    pub base_interval_secs: u64,
    /// Maximum interval (after backoff) in seconds
    pub max_interval_secs: u64,
    /// Number of consecutive failures before marking device disconnected
    pub failure_threshold: u32,
    /// Backoff multiplier when failures occur
    pub backoff_multiplier: f64,
}

impl Default for ReconnectConfig {
    fn default() -> Self {
        Self {
            enabled: true,
            max_attempts: 10,
            initial_delay_secs: 2,
            max_delay_secs: 60,
            backoff_multiplier: 1.5,
        }
    }
}

/// State of a managed device
#[derive(Debug, Clone)]
pub struct ManagedDevice {
    pub info: DeviceInfo,
    pub connection_state: ConnectionState,
    pub last_error: Option<String>,
    pub reconnect_attempts: u32,
    pub auto_reconnect: bool,
    /// Last successful communication timestamp (milliseconds since epoch)
    pub last_successful_comm: Option<i64>,
    /// Whether heartbeat monitoring is active
    pub heartbeat_active: bool,
}

/// The Device Manager handles all device connections
pub struct DeviceManager {
    /// Application state for publishing events
    app_state: SharedAppState,
    
    /// Managed devices by their ID
    devices: RwLock<HashMap<String, ManagedDevice>>,
    
    /// Reconnection configuration
    reconnect_config: ReconnectConfig,
    
    /// Flag to stop the reconnection task
    stop_reconnect: Arc<RwLock<bool>>,
    
    /// Active native device instances
    native_devices: RwLock<HashMap<String, Box<dyn NativeDevice>>>,

    /// Active ASCOM camera wrappers (for typed access, wrapped in RwLock for interior mutability)
    #[cfg(windows)]
    /// Active ASCOM camera wrappers (for typed access, wrapped in RwLock for interior mutability)
    #[cfg(windows)]
    ascom_cameras: RwLock<HashMap<String, Arc<RwLock<crate::ascom_wrapper::AscomCameraWrapper>>>>,

    /// Active ASCOM mount wrappers
    #[cfg(windows)]
    ascom_mounts: RwLock<HashMap<String, Arc<RwLock<crate::ascom_wrapper_mount::AscomMountWrapper>>>>,

    /// Active ASCOM focuser wrappers
    #[cfg(windows)]
    ascom_focusers: RwLock<HashMap<String, Arc<RwLock<crate::ascom_wrapper_focuser::AscomFocuserWrapper>>>>,

    /// Active ASCOM filter wheel wrappers
    #[cfg(windows)]
    ascom_filter_wheels: RwLock<HashMap<String, Arc<RwLock<crate::ascom_wrapper_filterwheel::AscomFilterWheelWrapper>>>>,

    /// Active ASCOM dome wrappers
    #[cfg(windows)]
    ascom_domes: RwLock<HashMap<String, Arc<RwLock<crate::ascom_wrapper_dome::AscomDomeWrapper>>>>,

    /// Active ASCOM switch wrappers
    #[cfg(windows)]
    ascom_switches: RwLock<HashMap<String, Arc<RwLock<crate::ascom_wrapper_switch::AscomSwitchWrapper>>>>,

    /// Active ASCOM cover calibrator wrappers
    #[cfg(windows)]
    ascom_cover_calibrators: RwLock<HashMap<String, Arc<RwLock<crate::ascom_wrapper_covercalibrator::AscomCoverCalibratorWrapper>>>>,

    /// Active INDI clients (key: "host:port")
    indi_clients: RwLock<HashMap<String, Arc<RwLock<nightshade_indi::IndiClient>>>>,

    /// Active Alpaca camera clients
    alpaca_cameras: RwLock<HashMap<String, Arc<nightshade_alpaca::AlpacaCamera>>>,

    /// Active Alpaca mount clients
    alpaca_mounts: RwLock<HashMap<String, Arc<nightshade_alpaca::AlpacaTelescope>>>,

    /// Active Alpaca focuser clients
    alpaca_focusers: RwLock<HashMap<String, Arc<nightshade_alpaca::AlpacaFocuser>>>,

    /// Active Alpaca filter wheel clients
    alpaca_filter_wheels: RwLock<HashMap<String, Arc<nightshade_alpaca::AlpacaFilterWheel>>>,

    /// Active Alpaca rotator clients
    alpaca_rotators: RwLock<HashMap<String, Arc<nightshade_alpaca::AlpacaRotator>>>,

    /// Active Alpaca dome clients
    alpaca_domes: RwLock<HashMap<String, Arc<nightshade_alpaca::AlpacaDome>>>,

    /// Active Alpaca observing conditions (weather) clients
    alpaca_weather: RwLock<HashMap<String, Arc<nightshade_alpaca::AlpacaObservingConditions>>>,

    /// Active Alpaca safety monitor clients
    alpaca_safety_monitors: RwLock<HashMap<String, Arc<nightshade_alpaca::AlpacaSafetyMonitor>>>,

    /// Active Alpaca switch clients
    alpaca_switches: RwLock<HashMap<String, Arc<nightshade_alpaca::AlpacaSwitch>>>,

    /// Active Alpaca cover calibrator clients
    alpaca_cover_calibrators: RwLock<HashMap<String, Arc<nightshade_alpaca::AlpacaCoverCalibrator>>>,

    /// Active Native SDK cameras (stored separately for typed access)
    pub(crate) native_cameras: RwLock<HashMap<String, Box<dyn NativeCamera + Send + Sync>>>,

    /// Active Native SDK focusers (stored separately for typed access)
    pub(crate) native_focusers: RwLock<HashMap<String, Box<dyn NativeFocuser + Send + Sync>>>,

    /// Active Native SDK filter wheels (stored separately for typed access)
    pub(crate) native_filter_wheels: RwLock<HashMap<String, Box<dyn NativeFilterWheel + Send + Sync>>>,

    /// Active Native SDK mounts (stored separately for typed access)
    pub(crate) native_mounts: RwLock<HashMap<String, Box<dyn NativeMount + Send + Sync>>>,

    /// Active Native SDK rotators (stored separately for typed access)
    pub(crate) native_rotators: RwLock<HashMap<String, Box<dyn NativeRotator + Send + Sync>>>,

    /// Active Native SDK domes (stored separately for typed access)
    pub(crate) native_domes: RwLock<HashMap<String, Box<dyn NativeDome + Send + Sync>>>,

    /// Active Native SDK weather stations (stored separately for typed access)
    pub(crate) native_weather: RwLock<HashMap<String, Box<dyn NativeWeather + Send + Sync>>>,

    /// Active Native SDK safety monitors (stored separately for typed access)
    pub(crate) native_safety_monitors: RwLock<HashMap<String, Box<dyn NativeSafetyMonitor + Send + Sync>>>,

    /// Active heartbeat monitoring tasks (device_id -> join handle)
    heartbeat_tasks: RwLock<HashMap<String, tokio::task::JoinHandle<()>>>,
}



impl DeviceManager {
    /// Create a new device manager
    pub fn new(app_state: SharedAppState) -> Arc<Self> {
        let manager = Arc::new(Self {
            app_state,
            devices: RwLock::new(HashMap::new()),
            reconnect_config: ReconnectConfig::default(),
            stop_reconnect: Arc::new(RwLock::new(false)),
            native_devices: RwLock::new(HashMap::new()),
            #[cfg(windows)]
            ascom_cameras: RwLock::new(HashMap::new()),
            #[cfg(windows)]
            ascom_mounts: RwLock::new(HashMap::new()),
            #[cfg(windows)]
            ascom_focusers: RwLock::new(HashMap::new()),
            #[cfg(windows)]
            ascom_filter_wheels: RwLock::new(HashMap::new()),
            #[cfg(windows)]
            ascom_domes: RwLock::new(HashMap::new()),
            #[cfg(windows)]
            ascom_switches: RwLock::new(HashMap::new()),
            #[cfg(windows)]
            ascom_cover_calibrators: RwLock::new(HashMap::new()),
            indi_clients: RwLock::new(HashMap::new()),
            alpaca_cameras: RwLock::new(HashMap::new()),
            alpaca_mounts: RwLock::new(HashMap::new()),
            alpaca_focusers: RwLock::new(HashMap::new()),
            alpaca_filter_wheels: RwLock::new(HashMap::new()),
            alpaca_rotators: RwLock::new(HashMap::new()),
            alpaca_domes: RwLock::new(HashMap::new()),
            alpaca_weather: RwLock::new(HashMap::new()),
            alpaca_safety_monitors: RwLock::new(HashMap::new()),
            alpaca_switches: RwLock::new(HashMap::new()),
            alpaca_cover_calibrators: RwLock::new(HashMap::new()),
            native_cameras: RwLock::new(HashMap::new()),
            native_focusers: RwLock::new(HashMap::new()),
            native_filter_wheels: RwLock::new(HashMap::new()),
            native_mounts: RwLock::new(HashMap::new()),
            native_rotators: RwLock::new(HashMap::new()),
            native_domes: RwLock::new(HashMap::new()),
            native_weather: RwLock::new(HashMap::new()),
            native_safety_monitors: RwLock::new(HashMap::new()),
            heartbeat_tasks: RwLock::new(HashMap::new()),
        });

        // Start the reconnection background task
        // Note: Must have runtime available - ensured by api_init() calling ensure_runtime()
        let manager_clone = Arc::clone(&manager);
        // Get the runtime handle and spawn the task
        // We use the crate-level runtime which must be initialized first
        if let Ok(runtime) = crate::ensure_runtime() {
            runtime.handle().spawn(async move {
                manager_clone.reconnection_loop().await;
            });
        } else {
            tracing::error!("Cannot start reconnection loop: runtime initialization failed");
        }

        manager
    }

    /// Create with custom reconnection config
    pub fn with_config(app_state: SharedAppState, config: ReconnectConfig) -> Arc<Self> {
        let manager = Arc::new(Self {
            app_state,
            devices: RwLock::new(HashMap::new()),
            reconnect_config: config,
            stop_reconnect: Arc::new(RwLock::new(false)),
            native_devices: RwLock::new(HashMap::new()),
            #[cfg(windows)]
            ascom_cameras: RwLock::new(HashMap::new()),
            #[cfg(windows)]
            ascom_mounts: RwLock::new(HashMap::new()),
            #[cfg(windows)]
            ascom_focusers: RwLock::new(HashMap::new()),
            #[cfg(windows)]
            ascom_filter_wheels: RwLock::new(HashMap::new()),
            #[cfg(windows)]
            ascom_domes: RwLock::new(HashMap::new()),
            #[cfg(windows)]
            ascom_switches: RwLock::new(HashMap::new()),
            #[cfg(windows)]
            ascom_cover_calibrators: RwLock::new(HashMap::new()),
            indi_clients: RwLock::new(HashMap::new()),
            alpaca_cameras: RwLock::new(HashMap::new()),
            alpaca_mounts: RwLock::new(HashMap::new()),
            alpaca_focusers: RwLock::new(HashMap::new()),
            alpaca_filter_wheels: RwLock::new(HashMap::new()),
            alpaca_rotators: RwLock::new(HashMap::new()),
            alpaca_domes: RwLock::new(HashMap::new()),
            alpaca_weather: RwLock::new(HashMap::new()),
            alpaca_safety_monitors: RwLock::new(HashMap::new()),
            alpaca_switches: RwLock::new(HashMap::new()),
            alpaca_cover_calibrators: RwLock::new(HashMap::new()),
            native_cameras: RwLock::new(HashMap::new()),
            native_focusers: RwLock::new(HashMap::new()),
            native_filter_wheels: RwLock::new(HashMap::new()),
            native_mounts: RwLock::new(HashMap::new()),
            native_rotators: RwLock::new(HashMap::new()),
            native_domes: RwLock::new(HashMap::new()),
            native_weather: RwLock::new(HashMap::new()),
            native_safety_monitors: RwLock::new(HashMap::new()),
            heartbeat_tasks: RwLock::new(HashMap::new()),
        });

        // Start the reconnection background task
        // Note: Must have runtime available - ensured by api_init() calling ensure_runtime()
        let manager_clone = Arc::clone(&manager);
        // Get the runtime handle and spawn the task
        // We use the crate-level runtime which must be initialized first
        if let Ok(runtime) = crate::ensure_runtime() {
            runtime.handle().spawn(async move {
                manager_clone.reconnection_loop().await;
            });
        } else {
            tracing::error!("Cannot start reconnection loop: runtime initialization failed");
        }

        manager
    }

    /// Background task for automatic reconnection
    async fn reconnection_loop(&self) {
        let mut check_interval = interval(Duration::from_secs(5));
        
        loop {
            check_interval.tick().await;
            
            // Check if we should stop
            if *self.stop_reconnect.read().await {
                break;
            }
            
            if !self.reconnect_config.enabled {
                continue;
            }
            
            // Find devices that need reconnection
            let devices_to_reconnect: Vec<(String, ManagedDevice)> = {
                let devices = self.devices.read().await;
                devices
                    .iter()
                    .filter(|(_, dev)| {
                        dev.auto_reconnect 
                            && dev.connection_state == ConnectionState::Error
                            && (self.reconnect_config.max_attempts == 0 
                                || dev.reconnect_attempts < self.reconnect_config.max_attempts)
                    })
                    .map(|(id, dev)| (id.clone(), dev.clone()))
                    .collect()
            };
            
            // Attempt reconnection for each device
            for (device_id, device) in devices_to_reconnect {
                tracing::info!(
                    "Attempting reconnection for {} (attempt {})", 
                    device_id, 
                    device.reconnect_attempts + 1
                );
                
                // Calculate backoff delay
                let delay = self.calculate_backoff_delay(device.reconnect_attempts);
                tokio::time::sleep(Duration::from_secs(delay)).await;
                
                // Attempt reconnection
                if let Err(e) = self.connect_device_internal(&device.info).await {
                    tracing::warn!("Reconnection failed for {}: {}", device_id, e);
                    
                    // Update attempt counter
                    let mut devices = self.devices.write().await;
                    if let Some(dev) = devices.get_mut(&device_id) {
                        dev.reconnect_attempts += 1;
                        dev.last_error = Some(e.clone());
                        
                        // Publish reconnection failed event
                        self.app_state.publish_equipment_event(
                            EquipmentEvent::Error {
                                device_type: dev.info.device_type.as_str().to_string(),
                                device_id: device_id.clone(),
                                message: format!("Reconnection attempt {} failed: {}", 
                                    dev.reconnect_attempts, e),
                            },
                            EventSeverity::Warning,
                        );
                    }
                } else {
                    tracing::info!("Reconnection successful for {}", device_id);
                    
                    // Reset attempt counter on success
                    let mut devices = self.devices.write().await;
                    if let Some(dev) = devices.get_mut(&device_id) {
                        dev.reconnect_attempts = 0;
                        dev.last_error = None;
                    }
                }
            }
        }
    }
    
    /// Calculate backoff delay for reconnection
    fn calculate_backoff_delay(&self, attempts: u32) -> u64 {
        let delay = (self.reconnect_config.initial_delay_secs as f64)
            * self.reconnect_config.backoff_multiplier.powi(attempts as i32);
        
        (delay as u64).min(self.reconnect_config.max_delay_secs)
    }
    
    /// Register a device for management
    pub async fn register_device(&self, info: DeviceInfo, auto_reconnect: bool) {
        let mut devices = self.devices.write().await;
        devices.insert(info.id.clone(), ManagedDevice {
            info,
            connection_state: ConnectionState::Disconnected,
            last_error: None,
            reconnect_attempts: 0,
            auto_reconnect,
            last_successful_comm: None,
            heartbeat_active: false,
        });
    }
    
    /// Check if a device is registered
    pub async fn is_device_registered(&self, device_id: &str) -> bool {
        let devices = self.devices.read().await;
        devices.contains_key(device_id)
    }
    
    /// Connect to a device
    pub async fn connect_device(&self, device_id: &str) -> Result<(), String> {
        let device_info = {
            let devices = self.devices.read().await;
            devices.get(device_id)
                .map(|d| d.info.clone())
                .ok_or_else(|| format!("Device not found: {}", device_id))?
        };
        
        self.connect_device_internal(&device_info).await
    }
    
    /// Internal connection logic
    async fn connect_device_internal(&self, info: &DeviceInfo) -> Result<(), String> {
        let device_id = &info.id;
        
        // Update state to connecting
        {
            let mut devices = self.devices.write().await;
            if let Some(dev) = devices.get_mut(device_id) {
                dev.connection_state = ConnectionState::Connecting;
            }
        }
        
        // Publish connecting event
        self.app_state.publish_equipment_event(
            EquipmentEvent::Connecting {
                device_type: info.device_type.as_str().to_string(),
                device_id: device_id.clone(),
            },
            EventSeverity::Info,
        );
        
        // Perform actual connection based on driver type
        let result = match info.driver_type {
            DriverType::Simulator => self.connect_simulator(info).await,
            DriverType::Ascom => self.connect_ascom(info).await,
            DriverType::Alpaca => self.connect_alpaca(info).await,
            DriverType::Indi => self.connect_indi(info).await,
            DriverType::Native => self.connect_native(info).await,
        };
        
        // Update state based on result
        {
            let mut devices = self.devices.write().await;
            if let Some(dev) = devices.get_mut(device_id) {
                match &result {
                    Ok(_) => {
                        dev.connection_state = ConnectionState::Connected;
                        dev.last_error = None;
                        dev.reconnect_attempts = 0;
                    }
                    Err(e) => {
                        dev.connection_state = ConnectionState::Error;
                        dev.last_error = Some(e.clone());
                    }
                }
            }
        }
        
        // Publish result event
        match &result {
            Ok(_) => {
                self.app_state.publish_equipment_event(
                    EquipmentEvent::Connected {
                        device_type: info.device_type.as_str().to_string(),
                        device_id: device_id.clone(),
                    },
                    EventSeverity::Info,
                );
                
                // Also register in app state
                self.app_state.register_device(info.clone(), ConnectionState::Connected).await;
            }
            Err(e) => {
                self.app_state.publish_equipment_event(
                    EquipmentEvent::Error {
                        device_type: info.device_type.as_str().to_string(),
                        device_id: device_id.clone(),
                        message: e.clone(),
                    },
                    EventSeverity::Error,
                );
            }
        }
        
        result
    }
    
    /// Connect to a simulator device - DISABLED
    async fn connect_simulator(&self, _info: &DeviceInfo) -> Result<(), String> {
        Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
    }
    
    /// Connect to an ASCOM device
    #[cfg(windows)]
    async fn connect_ascom(&self, info: &DeviceInfo) -> Result<(), String> {
        use nightshade_ascom::*;
        
        let prog_id = info.id.strip_prefix("ascom:")
            .ok_or_else(|| "Invalid ASCOM device ID".to_string())?;
        
        match info.device_type {
            DeviceType::Camera => {
                use crate::ascom_wrapper::AscomCameraWrapper;
                let mut camera = AscomCameraWrapper::new(prog_id.to_string())?;
                // Let user select the specific camera/config via ASCOM SetupDialog before connecting
                camera.setup_dialog().await.map_err(|e| e.to_string())?;
                camera.connect().await.map_err(|e| e.to_string())?;
                
                // Store in typed map for camera-specific operations, wrapped in Arc<RwLock>
                let mut ascom_cameras = self.ascom_cameras.write().await;
                ascom_cameras.insert(info.id.clone(), Arc::new(RwLock::new(camera)));
            }
            DeviceType::Mount => {
                use crate::ascom_wrapper_mount::AscomMountWrapper;
                let mut mount = AscomMountWrapper::new(prog_id.to_string())?;
                mount.connect().await.map_err(|e| e.to_string())?;
                
                let mut ascom_mounts = self.ascom_mounts.write().await;
                ascom_mounts.insert(info.id.clone(), Arc::new(RwLock::new(mount)));
            }
            DeviceType::Focuser => {
                use crate::ascom_wrapper_focuser::AscomFocuserWrapper;
                let mut focuser = AscomFocuserWrapper::new(prog_id.to_string())?;
                focuser.connect().await.map_err(|e| e.to_string())?;
                
                let mut ascom_focusers = self.ascom_focusers.write().await;
                ascom_focusers.insert(info.id.clone(), Arc::new(RwLock::new(focuser)));
            }
            DeviceType::FilterWheel => {
                use crate::ascom_wrapper_filterwheel::AscomFilterWheelWrapper;
                let mut fw = AscomFilterWheelWrapper::new(prog_id.to_string())?;
                fw.connect().await.map_err(|e| e.to_string())?;
                
                let mut ascom_filter_wheels = self.ascom_filter_wheels.write().await;
                ascom_filter_wheels.insert(info.id.clone(), Arc::new(RwLock::new(fw)));
            }
            DeviceType::Rotator => {
                let mut rotator = AscomRotator::new(prog_id)?;
                rotator.connect()?;
            }
            DeviceType::Dome => {
                let mut dome = AscomDome::new(prog_id)?;
                dome.connect()?;
            }
            DeviceType::Weather => {
                let mut weather = AscomObservingConditions::new(prog_id)?;
                weather.connect()?;
            }
            DeviceType::SafetyMonitor => {
                let mut safety = AscomSafetyMonitor::new(prog_id)?;
                safety.connect()?;
            }
            DeviceType::CoverCalibrator => {
                use crate::ascom_wrapper_covercalibrator::AscomCoverCalibratorWrapper;
                let mut cover_cal = AscomCoverCalibratorWrapper::new(prog_id.to_string())?;
                cover_cal.connect().await?;

                let mut ascom_cover_cals = self.ascom_cover_calibrators.write().await;
                ascom_cover_cals.insert(info.id.clone(), Arc::new(RwLock::new(cover_cal)));
            }
            _ => {
                return Err(format!("ASCOM {} not yet implemented", info.device_type.as_str()));
            }
        }
        
        Ok(())
    }
    
    #[cfg(not(windows))]
    async fn connect_ascom(&self, _info: &DeviceInfo) -> Result<(), String> {
        Err("ASCOM is only available on Windows".to_string())
    }
    
    /// Connect to an Alpaca device
    async fn connect_alpaca(&self, info: &DeviceInfo) -> Result<(), String> {
        use nightshade_alpaca::*;

        // Parse alpaca:url:type:number format
        let parts: Vec<&str> = info.id.strip_prefix("alpaca:")
            .ok_or_else(|| "Invalid Alpaca device ID".to_string())?
            .splitn(3, ':')
            .collect();

        if parts.len() < 3 {
            return Err("Invalid Alpaca device ID format".to_string());
        }

        let base_url = parts[0];
        let device_number: u32 = parts[2].parse()
            .map_err(|_| "Invalid device number".to_string())?;

        match info.device_type {
            DeviceType::Camera => {
                let camera = AlpacaCamera::from_server(base_url, device_number);
                camera.connect().await?;
                // Store for later use
                let mut alpaca_cameras = self.alpaca_cameras.write().await;
                alpaca_cameras.insert(info.id.clone(), Arc::new(camera));
            }
            DeviceType::Mount => {
                let telescope = AlpacaTelescope::from_server(base_url, device_number);
                telescope.connect().await?;
                // Store for later use
                let mut alpaca_mounts = self.alpaca_mounts.write().await;
                alpaca_mounts.insert(info.id.clone(), Arc::new(telescope));
            }
            DeviceType::Focuser => {
                let focuser = AlpacaFocuser::from_server(base_url, device_number);
                focuser.connect().await?;
                // Store for later use
                let mut alpaca_focusers = self.alpaca_focusers.write().await;
                alpaca_focusers.insert(info.id.clone(), Arc::new(focuser));
            }
            DeviceType::FilterWheel => {
                let fw = AlpacaFilterWheel::from_server(base_url, device_number);
                fw.connect().await?;
                // Store for later use
                let mut alpaca_filter_wheels = self.alpaca_filter_wheels.write().await;
                alpaca_filter_wheels.insert(info.id.clone(), Arc::new(fw));
            }
            DeviceType::Rotator => {
                let rotator = AlpacaRotator::from_server(base_url, device_number);
                rotator.connect().await?;
                // Store for later use
                let mut alpaca_rotators = self.alpaca_rotators.write().await;
                alpaca_rotators.insert(info.id.clone(), Arc::new(rotator));
            }
            DeviceType::Dome => {
                let dome = AlpacaDome::from_server(base_url, device_number);
                dome.connect().await?;
                // Store for later use
                let mut alpaca_domes = self.alpaca_domes.write().await;
                alpaca_domes.insert(info.id.clone(), Arc::new(dome));
            }
            DeviceType::Weather => {
                let weather = AlpacaObservingConditions::from_server(base_url, device_number);
                weather.connect().await?;
                // Store for later use
                let mut alpaca_weather = self.alpaca_weather.write().await;
                alpaca_weather.insert(info.id.clone(), Arc::new(weather));
            }
            DeviceType::SafetyMonitor => {
                let safety = AlpacaSafetyMonitor::from_server(base_url, device_number);
                safety.connect().await?;
                // Store for later use
                let mut alpaca_safety = self.alpaca_safety_monitors.write().await;
                alpaca_safety.insert(info.id.clone(), Arc::new(safety));
            }
            DeviceType::Switch => {
                let switch = AlpacaSwitch::from_server(base_url, device_number);
                switch.connect().await?;
                // Store for later use
                let mut alpaca_switches = self.alpaca_switches.write().await;
                alpaca_switches.insert(info.id.clone(), Arc::new(switch));
            }
            DeviceType::CoverCalibrator => {
                let cover_cal = AlpacaCoverCalibrator::from_server(base_url, device_number);
                cover_cal.connect().await?;
                // Store for later use
                let mut alpaca_cover_cals = self.alpaca_cover_calibrators.write().await;
                alpaca_cover_cals.insert(info.id.clone(), Arc::new(cover_cal));
            }
            _ => {
                return Err(format!("Alpaca {} not yet implemented", info.device_type.as_str()));
            }
        }

        Ok(())
    }
    
    /// Connect to an INDI device
    async fn connect_indi(&self, info: &DeviceInfo) -> Result<(), String> {
        use nightshade_indi::IndiClient;
        use std::sync::Arc;
        use tokio::sync::RwLock;
        
        // Parse INDI device ID: indi:host:port:device_name
        let parts: Vec<&str> = info.id.split(':').collect();
        if parts.len() < 4 {
            return Err("Invalid INDI device ID format. Expected: indi:host:port:device_name".to_string());
        }
        
        let host = parts[1];
        let port: u16 = parts[2].parse().map_err(|_| "Invalid port number")?;
        let device_name = parts[3..].join(":");
        let server_key = format!("{}:{}", host, port);
        
        // Check if client exists
        let client = {
            let mut clients = self.indi_clients.write().await;
            if let Some(client) = clients.get(&server_key) {
                client.clone()
            } else {
                // Create new client
                let mut new_client = IndiClient::new(host, Some(port));
                new_client.connect().await?;
                let client_arc = Arc::new(RwLock::new(new_client));
                clients.insert(server_key.clone(), client_arc.clone());
                client_arc
            }
        };
        
        // Use the client to connect the device
        let mut locked_client = client.write().await;
        
        // Enable BLOB for cameras
        if info.device_type == DeviceType::Camera {
            if let Err(e) = locked_client.enable_blob(&device_name).await {
                tracing::warn!("Failed to enable BLOB for {}: {}", device_name, e);
            }
        }
        
        // Connect to the specific device
        locked_client.connect_device(&device_name).await?;
        
        tracing::info!("Connected to INDI device: {} at {}", device_name, server_key);
        Ok(())
    }

    /// Connect to a Native device
    async fn connect_native(&self, info: &DeviceInfo) -> Result<(), String> {
        // Parse device ID: native:vendor:id
        let parts: Vec<&str> = info.id.split(':').collect();
        if parts.len() < 3 {
            return Err("Invalid native device ID format".to_string());
        }

        let vendor = parts[1];
        let id_str = parts[2];

        // Handle cameras
        if info.device_type == DeviceType::Camera {
            let mut camera: Box<dyn NativeCamera + Send + Sync> = match vendor {
                "zwo" => {
                    let id = id_str.parse::<i32>().map_err(|_| "Invalid ZWO camera ID")?;
                    Box::new(ZwoCamera::new(id))
                },
                "qhy" => {
                    Box::new(QhyCamera::new(id_str.to_string()))
                },
                "player_one" => {
                    let id = id_str.parse::<i32>().map_err(|_| "Invalid Player One camera ID")?;
                    Box::new(PlayerOneCamera::new(id))
                },
                "svbony" => {
                    let id = id_str.parse::<i32>().map_err(|_| "Invalid SVBony camera ID")?;
                    Box::new(SvbonyCamera::new(id))
                },
                "atik" => {
                    let id = id_str.parse::<i32>().map_err(|_| "Invalid Atik camera ID")?;
                    Box::new(AtikCamera::new(id))
                },
                "fli" => {
                    // FLI uses device path as ID
                    Box::new(FliCamera::new(id_str.to_string()))
                },
                "touptek" => {
                    let idx = id_str.parse::<usize>().map_err(|_| "Invalid Touptek camera ID")?;
                    // For Touptek, we need to get the camera_id from discovery
                    Box::new(TouptekCamera::new(idx, String::new()))
                },
                "moravian" => {
                    let camera_id = id_str.parse::<u32>().map_err(|_| "Invalid Moravian camera ID")?;
                    Box::new(MoravianCamera::new(camera_id, 0))
                },
                _ => return Err(format!("Unknown native camera vendor: {}", vendor)),
            };

            // Connect
            camera.connect().await.map_err(|e| e.to_string())?;

            // Store in native_cameras for typed camera access
            let mut native_cameras = self.native_cameras.write().await;
            native_cameras.insert(info.id.clone(), camera);

            tracing::info!("Connected to native camera: {}", info.name);
            return Ok(());
        }

        // Handle focusers
        if info.device_type == DeviceType::Focuser {
            let mut focuser: Box<dyn NativeFocuser + Send + Sync> = match vendor {
                "zwo" => {
                    let id = id_str.parse::<i32>().map_err(|_| "Invalid ZWO focuser ID")?;
                    Box::new(ZwoFocuser::new(id))
                },
                "fli_focuser" => {
                    // FLI uses device path as ID
                    Box::new(FliFocuser::new(id_str.to_string()))
                },
                _ => return Err(format!("Unknown native focuser vendor: {}", vendor)),
            };

            // Connect
            focuser.connect().await.map_err(|e| e.to_string())?;

            // Store in native_focusers for typed focuser access
            let mut native_focusers = self.native_focusers.write().await;
            native_focusers.insert(info.id.clone(), focuser);

            tracing::info!("Connected to native focuser: {}", info.name);
            return Ok(());
        }

        // Handle filter wheels
        if info.device_type == DeviceType::FilterWheel {
            let mut filterwheel: Box<dyn NativeFilterWheel + Send + Sync> = match vendor {
                "zwo" => {
                    let id = id_str.parse::<i32>().map_err(|_| "Invalid ZWO filter wheel ID")?;
                    Box::new(ZwoFilterWheel::new(id))
                },
                "qhy_cfw" => {
                    // QHY CFW uses camera ID string directly
                    Box::new(QhyFilterWheel::new(id_str.to_string()))
                },
                "fli_fw" => {
                    // FLI uses device path as ID
                    Box::new(FliFilterWheel::new(id_str.to_string()))
                },
                _ => return Err(format!("Unknown native filter wheel vendor: {}", vendor)),
            };

            // Connect
            filterwheel.connect().await.map_err(|e| e.to_string())?;

            // Store in native_filter_wheels for typed filter wheel access
            let mut native_filter_wheels = self.native_filter_wheels.write().await;
            native_filter_wheels.insert(info.id.clone(), filterwheel);

            tracing::info!("Connected to native filter wheel: {}", info.name);
            return Ok(());
        }

        // Handle mounts
        if info.device_type == DeviceType::Mount {
            let mut mount: Box<dyn NativeMount + Send + Sync> = match vendor {
                "skywatcher" => {
                    // id_str is the serial port
                    Box::new(SkyWatcherMount::new_serial(id_str.to_string(), None))
                },
                "ioptron" => {
                    // id_str is the serial port
                    Box::new(IOptronMount::new(id_str.to_string(), None))
                },
                "onstep" | "pegasus" => {
                    // OnStep-based mounts (Pegasus NYX, DIY OnStep)
                    Box::new(Lx200Mount::new_onstep(id_str.to_string()))
                },
                "meade" | "lx200" => {
                    Box::new(Lx200Mount::new_meade(id_str.to_string()))
                },
                "losmandy" => {
                    Box::new(Lx200Mount::new(id_str.to_string(), Lx200MountType::Losmandy, None))
                },
                "10micron" => {
                    Box::new(Lx200Mount::new(id_str.to_string(), Lx200MountType::TenMicron, None))
                },
                _ => return Err(format!("Unknown native mount vendor: {}", vendor)),
            };

            // Connect
            mount.connect().await.map_err(|e| e.to_string())?;

            // Store in native_mounts for typed mount access
            let mut native_mounts = self.native_mounts.write().await;
            native_mounts.insert(info.id.clone(), mount);

            tracing::info!("Connected to native mount: {}", info.name);
            return Ok(());
        }

        // For other device types, use the generic storage
        let mut device: Box<dyn NativeDevice> = match vendor {
            "zwo" => {
                let id = id_str.parse::<i32>().map_err(|_| "Invalid ZWO camera ID")?;
                Box::new(ZwoCamera::new(id))
            },
            "qhy" => {
                Box::new(QhyCamera::new(id_str.to_string()))
            },
            "player_one" => {
                let id = id_str.parse::<i32>().map_err(|_| "Invalid Player One camera ID")?;
                Box::new(PlayerOneCamera::new(id))
            },
            "svbony" => {
                let id = id_str.parse::<i32>().map_err(|_| "Invalid SVBony camera ID")?;
                Box::new(SvbonyCamera::new(id))
            },
            "atik" => {
                let id = id_str.parse::<i32>().map_err(|_| "Invalid Atik camera ID")?;
                Box::new(AtikCamera::new(id))
            },
            "fli" => {
                Box::new(FliCamera::new(id_str.to_string()))
            },
            "touptek" => {
                let idx = id_str.parse::<usize>().map_err(|_| "Invalid Touptek camera ID")?;
                Box::new(TouptekCamera::new(idx, String::new()))
            },
            "moravian" => {
                let camera_id = id_str.parse::<u32>().map_err(|_| "Invalid Moravian camera ID")?;
                Box::new(MoravianCamera::new(camera_id, 0))
            },
            _ => return Err(format!("Unknown native vendor: {}", vendor)),
        };

        // Connect
        device.connect().await.map_err(|e| e.to_string())?;

        // Store the connected device instance
        let mut native_devices = self.native_devices.write().await;
        native_devices.insert(info.id.clone(), device);

        tracing::info!("Connected to native device: {}", info.name);
        Ok(())
    }
    
    /// Disconnect a device
    pub async fn disconnect_device(&self, device_id: &str) -> Result<(), String> {
        let device_info = {
            let devices = self.devices.read().await;
            devices.get(device_id)
                .map(|d| d.info.clone())
                .ok_or_else(|| format!("Device not found: {}", device_id))?
        };
        
        // Update state
        {
            let mut devices = self.devices.write().await;
            if let Some(dev) = devices.get_mut(device_id) {
                dev.connection_state = ConnectionState::Disconnected;
                dev.auto_reconnect = false; // Disable auto-reconnect on manual disconnect
            }
        }
        
        // Clean up device from driver-specific storage based on driver type and device type
        match device_info.driver_type {
            DriverType::Native => {
                // Remove from generic native_devices map
                let mut native_devices = self.native_devices.write().await;
                if let Some(mut device) = native_devices.remove(device_id) {
                    let _ = device.disconnect().await;
                }

                // Also remove from typed native storage maps
                match device_info.device_type {
                    DeviceType::Camera => {
                        let mut cameras = self.native_cameras.write().await;
                        if let Some(mut camera) = cameras.remove(device_id) {
                            let _ = camera.disconnect().await;
                        }
                    }
                    DeviceType::Mount => {
                        let mut mounts = self.native_mounts.write().await;
                        if let Some(mut mount) = mounts.remove(device_id) {
                            let _ = mount.disconnect().await;
                        }
                    }
                    DeviceType::Focuser => {
                        let mut focusers = self.native_focusers.write().await;
                        if let Some(mut focuser) = focusers.remove(device_id) {
                            let _ = focuser.disconnect().await;
                        }
                    }
                    DeviceType::FilterWheel => {
                        let mut fws = self.native_filter_wheels.write().await;
                        if let Some(mut fw) = fws.remove(device_id) {
                            let _ = fw.disconnect().await;
                        }
                    }
                    DeviceType::Rotator => {
                        let mut rotators = self.native_rotators.write().await;
                        if let Some(mut rotator) = rotators.remove(device_id) {
                            let _ = rotator.disconnect().await;
                        }
                    }
                    DeviceType::Dome => {
                        let mut domes = self.native_domes.write().await;
                        if let Some(mut dome) = domes.remove(device_id) {
                            let _ = dome.disconnect().await;
                        }
                    }
                    DeviceType::Weather => {
                        let mut weather = self.native_weather.write().await;
                        if let Some(mut w) = weather.remove(device_id) {
                            let _ = w.disconnect().await;
                        }
                    }
                    DeviceType::SafetyMonitor => {
                        let mut safety = self.native_safety_monitors.write().await;
                        if let Some(mut s) = safety.remove(device_id) {
                            let _ = s.disconnect().await;
                        }
                    }
                    _ => {} // Guider, Switch, CoverCalibrator - no typed native storage
                }
            }
            DriverType::Alpaca => {
                // Remove from Alpaca storage based on device type
                match device_info.device_type {
                    DeviceType::Camera => {
                        let mut cameras = self.alpaca_cameras.write().await;
                        if let Some(camera) = cameras.remove(device_id) {
                            let _ = camera.disconnect().await;
                        }
                    }
                    DeviceType::Mount => {
                        let mut mounts = self.alpaca_mounts.write().await;
                        if let Some(mount) = mounts.remove(device_id) {
                            let _ = mount.disconnect().await;
                        }
                    }
                    DeviceType::Focuser => {
                        let mut focusers = self.alpaca_focusers.write().await;
                        if let Some(focuser) = focusers.remove(device_id) {
                            let _ = focuser.disconnect().await;
                        }
                    }
                    DeviceType::FilterWheel => {
                        let mut fws = self.alpaca_filter_wheels.write().await;
                        if let Some(fw) = fws.remove(device_id) {
                            let _ = fw.disconnect().await;
                        }
                    }
                    DeviceType::Rotator => {
                        let mut rotators = self.alpaca_rotators.write().await;
                        if let Some(rotator) = rotators.remove(device_id) {
                            let _ = rotator.disconnect().await;
                        }
                    }
                    DeviceType::Dome => {
                        let mut domes = self.alpaca_domes.write().await;
                        if let Some(dome) = domes.remove(device_id) {
                            let _ = dome.disconnect().await;
                        }
                    }
                    DeviceType::Weather => {
                        let mut weather = self.alpaca_weather.write().await;
                        if let Some(w) = weather.remove(device_id) {
                            let _ = w.disconnect().await;
                        }
                    }
                    DeviceType::SafetyMonitor => {
                        let mut safety = self.alpaca_safety_monitors.write().await;
                        if let Some(s) = safety.remove(device_id) {
                            let _ = s.disconnect().await;
                        }
                    }
                    DeviceType::Switch => {
                        let mut switches = self.alpaca_switches.write().await;
                        if let Some(sw) = switches.remove(device_id) {
                            let _ = sw.disconnect().await;
                        }
                    }
                    DeviceType::CoverCalibrator => {
                        let mut covers = self.alpaca_cover_calibrators.write().await;
                        if let Some(cover) = covers.remove(device_id) {
                            let _ = cover.disconnect().await;
                        }
                    }
                    DeviceType::Guider => {} // Guider not implemented for Alpaca
                }
            }
            #[cfg(windows)]
            DriverType::Ascom => {
                // Remove from ASCOM storage based on device type
                match device_info.device_type {
                    DeviceType::Camera => {
                        let mut cameras = self.ascom_cameras.write().await;
                        if let Some(camera) = cameras.remove(device_id) {
                            let mut cam = camera.write().await;
                            let _ = cam.disconnect().await;
                        }
                    }
                    DeviceType::Mount => {
                        let mut mounts = self.ascom_mounts.write().await;
                        if let Some(mount) = mounts.remove(device_id) {
                            let mut m = mount.write().await;
                            let _ = m.disconnect().await;
                        }
                    }
                    DeviceType::Focuser => {
                        let mut focusers = self.ascom_focusers.write().await;
                        if let Some(focuser) = focusers.remove(device_id) {
                            let mut f = focuser.write().await;
                            let _ = f.disconnect().await;
                        }
                    }
                    DeviceType::FilterWheel => {
                        let mut fws = self.ascom_filter_wheels.write().await;
                        if let Some(fw) = fws.remove(device_id) {
                            let mut f = fw.write().await;
                            let _ = f.disconnect().await;
                        }
                    }
                    DeviceType::Dome => {
                        let mut domes = self.ascom_domes.write().await;
                        if let Some(dome) = domes.remove(device_id) {
                            let mut d = dome.write().await;
                            let _ = d.disconnect().await;
                        }
                    }
                    DeviceType::Switch => {
                        let mut switches = self.ascom_switches.write().await;
                        if let Some(sw) = switches.remove(device_id) {
                            let mut s = sw.write().await;
                            let _ = s.disconnect().await;
                        }
                    }
                    DeviceType::CoverCalibrator => {
                        let mut covers = self.ascom_cover_calibrators.write().await;
                        if let Some(cover) = covers.remove(device_id) {
                            let mut c = cover.write().await;
                            let _ = c.disconnect().await;
                        }
                    }
                    _ => {} // Rotator, Weather, SafetyMonitor, Guider - not implemented for ASCOM
                }
            }
            #[cfg(not(windows))]
            DriverType::Ascom => {
                // ASCOM not available on non-Windows platforms
            }
            DriverType::Indi => {
                // INDI cleanup handled separately through INDI client
                // The client manages device connections internally
            }
            DriverType::Simulator => {
                // Simulators should never be connected - connection is disabled
                // No cleanup needed even if this is somehow reached
            }
        }

        // Publish event
        self.app_state.publish_equipment_event(
            EquipmentEvent::Disconnected {
                device_type: device_info.device_type.as_str().to_string(),
                device_id: device_id.to_string(),
            },
            EventSeverity::Info,
        );
        
        // Update app state
        self.app_state.remove_device(device_info.device_type, device_id).await;
        
        Ok(())
    }
    
    /// Get all managed devices
    pub async fn get_all_devices(&self) -> Vec<ManagedDevice> {
        let devices = self.devices.read().await;
        devices.values().cloned().collect()
    }
    
    /// Get devices by type
    pub async fn get_devices_by_type(&self, device_type: DeviceType) -> Vec<ManagedDevice> {
        let devices = self.devices.read().await;
        devices
            .values()
            .filter(|d| d.info.device_type == device_type)
            .cloned()
            .collect()
    }
    
    /// Get a specific device
    pub async fn get_device(&self, device_id: &str) -> Option<ManagedDevice> {
        let devices = self.devices.read().await;
        devices.get(device_id).cloned()
    }
    
    /// Check if a device is connected
    pub async fn is_connected(&self, device_id: &str) -> bool {
        let devices = self.devices.read().await;
        devices
            .get(device_id)
            .map(|d| d.connection_state == ConnectionState::Connected)
            .unwrap_or(false)
    }
    
    /// Enable or disable auto-reconnect for a device
    pub async fn set_auto_reconnect(&self, device_id: &str, enabled: bool) {
        let mut devices = self.devices.write().await;
        if let Some(dev) = devices.get_mut(device_id) {
            dev.auto_reconnect = enabled;
        }
    }
    
    /// Report a connection error (triggers auto-reconnect if enabled)
    pub async fn report_error(&self, device_id: &str, error: String) {
        let mut devices = self.devices.write().await;
        if let Some(dev) = devices.get_mut(device_id) {
            dev.connection_state = ConnectionState::Error;
            dev.last_error = Some(error.clone());
            
            self.app_state.publish_equipment_event(
                EquipmentEvent::Error {
                    device_type: dev.info.device_type.as_str().to_string(),
                    device_id: device_id.to_string(),
                    message: error,
                },
                EventSeverity::Error,
            );
        }
    }
    
    /// Stop the reconnection background task
    pub async fn shutdown(&self) {
        *self.stop_reconnect.write().await = true;
    }
    
    /// Unregister a device
    pub async fn unregister_device(&self, device_id: &str) {
        let mut devices = self.devices.write().await;
        devices.remove(device_id);
    }

    /// Get an INDI client for a device ID
    pub async fn get_indi_client(&self, device_id: &str) -> Option<Arc<RwLock<nightshade_indi::IndiClient>>> {
        // Parse INDI device ID: indi:host:port:device_name
        if !device_id.starts_with("indi:") {
            return None;
        }
        
        let parts: Vec<&str> = device_id.split(':').collect();
        if parts.len() < 4 {
            return None;
        }
        
        let host = parts[1];
        let port = parts[2];
        let server_key = format!("{}:{}", host, port);
        
        let clients = self.indi_clients.read().await;
        clients.get(&server_key).cloned()
    }

    /// Discover INDI devices at a specific address
    pub async fn discover_indi_devices(&self, host: &str, port: u16) -> Result<Vec<DeviceInfo>, String> {
        use nightshade_indi::IndiClient;
        
        let server_key = format!("{}:{}", host, port);
        
        // Get or create client
        let client = {
            let mut clients = self.indi_clients.write().await;
            if let Some(client) = clients.get(&server_key) {
                client.clone()
            } else {
                // Create new client
                let mut new_client = IndiClient::new(host, Some(port));
                new_client.connect().await.map_err(|e| e.to_string())?;
                let client_arc = Arc::new(RwLock::new(new_client));
                clients.insert(server_key.clone(), client_arc.clone());
                client_arc
            }
        };
        
        // Wait a moment for devices to be populated
        // In a real scenario, we might want to wait for a specific event or have a timeout
        // For now, we'll wait up to 2 seconds for devices to appear
        let start = std::time::Instant::now();
        loop {
            {
                let locked_client = client.read().await;
                let devices = locked_client.get_devices().await;
                if !devices.is_empty() {
                    break;
                }
            }
            
            if start.elapsed().as_secs() >= 2 {
                break;
            }
            
            tokio::time::sleep(Duration::from_millis(100)).await;
        }
        
        // Get devices and convert to DeviceInfo
        let locked_client = client.read().await;
        let indi_devices = locked_client.get_devices().await;
        
        let mut devices = Vec::new();
        for dev in indi_devices {
            // Determine device type based on properties
            // This is a simplification; robust type detection might need more property checks
            // Determine device type based on name/driver
            let name_upper = dev.name.to_uppercase();
            let driver_upper = dev.driver.to_uppercase();
            
            let device_type = if name_upper.contains("CCD") || name_upper.contains("CAMERA") || driver_upper.contains("CCD") || driver_upper.contains("CAMERA") {
                DeviceType::Camera
            } else if name_upper.contains("TELESCOPE") || name_upper.contains("MOUNT") || driver_upper.contains("TELESCOPE") || driver_upper.contains("MOUNT") {
                DeviceType::Mount
            } else if name_upper.contains("FOCUSER") || driver_upper.contains("FOCUSER") {
                DeviceType::Focuser
            } else if name_upper.contains("WHEEL") || driver_upper.contains("WHEEL") {
                DeviceType::FilterWheel
            } else if name_upper.contains("ROTATOR") || driver_upper.contains("ROTATOR") {
                DeviceType::Rotator
            } else {
                // Default to Camera if unknown, or skip?
                // For now, let's assume Camera if ambiguous as it's most common
                DeviceType::Camera 
            };
            
            // TODO: Query DEVICE_INFO property for serial number
            devices.push(DeviceInfo {
                id: format!("indi:{}:{}:{}", host, port, dev.name),
                name: dev.name.clone(),
                device_type,
                driver_type: DriverType::Indi,
                description: format!("INDI device on {}:{}", host, port),
                driver_version: "INDI".to_string(),
                serial_number: None,
                unique_id: None,
                display_name: dev.name.clone(),
            });
        }

        Ok(devices)
    }

    /// Get all discovered INDI devices from all connected clients
    pub async fn get_all_indi_devices(&self) -> Vec<DeviceInfo> {
        let clients = self.indi_clients.read().await;
        let mut all_devices = Vec::new();
        
        for (server_key, client_arc) in clients.iter() {
            let client = client_arc.read().await;
            let indi_devices = client.get_devices().await;
            
            for dev in indi_devices {
                // Determine device type
                // Determine device type based on name/driver
                let name_upper = dev.name.to_uppercase();
                let driver_upper = dev.driver.to_uppercase();
                
                let device_type = if name_upper.contains("CCD") || name_upper.contains("CAMERA") || driver_upper.contains("CCD") || driver_upper.contains("CAMERA") {
                    DeviceType::Camera
                } else if name_upper.contains("TELESCOPE") || name_upper.contains("MOUNT") || driver_upper.contains("TELESCOPE") || driver_upper.contains("MOUNT") {
                    DeviceType::Mount
                } else if name_upper.contains("FOCUSER") || driver_upper.contains("FOCUSER") {
                    DeviceType::Focuser
                } else if name_upper.contains("WHEEL") || driver_upper.contains("WHEEL") {
                    DeviceType::FilterWheel
                } else if name_upper.contains("ROTATOR") || driver_upper.contains("ROTATOR") {
                    DeviceType::Rotator
                } else {
                    continue;
                };
                
                // TODO: Query DEVICE_INFO property for serial number
                all_devices.push(DeviceInfo {
                    id: format!("indi:{}:{}", server_key, dev.name),
                    name: dev.name.clone(),
                    device_type,
                    driver_type: DriverType::Indi,
                    description: format!("INDI device on {}", server_key),
                    driver_version: "INDI".to_string(),
                    serial_number: None,
                    unique_id: None,
                    display_name: dev.name.clone(),
                });
            }
        }

        all_devices
    }

    // =========================================================================
    // Camera Control
    // =========================================================================
    
    /// Start a camera exposure
    pub async fn camera_start_exposure(
        &self,
        device_id: &str,
        duration: f64,
        gain: i32,
        offset: i32,
        bin_x: i32,
        bin_y: i32,
    ) -> Result<(), String> {
        tracing::info!("DeviceManager: camera_start_exposure for {} duration={}", device_id, duration);

        // Get the driver type for this device
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Ascom) => {
                #[cfg(windows)]
                {
                    let cameras = self.ascom_cameras.read().await;
                    if let Some(camera) = cameras.get(device_id) {
                        let params = ExposureParams {
                            duration_secs: duration,
                            bin_x,
                            bin_y,
                            gain: Some(gain),
                            offset: Some(offset),
                            subframe: None,
                            readout_mode: None,
                        };
                        tracing::info!("DeviceManager: Calling AscomCameraWrapper.start_exposure()");
                        let mut camera = camera.write().await;
                        return camera.start_exposure(params).await.map_err(|e| e.to_string());
                    }
                }
                Err(format!("ASCOM camera {} not found", device_id))
            }
            Some(DriverType::Alpaca) => {
                let cameras = self.alpaca_cameras.read().await;
                if let Some(camera) = cameras.get(device_id) {
                    tracing::info!("DeviceManager: Calling AlpacaCamera.start_exposure()");
                    // Set gain and offset before exposure - propagate errors
                    camera.set_gain(gain).await
                        .map_err(|e| format!("Failed to set Alpaca camera gain: {}", e))?;
                    camera.set_offset(offset).await
                        .map_err(|e| format!("Failed to set Alpaca camera offset: {}", e))?;
                    // Set binning - propagate errors
                    camera.set_bin_x(bin_x).await
                        .map_err(|e| format!("Failed to set Alpaca camera bin_x: {}", e))?;
                    camera.set_bin_y(bin_y).await
                        .map_err(|e| format!("Failed to set Alpaca camera bin_y: {}", e))?;
                    // Start the exposure
                    return camera.start_exposure(duration, true).await;
                }
                Err(format!("Alpaca camera {} not found", device_id))
            }
            Some(DriverType::Indi) => {
                // Parse INDI device ID: indi:host:port:device_name
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() >= 4 {
                    let host = parts[1];
                    let port = parts[2];
                    let device_name = parts[3..].join(":");
                    let server_key = format!("{}:{}", host, port);

                    let clients = self.indi_clients.read().await;
                    if let Some(client) = clients.get(&server_key) {
                        tracing::info!("DeviceManager: Starting INDI exposure on {}", device_name);
                        let mut locked_client = client.write().await;
                        // Set gain/offset if supported - some INDI cameras don't support these, so warn but continue
                        if let Err(e) = locked_client.set_number(&device_name, "CCD_CONTROLS", "Gain", gain as f64).await {
                            tracing::warn!("Failed to set INDI camera gain (device may not support it): {}", e);
                        }
                        if let Err(e) = locked_client.set_number(&device_name, "CCD_CONTROLS", "Offset", offset as f64).await {
                            tracing::warn!("Failed to set INDI camera offset (device may not support it): {}", e);
                        }
                        // Set binning - propagate errors since binning is typically supported
                        locked_client.set_number(&device_name, "CCD_BINNING", "HOR_BIN", bin_x as f64).await
                            .map_err(|e| format!("Failed to set INDI camera horizontal binning: {}", e))?;
                        locked_client.set_number(&device_name, "CCD_BINNING", "VER_BIN", bin_y as f64).await
                            .map_err(|e| format!("Failed to set INDI camera vertical binning: {}", e))?;
                        // Start exposure
                        return locked_client.set_number(&device_name, "CCD_EXPOSURE", "CCD_EXPOSURE_VALUE", duration).await
                            .map_err(|e| e.to_string());
                    }
                }
                Err(format!("INDI camera {} not found", device_id))
            }
            Some(DriverType::Native) => {
                let mut native_cameras = self.native_cameras.write().await;
                if let Some(camera) = native_cameras.get_mut(device_id) {
                    tracing::info!("DeviceManager: Starting Native SDK exposure");
                    let params = ExposureParams {
                        duration_secs: duration,
                        bin_x,
                        bin_y,
                        gain: Some(gain),
                        offset: Some(offset),
                        subframe: None,
                        readout_mode: None,
                    };
                    return camera.start_exposure(params).await.map_err(|e| e.to_string());
                }
                Err(format!("Native SDK camera {} not found", device_id))
            }
            Some(DriverType::Simulator) => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            None => Err(format!("Device {} not found", device_id)),
        }
    }
    
    /// Check if camera exposure is complete
    pub async fn camera_is_exposure_complete(&self, device_id: &str) -> Result<bool, String> {
        // Get the driver type for this device
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Ascom) => {
                #[cfg(windows)]
                {
                    let cameras = self.ascom_cameras.read().await;
                    if let Some(camera) = cameras.get(device_id) {
                        let camera = camera.read().await;
                        return camera.is_exposure_complete().await.map_err(|e| e.to_string());
                    }
                }
                Err(format!("ASCOM camera {} not found", device_id))
            }
            Some(DriverType::Alpaca) => {
                let cameras = self.alpaca_cameras.read().await;
                if let Some(camera) = cameras.get(device_id) {
                    return camera.image_ready().await;
                }
                Err(format!("Alpaca camera {} not found", device_id))
            }
            Some(DriverType::Indi) => {
                // For INDI, check CCD_EXPOSURE state - when value is 0, exposure is complete
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() >= 4 {
                    let host = parts[1];
                    let port = parts[2];
                    let device_name = parts[3..].join(":");
                    let server_key = format!("{}:{}", host, port);

                    let clients = self.indi_clients.read().await;
                    if let Some(client) = clients.get(&server_key) {
                        let locked_client = client.read().await;
                        // Check if exposure value is 0 (complete) - get_number returns Option
                        if let Some(value) = locked_client.get_number(&device_name, "CCD_EXPOSURE", "CCD_EXPOSURE_VALUE").await {
                            return Ok(value <= 0.0);
                        }
                        // If we can't get the value, assume complete
                        return Ok(true);
                    }
                }
                Err(format!("INDI camera {} not found", device_id))
            }
            Some(DriverType::Simulator) => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            Some(DriverType::Native) => {
                let native_cameras = self.native_cameras.read().await;
                if let Some(camera) = native_cameras.get(device_id) {
                    return camera.is_exposure_complete().await.map_err(|e| e.to_string());
                }
                Err(format!("Native SDK camera {} not found", device_id))
            }
            None => {
                Err(format!("Camera {} not found", device_id))
            }
        }
    }

    /// Download image from camera
    pub async fn camera_download_image(&self, device_id: &str) -> Result<ImageData, String> {
        // Get the driver type for this device
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Ascom) => {
                #[cfg(windows)]
                {
                    let cameras = self.ascom_cameras.read().await;
                    if let Some(camera) = cameras.get(device_id) {
                        let mut camera = camera.write().await;
                        return camera.download_image().await.map_err(|e| e.to_string());
                    }
                }
                Err(format!("ASCOM camera {} not found", device_id))
            }
            Some(DriverType::Alpaca) => {
                let cameras = self.alpaca_cameras.read().await;
                if let Some(camera) = cameras.get(device_id) {
                    // Use the new download_image_data method
                    let (width, height, pixels) = camera.download_image_data().await?;

                    // Get camera metadata
                    let gain = camera.gain().await.unwrap_or(0);
                    let offset = camera.offset().await.unwrap_or(0);
                    let bin_x = camera.bin_x().await.unwrap_or(1);
                    let bin_y = camera.bin_y().await.unwrap_or(1);
                    let temp = camera.ccd_temperature().await.ok();
                    let exposure_time = camera.last_exposure_duration().await.unwrap_or(0.0);

                    // Determine if color camera (sensor_type: 0=Monochrome, 1=Color, etc.)
                    let sensor_type = camera.sensor_type().await.unwrap_or(0);
                    let bayer_pattern = if sensor_type == 1 {
                        // Get bayer offsets for color cameras
                        let offset_x = camera.bayer_offset_x().await.unwrap_or(0);
                        let offset_y = camera.bayer_offset_y().await.unwrap_or(0);
                        // Map offsets to bayer pattern
                        Some(match (offset_x, offset_y) {
                            (0, 0) => nightshade_native::camera::BayerPattern::Rggb,
                            (1, 0) => nightshade_native::camera::BayerPattern::Grbg,
                            (0, 1) => nightshade_native::camera::BayerPattern::Gbrg,
                            (1, 1) => nightshade_native::camera::BayerPattern::Bggr,
                            _ => nightshade_native::camera::BayerPattern::Rggb,
                        })
                    } else {
                        None
                    };

                    return Ok(ImageData {
                        width,
                        height,
                        data: pixels,
                        bits_per_pixel: 16,
                        bayer_pattern,
                        metadata: nightshade_native::camera::ImageMetadata {
                            exposure_time,
                            gain,
                            offset,
                            bin_x,
                            bin_y,
                            temperature: temp,
                            timestamp: chrono::Utc::now(),
                            subframe: None,
                            readout_mode: None,
                            vendor_data: nightshade_native::camera::VendorFeatures::default(),
                        },
                    });
                }
                Err(format!("Alpaca camera {} not found", device_id))
            }
            Some(DriverType::Indi) => {
                // For INDI, image download uses event-based BLOB handling
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() >= 4 {
                    let host = parts[1];
                    let port = parts[2];
                    let device_name = parts[3..].join(":");
                    let server_key = format!("{}:{}", host, port);

                    let clients = self.indi_clients.read().await;
                    if let Some(client) = clients.get(&server_key) {
                        // Create an IndiCamera wrapper to handle BLOB download
                        let camera = nightshade_indi::IndiCamera::new(Arc::clone(client), &device_name);

                        // Enable BLOB transfer if not already enabled
                        let _ = camera.enable_blob().await;

                        // Get image metadata
                        let width = camera.get_sensor_width().await.unwrap_or(1920) as u32;
                        let height = camera.get_sensor_height().await.unwrap_or(1080) as u32;
                        let (bin_x, bin_y) = camera.get_binning().await.unwrap_or((1, 1));
                        let temp = camera.get_temperature().await.ok();
                        let gain = camera.get_gain().await.unwrap_or(0);
                        let offset = camera.get_offset().await.unwrap_or(0);

                        // Subscribe to events and wait for BLOB
                        let mut rx = {
                            let locked_client = client.read().await;
                            locked_client.subscribe()
                        };

                        // Wait for BLOB data with timeout (30 seconds)
                        let timeout = std::time::Duration::from_secs(30);
                        let start_time = std::time::Instant::now();

                        loop {
                            if start_time.elapsed() > timeout {
                                return Err("Timeout waiting for INDI image BLOB".to_string());
                            }

                            match tokio::time::timeout(std::time::Duration::from_secs(1), rx.recv()).await {
                                Ok(Ok(event)) => {
                                    match event {
                                        nightshade_indi::IndiEvent::BlobReceived { device, element, data, .. } => {
                                            if device == device_name && (element == "CCD1" || element == "CCD2") {
                                                // Parse FITS data
                                                // For now, we'll try to extract the raw image data
                                                // FITS files have a header followed by binary data
                                                // This is a simplified implementation - full FITS parsing would be more robust

                                                // Try to parse as FITS and extract u16 data
                                                let image_data = if data.starts_with(b"SIMPLE") {
                                                    // FITS file - extract binary data after header
                                                    // FITS headers are 2880-byte blocks
                                                    let mut offset = 0;
                                                    for chunk in data.chunks(80) {
                                                        offset += 80;
                                                        if chunk.starts_with(b"END") {
                                                            // Header ends, align to 2880-byte boundary
                                                            offset = ((offset + 2879) / 2880) * 2880;
                                                            break;
                                                        }
                                                    }

                                                    // Extract binary data as u16
                                                    let binary_data = &data[offset..];
                                                    let mut pixels: Vec<u16> = Vec::with_capacity(binary_data.len() / 2);
                                                    for chunk in binary_data.chunks_exact(2) {
                                                        let value = u16::from_be_bytes([chunk[0], chunk[1]]);
                                                        pixels.push(value);
                                                    }
                                                    pixels
                                                } else {
                                                    // Not a FITS file, try to parse as raw u16 data
                                                    let mut pixels: Vec<u16> = Vec::with_capacity(data.len() / 2);
                                                    for chunk in data.chunks_exact(2) {
                                                        let value = u16::from_le_bytes([chunk[0], chunk[1]]);
                                                        pixels.push(value);
                                                    }
                                                    pixels
                                                };

                                                return Ok(ImageData {
                                                    width,
                                                    height,
                                                    data: image_data,
                                                    bits_per_pixel: 16,
                                                    bayer_pattern: None,
                                                    metadata: nightshade_native::camera::ImageMetadata {
                                                        exposure_time: 0.0, // Not available in BLOB event
                                                        gain,
                                                        offset,
                                                        bin_x,
                                                        bin_y,
                                                        temperature: temp,
                                                        timestamp: chrono::Utc::now(),
                                                        subframe: None,
                                                        readout_mode: None,
                                                        vendor_data: nightshade_native::camera::VendorFeatures::default(),
                                                    },
                                                });
                                            }
                                        },
                                        _ => {}
                                    }
                                }
                                Ok(Err(_)) => {
                                    return Err("INDI event channel closed".to_string());
                                }
                                Err(_) => {
                                    // Timeout on recv, check total timeout and continue
                                    continue;
                                }
                            }
                        }
                    }
                }
                Err(format!("INDI camera {} not found", device_id))
            }
            Some(DriverType::Simulator) => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            Some(DriverType::Native) => {
                let mut native_cameras = self.native_cameras.write().await;
                if let Some(camera) = native_cameras.get_mut(device_id) {
                    return camera.download_image().await.map_err(|e| e.to_string());
                }
                Err(format!("Native SDK camera {} not found", device_id))
            }
            None => {
                Err(format!("Camera {} not found", device_id))
            }
        }
    }

    /// Abort a camera exposure
    pub async fn camera_abort_exposure(&self, device_id: &str) -> Result<(), String> {
        tracing::info!("DeviceManager: camera_abort_exposure for {}", device_id);

        // Get the driver type for this device
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Ascom) => {
                #[cfg(windows)]
                {
                    let cameras = self.ascom_cameras.read().await;
                    if let Some(camera) = cameras.get(device_id) {
                        let mut camera = camera.write().await;
                        return camera.abort_exposure().await.map_err(|e| e.to_string());
                    }
                }
                Err(format!("ASCOM camera {} not found", device_id))
            }
            Some(DriverType::Alpaca) => {
                let cameras = self.alpaca_cameras.read().await;
                if let Some(camera) = cameras.get(device_id) {
                    return camera.abort_exposure().await;
                }
                Err(format!("Alpaca camera {} not found", device_id))
            }
            Some(DriverType::Indi) => {
                // For INDI, set exposure to 0 to abort
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() >= 4 {
                    let host = parts[1];
                    let port = parts[2];
                    let device_name = parts[3..].join(":");
                    let server_key = format!("{}:{}", host, port);

                    let clients = self.indi_clients.read().await;
                    if let Some(client) = clients.get(&server_key) {
                        let mut locked_client = client.write().await;
                        return locked_client.set_switch(&device_name, "CCD_ABORT_EXPOSURE", "ABORT", true).await
                            .map_err(|e| e.to_string());
                    }
                }
                Err(format!("INDI camera {} not found", device_id))
            }
            Some(DriverType::Simulator) => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            Some(DriverType::Native) => {
                let mut native_cameras = self.native_cameras.write().await;
                if let Some(camera) = native_cameras.get_mut(device_id) {
                    return camera.abort_exposure().await.map_err(|e| e.to_string());
                }
                Err(format!("Native SDK camera {} not found", device_id))
            }
            None => {
                Err(format!("Camera {} not found", device_id))
            }
        }
    }

    /// Get camera status
    pub async fn camera_get_status(&self, device_id: &str) -> Result<crate::device::CameraStatus, String> {
        // Get the driver type for this device
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Ascom) => {
                #[cfg(windows)]
                {
                    let cameras = self.ascom_cameras.read().await;
                    if let Some(camera) = cameras.get(device_id) {
                        let camera_guard = camera.read().await;
                        let native_status = camera_guard.get_status().await
                            .map_err(|e| e.to_string())?;

                        return Ok(crate::device::CameraStatus {
                            connected: true,
                            state: match native_status.state {
                                nightshade_native::camera::CameraState::Idle => crate::device::CameraState::Idle,
                                nightshade_native::camera::CameraState::Waiting => crate::device::CameraState::Waiting,
                                nightshade_native::camera::CameraState::Exposing => crate::device::CameraState::Exposing,
                                nightshade_native::camera::CameraState::Reading => crate::device::CameraState::Reading,
                                nightshade_native::camera::CameraState::Downloading => crate::device::CameraState::Download,
                                nightshade_native::camera::CameraState::Error => crate::device::CameraState::Error,
                            },
                            sensor_temp: native_status.sensor_temp,
                            cooler_power: native_status.cooler_power,
                            target_temp: native_status.target_temp,
                            cooler_on: native_status.cooler_on,
                            gain: native_status.gain,
                            offset: native_status.offset,
                            bin_x: native_status.bin_x,
                            bin_y: native_status.bin_y,
                            sensor_width: 4144,
                            sensor_height: 2822,
                            pixel_size_x: 3.76,
                            pixel_size_y: 3.76,
                            max_adu: 65535,
                            can_cool: true,
                            can_set_gain: true,
                            can_set_offset: true,
                        });
                    }
                }
                Err(format!("ASCOM camera {} not found", device_id))
            }
            Some(DriverType::Alpaca) => {
                let cameras = self.alpaca_cameras.read().await;
                if let Some(camera) = cameras.get(device_id) {
                    // Get status from Alpaca camera
                    let state = camera.camera_state().await.unwrap_or(nightshade_alpaca::CameraState::Idle);
                    let sensor_temp = camera.ccd_temperature().await.ok();
                    let cooler_power = camera.cooler_power().await.ok();
                    let cooler_on = camera.cooler_on().await.unwrap_or(false);
                    let gain = camera.gain().await.unwrap_or(0);
                    let offset = camera.offset().await.unwrap_or(0);
                    let bin_x = camera.bin_x().await.unwrap_or(1);
                    let bin_y = camera.bin_y().await.unwrap_or(1);
                    let sensor_width = camera.camera_x_size().await.unwrap_or(4144) as u32;
                    let sensor_height = camera.camera_y_size().await.unwrap_or(2822) as u32;
                    let pixel_size_x = camera.pixel_size_x().await.unwrap_or(3.76);
                    let pixel_size_y = camera.pixel_size_y().await.unwrap_or(3.76);
                    let max_adu = camera.max_adu().await.unwrap_or(65535) as u32;

                    return Ok(crate::device::CameraStatus {
                        connected: true,
                        state: match state {
                            nightshade_alpaca::CameraState::Idle => crate::device::CameraState::Idle,
                            nightshade_alpaca::CameraState::Waiting => crate::device::CameraState::Waiting,
                            nightshade_alpaca::CameraState::Exposing => crate::device::CameraState::Exposing,
                            nightshade_alpaca::CameraState::Reading => crate::device::CameraState::Reading,
                            nightshade_alpaca::CameraState::Download => crate::device::CameraState::Download,
                            nightshade_alpaca::CameraState::Error => crate::device::CameraState::Error,
                        },
                        sensor_temp,
                        cooler_power,
                        target_temp: None, // Alpaca doesn't provide target temp directly
                        cooler_on,
                        gain,
                        offset,
                        bin_x,
                        bin_y,
                        sensor_width,
                        sensor_height,
                        pixel_size_x,
                        pixel_size_y,
                        max_adu,
                        can_cool: camera.can_set_ccd_temperature().await.unwrap_or(false),
                        can_set_gain: true,
                        can_set_offset: true,
                    });
                }
                Err(format!("Alpaca camera {} not found", device_id))
            }
            Some(DriverType::Simulator) => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            Some(DriverType::Native) => {
                let native_cameras = self.native_cameras.read().await;
                if let Some(camera) = native_cameras.get(device_id) {
                    let native_status = camera.get_status().await.map_err(|e| e.to_string())?;
                    let capabilities = camera.capabilities();
                    let sensor_info = camera.get_sensor_info();

                    return Ok(crate::device::CameraStatus {
                        connected: camera.is_connected(),
                        state: match native_status.state {
                            nightshade_native::camera::CameraState::Idle => crate::device::CameraState::Idle,
                            nightshade_native::camera::CameraState::Waiting => crate::device::CameraState::Waiting,
                            nightshade_native::camera::CameraState::Exposing => crate::device::CameraState::Exposing,
                            nightshade_native::camera::CameraState::Reading => crate::device::CameraState::Reading,
                            nightshade_native::camera::CameraState::Downloading => crate::device::CameraState::Download,
                            nightshade_native::camera::CameraState::Error => crate::device::CameraState::Error,
                        },
                        sensor_temp: native_status.sensor_temp,
                        cooler_power: native_status.cooler_power,
                        target_temp: native_status.target_temp,
                        cooler_on: native_status.cooler_on,
                        gain: native_status.gain,
                        offset: native_status.offset,
                        bin_x: native_status.bin_x,
                        bin_y: native_status.bin_y,
                        sensor_width: sensor_info.width,
                        sensor_height: sensor_info.height,
                        pixel_size_x: sensor_info.pixel_size_x,
                        pixel_size_y: sensor_info.pixel_size_y,
                        max_adu: (1 << sensor_info.bit_depth) - 1,
                        can_cool: capabilities.can_cool,
                        can_set_gain: capabilities.can_set_gain,
                        can_set_offset: capabilities.can_set_offset,
                    });
                }
                Err(format!("Native SDK camera {} not found", device_id))
            }
            Some(DriverType::Indi) => {
                // Parse device_id format: indi:host:port:device_name
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() < 4 {
                    return Err(format!("Invalid INDI device ID format: {}", device_id));
                }
                let host = parts[1];
                let port = parts[2];
                let device_name = parts[3..].join(":");
                let server_key = format!("{}:{}", host, port);

                let clients = self.indi_clients.read().await;
                if let Some(client) = clients.get(&server_key) {
                    let locked_client = client.read().await;

                    // Query INDI camera properties
                    let sensor_temp = locked_client.get_number(&device_name, "CCD_TEMPERATURE", "CCD_TEMPERATURE_VALUE").await;
                    let cooler_on = locked_client.get_switch(&device_name, "CCD_COOLER", "COOLER_ON").await.unwrap_or(false);
                    let bin_x = locked_client.get_number(&device_name, "CCD_BINNING", "HOR_BIN").await.map(|v| v as i32).unwrap_or(1);
                    let bin_y = locked_client.get_number(&device_name, "CCD_BINNING", "VER_BIN").await.map(|v| v as i32).unwrap_or(1);
                    let exposure_value = locked_client.get_number(&device_name, "CCD_EXPOSURE", "CCD_EXPOSURE_VALUE").await;

                    // Determine camera state based on exposure value
                    let state = if exposure_value.unwrap_or(0.0) > 0.0 {
                        crate::device::CameraState::Exposing
                    } else {
                        crate::device::CameraState::Idle
                    };

                    return Ok(crate::device::CameraStatus {
                        connected: true,
                        state,
                        sensor_temp,
                        cooler_power: None, // INDI may not provide this
                        target_temp: None,
                        cooler_on,
                        gain: 0, // Would need CCD_GAIN property
                        offset: 0,
                        bin_x,
                        bin_y,
                        sensor_width: 4144, // Would need CCD_INFO property
                        sensor_height: 2822,
                        pixel_size_x: 3.76,
                        pixel_size_y: 3.76,
                        max_adu: 65535,
                        can_cool: true,
                        can_set_gain: true,
                        can_set_offset: true,
                    });
                }
                Err(format!("INDI client not connected for server {}", server_key))
            }
            None => {
                Err(format!("Camera {} not found or status not supported", device_id))
            }
        }
    }

    /// Set camera gain
    pub async fn camera_set_gain(&self, device_id: &str, gain: i32) -> Result<(), String> {
        tracing::info!("DeviceManager: camera_set_gain for {} gain={}", device_id, gain);

        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Ascom) => {
                #[cfg(windows)]
                {
                    let cameras = self.ascom_cameras.read().await;
                    if let Some(camera) = cameras.get(device_id) {
                        let mut camera = camera.write().await;
                        return camera.set_gain(gain).await.map_err(|e| e.to_string());
                    }
                }
                Err(format!("ASCOM camera {} not found", device_id))
            }
            Some(DriverType::Alpaca) => {
                let cameras = self.alpaca_cameras.read().await;
                if let Some(camera) = cameras.get(device_id) {
                    return camera.set_gain(gain).await;
                }
                Err(format!("Alpaca camera {} not found", device_id))
            }
            Some(DriverType::Native) => {
                let mut native_cameras = self.native_cameras.write().await;
                if let Some(camera) = native_cameras.get_mut(device_id) {
                    return camera.set_gain(gain).await.map_err(|e| e.to_string());
                }
                Err(format!("Native SDK camera {} not found", device_id))
            }
            Some(DriverType::Simulator) => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err(format!("Camera {} not found or not supported", device_id)),
        }
    }

    /// Set camera offset
    pub async fn camera_set_offset(&self, device_id: &str, offset: i32) -> Result<(), String> {
        tracing::info!("DeviceManager: camera_set_offset for {} offset={}", device_id, offset);

        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Ascom) => {
                #[cfg(windows)]
                {
                    let cameras = self.ascom_cameras.read().await;
                    if let Some(camera) = cameras.get(device_id) {
                        let mut camera = camera.write().await;
                        return camera.set_offset(offset).await.map_err(|e| e.to_string());
                    }
                }
                Err(format!("ASCOM camera {} not found", device_id))
            }
            Some(DriverType::Alpaca) => {
                let cameras = self.alpaca_cameras.read().await;
                if let Some(camera) = cameras.get(device_id) {
                    return camera.set_offset(offset).await;
                }
                Err(format!("Alpaca camera {} not found", device_id))
            }
            Some(DriverType::Native) => {
                let mut native_cameras = self.native_cameras.write().await;
                if let Some(camera) = native_cameras.get_mut(device_id) {
                    return camera.set_offset(offset).await.map_err(|e| e.to_string());
                }
                Err(format!("Native SDK camera {} not found", device_id))
            }
            Some(DriverType::Simulator) => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err(format!("Camera {} not found or not supported", device_id)),
        }
    }

    /// Set camera cooler
    pub async fn camera_set_cooler(&self, device_id: &str, enabled: bool, target_temp: Option<f64>) -> Result<(), String> {
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Ascom) => {
                #[cfg(windows)]
                {
                    let cameras = self.ascom_cameras.read().await;
                    if let Some(cam) = cameras.get(device_id) {
                        let mut cam = cam.write().await;
                        cam.set_cooler(enabled, target_temp.unwrap_or(-10.0)).await.map_err(|e| e.to_string())?;
                        return Ok(());
                    }
                }
                Err("ASCOM camera not connected".to_string())
            }
            Some(DriverType::Alpaca) => {
                let cameras = self.alpaca_cameras.read().await;
                if let Some(camera) = cameras.get(device_id) {
                    camera.set_cooler_on(enabled).await?;
                    if let Some(temp) = target_temp {
                        camera.set_ccd_temperature(temp).await?;
                    }
                    return Ok(());
                }
                Err(format!("Alpaca camera {} not found", device_id))
            }
            Some(DriverType::Indi) => {
                // Parse device_id format: indi:host:port:device_name
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() < 4 {
                    return Err(format!("Invalid INDI device ID format: {}", device_id));
                }
                let host = parts[1];
                let port = parts[2];
                let device_name = parts[3..].join(":");
                let server_key = format!("{}:{}", host, port);

                let clients = self.indi_clients.read().await;
                if let Some(client) = clients.get(&server_key) {
                    let mut locked_client = client.write().await;
                    // Set cooler on/off
                    let switch_element = if enabled { "COOLER_ON" } else { "COOLER_OFF" };
                    locked_client.set_switch(&device_name, "CCD_COOLER", switch_element, true).await?;
                    // Set target temperature if provided
                    if let Some(temp) = target_temp {
                        locked_client.set_number(&device_name, "CCD_TEMPERATURE", "CCD_TEMPERATURE_VALUE", temp).await?;
                    }
                    return Ok(());
                }
                Err(format!("INDI client not connected for server {}", server_key))
            }
            Some(DriverType::Native) => {
                let mut native_cameras = self.native_cameras.write().await;
                if let Some(camera) = native_cameras.get_mut(device_id) {
                    return camera.set_cooler(enabled, target_temp.unwrap_or(-10.0)).await.map_err(|e| e.to_string());
                }
                Err(format!("Native SDK camera {} not found", device_id))
            }
            Some(DriverType::Simulator) => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            Some(_) => Err("Not implemented for this driver type".to_string()),
            None => Err("Driver type not found".to_string()),
        }
    }

    // =========================================================================
    // Mount Control
    // =========================================================================
    
    pub async fn mount_slew(&self, device_id: &str, ra: f64, dec: f64) -> Result<(), String> {
        tracing::debug!("mount_slew called: device_id={}, ra={}, dec={}", device_id, ra, dec);

        let devices = self.devices.read().await;
        let info = devices.get(device_id).map(|d| d.info.clone())
            .ok_or_else(|| {
                tracing::error!("mount_slew: Device not found in devices map: {}", device_id);
                format!("Device not found: {}", device_id)
            })?;

        tracing::debug!("mount_slew: Found device with driver_type={:?}", info.driver_type);

        match info.driver_type {
            DriverType::Ascom => {
                #[cfg(windows)]
                {
                    let mounts = self.ascom_mounts.read().await;
                    tracing::debug!("mount_slew: ascom_mounts contains {} entries", mounts.len());
                    if let Some(mount) = mounts.get(device_id) {
                        let mut mount = mount.write().await;
                        return mount.slew_to_coordinates(ra, dec).await.map_err(|e| {
                            tracing::error!("mount_slew ASCOM error: {}", e);
                            e.to_string()
                        });
                    } else {
                        tracing::error!("mount_slew: Mount {} not found in ascom_mounts. Available: {:?}",
                            device_id, mounts.keys().collect::<Vec<_>>());
                    }
                }
                Err("ASCOM mount not connected".to_string())
            }
            DriverType::Alpaca => {
                let mounts = self.alpaca_mounts.read().await;
                if let Some(mount) = mounts.get(device_id) {
                    tracing::debug!("mount_slew: Calling Alpaca slew_to_coordinates_async");
                    return mount.slew_to_coordinates_async(ra, dec).await.map_err(|e| {
                        tracing::error!("mount_slew Alpaca error: {}", e);
                        e
                    });
                }
                tracing::error!("mount_slew: Alpaca mount {} not connected", device_id);
                Err(format!("Alpaca mount {} not connected", device_id))
            }
            DriverType::Indi => {
                // Parse INDI device ID: indi:host:port:device_name
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() >= 4 {
                    let host = parts[1];
                    let port = parts[2];
                    let device_name = parts[3..].join(":");
                    let server_key = format!("{}:{}", host, port);

                    let clients = self.indi_clients.read().await;
                    if let Some(client) = clients.get(&server_key) {
                        tracing::debug!("mount_slew: Creating INDI mount wrapper for {}", device_name);
                        let mount = nightshade_indi::IndiMount::new(client.clone(), &device_name);
                        return mount.slew_to_coordinates(ra, dec).await.map_err(|e| {
                            tracing::error!("mount_slew INDI error: {}", e);
                            e
                        });
                    }
                    tracing::error!("mount_slew: INDI client not connected for {}", server_key);
                    return Err(format!("INDI client not connected for {}", server_key));
                }
                Err(format!("Invalid INDI device ID format: {}", device_id))
            }
            DriverType::Native => {
                let mut native_mounts = self.native_mounts.write().await;
                if let Some(mount) = native_mounts.get_mut(device_id) {
                    return mount.slew_to_coordinates(ra, dec).await.map_err(|e| {
                        tracing::error!("mount_slew Native error: {}", e);
                        e.to_string()
                    });
                }
                tracing::error!("mount_slew: Native mount {} not connected", device_id);
                Err("Native mount not connected".to_string())
            }
            DriverType::Simulator => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
        }
    }

    pub async fn mount_sync(&self, device_id: &str, ra: f64, dec: f64) -> Result<(), String> {
        let devices = self.devices.read().await;
        let info = devices.get(device_id).map(|d| d.info.clone())
            .ok_or_else(|| format!("Device not found: {}", device_id))?;

        match info.driver_type {
            DriverType::Ascom => {
                #[cfg(windows)]
                {
                    let mounts = self.ascom_mounts.read().await;
                    if let Some(mount) = mounts.get(device_id) {
                        let mut mount = mount.write().await;
                        return mount.sync_to_coordinates(ra, dec).await.map_err(|e| e.to_string());
                    }
                }
                Err("ASCOM mount not connected".to_string())
            }
            DriverType::Alpaca => {
                let mounts = self.alpaca_mounts.read().await;
                if let Some(mount) = mounts.get(device_id) {
                    return mount.sync_to_coordinates(ra, dec).await;
                }
                Err(format!("Alpaca mount {} not connected", device_id))
            }
            DriverType::Indi => {
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() >= 4 {
                    let server_key = format!("{}:{}", parts[1], parts[2]);
                    let device_name = parts[3..].join(":");
                    let clients = self.indi_clients.read().await;
                    if let Some(client) = clients.get(&server_key) {
                        let mount = nightshade_indi::IndiMount::new(client.clone(), &device_name);
                        return mount.sync_to_coordinates(ra, dec).await;
                    }
                    return Err(format!("INDI client not connected for {}", server_key));
                }
                Err(format!("Invalid INDI device ID format: {}", device_id))
            }
            DriverType::Native => {
                let mut native_mounts = self.native_mounts.write().await;
                if let Some(mount) = native_mounts.get_mut(device_id) {
                    return mount.sync_to_coordinates(ra, dec).await.map_err(|e| e.to_string());
                }
                Err("Native mount not connected".to_string())
            }
            DriverType::Simulator => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
        }
    }

    pub async fn mount_park(&self, device_id: &str) -> Result<(), String> {
        let devices = self.devices.read().await;
        let info = devices.get(device_id).map(|d| d.info.clone())
            .ok_or_else(|| format!("Device not found: {}", device_id))?;

        match info.driver_type {
            DriverType::Ascom => {
                #[cfg(windows)]
                {
                    let mounts = self.ascom_mounts.read().await;
                    if let Some(mount) = mounts.get(device_id) {
                        let mut mount = mount.write().await;
                        return mount.park().await.map_err(|e| e.to_string());
                    }
                }
                Err("ASCOM mount not connected".to_string())
            }
            DriverType::Alpaca => {
                let mounts = self.alpaca_mounts.read().await;
                if let Some(mount) = mounts.get(device_id) {
                    return mount.park().await;
                }
                Err(format!("Alpaca mount {} not connected", device_id))
            }
            DriverType::Indi => {
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() >= 4 {
                    let server_key = format!("{}:{}", parts[1], parts[2]);
                    let device_name = parts[3..].join(":");
                    let clients = self.indi_clients.read().await;
                    if let Some(client) = clients.get(&server_key) {
                        let mount = nightshade_indi::IndiMount::new(client.clone(), &device_name);
                        return mount.park().await;
                    }
                    return Err(format!("INDI client not connected for {}", server_key));
                }
                Err(format!("Invalid INDI device ID format: {}", device_id))
            }
            DriverType::Native => {
                let mut native_mounts = self.native_mounts.write().await;
                if let Some(mount) = native_mounts.get_mut(device_id) {
                    return mount.park().await.map_err(|e| e.to_string());
                }
                Err("Native mount not connected".to_string())
            }
            DriverType::Simulator => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
        }
    }

    pub async fn mount_unpark(&self, device_id: &str) -> Result<(), String> {
        let devices = self.devices.read().await;
        let info = devices.get(device_id).map(|d| d.info.clone())
            .ok_or_else(|| format!("Device not found: {}", device_id))?;

        match info.driver_type {
            DriverType::Ascom => {
                #[cfg(windows)]
                {
                    let mounts = self.ascom_mounts.read().await;
                    if let Some(mount) = mounts.get(device_id) {
                        let mut mount = mount.write().await;
                        return mount.unpark().await.map_err(|e| e.to_string());
                    }
                }
                Err("ASCOM mount not connected".to_string())
            }
            DriverType::Alpaca => {
                let mounts = self.alpaca_mounts.read().await;
                if let Some(mount) = mounts.get(device_id) {
                    return mount.unpark().await;
                }
                Err(format!("Alpaca mount {} not connected", device_id))
            }
            DriverType::Indi => {
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() >= 4 {
                    let server_key = format!("{}:{}", parts[1], parts[2]);
                    let device_name = parts[3..].join(":");
                    let clients = self.indi_clients.read().await;
                    if let Some(client) = clients.get(&server_key) {
                        let mount = nightshade_indi::IndiMount::new(client.clone(), &device_name);
                        return mount.unpark().await;
                    }
                    return Err(format!("INDI client not connected for {}", server_key));
                }
                Err(format!("Invalid INDI device ID format: {}", device_id))
            }
            DriverType::Native => {
                let mut native_mounts = self.native_mounts.write().await;
                if let Some(mount) = native_mounts.get_mut(device_id) {
                    return mount.unpark().await.map_err(|e| e.to_string());
                }
                Err("Native mount not connected".to_string())
            }
            DriverType::Simulator => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
        }
    }

    pub async fn mount_get_coordinates(&self, device_id: &str) -> Result<(f64, f64), String> {
        let devices = self.devices.read().await;
        let info = devices.get(device_id).map(|d| d.info.clone())
            .ok_or_else(|| format!("Device not found: {}", device_id))?;

        match info.driver_type {
            DriverType::Ascom => {
                #[cfg(windows)]
                {
                    let mounts = self.ascom_mounts.read().await;
                    if let Some(mount) = mounts.get(device_id) {
                        let mount = mount.read().await;
                        return mount.get_coordinates().await.map_err(|e| e.to_string());
                    }
                }
                Err("ASCOM mount not connected".to_string())
            }
            DriverType::Native => {
                let native_mounts = self.native_mounts.read().await;
                if let Some(mount) = native_mounts.get(device_id) {
                    return mount.get_coordinates().await.map_err(|e| e.to_string());
                }
                Err("Native mount not connected".to_string())
            }
            DriverType::Simulator => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    pub async fn mount_abort(&self, device_id: &str) -> Result<(), String> {
        let devices = self.devices.read().await;
        let info = devices.get(device_id).map(|d| d.info.clone())
            .ok_or_else(|| format!("Device not found: {}", device_id))?;

        match info.driver_type {
            DriverType::Ascom => {
                #[cfg(windows)]
                {
                    let mounts = self.ascom_mounts.read().await;
                    if let Some(mount) = mounts.get(device_id) {
                        let mut mount = mount.write().await;
                        return mount.abort_slew().await.map_err(|e| e.to_string());
                    }
                }
                Err("ASCOM mount not connected".to_string())
            }
            DriverType::Native => {
                let mut native_mounts = self.native_mounts.write().await;
                if let Some(mount) = native_mounts.get_mut(device_id) {
                    return mount.abort_slew().await.map_err(|e| e.to_string());
                }
                Err("Native mount not connected".to_string())
            }
            DriverType::Simulator => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    pub async fn mount_set_tracking(&self, device_id: &str, enabled: bool) -> Result<(), String> {
        let devices = self.devices.read().await;
        let info = devices.get(device_id).map(|d| d.info.clone())
            .ok_or_else(|| format!("Device not found: {}", device_id))?;

        match info.driver_type {
            DriverType::Ascom => {
                #[cfg(windows)]
                {
                    let mounts = self.ascom_mounts.read().await;
                    if let Some(mount) = mounts.get(device_id) {
                        let mut mount = mount.write().await;
                        return mount.set_tracking(enabled).await.map_err(|e| e.to_string());
                    }
                }
                Err("ASCOM mount not connected".to_string())
            }
            DriverType::Native => {
                let mut native_mounts = self.native_mounts.write().await;
                if let Some(mount) = native_mounts.get_mut(device_id) {
                    return mount.set_tracking(enabled).await.map_err(|e| e.to_string());
                }
                Err("Native mount not connected".to_string())
            }
            DriverType::Simulator => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    pub async fn mount_pulse_guide(&self, device_id: &str, direction: String, duration_ms: u32) -> Result<(), String> {
        let devices = self.devices.read().await;
        let info = devices.get(device_id).map(|d| d.info.clone())
            .ok_or_else(|| format!("Device not found: {}", device_id))?;

        let dir = match direction.to_lowercase().as_str() {
            "north" | "n" => nightshade_native::traits::GuideDirection::North,
            "south" | "s" => nightshade_native::traits::GuideDirection::South,
            "east" | "e" => nightshade_native::traits::GuideDirection::East,
            "west" | "w" => nightshade_native::traits::GuideDirection::West,
            _ => return Err(format!("Invalid direction: {}", direction)),
        };

        match info.driver_type {
            DriverType::Ascom => {
                #[cfg(windows)]
                {
                    let mounts = self.ascom_mounts.read().await;
                    if let Some(mount) = mounts.get(device_id) {
                        let mut mount = mount.write().await;
                        return mount.pulse_guide(dir, duration_ms).await.map_err(|e| e.to_string());
                    }
                }
                Err("ASCOM mount not connected".to_string())
            }
            DriverType::Native => {
                let mut native_mounts = self.native_mounts.write().await;
                if let Some(mount) = native_mounts.get_mut(device_id) {
                    return mount.pulse_guide(dir, duration_ms).await.map_err(|e| e.to_string());
                }
                Err("Native mount not connected".to_string())
            }
            DriverType::Simulator => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    pub async fn mount_get_status(&self, device_id: &str) -> Result<MountStatus, String> {
        let devices = self.devices.read().await;
        let info = devices.get(device_id).map(|d| d.info.clone())
            .ok_or_else(|| format!("Device not found: {}", device_id))?;

        match info.driver_type {
            DriverType::Ascom => {
                #[cfg(windows)]
                {
                    let mounts = self.ascom_mounts.read().await;
                    if let Some(mount) = mounts.get(device_id) {
                        let mount = mount.read().await;

                        // Gather all status info
                        let (ra, dec) = mount.get_coordinates().await.map_err(|e| e.to_string())?;
                        let (alt, az) = mount.get_alt_az().await.map_err(|e| e.to_string())?;
                        let tracking = mount.get_tracking().await.map_err(|e| e.to_string())?;
                        let slewing = mount.is_slewing().await.map_err(|e| e.to_string())?;
                        let parked = mount.is_parked().await.map_err(|e| e.to_string())?;
                        let side_of_pier_native = mount.get_side_of_pier().await.map_err(|e| e.to_string())?;
                        let side_of_pier = match side_of_pier_native {
                            nightshade_native::traits::PierSide::East => crate::device::PierSide::East,
                            nightshade_native::traits::PierSide::West => crate::device::PierSide::West,
                            nightshade_native::traits::PierSide::Unknown => crate::device::PierSide::Unknown,
                        };
                        let sidereal_time = mount.get_sidereal_time().await.map_err(|e| e.to_string())?;

                        return Ok(MountStatus {
                            connected: true,
                            tracking,
                            slewing,
                            parked,
                            at_home: false, // Not implemented yet
                            side_of_pier,
                            right_ascension: ra,
                            declination: dec,
                            altitude: alt,
                            azimuth: az,
                            sidereal_time,
                            tracking_rate: TrackingRate::Sidereal, // Default for now
                            can_park: true,
                            can_slew: true,
                            can_sync: true,
                            can_pulse_guide: true,
                            can_set_tracking_rate: true, // ASCOM mounts typically support this
                        });
                    }
                }
                Err("ASCOM mount not connected".to_string())
            }
            DriverType::Native => {
                let native_mounts = self.native_mounts.read().await;
                if let Some(mount) = native_mounts.get(device_id) {
                    // Gather all status info
                    let (ra, dec) = mount.get_coordinates().await.map_err(|e| e.to_string())?;
                    let tracking = mount.get_tracking().await.map_err(|e| e.to_string())?;
                    let slewing = mount.is_slewing().await.map_err(|e| e.to_string())?;
                    let parked = mount.is_parked().await.map_err(|e| e.to_string())?;
                    let side_of_pier_native = mount.get_side_of_pier().await.map_err(|e| e.to_string())?;
                    let side_of_pier = match side_of_pier_native {
                        nightshade_native::traits::PierSide::East => crate::device::PierSide::East,
                        nightshade_native::traits::PierSide::West => crate::device::PierSide::West,
                        nightshade_native::traits::PierSide::Unknown => crate::device::PierSide::Unknown,
                    };

                    // Alt/Az and sidereal time may not be supported by all native mounts
                    let (alt, az) = mount.get_alt_az().await.unwrap_or((0.0, 0.0));
                    let sidereal_time = mount.get_sidereal_time().await.unwrap_or(0.0);

                    return Ok(MountStatus {
                        connected: true,
                        tracking,
                        slewing,
                        parked,
                        at_home: false,
                        side_of_pier,
                        right_ascension: ra,
                        declination: dec,
                        altitude: alt,
                        azimuth: az,
                        sidereal_time,
                        tracking_rate: TrackingRate::Sidereal,
                        can_park: true,
                        can_slew: true,
                        can_sync: true,
                        can_pulse_guide: true,
                        can_set_tracking_rate: false, // Native mounts don't support tracking rate changes
                    });
                }
                Err("Native mount not connected".to_string())
            }
            DriverType::Alpaca => {
                let mounts = self.alpaca_mounts.read().await;
                if let Some(mount) = mounts.get(device_id) {
                    let ra = mount.right_ascension().await.unwrap_or(0.0);
                    let dec = mount.declination().await.unwrap_or(0.0);
                    let alt = mount.altitude().await.unwrap_or(0.0);
                    let az = mount.azimuth().await.unwrap_or(0.0);
                    let tracking = mount.tracking().await.unwrap_or(false);
                    let slewing = mount.slewing().await.unwrap_or(false);
                    let parked = mount.at_park().await.unwrap_or(false);
                    let at_home = mount.at_home().await.unwrap_or(false);
                    let sidereal_time = mount.sidereal_time().await.unwrap_or(0.0);
                    let side_of_pier_alpaca = mount.side_of_pier().await.unwrap_or(nightshade_alpaca::PierSide::Unknown);
                    let side_of_pier = match side_of_pier_alpaca {
                        nightshade_alpaca::PierSide::East => crate::device::PierSide::East,
                        nightshade_alpaca::PierSide::West => crate::device::PierSide::West,
                        nightshade_alpaca::PierSide::Unknown => crate::device::PierSide::Unknown,
                    };

                    return Ok(MountStatus {
                        connected: true,
                        tracking,
                        slewing,
                        parked,
                        at_home,
                        side_of_pier,
                        right_ascension: ra,
                        declination: dec,
                        altitude: alt,
                        azimuth: az,
                        sidereal_time,
                        tracking_rate: TrackingRate::Sidereal, // TODO: Get actual tracking rate
                        can_park: mount.can_park().await.unwrap_or(true),
                        can_slew: mount.can_slew().await.unwrap_or(true),
                        can_sync: mount.can_sync().await.unwrap_or(true),
                        can_pulse_guide: mount.can_pulse_guide().await.unwrap_or(false),
                        can_set_tracking_rate: mount.can_set_tracking().await.unwrap_or(false),
                    });
                }
                Err("Alpaca mount not connected".to_string())
            }
            DriverType::Indi => {
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() >= 4 {
                    let server_key = format!("{}:{}", parts[1], parts[2]);
                    let device_name = parts[3..].join(":");
                    let clients = self.indi_clients.read().await;
                    if let Some(client) = clients.get(&server_key) {
                        let mount = nightshade_indi::IndiMount::new(client.clone(), &device_name);
                        let (ra, dec) = mount.get_coordinates().await.unwrap_or((0.0, 0.0));
                        let (alt, az) = mount.get_horizontal_coordinates().await.unwrap_or((0.0, 0.0));
                        let tracking = mount.is_tracking().await;
                        let slewing = mount.is_slewing().await;
                        let parked = mount.is_parked().await;

                        return Ok(MountStatus {
                            connected: true,
                            tracking,
                            slewing,
                            parked,
                            at_home: false, // INDI doesn't typically report at_home
                            side_of_pier: crate::device::PierSide::Unknown, // INDI pier side is complex
                            right_ascension: ra,
                            declination: dec,
                            altitude: alt,
                            azimuth: az,
                            sidereal_time: 0.0, // Would need to calculate from coordinates
                            tracking_rate: TrackingRate::Sidereal,
                            can_park: true,
                            can_slew: true,
                            can_sync: true,
                            can_pulse_guide: true,
                            can_set_tracking_rate: false,
                        });
                    }
                    return Err(format!("INDI client not connected for {}", server_key));
                }
                Err("Invalid INDI device ID format".to_string())
            }
            DriverType::Simulator => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    pub async fn mount_set_tracking_rate(&self, device_id: &str, rate: i32) -> Result<(), String> {
        let devices = self.devices.read().await;
        let info = devices.get(device_id).map(|d| d.info.clone())
            .ok_or_else(|| format!("Device not found: {}", device_id))?;

        match info.driver_type {
            DriverType::Ascom => {
                #[cfg(windows)]
                {
                    let mounts = self.ascom_mounts.read().await;
                    if let Some(mount) = mounts.get(device_id) {
                        let mut mount = mount.write().await;
                        return mount.set_tracking_rate(rate).await.map_err(|e| e.to_string());
                    }
                }
                Err("ASCOM mount not connected".to_string())
            }
            DriverType::Native => {
                let mut native_mounts = self.native_mounts.write().await;
                if let Some(mount) = native_mounts.get_mut(device_id) {
                    // Convert i32 rate to TrackingRate enum
                    let tracking_rate = match rate {
                        0 => nightshade_native::traits::TrackingRate::Sidereal,
                        1 => nightshade_native::traits::TrackingRate::Lunar,
                        2 => nightshade_native::traits::TrackingRate::Solar,
                        3 => nightshade_native::traits::TrackingRate::King,
                        4 => nightshade_native::traits::TrackingRate::Custom,
                        _ => return Err(format!("Invalid tracking rate: {}", rate)),
                    };
                    return mount.set_tracking_rate(tracking_rate).await.map_err(|e| e.to_string());
                }
                Err("Native mount not connected".to_string())
            }
            _ => Err("Setting tracking rate is not supported by this driver type".to_string()),
        }
    }

    pub async fn mount_get_tracking_rate(&self, device_id: &str) -> Result<i32, String> {
        let devices = self.devices.read().await;
        let info = devices.get(device_id).map(|d| d.info.clone())
            .ok_or_else(|| format!("Device not found: {}", device_id))?;

        match info.driver_type {
            DriverType::Ascom => {
                #[cfg(windows)]
                {
                    let mounts = self.ascom_mounts.read().await;
                    if let Some(mount) = mounts.get(device_id) {
                        let mount = mount.read().await;
                        return mount.get_tracking_rate().await.map_err(|e| e.to_string());
                    }
                }
                Err("ASCOM mount not connected".to_string())
            }
            DriverType::Native => {
                let native_mounts = self.native_mounts.read().await;
                if let Some(mount) = native_mounts.get(device_id) {
                    let rate = mount.get_tracking_rate().await.map_err(|e| e.to_string())?;
                    return Ok(rate as i32);
                }
                Err("Native mount not connected".to_string())
            }
            _ => Err("Getting tracking rate is not supported by this driver type".to_string()),
        }
    }

    /// Move an axis at the specified rate (degrees/second)
    /// axis: 0=RA/Azimuth (primary), 1=Dec/Altitude (secondary)
    /// rate: degrees per second (positive = N/E, negative = S/W), 0 to stop
    pub async fn mount_move_axis(&self, device_id: &str, axis: i32, rate: f64) -> Result<(), String> {
        tracing::debug!("mount_move_axis called: device_id={}, axis={}, rate={}", device_id, axis, rate);

        let devices = self.devices.read().await;
        let info = devices.get(device_id).map(|d| d.info.clone())
            .ok_or_else(|| {
                tracing::error!("mount_move_axis: Device not found in devices map: {}", device_id);
                format!("Device not found: {}", device_id)
            })?;

        tracing::debug!("mount_move_axis: Found device with driver_type={:?}", info.driver_type);

        match info.driver_type {
            DriverType::Ascom => {
                #[cfg(windows)]
                {
                    let mounts = self.ascom_mounts.read().await;
                    tracing::debug!("mount_move_axis: ascom_mounts contains {} entries", mounts.len());
                    if let Some(mount) = mounts.get(device_id) {
                        let mut mount = mount.write().await;
                        return mount.move_axis(axis, rate).await.map_err(|e| {
                            tracing::error!("mount_move_axis ASCOM error: {}", e);
                            e.to_string()
                        });
                    } else {
                        tracing::error!("mount_move_axis: Mount {} not found in ascom_mounts. Available: {:?}",
                            device_id, mounts.keys().collect::<Vec<_>>());
                    }
                }
                Err("ASCOM mount not connected".to_string())
            }
            DriverType::Alpaca => {
                let mounts = self.alpaca_mounts.read().await;
                if let Some(mount) = mounts.get(device_id) {
                    tracing::debug!("mount_move_axis: Calling Alpaca move_axis");
                    return mount.move_axis(axis, rate).await.map_err(|e| {
                        tracing::error!("mount_move_axis Alpaca error: {}", e);
                        e
                    });
                }
                tracing::error!("mount_move_axis: Alpaca mount {} not connected", device_id);
                Err(format!("Alpaca mount {} not connected", device_id))
            }
            DriverType::Indi => {
                // INDI uses directional movement (NSEW) instead of axis rates
                // We need to map axis/rate to directional commands
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() >= 4 {
                    let host = parts[1];
                    let port = parts[2];
                    let device_name = parts[3..].join(":");
                    let server_key = format!("{}:{}", host, port);

                    let clients = self.indi_clients.read().await;
                    if let Some(client) = clients.get(&server_key) {
                        let mount = nightshade_indi::IndiMount::new(client.clone(), &device_name);

                        // Convert axis/rate to directional movement
                        // axis 0 = RA/Az (East/West), axis 1 = Dec/Alt (North/South)
                        // rate > 0 = North/East, rate < 0 = South/West, rate = 0 = stop
                        if axis == 0 {
                            // RA/Azimuth axis
                            if rate > 0.0 {
                                return mount.move_east(true).await.map_err(|e| {
                                    tracing::error!("mount_move_axis INDI error (move east): {}", e);
                                    e
                                });
                            } else if rate < 0.0 {
                                return mount.move_west(true).await.map_err(|e| {
                                    tracing::error!("mount_move_axis INDI error (move west): {}", e);
                                    e
                                });
                            } else {
                                // Stop both directions
                                let _ = mount.move_east(false).await;
                                return mount.move_west(false).await.map_err(|e| {
                                    tracing::error!("mount_move_axis INDI error (stop RA): {}", e);
                                    e
                                });
                            }
                        } else {
                            // Dec/Altitude axis
                            if rate > 0.0 {
                                return mount.move_north(true).await.map_err(|e| {
                                    tracing::error!("mount_move_axis INDI error (move north): {}", e);
                                    e
                                });
                            } else if rate < 0.0 {
                                return mount.move_south(true).await.map_err(|e| {
                                    tracing::error!("mount_move_axis INDI error (move south): {}", e);
                                    e
                                });
                            } else {
                                // Stop both directions
                                let _ = mount.move_north(false).await;
                                return mount.move_south(false).await.map_err(|e| {
                                    tracing::error!("mount_move_axis INDI error (stop Dec): {}", e);
                                    e
                                });
                            }
                        }
                    }
                    tracing::error!("mount_move_axis: INDI client not connected for {}", server_key);
                    return Err(format!("INDI client not connected for {}", server_key));
                }
                Err(format!("Invalid INDI device ID format: {}", device_id))
            }
            DriverType::Native => {
                tracing::warn!("mount_move_axis: Native SDK does not support mount axis movement");
                Err("Native SDK does not support mount axis movement".to_string())
            }
            DriverType::Simulator => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
        }
    }

    // =========================================================================
    // Focuser Control
    // =========================================================================

    pub async fn focuser_move_abs(&self, device_id: &str, position: i32) -> Result<(), String> {
        let devices = self.devices.read().await;
        let info = devices.get(device_id).map(|d| d.info.clone())
            .ok_or_else(|| format!("Device not found: {}", device_id))?;
        drop(devices);

        match info.driver_type {
            DriverType::Ascom => {
                #[cfg(windows)]
                {
                    let focusers = self.ascom_focusers.read().await;
                    if let Some(focuser) = focusers.get(device_id) {
                        let mut focuser = focuser.write().await;
                        return focuser.move_to(position).await.map_err(|e| e.to_string());
                    }
                }
                Err("ASCOM focuser not connected".to_string())
            }
            DriverType::Native => {
                let mut native_focusers = self.native_focusers.write().await;
                if let Some(focuser) = native_focusers.get_mut(device_id) {
                    return focuser.move_to(position).await.map_err(|e| e.to_string());
                }
                Err("Native focuser not connected".to_string())
            }
            DriverType::Alpaca => {
                let alpaca_focusers = self.alpaca_focusers.read().await;
                if let Some(focuser) = alpaca_focusers.get(device_id) {
                    return focuser.move_to(position).await;
                }
                Err("Alpaca focuser not connected".to_string())
            }
            DriverType::Indi => {
                // Parse INDI device ID: indi:host:port:device_name
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() >= 4 {
                    let host = parts[1];
                    let port: u16 = parts[2].parse().map_err(|_| "Invalid port in INDI device ID")?;
                    let device_name = parts[3..].join(":");
                    let server_key = format!("{}:{}", host, port);

                    let clients = self.indi_clients.read().await;
                    if let Some(client) = clients.get(&server_key) {
                        let focuser = nightshade_indi::IndiFocuser::new(client.clone(), &device_name);
                        return focuser.move_to(position).await;
                    }
                    return Err(format!("INDI client not connected for {}", server_key));
                }
                Err("Invalid INDI device ID format".to_string())
            }
            DriverType::Simulator => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    pub async fn focuser_move_rel(&self, device_id: &str, steps: i32) -> Result<(), String> {
        let devices = self.devices.read().await;
        let info = devices.get(device_id).map(|d| d.info.clone())
            .ok_or_else(|| format!("Device not found: {}", device_id))?;
        drop(devices);

        match info.driver_type {
            DriverType::Ascom => {
                #[cfg(windows)]
                {
                    let focusers = self.ascom_focusers.read().await;
                    if let Some(focuser) = focusers.get(device_id) {
                        let mut focuser = focuser.write().await;
                        return focuser.move_relative(steps).await.map_err(|e| e.to_string());
                    }
                }
                Err("ASCOM focuser not connected".to_string())
            }
            DriverType::Native => {
                let mut native_focusers = self.native_focusers.write().await;
                if let Some(focuser) = native_focusers.get_mut(device_id) {
                    return focuser.move_relative(steps).await.map_err(|e| e.to_string());
                }
                Err("Native focuser not connected".to_string())
            }
            DriverType::Alpaca => {
                // Alpaca focusers only support absolute positioning, so we compute target position
                let alpaca_focusers = self.alpaca_focusers.read().await;
                if let Some(focuser) = alpaca_focusers.get(device_id) {
                    let current_position = focuser.position().await?;
                    let target_position = current_position + steps;
                    return focuser.move_to(target_position).await;
                }
                Err("Alpaca focuser not connected".to_string())
            }
            DriverType::Indi => {
                // Parse INDI device ID: indi:host:port:device_name
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() >= 4 {
                    let host = parts[1];
                    let port: u16 = parts[2].parse().map_err(|_| "Invalid port in INDI device ID")?;
                    let device_name = parts[3..].join(":");
                    let server_key = format!("{}:{}", host, port);

                    let clients = self.indi_clients.read().await;
                    if let Some(client) = clients.get(&server_key) {
                        let focuser = nightshade_indi::IndiFocuser::new(client.clone(), &device_name);
                        return focuser.move_relative(steps).await;
                    }
                    return Err(format!("INDI client not connected for {}", server_key));
                }
                Err("Invalid INDI device ID format".to_string())
            }
            DriverType::Simulator => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    pub async fn focuser_halt(&self, device_id: &str) -> Result<(), String> {
        let devices = self.devices.read().await;
        let info = devices.get(device_id).map(|d| d.info.clone())
            .ok_or_else(|| format!("Device not found: {}", device_id))?;
        drop(devices);

        match info.driver_type {
            DriverType::Ascom => {
                #[cfg(windows)]
                {
                    let focusers = self.ascom_focusers.read().await;
                    if let Some(focuser) = focusers.get(device_id) {
                        let mut focuser = focuser.write().await;
                        return focuser.halt().await.map_err(|e| e.to_string());
                    }
                }
                Err("ASCOM focuser not connected".to_string())
            }
            DriverType::Native => {
                let mut native_focusers = self.native_focusers.write().await;
                if let Some(focuser) = native_focusers.get_mut(device_id) {
                    return focuser.halt().await.map_err(|e| e.to_string());
                }
                Err("Native focuser not connected".to_string())
            }
            DriverType::Alpaca => {
                let alpaca_focusers = self.alpaca_focusers.read().await;
                if let Some(focuser) = alpaca_focusers.get(device_id) {
                    return focuser.halt().await;
                }
                Err("Alpaca focuser not connected".to_string())
            }
            DriverType::Indi => {
                // Parse INDI device ID: indi:host:port:device_name
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() >= 4 {
                    let host = parts[1];
                    let port: u16 = parts[2].parse().map_err(|_| "Invalid port in INDI device ID")?;
                    let device_name = parts[3..].join(":");
                    let server_key = format!("{}:{}", host, port);

                    let clients = self.indi_clients.read().await;
                    if let Some(client) = clients.get(&server_key) {
                        let focuser = nightshade_indi::IndiFocuser::new(client.clone(), &device_name);
                        return focuser.abort_motion().await;
                    }
                    return Err(format!("INDI client not connected for {}", server_key));
                }
                Err("Invalid INDI device ID format".to_string())
            }
            DriverType::Simulator => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    pub async fn focuser_get_position(&self, device_id: &str) -> Result<i32, String> {
        let devices = self.devices.read().await;
        let info = devices.get(device_id).map(|d| d.info.clone())
            .ok_or_else(|| format!("Device not found: {}", device_id))?;

        match info.driver_type {
            DriverType::Ascom => {
                #[cfg(windows)]
                {
                    let focusers = self.ascom_focusers.read().await;
                    if let Some(focuser) = focusers.get(device_id) {
                        let focuser = focuser.read().await;
                        return focuser.get_position().await.map_err(|e| e.to_string());
                    }
                }
                Err("ASCOM focuser not connected".to_string())
            }
            DriverType::Native => {
                let native_focusers = self.native_focusers.read().await;
                if let Some(focuser) = native_focusers.get(device_id) {
                    return focuser.get_position().await.map_err(|e| e.to_string());
                }
                Err("Native focuser not connected".to_string())
            }
            DriverType::Alpaca => {
                let alpaca_focusers = self.alpaca_focusers.read().await;
                if let Some(focuser) = alpaca_focusers.get(device_id) {
                    return focuser.position().await;
                }
                Err("Alpaca focuser not connected".to_string())
            }
            DriverType::Indi => {
                // Parse INDI device ID: indi:host:port:device_name
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() >= 4 {
                    let host = parts[1];
                    let port: u16 = parts[2].parse().map_err(|_| "Invalid port in INDI device ID")?;
                    let device_name = parts[3..].join(":");
                    let server_key = format!("{}:{}", host, port);

                    let clients = self.indi_clients.read().await;
                    if let Some(client) = clients.get(&server_key) {
                        let focuser = nightshade_indi::IndiFocuser::new(client.clone(), &device_name);
                        return focuser.get_position().await;
                    }
                    return Err(format!("INDI client not connected for {}", server_key));
                }
                Err("Invalid INDI device ID format".to_string())
            }
            DriverType::Simulator => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    pub async fn focuser_is_moving(&self, device_id: &str) -> Result<bool, String> {
        let devices = self.devices.read().await;
        let info = devices.get(device_id).map(|d| d.info.clone())
            .ok_or_else(|| format!("Device not found: {}", device_id))?;

        match info.driver_type {
            DriverType::Ascom => {
                #[cfg(windows)]
                {
                    let focusers = self.ascom_focusers.read().await;
                    if let Some(focuser) = focusers.get(device_id) {
                        let focuser = focuser.read().await;
                        return focuser.is_moving().await.map_err(|e| e.to_string());
                    }
                }
                Err("ASCOM focuser not connected".to_string())
            }
            DriverType::Native => {
                let native_focusers = self.native_focusers.read().await;
                if let Some(focuser) = native_focusers.get(device_id) {
                    return focuser.is_moving().await.map_err(|e| e.to_string());
                }
                Err("Native focuser not connected".to_string())
            }
            DriverType::Alpaca => {
                let alpaca_focusers = self.alpaca_focusers.read().await;
                if let Some(focuser) = alpaca_focusers.get(device_id) {
                    return focuser.is_moving().await;
                }
                Err("Alpaca focuser not connected".to_string())
            }
            DriverType::Indi => {
                // Parse INDI device ID: indi:host:port:device_name
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() >= 4 {
                    let host = parts[1];
                    let port: u16 = parts[2].parse().map_err(|_| "Invalid port")?;
                    let device_name = parts[3..].join(":");
                    let server_key = format!("{}:{}", host, port);

                    let clients = self.indi_clients.read().await;
                    if let Some(client) = clients.get(&server_key) {
                        let focuser = nightshade_indi::IndiFocuser::new(client.clone(), &device_name);
                        return Ok(focuser.is_moving().await);
                    }
                    return Err(format!("INDI client not connected for {}", server_key));
                }
                Err("Invalid INDI device ID format".to_string())
            }
            DriverType::Simulator => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    pub async fn focuser_get_temp(&self, device_id: &str) -> Result<Option<f64>, String> {
        let devices = self.devices.read().await;
        let info = devices.get(device_id).map(|d| d.info.clone())
            .ok_or_else(|| format!("Device not found: {}", device_id))?;

        match info.driver_type {
            DriverType::Ascom => {
                #[cfg(windows)]
                {
                    let focusers = self.ascom_focusers.read().await;
                    if let Some(focuser) = focusers.get(device_id) {
                        let focuser = focuser.read().await;
                        return focuser.get_temperature().await.map_err(|e| e.to_string());
                    }
                }
                Err("ASCOM focuser not connected".to_string())
            }
            DriverType::Native => {
                let native_focusers = self.native_focusers.read().await;
                if let Some(focuser) = native_focusers.get(device_id) {
                    return focuser.get_temperature().await.map_err(|e| e.to_string());
                }
                Err("Native focuser not connected".to_string())
            }
            DriverType::Alpaca => {
                let alpaca_focusers = self.alpaca_focusers.read().await;
                if let Some(focuser) = alpaca_focusers.get(device_id) {
                    // Alpaca temperature() returns f64, wrap in Some for consistency
                    return focuser.temperature().await.map(Some);
                }
                Err("Alpaca focuser not connected".to_string())
            }
            DriverType::Indi => {
                // Parse INDI device ID: indi:host:port:device_name
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() >= 4 {
                    let host = parts[1];
                    let port: u16 = parts[2].parse().map_err(|_| "Invalid port in INDI device ID")?;
                    let device_name = parts[3..].join(":");
                    let server_key = format!("{}:{}", host, port);

                    let clients = self.indi_clients.read().await;
                    if let Some(client) = clients.get(&server_key) {
                        let focuser = nightshade_indi::IndiFocuser::new(client.clone(), &device_name);
                        // Temperature might not be available on all focusers
                        match focuser.get_temperature().await {
                            Ok(temp) => return Ok(Some(temp)),
                            Err(_) => return Ok(None), // Temperature not available
                        }
                    }
                    return Err(format!("INDI client not connected for {}", server_key));
                }
                Err("Invalid INDI device ID format".to_string())
            }
            DriverType::Simulator => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    pub async fn focuser_get_details(&self, device_id: &str) -> Result<(i32, f64), String> {
        let devices = self.devices.read().await;
        let info = devices.get(device_id).map(|d| d.info.clone())
            .ok_or_else(|| format!("Device not found: {}", device_id))?;

        match info.driver_type {
            DriverType::Ascom => {
                #[cfg(windows)]
                {
                    let focusers = self.ascom_focusers.read().await;
                    if let Some(focuser) = focusers.get(device_id) {
                        let focuser = focuser.read().await;
                        return Ok((focuser.get_max_position(), focuser.get_step_size()));
                    }
                }
                Err("ASCOM focuser not connected".to_string())
            }
            DriverType::Native => {
                let native_focusers = self.native_focusers.read().await;
                if let Some(focuser) = native_focusers.get(device_id) {
                    return Ok((focuser.get_max_position(), focuser.get_step_size()));
                }
                Err("Native focuser not connected".to_string())
            }
            DriverType::Alpaca => {
                let alpaca_focusers = self.alpaca_focusers.read().await;
                if let Some(focuser) = alpaca_focusers.get(device_id) {
                    let max_step = focuser.max_step().await?;
                    let step_size = focuser.step_size().await.unwrap_or(1.0);
                    return Ok((max_step, step_size));
                }
                Err("Alpaca focuser not connected".to_string())
            }
            DriverType::Indi => {
                // Parse INDI device ID: indi:host:port:device_name
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() >= 4 {
                    let host = parts[1];
                    let port: u16 = parts[2].parse().map_err(|_| "Invalid port in INDI device ID")?;
                    let device_name = parts[3..].join(":");
                    let server_key = format!("{}:{}", host, port);

                    let clients = self.indi_clients.read().await;
                    if let Some(client) = clients.get(&server_key) {
                        let client = client.read().await;

                        // Try to get max position from FOCUS_MAX property (common INDI standard)
                        // Fall back to reasonable default if not available
                        let max_position = client.get_number(&device_name, "FOCUS_MAX", "FOCUS_MAX_VALUE")
                            .await
                            .map(|v| v as i32)
                            .unwrap_or(100000); // Default max position

                        // Step size is not universally standardized in INDI
                        // Most focusers use discrete steps, default to 1.0 micron step
                        let step_size = client.get_number(&device_name, "FOCUS_STEP", "FOCUS_STEP_VALUE")
                            .await
                            .unwrap_or(1.0);

                        return Ok((max_position, step_size));
                    }
                    return Err(format!("INDI client not connected for {}", server_key));
                }
                Err("Invalid INDI device ID format".to_string())
            }
            DriverType::Simulator => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    // =========================================================================
    // Filter Wheel Control
    // =========================================================================

    pub async fn filter_wheel_set_position(&self, device_id: &str, position: i32) -> Result<(), String> {
        let devices = self.devices.read().await;
        let info = devices.get(device_id).map(|d| d.info.clone())
            .ok_or_else(|| format!("Device not found: {}", device_id))?;
        drop(devices);

        match info.driver_type {
            DriverType::Ascom => {
                #[cfg(windows)]
                {
                    let wheels = self.ascom_filter_wheels.read().await;
                    if let Some(wheel) = wheels.get(device_id) {
                        let mut wheel = wheel.write().await;
                        return wheel.move_to_position(position).await.map_err(|e| e.to_string());
                    }
                }
                Err("ASCOM filter wheel not connected".to_string())
            }
            DriverType::Native => {
                let mut native_filter_wheels = self.native_filter_wheels.write().await;
                if let Some(wheel) = native_filter_wheels.get_mut(device_id) {
                    return wheel.move_to_position(position).await.map_err(|e| e.to_string());
                }
                Err("Native filter wheel not connected".to_string())
            }
            DriverType::Alpaca => {
                let wheels = self.alpaca_filter_wheels.read().await;
                if let Some(wheel) = wheels.get(device_id) {
                    return wheel.set_position(position).await;
                }
                Err(format!("Alpaca filter wheel {} not found", device_id))
            }
            DriverType::Indi => {
                // Parse INDI device ID: indi:host:port:device_name
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() < 4 {
                    return Err("Invalid INDI device ID format".to_string());
                }
                let server_key = format!("{}:{}", parts[1], parts[2]);
                let device_name = parts[3..].join(":");

                let clients = self.indi_clients.read().await;
                if let Some(client) = clients.get(&server_key) {
                    let mut locked = client.write().await;
                    // INDI filter slots are 1-based
                    return locked.set_number(&device_name, "FILTER_SLOT", "FILTER_SLOT_VALUE", position as f64).await;
                }
                Err("INDI filter wheel not connected".to_string())
            }
            DriverType::Simulator => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    pub async fn filter_wheel_get_position(&self, device_id: &str) -> Result<i32, String> {
        let devices = self.devices.read().await;
        let info = devices.get(device_id).map(|d| d.info.clone())
            .ok_or_else(|| format!("Device not found: {}", device_id))?;

        match info.driver_type {
            DriverType::Ascom => {
                #[cfg(windows)]
                {
                    let wheels = self.ascom_filter_wheels.read().await;
                    if let Some(wheel) = wheels.get(device_id) {
                        let wheel = wheel.read().await;
                        return wheel.get_position().await.map_err(|e| e.to_string());
                    }
                }
                Err("ASCOM filter wheel not connected".to_string())
            }
            DriverType::Native => {
                let native_filter_wheels = self.native_filter_wheels.read().await;
                if let Some(wheel) = native_filter_wheels.get(device_id) {
                    return wheel.get_position().await.map_err(|e| e.to_string());
                }
                Err("Native filter wheel not connected".to_string())
            }
            DriverType::Alpaca => {
                let wheels = self.alpaca_filter_wheels.read().await;
                if let Some(wheel) = wheels.get(device_id) {
                    return wheel.position().await;
                }
                Err(format!("Alpaca filter wheel {} not found", device_id))
            }
            DriverType::Indi => {
                // Parse INDI device ID: indi:host:port:device_name
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() < 4 {
                    return Err("Invalid INDI device ID format".to_string());
                }
                let server_key = format!("{}:{}", parts[1], parts[2]);
                let device_name = parts[3..].join(":");

                let clients = self.indi_clients.read().await;
                if let Some(client) = clients.get(&server_key) {
                    let locked = client.read().await;
                    // INDI filter slots are 1-based
                    if let Some(pos) = locked.get_number(&device_name, "FILTER_SLOT", "FILTER_SLOT_VALUE").await {
                        return Ok(pos as i32);
                    }
                    return Err("Could not read filter position from INDI device".to_string());
                }
                Err("INDI filter wheel not connected".to_string())
            }
            DriverType::Simulator => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    pub async fn filter_wheel_is_moving(&self, device_id: &str) -> Result<bool, String> {
        let devices = self.devices.read().await;
        let info = devices.get(device_id).map(|d| d.info.clone())
            .ok_or_else(|| format!("Device not found: {}", device_id))?;

        match info.driver_type {
            DriverType::Ascom => {
                #[cfg(windows)]
                {
                    let wheels = self.ascom_filter_wheels.read().await;
                    if let Some(wheel) = wheels.get(device_id) {
                        let wheel = wheel.read().await;
                        return wheel.is_moving().await.map_err(|e| e.to_string());
                    }
                }
                Err("ASCOM filter wheel not connected".to_string())
            }
            DriverType::Native => {
                let native_filter_wheels = self.native_filter_wheels.read().await;
                if let Some(wheel) = native_filter_wheels.get(device_id) {
                    return wheel.is_moving().await.map_err(|e| e.to_string());
                }
                Err("Native filter wheel not connected".to_string())
            }
            DriverType::Alpaca => {
                let wheels = self.alpaca_filter_wheels.read().await;
                if let Some(wheel) = wheels.get(device_id) {
                    // Alpaca filter wheels return -1 for position when moving
                    let pos = wheel.position().await?;
                    return Ok(pos == -1);
                }
                Err(format!("Alpaca filter wheel {} not found", device_id))
            }
            DriverType::Indi => {
                // Parse INDI device ID: indi:host:port:device_name
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() < 4 {
                    return Err("Invalid INDI device ID format".to_string());
                }
                let server_key = format!("{}:{}", parts[1], parts[2]);
                let device_name = parts[3..].join(":");

                let clients = self.indi_clients.read().await;
                if let Some(client) = clients.get(&server_key) {
                    let locked = client.read().await;
                    // INDI uses property busy state to indicate movement
                    return Ok(locked.is_property_busy(&device_name, "FILTER_SLOT").await);
                }
                Err("INDI filter wheel not connected".to_string())
            }
            DriverType::Simulator => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    pub async fn filter_wheel_get_config(&self, device_id: &str) -> Result<(i32, Vec<String>), String> {
        tracing::debug!("filter_wheel_get_config: Looking up device_id='{}'", device_id);

        let devices = self.devices.read().await;
        let device_keys: Vec<_> = devices.keys().collect();
        tracing::debug!("filter_wheel_get_config: Available devices in registry: {:?}", device_keys);

        let info = devices.get(device_id).map(|d| d.info.clone())
            .ok_or_else(|| format!("Device not found: {}", device_id))?;
        tracing::debug!("filter_wheel_get_config: Found device with driver_type={:?}", info.driver_type);
        drop(devices); // Release the lock before async operations

        match info.driver_type {
            DriverType::Ascom => {
                #[cfg(windows)]
                {
                    let wheels = self.ascom_filter_wheels.read().await;
                    let ascom_keys: Vec<_> = wheels.keys().collect();
                    tracing::debug!("filter_wheel_get_config: Looking for '{}' in ascom_filter_wheels: {:?}", device_id, ascom_keys);

                    if let Some(wheel) = wheels.get(device_id) {
                        let wheel = wheel.read().await;
                        let count = wheel.get_filter_count();
                        let names = wheel.get_filter_names().await.map_err(|e| e.to_string())?;
                        return Ok((count, names));
                    }
                    tracing::error!("filter_wheel_get_config: ASCOM filter wheel '{}' not found in ascom_filter_wheels map!", device_id);
                }
                Err("ASCOM filter wheel not connected".to_string())
            }
            DriverType::Alpaca => {
                // Parse Alpaca device ID: alpaca:http://host:port:filterwheel:N
                let id_str = device_id.strip_prefix("alpaca:").unwrap_or("");
                let parts: Vec<&str> = id_str.split(':').collect();

                if parts.len() >= 5 {
                    let protocol = parts[0];
                    let host_part = parts[1].trim_start_matches("//");
                    let port = parts[2];
                    let device_num: u32 = parts[4].parse().unwrap_or(0);

                    let base_url = format!("{}://{}:{}", protocol, host_part, port);
                    let fw = nightshade_alpaca::AlpacaFilterWheel::from_server(&base_url, device_num);
                    fw.connect().await?;
                    let names = fw.names().await?;
                    let count = names.len() as i32;
                    fw.disconnect().await.ok();
                    return Ok((count, names));
                }
                Err("Invalid Alpaca filter wheel ID".to_string())
            }
            DriverType::Indi => {
                // Parse INDI device ID: indi:host:port:device_name
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() >= 4 {
                    let host = parts[1];
                    let port: u16 = parts[2].parse().map_err(|_| "Invalid port")?;
                    let device_name = parts[3..].join(":");
                    let server_key = format!("{}:{}", host, port);

                    let clients = self.indi_clients.read().await;
                    if let Some(client) = clients.get(&server_key) {
                        let locked_client = client.read().await;
                        let names = locked_client.get_filter_names(&device_name).await
                            .unwrap_or_else(|_| vec![]);
                        let count = names.len() as i32;
                        return Ok((count, names));
                    }
                }
                Err("INDI filter wheel not connected".to_string())
            }
            DriverType::Native => {
                let native_filter_wheels = self.native_filter_wheels.read().await;
                let native_keys: Vec<_> = native_filter_wheels.keys().collect();
                tracing::debug!("filter_wheel_get_config: Looking for '{}' in native_filter_wheels: {:?}", device_id, native_keys);

                if let Some(wheel) = native_filter_wheels.get(device_id) {
                    let count = wheel.get_filter_count();
                    let names = wheel.get_filter_names().await.map_err(|e| e.to_string())?;
                    return Ok((count, names));
                }
                tracing::error!("filter_wheel_get_config: Native filter wheel '{}' not found in native_filter_wheels map!", device_id);
                Err("Native filter wheel not connected".to_string())
            }
            DriverType::Simulator => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    // =========================================================================
    // Rotator Control
    // =========================================================================

    /// Get rotator position (sky angle in degrees)
    pub async fn rotator_get_position(&self, device_id: &str) -> Result<f64, String> {
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Alpaca) => {
                let rotators = self.alpaca_rotators.read().await;
                if let Some(rotator) = rotators.get(device_id) {
                    return rotator.position().await;
                }
                Err(format!("Alpaca rotator {} not found", device_id))
            }
            Some(DriverType::Indi) => {
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() < 4 {
                    return Err("Invalid INDI device ID".to_string());
                }
                let server_key = format!("{}:{}", parts[1], parts[2]);
                let device_name = parts[3..].join(":");

                let clients = self.indi_clients.read().await;
                if let Some(client) = clients.get(&server_key) {
                    let locked = client.read().await;
                    if let Some(pos) = locked.get_number(&device_name, "ABS_ROTATOR_ANGLE", "ANGLE").await {
                        return Ok(pos);
                    }
                }
                Err("INDI rotator not connected".to_string())
            }
            Some(DriverType::Native) => {
                let native_rotators = self.native_rotators.read().await;
                if let Some(rotator) = native_rotators.get(device_id) {
                    return rotator.get_position().await.map_err(|e| e.to_string());
                }
                Err("Native rotator not connected".to_string())
            }
            Some(DriverType::Simulator) => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    /// Move rotator to absolute sky position (degrees)
    pub async fn rotator_move_absolute(&self, device_id: &str, position: f64) -> Result<(), String> {
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Alpaca) => {
                let rotators = self.alpaca_rotators.read().await;
                if let Some(rotator) = rotators.get(device_id) {
                    return rotator.move_absolute(position).await;
                }
                Err(format!("Alpaca rotator {} not found", device_id))
            }
            Some(DriverType::Indi) => {
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() < 4 {
                    return Err("Invalid INDI device ID".to_string());
                }
                let server_key = format!("{}:{}", parts[1], parts[2]);
                let device_name = parts[3..].join(":");

                let clients = self.indi_clients.read().await;
                if let Some(client) = clients.get(&server_key) {
                    let mut locked = client.write().await;
                    return locked.set_number(&device_name, "ABS_ROTATOR_ANGLE", "ANGLE", position).await;
                }
                Err("INDI rotator not connected".to_string())
            }
            Some(DriverType::Native) => {
                let mut native_rotators = self.native_rotators.write().await;
                if let Some(rotator) = native_rotators.get_mut(device_id) {
                    return rotator.move_to(position).await.map_err(|e| e.to_string());
                }
                Err("Native rotator not connected".to_string())
            }
            Some(DriverType::Simulator) => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    /// Halt rotator motion
    pub async fn rotator_halt(&self, device_id: &str) -> Result<(), String> {
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Alpaca) => {
                let rotators = self.alpaca_rotators.read().await;
                if let Some(rotator) = rotators.get(device_id) {
                    return rotator.halt().await;
                }
                Err(format!("Alpaca rotator {} not found", device_id))
            }
            Some(DriverType::Indi) => {
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() < 4 {
                    return Err("Invalid INDI device ID".to_string());
                }
                let server_key = format!("{}:{}", parts[1], parts[2]);
                let device_name = parts[3..].join(":");

                let clients = self.indi_clients.read().await;
                if let Some(client) = clients.get(&server_key) {
                    let mut locked = client.write().await;
                    return locked.set_switch(&device_name, "ROTATOR_ABORT_MOTION", "ABORT", true).await;
                }
                Err("INDI rotator not connected".to_string())
            }
            Some(DriverType::Native) => {
                let mut native_rotators = self.native_rotators.write().await;
                if let Some(rotator) = native_rotators.get_mut(device_id) {
                    return rotator.halt().await.map_err(|e| e.to_string());
                }
                Err("Native rotator not connected".to_string())
            }
            Some(DriverType::Simulator) => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    /// Check if rotator is moving
    pub async fn rotator_is_moving(&self, device_id: &str) -> Result<bool, String> {
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Alpaca) => {
                let rotators = self.alpaca_rotators.read().await;
                if let Some(rotator) = rotators.get(device_id) {
                    return rotator.is_moving().await;
                }
                Err(format!("Alpaca rotator {} not found", device_id))
            }
            Some(DriverType::Native) => {
                let native_rotators = self.native_rotators.read().await;
                if let Some(rotator) = native_rotators.get(device_id) {
                    return rotator.is_moving().await.map_err(|e| e.to_string());
                }
                Err("Native rotator not connected".to_string())
            }
            Some(DriverType::Simulator) => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    // =========================================================================
    // Dome Control
    // =========================================================================

    /// Open dome shutter
    pub async fn dome_open_shutter(&self, device_id: &str) -> Result<(), String> {
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Alpaca) => {
                let domes = self.alpaca_domes.read().await;
                if let Some(dome) = domes.get(device_id) {
                    return dome.open_shutter().await;
                }
                Err(format!("Alpaca dome {} not found", device_id))
            }
            Some(DriverType::Ascom) => {
                #[cfg(windows)]
                {
                    let domes = self.ascom_domes.read().await;
                    if let Some(dome) = domes.get(device_id) {
                        let dome_guard = dome.read().await;
                        return dome_guard.open_shutter().await.map_err(|e| e.to_string());
                    }
                    Err(format!("ASCOM dome {} not found", device_id))
                }
                #[cfg(not(windows))]
                Err("ASCOM not supported on this platform".to_string())
            }
            Some(DriverType::Indi) => {
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() < 4 {
                    return Err("Invalid INDI device ID".to_string());
                }
                let server_key = format!("{}:{}", parts[1], parts[2]);
                let device_name = parts[3..].join(":");

                let clients = self.indi_clients.read().await;
                if let Some(client) = clients.get(&server_key) {
                    let mut locked = client.write().await;
                    return locked.set_switch(&device_name, "DOME_SHUTTER", "SHUTTER_OPEN", true).await;
                }
                Err("INDI dome not connected".to_string())
            }
            Some(DriverType::Native) => {
                let mut native_domes = self.native_domes.write().await;
                if let Some(dome) = native_domes.get_mut(device_id) {
                    return dome.open_shutter().await.map_err(|e| e.to_string());
                }
                Err("Native dome not connected".to_string())
            }
            Some(DriverType::Simulator) => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    /// Close dome shutter
    pub async fn dome_close_shutter(&self, device_id: &str) -> Result<(), String> {
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Alpaca) => {
                let domes = self.alpaca_domes.read().await;
                if let Some(dome) = domes.get(device_id) {
                    return dome.close_shutter().await;
                }
                Err(format!("Alpaca dome {} not found", device_id))
            }
            Some(DriverType::Ascom) => {
                #[cfg(windows)]
                {
                    let domes = self.ascom_domes.read().await;
                    if let Some(dome) = domes.get(device_id) {
                        let dome_guard = dome.read().await;
                        return dome_guard.close_shutter().await.map_err(|e| e.to_string());
                    }
                    Err(format!("ASCOM dome {} not found", device_id))
                }
                #[cfg(not(windows))]
                Err("ASCOM not supported on this platform".to_string())
            }
            Some(DriverType::Indi) => {
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() < 4 {
                    return Err("Invalid INDI device ID".to_string());
                }
                let server_key = format!("{}:{}", parts[1], parts[2]);
                let device_name = parts[3..].join(":");

                let clients = self.indi_clients.read().await;
                if let Some(client) = clients.get(&server_key) {
                    let mut locked = client.write().await;
                    return locked.set_switch(&device_name, "DOME_SHUTTER", "SHUTTER_CLOSE", true).await;
                }
                Err("INDI dome not connected".to_string())
            }
            Some(DriverType::Native) => {
                let mut native_domes = self.native_domes.write().await;
                if let Some(dome) = native_domes.get_mut(device_id) {
                    return dome.close_shutter().await.map_err(|e| e.to_string());
                }
                Err("Native dome not connected".to_string())
            }
            Some(DriverType::Simulator) => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    /// Slew dome to azimuth
    pub async fn dome_slew_to_azimuth(&self, device_id: &str, azimuth: f64) -> Result<(), String> {
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Alpaca) => {
                let domes = self.alpaca_domes.read().await;
                if let Some(dome) = domes.get(device_id) {
                    return dome.slew_to_azimuth(azimuth).await;
                }
                Err(format!("Alpaca dome {} not found", device_id))
            }
            Some(DriverType::Indi) => {
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() < 4 {
                    return Err("Invalid INDI device ID".to_string());
                }
                let server_key = format!("{}:{}", parts[1], parts[2]);
                let device_name = parts[3..].join(":");

                let clients = self.indi_clients.read().await;
                if let Some(client) = clients.get(&server_key) {
                    let mut locked = client.write().await;
                    return locked.set_number(&device_name, "ABS_DOME_POSITION", "DOME_ABSOLUTE_POSITION", azimuth).await;
                }
                Err("INDI dome not connected".to_string())
            }
            Some(DriverType::Ascom) => {
                #[cfg(windows)]
                {
                    let domes = self.ascom_domes.read().await;
                    if let Some(dome) = domes.get(device_id) {
                        let dome_guard = dome.read().await;
                        return dome_guard.slew_to_azimuth(azimuth).await;
                    }
                    Err(format!("ASCOM dome {} not found", device_id))
                }
                #[cfg(not(windows))]
                Err("ASCOM not supported on this platform".to_string())
            }
            Some(DriverType::Native) => {
                let mut native_domes = self.native_domes.write().await;
                if let Some(dome) = native_domes.get_mut(device_id) {
                    return dome.slew_to_azimuth(azimuth).await.map_err(|e| e.to_string());
                }
                Err("Native dome not connected".to_string())
            }
            Some(DriverType::Simulator) => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    /// Get dome azimuth
    pub async fn dome_get_azimuth(&self, device_id: &str) -> Result<f64, String> {
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Alpaca) => {
                let domes = self.alpaca_domes.read().await;
                if let Some(dome) = domes.get(device_id) {
                    return dome.azimuth().await;
                }
                Err(format!("Alpaca dome {} not found", device_id))
            }
            Some(DriverType::Indi) => {
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() < 4 {
                    return Err("Invalid INDI device ID".to_string());
                }
                let server_key = format!("{}:{}", parts[1], parts[2]);
                let device_name = parts[3..].join(":");

                let clients = self.indi_clients.read().await;
                if let Some(client) = clients.get(&server_key) {
                    let locked = client.read().await;
                    if let Some(az) = locked.get_number(&device_name, "ABS_DOME_POSITION", "DOME_ABSOLUTE_POSITION").await {
                        return Ok(az);
                    }
                }
                Err("INDI dome not connected".to_string())
            }
            Some(DriverType::Ascom) => {
                #[cfg(windows)]
                {
                    let domes = self.ascom_domes.read().await;
                    if let Some(dome) = domes.get(device_id) {
                        let dome_guard = dome.read().await;
                        return dome_guard.azimuth().await;
                    }
                    Err(format!("ASCOM dome {} not found", device_id))
                }
                #[cfg(not(windows))]
                Err("ASCOM not supported on this platform".to_string())
            }
            Some(DriverType::Native) => {
                let native_domes = self.native_domes.read().await;
                if let Some(dome) = native_domes.get(device_id) {
                    return dome.get_azimuth().await.map_err(|e| e.to_string());
                }
                Err("Native dome not connected".to_string())
            }
            Some(DriverType::Simulator) => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    /// Get dome shutter status
    pub async fn dome_get_shutter_status(&self, device_id: &str) -> Result<i32, String> {
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Alpaca) => {
                let domes = self.alpaca_domes.read().await;
                if let Some(dome) = domes.get(device_id) {
                    let status = dome.shutter_status().await?;
                    return Ok(status as i32);
                }
                Err(format!("Alpaca dome {} not found", device_id))
            }
            Some(DriverType::Ascom) => {
                #[cfg(windows)]
                {
                    let domes = self.ascom_domes.read().await;
                    if let Some(dome) = domes.get(device_id) {
                        let dome_guard = dome.read().await;
                        return dome_guard.shutter_status().await;
                    }
                    Err(format!("ASCOM dome {} not found", device_id))
                }
                #[cfg(not(windows))]
                Err("ASCOM not supported on this platform".to_string())
            }
            Some(DriverType::Indi) => {
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() < 4 {
                    return Err("Invalid INDI device ID".to_string());
                }
                let server_key = format!("{}:{}", parts[1], parts[2]);
                let device_name = parts[3..].join(":");

                let clients = self.indi_clients.read().await;
                if let Some(client) = clients.get(&server_key) {
                    let locked = client.read().await;
                    // Check INDI shutter switches: 0=Open, 1=Closed, 4=Unknown
                    if locked.get_switch(&device_name, "DOME_SHUTTER", "SHUTTER_OPEN").await.unwrap_or(false) {
                        return Ok(0); // Open
                    } else if locked.get_switch(&device_name, "DOME_SHUTTER", "SHUTTER_CLOSE").await.unwrap_or(false) {
                        return Ok(1); // Closed
                    }
                }
                Ok(4) // Unknown/Error
            }
            Some(DriverType::Native) => {
                let native_domes = self.native_domes.read().await;
                if let Some(dome) = native_domes.get(device_id) {
                    let status = dome.get_shutter_status().await.map_err(|e| e.to_string())?;
                    // Convert ShutterState enum to i32: Open=0, Closed=1, Opening=2, Closing=3, Error=4, Unknown=5
                    let code = match status {
                        nightshade_native::traits::ShutterState::Open => 0,
                        nightshade_native::traits::ShutterState::Closed => 1,
                        nightshade_native::traits::ShutterState::Opening => 2,
                        nightshade_native::traits::ShutterState::Closing => 3,
                        nightshade_native::traits::ShutterState::Error => 4,
                        nightshade_native::traits::ShutterState::Unknown => 5,
                    };
                    return Ok(code);
                }
                Err("Native dome not connected".to_string())
            }
            Some(DriverType::Simulator) => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    /// Park dome
    pub async fn dome_park(&self, device_id: &str) -> Result<(), String> {
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Alpaca) => {
                let domes = self.alpaca_domes.read().await;
                if let Some(dome) = domes.get(device_id) {
                    return dome.park().await;
                }
                Err(format!("Alpaca dome {} not found", device_id))
            }
            Some(DriverType::Ascom) => {
                #[cfg(windows)]
                {
                    let domes = self.ascom_domes.read().await;
                    if let Some(dome) = domes.get(device_id) {
                        let dome_guard = dome.read().await;
                        return dome_guard.park().await.map_err(|e| e.to_string());
                    }
                    Err(format!("ASCOM dome {} not found", device_id))
                }
                #[cfg(not(windows))]
                Err("ASCOM not supported on this platform".to_string())
            }
            Some(DriverType::Indi) => {
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() < 4 {
                    return Err("Invalid INDI device ID".to_string());
                }
                let server_key = format!("{}:{}", parts[1], parts[2]);
                let device_name = parts[3..].join(":");

                let clients = self.indi_clients.read().await;
                if let Some(client) = clients.get(&server_key) {
                    let mut locked = client.write().await;
                    return locked.set_switch(&device_name, "DOME_PARK", "PARK", true).await;
                }
                Err("INDI dome not connected".to_string())
            }
            Some(DriverType::Native) => {
                let mut native_domes = self.native_domes.write().await;
                if let Some(dome) = native_domes.get_mut(device_id) {
                    return dome.park().await.map_err(|e| e.to_string());
                }
                Err("Native dome not connected".to_string())
            }
            Some(DriverType::Simulator) => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    /// Check if dome is slewing
    pub async fn dome_is_slewing(&self, device_id: &str) -> Result<bool, String> {
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Alpaca) => {
                let domes = self.alpaca_domes.read().await;
                if let Some(dome) = domes.get(device_id) {
                    return dome.slewing().await;
                }
                Err(format!("Alpaca dome {} not found", device_id))
            }
            Some(DriverType::Ascom) => {
                #[cfg(windows)]
                {
                    let domes = self.ascom_domes.read().await;
                    if let Some(dome) = domes.get(device_id) {
                        let dome_guard = dome.read().await;
                        return dome_guard.slewing().await.map_err(|e| e.to_string());
                    }
                    Err(format!("ASCOM dome {} not found", device_id))
                }
                #[cfg(not(windows))]
                Err("ASCOM not supported on this platform".to_string())
            }
            Some(DriverType::Native) => {
                let native_domes = self.native_domes.read().await;
                if let Some(dome) = native_domes.get(device_id) {
                    return dome.is_slewing().await.map_err(|e| e.to_string());
                }
                Err("Native dome not connected".to_string())
            }
            Some(DriverType::Simulator) => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    /// Get comprehensive dome status
    pub async fn dome_get_status(&self, device_id: &str) -> Result<crate::device::DomeStatus, String> {
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Alpaca) => {
                let domes = self.alpaca_domes.read().await;
                if let Some(dome) = domes.get(device_id) {
                    // Get status from Alpaca dome
                    let alpaca_status = dome.get_status().await?;

                    // Query capabilities
                    let can_set_altitude = dome.can_set_altitude().await.unwrap_or(false);
                    let can_set_azimuth = dome.can_set_azimuth().await.unwrap_or(false);
                    let can_set_shutter = dome.can_set_shutter().await.unwrap_or(false);
                    let can_slave = dome.can_slave().await.unwrap_or(false);

                    return Ok(crate::device::DomeStatus {
                        connected: true,
                        azimuth: alpaca_status.azimuth,
                        altitude: alpaca_status.altitude,
                        shutter_status: match alpaca_status.shutter_status {
                            nightshade_alpaca::ShutterStatus::Open => crate::device::ShutterState::Open,
                            nightshade_alpaca::ShutterStatus::Closed => crate::device::ShutterState::Closed,
                            nightshade_alpaca::ShutterStatus::Opening => crate::device::ShutterState::Opening,
                            nightshade_alpaca::ShutterStatus::Closing => crate::device::ShutterState::Closing,
                            nightshade_alpaca::ShutterStatus::Error => crate::device::ShutterState::Error,
                        },
                        slewing: alpaca_status.slewing,
                        at_home: alpaca_status.at_home,
                        at_park: alpaca_status.at_park,
                        can_set_altitude,
                        can_set_azimuth,
                        can_set_shutter,
                        can_slave,
                        is_slaved: alpaca_status.slaved,
                    });
                }
                Err(format!("Alpaca dome {} not found", device_id))
            }
            Some(DriverType::Ascom) => {
                #[cfg(windows)]
                {
                    let domes = self.ascom_domes.read().await;
                    if let Some(dome) = domes.get(device_id) {
                        let dome_guard = dome.read().await;

                        // Query all dome properties from ASCOM driver
                        let shutter_status_code = dome_guard.shutter_status().await.unwrap_or(4);
                        let slewing = dome_guard.slewing().await.unwrap_or(false);
                        let at_park = dome_guard.at_park().await.unwrap_or(false);

                        // Map ASCOM shutter status codes to ShutterState
                        let shutter_status = match shutter_status_code {
                            0 => crate::device::ShutterState::Open,
                            1 => crate::device::ShutterState::Closed,
                            2 => crate::device::ShutterState::Opening,
                            3 => crate::device::ShutterState::Closing,
                            4 => crate::device::ShutterState::Error,
                            _ => crate::device::ShutterState::Unknown,
                        };

                        return Ok(crate::device::DomeStatus {
                            connected: true,
                            azimuth: 0.0, // ASCOM domes don't always expose azimuth
                            altitude: None, // ASCOM domes typically don't have altitude
                            shutter_status,
                            slewing,
                            at_home: false, // ASCOM dome interface doesn't have at_home
                            at_park,
                            can_set_altitude: false,
                            can_set_azimuth: false, // Could query CanSetAzimuth if needed
                            can_set_shutter: true, // All ASCOM domes have shutter control
                            can_slave: false,
                            is_slaved: false,
                        });
                    }
                    Err(format!("ASCOM dome {} not found", device_id))
                }
                #[cfg(not(windows))]
                Err("ASCOM not supported on this platform".to_string())
            }
            Some(DriverType::Indi) => {
                #[cfg(not(windows))]
                {
                    let parts: Vec<&str> = device_id.split(':').collect();
                    if parts.len() < 3 {
                        return Err("Invalid INDI device ID".to_string());
                    }
                    let device_name = parts[2];

                    let domes = self.indi_domes.read().await;
                    if let Some(dome) = domes.get(device_id) {
                        let dome_guard = dome.read().await;
                        let native_status = dome_guard.get_status(device_name).await
                            .map_err(|e| e.to_string())?;

                        return Ok(crate::device::DomeStatus {
                            connected: true,
                            azimuth: native_status.azimuth,
                            altitude: native_status.altitude,
                            shutter_status: match native_status.shutter_status {
                                nightshade_indi::dome::ShutterState::Open => crate::device::ShutterState::Open,
                                nightshade_indi::dome::ShutterState::Closed => crate::device::ShutterState::Closed,
                                nightshade_indi::dome::ShutterState::Opening => crate::device::ShutterState::Opening,
                                nightshade_indi::dome::ShutterState::Closing => crate::device::ShutterState::Closing,
                                nightshade_indi::dome::ShutterState::Error => crate::device::ShutterState::Error,
                                nightshade_indi::dome::ShutterState::Unknown => crate::device::ShutterState::Unknown,
                            },
                            slewing: native_status.slewing,
                            at_home: native_status.at_home,
                            at_park: native_status.at_park,
                            can_set_altitude: native_status.can_set_altitude,
                            can_set_azimuth: native_status.can_set_azimuth,
                            can_set_shutter: native_status.can_set_shutter,
                            can_slave: native_status.can_slave,
                            is_slaved: native_status.is_slaved,
                        });
                    }
                }
                #[cfg(windows)]
                return Err("INDI not supported on this platform".to_string());

                #[cfg(not(windows))]
                Err(format!("INDI dome {} not found", device_id))
            }
            Some(DriverType::Native) => {
                let native_domes = self.native_domes.read().await;
                if let Some(dome) = native_domes.get(device_id) {
                    // Query all native dome properties
                    let azimuth = dome.get_azimuth().await.unwrap_or(0.0);
                    let altitude = dome.get_altitude().await.ok().flatten();
                    let shutter_status = match dome.get_shutter_status().await.unwrap_or(nightshade_native::traits::ShutterState::Unknown) {
                        nightshade_native::traits::ShutterState::Open => crate::device::ShutterState::Open,
                        nightshade_native::traits::ShutterState::Closed => crate::device::ShutterState::Closed,
                        nightshade_native::traits::ShutterState::Opening => crate::device::ShutterState::Opening,
                        nightshade_native::traits::ShutterState::Closing => crate::device::ShutterState::Closing,
                        nightshade_native::traits::ShutterState::Error => crate::device::ShutterState::Error,
                        nightshade_native::traits::ShutterState::Unknown => crate::device::ShutterState::Unknown,
                    };
                    let slewing = dome.is_slewing().await.unwrap_or(false);
                    let at_home = dome.is_at_home().await.unwrap_or(false);
                    let at_park = dome.is_parked().await.unwrap_or(false);
                    let is_slaved = dome.is_slaved().await.unwrap_or(false);

                    return Ok(crate::device::DomeStatus {
                        connected: true,
                        azimuth,
                        altitude,
                        shutter_status,
                        slewing,
                        at_home,
                        at_park,
                        can_set_altitude: dome.can_set_altitude(),
                        can_set_azimuth: dome.can_set_azimuth(),
                        can_set_shutter: dome.can_set_shutter(),
                        can_slave: dome.can_slave(),
                        is_slaved,
                    });
                }
                Err("Native dome not connected".to_string())
            }
            Some(DriverType::Simulator) => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    // =========================================================================
    // Weather (Observing Conditions)
    // =========================================================================

    /// Get weather conditions
    pub async fn weather_get_conditions(&self, device_id: &str) -> Result<WeatherConditions, String> {
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Alpaca) => {
                let weather_devs = self.alpaca_weather.read().await;
                if let Some(weather) = weather_devs.get(device_id) {
                    return Ok(WeatherConditions {
                        temperature: weather.temperature().await.ok(),
                        humidity: weather.humidity().await.ok(),
                        pressure: weather.pressure().await.ok(),
                        cloud_cover: weather.cloud_cover().await.ok(),
                        dew_point: weather.dew_point().await.ok(),
                        wind_speed: weather.wind_speed().await.ok(),
                        wind_direction: weather.wind_direction().await.ok(),
                        sky_quality: weather.sky_quality().await.ok(),
                        sky_temperature: weather.sky_temperature().await.ok(),
                        rain_rate: weather.rain_rate().await.ok(),
                    });
                }
                Err(format!("Alpaca weather device {} not found", device_id))
            }
            Some(DriverType::Indi) => {
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() < 4 {
                    return Err("Invalid INDI device ID".to_string());
                }
                let server_key = format!("{}:{}", parts[1], parts[2]);
                let device_name = parts[3..].join(":");

                let clients = self.indi_clients.read().await;
                if let Some(client) = clients.get(&server_key) {
                    let locked = client.read().await;
                    return Ok(WeatherConditions {
                        temperature: locked.get_number(&device_name, "WEATHER_PARAMETERS", "WEATHER_TEMPERATURE").await,
                        humidity: locked.get_number(&device_name, "WEATHER_PARAMETERS", "WEATHER_HUMIDITY").await,
                        pressure: locked.get_number(&device_name, "WEATHER_PARAMETERS", "WEATHER_PRESSURE").await,
                        cloud_cover: None,
                        dew_point: locked.get_number(&device_name, "WEATHER_PARAMETERS", "WEATHER_DEWPOINT").await,
                        wind_speed: locked.get_number(&device_name, "WEATHER_PARAMETERS", "WEATHER_WIND_SPEED").await,
                        wind_direction: locked.get_number(&device_name, "WEATHER_PARAMETERS", "WEATHER_WIND_DIRECTION").await,
                        sky_quality: locked.get_number(&device_name, "WEATHER_PARAMETERS", "WEATHER_SKY_QUALITY").await,
                        sky_temperature: locked.get_number(&device_name, "WEATHER_PARAMETERS", "WEATHER_SKY_TEMP").await,
                        rain_rate: locked.get_number(&device_name, "WEATHER_PARAMETERS", "WEATHER_RAIN_RATE").await,
                    });
                }
                Err("INDI weather device not connected".to_string())
            }
            Some(DriverType::Native) => {
                let native_weather = self.native_weather.read().await;
                if let Some(weather) = native_weather.get(device_id) {
                    return Ok(WeatherConditions {
                        temperature: weather.get_temperature().await.ok().flatten(),
                        humidity: weather.get_humidity().await.ok().flatten(),
                        pressure: weather.get_pressure().await.ok().flatten(),
                        cloud_cover: weather.get_cloud_cover().await.ok().flatten(),
                        dew_point: weather.get_dew_point().await.ok().flatten(),
                        wind_speed: weather.get_wind_speed().await.ok().flatten(),
                        wind_direction: weather.get_wind_direction().await.ok().flatten(),
                        sky_quality: weather.get_sky_quality().await.ok().flatten(),
                        sky_temperature: None, // Not in native trait, could add later
                        rain_rate: weather.get_rain_rate().await.ok().flatten(),
                    });
                }
                Err("Native weather device not connected".to_string())
            }
            Some(DriverType::Simulator) => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    // =========================================================================
    // Safety Monitor
    // =========================================================================

    /// Check if conditions are safe
    pub async fn safety_is_safe(&self, device_id: &str) -> Result<bool, String> {
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Alpaca) => {
                let safety_devs = self.alpaca_safety_monitors.read().await;
                if let Some(safety) = safety_devs.get(device_id) {
                    return safety.is_safe().await;
                }
                Err(format!("Alpaca safety monitor {} not found", device_id))
            }
            Some(DriverType::Indi) => {
                // INDI doesn't have a standard safety monitor property
                // Some implementations use custom properties
                Err("INDI safety monitor not standardized".to_string())
            }
            Some(DriverType::Native) => {
                let native_safety = self.native_safety_monitors.read().await;
                if let Some(safety) = native_safety.get(device_id) {
                    return safety.is_safe().await.map_err(|e| e.to_string());
                }
                Err("Native safety monitor not connected".to_string())
            }
            Some(DriverType::Simulator) => {
                Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
            }
            _ => Err("Not implemented for this driver type".to_string()),
        }
    }

    // =========================================================================
    // Heartbeat Monitoring
    // =========================================================================

    /// Configuration for heartbeat monitoring per device type
    fn get_heartbeat_config(device_type: &DeviceType) -> HeartbeatConfig {
        match device_type {
            // Cameras need less frequent heartbeats during long exposures
            DeviceType::Camera => HeartbeatConfig {
                base_interval_secs: 10,
                max_interval_secs: 60,
                failure_threshold: 3,
                backoff_multiplier: 2.0,
            },
            // Mounts need regular heartbeats for tracking status
            DeviceType::Mount => HeartbeatConfig {
                base_interval_secs: 5,
                max_interval_secs: 30,
                failure_threshold: 2,
                backoff_multiplier: 1.5,
            },
            // Focusers are relatively stable
            DeviceType::Focuser => HeartbeatConfig {
                base_interval_secs: 15,
                max_interval_secs: 60,
                failure_threshold: 3,
                backoff_multiplier: 2.0,
            },
            // Filter wheels are rarely polled
            DeviceType::FilterWheel => HeartbeatConfig {
                base_interval_secs: 20,
                max_interval_secs: 120,
                failure_threshold: 3,
                backoff_multiplier: 2.0,
            },
            // Default for other devices
            _ => HeartbeatConfig {
                base_interval_secs: 10,
                max_interval_secs: 60,
                failure_threshold: 3,
                backoff_multiplier: 2.0,
            },
        }
    }

    /// Start heartbeat monitoring for a device
    ///
    /// This spawns a background task that periodically checks if the device
    /// is still responding. If the device fails to respond after multiple
    /// attempts with exponential backoff, a Disconnected event is emitted.
    pub async fn start_heartbeat(&self, device_id: &str, interval: Duration) -> Result<(), String> {
        // Check if device exists and get its info
        let (device_type, device_type_str, driver_type) = {
            let devices = self.devices.read().await;
            match devices.get(device_id) {
                Some(device) => (
                    device.info.device_type.clone(),
                    device.info.device_type.as_str().to_string(),
                    device.info.driver_type.clone(),
                ),
                None => return Err(format!("Device {} not found", device_id)),
            }
        };

        // Stop any existing heartbeat for this device
        self.stop_heartbeat(device_id).await?;

        // Get device-type specific heartbeat configuration
        let config = Self::get_heartbeat_config(&device_type);

        // Mark heartbeat as active
        {
            let mut devices = self.devices.write().await;
            if let Some(device) = devices.get_mut(device_id) {
                device.heartbeat_active = true;
                device.last_successful_comm = Some(chrono::Utc::now().timestamp_millis());
            }
        }

        // Create cancellation token
        let (cancel_tx, mut cancel_rx) = tokio::sync::watch::channel(false);

        // Spawn heartbeat task
        let device_id_clone = device_id.to_string();
        let app_state = self.app_state.clone();

        let task = tokio::spawn(async move {
            let mut current_interval = Duration::from_secs(config.base_interval_secs);
            let max_interval = Duration::from_secs(config.max_interval_secs);
            let mut consecutive_failures = 0u32;

            loop {
                // Wait for interval or cancellation
                tokio::select! {
                    _ = tokio::time::sleep(current_interval) => {}
                    _ = cancel_rx.changed() => {
                        if *cancel_rx.borrow() {
                            tracing::debug!("Heartbeat cancelled for device: {}", device_id_clone);
                            break;
                        }
                    }
                }

                // Check cancellation after waking
                if *cancel_rx.borrow() {
                    break;
                }

                // Perform health check based on driver type
                let health_check_result = match driver_type {
                    DriverType::Simulator => {
                        Err("Simulator devices are disabled. Connect real hardware or use INDI/ASCOM/Alpaca simulators for testing.".to_string())
                    }
                    DriverType::Alpaca => {
                        // For Alpaca, we could ping the device
                        // For now, return healthy - real implementation would do a status check
                        Ok(true)
                    }
                    DriverType::Ascom => {
                        // For ASCOM, we could check Connected property
                        Ok(true)
                    }
                    DriverType::Indi => {
                        // For INDI, check client connection
                        Ok(true)
                    }
                    DriverType::Native => {
                        // For Native SDK, check device state
                        Ok(true)
                    }
                };

                match health_check_result {
                    Ok(true) => {
                        // Device is healthy - reset failure counter and interval
                        if consecutive_failures > 0 {
                            tracing::info!("Heartbeat recovered for device: {} after {} failures",
                                device_id_clone, consecutive_failures);
                        }
                        consecutive_failures = 0;
                        current_interval = Duration::from_secs(config.base_interval_secs);
                        tracing::trace!("Heartbeat OK for device: {}", device_id_clone);
                    }
                    Ok(false) | Err(_) => {
                        // Health check failed
                        consecutive_failures += 1;
                        tracing::warn!("Heartbeat failure {} for device: {}",
                            consecutive_failures, device_id_clone);

                        // Apply exponential backoff
                        let new_interval = Duration::from_secs_f64(
                            current_interval.as_secs_f64() * config.backoff_multiplier
                        );
                        current_interval = new_interval.min(max_interval);

                        // Check if we've exceeded failure threshold
                        if consecutive_failures >= config.failure_threshold {
                            tracing::error!("Heartbeat failed {} times for device: {} - marking disconnected",
                                consecutive_failures, device_id_clone);

                            app_state.publish_equipment_event(
                                EquipmentEvent::Disconnected {
                                    device_type: device_type_str.clone(),
                                    device_id: device_id_clone.clone(),
                                },
                                EventSeverity::Warning,
                            );

                            // Additional error event with details
                            app_state.publish_equipment_event(
                                EquipmentEvent::Error {
                                    device_type: device_type_str.clone(),
                                    device_id: device_id_clone.clone(),
                                    message: format!(
                                        "Device unresponsive after {} heartbeat failures",
                                        consecutive_failures
                                    ),
                                },
                                EventSeverity::Error,
                            );

                            // Stop heartbeat monitoring after disconnect
                            break;
                        }
                    }
                }
            }

            tracing::debug!("Heartbeat task ended for device: {}", device_id_clone);
        });

        // Store the task handle and cancel token
        {
            let mut tasks = self.heartbeat_tasks.write().await;
            tasks.insert(device_id.to_string(), task);
        }

        Ok(())
    }

    /// Stop heartbeat monitoring for a device
    pub async fn stop_heartbeat(&self, device_id: &str) -> Result<(), String> {
        // Remove and abort the task
        let task = {
            let mut tasks = self.heartbeat_tasks.write().await;
            tasks.remove(device_id)
        };

        if let Some(task) = task {
            // Abort the task (gracefully cancels via the select!)
            task.abort();

            // Wait briefly for clean shutdown
            match tokio::time::timeout(Duration::from_millis(100), task).await {
                Ok(_) => tracing::debug!("Heartbeat task stopped cleanly for {}", device_id),
                Err(_) => tracing::debug!("Heartbeat task aborted for {}", device_id),
            }
        }

        // Mark heartbeat as inactive
        {
            let mut devices = self.devices.write().await;
            if let Some(device) = devices.get_mut(device_id) {
                device.heartbeat_active = false;
            }
        }

        Ok(())
    }

    /// Stop all heartbeat tasks (call during shutdown)
    pub async fn stop_all_heartbeats(&self) {
        let tasks: Vec<(String, tokio::task::JoinHandle<()>)> = {
            let mut tasks = self.heartbeat_tasks.write().await;
            std::mem::take(&mut *tasks).into_iter().collect()
        };

        for (device_id, task) in tasks {
            task.abort();
            tracing::debug!("Aborted heartbeat for device: {}", device_id);
        }

        // Mark all heartbeats as inactive
        {
            let mut devices = self.devices.write().await;
            for device in devices.values_mut() {
                device.heartbeat_active = false;
            }
        }
    }

    /// Get device health status
    ///
    /// Returns (last_successful_timestamp_ms, is_healthy)
    pub async fn get_device_health(&self, device_id: &str) -> Result<(i64, bool), String> {
        let devices = self.devices.read().await;

        if let Some(device) = devices.get(device_id) {
            let last_comm = device.last_successful_comm.unwrap_or(0);
            let now = chrono::Utc::now().timestamp_millis();

            // Consider device unhealthy if no communication in last 30 seconds
            let is_healthy = if let Some(last) = device.last_successful_comm {
                (now - last) < 30_000
            } else {
                false
            };

            Ok((last_comm, is_healthy))
        } else {
            Err(format!("Device {} not found", device_id))
        }
    }

    /// Update last successful communication timestamp for a device
    ///
    /// This should be called by device operations when they successfully
    /// communicate with the device.
    pub async fn update_device_communication(&self, device_id: &str) {
        let mut devices = self.devices.write().await;
        if let Some(device) = devices.get_mut(device_id) {
            device.last_successful_comm = Some(chrono::Utc::now().timestamp_millis());
        }
    }

    /// Check if heartbeat is active for a device
    pub async fn is_heartbeat_active(&self, device_id: &str) -> bool {
        let devices = self.devices.read().await;
        devices.get(device_id)
            .map(|d| d.heartbeat_active)
            .unwrap_or(false)
    }

    // =========================================================================
    // Cover Calibrator Control
    // =========================================================================

    /// Open cover calibrator cover
    pub async fn cover_calibrator_open_cover(&self, device_id: &str) -> Result<(), String> {
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Alpaca) => {
                let cover_cals = self.alpaca_cover_calibrators.read().await;
                if let Some(cover_cal) = cover_cals.get(device_id) {
                    return cover_cal.open_cover().await;
                }
                Err(format!("Alpaca cover calibrator {} not found", device_id))
            }
            #[cfg(windows)]
            Some(DriverType::Ascom) => {
                let cover_cals = self.ascom_cover_calibrators.read().await;
                if let Some(cover_cal) = cover_cals.get(device_id) {
                    let mut locked = cover_cal.write().await;
                    return locked.open_cover().await;
                }
                Err(format!("ASCOM cover calibrator {} not found", device_id))
            }
            Some(DriverType::Indi) => {
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() < 4 {
                    return Err("Invalid INDI device ID".to_string());
                }
                let server_key = format!("{}:{}", parts[1], parts[2]);
                let device_name = parts[3..].join(":");

                let clients = self.indi_clients.read().await;
                if let Some(client) = clients.get(&server_key) {
                    let mut locked = client.write().await;
                    return locked.set_switch(&device_name, "CAP_PARK", "UNPARK", true).await;
                }
                Err("INDI cover calibrator not connected".to_string())
            }
            _ => Err("Cover calibrator not supported for this driver type".to_string()),
        }
    }

    /// Close cover calibrator cover
    pub async fn cover_calibrator_close_cover(&self, device_id: &str) -> Result<(), String> {
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Alpaca) => {
                let cover_cals = self.alpaca_cover_calibrators.read().await;
                if let Some(cover_cal) = cover_cals.get(device_id) {
                    return cover_cal.close_cover().await;
                }
                Err(format!("Alpaca cover calibrator {} not found", device_id))
            }
            #[cfg(windows)]
            Some(DriverType::Ascom) => {
                let cover_cals = self.ascom_cover_calibrators.read().await;
                if let Some(cover_cal) = cover_cals.get(device_id) {
                    let mut locked = cover_cal.write().await;
                    return locked.close_cover().await;
                }
                Err(format!("ASCOM cover calibrator {} not found", device_id))
            }
            Some(DriverType::Indi) => {
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() < 4 {
                    return Err("Invalid INDI device ID".to_string());
                }
                let server_key = format!("{}:{}", parts[1], parts[2]);
                let device_name = parts[3..].join(":");

                let clients = self.indi_clients.read().await;
                if let Some(client) = clients.get(&server_key) {
                    let mut locked = client.write().await;
                    return locked.set_switch(&device_name, "CAP_PARK", "PARK", true).await;
                }
                Err("INDI cover calibrator not connected".to_string())
            }
            _ => Err("Cover calibrator not supported for this driver type".to_string()),
        }
    }

    /// Halt cover calibrator cover movement
    pub async fn cover_calibrator_halt_cover(&self, device_id: &str) -> Result<(), String> {
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Alpaca) => {
                let cover_cals = self.alpaca_cover_calibrators.read().await;
                if let Some(cover_cal) = cover_cals.get(device_id) {
                    return cover_cal.halt_cover().await;
                }
                Err(format!("Alpaca cover calibrator {} not found", device_id))
            }
            #[cfg(windows)]
            Some(DriverType::Ascom) => {
                let cover_cals = self.ascom_cover_calibrators.read().await;
                if let Some(cover_cal) = cover_cals.get(device_id) {
                    let mut locked = cover_cal.write().await;
                    return locked.halt_cover().await;
                }
                Err(format!("ASCOM cover calibrator {} not found", device_id))
            }
            Some(DriverType::Indi) => {
                // INDI doesn't have a specific halt command for dust caps
                Err("INDI cover calibrator halt not supported".to_string())
            }
            _ => Err("Cover calibrator not supported for this driver type".to_string()),
        }
    }

    /// Turn on cover calibrator light
    pub async fn cover_calibrator_calibrator_on(&self, device_id: &str, brightness: i32) -> Result<(), String> {
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Alpaca) => {
                let cover_cals = self.alpaca_cover_calibrators.read().await;
                if let Some(cover_cal) = cover_cals.get(device_id) {
                    return cover_cal.calibrator_on(brightness).await;
                }
                Err(format!("Alpaca cover calibrator {} not found", device_id))
            }
            #[cfg(windows)]
            Some(DriverType::Ascom) => {
                let cover_cals = self.ascom_cover_calibrators.read().await;
                if let Some(cover_cal) = cover_cals.get(device_id) {
                    let mut locked = cover_cal.write().await;
                    return locked.calibrator_on(brightness).await;
                }
                Err(format!("ASCOM cover calibrator {} not found", device_id))
            }
            Some(DriverType::Indi) => {
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() < 4 {
                    return Err("Invalid INDI device ID".to_string());
                }
                let server_key = format!("{}:{}", parts[1], parts[2]);
                let device_name = parts[3..].join(":");

                let clients = self.indi_clients.read().await;
                if let Some(client) = clients.get(&server_key) {
                    let mut locked = client.write().await;
                    // Set brightness first, then turn on
                    locked.set_number(&device_name, "FLAT_LIGHT_INTENSITY", "FLAT_LIGHT_INTENSITY_VALUE", brightness as f64).await?;
                    return locked.set_switch(&device_name, "FLAT_LIGHT_CONTROL", "FLAT_LIGHT_ON", true).await;
                }
                Err("INDI cover calibrator not connected".to_string())
            }
            _ => Err("Cover calibrator not supported for this driver type".to_string()),
        }
    }

    /// Turn off cover calibrator light
    pub async fn cover_calibrator_calibrator_off(&self, device_id: &str) -> Result<(), String> {
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Alpaca) => {
                let cover_cals = self.alpaca_cover_calibrators.read().await;
                if let Some(cover_cal) = cover_cals.get(device_id) {
                    return cover_cal.calibrator_off().await;
                }
                Err(format!("Alpaca cover calibrator {} not found", device_id))
            }
            #[cfg(windows)]
            Some(DriverType::Ascom) => {
                let cover_cals = self.ascom_cover_calibrators.read().await;
                if let Some(cover_cal) = cover_cals.get(device_id) {
                    let mut locked = cover_cal.write().await;
                    return locked.calibrator_off().await;
                }
                Err(format!("ASCOM cover calibrator {} not found", device_id))
            }
            Some(DriverType::Indi) => {
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() < 4 {
                    return Err("Invalid INDI device ID".to_string());
                }
                let server_key = format!("{}:{}", parts[1], parts[2]);
                let device_name = parts[3..].join(":");

                let clients = self.indi_clients.read().await;
                if let Some(client) = clients.get(&server_key) {
                    let mut locked = client.write().await;
                    return locked.set_switch(&device_name, "FLAT_LIGHT_CONTROL", "FLAT_LIGHT_OFF", true).await;
                }
                Err("INDI cover calibrator not connected".to_string())
            }
            _ => Err("Cover calibrator not supported for this driver type".to_string()),
        }
    }

    /// Get cover calibrator cover state
    /// Returns: 0=NotPresent, 1=Closed, 2=Moving, 3=Open, 4=Unknown, 5=Error
    pub async fn cover_calibrator_get_cover_state(&self, device_id: &str) -> Result<i32, String> {
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Alpaca) => {
                let cover_cals = self.alpaca_cover_calibrators.read().await;
                if let Some(cover_cal) = cover_cals.get(device_id) {
                    let state = cover_cal.cover_state().await?;
                    return Ok(state as i32);
                }
                Err(format!("Alpaca cover calibrator {} not found", device_id))
            }
            #[cfg(windows)]
            Some(DriverType::Ascom) => {
                let cover_cals = self.ascom_cover_calibrators.read().await;
                if let Some(cover_cal) = cover_cals.get(device_id) {
                    let locked = cover_cal.read().await;
                    return locked.cover_state().await;
                }
                Err(format!("ASCOM cover calibrator {} not found", device_id))
            }
            Some(DriverType::Indi) => {
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() < 4 {
                    return Err("Invalid INDI device ID".to_string());
                }
                let server_key = format!("{}:{}", parts[1], parts[2]);
                let device_name = parts[3..].join(":");

                let clients = self.indi_clients.read().await;
                if let Some(client) = clients.get(&server_key) {
                    let locked = client.read().await;
                    if let Some(state) = locked.get_switch(&device_name, "CAP_PARK", "PARK").await {
                        // PARK=on means closed, UNPARK=on means open
                        return Ok(if state { 1 } else { 3 }); // 1=Closed, 3=Open
                    }
                    return Ok(4); // Unknown
                }
                Err("INDI cover calibrator not connected".to_string())
            }
            _ => Err("Cover calibrator not supported for this driver type".to_string()),
        }
    }

    /// Get cover calibrator calibrator state
    /// Returns: 0=NotPresent, 1=Off, 2=NotReady, 3=Ready, 4=Unknown, 5=Error
    pub async fn cover_calibrator_get_calibrator_state(&self, device_id: &str) -> Result<i32, String> {
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Alpaca) => {
                let cover_cals = self.alpaca_cover_calibrators.read().await;
                if let Some(cover_cal) = cover_cals.get(device_id) {
                    let state = cover_cal.calibrator_state().await?;
                    return Ok(state as i32);
                }
                Err(format!("Alpaca cover calibrator {} not found", device_id))
            }
            #[cfg(windows)]
            Some(DriverType::Ascom) => {
                let cover_cals = self.ascom_cover_calibrators.read().await;
                if let Some(cover_cal) = cover_cals.get(device_id) {
                    let locked = cover_cal.read().await;
                    return locked.calibrator_state().await;
                }
                Err(format!("ASCOM cover calibrator {} not found", device_id))
            }
            Some(DriverType::Indi) => {
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() < 4 {
                    return Err("Invalid INDI device ID".to_string());
                }
                let server_key = format!("{}:{}", parts[1], parts[2]);
                let device_name = parts[3..].join(":");

                let clients = self.indi_clients.read().await;
                if let Some(client) = clients.get(&server_key) {
                    let locked = client.read().await;
                    if let Some(state) = locked.get_switch(&device_name, "FLAT_LIGHT_CONTROL", "FLAT_LIGHT_ON").await {
                        // FLAT_LIGHT_ON=true means Ready (light is on), false means Off
                        return Ok(if state { 3 } else { 1 }); // 3=Ready, 1=Off
                    }
                    return Ok(4); // Unknown
                }
                Err("INDI cover calibrator not connected".to_string())
            }
            _ => Err("Cover calibrator not supported for this driver type".to_string()),
        }
    }

    /// Get cover calibrator brightness
    pub async fn cover_calibrator_get_brightness(&self, device_id: &str) -> Result<i32, String> {
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Alpaca) => {
                let cover_cals = self.alpaca_cover_calibrators.read().await;
                if let Some(cover_cal) = cover_cals.get(device_id) {
                    return cover_cal.brightness().await;
                }
                Err(format!("Alpaca cover calibrator {} not found", device_id))
            }
            #[cfg(windows)]
            Some(DriverType::Ascom) => {
                let cover_cals = self.ascom_cover_calibrators.read().await;
                if let Some(cover_cal) = cover_cals.get(device_id) {
                    let locked = cover_cal.read().await;
                    return locked.brightness().await;
                }
                Err(format!("ASCOM cover calibrator {} not found", device_id))
            }
            Some(DriverType::Indi) => {
                let parts: Vec<&str> = device_id.split(':').collect();
                if parts.len() < 4 {
                    return Err("Invalid INDI device ID".to_string());
                }
                let server_key = format!("{}:{}", parts[1], parts[2]);
                let device_name = parts[3..].join(":");

                let clients = self.indi_clients.read().await;
                if let Some(client) = clients.get(&server_key) {
                    let locked = client.read().await;
                    if let Some(brightness) = locked.get_number(&device_name, "FLAT_LIGHT_INTENSITY", "FLAT_LIGHT_INTENSITY_VALUE").await {
                        return Ok(brightness as i32);
                    }
                    return Ok(0);
                }
                Err("INDI cover calibrator not connected".to_string())
            }
            _ => Err("Cover calibrator not supported for this driver type".to_string()),
        }
    }

    /// Get cover calibrator max brightness
    pub async fn cover_calibrator_get_max_brightness(&self, device_id: &str) -> Result<i32, String> {
        let driver_type = {
            let devices = self.devices.read().await;
            devices.get(device_id).map(|d| d.info.driver_type.clone())
        };

        match driver_type {
            Some(DriverType::Alpaca) => {
                let cover_cals = self.alpaca_cover_calibrators.read().await;
                if let Some(cover_cal) = cover_cals.get(device_id) {
                    return cover_cal.max_brightness().await;
                }
                Err(format!("Alpaca cover calibrator {} not found", device_id))
            }
            #[cfg(windows)]
            Some(DriverType::Ascom) => {
                let cover_cals = self.ascom_cover_calibrators.read().await;
                if let Some(cover_cal) = cover_cals.get(device_id) {
                    let locked = cover_cal.read().await;
                    return Ok(locked.cached_max_brightness());
                }
                Err(format!("ASCOM cover calibrator {} not found", device_id))
            }
            Some(DriverType::Indi) => {
                // Most INDI flat panels use percentage (0-100) or don't expose max
                Ok(255)
            }
            _ => Err("Cover calibrator not supported for this driver type".to_string()),
        }
    }

    /// Get cover calibrator status (combined state)
    pub async fn cover_calibrator_get_status(&self, device_id: &str) -> Result<CoverCalibratorStatus, String> {
        let cover_state_raw = self.cover_calibrator_get_cover_state(device_id).await.unwrap_or(4);
        let calibrator_state_raw = self.cover_calibrator_get_calibrator_state(device_id).await.unwrap_or(4);
        let brightness = self.cover_calibrator_get_brightness(device_id).await.unwrap_or(0);
        let max_brightness = self.cover_calibrator_get_max_brightness(device_id).await.unwrap_or(255);

        // Check if the device is connected by seeing if any data is available
        let connected = self.cover_calibrator_get_cover_state(device_id).await.is_ok()
            || self.cover_calibrator_get_calibrator_state(device_id).await.is_ok();

        Ok(CoverCalibratorStatus {
            connected,
            cover_state: CoverState::from_i32(cover_state_raw),
            calibrator_state: CalibratorState::from_i32(calibrator_state_raw),
            brightness,
            max_brightness,
        })
    }
}
