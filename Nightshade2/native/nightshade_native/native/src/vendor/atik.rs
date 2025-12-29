//! Atik Camera SDK Bindings
//!
//! Native driver for Atik cameras using their official Artemis SDK.
//! Supports Atik Horizon, ACIS, APX, and older series cameras.

use crate::camera::{
    BayerPattern, CameraCapabilities, CameraState, CameraStatus, ExposureParams,
    ImageData, ImageMetadata, ReadoutMode, SensorInfo, SubFrame, VendorFeatures,
};
use crate::sync::atik_mutex;
use crate::traits::{NativeCamera, NativeDevice, NativeError};
use crate::NativeVendor;
use async_trait::async_trait;
use std::ffi::{c_char, c_float, c_int, c_void, CStr};
use std::sync::{Mutex, OnceLock};

// =============================================================================
// Atik SDK Types (from AtikDefs.h and AtikCameras.h)
// =============================================================================

/// Atik SDK handle type
type ArtemisHandle = *mut c_void;

/// Wrapper to make raw pointer Send + Sync
/// SAFETY: The Atik SDK requires that all calls to a given camera handle
/// be serialized, which we ensure through the Mutex wrapper in AtikCamera.
struct HandleWrapper(ArtemisHandle);
unsafe impl Send for HandleWrapper {}
unsafe impl Sync for HandleWrapper {}

/// Atik error codes
#[repr(i32)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
enum ArtemisError {
    Ok = 0,
    InvalidParameter = 1,
    NotConnected = 2,
    NotImplemented = 3,
    NoResponse = 4,
    InvalidFunction = 5,
    NotInitialized = 6,
    OperationFailed = 7,
    InvalidPassword = 8,
}

impl ArtemisError {
    fn from_i32(code: i32) -> Self {
        match code {
            0 => ArtemisError::Ok,
            1 => ArtemisError::InvalidParameter,
            2 => ArtemisError::NotConnected,
            3 => ArtemisError::NotImplemented,
            4 => ArtemisError::NoResponse,
            5 => ArtemisError::InvalidFunction,
            6 => ArtemisError::NotInitialized,
            7 => ArtemisError::OperationFailed,
            8 => ArtemisError::InvalidPassword,
            _ => ArtemisError::OperationFailed,
        }
    }

    fn to_native_error(self, msg: &str) -> NativeError {
        tracing::error!(
            "Atik SDK error during '{}': {:?}. Check camera connection and SDK installation.",
            msg, self
        );
        NativeError::SdkError(format!(
            "Atik {}: {:?}. Ensure camera is connected and AtikCameras driver is installed.",
            msg, self
        ))
    }
}

/// Camera colour type
#[repr(i32)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
enum ArtemisColourType {
    Unknown = 0,
    None = 1,
    Rggb = 2,
}

/// Camera state
#[repr(i32)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
enum ArtemisCameraState {
    Error = -1,
    Idle = 0,
    Waiting = 1,
    Exposing = 2,
    Reading = 3,
    Downloading = 4,
    Flushing = 5,
    ExternalTrigger = 6,
}

/// Camera properties structure (ARTEMISPROPERTIES)
#[repr(C)]
#[derive(Debug)]
struct ArtemisProperties {
    protocol: c_int,
    pixels_x: c_int,
    pixels_y: c_int,
    pixel_microns_x: c_float,
    pixel_microns_y: c_float,
    ccd_flags: c_int,
    camera_flags: c_int,
    description: [c_char; 40],
    manufacturer: [c_char; 40],
}

// Camera flags from ARTEMISPROPERTIESCAMERAFLAGS
const ARTEMIS_CAMERA_HAS_SHUTTER: c_int = 16;
const ARTEMIS_CAMERA_HAS_GUIDE_PORT: c_int = 32;
const ARTEMIS_CAMERA_HAS_FILTERWHEEL: c_int = 1024;

// =============================================================================
// SDK Function Pointers
// =============================================================================

