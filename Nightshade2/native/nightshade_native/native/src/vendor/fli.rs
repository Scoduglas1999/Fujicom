//! Finger Lakes Instrumentation (FLI) SDK Bindings
//!
//! Native driver for FLI cameras, focusers, and filter wheels using libfli.
//! This is an open-source library with cross-platform support.

use crate::camera::{
    BayerPattern, CameraCapabilities, CameraState, CameraStatus, ExposureParams,
    ImageData, ImageMetadata, ReadoutMode, SensorInfo, SubFrame, VendorFeatures,
};
use crate::sync::fli_mutex;
use crate::traits::{NativeCamera, NativeDevice, NativeError, NativeFocuser, NativeFilterWheel};
use crate::NativeVendor;
use async_trait::async_trait;
use std::ffi::{c_char, c_double, c_int, c_long, CStr, CString};
use std::sync::OnceLock;

// =============================================================================
// FLI SDK Types (from libfli.h)
// =============================================================================

/// FLI device handle
type FliDev = c_long;

const FLI_INVALID_DEVICE: FliDev = -1;

// Domain flags
const FLIDOMAIN_USB: c_long = 0x02;
const FLIDEVICE_CAMERA: c_long = 0x100;
const FLIDEVICE_FILTERWHEEL: c_long = 0x200;
const FLIDEVICE_FOCUSER: c_long = 0x300;

// Frame types
const FLI_FRAME_TYPE_NORMAL: c_long = 0;
const FLI_FRAME_TYPE_DARK: c_long = 1;

// Bit depth
const FLI_MODE_16BIT: c_long = 1;

// Camera status
const FLI_CAMERA_STATUS_IDLE: c_long = 0x00;
const FLI_CAMERA_STATUS_EXPOSING: c_long = 0x02;
const FLI_CAMERA_STATUS_READING_CCD: c_long = 0x03;
const FLI_CAMERA_DATA_READY: c_long = 0x80000000u32 as c_long;

// =============================================================================
// SDK Function Pointers
// =============================================================================

type FLIOpen = unsafe extern "C" fn(dev: *mut FliDev, name: *const c_char, domain: c_long) -> c_long;
type FLIClose = unsafe extern "C" fn(dev: FliDev) -> c_long;
type FLIGetModel = unsafe extern "C" fn(dev: FliDev, model: *mut c_char, len: usize) -> c_long;
type FLIGetSerialString = unsafe extern "C" fn(dev: FliDev, serial: *mut c_char, len: usize) -> c_long;
type FLIGetPixelSize = unsafe extern "C" fn(dev: FliDev, pixel_x: *mut c_double, pixel_y: *mut c_double) -> c_long;
type FLIGetArrayArea = unsafe extern "C" fn(dev: FliDev, ul_x: *mut c_long, ul_y: *mut c_long, lr_x: *mut c_long, lr_y: *mut c_long) -> c_long;
type FLIGetVisibleArea = unsafe extern "C" fn(dev: FliDev, ul_x: *mut c_long, ul_y: *mut c_long, lr_x: *mut c_long, lr_y: *mut c_long) -> c_long;
type FLISetExposureTime = unsafe extern "C" fn(dev: FliDev, exptime: c_long) -> c_long;
type FLISetImageArea = unsafe extern "C" fn(dev: FliDev, ul_x: c_long, ul_y: c_long, lr_x: c_long, lr_y: c_long) -> c_long;
type FLISetHBin = unsafe extern "C" fn(dev: FliDev, hbin: c_long) -> c_long;
type FLISetVBin = unsafe extern "C" fn(dev: FliDev, vbin: c_long) -> c_long;
type FLISetFrameType = unsafe extern "C" fn(dev: FliDev, frametype: c_long) -> c_long;
type FLIExposeFrame = unsafe extern "C" fn(dev: FliDev) -> c_long;
type FLICancelExposure = unsafe extern "C" fn(dev: FliDev) -> c_long;
type FLIGetExposureStatus = unsafe extern "C" fn(dev: FliDev, timeleft: *mut c_long) -> c_long;
type FLISetTemperature = unsafe extern "C" fn(dev: FliDev, temperature: c_double) -> c_long;
type FLIGetTemperature = unsafe extern "C" fn(dev: FliDev, temperature: *mut c_double) -> c_long;
type FLIGetCoolerPower = unsafe extern "C" fn(dev: FliDev, power: *mut c_double) -> c_long;
type FLIGrabFrame = unsafe extern "C" fn(dev: FliDev, buff: *mut u8, buffsize: usize, bytesgrabbed: *mut usize) -> c_long;
type FLIGrabRow = unsafe extern "C" fn(dev: FliDev, buff: *mut u8, width: usize) -> c_long;
type FLISetBitDepth = unsafe extern "C" fn(dev: FliDev, bitdepth: c_long) -> c_long;
type FLIGetDeviceStatus = unsafe extern "C" fn(dev: FliDev, status: *mut c_long) -> c_long;
type FLIGetLibVersion = unsafe extern "C" fn(ver: *mut c_char, len: usize) -> c_long;
type FLIList = unsafe extern "C" fn(domain: c_long, names: *mut *mut *mut c_char) -> c_long;
type FLIFreeList = unsafe extern "C" fn(names: *mut *mut c_char) -> c_long;
type FLICreateList = unsafe extern "C" fn(domain: c_long) -> c_long;
type FLIDeleteList = unsafe extern "C" fn() -> c_long;
type FLIListFirst = unsafe extern "C" fn(domain: *mut c_long, filename: *mut c_char, fnlen: usize, name: *mut c_char, namelen: usize) -> c_long;
type FLIListNext = unsafe extern "C" fn(domain: *mut c_long, filename: *mut c_char, fnlen: usize, name: *mut c_char, namelen: usize) -> c_long;
type FLISetFilterPos = unsafe extern "C" fn(dev: FliDev, filter: c_long) -> c_long;
type FLIGetFilterPos = unsafe extern "C" fn(dev: FliDev, filter: *mut c_long) -> c_long;
type FLIGetFilterCount = unsafe extern "C" fn(dev: FliDev, filter: *mut c_long) -> c_long;
type FLIStepMotor = unsafe extern "C" fn(dev: FliDev, steps: c_long) -> c_long;
type FLIStepMotorAsync = unsafe extern "C" fn(dev: FliDev, steps: c_long) -> c_long;
type FLIGetStepperPosition = unsafe extern "C" fn(dev: FliDev, position: *mut c_long) -> c_long;
type FLIGetStepsRemaining = unsafe extern "C" fn(dev: FliDev, steps: *mut c_long) -> c_long;
type FLIHomeFocuser = unsafe extern "C" fn(dev: FliDev) -> c_long;
type FLIGetFocuserExtent = unsafe extern "C" fn(dev: FliDev, extent: *mut c_long) -> c_long;
type FLIReadTemperature = unsafe extern "C" fn(dev: FliDev, channel: c_long, temperature: *mut c_double) -> c_long;
type FLIEndExposure = unsafe extern "C" fn(dev: FliDev) -> c_long;

