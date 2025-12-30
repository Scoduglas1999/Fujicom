# Planetarium Phase 3: Visual Polish Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Elevate the planetarium from functional to beautiful with enhanced star rendering, refined constellation lines, improved grids, and label collision avoidance.

**Architecture:** Modify existing rendering methods in SkyCanvasPainter to improve visual quality while maintaining performance through quality tier settings.

**Tech Stack:** Flutter CustomPainter, existing render quality config

**Design Doc:** `docs/plans/2025-12-29-planetarium-overhaul-design.md`

---

## Task 1: Enhance Star Magnitude-to-Size Scaling

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/rendering/sky_renderer.dart`

**Step 1: Improve magnitude to radius formula**

Find the star rendering section (~line 962) and update the size calculation:

```dart
// Enhanced magnitude-to-size scaling
// Brighter stars "pop" more with exponential scaling
double _calculateStarRadius(double magnitude, double fov) {
  // Base radius calculation with exponential curve for bright stars
  double baseRadius;
  if (magnitude < 0) {
    // Very bright stars (Sirius, Canopus, etc.) - exponential boost
    baseRadius = 6.0 + (0 - magnitude) * 2.5;
  } else if (magnitude < 2) {
    // Bright stars - significant boost
    baseRadius = 3.0 + (2 - magnitude) * 1.5;
  } else if (magnitude < 4) {
    // Medium stars - moderate scaling
    baseRadius = 1.5 + (4 - magnitude) * 0.75;
  } else {
    // Faint stars - small but visible
    baseRadius = max(0.5, (6.5 - magnitude) * 0.3);
  }

  // Scale with zoom level (stars appear larger when zoomed in)
  final zoomFactor = (90 / fov).clamp(0.8, 2.0);

  return (baseRadius * zoomFactor).clamp(0.5, 25.0);
}
```

**Step 2: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`

**Step 3: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/rendering/sky_renderer.dart
git commit -m "feat(planetarium): enhance star magnitude-to-size scaling"
```

---

## Task 2: Boost Star Color Saturation for Bright Stars

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/rendering/sky_renderer.dart`

**Step 1: Enhance color saturation for bright stars**

Find the star color calculation and boost saturation:

```dart
Color _getEnhancedStarColor(Star star) {
  final baseColor = star.getStarColor(); // Existing method

  // Boost saturation for bright stars (mag < 2)
  if (star.magnitude < 2) {
    final hsl = HSLColor.fromColor(baseColor);
    final boostFactor = ((2 - star.magnitude) / 4).clamp(0.0, 0.5);
    final boostedSaturation = (hsl.saturation + boostFactor).clamp(0.0, 1.0);

    return hsl
      .withSaturation(boostedSaturation)
      .withLightness((hsl.lightness * 1.1).clamp(0.0, 1.0))
      .toColor();
  }

  return baseColor;
}
```

**Step 2: Apply enhanced colors in star rendering**

Update the star drawing to use `_getEnhancedStarColor`:

```dart
final starColor = _getEnhancedStarColor(star);
```

**Step 3: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`

**Step 4: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/rendering/sky_renderer.dart
git commit -m "feat(planetarium): boost color saturation for bright stars"
```

---

## Task 3: Refine Constellation Lines

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/rendering/sky_renderer.dart`

**Step 1: Improve constellation line rendering**

Find `_drawConstellations` (~line 918) and enhance:

```dart
void _drawConstellationLines(Canvas canvas, Size size, Offset center, double scale) {
  for (final constellation in constellations) {
    for (final line in constellation.lines) {
      final start = _celestialToScreen(line.start, center, scale);
      final end = _celestialToScreen(line.end, center, scale);

      if (start == null || end == null) continue;
      if (!_isInView(start, size) && !_isInView(end, size)) continue;

      // Calculate gradient from start to end
      final gradient = LinearGradient(
        colors: [
          Colors.white.withValues(alpha: 0.35),
          Colors.white.withValues(alpha: 0.25),
          Colors.white.withValues(alpha: 0.35),
        ],
        stops: const [0.0, 0.5, 1.0],
      );

      final paint = Paint()
        ..shader = gradient.createShader(Rect.fromPoints(start, end))
        ..strokeWidth = 1.5  // Increased from 1.0
        ..strokeCap = StrokeCap.round  // Smooth anti-aliased caps
        ..strokeJoin = StrokeJoin.round
        ..style = PaintingStyle.stroke;

      canvas.drawLine(start, end, paint);
    }
  }
}
```

**Step 2: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`

