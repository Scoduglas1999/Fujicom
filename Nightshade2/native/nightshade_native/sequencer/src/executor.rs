//! Sequence execution engine

use crate::{NodeId, NodeStatus, NodeType, SequenceDefinition, NodeDefinition, RecoveryAction};
use crate::node::{Node, RuntimeNode, ExecutionContext, ProgressUpdate};
use crate::device_ops::SharedDeviceOps;
use crate::triggers::{TriggerManager, TriggerState};
use std::collections::HashMap;
use std::sync::Arc;
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::RwLock as StdRwLock;
use tokio::sync::{mpsc, RwLock, broadcast};
use serde::{Deserialize, Serialize};

/// Commands that can be sent to the executor
#[derive(Debug, Clone)]
pub enum ExecutorCommand {
    Start,
    Pause,
    Resume,
    Stop,
    Skip,
    SkipToNode(NodeId),
}

/// State of the sequence executor
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
pub enum ExecutorState {
    Idle,
    Running,
    Paused,
    Stopping,
    Completed,
    Failed,
}

/// Progress information for the sequence
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SequenceProgress {
    pub state: ExecutorState,
    pub current_node_id: Option<NodeId>,
    pub current_node_name: Option<String>,
    pub current_node_status: Option<NodeStatus>,
    pub total_exposures: u32,
    pub completed_exposures: u32,
    pub total_integration_secs: f64,
    pub completed_integration_secs: f64,
    pub elapsed_secs: f64,
    pub estimated_remaining_secs: Option<f64>,
    pub current_target: Option<String>,
    pub current_filter: Option<String>,
    pub message: Option<String>,
    pub node_statuses: HashMap<NodeId, NodeStatus>,
}

impl Default for SequenceProgress {
    fn default() -> Self {
        Self {
            state: ExecutorState::Idle,
            current_node_id: None,
            current_node_name: None,
            current_node_status: None,
            total_exposures: 0,
            completed_exposures: 0,
            total_integration_secs: 0.0,
            completed_integration_secs: 0.0,
            elapsed_secs: 0.0,
            estimated_remaining_secs: None,
            current_target: None,
            current_filter: None,
            message: None,
            node_statuses: HashMap::new(),
        }
    }
}

/// Event emitted by the executor
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum ExecutorEvent {
    StateChanged(ExecutorState),
    ProgressUpdated(SequenceProgress),
    NodeStarted { id: NodeId, name: String },
    NodeCompleted { id: NodeId, status: NodeStatus },
    NodeProgress { node_id: NodeId, instruction: String, progress_percent: f64, detail: String },
    ExposureStarted { frame: u32, total: u32, filter: Option<String> },
    ExposureCompleted { frame: u32, total: u32 },
    TargetStarted { name: String, ra: f64, dec: f64 },
    TargetCompleted { name: String },
    TriggerFired { trigger_id: String, trigger_name: String, action: String },
    Error { message: String },
    SequenceCompleted,
    SequenceFailed { error: String },
}

/// The sequence executor manages running a sequence
pub struct SequenceExecutor {
    sequence: Option<SequenceDefinition>,
    state: Arc<RwLock<ExecutorState>>,
    progress: Arc<StdRwLock<SequenceProgress>>,
    command_tx: Option<mpsc::Sender<ExecutorCommand>>,
    event_tx: broadcast::Sender<ExecutorEvent>,
    is_cancelled: Arc<AtomicBool>,
    root_node: Option<Box<dyn Node>>,
    /// Device operations handler - None indicates no device ops have been configured.
    /// Device ops MUST be set via set_device_ops() before starting a sequence.
    device_ops: Option<SharedDeviceOps>,
    /// Connected device IDs
    pub camera_id: Option<String>,
    pub mount_id: Option<String>,
    pub focuser_id: Option<String>,
    pub filterwheel_id: Option<String>,
    pub rotator_id: Option<String>,
    pub dome_id: Option<String>,
    pub cover_calibrator_id: Option<String>,
    /// Base save path for images
    pub save_path: Option<std::path::PathBuf>,
    /// Observer location
    pub latitude: Option<f64>,
    pub longitude: Option<f64>,
    /// Trigger manager for monitoring conditions
    trigger_manager: Arc<RwLock<TriggerManager>>,
    /// Enable/disable trigger monitoring
    pub triggers_enabled: bool,
    /// Checkpoint manager for crash recovery
    checkpoint_manager: Option<crate::checkpoint::CheckpointManager>,
    /// Current checkpoint being updated
    current_checkpoint: Option<crate::checkpoint::SessionCheckpoint>,
}

