//! Device Operations Implementation for the Sequencer
//!
//! This module implements the DeviceOps trait from the sequencer crate,
//! routing calls to actual connected devices via the bridge API.

use async_trait::async_trait;
use nightshade_sequencer::{DeviceOps, DeviceResult, ImageData, PlateSolveResult, GuidingStatus};
use crate::state::SharedAppState;
use crate::api::*;
use crate::event::{EquipmentEvent, EventSeverity};
use std::sync::Arc;

/// Real device operations implementation that uses connected devices
pub struct BridgeDeviceOps {
    app_state: SharedAppState,
}

impl BridgeDeviceOps {
    pub fn new(app_state: SharedAppState) -> Self {
        Self { app_state }
    }
}

#[async_trait]
impl DeviceOps for BridgeDeviceOps {
    // =========================================================================
    // MOUNT OPERATIONS
    // =========================================================================
    
    async fn mount_slew_to_coordinates(&self, mount_id: &str, ra_hours: f64, dec_degrees: f64) -> DeviceResult<()> {
        // Emit start event
        self.app_state.publish_equipment_event(
            EquipmentEvent::MountSlewStarted { ra: ra_hours, dec: dec_degrees },
            EventSeverity::Info,
        );

        tracing::info!("Slewing mount {} to RA={:.4}h Dec={:.4}°", mount_id, ra_hours, dec_degrees);

        let result = mount_slew(mount_id.to_string(), ra_hours, dec_degrees)
            .await
            .map_err(|e| format!("Slew failed: {}", e));

        // Emit completion event on success
        if result.is_ok() {
            self.app_state.publish_equipment_event(
                EquipmentEvent::MountSlewCompleted { ra: ra_hours, dec: dec_degrees },
                EventSeverity::Info,
            );
        }

        result
    }

    async fn mount_abort_slew(&self, mount_id: &str) -> DeviceResult<()> {
        tracing::info!("Aborting slew for mount {}", mount_id);
        
        mount_abort(mount_id.to_string())
            .await
            .map_err(|e| format!("Abort slew failed: {}", e))
    }

    async fn mount_get_coordinates(&self, mount_id: &str) -> DeviceResult<(f64, f64)> {
        mount_get_coordinates(mount_id.to_string())
            .await
            .map_err(|e| format!("Get coordinates failed: {}", e))
    }
    
    async fn mount_sync(&self, mount_id: &str, ra_hours: f64, dec_degrees: f64) -> DeviceResult<()> {
        tracing::info!("Syncing mount {} to RA={:.4}h Dec={:.4}°", mount_id, ra_hours, dec_degrees);
        
        mount_sync(mount_id.to_string(), ra_hours, dec_degrees)
            .await
            .map_err(|e| format!("Sync failed: {}", e))
    }
    
    async fn mount_park(&self, mount_id: &str) -> DeviceResult<()> {
        // Emit start event
        self.app_state.publish_equipment_event(
            EquipmentEvent::MountParkStarted,
            EventSeverity::Info,
        );

        tracing::info!("Parking mount {}", mount_id);

        let result = mount_park(mount_id.to_string())
            .await
            .map_err(|e| format!("Park failed: {}", e));

        // Emit completion event on success
        if result.is_ok() {
            self.app_state.publish_equipment_event(
                EquipmentEvent::MountParkCompleted,
                EventSeverity::Info,
            );
        }

        result
    }
    
    async fn mount_unpark(&self, mount_id: &str) -> DeviceResult<()> {
        tracing::info!("Unparking mount {}", mount_id);

        let result = mount_unpark(mount_id.to_string())
            .await
            .map_err(|e| format!("Unpark failed: {}", e));

        // Emit event on success
        if result.is_ok() {
            self.app_state.publish_equipment_event(
                EquipmentEvent::MountUnparked,
                EventSeverity::Info,
            );
        }

        result
    }
    
