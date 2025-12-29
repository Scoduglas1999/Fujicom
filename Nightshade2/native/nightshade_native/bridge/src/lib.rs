mod frb_generated; /* AUTO INJECTED BY flutter_rust_bridge. This line may not be accurate, and you can change it according to your needs. */
// Nightshade Native Bridge
//
// This crate provides the FFI bridge between Dart and the native Rust backend.
// All public functions are exposed via flutter_rust_bridge.

mod api;
mod device;
mod device_capabilities;
mod device_guard;
mod device_id;
mod devices;
mod error;
mod event;
mod state;
mod storage;
mod sequencer_ops;
mod real_device_ops;
mod imaging_ops;
mod timeout_ops;
mod unified_device_ops;
mod ascom_wrapper;
mod ascom_wrapper_mount;
mod ascom_wrapper_focuser;
mod ascom_wrapper_filterwheel;
mod ascom_wrapper_dome;
mod ascom_wrapper_switch;
mod ascom_wrapper_covercalibrator;
mod sequencer_api;

pub use api::*;
pub use sequencer_api::*;
pub use device::*;
pub use device_capabilities::*;
pub use device_guard::*;
pub use device_id::*;
pub use devices::*;
pub use error::*;
pub use event::*;
pub use state::*;
pub use storage::*;
pub use sequencer_ops::*;
pub use real_device_ops::*;
pub use imaging_ops::*;
pub use timeout_ops::*;
pub use unified_device_ops::*;

use std::sync::OnceLock;
use std::sync::atomic::{AtomicBool, Ordering};
use std::path::PathBuf;
use std::panic::{self, AssertUnwindSafe};
use futures::FutureExt;
use tokio::runtime::Runtime;
use tracing_subscriber::layer::SubscriberExt;
use tracing_subscriber::util::SubscriberInitExt;

/// Global Tokio runtime for async operations
static RUNTIME: OnceLock<Runtime> = OnceLock::new();

/// Tracks whether runtime initialization has permanently failed.
/// Once this is set to true, all async operations will return errors instead of attempting
/// to create a runtime (which would just fail again).
static RUNTIME_INIT_FAILED: AtomicBool = AtomicBool::new(false);

/// Error message from the last runtime initialization failure.
/// Used to provide consistent error messages when RUNTIME_INIT_FAILED is true.
static RUNTIME_ERROR_MSG: OnceLock<String> = OnceLock::new();

/// Global log directory path
static LOG_DIR: OnceLock<PathBuf> = OnceLock::new();

/// Global log file guard (keeps file writer alive)
static LOG_GUARD: OnceLock<tracing_appender::non_blocking::WorkerGuard> = OnceLock::new();

/// Initialize the global runtime, returning a Result.
///
/// This function NEVER panics. If runtime creation fails after all fallback attempts,
/// it sets a static error state and returns an error. Subsequent calls will immediately
/// return the cached error without retrying (to avoid repeated resource exhaustion).
///
/// # Returns
/// - `Ok(&'static Runtime)` if the runtime was successfully created or already exists
/// - `Err(NightshadeError)` if runtime creation failed permanently
pub(crate) fn ensure_runtime() -> Result<&'static Runtime, NightshadeError> {
    // Fast path: check if we already have a runtime
    if let Some(rt) = RUNTIME.get() {
        return Ok(rt);
    }

    // Fast path: check if we've already failed permanently
    if RUNTIME_INIT_FAILED.load(Ordering::Acquire) {
        let msg = RUNTIME_ERROR_MSG.get()
            .map(|s| s.as_str())
            .unwrap_or("Unknown runtime initialization failure");
        return Err(NightshadeError::RuntimeInitFailed(msg.to_string()));
    }

    // Try to initialize the runtime with fallbacks
    // We use get_or_init but wrap the entire creation in Result handling
    let result = try_create_runtime_with_fallbacks();

    match result {
        Ok(rt) => {
            // Successfully created - store it
            // Note: If another thread beat us to it, get_or_init returns the existing one
            Ok(RUNTIME.get_or_init(|| rt))
        }
        Err(error_msg) => {
            // All attempts failed - record the permanent failure
            eprintln!("FATAL: Runtime initialization failed permanently: {}", error_msg);
            tracing::error!("Runtime initialization failed permanently: {}", error_msg);

            // Set the error state (only the first thread to fail sets the message)
            let _ = RUNTIME_ERROR_MSG.set(error_msg.clone());
            RUNTIME_INIT_FAILED.store(true, Ordering::Release);

            Err(NightshadeError::RuntimeInitFailed(error_msg))
        }
    }
}

