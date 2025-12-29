# Polar Alignment UX Redesign Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Redesign the Three-Point Polar Alignment UI with live image display, tiered settings with tooltips, and intuitive "Left/Right/Up/Down" adjustment directions.

**Architecture:** Enhance Rust backend (`polar_align.rs`) to emit stretched JPEG images for UI display and add auto-complete threshold. Rewrite Dart UI (`polar_alignment_screen.dart`) with tiered settings, live camera feed with bullseye overlay, and clear text directions.

**Tech Stack:** Rust (imaging crate for debayer/stretch, image crate for JPEG encoding), Dart/Flutter, Riverpod

---

## Task 1: Add PolarAlignmentImageEvent to Event System

**Files:**
- Modify: `native/nightshade_native/bridge/src/event.rs`

**Step 1: Add the new event struct**

After `PolarAlignmentStatus` struct (around line 144), add:

```rust
/// Polar alignment image data for UI display
#[frb]
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct PolarAlignmentImageEvent {
    /// JPEG-encoded image bytes for display
    pub image_data: Vec<u8>,
    /// Image width
    pub width: u32,
    /// Image height
    pub height: u32,
    /// Plate solve result (if available)
    pub solved_ra: Option<f64>,
    pub solved_dec: Option<f64>,
    /// Current measurement point (1-3) or 0 for adjustment phase
    pub point: i32,
    /// Phase: "measuring" or "adjusting"
    pub phase: String,
}
```

**Step 2: Add to EventPayload enum**

In the `EventPayload` enum (around line 317), add a new variant:

```rust
PolarAlignmentImage(PolarAlignmentImageEvent),
```

**Step 3: Run cargo check**

Run: `cd native/nightshade_native && cargo check --package nightshade_bridge`
Expected: Compiles successfully

**Step 4: Commit**

```bash
git add native/nightshade_native/bridge/src/event.rs
git commit -m "feat(polar-align): add PolarAlignmentImageEvent for live image display"
```

---

## Task 2: Add auto_complete_threshold to PolarAlignConfig

**Files:**
- Modify: `native/nightshade_native/sequencer/src/polar_align.rs`

**Step 1: Add field to PolarAlignConfig struct**

In `PolarAlignConfig` (around line 12), add after `is_north`:

```rust
    /// Auto-complete threshold in arcseconds (default 30")
    /// When total error drops below this and stays for 3 seconds, alignment completes
    pub auto_complete_threshold: f64,
```

**Step 2: Update Default impl if exists, or add default value documentation**

The struct uses serde, so add a default function. After the struct, add:

```rust
impl Default for PolarAlignConfig {
    fn default() -> Self {
        Self {
            step_size: 15.0,  // Changed from 30.0
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
```

**Step 3: Run cargo check**

Run: `cd native/nightshade_native && cargo check --package nightshade_sequencer`
Expected: Compiles successfully

**Step 4: Commit**

```bash
git add native/nightshade_native/sequencer/src/polar_align.rs
git commit -m "feat(polar-align): add auto_complete_threshold config (default 30 arcsec)"
```

---

## Task 3: Create Image Preparation Helper Function

**Files:**
- Modify: `native/nightshade_native/sequencer/src/polar_align.rs`

**Step 1: Add imports at top of file**

```rust
use nightshade_imaging::{ImageData, stretch, debayer};
```

**Step 2: Add helper function before perform_polar_alignment**

