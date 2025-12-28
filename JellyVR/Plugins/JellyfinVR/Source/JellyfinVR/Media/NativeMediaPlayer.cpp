// Copyright JellyVR Project. All Rights Reserved.

#include "NativeMediaPlayer.h"

// Platform-specific includes
#if PLATFORM_WINDOWS
#include "MpvMediaPlayer.h"
#elif PLATFORM_ANDROID
#include "ExoMediaPlayer.h"
#endif

// ============================================================================
// FAudioRingBuffer Implementation
// ============================================================================

FAudioRingBuffer::FAudioRingBuffer(int32 InCapacitySamples)
	: ReadPos(0)
	, WritePos(0)
	, Capacity(InCapacitySamples)
{
	Buffer.SetNumZeroed(Capacity);
}

void FAudioRingBuffer::Reset()
{
	FScopeLock Lock(&BufferLock);
	ReadPos = 0;
	WritePos = 0;
	FMemory::Memzero(Buffer.GetData(), Buffer.Num() * sizeof(float));
}

int32 FAudioRingBuffer::Write(const float* Data, int32 NumSamples)
{
	FScopeLock Lock(&BufferLock);

	int32 Written = 0;
	int32 CurrentWritePos = WritePos.Load();
	int32 CurrentReadPos = ReadPos.Load();

	while (Written < NumSamples)
	{
		int32 NextWritePos = (CurrentWritePos + 1) % Capacity;

		// Check if buffer is full
		if (NextWritePos == CurrentReadPos)
		{
			break;
		}

		Buffer[CurrentWritePos] = Data[Written];
		CurrentWritePos = NextWritePos;
		Written++;
	}

	WritePos = CurrentWritePos;
	return Written;
}

int32 FAudioRingBuffer::Read(float* OutData, int32 NumSamples)
{
	FScopeLock Lock(&BufferLock);

	int32 Read = 0;
	int32 CurrentReadPos = ReadPos.Load();
	int32 CurrentWritePos = WritePos.Load();

	while (Read < NumSamples)
	{
		// Check if buffer is empty
		if (CurrentReadPos == CurrentWritePos)
		{
			break;
		}

		OutData[Read] = Buffer[CurrentReadPos];
		CurrentReadPos = (CurrentReadPos + 1) % Capacity;
		Read++;
	}

	ReadPos = CurrentReadPos;
	return Read;
}

int32 FAudioRingBuffer::Available() const
{
	int32 CurrentWritePos = WritePos.Load();
	int32 CurrentReadPos = ReadPos.Load();

	if (CurrentWritePos >= CurrentReadPos)
	{
		return CurrentWritePos - CurrentReadPos;
	}
	else
	{
		return Capacity - CurrentReadPos + CurrentWritePos;
	}
}

int32 FAudioRingBuffer::FreeSpace() const
{
	return Capacity - Available() - 1;
}

// ============================================================================
// FVideoFrameBuffer Implementation
// ============================================================================

FVideoFrameBuffer::FVideoFrameBuffer()
	: WriteIndex(0)
	, ReadIndex(0)
	, bNewFrameAvailable(false)
{
}

void FVideoFrameBuffer::Reset()
{
	FScopeLock Lock(&FrameLock);
	WriteIndex = 0;
	ReadIndex = 0;
	bNewFrameAvailable = false;
	Frames[0] = FNativeVideoFrame();
	Frames[1] = FNativeVideoFrame();
}

bool FVideoFrameBuffer::WriteFrame(const FNativeVideoFrame& Frame)
{
	FScopeLock Lock(&FrameLock);

	int32 CurrentWriteIndex = WriteIndex.Load();
	Frames[CurrentWriteIndex] = Frame;

	// Swap buffers
	WriteIndex = 1 - CurrentWriteIndex;
	ReadIndex = CurrentWriteIndex;
	bNewFrameAvailable = true;

	return true;
}

bool FVideoFrameBuffer::ReadFrame(FNativeVideoFrame& OutFrame)
{
	FScopeLock Lock(&FrameLock);

	if (!bNewFrameAvailable.Load())
	{
		return false;
	}

	OutFrame = Frames[ReadIndex.Load()];
	bNewFrameAvailable = false;

	return OutFrame.bIsValid;
}

bool FVideoFrameBuffer::HasNewFrame() const
{
	return bNewFrameAvailable.Load();
}

// ============================================================================
// INativeMediaPlayer Factory
// ============================================================================

TSharedPtr<INativeMediaPlayer> INativeMediaPlayer::Create()
{
#if PLATFORM_WINDOWS
	return MakeShared<FMpvMediaPlayer>();
#elif PLATFORM_ANDROID
	return MakeShared<FExoMediaPlayer>();
#else
	UE_LOG(LogTemp, Error, TEXT("NativeMediaPlayer: Platform not supported"));
	return nullptr;
#endif
}

bool INativeMediaPlayer::IsPlatformSupported()
{
#if PLATFORM_WINDOWS || PLATFORM_ANDROID
	return true;
#else
	return false;
#endif
}

FString INativeMediaPlayer::GetPlatformPlayerName()
{
#if PLATFORM_WINDOWS
	return TEXT("MPV (libmpv)");
#elif PLATFORM_ANDROID
	return TEXT("ExoPlayer");
#else
	return TEXT("Unsupported");
#endif
}