/// FLI SDK wrapper with dynamically loaded functions
struct FliSdk {
    _library: libloading::Library,
    open: FLIOpen,
    close: FLIClose,
    get_model: FLIGetModel,
    get_serial_string: FLIGetSerialString,
    get_pixel_size: FLIGetPixelSize,
    get_array_area: FLIGetArrayArea,
    get_visible_area: FLIGetVisibleArea,
    set_exposure_time: FLISetExposureTime,
    set_image_area: FLISetImageArea,
    set_hbin: FLISetHBin,
    set_vbin: FLISetVBin,
    set_frame_type: FLISetFrameType,
    expose_frame: FLIExposeFrame,
    cancel_exposure: FLICancelExposure,
    get_exposure_status: FLIGetExposureStatus,
    set_temperature: FLISetTemperature,
    get_temperature: FLIGetTemperature,
    get_cooler_power: FLIGetCoolerPower,
    grab_frame: FLIGrabFrame,
    grab_row: FLIGrabRow,
    set_bit_depth: FLISetBitDepth,
    get_device_status: FLIGetDeviceStatus,
    get_lib_version: FLIGetLibVersion,
    create_list: FLICreateList,
    delete_list: FLIDeleteList,
    list_first: FLIListFirst,
    list_next: FLIListNext,
    set_filter_pos: FLISetFilterPos,
    get_filter_pos: FLIGetFilterPos,
    get_filter_count: FLIGetFilterCount,
    step_motor: FLIStepMotor,
    step_motor_async: FLIStepMotorAsync,
    get_stepper_position: FLIGetStepperPosition,
    get_steps_remaining: FLIGetStepsRemaining,
    home_focuser: FLIHomeFocuser,
    get_focuser_extent: FLIGetFocuserExtent,
    read_temperature: FLIReadTemperature,
    end_exposure: FLIEndExposure,
}

