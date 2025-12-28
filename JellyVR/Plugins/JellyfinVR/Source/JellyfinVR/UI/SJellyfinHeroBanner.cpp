// Copyright JellyVR Project. All Rights Reserved.

#include "SJellyfinHeroBanner.h"
#include "JellyfinUIAnimator.h"
#include "JellyfinUIStyles.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SBoxPanel.h"
#include "Engine/Texture2D.h"

void SJellyfinHeroBanner::Construct(const FArguments& InArgs)
{
	BannerHeight = InArgs._Height;
	bAutoRotate = InArgs._AutoRotate;
	RotateInterval = InArgs._RotateInterval;
	OnPlayClickedCallback = InArgs._OnPlayClicked;
	OnInfoClickedCallback = InArgs._OnInfoClicked;

	ChildSlot
	[
		SNew(SBox)
		.HeightOverride(BannerHeight)
		[
			SNew(SOverlay)

			// Backdrop image with Ken Burns
			+ SOverlay::Slot()
			[
				SAssignNew(BackdropWidget, SBox)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SScaleBox)
					.Stretch(EStretch::ScaleToFill)
					[
						SNew(SImage)
						.Image_Lambda([this]() -> const FSlateBrush*
						{
							return BackdropBrush.IsValid() ? BackdropBrush.Get() : nullptr;
						})
						.ColorAndOpacity_Lambda([this]()
						{
							return FLinearColor(1, 1, 1, BackdropOpacity);
						})
						// Ken Burns transform would be applied via RenderTransform
						// For now, we'll use opacity as the primary animation
					]
				]
			]

			// Gradient overlay (transparent top to solid bottom)
			+ SOverlay::Slot()
			[
				SAssignNew(GradientOverlayWidget, SBorder)
				.BorderBackgroundColor(FLinearColor::Transparent)
				.Padding(0)
				[
					SNew(SVerticalBox)

					// Top half - transparent
					+ SVerticalBox::Slot()
					.FillHeight(0.4f)
					[
						SNew(SBorder)
						.BorderBackgroundColor(FLinearColor(0, 0, 0, 0))
					]

					// Bottom half - gradient to solid
					+ SVerticalBox::Slot()
					.FillHeight(0.6f)
					[
						SNew(SBorder)
						.BorderBackgroundColor(FLinearColor(0.039f, 0.039f, 0.047f, 0.95f))
					]
				]
			]

			// Content overlay
			+ SOverlay::Slot()
			.VAlign(VAlign_Bottom)
			.Padding(FMargin(40, 0, 40, 40))
			[
				SNew(SHorizontalBox)

				// Info panel (left side)
				+ SHorizontalBox::Slot()
				.FillWidth(0.5f)
				[
					BuildInfoPanel()
				]

				// Right side - empty for visual balance
				+ SHorizontalBox::Slot()
				.FillWidth(0.5f)
			]

			// Page indicators (bottom center)
			+ SOverlay::Slot()
			.VAlign(VAlign_Bottom)
			.HAlign(HAlign_Center)
			.Padding(FMargin(0, 0, 0, 16))
			[
				BuildPageIndicators()
			]
		]
	];

	// Start Ken Burns
	StartKenBurns();
}

