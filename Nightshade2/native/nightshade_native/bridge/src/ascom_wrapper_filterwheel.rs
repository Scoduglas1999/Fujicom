use nightshade_ascom::{AscomFilterWheel, init_com, uninit_com};
use nightshade_native::traits::{NativeDevice, NativeFilterWheel, NativeError};
use nightshade_native::NativeVendor;
use std::sync::Arc;
use std::thread;
use std::time::Duration;
use tokio::sync::{mpsc, oneshot};
use std::fmt::Debug;
use crate::timeout_ops::Timeouts;

#[derive(Debug)]
enum AscomFilterWheelCommand {
    Connect(oneshot::Sender<Result<(), String>>),
    Disconnect(oneshot::Sender<Result<(), String>>),
    SetPosition(i32, oneshot::Sender<Result<(), String>>),
    GetPosition(oneshot::Sender<Result<i32, String>>),
    GetNames(oneshot::Sender<Result<Vec<String>, String>>),
}

pub struct AscomFilterWheelWrapper {
    id: String,
    name: String,
    sender: mpsc::Sender<AscomFilterWheelCommand>,
    _thread_handle: Arc<thread::JoinHandle<()>>,
    // Cache filter count and names to avoid async calls if possible, 
    // but names might change? Unlikely for ASCOM.
    // NativeFilterWheel::get_filter_count is synchronous.
    filter_count: i32,
}

impl Debug for AscomFilterWheelWrapper {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("AscomFilterWheelWrapper")
            .field("id", &self.id)
            .field("name", &self.name)
            .finish()
    }
}

impl AscomFilterWheelWrapper {
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
            
            let mut fw = match AscomFilterWheel::new(&prog_id_clone) {
                Ok(f) => f,
                Err(e) => {
                    let _ = init_tx.send(Err(format!("Failed to create ASCOM filter wheel: {}", e)));
                    uninit_com();
                    return;
                }
            };
            
            // Fetch filter names to determine count
            let names = fw.names().unwrap_or_default();
            let count = names.len() as i32;
            tracing::info!("ASCOM FilterWheel initialized with {} filters: {:?}", count, names);

            let _ = init_tx.send(Ok(count));
            
            while let Some(cmd) = rx.blocking_recv() {
                match cmd {
                    AscomFilterWheelCommand::Connect(reply) => {
                        let _ = reply.send(fw.connect().map_err(|e| e.to_string()));
                    }
                    AscomFilterWheelCommand::Disconnect(reply) => {
                        let _ = reply.send(fw.disconnect().map_err(|e| e.to_string()));
                    }
                    AscomFilterWheelCommand::SetPosition(pos, reply) => {
                        let _ = reply.send(fw.set_position(pos).map_err(|e| e.to_string()));
                    }
                    AscomFilterWheelCommand::GetPosition(reply) => {
                        let _ = reply.send(fw.position().map_err(|e| e.to_string()));
                    }
                    AscomFilterWheelCommand::GetNames(reply) => {
                        let result = fw.names();
                        match &result {
                            Ok(names) => tracing::info!("ASCOM FilterWheel GetNames returned {} filters: {:?}", names.len(), names),
                            Err(e) => tracing::error!("ASCOM FilterWheel GetNames failed: {}", e),
                        }
                        let _ = reply.send(result.map_err(|e| e.to_string()));
                    }
                }
            }
            
            uninit_com();
        });
        
        // Wait for initialization
        let count = init_rx.recv()
            .map_err(|e| format!("Failed to receive init result: {}", e))??;
            
        Ok(Self {
            id: prog_id.clone(),
            name: prog_id,
            sender: tx,
            _thread_handle: Arc::new(handle),
            filter_count: count,
        })
    }

    /// Helper to receive a response with a timeout
    async fn recv_with_timeout<T>(
        rx: oneshot::Receiver<Result<T, String>>,
        timeout: Duration,
        operation: &str,
    ) -> Result<T, NativeError> {
        match tokio::time::timeout(timeout, rx).await {
            Ok(Ok(result)) => result.map_err(|e| NativeError::SdkError(e)),
            Ok(Err(_recv_err)) => Err(NativeError::Unknown(
                format!("Worker thread dead during {}", operation)
            )),
            Err(_elapsed) => Err(NativeError::Timeout(
                format!("FilterWheel {} timed out after {:?}", operation, timeout)
            )),
        }
    }
}

#[async_trait::async_trait]
impl NativeDevice for AscomFilterWheelWrapper {
    fn id(&self) -> &str {
        &self.id
    }

    fn name(&self) -> &str {
        &self.name
    }

    fn vendor(&self) -> NativeVendor {
        NativeVendor::Ascom
    }

    fn is_connected(&self) -> bool {
        true // Placeholder
    }

    async fn connect(&mut self) -> Result<(), NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomFilterWheelCommand::Connect(tx)).await
            .map_err(|e| NativeError::SdkError(e.to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::connection(), "connect").await
    }

    async fn disconnect(&mut self) -> Result<(), NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomFilterWheelCommand::Disconnect(tx)).await
            .map_err(|e| NativeError::SdkError(e.to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::connection(), "disconnect").await
    }
}

#[async_trait::async_trait]
impl NativeFilterWheel for AscomFilterWheelWrapper {
    async fn move_to_position(&mut self, position: i32) -> Result<(), NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomFilterWheelCommand::SetPosition(position, tx)).await
            .map_err(|e| NativeError::SdkError(e.to_string()))?;
        // Filter wheel rotation can take time
        Self::recv_with_timeout(rx, Timeouts::filter_wheel(), "move_to_position").await
    }

    async fn get_position(&self) -> Result<i32, NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomFilterWheelCommand::GetPosition(tx)).await
            .map_err(|e| NativeError::SdkError(e.to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::property_read(), "get_position").await
    }

    async fn is_moving(&self) -> Result<bool, NativeError> {
        // ASCOM Position returns -1 if moving
        match self.get_position().await {
            Ok(pos) => Ok(pos == -1),
            Err(_) => Ok(false), // Or error?
        }
    }

    fn get_filter_count(&self) -> i32 {
        self.filter_count
    }

    async fn get_filter_names(&self) -> Result<Vec<String>, NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomFilterWheelCommand::GetNames(tx)).await
            .map_err(|e| NativeError::SdkError(e.to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::property_read(), "get_filter_names").await
    }

    async fn set_filter_name(&mut self, _position: i32, _name: String) -> Result<(), NativeError> {
        Err(NativeError::NotSupported)
    }
}
