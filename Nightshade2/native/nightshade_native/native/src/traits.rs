//! Native Driver Traits
//!
//! Defines the common interface that all native drivers must implement.
//! This is similar to NINA's ICameraDevice, IMountDevice, etc. interfaces.

use crate::camera::*;
use async_trait::async_trait;
use std::fmt::Debug;
use std::time::Duration;

// =============================================================================
// TIMEOUT CONFIGURATION
// =============================================================================

/// Configuration for operation timeouts in native SDK drivers.
///
/// SDK operations can hang indefinitely if hardware becomes unresponsive.
/// This struct provides configurable timeouts for different operation types
/// to prevent the caller from waiting forever.
///
/// # Default Values
/// The defaults are conservative to handle slow hardware:
/// - `exposure_poll_timeout`: Exposure duration + 60 seconds margin
/// - `image_download_timeout`: 120 seconds (large images over USB 2.0)
/// - `connect_timeout`: 30 seconds
/// - `property_timeout`: 10 seconds
/// - `focuser_move_timeout`: 300 seconds (5 minutes for slow focusers)
/// - `filterwheel_move_timeout`: 60 seconds
#[derive(Debug, Clone)]
pub struct NativeTimeoutConfig {
    /// Maximum time to wait for exposure completion polling.
    /// This should be set based on the actual exposure duration plus a margin.
    /// Default: 60 seconds (caller should set based on actual exposure).
    pub exposure_poll_timeout: Duration,

    /// Maximum time to wait for image download from camera.
    /// Long exposures and high-resolution cameras may need more time.
    /// Default: 120 seconds.
    pub image_download_timeout: Duration,

    /// Maximum time to wait for device connection.
    /// Default: 30 seconds.
    pub connect_timeout: Duration,

    /// Maximum time to wait for property get/set operations.
    /// These are typically fast but can hang on unresponsive hardware.
    /// Default: 10 seconds.
    pub property_timeout: Duration,

    /// Maximum time to wait for focuser move completion.
    /// Some focusers are very slow, especially long-travel models.
    /// Default: 300 seconds (5 minutes).
    pub focuser_move_timeout: Duration,

    /// Maximum time to wait for filter wheel move completion.
    /// Default: 60 seconds.
    pub filterwheel_move_timeout: Duration,

    /// Poll interval for checking operation completion.
    /// Shorter intervals are more responsive but use more CPU.
    /// Default: 100ms.
    pub poll_interval: Duration,
}

impl Default for NativeTimeoutConfig {
    fn default() -> Self {
        Self {
            exposure_poll_timeout: Duration::from_secs(60),
            image_download_timeout: Duration::from_secs(120),
            connect_timeout: Duration::from_secs(30),
            property_timeout: Duration::from_secs(10),
            focuser_move_timeout: Duration::from_secs(300),
            filterwheel_move_timeout: Duration::from_secs(60),
            poll_interval: Duration::from_millis(100),
        }
    }
}

impl NativeTimeoutConfig {
    /// Create a new timeout config with default values.
    pub fn new() -> Self {
        Self::default()
    }

    /// Create timeout config for a specific exposure duration.
    ///
    /// Sets the exposure poll timeout to the exposure duration plus a 60-second margin.
    /// This is the recommended way to configure timeouts for exposure operations.
    ///
    /// # Arguments
    /// * `exposure_secs` - The actual exposure duration in seconds.
    pub fn for_exposure(exposure_secs: f64) -> Self {
        let mut config = Self::default();
        // Add 60 seconds margin for readout time, plus the exposure duration
        let timeout_secs = (exposure_secs + 60.0).max(60.0);
        config.exposure_poll_timeout = Duration::from_secs_f64(timeout_secs);
        config
    }

