use std::sync::Arc;
use std::thread;
use std::time::Duration;
use tokio::sync::{mpsc, oneshot};
use nightshade_native::traits::{NativeDevice, NativeCamera, NativeError};
use nightshade_native::camera::{ImageData, SubFrame, SensorInfo, ReadoutMode, VendorFeatures, CameraStatus, CameraState, ExposureParams, CameraCapabilities};
use nightshade_native::NativeVendor;
use nightshade_ascom::{AscomCamera, init_com, uninit_com};
use std::fmt::Debug;
use chrono;
use crate::timeout_ops::Timeouts;

/// Connection health status for ASCOM devices
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum CameraConnectionHealth {
    /// Device is healthy and responding
    Healthy,
    /// Device is not responding but may recover
    Degraded,
    /// Device connection has failed
    Failed,
    /// Device health is unknown
    Unknown,
}

/// Command sent to the ASCOM worker thread
enum AscomCommand {
    Connect(oneshot::Sender<Result<(), String>>),
    SetupDialog(oneshot::Sender<Result<(), String>>),
    Disconnect(oneshot::Sender<Result<(), String>>),
    GetStatus(oneshot::Sender<Result<CameraStatus, String>>),
    StartExposure(ExposureParams, oneshot::Sender<Result<(), String>>),
    AbortExposure(oneshot::Sender<Result<(), String>>),
    IsExposureComplete(oneshot::Sender<Result<bool, String>>),
    DownloadImage(oneshot::Sender<Result<ImageData, String>>),
    SetSubframe(Option<SubFrame>, oneshot::Sender<Result<(), String>>),
    SetBinning(i32, i32, oneshot::Sender<Result<(), String>>),
    SetGain(i32, oneshot::Sender<Result<(), String>>),
    SetOffset(i32, oneshot::Sender<Result<(), String>>),
    SetCooler(bool, f64, oneshot::Sender<Result<(), String>>),
    /// Heartbeat check to verify device is still responding
    Heartbeat(oneshot::Sender<Result<CameraConnectionHealth, String>>),
    Stop(oneshot::Sender<()>),
}

/// Wrapper for ASCOM Camera that runs on a dedicated thread to support STA and Send/Sync
#[derive(Debug)]
pub struct AscomCameraWrapper {
    id: String,
    name: String,
    sender: mpsc::Sender<AscomCommand>,
    _thread_handle: Arc<thread::JoinHandle<()>>,
}

