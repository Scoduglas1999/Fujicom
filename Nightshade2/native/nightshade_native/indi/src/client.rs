//! INDI client implementation
//!
//! This module provides a robust INDI client with:
//! - Proper error handling using IndiError
//! - Reader task supervision with automatic reconnection
//! - XML parse timeout for incomplete messages
//! - Atomic keepalive operations
//! - BLOB format validation
//! - Property min/max extraction
//! - Permission checking before writes
//! - Protocol version negotiation
//! - Exponential backoff with jitter for reconnection
//! - Configurable timeouts for all operations

use crate::error::{IndiError, IndiResult};
use crate::{
    IndiDevice, IndiPermission, IndiProperty, IndiPropertyState, IndiPropertyType,
    IndiTimeoutConfig, IndiTimeoutError, INDI_DEFAULT_PORT,
};
use base64::{engine::general_purpose::STANDARD as BASE64, Engine as _};
use quick_xml::events::Event;
use std::collections::HashMap;
use std::sync::atomic::{AtomicBool, AtomicU32, AtomicU64, Ordering};
use std::sync::Arc;
use std::time::Duration;
use tokio::io::{AsyncRead, AsyncWrite, AsyncWriteExt};
use tokio::net::TcpStream;
use tokio::sync::{broadcast, mpsc, oneshot, RwLock};
use tokio::time::{sleep, timeout, Instant};

/// Supported INDI protocol versions
pub const INDI_PROTOCOL_VERSIONS: &[&str] = &["1.7", "1.8", "1.9"];

/// Default protocol version to use
pub const DEFAULT_PROTOCOL_VERSION: &str = "1.7";

/// INDI client event
#[derive(Debug, Clone)]
pub enum IndiEvent {
    /// Device defined
    DeviceDefined(String),
    /// Property defined
    PropertyDefined(String, String, IndiPropertyType),
    /// Property updated
    PropertyUpdated(String, String),
    /// Property deleted
    PropertyDeleted(String, String),
    /// BLOB received with format information
    BlobReceived {
        device: String,
        property: String,
        element: String,
        data: Vec<u8>,
        format: String,
        size: usize,
    },
    /// Connection state changed
    ConnectionStateChanged(bool),
    /// Error occurred
    Error(String),
    /// Reader task died (for supervision) - includes error message
    ReaderDied(String),
    /// Reader task is restarting - includes attempt number and delay
    ReaderRestarting {
        attempt: u32,
        max_attempts: u32,
        delay_secs: f64,
    },
    /// Reader task restarted successfully after failure
    ReaderRestarted {
        attempts_used: u32,
    },
    /// Reader task restart failed after max attempts
    ReaderRestartFailed {
        attempts: u32,
        last_error: String,
    },
    /// Reader task health changed
    ReaderHealthChanged {
        healthy: bool,
        status: ReaderStatus,
        consecutive_failures: u32,
    },
    /// Protocol version detected
    ProtocolVersionDetected(String),
}

/// Number element limits (min, max, step)
#[derive(Debug, Clone, Default)]
pub struct NumberLimits {
    pub min: Option<f64>,
    pub max: Option<f64>,
    pub step: Option<f64>,
    pub format: Option<String>,
}

/// Type alias for property value storage
type PropertyValueMap = HashMap<(String, String, String), String>;

/// Type alias for number limits storage
type NumberLimitsMap = HashMap<(String, String, String), NumberLimits>;

/// Reader task status for supervision
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ReaderStatus {
    /// Reader task is running normally
    Running,
    /// Reader task has stopped gracefully
    Stopped,
    /// Reader task has crashed/failed
    Crashed,
    /// Reader task is being restarted
    Restarting,
}

/// Configuration for reader task supervision
#[derive(Debug, Clone)]
pub struct ReaderTaskConfig {
    /// Maximum number of consecutive failures before giving up (default: 5)
    pub max_consecutive_failures: u32,
    /// Base delay for restart backoff (default: 1 second)
    pub restart_base_delay_secs: u64,
    /// Maximum delay cap for restart backoff (default: 60 seconds)
    pub restart_max_delay_secs: u64,
    /// Whether to automatically restart on failure (default: true)
    pub auto_restart: bool,
    /// Use jitter in restart delays to prevent thundering herd (default: true)
    pub use_jitter: bool,
    /// Jitter factor (0.0 to 1.0, default 0.3)
    pub jitter_factor: f64,
}

impl Default for ReaderTaskConfig {
    fn default() -> Self {
        Self {
            max_consecutive_failures: 5,
            restart_base_delay_secs: 1,
            restart_max_delay_secs: 60,
            auto_restart: true,
            use_jitter: true,
            jitter_factor: 0.3,
        }
    }
}

impl ReaderTaskConfig {
    /// Calculate restart delay for a given attempt number with optional jitter
    pub fn calculate_restart_delay(&self, attempt: u32) -> Duration {
        let base = Duration::from_secs(self.restart_base_delay_secs);
        let max = Duration::from_secs(self.restart_max_delay_secs);

        // Calculate exponential delay: base * 2^(attempt-1)
        let exponential_delay = base
            .checked_mul(2u32.pow(attempt.saturating_sub(1)))
            .unwrap_or(max)
            .min(max);

        if self.use_jitter && self.jitter_factor > 0.0 {
            let jitter_range = exponential_delay.as_secs_f64() * self.jitter_factor;
            let random_factor = rand_simple() * jitter_range - (jitter_range / 2.0);
            let jittered_secs = (exponential_delay.as_secs_f64() + random_factor).max(0.1);
            Duration::from_secs_f64(jittered_secs.min(max.as_secs_f64()))
        } else {
            exponential_delay
        }
    }
}

/// Configuration for protocol version
#[derive(Debug, Clone)]
pub struct ProtocolConfig {
    /// Preferred protocol version
    pub preferred_version: String,
    /// Whether to auto-detect server version
    pub auto_detect: bool,
    /// Minimum supported version
    pub min_version: Option<String>,
}

impl Default for ProtocolConfig {
    fn default() -> Self {
        Self {
            preferred_version: DEFAULT_PROTOCOL_VERSION.to_string(),
            auto_detect: true,
            min_version: None,
        }
    }
}

/// Reconnection configuration with jitter support
#[derive(Debug, Clone)]
pub struct ReconnectionConfig {
    /// Base delay for exponential backoff
    pub base_delay_secs: u64,
    /// Maximum delay cap
    pub max_delay_secs: u64,
    /// Maximum number of reconnection attempts
    pub max_attempts: u32,
    /// Whether to add jitter (randomness) to prevent thundering herd
    pub use_jitter: bool,
    /// Jitter factor (0.0 to 1.0, default 0.3 = 30% variation)
    pub jitter_factor: f64,
}

impl Default for ReconnectionConfig {
    fn default() -> Self {
        Self {
            base_delay_secs: 1,
            max_delay_secs: 30,
            max_attempts: 5,
            use_jitter: true,
            jitter_factor: 0.3,
        }
    }
}

impl ReconnectionConfig {
    /// Calculate delay for a given attempt number with optional jitter
    pub fn calculate_delay(&self, attempt: u32) -> Duration {
        // Calculate base exponential delay: base * 2^(attempt-1)
        let base = Duration::from_secs(self.base_delay_secs);
        let max = Duration::from_secs(self.max_delay_secs);

        let exponential_delay = base
            .checked_mul(2u32.pow(attempt.saturating_sub(1)))
            .unwrap_or(max)
            .min(max);

        if self.use_jitter && self.jitter_factor > 0.0 {
            // Add jitter: delay * (1 - jitter_factor/2 + random * jitter_factor)
            // This gives a range of [delay * (1 - jitter_factor/2), delay * (1 + jitter_factor/2)]
            let jitter_range = exponential_delay.as_secs_f64() * self.jitter_factor;
            let random_factor = rand_simple() * jitter_range - (jitter_range / 2.0);
            let jittered_secs = (exponential_delay.as_secs_f64() + random_factor).max(0.1);
            Duration::from_secs_f64(jittered_secs.min(max.as_secs_f64()))
        } else {
            exponential_delay
        }
    }
}

/// Simple pseudo-random number generator for jitter (0.0 to 1.0)
/// Uses a simple approach based on system time nanoseconds
fn rand_simple() -> f64 {
    use std::time::SystemTime;
    let nanos = SystemTime::now()
        .duration_since(SystemTime::UNIX_EPOCH)
        .map(|d| d.subsec_nanos())
        .unwrap_or(0);
    (nanos as f64 % 1000.0) / 1000.0
}

/// INDI client for communicating with an INDI server
pub struct IndiClient {
    host: String,
    port: u16,
    connected: Arc<AtomicBool>,
    devices: Arc<RwLock<HashMap<String, IndiDevice>>>,
    properties: Arc<RwLock<HashMap<(String, String), IndiProperty>>>,
    property_values: Arc<RwLock<PropertyValueMap>>,
    number_limits: Arc<RwLock<NumberLimitsMap>>,
    tx: Option<mpsc::Sender<String>>,
    event_tx: broadcast::Sender<IndiEvent>,
    timeout_config: IndiTimeoutConfig,
    /// Atomic timestamp for last keepalive (milliseconds since UNIX epoch)
    last_keepalive_ms: Arc<AtomicU64>,
    /// Atomic reconnection attempt counter
    reconnect_attempts: Arc<AtomicU32>,
    /// Reader task status
    reader_status: Arc<RwLock<ReaderStatus>>,
    /// Consecutive reader failure count (for supervision)
    reader_consecutive_failures: Arc<AtomicU32>,
    /// Shutdown signal sender
    shutdown_tx: Option<oneshot::Sender<()>>,
    /// Protocol configuration
    protocol_config: ProtocolConfig,
    /// Detected server protocol version
    server_version: Arc<RwLock<Option<String>>>,
    /// Reconnection configuration
    reconnection_config: ReconnectionConfig,
    /// Reader task supervision configuration
    reader_task_config: ReaderTaskConfig,
}

