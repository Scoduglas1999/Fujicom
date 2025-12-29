//! Alpaca HTTP Client

use crate::{AlpacaDevice, AlpacaDeviceType};
use reqwest::Client;
use serde::Deserialize;
use std::sync::atomic::{AtomicU32, Ordering};
use std::time::Duration;
use thiserror::Error;
use tracing::{debug, warn};

/// Client ID for Alpaca API calls (thread-safe)
static CLIENT_ID: AtomicU32 = AtomicU32::new(1);
static TRANSACTION_ID: AtomicU32 = AtomicU32::new(0);

/// Alpaca-specific error types
#[derive(Debug, Error)]
pub enum AlpacaError {
    #[error("Connection timeout after {duration_ms}ms during {operation}")]
    Timeout {
        operation: String,
        duration_ms: u64,
    },

    #[error("Connection refused: {url} - {cause}")]
    ConnectionRefused {
        url: String,
        cause: String,
    },

    #[error("HTTP error {status}: {message}")]
    HttpError { status: u16, message: String },

    #[error("Device error {code}: {message}")]
    DeviceError { code: i32, message: String },

    #[error("Parse error: {0}")]
    ParseError(String),

    #[error("Not connected")]
    NotConnected,

    #[error("Operation failed: {0}")]
    OperationFailed(String),

    #[error("Request failed: {0}")]
    RequestFailed(String),

    #[error("API version not supported: {0}")]
    UnsupportedApiVersion(String),

    #[error("Connection validation failed: {0}")]
    ValidationFailed(String),

    #[error("Retry exhausted after {attempts} attempts: {last_error}")]
    RetryExhausted { attempts: u32, last_error: String },
}

impl AlpacaError {
    /// Create a timeout error with operation context
    pub fn timeout(operation: impl Into<String>, duration_ms: u64) -> Self {
        AlpacaError::Timeout {
            operation: operation.into(),
            duration_ms,
        }
    }

    /// Create a connection refused error
    pub fn connection_refused(url: impl Into<String>, cause: impl Into<String>) -> Self {
        AlpacaError::ConnectionRefused {
            url: url.into(),
            cause: cause.into(),
        }
    }

    /// Check if this error is retryable
    pub fn is_retryable(&self) -> bool {
        match self {
            AlpacaError::Timeout { .. } => true,
            AlpacaError::ConnectionRefused { .. } => true,
            AlpacaError::HttpError { status, .. } => {
                // Retry on 5xx server errors and 429 rate limiting
                *status >= 500 || *status == 429
            }
            AlpacaError::RequestFailed(_) => true,
            // Don't retry device errors, parse errors, validation failures
            AlpacaError::DeviceError { .. } => false,
            AlpacaError::ParseError(_) => false,
            AlpacaError::NotConnected => false,
            AlpacaError::OperationFailed(_) => false,
            AlpacaError::UnsupportedApiVersion(_) => false,
            AlpacaError::ValidationFailed(_) => false,
            AlpacaError::RetryExhausted { .. } => false,
        }
    }
}

impl From<reqwest::Error> for AlpacaError {
    fn from(err: reqwest::Error) -> Self {
        if err.is_timeout() {
            AlpacaError::Timeout {
                operation: "HTTP request".to_string(),
                duration_ms: 30000, // Default timeout - actual tracked in specific methods
            }
        } else if err.is_connect() {
            let url = err.url()
                .map(|u| u.to_string())
                .unwrap_or_else(|| "unknown".to_string());
            AlpacaError::ConnectionRefused {
                url,
                cause: err.to_string(),
            }
        } else if let Some(status) = err.status() {
            AlpacaError::HttpError {
                status: status.as_u16(),
                message: err.to_string(),
            }
        } else {
            AlpacaError::RequestFailed(err.to_string())
        }
    }
}

impl From<serde_json::Error> for AlpacaError {
    fn from(err: serde_json::Error) -> Self {
        AlpacaError::ParseError(err.to_string())
    }
}

// Backward compatibility: convert AlpacaError to String for existing code
impl From<AlpacaError> for String {
    fn from(err: AlpacaError) -> Self {
        err.to_string()
    }
}

pub fn get_client_transaction() -> (u32, u32) {
    let client_id = CLIENT_ID.load(Ordering::SeqCst);
    let transaction_id = TRANSACTION_ID.fetch_add(1, Ordering::SeqCst);
    (client_id, transaction_id)
}

/// Get the current client ID
pub fn get_client_id() -> u32 {
    CLIENT_ID.load(Ordering::SeqCst)
}

/// Set the client ID
pub fn set_client_id(id: u32) {
    CLIENT_ID.store(id, Ordering::SeqCst);
}

/// Timeout configuration for different operation types
#[derive(Debug, Clone)]
pub struct TimeoutConfig {
    /// Timeout for quick status queries (e.g., is_connected, position)
    pub quick_query_ms: u64,
    /// Timeout for standard operations (e.g., filter change, short moves)
    pub standard_operation_ms: u64,
    /// Timeout for long operations (e.g., image download, parking, slewing)
    pub long_operation_ms: u64,
    /// Timeout for very long operations (e.g., large image downloads, dome rotation)
    pub very_long_operation_ms: u64,
    /// Connection timeout
    pub connect_ms: u64,
}

