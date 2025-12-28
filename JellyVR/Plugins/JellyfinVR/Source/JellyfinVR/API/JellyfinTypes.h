// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "JellyfinTypes.generated.h"

/**
 * Jellyfin media item types
 */
UENUM(BlueprintType)
enum class EJellyfinItemType : uint8
{
	Unknown,
	Movie,
	Series,
	Season,
	Episode,
	Audio,
	MusicAlbum,
	MusicArtist,
	Folder,
	CollectionFolder,
	BoxSet,
	Playlist
};

/**
 * Jellyfin playback stream type
 */
UENUM(BlueprintType)
enum class EJellyfinStreamType : uint8
{
	DirectPlay,
	DirectStream,
	Transcode
};

/**
 * Video 3D format (for future support)
 */
UENUM(BlueprintType)
enum class EJellyfin3DFormat : uint8
{
	None,
	SideBySide,
	OverUnder,
	FullSideBySide,
	FullOverUnder
};

/**
 * Authentication state
 */
UENUM(BlueprintType)
enum class EJellyfinAuthState : uint8
{
	NotAuthenticated,
	Authenticating,
	Authenticated,
	Failed
};

/**
 * User session data
 */
USTRUCT(BlueprintType)
struct JELLYFINVR_API FJellyfinUserSession
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString UserId;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString Username;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString AccessToken;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString ServerId;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString ServerName;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	EJellyfinAuthState AuthState = EJellyfinAuthState::NotAuthenticated;

	bool IsValid() const { return !AccessToken.IsEmpty() && !UserId.IsEmpty(); }
};

/**
 * Image information
 */
USTRUCT(BlueprintType)
struct JELLYFINVR_API FJellyfinImageInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString ImageTag;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	int32 Width = 0;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	int32 Height = 0;
};

/**
 * Media stream information (audio/video/subtitle tracks)
 */
USTRUCT(BlueprintType)
struct JELLYFINVR_API FJellyfinMediaStream
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	int32 Index = 0;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString Type; // "Video", "Audio", "Subtitle"

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString Codec;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString Language;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString DisplayTitle;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	bool bIsDefault = false;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	bool bIsForced = false;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	bool bIsExternal = false;

	// Video-specific properties
	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	int32 Width = 0;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	int32 Height = 0;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	float AspectRatio = 0.0f;

	// HDR video support
	// Indicates if this stream contains HDR content (HDR10, Dolby Vision, HLG)
	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	bool bIsHDR = false;

	// Video dynamic range format from Jellyfin server
	// Values: "SDR", "HDR10", "DolbyVision", "HLG"
	// HDR10: SMPTE ST.2084 (PQ) transfer function with BT.2020 color space
	// Dolby Vision: Requires license, not currently supported
	// HLG: Hybrid Log-Gamma for broadcast HDR
	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString VideoRange; // "SDR", "HDR10", "DolbyVision", "HLG"

	// Audio-specific
	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	int32 Channels = 0;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	int32 SampleRate = 0;
};

/**
 * Chapter information
 */
USTRUCT(BlueprintType)
struct JELLYFINVR_API FJellyfinChapter
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString Name;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	int64 StartPositionTicks = 0;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString ImageTag;

	// Helper to convert ticks to seconds
	double GetStartPositionSeconds() const { return StartPositionTicks / 10000000.0; }
};

/**
 * Core media item data
 */
USTRUCT(BlueprintType)
struct JELLYFINVR_API FJellyfinMediaItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString Id;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString Name;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString SortName;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString Overview;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	EJellyfinItemType Type = EJellyfinItemType::Unknown;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString SeriesId;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString SeriesName;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString SeasonId;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString SeasonName;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	int32 IndexNumber = 0; // Episode number or season number

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	int32 ParentIndexNumber = 0; // Season number for episodes

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	int32 ProductionYear = 0;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString OfficialRating; // "PG-13", "R", etc.

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	float CommunityRating = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	int64 RunTimeTicks = 0;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	TArray<FString> Genres;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	TArray<FString> Studios;

	// Playback state
	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	int64 PlaybackPositionTicks = 0;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	bool bIsPlayed = false;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	bool bIsFavorite = false;

	// Image tags
	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString PrimaryImageTag;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString BackdropImageTag;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString ThumbImageTag;

	// Media info
	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	TArray<FJellyfinMediaStream> MediaStreams;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	TArray<FJellyfinChapter> Chapters;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString Container; // "mkv", "mp4", etc.

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString Path;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	int64 Size = 0;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	int32 Bitrate = 0;

	// 3D format
	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	EJellyfin3DFormat Video3DFormat = EJellyfin3DFormat::None;

	// Helper functions
	double GetRunTimeSeconds() const { return RunTimeTicks / 10000000.0; }
	double GetPlaybackPositionSeconds() const { return PlaybackPositionTicks / 10000000.0; }
	float GetPlaybackProgress() const { return RunTimeTicks > 0 ? (float)PlaybackPositionTicks / (float)RunTimeTicks : 0.0f; }
};

/**
 * Library/collection view
 */
USTRUCT(BlueprintType)
struct JELLYFINVR_API FJellyfinLibrary
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString Id;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString Name;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString CollectionType; // "movies", "tvshows", "music", etc.

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString PrimaryImageTag;
};

/**
 * Playback info for streaming
 */
USTRUCT(BlueprintType)
struct JELLYFINVR_API FJellyfinPlaybackInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString MediaSourceId;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString PlaySessionId;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString StreamUrl;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	EJellyfinStreamType StreamType = EJellyfinStreamType::DirectPlay;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString Container;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	bool bSupportsDirectPlay = false;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	bool bSupportsDirectStream = false;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	bool bSupportsTranscoding = false;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	TArray<FJellyfinMediaStream> MediaStreams;

	// Transcoding info
	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	int32 TranscodingBitrate = 0;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString TranscodingContainer;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString TranscodingVideoCodec;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString TranscodingAudioCodec;
};

/**
 * Search result hint
 */
USTRUCT(BlueprintType)
struct JELLYFINVR_API FJellyfinSearchHint
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString Id;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString Name;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	EJellyfinItemType Type = EJellyfinItemType::Unknown;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	int32 ProductionYear = 0;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString PrimaryImageTag;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString ThumbImageTag;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString SeriesName;
};

/**
 * Items query result
 */
USTRUCT(BlueprintType)
struct JELLYFINVR_API FJellyfinItemsResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	TArray<FJellyfinMediaItem> Items;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	int32 TotalRecordCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	int32 StartIndex = 0;
};

/**
 * Server connection settings
 */
USTRUCT(BlueprintType)
struct JELLYFINVR_API FJellyfinServerSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "JellyfinVR")
	FString ServerUrl;

	UPROPERTY(BlueprintReadWrite, Category = "JellyfinVR")
	FString Username;

	// Note: Password should not be stored, only used transiently for auth

	UPROPERTY(BlueprintReadWrite, Category = "JellyfinVR")
	bool bRememberMe = true;

	UPROPERTY(BlueprintReadWrite, Category = "JellyfinVR")
	int32 TranscodingBitrateMbps = 20; // Default 20 Mbps

	UPROPERTY(BlueprintReadWrite, Category = "JellyfinVR")
	bool bPreferDirectPlay = true;
};
