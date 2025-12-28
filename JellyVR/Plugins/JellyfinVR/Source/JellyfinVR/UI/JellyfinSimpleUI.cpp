// Copyright JellyVR Project. All Rights Reserved.

#include "JellyfinSimpleUI.h"
#include "JellyfinScreenActor.h"
#include "JellyfinVRModule.h"
#include "API/JellyfinClient.h"
#include "API/JellyfinAuth.h"
#include "API/JellyfinImageLoader.h"
#include "Media/JellyfinMediaPlayer.h"
#include "Engine/Texture2D.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"

// Animated UI components
#include "JellyfinUIAnimator.h"
#include "JellyfinUIStyles.h"
#include "SJellyfinCard.h"
#include "SJellyfinHeroBanner.h"
#include "SJellyfinScreenContainer.h"

#define LOCTEXT_NAMESPACE "JellyfinVR"

// ============ UJellyfinLoginWidget ============

void UJellyfinLoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind to auth events
	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
	{
		if (UJellyfinAuthSubsystem* AuthSubsystem = GameInstance->GetSubsystem<UJellyfinAuthSubsystem>())
		{
			AuthSubsystem->OnConnectionStateChanged.AddDynamic(this, &UJellyfinLoginWidget::OnConnectionStateChanged);

			// Pre-fill saved values
			if (AuthSubsystem->HasSavedCredentials())
			{
				if (ServerUrlBox.IsValid())
				{
					ServerUrlBox->SetText(FText::FromString(AuthSubsystem->GetSavedServerUrl()));
				}
				if (UsernameBox.IsValid())
				{
					UsernameBox->SetText(FText::FromString(AuthSubsystem->GetSavedUsername()));
				}
			}
		}
	}
}

TSharedRef<SWidget> UJellyfinLoginWidget::RebuildWidget()
{
	// Full-screen dark background with centered login card
	return SNew(SBorder)
		.BorderBackgroundColor(JellyfinColors::Background)
		[
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				// Login card with subtle border
				SNew(SBorder)
				.BorderBackgroundColor(JellyfinColors::CardBackground)
				.BorderImage(FCoreStyle::Get().GetBrush("Border"))
				.Padding(JellyfinLayout::LoginCardPadding)
				[
					SNew(SBox)
					.WidthOverride(JellyfinLayout::LoginCardWidth)
					[
						SNew(SVerticalBox)

						// Logo/Title
						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Center)
						.Padding(0, 0, 0, 8)
						[
							SNew(STextBlock)
							.Text(LOCTEXT("AppTitle", "JellyVR"))
							.Font(FCoreStyle::GetDefaultFontStyle("Bold", 32))
							.ColorAndOpacity(JellyfinColors::Primary)
						]

						// Tagline
						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Center)
						.Padding(0, 0, 0, 32)
						[
							SNew(STextBlock)
							.Text(LOCTEXT("Tagline", "Stream your media in VR"))
							.Font(FCoreStyle::GetDefaultFontStyle("Regular", 14))
							.ColorAndOpacity(JellyfinColors::TextSecondary)
						]

						// Server URL Label
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(0, 0, 0, 8)
						[
							SNew(STextBlock)
							.Text(LOCTEXT("ServerUrlLabel", "Server URL"))
							.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
							.ColorAndOpacity(JellyfinColors::TextSecondary)
						]

						// Server URL Input
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(0, 0, 0, 16)
						[
							SNew(SBox)
							.HeightOverride(JellyfinLayout::InputHeight)
							[
								SAssignNew(ServerUrlBox, SEditableTextBox)
								.HintText(LOCTEXT("ServerUrlHint", "http://your-server:8096"))
								.Font(FCoreStyle::GetDefaultFontStyle("Regular", 14))
								.BackgroundColor(JellyfinColors::InputBackground)
								.ForegroundColor(JellyfinColors::Text)
							]
						]

						// Username Label
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(0, 0, 0, 8)
						[
							SNew(STextBlock)
							.Text(LOCTEXT("UsernameLabel", "Username"))
							.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
							.ColorAndOpacity(JellyfinColors::TextSecondary)
						]

						// Username Input
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(0, 0, 0, 16)
						[
							SNew(SBox)
							.HeightOverride(JellyfinLayout::InputHeight)
							[
								SAssignNew(UsernameBox, SEditableTextBox)
								.HintText(LOCTEXT("UsernameHint", "Enter username"))
								.Font(FCoreStyle::GetDefaultFontStyle("Regular", 14))
								.BackgroundColor(JellyfinColors::InputBackground)
								.ForegroundColor(JellyfinColors::Text)
							]
						]

						// Password Label
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(0, 0, 0, 8)
						[
							SNew(STextBlock)
							.Text(LOCTEXT("PasswordLabel", "Password"))
							.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
							.ColorAndOpacity(JellyfinColors::TextSecondary)
						]

						// Password Input
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(0, 0, 0, 24)
						[
							SNew(SBox)
							.HeightOverride(JellyfinLayout::InputHeight)
							[
								SAssignNew(PasswordBox, SEditableTextBox)
								.HintText(LOCTEXT("PasswordHint", "Enter password"))
								.Font(FCoreStyle::GetDefaultFontStyle("Regular", 14))
								.BackgroundColor(JellyfinColors::InputBackground)
								.ForegroundColor(JellyfinColors::Text)
								.IsPassword(true)
							]
						]

						// Connect Button
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(0, 0, 0, 16)
						[
							SNew(SBox)
							.HeightOverride(JellyfinLayout::ButtonHeight)
							[
								SAssignNew(ConnectButton, SButton)
								.OnClicked_Lambda([this]() { OnConnectClicked(); return FReply::Handled(); })
								.HAlign(HAlign_Fill)
								.VAlign(VAlign_Fill)
								.ButtonColorAndOpacity(JellyfinColors::Primary)
								[
									SNew(SBox)
									.HAlign(HAlign_Center)
									.VAlign(VAlign_Center)
									[
										SNew(STextBlock)
										.Text(LOCTEXT("ConnectButton", "Connect"))
										.Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
										.ColorAndOpacity(FLinearColor::White)
									]
								]
							]
						]

						// Status Text
						+ SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Center)
						[
							SAssignNew(StatusText, STextBlock)
							.Text(FText::GetEmpty())
							.Font(FCoreStyle::GetDefaultFontStyle("Regular", 14))
							.ColorAndOpacity(JellyfinColors::TextSecondary)
						]
					]
				]
			]
		];
}