impl AscomCameraWrapper {
    pub fn new(prog_id: String) -> Result<Self, String> {
        let (tx, mut rx) = mpsc::channel(32);
        let prog_id_clone = prog_id.clone();
        
        let handle = thread::spawn(move || {
            // Initialize COM as STA on this thread
            if let Err(e) = init_com() {
                tracing::error!("Failed to init COM on ASCOM thread: {}", e);
                return;
            }
            
            let mut camera: Option<AscomCamera> = None;
            
            // Try to create the camera object immediately
            match AscomCamera::new(&prog_id_clone) {
                Ok(cam) => camera = Some(cam),
                Err(e) => tracing::error!("Failed to create ASCOM camera {}: {}", prog_id_clone, e),
            }
            
            while let Some(cmd) = rx.blocking_recv() {
                match cmd {
                    AscomCommand::Connect(reply) => {
                        if let Some(cam) = &mut camera {
                            let _ = reply.send(cam.connect().map_err(|e| e.to_string()));
                        } else {
                            let _ = reply.send(Err("Camera not created".to_string()));
                        }
                    }
                    AscomCommand::SetupDialog(reply) => {
                        if let Some(cam) = &mut camera {
                            let _ = reply.send(cam.setup_dialog().map_err(|e| e.to_string()));
                        } else {
                            let _ = reply.send(Err("Camera not created".to_string()));
                        }
                    }
                    AscomCommand::Disconnect(reply) => {
                        if let Some(cam) = &mut camera {
                            let _ = reply.send(cam.disconnect().map_err(|e| e.to_string()));
                        } else {
                            let _ = reply.send(Err("Camera not created".to_string()));
                        }
                    }
                    AscomCommand::GetStatus(reply) => {
                        if let Some(cam) = &camera {
                            // Use batch query for efficiency - fewer COM calls
                            let full_status = cam.get_full_status();

                            // Map ASCOM camera state to our CameraState enum
                            let state = match full_status.state.unwrap_or(0) {
                                0 => CameraState::Idle,
                                1 => CameraState::Idle, // Waiting maps to Idle
                                2 => CameraState::Exposing,
                                3 => CameraState::Reading,
                                4 => CameraState::Downloading,
                                5 => CameraState::Error,
                                _ => CameraState::Idle,
                            };

                            // Log thermal status if available
                            if let Some(temp) = full_status.thermal.temperature {
                                tracing::debug!("ASCOM camera temperature: {}C", temp);
                            }
                            if let Some(power) = full_status.thermal.cooler_power {
                                tracing::debug!("ASCOM cooler power: {}%", power);
                            }

                            // Determine target temp (ASCOM SetCCDTemperature is write-only)
                            let target_temp = if full_status.thermal.can_set_temperature.unwrap_or(false) {
                                Some(-10.0) // Default target, would need to track actual setpoint
                            } else {
                                None
                            };

                            let status = CameraStatus {
                                state,
                                sensor_temp: full_status.thermal.temperature,
                                cooler_power: full_status.thermal.cooler_power,
                                target_temp,
                                cooler_on: full_status.thermal.cooler_on.unwrap_or(false),
                                gain: full_status.exposure_settings.gain.unwrap_or(0),
                                offset: full_status.exposure_settings.offset.unwrap_or(0),
                                bin_x: full_status.exposure_settings.bin_x.unwrap_or(1),
                                bin_y: full_status.exposure_settings.bin_y.unwrap_or(1),
                                exposure_remaining: None, // ASCOM doesn't provide this directly
                            };
                            let _ = reply.send(Ok(status));
                        } else {
                            let _ = reply.send(Err("Camera not created".to_string()));
                        }
                    }
                    AscomCommand::StartExposure(params, reply) => {
                        tracing::info!("ASCOM: StartExposure called with duration={}", params.duration_secs);
                        if let Some(cam) = &mut camera {
                            tracing::info!("ASCOM: Calling cam.start_exposure({}, true)", params.duration_secs);
                            match cam.start_exposure(params.duration_secs, true) {
                                Ok(_) => {
                                    tracing::info!("ASCOM: start_exposure succeeded");
                                    let _ = reply.send(Ok(()));
                                }
                                Err(e) => {
                                    tracing::error!("ASCOM: start_exposure failed: {}", e);
                                    let _ = reply.send(Err(format!("Failed to start exposure: {}", e)));
                                }
                            }
                        } else {
                            tracing::error!("ASCOM: Camera not created");
                            let _ = reply.send(Err("Camera not created".to_string()));
                        }
                    }
                    AscomCommand::AbortExposure(reply) => {
                        tracing::info!("ASCOM: AbortExposure called");
                        if let Some(cam) = &mut camera {
                            let result = cam.abort_exposure()
                                .map_err(|e| format!("Failed to abort exposure: {}", e));
                            let _ = reply.send(result);
                        } else {
                            let _ = reply.send(Err("Camera not created".to_string()));
                        }
                    }
                    AscomCommand::IsExposureComplete(reply) => {
                        if let Some(cam) = &camera {
                            match cam.image_ready() {
                                Ok(ready) => { 
                                    tracing::debug!("ASCOM: image_ready() returned {}", ready);
                                    let _ = reply.send(Ok(ready)); 
                                }
                                Err(e) => { 
                                    tracing::error!("ASCOM: image_ready() failed: {}", e);
                                    let _ = reply.send(Err(format!("Failed to check image ready: {}", e))); 
                                }
                            }
                        } else {
                            let _ = reply.send(Err("Camera not created".to_string()));
                        }
                    }
                    AscomCommand::DownloadImage(reply) => {
                        tracing::info!("ASCOM: DownloadImage called");
                        if let Some(cam) = &camera {
                            tracing::info!("ASCOM: Getting camera dimensions");
                            let width = cam.camera_x_size().unwrap_or(1) as u32;
                            let height = cam.camera_y_size().unwrap_or(1) as u32;
                            tracing::info!("ASCOM: Camera dimensions: {}x{}", width, height);
                            
                            tracing::info!("ASCOM: Calling cam.image_array()");
                            match cam.image_array() {
                                Ok((data, w, h)) => {
                                    tracing::info!("ASCOM: image_array() returned {} pixels ({}x{})", data.len(), w, h);
                                    
                                    // Convert i32 array to u16 array
                                    let u16_data: Vec<u16> = data.iter().map(|&v| v.max(0).min(65535) as u16).collect();
                                    
                                    // Log min/max values for debugging
                                    if let (Some(&min), Some(&max)) = (data.iter().min(), data.iter().max()) {
                                        tracing::info!("ASCOM: Image data range: {} to {}", min, max);
                                    }
                                    
                                    let image_data = ImageData {
                                        width: w as u32,
                                        height: h as u32,
                                        data: u16_data,
                                        bits_per_pixel: 16,
                                        bayer_pattern: None, // ASCOM doesn't provide bayer pattern info easily
                                        metadata: nightshade_native::camera::ImageMetadata {
                                            exposure_time: 0.0, // Would need to track this
                                            gain: 0,
                                            offset: 0,
                                            bin_x: 1,
                                            bin_y: 1,
                                            temperature: None,
                                            timestamp: chrono::Utc::now(),
                                            subframe: None,
                                            readout_mode: None,
                                            vendor_data: VendorFeatures::default(),
                                        },
                                    };
                                    tracing::info!("ASCOM: Sending ImageData with {} pixels", image_data.data.len());
                                    let _ = reply.send(Ok(image_data));
                                }
                                Err(e) => {
                                    tracing::error!("ASCOM: image_array() failed: {}", e);
                                    let _ = reply.send(Err(format!("Failed to download image: {}", e)));
                                }
                            }
                        } else {
                            tracing::error!("ASCOM: Camera not created");
                            let _ = reply.send(Err("Camera not created".to_string()));
                        }
                    }
                    AscomCommand::SetSubframe(subframe, reply) => {
                        tracing::info!("ASCOM: SetSubframe called");
                        if let Some(cam) = &mut camera {
                            match subframe {
                                Some(sf) => {
                                    // Validate and set ROI
                                    let max_x = cam.camera_x_size().unwrap_or(1);
                                    let max_y = cam.camera_y_size().unwrap_or(1);

                                    if sf.start_x as i32 + sf.width as i32 > max_x || sf.start_y as i32 + sf.height as i32 > max_y {
                                        let _ = reply.send(Err("Subframe exceeds sensor bounds".to_string()));
                                    } else {
                                        let result = cam.set_start_x(sf.start_x as i32)
                                            .and_then(|_| cam.set_start_y(sf.start_y as i32))
                                            .and_then(|_| cam.set_num_x(sf.width as i32))
                                            .and_then(|_| cam.set_num_y(sf.height as i32))
                                            .map_err(|e| format!("Failed to set subframe: {}", e));
                                        let _ = reply.send(result);
                                    }
                                }
                                None => {
                                    // Reset to full frame
                                    let max_x = cam.camera_x_size().unwrap_or(1);
                                    let max_y = cam.camera_y_size().unwrap_or(1);
                                    let result = cam.set_start_x(0)
                                        .and_then(|_| cam.set_start_y(0))
                                        .and_then(|_| cam.set_num_x(max_x))
                                        .and_then(|_| cam.set_num_y(max_y))
                                        .map_err(|e| format!("Failed to reset to full frame: {}", e));
                                    let _ = reply.send(result);
                                }
                            }
                        } else {
                            let _ = reply.send(Err("Camera not created".to_string()));
                        }
                    }
                    AscomCommand::SetBinning(bin_x, bin_y, reply) => {
                        tracing::info!("ASCOM: SetBinning called: {}x{}", bin_x, bin_y);
                        if let Some(cam) = &mut camera {
                            let max_bin_x = cam.max_bin_x().unwrap_or(1);
                            let max_bin_y = cam.max_bin_y().unwrap_or(1);

                            if bin_x > max_bin_x || bin_y > max_bin_y {
                                let _ = reply.send(Err(format!("Binning {}x{} exceeds max {}x{}", bin_x, bin_y, max_bin_x, max_bin_y)));
                            } else {
                                let result = cam.set_bin_x(bin_x)
                                    .and_then(|_| cam.set_bin_y(bin_y))
                                    .map_err(|e| format!("Failed to set binning: {}", e));
                                let _ = reply.send(result);
                            }
                        } else {
                            let _ = reply.send(Err("Camera not created".to_string()));
                        }
                    }
                    AscomCommand::SetGain(gain, reply) => {
                        tracing::info!("ASCOM: SetGain called: {}", gain);
                        if let Some(cam) = &mut camera {
                            let result = cam.set_gain(gain)
                                .map_err(|e| format!("Failed to set gain: {}", e));
                            let _ = reply.send(result);
                        } else {
                            let _ = reply.send(Err("Camera not created".to_string()));
                        }
                    }
                    AscomCommand::SetOffset(offset, reply) => {
                        tracing::info!("ASCOM: SetOffset called: {}", offset);
                        if let Some(cam) = &mut camera {
                            let result = cam.set_offset(offset)
                                .map_err(|e| format!("Failed to set offset: {}", e));
                            let _ = reply.send(result);
                        } else {
                            let _ = reply.send(Err("Camera not created".to_string()));
                        }
                    }
                    AscomCommand::SetCooler(enabled, target_temp, reply) => {
                        tracing::info!("ASCOM: SetCooler called: enabled={}, temp={}", enabled, target_temp);
                        if let Some(cam) = &mut camera {
                            let result = cam.set_ccd_temperature(target_temp)
                                .and_then(|_| cam.set_cooler_on(enabled))
                                .map_err(|e| format!("Failed to set cooler: {}", e));
                            let _ = reply.send(result);
                        } else {
                            let _ = reply.send(Err("Camera not created".to_string()));
                        }
                    }
                    AscomCommand::Heartbeat(reply) => {
                        if let Some(cam) = &camera {
                            // Perform heartbeat check and return health status
                            match cam.heartbeat() {
                                Ok(()) => {
                                    let health = cam.get_health();
                                    let status = match health {
                                        nightshade_ascom::ConnectionHealth::Healthy => CameraConnectionHealth::Healthy,
                                        nightshade_ascom::ConnectionHealth::Degraded => CameraConnectionHealth::Degraded,
                                        nightshade_ascom::ConnectionHealth::Failed => CameraConnectionHealth::Failed,
                                        nightshade_ascom::ConnectionHealth::Unknown => CameraConnectionHealth::Unknown,
                                    };
                                    let _ = reply.send(Ok(status));
                                }
                                Err(e) => {
                                    tracing::warn!("ASCOM heartbeat failed: {}", e);
                                    let _ = reply.send(Ok(CameraConnectionHealth::Degraded));
                                }
                            }
                        } else {
                            let _ = reply.send(Err("Camera not created".to_string()));
                        }
                    }
                    AscomCommand::Stop(reply) => {
                        let _ = reply.send(());
                        break;
                    }
                }
            }
            
            uninit_com();
        });

        Ok(Self {
            id: prog_id.clone(),
            name: prog_id, // TODO: Get friendly name
            sender: tx,
            _thread_handle: Arc::new(handle),
        })
    }

