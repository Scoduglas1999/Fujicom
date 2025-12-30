import 'dart:async';
import 'dart:ui' as ui;
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';
import 'package:lucide_icons/lucide_icons.dart';
import 'package:nightshade_ui/nightshade_ui.dart';
import 'package:nightshade_planetarium/nightshade_planetarium.dart';
import 'package:nightshade_core/nightshade_core.dart';
import 'package:intl/intl.dart';
import 'widgets/filter_sidebar.dart';

/// Get display name and catalog tag for a DSO
/// Returns (displayName, catalogTag)
(String, String) getDsoDisplayInfo(DeepSkyObject dso) {
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

class PlanetariumScreen extends ConsumerStatefulWidget {
  const PlanetariumScreen({super.key});

  @override
  ConsumerState<PlanetariumScreen> createState() => _PlanetariumScreenState();
}

class _PlanetariumScreenState extends ConsumerState<PlanetariumScreen>
    with SingleTickerProviderStateMixin {
  final _searchController = TextEditingController();

  // Popup state
  bool _showPopup = false;
  Offset _popupPosition = Offset.zero;
  CelestialObject? _popupObject;
  CelestialCoordinate? _popupCoordinates;
  final GlobalKey _skyViewKey = GlobalKey();

  // Slew mode state
  bool _slewMode = false;

  // FOV overlay state
  bool _showFOV = false;

  // Track if initial sync has been done
  bool _initialSyncDone = false;

  // Filter sidebar state
  bool _filterSidebarExpanded = false;

  @override
  void initState() {
    super.initState();
    // Do initial sync after first frame
    WidgetsBinding.instance.addPostFrameCallback((_) {
      if (!mounted) return;
      _performInitialSync();
    });
  }
  
  @override
  void dispose() {
    _searchController.dispose();
    super.dispose();
  }
  
  void _performInitialSync() {
    if (_initialSyncDone) return;
    _initialSyncDone = true;
    
    // Initial mount sync
    final mountState = ref.read(mountStateProvider);
    final mountNotifier = ref.read(mountPositionProvider.notifier);
    if (mountState.connectionState == DeviceConnectionState.connected) {
      MountTrackingStatus status;
      if (mountState.isSlewing) {
        status = MountTrackingStatus.slewing;
      } else if (mountState.isParked) {
        status = MountTrackingStatus.parked;
      } else if (mountState.isTracking) {
        status = MountTrackingStatus.tracking;
      } else {
        status = MountTrackingStatus.stopped;
      }
      mountNotifier.updatePosition(
        raHours: mountState.ra,
        decDegrees: mountState.dec,
        status: status,
        isConnected: true,
      );
    }
    
    // Initial rotator sync
    final rotatorState = ref.read(rotatorStateProvider);
    if (rotatorState.connectionState == DeviceConnectionState.connected && rotatorState.position != null) {
      ref.read(equipmentFOVProvider.notifier).setRotation(rotatorState.position!);
    }
  }
  
  void _handleObjectTapped(CelestialObject? object, CelestialCoordinate coordinates, Offset screenPosition) {
    // If in slew mode, handle slew instead of normal tap behavior
    if (_slewMode) {
      _handleSlewToCoordinates(coordinates, objectName: object?.name);
      return;
    }

    // Update selected object provider
    if (object != null) {
      ref.read(selectedObjectProvider.notifier).selectObject(object);
    } else {
      ref.read(selectedObjectProvider.notifier).clearSelection();
    }

    // Only show popup if an object was found
    if (object != null) {
      final renderBox = _skyViewKey.currentContext?.findRenderObject() as RenderBox?;
      if (renderBox != null) {
        // Convert to global position for proper popup placement
        final globalPosition = renderBox.localToGlobal(screenPosition);
        setState(() {
          _showPopup = true;
          _popupPosition = globalPosition;
          _popupObject = object;
          _popupCoordinates = coordinates;
        });
      }
    } else {
      _dismissPopup();
    }
  }
  
  void _dismissPopup() {
    if (_showPopup) {
      setState(() {
        _showPopup = false;
        _popupObject = null;
        _popupCoordinates = null;
      });
    }
  }
  
  void _sendToFraming() {
    if (_popupObject == null) return;
    
    final obj = _popupObject!;
    final coords = _popupCoordinates ?? obj.coordinates;
    
    // Set the framing target
    ref.read(framingProvider.notifier).setTargetCoordinates(
      coords.ra,
      coords.dec,
      name: obj.name,
    );
    
    // Navigate to framing screen
    try {
      context.goNamed('framing');
    } catch (e) {
      // Router might not be available, ignore
    }
    
    _dismissPopup();
  }
  
  void _addToSequencer() {
    if (_popupObject == null) return;

    final obj = _popupObject!;
    final coords = _popupCoordinates ?? obj.coordinates;

    // Add to sequencer, adopting any orphan instructions
    ref.read(currentSequenceProvider.notifier).addTargetHeader(
      TargetHeaderNode(
        targetName: obj.name,
        raHours: coords.ra,
        decDegrees: coords.dec,
      ),
    );

    // Show confirmation
    ScaffoldMessenger.of(context).showSnackBar(
      SnackBar(
        content: Text('Added ${obj.name} to sequence'),
        behavior: SnackBarBehavior.floating,
        duration: const Duration(seconds: 2),
      ),
    );

    _dismissPopup();
  }

  Future<void> _handleSlewToTarget() async {
    if (_popupObject == null) return;

    final obj = _popupObject!;
    final coords = _popupCoordinates ?? obj.coordinates;

    try {
      // Slew mount
      await ref.read(deviceServiceProvider).slewMountToCoordinates(
        coords.ra,
        coords.dec,
      );

      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Slewing to ${obj.name}...'),
            behavior: SnackBarBehavior.floating,
            duration: const Duration(seconds: 2),
          ),
        );
      }
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Failed to slew: $e'),
            backgroundColor: Theme.of(context).colorScheme.error,
            behavior: SnackBarBehavior.floating,
          ),
        );
      }
    }

    _dismissPopup();
  }

  Future<void> _handleSlewToCoordinates(CelestialCoordinate coords, {String? objectName}) async {
    // Check if mount is connected
    final mountState = ref.read(mountStateProvider);
    if (mountState.connectionState != DeviceConnectionState.connected) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          const SnackBar(
            content: Text('Mount not connected'),
            behavior: SnackBarBehavior.floating,
          ),
        );
      }
      return;
    }

    // Show confirmation dialog
    final confirmed = await showDialog<bool>(
      context: context,
      builder: (context) => AlertDialog(
        title: const Text('Confirm Slew'),
        content: Text(
          objectName != null
              ? 'Slew mount to $objectName?\n\nRA: ${coords.ra.toStringAsFixed(4)}h\nDec: ${coords.dec.toStringAsFixed(4)}°'
              : 'Slew mount to coordinates?\n\nRA: ${coords.ra.toStringAsFixed(4)}h\nDec: ${coords.dec.toStringAsFixed(4)}°',
        ),
        actions: [
          TextButton(
            onPressed: () => Navigator.of(context).pop(false),
            child: const Text('Cancel'),
          ),
          FilledButton(
            onPressed: () => Navigator.of(context).pop(true),
            child: const Text('Slew'),
          ),
        ],
      ),
    );

    if (confirmed != true) return;

    try {
      await ref.read(deviceServiceProvider).slewMountToCoordinates(
        coords.ra,
        coords.dec,
      );

      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text(objectName != null ? 'Slewing to $objectName...' : 'Slewing to coordinates...'),
            behavior: SnackBarBehavior.floating,
            duration: const Duration(seconds: 2),
          ),
        );
      }
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Failed to slew: $e'),
            backgroundColor: Theme.of(context).colorScheme.error,
            behavior: SnackBarBehavior.floating,
          ),
        );
      }
    }
  }

  void _toggleSlewMode() {
    setState(() {
      _slewMode = !_slewMode;
    });
    if (_slewMode) {
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(
          content: Text('Slew mode enabled - tap on sky to slew mount'),
          behavior: SnackBarBehavior.floating,
          duration: Duration(seconds: 2),
        ),
      );
    }
  }

  Future<void> _handleStopSlew() async {
    // Check if mount is connected
    final mountState = ref.read(mountStateProvider);
    if (mountState.connectionState != DeviceConnectionState.connected) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          const SnackBar(
            content: Text('Mount not connected'),
            behavior: SnackBarBehavior.floating,
          ),
        );
      }
      return;
    }

    // Check if mount is actually slewing
    if (!mountState.isSlewing) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          const SnackBar(
            content: Text('Mount is not slewing'),
            behavior: SnackBarBehavior.floating,
          ),
        );
      }
      return;
    }

    try {
      // Abort the slew
      await ref.read(deviceServiceProvider).abortMountSlew();

      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          const SnackBar(
            content: Text('Mount slew aborted'),
            behavior: SnackBarBehavior.floating,
            backgroundColor: Colors.green,
          ),
        );
      }
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Failed to abort slew: $e'),
            behavior: SnackBarBehavior.floating,
            backgroundColor: Colors.red,
          ),
        );
      }
    }
  }

  /// Handle keyboard events for desktop navigation
  KeyEventResult _handleKeyEvent(FocusNode node, KeyEvent event) {
    if (event is! KeyDownEvent) return KeyEventResult.ignored;

    final key = event.logicalKey;

    // Arrow keys - pan view
    if (key == LogicalKeyboardKey.arrowUp) {
      _panView(0, -1);
      return KeyEventResult.handled;
    }
    if (key == LogicalKeyboardKey.arrowDown) {
      _panView(0, 1);
      return KeyEventResult.handled;
    }
    if (key == LogicalKeyboardKey.arrowLeft) {
      _panView(-1, 0);
      return KeyEventResult.handled;
    }
    if (key == LogicalKeyboardKey.arrowRight) {
      _panView(1, 0);
      return KeyEventResult.handled;
    }

    // +/- or =/- for zoom
    if (key == LogicalKeyboardKey.equal || key == LogicalKeyboardKey.add || key == LogicalKeyboardKey.numpadAdd) {
      _zoomIn();
      return KeyEventResult.handled;
    }
    if (key == LogicalKeyboardKey.minus || key == LogicalKeyboardKey.numpadSubtract) {
      _zoomOut();
      return KeyEventResult.handled;
    }

    // R - reset view to default
    if (key == LogicalKeyboardKey.keyR) {
      _resetView();
      return KeyEventResult.handled;
    }

    // G - toggle coordinate grid
    if (key == LogicalKeyboardKey.keyG) {
      ref.read(skyRenderConfigProvider.notifier).toggleGrid();
      return KeyEventResult.handled;
    }

    // C - toggle constellation lines
    if (key == LogicalKeyboardKey.keyC) {
      ref.read(skyRenderConfigProvider.notifier).toggleConstellationLines();
      return KeyEventResult.handled;
    }

    // M - toggle minimap
    if (key == LogicalKeyboardKey.keyM) {
      ref.read(showMinimapProvider.notifier).state = !ref.read(showMinimapProvider);
      return KeyEventResult.handled;
    }

    // F - toggle FOV overlay
    if (key == LogicalKeyboardKey.keyF) {
      setState(() => _showFOV = !_showFOV);
      return KeyEventResult.handled;
    }

    // Escape - dismiss popup and clear selection
    if (key == LogicalKeyboardKey.escape) {
      _dismissPopup();
      ref.read(selectedObjectProvider.notifier).clearSelection();
      return KeyEventResult.handled;
    }

    return KeyEventResult.ignored;
  }

  /// Pan the view by a relative amount
  void _panView(double dx, double dy) {
    final viewState = ref.read(skyViewStateProvider);
    final panAmount = viewState.fieldOfView / 20; // Pan 5% of FOV
    ref.read(skyViewStateProvider.notifier).setCenter(
      viewState.centerRA + dx * panAmount / 15, // Convert degrees to hours for RA
      (viewState.centerDec + dy * panAmount).clamp(-90.0, 90.0),
    );
  }

  /// Zoom in by 20%
  void _zoomIn() {
    final viewState = ref.read(skyViewStateProvider);
    ref.read(skyViewStateProvider.notifier).setFieldOfView(
      (viewState.fieldOfView * 0.8).clamp(1.0, 120.0),
    );
  }

  /// Zoom out by 25%
  void _zoomOut() {
    final viewState = ref.read(skyViewStateProvider);
    ref.read(skyViewStateProvider.notifier).setFieldOfView(
      (viewState.fieldOfView * 1.25).clamp(1.0, 120.0),
    );
  }

  /// Reset view to default center and FOV
  void _resetView() {
    ref.read(skyViewStateProvider.notifier).setCenter(0, 0);
    ref.read(skyViewStateProvider.notifier).setFieldOfView(60);
  }

  @override
  Widget build(BuildContext context) {
    final colors = Theme.of(context).extension<NightshadeColors>()!;
    final selectedObject = ref.watch(selectedObjectProvider);

    // Sync mount state from equipment provider to planetarium mount position provider
    ref.listen<MountState>(mountStateProvider, (previous, next) {
      final mountNotifier = ref.read(mountPositionProvider.notifier);
      if (next.connectionState == DeviceConnectionState.connected) {
        // Map tracking status
        MountTrackingStatus status;
        if (next.isSlewing) {
          status = MountTrackingStatus.slewing;
        } else if (next.isParked) {
          status = MountTrackingStatus.parked;
        } else if (next.isTracking) {
          status = MountTrackingStatus.tracking;
        } else {
          status = MountTrackingStatus.stopped;
        }

        mountNotifier.updatePosition(
          raHours: next.ra,
          decDegrees: next.dec,
          status: status,
          isConnected: true,
        );
      } else {
        mountNotifier.setDisconnected();
      }
    });

    // Sync rotator position to equipment FOV rotation
    ref.listen<RotatorState>(rotatorStateProvider, (previous, next) {
      if (next.connectionState == DeviceConnectionState.connected && next.position != null) {
        ref.read(equipmentFOVProvider.notifier).setRotation(next.position!);
      }
    });

    return Focus(
      autofocus: true,
      onKeyEvent: _handleKeyEvent,
      child: GestureDetector(
        onTapDown: (details) {
          // Dismiss popup when clicking elsewhere
          if (_showPopup) {
            // Check if tap is outside the popup area
            final popupRect = Rect.fromCenter(
              center: _popupPosition,
              width: 320,
              height: 280,
            );
            if (!popupRect.contains(details.globalPosition)) {
              _dismissPopup();
            }
          }
        },
        child: Stack(
          children: [
            Row(
              children: [
                // Sky canvas (main area)
                Expanded(
                  child: Stack(
                    key: _skyViewKey,
                    children: [
                      // Interactive sky view
                      InteractiveSkyView(
                        showFOV: _showFOV,
                        onObjectTapped: _handleObjectTapped,
                      ),

                      // Top overlay bar
                      Positioned(
                        top: 0,
                        left: 0,
                        right: 0,
                        child: SizedBox(
                          width: double.infinity,
                          child: _TopOverlay(colors: colors),
                        ),
                      ),

                      // Bottom info bar
                      Positioned(
                        bottom: 0,
                        left: 0,
                        right: 0,
                        child: SizedBox(
                          width: double.infinity,
                          child: _BottomInfoBar(colors: colors),
                        ),
                      ),

                      // View controls
                      Positioned(
                        top: 60,
                        left: 16,
                        child: _ViewControls(
                          colors: colors,
                          showFOV: _showFOV,
                          onToggleFOV: () => setState(() => _showFOV = !_showFOV),
                        ),
                      ),

                      // Slew controls
                      Positioned(
                        top: 220,
                        left: 16,
                        child: _SlewControls(
                          colors: colors,
                          slewMode: _slewMode,
                          onToggleSlewMode: _toggleSlewMode,
                          onStopSlew: _handleStopSlew,
                        ),
                      ),

                      // Selected Object HUD
                      Positioned(
                        top: 100,
                        left: 0,
                        right: 0,
                        child: Center(
                          child: _SelectedObjectHud(
                            colors: colors,
                            onSlew: () async {
                              final selectedState = ref.read(selectedObjectProvider);
                              final obj = selectedState.object;
                              final coords = selectedState.coordinates;
                              if (obj != null && coords != null) {
                                try {
                                  await ref.read(deviceServiceProvider).slewMountToCoordinates(
                                    coords.ra,
                                    coords.dec,
                                  );
                                  if (context.mounted) {
                                    ScaffoldMessenger.of(context).showSnackBar(
                                      SnackBar(content: Text('Slewing to ${obj.name}...')),
                                    );
                                  }
                                } catch (e) {
                                  if (context.mounted) {
                                    ScaffoldMessenger.of(context).showSnackBar(
                                      SnackBar(content: Text('Slew failed: $e')),
                                    );
                                  }
                                }
                              }
                            },
                          ),
                        ),
                      ),

                      // Compass HUD (bottom-left, above time control)
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

                      // Time Control Panel
                      Positioned(
                        bottom: 110,
                        left: 16,
                        child: TimeControlPanel(
                          backgroundColor: colors.surface.withValues(alpha: 0.9),
                          textColor: colors.textPrimary,
                          accentColor: colors.accent,
                          compact: false,
                        ),
                      ),

                      // Filter Sidebar
                      Positioned(
                        top: 60,
                        right: 0,
                        bottom: 0,
                        child: FilterSidebar(
                          isExpanded: _filterSidebarExpanded,
                          onToggle: () => setState(() => _filterSidebarExpanded = !_filterSidebarExpanded),
                        ),
                      ),
                    ],
                  ),
                ),

                // Right sidebar
                ResizablePanel(
                  initialWidth: 340,
                  minWidth: 250,
                  maxWidth: 500,
                  side: ResizeSide.left,
                  child: Container(
                    // width: 340, // Removed for ResizablePanel
                    decoration: BoxDecoration(
                      color: colors.surface,
                      border: Border(left: BorderSide(color: colors.border)),
                    ),
                    child: Column(
                      children: [
                        // Search
                        _SearchHeader(
                          colors: colors,
                          controller: _searchController,
                          onSearch: (query) {
                            ref.read(objectSearchProvider.notifier).search(query);
                          },
                        ),

                        // Tabs
                        Expanded(
                          child: DefaultTabController(
                            length: 4,
                            child: Column(
                              children: [
                                _SidebarTabs(colors: colors),
                                Expanded(
                                  child: TabBarView(
                                    children: [
                                      _TonightTab(colors: colors),
                                      _ObjectsTab(colors: colors),
                                      _SearchResultsTab(colors: colors),
                                      _InfoTab(colors: colors, selectedObject: selectedObject),
                                    ],
                                  ),
                                ),
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
            
            // Object info popup overlay
            if (_showPopup && _popupObject != null)
              _ObjectInfoPopup(
                colors: colors,
                object: _popupObject!,
                coordinates: _popupCoordinates ?? _popupObject!.coordinates,
                selectedObjectState: selectedObject,
                position: _popupPosition,
                onDismiss: _dismissPopup,
                onSendToFraming: _sendToFraming,
                onAddToSequencer: _addToSequencer,
                onSlewToTarget: _handleSlewToTarget,
              ),
          ],
        ),
      ),
    );
  }
}

class _TopOverlay extends ConsumerWidget {
  final NightshadeColors colors;

  const _TopOverlay({required this.colors});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final location = ref.watch(observerLocationProvider);
    final time = ref.watch(observationTimeProvider);
    final lst = ref.watch(localSiderealTimeProvider);
    final renderConfig = ref.watch(skyRenderConfigProvider);
    final settingsAsync = ref.watch(appSettingsProvider);
    
    // Get location name from settings if available
    String locationLabel;
    final settings = settingsAsync.valueOrNull;
    if (settings != null && (settings.latitude != 0.0 || settings.longitude != 0.0)) {
      locationLabel = '${settings.latitude.toStringAsFixed(2)}°, ${settings.longitude.toStringAsFixed(2)}°';
    } else {
      locationLabel = location.locationName ?? 
        '${location.latitude.toStringAsFixed(2)}°, ${location.longitude.toStringAsFixed(2)}°';
    }
    
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 20, vertical: 12),
      decoration: BoxDecoration(
        gradient: LinearGradient(
          begin: Alignment.topCenter,
          end: Alignment.bottomCenter,
          colors: [
            Colors.black.withValues(alpha: 0.8),
            Colors.transparent,
          ],
        ),
      ),
      child: Row(
        children: [
          _OverlayChip(
            icon: LucideIcons.mapPin,
            label: locationLabel,
            colors: colors,
          ),
          const SizedBox(width: 12),
          _OverlayChip(
            icon: LucideIcons.clock,
            label: DateFormat('HH:mm:ss').format(time.time),
            colors: colors,
          ),
          const SizedBox(width: 12),
          _OverlayChip(
            icon: LucideIcons.star,
            label: 'LST ${_formatHours(lst)}',
            colors: colors,
          ),
          if (!time.isRealTime) ...[
            const SizedBox(width: 12),
            _TimeControlButton(
              icon: LucideIcons.play,
              onTap: () => ref.read(observationTimeProvider.notifier).setRealTime(true),
              colors: colors,
            ),
          ],
          const Spacer(),
          _OverlayToggle(
            icon: LucideIcons.grid,
            isActive: renderConfig.showCoordinateGrid,
            onTap: ref.read(skyRenderConfigProvider.notifier).toggleGrid,
          ),
          const SizedBox(width: 4),
          _OverlayToggle(
            icon: LucideIcons.activity,
            isActive: renderConfig.showConstellationLines,
            onTap: ref.read(skyRenderConfigProvider.notifier).toggleConstellationLines,
          ),
          const SizedBox(width: 4),
          _OverlayToggle(
            icon: LucideIcons.tag,
            isActive: renderConfig.showConstellationLabels,
            onTap: ref.read(skyRenderConfigProvider.notifier).toggleConstellationLabels,
          ),
          const SizedBox(width: 4),
          _OverlayToggle(
            icon: LucideIcons.circle,
            isActive: renderConfig.showHorizon,
            onTap: ref.read(skyRenderConfigProvider.notifier).toggleHorizon,
          ),
        ],
      ),
    );
  }
  
  String _formatHours(double hours) {
    final h = hours.floor();
    final m = ((hours - h) * 60).floor();
    return '${h.toString().padLeft(2, '0')}:${m.toString().padLeft(2, '0')}';
  }
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
      padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 6),
      decoration: BoxDecoration(
        color: Colors.black.withValues(alpha: 0.5),
        borderRadius: BorderRadius.circular(20),
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
              fontFeatures: [FontFeature.tabularFigures()],
            ),
          ),
        ],
      ),
    );
  }
}

