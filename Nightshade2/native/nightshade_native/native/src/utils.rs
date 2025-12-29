//! Common utilities for native SDK drivers
//!
//! This module provides:
//! - Safe C string conversion with null-termination validation
//! - Overflow-safe buffer size calculations
//! - Common error handling utilities
//!
//! Note: Per-vendor SDK mutexes are defined in `sync.rs`, not here.
//! Use functions like `zwo_camera_mutex()`, `qhy_mutex()`, etc. from `crate::sync`.

use crate::traits::NativeError;
use std::ffi::c_char;

// =============================================================================
// SAFE STRING CONVERSION
// =============================================================================

/// Safely convert a C string pointer to a Rust String with bounds checking.
///
/// This function:
/// 1. Handles null pointers gracefully (returns empty string)
/// 2. Enforces a maximum length to prevent buffer overruns
/// 3. Finds the null terminator safely within the bounded slice
/// 4. Uses lossy UTF-8 conversion for robustness
///
/// # Arguments
/// * `ptr` - Pointer to a null-terminated C string (can be null)
/// * `max_len` - Maximum number of bytes to read (typically the buffer size)
///
/// # Returns
/// A safe Rust String, empty if the pointer is null or the string is empty
///
/// # Safety
/// The caller must ensure that `ptr` points to valid memory of at least `max_len` bytes
/// if `ptr` is not null.
///
/// # Example
/// ```ignore
/// let name = safe_cstr_to_string(info.name.as_ptr(), 64);
/// ```
pub fn safe_cstr_to_string(ptr: *const c_char, max_len: usize) -> String {
    if ptr.is_null() {
        return String::new();
    }

    // Safety: We're treating the pointer as a byte slice with bounded length.
    // The caller guarantees the pointer points to valid memory of at least max_len bytes.
    unsafe {
        let slice = std::slice::from_raw_parts(ptr as *const u8, max_len);
        // Find the null terminator, or use max_len if not found
        let null_pos = slice.iter().position(|&c| c == 0).unwrap_or(max_len);
        // Convert only up to the null terminator
        String::from_utf8_lossy(&slice[..null_pos]).to_string()
    }
}

/// Safely convert a fixed-size C char array to a Rust String.
///
/// This is a convenience wrapper for arrays that are common in SDK structs.
///
/// # Arguments
/// * `arr` - Reference to a fixed-size C char array
///
/// # Returns
/// A safe Rust String, trimmed at the first null byte
pub fn safe_char_array_to_string<const N: usize>(arr: &[c_char; N]) -> String {
    safe_cstr_to_string(arr.as_ptr(), N)
}

// =============================================================================
// OVERFLOW-SAFE BUFFER CALCULATIONS
// =============================================================================

/// Calculate buffer size for image data with overflow protection.
///
/// This function uses checked arithmetic to prevent integer overflow when
/// calculating buffer sizes for image data. This is critical for memory safety
/// as unchecked multiplication of width * height * bytes_per_pixel can overflow
/// on large images, leading to undersized buffer allocations.
///
/// # Arguments
/// * `width` - Image width in pixels
/// * `height` - Image height in pixels
/// * `bytes_per_pixel` - Number of bytes per pixel (1 for 8-bit, 2 for 16-bit, etc.)
///
/// # Returns
/// * `Ok(usize)` - The safe buffer size
/// * `Err(NativeError)` - If the calculation would overflow
///
/// # Example
/// ```ignore
/// let buffer_size = calculate_buffer_size(4656, 3520, 2)?;
/// let mut buffer: Vec<u8> = vec![0u8; buffer_size];
/// ```
pub fn calculate_buffer_size(width: u32, height: u32, bytes_per_pixel: u32) -> Result<usize, NativeError> {
    width
        .checked_mul(height)
        .and_then(|pixels| pixels.checked_mul(bytes_per_pixel))
        .map(|size| size as usize)
        .ok_or_else(|| {
            NativeError::InvalidParameter(format!(
                "Image buffer size overflow: {}x{} with {} bytes/pixel",
                width, height, bytes_per_pixel
            ))
        })
}

