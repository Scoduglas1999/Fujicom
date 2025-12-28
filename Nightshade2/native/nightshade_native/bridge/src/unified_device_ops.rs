//! Unified Device Operations Implementation
//!
//! This module provides a unified entry point for the sequencer to interact with
//! hardware devices. It consolidates the two previous implementations:
//!
//! - `BridgeDeviceOps`: Routes through the bridge API, which handles device ID routing
//!   and dispatches to the appropriate driver (ASCOM, Alpaca, INDI, Native)
//! - `RealDeviceOps`: Direct access to ASCOM/Alpaca drivers (used internally)
//!
//! # Architecture
//!
//! ```text
//! ┌─────────────────┐
//! │    Sequencer    │
//! └────────┬────────┘
//!          │ uses DeviceOps trait
//!          ▼
//! ┌─────────────────┐
//! │ UnifiedDeviceOps│
//! └────────┬────────┘
//!          │ calls bridge API
//!          ▼
//! ┌─────────────────┐
//! │   Bridge API    │
//! │  (api_* funcs)  │
//! └────────┬────────┘
//!          │ routes by device ID prefix
//!          ▼
//! ┌─────────────────────────────────────────┐
//! │              DeviceManager              │
//! ├────────┬────────┬─────────┬────────────┤
//! │ ASCOM  │ Alpaca │  INDI   │   Native   │
//! │(ascom:)│(alpaca:)│(indi:) │(native:zwo)│
//! └────────┴────────┴─────────┴────────────┘
//! ```
//!
//! # Usage
//!
//! ```rust,ignore
//! use nightshade_bridge::unified_device_ops::create_unified_device_ops;
//!
//! let ops = create_unified_device_ops();
//! executor.set_device_ops(ops);
//! ```

use async_trait::async_trait;
use nightshade_sequencer::{DeviceOps, DeviceResult, ImageData, PlateSolveResult, GuidingStatus, PolarAlignResult};
use crate::state::SharedAppState;
use crate::api::*;
use crate::FitsWriteHeader;
use crate::event::*;
use std::sync::Arc;

/// Unified device operations implementation
///
/// This is the recommended DeviceOps implementation for the sequencer.
/// It routes all device operations through the bridge API which handles:
/// - Device ID prefix routing (ascom:, alpaca:, indi:, native:)
/// - Connection state management
/// - Error handling and logging
pub struct UnifiedDeviceOps {
    app_state: SharedAppState,
}

impl UnifiedDeviceOps {
    pub fn new(app_state: SharedAppState) -> Self {
        Self { app_state }
    }
}

#[async_trait]
impl DeviceOps for UnifiedDeviceOps {
    // =========================================================================
    // MOUNT OPERATIONS
    // =========================================================================
    
    async fn mount_slew_to_coordinates(&self, mount_id: &str, ra_hours: f64, dec_degrees: f64) -> DeviceResult<()> {
        tracing::info!("Slewing mount {} to RA={:.4}h Dec={:.4}°", mount_id, ra_hours, dec_degrees);

        // Emit slew started event
        self.app_state.publish_event(create_event(
            EventSeverity::Info,
            EventCategory::Equipment,
            EventPayload::Equipment(EquipmentEvent::MountSlewStarted {
                ra: ra_hours,
                dec: dec_degrees
            }),
        ));

        let result = get_device_manager().mount_slew(mount_id, ra_hours, dec_degrees)
            .await
            .map_err(|e| format!("Slew failed: {}", e));

        // Emit slew completed event on success
        if result.is_ok() {
            self.app_state.publish_event(create_event(
                EventSeverity::Info,
                EventCategory::Equipment,
                EventPayload::Equipment(EquipmentEvent::MountSlewCompleted {
                    ra: ra_hours,
                    dec: dec_degrees
                }),
            ));
        }

        // Map bayer pattern to sensor_type and bayer_offset

        result
    }

    async fn mount_abort_slew(&self, mount_id: &str) -> DeviceResult<()> {
        tracing::info!("Aborting slew for mount {}", mount_id);
        
        get_device_manager().mount_abort(mount_id)
            .await
            .map_err(|e| format!("Abort slew failed: {}", e))
    }
    
    async fn mount_get_coordinates(&self, mount_id: &str) -> DeviceResult<(f64, f64)> {
        let status = get_device_manager().mount_get_status(mount_id)
            .await
            .map_err(|e| format!("Failed to get mount status: {}", e))?;
        
        Ok((status.right_ascension, status.declination))
    }
    
    async fn mount_sync(&self, mount_id: &str, ra_hours: f64, dec_degrees: f64) -> DeviceResult<()> {
        tracing::info!("Syncing mount {} to RA={:.4}h Dec={:.4}°", mount_id, ra_hours, dec_degrees);
        
        get_device_manager().mount_sync(mount_id, ra_hours, dec_degrees)
            .await
            .map_err(|e| format!("Sync failed: {}", e))
    }
    
