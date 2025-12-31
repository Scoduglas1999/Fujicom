# Equipment Connection Screen Redesign

**Date:** 2025-12-31
**Status:** Design Approved

## Overview

Redesign of the Equipment Connection Screen to be commercial-quality, serving three user personas:
- **Setup-once users**: Configure once, use daily with minimal friction
- **Multi-rig operators**: Quickly switch between equipment configurations
- **Learning users**: Need guidance on what to connect and why

## Design Principles

1. **Profile-first interaction** - Profiles are the primary UI element, not device cards
2. **Progressive disclosure** - Show complexity only when needed
3. **Adaptive zones** - UI changes based on connection state
4. **One-click workflow** - Daily use requires minimal clicks
5. **Guided onboarding** - First-time users get step-by-step help

## Layout Structure

Three-zone vertical layout:

```
┌─────────────────────────────────────────────────────────┐
│  ZONE 1: Quick Connect Bar (always visible, compact)    │
│  Horizontal scrollable profile chips with connection    │
│  state indicators                                       │
├─────────────────────────────────────────────────────────┤
│  ZONE 2: Connection Status (expands based on state)     │
│  - Disconnected: Profile preview + "Connect All"        │
│  - Connecting: Live progress per device                 │
│  - Connected: Compact status bar                        │
│  - Error: Actionable error with retry options           │
├─────────────────────────────────────────────────────────┤
│  ZONE 3: Device Management (tabbed, scrollable)         │
│  [Discovery] [Connected Devices] [Settings]             │
└─────────────────────────────────────────────────────────┘
```

## Zone 1: Quick Connect Bar

### Visual Design
- Horizontal scrollable row of profile chips
- Selected profile has elevated appearance (shadow, accent border)
- Each chip shows: Profile name, device count, connection indicator

### Connection Indicators
```
○  Disconnected (outline)
◐  Connecting (animated, half-filled)
●  Connected (filled, green accent)
⚠  Error (amber accent)
```

### Chip Content
```dart
ProfileChip(
  name: "Observatory",
  deviceCount: 5,
  state: ConnectionState.disconnected,
  isSelected: true,
)
```

### Interactions
- **Tap disconnected profile**: Show Zone 2 expanded with preview + "Connect All"
- **Tap different profile while connected**: Show switch confirmation dialog
- **Long-press/right-click**: Context menu (Edit, Duplicate, Set Default, Delete)
- **Tap "+" chip**: Open create profile flow

## Zone 2: Connection Status Zone

Adaptive height zone that changes based on connection state.

### State: Disconnected (Expanded)
Shows selected profile's device list with connect action:

```
┌─────────────────────────────────────────────────────────┐
│  Observatory Profile                                     │
│                                                         │
│  Camera:      ZWO ASI2600MM Pro                         │
│  Mount:       Sky-Watcher EQ6-R Pro                     │
│  Focuser:     ZWO EAF                                   │
│  Filter:      ZWO EFW (8-position)                      │
│  Guider:      ZWO ASI290MM Mini                         │
│                                                         │
│  [Connect All]                    [Edit Profile]        │
└─────────────────────────────────────────────────────────┘
```

### State: Connecting (Animated)
Shows live progress with per-device status:

```
┌─────────────────────────────────────────────────────────┐
│  Connecting Observatory Profile...                       │
│                                                         │
│  ✓ Camera        ZWO ASI2600MM Pro          Connected   │
│  ◐ Mount         Sky-Watcher EQ6-R Pro      Connecting  │
│  ○ Focuser       ZWO EAF                    Waiting     │
│  ○ Filter        ZWO EFW                    Waiting     │
│  ○ Guider        ZWO ASI290MM Mini          Waiting     │
│                                                         │
│  [Cancel]                                               │
└─────────────────────────────────────────────────────────┘
```

Progress indicators:
- `✓` Check mark with green color for connected
- `◐` Animated spinner for connecting
- `○` Empty circle for waiting
- `✗` X mark with red color for failed

