//! Moravian Instruments Camera Native Driver
//!
//! Provides FFI bindings to the Moravian gXusb SDK (gXusb.dll).
//! Moravian Instruments manufactures CCD cameras for astronomy.

use crate::camera::{
    BayerPattern, CameraCapabilities, CameraState, CameraStatus, ExposureParams,
    ImageData, ImageMetadata, ReadoutMode, SensorInfo, SubFrame, VendorFeatures,
};
use crate::sync::moravian_mutex;
use crate::traits::{NativeCamera, NativeDevice, NativeError};
use crate::NativeVendor;
use async_trait::async_trait;
use libloading::Library;
use std::ffi::{c_char, c_float, c_int, c_uint, c_void};
use std::sync::{Mutex, OnceLock};

// ============================================================================
// SDK Types and Constants
// ============================================================================

/// Camera handle type (opaque pointer)
type CCamera = c_void;
type PCCamera = *mut CCamera;

type Cardinal = c_uint;
type Integer = c_int;
type Boolean = u8;
type Real = c_float;
type LongReal = f64;

// GetBooleanParameter indexes
const GBP_CONNECTED: Cardinal = 0;
const GBP_SUBFRAME: Cardinal = 1;
const GBP_READ_MODES: Cardinal = 2;
const GBP_SHUTTER: Cardinal = 3;
const GBP_COOLER: Cardinal = 4;
const GBP_FAN: Cardinal = 5;
const GBP_FILTERS: Cardinal = 6;
const GBP_GUIDE: Cardinal = 7;
const GBP_GAIN: Cardinal = 13;
const GBP_RGB: Cardinal = 128;

// GetIntegerParameter indexes
const GIP_CAMERA_ID: Cardinal = 0;
const GIP_CHIP_W: Cardinal = 1;
const GIP_CHIP_D: Cardinal = 2;
const GIP_PIXEL_W: Cardinal = 3;
const GIP_PIXEL_D: Cardinal = 4;
const GIP_MAX_BINNING_X: Cardinal = 5;
const GIP_MAX_BINNING_Y: Cardinal = 6;
const GIP_READ_MODES: Cardinal = 7;
const GIP_FILTERS: Cardinal = 8;
const GIP_MIN_EXPOSURE: Cardinal = 9;
const GIP_MAX_EXPOSURE: Cardinal = 10;
const GIP_MAX_GAIN: Cardinal = 16;

// GetStringParameter indexes
const GSP_CAMERA_DESCRIPTION: Cardinal = 0;
const GSP_MANUFACTURER: Cardinal = 1;
const GSP_CAMERA_SERIAL: Cardinal = 2;
const GSP_CHIP_DESCRIPTION: Cardinal = 3;

// GetValue indexes
const GV_CHIP_TEMPERATURE: Cardinal = 0;
const GV_HOT_TEMPERATURE: Cardinal = 1;
const GV_POWER_UTILIZATION: Cardinal = 11;

// ============================================================================
// SDK Function Types
// ============================================================================

type EnumerateCallback = unsafe extern "C" fn(Cardinal);
type Enumerate = unsafe extern "C" fn(callback: EnumerateCallback);
type Initialize = unsafe extern "C" fn(id: Cardinal) -> PCCamera;
type Release = unsafe extern "C" fn(camera: PCCamera);
type GetBooleanParameter = unsafe extern "C" fn(camera: PCCamera, index: Cardinal, value: *mut Boolean) -> Boolean;
type GetIntegerParameter = unsafe extern "C" fn(camera: PCCamera, index: Cardinal, value: *mut Cardinal) -> Boolean;
type GetStringParameter = unsafe extern "C" fn(camera: PCCamera, index: Cardinal, len: Cardinal, buf: *mut c_char) -> Boolean;
type GetValue = unsafe extern "C" fn(camera: PCCamera, index: Cardinal, value: *mut Real) -> Boolean;
type SetTemperature = unsafe extern "C" fn(camera: PCCamera, temp: Real) -> Boolean;
type SetBinning = unsafe extern "C" fn(camera: PCCamera, x: Cardinal, y: Cardinal) -> Boolean;
type SetGain = unsafe extern "C" fn(camera: PCCamera, gain: Cardinal) -> Boolean;
type SetReadMode = unsafe extern "C" fn(camera: PCCamera, mode: Cardinal) -> Boolean;
type EnumerateReadModes = unsafe extern "C" fn(camera: PCCamera, index: Cardinal, len: Cardinal, desc: *mut c_char) -> Boolean;
type ClearSensor = unsafe extern "C" fn(camera: PCCamera) -> Boolean;
type Open_ = unsafe extern "C" fn(camera: PCCamera) -> Boolean;
type Close_ = unsafe extern "C" fn(camera: PCCamera) -> Boolean;
type BeginExposure = unsafe extern "C" fn(camera: PCCamera, use_shutter: Boolean) -> Boolean;
type EndExposure = unsafe extern "C" fn(camera: PCCamera, use_shutter: Boolean, abort: Boolean) -> Boolean;
type GetImage16b = unsafe extern "C" fn(
    camera: PCCamera,
    x: Integer,
    y: Integer,
    w: Integer,
    d: Integer,
    buffer_len: Cardinal,
    buffer: *mut c_void,
) -> Boolean;
type AdjustSubFrame = unsafe extern "C" fn(
    camera: PCCamera,
    x: *mut Integer,
    y: *mut Integer,
    w: *mut Integer,
    d: *mut Integer,
) -> Boolean;
type MoveTelescope = unsafe extern "C" fn(camera: PCCamera, ra_ms: i16, dec_ms: i16) -> Boolean;