```rust
/// Prepare an image for UI display
/// Handles mono and color frames, applies auto-stretch, encodes to JPEG
fn prepare_image_for_display(image: &ImageData, is_color: bool, bayer_pattern: Option<&str>) -> Result<Vec<u8>, String> {
    use image::{ImageBuffer, GrayImage, RgbImage, ImageEncoder};
    use image::codecs::jpeg::JpegEncoder;
    use std::io::Cursor;

    let stretched_bytes = if is_color && bayer_pattern.is_some() {
        // Color camera: debayer then stretch
        let pattern = debayer::BayerPattern::from_str(bayer_pattern.unwrap())
            .unwrap_or(debayer::BayerPattern::RGGB);

        let rgb = debayer::debayer_bilinear(image, pattern);

        // Create interleaved RGB data for stretch
        let rgb_interleaved: Vec<u16> = (0..rgb.red.len())
            .flat_map(|i| [rgb.red[i], rgb.green[i], rgb.blue[i]])
            .collect();

        let (r_params, g_params, b_params) = stretch::auto_stretch_rgb(&rgb_interleaved, image.width, image.height);

        // Use green channel params for linked stretch (more natural)
        let linked_params = g_params;
        stretch::apply_stretch_rgb(&rgb_interleaved, image.width, image.height, &linked_params)
    } else {
        // Mono camera: just stretch
        let params = stretch::auto_stretch_stf(image);
        stretch::apply_stretch(image, &params)
    };

    // Encode to JPEG
    let mut jpeg_bytes = Vec::new();
    let mut cursor = Cursor::new(&mut jpeg_bytes);

    if is_color && bayer_pattern.is_some() {
        // RGB image
        let img: RgbImage = ImageBuffer::from_raw(
            image.width,
            image.height,
            stretched_bytes,
        ).ok_or("Failed to create RGB image buffer")?;

        let mut encoder = JpegEncoder::new_with_quality(&mut cursor, 85);
        encoder.encode(
            img.as_raw(),
            image.width,
            image.height,
            image::ExtendedColorType::Rgb8,
        ).map_err(|e| format!("JPEG encode error: {}", e))?;
    } else {
        // Grayscale image
        let img: GrayImage = ImageBuffer::from_raw(
            image.width,
            image.height,
            stretched_bytes,
        ).ok_or("Failed to create grayscale image buffer")?;

        let mut encoder = JpegEncoder::new_with_quality(&mut cursor, 85);
        encoder.encode(
            img.as_raw(),
            image.width,
            image.height,
            image::ExtendedColorType::L8,
        ).map_err(|e| format!("JPEG encode error: {}", e))?;
    }

    Ok(jpeg_bytes)
}
```

**Step 3: Run cargo check**

Run: `cd native/nightshade_native && cargo check --package nightshade_sequencer`
Expected: Compiles (may need to add image crate dependency)

**Step 4: Commit**

```bash
git add native/nightshade_native/sequencer/src/polar_align.rs
git commit -m "feat(polar-align): add prepare_image_for_display helper for mono/color"
```

---

## Task 4: Emit Images During Measurement Phase

**Files:**
- Modify: `native/nightshade_native/sequencer/src/polar_align.rs`

**Step 1: Add image callback parameter to perform_polar_alignment**

Change function signature:

```rust
pub async fn perform_polar_alignment(
    config: &PolarAlignConfig,
    ctx: &InstructionContext,
    status_callback: impl Fn(String, Option<f64>),
    image_callback: impl Fn(PolarAlignmentImageEvent),  // NEW
) -> InstructionResult {
```

**Step 2: After capturing each measurement image, emit it**

In the measurement loop (around line 104-125), after `let image_data = ...` and before plate solving, add:

```rust
        // Prepare and emit image for UI
        let is_color = ctx.device_ops.camera_is_color(&camera_id).await.unwrap_or(false);
        let bayer = ctx.device_ops.camera_bayer_pattern(&camera_id).await.ok().flatten();

        if let Ok(jpeg_bytes) = prepare_image_for_display(&image_data, is_color, bayer.as_deref()) {
            image_callback(PolarAlignmentImageEvent {
                image_data: jpeg_bytes,
                width: image_data.width,
                height: image_data.height,
                solved_ra: None,  // Will update after solve
                solved_dec: None,
                point: (i + 1) as i32,
                phase: "measuring".to_string(),
            });
        }
```

**Step 3: After plate solve, emit updated event with coordinates**

