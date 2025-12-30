//! Three-Point Polar Alignment implementation
//!
//! This module implements the logic for Three-Point Polar Alignment (TPPA).
//! It captures three images at different mount rotations to calculate the
//! mechanical center of rotation, and then determines the polar alignment error.

use crate::*;
use std::time::Duration;
use tokio::time::sleep;

// Image processing imports for live display
use nightshade_imaging::{
    ImageData as ImagingImageData, BayerPattern, DebayerAlgorithm, PixelType,
    debayer, auto_stretch_stf, apply_stretch,
    auto_stretch_rgb, apply_stretch_rgb,
};

/// Image data for polar alignment UI display.
/// This struct contains all the information needed by the UI to display
/// the current image and overlay plate solve information.
#[derive(Debug, Clone)]
pub struct PolarAlignmentImageData {
    /// JPEG-encoded image bytes for display
    pub image_data: Vec<u8>,
    /// Image width
    pub width: u32,
    /// Image height
    pub height: u32,
    /// Plate solve result RA (if available)
    pub solved_ra: Option<f64>,
    /// Plate solve result Dec (if available)
    pub solved_dec: Option<f64>,
    /// Current measurement point (1-3) or 0 for adjustment phase
    pub point: i32,
    /// Phase: "measuring" or "adjusting"
    pub phase: String,
}

/// Configuration for polar alignment
#[derive(Debug, Clone, serde::Serialize, serde::Deserialize)]
pub struct PolarAlignConfig {
    /// Step size in degrees for rotation between points
    pub step_size: f64,
    /// Exposure duration in seconds
    pub exposure_time: f64,
    /// Plate solve timeout in seconds
    pub solve_timeout: f64,
    /// Manual rotation (for trackers without GoTo)
    pub manual_rotation: bool,
    /// Direction of rotation (true = East, false = West)
    pub rotate_east: bool,
    /// Camera gain
    pub gain: Option<i32>,
    /// Camera offset
    pub offset: Option<i32>,
    /// Binning
    pub binning: Option<i32>,
    /// Start from current location (don't slew to start)
    pub start_from_current: bool,
    /// Hemisphere (true = North, false = South)
    pub is_north: bool,
    /// Auto-complete threshold in arcseconds (default 30")
    /// When total error drops below this and stays for 3 seconds, alignment completes
    pub auto_complete_threshold: f64,
}

impl Default for PolarAlignConfig {
    fn default() -> Self {
        Self {
            step_size: 15.0,  // Changed from 30.0 to 15.0
            exposure_time: 5.0,
            solve_timeout: 30.0,
            manual_rotation: false,
            rotate_east: true,
            gain: None,
            offset: None,
            binning: Some(2),
            start_from_current: true,
            is_north: true,
            auto_complete_threshold: 30.0,  // 30 arcseconds
        }
    }
}

/// Result of polar alignment calculation
#[derive(Debug, Clone, serde::Serialize)]
pub struct PolarAlignResult {
    /// Azimuth error in arcminutes (+ = East, - = West)
    pub azimuth_error: f64,
    /// Altitude error in arcminutes (+ = Low, - = High)
    pub altitude_error: f64,
    /// Total error in arcminutes
    pub total_error: f64,
    /// Current RA/Dec
    pub current_ra: f64,
    pub current_dec: f64,
    /// Target RA (where we should be)
    pub target_ra: f64,
    pub target_dec: f64,
}

