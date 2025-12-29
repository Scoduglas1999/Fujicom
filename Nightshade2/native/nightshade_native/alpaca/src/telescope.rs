//! Alpaca Telescope (Mount) API implementation

use crate::{AlpacaClient, AlpacaClientBuilder, AlpacaDevice, AlpacaDeviceType, AlpacaError, TimeoutConfig, RetryConfig};
use std::time::Duration;

/// Pier side enum
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum PierSide {
    East = 0,
    West = 1,
    Unknown = -1,
}

impl From<i32> for PierSide {
    fn from(value: i32) -> Self {
        match value {
            0 => PierSide::East,
            1 => PierSide::West,
            _ => PierSide::Unknown,
        }
    }
}

impl std::fmt::Display for PierSide {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            PierSide::East => write!(f, "East"),
            PierSide::West => write!(f, "West"),
            PierSide::Unknown => write!(f, "Unknown"),
        }
    }
}

/// Tracking rate enum
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum DriveRate {
    Sidereal = 0,
    Lunar = 1,
    Solar = 2,
    King = 3,
}

impl From<i32> for DriveRate {
    fn from(value: i32) -> Self {
        match value {
            0 => DriveRate::Sidereal,
            1 => DriveRate::Lunar,
            2 => DriveRate::Solar,
            3 => DriveRate::King,
            _ => DriveRate::Sidereal,
        }
    }
}

impl std::fmt::Display for DriveRate {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            DriveRate::Sidereal => write!(f, "Sidereal"),
            DriveRate::Lunar => write!(f, "Lunar"),
            DriveRate::Solar => write!(f, "Solar"),
            DriveRate::King => write!(f, "King"),
        }
    }
}

/// Alignment mode enum matching ASCOM AlignmentModes
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum AlignmentMode {
    AltAz = 0,
    Polar = 1,
    GermanPolar = 2,
}

impl From<i32> for AlignmentMode {
    fn from(value: i32) -> Self {
        match value {
            0 => AlignmentMode::AltAz,
            1 => AlignmentMode::Polar,
            2 => AlignmentMode::GermanPolar,
            _ => AlignmentMode::AltAz,
        }
    }
}

/// Telescope status aggregate for parallel status query
#[derive(Debug, Clone)]
pub struct TelescopeStatus {
    pub connected: bool,
    pub right_ascension: f64,
    pub declination: f64,
    pub altitude: f64,
    pub azimuth: f64,
    pub slewing: bool,
    pub tracking: bool,
    pub tracking_rate: DriveRate,
    pub at_home: bool,
    pub at_park: bool,
    pub side_of_pier: PierSide,
    pub sidereal_time: f64,
}

/// Telescope capabilities for determining what features are available
#[derive(Debug, Clone)]
pub struct TelescopeCapabilities {
    pub can_find_home: bool,
    pub can_park: bool,
    pub can_unpark: bool,
    pub can_set_park: bool,
    pub can_slew: bool,
    pub can_slew_async: bool,
    pub can_slew_alt_az: bool,
    pub can_slew_alt_az_async: bool,
    pub can_sync: bool,
    pub can_sync_alt_az: bool,
    pub can_set_tracking: bool,
    pub can_pulse_guide: bool,
    pub can_set_guide_rates: bool,
}

/// Telescope site information
#[derive(Debug, Clone)]
pub struct TelescopeSiteInfo {
    pub site_latitude: f64,
    pub site_longitude: f64,
    pub site_elevation: f64,
}

/// Telescope optical information
#[derive(Debug, Clone)]
pub struct TelescopeOpticsInfo {
    pub aperture_area: Option<f64>,
    pub aperture_diameter: Option<f64>,
    pub focal_length: Option<f64>,
    pub alignment_mode: AlignmentMode,
}

/// Alpaca Telescope (Mount) client
pub struct AlpacaTelescope {
    client: AlpacaClient,
}

impl AlpacaTelescope {
    /// Create a new Alpaca telescope client
    pub fn new(device: &AlpacaDevice) -> Self {
        assert_eq!(device.device_type, AlpacaDeviceType::Telescope);
        Self {
            client: AlpacaClient::new(device),
        }
    }

    /// Create a telescope client with custom configuration
    pub fn with_config(
        device: &AlpacaDevice,
        timeout_config: TimeoutConfig,
        retry_config: RetryConfig,
    ) -> Self {
        assert_eq!(device.device_type, AlpacaDeviceType::Telescope);
        Self {
            client: AlpacaClient::with_config(device, timeout_config, retry_config),
        }
    }

