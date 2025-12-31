import 'dart:io' show Platform;

import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:lucide_icons/lucide_icons.dart';
import 'package:nightshade_core/nightshade_core.dart';
import 'package:nightshade_core/src/database/database.dart' as db;
import 'package:nightshade_ui/nightshade_ui.dart';
import '../dialogs/indi_server_dialog.dart';
import '../widgets/backend_selector_chips.dart';

/// Device type enum for the save to profile dialog
enum DeviceCategory { camera, mount, focuser, filterWheel, guider, rotator }

/// Shows a dialog asking to save the connected device to the active profile.
/// Returns true if the device was saved, false otherwise.
Future<bool> showSaveToProfileDialog({
  required BuildContext context,
  required WidgetRef ref,
  required String deviceId,
  required String deviceName,
  required DeviceCategory deviceType,
}) async {
  final colors = Theme.of(context).extension<NightshadeColors>()!;

  // Check if there's an active profile
  final activeProfile = await ref.read(activeProfileProvider.future);
  if (activeProfile == null) {
    // No active profile, silently return
    return false;
  }

  // Check if device is already assigned to this profile
  bool alreadyAssigned = false;
  switch (deviceType) {
    case DeviceCategory.camera:
      alreadyAssigned = activeProfile.cameraId == deviceId;
      break;
    case DeviceCategory.mount:
      alreadyAssigned = activeProfile.mountId == deviceId;
      break;
    case DeviceCategory.focuser:
      alreadyAssigned = activeProfile.focuserId == deviceId;
      break;
    case DeviceCategory.filterWheel:
      alreadyAssigned = activeProfile.filterWheelId == deviceId;
      break;
    case DeviceCategory.guider:
      alreadyAssigned = activeProfile.guiderId == deviceId;
      break;
    case DeviceCategory.rotator:
      alreadyAssigned = activeProfile.rotatorId == deviceId;
      break;
  }

  // If already assigned, don't show dialog
  if (alreadyAssigned) {
    return false;
  }

  // Show the dialog
  final result = await showDialog<bool>(
    context: context,
    builder: (context) => AlertDialog(
      backgroundColor: colors.surface,
      shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)),
      title: Row(
        children: [
          Icon(LucideIcons.save, color: colors.primary, size: 20),
          const SizedBox(width: 12),
          Text(
            'Save to Profile',
            style: TextStyle(
              color: colors.textPrimary,
              fontSize: 18,
              fontWeight: FontWeight.w600,
            ),
          ),
        ],
      ),
      content: Column(
        mainAxisSize: MainAxisSize.min,
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Text(
            'Save "$deviceName" to the active profile "${activeProfile.name}"?',
            style: TextStyle(
              color: colors.textSecondary,
              fontSize: 14,
            ),
          ),
          const SizedBox(height: 12),
          Container(
            padding: const EdgeInsets.all(12),
            decoration: BoxDecoration(
              color: colors.primary.withValues(alpha: 0.1),
              borderRadius: BorderRadius.circular(8),
              border: Border.all(color: colors.primary.withValues(alpha: 0.3)),
            ),
            child: Row(
              children: [
                Icon(LucideIcons.info, color: colors.primary, size: 16),
                const SizedBox(width: 10),
                Expanded(
                  child: Text(
                    'This device will be auto-connected when you activate this profile.',
                    style: TextStyle(
                      fontSize: 12,
                      color: colors.textSecondary,
                    ),
                  ),
                ),
              ],
            ),
          ),
        ],
      ),
      actions: [
        TextButton(
          onPressed: () => Navigator.pop(context, false),
          child: Text(
            'Not Now',
            style: TextStyle(color: colors.textMuted),
          ),
        ),
        FilledButton(
          onPressed: () => Navigator.pop(context, true),
          style: FilledButton.styleFrom(backgroundColor: colors.primary),
          child: const Text('Save to Profile'),
        ),
      ],
    ),
  );

  if (result == true) {
    try {
      final profileService = ref.read(profileServiceProvider);

      switch (deviceType) {
        case DeviceCategory.camera:
          await profileService.updateProfileDevices(activeProfile.id, cameraId: deviceId);
          break;
        case DeviceCategory.mount:
          await profileService.updateProfileDevices(activeProfile.id, mountId: deviceId);
          break;
        case DeviceCategory.focuser:
          await profileService.updateProfileDevices(activeProfile.id, focuserId: deviceId);
          break;
        case DeviceCategory.filterWheel:
          await profileService.updateProfileDevices(activeProfile.id, filterWheelId: deviceId);
          break;
        case DeviceCategory.guider:
          await profileService.updateProfileDevices(activeProfile.id, guiderId: deviceId);
          break;
        case DeviceCategory.rotator:
          await profileService.updateProfileDevices(activeProfile.id, rotatorId: deviceId);
          break;
      }

      // Invalidate the profile provider to refresh the UI
      ref.invalidate(activeProfileProvider);

      return true;
    } catch (e) {
      if (context.mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Failed to save to profile: $e'),
            backgroundColor: colors.error,
          ),
        );
      }
      return false;
    }
  }

  return false;
}

class ConnectionsTab extends ConsumerStatefulWidget {
  const ConnectionsTab({super.key});

  @override
  ConsumerState<ConnectionsTab> createState() => _ConnectionsTabState();
}

class _ConnectionsTabState extends ConsumerState<ConnectionsTab> {
  bool _isScanning = false;

  Future<void> _scanForDevices() async {
    setState(() => _isScanning = true);

    try {
      // Use unified discovery - discovers from ALL backends in parallel
      // The unifiedDiscoveryProvider already caches results in rawDevices/groupedDevices
      await ref.read(unifiedDiscoveryProvider.notifier).discoverAll();
    } finally {
      if (mounted) {
        setState(() => _isScanning = false);
      }
    }
  }

