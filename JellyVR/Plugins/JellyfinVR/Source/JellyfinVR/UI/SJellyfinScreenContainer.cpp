// Copyright JellyVR Project. All Rights Reserved.

#include "SJellyfinScreenContainer.h"
#include "JellyfinUIAnimator.h"
#include "JellyfinUIStyles.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SNullWidget.h"

void SJellyfinScreenContainer::Construct(const FArguments& InArgs)
{
	DefaultTransitionType = InArgs._DefaultTransition;
	DefaultDuration = InArgs._TransitionDuration;

	// Create black overlay for fade-to-black transitions
	BlackOverlay = SNew(SBorder)
		.BorderBackgroundColor(FLinearColor::Black)
		.Visibility(EVisibility::HitTestInvisible);

	// Set initial content if provided
	if (InArgs._Content.Widget != SNullWidget::NullWidget)
	{
		CurrentContent = InArgs._Content.Widget;
	}

	ChildSlot
	[
		SNew(SOverlay)

		// Current content
		+ SOverlay::Slot()
		[
			SNew(SBox)
			.RenderOpacity_Lambda([this]() { return CurrentContentOpacity; })
			.RenderTransform_Lambda([this]()
			{
				return FSlateRenderTransform(
					FScale2D(CurrentContentScale),
					FVector2D(CurrentContentOffsetX, 0)
				);
			})
			.RenderTransformPivot(FVector2D(0.5f, 0.5f))
			[
				SAssignNew(CurrentContentContainer, SBorder)
				.BorderBackgroundColor(FLinearColor::Transparent)
				.Padding(0)
				[
					CurrentContent.IsValid() ? CurrentContent.ToSharedRef() : SNullWidget::NullWidget
				]
			]
		]

		// Next content (during transitions)
		+ SOverlay::Slot()
		[
			SNew(SBox)
			.RenderOpacity_Lambda([this]() { return NextContentOpacity; })
			.RenderTransform_Lambda([this]()
			{
				return FSlateRenderTransform(
					FScale2D(NextContentScale),
					FVector2D(NextContentOffsetX, 0)
				);
			})
			.RenderTransformPivot(FVector2D(0.5f, 0.5f))
			[
				SAssignNew(NextContentContainer, SBorder)
				.BorderBackgroundColor(FLinearColor::Transparent)
				.Padding(0)
				.Visibility_Lambda([this]()
				{
					return bIsTransitioning ? EVisibility::Visible : EVisibility::Collapsed;
				})
				[
					NextContent.IsValid() ? NextContent.ToSharedRef() : SNullWidget::NullWidget
				]
			]
		]

		// Black overlay for fade-to-black
		+ SOverlay::Slot()
		[
			SNew(SBorder)
			.BorderBackgroundColor_Lambda([this]()
			{
				return FLinearColor(0, 0, 0, BlackOverlayOpacity);
			})
			.Visibility_Lambda([this]()
			{
				return BlackOverlayOpacity > 0.0f ? EVisibility::HitTestInvisible : EVisibility::Collapsed;
			})
		]
	];
}

void SJellyfinScreenContainer::TransitionTo(TSharedRef<SWidget> NewContent)
{
	TransitionTo(NewContent, DefaultTransitionType, DefaultDuration);
}

void SJellyfinScreenContainer::TransitionTo(TSharedRef<SWidget> NewContent, EJellyfinTransitionType TransitionType)
{
	TransitionTo(NewContent, TransitionType, DefaultDuration);
}