    async fn mount_is_slewing(&self, mount_id: &str) -> DeviceResult<bool> {
        let status = mount_get_status(mount_id.to_string())
            .await
            .map_err(|e| format!("Failed to get mount status: {}", e))?;
        
        Ok(status.slewing)
    }
    
    async fn mount_is_parked(&self, mount_id: &str) -> DeviceResult<bool> {
        let status = mount_get_status(mount_id.to_string())
            .await
            .map_err(|e| format!("Failed to get mount status: {}", e))?;

        Ok(status.parked)
    }

    async fn mount_can_flip(&self, mount_id: &str) -> DeviceResult<bool> {
        // Check if mount supports meridian flip by checking if it's a GEM
        let status = mount_get_status(mount_id.to_string())
            .await
            .map_err(|e| format!("Failed to get mount status: {}", e))?;

        // GEM mounts support flipping - assume true if tracking is possible
        Ok(status.tracking)
    }

    async fn mount_side_of_pier(&self, mount_id: &str) -> DeviceResult<nightshade_sequencer::meridian::PierSide> {
        let status = mount_get_status(mount_id.to_string())
            .await
            .map_err(|e| format!("Failed to get mount status: {}", e))?;

        // Convert from bridge PierSide to sequencer PierSide
        use crate::device::PierSide as BridgePierSide;
        use nightshade_sequencer::meridian::PierSide as SeqPierSide;

        Ok(match status.side_of_pier {
            BridgePierSide::East => SeqPierSide::East,
            BridgePierSide::West => SeqPierSide::West,
            BridgePierSide::Unknown => SeqPierSide::Unknown,
        })
    }

    async fn mount_is_tracking(&self, mount_id: &str) -> DeviceResult<bool> {
        let status = mount_get_status(mount_id.to_string())
            .await
            .map_err(|e| format!("Failed to get mount status: {}", e))?;

        Ok(status.tracking)
    }

    async fn mount_set_tracking(&self, mount_id: &str, enabled: bool) -> DeviceResult<()> {
        tracing::info!("Setting tracking {} for mount {}", if enabled { "on" } else { "off" }, mount_id);

        let result = mount_set_tracking(mount_id.to_string(), if enabled { 1 } else { 0 })
            .await
            .map_err(|e| format!("Set tracking failed: {}", e));

        // Emit event on success
        if result.is_ok() {
            self.app_state.publish_equipment_event(
                if enabled {
                    EquipmentEvent::MountTrackingStarted
                } else {
                    EquipmentEvent::MountTrackingStopped
                },
                EventSeverity::Info,
            );
        }

        result
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

        // Set camera parameters if provided - propagate errors instead of ignoring
        if let Some(g) = gain {
            set_camera_gain(camera_id.to_string(), g)
                .await
                .map_err(|e| format!("Failed to set gain: {}", e))?;
        }
        if let Some(o) = offset {
            set_camera_offset(camera_id.to_string(), o)
                .await
                .map_err(|e| format!("Failed to set offset: {}", e))?;
        }
        if bin_x > 1 || bin_y > 1 {
            api_set_camera_binning(camera_id.to_string(), bin_x, bin_y)
                .await
                .map_err(|e| format!("Failed to set binning: {}", e))?;
        }

        // Start the exposure (waits for completion)
        start_exposure(
            camera_id.to_string(),
            duration_secs,
            gain.unwrap_or(0),
            offset.unwrap_or(0),
            bin_x,
            bin_y,
        )
            .await
            .map_err(|e| format!("Exposure failed: {}", e))?;

        // Get the raw image data with metadata - this is the ACTUAL 16-bit sensor data,
        // not the 8-bit display-stretched visualization data
        let raw_info = get_last_raw_image_info()
            .await
            .map_err(|e| format!("Failed to get raw image info: {}", e))?
            .ok_or_else(|| "No raw image data available after exposure".to_string())?;

        // Validate the raw data
        let expected_size = (raw_info.width as usize) * (raw_info.height as usize);
        if raw_info.data.len() != expected_size {
            return Err(format!(
                "Image data size mismatch: got {} pixels, expected {} ({}x{})",
                raw_info.data.len(), expected_size, raw_info.width, raw_info.height
            ));
        }

        // Check for obviously bad frames (all pixels identical = sensor failure or dead frame)
        if raw_info.data.len() > 0 {
            let first_val = raw_info.data[0];
            let all_same = raw_info.data.iter().all(|&v| v == first_val);
            if all_same {
                return Err(format!(
                    "Invalid image: all {} pixels have identical value {} - possible sensor failure or dead frame",
                    raw_info.data.len(), first_val
                ));
            }
        }

        // Get camera status for temperature metadata
        let status = get_camera_status(camera_id.to_string())
            .await
            .ok();

        tracing::info!(
            "Raw image captured: {}x{}, {} pixels, sensor_type={:?}, bayer_offset={:?}",
            raw_info.width, raw_info.height, raw_info.data.len(),
            raw_info.sensor_type, raw_info.bayer_offset
        );

        Ok(ImageData {
            width: raw_info.width,
            height: raw_info.height,
            data: raw_info.data,  // Use ACTUAL raw 16-bit sensor data
            bits_per_pixel: 16,
            exposure_secs: duration_secs,
            gain,
            offset,
            temperature: status.as_ref().and_then(|s| s.sensor_temp),
            filter: None, // Would come from filter wheel
            timestamp: chrono::Utc::now().timestamp(),
            sensor_type: raw_info.sensor_type,
            bayer_offset: raw_info.bayer_offset,
        })
    }
    
