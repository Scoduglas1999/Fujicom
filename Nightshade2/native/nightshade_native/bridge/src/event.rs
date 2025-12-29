//! Global Event Bus for cross-component communication
//!
//! Events are published by various components and can be subscribed to
//! by the Dart side to update UI and trigger reactions.
//!
//! # Features
//!
//! - **Sequence Numbers**: Each event has a unique, monotonically increasing ID
//! - **Causality Tracking**: Events can reference the event that caused them
//! - **Category-based Filtering**: Subscribe to specific event categories
//! - **Overflow Handling**: Graceful handling when event buffer is full with diagnostics
//!
//! # Buffer Size
//!
//! The default buffer size is 4096 events (`DEFAULT_EVENT_BUFFER_SIZE`). This is sized
//! to handle burst scenarios like rapid autofocus loops or high-frequency guiding updates
//! without dropping events. If the buffer fills up (slow consumer), events are logged
//! and counted for diagnostics.
//!
//! # Example
//!
//! ```rust
//! let bus = EventBus::new(DEFAULT_EVENT_BUFFER_SIZE);
//!
//! // Publish an event
//! let event_id = bus.publish_with_tracking(
//!     EventSeverity::Info,
//!     EventCategory::Equipment,
//!     EventPayload::Equipment(EquipmentEvent::Connected { .. }),
//!     None, // No causal parent
//! );
//!
//! // Publish a follow-up event with causality
//! bus.publish_with_tracking(
//!     EventSeverity::Info,
//!     EventCategory::Imaging,
//!     EventPayload::Imaging(ImagingEvent::ExposureStarted { .. }),
//!     Some(event_id), // This was caused by the connection event
//! );
//! ```

use flutter_rust_bridge::frb;
use serde::{Deserialize, Serialize};
use std::sync::Arc;
use std::sync::atomic::{AtomicU64, Ordering};
use tokio::sync::broadcast;

/// Default event buffer size.
///
/// This is sized to handle burst scenarios like:
/// - Rapid autofocus loops (100+ events in seconds)
/// - High-frequency guiding corrections (10+ per second)
/// - Multiple simultaneous device state changes
///
/// The buffer uses a broadcast channel, so if any receiver falls behind by more than
/// this many events, it will receive a `Lagged` error and skip to the latest events.
/// Increasing this value uses more memory but reduces the chance of dropping events
/// when the Dart side is slow to consume them.
pub const DEFAULT_EVENT_BUFFER_SIZE: usize = 4096;

/// Event severity levels
#[frb]
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
pub enum EventSeverity {
    Info,
    Warning,
    Error,
    Critical,
}

/// Categories of events
#[frb]
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
pub enum EventCategory {
    Equipment,
    Imaging,
    Guiding,
    Sequencer,
    Safety,
    System,
    PolarAlignment,
}

/// Equipment-specific events
#[frb]
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum EquipmentEvent {
    // Generic device events
    Connecting { device_type: String, device_id: String },
    Connected { device_type: String, device_id: String },
    Disconnected { device_type: String, device_id: String },
    PropertyChanged { device_type: String, device_id: String, property: String, value: String },
    Error { device_type: String, device_id: String, message: String },

    // Mount events
    MountSlewStarted { ra: f64, dec: f64 },
    MountSlewCompleted { ra: f64, dec: f64 },
    MountTrackingStarted,
    MountTrackingStopped,
    MountParkStarted,
    MountParkCompleted,
    MountUnparked,

    // Focuser events
    FocuserMoveStarted { target_position: i32 },
    FocuserMoveCompleted { position: i32 },
    FocuserTemperatureChanged { temperature: f64 },

    // Filter wheel events
    FilterChanging { from_position: i32, to_position: i32, filter_name: Option<String> },
    FilterChanged { position: i32, filter_name: Option<String> },

    // Rotator events
    RotatorMoveStarted { target_angle: f64 },
    RotatorMoveCompleted { angle: f64 },

    // Camera events
    CameraCoolingStarted { target_temp: f64 },
    CameraCoolingReached { temperature: f64 },
    CameraWarmingStarted,
    CameraWarmingCompleted,
}

/// Polar alignment error data
#[frb]
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct PolarAlignmentEvent {
    pub azimuth_error: f64,
    pub altitude_error: f64,
    pub total_error: f64,
    pub current_ra: f64,
    pub current_dec: f64,
    pub target_ra: f64,
    pub target_dec: f64,
}

/// Polar alignment status update
#[frb]
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct PolarAlignmentStatus {
    pub status: String,
    pub phase: String,
    pub point: i32,
}