After `let solve_result = ...`:

```rust
        // Emit image again with solve results
        if let Ok(jpeg_bytes) = prepare_image_for_display(&image_data, is_color, bayer.as_deref()) {
            image_callback(PolarAlignmentImageEvent {
                image_data: jpeg_bytes,
                width: image_data.width,
                height: image_data.height,
                solved_ra: Some(solve_result.ra_degrees),
                solved_dec: Some(solve_result.dec_degrees),
                point: (i + 1) as i32,
                phase: "measuring".to_string(),
            });
        }
```

**Step 4: Run cargo check**

Run: `cd native/nightshade_native && cargo check --package nightshade_sequencer`

**Step 5: Commit**

```bash
git add native/nightshade_native/sequencer/src/polar_align.rs
git commit -m "feat(polar-align): emit images during measurement phase"
```

---

## Task 5: Add Auto-Complete Logic and Emit Images in Adjustment Loop

**Files:**
- Modify: `native/nightshade_native/sequencer/src/polar_align.rs`

**Step 1: Add tracking variables before adjustment loop**

Before `loop {` (around line 162):

```rust
    // Auto-complete tracking
    let threshold_arcsec = config.auto_complete_threshold;
    let threshold_arcmin = threshold_arcsec / 60.0;
    let mut below_threshold_start: Option<std::time::Instant> = None;
    const AUTO_COMPLETE_HOLD_SECS: u64 = 3;
```

**Step 2: In adjustment loop, emit image after each capture**

After getting solve_result in the loop (around line 184), before calculating error:

```rust
        // Prepare and emit image for UI
        let is_color = ctx.device_ops.camera_is_color(&camera_id).await.unwrap_or(false);
        let bayer = ctx.device_ops.camera_bayer_pattern(&camera_id).await.ok().flatten();

        if let Ok(jpeg_bytes) = prepare_image_for_display(&image_data, is_color, bayer.as_deref()) {
            image_callback(PolarAlignmentImageEvent {
                image_data: jpeg_bytes,
                width: image_data.width,
                height: image_data.height,
                solved_ra: Some(solve_result.ra_degrees),
                solved_dec: Some(solve_result.dec_degrees),
                point: 0,  // 0 = adjustment phase
                phase: "adjusting".to_string(),
            });
        }
```

**Step 3: Add auto-complete check after error calculation**

After calculating `total_error_am` (around line 201):

```rust
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
```

**Step 4: Run cargo check**

Run: `cd native/nightshade_native && cargo check --package nightshade_sequencer`

**Step 5: Commit**

```bash
git add native/nightshade_native/sequencer/src/polar_align.rs
git commit -m "feat(polar-align): add auto-complete logic and emit images in adjustment loop"
```

---

## Task 6: Update API Layer to Handle New Image Callback

**Files:**
- Modify: `native/nightshade_native/bridge/src/api.rs`

**Step 1: Find the polar alignment API function**

Search for `api_start_polar_alignment` or similar function that calls `perform_polar_alignment`.

**Step 2: Add image event emission**

In the API wrapper, add an image callback that publishes to the event bus:

```rust
let image_callback = |event: PolarAlignmentImageEvent| {
    if let Some(bus) = EVENT_BUS.get() {
        bus.publish_with_tracking(
            EventSeverity::Info,
            EventCategory::PolarAlignment,
            EventPayload::PolarAlignmentImage(event),
            None,
        );
    }
};
```

**Step 3: Pass the callback to perform_polar_alignment**

Update the call to include the new callback parameter.

**Step 4: Run cargo check**

Run: `cd native/nightshade_native && cargo check --package nightshade_bridge`

**Step 5: Commit**

```bash
git add native/nightshade_native/bridge/src/api.rs
git commit -m "feat(polar-align): wire up image callback to event bus"
```

---

## Task 7: Update Dart Event Handling

