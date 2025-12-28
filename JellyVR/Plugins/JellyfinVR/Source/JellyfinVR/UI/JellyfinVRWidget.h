// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "API/JellyfinTypes.h"
#include "JellyfinVRWidget.generated.h"

class UJellyfinClient;
class AJellyfinScreenActor;

/**
 * Base class for all Jellyfin VR UI widgets
 * Provides common functionality for VR interaction and Jellyfin integration
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class JELLYFINVR_API UJellyfinVRWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/**
	 * Set the owning screen actor
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	void SetOwningScreen(AJellyfinScreenActor* Screen);

	/**
	 * Get the Jellyfin client
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|UI")
	UJellyfinClient* GetJellyfinClient() const;

	/**
	 * Get the owning screen actor
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|UI")
	AJellyfinScreenActor* GetOwningScreen() const { return OwningScreen; }

	/**
	 * Navigate back (called by back button or gesture)
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "JellyfinVR|UI")
	void NavigateBack();

	/**
	 * Show loading indicator
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "JellyfinVR|UI")
	void ShowLoading(const FString& Message = TEXT("Loading..."));

	/**
	 * Hide loading indicator
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "JellyfinVR|UI")
	void HideLoading();

	/**
	 * Show error message
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "JellyfinVR|UI")
	void ShowError(const FString& ErrorMessage);

	/**
	 * Check if this widget can navigate back
	 */
	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "JellyfinVR|UI")
	bool CanNavigateBack() const;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	AJellyfinScreenActor* OwningScreen;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	UJellyfinClient* JellyfinClient;

	/** Override in Blueprint to handle loading state visuals */
	UPROPERTY(BlueprintReadWrite, Category = "JellyfinVR|UI")
	bool bIsLoading = false;
};

/**
 * Widget for displaying a single media item (poster card)
 */
UCLASS(BlueprintType, Blueprintable)
class JELLYFINVR_API UJellyfinMediaItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Set the media item to display
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	void SetItem(const FJellyfinMediaItem& Item);

	/**
	 * Get the displayed item
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|UI")
	const FJellyfinMediaItem& GetItem() const { return MediaItem; }

	/**
	 * Called when this item is clicked
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "JellyfinVR|UI")
	void OnItemClicked();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FJellyfinMediaItem MediaItem;

	/** Override in Blueprint to update visuals when item changes */
	UFUNCTION(BlueprintImplementableEvent, Category = "JellyfinVR|UI")
	void OnItemSet();
};

/**
 * Home widget showing Continue Watching, Recently Added, Libraries
 */
UCLASS(BlueprintType, Blueprintable)
class JELLYFINVR_API UJellyfinHomeWidget : public UJellyfinVRWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	/**
	 * Refresh home data
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	void RefreshHome();

protected:
	UFUNCTION()
	void OnLibrariesLoaded(bool bSuccess, const TArray<FJellyfinLibrary>& Libraries);

	UFUNCTION()
	void OnResumeItemsLoaded(bool bSuccess, const FJellyfinItemsResult& Result);

	UFUNCTION()
	void OnLatestItemsLoaded(bool bSuccess, const FJellyfinItemsResult& Result);

	/** Called when home data is ready */
	UFUNCTION(BlueprintImplementableEvent, Category = "JellyfinVR|UI")
	void OnHomeDataReady(const TArray<FJellyfinLibrary>& InLibraries,
		const TArray<FJellyfinMediaItem>& InResumeItems,
		const TArray<FJellyfinMediaItem>& InLatestItems);

private:
	TArray<FJellyfinLibrary> LoadedLibraries;
	TArray<FJellyfinMediaItem> ResumeItems;
	TArray<FJellyfinMediaItem> LatestItems;
	int32 PendingRequests = 0;
};

/**
 * Library browser widget
 */
UCLASS(BlueprintType, Blueprintable)
class JELLYFINVR_API UJellyfinLibraryWidget : public UJellyfinVRWidget
{
	GENERATED_BODY()

public:
	/**
	 * Browse a library
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	void BrowseLibrary(const FString& LibraryId);

	/**
	 * Browse a folder/collection
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	void BrowseFolder(const FString& FolderId);

	/**
	 * Load next page of items
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	void LoadNextPage();

	virtual bool CanNavigateBack_Implementation() const override;
	virtual void NavigateBack_Implementation() override;

protected:
	UFUNCTION()
	void OnItemsLoaded(bool bSuccess, const FJellyfinItemsResult& Result);

	/** Called when items are loaded */
	UFUNCTION(BlueprintImplementableEvent, Category = "JellyfinVR|UI")
	void OnItemsReady(const TArray<FJellyfinMediaItem>& Items, int32 TotalCount, bool bHasMore);

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	TArray<FString> NavigationStack;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString CurrentFolderId;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	TArray<FJellyfinMediaItem> LoadedItems;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	int32 TotalItemCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	int32 CurrentPage = 0;

	UPROPERTY(EditDefaultsOnly, Category = "JellyfinVR")
	int32 ItemsPerPage = 50;
};

