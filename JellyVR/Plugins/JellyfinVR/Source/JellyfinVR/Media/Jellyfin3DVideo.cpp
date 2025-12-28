// Copyright JellyVR Project. All Rights Reserved.

#include "Jellyfin3DVideo.h"
#include "JellyfinVRModule.h"
#include "Materials/MaterialInstanceDynamic.h"

UJellyfin3DVideoComponent::UJellyfin3DVideoComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

E3DVideoFormat UJellyfin3DVideoComponent::DetectFormatFromMediaItem(const FJellyfinMediaItem& MediaItem)
{
	// First, check if Jellyfin server provided explicit 3D format metadata
	if (MediaItem.Video3DFormat != EJellyfin3DFormat::None)
	{
		E3DVideoFormat DetectedFormat = ConvertJellyfinFormat(MediaItem.Video3DFormat);
		SetFormat(DetectedFormat);
		UE_LOG(LogJellyfinVR, Log, TEXT("Detected 3D format from Jellyfin metadata: %s"), *GetFormatDisplayName());
		return DetectedFormat;
	}

	// Fall back to filename detection
	E3DVideoFormat FilenameFormat = DetectFormatFromFilename(MediaItem.Name);
	if (FilenameFormat != E3DVideoFormat::None)
	{
		SetFormat(FilenameFormat);
		UE_LOG(LogJellyfinVR, Log, TEXT("Detected 3D format from filename '%s': %s"), *MediaItem.Name, *GetFormatDisplayName());
		return FilenameFormat;
	}

	// Also check the full path if available
	if (!MediaItem.Path.IsEmpty())
	{
		E3DVideoFormat PathFormat = DetectFormatFromFilename(MediaItem.Path);
		if (PathFormat != E3DVideoFormat::None)
		{
			SetFormat(PathFormat);
			UE_LOG(LogJellyfinVR, Log, TEXT("Detected 3D format from path '%s': %s"), *MediaItem.Path, *GetFormatDisplayName());
			return PathFormat;
		}
	}

	// No 3D format detected - treat as 2D
	SetFormat(E3DVideoFormat::None);
	UE_LOG(LogJellyfinVR, Verbose, TEXT("No 3D format detected for '%s' - treating as 2D"), *MediaItem.Name);
	return E3DVideoFormat::None;
}

E3DVideoFormat UJellyfin3DVideoComponent::DetectFormatFromFilename(const FString& Filename)
{
	return ParseFilenameForFormat(Filename);
}

