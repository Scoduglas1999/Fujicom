import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:lucide_icons/lucide_icons.dart';
import 'package:nightshade_core/nightshade_core.dart';
import 'package:nightshade_ui/nightshade_ui.dart';

/// Validation issue severity
enum ValidationSeverity {
  error,   // Cannot start sequence
  warning, // Can start but may cause issues
  info,    // Informational
}

/// A single validation issue
class ValidationIssue {
  final ValidationSeverity severity;
  final String category;
  final String title;
  final String description;
  final String? nodeId; // If related to a specific node
  final String? resolution; // Suggested fix

  const ValidationIssue({
    required this.severity,
    required this.category,
    required this.title,
    required this.description,
    this.nodeId,
    this.resolution,
  });
}

/// Validation result for a sequence
class ValidationResult {
  final List<ValidationIssue> issues;
  final DateTime validatedAt;

  const ValidationResult({
    required this.issues,
    required this.validatedAt,
  });

  bool get hasErrors => issues.any((i) => i.severity == ValidationSeverity.error);
  bool get hasWarnings => issues.any((i) => i.severity == ValidationSeverity.warning);
  bool get isValid => !hasErrors;

  int get errorCount => issues.where((i) => i.severity == ValidationSeverity.error).length;
  int get warningCount => issues.where((i) => i.severity == ValidationSeverity.warning).length;
  int get infoCount => issues.where((i) => i.severity == ValidationSeverity.info).length;
}

/// Provider for sequence validation
final sequenceValidationProvider = Provider<SequenceValidator>((ref) {
  return SequenceValidator(ref);
});

/// Validates sequences before execution
class SequenceValidator {
  final Ref ref;

  SequenceValidator(this.ref);

  /// Run all validation checks
  Future<ValidationResult> validate(Sequence sequence) async {
    final issues = <ValidationIssue>[];

    // Run all checks
    issues.addAll(_checkSequenceStructure(sequence));
    issues.addAll(_checkTargets(sequence));
    issues.addAll(_checkExposures(sequence));
    issues.addAll(await _checkEquipment(sequence));
    issues.addAll(_checkSettings(sequence));
    issues.addAll(_checkTiming(sequence));

    return ValidationResult(
      issues: issues,
      validatedAt: DateTime.now(),
    );
  }

  /// Check basic sequence structure
  List<ValidationIssue> _checkSequenceStructure(Sequence sequence) {
    final issues = <ValidationIssue>[];

    if (sequence.nodes.isEmpty) {
      issues.add(const ValidationIssue(
        severity: ValidationSeverity.error,
        category: 'Structure',
        title: 'Empty Sequence',
        description: 'The sequence has no nodes. Add at least one instruction to run.',
        resolution: 'Add exposure or other instruction nodes to the sequence.',
      ));
    }

    if (sequence.rootNodeId == null) {
      issues.add(const ValidationIssue(
        severity: ValidationSeverity.error,
        category: 'Structure',
        title: 'No Root Node',
        description: 'The sequence has no root node to execute.',
        resolution: 'Ensure the sequence has a root node.',
      ));
    }

    // Check for orphaned nodes
    final referencedIds = <String>{};
    for (final node in sequence.nodes.values) {
      if (node.childIds.isNotEmpty) {
        referencedIds.addAll(node.childIds);
      }
    }
    if (sequence.rootNodeId != null) {
      referencedIds.add(sequence.rootNodeId!);
    }

    final orphanedNodes = sequence.nodes.keys.where((id) => !referencedIds.contains(id)).toList();
    if (orphanedNodes.isNotEmpty) {
      issues.add(ValidationIssue(
        severity: ValidationSeverity.warning,
        category: 'Structure',
        title: 'Orphaned Nodes',
        description: '${orphanedNodes.length} node(s) are not connected to the sequence.',
        resolution: 'Remove unused nodes or connect them to a parent.',
      ));
    }

    return issues;
  }

  /// Get all exposure nodes from sequence
  List<ExposureNode> _getExposureNodes(Sequence sequence) {
    return sequence.nodes.values
        .whereType<ExposureNode>()
        .where((n) => n.isEnabled)
        .toList();
  }

