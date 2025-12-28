// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeMediaPlayer.h"

#if PLATFORM_ANDROID

/**
 * ExoPlayer-based media player for Android/Quest
 * Uses ExoPlayer via JNI for hardware-accelerated decoding
 *
 * TODO: Full implementation pending - this is a stub
 */
class FExoMediaPlayer : public INativeMediaPlayer
{
public:
	FExoMediaPlayer();
	virtual ~FExoMediaPlayer();

	// INativeMediaPlayer interface
	virtual bool Initialize() override;
	virtual void Shutdown() override;
	virtual bool IsInitialized() const override { return bInitialized; }

	virtual bool Open(const FString& Url, const TMap<FString, FString>& Headers = TMap<FString, FString>()) override;
	virtual void Close() override;
	virtual bool Play() override;
	virtual bool Pause() override;
	virtual bool Stop() override;
	virtual bool Seek(int64 PositionUs) override;

	virtual ENativePlaybackState GetState() const override { return State; }
	virtual int64 GetPosition() const override;
	virtual int64 GetDuration() const override;
	virtual float GetVolume() const override;
	virtual void SetVolume(float Volume) override;
	virtual const FNativeMediaInfo& GetMediaInfo() const override { return MediaInfo; }

	virtual bool HasNewVideoFrame() const override;
	virtual bool GetVideoFrame(FNativeVideoFrame& OutFrame) override;
	virtual bool HasAudioSamples() const override;
	virtual bool GetAudioSamples(FNativeAudioSamples& OutSamples, int32 NumSamplesRequested) override;
	virtual int64 GetAudioClock() const override;

private:
	bool bInitialized = false;
	ENativePlaybackState State = ENativePlaybackState::Idle;
	FNativeMediaInfo MediaInfo;

	TSharedPtr<FAudioRingBuffer> AudioBuffer;
	TSharedPtr<FVideoFrameBuffer> VideoBuffer;

	TAtomic<int64> CurrentPositionUs;
	TAtomic<int64> DurationUs;
	TAtomic<float> CurrentVolume;
	TAtomic<int64> AudioClockUs;

	// JNI handles (to be implemented)
	// jobject ExoPlayerInstance = nullptr;
};

#endif // PLATFORM_ANDROID