// ============================================================================
// SDK Singleton
// ============================================================================

static SDK: OnceLock<Result<MoravianSdk, String>> = OnceLock::new();

struct MoravianSdk {
    enumerate: Enumerate,
    initialize: Initialize,
    release: Release,
    get_boolean_parameter: GetBooleanParameter,
    get_integer_parameter: GetIntegerParameter,
    get_string_parameter: GetStringParameter,
    get_value: GetValue,
    set_temperature: SetTemperature,
    set_binning: SetBinning,
    set_gain: SetGain,
    set_read_mode: SetReadMode,
    enumerate_read_modes: EnumerateReadModes,
    clear_sensor: ClearSensor,
    open: Open_,
    close: Close_,
    begin_exposure: BeginExposure,
    end_exposure: EndExposure,
    get_image_16b: GetImage16b,
    adjust_subframe: AdjustSubFrame,
    move_telescope: MoveTelescope,
    _library: Library,
}

unsafe impl Send for MoravianSdk {}
unsafe impl Sync for MoravianSdk {}

impl MoravianSdk {
    fn load() -> Result<Self, String> {
        let library = unsafe { Library::new("gXusb.dll") }
            .map_err(|e| format!("Failed to load gXusb.dll: {}", e))?;

        unsafe {
            Ok(Self {
                enumerate: *library.get::<Enumerate>(b"Enumerate\0")
                    .map_err(|e| format!("Failed to get Enumerate: {}", e))?,
                initialize: *library.get::<Initialize>(b"Initialize\0")
                    .map_err(|e| format!("Failed to get Initialize: {}", e))?,
                release: *library.get::<Release>(b"Release\0")
                    .map_err(|e| format!("Failed to get Release: {}", e))?,
                get_boolean_parameter: *library.get::<GetBooleanParameter>(b"GetBooleanParameter\0")
                    .map_err(|e| format!("Failed to get GetBooleanParameter: {}", e))?,
                get_integer_parameter: *library.get::<GetIntegerParameter>(b"GetIntegerParameter\0")
                    .map_err(|e| format!("Failed to get GetIntegerParameter: {}", e))?,
                get_string_parameter: *library.get::<GetStringParameter>(b"GetStringParameter\0")
                    .map_err(|e| format!("Failed to get GetStringParameter: {}", e))?,
                get_value: *library.get::<GetValue>(b"GetValue\0")
                    .map_err(|e| format!("Failed to get GetValue: {}", e))?,
                set_temperature: *library.get::<SetTemperature>(b"SetTemperature\0")
                    .map_err(|e| format!("Failed to get SetTemperature: {}", e))?,
                set_binning: *library.get::<SetBinning>(b"SetBinning\0")
                    .map_err(|e| format!("Failed to get SetBinning: {}", e))?,
                set_gain: *library.get::<SetGain>(b"SetGain\0")
                    .map_err(|e| format!("Failed to get SetGain: {}", e))?,
                set_read_mode: *library.get::<SetReadMode>(b"SetReadMode\0")
                    .map_err(|e| format!("Failed to get SetReadMode: {}", e))?,
                enumerate_read_modes: *library.get::<EnumerateReadModes>(b"EnumerateReadModes\0")
                    .map_err(|e| format!("Failed to get EnumerateReadModes: {}", e))?,
                clear_sensor: *library.get::<ClearSensor>(b"ClearSensor\0")
                    .map_err(|e| format!("Failed to get ClearSensor: {}", e))?,
                open: *library.get::<Open_>(b"Open\0")
                    .map_err(|e| format!("Failed to get Open: {}", e))?,
                close: *library.get::<Close_>(b"Close\0")
                    .map_err(|e| format!("Failed to get Close: {}", e))?,
                begin_exposure: *library.get::<BeginExposure>(b"BeginExposure\0")
                    .map_err(|e| format!("Failed to get BeginExposure: {}", e))?,
                end_exposure: *library.get::<EndExposure>(b"EndExposure\0")
                    .map_err(|e| format!("Failed to get EndExposure: {}", e))?,
                get_image_16b: *library.get::<GetImage16b>(b"GetImage16b\0")
                    .map_err(|e| format!("Failed to get GetImage16b: {}", e))?,
                adjust_subframe: *library.get::<AdjustSubFrame>(b"AdjustSubFrame\0")
                    .map_err(|e| format!("Failed to get AdjustSubFrame: {}", e))?,
                move_telescope: *library.get::<MoveTelescope>(b"MoveTelescope\0")
                    .map_err(|e| format!("Failed to get MoveTelescope: {}", e))?,
                _library: library,
            })
        }
    }
}