impl IndiClient {
    /// Create a new INDI client
    pub fn new(host: &str, port: Option<u16>) -> Self {
        Self::with_timeout_config(host, port, IndiTimeoutConfig::default())
    }

    /// Create a new INDI client with custom timeout configuration
    pub fn with_timeout_config(
        host: &str,
        port: Option<u16>,
        timeout_config: IndiTimeoutConfig,
    ) -> Self {
        let (event_tx, _) = broadcast::channel(100);
        Self {
            host: host.to_string(),
            port: port.unwrap_or(INDI_DEFAULT_PORT),
            connected: Arc::new(AtomicBool::new(false)),
            devices: Arc::new(RwLock::new(HashMap::new())),
            properties: Arc::new(RwLock::new(HashMap::new())),
            property_values: Arc::new(RwLock::new(HashMap::new())),
            number_limits: Arc::new(RwLock::new(HashMap::new())),
            tx: None,
            event_tx,
            timeout_config,
            last_keepalive_ms: Arc::new(AtomicU64::new(current_time_ms())),
            reconnect_attempts: Arc::new(AtomicU32::new(0)),
            reader_status: Arc::new(RwLock::new(ReaderStatus::Stopped)),
            reader_consecutive_failures: Arc::new(AtomicU32::new(0)),
            shutdown_tx: None,
            protocol_config: ProtocolConfig::default(),
            server_version: Arc::new(RwLock::new(None)),
            reconnection_config: ReconnectionConfig::default(),
            reader_task_config: ReaderTaskConfig::default(),
        }
    }

    /// Create a new INDI client with full configuration
    pub fn with_full_config(
        host: &str,
        port: Option<u16>,
        timeout_config: IndiTimeoutConfig,
        protocol_config: ProtocolConfig,
        reconnection_config: ReconnectionConfig,
    ) -> Self {
        Self::with_all_config(
            host,
            port,
            timeout_config,
            protocol_config,
            reconnection_config,
            ReaderTaskConfig::default(),
        )
    }

    /// Create a new INDI client with all configuration options including reader task config
    pub fn with_all_config(
        host: &str,
        port: Option<u16>,
        timeout_config: IndiTimeoutConfig,
        protocol_config: ProtocolConfig,
        reconnection_config: ReconnectionConfig,
        reader_task_config: ReaderTaskConfig,
    ) -> Self {
        let (event_tx, _) = broadcast::channel(100);
        Self {
            host: host.to_string(),
            port: port.unwrap_or(INDI_DEFAULT_PORT),
            connected: Arc::new(AtomicBool::new(false)),
            devices: Arc::new(RwLock::new(HashMap::new())),
            properties: Arc::new(RwLock::new(HashMap::new())),
            property_values: Arc::new(RwLock::new(HashMap::new())),
            number_limits: Arc::new(RwLock::new(HashMap::new())),
            tx: None,
            event_tx,
            timeout_config,
            last_keepalive_ms: Arc::new(AtomicU64::new(current_time_ms())),
            reconnect_attempts: Arc::new(AtomicU32::new(0)),
            reader_status: Arc::new(RwLock::new(ReaderStatus::Stopped)),
            reader_consecutive_failures: Arc::new(AtomicU32::new(0)),
            shutdown_tx: None,
            protocol_config,
            server_version: Arc::new(RwLock::new(None)),
            reconnection_config,
            reader_task_config,
        }
    }

    /// Get the timeout configuration
    pub fn timeout_config(&self) -> &IndiTimeoutConfig {
        &self.timeout_config
    }

    /// Set the timeout configuration
    pub fn set_timeout_config(&mut self, config: IndiTimeoutConfig) {
        self.timeout_config = config;
    }

    /// Get the protocol configuration
    pub fn protocol_config(&self) -> &ProtocolConfig {
        &self.protocol_config
    }

    /// Set the protocol configuration
    pub fn set_protocol_config(&mut self, config: ProtocolConfig) {
        self.protocol_config = config;
    }

    /// Get the reconnection configuration
    pub fn reconnection_config(&self) -> &ReconnectionConfig {
        &self.reconnection_config
    }

    /// Set the reconnection configuration
    pub fn set_reconnection_config(&mut self, config: ReconnectionConfig) {
        self.reconnection_config = config;
    }

    /// Get the reader task configuration
    pub fn reader_task_config(&self) -> &ReaderTaskConfig {
        &self.reader_task_config
    }

    /// Set the reader task configuration
    pub fn set_reader_task_config(&mut self, config: ReaderTaskConfig) {
        self.reader_task_config = config;
    }

    /// Get the detected server protocol version
    pub async fn server_version(&self) -> Option<String> {
        self.server_version.read().await.clone()
    }

    /// Subscribe to INDI events
    pub fn subscribe(&self) -> broadcast::Receiver<IndiEvent> {
        self.event_tx.subscribe()
    }

    /// Connect to the INDI server
    pub async fn connect(&mut self) -> IndiResult<()> {
        let addr = format!("{}:{}", self.host, self.port);
        let connection_timeout = self.timeout_config.connection_timeout();

        // Apply connection timeout
        let stream = match timeout(connection_timeout, TcpStream::connect(&addr)).await {
            Ok(Ok(stream)) => stream,
            Ok(Err(e)) => {
                return Err(IndiError::ConnectionFailed(format!(
                    "Failed to connect to INDI server at {}: {}. Check that the server is running and the address is correct.",
                    addr, e
                )));
            }
            Err(_) => {
                return Err(IndiError::ConnectionTimeout {
                    host: self.host.clone(),
                    port: self.port,
                    duration: connection_timeout,
                });
            }
        };

        let (read_half, write_half) = stream.into_split();

        // Create channel for sending commands
        let (tx, rx) = mpsc::channel::<String>(100);
        self.tx = Some(tx);

        // Create shutdown channel for reader task supervision
        let (shutdown_tx, shutdown_rx) = oneshot::channel();
        self.shutdown_tx = Some(shutdown_tx);

        // Spawn writer task
        tokio::spawn(Self::writer_task(write_half, rx));

        // Spawn supervised reader task
        let devices = self.devices.clone();
        let properties = self.properties.clone();
        let property_values = self.property_values.clone();
        let number_limits = self.number_limits.clone();
        let connected = self.connected.clone();
        let event_tx = self.event_tx.clone();
        let reader_status = self.reader_status.clone();
        let reader_consecutive_failures = self.reader_consecutive_failures.clone();
        let server_version = self.server_version.clone();
        let last_keepalive_ms = self.last_keepalive_ms.clone();
        let timeout_config = self.timeout_config.clone();
        let reader_task_config = self.reader_task_config.clone();

        // Reset consecutive failures on successful connect
        self.reader_consecutive_failures.store(0, Ordering::SeqCst);

        // Update reader status
        *self.reader_status.write().await = ReaderStatus::Running;

        // Emit health changed event - reader is now healthy
        let _ = self.event_tx.send(IndiEvent::ReaderHealthChanged {
            healthy: true,
            status: ReaderStatus::Running,
            consecutive_failures: 0,
        });

        tokio::spawn(async move {
            Self::supervised_reader_task(
                read_half,
                devices,
                properties,
                property_values,
                number_limits,
                connected,
                event_tx,
                reader_status,
                reader_consecutive_failures,
                server_version,
                last_keepalive_ms,
                timeout_config,
                reader_task_config,
                shutdown_rx,
            )
            .await;
        });

        // Mark as connected
        self.connected.store(true, Ordering::SeqCst);
        let _ = self.event_tx.send(IndiEvent::ConnectionStateChanged(true));

        // Update keepalive timestamp atomically
        self.last_keepalive_ms
            .store(current_time_ms(), Ordering::SeqCst);

        // Request device list with configured protocol version
        let version = &self.protocol_config.preferred_version;
        self.send_command(&format!("<getProperties version=\"{}\"/>", version))
            .await?;

        Ok(())
    }

    /// Writer task - sends commands to INDI server
    async fn writer_task<W: AsyncWrite + Unpin>(mut writer: W, mut rx: mpsc::Receiver<String>) {
        while let Some(cmd) = rx.recv().await {
            if let Err(e) = writer.write_all(cmd.as_bytes()).await {
                tracing::error!("INDI write error: {}", e);
                break;
            }
            if let Err(e) = writer.write_all(b"\n").await {
                tracing::error!("INDI write error: {}", e);
                break;
            }
        }
    }

