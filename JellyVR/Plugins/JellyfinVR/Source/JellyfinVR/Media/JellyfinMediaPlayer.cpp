// Copyright JellyVR Project. All Rights Reserved.

#include "JellyfinMediaPlayer.h"
#include "JellyfinVRModule.h"
#include "API/JellyfinClient.h"
#include "API/JellyfinAuth.h"
#include "NativeMediaPlayer.h"
#include "MediaPlayer.h"
#include "StreamMediaSource.h"
#include "MediaTexture.h"
#include "MediaSoundComponent.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Texture2DDynamic.h"
#include "Sound/SoundWaveProcedural.h"
#include "Components/AudioComponent.h"

UJellyfinMediaPlayerComponent::UJellyfinMediaPlayerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f; // Tick at 10Hz for progress updates
}

void UJellyfinMediaPlayerComponent::BeginPlay()
{
	Super::BeginPlay();

	// Try native player first if enabled
	if (bUseNativePlayer)
	{
		InitializeNativePlayer();
	}

	// Create fallback media player (also used if native player fails)
	MediaPlayer = NewObject<UMediaPlayer>(this);
	MediaPlayer->OnMediaOpened.AddDynamic(this, &UJellyfinMediaPlayerComponent::OnMediaOpened);
	MediaPlayer->OnMediaOpenFailed.AddDynamic(this, &UJellyfinMediaPlayerComponent::OnMediaOpenFailed);
	MediaPlayer->OnEndReached.AddDynamic(this, &UJellyfinMediaPlayerComponent::OnEndReached);

	// Create stream media source
	MediaSource = NewObject<UStreamMediaSource>(this);

	// Create media texture (for fallback player)
	MediaTexture = NewObject<UMediaTexture>(this);
	MediaTexture->SetMediaPlayer(MediaPlayer);
	MediaTexture->UpdateResource();

	// Try to get the Jellyfin client from the auth subsystem
	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
	{
		if (UJellyfinAuthSubsystem* AuthSubsystem = GameInstance->GetSubsystem<UJellyfinAuthSubsystem>())
		{
			JellyfinClient = AuthSubsystem->GetClient();
			if (JellyfinClient)
			{
				JellyfinClient->OnPlaybackInfoLoaded.AddDynamic(this, &UJellyfinMediaPlayerComponent::OnPlaybackInfoLoaded);
			}
		}
	}

	UE_LOG(LogJellyfinVR, Log, TEXT("JellyfinMediaPlayerComponent initialized (Native: %s)"),
		bUseNativePlayer && bNativePlayerInitialized ? TEXT("YES") : TEXT("NO"));
}

void UJellyfinMediaPlayerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Report final position
	if (PlaybackState == EJellyfinPlaybackState::Playing || PlaybackState == EJellyfinPlaybackState::Paused)
	{
		Stop();
	}

	// Shutdown native player
	ShutdownNativePlayer();

	if (JellyfinClient)
	{
		JellyfinClient->OnPlaybackInfoLoaded.RemoveDynamic(this, &UJellyfinMediaPlayerComponent::OnPlaybackInfoLoaded);
	}

	Super::EndPlay(EndPlayReason);
}

void UJellyfinMediaPlayerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update native player frame if active
	if (bUseNativePlayer && bNativePlayerInitialized && NativePlayer.IsValid())
	{
		UpdateNativePlayerFrame();
		UpdateNativePlayerAudio();
	}

	if (PlaybackState == EJellyfinPlaybackState::Playing)
	{
		// Broadcast progress
		float CurrentProgress = GetProgress();
		OnPlaybackProgress.Broadcast(CurrentProgress);

		// Report to server periodically
		TimeSinceLastReport += DeltaTime;
		if (TimeSinceLastReport >= ProgressReportInterval)
		{
			ReportProgressToServer();
			TimeSinceLastReport = 0.0f;
		}
	}
}

