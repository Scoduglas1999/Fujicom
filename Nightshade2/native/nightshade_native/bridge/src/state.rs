//! Global state management for the Nightshade application
//!
//! This module maintains the application state and provides
//! thread-safe access to it from multiple components.

use crate::device::*;
use crate::event::*;
use crate::storage::ProfileStorage;
use crate::storage::SettingsStorage;
use crate::storage::AppSettings;
use crate::storage::ObserverLocation;
use std::collections::HashMap;
use std::sync::Arc;
use std::sync::OnceLock;
use tokio::sync::RwLock;

/// Global application state
pub struct AppState {
    /// Event bus for publishing/subscribing to events
    pub event_bus: SharedEventBus,

    /// Connected devices by type and ID
    devices: RwLock<HashMap<(DeviceType, String), DeviceConnection>>,

    /// Current session state
    session: RwLock<SessionState>,

    /// Equipment profile
    profile: RwLock<Option<EquipmentProfile>>,

    /// Observer location (in-memory, synced from Dart on startup/change)
    /// This is the single source of truth for location during runtime
    observer_location: RwLock<Option<ObserverLocation>>,
}

/// Global profile storage
static PROFILE_STORAGE: OnceLock<ProfileStorage> = OnceLock::new();

/// Initialize profile storage
pub fn init_profile_storage(storage_dir: std::path::PathBuf) -> Result<(), String> {
    let storage = ProfileStorage::new(storage_dir)?;
    PROFILE_STORAGE.set(storage).map_err(|_| "Profile storage already initialized".to_string())
}

fn get_profile_storage() -> Result<&'static ProfileStorage, String> {
    PROFILE_STORAGE.get().ok_or_else(|| "Profile storage not initialized".to_string())
}

/// Global settings storage
static SETTINGS_STORAGE: OnceLock<SettingsStorage> = OnceLock::new();

/// Initialize settings storage
pub fn init_settings_storage(storage_dir: std::path::PathBuf) -> Result<(), String> {
    let storage = SettingsStorage::new(storage_dir)?;
    SETTINGS_STORAGE.set(storage).map_err(|_| "Settings storage already initialized".to_string())
}

fn get_settings_storage() -> Result<&'static SettingsStorage, String> {
    SETTINGS_STORAGE.get().ok_or_else(|| "Settings storage not initialized".to_string())
}

/// A connected device with its current status
pub struct DeviceConnection {
    pub info: DeviceInfo,
    pub state: ConnectionState,
}

/// Current imaging session state
#[derive(Debug, Clone, Default)]
pub struct SessionState {
    pub is_active: bool,
    pub start_time: Option<i64>,
    pub target_name: Option<String>,
    pub target_ra: Option<f64>,
    pub target_dec: Option<f64>,
    pub total_exposures: u32,
    pub completed_exposures: u32,
    pub total_integration_secs: f64,
    pub current_filter: Option<String>,
    pub is_guiding: bool,
    pub is_capturing: bool,
    pub is_dithering: bool,
}

/// Equipment profile containing device selections
#[derive(Debug, Clone, serde::Serialize, serde::Deserialize)]
pub struct EquipmentProfile {
    pub id: String,
    pub name: String,
    pub camera_id: Option<String>,
    pub mount_id: Option<String>,
    pub focuser_id: Option<String>,
    pub filter_wheel_id: Option<String>,
    pub guider_id: Option<String>,
    pub rotator_id: Option<String>,
    pub dome_id: Option<String>,
    pub weather_id: Option<String>,
    pub cover_calibrator_id: Option<String>,
    pub telescope_focal_length: f64,
    pub telescope_aperture: f64,
}

impl AppState {
    /// Create a new application state
    pub fn new() -> Arc<Self> {
        Arc::new(Self {
            event_bus: Arc::new(EventBus::new(crate::event::DEFAULT_EVENT_BUFFER_SIZE)),
            devices: RwLock::new(HashMap::new()),
            session: RwLock::new(SessionState::default()),
            profile: RwLock::new(None),
            observer_location: RwLock::new(None),
        })
    }
    
    /// Create with profile storage (Legacy - storage is now global)
    pub fn new_with_storage(storage_dir: std::path::PathBuf) -> Arc<Self> {
        let _ = init_profile_storage(storage_dir);
        Self::new()
    }

    /// Register a device connection
    pub async fn register_device(&self, info: DeviceInfo, state: ConnectionState) {
        let key = (info.device_type, info.id.clone());
        let connection = DeviceConnection { info, state };
        
        let mut devices = self.devices.write().await;
        devices.insert(key, connection);
    }

    /// Update a device's connection state
    pub async fn update_device_state(&self, device_type: DeviceType, device_id: &str, state: ConnectionState) {
        let key = (device_type, device_id.to_string());
        
        let mut devices = self.devices.write().await;
        if let Some(conn) = devices.get_mut(&key) {
            conn.state = state;
        }
    }