  void _showDebugInfo() {
    // Use unified discovery state (doesn't trigger new discovery)
    final discoveryState = ref.read(unifiedDiscoveryProvider);
    final rawDevices = discoveryState.rawDevices;
    final groupedDevices = discoveryState.groupedDevices;

    // Count by type from raw devices
    final cameras = rawDevices.where((d) => d.type == NightshadeDeviceType.camera).toList();
    final mounts = rawDevices.where((d) => d.type == NightshadeDeviceType.mount).toList();
    final focusers = rawDevices.where((d) => d.type == NightshadeDeviceType.focuser).toList();
    final wheels = rawDevices.where((d) => d.type == NightshadeDeviceType.filterWheel).toList();
    final guiders = rawDevices.where((d) => d.type == NightshadeDeviceType.guider).toList();
    final rotators = rawDevices.where((d) => d.type == NightshadeDeviceType.rotator).toList();

    showDialog(
      context: context,
      builder: (context) => AlertDialog(
        title: const Text('Debug: Discovered Devices'),
        content: SingleChildScrollView(
          child: Column(
            mainAxisSize: MainAxisSize.min,
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Text('=== Raw Devices (${rawDevices.length} total) ===',
                  style: const TextStyle(fontWeight: FontWeight.bold)),
              const SizedBox(height: 8),
              Text('Cameras: ${cameras.length}'),
              ...cameras.map((d) => Text('• ${d.name} (${d.backend.name})')),
              const SizedBox(height: 8),
              Text('Mounts: ${mounts.length}'),
              ...mounts.map((d) => Text('• ${d.name} (${d.backend.name})')),
              const SizedBox(height: 8),
              Text('Focusers: ${focusers.length}'),
              ...focusers.map((d) => Text('• ${d.name} (${d.backend.name})')),
              const SizedBox(height: 8),
              Text('Filter Wheels: ${wheels.length}'),
              ...wheels.map((d) => Text('• ${d.name} (${d.backend.name})')),
              const SizedBox(height: 8),
              Text('Guiders: ${guiders.length}'),
              ...guiders.map((d) => Text('• ${d.name} (${d.backend.name})')),
              const SizedBox(height: 8),
              Text('Rotators: ${rotators.length}'),
              ...rotators.map((d) => Text('• ${d.name} (${d.backend.name})')),
              const SizedBox(height: 16),
              Text('=== Unified Devices (${groupedDevices.length} physical) ===',
                  style: const TextStyle(fontWeight: FontWeight.bold)),
              const SizedBox(height: 8),
              ...groupedDevices.map((u) {
                final backends = u.availableBackends.keys.map((b) => b.shortLabel).join(', ');
                return Text('• ${u.displayName} [$backends]');
              }),
            ],
          ),
        ),
        actions: [
          TextButton(
            onPressed: () => Navigator.pop(context),
            child: const Text('Close'),
          ),
        ],
      ),
    );
  }

  void _showAddAlpacaServerDialog() {
    final colors = Theme.of(context).extension<NightshadeColors>()!;
    final hostController = TextEditingController(text: 'localhost');
    final portController = TextEditingController(text: '11111');

    showDialog(
      context: context,
      builder: (context) => AlertDialog(
        backgroundColor: colors.surface,
        title: Text(
          'Add Alpaca Server',
          style: TextStyle(color: colors.textPrimary),
        ),
        content: Column(
          mainAxisSize: MainAxisSize.min,
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            SizedBox(
              width: 360,
              child: Column(
                mainAxisSize: MainAxisSize.min,
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Text(
                    'Enter the address of your Alpaca server.\n'
                    'If you have ASCOM drivers, install "ASCOM Remote" to expose them via Alpaca.',
                    style: TextStyle(color: colors.textSecondary, fontSize: 12),
                  ),
                  const SizedBox(height: 16),
                  TextField(
                    controller: hostController,
                    style: TextStyle(color: colors.textPrimary),
                    decoration: InputDecoration(
                      labelText: 'Host',
                      labelStyle: TextStyle(color: colors.textMuted),
                      hintText: 'localhost or IP address',
                      hintStyle: TextStyle(color: colors.textMuted.withValues(alpha: 0.5)),
                      enabledBorder: OutlineInputBorder(
                        borderSide: BorderSide(color: colors.border),
                      ),
                      focusedBorder: OutlineInputBorder(
                        borderSide: BorderSide(color: colors.primary),
                      ),
                    ),
                  ),
                  const SizedBox(height: 12),
                  TextField(
                    controller: portController,
                    style: TextStyle(color: colors.textPrimary),
                    keyboardType: TextInputType.number,
                    decoration: InputDecoration(
                      labelText: 'Port',
                      labelStyle: TextStyle(color: colors.textMuted),
                      hintText: '11111',
                      hintStyle: TextStyle(color: colors.textMuted.withValues(alpha: 0.5)),
                      enabledBorder: OutlineInputBorder(
                        borderSide: BorderSide(color: colors.border),
                      ),
                      focusedBorder: OutlineInputBorder(
                        borderSide: BorderSide(color: colors.primary),
                      ),
                    ),
                  ),
                ],
              ),
            ),
          ],
        ),
        actions: [
          TextButton(
            onPressed: () => Navigator.pop(context),
            child: Text('Cancel', style: TextStyle(color: colors.textMuted)),
          ),
          FilledButton(
            onPressed: () async {
              Navigator.pop(context);
              final host = hostController.text.trim();
              final port = int.tryParse(portController.text) ?? 11111;
              await _connectToAlpacaServer(host, port);
            },
            style: FilledButton.styleFrom(backgroundColor: colors.primary),
            child: const Text('Connect'),
          ),
        ],
      ),
    );
  }

  Future<void> _connectToAlpacaServer(String host, int port) async {
    final colors = Theme.of(context).extension<NightshadeColors>()!;

    // Show loading indicator
    ScaffoldMessenger.of(context).showSnackBar(
      SnackBar(
        content: Text('Connecting to Alpaca server at $host:$port...'),
        backgroundColor: colors.surface,
        duration: const Duration(seconds: 2),
      ),
    );

    try {
      // Import and use the Alpaca client directly
      final devices = await _discoverAlpacaAtAddress(host, port);

      if (devices.isEmpty) {
        if (mounted) {
          ScaffoldMessenger.of(context).showSnackBar(
            SnackBar(
              content: Text('No devices found at $host:$port. Make sure ASCOM Remote is running.'),
              backgroundColor: colors.error,
            ),
          );
        }
      } else {
        // Re-discover all devices to include the new Alpaca devices
        // This uses the unified discovery which discovers from ALL backends
        await ref.read(unifiedDiscoveryProvider.notifier).discoverAll();

        if (mounted) {
          ScaffoldMessenger.of(context).showSnackBar(
            SnackBar(
              content: Text('Found ${devices.length} device(s) at $host:$port'),
              backgroundColor: colors.success,
            ),
          );
        }
      }
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Failed to connect: $e'),
            backgroundColor: colors.error,
          ),
        );
      }
    }
  }

  Future<List<AvailableDevice>> _discoverAlpacaAtAddress(String host, int port) async {
    final deviceService = ref.read(deviceServiceProvider);
    return deviceService.discoverAlpacaAtAddress(host, port);
  }

  Future<void> _connectToIndiServer(String host, int port) async {
    final colors = Theme.of(context).extension<NightshadeColors>()!;
    final deviceService = ref.read(deviceServiceProvider);

    // Show loading indicator
    ScaffoldMessenger.of(context).showSnackBar(
      SnackBar(
        content: Text('Connecting to INDI server at $host:$port...'),
        backgroundColor: colors.surface,
        duration: const Duration(seconds: 2),
      ),
    );

    try {
      // Use the device service to discover INDI devices
      final devices = await deviceService.discoverIndiAtAddress(host, port);

      if (devices.isEmpty) {
        if (mounted) {
          ScaffoldMessenger.of(context).showSnackBar(
            SnackBar(
              content: Text('No devices found at $host:$port.'),
              backgroundColor: colors.warning,
            ),
          );
        }
      } else {
        // Re-discover all devices to include the new INDI devices
        // This uses the unified discovery which discovers from ALL backends
        await ref.read(unifiedDiscoveryProvider.notifier).discoverAll();

        if (mounted) {
          ScaffoldMessenger.of(context).showSnackBar(
            SnackBar(
              content: Text('Found ${devices.length} device(s) at $host:$port'),
              backgroundColor: colors.success,
            ),
          );
        }
      }
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Failed to connect: $e'),
            backgroundColor: colors.error,
          ),
        );
      }
    }
  }

  void _showIndiServerDialog() async {
    final result = await showDialog<Map<String, dynamic>>(
      context: context,
      builder: (context) => const IndiServerDialog(),
    );
    
    if (result != null) {
      final host = result['host'] as String;
      final port = result['port'] as int;
      
      // Connect to the INDI server
      await _connectToIndiServer(host, port);
    }
  }

  @override
  Widget build(BuildContext context) {
    final colors = Theme.of(context).extension<NightshadeColors>()!;

    // Watch device states
    final cameraState = ref.watch(cameraStateProvider);
    final mountState = ref.watch(mountStateProvider);
    final focuserState = ref.watch(focuserStateProvider);
    final filterWheelState = ref.watch(filterWheelStateProvider);
    final guiderState = ref.watch(guiderStateProvider);
    final rotatorState = ref.watch(rotatorStateProvider);

    // Device cards now use unified providers internally (unifiedCamerasProvider, etc.)
    // Each card fetches its own grouped devices and shows inline backend selector chips

    return SingleChildScrollView(
      padding: const EdgeInsets.all(24),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          // Device Discovery Section
          _DeviceDiscoveryCard(
            isScanning: _isScanning,
            onScan: _scanForDevices,
            onAddAlpacaServer: _showAddAlpacaServerDialog,
            onAddIndiServer: _showIndiServerDialog,
            onDebug: _showDebugInfo,
            colors: colors,
          ),
          const SizedBox(height: 24),

          // Essential Equipment Section
          _SectionHeader(
            title: 'Essential Equipment',
            subtitle: 'Required for imaging',
            colors: colors,
          ),
          const SizedBox(height: 16),

          // Camera and Mount - responsive grid
          ResponsiveCardGrid(
            minCardWidth: 350,
            children: [
              _CameraDeviceCard(
                cameraState: cameraState,
                colors: colors,
              ),
              _MountDeviceCard(
                mountState: mountState,
                colors: colors,
              ),
            ],
          ),

          const SizedBox(height: 32),

          // Optional Equipment Section
          _SectionHeader(
            title: 'Optional Equipment',
            subtitle: 'Enhance your imaging workflow',
            colors: colors,
          ),
          const SizedBox(height: 16),

          // Optional equipment - responsive grid
          ResponsiveCardGrid(
            minCardWidth: 300,
            children: [
              _GuiderDeviceCard(
                guiderState: guiderState,
                colors: colors,
              ),
              _FocuserDeviceCard(
                focuserState: focuserState,
                colors: colors,
              ),
              _FilterWheelDeviceCard(
                filterWheelState: filterWheelState,
                colors: colors,
              ),
              _RotatorDeviceCard(
                rotatorState: rotatorState,
                colors: colors,
              ),
            ],
          ),

          const SizedBox(height: 32),

          // Telescope Configuration Section
          _SectionHeader(
            title: 'Telescope Configuration',
            subtitle: 'Optical system details',
            colors: colors,
          ),
          const SizedBox(height: 16),

          _TelescopeCard(colors: colors),
        ],
      ),
    );
  }
}

