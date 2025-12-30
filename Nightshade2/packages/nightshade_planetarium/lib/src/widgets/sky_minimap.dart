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
