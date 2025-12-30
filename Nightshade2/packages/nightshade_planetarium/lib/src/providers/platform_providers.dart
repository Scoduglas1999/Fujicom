import 'dart:io';
import 'package:flutter/foundation.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';

/// Whether we're on a touch-primary device
final isTouchDeviceProvider = Provider<bool>((ref) {
  if (kIsWeb) return false;
  return Platform.isIOS || Platform.isAndroid;
});

/// Whether hover interactions are available
final hasHoverProvider = Provider<bool>((ref) {
  if (kIsWeb) return true;
  return Platform.isWindows || Platform.isMacOS || Platform.isLinux;
});

/// Whether right-click context menus are expected
final hasContextMenuProvider = Provider<bool>((ref) {
  return ref.watch(hasHoverProvider);
});
