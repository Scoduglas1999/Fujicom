# Planetarium Phase 4: UI/UX Improvements Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Production-ready interaction design with enriched object popups, collapsible filter sidebar, improved search, and keyboard shortcuts.

**Architecture:** Enhance existing UI widgets with new features. Add keyboard handling at the screen level. Create new filter sidebar widget.

**Tech Stack:** Flutter widgets, Riverpod providers, lucide_icons

**Design Doc:** `docs/plans/2025-12-29-planetarium-overhaul-design.md`

---

## Task 1: Enhance Object Details Panel - Add Thumbnail

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/widgets/object_details_panel.dart`

**Step 1: Add DSS thumbnail loading**

Add a provider to fetch DSS (Digital Sky Survey) thumbnails:

```dart
/// Fetches DSS thumbnail for DSO objects
final dssThumbailProvider = FutureProvider.family<Uint8List?, String>((ref, objectId) async {
  // Use STScI DSS endpoint for thumbnails
  // Format: https://archive.stsci.edu/cgi-bin/dss_search?v=poss2ukstu_red&r=RA&d=DEC&e=J2000&h=5&w=5&f=gif
  // Note: In production, consider caching these
  return null; // Placeholder - implement actual fetch
});
```

**Step 2: Add thumbnail widget to panel**

```dart
Widget _buildThumbnail(DeepSkyObject dso) {
  return Container(
    width: 100,
    height: 100,
    decoration: BoxDecoration(
      color: Colors.grey[900],
      borderRadius: BorderRadius.circular(8),
      border: Border.all(color: Colors.white24),
    ),
    child: ClipRRect(
      borderRadius: BorderRadius.circular(7),
      child: Consumer(
        builder: (context, ref, _) {
          final thumbnail = ref.watch(dssThumbnailProvider(dso.id));
          return thumbnail.when(
            data: (bytes) => bytes != null
              ? Image.memory(bytes, fit: BoxFit.cover)
              : _buildPlaceholder(dso),
            loading: () => const Center(child: CircularProgressIndicator()),
            error: (_, __) => _buildPlaceholder(dso),
          );
        },
      ),
    ),
  );
}

Widget _buildPlaceholder(DeepSkyObject dso) {
  return Center(
    child: Icon(
      _getDsoIcon(dso.type),
      color: _getDsoColor(dso.type),
      size: 40,
    ),
  );
}
```

**Step 3: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`

**Step 4: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/widgets/object_details_panel.dart
git commit -m "feat(planetarium): add DSS thumbnail to object details panel"
```

---

## Task 2: Add Quick Stats Bar to Object Panel

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/widgets/object_details_panel.dart`

**Step 1: Create quick stats widget**

```dart
Widget _buildQuickStats(CelestialObject object, WidgetRef ref) {
  final location = ref.watch(observerLocationProvider);
  final time = ref.watch(observationTimeProvider);
  final moonPos = ref.watch(moonPositionProvider);

  // Calculate altitude
  final lst = AstronomyCalculations.localSiderealTime(time.time, location.longitude);
  final (alt, az) = AstronomyCalculations.equatorialToHorizontal(
    raDeg: object.ra * 15,
    decDeg: object.dec,
    latitudeDeg: location.latitude,
    lstHours: lst,
  );

  // Calculate transit time
  final transitTime = _calculateTransitTime(object, location, time);

  // Calculate moon distance
  final moonDistance = _calculateMoonDistance(object, moonPos);

  return Container(
    padding: const EdgeInsets.all(12),
    decoration: BoxDecoration(
      color: Colors.grey[850],
      borderRadius: BorderRadius.circular(8),
    ),
    child: Row(
      mainAxisAlignment: MainAxisAlignment.spaceAround,
      children: [
        _StatItem(
          icon: LucideIcons.mountain,
          label: 'Alt',
          value: '${alt.toStringAsFixed(0)}°',
        ),
        _StatItem(
          icon: LucideIcons.timer,
          label: 'Transit',
          value: transitTime,
        ),
        _StatItem(
          icon: LucideIcons.moon,
          label: 'Moon',
          value: '${moonDistance.toStringAsFixed(0)}°',
        ),
      ],
    ),
  );
}

class _StatItem extends StatelessWidget {
  final IconData icon;
  final String label;
  final String value;

  const _StatItem({
    required this.icon,
    required this.label,
    required this.value,
  });

  @override
  Widget build(BuildContext context) {
    return Column(
      mainAxisSize: MainAxisSize.min,
      children: [
        Icon(icon, size: 16, color: Colors.white54),
        const SizedBox(height: 4),
        Text(value, style: const TextStyle(fontWeight: FontWeight.bold)),
        Text(label, style: TextStyle(fontSize: 10, color: Colors.white54)),
      ],
    );
  }
}
```

