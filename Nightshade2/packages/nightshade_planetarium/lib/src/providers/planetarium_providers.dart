import 'dart:async';
import 'dart:math' as math;
import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import '../celestial_object.dart';
import '../coordinate_system.dart';
import '../catalogs/star_catalog.dart';
import '../catalogs/constellation_data.dart';
import '../catalogs/catalog.dart';
import '../astronomy/astronomy_calculations.dart';
import '../astronomy/planetary_positions.dart';
import '../astronomy/milky_way_data.dart';
import '../rendering/sky_renderer.dart';
import '../rendering/render_quality.dart';
import '../services/survey_image_service.dart';
import '../services/mosaic_planner.dart';

/// Get display name for search matching
(String, String) _getDsoDisplayInfoForSearch(DeepSkyObject dso) {
  // If it's a Messier object, use Messier number as name
  if (dso.isMessier) {
    final messierNum = dso.messierNumber;
    if (messierNum != null) {
      return (messierNum, 'M');
    }
  }
  
  // For non-Messier objects, use NGC/IC designation as name
  final ngcIc = dso.ngcIcDesignation;
  if (ngcIc != null) {
    if (ngcIc.startsWith('NGC')) {
      return (ngcIc, 'NGC');
    } else if (ngcIc.startsWith('IC')) {
      return (ngcIc, 'IC');
    }
  }
  
  // Fallback to id and extract catalog prefix
  if (dso.id.startsWith('NGC')) {
    return (dso.id, 'NGC');
  } else if (dso.id.startsWith('IC')) {
    return (dso.id, 'IC');
  } else if (dso.id.startsWith('M')) {
    return (dso.id, 'M');
  }
  
  // Last resort: use name and id
  return (dso.name, dso.id);
}

// ============================================================================
// Location Provider
// ============================================================================

/// Observer location state
class ObserverLocation {
  final double latitude;
  final double longitude;
  final double elevation;
  final String? locationName;
  
  const ObserverLocation({
    this.latitude = 34.0522, // Los Angeles default
    this.longitude = -118.2437,
    this.elevation = 0,
    this.locationName,
  });
  
  ObserverLocation copyWith({
    double? latitude,
    double? longitude,
    double? elevation,
    String? locationName,
  }) {
    return ObserverLocation(
      latitude: latitude ?? this.latitude,
      longitude: longitude ?? this.longitude,
      elevation: elevation ?? this.elevation,
      locationName: locationName ?? this.locationName,
    );
  }
}

class ObserverLocationNotifier extends StateNotifier<ObserverLocation> {
  ObserverLocationNotifier() : super(const ObserverLocation());
  
  void setLocation({
    double? latitude,
    double? longitude,
    double? elevation,
    String? locationName,
  }) {
    state = state.copyWith(
      latitude: latitude,
      longitude: longitude,
      elevation: elevation,
      locationName: locationName,
    );
    
    // Settings sync will be handled at app level
  }
}

final observerLocationProvider = StateNotifierProvider<ObserverLocationNotifier, ObserverLocation>((ref) {
  return ObserverLocationNotifier();
});

// ============================================================================
// Observation Time Provider
// ============================================================================

/// Current observation time (can be simulated or real-time)
class ObservationTimeState {
  final DateTime time;
  final bool isRealTime;
  final double speedMultiplier;
  
  const ObservationTimeState({
    required this.time,
    this.isRealTime = true,
    this.speedMultiplier = 1.0,
  });
  
  ObservationTimeState copyWith({
    DateTime? time,
    bool? isRealTime,
    double? speedMultiplier,
  }) {
    return ObservationTimeState(
      time: time ?? this.time,
      isRealTime: isRealTime ?? this.isRealTime,
      speedMultiplier: speedMultiplier ?? this.speedMultiplier,
    );
  }
}

class ObservationTimeNotifier extends StateNotifier<ObservationTimeState> {
  Timer? _timer;
  
  ObservationTimeNotifier() : super(ObservationTimeState(time: DateTime.now())) {
    _startTimer();
  }
  
  void _startTimer() {
    _timer?.cancel();
    _timer = Timer.periodic(const Duration(seconds: 1), (_) {
      if (state.isRealTime) {
        state = state.copyWith(time: DateTime.now());
      } else if (state.speedMultiplier != 0) {
        final delta = Duration(seconds: state.speedMultiplier.round());
        state = state.copyWith(time: state.time.add(delta));
      }
    });
  }
  
  void setTime(DateTime time) {
    state = state.copyWith(time: time, isRealTime: false);
  }
  
  void setRealTime(bool realTime) {
    state = state.copyWith(
      isRealTime: realTime,
      time: realTime ? DateTime.now() : state.time,
    );
  }
  
  void setSpeedMultiplier(double multiplier) {
    state = state.copyWith(speedMultiplier: multiplier, isRealTime: false);
  }
  
  void fastForward(Duration duration) {
    state = state.copyWith(
      time: state.time.add(duration),
      isRealTime: false,
    );
  }
  
  void rewind(Duration duration) {
    state = state.copyWith(
      time: state.time.subtract(duration),
      isRealTime: false,
    );
  }
  
  @override
  void dispose() {
    _timer?.cancel();
    super.dispose();
  }
}

final observationTimeProvider = StateNotifierProvider<ObservationTimeNotifier, ObservationTimeState>((ref) {
  return ObservationTimeNotifier();
});

// ============================================================================
// Sky View State Provider
// ============================================================================

class SkyViewNotifier extends StateNotifier<SkyViewState> {
  SkyViewNotifier() : super(const SkyViewState(
    centerRA: 0,
    centerDec: 0,
    fieldOfView: 60,
  ));
  
