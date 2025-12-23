# Autofocus Progress Widget Enhancement

## Overview

Enhance the autofocus progress panel in the sequencer to provide better visual feedback during AF operations. The current implementation has a squished 40px V-curve with no axis labels and no star visualization.

## Requirements

1. **Larger V-curve graph** - Increase from 40px to 120px height
2. **Axis labels** - Light labeling with axis names and key values (min/max focus, min/current/max HFR)
3. **Star zoom panel** - Show detected star at each focus position to visualize defocus → sharp → defocus
4. **Star navigation** - Cycle between top 5 detected stars with always-visible arrows
5. **Refresh button** - Re-extract star crops if initial detection is limited (e.g., badly defocused)

## Design

### Layout

```
┌─────────────────────────────────────────────────────┐
│ [🎯] Autofocus                      Point 5 of 9   │
├─────────────────────────────────────────────────────┤
│  ┌──────┐                                          │
│  │ Star │   HFR ▲                                  │
│  │ View │    4.2│ ·                                │
│  │80x80 │    3.0│   ·                              │
│  ├──────┤    1.8│     ·  ★                         │
│  │◄ 3/5 ►│ 🔄  │        ·                    120px │
│  └──────┘      └──────────────────────────────────►│
│                 Focus Position (12000 → 15000)     │
├─────────────────────────────────────────────────────┤
│  [HFR: 2.45 px]  [Stars: 127]                      │
│  ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓░░░░░░░░░░░░░░░░░░░░░░░░░░░  55%  │
└─────────────────────────────────────────────────────┘
```

- Star zoom panel (80x80px) floats in top-left corner of V-curve area
- Navigation arrows and counter ("3/5") directly below star panel
- Refresh button (🔄) next to counter
- V-curve fills remaining space at 120px height
- Light axis labels: "HFR" on Y with key values, focus range on X

### Data Flow

**From Rust (sequencer) to Dart:**

Progress updates include structured data:
- `points`: Array of `(focus_position, hfr)` tuples collected so far
- `star_crops`: Up to 5 base64-encoded 80x80 grayscale images (top stars by brightness)
- `star_count`: Total stars detected at current position
- `focus_range`: `(min, max)` focus positions for X-axis scaling

**Important:** Implementation must be in the real autofocus execution path (`execute_autofocus` in `instructions.rs`), not simulator stubs. Simulator can return placeholder data but real hardware path must emit actual data.

### Star Crop Extraction

New function in `imaging/src/stats.rs`:

```rust
pub fn extract_star_crop(
    image: &[u16],      // Full image buffer
    width: u32,
    height: u32,
    star: &StarInfo,    // Detected star with x, y coords
    crop_size: u32,     // 80 pixels
) -> Vec<u8>            // Cropped, normalized grayscale bytes
```

Behavior:
- Centers crop on star position
- Handles edge cases near image borders (clamp or pad)
- Normalizes pixel values to 0-255 for display (auto-stretch based on crop min/max)
- Returns raw bytes; bridge layer handles base64 encoding

### Widget State & Interactions

**State:**
- `currentStarIndex`: Which of available stars is displayed (0-4)
- `starCrops`: List of up to 5 cropped star images
- `starCount`: Total stars detected
- `vcurvePoints`: List of (focusPosition, hfr) tuples
- `focusRange`: (min, max) focus positions

**Interactions:**
- **Left/Right arrows**: Cycle through available star crops (wrap around)
- **Refresh button**: Request re-extraction of star crops from current AF frame
- **V-curve**: Display only, no interaction

**Visual feedback:**
- Arrows gray out if only 1 star available
- Refresh button shows spinner while processing
- Counter shows "★ 3/5" (current index / available crops)

## Implementation Plan

### Rust Changes

1. **`imaging/src/stats.rs`**
   - Add `extract_star_crop()` function
   - Add `StarCropData` struct for serialization

2. **`sequencer/src/instructions.rs`**
   - Modify `execute_autofocus()` to:
     - Sort detected stars by brightness/SNR
     - Extract top 5 star crops after each frame
     - Build structured progress data with V-curve points + crops
     - Serialize as JSON in progress detail

3. **`bridge/src/api/`** (if needed)
   - Add `AutofocusProgressData` struct for FRB

### Dart Changes

4. **`nightshade_core/lib/src/models/`**
   - Add `AutofocusProgressData` model
   - JSON parsing from progress detail

5. **`nightshade_app/.../node_progress_panels.dart`**
   - Convert `_AutofocusProgressPanel` to StatefulWidget
   - Add `_VCurvePainter` with proper axis labels and data points
   - Add `_StarZoomPanel` widget with navigation
   - Add refresh button handler

6. **Communication for refresh**
   - Add mechanism for UI to request new star crops (sequencer command or API call)

## Notes

- Pre-send top 5 star crops to minimize latency when cycling
- Refresh button handles edge case of badly defocused initial frames
- Touch-friendly: always-visible arrows (no hover states)
- Real implementation in hardware path, not just simulator stubs