**Files:**
- Modify: `packages/nightshade_bridge/lib/src/event.dart` (if manual)
- Or regenerate FRB bindings

**Step 1: Regenerate FRB bindings**

Run: `flutter_rust_bridge_codegen generate`

This will pick up the new `PolarAlignmentImageEvent` struct and `EventPayload` variant.

**Step 2: Update polar_alignment_screen.dart to handle image events**

In `_handlePolarAlignEvent` or equivalent, add handling for image data:

```dart
if (event.payload case EventPayload_PolarAlignmentImage(:final field0)) {
  final imageEvent = field0;
  ref.read(polarAlignImageProvider.notifier).state = imageEvent.imageData;
  // Also update solve coordinates if available
  if (imageEvent.solvedRa != null) {
    ref.read(polarAlignSolveProvider.notifier).state = (
      ra: imageEvent.solvedRa!,
      dec: imageEvent.solvedDec!,
    );
  }
}
```

**Step 3: Run flutter analyze**

Run: `melos run analyze`

**Step 4: Commit**

```bash
git add packages/nightshade_bridge packages/nightshade_app
git commit -m "feat(polar-align): handle PolarAlignmentImageEvent in Dart"
```

---

## Task 8: Rewrite Settings Panel with Tiers and Tooltips

**Files:**
- Modify: `packages/nightshade_app/lib/screens/polar_alignment/polar_alignment_screen.dart`

**Step 1: Add tooltip helper widget**

Add before the screen class:

```dart
class _SettingRow extends StatelessWidget {
  final String label;
  final String tooltip;
  final Widget child;
  final NightshadeColors colors;

  const _SettingRow({
    required this.label,
    required this.tooltip,
    required this.child,
    required this.colors,
  });

  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        Row(
          children: [
            Text(
              label,
              style: TextStyle(fontSize: 11, color: colors.textMuted),
            ),
            const SizedBox(width: 4),
            Tooltip(
              message: tooltip,
              waitDuration: const Duration(milliseconds: 500),
              child: Icon(
                LucideIcons.helpCircle,
                size: 12,
                color: colors.textMuted.withOpacity(0.6),
              ),
            ),
          ],
        ),
        const SizedBox(height: 4),
        child,
      ],
    );
  }
}
```

**Step 2: Create tiered settings sections**

Replace `_buildConfigControls` with three separate builders:

```dart
Widget _buildEssentialSettings(NightshadeColors colors, bool isRunning) {
  return Column(
    children: [
      _SettingRow(
        label: 'Hemisphere',
        tooltip: 'Northern or Southern hemisphere determines celestial pole position',
        colors: colors,
        child: SegmentedButton<bool>(
          segments: const [
            ButtonSegment(value: true, label: Text('North')),
            ButtonSegment(value: false, label: Text('South')),
          ],
          selected: {_isNorthernHemisphere},
          onSelectionChanged: isRunning ? null : (v) => setState(() => _isNorthernHemisphere = v.first),
          style: ButtonStyle(
            visualDensity: VisualDensity.compact,
            textStyle: WidgetStateProperty.all(const TextStyle(fontSize: 11)),
          ),
        ),
      ),
      const SizedBox(height: 12),
      _SettingRow(
        label: 'Exposure',
        tooltip: 'Longer exposures capture more stars but slow down iterations',
        colors: colors,
        child: Row(
          children: [
            Expanded(
              child: Slider(
                value: _exposureTime,
                min: 1,
                max: 30,
                divisions: 29,
                onChanged: isRunning ? null : (v) => setState(() => _exposureTime = v),
              ),
            ),
            SizedBox(
              width: 40,
              child: Text('${_exposureTime.toInt()}s', style: TextStyle(fontSize: 11, color: colors.textPrimary)),
            ),
          ],
        ),
      ),
    ],
  );
}
```

**Step 3: Add collapsible sections for Common and Advanced**