    async fn mount_park(&self, mount_id: &str) -> DeviceResult<()> {
        tracing::info!("Parking mount {}", mount_id);

        // Emit park started event
        self.app_state.publish_event(create_event(
            EventSeverity::Info,
            EventCategory::Equipment,
            EventPayload::Equipment(EquipmentEvent::MountParkStarted),
        ));

        let result = get_device_manager().mount_park(mount_id)
            .await
            .map_err(|e| format!("Park failed: {}", e));

        if result.is_ok() {
            self.app_state.publish_event(create_event(
                EventSeverity::Info,
                EventCategory::Equipment,
                EventPayload::Equipment(EquipmentEvent::MountParkCompleted),
            ));
        }

        // Map bayer pattern to sensor_type and bayer_offset

        result
    }

    async fn mount_unpark(&self, mount_id: &str) -> DeviceResult<()> {
        tracing::info!("Unparking mount {}", mount_id);

        let result = get_device_manager().mount_unpark(mount_id)
            .await
            .map_err(|e| format!("Unpark failed: {}", e));

        if result.is_ok() {
            self.app_state.publish_event(create_event(
                EventSeverity::Info,
                EventCategory::Equipment,
                EventPayload::Equipment(EquipmentEvent::MountUnparked),
            ));
        }

        // Map bayer pattern to sensor_type and bayer_offset

        result
    }
    
    async fn mount_is_slewing(&self, mount_id: &str) -> DeviceResult<bool> {
        let status = get_device_manager().mount_get_status(mount_id)
            .await
            .map_err(|e| format!("Failed to get mount status: {}", e))?;
        
        Ok(status.slewing)
    }
    
    async fn mount_is_parked(&self, mount_id: &str) -> DeviceResult<bool> {
        let status = get_device_manager().mount_get_status(mount_id)
            .await
            .map_err(|e| format!("Failed to get mount status: {}", e))?;

        Ok(status.parked)
    }

    async fn mount_can_flip(&self, mount_id: &str) -> DeviceResult<bool> {
        // Check if mount is a GEM (German Equatorial Mount) that can flip
        let status = get_device_manager().mount_get_status(mount_id)
            .await
            .map_err(|e| format!("Failed to get mount status: {}", e))?;

        // GEM mounts that can track generally support flipping
        Ok(status.tracking || !status.parked)
    }

    async fn mount_side_of_pier(&self, mount_id: &str) -> DeviceResult<nightshade_sequencer::meridian::PierSide> {
        // Get pier side from mount status
        let status = get_device_manager().mount_get_status(mount_id)
            .await
            .map_err(|e| format!("Failed to get mount status: {}", e))?;

        Ok(match status.side_of_pier {
            crate::device::PierSide::East => nightshade_sequencer::meridian::PierSide::East,
            crate::device::PierSide::West => nightshade_sequencer::meridian::PierSide::West,
            crate::device::PierSide::Unknown => nightshade_sequencer::meridian::PierSide::Unknown,
        })
    }

    async fn mount_is_tracking(&self, mount_id: &str) -> DeviceResult<bool> {
        let status = get_device_manager().mount_get_status(mount_id)
            .await
            .map_err(|e| format!("Failed to get mount status: {}", e))?;

        Ok(status.tracking)
    }

    async fn mount_set_tracking(&self, mount_id: &str, enabled: bool) -> DeviceResult<()> {
        tracing::info!("Setting tracking {} for mount {}", if enabled { "on" } else { "off" }, mount_id);

        get_device_manager().mount_set_tracking(mount_id, enabled)
            .await
            .map_err(|e| format!("Set tracking failed: {}", e))
    }

    // =========================================================================
    // CAMERA OPERATIONS
    // =========================================================================
    