  void setCenter(double ra, double dec) {
    state = state.copyWith(
      centerRA: ra.clamp(0, 24),
      centerDec: dec.clamp(-90, 90),
    );
  }
  
  void setFieldOfView(double fov) {
    state = state.copyWith(fieldOfView: fov.clamp(1, 180));
  }
  
  void setRotation(double rotation) {
    state = state.copyWith(rotation: rotation % 360);
  }
  
  void setProjection(SkyProjection projection) {
    state = state.copyWith(projection: projection);
  }
  
  void zoomIn({Offset? mousePosition, Size? viewSize}) {
    if (mousePosition != null && viewSize != null) {
      _zoomAtPosition(mousePosition, viewSize, 1.5);
    } else {
      state = state.copyWith(fieldOfView: (state.fieldOfView / 1.5).clamp(1, 180));
    }
  }
  
  void zoomOut({Offset? mousePosition, Size? viewSize}) {
    if (mousePosition != null && viewSize != null) {
      _zoomAtPosition(mousePosition, viewSize, 1 / 1.5);
    } else {
      state = state.copyWith(fieldOfView: (state.fieldOfView * 1.5).clamp(1, 180));
    }
  }
  
  /// Zoom at a specific screen position, keeping that position fixed
  void _zoomAtPosition(Offset mousePosition, Size viewSize, double zoomFactor) {
    // Get the celestial coordinate at the mouse position before zoom
    final coordBefore = _screenToCelestial(mousePosition, viewSize);
    if (coordBefore == null) {
      // Fallback to center zoom if conversion fails
      state = state.copyWith(fieldOfView: (state.fieldOfView / zoomFactor).clamp(1, 180));
      return;
    }
    
    // Apply zoom
    final oldFOV = state.fieldOfView;
    final newFOV = (oldFOV / zoomFactor).clamp(1.0, 180.0);
    state = state.copyWith(fieldOfView: newFOV);
    
    // Get the celestial coordinate at the same screen position after zoom
    final coordAfter = _screenToCelestial(mousePosition, viewSize);
    if (coordAfter == null) return;
    
    // Calculate the offset needed to keep the mouse position pointing at the same celestial coordinate
    final dRA = coordBefore.ra - coordAfter.ra;
    final dDec = coordBefore.dec - coordAfter.dec;
    
    // Adjust center to compensate
    var newRA = state.centerRA + dRA;
    if (newRA < 0) newRA += 24;
    if (newRA >= 24) newRA -= 24;
    
    state = state.copyWith(
      centerRA: newRA,
      centerDec: (state.centerDec + dDec).clamp(-90, 90),
    );
  }
  
  /// Convert screen position to celestial coordinates
  CelestialCoordinate? _screenToCelestial(Offset position, Size size) {
    final center = Offset(size.width / 2, size.height / 2);
    final scale = math.min(size.width, size.height) / 2 / (state.fieldOfView / 2);
    
    // Offset from center in screen pixels
    final dx = -(position.dx - center.dx) / scale;
    final dy = -(position.dy - center.dy) / scale;
    
    // Reverse rotation
    final rotRad = -state.rotation * math.pi / 180;
    final x = dx * math.cos(rotRad) - dy * math.sin(rotRad);
    final y = dx * math.sin(rotRad) + dy * math.cos(rotRad);
    
    // Convert to RA/Dec (inverse of stereographic projection)
    final centerRaDeg = state.centerRA * 15;
    final centerDecDeg = state.centerDec;
    
    final xRad = x * math.pi / 180;
    final yRad = y * math.pi / 180;
    final centerRaRad = centerRaDeg * math.pi / 180;
    final centerDecRad = centerDecDeg * math.pi / 180;
    
    final rho = math.sqrt(xRad * xRad + yRad * yRad);
    if (rho < 0.0001) {
      return CelestialCoordinate(ra: state.centerRA, dec: state.centerDec);
    }
    
    final c = 2 * math.atan(rho / 2);
    final sinc = math.sin(c);
    final cosc = math.cos(c);
    
    final dec = math.asin(cosc * math.sin(centerDecRad) + yRad * sinc * math.cos(centerDecRad) / rho);
    final ra = centerRaRad + math.atan2(
      xRad * sinc,
      rho * math.cos(centerDecRad) * cosc - yRad * math.sin(centerDecRad) * sinc,
    );
    
    var raHours = (ra * 180 / math.pi / 15).toDouble();
    if (raHours < 0) raHours += 24;
    if (raHours >= 24) raHours -= 24;
    
    final decDeg = (dec * 180 / math.pi).toDouble();
    
    return CelestialCoordinate(ra: raHours, dec: decDeg.clamp(-90, 90));
  }
  
  void pan(double dRA, double dDec) {
    var newRA = state.centerRA + dRA;
    if (newRA < 0) newRA += 24;
    if (newRA >= 24) newRA -= 24;
    
    state = state.copyWith(
      centerRA: newRA,
      centerDec: (state.centerDec + dDec).clamp(-90, 90),
    );
  }
  
  void lookAt(CelestialCoordinate coord) {
    state = state.copyWith(centerRA: coord.ra, centerDec: coord.dec);
  }
}

final skyViewStateProvider = StateNotifierProvider<SkyViewNotifier, SkyViewState>((ref) {
  return SkyViewNotifier();
});