// ============================================================================
// Device Discovery Card
// ============================================================================

class _DeviceDiscoveryCard extends ConsumerWidget {
  final bool isScanning;
  final VoidCallback onScan;
  final VoidCallback onAddAlpacaServer;
  final VoidCallback onAddIndiServer;
  final VoidCallback onDebug;
  final NightshadeColors colors;

  const _DeviceDiscoveryCard({
    required this.isScanning,
    required this.onScan,
    required this.onAddAlpacaServer,
    required this.onAddIndiServer,
    required this.onDebug,
    required this.colors,
  });

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    // Watch discovery state for progress info
    final discoveryState = ref.watch(unifiedDiscoveryProvider);
    final isDiscovering = discoveryState.isDiscovering || isScanning;

    // Build backend status indicators
    final backendStatusWidgets = <Widget>[];
    for (final backend in [DriverBackend.native, DriverBackend.ascom, DriverBackend.alpaca, DriverBackend.indi]) {
      if (backend == DriverBackend.ascom && !Platform.isWindows) {
        continue; // Skip ASCOM on non-Windows
      }
      final state = discoveryState.backendStates[backend];
      final status = state?.status ?? DiscoveryStatus.idle;
      final deviceCount = state?.devices.length ?? 0;

      Color statusColor;
      IconData statusIcon;
      switch (status) {
        case DiscoveryStatus.idle:
          statusColor = colors.textMuted;
          statusIcon = LucideIcons.circle;
          break;
        case DiscoveryStatus.discovering:
          statusColor = colors.info;
          statusIcon = LucideIcons.loader2;
          break;
        case DiscoveryStatus.completed:
          statusColor = deviceCount > 0 ? colors.success : colors.textMuted;
          statusIcon = deviceCount > 0 ? LucideIcons.checkCircle : LucideIcons.circle;
          break;
        case DiscoveryStatus.error:
          statusColor = colors.error;
          statusIcon = LucideIcons.alertCircle;
          break;
      }

      backendStatusWidgets.add(
        Tooltip(
          message: state?.error ?? '${backend.displayName}: $deviceCount device(s)',
          child: Container(
            padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 4),
            decoration: BoxDecoration(
              color: statusColor.withValues(alpha: 0.1),
              borderRadius: BorderRadius.circular(12),
              border: Border.all(color: statusColor.withValues(alpha: 0.3)),
            ),
            child: Row(
              mainAxisSize: MainAxisSize.min,
              children: [
                Icon(statusIcon, size: 12, color: statusColor),
                const SizedBox(width: 4),
                Text(
                  backend.shortLabel,
                  style: TextStyle(fontSize: 10, color: statusColor, fontWeight: FontWeight.w500),
                ),
                if (status == DiscoveryStatus.completed && deviceCount > 0) ...[
                  const SizedBox(width: 4),
                  Text(
                    '($deviceCount)',
                    style: TextStyle(fontSize: 10, color: statusColor),
                  ),
                ],
              ],
            ),
          ),
        ),
      );
    }

    return Container(
      padding: const EdgeInsets.all(20),
      decoration: BoxDecoration(
        color: colors.surface,
        borderRadius: BorderRadius.circular(12),
        border: Border.all(color: colors.border),
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Row(
            children: [
              Icon(LucideIcons.radar, color: colors.primary, size: 20),
              const SizedBox(width: 12),
              Expanded(
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Text(
                      'Device Discovery',
                      style: TextStyle(
                        fontSize: 16,
                        fontWeight: FontWeight.w600,
                        color: colors.textPrimary,
                      ),
                    ),
                    const SizedBox(height: 2),
                    Text(
                      'Scan for devices across all driver types',
                      style: TextStyle(
                        fontSize: 12,
                        color: colors.textMuted,
                      ),
                    ),
                  ],
                ),
              ),
            ],
          ),

          const SizedBox(height: 16),

          // Backend status indicators
          Wrap(
            spacing: 8,
            runSpacing: 8,
            children: backendStatusWidgets,
          ),

          const SizedBox(height: 16),

          // Info box
          Container(
            padding: const EdgeInsets.all(12),
            decoration: BoxDecoration(
              color: colors.primary.withValues(alpha: 0.1),
              borderRadius: BorderRadius.circular(8),
              border: Border.all(color: colors.primary.withValues(alpha: 0.3)),
            ),
            child: Row(
              children: [
                Icon(LucideIcons.info, color: colors.primary, size: 16),
                const SizedBox(width: 10),
                Expanded(
                  child: Text(
                    'Discovers devices from Native SDKs, ASCOM, Alpaca, and INDI simultaneously. '
                    'Each device shows which drivers are available.',
                    style: TextStyle(
                      fontSize: 11,
                      color: colors.textSecondary,
                    ),
                  ),
                ),
              ],
            ),
          ),

          const SizedBox(height: 16),

          // Action buttons
          Row(
            children: [
              Expanded(
                child: FilledButton.icon(
                  onPressed: isDiscovering ? null : onScan,
                  icon: isDiscovering
                      ? SizedBox(
                          width: 16,
                          height: 16,
                          child: CircularProgressIndicator(
                            strokeWidth: 2,
                            color: colors.textPrimary,
                          ),
                        )
                      : const Icon(LucideIcons.search, size: 16),
                  label: Text(isDiscovering ? 'Discovering...' : 'Discover Devices'),
                  style: FilledButton.styleFrom(
                    backgroundColor: colors.primary,
                    foregroundColor: Colors.white,
                    padding: const EdgeInsets.symmetric(vertical: 12),
                  ),
                ),
              ),
              const SizedBox(width: 12),
              // Add Server dropdown for INDI/Alpaca
              PopupMenuButton<String>(
                onSelected: (value) {
                  if (value == 'indi') {
                    onAddIndiServer();
                  } else if (value == 'alpaca') {
                    onAddAlpacaServer();
                  }
                },
                itemBuilder: (context) => [
                  PopupMenuItem(
                    value: 'indi',
                    child: Row(
                      children: [
                        Icon(LucideIcons.server, size: 16, color: colors.textSecondary),
                        const SizedBox(width: 8),
                        const Text('Add INDI Server'),
                      ],
                    ),
                  ),
                  PopupMenuItem(
                    value: 'alpaca',
                    child: Row(
                      children: [
                        Icon(LucideIcons.globe, size: 16, color: colors.textSecondary),
                        const SizedBox(width: 8),
                        const Text('Add Alpaca Server'),
                      ],
                    ),
                  ),
                ],
                child: OutlinedButton.icon(
                  onPressed: null, // Handled by popup
                  icon: const Icon(LucideIcons.plus, size: 16),
                  label: const Text('Add Server'),
                  style: OutlinedButton.styleFrom(
                    foregroundColor: colors.textPrimary,
                    side: BorderSide(color: colors.border),
                    padding: const EdgeInsets.symmetric(vertical: 12, horizontal: 16),
                  ),
                ),
              ),
              const SizedBox(width: 12),
              IconButton(
                onPressed: onDebug,
                icon: const Icon(LucideIcons.bug),
                tooltip: 'Debug Info',
                style: IconButton.styleFrom(
                  foregroundColor: colors.textMuted,
                ),
              ),
            ],
          ),
        ],
      ),
    );
  }
}

// ============================================================================
// Section Header
// ============================================================================

class _SectionHeader extends StatelessWidget {
  final String title;
  final String subtitle;
  final NightshadeColors colors;