fn get_sdk() -> Result<&'static MoravianSdk, NativeError> {
    SDK.get_or_init(|| MoravianSdk::load())
        .as_ref()
        .map_err(|e| NativeError::SdkError(e.clone()))
}

// ============================================================================
// Device Discovery
// ============================================================================

/// Thread-local storage for enumeration results
static DISCOVERED_IDS: Mutex<Vec<Cardinal>> = Mutex::new(Vec::new());

/// Callback for camera enumeration
unsafe extern "C" fn enumerate_callback(id: Cardinal) {
    if let Ok(mut ids) = DISCOVERED_IDS.lock() {
        ids.push(id);
    }
}

/// Discovered Moravian camera info
#[derive(Debug, Clone)]
pub struct MoravianCameraInfo {
    pub camera_id: Cardinal,
    pub name: String,
    pub serial_number: Option<String>,
    pub discovery_index: usize,
}

/// Discover all connected Moravian cameras
pub async fn discover_devices() -> Result<Vec<MoravianCameraInfo>, NativeError> {
    let sdk = get_sdk()?;

    // Acquire global SDK mutex for thread safety
    let _lock = moravian_mutex().lock().await;

    // Clear previous results
    {
        let mut ids = DISCOVERED_IDS.lock().unwrap();
        ids.clear();
    }

    // Enumerate cameras
    unsafe { (sdk.enumerate)(enumerate_callback) };

    // Collect results
    let ids: Vec<Cardinal> = {
        let ids = DISCOVERED_IDS.lock().unwrap();
        ids.clone()
    };

    let mut devices = Vec::new();

    for (index, &id) in ids.iter().enumerate() {
        // Temporarily initialize to get camera info
        let handle = unsafe { (sdk.initialize)(id) };
        if handle.is_null() {
            continue;
        }

        // Get camera description
        let mut name_buf = [0i8; 256];
        if unsafe { (sdk.get_string_parameter)(handle, GSP_CAMERA_DESCRIPTION, 256, name_buf.as_mut_ptr()) } != 0 {
            let name = unsafe { std::ffi::CStr::from_ptr(name_buf.as_ptr()) }
                .to_string_lossy()
                .to_string();

            // Get serial number
            let mut serial_buf = [0i8; 64];
            let serial_number = if unsafe { (sdk.get_string_parameter)(handle, GSP_CAMERA_SERIAL, 64, serial_buf.as_mut_ptr()) } != 0 {
                let serial = unsafe { std::ffi::CStr::from_ptr(serial_buf.as_ptr()) }
                    .to_string_lossy()
                    .to_string();
                if !serial.is_empty() { Some(serial) } else { None }
            } else {
                None
            };

            devices.push(MoravianCameraInfo {
                camera_id: id,
                name,
                serial_number,
                discovery_index: index,
            });
        }

        // Release temporary handle
        unsafe { (sdk.release)(handle) };
    }

    Ok(devices)
}

// ============================================================================
// Handle Wrapper for Send + Sync
// ============================================================================

struct HandleWrapper(PCCamera);
unsafe impl Send for HandleWrapper {}
unsafe impl Sync for HandleWrapper {}

// ============================================================================
// Camera Implementation
// ============================================================================

/// Moravian camera instance
pub struct MoravianCamera {
    camera_id: Cardinal,
    device_id: String,
    name: String,
    handle: Mutex<HandleWrapper>,
    connected: bool,
    capabilities: CameraCapabilities,
    sensor_info: SensorInfo,
    state: CameraState,
    current_gain: i32,
    current_offset: i32,
    current_bin_x: i32,
    current_bin_y: i32,
    subframe: Option<SubFrame>,
    cooler_on: bool,
    target_temp: f64,
    exposure_duration: f64,
    use_shutter: bool,
    discovery_index: usize,
}