/// Calculate buffer size with i32 inputs (common in C APIs) with overflow protection.
///
/// # Arguments
/// * `width` - Image width in pixels (must be positive)
/// * `height` - Image height in pixels (must be positive)
/// * `bytes_per_pixel` - Number of bytes per pixel (must be positive)
///
/// # Returns
/// * `Ok(usize)` - The safe buffer size
/// * `Err(NativeError)` - If any input is negative or the calculation would overflow
pub fn calculate_buffer_size_i32(width: i32, height: i32, bytes_per_pixel: i32) -> Result<usize, NativeError> {
    // Validate inputs are positive
    if width <= 0 || height <= 0 || bytes_per_pixel <= 0 {
        return Err(NativeError::InvalidParameter(format!(
            "Invalid image dimensions: width={}, height={}, bytes_per_pixel={}",
            width, height, bytes_per_pixel
        )));
    }

    calculate_buffer_size(width as u32, height as u32, bytes_per_pixel as u32)
}

/// Validate that a buffer is large enough for the specified image dimensions.
///
/// # Arguments
/// * `buffer_len` - Actual buffer length in bytes
/// * `width` - Image width in pixels
/// * `height` - Image height in pixels
/// * `bytes_per_pixel` - Number of bytes per pixel
///
/// # Returns
/// * `Ok(())` - If the buffer is large enough
/// * `Err(NativeError)` - If the buffer is too small or dimensions overflow
pub fn validate_buffer_size(
    buffer_len: usize,
    width: u32,
    height: u32,
    bytes_per_pixel: u32,
) -> Result<(), NativeError> {
    let required = calculate_buffer_size(width, height, bytes_per_pixel)?;
    if buffer_len < required {
        return Err(NativeError::InvalidParameter(format!(
            "Buffer too small: have {} bytes, need {} bytes for {}x{} image",
            buffer_len, required, width, height
        )));
    }
    Ok(())
}

// =============================================================================
// ERROR HANDLING UTILITIES
// =============================================================================

/// Convert a vendor SDK error code to a NativeError with detailed context.
///
/// This is a helper for creating consistent error messages across vendors.
///
/// # Arguments
/// * `vendor` - Name of the vendor (e.g., "ZWO", "QHY")
/// * `operation` - Description of the operation that failed
/// * `code` - The SDK error code
/// * `message` - Optional additional message
pub fn sdk_error(vendor: &str, operation: &str, code: i32, message: Option<&str>) -> NativeError {
    match message {
        Some(msg) => NativeError::SdkError(format!(
            "{} {}: error code {} - {}",
            vendor, operation, code, msg
        )),
        None => NativeError::SdkError(format!(
            "{} {}: error code {}",
            vendor, operation, code
        )),
    }
}

// =============================================================================
// CONNECT WITH CLEANUP GUARD
// =============================================================================

/// A guard that ensures cleanup is called if the guarded block fails.
///
/// This is useful for implementing the pattern where we need to close
/// a device handle if subsequent initialization steps fail after opening.
///
/// # Example
/// ```ignore
/// // Open the device
/// sdk.open(device_id);
///
/// // Create guard that will close on drop if not defused
/// let cleanup_guard = CleanupGuard::new(|| {
///     sdk.close(device_id);
/// });
///
/// // Do initialization that might fail
/// sdk.init(device_id)?;
/// sdk.configure(device_id)?;
///
/// // Success! Defuse the guard so it doesn't clean up
/// cleanup_guard.defuse();
/// ```
pub struct CleanupGuard<F: FnOnce()> {
    cleanup: Option<F>,
}

impl<F: FnOnce()> CleanupGuard<F> {
    /// Create a new cleanup guard with the given cleanup function.
    pub fn new(cleanup: F) -> Self {
        Self {
            cleanup: Some(cleanup),
        }
    }

    /// Defuse the guard, preventing the cleanup function from running.
    /// Call this when the operation succeeds and cleanup is not needed.
    pub fn defuse(mut self) {
        self.cleanup = None;
    }
}

impl<F: FnOnce()> Drop for CleanupGuard<F> {
    fn drop(&mut self) {
        if let Some(cleanup) = self.cleanup.take() {
            cleanup();
        }
    }
}

// =============================================================================
// TIMEOUT UTILITIES
// =============================================================================

use std::time::{Duration, Instant};
use crate::traits::NativeTimeoutConfig;

