import 'dart:math' as math;
import 'package:flutter/material.dart';
import 'package:flutter/gestures.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import '../coordinate_system.dart';
import '../celestial_object.dart';
import '../rendering/sky_renderer.dart';
import '../providers/planetarium_providers.dart';

/// Interactive sky view widget with pan, zoom, and object selection
class InteractiveSkyView extends ConsumerStatefulWidget {
  /// Callback when an object is selected
  final ValueChanged<CelestialObject?>? onObjectSelected;
  
  /// Callback when coordinates are tapped
  final ValueChanged<CelestialCoordinate>? onCoordinateTapped;
  
  /// Callback when an object is tapped with position info for popup display
  final void Function(CelestialObject? object, CelestialCoordinate coordinates, Offset screenPosition)? onObjectTapped;
  
  /// Whether to show the FOV indicator
  final bool showFOV;
  
  /// Custom FOV rectangle (if not using equipment provider)
  final (double width, double height)? customFOV;
  
  /// FOV center coordinate (if different from view center)
  final CelestialCoordinate? fovCenter;
  
  const InteractiveSkyView({
    super.key,
    this.onObjectSelected,
    this.onCoordinateTapped,
    this.onObjectTapped,
    this.showFOV = false,
    this.customFOV,
    this.fovCenter,
  });

  @override
  ConsumerState<InteractiveSkyView> createState() => _InteractiveSkyViewState();
}

