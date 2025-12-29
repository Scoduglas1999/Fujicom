//! Error types for the Nightshade bridge
//!
//! This module provides comprehensive error types for the FFI bridge layer.
//! All errors are designed to be:
//! - Informative: Include context about what failed
//! - Recoverable: Where possible, indicate how to recover
//! - Safe: Never panic - this is the FFI boundary
//!
//! # Error Categories
//!
//! - Device errors: Connection, communication, timeout issues
//! - Driver errors: ASCOM, Alpaca, INDI, Native SDK specific errors
//! - Operation errors: Invalid parameters, unsupported operations
//! - System errors: I/O, internal failures

use thiserror::Error;
use std::time::Duration;

/// Main error type for the Nightshade native library
///
/// This enum covers all error cases that can occur at the FFI boundary.
/// Each variant includes enough context for meaningful error messages
/// and potential recovery actions.
#[derive(Error, Debug, Clone)]
pub enum NightshadeError {
    // =========================================================================
    // Device Discovery & Connection Errors
    // =========================================================================

    #[error("Device not found: {0}")]
    DeviceNotFound(String),

    #[error("Device connection failed: {device_id} - {reason}")]
    ConnectionFailed {
        device_id: String,
        reason: String,
    },

    #[error("Device already connected: {0}")]
    AlreadyConnected(String),

    #[error("Device not connected: {0}")]
    NotConnected(String),

    #[error("Device disconnected unexpectedly: {device_id} - {reason}")]
    DeviceDisconnected {
        device_id: String,
        reason: String,
    },

    // =========================================================================
    // Hardware Errors
    // =========================================================================

    /// General hardware error from device
    #[error("Hardware error: {device_id} - {message}")]
    HardwareError {
        device_id: String,
        message: String,
        /// Optional vendor-specific error code
        error_code: Option<i32>,
    },

    /// Hardware communication error
    #[error("Communication error: {device_id} - {message}")]
    CommunicationError {
        device_id: String,
        message: String,
    },

    // =========================================================================
    // Timeout Errors
    // =========================================================================

    /// Generic timeout error (for backwards compatibility)
    #[error("Operation timed out: {0}")]
    Timeout(String),

    /// Device-specific timeout with details
    #[error("Device timeout: {device_id} operation '{operation}' after {timeout_secs:.1}s")]
    DeviceTimeout {
        device_id: String,
        operation: String,
        timeout_secs: f64,
    },

    /// Connection timeout
    #[error("Connection timeout: {device_id} after {timeout_secs:.1}s")]
    ConnectionTimeout {
        device_id: String,
        timeout_secs: f64,
    },

    // =========================================================================
    // Parameter Validation Errors
    // =========================================================================

    #[error("Invalid parameter: {0}")]
    InvalidParameter(String),

    #[error("Invalid input: {0}")]
    InvalidInput(String),

    #[error("Invalid device ID: {device_id} - {reason}")]
    InvalidDeviceId {
        device_id: String,
        reason: String,
    },

    #[error("Parameter out of range: {param_name} = {value} (valid: {min} to {max})")]
    ParameterOutOfRange {
        param_name: String,
        value: String,
        min: String,
        max: String,
    },

    // =========================================================================
    // Operation Errors
    // =========================================================================

    #[error("Operation failed: {0}")]
    OperationFailed(String),

    #[error("Operation not supported: {operation} on device {device_id}")]
    NotSupported {
        device_id: String,
        operation: String,
    },

    #[error("Device busy: {device_id} - {current_operation}")]
    DeviceBusy {
        device_id: String,
        current_operation: String,
    },

    // =========================================================================
    // Imaging Errors
    // =========================================================================

    #[error("Image processing error: {0}")]
    ImageError(String),

    #[error("Camera error: {0}")]
    CameraError(String),

    #[error("No image available")]
    NoImageAvailable,

    #[error("Exposure cancelled")]
    ExposureCancelled,

    #[error("Exposure failed: {camera_id} - {reason}")]
    ExposureFailed {
        camera_id: String,
        reason: String,
    },

    #[error("Image download failed: {camera_id} - {reason}")]
    DownloadFailed {
        camera_id: String,
        reason: String,
    },

    // =========================================================================
    // I/O Errors
    // =========================================================================

    #[error("File I/O error: {0}")]
    IoError(String),

