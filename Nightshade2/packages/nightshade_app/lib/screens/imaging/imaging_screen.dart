import 'dart:async';
import 'dart:math' as math;
import 'dart:typed_data';
import 'dart:ui' as ui;
import 'package:file_selector/file_selector.dart';
import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:lucide_icons/lucide_icons.dart';
import 'package:nightshade_ui/nightshade_ui.dart';
import 'package:nightshade_core/nightshade_core.dart';
import 'package:nightshade_planetarium/nightshade_planetarium.dart';
import '../../widgets/annotation_overlay.dart';
import '../settings/catalog_settings_screen.dart';
import 'tabs/mount_tab.dart';

/// Provider to check if annotation catalog is installed
final annotationCatalogInstalledProvider = FutureProvider<bool>((ref) async {
  final status = await CatalogManager.instance.getAnnotationCatalogStatus();
  return status.isInstalled;
});

/// Provider to track if the annotation catalog banner has been dismissed
final annotationBannerDismissedProvider = StateProvider<bool>((ref) => false);

class ImagingScreen extends ConsumerStatefulWidget {
  const ImagingScreen({super.key});

  @override
  ConsumerState<ImagingScreen> createState() => _ImagingScreenState();
}

class _ImagingScreenState extends ConsumerState<ImagingScreen>
    with SingleTickerProviderStateMixin {
  // Panel selection is now stored in provider for persistence across navigation
  late AnimationController _fadeController;

  // Local capture state
  bool _isLooping = false;
  bool _isSingleCapture = false;

  // Image view state
  double _zoomLevel = 1.0;
  Offset _panOffset = Offset.zero;
  bool _showCrosshair = true;
  bool _showGrid = false;
  bool _showStarOverlay = false;

  @override
  void initState() {
    super.initState();
    _fadeController = AnimationController(
      vsync: this,
      duration: const Duration(milliseconds: 200),
    )..forward();

    // Initialize the annotation service to set up the image listener
    // This must happen on first frame to have access to ref
    WidgetsBinding.instance.addPostFrameCallback((_) {
      _initializeAnnotationService();
    });
  }

  /// Initialize the annotation service so it starts listening for new images
  void _initializeAnnotationService() {
    // Reading the provider creates the AnnotationService instance
    // which sets up the listener for currentImageProvider
    ref.read(annotationServiceProvider);
    print('[IMAGING] AnnotationService initialized');
  }

  @override
  void dispose() {
    _fadeController.dispose();
    super.dispose();
  }

  void _selectPanel(int index) {
    final currentPanel = ref.read(selectedImagingPanelProvider);
    if (index != currentPanel) {
      _fadeController.reset();
      ref.read(selectedImagingPanelProvider.notifier).state = index;
      _fadeController.forward();
    }
  }

  // =========================================================================
  // CAPTURE ACTIONS
  // =========================================================================

  Future<void> _takeSnapshot() async {
    if (_isSingleCapture || _isLooping) return;

    setState(() => _isSingleCapture = true);

    try {
      final settings = ref.read(exposureSettingsProvider);
      final imagingService = ref.read(imagingServiceProvider);
      final sessionNotifier = ref.read(sessionStateProvider.notifier);

      sessionNotifier.setCapturing(true);

      final result = await imagingService.captureImage(
        settings: settings,
        targetName: ref.read(sessionStateProvider).targetName,
      );

      if (result != null) {
        ref.read(currentImageProvider.notifier).state = result;
        ref.read(lastImageStatsProvider.notifier).state = result.stats;
        sessionNotifier.recordExposureComplete(
          exposureTime: settings.exposureTime,
          hfr: result.stats.hfr,
        );
      }
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Capture failed: $e')),
        );
      }
    } finally {
      if (mounted) {
        setState(() => _isSingleCapture = false);
        ref.read(sessionStateProvider.notifier).setCapturing(false);
      }
    }
  }

  Future<void> _toggleLoop() async {
    if (_isSingleCapture) return;

    if (_isLooping) {
      // Stop looping
      setState(() => _isLooping = false);
      ref.read(imagingServiceProvider).cancelExposure();
      return;
    }

    setState(() => _isLooping = true);
    ref.read(sessionStateProvider.notifier).setCapturing(true);

    final settings = ref.read(exposureSettingsProvider);
    final imagingService = ref.read(imagingServiceProvider);

    try {
      await imagingService.startLoopCapture(
          settings: settings,
          targetName: ref.read(sessionStateProvider).targetName,
          onImageCaptured: (image) {
            if (mounted) {
              ref.read(currentImageProvider.notifier).state = image;
              ref.read(lastImageStatsProvider.notifier).state = image.stats;
              ref.read(sessionStateProvider.notifier).recordExposureComplete(
                    exposureTime: settings.exposureTime,
                    hfr: image.stats.hfr,
                  );
            }
          },
          onError: (error) {
            if (mounted) {
              ScaffoldMessenger.of(context).showSnackBar(
                SnackBar(content: Text('Capture error: $error')),
              );
            }
          });
    } finally {
      if (mounted) {
        setState(() => _isLooping = false);
        ref.read(sessionStateProvider.notifier).setCapturing(false);
      }
    }
  }

  void _abortCapture() {
    ref.read(imagingServiceProvider).cancelExposure();
    setState(() {
      _isLooping = false;
      _isSingleCapture = false;
    });
    ref.read(sessionStateProvider.notifier).setCapturing(false);
  }

  // =========================================================================
  // ZOOM/PAN CONTROLS
  // =========================================================================

  void _zoomIn() {
    setState(() {
      _zoomLevel = (_zoomLevel * 1.25).clamp(0.25, 8.0);
    });
  }

  void _zoomOut() {
    setState(() {
      _zoomLevel = (_zoomLevel / 1.25).clamp(0.25, 8.0);
    });
  }

  void _fitToWindow() {
    setState(() {
      _zoomLevel = 1.0;
      _panOffset = Offset.zero;
    });
  }

  void _zoom1to1() {
    setState(() {
      _zoomLevel = 1.0;
      _panOffset = Offset.zero;
    });
  }

  @override
  Widget build(BuildContext context) {
    final colors = Theme.of(context).extension<NightshadeColors>()!;
    final selectedPanel = ref.watch(selectedImagingPanelProvider);
    final annotationSettings = ref.watch(annotationSettingsProvider);
    final catalogInstalled = ref.watch(annotationCatalogInstalledProvider);
    final bannerDismissed = ref.watch(annotationBannerDismissedProvider);

    // Show banner if annotations are enabled but catalog is not installed
    final showBanner = (annotationSettings.valueOrNull?.enabled ?? false) &&
        catalogInstalled.valueOrNull == false &&
        !bannerDismissed;

    return Column(
      children: [
        // Annotation catalog banner
        if (showBanner)
          _AnnotationCatalogBanner(
            colors: colors,
            onDismiss: () => ref
                .read(annotationBannerDismissedProvider.notifier)
                .state = true,
            onSetup: () {
              // Show catalog settings dialog
              showDialog(
                context: context,
                builder: (context) => Dialog(
                  child: ConstrainedBox(
                    constraints:
                        const BoxConstraints(maxWidth: 800, maxHeight: 700),
                    child: const CatalogSettingsScreen(),
                  ),
                ),
              ).then((_) {
                // Refresh catalog status after dialog closes
                ref.invalidate(annotationCatalogInstalledProvider);
              });
            },
          ),

        // Main content
        Expanded(
          child: Row(
            children: [
              // Main content area (image + controls)
              Expanded(
                flex: 7,
                child: Column(
                  children: [
                    // Live preview area
                    Expanded(
                      flex: 6,
                      child: _LivePreviewArea(
                        colors: colors,
                        zoomLevel: _zoomLevel,
                        panOffset: _panOffset,
                        showCrosshair: _showCrosshair,
                        showGrid: _showGrid,
                        showStarOverlay: _showStarOverlay,
                        onZoomIn: _zoomIn,
                        onZoomOut: _zoomOut,
                        onFitToWindow: _fitToWindow,
                        onZoom1to1: _zoom1to1,
                        onAbortCapture: _abortCapture,
                        onToggleCrosshair: () =>
                            setState(() => _showCrosshair = !_showCrosshair),
                        onToggleGrid: () =>
                            setState(() => _showGrid = !_showGrid),
                        onToggleStarOverlay: () =>
                            setState(() => _showStarOverlay = !_showStarOverlay),
                      ),
                    ),

                    // Bottom control panel
                    Container(
                      constraints: const BoxConstraints(minHeight: 120),
                      decoration: BoxDecoration(
                        color: colors.surface,
                        border: Border(
                          top: BorderSide(color: colors.border),
                        ),
                      ),
                      child: FadeTransition(
                        opacity: _fadeController,
                        child: _buildControlPanel(colors),
                      ),
                    ),
                  ],
                ),
              ),

              // Right panel with tabs
              ResizablePanel(
                initialWidth: 320,
                minWidth: 250,
                maxWidth: 500,
                side: ResizeSide.left,
                child: Container(
                  decoration: BoxDecoration(
                    color: colors.surface,
                    border: Border(
                      left: BorderSide(color: colors.border),
                    ),
                  ),
                  child: Column(
                    children: [
                      // Panel tabs
                      _PanelTabs(
                        selectedIndex: selectedPanel,
                        onSelected: _selectPanel,
                        colors: colors,
                      ),

                      // Panel content
                      Expanded(
                        child: FadeTransition(
                          opacity: _fadeController,
                          child: IndexedStack(
                            index: selectedPanel,
                            children: [
                              _CapturePanel(colors: colors),
                              _CameraPanel(colors: colors),
                              _FocusPanel(colors: colors),
                              _GuidingPanel(colors: colors),
                              const MountTab(),
                            ],
                          ),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
            ],
          ),
        ),
      ],
    );
  }

  Widget _buildControlPanel(NightshadeColors colors) {
    final exposureSettings = ref.watch(exposureSettingsProvider);
    final cameraState = ref.watch(cameraStateProvider);
    final isConnected =
        cameraState.connectionState == DeviceConnectionState.connected;
    final isCapturing = _isSingleCapture || _isLooping;

    return LayoutBuilder(
      builder: (context, constraints) {
        final isMobile = constraints.maxWidth < 600;
        final isSmallMobile = constraints.maxWidth < 400;
        final horizontalPadding = isMobile ? 12.0 : 16.0;
        final verticalPadding = isMobile ? 12.0 : 16.0;
        final sectionSpacing = isSmallMobile ? 12.0 : (isMobile ? 16.0 : 24.0);

        // On very small screens, stack vertically
        if (isSmallMobile) {
          return Padding(
            padding: EdgeInsets.symmetric(
                horizontal: horizontalPadding, vertical: verticalPadding),
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.stretch,
              children: [
                // Capture controls
                _ControlSection(
                  title: 'Capture',
                  colors: colors,
                  child: Row(
                    children: [
                      Expanded(
                        child: _BigActionButton(
                          icon: _isSingleCapture
                              ? LucideIcons.loader2
                              : LucideIcons.camera,
                          label: _isSingleCapture ? 'Taking...' : 'Snapshot',
                          color: colors.primary,
                          isLoading: _isSingleCapture,
                          isEnabled: isConnected && !isCapturing,
                          onPressed: _takeSnapshot,
                          isMobile: true,
                        ),
                      ),
                      SizedBox(width: sectionSpacing),
                      Expanded(
                        child: _BigActionButton(
                          icon: _isLooping
                              ? LucideIcons.square
                              : LucideIcons.video,
                          label: _isLooping ? 'Stop' : 'Loop',
                          color: _isLooping ? colors.error : colors.accent,
                          isEnabled: isConnected && !_isSingleCapture,
                          onPressed: _toggleLoop,
                          isMobile: true,
                        ),
                      ),
                    ],
                  ),
                ),
                SizedBox(height: sectionSpacing),
                // Exposure settings
                _ControlSection(
                  title: 'Exposure',
                  colors: colors,
                  child: Row(
                    children: [
                      Expanded(
                        child: _EditableCompactInput(
                          label: 'Duration',
                          value:
                              exposureSettings.exposureTime.toStringAsFixed(0),
                          suffix: 's',
                          colors: colors,
                          isMobile: true,
                          onChanged: (value) {
                            final parsed = double.tryParse(value);
                            if (parsed != null && parsed > 0) {
                              ref
                                      .read(exposureSettingsProvider.notifier)
                                      .state =
                                  exposureSettings.copyWith(
                                      exposureTime: parsed);
                            }
                          },
                        ),
                      ),
                      SizedBox(width: sectionSpacing),
                      Expanded(
                        child: _EditableCompactInput(
                          label: 'Gain',
                          value: exposureSettings.gain.toString(),
                          colors: colors,
                          isMobile: true,
                          onChanged: (value) {
                            final parsed = int.tryParse(value);
                            if (parsed != null && parsed >= 0) {
                              ref
                                      .read(exposureSettingsProvider.notifier)
                                      .state =
                                  exposureSettings.copyWith(gain: parsed);
                            }
                          },
                        ),
                      ),
                      SizedBox(width: sectionSpacing),
                      Expanded(
                        child: _EditableCompactInput(
                          label: 'Offset',
                          value: exposureSettings.offset.toString(),
                          colors: colors,
                          isMobile: true,
                          onChanged: (value) {
                            final parsed = int.tryParse(value);
                            if (parsed != null && parsed >= 0) {
                              ref
                                      .read(exposureSettingsProvider.notifier)
                                      .state =
                                  exposureSettings.copyWith(offset: parsed);
                            }
                          },
                        ),
                      ),
                    ],
                  ),
                ),
                SizedBox(height: sectionSpacing),
                // Filter selection
                _ControlSection(
                  title: 'Filter',
                  colors: colors,
                  child: _FilterSelector(colors: colors, isMobile: true),
                ),
              ],
            ),
          );
        }

        // On larger screens, use horizontal layout
        return Padding(
          padding: EdgeInsets.symmetric(
              horizontal: horizontalPadding, vertical: verticalPadding),
          child: SingleChildScrollView(
            scrollDirection: Axis.horizontal,
            child: Row(
              mainAxisSize: MainAxisSize.min,
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                // Capture controls
                _ControlSection(
                  title: 'Capture',
                  colors: colors,
                  child: Row(
                    mainAxisSize: MainAxisSize.min,
                    children: [
                      _BigActionButton(
                        icon: _isSingleCapture
                            ? LucideIcons.loader2
                            : LucideIcons.camera,
                        label: _isSingleCapture ? 'Taking...' : 'Snapshot',
                        color: colors.primary,
                        isLoading: _isSingleCapture,
                        isEnabled: isConnected && !isCapturing,
                        onPressed: _takeSnapshot,
                      ),
                      const SizedBox(width: 12),
                      _BigActionButton(
                        icon: _isLooping
                            ? LucideIcons.square
                            : LucideIcons.video,
                        label: _isLooping ? 'Stop' : 'Loop',
                        color: _isLooping ? colors.error : colors.accent,
                        isEnabled: isConnected && !_isSingleCapture,
                        onPressed: _toggleLoop,
                      ),
                    ],
                  ),
                ),

                SizedBox(width: sectionSpacing),

                // Exposure settings
                _ControlSection(
                  title: 'Exposure',
                  colors: colors,
                  child: Row(
                    mainAxisSize: MainAxisSize.min,
                    children: [
                      _EditableCompactInput(
                        label: 'Duration',
                        value: exposureSettings.exposureTime
                            .toStringAsFixed(0),
                        suffix: 's',
                        colors: colors,
                        isMobile: isMobile,
                        onChanged: (value) {
                          final parsed = double.tryParse(value);
                          if (parsed != null && parsed > 0) {
                            ref
                                    .read(exposureSettingsProvider.notifier)
                                    .state =
                                exposureSettings.copyWith(
                                    exposureTime: parsed);
                          }
                        },
                      ),
                      SizedBox(width: isMobile ? 8.0 : 12.0),
                      _EditableCompactInput(
                        label: 'Gain',
                        value: exposureSettings.gain.toString(),
                        colors: colors,
                        isMobile: isMobile,
                        onChanged: (value) {
                          final parsed = int.tryParse(value);
                          if (parsed != null && parsed >= 0) {
                            ref
                                    .read(exposureSettingsProvider.notifier)
                                    .state =
                                exposureSettings.copyWith(gain: parsed);
                          }
                        },
                      ),
                      SizedBox(width: isMobile ? 8.0 : 12.0),
                      _EditableCompactInput(
                        label: 'Offset',
                        value: exposureSettings.offset.toString(),
                        colors: colors,
                        isMobile: isMobile,
                        onChanged: (value) {
                          final parsed = int.tryParse(value);
                          if (parsed != null && parsed >= 0) {
                            ref
                                    .read(exposureSettingsProvider.notifier)
                                    .state =
                                exposureSettings.copyWith(offset: parsed);
                          }
                        },
                      ),
                    ],
                  ),
                ),

                SizedBox(width: sectionSpacing),

                // Filter selection
                _ControlSection(
                  title: 'Filter',
                  colors: colors,
                  child: _FilterSelector(colors: colors, isMobile: isMobile),
                ),

                if (!isMobile) ...[
                  SizedBox(width: sectionSpacing),
                  // Quick stats with live data (hide on mobile to save space)
                  _QuickStatsPanel(colors: colors),
                ],
              ],
            ),
          ),
        );
      },
    );
  }
}

class _LivePreviewArea extends ConsumerWidget {
  final NightshadeColors colors;
  final double zoomLevel;
  final Offset panOffset;
  final bool showCrosshair;
  final bool showGrid;
  final bool showStarOverlay;
  final VoidCallback onZoomIn;
  final VoidCallback onZoomOut;
  final VoidCallback onFitToWindow;
  final VoidCallback onZoom1to1;
  final VoidCallback onAbortCapture;
  final VoidCallback onToggleCrosshair;
  final VoidCallback onToggleGrid;
  final VoidCallback onToggleStarOverlay;

  const _LivePreviewArea({
    required this.colors,
    required this.zoomLevel,
    required this.panOffset,
    required this.showCrosshair,
    required this.showGrid,
    required this.showStarOverlay,
    required this.onZoomIn,
    required this.onZoomOut,
    required this.onFitToWindow,
    required this.onZoom1to1,
    required this.onAbortCapture,
    required this.onToggleCrosshair,
    required this.onToggleGrid,
    required this.onToggleStarOverlay,
  });

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final currentImage = ref.watch(currentImageProvider);
    final exposureProgress = ref.watch(exposureProgressProvider);
    final lastStats = ref.watch(lastImageStatsProvider);
    final cameraState = ref.watch(cameraStateProvider);
    final exposureSettings = ref.watch(exposureSettingsProvider);
    final starDetectionResult = ref.watch(starDetectionResultProvider);

    final isConnected =
        cameraState.connectionState == DeviceConnectionState.connected;
    final isExposing =
        exposureProgress.percent > 0 || exposureProgress.isDownloading;

    return Container(
      color: const Color(0xFF08080C),
      child: Stack(
        children: [
          // Image display or placeholder
          if (currentImage != null)
            Positioned.fill(
              child: _ImageDisplayWidget(
                imageData: currentImage,
                zoomLevel: zoomLevel,
                panOffset: panOffset,
              ),
            )
          else
            // Star field background with placeholder message
            Positioned.fill(
              child: Stack(
                children: [
                  Positioned.fill(
                    child: CustomPaint(
                      painter: _StarFieldPainter(colors: colors),
                    ),
                  ),
                  Center(
                    child: Column(
                      mainAxisSize: MainAxisSize.min,
                      children: [
                        Container(
                          padding: const EdgeInsets.all(24),
                          decoration: BoxDecoration(
                            color: colors.surface.withValues(alpha: 0.8),
                            shape: BoxShape.circle,
                            border: Border.all(color: colors.border),
                          ),
                          child: Icon(
                            LucideIcons.camera,
                            size: 48,
                            color: colors.textMuted,
                          ),
                        ),
                        const SizedBox(height: 20),
                        Text(
                          isConnected ? 'No Image' : 'No Camera Connected',
                          style: TextStyle(
                            fontSize: 18,
                            fontWeight: FontWeight.w600,
                            color: colors.textSecondary,
                          ),
                        ),
                        const SizedBox(height: 8),
                        Text(
                          isConnected
                              ? 'Take a snapshot or start a capture loop'
                              : 'Connect a camera in Equipment settings',
                          style: TextStyle(
                            fontSize: 13,
                            color: colors.textMuted,
                          ),
                        ),
                      ],
                    ),
                  ),
                ],
              ),
            ),

          // Crosshair overlay
          if (showCrosshair && currentImage != null)
            Positioned.fill(
              child: CustomPaint(
                painter: _CrosshairOverlayPainter(
                  color: colors.primary.withValues(alpha: 0.4),
                ),
              ),
            ),

          // Grid overlay
          if (showGrid && currentImage != null)
            Positioned.fill(
              child: CustomPaint(
                painter: _GridOverlayPainter(
                  color: colors.primary.withValues(alpha: 0.2),
                ),
              ),
            ),

          // Star overlay
          if (showStarOverlay && currentImage != null && starDetectionResult != null && starDetectionResult.stars.isNotEmpty)
            Positioned.fill(
              child: CustomPaint(
                painter: _StarOverlayPainter(
                  stars: starDetectionResult.stars,
                  color: colors.accent.withValues(alpha: 0.8),
                  zoomLevel: zoomLevel,
                  panOffset: panOffset,
                ),
              ),
            ),

          // Annotation overlay with fade effects
          if (currentImage != null)
            Positioned.fill(
              child: _AnnotationOverlayWrapper(
                zoomLevel: zoomLevel,
                panOffset: panOffset,
                imageSize: Size(currentImage.width.toDouble(),
                    currentImage.height.toDouble()),
                colors: colors,
              ),
            ),

          // Exposure progress overlay
          if (isExposing)
            Positioned.fill(
              child: _ExposureProgressOverlay(
                progress: exposureProgress,
                colors: colors,
              ),
            ),

          // Top overlay bar
          Positioned(
            top: 0,
            left: 0,
            right: 0,
            child: Container(
              padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 8),
              decoration: BoxDecoration(
                gradient: LinearGradient(
                  begin: Alignment.topCenter,
                  end: Alignment.bottomCenter,
                  colors: [
                    Colors.black.withValues(alpha: 0.6),
                    Colors.transparent,
                  ],
                ),
              ),
              child: Row(
                children: [
                  _OverlayChip(
                    icon: LucideIcons.maximize2,
                    label: currentImage != null
                        ? '${currentImage.width} × ${currentImage.height}'
                        : '--- × ---',
                    colors: colors,
                  ),
                  const SizedBox(width: 8),
                  _OverlayChip(
                    icon: LucideIcons.grid,
                    label: 'Binning ${exposureSettings.binning}',
                    colors: colors,
                  ),
                  const SizedBox(width: 8),
                  _OverlayChip(
                    icon: LucideIcons.search,
                    label: '${(zoomLevel * 100).round()}%',
                    colors: colors,
                  ),
                  const Spacer(),
                  _OverlayIconButton(
                    icon: LucideIcons.crosshair,
                    tooltip: 'Toggle crosshair',
                    colors: colors,
                    isActive: showCrosshair,
                    onTap: onToggleCrosshair,
                  ),
                  _OverlayIconButton(
                    icon: LucideIcons.grid,
                    tooltip: 'Toggle grid',
                    colors: colors,
                    isActive: showGrid,
                    onTap: onToggleGrid,
                  ),
                  _OverlayIconButton(
                    icon: LucideIcons.sparkles,
                    tooltip: 'Toggle star overlay',
                    colors: colors,
                    isActive: showStarOverlay,
                    onTap: onToggleStarOverlay,
                  ),
                  _OverlayIconButton(
                    icon: LucideIcons.zoomIn,
                    tooltip: 'Zoom in',
                    colors: colors,
                    onTap: onZoomIn,
                  ),
                  _OverlayIconButton(
                    icon: LucideIcons.zoomOut,
                    tooltip: 'Zoom out',
                    colors: colors,
                    onTap: onZoomOut,
                  ),
                  _OverlayIconButton(
                    icon: LucideIcons.minimize2,
                    tooltip: '1:1 zoom',
                    colors: colors,
                    onTap: onZoom1to1,
                  ),
                  _OverlayIconButton(
                    icon: LucideIcons.maximize,
                    tooltip: 'Fit to window',
                    colors: colors,
                    onTap: onFitToWindow,
                  ),
                  if (exposureProgress.percent > 0 && exposureProgress.percent < 1.0)
                    _OverlayIconButton(
                      icon: LucideIcons.x,
                      tooltip: 'Abort capture',
                      colors: colors,
                      onTap: onAbortCapture,
                    ),
                ],
              ),
            ),
          ),

          // Bottom histogram overlay
          Positioned(
            bottom: 16,
            left: 16,
            child: _HistogramWidget(
              colors: colors,
              histogram: currentImage?.histogram,
            ),
          ),

          // Right side stats
          Positioned(
            bottom: 16,
            right: 16,
            child: _ImageStatsOverlay(
              colors: colors,
              stats: lastStats,
            ),
          ),

          // Annotation status indicator (top left, below the overlay bar)
          if (currentImage != null)
            Positioned(
              top: 48,
              left: 16,
              child: _AnnotationStatusIndicator(colors: colors),
            ),
        ],
      ),
    );
  }
}

class _StarFieldPainter extends CustomPainter {
  final NightshadeColors colors;

  _StarFieldPainter({required this.colors});

  @override
  void paint(Canvas canvas, Size size) {
    final random = math.Random(42);
    final paint = Paint();

    for (var i = 0; i < 80; i++) {
      final x = random.nextDouble() * size.width;
      final y = random.nextDouble() * size.height;
      final brightness = random.nextDouble() * 0.25 + 0.05;
      final radius = random.nextDouble() * 1.2 + 0.3;

      paint.color = Colors.white.withValues(alpha: brightness);
      canvas.drawCircle(Offset(x, y), radius, paint);
    }
  }

  @override
  bool shouldRepaint(covariant CustomPainter oldDelegate) => false;
}

class _OverlayChip extends StatelessWidget {
  final IconData icon;
  final String label;
  final NightshadeColors colors;

  const _OverlayChip({
    required this.icon,
    required this.label,
    required this.colors,
  });

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 6),
      decoration: BoxDecoration(
        color: Colors.black.withValues(alpha: 0.5),
        borderRadius: BorderRadius.circular(6),
        border: Border.all(color: Colors.white.withValues(alpha: 0.1)),
      ),
      child: Row(
        mainAxisSize: MainAxisSize.min,
        children: [
          Icon(icon, size: 12, color: Colors.white70),
          const SizedBox(width: 6),
          Text(
            label,
            style: const TextStyle(
              fontSize: 11,
              color: Colors.white70,
            ),
          ),
        ],
      ),
    );
  }
}