### State: Connected (Compact Bar)
Minimal height showing health at a glance:

```
┌─────────────────────────────────────────────────────────┐
│ ● All Connected   Camera ●  Mount ●  Focuser ●  ...    │
└─────────────────────────────────────────────────────────┘
```

- Tap to expand and show device details
- Each device dot is clickable to jump to that device's settings

### State: Partial/Error (Attention Required)
Prominent error with actionable options:

```
┌─────────────────────────────────────────────────────────┐
│ ⚠ 4/5 Connected                                [Retry]  │
│                                                         │
│  ✗ Focuser failed: Device not responding               │
│    [Retry] [Skip] [Troubleshoot]                       │
└─────────────────────────────────────────────────────────┘
```

## Zone 3: Device Management Tabs

### Discovery Tab
Shows all available devices across backends, grouped by type:

```
┌─────────────────────────────────────────────────────────┐
│  Backends: [ASCOM ●] [INDI ○] [Alpaca ○] [Native ●]    │
│                                                         │
│  ▼ Cameras (3 found)                                    │
│    ZWO ASI2600MM Pro          ASCOM    [Add to Profile] │
│    ZWO ASI290MM Mini          ASCOM    [Add to Profile] │
│    QHY268M                    Native   [Add to Profile] │
│                                                         │
│  ▼ Mounts (1 found)                                     │
│    Sky-Watcher EQ6-R Pro      ASCOM    [Add to Profile] │
│                                                         │
│  ▶ Focusers (0 found)                                   │
│                                                         │
│  [Refresh Discovery]                                    │
└─────────────────────────────────────────────────────────┘
```

Features:
- Collapsible sections per device type
- Backend filter chips (toggle on/off)
- "Add to Profile" adds to currently selected profile
- "Refresh Discovery" re-scans all enabled backends
- Empty sections collapsed by default

### Connected Tab
Shows currently connected devices with live telemetry:

```
┌─────────────────────────────────────────────────────────┐
│  ┌─ Camera: ZWO ASI2600MM Pro ─────────────────────────┐│
│  │  Temperature: -10.0°C (target: -10°C) ✓             ││
│  │  Cooler: 45% power                                  ││
│  │  Status: Idle                                       ││
│  │  [Settings] [Disconnect]                            ││
│  └─────────────────────────────────────────────────────┘│
│                                                         │
│  ┌─ Mount: Sky-Watcher EQ6-R Pro ──────────────────────┐│
│  │  Position: RA 12h 30m, Dec +45° 20'                 ││
│  │  Tracking: Sidereal ●                               ││
│  │  [Park] [Settings] [Disconnect]                     ││
│  └─────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────┘
```

Each device card shows:
- Device type icon and name
- Key telemetry (2-3 most important values)
- Quick actions relevant to device type
- Settings and Disconnect buttons

### Settings Tab
Equipment-related global settings:
- Default profile (auto-select on app launch)
- Connection timeout duration
- Auto-reconnect on disconnect
- Backend priority order
- Parallel vs sequential connection
- Cooler warmup on disconnect option

## First-Time User Experience

When no profiles exist, show onboarding overlay:

```
┌─────────────────────────────────────────────────────────┐
│                                                         │
│     Welcome to Nightshade                               │
│                                                         │
│     Let's set up your first equipment profile           │
│                                                         │
│     ┌─────────────────────────────────────────────────┐ │
│     │  1. We'll scan for connected equipment          │ │
│     │  2. Select the devices you want to use          │ │
│     │  3. Save as a profile for one-click connection  │ │
│     └─────────────────────────────────────────────────┘ │
│                                                         │
│     [Start Setup]         [I'll do it manually]         │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

**Guided Setup Flow:**
1. Scan all backends for devices (show progress)
2. Present grouped device list with checkboxes
3. Pre-select common devices (camera, mount)
4. Ask for profile name with suggestions
5. Offer to connect immediately after save

## Profile Data Model

```dart
class EquipmentProfile {
  final String id;
  final String name;
  final String? description;
  final bool isDefault;
  final DateTime createdAt;
  final DateTime lastUsedAt;

