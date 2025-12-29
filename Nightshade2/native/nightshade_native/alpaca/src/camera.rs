//! Alpaca Camera API implementation

use crate::{AlpacaClient, AlpacaClientBuilder, AlpacaDevice, AlpacaDeviceType, AlpacaError, TimeoutConfig, RetryConfig};
use std::time::Duration;

/// Camera state enum matching ASCOM CameraState
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum CameraState {
    Idle = 0,
    Waiting = 1,
    Exposing = 2,
    Reading = 3,
    Download = 4,
    Error = 5,
}

impl From<i32> for CameraState {
    fn from(value: i32) -> Self {
        match value {
            0 => CameraState::Idle,
            1 => CameraState::Waiting,
            2 => CameraState::Exposing,
            3 => CameraState::Reading,
            4 => CameraState::Download,
            _ => CameraState::Error,
        }
    }
}

impl std::fmt::Display for CameraState {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            CameraState::Idle => write!(f, "Idle"),
            CameraState::Waiting => write!(f, "Waiting"),
            CameraState::Exposing => write!(f, "Exposing"),
            CameraState::Reading => write!(f, "Reading"),
            CameraState::Download => write!(f, "Downloading"),
            CameraState::Error => write!(f, "Error"),
        }
    }
}

/// Sensor type enum matching ASCOM SensorType
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum SensorType {
    Monochrome = 0,
    Color = 1,
    RGGB = 2,
    CMYG = 3,
    CMYG2 = 4,
    LRGB = 5,
}

impl From<i32> for SensorType {
    fn from(value: i32) -> Self {
        match value {
            0 => SensorType::Monochrome,
            1 => SensorType::Color,
            2 => SensorType::RGGB,
            3 => SensorType::CMYG,
            4 => SensorType::CMYG2,
            5 => SensorType::LRGB,
            _ => SensorType::Monochrome,
        }
    }
}

/// Camera status aggregate for parallel status query
#[derive(Debug, Clone)]
pub struct CameraStatus {
    pub state: CameraState,
    pub connected: bool,
    pub image_ready: bool,
    pub percent_completed: Option<i32>,
    pub ccd_temperature: Option<f64>,
    pub cooler_on: Option<bool>,
    pub cooler_power: Option<f64>,
    pub bin_x: i32,
    pub bin_y: i32,
}

/// Camera capabilities for determining what features are available
#[derive(Debug, Clone)]
pub struct CameraCapabilities {
    pub can_abort_exposure: bool,
    pub can_stop_exposure: bool,
    pub can_asymmetric_bin: bool,
    pub can_pulse_guide: bool,
    pub can_fast_readout: bool,
    pub can_set_ccd_temperature: bool,
    pub can_get_cooler_power: bool,
    pub has_shutter: bool,
    pub max_bin_x: i32,
    pub max_bin_y: i32,
}

/// Camera sensor information
#[derive(Debug, Clone)]
pub struct CameraSensorInfo {
    pub camera_x_size: i32,
    pub camera_y_size: i32,
    pub pixel_size_x: f64,
    pub pixel_size_y: f64,
    pub max_adu: i32,
    pub sensor_type: SensorType,
    pub sensor_name: String,
    pub bayer_offset_x: Option<i32>,
    pub bayer_offset_y: Option<i32>,
}

/// Alpaca Camera client
pub struct AlpacaCamera {
    client: AlpacaClient,
}

impl AlpacaCamera {
    /// Create a new Alpaca camera client
    pub fn new(device: &AlpacaDevice) -> Self {
        assert_eq!(device.device_type, AlpacaDeviceType::Camera);
        Self {
            client: AlpacaClient::new(device),
        }
    }

    /// Create a camera client with custom configuration
    pub fn with_config(
        device: &AlpacaDevice,
        timeout_config: TimeoutConfig,
        retry_config: RetryConfig,
    ) -> Self {
        assert_eq!(device.device_type, AlpacaDeviceType::Camera);
        Self {
            client: AlpacaClient::with_config(device, timeout_config, retry_config),
        }
    }

    /// Create from server details
    pub fn from_server(base_url: &str, device_number: u32) -> Self {
        let device = AlpacaDevice {
            device_type: AlpacaDeviceType::Camera,
            device_number,
            server_name: String::new(),
            manufacturer: String::new(),
            device_name: String::new(),
            unique_id: String::new(),
            base_url: base_url.to_string(),
        };
        Self::new(&device)
    }

    /// Create a builder for custom configuration
    pub fn builder(device: AlpacaDevice) -> AlpacaClientBuilder {
        AlpacaClientBuilder::new(device)
    }