    #[error("Plate solving failed: {0}")]
    PlateSolveError(String),

    // =========================================================================
    // Sequence Errors
    // =========================================================================

    #[error("Sequence error: {0}")]
    SequenceError(String),

    // =========================================================================
    // Driver-Specific Errors
    // =========================================================================

    /// ASCOM driver error (Windows only)
    #[error("ASCOM error: {prog_id} - {message} (code: {error_code})")]
    AscomError {
        prog_id: String,
        message: String,
        error_code: i32,
    },

    /// Alpaca REST API error
    #[error("Alpaca error: {base_url} device {device_number} - {message} (code: {error_code})")]
    AlpacaError {
        base_url: String,
        device_number: u32,
        message: String,
        error_code: i32,
    },

    /// INDI protocol error
    #[error("INDI error: {server}:{port} device '{device_name}' - {message}")]
    IndiError {
        server: String,
        port: u16,
        device_name: String,
        message: String,
    },

    /// Native SDK error
    #[error("Native SDK error: {vendor} - {message} (code: {error_code})")]
    NativeError {
        vendor: String,
        message: String,
        error_code: i32,
    },

    /// COM/OLE error (Windows ASCOM)
    #[error("COM error: {message} (HRESULT: 0x{hresult:08X})")]
    ComError {
        message: String,
        hresult: u32,
    },

    // =========================================================================
    // System Errors
    // =========================================================================

    #[error("Internal error: {0}")]
    Internal(String),

    #[error("Operation cancelled")]
    Cancelled,

    #[error("Runtime initialization failed: {0}")]
    RuntimeInitFailed(String),

    #[error("Resource exhausted: {resource} - {message}")]
    ResourceExhausted {
        resource: String,
        message: String,
    },
}

impl NightshadeError {
    // =========================================================================
    // Constructor Helpers
    // =========================================================================

    /// Create a device timeout error
    pub fn device_timeout(device_id: impl Into<String>, operation: impl Into<String>, timeout: Duration) -> Self {
        NightshadeError::DeviceTimeout {
            device_id: device_id.into(),
            operation: operation.into(),
            timeout_secs: timeout.as_secs_f64(),
        }
    }

    /// Create a connection failed error
    pub fn connection_failed(device_id: impl Into<String>, reason: impl Into<String>) -> Self {
        NightshadeError::ConnectionFailed {
            device_id: device_id.into(),
            reason: reason.into(),
        }
    }

    /// Create an invalid device ID error
    pub fn invalid_device_id(device_id: impl Into<String>, reason: impl Into<String>) -> Self {
        NightshadeError::InvalidDeviceId {
            device_id: device_id.into(),
            reason: reason.into(),
        }
    }

    /// Create a not supported error
    pub fn not_supported(device_id: impl Into<String>, operation: impl Into<String>) -> Self {
        NightshadeError::NotSupported {
            device_id: device_id.into(),
            operation: operation.into(),
        }
    }

    /// Create an ASCOM error
    #[cfg(windows)]
    pub fn ascom_error(prog_id: impl Into<String>, message: impl Into<String>, error_code: i32) -> Self {
        NightshadeError::AscomError {
            prog_id: prog_id.into(),
            message: message.into(),
            error_code,
        }
    }

    /// Create an Alpaca error
    pub fn alpaca_error(base_url: impl Into<String>, device_number: u32, message: impl Into<String>, error_code: i32) -> Self {
        NightshadeError::AlpacaError {
            base_url: base_url.into(),
            device_number,
            message: message.into(),
            error_code,
        }
    }

    /// Create an INDI error
    pub fn indi_error(server: impl Into<String>, port: u16, device_name: impl Into<String>, message: impl Into<String>) -> Self {
        NightshadeError::IndiError {
            server: server.into(),
            port,
            device_name: device_name.into(),
            message: message.into(),
        }
    }

    /// Create a native SDK error
    pub fn native_error(vendor: impl Into<String>, message: impl Into<String>, error_code: i32) -> Self {
        NightshadeError::NativeError {
            vendor: vendor.into(),
            message: message.into(),
            error_code,
        }
    }

    /// Create a hardware error
    pub fn hardware_error(device_id: impl Into<String>, message: impl Into<String>) -> Self {
        NightshadeError::HardwareError {
            device_id: device_id.into(),
            message: message.into(),
            error_code: None,
        }
    }