  /// Check target configurations
  List<ValidationIssue> _checkTargets(Sequence sequence) {
    final issues = <ValidationIssue>[];

    final targets = sequence.targetGroups;
    final exposures = _getExposureNodes(sequence);
    if (targets.isEmpty && exposures.isNotEmpty) {
      issues.add(const ValidationIssue(
        severity: ValidationSeverity.warning,
        category: 'Targets',
        title: 'No Targets Defined',
        description: 'Exposures exist but no target is defined. The mount will image at its current position.',
        resolution: 'Add a Target Group node with coordinates.',
      ));
    }

    for (final target in targets) {
      // Check coordinates validity
      if (target.raHours < 0 || target.raHours >= 24) {
        issues.add(ValidationIssue(
          severity: ValidationSeverity.error,
          category: 'Targets',
          title: 'Invalid RA',
          description: 'Target "${target.targetName}" has invalid RA: ${target.raHours}h',
          nodeId: target.id,
          resolution: 'RA must be between 0 and 24 hours.',
        ));
      }

      if (target.decDegrees < -90 || target.decDegrees > 90) {
        issues.add(ValidationIssue(
          severity: ValidationSeverity.error,
          category: 'Targets',
          title: 'Invalid Dec',
          description: 'Target "${target.targetName}" has invalid Dec: ${target.decDegrees}°',
          nodeId: target.id,
          resolution: 'Declination must be between -90 and +90 degrees.',
        ));
      }

      // Check if target has any child nodes
      if (target.childIds.isEmpty) {
        issues.add(ValidationIssue(
          severity: ValidationSeverity.warning,
          category: 'Targets',
          title: 'Empty Target',
          description: 'Target "${target.targetName}" has no instructions.',
          nodeId: target.id,
          resolution: 'Add exposure or other instruction nodes to the target.',
        ));
      }

      // Check minimum altitude
      if (target.minAltitude != null && target.minAltitude! < 10) {
        issues.add(ValidationIssue(
          severity: ValidationSeverity.warning,
          category: 'Targets',
          title: 'Very Low Altitude Limit',
          description: 'Target "${target.targetName}" minimum altitude is ${target.minAltitude}°. Imaging near the horizon may result in poor quality.',
          nodeId: target.id,
          resolution: 'Consider setting minimum altitude to 20° or higher.',
        ));
      }
    }

    return issues;
  }

  /// Check exposure configurations
  List<ValidationIssue> _checkExposures(Sequence sequence) {
    final issues = <ValidationIssue>[];

    final exposures = _getExposureNodes(sequence);
    if (exposures.isEmpty) {
      issues.add(const ValidationIssue(
        severity: ValidationSeverity.warning,
        category: 'Imaging',
        title: 'No Exposures',
        description: 'No exposure nodes found. The sequence will run but capture no images.',
        resolution: 'Add Exposure nodes to capture images.',
      ));
      return issues;
    }

    for (final exposure in exposures) {
      // Check exposure time
      if (exposure.durationSecs <= 0) {
        issues.add(ValidationIssue(
          severity: ValidationSeverity.error,
          category: 'Imaging',
          title: 'Invalid Exposure Time',
          description: 'Exposure "${exposure.name}" has invalid duration: ${exposure.durationSecs}s',
          nodeId: exposure.id,
          resolution: 'Set a positive exposure duration.',
        ));
      }

      if (exposure.durationSecs > 1800) {
        issues.add(ValidationIssue(
          severity: ValidationSeverity.warning,
          category: 'Imaging',
          title: 'Very Long Exposure',
          description: 'Exposure "${exposure.name}" is ${(exposure.durationSecs / 60).toStringAsFixed(0)} minutes. Very long exposures may fail due to tracking errors.',
          nodeId: exposure.id,
          resolution: 'Consider breaking into shorter exposures or using auto-guiding.',
        ));
      }

      // Check count
      if (exposure.count <= 0) {
        issues.add(ValidationIssue(
          severity: ValidationSeverity.error,
          category: 'Imaging',
          title: 'Invalid Frame Count',
          description: 'Exposure "${exposure.name}" has count of ${exposure.count}.',
          nodeId: exposure.id,
          resolution: 'Set at least 1 frame to capture.',
        ));
      }

      // Check binning - high binning reduces resolution
      if (exposure.binning == BinningMode.three || exposure.binning == BinningMode.four) {
        issues.add(ValidationIssue(
          severity: ValidationSeverity.info,
          category: 'Imaging',
          title: 'High Binning',
          description: 'Exposure "${exposure.name}" uses ${exposure.binning.label} binning which reduces resolution.',
          nodeId: exposure.id,
        ));
      }
    }

    // Total integration check
    final totalSecs = sequence.totalIntegrationSecs;
    if (totalSecs > 28800) { // 8 hours
      issues.add(ValidationIssue(
        severity: ValidationSeverity.warning,
        category: 'Timing',
        title: 'Very Long Sequence',
        description: 'Total integration time is ${(totalSecs / 3600).toStringAsFixed(1)} hours. Consider splitting across multiple nights.',
      ));
    }

    return issues;
  }