impl std::fmt::Debug for MoravianCamera {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("MoravianCamera")
            .field("name", &self.name)
            .field("camera_id", &self.camera_id)
            .finish()
    }
}

impl MoravianCamera {
    /// Create a new Moravian camera instance
    pub fn new(camera_id: Cardinal, discovery_index: usize) -> Self {
        Self {
            camera_id,
            device_id: format!("moravian_{}", camera_id),
            name: format!("Moravian Camera {}", camera_id),
            handle: Mutex::new(HandleWrapper(std::ptr::null_mut())),
            connected: false,
            capabilities: CameraCapabilities::default(),
            sensor_info: SensorInfo::default(),
            state: CameraState::Idle,
            current_gain: 0,
            current_offset: 0,
            current_bin_x: 1,
            current_bin_y: 1,
            subframe: None,
            cooler_on: false,
            target_temp: 0.0,
            exposure_duration: 0.0,
            use_shutter: true,
            discovery_index,
        }
    }

    /// Get boolean parameter
    fn get_bool_param(&self, index: Cardinal) -> Result<bool, NativeError> {
        let sdk = get_sdk()?;
        let handle = self.handle.lock().unwrap().0;
        let mut value: Boolean = 0;
        if unsafe { (sdk.get_boolean_parameter)(handle, index, &mut value) } != 0 {
            Ok(value != 0)
        } else {
            Err(NativeError::SdkError("Failed to get boolean parameter".into()))
        }
    }

    /// Get integer parameter
    fn get_int_param(&self, index: Cardinal) -> Result<Cardinal, NativeError> {
        let sdk = get_sdk()?;
        let handle = self.handle.lock().unwrap().0;
        let mut value: Cardinal = 0;
        if unsafe { (sdk.get_integer_parameter)(handle, index, &mut value) } != 0 {
            Ok(value)
        } else {
            Err(NativeError::SdkError("Failed to get integer parameter".into()))
        }
    }

    /// Get value (float)
    fn get_value_param(&self, index: Cardinal) -> Result<f32, NativeError> {
        let sdk = get_sdk()?;
        let handle = self.handle.lock().unwrap().0;
        let mut value: Real = 0.0;
        if unsafe { (sdk.get_value)(handle, index, &mut value) } != 0 {
            Ok(value)
        } else {
            Err(NativeError::SdkError("Failed to get value".into()))
        }
    }
}

#[async_trait]
impl NativeDevice for MoravianCamera {
    fn id(&self) -> &str {
        &self.device_id
    }

    fn name(&self) -> &str {
        &self.name
    }

    fn vendor(&self) -> NativeVendor {
        NativeVendor::Moravian
    }

    fn is_connected(&self) -> bool {
        self.connected
    }