impl Default for TimeoutConfig {
    fn default() -> Self {
        Self {
            quick_query_ms: 5000,           // 5 seconds for quick queries
            standard_operation_ms: 30000,    // 30 seconds for standard operations
            long_operation_ms: 300000,       // 5 minutes for long operations
            very_long_operation_ms: 600000,  // 10 minutes for very long operations
            connect_ms: 10000,               // 10 seconds for initial connection
        }
    }
}

impl TimeoutConfig {
    /// Create timeout config optimized for camera operations
    /// Cameras need longer timeouts for image downloads
    pub fn for_camera() -> Self {
        Self {
            quick_query_ms: 5000,
            standard_operation_ms: 30000,
            long_operation_ms: 300000,       // 5 minutes for image download
            very_long_operation_ms: 900000,  // 15 minutes for very large images
            connect_ms: 15000,
        }
    }

    /// Create timeout config optimized for telescope/mount operations
    /// Mounts need longer timeouts for slewing across the sky
    pub fn for_telescope() -> Self {
        Self {
            quick_query_ms: 5000,
            standard_operation_ms: 60000,    // 1 minute for sync operations
            long_operation_ms: 300000,       // 5 minutes for slewing
            very_long_operation_ms: 600000,  // 10 minutes for parking/homing
            connect_ms: 15000,
        }
    }

    /// Create timeout config optimized for dome operations
    /// Domes can take a long time to rotate and operate shutters
    pub fn for_dome() -> Self {
        Self {
            quick_query_ms: 5000,
            standard_operation_ms: 60000,    // 1 minute for status queries
            long_operation_ms: 300000,       // 5 minutes for shutter operations
            very_long_operation_ms: 600000,  // 10 minutes for full rotation
            connect_ms: 15000,
        }
    }

    /// Create timeout config optimized for focuser operations
    pub fn for_focuser() -> Self {
        Self {
            quick_query_ms: 5000,
            standard_operation_ms: 30000,
            long_operation_ms: 120000,       // 2 minutes for long focus moves
            very_long_operation_ms: 300000,  // 5 minutes for full travel
            connect_ms: 10000,
        }
    }

    /// Create timeout config optimized for filter wheel operations
    pub fn for_filter_wheel() -> Self {
        Self {
            quick_query_ms: 5000,
            standard_operation_ms: 30000,    // 30 seconds for filter changes
            long_operation_ms: 60000,        // 1 minute maximum
            very_long_operation_ms: 120000,  // 2 minutes for slow wheels
            connect_ms: 10000,
        }
    }

    /// Create timeout config optimized for rotator operations
    pub fn for_rotator() -> Self {
        Self {
            quick_query_ms: 5000,
            standard_operation_ms: 30000,
            long_operation_ms: 120000,       // 2 minutes for 180-degree rotation
            very_long_operation_ms: 300000,  // 5 minutes for slow rotators
            connect_ms: 10000,
        }
    }

    /// Create timeout config for discovery operations
    pub fn for_discovery() -> Self {
        Self {
            quick_query_ms: 2000,
            standard_operation_ms: 5000,
            long_operation_ms: 10000,
            very_long_operation_ms: 15000,
            connect_ms: 5000,
        }
    }
}

/// Retry configuration for failed requests
#[derive(Debug, Clone)]
pub struct RetryConfig {
    /// Maximum number of retry attempts
    pub max_attempts: u32,
    /// Initial delay between retries in milliseconds
    pub initial_delay_ms: u64,
    /// Maximum delay between retries in milliseconds
    pub max_delay_ms: u64,
    /// Multiplier for exponential backoff (e.g., 2.0 doubles the delay each time)
    pub backoff_multiplier: f64,
    /// Whether to add jitter to retry delays
    pub use_jitter: bool,
}

impl Default for RetryConfig {
    fn default() -> Self {
        Self {
            max_attempts: 3,
            initial_delay_ms: 100,
            max_delay_ms: 5000,
            backoff_multiplier: 2.0,
            use_jitter: true,
        }
    }
}

impl RetryConfig {
    /// Calculate the delay for a given attempt number (0-indexed)
    pub fn delay_for_attempt(&self, attempt: u32) -> Duration {
        let base_delay = self.initial_delay_ms as f64
            * self.backoff_multiplier.powi(attempt as i32);
        let capped_delay = base_delay.min(self.max_delay_ms as f64);

        let final_delay = if self.use_jitter {
            // Add +/- 25% jitter
            let jitter_factor = 0.75 + (rand_simple() * 0.5);
            capped_delay * jitter_factor
        } else {
            capped_delay
        };

        Duration::from_millis(final_delay as u64)
    }

    /// Create a config with no retries
    pub fn no_retry() -> Self {
        Self {
            max_attempts: 1,
            ..Default::default()
        }
    }
}