  /// Check equipment connection status based on what devices the sequence needs
  Future<List<ValidationIssue>> _checkEquipment(Sequence sequence) async {
    final issues = <ValidationIssue>[];

    // Collect all required device types from enabled nodes
    final requiredDevices = <DeviceType>{};
    for (final node in sequence.nodes.values) {
      if (node.isEnabled) {
        requiredDevices.addAll(node.requiredDevices);
      }
    }

    // If no devices required, nothing to check
    if (requiredDevices.isEmpty) {
      return issues;
    }

    try {
      final backend = ref.read(backendProvider);
      final connectedDevices = await backend.getConnectedDevices();

      // Build set of connected device types from backend
      final connectedTypes = connectedDevices.map((d) => d.deviceType).toSet();

      // Check guider separately via guiderStateProvider (PHD2 is not in getConnectedDevices)
      final guiderState = ref.read(guiderStateProvider);
      final hasGuider = guiderState.connectionState == DeviceConnectionState.connected;
      if (hasGuider) {
        connectedTypes.add(DeviceType.guider);
      }

      // Check each required device type
      if (requiredDevices.contains(DeviceType.camera) && !connectedTypes.contains(DeviceType.camera)) {
        issues.add(const ValidationIssue(
          severity: ValidationSeverity.error,
          category: 'Equipment',
          title: 'No Camera Connected',
          description: 'This sequence requires a camera to capture images.',
          resolution: 'Connect a camera in the Equipment panel.',
        ));
      }

      if (requiredDevices.contains(DeviceType.mount) && !connectedTypes.contains(DeviceType.mount)) {
        issues.add(const ValidationIssue(
          severity: ValidationSeverity.warning,
          category: 'Equipment',
          title: 'No Mount Connected',
          description: 'This sequence includes slewing or tracking operations that require a mount.',
          resolution: 'Connect a mount in the Equipment panel.',
        ));
      }

      if (requiredDevices.contains(DeviceType.focuser) && !connectedTypes.contains(DeviceType.focuser)) {
        issues.add(const ValidationIssue(
          severity: ValidationSeverity.warning,
          category: 'Equipment',
          title: 'No Focuser Connected',
          description: 'This sequence includes autofocus operations that require a focuser.',
          resolution: 'Connect a focuser in the Equipment panel.',
        ));
      }

      if (requiredDevices.contains(DeviceType.filterWheel) && !connectedTypes.contains(DeviceType.filterWheel)) {
        issues.add(const ValidationIssue(
          severity: ValidationSeverity.warning,
          category: 'Equipment',
          title: 'No Filter Wheel Connected',
          description: 'This sequence includes filter changes that require a filter wheel.',
          resolution: 'Connect a filter wheel in the Equipment panel.',
        ));
      }

      if (requiredDevices.contains(DeviceType.guider) && !hasGuider) {
        issues.add(const ValidationIssue(
          severity: ValidationSeverity.warning,
          category: 'Equipment',
          title: 'No Guider Connected',
          description: 'This sequence includes guiding or dithering operations that require PHD2.',
          resolution: 'Connect to PHD2 in the Guiding panel.',
        ));
      }

      if (requiredDevices.contains(DeviceType.rotator) && !connectedTypes.contains(DeviceType.rotator)) {
        issues.add(const ValidationIssue(
          severity: ValidationSeverity.info,
          category: 'Equipment',
          title: 'No Rotator Connected',
          description: 'This sequence includes rotator operations.',
          resolution: 'Connect a rotator in the Equipment panel.',
        ));
      }

      if (requiredDevices.contains(DeviceType.dome) && !connectedTypes.contains(DeviceType.dome)) {
        issues.add(const ValidationIssue(
          severity: ValidationSeverity.info,
          category: 'Equipment',
          title: 'No Dome Connected',
          description: 'This sequence includes dome operations.',
          resolution: 'Connect a dome in the Equipment panel.',
        ));
      }
    } catch (e) {
      // Backend not available (e.g., disconnected mode)
      issues.add(const ValidationIssue(
        severity: ValidationSeverity.warning,
        category: 'Equipment',
        title: 'Equipment Status Unknown',
        description: 'Could not check equipment status. Ensure you are connected to the backend.',
      ));
    }

    return issues;
  }