void UJellyfinLoginWidget::OnConnectClicked()
{
	if (!ServerUrlBox.IsValid() || !UsernameBox.IsValid() || !PasswordBox.IsValid())
	{
		return;
	}

	FString ServerUrl = ServerUrlBox->GetText().ToString().TrimStartAndEnd();
	FString Username = UsernameBox->GetText().ToString().TrimStartAndEnd();
	FString Password = PasswordBox->GetText().ToString();

	if (ServerUrl.IsEmpty() || Username.IsEmpty())
	{
		StatusText->SetText(LOCTEXT("FillAllFields", "Please fill in all fields"));
		StatusText->SetColorAndOpacity(JellyfinColors::Error);
		return;
	}

	// Ensure URL has http:// or https:// protocol
	if (!ServerUrl.StartsWith(TEXT("http://")) && !ServerUrl.StartsWith(TEXT("https://")))
	{
		ServerUrl = TEXT("http://") + ServerUrl;
	}

	StatusText->SetText(LOCTEXT("Connecting", "Connecting..."));
	StatusText->SetColorAndOpacity(JellyfinColors::TextSecondary);

	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
	{
		if (UJellyfinAuthSubsystem* AuthSubsystem = GameInstance->GetSubsystem<UJellyfinAuthSubsystem>())
		{
			AuthSubsystem->Connect(ServerUrl, Username, Password, true);
		}
	}
}

void UJellyfinLoginWidget::OnConnectionStateChanged(EJellyfinAuthState NewState)
{
	switch (NewState)
	{
	case EJellyfinAuthState::Authenticating:
		StatusText->SetText(LOCTEXT("Authenticating", "Authenticating..."));
		StatusText->SetColorAndOpacity(JellyfinColors::TextSecondary);
		break;

	case EJellyfinAuthState::Authenticated:
		StatusText->SetText(LOCTEXT("Connected", "Connected!"));
		StatusText->SetColorAndOpacity(JellyfinColors::Success);
		// Screen will switch to home view
		break;

	case EJellyfinAuthState::Failed:
		StatusText->SetText(LOCTEXT("ConnectionFailed", "Connection failed. Check your credentials."));
		StatusText->SetColorAndOpacity(JellyfinColors::Error);
		break;

	default:
		break;
	}
}

// ============ UJellyfinSimpleHomeWidget ============

void UJellyfinSimpleHomeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
	{
		if (UJellyfinAuthSubsystem* AuthSubsystem = GameInstance->GetSubsystem<UJellyfinAuthSubsystem>())
		{
			JellyfinClient = AuthSubsystem->GetClient();
			if (JellyfinClient)
			{
				JellyfinClient->OnLibrariesLoaded.AddDynamic(this, &UJellyfinSimpleHomeWidget::OnLibrariesLoaded);
				JellyfinClient->OnItemsLoaded.AddDynamic(this, &UJellyfinSimpleHomeWidget::OnItemsLoaded);
			}
		}
	}

	// Subscribe to image loaded events
	if (UJellyfinImageLoader* ImageLoader = UJellyfinImageLoader::Get(this))
	{
		ImageLoader->OnAnyImageLoaded.AddDynamic(this, &UJellyfinSimpleHomeWidget::OnImageLoaded);
	}

	Refresh();
}

