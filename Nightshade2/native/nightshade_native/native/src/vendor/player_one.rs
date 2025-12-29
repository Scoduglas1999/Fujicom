//! Player One Camera SDK Wrapper
//!
//! Provides native support for Player One cameras by wrapping the POA SDK.
//! Player One cameras feature low read noise and built-in anti-dew heaters.
//!
//! Thread Safety: The POA SDK is NOT thread-safe. All SDK operations are protected
//! by PLAYER_ONE_SDK_MUTEX to prevent concurrent access.
//!
//! ## Timeout Handling
//!
//! All SDK operations that can potentially hang (exposure polling, image download)
//! have configurable timeouts via `NativeTimeoutConfig`.

#![allow(dead_code)] // FFI types must match SDK headers even if not all variants are used

use crate::camera::*;
use crate::traits::*;
use crate::utils::{calculate_buffer_size_i32, safe_cstr_to_string, wait_for_exposure};
use crate::NativeVendor;
use async_trait::async_trait;
use std::ffi::{c_char, c_int, c_long, CStr};
use std::sync::OnceLock;

// =============================================================================
// POA SDK TYPE DEFINITIONS
// =============================================================================

/// POA Camera handle (index-based)
type PoaCameraIdx = c_int;

/// POA Camera Properties structure - matches actual SDK struct from PlayerOneCamera.h
#[repr(C)]
#[derive(Debug, Clone)]
struct POACameraProperties {
    camera_model_name: [c_char; 256],  // cameraModelName
    user_custom_id: [c_char; 16],       // userCustomID  
    camera_id: c_int,                    // cameraID
    max_width: c_int,                    // maxWidth (NOTE: width comes before height in SDK)
    max_height: c_int,                   // maxHeight
    bit_depth: c_int,                    // bitDepth
    is_color_camera: c_int,              // isColorCamera (POABool)
    is_has_st4_port: c_int,              // isHasST4Port (POABool)
    is_has_cooler: c_int,                // isHasCooler (POABool)
    is_usb3_speed: c_int,                // isUSB3Speed (POABool)
    bayer_pattern: c_int,                // bayerPattern (POABayerPattern)
    pixel_size: f64,                     // pixelSize (double)
    sn: [c_char; 64],                    // SN
    sensor_model_name: [c_char; 32],     // sensorModelName
    local_path: [c_char; 256],           // localPath
    bins: [c_int; 8],                    // bins - supported bin modes
    img_formats: [c_int; 8],             // imgFormats - supported image formats
    is_support_hard_bin: c_int,          // isSupportHardBin (POABool)
    p_id: c_int,                         // pID
    reserved: [c_char; 248],             // reserved
}

/// POA Exposure Status
#[repr(C)]
#[derive(Debug, Clone, Copy, PartialEq)]
enum POAExposureStatus {
    Idle = 0,
    Working = 1,
    Success = 2,
    Failed = 3,
}

/// POA Bool type
type POABool = c_int;
const POA_FALSE: POABool = 0;
const POA_TRUE: POABool = 1;

/// POA Error codes
#[repr(C)]
#[derive(Debug, Clone, Copy, PartialEq)]
#[allow(dead_code)]
enum POAErrors {
    Success = 0,
    InvalidIndex = 1,
    InvalidId = 2,
    InvalidConfig = 3,
    InvalidArg = 4,
    NotOpened = 5,
    DeviceNotFound = 6,
    OutOfLimit = 7,
    ExposureFailed = 8,
    Timeout = 9,
    SizeTooSmall = 10,
    NotSupported = 11,
    ConfigError = 12,
    Unknown = 13,
}