```dart
bool _showCommonSettings = false;
bool _showAdvancedSettings = false;

Widget _buildCommonSettings(NightshadeColors colors, bool isRunning) {
  return ExpansionTile(
    title: Text('Common Settings', style: TextStyle(fontSize: 12, color: colors.textPrimary)),
    initiallyExpanded: _showCommonSettings,
    onExpansionChanged: (v) => setState(() => _showCommonSettings = v),
    children: [
      Padding(
        padding: const EdgeInsets.symmetric(horizontal: 16),
        child: Column(
          children: [
            _SettingRow(
              label: 'Binning',
              tooltip: 'Higher binning = faster plate solves, lower resolution',
              colors: colors,
              child: /* binning selector */,
            ),
            _SettingRow(
              label: 'Step Size',
              tooltip: 'Distance between measurement points. Larger = more accurate but may hit mount limits',
              colors: colors,
              child: /* step size slider, default 15° */,
            ),
            _SettingRow(
              label: 'Direction',
              tooltip: 'Which way to rotate. Use West if near Eastern meridian limit',
              colors: colors,
              child: /* direction selector */,
            ),
          ],
        ),
      ),
    ],
  );
}

Widget _buildAdvancedSettings(NightshadeColors colors, bool isRunning) {
  // Similar pattern for gain, offset, manual rotation, solve timeout,
  // start position, auto-complete threshold
}
```

**Step 4: Run flutter analyze**

Run: `melos run analyze`

**Step 5: Commit**

```bash
git add packages/nightshade_app/lib/screens/polar_alignment/polar_alignment_screen.dart
git commit -m "feat(polar-align): rewrite settings panel with tiers and tooltips"
```

---

## Task 9: Rewrite Measurement Phase UI with Live Image

**Files:**
- Modify: `packages/nightshade_app/lib/screens/polar_alignment/polar_alignment_screen.dart`

**Step 1: Add providers for solve coordinates**

```dart
final polarAlignSolveProvider = StateProvider<({double ra, double dec})?>((ref) => null);
```

**Step 2: Replace _buildMeasuringStatus with image display**

```dart
Widget _buildMeasuringStatus(NightshadeColors colors, int point, String status) {
  final imageData = ref.watch(polarAlignImageProvider);
  final solveCoords = ref.watch(polarAlignSolveProvider);

  return Row(
    children: [
      // Main image area
      Expanded(
        flex: 2,
        child: Container(
          decoration: BoxDecoration(
            color: colors.surfaceAlt,
            borderRadius: BorderRadius.circular(8),
          ),
          child: Stack(
            children: [
              // Image display
              if (imageData != null)
                Center(
                  child: Image.memory(
                    Uint8List.fromList(imageData),
                    fit: BoxFit.contain,
                  ),
                )
              else
                Center(
                  child: Column(
                    mainAxisSize: MainAxisSize.min,
                    children: [
                      CircularProgressIndicator(color: colors.primary),
                      const SizedBox(height: 16),
                      Text('Waiting for image...', style: TextStyle(color: colors.textMuted)),
                    ],
                  ),
                ),

              // Solve coordinates overlay
              if (solveCoords != null)
                Positioned(
                  left: 12,
                  bottom: 12,
                  child: Container(
                    padding: const EdgeInsets.all(8),
                    decoration: BoxDecoration(
                      color: colors.background.withOpacity(0.8),
                      borderRadius: BorderRadius.circular(4),
                    ),
                    child: Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: [
                        Text(
                          'RA: ${_formatRA(solveCoords.ra)}',
                          style: TextStyle(fontSize: 11, color: colors.textPrimary, fontFamily: 'monospace'),
                        ),
                        Text(
                          'Dec: ${_formatDec(solveCoords.dec)}',
                          style: TextStyle(fontSize: 11, color: colors.textPrimary, fontFamily: 'monospace'),
                        ),
                      ],
                    ),
                  ),
                ),
            ],
          ),
        ),
      ),

      const SizedBox(width: 16),

      // Progress panel
      SizedBox(
        width: 180,
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Text('Progress', style: TextStyle(fontSize: 12, fontWeight: FontWeight.w600, color: colors.textPrimary)),
            const SizedBox(height: 12),
            _ProgressItem(colors: colors, label: 'Point 1', isActive: point == 1, isComplete: point > 1),
            _ProgressItem(colors: colors, label: 'Point 2', isActive: point == 2, isComplete: point > 2),
            _ProgressItem(colors: colors, label: 'Point 3', isActive: point == 3, isComplete: point > 3),
            const SizedBox(height: 16),
            Text('Status', style: TextStyle(fontSize: 11, color: colors.textMuted)),
            const SizedBox(height: 4),
            Text(status, style: TextStyle(fontSize: 12, color: colors.textSecondary)),
          ],
        ),
      ),
    ],
  );
}

String _formatRA(double degrees) {
  final hours = degrees / 15.0;
  final h = hours.floor();
  final m = ((hours - h) * 60).floor();
  final s = (((hours - h) * 60 - m) * 60).toStringAsFixed(1);
  return '${h.toString().padLeft(2, '0')}h ${m.toString().padLeft(2, '0')}m ${s}s';
}

String _formatDec(double degrees) {
  final sign = degrees >= 0 ? '+' : '-';
  final abs = degrees.abs();
  final d = abs.floor();
  final m = ((abs - d) * 60).floor();
  final s = (((abs - d) * 60 - m) * 60).toStringAsFixed(0);
  return '$sign${d.toString().padLeft(2, '0')}° ${m.toString().padLeft(2, '0')}\' ${s}"';
}
```

