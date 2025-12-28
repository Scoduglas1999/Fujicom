// Copyright JellyVR Project. All Rights Reserved.

#include "MpvMediaPlayer.h"
#include "JellyfinVRModule.h"

#if PLATFORM_WINDOWS

#include "Windows/WindowsPlatformProcess.h"
#include "Misc/Paths.h"

// MPV constants (from mpv/client.h)
#define MPV_FORMAT_NONE 0
#define MPV_FORMAT_STRING 1
#define MPV_FORMAT_OSD_STRING 2
#define MPV_FORMAT_FLAG 3
#define MPV_FORMAT_INT64 4
#define MPV_FORMAT_DOUBLE 5
#define MPV_FORMAT_NODE 6

#define MPV_EVENT_NONE 0
#define MPV_EVENT_SHUTDOWN 1
#define MPV_EVENT_LOG_MESSAGE 2
#define MPV_EVENT_GET_PROPERTY_REPLY 3
#define MPV_EVENT_SET_PROPERTY_REPLY 4
#define MPV_EVENT_COMMAND_REPLY 5
#define MPV_EVENT_START_FILE 6
#define MPV_EVENT_END_FILE 7
#define MPV_EVENT_FILE_LOADED 8
#define MPV_EVENT_IDLE 11
#define MPV_EVENT_TICK 14
#define MPV_EVENT_PROPERTY_CHANGE 22

#define MPV_END_FILE_REASON_EOF 0
#define MPV_END_FILE_REASON_STOP 2
#define MPV_END_FILE_REASON_ERROR 4

// MPV render constants
#define MPV_RENDER_API_TYPE_SW 2
#define MPV_RENDER_PARAM_API_TYPE 1
#define MPV_RENDER_PARAM_SW_SIZE 13
#define MPV_RENDER_PARAM_SW_FORMAT 14
#define MPV_RENDER_PARAM_SW_STRIDE 15
#define MPV_RENDER_PARAM_SW_POINTER 16

// MPV event structure (simplified)
struct mpv_event
{
	int event_id;
	int error;
	uint64_t reply_userdata;
	void* data;
};

struct mpv_event_property
{
	const char* name;
	int format;
	void* data;
};

struct mpv_event_end_file
{
	int reason;
	int error;
};

// MPV render param
struct mpv_render_param
{
	int type;
	void* data;
};

FMpvMediaPlayer::FMpvMediaPlayer()
	: bShouldStop(false)
	, CurrentPositionUs(0)
	, DurationUs(0)
	, CurrentVolume(1.0f)
	, AudioClockUs(0)
{
	AudioBuffer = MakeShared<FAudioRingBuffer>();
	VideoBuffer = MakeShared<FVideoFrameBuffer>();
}

FMpvMediaPlayer::~FMpvMediaPlayer()
{
	Shutdown();
}