class _OverlayToggle extends StatefulWidget {
  final IconData icon;
  final bool isActive;
  final VoidCallback onTap;

  const _OverlayToggle({
    required this.icon,
    required this.isActive,
    required this.onTap,
  });

  @override
  State<_OverlayToggle> createState() => _OverlayToggleState();
}

class _OverlayToggleState extends State<_OverlayToggle> {
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
          width: 32,
          height: 32,
          decoration: BoxDecoration(
            color: widget.isActive
                ? Colors.white.withValues(alpha: 0.2)
                : _isHovered
                    ? Colors.white.withValues(alpha: 0.1)
                    : Colors.transparent,
            borderRadius: BorderRadius.circular(6),
          ),
          child: Icon(
            widget.icon,
            size: 16,
            color: widget.isActive ? Colors.white : Colors.white70,
          ),
        ),
      ),
    );
  }
}

class _TimeControlButton extends StatelessWidget {
  final IconData icon;
  final VoidCallback onTap;
  final NightshadeColors colors;

  const _TimeControlButton({
    required this.icon,
    required this.onTap,
    required this.colors,
  });

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: onTap,
      child: Container(
        padding: const EdgeInsets.all(6),
        decoration: BoxDecoration(
          color: colors.primary.withValues(alpha: 0.3),
          borderRadius: BorderRadius.circular(16),
        ),
        child: Icon(icon, size: 14, color: colors.primary),
      ),
    );
  }
}

class _ViewControls extends ConsumerWidget {
  final NightshadeColors colors;
  final bool showFOV;
  final VoidCallback onToggleFOV;