type ArtemisDeviceCount = unsafe extern "C" fn() -> c_int;
type ArtemisDevicePresent = unsafe extern "C" fn(device: c_int) -> c_int;
type ArtemisDeviceName = unsafe extern "C" fn(device: c_int, name: *mut c_char) -> c_int;
type ArtemisDeviceSerial = unsafe extern "C" fn(device: c_int, serial: *mut c_char) -> c_int;
type ArtemisDeviceIsCamera = unsafe extern "C" fn(device: c_int) -> c_int;
type ArtemisConnect = unsafe extern "C" fn(device: c_int) -> ArtemisHandle;
type ArtemisDisconnect = unsafe extern "C" fn(handle: ArtemisHandle) -> c_int;
type ArtemisIsConnected = unsafe extern "C" fn(handle: ArtemisHandle) -> c_int;
type ArtemisProperties_ = unsafe extern "C" fn(handle: ArtemisHandle, prop: *mut ArtemisProperties) -> c_int;
type ArtemisColourProperties = unsafe extern "C" fn(
    handle: ArtemisHandle,
    colour_type: *mut c_int,
    normal_offset_x: *mut c_int,
    normal_offset_y: *mut c_int,
    preview_offset_x: *mut c_int,
    preview_offset_y: *mut c_int,
) -> c_int;
type ArtemisBin = unsafe extern "C" fn(handle: ArtemisHandle, x: c_int, y: c_int) -> c_int;
type ArtemisGetBin = unsafe extern "C" fn(handle: ArtemisHandle, x: *mut c_int, y: *mut c_int) -> c_int;
type ArtemisGetMaxBin = unsafe extern "C" fn(handle: ArtemisHandle, x: *mut c_int, y: *mut c_int) -> c_int;
type ArtemisSubframe = unsafe extern "C" fn(handle: ArtemisHandle, x: c_int, y: c_int, w: c_int, h: c_int) -> c_int;
type ArtemisGetSubframe = unsafe extern "C" fn(handle: ArtemisHandle, x: *mut c_int, y: *mut c_int, w: *mut c_int, h: *mut c_int) -> c_int;
type ArtemisStartExposure = unsafe extern "C" fn(handle: ArtemisHandle, seconds: c_float) -> c_int;
type ArtemisAbortExposure = unsafe extern "C" fn(handle: ArtemisHandle) -> c_int;
type ArtemisImageReady = unsafe extern "C" fn(handle: ArtemisHandle) -> c_int;
type ArtemisCameraState_ = unsafe extern "C" fn(handle: ArtemisHandle) -> c_int;
type ArtemisExposureTimeRemaining = unsafe extern "C" fn(handle: ArtemisHandle) -> c_float;
type ArtemisGetImageData = unsafe extern "C" fn(
    handle: ArtemisHandle,
    x: *mut c_int,
    y: *mut c_int,
    w: *mut c_int,
    h: *mut c_int,
    binx: *mut c_int,
    biny: *mut c_int,
) -> c_int;
type ArtemisImageBuffer = unsafe extern "C" fn(handle: ArtemisHandle) -> *mut c_void;
type ArtemisSetCooling = unsafe extern "C" fn(handle: ArtemisHandle, setpoint: c_int) -> c_int;
type ArtemisCoolingInfo = unsafe extern "C" fn(
    handle: ArtemisHandle,
    flags: *mut c_int,
    level: *mut c_int,
    minlvl: *mut c_int,
    maxlvl: *mut c_int,
    setpoint: *mut c_int,
) -> c_int;
type ArtemisCoolerWarmUp = unsafe extern "C" fn(handle: ArtemisHandle) -> c_int;
type ArtemisTemperatureSensorInfo = unsafe extern "C" fn(handle: ArtemisHandle, sensor: c_int, temperature: *mut c_int) -> c_int;
type ArtemisSetGain = unsafe extern "C" fn(handle: ArtemisHandle, preview: c_int, gain: c_int, offset: c_int) -> c_int;
type ArtemisGetGain = unsafe extern "C" fn(handle: ArtemisHandle, preview: c_int, gain: *mut c_int, offset: *mut c_int) -> c_int;
type ArtemisPulseGuide = unsafe extern "C" fn(handle: ArtemisHandle, axis: c_int, milli: c_int) -> c_int;
type ArtemisAPIVersion = unsafe extern "C" fn() -> c_int;
type ArtemisSetDarkMode = unsafe extern "C" fn(handle: ArtemisHandle, enable: c_int) -> c_int;
type ArtemisEightBitMode = unsafe extern "C" fn(handle: ArtemisHandle, eightbit: c_int) -> c_int;

/// Atik SDK wrapper with dynamically loaded functions
struct AtikSdk {
    _library: libloading::Library,
    device_count: ArtemisDeviceCount,
    device_present: ArtemisDevicePresent,
    device_name: ArtemisDeviceName,
    device_serial: ArtemisDeviceSerial,
    device_is_camera: ArtemisDeviceIsCamera,
    connect: ArtemisConnect,
    disconnect: ArtemisDisconnect,
    is_connected: ArtemisIsConnected,
    properties: ArtemisProperties_,
    colour_properties: ArtemisColourProperties,
    bin: ArtemisBin,
    get_bin: ArtemisGetBin,
    get_max_bin: ArtemisGetMaxBin,
    subframe: ArtemisSubframe,
    get_subframe: ArtemisGetSubframe,
    start_exposure: ArtemisStartExposure,
    abort_exposure: ArtemisAbortExposure,
    image_ready: ArtemisImageReady,
    camera_state: ArtemisCameraState_,
    exposure_time_remaining: ArtemisExposureTimeRemaining,
    get_image_data: ArtemisGetImageData,
    image_buffer: ArtemisImageBuffer,
    set_cooling: ArtemisSetCooling,
    cooling_info: ArtemisCoolingInfo,
    cooler_warm_up: ArtemisCoolerWarmUp,
    temperature_sensor_info: ArtemisTemperatureSensorInfo,
    set_gain: ArtemisSetGain,
    get_gain: ArtemisGetGain,
    pulse_guide: ArtemisPulseGuide,
    api_version: ArtemisAPIVersion,
    set_dark_mode: ArtemisSetDarkMode,
    eight_bit_mode: ArtemisEightBitMode,
}