void UJellyfinMediaPlayerComponent::OpenItem(const FJellyfinMediaItem& Item)
{
	UE_LOG(LogJellyfinVR, Log, TEXT("OpenItem called for: %s (ID: %s)"), *Item.Name, *Item.Id);

	CurrentItem = Item;
	SetPlaybackState(EJellyfinPlaybackState::Opening);

	if (!JellyfinClient)
	{
		UE_LOG(LogJellyfinVR, Error, TEXT("OpenItem: JellyfinClient is NULL!"));
		SetPlaybackState(EJellyfinPlaybackState::Error);
		OnPlaybackError.Broadcast(TEXT("Jellyfin client not initialized"));
		return;
	}

	if (!JellyfinClient->IsAuthenticated())
	{
		UE_LOG(LogJellyfinVR, Error, TEXT("OpenItem: JellyfinClient is not authenticated!"));
		SetPlaybackState(EJellyfinPlaybackState::Error);
		OnPlaybackError.Broadcast(TEXT("Not connected to Jellyfin server"));
		return;
	}

	UE_LOG(LogJellyfinVR, Log, TEXT("OpenItem: Requesting playback info for item: %s"), *Item.Id);
	JellyfinClient->GetPlaybackInfo(Item.Id);
}

void UJellyfinMediaPlayerComponent::OnPlaybackInfoLoaded(bool bSuccess, const FJellyfinPlaybackInfo& PlaybackInfo)
{
	UE_LOG(LogJellyfinVR, Log, TEXT("OnPlaybackInfoLoaded: bSuccess=%s, StreamUrl=%s"),
		bSuccess ? TEXT("true") : TEXT("false"),
		PlaybackInfo.StreamUrl.Len() > 50 ? *(PlaybackInfo.StreamUrl.Left(50) + TEXT("...")) : *PlaybackInfo.StreamUrl);

	if (!bSuccess)
	{
		UE_LOG(LogJellyfinVR, Error, TEXT("OnPlaybackInfoLoaded: Failed to get playback info"));
		SetPlaybackState(EJellyfinPlaybackState::Error);
		OnPlaybackError.Broadcast(TEXT("Failed to get playback info"));
		return;
	}

	CurrentPlaybackInfo = PlaybackInfo;

	// Set up the media source with the stream URL
	if (PlaybackInfo.StreamUrl.IsEmpty())
	{
		SetPlaybackState(EJellyfinPlaybackState::Error);
		OnPlaybackError.Broadcast(TEXT("No valid stream URL"));
		return;
	}

	UE_LOG(LogJellyfinVR, Log, TEXT("Opening stream: %s"), *PlaybackInfo.StreamUrl);

	// Use native player if available
	if (bUseNativePlayer && bNativePlayerInitialized && NativePlayer.IsValid())
	{
		UE_LOG(LogJellyfinVR, Log, TEXT("Using native player (MPV/ExoPlayer)"));

		// Build headers for authentication
		TMap<FString, FString> Headers;
		// Note: api_key is already in URL, but we could add other headers here if needed

		if (!NativePlayer->Open(PlaybackInfo.StreamUrl, Headers))
		{
			UE_LOG(LogJellyfinVR, Error, TEXT("Native player failed to open stream, falling back to built-in player"));
			bUseNativePlayer = false;
			// Fall through to built-in player
		}
		else
		{
			// Native player is handling playback
			return;
		}
	}

	// Fallback to built-in UE media player
	UE_LOG(LogJellyfinVR, Log, TEXT("Using built-in UE media player"));
	MediaSource->StreamUrl = PlaybackInfo.StreamUrl;

	// Open the media
	if (!MediaPlayer->OpenSource(MediaSource))
	{
		SetPlaybackState(EJellyfinPlaybackState::Error);
		OnPlaybackError.Broadcast(TEXT("Failed to open media source"));
	}
}

