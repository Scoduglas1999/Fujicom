
import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:lucide_icons/lucide_icons.dart';
import 'package:nightshade_core/nightshade_core.dart' hide DeviceType, PlateSolveResult;
import 'package:nightshade_core/src/services/plate_solve_service.dart' show PlateSolveResult;
import 'package:nightshade_ui/nightshade_ui.dart';

import 'device_card.dart';

class MountControlPanel extends ConsumerStatefulWidget {
  final MountState mountState;
  final AsyncValue<List<AvailableDevice>> availableMounts;
  final NightshadeColors colors;

  const MountControlPanel({
    super.key,
    required this.mountState,
    required this.availableMounts,
    required this.colors,
  });

  @override
  ConsumerState<MountControlPanel> createState() => _MountControlPanelState();
}

class _MountControlPanelState extends ConsumerState<MountControlPanel> {
  String? _selectedDeviceId;
  final bool _isHovered = false;
  bool _isConnecting = false;
  bool _isSolving = false;

  bool get _isConnected =>
      widget.mountState.connectionState == DeviceConnectionState.connected;

  @override
  Widget build(BuildContext context) {
    final statusDetails = <String>[];
    if (_isConnected && widget.mountState.ra != null) {
      statusDetails.add('RA: ${widget.mountState.ra!.toStringAsFixed(2)}h');
      statusDetails.add('Dec: ${widget.mountState.dec!.toStringAsFixed(1)}°');
    } else {
      statusDetails.addAll(['RA: ---', 'Dec: ---']);
    }

    return Column(
      children: [
        DeviceCard(
          title: 'Mount',
          deviceType: DeviceType.mount,
          isConnected: _isConnected,
          selectedDevice: _selectedDeviceId,
          availableDevices: widget.availableMounts.valueOrNull?.map((d) => d.id).toList() ?? [],
          onDeviceSelected: (id) => setState(() => _selectedDeviceId = id),
          onConnect: _handleConnect,
          onDisconnect: _handleDisconnect,
          statusWidget: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Text(
                _getStatusLabel(),
                style: TextStyle(
                  color: _isConnected ? widget.colors.success : widget.colors.textSecondary,
                  fontWeight: FontWeight.w500,
                ),
              ),
              const SizedBox(height: 4),
              ...statusDetails.map((detail) => Text(detail)),
            ],
          ),
        ),
        if (_isConnected) ...[
          const SizedBox(height: 16),
          _buildControls(),
        ],
      ],
    );
  }

  Widget _buildControls() {
    return Container(
      padding: const EdgeInsets.all(16),
      decoration: BoxDecoration(
        color: widget.colors.surface,
        borderRadius: BorderRadius.circular(12),
        border: Border.all(color: widget.colors.border),
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Text(
            'Controls',
            style: TextStyle(
              fontSize: 14,
              fontWeight: FontWeight.w600,
              color: widget.colors.textPrimary,
            ),
          ),
          const SizedBox(height: 12),
          Row(
            children: [
              Expanded(
                child: NightshadeButton(
                  label: widget.mountState.isParked ? 'Unpark' : 'Park',
                  icon: LucideIcons.parkingSquare,
                  variant: ButtonVariant.outline,
                  onPressed: _togglePark,
                ),
              ),
              const SizedBox(width: 12),
              Expanded(
                child: NightshadeButton(
                  label: widget.mountState.isTracking ? 'Stop Track' : 'Track',
                  icon: LucideIcons.activity,
                  variant: widget.mountState.isTracking ? ButtonVariant.primary : ButtonVariant.outline,
                  onPressed: _toggleTracking,
                ),
              ),
            ],
          ),
          const SizedBox(height: 12),
          SizedBox(
            width: double.infinity,
            child: NightshadeButton(
              label: _isSolving ? 'Solving...' : 'Plate Solve & Sync',
              icon: _isSolving ? null : LucideIcons.target,
              variant: ButtonVariant.primary,
              isLoading: _isSolving,
              onPressed: _isSolving ? null : _handlePlateSolveAndSync,
            ),
          ),
        ],
      ),
    );
  }

  String _getStatusLabel() {
    if (widget.mountState.isSlewing) return 'Slewing';
    if (widget.mountState.isParked) return 'Parked';
    if (widget.mountState.isTracking) return 'Tracking';
    if (_isConnected) return 'Ready';
    return 'Idle';
  }

  Future<void> _handleConnect() async {
    if (_selectedDeviceId == null) return;
    
    setState(() => _isConnecting = true);
    try {
      await ref.read(deviceServiceProvider).connectMount(_selectedDeviceId!);
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

  Future<void> _togglePark() async {
    try {
      if (widget.mountState.isParked) {
        await ref.read(deviceServiceProvider).unparkMount();
      } else {
        await ref.read(deviceServiceProvider).parkMount();
      }
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Failed to park/unpark: $e')),
        );
      }
    }
  }

  Future<void> _toggleTracking() async {
    try {
      await ref.read(deviceServiceProvider).setMountTracking(!widget.mountState.isTracking);
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Failed to toggle tracking: $e')),
        );
      }
    }
  }

  Future<void> _handlePlateSolveAndSync() async {
    setState(() => _isSolving = true);
    
    try {
      // 1. Capture Image
      final imagingService = ref.read(imagingServiceProvider);
      // Use short exposure for plate solving
      const settings = ExposureSettings(
        exposureTime: 2.0, // 2 seconds
        gain: 100,
        offset: 10,
        binningX: 2, // Bin 2x2 for speed
        binningY: 2,
        frameType: FrameType.light,
      );
      
      final image = await imagingService.captureImage(
        settings: settings,
        targetName: 'Plate Solve',
      );
      
      if (image == null || image.filePath == null) {
        throw Exception('Failed to capture image');
      }
      
      // 2. Plate Solve
      final plateSolveService = ref.read(plateSolveServiceProvider);
      // Use blind solve if we don't have good coordinates, or near solve if we do
      // For now, let's try blind solve as it's safer if we are lost
      // But if we have mount coordinates, we should use them as hint

      // Get ASTAP path from app settings with fallback to common paths
      final appSettings = ref.read(appSettingsProvider).value;
      final executablePath = await PlateSolverUtils.findAstapExecutable(appSettings?.astapPath);

      if (executablePath == null) {
        throw Exception(PlateSolverUtils.getAstapNotFoundMessage());
      }

      PlateSolveResult result;

      if (widget.mountState.ra != null && widget.mountState.dec != null) {
         result = await plateSolveService.solve(
          image.filePath!,
          PlateSolverConfig(
            type: PlateSolverType.astap,
            hintRa: widget.mountState.ra,
            hintDec: widget.mountState.dec,
            searchRadius: 10.0, // 10 degrees search
            executablePath: executablePath,
          ),
        );
      } else {
        result = await plateSolveService.solve(
          image.filePath!,
          PlateSolverConfig(
            type: PlateSolverType.astap,
            executablePath: executablePath,
            searchRadius: 180.0, // Blind solve
          ),
        );
      }
      
      if (!result.success) {
        throw Exception('Plate solving failed: ${result.errorMessage}');
      }
      
      if (result.ra == null || result.dec == null) {
         throw Exception('Plate solving succeeded but returned null coordinates');
      }
      
      // 3. Sync Mount
      await ref.read(deviceServiceProvider).syncMountToCoordinates(result.ra!, result.dec!);
      
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Synced to RA: ${result.ra!.toStringAsFixed(2)}h, Dec: ${result.dec!.toStringAsFixed(1)}°'),
            backgroundColor: widget.colors.success,
          ),
        );
      }
      
    } catch (e) {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Error: $e'),
            backgroundColor: widget.colors.error,
          ),
        );
      }
    } finally {
      if (mounted) setState(() => _isSolving = false);
    }
  }
}
