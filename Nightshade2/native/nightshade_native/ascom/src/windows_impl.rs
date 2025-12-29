//! Real Windows ASCOM COM implementation
//!
//! Full COM interop for ASCOM devices using windows-rs.
//!
//! This module provides robust, production-quality ASCOM driver support with:
//! - Proper error types with detailed COM and ASCOM error information
//! - Operation timeouts to prevent hangs
//! - Connection health monitoring
//! - RAII-based resource cleanup

use crate::AscomDevice;
use std::ptr;
use std::sync::atomic::{AtomicBool, AtomicU64, Ordering};
use windows::{
    core::{GUID, PCWSTR, PWSTR},
    Win32::{
        Foundation::VARIANT_BOOL,
        System::{
            Com::{
                CoCreateInstance, CoInitializeEx, CoUninitialize, CLSIDFromProgID, IDispatch,
                CLSCTX_ALL, COINIT_APARTMENTTHREADED, DISPATCH_METHOD, DISPATCH_PROPERTYGET,
                DISPATCH_PROPERTYPUT, DISPPARAMS, EXCEPINFO, SAFEARRAY,
            },
            Registry::{
                RegCloseKey, RegEnumKeyExW, RegOpenKeyExW, RegQueryValueExW,
                HKEY, HKEY_LOCAL_MACHINE, KEY_READ, REG_SZ, REG_VALUE_TYPE,
            },
            Variant::{VT_ARRAY, VT_BOOL, VT_BSTR, VT_I2, VT_I4, VT_R8, VT_UI2, VT_VARIANT, VARIANT},
        },
    },
};

// ============================================================================
// Error Types
// ============================================================================

/// ASCOM-specific error types for better error handling and diagnostics
#[derive(Debug, Clone)]
pub enum AscomError {
    /// COM error with HRESULT code
    ComError {
        hresult: i32,
        message: String,
    },
    /// Operation timed out
    Timeout {
        operation: String,
        duration_ms: u64,
    },
    /// Device is not connected
    NotConnected,
    /// Property is not available on this device
    PropertyNotAvailable {
        property: String,
        reason: String,
    },
    /// Invalid value provided
    InvalidValue {
        value: String,
        reason: String,
    },
    /// ASCOM exception from driver
    AscomException {
        code: i32,
        source: String,
        description: String,
    },
    /// Device communication error
    CommunicationError {
        message: String,
    },
    /// Resource allocation error
    ResourceError {
        message: String,
    },
    /// Generic error
    Other(String),
}

impl std::fmt::Display for AscomError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            AscomError::ComError { hresult, message } => {
                write!(f, "COM error (HRESULT {:#X}): {}", hresult, message)
            }
            AscomError::Timeout { operation, duration_ms } => {
                write!(f, "Operation '{}' timed out after {}ms", operation, duration_ms)
            }
            AscomError::NotConnected => {
                write!(f, "Device is not connected")
            }
            AscomError::PropertyNotAvailable { property, reason } => {
                write!(f, "Property '{}' not available: {}", property, reason)
            }
            AscomError::InvalidValue { value, reason } => {
                write!(f, "Invalid value '{}': {}", value, reason)
            }
            AscomError::AscomException { code, source, description } => {
                write!(f, "ASCOM exception (code {}): {} - {}", code, source, description)
            }
            AscomError::CommunicationError { message } => {
                write!(f, "Communication error: {}", message)
            }
            AscomError::ResourceError { message } => {
                write!(f, "Resource error: {}", message)
            }
            AscomError::Other(msg) => {
                write!(f, "{}", msg)
            }
        }
    }
}

impl std::error::Error for AscomError {}

impl From<AscomError> for String {
    fn from(err: AscomError) -> String {
        err.to_string()
    }
}

impl From<String> for AscomError {
    fn from(s: String) -> Self {
        AscomError::Other(s)
    }
}

impl From<&str> for AscomError {
    fn from(s: &str) -> Self {
        AscomError::Other(s.to_string())
    }
}

/// Result type for ASCOM operations
pub type AscomResult<T> = Result<T, AscomError>;

// ============================================================================
// Timeout Configuration
// ============================================================================

/// Default timeout values for different operation types (in milliseconds)
#[derive(Debug, Clone, Copy)]
pub struct TimeoutConfig {
    /// Default timeout for property get operations
    pub property_get_ms: u64,
    /// Default timeout for property set operations
    pub property_set_ms: u64,
    /// Default timeout for method calls
    pub method_call_ms: u64,
    /// Timeout for long-running operations (slewing, exposures, etc.)
    pub long_operation_ms: u64,
    /// Timeout for connection operations
    pub connect_ms: u64,
}

impl Default for TimeoutConfig {
    fn default() -> Self {
        Self {
            property_get_ms: 5_000,      // 5 seconds
            property_set_ms: 10_000,     // 10 seconds
            method_call_ms: 30_000,      // 30 seconds
            long_operation_ms: 300_000,  // 5 minutes
            connect_ms: 60_000,          // 1 minute
        }
    }
}

/// Global timeout configuration - can be modified at runtime
static TIMEOUT_CONFIG: std::sync::OnceLock<std::sync::RwLock<TimeoutConfig>> = std::sync::OnceLock::new();

/// Get the current timeout configuration
pub fn get_timeout_config() -> TimeoutConfig {
    TIMEOUT_CONFIG
        .get_or_init(|| std::sync::RwLock::new(TimeoutConfig::default()))
        .read()
        .map(|g| *g)
        .unwrap_or_default()
}

/// Set the timeout configuration
pub fn set_timeout_config(config: TimeoutConfig) {
    if let Some(lock) = TIMEOUT_CONFIG.get() {
        if let Ok(mut guard) = lock.write() {
            *guard = config;
        }
    } else {
        let _ = TIMEOUT_CONFIG.set(std::sync::RwLock::new(config));
    }
}

// ============================================================================
// Connection Health Monitoring
// ============================================================================

/// Health status of an ASCOM device connection
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ConnectionHealth {
    /// Device is healthy and responding
    Healthy,
    /// Device is not responding but may recover
    Degraded,
    /// Device connection has failed
    Failed,
    /// Device health is unknown (not yet checked)
    Unknown,
}

/// Tracks connection health for an ASCOM device
#[derive(Debug)]
pub struct HealthMonitor {
    /// Last successful communication timestamp (epoch ms)
    last_success: AtomicU64,
    /// Last failed communication timestamp (epoch ms)
    last_failure: AtomicU64,
    /// Consecutive failure count
    failure_count: std::sync::atomic::AtomicU32,
    /// Whether the connection is considered healthy
    is_healthy: AtomicBool,
    /// Maximum time between health checks before considering connection degraded (ms)
    health_check_interval_ms: u64,
    /// Number of consecutive failures before marking connection as failed
    max_failures: u32,
}

impl Default for HealthMonitor {
    fn default() -> Self {
        Self {
            last_success: AtomicU64::new(0),
            last_failure: AtomicU64::new(0),
            failure_count: std::sync::atomic::AtomicU32::new(0),
            is_healthy: AtomicBool::new(true),
            health_check_interval_ms: 30_000, // 30 seconds
            max_failures: 3,
        }
    }
}

impl HealthMonitor {
    /// Create a new health monitor with custom settings
    pub fn new(health_check_interval_ms: u64, max_failures: u32) -> Self {
        Self {
            health_check_interval_ms,
            max_failures,
            ..Default::default()
        }
    }

    /// Record a successful operation
    pub fn record_success(&self) {
        let now = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .map(|d| d.as_millis() as u64)
            .unwrap_or(0);
        self.last_success.store(now, Ordering::SeqCst);
        self.failure_count.store(0, Ordering::SeqCst);
        self.is_healthy.store(true, Ordering::SeqCst);
    }

    /// Record a failed operation
    pub fn record_failure(&self) {
        let now = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .map(|d| d.as_millis() as u64)
            .unwrap_or(0);
        self.last_failure.store(now, Ordering::SeqCst);
        let failures = self.failure_count.fetch_add(1, Ordering::SeqCst) + 1;
        if failures >= self.max_failures {
            self.is_healthy.store(false, Ordering::SeqCst);
        }
    }

    /// Get the current health status
    pub fn get_health(&self) -> ConnectionHealth {
        if !self.is_healthy.load(Ordering::SeqCst) {
            return ConnectionHealth::Failed;
        }

        let now = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .map(|d| d.as_millis() as u64)
            .unwrap_or(0);
        let last_success = self.last_success.load(Ordering::SeqCst);

        if last_success == 0 {
            return ConnectionHealth::Unknown;
        }

        let elapsed = now.saturating_sub(last_success);
        if elapsed > self.health_check_interval_ms {
            ConnectionHealth::Degraded
        } else {
            ConnectionHealth::Healthy
        }
    }

    /// Reset the health monitor (e.g., on reconnection)
    pub fn reset(&self) {
        self.last_success.store(0, Ordering::SeqCst);
        self.last_failure.store(0, Ordering::SeqCst);
        self.failure_count.store(0, Ordering::SeqCst);
        self.is_healthy.store(true, Ordering::SeqCst);
    }

    /// Get time since last successful operation in milliseconds
    pub fn time_since_last_success(&self) -> Option<u64> {
        let last = self.last_success.load(Ordering::SeqCst);
        if last == 0 {
            return None;
        }
        let now = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .map(|d| d.as_millis() as u64)
            .unwrap_or(0);
        Some(now.saturating_sub(last))
    }
}

// SAFEARRAY functions from OleAut32.dll
#[link(name = "oleaut32")]
extern "system" {
    fn SafeArrayGetDim(psa: *const SAFEARRAY) -> u32;
    fn SafeArrayGetLBound(psa: *const SAFEARRAY, nDim: u32, plLbound: *mut i32) -> windows::core::HRESULT;
    fn SafeArrayGetUBound(psa: *const SAFEARRAY, nDim: u32, plUbound: *mut i32) -> windows::core::HRESULT;
    fn SafeArrayAccessData(psa: *const SAFEARRAY, ppvData: *mut *mut std::ffi::c_void) -> windows::core::HRESULT;
    fn SafeArrayUnaccessData(psa: *const SAFEARRAY) -> windows::core::HRESULT;
}

const DISPID_PROPERTYPUT: i32 = -3;

/// Initialize COM for the current thread
pub fn init_com() -> Result<(), String> {
    unsafe {
        CoInitializeEx(None, COINIT_APARTMENTTHREADED)
            .map_err(|e| format!("Failed to initialize COM: {}", e))
    }
}

