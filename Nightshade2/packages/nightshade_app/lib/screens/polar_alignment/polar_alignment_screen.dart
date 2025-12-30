import 'dart:async';
import 'dart:typed_data';
import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';
import 'package:lucide_icons/lucide_icons.dart';
import 'package:nightshade_core/nightshade_core.dart';
import 'package:nightshade_ui/nightshade_ui.dart';

/// Phase of polar alignment process
enum PolarAlignPhase {
  setup,
  measuring,
  adjusting,
  complete,
  error,
}

/// Provider for polar alignment phase
final polarAlignPhaseProvider = StateProvider<PolarAlignPhase>((ref) {
  return PolarAlignPhase.setup;
});

/// Provider for current measurement point (1-3)
final polarAlignPointProvider = StateProvider<int>((ref) => 0);

/// Provider for polar alignment error values
final polarAlignErrorProvider = StateProvider<({
  double azimuth,
  double altitude,
  double total,
  double currentRa,
  double currentDec,
  double targetRa,
  double targetDec,
})?>((ref) => null);

/// Provider for polar alignment status message
final polarAlignStatusProvider = StateProvider<String>((ref) {
  return 'Configure settings and click Start';
});

/// Provider for last captured image during alignment
final polarAlignImageProvider = StateProvider<List<int>?>((ref) => null);

/// Provider for plate solve coordinates
final polarAlignSolveProvider = StateProvider<({double ra, double dec})?>((ref) => null);

class PolarAlignmentScreen extends ConsumerStatefulWidget {
  const PolarAlignmentScreen({super.key});

  @override
  ConsumerState<PolarAlignmentScreen> createState() => _PolarAlignmentScreenState();
}