TSharedRef<SWidget> SJellyfinHeroBanner::BuildInfoPanel()
{
	return SNew(SVerticalBox)

		// Label (CONTINUE WATCHING, NEW, etc.)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0, 0, 0, 8)
		[
			SNew(STextBlock)
			.Text_Lambda([this]() { return GetLabelText(); })
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
			.ColorAndOpacity_Lambda([this]()
			{
				FLinearColor Color = JellyfinColors::Primary;
				Color.A = TitleOpacity;
				return Color;
			})
		]

		// Title
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0, 0, 0, 12)
		[
			SNew(STextBlock)
			.Text_Lambda([this]()
			{
				return FText::FromString(CurrentItem.Name);
			})
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 48))
			.ColorAndOpacity_Lambda([this]()
			{
				FLinearColor Color = JellyfinColors::Text;
				Color.A = TitleOpacity;
				return Color;
			})
			.AutoWrapText(false)
			.OverflowPolicy(ETextOverflowPolicy::Ellipsis)
		]

		// Metadata (Year, Runtime, Genres)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0, 0, 0, 12)
		[
			SNew(STextBlock)
			.Text_Lambda([this]()
			{
				TArray<FString> Parts;

				if (CurrentItem.ProductionYear > 0)
				{
					Parts.Add(FString::FromInt(CurrentItem.ProductionYear));
				}

				if (CurrentItem.RunTimeTicks > 0)
				{
					Parts.Add(FormatRuntime(CurrentItem.RunTimeTicks));
				}

				if (!CurrentItem.OfficialRating.IsEmpty())
				{
					Parts.Add(CurrentItem.OfficialRating);
				}

				return FText::FromString(FString::Join(Parts, TEXT(" \u2022 ")));
			})
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 16))
			.ColorAndOpacity_Lambda([this]()
			{
				FLinearColor Color = JellyfinColors::Text;
				Color.A = MetadataOpacity * 0.7f;
				return Color;
			})
		]

		// Description
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0, 0, 0, 24)
		[
			SNew(SBox)
			.MaxDesiredWidth(500.0f)
			[
				SNew(STextBlock)
				.Text_Lambda([this]()
				{
					// Truncate description to ~150 chars
					FString Desc = CurrentItem.Overview;
					if (Desc.Len() > 150)
					{
						Desc = Desc.Left(147) + TEXT("...");
					}
					return FText::FromString(Desc);
				})
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 14))
				.ColorAndOpacity_Lambda([this]()
				{
					FLinearColor Color = JellyfinColors::Text;
					Color.A = DescriptionOpacity * 0.6f;
					return Color;
				})
				.AutoWrapText(true)
			]
		]

		// Action buttons
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBox)
			.RenderOpacity_Lambda([this]() { return ButtonsOpacity; })
			[
				BuildActionButtons()
			]
		];
}

TSharedRef<SWidget> SJellyfinHeroBanner::BuildActionButtons()
{
	return SNew(SHorizontalBox)

		// Play/Resume button (primary)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(0, 0, 16, 0)
		[
			SNew(SBox)
			.HeightOverride(48.0f)
			.MinDesiredWidth(140.0f)
			[
				SNew(SButton)
				.ButtonColorAndOpacity(JellyfinColors::Primary)
				.OnClicked_Lambda([this]()
				{
					if (OnPlayClickedCallback.IsBound())
					{
						OnPlayClickedCallback.Execute();
					}
					return FReply::Handled();
				})
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(16, 0, 8, 0)
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("\u25B6"))) // Play triangle
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 16))
						.ColorAndOpacity(FLinearColor::White)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(0, 0, 16, 0)
					[
						SNew(STextBlock)
						.Text_Lambda([this]()
						{
							return CurrentItem.GetPlaybackProgress() > 0.0f
								? FText::FromString(TEXT("Resume"))
								: FText::FromString(TEXT("Play"));
						})
						.Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
						.ColorAndOpacity(FLinearColor::White)
					]
				]
			]
		]

		// More Info button (secondary)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SBox)
			.HeightOverride(48.0f)
			.MinDesiredWidth(140.0f)
			[
				SNew(SButton)
				.ButtonColorAndOpacity(FLinearColor(1, 1, 1, 0.2f))
				.OnClicked_Lambda([this]()
				{
					if (OnInfoClickedCallback.IsBound())
					{
						OnInfoClickedCallback.Execute();
					}
					return FReply::Handled();
				})
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(16, 0, 8, 0)
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("\u24D8"))) // Info icon
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 16))
						.ColorAndOpacity(FLinearColor::White)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(0, 0, 16, 0)
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("More Info")))
						.Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
						.ColorAndOpacity(FLinearColor::White)
					]
				]
			]
		];
}

TSharedRef<SWidget> SJellyfinHeroBanner::BuildPageIndicators()
{
	TSharedRef<SHorizontalBox> IndicatorBox = SNew(SHorizontalBox);

	// We'll populate this dynamically when items are set
	// For now, return empty container
	PageIndicatorsWidget = IndicatorBox;

	return IndicatorBox;
}