impl AtikSdk {
    /// Load the SDK from the default paths
    fn load() -> Result<Self, NativeError> {
        let lib_name = if cfg!(target_os = "windows") {
            "AtikCameras.dll"
        } else if cfg!(target_os = "macos") {
            "libatikcameras.dylib"
        } else {
            "libatikcameras.so"
        };

        let library = unsafe { libloading::Library::new(lib_name) }
            .map_err(|e| NativeError::SdkError(format!("Failed to load Atik SDK: {}", e)))?;

        unsafe {
            Ok(Self {
                device_count: *library
                    .get::<ArtemisDeviceCount>(b"ArtemisDeviceCount\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisDeviceCount: {}", e)))?,
                device_present: *library
                    .get::<ArtemisDevicePresent>(b"ArtemisDevicePresent\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisDevicePresent: {}", e)))?,
                device_name: *library
                    .get::<ArtemisDeviceName>(b"ArtemisDeviceName\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisDeviceName: {}", e)))?,
                device_serial: *library
                    .get::<ArtemisDeviceSerial>(b"ArtemisDeviceSerial\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisDeviceSerial: {}", e)))?,
                device_is_camera: *library
                    .get::<ArtemisDeviceIsCamera>(b"ArtemisDeviceIsCamera\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisDeviceIsCamera: {}", e)))?,
                connect: *library
                    .get::<ArtemisConnect>(b"ArtemisConnect\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisConnect: {}", e)))?,
                disconnect: *library
                    .get::<ArtemisDisconnect>(b"ArtemisDisconnect\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisDisconnect: {}", e)))?,
                is_connected: *library
                    .get::<ArtemisIsConnected>(b"ArtemisIsConnected\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisIsConnected: {}", e)))?,
                properties: *library
                    .get::<ArtemisProperties_>(b"ArtemisProperties\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisProperties: {}", e)))?,
                colour_properties: *library
                    .get::<ArtemisColourProperties>(b"ArtemisColourProperties\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisColourProperties: {}", e)))?,
                bin: *library
                    .get::<ArtemisBin>(b"ArtemisBin\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisBin: {}", e)))?,
                get_bin: *library
                    .get::<ArtemisGetBin>(b"ArtemisGetBin\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisGetBin: {}", e)))?,
                get_max_bin: *library
                    .get::<ArtemisGetMaxBin>(b"ArtemisGetMaxBin\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisGetMaxBin: {}", e)))?,
                subframe: *library
                    .get::<ArtemisSubframe>(b"ArtemisSubframe\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisSubframe: {}", e)))?,
                get_subframe: *library
                    .get::<ArtemisGetSubframe>(b"ArtemisGetSubframe\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisGetSubframe: {}", e)))?,
                start_exposure: *library
                    .get::<ArtemisStartExposure>(b"ArtemisStartExposure\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisStartExposure: {}", e)))?,
                abort_exposure: *library
                    .get::<ArtemisAbortExposure>(b"ArtemisAbortExposure\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisAbortExposure: {}", e)))?,
                image_ready: *library
                    .get::<ArtemisImageReady>(b"ArtemisImageReady\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisImageReady: {}", e)))?,
                camera_state: *library
                    .get::<ArtemisCameraState_>(b"ArtemisCameraState\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisCameraState: {}", e)))?,
                exposure_time_remaining: *library
                    .get::<ArtemisExposureTimeRemaining>(b"ArtemisExposureTimeRemaining\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisExposureTimeRemaining: {}", e)))?,
                get_image_data: *library
                    .get::<ArtemisGetImageData>(b"ArtemisGetImageData\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisGetImageData: {}", e)))?,
                image_buffer: *library
                    .get::<ArtemisImageBuffer>(b"ArtemisImageBuffer\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisImageBuffer: {}", e)))?,
                set_cooling: *library
                    .get::<ArtemisSetCooling>(b"ArtemisSetCooling\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisSetCooling: {}", e)))?,
                cooling_info: *library
                    .get::<ArtemisCoolingInfo>(b"ArtemisCoolingInfo\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisCoolingInfo: {}", e)))?,
                cooler_warm_up: *library
                    .get::<ArtemisCoolerWarmUp>(b"ArtemisCoolerWarmUp\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisCoolerWarmUp: {}", e)))?,
                temperature_sensor_info: *library
                    .get::<ArtemisTemperatureSensorInfo>(b"ArtemisTemperatureSensorInfo\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisTemperatureSensorInfo: {}", e)))?,
                set_gain: *library
                    .get::<ArtemisSetGain>(b"ArtemisSetGain\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisSetGain: {}", e)))?,
                get_gain: *library
                    .get::<ArtemisGetGain>(b"ArtemisGetGain\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisGetGain: {}", e)))?,
                pulse_guide: *library
                    .get::<ArtemisPulseGuide>(b"ArtemisPulseGuide\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisPulseGuide: {}", e)))?,
                api_version: *library
                    .get::<ArtemisAPIVersion>(b"ArtemisAPIVersion\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisAPIVersion: {}", e)))?,
                set_dark_mode: *library
                    .get::<ArtemisSetDarkMode>(b"ArtemisSetDarkMode\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisSetDarkMode: {}", e)))?,
                eight_bit_mode: *library
                    .get::<ArtemisEightBitMode>(b"ArtemisEightBitMode\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load ArtemisEightBitMode: {}", e)))?,
                _library: library,
            })
        }
    }
}

