//! INDI Autofocus Implementation
//!
//! Provides automated focusing for INDI focusers using V-curve algorithm
//! with HFD/HFR measurement. This module integrates the autofocus engine
//! from the sequencer crate with INDI camera and focuser devices.

use crate::{IndiCamera, IndiFocuser};
use std::sync::Arc;
use std::time::Duration;
use tokio::time::sleep;

/// Configuration for INDI autofocus
#[derive(Debug, Clone)]
pub struct IndiAutofocusConfig {
    /// Autofocus method (VCurve, Quadratic, Hyperbolic)
    pub method: AutofocusMethod,
    /// Step size in focuser units
    pub step_size: i32,
    /// Number of steps to move outward from center (total points = 2*steps_out+1)
    pub steps_out: u32,
    /// Exposure duration in seconds for focus frames
    pub exposure_duration: f64,
    /// Backlash compensation in focuser units
    pub backlash_compensation: i32,
    /// Use temperature prediction for starting position
    pub use_temperature_prediction: bool,
    /// Maximum allowed star count change (0.0 to 1.0, e.g. 0.5 = 50%)
    pub max_star_count_change: Option<f64>,
    /// Outlier rejection sigma threshold (0 = disabled)
    pub outlier_rejection_sigma: f64,
    /// Binning to use for focus frames (1, 2, 3, or 4)
    pub binning: i32,
    /// Timeout for focuser moves in seconds
    pub move_timeout_secs: u64,
    /// Settling time after focuser move in milliseconds
    pub settling_time_ms: u64,
}

impl Default for IndiAutofocusConfig {
    fn default() -> Self {
        Self {
            method: AutofocusMethod::VCurve,
            step_size: 100,
            steps_out: 7,
            exposure_duration: 3.0,
            backlash_compensation: 50,
            use_temperature_prediction: true,
            max_star_count_change: Some(0.5),
            outlier_rejection_sigma: 3.0,
            binning: 1,
            move_timeout_secs: 120,
            settling_time_ms: 500,
        }
    }
}

/// Autofocus method
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum AutofocusMethod {
    /// Simple V-curve (find minimum)
    VCurve,
    /// Parabolic (quadratic) curve fitting
    Quadratic,
    /// Hyperbolic curve fitting
    Hyperbolic,
}

/// Result of an autofocus run
#[derive(Debug, Clone)]
pub struct IndiAutofocusResult {
    pub best_position: i32,
    pub best_hfr: f64,
    pub curve_fit_quality: f64,
    pub method_used: AutofocusMethod,
    pub data_points: Vec<FocusDataPoint>,
    pub temperature_celsius: Option<f64>,
    pub backlash_applied: bool,
    pub success: bool,
    pub error_message: Option<String>,
}

/// A single data point in the autofocus curve
#[derive(Debug, Clone)]
pub struct FocusDataPoint {
    pub position: i32,
    pub hfr: f64,
    pub fwhm: Option<f64>,
    pub star_count: u32,
}

/// INDI Autofocus Engine
pub struct IndiAutofocus {
    camera: Arc<IndiCamera>,
    focuser: Arc<IndiFocuser>,
    config: IndiAutofocusConfig,
}

impl IndiAutofocus {
    /// Create a new INDI autofocus engine
    pub fn new(
        camera: Arc<IndiCamera>,
        focuser: Arc<IndiFocuser>,
        config: IndiAutofocusConfig,
    ) -> Self {
        Self {
            camera,
            focuser,
            config,
        }
    }