class _PolarAlignmentScreenState extends ConsumerState<PolarAlignmentScreen>
    with TickerProviderStateMixin {
  // Essential Settings
  double _exposureTime = 5.0;
  bool _isNorthernHemisphere = true;

  // Common Settings
  double _stepSize = 15.0; // Changed from 30° per UX design
  int _binning = 2;
  bool _rotateEast = true;

  // Advanced Settings
  bool _manualRotation = false;
  double _autoCompleteThreshold = 30.0; // arcseconds
  int? _gain;
  int? _offset;
  double _solveTimeout = 30.0;
  bool _startFromCurrent = true;

  // Panel expansion state
  bool _showCommonSettings = false;
  bool _showAdvancedSettings = false;

  StreamSubscription? _eventSubscription;
  late AnimationController _pulseController;

  @override
  void initState() {
    super.initState();
    _pulseController = AnimationController(
      duration: const Duration(milliseconds: 1500),
      vsync: this,
    )..repeat(reverse: true);
  }

  @override
  void dispose() {
    _eventSubscription?.cancel();
    _pulseController.dispose();
    // Reset providers on close
    Future.microtask(() {
      ref.invalidate(polarAlignPhaseProvider);
      ref.invalidate(polarAlignPointProvider);
      ref.invalidate(polarAlignErrorProvider);
      ref.invalidate(polarAlignStatusProvider);
      ref.invalidate(polarAlignImageProvider);
      ref.invalidate(polarAlignSolveProvider);
    });
    super.dispose();
  }

  Future<void> _startAlignment() async {
    ref.read(polarAlignPhaseProvider.notifier).state = PolarAlignPhase.measuring;
    ref.read(polarAlignPointProvider.notifier).state = 1;
    ref.read(polarAlignStatusProvider.notifier).state = 'Starting polar alignment...';

    try {
      final backend = ref.read(backendProvider);

      // Subscribe to polar alignment events
      _eventSubscription?.cancel();
      _eventSubscription = backend.polarAlignmentEvents.listen((event) {
        _handlePolarAlignEvent(event);
      });

      // Start the polar alignment process
      await backend.startPolarAlignment(
        exposureTime: _exposureTime,
        stepSize: _stepSize,
        binning: _binning,
        isNorth: _isNorthernHemisphere,
        manualRotation: _manualRotation,
        rotateEast: _rotateEast,
      );
    } catch (e) {
      ref.read(polarAlignPhaseProvider.notifier).state = PolarAlignPhase.error;
      ref.read(polarAlignStatusProvider.notifier).state = 'Error: $e';
    }
  }

  void _handlePolarAlignEvent(Map<String, dynamic> event) {
    if (event.containsKey('status')) {
      ref.read(polarAlignStatusProvider.notifier).state = event['status'] as String;
    }

    if (event.containsKey('point')) {
      ref.read(polarAlignPointProvider.notifier).state = event['point'] as int;
    }

    if (event.containsKey('phase')) {
      final phase = event['phase'] as String;
      switch (phase) {
        case 'measuring':
          ref.read(polarAlignPhaseProvider.notifier).state = PolarAlignPhase.measuring;
        case 'adjusting':
          ref.read(polarAlignPhaseProvider.notifier).state = PolarAlignPhase.adjusting;
        case 'complete':
          ref.read(polarAlignPhaseProvider.notifier).state = PolarAlignPhase.complete;
        case 'error':
          ref.read(polarAlignPhaseProvider.notifier).state = PolarAlignPhase.error;
      }
    }

    if (event.containsKey('azimuth_error')) {
      ref.read(polarAlignErrorProvider.notifier).state = (
        azimuth: (event['azimuth_error'] as num).toDouble(),
        altitude: (event['altitude_error'] as num).toDouble(),
        total: (event['total_error'] as num).toDouble(),
        currentRa: (event['current_ra'] as num?)?.toDouble() ?? 0.0,
        currentDec: (event['current_dec'] as num?)?.toDouble() ?? 0.0,
        targetRa: (event['target_ra'] as num?)?.toDouble() ?? 0.0,
        targetDec: (event['target_dec'] as num?)?.toDouble() ?? 0.0,
      );
    }

    if (event.containsKey('image_data')) {
      ref.read(polarAlignImageProvider.notifier).state =
          (event['image_data'] as List).cast<int>();
    }
  }

  Future<void> _stopAlignment() async {
    try {
      final backend = ref.read(backendProvider);
      await backend.stopPolarAlignment();
    } finally {
      _eventSubscription?.cancel();
      ref.read(polarAlignPhaseProvider.notifier).state = PolarAlignPhase.setup;
      ref.read(polarAlignPointProvider.notifier).state = 0;
      ref.read(polarAlignStatusProvider.notifier).state = 'Stopped - Configure and restart';
      ref.read(polarAlignErrorProvider.notifier).state = null;
      ref.read(polarAlignImageProvider.notifier).state = null;
      ref.read(polarAlignSolveProvider.notifier).state = null;
    }
  }

  @override
  Widget build(BuildContext context) {
    final colors = Theme.of(context).extension<NightshadeColors>()!;
    final phase = ref.watch(polarAlignPhaseProvider);
    final point = ref.watch(polarAlignPointProvider);
    final error = ref.watch(polarAlignErrorProvider);
    final status = ref.watch(polarAlignStatusProvider);

    final isRunning = phase == PolarAlignPhase.measuring || phase == PolarAlignPhase.adjusting;

    return Scaffold(
      backgroundColor: colors.background,
      body: Column(
        children: [
          // Header bar
          _buildHeader(colors, isRunning),

          // Main content
          Expanded(
            child: Row(
              children: [
                // Left panel - Equipment status & config
                SizedBox(
                  width: 320,
                  child: _buildLeftPanel(colors, phase, isRunning),
                ),

                // Divider
                Container(width: 1, color: colors.border),

                // Center panel - Progress & Instructions
                Expanded(
                  flex: 2,
                  child: _buildCenterPanel(colors, phase, point, error, status),
                ),

                // Divider
                Container(width: 1, color: colors.border),

                // Right panel - Error visualization
                SizedBox(
                  width: 400,
                  child: _buildRightPanel(colors, phase, error),
                ),
              ],
            ),
          ),

          // Footer with actions
          _buildFooter(colors, phase, status, isRunning),
        ],
      ),
    );
  }

  Widget _buildHeader(NightshadeColors colors, bool isRunning) {
    return Container(
      height: 56,
      padding: const EdgeInsets.symmetric(horizontal: 16),
      decoration: BoxDecoration(
        color: colors.surface,
        border: Border(bottom: BorderSide(color: colors.border)),
      ),
      child: Row(
        children: [
          // Back button
          IconButton(
            icon: Icon(LucideIcons.arrowLeft, color: colors.textPrimary),
            onPressed: isRunning
                ? null
                : () {
                    if (context.canPop()) {
                      context.pop();
                    } else {
                      context.go('/imaging');
                    }
                  },
            tooltip: isRunning ? 'Stop alignment first' : 'Back',
          ),
          const SizedBox(width: 12),

          // Title
          Icon(LucideIcons.compass, color: colors.primary, size: 24),
          const SizedBox(width: 12),
          Column(
            mainAxisAlignment: MainAxisAlignment.center,
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Text(
                'Polar Alignment',
                style: TextStyle(
                  fontSize: 16,
                  fontWeight: FontWeight.w600,
                  color: colors.textPrimary,
                ),
              ),
              Text(
                'Three-point method',
                style: TextStyle(
                  fontSize: 11,
                  color: colors.textMuted,
                ),
              ),
            ],
          ),

          const Spacer(),

          // Equipment status indicators
          _buildEquipmentIndicators(colors),
        ],
      ),
    );
  }

  Widget _buildEquipmentIndicators(NightshadeColors colors) {
    final cameraState = ref.watch(cameraStateProvider);
    final mountState = ref.watch(mountStateProvider);

    return Row(
      children: [
        _StatusChip(
          icon: LucideIcons.camera,
          label: 'Camera',
          isConnected: cameraState.connectionState == DeviceConnectionState.connected,
          colors: colors,
        ),
        const SizedBox(width: 8),
        _StatusChip(
          icon: LucideIcons.move,
          label: 'Mount',
          isConnected: mountState.connectionState == DeviceConnectionState.connected,
          colors: colors,
        ),
      ],
    );
  }

  Widget _buildLeftPanel(NightshadeColors colors, PolarAlignPhase phase, bool isRunning) {
    return Container(
      color: colors.surface,
      child: Column(
        children: [
          // Configuration section
          Expanded(
            child: SingleChildScrollView(
              padding: const EdgeInsets.all(16),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  // Essential settings - always visible
                  _buildSectionHeader(colors, 'Essential', LucideIcons.settings),
                  const SizedBox(height: 12),
                  _buildEssentialSettings(colors, isRunning),

                  const SizedBox(height: 16),

                  // Common settings - collapsible
                  _buildCommonSettings(colors, isRunning),

                  const SizedBox(height: 8),

                  // Advanced settings - collapsible
                  _buildAdvancedSettings(colors, isRunning),

                  if (phase == PolarAlignPhase.adjusting) ...[
                    const SizedBox(height: 24),
                    _buildSectionHeader(colors, 'Adjustment Tips', LucideIcons.lightbulb),
                    const SizedBox(height: 12),
                    _buildAdjustmentTips(colors),
                  ],
                ],
              ),
            ),
          ),
        ],
      ),
    );
  }

  Widget _buildSectionHeader(NightshadeColors colors, String title, IconData icon) {
    return Row(
      children: [
        Icon(icon, size: 14, color: colors.textMuted),
        const SizedBox(width: 8),
        Text(
          title,
          style: TextStyle(
            fontSize: 12,
            fontWeight: FontWeight.w600,
            color: colors.textPrimary,
          ),
        ),
      ],
    );
  }

  Widget _buildEssentialSettings(NightshadeColors colors, bool isRunning) {
    return Column(
      children: [
        // Hemisphere
        _SettingRow(
          label: 'Hemisphere',
          tooltip: 'Northern or Southern hemisphere determines celestial pole position',
          colors: colors,
          child: SegmentedButton<bool>(
            segments: const [
              ButtonSegment(value: true, label: Text('North')),
              ButtonSegment(value: false, label: Text('South')),
            ],
            selected: {_isNorthernHemisphere},
            onSelectionChanged: isRunning
                ? null
                : (v) => setState(() => _isNorthernHemisphere = v.first),
            style: ButtonStyle(
              visualDensity: VisualDensity.compact,
              textStyle: WidgetStateProperty.all(const TextStyle(fontSize: 11)),
            ),
          ),
        ),
        const SizedBox(height: 12),

        // Exposure time
        _SettingRow(
          label: 'Exposure',
          tooltip: 'Longer exposures capture more stars but slow down iterations',
          colors: colors,
          child: Row(
            children: [
              Expanded(
                child: Slider(
                  value: _exposureTime,
                  min: 1,
                  max: 30,
                  divisions: 29,
                  onChanged: isRunning ? null : (v) => setState(() => _exposureTime = v),
                ),
              ),
              SizedBox(
                width: 40,
                child: Text(
                  '${_exposureTime.toInt()}s',
                  style: TextStyle(fontSize: 11, color: colors.textPrimary),
                ),
              ),
            ],
          ),
        ),
      ],
    );
  }

  Widget _buildCommonSettings(NightshadeColors colors, bool isRunning) {
    return Theme(
      data: Theme.of(context).copyWith(dividerColor: Colors.transparent),
      child: ExpansionTile(
        tilePadding: EdgeInsets.zero,
        childrenPadding: const EdgeInsets.only(bottom: 8),
        initiallyExpanded: _showCommonSettings,
        onExpansionChanged: (v) => setState(() => _showCommonSettings = v),
        title: Row(
          children: [
            Icon(LucideIcons.sliders, size: 14, color: colors.textMuted),
            const SizedBox(width: 8),
            Text(
              'Common',
              style: TextStyle(
                fontSize: 12,
                fontWeight: FontWeight.w600,
                color: colors.textPrimary,
              ),
            ),
          ],
        ),
        children: [
          // Binning
          _SettingRow(
            label: 'Binning',
            tooltip: 'Higher binning = faster plate solves, lower resolution',
            colors: colors,
            child: SegmentedButton<int>(
              segments: const [
                ButtonSegment(value: 1, label: Text('1x1')),
                ButtonSegment(value: 2, label: Text('2x2')),
                ButtonSegment(value: 4, label: Text('4x4')),
              ],
              selected: {_binning},
              onSelectionChanged: isRunning
                  ? null
                  : (v) => setState(() => _binning = v.first),
              style: ButtonStyle(
                visualDensity: VisualDensity.compact,
                textStyle: WidgetStateProperty.all(const TextStyle(fontSize: 11)),
              ),
            ),
          ),
          const SizedBox(height: 12),

          // Step size
          _SettingRow(
            label: 'Step Size',
            tooltip: 'Distance between measurement points. Larger = more accurate but may hit mount limits',
            colors: colors,
            child: Row(
              children: [
                Expanded(
                  child: Slider(
                    value: _stepSize,
                    min: 10,
                    max: 45,
                    divisions: 7,
                    onChanged: isRunning ? null : (v) => setState(() => _stepSize = v),
                  ),
                ),
                SizedBox(
                  width: 40,
                  child: Text(
                    '${_stepSize.toInt()}°',
                    style: TextStyle(fontSize: 11, color: colors.textPrimary),
                  ),
                ),
              ],
            ),
          ),
          const SizedBox(height: 12),

          // Direction
          _SettingRow(
            label: 'Direction',
            tooltip: 'Which way to rotate for measurements. Use West if near Eastern meridian limit',
            colors: colors,
            child: SegmentedButton<bool>(
              segments: const [
                ButtonSegment(value: true, label: Text('East')),
                ButtonSegment(value: false, label: Text('West')),
              ],
              selected: {_rotateEast},
              onSelectionChanged: isRunning
                  ? null
                  : (v) => setState(() => _rotateEast = v.first),
              style: ButtonStyle(
                visualDensity: VisualDensity.compact,
                textStyle: WidgetStateProperty.all(const TextStyle(fontSize: 11)),
              ),
            ),
          ),
        ],
      ),
    );
  }

  Widget _buildAdvancedSettings(NightshadeColors colors, bool isRunning) {
    return Theme(
      data: Theme.of(context).copyWith(dividerColor: Colors.transparent),
      child: ExpansionTile(
        tilePadding: EdgeInsets.zero,
        childrenPadding: const EdgeInsets.only(bottom: 8),
        initiallyExpanded: _showAdvancedSettings,
        onExpansionChanged: (v) => setState(() => _showAdvancedSettings = v),
        title: Row(
          children: [
            Icon(LucideIcons.settings2, size: 14, color: colors.textMuted),
            const SizedBox(width: 8),
            Text(
              'Advanced',
              style: TextStyle(
                fontSize: 12,
                fontWeight: FontWeight.w600,
                color: colors.textPrimary,
              ),
            ),
          ],
        ),
        children: [
          // Manual rotation toggle
          _SettingRow(
            label: 'Manual Rotation',
            tooltip: 'Enable for star trackers without GoTo capability',
            colors: colors,
            child: Switch(
              value: _manualRotation,
              onChanged: isRunning ? null : (v) => setState(() => _manualRotation = v),
            ),
          ),
          const SizedBox(height: 12),

          // Solve timeout
          _SettingRow(
            label: 'Solve Timeout',
            tooltip: 'Maximum time to wait for plate solve',
            colors: colors,
            child: Row(
              children: [
                Expanded(
                  child: Slider(
                    value: _solveTimeout,
                    min: 10,
                    max: 120,
                    divisions: 11,
                    onChanged: isRunning ? null : (v) => setState(() => _solveTimeout = v),
                  ),
                ),
                SizedBox(
                  width: 40,
                  child: Text(
                    '${_solveTimeout.toInt()}s',
                    style: TextStyle(fontSize: 11, color: colors.textPrimary),
                  ),
                ),
              ],
            ),
          ),
          const SizedBox(height: 12),

          // Start position
          _SettingRow(
            label: 'Start From',
            tooltip: 'Use current telescope position or slew near pole first',
            colors: colors,
            child: SegmentedButton<bool>(
              segments: const [
                ButtonSegment(value: true, label: Text('Current')),
                ButtonSegment(value: false, label: Text('Pole')),
              ],
              selected: {_startFromCurrent},
              onSelectionChanged: isRunning
                  ? null
                  : (v) => setState(() => _startFromCurrent = v.first),
              style: ButtonStyle(
                visualDensity: VisualDensity.compact,
                textStyle: WidgetStateProperty.all(const TextStyle(fontSize: 11)),
              ),
            ),
          ),
          const SizedBox(height: 12),

          // Auto-complete threshold
          _SettingRow(
            label: 'Auto-Complete',
            tooltip: 'Automatically finish when error stays below this value for 3 seconds',
            colors: colors,
            child: Row(
              children: [
                Expanded(
                  child: Slider(
                    value: _autoCompleteThreshold,
                    min: 10,
                    max: 120,
                    divisions: 11,
                    onChanged: isRunning ? null : (v) => setState(() => _autoCompleteThreshold = v),
                  ),
                ),
                SizedBox(
                  width: 40,
                  child: Text(
                    '${_autoCompleteThreshold.toInt()}"',
                    style: TextStyle(fontSize: 11, color: colors.textPrimary),
                  ),
                ),
              ],
            ),
          ),
        ],
      ),
    );
  }

  Widget _buildAdjustmentTips(NightshadeColors colors) {
    return Container(
      padding: const EdgeInsets.all(12),
      decoration: BoxDecoration(
        color: colors.surfaceAlt,
        borderRadius: BorderRadius.circular(8),
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          _TipItem(colors: colors, text: 'Make small adjustments'),
          _TipItem(colors: colors, text: 'Watch the error decrease'),
          _TipItem(colors: colors, text: 'Target < 1 arcmin for best results'),
          _TipItem(colors: colors, text: 'Click Done when satisfied'),
        ],
      ),
    );
  }

  Widget _buildCenterPanel(NightshadeColors colors, PolarAlignPhase phase,
      int point, dynamic error, String status) {
    return Container(
      color: colors.background,
      child: Column(
        children: [
          // Progress indicator
          if (phase == PolarAlignPhase.measuring || phase == PolarAlignPhase.adjusting)
            _buildProgressSteps(colors, phase, point),

          // Main content area
          Expanded(
            child: Center(
              child: phase == PolarAlignPhase.setup
                  ? _buildSetupInstructions(colors)
                  : phase == PolarAlignPhase.measuring
                      ? _buildMeasuringStatus(colors, point, status)
                      : phase == PolarAlignPhase.adjusting
                          ? _buildAdjustmentInstructions(colors, error)
                          : phase == PolarAlignPhase.complete
                              ? _buildCompleteStatus(colors, error)
                              : _buildErrorStatus(colors, status),
            ),
          ),
        ],
      ),
    );
  }

  Widget _buildProgressSteps(NightshadeColors colors, PolarAlignPhase phase, int point) {
    return Container(
      padding: const EdgeInsets.symmetric(vertical: 16, horizontal: 24),
      decoration: BoxDecoration(
        color: colors.surfaceAlt,
        border: Border(bottom: BorderSide(color: colors.border)),
      ),
      child: Row(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          _ProgressStep(
            colors: colors,
            number: 1,
            label: 'Capture 1',
            isActive: phase == PolarAlignPhase.measuring && point == 1,
            isComplete: point > 1 || phase == PolarAlignPhase.adjusting,
          ),
          _ProgressConnector(colors: colors, isComplete: point > 1),
          _ProgressStep(
            colors: colors,
            number: 2,
            label: 'Capture 2',
            isActive: phase == PolarAlignPhase.measuring && point == 2,
            isComplete: point > 2 || phase == PolarAlignPhase.adjusting,
          ),
          _ProgressConnector(colors: colors, isComplete: point > 2),
          _ProgressStep(
            colors: colors,
            number: 3,
            label: 'Capture 3',
            isActive: phase == PolarAlignPhase.measuring && point == 3,
            isComplete: phase == PolarAlignPhase.adjusting,
          ),
          _ProgressConnector(colors: colors, isComplete: phase == PolarAlignPhase.adjusting),
          _ProgressStep(
            colors: colors,
            number: 4,
            label: 'Adjust',
            isActive: phase == PolarAlignPhase.adjusting,
            isComplete: phase == PolarAlignPhase.complete,
          ),
        ],
      ),
    );
  }

  Widget _buildSetupInstructions(NightshadeColors colors) {
    return Padding(
      padding: const EdgeInsets.all(32),
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          Icon(
            LucideIcons.compass,
            size: 64,
            color: colors.primary.withOpacity(0.5),
          ),
          const SizedBox(height: 24),
          Text(
            'Three-Point Polar Alignment',
            style: TextStyle(
              fontSize: 20,
              fontWeight: FontWeight.bold,
              color: colors.textPrimary,
            ),
          ),
          const SizedBox(height: 12),
          Text(
            'This wizard will help you precisely align your mount to the celestial pole.\n'
            'The process captures 3 images at different positions, plate solves them,\n'
            'and calculates your polar alignment error.',
            textAlign: TextAlign.center,
            style: TextStyle(
              fontSize: 13,
              color: colors.textSecondary,
              height: 1.5,
            ),
          ),
          const SizedBox(height: 32),
          _InstructionStep(
            colors: colors,
            number: 1,
            text: 'Roughly align your mount to the pole (within a few degrees)',
          ),
          _InstructionStep(
            colors: colors,
            number: 2,
            text: 'Point the telescope near the celestial pole',
          ),
          _InstructionStep(
            colors: colors,
            number: 3,
            text: 'Ensure camera and mount are connected',
          ),
          _InstructionStep(
            colors: colors,
            number: 4,
            text: 'Configure settings on the left and click Start',
          ),
        ],
      ),
    );
  }

  Widget _buildMeasuringStatus(NightshadeColors colors, int point, String status) {
    final imageData = ref.watch(polarAlignImageProvider);
    final solveCoords = ref.watch(polarAlignSolveProvider);

    return Padding(
      padding: const EdgeInsets.all(16),
      child: Row(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          // Main image area
          Expanded(
            flex: 2,
            child: Container(
              decoration: BoxDecoration(
                color: colors.surfaceAlt,
                borderRadius: BorderRadius.circular(8),
                border: Border.all(color: colors.border),
              ),
              child: Stack(
                children: [
                  // Image display
                  if (imageData != null)
                    Center(
                      child: ClipRRect(
                        borderRadius: BorderRadius.circular(8),
                        child: Image.memory(
                          Uint8List.fromList(imageData),
                          fit: BoxFit.contain,
                          gaplessPlayback: true,
                        ),
                      ),
                    )
                  else
                    Center(
                      child: Column(
                        mainAxisSize: MainAxisSize.min,
                        children: [
                          SizedBox(
                            width: 60,
                            height: 60,
                            child: CircularProgressIndicator(
                              strokeWidth: 4,
                              color: colors.primary,
                            ),
                          ),
                          const SizedBox(height: 16),
                          Text(
                            'Waiting for image...',
                            style: TextStyle(
                              fontSize: 12,
                              color: colors.textMuted,
                            ),
                          ),
                        ],
                      ),
                    ),

                  // Solve coordinates overlay
                  if (solveCoords != null)
                    Positioned(
                      left: 12,
                      bottom: 12,
                      child: Container(
                        padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 6),
                        decoration: BoxDecoration(
                          color: colors.background.withOpacity(0.85),
                          borderRadius: BorderRadius.circular(4),
                          border: Border.all(color: colors.border),
                        ),
                        child: Column(
                          crossAxisAlignment: CrossAxisAlignment.start,
                          mainAxisSize: MainAxisSize.min,
                          children: [
                            Row(
                              mainAxisSize: MainAxisSize.min,
                              children: [
                                Icon(LucideIcons.checkCircle, size: 12, color: colors.success),
                                const SizedBox(width: 6),
                                Text(
                                  'Plate Solved',
                                  style: TextStyle(
                                    fontSize: 10,
                                    fontWeight: FontWeight.w600,
                                    color: colors.success,
                                  ),
                                ),
                              ],
                            ),
                            const SizedBox(height: 4),
                            Text(
                              'RA: ${_formatRA(solveCoords.ra)}',
                              style: TextStyle(
                                fontSize: 11,
                                color: colors.textPrimary,
                                fontFamily: 'monospace',
                              ),
                            ),
                            Text(
                              'Dec: ${_formatDec(solveCoords.dec)}',
                              style: TextStyle(
                                fontSize: 11,
                                color: colors.textPrimary,
                                fontFamily: 'monospace',
                              ),
                            ),
                          ],
                        ),
                      ),
                    ),
                ],
              ),
            ),
          ),

          const SizedBox(width: 16),

          // Progress panel
          SizedBox(
            width: 180,
            child: Container(
              padding: const EdgeInsets.all(16),
              decoration: BoxDecoration(
                color: colors.surface,
                borderRadius: BorderRadius.circular(8),
                border: Border.all(color: colors.border),
              ),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Text(
                    'Progress',
                    style: TextStyle(
                      fontSize: 12,
                      fontWeight: FontWeight.w600,
                      color: colors.textPrimary,
                    ),
                  ),
                  const SizedBox(height: 16),
                  _MeasurementProgressItem(
                    colors: colors,
                    label: 'Point 1',
                    isActive: point == 1,
                    isComplete: point > 1,
                  ),
                  const SizedBox(height: 8),
                  _MeasurementProgressItem(
                    colors: colors,
                    label: 'Point 2',
                    isActive: point == 2,
                    isComplete: point > 2,
                  ),
                  const SizedBox(height: 8),
                  _MeasurementProgressItem(
                    colors: colors,
                    label: 'Point 3',
                    isActive: point == 3,
                    isComplete: point > 3,
                  ),
                  const SizedBox(height: 24),
                  Text(
                    'Status',
                    style: TextStyle(
                      fontSize: 11,
                      color: colors.textMuted,
                    ),
                  ),
                  const SizedBox(height: 6),
                  Text(
                    status,
                    style: TextStyle(
                      fontSize: 12,
                      color: colors.textSecondary,
                    ),
                  ),
                  const Spacer(),
                  // Mount activity indicator
                  Container(
                    padding: const EdgeInsets.all(10),
                    decoration: BoxDecoration(
                      color: colors.surfaceAlt,
                      borderRadius: BorderRadius.circular(6),
                    ),
                    child: Row(
                      children: [
                        SizedBox(
                          width: 14,
                          height: 14,
                          child: CircularProgressIndicator(
                            strokeWidth: 2,
                            color: colors.primary,
                          ),
                        ),
                        const SizedBox(width: 10),
                        Expanded(
                          child: Text(
                            'Capturing Point $point',
                            style: TextStyle(
                              fontSize: 11,
                              color: colors.textPrimary,
                            ),
                          ),
                        ),
                      ],
                    ),
                  ),
                ],
              ),
            ),
          ),
        ],
      ),
    );
  }

  String _formatRA(double degrees) {
    final hours = degrees / 15.0;
    final h = hours.floor();
    final m = ((hours - h) * 60).floor();
    final s = (((hours - h) * 60 - m) * 60).toStringAsFixed(1);
    return '${h.toString().padLeft(2, '0')}h ${m.toString().padLeft(2, '0')}m ${s}s';
  }

  String _formatDec(double degrees) {
    final sign = degrees >= 0 ? '+' : '-';
    final abs = degrees.abs();
    final d = abs.floor();
    final m = ((abs - d) * 60).floor();
    final s = (((abs - d) * 60 - m) * 60).toStringAsFixed(0);
    return '$sign${d.toString().padLeft(2, '0')}° ${m.toString().padLeft(2, '0')}\' ${s}"';
  }

  Widget _buildAdjustmentInstructions(NightshadeColors colors, dynamic error) {
    final imageData = ref.watch(polarAlignImageProvider);

    // Direction text - use Left/Right/Up/Down as per UX design
    final azDir = error != null ? (error.azimuth > 0 ? 'Right' : 'Left') : '--';
    final altDir = error != null ? (error.altitude > 0 ? 'Down' : 'Up') : '--';

    return Padding(
      padding: const EdgeInsets.all(16),
      child: Row(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          // Main image area with bullseye overlay
          Expanded(
            flex: 2,
            child: Container(
              decoration: BoxDecoration(
                color: colors.surfaceAlt,
                borderRadius: BorderRadius.circular(8),
                border: Border.all(color: colors.border),
              ),
              child: Stack(
                children: [
                  // Live image
                  if (imageData != null)
                    Center(
                      child: ClipRRect(
                        borderRadius: BorderRadius.circular(8),
                        child: Image.memory(
                          Uint8List.fromList(imageData),
                          fit: BoxFit.contain,
                          gaplessPlayback: true,
                        ),
                      ),
                    )
                  else
                    Center(
                      child: Column(
                        mainAxisSize: MainAxisSize.min,
                        children: [
                          SizedBox(
                            width: 60,
                            height: 60,
                            child: CircularProgressIndicator(
                              strokeWidth: 4,
                              color: colors.primary,
                            ),
                          ),
                          const SizedBox(height: 16),
                          Text(
                            'Capturing adjustment image...',
                            style: TextStyle(
                              fontSize: 12,
                              color: colors.textMuted,
                            ),
                          ),
                        ],
                      ),
                    ),

                  // Bullseye overlay
                  Positioned.fill(
                    child: CustomPaint(
                      painter: _BullseyeOverlayPainter(
                        colors: colors,
                        azimuthError: error?.azimuth as double?,
                        altitudeError: error?.altitude as double?,
                      ),
                    ),
                  ),
                ],
              ),
            ),
          ),

          const SizedBox(width: 16),

          // Direction panel
          SizedBox(
            width: 200,
            child: Container(
              padding: const EdgeInsets.all(16),
              decoration: BoxDecoration(
                color: colors.surface,
                borderRadius: BorderRadius.circular(8),
                border: Border.all(color: colors.border),
              ),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Text(
                    'Adjust Mount',
                    style: TextStyle(
                      fontSize: 14,
                      fontWeight: FontWeight.w600,
                      color: colors.textPrimary,
                    ),
                  ),
                  const SizedBox(height: 20),

                  // Azimuth direction
                  Text(
                    'Azimuth',
                    style: TextStyle(
                      fontSize: 11,
                      color: colors.textMuted,
                    ),
                  ),
                  const SizedBox(height: 4),
                  if (error != null)
                    Text(
                      '$azDir ${error.azimuth.abs().toStringAsFixed(1)}\'',
                      style: TextStyle(
                        fontSize: 22,
                        fontWeight: FontWeight.bold,
                        color: error.azimuth.abs() < 1.0
                            ? colors.success
                            : error.azimuth.abs() < 3.0
                                ? colors.warning
                                : colors.error,
                      ),
                    )
                  else
                    Text(
                      '--',
                      style: TextStyle(
                        fontSize: 22,
                        fontWeight: FontWeight.bold,
                        color: colors.textMuted,
                      ),
                    ),

                  const SizedBox(height: 20),

                  // Altitude direction
                  Text(
                    'Altitude',
                    style: TextStyle(
                      fontSize: 11,
                      color: colors.textMuted,
                    ),
                  ),
                  const SizedBox(height: 4),
                  if (error != null)
                    Text(
                      '$altDir ${error.altitude.abs().toStringAsFixed(1)}\'',
                      style: TextStyle(
                        fontSize: 22,
                        fontWeight: FontWeight.bold,
                        color: error.altitude.abs() < 1.0
                            ? colors.success
                            : error.altitude.abs() < 3.0
                                ? colors.warning
                                : colors.error,
                      ),
                    )
                  else
                    Text(
                      '--',
                      style: TextStyle(
                        fontSize: 22,
                        fontWeight: FontWeight.bold,
                        color: colors.textMuted,
                      ),
                    ),

                  const SizedBox(height: 24),
                  Divider(color: colors.border),
                  const SizedBox(height: 16),

                  // Total error
                  Text(
                    'Total Error',
                    style: TextStyle(
                      fontSize: 11,
                      color: colors.textMuted,
                    ),
                  ),
                  const SizedBox(height: 4),
                  if (error != null)
                    Text(
                      '${error.total.toStringAsFixed(1)}\'',
                      style: TextStyle(
                        fontSize: 28,
                        fontWeight: FontWeight.bold,
                        color: error.total < 1.0
                            ? colors.success
                            : error.total < 3.0
                                ? colors.warning
                                : colors.error,
                      ),
                    )
                  else
                    Text(
                      '--',
                      style: TextStyle(
                        fontSize: 28,
                        fontWeight: FontWeight.bold,
                        color: colors.textMuted,
                      ),
                    ),

                  const Spacer(),

                  // Progress toward threshold
                  Text(
                    'Threshold: ${(_autoCompleteThreshold / 60).toStringAsFixed(1)}\'',
                    style: TextStyle(
                      fontSize: 10,
                      color: colors.textMuted,
                    ),
                  ),
                  const SizedBox(height: 6),
                  ClipRRect(
                    borderRadius: BorderRadius.circular(4),
                    child: LinearProgressIndicator(
                      value: error != null
                          ? (1.0 - (error.total / 5.0)).clamp(0.0, 1.0)
                          : 0.0,
                      backgroundColor: colors.surfaceAlt,
                      color: error != null
                          ? (error.total < 1.0
                              ? colors.success
                              : error.total < 3.0
                                  ? colors.warning
                                  : colors.error)
                          : colors.textMuted,
                      minHeight: 6,
                    ),
                  ),
                  const SizedBox(height: 12),

                  // Auto-complete indicator
                  Container(
                    padding: const EdgeInsets.all(10),
                    decoration: BoxDecoration(
                      color: colors.surfaceAlt,
                      borderRadius: BorderRadius.circular(6),
                    ),
                    child: Row(
                      children: [
                        Icon(
                          LucideIcons.target,
                          size: 14,
                          color: error != null && error.total * 60 < _autoCompleteThreshold
                              ? colors.success
                              : colors.textMuted,
                        ),
                        const SizedBox(width: 8),
                        Expanded(
                          child: Text(
                            error != null && error.total * 60 < _autoCompleteThreshold
                                ? 'Below threshold!'
                                : 'Adjust to threshold',
                            style: TextStyle(
                              fontSize: 11,
                              color: error != null && error.total * 60 < _autoCompleteThreshold
                                  ? colors.success
                                  : colors.textSecondary,
                            ),
                          ),
                        ),
                      ],
                    ),
                  ),
                ],
              ),
            ),
          ),
        ],
      ),
    );
  }

  Widget _buildCompleteStatus(NightshadeColors colors, dynamic error) {
    return Column(
      mainAxisSize: MainAxisSize.min,
      children: [
        Icon(
          LucideIcons.checkCircle,
          size: 64,
          color: colors.success,
        ),
        const SizedBox(height: 16),
        Text(
          'Alignment Complete',
          style: TextStyle(
            fontSize: 20,
            fontWeight: FontWeight.bold,
            color: colors.textPrimary,
          ),
        ),
        const SizedBox(height: 8),
        if (error != null)
          Text(
            'Final error: ${error.total.toStringAsFixed(1)} arcminutes',
            style: TextStyle(
              fontSize: 14,
              color: colors.textSecondary,
            ),
          ),
      ],
    );
  }

  Widget _buildErrorStatus(NightshadeColors colors, String status) {
    return Column(
      mainAxisSize: MainAxisSize.min,
      children: [
        Icon(
          LucideIcons.alertCircle,
          size: 64,
          color: colors.error,
        ),
        const SizedBox(height: 16),
        Text(
          'Error Occurred',
          style: TextStyle(
            fontSize: 20,
            fontWeight: FontWeight.bold,
            color: colors.textPrimary,
          ),
        ),
        const SizedBox(height: 8),
        Text(
          status,
          style: TextStyle(
            fontSize: 13,
            color: colors.error,
          ),
          textAlign: TextAlign.center,
        ),
      ],
    );
  }

  Widget _buildRightPanel(NightshadeColors colors, PolarAlignPhase phase, dynamic error) {
    return Container(
      color: colors.surface,
      child: Column(
        children: [
          // Error visualization
          Expanded(
            child: _PolarErrorVisualization(
              colors: colors,
              error: error,
              phase: phase,
              pulseAnimation: _pulseController,
            ),
          ),

          // Error values
          Container(
            padding: const EdgeInsets.all(16),
            decoration: BoxDecoration(
              color: colors.surfaceAlt,
              border: Border(top: BorderSide(color: colors.border)),
            ),
            child: _buildErrorValues(colors, error),
          ),
        ],
      ),
    );
  }

  Widget _buildErrorValues(NightshadeColors colors, dynamic error) {
    if (error == null) {
      return Row(
        mainAxisAlignment: MainAxisAlignment.spaceEvenly,
        children: [
          _ErrorValue(colors: colors, label: 'Azimuth', value: '--'),
          _ErrorValue(colors: colors, label: 'Altitude', value: '--'),
          _ErrorValue(colors: colors, label: 'Total', value: '--', isPrimary: true),
        ],
      );
    }

    return Row(
      mainAxisAlignment: MainAxisAlignment.spaceEvenly,
      children: [
        _ErrorValue(
          colors: colors,
          label: 'Azimuth',
          value: '${error.azimuth.toStringAsFixed(1)}\'',
          color: _getErrorColor(colors, error.azimuth.abs()),
        ),
        _ErrorValue(
          colors: colors,
          label: 'Altitude',
          value: '${error.altitude.toStringAsFixed(1)}\'',
          color: _getErrorColor(colors, error.altitude.abs()),
        ),
        _ErrorValue(
          colors: colors,
          label: 'Total',
          value: '${error.total.toStringAsFixed(1)}\'',
          color: _getErrorColor(colors, error.total),
          isPrimary: true,
        ),
      ],
    );
  }

  Color _getErrorColor(NightshadeColors colors, double error) {
    if (error < 1) return colors.success;
    if (error < 3) return colors.info;
    if (error < 5) return colors.warning;
    return colors.error;
  }

  Widget _buildFooter(NightshadeColors colors, PolarAlignPhase phase,
      String status, bool isRunning) {
    return Container(
      padding: const EdgeInsets.all(16),
      decoration: BoxDecoration(
        color: colors.surface,
        border: Border(top: BorderSide(color: colors.border)),
      ),
      child: Row(
        children: [
          // Status
          Expanded(
            child: Row(
              children: [
                if (isRunning)
                  Padding(
                    padding: const EdgeInsets.only(right: 8),
                    child: SizedBox(
                      width: 16,
                      height: 16,
                      child: CircularProgressIndicator(
                        strokeWidth: 2,
                        color: colors.primary,
                      ),
                    ),
                  ),
                Expanded(
                  child: Text(
                    status,
                    style: TextStyle(
                      fontSize: 12,
                      color: colors.textSecondary,
                    ),
                    overflow: TextOverflow.ellipsis,
                  ),
                ),
              ],
            ),
          ),

          const SizedBox(width: 16),

          // Action buttons
          if (phase == PolarAlignPhase.setup)
            FilledButton.icon(
              onPressed: _startAlignment,
              icon: const Icon(LucideIcons.play, size: 16),
              label: const Text('Start Alignment'),
            )
          else if (phase == PolarAlignPhase.measuring)
            OutlinedButton.icon(
              onPressed: _stopAlignment,
              icon: Icon(LucideIcons.square, size: 16, color: colors.error),
              label: Text('Stop', style: TextStyle(color: colors.error)),
            )
          else if (phase == PolarAlignPhase.adjusting)
            Row(
              children: [
                OutlinedButton.icon(
                  onPressed: _stopAlignment,
                  icon: Icon(LucideIcons.square, size: 16, color: colors.error),
                  label: Text('Stop', style: TextStyle(color: colors.error)),
                ),
                const SizedBox(width: 8),
                FilledButton.icon(
                  onPressed: () {
                    // Accept current alignment and mark as complete
                    ref.read(polarAlignPhaseProvider.notifier).state = PolarAlignPhase.complete;
                    ref.read(polarAlignStatusProvider.notifier).state = 'Alignment accepted';
                    _stopAlignment();
                  },
                  icon: const Icon(LucideIcons.check, size: 16),
                  label: const Text('Done'),
                ),
              ],
            )
          else if (phase == PolarAlignPhase.complete || phase == PolarAlignPhase.error)
            Row(
              children: [
                OutlinedButton.icon(
                  onPressed: () {
                    ref.read(polarAlignPhaseProvider.notifier).state = PolarAlignPhase.setup;
                    ref.read(polarAlignStatusProvider.notifier).state =
                        'Configure settings and click Start';
                  },
                  icon: const Icon(LucideIcons.rotateCcw, size: 16),
                  label: const Text('Restart'),
                ),
                const SizedBox(width: 8),
                FilledButton.icon(
                  onPressed: () {
                    if (context.canPop()) {
                      context.pop();
                    } else {
                      context.go('/imaging');
                    }
                  },
                  icon: const Icon(LucideIcons.check, size: 16),
                  label: const Text('Done'),
                ),
              ],
            ),
        ],
      ),
    );
  }
}