class _OverlayIconButton extends StatefulWidget {
  final IconData icon;
  final String tooltip;
  final NightshadeColors colors;
  final bool isActive;
  final VoidCallback? onTap;

  const _OverlayIconButton({
    required this.icon,
    required this.tooltip,
    required this.colors,
    this.isActive = false,
    this.onTap,
  });

  @override
  State<_OverlayIconButton> createState() => _OverlayIconButtonState();
}

class _OverlayIconButtonState extends State<_OverlayIconButton> {
  bool _isHovered = false;

  @override
  Widget build(BuildContext context) {
    return Tooltip(
      message: widget.tooltip,
      child: MouseRegion(
        onEnter: (_) => setState(() => _isHovered = true),
        onExit: (_) => setState(() => _isHovered = false),
        cursor: SystemMouseCursors.click,
        child: GestureDetector(
          onTap: widget.onTap,
          child: AnimatedContainer(
            duration: const Duration(milliseconds: 150),
            padding: const EdgeInsets.all(8),
            decoration: BoxDecoration(
              color: widget.isActive
                  ? widget.colors.primary.withValues(alpha: 0.3)
                  : _isHovered
                      ? Colors.white.withValues(alpha: 0.15)
                      : Colors.transparent,
              borderRadius: BorderRadius.circular(6),
              border: widget.isActive
                  ? Border.all(
                      color: widget.colors.primary.withValues(alpha: 0.5))
                  : null,
            ),
            child: Icon(
              widget.icon,
              size: 16,
              color: widget.isActive
                  ? widget.colors.primary
                  : _isHovered
                      ? Colors.white
                      : Colors.white70,
            ),
          ),
        ),
      ),
    );
  }
}

class _HistogramWidget extends StatelessWidget {
  final NightshadeColors colors;
  final List<int>? histogram;

  const _HistogramWidget({
    required this.colors,
    this.histogram,
  });

  @override
  Widget build(BuildContext context) {
    return Container(
      width: 200,
      height: 80,
      padding: const EdgeInsets.all(12),
      decoration: BoxDecoration(
        color: Colors.black.withValues(alpha: 0.7),
        borderRadius: BorderRadius.circular(10),
        border: Border.all(color: Colors.white.withValues(alpha: 0.1)),
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Text(
            'Histogram',
            style: TextStyle(
              fontSize: 10,
              fontWeight: FontWeight.w500,
              color: colors.textMuted,
            ),
          ),
          const SizedBox(height: 8),
          Expanded(
            child: histogram != null && histogram!.isNotEmpty
                ? CustomPaint(
                    painter: _HistogramPainter(
                      histogram: histogram!,
                      color: colors.primary,
                    ),
                    size: Size.infinite,
                  )
                : Container(
                    decoration: BoxDecoration(
                      color: Colors.white.withValues(alpha: 0.05),
                      borderRadius: BorderRadius.circular(4),
                    ),
                    child: Center(
                      child: Text(
                        'No data',
                        style: TextStyle(
                          fontSize: 9,
                          color: colors.textMuted,
                        ),
                      ),
                    ),
                  ),
          ),
        ],
      ),
    );
  }
}

class _HistogramPainter extends CustomPainter {
  final List<int> histogram;
  final Color color;

  _HistogramPainter({required this.histogram, required this.color});

  @override
  void paint(Canvas canvas, Size size) {
    if (histogram.isEmpty) return;

    final maxVal = histogram.reduce((a, b) => a > b ? a : b);
    if (maxVal == 0) return;

    final paint = Paint()
      ..color = color.withValues(alpha: 0.7)
      ..style = PaintingStyle.fill;

    final barWidth = size.width / histogram.length;

    for (int i = 0; i < histogram.length; i++) {
      final barHeight = (histogram[i] / maxVal) * size.height;
      canvas.drawRect(
        Rect.fromLTWH(
          i * barWidth,
          size.height - barHeight,
          barWidth,
          barHeight,
        ),
        paint,
      );
    }
  }

  @override
  bool shouldRepaint(covariant _HistogramPainter oldDelegate) {
    return histogram != oldDelegate.histogram;
  }
}

class _ImageStatsOverlay extends StatelessWidget {
  final NightshadeColors colors;
  final ImageStats? stats;

  const _ImageStatsOverlay({
    required this.colors,
    this.stats,
  });

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.all(12),
      decoration: BoxDecoration(
        color: Colors.black.withValues(alpha: 0.7),
        borderRadius: BorderRadius.circular(10),
        border: Border.all(color: Colors.white.withValues(alpha: 0.1)),
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.end,
        children: [
          _StatLine(
            label: 'HFR',
            value: stats?.hfr?.toStringAsFixed(2) ?? '---',
            colors: colors,
          ),
          _StatLine(
            label: 'Stars',
            value: stats?.starCount?.toString() ?? '---',
            colors: colors,
          ),
          _StatLine(
            label: 'Median',
            value: stats?.median?.toStringAsFixed(0) ?? '---',
            colors: colors,
          ),
          _StatLine(
            label: 'Mean',
            value: stats?.mean?.toStringAsFixed(0) ?? '---',
            colors: colors,
          ),
        ],
      ),
    );
  }
}

class _StatLine extends StatelessWidget {
  final String label;
  final String value;
  final NightshadeColors colors;

  const _StatLine({
    required this.label,
    required this.value,
    required this.colors,
  });

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.symmetric(vertical: 2),
      child: Row(
        mainAxisSize: MainAxisSize.min,
        children: [
          Text(
            '$label:',
            style: TextStyle(
              fontSize: 10,
              color: colors.textMuted,
            ),
          ),
          const SizedBox(width: 8),
          Text(
            value,
            style: const TextStyle(
              fontSize: 11,
              fontWeight: FontWeight.w500,
              color: Colors.white70,
              fontFeatures: [FontFeature.tabularFigures()],
            ),
          ),
        ],
      ),
    );
  }
}

class _PanelTabs extends StatelessWidget {
  final int selectedIndex;
  final ValueChanged<int> onSelected;
  final NightshadeColors colors;

  const _PanelTabs({
    required this.selectedIndex,
    required this.onSelected,
    required this.colors,
  });