    /// Get access to the underlying client
    pub fn client(&self) -> &AlpacaClient {
        &self.client
    }

    /// Get the base URL for this device
    pub fn base_url(&self) -> &str {
        self.client.base_url()
    }

    /// Get the device number for this device
    pub fn device_number(&self) -> u32 {
        self.client.device_number()
    }

    // Connection methods

    pub async fn connect(&self) -> Result<(), String> {
        self.client.connect().await
    }

    pub async fn disconnect(&self) -> Result<(), String> {
        self.client.disconnect().await
    }

    pub async fn is_connected(&self) -> Result<bool, String> {
        self.client.is_connected().await
    }

    /// Validate connection is alive
    pub async fn validate_connection(&self) -> Result<bool, AlpacaError> {
        self.client.validate_connection().await
    }

    /// Send heartbeat and get round-trip time
    pub async fn heartbeat(&self) -> Result<u64, AlpacaError> {
        self.client.heartbeat().await
    }

    // Camera information

    pub async fn name(&self) -> Result<String, String> {
        self.client.get_name().await
    }

    pub async fn description(&self) -> Result<String, String> {
        self.client.get_description().await
    }

    pub async fn camera_x_size(&self) -> Result<i32, String> {
        self.client.get("cameraxsize").await
    }

    pub async fn camera_y_size(&self) -> Result<i32, String> {
        self.client.get("cameraysize").await
    }

    pub async fn pixel_size_x(&self) -> Result<f64, String> {
        self.client.get("pixelsizex").await
    }

    pub async fn pixel_size_y(&self) -> Result<f64, String> {
        self.client.get("pixelsizey").await
    }

    pub async fn max_adu(&self) -> Result<i32, String> {
        self.client.get("maxadu").await
    }

    pub async fn sensor_type(&self) -> Result<i32, String> {
        self.client.get("sensortype").await
    }

    pub async fn sensor_name(&self) -> Result<String, String> {
        self.client.get("sensorname").await
    }

    // Binning

    pub async fn max_bin_x(&self) -> Result<i32, String> {
        self.client.get("maxbinx").await
    }

    pub async fn max_bin_y(&self) -> Result<i32, String> {
        self.client.get("maxbiny").await
    }

    pub async fn bin_x(&self) -> Result<i32, String> {
        self.client.get("binx").await
    }

    pub async fn bin_y(&self) -> Result<i32, String> {
        self.client.get("biny").await
    }

    pub async fn set_bin_x(&self, value: i32) -> Result<(), String> {
        self.client.put("binx", &[("BinX", &value.to_string())]).await
    }

    pub async fn set_bin_y(&self, value: i32) -> Result<(), String> {
        self.client.put("biny", &[("BinY", &value.to_string())]).await
    }

    // Cooling

    pub async fn can_set_ccd_temperature(&self) -> Result<bool, String> {
        self.client.get("cansetccdtemperature").await
    }

    pub async fn ccd_temperature(&self) -> Result<f64, String> {
        self.client.get("ccdtemperature").await
    }

    pub async fn set_ccd_temperature(&self, temp: f64) -> Result<(), String> {
        self.client.put("setccdtemperature", &[("SetCCDTemperature", &temp.to_string())]).await
    }

    pub async fn cooler_on(&self) -> Result<bool, String> {
        self.client.get("cooleron").await
    }

    pub async fn set_cooler_on(&self, on: bool) -> Result<(), String> {
        self.client.put("cooleron", &[("CoolerOn", &on.to_string())]).await
    }

    pub async fn cooler_power(&self) -> Result<f64, String> {
        self.client.get("coolerpower").await
    }

    pub async fn heat_sink_temperature(&self) -> Result<f64, String> {
        self.client.get("heatsinktemperature").await
    }

    // Gain and offset

    pub async fn can_get_cooler_power(&self) -> Result<bool, String> {
        self.client.get("cangetcoolerpower").await
    }

    pub async fn gain(&self) -> Result<i32, String> {
        self.client.get("gain").await
    }

    pub async fn set_gain(&self, gain: i32) -> Result<(), String> {
        self.client.put("gain", &[("Gain", &gain.to_string())]).await
    }

    pub async fn gain_min(&self) -> Result<i32, String> {
        self.client.get("gainmin").await
    }

    pub async fn gain_max(&self) -> Result<i32, String> {
        self.client.get("gainmax").await
    }

    pub async fn offset(&self) -> Result<i32, String> {
        self.client.get("offset").await
    }

