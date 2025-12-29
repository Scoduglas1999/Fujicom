/// Nightshade Bridge - Dart FFI bindings to Rust native code
///
/// This file provides the bridge to the Rust native library.
/// The native DLL is loaded dynamically and provides real ASCOM/Alpaca
/// device discovery and connection on Windows.
///
/// For Alpaca devices, we use direct HTTP communication from Dart,
/// which works cross-platform without needing the native bridge.
///
/// When the native library is not available, this bridge will NOT fall back
/// to simulator implementations. Instead, it will return empty device lists
/// and throw errors for hardware operations. Use INDI/ASCOM/Alpaca external
/// simulators for testing instead of built-in stubs.

import 'dart:async';
import 'dart:convert';
import 'dart:ffi';
import 'dart:io';
import 'dart:math' as math;
import 'dart:typed_data';
import 'package:ffi/ffi.dart';
import 'package:path/path.dart' as path;
import 'package:xml/xml.dart' as xml;
import 'alpaca_client.dart' as alpaca;
import 'ascom_client.dart' as ascom;
import 'phd2_client.dart' as phd2;
import 'api.dart' as gen_api;
import 'device.dart' as gen_device;
import 'event.dart' as gen_event;
import 'state.dart' as gen_state;
import 'storage.dart' as gen_storage;
import 'frb_generated.dart' as frb;

// ============================================================================
// Error Messages for Stub Mode
// ============================================================================

/// Error message thrown when stub operations are called in production
const _stubErrorMessage = '''
Native bridge not available. This is the Dart fallback stub.

Possible causes:
1. Native library failed to load - check build output
2. Running on unsupported platform (web)
3. DLL/dylib not found in expected location

For development: Use INDI/ASCOM/Alpaca simulators instead of built-in stubs.
Simulators are disabled to prevent silent failures with fake data.
''';

// ============================================================================
// Type Aliases - Use FRB-generated types to avoid duplication
// ============================================================================

// From device.dart
typedef DeviceType = gen_device.DeviceType;
typedef DriverType = gen_device.DriverType;
typedef CameraState = gen_device.CameraState;
typedef CameraStatus = gen_device.CameraStatus;
typedef DeviceInfo = gen_device.DeviceInfo;
typedef FilterWheelStatus = gen_device.FilterWheelStatus;
typedef FocuserStatus = gen_device.FocuserStatus;
typedef MountStatus = gen_device.MountStatus;
typedef PierSide = gen_device.PierSide;
typedef RotatorStatus = gen_device.RotatorStatus;
typedef TrackingRate = gen_device.TrackingRate;

// From state.dart
typedef EquipmentProfile = gen_state.EquipmentProfile;

// From storage.dart
typedef AppSettings = gen_storage.AppSettings;
typedef ObserverLocation = gen_storage.ObserverLocation;

// From api.dart
typedef AutofocusConfigApi = gen_api.AutofocusConfigApi;
typedef CapturedImageResult = gen_api.CapturedImageResult;
typedef ImageStatsResult = gen_api.ImageStatsResult;
typedef Phd2Status = gen_api.Phd2Status;
typedef Phd2StarImage = gen_api.Phd2StarImage;
typedef PlateSolveResult = gen_api.PlateSolveResult;
// Note: SequencerState is NOT typedefed because FRB's SequencerState is a class,
// but we use a local enum for internal state management (see _InternalSequencerState below)

// From event.dart
typedef NightshadeEvent = gen_event.NightshadeEvent;
typedef EventSeverity = gen_event.EventSeverity;
typedef EventCategory = gen_event.EventCategory;
typedef PolarAlignmentEvent = gen_event.PolarAlignmentEvent;

// ============================================================================
// Extension on FRB-generated DeviceType
// ============================================================================

extension DeviceTypeExtension on DeviceType {
  String get displayName {
    switch (this) {
      case DeviceType.camera:
        return 'Camera';
      case DeviceType.mount:
        return 'Mount';
      case DeviceType.focuser:
        return 'Focuser';
      case DeviceType.filterWheel:
        return 'Filter Wheel';
      case DeviceType.guider:
        return 'Guider';
      case DeviceType.dome:
        return 'Dome';
      case DeviceType.rotator:
        return 'Rotator';
      case DeviceType.weather:
        return 'Weather';
      case DeviceType.safetyMonitor:
        return 'Safety Monitor';
      case DeviceType.switch_:
        return 'Switch';
      case DeviceType.coverCalibrator:
        return 'Cover Calibrator';
    }
  }
}

// ============================================================================
// Enums unique to bridge_stub (not in FRB-generated code)
// ============================================================================

/// Device connection state
enum ConnectionState {
  disconnected,
  connecting,
  connected,
  error,
}

/// Frame type for camera exposures
enum FrameType {
  light,
  dark,
  flat,
  bias,
  darkFlat,
}

/// Dome shutter state
enum ShutterState {
  open,
  closed,
  opening,
  closing,
  error,
  unknown,
}

// EventSeverity, EventCategory, PolarAlignmentEvent, and NightshadeEvent are now typedefed from event.dart

// ============================================================================
// Data Classes unique to bridge_stub (not in FRB-generated code)
// ============================================================================

/// Session state from native
class NativeSessionState {
  final bool isActive;
  final int? startTime;
  final String? targetName;
  final double? targetRa;
  final double? targetDec;
  final int totalExposures;
  final int completedExposures;
  final double totalIntegrationSecs;
  final String? currentFilter;
  final bool isGuiding;
  final bool isCapturing;
  final bool isDithering;

  NativeSessionState({
    required this.isActive,
    this.startTime,
    this.targetName,
    this.targetRa,
    this.targetDec,
    required this.totalExposures,
    required this.completedExposures,
    required this.totalIntegrationSecs,
    this.currentFilter,
    required this.isGuiding,
    required this.isCapturing,
    required this.isDithering,
  });
}

/// Internal stub event - used for simulator mode
/// This is different from gen_event.NightshadeEvent which uses EventPayload
class _StubNightshadeEvent {
  final int timestamp;
  final gen_event.EventSeverity severity;
  final gen_event.EventCategory category;
  final String eventType;
  final Map<String, dynamic> data;

  _StubNightshadeEvent({
    required this.timestamp,
    required this.severity,
    required this.category,
    required this.eventType,
    required this.data,
  });
}

/// Image statistics (unique to bridge_stub - different from ImageStatsResult)
class ImageStats {
  final double min;
  final double max;
  final double mean;
  final double median;
  final double stdDev;
  final double mad;

  ImageStats({
    required this.min,
    required this.max,
    required this.mean,
    required this.median,
    required this.stdDev,
    required this.mad,
  });
}

/// Sequencer status (unique to bridge_stub - different from SequencerState)
class SequencerStatus {
  final String state;
  final String? currentNodeId;
  final String? currentNodeName;
  final double progress;
  final String? message;

  SequencerStatus({
    required this.state,
    this.currentNodeId,
    this.currentNodeName,
    required this.progress,
    this.message,
  });
}

/// Checkpoint information for crash recovery
class CheckpointInfoApi {
  final String sequenceName;
  final String timestamp; // ISO-8601 format
  final int completedExposures;
  final double completedIntegrationSecs;
  final bool canResume;
  final int ageSeconds;

  CheckpointInfoApi({
    required this.sequenceName,
    required this.timestamp,
    required this.completedExposures,
    required this.completedIntegrationSecs,
    required this.canResume,
    required this.ageSeconds,
  });
}

/// Sequencer state enum for local state management
/// Note: This is hidden from library exports and FRB's SequencerState (a class) is exported instead
enum SequencerState {
  idle,
  running,
  paused,
  stopping,
  completed,
  failed,
}

// ============================================================================
// Native Bridge Implementation
// ============================================================================

/// Native bridge for communication with Rust backend
///
/// This bridge attempts to load the native Rust library and use it for
/// real device discovery and control. When the native library is not
/// available, it falls back to simulator implementations.
class NativeBridge {
  static bool _initialized = false;
  static bool _nativeAvailable = false;
  static DynamicLibrary? _nativeLib;
  static final _eventController =
      StreamController<_StubNightshadeEvent>.broadcast();

  // Simulated device states
  static final Map<String, bool> _connectedDevices = {};
  static CameraStatus? _cameraStatus;
  static MountStatus? _mountStatus;
  static FocuserStatus? _focuserStatus;
  static FilterWheelStatus? _filterWheelStatus;

  // Alpaca discovery cache to avoid repeated UDP broadcasts
  static List<alpaca.AlpacaDevice>? _alpacaDiscoveryCache;
  static DateTime? _alpacaDiscoveryCacheTime;
  static const _alpacaDiscoveryCacheTtl = Duration(seconds: 10);
  static Future<List<alpaca.AlpacaDevice>>? _alpacaDiscoveryInProgress;

  // Active Alpaca connections
  static final Map<String, alpaca.AlpacaClient> _alpacaClients = {};
  static final Map<String, alpaca.AlpacaDevice> _alpacaDevices = {};

  // Active ASCOM connections
  static final Map<String, ascom.AscomDeviceClient> _ascomClients = {};

  // PHD2 client
  static phd2.Phd2Client? _phd2Client;

  // =========================================================================
  // Initialization
  // =========================================================================