    /// Create a hardware error with vendor-specific error code
    pub fn hardware_error_with_code(device_id: impl Into<String>, message: impl Into<String>, error_code: i32) -> Self {
        NightshadeError::HardwareError {
            device_id: device_id.into(),
            message: message.into(),
            error_code: Some(error_code),
        }
    }

    /// Create a communication error
    pub fn communication_error(device_id: impl Into<String>, message: impl Into<String>) -> Self {
        NightshadeError::CommunicationError {
            device_id: device_id.into(),
            message: message.into(),
        }
    }

    /// Create a disconnected error
    pub fn device_disconnected(device_id: impl Into<String>, reason: impl Into<String>) -> Self {
        NightshadeError::DeviceDisconnected {
            device_id: device_id.into(),
            reason: reason.into(),
        }
    }

    /// Create an operation not supported error
    pub fn operation_not_supported(device_id: impl Into<String>, operation: impl Into<String>) -> Self {
        NightshadeError::NotSupported {
            device_id: device_id.into(),
            operation: operation.into(),
        }
    }

    /// Create a parameter out of range error
    pub fn parameter_out_of_range<T: std::fmt::Display>(
        param_name: impl Into<String>,
        value: T,
        min: T,
        max: T,
    ) -> Self {
        NightshadeError::ParameterOutOfRange {
            param_name: param_name.into(),
            value: value.to_string(),
            min: min.to_string(),
            max: max.to_string(),
        }
    }

    // =========================================================================
    // Error Classification
    // =========================================================================

    /// Returns true if this error is recoverable by retrying the same operation.
    ///
    /// Recoverable errors include:
    /// - Timeouts (operation may succeed with more time or on retry)
    /// - Device busy (wait and retry)
    /// - Connection errors that could be transient
    ///
    /// Non-recoverable errors include:
    /// - Invalid parameters (same input will always fail)
    /// - Unsupported operations (device doesn't support this)
    /// - Permanent hardware failures
    pub fn is_recoverable(&self) -> bool {
        matches!(self,
            // Timeout errors - operation may succeed on retry
            NightshadeError::Timeout(_) |
            NightshadeError::DeviceTimeout { .. } |
            NightshadeError::ConnectionTimeout { .. } |
            // Device busy - wait and retry
            NightshadeError::DeviceBusy { .. } |
            // Connection issues - may be transient
            NightshadeError::ConnectionFailed { .. } |
            NightshadeError::DeviceDisconnected { .. } |
            NightshadeError::CommunicationError { .. } |
            // Some hardware errors may be transient
            NightshadeError::HardwareError { .. } |
            // Resource exhausted - may clear up
            NightshadeError::ResourceExhausted { .. }
        )
    }

    /// Returns true if this error is recoverable by retrying
    /// (Alias for backward compatibility, use `is_recoverable()` for new code)
    pub fn is_retryable(&self) -> bool {
        self.is_recoverable()
    }

    /// Returns true if this error is a timeout
    ///
    /// Use this to decide whether to:
    /// - Increase timeout and retry
    /// - Prompt user to wait longer
    /// - Check if device is responsive
    pub fn is_timeout(&self) -> bool {
        matches!(self,
            NightshadeError::Timeout(_) |
            NightshadeError::DeviceTimeout { .. } |
            NightshadeError::ConnectionTimeout { .. }
        )
    }

    /// Returns true if this error indicates the device needs reconnection
    ///
    /// Use this to automatically attempt reconnection before retrying the operation.
    pub fn needs_reconnect(&self) -> bool {
        matches!(self,
            NightshadeError::NotConnected(_) |
            NightshadeError::DeviceDisconnected { .. } |
            NightshadeError::CommunicationError { .. } |
            NightshadeError::ComError { .. }
        )
    }

    /// Returns true if we should try to reconnect to the device
    /// (Alias for `needs_reconnect()` for API clarity)
    pub fn should_reconnect(&self) -> bool {
        self.needs_reconnect()
    }

    /// Returns true if this is a hardware-level error
    ///
    /// Hardware errors typically require user attention:
    /// - Check physical connections
    /// - Restart device
    /// - Check device health/status
    pub fn is_hardware_error(&self) -> bool {
        matches!(self,
            NightshadeError::HardwareError { .. } |
            NightshadeError::CommunicationError { .. } |
            NightshadeError::ExposureFailed { .. } |
            NightshadeError::DownloadFailed { .. }
        )
    }