void UJellyfinMediaPlayerComponent::OnMediaOpened(FString OpenedUrl)
{
	UE_LOG(LogJellyfinVR, Log, TEXT("Media opened: %s"), *OpenedUrl);

	// Log HDR information
	if (IsHDRVideo())
	{
		FJellyfinMediaStream VideoInfo = GetVideoStreamInfo();
		UE_LOG(LogJellyfinVR, Log, TEXT("HDR Video Detected - Range: %s, Resolution: %dx%d, Codec: %s"),
			*VideoInfo.VideoRange, VideoInfo.Width, VideoInfo.Height, *VideoInfo.Codec);
	}
	else
	{
		FJellyfinMediaStream VideoInfo = GetVideoStreamInfo();
		UE_LOG(LogJellyfinVR, Log, TEXT("SDR Video - Resolution: %dx%d, Codec: %s"),
			VideoInfo.Width, VideoInfo.Height, *VideoInfo.Codec);
	}

	// Report playback start to server
	if (JellyfinClient && !CurrentItem.Id.IsEmpty())
	{
		JellyfinClient->ReportPlaybackStart(
			CurrentItem.Id,
			CurrentPlaybackInfo.MediaSourceId,
			CurrentPlaybackInfo.PlaySessionId
		);
	}

	// Seek to saved position if resuming
	if (CurrentItem.PlaybackPositionTicks > 0)
	{
		float ResumeTime = CurrentItem.GetPlaybackPositionSeconds();
		SeekToTime(ResumeTime);
		UE_LOG(LogJellyfinVR, Log, TEXT("Resuming from %s"), *FormatTime(ResumeTime));
	}

	// Start playback
	MediaPlayer->Play();
	SetPlaybackState(EJellyfinPlaybackState::Playing);
}

void UJellyfinMediaPlayerComponent::OnMediaOpenFailed(FString FailedUrl)
{
	UE_LOG(LogJellyfinVR, Error, TEXT("Failed to open media: %s"), *FailedUrl);
	SetPlaybackState(EJellyfinPlaybackState::Error);
	OnPlaybackError.Broadcast(FString::Printf(TEXT("Failed to open: %s"), *FailedUrl));
}

void UJellyfinMediaPlayerComponent::OnEndReached()
{
	UE_LOG(LogJellyfinVR, Log, TEXT("Playback ended"));

	// Report playback stopped
	if (JellyfinClient && !CurrentItem.Id.IsEmpty())
	{
		JellyfinClient->ReportPlaybackStopped(
			CurrentItem.Id,
			CurrentPlaybackInfo.MediaSourceId,
			CurrentPlaybackInfo.PlaySessionId,
			CurrentItem.RunTimeTicks // Report at end position
		);

		// Mark as played
		JellyfinClient->MarkPlayed(CurrentItem.Id);
	}

	SetPlaybackState(EJellyfinPlaybackState::Stopped);
	OnPlaybackEnded.Broadcast();
}

void UJellyfinMediaPlayerComponent::Play()
{
	if (PlaybackState == EJellyfinPlaybackState::Paused || PlaybackState == EJellyfinPlaybackState::Stopped)
	{
		if (bUseNativePlayer && NativePlayer.IsValid())
		{
			NativePlayer->Play();
		}
		else if (MediaPlayer)
		{
			MediaPlayer->Play();
		}
		SetPlaybackState(EJellyfinPlaybackState::Playing);
	}
}

void UJellyfinMediaPlayerComponent::Pause()
{
	if (PlaybackState == EJellyfinPlaybackState::Playing)
	{
		if (bUseNativePlayer && NativePlayer.IsValid())
		{
			NativePlayer->Pause();
		}
		else if (MediaPlayer)
		{
			MediaPlayer->Pause();
		}
		SetPlaybackState(EJellyfinPlaybackState::Paused);
		ReportProgressToServer();
	}
}

void UJellyfinMediaPlayerComponent::TogglePlayPause()
{
	if (PlaybackState == EJellyfinPlaybackState::Playing)
	{
		Pause();
	}
	else if (PlaybackState == EJellyfinPlaybackState::Paused)
	{
		Play();
	}
}

void UJellyfinMediaPlayerComponent::Stop()
{
	// Report final position
	if (JellyfinClient && !CurrentItem.Id.IsEmpty() && !CurrentPlaybackInfo.PlaySessionId.IsEmpty())
	{
		int64 CurrentTicks = (int64)(GetCurrentTime() * 10000000.0);
		JellyfinClient->ReportPlaybackStopped(
			CurrentItem.Id,
			CurrentPlaybackInfo.MediaSourceId,
			CurrentPlaybackInfo.PlaySessionId,
			CurrentTicks
		);
	}

	if (bUseNativePlayer && NativePlayer.IsValid())
	{
		NativePlayer->Stop();
	}
	else if (MediaPlayer)
	{
		MediaPlayer->Close();
	}

	SetPlaybackState(EJellyfinPlaybackState::Stopped);
}

