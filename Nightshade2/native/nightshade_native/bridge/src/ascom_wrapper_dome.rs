use nightshade_ascom::{AscomDome, init_com, uninit_com};
use std::sync::Arc;
use std::thread;
use std::time::Duration;
use tokio::sync::{mpsc, oneshot};
use std::fmt::Debug;
use crate::timeout_ops::Timeouts;

#[derive(Debug)]
enum AscomDomeCommand {
    Connect(oneshot::Sender<Result<(), String>>),
    Disconnect(oneshot::Sender<Result<(), String>>),
    OpenShutter(oneshot::Sender<Result<(), String>>),
    CloseShutter(oneshot::Sender<Result<(), String>>),
    Park(oneshot::Sender<Result<(), String>>),
    GetShutterStatus(oneshot::Sender<Result<i32, String>>),
    GetSlewing(oneshot::Sender<Result<bool, String>>),
    GetAtPark(oneshot::Sender<Result<bool, String>>),
    GetName(oneshot::Sender<Result<String, String>>),
    GetAzimuth(oneshot::Sender<Result<f64, String>>),
    SlewToAzimuth { azimuth: f64, reply: oneshot::Sender<Result<(), String>> },
}

pub struct AscomDomeWrapper {
    id: String,
    name: String,
    sender: mpsc::Sender<AscomDomeCommand>,
    _thread_handle: Arc<thread::JoinHandle<()>>,
}

impl Debug for AscomDomeWrapper {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("AscomDomeWrapper")
            .field("id", &self.id)
            .field("name", &self.name)
            .finish()
    }
}

impl AscomDomeWrapper {
    pub fn new(prog_id: String) -> Result<Self, String> {
        let (tx, mut rx) = mpsc::channel(32);
        let prog_id_clone = prog_id.clone();

        let (init_tx, init_rx) = std::sync::mpsc::channel();

        let handle = thread::spawn(move || {
            // Initialize COM as STA on this thread
            if let Err(e) = init_com() {
                let _ = init_tx.send(Err(format!("Failed to init COM: {}", e)));
                return;
            }

            let mut dome = match AscomDome::new(&prog_id_clone) {
                Ok(d) => d,
                Err(e) => {
                    let _ = init_tx.send(Err(format!("Failed to create ASCOM dome: {}", e)));
                    uninit_com();
                    return;
                }
            };

            // Try to get the device name
            let name = dome.name().unwrap_or_else(|_| prog_id_clone.clone());

            let _ = init_tx.send(Ok(name));

            while let Some(cmd) = rx.blocking_recv() {
                match cmd {
                    AscomDomeCommand::Connect(reply) => {
                        let _ = reply.send(dome.connect());
                    }
                    AscomDomeCommand::Disconnect(reply) => {
                        let _ = reply.send(dome.disconnect());
                    }
                    AscomDomeCommand::OpenShutter(reply) => {
                        let _ = reply.send(dome.open_shutter());
                    }
                    AscomDomeCommand::CloseShutter(reply) => {
                        let _ = reply.send(dome.close_shutter());
                    }
                    AscomDomeCommand::Park(reply) => {
                        let _ = reply.send(dome.park());
                    }
                    AscomDomeCommand::GetShutterStatus(reply) => {
                        let _ = reply.send(dome.shutter_status());
                    }
                    AscomDomeCommand::GetSlewing(reply) => {
                        let _ = reply.send(dome.slewing());
                    }
                    AscomDomeCommand::GetAtPark(reply) => {
                        let _ = reply.send(dome.at_park());
                    }
                    AscomDomeCommand::GetName(reply) => {
                        let _ = reply.send(dome.name());
                    }
                    AscomDomeCommand::GetAzimuth(reply) => {
                        let _ = reply.send(dome.azimuth());
                    }
                    AscomDomeCommand::SlewToAzimuth { azimuth, reply } => {
                        let _ = reply.send(dome.slew_to_azimuth(azimuth));
                    }
                }
            }

            uninit_com();
        });

        // Wait for initialization
        let name = init_rx.recv()
            .map_err(|e| format!("Failed to receive init result: {}", e))??;

        Ok(Self {
            id: prog_id.clone(),
            name,
            sender: tx,
            _thread_handle: Arc::new(handle),
        })
    }

    /// Helper to receive a response with a timeout
    async fn recv_with_timeout<T>(
        rx: oneshot::Receiver<Result<T, String>>,
        timeout: Duration,
        operation: &str,
    ) -> Result<T, String> {
        match tokio::time::timeout(timeout, rx).await {
            Ok(Ok(result)) => result,
            Ok(Err(_recv_err)) => Err(format!("Worker thread dead during {}", operation)),
            Err(_elapsed) => Err(format!("Dome {} timed out after {:?}", operation, timeout)),
        }
    }

    pub async fn connect(&mut self) -> Result<(), String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomDomeCommand::Connect(tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::connection(), "connect").await
    }

    pub async fn disconnect(&mut self) -> Result<(), String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomDomeCommand::Disconnect(tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::connection(), "disconnect").await
    }

    pub async fn open_shutter(&self) -> Result<(), String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomDomeCommand::OpenShutter(tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::dome_shutter(), "open_shutter").await
    }

    pub async fn close_shutter(&self) -> Result<(), String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomDomeCommand::CloseShutter(tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::dome_shutter(), "close_shutter").await
    }

    pub async fn park(&self) -> Result<(), String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomDomeCommand::Park(tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::dome(), "park").await
    }

    pub async fn shutter_status(&self) -> Result<i32, String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomDomeCommand::GetShutterStatus(tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::property_read(), "shutter_status").await
    }

    pub async fn slewing(&self) -> Result<bool, String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomDomeCommand::GetSlewing(tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::property_read(), "slewing").await
    }

    pub async fn at_park(&self) -> Result<bool, String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomDomeCommand::GetAtPark(tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::property_read(), "at_park").await
    }

    pub async fn name(&self) -> Result<String, String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomDomeCommand::GetName(tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::property_read(), "name").await
    }

    pub fn id(&self) -> &str {
        &self.id
    }

    pub fn cached_name(&self) -> &str {
        &self.name
    }

    pub async fn azimuth(&self) -> Result<f64, String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomDomeCommand::GetAzimuth(tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::property_read(), "azimuth").await
    }

    pub async fn slew_to_azimuth(&self, azimuth: f64) -> Result<(), String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomDomeCommand::SlewToAzimuth { azimuth, reply: tx }).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::dome(), "slew_to_azimuth").await
    }
}