    async fn camera_abort_exposure(&self, camera_id: &str) -> DeviceResult<()> {
        tracing::info!("Aborting exposure on camera {}", camera_id);
        
        cancel_exposure(camera_id.to_string())
            .await
            .map_err(|e| format!("Abort failed: {}", e))
    }
    
    async fn camera_set_cooler(&self, camera_id: &str, enabled: bool, target_temp: f64) -> DeviceResult<()> {
        // Emit event before starting
        self.app_state.publish_equipment_event(
            if enabled {
                EquipmentEvent::CameraCoolingStarted { target_temp }
            } else {
                EquipmentEvent::CameraWarmingStarted
            },
            EventSeverity::Info,
        );

        tracing::info!("Camera {} cooler: enabled={}, target={}°C", camera_id, enabled, target_temp);

        set_camera_cooler(camera_id.to_string(), enabled as u8, Some(target_temp))
            .await
            .map_err(|e| format!("Cooler control failed: {}", e))
    }
    
    async fn camera_get_temperature(&self, camera_id: &str) -> DeviceResult<f64> {
        let status = get_camera_status(camera_id.to_string())
            .await
            .map_err(|e| format!("Failed to get camera status: {}", e))?;
        
        status.sensor_temp.ok_or_else(|| "Temperature not available".to_string())
    }
    
    async fn camera_get_cooler_power(&self, camera_id: &str) -> DeviceResult<f64> {
        let status = get_camera_status(camera_id.to_string())
            .await
            .map_err(|e| format!("Failed to get camera status: {}", e))?;
        
        status.cooler_power.ok_or_else(|| "Cooler power not available".to_string())
    }
    
    // =========================================================================
    // FOCUSER OPERATIONS
    // =========================================================================
    
    async fn focuser_move_to(&self, focuser_id: &str, position: i32) -> DeviceResult<()> {
        // Emit start event
        self.app_state.publish_equipment_event(
            EquipmentEvent::FocuserMoveStarted { target_position: position },
            EventSeverity::Info,
        );

        tracing::info!("Moving focuser {} to position {}", focuser_id, position);

        let result = focuser_move_abs(focuser_id.to_string(), position)
            .await
            .map_err(|e| format!("Focuser move failed: {}", e));

        // Emit completion event on success
        if result.is_ok() {
            self.app_state.publish_equipment_event(
                EquipmentEvent::FocuserMoveCompleted { position },
                EventSeverity::Info,
            );
        }

        result
    }
    