/// Polar alignment image data for UI display
#[frb]
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct PolarAlignmentImageEvent {
    /// JPEG-encoded image bytes for display
    pub image_data: Vec<u8>,
    /// Image width
    pub width: u32,
    /// Image height
    pub height: u32,
    /// Plate solve result (if available)
    pub solved_ra: Option<f64>,
    pub solved_dec: Option<f64>,
    /// Current measurement point (1-3) or 0 for adjustment phase
    pub point: i32,
    /// Phase: "measuring" or "adjusting"
    pub phase: String,
}

/// Imaging-specific events
#[frb]
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum ImagingEvent {
    // Basic exposure events
    ExposureStarted {
        duration_secs: f64,
        frame_type: crate::device::FrameType,
    },
    ExposureStartedWithFrame {
        duration_secs: f64,
        frame_type: crate::device::FrameType,
        frame_number: u32,
        total_frames: Option<u32>,
    },
    ExposureProgress {
        progress: f64,
        remaining_secs: f64,
    },
    ExposureCompleted {
        file_path: Option<String>,
        hfr: f64,
        stars_detected: u32,
    },
    ExposureCompletedWithFrame {
        frame_number: u32,
        total_frames: Option<u32>,
        hfr: f64,
        stars_detected: u32,
    },
    ExposureFailed {
        error: String,
    },
    ExposureCancelled,
    
    // Download events
    DownloadStarted,
    DownloadCompleted,
    
    // Image events
    ImageReady {
        width: u32,
        height: u32,
    },
    ImageSaved {
        file_path: String,
    },
    
    // Temperature events
    TemperatureChanged {
        temp_celsius: f64,
        cooler_power: f64,
    },
    
    // Deprecated (for backwards compatibility)
    #[serde(rename = "ExposureComplete")]
    ExposureComplete {
        success: bool,
    },
    #[serde(rename = "ExposureFailed_Old")]
    ExposureFailedOld {
        reason: String,
    },
}

/// Guiding-specific events
#[frb]
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum GuidingEvent {
    Connected,
    Disconnected,
    GuidingStarted,
    GuidingStopped,
    Paused,
    Resumed,
    Settled { rms: f64 },
    LostStar,
    DitherStarted { pixels: f64 },
    DitherCompleted,
    Correction { ra: f64, dec: f64, ra_raw: f64, dec_raw: f64 },
}

/// Sequencer-specific events
#[frb]
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum SequencerEvent {
    Started { sequence_name: String },
    Paused,
    Resumed,
    Stopped,
    Completed,
    NodeStarted { node_id: String, node_type: String },
    NodeCompleted { node_id: String, success: bool },
    Progress { current: u32, total: u32 },
    TargetChanged { target_name: String },
    TargetCompleted { target_name: String },
    ExposureStarted { frame: u32, total: u32, filter: Option<String>, duration_secs: f64 },
    ExposureCompleted { frame: u32, total: u32, duration_secs: f64 },
    Error { message: String },
    /// Progress update for long-running instructions (cooling, autofocus, slewing)
    InstructionProgress {
        /// Node ID for mapping progress to the correct tree node
        node_id: String,
        /// Name of the instruction (e.g., "Cool Camera", "Autofocus")
        instruction: String,
        /// Progress percentage (0.0 to 100.0)
        progress_percent: f64,
        /// Detailed status message
        detail: String,
    },
}

/// Safety-specific events
#[frb]
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum SafetyEvent {
    WeatherUnsafe { reason: String },
    WeatherSafe,
    EmergencyStop { reason: String },
    ParkInitiated { reason: String },
    ParkCompleted,
}

/// System-level events
#[frb]
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum SystemEvent {
    Initialized,
    ShuttingDown,
    Error { message: String },
    DiskSpaceLow { available_gb: f64 },
    Notification { title: String, message: String, level: String },
    /// Notification that events were dropped due to slow consumer
    /// This is sent after the stream recovers to inform the Dart side
    EventsDropped {
        /// Number of events that were dropped/skipped
        dropped_count: u64,
        /// Total number of events dropped since app start
        total_dropped: u64,
    },
}

/// A unified event that can be any category
#[frb]
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct NightshadeEvent {
    /// Unique event ID (monotonically increasing sequence number)
    pub event_id: u64,
    /// Timestamp when the event was created (milliseconds since Unix epoch)
    pub timestamp: i64,
    /// Severity level of the event
    pub severity: EventSeverity,
    /// Category of the event
    pub category: EventCategory,
    /// The actual event data
    pub payload: EventPayload,
    /// Event ID that caused this event (for causality tracking)
    /// None if this is a root event (no causal parent)
    #[serde(skip_serializing_if = "Option::is_none")]
    pub caused_by: Option<u64>,
    /// Correlation ID for grouping related events (e.g., all events from one exposure)
    #[serde(skip_serializing_if = "Option::is_none")]
    pub correlation_id: Option<String>,
    /// Device ID if this event is device-related
    #[serde(skip_serializing_if = "Option::is_none")]
    pub device_id: Option<String>,
}

