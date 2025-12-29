use std::sync::Arc;
use std::thread;
use std::time::Duration;
use tokio::sync::{mpsc, oneshot};
use nightshade_native::traits::{NativeDevice, NativeMount, NativeError, GuideDirection, TrackingRate};
use nightshade_native::NativeVendor;
use nightshade_ascom::{AscomMount, init_com, uninit_com};
use std::fmt::Debug;
use crate::timeout_ops::Timeouts;

/// Command sent to the ASCOM worker thread
enum AscomMountCommand {
    Connect(oneshot::Sender<Result<(), String>>),
    Disconnect(oneshot::Sender<Result<(), String>>),
    SlewToCoordinates(f64, f64, oneshot::Sender<Result<(), String>>),
    SyncToCoordinates(f64, f64, oneshot::Sender<Result<(), String>>),
    Park(oneshot::Sender<Result<(), String>>),
    Unpark(oneshot::Sender<Result<(), String>>),
    GetCoordinates(oneshot::Sender<Result<(f64, f64), String>>),
    IsSlewing(oneshot::Sender<Result<bool, String>>),
    IsParked(oneshot::Sender<Result<bool, String>>),
    CanPark(oneshot::Sender<Result<bool, String>>),
    Stop(oneshot::Sender<()>),
    AbortSlew(oneshot::Sender<Result<(), String>>),
    SetTracking(bool, oneshot::Sender<Result<(), String>>),
    GetTracking(oneshot::Sender<Result<bool, String>>),
    PulseGuide(GuideDirection, u32, oneshot::Sender<Result<(), String>>),
    GetSideOfPier(oneshot::Sender<Result<nightshade_native::traits::PierSide, String>>),
    GetAltAz(oneshot::Sender<Result<(f64, f64), String>>),
    GetSiderealTime(oneshot::Sender<Result<f64, String>>),
    // Tracking rate commands
    SetTrackingRate(i32, oneshot::Sender<Result<(), String>>),
    GetTrackingRate(oneshot::Sender<Result<i32, String>>),
    // Axis movement commands (for jogging)
    MoveAxis(i32, f64, oneshot::Sender<Result<(), String>>),
}

/// Wrapper for ASCOM Mount that runs on a dedicated thread to support STA and Send/Sync
#[derive(Debug)]
pub struct AscomMountWrapper {
    id: String,
    name: String,
    sender: mpsc::Sender<AscomMountCommand>,
    _thread_handle: Arc<thread::JoinHandle<()>>,
}