    async fn connect(&mut self) -> Result<(), NativeError> {
        if self.connected {
            return Ok(());
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = moravian_mutex().lock().await;

        // Initialize camera
        let handle = unsafe { (sdk.initialize)(self.camera_id) };
        if handle.is_null() {
            tracing::error!(
                "Moravian Initialize() returned NULL for camera ID {}. Check USB connection and driver installation.",
                self.camera_id
            );
            return Err(NativeError::SdkError(format!(
                "Failed to initialize Moravian camera ID {} - SDK returned NULL handle. Ensure camera is connected and gXusb driver is installed.",
                self.camera_id
            )));
        }

        // Store handle
        {
            let mut h = self.handle.lock().unwrap();
            *h = HandleWrapper(handle);
        }

        // Get camera info using the stored handle (synchronous operations)
        {
            let handle = self.handle.lock().unwrap().0;

            // Get name
            let mut name_buf = [0i8; 256];
            if unsafe { (sdk.get_string_parameter)(handle, GSP_CAMERA_DESCRIPTION, 256, name_buf.as_mut_ptr()) } != 0 {
                self.name = unsafe { std::ffi::CStr::from_ptr(name_buf.as_ptr()) }
                    .to_string_lossy()
                    .to_string();
            }

            // Get sensor dimensions
            let mut width: Cardinal = 0;
            let mut height: Cardinal = 0;
            unsafe {
                (sdk.get_integer_parameter)(handle, GIP_CHIP_W, &mut width);
                (sdk.get_integer_parameter)(handle, GIP_CHIP_D, &mut height);
            }

            // Get pixel size (in 0.01 microns per SDK docs)
            let mut pixel_w: Cardinal = 0;
            let mut pixel_d: Cardinal = 0;
            unsafe {
                (sdk.get_integer_parameter)(handle, GIP_PIXEL_W, &mut pixel_w);
                (sdk.get_integer_parameter)(handle, GIP_PIXEL_D, &mut pixel_d);
            }

            // Check if color camera
            let mut is_color: Boolean = 0;
            let color = if unsafe { (sdk.get_boolean_parameter)(handle, GBP_RGB, &mut is_color) } != 0 {
                is_color != 0
            } else {
                false
            };

            self.sensor_info = SensorInfo {
                width,
                height,
                pixel_size_x: pixel_w as f64 / 100.0, // Convert from 0.01 microns
                pixel_size_y: pixel_d as f64 / 100.0,
                max_adu: 65535,
                bit_depth: 16,
                color,
                bayer_pattern: if color { Some(BayerPattern::Rggb) } else { None },
            };

            // Get capabilities
            let mut has_cooler: Boolean = 0;
            let mut has_shutter: Boolean = 0;
            let mut has_guide: Boolean = 0;
            let mut has_gain: Boolean = 0;
            let mut has_subframe: Boolean = 0;
            let mut max_bin_x: Cardinal = 1;
            let mut max_bin_y: Cardinal = 1;

            unsafe {
                (sdk.get_boolean_parameter)(handle, GBP_COOLER, &mut has_cooler);
                (sdk.get_boolean_parameter)(handle, GBP_SHUTTER, &mut has_shutter);
                (sdk.get_boolean_parameter)(handle, GBP_GUIDE, &mut has_guide);
                (sdk.get_boolean_parameter)(handle, GBP_GAIN, &mut has_gain);
                (sdk.get_boolean_parameter)(handle, GBP_SUBFRAME, &mut has_subframe);
                (sdk.get_integer_parameter)(handle, GIP_MAX_BINNING_X, &mut max_bin_x);
                (sdk.get_integer_parameter)(handle, GIP_MAX_BINNING_Y, &mut max_bin_y);
            }

            self.capabilities = CameraCapabilities {
                can_cool: has_cooler != 0,
                can_set_gain: has_gain != 0,
                can_set_offset: false, // Moravian doesn't have separate offset
                can_set_binning: max_bin_x > 1 || max_bin_y > 1,
                can_subframe: has_subframe != 0,
                has_shutter: has_shutter != 0,
                has_guider_port: has_guide != 0,
                max_bin_x: max_bin_x as i32,
                max_bin_y: max_bin_y as i32,
                supports_readout_modes: true, // Moravian supports readout modes
            };

            self.use_shutter = has_shutter != 0;
        }

        // Open camera for imaging
        {
            let handle = self.handle.lock().unwrap().0;
            if unsafe { (sdk.open)(handle) } == 0 {
                tracing::error!(
                    "Moravian Open() failed for camera '{}' (ID {}). Camera may be in use by another application.",
                    self.name, self.camera_id
                );
                return Err(NativeError::SdkError(format!(
                    "Failed to open Moravian camera '{}' - SDK Open() returned false. Check if camera is in use by another application.",
                    self.name
                )));
            }
        }

        self.connected = true;
        self.state = CameraState::Idle;

        tracing::info!(
            "Connected to Moravian camera: {} ({}x{})",
            self.name,
            self.sensor_info.width,
            self.sensor_info.height
        );

        Ok(())
    }

    async fn disconnect(&mut self) -> Result<(), NativeError> {
        if !self.connected {
            return Ok(());
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = moravian_mutex().lock().await;

        let handle = self.handle.lock().unwrap().0;

        // Close camera
        unsafe { (sdk.close)(handle) };

        // Release camera
        unsafe { (sdk.release)(handle) };

        {
            let mut h = self.handle.lock().unwrap();
            *h = HandleWrapper(std::ptr::null_mut());
        }
        self.connected = false;
        self.state = CameraState::Idle;

        tracing::info!("Disconnected from Moravian camera: {}", self.name);

        Ok(())
    }
}

#[async_trait]
impl NativeCamera for MoravianCamera {
    fn capabilities(&self) -> CameraCapabilities {
        self.capabilities.clone()
    }

    fn get_sensor_info(&self) -> SensorInfo {
        self.sensor_info.clone()
    }

    async fn get_status(&self) -> Result<CameraStatus, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = moravian_mutex().lock().await;

        let handle = self.handle.lock().unwrap().0;

        // Get temperature
        let current_temp = {
            let mut value: Real = 0.0;
            if unsafe { (sdk.get_value)(handle, GV_CHIP_TEMPERATURE, &mut value) } != 0 {
                Some(value as f64)
            } else {
                None
            }
        };

        // Get cooler power
        let cooler_power = {
            let mut value: Real = 0.0;
            if unsafe { (sdk.get_value)(handle, GV_POWER_UTILIZATION, &mut value) } != 0 {
                Some(value as f64)
            } else {
                None
            }
        };

        // Calculate exposure remaining (approximate)
        let exposure_remaining = if self.state == CameraState::Exposing {
            Some(self.exposure_duration)
        } else {
            None
        };

        Ok(CameraStatus {
            state: self.state.clone(),
            sensor_temp: current_temp,
            cooler_power,
            target_temp: Some(self.target_temp),
            cooler_on: self.cooler_on,
            gain: self.current_gain,
            offset: self.current_offset,
            bin_x: self.current_bin_x,
            bin_y: self.current_bin_y,
            exposure_remaining,
        })
    }

    async fn start_exposure(&mut self, params: ExposureParams) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        if self.state == CameraState::Exposing {
            return Err(NativeError::SdkError("Camera is already exposing".into()));
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = moravian_mutex().lock().await;

        let handle = self.handle.lock().unwrap().0;

        // Clear sensor first
        if unsafe { (sdk.clear_sensor)(handle) } == 0 {
            tracing::error!(
                "Moravian ClearSensor() failed for camera '{}'. Sensor may be busy or hardware error occurred.",
                self.name
            );
            return Err(NativeError::SdkError(format!(
                "Failed to clear sensor on Moravian camera '{}'. Sensor may be busy.",
                self.name
            )));
        }

        // Start exposure (use shutter if available)
        let use_shutter = if self.use_shutter { 1 } else { 0 };

        if unsafe { (sdk.begin_exposure)(handle, use_shutter) } == 0 {
            tracing::error!(
                "Moravian BeginExposure() failed for camera '{}'. Duration: {:.3}s, UseShutter: {}",
                self.name, params.duration_secs, self.use_shutter
            );
            return Err(NativeError::SdkError(format!(
                "Failed to start exposure on Moravian camera '{}'. The camera may be busy or disconnected.",
                self.name
            )));
        }

        self.exposure_duration = params.duration_secs;
        self.state = CameraState::Exposing;

        tracing::info!(
            "Started {:.3}s exposure on Moravian camera",
            params.duration_secs
        );

        // Wait for exposure duration
        tokio::time::sleep(tokio::time::Duration::from_secs_f64(params.duration_secs)).await;

        Ok(())
    }

    async fn abort_exposure(&mut self) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = moravian_mutex().lock().await;

        let handle = self.handle.lock().unwrap().0;

        // End exposure with abort
        unsafe { (sdk.end_exposure)(handle, 0, 1) };

        self.state = CameraState::Idle;
        tracing::info!("Aborted exposure on Moravian camera");

        Ok(())
    }