**Step 3: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/rendering/sky_renderer.dart
git commit -m "feat(planetarium): refine constellation lines with gradients and rounded caps"
```

---

## Task 4: Add Adaptive Grid Spacing

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/rendering/sky_renderer.dart`

**Step 1: Implement adaptive grid density**

Find `_drawGrid` (~line 484) and add adaptive spacing:

```dart
void _drawGrid(Canvas canvas, Size size, Offset center, double scale) {
  if (!config.showGrid) return;

  final fov = viewState.fieldOfView;

  // Adaptive grid spacing based on FOV
  double raSpacing;   // hours
  double decSpacing;  // degrees

  if (fov > 60) {
    raSpacing = 2.0;   // Every 2 hours (30°)
    decSpacing = 30.0; // Every 30°
  } else if (fov > 30) {
    raSpacing = 1.0;   // Every 1 hour (15°)
    decSpacing = 15.0; // Every 15°
  } else if (fov > 10) {
    raSpacing = 0.5;   // Every 30 min (7.5°)
    decSpacing = 10.0; // Every 10°
  } else {
    raSpacing = 0.25;  // Every 15 min (3.75°)
    decSpacing = 5.0;  // Every 5°
  }

  _drawGridLines(canvas, size, center, scale, raSpacing, decSpacing);
}
```

**Step 2: Add grid labels at intersections**

```dart
void _drawGridLabels(Canvas canvas, Offset intersection, double ra, double dec) {
  // Only show labels when zoomed out (less clutter)
  if (viewState.fieldOfView < 30) return;

  final raLabel = '${ra.floor()}h';
  final decLabel = '${dec.toInt()}°';

  final textStyle = TextStyle(
    color: Colors.white.withValues(alpha: 0.4),
    fontSize: 9,
    fontWeight: FontWeight.w300,
  );

  final textPainter = TextPainter(
    text: TextSpan(text: '$raLabel $decLabel', style: textStyle),
    textDirection: TextDirection.ltr,
  );
  textPainter.layout();
  textPainter.paint(canvas, intersection + const Offset(3, 3));
}
```

**Step 3: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`

**Step 4: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/rendering/sky_renderer.dart
git commit -m "feat(planetarium): add adaptive grid spacing based on FOV"
```

---

## Task 5: Add Zenith Marker

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/rendering/sky_renderer.dart`

**Step 1: Add zenith marker rendering**

Add a method to draw a subtle zenith indicator:

```dart
void _drawZenithMarker(Canvas canvas, Size size, Offset center, double scale) {
  // Calculate zenith position (altitude 90°)
  final location = observerLocation;
  final time = observationTime;
  final lst = AstronomyCalculations.localSiderealTime(time, location.longitude);

  final (ra, dec) = AstronomyCalculations.horizontalToEquatorial(
    altDeg: 90.0,
    azDeg: 0.0,
    latitudeDeg: location.latitude,
    lstHours: lst,
  );

  final zenithPos = _celestialToScreen(
    CelestialCoordinate(ra: ra / 15, dec: dec),
    center,
    scale,
  );

  if (zenithPos == null || !_isInView(zenithPos, size)) return;

  // Draw subtle crosshair at zenith
  final paint = Paint()
    ..color = Colors.white.withValues(alpha: 0.4)
    ..strokeWidth = 1.0
    ..style = PaintingStyle.stroke;

  final length = 12.0;
  canvas.drawLine(
    zenithPos - Offset(length, 0),
    zenithPos + Offset(length, 0),
    paint,
  );
  canvas.drawLine(
    zenithPos - Offset(0, length),
    zenithPos + Offset(0, length),
    paint,
  );

  // Draw "Z" label
  final textPainter = TextPainter(
    text: TextSpan(
      text: 'Z',
      style: TextStyle(
        color: Colors.white.withValues(alpha: 0.5),
        fontSize: 10,
        fontWeight: FontWeight.bold,
      ),
    ),
    textDirection: TextDirection.ltr,
  );
  textPainter.layout();
  textPainter.paint(canvas, zenithPos + const Offset(8, -14));
}
```

**Step 2: Call from paint method**

Add call after grid drawing:

```dart
// Draw zenith marker
if (config.showGrid) {
  _drawZenithMarker(canvas, size, center, scale);
}
```

**Step 3: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`