impl AscomMountWrapper {
    pub fn new(prog_id: String) -> Result<Self, String> {
        let (tx, mut rx) = mpsc::channel(32);
        let prog_id_clone = prog_id.clone();
        
        let handle = thread::spawn(move || {
            // Initialize COM as STA on this thread
            if let Err(e) = init_com() {
                tracing::error!("Failed to init COM on ASCOM thread: {}", e);
                return;
            }
            
            let mut mount: Option<AscomMount> = None;
            
            // Try to create the mount object immediately
            match AscomMount::new(&prog_id_clone) {
                Ok(m) => mount = Some(m),
                Err(e) => tracing::error!("Failed to create ASCOM mount {}: {}", prog_id_clone, e),
            }
            
            while let Some(cmd) = rx.blocking_recv() {
                match cmd {
                    AscomMountCommand::Connect(reply) => {
                        if let Some(m) = &mut mount {
                            let _ = reply.send(m.connect().map_err(|e| e.to_string()));
                        } else {
                            let _ = reply.send(Err("Mount not created".to_string()));
                        }
                    }
                    AscomMountCommand::Disconnect(reply) => {
                        if let Some(m) = &mut mount {
                            let _ = reply.send(m.disconnect().map_err(|e| e.to_string()));
                        } else {
                            let _ = reply.send(Err("Mount not created".to_string()));
                        }
                    }
                    AscomMountCommand::SlewToCoordinates(ra, dec, reply) => {
                        if let Some(m) = &mut mount {
                            let _ = reply.send(m.slew_to_coordinates_async(ra, dec).map_err(|e| e.to_string()));
                        } else {
                            let _ = reply.send(Err("Mount not created".to_string()));
                        }
                    }
                    AscomMountCommand::SyncToCoordinates(ra, dec, reply) => {
                        if let Some(m) = &mut mount {
                            match m.can_sync() {
                                Ok(true) => {
                                    let _ = reply.send(m.sync_to_coordinates(ra, dec).map_err(|e| e.to_string()));
                                }
                                Ok(false) => {
                                    let _ = reply.send(Err("Mount does not support Sync".to_string()));
                                }
                                Err(e) => {
                                    let _ = reply.send(Err(format!("Failed to check CanSync: {}", e)));
                                }
                            }
                        } else {
                            let _ = reply.send(Err("Mount not created".to_string()));
                        }
                    }
                    AscomMountCommand::Park(reply) => {
                        if let Some(m) = &mut mount {
                            match m.can_park() {
                                Ok(true) => {
                                    let _ = reply.send(m.park().map_err(|e| e.to_string()));
                                }
                                Ok(false) => {
                                    let _ = reply.send(Err("Mount does not support Park".to_string()));
                                }
                                Err(e) => {
                                    let _ = reply.send(Err(format!("Failed to check CanPark: {}", e)));
                                }
                            }
                        } else {
                            let _ = reply.send(Err("Mount not created".to_string()));
                        }
                    }
                    AscomMountCommand::Unpark(reply) => {
                        if let Some(m) = &mut mount {
                            let _ = reply.send(m.unpark().map_err(|e| e.to_string()));
                        } else {
                            let _ = reply.send(Err("Mount not created".to_string()));
                        }
                    }
                    AscomMountCommand::GetCoordinates(reply) => {
                        if let Some(m) = &mut mount {
                            let ra_res = m.right_ascension();
                            let dec_res = m.declination();
                            match (ra_res, dec_res) {
                                (Ok(ra), Ok(dec)) => {
                                    let _ = reply.send(Ok((ra, dec)));
                                }
                                (Err(e), _) => {
                                    let _ = reply.send(Err(format!("Failed to get RA: {}", e)));
                                }
                                (_, Err(e)) => {
                                    let _ = reply.send(Err(format!("Failed to get DEC: {}", e)));
                                }
                            }
                        } else {
                            let _ = reply.send(Err("Mount not created".to_string()));
                        }
                    }
                    AscomMountCommand::IsSlewing(reply) => {
                        if let Some(m) = &mut mount {
                            let _ = reply.send(m.slewing().map_err(|e| e.to_string()));
                        } else {
                            let _ = reply.send(Err("Mount not created".to_string()));
                        }
                    }
                    AscomMountCommand::IsParked(reply) => {
                        if let Some(m) = &mut mount {
                            let _ = reply.send(m.at_park().map_err(|e| e.to_string()));
                        } else {
                            let _ = reply.send(Err("Mount not created".to_string()));
                        }
                    }
                    AscomMountCommand::CanPark(reply) => {
                         if let Some(m) = &mut mount {
                            let _ = reply.send(m.can_park().map_err(|e| e.to_string()));
                        } else {
                            let _ = reply.send(Err("Mount not created".to_string()));
                        }
                    }
                    AscomMountCommand::AbortSlew(reply) => {
                        if let Some(m) = &mut mount {
                            let _ = reply.send(m.abort_slew().map_err(|e| e.to_string()));
                        } else {
                            let _ = reply.send(Err("Mount not created".to_string()));
                        }
                    }
                    AscomMountCommand::SetTracking(enabled, reply) => {
                        if let Some(m) = &mut mount {
                            let _ = reply.send(m.set_tracking(enabled).map_err(|e| e.to_string()));
                        } else {
                            let _ = reply.send(Err("Mount not created".to_string()));
                        }
                    }
                    AscomMountCommand::GetTracking(reply) => {
                        if let Some(m) = &mut mount {
                            let _ = reply.send(m.tracking().map_err(|e| e.to_string()));
                        } else {
                            let _ = reply.send(Err("Mount not created".to_string()));
                        }
                    }
                    AscomMountCommand::PulseGuide(dir, duration, reply) => {
                        if let Some(m) = &mut mount {
                            // Map GuideDirection to ASCOM direction (0=N, 1=S, 2=E, 3=W)
                            let d = match dir {
                                GuideDirection::North => 0,
                                GuideDirection::South => 1,
                                GuideDirection::East => 2,
                                GuideDirection::West => 3,
                            };
                            let _ = reply.send(m.pulse_guide(d, duration).map_err(|e| e.to_string()));
                        } else {
                            let _ = reply.send(Err("Mount not created".to_string()));
                        }
                    }
                    AscomMountCommand::GetSideOfPier(reply) => {
                        if let Some(m) = &mut mount {
                            match m.side_of_pier() {
                                Ok(0) => { let _ = reply.send(Ok(nightshade_native::traits::PierSide::East)); },
                                Ok(1) => { let _ = reply.send(Ok(nightshade_native::traits::PierSide::West)); },
                                Ok(_) => { let _ = reply.send(Ok(nightshade_native::traits::PierSide::Unknown)); },
                                Err(e) => { let _ = reply.send(Err(e.to_string())); }
                            }
                        } else {
                            let _ = reply.send(Err("Mount not created".to_string()));
                        }
                    }
                    AscomMountCommand::GetAltAz(reply) => {
                        if let Some(m) = &mut mount {
                            let alt_res = m.altitude();
                            let az_res = m.azimuth();
                            match (alt_res, az_res) {
                                (Ok(alt), Ok(az)) => { let _ = reply.send(Ok((alt, az))); },
                                (Err(e), _) => { let _ = reply.send(Err(format!("Failed to get Alt: {}", e))); },
                                (_, Err(e)) => { let _ = reply.send(Err(format!("Failed to get Az: {}", e))); }
                            }
                        } else {
                            let _ = reply.send(Err("Mount not created".to_string()));
                        }
                    }
                    AscomMountCommand::GetSiderealTime(reply) => {
                        if let Some(m) = &mut mount {
                            let _ = reply.send(m.sidereal_time().map_err(|e| e.to_string()));
                        } else {
                            let _ = reply.send(Err("Mount not created".to_string()));
                        }
                    }
                    AscomMountCommand::SetTrackingRate(rate, reply) => {
                        if let Some(m) = &mut mount {
                            let _ = reply.send(m.set_tracking_rate(rate).map_err(|e| e.to_string()));
                        } else {
                            let _ = reply.send(Err("Mount not created".to_string()));
                        }
                    }
                    AscomMountCommand::GetTrackingRate(reply) => {
                        if let Some(m) = &mut mount {
                            let _ = reply.send(m.tracking_rate().map_err(|e| e.to_string()));
                        } else {
                            let _ = reply.send(Err("Mount not created".to_string()));
                        }
                    }
                    AscomMountCommand::MoveAxis(axis, rate, reply) => {
                        if let Some(m) = &mut mount {
                            let _ = reply.send(m.move_axis(axis, rate).map_err(|e| e.to_string()));
                        } else {
                            let _ = reply.send(Err("Mount not created".to_string()));
                        }
                    }
                    AscomMountCommand::Stop(reply) => {
                        uninit_com();
                        let _ = reply.send(());
                        break;
                    }
                }
            }
        });
        
        Ok(Self {
            id: prog_id.clone(),
            name: prog_id,
            sender: tx,
            _thread_handle: Arc::new(handle),
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
                format!("Mount {} timed out after {:?}", operation, timeout)
            )),
        }
    }
}