// Helper widgets

class _StatusChip extends StatelessWidget {
  final IconData icon;
  final String label;
  final bool isConnected;
  final NightshadeColors colors;

  const _StatusChip({
    required this.icon,
    required this.label,
    required this.isConnected,
    required this.colors,
  });

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 6),
      decoration: BoxDecoration(
        color: isConnected
            ? colors.success.withOpacity(0.1)
            : colors.error.withOpacity(0.1),
        borderRadius: BorderRadius.circular(6),
        border: Border.all(
          color: isConnected
              ? colors.success.withOpacity(0.3)
              : colors.error.withOpacity(0.3),
        ),
      ),
      child: Row(
        mainAxisSize: MainAxisSize.min,
        children: [
          Icon(
            icon,
            size: 14,
            color: isConnected ? colors.success : colors.error,
          ),
          const SizedBox(width: 6),
          Text(
            label,
            style: TextStyle(
              fontSize: 11,
              fontWeight: FontWeight.w500,
              color: isConnected ? colors.success : colors.error,
            ),
          ),
        ],
      ),
    );
  }
}

class _SettingRow extends StatelessWidget {
  final String label;
  final String tooltip;
  final NightshadeColors colors;
  final Widget child;

  const _SettingRow({
    required this.label,
    required this.tooltip,
    required this.colors,
    required this.child,
  });

  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        Row(
          children: [
            Text(
              label,
              style: TextStyle(
                fontSize: 11,
                color: colors.textMuted,
              ),
            ),
            const SizedBox(width: 4),
            Tooltip(
              message: tooltip,
              waitDuration: const Duration(milliseconds: 500),
              child: Icon(
                LucideIcons.helpCircle,
                size: 12,
                color: colors.textMuted.withOpacity(0.6),
              ),
            ),
          ],
        ),
        const SizedBox(height: 4),
        child,
      ],
    );
  }
}

