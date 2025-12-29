//! INDI Cover Calibrator wrapper
//!
//! Provides high-level flat panel / dust cover control via INDI protocol.
//! Supports various INDI flat panel implementations including:
//! - Dust cap control (CAP_PARK, DUSTCAP_CONTROL)
//! - Flat light control (FLAT_LIGHT_CONTROL)
//! - Brightness control (FLAT_LIGHT_INTENSITY, LIGHTBOX_BRIGHTNESS)

use crate::client::IndiClient;
use crate::error::IndiResult;
use std::sync::Arc;
use tokio::sync::RwLock;

/// Cover state for INDI cover calibrators
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum IndiCoverState {
    /// Device does not have a cover
    NotPresent,
    /// Cover is closed/parked
    Closed,
    /// Cover is moving
    Moving,
    /// Cover is open/unparked
    Open,
    /// Cover state is unknown
    Unknown,
    /// Error condition
    Error,
}

impl IndiCoverState {
    /// Convert to ASCOM-compatible integer value
    pub fn to_i32(&self) -> i32 {
        match self {
            IndiCoverState::NotPresent => 0,
            IndiCoverState::Closed => 1,
            IndiCoverState::Moving => 2,
            IndiCoverState::Open => 3,
            IndiCoverState::Unknown => 4,
            IndiCoverState::Error => 5,
        }
    }
}

/// Calibrator state for INDI cover calibrators
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum IndiCalibratorState {
    /// Device does not have a calibrator/light
    NotPresent,
    /// Calibrator is off
    Off,
    /// Calibrator is stabilizing
    NotReady,
    /// Calibrator is on and ready
    Ready,
    /// Calibrator state is unknown
    Unknown,
    /// Error condition
    Error,
}

impl IndiCalibratorState {
    /// Convert to ASCOM-compatible integer value
    pub fn to_i32(&self) -> i32 {
        match self {
            IndiCalibratorState::NotPresent => 0,
            IndiCalibratorState::Off => 1,
            IndiCalibratorState::NotReady => 2,
            IndiCalibratorState::Ready => 3,
            IndiCalibratorState::Unknown => 4,
            IndiCalibratorState::Error => 5,
        }
    }
}

/// INDI Cover Calibrator device wrapper
pub struct IndiCoverCalibrator {
    client: Arc<RwLock<IndiClient>>,
    device_name: String,
}

impl IndiCoverCalibrator {
    /// Create a new INDI cover calibrator wrapper
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

    // =========================================================================
    // Connection
    // =========================================================================

    /// Connect to the cover calibrator
    pub async fn connect(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.connect_device(&self.device_name).await
    }

    /// Disconnect from the cover calibrator
    pub async fn disconnect(&self) -> IndiResult<()> {
        let mut client = self.client.write().await;
        client.disconnect_device(&self.device_name).await
    }

    /// Check if connected
    pub async fn is_connected(&self) -> bool {
        let client = self.client.read().await;
        client.is_device_connected(&self.device_name).await
    }

    // =========================================================================
    // Cover Control (Dust Cap)
    // =========================================================================

    /// Open the cover (unpark dust cap)
    pub async fn open_cover(&self) -> Result<(), String> {
        let mut client = self.client.write().await;

        // Try CAP_PARK first (common INDI standard)
        if client.set_switch(&self.device_name, "CAP_PARK", "UNPARK", true).await.is_ok() {
            return Ok(());
        }

        // Try DUSTCAP_CONTROL alternative
        if client.set_switch(&self.device_name, "DUSTCAP_CONTROL", "OPEN", true).await.is_ok() {
            return Ok(());
        }

        Err("No compatible cover control property found".to_string())
    }

    /// Close the cover (park dust cap)
    pub async fn close_cover(&self) -> Result<(), String> {
        let mut client = self.client.write().await;

        // Try CAP_PARK first (common INDI standard)
        if client.set_switch(&self.device_name, "CAP_PARK", "PARK", true).await.is_ok() {
            return Ok(());
        }

        // Try DUSTCAP_CONTROL alternative
        if client.set_switch(&self.device_name, "DUSTCAP_CONTROL", "CLOSE", true).await.is_ok() {
            return Ok(());
        }

        Err("No compatible cover control property found".to_string())
    }