/// Global SDK instance
static SDK: OnceLock<Result<AtikSdk, String>> = OnceLock::new();

fn get_sdk() -> Result<&'static AtikSdk, NativeError> {
    SDK.get_or_init(|| AtikSdk::load().map_err(|e| e.to_string()))
        .as_ref()
        .map_err(|e| NativeError::SdkError(e.clone()))
}

// =============================================================================
// Discovery
// =============================================================================

/// Discovered Atik camera info
#[derive(Debug, Clone)]
pub struct AtikDiscoveryInfo {
    pub device_index: i32,
    pub name: String,
    pub serial_number: Option<String>,
}

/// Discover connected Atik cameras
pub async fn discover_devices() -> Result<Vec<AtikDiscoveryInfo>, NativeError> {
    let sdk = match get_sdk() {
        Ok(sdk) => sdk,
        Err(_) => return Ok(Vec::new()),
    };

    // Acquire global SDK mutex for thread safety
    let _lock = atik_mutex().lock().await;

    let count = unsafe { (sdk.device_count)() };
    let mut devices = Vec::new();

    for i in 0..count {
        let present = unsafe { (sdk.device_present)(i) };
        if present == 0 {
            continue;
        }

        let is_camera = unsafe { (sdk.device_is_camera)(i) };
        if is_camera == 0 {
            continue;
        }

        let mut name_buf = [0i8; 100];
        let name = if unsafe { (sdk.device_name)(i, name_buf.as_mut_ptr()) } != 0 {
            unsafe { CStr::from_ptr(name_buf.as_ptr()) }
                .to_string_lossy()
                .to_string()
        } else {
            format!("Atik Camera {}", i)
        };

        let mut serial_buf = [0i8; 100];
        let serial = if unsafe { (sdk.device_serial)(i, serial_buf.as_mut_ptr()) } != 0 {
            let s = unsafe { CStr::from_ptr(serial_buf.as_ptr()) }
                .to_string_lossy()
                .to_string();
            if s.is_empty() { None } else { Some(s) }
        } else {
            None
        };

        devices.push(AtikDiscoveryInfo {
            device_index: i,
            name,
            serial_number: serial,
        });
    }

    Ok(devices)
}

/// Check if SDK is available
pub fn is_sdk_available() -> bool {
    get_sdk().is_ok()
}

/// Get SDK status for diagnostics
pub fn get_sdk_status() -> (bool, String) {
    match get_sdk() {
        Ok(sdk) => {
            let version = unsafe { (sdk.api_version)() };
            (true, format!("Atik SDK v{}", version))
        }
        Err(e) => (false, format!("SDK not available: {}", e)),
    }
}

// =============================================================================
// Atik Camera Implementation
// =============================================================================

/// Atik camera native driver
pub struct AtikCamera {
    device_index: i32,
    device_id: String,
    name: String,
    handle: Mutex<HandleWrapper>,
    connected: bool,
    capabilities: CameraCapabilities,
    sensor_info: SensorInfo,
    state: CameraState,
    // Current settings
    current_gain: i32,
    current_offset: i32,
    current_bin_x: i32,
    current_bin_y: i32,
    subframe: Option<SubFrame>,
    cooler_on: bool,
    target_temp: f64,
    // Exposure tracking
    exposure_duration: f64,
}