  const _ViewControls({
    required this.colors,
    required this.showFOV,
    required this.onToggleFOV,
  });

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final viewState = ref.watch(skyViewStateProvider);

    return Container(
      padding: const EdgeInsets.all(8),
      decoration: BoxDecoration(
        color: Colors.black.withValues(alpha: 0.6),
        borderRadius: BorderRadius.circular(8),
      ),
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          _ViewControlButton(
            icon: LucideIcons.plus,
            onTap: ref.read(skyViewStateProvider.notifier).zoomIn,
          ),
          const SizedBox(height: 4),
          Container(
            padding: const EdgeInsets.symmetric(vertical: 4, horizontal: 8),
            child: Text(
              '${viewState.fieldOfView.toStringAsFixed(0)}°',
              style: const TextStyle(
                fontSize: 10,
                color: Colors.white70,
                fontFeatures: [FontFeature.tabularFigures()],
              ),
            ),
          ),
          const SizedBox(height: 4),
          _ViewControlButton(
            icon: LucideIcons.minus,
            onTap: ref.read(skyViewStateProvider.notifier).zoomOut,
          ),
          const Divider(height: 16, color: Colors.white24),
          _ViewControlButton(
            icon: LucideIcons.home,
            onTap: () {
              ref.read(skyViewStateProvider.notifier).setCenter(0, 0);
              ref.read(skyViewStateProvider.notifier).setFieldOfView(60);
            },
          ),
          const SizedBox(height: 4),
          _ViewControlButton(
            icon: LucideIcons.frame,
            isActive: showFOV,
            onTap: onToggleFOV,
            tooltip: 'Toggle FOV indicator',
          ),
          const Divider(height: 16, color: Colors.white24),
          // Compass HUD toggle - wrapped in Consumer to scope rebuilds
          Consumer(
            builder: (context, ref, _) {
              return _ViewControlButton(
                icon: LucideIcons.compass,
                isActive: ref.watch(showCompassHudProvider),
                onTap: () {
                  final notifier = ref.read(showCompassHudProvider.notifier);
                  notifier.state = !notifier.state;
                },
                tooltip: 'Toggle Compass',
              );
            },
          ),
          const SizedBox(height: 4),
          // Mini-map toggle - wrapped in Consumer to scope rebuilds
          Consumer(
            builder: (context, ref, _) {
              return _ViewControlButton(
                icon: LucideIcons.map,
                isActive: ref.watch(showMinimapProvider),
                onTap: () {
                  final notifier = ref.read(showMinimapProvider.notifier);
                  notifier.state = !notifier.state;
                },
                tooltip: 'Toggle Mini-map',
              );
            },
          ),
          const SizedBox(height: 4),
          _QualitySettingsButton(colors: colors),
        ],
      ),
    );
  }
}

/// Quality settings popup button
class _QualitySettingsButton extends ConsumerWidget {
  final NightshadeColors colors;

  const _QualitySettingsButton({required this.colors});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final quality = ref.watch(renderQualityProvider);

    return PopupMenuButton<RenderQuality>(
      icon: const Icon(
        LucideIcons.settings2,
        size: 18,
        color: Colors.white70,
      ),
      tooltip: 'Render quality',
      color: colors.surface,
      shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(8)),
      offset: const Offset(-120, 0),
      onSelected: (tier) {
        ref.read(renderQualityProvider.notifier).setQuality(tier);
      },
      itemBuilder: (context) => [
        _buildQualityMenuItem(
          context,
          RenderQuality.performance,
          'Performance',
          'Best for Raspberry Pi',
          quality.quality,
        ),
        _buildQualityMenuItem(
          context,
          RenderQuality.balanced,
          'Balanced',
          'Recommended',
          quality.quality,
        ),
        _buildQualityMenuItem(
          context,
          RenderQuality.quality,
          'Quality',
          'Best visuals',
          quality.quality,
        ),
      ],
    );
  }

  PopupMenuItem<RenderQuality> _buildQualityMenuItem(
    BuildContext context,
    RenderQuality tier,
    String title,
    String subtitle,
    RenderQuality current,
  ) {
    final isSelected = tier == current;
    return PopupMenuItem<RenderQuality>(
      value: tier,
      child: Row(
        children: [
          Icon(
            isSelected ? LucideIcons.checkCircle : LucideIcons.circle,
            size: 16,
            color: isSelected ? colors.accent : colors.textSecondary,
          ),
          const SizedBox(width: 8),
          Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            mainAxisSize: MainAxisSize.min,
            children: [
              Text(
                title,
                style: TextStyle(
                  color: colors.textPrimary,
                  fontWeight: isSelected ? FontWeight.w600 : FontWeight.normal,
                ),
              ),
              Text(
                subtitle,
                style: TextStyle(
                  fontSize: 11,
                  color: colors.textSecondary,
                ),
              ),
            ],
          ),
        ],
      ),
    );
  }
}

/// Slew control buttons
class _SlewControls extends ConsumerWidget {
  final NightshadeColors colors;
  final bool slewMode;
  final VoidCallback onToggleSlewMode;
  final VoidCallback onStopSlew;

  const _SlewControls({
    required this.colors,
    required this.slewMode,
    required this.onToggleSlewMode,
    required this.onStopSlew,
  });

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final mountState = ref.watch(mountStateProvider);
    final isConnected = mountState.connectionState == DeviceConnectionState.connected;
    final isSlewing = mountState.isSlewing;

    return Container(
      padding: const EdgeInsets.all(8),
      decoration: BoxDecoration(
        color: Colors.black.withValues(alpha: 0.6),
        borderRadius: BorderRadius.circular(8),
        border: slewMode
            ? Border.all(color: const Color(0xFFFF9800), width: 2)
            : null,
      ),
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          // Slew mode toggle
          Tooltip(
            message: slewMode ? 'Disable slew mode' : 'Enable slew mode',
            child: _SlewControlButton(
              icon: LucideIcons.move,
              isActive: slewMode,
              isEnabled: isConnected,
              onTap: isConnected ? onToggleSlewMode : null,
            ),
          ),
          const SizedBox(height: 8),
          // Stop slew button
          Tooltip(
            message: 'Stop slew',
            child: _SlewControlButton(
              icon: LucideIcons.octagon,
              isActive: false,
              isEnabled: isConnected && isSlewing,
              isDestructive: true,
              onTap: isConnected && isSlewing ? onStopSlew : null,
            ),
          ),
          if (slewMode) ...[
            const SizedBox(height: 8),
            const Text(
              'SLEW',
              style: TextStyle(
                fontSize: 9,
                fontWeight: FontWeight.bold,
                color: Color(0xFFFF9800),
              ),
            ),
          ],
        ],
      ),
    );
  }
}

class _SlewControlButton extends StatefulWidget {
  final IconData icon;
  final bool isActive;
  final bool isEnabled;
  final bool isDestructive;
  final VoidCallback? onTap;

  const _SlewControlButton({
    required this.icon,
    required this.isActive,
    required this.isEnabled,
    this.isDestructive = false,
    this.onTap,
  });

  @override
  State<_SlewControlButton> createState() => _SlewControlButtonState();
}

class _SlewControlButtonState extends State<_SlewControlButton> {
  bool _isHovered = false;

  @override
  Widget build(BuildContext context) {
    final color = widget.isDestructive
        ? const Color(0xFFE53935)
        : widget.isActive
            ? const Color(0xFFFF9800)
            : Colors.white70;

    return MouseRegion(
      onEnter: (_) => setState(() => _isHovered = true),
      onExit: (_) => setState(() => _isHovered = false),
      child: GestureDetector(
        onTap: widget.isEnabled ? widget.onTap : null,
        child: AnimatedContainer(
          duration: const Duration(milliseconds: 150),
          width: 28,
          height: 28,
          decoration: BoxDecoration(
            color: widget.isActive
                ? const Color(0xFFFF9800).withValues(alpha: 0.2)
                : _isHovered && widget.isEnabled
                    ? Colors.white.withValues(alpha: 0.1)
                    : Colors.transparent,
            borderRadius: BorderRadius.circular(4),
          ),
          child: Icon(
            widget.icon,
            size: 16,
            color: widget.isEnabled ? color : Colors.white24,
          ),
        ),
      ),
    );
  }
}

class _ViewControlButton extends StatefulWidget {
  final IconData icon;
  final VoidCallback onTap;
  final bool isActive;
  final String? tooltip;

  const _ViewControlButton({
    required this.icon,
    required this.onTap,
    this.isActive = false,
    this.tooltip,
  });

  @override
  State<_ViewControlButton> createState() => _ViewControlButtonState();
}

class _ViewControlButtonState extends State<_ViewControlButton> {
  bool _isHovered = false;

  @override
  Widget build(BuildContext context) {
    final button = MouseRegion(
      onEnter: (_) => setState(() => _isHovered = true),
      onExit: (_) => setState(() => _isHovered = false),
      child: GestureDetector(
        onTap: widget.onTap,
        child: AnimatedContainer(
          duration: const Duration(milliseconds: 150),
          width: 28,
          height: 28,
          decoration: BoxDecoration(
            color: widget.isActive
                ? const Color(0xFF00E676).withValues(alpha: 0.3)
                : (_isHovered ? Colors.white.withValues(alpha: 0.1) : Colors.transparent),
            borderRadius: BorderRadius.circular(4),
            border: widget.isActive
                ? Border.all(color: const Color(0xFF00E676), width: 1)
                : null,
          ),
          child: Icon(
            widget.icon,
            size: 14,
            color: widget.isActive ? const Color(0xFF00E676) : Colors.white70,
          ),
        ),
      ),
    );

    if (widget.tooltip != null) {
      return Tooltip(
        message: widget.tooltip!,
        child: button,
      );
    }
    return button;
  }
}

class _BottomInfoBar extends ConsumerWidget {
  final NightshadeColors colors;

  const _BottomInfoBar({required this.colors});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final viewState = ref.watch(skyViewStateProvider);
    final selectedObject = ref.watch(selectedObjectProvider);
    
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 20, vertical: 12),
      decoration: BoxDecoration(
        gradient: LinearGradient(
          begin: Alignment.bottomCenter,
          end: Alignment.topCenter,
          colors: [
            Colors.black.withValues(alpha: 0.8),
            Colors.transparent,
          ],
        ),
      ),
      child: Row(
        children: [
          _InfoItem(
            label: 'Center RA',
            value: _formatRA(viewState.centerRA),
            colors: colors,
          ),
          const SizedBox(width: 20),
          _InfoItem(
            label: 'Center Dec',
            value: _formatDec(viewState.centerDec),
            colors: colors,
          ),
          const SizedBox(width: 20),
          _InfoItem(
            label: 'FOV',
            value: '${viewState.fieldOfView.toStringAsFixed(1)}°',
            colors: colors,
          ),
          if (selectedObject.currentAltAz != null) ...[
            const SizedBox(width: 40),
            _InfoItem(
              label: 'Selected Alt',
              value: '${selectedObject.currentAltAz!.$1.toStringAsFixed(1)}°',
              colors: colors,
              valueColor: selectedObject.currentAltAz!.$1 > 0 
                  ? colors.success 
                  : colors.error,
            ),
            const SizedBox(width: 20),
            _InfoItem(
              label: 'Az',
              value: '${selectedObject.currentAltAz!.$2.toStringAsFixed(1)}°',
              colors: colors,
            ),
          ],
        ],
      ),
    );
  }
  
  String _formatRA(double ra) {
    final h = ra.floor();
    final m = ((ra - h) * 60).floor();
    final s = (((ra - h) * 60 - m) * 60).floor();
    return '${h}h ${m}m ${s}s';
  }
  
  String _formatDec(double dec) {
    final sign = dec >= 0 ? '+' : '-';
    final d = dec.abs().floor();
    final m = ((dec.abs() - d) * 60).floor();
    return "$sign$d° $m'";
  }
}

class _InfoItem extends StatelessWidget {
  final String label;
  final String value;
  final NightshadeColors colors;
  final Color? valueColor;

