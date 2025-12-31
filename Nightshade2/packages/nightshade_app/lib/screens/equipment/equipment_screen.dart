import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:lucide_icons/lucide_icons.dart';
import 'package:nightshade_ui/nightshade_ui.dart';
import 'package:nightshade_core/nightshade_core.dart';
import 'package:nightshade_core/src/database/database.dart' as db;
import 'tabs/connections_tab.dart';
import 'tabs/settings_tab.dart';
import 'widgets/quick_connect_bar.dart';
import 'widgets/connection_status_zone.dart';

/// Device protocol types (kept for backward compatibility)
enum DeviceProtocol {
  ascom,
  alpaca,
  indi,
  native,
}

/// Provider for currently selected profile in the equipment screen
final selectedEquipmentProfileIdProvider = StateProvider<int?>((ref) {
  // Default to the active profile
  final activeProfile = ref.watch(activeProfileProvider).valueOrNull;
  return activeProfile?.id;
});

class EquipmentScreen extends ConsumerStatefulWidget {
  const EquipmentScreen({super.key});

  @override
  ConsumerState<EquipmentScreen> createState() => _EquipmentScreenState();
}

class _EquipmentScreenState extends ConsumerState<EquipmentScreen>
    with SingleTickerProviderStateMixin {
  int _currentSubTab = 0;
  late AnimationController _fadeController;
  late Animation<double> _fadeAnimation;

  static const _subTabs = [
    _SubTabData(icon: LucideIcons.radar, label: 'Discovery'),
    _SubTabData(icon: LucideIcons.plugZap, label: 'Connected'),
    _SubTabData(icon: LucideIcons.settings2, label: 'Settings'),
  ];

  @override
  void initState() {
    super.initState();
    _fadeController = AnimationController(
      vsync: this,
      duration: const Duration(milliseconds: 300),
    );
    _fadeAnimation = CurvedAnimation(
      parent: _fadeController,
      curve: Curves.easeOut,
    );
    _fadeController.forward();
  }

  @override
  void dispose() {
    _fadeController.dispose();
    super.dispose();
  }

  void _onTabSelected(int index) {
    if (index != _currentSubTab) {
      _fadeController.reset();
      setState(() => _currentSubTab = index);
      _fadeController.forward();
    }
  }

  @override
  Widget build(BuildContext context) {
    final colors = Theme.of(context).extension<NightshadeColors>()!;
    final profilesAsync = ref.watch(allProfilesProvider);
    final selectedProfileId = ref.watch(selectedEquipmentProfileIdProvider);

    // Check for first-time user (no profiles)
    final showOnboarding = profilesAsync.maybeWhen(
      data: (profiles) => profiles.isEmpty,
      orElse: () => false,
    );

    if (showOnboarding) {
      return _FirstTimeOnboarding(
        colors: colors,
        onStartSetup: () => _showCreateProfileWizard(context),
        onManualSetup: () {
          // Create an empty profile and proceed
          _createEmptyProfile();
        },
      );
    }

    // Get selected profile
    final selectedProfile = profilesAsync.maybeWhen(
      data: (profiles) => profiles.where((p) => p.id == selectedProfileId).firstOrNull,
      orElse: () => null,
    );

    return Column(
      children: [
        // ZONE 1: Quick Connect Bar
        QuickConnectBar(
          selectedProfileId: selectedProfileId,
          onProfileSelected: (profile) {
            ref.read(selectedEquipmentProfileIdProvider.notifier).state = profile.id;
          },
          onCreateProfile: () => _showCreateProfileDialog(context),
        ),

        // ZONE 2: Connection Status Zone
        ConnectionStatusZone(
          selectedProfile: selectedProfile,
          onConnectAll: () => _connectAllDevices(selectedProfile),
          onDisconnectAll: () => _disconnectAllDevices(),
          onEditProfile: () => _showEditProfileDialog(context, selectedProfile),
        ),

        // ZONE 3: Device Management Tabs
        Expanded(
          child: Column(
            children: [
              // Tab bar
              Container(
                padding: const EdgeInsets.fromLTRB(20, 12, 20, 0),
                decoration: BoxDecoration(
                  color: colors.background,
                  border: Border(
                    bottom: BorderSide(color: colors.border),
                  ),
                ),
                child: Row(
                  children: [
                    _SubTabBar(
                      tabs: _subTabs,
                      currentIndex: _currentSubTab,
                      onTabSelected: _onTabSelected,
                      colors: colors,
                    ),
                    const Spacer(),
                    _ConnectionBadge(colors: colors),
                  ],
                ),
              ),

              // Tab content
              Expanded(
                child: FadeTransition(
                  opacity: _fadeAnimation,
                  child: IndexedStack(
                    index: _currentSubTab,
                    children: const [
                      ConnectionsTab(), // Discovery tab (renamed)
                      _ConnectedDevicesTab(), // New connected devices tab
                      EquipmentSettingsTab(),
                    ],
                  ),
                ),
              ),
            ],
          ),
        ),
      ],
    );
  }

  Future<void> _connectAllDevices(db.EquipmentProfile? profile) async {
    if (profile == null) return;

    final deviceService = ref.read(deviceServiceProvider);

    // Connect devices in sequence
    try {
      if (profile.cameraId != null) {
        await deviceService.connectCamera(profile.cameraId!);
      }
      if (profile.mountId != null) {
        await deviceService.connectMount(profile.mountId!);
      }
      if (profile.focuserId != null) {
        await deviceService.connectFocuser(profile.focuserId!);
      }
      if (profile.filterWheelId != null) {
        await deviceService.connectFilterWheel(profile.filterWheelId!);
      }
      if (profile.guiderId != null) {
        await deviceService.connectGuider(profile.guiderId!);
      }
    } catch (e) {
      if (mounted) {
        final colors = Theme.of(context).extension<NightshadeColors>()!;
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Connection error: $e'),
            backgroundColor: colors.error,
          ),
        );
      }
    }
  }

  Future<void> _disconnectAllDevices() async {
    final deviceService = ref.read(deviceServiceProvider);

    try {
      await deviceService.disconnectCamera();
      await deviceService.disconnectMount();
      await deviceService.disconnectFocuser();
      await deviceService.disconnectFilterWheel();
      await deviceService.disconnectGuider();
    } catch (e) {
      if (mounted) {
        final colors = Theme.of(context).extension<NightshadeColors>()!;
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Disconnect error: $e'),
            backgroundColor: colors.error,
          ),
        );
      }
    }
  }

  Future<void> _createEmptyProfile() async {
    try {
      final profileService = ref.read(profileServiceProvider);
      final profileId = await profileService.createProfile('My Equipment');
      ref.read(selectedEquipmentProfileIdProvider.notifier).state = profileId;
    } catch (e) {
      if (mounted) {
        final colors = Theme.of(context).extension<NightshadeColors>()!;
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Failed to create profile: $e'),
            backgroundColor: colors.error,
          ),
        );
      }
    }
  }

  void _showCreateProfileDialog(BuildContext context) {
    final colors = Theme.of(context).extension<NightshadeColors>()!;
    final nameController = TextEditingController(text: 'New Profile');

    showDialog(
      context: context,
      builder: (context) => AlertDialog(
        backgroundColor: colors.surface,
        title: Text('Create Profile', style: TextStyle(color: colors.textPrimary)),
        content: TextField(
          controller: nameController,
          autofocus: true,
          style: TextStyle(color: colors.textPrimary),
          decoration: InputDecoration(
            labelText: 'Profile Name',
            labelStyle: TextStyle(color: colors.textMuted),
            enabledBorder: OutlineInputBorder(
              borderSide: BorderSide(color: colors.border),
            ),
            focusedBorder: OutlineInputBorder(
              borderSide: BorderSide(color: colors.primary),
            ),
          ),
        ),
        actions: [
          TextButton(
            onPressed: () => Navigator.pop(context),
            child: Text('Cancel', style: TextStyle(color: colors.textMuted)),
          ),
          FilledButton(
            onPressed: () async {
              Navigator.pop(context);
              try {
                final profileService = ref.read(profileServiceProvider);
                final profileId = await profileService.createProfile(nameController.text);
                ref.read(selectedEquipmentProfileIdProvider.notifier).state = profileId;
              } catch (e) {
                if (mounted) {
                  ScaffoldMessenger.of(context).showSnackBar(
                    SnackBar(
                      content: Text('Failed to create profile: $e'),
                      backgroundColor: colors.error,
                    ),
                  );
                }
              }
            },
            style: FilledButton.styleFrom(backgroundColor: colors.primary),
            child: const Text('Create'),
          ),
        ],
      ),
    );
  }

  void _showEditProfileDialog(BuildContext context, db.EquipmentProfile? profile) {
    if (profile == null) return;
    // TODO: Implement full profile editor dialog
    // For now, just show a simple name editor
    _showCreateProfileDialog(context);
  }

  void _showCreateProfileWizard(BuildContext context) {
    // TODO: Implement guided setup wizard
    // For now, just create a profile and start discovery
    _createEmptyProfile().then((_) {
      // Trigger device discovery
      ref.read(unifiedDiscoveryProvider.notifier).discoverAll();
    });
  }
}