/// Wait for an exposure to complete with timeout and exponential backoff.
///
/// This function polls for exposure completion with an exponential backoff
/// strategy to balance responsiveness with CPU usage.
///
/// # Arguments
/// * `is_complete` - Async function that returns true when exposure is complete
/// * `timeout_secs` - Maximum time to wait in seconds
///
/// # Returns
/// * `Ok(())` - Exposure completed successfully
/// * `Err(NativeError::Timeout)` - Exposure did not complete within timeout
pub async fn wait_for_exposure_with_timeout<F, Fut>(
    mut is_complete: F,
    timeout_secs: f64,
) -> Result<(), NativeError>
where
    F: FnMut() -> Fut,
    Fut: std::future::Future<Output = Result<bool, NativeError>>,
{
    let start = std::time::Instant::now();
    let mut backoff_ms = 10u64;
    const MAX_BACKOFF_MS: u64 = 500;

    loop {
        // Check if exposure is complete
        if is_complete().await? {
            return Ok(());
        }

        // Check timeout
        if start.elapsed().as_secs_f64() > timeout_secs {
            return Err(NativeError::Timeout(format!(
                "Exposure did not complete within {:.1}s timeout",
                timeout_secs
            )));
        }

        // Wait with exponential backoff
        tokio::time::sleep(std::time::Duration::from_millis(backoff_ms)).await;

        // Increase backoff (1.5x) up to max
        backoff_ms = (backoff_ms * 3 / 2).min(MAX_BACKOFF_MS);
    }
}

/// Wait for exposure completion with detailed timeout tracking.
///
/// Similar to `wait_for_exposure_with_timeout` but uses `NativeTimeoutConfig`
/// and provides more detailed error information.
///
/// # Arguments
/// * `is_complete` - Async function that returns true when exposure is complete
/// * `config` - Timeout configuration
/// * `exposure_secs` - The expected exposure duration in seconds
///
/// # Returns
/// * `Ok(())` - Exposure completed successfully
/// * `Err(NativeError::ExposureTimeout)` - Exposure did not complete within timeout
pub async fn wait_for_exposure<F, Fut>(
    mut is_complete: F,
    config: &NativeTimeoutConfig,
    exposure_secs: f64,
) -> Result<(), NativeError>
where
    F: FnMut() -> Fut,
    Fut: std::future::Future<Output = Result<bool, NativeError>>,
{
    let timeout = config.calculate_exposure_timeout(exposure_secs);
    let start = Instant::now();
    let poll_interval = config.poll_interval;

    loop {
        // Check if exposure is complete
        match is_complete().await {
            Ok(true) => {
                tracing::debug!(
                    "Exposure completed after {:.2}s (expected {:.1}s)",
                    start.elapsed().as_secs_f64(),
                    exposure_secs
                );
                return Ok(());
            }
            Ok(false) => {
                // Not complete yet, continue polling
            }
            Err(e) => {
                // Propagate errors from the completion check
                return Err(e);
            }
        }

        // Check timeout
        let elapsed = start.elapsed();
        if elapsed > timeout {
            tracing::warn!(
                "Exposure timeout after {:?} (expected {:.1}s exposure + margin)",
                elapsed,
                exposure_secs
            );
            return Err(NativeError::exposure_timeout(elapsed, exposure_secs));
        }

        // Wait before next poll
        tokio::time::sleep(poll_interval).await;
    }
}

/// Wait for a move operation (focuser, filter wheel) to complete with timeout.
///
/// Polls a completion function until it returns true or the timeout is reached.
///
/// # Arguments
/// * `is_moving` - Async function that returns true if the device is still moving
/// * `timeout` - Maximum time to wait
/// * `poll_interval` - How often to check
/// * `operation_desc` - Description for error messages (e.g., "focuser move to 5000")
///
/// # Returns
/// * `Ok(())` - Move completed successfully
/// * `Err(NativeError::MoveTimeout)` - Move did not complete within timeout
pub async fn wait_for_move_complete<F, Fut>(
    mut is_moving: F,
    timeout: Duration,
    poll_interval: Duration,
    operation_desc: impl Into<String>,
) -> Result<(), NativeError>
where
    F: FnMut() -> Fut,
    Fut: std::future::Future<Output = Result<bool, NativeError>>,
{
    let operation = operation_desc.into();
    let start = Instant::now();

    loop {
        // Check if still moving
        match is_moving().await {
            Ok(true) => {
                // Still moving, continue polling
            }
            Ok(false) => {
                // Move complete
                tracing::debug!(
                    "{} completed after {:.2}s",
                    operation,
                    start.elapsed().as_secs_f64()
                );
                return Ok(());
            }
            Err(e) => {
                // Propagate errors
                return Err(e);
            }
        }

        // Check timeout
        let elapsed = start.elapsed();
        if elapsed > timeout {
            tracing::warn!("{} timeout after {:?}", operation, elapsed);
            return Err(NativeError::MoveTimeout {
                duration: elapsed,
                details: operation,
            });
        }

        // Wait before next poll
        tokio::time::sleep(poll_interval).await;
    }
}

