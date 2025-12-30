/// Render quality configuration for the planetarium
///
/// Provides three quality tiers to support devices from Raspberry Pi
/// to high-end desktops.

/// Quality tier for planetarium rendering
enum RenderQuality {
  /// Maximum performance, minimal effects - for Raspberry Pi / low-end devices
  performance,

  /// Balanced - gradient glows, moderate object counts
  balanced,

  /// Full quality - all effects enabled for capable devices
  quality,
}

/// Configuration for planetarium rendering based on quality tier
class RenderQualityConfig {
  /// The selected quality tier
  final RenderQuality quality;

  /// Whether to use MaskFilter.blur effects (expensive on CPU)
  final bool useBlurEffects;

  /// Whether to use glow effects (gradients in balanced mode, blur in quality)
  final bool useGlowEffects;

  /// Maximum number of stars to render per frame
  final int maxStarsToRender;

  /// Maximum number of DSOs to render per frame
  final int maxDsosToRender;

  /// Detail level for Milky Way rendering (0.0 = off, 1.0 = full)
  final double milkyWayDetail;

  /// Whether to show constellation art overlay
  final bool showConstellationArt;

  /// Whether to animate star twinkle
  final bool animateStarTwinkle;

  /// Whether to use smooth zoom animations
  final bool smoothZoomAnimation;

  /// Minimum star magnitude to display (higher = fainter stars shown)
  final double starMagnitudeLimit;

  /// Minimum DSO magnitude to display
  final double dsoMagnitudeLimit;

  // === Visual Polish v2 Features ===

  /// Whether to show twilight gradient sky (sun-altitude aware)
  final bool enableTwilightGradient;

  /// Whether to show horizon glow effect
  final bool enableHorizonGlow;

  /// Whether to show light pollution dome effect
  final bool enableLightPollution;

  /// Whether to apply atmospheric extinction (dimming near horizon)
  final bool enableAtmosphericExtinction;

  /// Whether to use enhanced DSO symbols (spiral arms, wispy nebulae)
  final bool enableEnhancedDsoSymbols;

  /// Whether to show planet details (Saturn rings, Jupiter bands)
  final bool enablePlanetDetails;

  /// Whether to animate object selection (pulse, fade-in)
  final bool enableSelectionAnimation;

  /// Whether to animate star pop-in when zooming
  final bool enableStarPopin;

  /// Whether to apply parallax effect to dim stars during pan
  final bool enableParallax;

  /// Star point-spread function quality (0.0 = circle, 0.5 = gradient, 1.0 = full PSF)
  final double starPsfQuality;

  /// Ground plane detail: 0 = solid color, 0.5 = gradient, 1.0 = gradient + silhouette
  final double groundPlaneDetail;

  const RenderQualityConfig._({
    required this.quality,
    required this.useBlurEffects,
    required this.useGlowEffects,
    required this.maxStarsToRender,
    required this.maxDsosToRender,
    required this.milkyWayDetail,
    required this.showConstellationArt,
    required this.animateStarTwinkle,
    required this.smoothZoomAnimation,
    required this.starMagnitudeLimit,
    required this.dsoMagnitudeLimit,
    required this.enableTwilightGradient,
    required this.enableHorizonGlow,
    required this.enableLightPollution,
    required this.enableAtmosphericExtinction,
    required this.enableEnhancedDsoSymbols,
    required this.enablePlanetDetails,
    required this.enableSelectionAnimation,
    required this.enableStarPopin,
    required this.enableParallax,
    required this.starPsfQuality,
    required this.groundPlaneDetail,
  });

  /// Performance mode: Minimal effects for low-powered devices like Raspberry Pi
  ///
  /// - No blur effects (CPU-expensive)
  /// - No glow effects
  /// - Limited star/DSO counts
  /// - No Milky Way
  /// - Basic rendering only
  const RenderQualityConfig.performance()
      : quality = RenderQuality.performance,
        useBlurEffects = false,
        useGlowEffects = false,
        maxStarsToRender = 1000,
        maxDsosToRender = 500,
        milkyWayDetail = 0.0,
        showConstellationArt = false,
        animateStarTwinkle = false,
        smoothZoomAnimation = false,
        starMagnitudeLimit = 6.0,
        dsoMagnitudeLimit = 10.0,
        // Visual Polish v2 - all disabled for performance
        enableTwilightGradient = false,
        enableHorizonGlow = false,
        enableLightPollution = false,
        enableAtmosphericExtinction = false,
        enableEnhancedDsoSymbols = false,
        enablePlanetDetails = false,
        enableSelectionAnimation = false,
        enableStarPopin = false,
        enableParallax = false,
        starPsfQuality = 0.0,
        groundPlaneDetail = 0.0;

