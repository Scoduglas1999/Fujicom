# Planetarium Overhaul Design

**Date:** 2025-12-29
**Status:** Approved
**Goal:** Transform the planetarium from functional to production-ready with improved orientation, visual polish, and cross-platform optimization.

## Problem Statement

The current planetarium feels "unfinished" with a core usability issue: **spatial disorientation**. Users describe it as being "inside a goldfish bowl rolling it around from the inside" - there are no persistent visual references to maintain spatial awareness while panning.

Specific issues:
1. Hard to know which way is "up" (toward zenith vs horizon)
2. Difficult to quickly assess if an object is above/below the horizon
3. After panning, users lose track of cardinal directions
4. Some catalog objects aren't clickable or visible
5. Visual elements don't feel refined enough for a commercial product

## Design Overview

### Three-Layer Orientation System

**Layer 1: Ground Plane with Horizon Gradient**
- Replace simple dark fill with proper ground gradient (dark earth brown → muted green → horizon glow)
- Optional horizon silhouette overlay (simple mountain/tree line or observatory dome)
- Subtle color variation for texture without distraction
- Configurable: users can load custom horizon profiles for their actual location

**Layer 2: Compass Rose + Altitude HUD (Corner Overlay)**
- Small semi-transparent compass in bottom-left showing current azimuth bearing
- Altitude arc indicator showing current view angle (0° horizon → 90° zenith)
- Updates in real-time as user pans
- Minimal footprint, high information density

**Layer 3: All-Sky Mini-Map**
- Fisheye projection in corner (horizon at edge, zenith at center)
- Shows: horizon circle, cardinal directions, current FOV as a wedge
- Optional: show bright objects as dots on mini-map
- Tap on mini-map to quickly jump to that part of the sky
- Toggle visibility via toolbar or keyboard shortcut

### Dynamic Object Loading System

**Zoom-Aware Magnitude Limits:**

| FOV | Star Mag Limit | DSO Mag Limit | Approx Visible Objects |
|-----|----------------|---------------|------------------------|
| 90°+ | 4.5 | 8.0 | ~500 stars, ~200 DSOs |
| 60° | 5.5 | 10.0 | ~2,000 stars, ~500 DSOs |
| 30° | 6.5 | 12.0 | ~8,000 stars, ~2,000 DSOs |
| 15° | 7.5 | 14.0 | ~20,000 stars, ~5,000 DSOs |
| 5° | 9.0 | 16.0 | Full detail in viewport |

**Pop-In Behavior:**
- Newly visible objects fade in over 200-300ms
- Slight scale animation (start at 80%, grow to 100%)
- Prioritize brighter objects appearing first
- All visible objects must be clickable

**Visual Density Indicator:**
- Subtle glow on crowded regions when zoomed out
- Tooltip: "Zoom in to reveal X objects in this area"

**Filtering Panel (Collapsible Sidebar):**
- Toggle categories: Stars / Galaxies / Nebulae / Clusters / Planets
- Magnitude slider: "Show objects brighter than X"
- Named objects only toggle
- "Imaging targets" filter: objects suitable for current equipment FOV

### Visual Polish Improvements

**Stars:**
- Improved magnitude-to-size scaling (brighter stars "pop" more)
- Boosted color saturation for brightest stars
- Existing PSF, diffraction spikes, and twinkle retained

**Constellation Lines:**
- Increase thickness to 1.5px (from 1px)
- Subtle gradient along line length
- Smooth anti-aliased line caps

**Grid & Reference Lines:**
- Adaptive grid spacing based on zoom level
- RA/Dec labels at intersections when zoomed out
- Zenith marker (subtle "Z" or crosshair)
- Optional meridian line (important for imaging timing)

**Typography & Labels:**
- Size hierarchy based on object brightness
- Simple collision avoidance to prevent overlap
- Consider refined font choice

### UI Controls & Object Interaction

**Enhanced Object Popup:**
- DSS/survey thumbnail image for DSOs
- Quick stats bar: altitude, time until transit, moon distance
- Visibility indicator: green (excellent) / yellow (fair) / red (poor)
- "Why this rating" tooltip explaining factors

**Search Improvements:**
- Instant results as you type (max 8)
- Category grouping: Stars | DSOs | Constellations | Coordinates
- Accept coordinate input: "RA 5h 35m, Dec -5° 23'"
- Recent searches (last 5)
- Prioritize objects near current view center