class _TipItem extends StatelessWidget {
  final NightshadeColors colors;
  final String text;

  const _TipItem({required this.colors, required this.text});

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.symmetric(vertical: 4),
      child: Row(
        children: [
          Icon(LucideIcons.check, size: 12, color: colors.success),
          const SizedBox(width: 8),
          Text(
            text,
            style: TextStyle(fontSize: 11, color: colors.textSecondary),
          ),
        ],
      ),
    );
  }
}

class _MeasurementProgressItem extends StatelessWidget {
  final NightshadeColors colors;
  final String label;
  final bool isActive;
  final bool isComplete;

  const _MeasurementProgressItem({
    required this.colors,
    required this.label,
    required this.isActive,
    required this.isComplete,
  });

  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        Container(
          width: 20,
          height: 20,
          decoration: BoxDecoration(
            color: isComplete
                ? colors.success
                : isActive
                    ? colors.primary.withOpacity(0.2)
                    : colors.surfaceAlt,
            shape: BoxShape.circle,
            border: Border.all(
              color: isComplete
                  ? colors.success
                  : isActive
                      ? colors.primary
                      : colors.border,
              width: 2,
            ),
          ),
          child: Center(
            child: isComplete
                ? Icon(LucideIcons.check, size: 12, color: colors.background)
                : isActive
                    ? SizedBox(
                        width: 10,
                        height: 10,
                        child: CircularProgressIndicator(
                          strokeWidth: 2,
                          color: colors.primary,
                        ),
                      )
                    : null,
          ),
        ),
        const SizedBox(width: 10),
        Text(
          label,
          style: TextStyle(
            fontSize: 12,
            fontWeight: isActive ? FontWeight.w600 : FontWeight.normal,
            color: isComplete || isActive ? colors.textPrimary : colors.textMuted,
          ),
        ),
        if (isComplete) ...[
          const Spacer(),
          Icon(LucideIcons.checkCircle, size: 14, color: colors.success),
        ],
      ],
    );
  }
}