/// POA Config IDs (controls) - matches POAConfig enum from PlayerOneCamera.h
#[repr(C)]
#[derive(Debug, Clone, Copy)]
#[allow(non_camel_case_types)]
enum POAConfig {
    POA_EXPOSURE = 0,              // exposure time (us), VAL_INT
    POA_GAIN = 1,                  // gain, VAL_INT
    POA_HARDWARE_BIN = 2,          // hardware bin, VAL_BOOL
    POA_TEMPERATURE = 3,           // temperature (C), VAL_FLOAT, read-only
    POA_WB_R = 4,                  // white balance red, VAL_INT
    POA_WB_G = 5,                  // white balance green, VAL_INT
    POA_WB_B = 6,                  // white balance blue, VAL_INT
    POA_OFFSET = 7,                // offset, VAL_INT
    POA_AUTOEXPO_MAX_GAIN = 8,     // max gain for auto exposure, VAL_INT
    POA_AUTOEXPO_MAX_EXPOSURE = 9, // max exposure for auto (ms), VAL_INT
    POA_AUTOEXPO_BRIGHTNESS = 10,  // target brightness for auto, VAL_INT
    POA_GUIDE_NORTH = 11,          // ST4 guide north, VAL_BOOL
    POA_GUIDE_SOUTH = 12,          // ST4 guide south, VAL_BOOL
    POA_GUIDE_EAST = 13,           // ST4 guide east, VAL_BOOL
    POA_GUIDE_WEST = 14,           // ST4 guide west, VAL_BOOL
    POA_EGAIN = 15,                // e/ADU, VAL_FLOAT, read-only
    POA_COOLER_POWER = 16,         // cooler power %, VAL_INT, read-only
    POA_TARGET_TEMP = 17,          // target temperature (C), VAL_INT
    POA_COOLER = 18,               // cooler on/off, VAL_BOOL
    POA_HEATER = 19,               // lens heater state (deprecated), VAL_BOOL
    POA_HEATER_POWER = 20,         // lens heater power %, VAL_INT
    POA_FAN_POWER = 21,            // fan power %, VAL_INT
    POA_FLIP_NONE = 22,            // no flip, VAL_BOOL
    POA_FLIP_HORI = 23,            // horizontal flip, VAL_BOOL
    POA_FLIP_VERT = 24,            // vertical flip, VAL_BOOL
    POA_FLIP_BOTH = 25,            // both flip, VAL_BOOL
    POA_FRAME_LIMIT = 26,          // frame rate limit, VAL_INT
    POA_HQI = 27,                  // high quality image mode, VAL_BOOL
    POA_USB_BANDWIDTH_LIMIT = 28,  // USB bandwidth limit, VAL_INT
    POA_PIXEL_BIN_SUM = 29,        // pixel bin sum mode, VAL_BOOL
    POA_MONO_BIN = 30,             // mono bin mode, VAL_BOOL
}

/// POA Image Format
#[repr(C)]
#[derive(Debug, Clone, Copy)]
enum POAImgFormat {
    Raw8 = 0,
    Raw16 = 1,
    Rgb24 = 2,
    Mono8 = 3,
}

/// POA Bayer Pattern - matches POABayerPattern from PlayerOneCamera.h
#[repr(C)]
#[derive(Debug, Clone, Copy)]
enum POABayerPattern {
    Rg = 0,
    Bg = 1,
    Gr = 2,
    Gb = 3,
    Mono = -1,
}

/// POA Config Value union - used for get/set config values
#[repr(C)]
#[derive(Clone, Copy)]
union POAConfigValue {
    int_value: c_long,
    float_value: f64,
    bool_value: c_int,
}

impl Default for POAConfigValue {
    fn default() -> Self {
        Self { int_value: 0 }
    }
}

impl std::fmt::Debug for POAConfigValue {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        // Default to showing as int
        write!(f, "POAConfigValue({})", unsafe { self.int_value })
    }
}

// =============================================================================
// SDK LIBRARY LOADING
// =============================================================================

/// POA SDK library wrapper
struct PoaSdk {
    #[allow(dead_code)]
    lib: libloading::Library,
    
    // Function pointers - matches actual SDK signatures from PlayerOneCamera.h
    get_camera_count: unsafe extern "C" fn() -> c_int,
    get_camera_properties: unsafe extern "C" fn(c_int, *mut POACameraProperties) -> c_int,
    get_camera_properties_by_id: unsafe extern "C" fn(c_int, *mut POACameraProperties) -> c_int,
    open_camera: unsafe extern "C" fn(c_int) -> c_int,
    init_camera: unsafe extern "C" fn(c_int) -> c_int,
    close_camera: unsafe extern "C" fn(c_int) -> c_int,
    // POAGetConfig uses POAConfigValue union
    get_config: unsafe extern "C" fn(c_int, c_int, *mut POAConfigValue, *mut POABool) -> c_int,
    // POASetConfig uses POAConfigValue union
    set_config: unsafe extern "C" fn(c_int, c_int, POAConfigValue, POABool) -> c_int,
    set_image_bin: unsafe extern "C" fn(c_int, c_int) -> c_int,
    set_image_size: unsafe extern "C" fn(c_int, c_int, c_int) -> c_int,
    set_image_start_pos: unsafe extern "C" fn(c_int, c_int, c_int) -> c_int,
    set_image_format: unsafe extern "C" fn(c_int, c_int) -> c_int,
    start_exposure: unsafe extern "C" fn(c_int, POABool) -> c_int,
    stop_exposure: unsafe extern "C" fn(c_int) -> c_int,
    get_camera_state: unsafe extern "C" fn(c_int, *mut c_int) -> c_int,
    get_image_data: unsafe extern "C" fn(c_int, *mut u8, c_long, c_int) -> c_int,
    get_image_size: unsafe extern "C" fn(c_int, *mut c_int, *mut c_int) -> c_int,
    // Additional functions for readout modes
    image_ready: unsafe extern "C" fn(c_int, *mut POABool) -> c_int,
}