    /// Returns true if this error indicates the operation is not supported
    ///
    /// Use this to disable UI elements or avoid retrying operations
    /// that will never succeed on this device.
    pub fn is_not_supported(&self) -> bool {
        matches!(self, NightshadeError::NotSupported { .. })
    }

    /// Returns true if this error is due to invalid user input
    ///
    /// Use this to prompt user to correct their input.
    pub fn is_invalid_input(&self) -> bool {
        matches!(self,
            NightshadeError::InvalidParameter(_) |
            NightshadeError::InvalidInput(_) |
            NightshadeError::InvalidDeviceId { .. } |
            NightshadeError::ParameterOutOfRange { .. }
        )
    }

    /// Returns true if this is a user-initiated cancellation (not an error)
    pub fn is_cancellation(&self) -> bool {
        matches!(self,
            NightshadeError::Cancelled |
            NightshadeError::ExposureCancelled
        )
    }

    /// Get the device ID if this error is device-related
    pub fn device_id(&self) -> Option<&str> {
        match self {
            NightshadeError::DeviceNotFound(id) => Some(id),
            NightshadeError::ConnectionFailed { device_id, .. } => Some(device_id),
            NightshadeError::AlreadyConnected(id) => Some(id),
            NightshadeError::NotConnected(id) => Some(id),
            NightshadeError::DeviceDisconnected { device_id, .. } => Some(device_id),
            NightshadeError::HardwareError { device_id, .. } => Some(device_id),
            NightshadeError::CommunicationError { device_id, .. } => Some(device_id),
            NightshadeError::DeviceTimeout { device_id, .. } => Some(device_id),
            NightshadeError::ConnectionTimeout { device_id, .. } => Some(device_id),
            NightshadeError::InvalidDeviceId { device_id, .. } => Some(device_id),
            NightshadeError::NotSupported { device_id, .. } => Some(device_id),
            NightshadeError::DeviceBusy { device_id, .. } => Some(device_id),
            NightshadeError::ExposureFailed { camera_id, .. } => Some(camera_id),
            NightshadeError::DownloadFailed { camera_id, .. } => Some(camera_id),
            _ => None,
        }
    }

    /// Get the vendor-specific error code if available
    ///
    /// Returns error codes from ASCOM, Alpaca, INDI, native SDKs, etc.
    pub fn error_code(&self) -> Option<i32> {
        match self {
            NightshadeError::HardwareError { error_code, .. } => *error_code,
            NightshadeError::AscomError { error_code, .. } => Some(*error_code),
            NightshadeError::AlpacaError { error_code, .. } => Some(*error_code),
            NightshadeError::NativeError { error_code, .. } => Some(*error_code),
            NightshadeError::ComError { hresult, .. } => Some(*hresult as i32),
            _ => None,
        }
    }

    /// Get a user-friendly error message suitable for display in UI
    ///
    /// This provides a shorter, less technical message than the full error.
    pub fn user_message(&self) -> String {
        match self {
            NightshadeError::DeviceNotFound(id) => format!("Device '{}' not found", id),
            NightshadeError::ConnectionFailed { device_id, .. } => {
                format!("Could not connect to '{}'", device_id)
            }
            NightshadeError::NotConnected(id) => format!("'{}' is not connected", id),
            NightshadeError::DeviceDisconnected { device_id, .. } => {
                format!("'{}' disconnected unexpectedly", device_id)
            }
            NightshadeError::HardwareError { device_id, message, .. } => {
                format!("Hardware error on '{}': {}", device_id, message)
            }
            NightshadeError::Timeout(msg) | NightshadeError::DeviceTimeout { operation: msg, .. } => {
                format!("Operation timed out: {}", msg)
            }
            NightshadeError::ConnectionTimeout { device_id, timeout_secs } => {
                format!("Connection to '{}' timed out after {:.0}s", device_id, timeout_secs)
            }
            NightshadeError::NotSupported { device_id, operation } => {
                format!("'{}' does not support: {}", device_id, operation)
            }
            NightshadeError::InvalidParameter(msg) | NightshadeError::InvalidInput(msg) => {
                format!("Invalid input: {}", msg)
            }
            NightshadeError::ParameterOutOfRange { param_name, value, min, max } => {
                format!("{} value {} is out of range ({} to {})", param_name, value, min, max)
            }
            NightshadeError::ExposureCancelled | NightshadeError::Cancelled => {
                "Operation cancelled".to_string()
            }
            NightshadeError::DeviceBusy { device_id, current_operation } => {
                format!("'{}' is busy: {}", device_id, current_operation)
            }
            // For other errors, use the Display implementation
            other => other.to_string(),
        }
    }

