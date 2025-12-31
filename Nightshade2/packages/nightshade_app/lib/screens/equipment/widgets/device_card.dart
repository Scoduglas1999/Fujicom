import 'package:flutter/material.dart';
import 'package:lucide_icons/lucide_icons.dart';
import 'package:nightshade_ui/nightshade_ui.dart';

enum DeviceType {
  camera,
  telescope,
  mount,
  focuser,
  filterWheel,
  guider,
  weather,
  dome,
  rotator,
}

class DeviceCard extends StatelessWidget {
  final String title;
  final DeviceType deviceType;
  final bool isConnected;
  final String? selectedDevice;
  final List<String> availableDevices;
  final ValueChanged<String?> onDeviceSelected;
  final VoidCallback? onConnect;
  final VoidCallback? onDisconnect;
  final Widget statusWidget;
  final bool isOptional;
  final bool showEditButton;
  final VoidCallback? onEdit;

  const DeviceCard({
    super.key,
    required this.title,
    required this.deviceType,
    required this.isConnected,
    required this.selectedDevice,
    required this.availableDevices,
    required this.onDeviceSelected,
    required this.onConnect,
    required this.onDisconnect,
    required this.statusWidget,
    this.isOptional = false,
    this.showEditButton = false,
    this.onEdit,
  });

  IconData get _deviceIcon {
    switch (deviceType) {
      case DeviceType.camera:
        return LucideIcons.camera;
      case DeviceType.telescope:
        return LucideIcons.scan;
      case DeviceType.mount:
        return LucideIcons.move3d;
      case DeviceType.focuser:
        return LucideIcons.focus;
      case DeviceType.filterWheel:
        return LucideIcons.circle;
      case DeviceType.guider:
        return LucideIcons.crosshair;
      case DeviceType.weather:
        return LucideIcons.cloudSun;
      case DeviceType.dome:
        return LucideIcons.home;
      case DeviceType.rotator:
        return LucideIcons.rotateCw;
    }
  }

  @override
  Widget build(BuildContext context) {
    final colors = Theme.of(context).extension<NightshadeColors>()!;

    return NightshadeCard(
      child: Padding(
        padding: const EdgeInsets.all(16),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            // Header
            Row(
              children: [
                Container(
                  width: 32,
                  height: 32,
                  decoration: BoxDecoration(
                    color: isConnected
                        ? colors.success.withValues(alpha: 0.1)
                        : colors.surfaceAlt,
                    borderRadius: BorderRadius.circular(8),
                  ),
                  child: Icon(
                    _deviceIcon,
                    size: 16,
                    color: isConnected ? colors.success : colors.textSecondary,
                  ),
                ),
                const SizedBox(width: 12),
                Expanded(
                  child: Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: [
                      Row(
                        children: [
                          Text(
                            title,
                            style: TextStyle(
                              fontSize: 14,
                              fontWeight: FontWeight.w600,
                              color: colors.textPrimary,
                            ),
                          ),
                          if (isOptional) ...[
                            const SizedBox(width: 8),
                            Container(
                              padding: const EdgeInsets.symmetric(
                                horizontal: 6,
                                vertical: 2,
                              ),
                              decoration: BoxDecoration(
                                color: colors.surfaceAlt,
                                borderRadius: BorderRadius.circular(4),
                              ),
                              child: Text(
                                'Optional',
                                style: TextStyle(
                                  fontSize: 10,
                                  color: colors.textSecondary,
                                ),
                              ),
                            ),
                          ],
                        ],
                      ),
                      const SizedBox(height: 2),
                      Row(
                        children: [
                          Container(
                            width: 6,
                            height: 6,
                            decoration: BoxDecoration(
                              shape: BoxShape.circle,
                              color: isConnected
                                  ? colors.success
                                  : colors.textMuted,
                            ),
                          ),
                          const SizedBox(width: 6),
                          Text(
                            isConnected ? 'Connected' : 'Disconnected',
                            style: TextStyle(
                              fontSize: 11,
                              color: colors.textSecondary,
                            ),
                          ),
                        ],
                      ),
                    ],
                  ),
                ),
                if (showEditButton)
                  IconButton(
                    icon: Icon(
                      LucideIcons.pencil,
                      size: 14,
                      color: colors.textSecondary,
                    ),
                    onPressed: onEdit,
                    tooltip: 'Edit',
                  ),
              ],
            ),

            const SizedBox(height: 16),

            // Device selector
            NightshadeDropdown(
              value: selectedDevice,
              hint: 'Select device...',
              items: availableDevices,
              onChanged: onDeviceSelected,
              isExpanded: true,
            ),

            const SizedBox(height: 16),

            // Status
            DefaultTextStyle(
              style: TextStyle(
                fontSize: 12,
                color: colors.textSecondary,
              ),
              child: statusWidget,
            ),

            if (onConnect != null || onDisconnect != null) ...[
              const SizedBox(height: 16),

              // Connect/Disconnect button
              SizedBox(
                width: double.infinity,
                child: isConnected
                    ? NightshadeButton(
                        label: 'Disconnect',
                        variant: ButtonVariant.outline,
                        onPressed: onDisconnect,
                      )
                    : NightshadeButton(
                        label: 'Connect',
                        onPressed: selectedDevice != null ? onConnect : null,
                      ),
              ),
            ],
          ],
        ),
      ),
    );
  }
}