TSharedRef<SWidget> UJellyfinSimpleHomeWidget::RebuildWidget()
{
	return SNew(SBorder)
		.BorderBackgroundColor(JellyfinColors::Background)
		[
			SNew(SVerticalBox)

			// Header bar
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(JellyfinLayout::RowPadding, 20, JellyfinLayout::RowPadding, 20)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("HomeTitle", "JellyVR"))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 24))
					.ColorAndOpacity(JellyfinColors::Primary)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.ButtonColorAndOpacity(FLinearColor::Transparent)
					.OnClicked_Lambda([this]() {
						// Settings button - future functionality
						return FReply::Handled();
					})
					[
						SNew(STextBlock)
						.Text(LOCTEXT("SettingsIcon", "Settings"))
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 14))
						.ColorAndOpacity(JellyfinColors::TextSecondary)
					]
				]
			]

			// Scrollable content area
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				SNew(SScrollBox)
				.Orientation(Orient_Vertical)

				// Continue Watching Row
				+ SScrollBox::Slot()
				[
					SAssignNew(ContentBox, SVerticalBox)

					// Continue Watching Header
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(JellyfinLayout::RowPadding, 0, JellyfinLayout::RowPadding, 12)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("ContinueWatching", "Continue Watching"))
						.Font(FCoreStyle::GetDefaultFontStyle("Bold", 20))
						.ColorAndOpacity(JellyfinColors::Text)
					]

					// Continue Watching Cards
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(JellyfinLayout::RowPadding, 0, 0, JellyfinLayout::RowGap)
					[
						SAssignNew(ResumeScroll, SScrollBox)
						.Orientation(Orient_Horizontal)
					]

					// Libraries Header
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(JellyfinLayout::RowPadding, 0, JellyfinLayout::RowPadding, 12)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("Libraries", "Libraries"))
						.Font(FCoreStyle::GetDefaultFontStyle("Bold", 20))
						.ColorAndOpacity(JellyfinColors::Text)
					]

					// Libraries Cards
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(JellyfinLayout::RowPadding, 0, 0, JellyfinLayout::RowGap)
					[
						SAssignNew(LibrariesScroll, SScrollBox)
						.Orientation(Orient_Horizontal)
					]
				]
			]
		];
}

void UJellyfinSimpleHomeWidget::Refresh()
{
	if (JellyfinClient && JellyfinClient->IsAuthenticated())
	{
		JellyfinClient->GetLibraries();
		JellyfinClient->GetResumeItems(10);
	}
}

void UJellyfinSimpleHomeWidget::OnLibrariesLoaded(bool bSuccess, const TArray<FJellyfinLibrary>& Libraries)
{
	if (!bSuccess || !LibrariesScroll.IsValid())
	{
		return;
	}

	LoadedLibraries = Libraries;
	LibrariesScroll->ClearChildren();

	for (const FJellyfinLibrary& Library : Libraries)
	{
		FString LibId = Library.Id;
		FString LibName = Library.Name;

		LibrariesScroll->AddSlot()
		.Padding(0, 0, JellyfinLayout::CardGap, 0)
		[
			SNew(SBox)
			.WidthOverride(JellyfinLayout::LibraryCardWidth)
			.HeightOverride(JellyfinLayout::LibraryCardHeight)
			[
				SNew(SButton)
				.ButtonColorAndOpacity(FLinearColor::Transparent)
				.OnClicked_Lambda([this, LibId]() { OnLibraryClicked(LibId); return FReply::Handled(); })
				[
					SNew(SBorder)
					.BorderBackgroundColor(JellyfinColors::CardBackground)
					.Padding(0)
					[
						// Gradient overlay for text readability
						SNew(SOverlay)
						+ SOverlay::Slot()
						[
							// Background with subtle gradient (simulated with darker bottom)
							SNew(SBorder)
							.BorderBackgroundColor(JellyfinColors::CardBackground)
						]
						+ SOverlay::Slot()
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Center)
						[
							SNew(SVerticalBox)
							// Library icon placeholder
							+ SVerticalBox::Slot()
							.AutoHeight()
							.HAlign(HAlign_Center)
							.Padding(0, 0, 0, 8)
							[
								SNew(STextBlock)
								.Text(FText::FromString(TEXT("[ ]"))) // Icon placeholder
								.Font(FCoreStyle::GetDefaultFontStyle("Regular", 24))
								.ColorAndOpacity(JellyfinColors::TextSecondary)
							]
							// Library name
							+ SVerticalBox::Slot()
							.AutoHeight()
							.HAlign(HAlign_Center)
							[
								SNew(STextBlock)
								.Text(FText::FromString(LibName))
								.Font(FCoreStyle::GetDefaultFontStyle("Bold", 18))
								.ColorAndOpacity(JellyfinColors::Text)
							]
						]
					]
				]
			]
		];
	}
}

void UJellyfinSimpleHomeWidget::OnItemsLoaded(bool bSuccess, const FJellyfinItemsResult& Result)
{
	if (!bSuccess || !ResumeScroll.IsValid())
	{
		return;
	}

	ResumeItems = Result.Items;
	ResumeScroll->ClearChildren();

	for (const FJellyfinMediaItem& Item : ResumeItems)
	{
		FJellyfinMediaItem ItemCopy = Item;
		FString ItemId = Item.Id;
		FString ItemName = Item.Name;
		float Progress = Item.GetPlaybackProgress();

		// Get first letter for fallback display (streaming-style)
		FString FirstLetter = ItemName.Len() > 0 ? ItemName.Left(1).ToUpper() : TEXT("?");

		// Request image for this item
		RequestItemImage(ItemId);

		ResumeScroll->AddSlot()
		.Padding(0, 0, JellyfinLayout::CardGap, 0)
		[
			SNew(SBox)
			.WidthOverride(JellyfinLayout::PosterWidth)
			[
				SNew(SButton)
				.ButtonColorAndOpacity(FLinearColor::Transparent)
				.OnClicked_Lambda([this, ItemCopy]() { OnItemClicked(ItemCopy); return FReply::Handled(); })
				[
					SNew(SVerticalBox)

					// Poster image area
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBox)
						.HeightOverride(JellyfinLayout::PosterHeight)
						[
							SNew(SOverlay)

							// Poster background with first letter fallback (streaming-style)
							+ SOverlay::Slot()
							[
								SNew(SBorder)
								.BorderBackgroundColor(JellyfinColors::CardBackground)
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								[
									SNew(STextBlock)
									.Text(FText::FromString(FirstLetter))
									.Font(FCoreStyle::GetDefaultFontStyle("Bold", 48))
									.ColorAndOpacity(JellyfinColors::TextSecondary)
								]
							]

							// Poster image (shown when loaded, hidden when no image)
							+ SOverlay::Slot()
							[
								SNew(SImage)
								.Image_Lambda([this, ItemId]() -> const FSlateBrush*
								{
									return GetItemBrush(ItemId);
								})
								.Visibility_Lambda([this, ItemId]() -> EVisibility
								{
									return GetItemBrush(ItemId) != nullptr ? EVisibility::Visible : EVisibility::Collapsed;
								})
							]

							// Progress bar at bottom (inside poster)
							+ SOverlay::Slot()
							.VAlign(VAlign_Bottom)
							[
								SNew(SBox)
								.HeightOverride(4.0f)
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
									[
										SNew(SBox)
										.WidthOverride(JellyfinLayout::PosterWidth * Progress)
										[
											SNew(SBorder)
											.BorderBackgroundColor(JellyfinColors::Primary)
										]
									]
								]
							]
						]
					]

					// Title below poster
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0, 8, 0, 0)
					[
						SNew(STextBlock)
						.Text(FText::FromString(ItemName))
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 14))
						.ColorAndOpacity(JellyfinColors::Text)
						.AutoWrapText(false)
					]
				]
			]
		];
	}
}