static POA_SDK: OnceLock<Option<PoaSdk>> = OnceLock::new();

impl PoaSdk {
    /// Load the POA SDK library
    fn load() -> Option<Self> {
        let lib_paths = if cfg!(target_os = "windows") {
            vec![
                "PlayerOneCamera.dll",
                "C:\\Program Files\\PlayerOne\\SDK\\lib\\x64\\PlayerOneCamera.dll",
            ]
        } else if cfg!(target_os = "macos") {
            vec![
                "libPlayerOneCamera.dylib",
                "/usr/local/lib/libPlayerOneCamera.dylib",
            ]
        } else {
            vec![
                "libPlayerOneCamera.so",
                "libPlayerOneCamera.so.1",
                "/usr/lib/libPlayerOneCamera.so",
                "/usr/local/lib/libPlayerOneCamera.so",
            ]
        };

        for path in lib_paths {
            unsafe {
                if let Ok(lib) = libloading::Library::new(path) {
                    tracing::info!("Loaded Player One SDK from: {}", path);
                    
                    // Load all function pointers - actual SDK function names
                    let sdk = Self {
                        get_camera_count: *lib.get(b"POAGetCameraCount\0").ok()?,
                        get_camera_properties: *lib.get(b"POAGetCameraProperties\0").ok()?,
                        get_camera_properties_by_id: *lib.get(b"POAGetCameraPropertiesByID\0").ok()?,
                        open_camera: *lib.get(b"POAOpenCamera\0").ok()?,
                        init_camera: *lib.get(b"POAInitCamera\0").ok()?,
                        close_camera: *lib.get(b"POACloseCamera\0").ok()?,
                        get_config: *lib.get(b"POAGetConfig\0").ok()?,
                        set_config: *lib.get(b"POASetConfig\0").ok()?,
                        set_image_bin: *lib.get(b"POASetImageBin\0").ok()?,
                        set_image_size: *lib.get(b"POASetImageSize\0").ok()?,
                        set_image_start_pos: *lib.get(b"POASetImageStartPos\0").ok()?,
                        set_image_format: *lib.get(b"POASetImageFormat\0").ok()?,
                        start_exposure: *lib.get(b"POAStartExposure\0").ok()?,
                        stop_exposure: *lib.get(b"POAStopExposure\0").ok()?,
                        get_camera_state: *lib.get(b"POAGetCameraState\0").ok()?,
                        get_image_data: *lib.get(b"POAGetImageData\0").ok()?,
                        get_image_size: *lib.get(b"POAGetImageSize\0").ok()?,
                        image_ready: *lib.get(b"POAImageReady\0").ok()?,
                        lib,
                    };
                    
                    return Some(sdk);
                }
            }
        }
        
        tracing::warn!("Player One SDK not found");
        None
    }
    
    /// Get the global SDK instance
    fn get() -> Option<&'static PoaSdk> {
        POA_SDK.get_or_init(|| Self::load()).as_ref()
    }
}

/// Check POA error and convert to NativeError with detailed error messages
fn check_poa_error(code: c_int, operation: &str) -> Result<(), NativeError> {
    match code {
        0 => Ok(()), // POA_OK
        1 => Err(NativeError::InvalidDevice(format!(
            "{}: Invalid camera index - camera may not exist",
            operation
        ))),
        2 => Err(NativeError::InvalidDevice(format!(
            "{}: Invalid camera ID - camera may have been disconnected",
            operation
        ))),
        3 => Err(NativeError::InvalidParameter(format!(
            "{}: Invalid config - control type not available",
            operation
        ))),
        4 => Err(NativeError::InvalidParameter(format!(
            "{}: Invalid argument - value out of range",
            operation
        ))),
        5 => Err(NativeError::NotConnected),
        6 => Err(NativeError::Disconnected),
        7 => Err(NativeError::InvalidParameter(format!(
            "{}: Value out of limit",
            operation
        ))),
        8 => Err(NativeError::SdkError(format!(
            "{}: Exposure failed - check camera connection",
            operation
        ))),
        9 => Err(NativeError::Timeout(format!(
            "{}: Operation timed out",
            operation
        ))),
        10 => Err(NativeError::InvalidParameter(format!(
            "{}: Buffer size too small",
            operation
        ))),
        11 => Err(NativeError::NotSupported),
        12 => Err(NativeError::SdkError(format!(
            "{}: Config error - camera may need reinitialization",
            operation
        ))),
        _ => Err(NativeError::SdkError(format!(
            "{}: Unknown POA error code {}",
            operation, code
        ))),
    }
}

