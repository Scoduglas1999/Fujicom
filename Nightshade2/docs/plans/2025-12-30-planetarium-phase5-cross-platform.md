# Planetarium Phase 5: Cross-Platform Optimization Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Consistent experience everywhere with platform-adaptive layouts, performance auto-detection, and touch gesture refinements.

**Architecture:** Use platform detection to adapt UI layouts and interaction patterns. Implement performance monitoring to auto-switch quality tiers.

**Tech Stack:** Flutter platform detection, Platform.isXxx, MediaQuery, gesture detectors

**Design Doc:** `docs/plans/2025-12-29-planetarium-overhaul-design.md`

---

## Task 1: Create Platform-Aware Layout Provider

**Files:**
- Create: `packages/nightshade_planetarium/lib/src/providers/platform_providers.dart`

**Step 1: Create platform detection providers**

```dart
import 'dart:io';
import 'package:flutter/foundation.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';

/// Platform form factor enum
enum FormFactor { phone, tablet, desktop }

/// Current platform form factor
final formFactorProvider = Provider<FormFactor>((ref) {
  if (kIsWeb) {
    // For web, determine by screen size at runtime
    return FormFactor.desktop; // Default, will be overridden by adaptive widget
  }

  if (Platform.isWindows || Platform.isMacOS || Platform.isLinux) {
    return FormFactor.desktop;
  }

  // iOS/Android - check screen size to distinguish phone vs tablet
  // This will be determined dynamically in widgets using MediaQuery
  return FormFactor.phone; // Default, widgets should check screen size
});

/// Whether we're on a touch-primary device
final isTouchDeviceProvider = Provider<bool>((ref) {
  if (kIsWeb) return false; // Assume desktop for web
  return Platform.isIOS || Platform.isAndroid;
});

/// Whether hover interactions are available
final hasHoverProvider = Provider<bool>((ref) {
  if (kIsWeb) return true;
  return Platform.isWindows || Platform.isMacOS || Platform.isLinux;
});

/// Whether right-click context menus are expected
final hasContextMenuProvider = Provider<bool>((ref) {
  return ref.watch(hasHoverProvider);
});
```

**Step 2: Export from package**

Add to `nightshade_planetarium.dart`:

```dart
export 'src/providers/platform_providers.dart';
```

**Step 3: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`

**Step 4: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/providers/platform_providers.dart
git add packages/nightshade_planetarium/lib/nightshade_planetarium.dart
git commit -m "feat(planetarium): add platform detection providers"
```

---

## Task 2: Create Adaptive Layout Widget

**Files:**
- Create: `packages/nightshade_planetarium/lib/src/widgets/adaptive_planetarium_layout.dart`

**Step 1: Create adaptive layout wrapper**

```dart
import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import '../providers/platform_providers.dart';

/// Determines layout based on screen size
FormFactor getFormFactor(BuildContext context) {
  final width = MediaQuery.of(context).size.width;
  final height = MediaQuery.of(context).size.height;
  final shortestSide = MediaQuery.of(context).size.shortestSide;

  if (shortestSide < 600) {
    return FormFactor.phone;
  } else if (shortestSide < 900) {
    return FormFactor.tablet;
  } else {
    return FormFactor.desktop;
  }
}

/// Adaptive sizing based on form factor
class AdaptiveSizing {
  final FormFactor formFactor;

  AdaptiveSizing(this.formFactor);

  /// HUD element sizes
  double get compassSize {
    switch (formFactor) {
      case FormFactor.phone:
        return 60;
      case FormFactor.tablet:
        return 90;
      case FormFactor.desktop:
        return 80;
    }
  }

  double get minimapSize {
    switch (formFactor) {
      case FormFactor.phone:
        return 80;
      case FormFactor.tablet:
        return 120;
      case FormFactor.desktop:
        return 100;
    }
  }

  /// Touch target minimum sizes
  double get minTouchTarget {
    switch (formFactor) {
      case FormFactor.phone:
        return 48;
      case FormFactor.tablet:
        return 44;
      case FormFactor.desktop:
        return 32;
    }
  }

  /// Whether to show sidebar or bottom sheet for filters
  bool get useBottomSheet => formFactor == FormFactor.phone;

  /// Whether to show condensed HUD
  bool get useCondensedHud => formFactor == FormFactor.phone;

  /// Whether mini-map is optional (user can toggle)
  bool get minimapOptional => formFactor == FormFactor.phone;

  /// Edge padding
  double get edgePadding {
    switch (formFactor) {
      case FormFactor.phone:
        return 12;
      case FormFactor.tablet:
        return 16;
      case FormFactor.desktop:
        return 16;
    }
  }
}

/// Provider for adaptive sizing based on current screen
final adaptiveSizingProvider = Provider.family<AdaptiveSizing, BuildContext>((ref, context) {
  final formFactor = getFormFactor(context);
  return AdaptiveSizing(formFactor);
});
```