void UJellyfinSimpleHomeWidget::OnLibraryClicked(FString LibraryId)
{
	UE_LOG(LogJellyfinVR, Warning, TEXT("=== LIBRARY CLICKED ==="));
	UE_LOG(LogJellyfinVR, Warning, TEXT("Library ID: %s"), *LibraryId);
	UE_LOG(LogJellyfinVR, Warning, TEXT("OwningScreen: %s"), OwningScreen ? TEXT("VALID") : TEXT("NULL"));
	UE_LOG(LogJellyfinVR, Warning, TEXT("this pointer: %p"), this);

	if (OwningScreen)
	{
		// Find library name
		FString LibraryName = TEXT("Library");
		for (const FJellyfinLibrary& Lib : LoadedLibraries)
		{
			if (Lib.Id == LibraryId)
			{
				LibraryName = Lib.Name;
				break;
			}
		}

		UE_LOG(LogJellyfinVR, Warning, TEXT("Calling ShowLibrary: %s - %s"), *LibraryId, *LibraryName);
		OwningScreen->ShowLibrary(LibraryId, LibraryName);
	}
	else
	{
		UE_LOG(LogJellyfinVR, Error, TEXT("Cannot navigate - OwningScreen is NULL!"));
	}
}

void UJellyfinSimpleHomeWidget::OnItemClicked(FJellyfinMediaItem Item)
{
	UE_LOG(LogJellyfinVR, Log, TEXT("Item clicked: %s (ID: %s), OwningScreen valid: %s"),
		*Item.Name, *Item.Id, OwningScreen ? TEXT("YES") : TEXT("NO"));

	if (OwningScreen)
	{
		OwningScreen->PlayItem(Item);
	}
	else
	{
		UE_LOG(LogJellyfinVR, Error, TEXT("Cannot play item - OwningScreen is null!"));
	}
}

const FSlateBrush* UJellyfinSimpleHomeWidget::GetItemBrush(const FString& ItemId) const
{
	if (const TSharedPtr<FSlateBrush>* Brush = ImageBrushes.Find(ItemId))
	{
		return Brush->Get();
	}
	return nullptr;
}

void UJellyfinSimpleHomeWidget::RequestItemImage(const FString& ItemId)
{
	if (UJellyfinImageLoader* ImageLoader = UJellyfinImageLoader::Get(const_cast<UJellyfinSimpleHomeWidget*>(this)))
	{
		// Generate the cache key that will be used by the image loader
		FString CacheKey = FString::Printf(TEXT("item_%s_300x450"), *ItemId);

		// Check if already cached
		if (UTexture2D* CachedTexture = ImageLoader->GetCachedImage(CacheKey))
		{
			OnImageLoaded(ItemId, CachedTexture);
			return;
		}

		// Request the image - we'll get notification via OnAnyImageLoaded delegate
		FOnImageLoaded Callback;
		ImageLoader->LoadItemImage(ItemId, Callback, 300, 450);
	}
}

