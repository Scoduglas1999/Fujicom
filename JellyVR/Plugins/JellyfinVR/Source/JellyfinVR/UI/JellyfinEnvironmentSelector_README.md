# JellyfinEnvironmentSelector Widget

## Overview

The `UJellyfinEnvironmentSelector` widget provides a user interface for browsing and switching between different VR viewing environments in JellyVR. It automatically handles platform filtering (Quest vs PCVR), environment loading, and provides Blueprint events for UI updates.

## Architecture

```
UJellyfinEnvironmentSelector (C++ Widget)
    ├── Loads from Data Table (DT_Environments)
    ├── Uses UJellyfinEnvironmentManager for level loading
    └── Provides Blueprint events for UI implementation
```

## C++ API

### Key Methods

#### `void RefreshEnvironments()`
Loads environments from the data table and rebuilds the display list. Call this when:
- Widget is first constructed (automatic)
- Need to refresh after data table changes
- Returning to the selector after environment changes

#### `void SelectEnvironment(const FString& EnvironmentId)`
Loads a selected environment. Parameters:
- `EnvironmentId`: The row name from the data table (e.g., "TestRoom")

Triggers:
- `OnEnvironmentChanging` - Before level transition
- `OnEnvironmentLoaded` - On success
- `OnEnvironmentLoadFailed` - On failure

#### `FString GetCurrentEnvironmentId()`
Returns the currently loaded environment's ID.

#### `TArray<FJellyfinEnvironmentDisplayInfo> GetAvailableEnvironments()`
Returns all available environments (filtered by platform if enabled).

### Blueprint Events

#### `OnEnvironmentsReady(Environments)`
Called when the environment list is loaded and ready to display.

**Parameters:**
- `Environments`: Array of `FJellyfinEnvironmentDisplayInfo` structs

**Use Case:** Populate your grid/list of environment cards

**Example Blueprint Logic:**
```
OnEnvironmentsReady
    ├── Clear existing environment cards
    ├── For Each Environment in Environments
    │   ├── Create Widget: WBP_EnvironmentCard
    │   ├── Set Card Data (DisplayName, Description, PreviewImage)
    │   ├── Set "Current" indicator if bIsCurrent == true
    │   └── Add to Scroll Box
    └── Show selector panel
```

#### `OnEnvironmentLoaded(EnvironmentId, DisplayName)`
Called when an environment successfully loads.

**Parameters:**
- `EnvironmentId`: The row name
- `DisplayName`: User-friendly name

**Use Case:** Show success message, update current indicator, hide selector

#### `OnEnvironmentLoadFailed(EnvironmentId, ErrorMessage)`
Called when environment loading fails.

**Parameters:**
- `EnvironmentId`: The failed environment
- `ErrorMessage`: Human-readable error

**Use Case:** Show error dialog, keep selector open

#### `OnEnvironmentChanging(FromEnvironment, ToEnvironment)`
Called before level transition begins.

**Parameters:**
- `FromEnvironment`: Current environment ID
- `ToEnvironment`: New environment ID

**Use Case:** Show loading screen, save state, disable input

### Properties

#### `bool bFilterByPlatform = true`
If true, automatically filters out PCVR-only environments when running on Quest standalone.

#### `TSoftObjectPtr<UDataTable> EnvironmentDataTable`
Path to the data table asset. Default: `/JellyfinVR/Data/DT_Environments`

## Data Structures

### FJellyfinEnvironmentDisplayInfo
```cpp
struct FJellyfinEnvironmentDisplayInfo
{
    FString EnvironmentId;        // Row name from data table
    FString DisplayName;          // User-friendly name
    FString Description;          // Environment description
    UTexture2D* PreviewImage;     // Loaded preview texture (or nullptr)
    bool bRequiresPCVR;           // Whether this needs PCVR
    bool bIsCurrent;              // Whether this is currently loaded
};
```

## Blueprint Implementation Guide

### Step 1: Create Blueprint Widget

1. Create new Widget Blueprint: `WBP_EnvironmentSelector`
2. Set Parent Class to: `JellyfinEnvironmentSelector` (C++ class)
3. Design your UI layout:
   - Scroll Box for environment grid
   - Loading indicator overlay
   - Error message text block
   - Close/Back button

### Step 2: Create Environment Card Widget

Create `WBP_EnvironmentCard` with:
- Image widget for preview (bind to PreviewImage)
- Text blocks for DisplayName and Description
- Border/overlay for "Current" indicator
- Button for selection

### Step 3: Implement Event Handlers

In `WBP_EnvironmentSelector` Blueprint graph:

**Event Construct:**
```
Event Construct
    └── (RefreshEnvironments is called automatically)
```

**OnEnvironmentsReady:**
```
Event OnEnvironmentsReady
    ├── Clear Environment Cards Container
    ├── ForEachLoop (Environments array)
    │   ├── Create Widget: WBP_EnvironmentCard
    │   ├── Set Card Display Info
    │   │   ├── DisplayName → Text
    │   │   ├── Description → Text
    │   │   ├── PreviewImage → Image
    │   │   └── bIsCurrent → Show/Hide "Current" badge
    │   ├── Bind Card Click Event → SelectEnvironment(EnvironmentId)
    │   └── Add to Scroll Box
    └── Hide Loading, Show Grid
```

**OnEnvironmentLoaded:**
```
Event OnEnvironmentLoaded
    ├── Hide Loading Indicator
    ├── Show Success Message (optional)
    └── Close Selector Widget (navigate back to main UI)
```

