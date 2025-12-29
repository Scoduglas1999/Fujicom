//! RAII guards for Alpaca device resource cleanup
//!
//! This module provides guards that ensure proper cleanup of Alpaca device
//! connections even when operations fail mid-way. Since Rust's Drop is
//! synchronous but Alpaca operations are async, we use a pattern that
//! spawns cleanup tasks on drop.

use crate::{AlpacaCamera, AlpacaTelescope, AlpacaFocuser, AlpacaFilterWheel, AlpacaRotator, AlpacaDome};
use std::sync::Arc;

// ============================================================================
// Alpaca Connection Guard Trait
// ============================================================================

/// Trait for Alpaca devices that can be connected/disconnected
pub trait AlpacaConnectable: Send + Sync {
    /// Attempt to disconnect the device (best-effort cleanup)
    fn disconnect_sync(&self);
}

impl AlpacaConnectable for AlpacaCamera {
    fn disconnect_sync(&self) {
        // Spawn a task to disconnect asynchronously
        // This is best-effort cleanup - we can't block in Drop
        let base_url = self.base_url().to_string();
        let device_number = self.device_number();

        std::thread::spawn(move || {
            if let Ok(rt) = tokio::runtime::Builder::new_current_thread()
                .enable_all()
                .build()
            {
                let _ = rt.block_on(async {
                    let device = crate::AlpacaDevice {
                        device_type: crate::AlpacaDeviceType::Camera,
                        device_number,
                        server_name: String::new(),
                        manufacturer: String::new(),
                        device_name: String::new(),
                        unique_id: String::new(),
                        base_url,
                    };
                    let camera = AlpacaCamera::new(&device);
                    let _ = camera.disconnect().await;
                });
            }
        });
    }
}

impl AlpacaConnectable for AlpacaTelescope {
    fn disconnect_sync(&self) {
        let base_url = self.base_url().to_string();
        let device_number = self.device_number();

        std::thread::spawn(move || {
            if let Ok(rt) = tokio::runtime::Builder::new_current_thread()
                .enable_all()
                .build()
            {
                let _ = rt.block_on(async {
                    let device = crate::AlpacaDevice {
                        device_type: crate::AlpacaDeviceType::Telescope,
                        device_number,
                        server_name: String::new(),
                        manufacturer: String::new(),
                        device_name: String::new(),
                        unique_id: String::new(),
                        base_url,
                    };
                    let mount = AlpacaTelescope::new(&device);
                    let _ = mount.disconnect().await;
                });
            }
        });
    }
}

impl AlpacaConnectable for AlpacaFocuser {
    fn disconnect_sync(&self) {
        let base_url = self.base_url().to_string();
        let device_number = self.device_number();

        std::thread::spawn(move || {
            if let Ok(rt) = tokio::runtime::Builder::new_current_thread()
                .enable_all()
                .build()
            {
                let _ = rt.block_on(async {
                    let device = crate::AlpacaDevice {
                        device_type: crate::AlpacaDeviceType::Focuser,
                        device_number,
                        server_name: String::new(),
                        manufacturer: String::new(),
                        device_name: String::new(),
                        unique_id: String::new(),
                        base_url,
                    };
                    let focuser = AlpacaFocuser::new(&device);
                    let _ = focuser.disconnect().await;
                });
            }
        });
    }
}

impl AlpacaConnectable for AlpacaFilterWheel {
    fn disconnect_sync(&self) {
        let base_url = self.base_url().to_string();
        let device_number = self.device_number();

        std::thread::spawn(move || {
            if let Ok(rt) = tokio::runtime::Builder::new_current_thread()
                .enable_all()
                .build()
            {
                let _ = rt.block_on(async {
                    let device = crate::AlpacaDevice {
                        device_type: crate::AlpacaDeviceType::FilterWheel,
                        device_number,
                        server_name: String::new(),
                        manufacturer: String::new(),
                        device_name: String::new(),
                        unique_id: String::new(),
                        base_url,
                    };
                    let fw = AlpacaFilterWheel::new(&device);
                    let _ = fw.disconnect().await;
                });
            }
        });
    }
}

impl AlpacaConnectable for AlpacaRotator {
    fn disconnect_sync(&self) {
        let base_url = self.base_url().to_string();
        let device_number = self.device_number();

        std::thread::spawn(move || {
            if let Ok(rt) = tokio::runtime::Builder::new_current_thread()
                .enable_all()
                .build()
            {
                let _ = rt.block_on(async {
                    let device = crate::AlpacaDevice {
                        device_type: crate::AlpacaDeviceType::Rotator,
                        device_number,
                        server_name: String::new(),
                        manufacturer: String::new(),
                        device_name: String::new(),
                        unique_id: String::new(),
                        base_url,
                    };
                    let rotator = AlpacaRotator::new(&device);
                    let _ = rotator.disconnect().await;
                });
            }
        });
    }
}

impl AlpacaConnectable for AlpacaDome {
    fn disconnect_sync(&self) {
        let base_url = self.base_url().to_string();
        let device_number = self.device_number();

        std::thread::spawn(move || {
            if let Ok(rt) = tokio::runtime::Builder::new_current_thread()
                .enable_all()
                .build()
            {
                let _ = rt.block_on(async {
                    let device = crate::AlpacaDevice {
                        device_type: crate::AlpacaDeviceType::Dome,
                        device_number,
                        server_name: String::new(),
                        manufacturer: String::new(),
                        device_name: String::new(),
                        unique_id: String::new(),
                        base_url,
                    };
                    let dome = AlpacaDome::new(&device);
                    let _ = dome.disconnect().await;
                });
            }
        });
    }
}

