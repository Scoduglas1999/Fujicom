import 'dart:math' as math;
import 'dart:ui' as ui;
import 'package:flutter/material.dart';
import '../celestial_object.dart';
import '../coordinate_system.dart';
import '../catalogs/constellation_data.dart';
import '../astronomy/astronomy_calculations.dart';
import '../astronomy/planetary_positions.dart';
import '../astronomy/milky_way_data.dart';
import 'render_quality.dart';

/// Mount tracking status for rendering
enum MountRenderStatus {
  disconnected,
  parked,
  slewing,
  tracking,
  stopped,
}

/// Configuration for sky rendering
class SkyRenderConfig {
  final bool showStars;
  final bool showConstellationLines;
  final bool showConstellationLabels;
  final bool showDSOs;
  final bool showDSOLabels;
  final bool showCoordinateGrid;
  final bool showAltAzGrid;
  final bool showEquatorialGrid;
  final bool showEcliptic;
  final bool showHorizon;
  final bool showCardinalDirections;
  final bool showMilkyWay;
  final bool showMountPosition;
  final bool showSun;
  final bool showMoon;
  final bool showPlanets;
  final bool showGroundPlane;
  final Color groundColorDark;
  final Color groundColorLight;
  final Color horizonGlowColor;
  final double starMagnitudeLimit;
  final double dsoMagnitudeLimit;
  final Color gridColor;
  final Color constellationLineColor;
  final Color eclipticColor;
  final Color horizonColor;
  final Color mountPositionColor;

  const SkyRenderConfig({
    this.showStars = true,
    this.showConstellationLines = true,
    this.showConstellationLabels = true,
    this.showDSOs = true,
    this.showDSOLabels = true,
    this.showCoordinateGrid = true,
    this.showAltAzGrid = false,
    this.showEquatorialGrid = true,
    this.showEcliptic = false,
    this.showHorizon = true,
    this.showCardinalDirections = true,
    this.showMilkyWay = false,
    this.showMountPosition = true,
    this.showSun = true,
    this.showMoon = true,
    this.showPlanets = true,
    this.showGroundPlane = true,
    this.groundColorDark = const Color(0xFF0A0805),
    this.groundColorLight = const Color(0xFF1A1510),
    this.horizonGlowColor = const Color(0xFF2A2015),
    this.starMagnitudeLimit = 6.0,
    this.dsoMagnitudeLimit = 15.0,
    this.gridColor = const Color(0x33FFFFFF),
    this.constellationLineColor = const Color(0x40FFFFFF),
    this.eclipticColor = const Color(0x40FFEB3B),
    this.horizonColor = const Color(0x60FF5722),
    this.mountPositionColor = const Color(0xFFE53935),
  });
  
  SkyRenderConfig copyWith({
    bool? showStars,
    bool? showConstellationLines,
    bool? showConstellationLabels,
    bool? showDSOs,
    bool? showDSOLabels,
    bool? showCoordinateGrid,
    bool? showAltAzGrid,
    bool? showEquatorialGrid,
    bool? showEcliptic,
    bool? showHorizon,
    bool? showCardinalDirections,
    bool? showMilkyWay,
    bool? showMountPosition,
    bool? showSun,
    bool? showMoon,
    bool? showPlanets,
    bool? showGroundPlane,
    Color? groundColorDark,
    Color? groundColorLight,
    Color? horizonGlowColor,
    double? starMagnitudeLimit,
    double? dsoMagnitudeLimit,
    Color? gridColor,
    Color? constellationLineColor,
    Color? eclipticColor,
    Color? horizonColor,
    Color? mountPositionColor,
  }) {
    return SkyRenderConfig(
      showStars: showStars ?? this.showStars,
      showConstellationLines: showConstellationLines ?? this.showConstellationLines,
      showConstellationLabels: showConstellationLabels ?? this.showConstellationLabels,
      showDSOs: showDSOs ?? this.showDSOs,
      showDSOLabels: showDSOLabels ?? this.showDSOLabels,
      showCoordinateGrid: showCoordinateGrid ?? this.showCoordinateGrid,
      showAltAzGrid: showAltAzGrid ?? this.showAltAzGrid,
      showEquatorialGrid: showEquatorialGrid ?? this.showEquatorialGrid,
      showEcliptic: showEcliptic ?? this.showEcliptic,
      showHorizon: showHorizon ?? this.showHorizon,
      showCardinalDirections: showCardinalDirections ?? this.showCardinalDirections,
      showMilkyWay: showMilkyWay ?? this.showMilkyWay,
      showMountPosition: showMountPosition ?? this.showMountPosition,
      showSun: showSun ?? this.showSun,
      showMoon: showMoon ?? this.showMoon,
      showPlanets: showPlanets ?? this.showPlanets,
      showGroundPlane: showGroundPlane ?? this.showGroundPlane,
      groundColorDark: groundColorDark ?? this.groundColorDark,
      groundColorLight: groundColorLight ?? this.groundColorLight,
      horizonGlowColor: horizonGlowColor ?? this.horizonGlowColor,
      starMagnitudeLimit: starMagnitudeLimit ?? this.starMagnitudeLimit,
      dsoMagnitudeLimit: dsoMagnitudeLimit ?? this.dsoMagnitudeLimit,
      gridColor: gridColor ?? this.gridColor,
      constellationLineColor: constellationLineColor ?? this.constellationLineColor,
      eclipticColor: eclipticColor ?? this.eclipticColor,
      horizonColor: horizonColor ?? this.horizonColor,
      mountPositionColor: mountPositionColor ?? this.mountPositionColor,
    );
  }
}

/// Sky view projection type
enum SkyProjection {
  stereographic,
  orthographic,
  azimuthalEquidistant,
}

/// View state for sky rendering
class SkyViewState {
  final double centerRA; // hours
  final double centerDec; // degrees
  final double fieldOfView; // degrees
  final double rotation; // degrees
  final SkyProjection projection;
  
  const SkyViewState({
    this.centerRA = 0,
    this.centerDec = 0,
    this.fieldOfView = 90,
    this.rotation = 0,
    this.projection = SkyProjection.stereographic,
  });
  
  SkyViewState copyWith({
    double? centerRA,
    double? centerDec,
    double? fieldOfView,
    double? rotation,
    SkyProjection? projection,
  }) {
    return SkyViewState(
      centerRA: centerRA ?? this.centerRA,
      centerDec: centerDec ?? this.centerDec,
      fieldOfView: fieldOfView ?? this.fieldOfView,
      rotation: rotation ?? this.rotation,
      projection: projection ?? this.projection,
    );
  }
}

/// Enhanced sky rendering painter
class SkyCanvasPainter extends CustomPainter {
  final SkyViewState viewState;
  final SkyRenderConfig config;
  final RenderQualityConfig qualityConfig;
  final List<Star> stars;
  final List<DeepSkyObject> dsos;
  final List<ConstellationData> constellations;
  final DateTime observationTime;
  final double latitude;
  final double longitude;
  final CelestialCoordinate? selectedObject;
  final CelestialCoordinate? highlightedObject;
  final CelestialCoordinate? mountPosition;
  final MountRenderStatus mountStatus;
  final (double ra, double dec)? sunPosition;
  final (double ra, double dec, double illumination)? moonPosition;
  final List<PlanetData> planets;
  final List<MilkyWayPoint>? milkyWayPoints;

  /// Animation phase for star twinkle (0.0 - 1.0, cycles)
  final double? animationPhase;

  /// Animation phase for selection pulse (0.0 - 1.0, cycles)
  final double? selectionAnimationPhase;

  /// Animation phase for star pop-in (0.0 - 1.0)
  final double? popinAnimationPhase;

  /// Animation phase for DSO pop-in (0.0 - 1.0)
  final double? dsoPopinAnimationPhase;

  /// Current pan delta for parallax effect (pixels)
  final Offset? parallaxPanDelta;

  /// Density hotspots for crowded regions (ra, dec, visibleCount, hiddenCount)
  final List<(double, double, int, int)> densityHotspots;

  static const double _deg2rad = math.pi / 180;
  static const double _rad2deg = 180 / math.pi;

  SkyCanvasPainter({
    required this.viewState,
    required this.config,
    this.qualityConfig = const RenderQualityConfig.balanced(),
    required this.stars,
    required this.dsos,
    required this.constellations,
    required this.observationTime,
    required this.latitude,
    required this.longitude,
    this.selectedObject,
    this.highlightedObject,
    this.mountPosition,
    this.mountStatus = MountRenderStatus.disconnected,
    this.sunPosition,
    this.moonPosition,
    this.planets = const [],
    this.milkyWayPoints,
    this.animationPhase,
    this.selectionAnimationPhase,
    this.popinAnimationPhase,
    this.dsoPopinAnimationPhase,
    this.parallaxPanDelta,
    this.densityHotspots = const [],
  });
  
  @override
  void paint(Canvas canvas, Size size) {
    final center = Offset(size.width / 2, size.height / 2);
    final scale = math.min(size.width, size.height) / 2 / (viewState.fieldOfView / 2);
    
    // Draw background gradient
    _drawBackground(canvas, size);

    // Draw Milky Way (before everything else as background glow)
    if (config.showMilkyWay && milkyWayPoints != null && milkyWayPoints!.isNotEmpty) {
      _drawMilkyWay(canvas, size, center, scale);
    }

    // Draw coordinate grids
    if (config.showCoordinateGrid) {
      if (config.showEquatorialGrid) {
        _drawEquatorialGrid(canvas, size, center, scale);
      }
      if (config.showAltAzGrid) {
        _drawAltAzGrid(canvas, size, center, scale);
      }
    }
    
    // Draw ecliptic
    if (config.showEcliptic) {
      _drawEcliptic(canvas, size, center, scale);
    }

    // Draw ground plane (before horizon line so horizon draws on top)
    if (config.showGroundPlane) {
      _drawGroundPlane(canvas, size, center, scale);
    }

    // Draw horizon
    if (config.showHorizon) {
      _drawHorizon(canvas, size, center, scale);
      // Draw horizon glow effect
      if (qualityConfig.enableHorizonGlow) {
        _drawHorizonGlow(canvas, size, center, scale);
      }
    }

    // Draw light pollution dome effect (quality mode only)
    if (qualityConfig.enableLightPollution) {
      _drawLightPollutionDome(canvas, size, center, scale);
    }

    // Draw constellation lines
    if (config.showConstellationLines) {
      _drawConstellationLines(canvas, size, center, scale);
    }
    
    // Draw stars
    if (config.showStars) {
      _drawStars(canvas, size, center, scale);
    }
    
    // Draw DSOs
    if (config.showDSOs) {
      _drawDSOs(canvas, size, center, scale);
    }

    // Draw density indicators for crowded regions when zoomed out
    if (densityHotspots.isNotEmpty) {
      _drawDensityIndicators(canvas, size, center, scale);
    }

    // Draw Sun
    if (config.showSun && sunPosition != null) {
      _drawSun(canvas, size, center, scale);
    }

    // Draw Moon
    if (config.showMoon && moonPosition != null) {
      _drawMoon(canvas, size, center, scale);
    }

    // Draw planets
    if (config.showPlanets && planets.isNotEmpty) {
      _drawPlanets(canvas, size, center, scale);
    }

    // Draw constellation labels
    if (config.showConstellationLabels) {
      _drawConstellationLabels(canvas, size, center, scale);
    }

    // Draw cardinal directions
    if (config.showCardinalDirections) {
      _drawCardinalDirections(canvas, size);
    }

    // Draw mount position marker
    if (config.showMountPosition && mountPosition != null && mountStatus != MountRenderStatus.disconnected) {
      _drawMountPositionMarker(canvas, size, center, scale, mountPosition!, mountStatus);
    }

    // Draw selected object marker
    if (selectedObject != null) {
      _drawSelectionMarker(canvas, center, scale, selectedObject!);
    }
  }
  