    async fn focuser_get_position(&self, focuser_id: &str) -> DeviceResult<i32> {
        let pos = focuser_get_position(focuser_id.to_string())
            .await
            .map_err(|e| format!("Failed to get focuser position: {}", e))?;
        
        Ok(pos)
    }
    
    async fn focuser_is_moving(&self, focuser_id: &str) -> DeviceResult<bool> {
        let status = api_get_focuser_status(focuser_id.to_string())
            .await
            .map_err(|e| format!("Failed to get focuser status: {}", e))?;

        Ok(status.moving)
    }
    
    async fn focuser_get_temperature(&self, focuser_id: &str) -> DeviceResult<Option<f64>> {
        focuser_get_temp(focuser_id.to_string())
            .await
            .map_err(|e| format!("Failed to get focuser temp: {}", e))
    }

    async fn focuser_halt(&self, focuser_id: &str) -> DeviceResult<()> {
        tracing::info!("Halting focuser {}", focuser_id);
        
        focuser_halt(focuser_id.to_string())
            .await
            .map_err(|e| format!("Halt failed: {}", e))
    }
    
    // =========================================================================
    // FILTER WHEEL OPERATIONS
    // =========================================================================
    
    async fn filterwheel_set_position(&self, fw_id: &str, position: i32) -> DeviceResult<()> {
        // Get current position for event
        let from_position = filter_wheel_get_position(fw_id.to_string())
            .await
            .unwrap_or(0);

        // Emit changing event
        self.app_state.publish_equipment_event(
            EquipmentEvent::FilterChanging {
                from_position,
                to_position: position,
                filter_name: None,
            },
            EventSeverity::Info,
        );

        tracing::info!("Setting filter wheel {} to position {}", fw_id, position);

        let result = filter_wheel_set_position(fw_id.to_string(), position)
            .await
            .map_err(|e| format!("Filter change failed: {}", e));

        // Emit changed event on success
        if result.is_ok() {
            self.app_state.publish_equipment_event(
                EquipmentEvent::FilterChanged {
                    position,
                    filter_name: None,
                },
                EventSeverity::Info,
            );
        }

        result
    }
    
    async fn filterwheel_get_position(&self, fw_id: &str) -> DeviceResult<i32> {
        filter_wheel_get_position(fw_id.to_string())
            .await
            .map_err(|e| format!("Failed to get filter wheel position: {}", e))
    }
    
    async fn filterwheel_get_names(&self, fw_id: &str) -> DeviceResult<Vec<String>> {
        let (_, names) = filter_wheel_get_config(fw_id.to_string())
            .await
            .map_err(|e| format!("Failed to get filter wheel config: {}", e))?;
        
        Ok(names)
    }
    
    async fn filterwheel_set_filter_by_name(&self, fw_id: &str, name: &str) -> DeviceResult<i32> {
        let names = self.filterwheel_get_names(fw_id).await?;
        
        // Find the filter position by name (case-insensitive)
        let position = names.iter()
            .position(|n| n.eq_ignore_ascii_case(name))
            .map(|p| (p + 1) as i32) // Filter positions are 1-based
            .ok_or_else(|| format!("Filter '{}' not found", name))?;
        
        self.filterwheel_set_position(fw_id, position).await?;
        Ok(position)
    }
    
    // =========================================================================
    // ROTATOR OPERATIONS
    // =========================================================================