impl FliSdk {
    /// Load the SDK from the default paths
    fn load() -> Result<Self, NativeError> {
        let lib_name = if cfg!(target_os = "windows") {
            "libfli.dll"
        } else if cfg!(target_os = "macos") {
            "libfli.dylib"
        } else {
            "libfli.so"
        };

        let library = unsafe { libloading::Library::new(lib_name) }
            .map_err(|e| NativeError::SdkError(format!("Failed to load FLI SDK: {}", e)))?;

        unsafe {
            Ok(Self {
                open: *library.get::<FLIOpen>(b"FLIOpen\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIOpen: {}", e)))?,
                close: *library.get::<FLIClose>(b"FLIClose\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIClose: {}", e)))?,
                get_model: *library.get::<FLIGetModel>(b"FLIGetModel\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIGetModel: {}", e)))?,
                get_serial_string: *library.get::<FLIGetSerialString>(b"FLIGetSerialString\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIGetSerialString: {}", e)))?,
                get_pixel_size: *library.get::<FLIGetPixelSize>(b"FLIGetPixelSize\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIGetPixelSize: {}", e)))?,
                get_array_area: *library.get::<FLIGetArrayArea>(b"FLIGetArrayArea\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIGetArrayArea: {}", e)))?,
                get_visible_area: *library.get::<FLIGetVisibleArea>(b"FLIGetVisibleArea\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIGetVisibleArea: {}", e)))?,
                set_exposure_time: *library.get::<FLISetExposureTime>(b"FLISetExposureTime\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLISetExposureTime: {}", e)))?,
                set_image_area: *library.get::<FLISetImageArea>(b"FLISetImageArea\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLISetImageArea: {}", e)))?,
                set_hbin: *library.get::<FLISetHBin>(b"FLISetHBin\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLISetHBin: {}", e)))?,
                set_vbin: *library.get::<FLISetVBin>(b"FLISetVBin\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLISetVBin: {}", e)))?,
                set_frame_type: *library.get::<FLISetFrameType>(b"FLISetFrameType\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLISetFrameType: {}", e)))?,
                expose_frame: *library.get::<FLIExposeFrame>(b"FLIExposeFrame\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIExposeFrame: {}", e)))?,
                cancel_exposure: *library.get::<FLICancelExposure>(b"FLICancelExposure\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLICancelExposure: {}", e)))?,
                get_exposure_status: *library.get::<FLIGetExposureStatus>(b"FLIGetExposureStatus\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIGetExposureStatus: {}", e)))?,
                set_temperature: *library.get::<FLISetTemperature>(b"FLISetTemperature\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLISetTemperature: {}", e)))?,
                get_temperature: *library.get::<FLIGetTemperature>(b"FLIGetTemperature\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIGetTemperature: {}", e)))?,
                get_cooler_power: *library.get::<FLIGetCoolerPower>(b"FLIGetCoolerPower\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIGetCoolerPower: {}", e)))?,
                grab_frame: *library.get::<FLIGrabFrame>(b"FLIGrabFrame\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIGrabFrame: {}", e)))?,
                grab_row: *library.get::<FLIGrabRow>(b"FLIGrabRow\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIGrabRow: {}", e)))?,
                set_bit_depth: *library.get::<FLISetBitDepth>(b"FLISetBitDepth\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLISetBitDepth: {}", e)))?,
                get_device_status: *library.get::<FLIGetDeviceStatus>(b"FLIGetDeviceStatus\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIGetDeviceStatus: {}", e)))?,
                get_lib_version: *library.get::<FLIGetLibVersion>(b"FLIGetLibVersion\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIGetLibVersion: {}", e)))?,
                create_list: *library.get::<FLICreateList>(b"FLICreateList\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLICreateList: {}", e)))?,
                delete_list: *library.get::<FLIDeleteList>(b"FLIDeleteList\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIDeleteList: {}", e)))?,
                list_first: *library.get::<FLIListFirst>(b"FLIListFirst\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIListFirst: {}", e)))?,
                list_next: *library.get::<FLIListNext>(b"FLIListNext\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIListNext: {}", e)))?,
                set_filter_pos: *library.get::<FLISetFilterPos>(b"FLISetFilterPos\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLISetFilterPos: {}", e)))?,
                get_filter_pos: *library.get::<FLIGetFilterPos>(b"FLIGetFilterPos\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIGetFilterPos: {}", e)))?,
                get_filter_count: *library.get::<FLIGetFilterCount>(b"FLIGetFilterCount\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIGetFilterCount: {}", e)))?,
                step_motor: *library.get::<FLIStepMotor>(b"FLIStepMotor\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIStepMotor: {}", e)))?,
                step_motor_async: *library.get::<FLIStepMotorAsync>(b"FLIStepMotorAsync\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIStepMotorAsync: {}", e)))?,
                get_stepper_position: *library.get::<FLIGetStepperPosition>(b"FLIGetStepperPosition\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIGetStepperPosition: {}", e)))?,
                get_steps_remaining: *library.get::<FLIGetStepsRemaining>(b"FLIGetStepsRemaining\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIGetStepsRemaining: {}", e)))?,
                home_focuser: *library.get::<FLIHomeFocuser>(b"FLIHomeFocuser\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIHomeFocuser: {}", e)))?,
                get_focuser_extent: *library.get::<FLIGetFocuserExtent>(b"FLIGetFocuserExtent\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIGetFocuserExtent: {}", e)))?,
                read_temperature: *library.get::<FLIReadTemperature>(b"FLIReadTemperature\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIReadTemperature: {}", e)))?,
                end_exposure: *library.get::<FLIEndExposure>(b"FLIEndExposure\0")
                    .map_err(|e| NativeError::SdkError(format!("Failed to load FLIEndExposure: {}", e)))?,
                _library: library,
            })
        }
    }
}

/// Global SDK instance
static SDK: OnceLock<Result<FliSdk, String>> = OnceLock::new();

fn get_sdk() -> Result<&'static FliSdk, NativeError> {
    SDK.get_or_init(|| FliSdk::load().map_err(|e| e.to_string()))
        .as_ref()
        .map_err(|e| NativeError::SdkError(e.clone()))
}

fn check_fli_error(result: c_long, context: &str) -> Result<(), NativeError> {
    if result == 0 {
        Ok(())
    } else {
        tracing::error!(
            "FLI SDK error during '{}': error code {}. Check device connection and driver installation.",
            context, result
        );
        Err(NativeError::SdkError(format!(
            "FLI {}: error code {}. Ensure device is connected and libfli driver is installed.",
            context, result
        )))
    }
}

// =============================================================================
// Discovery
// =============================================================================