    async fn camera_start_exposure(
        &self,
        camera_id: &str,
        duration_secs: f64,
        gain: Option<i32>,
        offset: Option<i32>,
        bin_x: i32,
        bin_y: i32,
    ) -> DeviceResult<ImageData> {
        tracing::info!("Starting {:.1}s exposure on camera {}", duration_secs, camera_id);

        let mgr = get_device_manager();
        let start_time = std::time::Instant::now();

        // Publish ExposureStarted event
        self.app_state.publish_imaging_event(
            ImagingEvent::ExposureStarted {
                duration_secs,
                frame_type: crate::device::FrameType::Light,
            },
            EventSeverity::Info,
        );

        // Start the exposure
        mgr.camera_start_exposure(
            camera_id,
            duration_secs,
            gain.unwrap_or(0),
            offset.unwrap_or(0),
            bin_x,
            bin_y,
        )
            .await
            .map_err(|e| {
                // Publish failure event
                self.app_state.publish_imaging_event(
                    ImagingEvent::ExposureComplete { success: false },
                    EventSeverity::Error,
                );
                format!("Exposure failed: {}", e)
            })?;

        // Wait for completion with progress updates
        let poll_interval = std::time::Duration::from_millis(100);
        loop {
            tokio::time::sleep(poll_interval).await;

            // Calculate and publish progress
            let elapsed = start_time.elapsed().as_secs_f64();
            let progress = (elapsed / duration_secs).min(1.0);
            let remaining = (duration_secs - elapsed).max(0.0);

            self.app_state.publish_imaging_event(
                ImagingEvent::ExposureProgress {
                    progress,
                    remaining_secs: remaining,
                },
                EventSeverity::Info,
            );

            match mgr.camera_is_exposure_complete(camera_id).await {
                Ok(true) => break,
                Ok(false) => continue,
                Err(e) => {
                    self.app_state.publish_imaging_event(
                        ImagingEvent::ExposureComplete { success: false },
                        EventSeverity::Error,
                    );
                    return Err(format!("Failed to check exposure status: {}", e));
                }

        // Map bayer pattern to sensor_type and bayer_offset
            }

        // Map bayer pattern to sensor_type and bayer_offset
        }

        // Map bayer pattern to sensor_type and bayer_offset

        // Download image
        let native_image = mgr.camera_download_image(camera_id).await
            .map_err(|e| {
                self.app_state.publish_imaging_event(
                    ImagingEvent::ExposureComplete { success: false },
                    EventSeverity::Error,
                );
                format!("Failed to download image: {}", e)
            })?;

        // Validate downloaded image data to catch corrupted/bad frames early
        // This prevents cascading failures in autofocus, plate solving, etc.
        {
            // Convert to nightshade_imaging ImageData for validation
            let img_for_validation = nightshade_imaging::ImageData::from_u16(
                native_image.width,
                native_image.height,
                1, // channels
                &native_image.data
            );

            // Use comprehensive validation - bias frames (very short exposures) are allowed to have uniform data
            let is_bias_frame = duration_secs < 0.1; // Bias frames are typically < 100ms
            let validation = nightshade_imaging::validate_image_with_options(
                &img_for_validation,
                Some(native_image.width),
                Some(native_image.height),
                is_bias_frame,
            );


            // Log validation warnings (don't fail, just inform user via logging)
            for warning in &validation.warnings {
                tracing::warn!("[CAMERA] Image validation warning: {}", warning);
            }

        // Map bayer pattern to sensor_type and bayer_offset

            // Fail on validation errors (corrupted/unusable images)
            if !validation.is_valid {
                let error_msg = validation.errors.join("; ");
                tracing::error!("[CAMERA] Image validation failed: {}", error_msg);
                self.app_state.publish_imaging_event(
                    ImagingEvent::ExposureComplete { success: false },
                    EventSeverity::Error,
                );
                return Err(format!("Image validation failed: {}", error_msg));
            }

        // Map bayer pattern to sensor_type and bayer_offset
        }

        // Map bayer pattern to sensor_type and bayer_offset
        let (sensor_type, bayer_offset) = match &native_image.bayer_pattern {
            Some(pattern) => {
                let offset = match pattern {
                    nightshade_native::camera::BayerPattern::Rggb => (0, 0),
                    nightshade_native::camera::BayerPattern::Grbg => (1, 0),
                    nightshade_native::camera::BayerPattern::Gbrg => (0, 1),
                    nightshade_native::camera::BayerPattern::Bggr => (1, 1),
                };
                (Some("Color".to_string()), Some(offset))
            }

        // Map bayer pattern to sensor_type and bayer_offset
            None => (Some("Monochrome".to_string()), None),
        };

        // Publish success event
        self.app_state.publish_imaging_event(
            ImagingEvent::ExposureComplete { success: true },
            EventSeverity::Info,
        );

        tracing::info!(
            "Exposure complete: {}x{} image, {} sensor",
            native_image.width,
            native_image.height,
            sensor_type.as_deref().unwrap_or("unknown")
        );

        // Convert to sequencer ImageData
        Ok(ImageData {
            width: native_image.width,
            height: native_image.height,
            data: native_image.data,
            bits_per_pixel: native_image.bits_per_pixel,
            exposure_secs: if native_image.metadata.exposure_time > 0.0 {
                native_image.metadata.exposure_time
            } else {
                duration_secs
            },
            gain: Some(native_image.metadata.gain),
            offset: Some(native_image.metadata.offset),
            temperature: native_image.metadata.temperature,
            filter: None,
            timestamp: native_image.metadata.timestamp.timestamp(),
            sensor_type,
            bayer_offset,
        })
    }
    
    async fn camera_abort_exposure(&self, camera_id: &str) -> DeviceResult<()> {
        tracing::info!("Aborting exposure on camera {}", camera_id);
        
        get_device_manager().camera_abort_exposure(camera_id)
            .await
            .map_err(|e| format!("Abort failed: {}", e))
    }
    
    async fn camera_set_cooler(&self, camera_id: &str, enabled: bool, target_temp: f64) -> DeviceResult<()> {
        tracing::info!("Camera {} cooler: enabled={}, target={}°C", camera_id, enabled, target_temp);

        // Emit cooling event
        if enabled {
            self.app_state.publish_event(create_event(
                EventSeverity::Info,
                EventCategory::Equipment,
                EventPayload::Equipment(EquipmentEvent::CameraCoolingStarted {
                    target_temp
                }),
            ));
        } else {
            self.app_state.publish_event(create_event(
                EventSeverity::Info,
                EventCategory::Equipment,
                EventPayload::Equipment(EquipmentEvent::CameraWarmingStarted),
            ));
        }

        // Map bayer pattern to sensor_type and bayer_offset

        api_set_camera_cooler(camera_id.to_string(), enabled as u8, Some(target_temp))
            .await
            .map_err(|e| format!("Cooler control failed: {}", e))
    }
    