/// Uninitialize COM for the current thread
pub fn uninit_com() {
    unsafe {
        CoUninitialize();
    }
}

/// Discover ASCOM devices by reading the Windows Registry
pub fn discover_devices(device_type: &str) -> Vec<AscomDevice> {
    let mut devices = Vec::new();
    
    let reg_path = format!("SOFTWARE\\ASCOM\\{} Drivers", device_type);
    tracing::info!("Scanning ASCOM registry: {}", reg_path);
    
    if let Some(found) = scan_registry_path(&reg_path) {
        devices.extend(found);
    }
    
    // Also try WOW6432Node for 32-bit drivers on 64-bit Windows
    let reg_path_wow = format!("SOFTWARE\\WOW6432Node\\ASCOM\\{} Drivers", device_type);
    if let Some(found) = scan_registry_path(&reg_path_wow) {
        for dev in found {
            if !devices.iter().any(|d| d.prog_id == dev.prog_id) {
                devices.push(dev);
            }
        }
    }
    
    tracing::info!("Found {} ASCOM {} drivers", devices.len(), device_type);
    devices
}

fn scan_registry_path(reg_path: &str) -> Option<Vec<AscomDevice>> {
    let mut devices = Vec::new();
    
    unsafe {
        let mut key: HKEY = HKEY::default();
        let reg_path_wide: Vec<u16> = reg_path.encode_utf16().chain(std::iter::once(0)).collect();
        
        let result = RegOpenKeyExW(
            HKEY_LOCAL_MACHINE,
            PCWSTR::from_raw(reg_path_wide.as_ptr()),
            0,
            KEY_READ,
            &mut key,
        );
        
        if result.is_err() {
            return None;
        }
        
        let mut index: u32 = 0;
        let mut name_buffer: [u16; 256] = [0; 256];
        
        loop {
            let mut name_len = name_buffer.len() as u32;
            
            let result = RegEnumKeyExW(
                key,
                index,
                PWSTR(name_buffer.as_mut_ptr()),
                &mut name_len,
                None,
                PWSTR::null(),
                None,
                None,
            );
            
            if result.is_err() {
                break;
            }
            
            let prog_id = String::from_utf16_lossy(&name_buffer[..name_len as usize]);
            let registry_description = get_driver_description(&key, &prog_id).unwrap_or_default();

            if !prog_id.is_empty() {
                // NOTE: We intentionally do NOT probe the device here because:
                // 1. Some ASCOM drivers show setup dialogs when COM object is created
                // 2. Probing is slow (creates/destroys COM objects)
                // 3. We can probe later when user actually selects the device
                //
                // The probe_device_name() function is available for on-demand use
                // after user selects a device, if we need the real name.

                let name = if registry_description.is_empty() {
                    prog_id.clone()
                } else {
                    registry_description.clone()
                };

                tracing::info!("Found ASCOM driver: {} - {}", prog_id, registry_description);

                devices.push(AscomDevice {
                    prog_id: prog_id.clone(),
                    name,
                    description: registry_description,
                });
            }
            
            index += 1;
        }
        
        let _ = RegCloseKey(key);
    }
    
    Some(devices)
}

unsafe fn get_driver_description(parent_key: &HKEY, prog_id: &str) -> Option<String> {
    let mut subkey: HKEY = HKEY::default();
    let prog_id_wide: Vec<u16> = prog_id.encode_utf16().chain(std::iter::once(0)).collect();
    
    let result = RegOpenKeyExW(
        *parent_key,
        PCWSTR::from_raw(prog_id_wide.as_ptr()),
        0,
        KEY_READ,
        &mut subkey,
    );
    
    if result.is_err() {
        return None;
    }
    
    let mut data_type: REG_VALUE_TYPE = REG_VALUE_TYPE(0);
    let mut data_buffer: [u8; 512] = [0; 512];
    let mut data_len = data_buffer.len() as u32;
    
    let result = RegQueryValueExW(
        subkey,
        PCWSTR::null(),
        None,
        Some(&mut data_type),
        Some(data_buffer.as_mut_ptr()),
        Some(&mut data_len),
    );
    
    let _ = RegCloseKey(subkey);
    
    if result.is_ok() && data_type == REG_SZ {
        let wide_slice: &[u16] = std::slice::from_raw_parts(
            data_buffer.as_ptr() as *const u16,
            (data_len as usize / 2).saturating_sub(1),
        );
        return Some(String::from_utf16_lossy(wide_slice));
    }
    
    None
}

/// Probe an ASCOM device to get its actual name without connecting
///
/// This instantiates the COM object and reads the Name property, which
/// according to ASCOM standards should be available without setting Connected=true.
/// This allows us to get the real device name (e.g., "ASI1600MM-Cool") instead of
/// the generic registry description (e.g., "ASI Camera (1)").
pub fn probe_device_name(prog_id: &str) -> Option<String> {
    tracing::debug!("Probing ASCOM device name for: {}", prog_id);

    // Try to create the COM object and read Name property
    match AscomDeviceConnection::new(prog_id) {
        Ok(device) => {
            // Read Name property - should work without connecting
            match device.get_string_property("Name") {
                Ok(name) if !name.is_empty() => {
                    tracing::debug!("Probed device name: {} -> {}", prog_id, name);
                    Some(name)
                }
                Ok(_) => {
                    // Empty name, try Description
                    match device.get_string_property("Description") {
                        Ok(desc) if !desc.is_empty() => {
                            tracing::debug!("Probed device description: {} -> {}", prog_id, desc);
                            Some(desc)
                        }
                        _ => None
                    }
                }
                Err(e) => {
                    tracing::debug!("Failed to read Name property for {}: {}", prog_id, e);
                    // Try Description as fallback
                    device.get_string_property("Description").ok()
                }
            }
            // device is dropped here, releasing COM object
        }
        Err(e) => {
            tracing::debug!("Failed to create COM object for {}: {}", prog_id, e);
            None
        }
    }
}

/// Create a VARIANT with a boolean value
fn variant_bool(value: bool) -> VARIANT {
    unsafe {
        let mut var = VARIANT::default();
        (*var.Anonymous.Anonymous).vt = VT_BOOL;
        (*var.Anonymous.Anonymous).Anonymous.boolVal = if value { VARIANT_BOOL(-1) } else { VARIANT_BOOL(0) };
        var
    }
}

/// Create a VARIANT with a double value
fn variant_f64(value: f64) -> VARIANT {
    unsafe {
        let mut var = VARIANT::default();
        (*var.Anonymous.Anonymous).vt = VT_R8;
        (*var.Anonymous.Anonymous).Anonymous.dblVal = value;
        var
    }
}

/// Create a VARIANT with an i32 value
fn variant_i32(value: i32) -> VARIANT {
    unsafe {
        let mut var = VARIANT::default();
        (*var.Anonymous.Anonymous).vt = VT_I4;
        (*var.Anonymous.Anonymous).Anonymous.lVal = value;
        var
    }
}

/// Extract boolean from VARIANT
fn variant_to_bool(var: &VARIANT) -> Option<bool> {
    unsafe {
        if (*var.Anonymous.Anonymous).vt == VT_BOOL {
            Some((*var.Anonymous.Anonymous).Anonymous.boolVal.0 != 0)
        } else {
            None
        }
    }
}

/// Extract f64 from VARIANT
fn variant_to_f64(var: &VARIANT) -> Option<f64> {
    unsafe {
        let vt = (*var.Anonymous.Anonymous).vt;
        if vt == VT_R8 {
            Some((*var.Anonymous.Anonymous).Anonymous.dblVal)
        } else if vt == VT_I4 {
            Some((*var.Anonymous.Anonymous).Anonymous.lVal as f64)
        } else {
            None
        }
    }
}

/// Extract i32 from VARIANT
fn variant_to_i32(var: &VARIANT) -> Option<i32> {
    unsafe {
        if (*var.Anonymous.Anonymous).vt == VT_I4 {
            Some((*var.Anonymous.Anonymous).Anonymous.lVal)
        } else {
            None
        }
    }
}

/// Extract string from VARIANT
fn variant_to_string(var: &VARIANT) -> Option<String> {
    unsafe {
        if (*var.Anonymous.Anonymous).vt == VT_BSTR {
            let bstr = &(*var.Anonymous.Anonymous).Anonymous.bstrVal;
            // BSTR can be dereferenced to get the string
            if bstr.is_empty() {
                return Some(String::new());
            }
            Some(bstr.to_string())
        } else {
            None
        }
    }
}

/// Extract error message from EXCEPINFO structure
/// Returns the bstrDescription if available, otherwise bstrSource, otherwise a generic message
fn excepinfo_to_string(excep: &EXCEPINFO) -> String {
    // Try to get the description first (most useful)
    if !excep.bstrDescription.is_empty() {
        return excep.bstrDescription.to_string();
    }
    // Fall back to source
    if !excep.bstrSource.is_empty() {
        return format!("ASCOM error from {}", excep.bstrSource.to_string());
    }
    // Last resort: use the error code
    if excep.scode != 0 {
        return format!("ASCOM error code: 0x{:08X}", excep.scode);
    }
    if excep.wCode != 0 {
        return format!("ASCOM error code: {}", excep.wCode);
    }
    "Unknown ASCOM error".to_string()
}