/// Computed provider for current view center in horizontal coordinates
/// Returns (azimuth, altitude) in degrees
/// Uses minute precision to avoid excessive rebuilds from per-second time updates.
final viewCenterAltAzProvider = Provider<(double, double)>((ref) {
  final viewState = ref.watch(skyViewStateProvider);
  final location = ref.watch(observerLocationProvider);
  final time = ref.watch(_currentMinuteProvider);  // Use minute precision instead

  // Convert view center (RA/Dec) to Alt/Az
  final lst = AstronomyCalculations.localSiderealTime(time, location.longitude);

  final (alt, az) = AstronomyCalculations.equatorialToHorizontal(
    raDeg: viewState.centerRA * 15, // Convert hours to degrees
    decDeg: viewState.centerDec,
    latitudeDeg: location.latitude,
    lstHours: lst,
  );

  return (az, alt);
});

// ============================================================================
// HUD Toggle Providers
// ============================================================================

/// Whether to show the compass HUD
final showCompassHudProvider = StateProvider<bool>((ref) => true);

/// Whether to show the mini-map
final showMinimapProvider = StateProvider<bool>((ref) => true);

/// Whether to show the ground plane
final showGroundPlaneProvider = StateProvider<bool>((ref) => true);

// ============================================================================
// Sky Render Config Provider
// ============================================================================

class SkyRenderConfigNotifier extends StateNotifier<SkyRenderConfig> {
  SkyRenderConfigNotifier() : super(const SkyRenderConfig());
  
  void toggleStars() {
    state = state.copyWith(showStars: !state.showStars);
  }
  
  void toggleConstellationLines() {
    state = state.copyWith(showConstellationLines: !state.showConstellationLines);
  }
  
  void toggleConstellationLabels() {
    state = state.copyWith(showConstellationLabels: !state.showConstellationLabels);
  }
  
  void toggleDSOs() {
    state = state.copyWith(showDSOs: !state.showDSOs);
  }
  
  void toggleGrid() {
    state = state.copyWith(showCoordinateGrid: !state.showCoordinateGrid);
  }
  
  void toggleEquatorialGrid() {
    state = state.copyWith(showEquatorialGrid: !state.showEquatorialGrid);
  }
  
  void toggleAltAzGrid() {
    state = state.copyWith(showAltAzGrid: !state.showAltAzGrid);
  }
  
  void toggleEcliptic() {
    state = state.copyWith(showEcliptic: !state.showEcliptic);
  }
  
  void toggleHorizon() {
    state = state.copyWith(showHorizon: !state.showHorizon);
  }
  
  void setStarMagnitudeLimit(double limit) {
    state = state.copyWith(starMagnitudeLimit: limit);
  }
  
  void setDsoMagnitudeLimit(double limit) {
    state = state.copyWith(dsoMagnitudeLimit: limit);
  }

  void toggleMountPosition() {
    state = state.copyWith(showMountPosition: !state.showMountPosition);
  }

  void toggleMilkyWay() {
    state = state.copyWith(showMilkyWay: !state.showMilkyWay);
  }

  void toggleSun() {
    state = state.copyWith(showSun: !state.showSun);
  }

  void toggleMoon() {
    state = state.copyWith(showMoon: !state.showMoon);
  }

  void togglePlanets() {
    state = state.copyWith(showPlanets: !state.showPlanets);
  }

  void toggleGroundPlane() {
    state = state.copyWith(showGroundPlane: !state.showGroundPlane);
  }
}

final skyRenderConfigProvider = StateNotifierProvider<SkyRenderConfigNotifier, SkyRenderConfig>((ref) {
  return SkyRenderConfigNotifier();
});

/// Computed render config that combines the base config with the ground plane toggle
/// This is the provider that should be used for actual rendering to ensure
/// the ground plane visibility respects the HUD toggle.
final effectiveSkyRenderConfigProvider = Provider<SkyRenderConfig>((ref) {
  final config = ref.watch(skyRenderConfigProvider);
  final showGroundPlane = ref.watch(showGroundPlaneProvider);
  return config.copyWith(showGroundPlane: showGroundPlane);
});

// ============================================================================
// Render Quality Provider
// ============================================================================

/// Notifier for managing render quality settings
class RenderQualityNotifier extends StateNotifier<RenderQualityConfig> {
  RenderQualityNotifier() : super(const RenderQualityConfig.balanced());

  /// Set the quality tier
  void setQuality(RenderQuality quality) {
    state = RenderQualityConfig.fromQuality(quality);
  }

  /// Set a custom configuration
  void setConfig(RenderQualityConfig config) {
    state = config;
  }

  /// Toggle a specific setting
  void toggleBlurEffects() {
    state = state.copyWith(useBlurEffects: !state.useBlurEffects);
  }

  void toggleGlowEffects() {
    state = state.copyWith(useGlowEffects: !state.useGlowEffects);
  }

  void toggleStarTwinkle() {
    state = state.copyWith(animateStarTwinkle: !state.animateStarTwinkle);
  }

  void toggleSmoothZoom() {
    state = state.copyWith(smoothZoomAnimation: !state.smoothZoomAnimation);
  }

  void setMilkyWayDetail(double detail) {
    state = state.copyWith(milkyWayDetail: detail.clamp(0.0, 1.0));
  }

  void setStarMagnitudeLimit(double limit) {
    state = state.copyWith(starMagnitudeLimit: limit);
  }

  void setDsoMagnitudeLimit(double limit) {
    state = state.copyWith(dsoMagnitudeLimit: limit);
  }
}

/// Provider for render quality configuration
final renderQualityProvider = StateNotifierProvider<RenderQualityNotifier, RenderQualityConfig>((ref) {
  return RenderQualityNotifier();
});