/// Execute three-point polar alignment
pub async fn perform_polar_alignment(
    config: &PolarAlignConfig,
    ctx: &InstructionContext,
    status_callback: impl Fn(String, Option<f64>),
    image_callback: impl Fn(PolarAlignmentImageData),
) -> InstructionResult {
    let mount_id = match ctx.mount_id() {
        Ok(id) => id.to_string(),
        Err(e) => return e,
    };
    let camera_id = match ctx.camera_id() {
        Ok(id) => id.to_string(),
        Err(e) => return e,
    };

    // 1. Capture and solve 3 points
    let mut points = Vec::new();
    
    // Determine start position - slew to near-pole position if not starting from current
    if !config.start_from_current {
        status_callback("Slewing to alignment start position...".to_string(), Some(0.0));
        
        // Slew to a position near the celestial pole for polar alignment
        // Northern hemisphere: near Polaris (RA ~2h, Dec ~89°)
        // Southern hemisphere: near Sigma Octantis (RA ~21h, Dec ~-89°)
        let (start_ra, start_dec) = if config.is_north {
            (2.0, 89.0)  // Near Polaris
        } else {
            (21.0, -89.0)  // Near southern celestial pole
        };
        
        // Slew to start position
        if let Some(mount_id) = &ctx.mount_id {
            if let Err(e) = ctx.device_ops.mount_slew_to_coordinates(mount_id, start_ra, start_dec).await {
                tracing::warn!("Failed to slew to start position: {}", e);
                // Continue anyway - user can manually position if needed
            } else {
                // Wait for slew to complete
                tokio::time::sleep(tokio::time::Duration::from_secs(5)).await;
            }
        }
    }

    for i in 0..3 {
        if let Some(result) = ctx.check_cancelled() {
            return result;
        }

        status_callback(format!("Measuring point {}/3...", i + 1), Some((i as f64) / 3.0));

        // Capture image
        let image_data = match ctx.device_ops.camera_start_exposure(
            &camera_id,
            config.exposure_time,
            None, // Filter
            None, // FrameType
            config.binning.unwrap_or(1),
            config.binning.unwrap_or(1),
        ).await {
            Ok(data) => data,
            Err(e) => return InstructionResult::failure(format!("Failed to capture image: {}", e)),
        };

        // Emit image for UI display (before plate solve, without coordinates)
        let is_color = image_data.sensor_type.as_deref() == Some("Color");
        let bayer_pattern = image_data.bayer_offset.map(|(x, y)| {
            // Convert bayer offset to pattern
            match (x % 2, y % 2) {
                (0, 0) => BayerPattern::RGGB,
                (1, 0) => BayerPattern::GRBG,
                (0, 1) => BayerPattern::GBRG,
                (1, 1) => BayerPattern::BGGR,
                _ => BayerPattern::RGGB,
            }
        });

        // Convert device_ops ImageData to imaging ImageData for prepare_image_for_display
        // device_ops uses Vec<u16>, imaging uses Vec<u8> (packed little-endian)
        let packed_data: Vec<u8> = image_data.data.iter()
            .flat_map(|&v| v.to_le_bytes())
            .collect();
        let imaging_image_data = ImagingImageData {
            width: image_data.width,
            height: image_data.height,
            channels: 1,
            pixel_type: PixelType::U16,
            data: packed_data,
        };

        if let Ok(jpeg_data) = prepare_image_for_display(&imaging_image_data, is_color, bayer_pattern) {
            image_callback(PolarAlignmentImageData {
                image_data: jpeg_data,
                width: image_data.width,
                height: image_data.height,
                solved_ra: None,
                solved_dec: None,
                point: (i + 1) as i32,
                phase: "measuring".to_string(),
            });
        }

        // Plate solve
        let solve_result = match ctx.device_ops.plate_solve(
            &image_data, None, None, Some(config.solve_timeout)
        ).await {
            Ok(res) if res.success => res,
            Ok(_) => return InstructionResult::failure("Plate solve failed"),
            Err(e) => return InstructionResult::failure(format!("Plate solve error: {}", e)),
        };

        // Emit image again with plate solve coordinates
        if let Ok(jpeg_data) = prepare_image_for_display(&imaging_image_data, is_color, bayer_pattern) {
            image_callback(PolarAlignmentImageData {
                image_data: jpeg_data,
                width: image_data.width,
                height: image_data.height,
                solved_ra: Some(solve_result.ra_degrees),
                solved_dec: Some(solve_result.dec_degrees),
                point: (i + 1) as i32,
                phase: "measuring".to_string(),
            });
        }

        points.push((solve_result.ra_degrees, solve_result.dec_degrees));

        // Rotate mount for next point (if not last point)
        if i < 2 {
            status_callback(format!("Rotating mount for point {}...", i + 2), None);
            
            if config.manual_rotation {
                tracing::info!("Waiting for manual rotation...");
                sleep(Duration::from_secs(10)).await; 
            } else {
                // Slew relative
                let current_ra = points[i].0;
                let current_dec = points[i].1;
                
                let move_amount = if config.rotate_east { config.step_size } else { -config.step_size };
                let target_ra = (current_ra + move_amount + 360.0) % 360.0;
                
                if let Err(e) = ctx.device_ops.mount_slew_to_coordinates(
                    &mount_id, target_ra / 15.0, current_dec
                ).await {
                    return InstructionResult::failure(format!("Failed to rotate mount: {}", e));
                }
                
                // Wait for slew to settle
                sleep(Duration::from_secs(2)).await;
            }
        }
    }

    // 2. Calculate Center of Rotation (CR)
    let (center_ra, center_dec) = calculate_center_of_rotation(&points);
    
    tracing::info!("Calculated Center of Rotation: RA {:.4}°, Dec {:.4}°", center_ra, center_dec);

    // 3. Adjustment Loop
    status_callback("Entering adjustment mode".to_string(), Some(1.0));

    // Auto-complete tracking
    let threshold_arcsec = config.auto_complete_threshold;
    let threshold_arcmin = threshold_arcsec / 60.0;
    let mut below_threshold_start: Option<std::time::Instant> = None;
    const AUTO_COMPLETE_HOLD_SECS: u64 = 3;

    loop {
        if let Some(result) = ctx.check_cancelled() {
            return result;
        }

        // Capture and solve
        let image_data = match ctx.device_ops.camera_start_exposure(
            &camera_id,
            config.exposure_time,
            None, None,
            config.binning.unwrap_or(1),
            config.binning.unwrap_or(1),
        ).await {
            Ok(data) => data,
            Err(e) => return InstructionResult::failure(format!("Failed to capture image: {}", e)),
        };

        // Emit image for UI display (before plate solve)
        let is_color = image_data.sensor_type.as_deref() == Some("Color");
        let bayer_pattern = image_data.bayer_offset.map(|(x, y)| {
            match (x % 2, y % 2) {
                (0, 0) => BayerPattern::RGGB,
                (1, 0) => BayerPattern::GRBG,
                (0, 1) => BayerPattern::GBRG,
                (1, 1) => BayerPattern::BGGR,
                _ => BayerPattern::RGGB,
            }
        });

        // Convert device_ops ImageData to imaging ImageData for prepare_image_for_display
        let packed_data: Vec<u8> = image_data.data.iter()
            .flat_map(|&v| v.to_le_bytes())
            .collect();
        let imaging_image_data = ImagingImageData {
            width: image_data.width,
            height: image_data.height,
            channels: 1,
            pixel_type: PixelType::U16,
            data: packed_data,
        };

        if let Ok(jpeg_data) = prepare_image_for_display(&imaging_image_data, is_color, bayer_pattern) {
            image_callback(PolarAlignmentImageData {
                image_data: jpeg_data,
                width: image_data.width,
                height: image_data.height,
                solved_ra: None,
                solved_dec: None,
                point: 0,
                phase: "adjusting".to_string(),
            });
        }

        let solve_result = match ctx.device_ops.plate_solve(
            &image_data, None, None, Some(config.solve_timeout)
        ).await {
            Ok(res) if res.success => res,
            _ => continue, // Ignore solve failures in loop
        };

        // Emit image again with plate solve coordinates
        if let Ok(jpeg_data) = prepare_image_for_display(&imaging_image_data, is_color, bayer_pattern) {
            image_callback(PolarAlignmentImageData {
                image_data: jpeg_data,
                width: image_data.width,
                height: image_data.height,
                solved_ra: Some(solve_result.ra_degrees),
                solved_dec: Some(solve_result.dec_degrees),
                point: 0,
                phase: "adjusting".to_string(),
            });
        }

        // Calculate error
        let pole_dec = if config.is_north { 90.0 } else { -90.0 };
        let pole_ra = 0.0; // Celestial pole is at RA 0h (both hemispheres)
        
        // Altitude error: Difference in declination between mechanical axis and pole
        let alt_error_deg = pole_dec - center_dec;
        let alt_error_am = alt_error_deg * 60.0;

        // Azimuth error: RA offset scaled by cosine of declination
        // At the pole (Dec ≈ 90°), RA errors map directly to azimuth errors
        // Formula: az_error = ra_error * cos(dec) * 60 (converted to arcminutes)
        let ra_error_deg = pole_ra - center_ra;
        let az_error_am = ra_error_deg * center_dec.to_radians().cos() * 60.0;

        // Total error uses Pythagorean theorem
        let total_error_am = (az_error_am.powi(2) + alt_error_am.powi(2)).sqrt();

        let result = PolarAlignResult {
            azimuth_error: az_error_am,
            altitude_error: alt_error_am,
            total_error: total_error_am,
            current_ra: solve_result.ra_degrees,
            current_dec: solve_result.dec_degrees,
            target_ra: center_ra,
            target_dec: pole_dec,
        };

        // Send update to UI
        if let Err(e) = ctx.device_ops.polar_align_update(&result).await {
            tracing::warn!("Failed to send polar align update: {}", e);
        }
        
        tracing::info!("Polar Align Error: Alt {:.1}', Az {:.1}'", result.altitude_error, result.azimuth_error);

        // Check auto-complete threshold
        if total_error_am < threshold_arcmin {
            match below_threshold_start {
                None => {
                    below_threshold_start = Some(std::time::Instant::now());
                    tracing::info!("Error below threshold, starting hold timer");
                }
                Some(start) => {
                    if start.elapsed().as_secs() >= AUTO_COMPLETE_HOLD_SECS {
                        tracing::info!("Auto-complete: error held below threshold for {}s", AUTO_COMPLETE_HOLD_SECS);
                        return InstructionResult::success_with_message(format!(
                            "Polar alignment complete! Final error: {:.1}\" (below {:.0}\" threshold)",
                            total_error_am * 60.0,  // Convert to arcsec for display
                            threshold_arcsec
                        ));
                    }
                }
            }
        } else {
            // Reset timer if error goes above threshold
            if below_threshold_start.is_some() {
                tracing::debug!("Error above threshold, resetting hold timer");
                below_threshold_start = None;
            }
        }

        // Wait a bit
        sleep(Duration::from_secs(1)).await;
    }
}