/// Discovered FLI device info
#[derive(Debug, Clone)]
pub struct FliDiscoveryInfo {
    pub device_path: String,
    pub name: String,
    pub serial_number: Option<String>,
    pub device_type: FliDeviceType,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum FliDeviceType {
    Camera,
    Focuser,
    FilterWheel,
}

/// Discover connected FLI cameras
pub async fn discover_cameras() -> Result<Vec<FliDiscoveryInfo>, NativeError> {
    discover_devices_by_type(FLIDEVICE_CAMERA, FliDeviceType::Camera).await
}

/// Discover connected FLI focusers
pub async fn discover_focusers() -> Result<Vec<FliDiscoveryInfo>, NativeError> {
    discover_devices_by_type(FLIDEVICE_FOCUSER, FliDeviceType::Focuser).await
}

/// Discover connected FLI filter wheels
pub async fn discover_filter_wheels() -> Result<Vec<FliDiscoveryInfo>, NativeError> {
    discover_devices_by_type(FLIDEVICE_FILTERWHEEL, FliDeviceType::FilterWheel).await
}

async fn discover_devices_by_type(device_flag: c_long, device_type: FliDeviceType) -> Result<Vec<FliDiscoveryInfo>, NativeError> {
    let sdk = match get_sdk() {
        Ok(sdk) => sdk,
        Err(_) => return Ok(Vec::new()),
    };

    // Acquire global SDK mutex for thread safety
    let _lock = fli_mutex().lock().await;

    let domain = FLIDOMAIN_USB | device_flag;
    let mut devices = Vec::new();

    // Create device list
    let result = unsafe { (sdk.create_list)(domain) };
    if result != 0 {
        return Ok(Vec::new());
    }

    // Iterate through devices
    let mut dev_domain: c_long = 0;
    let mut filename = [0i8; 256];
    let mut name_buf = [0i8; 256];

    // Get first device
    let mut result = unsafe {
        (sdk.list_first)(
            &mut dev_domain,
            filename.as_mut_ptr(),
            filename.len(),
            name_buf.as_mut_ptr(),
            name_buf.len(),
        )
    };

    while result == 0 {
        let path = unsafe { CStr::from_ptr(filename.as_ptr()) }
            .to_string_lossy()
            .to_string();
        let name = unsafe { CStr::from_ptr(name_buf.as_ptr()) }
            .to_string_lossy()
            .to_string();

        // Try to get serial number by opening device temporarily
        let serial = if !path.is_empty() {
            let path_cstr = CString::new(path.clone()).unwrap();
            let mut dev: FliDev = FLI_INVALID_DEVICE;

            if unsafe { (sdk.open)(&mut dev, path_cstr.as_ptr(), domain) } == 0 {
                let mut serial_buf = [0i8; 64];
                let serial = if unsafe { (sdk.get_serial_string)(dev, serial_buf.as_mut_ptr(), serial_buf.len()) } == 0 {
                    let s = unsafe { CStr::from_ptr(serial_buf.as_ptr()) }
                        .to_string_lossy()
                        .to_string();
                    if s.is_empty() { None } else { Some(s) }
                } else {
                    None
                };
                let _ = unsafe { (sdk.close)(dev) };
                serial
            } else {
                None
            }
        } else {
            None
        };

        devices.push(FliDiscoveryInfo {
            device_path: path,
            name,
            serial_number: serial,
            device_type,
        });

        // Get next device
        result = unsafe {
            (sdk.list_next)(
                &mut dev_domain,
                filename.as_mut_ptr(),
                filename.len(),
                name_buf.as_mut_ptr(),
                name_buf.len(),
            )
        };
    }

    // Clean up
    let _ = unsafe { (sdk.delete_list)() };

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
            let mut version_buf = [0i8; 64];
            if unsafe { (sdk.get_lib_version)(version_buf.as_mut_ptr(), version_buf.len()) } == 0 {
                let version = unsafe { CStr::from_ptr(version_buf.as_ptr()) }
                    .to_string_lossy()
                    .to_string();
                (true, format!("FLI libfli v{}", version))
            } else {
                (true, "FLI libfli (version unknown)".to_string())
            }
        }
        Err(e) => (false, format!("SDK not available: {}", e)),
    }
}

// =============================================================================
// FLI Camera Implementation
// =============================================================================

/// FLI camera native driver
pub struct FliCamera {
    device_path: String,
    device_id: String,
    name: String,
    handle: FliDev,
    connected: bool,
    capabilities: CameraCapabilities,
    sensor_info: SensorInfo,
    state: CameraState,
    // Visible area (where actual image is)
    visible_ul_x: i32,
    visible_ul_y: i32,
    visible_lr_x: i32,
    visible_lr_y: i32,
    // Current settings
    current_bin_x: i32,
    current_bin_y: i32,
    subframe: Option<SubFrame>,
    cooler_on: bool,
    target_temp: f64,
    // Exposure tracking
    exposure_duration: f64,
}

impl std::fmt::Debug for FliCamera {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("FliCamera")
            .field("device_id", &self.device_id)
            .field("name", &self.name)
            .field("device_path", &self.device_path)
            .finish()
    }
}