/// Extract i32 array from SAFEARRAY in VARIANT
/// Handles both 1D and 2D SAFEARRAYs (some ASCOM drivers use different layouts)
unsafe fn extract_safearray_i32(var: &VARIANT) -> Result<(Vec<i32>, usize, usize), String> {
    let vt = (*var.Anonymous.Anonymous).vt;
    
    // Check if this is an array variant
    if (vt.0 & VT_ARRAY.0) == 0 {
        return Err(format!("VARIANT is not an array type, got vt={}", vt.0));
    }
    
    let psa: *mut SAFEARRAY = (*var.Anonymous.Anonymous).Anonymous.parray;
    if psa.is_null() {
        return Err("SAFEARRAY pointer is null".to_string());
    }
    
    // Get array dimensions
    let dims = SafeArrayGetDim(psa);
    if dims == 0 {
        return Err("SAFEARRAY has 0 dimensions".to_string());
    }
    
    if dims > 2 {
        return Err(format!("SAFEARRAY has {} dimensions, expected 1 or 2", dims));
    }
    
    // Get bounds for each dimension
    let mut lower1: i32 = 0;
    let mut upper1: i32 = 0;
    if SafeArrayGetLBound(psa, 1, &mut lower1).is_err() {
        return Err("Failed to get lower bound for dimension 1".to_string());
    }
    if SafeArrayGetUBound(psa, 1, &mut upper1).is_err() {
        return Err("Failed to get upper bound for dimension 1".to_string());
    }
    
    // Validate bounds to prevent integer overflow and stack overflow
    if upper1 < lower1 {
        return Err(format!("Invalid bounds: upper1 ({}) < lower1 ({})", upper1, lower1));
    }
    
    // Check for reasonable dimension size (individual dimension up to 15000 pixels is generous)
    // This prevents overflow when multiplying dimensions while still supporting large sensors
    let dim1_diff = upper1.saturating_sub(lower1);
    if dim1_diff > 15_000 {
        return Err(format!(
            "Dimension 1 size {} exceeds maximum 15000 pixels per dimension",
            dim1_diff + 1
        ));
    }
    
    let dim1_size = (dim1_diff + 1) as usize;
    let mut dim2_size = 1;
    
    if dims == 2 {
        let mut lower2: i32 = 0;
        let mut upper2: i32 = 0;
        if SafeArrayGetLBound(psa, 2, &mut lower2).is_err() {
            return Err("Failed to get lower bound for dimension 2".to_string());
        }
        if SafeArrayGetUBound(psa, 2, &mut upper2).is_err() {
            return Err("Failed to get upper bound for dimension 2".to_string());
        }
        
        // Validate bounds for dimension 2
        if upper2 < lower2 {
            return Err(format!("Invalid bounds: upper2 ({}) < lower2 ({})", upper2, lower2));
        }
        
        let dim2_diff = upper2.saturating_sub(lower2);
        if dim2_diff > 15_000 {
            return Err(format!(
                "Dimension 2 size {} exceeds maximum 15000 pixels per dimension",
                dim2_diff + 1
            ));
        }
        
        dim2_size = (dim2_diff + 1) as usize;
    }
    
    // Validate total size to prevent stack overflow and excessive memory allocation
    // Support large camera sensors (e.g., 100MP = 10000x10000 = 100M pixels)
    // At 4 bytes per i32, 100M elements = 400MB which is reasonable for modern systems
    // For 16-bit sensors, we can support up to ~150M pixels (600MB)
    const MAX_ELEMENTS: usize = 150_000_000; // ~600MB for i32, supports very large sensors

    // Use checked arithmetic to prevent overflow
    let size = dim1_size.checked_mul(dim2_size)
        .ok_or_else(|| format!(
            "Array size overflow: {} x {} exceeds maximum computable size",
            dim1_size, dim2_size
        ))?;

    if size > MAX_ELEMENTS {
        return Err(format!(
            "Array size {} elements ({} x {}) exceeds maximum {} elements (~{}MB)",
            size, dim1_size, dim2_size, MAX_ELEMENTS, MAX_ELEMENTS * 4 / (1024 * 1024)
        ));
    }
    
    // Access the raw data
    let mut data_ptr: *mut std::ffi::c_void = std::ptr::null_mut();
    if SafeArrayAccessData(psa, &mut data_ptr).is_err() {
        return Err("Failed to access SAFEARRAY data".to_string());
    }
    
    if data_ptr.is_null() {
        let _ = SafeArrayUnaccessData(psa);
        return Err("SAFEARRAY data pointer is null".to_string());
    }
    
    // Determine the element type and copy data
    let base_vt = vt.0 & !VT_ARRAY.0;
    let result = if base_vt == VT_I4.0 {
        // Data is i32 array
        let slice = std::slice::from_raw_parts(data_ptr as *const i32, size);
        Ok(slice.to_vec())
    } else if base_vt == VT_I2.0 {
        // Data is i16 array (convert to i32)
        let slice = std::slice::from_raw_parts(data_ptr as *const i16, size);
        Ok(slice.iter().map(|&x| x as i32).collect())
    } else if base_vt == VT_UI2.0 {
        // Data is u16 array (convert to i32)
        let slice = std::slice::from_raw_parts(data_ptr as *const u16, size);
        Ok(slice.iter().map(|&x| x as i32).collect())
    } else if base_vt == VT_VARIANT.0 {
        // Array of variants - need to extract each one
        let slice = std::slice::from_raw_parts(data_ptr as *const VARIANT, size);
        let mut result = Vec::with_capacity(size);
        for variant in slice {
            if let Some(val) = variant_to_i32(variant) {
                result.push(val);
            } else if let Some(val) = variant_to_f64(variant) {
                result.push(val as i32);
            } else {
                // Skip invalid values or use 0
                result.push(0);
            }
        }
        Ok(result)
    } else {
        Err(format!("Unsupported SAFEARRAY element type: vt={}", base_vt))
    };
    
    // Unaccess the data
    let _ = SafeArrayUnaccessData(psa);
    
    result.map(|data| (data, dim1_size, dim2_size))
}

/// Extract string array from SAFEARRAY in VARIANT
unsafe fn extract_safearray_string(var: &VARIANT) -> Result<Vec<String>, String> {
    let vt = (*var.Anonymous.Anonymous).vt;
    
    // Check if this is an array variant
    if (vt.0 & VT_ARRAY.0) == 0 {
        return Err(format!("VARIANT is not an array type, got vt={}", vt.0));
    }
    
    let psa: *mut SAFEARRAY = (*var.Anonymous.Anonymous).Anonymous.parray;
    if psa.is_null() {
        return Err("SAFEARRAY pointer is null".to_string());
    }
    
    // Get array dimensions
    let dims = SafeArrayGetDim(psa);
    if dims == 0 {
        return Err("SAFEARRAY has 0 dimensions".to_string());
    }
    
    // Get bounds
    let mut lower: i32 = 0;
    let mut upper: i32 = 0;
    if SafeArrayGetLBound(psa, 1, &mut lower).is_err() {
        return Err("Failed to get lower bound".to_string());
    }
    if SafeArrayGetUBound(psa, 1, &mut upper).is_err() {
        return Err("Failed to get upper bound".to_string());
    }
    
    // Validate bounds to prevent integer overflow and stack overflow
    if upper < lower {
        return Err(format!("Invalid bounds: upper ({}) < lower ({})", upper, lower));
    }
    
    // Check for potential integer overflow
    let diff = upper.saturating_sub(lower);
    if diff > 10_000_000 {
        return Err(format!("Array size too large: {}", diff + 1));
    }
    
    // Validate total size to prevent stack overflow and excessive memory allocation
    // Limit to ~100MB for safety (assuming BSTR/VARIANT elements)
    const MAX_ELEMENTS: usize = 1_000_000; // Conservative limit for string arrays
    let size = (diff + 1) as usize;
    
    if size > MAX_ELEMENTS {
        return Err(format!("Array size too large: {} elements (max: {})", size, MAX_ELEMENTS));
    }
    
    // Access the raw data
    let mut data_ptr: *mut std::ffi::c_void = std::ptr::null_mut();
    if SafeArrayAccessData(psa, &mut data_ptr).is_err() {
        return Err("Failed to access SAFEARRAY data".to_string());
    }
    
    let base_vt = vt.0 & !VT_ARRAY.0;
    let result = if base_vt == VT_BSTR.0 {
        // Array of BSTRs
        let slice = std::slice::from_raw_parts(data_ptr as *const windows::core::BSTR, size);
        let mut strings = Vec::with_capacity(size);
        for bstr in slice {
            strings.push(bstr.to_string());
        }
        Ok(strings)
    } else if base_vt == VT_VARIANT.0 {
        // Array of Variants containing strings
        let slice = std::slice::from_raw_parts(data_ptr as *const VARIANT, size);
        let mut strings = Vec::with_capacity(size);
        for variant in slice {
            if let Some(s) = variant_to_string(variant) {
                strings.push(s);
            } else {
                strings.push(String::new());
            }
        }
        Ok(strings)
    } else {
        Err(format!("Unsupported SAFEARRAY element type for strings: vt={}", base_vt))
    };
    
    let _ = SafeArrayUnaccessData(psa);
    
    result
}

/// ASCOM Device wrapper for COM interaction
///
/// This struct provides a safe wrapper around COM IDispatch for ASCOM devices.
/// It includes:
/// - Connection state tracking
/// - Health monitoring for detecting disconnected devices
/// - RAII cleanup via Drop trait
pub struct AscomDeviceConnection {
    dispatch: IDispatch,
    connected: bool,
    /// Health monitor for tracking device responsiveness
    health: HealthMonitor,
    /// ProgID for logging/diagnostics
    prog_id: String,
}

impl AscomDeviceConnection {
    /// Create a new ASCOM device connection
    pub fn new(prog_id: &str) -> Result<Self, String> {
        unsafe {
            let prog_id_wide: Vec<u16> = prog_id.encode_utf16().chain(std::iter::once(0)).collect();

            let clsid = CLSIDFromProgID(PCWSTR::from_raw(prog_id_wide.as_ptr()))
                .map_err(|e| format!("Failed to get CLSID for {}: {}", prog_id, e))?;

            let dispatch: IDispatch = CoCreateInstance(&clsid, None, CLSCTX_ALL)
                .map_err(|e| format!("Failed to create COM object {}: {}", prog_id, e))?;

            tracing::info!("Created ASCOM COM object for: {}", prog_id);

            Ok(Self {
                dispatch,
                connected: false,
                health: HealthMonitor::default(),
                prog_id: prog_id.to_string(),
            })
        }
    }

    /// Get the connection health status
    pub fn get_health(&self) -> ConnectionHealth {
        self.health.get_health()
    }

    /// Check if the device is healthy (responding to commands)
    pub fn is_healthy(&self) -> bool {
        matches!(self.health.get_health(), ConnectionHealth::Healthy | ConnectionHealth::Unknown)
    }

    /// Perform a heartbeat check by reading the Connected property
    /// This should be called periodically to verify device is still responding
    pub fn heartbeat(&self) -> Result<(), String> {
        match self.get_bool_property("Connected") {
            Ok(_) => {
                self.health.record_success();
                Ok(())
            }
            Err(e) => {
                self.health.record_failure();
                Err(e)
            }
        }
    }

    pub fn connect(&mut self) -> Result<(), String> {
        self.health.reset(); // Reset health state on new connection
        self.set_bool_property("Connected", true)?;
        self.connected = true;
        self.health.record_success();
        tracing::info!("ASCOM device {} connected", self.prog_id);
        Ok(())
    }

