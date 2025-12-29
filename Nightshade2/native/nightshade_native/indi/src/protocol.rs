//! INDI XML Protocol definitions

/// INDI protocol version
pub const INDI_PROTOCOL_VERSION: &str = "1.7";

/// CCD frame type
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum CcdFrameType {
    Light,
    Bias,
    Dark,
    Flat,
}

/// Standard INDI properties
pub mod standard_properties {
    /// Connection control switch
    pub const CONNECTION: &str = "CONNECTION";
    pub const CONNECT: &str = "CONNECT";
    pub const DISCONNECT: &str = "DISCONNECT";

    /// Device port
    pub const DEVICE_PORT: &str = "DEVICE_PORT";

    // Camera properties
    pub const CCD_EXPOSURE: &str = "CCD_EXPOSURE";
    pub const CCD_ABORT_EXPOSURE: &str = "CCD_ABORT_EXPOSURE";
    pub const CCD_FRAME: &str = "CCD_FRAME";
    pub const CCD_FRAME_TYPE: &str = "CCD_FRAME_TYPE";
    pub const CCD_BINNING: &str = "CCD_BINNING";
    pub const CCD_TEMPERATURE: &str = "CCD_TEMPERATURE";
    pub const CCD_COOLER: &str = "CCD_COOLER";
    pub const CCD_COOLER_POWER: &str = "CCD_COOLER_POWER";
    pub const CCD_GAIN: &str = "CCD_GAIN";
    pub const CCD_OFFSET: &str = "CCD_OFFSET";
    pub const CCD_INFO: &str = "CCD_INFO";
    pub const CCD1: &str = "CCD1";  // BLOB property for image data

    // Mount properties
    pub const EQUATORIAL_EOD_COORD: &str = "EQUATORIAL_EOD_COORD";
    pub const EQUATORIAL_COORD: &str = "EQUATORIAL_COORD";
    pub const HORIZONTAL_COORD: &str = "HORIZONTAL_COORD";
    pub const ON_COORD_SET: &str = "ON_COORD_SET";
    pub const TELESCOPE_TRACK_STATE: &str = "TELESCOPE_TRACK_STATE";
    pub const TELESCOPE_TRACK_RATE: &str = "TELESCOPE_TRACK_RATE";
    pub const TELESCOPE_PARK: &str = "TELESCOPE_PARK";
    pub const TELESCOPE_ABORT_MOTION: &str = "TELESCOPE_ABORT_MOTION";
    pub const TELESCOPE_MOTION_NS: &str = "TELESCOPE_MOTION_NS";
    pub const TELESCOPE_MOTION_WE: &str = "TELESCOPE_MOTION_WE";
    pub const TELESCOPE_SLEW_RATE: &str = "TELESCOPE_SLEW_RATE";

    // Focuser properties
    pub const FOCUS_MOTION: &str = "FOCUS_MOTION";
    pub const FOCUS_SPEED: &str = "FOCUS_SPEED";
    pub const FOCUS_TIMER: &str = "FOCUS_TIMER";
    pub const ABS_FOCUS_POSITION: &str = "ABS_FOCUS_POSITION";
    pub const REL_FOCUS_POSITION: &str = "REL_FOCUS_POSITION";
    pub const FOCUS_ABORT_MOTION: &str = "FOCUS_ABORT_MOTION";
    pub const FOCUS_TEMPERATURE: &str = "FOCUS_TEMPERATURE";

    // Filter wheel properties
    pub const FILTER_SLOT: &str = "FILTER_SLOT";
    pub const FILTER_NAME: &str = "FILTER_NAME";

    // Dome properties
    pub const DOME_SHUTTER: &str = "DOME_SHUTTER";
    pub const DOME_MOTION: &str = "DOME_MOTION";
    pub const ABS_DOME_POSITION: &str = "ABS_DOME_POSITION";
    pub const DOME_ABORT_MOTION: &str = "DOME_ABORT_MOTION";

    // Weather properties
    pub const WEATHER_STATUS: &str = "WEATHER_STATUS";
    pub const WEATHER_TEMPERATURE: &str = "WEATHER_TEMPERATURE";
    pub const WEATHER_HUMIDITY: &str = "WEATHER_HUMIDITY";
    pub const WEATHER_PRESSURE: &str = "WEATHER_PRESSURE";
    pub const WEATHER_WIND_SPEED: &str = "WEATHER_WIND_SPEED";
}

/// Common coordinate elements
pub mod coord_elements {
    pub const RA: &str = "RA";
    pub const DEC: &str = "DEC";
    pub const ALT: &str = "ALT";
    pub const AZ: &str = "AZ";
}





