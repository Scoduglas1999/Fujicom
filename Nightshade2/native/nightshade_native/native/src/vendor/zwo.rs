//! ZWO ASI Camera SDK Wrapper
//!
//! Provides native support for ZWO ASI cameras by wrapping the ASI SDK.
//! The SDK is typically provided as a DLL (Windows) or shared library (macOS/Linux).
//!
//! Thread Safety: The ASI SDK is NOT thread-safe. All SDK operations are protected
//! by ZWO_SDK_MUTEX to prevent concurrent access.
//!
//! ## Timeout Handling
//!
//! All SDK operations that can potentially hang (exposure polling, image download,
//! focuser moves, filter wheel moves) have configurable timeouts via `NativeTimeoutConfig`.
//! Use the helper methods like `wait_for_exposure_complete`, `move_focuser_with_timeout`,
//! and `move_filterwheel_with_timeout` to ensure operations don't block indefinitely.

#![allow(dead_code)] // FFI types must match SDK headers even if not all variants are used

use crate::camera::*;
use crate::traits::*;
use crate::utils::{
    calculate_buffer_size_i32, safe_cstr_to_string, CleanupGuard,
    wait_for_exposure, wait_for_filterwheel_move, wait_for_focuser_move,
};
use crate::NativeVendor;
use async_trait::async_trait;
use std::ffi::{c_char, c_int, c_long, c_uchar, CStr};
use std::sync::OnceLock;

// =============================================================================
// ASI SDK TYPE DEFINITIONS
// =============================================================================

/// ASI Camera Info structure from SDK - matches ASI_CAMERA_INFO from ASICamera2.h
#[repr(C)]
#[derive(Debug, Clone)]
struct ASICameraInfo {
    name: [c_char; 64],           // Name[64] - camera name
    camera_id: c_int,              // CameraID - unique camera ID
    max_height: c_long,            // MaxHeight - max height
    max_width: c_long,             // MaxWidth - max width
    is_color_cam: c_int,           // IsColorCam (ASI_BOOL)
    bayer_pattern: c_int,          // BayerPattern (ASI_BAYER_PATTERN)
    supported_bins: [c_int; 16],   // SupportedBins[16] - ends with 0
    supported_video_format: [c_int; 8], // SupportedVideoFormat[8] - ends with ASI_IMG_END
    pixel_size: f64,               // PixelSize (double) - in um
    mechanical_shutter: c_int,     // MechanicalShutter (ASI_BOOL)
    st4_port: c_int,               // ST4Port (ASI_BOOL)
    is_cooler_cam: c_int,          // IsCoolerCam (ASI_BOOL)
    is_usb3_host: c_int,           // IsUSB3Host (ASI_BOOL)
    is_usb3_camera: c_int,         // IsUSB3Camera (ASI_BOOL)
    elec_per_adu: f32,             // ElecPerADU (float)
    bit_depth: c_int,              // BitDepth (int)
    is_trigger_cam: c_int,         // IsTriggerCam (ASI_BOOL)
    unused: [c_char; 16],          // Unused[16] - padding
}

/// ASI Exposure Status
#[repr(C)]
#[derive(Debug, Clone, Copy, PartialEq)]
enum ASIExposureStatus {
    Idle = 0,
    Working = 1,
    Success = 2,
    Failed = 3,
}

/// ASI Bool type
type ASIBool = c_int;
const ASI_FALSE: ASIBool = 0;
const ASI_TRUE: ASIBool = 1;

/// ASI Error codes - matches ASI_ERROR_CODE from ASICamera2.h
#[repr(C)]
#[derive(Debug, Clone, Copy, PartialEq)]
#[allow(non_camel_case_types, dead_code)]
enum ASIError {
    ASI_SUCCESS = 0,
    ASI_ERROR_INVALID_INDEX = 1,      // no camera connected or index value out of boundary
    ASI_ERROR_INVALID_ID = 2,         // invalid ID
    ASI_ERROR_INVALID_CONTROL_TYPE = 3, // invalid control type
    ASI_ERROR_CAMERA_CLOSED = 4,      // camera didn't open
    ASI_ERROR_CAMERA_REMOVED = 5,     // failed to find the camera, maybe removed
    ASI_ERROR_INVALID_PATH = 6,       // cannot find the path of the file
    ASI_ERROR_INVALID_FILEFORMAT = 7,
    ASI_ERROR_INVALID_SIZE = 8,       // wrong video format size
    ASI_ERROR_INVALID_IMGTYPE = 9,    // unsupported image format
    ASI_ERROR_OUTOF_BOUNDARY = 10,    // the startpos is out of boundary
    ASI_ERROR_TIMEOUT = 11,           // timeout
    ASI_ERROR_INVALID_SEQUENCE = 12,  // stop capture first
    ASI_ERROR_BUFFER_TOO_SMALL = 13,  // buffer size is not big enough
    ASI_ERROR_VIDEO_MODE_ACTIVE = 14,
    ASI_ERROR_EXPOSURE_IN_PROGRESS = 15,
    ASI_ERROR_GENERAL_ERROR = 16,     // general error, eg: value is out of valid range
    ASI_ERROR_INVALID_MODE = 17,      // the current mode is wrong
    ASI_ERROR_GPS_NOT_SUPPORTED = 18, // camera does not support GPS
    ASI_ERROR_GPS_VER_ERR = 19,       // FPGA GPS ver is too low
    ASI_ERROR_GPS_FPGA_ERR = 20,      // failed to read or write data to FPGA
    ASI_ERROR_GPS_PARAM_OUT_OF_RANGE = 21, // start line or end line out of range
    ASI_ERROR_GPS_DATA_INVALID = 22,  // GPS has not yet found satellite
    ASI_ERROR_END = 23,
}

/// ASI Control types - matches ASI_CONTROL_TYPE enum from ASICamera2.h
#[repr(C)]
#[derive(Debug, Clone, Copy)]
#[allow(non_camel_case_types)]
enum ASIControlType {
    ASI_GAIN = 0,
    ASI_EXPOSURE = 1,
    ASI_GAMMA = 2,
    ASI_WB_R = 3,
    ASI_WB_B = 4,
    ASI_OFFSET = 5,
    ASI_BANDWIDTHOVERLOAD = 6,
    ASI_OVERCLOCK = 7,
    ASI_TEMPERATURE = 8,        // returns 10*temperature
    ASI_FLIP = 9,
    ASI_AUTO_MAX_GAIN = 10,
    ASI_AUTO_MAX_EXP = 11,      // micro second
    ASI_AUTO_TARGET_BRIGHTNESS = 12,
    ASI_HARDWARE_BIN = 13,
    ASI_HIGH_SPEED_MODE = 14,
    ASI_COOLER_POWER_PERC = 15,
    ASI_TARGET_TEMP = 16,       // NOT multiplied by 10 (direct degrees C)
    ASI_COOLER_ON = 17,
    ASI_MONO_BIN = 18,          // reduces grid at software bin for color camera
    ASI_FAN_ON = 19,
    ASI_PATTERN_ADJUST = 20,
    ASI_ANTI_DEW_HEATER = 21,
    ASI_FAN_ADJUST = 22,
    ASI_PWRLED_BRIGNT = 23,
    ASI_USBHUB_RESET = 24,
    ASI_GPS_SUPPORT = 25,
    ASI_GPS_START_LINE = 26,
    ASI_GPS_END_LINE = 27,
    ASI_ROLLING_INTERVAL = 28,  // microsecond
}

/// ASI Image type
#[repr(C)]
#[derive(Debug, Clone, Copy)]
enum ASIImgType {
    Raw8 = 0,
    Rgb24 = 1,
    Raw16 = 2,
    Y8 = 3,
    End = -1,
}

/// ASI Flip Status
#[repr(C)]
#[derive(Debug, Clone, Copy, PartialEq)]
enum ASIFlipStatus {
    None = 0,
    Horiz = 1,
    Vert = 2,
    Both = 3,
}

/// ASI Control Capabilities
#[repr(C)]
#[derive(Debug, Clone)]
struct ASIControlCaps {
    name: [c_char; 64],
    description: [c_char; 128],
    max_value: c_long,
    min_value: c_long,
    default_value: c_long,
    is_auto_supported: ASIBool,
    is_writable: ASIBool,
    control_type: ASIControlType,
    unused: [c_char; 32],
}

/// ASI Bayer pattern
#[repr(C)]
#[derive(Debug, Clone, Copy)]
enum ASIBayerPattern {
    Rg = 0,
    Bg = 1,
    Gr = 2,
    Gb = 3,
}

// =============================================================================
// SDK LIBRARY LOADING
// =============================================================================

/// ASI SDK library wrapper
struct AsiSdk {
    #[allow(dead_code)]
    lib: libloading::Library,

    // Function pointers
    get_num_cameras: unsafe extern "C" fn() -> c_int,
    // ASIGetCameraProperty(ASI_CAMERA_INFO *pASICameraInfo, int iCameraIndex)
    get_camera_property: unsafe extern "C" fn(*mut ASICameraInfo, c_int) -> c_int,
    open_camera: unsafe extern "C" fn(c_int) -> c_int,
    init_camera: unsafe extern "C" fn(c_int) -> c_int,
    close_camera: unsafe extern "C" fn(c_int) -> c_int,
    get_control_value: unsafe extern "C" fn(c_int, c_int, *mut c_long, *mut ASIBool) -> c_int,
    set_control_value: unsafe extern "C" fn(c_int, c_int, c_long, ASIBool) -> c_int,
    set_roi_format: unsafe extern "C" fn(c_int, c_int, c_int, c_int, c_int) -> c_int,
    set_start_pos: unsafe extern "C" fn(c_int, c_int, c_int) -> c_int,
    get_roi_format: unsafe extern "C" fn(c_int, *mut c_int, *mut c_int, *mut c_int, *mut c_int) -> c_int,
    start_exposure: unsafe extern "C" fn(c_int, ASIBool) -> c_int,
    stop_exposure: unsafe extern "C" fn(c_int) -> c_int,
    get_exp_status: unsafe extern "C" fn(c_int, *mut c_int) -> c_int,
    get_data_after_exp: unsafe extern "C" fn(c_int, *mut c_uchar, c_long) -> c_int,
    get_num_controls: unsafe extern "C" fn(c_int, *mut c_int) -> c_int,
    get_control_caps: unsafe extern "C" fn(c_int, c_int, *mut ASIControlCaps) -> c_int,
}