// ============================================================================
// First-Time User Onboarding
// ============================================================================

class _FirstTimeOnboarding extends StatelessWidget {
  final NightshadeColors colors;
  final VoidCallback onStartSetup;
  final VoidCallback onManualSetup;

  const _FirstTimeOnboarding({
    required this.colors,
    required this.onStartSetup,
    required this.onManualSetup,
  });

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Container(
        constraints: const BoxConstraints(maxWidth: 480),
        padding: const EdgeInsets.all(48),
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            // Welcome icon
            Container(
              width: 80,
              height: 80,
              decoration: BoxDecoration(
                gradient: LinearGradient(
                  colors: [
                    colors.primary.withValues(alpha: 0.2),
                    colors.primary.withValues(alpha: 0.1),
                  ],
                  begin: Alignment.topLeft,
                  end: Alignment.bottomRight,
                ),
                shape: BoxShape.circle,
                border: Border.all(
                  color: colors.primary.withValues(alpha: 0.3),
                  width: 2,
                ),
              ),
              child: Icon(
                LucideIcons.moon,
                size: 36,
                color: colors.primary,
              ),
            ),

            const SizedBox(height: 32),

            Text(
              'Welcome to Nightshade',
              style: TextStyle(
                fontSize: 24,
                fontWeight: FontWeight.w700,
                color: colors.textPrimary,
                letterSpacing: -0.5,
              ),
            ),

            const SizedBox(height: 12),

            Text(
              "Let's set up your first equipment profile",
              style: TextStyle(
                fontSize: 16,
                color: colors.textSecondary,
              ),
              textAlign: TextAlign.center,
            ),

            const SizedBox(height: 40),

            // Setup steps
            Container(
              padding: const EdgeInsets.all(24),
              decoration: BoxDecoration(
                color: colors.surfaceAlt,
                borderRadius: BorderRadius.circular(16),
                border: Border.all(color: colors.border),
              ),
              child: Column(
                children: [
                  _SetupStep(
                    number: '1',
                    text: "We'll scan for connected equipment",
                    colors: colors,
                  ),
                  const SizedBox(height: 16),
                  _SetupStep(
                    number: '2',
                    text: 'Select the devices you want to use',
                    colors: colors,
                  ),
                  const SizedBox(height: 16),
                  _SetupStep(
                    number: '3',
                    text: 'Save as a profile for one-click connection',
                    colors: colors,
                  ),
                ],
              ),
            ),

            const SizedBox(height: 32),

            // Action buttons
            SizedBox(
              width: double.infinity,
              child: FilledButton.icon(
                onPressed: onStartSetup,
                icon: const Icon(LucideIcons.sparkles, size: 18),
                label: const Text('Start Setup'),
                style: FilledButton.styleFrom(
                  backgroundColor: colors.primary,
                  foregroundColor: Colors.white,
                  padding: const EdgeInsets.symmetric(vertical: 16),
                  textStyle: const TextStyle(
                    fontSize: 15,
                    fontWeight: FontWeight.w600,
                  ),
                ),
              ),
            ),

            const SizedBox(height: 12),

            TextButton(
              onPressed: onManualSetup,
              child: Text(
                "I'll do it manually",
                style: TextStyle(
                  color: colors.textSecondary,
                  fontSize: 14,
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}

class _SetupStep extends StatelessWidget {
  final String number;
  final String text;
  final NightshadeColors colors;

  const _SetupStep({
    required this.number,
    required this.text,
    required this.colors,
  });

  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        Container(
          width: 28,
          height: 28,
          decoration: BoxDecoration(
            color: colors.primary.withValues(alpha: 0.15),
            shape: BoxShape.circle,
          ),
          child: Center(
            child: Text(
              number,
              style: TextStyle(
                fontSize: 13,
                fontWeight: FontWeight.w600,
                color: colors.primary,
              ),
            ),
          ),
        ),
        const SizedBox(width: 14),
        Expanded(
          child: Text(
            text,
            style: TextStyle(
              fontSize: 14,
              color: colors.textPrimary,
            ),
          ),
        ),
      ],
    );
  }
}

// ============================================================================
// Connected Devices Tab (New)
// ============================================================================

class _ConnectedDevicesTab extends ConsumerWidget {
  const _ConnectedDevicesTab();

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final colors = Theme.of(context).extension<NightshadeColors>()!;