// =============================================================================
// PLAYER ONE CAMERA IMPLEMENTATION
// =============================================================================

/// Player One Camera implementation
#[derive(Debug)]
pub struct PlayerOneCamera {
    camera_id: i32,
    camera_info: Option<POACameraProperties>,
    device_id: String,
    connected: bool,
    current_bin: i32,
    current_width: i32,
    current_height: i32,
    image_format: POAImgFormat,
    // Exposure metadata tracking
    exposure_time: f64,
    current_subframe: Option<SubFrame>,
}

impl PlayerOneCamera {
    /// Create a new Player One camera instance
    pub fn new(camera_id: i32) -> Self {
        Self {
            camera_id,
            camera_info: None,
            device_id: format!("native:playerone:{}", camera_id),
            connected: false,
            current_bin: 1,
            current_width: 0,
            current_height: 0,
            image_format: POAImgFormat::Raw16,
            exposure_time: 0.0,
            current_subframe: None,
        }
    }
    
    /// Load camera info from SDK
    fn load_camera_info(&mut self) -> Result<(), NativeError> {
        let sdk = PoaSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        
        let mut info: POACameraProperties = unsafe { std::mem::zeroed() };
        let result = unsafe { (sdk.get_camera_properties_by_id)(self.camera_id, &mut info) };
        check_poa_error(result, "GetCameraProperties")?;
        
        self.current_width = info.max_width;
        self.current_height = info.max_height;
        self.camera_info = Some(info);
        Ok(())
    }
    
    /// Get camera name using safe string conversion
    fn camera_name(&self) -> String {
        if let Some(info) = &self.camera_info {
            safe_cstr_to_string(info.camera_model_name.as_ptr(), 256)
        } else {
            format!("Player One Camera {}", self.camera_id)
        }
    }
    
    /// Get a control value as integer
    fn get_control_int(&self, control: POAConfig) -> Result<c_long, NativeError> {
        let sdk = PoaSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        let mut value = POAConfigValue::default();
        let mut is_auto: POABool = POA_FALSE;
        let result = unsafe { 
            (sdk.get_config)(self.camera_id, control as c_int, &mut value, &mut is_auto) 
        };
        check_poa_error(result, "POAGetConfig")?;
        Ok(unsafe { value.int_value })
    }
    
    /// Get a control value as float (for temperature)
    fn get_control_float(&self, control: POAConfig) -> Result<f64, NativeError> {
        let sdk = PoaSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        let mut value = POAConfigValue::default();
        let mut is_auto: POABool = POA_FALSE;
        let result = unsafe { 
            (sdk.get_config)(self.camera_id, control as c_int, &mut value, &mut is_auto) 
        };
        check_poa_error(result, "POAGetConfig")?;
        Ok(unsafe { value.float_value })
    }
    
    /// Set a control value (integer)
    fn set_control_int(&mut self, control: POAConfig, value: c_long, auto: bool) -> Result<(), NativeError> {
        let sdk = PoaSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        let config_value = POAConfigValue { int_value: value };
        let result = unsafe { 
            (sdk.set_config)(
                self.camera_id, 
                control as c_int, 
                config_value, 
                if auto { POA_TRUE } else { POA_FALSE }
            ) 
        };
        check_poa_error(result, "POASetConfig")
    }
    
    /// Set a control value (boolean)
    fn set_control_bool(&mut self, control: POAConfig, value: bool, auto: bool) -> Result<(), NativeError> {
        let sdk = PoaSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        let config_value = POAConfigValue { bool_value: if value { POA_TRUE } else { POA_FALSE } };
        let result = unsafe {
            (sdk.set_config)(
                self.camera_id,
                control as c_int,
                config_value,
                if auto { POA_TRUE } else { POA_FALSE }
            )
        };
        check_poa_error(result, "POASetConfig")
    }

    /// Wait for exposure to complete with timeout.
    ///
    /// Polls `is_exposure_complete()` until it returns true or the timeout is reached.
    /// Uses the timeout calculated from the exposure duration plus a margin.
    ///
    /// # Arguments
    /// * `config` - Timeout configuration
    ///
    /// # Returns
    /// * `Ok(())` - Exposure completed successfully
    /// * `Err(NativeError::ExposureTimeout)` - Exposure did not complete within timeout
    /// * `Err(NativeError::...)` - Other errors from polling
    pub async fn wait_for_exposure_complete(
        &self,
        config: &NativeTimeoutConfig,
    ) -> Result<(), NativeError> {
        wait_for_exposure(
            || async { self.is_exposure_complete().await },
            config,
            self.exposure_time,
        )
        .await
    }