  const _InfoItem({
    required this.label,
    required this.value,
    required this.colors,
    this.valueColor,
  });

  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        Text(
          '$label:',
          style: TextStyle(fontSize: 11, color: Colors.white.withValues(alpha: 0.5)),
        ),
        const SizedBox(width: 4),
        Text(
          value,
          style: TextStyle(
            fontSize: 12,
            fontWeight: FontWeight.w500,
            color: valueColor ?? Colors.white70,
            fontFeatures: const [FontFeature.tabularFigures()],
          ),
        ),
      ],
    );
  }
}

class _SearchHeader extends ConsumerStatefulWidget {
  final NightshadeColors colors;
  final TextEditingController controller;
  final ValueChanged<String> onSearch;

  const _SearchHeader({
    required this.colors,
    required this.controller,
    required this.onSearch,
  });

  @override
  ConsumerState<_SearchHeader> createState() => _SearchHeaderState();
}

class _SearchHeaderState extends ConsumerState<_SearchHeader> {
  final LayerLink _layerLink = LayerLink();
  OverlayEntry? _overlayEntry;
  final FocusNode _focusNode = FocusNode();
  Timer? _debounceTimer;
  CelestialCoordinate? _parsedCoordinate;

  @override
  void initState() {
    super.initState();
    _focusNode.addListener(() {
      if (_focusNode.hasFocus) {
        _showOverlay();
      } else {
        _hideOverlay();
      }
    });
    widget.controller.addListener(_onTextChanged);
  }

  @override
  void dispose() {
    _hideOverlay();
    _focusNode.dispose();
    _debounceTimer?.cancel();
    widget.controller.removeListener(_onTextChanged);
    super.dispose();
  }

  /// Parse coordinate input like "RA 5h 35m, Dec -5d 23'"
  CelestialCoordinate? _parseCoordinates(String input) {
    // Try pattern like "RA 5h 35m, Dec -5d 23'" or "RA 5h 35m Dec -5 23"
    final pattern = RegExp(r'RA\s*(\d+)h\s*(\d+)m.*Dec\s*([+-]?\d+)[°d]?\s*(\d+)', caseSensitive: false);
    final match = pattern.firstMatch(input);

    if (match != null) {
      final raHours = double.parse(match.group(1)!);
      final raMinutes = double.parse(match.group(2)!);
      final decDegrees = double.parse(match.group(3)!);
      final decMinutes = double.parse(match.group(4)!);

      final ra = raHours + raMinutes / 60;
      final dec = decDegrees + (decDegrees >= 0 ? decMinutes / 60 : -decMinutes / 60);

      return CelestialCoordinate(ra: ra, dec: dec);
    }
    return null;
  }

  void _onTextChanged() {
    // Cancel previous debounce timer
    _debounceTimer?.cancel();

    // Check for coordinate input first
    _parsedCoordinate = _parseCoordinates(widget.controller.text);
    if (_parsedCoordinate != null) {
      // If coordinates were parsed, show overlay immediately
      _showOverlay();
      return;
    }

    if (widget.controller.text.length >= 2) {
      // Debounce search by 250ms for instant results as user types
      _debounceTimer = Timer(const Duration(milliseconds: 250), () {
        if (mounted) {
          ref.read(objectSearchProvider.notifier).search(widget.controller.text);
          _showOverlay();
        }
      });
    } else {
      _hideOverlay();
    }
  }

  void _showOverlay() {
    if (_overlayEntry != null) return;

    // Don't show if query is too short (unless we have parsed coordinates)
    if (widget.controller.text.length < 2 && _parsedCoordinate == null) return;

    final overlay = Overlay.of(context);
    _overlayEntry = OverlayEntry(
      builder: (context) => Positioned(
        width: 308, // Match container width minus padding
        child: CompositedTransformFollower(
          link: _layerLink,
          showWhenUnlinked: false,
          offset: const Offset(0, 46), // Height of text field + padding
          child: Material(
            elevation: 8,
            color: widget.colors.surface,
            borderRadius: BorderRadius.circular(8),
            child: Container(
              decoration: BoxDecoration(
                border: Border.all(color: widget.colors.border),
                borderRadius: BorderRadius.circular(8),
                color: widget.colors.surface,
              ),
              constraints: const BoxConstraints(maxHeight: 350),
              child: Consumer(
                builder: (context, ref, child) {
                  // Check for parsed coordinates first
                  if (_parsedCoordinate != null) {
                    final coord = _parsedCoordinate!;
                    return Column(
                      mainAxisSize: MainAxisSize.min,
                      children: [
                        _SearchCategoryHeader(
                          title: 'Coordinates',
                          icon: LucideIcons.compass,
                          colors: widget.colors,
                        ),
                        MouseRegion(
                          cursor: SystemMouseCursors.click,
                          child: GestureDetector(
                            behavior: HitTestBehavior.opaque,
                            onTap: () {
                              // Navigate to parsed coordinates
                              ref.read(skyViewStateProvider.notifier).setCenter(coord.ra, coord.dec);
                              _hideOverlay();
                              _focusNode.unfocus();
                            },
                            child: ListTile(
                              dense: true,
                              leading: Container(
                                width: 32,
                                height: 32,
                                alignment: Alignment.center,
                                decoration: BoxDecoration(
                                  color: widget.colors.accent.withValues(alpha: 0.2),
                                  borderRadius: BorderRadius.circular(4),
                                ),
                                child: Icon(
                                  LucideIcons.crosshair,
                                  size: 16,
                                  color: widget.colors.accent,
                                ),
                              ),
                              title: Text(
                                'Go to coordinates',
                                style: TextStyle(color: widget.colors.textPrimary),
                              ),
                              subtitle: Text(
                                'RA ${coord.ra.toStringAsFixed(2)}h, Dec ${coord.dec.toStringAsFixed(2)}°',
                                style: TextStyle(color: widget.colors.textMuted, fontSize: 11),
                              ),
                            ),
                          ),
                        ),
                      ],
                    );
                  }

                  final searchState = ref.watch(objectSearchProvider);

                  if (searchState.isSearching) {
                    return const Center(
                      child: Padding(
                        padding: EdgeInsets.all(16.0),
                        child: CircularProgressIndicator(strokeWidth: 2),
                      ),
                    );
                  }

                  if (searchState.results.isEmpty) {
                    return Padding(
                      padding: const EdgeInsets.all(16.0),
                      child: Text(
                        'No results found',
                        style: TextStyle(color: widget.colors.textMuted),
                      ),
                    );
                  }

                  // Group results by category: Stars and DSOs
                  final stars = searchState.results.whereType<Star>().take(4).toList();
                  final dsos = searchState.results.whereType<DeepSkyObject>().take(4).toList();

                  // Calculate total items to show (max 8 results + category headers)
                  final totalItems = (stars.isNotEmpty ? stars.length + 1 : 0) +
                                    (dsos.isNotEmpty ? dsos.length + 1 : 0);

                  if (totalItems == 0) {
                    return Padding(
                      padding: const EdgeInsets.all(16.0),
                      child: Text(
                        'No results found',
                        style: TextStyle(color: widget.colors.textMuted),
                      ),
                    );
                  }

                  return ListView(
                    shrinkWrap: true,
                    padding: EdgeInsets.zero,
                    children: [
                      // DSO section
                      if (dsos.isNotEmpty) ...[
                        _SearchCategoryHeader(
                          title: 'Deep Sky Objects',
                          icon: LucideIcons.sparkles,
                          colors: widget.colors,
                        ),
                        ...dsos.map((dso) => _buildDsoResultTile(ref, dso)),
                      ],
                      // Stars section
                      if (stars.isNotEmpty) ...[
                        _SearchCategoryHeader(
                          title: 'Stars',
                          icon: LucideIcons.star,
                          colors: widget.colors,
                        ),
                        ...stars.map((star) => _buildStarResultTile(ref, star)),
                      ],
                    ],
                  );
                },
              ),
            ),
          ),
        ),
      ),
    );

    overlay.insert(_overlayEntry!);
  }

  Widget _buildDsoResultTile(WidgetRef ref, DeepSkyObject dso) {
    final info = getDsoDisplayInfo(dso);
    final displayName = info.$1;
    final catalogTag = info.$2;

    return MouseRegion(
      cursor: SystemMouseCursors.click,
      child: GestureDetector(
        behavior: HitTestBehavior.opaque,
        onTap: () {
          ref.read(selectedObjectProvider.notifier).selectObject(dso);
          ref.read(skyViewStateProvider.notifier).lookAt(dso.coordinates);
          widget.onSearch(dso.name);
          _hideOverlay();
          _focusNode.unfocus();
        },
        child: ListTile(
          dense: true,
          leading: Container(
            width: 32,
            height: 32,
            alignment: Alignment.center,
            decoration: BoxDecoration(
              color: widget.colors.surfaceAlt,
              borderRadius: BorderRadius.circular(4),
            ),
            child: Text(
              catalogTag,
              style: TextStyle(
                fontSize: 10,
                fontWeight: FontWeight.bold,
                color: widget.colors.primary,
              ),
            ),
          ),
          title: Text(
            displayName,
            style: TextStyle(color: widget.colors.textPrimary),
          ),
          subtitle: Text(
            dso.type.displayName,
            style: TextStyle(color: widget.colors.textMuted, fontSize: 11),
          ),
          trailing: dso.magnitude != null
              ? Text(
                  'mag ${dso.magnitude!.toStringAsFixed(1)}',
                  style: TextStyle(color: widget.colors.textMuted, fontSize: 11),
                )
              : null,
        ),
      ),
    );
  }

  Widget _buildStarResultTile(WidgetRef ref, Star star) {
    return MouseRegion(
      cursor: SystemMouseCursors.click,
      child: GestureDetector(
        behavior: HitTestBehavior.opaque,
        onTap: () {
          ref.read(selectedObjectProvider.notifier).selectObject(star);
          ref.read(skyViewStateProvider.notifier).lookAt(star.coordinates);
          widget.onSearch(star.name);
          _hideOverlay();
          _focusNode.unfocus();
        },
        child: ListTile(
          dense: true,
          leading: Container(
            width: 32,
            height: 32,
            alignment: Alignment.center,
            decoration: BoxDecoration(
              color: widget.colors.surfaceAlt,
              borderRadius: BorderRadius.circular(4),
            ),
            child: const Text(
              '★',
              style: TextStyle(
                fontSize: 14,
                color: Colors.amber,
              ),
            ),
          ),
          title: Text(
            star.name,
            style: TextStyle(color: widget.colors.textPrimary),
          ),
          subtitle: Text(
            'Star',
            style: TextStyle(color: widget.colors.textMuted, fontSize: 11),
          ),
          trailing: star.magnitude != null
              ? Text(
                  'mag ${star.magnitude!.toStringAsFixed(1)}',
                  style: TextStyle(color: widget.colors.textMuted, fontSize: 11),
                )
              : null,
        ),
      ),
    );
  }

  void _hideOverlay() {
    _overlayEntry?.remove();
    _overlayEntry = null;
  }

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.all(16),
      decoration: BoxDecoration(
        border: Border(bottom: BorderSide(color: widget.colors.border)),
      ),
      child: CompositedTransformTarget(
        link: _layerLink,
        child: TextField(
          controller: widget.controller,
          focusNode: _focusNode,
          style: TextStyle(fontSize: 13, color: widget.colors.textPrimary),
          decoration: InputDecoration(
            hintText: 'Search objects...',
            hintStyle: TextStyle(fontSize: 13, color: widget.colors.textMuted),
            prefixIcon: Icon(LucideIcons.search, size: 16, color: widget.colors.textMuted),
            suffixIcon: Container(
              margin: const EdgeInsets.all(8),
              padding: const EdgeInsets.symmetric(horizontal: 6, vertical: 2),
              decoration: BoxDecoration(
                color: widget.colors.background,
                borderRadius: BorderRadius.circular(4),
              ),
              child: Text(
                '⌘K',
                style: TextStyle(fontSize: 10, color: widget.colors.textMuted),
              ),
            ),
            filled: true,
            fillColor: widget.colors.surfaceAlt,
            border: OutlineInputBorder(
              borderRadius: BorderRadius.circular(10),
              borderSide: BorderSide(color: widget.colors.border),
            ),
            enabledBorder: OutlineInputBorder(
              borderRadius: BorderRadius.circular(10),
              borderSide: BorderSide(color: widget.colors.border),
            ),
            focusedBorder: OutlineInputBorder(
              borderRadius: BorderRadius.circular(10),
              borderSide: BorderSide(color: widget.colors.primary),
            ),
            contentPadding: const EdgeInsets.symmetric(horizontal: 14, vertical: 10),
          ),
          onSubmitted: (value) {
            widget.onSearch(value);
            _hideOverlay();
          },
        ),
      ),
    );
  }
}