E3DVideoFormat UJellyfin3DVideoComponent::ParseFilenameForFormat(const FString& Filename) const
{
	// Convert to uppercase for case-insensitive matching
	FString UpperFilename = Filename.ToUpper();

	// Remove file extension for cleaner matching
	int32 DotIndex;
	if (UpperFilename.FindLastChar('.', DotIndex))
	{
		UpperFilename = UpperFilename.Left(DotIndex);
	}

	// Common patterns for 3D video filenames:
	// - Half Side-by-Side: HSBS, H-SBS, SBS, 3DSBS
	// - Full Side-by-Side: FSBS, F-SBS, FULLSBS
	// - Half Over-Under: HOU, H-OU, HTAB, H-TAB, OU, TAB, TB
	// - Full Over-Under: FOU, F-OU, FTAB, F-TAB, FULLOU

	// Check for Half Side-by-Side (most common format)
	if (UpperFilename.Contains(TEXT("HSBS")) ||
		UpperFilename.Contains(TEXT("H-SBS")) ||
		UpperFilename.Contains(TEXT("H.SBS")) ||
		UpperFilename.Contains(TEXT("HALFSBS")) ||
		UpperFilename.Contains(TEXT("HALF-SBS")) ||
		UpperFilename.Contains(TEXT("HALF.SBS")))
	{
		return E3DVideoFormat::SBS_HalfWidth;
	}

	// Check for Full Side-by-Side
	if (UpperFilename.Contains(TEXT("FSBS")) ||
		UpperFilename.Contains(TEXT("F-SBS")) ||
		UpperFilename.Contains(TEXT("F.SBS")) ||
		UpperFilename.Contains(TEXT("FULLSBS")) ||
		UpperFilename.Contains(TEXT("FULL-SBS")) ||
		UpperFilename.Contains(TEXT("FULL.SBS")))
	{
		return E3DVideoFormat::SBS_FullWidth;
	}

	// Check for generic SBS (default to half-width as it's more common)
	if (UpperFilename.Contains(TEXT(".SBS.")) ||
		UpperFilename.Contains(TEXT("-SBS-")) ||
		UpperFilename.Contains(TEXT("_SBS_")) ||
		UpperFilename.Contains(TEXT("3DSBS")) ||
		UpperFilename.Contains(TEXT("3D-SBS")) ||
		UpperFilename.Contains(TEXT("3D.SBS")) ||
		UpperFilename.EndsWith(TEXT(".SBS")) ||
		UpperFilename.EndsWith(TEXT("-SBS")) ||
		UpperFilename.EndsWith(TEXT("_SBS")))
	{
		return E3DVideoFormat::SBS_HalfWidth;
	}

	// Check for Half Over-Under / Half Top-and-Bottom
	if (UpperFilename.Contains(TEXT("HOU")) ||
		UpperFilename.Contains(TEXT("H-OU")) ||
		UpperFilename.Contains(TEXT("H.OU")) ||
		UpperFilename.Contains(TEXT("HTAB")) ||
		UpperFilename.Contains(TEXT("H-TAB")) ||
		UpperFilename.Contains(TEXT("H.TAB")) ||
		UpperFilename.Contains(TEXT("HTB")) ||
		UpperFilename.Contains(TEXT("H-TB")) ||
		UpperFilename.Contains(TEXT("H.TB")) ||
		UpperFilename.Contains(TEXT("HALFOU")) ||
		UpperFilename.Contains(TEXT("HALF-OU")) ||
		UpperFilename.Contains(TEXT("HALFTAB")) ||
		UpperFilename.Contains(TEXT("HALF-TAB")))
	{
		return E3DVideoFormat::OU_HalfHeight;
	}

	// Check for Full Over-Under / Full Top-and-Bottom
	if (UpperFilename.Contains(TEXT("FOU")) ||
		UpperFilename.Contains(TEXT("F-OU")) ||
		UpperFilename.Contains(TEXT("F.OU")) ||
		UpperFilename.Contains(TEXT("FTAB")) ||
		UpperFilename.Contains(TEXT("F-TAB")) ||
		UpperFilename.Contains(TEXT("F.TAB")) ||
		UpperFilename.Contains(TEXT("FTB")) ||
		UpperFilename.Contains(TEXT("F-TB")) ||
		UpperFilename.Contains(TEXT("F.TB")) ||
		UpperFilename.Contains(TEXT("FULLOU")) ||
		UpperFilename.Contains(TEXT("FULL-OU")) ||
		UpperFilename.Contains(TEXT("FULLTAB")) ||
		UpperFilename.Contains(TEXT("FULL-TAB")))
	{
		return E3DVideoFormat::OU_FullHeight;
	}

	// Check for generic OU/TAB (default to half-height as it's more common)
	if (UpperFilename.Contains(TEXT(".OU.")) ||
		UpperFilename.Contains(TEXT("-OU-")) ||
		UpperFilename.Contains(TEXT("_OU_")) ||
		UpperFilename.Contains(TEXT(".TAB.")) ||
		UpperFilename.Contains(TEXT("-TAB-")) ||
		UpperFilename.Contains(TEXT("_TAB_")) ||
		UpperFilename.Contains(TEXT(".TB.")) ||
		UpperFilename.Contains(TEXT("-TB-")) ||
		UpperFilename.Contains(TEXT("_TB_")) ||
		UpperFilename.Contains(TEXT("3DOU")) ||
		UpperFilename.Contains(TEXT("3D-OU")) ||
		UpperFilename.Contains(TEXT("3DTAB")) ||
		UpperFilename.Contains(TEXT("3D-TAB")) ||
		UpperFilename.EndsWith(TEXT(".OU")) ||
		UpperFilename.EndsWith(TEXT("-OU")) ||
		UpperFilename.EndsWith(TEXT("_OU")) ||
		UpperFilename.EndsWith(TEXT(".TAB")) ||
		UpperFilename.EndsWith(TEXT("-TAB")) ||
		UpperFilename.EndsWith(TEXT("_TAB")) ||
		UpperFilename.EndsWith(TEXT(".TB")) ||
		UpperFilename.EndsWith(TEXT("-TB")) ||
		UpperFilename.EndsWith(TEXT("_TB")))
	{
		return E3DVideoFormat::OU_HalfHeight;
	}

	// No 3D format detected
	return E3DVideoFormat::None;
}