    /// Download image with timeout protection.
    ///
    /// This wrapper uses `tokio::time::timeout()` to enforce a hard timeout on the
    /// image download operation. If the download takes longer than
    /// `config.image_download_timeout`, the operation is cancelled and an error is returned.
    ///
    /// # Arguments
    /// * `config` - Timeout configuration
    ///
    /// # Returns
    /// * `Ok(ImageData)` - Image downloaded successfully
    /// * `Err(NativeError::DownloadTimeout)` - Download timed out
    pub async fn download_image_with_timeout(
        &mut self,
        config: &NativeTimeoutConfig,
    ) -> Result<ImageData, NativeError> {
        let timeout_duration = config.image_download_timeout;

        match tokio::time::timeout(timeout_duration, self.download_image()).await {
            Ok(result) => result,
            Err(_elapsed) => {
                tracing::error!(
                    "Player One image download timed out after {:?}",
                    timeout_duration
                );
                Err(NativeError::download_timeout(
                    timeout_duration,
                    self.current_width as u32,
                    self.current_height as u32,
                ))
            }
        }
    }
}

#[async_trait]
impl NativeDevice for PlayerOneCamera {
    fn id(&self) -> &str {
        &self.device_id
    }
    
    fn name(&self) -> &str {
        &self.device_id
    }
    
    fn vendor(&self) -> NativeVendor {
        NativeVendor::PlayerOne
    }
    
    fn is_connected(&self) -> bool {
        self.connected
    }
    
    async fn connect(&mut self) -> Result<(), NativeError> {
        let sdk = PoaSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        
        // Load camera info first
        self.load_camera_info()?;
        
        // Open camera
        let result = unsafe { (sdk.open_camera)(self.camera_id) };
        check_poa_error(result, "OpenCamera")?;
        
        // Initialize camera
        let result = unsafe { (sdk.init_camera)(self.camera_id) };
        if result != 0 {
            unsafe { (sdk.close_camera)(self.camera_id) };
            return Err(check_poa_error(result, "InitCamera").unwrap_err());
        }
        
        // Set default format (Raw16)
        let result = unsafe { (sdk.set_image_format)(self.camera_id, POAImgFormat::Raw16 as c_int) };
        check_poa_error(result, "SetImageFormat")?;
        
        // Set default binning and ROI
        if let Some(info) = &self.camera_info {
            let _ = unsafe { (sdk.set_image_bin)(self.camera_id, 1) };
            let _ = unsafe { (sdk.set_image_start_pos)(self.camera_id, 0, 0) };
            let _ = unsafe { (sdk.set_image_size)(self.camera_id, info.max_width, info.max_height) };
        }
        
        self.connected = true;
        tracing::info!("Connected to {}", self.camera_name());
        Ok(())
    }
    
    async fn disconnect(&mut self) -> Result<(), NativeError> {
        if self.connected {
            let sdk = PoaSdk::get().ok_or(NativeError::SdkNotLoaded)?;
            let result = unsafe { (sdk.close_camera)(self.camera_id) };
            check_poa_error(result, "CloseCamera")?;
            self.connected = false;
            tracing::info!("Disconnected from {}", self.camera_name());
        }
        Ok(())
    }
}

#[async_trait]
impl NativeCamera for PlayerOneCamera {
    fn capabilities(&self) -> CameraCapabilities {
        if let Some(info) = &self.camera_info {
            CameraCapabilities {
                can_cool: info.is_has_cooler != 0,
                can_set_gain: true,
                can_set_offset: true,
                can_set_binning: true,
                can_subframe: true,
                has_shutter: false,
                has_guider_port: info.is_has_st4_port != 0,
                max_bin_x: 4,
                max_bin_y: 4,
                supports_readout_modes: false, // Player One doesn't have readout modes
            }
        } else {
            CameraCapabilities::default()
        }
    }
    