impl SequenceExecutor {
    pub fn new() -> Self {
        let (event_tx, _) = broadcast::channel(256);
        let mut trigger_manager = TriggerManager::new();
        trigger_manager.create_standard_triggers(); // Add default triggers
        
        Self {
            sequence: None,
            state: Arc::new(RwLock::new(ExecutorState::Idle)),
            progress: Arc::new(StdRwLock::new(SequenceProgress::default())),
            command_tx: None,
            event_tx,
            is_cancelled: Arc::new(AtomicBool::new(false)),
            root_node: None,
            device_ops: None,
            camera_id: None,
            mount_id: None,
            focuser_id: None,
            filterwheel_id: None,
            rotator_id: None,
            dome_id: None,
            cover_calibrator_id: None,
            save_path: None,
            latitude: None,
            longitude: None,
            trigger_manager: Arc::new(RwLock::new(trigger_manager)),
            triggers_enabled: true,
            checkpoint_manager: None,
            current_checkpoint: None,
        }
    }
    
    /// Get the trigger manager for configuration
    pub fn trigger_manager(&self) -> Arc<RwLock<TriggerManager>> {
        self.trigger_manager.clone()
    }
    
    /// Enable or disable trigger monitoring
    pub fn set_triggers_enabled(&mut self, enabled: bool) {
        self.triggers_enabled = enabled;
    }
    
    /// Update trigger state with current readings
    pub async fn update_trigger_state<F>(&self, updater: F) 
    where 
        F: FnOnce(&mut TriggerState)
    {
        let manager = self.trigger_manager.read().await;
        let state_lock = manager.state();
        let mut state = state_lock.write().await;
        updater(&mut state);
    }
    
    /// Set the device operations handler
    /// This MUST be called before starting a sequence, otherwise start() will return an error.
    pub fn set_device_ops(&mut self, ops: SharedDeviceOps) {
        self.device_ops = Some(ops);
    }

    /// Check if device operations have been configured
    pub fn has_device_ops(&self) -> bool {
        self.device_ops.is_some()
    }
    
    /// Set connected device IDs
    pub fn set_devices(&mut self,
        camera: Option<String>,
        mount: Option<String>,
        focuser: Option<String>,
        filterwheel: Option<String>,
        rotator: Option<String>,
    ) {
        self.camera_id = camera;
        self.mount_id = mount;
        self.focuser_id = focuser;
        self.filterwheel_id = filterwheel;
        self.rotator_id = rotator;
    }

    /// Set dome device ID
    pub fn set_dome(&mut self, dome_id: Option<String>) {
        self.dome_id = dome_id;
    }

    /// Set cover calibrator (flat panel) device ID
    pub fn set_cover_calibrator(&mut self, cover_calibrator_id: Option<String>) {
        self.cover_calibrator_id = cover_calibrator_id;
    }
    
    /// Set save path for images
    pub fn set_save_path(&mut self, path: Option<std::path::PathBuf>) {
        self.save_path = path;
    }
    
    /// Set observer location
    pub fn set_location(&mut self, lat: Option<f64>, lon: Option<f64>) {
        self.latitude = lat;
        self.longitude = lon;
    }

    /// Subscribe to executor events
    pub fn subscribe(&self) -> broadcast::Receiver<ExecutorEvent> {
        self.event_tx.subscribe()
    }

    /// Load a sequence definition and build the node tree
    pub fn load_sequence(&mut self, sequence: SequenceDefinition) -> Result<(), String> {
        // Build node tree from definition
        let root_node = self.build_node_tree(&sequence)?;

        // Calculate totals
        let (total_exposures, total_integration) = self.calculate_totals(&sequence);

        {
            let mut progress = self.progress.write().unwrap();
            progress.total_exposures = total_exposures;
            progress.total_integration_secs = total_integration;
        }

        self.sequence = Some(sequence);
        self.root_node = Some(root_node);

        Ok(())
    }

    /// Build the node tree from the sequence definition
    fn build_node_tree(&self, sequence: &SequenceDefinition) -> Result<Box<dyn Node>, String> {
        // Create a map of nodes by ID
        let node_map: HashMap<&str, &NodeDefinition> = sequence.nodes
            .iter()
            .map(|n| (n.id.as_str(), n))
            .collect();

        // Find root node
        let root_id = sequence.root_node_id.as_ref()
            .ok_or("No root node specified")?;
        
        let root_def = node_map.get(root_id.as_str())
            .ok_or_else(|| format!("Root node {} not found", root_id))?;

        // Recursively build the tree
        fn build_node(
            def: &NodeDefinition,
            node_map: &HashMap<&str, &NodeDefinition>,
        ) -> Box<dyn Node> {
            let mut node = RuntimeNode::from_definition(def.clone());

            tracing::debug!("Building node '{}' (id={}) with {} children defined: {:?}",
                def.name, def.id, def.children.len(), def.children);

            // Add children recursively
            for child_id in &def.children {
                if let Some(child_def) = node_map.get(child_id.as_str()) {
                    tracing::debug!("  Adding child '{}' (id={}) to '{}'",
                        child_def.name, child_def.id, def.name);
                    let child = build_node(child_def, node_map);
                    node.add_child(child);
                } else {
                    tracing::warn!("  Child node '{}' not found in node_map for parent '{}'",
                        child_id, def.name);
                }
            }

            Box::new(node)
        }

        Ok(build_node(root_def, &node_map))
    }