**Keyboard Shortcuts (Desktop):**

| Key | Action |
|-----|--------|
| Arrow keys | Pan view |
| +/- or scroll | Zoom |
| R | Reset to default view |
| G | Toggle grid |
| C | Toggle constellation lines |
| M | Toggle mini-map |
| F | Center on mount position |
| Esc | Clear selection / exit slew mode |

### Cross-Platform Adaptations

**Desktop (Windows/macOS/Linux):**
- Full keyboard shortcuts
- Right-click context menu on objects
- Hover tooltips before clicking
- Sidebar filter panel always visible

**Tablet (iPad/Android Tablet):**
- Two-finger pinch zoom
- Swipe from right edge for filter panel
- Larger touch targets
- Slightly larger mini-map

**Phone (iOS/Android):**
- Bottom sheet for filter panel (not sidebar)
- Mini-map optional
- Condensed compass HUD
- Object popup as bottom card
- "Shake to reset view" option

### Performance Tiers

| Feature | Performance | Balanced | Quality |
|---------|-------------|----------|---------|
| Max stars | 2,000 | 10,000 | 120,000 |
| Star glow/PSF | None | Gradient | Airy disk + spikes |
| Milky Way | Off | Low detail | Full detail |
| DSO symbols | Simple | Standard | Enhanced |
| Ground plane | Solid color | Gradient | Gradient + silhouette |
| Mini-map | Off | On | On + object dots |
| Pop-in animation | Instant | Fade | Fade + scale |

**Auto-detection:** Measure frame time on first render, auto-downgrade if consistently below 30fps.

## Implementation Phases

### Phase 1: Orientation System (Highest Priority)
*Solves the core "goldfish bowl" problem*
1. Ground plane with horizon gradient
2. Compass rose + altitude HUD overlay
3. All-sky mini-map with FOV indicator

### Phase 2: Dynamic Object Loading
*Makes the planetarium feel alive and responsive*
1. Zoom-aware magnitude limits
2. Smooth pop-in animations
3. Ensure all catalog objects are clickable when visible
4. Visual density indicators for crowded regions

### Phase 3: Visual Polish
*Elevates from functional to beautiful*
1. Enhanced star rendering (better size scaling, color saturation)
2. Refined constellation lines
3. Grid improvements (adaptive density, labels, zenith/meridian markers)
4. Label collision avoidance

### Phase 4: UI/UX Improvements
*Production-ready interaction design*
1. Enriched object popup with thumbnails and visibility scores
2. Collapsible filter sidebar
3. Improved search with instant results
4. Keyboard shortcuts (desktop)

### Phase 5: Cross-Platform Optimization
*Consistent experience everywhere*
1. Platform-adaptive layouts
2. Performance auto-detection and tier switching
3. Touch gesture refinements per platform

## Files to Modify

| Change | Location |
|--------|----------|
| Ground plane rendering | `packages/nightshade_planetarium/lib/src/rendering/sky_renderer.dart` |
| Compass/HUD overlay | New widget in `packages/nightshade_planetarium/lib/src/widgets/` |
| Mini-map widget | New widget in `packages/nightshade_planetarium/lib/src/widgets/` |
| Dynamic magnitude limits | `packages/nightshade_planetarium/lib/src/providers/planetarium_providers.dart` |
| Pop-in animation | `packages/nightshade_planetarium/lib/src/widgets/interactive_sky_view.dart` |
| Filter sidebar | `packages/nightshade_app/lib/screens/planetarium/planetarium_screen.dart` |
| Object popup enhancements | `packages/nightshade_planetarium/lib/src/widgets/object_details_panel.dart` |
| Search improvements | `packages/nightshade_app/lib/screens/planetarium/planetarium_screen.dart` |
| Keyboard shortcuts | `packages/nightshade_app/lib/screens/planetarium/planetarium_screen.dart` |
| Render quality config | `packages/nightshade_planetarium/lib/src/rendering/render_quality.dart` |

## Success Criteria

1. Users can immediately identify horizon direction when panning
2. Current altitude/azimuth always visible in HUD
3. Mini-map provides instant "where am I looking" context
4. Zooming reveals appropriate detail level smoothly
5. All catalog objects are tappable when rendered
6. Consistent 30+ fps on balanced quality tier
7. Works intuitively on desktop, tablet, and phone