**Step 4: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/rendering/sky_renderer.dart
git commit -m "feat(planetarium): add zenith marker with crosshair"
```

---

## Task 6: Add Meridian Line

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/rendering/sky_renderer.dart`
- Modify: `packages/nightshade_planetarium/lib/src/rendering/sky_renderer.dart` (SkyRenderConfig)

**Step 1: Add showMeridian config option**

In SkyRenderConfig class:

```dart
final bool showMeridian;

// In constructor
this.showMeridian = false,

// In copyWith
bool? showMeridian,
// ...
showMeridian: showMeridian ?? this.showMeridian,
```

**Step 2: Implement meridian line rendering**

```dart
void _drawMeridianLine(Canvas canvas, Size size, Offset center, double scale) {
  if (!config.showMeridian) return;

  final location = observerLocation;
  final time = observationTime;
  final lst = AstronomyCalculations.localSiderealTime(time, location.longitude);

  // Meridian is at azimuth 0° (north) and 180° (south)
  // Draw from horizon to zenith through both
  final path = Path();
  var firstPoint = true;

  for (var alt = 0.0; alt <= 90; alt += 2) {
    // North meridian (az = 0)
    final (ra, dec) = AstronomyCalculations.horizontalToEquatorial(
      altDeg: alt,
      azDeg: 0.0,
      latitudeDeg: location.latitude,
      lstHours: lst,
    );

    final pos = _celestialToScreen(
      CelestialCoordinate(ra: ra / 15, dec: dec),
      center,
      scale,
    );

    if (pos != null && _isInView(pos, size)) {
      if (firstPoint) {
        path.moveTo(pos.dx, pos.dy);
        firstPoint = false;
      } else {
        path.lineTo(pos.dx, pos.dy);
      }
    }
  }

  final paint = Paint()
    ..color = Colors.green.withValues(alpha: 0.4)
    ..strokeWidth = 1.5
    ..style = PaintingStyle.stroke
    ..strokeCap = StrokeCap.round;

  canvas.drawPath(path, paint);

  // Label
  final textPainter = TextPainter(
    text: TextSpan(
      text: 'MERIDIAN',
      style: TextStyle(
        color: Colors.green.withValues(alpha: 0.5),
        fontSize: 9,
        fontWeight: FontWeight.w500,
      ),
    ),
    textDirection: TextDirection.ltr,
  );
  textPainter.layout();
  // Position label at top of meridian line
}
```

**Step 3: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`

**Step 4: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/rendering/sky_renderer.dart
git commit -m "feat(planetarium): add optional meridian line for imaging timing"
```

---

## Task 7: Implement Label Collision Avoidance

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/rendering/sky_renderer.dart`

**Step 1: Add label position tracking**

Create a system to track rendered label positions:

```dart
/// Tracks rendered label bounding boxes to avoid overlap
class LabelLayoutManager {
  final List<Rect> _renderedLabels = [];

  void clear() => _renderedLabels.clear();

  /// Returns true if label can be placed without overlap
  bool canPlace(Rect labelRect) {
    // Add padding between labels
    final paddedRect = labelRect.inflate(2);

    for (final existing in _renderedLabels) {
      if (paddedRect.overlaps(existing)) {
        return false;
      }
    }
    return true;
  }