class _ProgressStep extends StatelessWidget {
  final NightshadeColors colors;
  final int number;
  final String label;
  final bool isActive;
  final bool isComplete;

  const _ProgressStep({
    required this.colors,
    required this.number,
    required this.label,
    required this.isActive,
    required this.isComplete,
  });

  @override
  Widget build(BuildContext context) {
    final color = isComplete
        ? colors.success
        : isActive
            ? colors.primary
            : colors.textMuted;

    return Column(
      children: [
        Container(
          width: 32,
          height: 32,
          decoration: BoxDecoration(
            color: isComplete || isActive ? color.withOpacity(0.2) : colors.surfaceAlt,
            shape: BoxShape.circle,
            border: Border.all(color: color, width: 2),
          ),
          child: Center(
            child: isComplete
                ? Icon(LucideIcons.check, size: 16, color: color)
                : Text(
                    number.toString(),
                    style: TextStyle(
                      fontSize: 12,
                      fontWeight: FontWeight.bold,
                      color: color,
                    ),
                  ),
          ),
        ),
        const SizedBox(height: 4),
        Text(
          label,
          style: TextStyle(
            fontSize: 10,
            color: color,
          ),
        ),
      ],
    );
  }
}

class _ProgressConnector extends StatelessWidget {
  final NightshadeColors colors;
  final bool isComplete;

