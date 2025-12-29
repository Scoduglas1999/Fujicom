use nightshade_ascom::{AscomCoverCalibrator, init_com, uninit_com};
use std::sync::Arc;
use std::thread;
use std::time::Duration;
use tokio::sync::{mpsc, oneshot};
use std::fmt::Debug;
use crate::timeout_ops::Timeouts;

#[derive(Debug)]
enum AscomCoverCalibratorCommand {
    Connect(oneshot::Sender<Result<(), String>>),
    Disconnect(oneshot::Sender<Result<(), String>>),
    OpenCover(oneshot::Sender<Result<(), String>>),
    CloseCover(oneshot::Sender<Result<(), String>>),
    HaltCover(oneshot::Sender<Result<(), String>>),
    CalibratorOn(i32, oneshot::Sender<Result<(), String>>),
    CalibratorOff(oneshot::Sender<Result<(), String>>),
    GetCoverState(oneshot::Sender<Result<i32, String>>),
    GetCalibratorState(oneshot::Sender<Result<i32, String>>),
    GetBrightness(oneshot::Sender<Result<i32, String>>),
    SetBrightness(i32, oneshot::Sender<Result<(), String>>),
    GetMaxBrightness(oneshot::Sender<Result<i32, String>>),
    GetName(oneshot::Sender<Result<String, String>>),
}

pub struct AscomCoverCalibratorWrapper {
    id: String,
    name: String,
    sender: mpsc::Sender<AscomCoverCalibratorCommand>,
    _thread_handle: Arc<thread::JoinHandle<()>>,
    max_brightness: i32,
}

impl Debug for AscomCoverCalibratorWrapper {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("AscomCoverCalibratorWrapper")
            .field("id", &self.id)
            .field("name", &self.name)
            .field("max_brightness", &self.max_brightness)
            .finish()
    }
}

impl AscomCoverCalibratorWrapper {
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

            let mut cover_cal = match AscomCoverCalibrator::new(&prog_id_clone) {
                Ok(cc) => cc,
                Err(e) => {
                    let _ = init_tx.send(Err(format!("Failed to create ASCOM cover calibrator: {}", e)));
                    uninit_com();
                    return;
                }
            };

            // Get static properties
            let name = cover_cal.name().unwrap_or_else(|_| prog_id_clone.clone());
            let max_brightness = cover_cal.max_brightness().unwrap_or(100);

            let _ = init_tx.send(Ok((name, max_brightness)));

            while let Some(cmd) = rx.blocking_recv() {
                match cmd {
                    AscomCoverCalibratorCommand::Connect(reply) => {
                        let _ = reply.send(cover_cal.connect());
                    }
                    AscomCoverCalibratorCommand::Disconnect(reply) => {
                        let _ = reply.send(cover_cal.disconnect());
                    }
                    AscomCoverCalibratorCommand::OpenCover(reply) => {
                        let _ = reply.send(cover_cal.open_cover());
                    }
                    AscomCoverCalibratorCommand::CloseCover(reply) => {
                        let _ = reply.send(cover_cal.close_cover());
                    }
                    AscomCoverCalibratorCommand::HaltCover(reply) => {
                        let _ = reply.send(cover_cal.halt_cover());
                    }
                    AscomCoverCalibratorCommand::CalibratorOn(brightness, reply) => {
                        let _ = reply.send(cover_cal.calibrator_on(brightness));
                    }
                    AscomCoverCalibratorCommand::CalibratorOff(reply) => {
                        let _ = reply.send(cover_cal.calibrator_off());
                    }
                    AscomCoverCalibratorCommand::GetCoverState(reply) => {
                        let _ = reply.send(cover_cal.cover_state());
                    }
                    AscomCoverCalibratorCommand::GetCalibratorState(reply) => {
                        let _ = reply.send(cover_cal.calibrator_state());
                    }
                    AscomCoverCalibratorCommand::GetBrightness(reply) => {
                        let _ = reply.send(cover_cal.brightness());
                    }
                    AscomCoverCalibratorCommand::SetBrightness(brightness, reply) => {
                        let _ = reply.send(cover_cal.set_brightness(brightness));
                    }
                    AscomCoverCalibratorCommand::GetMaxBrightness(reply) => {
                        let _ = reply.send(cover_cal.max_brightness());
                    }
                    AscomCoverCalibratorCommand::GetName(reply) => {
                        let _ = reply.send(cover_cal.name());
                    }
                }
            }

            uninit_com();
        });

        // Wait for initialization
        let (name, max_brightness) = init_rx.recv()
            .map_err(|e| format!("Failed to receive init result: {}", e))??;

        Ok(Self {
            id: prog_id.clone(),
            name,
            sender: tx,
            _thread_handle: Arc::new(handle),
            max_brightness,
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
            Err(_elapsed) => Err(format!("CoverCalibrator {} timed out after {:?}", operation, timeout)),
        }
    }

    pub async fn connect(&mut self) -> Result<(), String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomCoverCalibratorCommand::Connect(tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::connection(), "connect").await
    }

    pub async fn disconnect(&mut self) -> Result<(), String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomCoverCalibratorCommand::Disconnect(tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::connection(), "disconnect").await
    }

    pub async fn open_cover(&mut self) -> Result<(), String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomCoverCalibratorCommand::OpenCover(tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::cover_calibrator(), "open_cover").await
    }

    pub async fn close_cover(&mut self) -> Result<(), String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomCoverCalibratorCommand::CloseCover(tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::cover_calibrator(), "close_cover").await
    }

    pub async fn halt_cover(&mut self) -> Result<(), String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomCoverCalibratorCommand::HaltCover(tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::property_write(), "halt_cover").await
    }

    pub async fn calibrator_on(&mut self, brightness: i32) -> Result<(), String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomCoverCalibratorCommand::CalibratorOn(brightness, tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::property_write(), "calibrator_on").await
    }

    pub async fn calibrator_off(&mut self) -> Result<(), String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomCoverCalibratorCommand::CalibratorOff(tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::property_write(), "calibrator_off").await
    }

    pub async fn cover_state(&self) -> Result<i32, String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomCoverCalibratorCommand::GetCoverState(tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::property_read(), "cover_state").await
    }

    pub async fn calibrator_state(&self) -> Result<i32, String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomCoverCalibratorCommand::GetCalibratorState(tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::property_read(), "calibrator_state").await
    }

    pub async fn brightness(&self) -> Result<i32, String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomCoverCalibratorCommand::GetBrightness(tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::property_read(), "brightness").await
    }

    pub async fn set_brightness(&mut self, brightness: i32) -> Result<(), String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomCoverCalibratorCommand::SetBrightness(brightness, tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::property_write(), "set_brightness").await
    }

    pub async fn max_brightness(&self) -> Result<i32, String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomCoverCalibratorCommand::GetMaxBrightness(tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::property_read(), "max_brightness").await
    }

    pub async fn name(&self) -> Result<String, String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomCoverCalibratorCommand::GetName(tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::property_read(), "name").await
    }

    pub fn id(&self) -> &str {
        &self.id
    }

    pub fn cached_name(&self) -> &str {
        &self.name
    }

    pub fn cached_max_brightness(&self) -> i32 {
        self.max_brightness
    }
}