/**
 * Settings/Login widget
 */
UCLASS(BlueprintType, Blueprintable)
class JELLYFINVR_API UJellyfinSettingsWidget : public UJellyfinVRWidget
{
	GENERATED_BODY()

public:
	/**
	 * Attempt to connect with provided credentials
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	void Connect(const FString& ServerUrl, const FString& Username, const FString& Password);

	/**
	 * Disconnect from server
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	void Disconnect();

protected:
	UFUNCTION()
	void OnConnectionStateUpdated(EJellyfinAuthState NewState);

	/** Called when connection state changes */
	UFUNCTION(BlueprintImplementableEvent, Category = "JellyfinVR|UI")
	void OnConnectionResult(bool bSuccess, const FString& Message);
};

/**
 * Playback controls overlay widget
 */
UCLASS(BlueprintType, Blueprintable)
class JELLYFINVR_API UJellyfinPlayerControlsWidget : public UJellyfinVRWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/**
	 * Play/Pause toggle
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	void TogglePlayPause();

	/**
	 * Seek to position
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	void SeekTo(float Progress);

	/**
	 * Skip forward
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	void SkipForward();

	/**
	 * Skip backward
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	void SkipBackward();

	/**
	 * Show audio/subtitle selection
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	void ShowTrackSelection();

	/**
	 * Get available audio tracks
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	TArray<FJellyfinMediaStream> GetAvailableAudioTracks() const;

	/**
	 * Get available subtitle tracks
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	TArray<FJellyfinMediaStream> GetAvailableSubtitleTracks() const;

	/**
	 * Get currently selected audio track index
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	int32 GetCurrentAudioTrackIndex() const;

	/**
	 * Get currently selected subtitle track index (-1 if disabled)
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	int32 GetCurrentSubtitleTrackIndex() const;

	/**
	 * Select audio track by index
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	void SelectAudioTrack(int32 TrackIndex);

	/**
	 * Select subtitle track by index (-1 to disable)
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	void SelectSubtitleTrack(int32 TrackIndex);

	/**
	 * Get preferred audio language
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	FString GetPreferredAudioLanguage() const { return PreferredAudioLanguage; }

	/**
	 * Get preferred subtitle language
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	FString GetPreferredSubtitleLanguage() const { return PreferredSubtitleLanguage; }

	/**
	 * Set preferred audio language (e.g., "eng", "jpn")
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	void SetPreferredAudioLanguage(const FString& Language);

	/**
	 * Set preferred subtitle language
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	void SetPreferredSubtitleLanguage(const FString& Language);

	/**
	 * Apply preferred track selections (called when media starts)
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	void ApplyPreferredTracks();

protected:
	/** Called every tick with current playback info */
	UFUNCTION(BlueprintImplementableEvent, Category = "JellyfinVR|UI")
	void OnPlaybackUpdate(float Progress, const FString& CurrentTime, const FString& Duration, bool bIsPlaying);

	/** Called when track selection should be shown */
	UFUNCTION(BlueprintImplementableEvent, Category = "JellyfinVR|UI")
	void OnShowTrackSelection(const TArray<FJellyfinMediaStream>& AudioTracks, const TArray<FJellyfinMediaStream>& SubtitleTracks);

	/** Called when audio track changes */
	UFUNCTION(BlueprintImplementableEvent, Category = "JellyfinVR|UI")
	void OnAudioTrackChanged(int32 NewTrackIndex, const FJellyfinMediaStream& TrackInfo);

	/** Called when subtitle track changes */
	UFUNCTION(BlueprintImplementableEvent, Category = "JellyfinVR|UI")
	void OnSubtitleTrackChanged(int32 NewTrackIndex);

private:
	void LoadTrackPreferences();
	void SaveTrackPreferences();

	UPROPERTY()
	FString PreferredAudioLanguage = TEXT("eng");

	UPROPERTY()
	FString PreferredSubtitleLanguage = TEXT("eng");

	UPROPERTY()
	int32 CurrentAudioTrackIndex = 0;

	UPROPERTY()
	int32 CurrentSubtitleTrackIndex = -1;
};

/**
 * Search widget
 */
UCLASS(BlueprintType, Blueprintable)
class JELLYFINVR_API UJellyfinSearchWidget : public UJellyfinVRWidget
{
	GENERATED_BODY()

public:
	/**
	 * Perform search
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	void Search(const FString& Query);

	/**
	 * Clear search results
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|UI")
	void ClearSearch();

protected:
	UFUNCTION()
	void OnSearchComplete(bool bSuccess, const TArray<FJellyfinSearchHint>& Results);

	/** Called when search results are ready */
	UFUNCTION(BlueprintImplementableEvent, Category = "JellyfinVR|UI")
	void OnSearchResults(const TArray<FJellyfinSearchHint>& Results);

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	FString CurrentQuery;

	UPROPERTY(BlueprintReadOnly, Category = "JellyfinVR")
	TArray<FJellyfinSearchHint> SearchResults;
};
