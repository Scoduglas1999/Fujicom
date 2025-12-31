import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:nightshade_ui/nightshade_ui.dart';
import 'package:nightshade_core/nightshade_core.dart';

/// INDI Server Configuration Dialog (Linux/macOS)
class IndiServerDialog extends ConsumerStatefulWidget {
  const IndiServerDialog({super.key});

  @override
  ConsumerState<IndiServerDialog> createState() => _IndiServerDialogState();
}

class _IndiServerDialogState extends ConsumerState<IndiServerDialog> {
  late TextEditingController _hostController;
  late TextEditingController _portController;
  bool _isConnecting = false;
  String? _statusMessage;
  bool? _connectionSuccess;

  @override
  void initState() {
    super.initState();
    // Initialize with default values, will be updated when settings load
    _hostController = TextEditingController(text: 'localhost');
    _portController = TextEditingController(text: '7624');
    
    // Load settings asynchronously
    WidgetsBinding.instance.addPostFrameCallback((_) async {
      final settings = await ref.read(appSettingsProvider.future);
      if (mounted) {
        setState(() {
          _hostController.text = settings.indiServerHost;
          _portController.text = settings.indiServerPort.toString();
        });
      }
    });
  }

  @override
  void dispose() {
    _hostController.dispose();
    _portController.dispose();
    super.dispose();
  }

  Future<void> _testConnection() async {
    setState(() {
      _isConnecting = true;
      _statusMessage = null;
      _connectionSuccess = null;
    });

    final host = _hostController.text.trim();
    final port = int.tryParse(_portController.text) ?? 7624;

    try {
      // Try to discover devices on the server using DeviceService
      final deviceService = ref.read(deviceServiceProvider);
      final devices = await deviceService.discoverIndiAtAddress(host, port);

      if (mounted) {
        setState(() {
          _isConnecting = false;
          _connectionSuccess = true;
          _statusMessage = 'Connected! Found ${devices.length} devices.';
        });
      }
    } catch (e) {
      if (mounted) {
        setState(() {
          _isConnecting = false;
          _connectionSuccess = false;
          _statusMessage = 'Connection failed: $e';
        });
      }
    }
  }

  void _saveAndConnect() {
    final host = _hostController.text.trim();
    final port = int.tryParse(_portController.text) ?? 7624;

    // Save settings
    // Note: We need to implement updateIndiSettings in SettingsProvider or use copyWith
    // For now, assuming we can't easily modify the provider implementation here, 
    // we'll just return the values and let the parent handle it, OR we can try to update if the provider allows.
    // Actually, let's just return the values.
    
    Navigator.pop(context, {
      'host': host,
      'port': port,
    });
  }

  @override
  Widget build(BuildContext context) {
    final colors = Theme.of(context).extension<NightshadeColors>()!;

    return AlertDialog(
      backgroundColor: colors.surface,
      title: Row(
        children: [
          Icon(Icons.power, color: colors.primary, size: 24),
          const SizedBox(width: 12),
          Text(
            'INDI Server Configuration',
            style: TextStyle(color: colors.textPrimary),
          ),
        ],
      ),
      content: SizedBox(
        width: 400,
        child: Column(
          mainAxisSize: MainAxisSize.min,
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            // Info about INDI
            Container(
              padding: const EdgeInsets.all(12),
              decoration: BoxDecoration(
                color: colors.primary.withValues(alpha: 0.1),
                borderRadius: BorderRadius.circular(8),
                border: Border.all(color: colors.primary.withValues(alpha: 0.3)),
              ),
              child: Row(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Icon(Icons.info_outline, color: colors.primary, size: 16),
                  const SizedBox(width: 10),
                  Expanded(
                    child: Text(
                      'INDI (Instrument Neutral Distributed Interface) provides '
                      'cross-platform access to astronomical equipment on Linux and macOS.',
                      style: TextStyle(
                        fontSize: 11,
                        color: colors.textSecondary,
                      ),
                    ),
                  ),
                ],
              ),
            ),
            const SizedBox(height: 20),

            // Host input
            TextField(
              controller: _hostController,
              style: TextStyle(color: colors.textPrimary),
              decoration: InputDecoration(
                labelText: 'INDI Server Host',
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
            const SizedBox(height: 16),

            // Port input
            TextField(
              controller: _portController,
              style: TextStyle(color: colors.textPrimary),
              keyboardType: TextInputType.number,
              decoration: InputDecoration(
                labelText: 'Port',
                labelStyle: TextStyle(color: colors.textMuted),
                hintText: '7624 (default)',
                hintStyle: TextStyle(color: colors.textMuted.withValues(alpha: 0.5)),
                enabledBorder: OutlineInputBorder(
                  borderSide: BorderSide(color: colors.border),
                ),
                focusedBorder: OutlineInputBorder(
                  borderSide: BorderSide(color: colors.primary),
                ),
              ),
            ),
            const SizedBox(height: 16),

            // Test connection button
            SizedBox(
              width: double.infinity,
              child: OutlinedButton.icon(
                onPressed: _isConnecting ? null : _testConnection,
                icon: _isConnecting
                    ? SizedBox(
                        width: 16,
                        height: 16,
                        child: CircularProgressIndicator(
                          strokeWidth: 2,
                          color: colors.primary,
                        ),
                      )
                    : const Icon(Icons.cloud_sync, size: 16),
                label: Text(_isConnecting ? 'Testing...' : 'Test Connection'),
                style: OutlinedButton.styleFrom(
                  foregroundColor: colors.textPrimary,
                  side: BorderSide(color: colors.border),
                  padding: const EdgeInsets.symmetric(vertical: 12),
                ),
              ),
            ),

            // Status message
            if (_statusMessage != null) ...[
              const SizedBox(height: 12),
              Container(
                padding: const EdgeInsets.all(10),
                decoration: BoxDecoration(
                  color: (_connectionSuccess ?? false)
                      ? colors.success.withValues(alpha: 0.1)
                      : colors.error.withValues(alpha: 0.1),
                  borderRadius: BorderRadius.circular(6),
                  border: Border.all(
                    color: (_connectionSuccess ?? false)
                        ? colors.success.withValues(alpha: 0.3)
                        : colors.error.withValues(alpha: 0.3),
                  ),
                ),
                child: Row(
                  children: [
                    Icon(
                      (_connectionSuccess ?? false) ? Icons.check_circle : Icons.error,
                      size: 16,
                      color: (_connectionSuccess ?? false) ? colors.success : colors.error,
                    ),
                    const SizedBox(width: 8),
                    Expanded(
                      child: Text(
                        _statusMessage!,
                        style: TextStyle(
                          fontSize: 11,
                          color: colors.textSecondary,
                        ),
                      ),
                    ),
                  ],
                ),
              ),
            ],
          ],
        ),
      ),
      actions: [
        TextButton(
          onPressed: () => Navigator.pop(context),
          child: Text('Cancel', style: TextStyle(color: colors.textMuted)),
        ),
        FilledButton(
          onPressed: _connectionSuccess == true ? _saveAndConnect : null,
          style: FilledButton.styleFrom(
            backgroundColor: colors.primary,
            disabledBackgroundColor: colors.surfaceAlt,
          ),
          child: const Text('Connect'),
        ),
      ],
    );
  }
}