impl FliCamera {
    /// Create a new FLI camera instance
    pub fn new(device_path: String) -> Self {
        let device_id = device_path.replace("/", "_").replace("\\", "_");
        Self {
            device_path: device_path.clone(),
            device_id: format!("fli_{}", device_id),
            name: format!("FLI Camera"),
            handle: FLI_INVALID_DEVICE,
            connected: false,
            capabilities: CameraCapabilities::default(),
            sensor_info: SensorInfo::default(),
            state: CameraState::Idle,
            visible_ul_x: 0,
            visible_ul_y: 0,
            visible_lr_x: 0,
            visible_lr_y: 0,
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
impl NativeDevice for FliCamera {
    fn id(&self) -> &str {
        &self.device_id
    }

    fn name(&self) -> &str {
        &self.name
    }

    fn vendor(&self) -> NativeVendor {
        NativeVendor::Fli
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
        let _lock = fli_mutex().lock().await;

        // Open device
        let path_cstr = CString::new(self.device_path.clone())
            .map_err(|_| {
                tracing::error!("FLI camera device path contains null bytes: '{}'", self.device_path);
                NativeError::SdkError(format!("Invalid device path '{}' contains null bytes", self.device_path))
            })?;
        let domain = FLIDOMAIN_USB | FLIDEVICE_CAMERA;

        let result = unsafe { (sdk.open)(&mut self.handle, path_cstr.as_ptr(), domain) };
        if result != 0 {
            tracing::error!(
                "FLI Open() failed for camera at '{}'. Error code: {}. Check USB connection and driver.",
                self.device_path, result
            );
            return Err(NativeError::SdkError(format!(
                "Failed to open FLI camera at '{}'. SDK error: {}. Ensure camera is connected and not in use.",
                self.device_path, result
            )));
        }

        // Get model name
        let mut model_buf = [0i8; 128];
        if unsafe { (sdk.get_model)(self.handle, model_buf.as_mut_ptr(), model_buf.len()) } == 0 {
            self.name = unsafe { CStr::from_ptr(model_buf.as_ptr()) }
                .to_string_lossy()
                .to_string();
        }

        // Get pixel size
        let mut pixel_x: c_double = 0.0;
        let mut pixel_y: c_double = 0.0;
        let _ = unsafe { (sdk.get_pixel_size)(self.handle, &mut pixel_x, &mut pixel_y) };

        // Get visible area (the actual image area)
        let mut ul_x: c_long = 0;
        let mut ul_y: c_long = 0;
        let mut lr_x: c_long = 0;
        let mut lr_y: c_long = 0;
        let result = unsafe { (sdk.get_visible_area)(self.handle, &mut ul_x, &mut ul_y, &mut lr_x, &mut lr_y) };
        check_fli_error(result, "get visible area")?;

        self.visible_ul_x = ul_x as i32;
        self.visible_ul_y = ul_y as i32;
        self.visible_lr_x = lr_x as i32;
        self.visible_lr_y = lr_y as i32;

        let width = (lr_x - ul_x) as u32;
        let height = (lr_y - ul_y) as u32;

        // Set sensor info
        self.sensor_info = SensorInfo {
            width,
            height,
            pixel_size_x: pixel_x,
            pixel_size_y: pixel_y,
            max_adu: 65535,
            bit_depth: 16,
            color: false, // FLI cameras are typically monochrome
            bayer_pattern: None,
        };

        // Set capabilities
        self.capabilities = CameraCapabilities {
            can_cool: true, // FLI cameras typically have cooling
            can_set_gain: false, // FLI doesn't expose gain control
            can_set_offset: false,
            can_set_binning: true,
            can_subframe: true,
            has_shutter: true,
            has_guider_port: false,
            max_bin_x: 16, // FLI typically supports up to 16x binning
            max_bin_y: 16,
            supports_readout_modes: false,
        };

        // Set 16-bit mode
        let _ = unsafe { (sdk.set_bit_depth)(self.handle, FLI_MODE_16BIT) };

        // Set full frame
        let result = unsafe { (sdk.set_image_area)(self.handle, ul_x, ul_y, lr_x, lr_y) };
        check_fli_error(result, "set image area")?;

        self.connected = true;
        self.state = CameraState::Idle;

        tracing::info!(
            "Connected to FLI camera: {} ({}x{})",
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
        let _lock = fli_mutex().lock().await;

        // Cancel any exposure
        let _ = unsafe { (sdk.cancel_exposure)(self.handle) };

        // Close device
        let result = unsafe { (sdk.close)(self.handle) };
        check_fli_error(result, "close camera")?;

        self.handle = FLI_INVALID_DEVICE;
        self.connected = false;
        self.state = CameraState::Idle;

        tracing::info!("Disconnected from FLI camera: {}", self.name);

        Ok(())
    }
}

#[async_trait]
impl NativeCamera for FliCamera {
    fn capabilities(&self) -> CameraCapabilities {
        self.capabilities.clone()
    }

    async fn get_status(&self) -> Result<CameraStatus, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = fli_mutex().lock().await;

        // Get temperature
        let sensor_temp = {
            let mut temp: c_double = 0.0;
            if unsafe { (sdk.get_temperature)(self.handle, &mut temp) } == 0 {
                Some(temp)
            } else {
                None
            }
        };

        // Get cooler power
        let cooler_power = {
            let mut power: c_double = 0.0;
            if unsafe { (sdk.get_cooler_power)(self.handle, &mut power) } == 0 {
                Some(power)
            } else {
                None
            }
        };

        // Get exposure remaining
        let exposure_remaining = if self.state == CameraState::Exposing {
            let mut timeleft: c_long = 0;
            if unsafe { (sdk.get_exposure_status)(self.handle, &mut timeleft) } == 0 {
                Some(timeleft as f64 / 1000.0) // Convert from ms to seconds
            } else {
                None
            }
        } else {
            None
        };

        Ok(CameraStatus {
            state: self.state,
            sensor_temp,
            cooler_power,
            target_temp: Some(self.target_temp),
            cooler_on: self.cooler_on,
            gain: 0,
            offset: 0,
            bin_x: self.current_bin_x,
            bin_y: self.current_bin_y,
            exposure_remaining,
        })
    }

    async fn start_exposure(&mut self, params: ExposureParams) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        // Set binning (has its own mutex lock)
        self.set_binning(params.bin_x, params.bin_y).await?;

        // Set subframe (has its own mutex lock)
        self.set_subframe(params.subframe.clone()).await?;

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = fli_mutex().lock().await;

        // Set frame type (normal mode by default - dark frames handled at higher level)
        let result = unsafe { (sdk.set_frame_type)(self.handle, FLI_FRAME_TYPE_NORMAL) };
        check_fli_error(result, "set frame type")?;

        // Set exposure time (in milliseconds)
        let exposure_ms = (params.duration_secs * 1000.0) as c_long;
        let result = unsafe { (sdk.set_exposure_time)(self.handle, exposure_ms) };
        check_fli_error(result, "set exposure time")?;

        // Start exposure
        let result = unsafe { (sdk.expose_frame)(self.handle) };
        check_fli_error(result, "expose frame")?;

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
        let _lock = fli_mutex().lock().await;

        let result = unsafe { (sdk.cancel_exposure)(self.handle) };
        check_fli_error(result, "cancel exposure")?;

        self.state = CameraState::Idle;
        Ok(())
    }

    async fn is_exposure_complete(&self) -> Result<bool, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = fli_mutex().lock().await;

        // Check device status
        let mut status: c_long = 0;
        let result = unsafe { (sdk.get_device_status)(self.handle, &mut status) };
        check_fli_error(result, "get device status")?;

        // Check if data is ready
        Ok((status & FLI_CAMERA_DATA_READY) != 0)
    }

    async fn download_image(&mut self) -> Result<ImageData, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = fli_mutex().lock().await;

        self.state = CameraState::Downloading;

        // Calculate image dimensions based on subframe and binning
        let (width, height) = if let Some(sf) = &self.subframe {
            (sf.width as usize / self.current_bin_x as usize,
             sf.height as usize / self.current_bin_y as usize)
        } else {
            ((self.visible_lr_x - self.visible_ul_x) as usize / self.current_bin_x as usize,
             (self.visible_lr_y - self.visible_ul_y) as usize / self.current_bin_y as usize)
        };

        // Allocate buffer (16-bit = 2 bytes per pixel)
        let row_bytes = width * 2;
        let mut data: Vec<u16> = vec![0u16; width * height];

        // Read image row by row (FLI style)
        for row in 0..height {
            let row_ptr = unsafe { data.as_mut_ptr().add(row * width) as *mut u8 };
            let result = unsafe { (sdk.grab_row)(self.handle, row_ptr, row_bytes) };
            if result != 0 {
                tracing::error!(
                    "FLI GrabRow() failed for camera '{}'. Row: {}/{}, width: {} bytes, error code: {}",
                    self.name, row, height, row_bytes, result
                );
                self.state = CameraState::Error;
                return Err(NativeError::SdkError(format!(
                    "Failed to grab row {} of {} from FLI camera '{}'. SDK error: {}. Image download interrupted.",
                    row, height, self.name, result
                )));
            }
        }

        // End exposure
        let _ = unsafe { (sdk.end_exposure)(self.handle) };

        // Get temperature for metadata
        let temperature = {
            let mut temp: c_double = 0.0;
            if unsafe { (sdk.get_temperature)(self.handle, &mut temp) } == 0 {
                Some(temp)
            } else {
                None
            }
        };

        let metadata = ImageMetadata {
            exposure_time: self.exposure_duration,
            gain: 0,
            offset: 0,
            bin_x: self.current_bin_x,
            bin_y: self.current_bin_y,
            temperature,
            timestamp: chrono::Utc::now(),
            subframe: self.subframe.clone(),
            readout_mode: None,
            vendor_data: VendorFeatures::default(),
        };

        self.state = CameraState::Idle;

        Ok(ImageData {
            width: width as u32,
            height: height as u32,
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

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = fli_mutex().lock().await;

        if enabled {
            let result = unsafe { (sdk.set_temperature)(self.handle, target_temp) };
            check_fli_error(result, "set temperature")?;
        } else {
            // Set to a high temperature to effectively disable cooling
            let result = unsafe { (sdk.set_temperature)(self.handle, 25.0) };
            check_fli_error(result, "set temperature")?;
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
        let _lock = fli_mutex().lock().await;

        let mut temp: c_double = 0.0;

        let result = unsafe { (sdk.get_temperature)(self.handle, &mut temp) };
        check_fli_error(result, "get temperature")?;

        Ok(temp)
    }

    async fn get_cooler_power(&self) -> Result<f64, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = fli_mutex().lock().await;

        let mut power: c_double = 0.0;

        let result = unsafe { (sdk.get_cooler_power)(self.handle, &mut power) };
        check_fli_error(result, "get cooler power")?;

        Ok(power)
    }

    async fn set_gain(&mut self, _gain: i32) -> Result<(), NativeError> {
        // FLI doesn't support gain control
        Ok(())
    }

    async fn get_gain(&self) -> Result<i32, NativeError> {
        Ok(0)
    }

    async fn set_offset(&mut self, _offset: i32) -> Result<(), NativeError> {
        // FLI doesn't support offset control
        Ok(())
    }

    async fn get_offset(&self) -> Result<i32, NativeError> {
        Ok(0)
    }

    async fn set_binning(&mut self, bin_x: i32, bin_y: i32) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = fli_mutex().lock().await;

        let result = unsafe { (sdk.set_hbin)(self.handle, bin_x as c_long) };
        check_fli_error(result, "set horizontal binning")?;

        let result = unsafe { (sdk.set_vbin)(self.handle, bin_y as c_long) };
        check_fli_error(result, "set vertical binning")?;

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
        let _lock = fli_mutex().lock().await;

        let (ul_x, ul_y, lr_x, lr_y) = match &subframe {
            Some(sf) => (
                self.visible_ul_x + sf.start_x as i32,
                self.visible_ul_y + sf.start_y as i32,
                self.visible_ul_x + sf.start_x as i32 + sf.width as i32,
                self.visible_ul_y + sf.start_y as i32 + sf.height as i32,
            ),
            None => (
                self.visible_ul_x,
                self.visible_ul_y,
                self.visible_lr_x,
                self.visible_lr_y,
            ),
        };

        let result = unsafe {
            (sdk.set_image_area)(self.handle, ul_x as c_long, ul_y as c_long, lr_x as c_long, lr_y as c_long)
        };
        check_fli_error(result, "set image area")?;

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

        // FLI cameras are primarily CCD cameras without adjustable gain.
        // This returns a nominal range for compatibility.
        Err(NativeError::NotSupported)
    }

    async fn get_offset_range(&self) -> Result<(i32, i32), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        // FLI cameras typically don't have user-adjustable offset.
        Err(NativeError::NotSupported)
    }
}