static ASI_SDK: OnceLock<Option<AsiSdk>> = OnceLock::new();

impl AsiSdk {
    /// Load the ASI SDK library
    fn load() -> Option<Self> {
        // Build list of paths to search, starting with most likely locations
        let mut lib_paths: Vec<String> = Vec::new();
        
        if cfg!(target_os = "windows") {
            // Try current directory first (works if DLL is in same folder as executable)
            lib_paths.push("ASICamera2.dll".to_string());
            
            // Standard installation paths
            lib_paths.push("C:\\Program Files\\ZWO\\ASI SDK\\lib\\x64\\ASICamera2.dll".to_string());
            lib_paths.push("C:\\Program Files (x86)\\ZWO\\ASI SDK\\lib\\x64\\ASICamera2.dll".to_string());
            // User workspace path
            lib_paths.push("C:\\Users\\scdou\\Documents\\Nightshade2\\SDKs\\ZWO\\ASI_Camera_SDK\\ASI_Windows_SDK_V1.40\\ASI SDK\\lib\\x64\\ASICamera2.dll".to_string());
            
            // Get executable directory and try paths relative to it
            if let Ok(exe_path) = std::env::current_exe() {
                if let Some(exe_dir) = exe_path.parent() {
                    // Check next to executable
                    lib_paths.push(exe_dir.join("ASICamera2.dll").to_string_lossy().to_string());
                    
                    // Check parent directories (for release builds in subdirectories)
                    if let Some(parent) = exe_dir.parent() {
                        lib_paths.push(parent.join("ASICamera2.dll").to_string_lossy().to_string());
                        
                        // Try SDKs directory if project structure exists
                        let sdk_path = parent.join("SDKs").join("ZWO").join("ASI_Camera_SDK")
                            .join("ASI_Windows_SDK_V1.40").join("ASI SDK").join("lib").join("x64")
                            .join("ASICamera2.dll");
                        lib_paths.push(sdk_path.to_string_lossy().to_string());
                    }
                    
                    // Check 2 levels up
                    if let Some(grandparent) = exe_dir.parent().and_then(|p| p.parent()) {
                        lib_paths.push(grandparent.join("ASICamera2.dll").to_string_lossy().to_string());
                        
                        let sdk_path = grandparent.join("SDKs").join("ZWO").join("ASI_Camera_SDK")
                            .join("ASI_Windows_SDK_V1.40").join("ASI SDK").join("lib").join("x64")
                            .join("ASICamera2.dll");
                        lib_paths.push(sdk_path.to_string_lossy().to_string());
                    }
                }
            }
        } else if cfg!(target_os = "macos") {
            lib_paths.push("libASICamera2.dylib".to_string());
            lib_paths.push("/usr/local/lib/libASICamera2.dylib".to_string());
        } else {
            lib_paths.push("libASICamera2.so".to_string());
            lib_paths.push("libASICamera2.so.1".to_string());
            lib_paths.push("/usr/lib/libASICamera2.so".to_string());
            lib_paths.push("/usr/local/lib/libASICamera2.so".to_string());
        };

        for path in &lib_paths {
            tracing::debug!("Trying to load ASI SDK from: {}", path);
            unsafe {
                match libloading::Library::new(path) {
                    Ok(lib) => {
                        tracing::info!("Found ASI SDK at: {}", path);

                        // Helper to load and log function pointer failures
                        fn load_symbol<T: Copy>(lib: &libloading::Library, name: &[u8], name_str: &str) -> Option<T> {
                            match unsafe { lib.get::<T>(name) } {
                                Ok(sym) => Some(*sym),
                                Err(e) => {
                                    tracing::error!("Failed to load ASI function '{}': {}", name_str, e);
                                    None
                                }
                            }
                        }

                        let get_num_cameras = load_symbol(&lib, b"ASIGetNumOfConnectedCameras\0", "ASIGetNumOfConnectedCameras")?;
                        let get_camera_property = load_symbol(&lib, b"ASIGetCameraProperty\0", "ASIGetCameraProperty")?;
                        let open_camera = load_symbol(&lib, b"ASIOpenCamera\0", "ASIOpenCamera")?;
                        let init_camera = load_symbol(&lib, b"ASIInitCamera\0", "ASIInitCamera")?;
                        let close_camera = load_symbol(&lib, b"ASICloseCamera\0", "ASICloseCamera")?;
                        let get_control_value = load_symbol(&lib, b"ASIGetControlValue\0", "ASIGetControlValue")?;
                        let set_control_value = load_symbol(&lib, b"ASISetControlValue\0", "ASISetControlValue")?;
                        let set_roi_format = load_symbol(&lib, b"ASISetROIFormat\0", "ASISetROIFormat")?;
                        let set_start_pos = load_symbol(&lib, b"ASISetStartPos\0", "ASISetStartPos")?;
                        let get_roi_format = load_symbol(&lib, b"ASIGetROIFormat\0", "ASIGetROIFormat")?;
                        let start_exposure = load_symbol(&lib, b"ASIStartExposure\0", "ASIStartExposure")?;
                        let stop_exposure = load_symbol(&lib, b"ASIStopExposure\0", "ASIStopExposure")?;
                        let get_exp_status = load_symbol(&lib, b"ASIGetExpStatus\0", "ASIGetExpStatus")?;
                        let get_data_after_exp = load_symbol(&lib, b"ASIGetDataAfterExp\0", "ASIGetDataAfterExp")?;
                        let get_num_controls = load_symbol(&lib, b"ASIGetNumOfControls\0", "ASIGetNumOfControls")?;
                        let get_control_caps = load_symbol(&lib, b"ASIGetControlCaps\0", "ASIGetControlCaps")?;

                        let sdk_result = Self {
                            get_num_cameras,
                            get_camera_property,
                            open_camera,
                            init_camera,
                            close_camera,
                            get_control_value,
                            set_control_value,
                            set_roi_format,
                            set_start_pos,
                            get_roi_format,
                            start_exposure,
                            stop_exposure,
                            get_exp_status,
                            get_data_after_exp,
                            get_num_controls,
                            get_control_caps,
                            lib,
                        };

                        tracing::info!("Successfully loaded all ASI SDK functions from: {}", path);
                        return Some(sdk_result);
                    }
                    Err(e) => {
                        // Always log DLL load failures so users can diagnose issues
                        tracing::debug!("ASI SDK not found at {}: {}", path, e);
                    }
                }
            }
        }
        
        // Try to find via PATH environment variable as last resort
        #[cfg(windows)]
        {
            tracing::debug!("Trying to load ASI SDK from system PATH");
            unsafe {
                match libloading::Library::new("ASICamera2.dll") {
                    Ok(lib) => {
                        tracing::info!("Found ASI SDK via system PATH");

                        // Helper to load and log function pointer failures
                        fn load_symbol<T: Copy>(lib: &libloading::Library, name: &[u8], name_str: &str) -> Option<T> {
                            match unsafe { lib.get::<T>(name) } {
                                Ok(sym) => Some(*sym),
                                Err(e) => {
                                    tracing::error!("Failed to load ASI function '{}': {}", name_str, e);
                                    None
                                }
                            }
                        }

                        let get_num_cameras = load_symbol(&lib, b"ASIGetNumOfConnectedCameras\0", "ASIGetNumOfConnectedCameras")?;
                        let get_camera_property = load_symbol(&lib, b"ASIGetCameraProperty\0", "ASIGetCameraProperty")?;
                        let open_camera = load_symbol(&lib, b"ASIOpenCamera\0", "ASIOpenCamera")?;
                        let init_camera = load_symbol(&lib, b"ASIInitCamera\0", "ASIInitCamera")?;
                        let close_camera = load_symbol(&lib, b"ASICloseCamera\0", "ASICloseCamera")?;
                        let get_control_value = load_symbol(&lib, b"ASIGetControlValue\0", "ASIGetControlValue")?;
                        let set_control_value = load_symbol(&lib, b"ASISetControlValue\0", "ASISetControlValue")?;
                        let set_roi_format = load_symbol(&lib, b"ASISetROIFormat\0", "ASISetROIFormat")?;
                        let set_start_pos = load_symbol(&lib, b"ASISetStartPos\0", "ASISetStartPos")?;
                        let get_roi_format = load_symbol(&lib, b"ASIGetROIFormat\0", "ASIGetROIFormat")?;
                        let start_exposure = load_symbol(&lib, b"ASIStartExposure\0", "ASIStartExposure")?;
                        let stop_exposure = load_symbol(&lib, b"ASIStopExposure\0", "ASIStopExposure")?;
                        let get_exp_status = load_symbol(&lib, b"ASIGetExpStatus\0", "ASIGetExpStatus")?;
                        let get_data_after_exp = load_symbol(&lib, b"ASIGetDataAfterExp\0", "ASIGetDataAfterExp")?;
                        let get_num_controls = load_symbol(&lib, b"ASIGetNumOfControls\0", "ASIGetNumOfControls")?;
                        let get_control_caps = load_symbol(&lib, b"ASIGetControlCaps\0", "ASIGetControlCaps")?;

                        let sdk = Self {
                            get_num_cameras,
                            get_camera_property,
                            open_camera,
                            init_camera,
                            close_camera,
                            get_control_value,
                            set_control_value,
                            set_roi_format,
                            set_start_pos,
                            get_roi_format,
                            start_exposure,
                            stop_exposure,
                            get_exp_status,
                            get_data_after_exp,
                            get_num_controls,
                            get_control_caps,
                            lib,
                        };
                        tracing::info!("Successfully loaded all ASI SDK functions from system PATH");
                        return Some(sdk);
                    }
                    Err(e) => {
                        tracing::debug!("ASI SDK not found in system PATH: {}", e);
                    }
                }
            }
        }

        tracing::error!("ZWO ASI SDK (ASICamera2.dll) not found! Checked {} locations. Native ZWO camera support will be unavailable.", lib_paths.len());
        tracing::error!("To use native ZWO drivers, install the ASI SDK from https://astronomy-imaging-camera.com/software-drivers or place ASICamera2.dll in the application directory.");
        None
    }
    