  void _drawBackground(Canvas canvas, Size size) {
    // Check if twilight gradient is enabled
    if (!qualityConfig.enableTwilightGradient) {
      // Simple dark gradient for performance mode
      final gradient = RadialGradient(
        center: Alignment.center,
        radius: 1.5,
        colors: const [
          Color(0xFF0A0A1A),
          Color(0xFF050510),
          Color(0xFF020208),
        ],
      );
      final paint = Paint()
        ..shader = gradient.createShader(Rect.fromLTWH(0, 0, size.width, size.height));
      canvas.drawRect(Rect.fromLTWH(0, 0, size.width, size.height), paint);
      return;
    }

    // Calculate sun altitude for twilight determination
    final sunAlt = AstronomyCalculations.sunAltitude(
      dt: observationTime,
      latitudeDeg: latitude,
      longitudeDeg: longitude,
    );

    // Get twilight colors based on sun altitude
    final (zenithColor, horizonColor) = _getTwilightColors(sunAlt);

    // Create vertical gradient from zenith (top) to horizon (bottom)
    // This is a simplification - the actual gradient should follow the horizon line
    // but for a first pass, a vertical gradient provides the visual effect
    final gradient = LinearGradient(
      begin: Alignment.topCenter,
      end: Alignment.bottomCenter,
      colors: [zenithColor, horizonColor],
      stops: const [0.0, 1.0],
    );

    final paint = Paint()
      ..shader = gradient.createShader(Rect.fromLTWH(0, 0, size.width, size.height));
    canvas.drawRect(Rect.fromLTWH(0, 0, size.width, size.height), paint);

    // Add a subtle radial darkening toward center for depth
    final radialGradient = RadialGradient(
      center: Alignment.center,
      radius: 1.5,
      colors: [
        Colors.transparent,
        zenithColor.withValues(alpha: 0.3),
      ],
    );
    final radialPaint = Paint()
      ..shader = radialGradient.createShader(Rect.fromLTWH(0, 0, size.width, size.height));
    canvas.drawRect(Rect.fromLTWH(0, 0, size.width, size.height), radialPaint);
  }

  /// Get twilight colors based on sun altitude
  /// Returns (zenithColor, horizonColor) for gradient
  (Color, Color) _getTwilightColors(double sunAltitude) {
    // Astronomical twilight: sun below -18°
    // Nautical twilight: sun between -18° and -12°
    // Civil twilight: sun between -12° and -6°
    // Golden hour: sun between -6° and 0°
    // Day: sun above 0°

    if (sunAltitude <= -18) {
      // Full night - dark blue-black gradient
      return (
        const Color(0xFF0A0A1A), // Zenith: very dark blue
        const Color(0xFF0D0D20), // Horizon: slightly lighter dark blue
      );
    } else if (sunAltitude <= -12) {
      // Nautical twilight - deep blues
      final t = (sunAltitude + 18) / 6; // 0 at -18, 1 at -12
      return (
        Color.lerp(const Color(0xFF0A0A1A), const Color(0xFF0F1028), t)!,
        Color.lerp(const Color(0xFF0D0D20), const Color(0xFF1A1A38), t)!,
      );
    } else if (sunAltitude <= -6) {
      // Civil twilight - navy to deep purple/blue
      final t = (sunAltitude + 12) / 6; // 0 at -12, 1 at -6
      return (
        Color.lerp(const Color(0xFF0F1028), const Color(0xFF1A1A40), t)!,
        Color.lerp(const Color(0xFF1A1A38), const Color(0xFF2D2040), t)!,
      );
    } else if (sunAltitude <= 0) {
      // Golden hour - purple/blue to orange/pink at horizon
      final t = (sunAltitude + 6) / 6; // 0 at -6, 1 at 0
      return (
        Color.lerp(const Color(0xFF1A1A40), const Color(0xFF252050), t)!,
        Color.lerp(const Color(0xFF2D2040), const Color(0xFF4A3048), t)!,
      );
    } else if (sunAltitude <= 6) {
      // Just after sunrise/before sunset - warm colors
      final t = (sunAltitude / 6).clamp(0.0, 1.0); // 0 at 0, 1 at 6
      return (
        Color.lerp(const Color(0xFF252050), const Color(0xFF354080), t)!,
        Color.lerp(const Color(0xFF4A3048), const Color(0xFF705040), t)!,
      );
    } else {
      // Full day - light blue sky (though planetarium usually used at night)
      return (
        const Color(0xFF4060A0), // Zenith: medium blue
        const Color(0xFF8090B0), // Horizon: lighter blue
      );
    }
  }

  void _drawMilkyWay(Canvas canvas, Size size, Offset center, double scale) {
    if (milkyWayPoints == null) return;

    // Calculate appropriate blur and point size based on FOV
    // Wider FOV = larger blur for smoother appearance
    final fovFactor = viewState.fieldOfView / 60;
    final blurRadius = (8 * fovFactor).clamp(4.0, 20.0);
    final pointRadius = (3 * fovFactor).clamp(2.0, 8.0);

    // Milky Way color - subtle blue-white glow
    const baseColor = Color(0xFF8090A8);

    for (final point in milkyWayPoints!) {
      final offset = _celestialToScreen(point.coordinates, center, scale);
      if (offset == null || !_isInView(offset, size)) continue;

      // Calculate alpha based on intensity
      final alpha = (point.intensity * 0.25).clamp(0.02, 0.25);

      // Draw glow circle
      final glowPaint = Paint()
        ..color = baseColor.withValues(alpha: alpha)
        ..maskFilter = MaskFilter.blur(BlurStyle.normal, blurRadius);

      canvas.drawCircle(offset, pointRadius * 2, glowPaint);

      // Draw slightly brighter core for high-intensity points
      if (point.intensity > 0.5) {
        final corePaint = Paint()
          ..color = baseColor.withValues(alpha: alpha * 1.5)
          ..maskFilter = MaskFilter.blur(BlurStyle.normal, blurRadius * 0.5);
        canvas.drawCircle(offset, pointRadius, corePaint);
      }
    }
  }

  void _drawEquatorialGrid(Canvas canvas, Size size, Offset center, double scale) {
    final paint = Paint()
      ..color = config.gridColor
      ..strokeWidth = 0.5
      ..style = PaintingStyle.stroke;
    
    // Draw RA lines (every 2 hours = 30 degrees)
    for (var ra = 0; ra < 24; ra += 2) {
      final path = Path();
      var firstPoint = true;
      
      for (var dec = -90.0; dec <= 90; dec += 5) {
        final offset = _celestialToScreen(
          CelestialCoordinate(ra: ra.toDouble(), dec: dec),
          center,
          scale,
        );
        
        if (offset != null && _isInView(offset, size)) {
          if (firstPoint) {
            path.moveTo(offset.dx, offset.dy);
            firstPoint = false;
          } else {
            path.lineTo(offset.dx, offset.dy);
          }
        } else {
          firstPoint = true;
        }
      }
      
      canvas.drawPath(path, paint);
    }
    
    // Draw Dec lines (every 30 degrees)
    for (var dec = -60; dec <= 60; dec += 30) {
      final path = Path();
      var firstPoint = true;
      
      for (var ra = 0.0; ra <= 24; ra += 0.5) {
        final offset = _celestialToScreen(
          CelestialCoordinate(ra: ra, dec: dec.toDouble()),
          center,
          scale,
        );
        
        if (offset != null && _isInView(offset, size)) {
          if (firstPoint) {
            path.moveTo(offset.dx, offset.dy);
            firstPoint = false;
          } else {
            path.lineTo(offset.dx, offset.dy);
          }
        } else {
          firstPoint = true;
        }
      }
      
      canvas.drawPath(path, paint);
    }
  }
  
  void _drawAltAzGrid(Canvas canvas, Size size, Offset center, double scale) {
    final paint = Paint()
      ..color = config.gridColor.withValues(alpha: 0.3)
      ..strokeWidth = 0.5
      ..style = PaintingStyle.stroke;
    
    final lst = AstronomyCalculations.localSiderealTime(observationTime, longitude);
    
    // Draw altitude circles
    for (var alt = 0; alt <= 90; alt += 30) {
      final path = Path();
      var firstPoint = true;
      
      for (var az = 0.0; az <= 360; az += 5) {
        // Convert alt/az to RA/Dec
        final (ra, dec) = AstronomyCalculations.horizontalToEquatorial(
          altDeg: alt.toDouble(),
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
          if (firstPoint) {
            path.moveTo(offset.dx, offset.dy);
            firstPoint = false;
          } else {
            path.lineTo(offset.dx, offset.dy);
          }
        } else {
          firstPoint = true;
        }
      }
      
      canvas.drawPath(path, paint);
    }
  }
  
  void _drawEcliptic(Canvas canvas, Size size, Offset center, double scale) {
    final paint = Paint()
      ..color = config.eclipticColor
      ..strokeWidth = 1.5
      ..style = PaintingStyle.stroke;
    
    final path = Path();
    var firstPoint = true;
    
    // Draw ecliptic as a great circle
    for (var lon = 0.0; lon <= 360; lon += 2) {
      final (ra, dec) = AstronomyCalculations.eclipticToEquatorial(
        lonDeg: lon,
        latDeg: 0,
        obliquityDeg: 23.44,
      );
      
      final offset = _celestialToScreen(
        CelestialCoordinate(ra: ra / 15, dec: dec),
        center,
        scale,
      );
      
      if (offset != null && _isInView(offset, size)) {
        if (firstPoint) {
          path.moveTo(offset.dx, offset.dy);
          firstPoint = false;
        } else {
          path.lineTo(offset.dx, offset.dy);
        }
      } else {
        firstPoint = true;
      }
    }
    
    canvas.drawPath(path, paint);
  }
  