// Extract ItemId from cache key format "item_{ItemId}_{Width}x{Height}"
// Example: "item_abc123def456_300x450" -> "abc123def456"
static FString ExtractItemIdFromCacheKey(const FString& CacheKey)
{
	const FString Prefix = TEXT("item_");
	if (CacheKey.StartsWith(Prefix))
	{
		// Remove the "item_" prefix
		FString Remainder = CacheKey.Mid(Prefix.Len());

		// Find the last underscore (before dimensions like "300x450")
		int32 LastUnderscore = Remainder.Find(TEXT("_"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		if (LastUnderscore > 0)
		{
			// Return everything before the last underscore (the ItemId)
			return Remainder.Left(LastUnderscore);
		}
	}
	return CacheKey;
}

void UJellyfinSimpleHomeWidget::OnImageLoaded(const FString& ImageId, UTexture2D* Texture)
{
	if (!Texture)
	{
		return;
	}

	// Extract the ItemId from the cache key (format: "item_{ItemId}_300x450")
	FString ItemId = ExtractItemIdFromCacheKey(ImageId);

	// Check if we're tracking this item
	bool bItemTracked = false;
	for (const FJellyfinMediaItem& Item : ResumeItems)
	{
		if (Item.Id == ItemId)
		{
			bItemTracked = true;
			break;
		}
	}

	if (!bItemTracked)
	{
		return; // Not our image
	}

	// Create a brush from the texture
	TSharedPtr<FSlateBrush> NewBrush = MakeShareable(new FSlateBrush());
	NewBrush->SetResourceObject(Texture);
	NewBrush->ImageSize = FVector2D(Texture->GetSizeX(), Texture->GetSizeY());
	NewBrush->DrawAs = ESlateBrushDrawType::Image;

	ImageBrushes.Add(ItemId, NewBrush);

	UE_LOG(LogJellyfinVR, Log, TEXT("Home: Image loaded for item: %s (%dx%d)"),
		*ItemId, Texture->GetSizeX(), Texture->GetSizeY());
}

// ============ UJellyfinSimpleLibraryWidget ============

void UJellyfinSimpleLibraryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
	{
		if (UJellyfinAuthSubsystem* AuthSubsystem = GameInstance->GetSubsystem<UJellyfinAuthSubsystem>())
		{
			JellyfinClient = AuthSubsystem->GetClient();
			if (JellyfinClient)
			{
				JellyfinClient->OnItemsLoaded.AddDynamic(this, &UJellyfinSimpleLibraryWidget::OnItemsLoaded);
			}
		}
	}

	// Subscribe to image loaded events
	if (UJellyfinImageLoader* ImageLoader = UJellyfinImageLoader::Get(this))
	{
		ImageLoader->OnAnyImageLoaded.AddDynamic(this, &UJellyfinSimpleLibraryWidget::OnImageLoaded);
	}
}

TSharedRef<SWidget> UJellyfinSimpleLibraryWidget::RebuildWidget()
{
	return SNew(SBorder)
		.BorderBackgroundColor(JellyfinColors::Background)
		[
			SAssignNew(ContentBox, SVerticalBox)

			// Header bar
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(JellyfinLayout::RowPadding, 20, JellyfinLayout::RowPadding, 16)
			[
				SNew(SHorizontalBox)

				// Home button
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(0, 0, 16, 0)
				[
					SNew(SButton)
					.ButtonColorAndOpacity(FLinearColor::Transparent)
					.OnClicked_Lambda([this]() { if (OwningScreen) OwningScreen->ShowHome(); return FReply::Handled(); })
					[
						SNew(STextBlock)
						.Text(LOCTEXT("HomeButton", "Home"))
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 14))
						.ColorAndOpacity(JellyfinColors::TextSecondary)
					]
				]

				// Back button (only when drilling into folders)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(0, 0, 24, 0)
				[
					SAssignNew(BackButton, SButton)
					.ButtonColorAndOpacity(FLinearColor::Transparent)
					.OnClicked_Lambda([this]() { OnBackClicked(); return FReply::Handled(); })
					.Visibility_Lambda([this]() { return NavigationStack.Num() > 0 ? EVisibility::Visible : EVisibility::Collapsed; })
					[
						SNew(STextBlock)
						.Text(LOCTEXT("BackButton", "< Back"))
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 14))
						.ColorAndOpacity(JellyfinColors::TextSecondary)
					]
				]

				// Title
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.VAlign(VAlign_Center)
				[
					SAssignNew(TitleText, STextBlock)
					.Text(LOCTEXT("LibraryTitle", "Library"))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 24))
					.ColorAndOpacity(JellyfinColors::Text)
				]
			]

			// Items grid with scroll
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			.Padding(JellyfinLayout::RowPadding, 0, JellyfinLayout::RowPadding, 0)
			[
				SNew(SScrollBox)
				.Orientation(Orient_Vertical)
				+ SScrollBox::Slot()
				[
					SAssignNew(ItemsGrid, SUniformGridPanel)
					.SlotPadding(JellyfinLayout::CardGap / 2.0f)
				]
			]
		];
}

void UJellyfinSimpleLibraryWidget::BrowseLibrary(const FString& LibraryId, const FString& LibraryName)
{
	NavigationStack.Empty();
	BrowseFolder(LibraryId, LibraryName);
}

void UJellyfinSimpleLibraryWidget::BrowseFolder(const FString& FolderId, const FString& FolderName)
{
	if (!CurrentFolderId.IsEmpty())
	{
		NavigationStack.Add(TPair<FString, FString>(CurrentFolderId, CurrentFolderName));
	}

	CurrentFolderId = FolderId;
	CurrentFolderName = FolderName;

	if (TitleText.IsValid())
	{
		TitleText->SetText(FText::FromString(FolderName));
	}

	if (JellyfinClient)
	{
		JellyfinClient->GetItems(FolderId, 0, 500);
	}
}