void UJellyfin3DVideoComponent::SetFormat(E3DVideoFormat NewFormat)
{
	if (CurrentFormat != NewFormat)
	{
		CurrentFormat = NewFormat;
		On3DFormatChanged.Broadcast(NewFormat);
		UE_LOG(LogJellyfinVR, Log, TEXT("3D video format changed to: %s"), *GetFormatDisplayName());
	}
}

E3DVideoFormat UJellyfin3DVideoComponent::ConvertJellyfinFormat(EJellyfin3DFormat JellyfinFormat) const
{
	switch (JellyfinFormat)
	{
	case EJellyfin3DFormat::SideBySide:
		return E3DVideoFormat::SBS_HalfWidth;

	case EJellyfin3DFormat::FullSideBySide:
		return E3DVideoFormat::SBS_FullWidth;

	case EJellyfin3DFormat::OverUnder:
		return E3DVideoFormat::OU_HalfHeight;

	case EJellyfin3DFormat::FullOverUnder:
		return E3DVideoFormat::OU_FullHeight;

	case EJellyfin3DFormat::None:
	default:
		return E3DVideoFormat::None;
	}
}

FBox2D UJellyfin3DVideoComponent::GetEyeUVRect(EStereoscopicEyeJellyfin Eye) const
{
	switch (CurrentFormat)
	{
	case E3DVideoFormat::SBS_FullWidth:
	case E3DVideoFormat::SBS_HalfWidth:
		// Side-by-Side: Left eye on left half, right eye on right half
		if (Eye == EStereoscopicEyeJellyfin::Left)
		{
			// Left half: UV (0, 0) to (0.5, 1)
			return FBox2D(FVector2D(0.0f, 0.0f), FVector2D(0.5f, 1.0f));
		}
		else
		{
			// Right half: UV (0.5, 0) to (1, 1)
			return FBox2D(FVector2D(0.5f, 0.0f), FVector2D(1.0f, 1.0f));
		}

	case E3DVideoFormat::OU_FullHeight:
	case E3DVideoFormat::OU_HalfHeight:
		// Over-Under: Left eye on top half, right eye on bottom half
		if (Eye == EStereoscopicEyeJellyfin::Left)
		{
			// Top half: UV (0, 0) to (1, 0.5)
			return FBox2D(FVector2D(0.0f, 0.0f), FVector2D(1.0f, 0.5f));
		}
		else
		{
			// Bottom half: UV (0, 0.5) to (1, 1)
			return FBox2D(FVector2D(0.0f, 0.5f), FVector2D(1.0f, 1.0f));
		}

	case E3DVideoFormat::None:
	default:
		// 2D video: Both eyes get full texture
		return FBox2D(FVector2D(0.0f, 0.0f), FVector2D(1.0f, 1.0f));
	}
}

FVector4 UJellyfin3DVideoComponent::GetEyeUVRectAsVector4(EStereoscopicEyeJellyfin Eye) const
{
	FBox2D UVRect = GetEyeUVRect(Eye);
	// Return as (MinU, MinV, MaxU, MaxV) for shader convenience
	return FVector4(UVRect.Min.X, UVRect.Min.Y, UVRect.Max.X, UVRect.Max.Y);
}