void SJellyfinScreenContainer::TransitionTo(TSharedRef<SWidget> NewContent, EJellyfinTransitionType TransitionType, float Duration)
{
	// If already transitioning, queue or ignore
	if (bIsTransitioning)
	{
		return;
	}

	// If same content, ignore
	if (CurrentContent == NewContent)
	{
		return;
	}

	// Instant swap
	if (TransitionType == EJellyfinTransitionType::None)
	{
		SetContent(NewContent);
		return;
	}

	bIsTransitioning = true;
	NextContent = NewContent;

	// Update next content container
	if (NextContentContainer.IsValid())
	{
		TSharedPtr<SBorder> Border = StaticCastSharedPtr<SBorder>(NextContentContainer);
		if (Border.IsValid())
		{
			Border->SetContent(NewContent);
		}
	}

	// Execute appropriate transition
	switch (TransitionType)
	{
	case EJellyfinTransitionType::Fade:
		ExecuteFadeTransition(NewContent, Duration);
		break;

	case EJellyfinTransitionType::SlideLeft:
		ExecuteSlideTransition(NewContent, true, Duration);
		break;

	case EJellyfinTransitionType::SlideRight:
		ExecuteSlideTransition(NewContent, false, Duration);
		break;

	case EJellyfinTransitionType::ZoomIn:
		ExecuteZoomTransition(NewContent, Duration);
		break;

	case EJellyfinTransitionType::FadeToBlack:
		ExecuteFadeToBlackTransition(NewContent, Duration);
		break;

	default:
		SetContent(NewContent);
		break;
	}
}

void SJellyfinScreenContainer::SetContent(TSharedRef<SWidget> NewContent)
{
	// Cancel any running animations
	FJellyfinUIAnimator::Get().Cancel(CurrentFadeAnimId);
	FJellyfinUIAnimator::Get().Cancel(CurrentSlideAnimId);
	FJellyfinUIAnimator::Get().Cancel(CurrentScaleAnimId);
	FJellyfinUIAnimator::Get().Cancel(NextFadeAnimId);
	FJellyfinUIAnimator::Get().Cancel(NextSlideAnimId);
	FJellyfinUIAnimator::Get().Cancel(NextScaleAnimId);
	FJellyfinUIAnimator::Get().Cancel(BlackFadeAnimId);

	// Reset animation values
	CurrentContentOpacity = 1.0f;
	CurrentContentOffsetX = 0.0f;
	CurrentContentScale = 1.0f;
	NextContentOpacity = 0.0f;
	NextContentOffsetX = 0.0f;
	NextContentScale = 1.0f;
	BlackOverlayOpacity = 0.0f;

	// Set content
	CurrentContent = NewContent;
	NextContent = nullptr;
	bIsTransitioning = false;

	// Update container
	if (CurrentContentContainer.IsValid())
	{
		TSharedPtr<SBorder> Border = StaticCastSharedPtr<SBorder>(CurrentContentContainer);
		if (Border.IsValid())
		{
			Border->SetContent(NewContent);
		}
	}
}

void SJellyfinScreenContainer::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

void SJellyfinScreenContainer::ExecuteFadeTransition(TSharedRef<SWidget> NewContent, float Duration)
{
	float HalfDuration = Duration * 0.4f; // Faster fade out

	// Reset next content state
	NextContentOpacity = 0.0f;
	NextContentScale = 0.98f; // Slight scale for subtle effect

	// Fade out current
	CurrentFadeAnimId = FJellyfinUIAnimator::Get().Animate(
		1.0f, 0.0f, HalfDuration, EJellyfinEaseType::EaseOut,
		[this](float V) { CurrentContentOpacity = V; }
	);

	// Fade in next with slight delay
	NextFadeAnimId = FJellyfinUIAnimator::Get().AnimateDelayed(
		HalfDuration * 0.5f,
		0.0f, 1.0f, Duration * 0.6f, EJellyfinEaseType::EaseOut,
		[this](float V) { NextContentOpacity = V; },
		[this]() { CompleteTransition(); }
	);

	// Scale animation for next content
	NextScaleAnimId = FJellyfinUIAnimator::Get().AnimateDelayed(
		HalfDuration * 0.5f,
		0.98f, 1.0f, Duration * 0.6f, EJellyfinEaseType::EaseOut,
		[this](float V) { NextContentScale = V; }
	);
}