**Step 2: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`

**Step 3: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/widgets/object_details_panel.dart
git commit -m "feat(planetarium): add quick stats bar to object panel"
```

---

## Task 3: Add Visibility Score Indicator

**Files:**
- Modify: `packages/nightshade_planetarium/lib/src/widgets/object_details_panel.dart`

**Step 1: Create visibility score calculator**

```dart
/// Calculate visibility score (0-100) based on multiple factors
int _calculateVisibilityScore(CelestialObject object, WidgetRef ref) {
  final location = ref.watch(observerLocationProvider);
  final time = ref.watch(observationTimeProvider);
  final moonPos = ref.watch(moonPositionProvider);
  final moonPhase = ref.watch(moonPhaseProvider);

  final lst = AstronomyCalculations.localSiderealTime(time.time, location.longitude);
  final (alt, az) = AstronomyCalculations.equatorialToHorizontal(
    raDeg: object.ra * 15,
    decDeg: object.dec,
    latitudeDeg: location.latitude,
    lstHours: lst,
  );

  int score = 0;

  // Altitude factor (0-40 points)
  if (alt < 0) {
    score += 0;
  } else if (alt < 20) {
    score += (alt * 1.0).round();
  } else if (alt < 60) {
    score += 20 + ((alt - 20) * 0.5).round();
  } else {
    score += 40;
  }

  // Moon distance factor (0-30 points)
  final moonDist = _calculateMoonDistance(object, moonPos);
  if (moonDist > 90) {
    score += 30;
  } else if (moonDist > 45) {
    score += 20;
  } else if (moonDist > 20) {
    score += 10;
  }

  // Moon phase factor (0-20 points)
  score += ((1 - moonPhase) * 20).round();

  // Twilight factor (0-10 points)
  final sunAlt = ref.watch(sunAltitudeProvider);
  if (sunAlt < -18) {
    score += 10; // Astronomical twilight
  } else if (sunAlt < -12) {
    score += 5;  // Nautical twilight
  }

  return score.clamp(0, 100);
}

Color _getVisibilityColor(int score) {
  if (score >= 70) return Colors.green;
  if (score >= 40) return Colors.amber;
  return Colors.red;
}

String _getVisibilityLabel(int score) {
  if (score >= 70) return 'Excellent';
  if (score >= 40) return 'Fair';
  return 'Poor';
}
```

**Step 2: Add visibility indicator widget**

```dart
Widget _buildVisibilityIndicator(CelestialObject object, WidgetRef ref) {
  final score = _calculateVisibilityScore(object, ref);
  final color = _getVisibilityColor(score);
  final label = _getVisibilityLabel(score);

  return Tooltip(
    message: _getVisibilityExplanation(score, object, ref),
    child: Container(
      padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 6),
      decoration: BoxDecoration(
        color: color.withValues(alpha: 0.2),
        borderRadius: BorderRadius.circular(16),
        border: Border.all(color: color),
      ),
      child: Row(
        mainAxisSize: MainAxisSize.min,
        children: [
          Icon(
            score >= 70 ? LucideIcons.eye : score >= 40 ? LucideIcons.eyeOff : LucideIcons.cloudOff,
            size: 16,
            color: color,
          ),
          const SizedBox(width: 6),
          Text(
            label,
            style: TextStyle(color: color, fontWeight: FontWeight.w600),
          ),
        ],
      ),
    ),
  );
}
```

**Step 3: Run analyzer**