    // Watch device states
    final cameraState = ref.watch(cameraStateProvider);
    final mountState = ref.watch(mountStateProvider);
    final focuserState = ref.watch(focuserStateProvider);
    final filterWheelState = ref.watch(filterWheelStateProvider);
    final guiderState = ref.watch(guiderStateProvider);
    final rotatorState = ref.watch(rotatorStateProvider);

    final connectedDevices = <Widget>[];

    // Add connected devices
    if (cameraState.connectionState == DeviceConnectionState.connected) {
      connectedDevices.add(_ConnectedDeviceCard(
        icon: LucideIcons.camera,
        title: 'Camera',
        name: cameraState.deviceName ?? 'Unknown Camera',
        telemetry: [
          if (cameraState.temperature != null)
            'Temperature: ${cameraState.temperature!.toStringAsFixed(1)}°C',
          if (cameraState.coolerPower != null)
            'Cooler: ${cameraState.coolerPower!.toStringAsFixed(0)}%',
          'Status: ${cameraState.isExposing ? "Exposing" : "Idle"}',
        ],
        accentColor: colors.primary,
        colors: colors,
        onSettings: () {},
        onDisconnect: () => ref.read(deviceServiceProvider).disconnectCamera(),
      ));
    }

    if (mountState.connectionState == DeviceConnectionState.connected) {
      connectedDevices.add(_ConnectedDeviceCard(
        icon: LucideIcons.compass,
        title: 'Mount',
        name: mountState.deviceName ?? 'Unknown Mount',
        telemetry: [
          'RA: ${mountState.ra?.toStringAsFixed(2) ?? "---"}  Dec: ${mountState.dec?.toStringAsFixed(2) ?? "---"}',
          'Tracking: ${mountState.isTracking ? "On" : "Off"}',
          'Status: ${mountState.isSlewing ? "Slewing" : "Ready"}',
        ],
        accentColor: colors.warning,
        colors: colors,
        quickActions: [
          _QuickAction(label: 'Park', onTap: () {}),
        ],
        onSettings: () {},
        onDisconnect: () => ref.read(deviceServiceProvider).disconnectMount(),
      ));
    }