#[async_trait::async_trait]
impl NativeDevice for AscomMountWrapper {
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
        self.sender.send(AscomMountCommand::Connect(tx)).await
            .map_err(|e| NativeError::SdkError(e.to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::connection(), "connect").await
    }

    async fn disconnect(&mut self) -> Result<(), NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomMountCommand::Disconnect(tx)).await
            .map_err(|e| NativeError::SdkError(e.to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::connection(), "disconnect").await
    }
}

#[async_trait::async_trait]
impl NativeMount for AscomMountWrapper {
    async fn slew_to_coordinates(&mut self, ra: f64, dec: f64) -> Result<(), NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomMountCommand::SlewToCoordinates(ra, dec, tx)).await
            .map_err(|e| NativeError::SdkError(e.to_string()))?;
        // Slews can take a long time
        Self::recv_with_timeout(rx, Timeouts::long_slew(), "slew_to_coordinates").await
    }

    async fn sync_to_coordinates(&mut self, ra: f64, dec: f64) -> Result<(), NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomMountCommand::SyncToCoordinates(ra, dec, tx)).await
            .map_err(|e| NativeError::SdkError(e.to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::property_write(), "sync_to_coordinates").await
    }

    async fn park(&mut self) -> Result<(), NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomMountCommand::Park(tx)).await
            .map_err(|e| NativeError::SdkError(e.to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::park(), "park").await
    }

    async fn unpark(&mut self) -> Result<(), NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomMountCommand::Unpark(tx)).await
            .map_err(|e| NativeError::SdkError(e.to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::park(), "unpark").await
    }

    async fn get_coordinates(&self) -> Result<(f64, f64), NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomMountCommand::GetCoordinates(tx)).await
            .map_err(|e| NativeError::SdkError(e.to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::property_read(), "get_coordinates").await
    }

    async fn is_slewing(&self) -> Result<bool, NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomMountCommand::IsSlewing(tx)).await
            .map_err(|e| NativeError::SdkError(e.to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::property_read(), "is_slewing").await
    }

    async fn is_parked(&self) -> Result<bool, NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomMountCommand::IsParked(tx)).await
            .map_err(|e| NativeError::SdkError(e.to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::property_read(), "is_parked").await
    }

    async fn pulse_guide(&mut self, direction: GuideDirection, duration_ms: u32) -> Result<(), NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomMountCommand::PulseGuide(direction, duration_ms, tx)).await
            .map_err(|e| NativeError::SdkError(e.to_string()))?;
        // Pulse guide takes the duration plus a buffer
        let timeout = Duration::from_millis(duration_ms as u64) + Timeouts::short_slew();
        Self::recv_with_timeout(rx, timeout, "pulse_guide").await
    }

    async fn abort_slew(&mut self) -> Result<(), NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomMountCommand::AbortSlew(tx)).await
            .map_err(|e| NativeError::SdkError(e.to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::property_write(), "abort_slew").await
    }

    async fn set_tracking(&mut self, enabled: bool) -> Result<(), NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomMountCommand::SetTracking(enabled, tx)).await
            .map_err(|e| NativeError::SdkError(e.to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::property_write(), "set_tracking").await
    }

    async fn get_tracking(&self) -> Result<bool, NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomMountCommand::GetTracking(tx)).await
            .map_err(|e| NativeError::SdkError(e.to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::property_read(), "get_tracking").await
    }

    async fn get_side_of_pier(&self) -> Result<nightshade_native::traits::PierSide, NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomMountCommand::GetSideOfPier(tx)).await
            .map_err(|e| NativeError::SdkError(e.to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::property_read(), "get_side_of_pier").await
    }

    async fn get_alt_az(&self) -> Result<(f64, f64), NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomMountCommand::GetAltAz(tx)).await
            .map_err(|e| NativeError::SdkError(e.to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::property_read(), "get_alt_az").await
    }

    async fn get_sidereal_time(&self) -> Result<f64, NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomMountCommand::GetSiderealTime(tx)).await
            .map_err(|e| NativeError::SdkError(e.to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::property_read(), "get_sidereal_time").await
    }

    async fn set_tracking_rate(&mut self, rate: TrackingRate) -> Result<(), NativeError> {
        let rate_int = match rate {
            TrackingRate::Sidereal => 0,
            TrackingRate::Lunar => 1,
            TrackingRate::Solar => 2,
            TrackingRate::King => 3,
            TrackingRate::Custom => return Err(NativeError::NotSupported),
        };
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomMountCommand::SetTrackingRate(rate_int, tx)).await
            .map_err(|e| NativeError::SdkError(e.to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::property_write(), "set_tracking_rate").await
    }

    async fn get_tracking_rate(&self) -> Result<TrackingRate, NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomMountCommand::GetTrackingRate(tx)).await
            .map_err(|e| NativeError::SdkError(e.to_string()))?;
        let rate_int = Self::recv_with_timeout(rx, Timeouts::property_read(), "get_tracking_rate").await?;
        match rate_int {
            0 => Ok(TrackingRate::Sidereal),
            1 => Ok(TrackingRate::Lunar),
            2 => Ok(TrackingRate::Solar),
            3 => Ok(TrackingRate::King),
            _ => Ok(TrackingRate::Custom),
        }
    }

    fn can_set_tracking_rate(&self) -> bool {
        true // ASCOM mounts generally support tracking rate
    }
}

