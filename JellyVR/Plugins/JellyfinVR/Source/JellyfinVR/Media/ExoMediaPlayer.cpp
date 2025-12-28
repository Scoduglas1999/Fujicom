// Copyright JellyVR Project. All Rights Reserved.

#include "ExoMediaPlayer.h"
#include "JellyfinVRModule.h"

#if PLATFORM_ANDROID

FExoMediaPlayer::FExoMediaPlayer()
	: CurrentPositionUs(0)
	, DurationUs(0)
	, CurrentVolume(1.0f)
	, AudioClockUs(0)
{
	AudioBuffer = MakeShared<FAudioRingBuffer>();
	VideoBuffer = MakeShared<FVideoFrameBuffer>();
}

FExoMediaPlayer::~FExoMediaPlayer()
{
	Shutdown();
}

bool FExoMediaPlayer::Initialize()
{
	if (bInitialized)
	{
		return true;
	}

	// TODO: Initialize JNI and create ExoPlayer instance
	// - Get JNI environment
	// - Find ExoPlayer class
	// - Create SimpleExoPlayer instance
	// - Set up video surface for frame extraction
	// - Set up audio sink for PCM extraction

	UE_LOG(LogJellyfinVR, Warning, TEXT("ExoMediaPlayer: Android implementation not yet complete"));

	// For now, mark as initialized to allow testing the interface
	bInitialized = true;
	return true;
}

void FExoMediaPlayer::Shutdown()
{
	if (!bInitialized)
	{
		return;
	}

	// TODO: Release JNI resources
	// - Release ExoPlayer instance
	// - Clean up surfaces

	bInitialized = false;
}

bool FExoMediaPlayer::Open(const FString& Url, const TMap<FString, FString>& Headers)
{
	if (!bInitialized)
	{
		return false;
	}

	// TODO: Implement via JNI
	// - Create MediaItem from URL
	// - Apply headers
	// - Prepare ExoPlayer

	UE_LOG(LogJellyfinVR, Warning, TEXT("ExoMediaPlayer::Open not yet implemented"));
	State = ENativePlaybackState::Error;
	OnError.ExecuteIfBound(TEXT("ExoPlayer not yet implemented"));
	return false;
}

void FExoMediaPlayer::Close()
{
	// TODO: Stop and release media
	State = ENativePlaybackState::Idle;
}

bool FExoMediaPlayer::Play()
{
	// TODO: Call ExoPlayer.play()
	return false;
}

bool FExoMediaPlayer::Pause()
{
	// TODO: Call ExoPlayer.pause()
	return false;
}

bool FExoMediaPlayer::Stop()
{
	Close();
	return true;
}

bool FExoMediaPlayer::Seek(int64 PositionUs)
{
	// TODO: Call ExoPlayer.seekTo()
	return false;
}

int64 FExoMediaPlayer::GetPosition() const
{
	return CurrentPositionUs.Load();
}

int64 FExoMediaPlayer::GetDuration() const
{
	return DurationUs.Load();
}

float FExoMediaPlayer::GetVolume() const
{
	return CurrentVolume.Load();
}

void FExoMediaPlayer::SetVolume(float Volume)
{
	CurrentVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
	// TODO: Call ExoPlayer.setVolume()
}

bool FExoMediaPlayer::HasNewVideoFrame() const
{
	return VideoBuffer.IsValid() && VideoBuffer->HasNewFrame();
}

bool FExoMediaPlayer::GetVideoFrame(FNativeVideoFrame& OutFrame)
{
	if (!VideoBuffer.IsValid())
	{
		return false;
	}
	return VideoBuffer->ReadFrame(OutFrame);
}

bool FExoMediaPlayer::HasAudioSamples() const
{
	return AudioBuffer.IsValid() && AudioBuffer->Available() > 0;
}

bool FExoMediaPlayer::GetAudioSamples(FNativeAudioSamples& OutSamples, int32 NumSamplesRequested)
{
	if (!AudioBuffer.IsValid())
	{
		return false;
	}

	OutSamples.Samples.SetNumUninitialized(NumSamplesRequested);
	int32 Read = AudioBuffer->Read(OutSamples.Samples.GetData(), NumSamplesRequested);

	if (Read < NumSamplesRequested)
	{
		FMemory::Memzero(OutSamples.Samples.GetData() + Read, (NumSamplesRequested - Read) * sizeof(float));
	}

	OutSamples.NumChannels = MediaInfo.AudioChannels > 0 ? MediaInfo.AudioChannels : 2;
	OutSamples.SampleRate = MediaInfo.AudioSampleRate > 0 ? MediaInfo.AudioSampleRate : 48000;
	OutSamples.TimestampUs = AudioClockUs.Load();

	return Read > 0;
}

int64 FExoMediaPlayer::GetAudioClock() const
{
	return AudioClockUs.Load();
}

#endif // PLATFORM_ANDROID