// =============================================================================
// FLI Focuser Implementation
// =============================================================================

/// FLI focuser native driver
pub struct FliFocuser {
    device_path: String,
    device_id: String,
    name: String,
    handle: FliDev,
    connected: bool,
    max_position: i32,
}

impl std::fmt::Debug for FliFocuser {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("FliFocuser")
            .field("device_id", &self.device_id)
            .field("name", &self.name)
            .finish()
    }
}

impl FliFocuser {
    pub fn new(device_path: String) -> Self {
        let device_id = device_path.replace("/", "_").replace("\\", "_");
        Self {
            device_path: device_path.clone(),
            device_id: format!("fli_focuser_{}", device_id),
            name: "FLI Focuser".to_string(),
            handle: FLI_INVALID_DEVICE,
            connected: false,
            max_position: 50000,
        }
    }
}

#[async_trait]
impl NativeDevice for FliFocuser {
    fn id(&self) -> &str {
        &self.device_id
    }

    fn name(&self) -> &str {
        &self.name
    }

    fn vendor(&self) -> NativeVendor {
        NativeVendor::Fli
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
        let _lock = fli_mutex().lock().await;

        let path_cstr = CString::new(self.device_path.clone())
            .map_err(|_| {
                tracing::error!("FLI focuser device path contains null bytes: '{}'", self.device_path);
                NativeError::SdkError(format!("Invalid device path '{}' contains null bytes", self.device_path))
            })?;
        let domain = FLIDOMAIN_USB | FLIDEVICE_FOCUSER;

        let result = unsafe { (sdk.open)(&mut self.handle, path_cstr.as_ptr(), domain) };
        if result != 0 {
            tracing::error!(
                "FLI Open() failed for focuser at '{}'. Error code: {}. Check USB connection.",
                self.device_path, result
            );
            return Err(NativeError::SdkError(format!(
                "Failed to open FLI focuser at '{}'. SDK error: {}",
                self.device_path, result
            )));
        }

        // Get model name
        let mut model_buf = [0i8; 128];
        if unsafe { (sdk.get_model)(self.handle, model_buf.as_mut_ptr(), model_buf.len()) } == 0 {
            self.name = unsafe { CStr::from_ptr(model_buf.as_ptr()) }
                .to_string_lossy()
                .to_string();
        }

        // Get max position
        let mut extent: c_long = 0;
        if unsafe { (sdk.get_focuser_extent)(self.handle, &mut extent) } == 0 {
            self.max_position = extent as i32;
        }

        self.connected = true;
        tracing::info!("Connected to FLI focuser: {}", self.name);

        Ok(())
    }