  /// Balanced mode: Gradient-based effects for mid-range devices
  ///
  /// - No blur effects (uses radial gradients instead)
  /// - Glow effects via gradients
  /// - Moderate star/DSO counts
  /// - Partial Milky Way
  /// - Smooth animations
  const RenderQualityConfig.balanced()
      : quality = RenderQuality.balanced,
        useBlurEffects = false,
        useGlowEffects = true,
        maxStarsToRender = 5000,
        maxDsosToRender = 2000,
        milkyWayDetail = 0.5,
        showConstellationArt = false,
        animateStarTwinkle = false,
        smoothZoomAnimation = true,
        starMagnitudeLimit = 8.0,
        dsoMagnitudeLimit = 12.0,
        // Visual Polish v2 - gradient-based effects enabled
        enableTwilightGradient = true,
        enableHorizonGlow = true,
        enableLightPollution = false, // quality only
        enableAtmosphericExtinction = true,
        enableEnhancedDsoSymbols = true,
        enablePlanetDetails = false, // quality only
        enableSelectionAnimation = true,
        enableStarPopin = true,
        enableParallax = false, // quality only
        starPsfQuality = 0.5,
        groundPlaneDetail = 0.5;

  /// Quality mode: Full effects for desktops with dedicated GPU
  ///
  /// - Blur effects enabled for best visuals
  /// - Full glow effects
  /// - High star/DSO counts
  /// - Full Milky Way
  /// - All animations enabled
  const RenderQualityConfig.quality()
      : quality = RenderQuality.quality,
        useBlurEffects = true,
        useGlowEffects = true,
        maxStarsToRender = 15000,
        maxDsosToRender = 5000,
        milkyWayDetail = 1.0,
        showConstellationArt = true,
        animateStarTwinkle = true,
        smoothZoomAnimation = true,
        starMagnitudeLimit = 10.0,
        dsoMagnitudeLimit = 14.0,
        // Visual Polish v2 - all features enabled
        enableTwilightGradient = true,
        enableHorizonGlow = true,
        enableLightPollution = true,
        enableAtmosphericExtinction = true,
        enableEnhancedDsoSymbols = true,
        enablePlanetDetails = true,
        enableSelectionAnimation = true,
        enableStarPopin = true,
        enableParallax = true,
        starPsfQuality = 1.0,
        groundPlaneDetail = 1.0;

  /// Create a custom configuration
  const RenderQualityConfig.custom({
    required this.quality,
    this.useBlurEffects = false,
    this.useGlowEffects = true,
    this.maxStarsToRender = 5000,
    this.maxDsosToRender = 2000,
    this.milkyWayDetail = 0.5,
    this.showConstellationArt = false,
    this.animateStarTwinkle = false,
    this.smoothZoomAnimation = true,
    this.starMagnitudeLimit = 8.0,
    this.dsoMagnitudeLimit = 12.0,
    this.enableTwilightGradient = true,
    this.enableHorizonGlow = true,
    this.enableLightPollution = false,
    this.enableAtmosphericExtinction = true,
    this.enableEnhancedDsoSymbols = true,
    this.enablePlanetDetails = false,
    this.enableSelectionAnimation = true,
    this.enableStarPopin = true,
    this.enableParallax = false,
    this.starPsfQuality = 0.5,
    this.groundPlaneDetail = 0.5,
  });

  /// Get configuration for a specific quality tier
  factory RenderQualityConfig.fromQuality(RenderQuality quality) {
    switch (quality) {
      case RenderQuality.performance:
        return const RenderQualityConfig.performance();
      case RenderQuality.balanced:
        return const RenderQualityConfig.balanced();
      case RenderQuality.quality:
        return const RenderQualityConfig.quality();
    }
  }

