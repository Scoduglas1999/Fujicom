import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:nightshade_ui/nightshade_ui.dart';
import 'package:nightshade_core/nightshade_core.dart';

class EquipmentSettingsTab extends ConsumerWidget {
  const EquipmentSettingsTab({super.key});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final colors = Theme.of(context).extension<NightshadeColors>()!;
    final settingsAsync = ref.watch(appSettingsProvider);

    return settingsAsync.when(
      loading: () => const Center(child: CircularProgressIndicator()),
      error: (e, _) => Center(child: Text('Error loading settings: $e')),
      data: (settings) => SingleChildScrollView(
        padding: const EdgeInsets.all(24),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Text(
              'Equipment Settings',
              style: TextStyle(
                fontSize: 20,
                fontWeight: FontWeight.w600,
                color: colors.textPrimary,
              ),
            ),
            const SizedBox(height: 24),
            ResponsiveCardGrid(
              children: [
                _CameraSettingsCard(settings: settings),
                _MountSettingsCard(settings: settings),
                _FocuserSettingsCard(settings: settings),
                _GuiderSettingsCard(settings: settings),
              ],
            ),
          ],
        ),
      ),
    );
  }
}

class _CameraSettingsCard extends ConsumerWidget {
  final AppSettings settings;

  const _CameraSettingsCard({required this.settings});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final colors = Theme.of(context).extension<NightshadeColors>()!;
    final notifier = ref.read(appSettingsProvider.notifier);