    if (focuserState.connectionState == DeviceConnectionState.connected) {
      connectedDevices.add(_ConnectedDeviceCard(
        icon: LucideIcons.focus,
        title: 'Focuser',
        name: focuserState.deviceName ?? 'Unknown Focuser',
        telemetry: [
          'Position: ${focuserState.position ?? "---"}',
          if (focuserState.temperature != null)
            'Temperature: ${focuserState.temperature!.toStringAsFixed(1)}°C',
        ],
        accentColor: colors.success,
        colors: colors,
        onSettings: () {},
        onDisconnect: () => ref.read(deviceServiceProvider).disconnectFocuser(),
      ));
    }

    if (filterWheelState.connectionState == DeviceConnectionState.connected) {
      connectedDevices.add(_ConnectedDeviceCard(
        icon: LucideIcons.circle,
        title: 'Filter Wheel',
        name: filterWheelState.deviceName ?? 'Unknown Filter Wheel',
        telemetry: [
          'Filter: ${filterWheelState.currentFilterName ?? "Unknown"}',
          'Position: ${filterWheelState.currentPosition ?? "---"}',
        ],
        accentColor: colors.warning,
        colors: colors,
        onSettings: () {},
        onDisconnect: () => ref.read(deviceServiceProvider).disconnectFilterWheel(),
      ));
    }