Run: `cd packages/nightshade_planetarium && flutter analyze`

**Step 4: Commit**

```bash
git add packages/nightshade_planetarium/lib/src/widgets/object_details_panel.dart
git commit -m "feat(planetarium): add visibility score indicator with tooltip"
```

---

## Task 4: Create Filter Sidebar Widget

**Files:**
- Create: `packages/nightshade_app/lib/screens/planetarium/widgets/filter_sidebar.dart`

**Step 1: Create filter sidebar widget**

```dart
import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:lucide_icons/lucide_icons.dart';
import 'package:nightshade_planetarium/nightshade_planetarium.dart';

class FilterSidebar extends ConsumerWidget {
  final bool isExpanded;
  final VoidCallback onToggle;

  const FilterSidebar({
    super.key,
    required this.isExpanded,
    required this.onToggle,
  });

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    return AnimatedContainer(
      duration: const Duration(milliseconds: 200),
      width: isExpanded ? 240 : 48,
      decoration: BoxDecoration(
        color: Colors.grey[900]?.withValues(alpha: 0.95),
        borderRadius: const BorderRadius.only(
          topLeft: Radius.circular(12),
          bottomLeft: Radius.circular(12),
        ),
        boxShadow: [
          BoxShadow(
            color: Colors.black.withValues(alpha: 0.3),
            blurRadius: 8,
            offset: const Offset(-2, 0),
          ),
        ],
      ),
      child: isExpanded ? _buildExpandedContent(ref) : _buildCollapsedContent(),
    );
  }

  Widget _buildCollapsedContent() {
    return Column(
      children: [
        IconButton(
          icon: const Icon(LucideIcons.panelRightOpen),
          onPressed: onToggle,
          tooltip: 'Expand filters',
        ),
      ],
    );
  }

  Widget _buildExpandedContent(WidgetRef ref) {
    final config = ref.watch(skyRenderConfigProvider);

    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        // Header
        Padding(
          padding: const EdgeInsets.all(12),
          child: Row(
            children: [
              const Text('Filters', style: TextStyle(fontWeight: FontWeight.bold)),
              const Spacer(),
              IconButton(
                icon: const Icon(LucideIcons.panelRightClose, size: 18),
                onPressed: onToggle,
              ),
            ],
          ),
        ),
        const Divider(height: 1),

        // Category toggles
        Expanded(
          child: ListView(
            padding: const EdgeInsets.all(12),
            children: [
              _SectionHeader('Object Types'),
              _FilterToggle(
                icon: LucideIcons.star,
                label: 'Stars',
                value: config.showStars,
                onChanged: (v) => ref.read(skyRenderConfigProvider.notifier).toggleStars(),
              ),
              _FilterToggle(
                icon: LucideIcons.circle,
                label: 'Planets',
                value: config.showPlanets,
                onChanged: (v) => ref.read(skyRenderConfigProvider.notifier).togglePlanets(),
              ),
              _FilterToggle(
                icon: LucideIcons.sparkles,
                label: 'Deep Sky Objects',
                value: config.showDsos,
                onChanged: (v) => ref.read(skyRenderConfigProvider.notifier).toggleDsos(),
              ),

              const SizedBox(height: 16),
              _SectionHeader('Overlays'),
              _FilterToggle(
                icon: LucideIcons.grid3x3,
                label: 'Grid',
                value: config.showGrid,
                onChanged: (v) => ref.read(skyRenderConfigProvider.notifier).toggleGrid(),
              ),
              _FilterToggle(
                icon: LucideIcons.network,
                label: 'Constellations',
                value: config.showConstellationLines,
                onChanged: (v) => ref.read(skyRenderConfigProvider.notifier).toggleConstellationLines(),
              ),
              _FilterToggle(
                icon: LucideIcons.mountain,
                label: 'Ground Plane',
                value: ref.watch(showGroundPlaneProvider),
                onChanged: (v) => ref.read(showGroundPlaneProvider.notifier).state = v,
              ),

              const SizedBox(height: 16),
              _SectionHeader('Magnitude Limit'),
              _buildMagnitudeSlider(ref),

              const SizedBox(height: 16),
              _FilterToggle(
                icon: LucideIcons.tag,
                label: 'Named Objects Only',
                value: false, // TODO: Add provider
                onChanged: (v) {},
              ),
            ],
          ),
        ),
      ],
    );
  }

  Widget _buildMagnitudeSlider(WidgetRef ref) {
    // TODO: Connect to magnitude limit provider
    return Slider(
      value: 8.0,
      min: 4.0,
      max: 14.0,
      divisions: 10,
      label: '8.0',
      onChanged: (v) {},
    );
  }
}

class _SectionHeader extends StatelessWidget {
  final String title;
  const _SectionHeader(this.title);

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.only(bottom: 8),
      child: Text(
        title,
        style: TextStyle(
          fontSize: 12,
          fontWeight: FontWeight.w600,
          color: Colors.white54,
        ),
      ),
    );
  }
}

class _FilterToggle extends StatelessWidget {
  final IconData icon;
  final String label;
  final bool value;
  final ValueChanged<bool> onChanged;

  const _FilterToggle({
    required this.icon,
    required this.label,
    required this.value,
    required this.onChanged,
  });

  @override
  Widget build(BuildContext context) {
    return InkWell(
      onTap: () => onChanged(!value),
      borderRadius: BorderRadius.circular(8),
      child: Padding(
        padding: const EdgeInsets.symmetric(vertical: 8),
        child: Row(
          children: [
            Icon(icon, size: 18, color: value ? Colors.white : Colors.white38),
            const SizedBox(width: 12),
            Expanded(child: Text(label)),
            Switch(
              value: value,
              onChanged: onChanged,
              materialTapTargetSize: MaterialTapTargetSize.shrinkWrap,
            ),
          ],
        ),
      ),
    );
  }
}
```

