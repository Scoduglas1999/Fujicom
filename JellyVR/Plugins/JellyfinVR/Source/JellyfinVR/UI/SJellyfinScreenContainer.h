// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

/**
 * Screen transition types
 */
enum class EJellyfinTransitionType : uint8
{
	None,           // Instant swap
	Fade,           // Fade out then fade in
	SlideLeft,      // Current slides left, new slides in from right
	SlideRight,     // Current slides right, new slides in from left
	ZoomIn,         // Zoom into new screen
	FadeToBlack     // Fade to black, then fade in
};

/**
 * Container widget that manages screen transitions
 * Handles smooth animated transitions between different UI screens
 */
class JELLYFINVR_API SJellyfinScreenContainer : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SJellyfinScreenContainer)
		: _DefaultTransition(EJellyfinTransitionType::Fade)
		, _TransitionDuration(0.3f)
	{}
		/** Default transition type */
		SLATE_ARGUMENT(EJellyfinTransitionType, DefaultTransition)

		/** Transition duration in seconds */
		SLATE_ARGUMENT(float, TransitionDuration)

		/** Initial content */
		SLATE_DEFAULT_SLOT(FArguments, Content)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	// ============ Navigation ============

	/**
	 * Transition to new content with default transition
	 */
	void TransitionTo(TSharedRef<SWidget> NewContent);

	/**
	 * Transition to new content with specific transition type
	 */
	void TransitionTo(TSharedRef<SWidget> NewContent, EJellyfinTransitionType TransitionType);

	/**
	 * Transition to new content with specific duration
	 */
	void TransitionTo(TSharedRef<SWidget> NewContent, EJellyfinTransitionType TransitionType, float Duration);

	/**
	 * Set content immediately without transition
	 */
	void SetContent(TSharedRef<SWidget> NewContent);

	/**
	 * Check if currently transitioning
	 */
	bool IsTransitioning() const { return bIsTransitioning; }

	/**
	 * Get current content
	 */
	TSharedPtr<SWidget> GetCurrentContent() const { return CurrentContent; }

	// ============ Configuration ============

	/**
	 * Set default transition type
	 */
	void SetDefaultTransition(EJellyfinTransitionType TransitionType) { DefaultTransitionType = TransitionType; }

	/**
	 * Set default transition duration
	 */
	void SetTransitionDuration(float Duration) { DefaultDuration = Duration; }

	// ============ SWidget Interface ============

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

protected:
	/**
	 * Execute fade transition
	 */
	void ExecuteFadeTransition(TSharedRef<SWidget> NewContent, float Duration);

	/**
	 * Execute slide transition
	 */
	void ExecuteSlideTransition(TSharedRef<SWidget> NewContent, bool bSlideLeft, float Duration);

	/**
	 * Execute zoom transition
	 */
	void ExecuteZoomTransition(TSharedRef<SWidget> NewContent, float Duration);

	/**
	 * Execute fade to black transition
	 */
	void ExecuteFadeToBlackTransition(TSharedRef<SWidget> NewContent, float Duration);

	/**
	 * Complete transition and cleanup
	 */
	void CompleteTransition();

private:
	// Configuration
	EJellyfinTransitionType DefaultTransitionType = EJellyfinTransitionType::Fade;
	float DefaultDuration = 0.3f;

	// State
	bool bIsTransitioning = false;

	// Content
	TSharedPtr<SWidget> CurrentContent;
	TSharedPtr<SWidget> NextContent;
	TSharedPtr<SWidget> BlackOverlay;

	// Animation values
	float CurrentContentOpacity = 1.0f;
	float CurrentContentOffsetX = 0.0f;
	float CurrentContentScale = 1.0f;

	float NextContentOpacity = 0.0f;
	float NextContentOffsetX = 0.0f;
	float NextContentScale = 1.0f;

	float BlackOverlayOpacity = 0.0f;

	// Animation IDs
	uint32 CurrentFadeAnimId = 0;
	uint32 CurrentSlideAnimId = 0;
	uint32 CurrentScaleAnimId = 0;
	uint32 NextFadeAnimId = 0;
	uint32 NextSlideAnimId = 0;
	uint32 NextScaleAnimId = 0;
	uint32 BlackFadeAnimId = 0;

	// Container widgets
	TSharedPtr<SWidget> CurrentContentContainer;
	TSharedPtr<SWidget> NextContentContainer;
};