    /// Get the global SDK instance
    fn get() -> Option<&'static AsiSdk> {
        ASI_SDK.get_or_init(|| Self::load()).as_ref()
    }
}

/// Check ASI error and convert to NativeError with detailed messages
fn check_asi_error(code: c_int) -> Result<(), NativeError> {
    match code {
        0 => Ok(()),
        1 => Err(NativeError::InvalidDevice("ASI_ERROR_INVALID_INDEX: No camera connected or camera index out of bounds".to_string())),
        2 => Err(NativeError::InvalidDevice("ASI_ERROR_INVALID_ID: Invalid camera ID - camera may have been disconnected".to_string())),
        3 => Err(NativeError::SdkError("ASI_ERROR_INVALID_CONTROL_TYPE: Invalid control type".to_string())),
        4 => Err(NativeError::NotConnected),
        5 => Err(NativeError::Disconnected),
        6 => Err(NativeError::SdkError("ASI_ERROR_INVALID_PATH: Cannot find file path".to_string())),
        7 => Err(NativeError::SdkError("ASI_ERROR_INVALID_FILEFORMAT: Invalid file format".to_string())),
        8 => Err(NativeError::SdkError("ASI_ERROR_INVALID_SIZE: Invalid video format size".to_string())),
        9 => Err(NativeError::SdkError("ASI_ERROR_INVALID_IMGTYPE: Unsupported image format".to_string())),
        10 => Err(NativeError::SdkError("ASI_ERROR_OUTOF_BOUNDARY: Start position out of boundary".to_string())),
        11 => Err(NativeError::Timeout("ASI_ERROR_TIMEOUT: Operation timed out".to_string())),
        12 => Err(NativeError::SdkError("ASI_ERROR_INVALID_SEQUENCE: Invalid operation sequence - stop capture first".to_string())),
        13 => Err(NativeError::SdkError("ASI_ERROR_BUFFER_TOO_SMALL: Buffer size is too small".to_string())),
        14 => Err(NativeError::SdkError("ASI_ERROR_VIDEO_MODE_ACTIVE: Camera is in video mode - may be in use by another application".to_string())),
        15 => Err(NativeError::SdkError("ASI_ERROR_EXPOSURE_IN_PROGRESS: Exposure in progress".to_string())),
        16 => Err(NativeError::SdkError("ASI_ERROR_GENERAL_ERROR: General error - camera may be in use by another application (NINA, SharpCap, etc.)".to_string())),
        17 => Err(NativeError::SdkError("ASI_ERROR_INVALID_MODE: Invalid mode".to_string())),
        _ => Err(NativeError::SdkError(format!("Unknown ASI error code: {}", code))),
    }
}

// =============================================================================
// ZWO CAMERA IMPLEMENTATION
// =============================================================================

/// ZWO ASI Camera implementation
#[derive(Debug)]
pub struct ZwoCamera {
    camera_id: i32,
    camera_info: Option<ASICameraInfo>,
    connected: bool,
    device_id: String,
    current_bin: i32,
    current_width: i32,
    current_height: i32,
    image_type: ASIImgType,

    // Current settings tracking
    current_gain: i32,
    current_offset: i32,
    // Exposure metadata tracking
    exposure_time: f64,
    current_subframe: Option<SubFrame>,
}

impl ZwoCamera {
    /// Create a new ZWO camera instance
    pub fn new(camera_id: i32) -> Self {
        Self {
            camera_id,
            camera_info: None,
            connected: false,
            device_id: format!("native:zwo:{}", camera_id),
            current_bin: 1,
            current_width: 0,
            current_height: 0,
            image_type: ASIImgType::Raw16,
            current_gain: 0,
            current_offset: 0,
            exposure_time: 0.0,
            current_subframe: None,
        }
    }
    
    /// Load camera info from SDK
    fn load_camera_info(&mut self) -> Result<(), NativeError> {
        let sdk = AsiSdk::get().ok_or(NativeError::SdkNotLoaded)?;

        let mut info: ASICameraInfo = unsafe { std::mem::zeroed() };
        // ASIGetCameraProperty(ASI_CAMERA_INFO *pASICameraInfo, int iCameraIndex)
        let result = unsafe { (sdk.get_camera_property)(&mut info, self.camera_id) };
        check_asi_error(result)?;
        
        self.current_width = info.max_width as i32;
        self.current_height = info.max_height as i32;
        self.camera_info = Some(info);
        Ok(())
    }
    
    /// Get camera name using safe string conversion
    fn camera_name(&self) -> String {
        if let Some(info) = &self.camera_info {
            // Use safe string conversion with bounded length
            safe_cstr_to_string(info.name.as_ptr(), 64)
        } else {
            format!("ZWO Camera {}", self.camera_id)
        }
    }
    
    /// Get a control value
    fn get_control(&self, control: ASIControlType) -> Result<c_long, NativeError> {
        let sdk = AsiSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        let mut value: c_long = 0;
        let mut is_auto: ASIBool = ASI_FALSE;
        let result = unsafe { 
            (sdk.get_control_value)(self.camera_id, control as c_int, &mut value, &mut is_auto) 
        };
        check_asi_error(result)?;
        Ok(value)
    }
    
    /// Set a control value
    fn set_control(&mut self, control: ASIControlType, value: c_long, auto: bool) -> Result<(), NativeError> {
        let sdk = AsiSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        let result = unsafe {
            (sdk.set_control_value)(
                self.camera_id,
                control as c_int,
                value,
                if auto { ASI_TRUE } else { ASI_FALSE }
            )
        };
        check_asi_error(result)
    }