**Step 2: Run analyzer**

Run: `flutter analyze packages/nightshade_app`

**Step 3: Commit**

```bash
git add packages/nightshade_app/lib/screens/planetarium/widgets/filter_sidebar.dart
git commit -m "feat(planetarium): create collapsible filter sidebar widget"
```

---

## Task 5: Integrate Filter Sidebar into Planetarium Screen

**Files:**
- Modify: `packages/nightshade_app/lib/screens/planetarium/planetarium_screen.dart`

**Step 1: Add sidebar state and widget**

Add state for sidebar:

```dart
bool _filterSidebarExpanded = false;
```

Add sidebar to the layout (typically on the right side):

```dart
Row(
  children: [
    // Main planetarium view (existing)
    Expanded(
      child: Stack(
        children: [
          InteractiveSkyView(...),
          // HUD overlays
        ],
      ),
    ),

    // Filter sidebar (right edge)
    FilterSidebar(
      isExpanded: _filterSidebarExpanded,
      onToggle: () => setState(() => _filterSidebarExpanded = !_filterSidebarExpanded),
    ),
  ],
),
```

**Step 2: Run analyzer**

Run: `flutter analyze packages/nightshade_app`

**Step 3: Commit**

```bash
git add packages/nightshade_app/lib/screens/planetarium/planetarium_screen.dart
git commit -m "feat(planetarium): integrate filter sidebar into planetarium screen"
```

---

## Task 6: Improve Search with Instant Results

**Files:**
- Modify: `packages/nightshade_app/lib/screens/planetarium/planetarium_screen.dart`

**Step 1: Update search to show instant results**

Find the search field and enhance:

```dart
Widget _buildSearchField() {
  return Column(
    mainAxisSize: MainAxisSize.min,
    children: [
      TextField(
        controller: _searchController,
        decoration: InputDecoration(
          hintText: 'Search objects or coordinates...',
          prefixIcon: const Icon(LucideIcons.search),
          suffixIcon: _searchController.text.isNotEmpty
            ? IconButton(
                icon: const Icon(LucideIcons.x),
                onPressed: () {
                  _searchController.clear();
                  setState(() {});
                },
              )
            : null,
        ),
        onChanged: (value) => setState(() {}),
      ),

      // Instant results dropdown
      if (_searchController.text.length >= 2)
        Consumer(
          builder: (context, ref, _) {
            final results = ref.watch(objectSearchProvider(_searchController.text));
            return _buildSearchResults(results);
          },
        ),
    ],
  );
}

Widget _buildSearchResults(AsyncValue<List<CelestialObject>> results) {
  return results.when(
    data: (objects) {
      if (objects.isEmpty) {
        return const Padding(
          padding: EdgeInsets.all(16),
          child: Text('No results found'),
        );
      }

      // Group by category
      final stars = objects.whereType<Star>().take(4).toList();
      final dsos = objects.whereType<DeepSkyObject>().take(4).toList();

      return Container(
        constraints: const BoxConstraints(maxHeight: 300),
        decoration: BoxDecoration(
          color: Colors.grey[900],
          borderRadius: BorderRadius.circular(8),
          boxShadow: [
            BoxShadow(color: Colors.black54, blurRadius: 8),
          ],
        ),
        child: ListView(
          shrinkWrap: true,
          children: [
            if (stars.isNotEmpty) ...[
              _CategoryHeader('Stars'),
              ...stars.map((s) => _SearchResultTile(object: s, onTap: () => _goToObject(s))),
            ],
            if (dsos.isNotEmpty) ...[
              _CategoryHeader('Deep Sky Objects'),
              ...dsos.map((d) => _SearchResultTile(object: d, onTap: () => _goToObject(d))),
            ],
          ],
        ),
      );
    },
    loading: () => const Padding(
      padding: EdgeInsets.all(16),
      child: CircularProgressIndicator(),
    ),
    error: (e, _) => Padding(
      padding: const EdgeInsets.all(16),
      child: Text('Error: $e'),
    ),
  );
}
```

**Step 2: Add coordinate parsing support**

```dart
CelestialCoordinate? _parseCoordinates(String input) {
  // Try to parse "RA 5h 35m, Dec -5° 23'"
  final raDecPattern = RegExp(r'RA\s*(\d+)h\s*(\d+)m.*Dec\s*([+-]?\d+)°\s*(\d+)');
  final match = raDecPattern.firstMatch(input);

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
```

**Step 3: Run analyzer**

Run: `flutter analyze packages/nightshade_app`

**Step 4: Commit**

```bash
git add packages/nightshade_app/lib/screens/planetarium/planetarium_screen.dart
git commit -m "feat(planetarium): improve search with instant results and coordinate parsing"
```

---

## Task 7: Add Keyboard Shortcuts (Desktop)

**Files:**
- Modify: `packages/nightshade_app/lib/screens/planetarium/planetarium_screen.dart`

**Step 1: Add keyboard listener**

Wrap the planetarium screen with Focus and KeyboardListener:

```dart
@override
Widget build(BuildContext context) {
  return Focus(
    autofocus: true,
    onKeyEvent: _handleKeyEvent,
    child: _buildPlanetariumContent(),
  );
}

KeyEventResult _handleKeyEvent(FocusNode node, KeyEvent event) {
  if (event is! KeyDownEvent) return KeyEventResult.ignored;

  final key = event.logicalKey;

  // Arrow keys - pan view
  if (key == LogicalKeyboardKey.arrowUp) {
    _panView(0, -10);
    return KeyEventResult.handled;
  }
  if (key == LogicalKeyboardKey.arrowDown) {
    _panView(0, 10);
    return KeyEventResult.handled;
  }
  if (key == LogicalKeyboardKey.arrowLeft) {
    _panView(-10, 0);
    return KeyEventResult.handled;
  }
  if (key == LogicalKeyboardKey.arrowRight) {
    _panView(10, 0);
    return KeyEventResult.handled;
  }

  // +/- zoom
  if (key == LogicalKeyboardKey.equal || key == LogicalKeyboardKey.add) {
    _zoomIn();
    return KeyEventResult.handled;
  }
  if (key == LogicalKeyboardKey.minus) {
    _zoomOut();
    return KeyEventResult.handled;
  }

  // R - reset view
  if (key == LogicalKeyboardKey.keyR) {
    _resetView();
    return KeyEventResult.handled;
  }

  // G - toggle grid
  if (key == LogicalKeyboardKey.keyG) {
    ref.read(skyRenderConfigProvider.notifier).toggleGrid();
    return KeyEventResult.handled;
  }

  // C - toggle constellations
  if (key == LogicalKeyboardKey.keyC) {
    ref.read(skyRenderConfigProvider.notifier).toggleConstellationLines();
    return KeyEventResult.handled;
  }

  // M - toggle mini-map
  if (key == LogicalKeyboardKey.keyM) {
    ref.read(showMinimapProvider.notifier).state = !ref.read(showMinimapProvider);
    return KeyEventResult.handled;
  }

  // F - center on mount
  if (key == LogicalKeyboardKey.keyF) {
    _centerOnMount();
    return KeyEventResult.handled;
  }

  // Escape - clear selection
  if (key == LogicalKeyboardKey.escape) {
    _clearSelection();
    return KeyEventResult.handled;
  }

  return KeyEventResult.ignored;
}
```