  /// Create a copy with modified parameters
  RenderQualityConfig copyWith({
    RenderQuality? quality,
    bool? useBlurEffects,
    bool? useGlowEffects,
    int? maxStarsToRender,
    int? maxDsosToRender,
    double? milkyWayDetail,
    bool? showConstellationArt,
    bool? animateStarTwinkle,
    bool? smoothZoomAnimation,
    double? starMagnitudeLimit,
    double? dsoMagnitudeLimit,
    bool? enableTwilightGradient,
    bool? enableHorizonGlow,
    bool? enableLightPollution,
    bool? enableAtmosphericExtinction,
    bool? enableEnhancedDsoSymbols,
    bool? enablePlanetDetails,
    bool? enableSelectionAnimation,
    bool? enableStarPopin,
    bool? enableParallax,
    double? starPsfQuality,
    double? groundPlaneDetail,
  }) {
    return RenderQualityConfig._(
      quality: quality ?? this.quality,
      useBlurEffects: useBlurEffects ?? this.useBlurEffects,
      useGlowEffects: useGlowEffects ?? this.useGlowEffects,
      maxStarsToRender: maxStarsToRender ?? this.maxStarsToRender,
      maxDsosToRender: maxDsosToRender ?? this.maxDsosToRender,
      milkyWayDetail: milkyWayDetail ?? this.milkyWayDetail,
      showConstellationArt: showConstellationArt ?? this.showConstellationArt,
      animateStarTwinkle: animateStarTwinkle ?? this.animateStarTwinkle,
      smoothZoomAnimation: smoothZoomAnimation ?? this.smoothZoomAnimation,
      starMagnitudeLimit: starMagnitudeLimit ?? this.starMagnitudeLimit,
      dsoMagnitudeLimit: dsoMagnitudeLimit ?? this.dsoMagnitudeLimit,
      enableTwilightGradient: enableTwilightGradient ?? this.enableTwilightGradient,
      enableHorizonGlow: enableHorizonGlow ?? this.enableHorizonGlow,
      enableLightPollution: enableLightPollution ?? this.enableLightPollution,
      enableAtmosphericExtinction: enableAtmosphericExtinction ?? this.enableAtmosphericExtinction,
      enableEnhancedDsoSymbols: enableEnhancedDsoSymbols ?? this.enableEnhancedDsoSymbols,
      enablePlanetDetails: enablePlanetDetails ?? this.enablePlanetDetails,
      enableSelectionAnimation: enableSelectionAnimation ?? this.enableSelectionAnimation,
      enableStarPopin: enableStarPopin ?? this.enableStarPopin,
      enableParallax: enableParallax ?? this.enableParallax,
      starPsfQuality: starPsfQuality ?? this.starPsfQuality,
      groundPlaneDetail: groundPlaneDetail ?? this.groundPlaneDetail,
    );
  }

  @override
  bool operator ==(Object other) {
    if (identical(this, other)) return true;
    return other is RenderQualityConfig &&
        other.quality == quality &&
        other.useBlurEffects == useBlurEffects &&
        other.useGlowEffects == useGlowEffects &&
        other.maxStarsToRender == maxStarsToRender &&
        other.maxDsosToRender == maxDsosToRender &&
        other.milkyWayDetail == milkyWayDetail &&
        other.showConstellationArt == showConstellationArt &&
        other.animateStarTwinkle == animateStarTwinkle &&
        other.smoothZoomAnimation == smoothZoomAnimation &&
        other.starMagnitudeLimit == starMagnitudeLimit &&
        other.dsoMagnitudeLimit == dsoMagnitudeLimit &&
        other.enableTwilightGradient == enableTwilightGradient &&
        other.enableHorizonGlow == enableHorizonGlow &&
        other.enableLightPollution == enableLightPollution &&
        other.enableAtmosphericExtinction == enableAtmosphericExtinction &&
        other.enableEnhancedDsoSymbols == enableEnhancedDsoSymbols &&
        other.enablePlanetDetails == enablePlanetDetails &&
        other.enableSelectionAnimation == enableSelectionAnimation &&
        other.enableStarPopin == enableStarPopin &&
        other.enableParallax == enableParallax &&
        other.starPsfQuality == starPsfQuality &&
        other.groundPlaneDetail == groundPlaneDetail;
  }

  @override
  int get hashCode => Object.hashAll([
        quality,
        useBlurEffects,
        useGlowEffects,
        maxStarsToRender,
        maxDsosToRender,
        milkyWayDetail,
        showConstellationArt,
        animateStarTwinkle,
        smoothZoomAnimation,
        starMagnitudeLimit,
        dsoMagnitudeLimit,
        enableTwilightGradient,
        enableHorizonGlow,
        enableLightPollution,
        enableAtmosphericExtinction,
        enableEnhancedDsoSymbols,
        enablePlanetDetails,
        enableSelectionAnimation,
        enableStarPopin,
        enableParallax,
        starPsfQuality,
        groundPlaneDetail,
      ]);

  @override
  String toString() => 'RenderQualityConfig($quality)';
}