/// Simple pseudo-random number generator for jitter (0.0 to 1.0)
fn rand_simple() -> f64 {
    use std::time::{SystemTime, UNIX_EPOCH};
    let nanos = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .unwrap_or_default()
        .subsec_nanos();
    (nanos as f64 / u32::MAX as f64).fract()
}

/// Alpaca API version
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ApiVersion {
    V1,
}

impl ApiVersion {
    pub fn as_str(&self) -> &'static str {
        match self {
            ApiVersion::V1 => "v1",
        }
    }
}

/// Alpaca API response wrapper
#[derive(Debug, Deserialize)]
#[serde(rename_all = "PascalCase")]
pub struct AlpacaResponse<T> {
    pub value: T,
    pub client_transaction_id: u32,
    pub server_transaction_id: u32,
    pub error_number: i32,
    pub error_message: String,
}

/// Server API information response
#[derive(Debug, Deserialize)]
#[serde(rename_all = "PascalCase")]
pub struct ApiVersionsResponse {
    pub value: Vec<u32>,
}

/// Alpaca client for communicating with a device
pub struct AlpacaClient {
    http_client: Client,
    base_url: String,
    device_type: AlpacaDeviceType,
    device_number: u32,
    timeout_config: TimeoutConfig,
    retry_config: RetryConfig,
    api_version: ApiVersion,
}

impl AlpacaClient {
    /// Create a new Alpaca client for a device with default configuration
    pub fn new(device: &AlpacaDevice) -> Self {
        Self::with_config(device, TimeoutConfig::default(), RetryConfig::default())
    }

    /// Create a new Alpaca client with custom timeout and retry configuration
    pub fn with_config(
        device: &AlpacaDevice,
        timeout_config: TimeoutConfig,
        retry_config: RetryConfig,
    ) -> Self {
        let http_client = Client::builder()
            .timeout(Duration::from_millis(timeout_config.standard_operation_ms))
            .connect_timeout(Duration::from_millis(timeout_config.connect_ms))
            .pool_idle_timeout(Duration::from_secs(30))
            .build()
            .expect("Failed to create HTTP client");

        Self {
            http_client,
            base_url: device.base_url.clone(),
            device_type: device.device_type,
            device_number: device.device_number,
            timeout_config,
            retry_config,
            api_version: ApiVersion::V1,
        }
    }

    /// Get the base URL for this client
    pub fn base_url(&self) -> &str {
        &self.base_url
    }

    /// Get the device number for this client
    pub fn device_number(&self) -> u32 {
        self.device_number
    }

    /// Get the timeout configuration
    pub fn timeout_config(&self) -> &TimeoutConfig {
        &self.timeout_config
    }

    /// Get the retry configuration
    pub fn retry_config(&self) -> &RetryConfig {
        &self.retry_config
    }

    /// Get the current API version being used
    pub fn api_version(&self) -> ApiVersion {
        self.api_version
    }

    /// Build the URL for an API endpoint
    fn build_url(&self, endpoint: &str) -> String {
        format!(
            "{}/api/{}/{}/{}/{}",
            self.base_url,
            self.api_version.as_str(),
            self.device_type.as_str(),
            self.device_number,
            endpoint
        )
    }

    /// Create a client with a specific timeout for long operations
    fn create_long_timeout_client(&self) -> Result<Client, AlpacaError> {
        Client::builder()
            .timeout(Duration::from_millis(self.timeout_config.long_operation_ms))
            .connect_timeout(Duration::from_millis(self.timeout_config.connect_ms))
            .build()
            .map_err(|e| AlpacaError::RequestFailed(e.to_string()))
    }

    /// Create a client with a specific timeout for quick queries
    fn create_quick_timeout_client(&self) -> Result<Client, AlpacaError> {
        Client::builder()
            .timeout(Duration::from_millis(self.timeout_config.quick_query_ms))
            .connect_timeout(Duration::from_millis(self.timeout_config.connect_ms))
            .build()
            .map_err(|e| AlpacaError::RequestFailed(e.to_string()))
    }

    /// Create a client with a specific timeout for very long operations
    fn create_very_long_timeout_client(&self) -> Result<Client, AlpacaError> {
        Client::builder()
            .timeout(Duration::from_millis(self.timeout_config.very_long_operation_ms))
            .connect_timeout(Duration::from_millis(self.timeout_config.connect_ms))
            .build()
            .map_err(|e| AlpacaError::RequestFailed(e.to_string()))
    }

    /// Create a client with a custom timeout value in milliseconds
    fn create_custom_timeout_client(&self, timeout_ms: u64) -> Result<Client, AlpacaError> {
        Client::builder()
            .timeout(Duration::from_millis(timeout_ms))
            .connect_timeout(Duration::from_millis(self.timeout_config.connect_ms))
            .build()
            .map_err(|e| AlpacaError::RequestFailed(e.to_string()))
    }