    /// Supervised reader task - wraps the reader with supervision logic
    ///
    /// This function monitors the reader task and tracks failures. When the reader
    /// crashes, it:
    /// 1. Increments the consecutive failure counter
    /// 2. Updates the reader status to Crashed
    /// 3. Emits appropriate events (ReaderDied, ReaderHealthChanged)
    /// 4. Sets connected to false
    ///
    /// The caller (usually IndiClient via its event subscriber) is responsible for
    /// deciding whether to reconnect based on the failure count and configuration.
    #[allow(clippy::too_many_arguments)]
    async fn supervised_reader_task<R: AsyncRead + Unpin>(
        reader: R,
        devices: Arc<RwLock<HashMap<String, IndiDevice>>>,
        properties: Arc<RwLock<HashMap<(String, String), IndiProperty>>>,
        property_values: Arc<RwLock<PropertyValueMap>>,
        number_limits: Arc<RwLock<NumberLimitsMap>>,
        connected: Arc<AtomicBool>,
        event_tx: broadcast::Sender<IndiEvent>,
        reader_status: Arc<RwLock<ReaderStatus>>,
        reader_consecutive_failures: Arc<AtomicU32>,
        server_version: Arc<RwLock<Option<String>>>,
        last_keepalive_ms: Arc<AtomicU64>,
        timeout_config: IndiTimeoutConfig,
        reader_task_config: ReaderTaskConfig,
        mut shutdown_rx: oneshot::Receiver<()>,
    ) {
        // Run the reader task with panic catching via AssertUnwindSafe
        let result = tokio::select! {
            result = Self::reader_task_with_timeout(
                reader,
                devices,
                properties,
                property_values,
                number_limits,
                connected.clone(),
                event_tx.clone(),
                server_version,
                last_keepalive_ms,
                timeout_config,
            ) => {
                result
            }
            _ = &mut shutdown_rx => {
                tracing::info!("INDI reader task received shutdown signal - graceful stop");
                // Graceful shutdown - reset failure counter
                reader_consecutive_failures.store(0, Ordering::SeqCst);
                Ok(())
            }
        };

        // Update status based on result
        match result {
            Ok(_) => {
                // Graceful shutdown - reset failure counter and update status
                *reader_status.write().await = ReaderStatus::Stopped;
                let _ = event_tx.send(IndiEvent::ReaderHealthChanged {
                    healthy: false,
                    status: ReaderStatus::Stopped,
                    consecutive_failures: 0,
                });
                tracing::info!("INDI reader task stopped gracefully");
            }
            Err(ref e) => {
                // Failure - increment failure counter
                let failures = reader_consecutive_failures.fetch_add(1, Ordering::SeqCst) + 1;
                let max_failures = reader_task_config.max_consecutive_failures;

                tracing::error!(
                    "INDI reader task crashed (failure {}/{}): {}",
                    failures,
                    max_failures,
                    e
                );

                // Update status to Crashed
                *reader_status.write().await = ReaderStatus::Crashed;

                // Emit ReaderDied event with error details
                let _ = event_tx.send(IndiEvent::ReaderDied(e.to_string()));

                // Emit health changed event
                let _ = event_tx.send(IndiEvent::ReaderHealthChanged {
                    healthy: false,
                    status: ReaderStatus::Crashed,
                    consecutive_failures: failures,
                });

                // Check if we've exceeded max failures
                if failures >= max_failures {
                    tracing::error!(
                        "INDI reader task exceeded max consecutive failures ({}) - giving up",
                        max_failures
                    );
                    let _ = event_tx.send(IndiEvent::ReaderRestartFailed {
                        attempts: failures,
                        last_error: e.to_string(),
                    });
                } else if reader_task_config.auto_restart {
                    // Calculate restart delay and emit restart event
                    let delay = reader_task_config.calculate_restart_delay(failures);
                    tracing::info!(
                        "INDI reader task will suggest restart in {:?} (attempt {}/{})",
                        delay,
                        failures,
                        max_failures
                    );
                    let _ = event_tx.send(IndiEvent::ReaderRestarting {
                        attempt: failures,
                        max_attempts: max_failures,
                        delay_secs: delay.as_secs_f64(),
                    });
                }
            }
        };

        // Always mark as disconnected when reader stops
        connected.store(false, Ordering::SeqCst);
        let _ = event_tx.send(IndiEvent::ConnectionStateChanged(false));
    }