**Step 2: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`

**Step 3: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/widgets/adaptive_planetarium_layout.dart
git commit -m "feat(planetarium): create adaptive layout sizing utilities"
```

---

## Task 3: Apply Adaptive Sizing to HUD Overlays

**Files:**
- Modify: `packages/nightshade_app/lib/screens/planetarium/planetarium_screen.dart`

**Step 1: Use adaptive sizing for compass and minimap**

Update the HUD overlay section:

```dart
// In the Stack children where CompassHud is positioned
Positioned(
  left: adaptiveSizing.edgePadding,
  bottom: adaptiveSizing.edgePadding + (adaptiveSizing.useCondensedHud ? 0 : 50),
  child: Consumer(
    builder: (context, ref, _) {
      final adaptiveSizing = ref.watch(adaptiveSizingProvider(context));
      final showCompass = ref.watch(showCompassHudProvider);
      if (!showCompass) return const SizedBox.shrink();

      final (az, alt) = ref.watch(viewCenterAltAzProvider);
      return CompassHud(
        azimuth: az,
        altitude: alt,
        size: adaptiveSizing.compassSize,
        showAltitude: !adaptiveSizing.useCondensedHud,
      );
    },
  ),
),

// Mini-map
Positioned(
  right: adaptiveSizing.edgePadding,
  bottom: adaptiveSizing.edgePadding,
  child: Consumer(
    builder: (context, ref, _) {
      final adaptiveSizing = ref.watch(adaptiveSizingProvider(context));
      final showMinimap = ref.watch(showMinimapProvider);

      // On phone, only show if explicitly enabled
      if (adaptiveSizing.minimapOptional && !showMinimap) {
        return const SizedBox.shrink();
      }
      if (!showMinimap) return const SizedBox.shrink();

      final (az, alt) = ref.watch(viewCenterAltAzProvider);
      final viewState = ref.watch(skyViewStateProvider);

      return SkyMinimap(
        azimuth: az,
        altitude: alt,
        fieldOfView: viewState.fieldOfView,
        rotation: viewState.rotation,
        size: adaptiveSizing.minimapSize,
        onTap: (tapAz, tapAlt) => _handleMinimapTap(ref, tapAz, tapAlt),
      );
    },
  ),
),
```

**Step 2: Run analyzer**

Run: `flutter analyze packages/nightshade_app`

**Step 3: Commit**

```bash
git add packages/nightshade_app/lib/screens/planetarium/planetarium_screen.dart
git commit -m "feat(planetarium): apply adaptive sizing to HUD overlays"
```

---

## Task 4: Implement Phone-Specific Bottom Sheet for Filters

**Files:**
- Modify: `packages/nightshade_app/lib/screens/planetarium/planetarium_screen.dart`

**Step 1: Add bottom sheet filter panel for phone**