    /// Run the autofocus routine
    pub async fn run(&self) -> Result<IndiAutofocusResult, String> {
        tracing::info!(
            "Starting INDI autofocus: {:?} method, {} steps, step size {}",
            self.config.method,
            self.config.steps_out,
            self.config.step_size
        );

        // Check if focuser and camera are connected
        if !self.focuser.is_connected().await {
            return Err("Focuser not connected".to_string());
        }
        if !self.camera.is_connected().await {
            return Err("Camera not connected".to_string());
        }

        // Get current focuser position
        let current_position = self.focuser.get_position().await
            .map_err(|e| format!("Failed to get focuser position: {}", e))?;

        tracing::info!("Current focuser position: {}", current_position);

        // Get current temperature for recording
        let current_temperature = self.focuser.get_temperature().await.ok();
        if let Some(temp) = current_temperature {
            tracing::info!("Focuser temperature: {:.1}°C", temp);
        }

        // Calculate sweep positions
        let positions = self.calculate_positions(current_position);
        let total_points = positions.len();

        tracing::info!(
            "Autofocus sweep: {} positions from {} to {}",
            total_points,
            positions[0],
            positions[total_points - 1]
        );

        // Move to starting position with backlash compensation
        let start_position = positions[0];
        self.move_with_backlash(current_position, start_position).await?;

        // Set camera binning
        if self.config.binning > 1 {
            self.camera.set_binning(self.config.binning, self.config.binning).await
                .map_err(|e| format!("Failed to set binning: {}", e))?;
        }

        // Enable BLOB transfer for camera
        self.camera.enable_blob().await
            .map_err(|e| format!("Failed to enable BLOB transfer: {}", e))?;

        // Collect focus data points
        let mut focus_data_points: Vec<FocusDataPoint> = Vec::with_capacity(total_points);
        let mut reference_star_count: Option<u32> = None;

        for (point_idx, &position) in positions.iter().enumerate() {
            tracing::info!(
                "Focus point {}/{} at position {}",
                point_idx + 1,
                total_points,
                position
            );

            // Move to position
            self.focuser.move_to_with_timeout(
                position,
                Some(Duration::from_secs(self.config.move_timeout_secs))
            ).await?;

            // Wait for settling
            if self.config.settling_time_ms > 0 {
                sleep(Duration::from_millis(self.config.settling_time_ms)).await;
            }

            // Take exposure and measure HFR
            let (hfr, star_count, fwhm) = self.capture_and_measure().await?;

            // Check for dramatic star count changes (clouds, tracking issues)
            if let Some(ref_count) = reference_star_count {
                if let Some(max_change) = self.config.max_star_count_change {
                    let count_change = ((star_count as f64 - ref_count as f64) / ref_count as f64).abs();
                    if count_change > max_change {
                        tracing::warn!(
                            "Star count changed by {:.1}% ({} -> {}), possible clouds or tracking issue",
                            count_change * 100.0,
                            ref_count,
                            star_count
                        );
                    }
                }
            } else {
                reference_star_count = Some(star_count);
            }

            tracing::info!(
                "Position {}: HFR = {:.2}, Stars = {}, FWHM = {:.2}",
                position,
                hfr,
                star_count,
                fwhm.unwrap_or(0.0)
            );

            focus_data_points.push(FocusDataPoint {
                position,
                hfr,
                fwhm,
                star_count,
            });
        }

        // Find best focus using curve fitting
        let (best_position, curve_quality) = self.find_best_focus(&focus_data_points)?;

        let best_hfr = focus_data_points
            .iter()
            .map(|p| p.hfr)
            .min_by(|a, b| a.partial_cmp(b).unwrap_or(std::cmp::Ordering::Equal))
            .unwrap_or(0.0);

        tracing::info!(
            "Autofocus complete: position = {}, HFR = {:.2}, R² = {:.3}",
            best_position,
            best_hfr,
            curve_quality
        );

        // Move to best position with backlash compensation
        let last_position = positions[positions.len() - 1];
        self.move_with_backlash(last_position, best_position).await?;

        Ok(IndiAutofocusResult {
            best_position,
            best_hfr,
            curve_fit_quality: curve_quality,
            method_used: self.config.method,
            data_points: focus_data_points,
            temperature_celsius: current_temperature,
            backlash_applied: self.config.backlash_compensation > 0,
            success: true,
            error_message: None,
        })
    }

    /// Calculate the focus sweep positions
    fn calculate_positions(&self, starting_position: i32) -> Vec<i32> {
        let half_range = (self.config.steps_out as i32) * self.config.step_size;
        let start_pos = starting_position - half_range;
        let total_points = (self.config.steps_out * 2 + 1) as usize;

        (0..total_points)
            .map(|i| start_pos + (i as i32) * self.config.step_size)
            .collect()
    }