class _InteractiveSkyViewState extends ConsumerState<InteractiveSkyView>
    with TickerProviderStateMixin {
  Offset? _lastFocalPoint;
  double? _lastScale;

  // Smooth zoom animation
  late AnimationController _zoomController;
  Animation<double>? _zoomAnimation;
  double _targetFOV = 60.0;
  double _startFOV = 60.0;

  // Star twinkle animation
  AnimationController? _twinkleController;
  double _twinklePhase = 0.0;

  // Selection pulse animation
  late AnimationController _selectionController;
  double _selectionPhase = 0.0;
  CelestialCoordinate? _lastSelection;

  // Panning momentum
  late AnimationController _momentumController;
  Offset _panVelocity = Offset.zero;
  List<_PanSample> _panSamples = [];

  // Star pop-in animation (tracks previous magnitude threshold)
  double _previousMagLimit = 6.0;
  late AnimationController _popinController;
  double _popinPhase = 0.0;

  // Parallax effect - tracks current pan delta for dim star offset
  Offset _currentPanDelta = Offset.zero;

  @override
  void initState() {
    super.initState();
    _zoomController = AnimationController(
      vsync: this,
      duration: const Duration(milliseconds: 300),
    )..addListener(_onZoomAnimation);

    // Twinkle animation cycles every 3 seconds
    _twinkleController = AnimationController(
      vsync: this,
      duration: const Duration(seconds: 3),
    )..addListener(() {
        setState(() {
          _twinklePhase = _twinkleController!.value;
        });
      });
    _twinkleController!.repeat();

    // Selection pulse animation (cycles every 1.5 seconds)
    _selectionController = AnimationController(
      vsync: this,
      duration: const Duration(milliseconds: 1500),
    )..addListener(() {
        setState(() {
          _selectionPhase = _selectionController.value;
        });
      });

    // Momentum deceleration animation
    _momentumController = AnimationController(
      vsync: this,
      duration: const Duration(milliseconds: 800),
    )..addListener(_onMomentumAnimation);

    // Star pop-in animation (600ms with elastic out)
    _popinController = AnimationController(
      vsync: this,
      duration: const Duration(milliseconds: 600),
    )..addListener(() {
        setState(() {
          _popinPhase = _popinController.value;
        });
      });
  }

  @override
  void dispose() {
    _zoomController.dispose();
    _twinkleController?.dispose();
    _selectionController.dispose();
    _momentumController.dispose();
    _popinController.dispose();
    super.dispose();
  }

  void _onZoomAnimation() {
    if (_zoomAnimation != null) {
      final newFOV = _zoomAnimation!.value;
      ref.read(skyViewStateProvider.notifier).setFieldOfView(newFOV);

      // Trigger star pop-in animation when zooming reveals fainter stars
      final qualityConfig = ref.read(renderQualityProvider);
      if (qualityConfig.enableStarPopin) {
        // Estimate new magnitude limit based on FOV
        // Wider FOV = lower mag limit, narrower FOV = higher mag limit
        final newMagLimit = 6.0 + (60.0 - newFOV).clamp(0.0, 54.0) / 6.0;
        if (newMagLimit > _previousMagLimit + 0.5) {
          // Zooming in revealed fainter stars - trigger pop-in
          _popinController.forward(from: 0.0);
        }
        _previousMagLimit = newMagLimit;
      }
    }
  }

  void _onMomentumAnimation() {
    if (_panVelocity.distance < 1) return;

    // Decelerate using a curve
    final t = Curves.decelerate.transform(_momentumController.value);
    final remaining = 1.0 - t;

    final viewState = ref.read(skyViewStateProvider);
    final panScale = viewState.fieldOfView / 500;

    // Apply decelerating pan
    final delta = _panVelocity * remaining * 0.02; // Time step factor
    ref.read(skyViewStateProvider.notifier).pan(
          -delta.dx * panScale / 15,
          delta.dy * panScale,
        );
  }

  /// Calculate pan velocity from recent samples
  Offset _calculatePanVelocity() {
    if (_panSamples.length < 2) return Offset.zero;

    // Use last few samples for velocity calculation
    final recent = _panSamples.length > 5
        ? _panSamples.sublist(_panSamples.length - 5)
        : _panSamples;

    if (recent.length < 2) return Offset.zero;

    // Calculate average velocity from recent samples
    var totalVelocity = Offset.zero;
    for (var i = 1; i < recent.length; i++) {
      final dt = recent[i].time.difference(recent[i - 1].time).inMilliseconds.toDouble();
      if (dt > 0) {
        final delta = recent[i].position - recent[i - 1].position;
        totalVelocity += delta / dt * 1000; // pixels per second
      }
    }

    return totalVelocity / (recent.length - 1).toDouble();
  }

  /// Animate FOV to a new target value
  void _animateZoom(double newFOV) {
    final currentFOV = ref.read(skyViewStateProvider).fieldOfView;
    _startFOV = currentFOV;
    _targetFOV = newFOV.clamp(1.0, 180.0);

    _zoomAnimation = Tween<double>(
      begin: _startFOV,
      end: _targetFOV,
    ).animate(CurvedAnimation(
      parent: _zoomController,
      curve: Curves.easeOutCubic,
    ));

    _zoomController.forward(from: 0.0);
  }

  /// Map provider mount status to renderer mount status
  MountRenderStatus _mapMountStatus(MountTrackingStatus status) {
    return switch (status) {
      MountTrackingStatus.disconnected => MountRenderStatus.disconnected,
      MountTrackingStatus.parked => MountRenderStatus.parked,
      MountTrackingStatus.slewing => MountRenderStatus.slewing,
      MountTrackingStatus.tracking => MountRenderStatus.tracking,
      MountTrackingStatus.stopped => MountRenderStatus.stopped,
    };
  }

  @override
  Widget build(BuildContext context) {
    final viewState = ref.watch(skyViewStateProvider);
    final renderConfig = ref.watch(effectiveSkyRenderConfigProvider);
    final location = ref.watch(observerLocationProvider);
    final time = ref.watch(observationTimeProvider);
    final selectedObject = ref.watch(selectedObjectProvider);
    final stars = ref.watch(loadedStarsProvider);
    final dsos = ref.watch(loadedDsosProvider);
    final constellations = ref.watch(constellationDataProvider);
    final equipmentFOV = ref.watch(equipmentFOVProvider);
    final mountPosition = ref.watch(mountPositionProvider);
    final sunPos = ref.watch(sunPositionProvider);
    final moonPos = ref.watch(moonPositionProvider);
    final moonIllumination = ref.watch(moonInfoProvider);
    final planets = ref.watch(planetPositionsProvider);
    final milkyWayPoints = ref.watch(milkyWayPointsProvider);
    final qualityConfig = ref.watch(renderQualityProvider);

    // Handle selection animation
    if (qualityConfig.enableSelectionAnimation) {
      if (selectedObject.coordinates != _lastSelection) {
        _lastSelection = selectedObject.coordinates;
        if (selectedObject.coordinates != null) {
          // Start pulsing animation for new selection
          _selectionController.repeat();
        } else {
          // Stop animation when deselected
          _selectionController.stop();
          _selectionController.reset();
        }
      }
    }

    return LayoutBuilder(
      builder: (context, constraints) {
        return Listener(
          onPointerSignal: (event) {
            if (event is PointerScrollEvent) {
              final currentFOV = ref.read(skyViewStateProvider).fieldOfView;
              // Determine zoom factor (faster at wide FOV, finer at narrow FOV)
              final zoomFactor = currentFOV > 30 ? 1.2 : 1.15;

              if (event.scrollDelta.dy > 0) {
                // Zoom out
                _animateZoom(currentFOV * zoomFactor);
              } else {
                // Zoom in
                _animateZoom(currentFOV / zoomFactor);
              }
            }
          },
          child: GestureDetector(
            onScaleStart: (details) {
              _lastFocalPoint = details.focalPoint;
              _lastScale = 1.0;
              _momentumController.stop();
              _panSamples.clear();
              _panSamples.add(_PanSample(details.focalPoint, DateTime.now()));
            },
            onScaleUpdate: (details) {
              final viewNotifier = ref.read(skyViewStateProvider.notifier);

              // Handle pan
              if (_lastFocalPoint != null) {
                final delta = details.focalPoint - _lastFocalPoint!;
                final panScale = viewState.fieldOfView / 500;
                viewNotifier.pan(
                  -delta.dx * panScale / 15, // Convert to hours
                  delta.dy * panScale,
                );
                _lastFocalPoint = details.focalPoint;

                // Track pan delta for parallax effect (decays over time)
                setState(() {
                  _currentPanDelta = delta;
                });

                // Track pan samples for momentum calculation
                _panSamples.add(_PanSample(details.focalPoint, DateTime.now()));
                // Keep only recent samples
                if (_panSamples.length > 10) {
                  _panSamples.removeAt(0);
                }
              }

              // Handle zoom
              if (_lastScale != null && details.scale != 1.0) {
                final scaleDelta = _lastScale! / details.scale;
                final newFOV = (viewState.fieldOfView * scaleDelta).clamp(1.0, 180.0);
                viewNotifier.setFieldOfView(newFOV);
                _lastScale = details.scale;
              }
            },
            onScaleEnd: (_) {
              // Calculate and apply pan momentum
              _panVelocity = _calculatePanVelocity();
              if (_panVelocity.distance > 50) {
                // Only apply momentum if velocity is significant
                _momentumController.forward(from: 0.0);
              }

              // Reset parallax delta
              setState(() {
                _currentPanDelta = Offset.zero;
              });

              _lastFocalPoint = null;
              _lastScale = null;
              _panSamples.clear();
            },
            onDoubleTap: () {
              // Reset view with animation
              _animateZoom(60);
            },
            onTapUp: (details) {
              _handleTap(details.localPosition, Size(constraints.maxWidth, constraints.maxHeight));
            },
            child: ClipRect(
              child: CustomPaint(
                painter: SkyCanvasPainter(
                  viewState: viewState,
                  config: renderConfig,
                  qualityConfig: qualityConfig,
                  stars: stars.valueOrNull ?? [],
                  dsos: dsos.valueOrNull ?? [],
                  constellations: constellations,
                  observationTime: time.time,
                  latitude: location.latitude,
                  longitude: location.longitude,
                  selectedObject: selectedObject.coordinates,
                  mountPosition: mountPosition.coordinates,
                  mountStatus: _mapMountStatus(mountPosition.status),
                  sunPosition: sunPos,
                  moonPosition: (moonPos.$1, moonPos.$2, moonIllumination.illumination),
                  planets: planets,
                  milkyWayPoints: milkyWayPoints,
                  animationPhase: _twinklePhase,
                  selectionAnimationPhase: qualityConfig.enableSelectionAnimation ? _selectionPhase : null,
                  popinAnimationPhase: qualityConfig.enableStarPopin ? _popinPhase : null,
                  parallaxPanDelta: qualityConfig.enableParallax ? _currentPanDelta : null,
                ),
                foregroundPainter: widget.showFOV
                    ? _FOVOverlayPainter(
                        viewState: viewState,
                        fovWidth: widget.customFOV?.$1 ?? equipmentFOV.fov?.$1,
                        fovHeight: widget.customFOV?.$2 ?? equipmentFOV.fov?.$2,
                        fovCenter: widget.fovCenter,
                        rotation: equipmentFOV.rotation,
                      )
                    : null,
                size: Size.infinite,
              ),
            ),
          ),
        );
      },
    );
  }
  
  void _handleTap(Offset position, Size size) {
    final viewState = ref.read(skyViewStateProvider);

    // Convert screen position to celestial coordinates
    final coord = _screenToCelestial(position, size, viewState);
    if (coord == null) return;

    // Notify coordinate tap
    widget.onCoordinateTapped?.call(coord);

    // Try to find a nearby object
    final stars = ref.read(loadedStarsProvider).valueOrNull ?? [];
    final dsos = ref.read(loadedDsosProvider).valueOrNull ?? [];

    CelestialObject? nearestObject;
    double nearestDistance = double.infinity;

    // Base search radius in degrees - adaptive to FOV with sensible bounds
    // At 60 deg FOV, search ~2 degrees; at 5 deg FOV, search ~0.3 degrees
    final baseSearchRadius = (viewState.fieldOfView / 30).clamp(0.3, 3.0);

    for (final star in stars) {
      final distance = _angularDistance(coord, star.coordinates);
      // Bright stars (mag < 2) get larger hitbox (1.5x) since they're more visible
      final starMag = star.magnitude ?? 6.0;
      final starRadius = baseSearchRadius * (starMag < 2.0 ? 1.5 : 1.0);
      if (distance < starRadius && distance < nearestDistance) {
        nearestDistance = distance;
        nearestObject = star;
      }
    }

    for (final dso in dsos) {
      final distance = _angularDistance(coord, dso.coordinates);
      // DSOs get hitbox based on their actual angular size
      final dsoSizeDeg = (dso.sizeArcMin ?? 5.0) / 60.0;
      // Use at least base radius, or half the object's size (whichever is larger)
      final dsoRadius = math.max(baseSearchRadius, dsoSizeDeg * 0.5);
      if (distance < dsoRadius && distance < nearestDistance) {
        nearestDistance = distance;
        nearestObject = dso;
      }
    }
    
    if (nearestObject != null) {
      ref.read(selectedObjectProvider.notifier).selectObject(nearestObject);
      widget.onObjectSelected?.call(nearestObject);
    } else {
      ref.read(selectedObjectProvider.notifier).selectCoordinates(coord);
      widget.onObjectSelected?.call(null);
    }
    
    // Always call the position callback for popup handling
    widget.onObjectTapped?.call(nearestObject, coord, position);
  }
  
  CelestialCoordinate? _screenToCelestial(Offset position, Size size, SkyViewState viewState) {
    final center = Offset(size.width / 2, size.height / 2);
    final scale = math.min(size.width, size.height) / 2 / (viewState.fieldOfView / 2);
    
    // Offset from center in screen pixels
    final dx = -(position.dx - center.dx) / scale;
    final dy = -(position.dy - center.dy) / scale;
    
    // Reverse rotation
    final rotRad = -viewState.rotation * math.pi / 180;
    final x = dx * math.cos(rotRad) - dy * math.sin(rotRad);
    final y = dx * math.sin(rotRad) + dy * math.cos(rotRad);
    
    // Convert to RA/Dec (inverse of stereographic projection)
    final centerRaDeg = viewState.centerRA * 15;
    final centerDecDeg = viewState.centerDec;
    
    final xRad = x * math.pi / 180;
    final yRad = y * math.pi / 180;
    final centerRaRad = centerRaDeg * math.pi / 180;
    final centerDecRad = centerDecDeg * math.pi / 180;
    
    final rho = math.sqrt(xRad * xRad + yRad * yRad);
    if (rho < 0.0001) {
      return CelestialCoordinate(ra: viewState.centerRA, dec: viewState.centerDec);
    }
    
    final c = 2 * math.atan(rho / 2);
    
    final sinc = math.sin(c);
    final cosc = math.cos(c);
    
    final dec = math.asin(cosc * math.sin(centerDecRad) + yRad * sinc * math.cos(centerDecRad) / rho);
    final ra = centerRaRad + math.atan2(
      xRad * sinc,
      rho * math.cos(centerDecRad) * cosc - yRad * math.sin(centerDecRad) * sinc,
    );
    
    var raHours = ra * 180 / math.pi / 15;
    if (raHours < 0) raHours += 24;
    if (raHours >= 24) raHours -= 24;
    
    final decDeg = dec * 180 / math.pi;
    
    return CelestialCoordinate(ra: raHours, dec: decDeg.clamp(-90, 90));
  }
  
  double _angularDistance(CelestialCoordinate a, CelestialCoordinate b) {
    final ra1 = a.ra * 15 * math.pi / 180;
    final dec1 = a.dec * math.pi / 180;
    final ra2 = b.ra * 15 * math.pi / 180;
    final dec2 = b.dec * math.pi / 180;
    
    final cosSep = math.sin(dec1) * math.sin(dec2) +
                   math.cos(dec1) * math.cos(dec2) * math.cos(ra1 - ra2);
    
    return math.acos(cosSep.clamp(-1.0, 1.0)) * 180 / math.pi;
  }
}