/// Wait for focuser move to complete with timeout from config.
///
/// # Arguments
/// * `is_moving` - Async function that returns true if focuser is still moving
/// * `config` - Timeout configuration
/// * `target_position` - Target position for error messages
pub async fn wait_for_focuser_move<F, Fut>(
    is_moving: F,
    config: &NativeTimeoutConfig,
    target_position: i32,
) -> Result<(), NativeError>
where
    F: FnMut() -> Fut,
    Fut: std::future::Future<Output = Result<bool, NativeError>>,
{
    wait_for_move_complete(
        is_moving,
        config.focuser_move_timeout,
        config.poll_interval,
        format!("focuser move to position {}", target_position),
    )
    .await
}

/// Wait for filter wheel move to complete with timeout from config.
///
/// # Arguments
/// * `is_moving` - Async function that returns true if filter wheel is still moving
/// * `config` - Timeout configuration
/// * `target_slot` - Target filter slot for error messages
pub async fn wait_for_filterwheel_move<F, Fut>(
    is_moving: F,
    config: &NativeTimeoutConfig,
    target_slot: i32,
) -> Result<(), NativeError>
where
    F: FnMut() -> Fut,
    Fut: std::future::Future<Output = Result<bool, NativeError>>,
{
    wait_for_move_complete(
        is_moving,
        config.filterwheel_move_timeout,
        config.poll_interval,
        format!("filter wheel move to slot {}", target_slot),
    )
    .await
}

/// Execute an async operation with a timeout.
///
/// This is a generic timeout wrapper that can be used for any async operation
/// that might hang (e.g., SDK calls on unresponsive hardware).
///
/// # Arguments
/// * `operation` - The async operation to execute
/// * `timeout` - Maximum time to wait
/// * `operation_name` - Description for error messages
///
/// # Returns
/// * `Ok(T)` - The result of the operation
/// * `Err(NativeError::OperationTimeout)` - Operation did not complete within timeout
pub async fn with_timeout<T, F, Fut>(
    operation: F,
    timeout: Duration,
    operation_name: impl Into<String>,
) -> Result<T, NativeError>
where
    F: FnOnce() -> Fut,
    Fut: std::future::Future<Output = Result<T, NativeError>>,
{
    let name = operation_name.into();
    match tokio::time::timeout(timeout, operation()).await {
        Ok(result) => result,
        Err(_) => {
            tracing::warn!("Operation '{}' timed out after {:?}", name, timeout);
            Err(NativeError::operation_timeout(name, timeout))
        }
    }
}

/// Execute an async operation with a timeout, providing a result type that indicates
/// whether the operation completed or timed out.
///
/// Unlike `with_timeout`, this function returns `Ok(None)` on timeout instead of
/// an error, allowing the caller to handle timeouts differently.
///
/// # Arguments
/// * `operation` - The async operation to execute
/// * `timeout` - Maximum time to wait
///
/// # Returns
/// * `Ok(Some(T))` - The result of the operation
/// * `Ok(None)` - Operation timed out
/// * `Err(NativeError)` - Operation failed with an error
pub async fn try_with_timeout<T, F, Fut>(
    operation: F,
    timeout: Duration,
) -> Result<Option<T>, NativeError>
where
    F: FnOnce() -> Fut,
    Fut: std::future::Future<Output = Result<T, NativeError>>,
{
    match tokio::time::timeout(timeout, operation()).await {
        Ok(result) => result.map(Some),
        Err(_) => Ok(None),
    }
}

/// Tracks the duration of an operation for timeout checking.
///
/// This struct provides a convenient way to check if an operation has
/// exceeded its timeout without repeatedly calculating elapsed time.
///
/// # Example
/// ```ignore
/// let tracker = TimeoutTracker::new(Duration::from_secs(30));
/// loop {
///     if tracker.is_expired() {
///         return Err(tracker.timeout_error("my operation"));
///     }
///     // Do polling work...
///     tokio::time::sleep(Duration::from_millis(100)).await;
/// }
/// ```
#[derive(Debug, Clone)]
pub struct TimeoutTracker {
    start: Instant,
    timeout: Duration,
}