    async fn camera_get_temperature(&self, camera_id: &str) -> DeviceResult<f64> {
        let status = api_get_camera_status(camera_id.to_string())
            .await
            .map_err(|e| format!("Failed to get camera status: {}", e))?;
        
        status.sensor_temp.ok_or_else(|| "Temperature not available".to_string())
    }
    
    async fn camera_get_cooler_power(&self, camera_id: &str) -> DeviceResult<f64> {
        let status = api_get_camera_status(camera_id.to_string())
            .await
            .map_err(|e| format!("Failed to get camera status: {}", e))?;
        
        status.cooler_power.ok_or_else(|| "Cooler power not available".to_string())
    }
    
    // =========================================================================
    // FOCUSER OPERATIONS
    // =========================================================================
    
    async fn focuser_move_to(&self, focuser_id: &str, position: i32) -> DeviceResult<()> {
        tracing::info!("Moving focuser {} to position {}", focuser_id, position);

        // Emit focuser move started event
        self.app_state.publish_event(create_event(
            EventSeverity::Info,
            EventCategory::Equipment,
            EventPayload::Equipment(EquipmentEvent::FocuserMoveStarted {
                target_position: position
            }),
        ));

        let result = api_focuser_move_to(focuser_id.to_string(), position)
            .await
            .map_err(|e| format!("Focuser move failed: {}", e));

        if result.is_ok() {
            self.app_state.publish_event(create_event(
                EventSeverity::Info,
                EventCategory::Equipment,
                EventPayload::Equipment(EquipmentEvent::FocuserMoveCompleted {
                    position
                }),
            ));
        }

        // Map bayer pattern to sensor_type and bayer_offset

        result
    }
    
    async fn focuser_get_position(&self, focuser_id: &str) -> DeviceResult<i32> {
        get_device_manager().focuser_get_position(focuser_id)
            .await
            .map_err(|e| format!("Get position failed: {}", e))
    }
    
    async fn focuser_is_moving(&self, focuser_id: &str) -> DeviceResult<bool> {
        get_device_manager().focuser_is_moving(focuser_id)
            .await
            .map_err(|e| format!("Is moving failed: {}", e))
    }
    
    async fn focuser_get_temperature(&self, focuser_id: &str) -> DeviceResult<Option<f64>> {
        get_device_manager().focuser_get_temp(focuser_id)
            .await
            .map_err(|e| format!("Get temperature failed: {}", e))
    }

    async fn focuser_halt(&self, focuser_id: &str) -> DeviceResult<()> {
        get_device_manager().focuser_halt(focuser_id)
            .await
            .map_err(|e| format!("Halt failed: {}", e))
    }
    
    // =========================================================================
    // FILTER WHEEL OPERATIONS
    // =========================================================================
    
    async fn filterwheel_set_position(&self, fw_id: &str, position: i32) -> DeviceResult<()> {
        // Get current position for the event
        let from_position = get_device_manager().filter_wheel_get_position(fw_id)
            .await
            .unwrap_or(-1);

        // Emit filter changing event
        self.app_state.publish_event(create_event(
            EventSeverity::Info,
            EventCategory::Equipment,
            EventPayload::Equipment(EquipmentEvent::FilterChanging {
                from_position,
                to_position: position,
                filter_name: None, // Will be populated by UI if available
            }),
        ));

        let result = get_device_manager().filter_wheel_set_position(fw_id, position)
            .await
            .map_err(|e| format!("Set position failed: {}", e));

        if result.is_ok() {
            self.app_state.publish_event(create_event(
                EventSeverity::Info,
                EventCategory::Equipment,
                EventPayload::Equipment(EquipmentEvent::FilterChanged {
                    position,
                    filter_name: None,
                }),
            ));
        }

        // Map bayer pattern to sensor_type and bayer_offset

        result
    }
    
    async fn filterwheel_get_position(&self, fw_id: &str) -> DeviceResult<i32> {
        get_device_manager().filter_wheel_get_position(fw_id)
            .await
            .map_err(|e| format!("Get position failed: {}", e))
    }
    
    async fn filterwheel_get_names(&self, fw_id: &str) -> DeviceResult<Vec<String>> {
        let (_, names) = get_device_manager().filter_wheel_get_config(fw_id)
            .await
            .map_err(|e| format!("Get names failed: {}", e))?;
        Ok(names)
    }
    