/// Category header for grouped search results
class _SearchCategoryHeader extends StatelessWidget {
  final String title;
  final IconData icon;
  final NightshadeColors colors;

  const _SearchCategoryHeader({
    required this.title,
    required this.icon,
    required this.colors,
  });

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 8),
      decoration: BoxDecoration(
        color: colors.surfaceAlt.withValues(alpha: 0.5),
        border: Border(bottom: BorderSide(color: colors.border.withValues(alpha: 0.5))),
      ),
      child: Row(
        children: [
          Icon(icon, size: 12, color: colors.textMuted),
          const SizedBox(width: 6),
          Text(
            title,
            style: TextStyle(
              fontSize: 10,
              fontWeight: FontWeight.w600,
              color: colors.textMuted,
              letterSpacing: 0.5,
            ),
          ),
        ],
      ),
    );
  }
}

class _SidebarTabs extends StatelessWidget {
  final NightshadeColors colors;

  const _SidebarTabs({required this.colors});

  @override
  Widget build(BuildContext context) {
    return Container(
      height: 44,
      decoration: BoxDecoration(
        color: colors.surfaceAlt,
        border: Border(bottom: BorderSide(color: colors.border)),
      ),
      child: TabBar(
        labelColor: colors.primary,
        unselectedLabelColor: colors.textSecondary,
        indicatorColor: colors.primary,
        indicatorSize: TabBarIndicatorSize.tab,
        labelStyle: const TextStyle(fontSize: 11, fontWeight: FontWeight.w600),
        unselectedLabelStyle: const TextStyle(fontSize: 11, fontWeight: FontWeight.w500),
        tabs: const [
          Tab(text: 'Tonight'),
          Tab(text: 'Catalog'),
          Tab(text: 'Search'),
          Tab(text: 'Info'),
        ],
      ),
    );
  }
}

class _TonightTab extends ConsumerWidget {
  final NightshadeColors colors;

  const _TonightTab({required this.colors});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final twilight = ref.watch(twilightTimesProvider);
    final moonInfo = ref.watch(moonInfoProvider);
    final bestTargets = ref.watch(bestTargetsProvider);
    final location = ref.watch(observerLocationProvider);
    final settingsAsync = ref.watch(appSettingsProvider);
    
    // Check if using default location (no location set in settings)
    final settings = settingsAsync.valueOrNull;
    final isDefaultLocation = settings == null || 
        (settings.latitude == 0.0 && settings.longitude == 0.0);

    return SingleChildScrollView(
      padding: const EdgeInsets.all(16),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          // Location indicator
          Container(
            padding: const EdgeInsets.all(12),
            decoration: BoxDecoration(
              color: isDefaultLocation 
                  ? colors.warning.withValues(alpha: 0.1)
                  : colors.success.withValues(alpha: 0.1),
              borderRadius: BorderRadius.circular(10),
              border: Border.all(
                color: isDefaultLocation 
                    ? colors.warning.withValues(alpha: 0.3)
                    : colors.success.withValues(alpha: 0.3),
              ),
            ),
            child: Row(
              children: [
                Icon(
                  LucideIcons.mapPin,
                  size: 14,
                  color: isDefaultLocation ? colors.warning : colors.success,
                ),
                const SizedBox(width: 8),
                Expanded(
                  child: Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: [
                      Text(
                        isDefaultLocation 
                            ? 'Using default location'
                            : location.locationName ?? 'Custom Location',
                        style: TextStyle(
                          fontSize: 11,
                          fontWeight: FontWeight.w600,
                          color: isDefaultLocation ? colors.warning : colors.success,
                        ),
                      ),
                      Text(
                        '${location.latitude.toStringAsFixed(2)}°N, ${location.longitude.abs().toStringAsFixed(2)}°${location.longitude >= 0 ? 'E' : 'W'}',
                        style: TextStyle(
                          fontSize: 10,
                          color: isDefaultLocation 
                              ? colors.warning.withValues(alpha: 0.8) 
                              : colors.textMuted,
                        ),
                      ),
                    ],
                  ),
                ),
                if (isDefaultLocation)
                  GestureDetector(
                    onTap: () {
                      try {
                        context.goNamed('settings');
                      } catch (e) {
                        // Router might not be available, ignore
                      }
                    },
                    child: Container(
                      padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 4),
                      decoration: BoxDecoration(
                        color: colors.warning.withValues(alpha: 0.2),
                        borderRadius: BorderRadius.circular(4),
                      ),
                      child: Text(
                        'Set Location',
                        style: TextStyle(
                          fontSize: 10,
                          fontWeight: FontWeight.w500,
                          color: colors.warning,
                        ),
                      ),
                    ),
                  ),
              ],
            ),
          ),
          
          const SizedBox(height: 16),

          // Twilight card - Evening
          _InfoCard(
            title: 'Evening Twilight',
            icon: LucideIcons.sunset,
            color: colors.warning,
            colors: colors,
            child: Column(
              children: [
                if (twilight.sunset != null)
                  _TwilightRow(
                    label: 'Sunset',
                    time: DateFormat('HH:mm').format(twilight.sunset!.toLocal()),
                    colors: colors,
                  ),
                if (twilight.civilDusk != null)
                  _TwilightRow(
                    label: 'Civil Dusk',
                    time: DateFormat('HH:mm').format(twilight.civilDusk!.toLocal()),
                    colors: colors,
                  ),
                if (twilight.nauticalDusk != null)
                  _TwilightRow(
                    label: 'Nautical Dusk',
                    time: DateFormat('HH:mm').format(twilight.nauticalDusk!.toLocal()),
                    colors: colors,
                  ),
                if (twilight.astronomicalDusk != null)
                  _TwilightRow(
                    label: 'Astro Dusk',
                    time: DateFormat('HH:mm').format(twilight.astronomicalDusk!.toLocal()),
                    isPrimary: true,
                    colors: colors,
                  ),
              ],
            ),
          ),
          
          const SizedBox(height: 16),
          
          // Darkness duration card
          if (twilight.astronomicalDusk != null && twilight.astronomicalDawn != null)
            _DarknessCard(
              twilight: twilight,
              colors: colors,
            ),
          
          const SizedBox(height: 16),
          
          // Morning Twilight card
          _InfoCard(
            title: 'Morning Twilight',
            icon: LucideIcons.sunrise,
            color: const Color(0xFFFF9F45),
            colors: colors,
            child: Column(
              children: [
                if (twilight.astronomicalDawn != null)
                  _TwilightRow(
                    label: 'Astro Dawn',
                    time: DateFormat('HH:mm').format(twilight.astronomicalDawn!.toLocal()),
                    isPrimary: true,
                    colors: colors,
                  ),
                if (twilight.nauticalDawn != null)
                  _TwilightRow(
                    label: 'Nautical Dawn',
                    time: DateFormat('HH:mm').format(twilight.nauticalDawn!.toLocal()),
                    colors: colors,
                  ),
                if (twilight.civilDawn != null)
                  _TwilightRow(
                    label: 'Civil Dawn',
                    time: DateFormat('HH:mm').format(twilight.civilDawn!.toLocal()),
                    colors: colors,
                  ),
                if (twilight.sunrise != null)
                  _TwilightRow(
                    label: 'Sunrise',
                    time: DateFormat('HH:mm').format(twilight.sunrise!.toLocal()),
                    colors: colors,
                  ),
              ],
            ),
          ),

          const SizedBox(height: 16),

          // Moon card
          _InfoCard(
            title: 'Moon',
            icon: LucideIcons.moon,
            color: colors.info,
            colors: colors,
            child: Column(
              children: [
                Row(
                  mainAxisAlignment: MainAxisAlignment.spaceBetween,
                  children: [
                    Text(
                      'Phase',
                      style: TextStyle(fontSize: 12, color: colors.textSecondary),
                    ),
                    Text(
                      moonInfo.phaseName,
                      style: TextStyle(
                        fontSize: 13,
                        fontWeight: FontWeight.w600,
                        color: colors.textPrimary,
                      ),
                    ),
                  ],
                ),
                const SizedBox(height: 8),
                Row(
                  mainAxisAlignment: MainAxisAlignment.spaceBetween,
                  children: [
                    Text(
                      'Illumination',
                      style: TextStyle(fontSize: 12, color: colors.textSecondary),
                    ),
                    Text(
                      '${moonInfo.illumination.toStringAsFixed(0)}%',
                      style: TextStyle(
                        fontSize: 13,
                        fontWeight: FontWeight.w600,
                        color: moonInfo.illumination < 25 
                            ? colors.success 
                            : moonInfo.illumination > 75 
                                ? colors.error 
                                : colors.warning,
                      ),
                    ),
                  ],
                ),
                const SizedBox(height: 8),
                if (moonInfo.moonrise != null)
                  _TwilightRow(
                    label: 'Moonrise',
                    time: DateFormat('HH:mm').format(moonInfo.moonrise!.toLocal()),
                    colors: colors,
                  ),
                if (moonInfo.moonset != null)
                  _TwilightRow(
                    label: 'Moonset',
                    time: DateFormat('HH:mm').format(moonInfo.moonset!.toLocal()),
                    colors: colors,
                  ),
              ],
            ),
          ),

          const SizedBox(height: 16),

          // Best targets tonight header
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              Text(
                'Best Targets Tonight',
                style: TextStyle(
                  fontSize: 13,
                  fontWeight: FontWeight.w600,
                  color: colors.textPrimary,
                ),
              ),
              Tooltip(
                message: 'Objects sorted by transit altitude (>30°)',
                child: Icon(
                  LucideIcons.helpCircle,
                  size: 14,
                  color: colors.textMuted,
                ),
              ),
            ],
          ),
          const SizedBox(height: 12),

          bestTargets.when(
            data: (targets) {
              if (targets.isEmpty) {
                return Container(
                  padding: const EdgeInsets.all(16),
                  decoration: BoxDecoration(
                    color: colors.surfaceAlt,
                    borderRadius: BorderRadius.circular(10),
                    border: Border.all(color: colors.border),
                  ),
                  child: Column(
                    children: [
                      Icon(LucideIcons.cloudOff, size: 32, color: colors.textMuted),
                      const SizedBox(height: 8),
                      Text(
                        'No targets above 30° tonight',
                        style: TextStyle(
                          fontSize: 12,
                          color: colors.textMuted,
                        ),
                      ),
                      if (isDefaultLocation) ...[
                        const SizedBox(height: 4),
                        Text(
                          'Try setting your actual location',
                          style: TextStyle(
                            fontSize: 11,
                            color: colors.textMuted,
                          ),
                        ),
                      ],
                    ],
                  ),
                );
              }
              return Column(
                children: targets.take(5).map((item) {
                  final (dso, visibility) = item;
                  final (displayName, catalogTag) = getDsoDisplayInfo(dso);
                  return _TargetCard(
                    name: displayName,
                    catalog: catalogTag,
                    type: _dsoTypeName(dso.type),
                    altitude: '${visibility.transitAltitude?.toStringAsFixed(0) ?? '-'}°',
                    transit: visibility.transitTime != null 
                        ? DateFormat('HH:mm').format(visibility.transitTime!)
                        : '-',
                    colors: colors,
                    onTap: () {
                      ref.read(selectedObjectProvider.notifier).selectObject(dso);
                      ref.read(skyViewStateProvider.notifier).lookAt(dso.coordinates);
                    },
                  );
                }).toList(),
              );
            },
            loading: () => Container(
              padding: const EdgeInsets.all(24),
              child: const Center(
                child: CircularProgressIndicator(strokeWidth: 2),
              ),
            ),
            error: (e, _) => Container(
              padding: const EdgeInsets.all(12),
              decoration: BoxDecoration(
                color: colors.error.withValues(alpha: 0.1),
                borderRadius: BorderRadius.circular(8),
              ),
              child: Row(
                children: [
                  Icon(LucideIcons.alertCircle, size: 16, color: colors.error),
                  const SizedBox(width: 8),
                  Expanded(
                    child: Text(
                      'Error loading targets',
                      style: TextStyle(fontSize: 12, color: colors.error),
                    ),
                  ),
                ],
              ),
            ),
          ),
        ],
      ),
    );
  }
  
  String _dsoTypeName(DsoType type) {
    switch (type) {
      case DsoType.galaxy:
        return 'Galaxy';
      case DsoType.nebula:
        return 'Nebula';
      case DsoType.openCluster:
        return 'Open Cluster';
      case DsoType.globularCluster:
        return 'Globular Cluster';
      case DsoType.planetaryNebula:
        return 'Planetary Nebula';
      case DsoType.supernova:
        return 'Supernova Remnant';
      default:
        return 'DSO';
    }
  }
  
}