    /// Execute a request with retry logic
    async fn execute_with_retry<F, Fut, T>(&self, operation: F) -> Result<T, AlpacaError>
    where
        F: Fn() -> Fut,
        Fut: std::future::Future<Output = Result<T, AlpacaError>>,
    {
        let mut last_error = AlpacaError::OperationFailed("No attempts made".to_string());

        for attempt in 0..self.retry_config.max_attempts {
            match operation().await {
                Ok(result) => return Ok(result),
                Err(e) => {
                    last_error = e;

                    // Check if the error is retryable using the is_retryable method
                    if !last_error.is_retryable() {
                        return Err(last_error);
                    }

                    // If not the last attempt, wait before retrying
                    if attempt + 1 < self.retry_config.max_attempts {
                        let delay = self.retry_config.delay_for_attempt(attempt);
                        debug!(
                            "Request failed (attempt {}/{}), retrying in {:?}: {}",
                            attempt + 1,
                            self.retry_config.max_attempts,
                            delay,
                            last_error
                        );
                        tokio::time::sleep(delay).await;
                    }
                }
            }
        }

        Err(AlpacaError::RetryExhausted {
            attempts: self.retry_config.max_attempts,
            last_error: last_error.to_string(),
        })
    }

    /// Make a GET request with typed error handling
    pub async fn get_typed<T: for<'de> Deserialize<'de>>(&self, endpoint: &str) -> Result<T, AlpacaError> {
        let endpoint = endpoint.to_string();
        self.execute_with_retry(|| {
            let endpoint = endpoint.clone();
            async move {
                let (client_id, transaction_id) = get_client_transaction();
                let url = format!(
                    "{}?ClientID={}&ClientTransactionID={}",
                    self.build_url(&endpoint),
                    client_id,
                    transaction_id
                );

                let response = self.http_client
                    .get(&url)
                    .send()
                    .await?;

                let status = response.status();
                if !status.is_success() {
                    let body = response.text().await.unwrap_or_default();
                    return Err(AlpacaError::HttpError {
                        status: status.as_u16(),
                        message: body,
                    });
                }

                let alpaca_response: AlpacaResponse<T> = response.json().await?;

                if alpaca_response.error_number != 0 {
                    return Err(AlpacaError::DeviceError {
                        code: alpaca_response.error_number,
                        message: alpaca_response.error_message,
                    });
                }

                Ok(alpaca_response.value)
            }
        }).await
    }

    /// Make a GET request (backward compatible with String errors)
    pub async fn get<T: for<'de> Deserialize<'de>>(&self, endpoint: &str) -> Result<T, String> {
        self.get_typed(endpoint).await.map_err(|e| e.to_string())
    }

    /// Make a PUT request with typed error handling
    pub async fn put_typed<T: for<'de> Deserialize<'de>>(
        &self,
        endpoint: &str,
        params: &[(&str, &str)],
    ) -> Result<T, AlpacaError> {
        let endpoint = endpoint.to_string();
        let params: Vec<(String, String)> = params
            .iter()
            .map(|(k, v)| (k.to_string(), v.to_string()))
            .collect();

        self.execute_with_retry(|| {
            let endpoint = endpoint.clone();
            let params = params.clone();
            async move {
                let (client_id, transaction_id) = get_client_transaction();
                let url = self.build_url(&endpoint);

                let mut form_params: Vec<(&str, String)> = vec![
                    ("ClientID", client_id.to_string()),
                    ("ClientTransactionID", transaction_id.to_string()),
                ];

                for (key, value) in &params {
                    form_params.push((key.as_str(), value.clone()));
                }

                let response = self.http_client
                    .put(&url)
                    .form(&form_params)
                    .send()
                    .await?;

                let status = response.status();
                if !status.is_success() {
                    let body = response.text().await.unwrap_or_default();
                    return Err(AlpacaError::HttpError {
                        status: status.as_u16(),
                        message: body,
                    });
                }

                let alpaca_response: AlpacaResponse<T> = response.json().await?;

                if alpaca_response.error_number != 0 {
                    return Err(AlpacaError::DeviceError {
                        code: alpaca_response.error_number,
                        message: alpaca_response.error_message,
                    });
                }

                Ok(alpaca_response.value)
            }
        }).await
    }

    /// Make a PUT request (backward compatible with String errors)
    pub async fn put<T: for<'de> Deserialize<'de>>(
        &self,
        endpoint: &str,
        params: &[(&str, &str)],
    ) -> Result<T, String> {
        self.put_typed(endpoint, params).await.map_err(|e| e.to_string())
    }

    /// Make a quick GET request with shorter timeout (no retry)
    pub async fn get_quick<T: for<'de> Deserialize<'de>>(&self, endpoint: &str) -> Result<T, AlpacaError> {
        let client = self.create_quick_timeout_client()?;
        let (client_id, transaction_id) = get_client_transaction();
        let url = format!(
            "{}?ClientID={}&ClientTransactionID={}",
            self.build_url(endpoint),
            client_id,
            transaction_id
        );

        let response = client
            .get(&url)
            .send()
            .await?;

        let status = response.status();
        if !status.is_success() {
            let body = response.text().await.unwrap_or_default();
            return Err(AlpacaError::HttpError {
                status: status.as_u16(),
                message: body,
            });
        }

        let alpaca_response: AlpacaResponse<T> = response.json().await?;

        if alpaca_response.error_number != 0 {
            return Err(AlpacaError::DeviceError {
                code: alpaca_response.error_number,
                message: alpaca_response.error_message,
            });
        }

        Ok(alpaca_response.value)
    }