/// FOV rectangle overlay painter
class _FOVOverlayPainter extends CustomPainter {
  final SkyViewState viewState;
  final double? fovWidth;
  final double? fovHeight;
  final CelestialCoordinate? fovCenter;
  final double rotation;
  
  _FOVOverlayPainter({
    required this.viewState,
    this.fovWidth,
    this.fovHeight,
    this.fovCenter,
    this.rotation = 0,
  });
  
  @override
  void paint(Canvas canvas, Size size) {
    if (fovWidth == null || fovHeight == null) return;
    
    final center = Offset(size.width / 2, size.height / 2);
    final scale = math.min(size.width, size.height) / 2 / (viewState.fieldOfView / 2);
    
    // Convert FOV to screen pixels
    final rectWidth = fovWidth! * scale;
    final rectHeight = fovHeight! * scale;
    
    // Calculate offset if FOV center is different
    Offset rectCenter = center;
    if (fovCenter != null) {
      // TODO: Calculate proper offset for different FOV center
    }
    
    // Draw FOV rectangle
    canvas.save();
    canvas.translate(rectCenter.dx, rectCenter.dy);
    canvas.rotate((rotation + viewState.rotation) * math.pi / 180);
    
    final rect = Rect.fromCenter(
      center: Offset.zero,
      width: rectWidth,
      height: rectHeight,
    );
    
    // Draw border
    final borderPaint = Paint()
      ..color = const Color(0xFF00E676)
      ..strokeWidth = 2
      ..style = PaintingStyle.stroke;
    
    canvas.drawRect(rect, borderPaint);
    
    // Draw corner brackets
    final bracketLength = math.min(rectWidth, rectHeight) * 0.1;
    final bracketPaint = Paint()
      ..color = const Color(0xFF00E676)
      ..strokeWidth = 3
      ..style = PaintingStyle.stroke;
    
    // Top-left
    canvas.drawLine(
      Offset(-rectWidth / 2, -rectHeight / 2 + bracketLength),
      Offset(-rectWidth / 2, -rectHeight / 2),
      bracketPaint,
    );
    canvas.drawLine(
      Offset(-rectWidth / 2, -rectHeight / 2),
      Offset(-rectWidth / 2 + bracketLength, -rectHeight / 2),
      bracketPaint,
    );
    
    // Top-right
    canvas.drawLine(
      Offset(rectWidth / 2 - bracketLength, -rectHeight / 2),
      Offset(rectWidth / 2, -rectHeight / 2),
      bracketPaint,
    );
    canvas.drawLine(
      Offset(rectWidth / 2, -rectHeight / 2),
      Offset(rectWidth / 2, -rectHeight / 2 + bracketLength),
      bracketPaint,
    );
    
    // Bottom-right
    canvas.drawLine(
      Offset(rectWidth / 2, rectHeight / 2 - bracketLength),
      Offset(rectWidth / 2, rectHeight / 2),
      bracketPaint,
    );
    canvas.drawLine(
      Offset(rectWidth / 2, rectHeight / 2),
      Offset(rectWidth / 2 - bracketLength, rectHeight / 2),
      bracketPaint,
    );
    
    // Bottom-left
    canvas.drawLine(
      Offset(-rectWidth / 2 + bracketLength, rectHeight / 2),
      Offset(-rectWidth / 2, rectHeight / 2),
      bracketPaint,
    );
    canvas.drawLine(
      Offset(-rectWidth / 2, rectHeight / 2),
      Offset(-rectWidth / 2, rectHeight / 2 - bracketLength),
      bracketPaint,
    );
    
    // Draw center crosshair
    final crosshairPaint = Paint()
      ..color = const Color(0xFF00E676).withValues(alpha: 0.5)
      ..strokeWidth = 1;
    
    canvas.drawLine(
      Offset(-15, 0),
      Offset(15, 0),
      crosshairPaint,
    );
    canvas.drawLine(
      Offset(0, -15),
      Offset(0, 15),
      crosshairPaint,
    );
    
    // Draw rotation indicator
    if (rotation != 0) {
      canvas.drawLine(
        Offset(0, -rectHeight / 2 - 20),
        Offset(0, -rectHeight / 2 - 5),
        borderPaint,
      );
    }
    
    canvas.restore();
    
    // Draw FOV dimensions label
    final fovText = '${fovWidth!.toStringAsFixed(2)}° × ${fovHeight!.toStringAsFixed(2)}°';
    final textPainter = TextPainter(
      text: TextSpan(
        text: fovText,
        style: const TextStyle(
          color: Color(0xFF00E676),
          fontSize: 11,
          fontWeight: FontWeight.w500,
        ),
      ),
      textDirection: TextDirection.ltr,
    );
    textPainter.layout();
    textPainter.paint(
      canvas,
      Offset(
        rectCenter.dx - textPainter.width / 2,
        rectCenter.dy + rectHeight / 2 + 10,
      ),
    );
  }
  