    /// Remove a device
    pub async fn remove_device(&self, device_type: DeviceType, device_id: &str) {
        let key = (device_type, device_id.to_string());
        
        let mut devices = self.devices.write().await;
        devices.remove(&key);
    }

    /// Get all devices of a specific type
    pub async fn get_devices(&self, device_type: DeviceType) -> Vec<DeviceInfo> {
        let devices = self.devices.read().await;
        devices
            .iter()
            .filter(|((dt, _), _)| *dt == device_type)
            .map(|(_, conn)| conn.info.clone())
            .collect()
    }

    /// Get a specific device
    pub async fn get_device(&self, device_type: DeviceType, device_id: &str) -> Option<DeviceInfo> {
        let key = (device_type, device_id.to_string());
        let devices = self.devices.read().await;
        devices.get(&key).map(|conn| conn.info.clone())
    }

    /// Check if a device is connected
    pub async fn is_device_connected(&self, device_type: DeviceType, device_id: &str) -> bool {
        let key = (device_type, device_id.to_string());
        let devices = self.devices.read().await;
        devices
            .get(&key)
            .map(|conn| conn.state == ConnectionState::Connected)
            .unwrap_or(false)
    }

    /// Get the current session state
    pub async fn get_session(&self) -> SessionState {
        self.session.read().await.clone()
    }

    /// Update the session state
    pub async fn update_session<F>(&self, updater: F)
    where
        F: FnOnce(&mut SessionState),
    {
        let mut session = self.session.write().await;
        updater(&mut session);
    }

    /// Start a new session
    pub async fn start_session(&self, target_name: Option<String>, ra: Option<f64>, dec: Option<f64>) {
        let mut session = self.session.write().await;
        *session = SessionState {
            is_active: true,
            start_time: Some(chrono::Utc::now().timestamp_millis()),
            target_name,
            target_ra: ra,
            target_dec: dec,
            ..Default::default()
        };
    }

    /// End the current session
    pub async fn end_session(&self) {
        let mut session = self.session.write().await;
        session.is_active = false;
    }

    /// Get the current equipment profile
    pub async fn get_profile(&self) -> Option<EquipmentProfile> {
        self.profile.read().await.clone()
    }

    /// Set the equipment profile
    pub async fn set_profile(&self, profile: Option<EquipmentProfile>) {
        let mut p = self.profile.write().await;
        *p = profile;
    }
    
    /// Load all profiles from storage
    pub fn load_profiles(&self) -> Result<Vec<EquipmentProfile>, String> {
        get_profile_storage()?.load_profiles()
    }
    
    /// Save a profile to storage
    pub fn save_profile_to_storage(&self, profile: &EquipmentProfile) -> Result<(), String> {
        get_profile_storage()?.save_profile(profile)
    }
    
    /// Delete a profile from storage
    pub fn delete_profile_from_storage(&self, profile_id: &str) -> Result<(), String> {
        get_profile_storage()?.delete_profile(profile_id)
    }
    
    /// Load a specific profile from storage and set it as active
    pub async fn load_and_set_profile(&self, profile_id: &str) -> Result<(), String> {
        let profile = get_profile_storage()?.get_profile(profile_id)?;
        
        self.set_profile(Some(profile)).await;
        Ok(())
    }

    /// Get current settings
    pub fn get_settings(&self) -> Result<AppSettings, String> {
        get_settings_storage()?.load_settings()
    }
    
    /// Update settings
    pub fn update_settings(&self, settings: &AppSettings) -> Result<(), String> {
        get_settings_storage()?.save_settings(settings)
    }
    
    /// Get observer location from in-memory state
    /// This is the primary accessor - reads from runtime state, not file
    pub fn get_observer_location(&self) -> Result<Option<ObserverLocation>, String> {
        // Use try_read to avoid panicking when called from async context
        // blocking_read() panics if called from within a Tokio runtime
        match self.observer_location.try_read() {
            Ok(guard) => {
                let location = guard.clone();
                match &location {
                    Some(loc) => {
                        // Only log occasionally to avoid spam - log on first call
                        tracing::debug!("Retrieved observer location: lat={}, lon={}, elev={}",
                            loc.latitude, loc.longitude, loc.elevation);
                    }
                    None => {
                        tracing::debug!("Observer location is not set");
                    }
                }
                Ok(location)
            }
            Err(_) => {
                // Lock is held for write, return None rather than blocking
                eprintln!("[RUST-STATE] get_observer_location: lock busy, returning None");
                tracing::debug!("Observer location lock busy, returning None");
                Ok(None)
            }
        }
    }