    /// Helper to receive a response with a timeout
    /// Returns an error if the receive times out or the operation fails
    async fn recv_with_timeout<T>(
        rx: oneshot::Receiver<Result<T, String>>,
        timeout: Duration,
        operation: &str,
    ) -> Result<T, NativeError> {
        match tokio::time::timeout(timeout, rx).await {
            Ok(Ok(result)) => result.map_err(|e| NativeError::SdkError(e)),
            Ok(Err(_recv_err)) => Err(NativeError::Unknown(
                format!("Worker thread dead during {}", operation)
            )),
            Err(_elapsed) => Err(NativeError::Timeout(
                format!("Camera {} timed out after {:?}", operation, timeout)
            )),
        }
    }

    /// Display the ASCOM driver SetupDialog to choose device/config
    /// This is used to let the user select which camera to use when multiple are connected
    pub async fn setup_dialog(&self) -> Result<(), NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender
            .send(AscomCommand::SetupDialog(tx))
            .await
            .map_err(|_| NativeError::Unknown("Worker thread dead".to_string()))?;
        Self::recv_with_timeout(
            rx,
            Duration::from_secs(300), // Setup dialog can take a long time (user interaction)
            "setup_dialog",
        ).await
    }

    /// Perform a heartbeat check to verify device is still responding
    ///
    /// This should be called periodically (e.g., every 30 seconds) to detect
    /// if the device has become unresponsive. Returns the current health status.
    pub async fn heartbeat(&self) -> Result<CameraConnectionHealth, NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender
            .send(AscomCommand::Heartbeat(tx))
            .await
            .map_err(|_| NativeError::Unknown("Worker thread dead".to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::heartbeat(), "heartbeat").await
    }
}

