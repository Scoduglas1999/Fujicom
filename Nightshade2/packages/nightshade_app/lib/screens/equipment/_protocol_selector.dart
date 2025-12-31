import 'package:flutter/material.dart';
import 'dart:io';
import 'package:nightshade_ui/nightshade_ui.dart';
import 'equipment_screen.dart';

// Protocol Selector Widget
class ProtocolSelector extends StatelessWidget {
  final DeviceProtocol selectedProtocol;
  final ValueChanged<DeviceProtocol> onProtocolChanged;
  final NightshadeColors colors;

  const ProtocolSelector({super.key, 
    required this.selectedProtocol,
    required this.onProtocolChanged,
    required this.colors,
  });

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.all(4),
      decoration: BoxDecoration(
        color: colors.surfaceAlt,
        borderRadius: BorderRadius.circular(10),
      ),
      child: Row(
        mainAxisSize: MainAxisSize.min,
        children: [
          // ASCOM (Windows only)
          if (Platform.isWindows)
            _ProtocolButton(
              protocol: DeviceProtocol.ascom,
              label: 'ASCOM',
              icon: Icons.desktop_windows,
              isSelected: selectedProtocol == DeviceProtocol.ascom,
              onTap: () => onProtocolChanged(DeviceProtocol.ascom),
              colors: colors,
            ),
          
          // Native (Cross-platform, vendor SDKs)
          _ProtocolButton(
            protocol: DeviceProtocol.native,
            label: 'Native',
            icon: Icons.usb,
            isSelected: selectedProtocol == DeviceProtocol.native,
            onTap: () => onProtocolChanged(DeviceProtocol.native),
            colors: colors,
          ),

          // Alpaca (All platforms)
          _ProtocolButton(
            protocol: DeviceProtocol.alpaca,
            label: 'Alpaca',
            icon: Icons.cloud_outlined,
            isSelected: selectedProtocol == DeviceProtocol.alpaca,
            onTap: () => onProtocolChanged(DeviceProtocol.alpaca),
            colors: colors,
          ),
          
          // INDI (Cross-platform)
          _ProtocolButton(
            protocol: DeviceProtocol.indi,
            label: 'INDI',
            icon: Icons.power,
            isSelected: selectedProtocol == DeviceProtocol.indi,
            onTap: () => onProtocolChanged(DeviceProtocol.indi),
            colors: colors,
          ),
        ],
      ),
    );
  }
}

class _ProtocolButton extends StatefulWidget {
  final DeviceProtocol protocol;
  final String label;
  final IconData icon;
  final bool isSelected;
  final VoidCallback onTap;
  final NightshadeColors colors;

  const _ProtocolButton({
    required this.protocol,
    required this.label,
    required this.icon,
    required this.isSelected,
    required this.onTap,
    required this.colors,
  });

  @override
  State<_ProtocolButton> createState() => _ProtocolButtonState();
}

class _ProtocolButtonState extends State<_ProtocolButton> {
  bool _isHovered = false;

  @override
  Widget build(BuildContext context) {
    return Semantics(
      button: true,
      selected: widget.isSelected,
      label: widget.label,
      child: MouseRegion(
        onEnter: (_) => setState(() => _isHovered = true),
        onExit: (_) => setState(() => _isHovered = false),
        child: AnimatedContainer(
          duration: const Duration(milliseconds: 200),
          margin: const EdgeInsets.symmetric(horizontal: 2),
          decoration: BoxDecoration(
            color: widget.isSelected
                ? widget.colors.surface
                : _isHovered
                    ? widget.colors.surface.withValues(alpha: 0.5)
                    : Colors.transparent,
            borderRadius: BorderRadius.circular(6),
            boxShadow: widget.isSelected
                ? [
                    BoxShadow(
                      color: Colors.black.withValues(alpha: 0.1),
                      blurRadius: 3,
                      offset: const Offset(0, 1),
                    ),
                  ]
                : null,
          ),
          child: Material(
            type: MaterialType.transparency,
            child: InkWell(
              onTap: widget.onTap,
              borderRadius: BorderRadius.circular(6),
              hoverColor: Colors.transparent, // Handled by AnimatedContainer
              highlightColor: widget.colors.primary.withValues(alpha: 0.1),
              splashColor: widget.colors.primary.withValues(alpha: 0.1),
              child: Padding(
                padding: const EdgeInsets.symmetric(horizontal: 14, vertical: 8),
                child: Row(
                  mainAxisSize: MainAxisSize.min,
                  children: [
                    Icon(
                      widget.icon,
                      size: 14,
                      color: widget.isSelected
                          ? widget.colors.primary
                          : widget.colors.textSecondary,
                    ),
                    const SizedBox(width: 6),
                    Text(
                      widget.label,
                      style: TextStyle(
                        fontSize: 11,
                        fontWeight:
                            widget.isSelected ? FontWeight.w600 : FontWeight.w500,
                        color: widget.isSelected
                            ? widget.colors.textPrimary
                            : widget.colors.textSecondary,
                      ),
                    ),
                  ],
                ),
              ),
            ),
          ),
        ),
      ),
    );
  }
}
