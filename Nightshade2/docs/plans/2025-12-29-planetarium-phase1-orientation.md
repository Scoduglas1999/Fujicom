# Planetarium Phase 1: Orientation System Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add a three-layer orientation system (ground plane, compass HUD, mini-map) to solve spatial disorientation in the planetarium.

**Architecture:** Three independent overlay components that work together: (1) Ground plane rendered in SkyCanvasPainter below the horizon, (2) Compass/altitude HUD as a Flutter widget overlay, (3) All-sky mini-map as a separate CustomPainter widget. Each can be toggled independently.

**Tech Stack:** Flutter CustomPainter, Riverpod providers, existing astronomy calculations from `nightshade_planetarium`

**Design Doc:** `docs/plans/2025-12-29-planetarium-overhaul-design.md`

---

## Task 1: Add Ground Plane Configuration to Render Config

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/rendering/sky_renderer.dart:22-124` (SkyRenderConfig)
- Modify: `packages/nightshade_planetarium/lib/src/rendering/render_quality.dart` (RenderQualityConfig)

**Step 1: Add ground plane fields to SkyRenderConfig**

In `sky_renderer.dart`, add these fields to the `SkyRenderConfig` class:

```dart
// Add after line 45 (after showPlanets field)
final bool showGroundPlane;
final Color groundColorDark;
final Color groundColorLight;
final Color horizonGlowColor;
```

Add to constructor (after `this.showPlanets = true,`):

```dart
this.showGroundPlane = true,
this.groundColorDark = const Color(0xFF0A0805),
this.groundColorLight = const Color(0xFF1A1510),
this.horizonGlowColor = const Color(0xFF2A2015),
```

Add to copyWith method:

```dart
bool? showGroundPlane,
Color? groundColorDark,
Color? groundColorLight,
Color? horizonGlowColor,
```

And in the return statement:

```dart
showGroundPlane: showGroundPlane ?? this.showGroundPlane,
groundColorDark: groundColorDark ?? this.groundColorDark,
groundColorLight: groundColorLight ?? this.groundColorLight,
horizonGlowColor: horizonGlowColor ?? this.horizonGlowColor,
```

**Step 2: Add ground plane detail level to RenderQualityConfig**

In `render_quality.dart`, add to RenderQualityConfig class:

```dart
/// Ground plane detail: 0 = solid color, 0.5 = gradient, 1.0 = gradient + silhouette
final double groundPlaneDetail;
```

Add to constructors:
- `RenderQualityConfig.performance()`: `groundPlaneDetail = 0.0`
- `RenderQualityConfig.balanced()`: `groundPlaneDetail = 0.5`
- `RenderQualityConfig.quality()`: `groundPlaneDetail = 1.0`

**Step 3: Run analyzer to verify no errors**

Run: `cd packages/nightshade_planetarium && flutter analyze`
Expected: No errors related to new fields

**Step 4: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/rendering/sky_renderer.dart packages/nightshade_planetarium/lib/src/rendering/render_quality.dart
git commit -m "feat(planetarium): add ground plane configuration options"
```

---