/// Computed magnitude limits based on current FOV
/// Returns (starMagLimit, dsoMagLimit)
///
/// As the user zooms in (narrower FOV), fainter objects become visible.
/// This provides a more natural experience where zooming reveals more detail.
final dynamicMagnitudeLimitsProvider = Provider<(double, double)>((ref) {
  final viewState = ref.watch(skyViewStateProvider);
  final quality = ref.watch(renderQualityProvider);
  final fov = viewState.fieldOfView;

  // Base limits from quality tier
  final baseStarLimit = quality.starMagnitudeLimit;
  final baseDsoLimit = quality.dsoMagnitudeLimit;

  // Scale limits based on FOV (narrower FOV = deeper limits)
  // FOV 90°+ = base limits, FOV 5° = base + 4.5 magnitudes
  double fovFactor;
  if (fov >= 90) {
    fovFactor = 0.0;
  } else if (fov >= 60) {
    fovFactor = 1.0;
  } else if (fov >= 30) {
    fovFactor = 2.0;
  } else if (fov >= 15) {
    fovFactor = 3.0;
  } else {
    fovFactor = 4.5;
  }

  return (
    (baseStarLimit + fovFactor).clamp(4.5, 12.0),
    (baseDsoLimit + fovFactor).clamp(8.0, 16.0),
  );
});

// ============================================================================
// Catalog Providers
// ============================================================================

final loadedStarsProvider = FutureProvider<List<Star>>((ref) async {
  // Load stars up to magnitude 10.0 to allow deeper viewing when zoomed in
  // The dynamic magnitude limit will filter these based on FOV
  return HygStarCatalog(magnitudeLimit: 10.0).loadObjects();
});

final loadedDsosProvider = FutureProvider<List<DeepSkyObject>>((ref) async {
  // Load DSOs up to magnitude 14.0 to include faint imaging targets when zoomed in
  // The dynamic magnitude limit will filter these based on FOV
  return OpenNgcDsoCatalog(magnitudeLimit: 14.0).loadObjects();
});

/// Stars filtered by dynamic magnitude limit based on current FOV
/// As the user zooms in (narrower FOV), fainter stars become visible.
/// This provider should be used by the sky renderer for FOV-aware star display.
final fovFilteredStarsProvider = Provider<AsyncValue<List<Star>>>((ref) {
  final starsAsync = ref.watch(loadedStarsProvider);
  final (starMagLimit, _) = ref.watch(dynamicMagnitudeLimitsProvider);

  return starsAsync.whenData((stars) {
    return stars.where((star) => (star.magnitude ?? 99) <= starMagLimit).toList();
  });
});

/// DSOs filtered by dynamic magnitude limit based on current FOV
/// As the user zooms in (narrower FOV), fainter DSOs become visible.
/// This provider should be used by the sky renderer for FOV-aware DSO display.
final fovFilteredDsosProvider = Provider<AsyncValue<List<DeepSkyObject>>>((ref) {
  final dsosAsync = ref.watch(loadedDsosProvider);
  final (_, dsoMagLimit) = ref.watch(dynamicMagnitudeLimitsProvider);

  return dsosAsync.whenData((dsos) {
    return dsos.where((dso) => (dso.magnitude ?? 99) <= dsoMagLimit).toList();
  });
});

final constellationDataProvider = Provider<List<ConstellationData>>((ref) {
  return Constellations.all;
});

// ============================================================================
// Computed Astronomy Data Providers
// ============================================================================

/// Provider that only updates when the date changes (not every second)
/// This prevents unnecessary recalculations of date-dependent values like twilight.
final _currentDateProvider = Provider<DateTime>((ref) {
  final time = ref.watch(observationTimeProvider);
  // Return only the date portion, so it only changes at midnight
  return DateTime(time.time.year, time.time.month, time.time.day);
});

/// Provider that only updates when the minute changes (not every second)
/// This prevents unnecessary recalculations of astronomical positions
/// which don't need per-second precision for sky rendering.
final _currentMinuteProvider = Provider<DateTime>((ref) {
  final time = ref.watch(observationTimeProvider);
  // Return only up to minute precision, ignoring seconds
  return DateTime(
    time.time.year,
    time.time.month,
    time.time.day,
    time.time.hour,
    time.time.minute,
  );
});

/// Twilight times for current date and location
/// Uses date-level precision since twilight only changes once per day.
final twilightTimesProvider = Provider<TwilightTimes>((ref) {
  final location = ref.watch(observerLocationProvider);
  final currentDate = ref.watch(_currentDateProvider);

  return AstronomyCalculations.calculateTwilightTimes(
    date: currentDate,
    latitudeDeg: location.latitude,
    longitudeDeg: location.longitude,
  );
});

/// Moon information for current time and location
/// Uses date precision for rise/set, minute precision for illumination.
final moonInfoProvider = Provider<MoonTimes>((ref) {
  final location = ref.watch(observerLocationProvider);
  final currentDate = ref.watch(_currentDateProvider);
  final currentMinute = ref.watch(_currentMinuteProvider);

  // Calculate rise/set times for the date
  final moonTimes = AstronomyCalculations.calculateMoonTimes(
    date: currentDate,
    latitudeDeg: location.latitude,
    longitudeDeg: location.longitude,
  );

  // Calculate phase and illumination - minute precision is sufficient
  final illumination = AstronomyCalculations.moonIllumination(currentMinute);
  final phaseName = AstronomyCalculations.moonPhaseName(currentMinute);

  // Return combined data
  return MoonTimes(
    moonrise: moonTimes.moonrise,
    moonset: moonTimes.moonset,
    illumination: illumination,
    phaseName: phaseName,
  );
});