```dart
void _showFilterBottomSheet(BuildContext context) {
  showModalBottomSheet(
    context: context,
    isScrollControlled: true,
    backgroundColor: Colors.grey[900],
    shape: const RoundedRectangleBorder(
      borderRadius: BorderRadius.vertical(top: Radius.circular(16)),
    ),
    builder: (context) => DraggableScrollableSheet(
      initialChildSize: 0.5,
      minChildSize: 0.3,
      maxChildSize: 0.9,
      expand: false,
      builder: (context, scrollController) => Consumer(
        builder: (context, ref, _) {
          return _FilterBottomSheet(scrollController: scrollController);
        },
      ),
    ),
  );
}

class _FilterBottomSheet extends ConsumerWidget {
  final ScrollController scrollController;

  const _FilterBottomSheet({required this.scrollController});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final config = ref.watch(skyRenderConfigProvider);

    return ListView(
      controller: scrollController,
      padding: const EdgeInsets.all(16),
      children: [
        // Drag handle
        Center(
          child: Container(
            width: 40,
            height: 4,
            decoration: BoxDecoration(
              color: Colors.white38,
              borderRadius: BorderRadius.circular(2),
            ),
          ),
        ),
        const SizedBox(height: 16),

        const Text(
          'Filters',
          style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold),
        ),
        const SizedBox(height: 16),

        // Object type toggles
        SwitchListTile(
          title: const Text('Stars'),
          secondary: const Icon(LucideIcons.star),
          value: config.showStars,
          onChanged: (_) => ref.read(skyRenderConfigProvider.notifier).toggleStars(),
        ),
        SwitchListTile(
          title: const Text('Planets'),
          secondary: const Icon(LucideIcons.circle),
          value: config.showPlanets,
          onChanged: (_) => ref.read(skyRenderConfigProvider.notifier).togglePlanets(),
        ),
        SwitchListTile(
          title: const Text('Deep Sky Objects'),
          secondary: const Icon(LucideIcons.sparkles),
          value: config.showDsos,
          onChanged: (_) => ref.read(skyRenderConfigProvider.notifier).toggleDsos(),
        ),

        const Divider(),

        // Overlay toggles
        SwitchListTile(
          title: const Text('Grid'),
          secondary: const Icon(LucideIcons.grid3x3),
          value: config.showGrid,
          onChanged: (_) => ref.read(skyRenderConfigProvider.notifier).toggleGrid(),
        ),
        SwitchListTile(
          title: const Text('Constellations'),
          secondary: const Icon(LucideIcons.network),
          value: config.showConstellationLines,
          onChanged: (_) => ref.read(skyRenderConfigProvider.notifier).toggleConstellationLines(),
        ),
      ],
    );
  }
}
```

**Step 2: Use bottom sheet on phone, sidebar on desktop/tablet**

```dart
// In planetarium screen layout
Widget _buildFilterPanel(BuildContext context) {
  final adaptiveSizing = AdaptiveSizing(getFormFactor(context));

  if (adaptiveSizing.useBottomSheet) {
    // Phone: show FAB that opens bottom sheet
    return Positioned(
      right: 16,
      top: 16,
      child: FloatingActionButton.small(
        onPressed: () => _showFilterBottomSheet(context),
        child: const Icon(LucideIcons.slidersHorizontal),
      ),
    );
  } else {
    // Tablet/Desktop: show collapsible sidebar
    return FilterSidebar(
      isExpanded: _filterSidebarExpanded,
      onToggle: () => setState(() => _filterSidebarExpanded = !_filterSidebarExpanded),
    );
  }
}
```

**Step 3: Run analyzer**

Run: `flutter analyze packages/nightshade_app`

**Step 4: Commit**

```bash
git add packages/nightshade_app/lib/screens/planetarium/planetarium_screen.dart
git commit -m "feat(planetarium): implement bottom sheet filters for phone layout"
```

---

## Task 5: Add Performance Auto-Detection

**Files:**
- Create: `packages/nightshade_planetarium/lib/src/providers/performance_providers.dart`

**Step 1: Create performance monitoring provider**