    /// Calculate total exposures and integration time
    fn calculate_totals(&self, sequence: &SequenceDefinition) -> (u32, f64) {
        let mut total_exposures = 0u32;
        let mut total_integration = 0.0f64;

        for node in &sequence.nodes {
            if let NodeType::TakeExposure(config) = &node.node_type {
                if node.enabled {
                    total_exposures += config.count;
                    total_integration += config.count as f64 * config.duration_secs;
                }
            }
        }

        (total_exposures, total_integration)
    }

    /// Get the current state
    pub async fn get_state(&self) -> ExecutorState {
        *self.state.read().await
    }

    /// Get the current progress
    pub fn get_progress(&self) -> SequenceProgress {
        self.progress.read().unwrap().clone()
    }

    /// Emit an event
    fn emit(&self, event: ExecutorEvent) {
        let _ = self.event_tx.send(event);
    }

    /// Set state and emit event
    async fn set_state(&self, state: ExecutorState) {
        *self.state.write().await = state;
        {
            let mut progress = self.progress.write().unwrap();
            progress.state = state;
        }
        self.emit(ExecutorEvent::StateChanged(state));
    }

    /// Start executing the sequence
    pub async fn start(&mut self) -> Result<(), String> {
        let state = self.get_state().await;
        if state != ExecutorState::Idle {
            return Err(format!("Cannot start: executor is {:?}", state));
        }

        if self.sequence.is_none() || self.root_node.is_none() {
            return Err("No sequence loaded".to_string());
        }

        // Check that device operations have been configured
        // This is a critical check - without device ops, all operations will silently do nothing
        let device_ops = self.device_ops.clone().ok_or_else(|| {
            "No device operations configured. Call set_device_ops() before starting a sequence. \
             This ensures all device operations use real hardware instead of silently doing nothing."
                .to_string()
        })?;

        // Reset cancellation flag
        self.is_cancelled.store(false, Ordering::Relaxed);

        // Create command channel
        let (tx, mut rx) = mpsc::channel::<ExecutorCommand>(32);
        self.command_tx = Some(tx);

        // Update state
        self.set_state(ExecutorState::Running).await;

        // Get references for the async task
        let state = self.state.clone();
        let progress = self.progress.clone();
        let event_tx = self.event_tx.clone();
        let is_cancelled = self.is_cancelled.clone();
        let mut root_node = self.root_node.take()
            .ok_or("No root node available - sequence may not be properly loaded".to_string())?;

        // device_ops already cloned and validated above
        let camera_id = self.camera_id.clone();
        let mount_id = self.mount_id.clone();
        let focuser_id = self.focuser_id.clone();
        let filterwheel_id = self.filterwheel_id.clone();
        let rotator_id = self.rotator_id.clone();
        let dome_id = self.dome_id.clone();
        let cover_calibrator_id = self.cover_calibrator_id.clone();
        let save_path = self.save_path.clone();
        let latitude = self.latitude;
        let longitude = self.longitude;
        
        // Clone trigger manager for the async task
        let trigger_manager = self.trigger_manager.clone();
        let triggers_enabled = self.triggers_enabled;

        // Create shared pause state for context
        let is_paused = Arc::new(AtomicBool::new(false));
        let resume_notify = Arc::new(tokio::sync::Notify::new());

        // Spawn execution task
        let is_paused_clone = is_paused.clone();
        let resume_notify_clone = resume_notify.clone();
        tokio::spawn(async move {
            let start_time = std::time::Instant::now();

            // Clone device_ops for trigger monitoring before passing to context
            let device_ops_for_triggers = device_ops.clone();

            // Set up execution context with device ops and IDs
            let mut context = ExecutionContext::new("root".to_string())
                .with_device_ops(device_ops);
            context.is_cancelled = is_cancelled.clone();
            context.is_paused = is_paused_clone;
            context.resume_notify = resume_notify_clone;
            context.camera_id = camera_id;
            context.mount_id = mount_id;
            context.focuser_id = focuser_id;
            context.filterwheel_id = filterwheel_id;
            context.rotator_id = rotator_id;
            context.dome_id = dome_id;
            context.cover_calibrator_id = cover_calibrator_id;
            context.save_path = save_path;
            context.latitude = latitude;
            context.longitude = longitude;
            // Set trigger state for HFR tracking and exposure counts
            context.trigger_state = Some(trigger_manager.read().await.state());
            
            // Set up progress callback
            let progress_clone = progress.clone();
            let event_tx_clone = event_tx.clone();
            // Track nodes that have already had NodeStarted emitted (thread-safe)
            let started_nodes = Arc::new(std::sync::RwLock::new(std::collections::HashSet::<NodeId>::new()));
            context.progress_callback = Some(Box::new(move |update: ProgressUpdate| {
                let mut prog = progress_clone.write().unwrap();
                prog.current_node_id = Some(update.node_id.clone());
                prog.current_node_status = Some(update.status);
                prog.message = update.message.clone();
                prog.node_statuses.insert(update.node_id.clone(), update.status);
                prog.elapsed_secs = start_time.elapsed().as_secs_f64();

                // Emit NodeStarted event when a node transitions to Running
                if update.status == NodeStatus::Running {
                    let mut started = started_nodes.write().unwrap();
                    if !started.contains(&update.node_id) {
                        started.insert(update.node_id.clone());
                        // Extract node name from message (format: "Executing: <name>" or "Step X/Y: <name>")
                        let node_name = update.message.as_ref()
                            .and_then(|m| {
                                if let Some(name) = m.strip_prefix("Executing: ") {
                                    Some(name.to_string())
                                } else if let Some(rest) = m.split_once(": ").map(|(_, rest)| rest) {
                                    Some(rest.to_string())
                                } else {
                                    Some(m.clone())
                                }
                            })
                            .unwrap_or_else(|| "Unknown".to_string());
                        tracing::info!("[PROGRESS_CB] Emitting NodeStarted: id={}, name={}", update.node_id, node_name);
                        let _ = event_tx_clone.send(ExecutorEvent::NodeStarted {
                            id: update.node_id.clone(),
                            name: node_name,
                        });
                    }
                }

                if let (Some(current), Some(_total)) = (update.current_frame, update.total_frames) {
                    prog.completed_exposures = current;
                }

                // Track completed integration time
                if let Some(exposure_secs) = update.completed_exposure_secs {
                    prog.completed_integration_secs += exposure_secs;
                }

                // Emit NodeProgress event for instruction-specific progress
                // Parse messages like "Autofocus: Moving to start position: 25000 (5%)"
                if let Some(ref message) = update.message {
                    tracing::debug!("[PROGRESS_CB] Received message: {}", message);
                    if let Some((instruction, rest)) = message.split_once(':') {
                        tracing::debug!("[PROGRESS_CB] Parsed instruction='{}', rest='{}'", instruction, rest);
                        // Look for percentage in parentheses at the end
                        if let Some(pct_start) = rest.rfind('(') {
                            if let Some(pct_end) = rest[pct_start..].find(')') {
                                let pct_str = &rest[pct_start + 1..pct_start + pct_end];
                                tracing::debug!("[PROGRESS_CB] pct_str='{}'", pct_str);
                                if let Some(pct_val) = pct_str.strip_suffix('%') {
                                    if let Ok(progress_percent) = pct_val.trim().parse::<f64>() {
                                        let detail = rest[..pct_start].trim().to_string();
                                        tracing::info!("[PROGRESS_CB] Emitting NodeProgress: node_id={}, instruction={}, progress={}%, detail={}",
                                            update.node_id, instruction.trim(), progress_percent, detail);
                                        let _ = event_tx_clone.send(ExecutorEvent::NodeProgress {
                                            node_id: update.node_id.clone(),
                                            instruction: instruction.trim().to_string(),
                                            progress_percent,
                                            detail,
                                        });
                                    } else {
                                        tracing::debug!("[PROGRESS_CB] Failed to parse pct_val='{}' as f64", pct_val);
                                    }
                                } else {
                                    tracing::debug!("[PROGRESS_CB] pct_str '{}' doesn't end with '%'", pct_str);
                                }
                            } else {
                                tracing::debug!("[PROGRESS_CB] No ')' found after '(' in rest");
                            }
                        } else {
                            tracing::debug!("[PROGRESS_CB] No '(' found in rest: '{}'", rest);
                        }
                    } else {
                        tracing::debug!("[PROGRESS_CB] No ':' found in message");
                    }
                } else {
                    tracing::debug!("[PROGRESS_CB] No message in ProgressUpdate");
                }

                let _ = event_tx_clone.send(ExecutorEvent::ProgressUpdated(prog.clone()));
            }));

            // Handle commands during execution
            let is_paused_cmd = is_paused.clone();
            let resume_notify_cmd = resume_notify.clone();
            let command_handler = async {
                while let Some(cmd) = rx.recv().await {
                    match cmd {
                        ExecutorCommand::Pause => {
                            is_paused_cmd.store(true, Ordering::Relaxed);
                            *state.write().await = ExecutorState::Paused;
                            let _ = event_tx.send(ExecutorEvent::StateChanged(ExecutorState::Paused));
                        }
                        ExecutorCommand::Resume => {
                            // Signal context to resume if it's waiting
                            is_paused_cmd.store(false, Ordering::Relaxed);
                            resume_notify_cmd.notify_waiters();
                            *state.write().await = ExecutorState::Running;
                            let _ = event_tx.send(ExecutorEvent::StateChanged(ExecutorState::Running));
                        }
                        ExecutorCommand::Stop => {
                            is_cancelled.store(true, Ordering::Relaxed);
                            *state.write().await = ExecutorState::Stopping;
                            let _ = event_tx.send(ExecutorEvent::StateChanged(ExecutorState::Stopping));
                            break;
                        }
                        ExecutorCommand::Skip => {
                            // Signal skip by emitting a skip event
                            // The current node should check for this and abort gracefully
                            tracing::info!("Skip requested - aborting current node");
                            let _ = event_tx.send(ExecutorEvent::NodeCompleted { 
                                id: "current".to_string(), 
                                status: NodeStatus::Skipped 
                            });
                        }
                        _ => {}
                    }
                }
            };

            // Clone device IDs for trigger monitor BEFORE execution borrow
            let mount_id_for_triggers = context.mount_id.clone();
            let camera_id_for_triggers = context.camera_id.clone();
            let dome_id_for_triggers = context.dome_id.clone();

            // Execute the sequence
            let execution = async {
                root_node.execute(&mut context).await
            };

            // Trigger monitoring loop
            let state_clone = state.clone();
            let event_tx_clone2 = event_tx.clone();
            let is_cancelled_clone = is_cancelled.clone();
            let trigger_monitor = async {
                if !triggers_enabled {
                    // If triggers disabled, just wait forever (let other tasks complete)
                    std::future::pending::<()>().await;
                    return Vec::new();
                }

                let mut check_interval = tokio::time::interval(std::time::Duration::from_secs(1));
                let mut fired_triggers: Vec<(String, RecoveryAction)> = Vec::new();

                loop {
                    check_interval.tick().await;

                    // Only check triggers while running (not paused or stopping)
                    let current_state = *state_clone.read().await;
                    if current_state != ExecutorState::Running {
                        continue;
                    }

                    // Check if cancelled
                    if is_cancelled_clone.load(Ordering::Relaxed) {
                        break;
                    }

                    // Poll weather/safety status and update trigger state
                    let is_safe = device_ops_for_triggers.safety_is_safe(None).await.unwrap_or(true);

                    // Poll guiding status if guiding is enabled
                    let guiding_rms = device_ops_for_triggers.guider_get_status().await
                        .ok()
                        .map(|status| status.rms_total);

                    {
                        let manager = trigger_manager.read().await;
                        let trigger_state = manager.state();
                        let mut state = trigger_state.write().await;
                        state.weather_safe = is_safe;

                        // Update guiding RMS if available
                        if let Some(rms) = guiding_rms {
                            state.update_guiding_rms(rms);
                            tracing::trace!("Updated guiding RMS: {:.2}", rms);
                        }

                        // Update observer location for dawn calculation (and calculate dawn time)
                        if state.observer_latitude.is_none() {
                            if let Some((lat, lon)) = device_ops_for_triggers.get_observer_location() {
                                state.observer_latitude = Some(lat);
                                state.observer_longitude = Some(lon);
                                // Pre-calculate dawn time when location is set
                                state.dawn_time = Some(crate::triggers::calculate_dawn_time(lat, lon));
                                tracing::debug!("Observer location set for dawn trigger: {}, {}", lat, lon);
                            }
                        }
                    }

                    // Poll mount tracking status
                    if let Some(mount_id) = &mount_id_for_triggers {
                        if let Ok(is_tracking) = device_ops_for_triggers.mount_is_tracking(mount_id).await {
                            let manager = trigger_manager.read().await;
                            let trigger_state = manager.state();
                            let mut state = trigger_state.write().await;
                            if state.mount_tracking_expected && !is_tracking {
                                tracing::warn!("Mount tracking lost during exposure!");
                                state.mount_tracking_lost = true;
                            }
                            state.mount_is_tracking = Some(is_tracking);
                        }
                    }

                    // Poll camera temperature
                    if let Some(camera_id) = &camera_id_for_triggers {
                        if let Ok(temp) = device_ops_for_triggers.camera_get_temperature(camera_id).await {
                            let manager = trigger_manager.read().await;
                            let trigger_state = manager.state();
                            let mut state = trigger_state.write().await;
                            state.update_temperature(temp);
                            tracing::trace!("Updated camera temperature: {:.1}°C", temp);
                        }
                    }

                    // Poll dome shutter status
                    if let Some(dome_id) = &dome_id_for_triggers {
                        if let Ok(status) = device_ops_for_triggers.dome_get_shutter_status(dome_id).await {
                            let manager = trigger_manager.read().await;
                            let trigger_state = manager.state();
                            let mut state = trigger_state.write().await;
                            state.update_dome_status(status.clone());
                            if status != "Open" && state.dome_shutter_open_expected {
                                tracing::warn!("Dome shutter not open during sequence: {}", status);
                            }
                        }
                    }

                    // Check all triggers
                    let mut manager = trigger_manager.write().await;
                    let fired = manager.check_all().await;

                    for (trigger_id, action) in fired {
                        // Get trigger name for better logging
                        let trigger_name = manager.get_trigger(&trigger_id)
                            .map(|t| t.name.clone())
                            .unwrap_or_else(|| trigger_id.clone());

                        let action_str = format!("{:?}", action);

                        tracing::warn!("Trigger fired: {} ({}) - action: {:?}", trigger_name, trigger_id, action);

                        // Emit TriggerFired event
                        let _ = event_tx_clone2.send(ExecutorEvent::TriggerFired {
                            trigger_id: trigger_id.clone(),
                            trigger_name: trigger_name.clone(),
                            action: action_str.clone(),
                        });

                        // Handle recovery actions
                        match &action {
                            RecoveryAction::Pause => {
                                *state_clone.write().await = ExecutorState::Paused;
                                let _ = event_tx_clone2.send(ExecutorEvent::StateChanged(ExecutorState::Paused));
                            }
                            RecoveryAction::ParkAndAbort => {
                                // Signal cancellation
                                is_cancelled_clone.store(true, Ordering::Relaxed);
                                fired_triggers.push((trigger_id, action));
                                return fired_triggers;
                            }
                            RecoveryAction::NextTarget => {
                                // Signal to skip to next target
                                let _ = event_tx_clone2.send(ExecutorEvent::NodeCompleted {
                                    id: "current".to_string(),
                                    status: NodeStatus::Skipped
                                });
                            }
                            RecoveryAction::Autofocus => {
                                // Autofocus will be handled by the trigger system
                                // Just log it here
                                tracing::info!("Autofocus trigger action will be executed by sequence");
                            }
                            _ => {}
                        }

                        fired_triggers.push((trigger_id, action));
                    }
                }
                
                fired_triggers
            };

            // Run all concurrently
            let result = tokio::select! {
                _ = command_handler => NodeStatus::Cancelled,
                result = execution => result,
                _triggers = trigger_monitor => NodeStatus::Cancelled,
            };

            // Update final state
            let final_state = match result {
                NodeStatus::Success => ExecutorState::Completed,
                NodeStatus::Cancelled => ExecutorState::Idle,
                _ => ExecutorState::Failed,
            };

            *state.write().await = final_state;
            {
                let mut prog = progress.write().unwrap();
                prog.state = final_state;
                prog.elapsed_secs = start_time.elapsed().as_secs_f64();
            }
            
            match result {
                NodeStatus::Success => {
                    let _ = event_tx.send(ExecutorEvent::SequenceCompleted);
                }
                NodeStatus::Failure => {
                    let _ = event_tx.send(ExecutorEvent::SequenceFailed { error: "Sequence failed".into() });
                }
                _ => {}
            }
            
            let _ = event_tx.send(ExecutorEvent::StateChanged(final_state));
        });

        Ok(())
    }