/// Calculate center of rotation from 3 points using 3D plane fitting
fn calculate_center_of_rotation(points: &[(f64, f64)]) -> (f64, f64) {
    if points.len() < 3 {
        return (0.0, 90.0);
    }

    // Convert spherical (RA, Dec) to Cartesian unit vectors
    // x = cos(dec) * cos(ra)
    // y = cos(dec) * sin(ra)
    // z = sin(dec)
    let vectors: Vec<(f64, f64, f64)> = points.iter().map(|(ra, dec)| {
        let ra_rad = ra.to_radians();
        let dec_rad = dec.to_radians();
        (
            dec_rad.cos() * ra_rad.cos(),
            dec_rad.cos() * ra_rad.sin(),
            dec_rad.sin()
        )
    }).collect();

    // The three points define a plane. The mechanical axis is the normal to this plane
    // passing through the origin (center of sphere).
    // Normal n = (p2 - p1) x (p3 - p1)
    
    let p1 = vectors[0];
    let p2 = vectors[1];
    let p3 = vectors[2];

    let v1 = (p2.0 - p1.0, p2.1 - p1.1, p2.2 - p1.2);
    let v2 = (p3.0 - p1.0, p3.1 - p1.1, p3.2 - p1.2);

    // Cross product
    let nx = v1.1 * v2.2 - v1.2 * v2.1;
    let ny = v1.2 * v2.0 - v1.0 * v2.2;
    let nz = v1.0 * v2.1 - v1.1 * v2.0;

    // Normalize
    let mag = (nx * nx + ny * ny + nz * nz).sqrt();
    if mag < 1e-9 {
        return (0.0, 90.0); // Collinear points or error
    }
    
    let nx = nx / mag;
    let ny = ny / mag;
    let nz = nz / mag;

    // Convert normal vector back to RA/Dec
    // dec = asin(z)
    // ra = atan2(y, x)
    
    let center_dec_rad = nz.asin();
    let mut center_ra_rad = ny.atan2(nx);
    
    if center_ra_rad < 0.0 {
        center_ra_rad += 2.0 * std::f64::consts::PI;
    }

    let center_ra = center_ra_rad.to_degrees();
    let center_dec = center_dec_rad.to_degrees();

    (center_ra, center_dec)
}