    pub fn disconnect(&mut self) -> Result<(), String> {
        self.set_bool_property("Connected", false)?;
        self.connected = false;
        tracing::info!("ASCOM device {} disconnected", self.prog_id);
        Ok(())
    }

    pub fn is_connected(&self) -> Result<bool, String> {
        self.get_bool_property("Connected")
    }
    
    fn get_dispid(&self, name: &str) -> Result<i32, String> {
        unsafe {
            let name_wide: Vec<u16> = name.encode_utf16().chain(std::iter::once(0)).collect();
            let names = [PCWSTR::from_raw(name_wide.as_ptr())];
            let mut dispid: i32 = 0;
            
            self.dispatch.GetIDsOfNames(
                &GUID::zeroed(),
                names.as_ptr(),
                1,
                0,
                &mut dispid,
            ).map_err(|e| format!("Failed to get DISPID for {}: {}", name, e))?;
            
            Ok(dispid)
        }
    }
    
    pub fn get_string_property(&self, name: &str) -> Result<String, String> {
        unsafe {
            let dispid = self.get_dispid(name)?;
            let mut result = VARIANT::default();
            let params = DISPPARAMS::default();
            
            self.dispatch.Invoke(
                dispid,
                &GUID::zeroed(),
                0,
                DISPATCH_PROPERTYGET,
                &params,
                Some(&mut result),
                None,
                None,
            ).map_err(|e| format!("Failed to get property {}: {}", name, e))?;
            
            variant_to_string(&result).ok_or_else(|| format!("Property {} is not a string", name))
        }
    }
    
    pub fn get_bool_property(&self, name: &str) -> Result<bool, String> {
        unsafe {
            let dispid = self.get_dispid(name)?;
            let mut result = VARIANT::default();
            let params = DISPPARAMS::default();
            
            self.dispatch.Invoke(
                dispid,
                &GUID::zeroed(),
                0,
                DISPATCH_PROPERTYGET,
                &params,
                Some(&mut result),
                None,
                None,
            ).map_err(|e| format!("Failed to get property {}: {}", name, e))?;
            
            variant_to_bool(&result).ok_or_else(|| format!("Property {} is not a bool", name))
        }
    }
    
    pub fn set_bool_property(&self, name: &str, value: bool) -> Result<(), String> {
        unsafe {
            let dispid = self.get_dispid(name)?;
            let mut arg = variant_bool(value);
            let mut dispid_named = DISPID_PROPERTYPUT;
            
            let params = DISPPARAMS {
                rgvarg: &mut arg,
                rgdispidNamedArgs: &mut dispid_named,
                cArgs: 1,
                cNamedArgs: 1,
            };
            
            self.dispatch.Invoke(
                dispid,
                &GUID::zeroed(),
                0,
                DISPATCH_PROPERTYPUT,
                &params,
                None,
                None,
                None,
            ).map_err(|e| format!("Failed to set property {}: {}", name, e))?;
            
            Ok(())
        }
    }
    
    pub fn get_double_property(&self, name: &str) -> Result<f64, String> {
        unsafe {
            let dispid = self.get_dispid(name)?;
            let mut result = VARIANT::default();
            let params = DISPPARAMS::default();
            
            self.dispatch.Invoke(
                dispid,
                &GUID::zeroed(),
                0,
                DISPATCH_PROPERTYGET,
                &params,
                Some(&mut result),
                None,
                None,
            ).map_err(|e| format!("Failed to get property {}: {}", name, e))?;
            
            variant_to_f64(&result).ok_or_else(|| format!("Property {} is not a double", name))
        }
    }
    
    pub fn set_double_property(&self, name: &str, value: f64) -> Result<(), String> {
        unsafe {
            let dispid = self.get_dispid(name)?;
            let mut arg = variant_f64(value);
            let mut dispid_named = DISPID_PROPERTYPUT;
            
            let params = DISPPARAMS {
                rgvarg: &mut arg,
                rgdispidNamedArgs: &mut dispid_named,
                cArgs: 1,
                cNamedArgs: 1,
            };
            
            self.dispatch.Invoke(
                dispid,
                &GUID::zeroed(),
                0,
                DISPATCH_PROPERTYPUT,
                &params,
                None,
                None,
                None,
            ).map_err(|e| format!("Failed to set property {}: {}", name, e))?;
            
            Ok(())
        }
    }
    
    pub fn get_int_property(&self, name: &str) -> Result<i32, String> {
        unsafe {
            let dispid = self.get_dispid(name)?;
            let mut result = VARIANT::default();
            let params = DISPPARAMS::default();
            
            self.dispatch.Invoke(
                dispid,
                &GUID::zeroed(),
                0,
                DISPATCH_PROPERTYGET,
                &params,
                Some(&mut result),
                None,
                None,
            ).map_err(|e| format!("Failed to get property {}: {}", name, e))?;
            
            variant_to_i32(&result).ok_or_else(|| format!("Property {} is not an int", name))
        }
    }
    
    pub fn set_int_property(&self, name: &str, value: i32) -> Result<(), String> {
        unsafe {
            let dispid = self.get_dispid(name)?;
            let mut arg = variant_i32(value);
            let mut dispid_named = DISPID_PROPERTYPUT;
            
            let params = DISPPARAMS {
                rgvarg: &mut arg,
                rgdispidNamedArgs: &mut dispid_named,
                cArgs: 1,
                cNamedArgs: 1,
            };
            
            self.dispatch.Invoke(
                dispid,
                &GUID::zeroed(),
                0,
                DISPATCH_PROPERTYPUT,
                &params,
                None,
                None,
                None,
            ).map_err(|e| format!("Failed to set property {}: {}", name, e))?;
            
            Ok(())
        }
    }
    
    pub fn call_method(&self, name: &str) -> Result<(), String> {
        unsafe {
            let dispid = self.get_dispid(name)?;
            let params = DISPPARAMS::default();

            // Capture exception info for better error messages
            let mut excep_info = EXCEPINFO::default();

            let result = self.dispatch.Invoke(
                dispid,
                &GUID::zeroed(),
                0,
                DISPATCH_METHOD,
                &params,
                None,
                Some(&mut excep_info),
                None,
            );

            if let Err(e) = result {
                // Check if we have exception info with a better message
                let excep_msg = excepinfo_to_string(&excep_info);
                if excep_msg != "Unknown ASCOM error" {
                    return Err(format!("Failed to call method {}: {}", name, excep_msg));
                }
                return Err(format!("Failed to call method {}: {}", name, e));
            }

            Ok(())
        }
    }

    pub fn get_string_array_property(&self, name: &str) -> Result<Vec<String>, String> {
        unsafe {
            let dispid = self.get_dispid(name)?;
            let mut result = VARIANT::default();
            let params = DISPPARAMS::default();
            
            self.dispatch.Invoke(
                dispid,
                &GUID::zeroed(),
                0,
                DISPATCH_PROPERTYGET,
                &params,
                Some(&mut result),
                None,
                None,
            ).map_err(|e| format!("Failed to get property {}: {}", name, e))?;
            
            extract_safearray_string(&result)
        }
    }
    
    pub fn call_method_2_double(&self, name: &str, arg1: f64, arg2: f64) -> Result<(), String> {
        unsafe {
            let dispid = self.get_dispid(name)?;

            // Arguments are passed in reverse order
            let mut args = [variant_f64(arg2), variant_f64(arg1)];

            let params = DISPPARAMS {
                rgvarg: args.as_mut_ptr(),
                rgdispidNamedArgs: ptr::null_mut(),
                cArgs: 2,
                cNamedArgs: 0,
            };

            // Capture exception info for better error messages
            let mut excep_info = EXCEPINFO::default();

            let result = self.dispatch.Invoke(
                dispid,
                &GUID::zeroed(),
                0,
                DISPATCH_METHOD,
                &params,
                None,
                Some(&mut excep_info),
                None,
            );

            if let Err(e) = result {
                // Check if we have exception info with a better message
                let excep_msg = excepinfo_to_string(&excep_info);
                if excep_msg != "Unknown ASCOM error" {
                    return Err(format!("Failed to call method {}: {}", name, excep_msg));
                }
                return Err(format!("Failed to call method {}: {}", name, e));
            }

            Ok(())
        }
    }

    pub fn call_method_1_double(&self, name: &str, arg: f64) -> Result<(), String> {
        unsafe {
            let dispid = self.get_dispid(name)?;
            let mut args = [variant_f64(arg)];

            let params = DISPPARAMS {
                rgvarg: args.as_mut_ptr(),
                rgdispidNamedArgs: ptr::null_mut(),
                cArgs: 1,
                cNamedArgs: 0,
            };

            // Capture exception info for better error messages
            let mut excep_info = EXCEPINFO::default();

            let result = self.dispatch.Invoke(
                dispid,
                &GUID::zeroed(),
                0,
                DISPATCH_METHOD,
                &params,
                None,
                Some(&mut excep_info),
                None,
            );

            if let Err(e) = result {
                let excep_msg = excepinfo_to_string(&excep_info);
                if excep_msg != "Unknown ASCOM error" {
                    return Err(format!("Failed to call method {}: {}", name, excep_msg));
                }
                return Err(format!("Failed to call method {}: {}", name, e));
            }

            Ok(())
        }
    }

    pub fn call_method_1_int(&self, name: &str, arg: i32) -> Result<(), String> {
        unsafe {
            let dispid = self.get_dispid(name)?;
            let mut args = [variant_i32(arg)];

            let params = DISPPARAMS {
                rgvarg: args.as_mut_ptr(),
                rgdispidNamedArgs: ptr::null_mut(),
                cArgs: 1,
                cNamedArgs: 0,
            };

            // Capture exception info for better error messages
            let mut excep_info = EXCEPINFO::default();

            let result = self.dispatch.Invoke(
                dispid,
                &GUID::zeroed(),
                0,
                DISPATCH_METHOD,
                &params,
                None,
                Some(&mut excep_info),
                None,
            );

            if let Err(e) = result {
                let excep_msg = excepinfo_to_string(&excep_info);
                if excep_msg != "Unknown ASCOM error" {
                    return Err(format!("Failed to call method {}: {}", name, excep_msg));
                }
                return Err(format!("Failed to call method {}: {}", name, e));
            }

            Ok(())
        }
    }