// Additional mount control methods (not in NativeMount trait)
impl AscomMountWrapper {
    /// Set the tracking rate (0=Sidereal, 1=Lunar, 2=Solar, 3=King)
    pub async fn set_tracking_rate_raw(&mut self, rate: i32) -> Result<(), NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomMountCommand::SetTrackingRate(rate, tx)).await
            .map_err(|e| NativeError::SdkError(e.to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::property_write(), "set_tracking_rate").await
    }

    /// Get the current tracking rate (0=Sidereal, 1=Lunar, 2=Solar, 3=King)
    pub async fn get_tracking_rate_raw(&self) -> Result<i32, NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomMountCommand::GetTrackingRate(tx)).await
            .map_err(|e| NativeError::SdkError(e.to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::property_read(), "get_tracking_rate").await
    }

    /// Move an axis at the specified rate (degrees/second)
    /// axis: 0=RA/Azimuth (primary), 1=Dec/Altitude (secondary)
    /// rate: degrees per second (positive = N/E, negative = S/W), 0 to stop
    pub async fn move_axis(&mut self, axis: i32, rate: f64) -> Result<(), NativeError> {
        let (tx, rx) = oneshot::channel();
        self.sender.send(AscomMountCommand::MoveAxis(axis, rate, tx)).await
            .map_err(|e| NativeError::SdkError(e.to_string()))?;
        Self::recv_with_timeout(rx, Timeouts::short_slew(), "move_axis").await
    }
}

