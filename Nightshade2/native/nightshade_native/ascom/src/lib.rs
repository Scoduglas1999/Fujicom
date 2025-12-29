//! ASCOM COM Interface (Windows Only)
//!
//! Provides real access to ASCOM devices via COM on Windows.
//! This module enables Nightshade to connect to actual astronomical
//! equipment through the ASCOM standard.

#[cfg(windows)]
mod windows_impl;

/// ASCOM device information discovered from Windows Registry
#[derive(Debug, Clone)]
pub struct AscomDevice {
    /// The COM ProgID used to instantiate the driver
    pub prog_id: String,
    /// Human-readable name
    pub name: String,
    /// Description from ASCOM profile
    pub description: String,
}

/// ASCOM device types as defined by ASCOM standard
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum AscomDeviceType {
    Camera,
    Telescope,
    Focuser,
    FilterWheel,
    Rotator,
    Dome,
    SafetyMonitor,
    ObservingConditions,
    Switch,
    CoverCalibrator,
}

impl AscomDeviceType {
    /// Get the registry key name for this device type
    pub fn registry_name(&self) -> &'static str {
        match self {
            AscomDeviceType::Camera => "Camera",
            AscomDeviceType::Telescope => "Telescope",
            AscomDeviceType::Focuser => "Focuser",
            AscomDeviceType::FilterWheel => "FilterWheel",
            AscomDeviceType::Rotator => "Rotator",
            AscomDeviceType::Dome => "Dome",
            AscomDeviceType::SafetyMonitor => "SafetyMonitor",
            AscomDeviceType::ObservingConditions => "ObservingConditions",
            AscomDeviceType::Switch => "Switch",
            AscomDeviceType::CoverCalibrator => "CoverCalibrator",
        }
    }
}

/// Discover ASCOM devices of a specific type
/// Returns a list of available drivers registered in the Windows Registry
#[cfg(windows)]
pub fn discover_devices(device_type: AscomDeviceType) -> Vec<AscomDevice> {
    windows_impl::discover_devices(device_type.registry_name())
}

/// Discover ASCOM devices (non-Windows stub)
#[cfg(not(windows))]
pub fn discover_devices(_device_type: AscomDeviceType) -> Vec<AscomDevice> {
    Vec::new()
}

/// Check if ASCOM is available on this platform
pub fn is_available() -> bool {
    cfg!(windows)
}

// Re-export Windows-specific types when on Windows
#[cfg(windows)]
pub use windows_impl::{
    // COM initialization
    init_com, uninit_com,
    // Device discovery
    probe_device_name,
    // Device connection wrapper
    AscomDeviceConnection,
    // Device types
    AscomCamera,
    AscomMount,
    AscomFocuser,
    AscomFilterWheel,
    AscomRotator,
    AscomDome,
    AscomSafetyMonitor,
    AscomObservingConditions,
    AscomSwitch,
    AscomCoverCalibrator,
    // Error types
    AscomError,
    AscomResult,
    // Configuration types
    TimeoutConfig,
    get_timeout_config,
    set_timeout_config,
    // Health monitoring
    ConnectionHealth,
    HealthMonitor,
    // Batch status types
    CameraThermalStatus,
    CameraSensorConfig,
    CameraExposureSettings,
    CameraFullStatus,
    // RAII guards for resource cleanup
    AscomOperationGuard,
    AscomCleanupGuard,
    AscomDisconnectable,
};