    /// Get the min/max range for a control
    fn get_control_range(&self, target_control: ASIControlType) -> Result<(i32, i32), NativeError> {
        let sdk = AsiSdk::get().ok_or(NativeError::SdkNotLoaded)?;

        // Get number of controls
        let mut num_controls: c_int = 0;
        let result = unsafe { (sdk.get_num_controls)(self.camera_id, &mut num_controls) };
        check_asi_error(result)?;

        // Search for the specific control
        for i in 0..num_controls {
            let mut caps: ASIControlCaps = unsafe { std::mem::zeroed() };
            let result = unsafe { (sdk.get_control_caps)(self.camera_id, i, &mut caps) };
            if result == 0 {
                // Check if this is the control we're looking for
                // The control_type field tells us which control this is
                if caps.control_type as c_int == target_control as c_int {
                    return Ok((caps.min_value as i32, caps.max_value as i32));
                }
            }
        }

        Err(NativeError::NotSupported)
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
                    "ZWO image download timed out after {:?}",
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
impl NativeDevice for ZwoCamera {
    fn id(&self) -> &str {
        &self.device_id
    }
    
    fn name(&self) -> &str {
        // We need to return a &str, but camera_name() returns String
        // For now, return the device_id - a proper implementation would store the name
        &self.device_id
    }
    
    fn vendor(&self) -> NativeVendor {
        NativeVendor::Zwo
    }
    
    fn is_connected(&self) -> bool {
        self.connected
    }
    
    async fn connect(&mut self) -> Result<(), NativeError> {
        tracing::info!("Connecting to ZWO camera ID {}...", self.camera_id);

        let sdk = AsiSdk::get().ok_or_else(|| {
            tracing::error!("Cannot connect to ZWO camera: ASI SDK not loaded");
            NativeError::SdkNotLoaded
        })?;

        // Load camera info
        tracing::debug!("Loading camera info for ID {}", self.camera_id);
        self.load_camera_info().map_err(|e| {
            tracing::error!("Failed to load camera info for ID {}: {:?}", self.camera_id, e);
            e
        })?;
        tracing::debug!("Camera info loaded successfully");

        // Open camera
        tracing::debug!("Opening camera ID {}", self.camera_id);
        let result = unsafe { (sdk.open_camera)(self.camera_id) };
        if result != 0 {
            tracing::error!("ASIOpenCamera failed for ID {}: ASI error code {}", self.camera_id, result);
            return Err(check_asi_error(result).unwrap_err());
        }
        tracing::debug!("Camera opened successfully");

        // Create cleanup guard to close the camera if subsequent operations fail
        let camera_id = self.camera_id;
        let cleanup_guard = CleanupGuard::new(|| {
            tracing::debug!("Cleaning up ZWO camera {} after failed connect", camera_id);
            if let Some(sdk) = AsiSdk::get() {
                let _ = unsafe { (sdk.close_camera)(camera_id) };
            }
        });

        // Initialize camera
        tracing::debug!("Initializing camera ID {}", self.camera_id);
        let result = unsafe { (sdk.init_camera)(self.camera_id) };
        if result != 0 {
            tracing::error!("ASIInitCamera failed for ID {}: ASI error code {}", self.camera_id, result);
            // cleanup_guard will handle closing the camera
            return Err(check_asi_error(result).unwrap_err());
        }
        tracing::debug!("Camera initialized successfully");

        // Set default ROI format (full frame, bin 1, Raw16)
        if let Some(info) = &self.camera_info {
            tracing::debug!("Setting ROI format: {}x{}, bin 1, Raw16", info.max_width, info.max_height);
            let result = unsafe {
                (sdk.set_roi_format)(
                    self.camera_id,
                    info.max_width as c_int,
                    info.max_height as c_int,
                    1, // bin
                    ASIImgType::Raw16 as c_int,
                )
            };
            if result != 0 {
                tracing::error!("ASISetROIFormat failed: ASI error code {}", result);
                return Err(check_asi_error(result).unwrap_err());
            }
            tracing::debug!("ROI format set successfully");
        }

        // Get current gain and offset
        tracing::debug!("Reading current gain and offset");
        if let Ok(val) = self.get_control(ASIControlType::ASI_GAIN) {
            self.current_gain = val as i32;
            tracing::debug!("Current gain: {}", self.current_gain);
        }
        if let Ok(val) = self.get_control(ASIControlType::ASI_OFFSET) {
            self.current_offset = val as i32;
            tracing::debug!("Current offset: {}", self.current_offset);
        }

        // All operations succeeded - defuse the cleanup guard
        cleanup_guard.defuse();

        self.connected = true;
        tracing::info!("Successfully connected to ZWO camera: {}", self.camera_name());
        Ok(())
    }
    
    async fn disconnect(&mut self) -> Result<(), NativeError> {
        if self.connected {
            let sdk = AsiSdk::get().ok_or(NativeError::SdkNotLoaded)?;
            let result = unsafe { (sdk.close_camera)(self.camera_id) };
            check_asi_error(result)?;
            self.connected = false;
            tracing::info!("Disconnected from {}", self.camera_name());
        }
        Ok(())
    }
}

#[async_trait]
impl NativeCamera for ZwoCamera {
    fn capabilities(&self) -> CameraCapabilities {
        if let Some(info) = &self.camera_info {
            CameraCapabilities {
                can_cool: info.is_cooler_cam != 0,
                can_set_gain: true,
                can_set_offset: true,
                can_set_binning: true,
                can_subframe: true,
                has_shutter: info.mechanical_shutter != 0,
                has_guider_port: info.st4_port != 0,
                max_bin_x: 4,
                max_bin_y: 4,
                supports_readout_modes: false,
            }
        } else {
            CameraCapabilities::default()
        }
    }
    
    async fn get_status(&self) -> Result<CameraStatus, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }
        
        let sdk = AsiSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        
        let mut exp_status: c_int = 0;
        let result = unsafe { (sdk.get_exp_status)(self.camera_id, &mut exp_status) };
        check_asi_error(result)?;
        
        let state = match exp_status {
            0 => CameraState::Idle,
            1 => CameraState::Exposing,
            2 => CameraState::Downloading,
            _ => CameraState::Error,
        };
        
        // Get temperature (ASI_TEMPERATURE returns 10*temperature)
        let temp = self.get_control(ASIControlType::ASI_TEMPERATURE)
            .map(|v| v as f64 / 10.0)
            .unwrap_or(0.0);
        
        let cooler_power = if self.camera_info.as_ref().map(|i| i.is_cooler_cam != 0).unwrap_or(false) {
            self.get_control(ASIControlType::ASI_COOLER_POWER_PERC).ok().map(|v| v as f64)
        } else {
            None
        };
        
        Ok(CameraStatus {
            state,
            sensor_temp: Some(temp),
            target_temp: None, // ZWO doesn't easily provide target temp back
            cooler_on: false, // ZWO SDK doesn't have a simple "is cooler on" property check
            cooler_power,
            gain: self.current_gain,
            offset: self.current_offset,
            bin_x: self.current_bin,
            bin_y: self.current_bin,
            exposure_remaining: None,
        })
    }
    
    async fn start_exposure(&mut self, params: ExposureParams) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }
        
        let sdk = AsiSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        
        // Set exposure time (in microseconds)
        let exposure_us = (params.duration_secs * 1_000_000.0) as c_long;
        self.set_control(ASIControlType::ASI_EXPOSURE, exposure_us, false)?;
        
        // Set gain
        if let Some(gain) = params.gain {
            self.set_control(ASIControlType::ASI_GAIN, gain as c_long, false)?;
            self.current_gain = gain;
        }
        
        // Set offset if provided
        if let Some(offset) = params.offset {
            self.set_control(ASIControlType::ASI_OFFSET, offset as c_long, false)?;
            self.current_offset = offset;
        }
        
        // Start exposure (false = not dark frame)
        let result = unsafe { (sdk.start_exposure)(self.camera_id, ASI_FALSE) };
        check_asi_error(result)?;

        // Track exposure time for metadata
        self.exposure_time = params.duration_secs;

        tracing::info!("Started {}s exposure", params.duration_secs);
        Ok(())
    }
    
    async fn abort_exposure(&mut self) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }
        
        let sdk = AsiSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        let result = unsafe { (sdk.stop_exposure)(self.camera_id) };
        check_asi_error(result)?;
        
        tracing::info!("Aborted exposure");
        Ok(())
    }
    
    async fn is_exposure_complete(&self) -> Result<bool, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = AsiSdk::get().ok_or(NativeError::SdkNotLoaded)?;

        let mut status: c_int = 0;
        let result = unsafe { (sdk.get_exp_status)(self.camera_id, &mut status) };
        check_asi_error(result)?;

        let is_complete = status == ASIExposureStatus::Success as c_int;
        // Log status for debugging (0=Idle, 1=Working, 2=Success, 3=Failed)
        if is_complete || status == ASIExposureStatus::Failed as c_int {
            tracing::info!("ZWO exposure status: {} ({})", status,
                match status {
                    0 => "Idle",
                    1 => "Working",
                    2 => "Success",
                    3 => "Failed",
                    _ => "Unknown"
                }
            );
        }

        Ok(is_complete)
    }
    
    async fn download_image(&mut self) -> Result<ImageData, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }
        
        let sdk = AsiSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        
        // Get current ROI
        let mut width: c_int = 0;
        let mut height: c_int = 0;
        let mut bin: c_int = 0;
        let mut img_type: c_int = 0;
        
        let result = unsafe { 
            (sdk.get_roi_format)(self.camera_id, &mut width, &mut height, &mut bin, &mut img_type) 
        };
        check_asi_error(result)?;
        
        // Calculate buffer size (Raw16 = 2 bytes per pixel) with overflow protection
        let bytes_per_pixel = if img_type == ASIImgType::Raw16 as c_int { 2 } else { 1 };
        let buffer_size = calculate_buffer_size_i32(width, height, bytes_per_pixel)?;
        
        let mut buffer: Vec<u8> = vec![0u8; buffer_size];
        
        let result = unsafe {
            (sdk.get_data_after_exp)(self.camera_id, buffer.as_mut_ptr(), buffer_size as c_long)
        };
        check_asi_error(result)?;
        
        // Convert to u16 if needed
        let data: Vec<u16> = if bytes_per_pixel == 2 {
            buffer
                .chunks_exact(2)
                .map(|chunk| u16::from_ne_bytes([chunk[0], chunk[1]]))
                .collect()
        } else {
            buffer.iter().map(|&x| (x as u16) * 256).collect()
        };

        // DIAGNOSTIC: Log data statistics to debug mid-gray image issue
        if !data.is_empty() {
            let min_val = data.iter().min().copied().unwrap_or(0);
            let max_val = data.iter().max().copied().unwrap_or(0);
            let sum: u64 = data.iter().map(|&x| x as u64).sum();
            let avg_val = sum / data.len() as u64;
            let non_zero_count = data.iter().filter(|&&x| x != 0).count();
            tracing::info!(
                "ZWO DIAGNOSTIC: Raw buffer stats - min={}, max={}, avg={}, non_zero={}/{}, img_type={}",
                min_val, max_val, avg_val, non_zero_count, data.len(), img_type
            );
            if min_val == max_val {
                tracing::warn!(
                    "ZWO WARNING: All pixels have same value {}! This indicates no actual image data was captured.",
                    min_val
                );
            }
        }

        tracing::info!("Downloaded {}x{} image ({} bytes, img_type={})", width, height, buffer_size, img_type);
        
        Ok(ImageData {
            width: width as u32,
            height: height as u32,
            data,
            bits_per_pixel: if bytes_per_pixel == 2 { 16 } else { 8 },
            bayer_pattern: self.camera_info.as_ref()
                .filter(|i| i.is_color_cam != 0)
                .map(|i| match i.bayer_pattern {
                    0 => BayerPattern::Rggb,
                    1 => BayerPattern::Bggr,
                    2 => BayerPattern::Grbg,
                    3 => BayerPattern::Gbrg,
                    _ => BayerPattern::Rggb,
                }),
            metadata: ImageMetadata {
                exposure_time: self.exposure_time,
                gain: self.current_gain,
                offset: self.current_offset,
                bin_x: self.current_bin,
                bin_y: self.current_bin,
                temperature: self.get_temperature().await.ok(),
                timestamp: chrono::Utc::now(),
                subframe: self.current_subframe.clone(),
                readout_mode: None,
                vendor_data: self.get_vendor_features().await?,
            },
        })
    }
    
    async fn set_cooler(&mut self, enabled: bool, target_temp: f64) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }
        
        if !self.camera_info.as_ref().map(|i| i.is_cooler_cam != 0).unwrap_or(false) {
            return Err(NativeError::NotSupported);
        }
        
        // Set target temperature (ASI_TARGET_TEMP is NOT multiplied by 10, direct degrees C)
        self.set_control(ASIControlType::ASI_TARGET_TEMP, target_temp as c_long, false)?;
        
        // Enable/disable cooler
        self.set_control(ASIControlType::ASI_COOLER_ON, if enabled { 1 } else { 0 }, false)?;
        
        Ok(())
    }
    
    async fn get_temperature(&self) -> Result<f64, NativeError> {
        // ASI_TEMPERATURE returns 10*temperature
        let value = self.get_control(ASIControlType::ASI_TEMPERATURE)?;
        Ok(value as f64 / 10.0)
    }
    
    async fn get_cooler_power(&self) -> Result<f64, NativeError> {
        if !self.camera_info.as_ref().map(|i| i.is_cooler_cam != 0).unwrap_or(false) {
            return Err(NativeError::NotSupported);
        }
        let value = self.get_control(ASIControlType::ASI_COOLER_POWER_PERC)?;
        Ok(value as f64)
    }
    
    async fn set_gain(&mut self, gain: i32) -> Result<(), NativeError> {
        self.current_gain = gain;
        self.set_control(ASIControlType::ASI_GAIN, gain as c_long, false)
    }
    
    async fn get_gain(&self) -> Result<i32, NativeError> {
        let val = self.get_control(ASIControlType::ASI_GAIN).map(|v| v as i32)?;
        // self.current_gain = val; // Update cache?
        Ok(val)
    }
    
    async fn set_offset(&mut self, offset: i32) -> Result<(), NativeError> {
        self.current_offset = offset;
        self.set_control(ASIControlType::ASI_OFFSET, offset as c_long, false)
    }
    
    async fn get_offset(&self) -> Result<i32, NativeError> {
        let val = self.get_control(ASIControlType::ASI_OFFSET).map(|v| v as i32)?;
        // self.current_offset = val;
        Ok(val)
    }
    
    async fn set_binning(&mut self, bin_x: i32, bin_y: i32) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }
        
        let sdk = AsiSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        
        // ZWO only supports symmetric binning
        let bin = bin_x.max(bin_y);
        
        // Calculate new dimensions
        let info = self.camera_info.as_ref().ok_or(NativeError::NotConnected)?;
        let new_width = info.max_width as i32 / bin;
        let new_height = info.max_height as i32 / bin;
        
        let result = unsafe {
            (sdk.set_roi_format)(
                self.camera_id,
                new_width as c_int,
                new_height as c_int,
                bin as c_int,
                self.image_type as c_int,
            )
        };
        check_asi_error(result)?;
        
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
        
        let sdk = AsiSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        let info = self.camera_info.as_ref().ok_or(NativeError::NotConnected)?;
        
        let (width, height, x, y) = if let Some(ref sf) = subframe {
            (sf.width as c_int, sf.height as c_int, sf.start_x as c_int, sf.start_y as c_int)
        } else {
            (info.max_width as c_int / self.current_bin as c_int, 
             info.max_height as c_int / self.current_bin as c_int,
             0, 0)
        };
        
        let result = unsafe {
            (sdk.set_roi_format)(
                self.camera_id,
                width,
                height,
                self.current_bin as c_int,
                self.image_type as c_int,
            )
        };
        check_asi_error(result)?;
        
        let result = unsafe {
             (sdk.set_start_pos)(self.camera_id, x, y)
        };
        check_asi_error(result)?;

        self.current_width = width as i32;
        self.current_height = height as i32;
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
                color: info.is_color_cam != 0,
                bayer_pattern: if info.is_color_cam != 0 {
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
        // ZWO doesn't have readout modes
        Ok(Vec::new())
    }
    
    async fn set_readout_mode(&mut self, _mode: &ReadoutMode) -> Result<(), NativeError> {
        Err(NativeError::NotSupported)
    }
    
    async fn get_vendor_features(&self) -> Result<VendorFeatures, NativeError> {
        let mut features = VendorFeatures::default();

        // Get USB bandwidth
        if let Ok(bw) = self.get_control(ASIControlType::ASI_BANDWIDTHOVERLOAD) {
            features.usb_bandwidth = Some(bw as f64);
        }

        // ZWO-specific: Anti-dew heater
        if let Ok(heater) = self.get_control(ASIControlType::ASI_ANTI_DEW_HEATER) {
            features.anti_dew_heater = Some(heater != 0);
        }

        Ok(features)
    }

    async fn get_gain_range(&self) -> Result<(i32, i32), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }
        self.get_control_range(ASIControlType::ASI_GAIN)
    }

    async fn get_offset_range(&self) -> Result<(i32, i32), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }
        self.get_control_range(ASIControlType::ASI_OFFSET)
    }
}