## Task 2: Implement Ground Plane Renderer

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/rendering/sky_renderer.dart` (add _drawGroundPlane method)

**Step 1: Add _drawGroundPlane method**

Add this method after the `_drawHorizon` method (around line 652):

```dart
/// Draw ground plane below the horizon with gradient
void _drawGroundPlane(Canvas canvas, Size size, Offset center, double scale) {
  if (!config.showGroundPlane) return;

  final lst = AstronomyCalculations.localSiderealTime(observationTime, longitude);

  // Build path for area below horizon
  final groundPath = Path();
  var firstPoint = true;
  final horizonPoints = <Offset>[];

  for (var az = 0.0; az <= 360; az += 2) {
    final (ra, dec) = AstronomyCalculations.horizontalToEquatorial(
      altDeg: 0,
      azDeg: az,
      latitudeDeg: latitude,
      lstHours: lst,
    );

    final offset = _celestialToScreen(
      CelestialCoordinate(ra: ra / 15, dec: dec),
      center,
      scale,
    );

    if (offset != null && _isInView(offset, size)) {
      horizonPoints.add(offset);
      if (firstPoint) {
        groundPath.moveTo(offset.dx, offset.dy);
        firstPoint = false;
      } else {
        groundPath.lineTo(offset.dx, offset.dy);
      }
    }
  }

  if (horizonPoints.isEmpty) return;

  // Close the path by extending to screen edges below horizon
  // This creates a filled region for the ground
  groundPath.lineTo(size.width, size.height);
  groundPath.lineTo(0, size.height);
  groundPath.close();

  // Draw based on quality setting
  if (qualityConfig.groundPlaneDetail <= 0.0) {
    // Performance: solid dark color
    final paint = Paint()
      ..color = config.groundColorDark
      ..style = PaintingStyle.fill;
    canvas.drawPath(groundPath, paint);
  } else {
    // Balanced/Quality: gradient from horizon down
    final gradient = LinearGradient(
      begin: Alignment.topCenter,
      end: Alignment.bottomCenter,
      colors: [
        config.horizonGlowColor,
        config.groundColorLight,
        config.groundColorDark,
      ],
      stops: const [0.0, 0.3, 1.0],
    );

    final paint = Paint()
      ..shader = gradient.createShader(Rect.fromLTWH(0, 0, size.width, size.height))
      ..style = PaintingStyle.fill;
    canvas.drawPath(groundPath, paint);

    // Quality mode: add subtle horizon glow line
    if (qualityConfig.groundPlaneDetail >= 1.0 && horizonPoints.length > 2) {
      final glowPaint = Paint()
        ..color = config.horizonGlowColor.withValues(alpha: 0.4)
        ..strokeWidth = 4
        ..style = PaintingStyle.stroke
        ..strokeCap = StrokeCap.round;

      if (qualityConfig.useBlurEffects) {
        glowPaint.maskFilter = const MaskFilter.blur(BlurStyle.normal, 6);
      }

      final glowPath = Path();
      glowPath.moveTo(horizonPoints.first.dx, horizonPoints.first.dy);
      for (final pt in horizonPoints.skip(1)) {
        glowPath.lineTo(pt.dx, pt.dy);
      }
      canvas.drawPath(glowPath, glowPaint);
    }
  }
}
```

**Step 2: Call _drawGroundPlane in paint method**

In the `paint()` method, add this call right BEFORE `_drawHorizon`:

```dart
// Draw ground plane (before horizon line so horizon draws on top)
if (config.showGroundPlane) {
  _drawGroundPlane(canvas, size, center, scale);
}
```

This should be around line 253, before the existing horizon drawing block.

**Step 3: Run analyzer and quick visual test**

Run: `cd packages/nightshade_planetarium && flutter analyze`
Expected: No errors

**Step 4: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/rendering/sky_renderer.dart
git commit -m "feat(planetarium): implement ground plane renderer with gradient"
```

---

## Task 3: Create Compass HUD Widget

**Files:**
- Create: `packages/nightshade_planetarium/lib/src/widgets/compass_hud.dart`
- Modify: `packages/nightshade_planetarium/lib/nightshade_planetarium.dart` (export)

**Step 1: Create compass_hud.dart**

