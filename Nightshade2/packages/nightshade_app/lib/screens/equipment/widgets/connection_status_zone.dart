import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:lucide_icons/lucide_icons.dart';
import 'package:nightshade_core/nightshade_core.dart';
import 'package:nightshade_core/src/database/database.dart' as db;
import 'package:nightshade_ui/nightshade_ui.dart';

/// The adaptive connection status zone that changes based on connection state.
///
/// States:
/// - Disconnected (expanded): Shows profile preview with device list and "Connect All" button
/// - Connecting (animated): Shows live progress for each device
/// - Connected (compact): Shows minimal status bar
/// - Error (attention required): Shows error with retry options
class ConnectionStatusZone extends ConsumerStatefulWidget {
  final db.EquipmentProfile? selectedProfile;
  final VoidCallback onConnectAll;
  final VoidCallback onDisconnectAll;
  final VoidCallback onEditProfile;

  const ConnectionStatusZone({
    super.key,
    required this.selectedProfile,
    required this.onConnectAll,
    required this.onDisconnectAll,
    required this.onEditProfile,
  });

  @override
  ConsumerState<ConnectionStatusZone> createState() => _ConnectionStatusZoneState();
}

class _ConnectionStatusZoneState extends ConsumerState<ConnectionStatusZone>
    with SingleTickerProviderStateMixin {
  late AnimationController _expandController;
  late Animation<double> _expandAnimation;
  bool _isExpanded = true;

  @override
  void initState() {
    super.initState();
    _expandController = AnimationController(
      vsync: this,
      duration: const Duration(milliseconds: 300),
    );
    _expandAnimation = CurvedAnimation(
      parent: _expandController,
      curve: Curves.easeInOut,
    );
    _expandController.value = 1.0;
  }

  @override
  void dispose() {
    _expandController.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final colors = Theme.of(context).extension<NightshadeColors>()!;

    if (widget.selectedProfile == null) {
      return _NoProfileSelected(colors: colors);
    }

    // Watch device states
    final cameraState = ref.watch(cameraStateProvider);
    final mountState = ref.watch(mountStateProvider);
    final focuserState = ref.watch(focuserStateProvider);
    final filterWheelState = ref.watch(filterWheelStateProvider);
    final guiderState = ref.watch(guiderStateProvider);

    // Build device list
    final devices = _buildDeviceList(
      widget.selectedProfile!,
      cameraState,
      mountState,
      focuserState,
      filterWheelState,
      guiderState,
    );

    // Calculate overall state
    final (overallState, connectedCount, totalCount, errorDevice) =
        _calculateOverallState(devices);

    // Auto-collapse/expand based on state
    WidgetsBinding.instance.addPostFrameCallback((_) {
      if (overallState == _OverallState.connected && _isExpanded) {
        _collapse();
      } else if (overallState != _OverallState.connected && !_isExpanded) {
        _expand();
      }
    });

    return AnimatedContainer(
      duration: const Duration(milliseconds: 300),
      decoration: BoxDecoration(
        color: colors.surface,
        border: Border(
          bottom: BorderSide(color: colors.border),
        ),
      ),
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          // Compact bar (always visible when collapsed, clickable to expand)
          if (!_isExpanded || overallState == _OverallState.connected)
            _CompactStatusBar(
              connectedCount: connectedCount,
              totalCount: totalCount,
              overallState: overallState,
              devices: devices,
              colors: colors,
              onTap: _toggle,
              onDisconnect: widget.onDisconnectAll,
            ),

          // Expandable content
          SizeTransition(
            sizeFactor: _expandAnimation,
            axisAlignment: -1.0,
            child: _buildExpandedContent(
              context,
              colors,
              devices,
              overallState,
              connectedCount,
              totalCount,
              errorDevice,
            ),
          ),
        ],
      ),
    );
  }

  Widget _buildExpandedContent(
    BuildContext context,
    NightshadeColors colors,
    List<_DeviceStatus> devices,
    _OverallState overallState,
    int connectedCount,
    int totalCount,
    _DeviceStatus? errorDevice,
  ) {
    switch (overallState) {
      case _OverallState.disconnected:
        return _DisconnectedView(
          profile: widget.selectedProfile!,
          devices: devices,
          colors: colors,
          onConnect: widget.onConnectAll,
          onEdit: widget.onEditProfile,
        );
      case _OverallState.connecting:
        return _ConnectingView(
          profile: widget.selectedProfile!,
          devices: devices,
          colors: colors,
          onCancel: widget.onDisconnectAll,
        );
      case _OverallState.connected:
        // Compact bar handles this
        return const SizedBox.shrink();
      case _OverallState.partiallyConnected:
      case _OverallState.error:
        return _ErrorView(
          devices: devices,
          errorDevice: errorDevice,
          connectedCount: connectedCount,
          totalCount: totalCount,
          colors: colors,
          onRetry: widget.onConnectAll,
          onSkip: () {}, // TODO: Implement skip
        );
    }
  }

  void _expand() {
    setState(() => _isExpanded = true);
    _expandController.forward();
  }

  void _collapse() {
    setState(() => _isExpanded = false);
    _expandController.reverse();
  }

  void _toggle() {
    if (_isExpanded) {
      _collapse();
    } else {
      _expand();
    }
  }

  List<_DeviceStatus> _buildDeviceList(
    db.EquipmentProfile profile,
    CameraState camera,
    MountState mount,
    FocuserState focuser,
    FilterWheelState filterWheel,
    GuiderState guider,
  ) {
    final devices = <_DeviceStatus>[];

    if (profile.cameraId != null) {
      devices.add(_DeviceStatus(
        type: 'Camera',
        name: camera.deviceName ?? _formatDeviceId(profile.cameraId!),
        icon: LucideIcons.camera,
        state: camera.connectionState,
        error: null, // TODO: Get actual error
      ));
    }

    if (profile.mountId != null) {
      devices.add(_DeviceStatus(
        type: 'Mount',
        name: mount.deviceName ?? _formatDeviceId(profile.mountId!),
        icon: LucideIcons.compass,
        state: mount.connectionState,
        error: null,
      ));
    }

    if (profile.focuserId != null) {
      devices.add(_DeviceStatus(
        type: 'Focuser',
        name: focuser.deviceName ?? _formatDeviceId(profile.focuserId!),
        icon: LucideIcons.focus,
        state: focuser.connectionState,
        error: null,
      ));
    }

    if (profile.filterWheelId != null) {
      devices.add(_DeviceStatus(
        type: 'Filter Wheel',
        name: filterWheel.deviceName ?? _formatDeviceId(profile.filterWheelId!),
        icon: LucideIcons.circle,
        state: filterWheel.connectionState,
        error: null,
      ));
    }

    if (profile.guiderId != null) {
      devices.add(_DeviceStatus(
        type: 'Guider',
        name: guider.deviceName ?? _formatDeviceId(profile.guiderId!),
        icon: LucideIcons.crosshair,
        state: guider.connectionState,
        error: null,
      ));
    }

    return devices;
  }

  String _formatDeviceId(String id) {
    if (id.contains('.')) {
      return id.split('.').last;
    }
    return id;
  }

  (_OverallState, int, int, _DeviceStatus?) _calculateOverallState(
    List<_DeviceStatus> devices,
  ) {
    if (devices.isEmpty) {
      return (_OverallState.disconnected, 0, 0, null);
    }

    int connected = 0;
    int connecting = 0;
    int error = 0;
    _DeviceStatus? errorDevice;

    for (final device in devices) {
      switch (device.state) {
        case DeviceConnectionState.connected:
          connected++;
          break;
        case DeviceConnectionState.connecting:
          connecting++;
          break;
        case DeviceConnectionState.error:
          error++;
          errorDevice ??= device;
          break;
        case DeviceConnectionState.disconnected:
          break;
      }
    }

    final total = devices.length;

    if (connecting > 0) {
      return (_OverallState.connecting, connected, total, null);
    }

    if (error > 0 && connected == 0) {
      return (_OverallState.error, connected, total, errorDevice);
    }

    if (connected == total) {
      return (_OverallState.connected, connected, total, null);
    }

    if (connected > 0 || error > 0) {
      return (_OverallState.partiallyConnected, connected, total, errorDevice);
    }

    return (_OverallState.disconnected, 0, total, null);
  }
}