**Step 2: Implement helper methods**

```dart
void _panView(double dx, double dy) {
  final viewState = ref.read(skyViewStateProvider);
  final panAmount = viewState.fieldOfView / 20; // Pan amount relative to FOV

  ref.read(skyViewStateProvider.notifier).setCenter(
    viewState.centerRA + dx * panAmount / 15, // Convert to hours
    (viewState.centerDec + dy * panAmount).clamp(-90, 90),
  );
}

void _zoomIn() {
  final viewState = ref.read(skyViewStateProvider);
  final newFov = (viewState.fieldOfView * 0.8).clamp(1.0, 120.0);
  ref.read(skyViewStateProvider.notifier).setFieldOfView(newFov);
}

void _zoomOut() {
  final viewState = ref.read(skyViewStateProvider);
  final newFov = (viewState.fieldOfView * 1.25).clamp(1.0, 120.0);
  ref.read(skyViewStateProvider.notifier).setFieldOfView(newFov);
}

void _resetView() {
  ref.read(skyViewStateProvider.notifier).reset();
}

void _centerOnMount() {
  final mountPos = ref.read(mountPositionProvider);
  if (mountPos != null) {
    ref.read(skyViewStateProvider.notifier).setCenter(mountPos.ra, mountPos.dec);
  }
}

void _clearSelection() {
  ref.read(selectedObjectProvider.notifier).state = null;
  setState(() => _slewMode = false);
}
```

**Step 3: Run analyzer**

Run: `flutter analyze packages/nightshade_app`

**Step 4: Commit**

```bash
git add packages/nightshade_app/lib/screens/planetarium/planetarium_screen.dart
git commit -m "feat(planetarium): add keyboard shortcuts for desktop navigation"
```

---

## Task 8: Integration Test

**Files:**
- None (manual testing)

**Step 1: Build and run**

Run: `melos run dev`

**Step 2: Manual verification checklist**

- [ ] Object popup shows DSS thumbnail (or placeholder for objects without image)
- [ ] Quick stats bar shows altitude, transit time, moon distance
- [ ] Visibility indicator shows with correct color coding
- [ ] Tooltip explains visibility rating factors
- [ ] Filter sidebar expands/collapses smoothly
- [ ] Filter toggles work correctly
- [ ] Search shows instant results as you type
- [ ] Results grouped by category (Stars, DSOs)
- [ ] Coordinate input works (e.g., "RA 5h 35m, Dec -5° 23'")
- [ ] Arrow keys pan the view
- [ ] +/- zoom in/out
- [ ] R resets view
- [ ] G toggles grid
- [ ] C toggles constellations
- [ ] M toggles mini-map
- [ ] F centers on mount position
- [ ] Escape clears selection

**Step 3: Final commit if fixes needed**

```bash
git add -A
git commit -m "fix(planetarium): polish UI/UX improvements"
```

---

## Summary

This plan implements Phase 4 UI/UX Improvements with:
1. DSS thumbnail images in object popups
2. Quick stats bar (altitude, transit, moon distance)
3. Visibility score indicator with tooltip
4. Collapsible filter sidebar
5. Improved search with instant results and coordinate input
6. Full keyboard shortcuts for desktop