    /// Move focuser with backlash compensation
    async fn move_with_backlash(&self, current: i32, target: i32) -> Result<(), String> {
        if self.config.backlash_compensation > 0 && target < current {
            // Moving inward - apply backlash compensation
            let overshoot = target - self.config.backlash_compensation;
            tracing::info!(
                "Applying backlash compensation: {} -> {} -> {}",
                current,
                overshoot,
                target
            );

            // Move to overshoot position
            self.focuser.move_to_with_timeout(
                overshoot,
                Some(Duration::from_secs(self.config.move_timeout_secs))
            ).await?;

            // Wait for settling
            if self.config.settling_time_ms > 0 {
                sleep(Duration::from_millis(self.config.settling_time_ms)).await;
            }

            // Move to final position
            self.focuser.move_to_with_timeout(
                target,
                Some(Duration::from_secs(self.config.move_timeout_secs))
            ).await?;
        } else {
            // Moving outward or no backlash - direct move
            self.focuser.move_to_with_timeout(
                target,
                Some(Duration::from_secs(self.config.move_timeout_secs))
            ).await?;
        }

        Ok(())
    }

    /// Capture an image and measure HFR/FWHM
    async fn capture_and_measure(&self) -> Result<(f64, u32, Option<f64>), String> {
        // Capture image
        let image_data = self.camera.capture_image_with_timeout(
            self.config.exposure_duration,
            Some(Duration::from_secs(
                self.config.exposure_duration as u64 + 60
            ))
        ).await?;

        // Parse FITS data to extract image
        let image = self.parse_fits_image(&image_data)?;

        // Measure HFR and star count
        let (hfr, star_count, fwhm) = self.calculate_hfr_and_stars(&image)?;

        Ok((hfr, star_count, fwhm))
    }

    /// Parse FITS image data
    fn parse_fits_image(&self, data: &[u8]) -> Result<ImageData, String> {
        // Simple FITS parser - extract 16-bit image data
        // FITS header is 2880 bytes (36 x 80-character lines)
        // We need to find NAXIS1 (width), NAXIS2 (height), and END keyword

        if data.len() < 2880 {
            return Err("FITS data too short".to_string());
        }

        let header = std::str::from_utf8(&data[0..2880])
            .map_err(|_| "Invalid FITS header".to_string())?;

        // Parse NAXIS1 (width) and NAXIS2 (height)
        let mut width = 0u32;
        let mut height = 0u32;

        for line in header.as_bytes().chunks(80) {
            let line_str = std::str::from_utf8(line).unwrap_or("");

            if line_str.starts_with("NAXIS1  =") {
                if let Some(value_str) = line_str.split('=').nth(1) {
                    if let Some(num_str) = value_str.split('/').next() {
                        width = num_str.trim().parse().unwrap_or(0);
                    }
                }
            } else if line_str.starts_with("NAXIS2  =") {
                if let Some(value_str) = line_str.split('=').nth(1) {
                    if let Some(num_str) = value_str.split('/').next() {
                        height = num_str.trim().parse().unwrap_or(0);
                    }
                }
            }
        }

        if width == 0 || height == 0 {
            return Err("Failed to parse FITS dimensions".to_string());
        }

        // Data starts after header (skip to next 2880-byte boundary)
        let pixel_count = (width * height) as usize;
        let data_start = 2880;
        let data_end = data_start + pixel_count * 2;

        if data_end > data.len() {
            return Err("FITS data truncated".to_string());
        }

        // Extract 16-bit pixel data (big-endian in FITS)
        let pixels: Vec<u16> = data[data_start..data_end]
            .chunks_exact(2)
            .map(|chunk| u16::from_be_bytes([chunk[0], chunk[1]]))
            .collect();

        Ok(ImageData {
            width,
            height,
            data: pixels,
        })
    }