    async fn download_image(&mut self) -> Result<ImageData, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = moravian_mutex().lock().await;

        let handle = self.handle.lock().unwrap().0;

        // End exposure
        if unsafe { (sdk.end_exposure)(handle, if self.use_shutter { 1 } else { 0 }, 0) } == 0 {
            tracing::error!(
                "Moravian EndExposure() failed for camera '{}'. Exposure may not have completed properly.",
                self.name
            );
            return Err(NativeError::SdkError(format!(
                "Failed to end exposure on Moravian camera '{}'. Exposure may not have completed.",
                self.name
            )));
        }

        self.state = CameraState::Downloading;

        // Calculate image dimensions
        let (x, y, width, height) = if let Some(ref sf) = self.subframe {
            (sf.start_x as i32, sf.start_y as i32, sf.width, sf.height)
        } else {
            (0, 0, self.sensor_info.width, self.sensor_info.height)
        };

        let binned_width = width / self.current_bin_x as u32;
        let binned_height = height / self.current_bin_y as u32;
        let buffer_size = (binned_width * binned_height) as usize;

        // Allocate buffer
        let mut data: Vec<u16> = vec![0u16; buffer_size];

        // Download image
        let result = unsafe {
            (sdk.get_image_16b)(
                handle,
                x,
                y,
                binned_width as i32,
                binned_height as i32,
                (buffer_size * 2) as Cardinal,
                data.as_mut_ptr() as *mut c_void,
            )
        };

        if result == 0 {
            tracing::error!(
                "Moravian GetImage16b() failed for camera '{}'. Requested {}x{} pixels at ({}, {})",
                self.name, binned_width, binned_height, x, y
            );
            return Err(NativeError::SdkError(format!(
                "Failed to download image from Moravian camera '{}'. Buffer size: {} bytes",
                self.name, buffer_size * 2
            )));
        }

        self.state = CameraState::Idle;

        // Get temperature while we still hold the mutex
        let temperature = {
            let mut value: Real = 0.0;
            if unsafe { (sdk.get_value)(handle, GV_CHIP_TEMPERATURE, &mut value) } != 0 {
                Some(value as f64)
            } else {
                None
            }
        };