class _ObjectsTab extends ConsumerWidget {
  final NightshadeColors colors;

  const _ObjectsTab({required this.colors});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final dsos = ref.watch(loadedDsosProvider);

    return dsos.when(
      data: (objects) => ListView.builder(
        padding: const EdgeInsets.all(16),
        itemCount: objects.length,
        itemBuilder: (context, index) {
          final dso = objects[index];
          final (displayName, catalogTag) = getDsoDisplayInfo(dso);
          return _TargetCard(
            name: displayName,
            catalog: catalogTag,
            type: _dsoTypeName(dso.type),
            altitude: dso.magnitude?.toStringAsFixed(1) ?? '-',
            transit: 'mag',
            colors: colors,
            onTap: () {
              ref.read(selectedObjectProvider.notifier).selectObject(dso);
              ref.read(skyViewStateProvider.notifier).lookAt(dso.coordinates);
            },
          );
        },
      ),
      loading: () => const Center(child: CircularProgressIndicator()),
      error: (e, _) => Center(
        child: Text('Error: $e', style: TextStyle(color: colors.error)),
      ),
    );
  }
  
  String _dsoTypeName(DsoType type) {
    switch (type) {
      case DsoType.galaxy:
        return 'Galaxy';
      case DsoType.nebula:
        return 'Nebula';
      case DsoType.openCluster:
        return 'Open Cluster';
      case DsoType.globularCluster:
        return 'Globular Cluster';
      case DsoType.planetaryNebula:
        return 'Planetary Nebula';
      case DsoType.supernova:
        return 'Supernova Remnant';
      default:
        return 'DSO';
    }
  }
  
}

class _SearchResultsTab extends ConsumerWidget {
  final NightshadeColors colors;

  const _SearchResultsTab({required this.colors});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final searchState = ref.watch(objectSearchProvider);

    if (searchState.query.isEmpty) {
      return Center(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            Icon(LucideIcons.search, size: 48, color: colors.textMuted),
            const SizedBox(height: 16),
            Text(
              'Search for objects',
              style: TextStyle(color: colors.textMuted),
            ),
            const SizedBox(height: 8),
            Text(
              'Try "M42", "Orion", or "Sirius"',
              style: TextStyle(fontSize: 12, color: colors.textMuted),
            ),
          ],
        ),
      );
    }

    if (searchState.isSearching) {
      return const Center(child: CircularProgressIndicator());
    }

    if (searchState.results.isEmpty) {
      return Center(
        child: Text(
          'No results for "${searchState.query}"',
          style: TextStyle(color: colors.textMuted),
        ),
      );
    }

    return ListView.builder(
      padding: const EdgeInsets.all(16),
      itemCount: searchState.results.length,
      itemBuilder: (context, index) {
        final obj = searchState.results[index];
        return _SearchResultCard(
          object: obj,
          colors: colors,
          onTap: () {
            ref.read(selectedObjectProvider.notifier).selectObject(obj);
            ref.read(skyViewStateProvider.notifier).lookAt(obj.coordinates);
          },
        );
      },
    );
  }
}

class _SearchResultCard extends StatelessWidget {
  final CelestialObject object;
  final NightshadeColors colors;
  final VoidCallback onTap;

  const _SearchResultCard({
    required this.object,
    required this.colors,
    required this.onTap,
  });

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: onTap,
      child: Container(
        margin: const EdgeInsets.only(bottom: 8),
        padding: const EdgeInsets.all(12),
        decoration: BoxDecoration(
          color: colors.surfaceAlt,
          borderRadius: BorderRadius.circular(8),
          border: Border.all(color: colors.border),
        ),
        child: Row(
          children: [
            Icon(
              object is Star ? LucideIcons.star : LucideIcons.circle,
              size: 16,
              color: object is Star ? Colors.yellow : colors.primary,
            ),
            const SizedBox(width: 12),
            Expanded(
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Text(
                    object is DeepSkyObject 
                        ? getDsoDisplayInfo(object as DeepSkyObject).$1 
                        : object.name,
                    style: TextStyle(
                      fontSize: 13,
                      fontWeight: FontWeight.w600,
                      color: colors.textPrimary,
                    ),
                  ),
                  Text(
                    object is DeepSkyObject 
                        ? getDsoDisplayInfo(object as DeepSkyObject).$2 
                        : object.id,
                    style: TextStyle(fontSize: 11, color: colors.textMuted),
                  ),
                ],
              ),
            ),
            if (object.magnitude != null)
              Text(
                'mag ${object.magnitude!.toStringAsFixed(1)}',
                style: TextStyle(fontSize: 11, color: colors.textSecondary),
              ),
          ],
        ),
      ),
    );
  }
}

class _InfoTab extends ConsumerWidget {
  final NightshadeColors colors;
  final SelectedObjectState selectedObject;

  const _InfoTab({
    required this.colors,
    required this.selectedObject,
  });

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    if (selectedObject.object == null && selectedObject.coordinates == null) {
      return Center(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            Icon(LucideIcons.info, size: 48, color: colors.textMuted),
            const SizedBox(height: 16),
            Text(
              'Select an object',
              style: TextStyle(color: colors.textMuted),
            ),
            const SizedBox(height: 8),
            Text(
              'Click on the sky to select',
              style: TextStyle(fontSize: 12, color: colors.textMuted),
            ),
          ],
        ),
      );
    }

    final obj = selectedObject.object;

    // If we have a celestial object, use the new ObjectDetailsPanel
    if (obj != null) {
      return Padding(
        padding: const EdgeInsets.all(8),
        child: ObjectDetailsPanel(
          object: obj,
          backgroundColor: colors.surfaceAlt,
          textColor: colors.textPrimary,
          accentColor: colors.accent,
          showVisibilityGraph: true,
          onGoTo: () {
            // Slew to object
            final coords = obj.coordinates;
            ref.read(deviceServiceProvider).slewMountToCoordinates(
              coords.ra,
              coords.dec,
            ).then((_) {
              ScaffoldMessenger.of(context).showSnackBar(
                SnackBar(content: Text('Slewing to ${obj.name}...')),
              );
            }).catchError((e) {
              ScaffoldMessenger.of(context).showSnackBar(
                SnackBar(content: Text('Slew failed: $e')),
              );
            });
          },
          onAddToTargets: () {
            // Add to sequencer
            final coords = obj.coordinates;
            ref.read(currentSequenceProvider.notifier).addTargetHeader(
              TargetHeaderNode(
                targetName: obj.name,
                raHours: coords.ra,
                decDegrees: coords.dec,
              ),
            );
            ScaffoldMessenger.of(context).showSnackBar(
              SnackBar(content: Text('Added ${obj.name} to sequence')),
            );
          },
        ),
      );
    }

    // Fallback for coordinates-only selection (rare case)
    final coords = selectedObject.coordinates;
    final altAz = selectedObject.currentAltAz;

    return SingleChildScrollView(
      padding: const EdgeInsets.all(16),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Text(
            'Selected Coordinates',
            style: TextStyle(
              fontSize: 18,
              fontWeight: FontWeight.bold,
              color: colors.textPrimary,
            ),
          ),
          const SizedBox(height: 16),
          _InfoCard(
            title: 'Coordinates',
            icon: LucideIcons.compass,
            color: colors.info,
            colors: colors,
            child: Column(
              children: [
                if (coords != null) ...[
                  _InfoRow(label: 'RA', value: coords.toString().split(',')[0].replaceAll('RA: ', ''), colors: colors),
                  _InfoRow(label: 'Dec', value: coords.toString().split(',')[1].replaceAll(' Dec: ', ''), colors: colors),
                ],
                if (altAz != null) ...[
                  _InfoRow(
                    label: 'Altitude',
                    value: '${altAz.$1.toStringAsFixed(1)}°',
                    colors: colors,
                    valueColor: altAz.$1 > 30
                        ? colors.success
                        : altAz.$1 > 0
                            ? colors.warning
                            : colors.error,
                  ),
                  _InfoRow(label: 'Azimuth', value: '${altAz.$2.toStringAsFixed(1)}°', colors: colors),
                ],
              ],
            ),
          ),
        ],
      ),
    );
  }
}

class _InfoRow extends StatelessWidget {
  final String label;
  final String value;
  final NightshadeColors colors;
  final Color? valueColor;

  const _InfoRow({
    required this.label,
    required this.value,
    required this.colors,
    this.valueColor,
  });

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.symmetric(vertical: 4),
      child: Row(
        mainAxisAlignment: MainAxisAlignment.spaceBetween,
        children: [
          Text(
            label,
            style: TextStyle(fontSize: 12, color: colors.textSecondary),
          ),
          Text(
            value,
            style: TextStyle(
              fontSize: 12,
              fontWeight: FontWeight.w500,
              color: valueColor ?? colors.textPrimary,
              fontFeatures: const [FontFeature.tabularFigures()],
            ),
          ),
        ],
      ),
    );
  }
}

class _InfoCard extends StatelessWidget {
  final String title;
  final IconData icon;
  final Color color;
  final Widget child;
  final NightshadeColors colors;

  const _InfoCard({
    required this.title,
    required this.icon,
    required this.color,
    required this.child,
    required this.colors,
  });

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.all(16),
      decoration: BoxDecoration(
        color: colors.surfaceAlt,
        borderRadius: BorderRadius.circular(12),
        border: Border.all(color: colors.border),
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Row(
            children: [
              Container(
                padding: const EdgeInsets.all(6),
                decoration: BoxDecoration(
                  color: color.withValues(alpha: 0.15),
                  borderRadius: BorderRadius.circular(6),
                ),
                child: Icon(icon, size: 14, color: color),
              ),
              const SizedBox(width: 10),
              Text(
                title,
                style: TextStyle(
                  fontSize: 13,
                  fontWeight: FontWeight.w600,
                  color: colors.textPrimary,
                ),
              ),
            ],
          ),
          const SizedBox(height: 12),
          child,
        ],
      ),
    );
  }
}

class _TwilightRow extends StatelessWidget {
  final String label;
  final String time;
  final bool isPrimary;
  final NightshadeColors colors;

  const _TwilightRow({
    required this.label,
    required this.time,
    this.isPrimary = false,
    required this.colors,
  });

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.symmetric(vertical: 4),
      child: Row(
        mainAxisAlignment: MainAxisAlignment.spaceBetween,
        children: [
          Text(
            label,
            style: TextStyle(
              fontSize: 12,
              color: isPrimary ? colors.textPrimary : colors.textSecondary,
              fontWeight: isPrimary ? FontWeight.w500 : FontWeight.normal,
            ),
          ),
          Text(
            time,
            style: TextStyle(
              fontSize: 12,
              fontWeight: isPrimary ? FontWeight.w600 : FontWeight.w500,
              color: isPrimary ? colors.primary : colors.textPrimary,
              fontFeatures: const [FontFeature.tabularFigures()],
            ),
          ),
        ],
      ),
    );
  }
}

/// Darkness duration card showing total imaging time
class _DarknessCard extends StatelessWidget {
  final TwilightTimes twilight;
  final NightshadeColors colors;

  const _DarknessCard({
    required this.twilight,
    required this.colors,
  });