    /// Get a classification of the error for telemetry/logging
    pub fn error_category(&self) -> &'static str {
        match self {
            NightshadeError::DeviceNotFound(_) |
            NightshadeError::ConnectionFailed { .. } |
            NightshadeError::AlreadyConnected(_) |
            NightshadeError::NotConnected(_) |
            NightshadeError::DeviceDisconnected { .. } => "connection",

            NightshadeError::HardwareError { .. } |
            NightshadeError::CommunicationError { .. } => "hardware",

            NightshadeError::Timeout(_) |
            NightshadeError::DeviceTimeout { .. } |
            NightshadeError::ConnectionTimeout { .. } => "timeout",

            NightshadeError::InvalidParameter(_) |
            NightshadeError::InvalidInput(_) |
            NightshadeError::InvalidDeviceId { .. } |
            NightshadeError::ParameterOutOfRange { .. } => "validation",

            NightshadeError::NotSupported { .. } => "unsupported",

            NightshadeError::DeviceBusy { .. } => "busy",

            NightshadeError::ImageError(_) |
            NightshadeError::CameraError(_) |
            NightshadeError::NoImageAvailable |
            NightshadeError::ExposureCancelled |
            NightshadeError::ExposureFailed { .. } |
            NightshadeError::DownloadFailed { .. } => "imaging",

            NightshadeError::IoError(_) |
            NightshadeError::PlateSolveError(_) => "io",

            NightshadeError::SequenceError(_) => "sequence",

            NightshadeError::AscomError { .. } |
            NightshadeError::AlpacaError { .. } |
            NightshadeError::IndiError { .. } |
            NightshadeError::NativeError { .. } |
            NightshadeError::ComError { .. } => "driver",

            NightshadeError::Internal(_) |
            NightshadeError::Cancelled |
            NightshadeError::RuntimeInitFailed(_) |
            NightshadeError::ResourceExhausted { .. } |
            NightshadeError::OperationFailed(_) => "system",
        }
    }
}

// =========================================================================
// From Implementations for Standard Library Types
// =========================================================================

impl From<std::io::Error> for NightshadeError {
    fn from(e: std::io::Error) -> Self {
        NightshadeError::IoError(e.to_string())
    }
}

impl From<anyhow::Error> for NightshadeError {
    fn from(e: anyhow::Error) -> Self {
        NightshadeError::Internal(e.to_string())
    }
}

impl From<tokio::time::error::Elapsed> for NightshadeError {
    fn from(_: tokio::time::error::Elapsed) -> Self {
        NightshadeError::Timeout("Operation timed out".to_string())
    }
}

impl From<tokio::task::JoinError> for NightshadeError {
    fn from(e: tokio::task::JoinError) -> Self {
        if e.is_cancelled() {
            NightshadeError::Cancelled
        } else if e.is_panic() {
            NightshadeError::Internal("Task panicked".to_string())
        } else {
            NightshadeError::Internal(format!("Task join error: {}", e))
        }
    }
}

impl From<String> for NightshadeError {
    fn from(s: String) -> Self {
        NightshadeError::Internal(s)
    }
}

impl From<&str> for NightshadeError {
    fn from(s: &str) -> Self {
        NightshadeError::Internal(s.to_string())
    }
}

// =========================================================================
// Backward Compatibility - Conversion TO String for Legacy APIs
// =========================================================================

/// Convert NightshadeError to String for backward compatibility with APIs
/// that return Result<T, String>
impl From<NightshadeError> for String {
    fn from(e: NightshadeError) -> Self {
        e.to_string()
    }
}

/// Also support reference conversion
impl From<&NightshadeError> for String {
    fn from(e: &NightshadeError) -> Self {
        e.to_string()
    }
}

// =========================================================================
// FFI Serialization Support
// =========================================================================