  static const _tabs = [
    (LucideIcons.camera, 'Capture'),
    (LucideIcons.aperture, 'Camera'),
    (LucideIcons.focus, 'Focus'),
    (LucideIcons.crosshair, 'Guiding'),
    (LucideIcons.compass, 'Mount'),
  ];

  @override
  Widget build(BuildContext context) {
    return Container(
      height: 44,
      decoration: BoxDecoration(
        color: colors.surfaceAlt,
        border: Border(
          bottom: BorderSide(color: colors.border),
        ),
      ),
      child: Row(
        children: _tabs.asMap().entries.map((entry) {
          final index = entry.key;
          final (icon, label) = entry.value;
          final isSelected = index == selectedIndex;

          return Expanded(
            child: _PanelTab(
              icon: icon,
              label: label,
              isSelected: isSelected,
              onTap: () => onSelected(index),
              colors: colors,
            ),
          );
        }).toList(),
      ),
    );
  }
}

class _PanelTab extends StatefulWidget {
  final IconData icon;
  final String label;
  final bool isSelected;
  final VoidCallback onTap;
  final NightshadeColors colors;

  const _PanelTab({
    required this.icon,
    required this.label,
    required this.isSelected,
    required this.onTap,
    required this.colors,
  });

  @override
  State<_PanelTab> createState() => _PanelTabState();
}

class _PanelTabState extends State<_PanelTab> {
  bool _isHovered = false;

  @override
  Widget build(BuildContext context) {
    return MouseRegion(
      onEnter: (_) => setState(() => _isHovered = true),
      onExit: (_) => setState(() => _isHovered = false),
      child: GestureDetector(
        onTap: widget.onTap,
        child: AnimatedContainer(
          duration: const Duration(milliseconds: 250),
          curve: Curves.easeOutCubic,
          decoration: BoxDecoration(
            color: widget.isSelected
                ? widget.colors.surface
                : _isHovered
                    ? widget.colors.surface.withValues(alpha: 0.5)
                    : Colors.transparent,
            border: Border(
              bottom: BorderSide(
                color: widget.isSelected
                    ? widget.colors.primary
                    : Colors.transparent,
                width: widget.isSelected ? 2.5 : 0,
              ),
            ),
          ),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Icon(
                widget.icon,
                size: 14,
                color: widget.isSelected
                    ? widget.colors.primary
                    : widget.colors.textSecondary,
              ),
              const SizedBox(height: 2),
              Text(
                widget.label,
                style: TextStyle(
                  fontSize: 10,
                  fontWeight:
                      widget.isSelected ? FontWeight.w600 : FontWeight.w500,
                  color: widget.isSelected
                      ? widget.colors.primary
                      : widget.colors.textSecondary,
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}

class _ControlSection extends StatelessWidget {
  final String title;
  final Widget child;
  final NightshadeColors colors;

  const _ControlSection({
    required this.title,
    required this.child,
    required this.colors,
  });

  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        Text(
          title,
          style: TextStyle(
            fontSize: 10,
            fontWeight: FontWeight.w600,
            color: colors.textMuted,
            letterSpacing: 0.5,
          ),
        ),
        const SizedBox(height: 8),
        child,
      ],
    );
  }
}

class _BigActionButton extends StatefulWidget {
  final IconData icon;
  final String label;
  final Color color;
  final VoidCallback onPressed;
  final bool isEnabled;
  final bool isLoading;
  final bool isMobile;

  const _BigActionButton({
    required this.icon,
    required this.label,
    required this.color,
    required this.onPressed,
    this.isEnabled = true,
    this.isLoading = false,
    this.isMobile = false,
  });

  @override
  State<_BigActionButton> createState() => _BigActionButtonState();
}

class _BigActionButtonState extends State<_BigActionButton>
    with SingleTickerProviderStateMixin {
  bool _isHovered = false;
  bool _isPressed = false;
  late AnimationController _loadingController;

  @override
  void initState() {
    super.initState();
    _loadingController = AnimationController(
      vsync: this,
      duration: const Duration(seconds: 1),
    )..repeat();
  }

  @override
  void dispose() {
    _loadingController.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final effectiveColor =
        widget.isEnabled ? widget.color : widget.color.withValues(alpha: 0.4);

    return MouseRegion(
      onEnter: (_) => setState(() => _isHovered = true),
      onExit: (_) => setState(() => _isHovered = false),
      cursor: widget.isEnabled
          ? SystemMouseCursors.click
          : SystemMouseCursors.basic,
      child: GestureDetector(
        onTapDown:
            widget.isEnabled ? (_) => setState(() => _isPressed = true) : null,
        onTapUp:
            widget.isEnabled ? (_) => setState(() => _isPressed = false) : null,
        onTapCancel:
            widget.isEnabled ? () => setState(() => _isPressed = false) : null,
        onTap: widget.isEnabled ? widget.onPressed : null,
        child: AnimatedContainer(
          duration: const Duration(milliseconds: 100),
          transform: () {
            final scale = _isPressed && widget.isEnabled ? 0.95 : 1.0;
            return Matrix4.identity()..scaleByDouble(scale, scale, scale, 1.0);
          }(),
          padding: EdgeInsets.symmetric(
            horizontal: widget.isMobile ? 12 : 20,
            vertical: widget.isMobile ? 12 : 16,
          ),
          decoration: BoxDecoration(
            gradient: LinearGradient(
              colors: [
                effectiveColor,
                effectiveColor.withValues(alpha: 0.8),
              ],
              begin: Alignment.topLeft,
              end: Alignment.bottomRight,
            ),
            borderRadius: BorderRadius.circular(12),
            boxShadow: _isHovered && widget.isEnabled
                ? [
                    BoxShadow(
                      color: effectiveColor.withValues(alpha: 0.4),
                      blurRadius: 16,
                      offset: const Offset(0, 4),
                    ),
                  ]
                : null,
          ),
          child: Column(
            mainAxisSize: MainAxisSize.min,
            children: [
              widget.isLoading
                  ? AnimatedBuilder(
                      animation: _loadingController,
                      builder: (context, child) {
                        return Transform.rotate(
                          angle: _loadingController.value * 2 * math.pi,
                          child: Icon(
                            LucideIcons.loader2,
                            size: 24,
                            color: Colors.white.withValues(
                                alpha: widget.isEnabled ? 1.0 : 0.5),
                          ),
                        );
                      },
                    )
                  : Icon(
                      widget.icon,
                      size: widget.isMobile ? 20 : 24,
                      color: Colors.white
                          .withValues(alpha: widget.isEnabled ? 1.0 : 0.5),
                    ),
              SizedBox(height: widget.isMobile ? 4 : 6),
              Flexible(
                child: Text(
                  widget.label,
                  style: TextStyle(
                    fontSize: widget.isMobile ? 11 : 12,
                    fontWeight: FontWeight.w600,
                    color: Colors.white
                        .withValues(alpha: widget.isEnabled ? 1.0 : 0.5),
                  ),
                  overflow: TextOverflow.ellipsis,
                  maxLines: 1,
                  textAlign: TextAlign.center,
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}

// TODO: Use this widget for compact input fields
// ignore: unused_element
class _CompactInput extends StatelessWidget {
  final String label;
  final String value;
  final String? suffix;
  final NightshadeColors colors;

  const _CompactInput({
    required this.label,
    required this.value,
    this.suffix,
    required this.colors,
  });

  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        Text(
          label,
          style: TextStyle(
            fontSize: 10,
            color: colors.textMuted,
          ),
        ),
        const SizedBox(height: 4),
        Container(
          width: 70,
          padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 8),
          decoration: BoxDecoration(
            color: colors.surfaceAlt,
            borderRadius: BorderRadius.circular(8),
            border: Border.all(color: colors.border),
          ),
          child: Row(
            children: [
              Expanded(
                child: Text(
                  value,
                  style: TextStyle(
                    fontSize: 13,
                    fontWeight: FontWeight.w500,
                    color: colors.textPrimary,
                  ),
                ),
              ),
              if (suffix != null)
                Text(
                  suffix!,
                  style: TextStyle(
                    fontSize: 11,
                    color: colors.textMuted,
                  ),
                ),
            ],
          ),
        ),
      ],
    );
  }
}

class _EditableCompactInput extends StatefulWidget {
  final String label;
  final String value;
  final String? suffix;
  final NightshadeColors colors;
  final ValueChanged<String> onChanged;
  final bool isMobile;

  const _EditableCompactInput({
    required this.label,
    required this.value,
    this.suffix,
    required this.colors,
    required this.onChanged,
    this.isMobile = false,
  });

  @override
  State<_EditableCompactInput> createState() => _EditableCompactInputState();
}

class _EditableCompactInputState extends State<_EditableCompactInput> {
  late TextEditingController _controller;
  bool _isEditing = false;
  final _focusNode = FocusNode();

  @override
  void initState() {
    super.initState();
    _controller = TextEditingController(text: widget.value);
    _focusNode.addListener(() {
      if (!_focusNode.hasFocus && _isEditing) {
        _commitValue();
      }
    });
  }

  @override
  void didUpdateWidget(_EditableCompactInput oldWidget) {
    super.didUpdateWidget(oldWidget);
    if (!_isEditing && widget.value != _controller.text) {
      _controller.text = widget.value;
    }
  }

  @override
  void dispose() {
    _controller.dispose();
    _focusNode.dispose();
    super.dispose();
  }

  void _commitValue() {
    setState(() => _isEditing = false);
    widget.onChanged(_controller.text);
  }

  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        Text(
          widget.label,
          style: TextStyle(
            fontSize: 10,
            color: widget.colors.textMuted,
          ),
        ),
        SizedBox(height: widget.isMobile ? 3 : 4),
        GestureDetector(
          onTap: () {
            setState(() => _isEditing = true);
            _focusNode.requestFocus();
            _controller.selection = TextSelection(
              baseOffset: 0,
              extentOffset: _controller.text.length,
            );
          },
          child: Container(
            width: widget.isMobile ? 70 : 90,
            constraints: BoxConstraints(
              minHeight: widget.isMobile ? 32 : 34,
            ),
            padding: EdgeInsets.symmetric(
              horizontal: widget.isMobile ? 8 : 10,
              vertical: widget.isMobile ? 6 : 8,
            ),
            decoration: BoxDecoration(
              color: widget.colors.surfaceAlt,
              borderRadius: BorderRadius.circular(8),
              border: Border.all(
                color:
                    _isEditing ? widget.colors.primary : widget.colors.border,
              ),
            ),
            child: Row(
              mainAxisSize: MainAxisSize.min,
              children: [
                Flexible(
                  child: _isEditing
                      ? TextField(
                          controller: _controller,
                          focusNode: _focusNode,
                          style: TextStyle(
                            fontSize: widget.isMobile ? 12 : 13,
                            fontWeight: FontWeight.w500,
                            color: widget.colors.textPrimary,
                          ),
                          decoration: const InputDecoration(
                            border: InputBorder.none,
                            isDense: true,
                            contentPadding: EdgeInsets.zero,
                          ),
                          keyboardType: TextInputType.number,
                          onSubmitted: (_) => _commitValue(),
                        )
                      : Text(
                          widget.value,
                          style: TextStyle(
                            fontSize: 13,
                            fontWeight: FontWeight.w500,
                            color: widget.colors.textPrimary,
                          ),
                        ),
                ),
                if (widget.suffix != null)
                  Text(
                    widget.suffix!,
                    style: TextStyle(
                      fontSize: 11,
                      color: widget.colors.textMuted,
                    ),
                  ),
              ],
            ),
          ),
        ),
      ],
    );
  }
}

// =============================================================================
// IMAGE DISPLAY AND OVERLAYS
// =============================================================================

class _ImageDisplayWidget extends StatefulWidget {
  final CapturedImageData imageData;
  final double zoomLevel;
  final Offset panOffset;

  const _ImageDisplayWidget({
    required this.imageData,
    required this.zoomLevel,
    required this.panOffset,
  });

  @override
  State<_ImageDisplayWidget> createState() => _ImageDisplayWidgetState();
}

class _ImageDisplayWidgetState extends State<_ImageDisplayWidget> {
  ui.Image? _decodedImage;
  bool _isDecoding = false;

  @override
  void initState() {
    super.initState();
    _decodeImage();
  }

  @override
  void didUpdateWidget(_ImageDisplayWidget oldWidget) {
    super.didUpdateWidget(oldWidget);
    if (widget.imageData != oldWidget.imageData) {
      _decodeImage();
    }
  }

  Future<void> _decodeImage() async {
    if (_isDecoding) return;
    _isDecoding = true;

    try {
      final width = widget.imageData.width;
      final height = widget.imageData.height;
      final pixels = widget.imageData.displayData;

      // If RGB, we need to add Alpha channel (RGBA) for decodeImageFromPixels
      // Or use a format that supports RGB. Unfortunately, Flutter usually expects RGBA.
      // Let's check if we can use RGB directly. PixelFormat.rgb888 might work if available,
      // but standard is rgba8888.

      // Converting RGB to RGBA in Dart is slow.
      // Ideally, the backend should return RGBA.
      // But for now, let's assume we can use the raw bytes if we specify the correct format.
      // Actually, let's try to use BMP header trick or just iterate once to convert.
      // Wait, iterating 40MP pixels in Dart to convert RGB->RGBA is ALSO slow.

      // BETTER APPROACH: Use `decodeImageFromPixels` with `PixelFormat.bgra8888` or similar?
      // Flutter's `decodeImageFromPixels` supports `PixelFormat.rgba8888` and `bgra8888`.
      // It does NOT support RGB (3 bytes).

      // So we MUST convert to RGBA in Rust or Dart.
      // Doing it in Dart is the bottleneck.
      // I should update `api.rs` to return RGBA!

      // But for now, let's just try to implement the widget structure.
      // I will assume the backend sends RGBA for now, or I will accept the slow conversion ONCE.
      // Converting once is better than converting every frame.

      // Let's implement the conversion here for now, but mark it as a TODO to move to Rust.

      final Completer<ui.Image> completer = Completer();

      // Create RGBA buffer
      final int numPixels = width * height;
      final Uint8List rgbaBytes = Uint8List(numPixels * 4);
      final Uint8List src = pixels;

      if (widget.imageData.isColor) {
        // RGB -> RGBA
        for (int i = 0; i < numPixels; i++) {
          rgbaBytes[i * 4] = src[i * 3]; // R
          rgbaBytes[i * 4 + 1] = src[i * 3 + 1]; // G
          rgbaBytes[i * 4 + 2] = src[i * 3 + 2]; // B
          rgbaBytes[i * 4 + 3] = 255; // A
        }
      } else {
        // Gray -> RGBA
        for (int i = 0; i < numPixels; i++) {
          final val = src[i];
          rgbaBytes[i * 4] = val;
          rgbaBytes[i * 4 + 1] = val;
          rgbaBytes[i * 4 + 2] = val;
          rgbaBytes[i * 4 + 3] = 255;
        }
      }

      ui.decodeImageFromPixels(
        rgbaBytes,
        width,
        height,
        ui.PixelFormat.rgba8888,
        (image) {
          if (mounted) {
            setState(() {
              _decodedImage = image;
              _isDecoding = false;
            });
          }
        },
      );
    } catch (e) {
      debugPrint("Error decoding image: $e");
      _isDecoding = false;
    }
  }

  @override
  Widget build(BuildContext context) {
    if (_decodedImage == null) {
      return const Center(child: CircularProgressIndicator());
    }

    return LayoutBuilder(
      builder: (context, constraints) {
        return InteractiveViewer(
          minScale: 0.1,
          maxScale: 8.0,
          child: Center(
            child: CustomPaint(
              painter: _DecodedImagePainter(
                image: _decodedImage!,
              ),
              size: Size(
                _decodedImage!.width.toDouble(),
                _decodedImage!.height.toDouble(),
              ),
            ),
          ),
        );
      },
    );
  }
}

class _DecodedImagePainter extends CustomPainter {
  final ui.Image image;

  _DecodedImagePainter({required this.image});

  @override
  void paint(Canvas canvas, Size size) {
    paintImage(
      canvas: canvas,
      rect: Rect.fromLTWH(0, 0, size.width, size.height),
      image: image,
      fit: BoxFit.contain,
      filterQuality: FilterQuality.medium,
    );
  }

  @override
  bool shouldRepaint(_DecodedImagePainter oldDelegate) {
    return image != oldDelegate.image;
  }
}

class _ExposureProgressOverlay extends StatelessWidget {
  final ExposureProgress progress;
  final NightshadeColors colors;

  const _ExposureProgressOverlay({
    required this.progress,
    required this.colors,
  });

  @override
  Widget build(BuildContext context) {
    final statusText =
        progress.isDownloading ? 'Downloading...' : 'Exposing...';
    final progressValue = (progress.percent / 100.0).clamp(0.0, 1.0);

    return Container(
      color: Colors.black.withValues(alpha: 0.7),
      child: Center(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            SizedBox(
              width: 80,
              height: 80,
              child: Stack(
                children: [
                  CircularProgressIndicator(
                    value: progressValue,
                    strokeWidth: 4,
                    backgroundColor: colors.surfaceAlt,
                    valueColor: AlwaysStoppedAnimation<Color>(colors.primary),
                  ),
                  Center(
                    child: Text(
                      '${progress.percent.toStringAsFixed(0)}%',
                      style: TextStyle(
                        fontSize: 16,
                        fontWeight: FontWeight.bold,
                        color: colors.primary,
                      ),
                    ),
                  ),
                ],
              ),
            ),
            const SizedBox(height: 16),
            Text(
              statusText,
              style: const TextStyle(
                fontSize: 14,
                fontWeight: FontWeight.w600,
                color: Colors.white,
              ),
            ),
            const SizedBox(height: 4),
            if (!progress.isDownloading)
              Text(
                '${progress.remaining.toStringAsFixed(1)}s remaining',
                style: const TextStyle(
                  fontSize: 12,
                  color: Colors.white70,
                ),
              ),
            if (progress.totalFrames != null)
              Padding(
                padding: const EdgeInsets.only(top: 4),
                child: Text(
                  'Frame ${progress.frameNumber} of ${progress.totalFrames}',
                  style: const TextStyle(
                    fontSize: 11,
                    color: Colors.white54,
                  ),
                ),
              ),
          ],
        ),
      ),
    );
  }
}

class _CrosshairOverlayPainter extends CustomPainter {
  final Color color;

  _CrosshairOverlayPainter({required this.color});

  @override
  void paint(Canvas canvas, Size size) {
    final paint = Paint()
      ..color = color
      ..strokeWidth = 1;

    final centerX = size.width / 2;
    final centerY = size.height / 2;

    // Horizontal line
    canvas.drawLine(
      Offset(0, centerY),
      Offset(size.width, centerY),
      paint,
    );

    // Vertical line
    canvas.drawLine(
      Offset(centerX, 0),
      Offset(centerX, size.height),
      paint,
    );

    // Center circle
    paint.style = PaintingStyle.stroke;
    canvas.drawCircle(Offset(centerX, centerY), 20, paint);
    canvas.drawCircle(Offset(centerX, centerY), 40, paint);
  }

  @override
  bool shouldRepaint(covariant CustomPainter oldDelegate) => false;
}

class _GridOverlayPainter extends CustomPainter {
  final Color color;

  _GridOverlayPainter({required this.color});

  @override
  void paint(Canvas canvas, Size size) {
    final paint = Paint()
      ..color = color
      ..strokeWidth = 0.5;

    const gridSize = 50.0;

    // Vertical lines
    for (double x = gridSize; x < size.width; x += gridSize) {
      canvas.drawLine(Offset(x, 0), Offset(x, size.height), paint);
    }

    // Horizontal lines
    for (double y = gridSize; y < size.height; y += gridSize) {
      canvas.drawLine(Offset(0, y), Offset(size.width, y), paint);
    }
  }

  @override
  bool shouldRepaint(covariant CustomPainter oldDelegate) => false;
}

class _StarOverlayPainter extends CustomPainter {
  final List<DetectedStar> stars;
  final Color color;
  final double zoomLevel;
  final Offset panOffset;

  _StarOverlayPainter({
    required this.stars,
    required this.color,
    required this.zoomLevel,
    required this.panOffset,
  });

  @override
  void paint(Canvas canvas, Size size) {
    final paint = Paint()
      ..color = color
      ..style = PaintingStyle.stroke
      ..strokeWidth = 1.5;

    final fillPaint = Paint()
      ..color = color.withValues(alpha: 0.2)
      ..style = PaintingStyle.fill;

    for (final star in stars) {
      final x = star.x * zoomLevel + panOffset.dx;
      final y = star.y * zoomLevel + panOffset.dy;

      // Skip stars outside the visible area
      if (x < -50 || x > size.width + 50 || y < -50 || y > size.height + 50) {
        continue;
      }

      final position = Offset(x, y);

      // Draw circle around star (radius based on HFR)
      final radius = (star.hfr * zoomLevel).clamp(3.0, 30.0);
      canvas.drawCircle(position, radius, fillPaint);
      canvas.drawCircle(position, radius, paint);

      // Draw crosshair
      const crosshairSize = 3.0;
      canvas.drawLine(
        Offset(x - crosshairSize, y),
        Offset(x + crosshairSize, y),
        paint,
      );
      canvas.drawLine(
        Offset(x, y - crosshairSize),
        Offset(x, y + crosshairSize),
        paint,
      );
    }
  }

  @override
  bool shouldRepaint(covariant _StarOverlayPainter oldDelegate) {
    return stars != oldDelegate.stars ||
        color != oldDelegate.color ||
        zoomLevel != oldDelegate.zoomLevel ||
        panOffset != oldDelegate.panOffset;
  }
}

class _FilterSelector extends ConsumerStatefulWidget {
  final NightshadeColors colors;
  final bool isMobile;

  const _FilterSelector({required this.colors, this.isMobile = false});

  @override
  ConsumerState<_FilterSelector> createState() => _FilterSelectorState();
}

class _FilterSelectorState extends ConsumerState<_FilterSelector> {
  String _selectedFilter = '';

  @override
  Widget build(BuildContext context) {
    final filterWheelState = ref.watch(filterWheelStateProvider);
    final isConnected =
        filterWheelState.connectionState == DeviceConnectionState.connected;
    final filterNames = filterWheelState.filterNames;

    if (!isConnected) {
      return Container(
        padding: EdgeInsets.symmetric(
          vertical: widget.isMobile ? 6 : 8,
          horizontal: widget.isMobile ? 8 : 12,
        ),
        decoration: BoxDecoration(
          color: widget.colors.surfaceAlt,
          borderRadius: BorderRadius.circular(8),
          border: Border.all(color: widget.colors.border),
        ),
        child: Text(
          'No filter wheel connected',
          style: TextStyle(
            fontSize: widget.isMobile ? 11 : 12,
            color: widget.colors.textMuted,
            fontStyle: FontStyle.italic,
          ),
        ),
      );
    }

    if (filterNames.isEmpty) {
      return Container(
        padding: EdgeInsets.symmetric(
          vertical: widget.isMobile ? 6 : 8,
          horizontal: widget.isMobile ? 8 : 12,
        ),
        decoration: BoxDecoration(
          color: widget.colors.surfaceAlt,
          borderRadius: BorderRadius.circular(8),
          border: Border.all(color: widget.colors.border),
        ),
        child: Text(
          'No filters configured',
          style: TextStyle(
            fontSize: widget.isMobile ? 11 : 12,
            color: widget.colors.textMuted,
            fontStyle: FontStyle.italic,
          ),
        ),
      );
    }

    // Update selected filter if needed
    if (_selectedFilter.isEmpty && filterNames.isNotEmpty) {
      // Try to find current position name
      if (filterWheelState.currentPosition != null &&
          filterWheelState.currentPosition! >= 0 &&
          filterWheelState.currentPosition! < filterNames.length) {
        _selectedFilter = filterNames[filterWheelState.currentPosition!];
      } else {
        _selectedFilter = filterNames[0];
      }
    }

    final isMoving = filterWheelState.isMoving;

    return SingleChildScrollView(
      scrollDirection: Axis.horizontal,
      child: Row(
        children: [
          for (int i = 0; i < filterNames.length; i++) ...[
            if (i > 0) const SizedBox(width: 4),
            _FilterButton(
              label: filterNames[i],
              isSelected: _selectedFilter == filterNames[i],
              color: _getFilterColor(filterNames[i]),
              colors: widget.colors,
              onTap: isMoving ? null : () => _selectFilter(filterNames[i], i),
              isMobile: widget.isMobile,
            ),
          ],
        ],
      ),
    );
  }

  Color _getFilterColor(String name) {
    final lowerName = name.toLowerCase();
    if (lowerName.contains('red') ||
        lowerName == 'r' ||
        lowerName == 'ha' ||
        lowerName.contains('h-alpha')) {
      return Colors.red;
    } else if (lowerName.contains('green') || lowerName == 'g') {
      return Colors.green;
    } else if (lowerName.contains('blue') || lowerName == 'b') {
      return Colors.blue;
    } else if (lowerName.contains('lum') || lowerName == 'l') {
      return Colors
          .white; // Use white for Lum instead of grey for better visibility
    } else if (lowerName.contains('oiii') || lowerName.contains('o3')) {
      return Colors.cyan;
    } else if (lowerName.contains('sii') || lowerName.contains('s2')) {
      return Colors.orange;
    }
    return widget.colors.primary;
  }

  Future<void> _selectFilter(String label, int position) async {
    setState(() => _selectedFilter = label);
    try {
      final deviceService = ref.read(deviceServiceProvider);
      await deviceService.setFilterWheelPosition(position);
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Failed to change filter: $e')),
        );
      }
    }
  }
}