impl TimeoutTracker {
    /// Create a new timeout tracker with the given timeout duration.
    pub fn new(timeout: Duration) -> Self {
        Self {
            start: Instant::now(),
            timeout,
        }
    }

    /// Create a timeout tracker from a `NativeTimeoutConfig` for exposure operations.
    pub fn for_exposure(config: &NativeTimeoutConfig, exposure_secs: f64) -> Self {
        Self::new(config.calculate_exposure_timeout(exposure_secs))
    }

    /// Check if the timeout has expired.
    pub fn is_expired(&self) -> bool {
        self.start.elapsed() > self.timeout
    }

    /// Get the elapsed time since the tracker was created.
    pub fn elapsed(&self) -> Duration {
        self.start.elapsed()
    }

    /// Get the remaining time before timeout, or zero if already expired.
    pub fn remaining(&self) -> Duration {
        self.timeout.saturating_sub(self.start.elapsed())
    }

    /// Get the configured timeout duration.
    pub fn timeout(&self) -> Duration {
        self.timeout
    }

    /// Create an operation timeout error with the elapsed duration.
    pub fn timeout_error(&self, operation: impl Into<String>) -> NativeError {
        NativeError::operation_timeout(operation, self.elapsed())
    }

    /// Create an exposure timeout error.
    pub fn exposure_timeout_error(&self, expected_exposure: f64) -> NativeError {
        NativeError::exposure_timeout(self.elapsed(), expected_exposure)
    }

    /// Create a move timeout error.
    pub fn move_timeout_error(&self, details: impl Into<String>) -> NativeError {
        NativeError::MoveTimeout {
            duration: self.elapsed(),
            details: details.into(),
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_calculate_buffer_size() {
        // Normal case
        assert_eq!(calculate_buffer_size(100, 100, 2).unwrap(), 20000);

        // Large but valid
        assert_eq!(calculate_buffer_size(4656, 3520, 2).unwrap(), 32778240);

        // Zero dimensions
        assert_eq!(calculate_buffer_size(0, 100, 2).unwrap(), 0);

        // Would overflow - this should error
        let result = calculate_buffer_size(u32::MAX, u32::MAX, 2);
        assert!(result.is_err());
    }

    #[test]
    fn test_calculate_buffer_size_i32() {
        // Normal case
        assert_eq!(calculate_buffer_size_i32(100, 100, 2).unwrap(), 20000);

        // Negative width
        assert!(calculate_buffer_size_i32(-100, 100, 2).is_err());

        // Negative height
        assert!(calculate_buffer_size_i32(100, -100, 2).is_err());

        // Zero bytes per pixel
        assert!(calculate_buffer_size_i32(100, 100, 0).is_err());
    }

    #[test]
    fn test_safe_cstr_to_string() {
        // Null pointer
        assert_eq!(safe_cstr_to_string(std::ptr::null(), 64), "");

        // Normal C string
        let test = b"Hello\0World\0";
        let ptr = test.as_ptr() as *const c_char;
        assert_eq!(safe_cstr_to_string(ptr, 12), "Hello");

        // No null terminator within bounds
        let test = b"HelloWorld";
        let ptr = test.as_ptr() as *const c_char;
        assert_eq!(safe_cstr_to_string(ptr, 5), "Hello");
    }

    #[test]
    fn test_cleanup_guard_defuse() {
        use std::sync::atomic::{AtomicBool, Ordering};

        let cleaned_up = AtomicBool::new(false);
        {
            let guard = CleanupGuard::new(|| {
                cleaned_up.store(true, Ordering::SeqCst);
            });
            guard.defuse();
        }
        // Should NOT have cleaned up because we defused
        assert!(!cleaned_up.load(Ordering::SeqCst));
    }

    #[test]
    fn test_cleanup_guard_drops() {
        use std::sync::atomic::{AtomicBool, Ordering};

        let cleaned_up = AtomicBool::new(false);
        {
            let _guard = CleanupGuard::new(|| {
                cleaned_up.store(true, Ordering::SeqCst);
            });
            // guard is dropped here without defuse
        }
        // Should have cleaned up
        assert!(cleaned_up.load(Ordering::SeqCst));
    }
}