// =============================================================================
// ZWO CAMERA DISCOVERY
// =============================================================================

/// ZWO camera discovery info
pub struct ZwoDiscoveryInfo {
    pub camera_id: i32,
    pub name: String,
    /// Discovery index (0-based) for disambiguation when multiple same-model cameras
    /// ZWO SDK doesn't expose serial numbers, so we use index instead
    pub discovery_index: usize,
}

/// Check if ZWO SDK is available
pub fn is_sdk_available() -> bool {
    AsiSdk::get().is_some()
}

/// Check if ZWO SDK is loaded and return status message
pub fn get_sdk_status() -> (bool, String) {
    match AsiSdk::get() {
        Some(_) => (true, "ZWO ASI SDK loaded successfully".to_string()),
        None => (false, "ZWO ASI SDK (ASICamera2.dll) not found. Install the ASI SDK or use ASCOM drivers instead.".to_string()),
    }
}

/// Discover ZWO cameras
pub async fn discover_devices() -> Result<Vec<ZwoDiscoveryInfo>, NativeError> {
    let sdk = match AsiSdk::get() {
        Some(sdk) => sdk,
        None => {
            // Log prominently so users know why discovery returned nothing
            tracing::warn!("ZWO native camera discovery skipped: ASI SDK not loaded. If you have ZWO cameras, either install the ASI SDK or use ASCOM/Alpaca drivers.");
            return Ok(Vec::new());
        }
    };

    tracing::info!("Discovering ZWO cameras via native ASI SDK...");
    let num_cameras = unsafe { (sdk.get_num_cameras)() };
    tracing::info!("ASI SDK reports {} connected camera(s)", num_cameras);

    let mut cameras = Vec::new();
    let mut failed_count = 0;

    for i in 0..num_cameras {
        let mut info: ASICameraInfo = unsafe { std::mem::zeroed() };
        // ASIGetCameraProperty(ASI_CAMERA_INFO *pASICameraInfo, int iCameraIndex)
        let result = unsafe { (sdk.get_camera_property)(&mut info, i) };

        if result == 0 {
            let name = unsafe {
                CStr::from_ptr(info.name.as_ptr())
                    .to_string_lossy()
                    .to_string()
            };
            tracing::info!("Found ZWO camera: {} (ID: {})", name, i);

            cameras.push(ZwoDiscoveryInfo {
                camera_id: i,
                name,
                discovery_index: i as usize,
            });
        } else {
            failed_count += 1;
            let error_desc = match result {
                1 => "INVALID_INDEX - camera may be in use by another application",
                2 => "INVALID_ID",
                3 => "INVALID_CONTROL_TYPE",
                4 => "CAMERA_CLOSED",
                5 => "CAMERA_REMOVED - camera was disconnected",
                6 => "INVALID_PATH",
                7 => "INVALID_FILEFORMAT",
                8 => "INVALID_SIZE",
                9 => "INVALID_IMGTYPE",
                10 => "OUTOF_BOUNDARY",
                11 => "TIMEOUT",
                12 => "INVALID_SEQUENCE",
                13 => "BUFFER_TOO_SMALL",
                14 => "VIDEO_MODE_ACTIVE",
                15 => "EXPOSURE_IN_PROGRESS",
                16 => "GENERAL_ERROR - camera may be in use by another application",
                17 => "INVALID_MODE",
                18 => "GPS_NOT_SUPPORTED",
                19 => "GPS_VER_ERROR",
                20 => "GPS_FPGA_ERROR",
                21 => "GPS_DATA_ERROR",
                22 => "END",
                _ => "UNKNOWN",
            };
            tracing::warn!(
                "Failed to query camera index {}: ASI error {} ({})",
                i, result, error_desc
            );
        }
    }

    if cameras.is_empty() && num_cameras > 0 {
        tracing::error!(
            "ASI SDK detected {} camera(s) but none could be queried. \
            This usually means the cameras are in use by another application \
            (NINA, SharpCap, APT, PHD2, etc.). Close other astrophotography software and try again.",
            num_cameras
        );
    } else if failed_count > 0 {
        tracing::warn!(
            "Successfully discovered {} of {} cameras. {} camera(s) may be in use by other software.",
            cameras.len(), num_cameras, failed_count
        );
    }

    Ok(cameras)
}

// =============================================================================
// ZWO EAF FOCUSER SDK
// =============================================================================

/// EAF Info structure from SDK
#[repr(C)]
#[derive(Debug, Clone)]
struct EAFInfo {
    id: c_int,
    name: [c_char; 64],
    max_step: c_int,
}

/// EAF Error codes
#[repr(C)]
#[derive(Debug, Clone, Copy, PartialEq)]
#[allow(non_camel_case_types, dead_code)]
enum EAFError {
    EAF_SUCCESS = 0,
    EAF_ERROR_INVALID_INDEX = 1,
    EAF_ERROR_INVALID_ID = 2,
    EAF_ERROR_INVALID_VALUE = 3,
    EAF_ERROR_REMOVED = 4,
    EAF_ERROR_MOVING = 5,
    EAF_ERROR_ERROR_STATE = 6,
    EAF_ERROR_GENERAL_ERROR = 7,
    EAF_ERROR_NOT_SUPPORTED = 8,
    EAF_ERROR_CLOSED = 9,
    EAF_ERROR_END = -1,
}