  @override
  bool shouldRepaint(covariant _FOVOverlayPainter oldDelegate) {
    return viewState != oldDelegate.viewState ||
           fovWidth != oldDelegate.fovWidth ||
           fovHeight != oldDelegate.fovHeight ||
           rotation != oldDelegate.rotation;
  }
}

/// Sky view toolbar widget
class SkyViewToolbar extends ConsumerWidget {
  /// Whether to show extended options (solar system objects, milky way)
  final bool showExtendedOptions;

  const SkyViewToolbar({
    super.key,
    this.showExtendedOptions = true,
  });

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final config = ref.watch(skyRenderConfigProvider);
    final configNotifier = ref.read(skyRenderConfigProvider.notifier);

    return Wrap(
      spacing: 8,
      runSpacing: 6,
      children: [
        _ToolbarToggle(
          label: 'Stars',
          isActive: config.showStars,
          onTap: configNotifier.toggleStars,
        ),
        _ToolbarToggle(
          label: 'Constellations',
          isActive: config.showConstellationLines,
          onTap: configNotifier.toggleConstellationLines,
        ),
        _ToolbarToggle(
          label: 'DSOs',
          isActive: config.showDSOs,
          onTap: configNotifier.toggleDSOs,
        ),
        _ToolbarToggle(
          label: 'Grid',
          isActive: config.showCoordinateGrid,
          onTap: configNotifier.toggleGrid,
        ),
        _ToolbarToggle(
          label: 'Horizon',
          isActive: config.showHorizon,
          onTap: configNotifier.toggleHorizon,
        ),
        _ToolbarToggle(
          label: 'Ecliptic',
          isActive: config.showEcliptic,
          onTap: configNotifier.toggleEcliptic,
        ),
        if (showExtendedOptions) ...[
          _ToolbarToggle(
            label: 'Milky Way',
            isActive: config.showMilkyWay,
            onTap: configNotifier.toggleMilkyWay,
          ),
          _ToolbarToggle(
            label: 'Sun',
            isActive: config.showSun,
            onTap: configNotifier.toggleSun,
          ),
          _ToolbarToggle(
            label: 'Moon',
            isActive: config.showMoon,
            onTap: configNotifier.toggleMoon,
          ),
          _ToolbarToggle(
            label: 'Planets',
            isActive: config.showPlanets,
            onTap: configNotifier.togglePlanets,
          ),
        ],
      ],
    );
  }
}

class _ToolbarToggle extends StatelessWidget {
  final String label;
  final bool isActive;
  final VoidCallback onTap;
  
  const _ToolbarToggle({
    required this.label,
    required this.isActive,
    required this.onTap,
  });
  
  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: onTap,
      child: Container(
        padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 6),
        decoration: BoxDecoration(
          color: isActive
              ? Colors.white.withValues(alpha: 0.2)
              : Colors.black.withValues(alpha: 0.3),
          borderRadius: BorderRadius.circular(16),
          border: Border.all(
            color: isActive
                ? Colors.white.withValues(alpha: 0.5)
                : Colors.white.withValues(alpha: 0.1),
          ),
        ),
        child: Text(
          label,
          style: TextStyle(
            fontSize: 11,
            color: isActive ? Colors.white : Colors.white70,
            fontWeight: isActive ? FontWeight.w600 : FontWeight.normal,
          ),
        ),
      ),
    );
  }
}

/// Helper class for tracking pan velocity samples
class _PanSample {
  final Offset position;
  final DateTime time;

  _PanSample(this.position, this.time);
}