**Step 3: Run flutter analyze**

Run: `melos run analyze`

**Step 4: Commit**

```bash
git add packages/nightshade_app/lib/screens/polar_alignment/polar_alignment_screen.dart
git commit -m "feat(polar-align): rewrite measurement phase UI with live image display"
```

---

## Task 10: Rewrite Adjustment Phase UI with Bullseye Overlay

**Files:**
- Modify: `packages/nightshade_app/lib/screens/polar_alignment/polar_alignment_screen.dart`

**Step 1: Create bullseye overlay painter**

```dart
class _BullseyeOverlayPainter extends CustomPainter {
  final NightshadeColors colors;
  final double? azimuthError;  // arcminutes
  final double? altitudeError; // arcminutes

  _BullseyeOverlayPainter({
    required this.colors,
    this.azimuthError,
    this.altitudeError,
  });

  @override
  void paint(Canvas canvas, Size size) {
    final center = Offset(size.width / 2, size.height / 2);
    final maxRadius = (size.width < size.height ? size.width : size.height) / 2 - 20;

    // Scale: 5 arcminutes = maxRadius
    final scale = maxRadius / 5.0;

    // Draw concentric rings at 1', 3', 5'
    final ringPaint = Paint()
      ..style = PaintingStyle.stroke
      ..strokeWidth = 1;

    for (final arcmin in [1.0, 3.0, 5.0]) {
      ringPaint.color = arcmin == 1.0
          ? colors.success.withOpacity(0.5)
          : arcmin == 3.0
              ? colors.warning.withOpacity(0.5)
              : colors.error.withOpacity(0.5);
      canvas.drawCircle(center, arcmin * scale, ringPaint);
    }

    // Draw crosshairs
    final crossPaint = Paint()
      ..color = colors.textMuted.withOpacity(0.5)
      ..strokeWidth = 1;
    canvas.drawLine(Offset(center.dx - maxRadius, center.dy), Offset(center.dx + maxRadius, center.dy), crossPaint);
    canvas.drawLine(Offset(center.dx, center.dy - maxRadius), Offset(center.dx, center.dy + maxRadius), crossPaint);

    // Draw center target
    final targetPaint = Paint()..color = colors.primary;
    canvas.drawCircle(center, 4, targetPaint);

    // Draw error position
    if (azimuthError != null && altitudeError != null) {
      final errorX = azimuthError!.clamp(-5.0, 5.0) * scale;
      final errorY = -altitudeError!.clamp(-5.0, 5.0) * scale;  // Negative because screen Y is inverted
      final errorPos = Offset(center.dx + errorX, center.dy + errorY);

      final errorPaint = Paint()..color = colors.error;
      canvas.drawCircle(errorPos, 8, errorPaint);
    }
  }

  @override
  bool shouldRepaint(covariant _BullseyeOverlayPainter oldDelegate) {
    return oldDelegate.azimuthError != azimuthError || oldDelegate.altitudeError != altitudeError;
  }
}
```