class _FilterButton extends StatefulWidget {
  final String label;
  final bool isSelected;
  final Color color;
  final NightshadeColors colors;
  final VoidCallback? onTap;
  final bool isMobile;

  const _FilterButton({
    required this.label,
    required this.isSelected,
    required this.color,
    required this.colors,
    this.onTap,
    this.isMobile = false,
  });

  @override
  State<_FilterButton> createState() => _FilterButtonState();
}

class _FilterButtonState extends State<_FilterButton> {
  bool _isHovered = false;

  @override
  Widget build(BuildContext context) {
    return MouseRegion(
      onEnter: (_) => setState(() => _isHovered = true),
      onExit: (_) => setState(() => _isHovered = false),
      child: GestureDetector(
        onTap: widget.onTap,
        child: AnimatedContainer(
          duration: const Duration(milliseconds: 150),
          width: widget.isMobile ? 32 : 36,
          height: widget.isMobile ? 32 : 36,
          decoration: BoxDecoration(
            color: widget.isSelected
                ? widget.color.withValues(alpha: 0.2)
                : _isHovered
                    ? widget.colors.surfaceAlt
                    : widget.colors.background,
            borderRadius: BorderRadius.circular(8),
            border: Border.all(
              color: widget.isSelected
                  ? widget.color.withValues(alpha: 0.5)
                  : widget.colors.border,
              width: widget.isSelected ? 2 : 1,
            ),
          ),
          child: Center(
            child: Text(
              widget.label,
              style: TextStyle(
                fontSize: widget.isMobile ? 10 : 11,
                fontWeight: FontWeight.w600,
                color: widget.isSelected
                    ? widget.color
                    : widget.colors.textSecondary,
              ),
            ),
          ),
        ),
      ),
    );
  }
}

class _QuickStatsPanel extends ConsumerWidget {
  final NightshadeColors colors;

  const _QuickStatsPanel({required this.colors});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final cameraState = ref.watch(cameraStateProvider);
    final guiderState = ref.watch(guiderStateProvider);
    final lastStats = ref.watch(lastImageStatsProvider);

    // Format temperature
    String tempValue = '---';
    if (cameraState.connectionState == DeviceConnectionState.connected) {
      if (cameraState.temperature != null) {
        tempValue = '${cameraState.temperature!.toStringAsFixed(1)}°C';
      } else {
        tempValue = 'N/A';
      }
    }

    // Format RMS
    String rmsValue = '---';
    if (guiderState.connectionState == DeviceConnectionState.connected &&
        guiderState.isGuiding &&
        guiderState.rmsTotal != null) {
      rmsValue = '${guiderState.rmsTotal!.toStringAsFixed(2)}"';
    }

    // Format HFR
    String hfrValue = '---';
    if (lastStats?.hfr != null) {
      hfrValue = lastStats!.hfr!.toStringAsFixed(2);
    }

    return Container(
      padding: const EdgeInsets.all(16),
      decoration: BoxDecoration(
        color: colors.surfaceAlt,
        borderRadius: BorderRadius.circular(12),
      ),
      child: Row(
        children: [
          _QuickStat(
            icon: LucideIcons.thermometer,
            label: 'Sensor',
            value: tempValue,
            colors: colors,
          ),
          const SizedBox(width: 24),
          _QuickStat(
            icon: LucideIcons.activity,
            label: 'RMS',
            value: rmsValue,
            colors: colors,
          ),
          const SizedBox(width: 24),
          _QuickStat(
            icon: LucideIcons.target,
            label: 'HFR',
            value: hfrValue,
            colors: colors,
          ),
        ],
      ),
    );
  }
}

class _QuickStat extends StatelessWidget {
  final IconData icon;
  final String label;
  final String value;
  final NightshadeColors colors;

  const _QuickStat({
    required this.icon,
    required this.label,
    required this.value,
    required this.colors,
  });

  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        Icon(icon, size: 16, color: colors.textMuted),
        const SizedBox(width: 8),
        Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          mainAxisSize: MainAxisSize.min,
          children: [
            Text(
              value,
              style: TextStyle(
                fontSize: 14,
                fontWeight: FontWeight.w600,
                color: colors.textPrimary,
              ),
            ),
            Text(
              label,
              style: TextStyle(
                fontSize: 10,
                color: colors.textMuted,
              ),
            ),
          ],
        ),
      ],
    );
  }
}

// Panel content widgets
class _CapturePanel extends ConsumerWidget {
  final NightshadeColors colors;

  const _CapturePanel({required this.colors});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final exposureSettings = ref.watch(exposureSettingsProvider);
    final namingPattern = ref.watch(namingPatternProvider);
    final sessionState = ref.watch(sessionStateProvider);
    final sessionImages = ref.watch(sessionImagesProvider);