    /// Create a strict timeout config for quick operations.
    /// Uses shorter timeouts suitable for fast cameras and good connections.
    pub fn strict() -> Self {
        Self {
            exposure_poll_timeout: Duration::from_secs(30),
            image_download_timeout: Duration::from_secs(30),
            connect_timeout: Duration::from_secs(10),
            property_timeout: Duration::from_secs(5),
            focuser_move_timeout: Duration::from_secs(120),
            filterwheel_move_timeout: Duration::from_secs(30),
            poll_interval: Duration::from_millis(50),
        }
    }

    /// Create a lenient timeout config for slow hardware.
    /// Uses longer timeouts suitable for USB 2.0 connections and slow devices.
    pub fn lenient() -> Self {
        Self {
            exposure_poll_timeout: Duration::from_secs(120),
            image_download_timeout: Duration::from_secs(300),
            connect_timeout: Duration::from_secs(60),
            property_timeout: Duration::from_secs(30),
            focuser_move_timeout: Duration::from_secs(600),
            filterwheel_move_timeout: Duration::from_secs(120),
            poll_interval: Duration::from_millis(200),
        }
    }

    /// Calculate the appropriate exposure poll timeout for a given exposure duration.
    /// Returns the exposure duration plus a margin, with a minimum of 30 seconds.
    pub fn calculate_exposure_timeout(&self, exposure_secs: f64) -> Duration {
        let margin_secs = 60.0; // 60 second margin for readout
        let min_timeout = 30.0;
        Duration::from_secs_f64((exposure_secs + margin_secs).max(min_timeout))
    }
}

/// Common interface for all native devices
#[async_trait]
pub trait NativeDevice: Send + Sync + Debug {
    /// Get the unique device ID
    fn id(&self) -> &str;
    
    /// Get the device name/model
    fn name(&self) -> &str;
    
    /// Get the vendor name
    fn vendor(&self) -> crate::NativeVendor;
    
    /// Check if the device is connected
    fn is_connected(&self) -> bool;
    
    /// Connect to the device
    async fn connect(&mut self) -> Result<(), NativeError>;
    
    /// Disconnect from the device
    async fn disconnect(&mut self) -> Result<(), NativeError>;
}

/// Native camera device interface
///
/// This trait provides all camera operations that native drivers must implement.
/// Similar to NINA's ICameraDevice interface.
#[async_trait]
pub trait NativeCamera: NativeDevice {
    /// Get camera capabilities
    fn capabilities(&self) -> CameraCapabilities;
    
    /// Get current camera status
    async fn get_status(&self) -> Result<CameraStatus, NativeError>;
    
    /// Start an exposure
    async fn start_exposure(&mut self, params: ExposureParams) -> Result<(), NativeError>;
    
    /// Abort current exposure
    async fn abort_exposure(&mut self) -> Result<(), NativeError>;
    
    /// Check if exposure is complete
    async fn is_exposure_complete(&self) -> Result<bool, NativeError>;
    
    /// Download the image data from the camera
    async fn download_image(&mut self) -> Result<ImageData, NativeError>;
    
    /// Set cooler state and target temperature
    async fn set_cooler(&mut self, enabled: bool, target_temp: f64) -> Result<(), NativeError>;
    
    /// Get current sensor temperature
    async fn get_temperature(&self) -> Result<f64, NativeError>;
    
    /// Get cooler power percentage
    async fn get_cooler_power(&self) -> Result<f64, NativeError>;
    
    /// Set gain
    async fn set_gain(&mut self, gain: i32) -> Result<(), NativeError>;
    
    /// Get current gain
    async fn get_gain(&self) -> Result<i32, NativeError>;
    
    /// Set offset
    async fn set_offset(&mut self, offset: i32) -> Result<(), NativeError>;
    
    /// Get current offset
    async fn get_offset(&self) -> Result<i32, NativeError>;
    
    /// Set binning
    async fn set_binning(&mut self, bin_x: i32, bin_y: i32) -> Result<(), NativeError>;
    
    /// Get current binning
    async fn get_binning(&self) -> Result<(i32, i32), NativeError>;
    