    if (guiderState.connectionState == DeviceConnectionState.connected) {
      connectedDevices.add(_ConnectedDeviceCard(
        icon: LucideIcons.crosshair,
        title: 'Guider',
        name: guiderState.deviceName ?? 'Unknown Guider',
        telemetry: [
          if (guiderState.rmsTotal != null)
            'RMS: ${guiderState.rmsTotal!.toStringAsFixed(2)}"',
          'Status: ${guiderState.isGuiding ? "Guiding" : "Idle"}',
        ],
        accentColor: colors.info,
        colors: colors,
        onSettings: () {},
        onDisconnect: () => ref.read(deviceServiceProvider).disconnectGuider(),
      ));
    }

    if (rotatorState.connectionState == DeviceConnectionState.connected) {
      connectedDevices.add(_ConnectedDeviceCard(
        icon: LucideIcons.rotateCw,
        title: 'Rotator',
        name: rotatorState.deviceName ?? 'Unknown Rotator',
        telemetry: [
          'Position: ${rotatorState.position?.toStringAsFixed(1) ?? "---"}°',
          'Status: ${rotatorState.isMoving ? "Moving" : "Ready"}',
        ],
        accentColor: colors.accent,
        colors: colors,
        onSettings: () {},
        onDisconnect: () => ref.read(rotatorStateProvider.notifier).disconnect(),
      ));
    }

    if (connectedDevices.isEmpty) {
      return Center(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            Icon(
              LucideIcons.unplug,
              size: 48,
              color: colors.textMuted,
            ),
            const SizedBox(height: 16),
            Text(
              'No devices connected',
              style: TextStyle(
                fontSize: 16,
                fontWeight: FontWeight.w600,
                color: colors.textPrimary,
              ),
            ),
            const SizedBox(height: 8),
            Text(
              'Select a profile and click "Connect All" to get started',
              style: TextStyle(
                fontSize: 13,
                color: colors.textSecondary,
              ),
            ),
          ],
        ),
      );
    }

    return SingleChildScrollView(
      padding: const EdgeInsets.all(20),
      child: Wrap(
        spacing: 16,
        runSpacing: 16,
        children: connectedDevices,
      ),
    );
  }
}

class _QuickAction {
  final String label;
  final VoidCallback onTap;

  _QuickAction({required this.label, required this.onTap});
}

class _ConnectedDeviceCard extends StatelessWidget {
  final IconData icon;
  final String title;
  final String name;
  final List<String> telemetry;
  final Color accentColor;
  final NightshadeColors colors;
  final List<_QuickAction>? quickActions;
  final VoidCallback onSettings;
  final VoidCallback onDisconnect;

  const _ConnectedDeviceCard({
    required this.icon,
    required this.title,
    required this.name,
    required this.telemetry,
    required this.accentColor,
    required this.colors,
    this.quickActions,
    required this.onSettings,
    required this.onDisconnect,
  });