void UJellyfinMediaPlayerComponent::SeekToProgress(float Progress)
{
	Progress = FMath::Clamp(Progress, 0.0f, 1.0f);
	float TargetTime = Progress * GetDuration();
	SeekToTime(TargetTime);
}

void UJellyfinMediaPlayerComponent::SeekToTime(float TimeSeconds)
{
	if (bUseNativePlayer && NativePlayer.IsValid())
	{
		int64 PositionUs = (int64)(TimeSeconds * 1000000.0);
		NativePlayer->Seek(PositionUs);
	}
	else if (MediaPlayer)
	{
		FTimespan TargetTime = FTimespan::FromSeconds(TimeSeconds);
		MediaPlayer->Seek(TargetTime);
	}
}

void UJellyfinMediaPlayerComponent::SeekRelative(float DeltaSeconds)
{
	float CurrentTime = GetCurrentTime();
	float NewTime = FMath::Clamp(CurrentTime + DeltaSeconds, 0.0f, GetDuration());
	SeekToTime(NewTime);
}

void UJellyfinMediaPlayerComponent::NextChapter()
{
	if (CurrentItem.Chapters.Num() == 0)
	{
		// No chapters, skip forward 30 seconds
		SeekRelative(30.0f);
		return;
	}

	float CurrentTime = GetCurrentTime();
	for (const FJellyfinChapter& Chapter : CurrentItem.Chapters)
	{
		double ChapterTime = Chapter.GetStartPositionSeconds();
		if (ChapterTime > CurrentTime + 1.0) // Add small buffer
		{
			SeekToTime(ChapterTime);
			return;
		}
	}
}

void UJellyfinMediaPlayerComponent::PreviousChapter()
{
	if (CurrentItem.Chapters.Num() == 0)
	{
		// No chapters, skip backward 30 seconds
		SeekRelative(-30.0f);
		return;
	}

	float CurrentTime = GetCurrentTime();
	for (int32 i = CurrentItem.Chapters.Num() - 1; i >= 0; --i)
	{
		double ChapterTime = CurrentItem.Chapters[i].GetStartPositionSeconds();
		if (ChapterTime < CurrentTime - 3.0) // Allow going back to current chapter start
		{
			SeekToTime(ChapterTime);
			return;
		}
	}

	// Go to beginning
	SeekToTime(0.0f);
}

void UJellyfinMediaPlayerComponent::SetAudioTrack(int32 TrackIndex)
{
	if (MediaPlayer)
	{
		MediaPlayer->SelectTrack(EMediaPlayerTrack::Audio, TrackIndex);
		CurrentAudioTrack = TrackIndex;
		UE_LOG(LogJellyfinVR, Log, TEXT("Audio track set to index %d"), TrackIndex);
	}
}

void UJellyfinMediaPlayerComponent::SetSubtitleTrack(int32 TrackIndex)
{
	CurrentSubtitleTrack = TrackIndex;

	if (MediaPlayer)
	{
		if (TrackIndex < 0)
		{
			// Disable subtitles by selecting INDEX_NONE
			MediaPlayer->SelectTrack(EMediaPlayerTrack::Caption, INDEX_NONE);
			UE_LOG(LogJellyfinVR, Log, TEXT("Subtitles disabled"));
		}
		else
		{
			MediaPlayer->SelectTrack(EMediaPlayerTrack::Caption, TrackIndex);
			UE_LOG(LogJellyfinVR, Log, TEXT("Subtitle track set to index %d"), TrackIndex);
		}
	}
}

float UJellyfinMediaPlayerComponent::GetProgress() const
{
	float Duration = GetDuration();
	if (Duration > 0.0f)
	{
		return GetCurrentTime() / Duration;
	}
	return 0.0f;
}