void SJellyfinHeroBanner::SetFeaturedItems(const TArray<FJellyfinMediaItem>& Items)
{
	FeaturedItems = Items;
	CurrentItemIndex = 0;
	TimeSinceLastRotate = 0.0f;

	if (FeaturedItems.Num() > 0)
	{
		CurrentItem = FeaturedItems[0];
		AnimateContentIn();
	}

	// Rebuild page indicators
	if (PageIndicatorsWidget.IsValid())
	{
		TSharedPtr<SHorizontalBox> IndicatorBox = StaticCastSharedPtr<SHorizontalBox>(PageIndicatorsWidget);
		if (IndicatorBox.IsValid())
		{
			IndicatorBox->ClearChildren();

			for (int32 i = 0; i < FeaturedItems.Num(); ++i)
			{
				int32 Index = i;
				IndicatorBox->AddSlot()
				.AutoWidth()
				.Padding(4, 0)
				[
					SNew(SBox)
					.WidthOverride(8.0f)
					.HeightOverride(8.0f)
					[
						SNew(SBorder)
						.BorderBackgroundColor_Lambda([this, Index]()
						{
							return Index == CurrentItemIndex
								? JellyfinColors::Text
								: JellyfinColors::TextSecondary;
						})
					]
				];
			}
		}
	}
}

void SJellyfinHeroBanner::SetFeaturedItem(const FJellyfinMediaItem& Item)
{
	TArray<FJellyfinMediaItem> SingleItem;
	SingleItem.Add(Item);
	SetFeaturedItems(SingleItem);
}

void SJellyfinHeroBanner::SetBackdropImage(UTexture2D* Texture)
{
	if (!Texture)
	{
		return;
	}

	if (!BackdropBrush.IsValid())
	{
		BackdropBrush = MakeShareable(new FSlateBrush());
	}

	BackdropBrush->SetResourceObject(Texture);
	BackdropBrush->ImageSize = FVector2D(Texture->GetSizeX(), Texture->GetSizeY());
	BackdropBrush->DrawAs = ESlateBrushDrawType::Image;

	// Animate backdrop fade in
	FJellyfinUIAnimator::Get().Cancel(BackdropFadeAnimId);
	BackdropFadeAnimId = FJellyfinUIAnimator::Get().Animate(
		BackdropOpacity, 1.0f,
		0.5f,
		EJellyfinEaseType::EaseOut,
		[this](float Value) { BackdropOpacity = Value; }
	);
}

void SJellyfinHeroBanner::NextItem()
{
	if (FeaturedItems.Num() <= 1)
	{
		return;
	}

	int32 NextIndex = (CurrentItemIndex + 1) % FeaturedItems.Num();
	TransitionToItem(NextIndex);
}

void SJellyfinHeroBanner::PreviousItem()
{
	if (FeaturedItems.Num() <= 1)
	{
		return;
	}

	int32 PrevIndex = (CurrentItemIndex - 1 + FeaturedItems.Num()) % FeaturedItems.Num();
	TransitionToItem(PrevIndex);
}

void SJellyfinHeroBanner::GoToItem(int32 Index)
{
	if (Index >= 0 && Index < FeaturedItems.Num() && Index != CurrentItemIndex)
	{
		TransitionToItem(Index);
	}
}

void SJellyfinHeroBanner::StartKenBurns()
{
	if (bKenBurnsActive)
	{
		return;
	}

	bKenBurnsActive = true;

	// Animate scale from 1.0 to 1.05 over 10 seconds
	KenBurnsAnimId = FJellyfinUIAnimator::Get().AnimateLoop(
		1.0f, 1.05f,
		10.0f,
		EJellyfinEaseType::Linear,
		[this](float Value) { KenBurnsScale = Value; }
	);
}

void SJellyfinHeroBanner::StopKenBurns()
{
	bKenBurnsActive = false;
	FJellyfinUIAnimator::Get().Cancel(KenBurnsAnimId);
	KenBurnsScale = 1.0f;
}