```dart
import 'dart:math' as math;
import 'package:flutter/material.dart';

/// Compass and altitude HUD overlay for planetarium orientation
class CompassHud extends StatelessWidget {
  /// Current azimuth in degrees (0 = North, 90 = East)
  final double azimuth;

  /// Current altitude in degrees (0 = horizon, 90 = zenith)
  final double altitude;

  /// Size of the compass widget
  final double size;

  /// Whether to show the altitude arc
  final bool showAltitude;

  const CompassHud({
    super.key,
    required this.azimuth,
    required this.altitude,
    this.size = 80,
    this.showAltitude = true,
  });

  @override
  Widget build(BuildContext context) {
    return SizedBox(
      width: size + (showAltitude ? 40 : 0),
      height: size,
      child: CustomPaint(
        painter: _CompassHudPainter(
          azimuth: azimuth,
          altitude: altitude,
          showAltitude: showAltitude,
        ),
      ),
    );
  }
}

class _CompassHudPainter extends CustomPainter {
  final double azimuth;
  final double altitude;
  final bool showAltitude;

  _CompassHudPainter({
    required this.azimuth,
    required this.altitude,
    required this.showAltitude,
  });

  @override
  void paint(Canvas canvas, Size size) {
    final compassRadius = size.height / 2 - 4;
    final compassCenter = Offset(compassRadius + 4, size.height / 2);

    // Background circle
    final bgPaint = Paint()
      ..color = Colors.black.withValues(alpha: 0.6)
      ..style = PaintingStyle.fill;
    canvas.drawCircle(compassCenter, compassRadius, bgPaint);

    // Outer ring
    final ringPaint = Paint()
      ..color = Colors.white.withValues(alpha: 0.3)
      ..style = PaintingStyle.stroke
      ..strokeWidth = 1.5;
    canvas.drawCircle(compassCenter, compassRadius, ringPaint);

    // Cardinal direction markers
    _drawCardinalMarkers(canvas, compassCenter, compassRadius);

    // Direction indicator (triangle pointing to current azimuth)
    _drawDirectionIndicator(canvas, compassCenter, compassRadius);

    // Azimuth text
    _drawAzimuthText(canvas, compassCenter);

    // Altitude arc (if enabled)
    if (showAltitude) {
      _drawAltitudeArc(canvas, size, compassCenter, compassRadius);
    }
  }

  void _drawCardinalMarkers(Canvas canvas, Offset center, double radius) {
    final textStyle = TextStyle(
      color: Colors.white.withValues(alpha: 0.8),
      fontSize: 10,
      fontWeight: FontWeight.bold,
    );

    final directions = ['N', 'E', 'S', 'W'];
    final angles = [0.0, 90.0, 180.0, 270.0];

    for (var i = 0; i < 4; i++) {
      // Rotate based on current azimuth so N always points to actual north
      final angle = (angles[i] - azimuth) * math.pi / 180 - math.pi / 2;
      final markerRadius = radius - 12;

      final x = center.dx + math.cos(angle) * markerRadius;
      final y = center.dy + math.sin(angle) * markerRadius;

      final textPainter = TextPainter(
        text: TextSpan(
          text: directions[i],
          style: directions[i] == 'N'
              ? textStyle.copyWith(color: Colors.red.shade300)
              : textStyle,
        ),
        textDirection: TextDirection.ltr,
      );
      textPainter.layout();
      textPainter.paint(
        canvas,
        Offset(x - textPainter.width / 2, y - textPainter.height / 2),
      );

      // Tick marks for intercardinal directions
      final tickAngle = (angles[i] + 45 - azimuth) * math.pi / 180 - math.pi / 2;
      final tickStart = Offset(
        center.dx + math.cos(tickAngle) * (radius - 4),
        center.dy + math.sin(tickAngle) * (radius - 4),
      );
      final tickEnd = Offset(
        center.dx + math.cos(tickAngle) * (radius - 8),
        center.dy + math.sin(tickAngle) * (radius - 8),
      );

      final tickPaint = Paint()
        ..color = Colors.white.withValues(alpha: 0.4)
        ..strokeWidth = 1;
      canvas.drawLine(tickStart, tickEnd, tickPaint);
    }
  }

  void _drawDirectionIndicator(Canvas canvas, Offset center, double radius) {
    // Fixed triangle at top (current viewing direction)
    final indicatorPaint = Paint()
      ..color = Colors.white
      ..style = PaintingStyle.fill;

    final triangleSize = 6.0;
    final topY = center.dy - radius + 2;

    final path = Path()
      ..moveTo(center.dx, topY)
      ..lineTo(center.dx - triangleSize, topY + triangleSize * 1.5)
      ..lineTo(center.dx + triangleSize, topY + triangleSize * 1.5)
      ..close();

    canvas.drawPath(path, indicatorPaint);
  }

  void _drawAzimuthText(Canvas canvas, Offset center) {
    final azText = '${azimuth.round()}°';
    final textPainter = TextPainter(
      text: TextSpan(
        text: azText,
        style: TextStyle(
          color: Colors.white.withValues(alpha: 0.9),
          fontSize: 11,
          fontWeight: FontWeight.w600,
        ),
      ),
      textDirection: TextDirection.ltr,
    );
    textPainter.layout();
    textPainter.paint(
      canvas,
      Offset(center.dx - textPainter.width / 2, center.dy - textPainter.height / 2),
    );
  }

  void _drawAltitudeArc(Canvas canvas, Size size, Offset compassCenter, double compassRadius) {
    final arcLeft = compassCenter.dx + compassRadius + 8;
    final arcWidth = 24.0;
    final arcHeight = size.height - 16;
    final arcTop = 8.0;

    // Background arc
    final bgRect = RRect.fromRectAndRadius(
      Rect.fromLTWH(arcLeft, arcTop, arcWidth, arcHeight),
      const Radius.circular(4),
    );
    final bgPaint = Paint()
      ..color = Colors.black.withValues(alpha: 0.6)
      ..style = PaintingStyle.fill;
    canvas.drawRRect(bgRect, bgPaint);

    // Border
    final borderPaint = Paint()
      ..color = Colors.white.withValues(alpha: 0.3)
      ..style = PaintingStyle.stroke
      ..strokeWidth = 1;
    canvas.drawRRect(bgRect, borderPaint);

    // Altitude markers (0, 30, 60, 90)
    final markerStyle = TextStyle(
      color: Colors.white.withValues(alpha: 0.5),
      fontSize: 7,
    );

    for (final alt in [0, 30, 60, 90]) {
      final y = arcTop + arcHeight - (alt / 90 * arcHeight);

      // Tick
      canvas.drawLine(
        Offset(arcLeft, y),
        Offset(arcLeft + 4, y),
        Paint()..color = Colors.white.withValues(alpha: 0.4)..strokeWidth = 1,
      );

      // Label (only 0 and 90 to avoid clutter)
      if (alt == 0 || alt == 90) {
        final textPainter = TextPainter(
          text: TextSpan(text: '$alt°', style: markerStyle),
          textDirection: TextDirection.ltr,
        );
        textPainter.layout();
        textPainter.paint(
          canvas,
          Offset(arcLeft + arcWidth + 2, y - textPainter.height / 2),
        );
      }
    }

    // Current altitude indicator
    final altY = arcTop + arcHeight - (altitude.clamp(0, 90) / 90 * arcHeight);
    final indicatorPaint = Paint()
      ..color = altitude >= 0 ? Colors.green.shade400 : Colors.red.shade400
      ..style = PaintingStyle.fill;

    // Triangle indicator
    final triPath = Path()
      ..moveTo(arcLeft + arcWidth - 2, altY)
      ..lineTo(arcLeft + arcWidth + 4, altY - 4)
      ..lineTo(arcLeft + arcWidth + 4, altY + 4)
      ..close();
    canvas.drawPath(triPath, indicatorPaint);

    // Fill from bottom to current altitude
    final fillRect = Rect.fromLTWH(
      arcLeft + 2,
      altY,
      arcWidth - 4,
      arcTop + arcHeight - altY - 2,
    );
    final fillPaint = Paint()
      ..color = Colors.green.withValues(alpha: 0.2)
      ..style = PaintingStyle.fill;
    canvas.drawRect(fillRect, fillPaint);

    // Altitude value
    final altText = '${altitude.round()}°';
    final altTextPainter = TextPainter(
      text: TextSpan(
        text: altText,
        style: TextStyle(
          color: Colors.white.withValues(alpha: 0.9),
          fontSize: 9,
          fontWeight: FontWeight.w600,
        ),
      ),
      textDirection: TextDirection.ltr,
    );
    altTextPainter.layout();
    altTextPainter.paint(
      canvas,
      Offset(arcLeft + (arcWidth - altTextPainter.width) / 2, altY - 14),
    );
  }

  @override
  bool shouldRepaint(covariant _CompassHudPainter oldDelegate) {
    return azimuth != oldDelegate.azimuth ||
           altitude != oldDelegate.altitude ||
           showAltitude != oldDelegate.showAltitude;
  }
}
```