    /// Call a method with one integer argument that returns a boolean
    /// Used for ASCOM methods like CanMoveAxis(TelescopeAxes) -> Boolean
    pub fn call_method_1_int_return_bool(&self, name: &str, arg: i32) -> Result<bool, String> {
        unsafe {
            let dispid = self.get_dispid(name)?;
            let mut args = [variant_i32(arg)];

            let params = DISPPARAMS {
                rgvarg: args.as_mut_ptr(),
                rgdispidNamedArgs: ptr::null_mut(),
                cArgs: 1,
                cNamedArgs: 0,
            };

            // Capture exception info and result
            let mut excep_info = EXCEPINFO::default();
            let mut result_var = VARIANT::default();

            let result = self.dispatch.Invoke(
                dispid,
                &GUID::zeroed(),
                0,
                DISPATCH_METHOD,
                &params,
                Some(&mut result_var),
                Some(&mut excep_info),
                None,
            );

            if let Err(e) = result {
                let excep_msg = excepinfo_to_string(&excep_info);
                if excep_msg != "Unknown ASCOM error" {
                    return Err(format!("Failed to call method {}: {}", name, excep_msg));
                }
                return Err(format!("Failed to call method {}: {}", name, e));
            }

            // Extract boolean result
            variant_to_bool(&result_var)
                .ok_or_else(|| format!("Method {} did not return a boolean", name))
        }
    }

    pub fn call_method_2_int(&self, name: &str, arg1: i32, arg2: i32) -> Result<(), String> {
        unsafe {
            let dispid = self.get_dispid(name)?;

            // Arguments are passed in reverse order
            let mut args = [variant_i32(arg2), variant_i32(arg1)];

            let params = DISPPARAMS {
                rgvarg: args.as_mut_ptr(),
                rgdispidNamedArgs: ptr::null_mut(),
                cArgs: 2,
                cNamedArgs: 0,
            };

            // Capture exception info for better error messages
            let mut excep_info = EXCEPINFO::default();

            let result = self.dispatch.Invoke(
                dispid,
                &GUID::zeroed(),
                0,
                DISPATCH_METHOD,
                &params,
                None,
                Some(&mut excep_info),
                None,
            );

            if let Err(e) = result {
                let excep_msg = excepinfo_to_string(&excep_info);
                if excep_msg != "Unknown ASCOM error" {
                    return Err(format!("Failed to call method {}: {}", name, excep_msg));
                }
                return Err(format!("Failed to call method {}: {}", name, e));
            }

            Ok(())
        }
    }

    pub fn call_method_2_double_bool(&self, name: &str, arg1: f64, arg2: bool) -> Result<(), String> {
        unsafe {
            let dispid = self.get_dispid(name)?;

            // Arguments are passed in reverse order
            let mut args = [variant_bool(arg2), variant_f64(arg1)];

            let params = DISPPARAMS {
                rgvarg: args.as_mut_ptr(),
                rgdispidNamedArgs: ptr::null_mut(),
                cArgs: 2,
                cNamedArgs: 0,
            };

            // Capture exception info for better error messages
            let mut excep_info = EXCEPINFO::default();

            let result = self.dispatch.Invoke(
                dispid,
                &GUID::zeroed(),
                0,
                DISPATCH_METHOD,
                &params,
                None,
                Some(&mut excep_info),
                None,
            );

            if let Err(e) = result {
                let excep_msg = excepinfo_to_string(&excep_info);
                if excep_msg != "Unknown ASCOM error" {
                    return Err(format!("Failed to call method {}: {}", name, excep_msg));
                }
                return Err(format!("Failed to call method {}: {}", name, e));
            }

            Ok(())
        }
    }

    /// Call a method with an int and a double argument (e.g., MoveAxis)
    pub fn call_method_int_double(&self, name: &str, arg1: i32, arg2: f64) -> Result<(), String> {
        unsafe {
            let dispid = self.get_dispid(name)?;

            // Arguments are passed in reverse order
            let mut args = [variant_f64(arg2), variant_i32(arg1)];

            let params = DISPPARAMS {
                rgvarg: args.as_mut_ptr(),
                rgdispidNamedArgs: ptr::null_mut(),
                cArgs: 2,
                cNamedArgs: 0,
            };

            // Capture exception info for better error messages
            let mut excep_info = EXCEPINFO::default();

            let result = self.dispatch.Invoke(
                dispid,
                &GUID::zeroed(),
                0,
                DISPATCH_METHOD,
                &params,
                None,
                Some(&mut excep_info),
                None,
            );

            if let Err(e) = result {
                let excep_msg = excepinfo_to_string(&excep_info);
                if excep_msg != "Unknown ASCOM error" {
                    return Err(format!("Failed to call method {}: {}", name, excep_msg));
                }
                return Err(format!("Failed to call method {}: {}", name, e));
            }

            Ok(())
        }
    }

    /// Call a parameterless method (e.g., SetupDialog) on the COM object
    pub fn call_method_0(&self, name: &str) -> Result<(), String> {
        unsafe {
            let dispid = self.get_dispid(name)?;

            let params = DISPPARAMS {
                rgvarg: ptr::null_mut(),
                rgdispidNamedArgs: ptr::null_mut(),
                cArgs: 0,
                cNamedArgs: 0,
            };

            // Capture exception info for better error messages
            let mut excep_info = EXCEPINFO::default();

            let result = self.dispatch.Invoke(
                dispid,
                &GUID::zeroed(),
                0,
                DISPATCH_METHOD,
                &params,
                None,
                Some(&mut excep_info),
                None,
            );

            if let Err(e) = result {
                let excep_msg = excepinfo_to_string(&excep_info);
                if excep_msg != "Unknown ASCOM error" {
                    return Err(format!("Failed to call method {}: {}", name, excep_msg));
                }
                return Err(format!("Failed to call method {}: {}", name, e));
            }

            Ok(())
        }
    }
}

impl Drop for AscomDeviceConnection {
    fn drop(&mut self) {
        // RAII cleanup: always attempt to disconnect if connected
        // This ensures COM resources are properly released even on error paths
        if self.connected {
            tracing::debug!("AscomDeviceConnection::drop - disconnecting {}", self.prog_id);
            if let Err(e) = self.disconnect() {
                // Log but don't panic in Drop
                tracing::warn!(
                    "Failed to disconnect ASCOM device {} during cleanup: {}",
                    self.prog_id, e
                );
            }
        }
        // The IDispatch handle will be released when the dispatch field is dropped
        // Windows COM reference counting handles the actual cleanup
        tracing::debug!("AscomDeviceConnection::drop - cleaned up {}", self.prog_id);
    }
}

// SAFETY: COM objects are apartment-threaded and we manage thread affinity ourselves.
// All COM calls should happen from the thread that called CoInitialize.
// The wrapper thread pattern used in ascom_wrapper*.rs ensures this.
unsafe impl Send for AscomDeviceConnection {}
unsafe impl Sync for AscomDeviceConnection {}

// ============================================================================
// ASCOM Operation Guard
// ============================================================================

/// RAII guard that ensures ASCOM device cleanup when operations fail.
///
/// This guard calls disconnect on the device if dropped without being defused.
/// Use this for multi-step operations where you need to ensure cleanup even
/// if an intermediate step fails.
///
/// # Example
/// ```ignore
/// let mut mount = AscomMount::new(&prog_id)?;
/// mount.connect()?;
///
/// // Create guard - will disconnect on drop if not defused
/// let guard = AscomOperationGuard::new(&mut mount as &mut dyn AscomDisconnectable, "slew");
///
/// // Perform operations
/// mount.slew_to_coordinates(ra, dec)?;
///
/// // Operation succeeded - defuse the guard
/// guard.defuse();
/// mount.disconnect()?;
/// ```
pub struct AscomOperationGuard<'a> {
    device: Option<&'a mut dyn AscomDisconnectable>,
    operation: String,
}

/// Trait for ASCOM devices that can be disconnected
pub trait AscomDisconnectable {
    /// Disconnect from the device (best-effort cleanup)
    fn try_disconnect(&mut self) -> Result<(), String>;
}

impl AscomDisconnectable for AscomDeviceConnection {
    fn try_disconnect(&mut self) -> Result<(), String> {
        self.disconnect()
    }
}

impl<'a> AscomOperationGuard<'a> {
    /// Create a new operation guard for the given device.
    pub fn new(device: &'a mut dyn AscomDisconnectable, operation: impl Into<String>) -> Self {
        Self {
            device: Some(device),
            operation: operation.into(),
        }
    }

    /// Defuse the guard, preventing automatic cleanup on drop.
    /// Call this when the operation succeeds.
    pub fn defuse(mut self) {
        self.device = None;
    }
}

impl<'a> Drop for AscomOperationGuard<'a> {
    fn drop(&mut self) {
        if let Some(device) = self.device.take() {
            tracing::warn!(
                "AscomOperationGuard: operation '{}' did not complete - disconnecting",
                self.operation
            );
            if let Err(e) = device.try_disconnect() {
                tracing::error!(
                    "AscomOperationGuard: failed to disconnect after failed '{}': {}",
                    self.operation, e
                );
            }
        }
    }
}

/// Synchronous cleanup guard for use in ASCOM connect sequences.
///
/// This guard runs a cleanup closure if dropped without being defused.
/// Useful for cleaning up partially-initialized state when connect fails.
///
/// # Example
/// ```ignore
/// // Open device
/// let device = AscomDeviceConnection::new(&prog_id)?;
///
/// // Create guard that will disconnect if subsequent operations fail
/// let guard = AscomCleanupGuard::new(|| {
///     let _ = device.disconnect();
/// });
///
/// // Do more initialization
/// device.connect()?;
/// device.setup_something()?;
///
/// // Success - defuse the guard
/// guard.defuse();
/// ```
pub struct AscomCleanupGuard<F: FnOnce()> {
    cleanup: Option<F>,
}

impl<F: FnOnce()> AscomCleanupGuard<F> {
    /// Create a new cleanup guard with the given cleanup function.
    pub fn new(cleanup: F) -> Self {
        Self {
            cleanup: Some(cleanup),
        }
    }

    /// Defuse the guard, preventing the cleanup function from running.
    pub fn defuse(mut self) {
        self.cleanup = None;
    }
}

impl<F: FnOnce()> Drop for AscomCleanupGuard<F> {
    fn drop(&mut self) {
        if let Some(cleanup) = self.cleanup.take() {
            cleanup();
        }
    }
}

/// ASCOM Camera
pub struct AscomCamera {
    device: AscomDeviceConnection,
}

impl AscomCamera {
    pub fn new(prog_id: &str) -> Result<Self, String> {
        Ok(Self {
            device: AscomDeviceConnection::new(prog_id)?,
        })
    }
    