#[async_trait::async_trait]
impl NativeDevice for AscomCameraWrapper {
    fn id(&self) -> &str {
        &self.id
    }

    fn name(&self) -> &str {
        &self.name
    }

    fn vendor(&self) -> NativeVendor {
        NativeVendor::Other("ASCOM".to_string())
    }

    fn is_connected(&self) -> bool {
        // We track connection state in DeviceManager, but could also query the thread
        true // Simplified
    }

    async fn connect(&mut self) -> Result<(), NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomCommand::Connect(tx)).await
            .map_err(|_| NativeError::Unknown("Worker thread dead".to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::connection(), "connect").await
    }

    async fn disconnect(&mut self) -> Result<(), NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomCommand::Disconnect(tx)).await
            .map_err(|_| NativeError::Unknown("Worker thread dead".to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::connection(), "disconnect").await
    }
}

#[async_trait::async_trait]
impl NativeCamera for AscomCameraWrapper {
    fn capabilities(&self) -> CameraCapabilities {
        CameraCapabilities::default() // TODO: Fetch from device
    }

    async fn get_status(&self) -> Result<CameraStatus, NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomCommand::GetStatus(tx)).await
            .map_err(|_| NativeError::Unknown("Worker thread dead".to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::property_read(), "get_status").await
    }

    async fn start_exposure(&mut self, params: ExposureParams) -> Result<(), NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomCommand::StartExposure(params, tx)).await
            .map_err(|_| NativeError::Unknown("Worker thread dead".to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::exposure_start(), "start_exposure").await
    }

    async fn abort_exposure(&mut self) -> Result<(), NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomCommand::AbortExposure(tx)).await
            .map_err(|_| NativeError::Unknown("Worker thread dead".to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::property_write(), "abort_exposure").await
    }

    async fn is_exposure_complete(&self) -> Result<bool, NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomCommand::IsExposureComplete(tx)).await
            .map_err(|_| NativeError::Unknown("Worker thread dead".to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::property_read(), "is_exposure_complete").await
    }

    async fn download_image(&mut self) -> Result<ImageData, NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomCommand::DownloadImage(tx)).await
            .map_err(|_| NativeError::Unknown("Worker thread dead".to_string()))?;
        // Image download can take a long time for large sensors
        Self::recv_with_timeout(rx, Timeouts::image_download_large(), "download_image").await
    }

    async fn set_cooler(&mut self, enabled: bool, target_temp: f64) -> Result<(), NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomCommand::SetCooler(enabled, target_temp, tx)).await
            .map_err(|_| NativeError::Unknown("Worker thread dead".to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::property_write(), "set_cooler").await
    }

    async fn get_temperature(&self) -> Result<f64, NativeError> {
        // Temperature is included in status
        let status = self.get_status().await?;
        status.sensor_temp.ok_or(NativeError::NotSupported)
    }

    async fn get_cooler_power(&self) -> Result<f64, NativeError> {
        // Cooler power is included in status
        let status = self.get_status().await?;
        status.cooler_power.ok_or(NativeError::NotSupported)
    }

    async fn set_gain(&mut self, gain: i32) -> Result<(), NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomCommand::SetGain(gain, tx)).await
            .map_err(|_| NativeError::Unknown("Worker thread dead".to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::property_write(), "set_gain").await
    }

    async fn get_gain(&self) -> Result<i32, NativeError> {
        // Gain is included in status
        let status = self.get_status().await?;
        Ok(status.gain)
    }

    async fn set_offset(&mut self, offset: i32) -> Result<(), NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomCommand::SetOffset(offset, tx)).await
            .map_err(|_| NativeError::Unknown("Worker thread dead".to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::property_write(), "set_offset").await
    }

    async fn get_offset(&self) -> Result<i32, NativeError> {
        // Offset is included in status
        let status = self.get_status().await?;
        Ok(status.offset)
    }

    async fn set_binning(&mut self, bin_x: i32, bin_y: i32) -> Result<(), NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomCommand::SetBinning(bin_x, bin_y, tx)).await
            .map_err(|_| NativeError::Unknown("Worker thread dead".to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::property_write(), "set_binning").await
    }

    async fn get_binning(&self) -> Result<(i32, i32), NativeError> {
        // Binning is included in status
        let status = self.get_status().await?;
        Ok((status.bin_x, status.bin_y))
    }

    async fn set_subframe(&mut self, subframe: Option<SubFrame>) -> Result<(), NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomCommand::SetSubframe(subframe, tx)).await
            .map_err(|_| NativeError::Unknown("Worker thread dead".to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::property_write(), "set_subframe").await
    }

    fn get_sensor_info(&self) -> SensorInfo {
        SensorInfo::default()
    }

    async fn get_readout_modes(&self) -> Result<Vec<ReadoutMode>, NativeError> {
        Ok(Vec::new())
    }

    async fn set_readout_mode(&mut self, _mode: &ReadoutMode) -> Result<(), NativeError> {
        Err(NativeError::NotSupported)
    }

    async fn get_vendor_features(&self) -> Result<VendorFeatures, NativeError> {
        Ok(VendorFeatures::default())
    }

    async fn get_gain_range(&self) -> Result<(i32, i32), NativeError> {
        // Return a reasonable default gain range
        Ok((0, 100))
    }

    async fn get_offset_range(&self) -> Result<(i32, i32), NativeError> {
        // Return a reasonable default offset range
        Ok((0, 255))
    }
}