void UJellyfinSimpleLibraryWidget::OnItemsLoaded(bool bSuccess, const FJellyfinItemsResult& Result)
{
	if (!bSuccess || !ItemsGrid.IsValid())
	{
		return;
	}

	ItemsGrid->ClearChildren();
	CurrentDisplayedItemIds.Empty();
	ImageBrushes.Empty();

	int32 Column = 0;
	int32 Row = 0;
	const int32 ColumnsPerRow = 6;

	for (const FJellyfinMediaItem& Item : Result.Items)
	{
		FJellyfinMediaItem ItemCopy = Item;
		FString ItemId = Item.Id;
		FString ItemName = Item.Name;
		int32 Year = Item.ProductionYear;
		bool bIsFolder = (Item.Type == EJellyfinItemType::Series ||
		                  Item.Type == EJellyfinItemType::Folder ||
		                  Item.Type == EJellyfinItemType::Season ||
		                  Item.Type == EJellyfinItemType::BoxSet);

		// Get first letter for fallback display (streaming-style)
		FString FirstLetter = ItemName.Len() > 0 ? ItemName.Left(1).ToUpper() : TEXT("?");

		// Track this item and request its image
		CurrentDisplayedItemIds.Add(ItemId);
		RequestItemImage(ItemId);

		ItemsGrid->AddSlot(Column, Row)
		[
			SNew(SBox)
			.WidthOverride(JellyfinLayout::PosterWidth + JellyfinLayout::CardGap)
			[
				SNew(SButton)
				.ButtonColorAndOpacity(FLinearColor::Transparent)
				.OnClicked_Lambda([this, ItemCopy]() { OnItemClicked(ItemCopy); return FReply::Handled(); })
				[
					SNew(SVerticalBox)

					// Poster area
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBox)
						.HeightOverride(JellyfinLayout::PosterHeight)
						.WidthOverride(JellyfinLayout::PosterWidth)
						[
							SNew(SOverlay)

							// Poster background with first letter fallback (streaming-style)
							+ SOverlay::Slot()
							[
								SNew(SBorder)
								.BorderBackgroundColor(JellyfinColors::CardBackground)
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								[
									SNew(STextBlock)
									.Text(FText::FromString(FirstLetter))
									.Font(FCoreStyle::GetDefaultFontStyle("Bold", 48))
									.ColorAndOpacity(JellyfinColors::TextSecondary)
								]
							]

							// Poster image (shown when loaded, hidden when no image)
							+ SOverlay::Slot()
							[
								SNew(SImage)
								.Image_Lambda([this, ItemId]() -> const FSlateBrush*
								{
									return GetItemBrush(ItemId);
								})
								.Visibility_Lambda([this, ItemId]() -> EVisibility
								{
									return GetItemBrush(ItemId) != nullptr ? EVisibility::Visible : EVisibility::Collapsed;
								})
							]

							// Folder/Series indicator (corner badge)
							+ SOverlay::Slot()
							.HAlign(HAlign_Right)
							.VAlign(VAlign_Top)
							.Padding(4)
							[
								SNew(SBorder)
								.BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.7f))
								.Padding(FMargin(6, 2))
								.Visibility_Lambda([bIsFolder]() { return bIsFolder ? EVisibility::Visible : EVisibility::Collapsed; })
								[
									SNew(STextBlock)
									.Text(FText::FromString(TEXT("▶")))
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
						.WidthOverride(JellyfinLayout::PosterWidth)
						[
							SNew(STextBlock)
							.Text(FText::FromString(ItemName))
							.Font(FCoreStyle::GetDefaultFontStyle("Regular", 14))
							.ColorAndOpacity(JellyfinColors::Text)
							.AutoWrapText(false)
						]
					]

					// Year (if available)
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0, 2, 0, 0)
					[
						SNew(STextBlock)
						.Text(FText::FromString(FString::FromInt(Year)))
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
						.ColorAndOpacity(JellyfinColors::TextSecondary)
						.Visibility_Lambda([Year]() { return Year > 0 ? EVisibility::Visible : EVisibility::Collapsed; })
					]
				]
			]
		];

		Column++;
		if (Column >= ColumnsPerRow)
		{
			Column = 0;
			Row++;
		}
	}
}

void UJellyfinSimpleLibraryWidget::OnItemClicked(FJellyfinMediaItem Item)
{
	UE_LOG(LogJellyfinVR, Log, TEXT("Library item clicked: %s (Type: %d), OwningScreen valid: %s"),
		*Item.Name, (int32)Item.Type, OwningScreen ? TEXT("YES") : TEXT("NO"));

	// If it's a series or folder, browse into it
	if (Item.Type == EJellyfinItemType::Series || Item.Type == EJellyfinItemType::Folder ||
		Item.Type == EJellyfinItemType::Season || Item.Type == EJellyfinItemType::BoxSet)
	{
		UE_LOG(LogJellyfinVR, Log, TEXT("Browsing into folder: %s"), *Item.Name);
		BrowseFolder(Item.Id, Item.Name);
	}
	else if (OwningScreen)
	{
		UE_LOG(LogJellyfinVR, Log, TEXT("Playing item: %s"), *Item.Name);
		OwningScreen->PlayItem(Item);
	}
	else
	{
		UE_LOG(LogJellyfinVR, Error, TEXT("Cannot play - OwningScreen is null!"));
	}
}

void UJellyfinSimpleLibraryWidget::OnBackClicked()
{
	if (NavigationStack.Num() > 0)
	{
		TPair<FString, FString> Previous = NavigationStack.Pop();
		CurrentFolderId = TEXT("");
		CurrentFolderName = TEXT("");
		BrowseFolder(Previous.Key, Previous.Value);
	}
	else if (OwningScreen)
	{
		// No more navigation history - go back to home
		OwningScreen->ShowHome();
	}
}