float UJellyfinMediaPlayerComponent::GetCurrentTime() const
{
	if (bUseNativePlayer && NativePlayer.IsValid())
	{
		return NativePlayer->GetPosition() / 1000000.0; // Microseconds to seconds
	}
	else if (MediaPlayer)
	{
		return MediaPlayer->GetTime().GetTotalSeconds();
	}
	return 0.0f;
}

float UJellyfinMediaPlayerComponent::GetDuration() const
{
	if (bUseNativePlayer && NativePlayer.IsValid())
	{
		int64 DurationUs = NativePlayer->GetDuration();
		if (DurationUs > 0)
		{
			return DurationUs / 1000000.0; // Microseconds to seconds
		}
	}
	else if (MediaPlayer)
	{
		return MediaPlayer->GetDuration().GetTotalSeconds();
	}
	return CurrentItem.GetRunTimeSeconds();
}

FString UJellyfinMediaPlayerComponent::GetCurrentTimeFormatted() const
{
	return FormatTime(GetCurrentTime());
}

FString UJellyfinMediaPlayerComponent::GetDurationFormatted() const
{
	return FormatTime(GetDuration());
}

TArray<FJellyfinMediaStream> UJellyfinMediaPlayerComponent::GetAudioTracks() const
{
	TArray<FJellyfinMediaStream> AudioTracks;
	for (const FJellyfinMediaStream& Stream : CurrentPlaybackInfo.MediaStreams)
	{
		if (Stream.Type == TEXT("Audio"))
		{
			AudioTracks.Add(Stream);
		}
	}
	return AudioTracks;
}

TArray<FJellyfinMediaStream> UJellyfinMediaPlayerComponent::GetSubtitleTracks() const
{
	TArray<FJellyfinMediaStream> SubtitleTracks;
	for (const FJellyfinMediaStream& Stream : CurrentPlaybackInfo.MediaStreams)
	{
		if (Stream.Type == TEXT("Subtitle"))
		{
			SubtitleTracks.Add(Stream);
		}
	}
	return SubtitleTracks;
}

int32 UJellyfinMediaPlayerComponent::GetCurrentAudioTrackIndex() const
{
	// Try to get the selected track from the media player
	if (MediaPlayer)
	{
		int32 SelectedTrack = MediaPlayer->GetSelectedTrack(EMediaPlayerTrack::Audio);
		if (SelectedTrack != INDEX_NONE)
		{
			return SelectedTrack;
		}
	}
	// Fall back to our cached value
	return CurrentAudioTrack;
}

int32 UJellyfinMediaPlayerComponent::GetCurrentChapterIndex() const
{
	if (CurrentItem.Chapters.Num() == 0)
	{
		return -1;
	}

	float CurrentTime = GetCurrentTime();
	for (int32 i = CurrentItem.Chapters.Num() - 1; i >= 0; --i)
	{
		if (CurrentItem.Chapters[i].GetStartPositionSeconds() <= CurrentTime)
		{
			return i;
		}
	}
	return 0;
}

void UJellyfinMediaPlayerComponent::SetPlaybackState(EJellyfinPlaybackState NewState)
{
	if (PlaybackState != NewState)
	{
		PlaybackState = NewState;
		OnPlaybackStateChanged.Broadcast(NewState);
	}
}

void UJellyfinMediaPlayerComponent::ReportProgressToServer()
{
	if (JellyfinClient && !CurrentItem.Id.IsEmpty() && !CurrentPlaybackInfo.PlaySessionId.IsEmpty())
	{
		int64 CurrentTicks = (int64)(GetCurrentTime() * 10000000.0);
		bool bIsPaused = PlaybackState == EJellyfinPlaybackState::Paused;

		JellyfinClient->ReportPlaybackProgress(
			CurrentItem.Id,
			CurrentPlaybackInfo.MediaSourceId,
			CurrentPlaybackInfo.PlaySessionId,
			CurrentTicks,
			bIsPaused
		);
	}
}