    /// Reader task with XML parse timeout - processes incoming INDI messages
    #[allow(clippy::too_many_arguments)]
    async fn reader_task_with_timeout<R: AsyncRead + Unpin>(
        reader: R,
        devices: Arc<RwLock<HashMap<String, IndiDevice>>>,
        properties: Arc<RwLock<HashMap<(String, String), IndiProperty>>>,
        property_values: Arc<RwLock<PropertyValueMap>>,
        number_limits: Arc<RwLock<NumberLimitsMap>>,
        connected: Arc<AtomicBool>,
        event_tx: broadcast::Sender<IndiEvent>,
        server_version: Arc<RwLock<Option<String>>>,
        last_keepalive_ms: Arc<AtomicU64>,
        timeout_config: IndiTimeoutConfig,
    ) -> IndiResult<()> {
        let mut reader = quick_xml::reader::Reader::from_reader(tokio::io::BufReader::new(reader));
        reader.trim_text(true);

        let mut buf = Vec::new();

        // State tracking
        let mut current_device = String::new();
        let mut current_property = String::new();
        let mut current_element = String::new();
        let mut current_blob_format = String::new();
        let mut current_blob_size: usize = 0;

        // XML parse timeout tracking - use configured timeout
        let xml_timeout = timeout_config.message_timeout();

        // BLOB reception timeout tracking
        let mut blob_start_time: Option<Instant> = None;
        let blob_timeout = timeout_config.blob_timeout();

        // Pending element for text content capture (currently unused but preserved for future use)
        #[allow(unused_assignments)]
        let mut _pending_number_limits: Option<(String, String, String, NumberLimits)> = None;

        // Track consecutive timeouts for message parse detection
        let mut incomplete_message_start: Option<Instant> = None;
        let mut incomplete_message_bytes: usize = 0;

        loop {
            // Check for XML parse timeout (incomplete messages)
            if let Some(start) = incomplete_message_start {
                if start.elapsed() > xml_timeout {
                    tracing::warn!(
                        "XML message parse timeout: incomplete message after {:?}. Received {} bytes. Resetting parser.",
                        xml_timeout,
                        incomplete_message_bytes
                    );
                    let _ = event_tx.send(IndiEvent::Error(format!(
                        "XML parse timeout after {:?}: {} bytes of incomplete message",
                        xml_timeout, incomplete_message_bytes
                    )));
                    buf.clear();
                    incomplete_message_start = None;
                    incomplete_message_bytes = 0;
                    continue;
                }
            }

            // Check for BLOB reception timeout
            if let Some(start) = blob_start_time {
                if start.elapsed() > blob_timeout {
                    tracing::error!(
                        "BLOB reception timeout for {}.{}: expected {} bytes after {:?}",
                        current_device, current_property, current_blob_size, blob_timeout
                    );
                    let _ = event_tx.send(IndiEvent::Error(format!(
                        "BLOB timeout for {}.{}: expected {} bytes after {:?}",
                        current_device, current_property, current_blob_size, blob_timeout
                    )));
                    // Reset BLOB state
                    blob_start_time = None;
                    current_blob_format.clear();
                    current_blob_size = 0;
                }
            }

            // Use timeout for reading events
            let read_timeout = Duration::from_secs(5);
            let read_result = timeout(read_timeout, reader.read_event_into_async(&mut buf)).await;

            match read_result {
                Ok(Ok(Event::Start(e))) => {
                    // Reset incomplete message tracking on successful event
                    incomplete_message_start = None;
                    incomplete_message_bytes = 0;
                    let name = e.name();
                    let name_str = String::from_utf8_lossy(name.as_ref()).to_string();

                    // Handle property definitions (def*Vector)
                    if name_str.starts_with("def") && name_str.ends_with("Vector") {
                        if let Some(dev) = get_attribute(&e, "device") {
                            current_device = dev.clone();
                            if let Some(prop) = get_attribute(&e, "name") {
                                current_property = prop.clone();

                                // Determine type
                                let prop_type = if name_str.contains("Switch") {
                                    IndiPropertyType::Switch
                                } else if name_str.contains("Number") {
                                    IndiPropertyType::Number
                                } else if name_str.contains("Text") {
                                    IndiPropertyType::Text
                                } else if name_str.contains("Light") {
                                    IndiPropertyType::Light
                                } else if name_str.contains("BLOB") {
                                    IndiPropertyType::Blob
                                } else {
                                    IndiPropertyType::Text
                                };

                                // Parse state and perm
                                let state_str =
                                    get_attribute(&e, "state").unwrap_or_else(|| "Idle".to_string());
                                let state = parse_state(&state_str);

                                let perm_str =
                                    get_attribute(&e, "perm").unwrap_or_else(|| "rw".to_string());
                                let perm = parse_perm(&perm_str);

                                // Add device if new
                                {
                                    let mut devs = devices.write().await;
                                    if !devs.contains_key(&current_device) {
                                        devs.insert(
                                            current_device.clone(),
                                            IndiDevice {
                                                name: current_device.clone(),
                                                driver: String::new(),
                                            },
                                        );
                                        let _ = event_tx
                                            .send(IndiEvent::DeviceDefined(current_device.clone()));
                                    }
                                }

                                // Add property
                                {
                                    let mut props = properties.write().await;
                                    props.insert(
                                        (current_device.clone(), current_property.clone()),
                                        IndiProperty {
                                            device: current_device.clone(),
                                            name: current_property.clone(),
                                            label: get_attribute(&e, "label")
                                                .unwrap_or_else(|| current_property.clone()),
                                            group: get_attribute(&e, "group").unwrap_or_default(),
                                            property_type: prop_type.clone(),
                                            state,
                                            perm,
                                            elements: Vec::new(),
                                        },
                                    );
                                }

                                let _ = event_tx.send(IndiEvent::PropertyDefined(
                                    current_device.clone(),
                                    current_property.clone(),
                                    prop_type,
                                ));
                            }
                        }
                    }
                    // Handle element definitions (defText, defNumber, etc. inside Vector)
                    else if name_str.starts_with("def") && !name_str.ends_with("Vector") {
                        if !current_device.is_empty() && !current_property.is_empty() {
                            if let Some(elem_name) = get_attribute(&e, "name") {
                                current_element = elem_name.clone();

                                // Add element to property
                                let mut props = properties.write().await;
                                if let Some(prop) = props
                                    .get_mut(&(current_device.clone(), current_property.clone()))
                                {
                                    prop.elements.push(elem_name.clone());
                                }

                                // Extract min/max/step/format for number elements
                                if name_str == "defNumber" {
                                    let limits = NumberLimits {
                                        min: get_attribute(&e, "min")
                                            .and_then(|s| s.parse().ok()),
                                        max: get_attribute(&e, "max")
                                            .and_then(|s| s.parse().ok()),
                                        step: get_attribute(&e, "step")
                                            .and_then(|s| s.parse().ok()),
                                        format: get_attribute(&e, "format"),
                                    };

                                    // Store limits
                                    let mut limits_map = number_limits.write().await;
                                    limits_map.insert(
                                        (
                                            current_device.clone(),
                                            current_property.clone(),
                                            elem_name.clone(),
                                        ),
                                        limits.clone(),
                                    );

                                    // Keep pending for value extraction
                                    _pending_number_limits = Some((
                                        current_device.clone(),
                                        current_property.clone(),
                                        elem_name,
                                        limits,
                                    ));
                                }
                            }
                        }
                    }
                    // Handle property updates (set*Vector, new*Vector)
                    else if (name_str.starts_with("set") || name_str.starts_with("new"))
                        && name_str.ends_with("Vector")
                    {
                        if let Some(dev) = get_attribute(&e, "device") {
                            current_device = dev;
                            if let Some(prop) = get_attribute(&e, "name") {
                                current_property = prop;

                                // Update state
                                if let Some(state_str) = get_attribute(&e, "state") {
                                    let state = parse_state(&state_str);
                                    let mut props = properties.write().await;
                                    if let Some(p) = props.get_mut(&(
                                        current_device.clone(),
                                        current_property.clone(),
                                    )) {
                                        p.state = state;
                                    }
                                }
                            }
                        }
                    }
                    // Handle BLOB elements with format attribute
                    else if name_str == "oneBLOB" {
                        if let Some(elem) = get_attribute(&e, "name") {
                            current_element = elem;
                        }
                        // Extract format attribute (e.g., ".fits", ".jpeg", ".png")
                        current_blob_format =
                            get_attribute(&e, "format").unwrap_or_else(|| ".fits".to_string());
                        // Extract size attribute
                        current_blob_size = get_attribute(&e, "size")
                            .and_then(|s| s.parse().ok())
                            .unwrap_or(0);
                        // Start BLOB reception timeout tracking
                        blob_start_time = Some(Instant::now());
                        tracing::debug!(
                            "Starting BLOB reception for {}.{}.{}: expected size {} bytes",
                            current_device, current_property, current_element, current_blob_size
                        );
                    }
                    // Handle elements values (oneSwitch, oneNumber, etc.)
                    else if name_str.starts_with("one") && name_str != "oneBLOB" {
                        if let Some(elem) = get_attribute(&e, "name") {
                            current_element = elem;
                        }
                    }
                    // Detect protocol version from server response
                    else if name_str == "getProperties" {
                        if let Some(version) = get_attribute(&e, "version") {
                            let mut sv = server_version.write().await;
                            *sv = Some(version.clone());
                            let _ = event_tx.send(IndiEvent::ProtocolVersionDetected(version));
                        }
                    }

                    // Update keepalive on any valid message
                    last_keepalive_ms.store(current_time_ms(), Ordering::SeqCst);
                }
                Ok(Ok(Event::Text(e))) => {
                    // Reset incomplete message tracking on successful event
                    incomplete_message_start = None;
                    incomplete_message_bytes = 0;
                    let text = e.unescape().unwrap_or_default().to_string();
                    if !current_device.is_empty()
                        && !current_property.is_empty()
                        && !current_element.is_empty()
                    {
                        // Store value
                        {
                            let mut vals = property_values.write().await;
                            vals.insert(
                                (
                                    current_device.clone(),
                                    current_property.clone(),
                                    current_element.clone(),
                                ),
                                text.clone(),
                            );
                        }

                        // Handle BLOB data with format validation
                        if !current_blob_format.is_empty() {
                            // Decode base64
                            match BASE64.decode(text.trim()) {
                                Ok(data) => {
                                    // Log successful BLOB reception
                                    if let Some(start) = blob_start_time {
                                        tracing::debug!(
                                            "BLOB received for {}.{}.{}: {} bytes in {:?}",
                                            current_device, current_property, current_element,
                                            data.len(), start.elapsed()
                                        );
                                    }

                                    // Validate BLOB format
                                    let validated_format =
                                        validate_blob_format(&current_blob_format, &data);

                                    let _ = event_tx.send(IndiEvent::BlobReceived {
                                        device: current_device.clone(),
                                        property: current_property.clone(),
                                        element: current_element.clone(),
                                        data,
                                        format: validated_format,
                                        size: current_blob_size,
                                    });
                                }
                                Err(e) => {
                                    tracing::warn!(
                                        "Failed to decode BLOB base64 for {}.{}.{}: {}",
                                        current_device,
                                        current_property,
                                        current_element,
                                        e
                                    );
                                }
                            }
                            // Reset BLOB tracking state
                            current_blob_format.clear();
                            current_blob_size = 0;
                            blob_start_time = None;
                        }
                    }

                    // Clear pending number limits after processing
                    _pending_number_limits = None;
                }
                Ok(Ok(Event::End(e))) => {
                    // Reset incomplete message tracking on successful event
                    incomplete_message_start = None;
                    incomplete_message_bytes = 0;
                    let name = String::from_utf8_lossy(e.name().as_ref()).to_string();
                    if name.starts_with("set") || name.starts_with("new") {
                        // Property update complete
                        let _ = event_tx.send(IndiEvent::PropertyUpdated(
                            current_device.clone(),
                            current_property.clone(),
                        ));
                        current_property.clear();
                    } else if name.starts_with("one") || name.starts_with("def") {
                        current_element.clear();
                    }
                }
                Ok(Ok(Event::Eof)) => {
                    tracing::info!("INDI connection closed (EOF)");
                    connected.store(false, Ordering::SeqCst);
                    let _ = event_tx.send(IndiEvent::ConnectionStateChanged(false));
                    break;
                }
                Ok(Err(e)) => {
                    tracing::error!(
                        "INDI XML parse error: {}. Raw buffer (first 200 chars): {:?}",
                        e,
                        String::from_utf8_lossy(&buf[..buf.len().min(200)])
                    );
                    // Continue on parse errors, try to recover
                }
                Err(_) => {
                    // Read timeout - check if connection is still alive
                    if !connected.load(Ordering::SeqCst) {
                        break;
                    }
                    // Track incomplete message if we have partial data in the buffer
                    if !buf.is_empty() {
                        if incomplete_message_start.is_none() {
                            incomplete_message_start = Some(Instant::now());
                        }
                        incomplete_message_bytes = buf.len();
                    }
                    // Continue waiting for data
                }
                _ => {}
            }
            buf.clear();
        }

        Ok(())
    }

    /// Disconnect from the INDI server
    ///
    /// This performs a graceful shutdown:
    /// 1. Sends shutdown signal to reader task
    /// 2. Closes the writer channel
    /// 3. Clears all cached device/property state
    /// 4. Resets failure counters (since this is intentional disconnect)
    /// 5. Emits connection state change event
    pub async fn disconnect(&mut self) -> IndiResult<()> {
        tracing::info!("Disconnecting from INDI server {}:{}", self.host, self.port);

        // Send shutdown signal to reader task
        if let Some(tx) = self.shutdown_tx.take() {
            let _ = tx.send(());
        }

        self.tx = None; // Drop sender, which will close the writer task
        self.connected.store(false, Ordering::SeqCst);

        // Clear cached state
        self.devices.write().await.clear();
        self.properties.write().await.clear();
        self.property_values.write().await.clear();
        self.number_limits.write().await.clear();

        // Reset failure counter since this is intentional disconnect
        self.reader_consecutive_failures.store(0, Ordering::SeqCst);

        // Update reader status
        *self.reader_status.write().await = ReaderStatus::Stopped;

        // Emit events
        let _ = self.event_tx.send(IndiEvent::ReaderHealthChanged {
            healthy: false,
            status: ReaderStatus::Stopped,
            consecutive_failures: 0,
        });
        let _ = self.event_tx.send(IndiEvent::ConnectionStateChanged(false));

        Ok(())
    }

    /// Check if connected
    pub async fn is_connected(&self) -> bool {
        self.connected.load(Ordering::SeqCst)
    }

    /// Get reader task status
    pub async fn reader_status(&self) -> ReaderStatus {
        *self.reader_status.read().await
    }

    /// Check if the reader task is healthy (running with no recent failures)
    ///
    /// Returns true if:
    /// - Reader status is Running
    /// - Consecutive failure count is 0
    ///
    /// Returns false if:
    /// - Reader is Stopped, Crashed, or Restarting
    /// - There have been any consecutive failures (even if currently running)
    pub fn is_reader_healthy(&self) -> bool {
        // Non-async version for quick health checks
        let failures = self.reader_consecutive_failures.load(Ordering::SeqCst);
        failures == 0 && self.connected.load(Ordering::SeqCst)
    }