    return NightshadeCard(
      child: Padding(
        padding: const EdgeInsets.all(16),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Text(
              'Camera Settings',
              style: TextStyle(
                fontSize: 14,
                fontWeight: FontWeight.w600,
                color: colors.textPrimary,
              ),
            ),
            const SizedBox(height: 16),
            _SettingRow(
              label: 'Cooling Behavior',
              child: NightshadeDropdown(
                value: settings.coolingBehavior,
                items: const ['On Connect', 'Manual', 'Never'],
                onChanged: (value) {
                  if (value != null) notifier.setCoolingBehavior(value);
                },
              ),
            ),
            const SizedBox(height: 12),
            _SettingRow(
              label: 'Default Gain',
              child: SizedBox(
                width: 100,
                child: NightshadeTextField(
                  initialValue: settings.defaultGain.toString(),
                  onChanged: (value) {
                    final parsed = int.tryParse(value);
                    if (parsed != null) notifier.setDefaultGain(parsed);
                  },
                ),
              ),
            ),
            const SizedBox(height: 12),
            _SettingRow(
              label: 'Default Offset',
              child: SizedBox(
                width: 100,
                child: NightshadeTextField(
                  initialValue: settings.defaultOffset.toString(),
                  onChanged: (value) {
                    final parsed = int.tryParse(value);
                    if (parsed != null) notifier.setDefaultOffset(parsed);
                  },
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}

class _MountSettingsCard extends ConsumerWidget {
  final AppSettings settings;

  const _MountSettingsCard({required this.settings});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final colors = Theme.of(context).extension<NightshadeColors>()!;
    final notifier = ref.read(appSettingsProvider.notifier);

    return NightshadeCard(
      child: Padding(
        padding: const EdgeInsets.all(16),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Text(
              'Mount Settings',
              style: TextStyle(
                fontSize: 14,
                fontWeight: FontWeight.w600,
                color: colors.textPrimary,
              ),
            ),
            const SizedBox(height: 16),
            _SettingRow(
              label: 'Meridian Flip',
              child: NightshadeSwitch(
                value: settings.enableMeridianFlip,
                onChanged: (value) => notifier.setEnableMeridianFlip(value),
              ),
            ),
            const SizedBox(height: 12),
            _SettingRow(
              label: 'Flip Offset (min)',
              child: SizedBox(
                width: 100,
                child: NightshadeTextField(
                  initialValue: settings.meridianFlipMinutes.toString(),
                  onChanged: (value) {
                    final parsed = int.tryParse(value);
                    if (parsed != null) notifier.setMeridianFlipMinutes(parsed);
                  },
                ),
              ),
            ),
            const SizedBox(height: 12),
            _SettingRow(
              label: 'Park on Unsafe',
              child: NightshadeSwitch(
                value: settings.parkOnUnsafeWeather,
                onChanged: (value) => notifier.setParkOnUnsafeWeather(value),
              ),
            ),
          ],
        ),
      ),
    );
  }
}

class _FocuserSettingsCard extends ConsumerWidget {
  final AppSettings settings;

  const _FocuserSettingsCard({required this.settings});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final colors = Theme.of(context).extension<NightshadeColors>()!;
    final notifier = ref.read(appSettingsProvider.notifier);

    return NightshadeCard(
      child: Padding(
        padding: const EdgeInsets.all(16),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Text(
              'Focuser Settings',
              style: TextStyle(
                fontSize: 14,
                fontWeight: FontWeight.w600,
                color: colors.textPrimary,
              ),
            ),
            const SizedBox(height: 16),
            _SettingRow(
              label: 'Temp Compensation',
              child: NightshadeSwitch(
                value: settings.tempCompensation,
                onChanged: (value) => notifier.setTempCompensation(value),
              ),
            ),
            const SizedBox(height: 12),
            _SettingRow(
              label: 'Temp Coefficient',
              child: SizedBox(
                width: 100,
                child: NightshadeTextField(
                  initialValue: settings.tempCoefficient.toString(),
                  onChanged: (value) {
                    final parsed = double.tryParse(value);
                    if (parsed != null) notifier.setTempCoefficient(parsed);
                  },
                ),
              ),
            ),
            const SizedBox(height: 12),
            _SettingRow(
              label: 'Backlash Comp',
              child: SizedBox(
                width: 100,
                child: NightshadeTextField(
                  initialValue: settings.backlashCompensation.toString(),
                  onChanged: (value) {
                    final parsed = int.tryParse(value);
                    if (parsed != null) notifier.setBacklashCompensation(parsed);
                  },
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}

class _GuiderSettingsCard extends ConsumerWidget {
  final AppSettings settings;

  const _GuiderSettingsCard({required this.settings});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final colors = Theme.of(context).extension<NightshadeColors>()!;
    final notifier = ref.read(appSettingsProvider.notifier);

    return NightshadeCard(
      child: Padding(
        padding: const EdgeInsets.all(16),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Text(
              'Guider Settings',
              style: TextStyle(
                fontSize: 14,
                fontWeight: FontWeight.w600,
                color: colors.textPrimary,
              ),
            ),
            const SizedBox(height: 16),
            _SettingRow(
              label: 'Dither Scale',
              child: NightshadeDropdown(
                value: settings.ditherScale,
                items: const ['Small', 'Medium', 'Large'],
                onChanged: (value) {
                  if (value != null) notifier.setDitherScale(value);
                },
              ),
            ),
            const SizedBox(height: 12),
            _SettingRow(
              label: 'Settle Threshold',
              child: SizedBox(
                width: 100,
                child: NightshadeTextField(
                  initialValue: settings.settleThreshold.toString(),
                  suffix: '"',
                  onChanged: (value) {
                    final parsed = double.tryParse(value);
                    if (parsed != null) notifier.setSettleThreshold(parsed);
                  },
                ),
              ),
            ),
            const SizedBox(height: 12),
            _SettingRow(
              label: 'Settle Timeout',
              child: SizedBox(
                width: 100,
                child: NightshadeTextField(
                  initialValue: settings.settleTimeout.toString(),
                  suffix: 's',
                  onChanged: (value) {
                    final parsed = int.tryParse(value);
                    if (parsed != null) notifier.setSettleTimeout(parsed);
                  },
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}

class _SettingRow extends StatelessWidget {
  final String label;
  final Widget child;

  const _SettingRow({required this.label, required this.child});

  @override
  Widget build(BuildContext context) {
    final colors = Theme.of(context).extension<NightshadeColors>()!;

    return Row(
      mainAxisAlignment: MainAxisAlignment.spaceBetween,
      children: [
        Text(
          label,
          style: TextStyle(
            fontSize: 13,
            color: colors.textSecondary,
          ),
        ),
        child,
      ],
    );
  }
}
