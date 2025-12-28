// Copyright JellyVR Project. All Rights Reserved.

#include "SJellyfinCard.h"
#include "JellyfinUIAnimator.h"
#include "JellyfinUIStyles.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Engine/Texture2D.h"

void SJellyfinCard::Construct(const FArguments& InArgs)
{
	CardType = InArgs._CardType;
	CardWidth = InArgs._Width;
	bShowProgress = InArgs._ShowProgress;
	bIsFolder = InArgs._IsFolder;
	OnClickedCallback = InArgs._OnClicked;
	TitleAttr = InArgs._Title;
	SubtitleAttr = InArgs._Subtitle;
	ProgressAttr = InArgs._Progress;

	float CardHeight = CardWidth * GetAspectRatio();

	// Start shimmer animation for skeleton
	ShimmerAnimId = FJellyfinUIAnimator::Get().AnimateLoop(
		0.0f, 1.0f,
		JellyfinAnimConstants::SkeletonShimmerDuration,
		EJellyfinEaseType::Linear,
		[this](float Value) { ShimmerPosition = Value; }
	);

	ChildSlot
	[
		// Outer container with scale transform
		SNew(SBox)
		.WidthOverride_Lambda([this]() { return CardWidth * CurrentScale; })
		[
			SNew(SVerticalBox)

			// Image area with overlays
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SBox)
				.HeightOverride(CardHeight)
				.WidthOverride(CardWidth)
				[
					SNew(SOverlay)

					// Skeleton loader (shown while loading)
					+ SOverlay::Slot()
					[
						SAssignNew(SkeletonWidget, SBorder)
						.BorderBackgroundColor_Lambda([this]()
						{
							// Shimmer gradient effect
							float shimmerCenter = ShimmerPosition;
							float shimmerWidth = 0.3f;

							// Base dark color
							FLinearColor BaseColor = JellyfinColors::CardBackground;
							FLinearColor HighlightColor = JellyfinColors::CardHover;

							// We'd ideally use a material here, but for Slate we simulate
							// by using the shimmer position to modulate brightness
							float brightness = FMath::Clamp(
								1.0f - FMath::Abs(0.5f - ShimmerPosition) * 2.0f,
								0.0f, 0.3f
							);

							return FLinearColor::LerpUsingHSV(BaseColor, HighlightColor, brightness);
						})
						.ColorAndOpacity_Lambda([this]()
						{
							return FLinearColor(1, 1, 1, CurrentSkeletonOpacity);
						})
						.Padding(0)
						[
							SNew(SBox)
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
						]
					]

					// Actual image
					+ SOverlay::Slot()
					[
						SAssignNew(ImageWidget, SBorder)
						.BorderBackgroundColor(FLinearColor::Transparent)
						.ColorAndOpacity_Lambda([this]()
						{
							return FLinearColor(1, 1, 1, CurrentImageOpacity);
						})
						.Padding(0)
						[
							SNew(SImage)
							.Image_Lambda([this]() -> const FSlateBrush*
							{
								return ImageBrush.IsValid() ? ImageBrush.Get() : nullptr;
							})
						]
					]

					// Glow border (visible on hover/focus)
					+ SOverlay::Slot()
					[
						SAssignNew(GlowBorderWidget, SBorder)
						.BorderImage(FCoreStyle::Get().GetBrush("Border"))
						.BorderBackgroundColor_Lambda([this]()
						{
							FLinearColor GlowColor = JellyfinColors::Primary;
							GlowColor.A = CurrentGlowOpacity;
							return GlowColor;
						})
						.Padding(2)
						[
							SNew(SBorder)
							.BorderBackgroundColor(FLinearColor::Transparent)
						]
					]

					// Progress bar (at bottom)
					+ SOverlay::Slot()
					.VAlign(VAlign_Bottom)
					[
						SNew(SBox)
						.HeightOverride(4.0f)
						.Visibility_Lambda([this]()
						{
							return bShowProgress && CurrentProgress > 0.0f
								? EVisibility::Visible
								: EVisibility::Collapsed;
						})
						[
							SNew(SOverlay)
							// Background track
							+ SOverlay::Slot()
							[
								SNew(SBorder)
								.BorderBackgroundColor(JellyfinColors::ProgressBackground)
							]
							// Fill
							+ SOverlay::Slot()
							.HAlign(HAlign_Left)
							[
								SNew(SBox)
								.WidthOverride_Lambda([this]() { return CardWidth * CurrentProgress; })
								[
									SNew(SBorder)
									.BorderBackgroundColor(JellyfinColors::Primary)
								]
							]
						]
					]

					// Folder badge (top right corner)
					+ SOverlay::Slot()
					.HAlign(HAlign_Right)
					.VAlign(VAlign_Top)
					.Padding(4)
					[
						SNew(SBorder)
						.BorderBackgroundColor(JellyfinColors::CardHover)
						.Padding(FMargin(6, 2))
						.Visibility_Lambda([this]()
						{
							return bIsFolder ? EVisibility::Visible : EVisibility::Collapsed;
						})
						[
							SNew(STextBlock)
							.Text(FText::FromString(TEXT(">")))
							.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
							.ColorAndOpacity(JellyfinColors::Text)
						]
					]
				]
			]

			// Title
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 8, 0, 0)
			[
				SNew(SBox)
				.WidthOverride(CardWidth)
				[
					SAssignNew(TitleWidget, STextBlock)
					.Text_Lambda([this]() { return TitleAttr.Get(); })
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 14))
					.ColorAndOpacity(JellyfinColors::Text)
					.AutoWrapText(false)
					.OverflowPolicy(ETextOverflowPolicy::Ellipsis)
				]
			]

			// Subtitle
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 2, 0, 0)
			[
				SNew(SBox)
				.WidthOverride(CardWidth)
				.Visibility_Lambda([this]()
				{
					return SubtitleAttr.Get().IsEmpty()
						? EVisibility::Collapsed
						: EVisibility::Visible;
				})
				[
					SAssignNew(SubtitleWidget, STextBlock)
					.Text_Lambda([this]() { return SubtitleAttr.Get(); })
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
					.ColorAndOpacity(JellyfinColors::TextSecondary)
					.AutoWrapText(false)
					.OverflowPolicy(ETextOverflowPolicy::Ellipsis)
				]
			]
		]
	];
}

