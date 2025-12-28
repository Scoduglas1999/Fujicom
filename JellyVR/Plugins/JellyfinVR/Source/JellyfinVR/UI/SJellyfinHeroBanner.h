// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "API/JellyfinTypes.h"

class UTexture2D;
struct FSlateBrush;

/**
 * Hero banner for featured content at top of home screen
 * Netflix-style large backdrop with title, metadata, and action buttons
 */
class JELLYFINVR_API SJellyfinHeroBanner : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SJellyfinHeroBanner)
		: _Height(400.0f)
		, _AutoRotate(true)
		, _RotateInterval(8.0f)
	{}
		/** Banner height in pixels */
		SLATE_ARGUMENT(float, Height)

		/** Whether to auto-rotate through featured items */
		SLATE_ARGUMENT(bool, AutoRotate)

		/** Time between rotations (seconds) */
		SLATE_ARGUMENT(float, RotateInterval)

		/** Called when Play/Resume button clicked */
		SLATE_EVENT(FSimpleDelegate, OnPlayClicked)

		/** Called when More Info button clicked */
		SLATE_EVENT(FSimpleDelegate, OnInfoClicked)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	// ============ Content Management ============

	/**
	 * Set featured items to display
	 */
	void SetFeaturedItems(const TArray<FJellyfinMediaItem>& Items);

	/**
	 * Set single featured item
	 */
	void SetFeaturedItem(const FJellyfinMediaItem& Item);

	/**
	 * Get current displayed item
	 */
	const FJellyfinMediaItem& GetCurrentItem() const { return CurrentItem; }

	/**
	 * Set backdrop image
	 */
	void SetBackdropImage(UTexture2D* Texture);

	/**
	 * Navigate to next featured item
	 */
	void NextItem();

	/**
	 * Navigate to previous featured item
	 */
	void PreviousItem();

	/**
	 * Jump to specific item index
	 */
	void GoToItem(int32 Index);

	// ============ Animation Control ============

	/**
	 * Start Ken Burns effect
	 */
	void StartKenBurns();

	/**
	 * Stop Ken Burns effect
	 */
	void StopKenBurns();

	/**
	 * Trigger content fade-in animation
	 */
	void AnimateContentIn();

	// ============ SWidget Interface ============

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

protected:
	/**
	 * Build the info panel content
	 */
	TSharedRef<SWidget> BuildInfoPanel();

	/**
	 * Build the action buttons
	 */
	TSharedRef<SWidget> BuildActionButtons();

	/**
	 * Build page indicators
	 */
	TSharedRef<SWidget> BuildPageIndicators();

	/**
	 * Update displayed content for current item
	 */
	void UpdateDisplayedContent();

	/**
	 * Transition to new item with animation
	 */
	void TransitionToItem(int32 NewIndex);

	/**
	 * Format runtime from ticks to readable string
	 */
	FString FormatRuntime(int64 RuntimeTicks) const;

	/**
	 * Get label text (CONTINUE WATCHING, NEW, etc.)
	 */
	FText GetLabelText() const;

private:
	// Configuration
	float BannerHeight = 400.0f;
	bool bAutoRotate = true;
	float RotateInterval = 8.0f;

	// Content
	TArray<FJellyfinMediaItem> FeaturedItems;
	FJellyfinMediaItem CurrentItem;
	int32 CurrentItemIndex = 0;

	// State
	float TimeSinceLastRotate = 0.0f;
	bool bIsTransitioning = false;

	// Ken Burns animation
	float KenBurnsScale = 1.0f;
	float KenBurnsOffsetX = 0.0f;
	float KenBurnsOffsetY = 0.0f;
	uint32 KenBurnsAnimId = 0;
	bool bKenBurnsActive = false;

	// Content fade animation
	float TitleOpacity = 0.0f;
	float MetadataOpacity = 0.0f;
	float DescriptionOpacity = 0.0f;
	float ButtonsOpacity = 0.0f;
	float BackdropOpacity = 0.0f;

	uint32 TitleFadeAnimId = 0;
	uint32 MetadataFadeAnimId = 0;
	uint32 DescriptionFadeAnimId = 0;
	uint32 ButtonsFadeAnimId = 0;
	uint32 BackdropFadeAnimId = 0;

	// Widgets
	TSharedPtr<SWidget> BackdropWidget;
	TSharedPtr<SWidget> GradientOverlayWidget;
	TSharedPtr<SWidget> InfoPanelWidget;
	TSharedPtr<SWidget> PageIndicatorsWidget;

	// Backdrop brush
	TSharedPtr<FSlateBrush> BackdropBrush;

	// Callbacks
	FSimpleDelegate OnPlayClickedCallback;
	FSimpleDelegate OnInfoClickedCallback;
};