    /// Pause the sequence
    pub async fn pause(&self) -> Result<(), String> {
        if let Some(tx) = &self.command_tx {
            tx.send(ExecutorCommand::Pause).await.map_err(|e| e.to_string())?;
        }
        Ok(())
    }

    /// Resume the sequence
    pub async fn resume(&self) -> Result<(), String> {
        if let Some(tx) = &self.command_tx {
            tx.send(ExecutorCommand::Resume).await.map_err(|e| e.to_string())?;
        }
        Ok(())
    }

    /// Stop the sequence
    pub async fn stop(&mut self) -> Result<(), String> {
        self.is_cancelled.store(true, Ordering::Relaxed);
        
        if let Some(tx) = &self.command_tx {
            let _ = tx.send(ExecutorCommand::Stop).await;
        }
        
        self.command_tx = None;
        
        // Wait a bit for graceful shutdown
        tokio::time::sleep(std::time::Duration::from_millis(100)).await;
        
        self.set_state(ExecutorState::Idle).await;
        Ok(())
    }

    /// Skip to the next item
    pub async fn skip(&self) -> Result<(), String> {
        if let Some(tx) = &self.command_tx {
            tx.send(ExecutorCommand::Skip).await.map_err(|e| e.to_string())?;
        }
        Ok(())
    }