void SJellyfinCard::SetImage(UTexture2D* Texture)
{
	if (!Texture)
	{
		return;
	}

	if (!ImageBrush.IsValid())
	{
		ImageBrush = MakeShareable(new FSlateBrush());
	}

	ImageBrush->SetResourceObject(Texture);
	ImageBrush->ImageSize = FVector2D(Texture->GetSizeX(), Texture->GetSizeY());
	ImageBrush->DrawAs = ESlateBrushDrawType::Image;

	bHasImage = true;

	// Animate image fade in
	FJellyfinUIAnimator::Get().Cancel(ImageFadeAnimId);
	ImageFadeAnimId = FJellyfinUIAnimator::Get().Animate(
		CurrentImageOpacity, 1.0f,
		JellyfinAnimConstants::ImageFadeInDuration,
		EJellyfinEaseType::EaseOut,
		[this](float Value) { CurrentImageOpacity = Value; }
	);

	// Fade out skeleton
	HideSkeleton();
}

void SJellyfinCard::SetImageBrush(const FSlateBrush* Brush)
{
	if (!Brush)
	{
		return;
	}

	if (!ImageBrush.IsValid())
	{
		ImageBrush = MakeShareable(new FSlateBrush(*Brush));
	}
	else
	{
		*ImageBrush = *Brush;
	}

	bHasImage = true;

	// Animate image fade in
	FJellyfinUIAnimator::Get().Cancel(ImageFadeAnimId);
	ImageFadeAnimId = FJellyfinUIAnimator::Get().Animate(
		CurrentImageOpacity, 1.0f,
		JellyfinAnimConstants::ImageFadeInDuration,
		EJellyfinEaseType::EaseOut,
		[this](float Value) { CurrentImageOpacity = Value; }
	);

	HideSkeleton();
}

void SJellyfinCard::ShowSkeleton()
{
	bShowingSkeleton = true;
	CurrentSkeletonOpacity = 1.0f;

	// Resume shimmer if stopped
	if (!FJellyfinUIAnimator::Get().IsAnimating(ShimmerAnimId))
	{
		ShimmerAnimId = FJellyfinUIAnimator::Get().AnimateLoop(
			0.0f, 1.0f,
			JellyfinAnimConstants::SkeletonShimmerDuration,
			EJellyfinEaseType::Linear,
			[this](float Value) { ShimmerPosition = Value; }
		);
	}
}

void SJellyfinCard::HideSkeleton()
{
	if (!bShowingSkeleton)
	{
		return;
	}

	bShowingSkeleton = false;

	// Fade out skeleton
	FJellyfinUIAnimator::Get().Cancel(SkeletonFadeAnimId);
	SkeletonFadeAnimId = FJellyfinUIAnimator::Get().Animate(
		CurrentSkeletonOpacity, 0.0f,
		0.15f,
		EJellyfinEaseType::EaseOut,
		[this](float Value) { CurrentSkeletonOpacity = Value; },
		[this]()
		{
			// Stop shimmer when skeleton is hidden
			FJellyfinUIAnimator::Get().Cancel(ShimmerAnimId);
		}
	);
}