    /// Get the number of consecutive reader failures
    pub fn reader_consecutive_failures(&self) -> u32 {
        self.reader_consecutive_failures.load(Ordering::SeqCst)
    }

    /// Check if the reader has exceeded the maximum failure threshold
    pub fn is_reader_failed_permanently(&self) -> bool {
        let failures = self.reader_consecutive_failures.load(Ordering::SeqCst);
        failures >= self.reader_task_config.max_consecutive_failures
    }

    /// Reset the consecutive failure counter (call after successful manual recovery)
    pub fn reset_reader_failures(&self) {
        self.reader_consecutive_failures.store(0, Ordering::SeqCst);
    }

    /// Send a raw INDI command
    pub async fn send_command(&mut self, command: &str) -> IndiResult<()> {
        if let Some(tx) = &self.tx {
            tx.send(command.to_string()).await.map_err(|e| {
                IndiError::ChannelClosed(format!(
                    "Failed to send INDI command to {}:{}: {}. The connection may have been lost.",
                    self.host, self.port, e
                ))
            })
        } else {
            Err(IndiError::NotConnected)
        }
    }

    /// Get the list of discovered devices
    pub async fn get_devices(&self) -> Vec<IndiDevice> {
        self.devices.read().await.values().cloned().collect()
    }

    /// Get properties for a device
    pub async fn get_properties(&self, device_name: &str) -> Vec<IndiProperty> {
        self.properties
            .read()
            .await
            .iter()
            .filter(|((device, _), _)| device == device_name)
            .map(|(_, prop)| prop.clone())
            .collect()
    }

    /// Get a property
    pub async fn get_property(&self, device: &str, property: &str) -> Option<IndiProperty> {
        self.properties
            .read()
            .await
            .get(&(device.to_string(), property.to_string()))
            .cloned()
    }

    /// Get a property value
    pub async fn get_property_value(
        &self,
        device: &str,
        property: &str,
        element: &str,
    ) -> Option<String> {
        self.property_values
            .read()
            .await
            .get(&(
                device.to_string(),
                property.to_string(),
                element.to_string(),
            ))
            .cloned()
    }

    /// Get number limits for a property element
    pub async fn get_number_limits(
        &self,
        device: &str,
        property: &str,
        element: &str,
    ) -> Option<NumberLimits> {
        self.number_limits
            .read()
            .await
            .get(&(
                device.to_string(),
                property.to_string(),
                element.to_string(),
            ))
            .cloned()
    }

    /// Get a number property value
    pub async fn get_number(&self, device: &str, property: &str, element: &str) -> Option<f64> {
        self.get_property_value(device, property, element)
            .await
            .and_then(|v| v.parse().ok())
    }

    /// Get a switch property value
    pub async fn get_switch(&self, device: &str, property: &str, element: &str) -> Option<bool> {
        self.get_property_value(device, property, element)
            .await
            .map(|v| v.eq_ignore_ascii_case("on"))
    }

    /// Get property state
    pub async fn get_property_state(
        &self,
        device: &str,
        property: &str,
    ) -> Option<IndiPropertyState> {
        self.properties
            .read()
            .await
            .get(&(device.to_string(), property.to_string()))
            .map(|p| p.state)
    }

    /// Get property permission
    pub async fn get_property_permission(
        &self,
        device: &str,
        property: &str,
    ) -> Option<IndiPermission> {
        self.properties
            .read()
            .await
            .get(&(device.to_string(), property.to_string()))
            .map(|p| p.perm)
    }

    /// Check if a property is in the busy state
    pub async fn is_property_busy(&self, device: &str, property: &str) -> bool {
        self.get_property_state(device, property)
            .await
            .map(|s| s == IndiPropertyState::Busy)
            .unwrap_or(false)
    }

    /// Check if a property exists for a device
    pub async fn has_property(&self, device: &str, property: &str) -> bool {
        self.properties
            .read()
            .await
            .contains_key(&(device.to_string(), property.to_string()))
    }

    /// Get a light property state value (0=Idle, 1=Ok, 2=Busy, 3=Alert)
    pub async fn get_light_state(
        &self,
        device: &str,
        property: &str,
        element: &str,
    ) -> Option<i32> {
        self.get_property_value(device, property, element)
            .await
            .and_then(|v| match v.as_str() {
                "Idle" => Some(0),
                "Ok" => Some(1),
                "Busy" => Some(2),
                "Alert" => Some(3),
                _ => v.parse().ok(),
            })
    }

    /// Enable BLOB mode for a device
    pub async fn enable_blob(&mut self, device: &str) -> IndiResult<()> {
        let cmd = format!(
            "<enableBLOB device=\"{}\" name=\"\">Also</enableBLOB>",
            device
        );
        self.send_command(&cmd).await
    }

    /// Check property permission before write
    fn check_write_permission(&self, perm: IndiPermission, property: &str) -> IndiResult<()> {
        match perm {
            IndiPermission::ReadOnly => Err(IndiError::PermissionDenied(format!(
                "Property '{}' is read-only",
                property
            ))),
            IndiPermission::WriteOnly | IndiPermission::ReadWrite => Ok(()),
        }
    }

    /// Validate number value against limits
    async fn validate_number_limits(
        &self,
        device: &str,
        property: &str,
        element: &str,
        value: f64,
    ) -> IndiResult<()> {
        if let Some(limits) = self.get_number_limits(device, property, element).await {
            if let (Some(min), Some(max)) = (limits.min, limits.max) {
                if value < min || value > max {
                    return Err(IndiError::ValueOutOfRange {
                        device: device.to_string(),
                        property: property.to_string(),
                        element: element.to_string(),
                        value,
                        min,
                        max,
                    });
                }
            }
        }
        Ok(())
    }

    /// Set a switch property with permission check
    pub async fn set_switch(
        &mut self,
        device: &str,
        property: &str,
        element: &str,
        state: bool,
    ) -> IndiResult<()> {
        // Check permission
        if let Some(perm) = self.get_property_permission(device, property).await {
            self.check_write_permission(perm, property)?;
        }

        let state_str = if state { "On" } else { "Off" };
        let cmd = format!(
            "<newSwitchVector device=\"{}\" name=\"{}\">\
             <oneSwitch name=\"{}\">{}</oneSwitch>\
             </newSwitchVector>",
            device, property, element, state_str
        );
        self.send_command(&cmd).await
    }

    /// Set a number property with permission and limits check
    pub async fn set_number(
        &mut self,
        device: &str,
        property: &str,
        element: &str,
        value: f64,
    ) -> IndiResult<()> {
        // Check permission
        if let Some(perm) = self.get_property_permission(device, property).await {
            self.check_write_permission(perm, property)?;
        }

        // Validate against limits
        self.validate_number_limits(device, property, element, value)
            .await?;

        let cmd = format!(
            "<newNumberVector device=\"{}\" name=\"{}\">\
             <oneNumber name=\"{}\">{}</oneNumber>\
             </newNumberVector>",
            device, property, element, value
        );
        self.send_command(&cmd).await
    }

    /// Set multiple number properties at once with validation
    pub async fn set_numbers(
        &mut self,
        device: &str,
        property: &str,
        values: &[(&str, f64)],
    ) -> IndiResult<()> {
        // Check permission
        if let Some(perm) = self.get_property_permission(device, property).await {
            self.check_write_permission(perm, property)?;
        }

        // Validate all values
        for (element, value) in values {
            self.validate_number_limits(device, property, element, *value)
                .await?;
        }

        let elements: String = values
            .iter()
            .map(|(name, value)| format!("<oneNumber name=\"{}\">{}</oneNumber>", name, value))
            .collect();
        let cmd = format!(
            "<newNumberVector device=\"{}\" name=\"{}\">{}</newNumberVector>",
            device, property, elements
        );
        self.send_command(&cmd).await
    }

    /// Set a text property with permission check
    pub async fn set_text(
        &mut self,
        device: &str,
        property: &str,
        element: &str,
        value: &str,
    ) -> IndiResult<()> {
        // Check permission
        if let Some(perm) = self.get_property_permission(device, property).await {
            self.check_write_permission(perm, property)?;
        }

        let cmd = format!(
            "<newTextVector device=\"{}\" name=\"{}\">\
             <oneText name=\"{}\">{}</oneText>\
             </newTextVector>",
            device, property, element, value
        );
        self.send_command(&cmd).await
    }

    // =========================================================================
    // HIGH-LEVEL DEVICE CONTROL METHODS
    // =========================================================================

    /// Connect to a device (turn on CONNECTION switch)
    pub async fn connect_device(&mut self, device: &str) -> IndiResult<()> {
        self.set_switch(device, "CONNECTION", "CONNECT", true).await
    }

    /// Disconnect from a device
    pub async fn disconnect_device(&mut self, device: &str) -> IndiResult<()> {
        self.set_switch(device, "CONNECTION", "DISCONNECT", true)
            .await
    }

    /// Check if a device is connected
    pub async fn is_device_connected(&self, device: &str) -> bool {
        self.get_switch(device, "CONNECTION", "CONNECT")
            .await
            .unwrap_or(false)
    }

    /// Get filter names for a filter wheel device
    pub async fn get_filter_names(&self, device: &str) -> Result<Vec<String>, String> {
        let props = self.get_properties(device).await;

        // Look for the FILTER_NAME property
        if let Some(prop) = props.iter().find(|p| p.name == "FILTER_NAME") {
            let mut names = Vec::new();
            for elem in &prop.elements {
                if let Some(val) = self.get_property_value(device, "FILTER_NAME", elem).await {
                    names.push(val);
                } else {
                    names.push(elem.clone());
                }
            }
            return Ok(names);
        }

        Ok(Vec::new())
    }

