// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class UTexture2D;
struct FSlateBrush;

/**
 * Card visual state
 */
enum class EJellyfinCardState : uint8
{
	Default,
	Hovered,
	Pressed,
	Focused
};

/**
 * Card type for layout variations
 */
enum class EJellyfinCardType : uint8
{
	Poster,      // 2:3 aspect ratio (movies, shows)
	Landscape,   // 16:9 aspect ratio (libraries, episodes)
	Square       // 1:1 aspect ratio (playlists)
};

/**
 * Animated card widget for JellyfinVR
 * Supports hover, press, and focus states with smooth animations
 */
class JELLYFINVR_API SJellyfinCard : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SJellyfinCard)
		: _CardType(EJellyfinCardType::Poster)
		, _Title()
		, _Subtitle()
		, _Progress(0.0f)
		, _ShowProgress(false)
		, _IsFolder(false)
		, _Width(150.0f)
	{}
		/** Type of card (affects aspect ratio) */
		SLATE_ARGUMENT(EJellyfinCardType, CardType)

		/** Primary title text */
		SLATE_ATTRIBUTE(FText, Title)

		/** Secondary subtitle text (year, runtime, etc.) */
		SLATE_ATTRIBUTE(FText, Subtitle)

		/** Watch progress (0.0 to 1.0) */
		SLATE_ATTRIBUTE(float, Progress)

		/** Whether to show progress bar */
		SLATE_ARGUMENT(bool, ShowProgress)

		/** Whether this card represents a folder/series */
		SLATE_ARGUMENT(bool, IsFolder)

		/** Card width in pixels */
		SLATE_ARGUMENT(float, Width)

		/** Called when card is clicked */
		SLATE_EVENT(FOnClicked, OnClicked)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	// ============ Image Management ============

	/**
	 * Set the poster/thumbnail image
	 */
	void SetImage(UTexture2D* Texture);

	/**
	 * Set image from brush (for dynamic updates)
	 */
	void SetImageBrush(const FSlateBrush* Brush);

	/**
	 * Check if image has been loaded
	 */
	bool HasImage() const { return bHasImage; }

	/**
	 * Show loading skeleton
	 */
	void ShowSkeleton();

	/**
	 * Hide skeleton (called automatically when image loads)
	 */
	void HideSkeleton();

	// ============ State Management ============

	/**
	 * Get current visual state
	 */
	EJellyfinCardState GetState() const { return CurrentState; }

	/**
	 * Set state directly (for keyboard/gamepad navigation)
	 */
	void SetState(EJellyfinCardState NewState);

	/**
	 * Update progress bar
	 */
	void SetProgress(float NewProgress);

	/**
	 * Update title
	 */
	void SetTitle(const FText& NewTitle);

	/**
	 * Update subtitle
	 */
	void SetSubtitle(const FText& NewSubtitle);

	// ============ SWidget Interface ============

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FCursorReply OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const override;
	virtual bool SupportsKeyboardFocus() const override { return true; }

protected:
	/**
	 * Get aspect ratio based on card type
	 */
	float GetAspectRatio() const;

	/**
	 * Update visual properties based on current animation values
	 */
	void UpdateVisuals();

	/**
	 * Start transition to new state
	 */
	void TransitionToState(EJellyfinCardState NewState);

	/**
	 * Get target scale for state
	 */
	float GetTargetScale(EJellyfinCardState State) const;

	/**
	 * Get target glow opacity for state
	 */
	float GetTargetGlowOpacity(EJellyfinCardState State) const;

private:
	// Configuration
	EJellyfinCardType CardType = EJellyfinCardType::Poster;
	float CardWidth = 150.0f;
	bool bShowProgress = false;
	bool bIsFolder = false;

	// State
	EJellyfinCardState CurrentState = EJellyfinCardState::Default;
	bool bHasImage = false;
	bool bShowingSkeleton = true;

	// Animation values (current)
	float CurrentScale = 1.0f;
	float CurrentGlowOpacity = 0.0f;
	float CurrentImageOpacity = 0.0f;
	float CurrentSkeletonOpacity = 1.0f;
	float CurrentProgress = 0.0f;

	// Animation IDs (for cancellation)
	uint32 ScaleAnimId = 0;
	uint32 GlowAnimId = 0;
	uint32 ImageFadeAnimId = 0;
	uint32 SkeletonFadeAnimId = 0;
	uint32 ShimmerAnimId = 0;

	// Shimmer animation position (0.0 to 1.0)
	float ShimmerPosition = 0.0f;

	// Content widgets
	TSharedPtr<SWidget> ImageWidget;
	TSharedPtr<SWidget> SkeletonWidget;
	TSharedPtr<SWidget> GlowBorderWidget;
	TSharedPtr<SWidget> ProgressBarWidget;
	TSharedPtr<SWidget> TitleWidget;
	TSharedPtr<SWidget> SubtitleWidget;
	TSharedPtr<SWidget> FolderBadgeWidget;

	// Image brush (owned)
	TSharedPtr<FSlateBrush> ImageBrush;

	// Callbacks
	FOnClicked OnClickedCallback;

	// Attributes
	TAttribute<FText> TitleAttr;
	TAttribute<FText> SubtitleAttr;
	TAttribute<float> ProgressAttr;
};