    pub fn connect(&mut self) -> Result<(), String> {
        self.device.connect()
    }
    
    pub fn disconnect(&mut self) -> Result<(), String> {
        self.device.disconnect()
    }

    /// Show the ASCOM driver setup dialog to let the user choose the device/config
    pub fn setup_dialog(&mut self) -> Result<(), String> {
        self.device.call_method_0("SetupDialog")
    }
    
    pub fn name(&self) -> Result<String, String> {
        self.device.get_string_property("Name")
    }
    
    pub fn description(&self) -> Result<String, String> {
        self.device.get_string_property("Description")
    }
    
    pub fn camera_x_size(&self) -> Result<i32, String> {
        self.device.get_int_property("CameraXSize")
    }
    
    pub fn camera_y_size(&self) -> Result<i32, String> {
        self.device.get_int_property("CameraYSize")
    }
    
    pub fn pixel_size_x(&self) -> Result<f64, String> {
        self.device.get_double_property("PixelSizeX")
    }
    
    pub fn pixel_size_y(&self) -> Result<f64, String> {
        self.device.get_double_property("PixelSizeY")
    }
    
    pub fn max_bin_x(&self) -> Result<i32, String> {
        self.device.get_int_property("MaxBinX")
    }
    
    pub fn max_bin_y(&self) -> Result<i32, String> {
        self.device.get_int_property("MaxBinY")
    }
    
    pub fn bin_x(&self) -> Result<i32, String> {
        self.device.get_int_property("BinX")
    }
    
    pub fn bin_y(&self) -> Result<i32, String> {
        self.device.get_int_property("BinY")
    }
    
    pub fn set_bin_x(&mut self, value: i32) -> Result<(), String> {
        self.device.set_int_property("BinX", value)
    }
    
    pub fn set_bin_y(&mut self, value: i32) -> Result<(), String> {
        self.device.set_int_property("BinY", value)
    }
    
    pub fn can_set_ccd_temperature(&self) -> Result<bool, String> {
        self.device.get_bool_property("CanSetCCDTemperature")
    }
    
    pub fn ccd_temperature(&self) -> Result<f64, String> {
        self.device.get_double_property("CCDTemperature")
    }
    
    pub fn set_ccd_temperature(&mut self, temp: f64) -> Result<(), String> {
        self.device.set_double_property("SetCCDTemperature", temp)
    }
    
    pub fn cooler_on(&self) -> Result<bool, String> {
        self.device.get_bool_property("CoolerOn")
    }
    
    pub fn set_cooler_on(&mut self, on: bool) -> Result<(), String> {
        self.device.set_bool_property("CoolerOn", on)
    }
    
    pub fn cooler_power(&self) -> Result<f64, String> {
        self.device.get_double_property("CoolerPower")
    }
    
    pub fn gain(&self) -> Result<i32, String> {
        self.device.get_int_property("Gain")
    }
    
    pub fn set_gain(&mut self, gain: i32) -> Result<(), String> {
        self.device.set_int_property("Gain", gain)
    }
    
    pub fn offset(&self) -> Result<i32, String> {
        self.device.get_int_property("Offset")
    }
    
    pub fn set_offset(&mut self, offset: i32) -> Result<(), String> {
        self.device.set_int_property("Offset", offset)
    }
    
    pub fn camera_state(&self) -> Result<i32, String> {
        self.device.get_int_property("CameraState")
    }
    
    pub fn image_ready(&self) -> Result<bool, String> {
        self.device.get_bool_property("ImageReady")
    }
    
    pub fn start_exposure(&mut self, duration: f64, light: bool) -> Result<(), String> {
        self.device.call_method_2_double_bool("StartExposure", duration, light)
    }
    
    pub fn abort_exposure(&mut self) -> Result<(), String> {
        self.device.call_method("AbortExposure")
    }
    
    pub fn stop_exposure(&mut self) -> Result<(), String> {
        self.device.call_method("StopExposure")
    }
    
    /// Get the image array from the camera
    /// Returns (pixel_data, dim1_size, dim2_size)
    /// Extracts the SAFEARRAY from the ASCOM ImageArray property
    pub fn image_array(&self) -> Result<(Vec<i32>, usize, usize), String> {
        unsafe {
            let dispid = self.device.get_dispid("ImageArray")?;
            let mut result = VARIANT::default();
            let params = DISPPARAMS::default();
            
            self.device.dispatch.Invoke(
                dispid,
                &GUID::zeroed(),
                0,
                DISPATCH_PROPERTYGET,
                &params,
                Some(&mut result),
                None,
                None,
            ).map_err(|e| format!("Failed to get ImageArray property: {}", e))?;
            
            // Extract SAFEARRAY from VARIANT
            extract_safearray_i32(&result)
        }
    }

    pub fn readout_modes(&self) -> Result<Vec<String>, String> {
        self.device.get_string_array_property("ReadoutModes")
    }

    pub fn set_readout_mode(&mut self, mode: i32) -> Result<(), String> {
        self.device.set_int_property("ReadoutMode", mode)
    }

    pub fn sensor_type(&self) -> Result<i32, String> {
        self.device.get_int_property("SensorType")
    }

    pub fn bayer_offset_x(&self) -> Result<i32, String> {
        self.device.get_int_property("BayerOffsetX")
    }

    pub fn bayer_offset_y(&self) -> Result<i32, String> {
        self.device.get_int_property("BayerOffsetY")
    }

    pub fn start_x(&self) -> Result<i32, String> {
        self.device.get_int_property("StartX")
    }

    pub fn start_y(&self) -> Result<i32, String> {
        self.device.get_int_property("StartY")
    }

    pub fn num_x(&self) -> Result<i32, String> {
        self.device.get_int_property("NumX")
    }

    pub fn num_y(&self) -> Result<i32, String> {
        self.device.get_int_property("NumY")
    }

    pub fn set_start_x(&mut self, value: i32) -> Result<(), String> {
        self.device.set_int_property("StartX", value)
    }

    pub fn set_start_y(&mut self, value: i32) -> Result<(), String> {
        self.device.set_int_property("StartY", value)
    }

    pub fn set_num_x(&mut self, value: i32) -> Result<(), String> {
        self.device.set_int_property("NumX", value)
    }

    pub fn set_num_y(&mut self, value: i32) -> Result<(), String> {
        self.device.set_int_property("NumY", value)
    }

    pub fn can_abort_exposure(&self) -> Result<bool, String> {
        self.device.get_bool_property("CanAbortExposure")
    }

    pub fn can_stop_exposure(&self) -> Result<bool, String> {
        self.device.get_bool_property("CanStopExposure")
    }

    // ========================================================================
    // Batch Property Queries
    // ========================================================================

    /// Get thermal status in a single batch operation
    /// Returns (temperature, cooler_on, cooler_power, can_set_temperature)
    ///
    /// This is more efficient than calling each property individually when you
    /// need multiple thermal-related properties.
    pub fn get_thermal_status(&self) -> CameraThermalStatus {
        CameraThermalStatus {
            temperature: self.ccd_temperature().ok(),
            cooler_on: self.cooler_on().ok(),
            cooler_power: self.cooler_power().ok(),
            can_set_temperature: self.can_set_ccd_temperature().ok(),
        }
    }

    /// Get sensor configuration in a single batch operation
    /// Returns sensor dimensions, pixel sizes, and binning limits
    pub fn get_sensor_config(&self) -> CameraSensorConfig {
        CameraSensorConfig {
            width: self.camera_x_size().ok(),
            height: self.camera_y_size().ok(),
            pixel_size_x: self.pixel_size_x().ok(),
            pixel_size_y: self.pixel_size_y().ok(),
            max_bin_x: self.max_bin_x().ok(),
            max_bin_y: self.max_bin_y().ok(),
            sensor_type: self.sensor_type().ok(),
            bayer_offset_x: self.bayer_offset_x().ok(),
            bayer_offset_y: self.bayer_offset_y().ok(),
        }
    }

    /// Get current exposure settings in a single batch operation
    pub fn get_exposure_settings(&self) -> CameraExposureSettings {
        CameraExposureSettings {
            bin_x: self.bin_x().ok(),
            bin_y: self.bin_y().ok(),
            start_x: self.start_x().ok(),
            start_y: self.start_y().ok(),
            num_x: self.num_x().ok(),
            num_y: self.num_y().ok(),
            gain: self.gain().ok(),
            offset: self.offset().ok(),
        }
    }

    /// Get complete camera status in a single batch operation
    /// This is the most comprehensive status query
    pub fn get_full_status(&self) -> CameraFullStatus {
        CameraFullStatus {
            state: self.camera_state().ok(),
            image_ready: self.image_ready().ok(),
            thermal: self.get_thermal_status(),
            exposure_settings: self.get_exposure_settings(),
        }
    }

    /// Perform a heartbeat check to verify device is still responding
    pub fn heartbeat(&self) -> Result<(), String> {
        self.device.heartbeat()
    }

    /// Get connection health status
    pub fn get_health(&self) -> ConnectionHealth {
        self.device.get_health()
    }
}

/// Thermal status for camera
#[derive(Debug, Clone, Default)]
pub struct CameraThermalStatus {
    pub temperature: Option<f64>,
    pub cooler_on: Option<bool>,
    pub cooler_power: Option<f64>,
    pub can_set_temperature: Option<bool>,
}

/// Sensor configuration for camera
#[derive(Debug, Clone, Default)]
pub struct CameraSensorConfig {
    pub width: Option<i32>,
    pub height: Option<i32>,
    pub pixel_size_x: Option<f64>,
    pub pixel_size_y: Option<f64>,
    pub max_bin_x: Option<i32>,
    pub max_bin_y: Option<i32>,
    pub sensor_type: Option<i32>,
    pub bayer_offset_x: Option<i32>,
    pub bayer_offset_y: Option<i32>,
}

/// Current exposure settings for camera
#[derive(Debug, Clone, Default)]
pub struct CameraExposureSettings {
    pub bin_x: Option<i32>,
    pub bin_y: Option<i32>,
    pub start_x: Option<i32>,
    pub start_y: Option<i32>,
    pub num_x: Option<i32>,
    pub num_y: Option<i32>,
    pub gain: Option<i32>,
    pub offset: Option<i32>,
}

/// Full camera status
#[derive(Debug, Clone, Default)]
pub struct CameraFullStatus {
    pub state: Option<i32>,
    pub image_ready: Option<bool>,
    pub thermal: CameraThermalStatus,
    pub exposure_settings: CameraExposureSettings,
}