**Step 2: Replace _buildAdjustmentInstructions with image + overlay**

```dart
Widget _buildAdjustmentUI(NightshadeColors colors, dynamic error) {
  final imageData = ref.watch(polarAlignImageProvider);

  return Row(
    children: [
      // Main image area with bullseye overlay
      Expanded(
        flex: 2,
        child: Container(
          decoration: BoxDecoration(
            color: colors.surfaceAlt,
            borderRadius: BorderRadius.circular(8),
          ),
          child: Stack(
            children: [
              // Live image
              if (imageData != null)
                Center(
                  child: Image.memory(
                    Uint8List.fromList(imageData),
                    fit: BoxFit.contain,
                  ),
                ),

              // Bullseye overlay
              Positioned.fill(
                child: CustomPaint(
                  painter: _BullseyeOverlayPainter(
                    colors: colors,
                    azimuthError: error?.azimuth,
                    altitudeError: error?.altitude,
                  ),
                ),
              ),
            ],
          ),
        ),
      ),

      const SizedBox(width: 16),

      // Direction panel
      SizedBox(
        width: 180,
        child: _buildDirectionPanel(colors, error),
      ),
    ],
  );
}

Widget _buildDirectionPanel(NightshadeColors colors, dynamic error) {
  if (error == null) return const SizedBox.shrink();

  final azDir = error.azimuth > 0 ? 'Right' : 'Left';
  final altDir = error.altitude > 0 ? 'Down' : 'Up';

  final azColor = error.azimuth.abs() < 1.0 ? colors.success : colors.warning;
  final altColor = error.altitude.abs() < 1.0 ? colors.success : colors.warning;
  final totalColor = error.total < 1.0 ? colors.success : error.total < 3.0 ? colors.warning : colors.error;

  return Column(
    crossAxisAlignment: CrossAxisAlignment.start,
    children: [
      // Azimuth
      Text('Azimuth', style: TextStyle(fontSize: 11, color: colors.textMuted)),
      const SizedBox(height: 4),
      Text(
        '$azDir ${error.azimuth.abs().toStringAsFixed(1)}\'',
        style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold, color: azColor),
      ),

      const SizedBox(height: 16),

      // Altitude
      Text('Altitude', style: TextStyle(fontSize: 11, color: colors.textMuted)),
      const SizedBox(height: 4),
      Text(
        '$altDir ${error.altitude.abs().toStringAsFixed(1)}\'',
        style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold, color: altColor),
      ),

      const SizedBox(height: 24),
      Divider(color: colors.border),
      const SizedBox(height: 16),

      // Total error
      Text('Total Error', style: TextStyle(fontSize: 11, color: colors.textMuted)),
      const SizedBox(height: 4),
      Text(
        '${error.total.toStringAsFixed(1)}\'',
        style: TextStyle(fontSize: 24, fontWeight: FontWeight.bold, color: totalColor),
      ),

      const SizedBox(height: 16),

      // Progress toward threshold
      Text('Threshold: ${(_autoCompleteThreshold / 60).toStringAsFixed(1)}\'',
           style: TextStyle(fontSize: 10, color: colors.textMuted)),
      const SizedBox(height: 4),
      LinearProgressIndicator(
        value: (1.0 - (error.total / 5.0)).clamp(0.0, 1.0),
        backgroundColor: colors.surfaceAlt,
        color: totalColor,
      ),
    ],
  );
}
```