    /// Make a long-running GET request with extended timeout
    pub async fn get_long<T: for<'de> Deserialize<'de>>(&self, endpoint: &str) -> Result<T, AlpacaError> {
        let client = self.create_long_timeout_client()?;
        let (client_id, transaction_id) = get_client_transaction();
        let url = format!(
            "{}?ClientID={}&ClientTransactionID={}",
            self.build_url(endpoint),
            client_id,
            transaction_id
        );

        let response = client
            .get(&url)
            .send()
            .await?;

        let status = response.status();
        if !status.is_success() {
            let body = response.text().await.unwrap_or_default();
            return Err(AlpacaError::HttpError {
                status: status.as_u16(),
                message: body,
            });
        }

        let alpaca_response: AlpacaResponse<T> = response.json().await?;

        if alpaca_response.error_number != 0 {
            return Err(AlpacaError::DeviceError {
                code: alpaca_response.error_number,
                message: alpaca_response.error_message,
            });
        }

        Ok(alpaca_response.value)
    }

    /// Make a long-running PUT request with extended timeout
    /// Use for operations like slewing, parking, and shutter control
    pub async fn put_long<T: for<'de> Deserialize<'de>>(
        &self,
        endpoint: &str,
        params: &[(&str, &str)],
    ) -> Result<T, AlpacaError> {
        let client = self.create_long_timeout_client()?;
        let (client_id, transaction_id) = get_client_transaction();
        let url = self.build_url(endpoint);

        let mut form_params: Vec<(&str, String)> = vec![
            ("ClientID", client_id.to_string()),
            ("ClientTransactionID", transaction_id.to_string()),
        ];

        for (key, value) in params {
            form_params.push((key, value.to_string()));
        }

        let response = client
            .put(&url)
            .form(&form_params)
            .send()
            .await
            .map_err(|e| {
                if e.is_timeout() {
                    AlpacaError::timeout(endpoint, self.timeout_config.long_operation_ms)
                } else {
                    e.into()
                }
            })?;

        let status = response.status();
        if !status.is_success() {
            let body = response.text().await.unwrap_or_default();
            return Err(AlpacaError::HttpError {
                status: status.as_u16(),
                message: body,
            });
        }

        let alpaca_response: AlpacaResponse<T> = response.json().await?;

        if alpaca_response.error_number != 0 {
            return Err(AlpacaError::DeviceError {
                code: alpaca_response.error_number,
                message: alpaca_response.error_message,
            });
        }

        Ok(alpaca_response.value)
    }

    /// Make a very long-running PUT request with extended timeout
    /// Use for operations like full dome rotation, image downloads, etc.
    pub async fn put_very_long<T: for<'de> Deserialize<'de>>(
        &self,
        endpoint: &str,
        params: &[(&str, &str)],
    ) -> Result<T, AlpacaError> {
        let client = self.create_very_long_timeout_client()?;
        let (client_id, transaction_id) = get_client_transaction();
        let url = self.build_url(endpoint);

        let mut form_params: Vec<(&str, String)> = vec![
            ("ClientID", client_id.to_string()),
            ("ClientTransactionID", transaction_id.to_string()),
        ];

        for (key, value) in params {
            form_params.push((key, value.to_string()));
        }

        let response = client
            .put(&url)
            .form(&form_params)
            .send()
            .await
            .map_err(|e| {
                if e.is_timeout() {
                    AlpacaError::timeout(endpoint, self.timeout_config.very_long_operation_ms)
                } else {
                    e.into()
                }
            })?;

        let status = response.status();
        if !status.is_success() {
            let body = response.text().await.unwrap_or_default();
            return Err(AlpacaError::HttpError {
                status: status.as_u16(),
                message: body,
            });
        }

        let alpaca_response: AlpacaResponse<T> = response.json().await?;

        if alpaca_response.error_number != 0 {
            return Err(AlpacaError::DeviceError {
                code: alpaca_response.error_number,
                message: alpaca_response.error_message,
            });
        }

        Ok(alpaca_response.value)
    }

    /// Make a very long-running GET request with extended timeout
    /// Use for operations like large image downloads
    pub async fn get_very_long<T: for<'de> Deserialize<'de>>(&self, endpoint: &str) -> Result<T, AlpacaError> {
        let client = self.create_very_long_timeout_client()?;
        let (client_id, transaction_id) = get_client_transaction();
        let url = format!(
            "{}?ClientID={}&ClientTransactionID={}",
            self.build_url(endpoint),
            client_id,
            transaction_id
        );

        let response = client
            .get(&url)
            .send()
            .await
            .map_err(|e| {
                if e.is_timeout() {
                    AlpacaError::timeout(endpoint, self.timeout_config.very_long_operation_ms)
                } else {
                    e.into()
                }
            })?;

        let status = response.status();
        if !status.is_success() {
            let body = response.text().await.unwrap_or_default();
            return Err(AlpacaError::HttpError {
                status: status.as_u16(),
                message: body,
            });
        }

        let alpaca_response: AlpacaResponse<T> = response.json().await?;

        if alpaca_response.error_number != 0 {
            return Err(AlpacaError::DeviceError {
                code: alpaca_response.error_number,
                message: alpaca_response.error_message,
            });
        }

        Ok(alpaca_response.value)
    }