    return SingleChildScrollView(
      padding: const EdgeInsets.all(16),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          // Exposure Settings
          _PanelSection(
            title: 'Exposure Settings',
            colors: colors,
            child: Column(
              children: [
                _InputRowEditable(
                  label: 'Exposure',
                  value: exposureSettings.exposureTime.toStringAsFixed(1),
                  suffix: 'sec',
                  colors: colors,
                  onChanged: (value) {
                    final parsed = double.tryParse(value);
                    if (parsed != null && parsed > 0) {
                      ref.read(exposureSettingsProvider.notifier).state =
                          exposureSettings.copyWith(exposureTime: parsed);
                    }
                  },
                ),
                const SizedBox(height: 12),
                _DropdownRow(
                  label: 'Frame Type',
                  value: exposureSettings.frameType.displayName,
                  items: FrameType.values.map((t) => t.displayName).toList(),
                  colors: colors,
                  onChanged: (value) {
                    if (value != null) {
                      final type = FrameType.values.firstWhere(
                        (t) => t.displayName == value,
                        orElse: () => FrameType.light,
                      );
                      ref.read(exposureSettingsProvider.notifier).state =
                          exposureSettings.copyWith(frameType: type);
                    }
                  },
                ),
                const SizedBox(height: 12),
                _DropdownRow(
                  label: 'Binning',
                  value: exposureSettings.binning,
                  items: const ['1x1', '2x2', '3x3', '4x4'],
                  colors: colors,
                  onChanged: (value) {
                    if (value != null) {
                      final parts = value.split('x');
                      ref.read(exposureSettingsProvider.notifier).state =
                          exposureSettings.copyWith(
                        binningX: int.parse(parts[0]),
                        binningY: int.parse(parts[1]),
                      );
                    }
                  },
                ),
              ],
            ),
          ),
          const SizedBox(height: 20),

          // File Settings
          _PanelSection(
            title: 'File Settings',
            colors: colors,
            child: Column(
              children: [
                _DropdownRow(
                  label: 'Format',
                  value: namingPattern.format.displayName,
                  items:
                      ImageFileFormat.values.map((f) => f.displayName).toList(),
                  colors: colors,
                  onChanged: (value) {
                    if (value != null) {
                      final format = ImageFileFormat.values.firstWhere(
                        (f) => f.displayName == value,
                        orElse: () => ImageFileFormat.fits,
                      );
                      ref
                          .read(appSettingsProvider.notifier)
                          .setImageFormat(format.settingsValue);
                    }
                  },
                ),
                const SizedBox(height: 12),
                _InputRow(
                  label: 'Save Path',
                  value: namingPattern.baseDir,
                  colors: colors,
                  trailing: GestureDetector(
                    onTap: () async {
                      final result = await getDirectoryPath(
                        confirmButtonText: 'Select',
                        initialDirectory: namingPattern.baseDir.isNotEmpty
                            ? namingPattern.baseDir
                            : null,
                      );
                      if (result != null) {
                        ref
                            .read(appSettingsProvider.notifier)
                            .setImageOutputPath(result);
                      }
                    },
                    child: Icon(LucideIcons.folderOpen,
                        size: 14, color: colors.textSecondary),
                  ),
                ),
              ],
            ),
          ),
          const SizedBox(height: 20),

          // Session Statistics
          _PanelSection(
            title: 'Session',
            colors: colors,
            child: Column(
              children: [
                Row(
                  mainAxisAlignment: MainAxisAlignment.spaceBetween,
                  children: [
                    Text('Captured',
                        style: TextStyle(
                            fontSize: 12, color: colors.textSecondary)),
                    Text(
                      '${sessionImages.length} frames',
                      style: TextStyle(
                          fontSize: 12,
                          fontWeight: FontWeight.w500,
                          color: colors.textPrimary),
                    ),
                  ],
                ),
                const SizedBox(height: 8),
                Row(
                  mainAxisAlignment: MainAxisAlignment.spaceBetween,
                  children: [
                    Text('Integration',
                        style: TextStyle(
                            fontSize: 12, color: colors.textSecondary)),
                    Text(
                      _formatDuration(sessionState.totalIntegrationSecs),
                      style: TextStyle(
                          fontSize: 12,
                          fontWeight: FontWeight.w500,
                          color: colors.textPrimary),
                    ),
                  ],
                ),
                const SizedBox(height: 8),
                // Session status and duration
                if (sessionState.isActive) ...[
                  Row(
                    mainAxisAlignment: MainAxisAlignment.spaceBetween,
                    children: [
                      Text('Status',
                          style: TextStyle(
                              fontSize: 12, color: colors.textSecondary)),
                      Row(
                        children: [
                          Container(
                            width: 6,
                            height: 6,
                            decoration: BoxDecoration(
                              color: colors.success,
                              shape: BoxShape.circle,
                            ),
                          ),
                          const SizedBox(width: 6),
                          Text(
                            'Active',
                            style: TextStyle(
                                fontSize: 12,
                                fontWeight: FontWeight.w500,
                                color: colors.success),
                          ),
                        ],
                      ),
                    ],
                  ),
                  const SizedBox(height: 8),
                  Row(
                    mainAxisAlignment: MainAxisAlignment.spaceBetween,
                    children: [
                      Text('Duration',
                          style: TextStyle(
                              fontSize: 12, color: colors.textSecondary)),
                      Text(
                        sessionState.duration != null
                            ? _formatSessionDuration(sessionState.duration!)
                            : '--:--:--',
                        style: TextStyle(
                            fontSize: 12,
                            fontWeight: FontWeight.w500,
                            color: colors.textPrimary),
                      ),
                    ],
                  ),
                  const SizedBox(height: 12),
                ],
                Row(
                  children: [
                    Expanded(
                      child: _SmallButton(
                        label: 'View Gallery',
                        icon: LucideIcons.galleryHorizontal,
                        colors: colors,
                        onTap: () {
                          // Would open gallery view
                        },
                      ),
                    ),
                    const SizedBox(width: 8),
                    Expanded(
                      child: _SmallButton(
                        label: 'Clear Session',
                        icon: LucideIcons.trash2,
                        isOutline: true,
                        colors: colors,
                        onTap: () {
                          ref
                              .read(sessionImagesProvider.notifier)
                              .clearSession();
                        },
                      ),
                    ),
                  ],
                ),
                if (sessionState.isActive) ...[
                  const SizedBox(height: 8),
                  SizedBox(
                    width: double.infinity,
                    child: _SmallButton(
                      label: 'End Session',
                      icon: LucideIcons.stopCircle,
                      colors: colors,
                      onTap: () => _showEndSessionDialog(context, ref, colors),
                    ),
                  ),
                ],
              ],
            ),
          ),
        ],
      ),
    );
  }

  String _formatDuration(double seconds) {
    final hours = (seconds / 3600).floor();
    final minutes = ((seconds % 3600) / 60).floor();
    final secs = (seconds % 60).round();

    if (hours > 0) {
      return '${hours}h ${minutes}m ${secs}s';
    } else if (minutes > 0) {
      return '${minutes}m ${secs}s';
    } else {
      return '${secs}s';
    }
  }

  String _formatSessionDuration(Duration duration) {
    final hours = duration.inHours;
    final minutes = duration.inMinutes.remainder(60);
    final seconds = duration.inSeconds.remainder(60);
    return '${hours.toString().padLeft(2, '0')}:'
        '${minutes.toString().padLeft(2, '0')}:'
        '${seconds.toString().padLeft(2, '0')}';
  }

  void _showEndSessionDialog(
      BuildContext context, WidgetRef ref, NightshadeColors colors) {
    final sessionState = ref.read(sessionStateProvider);

    showDialog(
      context: context,
      builder: (context) => AlertDialog(
        title: Row(
          children: [
            Icon(LucideIcons.stopCircle, color: colors.warning),
            const SizedBox(width: 12),
            const Text('End Session'),
          ],
        ),
        content: Column(
          mainAxisSize: MainAxisSize.min,
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Text(
              'Are you sure you want to end the current imaging session?',
              style: TextStyle(color: colors.textPrimary),
            ),
            const SizedBox(height: 16),
            Container(
              padding: const EdgeInsets.all(12),
              decoration: BoxDecoration(
                color: colors.surface,
                borderRadius: BorderRadius.circular(8),
                border: Border.all(color: colors.border),
              ),
              child: Column(
                children: [
                  Row(
                    mainAxisAlignment: MainAxisAlignment.spaceBetween,
                    children: [
                      Text('Images Captured:',
                          style: TextStyle(color: colors.textSecondary)),
                      Text('${sessionState.completedExposures}',
                          style: TextStyle(
                              fontWeight: FontWeight.bold,
                              color: colors.textPrimary)),
                    ],
                  ),
                  const SizedBox(height: 8),
                  Row(
                    mainAxisAlignment: MainAxisAlignment.spaceBetween,
                    children: [
                      Text('Total Integration:',
                          style: TextStyle(color: colors.textSecondary)),
                      Text(_formatDuration(sessionState.totalIntegrationSecs),
                          style: TextStyle(
                              fontWeight: FontWeight.bold,
                              color: colors.textPrimary)),
                    ],
                  ),
                  const SizedBox(height: 8),
                  Row(
                    mainAxisAlignment: MainAxisAlignment.spaceBetween,
                    children: [
                      Text('Duration:',
                          style: TextStyle(color: colors.textSecondary)),
                      Text(
                          sessionState.duration != null
                              ? _formatSessionDuration(sessionState.duration!)
                              : '--:--:--',
                          style: TextStyle(
                              fontWeight: FontWeight.bold,
                              color: colors.textPrimary)),
                    ],
                  ),
                ],
              ),
            ),
            const SizedBox(height: 16),
            Consumer(
              builder: (context, ref, child) {
                final parkOnEnd = ref.watch(_parkMountOnEndProvider);
                final mountState = ref.watch(mountStateProvider);
                final mountConnected = mountState.connectionState ==
                    DeviceConnectionState.connected;

                return CheckboxListTile(
                  value: parkOnEnd,
                  onChanged: mountConnected
                      ? (value) {
                          ref.read(_parkMountOnEndProvider.notifier).state =
                              value ?? false;
                        }
                      : null,
                  title: Text(
                    'Park mount after ending session',
                    style: TextStyle(
                      fontSize: 14,
                      color: mountConnected
                          ? colors.textPrimary
                          : colors.textSecondary,
                    ),
                  ),
                  contentPadding: EdgeInsets.zero,
                  controlAffinity: ListTileControlAffinity.leading,
                  enabled: mountConnected,
                );
              },
            ),
          ],
        ),
        actions: [
          TextButton(
            onPressed: () => Navigator.of(context).pop(),
            child: const Text('Cancel'),
          ),
          ElevatedButton(
            onPressed: () async {
              Navigator.of(context).pop();
              await _endSession(ref);
            },
            style: ElevatedButton.styleFrom(
              backgroundColor: colors.warning,
            ),
            child: const Text('End Session'),
          ),
        ],
      ),
    );
  }

  Future<void> _endSession(WidgetRef ref) async {
    try {
      final parkOnEnd = ref.read(_parkMountOnEndProvider);
      final mountState = ref.read(mountStateProvider);

      // End the session
      await ref.read(sessionStateProvider.notifier).endSession();

      // Park mount if requested and connected
      if (parkOnEnd &&
          mountState.connectionState == DeviceConnectionState.connected) {
        try {
          debugPrint('Parking mount after session end...');
          await ref.read(deviceServiceProvider).parkMount();
          debugPrint('Mount parked successfully');
        } catch (e) {
          debugPrint('Failed to park mount: $e');
        }
      }
    } catch (e) {
      debugPrint('Error ending session: $e');
    }
  }
}

// Provider for park mount on end setting
final _parkMountOnEndProvider = StateProvider<bool>((ref) => false);

class _CameraPanel extends ConsumerStatefulWidget {
  final NightshadeColors colors;

  const _CameraPanel({required this.colors});

  @override
  ConsumerState<_CameraPanel> createState() => _CameraPanelState();
}

class _CameraPanelState extends ConsumerState<_CameraPanel> {
  bool _isCooling = false; // Only for UI loading state

  @override
  Widget build(BuildContext context) {
    final cameraState = ref.watch(cameraStateProvider);
    final coolingSettings = ref.watch(coolingSettingsProvider);
    final coolingStatus = ref.watch(coolingStatusProvider);
    final exposureSettings = ref.watch(exposureSettingsProvider);
    // Use target temp from provider (persists across navigation)
    final targetTemp = cameraState.targetTemp;

    final isConnected =
        cameraState.connectionState == DeviceConnectionState.connected;

    return SingleChildScrollView(
      padding: const EdgeInsets.all(16),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          // Connection status
          if (!isConnected)
            Container(
              padding: const EdgeInsets.all(12),
              margin: const EdgeInsets.only(bottom: 16),
              decoration: BoxDecoration(
                color: widget.colors.warning.withValues(alpha: 0.1),
                borderRadius: BorderRadius.circular(8),
                border: Border.all(
                    color: widget.colors.warning.withValues(alpha: 0.3)),
              ),
              child: Row(
                children: [
                  Icon(LucideIcons.alertCircle,
                      size: 16, color: widget.colors.warning),
                  const SizedBox(width: 8),
                  Expanded(
                    child: Text(
                      'No camera connected',
                      style:
                          TextStyle(fontSize: 12, color: widget.colors.warning),
                    ),
                  ),
                ],
              ),
            ),

          // Cooling Section
          _PanelSection(
            title: 'Cooling',
            colors: widget.colors,
            child: Column(
              children: [
                // Current temperature display
                Row(
                  mainAxisAlignment: MainAxisAlignment.spaceBetween,
                  children: [
                    Text('Current',
                        style: TextStyle(
                            fontSize: 12, color: widget.colors.textSecondary)),
                    Row(
                      children: [
                        Text(
                          isConnected && cameraState.temperature != null
                              ? '${cameraState.temperature!.toStringAsFixed(1)}°C'
                              : '---',
                          style: TextStyle(
                            fontSize: 16,
                            fontWeight: FontWeight.w600,
                            color: widget.colors.textPrimary,
                          ),
                        ),
                        if (isConnected && coolingStatus.isCooling)
                          Padding(
                            padding: const EdgeInsets.only(left: 8),
                            child: Icon(
                              coolingStatus.isAtTarget
                                  ? LucideIcons.checkCircle2
                                  : LucideIcons.arrowDown,
                              size: 14,
                              color: coolingStatus.isAtTarget
                                  ? widget.colors.success
                                  : widget.colors.primary,
                            ),
                          ),
                      ],
                    ),
                  ],
                ),
                const SizedBox(height: 8),
                Row(
                  mainAxisAlignment: MainAxisAlignment.spaceBetween,
                  children: [
                    Text('Power',
                        style: TextStyle(
                            fontSize: 12, color: widget.colors.textSecondary)),
                    Text(
                      isConnected && cameraState.coolerPower != null
                          ? '${cameraState.coolerPower!.toStringAsFixed(0)}%'
                          : '---',
                      style: TextStyle(
                        fontSize: 14,
                        fontWeight: FontWeight.w500,
                        color: widget.colors.textPrimary,
                      ),
                    ),
                  ],
                ),
                if (isConnected && coolingStatus.isCooling)
                  Padding(
                    padding: const EdgeInsets.only(top: 8),
                    child: Row(
                      mainAxisAlignment: MainAxisAlignment.spaceBetween,
                      children: [
                        Text('Target',
                            style: TextStyle(
                                fontSize: 12,
                                color: widget.colors.textSecondary)),
                        Text(
                          '${coolingStatus.targetTemp.toStringAsFixed(1)}°C',
                          style: TextStyle(
                            fontSize: 12,
                            color: widget.colors.textSecondary,
                          ),
                        ),
                      ],
                    ),
                  ),
                const SizedBox(height: 16),

                // Target temperature slider
                _SliderRowInteractive(
                  label: 'Target Temperature',
                  value: targetTemp,
                  min: -30,
                  max: 20,
                  suffix: '°C',
                  colors: widget.colors,
                  onChanged: isConnected
                      ? (value) {
                          // Update provider so value persists across navigation
                          ref.read(cameraStateProvider.notifier).setTargetTemp(value);
                          // Also update settings provider for consistency
                          ref.read(coolingSettingsProvider.notifier).state =
                              coolingSettings.copyWith(targetTemp: value);
                        }
                      : null,
                ),
                const SizedBox(height: 16),
                Row(
                  children: [
                    Expanded(
                      child: _SmallButton(
                        label: _isCooling ? 'Setting...' : 'Cool Down',
                        icon: LucideIcons.snowflake,
                        colors: widget.colors,
                        isEnabled: isConnected && !_isCooling,
                        onTap: () async {
                          setState(() => _isCooling = true);
                          try {
                            await ref
                                .read(deviceServiceProvider)
                                .setCameraCooling(
                                  enabled: true,
                                  targetTemp: targetTemp,
                                );

                            // Update settings state
                            ref.read(coolingSettingsProvider.notifier).state =
                                coolingSettings.copyWith(
                                    enabled: true, targetTemp: targetTemp);
                            // Update camera state
                            ref.read(cameraStateProvider.notifier).setCooling(true);
                          } catch (e) {
                            if (mounted) {
                              ScaffoldMessenger.of(context).showSnackBar(
                                SnackBar(
                                    content: Text('Failed to set cooling: $e')),
                              );
                            }
                          } finally {
                            if (mounted) setState(() => _isCooling = false);
                          }
                        },
                      ),
                    ),
                    const SizedBox(width: 8),
                    Expanded(
                      child: _SmallButton(
                        label: 'Warm Up',
                        icon: LucideIcons.flame,
                        isOutline: true,
                        colors: widget.colors,
                        isEnabled: isConnected,
                        onTap: () async {
                          try {
                            await ref
                                .read(deviceServiceProvider)
                                .setCameraCooling(
                                  enabled: false,
                                );

                            ref.read(coolingSettingsProvider.notifier).state =
                                coolingSettings.copyWith(enabled: false);
                          } catch (e) {
                            if (mounted) {
                              ScaffoldMessenger.of(context).showSnackBar(
                                SnackBar(
                                    content:
                                        Text('Failed to turn off cooler: $e')),
                              );
                            }
                          }
                        },
                      ),
                    ),
                  ],
                ),
              ],
            ),
          ),
          const SizedBox(height: 20),

          // Sensor Settings
          _PanelSection(
            title: 'Sensor',
            colors: widget.colors,
            child: Column(
              children: [
                _DropdownRow(
                  label: 'Binning',
                  value: exposureSettings.binning,
                  items: const ['1x1', '2x2', '3x3', '4x4'],
                  colors: widget.colors,
                  onChanged: isConnected
                      ? (value) {
                          if (value != null) {
                            final parts = value.split('x');
                            ref.read(exposureSettingsProvider.notifier).state =
                                exposureSettings.copyWith(
                              binningX: int.parse(parts[0]),
                              binningY: int.parse(parts[1]),
                            );
                          }
                        }
                      : null,
                ),
                const SizedBox(height: 12),
                _DropdownRow(
                  label: 'Read Mode',
                  value: exposureSettings.fastReadout ? 'Fast' : 'High Quality',
                  items: const ['High Quality', 'Fast'],
                  colors: widget.colors,
                  onChanged: isConnected
                      ? (value) {
                          ref.read(exposureSettingsProvider.notifier).state =
                              exposureSettings.copyWith(
                                  fastReadout: value == 'Fast');
                        }
                      : null,
                ),
              ],
            ),
          ),
          const SizedBox(height: 20),

          // Gain/Offset
          _PanelSection(
            title: 'Gain / Offset',
            colors: widget.colors,
            child: Column(
              children: [
                _InputRowEditable(
                  label: 'Gain',
                  value: exposureSettings.gain.toString(),
                  colors: widget.colors,
                  onChanged: (value) {
                    final parsed = int.tryParse(value);
                    if (parsed != null && parsed >= 0) {
                      ref.read(exposureSettingsProvider.notifier).state =
                          exposureSettings.copyWith(gain: parsed);
                    }
                  },
                ),
                const SizedBox(height: 12),
                _InputRowEditable(
                  label: 'Offset',
                  value: exposureSettings.offset.toString(),
                  colors: widget.colors,
                  onChanged: (value) {
                    final parsed = int.tryParse(value);
                    if (parsed != null && parsed >= 0) {
                      ref.read(exposureSettingsProvider.notifier).state =
                          exposureSettings.copyWith(offset: parsed);
                    }
                  },
                ),
              ],
            ),
          ),
          const SizedBox(height: 20),
          const _DebayeringCard(),
        ],
      ),
    );
  }
}