  @override
  Widget build(BuildContext context) {
    return Container(
      width: 340,
      padding: const EdgeInsets.all(20),
      decoration: BoxDecoration(
        color: colors.surface,
        borderRadius: BorderRadius.circular(16),
        border: Border.all(color: colors.border),
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          // Header
          Row(
            children: [
              Container(
                width: 40,
                height: 40,
                decoration: BoxDecoration(
                  gradient: LinearGradient(
                    colors: [
                      accentColor.withValues(alpha: 0.2),
                      accentColor.withValues(alpha: 0.1),
                    ],
                  ),
                  borderRadius: BorderRadius.circular(10),
                ),
                child: Icon(icon, size: 18, color: colors.success),
              ),
              const SizedBox(width: 12),
              Expanded(
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Text(
                      title,
                      style: TextStyle(
                        fontSize: 11,
                        fontWeight: FontWeight.w500,
                        color: colors.textMuted,
                      ),
                    ),
                    Text(
                      name,
                      style: TextStyle(
                        fontSize: 14,
                        fontWeight: FontWeight.w600,
                        color: colors.textPrimary,
                      ),
                      overflow: TextOverflow.ellipsis,
                    ),
                  ],
                ),
              ),
              Container(
                padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 4),
                decoration: BoxDecoration(
                  color: colors.success.withValues(alpha: 0.15),
                  borderRadius: BorderRadius.circular(12),
                ),
                child: Row(
                  mainAxisSize: MainAxisSize.min,
                  children: [
                    Container(
                      width: 6,
                      height: 6,
                      decoration: BoxDecoration(
                        shape: BoxShape.circle,
                        color: colors.success,
                      ),
                    ),
                    const SizedBox(width: 6),
                    Text(
                      'Connected',
                      style: TextStyle(
                        fontSize: 10,
                        fontWeight: FontWeight.w500,
                        color: colors.success,
                      ),
                    ),
                  ],
                ),
              ),
            ],
          ),

          const SizedBox(height: 16),

          // Telemetry
          Container(
            padding: const EdgeInsets.all(12),
            decoration: BoxDecoration(
              color: colors.background,
              borderRadius: BorderRadius.circular(10),
            ),
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: telemetry.map((line) => Padding(
                padding: const EdgeInsets.only(bottom: 4),
                child: Text(
                  line,
                  style: TextStyle(
                    fontSize: 12,
                    color: colors.textSecondary,
                    fontFamily: 'monospace',
                  ),
                ),
              )).toList(),
            ),
          ),

          const SizedBox(height: 12),

          // Actions
          Row(
            children: [
              if (quickActions != null)
                ...quickActions!.map((action) => Padding(
                  padding: const EdgeInsets.only(right: 8),
                  child: OutlinedButton(
                    onPressed: action.onTap,
                    style: OutlinedButton.styleFrom(
                      foregroundColor: colors.textSecondary,
                      side: BorderSide(color: colors.border),
                      padding: const EdgeInsets.symmetric(
                        horizontal: 12,
                        vertical: 8,
                      ),
                    ),
                    child: Text(action.label, style: const TextStyle(fontSize: 12)),
                  ),
                )),
              const Spacer(),
              IconButton(
                onPressed: onSettings,
                icon: const Icon(LucideIcons.settings2, size: 16),
                tooltip: 'Settings',
                style: IconButton.styleFrom(
                  foregroundColor: colors.textMuted,
                ),
              ),
              IconButton(
                onPressed: onDisconnect,
                icon: const Icon(LucideIcons.unplug, size: 16),
                tooltip: 'Disconnect',
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
// Sub-tab components
// ============================================================================

class _SubTabData {
  final IconData icon;
  final String label;

  const _SubTabData({required this.icon, required this.label});
}

class _ConnectionBadge extends ConsumerWidget {
  final NightshadeColors colors;

  const _ConnectionBadge({required this.colors});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final cameraState = ref.watch(cameraStateProvider);
    final mountState = ref.watch(mountStateProvider);
    final focuserState = ref.watch(focuserStateProvider);
    final filterWheelState = ref.watch(filterWheelStateProvider);
    final guiderState = ref.watch(guiderStateProvider);

    final connectionStates = [
      cameraState.connectionState,
      mountState.connectionState,
      focuserState.connectionState,
      filterWheelState.connectionState,
      guiderState.connectionState,
    ];
    final connectedCount = connectionStates
        .where((state) => state == DeviceConnectionState.connected)
        .length;
    final totalDevices = connectionStates.length;
    final allConnected = connectedCount == totalDevices && totalDevices > 0;
    final someConnected = connectedCount > 0;

    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 4),
      decoration: BoxDecoration(
        color: allConnected
            ? colors.success.withValues(alpha: 0.15)
            : someConnected
                ? colors.warning.withValues(alpha: 0.15)
                : colors.surfaceAlt,
        borderRadius: BorderRadius.circular(20),
        border: Border.all(
          color: allConnected
              ? colors.success.withValues(alpha: 0.3)
              : someConnected
                  ? colors.warning.withValues(alpha: 0.3)
                  : colors.border,
        ),
      ),
      child: Row(
        mainAxisSize: MainAxisSize.min,
        children: [
          Container(
            width: 6,
            height: 6,
            decoration: BoxDecoration(
              shape: BoxShape.circle,
              color: allConnected
                  ? colors.success
                  : someConnected
                      ? colors.warning
                      : colors.textMuted,
            ),
          ),
          const SizedBox(width: 6),
          Text(
            '$connectedCount / $totalDevices',
            style: TextStyle(
              fontSize: 11,
              fontWeight: FontWeight.w600,
              color: allConnected
                  ? colors.success
                  : someConnected
                      ? colors.warning
                      : colors.textSecondary,
            ),
          ),
        ],
      ),
    );
  }
}