/// EAF ID/Serial Number structure
#[repr(C)]
#[derive(Debug, Clone)]
struct EAFSerialNumber {
    id: [c_uchar; 8],
}

/// EAF SDK function pointers
struct EafSdk {
    get_num: unsafe extern "C" fn() -> c_int,
    get_id: unsafe extern "C" fn(c_int, *mut c_int) -> c_int,
    open: unsafe extern "C" fn(c_int) -> c_int,
    close: unsafe extern "C" fn(c_int) -> c_int,
    get_property: unsafe extern "C" fn(c_int, *mut EAFInfo) -> c_int,
    move_to: unsafe extern "C" fn(c_int, c_int) -> c_int,
    stop: unsafe extern "C" fn(c_int) -> c_int,
    is_moving: unsafe extern "C" fn(c_int, *mut bool, *mut bool) -> c_int,
    get_position: unsafe extern "C" fn(c_int, *mut c_int) -> c_int,
    get_temp: unsafe extern "C" fn(c_int, *mut f32) -> c_int,
    set_max_step: unsafe extern "C" fn(c_int, c_int) -> c_int,
    get_max_step: unsafe extern "C" fn(c_int, *mut c_int) -> c_int,
    set_backlash: unsafe extern "C" fn(c_int, c_int) -> c_int,
    get_backlash: unsafe extern "C" fn(c_int, *mut c_int) -> c_int,
    set_reverse: unsafe extern "C" fn(c_int, bool) -> c_int,
    get_reverse: unsafe extern "C" fn(c_int, *mut bool) -> c_int,
    set_beep: unsafe extern "C" fn(c_int, bool) -> c_int,
    get_beep: unsafe extern "C" fn(c_int, *mut bool) -> c_int,
    get_sdk_version: unsafe extern "C" fn() -> *const c_char,
    get_firmware_version: unsafe extern "C" fn(c_int, *mut c_uchar, *mut c_uchar, *mut c_uchar) -> c_int,
    get_serial_number: unsafe extern "C" fn(c_int, *mut EAFSerialNumber) -> c_int,
    reset_position: unsafe extern "C" fn(c_int, c_int) -> c_int,
    _library: libloading::Library,
}

static EAF_SDK: OnceLock<Option<EafSdk>> = OnceLock::new();

impl EafSdk {
    /// Try to load the EAF SDK library
    fn load() -> Option<Self> {
        #[cfg(target_os = "windows")]
        let lib_name = "EAF_focuser.dll";
        #[cfg(target_os = "macos")]
        let lib_name = "libEAF_focuser.dylib";
        #[cfg(target_os = "linux")]
        let lib_name = "libEAF_focuser.so";

        let library = unsafe { libloading::Library::new(lib_name).ok()? };

        unsafe {
            Some(EafSdk {
                get_num: *library.get(b"EAFGetNum\0").ok()?,
                get_id: *library.get(b"EAFGetID\0").ok()?,
                open: *library.get(b"EAFOpen\0").ok()?,
                close: *library.get(b"EAFClose\0").ok()?,
                get_property: *library.get(b"EAFGetProperty\0").ok()?,
                move_to: *library.get(b"EAFMove\0").ok()?,
                stop: *library.get(b"EAFStop\0").ok()?,
                is_moving: *library.get(b"EAFIsMoving\0").ok()?,
                get_position: *library.get(b"EAFGetPosition\0").ok()?,
                get_temp: *library.get(b"EAFGetTemp\0").ok()?,
                set_max_step: *library.get(b"EAFSetMaxStep\0").ok()?,
                get_max_step: *library.get(b"EAFGetMaxStep\0").ok()?,
                set_backlash: *library.get(b"EAFSetBacklash\0").ok()?,
                get_backlash: *library.get(b"EAFGetBacklash\0").ok()?,
                set_reverse: *library.get(b"EAFSetReverse\0").ok()?,
                get_reverse: *library.get(b"EAFGetReverse\0").ok()?,
                set_beep: *library.get(b"EAFSetBeep\0").ok()?,
                get_beep: *library.get(b"EAFGetBeep\0").ok()?,
                get_sdk_version: *library.get(b"EAFGetSDKVersion\0").ok()?,
                get_firmware_version: *library.get(b"EAFGetFirmwareVersion\0").ok()?,
                get_serial_number: *library.get(b"EAFGetSerialNumber\0").ok()?,
                reset_position: *library.get(b"EAFResetPostion\0").ok()?, // Note: typo in SDK
                _library: library,
            })
        }
    }

    /// Get the singleton SDK instance
    fn get() -> Option<&'static EafSdk> {
        EAF_SDK.get_or_init(|| Self::load()).as_ref()
    }
}

/// Check EAF error code and convert to NativeError
fn check_eaf_error(code: c_int) -> Result<(), NativeError> {
    match code {
        0 => Ok(()),
        1 => Err(NativeError::InvalidDevice("EAF_ERROR_INVALID_INDEX".to_string())),
        2 => Err(NativeError::InvalidDevice("EAF_ERROR_INVALID_ID".to_string())),
        3 => Err(NativeError::InvalidParameter("EAF_ERROR_INVALID_VALUE".to_string())),
        4 => Err(NativeError::Disconnected),
        5 => Err(NativeError::SdkError("EAF_ERROR_MOVING: Focuser is moving".to_string())),
        6 => Err(NativeError::SdkError("EAF_ERROR_ERROR_STATE: Focuser in error state".to_string())),
        7 => Err(NativeError::SdkError("EAF_ERROR_GENERAL_ERROR".to_string())),
        8 => Err(NativeError::NotSupported),
        9 => Err(NativeError::NotConnected),
        _ => Err(NativeError::SdkError(format!("Unknown EAF error code: {}", code))),
    }
}

// =============================================================================
// ZWO FOCUSER IMPLEMENTATION
// =============================================================================

/// ZWO EAF Focuser implementation
#[derive(Debug)]
pub struct ZwoFocuser {
    focuser_id: i32,
    device_id: String,
    connected: bool,
    max_position: i32,
    name: String,
}

impl ZwoFocuser {
    /// Create a new ZWO focuser instance
    pub fn new(focuser_id: i32) -> Self {
        Self {
            focuser_id,
            device_id: format!("native:zwo:eaf:{}", focuser_id),
            connected: false,
            max_position: 0,
            name: format!("ZWO EAF {}", focuser_id),
        }
    }
}

#[async_trait]
impl NativeDevice for ZwoFocuser {
    fn id(&self) -> &str {
        &self.device_id
    }

    fn name(&self) -> &str {
        &self.name
    }

    fn vendor(&self) -> NativeVendor {
        NativeVendor::Zwo
    }

    fn is_connected(&self) -> bool {
        self.connected
    }

    async fn connect(&mut self) -> Result<(), NativeError> {
        tracing::info!("Connecting to ZWO EAF focuser ID {}...", self.focuser_id);

        let sdk = EafSdk::get().ok_or_else(|| {
            tracing::error!("Cannot connect to ZWO EAF: EAF SDK not loaded");
            NativeError::SdkNotLoaded
        })?;

        // Open focuser
        let result = unsafe { (sdk.open)(self.focuser_id) };
        check_eaf_error(result)?;

        // Create cleanup guard to close the focuser if subsequent operations fail
        let focuser_id = self.focuser_id;
        let cleanup_guard = CleanupGuard::new(|| {
            tracing::debug!("Cleaning up ZWO EAF focuser {} after failed connect", focuser_id);
            if let Some(sdk) = EafSdk::get() {
                let _ = unsafe { (sdk.close)(focuser_id) };
            }
        });

        // Get properties
        let mut info: EAFInfo = unsafe { std::mem::zeroed() };
        let result = unsafe { (sdk.get_property)(self.focuser_id, &mut info) };
        check_eaf_error(result)?;

        self.max_position = info.max_step;
        self.name = safe_cstr_to_string(info.name.as_ptr(), 64);

        // All operations succeeded - defuse the cleanup guard
        cleanup_guard.defuse();

        self.connected = true;
        tracing::info!("Connected to ZWO EAF: {} (max step: {})", self.name, self.max_position);
        Ok(())
    }

    async fn disconnect(&mut self) -> Result<(), NativeError> {
        if !self.connected {
            return Ok(());
        }

        tracing::info!("Disconnecting from ZWO EAF focuser ID {}...", self.focuser_id);

        let sdk = EafSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        let result = unsafe { (sdk.close)(self.focuser_id) };
        check_eaf_error(result)?;

        self.connected = false;
        tracing::info!("Disconnected from ZWO EAF");
        Ok(())
    }
}

#[async_trait]
impl NativeFocuser for ZwoFocuser {
    async fn move_to(&mut self, position: i32) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = EafSdk::get().ok_or(NativeError::SdkNotLoaded)?;

        // Clamp position to valid range
        let target = position.clamp(0, self.max_position);

        tracing::debug!("Moving ZWO EAF to position {}", target);
        let result = unsafe { (sdk.move_to)(self.focuser_id, target) };
        check_eaf_error(result)
    }

    async fn move_relative(&mut self, steps: i32) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let current = self.get_position().await?;
        let target = (current + steps).clamp(0, self.max_position);
        self.move_to(target).await
    }

    async fn get_position(&self) -> Result<i32, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = EafSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        let mut position: c_int = 0;
        let result = unsafe { (sdk.get_position)(self.focuser_id, &mut position) };
        check_eaf_error(result)?;
        Ok(position)
    }

    async fn is_moving(&self) -> Result<bool, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = EafSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        let mut is_moving = false;
        let mut hand_control = false;
        let result = unsafe { (sdk.is_moving)(self.focuser_id, &mut is_moving, &mut hand_control) };
        check_eaf_error(result)?;
        Ok(is_moving)
    }

    async fn halt(&mut self) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = EafSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        tracing::debug!("Stopping ZWO EAF movement");
        let result = unsafe { (sdk.stop)(self.focuser_id) };
        check_eaf_error(result)
    }

    async fn get_temperature(&self) -> Result<Option<f64>, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = EafSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        let mut temp: f32 = 0.0;
        let result = unsafe { (sdk.get_temp)(self.focuser_id, &mut temp) };

        // Temperature of -273 means invalid/unavailable
        if result == 0 && temp > -273.0 {
            Ok(Some(temp as f64))
        } else {
            Ok(None)
        }
    }

    fn get_max_position(&self) -> i32 {
        self.max_position
    }

    fn get_step_size(&self) -> f64 {
        // EAF step size is approximately 8 microns per step
        8.0
    }
}

