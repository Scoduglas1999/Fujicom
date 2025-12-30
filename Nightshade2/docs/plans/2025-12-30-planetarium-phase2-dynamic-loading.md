# Planetarium Phase 2: Dynamic Object Loading Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Make the planetarium feel alive and responsive with zoom-aware object loading, smooth pop-in animations, and visual density indicators.

**Architecture:** Extend existing rendering pipeline with FOV-based magnitude limits, add DSO pop-in animations matching star behavior, and add density indicators for crowded regions.

**Tech Stack:** Flutter CustomPainter, Riverpod providers, existing astronomy calculations

**Design Doc:** `docs/plans/2025-12-29-planetarium-overhaul-design.md`

---

## Task 1: Add FOV-Based Magnitude Limit Provider

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/providers/planetarium_providers.dart`

**Step 1: Create dynamic magnitude limit provider**

Add after the existing quality providers:

```dart
/// Computed magnitude limits based on current FOV
/// Returns (starMagLimit, dsoMagLimit)
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
```

**Step 2: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`

**Step 3: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/providers/planetarium_providers.dart
git commit -m "feat(planetarium): add FOV-based dynamic magnitude limits provider"
```

---

## Task 2: Update Star Loading to Use Dynamic Limits

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/providers/planetarium_providers.dart`

**Step 1: Update loadedStarsProvider to use dynamic limits**

Find `loadedStarsProvider` and modify it to watch `dynamicMagnitudeLimitsProvider`:

```dart
final loadedStarsProvider = FutureProvider<List<Star>>((ref) async {
  final (starMagLimit, _) = ref.watch(dynamicMagnitudeLimitsProvider);
  final catalog = ref.watch(starCatalogProvider);

  // Load stars up to the dynamic magnitude limit
  return catalog.getStarsToMagnitude(starMagLimit);
});
```

If star loading is done differently, adapt accordingly. The key is that the magnitude limit should now be FOV-dependent.

**Step 2: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`

**Step 3: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/providers/planetarium_providers.dart
git commit -m "feat(planetarium): use dynamic magnitude limits for star loading"
```

---

## Task 3: Update DSO Loading to Use Dynamic Limits

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/providers/planetarium_providers.dart`

**Step 1: Update DSO loading provider to use dynamic limits**

Find the DSO loading provider and modify it similarly:

```dart
final loadedDsosProvider = FutureProvider<List<DeepSkyObject>>((ref) async {
  final (_, dsoMagLimit) = ref.watch(dynamicMagnitudeLimitsProvider);
  final catalog = ref.watch(dsoCatalogProvider);

  // Load DSOs up to the dynamic magnitude limit
  return catalog.getDsosToMagnitude(dsoMagLimit);
});
```

**Step 2: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`

**Step 3: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/providers/planetarium_providers.dart
git commit -m "feat(planetarium): use dynamic magnitude limits for DSO loading"
```

---

## Task 4: Add DSO Pop-In Animation State

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/widgets/interactive_sky_view.dart`

**Step 1: Add DSO visibility tracking**

Stars already have pop-in animation. Add similar tracking for DSOs. Find the `_InteractiveSkyViewState` class and add:

```dart
// DSO pop-in animation state
final Map<String, double> _dsoVisibilityProgress = {};
DateTime? _lastDsoMagLimit;

void _updateDsoVisibility(double currentMagLimit) {
  final now = DateTime.now();
  if (_lastDsoMagLimit != null && currentMagLimit != _lastDsoMagLimit) {
    // Magnitude limit changed, new DSOs appearing
    // Mark newly visible DSOs for animation
  }
  _lastDsoMagLimit = currentMagLimit;
}
```

**Step 2: Animate DSO opacity/scale based on visibility progress**

In the build method where DSOs are passed to the renderer, include visibility progress:

```dart
// DSOs with visibility progress for pop-in animation
final dsosWithProgress = loadedDsos.map((dso) {
  final progress = _dsoVisibilityProgress[dso.id] ?? 1.0;
  return (dso, progress);
}).toList();
```

**Step 3: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`

**Step 4: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/widgets/interactive_sky_view.dart
git commit -m "feat(planetarium): add DSO pop-in animation state tracking"
```

---

## Task 5: Implement DSO Pop-In Animation in Renderer

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/rendering/sky_renderer.dart`

**Step 1: Update DSO rendering to apply pop-in effect**

Find the DSO rendering section (~lines 1242-1672) and add pop-in animation support:

```dart
void _drawDso(Canvas canvas, DeepSkyObject dso, Offset offset, double displaySize, {double visibility = 1.0}) {
  if (visibility <= 0) return;

  // Apply pop-in: scale from 80% to 100%, fade from 0 to 1
  final scale = 0.8 + (0.2 * visibility);
  final alpha = visibility;

  canvas.save();
  canvas.translate(offset.dx, offset.dy);
  canvas.scale(scale);
  canvas.translate(-offset.dx, -offset.dy);

  // Existing DSO drawing code with alpha applied to colors
  // ... modify paint colors to use .withValues(alpha: existingAlpha * alpha)

  canvas.restore();
}
```

**Step 2: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`

**Step 3: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/rendering/sky_renderer.dart
git commit -m "feat(planetarium): implement DSO pop-in animation in renderer"
```

---

## Task 6: Add Visual Density Indicator Provider

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/providers/planetarium_providers.dart`

**Step 1: Create density calculation provider**

Add a provider that calculates object density in regions:

```dart
/// Calculates density hotspots for crowded regions
/// Returns list of (RA, Dec, objectCount, hiddenCount) for areas with many hidden objects
final densityHotspotsProvider = Provider<List<(double, double, int, int)>>((ref) {
  final viewState = ref.watch(skyViewStateProvider);
  final (starMagLimit, dsoMagLimit) = ref.watch(dynamicMagnitudeLimitsProvider);
  final allStars = ref.watch(starCatalogProvider).getAllStars();

  // Only show density indicators when zoomed out (FOV > 30°)
  if (viewState.fieldOfView < 30) return [];

  // Grid the visible sky into cells and count objects
  final cellSize = 10.0; // degrees
  final Map<String, (int visible, int hidden)> cells = {};

  for (final star in allStars) {
    if (!_isInView(star.ra, star.dec, viewState)) continue;

    final cellKey = '${(star.ra / cellSize).floor()}_${(star.dec / cellSize).floor()}';
    final current = cells[cellKey] ?? (0, 0);

    if (star.magnitude <= starMagLimit) {
      cells[cellKey] = (current.$1 + 1, current.$2);
    } else {
      cells[cellKey] = (current.$1, current.$2 + 1);
    }
  }

  // Return cells with significant hidden objects
  return cells.entries
    .where((e) => e.value.$2 > 50) // More than 50 hidden objects
    .map((e) {
      final parts = e.key.split('_');
      final ra = double.parse(parts[0]) * cellSize + cellSize / 2;
      final dec = double.parse(parts[1]) * cellSize + cellSize / 2;
      return (ra, dec, e.value.$1, e.value.$2);
    })
    .toList();
});
```

**Step 2: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`

**Step 3: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/providers/planetarium_providers.dart
git commit -m "feat(planetarium): add density hotspot calculation provider"
```

---

## Task 7: Render Density Indicators

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/rendering/sky_renderer.dart`

**Step 1: Add density indicator rendering**

Add a method to draw subtle glow indicators for crowded regions:

```dart
void _drawDensityIndicators(Canvas canvas, Size size, Offset center, double scale, List<(double, double, int, int)> hotspots) {
  if (hotspots.isEmpty) return;

  for (final (ra, dec, visible, hidden) in hotspots) {
    final offset = _celestialToScreen(
      CelestialCoordinate(ra: ra, dec: dec),
      center,
      scale,
    );

    if (offset == null || !_isInView(offset, size)) continue;

    // Subtle glow indicating hidden objects
    final intensity = (hidden / 200).clamp(0.3, 0.8);
    final radius = 20.0 + (hidden / 50).clamp(0, 30);

    final paint = Paint()
      ..color = Colors.blue.withValues(alpha: intensity * 0.3)
      ..style = PaintingStyle.fill;

    if (qualityConfig.useBlurEffects) {
      paint.maskFilter = MaskFilter.blur(BlurStyle.normal, radius / 2);
    }

    canvas.drawCircle(offset, radius, paint);

    // Optional: Draw count label
    if (hidden > 100) {
      final textPainter = TextPainter(
        text: TextSpan(
          text: '+${hidden}',
          style: TextStyle(
            color: Colors.white.withValues(alpha: 0.6),
            fontSize: 10,
          ),
        ),
        textDirection: TextDirection.ltr,
      );
      textPainter.layout();
      textPainter.paint(canvas, offset - Offset(textPainter.width / 2, textPainter.height / 2));
    }
  }
}
```

**Step 2: Call from paint method**

Add call in the paint method after drawing stars/DSOs:

```dart
// Draw density indicators for crowded regions
_drawDensityIndicators(canvas, size, center, scale, densityHotspots);
```

**Step 3: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`

**Step 4: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/rendering/sky_renderer.dart
git commit -m "feat(planetarium): render visual density indicators for crowded regions"
```

---

## Task 8: Ensure All Visible Objects Are Clickable

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/widgets/interactive_sky_view.dart`

**Step 1: Review and fix tap detection**

Ensure the tap detection uses the same magnitude limits as rendering:

```dart
// In _handleTap or similar method
final (starMagLimit, dsoMagLimit) = ref.read(dynamicMagnitudeLimitsProvider);

// Filter objects to only those currently visible
final visibleStars = loadedStars.where((s) => s.magnitude <= starMagLimit);
final visibleDsos = loadedDsos.where((d) => d.magnitude <= dsoMagLimit);

// Use these filtered lists for tap detection
```

**Step 2: Verify hitbox sizes are appropriate**

Ensure smaller/fainter objects still have reasonable hit targets:

```dart
// Minimum tap target size regardless of object brightness
final minHitRadius = 8.0; // pixels
final hitRadius = max(minHitRadius, calculatedRadius * 1.5);
```

**Step 3: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`

**Step 4: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/widgets/interactive_sky_view.dart
git commit -m "feat(planetarium): ensure all visible objects are clickable with proper hit targets"
```

---

## Task 9: Integration Test

**Files:**
- None (manual testing)

**Step 1: Build and run**

Run: `melos run dev`

**Step 2: Manual verification checklist**

- [ ] Zooming in reveals more stars progressively
- [ ] Zooming in reveals more DSOs progressively
- [ ] New objects fade in smoothly (not instant pop)
- [ ] DSOs have scale animation when appearing
- [ ] Density indicators show on crowded regions when zoomed out
- [ ] Density indicators disappear when zoomed in
- [ ] All visible objects can be tapped/selected
- [ ] Performance remains smooth during zoom

**Step 3: Final commit if fixes needed**

```bash
git add -A
git commit -m "fix(planetarium): polish dynamic object loading"
```

---

## Summary

This plan implements Phase 2 Dynamic Object Loading with:
1. FOV-based magnitude limits (deeper limits when zoomed in)
2. DSO pop-in animations matching star behavior
3. Visual density indicators for crowded regions
4. Guaranteed clickability for all visible objects