FString UJellyfinMediaPlayerComponent::FormatTime(float TimeSeconds) const
{
	int32 Hours = FMath::FloorToInt(TimeSeconds / 3600.0f);
	int32 Minutes = FMath::FloorToInt(FMath::Fmod(TimeSeconds, 3600.0f) / 60.0f);
	int32 Seconds = FMath::FloorToInt(FMath::Fmod(TimeSeconds, 60.0f));

	if (Hours > 0)
	{
		return FString::Printf(TEXT("%d:%02d:%02d"), Hours, Minutes, Seconds);
	}
	return FString::Printf(TEXT("%d:%02d"), Minutes, Seconds);
}

bool UJellyfinMediaPlayerComponent::IsHDRVideo() const
{
	// Check the video stream metadata from playback info
	for (const FJellyfinMediaStream& Stream : CurrentPlaybackInfo.MediaStreams)
	{
		if (Stream.Type == TEXT("Video") && Stream.bIsHDR)
		{
			return true;
		}
	}

	// Fallback: check media item streams
	for (const FJellyfinMediaStream& Stream : CurrentItem.MediaStreams)
	{
		if (Stream.Type == TEXT("Video") && Stream.bIsHDR)
		{
			return true;
		}
	}

	return false;
}

FString UJellyfinMediaPlayerComponent::GetVideoHDRRange() const
{
	// Check playback info first
	for (const FJellyfinMediaStream& Stream : CurrentPlaybackInfo.MediaStreams)
	{
		if (Stream.Type == TEXT("Video") && !Stream.VideoRange.IsEmpty())
		{
			return Stream.VideoRange;
		}
	}

	// Fallback: check media item streams
	for (const FJellyfinMediaStream& Stream : CurrentItem.MediaStreams)
	{
		if (Stream.Type == TEXT("Video") && !Stream.VideoRange.IsEmpty())
		{
			return Stream.VideoRange;
		}
	}

	return TEXT("SDR");
}

FJellyfinMediaStream UJellyfinMediaPlayerComponent::GetVideoStreamInfo() const
{
	// Return the first video stream from playback info
	for (const FJellyfinMediaStream& Stream : CurrentPlaybackInfo.MediaStreams)
	{
		if (Stream.Type == TEXT("Video"))
		{
			return Stream;
		}
	}

	// Fallback: return first video stream from media item
	for (const FJellyfinMediaStream& Stream : CurrentItem.MediaStreams)
	{
		if (Stream.Type == TEXT("Video"))
		{
			return Stream;
		}
	}

	// Return empty stream if none found
	return FJellyfinMediaStream();
}

void UJellyfinMediaPlayerComponent::SetHDREnabled(bool bEnabled)
{
	if (bHDREnabled != bEnabled)
	{
		bHDREnabled = bEnabled;

		// Log HDR state change
		FString HDRRange = GetVideoHDRRange();
		if (IsHDRVideo())
		{
			UE_LOG(LogJellyfinVR, Log, TEXT("HDR processing %s for %s content"),
				bEnabled ? TEXT("enabled") : TEXT("disabled"), *HDRRange);
		}

		// Note: UMediaPlayer doesn't expose direct HDR control in UE5
		// HDR is handled automatically by the platform's video decoder
		// This flag can be used by the rendering pipeline for tone mapping
		// if displaying HDR content on an SDR display
	}
}

// ============================================================================
// Native Media Player Integration
// ============================================================================

void UJellyfinMediaPlayerComponent::SetUseNativePlayer(bool bUseNative)
{
	if (PlaybackState != EJellyfinPlaybackState::Stopped)
	{
		UE_LOG(LogJellyfinVR, Warning, TEXT("Cannot change player mode during playback"));
		return;
	}

	bUseNativePlayer = bUseNative;
	UE_LOG(LogJellyfinVR, Log, TEXT("Native player mode: %s"), bUseNative ? TEXT("enabled") : TEXT("disabled"));
}

UTexture* UJellyfinMediaPlayerComponent::GetVideoTexture() const
{
	if (bUseNativePlayer && NativeVideoTexture)
	{
		return NativeVideoTexture;
	}
	return MediaTexture;
}