impl std::fmt::Debug for AtikCamera {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("AtikCamera")
            .field("device_id", &self.device_id)
            .field("name", &self.name)
            .field("device_index", &self.device_index)
            .finish()
    }
}

impl AtikCamera {
    /// Create a new Atik camera instance
    pub fn new(device_index: i32) -> Self {
        Self {
            device_index,
            device_id: format!("atik_{}", device_index),
            name: format!("Atik Camera {}", device_index),
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
            target_temp: -10.0,
            exposure_duration: 0.0,
        }
    }
}

#[async_trait]
impl NativeDevice for AtikCamera {
    fn id(&self) -> &str {
        &self.device_id
    }

    fn name(&self) -> &str {
        &self.name
    }

    fn vendor(&self) -> NativeVendor {
        NativeVendor::Atik
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
        let _lock = atik_mutex().lock().await;

        // Connect to camera
        let new_handle = unsafe { (sdk.connect)(self.device_index) };
        if new_handle.is_null() {
            tracing::error!(
                "Atik ArtemisConnect() returned NULL for device index {}. Check USB connection.",
                self.device_index
            );
            return Err(NativeError::SdkError(format!(
                "Failed to connect to Atik camera at index {}. SDK returned NULL handle. Ensure camera is connected and not in use.",
                self.device_index
            )));
        }

        // Store handle
        {
            let mut handle = self.handle.lock().unwrap();
            *handle = HandleWrapper(new_handle);
        }

        // Check connection
        let handle = self.handle.lock().unwrap().0;
        if unsafe { (sdk.is_connected)(handle) } == 0 {
            tracing::error!(
                "Atik camera at index {} - ArtemisIsConnected() returned false after successful connect.",
                self.device_index
            );
            return Err(NativeError::SdkError(format!(
                "Atik camera connection verification failed for index {}. Device may have disconnected.",
                self.device_index
            )));
        }

        // Get camera properties
        let mut props: ArtemisProperties = unsafe { std::mem::zeroed() };
        let result = unsafe { (sdk.properties)(handle, &mut props) };
        if ArtemisError::from_i32(result) != ArtemisError::Ok {
            unsafe { (sdk.disconnect)(handle) };
            return Err(ArtemisError::from_i32(result).to_native_error("get properties"));
        }

        // Get max binning
        let mut max_bin_x: c_int = 1;
        let mut max_bin_y: c_int = 1;
        let _ = unsafe { (sdk.get_max_bin)(handle, &mut max_bin_x, &mut max_bin_y) };

        // Check for cooling support
        let mut cooling_flags: c_int = 0;
        let mut _level: c_int = 0;
        let mut _minlvl: c_int = 0;
        let mut _maxlvl: c_int = 0;
        let mut _setpoint: c_int = 0;
        let can_cool = unsafe {
            (sdk.cooling_info)(handle, &mut cooling_flags, &mut _level, &mut _minlvl, &mut _maxlvl, &mut _setpoint) == 0
                && (cooling_flags & 1) != 0 // ARTEMIS_COOLING_INFO_HASCOOLING
        };

        // Set capabilities
        self.capabilities = CameraCapabilities {
            can_cool,
            can_set_gain: true,
            can_set_offset: true,
            can_set_binning: max_bin_x > 1 || max_bin_y > 1,
            can_subframe: true,
            has_shutter: (props.camera_flags & ARTEMIS_CAMERA_HAS_SHUTTER) != 0,
            has_guider_port: (props.camera_flags & ARTEMIS_CAMERA_HAS_GUIDE_PORT) != 0,
            max_bin_x: max_bin_x,
            max_bin_y: max_bin_y,
            supports_readout_modes: false,
        };

        // Get colour properties for Bayer pattern
        let mut colour_type: c_int = 0;
        let mut _normal_offset_x: c_int = 0;
        let mut _normal_offset_y: c_int = 0;
        let mut _preview_offset_x: c_int = 0;
        let mut _preview_offset_y: c_int = 0;
        let _ = unsafe {
            (sdk.colour_properties)(
                handle,
                &mut colour_type,
                &mut _normal_offset_x,
                &mut _normal_offset_y,
                &mut _preview_offset_x,
                &mut _preview_offset_y,
            )
        };

        let is_color = colour_type == ArtemisColourType::Rggb as i32;
        let bayer_pattern = if is_color {
            Some(BayerPattern::Rggb)
        } else {
            None
        };

        // Set sensor info
        self.sensor_info = SensorInfo {
            width: props.pixels_x as u32,
            height: props.pixels_y as u32,
            pixel_size_x: props.pixel_microns_x as f64,
            pixel_size_y: props.pixel_microns_y as f64,
            max_adu: 65535,
            bit_depth: 16,
            color: is_color,
            bayer_pattern,
        };

        // Get camera name from description
        let name = unsafe { CStr::from_ptr(props.description.as_ptr()) }
            .to_string_lossy()
            .trim()
            .to_string();
        if !name.is_empty() {
            self.name = name;
        }

        // Set 16-bit mode
        let _ = unsafe { (sdk.eight_bit_mode)(handle, 0) };

        // Get initial gain/offset
        let mut gain: c_int = 0;
        let mut offset: c_int = 0;
        if unsafe { (sdk.get_gain)(handle, 0, &mut gain, &mut offset) } == 0 {
            self.current_gain = gain;
            self.current_offset = offset;
        }

        self.connected = true;
        self.state = CameraState::Idle;

        tracing::info!(
            "Connected to Atik camera: {} ({}x{})",
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
        let _lock = atik_mutex().lock().await;

        let handle = self.handle.lock().unwrap().0;

        // Abort any exposure
        let _ = unsafe { (sdk.abort_exposure)(handle) };

        // Warm up cooler gracefully
        if self.cooler_on {
            let _ = unsafe { (sdk.cooler_warm_up)(handle) };
        }

        // Disconnect
        let result = unsafe { (sdk.disconnect)(handle) };
        if result == 0 {
            tracing::error!(
                "Atik ArtemisDisconnect() failed for camera '{}'. Device may be in an inconsistent state.",
                self.name
            );
            return Err(NativeError::SdkError(format!(
                "Failed to disconnect from Atik camera '{}'. Device may need reconnection.",
                self.name
            )));
        }

        {
            let mut h = self.handle.lock().unwrap();
            *h = HandleWrapper(std::ptr::null_mut());
        }
        self.connected = false;
        self.state = CameraState::Idle;

        tracing::info!("Disconnected from Atik camera: {}", self.name);

        Ok(())
    }
}

#[async_trait]
impl NativeCamera for AtikCamera {
    fn capabilities(&self) -> CameraCapabilities {
        self.capabilities.clone()
    }

