// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Streaming-style color palette for JellyfinVR UI
 * Netflix-inspired dark theme with Jellyfin blue accent
 */
namespace JellyfinColors
{
	// Backgrounds
	inline const FLinearColor Background = FLinearColor(0.039f, 0.039f, 0.047f, 1.0f);       // #0A0A0C
	inline const FLinearColor CardBackground = FLinearColor(0.086f, 0.086f, 0.102f, 1.0f);   // #16161A
	inline const FLinearColor CardHover = FLinearColor(0.118f, 0.118f, 0.141f, 1.0f);        // #1E1E24

	// Accent colors
	inline const FLinearColor Primary = FLinearColor(0.0f, 0.643f, 0.863f, 1.0f);            // #00A4DC Jellyfin blue
	inline const FLinearColor PrimaryGlow = FLinearColor(0.0f, 0.643f, 0.863f, 0.6f);        // For focus glow
	inline const FLinearColor Success = FLinearColor(0.29f, 0.87f, 0.5f, 1.0f);              // #4ADE80
	inline const FLinearColor Error = FLinearColor(0.973f, 0.443f, 0.443f, 1.0f);            // #F87171

	// Text
	inline const FLinearColor Text = FLinearColor(1.0f, 1.0f, 1.0f, 0.95f);                  // White 95%
	inline const FLinearColor TextSecondary = FLinearColor(1.0f, 1.0f, 1.0f, 0.6f);          // White 60%
	inline const FLinearColor TextDisabled = FLinearColor(1.0f, 1.0f, 1.0f, 0.3f);           // White 30%

	// UI elements
	inline const FLinearColor InputBackground = FLinearColor(0.039f, 0.039f, 0.047f, 1.0f);  // Same as main BG
	inline const FLinearColor InputBorder = FLinearColor(1.0f, 1.0f, 1.0f, 0.1f);            // White 10%
	inline const FLinearColor ProgressBackground = FLinearColor(1.0f, 1.0f, 1.0f, 0.2f);     // White 20%
	inline const FLinearColor CardBorder = FLinearColor(1.0f, 1.0f, 1.0f, 0.05f);            // White 5%
}

/**
 * Layout dimensions for JellyfinVR UI
 */
namespace JellyfinLayout
{
	// Poster cards (2:3 aspect ratio)
	inline constexpr float PosterWidth = 150.0f;
	inline constexpr float PosterHeight = 225.0f;
	inline constexpr float PosterRadius = 8.0f;

	// Library cards (landscape)
	inline constexpr float LibraryCardWidth = 200.0f;
	inline constexpr float LibraryCardHeight = 120.0f;

	// Spacing
	inline constexpr float CardGap = 16.0f;
	inline constexpr float RowPadding = 40.0f;
	inline constexpr float RowGap = 32.0f;

	// Focus effects
	inline constexpr float FocusScale = 1.08f;

	// Login card
	inline constexpr float LoginCardWidth = 400.0f;
	inline constexpr float LoginCardPadding = 48.0f;
	inline constexpr float LoginCardRadius = 16.0f;

	// Input fields
	inline constexpr float InputHeight = 48.0f;
	inline constexpr float InputRadius = 8.0f;

	// Buttons
	inline constexpr float ButtonHeight = 48.0f;
	inline constexpr float ButtonRadius = 8.0f;

	// Hero banner
	inline constexpr float HeroBannerHeight = 400.0f;
	inline constexpr float HeroContentMaxWidth = 500.0f;
}
