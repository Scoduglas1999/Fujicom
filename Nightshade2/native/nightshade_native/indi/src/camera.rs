//! INDI Camera wrapper
//!
//! Provides high-level camera control via INDI protocol.

use crate::client::IndiClient;
use crate::error::IndiResult;
use crate::protocol::{CcdFrameType, standard_properties::*};
use std::sync::Arc;
use std::time::Duration;
use tokio::sync::RwLock;

/// INDI Camera device wrapper
pub struct IndiCamera {
    client: Arc<RwLock<IndiClient>>,
    device_name: String,
}

impl IndiCamera {
    /// Create a new INDI camera wrapper
    pub fn new(client: Arc<RwLock<IndiClient>>, device_name: &str) -> Self {
        Self {
            client,
            device_name: device_name.to_string(),
        }
    }

    /// Get the device name
    pub fn device_name(&self) -> &str {
        &self.device_name
    }

    /// Connect to the camera
    pub async fn connect(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.connect_device(&self.device_name).await
    }

    /// Disconnect from the camera
    pub async fn disconnect(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.disconnect_device(&self.device_name).await
    }

    /// Check if connected
    pub async fn is_connected(&self) -> bool {
        let client = self.client.read().await;
        client.is_device_connected(&self.device_name).await
    }

    /// Start an exposure
    pub async fn start_exposure(&self, duration_secs: f64) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_number(&self.device_name, CCD_EXPOSURE, "CCD_EXPOSURE_VALUE", duration_secs).await
    }

    /// Abort the current exposure
    pub async fn abort_exposure(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_switch(&self.device_name, CCD_ABORT_EXPOSURE, "ABORT", true).await
    }

    /// Set binning
    pub async fn set_binning(&self, bin_x: i32, bin_y: i32) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_numbers(&self.device_name, CCD_BINNING, &[
            ("HOR_BIN", bin_x as f64),
            ("VER_BIN", bin_y as f64),
        ]).await
    }

    /// Get current binning
    pub async fn get_binning(&self) -> Result<(i32, i32), String> {
        let client = self.client.read().await;
        let bin_x = client.get_number(&self.device_name, CCD_BINNING, "HOR_BIN")
            .await
            .unwrap_or(1.0) as i32;
        let bin_y = client.get_number(&self.device_name, CCD_BINNING, "VER_BIN")
            .await
            .unwrap_or(1.0) as i32;
        Ok((bin_x, bin_y))
    }

    /// Set frame (ROI)
    pub async fn set_frame(&self, x: i32, y: i32, width: i32, height: i32) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_numbers(&self.device_name, CCD_FRAME, &[
            ("X", x as f64),
            ("Y", y as f64),
            ("WIDTH", width as f64),
            ("HEIGHT", height as f64),
        ]).await
    }

    /// Get frame (ROI)
    pub async fn get_frame(&self) -> Result<(i32, i32, i32, i32), String> {
        let client = self.client.read().await;
        let x = client.get_number(&self.device_name, CCD_FRAME, "X")
            .await
            .unwrap_or(0.0) as i32;
        let y = client.get_number(&self.device_name, CCD_FRAME, "Y")
            .await
            .unwrap_or(0.0) as i32;
        let width = client.get_number(&self.device_name, CCD_FRAME, "WIDTH")
            .await
            .unwrap_or(0.0) as i32;
        let height = client.get_number(&self.device_name, CCD_FRAME, "HEIGHT")
            .await
            .unwrap_or(0.0) as i32;
        Ok((x, y, width, height))
    }

    /// Set cooler target temperature
    pub async fn set_temperature(&self, temp_celsius: f64) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_number(&self.device_name, CCD_TEMPERATURE, "CCD_TEMPERATURE_VALUE", temp_celsius).await
    }

    /// Get current temperature
    pub async fn get_temperature(&self) -> Result<f64, String> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, CCD_TEMPERATURE, "CCD_TEMPERATURE_VALUE")
            .await
            .ok_or_else(|| "Temperature not available".to_string())
    }

    /// Enable/disable cooler
    pub async fn set_cooler(&self, enabled: bool) -> IndiResult<()> {
        let mut client = self.client.write().await;
        if enabled {
            client.set_switch(&self.device_name, CCD_COOLER, "COOLER_ON", true).await
        } else {
            client.set_switch(&self.device_name, CCD_COOLER, "COOLER_OFF", true).await
        }
    }

    /// Check if cooler is on
    pub async fn is_cooler_on(&self) -> bool {
        let client = self.client.read().await;
        client.get_switch(&self.device_name, CCD_COOLER, "COOLER_ON")
            .await
            .unwrap_or(false)
    }

    /// Set gain
    pub async fn set_gain(&self, gain: i32) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_number(&self.device_name, CCD_GAIN, "GAIN", gain as f64).await
    }

    /// Get gain
    pub async fn get_gain(&self) -> Result<i32, String> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, CCD_GAIN, "GAIN")
            .await
            .map(|g| g as i32)
            .ok_or_else(|| "Gain not available".to_string())
    }

    /// Set offset
    pub async fn set_offset(&self, offset: i32) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.set_number(&self.device_name, CCD_OFFSET, "OFFSET", offset as f64).await
    }

    /// Get offset
    pub async fn get_offset(&self) -> Result<i32, String> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, CCD_OFFSET, "OFFSET")
            .await
            .map(|o| o as i32)
            .ok_or_else(|| "Offset not available".to_string())
    }

    /// Enable BLOB transfer for image data
    pub async fn enable_blob(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.enable_blob(&self.device_name).await
    }
    // =========================================================================
    // Sensor Information (CCD_INFO property)
    // =========================================================================

    /// Get sensor width in pixels
    pub async fn get_sensor_width(&self) -> Option<i32> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, CCD_INFO, "CCD_MAX_X")
            .await
            .map(|v| v as i32)
    }

    /// Get sensor height in pixels
    pub async fn get_sensor_height(&self) -> Option<i32> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, CCD_INFO, "CCD_MAX_Y")
            .await
            .map(|v| v as i32)
    }

    /// Get pixel size X in microns
    pub async fn get_pixel_size_x(&self) -> Option<f64> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, CCD_INFO, "CCD_PIXEL_SIZE_X").await
    }

    /// Get pixel size Y in microns
    pub async fn get_pixel_size_y(&self) -> Option<f64> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, CCD_INFO, "CCD_PIXEL_SIZE_Y").await
    }

    /// Get bits per pixel
    pub async fn get_bits_per_pixel(&self) -> Option<i32> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, CCD_INFO, "CCD_BITSPERPIXEL")
            .await
            .map(|v| v as i32)
    }

    // =========================================================================
    // Binning Limits
    // =========================================================================

    /// Get maximum horizontal binning
    pub async fn get_max_bin_x(&self) -> Option<i32> {
        let client = self.client.read().await;
        // Check CCD_BINNING property for max values
        // INDI stores current values, but some drivers expose max in CCD_INFO
        client.get_number(&self.device_name, CCD_INFO, "CCD_MAX_BIN_X")
            .await
            .or(Some(4.0)) // Default max if not available
            .map(|v| v as i32)
    }

    /// Get maximum vertical binning
    pub async fn get_max_bin_y(&self) -> Option<i32> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, CCD_INFO, "CCD_MAX_BIN_Y")
            .await
            .or(Some(4.0)) // Default max if not available
            .map(|v| v as i32)
    }

    // =========================================================================
    // Frame Type
    // =========================================================================

    /// Set frame type (Light, Bias, Dark, Flat)
    pub async fn set_frame_type(&self, frame_type: CcdFrameType) -> IndiResult<()> {
        let mut client = self.client.write().await;
        let element = match frame_type {
            CcdFrameType::Light => "FRAME_LIGHT",
            CcdFrameType::Bias => "FRAME_BIAS",
            CcdFrameType::Dark => "FRAME_DARK",
            CcdFrameType::Flat => "FRAME_FLAT",
        };
        client.set_switch(&self.device_name, CCD_FRAME_TYPE, element, true).await
    }

    /// Get current frame type
    pub async fn get_frame_type(&self) -> CcdFrameType {
        let client = self.client.read().await;
        if client.get_switch(&self.device_name, CCD_FRAME_TYPE, "FRAME_BIAS").await.unwrap_or(false) {
            CcdFrameType::Bias
        } else if client.get_switch(&self.device_name, CCD_FRAME_TYPE, "FRAME_DARK").await.unwrap_or(false) {
            CcdFrameType::Dark
        } else if client.get_switch(&self.device_name, CCD_FRAME_TYPE, "FRAME_FLAT").await.unwrap_or(false) {
            CcdFrameType::Flat
        } else {
            CcdFrameType::Light
        }
    }

    // =========================================================================
    // Exposure State
    // =========================================================================

    /// Check if camera is currently exposing
    pub async fn is_exposing(&self) -> bool {
        let client = self.client.read().await;
        client.is_property_busy(&self.device_name, CCD_EXPOSURE).await
    }

    /// Get remaining exposure time in seconds (if available)
    pub async fn get_exposure_remaining(&self) -> Option<f64> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, CCD_EXPOSURE, "CCD_EXPOSURE_VALUE").await
    }

    // =========================================================================
    // Reset to Full Frame
    // =========================================================================

    /// Reset frame to full sensor size
    pub async fn reset_frame(&self) -> Result<(), String> {
        let width = self.get_sensor_width().await.ok_or("Sensor width not available")?;
        let height = self.get_sensor_height().await.ok_or("Sensor height not available")?;
        self.set_frame(0, 0, width, height).await.map_err(|e| e.to_string())
    }

    // =========================================================================
    // Cooler Power
    // =========================================================================

    /// Get cooler power percentage (if available)
    pub async fn get_cooler_power(&self) -> Option<f64> {
        let client = self.client.read().await;
        client.get_number(&self.device_name, CCD_COOLER_POWER, "CCD_COOLER_VALUE").await
    }

    /// Capture an image
    pub async fn capture_image(&self, duration_secs: f64) -> Result<Vec<u8>, String> {
        // Subscribe to events BEFORE starting exposure to avoid missing the event
        let mut rx = {
            let client = self.client.read().await;
            client.subscribe()
        };

        // Start exposure
        self.start_exposure(duration_secs).await?;

        // Wait for BLOB
        // We might get other events, so loop until we get the blob or timeout
        let timeout = std::time::Duration::from_secs_f64(duration_secs + 30.0); // Exposure + 30s buffer

        let start_time = std::time::Instant::now();

        loop {
            if start_time.elapsed() > timeout {
                return Err("Timeout waiting for image".to_string());
            }

            match tokio::time::timeout(std::time::Duration::from_secs(1), rx.recv()).await {
                Ok(Ok(event)) => {
                    match event {
                        crate::IndiEvent::BlobReceived { device, element, data, .. } => {
                            if device == self.device_name && (element == "CCD1" || element == "CCD2") {
                                return Ok(data);
                            }
                        },
                        _ => {}
                    }
                }
                Ok(Err(e)) => {
                    // Channel lag or closed
                    tracing::warn!("INDI event channel error: {}", e);
                    // If channel is closed, we can't receive image
                    return Err(format!("Event channel error: {}", e));
                }
                Err(_) => {
                    // Timeout on recv, check total timeout
                    continue;
                }
            }
        }
    }

    /// Capture an image with configurable timeout
    pub async fn capture_image_with_timeout(&self, duration_secs: f64, timeout_buffer: Option<Duration>) -> Result<Vec<u8>, String> {
        let buffer_secs = timeout_buffer.unwrap_or_else(|| {
            let client = tokio::task::block_in_place(|| {
                tokio::runtime::Handle::current().block_on(self.client.read())
            });
            Duration::from_secs(client.timeout_config().camera_exposure_buffer_secs)
        });

        // Subscribe to events BEFORE starting exposure to avoid missing the event
        let mut rx = {
            let client = self.client.read().await;
            client.subscribe()
        };

        // Start exposure
        self.start_exposure(duration_secs).await?;

        // Calculate total timeout: exposure time + buffer
        let timeout = Duration::from_secs_f64(duration_secs) + buffer_secs;
        let start_time = std::time::Instant::now();

        loop {
            if start_time.elapsed() > timeout {
                return Err(format!(
                    "Timeout waiting for image from device '{}' after exposure of {:.1}s + buffer of {:?}. \
                    The camera may have failed to complete the exposure or transfer the image.",
                    self.device_name, duration_secs, buffer_secs
                ));
            }

            match tokio::time::timeout(Duration::from_secs(1), rx.recv()).await {
                Ok(Ok(event)) => {
                    match event {
                        crate::IndiEvent::BlobReceived { device, element, data, .. } => {
                            if device == self.device_name && (element == "CCD1" || element == "CCD2") {
                                return Ok(data);
                            }
                        },
                        _ => {}
                    }
                }
                Ok(Err(e)) => {
                    tracing::warn!("INDI event channel error for device '{}': {}", self.device_name, e);
                    return Err(format!("Event channel error for device '{}': {}. The connection may have been lost.", self.device_name, e));
                }
                Err(_) => {
                    // Timeout on recv (1 second), check total timeout
                    continue;
                }
            }
        }
    }

    /// Start exposure and wait for it to complete with timeout
    pub async fn start_exposure_with_timeout(&self, duration_secs: f64, timeout_buffer: Option<Duration>) -> Result<(), String> {
        let buffer_secs = timeout_buffer.unwrap_or_else(|| {
            let client = tokio::task::block_in_place(|| {
                tokio::runtime::Handle::current().block_on(self.client.read())
            });
            Duration::from_secs(client.timeout_config().camera_exposure_buffer_secs)
        });

        // Start the exposure
        {
            let mut client = self.client.write().await;
            client.set_number(&self.device_name, CCD_EXPOSURE, "CCD_EXPOSURE_VALUE", duration_secs).await?;
        }

        // Wait for exposure to complete
        let timeout_duration = Duration::from_secs_f64(duration_secs) + buffer_secs;
        let client = self.client.read().await;
        client
            .wait_for_property_not_busy(&self.device_name, CCD_EXPOSURE, timeout_duration)
            .await
            .map_err(|e| format!("Camera exposure of {:.1}s failed: {}", duration_secs, e))
    }
}