  /// Check app settings relevant to sequencing
  List<ValidationIssue> _checkSettings(Sequence sequence) {
    final issues = <ValidationIssue>[];

    // Check if sequence has a name
    if (sequence.name.isEmpty || sequence.name == 'Untitled Sequence') {
      issues.add(const ValidationIssue(
        severity: ValidationSeverity.info,
        category: 'Settings',
        title: 'Default Sequence Name',
        description: 'Consider naming your sequence for easier identification.',
      ));
    }

    // Check for very long estimated duration
    if (sequence.estimatedDurationMins != null && sequence.estimatedDurationMins! > 600) {
      issues.add(const ValidationIssue(
        severity: ValidationSeverity.info,
        category: 'Settings',
        title: 'Long Sequence',
        description: 'This sequence is estimated to run for over 10 hours.',
      ));
    }

    return issues;
  }

  /// Check timing and scheduling issues
  List<ValidationIssue> _checkTiming(Sequence sequence) {
    final issues = <ValidationIssue>[];

    // Check wait nodes
    for (final node in sequence.nodes.values) {
      if (node is WaitTimeNode && node.waitUntil != null) {
        if (node.waitUntil!.isBefore(DateTime.now())) {
          issues.add(ValidationIssue(
            severity: ValidationSeverity.warning,
            category: 'Timing',
            title: 'Wait Time Passed',
            description: 'Wait node "${node.name}" is set for a time that has already passed.',
            nodeId: node.id,
            resolution: 'Update the wait time or remove the node.',
          ));
        }
      }

      if (node is LoopNode && node.repeatUntil != null) {
        if (node.repeatUntil!.isBefore(DateTime.now())) {
          issues.add(ValidationIssue(
            severity: ValidationSeverity.warning,
            category: 'Timing',
            title: 'Loop End Time Passed',
            description: 'Loop "${node.name}" end time has already passed.',
            nodeId: node.id,
            resolution: 'Update the end time or change loop condition.',
          ));
        }
      }
    }

    return issues;
  }
}

/// Pre-flight validation dialog
class PreFlightValidationDialog extends ConsumerStatefulWidget {
  final VoidCallback? onStartSequence;

  const PreFlightValidationDialog({
    super.key,
    this.onStartSequence,
  });

  @override
  ConsumerState<PreFlightValidationDialog> createState() => _PreFlightValidationDialogState();
}

class _PreFlightValidationDialogState extends ConsumerState<PreFlightValidationDialog> {
  ValidationResult? _result;
  bool _isValidating = true;

  @override
  void initState() {
    super.initState();
    _runValidation();
  }