  const _ProgressConnector({
    required this.colors,
    required this.isComplete,
  });

  @override
  Widget build(BuildContext context) {
    return Container(
      width: 40,
      height: 2,
      margin: const EdgeInsets.only(bottom: 18),
      color: isComplete ? colors.success : colors.border,
    );
  }
}

class _InstructionStep extends StatelessWidget {
  final NightshadeColors colors;
  final int number;
  final String text;

  const _InstructionStep({
    required this.colors,
    required this.number,
    required this.text,
  });

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.symmetric(vertical: 8),
      child: Row(
        children: [
          Container(
            width: 24,
            height: 24,
            decoration: BoxDecoration(
              color: colors.primary.withOpacity(0.1),
              shape: BoxShape.circle,
            ),
            child: Center(
              child: Text(
                number.toString(),
                style: TextStyle(
                  fontSize: 12,
                  fontWeight: FontWeight.bold,
                  color: colors.primary,
                ),
              ),
            ),
          ),
          const SizedBox(width: 12),
          Expanded(
            child: Text(
              text,
              style: TextStyle(
                fontSize: 13,
                color: colors.textSecondary,
              ),
            ),
          ),
        ],
      ),
    );
  }
}

class _ErrorValue extends StatelessWidget {
  final NightshadeColors colors;
  final String label;
  final String value;
  final Color? color;
  final bool isPrimary;