**Step 3: Run flutter analyze**

Run: `melos run analyze`

**Step 4: Commit**

```bash
git add packages/nightshade_app/lib/screens/polar_alignment/polar_alignment_screen.dart
git commit -m "feat(polar-align): rewrite adjustment phase UI with bullseye overlay"
```

---

## Task 11: Add Done Button and Auto-Complete Threshold Setting

**Files:**
- Modify: `packages/nightshade_app/lib/screens/polar_alignment/polar_alignment_screen.dart`

**Step 1: Add state variable for threshold**

```dart
double _autoCompleteThreshold = 30.0;  // arcseconds
```

**Step 2: Add threshold setting in Advanced section**

```dart
_SettingRow(
  label: 'Auto-complete Threshold',
  tooltip: 'Automatically finish when error stays below this value for 3 seconds',
  colors: colors,
  child: Row(
    children: [
      Expanded(
        child: Slider(
          value: _autoCompleteThreshold,
          min: 10,
          max: 120,
          divisions: 11,
          onChanged: isRunning ? null : (v) => setState(() => _autoCompleteThreshold = v),
        ),
      ),
      SizedBox(
        width: 50,
        child: Text('${_autoCompleteThreshold.toInt()}"', style: TextStyle(fontSize: 11, color: colors.textPrimary)),
      ),
    ],
  ),
),
```

**Step 3: Update footer to show Done button during adjustment**

In `_buildFooter`, update the adjustment phase buttons:

```dart
if (phase == PolarAlignPhase.adjusting)
  Row(
    children: [
      OutlinedButton.icon(
        onPressed: _stopAlignment,
        icon: Icon(LucideIcons.square, size: 16, color: colors.error),
        label: Text('Stop', style: TextStyle(color: colors.error)),
      ),
      const SizedBox(width: 8),
      FilledButton.icon(
        onPressed: () {
          // Mark as complete with current error
          ref.read(polarAlignPhaseProvider.notifier).state = PolarAlignPhase.complete;
          _stopAlignment();
        },
        icon: const Icon(LucideIcons.check, size: 16),
        label: const Text('Done'),
      ),
    ],
  )
```

**Step 4: Run flutter analyze**

Run: `melos run analyze`

**Step 5: Commit**

```bash
git add packages/nightshade_app/lib/screens/polar_alignment/polar_alignment_screen.dart
git commit -m "feat(polar-align): add Done button and auto-complete threshold setting"
```

---

## Task 12: Integration Test and Polish

**Step 1: Run full build**

Run: `melos run dev:norun`

**Step 2: Test manually**

- Start polar alignment
- Verify images appear during measurement phase
- Verify solve coordinates overlay
- Verify bullseye overlay during adjustment
- Verify "Left/Right/Up/Down" directions
- Verify Done button works
- Verify auto-complete works when threshold is reached
- Verify tooltips appear on hover

**Step 3: Fix any issues found**

**Step 4: Final commit**

```bash
git add -A
git commit -m "feat(polar-align): complete TPPA UX redesign with live images and improved directions"
```

---

## Summary

| Task | Description |
|------|-------------|
| 1 | Add PolarAlignmentImageEvent to event system |
| 2 | Add auto_complete_threshold to config |
| 3 | Create image preparation helper (mono/color handling) |
| 4 | Emit images during measurement phase |
| 5 | Add auto-complete logic and emit images in adjustment loop |
| 6 | Wire up image callback to event bus in API |
| 7 | Update Dart event handling for new image event |
| 8 | Rewrite settings panel with tiers and tooltips |
| 9 | Rewrite measurement phase UI with live image |
| 10 | Rewrite adjustment phase UI with bullseye overlay |
| 11 | Add Done button and auto-complete threshold setting |
| 12 | Integration test and polish |