**Step 2: Export from package**

In `packages/nightshade_planetarium/lib/nightshade_planetarium.dart`, add:

```dart
export 'src/widgets/compass_hud.dart';
```

**Step 3: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`
Expected: No errors

**Step 4: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/widgets/compass_hud.dart packages/nightshade_planetarium/lib/nightshade_planetarium.dart
git commit -m "feat(planetarium): add compass and altitude HUD widget"
```

---

## Task 4: Create All-Sky Mini-Map Widget

**Files:**
- Create: `packages/nightshade_planetarium/lib/src/widgets/sky_minimap.dart`
- Modify: `packages/nightshade_planetarium/lib/nightshade_planetarium.dart` (export)

**Step 1: Create sky_minimap.dart**

```dart
import 'dart:math' as math;
import 'package:flutter/material.dart';
import '../coordinate_system.dart';

/// All-sky mini-map showing current FOV position
/// Uses fisheye projection: horizon at edge, zenith at center
class SkyMinimap extends StatelessWidget {
  /// Current view center in horizontal coordinates
  final double azimuth; // degrees, 0 = North
  final double altitude; // degrees, 0 = horizon

  /// Current field of view in degrees
  final double fieldOfView;

  /// View rotation in degrees
  final double rotation;

  /// Size of the mini-map
  final double size;

  /// Callback when user taps on the mini-map
  final void Function(double azimuth, double altitude)? onTap;

  /// Optional: show cardinal direction labels
  final bool showLabels;

  const SkyMinimap({
    super.key,
    required this.azimuth,
    required this.altitude,
    required this.fieldOfView,
    this.rotation = 0,
    this.size = 100,
    this.onTap,
    this.showLabels = true,
  });

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTapUp: onTap != null ? (details) => _handleTap(details.localPosition) : null,
      child: Container(
        width: size,
        height: size,
        decoration: BoxDecoration(
          shape: BoxShape.circle,
          color: Colors.black.withValues(alpha: 0.7),
          border: Border.all(
            color: Colors.white.withValues(alpha: 0.3),
            width: 1.5,
          ),
        ),
        child: ClipOval(
          child: CustomPaint(
            size: Size(size, size),
            painter: _SkyMinimapPainter(
              azimuth: azimuth,
              altitude: altitude,
              fieldOfView: fieldOfView,
              rotation: rotation,
              showLabels: showLabels,
            ),
          ),
        ),
      ),
    );
  }

  void _handleTap(Offset localPosition) {
    if (onTap == null) return;

    final center = Offset(size / 2, size / 2);
    final dx = localPosition.dx - center.dx;
    final dy = localPosition.dy - center.dy;

    // Convert to polar coordinates
    final distance = math.sqrt(dx * dx + dy * dy);
    final maxRadius = size / 2 - 4;

    if (distance > maxRadius) return; // Outside the map

    // Distance from center = altitude (center = 90°, edge = 0°)
    final tappedAlt = 90 * (1 - distance / maxRadius);

    // Angle = azimuth (0 = up = North)
    var tappedAz = math.atan2(dx, -dy) * 180 / math.pi;
    if (tappedAz < 0) tappedAz += 360;

    onTap!(tappedAz, tappedAlt);
  }
}

class _SkyMinimapPainter extends CustomPainter {
  final double azimuth;
  final double altitude;
  final double fieldOfView;
  final double rotation;
  final bool showLabels;

  _SkyMinimapPainter({
    required this.azimuth,
    required this.altitude,
    required this.fieldOfView,
    required this.rotation,
    required this.showLabels,
  });

  @override
  void paint(Canvas canvas, Size size) {
    final center = Offset(size.width / 2, size.height / 2);
    final radius = size.width / 2 - 4;

    // Draw altitude circles (30°, 60°)
    _drawAltitudeCircles(canvas, center, radius);

    // Draw cardinal directions
    _drawCardinalDirections(canvas, center, radius);

    // Draw horizon (outer edge is already the border)
    // Draw zenith marker
    _drawZenithMarker(canvas, center);

    // Draw current FOV indicator
    _drawFOVIndicator(canvas, center, radius);
  }

  void _drawAltitudeCircles(Canvas canvas, Offset center, double radius) {
    final paint = Paint()
      ..color = Colors.white.withValues(alpha: 0.15)
      ..style = PaintingStyle.stroke
      ..strokeWidth = 0.5;

    // 30° and 60° altitude circles
    for (final alt in [30.0, 60.0]) {
      final circleRadius = radius * (1 - alt / 90);
      canvas.drawCircle(center, circleRadius, paint);
    }
  }

  void _drawCardinalDirections(Canvas canvas, Offset center, double radius) {
    final linePaint = Paint()
      ..color = Colors.white.withValues(alpha: 0.2)
      ..strokeWidth = 0.5;

    // Draw cross for N-S and E-W
    canvas.drawLine(
      Offset(center.dx, center.dy - radius),
      Offset(center.dx, center.dy + radius),
      linePaint,
    );
    canvas.drawLine(
      Offset(center.dx - radius, center.dy),
      Offset(center.dx + radius, center.dy),
      linePaint,
    );

    if (showLabels) {
      final textStyle = TextStyle(
        color: Colors.white.withValues(alpha: 0.7),
        fontSize: 9,
        fontWeight: FontWeight.bold,
      );

      final directions = [
        ('N', Offset(center.dx, center.dy - radius + 10), Colors.red.shade300),
        ('S', Offset(center.dx, center.dy + radius - 18), Colors.white70),
        ('E', Offset(center.dx + radius - 14, center.dy), Colors.white70),
        ('W', Offset(center.dx - radius + 6, center.dy), Colors.white70),
      ];

      for (final (label, pos, color) in directions) {
        final textPainter = TextPainter(
          text: TextSpan(
            text: label,
            style: textStyle.copyWith(color: color),
          ),
          textDirection: TextDirection.ltr,
        );
        textPainter.layout();
        textPainter.paint(
          canvas,
          Offset(pos.dx - textPainter.width / 2, pos.dy - textPainter.height / 2),
        );
      }
    }
  }

  void _drawZenithMarker(Canvas canvas, Offset center) {
    final paint = Paint()
      ..color = Colors.white.withValues(alpha: 0.5)
      ..style = PaintingStyle.fill;
    canvas.drawCircle(center, 2, paint);
  }

  void _drawFOVIndicator(Canvas canvas, Offset center, double radius) {
    // Convert current view position to mini-map coordinates
    // Distance from center = 90 - altitude (zenith at center)
    final viewDistance = radius * (1 - altitude.clamp(0, 90) / 90);

    // Azimuth angle (0 = North = up)
    final azRad = azimuth * math.pi / 180;

    final viewCenter = Offset(
      center.dx + math.sin(azRad) * viewDistance,
      center.dy - math.cos(azRad) * viewDistance,
    );

    // FOV size in mini-map scale
    // At zenith (altitude=90), FOV is a circle
    // At horizon (altitude=0), FOV is stretched
    final fovRadius = radius * (fieldOfView / 180).clamp(0.05, 0.5);

    // Draw FOV rectangle/ellipse
    canvas.save();
    canvas.translate(viewCenter.dx, viewCenter.dy);
    canvas.rotate((rotation + azimuth) * math.pi / 180);

    // FOV indicator - filled rectangle with border
    final fovRect = Rect.fromCenter(
      center: Offset.zero,
      width: fovRadius * 2,
      height: fovRadius * 1.5, // Typical camera aspect ratio
    );

    // Semi-transparent fill
    final fillPaint = Paint()
      ..color = Colors.blue.withValues(alpha: 0.3)
      ..style = PaintingStyle.fill;
    canvas.drawRect(fovRect, fillPaint);

    // Border
    final borderPaint = Paint()
      ..color = Colors.blue.shade300
      ..style = PaintingStyle.stroke
      ..strokeWidth = 1.5;
    canvas.drawRect(fovRect, borderPaint);

    // Center crosshair
    final crossPaint = Paint()
      ..color = Colors.blue.shade300
      ..strokeWidth = 1;
    canvas.drawLine(Offset(-4, 0), Offset(4, 0), crossPaint);
    canvas.drawLine(Offset(0, -4), Offset(0, 4), crossPaint);

    canvas.restore();

    // Draw "up" indicator if below horizon
    if (altitude < 0) {
      final warningPaint = Paint()
        ..color = Colors.red.shade400
        ..style = PaintingStyle.fill;

      // Small warning dot
      canvas.drawCircle(
        Offset(viewCenter.dx, center.dy + radius - 8),
        3,
        warningPaint,
      );
    }
  }

  @override
  bool shouldRepaint(covariant _SkyMinimapPainter oldDelegate) {
    return azimuth != oldDelegate.azimuth ||
           altitude != oldDelegate.altitude ||
           fieldOfView != oldDelegate.fieldOfView ||
           rotation != oldDelegate.rotation;
  }
}
```