    /// Reset the executor
    pub async fn reset(&mut self) {
        self.command_tx = None;
        self.is_cancelled.store(false, Ordering::Relaxed);
        *self.state.write().await = ExecutorState::Idle;
        *self.progress.write().unwrap() = SequenceProgress::default();

        if let Some(ref mut node) = self.root_node {
            node.reset();
        }
    }

    // =========================================================================
    // Checkpoint / Crash Recovery
    // =========================================================================

    /// Set the checkpoint directory for crash recovery
    pub fn set_checkpoint_dir<P: AsRef<std::path::Path>>(&mut self, path: P) {
        self.checkpoint_manager = Some(crate::checkpoint::CheckpointManager::new(path));
    }

    /// Check if a recoverable checkpoint exists
    pub fn has_recoverable_checkpoint(&self) -> bool {
        self.checkpoint_manager
            .as_ref()
            .map(|m| m.has_recoverable_checkpoint())
            .unwrap_or(false)
    }

    /// Get info about the current checkpoint
    pub fn get_checkpoint_info(&self) -> Option<crate::checkpoint::CheckpointInfo> {
        self.checkpoint_manager
            .as_ref()
            .and_then(|m| m.get_checkpoint_info().ok().flatten())
    }

    /// Load and prepare to resume from a checkpoint
    pub fn load_checkpoint(&mut self) -> Result<Option<crate::checkpoint::SessionCheckpoint>, String> {
        let checkpoint = self.checkpoint_manager
            .as_ref()
            .ok_or("No checkpoint manager configured")?
            .load()?;

        if let Some(ref cp) = checkpoint {
            // Store checkpoint for resume
            self.current_checkpoint = Some(cp.clone());

            // Restore device IDs
            self.camera_id = cp.camera_id.clone();
            self.mount_id = cp.mount_id.clone();
            self.focuser_id = cp.focuser_id.clone();
            self.filterwheel_id = cp.filterwheel_id.clone();
            self.rotator_id = cp.rotator_id.clone();

            // Restore location
            self.latitude = cp.latitude;
            self.longitude = cp.longitude;

            // Restore save path
            self.save_path = cp.save_path.clone();

            tracing::info!("Loaded checkpoint for sequence: {}", cp.sequence.name);
        }

        Ok(checkpoint)
    }