  /// Try to find a non-overlapping position for label
  /// Returns adjusted offset or null if no good position found
  Offset? findPlacement(Offset preferredPos, Size labelSize, Size canvasSize) {
    final offsets = [
      preferredPos,
      preferredPos + const Offset(0, -15),  // Above
      preferredPos + const Offset(15, 0),   // Right
      preferredPos + const Offset(-15, 0),  // Left
      preferredPos + const Offset(0, 15),   // Below
    ];

    for (final offset in offsets) {
      final rect = Rect.fromLTWH(offset.dx, offset.dy, labelSize.width, labelSize.height);
      if (canPlace(rect) && _isInBounds(rect, canvasSize)) {
        _renderedLabels.add(rect);
        return offset;
      }
    }

    return null; // Cannot place without overlap
  }

  bool _isInBounds(Rect rect, Size canvasSize) {
    return rect.left >= 0 &&
           rect.top >= 0 &&
           rect.right <= canvasSize.width &&
           rect.bottom <= canvasSize.height;
  }
}
```

**Step 2: Use layout manager when drawing labels**

```dart
// At start of paint()
_labelManager.clear();

// When drawing star label
final preferredPos = starOffset + Offset(radius + 3, -height / 2);
final labelPos = _labelManager.findPlacement(preferredPos, Size(width, height), size);
if (labelPos != null) {
  textPainter.paint(canvas, labelPos);
}
```

**Step 3: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`

**Step 4: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/rendering/sky_renderer.dart
git commit -m "feat(planetarium): implement label collision avoidance"
```

---

## Task 8: Add Label Size Hierarchy

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/rendering/sky_renderer.dart`

**Step 1: Implement brightness-based label sizing**

```dart
double _getLabelFontSize(double magnitude, String objectType) {
  // Base sizes by object type
  double baseSize;
  switch (objectType) {
    case 'star':
      if (magnitude < 0) {
        baseSize = 12.0; // Very bright stars (Sirius, etc.)
      } else if (magnitude < 2) {
        baseSize = 11.0; // Bright stars
      } else if (magnitude < 4) {
        baseSize = 10.0; // Medium stars
      } else {
        baseSize = 9.0;  // Faint stars
      }
      break;
    case 'dso':
      if (magnitude < 6) {
        baseSize = 11.0; // Bright DSOs (M31, M42, etc.)
      } else if (magnitude < 9) {
        baseSize = 10.0; // Medium DSOs
      } else {
        baseSize = 9.0;  // Faint DSOs
      }
      break;
    case 'planet':
      baseSize = 12.0; // Planets always prominent
      break;
    default:
      baseSize = 10.0;
  }

  return baseSize;
}

FontWeight _getLabelFontWeight(double magnitude) {
  if (magnitude < 1) return FontWeight.w600;
  if (magnitude < 3) return FontWeight.w500;
  return FontWeight.w400;
}
```

**Step 2: Apply hierarchy to label rendering**

Update star and DSO label rendering to use these methods.

**Step 3: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`

**Step 4: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/rendering/sky_renderer.dart
git commit -m "feat(planetarium): add label size hierarchy based on brightness"
```

---

## Task 9: Integration Test

**Files:**
- None (manual testing)

**Step 1: Build and run**

Run: `melos run dev`

**Step 2: Manual verification checklist**

- [ ] Bright stars appear noticeably larger than dim stars
- [ ] Sirius, Canopus, Vega have saturated colors
- [ ] Constellation lines are 1.5px with gradient effect
- [ ] Line ends are smoothly rounded
- [ ] Grid spacing adapts when zooming in/out
- [ ] Grid labels appear at intersections when zoomed out
- [ ] Zenith marker visible with "Z" label
- [ ] Meridian line visible when enabled
- [ ] Labels don't overlap each other
- [ ] Brighter objects have larger labels

**Step 3: Final commit if fixes needed**

```bash
git add -A
git commit -m "fix(planetarium): polish visual improvements"
```

---

## Summary

This plan implements Phase 3 Visual Polish with:
1. Enhanced star size scaling (brighter stars pop more)
2. Boosted color saturation for bright stars
3. Refined constellation lines (1.5px, gradients, rounded caps)
4. Adaptive grid spacing based on FOV
5. Zenith marker with crosshair
6. Optional meridian line for imaging
7. Label collision avoidance
8. Label size hierarchy based on brightness
