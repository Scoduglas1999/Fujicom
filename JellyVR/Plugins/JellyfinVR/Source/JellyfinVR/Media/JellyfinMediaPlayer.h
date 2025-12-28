// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "API/JellyfinTypes.h"
#include "NativeMediaPlayer.h"
#include "JellyfinMediaPlayer.generated.h"

class UMediaPlayer;
class UMediaSource;
class UStreamMediaSource;
class UMediaTexture;
class UMediaSoundComponent;
class UJellyfinClient;
class UTexture2DDynamic;
class USoundWaveProcedural;
class UAudioComponent;

UENUM(BlueprintType)
enum class EJellyfinPlaybackState : uint8
{
	Stopped,
	Opening,
	Buffering,
	Playing,
	Paused,
	Error
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlaybackStateChanged, EJellyfinPlaybackState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlaybackProgress, float, Progress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlaybackEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlaybackError, const FString&, ErrorMessage);

/**
 * Media player component for Jellyfin content
 * Handles video playback, progress tracking, and Jellyfin sync
 */
UCLASS(ClassGroup=(JellyfinVR), meta=(BlueprintSpawnableComponent))
class JELLYFINVR_API UJellyfinMediaPlayerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UJellyfinMediaPlayerComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * Open a media item for playback
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Playback")
	void OpenItem(const FJellyfinMediaItem& Item);

	/**
	 * Play the current media
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Playback")
	void Play();

	/**
	 * Pause playback
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Playback")
	void Pause();

	/**
	 * Toggle play/pause
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Playback")
	void TogglePlayPause();

	/**
	 * Stop playback
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Playback")
	void Stop();

	/**
	 * Seek to position (0.0 - 1.0)
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Playback")
	void SeekToProgress(float Progress);

	/**
	 * Seek to time in seconds
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Playback")
	void SeekToTime(float TimeSeconds);

	/**
	 * Seek forward/backward by seconds
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Playback")
	void SeekRelative(float DeltaSeconds);

	/**
	 * Skip to next chapter
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Playback")
	void NextChapter();

	/**
	 * Skip to previous chapter
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Playback")
	void PreviousChapter();

	/**
	 * Set audio track by index
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Playback")
	void SetAudioTrack(int32 TrackIndex);

	/**
	 * Set subtitle track by index (-1 to disable)
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Playback")
	void SetSubtitleTrack(int32 TrackIndex);

	/**
	 * Get current playback state
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Playback")
	EJellyfinPlaybackState GetPlaybackState() const { return PlaybackState; }

	/**
	 * Get current playback progress (0.0 - 1.0)
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Playback")
	float GetProgress() const;

	/**
	 * Get current playback time in seconds
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Playback")
	float GetCurrentTime() const;

	/**
	 * Get total duration in seconds
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Playback")
	float GetDuration() const;

	/**
	 * Get formatted time string (HH:MM:SS)
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Playback")
	FString GetCurrentTimeFormatted() const;

	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Playback")
	FString GetDurationFormatted() const;

	/**
	 * Get the media texture for rendering
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Playback")
	UMediaTexture* GetMediaTexture() const { return MediaTexture; }

	/**
	 * Get current media item
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Playback")
	const FJellyfinMediaItem& GetCurrentItem() const { return CurrentItem; }

	/**
	 * Get playback info
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Playback")
	const FJellyfinPlaybackInfo& GetPlaybackInfo() const { return CurrentPlaybackInfo; }

	/**
	 * Get available audio tracks
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Playback")
	TArray<FJellyfinMediaStream> GetAudioTracks() const;

	/**
	 * Get available subtitle tracks
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Playback")
	TArray<FJellyfinMediaStream> GetSubtitleTracks() const;

	/**
	 * Get currently selected audio track index
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Playback")
	int32 GetCurrentAudioTrackIndex() const;

	/**
	 * Get currently selected subtitle track index (-1 if disabled)
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Playback")
	int32 GetCurrentSubtitleTrackIndex() const { return CurrentSubtitleTrack; }

	/**
	 * Get current chapter index
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Playback")
	int32 GetCurrentChapterIndex() const;

	/**
	 * Check if currently playing
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Playback")
	bool IsPlaying() const { return PlaybackState == EJellyfinPlaybackState::Playing; }

	/**
	 * Check if current video is HDR
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Playback")
	bool IsHDRVideo() const;

	/**
	 * Get HDR video range (SDR, HDR10, DolbyVision, HLG)
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Playback")
	FString GetVideoHDRRange() const;

	/**
	 * Get video stream information (resolution, codec, HDR)
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Playback")
	FJellyfinMediaStream GetVideoStreamInfo() const;

	/**
	 * Enable or disable HDR processing (if supported by platform)
	 * Note: This affects tone mapping for non-HDR displays
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Playback")
	void SetHDREnabled(bool bEnabled);

	/**
	 * Check if HDR processing is enabled
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Playback")
	bool IsHDREnabled() const { return bHDREnabled; }

	// Events
	UPROPERTY(BlueprintAssignable, Category = "JellyfinVR|Events")
	FOnPlaybackStateChanged OnPlaybackStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "JellyfinVR|Events")
	FOnPlaybackProgress OnPlaybackProgress;

	UPROPERTY(BlueprintAssignable, Category = "JellyfinVR|Events")
	FOnPlaybackEnded OnPlaybackEnded;

	UPROPERTY(BlueprintAssignable, Category = "JellyfinVR|Events")
	FOnPlaybackError OnPlaybackError;

protected:
	UFUNCTION()
	void OnPlaybackInfoLoaded(bool bSuccess, const FJellyfinPlaybackInfo& PlaybackInfo);

	UFUNCTION()
	void OnMediaOpened(FString OpenedUrl);

	UFUNCTION()
	void OnMediaOpenFailed(FString FailedUrl);

	UFUNCTION()
	void OnEndReached();

	void SetPlaybackState(EJellyfinPlaybackState NewState);
	void ReportProgressToServer();
	FString FormatTime(float TimeSeconds) const;

	// Native player methods
	void InitializeNativePlayer();
	void ShutdownNativePlayer();
	void UpdateNativePlayerFrame();
	void UpdateNativePlayerAudio();
	void OnNativeMediaOpened(bool bSuccess);
	void OnNativeStateChanged(ENativePlaybackState NewState);
	void OnNativeError(const FString& ErrorMessage);
	void OnNativeEndReached();

public:
	/**
	 * Get the video texture (works for both native and built-in player)
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Playback")
	UTexture* GetVideoTexture() const;

	/**
	 * Check if using native player
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Playback")
	bool IsUsingNativePlayer() const { return bUseNativePlayer; }

	/**
	 * Set whether to use native player (MPV/ExoPlayer) for better codec support
	 * Must be called before opening media
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Playback")
	void SetUseNativePlayer(bool bUseNative);

private:
	// Built-in UE media player (fallback)
	UPROPERTY()
	UMediaPlayer* MediaPlayer;

	UPROPERTY()
	UStreamMediaSource* MediaSource;

	UPROPERTY()
	UMediaTexture* MediaTexture;

	UPROPERTY()
	UMediaSoundComponent* SoundComponent;

	// Native media player (MPV on Windows, ExoPlayer on Android)
	TSharedPtr<INativeMediaPlayer> NativePlayer;

	UPROPERTY()
	UTexture2DDynamic* NativeVideoTexture;

	UPROPERTY()
	USoundWaveProcedural* NativeAudioWave;

	UPROPERTY()
	UAudioComponent* NativeAudioComponent;

	// Whether to use native player (default true for better codec support)
	bool bUseNativePlayer = true;
	bool bNativePlayerInitialized = false;

	UPROPERTY()
	UJellyfinClient* JellyfinClient;

	UPROPERTY()
	FJellyfinMediaItem CurrentItem;

	UPROPERTY()
	FJellyfinPlaybackInfo CurrentPlaybackInfo;

	EJellyfinPlaybackState PlaybackState = EJellyfinPlaybackState::Stopped;

	// Progress reporting
	float LastReportedProgress = 0.0f;
	float ProgressReportInterval = 10.0f; // Report every 10 seconds
	float TimeSinceLastReport = 0.0f;

	// Track indices
	int32 CurrentAudioTrack = 0;
	int32 CurrentSubtitleTrack = -1;

	// HDR settings
	bool bHDREnabled = true; // Enable HDR by default if content supports it

	// Native player state
	int32 NativeVideoWidth = 0;
	int32 NativeVideoHeight = 0;
};