    /// Save current execution state as a checkpoint
    pub async fn save_checkpoint(&self) -> Result<(), String> {
        let manager = self.checkpoint_manager
            .as_ref()
            .ok_or("No checkpoint manager configured")?;

        let sequence = self.sequence
            .as_ref()
            .ok_or("No sequence loaded")?;

        let progress = self.progress.read().unwrap().clone();
        let state = self.get_state().await;

        let mut checkpoint = crate::checkpoint::SessionCheckpoint::new(sequence.clone());
        checkpoint.node_statuses = progress.node_statuses.clone();
        checkpoint.current_node = progress.current_node_id.clone();
        checkpoint.executor_state = state;
        checkpoint.completed_exposures = progress.completed_exposures;
        checkpoint.completed_integration_secs = progress.completed_integration_secs;
        checkpoint.is_active = state == ExecutorState::Running;

        // Find last completed node
        checkpoint.last_completed_node = progress.node_statuses
            .iter()
            .filter(|(_, status)| matches!(status, NodeStatus::Success))
            .max_by_key(|(id, _)| *id)
            .map(|(id, _)| (*id).clone());

        // Set device info
        checkpoint.set_devices(
            self.camera_id.clone(),
            self.mount_id.clone(),
            self.focuser_id.clone(),
            self.filterwheel_id.clone(),
            self.rotator_id.clone(),
        );
        checkpoint.set_location(self.latitude, self.longitude);
        checkpoint.set_save_path(self.save_path.clone());

        manager.save(&checkpoint)?;

        Ok(())
    }