    // Common device properties

    /// Check if the device is connected
    pub async fn is_connected(&self) -> Result<bool, String> {
        self.get("connected").await
    }

    /// Check if the device is connected (typed error)
    pub async fn is_connected_typed(&self) -> Result<bool, AlpacaError> {
        self.get_quick("connected").await
    }

    /// Connect to the device
    pub async fn connect(&self) -> Result<(), String> {
        self.put::<()>("connected", &[("Connected", "true")]).await
    }

    /// Connect to the device (typed error)
    pub async fn connect_typed(&self) -> Result<(), AlpacaError> {
        self.put_typed::<()>("connected", &[("Connected", "true")]).await
    }

    /// Disconnect from the device
    pub async fn disconnect(&self) -> Result<(), String> {
        self.put::<()>("connected", &[("Connected", "false")]).await
    }

    /// Disconnect from the device (typed error)
    pub async fn disconnect_typed(&self) -> Result<(), AlpacaError> {
        self.put_typed::<()>("connected", &[("Connected", "false")]).await
    }

    /// Get the device name
    pub async fn get_name(&self) -> Result<String, String> {
        self.get("name").await
    }

    /// Get the device name (typed error)
    pub async fn get_name_typed(&self) -> Result<String, AlpacaError> {
        self.get_typed("name").await
    }

    /// Get the device description
    pub async fn get_description(&self) -> Result<String, String> {
        self.get("description").await
    }

    /// Get the device description (typed error)
    pub async fn get_description_typed(&self) -> Result<String, AlpacaError> {
        self.get_typed("description").await
    }

    /// Get the driver version
    pub async fn get_driver_version(&self) -> Result<String, String> {
        self.get("driverversion").await
    }

    /// Get the driver version (typed error)
    pub async fn get_driver_version_typed(&self) -> Result<String, AlpacaError> {
        self.get_typed("driverversion").await
    }

    /// Get the driver info
    pub async fn get_driver_info(&self) -> Result<String, String> {
        self.get("driverinfo").await
    }

    /// Get the interface version
    pub async fn get_interface_version(&self) -> Result<i32, String> {
        self.get("interfaceversion").await
    }

    /// Get supported actions
    pub async fn get_supported_actions(&self) -> Result<Vec<String>, String> {
        self.get("supportedactions").await
    }

    /// Validate that the connection is still alive and responding
    /// Performs a lightweight check that the device can process requests
    pub async fn validate_connection(&self) -> Result<bool, AlpacaError> {
        // Use a quick timeout for validation
        let client = self.create_quick_timeout_client()?;
        let (client_id, transaction_id) = get_client_transaction();
        let url = format!(
            "{}?ClientID={}&ClientTransactionID={}",
            self.build_url("connected"),
            client_id,
            transaction_id
        );

        match client.get(&url).send().await {
            Ok(response) => {
                if !response.status().is_success() {
                    return Err(AlpacaError::ValidationFailed(
                        format!("HTTP status: {}", response.status())
                    ));
                }

                let result: Result<AlpacaResponse<bool>, _> = response.json().await;
                match result {
                    Ok(alpaca_response) => {
                        if alpaca_response.error_number != 0 {
                            Err(AlpacaError::ValidationFailed(
                                alpaca_response.error_message
                            ))
                        } else {
                            Ok(alpaca_response.value)
                        }
                    }
                    Err(e) => Err(AlpacaError::ValidationFailed(e.to_string()))
                }
            }
            Err(e) => {
                if e.is_timeout() {
                    Err(AlpacaError::timeout("connection validation", self.timeout_config.quick_query_ms))
                } else if e.is_connect() {
                    Err(AlpacaError::connection_refused(&self.base_url, e.to_string()))
                } else {
                    Err(AlpacaError::ValidationFailed(e.to_string()))
                }
            }
        }
    }

    /// Send a heartbeat ping and return the round-trip time in milliseconds
    /// Uses the "connected" endpoint as a lightweight ping
    pub async fn heartbeat(&self) -> Result<u64, AlpacaError> {
        let start = std::time::Instant::now();

        let client = self.create_quick_timeout_client()?;
        let (client_id, transaction_id) = get_client_transaction();
        let url = format!(
            "{}?ClientID={}&ClientTransactionID={}",
            self.build_url("connected"),
            client_id,
            transaction_id
        );

        let response = client.get(&url).send().await?;

        if !response.status().is_success() {
            return Err(AlpacaError::HttpError {
                status: response.status().as_u16(),
                message: "Heartbeat failed".to_string(),
            });
        }

        // Consume the response body
        let _: AlpacaResponse<bool> = response.json().await?;

        let elapsed = start.elapsed();
        Ok(elapsed.as_millis() as u64)
    }

