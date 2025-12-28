// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"

/**
 * Audio sample format for decoded audio
 */
struct FNativeAudioSamples
{
	TArray<float> Samples;      // Interleaved PCM samples
	int32 NumChannels = 2;
	int32 SampleRate = 48000;
	int64 TimestampUs = 0;      // Presentation timestamp in microseconds
};

/**
 * Video frame format for decoded video
 */
struct FNativeVideoFrame
{
	TArray<uint8> Pixels;       // BGRA8 pixel data
	int32 Width = 0;
	int32 Height = 0;
	int64 TimestampUs = 0;      // Presentation timestamp in microseconds
	bool bIsValid = false;
};

/**
 * Playback state enum
 */
enum class ENativePlaybackState : uint8
{
	Idle,
	Opening,
	Buffering,
	Playing,
	Paused,
	Stopped,
	EndOfMedia,
	Error
};

/**
 * Media information after opening
 */
struct FNativeMediaInfo
{
	int32 VideoWidth = 0;
	int32 VideoHeight = 0;
	float VideoFrameRate = 0.0f;
	int32 AudioChannels = 0;
	int32 AudioSampleRate = 0;
	int64 DurationUs = 0;       // Duration in microseconds
	FString VideoCodec;
	FString AudioCodec;
	FString Container;
};

/**
 * Delegate types for async callbacks
 */
DECLARE_DELEGATE_OneParam(FOnNativeMediaOpened, bool /* bSuccess */);
DECLARE_DELEGATE_OneParam(FOnNativeMediaStateChanged, ENativePlaybackState);
DECLARE_DELEGATE_OneParam(FOnNativeMediaError, const FString& /* ErrorMessage */);
DECLARE_DELEGATE(FOnNativeMediaEndReached);

/**
 * Abstract interface for native media players
 * Implemented by platform-specific backends (MPV for Windows, ExoPlayer for Android)
 */
class INativeMediaPlayer
{
public:
	virtual ~INativeMediaPlayer() = default;

	// Lifecycle
	virtual bool Initialize() = 0;
	virtual void Shutdown() = 0;
	virtual bool IsInitialized() const = 0;

	// Media control
	virtual bool Open(const FString& Url, const TMap<FString, FString>& Headers = TMap<FString, FString>()) = 0;
	virtual void Close() = 0;
	virtual bool Play() = 0;
	virtual bool Pause() = 0;
	virtual bool Stop() = 0;
	virtual bool Seek(int64 PositionUs) = 0;

	// State queries
	virtual ENativePlaybackState GetState() const = 0;
	virtual int64 GetPosition() const = 0;          // Current position in microseconds
	virtual int64 GetDuration() const = 0;          // Duration in microseconds
	virtual float GetVolume() const = 0;
	virtual void SetVolume(float Volume) = 0;       // 0.0 to 1.0
	virtual const FNativeMediaInfo& GetMediaInfo() const = 0;

	// Frame access - called from game thread
	virtual bool HasNewVideoFrame() const = 0;
	virtual bool GetVideoFrame(FNativeVideoFrame& OutFrame) = 0;

	// Audio access - called from audio thread
	virtual bool HasAudioSamples() const = 0;
	virtual bool GetAudioSamples(FNativeAudioSamples& OutSamples, int32 NumSamplesRequested) = 0;

	// Audio/Video sync
	virtual int64 GetAudioClock() const = 0;        // Current audio playback time in microseconds

	// Event callbacks
	FOnNativeMediaOpened OnMediaOpened;
	FOnNativeMediaStateChanged OnStateChanged;
	FOnNativeMediaError OnError;
	FOnNativeMediaEndReached OnEndReached;

	// Factory method - creates platform-appropriate player
	static TSharedPtr<INativeMediaPlayer> Create();

	// Platform support check
	static bool IsPlatformSupported();
	static FString GetPlatformPlayerName();
};

/**
 * Thread-safe ring buffer for audio samples
 */
class FAudioRingBuffer
{
public:
	FAudioRingBuffer(int32 InCapacitySamples = 48000 * 2 * 2); // 2 seconds stereo @ 48kHz

	void Reset();
	int32 Write(const float* Data, int32 NumSamples);
	int32 Read(float* OutData, int32 NumSamples);
	int32 Available() const;
	int32 FreeSpace() const;

private:
	TArray<float> Buffer;
	TAtomic<int32> ReadPos;
	TAtomic<int32> WritePos;
	int32 Capacity;
	FCriticalSection BufferLock;
};

/**
 * Thread-safe frame buffer for video frames (double-buffered)
 */
class FVideoFrameBuffer
{
public:
	FVideoFrameBuffer();

	void Reset();
	bool WriteFrame(const FNativeVideoFrame& Frame);
	bool ReadFrame(FNativeVideoFrame& OutFrame);
	bool HasNewFrame() const;

private:
	FNativeVideoFrame Frames[2];
	TAtomic<int32> WriteIndex;
	TAtomic<int32> ReadIndex;
	TAtomic<bool> bNewFrameAvailable;
	FCriticalSection FrameLock;
};