/// Current Local Sidereal Time
/// Needs per-second precision for accurate clock display.
final localSiderealTimeProvider = Provider<double>((ref) {
  final location = ref.watch(observerLocationProvider);
  final time = ref.watch(observationTimeProvider);

  return AstronomyCalculations.localSiderealTime(time.time, location.longitude);
});

/// Sun position for current time
/// Uses minute precision - sun moves ~0.25 degrees per minute which is fine for rendering.
final sunPositionProvider = Provider<(double ra, double dec)>((ref) {
  final time = ref.watch(_currentMinuteProvider);
  return AstronomyCalculations.sunPosition(time);
});

/// Moon position for current time
/// Uses minute precision - moon moves ~0.5 arcmin per minute which is fine for rendering.
final moonPositionProvider = Provider<(double ra, double dec, double distance)>((ref) {
  final time = ref.watch(_currentMinuteProvider);
  return AstronomyCalculations.moonPosition(time);
});

/// Planet positions for current time
/// Uses minute precision - planets move very slowly, minute precision is more than enough.
final planetPositionsProvider = Provider<List<PlanetData>>((ref) {
  final time = ref.watch(_currentMinuteProvider);
  return PlanetaryPositions.getAllPlanetPositions(time);
});

/// Milky Way points for rendering (static, only needs to be generated once)
final milkyWayPointsProvider = Provider<List<MilkyWayPoint>>((ref) {
  return MilkyWayData.generateMilkyWayPoints();
});

// ============================================================================
// Mount Position Provider
// ============================================================================

/// Tracking status for the mount
enum MountTrackingStatus {
  disconnected,
  parked,
  slewing,
  tracking,
  stopped,
}

/// Mount position state for displaying on planetarium
class MountPositionState {
  final double? raHours;
  final double? decDegrees;
  final MountTrackingStatus status;
  final bool isConnected;

  const MountPositionState({
    this.raHours,
    this.decDegrees,
    this.status = MountTrackingStatus.disconnected,
    this.isConnected = false,
  });

  /// Get the mount position as celestial coordinates
  CelestialCoordinate? get coordinates {
    if (raHours == null || decDegrees == null) return null;
    return CelestialCoordinate(ra: raHours!, dec: decDegrees!);
  }

  MountPositionState copyWith({
    double? raHours,
    double? decDegrees,
    MountTrackingStatus? status,
    bool? isConnected,
  }) {
    return MountPositionState(
      raHours: raHours ?? this.raHours,
      decDegrees: decDegrees ?? this.decDegrees,
      status: status ?? this.status,
      isConnected: isConnected ?? this.isConnected,
    );
  }
}

class MountPositionNotifier extends StateNotifier<MountPositionState> {
  MountPositionNotifier() : super(const MountPositionState());

  /// Update the mount position from external source (e.g., equipment provider)
  void updatePosition({
    required double? raHours,
    required double? decDegrees,
    required MountTrackingStatus status,
    required bool isConnected,
  }) {
    state = MountPositionState(
      raHours: raHours,
      decDegrees: decDegrees,
      status: status,
      isConnected: isConnected,
    );
  }

  void setDisconnected() {
    state = const MountPositionState();
  }
}

final mountPositionProvider = StateNotifierProvider<MountPositionNotifier, MountPositionState>((ref) {
  return MountPositionNotifier();
});

// ============================================================================
// Selected Object Provider
// ============================================================================

/// Currently selected celestial object
class SelectedObjectState {
  final CelestialObject? object;
  final CelestialCoordinate? coordinates;
  final ObjectVisibility? visibility;
  final (double alt, double az)? currentAltAz;
  
  const SelectedObjectState({
    this.object,
    this.coordinates,
    this.visibility,
    this.currentAltAz,
  });
}

class SelectedObjectNotifier extends StateNotifier<SelectedObjectState> {
  final Ref _ref;
  
  SelectedObjectNotifier(this._ref) : super(const SelectedObjectState());
  
  void selectObject(CelestialObject object) {
    final location = _ref.read(observerLocationProvider);
    final time = _ref.read(observationTimeProvider);
    
    final visibility = AstronomyCalculations.calculateObjectVisibility(
      raDeg: object.coordinates.raDegrees,
      decDeg: object.coordinates.dec,
      date: time.time,
      latitudeDeg: location.latitude,
      longitudeDeg: location.longitude,
    );
    
    final altAz = AstronomyCalculations.objectAltAz(
      raDeg: object.coordinates.raDegrees,
      decDeg: object.coordinates.dec,
      dt: time.time,
      latitudeDeg: location.latitude,
      longitudeDeg: location.longitude,
    );
    
    state = SelectedObjectState(
      object: object,
      coordinates: object.coordinates,
      visibility: visibility,
      currentAltAz: altAz,
    );
  }
  
  void selectCoordinates(CelestialCoordinate coord) {
    final location = _ref.read(observerLocationProvider);
    final time = _ref.read(observationTimeProvider);
    
    final visibility = AstronomyCalculations.calculateObjectVisibility(
      raDeg: coord.raDegrees,
      decDeg: coord.dec,
      date: time.time,
      latitudeDeg: location.latitude,
      longitudeDeg: location.longitude,
    );
    
    final altAz = AstronomyCalculations.objectAltAz(
      raDeg: coord.raDegrees,
      decDeg: coord.dec,
      dt: time.time,
      latitudeDeg: location.latitude,
      longitudeDeg: location.longitude,
    );
    
    state = SelectedObjectState(
      coordinates: coord,
      visibility: visibility,
      currentAltAz: altAz,
    );
  }
  
  void clearSelection() {
    state = const SelectedObjectState();
  }
}