void UJellyfinMediaPlayerComponent::InitializeNativePlayer()
{
	if (bNativePlayerInitialized)
	{
		return;
	}

	if (!INativeMediaPlayer::IsPlatformSupported())
	{
		UE_LOG(LogJellyfinVR, Warning, TEXT("Native player not supported on this platform, falling back to built-in player"));
		bUseNativePlayer = false;
		return;
	}

	NativePlayer = INativeMediaPlayer::Create();
	if (!NativePlayer.IsValid())
	{
		UE_LOG(LogJellyfinVR, Error, TEXT("Failed to create native player"));
		bUseNativePlayer = false;
		return;
	}

	if (!NativePlayer->Initialize())
	{
		UE_LOG(LogJellyfinVR, Error, TEXT("Failed to initialize native player"));
		NativePlayer.Reset();
		bUseNativePlayer = false;
		return;
	}

	// Set up callbacks
	NativePlayer->OnMediaOpened.BindRaw(this, &UJellyfinMediaPlayerComponent::OnNativeMediaOpened);
	NativePlayer->OnStateChanged.BindRaw(this, &UJellyfinMediaPlayerComponent::OnNativeStateChanged);
	NativePlayer->OnError.BindRaw(this, &UJellyfinMediaPlayerComponent::OnNativeError);
	NativePlayer->OnEndReached.BindRaw(this, &UJellyfinMediaPlayerComponent::OnNativeEndReached);

	bNativePlayerInitialized = true;
	UE_LOG(LogJellyfinVR, Log, TEXT("Native player initialized: %s"), *INativeMediaPlayer::GetPlatformPlayerName());
}

void UJellyfinMediaPlayerComponent::ShutdownNativePlayer()
{
	if (!bNativePlayerInitialized)
	{
		return;
	}

	if (NativePlayer.IsValid())
	{
		NativePlayer->Shutdown();
		NativePlayer.Reset();
	}

	NativeVideoTexture = nullptr;
	NativeAudioWave = nullptr;
	NativeAudioComponent = nullptr;
	bNativePlayerInitialized = false;

	UE_LOG(LogJellyfinVR, Log, TEXT("Native player shutdown"));
}

void UJellyfinMediaPlayerComponent::UpdateNativePlayerFrame()
{
	if (!NativePlayer.IsValid() || !NativePlayer->HasNewVideoFrame())
	{
		return;
	}

	FNativeVideoFrame Frame;
	if (!NativePlayer->GetVideoFrame(Frame) || !Frame.bIsValid)
	{
		return;
	}

	// Create or resize texture if needed
	if (!NativeVideoTexture || NativeVideoWidth != Frame.Width || NativeVideoHeight != Frame.Height)
	{
		NativeVideoWidth = Frame.Width;
		NativeVideoHeight = Frame.Height;

		// UE5.7: Use FTexture2DDynamicCreateInfo
		FTexture2DDynamicCreateInfo CreateInfo(PF_B8G8R8A8);
		CreateInfo.bSRGB = true;
		NativeVideoTexture = UTexture2DDynamic::Create(Frame.Width, Frame.Height, CreateInfo);
		NativeVideoTexture->UpdateResource();

		UE_LOG(LogJellyfinVR, Log, TEXT("Created native video texture: %dx%d"), Frame.Width, Frame.Height);
	}

	// Update texture with frame data using UpdateTextureRegions2D
	if (NativeVideoTexture && Frame.Pixels.Num() == Frame.Width * Frame.Height * 4)
	{
		// Prepare update region
		FUpdateTextureRegion2D Region;
		Region.SrcX = 0;
		Region.SrcY = 0;
		Region.DestX = 0;
		Region.DestY = 0;
		Region.Width = Frame.Width;
		Region.Height = Frame.Height;

		// Copy pixel data for async update - we need to manage the memory ourselves
		uint8* PixelDataCopy = new uint8[Frame.Pixels.Num()];
		FMemory::Memcpy(PixelDataCopy, Frame.Pixels.GetData(), Frame.Pixels.Num());

		NativeVideoTexture->UpdateTextureRegions(
			0,  // MipIndex
			1,  // NumRegions
			&Region,
			Frame.Width * 4,  // SrcPitch (BGRA = 4 bytes per pixel)
			4,  // SrcBpp
			PixelDataCopy,
			[](uint8* SrcData, const FUpdateTextureRegion2D* Regions)
			{
				// Cleanup the pixel data we allocated
				delete[] SrcData;
			}
		);
	}
}

