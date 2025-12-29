# Polar Alignment UX Redesign

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Redesign the Three-Point Polar Alignment UI to match NINA TPPA functionality with better settings UX, live image display, and intuitive adjustment directions.

**Architecture:** Enhance existing `polar_align.rs` backend to emit image data, add auto-complete threshold. Rewrite `polar_alignment_screen.dart` UI with tiered settings, live camera feed with bullseye overlay, and clear text directions.

**Tech Stack:** Dart/Flutter, Rust, Riverpod, existing imaging pipeline (LibRaw, FITS, debayer, stretch)

---

## Design Decisions

### Settings Organization

Three tiers with hover tooltips (no inline clutter):

**Essential (always visible):**
| Setting | Default | Tooltip |
|---------|---------|---------|
| Hemisphere | Auto-detect from site location | Northern or Southern hemisphere determines pole position |
| Exposure | 5s | Longer = more stars, but slower iterations |

**Common (collapsed section, click to expand):**
| Setting | Default | Tooltip |
|---------|---------|---------|
| Binning | 2x2 | Higher binning = faster solves, lower resolution |
| Step Size | 15° | Distance between measurement points. Larger = more accurate but may hit mount limits |
| Direction | East | Which way to rotate for measurements. Use West if near meridian limit |

**Advanced (collapsed, labeled "Advanced"):**
| Setting | Default | Tooltip |
|---------|---------|---------|
| Gain | Camera default | Override camera gain for alignment exposures |
| Offset | Camera default | Override camera offset for alignment exposures |
| Manual Rotation | Off | Enable for star trackers without GoTo capability |
| Solve Timeout | 30s | Maximum time to wait for plate solve |
| Start Position | Current | Use current position or slew near pole first |
| Auto-complete Threshold | 30" | Automatically finish when error stays below this value |

### Measurement Phase UI

```
┌─────────────────────────────────────────────────────────────┐
│  [Header: Polar Alignment - Measuring]           [Stop]     │
├────────────────────────────────────┬────────────────────────┤
│                                    │  Progress              │
│                                    │  ────────────────────  │
│     Live Camera Image              │  ● Point 1  ✓         │
│     (stretched, plate solved)      │  ● Point 2  ◐ Solving…│
│                                    │  ○ Point 3            │
│     Overlay: Solved coordinates    │                        │
│     RA: 02h 31m 49s                │  Status:              │
│     Dec: +89° 15' 51"              │  Plate solving...     │
│                                    │                        │
│                                    │  Mount:               │
│                                    │  Slewing to point 2   │
│                                    │                        │
└────────────────────────────────────┴────────────────────────┘
```

- Main panel shows current/most recent captured image (each new image replaces previous)
- Solved RA/Dec overlaid on image
- Right panel shows progress checklist and current status
- Status updates: "Exposing...", "Plate solving...", "Slewing to point 2..."

### Adjustment Phase UI

```
┌─────────────────────────────────────────────────────────────┐
│  [Header: Polar Alignment - Adjusting]      [Done]  [Stop]  │
├────────────────────────────────────┬────────────────────────┤
│                                    │                        │
│     Live Camera Image              │   Azimuth              │
│     (updating every few seconds)   │   Left 2.3'            │
│                                    │                        │
│         ╭───────────╮              │   Altitude             │
│         │     ○     │  ← Bullseye  │   Up 1.1'              │
│         │   ──┼──   │    overlay   │                        │
│         │     │     │              │   ────────────────     │
│         ╰───────────╯              │   Total Error          │
│              ●  ← Current position │   2.5'                 │
│                                    │                        │
│                                    │   Threshold: 0.5'      │
│                                    │   ████████░░ 80%       │
│                                    │                        │
└────────────────────────────────────┴────────────────────────┘
```

- Live camera feed with bullseye overlay (centered on pole)
- Red dot shows current alignment position, moves as user adjusts
- Right panel: Clear "Left/Right/Up/Down" directions with magnitude in arcminutes
- Progress bar toward auto-complete threshold
- "Done" button always visible to accept current alignment
- Image refreshes every 2-3 seconds during adjustment

**Direction format:** "Az: Left 2.3'" / "Alt: Up 1.1'" (no arrows, plain text)

**Color coding for total error:**
- Green: < 1'
- Yellow: 1-3'
- Red: > 3'

### Completion Behavior

- Auto-complete when error drops below threshold (default 30") and stays there for ~3 seconds
- Manual "Done" button always available to accept current alignment
- Threshold configurable in Advanced settings
- High default (30") ensures it doesn't surprise users chasing perfection

### Image Pipeline

```
Camera RAW (FITS, CR2, ARW, NEF, etc.) - mono or color
    ↓
Rust imaging crate
    - LibRaw for vendor RAWs
    - FITS reader for FITS
    - Debayer (color) or direct (mono)
    ↓
Stretch for display (existing stretch.rs)
    ↓
Encode to JPEG (for transport to UI only)
    ↓
Send bytes to Dart via event
    ↓
Dart displays with Image.memory() + bullseye overlay
```

- Plate solver receives original processed image data (not the JPEG)
- UI preview gets JPEG-encoded stretched image
- Handle mono frames (no debayer) and color frames (debayer) appropriately

---

## Files to Modify

| File | Changes |
|------|---------|
| `native/nightshade_native/sequencer/src/polar_align.rs` | Add image emission, auto-complete threshold, default step size 15° |
| `native/nightshade_native/sequencer/src/lib.rs` | Export new config fields |
| `native/nightshade_native/bridge/src/api.rs` | Add polar alignment image event |
| `packages/nightshade_core/lib/src/models/sequence/sequence_models.dart` | Update PolarAlignmentNode with new fields |
| `packages/nightshade_app/lib/screens/polar_alignment/polar_alignment_screen.dart` | Complete UI rewrite |

## What Stays the Same

- Core 3-point algorithm (already correct)
- Center of rotation calculation
- Error calculation math
- DeviceOps integration for camera/mount/plate solve