    /// Set subframe region
    async fn set_subframe(&mut self, subframe: Option<SubFrame>) -> Result<(), NativeError>;
    
    /// Get sensor information
    fn get_sensor_info(&self) -> SensorInfo;
    
    /// Get available readout modes (vendor-specific)
    async fn get_readout_modes(&self) -> Result<Vec<ReadoutMode>, NativeError>;
    
    /// Set readout mode (vendor-specific)
    async fn set_readout_mode(&mut self, mode: &ReadoutMode) -> Result<(), NativeError>;
    
    /// Get vendor-specific features (e.g., QHY sensor chamber readings)
    async fn get_vendor_features(&self) -> Result<VendorFeatures, NativeError>;

    /// Get the valid range for gain setting.
    ///
    /// Returns (min_gain, max_gain) tuple.
    /// If the camera does not support gain adjustment, returns Err(NotSupported).
    async fn get_gain_range(&self) -> Result<(i32, i32), NativeError>;

    /// Get the valid range for offset/black level setting.
    ///
    /// Returns (min_offset, max_offset) tuple.
    /// If the camera does not support offset adjustment, returns Err(NotSupported).
    async fn get_offset_range(&self) -> Result<(i32, i32), NativeError>;
}

/// Tracking rate for mount
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum TrackingRate {
    Sidereal = 0,
    Lunar = 1,
    Solar = 2,
    King = 3,
    Custom = 4,
}

/// Native mount/telescope device interface
#[async_trait]
pub trait NativeMount: NativeDevice {
    /// Slew to coordinates (RA in hours, Dec in degrees)
    async fn slew_to_coordinates(&mut self, ra_hours: f64, dec_degrees: f64) -> Result<(), NativeError>;

    /// Get current coordinates
    async fn get_coordinates(&self) -> Result<(f64, f64), NativeError>;

    /// Sync to coordinates
    async fn sync_to_coordinates(&mut self, ra_hours: f64, dec_degrees: f64) -> Result<(), NativeError>;

    /// Park the mount
    async fn park(&mut self) -> Result<(), NativeError>;

    /// Unpark the mount
    async fn unpark(&mut self) -> Result<(), NativeError>;

    /// Check if mount is slewing
    async fn is_slewing(&self) -> Result<bool, NativeError>;

    /// Check if mount is parked
    async fn is_parked(&self) -> Result<bool, NativeError>;

    /// Pulse guide (for autoguiding)
    async fn pulse_guide(&mut self, direction: GuideDirection, duration_ms: u32) -> Result<(), NativeError>;

    /// Abort current slew
    async fn abort_slew(&mut self) -> Result<(), NativeError>;

    /// Set tracking enabled/disabled
    async fn set_tracking(&mut self, enabled: bool) -> Result<(), NativeError>;

    /// Get tracking state
    async fn get_tracking(&self) -> Result<bool, NativeError>;

    /// Set tracking rate
    async fn set_tracking_rate(&mut self, rate: TrackingRate) -> Result<(), NativeError>;

    /// Get tracking rate
    async fn get_tracking_rate(&self) -> Result<TrackingRate, NativeError>;

    /// Check if mount supports setting tracking rate
    fn can_set_tracking_rate(&self) -> bool;

    /// Get side of pier
    async fn get_side_of_pier(&self) -> Result<PierSide, NativeError>;

    /// Get Alt/Az coordinates
    async fn get_alt_az(&self) -> Result<(f64, f64), NativeError>;

    /// Get local sidereal time
    async fn get_sidereal_time(&self) -> Result<f64, NativeError>;
}

/// Side of Pier
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum PierSide {
    East,
    West,
    Unknown,
}

/// Native focuser device interface
#[async_trait]
pub trait NativeFocuser: NativeDevice {
    /// Move to absolute position
    async fn move_to(&mut self, position: i32) -> Result<(), NativeError>;
    