void UJellyfinMediaPlayerComponent::UpdateNativePlayerAudio()
{
	// Audio integration is more complex - for now, let native player output audio directly
	// Full spatial audio integration requires:
	// 1. USoundWaveProcedural for procedural audio
	// 2. OnSoundWaveProceduralUnderflow callback to pull audio samples
	// 3. UAudioComponent attached to the screen actor for spatial positioning

	// TODO: Implement full spatial audio pipeline
	// For now, MPV outputs audio directly through its own audio output
}

void UJellyfinMediaPlayerComponent::OnNativeMediaOpened(bool bSuccess)
{
	if (!bSuccess)
	{
		UE_LOG(LogJellyfinVR, Error, TEXT("Native player failed to open media"));
		SetPlaybackState(EJellyfinPlaybackState::Error);
		OnPlaybackError.Broadcast(TEXT("Failed to open media"));
		return;
	}

	UE_LOG(LogJellyfinVR, Log, TEXT("Native player opened media successfully"));

	// Report playback start to server
	if (JellyfinClient && !CurrentItem.Id.IsEmpty())
	{
		JellyfinClient->ReportPlaybackStart(
			CurrentItem.Id,
			CurrentPlaybackInfo.MediaSourceId,
			CurrentPlaybackInfo.PlaySessionId
		);
	}

	// Seek to saved position if resuming
	if (CurrentItem.PlaybackPositionTicks > 0 && NativePlayer.IsValid())
	{
		int64 ResumePositionUs = CurrentItem.PlaybackPositionTicks / 10; // Ticks to microseconds
		NativePlayer->Seek(ResumePositionUs);
		UE_LOG(LogJellyfinVR, Log, TEXT("Resuming from %s"), *FormatTime(CurrentItem.GetPlaybackPositionSeconds()));
	}

	SetPlaybackState(EJellyfinPlaybackState::Playing);
}

void UJellyfinMediaPlayerComponent::OnNativeStateChanged(ENativePlaybackState NewState)
{
	switch (NewState)
	{
	case ENativePlaybackState::Playing:
		SetPlaybackState(EJellyfinPlaybackState::Playing);
		break;
	case ENativePlaybackState::Paused:
		SetPlaybackState(EJellyfinPlaybackState::Paused);
		break;
	case ENativePlaybackState::Buffering:
		SetPlaybackState(EJellyfinPlaybackState::Buffering);
		break;
	case ENativePlaybackState::Stopped:
	case ENativePlaybackState::Idle:
		SetPlaybackState(EJellyfinPlaybackState::Stopped);
		break;
	case ENativePlaybackState::Error:
		SetPlaybackState(EJellyfinPlaybackState::Error);
		break;
	default:
		break;
	}
}

void UJellyfinMediaPlayerComponent::OnNativeError(const FString& ErrorMessage)
{
	UE_LOG(LogJellyfinVR, Error, TEXT("Native player error: %s"), *ErrorMessage);
	SetPlaybackState(EJellyfinPlaybackState::Error);
	OnPlaybackError.Broadcast(ErrorMessage);
}

void UJellyfinMediaPlayerComponent::OnNativeEndReached()
{
	UE_LOG(LogJellyfinVR, Log, TEXT("Native player: playback ended"));

	// Report playback stopped
	if (JellyfinClient && !CurrentItem.Id.IsEmpty())
	{
		JellyfinClient->ReportPlaybackStopped(
			CurrentItem.Id,
			CurrentPlaybackInfo.MediaSourceId,
			CurrentPlaybackInfo.PlaySessionId,
			CurrentItem.RunTimeTicks
		);
		JellyfinClient->MarkPlayed(CurrentItem.Id);
	}

	SetPlaybackState(EJellyfinPlaybackState::Stopped);
	OnPlaybackEnded.Broadcast();
}
