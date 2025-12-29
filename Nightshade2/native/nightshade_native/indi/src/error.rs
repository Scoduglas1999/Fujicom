//! INDI error types
//!
//! Provides structured error types for INDI operations.

use std::fmt;
use std::time::Duration;

/// INDI client errors
#[derive(Debug, Clone)]
pub enum IndiError {
    /// Connection to INDI server failed
    ConnectionFailed(String),
    /// Connection timeout with context
    ConnectionTimeout {
        host: String,
        port: u16,
        duration: Duration,
    },
    /// Operation timeout with detailed context
    OperationTimeout {
        operation: String,
        device: Option<String>,
        property: Option<String>,
        duration: Duration,
        context: String,
    },
    /// Message parse timeout - partial XML message not completed
    MessageParseTimeout {
        duration: Duration,
        bytes_received: usize,
        context: String,
    },
    /// BLOB reception timeout
    BlobTimeout {
        device: String,
        property: String,
        expected_size: usize,
        received_size: usize,
        duration: Duration,
    },
    /// Property response timeout
    PropertyTimeout {
        device: String,
        property: String,
        duration: Duration,
        last_state: Option<String>,
    },
    /// Legacy timeout variant (kept for backwards compatibility)
    Timeout,
    /// XML parse error
    ParseError(String),
    /// Property not found
    PropertyNotFound { device: String, property: String },
    /// Permission denied (attempted to write to read-only property)
    PermissionDenied(String),
    /// Device alert state
    DeviceAlert(String),
    /// Protocol error
    ProtocolError(String),
    /// Reader task died unexpectedly
    ReaderDied(String),
    /// Send channel closed
    ChannelClosed(String),
    /// Not connected to server
    NotConnected,
    /// BLOB format error
    BlobFormatError { format: String, message: String },
    /// Property value out of range
    ValueOutOfRange {
        device: String,
        property: String,
        element: String,
        value: f64,
        min: f64,
        max: f64,
    },
    /// Protocol version mismatch
    VersionMismatch { required: String, server: String },
    /// Reconnection failed after max attempts
    ReconnectionFailed { attempts: u32, last_error: String },
}

impl std::error::Error for IndiError {}

impl fmt::Display for IndiError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            IndiError::ConnectionFailed(msg) => write!(f, "Connection failed: {}", msg),
            IndiError::ConnectionTimeout { host, port, duration } => {
                write!(
                    f,
                    "Connection timeout: failed to connect to {}:{} after {:?}",
                    host, port, duration
                )
            }
            IndiError::OperationTimeout {
                operation,
                device,
                property,
                duration,
                context,
            } => {
                let location = match (device, property) {
                    (Some(d), Some(p)) => format!(" on {}.{}", d, p),
                    (Some(d), None) => format!(" on device {}", d),
                    _ => String::new(),
                };
                write!(
                    f,
                    "Operation '{}' timed out{} after {:?}: {}",
                    operation, location, duration, context
                )
            }
            IndiError::MessageParseTimeout {
                duration,
                bytes_received,
                context,
            } => {
                write!(
                    f,
                    "XML message parse timeout after {:?}: received {} bytes of incomplete message. {}",
                    duration, bytes_received, context
                )
            }
            IndiError::BlobTimeout {
                device,
                property,
                expected_size,
                received_size,
                duration,
            } => {
                write!(
                    f,
                    "BLOB reception timeout for {}.{}: received {}/{} bytes after {:?}",
                    device, property, received_size, expected_size, duration
                )
            }
            IndiError::PropertyTimeout {
                device,
                property,
                duration,
                last_state,
            } => {
                let state_info = last_state
                    .as_ref()
                    .map(|s| format!(" (last state: {})", s))
                    .unwrap_or_default();
                write!(
                    f,
                    "Property timeout for {}.{} after {:?}{}",
                    device, property, duration, state_info
                )
            }
            IndiError::Timeout => write!(f, "Connection timeout"),
            IndiError::ParseError(msg) => write!(f, "XML parse error: {}", msg),
            IndiError::PropertyNotFound { device, property } => {
                write!(f, "Property not found: {}.{}", device, property)
            }
            IndiError::PermissionDenied(msg) => write!(f, "Permission denied: {}", msg),
            IndiError::DeviceAlert(msg) => write!(f, "Device alert: {}", msg),
            IndiError::ProtocolError(msg) => write!(f, "Protocol error: {}", msg),
            IndiError::ReaderDied(msg) => write!(f, "Reader task died: {}", msg),
            IndiError::ChannelClosed(msg) => write!(f, "Channel closed: {}", msg),
            IndiError::NotConnected => write!(f, "Not connected to INDI server"),
            IndiError::BlobFormatError { format, message } => {
                write!(f, "BLOB format error ({}): {}", format, message)
            }
            IndiError::ValueOutOfRange {
                device,
                property,
                element,
                value,
                min,
                max,
            } => {
                write!(
                    f,
                    "Value {} out of range [{}, {}] for {}.{}.{}",
                    value, min, max, device, property, element
                )
            }
            IndiError::VersionMismatch { required, server } => {
                write!(
                    f,
                    "Protocol version mismatch: required {}, server {}",
                    required, server
                )
            }
            IndiError::ReconnectionFailed {
                attempts,
                last_error,
            } => {
                write!(
                    f,
                    "Reconnection failed after {} attempts: {}",
                    attempts, last_error
                )
            }
        }
    }
}