void SJellyfinScreenContainer::ExecuteSlideTransition(TSharedRef<SWidget> NewContent, bool bSlideLeft, float Duration)
{
	float SlideDistance = 400.0f; // Pixels to slide
	float Direction = bSlideLeft ? -1.0f : 1.0f;

	// Next content starts offscreen
	NextContentOpacity = 1.0f;
	NextContentOffsetX = -Direction * SlideDistance;

	// Slide current out
	CurrentSlideAnimId = FJellyfinUIAnimator::Get().Animate(
		0.0f, Direction * SlideDistance, Duration, EJellyfinEaseType::EaseInOut,
		[this](float V) { CurrentContentOffsetX = V; }
	);

	// Fade current out
	CurrentFadeAnimId = FJellyfinUIAnimator::Get().Animate(
		1.0f, 0.0f, Duration, EJellyfinEaseType::EaseOut,
		[this](float V) { CurrentContentOpacity = V; }
	);

	// Slide next in
	NextSlideAnimId = FJellyfinUIAnimator::Get().Animate(
		-Direction * SlideDistance, 0.0f, Duration, EJellyfinEaseType::EaseInOut,
		[this](float V) { NextContentOffsetX = V; },
		[this]() { CompleteTransition(); }
	);
}

void SJellyfinScreenContainer::ExecuteZoomTransition(TSharedRef<SWidget> NewContent, float Duration)
{
	// Next content starts slightly zoomed out
	NextContentOpacity = 0.0f;
	NextContentScale = 0.9f;

	// Fade out and scale down current
	CurrentFadeAnimId = FJellyfinUIAnimator::Get().Animate(
		1.0f, 0.0f, Duration * 0.4f, EJellyfinEaseType::EaseOut,
		[this](float V) { CurrentContentOpacity = V; }
	);

	CurrentScaleAnimId = FJellyfinUIAnimator::Get().Animate(
		1.0f, 1.1f, Duration * 0.4f, EJellyfinEaseType::EaseOut,
		[this](float V) { CurrentContentScale = V; }
	);

	// Fade in and scale up next
	NextFadeAnimId = FJellyfinUIAnimator::Get().AnimateDelayed(
		Duration * 0.3f,
		0.0f, 1.0f, Duration * 0.7f, EJellyfinEaseType::EaseOut,
		[this](float V) { NextContentOpacity = V; },
		[this]() { CompleteTransition(); }
	);

	NextScaleAnimId = FJellyfinUIAnimator::Get().AnimateDelayed(
		Duration * 0.3f,
		0.9f, 1.0f, Duration * 0.7f, EJellyfinEaseType::EaseOutBack,
		[this](float V) { NextContentScale = V; }
	);
}

void SJellyfinScreenContainer::ExecuteFadeToBlackTransition(TSharedRef<SWidget> NewContent, float Duration)
{
	float FadeOutDuration = Duration * 0.4f;
	float FadeInDuration = Duration * 0.6f;

	// Fade to black
	BlackFadeAnimId = FJellyfinUIAnimator::Get().Animate(
		0.0f, 1.0f, FadeOutDuration, EJellyfinEaseType::EaseOut,
		[this](float V) { BlackOverlayOpacity = V; },
		[this, FadeInDuration]()
		{
			// At full black, swap content
			CurrentContentOpacity = 0.0f;
			NextContentOpacity = 1.0f;

			// Fade from black
			BlackFadeAnimId = FJellyfinUIAnimator::Get().Animate(
				1.0f, 0.0f, FadeInDuration, EJellyfinEaseType::EaseOut,
				[this](float V) { BlackOverlayOpacity = V; },
				[this]() { CompleteTransition(); }
			);
		}
	);
}

void SJellyfinScreenContainer::CompleteTransition()
{
	// Move next content to current
	CurrentContent = NextContent;
	NextContent = nullptr;

	// Update container
	if (CurrentContentContainer.IsValid())
	{
		TSharedPtr<SBorder> Border = StaticCastSharedPtr<SBorder>(CurrentContentContainer);
		if (Border.IsValid() && CurrentContent.IsValid())
		{
			Border->SetContent(CurrentContent.ToSharedRef());
		}
	}

	// Reset animation values
	CurrentContentOpacity = 1.0f;
	CurrentContentOffsetX = 0.0f;
	CurrentContentScale = 1.0f;
	NextContentOpacity = 0.0f;
	NextContentOffsetX = 0.0f;
	NextContentScale = 1.0f;
	BlackOverlayOpacity = 0.0f;

	bIsTransitioning = false;
}