**Step 2: Export from package**

In `packages/nightshade_planetarium/lib/nightshade_planetarium.dart`, add:

```dart
export 'src/widgets/sky_minimap.dart';
```

**Step 3: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`
Expected: No errors

**Step 4: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/widgets/sky_minimap.dart packages/nightshade_planetarium/lib/nightshade_planetarium.dart
git commit -m "feat(planetarium): add all-sky mini-map widget"
```

---

## Task 5: Add View Alt/Az Provider

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/providers/planetarium_providers.dart`

**Step 1: Add computed alt/az provider for current view center**

Find the `skyViewStateProvider` in `planetarium_providers.dart` and add a new provider after it:

```dart
/// Computed provider for current view center in horizontal coordinates
/// Returns (azimuth, altitude) in degrees
final viewCenterAltAzProvider = Provider<(double, double)>((ref) {
  final viewState = ref.watch(skyViewStateProvider);
  final location = ref.watch(observerLocationProvider);
  final time = ref.watch(observationTimeProvider);

  // Convert view center (RA/Dec) to Alt/Az
  final lst = AstronomyCalculations.localSiderealTime(time.time, location.longitude);

  final (alt, az) = AstronomyCalculations.equatorialToHorizontal(
    raDeg: viewState.centerRA * 15, // Convert hours to degrees
    decDeg: viewState.centerDec,
    latitudeDeg: location.latitude,
    lstHours: lst,
  );

  return (az, alt);
});
```

Make sure `AstronomyCalculations` is imported at the top of the file:

```dart
import '../astronomy/astronomy_calculations.dart';
```

**Step 2: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`
Expected: No errors