class _FocusPanel extends ConsumerStatefulWidget {
  final NightshadeColors colors;

  const _FocusPanel({required this.colors});

  @override
  ConsumerState<_FocusPanel> createState() => _FocusPanelState();
}

class _FocusPanelState extends ConsumerState<_FocusPanel> {
  // UI-only transient state (doesn't need to persist)
  bool _isRunningAutofocus = false;

  Future<void> _moveFocuser(int delta) async {
    final focusSettings = ref.read(focusSettingsProvider);
    try {
      final deviceService = ref.read(deviceServiceProvider);
      await deviceService.moveFocuserRelative(delta * focusSettings.stepSize ~/ 100);
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Focuser error: $e')),
        );
      }
    }
  }

  Future<void> _goToPosition(int position) async {
    try {
      final deviceService = ref.read(deviceServiceProvider);
      await deviceService.moveFocuserTo(position);
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Focuser error: $e')),
        );
      }
    }
  }

  void _showGoToPositionDialog() {
    final focuserState = ref.read(focuserStateProvider);
    final maxPosition = focuserState.maxPosition ?? 50000;
    final currentPosition = focuserState.position ?? 0;

    showDialog(
      context: context,
      builder: (context) {
        final controller = TextEditingController(text: currentPosition.toString());
        return AlertDialog(
          title: const Text('Go To Position'),
          content: Column(
            mainAxisSize: MainAxisSize.min,
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Text('Enter position (0 - $maxPosition):'),
              const SizedBox(height: 12),
              TextField(
                controller: controller,
                keyboardType: TextInputType.number,
                autofocus: true,
                decoration: const InputDecoration(
                  hintText: 'Position',
                  border: OutlineInputBorder(),
                ),
              ),
            ],
          ),
          actions: [
            TextButton(
              onPressed: () => Navigator.of(context).pop(),
              child: const Text('Cancel'),
            ),
            ElevatedButton(
              onPressed: () {
                final position = int.tryParse(controller.text);
                if (position != null && position >= 0 && position <= maxPosition) {
                  Navigator.of(context).pop();
                  _goToPosition(position);
                } else {
                  ScaffoldMessenger.of(context).showSnackBar(
                    SnackBar(
                      content: Text('Invalid position. Must be between 0 and $maxPosition'),
                    ),
                  );
                }
              },
              child: const Text('Go'),
            ),
          ],
        );
      },
    );
  }

  Future<void> _runAutofocus() async {
    setState(() => _isRunningAutofocus = true);
    ref.read(sessionStateProvider.notifier).setAutofocusing(true);

    try {
      // Simulate autofocus run
      await Future.delayed(const Duration(seconds: 5));
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          const SnackBar(content: Text('Autofocus complete')),
        );
      }
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Autofocus failed: $e')),
        );
      }
    } finally {
      if (mounted) {
        setState(() => _isRunningAutofocus = false);
        ref.read(sessionStateProvider.notifier).setAutofocusing(false);
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    final focuserState = ref.watch(focuserStateProvider);
    final focusSettings = ref.watch(focusSettingsProvider);
    final isConnected =
        focuserState.connectionState == DeviceConnectionState.connected;
    final currentPosition = focuserState.position ?? 0;
    final maxPosition = focuserState.maxPosition ?? 50000;
    final temperature = focuserState.temperature;
    final isMoving = focuserState.isMoving;

    return SingleChildScrollView(
      padding: const EdgeInsets.all(16),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          // Connection status
          if (!isConnected)
            Container(
              padding: const EdgeInsets.all(12),
              margin: const EdgeInsets.only(bottom: 16),
              decoration: BoxDecoration(
                color: widget.colors.warning.withValues(alpha: 0.1),
                borderRadius: BorderRadius.circular(8),
                border: Border.all(
                    color: widget.colors.warning.withValues(alpha: 0.3)),
              ),
              child: Row(
                children: [
                  Icon(LucideIcons.alertCircle,
                      size: 16, color: widget.colors.warning),
                  const SizedBox(width: 8),
                  Expanded(
                    child: Text(
                      'No focuser connected',
                      style:
                          TextStyle(fontSize: 12, color: widget.colors.warning),
                    ),
                  ),
                ],
              ),
            ),

          // Manual Focus Section
          _PanelSection(
            title: 'Manual Focus',
            colors: widget.colors,
            child: Column(
              children: [
                // Position display
                Row(
                  mainAxisAlignment: MainAxisAlignment.spaceBetween,
                  children: [
                    Text('Position',
                        style: TextStyle(
                            fontSize: 12, color: widget.colors.textSecondary)),
                    Row(
                      children: [
                        Text(
                          isConnected ? '$currentPosition' : '---',
                          style: TextStyle(
                            fontSize: 18,
                            fontWeight: FontWeight.w600,
                            color: widget.colors.textPrimary,
                            fontFeatures: const [FontFeature.tabularFigures()],
                          ),
                        ),
                        Text(
                          isConnected ? ' / $maxPosition' : '',
                          style: TextStyle(
                              fontSize: 12, color: widget.colors.textMuted),
                        ),
                        if (isMoving)
                          Padding(
                            padding: const EdgeInsets.only(left: 8),
                            child: SizedBox(
                              width: 12,
                              height: 12,
                              child: CircularProgressIndicator(
                                strokeWidth: 2,
                                color: widget.colors.primary,
                              ),
                            ),
                          ),
                      ],
                    ),
                  ],
                ),
                if (temperature != null)
                  Padding(
                    padding: const EdgeInsets.only(top: 4),
                    child: Row(
                      mainAxisAlignment: MainAxisAlignment.spaceBetween,
                      children: [
                        Text('Temperature',
                            style: TextStyle(
                                fontSize: 12,
                                color: widget.colors.textSecondary)),
                        Text(
                          '${temperature.toStringAsFixed(1)}°C',
                          style: TextStyle(
                              fontSize: 12, color: widget.colors.textPrimary),
                        ),
                      ],
                    ),
                  ),
                const SizedBox(height: 16),

                // Movement buttons
                Row(
                  children: [
                    _FocusButton(
                      icon: LucideIcons.chevronsLeft,
                      label: '<<',
                      colors: widget.colors,
                      isEnabled: isConnected && !isMoving,
                      onTap: () => _moveFocuser(-10),
                    ),
                    const SizedBox(width: 4),
                    _FocusButton(
                      icon: LucideIcons.chevronLeft,
                      label: '<',
                      colors: widget.colors,
                      isEnabled: isConnected && !isMoving,
                      onTap: () => _moveFocuser(-1),
                    ),
                    const Spacer(),
                    _FocusButton(
                      icon: LucideIcons.chevronRight,
                      label: '>',
                      colors: widget.colors,
                      isEnabled: isConnected && !isMoving,
                      onTap: () => _moveFocuser(1),
                    ),
                    const SizedBox(width: 4),
                    _FocusButton(
                      icon: LucideIcons.chevronsRight,
                      label: '>>',
                      colors: widget.colors,
                      isEnabled: isConnected && !isMoving,
                      onTap: () => _moveFocuser(10),
                    ),
                  ],
                ),
                const SizedBox(height: 12),

                // Step size selector
                Row(
                  children: [
                    Text('Step Size:',
                        style: TextStyle(
                            fontSize: 11, color: widget.colors.textSecondary)),
                    const SizedBox(width: 8),
                    ...[10, 50, 100, 500].map((step) {
                      final isSelected = focusSettings.stepSize == step;
                      return Padding(
                        padding: const EdgeInsets.only(right: 6),
                        child: GestureDetector(
                          onTap: () => ref.read(focusSettingsProvider.notifier).state =
                              focusSettings.copyWith(stepSize: step),
                          child: Container(
                            padding: const EdgeInsets.symmetric(
                                horizontal: 8, vertical: 4),
                            decoration: BoxDecoration(
                              color: isSelected
                                  ? widget.colors.primary.withValues(alpha: 0.2)
                                  : widget.colors.background,
                              borderRadius: BorderRadius.circular(4),
                              border: Border.all(
                                color: isSelected
                                    ? widget.colors.primary
                                    : widget.colors.border,
                              ),
                            ),
                            child: Text(
                              '$step',
                              style: TextStyle(
                                fontSize: 10,
                                fontWeight: isSelected
                                    ? FontWeight.w600
                                    : FontWeight.normal,
                                color: isSelected
                                    ? widget.colors.primary
                                    : widget.colors.textSecondary,
                              ),
                            ),
                          ),
                        ),
                      );
                    }),
                  ],
                ),
                const SizedBox(height: 12),

                // Go to position button
                SizedBox(
                  width: double.infinity,
                  child: _SmallButton(
                    label: 'Go To Position...',
                    icon: LucideIcons.move,
                    colors: widget.colors,
                    isEnabled: isConnected && !isMoving,
                    onTap: _showGoToPositionDialog,
                  ),
                ),
              ],
            ),
          ),
          const SizedBox(height: 20),

          // Autofocus Section
          _PanelSection(
            title: 'Autofocus',
            colors: widget.colors,
            child: Column(
              children: [
                _DropdownRow(
                  label: 'Method',
                  value: focusSettings.method,
                  items: const ['V-Curve', 'Hyperbolic', 'Parabolic'],
                  colors: widget.colors,
                  onChanged: (value) {
                    if (value != null) {
                      ref.read(focusSettingsProvider.notifier).state =
                          focusSettings.copyWith(method: value);
                    }
                  },
                ),
                const SizedBox(height: 12),
                _InputRowEditable(
                  label: 'Step Size',
                  value: '${focusSettings.afStepSize}',
                  suffix: 'steps',
                  colors: widget.colors,
                  onChanged: (value) {
                    final parsed = int.tryParse(value);
                    if (parsed != null && parsed > 0) {
                      ref.read(focusSettingsProvider.notifier).state =
                          focusSettings.copyWith(afStepSize: parsed);
                    }
                  },
                ),
                const SizedBox(height: 12),
                _InputRowEditable(
                  label: 'Steps Out',
                  value: '${focusSettings.stepsOut}',
                  colors: widget.colors,
                  onChanged: (value) {
                    final parsed = int.tryParse(value);
                    if (parsed != null && parsed > 0) {
                      ref.read(focusSettingsProvider.notifier).state =
                          focusSettings.copyWith(stepsOut: parsed);
                    }
                  },
                ),
                const SizedBox(height: 12),
                _InputRowEditable(
                  label: 'Exposure',
                  value: focusSettings.exposureTime.toStringAsFixed(1),
                  suffix: 'sec',
                  colors: widget.colors,
                  onChanged: (value) {
                    final parsed = double.tryParse(value);
                    if (parsed != null && parsed > 0) {
                      ref.read(focusSettingsProvider.notifier).state =
                          focusSettings.copyWith(exposureTime: parsed);
                    }
                  },
                ),
                const SizedBox(height: 16),
                SizedBox(
                  width: double.infinity,
                  child: _SmallButton(
                    label: _isRunningAutofocus ? 'Running...' : 'Run Autofocus',
                    icon: _isRunningAutofocus
                        ? LucideIcons.loader2
                        : LucideIcons.focus,
                    colors: widget.colors,
                    isEnabled: isConnected && !_isRunningAutofocus,
                    onTap: _runAutofocus,
                  ),
                ),
              ],
            ),
          ),
        ],
      ),
    );
  }
}

class _GuidingPanel extends ConsumerStatefulWidget {
  final NightshadeColors colors;

  const _GuidingPanel({required this.colors});

  @override
  ConsumerState<_GuidingPanel> createState() => _GuidingPanelState();
}

class _GuidingPanelState extends ConsumerState<_GuidingPanel> {
  // UI-only transient state (doesn't need to persist)
  bool _isStartingGuiding = false;
  bool _isDithering = false;