    async fn filterwheel_set_filter_by_name(&self, fw_id: &str, name: &str) -> DeviceResult<i32> {
        let names = self.filterwheel_get_names(fw_id).await?;
        
        // Find the filter position by name (case-insensitive)
        let position = names.iter()
            .position(|n| n.eq_ignore_ascii_case(name))
            .map(|p| (p + 1) as i32)
            .ok_or_else(|| format!("Filter '{}' not found", name))?;
        
        self.filterwheel_set_position(fw_id, position).await?;
        Ok(position)
    }
    
    // =========================================================================
    // ROTATOR OPERATIONS
    // =========================================================================
    
    async fn rotator_move_to(&self, rotator_id: &str, angle: f64) -> DeviceResult<()> {
        tracing::info!("Moving rotator {} to {}°", rotator_id, angle);

        // Emit rotator move started event
        self.app_state.publish_event(create_event(
            EventSeverity::Info,
            EventCategory::Equipment,
            EventPayload::Equipment(EquipmentEvent::RotatorMoveStarted {
                target_angle: angle
            }),
        ));

        let result = api_rotator_move_to(rotator_id.to_string(), angle)
            .await
            .map_err(|e| format!("Rotator move failed: {}", e));

        if result.is_ok() {
            self.app_state.publish_event(create_event(
                EventSeverity::Info,
                EventCategory::Equipment,
                EventPayload::Equipment(EquipmentEvent::RotatorMoveCompleted { angle }),
            ));
        }

        // Map bayer pattern to sensor_type and bayer_offset

        result
    }
    
    async fn rotator_move_relative(&self, rotator_id: &str, delta: f64) -> DeviceResult<()> {
        tracing::info!("Moving rotator {} by {}°", rotator_id, delta);
        
        api_rotator_move_relative(rotator_id.to_string(), delta)
            .await
            .map_err(|e| format!("Rotator move relative failed: {}", e))
    }
    
    async fn rotator_get_angle(&self, rotator_id: &str) -> DeviceResult<f64> {
        let status = api_get_rotator_status(rotator_id.to_string())
            .await
            .map_err(|e| format!("Failed to get rotator status: {}", e))?;
        
        Ok(status.position)
    }

    async fn rotator_halt(&self, rotator_id: &str) -> DeviceResult<()> {
        tracing::info!("Halting rotator {}", rotator_id);
        
        api_rotator_halt(rotator_id.to_string())
            .await
            .map_err(|e| format!("Halt failed: {}", e))
    }
    
    // =========================================================================
    // GUIDING / PHD2 OPERATIONS
    // =========================================================================
    
    async fn guider_dither(
        &self,
        pixels: f64,
        settle_pixels: f64,
        settle_time: f64,
        settle_timeout: f64,
        ra_only: bool,
    ) -> DeviceResult<()> {
        tracing::info!("Dithering {} pixels (settle: <{}px in {}s)", pixels, settle_pixels, settle_time);
        
        api_phd2_dither(pixels, ra_only as u8, settle_pixels, settle_time, settle_timeout)
            .await
            .map_err(|e| format!("Dither failed: {}", e))
    }
    
    async fn guider_get_status(&self) -> DeviceResult<GuidingStatus> {
        let status = api_phd2_get_status()
            .await
            .map_err(|e| format!("Failed to get guiding status: {}", e))?;
        
        Ok(GuidingStatus {
            is_guiding: status.state == "Guiding",
            rms_ra: status.rms_ra,
            rms_dec: status.rms_dec,
            rms_total: status.rms_total,
        })
    }
    
    async fn guider_start(&self, settle_pixels: f64, settle_time: f64, settle_timeout: f64) -> DeviceResult<()> {
        tracing::info!("Starting guiding");
        
        api_phd2_start_guiding(settle_pixels, settle_time, settle_timeout)
            .await
            .map_err(|e| format!("Start guiding failed: {}", e))
    }
    
    async fn guider_stop(&self) -> DeviceResult<()> {
        tracing::info!("Stopping guiding");
        
        api_phd2_stop_guiding()
            .await
            .map_err(|e| format!("Stop guiding failed: {}", e))
    }
    
    // =========================================================================
    // PLATE SOLVING
    // =========================================================================
    