    /// Move relative by steps
    async fn move_relative(&mut self, steps: i32) -> Result<(), NativeError>;
    
    /// Get current position
    async fn get_position(&self) -> Result<i32, NativeError>;
    
    /// Check if focuser is moving
    async fn is_moving(&self) -> Result<bool, NativeError>;
    
    /// Halt movement
    async fn halt(&mut self) -> Result<(), NativeError>;
    
    /// Get temperature (if available)
    async fn get_temperature(&self) -> Result<Option<f64>, NativeError>;
    
    /// Get maximum position
    fn get_max_position(&self) -> i32;
    
    /// Get step size
    fn get_step_size(&self) -> f64;
}

/// Native filter wheel device interface
#[async_trait]
pub trait NativeFilterWheel: NativeDevice {
    /// Move to filter position (0-indexed)
    async fn move_to_position(&mut self, position: i32) -> Result<(), NativeError>;
    
    /// Get current filter position
    async fn get_position(&self) -> Result<i32, NativeError>;
    
    /// Check if filter wheel is moving
    async fn is_moving(&self) -> Result<bool, NativeError>;
    
    /// Get number of filter slots
    fn get_filter_count(&self) -> i32;
    
    /// Get filter names
    async fn get_filter_names(&self) -> Result<Vec<String>, NativeError>;
    
    /// Set filter name
    async fn set_filter_name(&mut self, position: i32, name: String) -> Result<(), NativeError>;
}

/// Error type for native driver operations
#[derive(Debug, thiserror::Error)]
pub enum NativeError {
    #[error("Device not connected")]
    NotConnected,

    #[error("Device disconnected")]
    Disconnected,

    #[error("Device not found: {0}")]
    DeviceNotFound(String),

    #[error("Invalid device: {0}")]
    InvalidDevice(String),

    #[error("SDK error: {0}")]
    SdkError(String),

    #[error("SDK not loaded")]
    SdkNotLoaded,

    #[error("Invalid parameter: {0}")]
    InvalidParameter(String),

    #[error("Operation not supported")]
    NotSupported,

    /// Simple timeout error with a description message.
    /// Used for backward compatibility with existing code.
    #[error("Operation timeout: {0}")]
    Timeout(String),

    /// Detailed timeout error with operation context and duration.
    /// Provides specific information about what operation timed out and how long
    /// the system waited before giving up.
    #[error("Timeout: {operation} did not complete within {duration:?}")]
    OperationTimeout {
        /// Description of the operation that timed out
        operation: String,
        /// How long we waited before timing out
        duration: Duration,
        /// Additional context about the timeout (e.g., device state)
        context: Option<String>,
    },

    /// Exposure polling timeout.
    /// The exposure completion check exceeded the maximum allowed polling time.
    #[error("Exposure polling timeout after {duration:?}: {details}")]
    ExposureTimeout {
        /// How long we polled before timing out
        duration: Duration,
        /// Details about the exposure (e.g., expected duration)
        details: String,
    },

    /// Image download timeout.
    /// The camera took too long to transfer image data.
    #[error("Image download timeout after {duration:?}: {details}")]
    DownloadTimeout {
        /// How long we waited before timing out
        duration: Duration,
        /// Details about the download (e.g., image size, transfer rate)
        details: String,
    },

    /// Device connection timeout.
    /// The device did not respond to connection attempts in time.
    #[error("Connection timeout after {duration:?}: {device}")]
    ConnectionTimeout {
        /// How long we waited before timing out
        duration: Duration,
        /// Device identifier or description
        device: String,
    },

    /// Move operation timeout (for focuser, filter wheel, mount, etc.)
    /// The device did not complete its move operation in time.
    #[error("Move timeout after {duration:?}: {details}")]
    MoveTimeout {
        /// How long we waited before timing out
        duration: Duration,
        /// Details about the move (e.g., target position, device type)
        details: String,
    },