void SJellyfinCard::SetState(EJellyfinCardState NewState)
{
	if (CurrentState != NewState)
	{
		TransitionToState(NewState);
	}
}

void SJellyfinCard::SetProgress(float NewProgress)
{
	CurrentProgress = FMath::Clamp(NewProgress, 0.0f, 1.0f);
}

void SJellyfinCard::SetTitle(const FText& NewTitle)
{
	TitleAttr = NewTitle;
}

void SJellyfinCard::SetSubtitle(const FText& NewSubtitle)
{
	SubtitleAttr = NewSubtitle;
}

void SJellyfinCard::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	// Update progress from attribute if bound
	if (ProgressAttr.IsBound())
	{
		CurrentProgress = ProgressAttr.Get();
	}
}

void SJellyfinCard::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	SCompoundWidget::OnMouseEnter(MyGeometry, MouseEvent);
	TransitionToState(EJellyfinCardState::Hovered);
}

void SJellyfinCard::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	TransitionToState(EJellyfinCardState::Default);
}

FReply SJellyfinCard::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		TransitionToState(EJellyfinCardState::Pressed);
		return FReply::Handled().CaptureMouse(AsShared());
	}
	return FReply::Unhandled();
}

FReply SJellyfinCard::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && HasMouseCapture())
	{
		// Check if still hovering
		bool bIsHovering = MyGeometry.IsUnderLocation(MouseEvent.GetScreenSpacePosition());

		if (bIsHovering)
		{
			TransitionToState(EJellyfinCardState::Hovered);

			// Fire click callback
			if (OnClickedCallback.IsBound())
			{
				OnClickedCallback.Execute();
			}
		}
		else
		{
			TransitionToState(EJellyfinCardState::Default);
		}

		return FReply::Handled().ReleaseMouseCapture();
	}
	return FReply::Unhandled();
}

FCursorReply SJellyfinCard::OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const
{
	return FCursorReply::Cursor(EMouseCursor::Hand);
}

float SJellyfinCard::GetAspectRatio() const
{
	switch (CardType)
	{
	case EJellyfinCardType::Poster:
		return 1.5f; // 2:3 aspect ratio (height = 1.5 * width)

	case EJellyfinCardType::Landscape:
		return 0.5625f; // 16:9 aspect ratio (height = 0.5625 * width)

	case EJellyfinCardType::Square:
		return 1.0f;

	default:
		return 1.5f;
	}
}

void SJellyfinCard::UpdateVisuals()
{
	// Visuals are updated via lambdas bound in Construct
}

void SJellyfinCard::TransitionToState(EJellyfinCardState NewState)
{
	CurrentState = NewState;

	float TargetScale = GetTargetScale(NewState);
	float TargetGlow = GetTargetGlowOpacity(NewState);

	// Cancel any running animations
	FJellyfinUIAnimator::Get().Cancel(ScaleAnimId);
	FJellyfinUIAnimator::Get().Cancel(GlowAnimId);

	// Animate scale
	ScaleAnimId = FJellyfinUIAnimator::Get().Animate(
		CurrentScale, TargetScale,
		JellyfinAnimConstants::HoverDuration,
		NewState == EJellyfinCardState::Pressed ? EJellyfinEaseType::EaseOut : EJellyfinEaseType::EaseOutBack,
		[this](float Value) { CurrentScale = Value; }
	);

	// Animate glow
	GlowAnimId = FJellyfinUIAnimator::Get().Animate(
		CurrentGlowOpacity, TargetGlow,
		JellyfinAnimConstants::FocusGlowDuration,
		EJellyfinEaseType::EaseOut,
		[this](float Value) { CurrentGlowOpacity = Value; }
	);
}

float SJellyfinCard::GetTargetScale(EJellyfinCardState State) const
{
	switch (State)
	{
	case EJellyfinCardState::Default:
		return JellyfinAnimConstants::DefaultScale;

	case EJellyfinCardState::Hovered:
		return JellyfinAnimConstants::HoverScale;

	case EJellyfinCardState::Pressed:
		return JellyfinAnimConstants::PressScale;

	case EJellyfinCardState::Focused:
		return JellyfinAnimConstants::FocusScale;

	default:
		return JellyfinAnimConstants::DefaultScale;
	}
}

float SJellyfinCard::GetTargetGlowOpacity(EJellyfinCardState State) const
{
	switch (State)
	{
	case EJellyfinCardState::Default:
		return 0.0f;

	case EJellyfinCardState::Hovered:
		return 0.8f;

	case EJellyfinCardState::Pressed:
		return 1.0f;

	case EJellyfinCardState::Focused:
		return 0.5f;

	default:
		return 0.0f;
	}
}