    async fn plate_solve(
        &self,
        image_data: &ImageData,
        hint_ra: Option<f64>,
        hint_dec: Option<f64>,
        hint_scale: Option<f64>,
    ) -> DeviceResult<PlateSolveResult> {
        tracing::info!("Plate solving image");
        
        // Use platform-appropriate temp directory
        let temp_dir = std::env::temp_dir();
        let temp_file = temp_dir.join("nightshade_platesolve_temp.fits");
        let temp_path = temp_file.to_string_lossy().to_string();
        
        // Save the image data to the temp file first
        let header = FitsWriteHeader {
            object_name: Some("Plate Solve".to_string()),
            exposure_time: image_data.exposure_secs,
            capture_timestamp: chrono::Utc::now().format("%Y-%m-%dT%H:%M:%S").to_string(),
            frame_type: "Light".to_string(),
            filter: image_data.filter.clone(),
            gain: image_data.gain,
            offset: image_data.offset,
            ccd_temp: image_data.temperature,
            ra: hint_ra.map(|r| r / 15.0),
            dec: hint_dec,
            altitude: None,
            telescope: None,
            instrument: None,
            observer: None,
            bin_x: 1,
            bin_y: 1,
            focal_length: None,
            aperture: None,
            pixel_size_x: None,
            pixel_size_y: None,
            site_latitude: None,
            site_longitude: None,
            site_elevation: None,
        };
        
        api_save_fits_file(
            temp_path.clone(),
            image_data.width,
            image_data.height,
            image_data.data.clone(),
            header,
        ).await.map_err(|e| format!("Failed to save temp FITS for plate solve: {}", e))?;
        
        // Use the near solve if we have hints, otherwise blind solve
        let result = if let (Some(ra), Some(dec)) = (hint_ra, hint_dec) {
            api_plate_solve_near(
                temp_path.clone(),
                ra,
                dec,
                hint_scale.unwrap_or(5.0),
            ).await
        } else {
            api_plate_solve_blind(temp_path.clone()).await
        };
        
        // Clean up temp file
        let _ = std::fs::remove_file(&temp_path);
        
        result.map(|r| PlateSolveResult {
            ra_degrees: r.ra,
            dec_degrees: r.dec,
            pixel_scale: r.pixel_scale,
            rotation: r.rotation,
            success: r.success,
        }).map_err(|e| format!("Plate solve failed: {}", e))
    }
    
    // =========================================================================
    // IMAGE SAVING
    // =========================================================================
    
    async fn save_fits(
        &self,
        image_data: &ImageData,
        file_path: &str,
        target_name: Option<&str>,
        filter: Option<&str>,
        ra_hours: Option<f64>,
        dec_degrees: Option<f64>,
    ) -> DeviceResult<()> {
        tracing::info!("Saving FITS image to: {}", file_path);
        
        let header = FitsWriteHeader {
            object_name: target_name.map(|s| s.to_string()),
            exposure_time: image_data.exposure_secs,
            capture_timestamp: chrono::Utc::now().format("%Y-%m-%dT%H:%M:%S").to_string(),
            frame_type: "Light".to_string(),
            filter: filter.map(|s| s.to_string()),
            gain: image_data.gain,
            offset: image_data.offset,
            ccd_temp: image_data.temperature,
            ra: ra_hours,
            dec: dec_degrees,
            altitude: None,
            telescope: None,
            instrument: None,
            observer: None,
            bin_x: 1,
            bin_y: 1,
            focal_length: None,
            aperture: None,
            pixel_size_x: None,
            pixel_size_y: None,
            site_latitude: None,
            site_longitude: None,
            site_elevation: None,
        };

        api_save_fits_file(
            file_path.to_string(),
            image_data.width,
            image_data.height,
            image_data.data.clone(),
            header,
        ).await.map_err(|e| format!("Save FITS failed: {}", e))
    }
    
    // =========================================================================
    // NOTIFICATIONS
    // =========================================================================
    
    async fn send_notification(&self, level: &str, title: &str, message: &str) -> DeviceResult<()> {
        tracing::info!("[NOTIFICATION][{}] {}: {}", level.to_uppercase(), title, message);
        
        // Publish as event to the event bus
        let severity = match level {
            "error" => EventSeverity::Error,
            "warning" => EventSeverity::Warning,
            _ => EventSeverity::Info,
        };
        
        self.app_state.publish_event(create_event(
            severity,
            EventCategory::System,
            EventPayload::System(SystemEvent::Notification {
                title: title.to_string(),
                message: message.to_string(),
                level: level.to_string(),
            }),
        ));
        
        Ok(())
    }

    async fn polar_align_update(&self, result: &PolarAlignResult) -> DeviceResult<()> {
        tracing::info!("Polar Align Update: Alt {:.1}', Az {:.1}'", result.altitude_error, result.azimuth_error);
        
        let event = PolarAlignmentEvent {
            azimuth_error: result.azimuth_error,
            altitude_error: result.altitude_error,
            total_error: result.total_error,
            current_ra: result.current_ra,
            current_dec: result.current_dec,
            target_ra: result.target_ra,
            target_dec: result.target_dec,
        };
        
        self.app_state.publish_event(create_event(
            EventSeverity::Info,
            EventCategory::PolarAlignment,
            EventPayload::PolarAlignment(event),
        ));
        
        Ok(())
    }
    
    
    // =========================================================================
    // DOME OPERATIONS
    // =========================================================================

    async fn dome_open(&self, dome_id: &str) -> DeviceResult<()> {
        tracing::info!("Opening dome shutter {}", dome_id);

        get_device_manager().dome_open_shutter(dome_id)
            .await
            .map_err(|e| format!("Open dome shutter failed: {}", e))
    }

    async fn dome_close(&self, dome_id: &str) -> DeviceResult<()> {
        tracing::info!("Closing dome shutter {}", dome_id);

        get_device_manager().dome_close_shutter(dome_id)
            .await
            .map_err(|e| format!("Close dome shutter failed: {}", e))
    }

