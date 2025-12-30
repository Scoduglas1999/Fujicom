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
      width: isExpanded ? 220 : 48,
      decoration: BoxDecoration(
        color: Colors.grey[900]?.withValues(alpha: 0.95),
        borderRadius: const BorderRadius.only(
          topLeft: Radius.circular(12),
          bottomLeft: Radius.circular(12),
        ),
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
              const Text('Filters',
                  style: TextStyle(fontWeight: FontWeight.bold)),
              const Spacer(),
              IconButton(
                icon: const Icon(LucideIcons.panelRightClose, size: 18),
                onPressed: onToggle,
              ),
            ],
          ),
        ),
        const Divider(height: 1),

        // Toggles
        Expanded(
          child: ListView(
            padding: const EdgeInsets.all(12),
            children: [
              _FilterToggle(
                label: 'Stars',
                value: config.showStars,
                onChanged: (_) =>
                    ref.read(skyRenderConfigProvider.notifier).toggleStars(),
              ),
              _FilterToggle(
                label: 'Planets',
                value: config.showPlanets,
                onChanged: (_) =>
                    ref.read(skyRenderConfigProvider.notifier).togglePlanets(),
              ),
              _FilterToggle(
                label: 'Deep Sky',
                value: config.showDSOs,
                onChanged: (_) =>
                    ref.read(skyRenderConfigProvider.notifier).toggleDSOs(),
              ),
              const Divider(),
              _FilterToggle(
                label: 'Grid',
                value: config.showCoordinateGrid,
                onChanged: (_) =>
                    ref.read(skyRenderConfigProvider.notifier).toggleGrid(),
              ),
              _FilterToggle(
                label: 'Constellations',
                value: config.showConstellationLines,
                onChanged: (_) => ref
                    .read(skyRenderConfigProvider.notifier)
                    .toggleConstellationLines(),
              ),
              _FilterToggle(
                label: 'Ground',
                value: ref.watch(showGroundPlaneProvider),
                onChanged: (v) =>
                    ref.read(showGroundPlaneProvider.notifier).state = v,
              ),
            ],
          ),
        ),
      ],
    );
  }
}

class _FilterToggle extends StatelessWidget {
  final String label;
  final bool value;
  final ValueChanged<bool> onChanged;

  const _FilterToggle({
    required this.label,
    required this.value,
    required this.onChanged,
  });

  @override
  Widget build(BuildContext context) {
    return SwitchListTile(
      title: Text(label),
      value: value,
      onChanged: onChanged,
      dense: true,
      contentPadding: EdgeInsets.zero,
    );
  }
}