    /// Set observer location in-memory and optionally persist to file
    /// This updates the runtime state that all components use
    pub fn set_observer_location(&self, location: Option<ObserverLocation>) -> Result<(), String> {
        eprintln!("[RUST-STATE] set_observer_location called");
        match &location {
            Some(loc) => {
                eprintln!("[RUST-STATE] Setting observer location: lat={}, lon={}, elev={}",
                    loc.latitude, loc.longitude, loc.elevation);
                tracing::info!("Setting observer location: lat={}, lon={}, elev={}",
                    loc.latitude, loc.longitude, loc.elevation);
            }
            None => {
                eprintln!("[RUST-STATE] Clearing observer location");
                tracing::info!("Clearing observer location");
            }
        }

        // Update in-memory state (this is the primary source of truth at runtime)
        // Use try_write to avoid panicking when called from async context
        match self.observer_location.try_write() {
            Ok(mut loc_guard) => {
                *loc_guard = location.clone();
                eprintln!("[RUST-STATE] Observer location updated in memory (try_write succeeded)");
                tracing::debug!("Observer location updated in memory");
            }
            Err(_) => {
                // Lock is held for read, try to wait a bit and retry
                eprintln!("[RUST-STATE] try_write failed, using blocking_write");
                tracing::warn!("Observer location lock busy for write, using blocking fallback");
                let mut loc_guard = self.observer_location.blocking_write();
                *loc_guard = location.clone();
                eprintln!("[RUST-STATE] Observer location updated in memory (blocking)");
                tracing::debug!("Observer location updated in memory (blocking)");
            }
        }

        // Also persist to settings file if storage is initialized (best-effort)
        if let Ok(mut settings) = self.get_settings() {
            settings.location = location;
            if let Err(e) = self.update_settings(&settings) {
                tracing::warn!("Failed to persist observer location to file: {}", e);
                // Don't fail - in-memory state is already updated
            } else {
                tracing::debug!("Observer location persisted to file");
            }
        }

        Ok(())
    }

    /// Load observer location from persisted settings into in-memory state
    /// Call this at startup after settings storage is initialized
    pub fn load_observer_location_from_settings(&self) {
        if let Ok(settings) = self.get_settings() {
            if let Some(loc) = settings.location {
                tracing::info!("Loading observer location from settings: lat={}, lon={}, elev={}",
                    loc.latitude, loc.longitude, loc.elevation);
                let mut loc_guard = self.observer_location.blocking_write();
                *loc_guard = Some(loc);
            }
        }
    }

    // =========================================================================
    // Event Publishing (using new event bus with sequence numbers)
    // =========================================================================

    /// Publish an event to the event bus
    /// Returns the event ID
    pub fn publish_event(&self, event: NightshadeEvent) -> u64 {
        self.event_bus.publish(event)
    }

    /// Create and publish a system event
    /// Returns the event ID
    pub fn publish_system_event(&self, event: SystemEvent) -> u64 {
        self.event_bus.publish_with_tracking(
            EventSeverity::Info,
            EventCategory::System,
            EventPayload::System(event),
            None,
        )
    }

    /// Create and publish an equipment event
    /// Returns the event ID
    pub fn publish_equipment_event(&self, event: EquipmentEvent, severity: EventSeverity) -> u64 {
        // Extract device_id from the event for context
        let device_id = match &event {
            EquipmentEvent::Connecting { device_id, .. } => Some(device_id.clone()),
            EquipmentEvent::Connected { device_id, .. } => Some(device_id.clone()),
            EquipmentEvent::Disconnected { device_id, .. } => Some(device_id.clone()),
            EquipmentEvent::PropertyChanged { device_id, .. } => Some(device_id.clone()),
            EquipmentEvent::Error { device_id, .. } => Some(device_id.clone()),
            _ => None,
        };

        if let Some(ref device_id) = device_id {
            self.event_bus.publish_device_event(
                severity,
                EventCategory::Equipment,
                EventPayload::Equipment(event),
                device_id,
                None,
            )
        } else {
            self.event_bus.publish_with_tracking(
                severity,
                EventCategory::Equipment,
                EventPayload::Equipment(event),
                None,
            )
        }
    }

    /// Create and publish an equipment event with causality
    pub fn publish_equipment_event_caused_by(
        &self,
        event: EquipmentEvent,
        severity: EventSeverity,
        caused_by: u64,
    ) -> u64 {
        self.event_bus.publish_with_tracking(
            severity,
            EventCategory::Equipment,
            EventPayload::Equipment(event),
            Some(caused_by),
        )
    }

    /// Create and publish an imaging event
    /// Returns the event ID
    pub fn publish_imaging_event(&self, event: ImagingEvent, severity: EventSeverity) -> u64 {
        self.event_bus.publish_with_tracking(
            severity,
            EventCategory::Imaging,
            EventPayload::Imaging(event),
            None,
        )
    }