/// Try to create a runtime with fallbacks. Returns the runtime or an error message.
/// This function NEVER panics.
fn try_create_runtime_with_fallbacks() -> Result<Runtime, String> {
    // First attempt: Create a multi-threaded runtime with default settings
    match Runtime::new() {
        Ok(rt) => {
            tracing::debug!("Created multi-threaded Tokio runtime");
            return Ok(rt);
        }
        Err(e) => {
            eprintln!("WARNING: Failed to create default Tokio runtime: {}", e);
            tracing::warn!("Failed to create default Tokio runtime: {}", e);
        }
    }

    // Second attempt: Try a current-thread runtime with all features
    match tokio::runtime::Builder::new_current_thread()
        .enable_all()
        .build()
    {
        Ok(rt) => {
            tracing::warn!("Using single-threaded Tokio runtime as fallback");
            return Ok(rt);
        }
        Err(e2) => {
            eprintln!("WARNING: Single-threaded runtime with all features failed: {}", e2);
            tracing::warn!("Single-threaded runtime with all features failed: {}", e2);
        }
    }

    // Third attempt: Minimal runtime without IO or timers
    match tokio::runtime::Builder::new_current_thread().build() {
        Ok(rt) => {
            tracing::warn!("Using minimal Tokio runtime (no IO, no timers)");
            eprintln!("WARNING: Using minimal Tokio runtime - some features may not work");
            return Ok(rt);
        }
        Err(e3) => {
            let error_msg = format!(
                "All runtime creation attempts failed. System may be resource-exhausted: {}",
                e3
            );
            eprintln!("CRITICAL: {}", error_msg);
            Err(error_msg)
        }
    }
}

/// Try to create a runtime, returning an error instead of panicking
/// Use this for cases where failure can be handled gracefully
pub(crate) fn try_create_runtime() -> Result<Runtime, NightshadeError> {
    Runtime::new().map_err(|e| {
        NightshadeError::RuntimeInitFailed(format!("Failed to create Tokio runtime: {}", e))
    })
}

/// Initialize panic handler to catch panics and log them
fn init_panic_handler() {
    panic::set_hook(Box::new(|panic_info| {
        let msg = panic_info.to_string();
        let location = panic_info.location().map(|l| {
            format!("{}:{}:{}", l.file(), l.line(), l.column())
        }).unwrap_or_else(|| "unknown".to_string());

        eprintln!("PANIC at {}: {}", location, msg);
        tracing::error!("PANIC at {}: {}", location, msg);
    }));
}

/// Initialize the native bridge with logging
/// Must be called once at app startup
///
/// This function wraps all initialization in panic catching to ensure that even
/// if initialization fails catastrophically, we return an error rather than
/// crashing the Flutter app.
///
/// # Arguments
/// * `log_directory` - Optional path to store log files. If None, logs only to console.
#[flutter_rust_bridge::frb(sync)]
pub fn init_native_with_logging(log_directory: Option<String>) -> Result<(), NightshadeError> {
    // Install panic handler first - this must happen before any other code
    // so that panics are properly logged even if catch_unwind doesn't catch them
    init_panic_handler();

    // Wrap the entire initialization in panic catching for FFI safety
    catch_panic_sync(|| init_native_internal(log_directory))
}

/// Internal initialization function that does the actual work.
/// Separated out so we can wrap it in panic catching.
fn init_native_internal(log_directory: Option<String>) -> Result<(), NightshadeError> {
    use tracing_subscriber::fmt;
    use tracing_subscriber::EnvFilter;

    // Create env filter for log level
    let env_filter = EnvFilter::try_from_default_env()
        .unwrap_or_else(|_| EnvFilter::new("debug"));

    if let Some(log_dir) = log_directory {
        // Store log directory for later access
        let log_path = PathBuf::from(&log_dir);

        // Create log directory if it doesn't exist
        if let Err(e) = std::fs::create_dir_all(&log_path) {
            eprintln!("Failed to create log directory: {}", e);
            // Fall back to console-only logging
            return init_native_internal(None);
        }

        LOG_DIR.set(log_path.clone()).ok();

        // Create daily rolling file appender
        let file_appender = tracing_appender::rolling::daily(&log_path, "nightshade.log");
        let (non_blocking, guard) = tracing_appender::non_blocking(file_appender);

        // Keep guard alive for the lifetime of the app
        LOG_GUARD.set(guard).ok();

        // Create a layered subscriber with both console and file output
        let console_layer = fmt::layer()
            .with_target(false)
            .with_ansi(true);

        let file_layer = fmt::layer()
            .with_target(true)
            .with_ansi(false)
            .with_writer(non_blocking);

        tracing_subscriber::registry()
            .with(env_filter)
            .with(console_layer)
            .with(file_layer)
            .init();

        tracing::info!("Nightshade Native Bridge initialized with file logging");
        tracing::info!("Log directory: {}", log_dir);

        // Clean up old log files (keep last 7 days)
        cleanup_old_logs(&log_path, 7);
    } else {
        // Console-only logging
        tracing_subscriber::fmt()
            .with_max_level(tracing::Level::DEBUG)
            .with_target(false)
            .init();

        tracing::info!("Nightshade Native Bridge initialized (console logging only)");
    }

    // Ensure runtime is created - propagate errors to caller
    ensure_runtime()?;

    Ok(())
}