  const _SectionHeader({
    required this.title,
    required this.subtitle,
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
            fontSize: 16,
            fontWeight: FontWeight.w600,
            color: colors.textPrimary,
          ),
        ),
        const SizedBox(height: 2),
        Text(
          subtitle,
          style: TextStyle(
            fontSize: 12,
            color: colors.textMuted,
          ),
        ),
      ],
    );
  }
}

// ============================================================================
// Camera Device Card
// ============================================================================

class _CameraDeviceCard extends ConsumerStatefulWidget {
  final CameraState cameraState;
  final NightshadeColors colors;

  const _CameraDeviceCard({
    required this.cameraState,
    required this.colors,
  });

  @override
  ConsumerState<_CameraDeviceCard> createState() => _CameraDeviceCardState();
}

class _CameraDeviceCardState extends ConsumerState<_CameraDeviceCard> {
  UnifiedDevice? _selectedDevice;
  bool _isHovered = false;
  bool _isConnecting = false;

  bool get _isConnected =>
      widget.cameraState.connectionState == DeviceConnectionState.connected;

  @override
  Widget build(BuildContext context) {
    // Use unified cameras (grouped by physical device)
    final unifiedCameras = ref.watch(unifiedCamerasProvider);

    final statusDetails = <String>[];
    if (_isConnected) {
      if (widget.cameraState.temperature != null) {
        statusDetails.add('Sensor: ${widget.cameraState.temperature!.toStringAsFixed(1)}°C');
      }
      if (widget.cameraState.coolerPower != null) {
        statusDetails.add('Cooler: ${widget.cameraState.coolerPower!.toStringAsFixed(0)}%');
      }
    } else {
      statusDetails.addAll(['Sensor: ---', 'Cooling: ---']);
    }

    return _UnifiedBaseDeviceCard(
      icon: LucideIcons.camera,
      title: 'Camera',
      subtitle: widget.cameraState.deviceName ?? 'Main imaging camera',
      isConnected: _isConnected,
      isConnecting: _isConnecting || widget.cameraState.connectionState == DeviceConnectionState.connecting,
      statusLabel: _getStatusLabel(),
      statusDetails: statusDetails,
      accentColor: widget.colors.primary,
      colors: widget.colors,
      isHovered: _isHovered,
      onHoverChanged: (hovered) => setState(() => _isHovered = hovered),
      unifiedDevices: unifiedCameras,
      selectedDevice: _selectedDevice,
      onDeviceSelected: (device) => setState(() => _selectedDevice = device),
      onBackendSelected: _handleBackendSelected,
      onConnect: _handleConnect,
      onDisconnect: _handleDisconnect,
    );
  }

  void _handleBackendSelected(DriverBackend backend) {
    if (_selectedDevice == null) return;
    // Update the selected device with the new backend
    setState(() {
      _selectedDevice = _selectedDevice!.withSelectedBackend(backend);
    });
  }

  String _getStatusLabel() {
    if (widget.cameraState.isExposing) return 'Exposing';
    if (widget.cameraState.isCooling) return 'Cooling';
    if (_isConnected) return 'Ready';
    return 'Idle';
  }

  Future<void> _handleConnect() async {
    if (_selectedDevice == null) return;

    setState(() => _isConnecting = true);
    try {
      final deviceId = _selectedDevice!.activeDeviceId;
      await ref.read(deviceServiceProvider).connectCamera(deviceId);

      // Show save to profile dialog after successful connection
      if (mounted) {
        await showSaveToProfileDialog(
          context: context,
          ref: ref,
          deviceId: deviceId,
          deviceName: _selectedDevice!.displayName,
          deviceType: DeviceCategory.camera,
        );
      }
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Failed to connect: $e')),
        );
      }
    } finally {
      if (mounted) setState(() => _isConnecting = false);
    }
  }

  Future<void> _handleDisconnect() async {
    await ref.read(deviceServiceProvider).disconnectCamera();
  }
}

// ============================================================================
// Mount Device Card
// ============================================================================

class _MountDeviceCard extends ConsumerStatefulWidget {
  final MountState mountState;
  final NightshadeColors colors;

  const _MountDeviceCard({
    required this.mountState,
    required this.colors,
  });

  @override
  ConsumerState<_MountDeviceCard> createState() => _MountDeviceCardState();
}

class _MountDeviceCardState extends ConsumerState<_MountDeviceCard> {
  UnifiedDevice? _selectedDevice;
  bool _isHovered = false;
  bool _isConnecting = false;

  bool get _isConnected =>
      widget.mountState.connectionState == DeviceConnectionState.connected;

  @override
  Widget build(BuildContext context) {
    // Use unified mounts (grouped by physical device)
    final unifiedMounts = ref.watch(unifiedMountsProvider);

    final statusDetails = <String>[];
    if (_isConnected) {
      final ra = widget.mountState.ra?.toStringAsFixed(2) ?? '---';
      final dec = widget.mountState.dec?.toStringAsFixed(2) ?? '---';
      statusDetails.add('RA: $ra  Dec: $dec');
      if (widget.mountState.isTracking) {
        statusDetails.add('Tracking');
      }
    } else {
      statusDetails.add('RA: ---  Dec: ---');
    }

    return _UnifiedBaseDeviceCard(
      icon: LucideIcons.compass,
      title: 'Mount',
      subtitle: widget.mountState.deviceName ?? 'Telescope mount',
      isConnected: _isConnected,
      isConnecting: _isConnecting || widget.mountState.connectionState == DeviceConnectionState.connecting,
      statusLabel: widget.mountState.isSlewing ? 'Slewing' : (_isConnected ? 'Ready' : 'Idle'),
      statusDetails: statusDetails,
      accentColor: widget.colors.warning,
      colors: widget.colors,
      isHovered: _isHovered,
      onHoverChanged: (hovered) => setState(() => _isHovered = hovered),
      unifiedDevices: unifiedMounts,
      selectedDevice: _selectedDevice,
      onDeviceSelected: (device) => setState(() => _selectedDevice = device),
      onBackendSelected: _handleBackendSelected,
      onConnect: _handleConnect,
      onDisconnect: _handleDisconnect,
    );
  }

  void _handleBackendSelected(DriverBackend backend) {
    if (_selectedDevice == null) return;
    // Update the selected device with the new backend
    setState(() {
      _selectedDevice = _selectedDevice!.withSelectedBackend(backend);
    });
  }

  Future<void> _handleConnect() async {
    if (_selectedDevice == null) return;

    setState(() => _isConnecting = true);
    try {
      final deviceId = _selectedDevice!.activeDeviceId;
      await ref.read(deviceServiceProvider).connectMount(deviceId);

      // Show save to profile dialog after successful connection
      if (mounted) {
        await showSaveToProfileDialog(
          context: context,
          ref: ref,
          deviceId: deviceId,
          deviceName: _selectedDevice!.displayName,
          deviceType: DeviceCategory.mount,
        );
      }
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Failed to connect: $e')),
        );
      }
    } finally {
      if (mounted) setState(() => _isConnecting = false);
    }
  }

  Future<void> _handleDisconnect() async {
    await ref.read(deviceServiceProvider).disconnectMount();
  }
}

// ============================================================================
// Focuser Device Card
// ============================================================================

class _FocuserDeviceCard extends ConsumerStatefulWidget {
  final FocuserState focuserState;
  final NightshadeColors colors;

  const _FocuserDeviceCard({
    required this.focuserState,
    required this.colors,
  });

  @override
  ConsumerState<_FocuserDeviceCard> createState() => _FocuserDeviceCardState();
}

class _FocuserDeviceCardState extends ConsumerState<_FocuserDeviceCard> {
  UnifiedDevice? _selectedDevice;
  bool _isHovered = false;
  bool _isConnecting = false;

  bool get _isConnected =>
      widget.focuserState.connectionState == DeviceConnectionState.connected;