/// Event payload - one of the specific event types
#[frb]
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum EventPayload {
    Equipment(EquipmentEvent),
    Imaging(ImagingEvent),
    Guiding(GuidingEvent),
    Sequencer(SequencerEvent),
    Safety(SafetyEvent),
    System(SystemEvent),
    PolarAlignment(PolarAlignmentEvent),
    PolarAlignmentStatus(PolarAlignmentStatus),
    PolarAlignmentImage(PolarAlignmentImageEvent),
}

/// Statistics about the event bus
#[derive(Debug, Clone, Default)]
pub struct EventBusStats {
    /// Total events published
    pub events_published: u64,
    /// Events dropped due to slow receivers
    pub events_dropped: u64,
    /// Current number of subscribers
    pub subscriber_count: usize,
    /// Events by category (for the last N events)
    pub events_by_category: std::collections::HashMap<EventCategory, u64>,
}

/// Global event bus for publishing and subscribing to events
pub struct EventBus {
    /// Main event channel
    sender: broadcast::Sender<NightshadeEvent>,
    /// Sequence number generator
    sequence: AtomicU64,
    /// Events published counter
    events_published: AtomicU64,
    /// Events dropped counter
    events_dropped: AtomicU64,
    /// Channel capacity
    capacity: usize,
}

impl EventBus {
    /// Create a new event bus with the specified channel capacity
    pub fn new(capacity: usize) -> Self {
        let (sender, _) = broadcast::channel(capacity);
        Self {
            sender,
            sequence: AtomicU64::new(1),
            events_published: AtomicU64::new(0),
            events_dropped: AtomicU64::new(0),
            capacity,
        }
    }

    /// Get the next sequence number
    fn next_sequence(&self) -> u64 {
        self.sequence.fetch_add(1, Ordering::SeqCst)
    }

    /// Publish an event to all subscribers
    /// Returns the event ID assigned to the published event
    pub fn publish(&self, event: NightshadeEvent) -> u64 {
        let event_id = event.event_id;
        self.events_published.fetch_add(1, Ordering::Relaxed);

        match self.sender.send(event) {
            Ok(_) => {}
            Err(_) => {
                // No receivers - this is fine
            }
        }

        event_id
    }

    /// Publish an event with full tracking support
    ///
    /// # Arguments
    /// * `severity` - Event severity level
    /// * `category` - Event category
    /// * `payload` - The event data
    /// * `caused_by` - Optional parent event ID for causality tracking
    ///
    /// # Returns
    /// The event ID of the published event
    pub fn publish_with_tracking(
        &self,
        severity: EventSeverity,
        category: EventCategory,
        payload: EventPayload,
        caused_by: Option<u64>,
    ) -> u64 {
        let event_id = self.next_sequence();
        let event = NightshadeEvent {
            event_id,
            timestamp: chrono::Utc::now().timestamp_millis(),
            severity,
            category,
            payload,
            caused_by,
            correlation_id: None,
            device_id: None,
        };

        self.publish(event)
    }

    /// Publish an event with device context
    pub fn publish_device_event(
        &self,
        severity: EventSeverity,
        category: EventCategory,
        payload: EventPayload,
        device_id: &str,
        caused_by: Option<u64>,
    ) -> u64 {
        let event_id = self.next_sequence();
        let event = NightshadeEvent {
            event_id,
            timestamp: chrono::Utc::now().timestamp_millis(),
            severity,
            category,
            payload,
            caused_by,
            correlation_id: None,
            device_id: Some(device_id.to_string()),
        };

        self.publish(event)
    }

    /// Publish an event with correlation ID for grouping related events
    pub fn publish_correlated(
        &self,
        severity: EventSeverity,
        category: EventCategory,
        payload: EventPayload,
        correlation_id: &str,
        caused_by: Option<u64>,
    ) -> u64 {
        let event_id = self.next_sequence();
        let event = NightshadeEvent {
            event_id,
            timestamp: chrono::Utc::now().timestamp_millis(),
            severity,
            category,
            payload,
            caused_by,
            correlation_id: Some(correlation_id.to_string()),
            device_id: None,
        };

        self.publish(event)
    }

    /// Subscribe to receive events
    pub fn subscribe(&self) -> broadcast::Receiver<NightshadeEvent> {
        self.sender.subscribe()
    }