bool FMpvMediaPlayer::LoadMpvLibrary()
{
	// Look for libmpv in the plugin's binaries folder
	FString PluginDir = FPaths::Combine(FPaths::ProjectPluginsDir(), TEXT("JellyfinVR"));
	FString MpvPath = FPaths::Combine(PluginDir, TEXT("Binaries/Win64/libmpv-2.dll"));

	// Fallback to project binaries
	if (!FPaths::FileExists(MpvPath))
	{
		MpvPath = FPaths::Combine(FPaths::ProjectDir(), TEXT("Binaries/Win64/libmpv-2.dll"));
	}

	// Fallback to system PATH
	if (!FPaths::FileExists(MpvPath))
	{
		MpvPath = TEXT("libmpv-2.dll");
	}

	UE_LOG(LogJellyfinVR, Log, TEXT("Loading MPV library from: %s"), *MpvPath);

	MpvDllHandle = FPlatformProcess::GetDllHandle(*MpvPath);
	if (!MpvDllHandle)
	{
		UE_LOG(LogJellyfinVR, Error, TEXT("Failed to load MPV library"));
		return false;
	}

	// Load function pointers
#define LOAD_MPV_FUNC(name) \
	Api.name = (decltype(Api.name))FPlatformProcess::GetDllExport(MpvDllHandle, TEXT(#name)); \
	if (!Api.name) { UE_LOG(LogJellyfinVR, Warning, TEXT("Failed to load MPV function: " #name)); }

	LOAD_MPV_FUNC(mpv_create);
	LOAD_MPV_FUNC(mpv_initialize);
	LOAD_MPV_FUNC(mpv_terminate_destroy);
	LOAD_MPV_FUNC(mpv_command);
	LOAD_MPV_FUNC(mpv_command_string);
	LOAD_MPV_FUNC(mpv_set_option_string);
	LOAD_MPV_FUNC(mpv_set_property_string);
	LOAD_MPV_FUNC(mpv_get_property);
	LOAD_MPV_FUNC(mpv_get_property_string);
	LOAD_MPV_FUNC(mpv_free);
	LOAD_MPV_FUNC(mpv_observe_property);
	LOAD_MPV_FUNC(mpv_set_wakeup_callback);
	LOAD_MPV_FUNC(mpv_wait_event);
	LOAD_MPV_FUNC(mpv_error_string);
	LOAD_MPV_FUNC(mpv_render_context_create);
	LOAD_MPV_FUNC(mpv_render_context_free);
	LOAD_MPV_FUNC(mpv_render_context_render);
	LOAD_MPV_FUNC(mpv_render_context_set_update_callback);
	LOAD_MPV_FUNC(mpv_render_context_update);

#undef LOAD_MPV_FUNC

	if (!Api.IsLoaded())
	{
		UE_LOG(LogJellyfinVR, Error, TEXT("Failed to load required MPV functions"));
		UnloadMpvLibrary();
		return false;
	}

	UE_LOG(LogJellyfinVR, Log, TEXT("MPV library loaded successfully"));
	return true;
}

void FMpvMediaPlayer::UnloadMpvLibrary()
{
	if (MpvDllHandle)
	{
		FPlatformProcess::FreeDllHandle(MpvDllHandle);
		MpvDllHandle = nullptr;
	}
	FMemory::Memzero(&Api, sizeof(Api));
}

bool FMpvMediaPlayer::CreateMpvContext()
{
	MpvHandle = (mpv_handle*)Api.mpv_create();
	if (!MpvHandle)
	{
		UE_LOG(LogJellyfinVR, Error, TEXT("Failed to create MPV context"));
		return false;
	}

	// Configure MPV for software decoding to textures
	Api.mpv_set_option_string(MpvHandle, "vo", "libmpv");
	Api.mpv_set_option_string(MpvHandle, "hwdec", "auto-safe");
	Api.mpv_set_option_string(MpvHandle, "keep-open", "yes");
	Api.mpv_set_option_string(MpvHandle, "idle", "yes");

	// Audio: we want to capture PCM, not play directly
	// Using --ao=null and --audio-buffer to get audio data via properties
	Api.mpv_set_option_string(MpvHandle, "ao", "null");
	Api.mpv_set_option_string(MpvHandle, "audio-buffer", "0.5");

	// Network options for streaming
	Api.mpv_set_option_string(MpvHandle, "cache", "yes");
	Api.mpv_set_option_string(MpvHandle, "demuxer-max-bytes", "50MiB");
	Api.mpv_set_option_string(MpvHandle, "demuxer-max-back-bytes", "25MiB");

	// Enable user agent for Jellyfin
	Api.mpv_set_option_string(MpvHandle, "user-agent", "JellyVR/1.0");

	// Initialize
	int result = Api.mpv_initialize(MpvHandle);
	if (result < 0)
	{
		UE_LOG(LogJellyfinVR, Error, TEXT("Failed to initialize MPV: %s"),
			ANSI_TO_TCHAR(Api.mpv_error_string(result)));
		Api.mpv_terminate_destroy(MpvHandle);
		MpvHandle = nullptr;
		return false;
	}

	// Observe properties for state tracking
	Api.mpv_observe_property(MpvHandle, 0, "pause", MPV_FORMAT_FLAG);
	Api.mpv_observe_property(MpvHandle, 0, "eof-reached", MPV_FORMAT_FLAG);
	Api.mpv_observe_property(MpvHandle, 0, "time-pos", MPV_FORMAT_DOUBLE);
	Api.mpv_observe_property(MpvHandle, 0, "duration", MPV_FORMAT_DOUBLE);
	Api.mpv_observe_property(MpvHandle, 0, "volume", MPV_FORMAT_DOUBLE);
	Api.mpv_observe_property(MpvHandle, 0, "width", MPV_FORMAT_INT64);
	Api.mpv_observe_property(MpvHandle, 0, "height", MPV_FORMAT_INT64);

	// Set wakeup callback
	Api.mpv_set_wakeup_callback(MpvHandle, &FMpvMediaPlayer::OnMpvEvent, this);

	UE_LOG(LogJellyfinVR, Log, TEXT("MPV context created successfully"));
	return true;
}

void FMpvMediaPlayer::DestroyMpvContext()
{
	if (RenderContext)
	{
		Api.mpv_render_context_free(RenderContext);
		RenderContext = nullptr;
	}

	if (MpvHandle)
	{
		Api.mpv_terminate_destroy(MpvHandle);
		MpvHandle = nullptr;
	}
}

bool FMpvMediaPlayer::SetupRenderContext()
{
	if (!MpvHandle || !Api.mpv_render_context_create)
	{
		return false;
	}

	// Create software render context
	int sw_api = MPV_RENDER_API_TYPE_SW;
	mpv_render_param params[] = {
		{ MPV_RENDER_PARAM_API_TYPE, &sw_api },
		{ 0, nullptr }
	};

	int result = Api.mpv_render_context_create(&RenderContext, MpvHandle, params);
	if (result < 0)
	{
		UE_LOG(LogJellyfinVR, Error, TEXT("Failed to create MPV render context: %s"),
			ANSI_TO_TCHAR(Api.mpv_error_string(result)));
		return false;
	}

	UE_LOG(LogJellyfinVR, Log, TEXT("MPV render context created"));
	return true;
}

bool FMpvMediaPlayer::Initialize()
{
	if (bInitialized)
	{
		return true;
	}

	if (!LoadMpvLibrary())
	{
		return false;
	}

	if (!CreateMpvContext())
	{
		UnloadMpvLibrary();
		return false;
	}

	if (!SetupRenderContext())
	{
		DestroyMpvContext();
		UnloadMpvLibrary();
		return false;
	}

	// Start decode thread
	bShouldStop = false;
	DecodeThread = MakeUnique<FThread>(TEXT("MpvDecodeThread"), [this]() { DecodeThreadRun(); });

	bInitialized = true;
	UE_LOG(LogJellyfinVR, Log, TEXT("MPV Media Player initialized successfully"));
	return true;
}

void FMpvMediaPlayer::Shutdown()
{
	if (!bInitialized)
	{
		return;
	}

	// Stop decode thread
	StopDecodeThread();

	DestroyMpvContext();
	UnloadMpvLibrary();

	AudioBuffer->Reset();
	VideoBuffer->Reset();

	bInitialized = false;
	UE_LOG(LogJellyfinVR, Log, TEXT("MPV Media Player shutdown"));
}

bool FMpvMediaPlayer::Open(const FString& Url, const TMap<FString, FString>& Headers)
{
	if (!bInitialized || !MpvHandle)
	{
		return false;
	}

	// Build headers string for HTTP requests
	if (Headers.Num() > 0)
	{
		FString HeaderStr;
		for (const auto& Pair : Headers)
		{
			HeaderStr += FString::Printf(TEXT("%s: %s\r\n"), *Pair.Key, *Pair.Value);
		}
		Api.mpv_set_option_string(MpvHandle, "http-header-fields", TCHAR_TO_ANSI(*HeaderStr));
	}

	SetState(ENativePlaybackState::Opening);

	// Load the file
	const char* cmd[] = { "loadfile", TCHAR_TO_ANSI(*Url), nullptr };
	int result = Api.mpv_command(MpvHandle, cmd);

	if (result < 0)
	{
		UE_LOG(LogJellyfinVR, Error, TEXT("Failed to open media: %s"), ANSI_TO_TCHAR(Api.mpv_error_string(result)));
		SetState(ENativePlaybackState::Error);
		OnError.ExecuteIfBound(FString::Printf(TEXT("Failed to open: %s"), ANSI_TO_TCHAR(Api.mpv_error_string(result))));
		return false;
	}

	UE_LOG(LogJellyfinVR, Log, TEXT("Opening media: %s"), *Url);
	return true;
}

void FMpvMediaPlayer::Close()
{
	if (!MpvHandle)
	{
		return;
	}

	const char* cmd[] = { "stop", nullptr };
	Api.mpv_command(MpvHandle, cmd);

	AudioBuffer->Reset();
	VideoBuffer->Reset();
	MediaInfo = FNativeMediaInfo();
	SetState(ENativePlaybackState::Idle);
}

bool FMpvMediaPlayer::Play()
{
	if (!MpvHandle)
	{
		return false;
	}

	Api.mpv_set_property_string(MpvHandle, "pause", "no");
	return true;
}

bool FMpvMediaPlayer::Pause()
{
	if (!MpvHandle)
	{
		return false;
	}

	Api.mpv_set_property_string(MpvHandle, "pause", "yes");
	return true;
}

bool FMpvMediaPlayer::Stop()
{
	Close();
	return true;
}

bool FMpvMediaPlayer::Seek(int64 PositionUs)
{
	if (!MpvHandle)
	{
		return false;
	}

	double PositionSec = PositionUs / 1000000.0;
	FString SeekCmd = FString::Printf(TEXT("seek %f absolute"), PositionSec);
	Api.mpv_command_string(MpvHandle, TCHAR_TO_ANSI(*SeekCmd));
	return true;
}

int64 FMpvMediaPlayer::GetPosition() const
{
	return CurrentPositionUs.Load();
}

int64 FMpvMediaPlayer::GetDuration() const
{
	return DurationUs.Load();
}

float FMpvMediaPlayer::GetVolume() const
{
	return CurrentVolume.Load();
}

void FMpvMediaPlayer::SetVolume(float Volume)
{
	if (!MpvHandle)
	{
		return;
	}

	Volume = FMath::Clamp(Volume, 0.0f, 1.0f);
	CurrentVolume = Volume;

	// MPV uses 0-100 scale
	FString VolumeStr = FString::Printf(TEXT("%d"), FMath::RoundToInt(Volume * 100.0f));
	Api.mpv_set_property_string(MpvHandle, "volume", TCHAR_TO_ANSI(*VolumeStr));
}

bool FMpvMediaPlayer::HasNewVideoFrame() const
{
	return VideoBuffer.IsValid() && VideoBuffer->HasNewFrame();
}

bool FMpvMediaPlayer::GetVideoFrame(FNativeVideoFrame& OutFrame)
{
	if (!VideoBuffer.IsValid())
	{
		return false;
	}
	return VideoBuffer->ReadFrame(OutFrame);
}

bool FMpvMediaPlayer::HasAudioSamples() const
{
	return AudioBuffer.IsValid() && AudioBuffer->Available() > 0;
}

bool FMpvMediaPlayer::GetAudioSamples(FNativeAudioSamples& OutSamples, int32 NumSamplesRequested)
{
	if (!AudioBuffer.IsValid())
	{
		return false;
	}

	OutSamples.Samples.SetNumUninitialized(NumSamplesRequested);
	int32 Read = AudioBuffer->Read(OutSamples.Samples.GetData(), NumSamplesRequested);

	if (Read < NumSamplesRequested)
	{
		// Zero-fill remaining
		FMemory::Memzero(OutSamples.Samples.GetData() + Read, (NumSamplesRequested - Read) * sizeof(float));
	}

	OutSamples.NumChannels = MediaInfo.AudioChannels > 0 ? MediaInfo.AudioChannels : 2;
	OutSamples.SampleRate = MediaInfo.AudioSampleRate > 0 ? MediaInfo.AudioSampleRate : 48000;
	OutSamples.TimestampUs = AudioClockUs.Load();

	return Read > 0;
}

int64 FMpvMediaPlayer::GetAudioClock() const
{
	return AudioClockUs.Load();
}

void FMpvMediaPlayer::SetState(ENativePlaybackState NewState)
{
	FScopeLock Lock(&StateLock);
	if (State != NewState)
	{
		State = NewState;
		OnStateChanged.ExecuteIfBound(NewState);
	}
}

void FMpvMediaPlayer::OnMpvEvent(void* UserData)
{
	// Called from MPV thread when events are available
	// We process events in our decode thread
}

uint32 FMpvMediaPlayer::DecodeThreadRun()
{
	UE_LOG(LogJellyfinVR, Log, TEXT("MPV decode thread started"));

	while (!bShouldStop)
	{
		ProcessEvents();

		// Render video frame if playing
		if (State == ENativePlaybackState::Playing)
		{
			RenderVideoFrame();
		}

		// Small sleep to prevent busy-waiting
		FPlatformProcess::Sleep(0.001f); // 1ms
	}

	UE_LOG(LogJellyfinVR, Log, TEXT("MPV decode thread stopped"));
	return 0;
}

void FMpvMediaPlayer::StopDecodeThread()
{
	bShouldStop = true;
	if (DecodeThread)
	{
		DecodeThread->Join();
		DecodeThread.Reset();
	}
}

void FMpvMediaPlayer::ProcessEvents()
{
	if (!MpvHandle)
	{
		return;
	}

	while (true)
	{
		mpv_event* Event = (mpv_event*)Api.mpv_wait_event(MpvHandle, 0);
		if (!Event || Event->event_id == MPV_EVENT_NONE)
		{
			break;
		}

		switch (Event->event_id)
		{
		case MPV_EVENT_FILE_LOADED:
			UE_LOG(LogJellyfinVR, Log, TEXT("MPV: File loaded"));
			UpdateMediaInfo();
			SetState(ENativePlaybackState::Playing);
			OnMediaOpened.ExecuteIfBound(true);
			break;

		case MPV_EVENT_END_FILE:
			{
				mpv_event_end_file* EndFile = (mpv_event_end_file*)Event->data;
				if (EndFile)
				{
					if (EndFile->reason == MPV_END_FILE_REASON_EOF)
					{
						UE_LOG(LogJellyfinVR, Log, TEXT("MPV: End of file"));
						SetState(ENativePlaybackState::EndOfMedia);
						OnEndReached.ExecuteIfBound();
					}
					else if (EndFile->reason == MPV_END_FILE_REASON_ERROR)
					{
						UE_LOG(LogJellyfinVR, Error, TEXT("MPV: Playback error"));
						SetState(ENativePlaybackState::Error);
						OnError.ExecuteIfBound(TEXT("Playback error"));
					}
				}
			}
			break;

		case MPV_EVENT_PROPERTY_CHANGE:
			{
				mpv_event_property* Prop = (mpv_event_property*)Event->data;
				if (Prop && Prop->data)
				{
					FString PropName = ANSI_TO_TCHAR(Prop->name);

					if (PropName == TEXT("pause") && Prop->format == MPV_FORMAT_FLAG)
					{
						bool bPaused = *(int*)Prop->data;
						if (bPaused && State == ENativePlaybackState::Playing)
						{
							SetState(ENativePlaybackState::Paused);
						}
						else if (!bPaused && State == ENativePlaybackState::Paused)
						{
							SetState(ENativePlaybackState::Playing);
						}
					}
					else if (PropName == TEXT("time-pos") && Prop->format == MPV_FORMAT_DOUBLE)
					{
						double PosSec = *(double*)Prop->data;
						CurrentPositionUs = (int64)(PosSec * 1000000.0);
						AudioClockUs = CurrentPositionUs.Load();
					}
					else if (PropName == TEXT("duration") && Prop->format == MPV_FORMAT_DOUBLE)
					{
						double DurSec = *(double*)Prop->data;
						DurationUs = (int64)(DurSec * 1000000.0);
						MediaInfo.DurationUs = DurationUs.Load();
					}
					else if (PropName == TEXT("width") && Prop->format == MPV_FORMAT_INT64)
					{
						MediaInfo.VideoWidth = (int32)(*(int64*)Prop->data);
					}
					else if (PropName == TEXT("height") && Prop->format == MPV_FORMAT_INT64)
					{
						MediaInfo.VideoHeight = (int32)(*(int64*)Prop->data);
					}
				}
			}
			break;
		}
	}
}

void FMpvMediaPlayer::RenderVideoFrame()
{
	if (!RenderContext || !Api.mpv_render_context_render)
	{
		return;
	}

	// Check if new frame is available
	uint64_t flags = Api.mpv_render_context_update(RenderContext);
	if (!(flags & 1)) // MPV_RENDER_UPDATE_FRAME
	{
		return;
	}

	// Get video dimensions
	int32 Width = MediaInfo.VideoWidth;
	int32 Height = MediaInfo.VideoHeight;

	if (Width <= 0 || Height <= 0)
	{
		return;
	}

	// Resize render buffer if needed
	if (RenderWidth != Width || RenderHeight != Height)
	{
		RenderWidth = Width;
		RenderHeight = Height;
		RenderBuffer.SetNumZeroed(Width * Height * 4); // BGRA
		UE_LOG(LogJellyfinVR, Log, TEXT("MPV: Video resolution %dx%d"), Width, Height);
	}

	// Setup render parameters for software rendering
	int size[2] = { Width, Height };
	int stride = Width * 4;
	const char* format = "bgra";

	mpv_render_param params[] = {
		{ MPV_RENDER_PARAM_SW_SIZE, size },
		{ MPV_RENDER_PARAM_SW_FORMAT, (void*)format },
		{ MPV_RENDER_PARAM_SW_STRIDE, &stride },
		{ MPV_RENDER_PARAM_SW_POINTER, RenderBuffer.GetData() },
		{ 0, nullptr }
	};

	int result = Api.mpv_render_context_render(RenderContext, params);
	if (result < 0)
	{
		return;
	}

	// Copy to video buffer
	FNativeVideoFrame Frame;
	Frame.Width = Width;
	Frame.Height = Height;
	Frame.Pixels = RenderBuffer;
	Frame.TimestampUs = CurrentPositionUs.Load();
	Frame.bIsValid = true;

	VideoBuffer->WriteFrame(Frame);
}

void FMpvMediaPlayer::UpdateMediaInfo()
{
	if (!MpvHandle)
	{
		return;
	}

	// Get video info
	int64 Width = 0, Height = 0;
	Api.mpv_get_property(MpvHandle, "width", MPV_FORMAT_INT64, &Width);
	Api.mpv_get_property(MpvHandle, "height", MPV_FORMAT_INT64, &Height);
	MediaInfo.VideoWidth = (int32)Width;
	MediaInfo.VideoHeight = (int32)Height;

	// Get duration
	double Duration = 0;
	Api.mpv_get_property(MpvHandle, "duration", MPV_FORMAT_DOUBLE, &Duration);
	MediaInfo.DurationUs = (int64)(Duration * 1000000.0);
	DurationUs = MediaInfo.DurationUs;

	// Get codec info
	char* VideoCodec = nullptr;
	char* AudioCodec = nullptr;
	Api.mpv_get_property_string(MpvHandle, "video-codec", &VideoCodec);
	Api.mpv_get_property_string(MpvHandle, "audio-codec", &AudioCodec);

	if (VideoCodec)
	{
		MediaInfo.VideoCodec = ANSI_TO_TCHAR(VideoCodec);
		Api.mpv_free(VideoCodec);
	}
	if (AudioCodec)
	{
		MediaInfo.AudioCodec = ANSI_TO_TCHAR(AudioCodec);
		Api.mpv_free(AudioCodec);
	}

	// Get audio info
	int64 AudioChannels = 0;
	int64 AudioSampleRate = 0;
	Api.mpv_get_property(MpvHandle, "audio-params/channel-count", MPV_FORMAT_INT64, &AudioChannels);
	Api.mpv_get_property(MpvHandle, "audio-params/samplerate", MPV_FORMAT_INT64, &AudioSampleRate);
	MediaInfo.AudioChannels = (int32)AudioChannels;
	MediaInfo.AudioSampleRate = (int32)AudioSampleRate;

	UE_LOG(LogJellyfinVR, Log, TEXT("MPV Media Info: %dx%d, %s/%s, Duration: %.1fs"),
		MediaInfo.VideoWidth, MediaInfo.VideoHeight,
		*MediaInfo.VideoCodec, *MediaInfo.AudioCodec,
		MediaInfo.DurationUs / 1000000.0);
}

#endif // PLATFORM_WINDOWS