    /// Clear checkpoint (call when sequence completes normally)
    pub fn clear_checkpoint(&self) -> Result<(), String> {
        if let Some(ref manager) = self.checkpoint_manager {
            manager.clear()?;
        }
        Ok(())
    }

    /// Mark checkpoint as completed (sequence finished gracefully)
    pub fn mark_checkpoint_completed(&self) -> Result<(), String> {
        if let Some(ref manager) = self.checkpoint_manager {
            manager.mark_completed()?;
        }
        Ok(())
    }

    /// Resume sequence from checkpoint
    ///
    /// This loads the sequence from checkpoint and prepares it for resumed execution.
    /// The sequence will skip already-completed nodes.
    pub async fn resume_from_checkpoint(&mut self) -> Result<(), String> {
        let checkpoint = self.load_checkpoint()?
            .ok_or("No checkpoint to resume from")?;

        if !checkpoint.can_resume() {
            return Err("Checkpoint cannot be resumed".to_string());
        }

        // Load the sequence
        self.load_sequence(checkpoint.sequence.clone())?;

        // Restore progress
        {
            let mut progress = self.progress.write().unwrap();
            progress.node_statuses = checkpoint.node_statuses.clone();
            progress.completed_exposures = checkpoint.completed_exposures;
            progress.completed_integration_secs = checkpoint.completed_integration_secs;
        }

        // Mark completed nodes to be skipped
        if let Some(ref mut root) = self.root_node {
            for node_id in checkpoint.get_completed_nodes() {
                root.mark_completed(&node_id);
            }
        }

        tracing::info!(
            "Prepared to resume sequence '{}' from checkpoint ({}  exposures, {:.1} min integration)",
            checkpoint.sequence.name,
            checkpoint.completed_exposures,
            checkpoint.completed_integration_secs / 60.0
        );

        Ok(())
    }
}