class _SubTabBar extends StatelessWidget {
  final List<_SubTabData> tabs;
  final int currentIndex;
  final ValueChanged<int> onTabSelected;
  final NightshadeColors colors;

  const _SubTabBar({
    required this.tabs,
    required this.currentIndex,
    required this.onTabSelected,
    required this.colors,
  });

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.all(4),
      decoration: BoxDecoration(
        color: colors.surfaceAlt,
        borderRadius: BorderRadius.circular(12),
      ),
      child: Row(
        mainAxisSize: MainAxisSize.min,
        children: tabs.asMap().entries.map((entry) {
          final index = entry.key;
          final tab = entry.value;
          final isSelected = index == currentIndex;

          return _SubTabButton(
            icon: tab.icon,
            label: tab.label,
            isSelected: isSelected,
            onTap: () => onTabSelected(index),
            colors: colors,
          );
        }).toList(),
      ),
    );
  }
}

class _SubTabButton extends StatefulWidget {
  final IconData icon;
  final String label;
  final bool isSelected;
  final VoidCallback onTap;
  final NightshadeColors colors;

  const _SubTabButton({
    required this.icon,
    required this.label,
    required this.isSelected,
    required this.onTap,
    required this.colors,
  });

  @override
  State<_SubTabButton> createState() => _SubTabButtonState();
}

class _SubTabButtonState extends State<_SubTabButton> {
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
          decoration: BoxDecoration(
            color: widget.isSelected
                ? widget.colors.surface
                : _isHovered
                    ? widget.colors.surface.withValues(alpha: 0.5)
                    : Colors.transparent,
            borderRadius: BorderRadius.circular(8),
            boxShadow: widget.isSelected
                ? [
                    BoxShadow(
                      color: Colors.black.withValues(alpha: 0.1),
                      blurRadius: 4,
                      offset: const Offset(0, 1),
                    ),
                  ]
                : null,
          ),
          child: Material(
            type: MaterialType.transparency,
            child: InkWell(
              onTap: widget.onTap,
              borderRadius: BorderRadius.circular(8),
              hoverColor: Colors.transparent,
              highlightColor: widget.colors.primary.withValues(alpha: 0.1),
              splashColor: widget.colors.primary.withValues(alpha: 0.1),
              child: Padding(
                padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 10),
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
                    const SizedBox(width: 8),
                    Text(
                      widget.label,
                      style: TextStyle(
                        fontSize: 12,
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
