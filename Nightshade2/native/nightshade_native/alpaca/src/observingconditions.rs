//! Alpaca Observing Conditions (Weather Station) API implementation

use crate::{AlpacaClient, AlpacaDevice, AlpacaDeviceType};

/// Alpaca Observing Conditions client
pub struct AlpacaObservingConditions {
    client: AlpacaClient,
}

impl AlpacaObservingConditions {
    /// Create a new Alpaca observing conditions client
    pub fn new(device: &AlpacaDevice) -> Self {
        assert_eq!(device.device_type, AlpacaDeviceType::ObservingConditions);
        Self {
            client: AlpacaClient::new(device),
        }
    }

    /// Create from server details
    pub fn from_server(base_url: &str, device_number: u32) -> Self {
        let device = AlpacaDevice {
            device_type: AlpacaDeviceType::ObservingConditions,
            device_number,
            server_name: String::new(),
            manufacturer: String::new(),
            device_name: String::new(),
            unique_id: String::new(),
            base_url: base_url.to_string(),
        };
        Self::new(&device)
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

    // Observing Conditions information

    pub async fn name(&self) -> Result<String, String> {
        self.client.get_name().await
    }

    pub async fn description(&self) -> Result<String, String> {
        self.client.get_description().await
    }

    // Weather data

    pub async fn cloud_cover(&self) -> Result<f64, String> {
        self.client.get("cloudcover").await
    }

    pub async fn dew_point(&self) -> Result<f64, String> {
        self.client.get("dewpoint").await
    }

    pub async fn humidity(&self) -> Result<f64, String> {
        self.client.get("humidity").await
    }

    pub async fn pressure(&self) -> Result<f64, String> {
        self.client.get("pressure").await
    }

    pub async fn rain_rate(&self) -> Result<f64, String> {
        self.client.get("rainrate").await
    }

    pub async fn sky_brightness(&self) -> Result<f64, String> {
        self.client.get("skybrightness").await
    }

    pub async fn sky_quality(&self) -> Result<f64, String> {
        self.client.get("skyquality").await
    }

    pub async fn sky_temperature(&self) -> Result<f64, String> {
        self.client.get("skytemperature").await
    }

    pub async fn temperature(&self) -> Result<f64, String> {
        self.client.get("temperature").await
    }

    pub async fn wind_direction(&self) -> Result<f64, String> {
        self.client.get("winddirection").await
    }

    pub async fn wind_gust(&self) -> Result<f64, String> {
        self.client.get("windgust").await
    }

    pub async fn wind_speed(&self) -> Result<f64, String> {
        self.client.get("windspeed").await
    }

    // Sensor timing

    pub async fn average_period(&self) -> Result<f64, String> {
        self.client.get("averageperiod").await
    }

    pub async fn set_average_period(&self, period: f64) -> Result<(), String> {
        self.client.put("averageperiod", &[("AveragePeriod", &period.to_string())]).await
    }

    pub async fn time_since_last_update(&self, sensor: &str) -> Result<f64, String> {
        self.client.get(&format!("timesincelastupdate?SensorName={}", sensor)).await
    }

    // Control

    pub async fn refresh(&self) -> Result<(), String> {
        self.client.put("refresh", &[]).await
    }
}