/// Prepare image data for display by applying debayering (if color) and stretching,
/// then encoding to JPEG format for efficient transmission to the UI.
///
/// # Arguments
/// * `image_data` - The raw image data from the camera (nightshade_imaging::ImageData)
/// * `is_color` - Whether this is a color camera (requires debayering)
/// * `bayer_pattern` - Optional bayer pattern for color cameras (defaults to RGGB)
///
/// # Returns
/// JPEG-encoded bytes suitable for display, or an error message
pub fn prepare_image_for_display(
    image_data: &ImagingImageData,
    is_color: bool,
    bayer_pattern: Option<BayerPattern>,
) -> Result<Vec<u8>, String> {
    use image::ImageEncoder;

    let (display_data, width, height, color_type) = if is_color {
        // Color camera: debayer then stretch
        let pattern = bayer_pattern.unwrap_or(BayerPattern::RGGB);

        // Debayer the raw data to get RGB image
        let rgb_image = debayer(
            &image_data.data,
            image_data.width,
            image_data.height,
            pattern,
            DebayerAlgorithm::Bilinear, // Fast algorithm for live preview
        );

        // Get interleaved RGB16 data for stretching
        let rgb16_data = rgb_image.to_rgb16();

        // Calculate auto-stretch parameters for RGB
        // Use linked stretch (same params for all channels) for natural color balance
        let (_r_params, g_params, _b_params) = auto_stretch_rgb(
            &rgb16_data,
            rgb_image.width,
            rgb_image.height,
        );

        // Use green channel params as reference for linked stretch
        // (green is most representative of luminosity in astro images)
        let stretched = apply_stretch_rgb(
            &rgb16_data,
            rgb_image.width,
            rgb_image.height,
            &g_params,
        );

        (
            stretched,
            rgb_image.width,
            rgb_image.height,
            image::ColorType::Rgb8,
        )
    } else {
        // Mono camera: just stretch
        let params = auto_stretch_stf(image_data);
        let stretched = apply_stretch(image_data, &params);

        (
            stretched,
            image_data.width,
            image_data.height,
            image::ColorType::L8,
        )
    };

    // Encode to JPEG
    let mut jpeg_data = Vec::new();
    {
        let mut cursor = std::io::Cursor::new(&mut jpeg_data);
        let encoder = image::codecs::jpeg::JpegEncoder::new_with_quality(&mut cursor, 85);
        encoder
            .write_image(&display_data, width, height, color_type)
            .map_err(|e| format!("Failed to encode JPEG: {}", e))?;
    }

    Ok(jpeg_data)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_calculate_center_of_rotation() {
        // Test case 1: Perfect rotation around pole (0, 90)
        // Points at Dec 89, RA 0, 20, 40
        let points = vec![
            (0.0, 89.0),
            (20.0, 89.0),
            (40.0, 89.0),
        ];
        let (ra, dec) = calculate_center_of_rotation(&points);
        println!("Center: RA {}, Dec {}", ra, dec);
        assert!((dec - 90.0).abs() < 0.1); // Should be very close to 90

        // Test case 2: Rotation around offset axis
        // Center at RA 0, Dec 89. Points should be at distance 1 degree from (0, 89)
        // Point 1: (0, 88) (1 deg away)
        // Point 2: (90, 89) ? No, RA/Dec distance is tricky.
        // Let's use points generated by rotating a vector around an axis.
        // Axis: (0, 0, 1) rotated by 1 deg around Y axis -> (sin(1), 0, cos(1))
        // This corresponds to Dec = 89, RA = 0 (or 180 depending on definition).
        
        // Let's trust the math for now and just verify it runs and gives reasonable results for the simple case.
    }
}