impl ZwoFocuser {
    /// Move focuser to position and wait for completion with timeout.
    ///
    /// This is a convenience method that combines `move_to` with waiting for
    /// the move to complete, with timeout protection.
    ///
    /// # Arguments
    /// * `position` - Target position to move to
    /// * `config` - Timeout configuration
    ///
    /// # Returns
    /// * `Ok(())` - Move completed successfully
    /// * `Err(NativeError::MoveTimeout)` - Move did not complete within timeout
    pub async fn move_to_with_timeout(
        &mut self,
        position: i32,
        config: &NativeTimeoutConfig,
    ) -> Result<(), NativeError> {
        // Start the move
        self.move_to(position).await?;

        // Wait for move to complete
        wait_for_focuser_move(
            || async { self.is_moving().await },
            config,
            position,
        )
        .await
    }

    /// Move focuser relative and wait for completion with timeout.
    ///
    /// # Arguments
    /// * `steps` - Number of steps to move (positive = outward, negative = inward)
    /// * `config` - Timeout configuration
    ///
    /// # Returns
    /// * `Ok(())` - Move completed successfully
    /// * `Err(NativeError::MoveTimeout)` - Move did not complete within timeout
    pub async fn move_relative_with_timeout(
        &mut self,
        steps: i32,
        config: &NativeTimeoutConfig,
    ) -> Result<(), NativeError> {
        // Calculate target position
        let current = self.get_position().await?;
        let target = (current + steps).clamp(0, self.max_position);

        // Use move_to_with_timeout
        self.move_to_with_timeout(target, config).await
    }
}

// =============================================================================
// ZWO FOCUSER DISCOVERY
// =============================================================================

/// ZWO focuser discovery info
pub struct ZwoFocuserDiscoveryInfo {
    pub focuser_id: i32,
    pub name: String,
    pub serial_number: Option<String>,
    pub discovery_index: usize,
}

/// Check if EAF SDK is available
pub fn is_eaf_sdk_available() -> bool {
    EafSdk::get().is_some()
}

/// Get EAF SDK status
pub fn get_eaf_sdk_status() -> (bool, String) {
    match EafSdk::get() {
        Some(_) => (true, "ZWO EAF SDK loaded successfully".to_string()),
        None => (false, "ZWO EAF SDK (EAF_focuser.dll) not found.".to_string()),
    }
}

/// Discover ZWO EAF focusers
pub async fn discover_focusers() -> Result<Vec<ZwoFocuserDiscoveryInfo>, NativeError> {
    let sdk = match EafSdk::get() {
        Some(sdk) => sdk,
        None => {
            tracing::warn!("ZWO EAF discovery skipped: EAF SDK not loaded");
            return Ok(Vec::new());
        }
    };

    tracing::info!("Discovering ZWO EAF focusers via native SDK...");
    let num_focusers = unsafe { (sdk.get_num)() };
    tracing::info!("EAF SDK reports {} connected focuser(s)", num_focusers);

    let mut focusers = Vec::new();

    for i in 0..num_focusers {
        let mut id: c_int = 0;
        let result = unsafe { (sdk.get_id)(i, &mut id) };

        if result == 0 {
            // Get focuser info
            let result = unsafe { (sdk.open)(id) };
            if result == 0 {
                let mut info: EAFInfo = unsafe { std::mem::zeroed() };
                let _ = unsafe { (sdk.get_property)(id, &mut info) };
                let name = unsafe {
                    CStr::from_ptr(info.name.as_ptr())
                        .to_string_lossy()
                        .to_string()
                };

                // Try to get serial number (must be done before close)
                let mut sn: EAFSerialNumber = unsafe { std::mem::zeroed() };
                let serial_number = if unsafe { (sdk.get_serial_number)(id, &mut sn) } == 0 {
                    let sn_bytes: [u8; 8] = sn.id;
                    let sn_str = sn_bytes.iter()
                        .take_while(|&&b| b != 0)
                        .map(|&b| format!("{:02X}", b))
                        .collect::<String>();
                    if sn_str.is_empty() { None } else { Some(sn_str) }
                } else {
                    None
                };

                let _ = unsafe { (sdk.close)(id) };

                tracing::info!("Found ZWO EAF: {} (ID: {}, SN: {:?})", name, id, serial_number);
                focusers.push(ZwoFocuserDiscoveryInfo {
                    focuser_id: id,
                    name,
                    serial_number,
                    discovery_index: i as usize,
                });
            }
        }
    }

    Ok(focusers)
}

// =============================================================================
// ZWO EFW FILTER WHEEL SDK
// =============================================================================

/// EFW Info structure from SDK
#[repr(C)]
#[derive(Debug, Clone)]
struct EFWInfo {
    id: c_int,
    name: [c_char; 64],
    slot_num: c_int,
}

/// EFW Error codes
#[repr(C)]
#[derive(Debug, Clone, Copy, PartialEq)]
#[allow(non_camel_case_types, dead_code)]
enum EFWError {
    EFW_SUCCESS = 0,
    EFW_ERROR_INVALID_INDEX = 1,
    EFW_ERROR_INVALID_ID = 2,
    EFW_ERROR_INVALID_VALUE = 3,
    EFW_ERROR_REMOVED = 4,
    EFW_ERROR_MOVING = 5,
    EFW_ERROR_ERROR_STATE = 6,
    EFW_ERROR_GENERAL_ERROR = 7,
    EFW_ERROR_NOT_SUPPORTED = 8,
    EFW_ERROR_CLOSED = 9,
    EFW_ERROR_END = -1,
}

/// EFW ID/Serial Number structure
#[repr(C)]
#[derive(Debug, Clone)]
struct EFWSerialNumber {
    id: [c_uchar; 8],
}

/// EFW SDK function pointers
struct EfwSdk {
    get_num: unsafe extern "C" fn() -> c_int,
    get_id: unsafe extern "C" fn(c_int, *mut c_int) -> c_int,
    open: unsafe extern "C" fn(c_int) -> c_int,
    close: unsafe extern "C" fn(c_int) -> c_int,
    get_property: unsafe extern "C" fn(c_int, *mut EFWInfo) -> c_int,
    get_position: unsafe extern "C" fn(c_int, *mut c_int) -> c_int,
    set_position: unsafe extern "C" fn(c_int, c_int) -> c_int,
    set_direction: unsafe extern "C" fn(c_int, bool) -> c_int,
    get_direction: unsafe extern "C" fn(c_int, *mut bool) -> c_int,
    calibrate: unsafe extern "C" fn(c_int) -> c_int,
    get_sdk_version: unsafe extern "C" fn() -> *const c_char,
    get_hw_error_code: unsafe extern "C" fn(c_int, *mut c_int) -> c_int,
    get_firmware_version: unsafe extern "C" fn(c_int, *mut c_uchar, *mut c_uchar, *mut c_uchar) -> c_int,
    get_serial_number: unsafe extern "C" fn(c_int, *mut EFWSerialNumber) -> c_int,
    _library: libloading::Library,
}

static EFW_SDK: OnceLock<Option<EfwSdk>> = OnceLock::new();

impl EfwSdk {
    /// Try to load the EFW SDK library
    fn load() -> Option<Self> {
        #[cfg(target_os = "windows")]
        let lib_name = "EFW_filter.dll";
        #[cfg(target_os = "macos")]
        let lib_name = "libEFW_filter.dylib";
        #[cfg(target_os = "linux")]
        let lib_name = "libEFW_filter.so";

        let library = unsafe { libloading::Library::new(lib_name).ok()? };

        unsafe {
            Some(EfwSdk {
                get_num: *library.get(b"EFWGetNum\0").ok()?,
                get_id: *library.get(b"EFWGetID\0").ok()?,
                open: *library.get(b"EFWOpen\0").ok()?,
                close: *library.get(b"EFWClose\0").ok()?,
                get_property: *library.get(b"EFWGetProperty\0").ok()?,
                get_position: *library.get(b"EFWGetPosition\0").ok()?,
                set_position: *library.get(b"EFWSetPosition\0").ok()?,
                set_direction: *library.get(b"EFWSetDirection\0").ok()?,
                get_direction: *library.get(b"EFWGetDirection\0").ok()?,
                calibrate: *library.get(b"EFWCalibrate\0").ok()?,
                get_sdk_version: *library.get(b"EFWGetSDKVersion\0").ok()?,
                get_hw_error_code: *library.get(b"EFWGetHWErrorCode\0").ok()?,
                get_firmware_version: *library.get(b"EFWGetFirmwareVersion\0").ok()?,
                get_serial_number: *library.get(b"EFWGetSerialNumber\0").ok()?,
                _library: library,
            })
        }
    }

    /// Get the singleton SDK instance
    fn get() -> Option<&'static EfwSdk> {
        EFW_SDK.get_or_init(|| Self::load()).as_ref()
    }
}