enum _OverallState {
  disconnected,
  connecting,
  connected,
  partiallyConnected,
  error,
}

class _DeviceStatus {
  final String type;
  final String name;
  final IconData icon;
  final DeviceConnectionState state;
  final String? error;

  _DeviceStatus({
    required this.type,
    required this.name,
    required this.icon,
    required this.state,
    this.error,
  });
}

class _NoProfileSelected extends StatelessWidget {
  final NightshadeColors colors;

  const _NoProfileSelected({required this.colors});

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.all(24),
      child: Row(
        children: [
          Container(
            width: 48,
            height: 48,
            decoration: BoxDecoration(
              color: colors.surfaceAlt,
              borderRadius: BorderRadius.circular(12),
            ),
            child: Icon(
              LucideIcons.info,
              color: colors.textMuted,
              size: 24,
            ),
          ),
          const SizedBox(width: 16),
          Expanded(
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Text(
                  'No Profile Selected',
                  style: TextStyle(
                    fontSize: 16,
                    fontWeight: FontWeight.w600,
                    color: colors.textPrimary,
                  ),
                ),
                const SizedBox(height: 4),
                Text(
                  'Select a profile above to view and connect your equipment',
                  style: TextStyle(
                    fontSize: 13,
                    color: colors.textSecondary,
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

class _CompactStatusBar extends StatelessWidget {
  final int connectedCount;
  final int totalCount;
  final _OverallState overallState;
  final List<_DeviceStatus> devices;
  final NightshadeColors colors;
  final VoidCallback onTap;
  final VoidCallback onDisconnect;

  const _CompactStatusBar({
    required this.connectedCount,
    required this.totalCount,
    required this.overallState,
    required this.devices,
    required this.colors,
    required this.onTap,
    required this.onDisconnect,
  });

  @override
  Widget build(BuildContext context) {
    final statusColor = overallState == _OverallState.connected
        ? colors.success
        : overallState == _OverallState.error
            ? colors.error
            : colors.warning;

    return Material(
      color: Colors.transparent,
      child: InkWell(
        onTap: onTap,
        child: Container(
          padding: const EdgeInsets.symmetric(horizontal: 20, vertical: 12),
          child: Row(
            children: [
              // Status indicator
              Container(
                width: 8,
                height: 8,
                decoration: BoxDecoration(
                  shape: BoxShape.circle,
                  color: statusColor,
                ),
              ),
              const SizedBox(width: 12),

              // Status text
              Text(
                overallState == _OverallState.connected
                    ? 'All Connected'
                    : '$connectedCount/$totalCount Connected',
                style: TextStyle(
                  fontSize: 13,
                  fontWeight: FontWeight.w600,
                  color: statusColor,
                ),
              ),

              const SizedBox(width: 16),

              // Device dots
              ...devices.map((device) {
                final dotColor = device.state == DeviceConnectionState.connected
                    ? colors.success
                    : device.state == DeviceConnectionState.error
                        ? colors.error
                        : colors.textMuted.withValues(alpha: 0.5);

                return Padding(
                  padding: const EdgeInsets.only(right: 6),
                  child: Tooltip(
                    message: '${device.type}: ${device.name}',
                    child: Container(
                      width: 8,
                      height: 8,
                      decoration: BoxDecoration(
                        shape: BoxShape.circle,
                        color: dotColor,
                      ),
                    ),
                  ),
                );
              }),

              const Spacer(),

              // Disconnect button (only when connected)
              if (overallState == _OverallState.connected)
                TextButton(
                  onPressed: onDisconnect,
                  child: Text(
                    'Disconnect',
                    style: TextStyle(
                      fontSize: 12,
                      color: colors.textSecondary,
                    ),
                  ),
                ),

              Icon(
                LucideIcons.chevronDown,
                size: 16,
                color: colors.textMuted,
              ),
            ],
          ),
        ),
      ),
    );
  }
}

class _DisconnectedView extends StatelessWidget {
  final db.EquipmentProfile profile;
  final List<_DeviceStatus> devices;
  final NightshadeColors colors;
  final VoidCallback onConnect;
  final VoidCallback onEdit;

  const _DisconnectedView({
    required this.profile,
    required this.devices,
    required this.colors,
    required this.onConnect,
    required this.onEdit,
  });

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.all(20),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Text(
            profile.name,
            style: TextStyle(
              fontSize: 16,
              fontWeight: FontWeight.w600,
              color: colors.textPrimary,
            ),
          ),
          const SizedBox(height: 16),

          // Device list
          ...devices.map((device) => Padding(
                padding: const EdgeInsets.only(bottom: 8),
                child: Row(
                  children: [
                    Icon(device.icon, size: 14, color: colors.textMuted),
                    const SizedBox(width: 12),
                    Text(
                      '${device.type}:',
                      style: TextStyle(
                        fontSize: 13,
                        color: colors.textSecondary,
                      ),
                    ),
                    const SizedBox(width: 8),
                    Expanded(
                      child: Text(
                        device.name,
                        style: TextStyle(
                          fontSize: 13,
                          color: colors.textPrimary,
                        ),
                        overflow: TextOverflow.ellipsis,
                      ),
                    ),
                  ],
                ),
              )),

          if (devices.isEmpty)
            Container(
              padding: const EdgeInsets.all(16),
              decoration: BoxDecoration(
                color: colors.warning.withValues(alpha: 0.1),
                borderRadius: BorderRadius.circular(8),
                border: Border.all(color: colors.warning.withValues(alpha: 0.3)),
              ),
              child: Row(
                children: [
                  Icon(LucideIcons.alertTriangle,
                      size: 16, color: colors.warning),
                  const SizedBox(width: 12),
                  Expanded(
                    child: Text(
                      'No devices assigned to this profile. Edit the profile to add devices.',
                      style: TextStyle(
                        fontSize: 12,
                        color: colors.textSecondary,
                      ),
                    ),
                  ),
                ],
              ),
            ),

          const SizedBox(height: 20),

          // Action buttons
          Row(
            children: [
              Expanded(
                child: FilledButton.icon(
                  onPressed: devices.isNotEmpty ? onConnect : null,
                  icon: const Icon(LucideIcons.plug, size: 16),
                  label: const Text('Connect All'),
                  style: FilledButton.styleFrom(
                    backgroundColor: colors.primary,
                    foregroundColor: Colors.white,
                    padding: const EdgeInsets.symmetric(vertical: 14),
                  ),
                ),
              ),
              const SizedBox(width: 12),
              OutlinedButton(
                onPressed: onEdit,
                child: const Text('Edit Profile'),
                style: OutlinedButton.styleFrom(
                  foregroundColor: colors.textSecondary,
                  side: BorderSide(color: colors.border),
                  padding:
                      const EdgeInsets.symmetric(vertical: 14, horizontal: 20),
                ),
              ),
            ],
          ),
        ],
      ),
    );
  }
}

class _ConnectingView extends StatelessWidget {
  final db.EquipmentProfile profile;
  final List<_DeviceStatus> devices;
  final NightshadeColors colors;
  final VoidCallback onCancel;

  const _ConnectingView({
    required this.profile,
    required this.devices,
    required this.colors,
    required this.onCancel,
  });

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.all(20),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Row(
            children: [
              SizedBox(
                width: 16,
                height: 16,
                child: CircularProgressIndicator(
                  strokeWidth: 2,
                  color: colors.primary,
                ),
              ),
              const SizedBox(width: 12),
              Text(
                'Connecting ${profile.name}...',
                style: TextStyle(
                  fontSize: 14,
                  fontWeight: FontWeight.w600,
                  color: colors.textPrimary,
                ),
              ),
            ],
          ),
          const SizedBox(height: 16),

          // Device progress list
          ...devices.map((device) {
            final (icon, color) = _getStateIcon(device.state, colors);

            return Padding(
              padding: const EdgeInsets.only(bottom: 10),
              child: Row(
                children: [
                  SizedBox(
                    width: 16,
                    height: 16,
                    child: device.state == DeviceConnectionState.connecting
                        ? CircularProgressIndicator(
                            strokeWidth: 2,
                            color: color,
                          )
                        : Icon(icon, size: 16, color: color),
                  ),
                  const SizedBox(width: 12),
                  Text(
                    device.type,
                    style: TextStyle(
                      fontSize: 13,
                      color: colors.textSecondary,
                    ),
                  ),
                  const SizedBox(width: 8),
                  Expanded(
                    child: Text(
                      device.name,
                      style: TextStyle(
                        fontSize: 13,
                        color: colors.textPrimary,
                      ),
                      overflow: TextOverflow.ellipsis,
                    ),
                  ),
                  const SizedBox(width: 8),
                  Text(
                    _getStateLabel(device.state),
                    style: TextStyle(
                      fontSize: 11,
                      color: color,
                      fontWeight: FontWeight.w500,
                    ),
                  ),
                ],
              ),
            );
          }),

          const SizedBox(height: 16),

          // Cancel button
          Align(
            alignment: Alignment.centerLeft,
            child: TextButton(
              onPressed: onCancel,
              child: Text(
                'Cancel',
                style: TextStyle(color: colors.textSecondary),
              ),
            ),
          ),
        ],
      ),
    );
  }

  (IconData, Color) _getStateIcon(DeviceConnectionState state, NightshadeColors colors) {
    switch (state) {
      case DeviceConnectionState.connected:
        return (LucideIcons.checkCircle, colors.success);
      case DeviceConnectionState.connecting:
        return (LucideIcons.loader, colors.warning);
      case DeviceConnectionState.error:
        return (LucideIcons.xCircle, colors.error);
      case DeviceConnectionState.disconnected:
        return (LucideIcons.circle, colors.textMuted);
    }
  }

  String _getStateLabel(DeviceConnectionState state) {
    switch (state) {
      case DeviceConnectionState.connected:
        return 'Connected';
      case DeviceConnectionState.connecting:
        return 'Connecting';
      case DeviceConnectionState.error:
        return 'Failed';
      case DeviceConnectionState.disconnected:
        return 'Waiting';
    }
  }
}