    #[error("IO error: {0}")]
    Io(#[from] std::io::Error),

    #[error("Unknown error: {0}")]
    Unknown(String),
}

impl NativeError {
    /// Create an operation timeout error with full context.
    pub fn operation_timeout(operation: impl Into<String>, duration: Duration) -> Self {
        Self::OperationTimeout {
            operation: operation.into(),
            duration,
            context: None,
        }
    }

    /// Create an operation timeout error with additional context.
    pub fn operation_timeout_with_context(
        operation: impl Into<String>,
        duration: Duration,
        context: impl Into<String>,
    ) -> Self {
        Self::OperationTimeout {
            operation: operation.into(),
            duration,
            context: Some(context.into()),
        }
    }

    /// Create an exposure polling timeout error.
    pub fn exposure_timeout(duration: Duration, expected_exposure: f64) -> Self {
        Self::ExposureTimeout {
            duration,
            details: format!(
                "expected exposure was {:.1}s, waited {:.1}s",
                expected_exposure,
                duration.as_secs_f64()
            ),
        }
    }

    /// Create an image download timeout error.
    pub fn download_timeout(duration: Duration, width: u32, height: u32) -> Self {
        Self::DownloadTimeout {
            duration,
            details: format!(
                "downloading {}x{} image ({:.1}MP)",
                width,
                height,
                (width as f64 * height as f64) / 1_000_000.0
            ),
        }
    }

    /// Create a connection timeout error.
    pub fn connection_timeout(duration: Duration, device: impl Into<String>) -> Self {
        Self::ConnectionTimeout {
            duration,
            device: device.into(),
        }
    }

    /// Create a focuser move timeout error.
    pub fn focuser_move_timeout(duration: Duration, target_position: i32) -> Self {
        Self::MoveTimeout {
            duration,
            details: format!("focuser moving to position {}", target_position),
        }
    }

    /// Create a filter wheel move timeout error.
    pub fn filterwheel_move_timeout(duration: Duration, target_position: i32) -> Self {
        Self::MoveTimeout {
            duration,
            details: format!("filter wheel moving to slot {}", target_position),
        }
    }

    /// Check if this error is any type of timeout error.
    pub fn is_timeout(&self) -> bool {
        matches!(
            self,
            Self::Timeout(_)
                | Self::OperationTimeout { .. }
                | Self::ExposureTimeout { .. }
                | Self::DownloadTimeout { .. }
                | Self::ConnectionTimeout { .. }
                | Self::MoveTimeout { .. }
        )
    }

    /// Get the duration if this is a timeout error, otherwise None.
    pub fn timeout_duration(&self) -> Option<Duration> {
        match self {
            Self::OperationTimeout { duration, .. } => Some(*duration),
            Self::ExposureTimeout { duration, .. } => Some(*duration),
            Self::DownloadTimeout { duration, .. } => Some(*duration),
            Self::ConnectionTimeout { duration, .. } => Some(*duration),
            Self::MoveTimeout { duration, .. } => Some(*duration),
            _ => None,
        }
    }
}

/// Guide direction for mount pulse guiding
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum GuideDirection {
    North,
    South,
    East,
    West,
}

/// Native rotator device interface
#[async_trait]
pub trait NativeRotator: NativeDevice {
    /// Move to absolute position (in degrees)
    async fn move_to(&mut self, position: f64) -> Result<(), NativeError>;

    /// Get current position (in degrees)
    async fn get_position(&self) -> Result<f64, NativeError>;

    /// Get mechanical position (in degrees)
    async fn get_mechanical_position(&self) -> Result<f64, NativeError>;

    /// Check if rotator is moving
    async fn is_moving(&self) -> Result<bool, NativeError>;

    /// Halt movement
    async fn halt(&mut self) -> Result<(), NativeError>;

    /// Check if reverse is supported
    fn can_reverse(&self) -> bool;

    /// Set reverse mode
    async fn set_reverse(&mut self, reverse: bool) -> Result<(), NativeError>;

