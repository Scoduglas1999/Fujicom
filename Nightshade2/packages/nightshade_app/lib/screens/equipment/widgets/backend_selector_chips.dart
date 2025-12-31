import 'package:flutter/material.dart';
import 'package:nightshade_core/nightshade_core.dart';
import 'package:nightshade_ui/nightshade_ui.dart';

/// A row of selectable chips for choosing which driver backend to use.
///
/// Shows all available backends for a device, with the recommended one
/// marked with a star icon. The selected backend is highlighted.
class BackendSelectorChips extends StatelessWidget {
  /// List of available backends for this device
  final List<DriverBackend> availableBackends;

  /// Currently selected backend
  final DriverBackend selectedBackend;

  /// The recommended backend (usually Native or best available)
  final DriverBackend recommendedBackend;

  /// Called when user selects a different backend
  final ValueChanged<DriverBackend> onBackendSelected;

  /// Whether the selector is enabled
  final bool isEnabled;

  const BackendSelectorChips({
    super.key,
    required this.availableBackends,
    required this.selectedBackend,
    required this.recommendedBackend,
    required this.onBackendSelected,
    this.isEnabled = true,
  });

  @override
  Widget build(BuildContext context) {
    final colors = Theme.of(context).extension<NightshadeColors>()!;

    if (availableBackends.isEmpty) {
      return const SizedBox.shrink();
    }

    // Sort backends by priority (Native first)
    final sortedBackends = List<DriverBackend>.from(availableBackends);
    sortedBackends.sort((a, b) => _backendPriority(a).compareTo(_backendPriority(b)));

    return Row(
      mainAxisSize: MainAxisSize.min,
      children: [
        Text(
          'Driver:',
          style: TextStyle(
            fontSize: 11,
            color: colors.textSecondary,
          ),
        ),
        const SizedBox(width: 8),
        Wrap(
          spacing: 6,
          runSpacing: 4,
          children: sortedBackends.map((backend) {
            final isSelected = backend == selectedBackend;
            final isRecommended = backend == recommendedBackend;

            return _BackendChip(
              backend: backend,
              isSelected: isSelected,
              isRecommended: isRecommended,
              isEnabled: isEnabled,
              onTap: () => onBackendSelected(backend),
            );
          }).toList(),
        ),
      ],
    );
  }

  int _backendPriority(DriverBackend backend) {
    switch (backend) {
      case DriverBackend.native:
        return 0;
      case DriverBackend.ascom:
        return 1;
      case DriverBackend.alpaca:
        return 2;
      case DriverBackend.indi:
        return 3;
      case DriverBackend.simulator:
        return 4;
    }
  }
}

class _BackendChip extends StatefulWidget {
  final DriverBackend backend;
  final bool isSelected;
  final bool isRecommended;
  final bool isEnabled;
  final VoidCallback onTap;

  const _BackendChip({
    required this.backend,
    required this.isSelected,
    required this.isRecommended,
    required this.isEnabled,
    required this.onTap,
  });

  @override
  State<_BackendChip> createState() => _BackendChipState();
}

class _BackendChipState extends State<_BackendChip> {
  bool _isHovered = false;

  Color _getBackendColor(NightshadeColors colors) {
    switch (widget.backend) {
      case DriverBackend.native:
        return colors.success; // Green for native (best)
      case DriverBackend.ascom:
        return colors.info; // Blue for ASCOM
      case DriverBackend.alpaca:
        return colors.warning; // Orange for Alpaca
      case DriverBackend.indi:
        return const Color(0xFF9333EA); // Purple for INDI
      case DriverBackend.simulator:
        return colors.textMuted; // Gray for simulator
    }
  }

