//! Alpaca Switch API implementation

use crate::{AlpacaClient, AlpacaDevice, AlpacaDeviceType};

/// Alpaca Switch client
pub struct AlpacaSwitch {
    client: AlpacaClient,
}

impl AlpacaSwitch {
    /// Create a new Alpaca switch client
    pub fn new(device: &AlpacaDevice) -> Self {
        assert_eq!(device.device_type, AlpacaDeviceType::Switch);
        Self {
            client: AlpacaClient::new(device),
        }
    }

    /// Create from server details
    pub fn from_server(base_url: &str, device_number: u32) -> Self {
        let device = AlpacaDevice {
            device_type: AlpacaDeviceType::Switch,
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

    // Switch information

    pub async fn name(&self) -> Result<String, String> {
        self.client.get_name().await
    }

    pub async fn description(&self) -> Result<String, String> {
        self.client.get_description().await
    }

    pub async fn max_switch(&self) -> Result<i32, String> {
        self.client.get("maxswitch").await
    }

    // Switch operations

    pub async fn get_switch(&self, id: i32) -> Result<bool, String> {
        self.client.get(&format!("getswitch?Id={}", id)).await
    }

    pub async fn set_switch(&self, id: i32, state: bool) -> Result<(), String> {
        self.client.put("setswitch", &[
            ("Id", &id.to_string()),
            ("State", &state.to_string()),
        ]).await
    }

    pub async fn get_switch_name(&self, id: i32) -> Result<String, String> {
        self.client.get(&format!("getswitchname?Id={}", id)).await
    }

    pub async fn set_switch_name(&self, id: i32, name: &str) -> Result<(), String> {
        self.client.put("setswitchname", &[
            ("Id", &id.to_string()),
            ("Name", name),
        ]).await
    }

    pub async fn get_switch_description(&self, id: i32) -> Result<String, String> {
        self.client.get(&format!("getswitchdescription?Id={}", id)).await
    }

    pub async fn get_switch_value(&self, id: i32) -> Result<f64, String> {
        self.client.get(&format!("getswitchvalue?Id={}", id)).await
    }

    pub async fn set_switch_value(&self, id: i32, value: f64) -> Result<(), String> {
        self.client.put("setswitchvalue", &[
            ("Id", &id.to_string()),
            ("Value", &value.to_string()),
        ]).await
    }

    pub async fn min_switch_value(&self, id: i32) -> Result<f64, String> {
        self.client.get(&format!("minswitchvalue?Id={}", id)).await
    }

    pub async fn max_switch_value(&self, id: i32) -> Result<f64, String> {
        self.client.get(&format!("maxswitchvalue?Id={}", id)).await
    }

    pub async fn can_write(&self, id: i32) -> Result<bool, String> {
        self.client.get(&format!("canwrite?Id={}", id)).await
    }
}