    // =========================================================================
    // TIMEOUT AND RELIABILITY METHODS
    // =========================================================================

    /// Wait for a property to reach a specific state with timeout
    pub async fn wait_for_property_state(
        &self,
        device: &str,
        property: &str,
        expected_state: IndiPropertyState,
        timeout_duration: Duration,
    ) -> Result<(), IndiTimeoutError> {
        let start = Instant::now();
        let poll_interval = Duration::from_millis(self.timeout_config.property_poll_interval_ms);
        let mut last_state = None;

        loop {
            // Check timeout
            if start.elapsed() >= timeout_duration {
                return Err(IndiTimeoutError {
                    device: device.to_string(),
                    property: property.to_string(),
                    context: format!(
                        "Timed out waiting for state {:?} after {:?}",
                        expected_state, timeout_duration
                    ),
                    last_state,
                });
            }

            // Check current state
            if let Some(state) = self.get_property_state(device, property).await {
                last_state = Some(state);

                if state == expected_state {
                    return Ok(());
                }

                // If state is Alert, return early with error
                if state == IndiPropertyState::Alert {
                    return Err(IndiTimeoutError {
                        device: device.to_string(),
                        property: property.to_string(),
                        context: format!(
                            "Property entered Alert state while waiting for {:?}",
                            expected_state
                        ),
                        last_state: Some(state),
                    });
                }
            }

            // Wait before polling again
            sleep(poll_interval).await;
        }
    }

    /// Wait for a property to no longer be busy (Ok or Idle state)
    pub async fn wait_for_property_not_busy(
        &self,
        device: &str,
        property: &str,
        timeout_duration: Duration,
    ) -> Result<(), IndiTimeoutError> {
        let start = Instant::now();
        let poll_interval = Duration::from_millis(self.timeout_config.property_poll_interval_ms);
        let mut last_state = None;

        loop {
            // Check timeout
            if start.elapsed() >= timeout_duration {
                return Err(IndiTimeoutError {
                    device: device.to_string(),
                    property: property.to_string(),
                    context: format!(
                        "Timed out waiting for property to finish (not Busy) after {:?}",
                        timeout_duration
                    ),
                    last_state,
                });
            }

            // Check current state
            if let Some(state) = self.get_property_state(device, property).await {
                last_state = Some(state);

                // Success if Ok or Idle
                if state == IndiPropertyState::Ok || state == IndiPropertyState::Idle {
                    return Ok(());
                }

                // Alert is an error condition
                if state == IndiPropertyState::Alert {
                    return Err(IndiTimeoutError {
                        device: device.to_string(),
                        property: property.to_string(),
                        context: "Property entered Alert state".to_string(),
                        last_state: Some(state),
                    });
                }
            }

            // Wait before polling again
            sleep(poll_interval).await;
        }
    }

    /// Send keepalive to detect dead connections (atomic operation)
    async fn send_keepalive(&mut self) -> IndiResult<()> {
        // Update timestamp atomically BEFORE sending
        self.last_keepalive_ms
            .store(current_time_ms(), Ordering::SeqCst);

        // Use configured protocol version
        let version = &self.protocol_config.preferred_version;
        self.send_command(&format!("<getProperties version=\"{}\"/>", version))
            .await
    }

    /// Check if keepalive is needed and send it (atomic operations to prevent race)
    pub async fn check_keepalive(&mut self) -> IndiResult<()> {
        let keepalive_interval_ms = self.timeout_config.keepalive_interval_secs * 1000;
        let last_ms = self.last_keepalive_ms.load(Ordering::SeqCst);
        let current_ms = current_time_ms();

        // Check if enough time has passed AND we're connected
        if current_ms.saturating_sub(last_ms) >= keepalive_interval_ms
            && self.connected.load(Ordering::SeqCst)
        {
            self.send_keepalive().await?;
        }

        Ok(())
    }

    /// Attempt to reconnect with exponential backoff and jitter
    pub async fn reconnect_with_backoff(&mut self) -> IndiResult<()> {
        let max_attempts = self.reconnection_config.max_attempts;
        let mut last_error = String::new();

        for attempt in 1..=max_attempts {
            self.reconnect_attempts.store(attempt, Ordering::SeqCst);

            tracing::info!(
                "Reconnection attempt {}/{} to {}:{}",
                attempt,
                max_attempts,
                self.host,
                self.port
            );

            match self.connect().await {
                Ok(_) => {
                    tracing::info!("Successfully reconnected to {}:{}", self.host, self.port);
                    self.reconnect_attempts.store(0, Ordering::SeqCst);
                    return Ok(());
                }
                Err(e) => {
                    last_error = e.to_string();
                    tracing::warn!("Reconnection attempt {} failed: {}", attempt, last_error);

                    if attempt < max_attempts {
                        let delay = self.reconnection_config.calculate_delay(attempt);
                        tracing::info!("Waiting {:?} before next reconnection attempt", delay);
                        sleep(delay).await;
                    }
                }
            }
        }

        Err(IndiError::ReconnectionFailed {
            attempts: max_attempts,
            last_error,
        })
    }

    /// Attempt to recover from a reader crash with proper supervision
    ///
    /// This method should be called when receiving a `ReaderRestarting` event.
    /// It will:
    /// 1. Wait for the suggested delay (based on failure count and backoff)
    /// 2. Attempt to reconnect
    /// 3. Emit appropriate events on success/failure
    ///
    /// Returns Ok(()) if reconnection succeeds, Err if it fails.
    pub async fn recover_reader(&mut self) -> IndiResult<()> {
        // Check if we've already exceeded max failures
        if self.is_reader_failed_permanently() {
            let failures = self.reader_consecutive_failures();
            return Err(IndiError::ReconnectionFailed {
                attempts: failures,
                last_error: "Exceeded maximum consecutive reader failures".to_string(),
            });
        }

        // Get current failure count for delay calculation
        let failures = self.reader_consecutive_failures();

        // Update status to Restarting
        *self.reader_status.write().await = ReaderStatus::Restarting;
        let _ = self.event_tx.send(IndiEvent::ReaderHealthChanged {
            healthy: false,
            status: ReaderStatus::Restarting,
            consecutive_failures: failures,
        });

        // Calculate and wait for delay
        if failures > 0 {
            let delay = self.reader_task_config.calculate_restart_delay(failures);
            tracing::info!(
                "Waiting {:?} before reader recovery attempt {}",
                delay,
                failures
            );
            sleep(delay).await;
        }

        // Attempt to reconnect
        match self.connect().await {
            Ok(_) => {
                tracing::info!("Reader recovery successful after {} attempts", failures);
                let _ = self.event_tx.send(IndiEvent::ReaderRestarted {
                    attempts_used: failures,
                });
                Ok(())
            }
            Err(e) => {
                tracing::error!("Reader recovery failed: {}", e);
                // Note: connect() will have already incremented the failure counter
                // and emitted appropriate events through supervised_reader_task
                Err(e)
            }
        }
    }

    /// Check if a reconnection is safe (not already in progress)
    ///
    /// Returns false if:
    /// - Already connected
    /// - Reader is in Restarting state
    /// - A reconnection attempt is in progress
    pub async fn can_reconnect(&self) -> bool {
        if self.connected.load(Ordering::SeqCst) {
            return false;
        }

        let status = *self.reader_status.read().await;
        if status == ReaderStatus::Restarting {
            return false;
        }

        let reconnect_attempts = self.reconnect_attempts.load(Ordering::SeqCst);
        reconnect_attempts == 0
    }

    /// Get the number of reconnection attempts
    pub async fn reconnect_attempts(&self) -> u32 {
        self.reconnect_attempts.load(Ordering::SeqCst)
    }

    /// Request protocol version info from server
    pub async fn request_version(&mut self) -> IndiResult<()> {
        self.send_command("<getProperties version=\"\"/>").await
    }

    /// Check if server version is compatible with minimum required version
    pub async fn check_version_compatibility(&self) -> IndiResult<()> {
        if let Some(ref min_version) = self.protocol_config.min_version {
            if let Some(server_ver) = self.server_version().await {
                if !is_version_compatible(&server_ver, min_version) {
                    return Err(IndiError::VersionMismatch {
                        required: min_version.clone(),
                        server: server_ver,
                    });
                }
            }
        }
        Ok(())
    }
}

/// Get current time in milliseconds since UNIX epoch
fn current_time_ms() -> u64 {
    use std::time::SystemTime;
    SystemTime::now()
        .duration_since(SystemTime::UNIX_EPOCH)
        .map(|d| d.as_millis() as u64)
        .unwrap_or(0)
}

/// Helper to get attribute from XML event
fn get_attribute(e: &quick_xml::events::BytesStart, name: &str) -> Option<String> {
    e.attributes()
        .filter_map(|a| a.ok())
        .find(|a| a.key.as_ref() == name.as_bytes())
        .map(|a| String::from_utf8_lossy(&a.value).to_string())
}

fn parse_state(s: &str) -> IndiPropertyState {
    match s {
        "Idle" => IndiPropertyState::Idle,
        "Ok" => IndiPropertyState::Ok,
        "Busy" => IndiPropertyState::Busy,
        "Alert" => IndiPropertyState::Alert,
        _ => IndiPropertyState::Idle,
    }
}

fn parse_perm(s: &str) -> IndiPermission {
    match s.to_lowercase().as_str() {
        "ro" => IndiPermission::ReadOnly,
        "wo" => IndiPermission::WriteOnly,
        "rw" => IndiPermission::ReadWrite,
        _ => IndiPermission::ReadWrite,
    }
}