bool UJellyfin3DVideoComponent::IsSideBySide() const
{
	return CurrentFormat == E3DVideoFormat::SBS_FullWidth ||
	       CurrentFormat == E3DVideoFormat::SBS_HalfWidth;
}

bool UJellyfin3DVideoComponent::IsOverUnder() const
{
	return CurrentFormat == E3DVideoFormat::OU_FullHeight ||
	       CurrentFormat == E3DVideoFormat::OU_HalfHeight;
}

bool UJellyfin3DVideoComponent::IsFullResolution() const
{
	return CurrentFormat == E3DVideoFormat::SBS_FullWidth ||
	       CurrentFormat == E3DVideoFormat::OU_FullHeight;
}

FString UJellyfin3DVideoComponent::GetFormatDisplayName() const
{
	switch (CurrentFormat)
	{
	case E3DVideoFormat::SBS_FullWidth:
		return TEXT("Side-by-Side (Full Width)");

	case E3DVideoFormat::SBS_HalfWidth:
		return TEXT("Side-by-Side (Half Width)");

	case E3DVideoFormat::OU_FullHeight:
		return TEXT("Over-Under (Full Height)");

	case E3DVideoFormat::OU_HalfHeight:
		return TEXT("Over-Under (Half Height)");

	case E3DVideoFormat::None:
	default:
		return TEXT("2D (No Stereo)");
	}
}

float UJellyfin3DVideoComponent::GetAspectRatioMultiplier() const
{
	switch (CurrentFormat)
	{
	case E3DVideoFormat::SBS_FullWidth:
	case E3DVideoFormat::SBS_HalfWidth:
		// SBS videos are 2x wider than the intended viewing aspect ratio
		// Multiply by 0.5 to get correct aspect ratio for one eye
		return 0.5f;

	case E3DVideoFormat::OU_FullHeight:
	case E3DVideoFormat::OU_HalfHeight:
		// OU videos are 2x taller than the intended viewing aspect ratio
		// Multiply by 2.0 to get correct aspect ratio for one eye
		return 2.0f;

	case E3DVideoFormat::None:
	default:
		// No adjustment needed for 2D
		return 1.0f;
	}
}

void UJellyfin3DVideoComponent::ApplyToMaterial(UMaterialInstanceDynamic* Material)
{
	if (!Material)
	{
		UE_LOG(LogJellyfinVR, Warning, TEXT("Cannot apply 3D format to null material"));
		return;
	}

	// Set whether this is 3D video
	Material->SetScalarParameterValue(TEXT("Is3DVideo"), Is3DVideo() ? 1.0f : 0.0f);

	// Set format type as scalar (for any shader logic that needs it)
	Material->SetScalarParameterValue(TEXT("Format3D"), static_cast<float>(CurrentFormat));

	// Set UV rectangles for each eye
	FVector4 LeftUV = GetEyeUVRectAsVector4(EStereoscopicEyeJellyfin::Left);
	FVector4 RightUV = GetEyeUVRectAsVector4(EStereoscopicEyeJellyfin::Right);

	Material->SetVectorParameterValue(TEXT("LeftEyeUV"), FLinearColor(LeftUV.X, LeftUV.Y, LeftUV.Z, LeftUV.W));
	Material->SetVectorParameterValue(TEXT("RightEyeUV"), FLinearColor(RightUV.X, RightUV.Y, RightUV.Z, RightUV.W));

	UE_LOG(LogJellyfinVR, Verbose, TEXT("Applied 3D format to material: %s | Left UV: (%.2f,%.2f,%.2f,%.2f) | Right UV: (%.2f,%.2f,%.2f,%.2f)"),
		*GetFormatDisplayName(),
		LeftUV.X, LeftUV.Y, LeftUV.Z, LeftUV.W,
		RightUV.X, RightUV.Y, RightUV.Z, RightUV.W);
}
