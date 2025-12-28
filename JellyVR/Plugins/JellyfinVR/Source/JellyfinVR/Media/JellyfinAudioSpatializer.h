// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "JellyfinAudioSpatializer.generated.h"

class UMediaPlayer;
class UMediaSoundComponent;
class UAudioComponent;

UENUM(BlueprintType)
enum class EJellyfinAudioMode : uint8
{
	/** Stereo audio centered on screen */
	Stereo,
	/** Full 3D spatialized audio from screen location */
	Spatial3D,
	/** Binaural/HRTF for headphone listening */
	Binaural
};

/**
 * Audio spatializer for Jellyfin VR playback
 * Positions audio in 3D space relative to the screen for immersive VR audio
 */
UCLASS(ClassGroup=(JellyfinVR), meta=(BlueprintSpawnableComponent))
class JELLYFINVR_API UJellyfinAudioSpatializer : public UActorComponent
{
	GENERATED_BODY()

public:
	UJellyfinAudioSpatializer();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * Initialize with a media player
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Audio")
	void Initialize(UMediaPlayer* InMediaPlayer);

	/**
	 * Set audio spatialization mode
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Audio")
	void SetAudioMode(EJellyfinAudioMode Mode);

	/**
	 * Get current audio mode
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Audio")
	EJellyfinAudioMode GetAudioMode() const { return AudioMode; }

	/**
	 * Set master volume (0.0 - 1.0)
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Audio")
	void SetVolume(float Volume);

	/**
	 * Get current volume
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Audio")
	float GetVolume() const { return CurrentVolume; }

	/**
	 * Mute/unmute audio
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Audio")
	void SetMuted(bool bMuted);

	/**
	 * Check if muted
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Audio")
	bool IsMuted() const { return bIsMuted; }

	/**
	 * Toggle mute state
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Audio")
	void ToggleMute();

	/**
	 * Set the screen transform for spatial positioning
	 * Audio will appear to come from this location
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Audio")
	void SetScreenTransform(const FTransform& ScreenTransform);

	/**
	 * Set screen dimensions for proper audio spread
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Audio")
	void SetScreenDimensions(float Width, float Height);

	/**
	 * Set attenuation settings for 3D audio falloff
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Audio")
	void SetAttenuationRadius(float InnerRadius, float OuterRadius);

	/**
	 * Get the media sound component
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Audio")
	UMediaSoundComponent* GetSoundComponent() const { return MediaSoundComponent; }

protected:
	void UpdateSpatialPosition();
	void ApplyAudioMode();

	// Configuration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Audio")
	EJellyfinAudioMode AudioMode = EJellyfinAudioMode::Spatial3D;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Audio", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float CurrentVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Audio")
	bool bIsMuted = false;

	/** Inner radius where audio is at full volume */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Audio", meta = (ClampMin = "0.0"))
	float AttenuationInnerRadius = 200.0f;

	/** Outer radius where audio falls to zero */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Audio", meta = (ClampMin = "0.0"))
	float AttenuationOuterRadius = 2000.0f;

	/** Enable distance-based attenuation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Audio")
	bool bEnableAttenuation = true;

	/** Enable occlusion (audio blocked by objects) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Audio")
	bool bEnableOcclusion = false;

private:
	UPROPERTY()
	UMediaPlayer* MediaPlayer;

	UPROPERTY()
	UMediaSoundComponent* MediaSoundComponent;

	FTransform ScreenWorldTransform;
	float ScreenWidth = 400.0f;
	float ScreenHeight = 225.0f;

	float VolumeBeforeMute = 1.0f;
};