    async fn dome_park(&self, dome_id: &str) -> DeviceResult<()> {
        tracing::info!("Parking dome {}", dome_id);

        get_device_manager().dome_park(dome_id)
            .await
            .map_err(|e| format!("Park dome failed: {}", e))
    }

    async fn dome_get_shutter_status(&self, dome_id: &str) -> DeviceResult<String> {
        let status = get_device_manager().dome_get_shutter_status(dome_id)
            .await
            .map_err(|e| format!("Get dome shutter status failed: {}", e))?;

        // Convert i32 status to string
        // ASCOM ShutterStatus: 0=Open, 1=Closed, 2=Opening, 3=Closing, 4=Error
        Ok(match status {
            0 => "Open".to_string(),
            1 => "Closed".to_string(),
            2 => "Opening".to_string(),
            3 => "Closing".to_string(),
            _ => "Error".to_string(),
        })
    }
    
    // =========================================================================
    // UTILITY
    // =========================================================================
    
    fn calculate_altitude(&self, ra_hours: f64, dec_degrees: f64, lat: f64, lon: f64) -> f64 {
        // Calculate Local Sidereal Time
        let now = chrono::Utc::now();
        let jd = julian_day(now);
        let lst = local_sidereal_time(jd, lon);
        
        // Calculate hour angle
        let ha = lst - ra_hours;
        let ha_rad = ha * 15.0_f64.to_radians();
        let dec_rad = dec_degrees.to_radians();
        let lat_rad = lat.to_radians();
        
        // Calculate altitude
        let sin_alt = lat_rad.sin() * dec_rad.sin() + 
                      lat_rad.cos() * dec_rad.cos() * ha_rad.cos();
        sin_alt.asin().to_degrees()
    }
    
    fn get_observer_location(&self) -> Option<(f64, f64)> {
        // Get observer location from app settings
        match self.app_state.get_observer_location() {
            Ok(Some(location)) => {
                tracing::debug!("Observer location retrieved: lat={}, lon={}",
                    location.latitude, location.longitude);
                Some((location.latitude, location.longitude))
            }

        // Map bayer pattern to sensor_type and bayer_offset
            Ok(None) => {
                tracing::debug!("Observer location not set in settings, will retry");
                None
            }

        // Map bayer pattern to sensor_type and bayer_offset
            Err(e) => {
                tracing::warn!("Failed to get observer location: {}", e);
                None
            }

        // Map bayer pattern to sensor_type and bayer_offset
        }

        // Map bayer pattern to sensor_type and bayer_offset
    }

    async fn safety_is_safe(&self, safety_id: Option<&str>) -> DeviceResult<bool> {
        // If no safety monitor specified, check profile
        let device_id = match safety_id {
            Some(id) => id.to_string(),
            None => {
                // Try to get from profile
                let profile = self.app_state.get_profile().await;
                match profile.and_then(|p| p.weather_id) {
                    Some(id) => id,
                    None => {
                        tracing::debug!("No safety monitor configured, assuming safe");
                        return Ok(true);
                    }

        // Map bayer pattern to sensor_type and bayer_offset
                }

        // Map bayer pattern to sensor_type and bayer_offset
            }

        // Map bayer pattern to sensor_type and bayer_offset
        };

        tracing::debug!("Checking safety status for device: {}", device_id);

        // Use DeviceManager which handles all driver types (ASCOM, Alpaca, INDI, Native)
        match get_device_manager().safety_is_safe(&device_id).await {
            Ok(is_safe) => {
                tracing::info!("Safety monitor {} reports: {}", device_id, if is_safe { "SAFE" } else { "UNSAFE" });
                Ok(is_safe)
            }

        // Map bayer pattern to sensor_type and bayer_offset
            Err(e) => {
                tracing::warn!("Failed to check safety monitor {}: {} - assuming safe (fail-open)", device_id, e);
                Ok(true) // Fail-open for safety
            }

        // Map bayer pattern to sensor_type and bayer_offset
        }

        // Map bayer pattern to sensor_type and bayer_offset
    }

    // =========================================================================
    // IMAGE ANALYSIS
    // =========================================================================

    async fn calculate_image_hfr(&self, image_data: &ImageData) -> DeviceResult<Option<f64>> {
        use nightshade_imaging::{detect_stars, StarDetectionConfig};

        // Convert to nightshade_imaging::ImageData
        let img = nightshade_imaging::ImageData::from_u16(
            image_data.width,
            image_data.height,
            1,
            &image_data.data
        );

        let config = StarDetectionConfig::default();
        let stars = detect_stars(&img, &config);

        if stars.is_empty() {
            return Ok(None);
        }

        // Map bayer pattern to sensor_type and bayer_offset

        // Calculate average HFR
        let total_hfr: f64 = stars.iter().map(|s| s.hfr).sum();
        let avg_hfr = total_hfr / stars.len() as f64;

        Ok(Some(avg_hfr))
    }