final selectedObjectProvider = StateNotifierProvider<SelectedObjectNotifier, SelectedObjectState>((ref) {
  return SelectedObjectNotifier(ref);
});

// ============================================================================
// Equipment FOV Provider
// ============================================================================

/// Equipment configuration for FOV display
class EquipmentFOVState {
  final CameraSensorSpecs? camera;
  final TelescopeSpecs? telescope;
  final double focalReducer;
  final double rotation;
  
  const EquipmentFOVState({
    this.camera,
    this.telescope,
    this.focalReducer = 1.0,
    this.rotation = 0,
  });
  
  /// Get effective focal length
  double? get effectiveFocalLength {
    if (telescope == null) return null;
    return telescope!.focalLengthMm * focalReducer;
  }
  
  /// Get calculated FOV
  (double width, double height)? get fov {
    if (camera == null || effectiveFocalLength == null) return null;
    
    return FOVCalculator.calculateFOV(
      sensorWidthMm: camera!.widthMm,
      sensorHeightMm: camera!.heightMm,
      focalLengthMm: effectiveFocalLength!,
    );
  }
  
  /// Get image scale in arcsec/pixel
  double? get imageScale {
    if (camera == null || effectiveFocalLength == null) return null;
    
    return FOVCalculator.calculateImageScale(
      pixelSizeMicrons: camera!.pixelSizeMicrons,
      focalLengthMm: effectiveFocalLength!,
    );
  }
  
  EquipmentFOVState copyWith({
    CameraSensorSpecs? camera,
    TelescopeSpecs? telescope,
    double? focalReducer,
    double? rotation,
  }) {
    return EquipmentFOVState(
      camera: camera ?? this.camera,
      telescope: telescope ?? this.telescope,
      focalReducer: focalReducer ?? this.focalReducer,
      rotation: rotation ?? this.rotation,
    );
  }
}

class EquipmentFOVNotifier extends StateNotifier<EquipmentFOVState> {
  EquipmentFOVNotifier() : super(const EquipmentFOVState());
  
  void setCamera(CameraSensorSpecs camera) {
    state = state.copyWith(camera: camera);
  }
  
  void setTelescope(TelescopeSpecs telescope) {
    state = state.copyWith(telescope: telescope);
  }
  
  void setFocalReducer(double multiplier) {
    state = state.copyWith(focalReducer: multiplier);
  }
  
  void setRotation(double rotation) {
    state = state.copyWith(rotation: rotation % 360);
  }
}

final equipmentFOVProvider = StateNotifierProvider<EquipmentFOVNotifier, EquipmentFOVState>((ref) {
  return EquipmentFOVNotifier();
});

// ============================================================================
// Mosaic Plan Provider
// ============================================================================

/// Current mosaic plan state
class MosaicPlanState {
  final MosaicPlan? plan;
  final MosaicConfig? config;
  final bool isEditing;
  
  const MosaicPlanState({
    this.plan,
    this.config,
    this.isEditing = false,
  });
  
  MosaicPlanState copyWith({
    MosaicPlan? plan,
    MosaicConfig? config,
    bool? isEditing,
  }) {
    return MosaicPlanState(
      plan: plan ?? this.plan,
      config: config ?? this.config,
      isEditing: isEditing ?? this.isEditing,
    );
  }
}

class MosaicPlanNotifier extends StateNotifier<MosaicPlanState> {
  final Ref _ref;
  
  MosaicPlanNotifier(this._ref) : super(const MosaicPlanState());
  
  void createMosaic({
    required CelestialCoordinate center,
    required double totalWidth,
    required double totalHeight,
  }) {
    final equipment = _ref.read(equipmentFOVProvider);
    final fov = equipment.fov;
    
    if (fov == null) return;
    
    final config = MosaicConfig(
      center: center,
      totalWidth: totalWidth,
      totalHeight: totalHeight,
      panelFovWidth: fov.$1,
      panelFovHeight: fov.$2,
      rotation: equipment.rotation,
    );
    
    final plan = MosaicPlanner.generateMosaic(config);
    
    state = MosaicPlanState(
      plan: plan,
      config: config,
      isEditing: true,
    );
  }
  
  void createRectangularMosaic({
    required CelestialCoordinate center,
    required int rows,
    required int columns,
  }) {
    final equipment = _ref.read(equipmentFOVProvider);
    final fov = equipment.fov;
    
    if (fov == null) return;
    
    final plan = MosaicPlanner.generateRectangularMosaic(
      center: center,
      rows: rows,
      columns: columns,
      panelFovWidth: fov.$1,
      panelFovHeight: fov.$2,
      rotation: equipment.rotation,
    );
    
    state = MosaicPlanState(
      plan: plan,
      config: plan.config,
      isEditing: true,
    );
  }
  
  void updateOverlap(double horizontal, double vertical) {
    if (state.config == null) return;
    
    final newConfig = state.config!.copyWith(
      overlap: MosaicOverlap(horizontal: horizontal, vertical: vertical),
    );
    
    final plan = MosaicPlanner.generateMosaic(newConfig);
    
    state = state.copyWith(plan: plan, config: newConfig);
  }
  
  void updateRotation(double rotation) {
    if (state.config == null) return;
    
    final newConfig = state.config!.copyWith(rotation: rotation);
    final plan = MosaicPlanner.generateMosaic(newConfig);
    
    state = state.copyWith(plan: plan, config: newConfig);
  }
  
  void optimizeCaptureOrder({bool snakePattern = true}) {
    state.plan?.optimizeCaptureOrder(snakePattern: snakePattern);
    state = state.copyWith(plan: state.plan);
  }
  