  @override
  Widget build(BuildContext context) {
    // Use unified focusers (grouped by physical device)
    final unifiedFocusers = ref.watch(unifiedFocusersProvider);

    final statusDetails = <String>[];
    if (_isConnected && widget.focuserState.position != null) {
      statusDetails.add('Position: ${widget.focuserState.position}');
    } else {
      statusDetails.add('Position: ---');
    }

    return _UnifiedBaseDeviceCard(
      icon: LucideIcons.focus,
      title: 'Focuser',
      subtitle: widget.focuserState.deviceName ?? 'Motor focuser',
      isConnected: _isConnected,
      isConnecting: _isConnecting || widget.focuserState.connectionState == DeviceConnectionState.connecting,
      statusLabel: _isConnected ? 'Ready' : 'Idle',
      statusDetails: statusDetails,
      accentColor: widget.colors.success,
      colors: widget.colors,
      isOptional: true,
      isHovered: _isHovered,
      onHoverChanged: (hovered) => setState(() => _isHovered = hovered),
      unifiedDevices: unifiedFocusers,
      selectedDevice: _selectedDevice,
      onDeviceSelected: (device) => setState(() => _selectedDevice = device),
      onBackendSelected: _handleBackendSelected,
      onConnect: _handleConnect,
      onDisconnect: _handleDisconnect,
    );
  }

  void _handleBackendSelected(DriverBackend backend) {
    if (_selectedDevice == null) return;
    setState(() {
      _selectedDevice = _selectedDevice!.withSelectedBackend(backend);
    });
  }

  Future<void> _handleConnect() async {
    if (_selectedDevice == null) return;

    setState(() => _isConnecting = true);
    try {
      final deviceId = _selectedDevice!.activeDeviceId;
      await ref.read(deviceServiceProvider).connectFocuser(deviceId);

      // Show save to profile dialog after successful connection
      if (mounted) {
        await showSaveToProfileDialog(
          context: context,
          ref: ref,
          deviceId: deviceId,
          deviceName: _selectedDevice!.displayName,
          deviceType: DeviceCategory.focuser,
        );
      }
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Failed to connect: $e')),
        );
      }
    } finally {
      if (mounted) setState(() => _isConnecting = false);
    }
  }

  Future<void> _handleDisconnect() async {
    await ref.read(deviceServiceProvider).disconnectFocuser();
  }
}

// ============================================================================
// Filter Wheel Device Card
// ============================================================================

class _FilterWheelDeviceCard extends ConsumerStatefulWidget {
  final FilterWheelState filterWheelState;
  final NightshadeColors colors;

  const _FilterWheelDeviceCard({
    required this.filterWheelState,
    required this.colors,
  });

  @override
  ConsumerState<_FilterWheelDeviceCard> createState() => _FilterWheelDeviceCardState();
}

class _FilterWheelDeviceCardState extends ConsumerState<_FilterWheelDeviceCard> {
  UnifiedDevice? _selectedDevice;
  bool _isHovered = false;
  bool _isConnecting = false;

  bool get _isConnected =>
      widget.filterWheelState.connectionState == DeviceConnectionState.connected;

  @override
  Widget build(BuildContext context) {
    // Use unified filter wheels (grouped by physical device)
    final unifiedFilterWheels = ref.watch(unifiedFilterWheelsProvider);

    final statusDetails = <String>[];
    if (_isConnected) {
      final filterName = widget.filterWheelState.currentFilterName ?? 'Unknown';
      statusDetails.add('Filter: $filterName');
    } else {
      statusDetails.add('Filter: ---');
    }

    return _UnifiedBaseDeviceCard(
      icon: LucideIcons.circle,
      title: 'Filter Wheel',
      subtitle: widget.filterWheelState.deviceName ?? 'Electronic filter wheel',
      isConnected: _isConnected,
      isConnecting: _isConnecting || widget.filterWheelState.connectionState == DeviceConnectionState.connecting,
      statusLabel: _isConnected ? 'Ready' : 'Idle',
      statusDetails: statusDetails,
      accentColor: widget.colors.warning,
      colors: widget.colors,
      isOptional: true,
      isHovered: _isHovered,
      onHoverChanged: (hovered) => setState(() => _isHovered = hovered),
      unifiedDevices: unifiedFilterWheels,
      selectedDevice: _selectedDevice,
      onDeviceSelected: (device) => setState(() => _selectedDevice = device),
      onBackendSelected: _handleBackendSelected,
      onConnect: _handleConnect,
      onDisconnect: _handleDisconnect,
    );
  }

  void _handleBackendSelected(DriverBackend backend) {
    if (_selectedDevice == null) return;
    setState(() {
      _selectedDevice = _selectedDevice!.withSelectedBackend(backend);
    });
  }

  Future<void> _handleConnect() async {
    if (_selectedDevice == null) return;

    setState(() => _isConnecting = true);
    try {
      final deviceId = _selectedDevice!.activeDeviceId;
      await ref.read(deviceServiceProvider).connectFilterWheel(deviceId);

      // Show save to profile dialog after successful connection
      if (mounted) {
        await showSaveToProfileDialog(
          context: context,
          ref: ref,
          deviceId: deviceId,
          deviceName: _selectedDevice!.displayName,
          deviceType: DeviceCategory.filterWheel,
        );
      }
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Failed to connect: $e')),
        );
      }
    } finally {
      if (mounted) setState(() => _isConnecting = false);
    }
  }

  Future<void> _handleDisconnect() async {
    await ref.read(deviceServiceProvider).disconnectFilterWheel();
  }
}

// ============================================================================
// Guider Device Card
// ============================================================================

class _GuiderDeviceCard extends ConsumerStatefulWidget {
  final GuiderState guiderState;
  final NightshadeColors colors;

  const _GuiderDeviceCard({
    required this.guiderState,
    required this.colors,
  });

  @override
  ConsumerState<_GuiderDeviceCard> createState() => _GuiderDeviceCardState();
}

class _GuiderDeviceCardState extends ConsumerState<_GuiderDeviceCard> {
  UnifiedDevice? _selectedDevice;
  bool _isHovered = false;
  bool _isConnecting = false;

  bool get _isConnected =>
      widget.guiderState.connectionState == DeviceConnectionState.connected;

  @override
  Widget build(BuildContext context) {
    // Use unified guiders (grouped by physical device)
    final unifiedGuiders = ref.watch(unifiedGuidersProvider);

    final statusDetails = <String>[];
    if (_isConnected && widget.guiderState.rmsTotal != null) {
      statusDetails.add('RMS: ${widget.guiderState.rmsTotal!.toStringAsFixed(2)}"');
    } else {
      statusDetails.add('RMS: ---');
    }

    return _UnifiedBaseDeviceCard(
      icon: LucideIcons.crosshair,
      title: 'Guider',
      subtitle: widget.guiderState.deviceName ?? 'Autoguiding camera',
      isConnected: _isConnected,
      isConnecting: _isConnecting || widget.guiderState.connectionState == DeviceConnectionState.connecting,
      statusLabel: _getStatusLabel(),
      statusDetails: statusDetails,
      accentColor: widget.colors.info,
      colors: widget.colors,
      isOptional: true,
      isHovered: _isHovered,
      onHoverChanged: (hovered) => setState(() => _isHovered = hovered),
      unifiedDevices: unifiedGuiders,
      selectedDevice: _selectedDevice,
      onDeviceSelected: (device) => setState(() => _selectedDevice = device),
      onBackendSelected: _handleBackendSelected,
      onConnect: _handleConnect,
      onDisconnect: _handleDisconnect,
    );
  }

  void _handleBackendSelected(DriverBackend backend) {
    if (_selectedDevice == null) return;
    setState(() {
      _selectedDevice = _selectedDevice!.withSelectedBackend(backend);
    });
  }

  String _getStatusLabel() {
    if (widget.guiderState.isCalibrating) return 'Calibrating';
    if (widget.guiderState.isGuiding) return 'Guiding';
    if (_isConnected) return 'Ready';
    return 'Idle';
  }

  Future<void> _handleConnect() async {
    if (_selectedDevice == null) return;

    setState(() => _isConnecting = true);
    try {
      final deviceId = _selectedDevice!.activeDeviceId;
      await ref.read(deviceServiceProvider).connectGuider(deviceId);

      // Show save to profile dialog after successful connection
      if (mounted) {
        await showSaveToProfileDialog(
          context: context,
          ref: ref,
          deviceId: deviceId,
          deviceName: _selectedDevice!.displayName,
          deviceType: DeviceCategory.guider,
        );
      }
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Failed to connect: $e')),
        );
      }
    } finally {
      if (mounted) setState(() => _isConnecting = false);
    }
  }

  Future<void> _handleDisconnect() async {
    await ref.read(deviceServiceProvider).disconnectGuider();
  }
}

// ============================================================================
// Rotator Device Card
// ============================================================================