    async fn rotator_move_to(&self, rotator_id: &str, angle: f64) -> DeviceResult<()> {
        // Emit start event
        self.app_state.publish_equipment_event(
            EquipmentEvent::RotatorMoveStarted { target_angle: angle },
            EventSeverity::Info,
        );

        tracing::info!("Moving rotator {} to {}°", rotator_id, angle);

        let result = api_rotator_move_to(rotator_id.to_string(), angle)
            .await
            .map_err(|e| format!("Rotator move failed: {}", e));

        // Emit completion event on success
        if result.is_ok() {
            self.app_state.publish_equipment_event(
                EquipmentEvent::RotatorMoveCompleted { angle },
                EventSeverity::Info,
            );
        }

        result
    }

    async fn rotator_move_relative(&self, rotator_id: &str, delta: f64) -> DeviceResult<()> {
        tracing::info!("Moving rotator {} by {}°", rotator_id, delta);

        api_rotator_move_relative(rotator_id.to_string(), delta)
            .await
            .map_err(|e| format!("Rotator relative move failed: {}", e))
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
            .map_err(|e| format!("Rotator halt failed: {}", e))
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
            .map_err(|e| format!("Failed to get guider status: {}", e))?;

        Ok(GuidingStatus {
            is_guiding: status.state == "Guiding",
            rms_ra: status.rms_ra,
            rms_dec: status.rms_dec,
            rms_total: status.rms_total,
        })
    }

    async fn guider_start(&self, settle_pixels: f64, settle_time: f64, settle_timeout: f64) -> DeviceResult<()> {
        tracing::info!("Starting guiding (settle: <{}px in {}s, timeout {}s)", settle_pixels, settle_time, settle_timeout);

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
        
        // Convert to imaging::ImageData
        let img = nightshade_imaging::ImageData::from_u16(
            image_data.width,
            image_data.height,
            1, // Assuming mono/bayer raw
            &image_data.data
        );
        
        // Create header
        let mut header = nightshade_imaging::FitsHeader::new();
        header.set_float("EXPTIME", image_data.exposure_secs);
        if let Some(gain) = image_data.gain {
            header.set_int("GAIN", gain as i64);
        }
        if let Some(offset) = image_data.offset {
            header.set_int("OFFSET", offset as i64);
        }
        if let Some(temp) = image_data.temperature {
            header.set_float("CCD-TEMP", temp);
        }
        if let Some(ra) = hint_ra {
            header.set_float("RA", ra / 15.0); // Hours
        }
        if let Some(dec) = hint_dec {
            header.set_float("DEC", dec);
        }
        if let Some(scale) = hint_scale {
            // Approximate focal length from scale (assuming 3.76um pixels)
            let focal_len = 206.265 * 3.76 / scale;
            header.set_float("FOCALLEN", focal_len);
        }
        
        // Save temp FITS
        nightshade_imaging::write_fits(std::path::Path::new(&temp_path), &img, &header)
            .map_err(|e| format!("Failed to save temp FITS: {}", e))?;
            
        tracing::info!("Saved temp FITS for plate solving: {}", temp_path);
        
        // Run solver
        let result = if let (Some(ra), Some(dec)) = (hint_ra, hint_dec) {
            nightshade_imaging::solve_near(
                std::path::Path::new(&temp_path),
                ra,
                dec,
                5.0, // 5 degree search radius
            )
        } else {
            nightshade_imaging::blind_solve(std::path::Path::new(&temp_path))
        };
        
        // Clean up
        let _ = std::fs::remove_file(&temp_path);
        
        let r = result; // No need to map error, it returns PlateSolveResult directly
        
        if r.success {
            Ok(PlateSolveResult {
                ra_degrees: r.ra,
                dec_degrees: r.dec,
                pixel_scale: r.pixel_scale,
                rotation: r.rotation,
                success: true,
            })
        } else {
            tracing::warn!("Plate solve failed: {:?}", r.error);
            Err(r.error.unwrap_or_else(|| "Unknown plate solve error".to_string()))
        }
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
        
        // Convert to imaging::ImageData
        let img = nightshade_imaging::ImageData::from_u16(
            image_data.width,
            image_data.height,
            1,
            &image_data.data
        );
        
        // Create header
        let mut header = nightshade_imaging::FitsHeader::new();
        if let Some(name) = target_name {
            header.set_string("OBJECT", name);
        }
        header.set_float("EXPTIME", image_data.exposure_secs);
        if let Some(f) = filter {
            header.set_string("FILTER", f);
        }
        if let Some(gain) = image_data.gain {
            header.set_int("GAIN", gain as i64);
        }
        if let Some(offset) = image_data.offset {
            header.set_int("OFFSET", offset as i64);
        }
        if let Some(temp) = image_data.temperature {
            header.set_float("CCD-TEMP", temp);
        }
        if let Some(ra) = ra_hours {
            header.set_float("RA", ra);
        }
        if let Some(dec) = dec_degrees {
            header.set_float("DEC", dec);
        }
        header.set_string("DATE-OBS", &chrono::Utc::now().to_rfc3339());
        
        // Save FITS
        nightshade_imaging::write_fits(std::path::Path::new(file_path), &img, &header)
            .map_err(|e| format!("Save FITS failed: {}", e))
    }
    