const FSlateBrush* UJellyfinSimpleLibraryWidget::GetItemBrush(const FString& ItemId) const
{
	if (const TSharedPtr<FSlateBrush>* Brush = ImageBrushes.Find(ItemId))
	{
		return Brush->Get();
	}
	return nullptr;
}

void UJellyfinSimpleLibraryWidget::RequestItemImage(const FString& ItemId)
{
	if (UJellyfinImageLoader* ImageLoader = UJellyfinImageLoader::Get(const_cast<UJellyfinSimpleLibraryWidget*>(this)))
	{
		// Generate the cache key that will be used by the image loader
		FString CacheKey = FString::Printf(TEXT("item_%s_300x450"), *ItemId);

		// Check if already cached
		if (UTexture2D* CachedTexture = ImageLoader->GetCachedImage(CacheKey))
		{
			// Directly set the brush since we have the ItemId
			TSharedPtr<FSlateBrush> NewBrush = MakeShareable(new FSlateBrush());
			NewBrush->SetResourceObject(CachedTexture);
			NewBrush->ImageSize = FVector2D(CachedTexture->GetSizeX(), CachedTexture->GetSizeY());
			NewBrush->DrawAs = ESlateBrushDrawType::Image;
			ImageBrushes.Add(ItemId, NewBrush);
			return;
		}

		// Request the image - we'll get notification via OnAnyImageLoaded delegate
		FOnImageLoaded Callback;
		ImageLoader->LoadItemImage(ItemId, Callback, 300, 450);
	}
}

void UJellyfinSimpleLibraryWidget::OnImageLoaded(const FString& ImageId, UTexture2D* Texture)
{
	if (!Texture)
	{
		return;
	}

	// Extract the ItemId from the cache key (format: "item_{ItemId}_300x450")
	FString ItemId = ExtractItemIdFromCacheKey(ImageId);

	// Check if we're displaying this item
	if (!CurrentDisplayedItemIds.Contains(ItemId))
	{
		return; // Not our image
	}

	// Create a brush from the texture
	TSharedPtr<FSlateBrush> NewBrush = MakeShareable(new FSlateBrush());
	NewBrush->SetResourceObject(Texture);
	NewBrush->ImageSize = FVector2D(Texture->GetSizeX(), Texture->GetSizeY());
	NewBrush->DrawAs = ESlateBrushDrawType::Image;

	ImageBrushes.Add(ItemId, NewBrush);

	UE_LOG(LogJellyfinVR, Log, TEXT("Library: Image loaded for item: %s (%dx%d)"),
		*ItemId, Texture->GetSizeX(), Texture->GetSizeY());
}

// ============ UJellyfinSimpleControlsWidget ============

void UJellyfinSimpleControlsWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

TSharedRef<SWidget> UJellyfinSimpleControlsWidget::RebuildWidget()
{
	// Player controls overlay - gradient background from transparent to dark
	return SNew(SBox)
		.HeightOverride(160.0f)
		[
			SNew(SOverlay)

			// Gradient background (simulated with solid dark + transparency)
			+ SOverlay::Slot()
			[
				SNew(SBorder)
				.BorderBackgroundColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.85f))
			]

			// Content
			+ SOverlay::Slot()
			.Padding(JellyfinLayout::RowPadding, 16, JellyfinLayout::RowPadding, 24)
			[
				SNew(SVerticalBox)

				// Title row
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 12)
				[
					SAssignNew(TitleText, STextBlock)
					.Text(LOCTEXT("NowPlaying", "Now Playing"))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 20))
					.ColorAndOpacity(JellyfinColors::Text)
				]

				// Progress/Seek slider
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 8)
				[
					SNew(SBox)
					.HeightOverride(8.0f)
					[
						SAssignNew(SeekSlider, SSlider)
						.OnValueChanged_Lambda([this](float Value) { OnProgressChanged(Value); })
						.SliderBarColor(JellyfinColors::ProgressBackground)
						.SliderHandleColor(FLinearColor::White)
					]
				]

				// Time display row
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 16)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					[
						SAssignNew(TimeText, STextBlock)
						.Text(LOCTEXT("TimeDisplay", "0:00 / 0:00"))
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 14))
						.ColorAndOpacity(JellyfinColors::TextSecondary)
					]
				]

				// Control buttons row (centered)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Center)
				[
					SNew(SHorizontalBox)

					// Skip Back
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(0, 0, 32, 0)
					[
						SNew(SButton)
						.ButtonColorAndOpacity(FLinearColor::Transparent)
						.OnClicked_Lambda([this]() { OnBackClicked(); return FReply::Handled(); })
						[
							SNew(STextBlock)
							.Text(LOCTEXT("Back10", "-10s"))
							.Font(FCoreStyle::GetDefaultFontStyle("Regular", 16))
							.ColorAndOpacity(JellyfinColors::Text)
						]
					]

					// Play/Pause (larger, prominent)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(0, 0, 32, 0)
					[
						SNew(SBox)
						.WidthOverride(64.0f)
						.HeightOverride(40.0f)
						[
							SAssignNew(PlayPauseButton, SButton)
							.ButtonColorAndOpacity(JellyfinColors::Primary)
							.OnClicked_Lambda([this]() { OnPlayPauseClicked(); return FReply::Handled(); })
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							[
								SAssignNew(PlayPauseText, STextBlock)
								.Text(LOCTEXT("Pause", "Pause"))
								.Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
								.ColorAndOpacity(FLinearColor::White)
							]
						]
					]

					// Skip Forward
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(0, 0, 32, 0)
					[
						SNew(SButton)
						.ButtonColorAndOpacity(FLinearColor::Transparent)
						.OnClicked_Lambda([this]() { OnForwardClicked(); return FReply::Handled(); })
						[
							SNew(STextBlock)
							.Text(LOCTEXT("Forward10", "+10s"))
							.Font(FCoreStyle::GetDefaultFontStyle("Regular", 16))
							.ColorAndOpacity(JellyfinColors::Text)
						]
					]

					// Stop
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					[
						SNew(SButton)
						.ButtonColorAndOpacity(FLinearColor::Transparent)
						.OnClicked_Lambda([this]() { OnStopClicked(); return FReply::Handled(); })
						[
							SNew(STextBlock)
							.Text(LOCTEXT("Stop", "Stop"))
							.Font(FCoreStyle::GetDefaultFontStyle("Regular", 16))
							.ColorAndOpacity(JellyfinColors::TextSecondary)
						]
					]
				]
			]
		];
}

void UJellyfinSimpleControlsWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!OwningScreen)
	{
		return;
	}

	UJellyfinMediaPlayerComponent* Player = OwningScreen->GetMediaPlayer();
	if (!Player)
	{
		return;
	}

	// Update title
	if (TitleText.IsValid())
	{
		TitleText->SetText(FText::FromString(Player->GetCurrentItem().Name));
	}

	// Update time display
	if (TimeText.IsValid())
	{
		FString TimeString = FString::Printf(TEXT("%s / %s"),
			*Player->GetCurrentTimeFormatted(),
			*Player->GetDurationFormatted());
		TimeText->SetText(FText::FromString(TimeString));
	}

	// Update progress bar (only if not seeking)
	if (SeekSlider.IsValid() && !bIsSeeking)
	{
		SeekSlider->SetValue(Player->GetProgress());
	}

	// Update play/pause button text
	if (PlayPauseText.IsValid())
	{
		PlayPauseText->SetText(Player->IsPlaying() ?
			LOCTEXT("Pause", "Pause") : LOCTEXT("Play", "Play"));
	}
}

void UJellyfinSimpleControlsWidget::OnPlayPauseClicked()
{
	if (OwningScreen)
	{
		if (UJellyfinMediaPlayerComponent* Player = OwningScreen->GetMediaPlayer())
		{
			Player->TogglePlayPause();
		}
	}
}

void UJellyfinSimpleControlsWidget::OnBackClicked()
{
	if (OwningScreen)
	{
		if (UJellyfinMediaPlayerComponent* Player = OwningScreen->GetMediaPlayer())
		{
			Player->SeekRelative(-10.0f);
		}
	}
}

void UJellyfinSimpleControlsWidget::OnForwardClicked()
{
	if (OwningScreen)
	{
		if (UJellyfinMediaPlayerComponent* Player = OwningScreen->GetMediaPlayer())
		{
			Player->SeekRelative(10.0f);
		}
	}
}

void UJellyfinSimpleControlsWidget::OnStopClicked()
{
	if (OwningScreen)
	{
		if (UJellyfinMediaPlayerComponent* Player = OwningScreen->GetMediaPlayer())
		{
			Player->Stop();
		}
		OwningScreen->ShowUI();
	}
}

void UJellyfinSimpleControlsWidget::OnProgressChanged(float NewValue)
{
	bIsSeeking = true;

	if (OwningScreen)
	{
		if (UJellyfinMediaPlayerComponent* Player = OwningScreen->GetMediaPlayer())
		{
			Player->SeekToProgress(NewValue);
		}
	}

	// Reset seeking flag after a short delay
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		bIsSeeking = false;
	}, 0.5f, false);
}

// ============ UJellyfinUIFactory ============

UJellyfinLoginWidget* UJellyfinUIFactory::CreateLoginWidget(AJellyfinScreenActor* OwningScreen)
{
	UJellyfinLoginWidget* Widget = CreateWidget<UJellyfinLoginWidget>(OwningScreen->GetWorld());
	return Widget;
}

UJellyfinSimpleHomeWidget* UJellyfinUIFactory::CreateHomeWidget(AJellyfinScreenActor* OwningScreen)
{
	UJellyfinSimpleHomeWidget* Widget = CreateWidget<UJellyfinSimpleHomeWidget>(OwningScreen->GetWorld());
	Widget->SetOwningScreen(OwningScreen);
	return Widget;
}

UJellyfinSimpleLibraryWidget* UJellyfinUIFactory::CreateLibraryWidget(AJellyfinScreenActor* OwningScreen)
{
	UJellyfinSimpleLibraryWidget* Widget = CreateWidget<UJellyfinSimpleLibraryWidget>(OwningScreen->GetWorld());
	Widget->SetOwningScreen(OwningScreen);
	return Widget;
}

UJellyfinSimpleControlsWidget* UJellyfinUIFactory::CreateControlsWidget(AJellyfinScreenActor* OwningScreen)
{
	UJellyfinSimpleControlsWidget* Widget = CreateWidget<UJellyfinSimpleControlsWidget>(OwningScreen->GetWorld());
	Widget->SetOwningScreen(OwningScreen);
	return Widget;
}

#undef LOCTEXT_NAMESPACE