**Step 3: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/providers/planetarium_providers.dart
git commit -m "feat(planetarium): add view center alt/az provider"
```

---

## Task 6: Add HUD Toggle Providers

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/providers/planetarium_providers.dart`

**Step 1: Add toggle state providers**

Add these providers for UI toggle state:

```dart
/// Whether to show the compass HUD
final showCompassHudProvider = StateProvider<bool>((ref) => true);

/// Whether to show the mini-map
final showMinimapProvider = StateProvider<bool>((ref) => true);

/// Whether to show the ground plane
final showGroundPlaneProvider = StateProvider<bool>((ref) => true);
```

**Step 2: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/providers/planetarium_providers.dart
git commit -m "feat(planetarium): add HUD visibility toggle providers"
```

---

## Task 7: Integrate Orientation System into Planetarium Screen

**Files:**
- Modify: `packages/nightshade_app/lib/screens/planetarium/planetarium_screen.dart`

**Step 1: Add orientation HUD overlay to build method**

Locate the `build` method and find where `InteractiveSkyView` is rendered. Wrap it in a `Stack` and add the HUD overlays.

Add these imports at the top:

```dart
// Should already have nightshade_planetarium imported
// Ensure CompassHud and SkyMinimap are accessible
```

In the build method, find the `InteractiveSkyView` widget and wrap it:

```dart
Stack(
  children: [
    // Main sky view
    InteractiveSkyView(
      key: _skyViewKey,
      onObjectTapped: _handleObjectTapped,
      onCoordinateTapped: (coord) {
        if (_slewMode) {
          _handleSlewToCoordinates(coord);
        }
      },
      showFOV: _showFOV,
    ),

    // Compass HUD (bottom-left)
    Positioned(
      left: 16,
      bottom: 16,
      child: Consumer(
        builder: (context, ref, _) {
          final showCompass = ref.watch(showCompassHudProvider);
          if (!showCompass) return const SizedBox.shrink();

          final (az, alt) = ref.watch(viewCenterAltAzProvider);
          return CompassHud(
            azimuth: az,
            altitude: alt,
            size: 80,
          );
        },
      ),
    ),

    // Mini-map (bottom-right)
    Positioned(
      right: 16,
      bottom: 16,
      child: Consumer(
        builder: (context, ref, _) {
          final showMinimap = ref.watch(showMinimapProvider);
          if (!showMinimap) return const SizedBox.shrink();

          final (az, alt) = ref.watch(viewCenterAltAzProvider);
          final viewState = ref.watch(skyViewStateProvider);

          return SkyMinimap(
            azimuth: az,
            altitude: alt,
            fieldOfView: viewState.fieldOfView,
            rotation: viewState.rotation,
            size: 100,
            onTap: (tapAz, tapAlt) {
              // Convert alt/az back to RA/Dec and update view
              final location = ref.read(observerLocationProvider);
              final time = ref.read(observationTimeProvider);
              final lst = AstronomyCalculations.localSiderealTime(time.time, location.longitude);

              final (ra, dec) = AstronomyCalculations.horizontalToEquatorial(
                altDeg: tapAlt,
                azDeg: tapAz,
                latitudeDeg: location.latitude,
                lstHours: lst,
              );

              ref.read(skyViewStateProvider.notifier).setCenter(ra / 15, dec);
            },
          );
        },
      ),
    ),
  ],
),
```

**Step 2: Add HUD toggle buttons to toolbar**

Find the toolbar section (likely in a Row or Wrap widget with toggle buttons) and add:

```dart
_ToolbarToggle(
  icon: LucideIcons.compass,
  label: 'Compass',
  isActive: ref.watch(showCompassHudProvider),
  onTap: () => ref.read(showCompassHudProvider.notifier).state =
      !ref.read(showCompassHudProvider),
),
_ToolbarToggle(
  icon: LucideIcons.map,
  label: 'Map',
  isActive: ref.watch(showMinimapProvider),
  onTap: () => ref.read(showMinimapProvider.notifier).state =
      !ref.read(showMinimapProvider),
),
```

**Step 3: Run analyzer**

Run: `flutter analyze packages/nightshade_app`
Expected: No errors

**Step 4: Commit**

```bash
git add packages/nightshade_app/lib/screens/planetarium/planetarium_screen.dart
git commit -m "feat(planetarium): integrate orientation HUD overlays"
```

---

## Task 8: Update SkyRenderConfigProvider for Ground Plane Toggle

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/providers/planetarium_providers.dart`