/// ASCOM Mount (Telescope)
pub struct AscomMount {
    device: AscomDeviceConnection,
}

impl AscomMount {
    pub fn new(prog_id: &str) -> Result<Self, String> {
        Ok(Self {
            device: AscomDeviceConnection::new(prog_id)?,
        })
    }
    
    pub fn connect(&mut self) -> Result<(), String> {
        self.device.connect()
    }
    
    pub fn disconnect(&mut self) -> Result<(), String> {
        self.device.disconnect()
    }
    
    pub fn name(&self) -> Result<String, String> {
        self.device.get_string_property("Name")
    }
    
    pub fn right_ascension(&self) -> Result<f64, String> {
        self.device.get_double_property("RightAscension")
    }
    
    pub fn declination(&self) -> Result<f64, String> {
        self.device.get_double_property("Declination")
    }
    
    pub fn altitude(&self) -> Result<f64, String> {
        self.device.get_double_property("Altitude")
    }
    
    pub fn azimuth(&self) -> Result<f64, String> {
        self.device.get_double_property("Azimuth")
    }
    
    pub fn side_of_pier(&self) -> Result<i32, String> {
        self.device.get_int_property("SideOfPier")
    }
    
    pub fn sidereal_time(&self) -> Result<f64, String> {
        self.device.get_double_property("SiderealTime")
    }
    
    pub fn tracking(&self) -> Result<bool, String> {
        self.device.get_bool_property("Tracking")
    }
    
    pub fn set_tracking(&mut self, tracking: bool) -> Result<(), String> {
        self.device.set_bool_property("Tracking", tracking)
    }
    
    pub fn slewing(&self) -> Result<bool, String> {
        self.device.get_bool_property("Slewing")
    }
    
    pub fn at_park(&self) -> Result<bool, String> {
        self.device.get_bool_property("AtPark")
    }
    
    pub fn at_home(&self) -> Result<bool, String> {
        self.device.get_bool_property("AtHome")
    }
    
    pub fn can_park(&self) -> Result<bool, String> {
        self.device.get_bool_property("CanPark")
    }
    
    pub fn can_unpark(&self) -> Result<bool, String> {
        self.device.get_bool_property("CanUnpark")
    }
    
    pub fn can_slew(&self) -> Result<bool, String> {
        self.device.get_bool_property("CanSlew")
    }
    
    pub fn can_slew_async(&self) -> Result<bool, String> {
        self.device.get_bool_property("CanSlewAsync")
    }
    
    pub fn can_sync(&self) -> Result<bool, String> {
        self.device.get_bool_property("CanSync")
    }
    
    pub fn park(&mut self) -> Result<(), String> {
        self.device.call_method("Park")
    }
    
    pub fn unpark(&mut self) -> Result<(), String> {
        self.device.call_method("Unpark")
    }
    
    pub fn abort_slew(&mut self) -> Result<(), String> {
        self.device.call_method("AbortSlew")
    }
    
    pub fn find_home(&mut self) -> Result<(), String> {
        self.device.call_method("FindHome")
    }
    
    pub fn slew_to_coordinates_async(&mut self, ra: f64, dec: f64) -> Result<(), String> {
        self.device.call_method_2_double("SlewToCoordinatesAsync", ra, dec)
    }
    
    pub fn slew_to_coordinates(&mut self, ra: f64, dec: f64) -> Result<(), String> {
        self.device.call_method_2_double("SlewToCoordinates", ra, dec)
    }
    
    pub fn sync_to_coordinates(&mut self, ra: f64, dec: f64) -> Result<(), String> {
        self.device.call_method_2_double("SyncToCoordinates", ra, dec)
    }
    
    pub fn slew_to_alt_az_async(&mut self, alt: f64, az: f64) -> Result<(), String> {
        self.device.call_method_2_double("SlewToAltAzAsync", az, alt)
    }

    pub fn can_pulse_guide(&self) -> Result<bool, String> {
        self.device.get_bool_property("CanPulseGuide")
    }

    pub fn is_pulse_guiding(&self) -> Result<bool, String> {
        self.device.get_bool_property("IsPulseGuiding")
    }

    pub fn pulse_guide(&mut self, direction: i32, duration_ms: u32) -> Result<(), String> {
        self.device.call_method_2_int("PulseGuide", direction, duration_ms as i32)
    }

    pub fn guide_rate_right_ascension(&self) -> Result<f64, String> {
        self.device.get_double_property("GuideRateRightAscension")
    }

    pub fn guide_rate_declination(&self) -> Result<f64, String> {
        self.device.get_double_property("GuideRateDeclination")
    }

    pub fn set_guide_rate_right_ascension(&mut self, rate: f64) -> Result<(), String> {
        self.device.set_double_property("GuideRateRightAscension", rate)
    }

    pub fn set_guide_rate_declination(&mut self, rate: f64) -> Result<(), String> {
        self.device.set_double_property("GuideRateDeclination", rate)
    }

    /// Get the current tracking rate (0=Sidereal, 1=Lunar, 2=Solar, 3=King)
    pub fn tracking_rate(&self) -> Result<i32, String> {
        self.device.get_int_property("TrackingRate")
    }

    /// Set the tracking rate (0=Sidereal, 1=Lunar, 2=Solar, 3=King)
    pub fn set_tracking_rate(&mut self, rate: i32) -> Result<(), String> {
        self.device.set_int_property("TrackingRate", rate)
    }

    /// Check if axis movement is supported (axis: 0=RA/Az, 1=Dec/Alt, 2=Tertiary)
    ///
    /// This properly calls the ASCOM CanMoveAxis(TelescopeAxes) method which returns
    /// a boolean indicating whether the specified axis can be moved.
    ///
    /// According to ASCOM standards:
    /// - Axis 0: Primary axis (RA for equatorial, Azimuth for alt-az)
    /// - Axis 1: Secondary axis (Dec for equatorial, Altitude for alt-az)
    /// - Axis 2: Tertiary axis (if present, e.g., rotator on some mounts)
    pub fn can_move_axis(&self, axis: i32) -> Result<bool, String> {
        // Validate axis parameter
        if axis < 0 || axis > 2 {
            return Err(format!(
                "Invalid axis {}: must be 0 (Primary), 1 (Secondary), or 2 (Tertiary)",
                axis
            ));
        }

        // Call the ASCOM CanMoveAxis method with the axis parameter
        // CanMoveAxis is a method that takes a TelescopeAxes enum and returns a Boolean
        self.device.call_method_1_int_return_bool("CanMoveAxis", axis)
    }

    /// Move an axis at the specified rate (degrees/second)
    /// axis: 0=RA/Azimuth (primary), 1=Dec/Altitude (secondary)
    /// rate: degrees per second (positive = N/E, negative = S/W), 0 to stop
    pub fn move_axis(&mut self, axis: i32, rate: f64) -> Result<(), String> {
        self.device.call_method_int_double("MoveAxis", axis, rate)
    }
}

/// ASCOM Focuser
pub struct AscomFocuser {
    device: AscomDeviceConnection,
}

impl AscomFocuser {
    pub fn new(prog_id: &str) -> Result<Self, String> {
        Ok(Self {
            device: AscomDeviceConnection::new(prog_id)?,
        })
    }
    
    pub fn connect(&mut self) -> Result<(), String> {
        self.device.connect()
    }
    
    pub fn disconnect(&mut self) -> Result<(), String> {
        self.device.disconnect()
    }
    
    pub fn name(&self) -> Result<String, String> {
        self.device.get_string_property("Name")
    }
    
    pub fn position(&self) -> Result<i32, String> {
        self.device.get_int_property("Position")
    }
    
    pub fn max_step(&self) -> Result<i32, String> {
        self.device.get_int_property("MaxStep")
    }
    
    pub fn max_increment(&self) -> Result<i32, String> {
        self.device.get_int_property("MaxIncrement")
    }
    
    pub fn step_size(&self) -> Result<f64, String> {
        self.device.get_double_property("StepSize")
    }
    
    pub fn is_moving(&self) -> Result<bool, String> {
        self.device.get_bool_property("IsMoving")
    }
    
    pub fn absolute(&self) -> Result<bool, String> {
        self.device.get_bool_property("Absolute")
    }
    
    pub fn temp_comp(&self) -> Result<bool, String> {
        self.device.get_bool_property("TempComp")
    }
    
    pub fn set_temp_comp(&mut self, value: bool) -> Result<(), String> {
        self.device.set_bool_property("TempComp", value)
    }
    
    pub fn temp_comp_available(&self) -> Result<bool, String> {
        self.device.get_bool_property("TempCompAvailable")
    }
    
    pub fn temperature(&self) -> Result<f64, String> {
        self.device.get_double_property("Temperature")
    }
    
    pub fn move_to(&mut self, position: i32) -> Result<(), String> {
        self.device.call_method_1_int("Move", position)
    }
    
    pub fn halt(&mut self) -> Result<(), String> {
        self.device.call_method("Halt")
    }
}

/// ASCOM Filter Wheel
pub struct AscomFilterWheel {
    device: AscomDeviceConnection,
}

impl AscomFilterWheel {
    pub fn new(prog_id: &str) -> Result<Self, String> {
        Ok(Self {
            device: AscomDeviceConnection::new(prog_id)?,
        })
    }
    
    pub fn connect(&mut self) -> Result<(), String> {
        self.device.connect()
    }
    
    pub fn disconnect(&mut self) -> Result<(), String> {
        self.device.disconnect()
    }
    
    pub fn name(&self) -> Result<String, String> {
        self.device.get_string_property("Name")
    }
    
    pub fn position(&self) -> Result<i32, String> {
        self.device.get_int_property("Position")
    }
    
    pub fn set_position(&mut self, position: i32) -> Result<(), String> {
        self.device.set_int_property("Position", position)
    }

    pub fn names(&self) -> Result<Vec<String>, String> {
        self.device.get_string_array_property("Names")
    }
}

/// ASCOM Rotator
pub struct AscomRotator {
    device: AscomDeviceConnection,
}

impl AscomRotator {
    pub fn new(prog_id: &str) -> Result<Self, String> {
        Ok(Self {
            device: AscomDeviceConnection::new(prog_id)?,
        })
    }
    
    pub fn connect(&mut self) -> Result<(), String> {
        self.device.connect()
    }
    
    pub fn disconnect(&mut self) -> Result<(), String> {
        self.device.disconnect()
    }
    