    /// Calculate HFR, star count, and FWHM from image data
    fn calculate_hfr_and_stars(&self, image: &ImageData) -> Result<(f64, u32, Option<f64>), String> {
        // Convert to f64 for star detection
        let pixels: Vec<f64> = image.data.iter().map(|&p| p as f64).collect();

        // Estimate background using sigma-clipped median
        let (background, noise) = estimate_background(&pixels, image.width as usize, image.height as usize);

        // Detect stars
        let detection_config = StarDetectionConfig {
            detection_sigma: 3.0,
            min_area: 5,
            max_area: 10000,
            max_eccentricity: 0.8,
            saturation_limit: 60000,
            hfr_radius: 20,
        };

        let stars = detect_stars(&pixels, image.width, image.height, background, noise, &detection_config);

        if stars.is_empty() {
            // No stars detected - return high HFR to indicate bad focus
            return Ok((20.0, 0, None));
        }

        // Calculate median HFR from top 50% brightest stars
        let count = (stars.len() / 2).clamp(1, 50);
        let mut hfrs: Vec<f64> = stars.iter()
            .take(count)
            .map(|s| s.hfr)
            .filter(|&h| h > 0.0 && h < 20.0)
            .collect();

        if hfrs.is_empty() {
            return Ok((20.0, stars.len() as u32, None));
        }

        hfrs.sort_by(|a, b| a.partial_cmp(b).unwrap_or(std::cmp::Ordering::Equal));
        let median_hfr = hfrs[hfrs.len() / 2];

        // Calculate median FWHM
        let mut fwhms: Vec<f64> = stars.iter()
            .take(count)
            .map(|s| s.fwhm)
            .filter(|&f| f > 0.0)
            .collect();

        let median_fwhm = if !fwhms.is_empty() {
            fwhms.sort_by(|a, b| a.partial_cmp(b).unwrap_or(std::cmp::Ordering::Equal));
            Some(fwhms[fwhms.len() / 2])
        } else {
            None
        };

        Ok((median_hfr, stars.len() as u32, median_fwhm))
    }

    /// Find best focus position using curve fitting
    fn find_best_focus(&self, data_points: &[FocusDataPoint]) -> Result<(i32, f64), String> {
        if data_points.len() < 3 {
            return Err("Not enough data points for curve fitting".to_string());
        }

        // Apply outlier rejection if configured
        let filtered_points = if self.config.outlier_rejection_sigma > 0.0 {
            self.reject_outliers(data_points)?
        } else {
            data_points.to_vec()
        };

        if filtered_points.len() < 3 {
            return Err("Not enough valid data points after outlier rejection".to_string());
        }

        // Fit curve based on method
        match self.config.method {
            AutofocusMethod::VCurve => self.fit_vcurve(&filtered_points),
            AutofocusMethod::Quadratic => self.fit_parabola(&filtered_points),
            AutofocusMethod::Hyperbolic => self.fit_hyperbola(&filtered_points),
        }
    }

    /// Reject outliers using sigma clipping
    fn reject_outliers(&self, points: &[FocusDataPoint]) -> Result<Vec<FocusDataPoint>, String> {
        if points.len() < 3 {
            return Ok(points.to_vec());
        }

        // Calculate median and MAD
        let mut hfrs: Vec<f64> = points.iter().map(|p| p.hfr).collect();
        hfrs.sort_by(|a, b| a.partial_cmp(b).unwrap_or(std::cmp::Ordering::Equal));

        let median = hfrs[hfrs.len() / 2];

        let mut deviations: Vec<f64> = hfrs.iter().map(|&h| (h - median).abs()).collect();
        deviations.sort_by(|a, b| a.partial_cmp(b).unwrap_or(std::cmp::Ordering::Equal));
        let mad = deviations[deviations.len() / 2];

        // Convert MAD to standard deviation estimate
        let sigma = mad * 1.4826;
        let threshold = self.config.outlier_rejection_sigma * sigma;

        // Filter points
        let filtered: Vec<FocusDataPoint> = points
            .iter()
            .filter(|p| (p.hfr - median).abs() <= threshold)
            .cloned()
            .collect();

        if filtered.len() < 3 {
            return Err("Too many outliers detected, autofocus failed".to_string());
        }

        Ok(filtered)
    }

    /// Fit a V-curve (find minimum)
    fn fit_vcurve(&self, points: &[FocusDataPoint]) -> Result<(i32, f64), String> {
        let min_point = points
            .iter()
            .min_by(|a, b| a.hfr.partial_cmp(&b.hfr).unwrap_or(std::cmp::Ordering::Equal))
            .ok_or("No minimum found")?;

        // Calculate fit quality
        let mean_hfr: f64 = points.iter().map(|p| p.hfr).sum::<f64>() / points.len() as f64;
        let mut ss_tot = 0.0;
        let mut ss_res = 0.0;

        for point in points {
            ss_tot += (point.hfr - mean_hfr).powi(2);
            ss_res += (point.hfr - min_point.hfr).powi(2);
        }

        let r_squared = if ss_tot > 0.0 {
            1.0 - (ss_res / ss_tot)
        } else {
            0.0
        };

        Ok((min_point.position, r_squared.max(0.0)))
    }