**OnEnvironmentLoadFailed:**
```
Event OnEnvironmentLoadFailed
    ├── Hide Loading Indicator
    ├── Show Error Dialog
    │   ├── Title: "Failed to Load Environment"
    │   └── Message: ErrorMessage parameter
    └── Keep Selector Open
```

**OnEnvironmentChanging:**
```
Event OnEnvironmentChanging
    ├── Show Loading Indicator
    ├── Set Loading Text: "Loading {ToEnvironment}..."
    └── Disable Environment Cards (prevent multi-click)
```

### Step 4: Handle Selection

Create click handler for environment cards:

```
OnCardClicked (custom event in WBP_EnvironmentCard)
    ├── Get EnvironmentId from card data
    └── Call Parent: SelectEnvironment(EnvironmentId)
```

Or in `WBP_EnvironmentSelector`:

```
BindCardClickEvents
    ├── For Each Card
    │   └── OnClicked.AddDynamic
    │       └── SelectEnvironment(Card.EnvironmentId)
```

## Integration Example

### Adding to Settings Menu

In your `WBP_Settings` or main menu:

1. Add "Change Environment" button
2. On Click:
   ```
   Create Widget: WBP_EnvironmentSelector
       ├── Add to Viewport (or add to settings panel)
       └── Set Focus to selector
   ```

### Adding to Radial Menu (VR)

In your VR radial menu:

1. Add "Environments" option
2. On Select:
   ```
   Show Environment Selector on Screen
       ├── Get JellyfinScreenActor
       ├── Create Widget: WBP_EnvironmentSelector
       └── SetOwningScreen (for VR widget interaction)
   ```

## Data Table Setup

Environments are defined in `/JellyfinVR/Content/Data/DT_Environments` (Data Table):

### Adding New Environments

1. Open Data Table in Unreal Editor
2. Add Row with structure:
   ```
   Row Name: "MyCustomRoom" (must be unique ID)
   LevelPath: /Game/Environments/Environment_MyRoom
   DisplayName: "My Custom Room"
   Description: "A cozy viewing environment"
   PreviewImage: Texture2D asset reference
   RequiresPCVR: false (or true if too demanding for Quest)
   SortOrder: 10 (lower = appears earlier in list)
   ```

### Row Name Convention
Use PascalCase IDs that match your level names for clarity:
- `TestRoom` → `Environment_TestRoom` level
- `SpaceStation` → `Environment_SpaceStation` level
- `HomeTheater` → `Environment_HomeTheater` level

## Platform Filtering

The widget automatically filters environments based on platform:

- **Quest Standalone**: Shows all environments where `bRequiresPCVR == false`
- **PCVR (Win64)**: Shows all environments

Disable filtering:
```cpp
// In Blueprint or C++
EnvironmentSelector->bFilterByPlatform = false;
```

## Performance Considerations

### Preview Images
- Use compressed textures (DXT1/5 or ASTC on Quest)
- Recommended size: 512x512 or 1024x1024
- Load synchronously (small images) - already handled by widget

### Level Streaming
Current implementation uses `UGameplayStatics::OpenLevel()` which:
- ✅ Simple and reliable
- ✅ Fully unloads previous level
- ⚠️ Causes brief screen transition
- ⚠️ Resets all actors (screen, player, etc.)

For seamless transitions, consider implementing:
- Level streaming with `ULevelStreamingDynamic`
- Persistent player state across level changes
- Smooth fade transitions during environment change

## Example UI Flow

```
User clicks "Change Environment" button
    ↓
WBP_EnvironmentSelector appears
    ↓
OnEnvironmentsReady fires
    ↓
Grid of environment cards displayed
    ↓
User clicks "Space Station" card
    ↓
OnEnvironmentChanging fires → Show "Loading Space Station..."
    ↓
SelectEnvironment("SpaceStation") called
    ↓
Level loads → Brief transition
    ↓
OnEnvironmentLoaded fires → Success message
    ↓
Selector closes, user in new environment
```

## Troubleshooting

### "Failed to load environment data table"
- Check `EnvironmentDataTable` path is correct
- Ensure data table exists in `/JellyfinVR/Content/Data/`
- Verify data table row structure is `FJellyfinEnvironmentInfo`

### "Environment not found"
- Verify row name matches exactly (case-sensitive)
- Check data table has been saved and loaded in editor

### "This environment requires PC VR"
- Environment has `bRequiresPCVR = true` and you're on Quest
- Either run on PCVR or set `bFilterByPlatform = false` (not recommended)

### Preview images not showing
- Check `PreviewImage` soft object pointer is valid
- Ensure texture assets are in correct directory
- Verify textures are not editor-only

### Level fails to load
- Check `LevelPath` is correct FSoftObjectPath format
- Ensure level exists and is packaged for target platform
- Review Unreal Engine logs for level streaming errors

## Future Enhancements

Potential improvements for production:

1. **Smooth Transitions**: Use level streaming instead of OpenLevel
2. **Async Preview Loading**: Load preview images asynchronously with placeholders
3. **Environment Favorites**: Save user's preferred environments
4. **Dynamic Environments**: Support runtime environment modifications
5. **Environment Previews**: 360° preview before loading
6. **Quick Switch**: Hotkey to toggle between recent environments
7. **Per-Media Environments**: Auto-select environment based on content type

## See Also

- `JellyfinEnvironmentTypes.h` - Environment data structures and manager
- `JellyfinVRWidget.h` - Base VR widget class
- `DT_Environments` - Environment data table asset