  @override
  Widget build(BuildContext context) {
    final colors = Theme.of(context).extension<NightshadeColors>()!;
    final backendColor = _getBackendColor(colors);

    final backgroundColor = widget.isSelected
        ? backendColor.withValues(alpha: 0.2)
        : _isHovered
            ? colors.surfaceHover
            : colors.surfaceAlt;

    final borderColor = widget.isSelected
        ? backendColor.withValues(alpha: 0.5)
        : colors.border;

    final textColor = widget.isSelected
        ? backendColor
        : widget.isEnabled
            ? colors.textSecondary
            : colors.textMuted;

    return Tooltip(
      message: widget.backend.description,
      waitDuration: const Duration(milliseconds: 500),
      child: MouseRegion(
        onEnter: (_) => setState(() => _isHovered = true),
        onExit: (_) => setState(() => _isHovered = false),
        cursor: widget.isEnabled
            ? SystemMouseCursors.click
            : SystemMouseCursors.forbidden,
        child: GestureDetector(
          onTap: widget.isEnabled ? widget.onTap : null,
          child: AnimatedContainer(
            duration: const Duration(milliseconds: 150),
            padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 4),
            decoration: BoxDecoration(
              color: backgroundColor,
              borderRadius: BorderRadius.circular(12),
              border: Border.all(color: borderColor, width: 1),
            ),
            child: Row(
              mainAxisSize: MainAxisSize.min,
              children: [
                if (widget.isRecommended) ...[
                  Icon(
                    Icons.star,
                    size: 10,
                    color: widget.isSelected ? backendColor : colors.warning,
                  ),
                  const SizedBox(width: 3),
                ],
                Text(
                  widget.backend.shortLabel,
                  style: TextStyle(
                    fontSize: 11,
                    fontWeight:
                        widget.isSelected ? FontWeight.w600 : FontWeight.w500,
                    color: textColor,
                  ),
                ),
                if (widget.isSelected) ...[
                  const SizedBox(width: 3),
                  Icon(
                    Icons.check,
                    size: 10,
                    color: backendColor,
                  ),
                ],
              ],
            ),
          ),
        ),
      ),
    );
  }
}

/// A compact version of BackendSelectorChips for use in tight spaces
class CompactBackendSelector extends StatelessWidget {
  final List<DriverBackend> availableBackends;
  final DriverBackend selectedBackend;
  final DriverBackend recommendedBackend;
  final ValueChanged<DriverBackend> onBackendSelected;
  final bool isEnabled;

  const CompactBackendSelector({
    super.key,
    required this.availableBackends,
    required this.selectedBackend,
    required this.recommendedBackend,
    required this.onBackendSelected,
    this.isEnabled = true,
  });

  @override
  Widget build(BuildContext context) {
    final colors = Theme.of(context).extension<NightshadeColors>()!;

    if (availableBackends.length <= 1) {
      // Only one backend, show as a simple badge
      return Container(
        padding: const EdgeInsets.symmetric(horizontal: 6, vertical: 2),
        decoration: BoxDecoration(
          color: colors.surfaceAlt,
          borderRadius: BorderRadius.circular(8),
        ),
        child: Text(
          selectedBackend.shortLabel,
          style: TextStyle(
            fontSize: 10,
            color: colors.textSecondary,
          ),
        ),
      );
    }

    // Multiple backends available - show as dropdown
    return PopupMenuButton<DriverBackend>(
      initialValue: selectedBackend,
      onSelected: isEnabled ? onBackendSelected : null,
      enabled: isEnabled,
      tooltip: 'Select driver',
      position: PopupMenuPosition.under,
      constraints: const BoxConstraints(minWidth: 120),
      itemBuilder: (context) {
        return availableBackends.map((backend) {
          final isRecommended = backend == recommendedBackend;
          return PopupMenuItem<DriverBackend>(
            value: backend,
            child: Row(
              children: [
                if (isRecommended)
                  Icon(Icons.star, size: 12, color: colors.warning)
                else
                  const SizedBox(width: 12),
                const SizedBox(width: 6),
                Text(backend.displayName),
                const Spacer(),
                if (backend == selectedBackend)
                  Icon(Icons.check, size: 14, color: colors.success),
              ],
            ),
          );
        }).toList();
      },
      child: Container(
        padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 4),
        decoration: BoxDecoration(
          color: colors.surfaceAlt,
          borderRadius: BorderRadius.circular(8),
          border: Border.all(color: colors.border),
        ),
        child: Row(
          mainAxisSize: MainAxisSize.min,
          children: [
            Text(
              selectedBackend.shortLabel,
              style: TextStyle(
                fontSize: 11,
                color: colors.textSecondary,
              ),
            ),
            const SizedBox(width: 4),
            Icon(
              Icons.arrow_drop_down,
              size: 14,
              color: colors.textSecondary,
            ),
          ],
        ),
      ),
    );
  }
}