    /// Halt cover movement (abort)
    pub async fn halt_cover(&self) -> Result<(), String> {
        let mut client = self.client.write().await;

        // Try standard abort property
        if client.set_switch(&self.device_name, "CAP_ABORT", "ABORT", true).await.is_ok() {
            return Ok(());
        }

        // Some devices use generic abort
        client.set_switch(&self.device_name, "ABORT", "ABORT", true).await.map_err(|e| e.to_string())
    }

    /// Get cover state
    pub async fn get_cover_state(&self) -> IndiCoverState {
        let client = self.client.read().await;

        // Check CAP_PARK property
        let is_parked = client.get_switch(&self.device_name, "CAP_PARK", "PARK")
            .await
            .unwrap_or(false);
        let is_unparked = client.get_switch(&self.device_name, "CAP_PARK", "UNPARK")
            .await
            .unwrap_or(false);

        // Check if property is busy (moving)
        if client.is_property_busy(&self.device_name, "CAP_PARK").await {
            return IndiCoverState::Moving;
        }

        if is_parked && !is_unparked {
            return IndiCoverState::Closed;
        }
        if is_unparked && !is_parked {
            return IndiCoverState::Open;
        }

        // Try DUSTCAP_CONTROL alternative
        let is_closed = client.get_switch(&self.device_name, "DUSTCAP_CONTROL", "CLOSE")
            .await
            .unwrap_or(false);
        let is_open = client.get_switch(&self.device_name, "DUSTCAP_CONTROL", "OPEN")
            .await
            .unwrap_or(false);

        if client.is_property_busy(&self.device_name, "DUSTCAP_CONTROL").await {
            return IndiCoverState::Moving;
        }

        if is_closed && !is_open {
            return IndiCoverState::Closed;
        }
        if is_open && !is_closed {
            return IndiCoverState::Open;
        }

        // Check if cover properties exist at all
        let properties = client.get_properties(&self.device_name).await;
        let has_cover = properties.iter().any(|p| {
            p.name == "CAP_PARK" || p.name == "DUSTCAP_CONTROL"
        });

        if has_cover {
            IndiCoverState::Unknown
        } else {
            IndiCoverState::NotPresent
        }
    }

    // =========================================================================
    // Calibrator Control (Flat Light)
    // =========================================================================

    /// Turn on the calibrator light at specified brightness
    pub async fn calibrator_on(&self, brightness: i32) -> Result<(), String> {
        let mut client = self.client.write().await;

        // Set brightness first if supported
        if brightness > 0 {
            // Try FLAT_LIGHT_INTENSITY
            let _ = client.set_number(
                &self.device_name,
                "FLAT_LIGHT_INTENSITY",
                "FLAT_LIGHT_INTENSITY_VALUE",
                brightness as f64
            ).await;

            // Try LIGHTBOX_BRIGHTNESS alternative
            let _ = client.set_number(
                &self.device_name,
                "LIGHTBOX_BRIGHTNESS",
                "BRIGHTNESS",
                brightness as f64
            ).await;
        }

        // Turn on the light
        // Try FLAT_LIGHT_CONTROL first
        if client.set_switch(&self.device_name, "FLAT_LIGHT_CONTROL", "FLAT_LIGHT_ON", true).await.is_ok() {
            return Ok(());
        }

        // Try LIGHTBOX_BRIGHTNESS (setting to non-zero turns it on)
        if client.set_number(
            &self.device_name,
            "LIGHTBOX_BRIGHTNESS",
            "BRIGHTNESS",
            brightness.max(1) as f64
        ).await.is_ok() {
            return Ok(());
        }

        Err("No compatible calibrator control property found".to_string())
    }

    /// Turn off the calibrator light
    pub async fn calibrator_off(&self) -> Result<(), String> {
        let mut client = self.client.write().await;

        // Try FLAT_LIGHT_CONTROL first
        if client.set_switch(&self.device_name, "FLAT_LIGHT_CONTROL", "FLAT_LIGHT_OFF", true).await.is_ok() {
            return Ok(());
        }

        // Try setting brightness to 0
        if client.set_number(
            &self.device_name,
            "FLAT_LIGHT_INTENSITY",
            "FLAT_LIGHT_INTENSITY_VALUE",
            0.0
        ).await.is_ok() {
            return Ok(());
        }

        // Try LIGHTBOX_BRIGHTNESS alternative
        if client.set_number(
            &self.device_name,
            "LIGHTBOX_BRIGHTNESS",
            "BRIGHTNESS",
            0.0
        ).await.is_ok() {
            return Ok(());
        }

        Err("No compatible calibrator control property found".to_string())
    }

