// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeMediaPlayer.h"
#include "HAL/Thread.h"

#if PLATFORM_WINDOWS

// Forward declarations for MPV types (avoid including mpv headers in UE headers)
struct mpv_handle;
struct mpv_render_context;

/**
 * MPV-based media player for Windows
 * Uses libmpv for decoding with excellent codec support via FFmpeg
 */
class FMpvMediaPlayer : public INativeMediaPlayer
{
public:
	FMpvMediaPlayer();
	virtual ~FMpvMediaPlayer();

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
	// Decode thread entry point
	uint32 DecodeThreadRun();
	void StopDecodeThread();

	// MPV handles
	mpv_handle* MpvHandle = nullptr;
	mpv_render_context* RenderContext = nullptr;

	// State
	bool bInitialized = false;
	TAtomic<bool> bShouldStop;
	ENativePlaybackState State = ENativePlaybackState::Idle;
	FNativeMediaInfo MediaInfo;

	// Threading - using TUniquePtr<FThread> for cleaner thread management
	TUniquePtr<FThread> DecodeThread;
	FCriticalSection StateLock;

	// Buffers
	TSharedPtr<FAudioRingBuffer> AudioBuffer;
	TSharedPtr<FVideoFrameBuffer> VideoBuffer;

	// Current playback info
	TAtomic<int64> CurrentPositionUs;
	TAtomic<int64> DurationUs;
	TAtomic<float> CurrentVolume;
	TAtomic<int64> AudioClockUs;

	// Video frame rendering
	TArray<uint8> RenderBuffer;
	int32 RenderWidth = 0;
	int32 RenderHeight = 0;

	// Internal methods
	bool LoadMpvLibrary();
	void UnloadMpvLibrary();
	bool CreateMpvContext();
	void DestroyMpvContext();
	bool SetupRenderContext();
	void ProcessEvents();
	void RenderVideoFrame();
	void UpdateMediaInfo();
	void SetState(ENativePlaybackState NewState);

	// MPV event callback
	static void OnMpvEvent(void* UserData);

	// MPV library handle
	void* MpvDllHandle = nullptr;

	// Function pointers for MPV API
	struct FMpvApi
	{
		// Core
		void* (*mpv_create)() = nullptr;
		int (*mpv_initialize)(mpv_handle*) = nullptr;
		void (*mpv_terminate_destroy)(mpv_handle*) = nullptr;
		int (*mpv_command)(mpv_handle*, const char**) = nullptr;
		int (*mpv_command_string)(mpv_handle*, const char*) = nullptr;
		int (*mpv_set_option_string)(mpv_handle*, const char*, const char*) = nullptr;
		int (*mpv_set_property_string)(mpv_handle*, const char*, const char*) = nullptr;
		int (*mpv_get_property)(mpv_handle*, const char*, int, void*) = nullptr;
		int (*mpv_get_property_string)(mpv_handle*, const char*, char**) = nullptr;
		void (*mpv_free)(void*) = nullptr;
		int (*mpv_observe_property)(mpv_handle*, uint64_t, const char*, int) = nullptr;
		void (*mpv_set_wakeup_callback)(mpv_handle*, void(*)(void*), void*) = nullptr;
		void* (*mpv_wait_event)(mpv_handle*, double) = nullptr;
		const char* (*mpv_error_string)(int) = nullptr;

		// Render API
		int (*mpv_render_context_create)(mpv_render_context**, mpv_handle*, void*) = nullptr;
		void (*mpv_render_context_free)(mpv_render_context*) = nullptr;
		int (*mpv_render_context_render)(mpv_render_context*, void*) = nullptr;
		void (*mpv_render_context_set_update_callback)(mpv_render_context*, void(*)(void*), void*) = nullptr;
		uint64_t (*mpv_render_context_update)(mpv_render_context*) = nullptr;

		bool IsLoaded() const { return mpv_create != nullptr; }
	};
	FMpvApi Api;
};

#endif // PLATFORM_WINDOWS
