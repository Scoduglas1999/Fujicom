//! Real Device Operations Implementation
//!
//! Connects the sequencer engine to actual ASCOM/Alpaca devices via the DeviceManager.

use nightshade_sequencer::{DeviceOps, DeviceResult, ImageData as SeqImageData, PlateSolveResult, GuidingStatus};
use crate::api::{get_sim_focuser, get_sim_rotator};
use crate::state::SharedAppState;
use crate::devices::DeviceManager;
use crate::api::get_device_manager;
use crate::device::FilterWheelStatus;
use async_trait::async_trait;
use std::sync::Arc;
use std::collections::HashMap;
use tokio::sync::RwLock;
use chrono::{Datelike, Timelike};
use nightshade_native::traits::NativeCamera;
use nightshade_native::camera::ExposureParams;

#[cfg(windows)]
use nightshade_ascom::*;
use nightshade_alpaca::*;
use nightshade_indi::*;

/// Tracks filter wheel movement state
#[derive(Debug, Clone)]
struct FilterWheelMovement {
    target_position: i32,
    started_at: std::time::Instant,
}

/// Production device operations implementation
pub struct RealDeviceOps {
    app_state: SharedAppState,
    device_manager: Arc<DeviceManager>,
    /// Tracks pending filter wheel movements (device_id -> target position)
    fw_movements: Arc<RwLock<HashMap<String, FilterWheelMovement>>>,
    /// Cached equipment profile to avoid blocking calls
    /// Updated when profile changes (via set_profile)
    cached_profile: Arc<RwLock<Option<crate::state::EquipmentProfile>>>,
}

impl RealDeviceOps {
    pub fn new(app_state: SharedAppState, device_manager: Arc<DeviceManager>) -> Self {
        Self {
            app_state,
            device_manager,
            fw_movements: Arc::new(RwLock::new(HashMap::new())),
            cached_profile: Arc::new(RwLock::new(None)),
        }
    }

    /// Update the cached profile
    /// Call this when the equipment profile changes
    pub async fn update_cached_profile(&self) {
        if let Some(profile) = self.app_state.get_profile().await {
            let mut cached = self.cached_profile.write().await;
            *cached = Some(profile);
        }
    }

    /// Set a cached profile directly
    pub async fn set_cached_profile(&self, profile: Option<crate::state::EquipmentProfile>) {
        let mut cached = self.cached_profile.write().await;
        *cached = profile;
    }

    /// Helper to get device ID from equipment profile (async version - preferred)
    pub async fn get_device_id_async(&self, device_type: crate::device::DeviceType) -> Option<String> {
        use crate::device::DeviceType;

        // First try cached profile
        {
            let cached = self.cached_profile.read().await;
            if let Some(ref profile) = *cached {
                return match device_type {
                    DeviceType::Camera => profile.camera_id.clone(),
                    DeviceType::Mount => profile.mount_id.clone(),
                    DeviceType::Focuser => profile.focuser_id.clone(),
                    DeviceType::FilterWheel => profile.filter_wheel_id.clone(),
                    DeviceType::Rotator => profile.rotator_id.clone(),
                    DeviceType::Dome => profile.dome_id.clone(),
                    DeviceType::Weather => profile.weather_id.clone(),
                    _ => None,
                };
            }
        }

        // Fall back to fetching from app state
        let profile = self.app_state.get_profile().await?;

        // Update cache for next time
        {
            let mut cached = self.cached_profile.write().await;
            *cached = Some(profile.clone());
        }

        match device_type {
            DeviceType::Camera => profile.camera_id,
            DeviceType::Mount => profile.mount_id,
            DeviceType::Focuser => profile.focuser_id,
            DeviceType::FilterWheel => profile.filter_wheel_id,
            DeviceType::Rotator => profile.rotator_id,
            DeviceType::Dome => profile.dome_id,
            DeviceType::Weather => profile.weather_id,
            _ => None,
        }
    }

    /// Helper to get device ID from equipment profile (sync version)
    ///
    /// WARNING: This method should only be called from sync contexts.
    /// It uses try_read to avoid blocking and returns None if the lock is busy.
    /// For async contexts, use `get_device_id_async` instead.
    fn get_device_id(&self, device_type: crate::device::DeviceType) -> Option<String> {
        use crate::device::DeviceType;

        // First try the cached profile with non-blocking read
        if let Ok(cached) = self.cached_profile.try_read() {
            if let Some(ref profile) = *cached {
                return match device_type {
                    DeviceType::Camera => profile.camera_id.clone(),
                    DeviceType::Mount => profile.mount_id.clone(),
                    DeviceType::Focuser => profile.focuser_id.clone(),
                    DeviceType::FilterWheel => profile.filter_wheel_id.clone(),
                    DeviceType::Rotator => profile.rotator_id.clone(),
                    DeviceType::Dome => profile.dome_id.clone(),
                    DeviceType::Weather => profile.weather_id.clone(),
                    _ => None,
                };
            }
        }

        // If we have no cached profile and we're in an async context,
        // we need to be careful about blocking. Check if we have a runtime.
        if let Ok(handle) = tokio::runtime::Handle::try_current() {
            // We have a runtime handle - use spawn_blocking to avoid deadlock
            // But since this is sync, we need to block on the result
            // This is still potentially problematic, so prefer async version
            let app_state = self.app_state.clone();
            let cached_profile = self.cached_profile.clone();

            // Use block_in_place which is safe when called from a blocking task
            // but NOT safe from an async task
            let profile = std::thread::spawn(move || {
                // Create a new runtime for this thread to avoid blocking the main one
                let rt = tokio::runtime::Builder::new_current_thread()
                    .enable_all()
                    .build()
                    .ok()?;

                rt.block_on(async {
                    let profile = app_state.get_profile().await?;
                    // Update cache
                    let mut cached = cached_profile.write().await;
                    *cached = Some(profile.clone());
                    Some(profile)
                })
            })
            .join()
            .ok()
            .flatten()?;

            return match device_type {
                DeviceType::Camera => profile.camera_id,
                DeviceType::Mount => profile.mount_id,
                DeviceType::Focuser => profile.focuser_id,
                DeviceType::FilterWheel => profile.filter_wheel_id,
                DeviceType::Rotator => profile.rotator_id,
                DeviceType::Dome => profile.dome_id,
                DeviceType::Weather => profile.weather_id,
                _ => None,
            };
        }

        // No runtime and no cached profile - return None
        tracing::debug!("get_device_id: No runtime and no cached profile for {:?}", device_type);
        None
    }


    pub async fn filterwheel_is_moving(&self, fw_id: &str) -> DeviceResult<bool> {
        // Check if we have a pending movement for this filter wheel
        let movements = self.fw_movements.read().await;
        if let Some(movement) = movements.get(fw_id) {
            // Check if we've timed out (filter wheels shouldn't take more than 30s)
            let elapsed = movement.started_at.elapsed();
            if elapsed > std::time::Duration::from_secs(30) {
                drop(movements);
                // Timeout - clear the movement and return false
                self.fw_movements.write().await.remove(fw_id);
                tracing::warn!("Filter wheel {} movement timed out after {:?}", fw_id, elapsed);
                return Ok(false);
            }

            // Get current position and compare to target
            let target = movement.target_position;
            drop(movements); // Release read lock before calling get_position

            let current = self.filterwheel_get_position(fw_id).await?;

            if current == target {
                // Movement complete - clear the tracking
                self.fw_movements.write().await.remove(fw_id);
                tracing::debug!("Filter wheel {} reached position {}", fw_id, target);
                return Ok(false);
            }

            // Still moving
            return Ok(true);
        }

        // No pending movement tracked
        Ok(false)
    }

    /// Starts tracking a filter wheel movement
    async fn start_fw_movement(&self, fw_id: &str, target_position: i32) {
        let movement = FilterWheelMovement {
            target_position,
            started_at: std::time::Instant::now(),
        };
        self.fw_movements.write().await.insert(fw_id.to_string(), movement);
        tracing::debug!("Started tracking filter wheel {} movement to position {}", fw_id, target_position);
    }

    /// Waits for filter wheel to reach target position
    pub async fn filterwheel_wait_for_move(&self, fw_id: &str) -> DeviceResult<()> {
        let timeout = std::time::Duration::from_secs(30);
        let poll_interval = std::time::Duration::from_millis(250);
        let start = std::time::Instant::now();

        while self.filterwheel_is_moving(fw_id).await? {
            if start.elapsed() > timeout {
                return Err(format!("Filter wheel {} move timed out after {:?}", fw_id, timeout));
            }
            tokio::time::sleep(poll_interval).await;
        }

        Ok(())
    }

    pub async fn filterwheel_get_status(&self, fw_id: &str) -> DeviceResult<FilterWheelStatus> {
        // We need to call methods from DeviceOps trait, but we are inside inherent impl.
        // Since RealDeviceOps implements DeviceOps, we can call them.
        // However, to avoid ambiguity or issues, we might need to use fully qualified syntax or just ensure visibility.
        // Actually, since we are in the same struct, self.method() works if the trait is in scope.
        // DeviceOps is imported.
        
        let position = self.filterwheel_get_position(fw_id).await?;
        let moving = self.filterwheel_is_moving(fw_id).await?;
        let names = self.filterwheel_get_names(fw_id).await?;
        
        Ok(FilterWheelStatus {
            connected: true,
            position,
            moving,
            filter_count: names.len() as i32,
            filter_names: names,
        })
    }
    
    pub async fn filterwheel_move(&self, fw_id: &str, position: i32) -> DeviceResult<()> {
        self.filterwheel_set_position(fw_id, position).await
    }
}

#[async_trait]
impl DeviceOps for RealDeviceOps {
    // =========================================================================
    // MOUNT OPERATIONS
    // =========================================================================
    