    /// Fit a parabola to focus data
    fn fit_parabola(&self, points: &[FocusDataPoint]) -> Result<(i32, f64), String> {
        if points.len() < 3 {
            return Err("Need at least 3 points for parabolic fit".to_string());
        }

        // Fit y = ax^2 + bx + c where y=HFR, x=position
        let n = points.len() as f64;
        let mut sum_x = 0.0;
        let mut sum_y = 0.0;
        let mut sum_x2 = 0.0;
        let mut sum_x3 = 0.0;
        let mut sum_x4 = 0.0;
        let mut sum_xy = 0.0;
        let mut sum_x2y = 0.0;

        for point in points {
            let x = point.position as f64;
            let y = point.hfr;
            sum_x += x;
            sum_y += y;
            sum_x2 += x * x;
            sum_x3 += x * x * x;
            sum_x4 += x * x * x * x;
            sum_xy += x * y;
            sum_x2y += x * x * y;
        }

        // Solve 3x3 system using Cramer's rule
        let det = n * (sum_x2 * sum_x4 - sum_x3 * sum_x3)
            - sum_x * (sum_x * sum_x4 - sum_x2 * sum_x3)
            + sum_x2 * (sum_x * sum_x3 - sum_x2 * sum_x2);

        if det.abs() < 1e-10 {
            return Err("Singular matrix in parabolic fit".to_string());
        }

        let det_a = sum_y * (sum_x2 * sum_x4 - sum_x3 * sum_x3)
            - sum_x * (sum_xy * sum_x4 - sum_x2y * sum_x3)
            + sum_x2 * (sum_xy * sum_x3 - sum_x2y * sum_x2);

        let det_b = n * (sum_xy * sum_x4 - sum_x2y * sum_x3)
            - sum_y * (sum_x * sum_x4 - sum_x2 * sum_x3)
            + sum_x2 * (sum_x * sum_x2y - sum_xy * sum_x2);

        let det_c = n * (sum_x2 * sum_x2y - sum_x3 * sum_xy)
            - sum_x * (sum_x * sum_x2y - sum_x2 * sum_xy)
            + sum_y * (sum_x * sum_x3 - sum_x2 * sum_x2);

        let a = det_c / det;
        let b = det_b / det;
        let c = det_a / det;

        // Check if parabola opens upward
        if a <= 0.0 {
            return Err("Parabola does not have a minimum (a <= 0)".to_string());
        }

        // Find vertex: x = -b / (2a)
        let best_position = (-b / (2.0 * a)).round() as i32;

        // Calculate R-squared
        let mean_y = sum_y / n;
        let mut ss_tot = 0.0;
        let mut ss_res = 0.0;

        for point in points {
            let x = point.position as f64;
            let predicted = a * x * x + b * x + c;
            ss_tot += (point.hfr - mean_y).powi(2);
            ss_res += (point.hfr - predicted).powi(2);
        }

        let r_squared = if ss_tot > 0.0 {
            1.0 - (ss_res / ss_tot)
        } else {
            0.0
        };

        Ok((best_position, r_squared.max(0.0)))
    }