**Step 1: Connect ground plane toggle to render config**

Find the `skyRenderConfigProvider` (or the notifier that manages it) and add a method to toggle ground plane:

```dart
void toggleGroundPlane() {
  state = state.copyWith(showGroundPlane: !state.showGroundPlane);
}
```

If using a simpler provider pattern, ensure the `showGroundPlaneProvider` state is read when creating the render config.

**Step 2: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/providers/planetarium_providers.dart
git commit -m "feat(planetarium): connect ground plane toggle to render config"
```

---

## Task 9: Full Integration Test

**Files:**
- None (manual testing)

**Step 1: Build and run the app**

Run: `melos run dev`

**Step 2: Manual verification checklist**

- [ ] Ground plane visible below horizon with gradient
- [ ] Ground plane respects quality tier settings
- [ ] Compass HUD shows in bottom-left corner
- [ ] Compass rotates as you pan the view
- [ ] Altitude indicator shows current view altitude
- [ ] Mini-map shows in bottom-right corner
- [ ] Mini-map FOV indicator moves as you pan
- [ ] Tapping mini-map jumps view to that location
- [ ] Toggle buttons hide/show each HUD element
- [ ] Performance acceptable (no stuttering)

**Step 3: Final commit if any fixes needed**

```bash
git add -A
git commit -m "fix(planetarium): polish orientation system integration"
```

---

## Summary

This plan implements the Phase 1 Orientation System with:

| Task | Component | Estimated Complexity |
|------|-----------|---------------------|
| 1 | Ground plane config | Simple |
| 2 | Ground plane renderer | Medium |
| 3 | Compass HUD widget | Medium |
| 4 | Mini-map widget | Medium |
| 5 | Alt/Az provider | Simple |
| 6 | Toggle providers | Simple |
| 7 | Screen integration | Medium |
| 8 | Config connection | Simple |
| 9 | Integration test | Manual |

Total: 9 tasks, ~45-60 minutes of implementation time with subagents.