    /// Create and publish an imaging event with correlation ID (for grouping related events)
    pub fn publish_imaging_event_correlated(
        &self,
        event: ImagingEvent,
        severity: EventSeverity,
        correlation_id: &str,
    ) -> u64 {
        self.event_bus.publish_correlated(
            severity,
            EventCategory::Imaging,
            EventPayload::Imaging(event),
            correlation_id,
            None,
        )
    }

    /// Create and publish a guiding event
    /// Returns the event ID
    pub fn publish_guiding_event(&self, event: GuidingEvent, severity: EventSeverity) -> u64 {
        self.event_bus.publish_with_tracking(
            severity,
            EventCategory::Guiding,
            EventPayload::Guiding(event),
            None,
        )
    }

    /// Create and publish a safety event
    /// Returns the event ID
    pub fn publish_safety_event(&self, event: SafetyEvent, severity: EventSeverity) -> u64 {
        self.event_bus.publish_with_tracking(
            severity,
            EventCategory::Safety,
            EventPayload::Safety(event),
            None,
        )
    }

    /// Create and publish a sequencer event
    /// Returns the event ID
    pub fn publish_sequencer_event(&self, event: SequencerEvent, severity: EventSeverity) -> u64 {
        self.event_bus.publish_with_tracking(
            severity,
            EventCategory::Sequencer,
            EventPayload::Sequencer(event),
            None,
        )
    }

    // =========================================================================
    // Consolidated Device State Access
    // =========================================================================

    /// Get device state summary for all connected devices
    /// This is the single source of truth for device state
    pub async fn get_all_device_states(&self) -> Vec<DeviceStateSummary> {
        let devices = self.devices.read().await;
        devices
            .values()
            .map(|conn| DeviceStateSummary {
                device_id: conn.info.id.clone(),
                device_type: conn.info.device_type,
                driver_type: conn.info.driver_type,
                name: conn.info.name.clone(),
                connection_state: conn.state,
            })
            .collect()
    }

    /// Get state for a specific device
    pub async fn get_device_state(&self, device_type: DeviceType, device_id: &str) -> Option<DeviceStateSummary> {
        let key = (device_type, device_id.to_string());
        let devices = self.devices.read().await;
        devices.get(&key).map(|conn| DeviceStateSummary {
            device_id: conn.info.id.clone(),
            device_type: conn.info.device_type,
            driver_type: conn.info.driver_type,
            name: conn.info.name.clone(),
            connection_state: conn.state,
        })
    }

    /// Get all devices of a specific type with their states
    pub async fn get_devices_by_type(&self, device_type: DeviceType) -> Vec<DeviceStateSummary> {
        let devices = self.devices.read().await;
        devices
            .iter()
            .filter(|((dt, _), _)| *dt == device_type)
            .map(|(_, conn)| DeviceStateSummary {
                device_id: conn.info.id.clone(),
                device_type: conn.info.device_type,
                driver_type: conn.info.driver_type,
                name: conn.info.name.clone(),
                connection_state: conn.state,
            })
            .collect()
    }

    /// Get the device ID from the current profile for a specific device type
    pub async fn get_profile_device_id(&self, device_type: DeviceType) -> Option<String> {
        let profile = self.profile.read().await;
        profile.as_ref().and_then(|p| {
            match device_type {
                DeviceType::Camera => p.camera_id.clone(),
                DeviceType::Mount => p.mount_id.clone(),
                DeviceType::Focuser => p.focuser_id.clone(),
                DeviceType::FilterWheel => p.filter_wheel_id.clone(),
                DeviceType::Rotator => p.rotator_id.clone(),
                DeviceType::Dome => p.dome_id.clone(),
                DeviceType::Weather => p.weather_id.clone(),
                DeviceType::CoverCalibrator => p.cover_calibrator_id.clone(),
                _ => None,
            }
        })
    }

    /// Check if a device from the profile is connected
    pub async fn is_profile_device_connected(&self, device_type: DeviceType) -> bool {
        if let Some(device_id) = self.get_profile_device_id(device_type).await {
            self.is_device_connected(device_type, &device_id).await
        } else {
            false
        }
    }
}

/// Summary of a device's current state
#[derive(Debug, Clone)]
pub struct DeviceStateSummary {
    pub device_id: String,
    pub device_type: DeviceType,
    pub driver_type: DriverType,
    pub name: String,
    pub connection_state: ConnectionState,
}

impl Default for AppState {
    fn default() -> Self {
        Self {
            event_bus: Arc::new(EventBus::new(crate::event::DEFAULT_EVENT_BUFFER_SIZE)),
            devices: RwLock::new(HashMap::new()),
            session: RwLock::new(SessionState::default()),
            profile: RwLock::new(None),
            observer_location: RwLock::new(None),
        }
    }
}

/// Thread-safe shared application state
pub type SharedAppState = Arc<AppState>;