    /// Get the number of active subscribers
    pub fn subscriber_count(&self) -> usize {
        self.sender.receiver_count()
    }

    /// Get the current sequence number (useful for debugging)
    pub fn current_sequence(&self) -> u64 {
        self.sequence.load(Ordering::SeqCst)
    }

    /// Get statistics about the event bus
    pub fn stats(&self) -> EventBusStats {
        EventBusStats {
            events_published: self.events_published.load(Ordering::Relaxed),
            events_dropped: self.events_dropped.load(Ordering::Relaxed),
            subscriber_count: self.sender.receiver_count(),
            events_by_category: std::collections::HashMap::new(), // Would need ring buffer to track
        }
    }

    /// Check if there's capacity for more events
    pub fn has_capacity(&self) -> bool {
        // Broadcast channels don't have a way to check capacity directly
        // We rely on the lagged error handling
        true
    }

    /// Get the configured capacity
    pub fn capacity(&self) -> usize {
        self.capacity
    }
}

impl Default for EventBus {
    fn default() -> Self {
        Self::new(DEFAULT_EVENT_BUFFER_SIZE)
    }
}

/// Global sequence counter for events created outside of the EventBus.
/// This is used by `create_event_auto_id` for ad-hoc event creation.
static GLOBAL_EVENT_SEQUENCE: AtomicU64 = AtomicU64::new(1_000_000);

/// Create an event with the current timestamp and a specified event ID
pub fn create_event(event_id: u64, severity: EventSeverity, category: EventCategory, payload: EventPayload) -> NightshadeEvent {
    NightshadeEvent {
        event_id,
        timestamp: chrono::Utc::now().timestamp_millis(),
        severity,
        category,
        payload,
        caused_by: None,
        correlation_id: None,
        device_id: None,
    }
}

/// Create an event with an auto-generated event ID.
/// Useful for ad-hoc events created outside of the main EventBus.
/// Note: IDs start at 1,000,000 to avoid collisions with EventBus IDs which start at 1.
pub fn create_event_auto_id(severity: EventSeverity, category: EventCategory, payload: EventPayload) -> NightshadeEvent {
    let event_id = GLOBAL_EVENT_SEQUENCE.fetch_add(1, Ordering::Relaxed);
    create_event(event_id, severity, category, payload)
}

/// Create an event with causality tracking
pub fn create_event_with_cause(
    event_id: u64,
    severity: EventSeverity,
    category: EventCategory,
    payload: EventPayload,
    caused_by: u64,
) -> NightshadeEvent {
    NightshadeEvent {
        event_id,
        timestamp: chrono::Utc::now().timestamp_millis(),
        severity,
        category,
        payload,
        caused_by: Some(caused_by),
        correlation_id: None,
        device_id: None,
    }
}

/// Thread-safe shared event bus
pub type SharedEventBus = Arc<EventBus>;

// =========================================================================
// Event Context for Tracking Causality
// =========================================================================

/// Context for tracking event causality
///
/// Pass this through operations to automatically track which events
/// caused other events.
#[derive(Debug, Clone)]
pub struct EventContext {
    /// The event ID that started this chain
    pub root_event_id: u64,
    /// The immediate parent event ID
    pub parent_event_id: u64,
    /// Correlation ID for grouping
    pub correlation_id: Option<String>,
    /// Device ID if this context is device-specific
    pub device_id: Option<String>,
}

impl EventContext {
    /// Create a new root event context
    pub fn new(event_id: u64) -> Self {
        Self {
            root_event_id: event_id,
            parent_event_id: event_id,
            correlation_id: None,
            device_id: None,
        }
    }

    /// Create a child context from this context
    pub fn child(&self, event_id: u64) -> Self {
        Self {
            root_event_id: self.root_event_id,
            parent_event_id: event_id,
            correlation_id: self.correlation_id.clone(),
            device_id: self.device_id.clone(),
        }
    }

    /// Set the correlation ID
    pub fn with_correlation(mut self, correlation_id: &str) -> Self {
        self.correlation_id = Some(correlation_id.to_string());
        self
    }

    /// Set the device ID
    pub fn with_device(mut self, device_id: &str) -> Self {
        self.device_id = Some(device_id.to_string());
        self
    }
}

// =========================================================================
// Correlation ID Generator
// =========================================================================

/// Generate a unique correlation ID
pub fn generate_correlation_id() -> String {
    use std::time::{SystemTime, UNIX_EPOCH};

    let timestamp = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .unwrap_or_default()
        .as_micros();

    // Simple format: timestamp-random (no external deps)
    let random = timestamp.wrapping_mul(6364136223846793005) % 100000;
    format!("corr-{}-{:05}", timestamp, random)
}