    /// Fit a hyperbola to focus data
    fn fit_hyperbola(&self, points: &[FocusDataPoint]) -> Result<(i32, f64), String> {
        if points.len() < 3 {
            return Err("Need at least 3 points for hyperbolic fit".to_string());
        }

        // Use parabola as initial guess
        let (initial_x0, _) = self.fit_parabola(points)?;
        let mut x0 = initial_x0 as f64;

        // Find minimum HFR as initial b
        let min_hfr = points
            .iter()
            .map(|p| p.hfr)
            .min_by(|a, b| a.partial_cmp(b).unwrap_or(std::cmp::Ordering::Equal))
            .unwrap_or(1.0);
        let b = min_hfr;

        // Iterative refinement
        for _ in 0..10 {
            let mut sum_num = 0.0;
            let mut sum_den = 0.0;

            for point in points {
                let x = point.position as f64;
                let dx = x - x0;
                let y = point.hfr;

                if y * y > b * b {
                    let term = (y * y - b * b).sqrt();
                    if term.abs() > 1e-10 {
                        sum_num += dx * term;
                        sum_den += term * term;
                    }
                }
            }

            if sum_den > 1e-10 {
                let a = sum_num / sum_den;

                let mut new_x0_sum = 0.0;
                let mut count = 0.0;

                for point in points {
                    let x = point.position as f64;
                    let y = point.hfr;
                    let dx = x - x0;

                    if a.abs() > 1e-10 {
                        let predicted_dx_sq = (y * y - b * b) / (a * a);
                        if predicted_dx_sq >= 0.0 {
                            let predicted_dx = predicted_dx_sq.sqrt();
                            new_x0_sum += x - predicted_dx * dx.signum();
                            count += 1.0;
                        }
                    }
                }

                if count > 0.0 {
                    let new_x0 = new_x0_sum / count;
                    x0 = 0.7 * x0 + 0.3 * new_x0;
                }
            }
        }

        // Calculate R-squared
        let mean_y: f64 = points.iter().map(|p| p.hfr).sum::<f64>() / points.len() as f64;
        let mut ss_tot = 0.0;
        let mut ss_res = 0.0;

        // Recalculate 'a' for final fit
        let mut sum_num = 0.0;
        let mut sum_den = 0.0;
        for point in points {
            let x = point.position as f64;
            let dx = x - x0;
            let y = point.hfr;
            if y * y > b * b {
                let term = (y * y - b * b).sqrt();
                sum_num += dx * term;
                sum_den += term * term;
            }
        }
        let a = if sum_den > 1e-10 { sum_num / sum_den } else { 1.0 };

        for point in points {
            let x = point.position as f64;
            let dx = x - x0;
            let predicted = ((dx * a).powi(2) + b * b).sqrt();
            ss_tot += (point.hfr - mean_y).powi(2);
            ss_res += (point.hfr - predicted).powi(2);
        }

        let r_squared = if ss_tot > 0.0 {
            (1.0 - (ss_res / ss_tot)).max(0.0)
        } else {
            0.0
        };

        Ok((x0.round() as i32, r_squared))
    }
}

/// Simple image data structure
#[derive(Debug, Clone)]
struct ImageData {
    width: u32,
    height: u32,
    data: Vec<u16>,
}

/// Detected star
#[derive(Debug, Clone)]
#[allow(dead_code)]
struct DetectedStar {
    pub x: f64,
    pub y: f64,
    pub flux: f64,
    pub hfr: f64,
    pub fwhm: f64,
    pub peak: f64,
}

/// Star detection configuration
#[allow(dead_code)]
struct StarDetectionConfig {
    pub detection_sigma: f64,
    pub min_area: u32,
    pub max_area: u32,
    pub max_eccentricity: f64,
    pub saturation_limit: u16,
    pub hfr_radius: u32,
}

/// Estimate background using sigma clipping
fn estimate_background(pixels: &[f64], _width: usize, _height: usize) -> (f64, f64) {
    // Sample every 4th pixel for speed
    let mut samples: Vec<f64> = pixels.iter().step_by(4).copied().collect();

    if samples.is_empty() {
        return (0.0, 1.0);
    }

    // Sigma clipping iterations
    for _ in 0..3 {
        samples.sort_by(|a, b| a.partial_cmp(b).unwrap_or(std::cmp::Ordering::Equal));
        let median = samples[samples.len() / 2];

        let mad: f64 = samples.iter().map(|&v| (v - median).abs()).sum::<f64>() / samples.len() as f64;
        let sigma = mad * 1.4826;

        let lower = median - 3.0 * sigma;
        let upper = median + 3.0 * sigma;

        samples.retain(|&v| v >= lower && v <= upper);

        if samples.is_empty() {
            return (median, sigma);
        }
    }

    samples.sort_by(|a, b| a.partial_cmp(b).unwrap_or(std::cmp::Ordering::Equal));
    let background = samples[samples.len() / 2];

    let variance: f64 = samples.iter().map(|&v| (v - background).powi(2)).sum::<f64>() / samples.len() as f64;
    let noise = variance.sqrt();

    (background, noise.max(1.0))
}