    /// Get reverse mode
    async fn get_reverse(&self) -> Result<bool, NativeError>;
}

/// Shutter state for dome
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ShutterState {
    Open,
    Closed,
    Opening,
    Closing,
    Error,
    Unknown,
}

/// Native dome device interface
#[async_trait]
pub trait NativeDome: NativeDevice {
    /// Slew to azimuth (in degrees)
    async fn slew_to_azimuth(&mut self, azimuth: f64) -> Result<(), NativeError>;

    /// Get current azimuth (in degrees)
    async fn get_azimuth(&self) -> Result<f64, NativeError>;

    /// Open the shutter
    async fn open_shutter(&mut self) -> Result<(), NativeError>;

    /// Close the shutter
    async fn close_shutter(&mut self) -> Result<(), NativeError>;

    /// Get shutter status
    async fn get_shutter_status(&self) -> Result<ShutterState, NativeError>;

    /// Check if dome is slewing
    async fn is_slewing(&self) -> Result<bool, NativeError>;

    /// Abort slew
    async fn abort_slew(&mut self) -> Result<(), NativeError>;

    /// Park the dome
    async fn park(&mut self) -> Result<(), NativeError>;

    /// Check if parked
    async fn is_parked(&self) -> Result<bool, NativeError>;

    /// Check if at home position
    async fn is_at_home(&self) -> Result<bool, NativeError>;

    /// Find home position
    async fn find_home(&mut self) -> Result<(), NativeError>;

    /// Set slave mode (dome follows mount)
    async fn set_slaved(&mut self, slaved: bool) -> Result<(), NativeError>;

    /// Check if slaved
    async fn is_slaved(&self) -> Result<bool, NativeError>;

    /// Check capabilities
    fn can_set_azimuth(&self) -> bool;
    fn can_set_shutter(&self) -> bool;
    fn can_slave(&self) -> bool;
    fn can_set_altitude(&self) -> bool;

    /// Set altitude (if supported)
    async fn set_altitude(&mut self, altitude: f64) -> Result<(), NativeError>;

    /// Get altitude (if supported)
    async fn get_altitude(&self) -> Result<Option<f64>, NativeError>;
}

/// Native weather station device interface
#[async_trait]
pub trait NativeWeather: NativeDevice {
    /// Get temperature in Celsius
    async fn get_temperature(&self) -> Result<Option<f64>, NativeError>;

    /// Get humidity in percent (0-100)
    async fn get_humidity(&self) -> Result<Option<f64>, NativeError>;

    /// Get barometric pressure in hPa
    async fn get_pressure(&self) -> Result<Option<f64>, NativeError>;

    /// Get dew point in Celsius
    async fn get_dew_point(&self) -> Result<Option<f64>, NativeError>;

    /// Get wind speed in m/s
    async fn get_wind_speed(&self) -> Result<Option<f64>, NativeError>;

    /// Get wind direction in degrees (0-360)
    async fn get_wind_direction(&self) -> Result<Option<f64>, NativeError>;

    /// Get cloud cover in percent (0-100)
    async fn get_cloud_cover(&self) -> Result<Option<f64>, NativeError>;

    /// Get sky quality in mag/arcsec²
    async fn get_sky_quality(&self) -> Result<Option<f64>, NativeError>;

    /// Get sky brightness in lux
    async fn get_sky_brightness(&self) -> Result<Option<f64>, NativeError>;

    /// Get rain rate in mm/hr
    async fn get_rain_rate(&self) -> Result<Option<f64>, NativeError>;

    /// Check if conditions are safe for observing
    async fn is_safe(&self) -> Result<bool, NativeError>;
}

/// Native safety monitor device interface
#[async_trait]
pub trait NativeSafetyMonitor: NativeDevice {
    /// Check if conditions are safe
    async fn is_safe(&self) -> Result<bool, NativeError>;
}