    async fn get_status(&self) -> Result<CameraStatus, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = atik_mutex().lock().await;

        let handle = self.handle.lock().unwrap().0;

        // Get temperature
        let sensor_temp = {
            let mut temp: c_int = 0;
            // Sensor 1 is the CCD temperature
            if unsafe { (sdk.temperature_sensor_info)(handle, 1, &mut temp) } == 0 {
                Some(temp as f64 / 100.0)
            } else {
                None
            }
        };

        // Get cooler info
        let (cooler_power, target_temp) = if self.capabilities.can_cool {
            let mut flags: c_int = 0;
            let mut level: c_int = 0;
            let mut minlvl: c_int = 0;
            let mut maxlvl: c_int = 0;
            let mut setpoint: c_int = 0;
            if unsafe { (sdk.cooling_info)(handle, &mut flags, &mut level, &mut minlvl, &mut maxlvl, &mut setpoint) } == 0 {
                let power = if maxlvl > 0 {
                    Some((level as f64 / maxlvl as f64) * 100.0)
                } else {
                    None
                };
                let target = Some(setpoint as f64 / 100.0);
                (power, target)
            } else {
                (None, None)
            }
        } else {
            (None, None)
        };

        // Get exposure remaining
        let exposure_remaining = if self.state == CameraState::Exposing {
            let remaining = unsafe { (sdk.exposure_time_remaining)(handle) };
            Some(remaining as f64)
        } else {
            None
        };

        Ok(CameraStatus {
            state: self.state,
            sensor_temp,
            cooler_power,
            target_temp,
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

        // Set gain if provided
        if let Some(gain) = params.gain {
            self.set_gain(gain).await?;
        }

        // Set offset if provided
        if let Some(offset) = params.offset {
            self.set_offset(offset).await?;
        }

        // Set binning
        self.set_binning(params.bin_x, params.bin_y).await?;

        // Set subframe
        self.set_subframe(params.subframe.clone()).await?;

        // Now get SDK and handle after all awaits are complete
        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = atik_mutex().lock().await;

        let handle = self.handle.lock().unwrap().0;

        // Set dark mode (normal mode by default - dark frames handled at higher level)
        let _ = unsafe { (sdk.set_dark_mode)(handle, 0) };

        // Start exposure
        let result = unsafe { (sdk.start_exposure)(handle, params.duration_secs as c_float) };
        if ArtemisError::from_i32(result) != ArtemisError::Ok {
            return Err(ArtemisError::from_i32(result).to_native_error("start exposure"));
        }

        self.exposure_duration = params.duration_secs;
        self.state = CameraState::Exposing;

        Ok(())
    }

    async fn abort_exposure(&mut self) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = atik_mutex().lock().await;

        let handle = self.handle.lock().unwrap().0;
        let result = unsafe { (sdk.abort_exposure)(handle) };
        if ArtemisError::from_i32(result) != ArtemisError::Ok {
            return Err(ArtemisError::from_i32(result).to_native_error("abort exposure"));
        }

        self.state = CameraState::Idle;
        Ok(())
    }