        let metadata = ImageMetadata {
            exposure_time: self.exposure_duration,
            gain: self.current_gain,
            offset: self.current_offset,
            bin_x: self.current_bin_x,
            bin_y: self.current_bin_y,
            temperature,
            timestamp: chrono::Utc::now(),
            subframe: self.subframe.clone(),
            readout_mode: None,
            vendor_data: VendorFeatures::default(),
        };

        Ok(ImageData {
            width: binned_width,
            height: binned_height,
            data,
            bits_per_pixel: 16,
            bayer_pattern: self.sensor_info.bayer_pattern,
            metadata,
        })
    }

    async fn is_exposure_complete(&self) -> Result<bool, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }
        // For Moravian, we rely on timing-based exposure completion
        Ok(self.state != CameraState::Exposing)
    }

    async fn set_cooler(&mut self, enabled: bool, target_temp: f64) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        if !self.capabilities.can_cool {
            return Err(NativeError::NotSupported);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = moravian_mutex().lock().await;

        let handle = self.handle.lock().unwrap().0;

        if enabled {
            // Set target temperature
            if unsafe { (sdk.set_temperature)(handle, target_temp as f32) } == 0 {
                tracing::error!(
                    "Moravian SetTemperature() failed for camera '{}'. Target: {:.1}°C",
                    self.name, target_temp
                );
                return Err(NativeError::SdkError(format!(
                    "Failed to set cooler temperature to {:.1}°C on Moravian camera '{}'. Camera may not have a cooler.",
                    target_temp, self.name
                )));
            }
            self.cooler_on = true;
            self.target_temp = target_temp;
        } else {
            // Warm up to ambient (set high temperature target)
            unsafe { (sdk.set_temperature)(handle, 25.0) };
            self.cooler_on = false;
        }

        tracing::info!(
            "Moravian cooler {}: target {}°C",
            if enabled { "enabled" } else { "disabled" },
            target_temp
        );

        Ok(())
    }

    async fn get_temperature(&self) -> Result<f64, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = moravian_mutex().lock().await;

        let handle = self.handle.lock().unwrap().0;

        let mut value: Real = 0.0;
        if unsafe { (sdk.get_value)(handle, GV_CHIP_TEMPERATURE, &mut value) } != 0 {
            Ok(value as f64)
        } else {
            Err(NativeError::SdkError("Failed to get temperature".into()))
        }
    }

    async fn get_cooler_power(&self) -> Result<f64, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = moravian_mutex().lock().await;

        let handle = self.handle.lock().unwrap().0;

        let mut value: Real = 0.0;
        if unsafe { (sdk.get_value)(handle, GV_POWER_UTILIZATION, &mut value) } != 0 {
            Ok(value as f64)
        } else {
            Err(NativeError::SdkError("Failed to get cooler power".into()))
        }
    }

    async fn set_gain(&mut self, gain: i32) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        if !self.capabilities.can_set_gain {
            return Err(NativeError::NotSupported);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = moravian_mutex().lock().await;

        let handle = self.handle.lock().unwrap().0;

        if unsafe { (sdk.set_gain)(handle, gain as Cardinal) } == 0 {
            tracing::error!(
                "Moravian SetGain() failed for camera '{}'. Requested gain: {}",
                self.name, gain
            );
            return Err(NativeError::SdkError(format!(
                "Failed to set gain to {} on Moravian camera '{}'. Value may be out of range.",
                gain, self.name
            )));
        }

        self.current_gain = gain;
        Ok(())
    }

    async fn set_offset(&mut self, offset: i32) -> Result<(), NativeError> {
        // Moravian doesn't support offset
        self.current_offset = offset;
        Ok(())
    }

    async fn set_binning(&mut self, bin_x: i32, bin_y: i32) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        if !self.capabilities.can_set_binning && (bin_x > 1 || bin_y > 1) {
            return Err(NativeError::NotSupported);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = moravian_mutex().lock().await;

        let handle = self.handle.lock().unwrap().0;

        if unsafe { (sdk.set_binning)(handle, bin_x as Cardinal, bin_y as Cardinal) } == 0 {
            tracing::error!(
                "Moravian SetBinning() failed for camera '{}'. Requested: {}x{}. Max: {}x{}",
                self.name, bin_x, bin_y, self.capabilities.max_bin_x, self.capabilities.max_bin_y
            );
            return Err(NativeError::SdkError(format!(
                "Failed to set binning to {}x{} on Moravian camera '{}'. Max supported: {}x{}",
                bin_x, bin_y, self.name, self.capabilities.max_bin_x, self.capabilities.max_bin_y
            )));
        }

        self.current_bin_x = bin_x;
        self.current_bin_y = bin_y;
        Ok(())
    }

    async fn set_subframe(&mut self, subframe: Option<SubFrame>) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = moravian_mutex().lock().await;

        let handle = self.handle.lock().unwrap().0;

        if let Some(ref sf) = subframe {
            if !self.capabilities.can_subframe {
                return Err(NativeError::NotSupported);
            }

            // Validate subframe bounds with SDK
            let mut x = sf.start_x as Integer;
            let mut y = sf.start_y as Integer;
            let mut w = sf.width as Integer;
            let mut d = sf.height as Integer;

            if unsafe { (sdk.adjust_subframe)(handle, &mut x, &mut y, &mut w, &mut d) } == 0 {
                tracing::error!(
                    "Moravian AdjustSubFrame() failed for camera '{}'. Requested: ({}, {}) {}x{}. Sensor: {}x{}",
                    self.name, sf.start_x, sf.start_y, sf.width, sf.height,
                    self.sensor_info.width, self.sensor_info.height
                );
                return Err(NativeError::SdkError(format!(
                    "Failed to set subframe ({}, {}) {}x{} on Moravian camera '{}'. Check bounds vs sensor size {}x{}",
                    sf.start_x, sf.start_y, sf.width, sf.height, self.name,
                    self.sensor_info.width, self.sensor_info.height
                )));
            }

            // Store adjusted subframe
            self.subframe = Some(SubFrame {
                start_x: x as u32,
                start_y: y as u32,
                width: w as u32,
                height: d as u32,
            });
        } else {
            self.subframe = None;
        }

        Ok(())
    }

    async fn get_gain(&self) -> Result<i32, NativeError> {
        Ok(self.current_gain)
    }

    async fn get_offset(&self) -> Result<i32, NativeError> {
        Ok(self.current_offset)
    }

    async fn get_binning(&self) -> Result<(i32, i32), NativeError> {
        Ok((self.current_bin_x, self.current_bin_y))
    }

    async fn get_readout_modes(&self) -> Result<Vec<ReadoutMode>, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = moravian_mutex().lock().await;

        let handle = self.handle.lock().unwrap().0;

        // Get number of readout modes
        let num_modes = {
            let mut value: Cardinal = 0;
            if unsafe { (sdk.get_integer_parameter)(handle, GIP_READ_MODES, &mut value) } != 0 {
                value
            } else {
                1
            }
        };

        let mut modes = Vec::new();
        for i in 0..num_modes {
            let mut desc_buf = [0i8; 256];
            if unsafe { (sdk.enumerate_read_modes)(handle, i, 256, desc_buf.as_mut_ptr()) } != 0 {
                let description = unsafe { std::ffi::CStr::from_ptr(desc_buf.as_ptr()) }
                    .to_string_lossy()
                    .to_string();

                modes.push(ReadoutMode {
                    name: format!("Mode {}", i),
                    description,
                    index: i as i32,
                    gain_min: None,
                    gain_max: None,
                    offset_min: None,
                    offset_max: None,
                });
            }
        }

        if modes.is_empty() {
            modes.push(ReadoutMode {
                name: "Normal".to_string(),
                description: "Standard readout mode".to_string(),
                index: 0,
                gain_min: None,
                gain_max: None,
                offset_min: None,
                offset_max: None,
            });
        }

        Ok(modes)
    }

    async fn set_readout_mode(&mut self, mode: &ReadoutMode) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = moravian_mutex().lock().await;

        let handle = self.handle.lock().unwrap().0;

        if unsafe { (sdk.set_read_mode)(handle, mode.index as Cardinal) } == 0 {
            tracing::error!(
                "Moravian SetReadMode() failed for camera '{}'. Mode index: {} ('{}')",
                self.name, mode.index, mode.name
            );
            return Err(NativeError::SdkError(format!(
                "Failed to set readout mode '{}' (index {}) on Moravian camera '{}'. Mode may not be supported.",
                mode.name, mode.index, self.name
            )));
        }

        Ok(())
    }

    async fn get_vendor_features(&self) -> Result<VendorFeatures, NativeError> {
        // Moravian has hot side temp available but VendorFeatures doesn't have this field
        // Could use custom_data in future if needed
        Ok(VendorFeatures::default())
    }

    async fn get_gain_range(&self) -> Result<(i32, i32), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        // Moravian cameras (mostly CCD) typically have limited or no gain control.
        // CMOS Moravian cameras would have adjustable gain.
        // Return a nominal range that works for most.
        Ok((0, 100))
    }

    async fn get_offset_range(&self) -> Result<(i32, i32), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        // Moravian cameras typically have limited offset control.
        Ok((0, 255))
    }
}