  const _ErrorValue({
    required this.colors,
    required this.label,
    required this.value,
    this.color,
    this.isPrimary = false,
  });

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        Text(
          label,
          style: TextStyle(
            fontSize: 10,
            color: colors.textMuted,
          ),
        ),
        const SizedBox(height: 4),
        Text(
          value,
          style: TextStyle(
            fontSize: isPrimary ? 18 : 14,
            fontWeight: isPrimary ? FontWeight.bold : FontWeight.w500,
            color: color ?? colors.textPrimary,
          ),
        ),
      ],
    );
  }
}

class _BullseyeOverlayPainter extends CustomPainter {
  final NightshadeColors colors;
  final double? azimuthError;
  final double? altitudeError;

  _BullseyeOverlayPainter({
    required this.colors,
    this.azimuthError,
    this.altitudeError,
  });

  @override
  void paint(Canvas canvas, Size size) {
    final center = Offset(size.width / 2, size.height / 2);
    final maxRadius = (size.width < size.height ? size.width : size.height) / 2 - 40;

    // Scale: 5 arcminutes = maxRadius
    final scale = maxRadius / 5.0;

    // Draw concentric rings at 1', 3', 5'
    final ringPaint = Paint()
      ..style = PaintingStyle.stroke
      ..strokeWidth = 1.5;

    for (final arcmin in [1.0, 3.0, 5.0]) {
      ringPaint.color = arcmin == 1.0
          ? colors.success.withOpacity(0.6)
          : arcmin == 3.0
              ? colors.warning.withOpacity(0.6)
              : colors.error.withOpacity(0.6);
      canvas.drawCircle(center, arcmin * scale, ringPaint);

      // Draw labels
      final textPainter = TextPainter(
        text: TextSpan(
          text: '${arcmin.toInt()}\'',
          style: TextStyle(
            fontSize: 10,
            color: ringPaint.color,
          ),
        ),
        textDirection: TextDirection.ltr,
      );
      textPainter.layout();
      textPainter.paint(
        canvas,
        Offset(center.dx + arcmin * scale + 4, center.dy - textPainter.height / 2),
      );
    }

    // Draw crosshairs
    final crossPaint = Paint()
      ..color = colors.textMuted.withOpacity(0.4)
      ..strokeWidth = 1;
    canvas.drawLine(
      Offset(center.dx - maxRadius, center.dy),
      Offset(center.dx + maxRadius, center.dy),
      crossPaint,
    );
    canvas.drawLine(
      Offset(center.dx, center.dy - maxRadius),
      Offset(center.dx, center.dy + maxRadius),
      crossPaint,
    );

    // Draw center target
    final targetPaint = Paint()
      ..color = colors.primary
      ..style = PaintingStyle.fill;
    canvas.drawCircle(center, 6, targetPaint);

    // Draw error position
    if (azimuthError != null && altitudeError != null) {
      final errorX = azimuthError!.clamp(-5.0, 5.0) * scale;
      final errorY = -altitudeError!.clamp(-5.0, 5.0) * scale; // Negative because screen Y is inverted
      final errorPos = Offset(center.dx + errorX, center.dy + errorY);

      // Draw line from center to error position
      final linePaint = Paint()
        ..color = colors.error.withOpacity(0.5)
        ..strokeWidth = 2;
      canvas.drawLine(center, errorPos, linePaint);

      // Error indicator with glow effect
      final glowPaint = Paint()
        ..color = colors.error.withOpacity(0.3);
      canvas.drawCircle(errorPos, 14, glowPaint);

      final errorPaint = Paint()
        ..color = colors.error
        ..style = PaintingStyle.fill;
      canvas.drawCircle(errorPos, 8, errorPaint);
    }
  }