  Future<void> _startGuiding() async {
    setState(() => _isStartingGuiding = true);
    final ditherSettings = ref.read(ditherSettingsProvider);
    try {
      final deviceService = ref.read(deviceServiceProvider);
      await deviceService.startGuiding(
        settlePixels: ditherSettings.settlePixels,
        settleTime: ditherSettings.settleTime,
      );
      ref.read(sessionStateProvider.notifier).setGuiding(true);
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Failed to start guiding: $e')),
        );
      }
    } finally {
      if (mounted) setState(() => _isStartingGuiding = false);
    }
  }

  Future<void> _stopGuiding() async {
    try {
      final deviceService = ref.read(deviceServiceProvider);
      await deviceService.stopGuiding();
      ref.read(sessionStateProvider.notifier).setGuiding(false);
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Failed to stop guiding: $e')),
        );
      }
    }
  }

  Future<void> _dither() async {
    setState(() => _isDithering = true);
    ref.read(sessionStateProvider.notifier).setDithering(true);
    final ditherSettings = ref.read(ditherSettingsProvider);
    try {
      final deviceService = ref.read(deviceServiceProvider);
      await deviceService.dither(
        amount: ditherSettings.ditherAmount,
        settlePixels: ditherSettings.settlePixels,
        settleTime: ditherSettings.settleTime,
      );
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Dither failed: $e')),
        );
      }
    } finally {
      if (mounted) {
        setState(() => _isDithering = false);
        ref.read(sessionStateProvider.notifier).setDithering(false);
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    final guiderState = ref.watch(guiderStateProvider);
    final ditherSettings = ref.watch(ditherSettingsProvider);
    final isConnected =
        guiderState.connectionState == DeviceConnectionState.connected;
    final isGuiding = guiderState.isGuiding;

    final rmsRa = guiderState.rmsRa?.toStringAsFixed(2) ?? '---';
    final rmsDec = guiderState.rmsDec?.toStringAsFixed(2) ?? '---';
    final rmsTotal = guiderState.rmsTotal?.toStringAsFixed(2) ?? '---';

    return SingleChildScrollView(
      padding: const EdgeInsets.all(16),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          // Connection status
          if (!isConnected)
            Container(
              padding: const EdgeInsets.all(12),
              margin: const EdgeInsets.only(bottom: 16),
              decoration: BoxDecoration(
                color: widget.colors.warning.withValues(alpha: 0.1),
                borderRadius: BorderRadius.circular(8),
                border: Border.all(
                    color: widget.colors.warning.withValues(alpha: 0.3)),
              ),
              child: Row(
                children: [
                  Icon(LucideIcons.alertCircle,
                      size: 16, color: widget.colors.warning),
                  const SizedBox(width: 8),
                  Expanded(
                    child: Text(
                      'No guider connected (PHD2)',
                      style:
                          TextStyle(fontSize: 12, color: widget.colors.warning),
                    ),
                  ),
                ],
              ),
            ),

          // Guiding graph placeholder
          Container(
            height: 120,
            decoration: BoxDecoration(
              color: widget.colors.surfaceAlt,
              borderRadius: BorderRadius.circular(10),
              border: Border.all(color: widget.colors.border),
            ),
            child: Stack(
              children: [
                if (isGuiding)
                  Positioned.fill(
                    child: CustomPaint(
                      painter: _GuidingGraphPainter(colors: widget.colors),
                    ),
                  ),
                Center(
                  child: Column(
                    mainAxisSize: MainAxisSize.min,
                    children: [
                      Icon(
                        isGuiding
                            ? LucideIcons.activity
                            : LucideIcons.crosshair,
                        size: 24,
                        color: isGuiding
                            ? widget.colors.success
                            : widget.colors.textMuted,
                      ),
                      const SizedBox(height: 4),
                      Text(
                        isGuiding
                            ? 'Guiding Active'
                            : isConnected
                                ? 'Ready to guide'
                                : 'Connect PHD2',
                        style: TextStyle(
                          fontSize: 11,
                          color: isGuiding
                              ? widget.colors.success
                              : widget.colors.textMuted,
                        ),
                      ),
                    ],
                  ),
                ),
                // Legend
                Positioned(
                  bottom: 8,
                  left: 8,
                  child: Row(
                    children: [
                      Container(width: 12, height: 2, color: Colors.red),
                      const SizedBox(width: 4),
                      Text('RA',
                          style: TextStyle(
                              fontSize: 9, color: widget.colors.textMuted)),
                      const SizedBox(width: 12),
                      Container(width: 12, height: 2, color: Colors.blue),
                      const SizedBox(width: 4),
                      Text('Dec',
                          style: TextStyle(
                              fontSize: 9, color: widget.colors.textMuted)),
                    ],
                  ),
                ),
              ],
            ),
          ),
          const SizedBox(height: 16),

          // RMS Stats
          Row(
            children: [
              _GuideStat(
                  label: 'RA RMS', value: '$rmsRa"', colors: widget.colors),
              _GuideStat(
                  label: 'Dec RMS', value: '$rmsDec"', colors: widget.colors),
              _GuideStat(
                  label: 'Total', value: '$rmsTotal"', colors: widget.colors),
            ],
          ),
          const SizedBox(height: 20),

          // Control Section
          _PanelSection(
            title: 'Control',
            colors: widget.colors,
            child: Column(
              children: [
                Row(
                  children: [
                    Expanded(
                      child: _SmallButton(
                        label: _isStartingGuiding
                            ? 'Starting...'
                            : isGuiding
                                ? 'Guiding'
                                : 'Start',
                        icon:
                            isGuiding ? LucideIcons.activity : LucideIcons.play,
                        colors: widget.colors,
                        isEnabled:
                            isConnected && !isGuiding && !_isStartingGuiding,
                        onTap: _startGuiding,
                      ),
                    ),
                    const SizedBox(width: 8),
                    Expanded(
                      child: _SmallButton(
                        label: 'Stop',
                        icon: LucideIcons.square,
                        isOutline: true,
                        colors: widget.colors,
                        isEnabled: isConnected && isGuiding,
                        onTap: _stopGuiding,
                      ),
                    ),
                  ],
                ),
                const SizedBox(height: 12),
                SizedBox(
                  width: double.infinity,
                  child: _SmallButton(
                    label: _isDithering ? 'Dithering...' : 'Dither',
                    icon: _isDithering
                        ? LucideIcons.loader2
                        : LucideIcons.shuffle,
                    isOutline: true,
                    colors: widget.colors,
                    isEnabled: isConnected && isGuiding && !_isDithering,
                    onTap: _dither,
                  ),
                ),
              ],
            ),
          ),
          const SizedBox(height: 20),

          // Dithering Settings
          _PanelSection(
            title: 'Dither Settings',
            colors: widget.colors,
            child: Column(
              children: [
                _SliderRowInteractive(
                  label: 'Amount',
                  value: ditherSettings.ditherAmount,
                  min: 1,
                  max: 20,
                  suffix: 'px',
                  colors: widget.colors,
                  onChanged: (value) => ref.read(ditherSettingsProvider.notifier).state =
                      ditherSettings.copyWith(ditherAmount: value),
                ),
                const SizedBox(height: 12),
                _SliderRowInteractive(
                  label: 'Settle Threshold',
                  value: ditherSettings.settlePixels,
                  min: 0.3,
                  max: 3.0,
                  suffix: '"',
                  colors: widget.colors,
                  onChanged: (value) => ref.read(ditherSettingsProvider.notifier).state =
                      ditherSettings.copyWith(settlePixels: value),
                ),
                const SizedBox(height: 12),
                _SliderRowInteractive(
                  label: 'Settle Time',
                  value: ditherSettings.settleTime,
                  min: 5,
                  max: 30,
                  suffix: 's',
                  colors: widget.colors,
                  onChanged: (value) => ref.read(ditherSettingsProvider.notifier).state =
                      ditherSettings.copyWith(settleTime: value),
                ),
              ],
            ),
          ),
        ],
      ),
    );
  }
}

class _GuidingGraphPainter extends CustomPainter {
  final NightshadeColors colors;

  _GuidingGraphPainter({required this.colors});

  @override
  void paint(Canvas canvas, Size size) {
    // Draw a simple placeholder guiding graph
    final random = math.Random(42);

    final raPaint = Paint()
      ..color = Colors.red.withValues(alpha: 0.7)
      ..strokeWidth = 1
      ..style = PaintingStyle.stroke;

    final decPaint = Paint()
      ..color = Colors.blue.withValues(alpha: 0.7)
      ..strokeWidth = 1
      ..style = PaintingStyle.stroke;

    final centerY = size.height / 2;

    // Draw RA line
    final raPath = Path();
    raPath.moveTo(0, centerY + (random.nextDouble() - 0.5) * 20);
    for (double x = 1; x < size.width; x += 2) {
      raPath.lineTo(x, centerY + (random.nextDouble() - 0.5) * 20);
    }
    canvas.drawPath(raPath, raPaint);

    // Draw Dec line
    final decPath = Path();
    decPath.moveTo(0, centerY + (random.nextDouble() - 0.5) * 20);
    for (double x = 1; x < size.width; x += 2) {
      decPath.lineTo(x, centerY + (random.nextDouble() - 0.5) * 20);
    }
    canvas.drawPath(decPath, decPaint);

    // Draw center line
    final centerPaint = Paint()
      ..color = colors.border
      ..strokeWidth = 0.5;
    canvas.drawLine(
        Offset(0, centerY), Offset(size.width, centerY), centerPaint);
  }

  @override
  bool shouldRepaint(covariant CustomPainter oldDelegate) => false;
}

class _GuideStat extends StatelessWidget {
  final String label;
  final String value;
  final NightshadeColors colors;

  const _GuideStat({
    required this.label,
    required this.value,
    required this.colors,
  });

  @override
  Widget build(BuildContext context) {
    return Expanded(
      child: Column(
        children: [
          Text(
            value,
            style: TextStyle(
              fontSize: 14,
              fontWeight: FontWeight.w600,
              color: colors.textPrimary,
            ),
          ),
          const SizedBox(height: 2),
          Text(
            label,
            style: TextStyle(
              fontSize: 10,
              color: colors.textMuted,
            ),
          ),
        ],
      ),
    );
  }
}

class _PanelSection extends StatelessWidget {
  final String title;
  final Widget child;
  final NightshadeColors colors;

  const _PanelSection({
    required this.title,
    required this.child,
    required this.colors,
  });

  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        Text(
          title,
          style: TextStyle(
            fontSize: 12,
            fontWeight: FontWeight.w600,
            color: colors.textPrimary,
          ),
        ),
        const SizedBox(height: 12),
        Container(
          padding: const EdgeInsets.all(14),
          decoration: BoxDecoration(
            color: colors.surfaceAlt,
            borderRadius: BorderRadius.circular(10),
            border: Border.all(color: colors.border),
          ),
          child: child,
        ),
      ],
    );
  }
}

class _InputRow extends StatelessWidget {
  final String label;
  final String? value;
  final String? suffix;
  final bool isDropdown;
  final NightshadeColors colors;
  final Widget? trailing;

  const _InputRow({
    required this.label,
    this.value,
    this.suffix,
    this.isDropdown = false,
    required this.colors,
    this.trailing,
  });

  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        Expanded(
          flex: 2,
          child: Text(
            label,
            style: TextStyle(
              fontSize: 12,
              color: colors.textSecondary,
            ),
          ),
        ),
        Expanded(
          flex: 3,
          child: Container(
            padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 8),
            decoration: BoxDecoration(
              color: colors.background,
              borderRadius: BorderRadius.circular(6),
              border: Border.all(color: colors.border),
            ),
            child: Row(
              children: [
                Expanded(
                  child: Text(
                    value ?? '',
                    style: TextStyle(
                      fontSize: 12,
                      color: colors.textPrimary,
                    ),
                  ),
                ),
                if (suffix != null)
                  Text(
                    suffix!,
                    style: TextStyle(
                      fontSize: 10,
                      color: colors.textMuted,
                    ),
                  ),
                if (isDropdown)
                  Icon(
                    LucideIcons.chevronDown,
                    size: 12,
                    color: colors.textMuted,
                  ),
                if (trailing != null) ...[
                  const SizedBox(width: 8),
                  trailing!,
                ],
              ],
            ),
          ),
        ),
      ],
    );
  }
}

class _InputRowEditable extends StatelessWidget {
  final String label;
  final String value;
  final String? suffix;
  final NightshadeColors colors;
  final ValueChanged<String> onChanged;

  const _InputRowEditable({
    required this.label,
    required this.value,
    this.suffix,
    required this.colors,
    required this.onChanged,
  });

  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        Expanded(
          flex: 2,
          child: Text(
            label,
            style: TextStyle(
              fontSize: 12,
              color: colors.textSecondary,
            ),
          ),
        ),
        Expanded(
          flex: 3,
          child: Container(
            decoration: BoxDecoration(
              color: colors.background,
              borderRadius: BorderRadius.circular(6),
              border: Border.all(color: colors.border),
            ),
            child: TextField(
              controller: TextEditingController(text: value),
              style: TextStyle(
                fontSize: 12,
                color: colors.textPrimary,
              ),
              decoration: InputDecoration(
                contentPadding:
                    const EdgeInsets.symmetric(horizontal: 10, vertical: 8),
                border: InputBorder.none,
                isDense: true,
                suffixText: suffix,
                suffixStyle: TextStyle(
                  fontSize: 10,
                  color: colors.textMuted,
                ),
              ),
              onSubmitted: onChanged,
              onChanged: onChanged,
            ),
          ),
        ),
      ],
    );
  }
}

class _DropdownRow extends StatelessWidget {
  final String label;
  final String? value;
  final List<String> items;
  final NightshadeColors colors;
  final ValueChanged<String?>? onChanged;

  const _DropdownRow({
    required this.label,
    this.value,
    required this.items,
    required this.colors,
    this.onChanged,
  });

  @override
  Widget build(BuildContext context) {
    final isEnabled = onChanged != null;

    return Row(
      children: [
        Expanded(
          flex: 2,
          child: Text(
            label,
            style: TextStyle(
              fontSize: 12,
              color: isEnabled ? colors.textSecondary : colors.textMuted,
            ),
          ),
        ),
        Expanded(
          flex: 3,
          child: Container(
            padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 4),
            decoration: BoxDecoration(
              color: isEnabled ? colors.background : colors.surfaceAlt,
              borderRadius: BorderRadius.circular(6),
              border: Border.all(color: colors.border),
            ),
            child: DropdownButtonHideUnderline(
              child: DropdownButton<String>(
                value: items.contains(value) ? value : null,
                isExpanded: true,
                isDense: true,
                icon: Icon(
                  LucideIcons.chevronDown,
                  size: 14,
                  color: colors.textMuted,
                ),
                dropdownColor: colors.surface,
                style: TextStyle(
                  fontSize: 12,
                  color: isEnabled ? colors.textPrimary : colors.textMuted,
                ),
                items: items.map((item) {
                  return DropdownMenuItem<String>(
                    value: item,
                    child: Text(item),
                  );
                }).toList(),
                onChanged: onChanged,
              ),
            ),
          ),
        ),
      ],
    );
  }
}

// TODO: Use this widget for slider rows
// ignore: unused_element
class _SliderRow extends StatelessWidget {
  final String label;
  final double value;
  final double min;
  final double max;
  final String? suffix;
  final NightshadeColors colors;

  const _SliderRow({
    required this.label,
    required this.value,
    required this.min,
    required this.max,
    this.suffix,
    required this.colors,
  });

  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        Row(
          mainAxisAlignment: MainAxisAlignment.spaceBetween,
          children: [
            Text(
              label,
              style: TextStyle(
                fontSize: 12,
                color: colors.textSecondary,
              ),
            ),
            Text(
              '${value.toInt()}${suffix ?? ''}',
              style: TextStyle(
                fontSize: 12,
                fontWeight: FontWeight.w600,
                color: colors.textPrimary,
              ),
            ),
          ],
        ),
        const SizedBox(height: 8),
        SliderTheme(
          data: SliderThemeData(
            trackHeight: 4,
            activeTrackColor: colors.primary,
            inactiveTrackColor: colors.border,
            thumbColor: colors.primary,
            overlayColor: colors.primary.withValues(alpha: 0.1),
          ),
          child: Slider(
            value: value,
            min: min,
            max: max,
            onChanged: (_) {},
          ),
        ),
      ],
    );
  }
}

class _SliderRowInteractive extends StatelessWidget {
  final String label;
  final double value;
  final double min;
  final double max;
  final String suffix;
  final NightshadeColors colors;
  final ValueChanged<double>? onChanged;

  const _SliderRowInteractive({
    required this.label,
    required this.value,
    required this.min,
    required this.max,
    required this.suffix,
    required this.colors,
    this.onChanged,
  });

  @override
  Widget build(BuildContext context) {
    final isEnabled = onChanged != null;

    return Row(
      children: [
        Expanded(
          flex: 2,
          child: Text(
            label,
            style: TextStyle(
              fontSize: 11,
              color: isEnabled ? colors.textSecondary : colors.textMuted,
            ),
          ),
        ),
        Expanded(
          flex: 3,
          child: SliderTheme(
            data: SliderThemeData(
              trackHeight: 2,
              thumbShape: const RoundSliderThumbShape(enabledThumbRadius: 5),
              overlayShape: const RoundSliderOverlayShape(overlayRadius: 10),
              activeTrackColor: isEnabled ? colors.primary : colors.textMuted,
              inactiveTrackColor: colors.border,
              thumbColor: isEnabled ? colors.primary : colors.textMuted,
              overlayColor: colors.primary.withValues(alpha: 0.2),
            ),
            child: Slider(
              value: value.clamp(min, max),
              min: min,
              max: max,
              onChanged: onChanged,
            ),
          ),
        ),
        SizedBox(
          width: 45,
          child: Text(
            '${value.toStringAsFixed(1)}$suffix',
            textAlign: TextAlign.right,
            style: TextStyle(
              fontSize: 11,
              fontFeatures: const [FontFeature.tabularFigures()],
              color: isEnabled ? colors.textPrimary : colors.textMuted,
            ),
          ),
        ),
      ],
    );
  }
}

class _SmallButton extends StatefulWidget {
  final String label;
  final IconData icon;
  final bool isOutline;
  final bool isEnabled;
  final NightshadeColors colors;
  final VoidCallback? onTap;

  const _SmallButton({
    required this.label,
    required this.icon,
    this.isOutline = false,
    this.isEnabled = true,
    required this.colors,
    this.onTap,
  });

  @override
  State<_SmallButton> createState() => _SmallButtonState();
}

class _SmallButtonState extends State<_SmallButton> {
  bool _isHovered = false;