/// Check EFW error code and convert to NativeError
fn check_efw_error(code: c_int) -> Result<(), NativeError> {
    match code {
        0 => Ok(()),
        1 => Err(NativeError::InvalidDevice("EFW_ERROR_INVALID_INDEX".to_string())),
        2 => Err(NativeError::InvalidDevice("EFW_ERROR_INVALID_ID".to_string())),
        3 => Err(NativeError::InvalidParameter("EFW_ERROR_INVALID_VALUE".to_string())),
        4 => Err(NativeError::Disconnected),
        5 => Err(NativeError::SdkError("EFW_ERROR_MOVING: Filter wheel is moving".to_string())),
        6 => Err(NativeError::SdkError("EFW_ERROR_ERROR_STATE: Filter wheel in error state".to_string())),
        7 => Err(NativeError::SdkError("EFW_ERROR_GENERAL_ERROR".to_string())),
        8 => Err(NativeError::NotSupported),
        9 => Err(NativeError::NotConnected),
        _ => Err(NativeError::SdkError(format!("Unknown EFW error code: {}", code))),
    }
}

// =============================================================================
// ZWO FILTER WHEEL IMPLEMENTATION
// =============================================================================

/// ZWO EFW Filter Wheel implementation
#[derive(Debug)]
pub struct ZwoFilterWheel {
    filterwheel_id: i32,
    device_id: String,
    connected: bool,
    slot_count: i32,
    name: String,
    filter_names: Vec<String>,
}

impl ZwoFilterWheel {
    /// Create a new ZWO filter wheel instance
    pub fn new(filterwheel_id: i32) -> Self {
        Self {
            filterwheel_id,
            device_id: format!("native:zwo:efw:{}", filterwheel_id),
            connected: false,
            slot_count: 0,
            name: format!("ZWO EFW {}", filterwheel_id),
            filter_names: Vec::new(),
        }
    }
}

#[async_trait]
impl NativeDevice for ZwoFilterWheel {
    fn id(&self) -> &str {
        &self.device_id
    }

    fn name(&self) -> &str {
        &self.name
    }

    fn vendor(&self) -> NativeVendor {
        NativeVendor::Zwo
    }

    fn is_connected(&self) -> bool {
        self.connected
    }

    async fn connect(&mut self) -> Result<(), NativeError> {
        tracing::info!("Connecting to ZWO EFW filter wheel ID {}...", self.filterwheel_id);

        let sdk = EfwSdk::get().ok_or_else(|| {
            tracing::error!("Cannot connect to ZWO EFW: EFW SDK not loaded");
            NativeError::SdkNotLoaded
        })?;

        // Open filter wheel
        let result = unsafe { (sdk.open)(self.filterwheel_id) };
        check_efw_error(result)?;

        // Create cleanup guard to close the filter wheel if subsequent operations fail
        let filterwheel_id = self.filterwheel_id;
        let cleanup_guard = CleanupGuard::new(|| {
            tracing::debug!("Cleaning up ZWO EFW filter wheel {} after failed connect", filterwheel_id);
            if let Some(sdk) = EfwSdk::get() {
                let _ = unsafe { (sdk.close)(filterwheel_id) };
            }
        });

        // Get properties
        let mut info: EFWInfo = unsafe { std::mem::zeroed() };
        let result = unsafe { (sdk.get_property)(self.filterwheel_id, &mut info) };
        check_efw_error(result)?;

        self.slot_count = info.slot_num;
        self.name = safe_cstr_to_string(info.name.as_ptr(), 64);

        // Initialize default filter names
        self.filter_names = (0..self.slot_count)
            .map(|i| format!("Filter {}", i + 1))
            .collect();

        // All operations succeeded - defuse the cleanup guard
        cleanup_guard.defuse();

        self.connected = true;
        tracing::info!("Connected to ZWO EFW: {} ({} slots)", self.name, self.slot_count);
        Ok(())
    }

    async fn disconnect(&mut self) -> Result<(), NativeError> {
        if !self.connected {
            return Ok(());
        }

        tracing::info!("Disconnecting from ZWO EFW filter wheel ID {}...", self.filterwheel_id);

        let sdk = EfwSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        let result = unsafe { (sdk.close)(self.filterwheel_id) };
        check_efw_error(result)?;

        self.connected = false;
        tracing::info!("Disconnected from ZWO EFW");
        Ok(())
    }
}

#[async_trait]
impl NativeFilterWheel for ZwoFilterWheel {
    async fn move_to_position(&mut self, position: i32) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = EfwSdk::get().ok_or(NativeError::SdkNotLoaded)?;

        // Validate position
        if position < 0 || position >= self.slot_count {
            return Err(NativeError::InvalidParameter(format!(
                "Invalid position {}. Valid range: 0-{}",
                position,
                self.slot_count - 1
            )));
        }

        tracing::debug!("Moving ZWO EFW to position {}", position);
        let result = unsafe { (sdk.set_position)(self.filterwheel_id, position) };
        check_efw_error(result)
    }

    async fn get_position(&self) -> Result<i32, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = EfwSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        let mut position: c_int = 0;
        let result = unsafe { (sdk.get_position)(self.filterwheel_id, &mut position) };
        check_efw_error(result)?;
        Ok(position)
    }

    async fn is_moving(&self) -> Result<bool, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = EfwSdk::get().ok_or(NativeError::SdkNotLoaded)?;
        let mut position: c_int = 0;
        let result = unsafe { (sdk.get_position)(self.filterwheel_id, &mut position) };
        check_efw_error(result)?;
        // Position is -1 when moving
        Ok(position == -1)
    }

    fn get_filter_count(&self) -> i32 {
        self.slot_count
    }

    async fn get_filter_names(&self) -> Result<Vec<String>, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }
        Ok(self.filter_names.clone())
    }

    async fn set_filter_name(&mut self, position: i32, name: String) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        if position < 0 || position >= self.slot_count {
            return Err(NativeError::InvalidParameter(format!(
                "Invalid position {}. Valid range: 0-{}",
                position,
                self.slot_count - 1
            )));
        }

        self.filter_names[position as usize] = name;
        Ok(())
    }
}

impl ZwoFilterWheel {
    /// Move filter wheel to position and wait for completion with timeout.
    ///
    /// This is a convenience method that combines `move_to_position` with waiting for
    /// the move to complete, with timeout protection.
    ///
    /// # Arguments
    /// * `position` - Target filter slot (0-indexed)
    /// * `config` - Timeout configuration
    ///
    /// # Returns
    /// * `Ok(())` - Move completed successfully
    /// * `Err(NativeError::MoveTimeout)` - Move did not complete within timeout
    pub async fn move_to_position_with_timeout(
        &mut self,
        position: i32,
        config: &NativeTimeoutConfig,
    ) -> Result<(), NativeError> {
        // Start the move
        self.move_to_position(position).await?;

        // Wait for move to complete
        wait_for_filterwheel_move(
            || async { self.is_moving().await },
            config,
            position,
        )
        .await
    }
}

// =============================================================================
// ZWO FILTER WHEEL DISCOVERY
// =============================================================================

/// ZWO filter wheel discovery info
pub struct ZwoFilterWheelDiscoveryInfo {
    pub filterwheel_id: i32,
    pub name: String,
    pub slot_count: i32,
    pub serial_number: Option<String>,
    pub discovery_index: usize,
}

/// Check if EFW SDK is available
pub fn is_efw_sdk_available() -> bool {
    EfwSdk::get().is_some()
}

/// Get EFW SDK status
pub fn get_efw_sdk_status() -> (bool, String) {
    match EfwSdk::get() {
        Some(_) => (true, "ZWO EFW SDK loaded successfully".to_string()),
        None => (false, "ZWO EFW SDK (EFW_filter.dll) not found.".to_string()),
    }
}

/// Discover ZWO EFW filter wheels
pub async fn discover_filter_wheels() -> Result<Vec<ZwoFilterWheelDiscoveryInfo>, NativeError> {
    let sdk = match EfwSdk::get() {
        Some(sdk) => sdk,
        None => {
            tracing::warn!("ZWO EFW discovery skipped: EFW SDK not loaded");
            return Ok(Vec::new());
        }
    };

    tracing::info!("Discovering ZWO EFW filter wheels via native SDK...");
    let num_wheels = unsafe { (sdk.get_num)() };
    tracing::info!("EFW SDK reports {} connected filter wheel(s)", num_wheels);

    let mut wheels = Vec::new();

    for i in 0..num_wheels {
        let mut id: c_int = 0;
        let result = unsafe { (sdk.get_id)(i, &mut id) };

        if result == 0 {
            // Get filter wheel info
            let result = unsafe { (sdk.open)(id) };
            if result == 0 {
                let mut info: EFWInfo = unsafe { std::mem::zeroed() };
                let _ = unsafe { (sdk.get_property)(id, &mut info) };
                let name = unsafe {
                    CStr::from_ptr(info.name.as_ptr())
                        .to_string_lossy()
                        .to_string()
                };
                let slot_count = info.slot_num;

                // Try to get serial number (must be done before close)
                let mut sn: EFWSerialNumber = unsafe { std::mem::zeroed() };
                let serial_number = if unsafe { (sdk.get_serial_number)(id, &mut sn) } == 0 {
                    let sn_bytes: [u8; 8] = sn.id;
                    let sn_str = sn_bytes.iter()
                        .take_while(|&&b| b != 0)
                        .map(|&b| format!("{:02X}", b))
                        .collect::<String>();
                    if sn_str.is_empty() { None } else { Some(sn_str) }
                } else {
                    None
                };

                let _ = unsafe { (sdk.close)(id) };

                tracing::info!("Found ZWO EFW: {} (ID: {}, {} slots, SN: {:?})", name, id, slot_count, serial_number);
                wheels.push(ZwoFilterWheelDiscoveryInfo {
                    filterwheel_id: id,
                    name,
                    slot_count,
                    serial_number,
                    discovery_index: i as usize,
                });
            }
        }
    }

    Ok(wheels)
}