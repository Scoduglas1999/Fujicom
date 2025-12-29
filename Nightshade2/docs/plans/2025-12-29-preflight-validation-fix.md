# Pre-Flight Validation Fix Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Make pre-flight equipment check sequence-aware and fix guider detection

**Architecture:** Add `requiredDevices` property to SequenceNode classes, update validation to only check devices the sequence actually needs, fix guider detection to use `guiderStateProvider` instead of `getConnectedDevices()`

**Tech Stack:** Dart/Flutter, Riverpod

---

## Task 1: Add requiredDevices to SequenceNode Base Class

**Files:**
- Modify: `packages/nightshade_core/lib/src/models/sequence/sequence_models.dart`

Add import for DeviceType at top, then add to SequenceNode base class:
```dart
Set<DeviceType> get requiredDevices => {};
```

---

## Task 2: Add requiredDevices to Camera Nodes

**Files:**
- Modify: `packages/nightshade_core/lib/src/models/sequence/sequence_models.dart`

Add to ExposureNode, CoolCameraNode, WarmCameraNode:
```dart
@override
Set<DeviceType> get requiredDevices => {DeviceType.camera};
```

---

## Task 3: Add requiredDevices to Mount Nodes

Add to TargetGroupNode, ParkNode, UnparkNode, MeridianFlipNode (if exists):
```dart
@override
Set<DeviceType> get requiredDevices => {DeviceType.mount};
```

---

## Task 4: Add requiredDevices to Focuser/Filter Nodes

AutofocusNode:
```dart
@override
Set<DeviceType> get requiredDevices => {DeviceType.camera, DeviceType.focuser};
```

FilterChangeNode:
```dart
@override
Set<DeviceType> get requiredDevices => {DeviceType.filterWheel};
```

---

## Task 5: Add requiredDevices to Guiding Nodes

Add to StartGuidingNode, StopGuidingNode, DitherNode:
```dart
@override
Set<DeviceType> get requiredDevices => {DeviceType.guider};
```

---

## Task 6: Add requiredDevices to Rotator/Dome Nodes

RotatorMoveNode: `{DeviceType.rotator}`
Dome nodes: `{DeviceType.dome}`

---

## Task 7: Rewrite _checkEquipment in PreFlightValidationDialog

**Files:**
- Modify: `packages/nightshade_app/lib/screens/sequencer/widgets/preflight_validation_dialog.dart`

1. Import guiderStateProvider
2. Change signature to accept Sequence
3. Collect requiredDevices from all enabled nodes
4. Check guider via guiderStateProvider (not getConnectedDevices)
5. Only validate devices that are required
6. Update validate() to pass sequence

---

## Task 8: Verify Build and Test

Run `melos run analyze`, test scenarios manually.