  @override
  Widget build(BuildContext context) {
    final duration = twilight.darknessDuration;
    if (duration == null) return const SizedBox.shrink();
    
    final hours = duration.inHours;
    final minutes = duration.inMinutes % 60;
    
    return Container(
      padding: const EdgeInsets.all(14),
      decoration: BoxDecoration(
        gradient: const LinearGradient(
          begin: Alignment.topLeft,
          end: Alignment.bottomRight,
          colors: [
            Color(0xFF1A1A2E),
            Color(0xFF16213E),
          ],
        ),
        borderRadius: BorderRadius.circular(12),
        border: Border.all(color: colors.primary.withValues(alpha: 0.3)),
      ),
      child: Column(
        children: [
          Row(
            children: [
              Container(
                padding: const EdgeInsets.all(8),
                decoration: BoxDecoration(
                  color: colors.primary.withValues(alpha: 0.15),
                  borderRadius: BorderRadius.circular(8),
                ),
                child: Icon(LucideIcons.moon, size: 16, color: colors.primary),
              ),
              const SizedBox(width: 12),
              Expanded(
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Text(
                      'Total Darkness',
                      style: TextStyle(
                        fontSize: 11,
                        color: colors.textMuted,
                      ),
                    ),
                    const SizedBox(height: 2),
                    Text(
                      '${hours}h ${minutes.toString().padLeft(2, '0')}m',
                      style: TextStyle(
                        fontSize: 20,
                        fontWeight: FontWeight.bold,
                        color: colors.primary,
                        fontFeatures: const [FontFeature.tabularFigures()],
                      ),
                    ),
                  ],
                ),
              ),
            ],
          ),
          const SizedBox(height: 12),
          Container(
            height: 4,
            decoration: BoxDecoration(
              color: colors.surfaceAlt,
              borderRadius: BorderRadius.circular(2),
            ),
            child: Row(
              children: [
                Expanded(
                  flex: hours,
                  child: Container(
                    decoration: BoxDecoration(
                      color: colors.primary,
                      borderRadius: BorderRadius.circular(2),
                    ),
                  ),
                ),
                if (hours < 12)
                  Expanded(
                    flex: 12 - hours,
                    child: const SizedBox(),
                  ),
              ],
            ),
          ),
          const SizedBox(height: 8),
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              Text(
                'Astro Dusk: ${DateFormat('HH:mm').format(twilight.astronomicalDusk!.toLocal())}',
                style: TextStyle(fontSize: 10, color: colors.textMuted),
              ),
              Text(
                'Astro Dawn: ${DateFormat('HH:mm').format(twilight.astronomicalDawn!.toLocal())}',
                style: TextStyle(fontSize: 10, color: colors.textMuted),
              ),
            ],
          ),
        ],
      ),
    );
  }
}

class _TargetCard extends StatefulWidget {
  final String name;
  final String catalog;
  final String type;
  final String altitude;
  final String transit;
  final NightshadeColors colors;
  final VoidCallback? onTap;

  const _TargetCard({
    required this.name,
    required this.catalog,
    required this.type,
    required this.altitude,
    required this.transit,
    required this.colors,
    this.onTap,
  });

  @override
  State<_TargetCard> createState() => _TargetCardState();
}

class _TargetCardState extends State<_TargetCard> {
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
          margin: const EdgeInsets.only(bottom: 8),
          padding: const EdgeInsets.all(12),
          decoration: BoxDecoration(
            color: _isHovered ? widget.colors.surfaceAlt : widget.colors.background,
            borderRadius: BorderRadius.circular(10),
            border: Border.all(
              color: _isHovered ? widget.colors.primary.withValues(alpha: 0.5) : widget.colors.border,
            ),
          ),
          child: Row(
            children: [
              Expanded(
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Row(
                      children: [
                        Text(
                          widget.name,
                          style: TextStyle(
                            fontSize: 13,
                            fontWeight: FontWeight.w600,
                            color: widget.colors.textPrimary,
                          ),
                        ),
                        const SizedBox(width: 8),
                        Container(
                          padding: const EdgeInsets.symmetric(horizontal: 6, vertical: 2),
                          decoration: BoxDecoration(
                            color: widget.colors.primary.withValues(alpha: 0.15),
                            borderRadius: BorderRadius.circular(4),
                          ),
                          child: Text(
                            widget.catalog,
                            style: TextStyle(
                              fontSize: 10,
                              fontWeight: FontWeight.w600,
                              color: widget.colors.primary,
                            ),
                          ),
                        ),
                      ],
                    ),
                    const SizedBox(height: 4),
                    Text(
                      widget.type,
                      style: TextStyle(
                        fontSize: 11,
                        color: widget.colors.textMuted,
                      ),
                    ),
                  ],
                ),
              ),
              Column(
                crossAxisAlignment: CrossAxisAlignment.end,
                children: [
                  Text(
                    widget.altitude,
                    style: TextStyle(
                      fontSize: 13,
                      fontWeight: FontWeight.w600,
                      color: widget.colors.success,
                    ),
                  ),
                  Text(
                    widget.transit,
                    style: TextStyle(
                      fontSize: 10,
                      color: widget.colors.textMuted,
                    ),
                  ),
                ],
              ),
            ],
          ),
        ),
      ),
    );
  }
}

// =============================================================================
// OBJECT INFO POPUP
// =============================================================================

class _ObjectInfoPopup extends StatefulWidget {
  final NightshadeColors colors;
  final CelestialObject object;
  final CelestialCoordinate coordinates;
  final SelectedObjectState selectedObjectState;
  final Offset position;
  final VoidCallback onDismiss;
  final VoidCallback onSendToFraming;
  final VoidCallback onAddToSequencer;
  final VoidCallback onSlewToTarget;

  const _ObjectInfoPopup({
    required this.colors,
    required this.object,
    required this.coordinates,
    required this.selectedObjectState,
    required this.position,
    required this.onDismiss,
    required this.onSendToFraming,
    required this.onAddToSequencer,
    required this.onSlewToTarget,
  });

  @override
  State<_ObjectInfoPopup> createState() => _ObjectInfoPopupState();
}

class _ObjectInfoPopupState extends State<_ObjectInfoPopup>
    with SingleTickerProviderStateMixin {
  late AnimationController _animationController;
  late Animation<double> _scaleAnimation;
  late Animation<double> _fadeAnimation;

  @override
  void initState() {
    super.initState();
    _animationController = AnimationController(
      vsync: this,
      duration: const Duration(milliseconds: 200),
    );

    _scaleAnimation = Tween<double>(begin: 0.8, end: 1.0).animate(
      CurvedAnimation(parent: _animationController, curve: Curves.easeOutBack),
    );

    _fadeAnimation = Tween<double>(begin: 0.0, end: 1.0).animate(
      CurvedAnimation(parent: _animationController, curve: Curves.easeOut),
    );

    _animationController.forward();
  }

  @override
  void dispose() {
    _animationController.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final screenSize = MediaQuery.of(context).size;
    const popupWidth = 300.0;
    const popupHeight = 320.0;

    // Calculate position to keep popup on screen
    double left = widget.position.dx - popupWidth / 2;
    double top = widget.position.dy + 20; // Offset below the click

    // Clamp to screen bounds with padding
    const padding = 16.0;
    left = left.clamp(padding, screenSize.width - popupWidth - padding);
    
    // If popup would go below screen, show it above the click point
    if (top + popupHeight > screenSize.height - padding) {
      top = widget.position.dy - popupHeight - 20;
    }
    top = top.clamp(padding, screenSize.height - popupHeight - padding);

    // Determine if showing above or below click point for arrow direction
    final showAbove = top < widget.position.dy;

    return Positioned(
      left: left,
      top: top,
      child: AnimatedBuilder(
        animation: _animationController,
        builder: (context, child) {
          return Opacity(
            opacity: _fadeAnimation.value,
            child: Transform.scale(
              scale: _scaleAnimation.value,
              alignment: showAbove ? Alignment.bottomCenter : Alignment.topCenter,
              child: child,
            ),
          );
        },
        child: Material(
          color: Colors.transparent,
          child: GestureDetector(
            onTap: () {}, // Prevent tap-through
            child: Container(
              width: popupWidth,
              constraints: const BoxConstraints(maxHeight: popupHeight),
              decoration: BoxDecoration(
                color: const Color(0xFF1A1A24).withValues(alpha: 0.95),
                borderRadius: BorderRadius.circular(16),
                border: Border.all(
                  color: widget.colors.primary.withValues(alpha: 0.3),
                  width: 1,
                ),
                boxShadow: [
                  BoxShadow(
                    color: Colors.black.withValues(alpha: 0.5),
                    blurRadius: 24,
                    spreadRadius: 4,
                  ),
                  BoxShadow(
                    color: widget.colors.primary.withValues(alpha: 0.1),
                    blurRadius: 40,
                    spreadRadius: 2,
                  ),
                ],
              ),
              child: ClipRRect(
                borderRadius: BorderRadius.circular(16),
                child: BackdropFilter(
                  filter: ui.ImageFilter.blur(sigmaX: 10, sigmaY: 10),
                  child: Column(
                    mainAxisSize: MainAxisSize.min,
                    crossAxisAlignment: CrossAxisAlignment.stretch,
                    children: [
                      // Header
                      _buildHeader(),
                      
                      // Divider
                      Container(
                        height: 1,
                        color: widget.colors.border.withValues(alpha: 0.5),
                      ),
                      
                      // Content
                      Flexible(
                        child: SingleChildScrollView(
                          padding: const EdgeInsets.all(16),
                          child: Column(
                            crossAxisAlignment: CrossAxisAlignment.start,
                            children: [
                              _buildObjectDetails(),
                              const SizedBox(height: 16),
                              _buildCoordinates(),
                              if (widget.selectedObjectState.currentAltAz != null) ...[
                                const SizedBox(height: 12),
                                _buildAltAz(),
                              ],
                            ],
                          ),
                        ),
                      ),
                      
                      // Divider
                      Container(
                        height: 1,
                        color: widget.colors.border.withValues(alpha: 0.5),
                      ),
                      
                      // Action buttons
                      _buildActions(),
                    ],
                  ),
                ),
              ),
            ),
          ),
        ),
      ),
    );
  }

  Widget _buildHeader() {
    final obj = widget.object;
    IconData icon;
    Color iconColor;

    if (obj is Star) {
      icon = LucideIcons.star;
      iconColor = Colors.amber;
    } else if (obj is DeepSkyObject) {
      final dso = obj;
      if (dso.type.isGalaxy) {
        icon = LucideIcons.circle;
        iconColor = widget.colors.info;
      } else if (dso.type.isNebula) {
        icon = LucideIcons.cloud;
        iconColor = widget.colors.error;
      } else if (dso.type.isCluster) {
        icon = LucideIcons.sparkles;
        iconColor = widget.colors.warning;
      } else {
        icon = LucideIcons.target;
        iconColor = widget.colors.primary;
      }
    } else {
      icon = LucideIcons.target;
      iconColor = widget.colors.primary;
    }

    return Container(
      padding: const EdgeInsets.all(16),
      child: Row(
        children: [
          Container(
            padding: const EdgeInsets.all(10),
            decoration: BoxDecoration(
              color: iconColor.withValues(alpha: 0.15),
              borderRadius: BorderRadius.circular(10),
            ),
            child: Icon(icon, size: 20, color: iconColor),
          ),
          const SizedBox(width: 12),
          Expanded(
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Text(
                    obj is DeepSkyObject 
                        ? getDsoDisplayInfo(obj).$1 
                        : obj.name,
                    style: const TextStyle(
                      fontSize: 16,
                      fontWeight: FontWeight.bold,
                      color: Colors.white,
                    ),
                    overflow: TextOverflow.ellipsis,
                  ),
                  const SizedBox(height: 2),
                  Row(
                    children: [
                      Container(
                        padding: const EdgeInsets.symmetric(horizontal: 6, vertical: 2),
                        decoration: BoxDecoration(
                          color: widget.colors.primary.withValues(alpha: 0.2),
                          borderRadius: BorderRadius.circular(4),
                        ),
                        child: Text(
                          obj is DeepSkyObject 
                              ? getDsoDisplayInfo(obj).$2 
                              : obj.id,
                          style: TextStyle(
                            fontSize: 10,
                            fontWeight: FontWeight.w600,
                            color: widget.colors.primary,
                          ),
                        ),
                      ),
                    if (obj.magnitude != null) ...[
                      const SizedBox(width: 8),
                      Text(
                        'mag ${obj.magnitude!.toStringAsFixed(1)}',
                        style: const TextStyle(
                          fontSize: 11,
                          color: Colors.white60,
                        ),
                      ),
                    ],
                  ],
                ),
              ],
            ),
          ),
          // Close button
          GestureDetector(
            onTap: widget.onDismiss,
            child: Container(
              padding: const EdgeInsets.all(6),
              decoration: BoxDecoration(
                color: Colors.white.withValues(alpha: 0.1),
                borderRadius: BorderRadius.circular(6),
              ),
              child: const Icon(LucideIcons.x, size: 14, color: Colors.white60),
            ),
          ),
        ],
      ),
    );
  }

  Widget _buildObjectDetails() {
    final obj = widget.object;
    String typeLabel = 'Object';

    if (obj is Star) {
      typeLabel = 'Star';
      if (obj.spectralType != null) {
        typeLabel = 'Star (${obj.spectralType})';
      }
    } else if (obj is DeepSkyObject) {
      typeLabel = obj.type.displayName;
    }

    return Row(
      children: [
        _PopupInfoChip(
          label: 'Type',
          value: typeLabel,
          colors: widget.colors,
        ),
        const SizedBox(width: 8),
        if (obj is DeepSkyObject && obj.sizeString != null)
          _PopupInfoChip(
            label: 'Size',
            value: obj.sizeString!,
            colors: widget.colors,
          ),
      ],
    );
  }

  Widget _buildCoordinates() {
    final coords = widget.coordinates;
    
    // Format RA
    final raH = coords.ra.floor();
    final raM = ((coords.ra - raH) * 60).floor();
    final raS = (((coords.ra - raH) * 60 - raM) * 60).toStringAsFixed(1);
    final raStr = '${raH}h ${raM}m ${raS}s';
    
    // Format Dec
    final sign = coords.dec >= 0 ? '+' : '-';
    final decD = coords.dec.abs().floor();
    final decM = ((coords.dec.abs() - decD) * 60).floor();
    final decS = (((coords.dec.abs() - decD) * 60 - decM) * 60).toStringAsFixed(0);
    final decStr = "$sign$decD° $decM' $decS\"";

    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        const Text(
          'Coordinates',
          style: TextStyle(
            fontSize: 10,
            fontWeight: FontWeight.w600,
            color: Colors.white38,
            letterSpacing: 0.5,
          ),
        ),
        const SizedBox(height: 6),
        Row(
          children: [
            Expanded(
              child: _PopupCoordRow(
                label: 'RA',
                value: raStr,
                colors: widget.colors,
              ),
            ),
            const SizedBox(width: 16),
            Expanded(
              child: _PopupCoordRow(
                label: 'Dec',
                value: decStr,
                colors: widget.colors,
              ),
            ),
          ],
        ),
      ],
    );
  }

  Widget _buildAltAz() {
    final altAz = widget.selectedObjectState.currentAltAz!;
    final alt = altAz.$1;
    final az = altAz.$2;

    Color altColor;
    String statusText;
    if (alt > 30) {
      altColor = widget.colors.success;
      statusText = 'Excellent';
    } else if (alt > 15) {
      altColor = widget.colors.warning;
      statusText = 'Good';
    } else if (alt > 0) {
      altColor = widget.colors.warning;
      statusText = 'Low';
    } else {
      altColor = widget.colors.error;
      statusText = 'Below Horizon';
    }

    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        const Text(
          'Current Position',
          style: TextStyle(
            fontSize: 10,
            fontWeight: FontWeight.w600,
            color: Colors.white38,
            letterSpacing: 0.5,
          ),
        ),
        const SizedBox(height: 6),
        Row(
          children: [
            Container(
              padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 6),
              decoration: BoxDecoration(
                color: altColor.withValues(alpha: 0.15),
                borderRadius: BorderRadius.circular(6),
                border: Border.all(color: altColor.withValues(alpha: 0.3)),
              ),
              child: Row(
                mainAxisSize: MainAxisSize.min,
                children: [
                  Icon(
                    alt > 0 ? LucideIcons.arrowUp : LucideIcons.arrowDown,
                    size: 12,
                    color: altColor,
                  ),
                  const SizedBox(width: 4),
                  Text(
                    '${alt.toStringAsFixed(1)}°',
                    style: TextStyle(
                      fontSize: 12,
                      fontWeight: FontWeight.w600,
                      color: altColor,
                      fontFeatures: const [FontFeature.tabularFigures()],
                    ),
                  ),
                ],
              ),
            ),
            const SizedBox(width: 8),
            Text(
              'Az ${az.toStringAsFixed(1)}°',
              style: const TextStyle(
                fontSize: 11,
                color: Colors.white60,
                fontFeatures: [FontFeature.tabularFigures()],
              ),
            ),
            const Spacer(),
            Container(
              padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 4),
              decoration: BoxDecoration(
                color: altColor.withValues(alpha: 0.1),
                borderRadius: BorderRadius.circular(4),
              ),
              child: Text(
                statusText,
                style: TextStyle(
                  fontSize: 10,
                  fontWeight: FontWeight.w500,
                  color: altColor,
                ),
              ),
            ),
          ],
        ),
      ],
    );
  }

  Widget _buildActions() {
    return Padding(
      padding: const EdgeInsets.all(12),
      child: Row(
        children: [
          Expanded(
            child: _PopupActionButton(
              icon: LucideIcons.crosshair,
              label: 'Slew',
              colors: widget.colors,
              isPrimary: true,
              onTap: widget.onSlewToTarget,
            ),
          ),
          const SizedBox(width: 8),
          Expanded(
            child: _PopupActionButton(
              icon: LucideIcons.frame,
              label: 'Framing',
              colors: widget.colors,
              onTap: widget.onSendToFraming,
            ),
          ),
          const SizedBox(width: 8),
          Expanded(
            child: _PopupActionButton(
              icon: LucideIcons.listPlus,
              label: 'Add to Sequence',
              colors: widget.colors,
              isPrimary: true,
              onTap: widget.onAddToSequencer,
            ),
          ),
        ],
      ),
    );
  }
}