/// Detect stars in image
fn detect_stars(
    pixels: &[f64],
    width: u32,
    height: u32,
    background: f64,
    noise: f64,
    config: &StarDetectionConfig,
) -> Vec<DetectedStar> {
    let width = width as usize;
    let height = height as usize;
    let threshold = background + config.detection_sigma * noise;

    let mut visited = vec![false; pixels.len()];
    let mut stars = Vec::new();

    for y in 2..height - 2 {
        for x in 2..width - 2 {
            let idx = y * width + x;

            if visited[idx] || pixels[idx] < threshold {
                continue;
            }

            // Check if local maximum
            let val = pixels[idx];
            let is_max = val >= pixels[idx - 1]
                && val >= pixels[idx + 1]
                && val >= pixels[idx - width]
                && val >= pixels[idx + width]
                && val >= pixels[idx - width - 1]
                && val >= pixels[idx - width + 1]
                && val >= pixels[idx + width - 1]
                && val >= pixels[idx + width + 1];

            if !is_max || val > config.saturation_limit as f64 {
                continue;
            }

            // Measure star
            if let Some(star) = measure_star(pixels, width, height, x, y, background, config, &mut visited) {
                let area = star.flux / (star.peak - background);
                if area >= config.min_area as f64 && area <= config.max_area as f64 {
                    stars.push(star);
                }
            }
        }
    }

    stars.sort_by(|a, b| b.flux.partial_cmp(&a.flux).unwrap_or(std::cmp::Ordering::Equal));
    stars
}

/// Measure a star's properties
fn measure_star(
    pixels: &[f64],
    width: usize,
    height: usize,
    cx: usize,
    cy: usize,
    background: f64,
    config: &StarDetectionConfig,
    visited: &mut [bool],
) -> Option<DetectedStar> {
    let radius = config.hfr_radius as i32;

    let mut sum_x = 0.0;
    let mut sum_y = 0.0;
    let mut sum_flux = 0.0;
    let mut peak: f64 = 0.0;

    for dy in -radius..=radius {
        for dx in -radius..=radius {
            let x = cx as i32 + dx;
            let y = cy as i32 + dy;

            if x < 0 || y < 0 || x >= width as i32 || y >= height as i32 {
                continue;
            }

            let idx = y as usize * width + x as usize;
            let val = pixels[idx] - background;

            if val > 0.0 {
                sum_x += x as f64 * val;
                sum_y += y as f64 * val;
                sum_flux += val;
                peak = peak.max(pixels[idx]);
                visited[idx] = true;
            }
        }
    }

    if sum_flux <= 0.0 {
        return None;
    }

    let centroid_x = sum_x / sum_flux;
    let centroid_y = sum_y / sum_flux;

    // Calculate HFR
    let hfr = calculate_hfr(pixels, width, height, centroid_x, centroid_y, background, radius);

    // FWHM from HFR
    const FWHM_TO_HFR_RATIO: f64 = 2.3548200450309493;
    let fwhm = hfr * FWHM_TO_HFR_RATIO;

    Some(DetectedStar {
        x: centroid_x,
        y: centroid_y,
        flux: sum_flux,
        hfr,
        fwhm,
        peak,
    })
}