  @override
  bool shouldRepaint(covariant _BullseyeOverlayPainter oldDelegate) {
    return oldDelegate.azimuthError != azimuthError ||
        oldDelegate.altitudeError != altitudeError;
  }
}

class _PolarErrorVisualization extends StatelessWidget {
  final NightshadeColors colors;
  final dynamic error;
  final PolarAlignPhase phase;
  final AnimationController pulseAnimation;

  const _PolarErrorVisualization({
    required this.colors,
    required this.error,
    required this.phase,
    required this.pulseAnimation,
  });

  @override
  Widget build(BuildContext context) {
    return CustomPaint(
      painter: _PolarErrorPainter(
        colors: colors,
        error: error,
        phase: phase,
        pulseValue: pulseAnimation.value,
      ),
      size: Size.infinite,
    );
  }
}

class _PolarErrorPainter extends CustomPainter {
  final NightshadeColors colors;
  final dynamic error;
  final PolarAlignPhase phase;
  final double pulseValue;

  _PolarErrorPainter({
    required this.colors,
    required this.error,
    required this.phase,
    required this.pulseValue,
  });

  @override
  void paint(Canvas canvas, Size size) {
    final center = Offset(size.width / 2, size.height / 2);
    final maxRadius = size.width < size.height ? size.width / 2 - 20 : size.height / 2 - 20;

    // Draw error zones (5', 3', 1')
    final zones = [
      (5.0, colors.error.withOpacity(0.1)),
      (3.0, colors.warning.withOpacity(0.1)),
      (1.0, colors.success.withOpacity(0.1)),
    ];

    for (final (errorVal, color) in zones) {
      final radius = maxRadius * (errorVal / 5.0);
      canvas.drawCircle(
        center,
        radius,
        Paint()..color = color,
      );
      canvas.drawCircle(
        center,
        radius,
        Paint()
          ..color = color.withOpacity(0.5)
          ..style = PaintingStyle.stroke
          ..strokeWidth = 1,
      );
    }

    // Draw crosshairs
    final crossPaint = Paint()
      ..color = colors.border
      ..strokeWidth = 1;
    canvas.drawLine(
      Offset(center.dx - maxRadius, center.dy),
      Offset(center.dx + maxRadius, center.dy),
      crossPaint,
    );
    canvas.drawLine(
      Offset(center.dx, center.dy - maxRadius),
      Offset(center.dx, center.dy + maxRadius),
      crossPaint,
    );

    // Draw center target (pulsing)
    final targetRadius = 8.0 + pulseValue * 4;
    canvas.drawCircle(
      center,
      targetRadius,
      Paint()..color = colors.primary.withOpacity(0.3 + pulseValue * 0.3),
    );
    canvas.drawCircle(
      center,
      4,
      Paint()..color = colors.primary,
    );

    // Draw error position
    if (error != null && phase == PolarAlignPhase.adjusting) {
      final scale = maxRadius / 5.0; // 5 arcminutes = max radius
      final errorX = (error.azimuth as double).clamp(-5.0, 5.0) * scale;
      final errorY = -(error.altitude as double).clamp(-5.0, 5.0) * scale;
      final errorPos = Offset(center.dx + errorX, center.dy + errorY);

      // Error indicator
      canvas.drawCircle(
        errorPos,
        10,
        Paint()..color = colors.error.withOpacity(0.3),
      );
      canvas.drawCircle(
        errorPos,
        6,
        Paint()..color = colors.error,
      );
    }

    // Draw labels
    final textPainter = TextPainter(textDirection: TextDirection.ltr);
    final labels = ['5\'', '3\'', '1\''];
    final positions = [5.0, 3.0, 1.0];
    for (int i = 0; i < labels.length; i++) {
      final radius = maxRadius * (positions[i] / 5.0);
      textPainter.text = TextSpan(
        text: labels[i],
        style: TextStyle(fontSize: 9, color: colors.textMuted),
      );
      textPainter.layout();
      textPainter.paint(
        canvas,
        Offset(center.dx + radius + 4, center.dy - textPainter.height / 2),
      );
    }
  }

  @override
  bool shouldRepaint(covariant _PolarErrorPainter oldDelegate) {
    return oldDelegate.error != error ||
        oldDelegate.phase != phase ||
        oldDelegate.pulseValue != pulseValue;
  }
}