  Future<void> _runValidation() async {
    final sequence = ref.read(currentSequenceProvider);
    if (sequence == null) {
      setState(() => _isValidating = false);
      return;
    }

    final validator = ref.read(sequenceValidationProvider);
    final result = await validator.validate(sequence);

    if (mounted) {
      setState(() {
        _result = result;
        _isValidating = false;
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    final colors = Theme.of(context).extension<NightshadeColors>()!;

    return Dialog(
      backgroundColor: colors.surface,
      shape: RoundedRectangleBorder(
        borderRadius: BorderRadius.circular(16),
      ),
      child: Container(
        width: 500,
        constraints: const BoxConstraints(maxHeight: 600),
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            // Header
            _buildHeader(colors),

            // Content
            Flexible(
              child: _isValidating
                  ? _buildLoadingState(colors)
                  : _result == null
                      ? _buildErrorState(colors)
                      : _buildResults(colors),
            ),

            // Actions
            _buildActions(colors),
          ],
        ),
      ),
    );
  }

  Widget _buildHeader(NightshadeColors colors) {
    return Container(
      padding: const EdgeInsets.all(20),
      decoration: BoxDecoration(
        border: Border(bottom: BorderSide(color: colors.border)),
      ),
      child: Row(
        children: [
          Container(
            padding: const EdgeInsets.all(10),
            decoration: BoxDecoration(
              color: colors.primary.withValues(alpha: 0.1),
              borderRadius: BorderRadius.circular(10),
            ),
            child: Icon(LucideIcons.clipboardCheck, color: colors.primary, size: 20),
          ),
          const SizedBox(width: 16),
          Expanded(
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Text(
                  'Pre-Flight Validation',
                  style: TextStyle(
                    fontSize: 16,
                    fontWeight: FontWeight.w600,
                    color: colors.textPrimary,
                  ),
                ),
                Text(
                  'Checking sequence before execution',
                  style: TextStyle(
                    fontSize: 12,
                    color: colors.textSecondary,
                  ),
                ),
              ],
            ),
          ),
          IconButton(
            icon: Icon(LucideIcons.x, color: colors.textMuted, size: 18),
            onPressed: () => Navigator.of(context).pop(),
          ),
        ],
      ),
    );
  }

  Widget _buildLoadingState(NightshadeColors colors) {
    return Padding(
      padding: const EdgeInsets.all(40),
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          SizedBox(
            width: 40,
            height: 40,
            child: CircularProgressIndicator(
              strokeWidth: 3,
              color: colors.primary,
            ),
          ),
          const SizedBox(height: 16),
          Text(
            'Running validation checks...',
            style: TextStyle(
              fontSize: 13,
              color: colors.textSecondary,
            ),
          ),
        ],
      ),
    );
  }

  Widget _buildErrorState(NightshadeColors colors) {
    return Padding(
      padding: const EdgeInsets.all(40),
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          Icon(LucideIcons.alertCircle, size: 40, color: colors.error),
          const SizedBox(height: 16),
          Text(
            'No sequence to validate',
            style: TextStyle(
              fontSize: 14,
              color: colors.textPrimary,
            ),
          ),
        ],
      ),
    );
  }

  Widget _buildResults(NightshadeColors colors) {
    final result = _result!;

    return SingleChildScrollView(
      padding: const EdgeInsets.all(20),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          // Summary
          _buildSummary(colors, result),
          const SizedBox(height: 20),

          // Issues list
          if (result.issues.isNotEmpty) ...[
            Text(
              'Issues Found',
              style: TextStyle(
                fontSize: 12,
                fontWeight: FontWeight.w600,
                color: colors.textSecondary,
                letterSpacing: 0.5,
              ),
            ),
            const SizedBox(height: 12),
            ...result.issues.map((issue) => _buildIssueCard(colors, issue)),
          ] else ...[
            _buildAllClearCard(colors),
          ],
        ],
      ),
    );
  }

  Widget _buildSummary(NightshadeColors colors, ValidationResult result) {
    return Container(
      padding: const EdgeInsets.all(16),
      decoration: BoxDecoration(
        color: result.hasErrors
            ? colors.error.withValues(alpha: 0.1)
            : result.hasWarnings
                ? colors.warning.withValues(alpha: 0.1)
                : colors.success.withValues(alpha: 0.1),
        borderRadius: BorderRadius.circular(12),
        border: Border.all(
          color: result.hasErrors
              ? colors.error.withValues(alpha: 0.3)
              : result.hasWarnings
                  ? colors.warning.withValues(alpha: 0.3)
                  : colors.success.withValues(alpha: 0.3),
        ),
      ),
      child: Row(
        children: [
          Icon(
            result.hasErrors
                ? LucideIcons.xCircle
                : result.hasWarnings
                    ? LucideIcons.alertTriangle
                    : LucideIcons.checkCircle,
            size: 32,
            color: result.hasErrors
                ? colors.error
                : result.hasWarnings
                    ? colors.warning
                    : colors.success,
          ),
          const SizedBox(width: 16),
          Expanded(
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Text(
                  result.hasErrors
                      ? 'Cannot Start Sequence'
                      : result.hasWarnings
                          ? 'Ready with Warnings'
                          : 'All Checks Passed',
                  style: TextStyle(
                    fontSize: 14,
                    fontWeight: FontWeight.w600,
                    color: result.hasErrors
                        ? colors.error
                        : result.hasWarnings
                            ? colors.warning
                            : colors.success,
                  ),
                ),
                Text(
                  result.hasErrors
                      ? 'Please fix ${result.errorCount} error(s) before starting'
                      : result.hasWarnings
                          ? '${result.warningCount} warning(s) found'
                          : 'Sequence is ready to run',
                  style: TextStyle(
                    fontSize: 12,
                    color: colors.textSecondary,
                  ),
                ),
              ],
            ),
          ),
          // Issue counts
          Row(
            children: [
              if (result.errorCount > 0)
                _CountBadge(
                  count: result.errorCount,
                  color: colors.error,
                  icon: LucideIcons.xCircle,
                ),
              if (result.warningCount > 0) ...[
                const SizedBox(width: 8),
                _CountBadge(
                  count: result.warningCount,
                  color: colors.warning,
                  icon: LucideIcons.alertTriangle,
                ),
              ],
              if (result.infoCount > 0) ...[
                const SizedBox(width: 8),
                _CountBadge(
                  count: result.infoCount,
                  color: colors.info,
                  icon: LucideIcons.info,
                ),
              ],
            ],
          ),
        ],
      ),
    );
  }

  Widget _buildIssueCard(NightshadeColors colors, ValidationIssue issue) {
    final Color issueColor;
    final IconData issueIcon;

    switch (issue.severity) {
      case ValidationSeverity.error:
        issueColor = colors.error;
        issueIcon = LucideIcons.xCircle;
        break;
      case ValidationSeverity.warning:
        issueColor = colors.warning;
        issueIcon = LucideIcons.alertTriangle;
        break;
      case ValidationSeverity.info:
        issueColor = colors.info;
        issueIcon = LucideIcons.info;
        break;
    }

    return Container(
      margin: const EdgeInsets.only(bottom: 12),
      padding: const EdgeInsets.all(14),
      decoration: BoxDecoration(
        color: colors.surfaceAlt,
        borderRadius: BorderRadius.circular(10),
        border: Border.all(color: colors.border),
      ),
      child: Row(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Container(
            padding: const EdgeInsets.all(6),
            decoration: BoxDecoration(
              color: issueColor.withValues(alpha: 0.15),
              borderRadius: BorderRadius.circular(6),
            ),
            child: Icon(issueIcon, size: 14, color: issueColor),
          ),
          const SizedBox(width: 12),
          Expanded(
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Row(
                  children: [
                    Text(
                      issue.title,
                      style: TextStyle(
                        fontSize: 13,
                        fontWeight: FontWeight.w600,
                        color: colors.textPrimary,
                      ),
                    ),
                    const SizedBox(width: 8),
                    Container(
                      padding: const EdgeInsets.symmetric(horizontal: 6, vertical: 2),
                      decoration: BoxDecoration(
                        color: colors.surfaceAlt,
                        borderRadius: BorderRadius.circular(4),
                      ),
                      child: Text(
                        issue.category,
                        style: TextStyle(
                          fontSize: 9,
                          fontWeight: FontWeight.w600,
                          color: colors.textMuted,
                          letterSpacing: 0.3,
                        ),
                      ),
                    ),
                  ],
                ),
                const SizedBox(height: 4),
                Text(
                  issue.description,
                  style: TextStyle(
                    fontSize: 12,
                    color: colors.textSecondary,
                  ),
                ),
                if (issue.resolution != null) ...[
                  const SizedBox(height: 8),
                  Row(
                    children: [
                      Icon(LucideIcons.lightbulb, size: 12, color: colors.primary),
                      const SizedBox(width: 6),
                      Expanded(
                        child: Text(
                          issue.resolution!,
                          style: TextStyle(
                            fontSize: 11,
                            color: colors.primary,
                            fontStyle: FontStyle.italic,
                          ),
                        ),
                      ),
                    ],
                  ),
                ],
              ],
            ),
          ),
        ],
      ),
    );
  }

  Widget _buildAllClearCard(NightshadeColors colors) {
    return Container(
      padding: const EdgeInsets.all(24),
      decoration: BoxDecoration(
        color: colors.surfaceAlt,
        borderRadius: BorderRadius.circular(12),
        border: Border.all(color: colors.border),
      ),
      child: Column(
        children: [
          Icon(LucideIcons.sparkles, size: 40, color: colors.success),
          const SizedBox(height: 12),
          Text(
            'Looking Good!',
            style: TextStyle(
              fontSize: 14,
              fontWeight: FontWeight.w600,
              color: colors.textPrimary,
            ),
          ),
          const SizedBox(height: 4),
          Text(
            'No issues found. Your sequence is ready to run.',
            style: TextStyle(
              fontSize: 12,
              color: colors.textSecondary,
            ),
          ),
        ],
      ),
    );
  }

  Widget _buildActions(NightshadeColors colors) {
    final canStart = _result?.isValid ?? false;
    final hasWarningsOnly = _result != null && !_result!.hasErrors && _result!.hasWarnings;

    return Container(
      padding: const EdgeInsets.all(20),
      decoration: BoxDecoration(
        border: Border(top: BorderSide(color: colors.border)),
      ),
      child: Row(
        children: [
          // Refresh button
          TextButton.icon(
            onPressed: () {
              setState(() => _isValidating = true);
              _runValidation();
            },
            icon: Icon(LucideIcons.refreshCw, size: 14, color: colors.textSecondary),
            label: Text(
              'Re-check',
              style: TextStyle(
                fontSize: 12,
                color: colors.textSecondary,
              ),
            ),
          ),

          const Spacer(),

          // Cancel button
          TextButton(
            onPressed: () => Navigator.of(context).pop(),
            child: Text(
              'Cancel',
              style: TextStyle(
                fontSize: 13,
                color: colors.textSecondary,
              ),
            ),
          ),
          const SizedBox(width: 12),

          // Start button
          ElevatedButton.icon(
            onPressed: (canStart || hasWarningsOnly) ? () {
              Navigator.of(context).pop();
              widget.onStartSequence?.call();
            } : null,
            style: ElevatedButton.styleFrom(
              backgroundColor: canStart
                  ? colors.success
                  : hasWarningsOnly
                      ? colors.warning
                      : colors.textMuted,
              foregroundColor: Colors.white,
              padding: const EdgeInsets.symmetric(horizontal: 20, vertical: 12),
              shape: RoundedRectangleBorder(
                borderRadius: BorderRadius.circular(8),
              ),
            ),
            icon: Icon(canStart ? LucideIcons.play : LucideIcons.alertTriangle, size: 16),
            label: Text(
              hasWarningsOnly ? 'Start Anyway' : 'Start Sequence',
              style: const TextStyle(
                fontSize: 13,
                fontWeight: FontWeight.w600,
              ),
            ),
          ),
        ],
      ),
    );
  }
}

/// Small count badge widget
class _CountBadge extends StatelessWidget {
  final int count;
  final Color color;
  final IconData icon;

  const _CountBadge({
    required this.count,
    required this.color,
    required this.icon,
  });

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 4),
      decoration: BoxDecoration(
        color: color.withValues(alpha: 0.15),
        borderRadius: BorderRadius.circular(12),
      ),
      child: Row(
        mainAxisSize: MainAxisSize.min,
        children: [
          Icon(icon, size: 12, color: color),
          const SizedBox(width: 4),
          Text(
            count.toString(),
            style: TextStyle(
              fontSize: 11,
              fontWeight: FontWeight.w700,
              color: color,
            ),
          ),
        ],
      ),
    );
  }
}