/// Structured error information for FFI transfer
///
/// This struct provides a flat representation of the error that can be
/// easily serialized and transferred across the FFI boundary to Dart.
#[derive(Debug, Clone, serde::Serialize, serde::Deserialize)]
pub struct ErrorInfo {
    /// Error category (timeout, connection, hardware, validation, etc.)
    pub category: String,
    /// Human-readable error message
    pub message: String,
    /// User-friendly message for UI display
    pub user_message: String,
    /// Whether the operation can be retried
    pub is_recoverable: bool,
    /// Whether reconnection should be attempted
    pub should_reconnect: bool,
    /// Whether this was a timeout
    pub is_timeout: bool,
    /// Device ID if error is device-related
    pub device_id: Option<String>,
    /// Vendor-specific error code if available
    pub error_code: Option<i32>,
}

impl From<&NightshadeError> for ErrorInfo {
    fn from(e: &NightshadeError) -> Self {
        ErrorInfo {
            category: e.error_category().to_string(),
            message: e.to_string(),
            user_message: e.user_message(),
            is_recoverable: e.is_recoverable(),
            should_reconnect: e.should_reconnect(),
            is_timeout: e.is_timeout(),
            device_id: e.device_id().map(|s| s.to_string()),
            error_code: e.error_code(),
        }
    }
}

impl From<NightshadeError> for ErrorInfo {
    fn from(e: NightshadeError) -> Self {
        ErrorInfo::from(&e)
    }
}

impl NightshadeError {
    /// Convert to ErrorInfo for FFI serialization
    pub fn to_error_info(&self) -> ErrorInfo {
        ErrorInfo::from(self)
    }

    /// Serialize the error to JSON for FFI transfer
    pub fn to_json(&self) -> String {
        serde_json::to_string(&self.to_error_info()).unwrap_or_else(|_| self.to_string())
    }
}

// =========================================================================
// Backward Compatibility - Legacy Error Conversion
// =========================================================================

// Allow converting legacy string-based InvalidDeviceId to new format
impl NightshadeError {
    /// Convert from legacy InvalidDeviceId(String) format
    pub fn from_legacy_invalid_device_id(msg: String) -> Self {
        NightshadeError::InvalidDeviceId {
            device_id: "unknown".to_string(),
            reason: msg,
        }
    }

    /// Convert from legacy ConnectionFailed(String) format
    pub fn from_legacy_connection_failed(msg: String) -> Self {
        NightshadeError::ConnectionFailed {
            device_id: "unknown".to_string(),
            reason: msg,
        }
    }
}

/// Result type alias for Nightshade operations
pub type NightshadeResult<T> = Result<T, NightshadeError>;

// =========================================================================
// Safe Conversion Traits
// =========================================================================

/// Extension trait for safely converting Results with context
pub trait ResultExt<T> {
    /// Add context to an error
    fn with_context<C: FnOnce() -> String>(self, context: C) -> NightshadeResult<T>;

    /// Convert to NightshadeError with device context
    fn with_device_context(self, device_id: &str, operation: &str) -> NightshadeResult<T>;
}

impl<T, E: std::fmt::Display> ResultExt<T> for Result<T, E> {
    fn with_context<C: FnOnce() -> String>(self, context: C) -> NightshadeResult<T> {
        self.map_err(|e| NightshadeError::Internal(format!("{}: {}", context(), e)))
    }

    fn with_device_context(self, device_id: &str, operation: &str) -> NightshadeResult<T> {
        self.map_err(|e| NightshadeError::OperationFailed(
            format!("{} on {}: {}", operation, device_id, e)
        ))
    }
}

/// Extension trait for safely converting Options
pub trait OptionExt<T> {
    /// Convert None to a device not found error
    fn or_device_not_found(self, device_id: &str) -> NightshadeResult<T>;

    /// Convert None to an internal error
    fn or_internal(self, message: &str) -> NightshadeResult<T>;
}

impl<T> OptionExt<T> for Option<T> {
    fn or_device_not_found(self, device_id: &str) -> NightshadeResult<T> {
        self.ok_or_else(|| NightshadeError::DeviceNotFound(device_id.to_string()))
    }

    fn or_internal(self, message: &str) -> NightshadeResult<T> {
        self.ok_or_else(|| NightshadeError::Internal(message.to_string()))
    }
}