void SJellyfinHeroBanner::AnimateContentIn()
{
	// Cancel any running animations
	FJellyfinUIAnimator::Get().Cancel(TitleFadeAnimId);
	FJellyfinUIAnimator::Get().Cancel(MetadataFadeAnimId);
	FJellyfinUIAnimator::Get().Cancel(DescriptionFadeAnimId);
	FJellyfinUIAnimator::Get().Cancel(ButtonsFadeAnimId);

	// Reset opacities
	TitleOpacity = 0.0f;
	MetadataOpacity = 0.0f;
	DescriptionOpacity = 0.0f;
	ButtonsOpacity = 0.0f;

	// Staggered fade-in
	TitleFadeAnimId = FJellyfinUIAnimator::Get().Animate(
		0.0f, 1.0f, 0.3f, EJellyfinEaseType::EaseOut,
		[this](float V) { TitleOpacity = V; }
	);

	MetadataFadeAnimId = FJellyfinUIAnimator::Get().AnimateDelayed(
		0.1f, 0.0f, 1.0f, 0.3f, EJellyfinEaseType::EaseOut,
		[this](float V) { MetadataOpacity = V; }
	);

	DescriptionFadeAnimId = FJellyfinUIAnimator::Get().AnimateDelayed(
		0.2f, 0.0f, 1.0f, 0.3f, EJellyfinEaseType::EaseOut,
		[this](float V) { DescriptionOpacity = V; }
	);

	ButtonsFadeAnimId = FJellyfinUIAnimator::Get().AnimateDelayed(
		0.3f, 0.0f, 1.0f, 0.3f, EJellyfinEaseType::EaseOut,
		[this](float V) { ButtonsOpacity = V; }
	);
}

void SJellyfinHeroBanner::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	// Auto-rotate
	if (bAutoRotate && FeaturedItems.Num() > 1 && !bIsTransitioning)
	{
		TimeSinceLastRotate += InDeltaTime;
		if (TimeSinceLastRotate >= RotateInterval)
		{
			NextItem();
			TimeSinceLastRotate = 0.0f;
		}
	}
}

void SJellyfinHeroBanner::UpdateDisplayedContent()
{
	// Content is updated via lambdas bound in Construct
}

void SJellyfinHeroBanner::TransitionToItem(int32 NewIndex)
{
	if (bIsTransitioning || NewIndex == CurrentItemIndex)
	{
		return;
	}

	bIsTransitioning = true;

	// Fade out current content
	FJellyfinUIAnimator::Get().Animate(
		1.0f, 0.0f, 0.2f, EJellyfinEaseType::EaseOut,
		[this](float V)
		{
			TitleOpacity = V;
			MetadataOpacity = V;
			DescriptionOpacity = V;
			ButtonsOpacity = V;
		},
		[this, NewIndex]()
		{
			// Switch content
			CurrentItemIndex = NewIndex;
			CurrentItem = FeaturedItems[CurrentItemIndex];

			// Fade in new content
			AnimateContentIn();
			bIsTransitioning = false;
		}
	);

	TimeSinceLastRotate = 0.0f;
}

FString SJellyfinHeroBanner::FormatRuntime(int64 RuntimeTicks) const
{
	// Ticks are in 100-nanosecond units
	int64 TotalMinutes = RuntimeTicks / 600000000LL;
	int32 Hours = TotalMinutes / 60;
	int32 Minutes = TotalMinutes % 60;

	if (Hours > 0)
	{
		return FString::Printf(TEXT("%dh %dm"), Hours, Minutes);
	}
	else
	{
		return FString::Printf(TEXT("%dm"), Minutes);
	}
}

FText SJellyfinHeroBanner::GetLabelText() const
{
	if (CurrentItem.GetPlaybackProgress() > 0.0f)
	{
		return FText::FromString(TEXT("CONTINUE WATCHING"));
	}

	// Check if recently added (within 7 days)
	// For now, just return empty or "FEATURED"
	return FText::FromString(TEXT("FEATURED"));
}