    async fn get_status(&self) -> Result<CameraStatus, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }
        
        let sdk = PoaSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        
        let mut camera_state: c_int = 0;
        let result = unsafe { (sdk.get_camera_state)(self.camera_id, &mut camera_state) };
        check_poa_error(result, "GetCameraState")?;
        
        let state = match camera_state {
            0 => CameraState::Idle,
            1 => CameraState::Exposing,
            2 => CameraState::Downloading,
            _ => CameraState::Error,
        };
        
        // Get temperature (POA_TEMPERATURE is a float value, unit C)
        let temp = self.get_control_float(POAConfig::POA_TEMPERATURE)
            .unwrap_or(0.0);
        
        let cooler_power = if self.camera_info.as_ref().map(|i| i.is_has_cooler != 0).unwrap_or(false) {
            self.get_control_int(POAConfig::POA_COOLER_POWER).ok().map(|v| v as f64)
        } else {
            None
        };
        
        Ok(CameraStatus {
            state,
            sensor_temp: Some(temp),
            target_temp: None, // Need to get target temp from POA_TARGET_TEMP if needed
            cooler_on: false, // Need to check POA_COOLER status if needed
            cooler_power,
            gain: self.get_control_int(POAConfig::POA_GAIN).unwrap_or(0) as i32,
            offset: self.get_control_int(POAConfig::POA_OFFSET).unwrap_or(0) as i32,
            bin_x: self.current_bin,
            bin_y: self.current_bin,
            exposure_remaining: None, // Not directly available from POA SDK
        })
    }
    
    async fn start_exposure(&mut self, params: ExposureParams) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }
        
        let sdk = PoaSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        
        // Set exposure time (in microseconds)
        let exposure_us = (params.duration_secs * 1_000_000.0) as c_long;
        self.set_control_int(POAConfig::POA_EXPOSURE, exposure_us, false)?;
        
        // Set gain
        if let Some(gain) = params.gain {
            self.set_control_int(POAConfig::POA_GAIN, gain as c_long, false)?;
        }
        
        // Set offset if provided
        if let Some(offset) = params.offset {
            self.set_control_int(POAConfig::POA_OFFSET, offset as c_long, false)?;
        }
        
        // Start exposure (false = not snap mode, single frame)
        let result = unsafe { (sdk.start_exposure)(self.camera_id, POA_FALSE) };
        check_poa_error(result, "StartExposure")?;

        // Track exposure time for metadata
        self.exposure_time = params.duration_secs;

        tracing::info!("Started {}s exposure", params.duration_secs);
        Ok(())
    }
    
    async fn abort_exposure(&mut self) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }
        
        let sdk = PoaSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        let result = unsafe { (sdk.stop_exposure)(self.camera_id) };
        check_poa_error(result, "StopExposure")?;
        
        tracing::info!("Aborted exposure");
        Ok(())
    }
    
    async fn is_exposure_complete(&self) -> Result<bool, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }
        
        let sdk = PoaSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        
        // Use POAImageReady to check if image data is available
        let mut is_ready: POABool = POA_FALSE;
        let result = unsafe { (sdk.image_ready)(self.camera_id, &mut is_ready) };
        check_poa_error(result, "POAImageReady")?;
        
        Ok(is_ready == POA_TRUE)
    }
    
    async fn download_image(&mut self) -> Result<ImageData, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }
        
        let sdk = PoaSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        
        // Get current image dimensions
        let mut width: c_int = 0;
        let mut height: c_int = 0;
        let result = unsafe { (sdk.get_image_size)(self.camera_id, &mut width, &mut height) };
        check_poa_error(result, "GetImageSize")?;
        
        // Calculate buffer size (Raw16 = 2 bytes per pixel) with overflow protection
        let bytes_per_pixel = if matches!(self.image_format, POAImgFormat::Raw16) { 2 } else { 1 };
        let buffer_size = calculate_buffer_size_i32(width, height, bytes_per_pixel)?;
        
        let mut buffer: Vec<u8> = vec![0u8; buffer_size];
        
        // Get image data with 30 second timeout
        let result = unsafe {
            (sdk.get_image_data)(self.camera_id, buffer.as_mut_ptr(), buffer_size as c_long, 30000)
        };
        check_poa_error(result, "GetImageData")?;
        
        // Convert to u16
        let data: Vec<u16> = if bytes_per_pixel == 2 {
            buffer
                .chunks_exact(2)
                .map(|chunk| u16::from_ne_bytes([chunk[0], chunk[1]]))
                .collect()
        } else {
            // 8-bit to 16-bit scaling
            buffer.iter().map(|&x| (x as u16) * 256).collect()
        };

        tracing::info!("Downloaded {}x{} image ({} bytes)", width, height, buffer_size);
        
        Ok(ImageData {
            width: width as u32,
            height: height as u32,
            data,
            bits_per_pixel: if bytes_per_pixel == 2 { 16 } else { 8 },
            bayer_pattern: self.camera_info.as_ref()
                .filter(|i| i.is_color_camera != 0)
                .map(|i| match i.bayer_pattern {
                    0 => BayerPattern::Rggb,
                    1 => BayerPattern::Bggr,
                    2 => BayerPattern::Grbg,
                    3 => BayerPattern::Gbrg,
                    _ => BayerPattern::Rggb,
                }),
            metadata: ImageMetadata {
                exposure_time: self.exposure_time,
                gain: self.get_control_int(POAConfig::POA_GAIN).unwrap_or(0) as i32,
                offset: self.get_control_int(POAConfig::POA_OFFSET).unwrap_or(0) as i32,
                bin_x: self.current_bin,
                bin_y: self.current_bin,
                temperature: self.get_temperature().await.ok(),
                timestamp: chrono::Utc::now(),
                subframe: self.current_subframe.clone(),
                readout_mode: None, // Player One doesn't support readout modes
                vendor_data: self.get_vendor_features().await?,
            },
        })
    }
    
    async fn set_cooler(&mut self, enabled: bool, target_temp: f64) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }
        
        if !self.camera_info.as_ref().map(|i| i.is_has_cooler != 0).unwrap_or(false) {
            return Err(NativeError::NotSupported);
        }
        
        // Set target temperature (POA_TARGET_TEMP is in C, int value)
        self.set_control_int(POAConfig::POA_TARGET_TEMP, target_temp as c_long, false)?;
        
        // Enable/disable cooler (POA_COOLER is a bool)
        self.set_control_bool(POAConfig::POA_COOLER, enabled, false)?;
        
        Ok(())
    }
    
    async fn get_temperature(&self) -> Result<f64, NativeError> {
        // POA_TEMPERATURE is a float value in Celsius
        self.get_control_float(POAConfig::POA_TEMPERATURE)
    }
    
    async fn get_cooler_power(&self) -> Result<f64, NativeError> {
        if !self.camera_info.as_ref().map(|i| i.is_has_cooler != 0).unwrap_or(false) {
            return Err(NativeError::NotSupported);
        }
        let value = self.get_control_int(POAConfig::POA_COOLER_POWER)?;
        Ok(value as f64)
    }
    
    async fn set_gain(&mut self, gain: i32) -> Result<(), NativeError> {
        self.set_control_int(POAConfig::POA_GAIN, gain as c_long, false)
    }
    
    async fn get_gain(&self) -> Result<i32, NativeError> {
        self.get_control_int(POAConfig::POA_GAIN).map(|v| v as i32)
    }
    
    async fn set_offset(&mut self, offset: i32) -> Result<(), NativeError> {
        self.set_control_int(POAConfig::POA_OFFSET, offset as c_long, false)
    }
    
    async fn get_offset(&self) -> Result<i32, NativeError> {
        self.get_control_int(POAConfig::POA_OFFSET).map(|v| v as i32)
    }
    
    async fn set_binning(&mut self, bin_x: i32, bin_y: i32) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }
        
        let sdk = PoaSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        
        // Player One only supports symmetric binning
        let bin = bin_x.max(bin_y);
        
        let result = unsafe { (sdk.set_image_bin)(self.camera_id, bin as c_int) };
        check_poa_error(result, "SetImageBin")?;
        
        // Update dimensions
        let info = self.camera_info.as_ref().ok_or(NativeError::NotConnected)?;
        let new_width = info.max_width / bin;
        let new_height = info.max_height / bin;
        
        let result = unsafe { (sdk.set_image_size)(self.camera_id, new_width, new_height) };
        check_poa_error(result, "SetImageSize")?;
        
        self.current_bin = bin;
        self.current_width = new_width;
        self.current_height = new_height;
        
        Ok(())
    }
    
    async fn get_binning(&self) -> Result<(i32, i32), NativeError> {
        Ok((self.current_bin, self.current_bin))
    }
    
    async fn set_subframe(&mut self, subframe: Option<SubFrame>) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }
        
        let sdk = PoaSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        let info = self.camera_info.as_ref().ok_or(NativeError::NotConnected)?;
        
        let (x, y, width, height) = if let Some(ref sf) = subframe {
            (sf.start_x as c_int, sf.start_y as c_int, sf.width as c_int, sf.height as c_int)
        } else {
            (0, 0, info.max_width / self.current_bin, info.max_height / self.current_bin)
        };
        
        let result = unsafe { (sdk.set_image_start_pos)(self.camera_id, x, y) };
        check_poa_error(result, "SetImageStartPos")?;
        
        let result = unsafe { (sdk.set_image_size)(self.camera_id, width, height) };
        check_poa_error(result, "SetImageSize")?;

        self.current_width = width;
        self.current_height = height;
        // Track subframe for metadata
        self.current_subframe = subframe;

        Ok(())
    }
    
    fn get_sensor_info(&self) -> SensorInfo {
        if let Some(info) = &self.camera_info {
            SensorInfo {
                width: info.max_width as u32,
                height: info.max_height as u32,
                pixel_size_x: info.pixel_size,
                pixel_size_y: info.pixel_size,
                max_adu: (1u32 << info.bit_depth) - 1,
                bit_depth: info.bit_depth as u32,
                color: info.is_color_camera != 0,
                bayer_pattern: if info.is_color_camera != 0 {
                    Some(match info.bayer_pattern {
                        0 => BayerPattern::Rggb,
                        1 => BayerPattern::Bggr,
                        2 => BayerPattern::Grbg,
                        3 => BayerPattern::Gbrg,
                        _ => BayerPattern::Rggb,
                    })
                } else {
                    None
                },
            }
        } else {
            SensorInfo::default()
        }
    }
    
    async fn get_readout_modes(&self) -> Result<Vec<ReadoutMode>, NativeError> {
        // Player One doesn't have readout modes
        Ok(Vec::new())
    }
    
    async fn set_readout_mode(&mut self, _mode: &ReadoutMode) -> Result<(), NativeError> {
        Err(NativeError::NotSupported)
    }
    
    async fn get_vendor_features(&self) -> Result<VendorFeatures, NativeError> {
        let mut features = VendorFeatures::default();

        // Get USB bandwidth
        if let Ok(bw) = self.get_control_int(POAConfig::POA_USB_BANDWIDTH_LIMIT) {
            features.usb_bandwidth = Some(bw as f64);
        }

        // Player One specific: Anti-dew heater power
        if let Ok(heater_power) = self.get_control_int(POAConfig::POA_HEATER_POWER) {
            features.anti_dew_heater = Some(heater_power > 0);
        }

        // Player One specific: Fan power
        if let Ok(fan_power) = self.get_control_int(POAConfig::POA_FAN_POWER) {
            features.fan_power = Some(fan_power as f64);
        }

        Ok(features)
    }

    async fn get_gain_range(&self) -> Result<(i32, i32), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        // Player One SDK doesn't expose control min/max through a dedicated function.
        // The range depends on the camera model. Most Player One cameras support:
        // - Gain: 0 to 500 (or higher for some models)
        // - This is a conservative range that works for most cameras.
        // Note: The actual max gain varies by model (e.g., Mars-C/M: 510, Neptune-C: 500)
        // If the user sets a value outside the range, the SDK will return an error.
        Ok((0, 500))
    }

    async fn get_offset_range(&self) -> Result<(i32, i32), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        // Player One SDK doesn't expose control min/max through a dedicated function.
        // Most Player One cameras support offset in the range 0-100.
        // Some models may support higher values; the SDK will return an error if exceeded.
        Ok((0, 100))
    }
}