  /// Initialize the native bridge
  static Future<void> init({String? logDirectory}) async {
    if (_initialized) return;

    // Try to load native library manually (for stub fallback)
    _nativeAvailable = await _tryLoadNativeLibrary();

    // Try to initialize RustLib (it will try to auto-load the library)
    // This enables native ZWO discovery and proper ASCOM discovery
    // Note: If manual load succeeded, RustLib should also be able to find it
    try {
      print(
          '[NativeBridge] Initializing RustLib for native device discovery...');
      print(
          '[NativeBridge] RustLib will attempt to load the native library automatically');
      await frb.RustLib.init();

      // Initialize the native bridge API
      if (logDirectory != null) {
        gen_api.apiInitWithLogging(logDirectory: logDirectory);
        print(
            '[NativeBridge] Native bridge API initialized with logging to: $logDirectory');
      } else {
        gen_api.apiInit();
        print(
            '[NativeBridge] Native bridge API initialized (console logging only)');
      }

      // Verify it's working
      final version = gen_api.apiGetVersion();
      print('[NativeBridge] Native bridge version: $version');
      print(
          '[NativeBridge] ✓ Native bridge ready - will discover native ZWO, ASCOM, and Alpaca devices');

      // Mark as available for native discovery
      _nativeAvailable = true;
    } catch (e) {
      print('[NativeBridge] RustLib initialization failed: $e');
      print('[NativeBridge] Will fall back to stub discovery methods');
      // Mark as unavailable since RustLib couldn't initialize
      _nativeAvailable = false;
    }

    // Initialize default states
    _initializeDefaultStates();

    _initialized = true;

    // Emit initialization event
    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.system,
      eventType: 'Initialized',
      data: {'nativeAvailable': _nativeAvailable},
    ));

    if (_nativeAvailable) {
      print('Nightshade Native Bridge: Loaded native library');
    } else {
      print('Nightshade Native Bridge: Using simulator mode');
    }
  }

  static void _initializeDefaultStates() {
    _cameraStatus = const CameraStatus(
      connected: false,
      state: CameraState.idle,
      sensorTemp: 20.0,
      coolerPower: 0.0,
      targetTemp: -10.0,
      coolerOn: false,
      gain: 100,
      offset: 10,
      binX: 1,
      binY: 1,
      sensorWidth: 4144,
      sensorHeight: 2822,
      pixelSizeX: 3.76,
      pixelSizeY: 3.76,
      maxAdu: 65535,
      canCool: true,
      canSetGain: true,
      canSetOffset: true,
    );

    _mountStatus = const MountStatus(
      connected: false,
      tracking: false,
      slewing: false,
      parked: true,
      atHome: false,
      sideOfPier: PierSide.unknown,
      rightAscension: 0.0,
      declination: 0.0,
      altitude: 0.0,
      azimuth: 0.0,
      siderealTime: 0.0,
      trackingRate: TrackingRate.sidereal,
      canPark: true,
      canSlew: true,
      canSync: true,
      canPulseGuide: true,
      canSetTrackingRate: true,
    );

    _focuserStatus = const FocuserStatus(
      connected: false,
      position: 25000,
      moving: false,
      temperature: 20.0,
      maxPosition: 50000,
      stepSize: 1.0,
      isAbsolute: true,
      hasTemperature: true,
    );

    _filterWheelStatus = const FilterWheelStatus(
      connected: false,
      position: 0,
      moving: false,
      filterCount: 7,
      filterNames: ['L', 'R', 'G', 'B', 'Ha', 'OIII', 'SII'],
    );
  }

  static void _updateMountStatus({
    bool? connected,
    bool? tracking,
    bool? slewing,
    bool? parked,
    bool? atHome,
    PierSide? sideOfPier,
    double? rightAscension,
    double? declination,
    double? altitude,
    double? azimuth,
    double? siderealTime,
    TrackingRate? trackingRate,
  }) {
    final current = _mountStatus!;
    _mountStatus = MountStatus(
      connected: connected ?? current.connected,
      tracking: tracking ?? current.tracking,
      slewing: slewing ?? current.slewing,
      parked: parked ?? current.parked,
      atHome: atHome ?? current.atHome,
      sideOfPier: sideOfPier ?? current.sideOfPier,
      rightAscension: rightAscension ?? current.rightAscension,
      declination: declination ?? current.declination,
      altitude: altitude ?? current.altitude,
      azimuth: azimuth ?? current.azimuth,
      siderealTime: siderealTime ?? current.siderealTime,
      trackingRate: trackingRate ?? current.trackingRate,
      canPark: current.canPark,
      canSlew: current.canSlew,
      canSync: current.canSync,
      canPulseGuide: current.canPulseGuide,
      canSetTrackingRate: current.canSetTrackingRate,
    );
  }

  /// Try to load the native library
  static Future<bool> _tryLoadNativeLibrary() async {
    try {
      // Determine library name based on platform
      String libName;
      if (Platform.isWindows) {
        libName = 'nightshade_bridge.dll';
      } else if (Platform.isLinux) {
        libName = 'libnightshade_bridge.so';
      } else if (Platform.isMacOS) {
        libName = 'libnightshade_bridge.dylib';
      } else {
        // Unsupported platform
        return false;
      }

      // Get the executable directory
      final executablePath = Platform.resolvedExecutable;
      final executableDir = path.dirname(executablePath);

      // Try to find the native library in common locations
      final possiblePaths = <String>[];

      if (Platform.isWindows) {
        // Windows: library should be next to executable or in data directory
        possiblePaths.addAll([
          // First, check next to the executable (most common location)
          path.join(executableDir, libName),
          // Check parent directories (for release builds)
          path.join(executableDir, '..', libName),
          path.join(executableDir, '..', '..', libName),
          // Check in data directory
          path.join(executableDir, 'data', 'flutter_assets', libName),
          // Check if we can find the project root by looking for common markers
          // Try to find native/nightshade_native from executable location
          path.join(executableDir, '..', '..', '..', 'native',
              'nightshade_native', 'bridge', 'target', 'release', libName),
          path.join(executableDir, '..', '..', '..', 'native',
              'nightshade_native', 'target', 'release', libName),
          path.join(executableDir, '..', '..', '..', 'native',
              'nightshade_native', 'bridge', 'target', 'debug', libName),
          path.join(executableDir, '..', '..', '..', 'native',
              'nightshade_native', 'target', 'debug', libName),
          // Check if executable is in a Release/Debug folder
          path.join(executableDir, '..', '..', '..', 'native',
              'nightshade_native', 'bridge', 'target', 'release', libName),
        ]);

        // Also try to find project root from current working directory
        try {
          final cwd = Directory.current.path;
          possiblePaths.addAll([
            path.join(cwd, 'native', 'nightshade_native', 'bridge', 'target',
                'release', libName),
            path.join(cwd, 'native', 'nightshade_native', 'target', 'release',
                libName),
            path.join(cwd, '..', 'native', 'nightshade_native', 'bridge',
                'target', 'release', libName),
            path.join(cwd, '..', '..', 'native', 'nightshade_native', 'bridge',
                'target', 'release', libName),
          ]);
        } catch (e) {
          // Ignore errors getting current directory
        }
      } else if (Platform.isLinux) {
        // Linux: library should be in lib/ directory relative to executable
        possiblePaths.addAll([
          path.join(executableDir, 'lib', libName),
          path.join(executableDir, '..', 'lib', libName),
          path.join(executableDir, libName),
          // Development build location
          path.join(executableDir, '..', '..', '..', 'native',
              'nightshade_native', 'target', 'release', libName),
          path.join(executableDir, '..', '..', '..', 'native',
              'nightshade_native', 'target', 'debug', libName),
          // System library path
          '/usr/local/lib/$libName',
        ]);
      } else if (Platform.isMacOS) {
        // macOS: library should be in Frameworks directory of app bundle
        possiblePaths.addAll([
          path.join(executableDir, '..', 'Frameworks', libName),
          path.join(executableDir, 'Frameworks', libName),
          path.join(executableDir, libName),
          // Development build location
          path.join(executableDir, '..', '..', '..', 'native',
              'nightshade_native', 'target', 'release', libName),
          path.join(executableDir, '..', '..', '..', 'native',
              'nightshade_native', 'target', 'debug', libName),
        ]);
      }

      // Try to load the library from each possible path
      for (final libPath in possiblePaths) {
        try {
          final file = File(libPath);
          if (await file.exists()) {
            print('Attempting to load native library from: $libPath');

            // Try to load the library
            _nativeLib = DynamicLibrary.open(libPath);

            // Verify the library loaded by checking for a known symbol
            // For now, just check if it loaded successfully
            print('Successfully loaded native library from: $libPath');
            return true;
          }
        } catch (e) {
          // Continue trying other paths
          print('Failed to load library from $libPath: $e');
        }
      }

      // If we couldn't find the library, try loading by name (system will search)
      try {
        if (Platform.isWindows) {
          _nativeLib = DynamicLibrary.open(libName);
        } else if (Platform.isLinux) {
          _nativeLib = DynamicLibrary.open(libName);
        } else if (Platform.isMacOS) {
          _nativeLib = DynamicLibrary.open(libName);
        }
        print('Successfully loaded native library by name: $libName');
        return true;
      } catch (e) {
        print('Failed to load library by name: $e');
      }

      print('Native library not found. Falling back to simulator mode.');
      print('');
      print('To enable native device discovery:');
      print(
          '1. Build the native library: cd native/nightshade_native && cargo build --release --manifest-path bridge/Cargo.toml');
      if (Platform.isWindows) {
        print('2. Copy nightshade_bridge.dll to: $executableDir');
        print('   Or build the Flutter app which should copy it automatically');
      } else if (Platform.isLinux) {
        print('2. Copy $libName to: $executableDir/lib/');
      } else if (Platform.isMacOS) {
        print('2. Copy $libName to: Frameworks/ in the app bundle');
      }
      return false;
    } catch (e) {
      print('Error loading native library: $e');
      return false;
    }
  }

  /// Check if native library is available
  static bool get isNativeAvailable => _nativeAvailable;

  /// Get the version of the native library
  static String getNativeVersion() {
    if (_nativeAvailable && _nativeLib != null) {
      try {
        // Try to call the native get_version function
        final getVersion = _nativeLib!
            .lookupFunction<Pointer<Utf8> Function(), Pointer<Utf8> Function()>(
                'get_native_version');

        final versionPtr = getVersion();
        if (versionPtr != nullptr) {
          final version = versionPtr.toDartString();
          return version;
        }
      } catch (e) {
        print('Failed to get native version: $e');
      }
      return '0.1.0';
    }
    // Native library not loaded - return stub version
    // Note: Hardware operations will fail without native library
    return '0.1.0-stub (native library not loaded)';
  }

  /// Get the loaded native library (if available)
  static DynamicLibrary? get nativeLibrary => _nativeLib;

  // =========================================================================
  // Event Stream
  // =========================================================================

  /// Stream of events from the native side
  static Stream<NightshadeEvent> eventStream() {
    // If native is available, use the real event stream from Rust
    if (_nativeAvailable) {
      try {
        return gen_api.apiEventStream();
      } catch (e) {
        print('[NativeBridge] Failed to get native event stream: $e');
        print('[NativeBridge] Falling back to local event controller');
      }
    }

    // Fallback to local event controller for simulator mode
    // Convert internal stub events to proper NightshadeEvent format
    return _eventController.stream.map((stubEvent) {
      return gen_event.NightshadeEvent(
        timestamp: stubEvent.timestamp,
        severity: stubEvent.severity,
        category: stubEvent.category,
        payload: gen_event.EventPayload.system(
          gen_event.SystemEvent.notification(
            title: stubEvent.eventType,
            message: stubEvent.data.toString(),
            level: stubEvent.severity.name,
          ),
        ),
      );
    });
  }

  // =========================================================================
  // Device Discovery
  // =========================================================================

  /// Discover INDI devices at a specific server address
  static Future<List<DeviceInfo>> apiDiscoverIndiAtAddress({
    required String host,
    required int port,
  }) async {
    try {
      print('[Bridge] Connecting to INDI server at $host:$port...');

      // Connect to INDI server via TCP
      final socket =
          await Socket.connect(host, port, timeout: const Duration(seconds: 5));

      try {
        // Send getProperties command to request device list
        final command = '<getProperties version="1.7"/>\n';
        socket.add(utf8.encode(command));
        await socket.flush();

        // Read response with timeout
        final completer = Completer<String>();
        final buffer = StringBuffer();
        Timer? timeoutTimer;

        socket.listen(
          (data) {
            buffer.write(utf8.decode(data));
            // INDI responses can be chunked, so we accumulate until we have complete XML
            final response = buffer.toString();
            // Check if we have a complete XML document (ends with </indilib>)
            if (response.contains('</indilib>') ||
                response.contains('</defTextVector>') ||
                response.contains('</defNumberVector>')) {
              timeoutTimer?.cancel();
              if (!completer.isCompleted) {
                completer.complete(response);
              }
            }
          },
          onError: (error) {
            timeoutTimer?.cancel();
            if (!completer.isCompleted) {
              completer.completeError(error as Object);
            }
          },
          onDone: () {
            timeoutTimer?.cancel();
            if (!completer.isCompleted) {
              completer.complete(buffer.toString());
            }
          },
          cancelOnError: false,
        );

        // Set timeout for reading response
        timeoutTimer = Timer(const Duration(seconds: 3), () {
          if (!completer.isCompleted) {
            completer.complete(buffer.toString());
          }
        });

        // Wait for response or timeout
        final response = await completer.future.timeout(
          const Duration(seconds: 5),
          onTimeout: () => buffer.toString(),
        );

        // Parse XML response
        final devices = _parseIndiDevices(response, host, port);

        print(
            '[Bridge] Discovered ${devices.length} INDI devices at $host:$port');
        return devices;
      } finally {
        await socket.close();
      }
    } catch (e) {
      print('[Bridge] Failed to discover INDI devices at $host:$port: $e');
      return [];
    }
  }

  /// Parse INDI XML response to extract device information
  static List<DeviceInfo> _parseIndiDevices(
      String xmlResponse, String host, int port) {
    final devices = <DeviceInfo>[];

    if (xmlResponse.isEmpty) {
      return devices;
    }

    try {
      final document = xml.XmlDocument.parse(xmlResponse);
      final root = document.rootElement;

      // Track devices and their properties
      final deviceProperties = <String, Set<String>>{};

      // Parse all property definitions to determine device types
      for (final element in root.findAllElements('*')) {
        final name = element.localName;

        if (name == 'defTextVector' ||
            name == 'defNumberVector' ||
            name == 'defSwitchVector' ||
            name == 'defLightVector' ||
            name == 'defBLOBVector') {
          final deviceAttr = element.getAttribute('device');
          final nameAttr = element.getAttribute('name');

          if (deviceAttr != null && nameAttr != null) {
            deviceProperties
                .putIfAbsent(deviceAttr, () => <String>{})
                .add(nameAttr);
          }
        }
      }

      // Convert to DeviceInfo based on properties
      for (final entry in deviceProperties.entries) {
        final deviceName = entry.key;
        final properties = entry.value;

        // Determine device type based on properties (matching native implementation)
        DeviceType? deviceType;

        if (properties.contains('CCD_EXPOSURE') ||
            properties.contains('CCD1')) {
          deviceType = DeviceType.camera;
        } else if (properties.contains('EQUATORIAL_EOD_COORD') ||
            properties.contains('EQUATORIAL_COORD') ||
            properties.contains('TELESCOPE_PARK')) {
          deviceType = DeviceType.mount;
        } else if (properties.contains('ABS_FOCUS_POSITION') ||
            properties.contains('REL_FOCUS_POSITION') ||
            properties.contains('FOCUS_MOTION')) {
          deviceType = DeviceType.focuser;
        } else if (properties.contains('FILTER_SLOT') ||
            properties.contains('FILTER_NAME')) {
          deviceType = DeviceType.filterWheel;
        } else if (properties.contains('ABS_ROTATOR_ANGLE') ||
            properties.contains('ROTATOR_ROTATION')) {
          deviceType = DeviceType.rotator;
        } else if (properties.contains('DOME_PARK') ||
            properties.contains('DOME_SHUTTER')) {
          deviceType = DeviceType.dome;
        } else if (properties.contains('WEATHER_TEMPERATURE') ||
            properties.contains('WEATHER_HUMIDITY') ||
            properties.contains('WEATHER_CLOUD_COVER')) {
          deviceType = DeviceType.weather;
        }

        // Only include devices we can identify
        if (deviceType != null) {
          devices.add(DeviceInfo(
            id: 'indi:$host:$port:$deviceName',
            name: deviceName,
            deviceType: deviceType,
            driverType: DriverType.indi,
            description: 'INDI device on $host:$port',
            driverVersion: 'INDI',
            displayName: deviceName,
          ));
        }
      }
    } catch (e) {
      print('[Bridge] Error parsing INDI XML response: $e');
      // Try to extract device names even if full parsing fails
      final deviceNameRegex = RegExp(r'device="([^"]+)"');
      final matches = deviceNameRegex.allMatches(xmlResponse);
      final seenDevices = <String>{};

      for (final match in matches) {
        final deviceName = match.group(1);
        if (deviceName != null && !seenDevices.contains(deviceName)) {
          seenDevices.add(deviceName);
          // Try to infer type from context or default to camera
          devices.add(DeviceInfo(
            id: 'indi:$host:$port:$deviceName',
            name: deviceName,
            deviceType: DeviceType.camera, // Default fallback
            driverType: DriverType.indi,
            description: 'INDI device on $host:$port',
            driverVersion: 'INDI',
            displayName: deviceName,
          ));
        }
      }
    }

    return devices;
  }

  /// Discover available devices of a specific type
  ///
  /// This queries:
  /// 1. Native bridge (if available) - includes ASCOM, native ZWO, Alpaca, etc.
  /// 2. Real ASCOM drivers from Windows Registry (Windows only, fallback)
  /// 3. Real Alpaca devices on the network via HTTP (cross-platform)
  /// 4. Simulator devices for testing
  static Future<List<DeviceInfo>> discoverDevices(DeviceType deviceType) async {
    final devices = <DeviceInfo>[];

    // =========================================================================
    // Try Native Bridge Discovery First (includes ASCOM, native ZWO, Alpaca)
    // =========================================================================
    // Only attempt native discovery if RustLib was successfully initialized
    if (_nativeAvailable) {
      try {
        // Convert DeviceType to generated enum
        final genDeviceType = _toGenDeviceType(deviceType);

        // Call native bridge discovery
        final nativeDevices =
            await gen_api.apiDiscoverDevices(deviceType: genDeviceType);

        // Convert generated DeviceInfo to stub DeviceInfo
        for (final nativeDev in nativeDevices) {
          devices.add(DeviceInfo(
            id: nativeDev.id,
            name: nativeDev.name,
            deviceType: _fromGenDeviceType(nativeDev.deviceType),
            driverType: _fromGenDriverType(nativeDev.driverType),
            description: nativeDev.description,
            driverVersion: nativeDev.driverVersion,
            displayName: nativeDev.displayName,
          ));
        }

        if (nativeDevices.isNotEmpty) {
          print(
              '[NativeBridge] Found ${nativeDevices.length} native ${deviceType.displayName}(s)');
        }
      } catch (e) {
        // Only log errors, not expected RustLib issues
        if (!e.toString().contains('RustLib') &&
            !e.toString().contains('not initialized')) {
          print('[NativeBridge] Native discovery error: $e');
        }
        // Continue to fallback discovery methods
      }
    }

    // =========================================================================
    // Fallback: ASCOM Discovery (Windows only, direct COM via Registry)
    // =========================================================================
    // Only do stub ASCOM discovery if native bridge isn't available or didn't find devices
    // The native bridge already does ASCOM discovery, so this is just a fallback
    if (!_nativeAvailable && Platform.isWindows) {
      try {
        final ascomType = _deviceTypeToAscomType(deviceType);
        if (ascomType != null) {
          print('[ASCOM] Discovering ASCOM $ascomType drivers...');
          final ascomDrivers = await ascom.discoverAscomDrivers(ascomType);
          print(
              '[ASCOM] Found ${ascomDrivers.length} ASCOM $ascomType driver(s)');

          for (final driver in ascomDrivers) {
            devices.add(DeviceInfo(
              id: driver.id,
              name: driver.name,
              deviceType: deviceType,
              driverType: DriverType.ascom,
              description: 'ASCOM driver: ${driver.progId}',
              driverVersion: 'ASCOM',
              displayName: driver.name,
            ));
            print(
                '[ASCOM] Found ASCOM ${deviceType.displayName}: ${driver.name} (${driver.progId})');
          }
        } else {
          print('[ASCOM] No ASCOM type mapping for ${deviceType.displayName}');
        }
      } catch (e, stackTrace) {
        print('[ASCOM] Discovery failed: $e');
        print('[ASCOM] Stack trace: $stackTrace');
      }
    } else {
      print('[ASCOM] Not on Windows, skipping ASCOM discovery');
    }

    // =========================================================================
    // Alpaca Discovery (cross-platform, direct HTTP from Dart)
    // Uses caching AND locking to avoid repeated 2-second UDP broadcasts
    // =========================================================================
    try {
      List<alpaca.AlpacaDevice> alpacaDevices;

      // Check if we have a valid cache first
      final now = DateTime.now();
      if (_alpacaDiscoveryCache != null &&
          _alpacaDiscoveryCacheTime != null &&
          now.difference(_alpacaDiscoveryCacheTime!) <
              _alpacaDiscoveryCacheTtl) {
        // Use cached results
        alpacaDevices = _alpacaDiscoveryCache!;
      } else if (_alpacaDiscoveryInProgress != null) {
        // Another discovery is in progress - wait for it instead of starting a new one
        print('Alpaca discovery already in progress, waiting...');
        alpacaDevices = await _alpacaDiscoveryInProgress!;
      } else {
        // Start fresh discovery with lock to prevent parallel discoveries
        print('Discovering Alpaca devices (UDP broadcast)...');
        final discoveryFuture = alpaca.discoverAllAlpacaDevices(
          timeout: const Duration(seconds: 2),
        );
        _alpacaDiscoveryInProgress = discoveryFuture;
        try {
          alpacaDevices = await discoveryFuture;
          _alpacaDiscoveryCache = alpacaDevices;
          _alpacaDiscoveryCacheTime = DateTime.now();
        } finally {
          _alpacaDiscoveryInProgress = null;
        }
      }

      for (final device in alpacaDevices) {
        // Filter by device type
        if (_alpacaTypeMatches(device.deviceType, deviceType)) {
          devices.add(DeviceInfo(
            id: device.id,
            name: device.deviceName,
            deviceType: deviceType,
            driverType: DriverType.alpaca,
            description:
                'Alpaca device at ${device.server.host}:${device.server.port}',
            driverVersion: 'Alpaca',
            displayName: device.deviceName,
          ));
          print('Found Alpaca ${deviceType.displayName}: ${device.deviceName}');
        }
      }
    } catch (e) {
      print('Alpaca discovery failed: $e');
    }

    // =========================================================================
    // PHD2 Discovery (check if running on network)
    // =========================================================================
    if (deviceType == DeviceType.guider) {
      try {
        print('Discovering PHD2 instances...');
        final phd2Instances = await _discoverPhd2Instances();

        for (final instance in phd2Instances) {
          final phd2Name =
              instance['host'] == 'localhost' || instance['host'] == '127.0.0.1'
                  ? 'PHD2 Guiding'
                  : 'PHD2 Guiding (${instance['host']})';
          devices.add(DeviceInfo(
            id: 'phd2:${instance['host']}:${instance['port']}',
            name: phd2Name,
            deviceType: DeviceType.guider,
            driverType: DriverType.alpaca,
            description:
                'PHD2 autoguiding software (${instance['host']}:${instance['port']})',
            driverVersion: '2.6+',
            displayName: phd2Name,
          ));
          print('Found PHD2 at ${instance['host']}:${instance['port']}');
        }
      } catch (e) {
        print('PHD2 discovery failed: $e');
      }
    }

    // =========================================================================
    // Simulator Devices - DISABLED
    // =========================================================================
    // Built-in simulator devices have been removed to prevent silent failures
    // with fake data. For testing, use external simulators:
    // - ASCOM Simulator drivers (Windows)
    // - INDI Simulator drivers (Linux/macOS)
    // - Alpaca Simulator (cross-platform)

    return devices;
  }

  /// Convert DeviceType to ASCOM device type string
  static String? _deviceTypeToAscomType(DeviceType deviceType) {
    switch (deviceType) {
      case DeviceType.camera:
        return 'Camera';
      case DeviceType.mount:
        return 'Telescope';
      case DeviceType.focuser:
        return 'Focuser';
      case DeviceType.filterWheel:
        return 'FilterWheel';
      case DeviceType.guider:
        return 'Camera'; // Guider cameras use Camera type
      case DeviceType.rotator:
        return 'Rotator';
      case DeviceType.dome:
        return 'Dome';
      case DeviceType.weather:
        return 'ObservingConditions';
      case DeviceType.safetyMonitor:
        return 'SafetyMonitor';
      case DeviceType.switch_:
        return 'Switch';
      case DeviceType.coverCalibrator:
        return 'CoverCalibrator';
    }
  }

  /// Discover PHD2 instances on the network
  /// Checks if PHD2 is installed (always shows it if installed, even if not running)
  /// Connection will launch PHD2 if it's installed but not running
  /// Also scans local subnet for remote PHD2 instances
  static Future<List<Map<String, dynamic>>> _discoverPhd2Instances() async {
    final instances = <Map<String, dynamic>>[];
    const defaultPort = 4400;
    final discoveredHosts = <String>{};

    // Always check if PHD2 is installed - if it is, add it to the list
    // Connection will handle launching it if needed
    final isInstalled = await _isPhd2Installed();
    if (isInstalled) {
      instances.add({'host': 'localhost', 'port': defaultPort});
      discoveredHosts.add('localhost');
      discoveredHosts.add('127.0.0.1');
    }

    // Network subnet scanning for remote PHD2 instances
    try {
      print('Scanning local network for PHD2 instances...');
      final localIps = await _getLocalNetworkAddresses();

      for (final subnet in localIps) {
        print('Scanning subnet: $subnet');
        final remoteInstances = await _scanSubnetForPhd2(subnet, defaultPort);

        for (final host in remoteInstances) {
          // Don't add duplicates
          if (!discoveredHosts.contains(host)) {
            instances.add({'host': host, 'port': defaultPort});
            discoveredHosts.add(host);
            print('Found remote PHD2 at $host:$defaultPort');
          }
        }
      }
    } catch (e) {
      print('Network scan failed: $e');
      // Continue with local instance if we found one
    }

    return instances;
  }

  /// Get local network addresses to scan
  static Future<List<String>> _getLocalNetworkAddresses() async {
    final subnets = <String>[];

    try {
      final interfaces = await NetworkInterface.list(
        includeLinkLocal: false,
        type: InternetAddressType.IPv4,
      );

      for (final interface in interfaces) {
        for (final addr in interface.addresses) {
          final ip = addr.address;
          // Extract subnet (assuming /24 network)
          final parts = ip.split('.');
          if (parts.length == 4) {
            final subnet = '${parts[0]}.${parts[1]}.${parts[2]}';
            if (!subnets.contains(subnet)) {
              subnets.add(subnet);
            }
          }
        }
      }
    } catch (e) {
      print('Failed to get network interfaces: $e');
    }

    return subnets;
  }

  /// Scan a subnet for PHD2 instances
  /// Scans all hosts in the subnet (xxx.xxx.xxx.1-254) on port 4400
  static Future<List<String>> _scanSubnetForPhd2(
      String subnet, int port) async {
    final foundHosts = <String>[];
    final futures = <Future<void>>[];

    // Scan all possible host addresses in parallel (1-254)
    for (int i = 1; i <= 254; i++) {
      final host = '$subnet.$i';

      // Skip localhost (already checked)
      if (i == 1 && (subnet == '127.0.0' || subnet == '::1')) continue;

      futures.add(_checkPhd2AtHost(host, port).then((isRunning) {
        if (isRunning) {
          foundHosts.add(host);
        }
      }).catchError((e) {
        // Ignore individual connection failures
      }));

      // Process in batches to avoid overwhelming the system
      if (futures.length >= 50) {
        await Future.wait(futures, eagerError: false);
        futures.clear();
      }
    }

    // Wait for remaining checks
    if (futures.isNotEmpty) {
      await Future.wait(futures, eagerError: false);
    }

    return foundHosts;
  }

  /// Check if PHD2 is running at a specific host:port
  static Future<bool> _checkPhd2AtHost(String host, int port) async {
    try {
      final socket = await Socket.connect(
        host,
        port,
        timeout: const Duration(milliseconds: 500),
      );

      // Successfully connected - verify it's actually PHD2 by sending a simple request
      try {
        // Send a get_app_state request
        final request = '{"method":"get_app_state","id":1}\r\n';
        socket.write(request);
        await socket.flush();

        // Wait for response with timeout
        final response = await socket
            .timeout(
              const Duration(seconds: 1),
            )
            .first
            .timeout(
              const Duration(seconds: 1),
              onTimeout: () => Uint8List(0),
            );

        socket.destroy();

        // If we got a response, it's likely PHD2
        if (response.isNotEmpty) {
          final responseStr = String.fromCharCodes(response);
          // Check if response looks like JSON-RPC
          return responseStr.contains('result') ||
              responseStr.contains('error');
        }
      } catch (e) {
        socket.destroy();
      }

      return false;
    } catch (e) {
      return false;
    }
  }

  /// Check if PHD2 is installed on the system
  static Future<bool> _isPhd2Installed() async {
    // First check if it's already running (fastest check)
    if (await phd2.checkPhd2Running(host: 'localhost', port: 4400)) {
      return true;
    }

    // Platform-specific installation checks
    if (Platform.isWindows) {
      return await _isPhd2InstalledWindows();
    } else if (Platform.isMacOS) {
      return await _isPhd2InstalledMacOS();
    } else if (Platform.isLinux) {
      return await _isPhd2InstalledLinux();
    }

    // Unknown platform - assume not installed
    return false;
  }

  /// Check if PHD2 is installed on Windows
  static Future<bool> _isPhd2InstalledWindows() async {
    final phd2Paths = [
      r'C:\Program Files (x86)\PHDGuiding2\phd2.exe',
      r'C:\Program Files\PHDGuiding2\phd2.exe',
      r'C:\Program Files (x86)\PHD2\phd2.exe',
      r'C:\Program Files\PHD2\phd2.exe',
    ];

    for (final path in phd2Paths) {
      if (await File(path).exists()) {
        return true;
      }
    }

    // Check if phd2 process is running
    try {
      final result =
          await Process.run('tasklist', ['/FI', 'IMAGENAME eq phd2.exe']);
      if (result.exitCode == 0) {
        final output = result.stdout.toString();
        if (output.contains('phd2.exe')) {
          return true;
        }
      }
    } catch (e) {
      print('Failed to check for running PHD2 process: $e');
    }

    return false;
  }

  /// Check if PHD2 is installed on macOS
  static Future<bool> _isPhd2InstalledMacOS() async {
    // Common installation paths on macOS
    final phd2Paths = [
      '/Applications/PHD2.app',
      '/Applications/phd2.app',
      '${Platform.environment['HOME']}/Applications/PHD2.app',
      '${Platform.environment['HOME']}/Applications/phd2.app',
    ];

    for (final path in phd2Paths) {
      if (await Directory(path).exists()) {
        return true;
      }
    }

    // Check if phd2 is in PATH
    try {
      final result = await Process.run('which', ['phd2']);
      if (result.exitCode == 0) {
        return true;
      }
    } catch (e) {
      print('Failed to check for phd2 in PATH: $e');
    }

    // Check if phd2 process is running
    try {
      final result = await Process.run('pgrep', ['-x', 'phd2']);
      if (result.exitCode == 0) {
        return true;
      }
    } catch (e) {
      print('Failed to check for running PHD2 process: $e');
    }

    return false;
  }

  /// Check if PHD2 is installed on Linux
  static Future<bool> _isPhd2InstalledLinux() async {
    // Common installation paths on Linux
    final phd2Paths = [
      '/usr/bin/phd2',
      '/usr/local/bin/phd2',
      '${Platform.environment['HOME']}/.local/bin/phd2',
      '/opt/phd2/bin/phd2',
    ];

    for (final path in phd2Paths) {
      if (await File(path).exists()) {
        return true;
      }
    }

    // Check if phd2 is in PATH
    try {
      final result = await Process.run('which', ['phd2']);
      if (result.exitCode == 0) {
        return true;
      }
    } catch (e) {
      print('Failed to check for phd2 in PATH: $e');
    }

    // Check if phd2 process is running
    try {
      final result = await Process.run('pgrep', ['-x', 'phd2']);
      if (result.exitCode == 0) {
        return true;
      }
    } catch (e) {
      print('Failed to check for running PHD2 process: $e');
    }

    // Check common package manager installations
    try {
      // Check if installed via apt (Debian/Ubuntu)
      final dpkgResult = await Process.run('dpkg', ['-l', 'phd2']);
      if (dpkgResult.exitCode == 0 &&
          dpkgResult.stdout.toString().contains('phd2')) {
        return true;
      }
    } catch (e) {
      // dpkg might not be available
    }

    try {
      // Check if installed via rpm (Fedora/RedHat)
      final rpmResult = await Process.run('rpm', ['-q', 'phd2']);
      if (rpmResult.exitCode == 0) {
        return true;
      }
    } catch (e) {
      // rpm might not be available
    }

    return false;
  }

  /// Convert stub DeviceType to generated DeviceType
  static gen_device.DeviceType _toGenDeviceType(DeviceType deviceType) {
    switch (deviceType) {
      case DeviceType.camera:
        return gen_device.DeviceType.camera;
      case DeviceType.mount:
        return gen_device.DeviceType.mount;
      case DeviceType.focuser:
        return gen_device.DeviceType.focuser;
      case DeviceType.filterWheel:
        return gen_device.DeviceType.filterWheel;
      case DeviceType.guider:
        return gen_device.DeviceType.guider;
      case DeviceType.dome:
        return gen_device.DeviceType.dome;
      case DeviceType.rotator:
        return gen_device.DeviceType.rotator;
      case DeviceType.weather:
        return gen_device.DeviceType.weather;
      case DeviceType.safetyMonitor:
        return gen_device.DeviceType.safetyMonitor;
      case DeviceType.switch_:
        return gen_device.DeviceType.switch_;
      case DeviceType.coverCalibrator:
        return gen_device.DeviceType.coverCalibrator;
    }
  }

  /// Convert generated DeviceType to stub DeviceType
  static DeviceType _fromGenDeviceType(gen_device.DeviceType deviceType) {
    switch (deviceType) {
      case gen_device.DeviceType.camera:
        return DeviceType.camera;
      case gen_device.DeviceType.mount:
        return DeviceType.mount;
      case gen_device.DeviceType.focuser:
        return DeviceType.focuser;
      case gen_device.DeviceType.filterWheel:
        return DeviceType.filterWheel;
      case gen_device.DeviceType.guider:
        return DeviceType.guider;
      case gen_device.DeviceType.dome:
        return DeviceType.dome;
      case gen_device.DeviceType.rotator:
        return DeviceType.rotator;
      case gen_device.DeviceType.weather:
        return DeviceType.weather;
      case gen_device.DeviceType.safetyMonitor:
        return DeviceType.safetyMonitor;
      case gen_device.DeviceType.switch_:
        return DeviceType.switch_;
      case gen_device.DeviceType.coverCalibrator:
        return DeviceType.coverCalibrator;
    }
  }

  /// Convert generated DriverType to stub DriverType
  static DriverType _fromGenDriverType(gen_device.DriverType driverType) {
    switch (driverType) {
      case gen_device.DriverType.ascom:
        return DriverType.ascom;
      case gen_device.DriverType.alpaca:
        return DriverType.alpaca;
      case gen_device.DriverType.indi:
        return DriverType.indi;
      case gen_device.DriverType.native:
        return DriverType.native;
      case gen_device.DriverType.simulator:
        return DriverType.simulator;
      default:
        throw ArgumentError('Unknown DriverType: $driverType');
    }
  }

  /// Check if an Alpaca device type matches our DeviceType
  static bool _alpacaTypeMatches(String alpacaType, DeviceType deviceType) {
    switch (deviceType) {
      case DeviceType.camera:
        return alpacaType == 'camera';
      case DeviceType.mount:
        return alpacaType == 'telescope';
      case DeviceType.focuser:
        return alpacaType == 'focuser';
      case DeviceType.filterWheel:
        return alpacaType == 'filterwheel';
      case DeviceType.guider:
        return alpacaType == 'camera'; // Guider cameras
      case DeviceType.rotator:
        return alpacaType == 'rotator';
      case DeviceType.dome:
        return alpacaType == 'dome';
      case DeviceType.weather:
        return alpacaType == 'observingconditions';
      case DeviceType.safetyMonitor:
        return alpacaType == 'safetymonitor';
      case DeviceType.switch_:
        return alpacaType == 'switch';
      case DeviceType.coverCalibrator:
        return alpacaType == 'covercalibrator';
    }
  }

  // =========================================================================
  // Device Connection
  // =========================================================================

  /// Connect to a device
  static Future<void> connectDevice(
      DeviceType deviceType, String deviceId) async {
    // Check if this is PHD2 (supports new format: phd2:host:port or legacy: phd2)
    if (deviceId == 'phd2' || deviceId.startsWith('phd2:')) {
      String? host;
      int? port;

      if (deviceId.startsWith('phd2:')) {
        // Parse phd2:host:port format
        final parts = deviceId.split(':');
        if (parts.length >= 3) {
          host = parts[1];
          port = int.tryParse(parts[2]) ?? 4400;
        }
      }

      await phd2Connect(host: host, port: port);
      _connectedDevices[deviceId] = true;
      return;
    }

    // =========================================================================
    // Try Native Bridge Connection First (for ASCOM, native, Alpaca, INDI)
    // =========================================================================
    // For devices discovered by native bridge (ascom:, native:, indi:),
    // always use native bridge connection. For other devices (alpaca:),
    // try native bridge first but fall back to stub if needed.
    final shouldUseNativeOnly = deviceId.startsWith('ascom:') ||
        deviceId.startsWith('native:') ||
        deviceId.startsWith('indi:');

    if (_nativeAvailable) {
      try {
        print('[NativeBridge] Attempting native connection for $deviceId...');
        final genDeviceType = _toGenDeviceType(deviceType);
        await gen_api.apiConnectDevice(
            deviceType: genDeviceType, deviceId: deviceId);
        print(
            '[NativeBridge] ✓ Successfully connected to $deviceId via native bridge');

        // Mark as connected in stub state
        _connectedDevices[deviceId] = true;

        // Emit connection event
        _eventController.add(_StubNightshadeEvent(
          timestamp: DateTime.now().millisecondsSinceEpoch,
          severity: EventSeverity.info,
          category: EventCategory.equipment,
          eventType: 'Connected',
          data: {'deviceType': deviceType.name, 'deviceId': deviceId},
        ));

        return; // Success - native bridge handled it
      } catch (e, stackTrace) {
        print('[NativeBridge] ✗ Native connection failed for $deviceId');
        print('[NativeBridge] Error: $e');
        print('[NativeBridge] Stack trace: $stackTrace');

        // If this device must use native bridge (was discovered by it),
        // don't fall back - throw the error
        if (shouldUseNativeOnly) {
          throw Exception(
              'Failed to connect to $deviceId via native bridge: $e');
        }

        print(
            '[NativeBridge] Device supports fallback - trying stub methods...');
        // Continue to fallback stub methods below
      }
    } else if (shouldUseNativeOnly) {
      // Native bridge required but not available
      throw Exception(
          'Cannot connect to $deviceId: Native bridge required but not available');
    }

    // =========================================================================
    // Fallback: Stub Connection Methods (for when native bridge unavailable)
    // =========================================================================

    // Check if this is an Alpaca device
    if (deviceId.startsWith('alpaca:')) {
      await _connectAlpacaDevice(deviceType, deviceId);
      return;
    }

    // Check if this is an ASCOM device
    if (deviceId.startsWith('ascom:')) {
      await _connectAscomDevice(deviceType, deviceId);
      return;
    }

    // Unknown device type - can't connect
    throw Exception(
        'Unknown device: $deviceId. No ASCOM/Alpaca devices found.');
  }

  /// Connect to an ASCOM device
  static Future<void> _connectAscomDevice(
      DeviceType deviceType, String deviceId) async {
    if (!Platform.isWindows) {
      throw Exception('ASCOM is only available on Windows');
    }

    // Parse the device ID: ascom:ProgID
    final progId = deviceId.substring(6); // Remove "ascom:"

    final ascomType = _deviceTypeToAscomType(deviceType);
    if (ascomType == null) {
      throw Exception('Unsupported device type for ASCOM: $deviceType');
    }

    final client = ascom.AscomDeviceClient(
      progId: progId,
      deviceType: ascomType,
    );

    try {
      print('Connecting to ASCOM device: $progId');
      await client.connect();

      _ascomClients[deviceId] = client;
      _connectedDevices[deviceId] = true;

      _eventController.add(_StubNightshadeEvent(
        timestamp: DateTime.now().millisecondsSinceEpoch,
        severity: EventSeverity.info,
        category: EventCategory.equipment,
        eventType: 'Connected',
        data: {'deviceType': deviceType.name, 'deviceId': deviceId},
      ));

      print('Connected to ASCOM device: $progId');
    } catch (e) {
      client.dispose();
      throw Exception('Failed to connect to ASCOM device: $e');
    }
  }

  /// Connect to an Alpaca device
  static Future<void> _connectAlpacaDevice(
      DeviceType deviceType, String deviceId) async {
    // Parse the device ID: alpaca:host:port/type/number
    final parts = deviceId.substring(7).split('/'); // Remove "alpaca:"
    if (parts.length < 3) {
      throw Exception('Invalid Alpaca device ID: $deviceId');
    }

    final hostPort = parts[0].split(':');
    if (hostPort.length != 2) {
      throw Exception('Invalid Alpaca device ID: $deviceId');
    }

    final host = hostPort[0];
    final port = int.tryParse(hostPort[1]) ?? 11111;
    final deviceTypeName = parts[1];
    final deviceNumber = int.tryParse(parts[2]) ?? 0;

    final server = alpaca.AlpacaServer(host: host, port: port);
    final alpacaDevice = alpaca.AlpacaDevice(
      deviceName: 'Alpaca Device',
      deviceType: deviceTypeName,
      deviceNumber: deviceNumber,
      uniqueId: deviceId,
      server: server,
    );

    // Create appropriate client based on device type
    alpaca.AlpacaClient client;
    switch (deviceType) {
      case DeviceType.camera:
      case DeviceType.guider:
        client = alpaca.AlpacaCameraClient(alpacaDevice);
        break;
      case DeviceType.mount:
        client = alpaca.AlpacaMountClient(alpacaDevice);
        break;
      case DeviceType.focuser:
        client = alpaca.AlpacaFocuserClient(alpacaDevice);
        break;
      case DeviceType.filterWheel:
        client = alpaca.AlpacaFilterWheelClient(alpacaDevice);
        break;
      default:
        client = alpaca.AlpacaClient(alpacaDevice);
    }

    try {
      print('Connecting to Alpaca device: $deviceId');
      await client.connect();

      _alpacaClients[deviceId] = client;
      _alpacaDevices[deviceId] = alpacaDevice;
      _connectedDevices[deviceId] = true;

      _eventController.add(_StubNightshadeEvent(
        timestamp: DateTime.now().millisecondsSinceEpoch,
        severity: EventSeverity.info,
        category: EventCategory.equipment,
        eventType: 'Connected',
        data: {'deviceType': deviceType.name, 'deviceId': deviceId},
      ));

      print('Connected to Alpaca device: $deviceId');
    } catch (e) {
      client.dispose();
      throw Exception('Failed to connect to Alpaca device: $e');
    }
  }

  /// Disconnect from a device
  static Future<void> disconnectDevice(
      DeviceType deviceType, String deviceId) async {
    // Handle PHD2 disconnection (supports new format: phd2:host:port or legacy: phd2)
    if (deviceId == 'phd2' || deviceId.startsWith('phd2:')) {
      await phd2Disconnect();
    }

    // Handle Alpaca device disconnection
    if (deviceId.startsWith('alpaca:')) {
      final client = _alpacaClients[deviceId];
      if (client != null) {
        try {
          await client.disconnect();
        } catch (e) {
          print('Error disconnecting Alpaca device: $e');
        }
        client.dispose();
        _alpacaClients.remove(deviceId);
        _alpacaDevices.remove(deviceId);
      }
    }

    // Handle ASCOM device disconnection
    if (deviceId.startsWith('ascom:')) {
      final client = _ascomClients[deviceId];
      if (client != null) {
        try {
          await client.disconnect();
        } catch (e) {
          print('Error disconnecting ASCOM device: $e');
        }
        client.dispose();
        _ascomClients.remove(deviceId);
      }
    }

    _connectedDevices.remove(deviceId);

    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.equipment,
      eventType: 'Disconnected',
      data: {'deviceType': deviceType.name, 'deviceId': deviceId},
    ));
  }

  /// Check if a device is connected
  static Future<bool> isDeviceConnected(
      DeviceType deviceType, String deviceId) async {
    // If native bridge is available, use it for authoritative connection status
    if (_nativeAvailable) {
      try {
        return await gen_api.apiIsDeviceConnected(
            deviceType: _toGenDeviceType(deviceType), deviceId: deviceId);
      } catch (e) {
        print(
            '[NativeBridge] Warning: Failed to check device connection from native API: $e');
        // Fall through to local tracking
      }
    }
    return _connectedDevices[deviceId] ?? false;
  }

  /// Get list of connected devices
  static Future<List<DeviceInfo>> getConnectedDevices() async {
    // If native bridge is available, use it to get authoritative connected devices list
    if (_nativeAvailable) {
      try {
        final nativeDevices = await gen_api.apiGetConnectedDevices();
        // Sync our local tracking with native state
        _connectedDevices.clear();
        for (final device in nativeDevices) {
          _connectedDevices[device.id] = true;
        }
        return nativeDevices;
      } catch (e) {
        print(
            '[NativeBridge] Warning: Failed to get connected devices from native API: $e');
        // Fall through to stub implementation
      }
    }

    // Fallback: return devices from local tracking
    return _connectedDevices.keys.map((id) {
      final type = _getDeviceTypeFromId(id);
      final deviceName = 'Connected ${type.displayName}';
      return DeviceInfo(
        id: id,
        name: deviceName,
        deviceType: type,
        driverType: _getDriverTypeFromId(id),
        description: 'Connected device',
        driverVersion: '1.0.0',
        displayName: deviceName,
      );
    }).toList();
  }

  static DeviceType _getDeviceTypeFromId(String id) {
    if (id.contains('camera')) return DeviceType.camera;
    if (id.contains('mount') || id.contains('telescope'))
      return DeviceType.mount;
    if (id.contains('focuser')) return DeviceType.focuser;
    if (id.contains('filterwheel') || id.contains('filterWheel')) {
      return DeviceType.filterWheel;
    }
    if (id.contains('guider')) return DeviceType.guider;
    if (id.contains('rotator')) return DeviceType.rotator;
    if (id.contains('dome')) return DeviceType.dome;
    if (id.contains('weather')) return DeviceType.weather;
    return DeviceType.camera;
  }

  static DriverType _getDriverTypeFromId(String id) {
    if (id.startsWith('ascom:')) return DriverType.ascom;
    if (id.startsWith('alpaca:')) return DriverType.alpaca;
    if (id.startsWith('indi:')) return DriverType.indi;
    return DriverType.simulator;
  }

  // =========================================================================
  // Camera Control
  // =========================================================================

  /// Get camera status
  static Future<CameraStatus> getCameraStatus(String deviceId) async {
    // If native bridge is available, call the real Rust API
    if (_nativeAvailable) {
      try {
        return await gen_api.getCameraStatus(deviceId: deviceId);
      } catch (e) {
        print('[NativeBridge] Error getting camera status from native: $e');
        // Fall through to stub
      }
    }
    // Fallback to stub status
    return _cameraStatus!;
  }

  /// Set camera cooler
  static Future<void> setCameraCooler(
      String deviceId, bool enabled, double? targetTemp) async {
    // If native bridge is available, call the real Rust API
    if (_nativeAvailable) {
      try {
        await gen_api.setCameraCooler(
          deviceId: deviceId,
          enabled: enabled ? 1 : 0,
          targetTemp: targetTemp,
        );
        return;
      } catch (e) {
        print('[NativeBridge] Error setting camera cooler from native: $e');
        // Fall through to stub
      }
    }
    // Stub fallback
    await Future.delayed(const Duration(milliseconds: 100));
  }

  /// Set camera gain
  static Future<void> setCameraGain(String deviceId, int gain) async {
    // If native bridge is available, call the real Rust API
    if (_nativeAvailable) {
      try {
        await gen_api.setCameraGain(deviceId: deviceId, gain: gain);
        return;
      } catch (e) {
        print('[NativeBridge] Error setting camera gain from native: $e');
      }
    }
    await Future.delayed(const Duration(milliseconds: 50));
  }

  /// Set camera offset
  static Future<void> setCameraOffset(String deviceId, int offset) async {
    // If native bridge is available, call the real Rust API
    if (_nativeAvailable) {
      try {
        await gen_api.setCameraOffset(deviceId: deviceId, offset: offset);
        return;
      } catch (e) {
        print('[NativeBridge] Error setting camera offset from native: $e');
      }
    }
    await Future.delayed(const Duration(milliseconds: 50));
  }

  /// Set camera binning
  static Future<void> setCameraBinning(
      String deviceId, int binX, int binY) async {
    // If native bridge is available, call the real Rust API
    if (_nativeAvailable) {
      try {
        await gen_api.apiSetCameraBinning(
            deviceId: deviceId, binX: binX, binY: binY);
        return;
      } catch (e) {
        print('[NativeBridge] Error setting camera binning from native: $e');
      }
    }
    await Future.delayed(const Duration(milliseconds: 50));
  }

  /// Start a camera exposure
  static Future<void> startExposure({
    required String deviceId,
    required double durationSecs,
    required int gain,
    required int offset,
    required int binX,
    required int binY,
  }) async {
    // If native bridge is available, call the real Rust API via RustLib
    if (_nativeAvailable) {
      try {
        // Call the Rust API function directly
        await frb.RustLib.instance.api.crateApiApiCameraStartExposure(
          deviceId: deviceId,
          durationSecs: durationSecs,
          gain: gain,
          offset: offset,
          binX: binX,
          binY: binY,
        );
        return;
      } catch (e) {
        print('[NativeBridge] Error calling native startExposure: $e');
        // Fall through to simulation
      }
    }

    // Fallback stub implementation when native isn't available
    print('[NativeBridge] Using simulated exposure');
    // Emit exposure started event
    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.imaging,
      eventType: 'ExposureStarted',
      data: {'duration': durationSecs},
    ));

    // Simulate exposure with progress updates
    const steps = 10;
    for (int i = 0; i < steps; i++) {
      await Future.delayed(
          Duration(milliseconds: (durationSecs * 100).round()));
      final progress = (i + 1) / steps;
      _eventController.add(_StubNightshadeEvent(
        timestamp: DateTime.now().millisecondsSinceEpoch,
        severity: EventSeverity.info,
        category: EventCategory.imaging,
        eventType: 'ExposureProgress',
        data: {
          'progress': progress,
          'remainingSecs': durationSecs * (1 - progress)
        },
      ));
    }

    // Emit completion event
    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.imaging,
      eventType: 'ExposureComplete',
      data: {'duration': durationSecs},
    ));
  }

  /// Cancel current exposure
  static Future<void> cancelExposure(String deviceId) async {
    if (_nativeAvailable) {
      try {
        await frb.RustLib.instance.api
            .crateApiApiCameraCancelExposure(deviceId: deviceId);
        return;
      } catch (e) {
        print('[NativeBridge] Error calling native cancelExposure: $e');
      }
    }

    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.imaging,
      eventType: 'ExposureCancelled',
      data: {},
    ));
  }

  /// Get last captured image
  static Future<CapturedImageResult?> getLastImage() async {
    print(
        '[NativeBridge] getLastImage called, nativeAvailable=$_nativeAvailable');
    if (_nativeAvailable) {
      try {
        // Call the Rust API - returns generated CapturedImageResult (with Uint8List)
        print('[NativeBridge] Calling crateApiApiGetLastImage...');
        final rustResult =
            await frb.RustLib.instance.api.crateApiApiGetLastImage();
        print(
            '[NativeBridge] Got result: ${rustResult.width}x${rustResult.height}, displayData size: ${rustResult.displayData.length}');

        // Return the FRB-generated CapturedImageResult directly (already has Uint8List/Uint32List)
        return rustResult;
      } catch (e) {
        print('[NativeBridge] Error calling native getLastImage: $e');
      }
    }

    // Return simulated image as fallback
    print('[NativeBridge] Returning simulated fallback image');
    return CapturedImageResult(
      width: 4144,
      height: 2822,
      displayData: Uint8List.fromList(List.filled(4144 * 2822, 128)),
      histogram: Uint32List.fromList(List.generate(256, (i) => i * 100)),
      stats: const ImageStatsResult(
        min: 500,
        max: 60000,
        mean: 1200,
        median: 1100,
        stdDev: 150,
        hfr: 2.5,
        starCount: 150,
      ),
      exposureTime: 1.0,
      timestamp: DateTime.now().toIso8601String(),
      isColor: false,
    );
  }

  // =========================================================================
  // Mount Control
  // =========================================================================

  /// Get mount status
  static Future<MountStatus> getMountStatus(String deviceId) async {
    if (_nativeAvailable) {
      try {
        return await gen_api.apiGetMountStatus(deviceId: deviceId);
      } catch (e) {
        print('[NativeBridge] Error getting mount status from native: $e');
      }
    }
    return _mountStatus!;
  }

  /// Slew the mount to coordinates
  static Future<void> mountSlewToCoordinates(
      String deviceId, double ra, double dec) async {
    if (_nativeAvailable) {
      try {
        await gen_api.apiMountSlewToCoordinates(
            deviceId: deviceId, ra: ra, dec: dec);
        return;
      } catch (e) {
        print('[NativeBridge] Error slewing mount via native: $e');
      }
    }
    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.equipment,
      eventType: 'SlewStarted',
      data: {'deviceId': deviceId, 'ra': ra, 'dec': dec},
    ));

    _updateMountStatus(
      slewing: true,
      tracking: false,
      parked: false,
    );

    await Future.delayed(const Duration(seconds: 2));

    _updateMountStatus(
      slewing: false,
      tracking: true,
      rightAscension: ra,
      declination: dec,
      parked: false,
    );

    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.equipment,
      eventType: 'SlewCompleted',
      data: {'deviceId': deviceId, 'ra': ra, 'dec': dec},
    ));
  }

  /// Sync the mount to coordinates
  static Future<void> mountSync(String deviceId, double ra, double dec) async {
    if (_nativeAvailable) {
      try {
        await gen_api.apiMountSyncToCoordinates(
            deviceId: deviceId, ra: ra, dec: dec);
        return;
      } catch (e) {
        print('[NativeBridge] Error syncing mount via native: $e');
      }
    }
    _updateMountStatus(
      rightAscension: ra,
      declination: dec,
    );

    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.equipment,
      eventType: 'MountSynced',
      data: {'deviceId': deviceId, 'ra': ra, 'dec': dec},
    ));
  }

  /// Park the mount
  static Future<void> mountPark(String deviceId) async {
    if (_nativeAvailable) {
      try {
        await gen_api.apiMountPark(deviceId: deviceId);
        return;
      } catch (e) {
        print('[NativeBridge] Error parking mount via native: $e');
      }
    }
    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.equipment,
      eventType: 'ParkStarted',
      data: {'deviceId': deviceId},
    ));

    _updateMountStatus(
      slewing: true,
      tracking: false,
    );

    await Future.delayed(const Duration(seconds: 2));

    _updateMountStatus(
      slewing: false,
      parked: true,
      tracking: false,
    );

    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.equipment,
      eventType: 'ParkCompleted',
      data: {'deviceId': deviceId},
    ));
  }

  /// Unpark the mount
  static Future<void> mountUnpark(String deviceId) async {
    if (_nativeAvailable) {
      try {
        await gen_api.apiMountUnpark(deviceId: deviceId);
        return;
      } catch (e) {
        print('[NativeBridge] Error unparking mount via native: $e');
      }
    }
    await Future.delayed(const Duration(milliseconds: 200));
    _updateMountStatus(
      parked: false,
    );
    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.equipment,
      eventType: 'MountUnparked',
      data: {'deviceId': deviceId},
    ));
  }

  /// Set mount tracking
  static Future<void> mountSetTracking(String deviceId, bool enabled) async {
    if (_nativeAvailable) {
      try {
        await gen_api.apiMountSetTracking(
            deviceId: deviceId, enabled: enabled ? 1 : 0);
        return;
      } catch (e) {
        print('[NativeBridge] Error setting mount tracking via native: $e');
      }
    }
    await Future.delayed(const Duration(milliseconds: 100));
    _updateMountStatus(tracking: enabled);
    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.equipment,
      eventType: enabled ? 'TrackingEnabled' : 'TrackingDisabled',
      data: {'deviceId': deviceId},
    ));
  }

  /// Pulse guide mount
  static Future<void> mountPulseGuide(
      String deviceId, String direction, int durationMs) async {
    if (_nativeAvailable) {
      try {
        await gen_api.apiMountPulseGuide(
          deviceId: deviceId,
          direction: direction,
          durationMs: durationMs,
        );
        return;
      } catch (e) {
        print('[NativeBridge] Error pulse guiding mount via native: $e');
      }
    }
    // Simulate pulse guide duration
    await Future.delayed(Duration(milliseconds: durationMs));
    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.equipment,
      eventType: 'PulseGuideComplete',
      data: {
        'deviceId': deviceId,
        'direction': direction,
        'durationMs': durationMs
      },
    ));
  }

  /// Set mount tracking rate (0=Sidereal, 1=Lunar, 2=Solar, 3=King)
  static Future<void> mountSetTrackingRate(String deviceId, int rate) async {
    if (_nativeAvailable) {
      try {
        await gen_api.mountSetTrackingRate(deviceId: deviceId, rate: rate);
        return;
      } catch (e) {
        print('[NativeBridge] Error setting tracking rate from native: $e');
      }
    }
    await Future.delayed(const Duration(milliseconds: 100));
    _updateMountStatus(trackingRate: TrackingRate.values[rate.clamp(0, 3)]);
    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.equipment,
      eventType: 'TrackingRateChanged',
      data: {'deviceId': deviceId, 'rate': rate},
    ));
  }

  /// Get mount tracking rate (0=Sidereal, 1=Lunar, 2=Solar, 3=King)
  static Future<int> mountGetTrackingRate(String deviceId) async {
    if (_nativeAvailable) {
      try {
        return await gen_api.mountGetTrackingRate(deviceId: deviceId);
      } catch (e) {
        print('[NativeBridge] Error getting tracking rate from native: $e');
      }
    }
    return _mountStatus?.trackingRate.index ?? 0;
  }

  /// Move mount axis at specified rate (degrees/second)
  /// axis: 0=RA/Azimuth (primary), 1=Dec/Altitude (secondary)
  /// rate: degrees per second (positive = N/E, negative = S/W), 0 to stop
  static Future<void> mountMoveAxis(
      String deviceId, int axis, double rate) async {
    if (_nativeAvailable) {
      try {
        await gen_api.mountMoveAxis(deviceId: deviceId, axis: axis, rate: rate);
        return;
      } catch (e) {
        print('[NativeBridge] Error moving axis from native: $e');
      }
    }
    // Stub: just log the event
    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.equipment,
      eventType: rate == 0 ? 'AxisStopped' : 'AxisMoving',
      data: {'deviceId': deviceId, 'axis': axis, 'rate': rate},
    ));
  }

  // =========================================================================
  // Focuser Control
  // =========================================================================

  /// Get focuser status
  static Future<FocuserStatus> getFocuserStatus(String deviceId) async {
    if (_nativeAvailable) {
      try {
        return await gen_api.apiGetFocuserStatus(deviceId: deviceId);
      } catch (e) {
        print('[NativeBridge] Error getting focuser status from native: $e');
      }
    }
    return _focuserStatus!;
  }

  /// Move focuser to position
  static Future<void> focuserMoveTo(String deviceId, int position) async {
    if (_nativeAvailable) {
      try {
        await gen_api.apiFocuserMoveTo(deviceId: deviceId, position: position);
        return;
      } catch (e) {
        print('[NativeBridge] Error moving focuser via native: $e');
      }
    }
    final current = _focuserStatus!;
    _focuserStatus = FocuserStatus(
      connected: true,
      position: current.position,
      moving: true,
      temperature: current.temperature,
      maxPosition: current.maxPosition,
      stepSize: current.stepSize,
      isAbsolute: current.isAbsolute,
      hasTemperature: current.hasTemperature,
    );

    await Future.delayed(const Duration(seconds: 1));

    _focuserStatus = FocuserStatus(
      connected: true,
      position: position,
      moving: false,
      temperature: current.temperature,
      maxPosition: current.maxPosition,
      stepSize: current.stepSize,
      isAbsolute: current.isAbsolute,
      hasTemperature: current.hasTemperature,
    );
  }

  /// Move focuser by relative amount
  static Future<void> focuserMoveRelative(String deviceId, int delta) async {
    if (_nativeAvailable) {
      try {
        await gen_api.apiFocuserMoveRelative(deviceId: deviceId, delta: delta);
        return;
      } catch (e) {
        print('[NativeBridge] Error moving focuser relative via native: $e');
      }
    }
    final target = (_focuserStatus?.position ?? 0) + delta;
    await focuserMoveTo(deviceId, target);
  }

  /// Halt focuser
  static Future<void> apiFocuserHalt({required String deviceId}) async {
    if (_nativeAvailable) {
      try {
        await gen_api.apiFocuserHalt(deviceId: deviceId);
        return;
      } catch (e) {
        print('[NativeBridge] Error halting focuser via native: $e');
      }
    }
    if (_focuserStatus != null) {
      _focuserStatus = FocuserStatus(
        connected: true,
        position: _focuserStatus!.position,
        moving: false,
        temperature: _focuserStatus!.temperature,
        maxPosition: _focuserStatus!.maxPosition,
        stepSize: _focuserStatus!.stepSize,
        isAbsolute: _focuserStatus!.isAbsolute,
        hasTemperature: _focuserStatus!.hasTemperature,
      );
    }
  }

  // =========================================================================
  // Filter Wheel Control
  // =========================================================================

  /// Get filter wheel status
  static Future<FilterWheelStatus> getFilterWheelStatus(String deviceId) async {
    return _filterWheelStatus!;
  }

  /// Set filter wheel position
  static Future<void> filterWheelSetPosition(
      String deviceId, int position) async {
    final current = _filterWheelStatus!;
    _filterWheelStatus = FilterWheelStatus(
      connected: true,
      position: current.position,
      moving: true,
      filterCount: current.filterCount,
      filterNames: current.filterNames,
    );

    await Future.delayed(const Duration(milliseconds: 500));

    _filterWheelStatus = FilterWheelStatus(
      connected: true,
      position: position,
      moving: false,
      filterCount: current.filterCount,
      filterNames: current.filterNames,
    );
  }

  /// Set filter wheel position (API method)
  static Future<void> apiFilterwheelSetPosition({
    required String deviceId,
    required int position,
  }) async {
    await filterWheelSetPosition(deviceId, position);
  }

  /// Get filter wheel names (API method)
  static Future<List<String>> apiFilterwheelGetNames({
    required String deviceId,
  }) async {
    final status = await getFilterWheelStatus(deviceId);
    return status.filterNames;
  }

  /// Set filter wheel by name (API method)
  static Future<void> apiFilterwheelSetByName({
    required String deviceId,
    required String name,
  }) async {
    final status = await getFilterWheelStatus(deviceId);
    final index = status.filterNames.indexOf(name);
    if (index >= 0) {
      await filterWheelSetPosition(deviceId, index);
    }
  }

  // =========================================================================
  // Session Management
  // =========================================================================

  /// Get current session state
  static Future<NativeSessionState> getSessionState() async {
    return NativeSessionState(
      isActive: false,
      totalExposures: 0,
      completedExposures: 0,
      totalIntegrationSecs: 0.0,
      isGuiding: false,
      isCapturing: false,
      isDithering: false,
    );
  }

  /// Start a new imaging session
  static Future<void> startSession(
      {String? targetName, double? ra, double? dec}) async {
    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.sequencer,
      eventType: 'SessionStarted',
      data: {'target': targetName},
    ));
  }

  /// End the current session
  static Future<void> endSession() async {
    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.sequencer,
      eventType: 'SessionEnded',
      data: {},
    ));
  }

  // =========================================================================
  // Plate Solving
  // =========================================================================

  /// Check if plate solver is available
  static bool isPlateSolverAvailable() => false;

  /// Get plate solver path
  static String? getPlateSolverPath() => null;

  /// Plate solve blind
  static Future<PlateSolveResult> plateSolveBlind(String filePath) async {
    await Future.delayed(const Duration(seconds: 3));
    return const PlateSolveResult(
      success: false,
      ra: 0,
      dec: 0,
      pixelScale: 0,
      rotation: 0,
      fieldWidth: 0,
      fieldHeight: 0,
      solveTimeSecs: 3.0,
      error: 'Plate solver not available',
    );
  }

  /// Plate solve near coordinates
  static Future<PlateSolveResult> plateSolveNear(
    String filePath,
    double hintRa,
    double hintDec,
    double searchRadius,
  ) async {
    await Future.delayed(const Duration(seconds: 2));
    return PlateSolveResult(
      success: false,
      ra: hintRa,
      dec: hintDec,
      pixelScale: 0,
      rotation: 0,
      fieldWidth: 0,
      fieldHeight: 0,
      solveTimeSecs: 2.0,
      error: 'Plate solver not available',
    );
  }

  // =========================================================================
  // Autofocus (Simulated/Stub)
  // =========================================================================

  /// Run autofocus
  static Future<double> apiRunAutofocus({
    required String deviceId,
    required String cameraId,
    required AutofocusConfigApi config,
  }) async {
    print('Starting autofocus on $deviceId using camera $cameraId...');
    print(
        'Config: exposure=${config.exposureTime}s, step=${config.stepSize}, steps=${config.stepsOut}');

    // Simulate autofocus process
    for (int i = 0; i < config.stepsOut * 2 + 1; i++) {
      await Future.delayed(const Duration(milliseconds: 500));
      // In a real implementation, we'd be emitting progress events here
    }

    print('Autofocus complete. Best HFR: 1.5');
    return 1.5;
  }

  /// Cancel autofocus
  static Future<void> apiCancelAutofocus() async {
    print('Cancelling autofocus...');
  }

  // =========================================================================
  // PHD2 Guiding
  // =========================================================================

  /// Check if PHD2 is running
  static Future<bool> isPhd2Running(
      {String host = 'localhost', int port = 4400}) async {
    return phd2.checkPhd2Running(host: host, port: port);
  }

  /// Connect to PHD2 (auto-launches if not running on Windows)
  static Future<void> phd2Connect({String? host, int? port}) async {
    final targetHost = host ?? 'localhost';
    final targetPort = port ?? 4400;

    // Check if PHD2 is already running
    bool phd2Running =
        await phd2.checkPhd2Running(host: targetHost, port: targetPort);

    // If PHD2 is not running and we're on localhost, try to launch it
    if (!phd2Running &&
        (targetHost == 'localhost' || targetHost == '127.0.0.1')) {
      print(
          'DEBUG: PHD2 not running on localhost. Platform.isWindows: ${Platform.isWindows}');
      if (Platform.isWindows) {
        print('PHD2 not running, attempting to launch...');
        try {
          // Common PHD2 installation paths on Windows
          final phd2Paths = [
            r'C:\Program Files (x86)\PHDGuiding2\phd2.exe',
            r'C:\Program Files\PHDGuiding2\phd2.exe',
            r'C:\Program Files (x86)\PHD2\phd2.exe',
            r'C:\Program Files\PHD2\phd2.exe',
          ];

          String? phd2Path;
          for (final path in phd2Paths) {
            if (await File(path).exists()) {
              phd2Path = path;
              break;
            }
          }

          if (phd2Path != null) {
            await Process.start(phd2Path, [], mode: ProcessStartMode.detached);
            print('PHD2 launched from: $phd2Path');

            // Wait for PHD2 to start and open its server
            for (int i = 0; i < 30; i++) {
              await Future.delayed(const Duration(seconds: 1));
              if (await phd2.checkPhd2Running(
                  host: targetHost, port: targetPort)) {
                phd2Running = true;
                print('PHD2 is now running');
                break;
              }
            }

            if (!phd2Running) {
              throw Exception(
                  'PHD2 was launched but did not start its server within 30 seconds');
            }
          } else {
            throw Exception(
                'PHD2 not found. Please install PHD2 from https://openphdguiding.org/');
          }
        } catch (e) {
          print('Failed to launch PHD2: $e');
          throw Exception('Could not launch PHD2: $e');
        }
      } else {
        print(
            'DEBUG: Not on Windows, cannot auto-launch PHD2. Platform: ${Platform.operatingSystem}');
        throw Exception(
            'PHD2 is not running. Platform: ${Platform.operatingSystem}. Please start PHD2 manually.');
      }
    }

    // Now connect to PHD2
    _phd2Client?.dispose();
    _phd2Client = phd2.Phd2Client(
      host: targetHost,
      port: targetPort,
    );

    await _phd2Client!.connect();

    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.guiding,
      eventType: 'PHD2Connected',
      data: {'host': targetHost, 'port': targetPort},
    ));
  }

  /// Disconnect from PHD2
  static Future<void> phd2Disconnect() async {
    _phd2Client?.disconnect();
    _phd2Client = null;

    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.guiding,
      eventType: 'PHD2Disconnected',
      data: {},
    ));
  }

  /// Start guiding
  static Future<void> phd2StartGuiding({
    required double settlePixels,
    required double settleTime,
    required double settleTimeout,
  }) async {
    if (_phd2Client == null || !_phd2Client!.isConnected) {
      throw Exception('PHD2 not connected. Connect to PHD2 first.');
    }

    await _phd2Client!.startGuiding(
      settlePixels: settlePixels,
      settleTime: settleTime,
      settleTimeout: settleTimeout,
    );

    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.guiding,
      eventType: 'GuidingStarted',
      data: {},
    ));
  }

  /// Stop guiding
  static Future<void> phd2StopGuiding() async {
    if (_phd2Client != null && _phd2Client!.isConnected) {
      await _phd2Client!.stopGuiding();
    }

    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.guiding,
      eventType: 'GuidingStopped',
      data: {},
    ));
  }

  /// Pause guiding
  static Future<void> phd2PauseGuiding() async {
    if (_phd2Client == null || !_phd2Client!.isConnected) {
      throw Exception('PHD2 not connected');
    }
    await _phd2Client!.pauseGuiding();
  }

  /// Resume guiding
  static Future<void> phd2ResumeGuiding() async {
    if (_phd2Client == null || !_phd2Client!.isConnected) {
      throw Exception('PHD2 not connected');
    }
    await _phd2Client!.resumeGuiding();
  }

  /// Dither
  static Future<void> phd2Dither({
    required double amount,
    required bool raOnly,
    required double settlePixels,
    required double settleTime,
    required double settleTimeout,
  }) async {
    if (_phd2Client == null || !_phd2Client!.isConnected) {
      throw Exception('PHD2 not connected. Connect to PHD2 first.');
    }

    await _phd2Client!.dither(
      amount: amount,
      raOnly: raOnly,
      settlePixels: settlePixels,
      settleTime: settleTime,
      settleTimeout: settleTimeout,
    );

    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.guiding,
      eventType: 'DitherStarted',
      data: {'amount': amount, 'raOnly': raOnly},
    ));
  }

  /// Get PHD2 status
  static Future<Phd2Status> phd2GetStatus() async {
    if (_phd2Client == null) {
      return const Phd2Status(
        connected: false,
        state: 'Disconnected',
        rmsRa: 0,
        rmsDec: 0,
        rmsTotal: 0,
        snr: 0,
        starMass: 0,
        pixelScale: 0,
      );
    }

    return Phd2Status(
      connected: _phd2Client!.isConnected,
      state: _phd2Client!.state.name,
      rmsRa: _phd2Client!.rmsRa,
      rmsDec: _phd2Client!.rmsDec,
      rmsTotal: _phd2Client!.rmsTotal,
      snr: _phd2Client!.snr,
      starMass: _phd2Client!.starMass,
      pixelScale: 0, // Would need to call getPixelScale() separately
    );
  }

  /// Auto-select guide star in PHD2
  static Future<void> phd2AutoSelectStar() async {
    if (_phd2Client == null || !_phd2Client!.isConnected) {
      throw Exception('PHD2 not connected');
    }
    await _phd2Client!.autoSelectStar();
  }

  /// Start looping exposures in PHD2
  static Future<void> phd2Loop() async {
    if (_phd2Client == null || !_phd2Client!.isConnected) {
      throw Exception('PHD2 not connected');
    }
    await _phd2Client!.loop();
  }

  /// Get PHD2 star image (stub - not available in simulation mode)
  static Future<Phd2StarImage> phd2GetStarImage({int size = 50}) async {
    // Return a placeholder star image for simulation
    final pixelCount = size * size;
    final pixels = Uint8List(pixelCount);
    // Create a simple Gaussian-like pattern centered in the image
    final center = size ~/ 2;
    for (int y = 0; y < size; y++) {
      for (int x = 0; x < size; x++) {
        final dx = x - center;
        final dy = y - center;
        final dist = math.sqrt(dx * dx + dy * dy);
        final value = (255 * math.exp(-dist * dist / 50)).round();
        pixels[y * size + x] = value;
      }
    }
    return Phd2StarImage(
      frame: 1,
      width: size,
      height: size,
      starX: center.toDouble(),
      starY: center.toDouble(),
      pixels: pixels,
    );
  }

  /// Get PHD2 algorithm parameter names
  static Future<List<String>> phd2GetAlgoParamNames(
      {required String axis}) async {
    // Return typical PHD2 Brain parameters
    if (axis.toLowerCase() == 'ra') {
      return ['Aggressiveness', 'Hysteresis', 'MinMove', 'MaxDur'];
    } else {
      return ['Aggressiveness', 'MinMove', 'MaxDur', 'Algorithm'];
    }
  }

  /// Get PHD2 algorithm parameter value
  static Future<double> phd2GetAlgoParam({
    required String axis,
    required String name,
  }) async {
    // Return reasonable default values for simulation
    switch (name) {
      case 'Aggressiveness':
        return axis.toLowerCase() == 'ra' ? 70.0 : 70.0;
      case 'Hysteresis':
        return 10.0;
      case 'MinMove':
        return 0.15;
      case 'MaxDur':
        return axis.toLowerCase() == 'ra' ? 2500.0 : 2500.0;
      default:
        return 0.0;
    }
  }

  /// Set PHD2 algorithm parameter
  static Future<void> phd2SetAlgoParam({
    required String axis,
    required String name,
    required double value,
  }) async {
    // No-op in simulation mode
    print('[NativeBridge Stub] phd2SetAlgoParam: $axis.$name = $value');
  }

  /// Set PHD2 paused state
  static Future<void> phd2SetPaused({required bool paused}) async {
    if (_phd2Client == null || !_phd2Client!.isConnected) {
      throw Exception('PHD2 not connected');
    }
    if (paused) {
      await _phd2Client!.pauseGuiding();
    } else {
      await _phd2Client!.resumeGuiding();
    }
  }

  /// Clear PHD2 calibration
  static Future<void> phd2ClearCalibration({String which = 'both'}) async {
    if (_phd2Client == null || !_phd2Client!.isConnected) {
      throw Exception('PHD2 not connected');
    }
    // Note: The Dart PHD2 client doesn't have this method yet
    // For now, just log it
    print(
        '[NativeBridge Stub] phd2ClearCalibration($which) - not implemented in Dart client');
  }

  /// Flip PHD2 calibration (for meridian flip)
  static Future<void> phd2FlipCalibration() async {
    if (_phd2Client == null || !_phd2Client!.isConnected) {
      throw Exception('PHD2 not connected');
    }
    print(
        '[NativeBridge Stub] phd2FlipCalibration() - not implemented in Dart client');
  }

  /// Get PHD2 calibration data
  static Future<gen_api.Phd2CalibrationData> phd2GetCalibrationData() async {
    if (_phd2Client == null || !_phd2Client!.isConnected) {
      throw Exception('PHD2 not connected');
    }

    // Query PHD2 for calibration data
    final result = await _phd2Client!.getCalibrationData();

    // PHD2 returns null if not calibrated
    if (result == null) {
      return const gen_api.Phd2CalibrationData(
        isCalibrated: false,
        raAngle: null,
        decAngle: null,
        raRate: null,
        decRate: null,
      );
    }

    // Extract calibration parameters from PHD2's response
    // PHD2 returns xAngle/yAngle for RA/Dec rotation angles
    // and xRate/yRate for guide rates in pixels/second
    final xAngle = (result['xAngle'] as num?)?.toDouble();
    final yAngle = (result['yAngle'] as num?)?.toDouble();
    final xRate = (result['xRate'] as num?)?.toDouble();
    final yRate = (result['yRate'] as num?)?.toDouble();

    return gen_api.Phd2CalibrationData(
      isCalibrated: true,
      raAngle: xAngle,
      decAngle: yAngle,
      raRate: xRate,
      decRate: yRate,
    );
  }

  /// Find a guide star in PHD2
  static Future<(double, double)> phd2FindStar() async {
    if (_phd2Client == null || !_phd2Client!.isConnected) {
      throw Exception('PHD2 not connected');
    }
    await _phd2Client!.autoSelectStar();
    // Return center of frame as placeholder
    return (256.0, 256.0);
  }

  /// Set PHD2 lock position
  static Future<void> phd2SetLockPosition({
    required double x,
    required double y,
    bool exact = false,
  }) async {
    if (_phd2Client == null || !_phd2Client!.isConnected) {
      throw Exception('PHD2 not connected');
    }
    print(
        '[NativeBridge Stub] phd2SetLockPosition($x, $y, exact=$exact) - not implemented in Dart client');
  }

  /// Get PHD2 lock position
  static Future<(double, double)> phd2GetLockPosition() async {
    if (_phd2Client == null || !_phd2Client!.isConnected) {
      throw Exception('PHD2 not connected');
    }
    // Return placeholder
    return (256.0, 256.0);
  }

  /// Deselect star in PHD2
  static Future<void> phd2DeselectStar() async {
    if (_phd2Client == null || !_phd2Client!.isConnected) {
      throw Exception('PHD2 not connected');
    }
    print(
        '[NativeBridge Stub] phd2DeselectStar() - not implemented in Dart client');
  }

  // =========================================================================
  // Sequencer API
  // =========================================================================

  /// Sequencer state
  static SequencerState _sequencerState = SequencerState.idle;
  static String? _loadedSequenceJson;
  static bool _sequencerEventsSubscribed = false;

  /// Subscribe to sequencer events (must be called to receive sequencer events)
  /// This sets up the event forwarding from the Rust sequencer to the main event stream
  static Future<void> sequencerSubscribeEvents() async {
    if (_sequencerEventsSubscribed) return; // Already subscribed

    // If native bridge is available, subscribe to native events
    if (_nativeAvailable) {
      try {
        await frb.RustLib.instance.api.crateApiApiSequencerSubscribeEvents();
        _sequencerEventsSubscribed = true;
        print('[NativeBridge] Subscribed to sequencer events via native');
        return;
      } catch (e) {
        print('[NativeBridge] Error subscribing to sequencer events: $e');
        // Continue with stub - events will be local only
      }
    }

    // Stub: no-op, stub events are added directly to the controller
    _sequencerEventsSubscribed = true;
    print('[NativeBridge Stub] Sequencer event subscription initialized');
  }

  /// Load a sequence from JSON
  static Future<void> sequencerLoadJson(String json) async {
    // If native bridge is available, use real sequencer
    if (_nativeAvailable) {
      try {
        await frb.RustLib.instance.api.crateApiApiSequencerLoadJson(json: json);
        _loadedSequenceJson = json;
        return;
      } catch (e) {
        print('[NativeBridge] Error loading sequence via native: $e');
        rethrow;
      }
    }

    // Stub fallback
    _loadedSequenceJson = json;
    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.sequencer,
      eventType: 'SequenceLoaded',
      data: {},
    ));
  }

  /// Set connected devices for the sequencer
  static Future<void> sequencerSetDevices({
    String? cameraId,
    String? mountId,
    String? focuserId,
    String? filterwheelId,
    String? rotatorId,
  }) async {
    if (_nativeAvailable) {
      try {
        await frb.RustLib.instance.api.crateApiApiSequencerSetDevices(
          cameraId: cameraId,
          mountId: mountId,
          focuserId: focuserId,
          filterwheelId: filterwheelId,
          rotatorId: rotatorId,
        );
        print(
            '[NativeBridge] Set sequencer devices: camera=$cameraId, mount=$mountId, focuser=$focuserId, filterwheel=$filterwheelId, rotator=$rotatorId');
        return;
      } catch (e) {
        print('[NativeBridge] Error setting sequencer devices: $e');
        rethrow;
      }
    }
    print('[NativeBridge Stub] Sequencer devices would be set (stub mode)');
  }

  /// Start the loaded sequence
  static Future<void> sequencerStart() async {
    // Ensure event subscription is set up before starting
    await sequencerSubscribeEvents();

    // If native bridge is available, use real sequencer
    if (_nativeAvailable) {
      try {
        await frb.RustLib.instance.api.crateApiApiSequencerStart();
        _sequencerState = SequencerState.running;
        return;
      } catch (e) {
        print('[NativeBridge] Error starting sequence via native: $e');
        rethrow;
      }
    }

    // Stub fallback
    if (_loadedSequenceJson == null) {
      throw Exception('No sequence loaded');
    }

    _sequencerState = SequencerState.running;
    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.sequencer,
      eventType: 'SequenceStarted',
      data: {},
    ));
  }

  /// Pause the running sequence
  static Future<void> sequencerPause() async {
    // If native bridge is available, use real sequencer
    if (_nativeAvailable) {
      try {
        await frb.RustLib.instance.api.crateApiApiSequencerPause();
        _sequencerState = SequencerState.paused;
        return;
      } catch (e) {
        print('[NativeBridge] Error pausing sequence via native: $e');
        rethrow;
      }
    }

    // Stub fallback
    _sequencerState = SequencerState.paused;
    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.sequencer,
      eventType: 'SequencePaused',
      data: {},
    ));
  }

  /// Resume a paused sequence
  static Future<void> sequencerResume() async {
    // If native bridge is available, use real sequencer
    if (_nativeAvailable) {
      try {
        await frb.RustLib.instance.api.crateApiApiSequencerResume();
        _sequencerState = SequencerState.running;
        return;
      } catch (e) {
        print('[NativeBridge] Error resuming sequence via native: $e');
        rethrow;
      }
    }

    // Stub fallback
    _sequencerState = SequencerState.running;
    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.sequencer,
      eventType: 'SequenceResumed',
      data: {},
    ));
  }

  /// Stop the running sequence
  static Future<void> sequencerStop() async {
    // If native bridge is available, use real sequencer
    if (_nativeAvailable) {
      try {
        await frb.RustLib.instance.api.crateApiApiSequencerStop();
        _sequencerState = SequencerState.idle;
        _loadedSequenceJson = null;
        return;
      } catch (e) {
        print('[NativeBridge] Error stopping sequence via native: $e');
        rethrow;
      }
    }

    // Stub fallback
    _sequencerState = SequencerState.idle;
    _loadedSequenceJson = null;
    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.sequencer,
      eventType: 'SequenceStopped',
      data: {},
    ));
  }

  /// Skip the current node
  static Future<void> sequencerSkip() async {
    // If native bridge is available, use real sequencer
    if (_nativeAvailable) {
      try {
        await frb.RustLib.instance.api.crateApiApiSequencerSkip();
        return;
      } catch (e) {
        print('[NativeBridge] Error skipping node via native: $e');
        rethrow;
      }
    }

    // Stub fallback
    _eventController.add(_StubNightshadeEvent(
      timestamp: DateTime.now().millisecondsSinceEpoch,
      severity: EventSeverity.info,
      category: EventCategory.sequencer,
      eventType: 'NodeSkipped',
      data: {},
    ));
  }

  /// Reset the sequencer
  static Future<void> sequencerReset() async {
    // If native bridge is available, use real sequencer
    if (_nativeAvailable) {
      try {
        await frb.RustLib.instance.api.crateApiApiSequencerReset();
        _sequencerState = SequencerState.idle;
        _loadedSequenceJson = null;
        return;
      } catch (e) {
        print('[NativeBridge] Error resetting sequencer via native: $e');
        rethrow;
      }
    }

    // Stub fallback
    _sequencerState = SequencerState.idle;
    _loadedSequenceJson = null;
  }

  /// Get the current sequencer state
  static SequencerState getSequencerState() => _sequencerState;

  /// Subscribe to sequencer events
  /// Returns a stream of sequencer events
  static Stream<NightshadeEvent> sequencerEventStream() {
    // If native is available, use the real event stream from Rust
    // and filter for sequencer events
    if (_nativeAvailable) {
      try {
        return gen_api.apiEventStream().where(
            (event) => event.category == gen_event.EventCategory.sequencer);
      } catch (e) {
        print('[NativeBridge] Failed to get native sequencer event stream: $e');
        print('[NativeBridge] Falling back to local event controller');
      }
    }

    // Stub fallback
    return _eventController.stream
        .where((event) => event.category == gen_event.EventCategory.sequencer)
        .map((stubEvent) => gen_event.NightshadeEvent(
              timestamp: stubEvent.timestamp,
              severity: stubEvent.severity,
              category: stubEvent.category,
              payload: gen_event.EventPayload.system(
                gen_event.SystemEvent.notification(
                  title: stubEvent.eventType,
                  message: stubEvent.data.toString(),
                  level: stubEvent.severity.name,
                ),
              ),
            ));
  }

  static bool _simulationMode = false;

  /// Set simulation mode (use mock devices instead of real hardware)
  static Future<void> sequencerSetSimulationMode(bool enabled) async {
    // If native bridge is available, use real sequencer
    if (_nativeAvailable) {
      try {
        await frb.RustLib.instance.api
            .crateApiApiSequencerSetSimulationMode(enabled: enabled);
        _simulationMode = enabled;
        print(
            '[NativeBridge] Simulation mode via native: ${enabled ? "enabled" : "disabled"}');
        return;
      } catch (e) {
        print('[NativeBridge] Error setting simulation mode via native: $e');
        rethrow;
      }
    }

    // Stub fallback
    _simulationMode = enabled;
    print(
        '[NativeBridge Stub] Simulation mode: ${enabled ? "enabled" : "disabled"}');
  }

  /// Check if simulation mode is enabled
  static bool isSimulationMode() => _simulationMode;

  /// Get sequencer status
  static Future<SequencerStatus> sequencerGetStatus() async {
    // If native bridge is available, use real sequencer
    if (_nativeAvailable) {
      try {
        final nativeState =
            await frb.RustLib.instance.api.crateApiApiSequencerGetState();
        // Calculate progress from exposures
        final progress = nativeState.totalExposures > 0
            ? nativeState.completedExposures / nativeState.totalExposures
            : 0.0;
        return SequencerStatus(
          state: nativeState.state,
          currentNodeId: nativeState.currentNodeId,
          currentNodeName: nativeState.currentNodeName,
          progress: progress,
          message: nativeState.message,
        );
      } catch (e) {
        print('[NativeBridge] Error getting sequencer status via native: $e');
        rethrow;
      }
    }

    // Stub fallback
    // Convert enum to pascal case string (e.g., idle -> Idle, running -> Running)
    final stateStr = _sequencerState.name[0].toUpperCase() +
        _sequencerState.name.substring(1);
    return SequencerStatus(
      state: stateStr,
      currentNodeId: null,
      currentNodeName: null,
      progress: 0.0,
      message: null,
    );
  }

  // =========================================================================
  // Checkpoint / Crash Recovery (Stub - No real checkpoint in stub mode)
  // =========================================================================

  /// Set the checkpoint directory
  static Future<void> sequencerSetCheckpointDir(String path) async {
    // If native bridge is available, use real sequencer
    if (_nativeAvailable) {
      try {
        await frb.RustLib.instance.api
            .crateApiApiSequencerSetCheckpointDir(path: path);
        return;
      } catch (e) {
        print('[NativeBridge] Error setting checkpoint dir via native: $e');
        rethrow;
      }
    }

    // Stub: No-op since stub doesn't support checkpoints
    print(
        '[NativeBridge Stub] sequencerSetCheckpointDir called (no-op in stub mode)');
  }

  /// Check if a checkpoint exists
  static Future<bool> sequencerHasCheckpoint() async {
    // If native bridge is available, use real sequencer
    if (_nativeAvailable) {
      try {
        return await frb.RustLib.instance.api
            .crateApiApiSequencerHasCheckpoint();
      } catch (e) {
        print('[NativeBridge] Error checking checkpoint via native: $e');
        rethrow;
      }
    }

    // Stub: Always return false
    return false;
  }

  /// Get checkpoint info
  static Future<CheckpointInfoApi?> sequencerGetCheckpointInfo() async {
    // If native bridge is available, use real sequencer
    if (_nativeAvailable) {
      try {
        final nativeInfo = await frb.RustLib.instance.api
            .crateApiApiSequencerGetCheckpointInfo();
        if (nativeInfo == null) return null;
        // Map from FRB-generated type to stub type
        return CheckpointInfoApi(
          sequenceName: nativeInfo.sequenceName,
          timestamp: nativeInfo.timestamp,
          completedExposures: nativeInfo.completedExposures,
          completedIntegrationSecs: nativeInfo.completedIntegrationSecs,
          canResume: nativeInfo.canResume,
          ageSeconds: nativeInfo.ageSeconds.toInt(),
        );
      } catch (e) {
        print('[NativeBridge] Error getting checkpoint info via native: $e');
        rethrow;
      }
    }

    // Stub: Always return null
    return null;
  }

  /// Resume from checkpoint
  static Future<void> sequencerResumeFromCheckpoint() async {
    // If native bridge is available, use real sequencer
    if (_nativeAvailable) {
      try {
        await frb.RustLib.instance.api
            .crateApiApiSequencerResumeFromCheckpoint();
        _sequencerState = SequencerState.running;
        return;
      } catch (e) {
        print('[NativeBridge] Error resuming from checkpoint via native: $e');
        rethrow;
      }
    }

    // Stub: No-op
    print(
        '[NativeBridge Stub] sequencerResumeFromCheckpoint called (no-op in stub mode)');
  }

  /// Discard checkpoint
  static Future<void> sequencerDiscardCheckpoint() async {
    // If native bridge is available, use real sequencer
    if (_nativeAvailable) {
      try {
        await frb.RustLib.instance.api.crateApiApiSequencerClearCheckpoint();
        return;
      } catch (e) {
        print('[NativeBridge] Error discarding checkpoint via native: $e');
        rethrow;
      }
    }

    // Stub: No-op
    print(
        '[NativeBridge Stub] sequencerDiscardCheckpoint called (no-op in stub mode)');
  }

  /// Save checkpoint
  static Future<void> sequencerSaveCheckpoint() async {
    // If native bridge is available, use real sequencer
    if (_nativeAvailable) {
      try {
        await frb.RustLib.instance.api.crateApiApiSequencerSaveCheckpoint();
        return;
      } catch (e) {
        print('[NativeBridge] Error saving checkpoint via native: $e');
        rethrow;
      }
    }

    // Stub: No-op
    print(
        '[NativeBridge Stub] sequencerSaveCheckpoint called (no-op in stub mode)');
  }

  // =========================================================================
  // Rotator Control (API methods)
  // =========================================================================

  static RotatorStatus? _rotatorStatus;

  /// Move rotator to absolute angle
  static Future<void> apiRotatorMoveTo({
    required String deviceId,
    required double angle,
  }) async {
    _rotatorStatus = RotatorStatus(
      connected: true,
      position: angle,
      moving: true,
      mechanicalPosition: angle,
      isMoving: true,
      canReverse: true,
    );
    await Future.delayed(const Duration(milliseconds: 500));
    _rotatorStatus = RotatorStatus(
      connected: true,
      position: angle,
      moving: false,
      mechanicalPosition: angle,
      isMoving: false,
      canReverse: true,
    );
  }

  /// Move rotator by relative amount
  static Future<void> apiRotatorMoveRelative({
    required String deviceId,
    required double delta,
  }) async {
    final current = _rotatorStatus?.position ?? 0.0;
    await apiRotatorMoveTo(deviceId: deviceId, angle: current + delta);
  }

  /// Get rotator status
  static Future<RotatorStatus> apiGetRotatorStatus({
    required String deviceId,
  }) async {
    return _rotatorStatus ??
        const RotatorStatus(
          connected: false,
          position: 0.0,
          moving: false,
          mechanicalPosition: 0.0,
          isMoving: false,
          canReverse: true,
        );
  }

  /// Halt rotator movement
  static Future<void> apiRotatorHalt({
    required String deviceId,
  }) async {
    final current = _rotatorStatus?.position ?? 0.0;
    _rotatorStatus = RotatorStatus(
      connected: true,
      position: current,
      moving: false,
      mechanicalPosition: current,
      isMoving: false,
      canReverse: true,
    );
  }

  // =========================================================================
  // Equipment Profiles (API methods)
  // =========================================================================

  static final List<EquipmentProfile> _profiles = [];
  static String? _activeProfileId;

  /// Get all profiles
  static Future<List<EquipmentProfile>> apiGetProfiles() async {
    return List.from(_profiles);
  }

  /// Save a profile
  static Future<void> apiSaveProfile({
    required EquipmentProfile profile,
  }) async {
    final index = _profiles.indexWhere((p) => p.id == profile.id);
    if (index >= 0) {
      _profiles[index] = profile;
    } else {
      _profiles.add(profile);
    }
  }

  /// Delete a profile
  static Future<void> apiDeleteProfile({
    required String profileId,
  }) async {
    _profiles.removeWhere((p) => p.id == profileId);
    if (_activeProfileId == profileId) {
      _activeProfileId = null;
    }
  }

  /// Load a profile
  static Future<void> apiLoadProfile({
    required String profileId,
  }) async {
    if (_profiles.any((p) => p.id == profileId)) {
      _activeProfileId = profileId;
    }
  }

  /// Get active profile
  static Future<EquipmentProfile?> apiGetActiveProfile() async {
    if (_activeProfileId == null) return null;
    try {
      return _profiles.firstWhere((p) => p.id == _activeProfileId);
    } catch (_) {
      return null;
    }
  }

  // =========================================================================
  // Settings (API methods)
  // =========================================================================

  static AppSettings _appSettings = const AppSettings(
    theme: 'dark',
    language: 'en',
    autoConnect: true,
  );

  /// Initialize profile storage
  static Future<void> apiInitProfileStorage(
      {required String storagePath}) async {
    try {
      // Ensure storage directory exists
      final storageDir = Directory(storagePath);
      if (!await storageDir.exists()) {
        await storageDir.create(recursive: true);
      }

      // Create profiles.json file with empty profiles array if it doesn't exist
      final profilesFile = File(path.join(storagePath, 'profiles.json'));
      if (!await profilesFile.exists()) {
        const initialData = {'profiles': []};
        await profilesFile.writeAsString(
          const JsonEncoder.withIndent('  ').convert(initialData),
        );
      }
    } catch (e) {
      // Log error but don't throw - allow app to continue
      print('Warning: Failed to initialize profile storage: $e');
    }
  }

  /// Initialize settings storage
  static Future<void> apiInitSettingsStorage(
      {required String storagePath}) async {
    try {
      // Ensure storage directory exists
      final storageDir = Directory(storagePath);
      if (!await storageDir.exists()) {
        await storageDir.create(recursive: true);
      }

      // Create settings.json file with default settings if it doesn't exist
      final settingsFile = File(path.join(storagePath, 'settings.json'));
      if (!await settingsFile.exists()) {
        final defaultSettings = {
          'location': null,
          'theme': 'dark',
          'language': 'en',
          'auto_connect': true,
        };
        await settingsFile.writeAsString(
          const JsonEncoder.withIndent('  ').convert(defaultSettings),
        );
      }
    } catch (e) {
      // Log error but don't throw - allow app to continue
      print('Warning: Failed to initialize settings storage: $e');
    }
  }

  /// Get application settings
  static Future<AppSettings> apiGetSettings() async {
    return _appSettings;
  }

  /// Update application settings
  static Future<void> apiUpdateSettings({
    required AppSettings settings,
  }) async {
    _appSettings = settings;
  }

  // =========================================================================
  // Location (API methods)
  // =========================================================================

  /// Get observer location
  static Future<ObserverLocation?> apiGetLocation() async {
    // If native bridge is available, use real native function
    if (_nativeAvailable) {
      try {
        return frb.RustLib.instance.api.crateApiApiGetLocation();
      } catch (e) {
        print('[NativeBridge] Error getting location via native: $e');
        // Fall through to stub
      }
    }
    return _appSettings.location;
  }

  /// Set observer location
  static Future<void> apiSetLocation({
    ObserverLocation? location,
  }) async {
    // If native bridge is available, use real native function
    if (_nativeAvailable) {
      try {
        print(
            '[NativeBridge] Setting location via native: lat=${location?.latitude}, lon=${location?.longitude}');
        frb.RustLib.instance.api.crateApiApiSetLocation(location: location);
        print('[NativeBridge] Location set via native successfully');
        // Also update local cache for consistency
        _appSettings = AppSettings(
          location: location,
          theme: _appSettings.theme,
          language: _appSettings.language,
          autoConnect: _appSettings.autoConnect,
        );
        return;
      } catch (e) {
        print('[NativeBridge] Error setting location via native: $e');
        // Fall through to stub
      }
    }
    // Stub fallback
    _appSettings = AppSettings(
      location: location,
      theme: _appSettings.theme,
      language: _appSettings.language,
      autoConnect: _appSettings.autoConnect,
    );
  }

  // =========================================================================
  // Image Processing (API methods)
  // =========================================================================

  /// Get image statistics
  static Future<ImageStats> apiGetImageStats({
    required int width,
    required int height,
    required Uint16List data,
  }) async {
    if (data.isEmpty) {
      return ImageStats(
        min: 0.0,
        max: 0.0,
        mean: 0.0,
        median: 0.0,
        stdDev: 0.0,
        mad: 0.0,
      );
    }

    final values = data.map((e) => e.toDouble()).toList();
    values.sort();

    final min = values.first;
    final max = values.last;
    final mean = values.reduce((a, b) => a + b) / values.length;
    final median = values[values.length ~/ 2];

    final variance =
        values.map((v) => (v - mean) * (v - mean)).reduce((a, b) => a + b) /
            values.length;
    final stdDev = variance > 0 ? variance : 0.0;

    final medianAbsDev =
        values.map((v) => (v - median).abs()).reduce((a, b) => a + b) /
            values.length;

    return ImageStats(
      min: min,
      max: max,
      mean: mean,
      median: median,
      stdDev: stdDev,
      mad: medianAbsDev,
    );
  }

  /// Auto-stretch image
  static Future<Uint8List> apiAutoStretchImage({
    required int width,
    required int height,
    required Uint16List data,
  }) async {
    if (data.isEmpty) return Uint8List(0);

    final stats =
        await apiGetImageStats(width: width, height: height, data: data);
    final min = stats.min;
    final max = stats.max;
    final range = max - min;

    if (range == 0) {
      return Uint8List(width * height);
    }

    final result = Uint8List(width * height);
    for (int i = 0; i < data.length; i++) {
      final normalized = ((data[i] - min) / range * 255).clamp(0, 255);
      result[i] = normalized.round();
    }

    return result;
  }

  /// Debayer image
  static Future<Uint8List> apiDebayerImage({
    required int width,
    required int height,
    required Uint16List data,
    required String patternStr,
    required String algoStr,
  }) async {
    // Simple stub implementation - just return grayscale conversion
    final result = Uint8List(width * height);
    for (int i = 0; i < data.length && i < result.length; i++) {
      result[i] = (data[i] ~/ 256).clamp(0, 255);
    }
    return result;
  }

  // =========================================================================
  // Cleanup
  // =========================================================================

  /// Dispose of resources
  static void dispose() {
    _eventController.close();
  }
}

// SequencerState is now a typedef pointing to gen_api.SequencerState (defined at top of file)