class _RotatorDeviceCard extends ConsumerStatefulWidget {
  final RotatorState rotatorState;
  final NightshadeColors colors;

  const _RotatorDeviceCard({
    required this.rotatorState,
    required this.colors,
  });

  @override
  ConsumerState<_RotatorDeviceCard> createState() => _RotatorDeviceCardState();
}

class _RotatorDeviceCardState extends ConsumerState<_RotatorDeviceCard> {
  UnifiedDevice? _selectedDevice;
  bool _isHovered = false;
  bool _isConnecting = false;

  bool get _isConnected =>
      widget.rotatorState.connectionState == DeviceConnectionState.connected;

  @override
  Widget build(BuildContext context) {
    // Use unified rotators (grouped by physical device)
    final unifiedRotators = ref.watch(unifiedRotatorsProvider);

    final statusDetails = <String>[];
    if (_isConnected && widget.rotatorState.position != null) {
      statusDetails.add('Position: ${widget.rotatorState.position!.toStringAsFixed(1)}°');
      if (widget.rotatorState.mechanicalPosition != null) {
        statusDetails.add('Mechanical: ${widget.rotatorState.mechanicalPosition!.toStringAsFixed(1)}°');
      }
    } else {
      statusDetails.add('Position: ---');
    }

    return _UnifiedBaseDeviceCard(
      icon: LucideIcons.rotateCw,
      title: 'Rotator',
      subtitle: widget.rotatorState.deviceName ?? 'Field rotator',
      isConnected: _isConnected,
      isConnecting: _isConnecting || widget.rotatorState.connectionState == DeviceConnectionState.connecting,
      statusLabel: widget.rotatorState.isMoving ? 'Moving' : (_isConnected ? 'Ready' : 'Idle'),
      statusDetails: statusDetails,
      accentColor: widget.colors.accent,
      colors: widget.colors,
      isOptional: true,
      isHovered: _isHovered,
      onHoverChanged: (hovered) => setState(() => _isHovered = hovered),
      unifiedDevices: unifiedRotators,
      selectedDevice: _selectedDevice,
      onDeviceSelected: (device) => setState(() => _selectedDevice = device),
      onBackendSelected: _handleBackendSelected,
      onConnect: _handleConnect,
      onDisconnect: _handleDisconnect,
    );
  }

  void _handleBackendSelected(DriverBackend backend) {
    if (_selectedDevice == null) return;
    setState(() {
      _selectedDevice = _selectedDevice!.withSelectedBackend(backend);
    });
  }

  Future<void> _handleConnect() async {
    if (_selectedDevice == null) return;

    setState(() => _isConnecting = true);
    try {
      final deviceId = _selectedDevice!.activeDeviceId;
      await ref.read(rotatorStateProvider.notifier).connect(deviceId);

      // Show save to profile dialog after successful connection
      if (mounted) {
        await showSaveToProfileDialog(
          context: context,
          ref: ref,
          deviceId: deviceId,
          deviceName: _selectedDevice!.displayName,
          deviceType: DeviceCategory.rotator,
        );
      }
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Failed to connect: $e')),
        );
      }
    } finally {
      if (mounted) setState(() => _isConnecting = false);
    }
  }

  Future<void> _handleDisconnect() async {
    ref.read(rotatorStateProvider.notifier).disconnect();
  }
}

/// Dropdown for selecting unified devices (grouped by physical device)
/// Shows device name with backend count, not individual backend entries
class _UnifiedDeviceDropdown extends StatelessWidget {
  final List<UnifiedDevice> devices;
  final UnifiedDevice? selectedDevice;
  final ValueChanged<UnifiedDevice?> onSelected;
  final bool isEnabled;
  final NightshadeColors colors;

  const _UnifiedDeviceDropdown({
    required this.devices,
    required this.selectedDevice,
    required this.onSelected,
    required this.isEnabled,
    required this.colors,
  });

  Color _getBackendColor(DriverBackend backend) {
    switch (backend) {
      case DriverBackend.native:
        return colors.success;
      case DriverBackend.ascom:
        return colors.info;
      case DriverBackend.alpaca:
        return colors.warning;
      case DriverBackend.indi:
        return const Color(0xFF9333EA);
      case DriverBackend.simulator:
        return colors.textMuted;
    }
  }

  @override
  Widget build(BuildContext context) {
    return PopupMenuButton<UnifiedDevice>(
      enabled: isEnabled,
      onSelected: onSelected,
      offset: const Offset(0, 40),
      shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(10)),
      color: colors.surface,
      itemBuilder: (context) => devices.map((device) {
        final backendCount = device.availableBackends.length;
        final recommended = device.recommendedBackend;

        return PopupMenuItem<UnifiedDevice>(
          value: device,
          child: Row(
            children: [
              Expanded(
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Text(
                      device.displayName,
                      style: TextStyle(
                        fontSize: 13,
                        fontWeight: FontWeight.w500,
                        color: colors.textPrimary,
                      ),
                    ),
                    const SizedBox(height: 2),
                    // Show available backends as small colored dots/badges
                    Row(
                      children: [
                        ...device.sortedBackends.map((backend) {
                          final isRecommended = backend == recommended;
                          return Container(
                            margin: const EdgeInsets.only(right: 4),
                            padding: const EdgeInsets.symmetric(horizontal: 4, vertical: 1),
                            decoration: BoxDecoration(
                              color: _getBackendColor(backend).withValues(alpha: 0.15),
                              borderRadius: BorderRadius.circular(4),
                              border: Border.all(
                                color: _getBackendColor(backend).withValues(alpha: 0.3),
                              ),
                            ),
                            child: Row(
                              mainAxisSize: MainAxisSize.min,
                              children: [
                                if (isRecommended) ...[
                                  Icon(
                                    Icons.star,
                                    size: 8,
                                    color: colors.warning,
                                  ),
                                  const SizedBox(width: 2),
                                ],
                                Text(
                                  backend.shortLabel,
                                  style: TextStyle(
                                    fontSize: 9,
                                    fontWeight: isRecommended ? FontWeight.w600 : FontWeight.w400,
                                    color: _getBackendColor(backend),
                                  ),
                                ),
                              ],
                            ),
                          );
                        }),
                      ],
                    ),
                  ],
                ),
              ),
              if (backendCount > 1)
                Container(
                  padding: const EdgeInsets.symmetric(horizontal: 6, vertical: 2),
                  decoration: BoxDecoration(
                    color: colors.primary.withValues(alpha: 0.1),
                    borderRadius: BorderRadius.circular(8),
                  ),
                  child: Text(
                    '$backendCount drivers',
                    style: TextStyle(
                      fontSize: 9,
                      color: colors.primary,
                      fontWeight: FontWeight.w500,
                    ),
                  ),
                ),
            ],
          ),
        );
      }).toList(),
      child: Container(
        padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 10),
        decoration: BoxDecoration(
          color: isEnabled ? colors.surfaceAlt : colors.surface,
          borderRadius: BorderRadius.circular(10),
          border: Border.all(color: colors.border),
        ),
        child: Row(
          children: [
            Expanded(
              child: Text(
                selectedDevice?.displayName ?? 'Select device...',
                style: TextStyle(
                  fontSize: 12,
                  color: selectedDevice != null
                      ? colors.textPrimary
                      : colors.textMuted,
                ),
              ),
            ),
            if (selectedDevice != null && selectedDevice!.availableBackends.length > 1) ...[
              Container(
                padding: const EdgeInsets.symmetric(horizontal: 4, vertical: 1),
                decoration: BoxDecoration(
                  color: _getBackendColor(selectedDevice!.activeBackend).withValues(alpha: 0.15),
                  borderRadius: BorderRadius.circular(4),
                ),
                child: Text(
                  selectedDevice!.activeBackend.shortLabel,
                  style: TextStyle(
                    fontSize: 9,
                    color: _getBackendColor(selectedDevice!.activeBackend),
                  ),
                ),
              ),
              const SizedBox(width: 6),
            ],
            Icon(
              LucideIcons.chevronDown,
              size: 14,
              color: colors.textMuted,
            ),
          ],
        ),
      ),
    );
  }
}