    async fn mount_slew_to_coordinates(&self, mount_id: &str, ra_hours: f64, dec_degrees: f64) -> DeviceResult<()> {
        tracing::info!("Slewing mount {} to RA={:.4}h, Dec={:.4}°", mount_id, ra_hours, dec_degrees);
        
        #[cfg(windows)]
        {
            // Try ASCOM first
            if mount_id.starts_with("ascom:") {
                let prog_id = mount_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();
                
                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let mut mount = AscomMount::new(&prog_id)?;
                        mount.slew_to_coordinates(ra_hours, dec_degrees)?;
                        // Wait for slew to complete loop needs to be here or handle async differently
                        // Blocking wait inside spawn_blocking is fine
                        while mount.slewing()? {
                            std::thread::sleep(std::time::Duration::from_millis(500));
                        }
                        Ok(())
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }
        
        // Alpaca mount
        if mount_id.starts_with("alpaca:") {
            let id_str = mount_id.strip_prefix("alpaca:").unwrap_or("");
            let parts: Vec<&str> = id_str.split(':').collect();

            if parts.len() >= 5 {
                let protocol = parts[0];
                let host_part = parts[1].trim_start_matches("//");
                let port = parts[2];
                let _device_type = parts[3];
                let device_num: u32 = parts[4].parse().unwrap_or(0);

                let base_url = format!("{}://{}:{}", protocol, host_part, port);
                let mount = AlpacaTelescope::from_server(&base_url, device_num);
                mount.connect().await?;

                // Use guard to ensure disconnect on any error
                let result = with_alpaca_connection(&mount, "mount_slew_to_coordinates", async {
                    mount.set_target_right_ascension(ra_hours).await.map_err(|e| e.to_string())?;
                    mount.set_target_declination(dec_degrees).await.map_err(|e| e.to_string())?;
                    mount.slew_to_target().await.map_err(|e| e.to_string())?;

                    // Wait for slew to complete
                    while mount.slewing().await.map_err(|e| e.to_string())? {
                        tokio::time::sleep(std::time::Duration::from_millis(500)).await;
                    }
                    Ok::<(), String>(())
                }).await;

                return result;
            }
        }
        
        Err(format!("Mount {} not found or unsupported", mount_id))
    }
    
    async fn mount_get_coordinates(&self, mount_id: &str) -> DeviceResult<(f64, f64)> {
        #[cfg(windows)]
        {
            if mount_id.starts_with("ascom:") {
                let prog_id = mount_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();
                
                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let mount = AscomMount::new(&prog_id)?;
                        let ra = mount.right_ascension()?;
                        let dec = mount.declination()?;
                        Ok((ra, dec))
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }
        
        if mount_id.starts_with("alpaca:") {
            let id_str = mount_id.strip_prefix("alpaca:").unwrap_or("");
            let parts: Vec<&str> = id_str.split(':').collect();

            if parts.len() >= 5 {
                let protocol = parts[0];
                let host_part = parts[1].trim_start_matches("//");
                let port = parts[2];
                let device_num: u32 = parts[4].parse().unwrap_or(0);

                let base_url = format!("{}://{}:{}", protocol, host_part, port);
                let mount = AlpacaTelescope::from_server(&base_url, device_num);
                mount.connect().await?;

                // Use guard to ensure disconnect on any error
                let result = with_alpaca_connection(&mount, "mount_get_coordinates", async {
                    let ra = mount.right_ascension().await.map_err(|e| e.to_string())?;
                    let dec = mount.declination().await.map_err(|e| e.to_string())?;
                    Ok::<(f64, f64), String>((ra, dec))
                }).await;

                return result;
            }
        }

        Err(format!("Mount {} not found or unsupported", mount_id))
    }

    async fn mount_sync(&self, mount_id: &str, ra_hours: f64, dec_degrees: f64) -> DeviceResult<()> {
        #[cfg(windows)]
        {
            if mount_id.starts_with("ascom:") {
                let prog_id = mount_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();
                
                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let mut mount = AscomMount::new(&prog_id)?;
                        mount.sync_to_coordinates(ra_hours, dec_degrees)?;
                        Ok(())
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }
        
        if mount_id.starts_with("alpaca:") {
            let id_str = mount_id.strip_prefix("alpaca:").unwrap_or("");
            let parts: Vec<&str> = id_str.split(':').collect();

            if parts.len() >= 5 {
                let protocol = parts[0];
                let host_part = parts[1].trim_start_matches("//");
                let port = parts[2];
                let device_num: u32 = parts[4].parse().unwrap_or(0);

                let base_url = format!("{}://{}:{}", protocol, host_part, port);
                let mount = AlpacaTelescope::from_server(&base_url, device_num);
                mount.connect().await?;

                // Use guard to ensure disconnect on any error
                let result = with_alpaca_connection(&mount, "mount_sync", async {
                    mount.set_target_right_ascension(ra_hours).await.map_err(|e| e.to_string())?;
                    mount.set_target_declination(dec_degrees).await.map_err(|e| e.to_string())?;
                    mount.sync_to_target().await.map_err(|e| e.to_string())?;
                    Ok::<(), String>(())
                }).await;

                return result;
            }
        }

        Err(format!("Mount {} not found or unsupported", mount_id))
    }

    async fn mount_park(&self, mount_id: &str) -> DeviceResult<()> {
        tracing::info!("Parking mount {}", mount_id);
        
        #[cfg(windows)]
        {
            if mount_id.starts_with("ascom:") {
                let prog_id = mount_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();
                
                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let mut mount = AscomMount::new(&prog_id)?;
                        mount.park()?;
                        Ok(())
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }
        
        if mount_id.starts_with("alpaca:") {
            let id_str = mount_id.strip_prefix("alpaca:").unwrap_or("");
            let parts: Vec<&str> = id_str.split(':').collect();

            if parts.len() >= 5 {
                let protocol = parts[0];
                let host_part = parts[1].trim_start_matches("//");
                let port = parts[2];
                let device_num: u32 = parts[4].parse().unwrap_or(0);

                let base_url = format!("{}://{}:{}", protocol, host_part, port);
                let mount = AlpacaTelescope::from_server(&base_url, device_num);
                mount.connect().await?;

                // Use guard to ensure disconnect on any error
                let result = with_alpaca_connection(&mount, "mount_park", async {
                    mount.park().await.map_err(|e| e.to_string())?;

                    // Wait for park to complete
                    while mount.slewing().await.map_err(|e| e.to_string())? {
                        tokio::time::sleep(std::time::Duration::from_millis(500)).await;
                    }
                    Ok::<(), String>(())
                }).await;

                return result;
            }
        }

        Err(format!("Mount {} not found or unsupported", mount_id))
    }

    async fn mount_unpark(&self, mount_id: &str) -> DeviceResult<()> {
        tracing::info!("Unparking mount {}", mount_id);
        
        #[cfg(windows)]
        {
            if mount_id.starts_with("ascom:") {
                let prog_id = mount_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();
                
                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let mut mount = AscomMount::new(&prog_id)?;
                        mount.unpark()?;
                        Ok(())
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }
        
        if mount_id.starts_with("alpaca:") {
            let id_str = mount_id.strip_prefix("alpaca:").unwrap_or("");
            let parts: Vec<&str> = id_str.split(':').collect();

            if parts.len() >= 5 {
                let protocol = parts[0];
                let host_part = parts[1].trim_start_matches("//");
                let port = parts[2];
                let device_num: u32 = parts[4].parse().unwrap_or(0);

                let base_url = format!("{}://{}:{}", protocol, host_part, port);
                let mount = AlpacaTelescope::from_server(&base_url, device_num);
                mount.connect().await?;

                // Use guard to ensure disconnect on any error
                let result = with_alpaca_connection(&mount, "mount_unpark", async {
                    mount.unpark().await.map_err(|e| e.to_string())?;
                    Ok::<(), String>(())
                }).await;

                return result;
            }
        }

        Err(format!("Mount {} not found or unsupported", mount_id))
    }

    async fn mount_is_slewing(&self, mount_id: &str) -> DeviceResult<bool> {
        #[cfg(windows)]
        {
            if mount_id.starts_with("ascom:") {
                let prog_id = mount_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();
                
                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let mount = AscomMount::new(&prog_id)?;
                        Ok(mount.slewing()?)
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }
        
        if mount_id.starts_with("alpaca:") {
            let id_str = mount_id.strip_prefix("alpaca:").unwrap_or("");
            let parts: Vec<&str> = id_str.split(':').collect();

            if parts.len() >= 5 {
                let protocol = parts[0];
                let host_part = parts[1].trim_start_matches("//");
                let port = parts[2];
                let device_num: u32 = parts[4].parse().unwrap_or(0);

                let base_url = format!("{}://{}:{}", protocol, host_part, port);
                let mount = AlpacaTelescope::from_server(&base_url, device_num);
                mount.connect().await?;

                // Use guard to ensure disconnect on any error
                let result = with_alpaca_connection(&mount, "mount_is_slewing", async {
                    let slewing = mount.slewing().await.map_err(|e| e.to_string())?;
                    Ok::<bool, String>(slewing)
                }).await;

                return result;
            }
        }

        Err(format!("Mount {} not found or unsupported", mount_id))
    }

    async fn mount_is_parked(&self, mount_id: &str) -> DeviceResult<bool> {
        #[cfg(windows)]
        {
            if mount_id.starts_with("ascom:") {
                let prog_id = mount_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();
                
                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let mount = AscomMount::new(&prog_id)?;
                        Ok(mount.at_park()?)
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }
        
        if mount_id.starts_with("alpaca:") {
            let id_str = mount_id.strip_prefix("alpaca:").unwrap_or("");
            let parts: Vec<&str> = id_str.split(':').collect();

            if parts.len() >= 5 {
                let protocol = parts[0];
                let host_part = parts[1].trim_start_matches("//");
                let port = parts[2];
                let device_num: u32 = parts[4].parse().unwrap_or(0);

                let base_url = format!("{}://{}:{}", protocol, host_part, port);
                let mount = AlpacaTelescope::from_server(&base_url, device_num);
                mount.connect().await?;

                // Use guard to ensure disconnect on any error
                let result = with_alpaca_connection(&mount, "mount_is_parked", async {
                    let parked = mount.at_park().await.map_err(|e| e.to_string())?;
                    Ok::<bool, String>(parked)
                }).await;

                return result;
            }
        }

        Err(format!("Mount {} not found or unsupported", mount_id))
    }

    async fn mount_can_flip(&self, mount_id: &str) -> DeviceResult<bool> {
        #[cfg(windows)]
        {
            if mount_id.starts_with("ascom:") {
                let prog_id = mount_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();

                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let mount = AscomMount::new(&prog_id)?;
                        // Check if mount supports slewing which indicates GEM capability
                        Ok(mount.can_slew()?)
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }

        if mount_id.starts_with("alpaca:") {
            let id_str = mount_id.strip_prefix("alpaca:").unwrap_or("");
            let parts: Vec<&str> = id_str.split(':').collect();

            if parts.len() >= 5 {
                let protocol = parts[0];
                let host_part = parts[1].trim_start_matches("//");
                let port = parts[2];
                let device_num: u32 = parts[4].parse().unwrap_or(0);

                let base_url = format!("{}://{}:{}", protocol, host_part, port);
                let mount = AlpacaTelescope::from_server(&base_url, device_num);
                mount.connect().await?;

                // Use guard to ensure disconnect on any error
                let result = with_alpaca_connection(&mount, "mount_can_flip", async {
                    let can_flip = mount.can_slew().await.unwrap_or(true);
                    Ok::<bool, String>(can_flip)
                }).await;

                return result;
            }
        }

        Ok(false)
    }

    async fn mount_side_of_pier(&self, mount_id: &str) -> DeviceResult<nightshade_sequencer::meridian::PierSide> {
        #[cfg(windows)]
        {
            if mount_id.starts_with("ascom:") {
                let prog_id = mount_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();

                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let mount = AscomMount::new(&prog_id)?;
                        let side = mount.side_of_pier()?;
                        // ASCOM PierSide: 0=East, 1=West, -1=Unknown
                        Ok(match side {
                            0 => nightshade_sequencer::meridian::PierSide::East,
                            1 => nightshade_sequencer::meridian::PierSide::West,
                            _ => nightshade_sequencer::meridian::PierSide::Unknown,
                        })
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }

        if mount_id.starts_with("alpaca:") {
            let id_str = mount_id.strip_prefix("alpaca:").unwrap_or("");
            let parts: Vec<&str> = id_str.split(':').collect();

            if parts.len() >= 5 {
                let protocol = parts[0];
                let host_part = parts[1].trim_start_matches("//");
                let port = parts[2];
                let device_num: u32 = parts[4].parse().unwrap_or(0);

                let base_url = format!("{}://{}:{}", protocol, host_part, port);
                let mount = AlpacaTelescope::from_server(&base_url, device_num);
                mount.connect().await?;

                // Use guard to ensure disconnect on any error
                let result = with_alpaca_connection(&mount, "mount_side_of_pier", async {
                    let side = mount.side_of_pier().await.unwrap_or(nightshade_alpaca::PierSide::Unknown);
                    Ok::<nightshade_sequencer::meridian::PierSide, String>(match side {
                        nightshade_alpaca::PierSide::East => nightshade_sequencer::meridian::PierSide::East,
                        nightshade_alpaca::PierSide::West => nightshade_sequencer::meridian::PierSide::West,
                        nightshade_alpaca::PierSide::Unknown => nightshade_sequencer::meridian::PierSide::Unknown,
                    })
                }).await;

                return result;
            }
        }

        Ok(nightshade_sequencer::meridian::PierSide::Unknown)
    }

    async fn mount_is_tracking(&self, mount_id: &str) -> DeviceResult<bool> {
        #[cfg(windows)]
        {
            if mount_id.starts_with("ascom:") {
                let prog_id = mount_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();

                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let mount = AscomMount::new(&prog_id)?;
                        Ok(mount.tracking()?)
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }

        if mount_id.starts_with("alpaca:") {
            let id_str = mount_id.strip_prefix("alpaca:").unwrap_or("");
            let parts: Vec<&str> = id_str.split(':').collect();

            if parts.len() >= 5 {
                let protocol = parts[0];
                let host_part = parts[1].trim_start_matches("//");
                let port = parts[2];
                let device_num: u32 = parts[4].parse().unwrap_or(0);

                let base_url = format!("{}://{}:{}", protocol, host_part, port);
                let mount = AlpacaTelescope::from_server(&base_url, device_num);
                mount.connect().await?;

                // Use guard to ensure disconnect on any error
                let result = with_alpaca_connection(&mount, "mount_is_tracking", async {
                    let tracking = mount.tracking().await.map_err(|e| e.to_string())?;
                    Ok::<bool, String>(tracking)
                }).await;

                return result;
            }
        }

        Err(format!("Mount {} not found or unsupported", mount_id))
    }

    async fn mount_set_tracking(&self, mount_id: &str, enabled: bool) -> DeviceResult<()> {
        tracing::info!("Setting tracking {} for mount {}", if enabled { "on" } else { "off" }, mount_id);

        #[cfg(windows)]
        {
            if mount_id.starts_with("ascom:") {
                let prog_id = mount_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();

                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let mut mount = AscomMount::new(&prog_id)?;
                        mount.set_tracking(enabled)?;
                        Ok(())
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }

        if mount_id.starts_with("alpaca:") {
            let id_str = mount_id.strip_prefix("alpaca:").unwrap_or("");
            let parts: Vec<&str> = id_str.split(':').collect();

            if parts.len() >= 5 {
                let protocol = parts[0];
                let host_part = parts[1].trim_start_matches("//");
                let port = parts[2];
                let device_num: u32 = parts[4].parse().unwrap_or(0);

                let base_url = format!("{}://{}:{}", protocol, host_part, port);
                let mount = AlpacaTelescope::from_server(&base_url, device_num);
                mount.connect().await?;

                // Use guard to ensure disconnect on any error
                let result = with_alpaca_connection(&mount, "mount_set_tracking", async {
                    mount.set_tracking(enabled).await.map_err(|e| e.to_string())?;
                    Ok::<(), String>(())
                }).await;

                return result;
            }
        }

        Err(format!("Mount {} not found or unsupported", mount_id))
    }

    async fn mount_abort_slew(&self, mount_id: &str) -> DeviceResult<()> {
        tracing::info!("Aborting slew for mount {}", mount_id);

        #[cfg(windows)]
        {
            if mount_id.starts_with("ascom:") {
                let prog_id = mount_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();

                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let mut mount = AscomMount::new(&prog_id)?;
                        mount.abort_slew()?;
                        Ok(())
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }

        if mount_id.starts_with("alpaca:") {
            let id_str = mount_id.strip_prefix("alpaca:").unwrap_or("");
            let parts: Vec<&str> = id_str.split(':').collect();

            if parts.len() >= 5 {
                let protocol = parts[0];
                let host_part = parts[1].trim_start_matches("//");
                let port = parts[2];
                let device_num: u32 = parts[4].parse().unwrap_or(0);

                let base_url = format!("{}://{}:{}", protocol, host_part, port);
                let mount = AlpacaTelescope::from_server(&base_url, device_num);
                mount.connect().await?;

                // Use guard to ensure disconnect on any error
                let result = with_alpaca_connection(&mount, "mount_abort_slew", async {
                    mount.abort_slew().await.map_err(|e| e.to_string())?;
                    Ok::<(), String>(())
                }).await;

                return result;
            }
        }

        Err(format!("Mount {} not found or unsupported", mount_id))
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
    ) -> DeviceResult<SeqImageData> {
        tracing::info!(
            duration_secs, camera_id, gain, offset, bin_x, bin_y
        );
        
        // 1. Get current filter name if a filter wheel is connected
        let filter_name = if let Some(fw_id) = self.get_device_id(crate::device::DeviceType::FilterWheel) {
            match self.filterwheel_get_position(&fw_id).await {
                Ok(pos) => {
                    match self.filterwheel_get_names(&fw_id).await {
                        Ok(names) => {
                            if pos >= 0 && (pos as usize) < names.len() {
                                Some(names[pos as usize].clone())
                            } else {
                                None
                            }
                        },
                        Err(_) => None,
                    }
                },
                Err(_) => None,
            }
        } else {
            None
        };
        
        #[cfg(windows)]
        {
            if camera_id.starts_with("ascom:") {
                let prog_id = camera_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();
                
                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let mut camera = AscomCamera::new(&prog_id)?;
                        camera.connect()?;
                        
                        //Set parameters
                        if let Some(g) = gain {
                            if let Err(e) = camera.set_gain(g) {
                                tracing::warn!("Failed to set gain: {}", e);
                            }
                        }
                        if let Some(o) = offset {
                            if let Err(e) = camera.set_offset(o) {
                                tracing::warn!("Failed to set offset: {}", e);
                            }
                        }
                        if let Err(e) = camera.set_bin_x(bin_x) {
                            tracing::warn!("Failed to set bin_x: {}", e);
                        }
                        if let Err(e) = camera.set_bin_y(bin_y) {
                            tracing::warn!("Failed to set bin_y: {}", e);
                        }
                        
                        // Start exposure
                        camera.start_exposure(duration_secs, true)?;  // true = light frame
                        
                        // Wait for completion
                        while !camera.image_ready()? {
                            std::thread::sleep(std::time::Duration::from_millis(100));
                        }
                        
                        // Get actual image array from ASCOM with dimensions
                        let (image_data_i32, array_width, array_height) = camera.image_array()?;
                        
                        // Get reported dimensions
                        let report_width = camera.camera_x_size()? as u32;
                        let report_height = camera.camera_y_size()? as u32;
                        
                        tracing::info!("ASCOM ImageArray: {}x{} (Reported: {}x{})", 
                            array_width, array_height, report_width, report_height);
                            
                        // Use array dimensions to avoid stride issues
                        let width = array_width as u32;
                        let height = array_height as u32;
                        
                        // Convert i32 to u16 (ASCOM uses i32 but most cameras are 16-bit)
                        // Clamp values to 16-bit range
                        if image_data_i32.len() != (width * height) as usize {
                            tracing::warn!("Image array length mismatch! Expected {}*{}={}, got {}", 
                                width, height, width*height, image_data_i32.len());
                        }

                        let data: Vec<u16> = image_data_i32.iter()
                            .map(|&pixel| pixel.max(0).min(65535) as u16)
                            .collect();
                        
                        // Get actual gain if not explicitly set
                        let actual_gain = gain.or(camera.gain().ok());
                        
                        // Detect color sensor and Bayer pattern
                        let sensor_type = camera.sensor_type().ok();
                        let bayer_offset = if sensor_type == Some(1) || sensor_type == Some(2) { // 1=Color, 2=RGGB
                            let offset_x = camera.bayer_offset_x().ok();
                            let offset_y = camera.bayer_offset_y().ok();
                            match (offset_x, offset_y) {
                                (Some(x), Some(y)) => Some((x, y)),
                                _ => Some((0, 0)),  // Default to RGGB if color but no offset info
                            }
                        } else {
                            None
                        };

                        let sensor_type_str = match sensor_type {
                            Some(0) => Some("Monochrome".to_string()),
                            Some(_) => Some("Color".to_string()),
                            None => None,
                        };

                        Ok(SeqImageData {
                            width,
                            height,
                            data,
                            bits_per_pixel: 16,
                            exposure_secs: duration_secs,
                            gain: actual_gain,
                            offset: offset,
                            temperature: camera.ccd_temperature().ok(),
                            filter: filter_name,
                            timestamp: chrono::Utc::now().timestamp(),
                            sensor_type: sensor_type_str,
                            bayer_offset,
                        })
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }
        
        if camera_id.starts_with("indi:") {
            if let Some(client) = self.device_manager.get_indi_client(camera_id).await {
                // Parse device name from ID
                let parts: Vec<&str> = camera_id.split(':').collect();
                let device_name = parts[3..].join(":");
                
                let camera = IndiCamera::new(client, &device_name);
                
                // Set parameters
                if let Some(g) = gain {
                    camera.set_gain(g).await.ok();
                }
                if let Some(o) = offset {
                    camera.set_offset(o).await.ok();
                }
                camera.set_binning(bin_x, bin_y).await.ok();
                
                // Capture image
                let data = camera.capture_image(duration_secs).await
                    .map_err(|e| format!("INDI capture failed: {}", e))?;
                
                // Detect image format from magic bytes
                let format = nightshade_imaging::ImageFormat::from_magic_bytes(&data);
                tracing::info!("INDI BLOB format detected: {:?}", format);

                // Process based on detected format
                let (image, is_color) = match format {
                    Some(nightshade_imaging::ImageFormat::Fits) | None => {
                        // FITS format (most astronomy cameras) or unknown - try FITS first
                        let (img, header) = nightshade_imaging::read_fits_from_bytes(&data)
                            .map_err(|e| format!("Failed to parse INDI FITS: {}", e))?;

                        // Extract bayer pattern from FITS header
                        let bayer = header.keywords.get("BAYERPAT")
                            .and_then(|v| v.as_string().map(|_s| true));
                        let color = bayer.unwrap_or(false);
                        (img, color)
                    },
                    Some(fmt) if fmt.is_raw() => {
                        // DSLR RAW format (Canon CR2/CR3, Nikon NEF, Sony ARW, Fuji RAF, etc.)
                        tracing::info!("Processing DSLR RAW image ({:?}) via LibRaw", fmt);

                        let ext = nightshade_imaging::raw_format_extension(&data)
                            .unwrap_or("raw");

                        let (img, raw_meta) = nightshade_imaging::read_raw_from_bytes(&data, ext, None)
                            .map_err(|e| format!("Failed to process DSLR RAW: {}", e))?;

                        tracing::info!("RAW processed: {} {} ({}x{}), X-Trans: {}",
                            raw_meta.camera_make, raw_meta.camera_model,
                            img.width, img.height, raw_meta.is_xtrans);

                        // RAW files are always color after debayering
                        (img, true)
                    },
                    Some(nightshade_imaging::ImageFormat::Jpeg) => {
                        // JPEG preview (some cameras send JPEG for quick preview)
                        tracing::info!("Processing JPEG preview image");
                        let img = image::load_from_memory(&data)
                            .map_err(|e| format!("Failed to decode JPEG: {}", e))?;

                        let rgba = img.to_rgba16();
                        let (w, h) = (rgba.width(), rgba.height());
                        let raw_data = rgba.into_raw();

                        // Convert RGBA16 to u8 bytes for ImageData
                        let bytes: Vec<u8> = raw_data.iter()
                            .flat_map(|&v| v.to_le_bytes())
                            .collect();

                        let image_data = nightshade_imaging::ImageData {
                            width: w,
                            height: h,
                            channels: 4,
                            pixel_type: nightshade_imaging::PixelType::U16,
                            data: bytes,
                        };
                        (image_data, true)
                    },
                    Some(other) => {
                        return Err(format!("Unsupported image format from INDI: {:?}", other));
                    }
                };

                // Convert to u16 for sequencer
                let u16_data = image.as_u16()
                    .ok_or_else(|| "Image is not 16-bit".to_string())?;

                return Ok(SeqImageData {
                    width: image.width,
                    height: image.height,
                    data: u16_data,
                    bits_per_pixel: 16,
                    exposure_secs: duration_secs,
                    gain,
                    offset,
                    temperature: camera.get_temperature().await.ok(),
                    filter: filter_name,
                    timestamp: chrono::Utc::now().timestamp(),
                    sensor_type: if is_color { Some("Color".to_string()) } else { Some("Mono".to_string()) },
                    bayer_offset: None, // RAW files are already debayered
                });
            }
        }

        // Alpaca camera support (cross-platform)
        if camera_id.starts_with("alpaca:") {
            // Parse alpaca:http://192.168.1.100:11111:camera:0
            let id_str = camera_id.strip_prefix("alpaca:").unwrap_or("");
            let parts: Vec<&str> = id_str.split(':').collect();
            
            if parts.len() >= 5 {
                let protocol = parts[0]; // "http" or "https"
                let host_part = parts[1].trim_start_matches("//");
                let port = parts[2];
                let _device_type = parts[3];
                let device_num: u32 = parts[4].parse().unwrap_or(0);
                
                let base_url = format!("{}://{}:{}", protocol, host_part, port);
                
                tracing::info!("Connecting to Alpaca camera at {}, device {}", base_url, device_num);
                
                let camera = AlpacaCamera::from_server(&base_url, device_num);
                
                // Connect to camera
                camera.connect().await?;
                
                // Set parameters
                if let Some(g) = gain {
                    // Check if camera supports gain
                    if camera.gain_max().await.unwrap_or(0) > 0 {
                        camera.set_gain(g).await?;
                    }
                }
                if let Some(o) = offset {
                    if camera.offset_max().await.unwrap_or(0) > 0 {
                        camera.set_offset(o).await?;
                    }
                }
                camera.set_bin_x(bin_x).await?;
                camera.set_bin_y(bin_y).await?;
                
                // Start exposure
                camera.start_exposure(duration_secs, true).await?; // true = light frame
                
                // Poll for completion
                while !camera.image_ready().await? {
                    tokio::time::sleep(std::time::Duration::from_millis(100)).await;
                }
                
                // Get image dimensions
                let _width = camera.camera_x_size().await? as u32 / bin_x as u32;
                let _height = camera.camera_y_size().await? as u32 / bin_y as u32;
                
                // Get image array (returns JSON string with base64 data)
                let image_json = camera.image_array().await?;
                
                // Parse Alpaca ImageArray JSON response
                // Format: {"Type": 2, "Rank": 2, "Value": "base64data..."}
                let parsed: serde_json::Value = serde_json::from_str(&image_json)
                    .map_err(|e| format!("Failed to parse Alpaca ImageArray JSON: {}", e))?;
                
                let base64_data = parsed.get("Value")
                    .and_then(|v| v.as_str())
                    .ok_or("No Value field in ImageArray response")?;
                
                // Decode base64 to bytes
                let decoded_bytes = decode_base64(base64_data)?;
                
                // Get dimensions
                let width = camera.camera_x_size().await? as u32 / bin_x as u32;
                let height = camera.camera_y_size().await? as u32 / bin_y as u32;
                
                // Alpaca ImageArray Type: 0=Int16Array, 1=Int32Array, 2=DoubleArray
                let array_type = parsed.get("Type")
                    .and_then(|v| v.as_i64())
                    .unwrap_or(1); // Default to Int32
                
                // Convert bytes to u16 based on array type
                let data: Vec<u16> = match array_type {
                    0 => {
                        // Int16Array - convert i16 to u16
                        decoded_bytes.chunks_exact(2)
                            .map(|chunk| {
                                let val = i16::from_le_bytes([chunk[0], chunk[1]]);
                                val.max(0) as u16
                            })
                            .collect()
                    },
                    1 => {
                        // Int32Array - convert i32 to u16
                        decoded_bytes.chunks_exact(4)
                            .map(|chunk| {
                                let val = i32::from_le_bytes([chunk[0], chunk[1], chunk[2], chunk[3]]);
                                val.max(0).min(65535) as u16
                            })
                            .collect()
                    },
                    _ => {
                        return Err(format!("Unsupported Alpaca array type: {}", array_type));
                    }
                };
                
                // Query color metadata
                let sensor_type_code = camera.sensor_type().await.ok();
                let sensor_type = match sensor_type_code {
                    Some(0) => Some("Monochrome".to_string()),
                    Some(1) => Some("Color".to_string()),
                    Some(2) => Some("RGGB".to_string()),
                    Some(3) => Some("CMYG".to_string()),
                    _ => None,
                };
                
                let is_color = sensor_type.as_deref() == Some("Color") 
                    || sensor_type.as_deref() == Some("RGGB");
                
                let bayer_offset = if is_color {
                    let offset_x = camera.bayer_offset_x().await.ok();
                    let offset_y = camera.bayer_offset_y().await.ok();
                    match (offset_x, offset_y) {
                        (Some(x), Some(y)) => Some((x, y)),
                        _ => Some((0, 0)), // Default to RGGB
                    }
                } else {
                    None
                };
                
                camera.disconnect().await.ok(); // Clean disconnect
                
                return Ok(SeqImageData {
                    width,
                    height,
                    data,
                    bits_per_pixel: 16,
                    exposure_secs: duration_secs,
                    gain: gain,
                    offset,
                    temperature: camera.ccd_temperature().await.ok(),
                    filter: filter_name,
                    timestamp: chrono::Utc::now().timestamp(),
                    sensor_type,
                    bayer_offset,
                });
            }
        }

        // Native camera support
        if camera_id.starts_with("native:") {
            let mut native_cameras = self.device_manager.native_cameras.write().await;
            if let Some(camera) = native_cameras.get_mut(camera_id) {
                // Set gain if provided
                if let Some(g) = gain {
                    camera.set_gain(g).await.map_err(|e| format!("Failed to set gain: {}", e))?;
                }
                // Set offset if provided
                if let Some(o) = offset {
                    camera.set_offset(o).await.map_err(|e| format!("Failed to set offset: {}", e))?;
                }
                // Set binning
                camera.set_binning(bin_x, bin_y).await.map_err(|e| format!("Failed to set binning: {}", e))?;

                // Create exposure params
                let params = ExposureParams {
                    duration_secs,
                    gain,
                    offset,
                    bin_x,
                    bin_y,
                    subframe: None,
                    readout_mode: None,
                };

                // Start exposure
                camera.start_exposure(params).await.map_err(|e| format!("Failed to start exposure: {}", e))?;

                // Wait for exposure to complete
                while !camera.is_exposure_complete().await.map_err(|e| format!("Error checking exposure: {}", e))? {
                    tokio::time::sleep(std::time::Duration::from_millis(100)).await;
                }

                // Download image
                let image = camera.download_image().await.map_err(|e| format!("Failed to download image: {}", e))?;

                // Get sensor info for color detection
                let sensor_info = camera.get_sensor_info();
                let sensor_type = if sensor_info.color {
                    Some("Color".to_string())
                } else {
                    Some("Monochrome".to_string())
                };

                // Convert bayer pattern to offset
                let bayer_offset = image.bayer_pattern.map(|pattern| {
                    match pattern {
                        nightshade_native::camera::BayerPattern::Rggb => (0, 0),
                        nightshade_native::camera::BayerPattern::Grbg => (1, 0),
                        nightshade_native::camera::BayerPattern::Gbrg => (0, 1),
                        nightshade_native::camera::BayerPattern::Bggr => (1, 1),
                    }
                });

                return Ok(SeqImageData {
                    width: image.width,
                    height: image.height,
                    data: image.data,
                    bits_per_pixel: image.bits_per_pixel,
                    exposure_secs: image.metadata.exposure_time,
                    gain: Some(image.metadata.gain),
                    offset: Some(image.metadata.offset),
                    temperature: image.metadata.temperature,
                    filter: filter_name,
                    timestamp: chrono::Utc::now().timestamp(),
                    sensor_type,
                    bayer_offset,
                });
            }
        }

        Err(format!("Camera {} not found or unsupported", camera_id))
    }

    async fn camera_abort_exposure(&self, camera_id: &str) -> DeviceResult<()> {
        #[cfg(windows)]
        {
            if camera_id.starts_with("ascom:") {
                let prog_id = camera_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();
                
                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let mut camera = AscomCamera::new(&prog_id)?;
                        camera.abort_exposure()?;
                        Ok(())
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }
        
        if camera_id.starts_with("indi:") {
            if let Some(client) = self.device_manager.get_indi_client(camera_id).await {
                let parts: Vec<&str> = camera_id.split(':').collect();
                let device_name = parts[3..].join(":");
                let camera = IndiCamera::new(client, &device_name);
                camera.abort_exposure().await?;
                return Ok(());
            }
        }
        
        if camera_id.starts_with("alpaca:") {
            let parts: Vec<&str> = camera_id.split(':').collect();
            if parts.len() >= 3 {
                let base_url = parts[1];
                let device_number: u32 = parts[2].parse().map_err(|e| format!("Invalid device number: {}", e))?;

                let camera = AlpacaCamera::from_server(base_url, device_number);
                camera.abort_exposure().await?;
                return Ok(());
            }
        }

        // Native camera support
        if camera_id.starts_with("native:") {
            let mut native_cameras = self.device_manager.native_cameras.write().await;
            if let Some(camera) = native_cameras.get_mut(camera_id) {
                camera.abort_exposure().await.map_err(|e| format!("Failed to abort exposure: {}", e))?;
                return Ok(());
            }
        }

        Err(format!("Camera {} not found or unsupported", camera_id))
    }

    async fn camera_set_cooler(&self, camera_id: &str, enabled: bool, target_temp: f64) -> DeviceResult<()> {
        tracing::info!("Setting cooler on {} to {} (target: {}°C)", camera_id, enabled, target_temp);
        
        #[cfg(windows)]
        {
            if camera_id.starts_with("ascom:") {
                let prog_id = camera_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();
                
                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let mut camera = AscomCamera::new(&prog_id)?;
                        camera.set_ccd_temperature(target_temp)?;
                        camera.set_cooler_on(enabled)?;
                        Ok(())
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }
        
        if camera_id.starts_with("indi:") {
            if let Some(client) = self.device_manager.get_indi_client(camera_id).await {
                let parts: Vec<&str> = camera_id.split(':').collect();
                let device_name = parts[3..].join(":");
                let camera = IndiCamera::new(client, &device_name);
                camera.set_cooler(enabled).await?;
                camera.set_temperature(target_temp).await?;
                return Ok(());
            }
        }
        
        if camera_id.starts_with("alpaca:") {
            let parts: Vec<&str> = camera_id.split(':').collect();
            if parts.len() >= 3 {
                let base_url = parts[1];
                let device_number: u32 = parts[2].parse().map_err(|e| format!("Invalid device number: {}", e))?;

                let camera = AlpacaCamera::from_server(base_url, device_number);
                camera.set_ccd_temperature(target_temp).await?;
                camera.set_cooler_on(enabled).await?;
                return Ok(());
            }
        }

        // Native camera support
        if camera_id.starts_with("native:") {
            let mut native_cameras = self.device_manager.native_cameras.write().await;
            if let Some(camera) = native_cameras.get_mut(camera_id) {
                camera.set_cooler(enabled, target_temp).await.map_err(|e| format!("Failed to set cooler: {}", e))?;
                return Ok(());
            }
        }

        Err(format!("Camera {} not found or unsupported", camera_id))
    }

    async fn camera_get_temperature(&self, camera_id: &str) -> DeviceResult<f64> {
        #[cfg(windows)]
        {
            if camera_id.starts_with("ascom:") {
                let prog_id = camera_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();
                
                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let camera = AscomCamera::new(&prog_id)?;
                        Ok(camera.ccd_temperature()?)
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }
        
        if camera_id.starts_with("indi:") {
            if let Some(client) = self.device_manager.get_indi_client(camera_id).await {
                let parts: Vec<&str> = camera_id.split(':').collect();
                let device_name = parts[3..].join(":");
                let camera = IndiCamera::new(client, &device_name);
                return camera.get_temperature().await.map_err(|e| e);
            }
        }
        
        if camera_id.starts_with("alpaca:") {
            let parts: Vec<&str> = camera_id.split(':').collect();
            if parts.len() >= 3 {
                let base_url = parts[1];
                let device_number: u32 = parts[2].parse().map_err(|e| format!("Invalid device number: {}", e))?;

                let camera = AlpacaCamera::from_server(base_url, device_number);
                return Ok(camera.ccd_temperature().await?);
            }
        }

        // Native camera support
        if camera_id.starts_with("native:") {
            let native_cameras = self.device_manager.native_cameras.read().await;
            if let Some(camera) = native_cameras.get(camera_id) {
                return camera.get_temperature().await.map_err(|e| format!("Failed to get temperature: {}", e));
            }
        }

        Err(format!("Camera {} not found or unsupported", camera_id))
    }

    async fn camera_get_cooler_power(&self, camera_id: &str) -> DeviceResult<f64> {
        #[cfg(windows)]
        {
            if camera_id.starts_with("ascom:") {
                let prog_id = camera_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();
                
                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let camera = AscomCamera::new(&prog_id)?;
                        Ok(camera.cooler_power()?)
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }
        
        if camera_id.starts_with("indi:") {
            // INDI doesn't always expose cooler power in a standard way
            // For now return 0.0
            return Ok(0.0);
        }
        
        if camera_id.starts_with("alpaca:") {
            let parts: Vec<&str> = camera_id.split(':').collect();
            if parts.len() >= 3 {
                let base_url = parts[1];
                let device_number: u32 = parts[2].parse().map_err(|e| format!("Invalid device number: {}", e))?;

                let camera = AlpacaCamera::from_server(base_url, device_number);
                return Ok(camera.cooler_power().await?);
            }
        }

        // Native camera support
        if camera_id.starts_with("native:") {
            let native_cameras = self.device_manager.native_cameras.read().await;
            if let Some(camera) = native_cameras.get(camera_id) {
                return camera.get_cooler_power().await.map_err(|e| format!("Failed to get cooler power: {}", e));
            }
        }

        Err(format!("Camera {} not found or unsupported", camera_id))
    }

    // =========================================================================
    // FOCUSER OPERATIONS (Placeholder - to be implemented)
    // =========================================================================
    
    async fn focuser_move_to(&self, focuser_id: &str, position: i32) -> DeviceResult<()> {
        tracing::info!("Moving focuser {} to position {}", focuser_id, position);
        
        #[cfg(windows)]
        {
            if focuser_id.starts_with("ascom:") {
                let prog_id = focuser_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();
                
                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let mut focuser = AscomFocuser::new(&prog_id)?;
                        focuser.move_to(position)?;
                        
                        // Wait for move to complete
                        while focuser.is_moving()? {
                            std::thread::sleep(std::time::Duration::from_millis(100));
                        }
                        Ok(())
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }
        
        // Alpaca focuser
        if focuser_id.starts_with("alpaca:") {
            let id_str = focuser_id.strip_prefix("alpaca:").unwrap_or("");
            let parts: Vec<&str> = id_str.split(':').collect();
            
            if parts.len() >= 5 {
                let protocol = parts[0];
                let host_part = parts[1].trim_start_matches("//");
                let port = parts[2];
                let device_num: u32 = parts[4].parse().unwrap_or(0);
                
                let base_url = format!("{}://{}:{}", protocol, host_part, port);
                let focuser = AlpacaFocuser::from_server(&base_url, device_num);
                focuser.connect().await?;
                focuser.move_to(position).await?;
                
                // Wait for move to complete
                while focuser.is_moving().await? {
                    tokio::time::sleep(std::time::Duration::from_millis(100)).await;
                }
                focuser.disconnect().await.ok();
                
                return Ok(());
            }
        }

        // Native focuser
        if focuser_id.starts_with("native:") {
            let native_focusers = self.device_manager.native_focusers.read().await;
            if let Some(focuser) = native_focusers.get(focuser_id) {
                // We need mutable access, so drop the read lock and get write lock
                drop(native_focusers);
                let mut native_focusers = self.device_manager.native_focusers.write().await;
                if let Some(focuser) = native_focusers.get_mut(focuser_id) {
                    focuser.move_to(position).await.map_err(|e| e.to_string())?;

                    // Wait for move to complete
                    while focuser.is_moving().await.unwrap_or(false) {
                        tokio::time::sleep(std::time::Duration::from_millis(100)).await;
                    }
                    return Ok(());
                }
            }
        }

        Err(format!("Focuser {} not found or unsupported", focuser_id))
    }

    async fn focuser_get_position(&self, focuser_id: &str) -> DeviceResult<i32> {
        #[cfg(windows)]
        {
            if focuser_id.starts_with("ascom:") {
                let prog_id = focuser_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();
                
                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let focuser = AscomFocuser::new(&prog_id)?;
                        Ok(focuser.position()?)
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }
        
        if focuser_id.starts_with("alpaca:") {
            let id_str = focuser_id.strip_prefix("alpaca:").unwrap_or("");
            let parts: Vec<&str> = id_str.split(':').collect();
            
            if parts.len() >= 5 {
                let protocol = parts[0];
                let host_part = parts[1].trim_start_matches("//");
                let port = parts[2];
                let device_num: u32 = parts[4].parse().unwrap_or(0);
                
                let base_url = format!("{}://{}:{}", protocol, host_part, port);
                let focuser = AlpacaFocuser::from_server(&base_url, device_num);
                focuser.connect().await?;
                let pos = focuser.position().await?;
                focuser.disconnect().await.ok();
                return Ok(pos);
            }
        }

        // Native focuser
        if focuser_id.starts_with("native:") {
            let native_focusers = self.device_manager.native_focusers.read().await;
            if let Some(focuser) = native_focusers.get(focuser_id) {
                return focuser.get_position().await.map_err(|e| e.to_string());
            }
        }

        Err(format!("Focuser {} not found or unsupported", focuser_id))
    }

    async fn focuser_is_moving(&self, focuser_id: &str) -> DeviceResult<bool> {
        #[cfg(windows)]
        {
            if focuser_id.starts_with("ascom:") {
                let prog_id = focuser_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();
                
                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let focuser = AscomFocuser::new(&prog_id)?;
                        Ok(focuser.is_moving()?)
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }
        
        if focuser_id.starts_with("alpaca:") {
            let id_str = focuser_id.strip_prefix("alpaca:").unwrap_or("");
            let parts: Vec<&str> = id_str.split(':').collect();
            
            if parts.len() >= 5 {
                let protocol = parts[0];
                let host_part = parts[1].trim_start_matches("//");
                let port = parts[2];
                let device_num: u32 = parts[4].parse().unwrap_or(0);
                
                let base_url = format!("{}://{}:{}", protocol, host_part, port);
                let focuser = AlpacaFocuser::from_server(&base_url, device_num);
                focuser.connect().await?;
                let moving = focuser.is_moving().await?;
                focuser.disconnect().await.ok();
                return Ok(moving);
            }
        }

        // Native focuser
        if focuser_id.starts_with("native:") {
            let native_focusers = self.device_manager.native_focusers.read().await;
            if let Some(focuser) = native_focusers.get(focuser_id) {
                return focuser.is_moving().await.map_err(|e| e.to_string());
            }
        }

        Err(format!("Focuser {} not found or unsupported", focuser_id))
    }

    async fn focuser_get_temperature(&self, focuser_id: &str) -> DeviceResult<Option<f64>> {
        #[cfg(windows)]
        {
            if focuser_id.starts_with("ascom:") {
                let prog_id = focuser_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();
                
                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let focuser = AscomFocuser::new(&prog_id)?;
                        Ok(focuser.temperature().ok())
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }
        
        if focuser_id.starts_with("alpaca:") {
            let id_str = focuser_id.strip_prefix("alpaca:").unwrap_or("");
            let parts: Vec<&str> = id_str.split(':').collect();
            
            if parts.len() >= 5 {
                let protocol = parts[0];
                let host_part = parts[1].trim_start_matches("//");
                let port = parts[2];
                let device_num: u32 = parts[4].parse().unwrap_or(0);
                
                let base_url = format!("{}://{}:{}", protocol, host_part, port);
                let focuser = AlpacaFocuser::from_server(&base_url, device_num);
                focuser.connect().await?;
                let temp = focuser.temperature().await.ok();
                focuser.disconnect().await.ok();
                return Ok(temp);
            }
        }

        // Native focuser
        if focuser_id.starts_with("native:") {
            let native_focusers = self.device_manager.native_focusers.read().await;
            if let Some(focuser) = native_focusers.get(focuser_id) {
                return focuser.get_temperature().await.map_err(|e| e.to_string());
            }
        }

        Err(format!("Focuser {} not found or unsupported", focuser_id))
    }

    async fn focuser_halt(&self, focuser_id: &str) -> DeviceResult<()> {
        tracing::info!("Halting focuser {}", focuser_id);
        
        #[cfg(windows)]
        {
            if focuser_id.starts_with("ascom:") {
                let prog_id = focuser_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();
                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let mut focuser = AscomFocuser::new(&prog_id)?;
                        focuser.halt()?;
                        Ok(())
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }
        
        if focuser_id.starts_with("alpaca:") {
            let id_str = focuser_id.strip_prefix("alpaca:").unwrap_or("");
            let parts: Vec<&str> = id_str.split(':').collect();
            
            if parts.len() >= 5 {
                let protocol = parts[0];
                let host_part = parts[1].trim_start_matches("//");
                let port = parts[2];
                let device_num: u32 = parts[4].parse().unwrap_or(0);
                
                let base_url = format!("{}://{}:{}", protocol, host_part, port);
                let focuser = AlpacaFocuser::from_server(&base_url, device_num);
                focuser.connect().await?;
                focuser.halt().await?;
                focuser.disconnect().await.ok();
                return Ok(());
            }
        }
        
        if focuser_id.starts_with("sim_") {
            let mut focuser = get_sim_focuser().write().await;
            focuser.status.moving = false;
            return Ok(());
        }

        // Native focuser
        if focuser_id.starts_with("native:") {
            let native_focusers = self.device_manager.native_focusers.read().await;
            if let Some(_focuser) = native_focusers.get(focuser_id) {
                drop(native_focusers);
                let mut native_focusers = self.device_manager.native_focusers.write().await;
                if let Some(focuser) = native_focusers.get_mut(focuser_id) {
                    return focuser.halt().await.map_err(|e| e.to_string());
                }
            }
        }

        Err(format!("Focuser {} not found or unsupported", focuser_id))
    }

    // =========================================================================
    // FILTER WHEEL OPERATIONS (Placeholder - to be implemented)
    // =========================================================================
    
    async fn filterwheel_set_position(&self, fw_id: &str, position: i32) -> DeviceResult<()> {
        tracing::info!("Setting filter wheel {} to position {}", fw_id, position);

        // Start tracking this movement for is_moving() detection
        self.start_fw_movement(fw_id, position).await;

        #[cfg(windows)]
        {
            if fw_id.starts_with("ascom:") {
                let prog_id = fw_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();

                let result = tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let mut fw = AscomFilterWheel::new(&prog_id)?;
                        fw.set_position(position)?;
                        Ok::<(), String>(())
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;

                // Wait for the filter wheel to complete the move
                if result.is_ok() {
                    self.filterwheel_wait_for_move(fw_id).await?;
                }
                return result;
            }
        }

        if fw_id.starts_with("alpaca:") {
            let id_str = fw_id.strip_prefix("alpaca:").unwrap_or("");
            let parts: Vec<&str> = id_str.split(':').collect();

            if parts.len() >= 5 {
                let protocol = parts[0];
                let host_part = parts[1].trim_start_matches("//");
                let port = parts[2];
                let device_num: u32 = parts[4].parse().unwrap_or(0);

                let base_url = format!("{}://{}:{}", protocol, host_part, port);
                let fw = AlpacaFilterWheel::from_server(&base_url, device_num);
                fw.connect().await?;
                fw.set_position(position).await?;

                // Wait for the filter wheel to complete the move
                self.filterwheel_wait_for_move(fw_id).await?;

                fw.disconnect().await.ok();
                return Ok(());
            }
        }

        // Native filter wheel
        if fw_id.starts_with("native:") {
            let native_filter_wheels = self.device_manager.native_filter_wheels.read().await;
            if let Some(_fw) = native_filter_wheels.get(fw_id) {
                drop(native_filter_wheels);
                let mut native_filter_wheels = self.device_manager.native_filter_wheels.write().await;
                if let Some(fw) = native_filter_wheels.get_mut(fw_id) {
                    fw.move_to_position(position).await.map_err(|e| e.to_string())?;

                    // Wait for move to complete
                    while fw.is_moving().await.unwrap_or(false) {
                        tokio::time::sleep(std::time::Duration::from_millis(100)).await;
                    }

                    self.fw_movements.write().await.remove(fw_id);
                    return Ok(());
                }
            }
        }

        // Clear movement tracking if we couldn't find the device
        self.fw_movements.write().await.remove(fw_id);
        Err(format!("Filter wheel {} not found or unsupported", fw_id))
    }

    async fn filterwheel_get_position(&self, fw_id: &str) -> DeviceResult<i32> {
        #[cfg(windows)]
        {
            if fw_id.starts_with("ascom:") {
                let prog_id = fw_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();
                
                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let fw = AscomFilterWheel::new(&prog_id)?;
                        Ok(fw.position()?)
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }
        
        if fw_id.starts_with("alpaca:") {
            let id_str = fw_id.strip_prefix("alpaca:").unwrap_or("");
            let parts: Vec<&str> = id_str.split(':').collect();
            
            if parts.len() >= 5 {
                let protocol = parts[0];
                let host_part = parts[1].trim_start_matches("//");
                let port = parts[2];
                let device_num: u32 = parts[4].parse().unwrap_or(0);
                
                let base_url = format!("{}://{}:{}", protocol, host_part, port);
                let fw = AlpacaFilterWheel::from_server(&base_url, device_num);
                fw.connect().await?;
                let pos = fw.position().await?;
                fw.disconnect().await.ok();
                return Ok(pos);
            }
        }

        // Native filter wheel
        if fw_id.starts_with("native:") {
            let native_filter_wheels = self.device_manager.native_filter_wheels.read().await;
            if let Some(fw) = native_filter_wheels.get(fw_id) {
                return fw.get_position().await.map_err(|e| e.to_string());
            }
        }

        Err(format!("Filter wheel {} not found or unsupported", fw_id))
    }

    async fn filterwheel_get_names(&self, fw_id: &str) -> DeviceResult<Vec<String>> {
        #[cfg(windows)]
        {
            if fw_id.starts_with("ascom:") {
                let prog_id = fw_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();
                
                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let fw = AscomFilterWheel::new(&prog_id)?;
                        Ok(fw.names()?)
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }
        
        if fw_id.starts_with("alpaca:") {
            let id_str = fw_id.strip_prefix("alpaca:").unwrap_or("");
            let parts: Vec<&str> = id_str.split(':').collect();
            
            if parts.len() >= 5 {
                let protocol = parts[0];
                let host_part = parts[1].trim_start_matches("//");
                let port = parts[2];
                let device_num: u32 = parts[4].parse().unwrap_or(0);
                
                let base_url = format!("{}://{}:{}", protocol, host_part, port);
                let fw = AlpacaFilterWheel::from_server(&base_url, device_num);
                fw.connect().await?;
                let names = fw.names().await?;
                fw.disconnect().await.ok();
                return Ok(names);
            }
        }

        // Native filter wheel
        if fw_id.starts_with("native:") {
            let native_filter_wheels = self.device_manager.native_filter_wheels.read().await;
            let available_keys: Vec<_> = native_filter_wheels.keys().collect();
            tracing::debug!("filterwheel_get_names: Looking for '{}' in native filter wheels: {:?}", fw_id, available_keys);

            if let Some(fw) = native_filter_wheels.get(fw_id) {
                let names = fw.get_filter_names().await.map_err(|e| e.to_string())?;
                tracing::debug!("filterwheel_get_names: Found filter wheel with names: {:?}", names);
                return Ok(names);
            } else {
                tracing::warn!("filterwheel_get_names: Native filter wheel '{}' not found in device manager", fw_id);
            }
        }

        Err(format!("Filter wheel {} not found or unsupported", fw_id))
    }

    async fn filterwheel_set_filter_by_name(&self, fw_id: &str, name: &str) -> DeviceResult<i32> {
        tracing::debug!("filterwheel_set_filter_by_name: fw_id='{}', name='{}'", fw_id, name);
        let names = self.filterwheel_get_names(fw_id).await?;
        tracing::debug!("filterwheel_set_filter_by_name: Available filters: {:?}", names);

        if let Some(index) = names.iter().position(|n| n.eq_ignore_ascii_case(name)) {
            let position = index as i32;
            tracing::info!("filterwheel_set_filter_by_name: Moving to position {} for filter '{}'", position, name);
            self.filterwheel_set_position(fw_id, position).await?;
            return Ok(position);
        }

        tracing::error!("filterwheel_set_filter_by_name: Filter '{}' not found in available filters: {:?}", name, names);
        Err(format!("Filter '{}' not found in filter wheel {}. Available: {:?}", name, fw_id, names))
    }
    

    
    // =========================================================================
    // ROTATOR OPERATIONS (Placeholder - to be implemented)
    // =========================================================================
    
    async fn rotator_move_to(&self, rotator_id: &str, angle: f64) -> DeviceResult<()> {
        tracing::info!("Moving rotator {} to {}°", rotator_id, angle);
        
        #[cfg(windows)]
        {
            if rotator_id.starts_with("ascom:") {
                let prog_id = rotator_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();
                
                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let mut rotator = AscomRotator::new(&prog_id)?;
                        rotator.move_absolute(angle)?;
                        
                        // Wait for move to complete
                        while rotator.is_moving()? {
                            std::thread::sleep(std::time::Duration::from_millis(100));
                        }
                        Ok(())
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }
        
        if rotator_id.starts_with("alpaca:") {
            let parts: Vec<&str> = rotator_id.split(':').collect();
            if parts.len() >= 3 {
                let base_url = parts[1];
                let device_number: u32 = parts[2].parse().map_err(|e| format!("Invalid device number: {}", e))?;
                
                let rotator = AlpacaRotator::from_server(base_url, device_number);
                rotator.connect().await?;
                rotator.move_absolute(angle).await?;
                
                // Wait for move to complete
                while rotator.is_moving().await? {
                    tokio::time::sleep(std::time::Duration::from_millis(100)).await;
                }
                
                return Ok(());
            }
        }
        
        Err(format!("Rotator {} not found or unsupported", rotator_id))
    }
    
    async fn rotator_move_relative(&self, rotator_id: &str, delta: f64) -> DeviceResult<()> {
        tracing::info!("Moving rotator {} by {}°", rotator_id, delta);
        
        #[cfg(windows)]
        {
            if rotator_id.starts_with("ascom:") {
                let prog_id = rotator_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();
                
                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let mut rotator = AscomRotator::new(&prog_id)?;
                        
                        // ASCOM doesn't strictly have move_relative, so we calculate target
                        let current = rotator.position()?;
                        let target = (current + delta) % 360.0;
                        let target = if target < 0.0 { target + 360.0 } else { target };
                        
                        rotator.move_absolute(target)?;
                        
                        // Wait for move to complete
                        while rotator.is_moving()? {
                            std::thread::sleep(std::time::Duration::from_millis(100));
                        }
                        Ok(())
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }
        
        if rotator_id.starts_with("alpaca:") {
            let parts: Vec<&str> = rotator_id.split(':').collect();
            if parts.len() >= 3 {
                let base_url = parts[1];
                let device_number: u32 = parts[2].parse().map_err(|e| format!("Invalid device number: {}", e))?;
                
                let rotator = AlpacaRotator::from_server(base_url, device_number);
                rotator.connect().await?;
                
                // Use Alpaca's move_relative if available, or calculate
                // Alpaca 'move' is usually relative? Let's use move_relative wrapper
                rotator.move_relative(delta).await?;
                
                // Wait for move to complete
                while rotator.is_moving().await? {
                    tokio::time::sleep(std::time::Duration::from_millis(100)).await;
                }
                
                return Ok(());
            }
        }
        
        Err(format!("Rotator {} not found or unsupported", rotator_id))
    }
    
    async fn rotator_get_angle(&self, rotator_id: &str) -> DeviceResult<f64> {
        #[cfg(windows)]
        {
            if rotator_id.starts_with("ascom:") {
                let prog_id = rotator_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();
                
                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let rotator = AscomRotator::new(&prog_id)?;
                        Ok(rotator.position()?)
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }
        
        if rotator_id.starts_with("alpaca:") {
            let parts: Vec<&str> = rotator_id.split(':').collect();
            if parts.len() >= 3 {
                let base_url = parts[1];
                let device_number: u32 = parts[2].parse().map_err(|e| format!("Invalid device number: {}", e))?;
                
                let rotator = AlpacaRotator::from_server(base_url, device_number);
                return Ok(rotator.position().await?);
            }
        }
        
        Err(format!("Rotator {} not found or unsupported", rotator_id))
    }

    async fn rotator_halt(&self, rotator_id: &str) -> DeviceResult<()> {
        tracing::info!("Halting rotator {}", rotator_id);
        
        #[cfg(windows)]
        {
            if rotator_id.starts_with("ascom:") {
                let prog_id = rotator_id.strip_prefix("ascom:").ok_or("Invalid ASCOM ID")?.to_string();
                return tokio::task::spawn_blocking(move || {
                    nightshade_ascom::init_com().map_err(|e| format!("COM init failed: {}", e))?;
                    let result = (|| {
                        let mut rotator = AscomRotator::new(&prog_id)?;
                        rotator.halt()?;
                        Ok(())
                    })();
                    nightshade_ascom::uninit_com();
                    result
                }).await.map_err(|e| format!("Task join error: {}", e))?;
            }
        }
        
        if rotator_id.starts_with("alpaca:") {
            let parts: Vec<&str> = rotator_id.split(':').collect();
            if parts.len() >= 3 {
                let base_url = parts[1];
                let device_number: u32 = parts[2].parse().map_err(|e| format!("Invalid device number: {}", e))?;
                
                let rotator = AlpacaRotator::from_server(base_url, device_number);
                rotator.connect().await?;
                rotator.halt().await?;
                return Ok(());
            }
        }
        
        if rotator_id.starts_with("sim_") {
            let mut rotator = get_sim_rotator().write().await;
            rotator.status.moving = false;
            rotator.status.is_moving = false;
            return Ok(());
        }
        
        Err(format!("Rotator {} not found or unsupported", rotator_id))
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
        tracing::info!("Dithering {} pixels (RA only: {})", pixels, ra_only);
        
        // Access PHD2 client from global storage
        let mut storage = crate::api::get_phd2_storage().write().await;
        let client = storage.as_mut()
            .ok_or_else(|| "PHD2 not connected. Please connect to PHD2 first.".to_string())?;
        
        client.dither(pixels, ra_only, settle_pixels, settle_time, settle_timeout)
            .map_err(|e| format!("Dither failed: {}", e))?;
        
        // Wait for settling to complete by polling state
        let start = std::time::Instant::now();
        let timeout = std::time::Duration::from_secs(settle_timeout as u64 + 10);
        
        loop {
            tokio::time::sleep(std::time::Duration::from_millis(500)).await;
            
            if start.elapsed() > timeout {
                return Err("Dither settle timeout".to_string());
            }
            
            // Check if still settling
            match client.get_app_state() {
                Ok(nightshade_imaging::Phd2State::Guiding) => {
                    // Back to guiding, settling complete
                    break;
                }
                Ok(nightshade_imaging::Phd2State::Settling) => {
                    // Still settling, continue waiting
                    continue;
                }
                Ok(nightshade_imaging::Phd2State::LostLock) => {
                    return Err("Lost guide star during dither".to_string());
                }
                Ok(_) => continue,
                Err(e) => {
                    tracing::warn!("Error checking PHD2 state during dither: {}", e);
                    continue;
                }
            }
        }
        
        tracing::info!("Dither complete");
        Ok(())
    }
    
    async fn guider_get_status(&self) -> DeviceResult<GuidingStatus> {
        let mut storage = crate::api::get_phd2_storage().write().await;
        let client = storage.as_mut()
            .ok_or_else(|| "PHD2 not connected".to_string())?;

        let state = client.get_app_state()
            .map_err(|e| format!("Failed to get PHD2 state: {}", e))?;

        let is_guiding = matches!(state, nightshade_imaging::Phd2State::Guiding);

        // Get rolling RMS stats from PHD2 client
        let stats = client.get_rolling_stats();

        Ok(GuidingStatus {
            is_guiding,
            rms_ra: stats.rms_ra,
            rms_dec: stats.rms_dec,
            rms_total: stats.rms_total,
        })
    }
    
    async fn guider_start(&self, settle_pixels: f64, settle_time: f64, settle_timeout: f64) -> DeviceResult<()> {
        tracing::info!("Starting guiding (settle: {} px, {} sec, {} sec timeout)", 
            settle_pixels, settle_time, settle_timeout);
        
        let mut storage = crate::api::get_phd2_storage().write().await;
        let client = storage.as_mut()
            .ok_or_else(|| "PHD2 not connected. Please connect to PHD2 first.".to_string())?;
        
        // Ensure PHD2 is connected to equipment
        let connected = client.get_connected()
            .map_err(|e| format!("Failed to check PHD2 connection: {}", e))?;
        
        if !connected {
            client.set_connected(true)
                .map_err(|e| format!("Failed to connect PHD2 to equipment: {}", e))?;
        }
        
        // Start guiding
        client.guide(settle_pixels, settle_time, settle_timeout)
            .map_err(|e| format!("Failed to start guiding: {}", e))?;
        
        // Wait for guiding to start (or timeout)
        let start = std::time::Instant::now();
        let timeout = std::time::Duration::from_secs(settle_timeout as u64 + 30); // Extra time for calibration
        
        loop {
            tokio::time::sleep(std::time::Duration::from_millis(500)).await;
            
            if start.elapsed() > timeout {
                return Err("Guiding start timeout".to_string());
            }
            
            match client.get_app_state() {
                Ok(nightshade_imaging::Phd2State::Guiding) => {
                    tracing::info!("Guiding started successfully");
                    return Ok(());
                }
                Ok(nightshade_imaging::Phd2State::Calibrating) => {
                    // Still calibrating, continue waiting
                    continue;
                }
                Ok(nightshade_imaging::Phd2State::Settling) => {
                    // Settling after calibration
                    continue;
                }
                Ok(nightshade_imaging::Phd2State::LostLock) => {
                    return Err("Lost guide star".to_string());
                }
                Ok(_) => continue,
                Err(e) => {
                    tracing::warn!("Error checking PHD2 state: {}", e);
                    continue;
                }
            }
        }
    }
    
    async fn guider_stop(&self) -> DeviceResult<()> {
        tracing::info!("Stopping guiding");
        
        let mut storage = crate::api::get_phd2_storage().write().await;
        let client = storage.as_mut()
            .ok_or_else(|| "PHD2 not connected".to_string())?;
        
        client.stop_capture()
            .map_err(|e| format!("Failed to stop guiding: {}", e))?;
        
        tracing::info!("Guiding stopped");
        Ok(())
    }
    
    // =========================================================================
    // PLATE SOLVING
    // =========================================================================
    
    async fn plate_solve(
        &self,
        image_data: &SeqImageData,
        hint_ra: Option<f64>,
        hint_dec: Option<f64>,
        hint_scale: Option<f64>,
    ) -> DeviceResult<PlateSolveResult> {
        tracing::info!("Plate solving image ({}x{} pixels)", image_data.width, image_data.height);
        
        // Save image data to temporary FITS file
        let temp_dir = std::env::temp_dir();
        let temp_path = temp_dir.join(format!("nightshade_solve_{}.fits", 
            std::time::SystemTime::now()
                .duration_since(std::time::UNIX_EPOCH)
                .unwrap_or_default()
                .as_millis()
        ));
        let temp_path_str = temp_path.to_string_lossy().to_string();
        
        // Create FITS header
        let header = crate::api::FitsWriteHeader {
            object_name: Some("Plate Solve".to_string()),
            exposure_time: image_data.exposure_secs,
            capture_timestamp: chrono::Utc::now().format("%Y-%m-%dT%H:%M:%S").to_string(),
            frame_type: "Light".to_string(),
            filter: image_data.filter.clone(),
            gain: image_data.gain,
            offset: image_data.offset,
            ccd_temp: image_data.temperature,
            ra: hint_ra,
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
        
        // Save to temp file
        crate::api::api_save_fits_file(
            temp_path_str.clone(),
            image_data.width,
            image_data.height,
            image_data.data.clone(),
            header,
        ).await.map_err(|e| format!("Failed to save temp FITS for plate solve: {}", e))?;
        
        // Perform plate solve
        let result = if let (Some(ra), Some(dec)) = (hint_ra, hint_dec) {
            // Near-field solve with hints
            crate::api::api_plate_solve_near(
                temp_path_str.clone(),
                ra,
                dec,
                hint_scale.unwrap_or(5.0), // 5 degree search radius default
            ).await
        } else {
            // Blind solve
            crate::api::api_plate_solve_blind(temp_path_str.clone()).await
        };
        
        // Clean up temp file
        let _ = std::fs::remove_file(&temp_path);
        
        // Convert result
        result
            .map(|r| PlateSolveResult {
                ra_degrees: r.ra,
                dec_degrees: r.dec,
                pixel_scale: r.pixel_scale,
                rotation: r.rotation,
                success: true,
            })
            .map_err(|e| format!("Plate solve failed: {}", e))
    }
    
    // =========================================================================
    // IMAGE SAVING (Placeholder - to be implemented)
    // =========================================================================
    
    async fn save_fits(
        &self,
        image_data: &SeqImageData,
        file_path: &str,
        target_name: Option<&str>,
        filter: Option<&str>,
        ra_hours: Option<f64>,
        dec_degrees: Option<f64>,
    ) -> DeviceResult<()> {
        tracing::info!("Saving FITS image to: {}", file_path);
        
        // Create directory if it doesn't exist
        if let Some(parent) = std::path::Path::new(file_path).parent() {
            std::fs::create_dir_all(parent)
                .map_err(|e| format!("Failed to create directory: {}", e))?;
        }
        
        // Build FITS header
        let header = crate::api::FitsWriteHeader {
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
            bin_x: 1,  // TODO: Get from actual binning settings
            bin_y: 1,
            focal_length: None,
            aperture: None,
            pixel_size_x: None,
            pixel_size_y: None,
            site_latitude: None,
            site_longitude: None,
            site_elevation: None,
        };
        
        // Call the API function to save
        crate::api::api_save_fits_file(
            file_path.to_string(),
            image_data.width,
            image_data.height,
            image_data.data.clone(),
            header,
        ).await.map_err(|e| format!("Save FITS failed: {}", e))?;
        
        tracing::info!("FITS file saved successfully: {}", file_path);
        Ok(())
    }
    
    // =========================================================================
    // NOTIFICATIONS
    // =========================================================================
    
    async fn send_notification(&self, level: &str, title: &str, message: &str) -> DeviceResult<()> {
        tracing::info!("[NOTIFICATION][{}] {}: {}", level, title, message);
        
        // Determine severity from level
        let severity = match level.to_lowercase().as_str() {
            "error" | "critical" => crate::event::EventSeverity::Error,
            "warning" | "warn" => crate::event::EventSeverity::Warning,
            _ => crate::event::EventSeverity::Info,
        };
        
        // Create and publish notification event
        let event = crate::event::create_event(
            severity,
            crate::event::EventCategory::System,
            crate::event::EventPayload::System(crate::event::SystemEvent::Notification {
                title: title.to_string(),
                message: message.to_string(),
                level: level.to_string(),
            }),
        );
        
        self.app_state.event_bus.publish(event);
        Ok(())
    }

    // =========================================================================
    // UTILITY
    // =========================================================================

    fn calculate_altitude(&self, ra_hours: f64, dec_degrees: f64, lat: f64, lon: f64) -> f64 {
        // Calculate altitude of a celestial object using standard astronomical formula
        // Convert to radians
        let lat_rad = lat.to_radians();
        let dec_rad = dec_degrees.to_radians();

        // Calculate Local Sidereal Time (LST)
        // Approximate LST using current UTC time and longitude
        let now = chrono::Utc::now();

        // J2000.0 epoch: January 1, 2000, 12:00:00 TT (Julian Date 2451545.0)
        // Calculate days since J2000 using a formula that avoids panics entirely.
        let days_since_j2000 = calculate_days_since_j2000(&now);

        // Greenwich Mean Sidereal Time at 0h UT
        let gmst_at_0h = 280.46061837 + 360.98564736629 * days_since_j2000;
        let ut_hours = now.hour() as f64 + now.minute() as f64 / 60.0 + now.second() as f64 / 3600.0;
        let gmst = gmst_at_0h + 360.0 * ut_hours / 24.0;

        // Local Sidereal Time
        let lst = (gmst + lon) % 360.0;
        let lst_hours = lst / 15.0; // Convert to hours

        // Hour angle
        let ha_hours = lst_hours - ra_hours;
        let ha_rad = (ha_hours * 15.0).to_radians();

        // Calculate altitude
        let sin_alt = lat_rad.sin() * dec_rad.sin() + lat_rad.cos() * dec_rad.cos() * ha_rad.cos();
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

    async fn polar_align_update(&self, result: &nightshade_sequencer::PolarAlignResult) -> DeviceResult<()> {
        let event = crate::event::create_event(
            crate::event::EventSeverity::Info,
            crate::event::EventCategory::PolarAlignment,
            crate::event::EventPayload::PolarAlignment(crate::event::PolarAlignmentEvent {
                azimuth_error: result.azimuth_error,
                altitude_error: result.altitude_error,
                total_error: result.total_error,
                current_ra: result.current_ra,
                current_dec: result.current_dec,
                target_ra: result.target_ra,
                target_dec: result.target_dec,
            })
        );

        self.app_state.event_bus.publish(event);
        Ok(())
    }

    async fn safety_is_safe(&self, safety_id: Option<&str>) -> DeviceResult<bool> {
        // If no safety monitor configured, assume safe (fail-open for usability)
        let device_id = match safety_id {
            Some(id) => id.to_string(),
            None => {
                // Try to get from profile
                match self.get_device_id(crate::device::DeviceType::Weather) {
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
                let safety = AlpacaSafetyMonitor::from_server(&base_url, device_num);

                match safety.connect().await {
                    Ok(()) => {
                        let is_safe = safety.is_safe().await.unwrap_or_else(|e| {
                            tracing::warn!("Failed to get safety status: {}", e);
                            true // Fail-open
                        });
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

        // INDI safety monitors are checked via the IndiSafetyMonitor wrapper
        // For now, if we don't recognize the device type, assume safe
        tracing::debug!("Unknown safety monitor type for {}, assuming safe", device_id);
        Ok(true)
    }

    // =========================================================================
    // IMAGE ANALYSIS
    // =========================================================================

    async fn calculate_image_hfr(&self, image_data: &nightshade_sequencer::ImageData) -> DeviceResult<Option<f64>> {
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

        // Calculate average HFR
        let total_hfr: f64 = stars.iter().map(|s| s.hfr).sum();
        let avg_hfr = total_hfr / stars.len() as f64;

        Ok(Some(avg_hfr))
    }

    async fn detect_stars_in_image(&self, image_data: &nightshade_sequencer::ImageData) -> DeviceResult<Vec<(f64, f64, f64)>> {
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
    // COVER CALIBRATOR (FLAT PANEL)
    // =========================================================================

    async fn cover_calibrator_open_cover(&self, device_id: &str) -> DeviceResult<()> {
        self.device_manager
            .cover_calibrator_open_cover(device_id)
            .await
    }

    async fn cover_calibrator_close_cover(&self, device_id: &str) -> DeviceResult<()> {
        self.device_manager
            .cover_calibrator_close_cover(device_id)
            .await
    }

    async fn cover_calibrator_halt_cover(&self, device_id: &str) -> DeviceResult<()> {
        self.device_manager
            .cover_calibrator_halt_cover(device_id)
            .await
    }

    async fn cover_calibrator_calibrator_on(&self, device_id: &str, brightness: i32) -> DeviceResult<()> {
        self.device_manager
            .cover_calibrator_calibrator_on(device_id, brightness)
            .await
    }

    async fn cover_calibrator_calibrator_off(&self, device_id: &str) -> DeviceResult<()> {
        self.device_manager
            .cover_calibrator_calibrator_off(device_id)
            .await
    }

    async fn cover_calibrator_get_cover_state(&self, device_id: &str) -> DeviceResult<i32> {
        self.device_manager
            .cover_calibrator_get_cover_state(device_id)
            .await
    }

    async fn cover_calibrator_get_calibrator_state(&self, device_id: &str) -> DeviceResult<i32> {
        self.device_manager
            .cover_calibrator_get_calibrator_state(device_id)
            .await
    }

    async fn cover_calibrator_get_brightness(&self, device_id: &str) -> DeviceResult<i32> {
        self.device_manager
            .cover_calibrator_get_brightness(device_id)
            .await
    }

    async fn cover_calibrator_get_max_brightness(&self, device_id: &str) -> DeviceResult<i32> {
        self.device_manager
            .cover_calibrator_get_max_brightness(device_id)
            .await
    }
}

/// Decode base64 string to bytes (for Alpaca ImageArray)
fn decode_base64(input: &str) -> Result<Vec<u8>, String> {
    const ALPHABET: &[u8] = b"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

    let input = input.trim().replace('\n', "").replace('\r', "");
    let mut output = Vec::with_capacity(input.len() * 3 / 4);

    let mut buf = 0u32;
    let mut bits = 0;

    for c in input.bytes() {
        if c == b'=' {
            break;
        }

        let val = ALPHABET.iter().position(|&x| x == c)
            .ok_or_else(|| format!("Invalid base64 character: {}", c as char))?;

        buf = (buf << 6) | (val as u32);
        bits += 6;

        if bits >= 8 {
            bits -= 8;
            output.push((buf >> bits) as u8);
            buf &= (1 << bits) - 1;
        }
    }

    Ok(output)
}

/// Calculate days since J2000.0 epoch without any unwrap/panic potential.
///
/// J2000.0 epoch is January 1, 2000, 12:00:00 TT (Julian Date 2451545.0)
/// This uses a direct calculation approach that cannot panic.
///
/// This is a module-level helper function to avoid issues with trait impl blocks.
fn calculate_days_since_j2000(dt: &chrono::DateTime<chrono::Utc>) -> f64 {
    // Julian Date formula (Meeus, Astronomical Algorithms)
    // This formula works for any valid DateTime and never panics
    let year = dt.year();
    let month = dt.month() as i32;
    let day = dt.day() as i32;

    // Adjust year and month for the algorithm (Jan/Feb are month 13/14 of prev year)
    let (y, m) = if month <= 2 {
        (year - 1, month + 12)
    } else {
        (year, month)
    };

    // Calculate Julian Day Number for the date
    let a = y / 100;
    let b = 2 - a + (a / 4);

    // Integer part of Julian Day Number at noon
    let jdn = (365.25 * (y + 4716) as f64).floor()
        + (30.6001 * (m + 1) as f64).floor()
        + day as f64
        + b as f64
        - 1524.5;

    // Add fractional day from time
    let hours = dt.hour() as f64;
    let minutes = dt.minute() as f64;
    let seconds = dt.second() as f64;
    let fractional_day = (hours + minutes / 60.0 + seconds / 3600.0) / 24.0;

    let julian_date = jdn + fractional_day;

    // J2000.0 epoch is Julian Date 2451545.0
    const J2000_JD: f64 = 2451545.0;
    julian_date - J2000_JD
}