  void clearMosaic() {
    state = const MosaicPlanState();
  }
  
  String exportToJson() {
    if (state.plan == null) return '{}';
    return MosaicExporter.toJson(state.plan!);
  }
  
  String exportToCsv() {
    if (state.plan == null) return '';
    return MosaicExporter.toCsv(state.plan!);
  }
}

final mosaicPlanProvider = StateNotifierProvider<MosaicPlanNotifier, MosaicPlanState>((ref) {
  return MosaicPlanNotifier(ref);
});

// ============================================================================
// Best Targets Provider
// ============================================================================

/// Find best imaging targets for tonight
/// Uses cached date to avoid flickering from second-by-second updates
final bestTargetsProvider = FutureProvider<List<(DeepSkyObject, ObjectVisibility)>>((ref) async {
  final dsos = await ref.watch(loadedDsosProvider.future);
  final location = ref.watch(observerLocationProvider);
  final currentDate = ref.watch(_currentDateProvider);
  
  // Calculate twilight times for the current date (not watching the time provider directly)
  final twilight = AstronomyCalculations.calculateTwilightTimes(
    date: currentDate,
    latitudeDeg: location.latitude,
    longitudeDeg: location.longitude,
  );
  
  // Use astronomical twilight as imaging time, or 9 PM if not available
  final imagingTime = twilight.astronomicalDusk ?? 
      DateTime(currentDate.year, currentDate.month, currentDate.day, 21, 0);
  
  final targetsWithVisibility = <(DeepSkyObject, ObjectVisibility)>[];
  
  for (final dso in dsos) {
    final visibility = AstronomyCalculations.calculateObjectVisibility(
      raDeg: dso.coordinates.raDegrees,
      decDeg: dso.coordinates.dec,
      date: imagingTime,
      latitudeDeg: location.latitude,
      longitudeDeg: location.longitude,
      minAltitude: 30, // Only consider objects above 30°
    );
    
    if (!visibility.neverRises && (visibility.transitAltitude ?? 0) > 30) {
      targetsWithVisibility.add((dso, visibility));
    }
  }
  
  // Sort by transit altitude (highest first)
  targetsWithVisibility.sort((a, b) => 
    (b.$2.transitAltitude ?? 0).compareTo(a.$2.transitAltitude ?? 0)
  );
  
  return targetsWithVisibility.take(20).toList();
});

// ============================================================================
// Search Provider
// ============================================================================

/// Object search state
class ObjectSearchState {
  final String query;
  final List<CelestialObject> results;
  final bool isSearching;
  
  const ObjectSearchState({
    this.query = '',
    this.results = const [],
    this.isSearching = false,
  });
  
  ObjectSearchState copyWith({
    String? query,
    List<CelestialObject>? results,
    bool? isSearching,
  }) {
    return ObjectSearchState(
      query: query ?? this.query,
      results: results ?? this.results,
      isSearching: isSearching ?? this.isSearching,
    );
  }
}

class ObjectSearchNotifier extends StateNotifier<ObjectSearchState> {
  final Ref _ref;
  
  ObjectSearchNotifier(this._ref) : super(const ObjectSearchState());
  