/// Base device card that uses UnifiedDevice (grouped by physical device)
/// Shows device dropdown, backend selector chips, and connect button
class _UnifiedBaseDeviceCard extends StatelessWidget {
  final IconData icon;
  final String title;
  final String subtitle;
  final bool isConnected;
  final bool isConnecting;
  final String statusLabel;
  final List<String> statusDetails;
  final bool isOptional;
  final Color accentColor;
  final NightshadeColors colors;
  final bool isHovered;
  final ValueChanged<bool> onHoverChanged;
  final List<UnifiedDevice> unifiedDevices;
  final UnifiedDevice? selectedDevice;
  final ValueChanged<UnifiedDevice?> onDeviceSelected;
  final ValueChanged<DriverBackend> onBackendSelected;
  final VoidCallback onConnect;
  final VoidCallback onDisconnect;

  const _UnifiedBaseDeviceCard({
    required this.icon,
    required this.title,
    required this.subtitle,
    required this.isConnected,
    required this.isConnecting,
    required this.statusLabel,
    required this.statusDetails,
    this.isOptional = false,
    required this.accentColor,
    required this.colors,
    required this.isHovered,
    required this.onHoverChanged,
    required this.unifiedDevices,
    required this.selectedDevice,
    required this.onDeviceSelected,
    required this.onBackendSelected,
    required this.onConnect,
    required this.onDisconnect,
  });

  @override
  Widget build(BuildContext context) {
    return MouseRegion(
      onEnter: (_) => onHoverChanged(true),
      onExit: (_) => onHoverChanged(false),
      child: AnimatedContainer(
        duration: const Duration(milliseconds: 200),
        padding: const EdgeInsets.all(20),
        decoration: BoxDecoration(
          color: colors.surface,
          borderRadius: BorderRadius.circular(16),
          border: Border.all(
            color: isHovered
                ? accentColor.withValues(alpha: 0.5)
                : colors.border,
            width: isHovered ? 1.5 : 1,
          ),
          boxShadow: isHovered
              ? [
                  BoxShadow(
                    color: accentColor.withValues(alpha: 0.1),
                    blurRadius: 20,
                    offset: const Offset(0, 8),
                  ),
                ]
              : [
                  BoxShadow(
                    color: Colors.black.withValues(alpha: 0.05),
                    blurRadius: 10,
                    offset: const Offset(0, 4),
                  ),
                ],
        ),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            // Header
            Row(
              children: [
                // Icon container
                Container(
                  width: 44,
                  height: 44,
                  decoration: BoxDecoration(
                    gradient: LinearGradient(
                      colors: [
                        accentColor.withValues(alpha: 0.2),
                        accentColor.withValues(alpha: 0.05),
                      ],
                      begin: Alignment.topLeft,
                      end: Alignment.bottomRight,
                    ),
                    borderRadius: BorderRadius.circular(12),
                    border: Border.all(
                      color: accentColor.withValues(alpha: 0.2),
                    ),
                  ),
                  child: Icon(
                    icon,
                    size: 20,
                    color: isConnected ? colors.success : accentColor,
                  ),
                ),
                const SizedBox(width: 14),
                Expanded(
                  child: Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: [
                      Row(
                        children: [
                          Text(
                            title,
                            style: TextStyle(
                              fontSize: 15,
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
                                  fontSize: 9,
                                  fontWeight: FontWeight.w500,
                                  color: colors.textMuted,
                                ),
                              ),
                            ),
                          ],
                        ],
                      ),
                      const SizedBox(height: 2),
                      Text(
                        subtitle,
                        style: TextStyle(
                          fontSize: 11,
                          color: colors.textMuted,
                        ),
                        maxLines: 1,
                        overflow: TextOverflow.ellipsis,
                      ),
                    ],
                  ),
                ),
                // Connection indicator
                _ConnectionIndicator(
                  isConnected: isConnected,
                  isConnecting: isConnecting,
                  colors: colors,
                ),
              ],
            ),

            const SizedBox(height: 16),

            // Device selector (unified - shows grouped devices)
            _UnifiedDeviceDropdown(
              devices: unifiedDevices,
              selectedDevice: selectedDevice,
              onSelected: onDeviceSelected,
              isEnabled: !isConnected && !isConnecting,
              colors: colors,
            ),

            // Backend selector (only show if selected device has multiple backends)
            if (selectedDevice != null &&
                selectedDevice!.availableBackends.length > 1) ...[
              const SizedBox(height: 10),
              BackendSelectorChips(
                availableBackends: selectedDevice!.sortedBackends,
                selectedBackend: selectedDevice!.activeBackend,
                recommendedBackend: selectedDevice!.recommendedBackend,
                onBackendSelected: onBackendSelected,
                isEnabled: !isConnected && !isConnecting,
              ),
            ],

            const SizedBox(height: 12),

            // Status details
            Container(
              padding: const EdgeInsets.all(12),
              decoration: BoxDecoration(
                color: colors.background,
                borderRadius: BorderRadius.circular(10),
              ),
              child: Row(
                children: [
                  Text(
                    'Status:',
                    style: TextStyle(
                      fontSize: 11,
                      color: colors.textMuted,
                    ),
                  ),
                  const SizedBox(width: 8),
                  Text(
                    statusLabel,
                    style: TextStyle(
                      fontSize: 11,
                      fontWeight: FontWeight.w500,
                      color: isConnected ? colors.success : colors.textSecondary,
                    ),
                  ),
                  const Spacer(),
                  ...statusDetails.map((detail) {
                    return Padding(
                      padding: const EdgeInsets.only(left: 16),
                      child: Text(
                        detail,
                        style: TextStyle(
                          fontSize: 11,
                          color: colors.textMuted,
                        ),
                      ),
                    );
                  }),
                ],
              ),
            ),

            const SizedBox(height: 16),

            // Connect button
            SizedBox(
              width: double.infinity,
              child: _ConnectButton(
                isConnected: isConnected,
                isConnecting: isConnecting,
                isEnabled: selectedDevice != null || isConnected,
                accentColor: accentColor,
                colors: colors,
                onPressed: isConnected ? onDisconnect : onConnect,
              ),
            ),
          ],
        ),
      ),
    );
  }
}


class _ConnectionIndicator extends StatelessWidget {
  final bool isConnected;
  final bool isConnecting;
  final NightshadeColors colors;

  const _ConnectionIndicator({
    required this.isConnected,
    required this.isConnecting,
    required this.colors,
  });

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 6),
      decoration: BoxDecoration(
        color: isConnected
            ? colors.success.withValues(alpha: 0.15)
            : isConnecting
                ? colors.warning.withValues(alpha: 0.15)
                : colors.surfaceAlt,
        borderRadius: BorderRadius.circular(20),
        border: Border.all(
          color: isConnected
              ? colors.success.withValues(alpha: 0.3)
              : isConnecting
                  ? colors.warning.withValues(alpha: 0.3)
                  : colors.border,
        ),
      ),
      child: Row(
        mainAxisSize: MainAxisSize.min,
        children: [
          if (isConnecting)
            SizedBox(
              width: 8,
              height: 8,
              child: CircularProgressIndicator(
                strokeWidth: 1.5,
                color: colors.warning,
              ),
            )
          else
            Container(
              width: 6,
              height: 6,
              decoration: BoxDecoration(
                shape: BoxShape.circle,
                color: isConnected ? colors.success : colors.textMuted,
              ),
            ),
          const SizedBox(width: 6),
          Text(
            isConnecting
                ? 'Connecting...'
                : isConnected
                    ? 'Connected'
                    : 'Offline',
            style: TextStyle(
              fontSize: 10,
              fontWeight: FontWeight.w500,
              color: isConnected
                  ? colors.success
                  : isConnecting
                      ? colors.warning
                      : colors.textMuted,
            ),
          ),
        ],
      ),
    );
  }
}

class _ConnectButton extends StatefulWidget {
  final bool isConnected;
  final bool isConnecting;
  final bool isEnabled;
  final Color accentColor;
  final NightshadeColors colors;
  final VoidCallback onPressed;

  const _ConnectButton({
    required this.isConnected,
    required this.isConnecting,
    required this.isEnabled,
    required this.accentColor,
    required this.colors,
    required this.onPressed,
  });

  @override
  State<_ConnectButton> createState() => _ConnectButtonState();
}

class _ConnectButtonState extends State<_ConnectButton> {
  bool _isHovered = false;