/// Initialize the native bridge (legacy, console-only logging)
/// Must be called once at app startup
#[flutter_rust_bridge::frb(sync)]
pub fn init_native() -> Result<(), NightshadeError> {
    init_native_with_logging(None)
}

/// Get the current log directory path
#[flutter_rust_bridge::frb(sync)]
pub fn get_log_directory() -> Option<String> {
    LOG_DIR.get().map(|p| p.to_string_lossy().to_string())
}

/// Get the current log file path (today's log)
#[flutter_rust_bridge::frb(sync)]
pub fn get_current_log_file() -> Option<String> {
    LOG_DIR.get().map(|dir| {
        let today = chrono::Local::now().format("%Y-%m-%d");
        dir.join(format!("nightshade.log.{}", today))
            .to_string_lossy()
            .to_string()
    })
}

/// List all available log files
pub fn list_log_files() -> Vec<String> {
    if let Some(log_dir) = LOG_DIR.get() {
        if let Ok(entries) = std::fs::read_dir(log_dir) {
            return entries
                .filter_map(|e| e.ok())
                .filter(|e| {
                    e.file_name()
                        .to_string_lossy()
                        .starts_with("nightshade.log")
                })
                .map(|e| e.path().to_string_lossy().to_string())
                .collect();
        }
    }
    Vec::new()
}

/// Read a log file's contents
pub fn read_log_file(path: String) -> Result<String, NightshadeError> {
    std::fs::read_to_string(&path)
        .map_err(|e| NightshadeError::IoError(format!("Failed to read log file: {}", e)))
}

/// Export all logs to a single file for diagnostics
pub fn export_logs_to_file(output_path: String) -> Result<(), NightshadeError> {
    let mut output = std::fs::File::create(&output_path)
        .map_err(|e| NightshadeError::IoError(format!("Failed to create export file: {}", e)))?;

    use std::io::Write;

    // Write header
    writeln!(output, "=== Nightshade Log Export ===").ok();
    writeln!(output, "Exported: {}", chrono::Local::now().to_rfc3339()).ok();
    writeln!(output, "").ok();

    // Write all log files in chronological order
    let mut log_files = list_log_files();
    log_files.sort();

    for log_file in log_files {
        writeln!(output, "\n=== {} ===\n", log_file).ok();
        if let Ok(content) = std::fs::read_to_string(&log_file) {
            write!(output, "{}", content).ok();
        }
    }

    tracing::info!("Logs exported to: {}", output_path);
    Ok(())
}

/// Clean up old log files, keeping only the most recent N days
fn cleanup_old_logs(log_dir: &PathBuf, keep_days: i64) {
    use chrono::TimeZone;

    let cutoff = chrono::Local::now() - chrono::Duration::days(keep_days);

    let entries = match std::fs::read_dir(log_dir) {
        Ok(entries) => entries,
        Err(e) => {
            tracing::debug!("Cannot read log directory for cleanup: {}", e);
            return;
        }
    };

    for entry in entries.filter_map(|e| e.ok()) {
        let path = entry.path();
        let name = match path.file_name().and_then(|n| n.to_str()) {
            Some(name) => name,
            None => continue,
        };

        // Parse date from filename like "nightshade.log.2024-01-15"
        if !name.starts_with("nightshade.log.") {
            continue;
        }

        let date_str = match name.strip_prefix("nightshade.log.") {
            Some(s) => s,
            None => continue,
        };

        let file_date = match chrono::NaiveDate::parse_from_str(date_str, "%Y-%m-%d") {
            Ok(date) => date,
            Err(_) => continue, // Skip files with invalid date format
        };

        // Safely create datetime - if None, skip this file
        let file_datetime = match file_date.and_hms_opt(0, 0, 0) {
            Some(dt) => dt,
            None => {
                tracing::debug!("Invalid time components for date {}", date_str);
                continue;
            }
        };

        let file_local = match chrono::Local.from_local_datetime(&file_datetime).single() {
            Some(local) => local,
            None => continue, // Ambiguous or invalid local time
        };

        if file_local < cutoff {
            if let Err(e) = std::fs::remove_file(&path) {
                tracing::warn!("Failed to delete old log file {:?}: {}", path, e);
            } else {
                tracing::debug!("Deleted old log file: {:?}", path);
            }
        }
    }
}

