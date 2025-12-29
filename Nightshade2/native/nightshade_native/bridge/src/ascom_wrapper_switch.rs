use nightshade_ascom::{AscomSwitch, init_com, uninit_com};
use std::sync::Arc;
use std::thread;
use std::time::Duration;
use tokio::sync::{mpsc, oneshot};
use std::fmt::Debug;
use crate::timeout_ops::Timeouts;

#[derive(Debug)]
enum AscomSwitchCommand {
    Connect(oneshot::Sender<Result<(), String>>),
    Disconnect(oneshot::Sender<Result<(), String>>),
    GetMaxSwitch(oneshot::Sender<Result<i32, String>>),
    GetSwitch(i32, oneshot::Sender<Result<bool, String>>),
    SetSwitch(i32, bool, oneshot::Sender<Result<(), String>>),
    GetName(oneshot::Sender<Result<String, String>>),
}

pub struct AscomSwitchWrapper {
    id: String,
    name: String,
    sender: mpsc::Sender<AscomSwitchCommand>,
    _thread_handle: Arc<thread::JoinHandle<()>>,
    max_switch: i32,
}

impl Debug for AscomSwitchWrapper {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("AscomSwitchWrapper")
            .field("id", &self.id)
            .field("name", &self.name)
            .field("max_switch", &self.max_switch)
            .finish()
    }
}

impl AscomSwitchWrapper {
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

            let mut switch = match AscomSwitch::new(&prog_id_clone) {
                Ok(s) => s,
                Err(e) => {
                    let _ = init_tx.send(Err(format!("Failed to create ASCOM switch: {}", e)));
                    uninit_com();
                    return;
                }
            };

            // Get static properties
            let name = switch.name().unwrap_or_else(|_| prog_id_clone.clone());
            let max_switch = switch.max_switch().unwrap_or(0);

            let _ = init_tx.send(Ok((name, max_switch)));

            while let Some(cmd) = rx.blocking_recv() {
                match cmd {
                    AscomSwitchCommand::Connect(reply) => {
                        let _ = reply.send(switch.connect());
                    }
                    AscomSwitchCommand::Disconnect(reply) => {
                        let _ = reply.send(switch.disconnect());
                    }
                    AscomSwitchCommand::GetMaxSwitch(reply) => {
                        let _ = reply.send(switch.max_switch());
                    }
                    AscomSwitchCommand::GetSwitch(id, reply) => {
                        let _ = reply.send(switch.get_switch(id));
                    }
                    AscomSwitchCommand::SetSwitch(id, state, reply) => {
                        let _ = reply.send(switch.set_switch(id, state));
                    }
                    AscomSwitchCommand::GetName(reply) => {
                        let _ = reply.send(switch.name());
                    }
                }
            }

            uninit_com();
        });

        // Wait for initialization
        let (name, max_switch) = init_rx.recv()
            .map_err(|e| format!("Failed to receive init result: {}", e))??;

        Ok(Self {
            id: prog_id.clone(),
            name,
            sender: tx,
            _thread_handle: Arc::new(handle),
            max_switch,
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
            Err(_elapsed) => Err(format!("Switch {} timed out after {:?}", operation, timeout)),
        }
    }

    pub async fn connect(&mut self) -> Result<(), String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomSwitchCommand::Connect(tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::connection(), "connect").await
    }

    pub async fn disconnect(&mut self) -> Result<(), String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomSwitchCommand::Disconnect(tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::connection(), "disconnect").await
    }

    pub async fn get_max_switch(&self) -> Result<i32, String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomSwitchCommand::GetMaxSwitch(tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::property_read(), "get_max_switch").await
    }

    pub async fn get_switch(&self, id: i32) -> Result<bool, String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomSwitchCommand::GetSwitch(id, tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::property_read(), "get_switch").await
    }

    pub async fn set_switch(&mut self, id: i32, state: bool) -> Result<(), String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomSwitchCommand::SetSwitch(id, state, tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::property_write(), "set_switch").await
    }

    pub async fn name(&self) -> Result<String, String> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomSwitchCommand::GetName(tx)).await
            .map_err(|e| format!("Send error: {}", e))?;
        Self::recv_with_timeout(rx, Timeouts::property_read(), "name").await
    }

    pub fn id(&self) -> &str {
        &self.id
    }

    pub fn cached_name(&self) -> &str {
        &self.name
    }

    pub fn cached_max_switch(&self) -> i32 {
        self.max_switch
    }
}