  void _drawHorizon(Canvas canvas, Size size, Offset center, double scale) {
    final paint = Paint()
      ..color = config.horizonColor
      ..strokeWidth = 2
      ..style = PaintingStyle.stroke;
    
    final lst = AstronomyCalculations.localSiderealTime(observationTime, longitude);
    final path = Path();
    var firstPoint = true;
    
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
        if (firstPoint) {
          path.moveTo(offset.dx, offset.dy);
          firstPoint = false;
        } else {
          path.lineTo(offset.dx, offset.dy);
        }
      } else {
        firstPoint = true;
      }
    }
    
    canvas.drawPath(path, paint);

    // Fill below horizon with darker color
    if (!firstPoint) {
      path.close();
      final fillPaint = Paint()
        ..color = const Color(0x40000000)
        ..style = PaintingStyle.fill;
      canvas.drawPath(path, fillPaint);
    }
  }

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

  /// Draw light pollution dome effect (quality mode only)
  /// Creates a warm orange-white wash near the horizon that fades toward zenith
  void _drawLightPollutionDome(Canvas canvas, Size size, Offset center, double scale) {
    final lst = AstronomyCalculations.localSiderealTime(observationTime, longitude);

    // Light pollution color - warm orange-white
    const pollutionColor = Color(0xFFFFF5E0);

    // Draw concentric altitude bands from horizon upward
    // Maximum intensity at horizon (0°), fades to zero at ~45°
    final altitudeBands = [0.0, 10.0, 20.0, 30.0, 40.0];
    final bandOpacities = [0.12, 0.08, 0.05, 0.02, 0.005];

    for (var i = 0; i < altitudeBands.length; i++) {
      final alt = altitudeBands[i];
      final opacity = bandOpacities[i];

      final bandPath = Path();
      var firstPoint = true;

      // Draw a band at this altitude
      for (var az = 0.0; az <= 360; az += 5) {
        final (ra, dec) = AstronomyCalculations.horizontalToEquatorial(
          altDeg: alt,
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
          if (firstPoint) {
            bandPath.moveTo(offset.dx, offset.dy);
            firstPoint = false;
          } else {
            bandPath.lineTo(offset.dx, offset.dy);
          }
        } else {
          firstPoint = true;
        }
      }

      // Draw the band as a wide stroke
      final bandPaint = Paint()
        ..color = pollutionColor.withValues(alpha: opacity)
        ..strokeWidth = 30 - alt * 0.5 // Wider bands near horizon
        ..style = PaintingStyle.stroke
        ..strokeCap = StrokeCap.round;

      if (qualityConfig.useBlurEffects) {
        bandPaint.maskFilter = MaskFilter.blur(BlurStyle.normal, 15 - alt * 0.3);
      }

      canvas.drawPath(bandPath, bandPaint);
    }
  }

  /// Draw a subtle glow effect above the horizon line
  void _drawHorizonGlow(Canvas canvas, Size size, Offset center, double scale) {
    final lst = AstronomyCalculations.localSiderealTime(observationTime, longitude);

    // Calculate sun altitude to determine glow color
    final sunAlt = AstronomyCalculations.sunAltitude(
      dt: observationTime,
      latitudeDeg: latitude,
      longitudeDeg: longitude,
    );

    // Determine glow color based on twilight state
    Color glowColor;
    if (sunAlt <= -18) {
      // Full night - subtle cool blue glow
      glowColor = const Color(0xFF1A2030);
    } else if (sunAlt <= -6) {
      // Twilight - purple/blue glow
      final t = ((sunAlt + 18) / 12).clamp(0.0, 1.0);
      glowColor = Color.lerp(
        const Color(0xFF1A2030),
        const Color(0xFF3A2840),
        t,
      )!;
    } else if (sunAlt <= 0) {
      // Golden hour - warm orange glow
      final t = ((sunAlt + 6) / 6).clamp(0.0, 1.0);
      glowColor = Color.lerp(
        const Color(0xFF3A2840),
        const Color(0xFF604030),
        t,
      )!;
    } else {
      // Day - pale warm glow
      glowColor = const Color(0xFF706050);
    }

    // Draw glow at several altitudes above horizon (5, 10, 15 degrees)
    final glowAltitudes = [5.0, 10.0, 15.0];
    final glowOpacities = [0.15, 0.10, 0.05];

    for (var i = 0; i < glowAltitudes.length; i++) {
      final alt = glowAltitudes[i];
      final opacity = glowOpacities[i];

      final glowPath = Path();
      var firstPoint = true;

      for (var az = 0.0; az <= 360; az += 5) {
        final (ra, dec) = AstronomyCalculations.horizontalToEquatorial(
          altDeg: alt,
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
          if (firstPoint) {
            glowPath.moveTo(offset.dx, offset.dy);
            firstPoint = false;
          } else {
            glowPath.lineTo(offset.dx, offset.dy);
          }
        } else {
          firstPoint = true;
        }
      }

      // Draw the glow line
      final glowPaint = Paint()
        ..color = glowColor.withValues(alpha: opacity)
        ..strokeWidth = (20 - alt).clamp(5.0, 15.0) // Wider closer to horizon
        ..style = PaintingStyle.stroke
        ..strokeCap = StrokeCap.round;

      // Apply blur in quality mode
      if (qualityConfig.useBlurEffects) {
        glowPaint.maskFilter = MaskFilter.blur(BlurStyle.normal, 8);
      }

      canvas.drawPath(glowPath, glowPaint);
    }
  }

  void _drawConstellationLines(Canvas canvas, Size size, Offset center, double scale) {
    // Extract base color and alpha from config
    final baseColor = config.constellationLineColor;
    final baseAlpha = baseColor.a;

    for (final constellation in constellations) {
      for (final line in constellation.lines) {
        final start = _celestialToScreen(line.start, center, scale);
        final end = _celestialToScreen(line.end, center, scale);

        if (start != null && end != null) {
          if (_isInView(start, size) || _isInView(end, size)) {
            // Gradient along line for subtle depth effect
            final gradient = ui.Gradient.linear(
              start,
              end,
              [
                baseColor.withValues(alpha: baseAlpha * 0.7),
                baseColor.withValues(alpha: baseAlpha * 0.5),
                baseColor.withValues(alpha: baseAlpha * 0.7),
              ],
              [0.0, 0.5, 1.0],
            );

            final paint = Paint()
              ..shader = gradient
              ..strokeWidth = 1.5  // Increased from 1.0 for better visibility
              ..strokeCap = StrokeCap.round  // Smooth anti-aliased ends
              ..strokeJoin = StrokeJoin.round
              ..style = PaintingStyle.stroke;

            canvas.drawLine(start, end, paint);
          }
        }
      }
    }
  }
  
  void _drawConstellationLabels(Canvas canvas, Size size, Offset center, double scale) {
    final textStyle = TextStyle(
      color: Colors.white.withValues(alpha: 0.5),
      fontSize: 10,
      fontWeight: FontWeight.w500,
    );
    
    for (final constellation in constellations) {
      final offset = _celestialToScreen(constellation.center, center, scale);
      
      if (offset != null && _isInView(offset, size)) {
        final textPainter = TextPainter(
          text: TextSpan(text: constellation.name.toUpperCase(), style: textStyle),
          textDirection: ui.TextDirection.ltr,
        );
        textPainter.layout();
        textPainter.paint(
          canvas,
          offset - Offset(textPainter.width / 2, textPainter.height / 2),
        );
      }
    }
  }
  
  void _drawStars(Canvas canvas, Size size, Offset center, double scale) {
    // Respect quality config limits
    var starsDrawn = 0;
    final maxStars = qualityConfig.maxStarsToRender;
    final magLimit = math.min(config.starMagnitudeLimit, qualityConfig.starMagnitudeLimit);

    // Twinkle animation enabled in quality mode
    final doTwinkle = qualityConfig.animateStarTwinkle && animationPhase != null;

    // Pre-calculate LST for atmospheric extinction (computed once, not per-star)
    final doExtinction = qualityConfig.enableAtmosphericExtinction;
    final lst = doExtinction ? AstronomyCalculations.localSiderealTime(observationTime, longitude) : 0.0;

    // Parallax effect: offset dim stars during pan for depth illusion
    final doParallax = qualityConfig.enableParallax &&
        parallaxPanDelta != null &&
        parallaxPanDelta!.distance > 0.5;

    for (final star in stars) {
      if (starsDrawn >= maxStars) break;
      if ((star.magnitude ?? 99) > magLimit) continue;

      var offset = _celestialToScreen(star.coordinates, center, scale);
      if (offset == null || !_isInView(offset, size)) continue;

      final magnitude = star.magnitude ?? 5.0;

      // Apply parallax offset to dim stars (mag > 4)
      // Creates subtle depth illusion during panning
      if (doParallax && magnitude > 4.0) {
        // Dimmer stars lag behind by 1-2% of pan delta
        final parallaxFactor = ((magnitude - 4.0) / 6.0).clamp(0.0, 1.0) * 0.02;
        offset = Offset(
          offset.dx + parallaxPanDelta!.dx * parallaxFactor,
          offset.dy + parallaxPanDelta!.dy * parallaxFactor,
        );
      }

      var radius = _magnitudeToRadius(magnitude);
      var brightness = _magnitudeToBrightness(magnitude);

      // Apply twinkle effect for brighter stars (mag < 4)
      if (doTwinkle && magnitude < 4.0) {
        // Use star coordinates to create unique phase offset for each star
        final starPhase = (star.coordinates.ra * 1000 + star.coordinates.dec * 100) % 1.0;
        final twinklePhase = (animationPhase! + starPhase) % 1.0;

        // Sinusoidal brightness variation (subtle, ±15% for bright stars)
        final twinkleFactor = magnitude < 2.0 ? 0.15 : 0.08;
        final twinkleValue = math.sin(twinklePhase * 2 * math.pi) * twinkleFactor;
        brightness = (brightness + twinkleValue).clamp(0.0, 1.0);

        // Subtle size variation for very bright stars
        if (magnitude < 1.5) {
          radius *= 1.0 + twinkleValue * 0.3;
        }
      }

      // Star color based on spectral type
      var color = _spectralTypeToColor(star.spectralType ?? 'G');

      // Apply atmospheric extinction (dimming and reddening near horizon)
      if (doExtinction) {
        final (alt, _) = AstronomyCalculations.equatorialToHorizontal(
          raDeg: star.coordinates.raDegrees,
          decDeg: star.coordinates.dec,
          latitudeDeg: latitude,
          lstHours: lst,
        );

        if (alt < 30) {
          // Extinction factor: 0.5 at horizon, 1.0 at 30 degrees
          final extinctionFactor = (alt / 30).clamp(0.0, 1.0) * 0.5 + 0.5;
          brightness *= extinctionFactor;

          // Shift color toward red/orange for low altitude stars
          final redShift = (1 - extinctionFactor) * 0.4;
          color = Color.lerp(color, const Color(0xFFFFAA88), redShift)!;
        }
      }

      // Draw star using quality-appropriate PSF
      _drawStarPSF(canvas, offset, radius, color, brightness, magnitude);

      // Draw star name for bright stars
      if (magnitude < 2.0 && star.name.isNotEmpty) {
        final textStyle = TextStyle(
          color: Colors.white.withValues(alpha: 0.6),
          fontSize: 9,
        );
        final textPainter = TextPainter(
          text: TextSpan(text: star.name, style: textStyle),
          textDirection: ui.TextDirection.ltr,
        );
        textPainter.layout();
        textPainter.paint(canvas, offset + Offset(radius + 3, -textPainter.height / 2));
      }

      starsDrawn++;
    }
  }

  /// Draw a star using quality-appropriate point spread function
  void _drawStarPSF(
    Canvas canvas,
    Offset center,
    double radius,
    Color color,
    double brightness,
    double magnitude,
  ) {
    final psfQuality = qualityConfig.starPsfQuality;

    if (psfQuality <= 0.0) {
      // Performance mode: simple filled circle
      final paint = Paint()..color = color.withValues(alpha: brightness);
      canvas.drawCircle(center, radius, paint);
      return;
    }

    // Draw outer glow for bright stars
    if (magnitude < 2.5) {
      if (psfQuality >= 0.5) {
        // Balanced/Quality: radial gradient glow
        final glowRadius = radius * (3 + (1 - magnitude / 2.5));
        final glowGradient = RadialGradient(
          colors: [
            color.withValues(alpha: brightness * 0.4),
            color.withValues(alpha: brightness * 0.15),
            color.withValues(alpha: 0.0),
          ],
          stops: const [0.0, 0.5, 1.0],
        );
        final glowPaint = Paint()
          ..shader = glowGradient.createShader(
            Rect.fromCircle(center: center, radius: glowRadius),
          );
        canvas.drawCircle(center, glowRadius, glowPaint);
      }

      // Draw diffraction spikes for very bright stars
      if (magnitude < 1.0 && psfQuality >= 1.0) {
        _drawDiffractionSpikes(canvas, center, radius, color, brightness);
        // Add faint 45-degree secondary spikes for mag < 0
        if (magnitude < 0) {
          _drawSecondarySpikes(canvas, center, radius * 0.6, color, brightness * 0.4);
        }
      }
    }

    if (psfQuality >= 1.0) {
      // Quality mode: 3-ring Airy disk approximation
      // Outer ring (faint)
      final outerRing = RadialGradient(
        colors: [
          Colors.transparent,
          color.withValues(alpha: brightness * 0.1),
          color.withValues(alpha: brightness * 0.05),
          Colors.transparent,
        ],
        stops: const [0.0, 0.6, 0.8, 1.0],
      );
      final outerRadius = radius * 2.5;
      final outerPaint = Paint()
        ..shader = outerRing.createShader(
          Rect.fromCircle(center: center, radius: outerRadius),
        );
      canvas.drawCircle(center, outerRadius, outerPaint);

      // Middle ring
      final midRing = RadialGradient(
        colors: [
          color.withValues(alpha: brightness * 0.8),
          color.withValues(alpha: brightness * 0.4),
          color.withValues(alpha: brightness * 0.1),
          Colors.transparent,
        ],
        stops: const [0.0, 0.3, 0.6, 1.0],
      );
      final midRadius = radius * 1.5;
      final midPaint = Paint()
        ..shader = midRing.createShader(
          Rect.fromCircle(center: center, radius: midRadius),
        );
      canvas.drawCircle(center, midRadius, midPaint);

      // Core (bright center)
      final coreGradient = RadialGradient(
        colors: [
          Colors.white.withValues(alpha: brightness),
          color.withValues(alpha: brightness),
          color.withValues(alpha: brightness * 0.5),
        ],
        stops: const [0.0, 0.4, 1.0],
      );
      final corePaint = Paint()
        ..shader = coreGradient.createShader(
          Rect.fromCircle(center: center, radius: radius),
        );
      canvas.drawCircle(center, radius, corePaint);
    } else {
      // Balanced mode: 2-ring radial gradient
      final gradient = RadialGradient(
        colors: [
          Colors.white.withValues(alpha: brightness * 0.9),
          color.withValues(alpha: brightness),
          color.withValues(alpha: brightness * 0.3),
          Colors.transparent,
        ],
        stops: const [0.0, 0.3, 0.7, 1.0],
      );
      final paint = Paint()
        ..shader = gradient.createShader(
          Rect.fromCircle(center: center, radius: radius * 1.5),
        );
      canvas.drawCircle(center, radius * 1.5, paint);
    }
  }

  /// Draw secondary 45-degree diffraction spikes for very bright stars
  void _drawSecondarySpikes(Canvas canvas, Offset center, double starRadius, Color color, double brightness) {
    final spikeLength = starRadius * 5;

    // Draw 4 spikes at 45-degree angles
    for (final angle in [45.0, 135.0, 225.0, 315.0]) {
      final rad = angle * _deg2rad;
      final endX = center.dx + math.cos(rad) * spikeLength;
      final endY = center.dy + math.sin(rad) * spikeLength;

      final paint = Paint()
        ..shader = ui.Gradient.linear(
          center,
          Offset(endX, endY),
          [
            color.withValues(alpha: brightness * 0.4),
            color.withValues(alpha: 0.0),
          ],
        )
        ..strokeWidth = 0.5
        ..style = PaintingStyle.stroke;

      canvas.drawLine(center, Offset(endX, endY), paint);
    }
  }

  /// Draw 4-pointed diffraction spikes for very bright stars
  void _drawDiffractionSpikes(Canvas canvas, Offset center, double starRadius, Color color, double brightness) {
    final spikeLength = starRadius * 5; // Reduced from 8 for more delicate appearance
    final paint = Paint()
      ..shader = ui.Gradient.linear(
        center,
        center + Offset(spikeLength, 0),
        [
          color.withValues(alpha: brightness * 0.6),
          color.withValues(alpha: 0.0),
        ],
      )
      ..strokeWidth = 1.0
      ..style = PaintingStyle.stroke;

    // Draw 4 spikes (horizontal and vertical)
    for (final angle in [0.0, 90.0, 180.0, 270.0]) {
      final rad = angle * _deg2rad;
      final endX = center.dx + math.cos(rad) * spikeLength;
      final endY = center.dy + math.sin(rad) * spikeLength;

      // Update shader for this direction
      paint.shader = ui.Gradient.linear(
        center,
        Offset(endX, endY),
        [
          color.withValues(alpha: brightness * 0.4),
          color.withValues(alpha: 0.0),
        ],
      );

      canvas.drawLine(center, Offset(endX, endY), paint);
    }
  }
  
  void _drawDSOs(Canvas canvas, Size size, Offset center, double scale) {
    // Respect quality config limits
    var dsosDrawn = 0;
    final maxDsos = qualityConfig.maxDsosToRender;
    final magLimit = math.min(config.dsoMagnitudeLimit, qualityConfig.dsoMagnitudeLimit);

    // Calculate pop-in animation values
    // Phase goes from 0 to 1; use easeOutCubic for smooth deceleration
    final popinPhase = dsoPopinAnimationPhase ?? 1.0;
    final easedPhase = Curves.easeOutCubic.transform(popinPhase.clamp(0.0, 1.0));
    // Scale from 80% to 100%
    final popinScale = 0.8 + 0.2 * easedPhase;
    // Alpha from 0 to 1
    final popinAlpha = easedPhase;

    for (final dso in dsos) {
      if (dsosDrawn >= maxDsos) break;
      if ((dso.magnitude ?? 99) > magLimit) continue;

      final offset = _celestialToScreen(dso.coordinates, center, scale);
      if (offset == null || !_isInView(offset, size)) continue;

      final dsoSize = (dso.sizeArcMin ?? 5) / 60 * scale;
      final displaySize = dsoSize.clamp(4.0, 30.0);

      // Apply pop-in animation if active
      if (dsoPopinAnimationPhase != null && dsoPopinAnimationPhase! < 1.0) {
        canvas.save();
        // Scale from center of the DSO
        canvas.translate(offset.dx, offset.dy);
        canvas.scale(popinScale);
        canvas.translate(-offset.dx, -offset.dy);

        // Draw DSO symbol with pop-in alpha
        _drawDSOSymbolWithAlpha(canvas, offset, displaySize, dso.type, popinAlpha);

        // Draw DSO label with pop-in alpha
        if (config.showDSOLabels) {
          final baseAlpha = 0.7 * popinAlpha;
          final textStyle = TextStyle(
            color: _dsoTypeColor(dso.type).withValues(alpha: baseAlpha),
            fontSize: 9,
          );
          final textPainter = TextPainter(
            text: TextSpan(text: dso.name, style: textStyle),
            textDirection: ui.TextDirection.ltr,
          );
          textPainter.layout();
          textPainter.paint(canvas, offset + Offset(displaySize / 2 + 3, -textPainter.height / 2));
        }

        canvas.restore();
      } else {
        // Normal rendering without animation
        // Draw DSO symbol based on type
        _drawDSOSymbol(canvas, offset, displaySize, dso.type);

        // Draw DSO label
        if (config.showDSOLabels) {
          final textStyle = TextStyle(
            color: _dsoTypeColor(dso.type).withValues(alpha: 0.7),
            fontSize: 9,
          );
          final textPainter = TextPainter(
            text: TextSpan(text: dso.name, style: textStyle),
            textDirection: ui.TextDirection.ltr,
          );
          textPainter.layout();
          textPainter.paint(canvas, offset + Offset(displaySize / 2 + 3, -textPainter.height / 2));
        }
      }

      dsosDrawn++;
    }
  }

  /// Draw visual density indicators for crowded regions when zoomed out.
  /// Shows subtle glowing circles with count labels to indicate "zoom in to reveal more".
  void _drawDensityIndicators(Canvas canvas, Size size, Offset center, double scale) {
    for (final hotspot in densityHotspots) {
      final (ra, dec, visibleCount, hiddenCount) = hotspot;
      final coord = CelestialCoordinate(ra: ra, dec: dec);
      final offset = _celestialToScreen(coord, center, scale);

      if (offset == null || !_isInView(offset, size)) continue;

      // Calculate indicator size based on hidden count
      // More hidden objects = larger indicator
      final indicatorRadius = 15.0 + (hiddenCount / 100).clamp(0.0, 15.0);

      // Draw subtle blue glow
      const indicatorColor = Color(0xFF64B5F6); // Light blue

      // Outer glow
      if (qualityConfig.useBlurEffects) {
        final glowPaint = Paint()
          ..color = indicatorColor.withValues(alpha: 0.15)
          ..maskFilter = MaskFilter.blur(BlurStyle.normal, indicatorRadius * 0.8);
        canvas.drawCircle(offset, indicatorRadius * 1.5, glowPaint);
      }

      // Inner glow ring
      final ringPaint = Paint()
        ..color = indicatorColor.withValues(alpha: 0.25)
        ..style = PaintingStyle.stroke
        ..strokeWidth = 2;
      if (qualityConfig.useBlurEffects) {
        ringPaint.maskFilter = const MaskFilter.blur(BlurStyle.normal, 3);
      }
      canvas.drawCircle(offset, indicatorRadius, ringPaint);

      // Draw count label for regions with many hidden objects
      if (hiddenCount > 50) {
        final textStyle = TextStyle(
          color: indicatorColor.withValues(alpha: 0.8),
          fontSize: 9,
          fontWeight: FontWeight.w500,
        );
        final textPainter = TextPainter(
          text: TextSpan(text: '+$hiddenCount', style: textStyle),
          textDirection: ui.TextDirection.ltr,
        );
        textPainter.layout();

        // Draw small background for readability
        final bgRect = Rect.fromCenter(
          center: offset + Offset(0, indicatorRadius + 10),
          width: textPainter.width + 6,
          height: textPainter.height + 2,
        );
        final bgPaint = Paint()
          ..color = const Color(0xAA000000);
        canvas.drawRRect(
          RRect.fromRectAndRadius(bgRect, const Radius.circular(3)),
          bgPaint,
        );

        textPainter.paint(
          canvas,
          offset + Offset(-textPainter.width / 2, indicatorRadius + 10 - textPainter.height / 2),
        );
      }
    }
  }

  /// Draw DSO symbol with custom alpha for pop-in animation
  void _drawDSOSymbolWithAlpha(Canvas canvas, Offset center, double size, DsoType type, double alpha) {
    final baseColor = _dsoTypeColor(type);
    final adjustedColor = baseColor.withValues(alpha: baseColor.a * alpha);

    switch (type) {
      case DsoType.galaxy:
      case DsoType.galaxyPair:
      case DsoType.galaxyTriplet:
        _drawGalaxyWithAlpha(canvas, center, size, adjustedColor, alpha);
        break;

      case DsoType.nebula:
      case DsoType.emissionNebula:
      case DsoType.reflectionNebula:
      case DsoType.hiiRegion:
        _drawNebulaWithAlpha(canvas, center, size, adjustedColor, type, alpha);
        break;

      case DsoType.planetaryNebula:
        _drawPlanetaryNebulaWithAlpha(canvas, center, size, adjustedColor, alpha);
        break;

      case DsoType.openCluster:
        _drawOpenClusterWithAlpha(canvas, center, size, adjustedColor, alpha);
        break;

      case DsoType.globularCluster:
        _drawGlobularClusterWithAlpha(canvas, center, size, adjustedColor, alpha);
        break;

      case DsoType.supernova:
        _drawSupernovaWithAlpha(canvas, center, size, adjustedColor, alpha);
        break;

      default:
        // Fallback to simple circle
        final paint = Paint()
          ..color = adjustedColor.withValues(alpha: 0.6 * alpha)
          ..style = PaintingStyle.stroke
          ..strokeWidth = 1.5;
        canvas.drawCircle(center, size / 2, paint);
    }
  }

  void _drawGalaxyWithAlpha(Canvas canvas, Offset center, double size, Color color, double alpha) {
    // Outer glow
    _drawOvalGlow(canvas, center, size * 2.5, size * 1.5, color, 8.0, opacity: 0.15 * alpha);

    // Middle layer
    _drawOvalGlow(canvas, center, size * 1.8, size * 1.2, color, 4.0, opacity: 0.4 * alpha);

    // Add spiral arm hints for larger galaxies in quality mode
    if (qualityConfig.enableEnhancedDsoSymbols && size > 10) {
      _drawSpiralArmsWithAlpha(canvas, center, size, color, alpha);
    }

    // Bright core
    _drawOvalGlow(canvas, center, size * 0.8, size * 0.5, color, 2.0, opacity: 0.8 * alpha);

    // Central bright spot (always drawn)
    final centerPaint = Paint()..color = Colors.white.withValues(alpha: 0.9 * alpha);
    canvas.drawCircle(center, size * 0.15, centerPaint);
  }

  void _drawSpiralArmsWithAlpha(Canvas canvas, Offset center, double size, Color color, double alpha) {
    final armPaint = Paint()
      ..color = color.withValues(alpha: 0.15 * alpha)
      ..strokeWidth = size * 0.06
      ..style = PaintingStyle.stroke
      ..strokeCap = StrokeCap.round;

    if (qualityConfig.useBlurEffects) {
      armPaint.maskFilter = const MaskFilter.blur(BlurStyle.normal, 3);
    }

    for (final startAngle in [0.0, math.pi]) {
      final path = Path();
      var firstPoint = true;

      for (var t = 0.0; t <= 1.8; t += 0.08) {
        final r = size * 0.15 * math.exp(0.5 * t);
        final angle = startAngle + t * 2.5;
        final x = center.dx + r * math.cos(angle);
        final y = center.dy + r * math.sin(angle) * 0.5;

        if (firstPoint) {
          path.moveTo(x, y);
          firstPoint = false;
        } else {
          path.lineTo(x, y);
        }
      }
      canvas.drawPath(path, armPaint);
    }
  }

  void _drawNebulaWithAlpha(Canvas canvas, Offset center, double size, Color color, DsoType type, double alpha) {
    final random = math.Random(center.dx.toInt() + center.dy.toInt());

    Color nebulaColor = color;
    if (type == DsoType.emissionNebula || type == DsoType.hiiRegion) {
      nebulaColor = Color.fromRGBO(255, 23, 68, alpha); // Pink/red for emission
    } else if (type == DsoType.reflectionNebula) {
      nebulaColor = Color.fromRGBO(68, 138, 255, alpha); // Blue for reflection
    }

    // Draw multiple overlapping circles for wispy effect
    for (var i = 0; i < 5; i++) {
      final offsetX = (random.nextDouble() - 0.5) * size * 0.5;
      final offsetY = (random.nextDouble() - 0.5) * size * 0.5;
      final circleSize = size * (0.4 + random.nextDouble() * 0.4);

      final paint = Paint()
        ..color = nebulaColor.withValues(alpha: (0.15 + random.nextDouble() * 0.1) * alpha);

      if (qualityConfig.useBlurEffects) {
        paint.maskFilter = MaskFilter.blur(BlurStyle.normal, circleSize * 0.4);
      }

      canvas.drawCircle(
        Offset(center.dx + offsetX, center.dy + offsetY),
        circleSize,
        paint,
      );
    }
  }

  void _drawPlanetaryNebulaWithAlpha(Canvas canvas, Offset center, double size, Color color, double alpha) {
    // Outer ring
    final ringPaint = Paint()
      ..color = color.withValues(alpha: 0.5 * alpha)
      ..style = PaintingStyle.stroke
      ..strokeWidth = size * 0.15;

    if (qualityConfig.useBlurEffects) {
      ringPaint.maskFilter = MaskFilter.blur(BlurStyle.normal, size * 0.1);
    }

    canvas.drawCircle(center, size * 0.4, ringPaint);

    // Central star
    final starPaint = Paint()..color = Colors.white.withValues(alpha: 0.9 * alpha);
    canvas.drawCircle(center, size * 0.1, starPaint);
  }

  void _drawOpenClusterWithAlpha(Canvas canvas, Offset center, double size, Color color, double alpha) {
    final random = math.Random(center.dx.toInt() + center.dy.toInt());

    // Draw scattered small stars
    for (var i = 0; i < 8; i++) {
      final angle = random.nextDouble() * 2 * math.pi;
      final dist = random.nextDouble() * size * 0.4;
      final starSize = 1.0 + random.nextDouble() * 1.5;

      final starCenter = Offset(
        center.dx + math.cos(angle) * dist,
        center.dy + math.sin(angle) * dist,
      );

      final paint = Paint()..color = Colors.white.withValues(alpha: (0.6 + random.nextDouble() * 0.4) * alpha);
      canvas.drawCircle(starCenter, starSize, paint);
    }

    // Faint boundary circle
    final boundaryPaint = Paint()
      ..color = color.withValues(alpha: 0.2 * alpha)
      ..style = PaintingStyle.stroke
      ..strokeWidth = 1;
    canvas.drawCircle(center, size * 0.5, boundaryPaint);
  }

  void _drawGlobularClusterWithAlpha(Canvas canvas, Offset center, double size, Color color, double alpha) {
    // Dense core glow
    final corePaint = Paint()
      ..color = color.withValues(alpha: 0.6 * alpha);

    if (qualityConfig.useBlurEffects) {
      corePaint.maskFilter = MaskFilter.blur(BlurStyle.normal, size * 0.3);
    }

    canvas.drawCircle(center, size * 0.3, corePaint);

    // Outer halo
    final haloPaint = Paint()
      ..color = color.withValues(alpha: 0.2 * alpha);

    if (qualityConfig.useBlurEffects) {
      haloPaint.maskFilter = MaskFilter.blur(BlurStyle.normal, size * 0.5);
    }

    canvas.drawCircle(center, size * 0.5, haloPaint);

    // Bright center point
    final centerPaint = Paint()..color = Colors.white.withValues(alpha: 0.8 * alpha);
    canvas.drawCircle(center, size * 0.1, centerPaint);
  }

  void _drawSupernovaWithAlpha(Canvas canvas, Offset center, double size, Color color, double alpha) {
    // Bright central point
    final centerPaint = Paint()..color = Colors.white.withValues(alpha: 0.95 * alpha);
    canvas.drawCircle(center, size * 0.2, centerPaint);

    // Diffraction spikes
    final spikePaint = Paint()
      ..color = color.withValues(alpha: 0.7 * alpha)
      ..strokeWidth = 1.5
      ..strokeCap = StrokeCap.round;

    for (var angle = 0.0; angle < math.pi * 2; angle += math.pi / 2) {
      final dx = math.cos(angle) * size * 0.6;
      final dy = math.sin(angle) * size * 0.6;
      canvas.drawLine(
        Offset(center.dx - dx * 0.3, center.dy - dy * 0.3),
        Offset(center.dx + dx, center.dy + dy),
        spikePaint,
      );
    }

    // Glow
    final glowPaint = Paint()
      ..color = color.withValues(alpha: 0.3 * alpha);

    if (qualityConfig.useBlurEffects) {
      glowPaint.maskFilter = MaskFilter.blur(BlurStyle.normal, size * 0.4);
    }

    canvas.drawCircle(center, size * 0.4, glowPaint);
  }
  
  void _drawDSOSymbol(Canvas canvas, Offset center, double size, DsoType type) {
    final baseColor = _dsoTypeColor(type);
    
    switch (type) {
      case DsoType.galaxy:
      case DsoType.galaxyPair:
      case DsoType.galaxyTriplet:
        _drawGalaxy(canvas, center, size, baseColor);
        break;
        
      case DsoType.nebula:
      case DsoType.emissionNebula:
      case DsoType.reflectionNebula:
      case DsoType.hiiRegion:
        _drawNebula(canvas, center, size, baseColor, type);
        break;
        
      case DsoType.planetaryNebula:
        _drawPlanetaryNebula(canvas, center, size, baseColor);
        break;
        
      case DsoType.openCluster:
        _drawOpenCluster(canvas, center, size, baseColor);
        break;
        
      case DsoType.globularCluster:
        _drawGlobularCluster(canvas, center, size, baseColor);
        break;
        
      case DsoType.supernova:
        _drawSupernova(canvas, center, size, baseColor);
        break;
        
      default:
        // Fallback to simple circle
        final paint = Paint()
          ..color = baseColor.withValues(alpha: 0.6)
          ..style = PaintingStyle.stroke
          ..strokeWidth = 1.5;
        canvas.drawCircle(center, size / 2, paint);
    }
  }
  
  void _drawGalaxy(Canvas canvas, Offset center, double size, Color color) {
    // Outer glow
    _drawOvalGlow(canvas, center, size * 2.5, size * 1.5, color, 8.0, opacity: 0.15);

    // Middle layer
    _drawOvalGlow(canvas, center, size * 1.8, size * 1.2, color, 4.0, opacity: 0.4);

    // Add spiral arm hints for larger galaxies in quality mode
    if (qualityConfig.enableEnhancedDsoSymbols && size > 10) {
      _drawSpiralArms(canvas, center, size, color);
    }

    // Bright core
    _drawOvalGlow(canvas, center, size * 0.8, size * 0.5, color, 2.0, opacity: 0.8);

    // Central bright spot (always drawn)
    final centerPaint = Paint()..color = Colors.white.withValues(alpha: 0.9);
    canvas.drawCircle(center, size * 0.15, centerPaint);
  }

  /// Draw subtle spiral arm hints for galaxies
  void _drawSpiralArms(Canvas canvas, Offset center, double size, Color color) {
    final armPaint = Paint()
      ..color = color.withValues(alpha: 0.15)
      ..strokeWidth = size * 0.06
      ..style = PaintingStyle.stroke
      ..strokeCap = StrokeCap.round;

    // Apply blur if available
    if (qualityConfig.useBlurEffects) {
      armPaint.maskFilter = const MaskFilter.blur(BlurStyle.normal, 3);
    }

    // Draw 2 logarithmic spiral arms
    for (final startAngle in [0.0, math.pi]) {
      final path = Path();
      var firstPoint = true;

      for (var t = 0.0; t <= 1.8; t += 0.08) {
        // Logarithmic spiral: r = a * e^(b*theta)
        final r = size * 0.15 * math.exp(0.5 * t);
        final angle = startAngle + t * 2.5;
        final x = center.dx + r * math.cos(angle);
        final y = center.dy + r * math.sin(angle) * 0.5; // Squash for inclination

        if (firstPoint) {
          path.moveTo(x, y);
          firstPoint = false;
        } else {
          path.lineTo(x, y);
        }
      }
      canvas.drawPath(path, armPaint);
    }
  }
  
  void _drawNebula(Canvas canvas, Offset center, double size, Color color, DsoType type) {
    // Create wispy cloud effect with multiple overlapping circles
    final random = math.Random(center.dx.toInt() + center.dy.toInt());

    // Adjust color based on nebula type
    Color nebulaColor = color;
    if (type == DsoType.emissionNebula || type == DsoType.hiiRegion) {
      nebulaColor = const Color(0xFFFF1744); // Pink/red for emission
    } else if (type == DsoType.reflectionNebula) {
      nebulaColor = const Color(0xFF448AFF); // Blue for reflection
    }

    final enhanced = qualityConfig.enableEnhancedDsoSymbols && size > 6;
    final puffCount = enhanced ? 12 : 8;

    // Draw multiple cloud puffs
    for (var i = 0; i < puffCount; i++) {
      final angle = (i / puffCount) * 2 * math.pi + random.nextDouble() * 0.5;
      final distance = size * (0.3 + random.nextDouble() * 0.4);
      final puffCenter = Offset(
        center.dx + math.cos(angle) * distance,
        center.dy + math.sin(angle) * distance,
      );
      // Varying puff sizes for more organic look
      final puffSize = size * (0.3 + random.nextDouble() * 0.4);

      final puffPaint = Paint()
        ..color = nebulaColor.withValues(alpha: 0.15 + random.nextDouble() * 0.15);

      if (qualityConfig.useBlurEffects) {
        puffPaint.maskFilter = const MaskFilter.blur(BlurStyle.normal, 6);
      }
      canvas.drawCircle(puffCenter, puffSize, puffPaint);
    }

    // Enhanced mode: add wispy tendrils using bezier curves
    if (enhanced) {
      _drawNebulaTendrils(canvas, center, size, nebulaColor, random);
    }

    // Central brighter region
    final centralPaint = Paint()
      ..color = nebulaColor.withValues(alpha: 0.4);
    if (qualityConfig.useBlurEffects) {
      centralPaint.maskFilter = const MaskFilter.blur(BlurStyle.normal, 4);
    }
    canvas.drawCircle(center, size * 0.5, centralPaint);

    // Bright embedded stars for larger nebulae
    if (enhanced && size > 12) {
      for (var i = 0; i < 3; i++) {
        final starAngle = random.nextDouble() * 2 * math.pi;
        final starDist = size * 0.3 * random.nextDouble();
        final starPos = Offset(
          center.dx + math.cos(starAngle) * starDist,
          center.dy + math.sin(starAngle) * starDist,
        );
        final starPaint = Paint()..color = Colors.white.withValues(alpha: 0.7 + random.nextDouble() * 0.3);
        canvas.drawCircle(starPos, 1.0 + random.nextDouble(), starPaint);
      }
    }
  }

  /// Draw wispy tendrils extending from nebula using bezier curves
  void _drawNebulaTendrils(Canvas canvas, Offset center, double size, Color color, math.Random random) {
    final tendrilPaint = Paint()
      ..color = color.withValues(alpha: 0.12)
      ..strokeWidth = size * 0.08
      ..style = PaintingStyle.stroke
      ..strokeCap = StrokeCap.round;

    if (qualityConfig.useBlurEffects) {
      tendrilPaint.maskFilter = const MaskFilter.blur(BlurStyle.normal, 4);
    }

    // Draw 4-6 wispy tendrils
    final tendrilCount = 4 + random.nextInt(3);
    for (var i = 0; i < tendrilCount; i++) {
      final baseAngle = (i / tendrilCount) * 2 * math.pi + random.nextDouble() * 0.3;

      final path = Path();
      path.moveTo(
        center.dx + math.cos(baseAngle) * size * 0.3,
        center.dy + math.sin(baseAngle) * size * 0.3,
      );

      // Control points for bezier curve
      final cp1Distance = size * (0.5 + random.nextDouble() * 0.3);
      final cp1Angle = baseAngle + (random.nextDouble() - 0.5) * 0.8;
      final cp1 = Offset(
        center.dx + math.cos(cp1Angle) * cp1Distance,
        center.dy + math.sin(cp1Angle) * cp1Distance,
      );

      final endDistance = size * (0.8 + random.nextDouble() * 0.4);
      final endAngle = baseAngle + (random.nextDouble() - 0.5) * 0.5;
      final endPoint = Offset(
        center.dx + math.cos(endAngle) * endDistance,
        center.dy + math.sin(endAngle) * endDistance,
      );

      path.quadraticBezierTo(cp1.dx, cp1.dy, endPoint.dx, endPoint.dy);
      canvas.drawPath(path, tendrilPaint);
    }
  }
  
  void _drawPlanetaryNebula(Canvas canvas, Offset center, double size, Color color) {
    // Green ring (OIII emission)
    const ringColor = Color(0xFF00E676);
    final enhanced = qualityConfig.enableEnhancedDsoSymbols && size > 8;

    // Outer glow/shell
    final outerGlowPaint = Paint()
      ..color = ringColor.withValues(alpha: 0.2);
    if (qualityConfig.useBlurEffects) {
      outerGlowPaint.maskFilter = const MaskFilter.blur(BlurStyle.normal, 8);
    }
    canvas.drawCircle(center, size * 0.9, outerGlowPaint);

    // Enhanced mode: bipolar lobes for larger planetary nebulae
    if (enhanced) {
      _drawBipolarLobes(canvas, center, size, ringColor);
    }

    // Outer ring
    final outerRingPaint = Paint()
      ..color = ringColor.withValues(alpha: 0.4)
      ..style = PaintingStyle.stroke
      ..strokeWidth = 1.5;
    if (qualityConfig.useBlurEffects) {
      outerRingPaint.maskFilter = const MaskFilter.blur(BlurStyle.normal, 3);
    }
    canvas.drawCircle(center, size * 0.7, outerRingPaint);

    // Inner ring (main structure)
    final innerRingPaint = Paint()
      ..color = ringColor.withValues(alpha: 0.7)
      ..style = PaintingStyle.stroke
      ..strokeWidth = 2;
    if (qualityConfig.useBlurEffects) {
      innerRingPaint.maskFilter = const MaskFilter.blur(BlurStyle.normal, 2);
    }
    canvas.drawCircle(center, size * 0.45, innerRingPaint);

    // Enhanced: inner shell fill
    if (enhanced) {
      final innerFillPaint = Paint()
        ..color = ringColor.withValues(alpha: 0.15);
      if (qualityConfig.useBlurEffects) {
        innerFillPaint.maskFilter = const MaskFilter.blur(BlurStyle.normal, 2);
      }
      canvas.drawCircle(center, size * 0.4, innerFillPaint);
    }

    // Central star with diffraction pattern for quality mode
    if (qualityConfig.starPsfQuality >= 1.0 && size > 10) {
      // Draw small diffraction spikes for central star
      _drawDiffractionSpikes(canvas, center, 2.0, Colors.white, 0.8);
    }

    final starPaint = Paint()
      ..color = Colors.white.withValues(alpha: 0.9);
    if (qualityConfig.useBlurEffects) {
      starPaint.maskFilter = const MaskFilter.blur(BlurStyle.normal, 1);
    }
    canvas.drawCircle(center, 2, starPaint);
  }

  /// Draw bipolar lobes for planetary nebulae
  void _drawBipolarLobes(Canvas canvas, Offset center, double size, Color color) {
    final lobePaint = Paint()
      ..color = color.withValues(alpha: 0.15)
      ..style = PaintingStyle.fill;

    if (qualityConfig.useBlurEffects) {
      lobePaint.maskFilter = const MaskFilter.blur(BlurStyle.normal, 4);
    }

    // Draw two elongated lobes (top and bottom)
    final lobeWidth = size * 0.4;
    final lobeHeight = size * 0.8;

    // Top lobe
    canvas.drawOval(
      Rect.fromCenter(
        center: Offset(center.dx, center.dy - size * 0.35),
        width: lobeWidth,
        height: lobeHeight,
      ),
      lobePaint,
    );

    // Bottom lobe
    canvas.drawOval(
      Rect.fromCenter(
        center: Offset(center.dx, center.dy + size * 0.35),
        width: lobeWidth,
        height: lobeHeight,
      ),
      lobePaint,
    );
  }
  
  void _drawOpenCluster(Canvas canvas, Offset center, double size, Color color) {
    // Draw scattered star points
    final random = math.Random(center.dx.toInt() + center.dy.toInt());
    final starCount = (size / 2).clamp(5, 15).toInt();
    
    for (var i = 0; i < starCount; i++) {
      final angle = random.nextDouble() * 2 * math.pi;
      final distance = random.nextDouble() * size * 0.6;
      final starPos = Offset(
        center.dx + math.cos(angle) * distance,
        center.dy + math.sin(angle) * distance,
      );
      
      final starSize = 1.0 + random.nextDouble() * 1.5;
      
      // Star glow
      final glowPaint = Paint()
        ..color = color.withValues(alpha: 0.4)
        ..maskFilter = const MaskFilter.blur(BlurStyle.normal, 2);
      canvas.drawCircle(starPos, starSize * 2, glowPaint);
      
      // Star center
      final starPaint = Paint()
        ..color = color.withValues(alpha: 0.9);
      canvas.drawCircle(starPos, starSize, starPaint);
    }
  }
  
  void _drawGlobularCluster(Canvas canvas, Offset center, double size, Color color) {
    // Dense core with radial falloff
    final random = math.Random(center.dx.toInt() + center.dy.toInt());

    // Outer halo
    _drawGlow(canvas, center, size, color, 8.0, opacity: 0.15);

    // Middle region
    _drawGlow(canvas, center, size * 0.6, color, 4.0, opacity: 0.4);

    // Dense core
    _drawGlow(canvas, center, size * 0.3, color, 2.0, opacity: 0.7);

    // Add some individual star sparkles (always drawn)
    for (var i = 0; i < 6; i++) {
      final angle = (i / 6) * 2 * math.pi;
      final distance = size * (0.2 + random.nextDouble() * 0.3);
      final starPos = Offset(
        center.dx + math.cos(angle) * distance,
        center.dy + math.sin(angle) * distance,
      );

      final starPaint = Paint()..color = Colors.white.withValues(alpha: 0.8);
      canvas.drawCircle(starPos, 0.8, starPaint);
    }
  }
  
  void _drawSupernova(Canvas canvas, Offset center, double size, Color color) {
    // Bright starburst with glow
    final brightColor = const Color(0xFFFFFFFF);
    
    // Outer glow
    final glowPaint = Paint()
      ..color = color.withValues(alpha: 0.3)
      ..maskFilter = const MaskFilter.blur(BlurStyle.normal, 8);
    canvas.drawCircle(center, size * 1.5, glowPaint);
    
    // Inner glow
    final innerGlowPaint = Paint()
      ..color = brightColor.withValues(alpha: 0.5)
      ..maskFilter = const MaskFilter.blur(BlurStyle.normal, 4);
    canvas.drawCircle(center, size * 0.8, innerGlowPaint);
    
    // Rays
    final rayPaint = Paint()
      ..color = brightColor.withValues(alpha: 0.8)
      ..strokeWidth = 2
      ..maskFilter = const MaskFilter.blur(BlurStyle.normal, 2);
    
    for (var i = 0; i < 8; i++) {
      final angle = (i / 8) * 2 * math.pi;
      canvas.drawLine(
        center,
        center + Offset(math.cos(angle) * size, math.sin(angle) * size),
        rayPaint,
      );
    }
    
    // Central bright core
    final corePaint = Paint()
      ..color = brightColor;
    canvas.drawCircle(center, 3, corePaint);
  }
  
  Color _dsoTypeColor(DsoType type) {
    switch (type) {
      case DsoType.galaxy:
        return const Color(0xFF64B5F6); // Blue
      case DsoType.nebula:
        return const Color(0xFFE91E63); // Pink
      case DsoType.planetaryNebula:
        return const Color(0xFF4CAF50); // Green
      case DsoType.openCluster:
        return const Color(0xFFFFEB3B); // Yellow
      case DsoType.globularCluster:
        return const Color(0xFFFF9800); // Orange
      case DsoType.supernova:
        return const Color(0xFFF44336); // Red
      default:
        return const Color(0xFFFFFFFF); // White
    }
  }
  
  void _drawCardinalDirections(Canvas canvas, Size size) {
    final textStyle = TextStyle(
      color: Colors.white.withValues(alpha: 0.7),
      fontSize: 14,
      fontWeight: FontWeight.bold,
    );
    
    final directions = ['N', 'E', 'S', 'W'];
    final positions = [
      Offset(size.width / 2, 20),
      Offset(size.width - 20, size.height / 2),
      Offset(size.width / 2, size.height - 20),
      Offset(20, size.height / 2),
    ];
    
    for (var i = 0; i < 4; i++) {
      final textPainter = TextPainter(
        text: TextSpan(text: directions[i], style: textStyle),
        textDirection: ui.TextDirection.ltr,
      );
      textPainter.layout();
      textPainter.paint(
        canvas,
        positions[i] - Offset(textPainter.width / 2, textPainter.height / 2),
      );
    }
  }
  
  void _drawSelectionMarker(Canvas canvas, Offset center, double scale, CelestialCoordinate coord) {
    final offset = _celestialToScreen(coord, center, scale);
    if (offset == null) return;

    // Apply animation if enabled
    double pulseScale = 1.0;
    double glowOpacity = 0.3;
    if (qualityConfig.enableSelectionAnimation && selectionAnimationPhase != null) {
      // Sinusoidal pulse between 1.0 and 1.1
      pulseScale = 1.0 + 0.1 * math.sin(selectionAnimationPhase! * 2 * math.pi);
      // Pulsing glow opacity
      glowOpacity = 0.2 + 0.2 * math.sin(selectionAnimationPhase! * 2 * math.pi);
    }

    const baseColor = Color(0xFF00E676);

    // Draw animated glow behind the marker
    if (qualityConfig.enableSelectionAnimation && glowOpacity > 0) {
      final glowPaint = Paint()
        ..color = baseColor.withValues(alpha: glowOpacity);
      if (qualityConfig.useBlurEffects) {
        glowPaint.maskFilter = const MaskFilter.blur(BlurStyle.normal, 12);
      }
      canvas.drawCircle(offset, 20 * pulseScale, glowPaint);
    }

    final paint = Paint()
      ..color = baseColor
      ..strokeWidth = 2
      ..style = PaintingStyle.stroke;

    // Draw crosshairs with pulse
    final circleRadius = 15 * pulseScale;
    final innerOffset = 20 * pulseScale;
    final outerOffset = 25 * pulseScale;

    canvas.drawCircle(offset, circleRadius, paint);
    canvas.drawLine(
      offset - Offset(outerOffset, 0),
      offset - Offset(innerOffset, 0),
      paint,
    );
    canvas.drawLine(
      offset + Offset(innerOffset, 0),
      offset + Offset(outerOffset, 0),
      paint,
    );
    canvas.drawLine(
      offset - Offset(0, outerOffset),
      offset - Offset(0, innerOffset),
      paint,
    );
    canvas.drawLine(
      offset + Offset(0, innerOffset),
      offset + Offset(0, outerOffset),
      paint,
    );
  }

  void _drawMountPositionMarker(
    Canvas canvas,
    Size size,
    Offset center,
    double scale,
    CelestialCoordinate coord,
    MountRenderStatus status,
  ) {
    final offset = _celestialToScreen(coord, center, scale);
    if (offset == null) return;

    // Color based on tracking status
    Color markerColor;
    switch (status) {
      case MountRenderStatus.tracking:
        markerColor = const Color(0xFF4CAF50); // Green for tracking
        break;
      case MountRenderStatus.slewing:
        markerColor = const Color(0xFFFF9800); // Orange for slewing
        break;
      case MountRenderStatus.parked:
        markerColor = const Color(0xFF9E9E9E); // Gray for parked
        break;
      case MountRenderStatus.stopped:
        markerColor = const Color(0xFFE53935); // Red for stopped
        break;
      case MountRenderStatus.disconnected:
        return; // Don't draw if disconnected
    }

    // Outer glow
    final glowPaint = Paint()
      ..color = markerColor.withValues(alpha: 0.3)
      ..maskFilter = const MaskFilter.blur(BlurStyle.normal, 8);
    canvas.drawCircle(offset, 20, glowPaint);

    // Main crosshair with thicker lines
    final paint = Paint()
      ..color = markerColor
      ..strokeWidth = 2.5
      ..style = PaintingStyle.stroke;

    // Draw a distinctive mount marker (different from selection marker)
    // Outer circle
    canvas.drawCircle(offset, 18, paint);

    // Inner crosshair lines - extending to edge of circle
    paint.strokeWidth = 2;
    canvas.drawLine(offset - const Offset(30, 0), offset - const Offset(18, 0), paint);
    canvas.drawLine(offset + const Offset(18, 0), offset + const Offset(30, 0), paint);
    canvas.drawLine(offset - const Offset(0, 30), offset - const Offset(0, 18), paint);
    canvas.drawLine(offset + const Offset(0, 18), offset + const Offset(0, 30), paint);

    // Inner dot
    final dotPaint = Paint()
      ..color = markerColor
      ..style = PaintingStyle.fill;
    canvas.drawCircle(offset, 3, dotPaint);

    // Draw status label below the marker
    final statusText = switch (status) {
      MountRenderStatus.tracking => 'TRACKING',
      MountRenderStatus.slewing => 'SLEWING',
      MountRenderStatus.parked => 'PARKED',
      MountRenderStatus.stopped => 'STOPPED',
      MountRenderStatus.disconnected => '',
    };

    if (statusText.isNotEmpty) {
      final textStyle = TextStyle(
        color: markerColor,
        fontSize: 9,
        fontWeight: FontWeight.bold,
      );
      final textPainter = TextPainter(
        text: TextSpan(text: statusText, style: textStyle),
        textDirection: ui.TextDirection.ltr,
      );
      textPainter.layout();

      // Background for better readability
      final bgRect = Rect.fromCenter(
        center: offset + Offset(0, 35),
        width: textPainter.width + 8,
        height: textPainter.height + 4,
      );
      final bgPaint = Paint()
        ..color = const Color(0xCC000000);
      canvas.drawRRect(
        RRect.fromRectAndRadius(bgRect, const Radius.circular(3)),
        bgPaint,
      );

      textPainter.paint(
        canvas,
        offset + Offset(-textPainter.width / 2, 35 - textPainter.height / 2),
      );
    }
  }

  void _drawSun(Canvas canvas, Size size, Offset center, double scale) {
    if (sunPosition == null) return;

    final (ra, dec) = sunPosition!;
    final coord = CelestialCoordinate(ra: ra / 15, dec: dec); // ra is in degrees, convert to hours
    final offset = _celestialToScreen(coord, center, scale);
    if (offset == null) return;

    const sunColor = Color(0xFFFFEB3B);

    // Outer glow
    _drawGlow(canvas, offset, 25, sunColor, 20.0, opacity: 0.25);

    // Mid glow
    _drawGlow(canvas, offset, 15, sunColor, 10.0, opacity: 0.5);

    // Sun disc (always drawn)
    final sunPaint = Paint()
      ..color = sunColor
      ..style = PaintingStyle.fill;
    canvas.drawCircle(offset, 10, sunPaint);

    // Sun label
    const textStyle = TextStyle(
      color: sunColor,
      fontSize: 10,
      fontWeight: FontWeight.bold,
    );
    final textPainter = TextPainter(
      text: const TextSpan(text: 'SUN', style: textStyle),
      textDirection: ui.TextDirection.ltr,
    );
    textPainter.layout();
    textPainter.paint(
      canvas,
      offset + Offset(-textPainter.width / 2, 18),
    );
  }

  void _drawMoon(Canvas canvas, Size size, Offset center, double scale) {
    if (moonPosition == null) return;

    final (ra, dec, illumination) = moonPosition!;
    final coord = CelestialCoordinate(ra: ra / 15, dec: dec); // ra is in degrees, convert to hours
    final offset = _celestialToScreen(coord, center, scale);
    if (offset == null) return;

    const moonRadius = 12.0;

    // Outer glow
    final glowPaint = Paint()
      ..color = const Color(0x30B0BEC5)
      ..maskFilter = const MaskFilter.blur(BlurStyle.normal, 15);
    canvas.drawCircle(offset, moonRadius + 8, glowPaint);

    // Moon base (dark side)
    final darkPaint = Paint()
      ..color = const Color(0xFF37474F)
      ..style = PaintingStyle.fill;
    canvas.drawCircle(offset, moonRadius, darkPaint);

    // Moon lit portion based on illumination
    // This is a simplified phase rendering
    final litPaint = Paint()
      ..color = const Color(0xFFECEFF1)
      ..style = PaintingStyle.fill;

    if (illumination > 0.01) {
      // Save canvas state
      canvas.save();
      canvas.clipRect(Rect.fromCircle(center: offset, radius: moonRadius));

      // Draw lit portion - this approximates the moon phase
      // illumination 0 = new moon, 1 = full moon
      // For simplicity, we'll use a circular approximation
      if (illumination > 0.98) {
        // Full moon
        canvas.drawCircle(offset, moonRadius, litPaint);
      } else if (illumination > 0.5) {
        // Gibbous - draw full circle then dark crescent
        canvas.drawCircle(offset, moonRadius, litPaint);
        final darkCrescentPaint = Paint()
          ..color = const Color(0xFF37474F)
          ..style = PaintingStyle.fill;
        final crescentWidth = moonRadius * 2 * (1 - illumination);
        canvas.drawOval(
          Rect.fromCenter(
            center: offset + Offset(moonRadius - crescentWidth / 2, 0),
            width: crescentWidth,
            height: moonRadius * 2,
          ),
          darkCrescentPaint,
        );
      } else {
        // Crescent
        final crescentWidth = moonRadius * 2 * illumination;
        canvas.drawOval(
          Rect.fromCenter(
            center: offset - Offset(moonRadius - crescentWidth / 2, 0),
            width: crescentWidth,
            height: moonRadius * 2,
          ),
          litPaint,
        );
      }

      canvas.restore();
    }

    // Moon outline
    final outlinePaint = Paint()
      ..color = const Color(0x60ECEFF1)
      ..style = PaintingStyle.stroke
      ..strokeWidth = 1;
    canvas.drawCircle(offset, moonRadius, outlinePaint);

    // Moon label with illumination %
    final illuminationPct = (illumination * 100).round();
    final textStyle = const TextStyle(
      color: Color(0xFFB0BEC5),
      fontSize: 10,
      fontWeight: FontWeight.w500,
    );
    final textPainter = TextPainter(
      text: TextSpan(text: 'MOON $illuminationPct%', style: textStyle),
      textDirection: ui.TextDirection.ltr,
    );
    textPainter.layout();
    textPainter.paint(
      canvas,
      offset + Offset(-textPainter.width / 2, moonRadius + 6),
    );
  }

  void _drawPlanets(Canvas canvas, Size size, Offset center, double scale) {
    for (final planet in planets) {
      // PlanetData has ra in hours and dec in degrees
      final coord = CelestialCoordinate(ra: planet.ra, dec: planet.dec);
      final offset = _celestialToScreen(coord, center, scale);
      if (offset == null) continue;

      // Convert int color to Color
      final planetColor = Color(planet.color);

      // Planet glow
      final glowPaint = Paint()
        ..color = planetColor.withValues(alpha: 0.3)
        ..maskFilter = const MaskFilter.blur(BlurStyle.normal, 6);
      canvas.drawCircle(offset, 8, glowPaint);

      // Planet disc - size based on magnitude
      final radius = _magnitudeToRadius(planet.magnitude) * 1.5 + 2;
      final planetPaint = Paint()
        ..color = planetColor
        ..style = PaintingStyle.fill;
      canvas.drawCircle(offset, radius, planetPaint);

      // Add planet-specific details in quality mode
      if (qualityConfig.enablePlanetDetails && radius > 3) {
        _drawPlanetDetails(canvas, offset, radius, planet.name, planetColor);
      }

      // Planet label
      final textStyle = TextStyle(
        color: planetColor,
        fontSize: 9,
        fontWeight: FontWeight.w500,
      );
      final textPainter = TextPainter(
        text: TextSpan(text: planet.name.toUpperCase(), style: textStyle),
        textDirection: ui.TextDirection.ltr,
      );
      textPainter.layout();
      textPainter.paint(
        canvas,
        offset + Offset(-textPainter.width / 2, radius + 4),
      );
    }
  }

  /// Draw planet-specific details (Saturn rings, Jupiter bands)
  void _drawPlanetDetails(Canvas canvas, Offset center, double radius, String planetName, Color planetColor) {
    final name = planetName.toLowerCase();

    if (name == 'saturn') {
      _drawSaturnRings(canvas, center, radius, planetColor);
    } else if (name == 'jupiter') {
      _drawJupiterBands(canvas, center, radius, planetColor);
    } else if (name == 'mars') {
      _drawMarsPolarCap(canvas, center, radius);
    }
  }

  /// Draw Saturn's iconic ring system
  void _drawSaturnRings(Canvas canvas, Offset center, double radius, Color planetColor) {
    // Ring ellipse surrounding the planet
    final ringWidth = radius * 2.8;
    final ringHeight = radius * 0.8; // Tilted view

    // Outer ring (A ring)
    final outerRingPaint = Paint()
      ..color = const Color(0xFFD4C8A8).withValues(alpha: 0.6)
      ..style = PaintingStyle.stroke
      ..strokeWidth = radius * 0.15;
    canvas.drawOval(
      Rect.fromCenter(center: center, width: ringWidth, height: ringHeight),
      outerRingPaint,
    );

    // Inner ring (B ring - brighter)
    final innerRingPaint = Paint()
      ..color = const Color(0xFFE8DCC0).withValues(alpha: 0.5)
      ..style = PaintingStyle.stroke
      ..strokeWidth = radius * 0.2;
    canvas.drawOval(
      Rect.fromCenter(center: center, width: ringWidth * 0.75, height: ringHeight * 0.75),
      innerRingPaint,
    );

    // Cassini Division (dark gap)
    final divisionPaint = Paint()
      ..color = Colors.black.withValues(alpha: 0.3)
      ..style = PaintingStyle.stroke
      ..strokeWidth = radius * 0.05;
    canvas.drawOval(
      Rect.fromCenter(center: center, width: ringWidth * 0.82, height: ringHeight * 0.82),
      divisionPaint,
    );

    // Redraw planet disc on top of back-side ring portion
    final planetPaint = Paint()
      ..color = planetColor
      ..style = PaintingStyle.fill;
    canvas.drawCircle(center, radius, planetPaint);
  }

  /// Draw Jupiter's cloud bands
  void _drawJupiterBands(Canvas canvas, Offset center, double radius, Color planetColor) {
    // Subtle horizontal bands
    final bandPaint = Paint()
      ..color = const Color(0xFF8B6914).withValues(alpha: 0.3)
      ..style = PaintingStyle.stroke
      ..strokeWidth = radius * 0.15;

    // Draw 3 bands at different latitudes
    for (final offset in [-0.5, 0.0, 0.5]) {
      final bandY = center.dy + radius * offset * 0.7;
      final bandWidth = radius * math.sqrt(1 - offset * offset * 0.5);

      canvas.drawLine(
        Offset(center.dx - bandWidth, bandY),
        Offset(center.dx + bandWidth, bandY),
        bandPaint,
      );
    }

    // Great Red Spot hint for larger renderings
    if (radius > 5) {
      final spotPaint = Paint()
        ..color = const Color(0xFFB86B4A).withValues(alpha: 0.4);
      canvas.drawOval(
        Rect.fromCenter(
          center: Offset(center.dx + radius * 0.3, center.dy + radius * 0.25),
          width: radius * 0.4,
          height: radius * 0.25,
        ),
        spotPaint,
      );
    }
  }

  /// Draw Mars polar ice cap hint
  void _drawMarsPolarCap(Canvas canvas, Offset center, double radius) {
    // Small white cap at the top
    final capPaint = Paint()
      ..color = Colors.white.withValues(alpha: 0.5);

    final capPath = Path();
    capPath.moveTo(center.dx - radius * 0.4, center.dy - radius * 0.7);
    capPath.quadraticBezierTo(
      center.dx,
      center.dy - radius * 1.1,
      center.dx + radius * 0.4,
      center.dy - radius * 0.7,
    );
    capPath.close();

    canvas.drawPath(capPath, capPaint);
  }

  Offset? _celestialToScreen(CelestialCoordinate coord, Offset center, double scale) {
    // Convert RA from hours to degrees
    final raDeg = coord.ra * 15;
    final decDeg = coord.dec;
    
    // Calculate angular distance from view center
    final centerRaDeg = viewState.centerRA * 15;
    final centerDecDeg = viewState.centerDec;
    
    // Gnomonic/stereographic projection
    final ra1 = centerRaDeg * _deg2rad;
    final dec1 = centerDecDeg * _deg2rad;
    final ra2 = raDeg * _deg2rad;
    final dec2 = decDeg * _deg2rad;
    
    final cosc = math.sin(dec1) * math.sin(dec2) + 
                 math.cos(dec1) * math.cos(dec2) * math.cos(ra2 - ra1);
    
    // Object is behind the projection plane
    if (cosc < 0.01) return null;
    
    double x, y;
    
    switch (viewState.projection) {
      case SkyProjection.stereographic:
        final k = 2 / (1 + cosc);
        x = k * math.cos(dec2) * math.sin(ra2 - ra1);
        y = k * (math.cos(dec1) * math.sin(dec2) - 
                 math.sin(dec1) * math.cos(dec2) * math.cos(ra2 - ra1));
        break;
        
      case SkyProjection.orthographic:
        x = math.cos(dec2) * math.sin(ra2 - ra1);
        y = math.cos(dec1) * math.sin(dec2) - 
            math.sin(dec1) * math.cos(dec2) * math.cos(ra2 - ra1);
        break;
        
      case SkyProjection.azimuthalEquidistant:
        final c = math.acos(cosc);
        if (c < 0.0001) {
          x = 0;
          y = 0;
        } else {
          final k = c / math.sin(c);
          x = k * math.cos(dec2) * math.sin(ra2 - ra1);
          y = k * (math.cos(dec1) * math.sin(dec2) - 
                   math.sin(dec1) * math.cos(dec2) * math.cos(ra2 - ra1));
        }
        break;
    }
    
    // Apply rotation
    final rotRad = viewState.rotation * _deg2rad;
    final xRot = x * math.cos(rotRad) - y * math.sin(rotRad);
    final yRot = x * math.sin(rotRad) + y * math.cos(rotRad);
    
    // Scale and center
    return Offset(
      center.dx - xRot * scale * _rad2deg,
      center.dy - yRot * scale * _rad2deg,
    );
  }
  
  bool _isInView(Offset offset, Size size) {
    return offset.dx >= -50 && offset.dx <= size.width + 50 &&
           offset.dy >= -50 && offset.dy <= size.height + 50;
  }
  
  double _magnitudeToRadius(double mag) {
    // Brighter stars have larger radius
    return math.max(0.5, (6.5 - mag) * 0.8);
  }
  
  double _magnitudeToBrightness(double mag) {
    // Brighter stars are more opaque
    return math.min(1.0, math.max(0.3, (7 - mag) / 6));
  }
  
  Color _spectralTypeToColor(String spectralType) {
    if (spectralType.isEmpty) return Colors.white;

    switch (spectralType[0].toUpperCase()) {
      case 'O':
        return const Color(0xFF9BB0FF); // Blue
      case 'B':
        return const Color(0xFFAABFFF); // Blue-white
      case 'A':
        return const Color(0xFFCAD7FF); // White
      case 'F':
        return const Color(0xFFF8F7FF); // Yellow-white
      case 'G':
        return const Color(0xFFFFF4E8); // Yellow
      case 'K':
        return const Color(0xFFFFD2A1); // Orange
      case 'M':
        return const Color(0xFFFFCC6F); // Red-orange
      default:
        return Colors.white;
    }
  }

  // ============ Gradient-based glow helpers ============
  // These replace expensive MaskFilter.blur with radial gradients
  // for better performance on low-powered devices.

  /// Draw a circular glow using radial gradient (faster than blur)
  void _drawGlowCircle(
    Canvas canvas,
    Offset center,
    double radius,
    Color color, {
    double innerOpacity = 0.6,
    double midOpacity = 0.2,
  }) {
    if (!qualityConfig.useGlowEffects) return;

    final gradient = RadialGradient(
      colors: [
        color.withValues(alpha: innerOpacity),
        color.withValues(alpha: midOpacity),
        color.withValues(alpha: 0.0),
      ],
      stops: const [0.0, 0.5, 1.0],
    );

    final paint = Paint()
      ..shader = gradient.createShader(
        Rect.fromCircle(center: center, radius: radius),
      );

    canvas.drawCircle(center, radius, paint);
  }

  /// Draw an oval glow using radial gradient
  void _drawGlowOval(
    Canvas canvas,
    Offset center,
    double width,
    double height,
    Color color, {
    double innerOpacity = 0.6,
    double midOpacity = 0.2,
  }) {
    if (!qualityConfig.useGlowEffects) return;

    final gradient = RadialGradient(
      colors: [
        color.withValues(alpha: innerOpacity),
        color.withValues(alpha: midOpacity),
        color.withValues(alpha: 0.0),
      ],
      stops: const [0.0, 0.5, 1.0],
    );

    final rect = Rect.fromCenter(center: center, width: width, height: height);
    final paint = Paint()..shader = gradient.createShader(rect);

    canvas.drawOval(rect, paint);
  }

  /// Draw a glow effect - uses blur if available, gradient otherwise
  void _drawGlow(
    Canvas canvas,
    Offset center,
    double radius,
    Color color,
    double blurSigma, {
    double opacity = 0.3,
  }) {
    if (qualityConfig.useBlurEffects) {
      // High quality: use blur
      final glowPaint = Paint()
        ..color = color.withValues(alpha: opacity)
        ..maskFilter = MaskFilter.blur(BlurStyle.normal, blurSigma);
      canvas.drawCircle(center, radius, glowPaint);
    } else if (qualityConfig.useGlowEffects) {
      // Balanced: use gradient
      _drawGlowCircle(
        canvas,
        center,
        radius + blurSigma * 2,
        color,
        innerOpacity: opacity * 1.5,
        midOpacity: opacity * 0.5,
      );
    }
    // Performance mode: skip glow entirely
  }

  /// Draw an oval glow effect - uses blur if available, gradient otherwise
  void _drawOvalGlow(
    Canvas canvas,
    Offset center,
    double width,
    double height,
    Color color,
    double blurSigma, {
    double opacity = 0.3,
  }) {
    if (qualityConfig.useBlurEffects) {
      final glowPaint = Paint()
        ..color = color.withValues(alpha: opacity)
        ..maskFilter = MaskFilter.blur(BlurStyle.normal, blurSigma);
      canvas.drawOval(
        Rect.fromCenter(center: center, width: width, height: height),
        glowPaint,
      );
    } else if (qualityConfig.useGlowEffects) {
      _drawGlowOval(
        canvas,
        center,
        width + blurSigma * 2,
        height + blurSigma * 2,
        color,
        innerOpacity: opacity * 1.5,
        midOpacity: opacity * 0.5,
      );
    }
  }

  @override
  bool shouldRepaint(covariant SkyCanvasPainter oldDelegate) {
    // Primary triggers - always repaint for these
    if (viewState != oldDelegate.viewState ||
        config != oldDelegate.config ||
        qualityConfig != oldDelegate.qualityConfig ||
        selectedObject != oldDelegate.selectedObject ||
        highlightedObject != oldDelegate.highlightedObject) {
      return true;
    }

    // Mount status change always triggers repaint
    if (mountStatus != oldDelegate.mountStatus) {
      return true;
    }

    // Mount position - only repaint if moved significantly (>0.05 degrees = ~3 arcmin)
    if (mountPosition != oldDelegate.mountPosition) {
      if (mountPosition == null || oldDelegate.mountPosition == null) {
        return true;
      }
      final raDiff = (mountPosition!.ra - oldDelegate.mountPosition!.ra).abs();
      final decDiff = (mountPosition!.dec - oldDelegate.mountPosition!.dec).abs();
      if (raDiff > 0.05 / 15 || decDiff > 0.05) {
        return true;
      }
    }

    // Observation time - only check minute changes for horizon/alt-az grid
    // (stars/DSOs don't move visibly in a minute, but horizon does)
    if (config.showHorizon || config.showAltAzGrid) {
      if (observationTime.minute != oldDelegate.observationTime.minute) {
        return true;
      }
    }

    // Sun/Moon/Planets - these move slowly, check if data actually changed
    if (sunPosition != oldDelegate.sunPosition ||
        moonPosition != oldDelegate.moonPosition ||
        planets.length != oldDelegate.planets.length) {
      return true;
    }

    // Milky way data change
    if (milkyWayPoints != oldDelegate.milkyWayPoints) {
      return true;
    }

    // Animation phases - repaint when animations are active
    if (animationPhase != oldDelegate.animationPhase) {
      return true;
    }
    if (selectionAnimationPhase != oldDelegate.selectionAnimationPhase) {
      return true;
    }
    if (popinAnimationPhase != oldDelegate.popinAnimationPhase) {
      return true;
    }
    if (dsoPopinAnimationPhase != oldDelegate.dsoPopinAnimationPhase) {
      return true;
    }
    if (parallaxPanDelta != oldDelegate.parallaxPanDelta) {
      return true;
    }

    // Density hotspots change
    if (densityHotspots.length != oldDelegate.densityHotspots.length) {
      return true;
    }

    return false;
  }
}