  Future<void> search(String query) async {
    if (query.isEmpty) {
      state = const ObjectSearchState();
      return;
    }
    
    state = state.copyWith(query: query, isSearching: true);
    
    // Normalize query for better matching (e.g., "IC 410" -> "ic410")
    // Define it here so it's accessible in the sort callback
    final qLower = query.toLowerCase().trim();
    final normalizedQuery = qLower.replaceAll(RegExp(r'\s+'), '');

    try {
      final results = <CelestialObject>[];
      
      // Search stars - use cached provider if available
      try {
        final loadedStars = await _ref.read(loadedStarsProvider.future);
        final matchingStars = loadedStars.where((s) {
          final nameLower = s.name.toLowerCase();
          final idLower = s.id.toLowerCase();
          return nameLower.contains(qLower) || idLower.contains(qLower);
        }).take(20).toList(); // Limit stars to avoid too many results
        results.addAll(matchingStars);
      } catch (_) {
        // Star search failed, continue with DSOs
      }
      
      // Search DSOs - use cached loadedDsosProvider instead of loading from disk
      // This is much faster since the catalog is already loaded
      try {
        final loadedDsos = await _ref.read(loadedDsosProvider.future);
        
        final matchingDsos = loadedDsos.where((o) {
          // Check ID, name, and catalog IDs
          final idLower = o.id.toLowerCase();
          final nameLower = o.name.toLowerCase();
          
          // Direct matches
          final idMatch = idLower.contains(qLower);
          final nameMatch = nameLower.contains(qLower);
          final catalogMatch = o.catalogIds.any((c) => c.toLowerCase().contains(qLower));
          
          // Normalized matches (handles "IC 410" vs "IC410")
          final normalizedId = idLower.replaceAll(RegExp(r'\s+'), '');
          final normalizedName = nameLower.replaceAll(RegExp(r'\s+'), '');
          
          final normalizedIdMatch = normalizedId.contains(normalizedQuery);
          final normalizedNameMatch = normalizedName.contains(normalizedQuery);
          
          // Also check catalog IDs with normalization
          final normalizedCatalogMatch = o.catalogIds.any((c) {
            final cNormalized = c.toLowerCase().replaceAll(RegExp(r'\s+'), '');
            return cNormalized.contains(normalizedQuery);
          });
          
          return idMatch || nameMatch || catalogMatch || normalizedIdMatch || normalizedNameMatch || normalizedCatalogMatch;
        }).toList();
        
        print('Search for "$query" (norm: "$normalizedQuery") found ${matchingDsos.length} DSOs');
        if (matchingDsos.isEmpty) {
           // Debug specific missing objects
           if (normalizedQuery.contains('ic410')) {
             print('DEBUG: IC 410 not found. Checking first 5 objects in DB:');
             for (var i = 0; i < 5 && i < loadedDsos.length; i++) {
               print('${loadedDsos[i].id} / ${loadedDsos[i].name} / ${loadedDsos[i].catalogIds}');
             }
             // Check if it exists at all
             final exists = loadedDsos.any((o) => o.id == 'IC410' || o.name == 'IC410');
             print('DEBUG: Does IC410 exist in DB? $exists');
           }
        }
        
        results.addAll(matchingDsos);
      } catch (e) {
        // DSO search failed, continue with what we have
        print('DSO search error: $e');
      }
      
      // Sort by relevance (exact matches first, then by brightness)
      results.sort((a, b) {
        // For DSOs, also check display name and catalog IDs
        String aDisplayName = a.name;
        String bDisplayName = b.name;
        List<String> aCatalogIds = [];
        List<String> bCatalogIds = [];
        
        if (a is DeepSkyObject) {
          final (displayName, _) = _getDsoDisplayInfoForSearch(a);
          aDisplayName = displayName;
          aCatalogIds = a.catalogIds;
        }
        if (b is DeepSkyObject) {
          final (displayName, _) = _getDsoDisplayInfoForSearch(b);
          bDisplayName = displayName;
          bCatalogIds = b.catalogIds;
        }
        
        final aNameLower = aDisplayName.toLowerCase();
        final bNameLower = bDisplayName.toLowerCase();
        final aIdLower = a.id.toLowerCase();
        final bIdLower = b.id.toLowerCase();
        
        // Check exact match (including normalized)
        bool isExact(String val) {
          final valLower = val.toLowerCase();
          if (valLower == qLower) return true;
          // Check normalized match
          final normalizedVal = valLower.replaceAll(RegExp(r'\s+'), '');
          return normalizedVal == normalizedQuery;
        }
        
        final aExact = isExact(aDisplayName) || 
                       isExact(a.id) || 
                       aCatalogIds.any((c) => isExact(c));
                       
        final bExact = isExact(bDisplayName) || 
                       isExact(b.id) || 
                       bCatalogIds.any((c) => isExact(c));
        
        if (aExact && !bExact) return -1;
        if (!aExact && bExact) return 1;
        
        return (a.magnitude ?? 99).compareTo(b.magnitude ?? 99);
      });
      
      state = ObjectSearchState(
        query: query,
        results: results.take(50).toList(),
        isSearching: false,
      );
    } catch (e) {
      // If search fails, return empty results
      state = ObjectSearchState(
        query: query,
        results: [],
        isSearching: false,
      );
    }
  }
  
  void clear() {
    state = const ObjectSearchState();
  }
}

final objectSearchProvider = StateNotifierProvider<ObjectSearchNotifier, ObjectSearchState>((ref) {
  return ObjectSearchNotifier(ref);
});

// ============================================================================
// Density Hotspots Provider
// ============================================================================

/// Calculates density hotspots for crowded regions when zoomed out.
/// Returns list of (ra, dec, visibleCount, hiddenCount) for areas with many hidden objects.
/// This helps users know when to zoom in to reveal more objects.
final densityHotspotsProvider = Provider<List<(double, double, int, int)>>((ref) {
  final viewState = ref.watch(skyViewStateProvider);
  final (starMagLimit, _) = ref.watch(dynamicMagnitudeLimitsProvider);

  // Only show density indicators when zoomed out (FOV > 30 degrees)
  if (viewState.fieldOfView < 30) return [];

  // Get all loaded stars (not the filtered ones - we need the full set to count hidden)
  final starsAsync = ref.watch(loadedStarsProvider);
  final stars = starsAsync.valueOrNull ?? [];

  if (stars.isEmpty) return [];

  // Grid the sky into cells and count objects
  const cellSize = 15.0; // degrees
  final Map<String, (int, int)> cells = {}; // visible, hidden counts

  for (final star in stars) {
    // Calculate cell key from RA (hours to degrees) and Dec
    final raDegs = star.coordinates.ra * 15; // Convert hours to degrees
    final decDegs = star.coordinates.dec;

    // Normalize RA to 0-360 range before gridding
    final normalizedRA = raDegs < 0 ? raDegs + 360 : (raDegs >= 360 ? raDegs - 360 : raDegs);
    final cellKey = '${(normalizedRA ~/ cellSize)}_${((decDegs + 90) ~/ cellSize)}';

    final current = cells[cellKey] ?? (0, 0);
    final starMag = star.magnitude ?? 99;

    if (starMag <= starMagLimit) {
      cells[cellKey] = (current.$1 + 1, current.$2);
    } else {
      cells[cellKey] = (current.$1, current.$2 + 1);
    }
  }

  // Return cells with significant hidden objects (> 30)
  return cells.entries
      .where((e) => e.value.$2 > 30)
      .map((e) {
        final parts = e.key.split('_');
        final raCellIndex = double.parse(parts[0]);
        final decCellIndex = double.parse(parts[1]);
        // Convert back to center of cell in RA (hours) and Dec (degrees)
        final ra = (raCellIndex * cellSize + cellSize / 2) / 15; // Convert to hours
        final dec = decCellIndex * cellSize - 90 + cellSize / 2; // Convert from shifted index
        return (ra, dec, e.value.$1, e.value.$2);
      })
      .toList();
});