    pub fn position(&self) -> Result<f64, String> {
        self.device.get_double_property("Position")
    }
    
    pub fn mechanical_position(&self) -> Result<f64, String> {
        self.device.get_double_property("MechanicalPosition")
    }
    
    pub fn is_moving(&self) -> Result<bool, String> {
        self.device.get_bool_property("IsMoving")
    }
    
    pub fn move_to(&mut self, position: f64) -> Result<(), String> {
        self.device.call_method_1_double("Move", position)
    }
    
    pub fn move_absolute(&mut self, position: f64) -> Result<(), String> {
        self.device.call_method_1_double("MoveAbsolute", position)
    }
    
    pub fn halt(&mut self) -> Result<(), String> {
        self.device.call_method("Halt")
    }
}

/// ASCOM Dome
pub struct AscomDome {
    device: AscomDeviceConnection,
}

impl AscomDome {
    pub fn new(prog_id: &str) -> Result<Self, String> {
        Ok(Self {
            device: AscomDeviceConnection::new(prog_id)?,
        })
    }

    pub fn connect(&mut self) -> Result<(), String> {
        self.device.connect()
    }

    pub fn disconnect(&mut self) -> Result<(), String> {
        self.device.disconnect()
    }

    pub fn name(&self) -> Result<String, String> {
        self.device.get_string_property("Name")
    }

    /// Open the dome shutter
    pub fn open_shutter(&self) -> Result<(), String> {
        self.device.call_method("OpenShutter")
    }

    /// Close the dome shutter
    pub fn close_shutter(&self) -> Result<(), String> {
        self.device.call_method("CloseShutter")
    }

    /// Park the dome
    pub fn park(&self) -> Result<(), String> {
        self.device.call_method("Park")
    }

    /// Get shutter status (0=Open, 1=Closed, 2=Opening, 3=Closing, 4=Error)
    pub fn shutter_status(&self) -> Result<i32, String> {
        self.device.get_int_property("ShutterStatus")
    }

    /// Check if dome is at park position
    pub fn at_park(&self) -> Result<bool, String> {
        // AtPark returns a boolean
        let val: i32 = self.device.get_int_property("AtPark")?;
        Ok(val != 0)
    }

    /// Check if dome is slewing
    pub fn slewing(&self) -> Result<bool, String> {
        let val: i32 = self.device.get_int_property("Slewing")?;
        Ok(val != 0)
    }

    /// Get the dome azimuth in degrees
    pub fn azimuth(&self) -> Result<f64, String> {
        self.device.get_double_property("Azimuth")
    }

    /// Slew dome to the specified azimuth in degrees
    pub fn slew_to_azimuth(&self, azimuth: f64) -> Result<(), String> {
        self.device.call_method_1_double("SlewToAzimuth", azimuth)
    }
}

/// ASCOM Safety Monitor
pub struct AscomSafetyMonitor {
    device: AscomDeviceConnection,
}

impl AscomSafetyMonitor {
    pub fn new(prog_id: &str) -> Result<Self, String> {
        Ok(Self {
            device: AscomDeviceConnection::new(prog_id)?,
        })
    }
    
    pub fn connect(&mut self) -> Result<(), String> {
        self.device.connect()
    }
    
    pub fn disconnect(&mut self) -> Result<(), String> {
        self.device.disconnect()
    }
    
    pub fn name(&self) -> Result<String, String> {
        self.device.get_string_property("Name")
    }
    
    pub fn is_safe(&self) -> Result<bool, String> {
        self.device.get_bool_property("IsSafe")
    }
}

/// ASCOM Observing Conditions
pub struct AscomObservingConditions {
    device: AscomDeviceConnection,
}

impl AscomObservingConditions {
    pub fn new(prog_id: &str) -> Result<Self, String> {
        Ok(Self {
            device: AscomDeviceConnection::new(prog_id)?,
        })
    }
    
    pub fn connect(&mut self) -> Result<(), String> {
        self.device.connect()
    }
    
    pub fn disconnect(&mut self) -> Result<(), String> {
        self.device.disconnect()
    }
    
    pub fn name(&self) -> Result<String, String> {
        self.device.get_string_property("Name")
    }
    
    pub fn cloud_cover(&self) -> Result<f64, String> {
        self.device.get_double_property("CloudCover")
    }
    
    pub fn dew_point(&self) -> Result<f64, String> {
        self.device.get_double_property("DewPoint")
    }
    
    pub fn humidity(&self) -> Result<f64, String> {
        self.device.get_double_property("Humidity")
    }
    
    pub fn pressure(&self) -> Result<f64, String> {
        self.device.get_double_property("Pressure")
    }
    
    pub fn rain_rate(&self) -> Result<f64, String> {
        self.device.get_double_property("RainRate")
    }
    
    pub fn sky_brightness(&self) -> Result<f64, String> {
        self.device.get_double_property("SkyBrightness")
    }
    
    pub fn sky_quality(&self) -> Result<f64, String> {
        self.device.get_double_property("SkyQuality")
    }
    
    pub fn sky_temperature(&self) -> Result<f64, String> {
        self.device.get_double_property("SkyTemperature")
    }
    
    pub fn star_fwhm(&self) -> Result<f64, String> {
        self.device.get_double_property("StarFWHM")
    }
    
    pub fn temperature(&self) -> Result<f64, String> {
        self.device.get_double_property("Temperature")
    }
    
    pub fn wind_direction(&self) -> Result<f64, String> {
        self.device.get_double_property("WindDirection")
    }
    
    pub fn wind_gust(&self) -> Result<f64, String> {
        self.device.get_double_property("WindGust")
    }
    
    pub fn wind_speed(&self) -> Result<f64, String> {
        self.device.get_double_property("WindSpeed")
    }
}

/// ASCOM Switch
pub struct AscomSwitch {
    device: AscomDeviceConnection,
}

impl AscomSwitch {
    pub fn new(prog_id: &str) -> Result<Self, String> {
        Ok(Self {
            device: AscomDeviceConnection::new(prog_id)?,
        })
    }
    
    pub fn connect(&mut self) -> Result<(), String> {
        self.device.connect()
    }
    
    pub fn disconnect(&mut self) -> Result<(), String> {
        self.device.disconnect()
    }
    
    pub fn name(&self) -> Result<String, String> {
        self.device.get_string_property("Name")
    }
    
    pub fn max_switch(&self) -> Result<i32, String> {
        self.device.get_int_property("MaxSwitch")
    }
    
    pub fn get_switch(&self, id: i32) -> Result<bool, String> {
        unsafe {
            let dispid = self.device.get_dispid("GetSwitch")?;
            let mut args = [variant_i32(id)];
            
            let params = DISPPARAMS {
                rgvarg: args.as_mut_ptr(),
                rgdispidNamedArgs: ptr::null_mut(),
                cArgs: 1,
                cNamedArgs: 0,
            };
            
            let mut result = VARIANT::default();
            self.device.dispatch.Invoke(
                dispid,
                &GUID::zeroed(),
                0,
                DISPATCH_METHOD,
                &params,
                Some(&mut result),
                None,
                None,
            ).map_err(|e| format!("Failed to call GetSwitch: {}", e))?;
            
            variant_to_bool(&result).ok_or_else(|| "GetSwitch did not return a bool".to_string())
        }
    }
    
    pub fn set_switch(&mut self, id: i32, state: bool) -> Result<(), String> {
        unsafe {
            let dispid = self.device.get_dispid("SetSwitch")?;
            let mut args = [variant_bool(state), variant_i32(id)];
            
            let params = DISPPARAMS {
                rgvarg: args.as_mut_ptr(),
                rgdispidNamedArgs: ptr::null_mut(),
                cArgs: 2,
                cNamedArgs: 0,
            };
            
            self.device.dispatch.Invoke(
                dispid,
                &GUID::zeroed(),
                0,
                DISPATCH_METHOD,
                &params,
                None,
                None,
                None,
            ).map_err(|e| format!("Failed to call SetSwitch: {}", e))?;
            
            Ok(())
        }
    }
}

/// ASCOM Cover Calibrator
pub struct AscomCoverCalibrator {
    device: AscomDeviceConnection,
}

impl AscomCoverCalibrator {
    pub fn new(prog_id: &str) -> Result<Self, String> {
        Ok(Self {
            device: AscomDeviceConnection::new(prog_id)?,
        })
    }
    
    pub fn connect(&mut self) -> Result<(), String> {
        self.device.connect()
    }
    
    pub fn disconnect(&mut self) -> Result<(), String> {
        self.device.disconnect()
    }
    
    pub fn name(&self) -> Result<String, String> {
        self.device.get_string_property("Name")
    }
    
    pub fn cover_state(&self) -> Result<i32, String> {
        self.device.get_int_property("CoverState")
    }
    
    pub fn calibrator_state(&self) -> Result<i32, String> {
        self.device.get_int_property("CalibratorState")
    }
    
    pub fn brightness(&self) -> Result<i32, String> {
        self.device.get_int_property("Brightness")
    }
    
    pub fn set_brightness(&mut self, brightness: i32) -> Result<(), String> {
        self.device.set_int_property("Brightness", brightness)
    }
    
    pub fn max_brightness(&self) -> Result<i32, String> {
        self.device.get_int_property("MaxBrightness")
    }
    
    pub fn open_cover(&mut self) -> Result<(), String> {
        self.device.call_method("OpenCover")
    }
    
    pub fn close_cover(&mut self) -> Result<(), String> {
        self.device.call_method("CloseCover")
    }
    
    pub fn halt_cover(&mut self) -> Result<(), String> {
        self.device.call_method("HaltCover")
    }
    
    pub fn calibrator_on(&mut self, brightness: i32) -> Result<(), String> {
        unsafe {
            let dispid = self.device.get_dispid("CalibratorOn")?;
            let mut args = [variant_i32(brightness)];
            
            let params = DISPPARAMS {
                rgvarg: args.as_mut_ptr(),
                rgdispidNamedArgs: ptr::null_mut(),
                cArgs: 1,
                cNamedArgs: 0,
            };
            
            self.device.dispatch.Invoke(
                dispid,
                &GUID::zeroed(),
                0,
                DISPATCH_METHOD,
                &params,
                None,
                None,
                None,
            ).map_err(|e| format!("Failed to call CalibratorOn: {}", e))?;
            
            Ok(())
        }
    }
    
    pub fn calibrator_off(&mut self) -> Result<(), String> {
        self.device.call_method("CalibratorOff")
    }
}