    /// Create from server details
    pub fn from_server(base_url: &str, device_number: u32) -> Self {
        let device = AlpacaDevice {
            device_type: AlpacaDeviceType::Telescope,
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

    // Telescope information

    pub async fn name(&self) -> Result<String, String> {
        self.client.get_name().await
    }

    pub async fn description(&self) -> Result<String, String> {
        self.client.get_description().await
    }

    pub async fn alignment_mode(&self) -> Result<i32, String> {
        self.client.get("alignmentmode").await
    }

    pub async fn aperture_area(&self) -> Result<f64, String> {
        self.client.get("aperturearea").await
    }

    pub async fn aperture_diameter(&self) -> Result<f64, String> {
        self.client.get("aperturediameter").await
    }

    pub async fn focal_length(&self) -> Result<f64, String> {
        self.client.get("focallength").await
    }

    pub async fn equatorial_system(&self) -> Result<i32, String> {
        self.client.get("equatorialsystem").await
    }

    // Position

    pub async fn right_ascension(&self) -> Result<f64, String> {
        self.client.get("rightascension").await
    }

    pub async fn declination(&self) -> Result<f64, String> {
        self.client.get("declination").await
    }

    pub async fn altitude(&self) -> Result<f64, String> {
        self.client.get("altitude").await
    }

    pub async fn azimuth(&self) -> Result<f64, String> {
        self.client.get("azimuth").await
    }

    pub async fn sidereal_time(&self) -> Result<f64, String> {
        self.client.get("siderealtime").await
    }

    pub async fn utc_date(&self) -> Result<String, String> {
        self.client.get("utcdate").await
    }

    // Target coordinates

    pub async fn target_right_ascension(&self) -> Result<f64, String> {
        self.client.get("targetrightascension").await
    }

    pub async fn set_target_right_ascension(&self, ra: f64) -> Result<(), String> {
        self.client.put("targetrightascension", &[("TargetRightAscension", &ra.to_string())]).await
    }

    pub async fn target_declination(&self) -> Result<f64, String> {
        self.client.get("targetdeclination").await
    }

    pub async fn set_target_declination(&self, dec: f64) -> Result<(), String> {
        self.client.put("targetdeclination", &[("TargetDeclination", &dec.to_string())]).await
    }

    // Status

    pub async fn slewing(&self) -> Result<bool, String> {
        self.client.get("slewing").await
    }

    pub async fn tracking(&self) -> Result<bool, String> {
        self.client.get("tracking").await
    }

    pub async fn set_tracking(&self, tracking: bool) -> Result<(), String> {
        self.client.put("tracking", &[("Tracking", &tracking.to_string())]).await
    }

    pub async fn tracking_rate(&self) -> Result<DriveRate, String> {
        let rate: i32 = self.client.get("trackingrate").await?;
        Ok(DriveRate::from(rate))
    }

    pub async fn set_tracking_rate(&self, rate: DriveRate) -> Result<(), String> {
        self.client.put("trackingrate", &[("TrackingRate", &(rate as i32).to_string())]).await
    }

    pub async fn at_home(&self) -> Result<bool, String> {
        self.client.get("athome").await
    }

    pub async fn at_park(&self) -> Result<bool, String> {
        self.client.get("atpark").await
    }

    pub async fn side_of_pier(&self) -> Result<PierSide, String> {
        let side: i32 = self.client.get("sideofpier").await?;
        Ok(PierSide::from(side))
    }

    pub async fn set_side_of_pier(&self, side: PierSide) -> Result<(), String> {
        self.client.put("sideofpier", &[("SideOfPier", &(side as i32).to_string())]).await
    }

    // Site location

    pub async fn site_latitude(&self) -> Result<f64, String> {
        self.client.get("sitelatitude").await
    }

    pub async fn set_site_latitude(&self, lat: f64) -> Result<(), String> {
        self.client.put("sitelatitude", &[("SiteLatitude", &lat.to_string())]).await
    }

    pub async fn site_longitude(&self) -> Result<f64, String> {
        self.client.get("sitelongitude").await
    }

    pub async fn set_site_longitude(&self, lon: f64) -> Result<(), String> {
        self.client.put("sitelongitude", &[("SiteLongitude", &lon.to_string())]).await
    }

    pub async fn site_elevation(&self) -> Result<f64, String> {
        self.client.get("siteelevation").await
    }

    pub async fn set_site_elevation(&self, elev: f64) -> Result<(), String> {
        self.client.put("siteelevation", &[("SiteElevation", &elev.to_string())]).await
    }

    // Capabilities

    pub async fn can_find_home(&self) -> Result<bool, String> {
        self.client.get("canfindhome").await
    }

    pub async fn can_park(&self) -> Result<bool, String> {
        self.client.get("canpark").await
    }

    pub async fn can_unpark(&self) -> Result<bool, String> {
        self.client.get("canunpark").await
    }

    pub async fn can_set_park(&self) -> Result<bool, String> {
        self.client.get("cansetpark").await
    }

    pub async fn can_slew(&self) -> Result<bool, String> {
        self.client.get("canslew").await
    }

    pub async fn can_slew_async(&self) -> Result<bool, String> {
        self.client.get("canslewasync").await
    }

    pub async fn can_slew_alt_az(&self) -> Result<bool, String> {
        self.client.get("canslewaltaz").await
    }

    pub async fn can_slew_alt_az_async(&self) -> Result<bool, String> {
        self.client.get("canslewaltazasync").await
    }

    pub async fn can_sync(&self) -> Result<bool, String> {
        self.client.get("cansync").await
    }

    pub async fn can_sync_alt_az(&self) -> Result<bool, String> {
        self.client.get("cansyncaltaz").await
    }

    pub async fn can_set_tracking(&self) -> Result<bool, String> {
        self.client.get("cansettracking").await
    }

    pub async fn can_pulse_guide(&self) -> Result<bool, String> {
        self.client.get("canpulseguide").await
    }

    pub async fn can_set_guide_rates(&self) -> Result<bool, String> {
        self.client.get("cansetguiderates").await
    }

    pub async fn can_set_right_ascension_rate(&self) -> Result<bool, String> {
        self.client.get("cansetrightascensionrate").await
    }

    pub async fn can_set_declination_rate(&self) -> Result<bool, String> {
        self.client.get("cansetdeclinationrate").await
    }

    pub async fn can_move_axis(&self, axis: i32) -> Result<bool, String> {
        // axis: 0=Primary (RA), 1=Secondary (Dec), 2=Tertiary
        self.client.get(&format!("canmoveaxis?Axis={}", axis)).await
    }

    // Guide rates

    pub async fn guide_rate_right_ascension(&self) -> Result<f64, String> {
        self.client.get("guideraterightascension").await
    }

    pub async fn set_guide_rate_right_ascension(&self, rate: f64) -> Result<(), String> {
        self.client.put("guideraterightascension", &[("GuideRateRightAscension", &rate.to_string())]).await
    }

    pub async fn guide_rate_declination(&self) -> Result<f64, String> {
        self.client.get("guideratedeclination").await
    }

    pub async fn set_guide_rate_declination(&self, rate: f64) -> Result<(), String> {
        self.client.put("guideratedeclination", &[("GuideRateDeclination", &rate.to_string())]).await
    }

    pub async fn is_pulse_guiding(&self) -> Result<bool, String> {
        self.client.get("ispulseguiding").await
    }

    // Movement commands

    pub async fn abort_slew(&self) -> Result<(), String> {
        self.client.put("abortslew", &[]).await
    }

    /// Find the telescope home position
    /// Uses long timeout as homing can take several minutes
    pub async fn find_home(&self) -> Result<(), String> {
        self.find_home_typed().await.map_err(|e| e.to_string())
    }

    /// Find home with typed error handling and long timeout
    pub async fn find_home_typed(&self) -> Result<(), AlpacaError> {
        self.client.put_long("findhome", &[]).await
    }

    /// Park the telescope
    /// Uses long timeout as parking can take several minutes for some mounts
    pub async fn park(&self) -> Result<(), String> {
        self.park_typed().await.map_err(|e| e.to_string())
    }

    /// Park with typed error handling and long timeout
    pub async fn park_typed(&self) -> Result<(), AlpacaError> {
        self.client.put_long("park", &[]).await
    }

    /// Unpark the telescope
    pub async fn unpark(&self) -> Result<(), String> {
        self.unpark_typed().await.map_err(|e| e.to_string())
    }

    /// Unpark with typed error handling
    pub async fn unpark_typed(&self) -> Result<(), AlpacaError> {
        self.client.put_typed("unpark", &[]).await
    }

    pub async fn set_park(&self) -> Result<(), String> {
        self.client.put("setpark", &[]).await
    }

    /// Slew to equatorial coordinates (blocking)
    /// Uses long timeout as slewing can take several minutes
    pub async fn slew_to_coordinates(&self, ra: f64, dec: f64) -> Result<(), String> {
        self.slew_to_coordinates_typed(ra, dec).await.map_err(|e| e.to_string())
    }

    /// Slew to coordinates with typed error handling and long timeout
    pub async fn slew_to_coordinates_typed(&self, ra: f64, dec: f64) -> Result<(), AlpacaError> {
        self.client.put_long("slewtocoordinates", &[
            ("RightAscension", &ra.to_string()),
            ("Declination", &dec.to_string()),
        ]).await
    }

    /// Slew to equatorial coordinates (async - starts slew and returns immediately)
    pub async fn slew_to_coordinates_async(&self, ra: f64, dec: f64) -> Result<(), String> {
        self.client.put("slewtocoordinatesasync", &[
            ("RightAscension", &ra.to_string()),
            ("Declination", &dec.to_string()),
        ]).await
    }

    /// Slew to target coordinates (blocking)
    /// Uses long timeout as slewing can take several minutes
    pub async fn slew_to_target(&self) -> Result<(), String> {
        self.slew_to_target_typed().await.map_err(|e| e.to_string())
    }

    /// Slew to target with typed error handling and long timeout
    pub async fn slew_to_target_typed(&self) -> Result<(), AlpacaError> {
        self.client.put_long("slewtotarget", &[]).await
    }

    /// Slew to target coordinates (async - starts slew and returns immediately)
    pub async fn slew_to_target_async(&self) -> Result<(), String> {
        self.client.put("slewtotargetasync", &[]).await
    }

    /// Slew to alt-az coordinates (blocking)
    /// Uses long timeout as slewing can take several minutes
    pub async fn slew_to_alt_az(&self, alt: f64, az: f64) -> Result<(), String> {
        self.slew_to_alt_az_typed(alt, az).await.map_err(|e| e.to_string())
    }

    /// Slew to alt-az with typed error handling and long timeout
    pub async fn slew_to_alt_az_typed(&self, alt: f64, az: f64) -> Result<(), AlpacaError> {
        self.client.put_long("slewtoaltaz", &[
            ("Altitude", &alt.to_string()),
            ("Azimuth", &az.to_string()),
        ]).await
    }

    /// Slew to alt-az coordinates (async - starts slew and returns immediately)
    pub async fn slew_to_alt_az_async(&self, alt: f64, az: f64) -> Result<(), String> {
        self.client.put("slewtoaltazasync", &[
            ("Altitude", &alt.to_string()),
            ("Azimuth", &az.to_string()),
        ]).await
    }

    /// Sync to equatorial coordinates
    pub async fn sync_to_coordinates(&self, ra: f64, dec: f64) -> Result<(), String> {
        self.client.put("synctocoordinates", &[
            ("RightAscension", &ra.to_string()),
            ("Declination", &dec.to_string()),
        ]).await
    }

    /// Sync to target coordinates
    pub async fn sync_to_target(&self) -> Result<(), String> {
        self.client.put("synctotarget", &[]).await
    }

    /// Sync to alt-az coordinates
    pub async fn sync_to_alt_az(&self, alt: f64, az: f64) -> Result<(), String> {
        self.client.put("synctoaltaz", &[
            ("Altitude", &alt.to_string()),
            ("Azimuth", &az.to_string()),
        ]).await
    }

    /// Pulse guide in a direction
    pub async fn pulse_guide(&self, direction: i32, duration_ms: i32) -> Result<(), String> {
        self.client.put("pulseguide", &[
            ("Direction", &direction.to_string()),
            ("Duration", &duration_ms.to_string()),
        ]).await
    }

    /// Move axis at a rate
    pub async fn move_axis(&self, axis: i32, rate: f64) -> Result<(), String> {
        self.client.put("moveaxis", &[
            ("Axis", &axis.to_string()),
            ("Rate", &rate.to_string()),
        ]).await
    }

    /// Wait for telescope to stop slewing with configurable timeout
    pub async fn wait_for_slew_complete(
        &self,
        poll_interval: Duration,
        timeout: Duration,
    ) -> Result<bool, AlpacaError> {
        let deadline = std::time::Instant::now() + timeout;

        loop {
            match self.slewing().await {
                Ok(false) => return Ok(true),
                Ok(true) => {
                    if std::time::Instant::now() >= deadline {
                        return Ok(false);
                    }
                    tokio::time::sleep(poll_interval).await;
                }
                Err(e) => return Err(AlpacaError::OperationFailed(e)),
            }
        }
    }

    /// Slew to coordinates async and wait for completion
    /// Uses polling to check for slew completion with configurable timeout
    pub async fn slew_to_coordinates_and_wait(
        &self,
        ra: f64,
        dec: f64,
        poll_interval: Duration,
        timeout: Duration,
    ) -> Result<bool, AlpacaError> {
        self.slew_to_coordinates_async(ra, dec).await
            .map_err(|e| AlpacaError::OperationFailed(e))?;
        self.wait_for_slew_complete(poll_interval, timeout).await
    }

    /// Slew to alt-az async and wait for completion
    pub async fn slew_to_alt_az_and_wait(
        &self,
        alt: f64,
        az: f64,
        poll_interval: Duration,
        timeout: Duration,
    ) -> Result<bool, AlpacaError> {
        self.slew_to_alt_az_async(alt, az).await
            .map_err(|e| AlpacaError::OperationFailed(e))?;
        self.wait_for_slew_complete(poll_interval, timeout).await
    }

    /// Destination side of pier for slew to coordinates
    pub async fn destination_side_of_pier(&self, ra: f64, dec: f64) -> Result<PierSide, String> {
        let side: i32 = self.client.get(&format!(
            "destinationsideofpier?RightAscension={}&Declination={}",
            ra, dec
        )).await?;
        Ok(PierSide::from(side))
    }

    // Parallel status methods

    /// Get comprehensive telescope status in a single parallel query
    pub async fn get_status(&self) -> Result<TelescopeStatus, String> {
        let (
            connected,
            right_ascension,
            declination,
            altitude,
            azimuth,
            slewing,
            tracking,
            tracking_rate,
            at_home,
            at_park,
            side_of_pier,
            sidereal_time,
        ) = tokio::join!(
            self.is_connected(),
            self.right_ascension(),
            self.declination(),
            self.altitude(),
            self.azimuth(),
            self.slewing(),
            self.tracking(),
            self.tracking_rate(),
            self.at_home(),
            self.at_park(),
            self.side_of_pier(),
            self.sidereal_time(),
        );

        Ok(TelescopeStatus {
            connected: connected?,
            right_ascension: right_ascension?,
            declination: declination?,
            altitude: altitude?,
            azimuth: azimuth?,
            slewing: slewing?,
            tracking: tracking?,
            tracking_rate: tracking_rate?,
            at_home: at_home?,
            at_park: at_park?,
            side_of_pier: side_of_pier?,
            sidereal_time: sidereal_time?,
        })
    }

    /// Get telescope capabilities in a single parallel query
    pub async fn get_capabilities(&self) -> Result<TelescopeCapabilities, String> {
        let (
            can_find_home,
            can_park,
            can_unpark,
            can_set_park,
            can_slew,
            can_slew_async,
            can_slew_alt_az,
            can_slew_alt_az_async,
            can_sync,
            can_sync_alt_az,
            can_set_tracking,
            can_pulse_guide,
            can_set_guide_rates,
        ) = tokio::join!(
            self.can_find_home(),
            self.can_park(),
            self.can_unpark(),
            self.can_set_park(),
            self.can_slew(),
            self.can_slew_async(),
            self.can_slew_alt_az(),
            self.can_slew_alt_az_async(),
            self.can_sync(),
            self.can_sync_alt_az(),
            self.can_set_tracking(),
            self.can_pulse_guide(),
            self.can_set_guide_rates(),
        );

        Ok(TelescopeCapabilities {
            can_find_home: can_find_home?,
            can_park: can_park?,
            can_unpark: can_unpark?,
            can_set_park: can_set_park?,
            can_slew: can_slew?,
            can_slew_async: can_slew_async?,
            can_slew_alt_az: can_slew_alt_az?,
            can_slew_alt_az_async: can_slew_alt_az_async?,
            can_sync: can_sync?,
            can_sync_alt_az: can_sync_alt_az?,
            can_set_tracking: can_set_tracking?,
            can_pulse_guide: can_pulse_guide?,
            can_set_guide_rates: can_set_guide_rates?,
        })
    }

    /// Get site information in a single parallel query
    pub async fn get_site_info(&self) -> Result<TelescopeSiteInfo, String> {
        let (site_latitude, site_longitude, site_elevation) = tokio::join!(
            self.site_latitude(),
            self.site_longitude(),
            self.site_elevation(),
        );

        Ok(TelescopeSiteInfo {
            site_latitude: site_latitude?,
            site_longitude: site_longitude?,
            site_elevation: site_elevation?,
        })
    }

    /// Get optics information in a single parallel query
    pub async fn get_optics_info(&self) -> Result<TelescopeOpticsInfo, String> {
        let (aperture_area, aperture_diameter, focal_length, alignment_mode) = tokio::join!(
            self.aperture_area(),
            self.aperture_diameter(),
            self.focal_length(),
            self.alignment_mode(),
        );

        Ok(TelescopeOpticsInfo {
            aperture_area: aperture_area.ok(),
            aperture_diameter: aperture_diameter.ok(),
            focal_length: focal_length.ok(),
            alignment_mode: AlignmentMode::from(alignment_mode?),
        })
    }
}