impl From<IndiError> for String {
    fn from(err: IndiError) -> String {
        err.to_string()
    }
}

/// Result type for INDI operations
pub type IndiResult<T> = Result<T, IndiError>;

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_error_display() {
        let err = IndiError::ConnectionFailed("connection refused".to_string());
        assert_eq!(err.to_string(), "Connection failed: connection refused");

        let err = IndiError::PropertyNotFound {
            device: "CCD Simulator".to_string(),
            property: "CCD_EXPOSURE".to_string(),
        };
        assert_eq!(
            err.to_string(),
            "Property not found: CCD Simulator.CCD_EXPOSURE"
        );

        let err = IndiError::ValueOutOfRange {
            device: "Focuser".to_string(),
            property: "ABS_FOCUS_POSITION".to_string(),
            element: "FOCUS_ABSOLUTE_POSITION".to_string(),
            value: 100000.0,
            min: 0.0,
            max: 50000.0,
        };
        assert!(err.to_string().contains("100000"));
        assert!(err.to_string().contains("0"));
        assert!(err.to_string().contains("50000"));
    }

    #[test]
    fn test_error_to_string_conversion() {
        let err = IndiError::Timeout;
        let s: String = err.into();
        assert_eq!(s, "Connection timeout");
    }

    #[test]
    fn test_connection_timeout_display() {
        let err = IndiError::ConnectionTimeout {
            host: "192.168.1.100".to_string(),
            port: 7624,
            duration: Duration::from_secs(30),
        };
        let msg = err.to_string();
        assert!(msg.contains("192.168.1.100"));
        assert!(msg.contains("7624"));
        assert!(msg.contains("30"));
    }

    #[test]
    fn test_operation_timeout_display() {
        let err = IndiError::OperationTimeout {
            operation: "slew".to_string(),
            device: Some("Telescope".to_string()),
            property: Some("EQUATORIAL_EOD_COORD".to_string()),
            duration: Duration::from_secs(300),
            context: "Mount did not reach target position".to_string(),
        };
        let msg = err.to_string();
        assert!(msg.contains("slew"));
        assert!(msg.contains("Telescope"));
        assert!(msg.contains("EQUATORIAL_EOD_COORD"));
        assert!(msg.contains("did not reach target"));
    }

    #[test]
    fn test_message_parse_timeout_display() {
        let err = IndiError::MessageParseTimeout {
            duration: Duration::from_secs(60),
            bytes_received: 1024,
            context: "Server may have sent incomplete XML".to_string(),
        };
        let msg = err.to_string();
        assert!(msg.contains("60"));
        assert!(msg.contains("1024"));
        assert!(msg.contains("incomplete"));
    }

    #[test]
    fn test_blob_timeout_display() {
        let err = IndiError::BlobTimeout {
            device: "CCD Simulator".to_string(),
            property: "CCD1".to_string(),
            expected_size: 4194304,
            received_size: 1048576,
            duration: Duration::from_secs(120),
        };
        let msg = err.to_string();
        assert!(msg.contains("CCD Simulator"));
        assert!(msg.contains("CCD1"));
        assert!(msg.contains("1048576"));
        assert!(msg.contains("4194304"));
    }

    #[test]
    fn test_property_timeout_display() {
        let err = IndiError::PropertyTimeout {
            device: "Focuser".to_string(),
            property: "ABS_FOCUS_POSITION".to_string(),
            duration: Duration::from_secs(120),
            last_state: Some("Busy".to_string()),
        };
        let msg = err.to_string();
        assert!(msg.contains("Focuser"));
        assert!(msg.contains("ABS_FOCUS_POSITION"));
        assert!(msg.contains("Busy"));
    }
}