    pub async fn set_offset(&self, offset: i32) -> Result<(), String> {
        self.client.put("offset", &[("Offset", &offset.to_string())]).await
    }

    pub async fn offset_min(&self) -> Result<i32, String> {
        self.client.get("offsetmin").await
    }

    pub async fn offset_max(&self) -> Result<i32, String> {
        self.client.get("offsetmax").await
    }

    pub async fn bayer_offset_x(&self) -> Result<i32, String> {
        self.client.get("bayeroffsetx").await
    }

    pub async fn bayer_offset_y(&self) -> Result<i32, String> {
        self.client.get("bayeroffsety").await
    }

    // Exposure state

    pub async fn camera_state(&self) -> Result<CameraState, String> {
        let state: i32 = self.client.get("camerastate").await?;
        Ok(CameraState::from(state))
    }

    pub async fn image_ready(&self) -> Result<bool, String> {
        self.client.get("imageready").await
    }

    pub async fn is_pulse_guiding(&self) -> Result<bool, String> {
        self.client.get("ispulseguiding").await
    }

    pub async fn percent_completed(&self) -> Result<i32, String> {
        self.client.get("percentcompleted").await
    }

    // Exposure control

    pub async fn start_exposure(&self, duration: f64, light: bool) -> Result<(), String> {
        self.client.put("startexposure", &[
            ("Duration", &duration.to_string()),
            ("Light", &light.to_string()),
        ]).await
    }

    pub async fn abort_exposure(&self) -> Result<(), String> {
        self.client.put("abortexposure", &[]).await
    }

    pub async fn stop_exposure(&self) -> Result<(), String> {
        self.client.put("stopexposure", &[]).await
    }

    // Subframe

    pub async fn start_x(&self) -> Result<i32, String> {
        self.client.get("startx").await
    }

    pub async fn start_y(&self) -> Result<i32, String> {
        self.client.get("starty").await
    }

    pub async fn num_x(&self) -> Result<i32, String> {
        self.client.get("numx").await
    }

    pub async fn num_y(&self) -> Result<i32, String> {
        self.client.get("numy").await
    }

    pub async fn set_start_x(&self, value: i32) -> Result<(), String> {
        self.client.put("startx", &[("StartX", &value.to_string())]).await
    }

    pub async fn set_start_y(&self, value: i32) -> Result<(), String> {
        self.client.put("starty", &[("StartY", &value.to_string())]).await
    }

    pub async fn set_num_x(&self, value: i32) -> Result<(), String> {
        self.client.put("numx", &[("NumX", &value.to_string())]).await
    }

    pub async fn set_num_y(&self, value: i32) -> Result<(), String> {
        self.client.put("numy", &[("NumY", &value.to_string())]).await
    }

    // Last exposure info

    pub async fn last_exposure_start_time(&self) -> Result<String, String> {
        self.client.get("lastexposurestarttime").await
    }

    pub async fn last_exposure_duration(&self) -> Result<f64, String> {
        self.client.get("lastexposureduration").await
    }

    // Capabilities

    pub async fn can_abort_exposure(&self) -> Result<bool, String> {
        self.client.get("canabortexposure").await
    }

    pub async fn can_stop_exposure(&self) -> Result<bool, String> {
        self.client.get("canstopexposure").await
    }

    pub async fn can_asymmetric_bin(&self) -> Result<bool, String> {
        self.client.get("canasymmetricbin").await
    }

    pub async fn can_pulse_guide(&self) -> Result<bool, String> {
        self.client.get("canpulseguide").await
    }

    pub async fn can_fast_readout(&self) -> Result<bool, String> {
        self.client.get("canfastreadout").await
    }

    pub async fn has_shutter(&self) -> Result<bool, String> {
        self.client.get("hasshutter").await
    }

    // Image retrieval - returns 2D array of pixel values
    // The Alpaca imagearray endpoint returns a JSON object with:
    // - Value: 2D/3D array of i32 pixel values
    // - Type: data type (1=Int16, 2=Int32, etc.)
    // - Rank: 2 for mono, 3 for color
    pub async fn image_array(&self) -> Result<String, String> {
        self.client.get("imagearray").await
    }

    /// Download image as parsed pixel data
    /// Returns (width, height, data as u16 vec)
    /// Uses very long timeout (configurable, defaults to 15 minutes for large images)
    pub async fn download_image_data(&self) -> Result<(u32, u32, Vec<u16>), String> {
        self.download_image_data_typed().await.map_err(|e| e.to_string())
    }

