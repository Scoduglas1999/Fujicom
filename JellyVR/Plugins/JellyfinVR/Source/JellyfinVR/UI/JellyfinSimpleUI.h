// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "API/JellyfinTypes.h"
#include "JellyfinSimpleUI.generated.h"

// Forward declarations - UMG widgets
class UJellyfinClient;
class AJellyfinScreenActor;
class UButton;
class UTextBlock;
class UEditableTextBox;
class UImage;
class UScrollBox;
class UProgressBar;
class UHorizontalBox;
class UVerticalBox;
class UGridPanel;
class UBorder;
class USizeBox;
class UCanvasPanel;

// Forward declarations - Slate widgets
class SEditableTextBox;
class STextBlock;
class SButton;
class SVerticalBox;
class SHorizontalBox;
class SScrollBox;
class SUniformGridPanel;
class SProgressBar;
class SSlider;
class SBox;
class SBorder;
class SImage;

/**
 * Built-in simple login widget
 * Works out of the box without Blueprint customization
 */
UCLASS()
class JELLYFINVR_API UJellyfinLoginWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	UFUNCTION()
	void OnConnectClicked();

	UFUNCTION()
	void OnConnectionStateChanged(EJellyfinAuthState NewState);

private:
	TSharedPtr<SEditableTextBox> ServerUrlBox;
	TSharedPtr<SEditableTextBox> UsernameBox;
	TSharedPtr<SEditableTextBox> PasswordBox;
	TSharedPtr<STextBlock> StatusText;
	TSharedPtr<SButton> ConnectButton;

	UPROPERTY()
	AJellyfinScreenActor* OwningScreen;
};

/**
 * Built-in simple home widget
 * Shows libraries and continue watching
 */
UCLASS()
class JELLYFINVR_API UJellyfinSimpleHomeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetOwningScreen(AJellyfinScreenActor* Screen) { OwningScreen = Screen; }
	void Refresh();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	UFUNCTION()
	void OnLibrariesLoaded(bool bSuccess, const TArray<FJellyfinLibrary>& Libraries);

	UFUNCTION()
	void OnItemsLoaded(bool bSuccess, const FJellyfinItemsResult& Result);

	void OnLibraryClicked(FString LibraryId);
	void OnItemClicked(FJellyfinMediaItem Item);

private:
	TSharedPtr<SVerticalBox> ContentBox;
	TSharedPtr<SScrollBox> LibrariesScroll;
	TSharedPtr<SScrollBox> ResumeScroll;

	UPROPERTY()
	AJellyfinScreenActor* OwningScreen;

	UPROPERTY()
	UJellyfinClient* JellyfinClient;

	TArray<FJellyfinLibrary> LoadedLibraries;
	TArray<FJellyfinMediaItem> ResumeItems;

	// Image brushes for poster images (keyed by ItemId)
	TMap<FString, TSharedPtr<FSlateBrush>> ImageBrushes;

	// Helper to get or create brush for an item
	const FSlateBrush* GetItemBrush(const FString& ItemId) const;
	void RequestItemImage(const FString& ItemId);

	UFUNCTION()
	void OnImageLoaded(const FString& ImageId, UTexture2D* Texture);
};

/**
 * Built-in simple library browser widget
 */
UCLASS()
class JELLYFINVR_API UJellyfinSimpleLibraryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetOwningScreen(AJellyfinScreenActor* Screen) { OwningScreen = Screen; }
	void BrowseLibrary(const FString& LibraryId, const FString& LibraryName);
	void BrowseFolder(const FString& FolderId, const FString& FolderName);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	UFUNCTION()
	void OnItemsLoaded(bool bSuccess, const FJellyfinItemsResult& Result);

	void OnItemClicked(FJellyfinMediaItem Item);
	void OnBackClicked();

private:
	TSharedPtr<SVerticalBox> ContentBox;
	TSharedPtr<SUniformGridPanel> ItemsGrid;
	TSharedPtr<STextBlock> TitleText;
	TSharedPtr<SButton> BackButton;

	UPROPERTY()
	AJellyfinScreenActor* OwningScreen;

	UPROPERTY()
	UJellyfinClient* JellyfinClient;

	TArray<TPair<FString, FString>> NavigationStack; // Id, Name pairs
	FString CurrentFolderId;
	FString CurrentFolderName;

	// Currently displayed item IDs (for filtering image load callbacks)
	TSet<FString> CurrentDisplayedItemIds;

	// Image brushes for poster images (keyed by ItemId)
	TMap<FString, TSharedPtr<FSlateBrush>> ImageBrushes;

	// Helper to get or create brush for an item
	const FSlateBrush* GetItemBrush(const FString& ItemId) const;
	void RequestItemImage(const FString& ItemId);

	UFUNCTION()
	void OnImageLoaded(const FString& ImageId, UTexture2D* Texture);
};

/**
 * Built-in simple player controls widget
 */
UCLASS()
class JELLYFINVR_API UJellyfinSimpleControlsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void SetOwningScreen(AJellyfinScreenActor* Screen) { OwningScreen = Screen; }

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	void OnPlayPauseClicked();
	void OnBackClicked();
	void OnForwardClicked();
	void OnStopClicked();
	void OnProgressChanged(float NewValue);

private:
	TSharedPtr<STextBlock> TitleText;
	TSharedPtr<STextBlock> TimeText;
	TSharedPtr<SProgressBar> ProgressBar;
	TSharedPtr<SSlider> SeekSlider;
	TSharedPtr<SButton> PlayPauseButton;
	TSharedPtr<STextBlock> PlayPauseText;

	UPROPERTY()
	AJellyfinScreenActor* OwningScreen;

	bool bIsSeeking = false;
};

/**
 * Factory class to create default UI widgets
 */
UCLASS()
class JELLYFINVR_API UJellyfinUIFactory : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Create the login widget
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	static UJellyfinLoginWidget* CreateLoginWidget(AJellyfinScreenActor* OwningScreen);

	/**
	 * Create the home widget
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	static UJellyfinSimpleHomeWidget* CreateHomeWidget(AJellyfinScreenActor* OwningScreen);

	/**
	 * Create the library browser widget
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	static UJellyfinSimpleLibraryWidget* CreateLibraryWidget(AJellyfinScreenActor* OwningScreen);

	/**
	 * Create the player controls widget
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	static UJellyfinSimpleControlsWidget* CreateControlsWidget(AJellyfinScreenActor* OwningScreen);
};