/// Validate BLOB format and detect actual format from data
fn validate_blob_format(declared_format: &str, data: &[u8]) -> String {
    // Check magic bytes to detect actual format
    let detected: &str = if data.len() >= 6 && &data[0..6] == b"SIMPLE" {
        ".fits"
    } else if data.len() >= 8 && &data[0..8] == [0x89, b'P', b'N', b'G', 0x0D, 0x0A, 0x1A, 0x0A] {
        ".png"
    } else if data.len() >= 3 && &data[0..3] == [0xFF, 0xD8, 0xFF] {
        ".jpeg"
    } else if data.len() >= 4 && &data[0..4] == b"RIFF" && data.len() >= 12 && &data[8..12] == b"WEBP" {
        ".webp"
    } else if data.len() >= 4 && &data[0..4] == [0x1F, 0x8B, 0x08, 0x00] {
        ".gz"
    } else if data.len() >= 2 && &data[0..2] == [0x50, 0x4B] {
        ".zip"
    } else {
        // Use declared format
        declared_format
    };

    // Log warning if formats don't match
    if !declared_format.is_empty() && detected != declared_format {
        tracing::debug!(
            "BLOB format mismatch: declared '{}', detected '{}'",
            declared_format,
            detected
        );
    }

    detected.to_string()
}

/// Compare protocol versions (returns true if server >= required)
fn is_version_compatible(server: &str, required: &str) -> bool {
    let parse_version = |v: &str| -> (u32, u32) {
        let parts: Vec<&str> = v.split('.').collect();
        let major = parts.first().and_then(|s| s.parse().ok()).unwrap_or(0);
        let minor = parts.get(1).and_then(|s| s.parse().ok()).unwrap_or(0);
        (major, minor)
    };

    let (server_major, server_minor) = parse_version(server);
    let (req_major, req_minor) = parse_version(required);

    server_major > req_major || (server_major == req_major && server_minor >= req_minor)
}