class _ErrorView extends StatelessWidget {
  final List<_DeviceStatus> devices;
  final _DeviceStatus? errorDevice;
  final int connectedCount;
  final int totalCount;
  final NightshadeColors colors;
  final VoidCallback onRetry;
  final VoidCallback onSkip;

  const _ErrorView({
    required this.devices,
    required this.errorDevice,
    required this.connectedCount,
    required this.totalCount,
    required this.colors,
    required this.onRetry,
    required this.onSkip,
  });

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.all(20),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          // Error header
          Row(
            children: [
              Icon(LucideIcons.alertTriangle, size: 18, color: colors.warning),
              const SizedBox(width: 12),
              Text(
                '$connectedCount/$totalCount Connected',
                style: TextStyle(
                  fontSize: 14,
                  fontWeight: FontWeight.w600,
                  color: colors.warning,
                ),
              ),
              const Spacer(),
              FilledButton(
                onPressed: onRetry,
                style: FilledButton.styleFrom(
                  backgroundColor: colors.primary,
                  foregroundColor: Colors.white,
                  padding:
                      const EdgeInsets.symmetric(horizontal: 16, vertical: 8),
                ),
                child: const Text('Retry'),
              ),
            ],
          ),

          if (errorDevice != null) ...[
            const SizedBox(height: 16),
            Container(
              padding: const EdgeInsets.all(12),
              decoration: BoxDecoration(
                color: colors.error.withValues(alpha: 0.1),
                borderRadius: BorderRadius.circular(8),
                border: Border.all(color: colors.error.withValues(alpha: 0.3)),
              ),
              child: Row(
                children: [
                  Icon(LucideIcons.xCircle, size: 16, color: colors.error),
                  const SizedBox(width: 12),
                  Expanded(
                    child: Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: [
                        Text(
                          '${errorDevice!.type} failed',
                          style: TextStyle(
                            fontSize: 13,
                            fontWeight: FontWeight.w500,
                            color: colors.textPrimary,
                          ),
                        ),
                        const SizedBox(height: 2),
                        Text(
                          errorDevice!.error ?? 'Device not responding',
                          style: TextStyle(
                            fontSize: 12,
                            color: colors.textSecondary,
                          ),
                        ),
                      ],
                    ),
                  ),
                  const SizedBox(width: 8),
                  Row(
                    mainAxisSize: MainAxisSize.min,
                    children: [
                      TextButton(
                        onPressed: onRetry,
                        child: Text(
                          'Retry',
                          style: TextStyle(color: colors.primary, fontSize: 12),
                        ),
                      ),
                      TextButton(
                        onPressed: onSkip,
                        child: Text(
                          'Skip',
                          style: TextStyle(
                              color: colors.textSecondary, fontSize: 12),
                        ),
                      ),
                    ],
                  ),
                ],
              ),
            ),
          ],
        ],
      ),
    );
  }
}
