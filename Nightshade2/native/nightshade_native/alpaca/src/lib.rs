//! Alpaca Protocol Client
//!
//! Implements the ASCOM Alpaca REST API for cross-platform device control.
//! This provides network-based access to astronomical equipment without
//! requiring platform-specific COM or INDI interfaces.

mod client;
mod discovery;
mod camera;
mod telescope;
mod focuser;
mod filterwheel;
mod rotator;
mod dome;
mod safetymonitor;
mod observingconditions;
mod switch;
mod covercalibrator;
mod guard;

pub use client::*;
pub use guard::*;
pub use discovery::*;
pub use camera::*;
pub use telescope::*;
pub use focuser::*;
pub use filterwheel::*;
pub use rotator::*;
pub use dome::*;
pub use safetymonitor::*;
pub use observingconditions::*;
pub use switch::*;
pub use covercalibrator::*;

/// Alpaca API version
pub const ALPACA_API_VERSION: u32 = 1;

/// Default Alpaca discovery port
pub const ALPACA_DISCOVERY_PORT: u16 = 32227;

/// Default Alpaca API port
pub const ALPACA_DEFAULT_PORT: u16 = 11111;

/// Alpaca device types
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum AlpacaDeviceType {
    Camera,
    CoverCalibrator,
    Dome,
    FilterWheel,
    Focuser,
    ObservingConditions,
    Rotator,
    SafetyMonitor,
    Switch,
    Telescope,
}

impl AlpacaDeviceType {
    pub fn as_str(&self) -> &'static str {
        match self {
            AlpacaDeviceType::Camera => "camera",
            AlpacaDeviceType::CoverCalibrator => "covercalibrator",
            AlpacaDeviceType::Dome => "dome",
            AlpacaDeviceType::FilterWheel => "filterwheel",
            AlpacaDeviceType::Focuser => "focuser",
            AlpacaDeviceType::ObservingConditions => "observingconditions",
            AlpacaDeviceType::Rotator => "rotator",
            AlpacaDeviceType::SafetyMonitor => "safetymonitor",
            AlpacaDeviceType::Switch => "switch",
            AlpacaDeviceType::Telescope => "telescope",
        }
    }

    pub fn from_str(s: &str) -> Option<Self> {
        match s.to_lowercase().as_str() {
            "camera" => Some(AlpacaDeviceType::Camera),
            "covercalibrator" => Some(AlpacaDeviceType::CoverCalibrator),
            "dome" => Some(AlpacaDeviceType::Dome),
            "filterwheel" => Some(AlpacaDeviceType::FilterWheel),
            "focuser" => Some(AlpacaDeviceType::Focuser),
            "observingconditions" => Some(AlpacaDeviceType::ObservingConditions),
            "rotator" => Some(AlpacaDeviceType::Rotator),
            "safetymonitor" => Some(AlpacaDeviceType::SafetyMonitor),
            "switch" => Some(AlpacaDeviceType::Switch),
            "telescope" => Some(AlpacaDeviceType::Telescope),
            _ => None,
        }
    }

    /// Get a display name for the device type
    pub fn display_name(&self) -> &'static str {
        match self {
            AlpacaDeviceType::Camera => "Camera",
            AlpacaDeviceType::CoverCalibrator => "Cover Calibrator",
            AlpacaDeviceType::Dome => "Dome",
            AlpacaDeviceType::FilterWheel => "Filter Wheel",
            AlpacaDeviceType::Focuser => "Focuser",
            AlpacaDeviceType::ObservingConditions => "Observing Conditions",
            AlpacaDeviceType::Rotator => "Rotator",
            AlpacaDeviceType::SafetyMonitor => "Safety Monitor",
            AlpacaDeviceType::Switch => "Switch",
            AlpacaDeviceType::Telescope => "Telescope",
        }
    }
}

impl std::fmt::Display for AlpacaDeviceType {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.display_name())
    }
}

/// An Alpaca device discovered on the network
#[derive(Debug, Clone, PartialEq, Eq, Hash)]
pub struct AlpacaDevice {
    pub device_type: AlpacaDeviceType,
    pub device_number: u32,
    pub server_name: String,
    pub manufacturer: String,
    pub device_name: String,
    pub unique_id: String,
    pub base_url: String,
}

impl AlpacaDevice {
    /// Get a unique identifier for this device
    pub fn id(&self) -> String {
        format!("alpaca:{}:{}:{}", self.base_url, self.device_type.as_str(), self.device_number)
    }

    /// Get a display name combining device name and type
    pub fn display_name(&self) -> String {
        if self.device_name.is_empty() {
            format!("{} #{}", self.device_type.display_name(), self.device_number)
        } else {
            self.device_name.clone()
        }
    }
}

impl std::fmt::Display for AlpacaDevice {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{} at {}", self.display_name(), self.base_url)
    }
}