    /// Download image with typed error handling
    /// Uses configurable timeout for large image downloads
    pub async fn download_image_data_typed(&self) -> Result<(u32, u32, Vec<u16>), AlpacaError> {
        // Get image dimensions
        let width = self.num_x().await.map_err(AlpacaError::OperationFailed)? as u32;
        let height = self.num_y().await.map_err(AlpacaError::OperationFailed)? as u32;

        // Calculate expected download size to estimate timeout
        // Assume worst case: 16-bit pixels, JSON overhead (~3x raw size for numeric encoding)
        let estimated_bytes = (width as u64) * (height as u64) * 2 * 3;

        // Use very long timeout for image downloads
        // At minimum 10MB/s network speed, allow extra margin
        let timeout_ms = self.client.timeout_config().very_long_operation_ms;

        // Fetch the raw imagearray response
        let (client_id, transaction_id) = crate::client::get_client_transaction();
        let url = format!(
            "{}/api/v1/camera/{}/imagearray?ClientID={}&ClientTransactionID={}",
            self.client.base_url(),
            self.client.device_number(),
            client_id,
            transaction_id
        );

        let http_client = reqwest::Client::builder()
            .timeout(Duration::from_millis(timeout_ms))
            .build()
            .map_err(|e| AlpacaError::RequestFailed(e.to_string()))?;

        let response = http_client.get(&url)
            .send()
            .await
            .map_err(|e| {
                if e.is_timeout() {
                    AlpacaError::timeout(
                        format!("imagearray download ({}x{}, ~{} MB)", width, height, estimated_bytes / 1_000_000),
                        timeout_ms
                    )
                } else {
                    AlpacaError::from(e)
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

        let response_text = response.text().await
            .map_err(|e| AlpacaError::RequestFailed(format!("Failed to read image array response: {}", e)))?;

        // Parse the JSON response
        let json_value: serde_json::Value = serde_json::from_str(&response_text)
            .map_err(|e| AlpacaError::ParseError(format!("Failed to parse image array JSON: {}", e)))?;

        // Check for errors
        if let Some(error_num) = json_value.get("ErrorNumber").and_then(|v| v.as_i64()) {
            if error_num != 0 {
                let error_msg = json_value.get("ErrorMessage")
                    .and_then(|v| v.as_str())
                    .unwrap_or("Unknown error");
                return Err(AlpacaError::DeviceError {
                    code: error_num as i32,
                    message: error_msg.to_string(),
                });
            }
        }

        // Get the Value array
        let value = json_value.get("Value")
            .ok_or_else(|| AlpacaError::ParseError("Missing Value field in image array response".to_string()))?;

        // Parse the 2D array into a flat vector of u16
        // Alpaca returns [NumX][NumY] (column-major) but we iterate row-by-row
        let mut pixels: Vec<u16> = Vec::with_capacity((width * height) as usize);

        if let Some(outer) = value.as_array() {
            for inner in outer.iter() {
                if let Some(inner_arr) = inner.as_array() {
                    for pixel in inner_arr {
                        // Handle both integer and floating-point JSON values
                        // Alpaca Type 1,2 = integer, Type 3 = double
                        let pixel_val: i64 = if let Some(v) = pixel.as_i64() {
                            v
                        } else if let Some(v) = pixel.as_f64() {
                            v.round() as i64
                        } else {
                            0
                        };
                        // Clamp to u16 range
                        let u16_val = if pixel_val < 0 { 0 }
                            else if pixel_val > 65535 { 65535 }
                            else { pixel_val as u16 };
                        pixels.push(u16_val);
                    }
                }
            }
        } else {
            return Err(AlpacaError::ParseError("Image array Value is not an array".to_string()));
        }

        // Verify we got the expected number of pixels
        let expected = (width * height) as usize;
        if pixels.len() != expected {
            return Err(AlpacaError::ParseError(format!(
                "Image size mismatch: expected {} pixels ({}x{}), got {}",
                expected, width, height, pixels.len()
            )));
        }

        Ok((width, height, pixels))
    }

    /// Wait for image to be ready with configurable timeout
    /// Polls image_ready until true or timeout expires
    pub async fn wait_for_image_ready(
        &self,
        poll_interval: Duration,
        timeout: Duration,
    ) -> Result<bool, AlpacaError> {
        let deadline = std::time::Instant::now() + timeout;

        loop {
            match self.image_ready().await {
                Ok(true) => return Ok(true),
                Ok(false) => {
                    if std::time::Instant::now() >= deadline {
                        return Ok(false);
                    }
                    tokio::time::sleep(poll_interval).await;
                }
                Err(e) => return Err(AlpacaError::OperationFailed(e)),
            }
        }
    }

    /// Wait for camera to become idle with configurable timeout
    pub async fn wait_for_idle(
        &self,
        poll_interval: Duration,
        timeout: Duration,
    ) -> Result<bool, AlpacaError> {
        let deadline = std::time::Instant::now() + timeout;

        loop {
            match self.camera_state().await {
                Ok(CameraState::Idle) => return Ok(true),
                Ok(CameraState::Error) => return Err(AlpacaError::OperationFailed("Camera in error state".to_string())),
                Ok(_) => {
                    if std::time::Instant::now() >= deadline {
                        return Ok(false);
                    }
                    tokio::time::sleep(poll_interval).await;
                }
                Err(e) => return Err(AlpacaError::OperationFailed(e)),
            }
        }
    }

    /// Pulse guide in a direction
    pub async fn pulse_guide(&self, direction: i32, duration_ms: i32) -> Result<(), String> {
        self.client.put("pulseguide", &[
            ("Direction", &direction.to_string()),
            ("Duration", &duration_ms.to_string()),
        ]).await
    }

    // Parallel status methods

    /// Get comprehensive camera status in a single parallel query
    pub async fn get_status(&self) -> Result<CameraStatus, String> {
        let (
            state,
            connected,
            image_ready,
            percent_completed,
            ccd_temperature,
            cooler_on,
            cooler_power,
            bin_x,
            bin_y,
        ) = tokio::join!(
            self.camera_state(),
            self.is_connected(),
            self.image_ready(),
            self.percent_completed(),
            self.ccd_temperature(),
            self.cooler_on(),
            self.cooler_power(),
            self.bin_x(),
            self.bin_y(),
        );

        Ok(CameraStatus {
            state: state?,
            connected: connected?,
            image_ready: image_ready?,
            percent_completed: percent_completed.ok(),
            ccd_temperature: ccd_temperature.ok(),
            cooler_on: cooler_on.ok(),
            cooler_power: cooler_power.ok(),
            bin_x: bin_x?,
            bin_y: bin_y?,
        })
    }

    /// Get camera capabilities in a single parallel query
    pub async fn get_capabilities(&self) -> Result<CameraCapabilities, String> {
        let (
            can_abort_exposure,
            can_stop_exposure,
            can_asymmetric_bin,
            can_pulse_guide,
            can_fast_readout,
            can_set_ccd_temperature,
            can_get_cooler_power,
            has_shutter,
            max_bin_x,
            max_bin_y,
        ) = tokio::join!(
            self.can_abort_exposure(),
            self.can_stop_exposure(),
            self.can_asymmetric_bin(),
            self.can_pulse_guide(),
            self.can_fast_readout(),
            self.can_set_ccd_temperature(),
            self.can_get_cooler_power(),
            self.has_shutter(),
            self.max_bin_x(),
            self.max_bin_y(),
        );

        Ok(CameraCapabilities {
            can_abort_exposure: can_abort_exposure?,
            can_stop_exposure: can_stop_exposure?,
            can_asymmetric_bin: can_asymmetric_bin?,
            can_pulse_guide: can_pulse_guide?,
            can_fast_readout: can_fast_readout?,
            can_set_ccd_temperature: can_set_ccd_temperature?,
            can_get_cooler_power: can_get_cooler_power?,
            has_shutter: has_shutter?,
            max_bin_x: max_bin_x?,
            max_bin_y: max_bin_y?,
        })
    }

    /// Get camera sensor information in a single parallel query
    pub async fn get_sensor_info(&self) -> Result<CameraSensorInfo, String> {
        let (
            camera_x_size,
            camera_y_size,
            pixel_size_x,
            pixel_size_y,
            max_adu,
            sensor_type,
            sensor_name,
            bayer_offset_x,
            bayer_offset_y,
        ) = tokio::join!(
            self.camera_x_size(),
            self.camera_y_size(),
            self.pixel_size_x(),
            self.pixel_size_y(),
            self.max_adu(),
            self.sensor_type(),
            self.sensor_name(),
            self.bayer_offset_x(),
            self.bayer_offset_y(),
        );

        Ok(CameraSensorInfo {
            camera_x_size: camera_x_size?,
            camera_y_size: camera_y_size?,
            pixel_size_x: pixel_size_x?,
            pixel_size_y: pixel_size_y?,
            max_adu: max_adu?,
            sensor_type: SensorType::from(sensor_type?),
            sensor_name: sensor_name?,
            bayer_offset_x: bayer_offset_x.ok(),
            bayer_offset_y: bayer_offset_y.ok(),
        })
    }
}
