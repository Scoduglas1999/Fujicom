// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "API/JellyfinTypes.h"
#include "Jellyfin3DVideo.generated.h"

/**
 * Eye identifier for stereoscopic rendering
 */
UENUM(BlueprintType)
enum class EStereoscopicEyeJellyfin : uint8
{
	Left UMETA(DisplayName = "Left Eye"),
	Right UMETA(DisplayName = "Right Eye")
};

/**
 * 3D video format types for Side-by-Side and Over-Under stereoscopic content
 */
UENUM(BlueprintType)
enum class E3DVideoFormat : uint8
{
	/** No 3D (standard 2D video) */
	None UMETA(DisplayName = "2D (No Stereo)"),

	/** Side-by-Side with full width per eye (requires 2x width video) */
	SBS_FullWidth UMETA(DisplayName = "Side-by-Side Full Width"),

	/** Side-by-Side with half width per eye (most common format) */
	SBS_HalfWidth UMETA(DisplayName = "Side-by-Side Half Width"),

	/** Over-Under with full height per eye (requires 2x height video) */
	OU_FullHeight UMETA(DisplayName = "Over-Under Full Height"),

	/** Over-Under with half height per eye */
	OU_HalfHeight UMETA(DisplayName = "Over-Under Half Height")
};

/**
 * Component that handles 3D stereoscopic video detection and rendering for VR
 *
 * Supports Side-by-Side (SBS) and Over-Under (OU) formats, both full and half resolution.
 * Automatically detects format from Jellyfin metadata or filename patterns.
 * Provides UV coordinate mapping for left/right eye rendering in VR.
 */
UCLASS(ClassGroup=(JellyfinVR), meta=(BlueprintSpawnableComponent))
class JELLYFINVR_API UJellyfin3DVideoComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UJellyfin3DVideoComponent();

	// ============ Format Detection ============

	/**
	 * Detect 3D format from a media item's metadata
	 * Checks the Video3DFormat field first, then falls back to filename detection
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|3D Video")
	E3DVideoFormat DetectFormatFromMediaItem(const FJellyfinMediaItem& MediaItem);

	/**
	 * Detect 3D format from filename patterns
	 * Recognizes common naming conventions like:
	 * - "movie.SBS.mp4", "movie.3D.SBS.mkv"
	 * - "movie.HSBS.mp4" (half side-by-side)
	 * - "movie.OU.mp4", "movie.TAB.mkv" (top-and-bottom)
	 * - "movie.HOU.mp4", "movie.HTAB.mp4" (half over-under)
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|3D Video")
	E3DVideoFormat DetectFormatFromFilename(const FString& Filename);

	/**
	 * Manually set the 3D format
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|3D Video")
	void SetFormat(E3DVideoFormat NewFormat);

	/**
	 * Get the current 3D format
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|3D Video")
	E3DVideoFormat GetFormat() const { return CurrentFormat; }

	// ============ Eye UV Coordinates ============

	/**
	 * Get UV rectangle for a specific eye
	 * Returns the portion of the video texture that should be displayed to each eye
	 *
	 * For 2D video: Both eyes get FBox2D(0,0,1,1) - full texture
	 * For SBS: Left gets (0,0,0.5,1), Right gets (0.5,0,1,1)
	 * For OU: Left gets (0,0,1,0.5), Right gets (0,0.5,1,1)
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|3D Video")
	FBox2D GetEyeUVRect(EStereoscopicEyeJellyfin Eye) const;

	/**
	 * Get UV rectangle for left eye
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|3D Video")
	FBox2D GetLeftEyeUVRect() const { return GetEyeUVRect(EStereoscopicEyeJellyfin::Left); }

	/**
	 * Get UV rectangle for right eye
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|3D Video")
	FBox2D GetRightEyeUVRect() const { return GetEyeUVRect(EStereoscopicEyeJellyfin::Right); }

	/**
	 * Get UV coordinates as Vector4 for shader use (MinU, MinV, MaxU, MaxV)
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|3D Video")
	FVector4 GetEyeUVRectAsVector4(EStereoscopicEyeJellyfin Eye) const;

	// ============ Format Queries ============

	/**
	 * Check if current video is 3D (not 2D)
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|3D Video")
	bool Is3DVideo() const { return CurrentFormat != E3DVideoFormat::None; }

	/**
	 * Check if format is Side-by-Side
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|3D Video")
	bool IsSideBySide() const;

	/**
	 * Check if format is Over-Under
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|3D Video")
	bool IsOverUnder() const;

	/**
	 * Check if format uses full resolution per eye (requires 2x resolution source)
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|3D Video")
	bool IsFullResolution() const;

	/**
	 * Get display name for current format
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|3D Video")
	FString GetFormatDisplayName() const;

	/**
	 * Get aspect ratio multiplier for the format
	 * SBS formats return 0.5 (video is 2x wider), OU formats return 2.0 (video is 2x taller)
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|3D Video")
	float GetAspectRatioMultiplier() const;

	// ============ Material Integration ============

	/**
	 * Apply 3D format to a material instance dynamic
	 * Sets shader parameters for UV mapping based on current format
	 *
	 * Expected shader parameters:
	 * - Is3DVideo (Scalar): 1.0 if 3D, 0.0 if 2D
	 * - LeftEyeUV (Vector4): Left eye UV rect (MinU, MinV, MaxU, MaxV)
	 * - RightEyeUV (Vector4): Right eye UV rect (MinU, MinV, MaxU, MaxV)
	 * - Format3D (Scalar): Format enum value as float
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|3D Video")
	void ApplyToMaterial(UMaterialInstanceDynamic* Material);

	// ============ Events ============

	/** Broadcast when 3D format changes */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOn3DFormatChanged, E3DVideoFormat, NewFormat);

	UPROPERTY(BlueprintAssignable, Category = "JellyfinVR|Events")
	FOn3DFormatChanged On3DFormatChanged;

protected:
	/**
	 * Convert Jellyfin 3D format enum to our internal format
	 */
	E3DVideoFormat ConvertJellyfinFormat(EJellyfin3DFormat JellyfinFormat) const;

	/**
	 * Parse filename for 3D format indicators
	 */
	E3DVideoFormat ParseFilenameForFormat(const FString& Filename) const;

private:
	/** Current 3D video format */
	UPROPERTY()
	E3DVideoFormat CurrentFormat = E3DVideoFormat::None;
};