impl Default for SequenceExecutor {
    fn default() -> Self {
        Self::new()
    }
}

/// Global executor instance
static EXECUTOR: std::sync::OnceLock<Arc<RwLock<SequenceExecutor>>> = std::sync::OnceLock::new();

/// Get the global executor instance
pub fn get_executor() -> &'static Arc<RwLock<SequenceExecutor>> {
    EXECUTOR.get_or_init(|| Arc::new(RwLock::new(SequenceExecutor::new())))
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::SequenceDefinition;

    #[test]
    fn test_executor_creation() {
        let executor = SequenceExecutor::new();
        assert!(executor.sequence.is_none());
        assert!(executor.root_node.is_none());
    }

    #[test]
    fn test_load_sequence() {
        let mut executor = SequenceExecutor::new();
        let mut sequence = SequenceDefinition::new("Test Sequence".to_string());

        // Add a simple delay node as root
        let node = crate::NodeDefinition {
            id: "root".to_string(),
            name: "Root".to_string(),
            node_type: crate::NodeType::Delay(crate::DelayConfig::default()),
            enabled: true,
            children: vec![],
        };
        sequence.nodes.push(node);
        sequence.root_node_id = Some("root".to_string());

        let result = executor.load_sequence(sequence);
        assert!(result.is_ok(), "Failed to load sequence: {:?}", result.err());
        assert!(executor.sequence.is_some());
    }

    #[test]
    fn test_executor_state_transitions() {
        let executor = SequenceExecutor::new();

        // Use tokio runtime for async tests
        let rt = tokio::runtime::Builder::new_current_thread()
            .enable_all()
            .build()
            .unwrap();

        rt.block_on(async {
            // Initial state should be Idle
            assert_eq!(executor.get_state().await, ExecutorState::Idle);
        });
    }

    #[test]
    fn test_progress_tracking() {
        let executor = SequenceExecutor::new();

        let rt = tokio::runtime::Builder::new_current_thread()
            .enable_all()
            .build()
            .unwrap();

        rt.block_on(async {
            let progress = executor.get_progress().await;
            assert_eq!(progress.completed_exposures, 0);
            assert_eq!(progress.completed_integration_secs, 0.0);
            assert!(progress.current_node_id.is_none());
        });
    }

    #[test]
    fn test_location_configuration() {
        let mut executor = SequenceExecutor::new();

        executor.set_location(Some(45.5), Some(-122.6));

        assert_eq!(executor.latitude, Some(45.5));
        assert_eq!(executor.longitude, Some(-122.6));
    }

    #[test]
    fn test_save_path_configuration() {
        let mut executor = SequenceExecutor::new();

        executor.set_save_path(Some(std::path::PathBuf::from("/tmp/images")));

        assert!(executor.save_path.is_some());
    }

    #[test]
    fn test_get_set_state() {
        let executor = SequenceExecutor::new();

        let rt = tokio::runtime::Builder::new_current_thread()
            .enable_all()
            .build()
            .unwrap();

        rt.block_on(async {
            // Test state transitions
            assert_eq!(executor.get_state().await, ExecutorState::Idle);

            executor.set_state(ExecutorState::Running).await;
            assert_eq!(executor.get_state().await, ExecutorState::Running);

            executor.set_state(ExecutorState::Paused).await;
            assert_eq!(executor.get_state().await, ExecutorState::Paused);

            executor.set_state(ExecutorState::Stopping).await;
            assert_eq!(executor.get_state().await, ExecutorState::Stopping);

            executor.set_state(ExecutorState::Completed).await;
            assert_eq!(executor.get_state().await, ExecutorState::Completed);
        });
    }

    #[test]
    fn test_executor_state_debug() {
        // Test Debug trait (which is derived)
        assert_eq!(format!("{:?}", ExecutorState::Idle), "Idle");
        assert_eq!(format!("{:?}", ExecutorState::Running), "Running");
        assert_eq!(format!("{:?}", ExecutorState::Paused), "Paused");
        assert_eq!(format!("{:?}", ExecutorState::Stopping), "Stopping");
        assert_eq!(format!("{:?}", ExecutorState::Completed), "Completed");
    }

    #[test]
    fn test_node_status_debug() {
        // Test Debug trait (which is derived)
        assert_eq!(format!("{:?}", NodeStatus::Pending), "Pending");
        assert_eq!(format!("{:?}", NodeStatus::Running), "Running");
        assert_eq!(format!("{:?}", NodeStatus::Success), "Success");
        assert_eq!(format!("{:?}", NodeStatus::Failure), "Failure");
        assert_eq!(format!("{:?}", NodeStatus::Skipped), "Skipped");
    }

    #[test]
    fn test_executor_default() {
        let executor = SequenceExecutor::default();
        assert!(executor.sequence.is_none());
    }
}
