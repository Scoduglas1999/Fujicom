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