    // =========================================================================
    // NOTIFICATIONS
    // =========================================================================
    
    async fn send_notification(&self, level: &str, title: &str, message: &str) -> DeviceResult<()> {
        tracing::info!("[NOTIFICATION][{}] {}: {}", level.to_uppercase(), title, message);
        
        // Publish as event to the event bus
        use crate::event::*;
        
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

    async fn polar_align_update(&self, result: &nightshade_sequencer::PolarAlignResult) -> DeviceResult<()> {
        tracing::info!("Polar Align Update: Alt {:.1}', Az {:.1}'", result.altitude_error, result.azimuth_error);
        
        use crate::event::*;
        
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

        #[cfg(windows)]
        {
            if dome_id.starts_with("ascom:") {
                let prog_id = dome_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();

                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let dome = nightshade_ascom::AscomDome::new(&prog_id)?;
                        dome.open_shutter()?;
                        Ok(())
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }

        if dome_id.starts_with("alpaca:") {
            let id_str = dome_id.strip_prefix("alpaca:").unwrap_or("");
            let parts: Vec<&str> = id_str.split(':').collect();

            if parts.len() >= 5 {
                let protocol = parts[0];
                let host_part = parts[1].trim_start_matches("//");
                let port = parts[2];
                let device_num: u32 = parts[4].parse().unwrap_or(0);

                let base_url = format!("{}://{}:{}", protocol, host_part, port);
                let dome = nightshade_alpaca::AlpacaDome::from_server(&base_url, device_num);
                dome.connect().await?;
                dome.open_shutter().await?;
                dome.disconnect().await.ok();
                return Ok(());
            }
        }

        Err(format!("Dome {} not found or unsupported", dome_id))
    }

    async fn dome_close(&self, dome_id: &str) -> DeviceResult<()> {
        tracing::info!("Closing dome shutter {}", dome_id);

        #[cfg(windows)]
        {
            if dome_id.starts_with("ascom:") {
                let prog_id = dome_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();

                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let dome = nightshade_ascom::AscomDome::new(&prog_id)?;
                        dome.close_shutter()?;
                        Ok(())
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }

        if dome_id.starts_with("alpaca:") {
            let id_str = dome_id.strip_prefix("alpaca:").unwrap_or("");
            let parts: Vec<&str> = id_str.split(':').collect();

            if parts.len() >= 5 {
                let protocol = parts[0];
                let host_part = parts[1].trim_start_matches("//");
                let port = parts[2];
                let device_num: u32 = parts[4].parse().unwrap_or(0);

                let base_url = format!("{}://{}:{}", protocol, host_part, port);
                let dome = nightshade_alpaca::AlpacaDome::from_server(&base_url, device_num);
                dome.connect().await?;
                dome.close_shutter().await?;
                dome.disconnect().await.ok();
                return Ok(());
            }
        }

        Err(format!("Dome {} not found or unsupported", dome_id))
    }

    async fn dome_park(&self, dome_id: &str) -> DeviceResult<()> {
        tracing::info!("Parking dome {}", dome_id);

        #[cfg(windows)]
        {
            if dome_id.starts_with("ascom:") {
                let prog_id = dome_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();

                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let dome = nightshade_ascom::AscomDome::new(&prog_id)?;
                        dome.park()?;
                        Ok(())
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }

        if dome_id.starts_with("alpaca:") {
            let id_str = dome_id.strip_prefix("alpaca:").unwrap_or("");
            let parts: Vec<&str> = id_str.split(':').collect();

            if parts.len() >= 5 {
                let protocol = parts[0];
                let host_part = parts[1].trim_start_matches("//");
                let port = parts[2];
                let device_num: u32 = parts[4].parse().unwrap_or(0);

                let base_url = format!("{}://{}:{}", protocol, host_part, port);
                let dome = nightshade_alpaca::AlpacaDome::from_server(&base_url, device_num);
                dome.connect().await?;
                dome.park().await?;
                dome.disconnect().await.ok();
                return Ok(());
            }
        }

        Err(format!("Dome {} not found or unsupported", dome_id))
    }

    async fn dome_get_shutter_status(&self, dome_id: &str) -> DeviceResult<String> {
        #[cfg(windows)]
        {
            if dome_id.starts_with("ascom:") {
                let prog_id = dome_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();

                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let dome = nightshade_ascom::AscomDome::new(&prog_id)?;
                        let status = dome.shutter_status()?;
                        // ASCOM ShutterStatus: 0=Open, 1=Closed, 2=Opening, 3=Closing, 4=Error
                        Ok(match status {
                            0 => "Open".to_string(),
                            1 => "Closed".to_string(),
                            2 => "Opening".to_string(),
                            3 => "Closing".to_string(),
                            _ => "Error".to_string(),
                        })
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }

        if dome_id.starts_with("alpaca:") {
            let id_str = dome_id.strip_prefix("alpaca:").unwrap_or("");
            let parts: Vec<&str> = id_str.split(':').collect();

            if parts.len() >= 5 {
                let protocol = parts[0];
                let host_part = parts[1].trim_start_matches("//");
                let port = parts[2];
                let device_num: u32 = parts[4].parse().unwrap_or(0);

                let base_url = format!("{}://{}:{}", protocol, host_part, port);
                let dome = nightshade_alpaca::AlpacaDome::from_server(&base_url, device_num);
                dome.connect().await?;
                let status = dome.shutter_status().await?;
                dome.disconnect().await.ok();

                return Ok(match status {
                    nightshade_alpaca::ShutterStatus::Open => "Open".to_string(),
                    nightshade_alpaca::ShutterStatus::Closed => "Closed".to_string(),
                    nightshade_alpaca::ShutterStatus::Opening => "Opening".to_string(),
                    nightshade_alpaca::ShutterStatus::Closing => "Closing".to_string(),
                    nightshade_alpaca::ShutterStatus::Error => "Error".to_string(),
                });
            }
        }

        Err(format!("Dome {} not found or unsupported", dome_id))
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
        let ha_rad = ha * 15.0_f64.to_radians(); // Convert to radians
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
            Ok(None) => {
                tracing::debug!("Observer location not set in settings, will retry");
                None
            }
            Err(e) => {
                tracing::warn!("Failed to get observer location: {}", e);
                None
            }
        }
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
                }
            }
        };

        tracing::debug!("Checking safety status for device: {}", device_id);

        // Alpaca Safety Monitor
        if device_id.starts_with("alpaca:") {
            let id_str = device_id.strip_prefix("alpaca:").unwrap_or("");
            let parts: Vec<&str> = id_str.split(':').collect();

            if parts.len() >= 5 {
                let protocol = parts[0];
                let host_part = parts[1].trim_start_matches("//");
                let port = parts[2];
                let device_num: u32 = parts[4].parse().unwrap_or(0);

                let base_url = format!("{}://{}:{}", protocol, host_part, port);
                let safety = nightshade_alpaca::AlpacaSafetyMonitor::from_server(&base_url, device_num);

                match safety.connect().await {
                    Ok(()) => {
                        let is_safe = safety.is_safe().await.unwrap_or(true);
                        safety.disconnect().await.ok();
                        tracing::info!("Safety monitor {} reports: {}", device_id, if is_safe { "SAFE" } else { "UNSAFE" });
                        return Ok(is_safe);
                    }
                    Err(e) => {
                        tracing::warn!("Failed to connect to safety monitor {}: {}", device_id, e);
                        return Ok(true); // Fail-open
                    }
                }
            }
        }

        // Unknown device type, assume safe
        tracing::debug!("Unknown safety monitor type for {}, assuming safe", device_id);
        Ok(true)
    }

    // =========================================================================
    // IMAGE ANALYSIS
    // =========================================================================

    async fn calculate_image_hfr(&self, image_data: &ImageData) -> DeviceResult<Option<f64>> {
        // Use nightshade_imaging to calculate HFR
        let img = nightshade_imaging::ImageData::from_u16(
            image_data.width,
            image_data.height,
            1,
            &image_data.data
        );

        let config = nightshade_imaging::StarDetectionConfig::default();
        let stars = nightshade_imaging::detect_stars(&img, &config);

        if stars.is_empty() {
            return Ok(None);
        }

        // Calculate average HFR
        let total_hfr: f64 = stars.iter().map(|s| s.hfr).sum();
        let avg_hfr = total_hfr / stars.len() as f64;

        Ok(Some(avg_hfr))
    }

    async fn detect_stars_in_image(&self, image_data: &ImageData) -> DeviceResult<Vec<(f64, f64, f64)>> {
        // Use nightshade_imaging to detect stars
        let img = nightshade_imaging::ImageData::from_u16(
            image_data.width,
            image_data.height,
            1,
            &image_data.data
        );

        let config = nightshade_imaging::StarDetectionConfig::default();
        let stars = nightshade_imaging::detect_stars(&img, &config);

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
        tracing::info!("Opening cover calibrator cover: {}", device_id);
        api_cover_calibrator_open_cover(device_id.to_string())
            .await
            .map_err(|e| format!("Open cover failed: {}", e))
    }

    async fn cover_calibrator_close_cover(&self, device_id: &str) -> DeviceResult<()> {
        tracing::info!("Closing cover calibrator cover: {}", device_id);
        api_cover_calibrator_close_cover(device_id.to_string())
            .await
            .map_err(|e| format!("Close cover failed: {}", e))
    }

    async fn cover_calibrator_halt_cover(&self, device_id: &str) -> DeviceResult<()> {
        tracing::info!("Halting cover calibrator cover: {}", device_id);
        api_cover_calibrator_halt_cover(device_id.to_string())
            .await
            .map_err(|e| format!("Halt cover failed: {}", e))
    }

    async fn cover_calibrator_calibrator_on(&self, device_id: &str, brightness: i32) -> DeviceResult<()> {
        tracing::info!("Turning on calibrator {} at brightness {}", device_id, brightness);
        api_cover_calibrator_calibrator_on(device_id.to_string(), brightness)
            .await
            .map_err(|e| format!("Calibrator on failed: {}", e))
    }

    async fn cover_calibrator_calibrator_off(&self, device_id: &str) -> DeviceResult<()> {
        tracing::info!("Turning off calibrator {}", device_id);
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

/// Create a BridgeDeviceOps from the global app state
pub fn create_device_ops() -> Arc<dyn nightshade_sequencer::DeviceOps> {
    Arc::new(BridgeDeviceOps::new(crate::api::get_state().clone()))
}