```dart
import 'package:flutter/scheduler.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import '../rendering/render_quality.dart';

/// Tracks frame timing and suggests quality adjustments
class PerformanceMonitor {
  final List<double> _frameTimings = [];
  static const _sampleSize = 30;
  static const _targetFps = 30.0;
  static const _targetFrameTime = 1000 / _targetFps; // ~33ms

  void recordFrameTime(double milliseconds) {
    _frameTimings.add(milliseconds);
    if (_frameTimings.length > _sampleSize) {
      _frameTimings.removeAt(0);
    }
  }

  double get averageFrameTime {
    if (_frameTimings.isEmpty) return 0;
    return _frameTimings.reduce((a, b) => a + b) / _frameTimings.length;
  }

  double get estimatedFps {
    final avgTime = averageFrameTime;
    if (avgTime <= 0) return 60;
    return 1000 / avgTime;
  }

  /// Returns suggested quality tier based on recent performance
  RenderQuality? suggestQualityChange(RenderQuality current) {
    if (_frameTimings.length < _sampleSize) return null; // Not enough data

    final fps = estimatedFps;

    // If consistently below 30fps, suggest downgrade
    if (fps < 25 && current != RenderQuality.performance) {
      if (current == RenderQuality.quality) {
        return RenderQuality.balanced;
      } else {
        return RenderQuality.performance;
      }
    }

    // If consistently above 50fps and not at max, could upgrade
    if (fps > 50 && current != RenderQuality.quality) {
      if (current == RenderQuality.performance) {
        return RenderQuality.balanced;
      } else {
        return RenderQuality.quality;
      }
    }

    return null; // No change needed
  }
}

final performanceMonitorProvider = Provider<PerformanceMonitor>((ref) {
  return PerformanceMonitor();
});

/// Auto quality management provider
final autoQualityEnabledProvider = StateProvider<bool>((ref) => true);

/// Quality tier that may be auto-adjusted based on performance
final effectiveQualityProvider = Provider<RenderQuality>((ref) {
  final autoEnabled = ref.watch(autoQualityEnabledProvider);
  final userPreference = ref.watch(renderQualityTierProvider);

  if (!autoEnabled) return userPreference;

  final monitor = ref.watch(performanceMonitorProvider);
  final suggestion = monitor.suggestQualityChange(userPreference);

  // For now, just return user preference
  // Auto-switching would need more sophisticated state management
  return suggestion ?? userPreference;
});
```

**Step 2: Export from package**

Add to `nightshade_planetarium.dart`:

```dart
export 'src/providers/performance_providers.dart';
```

**Step 3: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`

**Step 4: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/providers/performance_providers.dart
git add packages/nightshade_planetarium/lib/nightshade_planetarium.dart
git commit -m "feat(planetarium): add performance auto-detection for quality tier"
```

---

## Task 6: Integrate Performance Monitoring into Renderer

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/widgets/interactive_sky_view.dart`

**Step 1: Add frame timing callback**

In the build method or animation ticker:

```dart
@override
void initState() {
  super.initState();

  // Add frame callback for performance monitoring
  SchedulerBinding.instance.addPostFrameCallback(_onFrame);
}

DateTime? _lastFrameTime;

void _onFrame(Duration timestamp) {
  final now = DateTime.now();
  if (_lastFrameTime != null) {
    final frameTime = now.difference(_lastFrameTime!).inMicroseconds / 1000;
    ref.read(performanceMonitorProvider).recordFrameTime(frameTime);
  }
  _lastFrameTime = now;

  // Schedule next callback
  SchedulerBinding.instance.addPostFrameCallback(_onFrame);
}
```

**Step 2: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`

**Step 3: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/widgets/interactive_sky_view.dart
git commit -m "feat(planetarium): integrate performance monitoring into renderer"
```

---

## Task 7: Add Touch Gesture Refinements

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/widgets/interactive_sky_view.dart`

**Step 1: Improve pinch zoom gesture**

Ensure smooth two-finger pinch zoom:

```dart
// In gesture detector or interactive viewer
ScaleGestureRecognizer? _scaleGesture;

void _handleScaleStart(ScaleStartDetails details) {
  _initialFov = viewState.fieldOfView;
  _initialFocalPoint = details.focalPoint;
}

void _handleScaleUpdate(ScaleUpdateDetails details) {
  // Pinch zoom
  if (details.scale != 1.0) {
    final newFov = (_initialFov / details.scale).clamp(1.0, 120.0);
    ref.read(skyViewStateProvider.notifier).setFieldOfView(newFov);
  }

  // Pan (two-finger drag)
  if (details.focalPointDelta != Offset.zero) {
    _handlePan(details.focalPointDelta);
  }
}

void _handleScaleEnd(ScaleEndDetails details) {
  // Apply momentum for natural feeling
  if (details.velocity.pixelsPerSecond.distance > 100) {
    _startMomentum(details.velocity.pixelsPerSecond);
  }
}
```

**Step 2: Add momentum scrolling**

```dart
AnimationController? _momentumController;
Offset _momentumVelocity = Offset.zero;

void _startMomentum(Offset velocity) {
  _momentumVelocity = velocity;
  _momentumController = AnimationController(
    duration: const Duration(milliseconds: 800),
    vsync: this,
  );

  _momentumController!.addListener(() {
    final progress = Curves.decelerate.transform(_momentumController!.value);
    final currentVelocity = _momentumVelocity * (1 - progress);
    _handlePan(currentVelocity * 0.016); // Assuming 60fps
  });

  _momentumController!.forward();
}
```

**Step 3: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`

**Step 4: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/widgets/interactive_sky_view.dart
git commit -m "feat(planetarium): refine touch gestures with momentum scrolling"
```

---

## Task 8: Add Right-Click Context Menu (Desktop)

**Files:**
- Modify: `packages/nightshade_app/lib/screens/planetarium/planetarium_screen.dart`

**Step 1: Add context menu on right-click**

```dart
Widget _buildInteractiveSkyView() {
  return Consumer(
    builder: (context, ref, _) {
      final hasContextMenu = ref.watch(hasContextMenuProvider);

      return GestureDetector(
        onSecondaryTapUp: hasContextMenu
          ? (details) => _showContextMenu(context, details.globalPosition)
          : null,
        child: InteractiveSkyView(
          // ... existing props
        ),
      );
    },
  );
}

void _showContextMenu(BuildContext context, Offset position) {
  final selectedObject = ref.read(selectedObjectProvider);

  showMenu(
    context: context,
    position: RelativeRect.fromLTRB(position.dx, position.dy, position.dx, position.dy),
    items: [
      if (selectedObject != null) ...[
        PopupMenuItem(
          child: ListTile(
            leading: const Icon(LucideIcons.info),
            title: const Text('Object Details'),
            contentPadding: EdgeInsets.zero,
          ),
          onTap: () => _showObjectDetails(selectedObject),
        ),
        PopupMenuItem(
          child: ListTile(
            leading: const Icon(LucideIcons.target),
            title: const Text('Slew to Object'),
            contentPadding: EdgeInsets.zero,
          ),
          onTap: () => _slewToObject(selectedObject),
        ),
        PopupMenuItem(
          child: ListTile(
            leading: const Icon(LucideIcons.plus),
            title: const Text('Add to Target List'),
            contentPadding: EdgeInsets.zero,
          ),
          onTap: () => _addToTargets(selectedObject),
        ),
        const PopupMenuDivider(),
      ],
      PopupMenuItem(
        child: ListTile(
          leading: const Icon(LucideIcons.home),
          title: const Text('Reset View'),
          contentPadding: EdgeInsets.zero,
        ),
        onTap: () => _resetView(),
      ),
      PopupMenuItem(
        child: ListTile(
          leading: const Icon(LucideIcons.grid3x3),
          title: Text(ref.read(skyRenderConfigProvider).showGrid ? 'Hide Grid' : 'Show Grid'),
          contentPadding: EdgeInsets.zero,
        ),
        onTap: () => ref.read(skyRenderConfigProvider.notifier).toggleGrid(),
      ),
    ],
  );
}
```

**Step 2: Run analyzer**

Run: `flutter analyze packages/nightshade_app`

**Step 3: Commit**

```bash
git add packages/nightshade_app/lib/screens/planetarium/planetarium_screen.dart
git commit -m "feat(planetarium): add right-click context menu for desktop"
```