  // Device assignments (device type -> device ID)
  final String? cameraId;
  final String? mountId;
  final String? focuserId;
  final String? filterWheelId;
  final String? guideCameraId;
  final String? rotatorId;

  // Per-profile settings
  final CoolerSettings? coolerSettings;
  final MountSettings? mountSettings;
}
```

## Widget Hierarchy

```
EquipmentScreen
├── QuickConnectBar
│   ├── ProfileChip (multiple)
│   └── AddProfileChip
├── ConnectionStatusZone
│   ├── DisconnectedView
│   │   └── ProfilePreviewList
│   ├── ConnectingView
│   │   └── DeviceProgressList
│   ├── ConnectedView
│   │   └── CompactStatusBar
│   └── ErrorView
│       └── ErrorActionButtons
└── DeviceManagementTabs
    ├── DiscoveryTab
    │   ├── BackendFilterChips
    │   └── DeviceTypeSection (multiple)
    │       └── DiscoveredDeviceRow (multiple)
    ├── ConnectedTab
    │   └── ConnectedDeviceCard (multiple)
    └── SettingsTab
        └── EquipmentSettingsForm
```

## State Management

### Providers Needed

```dart
// Current profile selection
final selectedProfileProvider = StateProvider<String?>((ref) => null);

// All profiles
final equipmentProfilesProvider = StreamProvider<List<EquipmentProfile>>(...);

// Connection state per profile
final profileConnectionStateProvider = StateNotifierProvider<...>(...);

// Currently connecting devices progress
final connectionProgressProvider = StateProvider<Map<DeviceType, ConnectionProgress>>(...);

// Discovery results
final discoveredDevicesProvider = FutureProvider<List<DiscoveredDevice>>(...);
```

### Connection State Machine

```
disconnected -> connecting -> connected
                    |
                    v
                  error -> retrying -> connected
                    |                      |
                    v                      v
                 partial_connected    disconnected
```

## Animations

1. **Profile chip selection**: Scale and shadow animation
2. **Zone 2 height transitions**: Smooth height animation with content fade
3. **Connection progress**: Sequential reveal of device rows
4. **Status indicators**: Pulse animation for connecting state
5. **Error state**: Subtle shake animation to draw attention

## Accessibility

- All interactive elements have minimum 48x48 touch targets
- Screen reader announces connection state changes
- Color is never the only indicator (always paired with icons/text)
- Keyboard navigation through profile chips with arrow keys
- Focus indicators on all interactive elements

## Implementation Phases

### Phase 1: Core Structure
- [ ] Create new widget hierarchy
- [ ] Implement QuickConnectBar with profile chips
- [ ] Implement ConnectionStatusZone with state variants
- [ ] Add basic animations

### Phase 2: Profile Management
- [ ] Update EquipmentProfile model
- [ ] Create profile CRUD operations
- [ ] Implement "Connect All" functionality
- [ ] Add profile context menu

### Phase 3: Discovery Integration
- [ ] Redesign Discovery tab layout
- [ ] Add "Add to Profile" action
- [ ] Implement backend filter chips
- [ ] Add collapsible device type sections

### Phase 4: Connected Devices
- [ ] Redesign Connected tab with telemetry
- [ ] Add device-specific quick actions
- [ ] Implement individual disconnect

### Phase 5: Onboarding
- [ ] Create first-time user detection
- [ ] Implement guided setup wizard
- [ ] Add profile name suggestions

### Phase 6: Polish
- [ ] Refine animations and transitions
- [ ] Add loading states
- [ ] Implement error recovery flows
- [ ] Accessibility audit

## Migration Notes

- Existing profiles should auto-migrate (schema compatible)
- Old connections_tab.dart will be replaced entirely
- profiles_tab.dart functionality merges into main screen
- settings_tab.dart remains largely unchanged