    /// Detect supported API versions from the server
    pub async fn detect_api_versions(&self) -> Result<Vec<u32>, AlpacaError> {
        let client = self.create_quick_timeout_client()?;
        let url = format!("{}/management/apiversions", self.base_url);

        match client.get(&url).send().await {
            Ok(response) => {
                if !response.status().is_success() {
                    return Err(AlpacaError::HttpError {
                        status: response.status().as_u16(),
                        message: "Failed to get API versions".to_string(),
                    });
                }

                let api_response: ApiVersionsResponse = response.json().await?;
                Ok(api_response.value)
            }
            Err(e) => {
                warn!("Failed to detect API versions: {}", e);
                // Default to v1 if detection fails
                Ok(vec![1])
            }
        }
    }

    /// Negotiate the best API version to use with the server
    pub async fn negotiate_api_version(&mut self) -> Result<ApiVersion, AlpacaError> {
        let versions = self.detect_api_versions().await?;

        // Currently we only support v1, but this framework allows future versions
        if versions.contains(&1) {
            self.api_version = ApiVersion::V1;
            Ok(ApiVersion::V1)
        } else {
            Err(AlpacaError::UnsupportedApiVersion(
                format!("Server supports versions {:?}, but client only supports v1", versions)
            ))
        }
    }
}

/// Builder for creating AlpacaClient with custom configuration
pub struct AlpacaClientBuilder {
    device: AlpacaDevice,
    timeout_config: TimeoutConfig,
    retry_config: RetryConfig,
}

impl AlpacaClientBuilder {
    pub fn new(device: AlpacaDevice) -> Self {
        Self {
            device,
            timeout_config: TimeoutConfig::default(),
            retry_config: RetryConfig::default(),
        }
    }

    pub fn timeout_config(mut self, config: TimeoutConfig) -> Self {
        self.timeout_config = config;
        self
    }

    pub fn retry_config(mut self, config: RetryConfig) -> Self {
        self.retry_config = config;
        self
    }

    pub fn quick_query_timeout(mut self, ms: u64) -> Self {
        self.timeout_config.quick_query_ms = ms;
        self
    }

    pub fn standard_timeout(mut self, ms: u64) -> Self {
        self.timeout_config.standard_operation_ms = ms;
        self
    }

    pub fn long_timeout(mut self, ms: u64) -> Self {
        self.timeout_config.long_operation_ms = ms;
        self
    }

    pub fn connect_timeout(mut self, ms: u64) -> Self {
        self.timeout_config.connect_ms = ms;
        self
    }

    pub fn max_retry_attempts(mut self, attempts: u32) -> Self {
        self.retry_config.max_attempts = attempts;
        self
    }

    pub fn no_retry(mut self) -> Self {
        self.retry_config = RetryConfig::no_retry();
        self
    }