class _PopupInfoChip extends StatelessWidget {
  final String label;
  final String value;
  final NightshadeColors colors;

  const _PopupInfoChip({
    required this.label,
    required this.value,
    required this.colors,
  });

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 6),
      decoration: BoxDecoration(
        color: Colors.white.withValues(alpha: 0.05),
        borderRadius: BorderRadius.circular(6),
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Text(
            label,
            style: const TextStyle(
              fontSize: 9,
              color: Colors.white38,
            ),
          ),
          const SizedBox(height: 2),
          Text(
            value,
            style: const TextStyle(
              fontSize: 11,
              fontWeight: FontWeight.w500,
              color: Colors.white70,
            ),
          ),
        ],
      ),
    );
  }
}

class _PopupCoordRow extends StatelessWidget {
  final String label;
  final String value;
  final NightshadeColors colors;

  const _PopupCoordRow({
    required this.label,
    required this.value,
    required this.colors,
  });

  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        Text(
          '$label: ',
          style: const TextStyle(
            fontSize: 11,
            color: Colors.white38,
          ),
        ),
        Expanded(
          child: Text(
            value,
            style: const TextStyle(
              fontSize: 11,
              fontWeight: FontWeight.w500,
              color: Colors.white70,
              fontFeatures: [FontFeature.tabularFigures()],
            ),
            overflow: TextOverflow.ellipsis,
          ),
        ),
      ],
    );
  }
}

class _PopupActionButton extends StatefulWidget {
  final IconData icon;
  final String label;
  final NightshadeColors colors;
  final bool isPrimary;
  final VoidCallback onTap;

  const _PopupActionButton({
    required this.icon,
    required this.label,
    required this.colors,
    this.isPrimary = false,
    required this.onTap,
  });

  @override
  State<_PopupActionButton> createState() => _PopupActionButtonState();
}

class _PopupActionButtonState extends State<_PopupActionButton> {
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
          padding: const EdgeInsets.symmetric(vertical: 10, horizontal: 12),
          decoration: BoxDecoration(
            gradient: widget.isPrimary
                ? LinearGradient(
                    colors: [
                      widget.colors.primary,
                      widget.colors.primary.withValues(alpha: 0.8),
                    ],
                  )
                : null,
            color: widget.isPrimary
                ? null
                : _isHovered
                    ? Colors.white.withValues(alpha: 0.1)
                    : Colors.white.withValues(alpha: 0.05),
            borderRadius: BorderRadius.circular(8),
            border: widget.isPrimary
                ? null
                : Border.all(
                    color: _isHovered
                        ? widget.colors.primary.withValues(alpha: 0.5)
                        : Colors.white.withValues(alpha: 0.1),
                  ),
            boxShadow: widget.isPrimary && _isHovered
                ? [
                    BoxShadow(
                      color: widget.colors.primary.withValues(alpha: 0.4),
                      blurRadius: 12,
                      offset: const Offset(0, 4),
                    ),
                  ]
                : null,
          ),
          child: Row(
            mainAxisAlignment: MainAxisAlignment.center,
            mainAxisSize: MainAxisSize.min,
            children: [
              Icon(
                widget.icon,
                size: 14,
                color: widget.isPrimary ? Colors.white : Colors.white70,
              ),
              const SizedBox(width: 6),
              Flexible(
                child: Text(
                  widget.label,
                  maxLines: 1,
                  overflow: TextOverflow.ellipsis,
                  softWrap: false,
                  style: TextStyle(
                    fontSize: 11,
                    fontWeight: FontWeight.w500,
                    color: widget.isPrimary ? Colors.white : Colors.white70,
                  ),
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}

class _SelectedObjectHud extends ConsumerWidget {
  final NightshadeColors colors;
  final VoidCallback onSlew;

  const _SelectedObjectHud({
    required this.colors,
    required this.onSlew,
  });

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final selectedState = ref.watch(selectedObjectProvider);
    final selectedObject = selectedState.object;

    if (selectedObject == null || selectedObject is! DeepSkyObject) return const SizedBox.shrink();

    final (displayName, catalogTag) = getDsoDisplayInfo(selectedObject);
    final typeName = selectedObject.type.toString().split('.').last;

    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 12),
      decoration: BoxDecoration(
        color: colors.surface.withValues(alpha: 0.9),
        borderRadius: BorderRadius.circular(12),
        border: Border.all(color: colors.border),
        boxShadow: [
          BoxShadow(
            color: Colors.black.withValues(alpha: 0.3),
            blurRadius: 10,
            offset: const Offset(0, 4),
          ),
        ],
      ),
      child: Row(
        mainAxisSize: MainAxisSize.min,
        children: [
          // Object Info
          Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            mainAxisSize: MainAxisSize.min,
            children: [
              Row(
                mainAxisSize: MainAxisSize.min,
                children: [
                  Container(
                    padding: const EdgeInsets.symmetric(horizontal: 6, vertical: 2),
                    decoration: BoxDecoration(
                      color: colors.primary.withValues(alpha: 0.2),
                      borderRadius: BorderRadius.circular(4),
                      border: Border.all(color: colors.primary.withValues(alpha: 0.3)),
                    ),
                    child: Text(
                      catalogTag,
                      style: TextStyle(
                        fontSize: 10,
                        fontWeight: FontWeight.bold,
                        color: colors.primary,
                      ),
                    ),
                  ),
                  const SizedBox(width: 8),
                  Flexible(
                    child: Text(
                      displayName,
                      maxLines: 1,
                      overflow: TextOverflow.ellipsis,
                      style: TextStyle(
                        fontSize: 14,
                        fontWeight: FontWeight.w600,
                        color: colors.textPrimary,
                      ),
                    ),
                  ),
                ],
              ),
              const SizedBox(height: 4),
              Text(
                typeName.toUpperCase(),
                style: TextStyle(
                  fontSize: 10,
                  color: colors.textMuted,
                  letterSpacing: 0.5,
                ),
              ),
            ],
          ),
          
          const SizedBox(width: 24),
          
          // Slew Button
          _PopupActionButton(
            icon: LucideIcons.crosshair,
            label: 'Slew',
            isPrimary: true,
            colors: colors,
            onTap: onSlew,
          ),
        ],
      ),
    );
  }
}