    async fn disconnect(&mut self) -> Result<(), NativeError> {
        if !self.connected {
            return Ok(());
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = fli_mutex().lock().await;

        let result = unsafe { (sdk.close)(self.handle) };
        check_fli_error(result, "close focuser")?;

        self.handle = FLI_INVALID_DEVICE;
        self.connected = false;

        Ok(())
    }
}

#[async_trait]
impl NativeFocuser for FliFocuser {
    async fn get_position(&self) -> Result<i32, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = fli_mutex().lock().await;

        let mut position: c_long = 0;

        let result = unsafe { (sdk.get_stepper_position)(self.handle, &mut position) };
        check_fli_error(result, "get position")?;

        Ok(position as i32)
    }

    async fn move_to(&mut self, position: i32) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = fli_mutex().lock().await;

        // Get current position
        let mut current: c_long = 0;
        let result = unsafe { (sdk.get_stepper_position)(self.handle, &mut current) };
        check_fli_error(result, "get current position")?;

        // Calculate steps needed
        let steps = position as c_long - current;

        // Move asynchronously
        let result = unsafe { (sdk.step_motor_async)(self.handle, steps) };
        check_fli_error(result, "move focuser")?;

        Ok(())
    }

    async fn move_relative(&mut self, steps: i32) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = fli_mutex().lock().await;

        let result = unsafe { (sdk.step_motor_async)(self.handle, steps as c_long) };
        check_fli_error(result, "move focuser relative")?;

        Ok(())
    }

    async fn is_moving(&self) -> Result<bool, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = fli_mutex().lock().await;

        let mut steps_remaining: c_long = 0;

        let result = unsafe { (sdk.get_steps_remaining)(self.handle, &mut steps_remaining) };
        check_fli_error(result, "get steps remaining")?;

        Ok(steps_remaining != 0)
    }

    async fn halt(&mut self) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        // FLI doesn't have a direct halt - move 0 steps
        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = fli_mutex().lock().await;

        let result = unsafe { (sdk.step_motor)(self.handle, 0) };
        check_fli_error(result, "halt focuser")?;

        Ok(())
    }

    fn get_max_position(&self) -> i32 {
        self.max_position
    }

    fn get_step_size(&self) -> f64 {
        1.0 // FLI focusers use step units
    }

    async fn get_temperature(&self) -> Result<Option<f64>, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = fli_mutex().lock().await;

        let mut temp: c_double = 0.0;

        // FLI_TEMPERATURE_EXTERNAL = 0x0001
        if unsafe { (sdk.read_temperature)(self.handle, 0x0001, &mut temp) } == 0 {
            Ok(Some(temp))
        } else {
            Ok(None)
        }
    }
}