    pub fn build(self) -> AlpacaClient {
        AlpacaClient::with_config(&self.device, self.timeout_config, self.retry_config)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::collections::HashSet;
    use std::sync::Arc;
    use std::thread;

    #[test]
    fn test_transaction_id_uniqueness_single_thread() {
        // Reset transaction ID to a known state
        let _ = TRANSACTION_ID.fetch_add(0, Ordering::SeqCst);

        let mut ids = HashSet::new();
        for _ in 0..1000 {
            let (_, tid) = get_client_transaction();
            assert!(ids.insert(tid), "Transaction ID {} was not unique", tid);
        }
        assert_eq!(ids.len(), 1000);
    }

    #[test]
    fn test_transaction_id_uniqueness_multi_thread() {
        use std::sync::Mutex;

        let ids = Arc::new(Mutex::new(HashSet::new()));
        let mut handles = vec![];

        // Spawn 10 threads, each generating 100 transaction IDs
        for _ in 0..10 {
            let handle = thread::spawn(move || {
                let mut local_ids = Vec::new();
                for _ in 0..100 {
                    let (_, tid) = get_client_transaction();
                    local_ids.push(tid);
                }
                local_ids
            });
            handles.push(handle);
        }

        // Collect all IDs from all threads
        for handle in handles {
            let local_ids = handle.join().unwrap();
            let mut ids_lock = ids.lock().unwrap();
            for id in local_ids {
                assert!(ids_lock.insert(id), "Transaction ID {} was not unique across threads", id);
            }
        }

        // Should have 1000 unique IDs
        let ids_lock = ids.lock().unwrap();
        assert_eq!(ids_lock.len(), 1000, "Expected 1000 unique transaction IDs, got {}", ids_lock.len());
    }

    #[test]
    fn test_client_id_get_set() {
        let original = get_client_id();

        set_client_id(42);
        assert_eq!(get_client_id(), 42);

        set_client_id(100);
        assert_eq!(get_client_id(), 100);

        // Restore original
        set_client_id(original);
    }

    #[test]
    fn test_transaction_id_atomicity() {
        // Test that fetch_add is atomic by checking that we get sequential IDs
        let id1 = TRANSACTION_ID.fetch_add(1, Ordering::SeqCst);
        let id2 = TRANSACTION_ID.fetch_add(1, Ordering::SeqCst);
        let id3 = TRANSACTION_ID.fetch_add(1, Ordering::SeqCst);

        assert_eq!(id2, id1 + 1);
        assert_eq!(id3, id2 + 1);
    }

    #[test]
    fn test_retry_config_delay_calculation() {
        let config = RetryConfig {
            max_attempts: 5,
            initial_delay_ms: 100,
            max_delay_ms: 5000,
            backoff_multiplier: 2.0,
            use_jitter: false, // Disable jitter for deterministic testing
        };

        // Attempt 0: 100ms
        let delay0 = config.delay_for_attempt(0);
        assert_eq!(delay0.as_millis(), 100);

        // Attempt 1: 200ms
        let delay1 = config.delay_for_attempt(1);
        assert_eq!(delay1.as_millis(), 200);

        // Attempt 2: 400ms
        let delay2 = config.delay_for_attempt(2);
        assert_eq!(delay2.as_millis(), 400);

        // Attempt 3: 800ms
        let delay3 = config.delay_for_attempt(3);
        assert_eq!(delay3.as_millis(), 800);

        // Attempt 4: 1600ms
        let delay4 = config.delay_for_attempt(4);
        assert_eq!(delay4.as_millis(), 1600);
    }

    #[test]
    fn test_retry_config_max_delay_cap() {
        let config = RetryConfig {
            max_attempts: 10,
            initial_delay_ms: 1000,
            max_delay_ms: 3000, // Cap at 3 seconds
            backoff_multiplier: 2.0,
            use_jitter: false,
        };

        // Attempt 0: 1000ms
        assert_eq!(config.delay_for_attempt(0).as_millis(), 1000);

        // Attempt 1: 2000ms
        assert_eq!(config.delay_for_attempt(1).as_millis(), 2000);

        // Attempt 2: should be 4000ms but capped to 3000ms
        assert_eq!(config.delay_for_attempt(2).as_millis(), 3000);

        // Attempt 3: would be 8000ms but capped to 3000ms
        assert_eq!(config.delay_for_attempt(3).as_millis(), 3000);
    }

    #[test]
    fn test_alpaca_error_conversion() {
        let error = AlpacaError::timeout("test_operation", 5000);
        let error_string: String = error.into();
        assert!(error_string.contains("5000ms"));
        assert!(error_string.contains("test_operation"));

        let error = AlpacaError::DeviceError {
            code: 1031,
            message: "Method not implemented".to_string(),
        };
        let error_string: String = error.into();
        assert!(error_string.contains("1031"));
        assert!(error_string.contains("Method not implemented"));
    }

    #[test]
    fn test_alpaca_error_is_retryable() {
        // Retryable errors
        assert!(AlpacaError::timeout("test", 5000).is_retryable());
        assert!(AlpacaError::connection_refused("http://localhost", "refused").is_retryable());
        assert!(AlpacaError::RequestFailed("network error".to_string()).is_retryable());
        assert!(AlpacaError::HttpError { status: 500, message: "server error".to_string() }.is_retryable());
        assert!(AlpacaError::HttpError { status: 503, message: "unavailable".to_string() }.is_retryable());
        assert!(AlpacaError::HttpError { status: 429, message: "rate limited".to_string() }.is_retryable());

        // Non-retryable errors
        assert!(!AlpacaError::DeviceError { code: 1, message: "device error".to_string() }.is_retryable());
        assert!(!AlpacaError::ParseError("parse error".to_string()).is_retryable());
        assert!(!AlpacaError::NotConnected.is_retryable());
        assert!(!AlpacaError::HttpError { status: 400, message: "bad request".to_string() }.is_retryable());
        assert!(!AlpacaError::HttpError { status: 404, message: "not found".to_string() }.is_retryable());
    }

    #[test]
    fn test_timeout_config_defaults() {
        let config = TimeoutConfig::default();
        assert_eq!(config.quick_query_ms, 5000);
        assert_eq!(config.standard_operation_ms, 30000);
        assert_eq!(config.long_operation_ms, 300000);
        assert_eq!(config.very_long_operation_ms, 600000);
        assert_eq!(config.connect_ms, 10000);
    }

    #[test]
    fn test_timeout_config_for_camera() {
        let config = TimeoutConfig::for_camera();
        assert_eq!(config.quick_query_ms, 5000);
        assert_eq!(config.long_operation_ms, 300000);
        assert_eq!(config.very_long_operation_ms, 900000);
    }

    #[test]
    fn test_timeout_config_for_telescope() {
        let config = TimeoutConfig::for_telescope();
        assert_eq!(config.quick_query_ms, 5000);
        assert_eq!(config.standard_operation_ms, 60000);
        assert_eq!(config.long_operation_ms, 300000);
    }

    #[test]
    fn test_timeout_config_for_dome() {
        let config = TimeoutConfig::for_dome();
        assert_eq!(config.quick_query_ms, 5000);
        assert_eq!(config.long_operation_ms, 300000);
        assert_eq!(config.very_long_operation_ms, 600000);
    }

    #[test]
    fn test_api_version() {
        assert_eq!(ApiVersion::V1.as_str(), "v1");
    }
}