    async fn is_exposure_complete(&self) -> Result<bool, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = atik_mutex().lock().await;

        let handle = self.handle.lock().unwrap().0;
        let ready = unsafe { (sdk.image_ready)(handle) };
        Ok(ready != 0)
    }

    async fn download_image(&mut self) -> Result<ImageData, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = atik_mutex().lock().await;

        let handle = self.handle.lock().unwrap().0;

        self.state = CameraState::Downloading;

        // Get image info
        let mut x: c_int = 0;
        let mut y: c_int = 0;
        let mut w: c_int = 0;
        let mut h: c_int = 0;
        let mut binx: c_int = 0;
        let mut biny: c_int = 0;

        let result = unsafe { (sdk.get_image_data)(handle, &mut x, &mut y, &mut w, &mut h, &mut binx, &mut biny) };
        if ArtemisError::from_i32(result) != ArtemisError::Ok {
            self.state = CameraState::Error;
            return Err(ArtemisError::from_i32(result).to_native_error("get image data"));
        }

        // Get image buffer
        let buffer_ptr = unsafe { (sdk.image_buffer)(handle) };
        if buffer_ptr.is_null() {
            tracing::error!(
                "Atik ArtemisImageBuffer() returned NULL for camera '{}'. Image: {}x{}, bin: {}x{}",
                self.name, w, h, binx, biny
            );
            self.state = CameraState::Error;
            return Err(NativeError::SdkError(format!(
                "Image buffer is NULL on Atik camera '{}'. Image download failed after successful exposure.",
                self.name
            )));
        }

        // Copy image data (16-bit)
        let pixel_count = (w as usize) * (h as usize);
        let buffer_slice = unsafe { std::slice::from_raw_parts(buffer_ptr as *const u16, pixel_count) };
        let data: Vec<u16> = buffer_slice.to_vec();

        // Get temperature for metadata
        let temperature = {
            let mut temp: c_int = 0;
            if unsafe { (sdk.temperature_sensor_info)(handle, 1, &mut temp) } == 0 {
                Some(temp as f64 / 100.0)
            } else {
                None
            }
        };

        let metadata = ImageMetadata {
            exposure_time: self.exposure_duration,
            gain: self.current_gain,
            offset: self.current_offset,
            bin_x: binx,
            bin_y: biny,
            temperature,
            timestamp: chrono::Utc::now(),
            subframe: self.subframe.clone(),
            readout_mode: None,
            vendor_data: VendorFeatures::default(),
        };

        self.state = CameraState::Idle;

        Ok(ImageData {
            width: w as u32,
            height: h as u32,
            data,
            bits_per_pixel: self.sensor_info.bit_depth,
            bayer_pattern: self.sensor_info.bayer_pattern,
            metadata,
        })
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
        let _lock = atik_mutex().lock().await;

        let handle = self.handle.lock().unwrap().0;

        if enabled {
            // Temperature in hundredths of degrees
            let setpoint = (target_temp * 100.0) as c_int;
            let result = unsafe { (sdk.set_cooling)(handle, setpoint) };
            if ArtemisError::from_i32(result) != ArtemisError::Ok {
                return Err(ArtemisError::from_i32(result).to_native_error("set cooling"));
            }
        } else {
            let result = unsafe { (sdk.cooler_warm_up)(handle) };
            if ArtemisError::from_i32(result) != ArtemisError::Ok {
                return Err(ArtemisError::from_i32(result).to_native_error("warm up cooler"));
            }
        }