  @override
  Widget build(BuildContext context) {
    final isEnabled = widget.isEnabled;
    final primaryColor =
        isEnabled ? widget.colors.primary : widget.colors.textMuted;

    return MouseRegion(
      onEnter: (_) => setState(() => _isHovered = true),
      onExit: (_) => setState(() => _isHovered = false),
      child: GestureDetector(
        onTap: isEnabled ? widget.onTap : null,
        child: AnimatedContainer(
          duration: const Duration(milliseconds: 150),
          padding: const EdgeInsets.symmetric(vertical: 10, horizontal: 14),
          decoration: BoxDecoration(
            color: widget.isOutline
                ? _isHovered && isEnabled
                    ? primaryColor.withValues(alpha: 0.1)
                    : Colors.transparent
                : isEnabled
                    ? primaryColor
                    : widget.colors.surfaceAlt,
            borderRadius: BorderRadius.circular(8),
            border: Border.all(
              color: primaryColor,
            ),
          ),
          child: Row(
            mainAxisAlignment: MainAxisAlignment.center,
            mainAxisSize: MainAxisSize.min,
            children: [
              Icon(
                widget.icon,
                size: 14,
                color: widget.isOutline
                    ? primaryColor
                    : isEnabled
                        ? Colors.white
                        : widget.colors.textMuted,
              ),
              const SizedBox(width: 6),
              Flexible(
                child: Text(
                  widget.label,
                  style: TextStyle(
                    fontSize: 12,
                    fontWeight: FontWeight.w500,
                    color: widget.isOutline
                        ? primaryColor
                        : isEnabled
                            ? Colors.white
                            : widget.colors.textMuted,
                  ),
                  overflow: TextOverflow.ellipsis,
                  maxLines: 1,
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}

class _FocusButton extends StatefulWidget {
  final IconData icon;
  final String label;
  final NightshadeColors colors;
  final VoidCallback? onTap;
  final bool isEnabled;

  const _FocusButton({
    required this.icon,
    required this.label,
    required this.colors,
    this.onTap,
    this.isEnabled = true,
  });

  @override
  State<_FocusButton> createState() => _FocusButtonState();
}

class _FocusButtonState extends State<_FocusButton> {
  bool _isHovered = false;

  @override
  Widget build(BuildContext context) {
    final isEnabled = widget.isEnabled;

    return MouseRegion(
      onEnter: (_) => setState(() => _isHovered = true),
      onExit: (_) => setState(() => _isHovered = false),
      child: GestureDetector(
        onTap: isEnabled ? widget.onTap : null,
        child: AnimatedContainer(
          duration: const Duration(milliseconds: 150),
          width: 44,
          height: 36,
          decoration: BoxDecoration(
            color: _isHovered && isEnabled
                ? widget.colors.primary.withValues(alpha: 0.1)
                : widget.colors.background,
            borderRadius: BorderRadius.circular(6),
            border: Border.all(
              color: _isHovered && isEnabled
                  ? widget.colors.primary
                  : widget.colors.border,
            ),
          ),
          child: Icon(
            widget.icon,
            size: 16,
            color: !isEnabled
                ? widget.colors.textMuted
                : _isHovered
                    ? widget.colors.primary
                    : widget.colors.textSecondary,
          ),
        ),
      ),
    );
  }
}

// =============================================================================
// DEBAYERING CARD
// =============================================================================

class _DebayeringCard extends ConsumerWidget {
  const _DebayeringCard();

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final colors = Theme.of(context).extension<NightshadeColors>()!;
    final debayerEnabled = ref.watch(debayerEnabledProvider);
    final bayerPattern = ref.watch(bayerPatternProvider);
    final debayerAlgorithm = ref.watch(debayerAlgorithmProvider);

    return _PanelSection(
      title: 'Debayering',
      colors: colors,
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              Text(
                'Enable Debayering',
                style: TextStyle(
                  fontSize: 12,
                  color: colors.textSecondary,
                ),
              ),
              Switch(
                value: debayerEnabled,
                onChanged: (value) {
                  ref.read(debayerEnabledProvider.notifier).state = value;
                },
                activeThumbColor: colors.primary,
              ),
            ],
          ),
          const SizedBox(height: 8),
          Text(
            'Enable for color cameras to convert raw Bayer data to RGB',
            style: TextStyle(fontSize: 10, color: colors.textMuted),
          ),
          const SizedBox(height: 16),

          // Algorithm selection
          _DropdownRow(
            label: 'Algorithm',
            value: debayerAlgorithm.displayName,
            items: DebayerAlgorithm.values.map((a) => a.displayName).toList(),
            colors: colors,
            onChanged: debayerEnabled
                ? (value) {
                    if (value != null) {
                      final algorithm = DebayerAlgorithm.values.firstWhere(
                        (a) => a.displayName == value,
                        orElse: () => DebayerAlgorithm.bilinear,
                      );
                      ref.read(debayerAlgorithmProvider.notifier).state =
                          algorithm;
                    }
                  }
                : null,
          ),
          const SizedBox(height: 12),

          // Bayer pattern selection
          _DropdownRow(
            label: 'Pattern',
            value: bayerPattern.displayName,
            items: BayerPattern.values.map((p) => p.displayName).toList(),
            colors: colors,
            onChanged: debayerEnabled
                ? (value) {
                    if (value != null) {
                      final pattern = BayerPattern.values.firstWhere(
                        (p) => p.displayName == value,
                        orElse: () => BayerPattern.rggb,
                      );
                      ref.read(bayerPatternProvider.notifier).state = pattern;
                    }
                  }
                : null,
          ),
          const SizedBox(height: 12),

          // Auto-detect option
          Consumer(
            builder: (context, ref, _) {
              final autoDetect = ref.watch(autoDetectBayerPatternProvider);
              return Row(
                children: [
                  Checkbox(
                    value: autoDetect,
                    onChanged: debayerEnabled
                        ? (v) {
                            ref.read(autoDetectBayerPatternProvider.notifier).state = v ?? false;
                          }
                        : null,
                    fillColor: WidgetStateProperty.all(colors.primary),
                    side: BorderSide(color: colors.border),
                  ),
                  const SizedBox(width: 8),
                  Expanded(
                    child: Text(
                      'Auto-detect from FITS header',
                      style: TextStyle(
                        fontSize: 12,
                        color: debayerEnabled
                            ? colors.textSecondary
                            : colors.textMuted,
                      ),
                    ),
                  ),
                ],
              );
            },
          ),
        ],
      ),
    );
  }
}

/// Wrapper widget for annotation overlay with object info popup
class _AnnotationOverlayWrapper extends ConsumerStatefulWidget {
  final double zoomLevel;
  final Offset panOffset;
  final Size imageSize;
  final NightshadeColors colors;

  const _AnnotationOverlayWrapper({
    required this.zoomLevel,
    required this.panOffset,
    required this.imageSize,
    required this.colors,
  });

  @override
  ConsumerState<_AnnotationOverlayWrapper> createState() =>
      _AnnotationOverlayWrapperState();
}

class _AnnotationOverlayWrapperState
    extends ConsumerState<_AnnotationOverlayWrapper> {
  CelestialObjectAnnotation? _selectedObject;
  Offset? _tooltipPosition;

  void _onObjectTapped(CelestialObjectAnnotation object) {
    setState(() {
      _selectedObject = object;
      // Position tooltip near the object
      _tooltipPosition = Offset(
        object.x * widget.zoomLevel + widget.panOffset.dx + 20,
        object.y * widget.zoomLevel + widget.panOffset.dy,
      );
    });
  }

  void _onIdentifyAt(double x, double y) async {
    // Use annotation service to identify object at position
    final annotationService = ref.read(annotationServiceProvider);
    final annotation = ref.read(currentAnnotationProvider);

    if (annotation?.plateSolve == null) return;

    final result = await annotationService.identifyAtPixel(
      plateSolve: annotation!.plateSolve,
      x: x,
      y: y,
    );

    if (result != null && mounted) {
      setState(() {
        _selectedObject = result;
        _tooltipPosition = Offset(
          x * widget.zoomLevel + widget.panOffset.dx + 20,
          y * widget.zoomLevel + widget.panOffset.dy,
        );
      });
    }
  }

  void _closeTooltip() {
    setState(() {
      _selectedObject = null;
      _tooltipPosition = null;
    });
  }

  void _showMoreInfo() {
    if (_selectedObject == null) return;
    final obj = _selectedObject!;

    showDialog(
      context: context,
      builder: (context) => AlertDialog(
        title: Text(obj.name),
        content: SingleChildScrollView(
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            mainAxisSize: MainAxisSize.min,
            children: [
              if (obj.commonName != null) ...[
                Text('Common Name: ${obj.commonName}', style: const TextStyle(fontSize: 14)),
                const SizedBox(height: 8),
              ],
              Text('Type: ${obj.type.toString().split('.').last}', style: const TextStyle(fontSize: 14)),
              const SizedBox(height: 8),
              Text('RA: ${obj.ra.toStringAsFixed(6)}°', style: const TextStyle(fontSize: 14)),
              const SizedBox(height: 8),
              Text('Dec: ${obj.dec.toStringAsFixed(6)}°', style: const TextStyle(fontSize: 14)),
              if (obj.magnitude != null) ...[
                const SizedBox(height: 8),
                Text('Magnitude: ${obj.magnitude!.toStringAsFixed(2)}', style: const TextStyle(fontSize: 14)),
              ],
              if (obj.size != null) ...[
                const SizedBox(height: 8),
                Text('Size: ${obj.size!.toStringAsFixed(2)}\'', style: const TextStyle(fontSize: 14)),
              ],
            ],
          ),
        ),
        actions: [
          TextButton(
            onPressed: () => Navigator.of(context).pop(),
            child: const Text('Close'),
          ),
        ],
      ),
    );

    _closeTooltip();
  }

  @override
  Widget build(BuildContext context) {
    final annotation = ref.watch(currentAnnotationProvider);

    return Stack(
      children: [
        AnnotationOverlay(
          annotation: annotation,
          zoomLevel: widget.zoomLevel,
          panOffset: widget.panOffset,
          imageSize: widget.imageSize,
          onObjectTapped: _onObjectTapped,
          onIdentifyAt: _onIdentifyAt,
        ),
        // Object info tooltip
        if (_selectedObject != null && _tooltipPosition != null)
          Positioned(
            left: _tooltipPosition!.dx
                .clamp(0, MediaQuery.of(context).size.width - 300),
            top: _tooltipPosition!.dy
                .clamp(0, MediaQuery.of(context).size.height - 200),
            child: ObjectInfoTooltip(
              object: _selectedObject!,
              onClose: _closeTooltip,
              onMoreInfo: _showMoreInfo,
            ),
          ),
      ],
    );
  }
}

/// Banner shown when annotation catalog is not installed
class _AnnotationCatalogBanner extends StatelessWidget {
  final NightshadeColors colors;
  final VoidCallback onDismiss;
  final VoidCallback onSetup;

  const _AnnotationCatalogBanner({
    required this.colors,
    required this.onDismiss,
    required this.onSetup,
  });

  @override
  Widget build(BuildContext context) {
    return Container(
      width: double.infinity,
      padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 10),
      decoration: BoxDecoration(
        color: colors.primary.withValues(alpha: 0.15),
        border: Border(
          bottom: BorderSide(color: colors.primary.withValues(alpha: 0.3)),
        ),
      ),
      child: Row(
        children: [
          Icon(LucideIcons.info, size: 16, color: colors.primary),
          const SizedBox(width: 12),
          Expanded(
            child: Text(
              'Annotations are enabled but no catalog is installed. Download the annotation catalog to identify objects in your images.',
              style: TextStyle(
                color: colors.textPrimary,
                fontSize: 12,
              ),
            ),
          ),
          const SizedBox(width: 16),
          TextButton(
            onPressed: onSetup,
            style: TextButton.styleFrom(
              foregroundColor: colors.primary,
              padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 6),
            ),
            child: const Text('Setup'),
          ),
          IconButton(
            icon: Icon(LucideIcons.x, size: 16, color: colors.textMuted),
            onPressed: onDismiss,
            tooltip: 'Dismiss',
            padding: EdgeInsets.zero,
            constraints: const BoxConstraints(minWidth: 32, minHeight: 32),
          ),
        ],
      ),
    );
  }
}

/// Status indicator for the live annotation pipeline
class _AnnotationStatusIndicator extends ConsumerWidget {
  final NightshadeColors colors;

  const _AnnotationStatusIndicator({required this.colors});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final annotationState = ref.watch(annotationStateProvider);
    final annotationSettings = ref.watch(annotationSettingsProvider).valueOrNull;

    // Don't show anything if annotations are disabled
    if (annotationSettings != null && !annotationSettings.enabled) {
      return const SizedBox.shrink();
    }

    // Don't show idle state (reduces visual clutter)
    if (annotationState.status == AnnotationStatus.idle) {
      return const SizedBox.shrink();
    }

    return AnimatedOpacity(
      opacity: 1.0,
      duration: const Duration(milliseconds: 200),
      child: Container(
        padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 6),
        decoration: BoxDecoration(
          color: _getBackgroundColor(annotationState.status),
          borderRadius: BorderRadius.circular(6),
          border: Border.all(
            color: _getBorderColor(annotationState.status),
          ),
        ),
        child: Row(
          mainAxisSize: MainAxisSize.min,
          children: [
            _getStatusIcon(annotationState.status),
            const SizedBox(width: 8),
            Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              mainAxisSize: MainAxisSize.min,
              children: [
                Text(
                  annotationState.message ?? _getStatusText(annotationState.status),
                  style: TextStyle(
                    color: _getTextColor(annotationState.status),
                    fontSize: 11,
                    fontWeight: FontWeight.w500,
                  ),
                ),
                if (annotationState.errorDetails != null)
                  Text(
                    annotationState.errorDetails!,
                    style: TextStyle(
                      color: _getTextColor(annotationState.status).withValues(alpha: 0.7),
                      fontSize: 10,
                    ),
                    maxLines: 1,
                    overflow: TextOverflow.ellipsis,
                  ),
              ],
            ),
          ],
        ),
      ),
    );
  }

  Color _getBackgroundColor(AnnotationStatus status) {
    switch (status) {
      case AnnotationStatus.checkingCatalogs:
      case AnnotationStatus.plateSolving:
      case AnnotationStatus.searchingCatalogs:
        return const Color(0xFF1E3A5F).withValues(alpha: 0.9); // Blue for processing
      case AnnotationStatus.complete:
        return const Color(0xFF1E4620).withValues(alpha: 0.9); // Green for success
      case AnnotationStatus.error:
      case AnnotationStatus.plateSolveFailed:
        return const Color(0xFF5F1E1E).withValues(alpha: 0.9); // Red for error
      case AnnotationStatus.catalogsNotInstalled:
        return const Color(0xFF5F4D1E).withValues(alpha: 0.9); // Orange for warning
      case AnnotationStatus.idle:
        return Colors.transparent;
    }
  }

  Color _getBorderColor(AnnotationStatus status) {
    switch (status) {
      case AnnotationStatus.checkingCatalogs:
      case AnnotationStatus.plateSolving:
      case AnnotationStatus.searchingCatalogs:
        return const Color(0xFF3B82F6).withValues(alpha: 0.5);
      case AnnotationStatus.complete:
        return const Color(0xFF22C55E).withValues(alpha: 0.5);
      case AnnotationStatus.error:
      case AnnotationStatus.plateSolveFailed:
        return const Color(0xFFEF4444).withValues(alpha: 0.5);
      case AnnotationStatus.catalogsNotInstalled:
        return const Color(0xFFF59E0B).withValues(alpha: 0.5);
      case AnnotationStatus.idle:
        return Colors.transparent;
    }
  }

  Color _getTextColor(AnnotationStatus status) {
    switch (status) {
      case AnnotationStatus.checkingCatalogs:
      case AnnotationStatus.plateSolving:
      case AnnotationStatus.searchingCatalogs:
        return const Color(0xFF93C5FD);
      case AnnotationStatus.complete:
        return const Color(0xFF86EFAC);
      case AnnotationStatus.error:
      case AnnotationStatus.plateSolveFailed:
        return const Color(0xFFFCA5A5);
      case AnnotationStatus.catalogsNotInstalled:
        return const Color(0xFFFCD34D);
      case AnnotationStatus.idle:
        return Colors.white70;
    }
  }

  Widget _getStatusIcon(AnnotationStatus status) {
    switch (status) {
      case AnnotationStatus.checkingCatalogs:
      case AnnotationStatus.plateSolving:
      case AnnotationStatus.searchingCatalogs:
        return SizedBox(
          width: 14,
          height: 14,
          child: CircularProgressIndicator(
            strokeWidth: 2,
            valueColor: AlwaysStoppedAnimation<Color>(_getTextColor(status)),
          ),
        );
      case AnnotationStatus.complete:
        return Icon(LucideIcons.checkCircle, size: 14, color: _getTextColor(status));
      case AnnotationStatus.error:
      case AnnotationStatus.plateSolveFailed:
        return Icon(LucideIcons.alertCircle, size: 14, color: _getTextColor(status));
      case AnnotationStatus.catalogsNotInstalled:
        return Icon(LucideIcons.alertTriangle, size: 14, color: _getTextColor(status));
      case AnnotationStatus.idle:
        return const SizedBox.shrink();
    }
  }

  String _getStatusText(AnnotationStatus status) {
    switch (status) {
      case AnnotationStatus.checkingCatalogs:
        return 'Checking catalogs...';
      case AnnotationStatus.plateSolving:
        return 'Plate solving...';
      case AnnotationStatus.searchingCatalogs:
        return 'Searching catalogs...';
      case AnnotationStatus.complete:
        return 'Annotation complete';
      case AnnotationStatus.error:
        return 'Annotation error';
      case AnnotationStatus.plateSolveFailed:
        return 'Plate solve failed';
      case AnnotationStatus.catalogsNotInstalled:
        return 'No catalogs installed';
      case AnnotationStatus.idle:
        return '';
    }
  }
}