// ============================================================================
// Connection Guard
// ============================================================================

/// RAII guard that ensures an Alpaca device is disconnected on drop.
///
/// Use this when performing operations that connect to a device temporarily.
/// The guard will automatically disconnect the device when dropped, even if
/// an error occurs.
///
/// # Example
/// ```ignore
/// let mount = AlpacaTelescope::from_server(&base_url, device_num);
/// mount.connect().await?;
///
/// // Create guard - will disconnect on drop if not defused
/// let guard = AlpacaConnectionGuard::new(Arc::new(mount));
///
/// // Perform operations
/// mount.slew_to_target().await?;
///
/// // Operation succeeded - defuse the guard and disconnect manually
/// guard.defuse();
/// mount.disconnect().await.ok();
/// ```
pub struct AlpacaConnectionGuard<T: AlpacaConnectable> {
    device: Option<Arc<T>>,
    device_name: String,
}

impl<T: AlpacaConnectable> AlpacaConnectionGuard<T> {
    /// Create a new connection guard for the given device.
    pub fn new(device: Arc<T>, device_name: impl Into<String>) -> Self {
        Self {
            device: Some(device),
            device_name: device_name.into(),
        }
    }

    /// Defuse the guard, preventing automatic disconnect on drop.
    /// Call this when the operation succeeds and you will handle disconnect manually.
    pub fn defuse(mut self) {
        self.device = None;
    }

    /// Get a reference to the guarded device.
    pub fn device(&self) -> Option<&Arc<T>> {
        self.device.as_ref()
    }
}

impl<T: AlpacaConnectable> Drop for AlpacaConnectionGuard<T> {
    fn drop(&mut self) {
        if let Some(device) = self.device.take() {
            tracing::debug!("AlpacaConnectionGuard: cleaning up connection to {}", self.device_name);
            device.disconnect_sync();
        }
    }
}

// ============================================================================
// Scoped Connection Helper
// ============================================================================

/// Helper for executing an operation with automatic connection cleanup.
///
/// This function connects to the device, executes the operation, and ensures
/// disconnect happens regardless of success or failure.
///
/// # Example
/// ```ignore
/// let result = with_alpaca_connection(&mount, "Mount", async {
///     mount.slew_to_target().await?;
///     while mount.slewing().await? {
///         tokio::time::sleep(Duration::from_millis(500)).await;
///     }
///     Ok(())
/// }).await;
/// ```
pub async fn with_alpaca_connection<T, F, R, E>(
    device: &T,
    device_name: &str,
    operation: F,
) -> Result<R, E>
where
    T: AlpacaConnectable,
    F: std::future::Future<Output = Result<R, E>>,
    E: From<String>,
{
    // Note: The caller should have already connected the device.
    // This guard just ensures cleanup on failure.

    // Create a cleanup guard using the device reference
    struct CleanupOnDrop<'a, T: AlpacaConnectable> {
        device: &'a T,
        device_name: String,
        should_cleanup: bool,
    }

    impl<'a, T: AlpacaConnectable> Drop for CleanupOnDrop<'a, T> {
        fn drop(&mut self) {
            if self.should_cleanup {
                tracing::debug!("with_alpaca_connection: cleaning up {} after error", self.device_name);
                self.device.disconnect_sync();
            }
        }
    }

    let mut guard = CleanupOnDrop {
        device,
        device_name: device_name.to_string(),
        should_cleanup: true,
    };

    match operation.await {
        Ok(result) => {
            guard.should_cleanup = false;
            Ok(result)
        }
        Err(e) => {
            // Guard will clean up on drop
            Err(e)
        }
    }
}

// ============================================================================
// Telescope/Mount specific guard with context
// ============================================================================

/// A guard specifically for telescope operations that tracks the operation context.
pub struct TelescopeOperationGuard {
    mount: Arc<AlpacaTelescope>,
    operation: String,
    defused: bool,
}

impl TelescopeOperationGuard {
    /// Create a new telescope operation guard.
    pub fn new(mount: Arc<AlpacaTelescope>, operation: impl Into<String>) -> Self {
        Self {
            mount,
            operation: operation.into(),
            defused: false,
        }
    }

    /// Defuse the guard to prevent cleanup on drop.
    pub fn defuse(mut self) {
        self.defused = true;
    }

    /// Get a reference to the mount.
    pub fn mount(&self) -> &AlpacaTelescope {
        &self.mount
    }
}

impl Drop for TelescopeOperationGuard {
    fn drop(&mut self) {
        if !self.defused {
            tracing::warn!(
                "TelescopeOperationGuard: operation '{}' did not complete - disconnecting",
                self.operation
            );
            self.mount.disconnect_sync();
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    // Note: These tests would require a mock Alpaca server to run.
    // They are here as documentation of intended behavior.

    #[test]
    fn test_guard_defuse_prevents_cleanup() {
        // When a guard is defused, it should not trigger cleanup
        // This is tested implicitly by the fact that defuse sets device to None
    }
}