        self.cooler_on = enabled;
        self.target_temp = target_temp;
        Ok(())
    }

    async fn get_temperature(&self) -> Result<f64, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = atik_mutex().lock().await;

        let handle = self.handle.lock().unwrap().0;
        let mut temp: c_int = 0;

        let result = unsafe { (sdk.temperature_sensor_info)(handle, 1, &mut temp) };
        if ArtemisError::from_i32(result) != ArtemisError::Ok {
            return Err(ArtemisError::from_i32(result).to_native_error("get temperature"));
        }

        Ok(temp as f64 / 100.0)
    }

    async fn get_cooler_power(&self) -> Result<f64, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }
        if !self.capabilities.can_cool {
            return Err(NativeError::NotSupported);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = atik_mutex().lock().await;

        let handle = self.handle.lock().unwrap().0;
        let mut flags: c_int = 0;
        let mut level: c_int = 0;
        let mut minlvl: c_int = 0;
        let mut maxlvl: c_int = 0;
        let mut setpoint: c_int = 0;

        let result = unsafe { (sdk.cooling_info)(handle, &mut flags, &mut level, &mut minlvl, &mut maxlvl, &mut setpoint) };
        if ArtemisError::from_i32(result) != ArtemisError::Ok {
            return Err(ArtemisError::from_i32(result).to_native_error("get cooler power"));
        }

        if maxlvl > 0 {
            Ok((level as f64 / maxlvl as f64) * 100.0)
        } else {
            Ok(0.0)
        }
    }

    async fn set_gain(&mut self, gain: i32) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = atik_mutex().lock().await;

        let handle = self.handle.lock().unwrap().0;
        let result = unsafe { (sdk.set_gain)(handle, 0, gain as c_int, self.current_offset as c_int) };
        if ArtemisError::from_i32(result) != ArtemisError::Ok {
            return Err(ArtemisError::from_i32(result).to_native_error("set gain"));
        }

        self.current_gain = gain;
        Ok(())
    }

    async fn get_gain(&self) -> Result<i32, NativeError> {
        Ok(self.current_gain)
    }

    async fn set_offset(&mut self, offset: i32) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = atik_mutex().lock().await;

        let handle = self.handle.lock().unwrap().0;
        let result = unsafe { (sdk.set_gain)(handle, 0, self.current_gain as c_int, offset as c_int) };
        if ArtemisError::from_i32(result) != ArtemisError::Ok {
            return Err(ArtemisError::from_i32(result).to_native_error("set offset"));
        }

        self.current_offset = offset;
        Ok(())
    }

    async fn get_offset(&self) -> Result<i32, NativeError> {
        Ok(self.current_offset)
    }

    async fn set_binning(&mut self, bin_x: i32, bin_y: i32) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        if bin_x > self.capabilities.max_bin_x || bin_y > self.capabilities.max_bin_y {
            return Err(NativeError::InvalidParameter(format!(
                "Binning {}x{} exceeds max {}x{}",
                bin_x, bin_y, self.capabilities.max_bin_x, self.capabilities.max_bin_y
            )));
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = atik_mutex().lock().await;

        let handle = self.handle.lock().unwrap().0;
        let result = unsafe { (sdk.bin)(handle, bin_x as c_int, bin_y as c_int) };
        if ArtemisError::from_i32(result) != ArtemisError::Ok {
            return Err(ArtemisError::from_i32(result).to_native_error("set binning"));
        }

        self.current_bin_x = bin_x;
        self.current_bin_y = bin_y;
        Ok(())
    }

    async fn get_binning(&self) -> Result<(i32, i32), NativeError> {
        Ok((self.current_bin_x, self.current_bin_y))
    }

    async fn set_subframe(&mut self, subframe: Option<SubFrame>) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = atik_mutex().lock().await;

        let handle = self.handle.lock().unwrap().0;

        let (x, y, w, h) = match &subframe {
            Some(sf) => (
                sf.start_x as c_int,
                sf.start_y as c_int,
                sf.width as c_int,
                sf.height as c_int,
            ),
            None => (
                0,
                0,
                (self.sensor_info.width / self.current_bin_x as u32) as c_int,
                (self.sensor_info.height / self.current_bin_y as u32) as c_int,
            ),
        };

        let result = unsafe { (sdk.subframe)(handle, x, y, w, h) };
        if ArtemisError::from_i32(result) != ArtemisError::Ok {
            return Err(ArtemisError::from_i32(result).to_native_error("set subframe"));
        }

        self.subframe = subframe;
        Ok(())
    }

    fn get_sensor_info(&self) -> SensorInfo {
        self.sensor_info.clone()
    }

    async fn get_readout_modes(&self) -> Result<Vec<ReadoutMode>, NativeError> {
        Ok(vec![ReadoutMode {
            name: "Normal".to_string(),
            description: "Standard readout mode".to_string(),
            index: 0,
            gain_min: None,
            gain_max: None,
            offset_min: None,
            offset_max: None,
        }])
    }

    async fn set_readout_mode(&mut self, _mode: &ReadoutMode) -> Result<(), NativeError> {
        Ok(())
    }

    async fn get_vendor_features(&self) -> Result<VendorFeatures, NativeError> {
        Ok(VendorFeatures::default())
    }

    async fn get_gain_range(&self) -> Result<(i32, i32), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        // Atik cameras typically have limited gain control compared to CMOS cameras.
        // Most Atik CCD cameras have fixed or limited gain.
        // Return reasonable defaults that work for most Atik cameras.
        Ok((0, 100))
    }

    async fn get_offset_range(&self) -> Result<(i32, i32), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        // Atik cameras have limited offset/bias control.
        // Return reasonable defaults.
        Ok((0, 255))
    }
}