// =============================================================================
// FLI Filter Wheel Implementation
// =============================================================================

/// FLI filter wheel native driver
pub struct FliFilterWheel {
    device_path: String,
    device_id: String,
    name: String,
    handle: FliDev,
    connected: bool,
    filter_count: i32,
}

impl std::fmt::Debug for FliFilterWheel {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("FliFilterWheel")
            .field("device_id", &self.device_id)
            .field("name", &self.name)
            .finish()
    }
}

impl FliFilterWheel {
    pub fn new(device_path: String) -> Self {
        let device_id = device_path.replace("/", "_").replace("\\", "_");
        Self {
            device_path: device_path.clone(),
            device_id: format!("fli_fw_{}", device_id),
            name: "FLI Filter Wheel".to_string(),
            handle: FLI_INVALID_DEVICE,
            connected: false,
            filter_count: 0,
        }
    }
}

#[async_trait]
impl NativeDevice for FliFilterWheel {
    fn id(&self) -> &str {
        &self.device_id
    }

    fn name(&self) -> &str {
        &self.name
    }

    fn vendor(&self) -> NativeVendor {
        NativeVendor::Fli
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
        let _lock = fli_mutex().lock().await;

        let path_cstr = CString::new(self.device_path.clone())
            .map_err(|_| {
                tracing::error!("FLI filter wheel device path contains null bytes: '{}'", self.device_path);
                NativeError::SdkError(format!("Invalid device path '{}' contains null bytes", self.device_path))
            })?;
        let domain = FLIDOMAIN_USB | FLIDEVICE_FILTERWHEEL;

        let result = unsafe { (sdk.open)(&mut self.handle, path_cstr.as_ptr(), domain) };
        if result != 0 {
            tracing::error!(
                "FLI Open() failed for filter wheel at '{}'. Error code: {}. Check USB connection.",
                self.device_path, result
            );
            return Err(NativeError::SdkError(format!(
                "Failed to open FLI filter wheel at '{}'. SDK error: {}",
                self.device_path, result
            )));
        }

        // Get model name
        let mut model_buf = [0i8; 128];
        if unsafe { (sdk.get_model)(self.handle, model_buf.as_mut_ptr(), model_buf.len()) } == 0 {
            self.name = unsafe { CStr::from_ptr(model_buf.as_ptr()) }
                .to_string_lossy()
                .to_string();
        }

        // Get filter count
        let mut count: c_long = 0;
        if unsafe { (sdk.get_filter_count)(self.handle, &mut count) } == 0 {
            self.filter_count = count as i32;
        }

        self.connected = true;
        tracing::info!("Connected to FLI filter wheel: {} ({} positions)", self.name, self.filter_count);

        Ok(())
    }

    async fn disconnect(&mut self) -> Result<(), NativeError> {
        if !self.connected {
            return Ok(());
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = fli_mutex().lock().await;

        let result = unsafe { (sdk.close)(self.handle) };
        check_fli_error(result, "close filter wheel")?;

        self.handle = FLI_INVALID_DEVICE;
        self.connected = false;

        Ok(())
    }
}

#[async_trait]
impl NativeFilterWheel for FliFilterWheel {
    async fn get_position(&self) -> Result<i32, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = fli_mutex().lock().await;

        let mut position: c_long = 0;

        let result = unsafe { (sdk.get_filter_pos)(self.handle, &mut position) };
        check_fli_error(result, "get filter position")?;

        Ok(position as i32)
    }

    async fn move_to_position(&mut self, position: i32) -> Result<(), NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        if position < 0 || position >= self.filter_count {
            return Err(NativeError::InvalidParameter(format!(
                "Invalid filter position: {} (max {})",
                position, self.filter_count - 1
            )));
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = fli_mutex().lock().await;

        let result = unsafe { (sdk.set_filter_pos)(self.handle, position as c_long) };
        check_fli_error(result, "set filter position")?;

        Ok(())
    }

    async fn is_moving(&self) -> Result<bool, NativeError> {
        if !self.connected {
            return Err(NativeError::NotConnected);
        }

        let sdk = get_sdk()?;

        // Acquire global SDK mutex for thread safety
        let _lock = fli_mutex().lock().await;

        let mut status: c_long = 0;

        let result = unsafe { (sdk.get_device_status)(self.handle, &mut status) };
        check_fli_error(result, "get device status")?;

        // Check if moving (bits 0-2 indicate movement direction)
        Ok((status & 0x07) != 0)
    }

    fn get_filter_count(&self) -> i32 {
        self.filter_count
    }

    async fn get_filter_names(&self) -> Result<Vec<String>, NativeError> {
        // FLI SDK doesn't store filter names, return generic names
        let names: Vec<String> = (0..self.filter_count)
            .map(|i| format!("Filter {}", i + 1))
            .collect();
        Ok(names)
    }

    async fn set_filter_name(&mut self, _position: i32, _name: String) -> Result<(), NativeError> {
        // FLI SDK doesn't support storing filter names
        Ok(())
    }
}