    async fn detect_stars_in_image(&self, image_data: &ImageData) -> DeviceResult<Vec<(f64, f64, f64)>> {
        use nightshade_imaging::{detect_stars, StarDetectionConfig};

        // Convert to nightshade_imaging::ImageData
        let img = nightshade_imaging::ImageData::from_u16(
            image_data.width,
            image_data.height,
            1,
            &image_data.data
        );

        let config = StarDetectionConfig::default();
        let stars = detect_stars(&img, &config);

        // Convert to (x, y, hfr) tuples
        let result: Vec<(f64, f64, f64)> = stars.iter()
            .map(|s| (s.x, s.y, s.hfr))
            .collect();

        Ok(result)
    }

    // =========================================================================
    // COVER CALIBRATOR (FLAT PANEL) OPERATIONS
    // =========================================================================

    async fn cover_calibrator_open_cover(&self, device_id: &str) -> DeviceResult<()> {
        api_cover_calibrator_open_cover(device_id.to_string())
            .await
            .map_err(|e| format!("Open cover failed: {}", e))
    }

    async fn cover_calibrator_close_cover(&self, device_id: &str) -> DeviceResult<()> {
        api_cover_calibrator_close_cover(device_id.to_string())
            .await
            .map_err(|e| format!("Close cover failed: {}", e))
    }

    async fn cover_calibrator_halt_cover(&self, device_id: &str) -> DeviceResult<()> {
        api_cover_calibrator_halt_cover(device_id.to_string())
            .await
            .map_err(|e| format!("Halt cover failed: {}", e))
    }

    async fn cover_calibrator_calibrator_on(&self, device_id: &str, brightness: i32) -> DeviceResult<()> {
        api_cover_calibrator_calibrator_on(device_id.to_string(), brightness)
            .await
            .map_err(|e| format!("Calibrator on failed: {}", e))
    }

    async fn cover_calibrator_calibrator_off(&self, device_id: &str) -> DeviceResult<()> {
        api_cover_calibrator_calibrator_off(device_id.to_string())
            .await
            .map_err(|e| format!("Calibrator off failed: {}", e))
    }

    async fn cover_calibrator_get_cover_state(&self, device_id: &str) -> DeviceResult<i32> {
        api_cover_calibrator_get_cover_state(device_id.to_string())
            .await
            .map_err(|e| format!("Get cover state failed: {}", e))
    }

    async fn cover_calibrator_get_calibrator_state(&self, device_id: &str) -> DeviceResult<i32> {
        api_cover_calibrator_get_calibrator_state(device_id.to_string())
            .await
            .map_err(|e| format!("Get calibrator state failed: {}", e))
    }

    async fn cover_calibrator_get_brightness(&self, device_id: &str) -> DeviceResult<i32> {
        api_cover_calibrator_get_brightness(device_id.to_string())
            .await
            .map_err(|e| format!("Get brightness failed: {}", e))
    }

    async fn cover_calibrator_get_max_brightness(&self, device_id: &str) -> DeviceResult<i32> {
        api_cover_calibrator_get_max_brightness(device_id.to_string())
            .await
            .map_err(|e| format!("Get max brightness failed: {}", e))
    }
}

/// Calculate Julian Day from UTC datetime
fn julian_day(dt: chrono::DateTime<chrono::Utc>) -> f64 {
    use chrono::{Datelike, Timelike};
    
    let year = dt.year();
    let month = dt.month() as i32;
    let day = dt.day() as f64;
    let hour = dt.hour() as f64 + dt.minute() as f64 / 60.0 + dt.second() as f64 / 3600.0;
    
    let (y, m) = if month <= 2 {
        (year - 1, month + 12)
    } else {
        (year, month)
    };
    
    let a = (y as f64 / 100.0).floor();
    let b = 2.0 - a + (a / 4.0).floor();
    
    (365.25 * (y + 4716) as f64).floor() + 
    (30.6001 * (m + 1) as f64).floor() + 
    day + hour / 24.0 + b - 1524.5
}

/// Calculate Local Sidereal Time in hours
fn local_sidereal_time(jd: f64, longitude: f64) -> f64 {
    let t = (jd - 2451545.0) / 36525.0;
    
    // Greenwich Mean Sidereal Time
    let gmst = 280.46061837 + 360.98564736629 * (jd - 2451545.0) +
               0.000387933 * t * t - t * t * t / 38710000.0;
    
    let lst = (gmst + longitude) % 360.0;
    if lst < 0.0 { (lst + 360.0) / 15.0 } else { lst / 15.0 }
}

// =============================================================================
// FACTORY FUNCTION
// =============================================================================

/// Create a unified DeviceOps instance for the sequencer
///
/// This is the recommended way to get a DeviceOps implementation.
/// It uses the unified implementation that routes through the bridge API.
pub fn create_unified_device_ops() -> Arc<dyn nightshade_sequencer::DeviceOps> {
    Arc::new(UnifiedDeviceOps::new(crate::api::get_state().clone()))
}

/// Create a unified DeviceOps instance with a specific app state
pub fn create_unified_device_ops_with_state(app_state: SharedAppState) -> Arc<dyn nightshade_sequencer::DeviceOps> {
    Arc::new(UnifiedDeviceOps::new(app_state))
}