    /// Get calibrator state
    pub async fn get_calibrator_state(&self) -> IndiCalibratorState {
        let client = self.client.read().await;

        // Check FLAT_LIGHT_CONTROL property
        let is_on = client.get_switch(&self.device_name, "FLAT_LIGHT_CONTROL", "FLAT_LIGHT_ON")
            .await
            .unwrap_or(false);
        let is_off = client.get_switch(&self.device_name, "FLAT_LIGHT_CONTROL", "FLAT_LIGHT_OFF")
            .await
            .unwrap_or(false);

        // Check if property is busy (stabilizing)
        if client.is_property_busy(&self.device_name, "FLAT_LIGHT_CONTROL").await {
            return IndiCalibratorState::NotReady;
        }

        if is_on && !is_off {
            return IndiCalibratorState::Ready;
        }
        if is_off && !is_on {
            return IndiCalibratorState::Off;
        }

        // Check brightness-based control
        if let Some(brightness) = client.get_number(
            &self.device_name,
            "FLAT_LIGHT_INTENSITY",
            "FLAT_LIGHT_INTENSITY_VALUE"
        ).await {
            if brightness > 0.0 {
                return IndiCalibratorState::Ready;
            } else {
                return IndiCalibratorState::Off;
            }
        }

        // Check LIGHTBOX_BRIGHTNESS alternative
        if let Some(brightness) = client.get_number(
            &self.device_name,
            "LIGHTBOX_BRIGHTNESS",
            "BRIGHTNESS"
        ).await {
            if brightness > 0.0 {
                return IndiCalibratorState::Ready;
            } else {
                return IndiCalibratorState::Off;
            }
        }

        // Check if calibrator properties exist at all
        let properties = client.get_properties(&self.device_name).await;
        let has_calibrator = properties.iter().any(|p| {
            p.name == "FLAT_LIGHT_CONTROL"
                || p.name == "FLAT_LIGHT_INTENSITY"
                || p.name == "LIGHTBOX_BRIGHTNESS"
        });

        if has_calibrator {
            IndiCalibratorState::Unknown
        } else {
            IndiCalibratorState::NotPresent
        }
    }

    // =========================================================================
    // Brightness Control
    // =========================================================================

    /// Get current brightness (0-max)
    pub async fn get_brightness(&self) -> Result<i32, String> {
        let client = self.client.read().await;

        // Try FLAT_LIGHT_INTENSITY
        if let Some(brightness) = client.get_number(
            &self.device_name,
            "FLAT_LIGHT_INTENSITY",
            "FLAT_LIGHT_INTENSITY_VALUE"
        ).await {
            return Ok(brightness as i32);
        }

        // Try LIGHTBOX_BRIGHTNESS alternative
        if let Some(brightness) = client.get_number(
            &self.device_name,
            "LIGHTBOX_BRIGHTNESS",
            "BRIGHTNESS"
        ).await {
            return Ok(brightness as i32);
        }

        Err("Brightness property not available".to_string())
    }

    /// Set brightness (0-max)
    pub async fn set_brightness(&self, brightness: i32) -> Result<(), String> {
        let mut client = self.client.write().await;

        // Try FLAT_LIGHT_INTENSITY
        if client.set_number(
            &self.device_name,
            "FLAT_LIGHT_INTENSITY",
            "FLAT_LIGHT_INTENSITY_VALUE",
            brightness as f64
        ).await.is_ok() {
            return Ok(());
        }

        // Try LIGHTBOX_BRIGHTNESS alternative
        if client.set_number(
            &self.device_name,
            "LIGHTBOX_BRIGHTNESS",
            "BRIGHTNESS",
            brightness as f64
        ).await.is_ok() {
            return Ok(());
        }

        Err("Brightness property not available".to_string())
    }

    /// Get maximum brightness
    ///
    /// INDI properties have min/max values defined in the defNumber element,
    /// but the current client doesn't expose these. We default to 255 which
    /// is the common maximum for most flat panels.
    pub async fn get_max_brightness(&self) -> Result<i32, String> {
        // Most flat panels use 0-255 brightness range
        // Future enhancement: parse min/max from INDI property definition
        Ok(255)
    }
}