/// Get the version of the native library
#[flutter_rust_bridge::frb(sync)]
pub fn get_native_version() -> String {
    env!("CARGO_PKG_VERSION").to_string()
}

// =============================================================================
// FFI BOUNDARY PANIC SAFETY
// =============================================================================

/// Wraps a synchronous operation with panic catching for FFI safety.
///
/// This function catches any panics that occur during execution and converts
/// them to `NightshadeError::Internal` instead of propagating the panic across
/// the FFI boundary, which would cause undefined behavior.
///
/// # Example
/// ```ignore
/// pub fn risky_operation() -> Result<i32, NightshadeError> {
///     catch_panic_sync(|| {
///         // potentially panicking code
///         Ok(42)
///     })
/// }
/// ```
pub fn catch_panic_sync<F, T>(f: F) -> Result<T, NightshadeError>
where
    F: FnOnce() -> Result<T, NightshadeError> + panic::UnwindSafe,
{
    match panic::catch_unwind(f) {
        Ok(result) => result,
        Err(panic_payload) => {
            let panic_msg = extract_panic_message(&panic_payload);
            tracing::error!("Panic caught at FFI boundary: {}", panic_msg);
            Err(NightshadeError::Internal(format!(
                "Internal panic: {}",
                panic_msg
            )))
        }
    }
}

/// Wraps an async operation with panic catching for FFI safety.
///
/// This function catches any panics that occur during future execution and
/// converts them to `NightshadeError::Internal`. The async block is wrapped
/// in `AssertUnwindSafe` to allow catching panics from async code.
///
/// # Example
/// ```ignore
/// pub async fn risky_async_operation() -> Result<i32, NightshadeError> {
///     catch_panic_async(async {
///         // potentially panicking async code
///         Ok(42)
///     }).await
/// }
/// ```
pub async fn catch_panic_async<F, T>(f: F) -> Result<T, NightshadeError>
where
    F: std::future::Future<Output = Result<T, NightshadeError>>,
{
    // Use tokio's spawn_blocking with catch_unwind for the future
    // Note: For truly async code, we catch panics differently
    let result = AssertUnwindSafe(f).catch_unwind().await;

    match result {
        Ok(inner_result) => inner_result,
        Err(panic_payload) => {
            let panic_msg = extract_panic_message(&panic_payload);
            tracing::error!("Async panic caught at FFI boundary: {}", panic_msg);
            Err(NightshadeError::Internal(format!(
                "Internal panic in async operation: {}",
                panic_msg
            )))
        }
    }
}

/// Extract a human-readable message from a panic payload.
///
/// Panic payloads can be different types (`&str`, `String`, or other types).
/// This function attempts to extract the most useful message.
fn extract_panic_message(payload: &Box<dyn std::any::Any + Send>) -> String {
    if let Some(s) = payload.downcast_ref::<&str>() {
        (*s).to_string()
    } else if let Some(s) = payload.downcast_ref::<String>() {
        s.clone()
    } else {
        "Unknown panic".to_string()
    }
}

/// Get the global runtime, returning a Result instead of panicking.
///
/// This is an alias for `ensure_runtime()` for backwards compatibility and clarity.
/// It will never panic - if the runtime cannot be created, it returns an error.
///
/// # Returns
/// - `Ok(&'static Runtime)` if the runtime is available
/// - `Err(NightshadeError::RuntimeInitFailed)` if runtime creation failed
pub(crate) fn get_runtime() -> Result<&'static Runtime, NightshadeError> {
    ensure_runtime()
}

/// Executes an async operation on the global runtime with panic safety.
///
/// This is the recommended way to execute async code from synchronous FFI entry points.
/// It handles:
/// - Runtime initialization failure
/// - Panics in async code
/// - Proper error conversion
///
/// # Example
/// ```ignore
/// pub fn sync_wrapper() -> Result<String, NightshadeError> {
///     run_async_safe(async {
///         // async code here
///         Ok("result".to_string())
///     })
/// }
/// ```
pub fn run_async_safe<F, T>(future: F) -> Result<T, NightshadeError>
where
    F: std::future::Future<Output = Result<T, NightshadeError>> + Send + 'static,
    T: Send + 'static,
{
    let runtime = get_runtime()?;

    // Wrap in catch_unwind for panic safety
    match panic::catch_unwind(AssertUnwindSafe(|| {
        runtime.block_on(async {
            catch_panic_async(future).await
        })
    })) {
        Ok(result) => result,
        Err(panic_payload) => {
            let msg = extract_panic_message(&panic_payload);
            Err(NightshadeError::Internal(format!(
                "Panic during async execution: {}",
                msg
            )))
        }
    }
}