---

## Task 9: Add Hover Tooltips (Desktop)

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/widgets/interactive_sky_view.dart`

**Step 1: Add hover detection for objects**

```dart
CelestialObject? _hoveredObject;
Offset? _hoverPosition;

void _handleHover(PointerHoverEvent event) {
  if (!ref.read(hasHoverProvider)) return;

  // Find object under cursor
  final object = _findObjectAtPosition(event.localPosition);

  if (object != _hoveredObject) {
    setState(() {
      _hoveredObject = object;
      _hoverPosition = event.position;
    });
  }
}

// In build, wrap with MouseRegion
MouseRegion(
  onHover: _handleHover,
  onExit: (_) => setState(() => _hoveredObject = null),
  child: CustomPaint(...),
),

// Show tooltip overlay
if (_hoveredObject != null && _hoverPosition != null)
  Positioned(
    left: _hoverPosition!.dx + 16,
    top: _hoverPosition!.dy + 16,
    child: _HoverTooltip(object: _hoveredObject!),
  ),
```

**Step 2: Create tooltip widget**

```dart
class _HoverTooltip extends StatelessWidget {
  final CelestialObject object;

  const _HoverTooltip({required this.object});

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 8),
      decoration: BoxDecoration(
        color: Colors.grey[850],
        borderRadius: BorderRadius.circular(6),
        boxShadow: [
          BoxShadow(color: Colors.black54, blurRadius: 8),
        ],
      ),
      child: Column(
        mainAxisSize: MainAxisSize.min,
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Text(
            object.name ?? object.id,
            style: const TextStyle(fontWeight: FontWeight.bold),
          ),
          if (object is Star)
            Text('Mag ${(object as Star).magnitude.toStringAsFixed(1)}',
              style: TextStyle(fontSize: 12, color: Colors.white54)),
          if (object is DeepSkyObject)
            Text('${(object as DeepSkyObject).type}',
              style: TextStyle(fontSize: 12, color: Colors.white54)),
        ],
      ),
    );
  }
}
```

**Step 3: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`

**Step 4: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/widgets/interactive_sky_view.dart
git commit -m "feat(planetarium): add hover tooltips for desktop"
```

---

## Task 10: Integration Test

**Files:**
- None (manual testing)

**Step 1: Build and run on multiple platforms**

Test on:
- Windows desktop
- macOS (if available)
- Android phone
- Android tablet
- iOS phone (if available)
- iOS tablet (if available)

**Step 2: Desktop verification checklist**

- [ ] Full keyboard shortcuts work
- [ ] Right-click context menu appears
- [ ] Hover tooltips show object info
- [ ] Filter sidebar visible and collapsible
- [ ] HUD sizes appropriate for desktop

**Step 3: Tablet verification checklist**

- [ ] Two-finger pinch zoom is smooth
- [ ] Filter sidebar works (swipe from edge)
- [ ] Larger HUD elements
- [ ] Larger touch targets
- [ ] Mini-map slightly larger

**Step 4: Phone verification checklist**

- [ ] Bottom sheet for filters (not sidebar)
- [ ] Condensed compass HUD
- [ ] Mini-map optional/toggleable
- [ ] Object popup as bottom card
- [ ] Touch gestures feel natural
- [ ] Performance stays above 30fps

**Step 5: Performance verification**

- [ ] Quality auto-adjusts if fps drops
- [ ] No stuttering during pan/zoom
- [ ] Smooth animations

**Step 6: Final commit if fixes needed**

```bash
git add -A
git commit -m "fix(planetarium): polish cross-platform experience"
```

---

## Summary

This plan implements Phase 5 Cross-Platform Optimization with:
1. Platform detection providers (form factor, touch/hover capabilities)
2. Adaptive layout sizing for phone/tablet/desktop
3. Phone-specific bottom sheet for filters
4. Performance auto-detection for quality tier switching
5. Refined touch gestures with momentum scrolling
6. Desktop right-click context menu
7. Desktop hover tooltips
