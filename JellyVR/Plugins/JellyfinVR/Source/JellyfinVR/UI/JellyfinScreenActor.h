// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "API/JellyfinTypes.h"
#include "JellyfinScreenActor.generated.h"

class UStaticMeshComponent;
class UWidgetComponent;
class UMaterialInstanceDynamic;
class UJellyfinMediaPlayerComponent;
class UJellyfinClient;

UENUM(BlueprintType)
enum class EJellyfinScreenMode : uint8
{
	UI,       // Showing Jellyfin browser UI
	Video,    // Playing video
	Settings  // Showing settings/login
};

UENUM(BlueprintType)
enum class EJellyfinScreenShape : uint8
{
	Flat,
	CurvedSubtle,   // ~10 degree curve
	CurvedMedium,   // ~30 degree curve
	CurvedWrap      // ~60 degree curve for immersive viewing
};

/**
 * Main screen actor for Jellyfin VR
 * Place this in your environment to add a Jellyfin viewing screen
 */
UCLASS(BlueprintType, Blueprintable)
class JELLYFINVR_API AJellyfinScreenActor : public AActor
{
	GENERATED_BODY()

public:
	AJellyfinScreenActor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// ============ Screen Configuration ============

	/** Screen width in world units (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Screen", meta = (ClampMin = "100", ClampMax = "2000"))
	float ScreenWidth = 400.0f;

	/** Screen aspect ratio (width/height) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Screen")
	float AspectRatio = 16.0f / 9.0f;

	/** Screen shape */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Screen")
	EJellyfinScreenShape ScreenShape = EJellyfinScreenShape::Flat;

	/** Enable ambient glow effect around screen */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Screen")
	bool bEnableAmbientGlow = true;

	/** Ambient glow intensity */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Screen", meta = (ClampMin = "0", ClampMax = "5", EditCondition = "bEnableAmbientGlow"))
	float AmbientGlowIntensity = 1.0f;

	/** Screen emissive brightness */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Screen", meta = (ClampMin = "1", ClampMax = "20"))
	float ScreenBrightness = 5.0f;

	// ============ VR Interaction ============

	/** Distance at which the screen becomes interactive */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Interaction", meta = (ClampMin = "100", ClampMax = "5000"))
	float InteractionDistance = 1000.0f;

	/** Auto-hide controls after this many seconds (0 = never hide) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Interaction", meta = (ClampMin = "0", ClampMax = "60"))
	float ControlsAutoHideDelay = 5.0f;

	// ============ Blueprint Functions ============

	/** Get current screen mode */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Screen")
	EJellyfinScreenMode GetScreenMode() const { return ScreenMode; }

	/** Switch to UI mode (library browser) */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Screen")
	void ShowUI();

	/** Switch to video mode */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Screen")
	void ShowVideo();

	/** Switch to settings mode */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Screen")
	void ShowSettings();

	/** Play an item */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Screen")
	void PlayItem(const FJellyfinMediaItem& Item);

	/** Get the media player component */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Screen")
	UJellyfinMediaPlayerComponent* GetMediaPlayer() const { return MediaPlayer; }

	/** Get the Jellyfin client */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Screen")
	UJellyfinClient* GetJellyfinClient() const;

	/** Show/hide playback controls overlay */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Screen")
	void SetControlsVisible(bool bVisible);

	/** Toggle controls visibility */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Screen")
	void ToggleControls();

	/** Update screen dimensions at runtime */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Screen")
	void UpdateScreenDimensions(float NewWidth, float NewAspectRatio);

	/** Navigate to library browser */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Screen")
	void ShowLibrary(const FString& LibraryId, const FString& LibraryName);

	/** Navigate back to home from library */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Screen")
	void ShowHome();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	void SetupScreenMesh();
	void SetupMaterials();
	void SetupWidgets();
	void UpdateScreenTransform();

	UFUNCTION()
	void OnPlaybackStateChanged(EJellyfinPlaybackState NewState);

	UFUNCTION()
	void OnPlaybackEnded();

	UFUNCTION()
	void OnAuthStateChanged(EJellyfinAuthState NewState);

	/** Switch to the home widget after successful login */
	void SwitchToHomeWidget();

private:
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* RootSceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ScreenMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* FrameMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* UIWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* ControlsWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UJellyfinMediaPlayerComponent* MediaPlayer;

	// Materials
	UPROPERTY()
	UMaterialInstanceDynamic* ScreenMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* FrameMaterial;

	// Viewport widget for desktop mode (keyboard input works reliably)
	UPROPERTY()
	UUserWidget* ViewportWidget;

	// Current home widget (when logged in)
	UPROPERTY()
	class UJellyfinSimpleHomeWidget* HomeWidget;

	// Current library widget (when browsing)
	UPROPERTY()
	class UJellyfinSimpleLibraryWidget* LibraryWidget;

	// State
	EJellyfinScreenMode ScreenMode = EJellyfinScreenMode::Settings;
	bool bControlsVisible = false;
	bool bIsDesktopMode = false;
	float TimeSinceLastInteraction = 0.0f;
};