  @override
  Widget build(BuildContext context) {
    final canPress = widget.isEnabled && !widget.isConnecting;
    
    return MouseRegion(
      onEnter: (_) => setState(() => _isHovered = true),
      onExit: (_) => setState(() => _isHovered = false),
      child: GestureDetector(
        onTap: canPress ? widget.onPressed : null,
        child: AnimatedContainer(
          duration: const Duration(milliseconds: 150),
          padding: const EdgeInsets.symmetric(vertical: 12),
          decoration: BoxDecoration(
            gradient: widget.isConnected || !canPress
                ? null
                : LinearGradient(
                    colors: [
                      widget.accentColor,
                      widget.accentColor.withValues(alpha: 0.8),
                    ],
                  ),
            color: widget.isConnected
                ? widget.colors.surfaceAlt
                : !canPress
                    ? widget.colors.surfaceAlt.withValues(alpha: 0.5)
                    : null,
            borderRadius: BorderRadius.circular(10),
            border: widget.isConnected
                ? Border.all(color: widget.colors.border)
                : null,
            boxShadow: !widget.isConnected && canPress && _isHovered
                ? [
                    BoxShadow(
                      color: widget.accentColor.withValues(alpha: 0.4),
                      blurRadius: 12,
                      offset: const Offset(0, 4),
                    ),
                  ]
                : null,
          ),
          child: Center(
            child: widget.isConnecting
                ? const Row(
                    mainAxisSize: MainAxisSize.min,
                    children: [
                      SizedBox(
                        width: 14,
                        height: 14,
                        child: CircularProgressIndicator(
                          strokeWidth: 2,
                          color: Colors.white,
                        ),
                      ),
                      SizedBox(width: 8),
                      Text(
                        'Connecting...',
                        style: TextStyle(
                          fontSize: 13,
                          fontWeight: FontWeight.w600,
                          color: Colors.white,
                        ),
                      ),
                    ],
                  )
                : Text(
                    widget.isConnected ? 'Disconnect' : 'Connect',
                    style: TextStyle(
                      fontSize: 13,
                      fontWeight: FontWeight.w600,
                      color: widget.isConnected
                          ? widget.colors.textSecondary
                          : canPress
                              ? Colors.white
                              : widget.colors.textMuted,
                    ),
                  ),
          ),
        ),
      ),
    );
  }
}

class _TelescopeCard extends ConsumerWidget {
  final NightshadeColors colors;

  const _TelescopeCard({required this.colors});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final profileAsync = ref.watch(activeProfileProvider);
    return Container(
      padding: const EdgeInsets.all(20),
      decoration: BoxDecoration(
        color: colors.surface,
        borderRadius: BorderRadius.circular(16),
        border: Border.all(color: colors.border),
      ),
      child: profileAsync.when(
        data: (db.EquipmentProfile? profile) {
          return Row(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Expanded(
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Row(
                      children: [
                        Container(
                          width: 44,
                          height: 44,
                          decoration: BoxDecoration(
                            gradient: LinearGradient(
                              colors: [
                                colors.accent.withValues(alpha: 0.2),
                                colors.accent.withValues(alpha: 0.05),
                              ],
                              begin: Alignment.topLeft,
                              end: Alignment.bottomRight,
                            ),
                            borderRadius: BorderRadius.circular(12),
                            border: Border.all(
                              color: colors.accent.withValues(alpha: 0.2),
                            ),
                          ),
                          child: Icon(
                            LucideIcons.scan,
                            size: 20,
                            color: colors.accent,
                          ),
                        ),
                        const SizedBox(width: 14),
                        Expanded(
                          child: Column(
                            crossAxisAlignment: CrossAxisAlignment.start,
                            children: [
                              Text(
                                profile?.name ?? 'No active profile',
                                style: TextStyle(
                                  fontSize: 15,
                                  fontWeight: FontWeight.w600,
                                  color: colors.textPrimary,
                                ),
                              ),
                              const SizedBox(height: 2),
                              Text(
                                profile?.description ??
                                    'Select a profile to use its equipment assignments',
                                style: TextStyle(
                                  fontSize: 11,
                                  color: colors.textMuted,
                                ),
                              ),
                            ],
                          ),
                        ),
                      ],
                    ),
                    const SizedBox(height: 16),
                    if (profile == null)
                      Text(
                        'No profile selected. Open the Profiles tab to activate one.',
                        style: TextStyle(color: colors.textSecondary, fontSize: 12),
                      )
                    else
                      Column(
                        crossAxisAlignment: CrossAxisAlignment.start,
                        children: [
                          _ProfileDeviceLine(
                            icon: LucideIcons.camera,
                            label: 'Camera',
                            value: profile.cameraId ?? 'Not assigned',
                            colors: colors,
                          ),
                          const SizedBox(height: 6),
                          _ProfileDeviceLine(
                            icon: LucideIcons.move3d,
                            label: 'Mount',
                            value: profile.mountId ?? 'Not assigned',
                            colors: colors,
                          ),
                          const SizedBox(height: 6),
                          _ProfileDeviceLine(
                            icon: LucideIcons.focus,
                            label: 'Focuser',
                            value: profile.focuserId ?? 'Not assigned',
                            colors: colors,
                          ),
                        ],
                      ),
                  ],
                ),
              ),
              const SizedBox(width: 24),
              _OpticsSummaryCard(profile: profile, colors: colors),
            ],
          );
        },
        loading: () => const Center(child: CircularProgressIndicator()),
        error: (error, stack) => Center(
          child: Text(
            'Failed to load active profile: $error',
            style: TextStyle(color: colors.error),
          ),
        ),
      ),
    );
  }
}

class _ProfileDeviceLine extends StatelessWidget {
  final IconData icon;
  final String label;
  final String value;
  final NightshadeColors colors;

  const _ProfileDeviceLine({
    required this.icon,
    required this.label,
    required this.value,
    required this.colors,
  });

  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        Icon(icon, size: 14, color: colors.textMuted),
        const SizedBox(width: 8),
        Text(
          '$label:',
          style: TextStyle(fontSize: 12, color: colors.textSecondary),
        ),
        const SizedBox(width: 6),
        Expanded(
          child: Text(
            value,
            style: TextStyle(fontSize: 12, color: colors.textPrimary),
            overflow: TextOverflow.ellipsis,
          ),
        ),
      ],
    );
  }
}

class _OpticsSummaryCard extends StatelessWidget {
  final db.EquipmentProfile? profile;
  final NightshadeColors colors;

  const _OpticsSummaryCard({required this.profile, required this.colors});

  @override
  Widget build(BuildContext context) {
    final focalLength = profile?.focalLength ?? 0;
    final aperture = profile?.aperture ?? 0;
    final focalRatio = profile?.focalRatio;

    String formatValue(double value, {String suffix = ''}) {
      if (value <= 0) return '--';
      return '${value.toStringAsFixed(0)}$suffix';
    }

    final ratioText = focalRatio != null && focalRatio > 0
        ? 'f/${focalRatio.toStringAsFixed(1)}'
        : '--';

    return Container(
      width: 200,
      padding: const EdgeInsets.all(16),
      decoration: BoxDecoration(
        color: colors.background,
        borderRadius: BorderRadius.circular(12),
      ),
      child: Column(
        children: [
          _SpecRow(
            label: 'Focal Length',
            value: formatValue(focalLength, suffix: 'mm'),
            colors: colors,
          ),
          const SizedBox(height: 8),
          _SpecRow(
            label: 'f-ratio',
            value: ratioText,
            colors: colors,
          ),
          const SizedBox(height: 8),
          _SpecRow(
            label: 'Aperture',
            value: formatValue(aperture, suffix: 'mm'),
            colors: colors,
          ),
        ],
      ),
    );
  }
}

class _SpecRow extends StatelessWidget {
  final String label;
  final String value;
  final NightshadeColors colors;

  const _SpecRow({
    required this.label,
    required this.value,
    required this.colors,
  });

  @override
  Widget build(BuildContext context) {
    return Row(
      mainAxisAlignment: MainAxisAlignment.spaceBetween,
      children: [
        Text(
          label,
          style: TextStyle(
            fontSize: 11,
            color: colors.textMuted,
          ),
        ),
        Text(
          value,
          style: TextStyle(
            fontSize: 12,
            fontWeight: FontWeight.w600,
            color: colors.textPrimary,
          ),
        ),
      ],
    );
  }
}