impl Default for IndiClient {
    fn default() -> Self {
        Self::new("localhost", None)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::IndiPropertyState;

    #[tokio::test]
    async fn test_timeout_config_default() {
        let config = IndiTimeoutConfig::default();
        assert_eq!(config.connection_timeout_secs, 30);
        assert_eq!(config.message_timeout_secs, 60);
        assert_eq!(config.blob_timeout_secs, 300);
        assert_eq!(config.property_timeout_secs, 30);
        assert_eq!(config.mount_slew_timeout_secs, 300);
        assert_eq!(config.focuser_move_timeout_secs, 120);
        assert_eq!(config.filter_change_timeout_secs, 60);
        assert_eq!(config.dome_slew_timeout_secs, 300);
        assert_eq!(config.rotator_move_timeout_secs, 120);
        assert_eq!(config.camera_exposure_buffer_secs, 60);
        assert_eq!(config.property_poll_interval_ms, 500);
        assert_eq!(config.keepalive_interval_secs, 30);
        assert_eq!(config.reconnect_base_delay_secs, 1);
        assert_eq!(config.reconnect_max_delay_secs, 30);
        assert_eq!(config.reconnect_max_attempts, 5);
    }

    #[tokio::test]
    async fn test_client_creation_with_timeout_config() {
        let custom_config = IndiTimeoutConfig {
            connection_timeout_secs: 60,
            message_timeout_secs: 120,
            blob_timeout_secs: 600,
            property_timeout_secs: 60,
            mount_slew_timeout_secs: 600,
            focuser_move_timeout_secs: 240,
            filter_change_timeout_secs: 120,
            dome_slew_timeout_secs: 600,
            rotator_move_timeout_secs: 240,
            camera_exposure_buffer_secs: 120,
            property_poll_interval_ms: 1000,
            keepalive_interval_secs: 60,
            reconnect_base_delay_secs: 2,
            reconnect_max_delay_secs: 60,
            reconnect_max_attempts: 10,
        };

        let client =
            IndiClient::with_timeout_config("localhost", Some(7624), custom_config.clone());
        assert_eq!(client.timeout_config().mount_slew_timeout_secs, 600);
        assert_eq!(client.timeout_config().message_timeout_secs, 120);
        assert_eq!(client.timeout_config().reconnect_max_attempts, 10);
    }

    #[tokio::test]
    async fn test_timeout_error_display() {
        let error = IndiTimeoutError {
            device: "TestMount".to_string(),
            property: "EQUATORIAL_EOD_COORD".to_string(),
            context: "Slew operation exceeded timeout".to_string(),
            last_state: Some(IndiPropertyState::Busy),
        };

        let error_msg = format!("{}", error);
        assert!(error_msg.contains("TestMount"));
        assert!(error_msg.contains("EQUATORIAL_EOD_COORD"));
        assert!(error_msg.contains("Slew operation exceeded timeout"));
    }

    #[tokio::test]
    async fn test_wait_for_property_state_timeout() {
        let client = IndiClient::new("localhost", Some(7624));

        // This should timeout immediately since we're not connected
        let result = client
            .wait_for_property_state(
                "TestDevice",
                "TestProperty",
                IndiPropertyState::Ok,
                Duration::from_millis(100),
            )
            .await;

        assert!(result.is_err());
        if let Err(e) = result {
            assert_eq!(e.device, "TestDevice");
            assert_eq!(e.property, "TestProperty");
        }
    }

    #[tokio::test]
    async fn test_exponential_backoff_with_jitter() {
        let config = ReconnectionConfig {
            base_delay_secs: 1,
            max_delay_secs: 30,
            max_attempts: 5,
            use_jitter: false, // Disable jitter for predictable testing
            jitter_factor: 0.0,
        };

        // Test exponential growth without jitter
        let delay1 = config.calculate_delay(1);
        assert_eq!(delay1, Duration::from_secs(1));

        let delay2 = config.calculate_delay(2);
        assert_eq!(delay2, Duration::from_secs(2));

        let delay3 = config.calculate_delay(3);
        assert_eq!(delay3, Duration::from_secs(4));

        let delay4 = config.calculate_delay(4);
        assert_eq!(delay4, Duration::from_secs(8));

        let delay5 = config.calculate_delay(5);
        assert_eq!(delay5, Duration::from_secs(16));

        // Test capping at max
        let delay6 = config.calculate_delay(6);
        assert_eq!(delay6, Duration::from_secs(30)); // Capped at max
    }

    #[tokio::test]
    async fn test_jitter_produces_variation() {
        let config = ReconnectionConfig {
            base_delay_secs: 10,
            max_delay_secs: 100,
            max_attempts: 5,
            use_jitter: true,
            jitter_factor: 0.3,
        };

        // With jitter, delays should vary somewhat
        let delay1 = config.calculate_delay(1);
        let delay2 = config.calculate_delay(1);

        // Both should be close to 10 seconds (within 30% jitter)
        assert!(delay1.as_secs_f64() >= 8.5 && delay1.as_secs_f64() <= 11.5);
        assert!(delay2.as_secs_f64() >= 8.5 && delay2.as_secs_f64() <= 11.5);
    }

    #[tokio::test]
    async fn test_reconnect_attempts_tracking() {
        let client = IndiClient::new("localhost", Some(7624));
        assert_eq!(client.reconnect_attempts().await, 0);
    }

    #[tokio::test]
    async fn test_send_command_error_messages() {
        let mut client = IndiClient::new("localhost", Some(7624));

        // Try to send without connecting
        let result = client.send_command("<getProperties/>").await;
        assert!(result.is_err());
        if let Err(e) = result {
            assert!(matches!(e, IndiError::NotConnected));
        }
    }

    #[tokio::test]
    async fn test_timeout_config_modification() {
        let mut client = IndiClient::new("localhost", Some(7624));

        // Check default
        assert_eq!(client.timeout_config().mount_slew_timeout_secs, 300);

        // Modify
        let mut new_config = client.timeout_config().clone();
        new_config.mount_slew_timeout_secs = 600;
        client.set_timeout_config(new_config);

        // Verify change
        assert_eq!(client.timeout_config().mount_slew_timeout_secs, 600);
    }

    #[tokio::test]
    async fn test_property_state_alert_detection() {
        let client = IndiClient::new("localhost", Some(7624));

        let result = client
            .wait_for_property_not_busy("TestDevice", "TestProperty", Duration::from_millis(100))
            .await;

        // Should timeout since device doesn't exist
        assert!(result.is_err());
    }

    #[tokio::test]
    async fn test_client_default_creation() {
        let client = IndiClient::default();
        assert_eq!(client.host, "localhost");
        assert_eq!(client.port, INDI_DEFAULT_PORT);
    }

    #[test]
    fn test_version_compatibility() {
        assert!(is_version_compatible("1.7", "1.7"));
        assert!(is_version_compatible("1.8", "1.7"));
        assert!(is_version_compatible("1.9", "1.7"));
        assert!(is_version_compatible("2.0", "1.7"));
        assert!(!is_version_compatible("1.6", "1.7"));
        assert!(!is_version_compatible("1.0", "1.7"));
    }

    #[test]
    fn test_blob_format_validation() {
        // FITS format detection
        let fits_data = b"SIMPLE  =                    T";
        assert_eq!(validate_blob_format(".fits", fits_data), ".fits");

        // PNG format detection
        let png_data = [0x89, b'P', b'N', b'G', 0x0D, 0x0A, 0x1A, 0x0A, 0, 0];
        assert_eq!(validate_blob_format(".fits", &png_data), ".png");

        // JPEG format detection
        let jpeg_data = [0xFF, 0xD8, 0xFF, 0xE0, 0, 0];
        assert_eq!(validate_blob_format(".fits", &jpeg_data), ".jpeg");

        // Unknown format uses declared
        let unknown_data = [0x00, 0x01, 0x02, 0x03];
        assert_eq!(validate_blob_format(".raw", &unknown_data), ".raw");
    }

    #[tokio::test]
    async fn test_protocol_config() {
        let protocol_config = ProtocolConfig {
            preferred_version: "1.8".to_string(),
            auto_detect: true,
            min_version: Some("1.7".to_string()),
        };

        let client = IndiClient::with_full_config(
            "localhost",
            Some(7624),
            IndiTimeoutConfig::default(),
            protocol_config.clone(),
            ReconnectionConfig::default(),
        );

        assert_eq!(client.protocol_config().preferred_version, "1.8");
        assert!(client.protocol_config().auto_detect);
    }

    #[tokio::test]
    async fn test_number_limits() {
        let client = IndiClient::new("localhost", Some(7624));

        // Should return None for non-existent property
        let limits = client
            .get_number_limits("TestDevice", "TestProperty", "TestElement")
            .await;
        assert!(limits.is_none());
    }

    #[tokio::test]
    async fn test_reader_status() {
        let client = IndiClient::new("localhost", Some(7624));

        // Should be stopped initially
        let status = client.reader_status().await;
        assert_eq!(status, ReaderStatus::Stopped);
    }

    // =========================================================================
    // Reader Supervision Tests
    // =========================================================================

    #[tokio::test]
    async fn test_reader_task_config_default() {
        let config = ReaderTaskConfig::default();
        assert_eq!(config.max_consecutive_failures, 5);
        assert_eq!(config.restart_base_delay_secs, 1);
        assert_eq!(config.restart_max_delay_secs, 60);
        assert!(config.auto_restart);
        assert!(config.use_jitter);
        assert!((config.jitter_factor - 0.3).abs() < 0.01);
    }

    #[tokio::test]
    async fn test_reader_task_config_delay_calculation() {
        let config = ReaderTaskConfig {
            max_consecutive_failures: 5,
            restart_base_delay_secs: 1,
            restart_max_delay_secs: 60,
            auto_restart: true,
            use_jitter: false, // Disable jitter for predictable testing
            jitter_factor: 0.0,
        };

        // Test exponential growth
        assert_eq!(config.calculate_restart_delay(1), Duration::from_secs(1));
        assert_eq!(config.calculate_restart_delay(2), Duration::from_secs(2));
        assert_eq!(config.calculate_restart_delay(3), Duration::from_secs(4));
        assert_eq!(config.calculate_restart_delay(4), Duration::from_secs(8));
        assert_eq!(config.calculate_restart_delay(5), Duration::from_secs(16));
        assert_eq!(config.calculate_restart_delay(6), Duration::from_secs(32));
        // Should cap at max
        assert_eq!(config.calculate_restart_delay(7), Duration::from_secs(60));
        assert_eq!(config.calculate_restart_delay(10), Duration::from_secs(60));
    }

    #[tokio::test]
    async fn test_reader_task_config_with_jitter() {
        let config = ReaderTaskConfig {
            max_consecutive_failures: 5,
            restart_base_delay_secs: 10,
            restart_max_delay_secs: 100,
            auto_restart: true,
            use_jitter: true,
            jitter_factor: 0.3,
        };

        // With 30% jitter, delay should be within +/- 15% of base
        let delay = config.calculate_restart_delay(1);
        let expected = 10.0;
        let tolerance = expected * 0.15;
        assert!(
            delay.as_secs_f64() >= expected - tolerance && delay.as_secs_f64() <= expected + tolerance,
            "Delay {} not within expected range [{}, {}]",
            delay.as_secs_f64(),
            expected - tolerance,
            expected + tolerance
        );
    }

    #[tokio::test]
    async fn test_is_reader_healthy_initial_state() {
        let client = IndiClient::new("localhost", Some(7624));

        // Initially not connected, so not healthy
        assert!(!client.is_reader_healthy());
        assert_eq!(client.reader_consecutive_failures(), 0);
        assert!(!client.is_reader_failed_permanently());
    }

    #[tokio::test]
    async fn test_reader_consecutive_failures_tracking() {
        let client = IndiClient::new("localhost", Some(7624));

        // Initially zero
        assert_eq!(client.reader_consecutive_failures(), 0);

        // Simulate failures (normally done by supervised_reader_task)
        client.reader_consecutive_failures.store(3, Ordering::SeqCst);
        assert_eq!(client.reader_consecutive_failures(), 3);

        // Reset
        client.reset_reader_failures();
        assert_eq!(client.reader_consecutive_failures(), 0);
    }

    #[tokio::test]
    async fn test_is_reader_failed_permanently() {
        let client = IndiClient::new("localhost", Some(7624));
        let max_failures = client.reader_task_config().max_consecutive_failures;

        // Not failed initially
        assert!(!client.is_reader_failed_permanently());

        // Simulate failures below threshold
        client.reader_consecutive_failures.store(max_failures - 1, Ordering::SeqCst);
        assert!(!client.is_reader_failed_permanently());

        // At threshold
        client.reader_consecutive_failures.store(max_failures, Ordering::SeqCst);
        assert!(client.is_reader_failed_permanently());

        // Above threshold
        client.reader_consecutive_failures.store(max_failures + 1, Ordering::SeqCst);
        assert!(client.is_reader_failed_permanently());
    }

    #[tokio::test]
    async fn test_can_reconnect_initial_state() {
        let client = IndiClient::new("localhost", Some(7624));

        // Initially not connected and not restarting, so can reconnect
        assert!(client.can_reconnect().await);
    }

    #[tokio::test]
    async fn test_can_reconnect_when_restarting() {
        let client = IndiClient::new("localhost", Some(7624));

        // Set status to Restarting
        *client.reader_status.write().await = ReaderStatus::Restarting;

        // Should not be able to reconnect while restarting
        assert!(!client.can_reconnect().await);
    }

    #[tokio::test]
    async fn test_can_reconnect_when_connected() {
        let client = IndiClient::new("localhost", Some(7624));

        // Simulate connected state
        client.connected.store(true, Ordering::SeqCst);

        // Should not be able to reconnect when already connected
        assert!(!client.can_reconnect().await);
    }

    #[tokio::test]
    async fn test_reader_status_enum_values() {
        // Test that all enum variants exist and are distinct
        let running = ReaderStatus::Running;
        let stopped = ReaderStatus::Stopped;
        let crashed = ReaderStatus::Crashed;
        let restarting = ReaderStatus::Restarting;

        assert_ne!(running, stopped);
        assert_ne!(running, crashed);
        assert_ne!(running, restarting);
        assert_ne!(stopped, crashed);
        assert_ne!(stopped, restarting);
        assert_ne!(crashed, restarting);
    }

    #[tokio::test]
    async fn test_reader_task_config_getter_setter() {
        let mut client = IndiClient::new("localhost", Some(7624));

        // Check default
        assert_eq!(client.reader_task_config().max_consecutive_failures, 5);

        // Modify
        let mut new_config = client.reader_task_config().clone();
        new_config.max_consecutive_failures = 10;
        new_config.auto_restart = false;
        client.set_reader_task_config(new_config);

        // Verify change
        assert_eq!(client.reader_task_config().max_consecutive_failures, 10);
        assert!(!client.reader_task_config().auto_restart);
    }

    #[tokio::test]
    async fn test_with_all_config_constructor() {
        let timeout_config = IndiTimeoutConfig::default();
        let protocol_config = ProtocolConfig::default();
        let reconnection_config = ReconnectionConfig::default();
        let reader_task_config = ReaderTaskConfig {
            max_consecutive_failures: 10,
            restart_base_delay_secs: 2,
            restart_max_delay_secs: 120,
            auto_restart: false,
            use_jitter: false,
            jitter_factor: 0.0,
        };

        let client = IndiClient::with_all_config(
            "192.168.1.100",
            Some(7625),
            timeout_config,
            protocol_config,
            reconnection_config,
            reader_task_config,
        );

        assert_eq!(client.host, "192.168.1.100");
        assert_eq!(client.port, 7625);
        assert_eq!(client.reader_task_config().max_consecutive_failures, 10);
        assert!(!client.reader_task_config().auto_restart);
    }

    #[tokio::test]
    async fn test_recover_reader_when_failed_permanently() {
        let mut client = IndiClient::new("localhost", Some(7624));
        let max_failures = client.reader_task_config().max_consecutive_failures;

        // Simulate exceeding max failures
        client.reader_consecutive_failures.store(max_failures, Ordering::SeqCst);

        // Recovery should fail
        let result = client.recover_reader().await;
        assert!(result.is_err());
        if let Err(IndiError::ReconnectionFailed { attempts, last_error }) = result {
            assert_eq!(attempts, max_failures);
            assert!(last_error.contains("Exceeded maximum"));
        } else {
            panic!("Expected ReconnectionFailed error");
        }
    }

    #[tokio::test]
    async fn test_disconnect_resets_failure_counter() {
        let mut client = IndiClient::new("localhost", Some(7624));

        // Simulate some failures
        client.reader_consecutive_failures.store(3, Ordering::SeqCst);
        assert_eq!(client.reader_consecutive_failures(), 3);

        // Disconnect should reset
        let _ = client.disconnect().await;
        assert_eq!(client.reader_consecutive_failures(), 0);
    }
}