/// Calculate HFR (Half Flux Radius)
fn calculate_hfr(
    pixels: &[f64],
    width: usize,
    height: usize,
    cx: f64,
    cy: f64,
    background: f64,
    radius: i32,
) -> f64 {
    let mut total_flux = 0.0;
    let mut weighted_radius_sum = 0.0;

    for dy in -radius..=radius {
        for dx in -radius..=radius {
            let x = (cx as i32 + dx).max(0).min(width as i32 - 1) as usize;
            let y = (cy as i32 + dy).max(0).min(height as i32 - 1) as usize;

            let val = (pixels[y * width + x] - background).max(0.0);
            let dist = ((dx as f64).powi(2) + (dy as f64).powi(2)).sqrt();

            total_flux += val;
            weighted_radius_sum += val * dist;
        }
    }

    if total_flux > 0.0 {
        weighted_radius_sum / total_flux
    } else {
        0.0
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use tokio::sync::RwLock;

    #[test]
    fn test_parabolic_fit() {
        let points = vec![
            FocusDataPoint { position: 4500, hfr: 3.5, fwhm: None, star_count: 50 },
            FocusDataPoint { position: 4700, hfr: 2.5, fwhm: None, star_count: 50 },
            FocusDataPoint { position: 4900, hfr: 2.1, fwhm: None, star_count: 50 },
            FocusDataPoint { position: 5000, hfr: 2.0, fwhm: None, star_count: 50 },
            FocusDataPoint { position: 5100, hfr: 2.1, fwhm: None, star_count: 50 },
            FocusDataPoint { position: 5300, hfr: 2.5, fwhm: None, star_count: 50 },
            FocusDataPoint { position: 5500, hfr: 3.5, fwhm: None, star_count: 50 },
        ];

        let config = IndiAutofocusConfig {
            method: AutofocusMethod::Quadratic,
            ..Default::default()
        };

        // Create a dummy camera and focuser (this test only checks the math)
        // We can't easily test the full autofocus without mocking devices

        // Test parabolic fitting directly
        let af = IndiAutofocus {
            camera: Arc::new(IndiCamera::new(
                Arc::new(RwLock::new(crate::IndiClient::new("localhost", None))),
                "TestCamera"
            )),
            focuser: Arc::new(IndiFocuser::new(
                Arc::new(RwLock::new(crate::IndiClient::new("localhost", None))),
                "TestFocuser"
            )),
            config,
        };

        let (best_pos, r_squared) = af.fit_parabola(&points).unwrap();
        assert!((best_pos - 5000).abs() < 100, "Best position should be near 5000");
        assert!(r_squared > 0.9, "R-squared should be high for good fit");
    }

    #[test]
    fn test_vcurve_fit() {
        let points = vec![
            FocusDataPoint { position: 1000, hfr: 5.0, fwhm: None, star_count: 50 },
            FocusDataPoint { position: 1100, hfr: 3.5, fwhm: None, star_count: 50 },
            FocusDataPoint { position: 1200, hfr: 2.2, fwhm: None, star_count: 50 },
            FocusDataPoint { position: 1300, hfr: 3.8, fwhm: None, star_count: 50 },
            FocusDataPoint { position: 1400, hfr: 5.5, fwhm: None, star_count: 50 },
        ];

        let config = IndiAutofocusConfig {
            method: AutofocusMethod::VCurve,
            ..Default::default()
        };

        let af = IndiAutofocus {
            camera: Arc::new(IndiCamera::new(
                Arc::new(RwLock::new(crate::IndiClient::new("localhost", None))),
                "TestCamera"
            )),
            focuser: Arc::new(IndiFocuser::new(
                Arc::new(RwLock::new(crate::IndiClient::new("localhost", None))),
                "TestFocuser"
            )),
            config,
        };

        let (best_pos, _) = af.fit_vcurve(&points).unwrap();
        assert_eq!(best_pos, 1200, "Should find minimum at position 1200");
    }

    #[test]
    fn test_outlier_rejection() {
        let points = vec![
            FocusDataPoint { position: 1000, hfr: 3.0, fwhm: None, star_count: 50 },
            FocusDataPoint { position: 1100, hfr: 2.8, fwhm: None, star_count: 50 },
            FocusDataPoint { position: 1200, hfr: 2.5, fwhm: None, star_count: 50 },
            FocusDataPoint { position: 1300, hfr: 15.0, fwhm: None, star_count: 50 }, // Outlier
            FocusDataPoint { position: 1400, hfr: 3.2, fwhm: None, star_count: 50 },
        ];

        let config = IndiAutofocusConfig {
            outlier_rejection_sigma: 2.0,
            ..Default::default()
        };

        let af = IndiAutofocus {
            camera: Arc::new(IndiCamera::new(
                Arc::new(RwLock::new(crate::IndiClient::new("localhost", None))),
                "TestCamera"
            )),
            focuser: Arc::new(IndiFocuser::new(
                Arc::new(RwLock::new(crate::IndiClient::new("localhost", None))),
                "TestFocuser"
            )),
            config,
        };

        let filtered = af.reject_outliers(&points).unwrap();
        assert_eq!(filtered.len(), 4, "Should reject 1 outlier");
        assert!(filtered.iter().all(|p| p.hfr < 10.0), "Outlier should be removed");
    }
}