// =============================================================================
// PLAYER ONE CAMERA DISCOVERY
// =============================================================================

/// Player One camera discovery info
pub struct PlayerOneCameraInfo {
    pub camera_id: i32,
    pub name: String,
    /// Serial number from POACameraProperties.sn
    pub serial_number: Option<String>,
    /// User custom ID (if set)
    pub user_custom_id: Option<String>,
}

/// Check if Player One SDK is available
pub fn is_sdk_available() -> bool {
    PoaSdk::get().is_some()
}

/// Discover Player One cameras
pub async fn discover_devices() -> Result<Vec<PlayerOneCameraInfo>, NativeError> {
    let sdk = match PoaSdk::get() {
        Some(sdk) => sdk,
        None => return Ok(Vec::new()), // SDK not available, return empty
    };

    let num_cameras = unsafe { (sdk.get_camera_count)() };

    let mut cameras = Vec::new();
    for i in 0..num_cameras {
        let mut info: POACameraProperties = unsafe { std::mem::zeroed() };
        let result = unsafe { (sdk.get_camera_properties)(i, &mut info) };

        if result == 0 {
            let name = unsafe {
                CStr::from_ptr(info.camera_model_name.as_ptr())
                    .to_string_lossy()
                    .to_string()
            };

            // Extract serial number
            let serial_number = unsafe {
                let sn = CStr::from_ptr(info.sn.as_ptr())
                    .to_string_lossy()
                    .to_string();
                if sn.is_empty() { None } else { Some(sn) }
            };

            // Extract user custom ID (if set by user)
            let user_custom_id = unsafe {
                let custom_id = CStr::from_ptr(info.user_custom_id.as_ptr())
                    .to_string_lossy()
                    .to_string();
                if custom_id.is_empty() { None } else { Some(custom_id) }
            };

            cameras.push(PlayerOneCameraInfo {
                camera_id: info.camera_id,
                name,
                serial_number,
                user_custom_id,
            });
        }
    }

    Ok(cameras)
}