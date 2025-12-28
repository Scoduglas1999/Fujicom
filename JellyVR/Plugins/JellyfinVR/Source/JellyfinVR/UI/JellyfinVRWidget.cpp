// Copyright JellyVR Project. All Rights Reserved.

#include "JellyfinVRWidget.h"
#include "JellyfinScreenActor.h"
#include "JellyfinVRModule.h"
#include "API/JellyfinClient.h"
#include "API/JellyfinAuth.h"
#include "Media/JellyfinMediaPlayer.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameUserSettings.h"

// ============ UJellyfinVRWidget ============

void UJellyfinVRWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Get Jellyfin client from auth subsystem
	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
	{
		if (UJellyfinAuthSubsystem* AuthSubsystem = GameInstance->GetSubsystem<UJellyfinAuthSubsystem>())
		{
			JellyfinClient = AuthSubsystem->GetClient();
		}
	}
}

void UJellyfinVRWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UJellyfinVRWidget::SetOwningScreen(AJellyfinScreenActor* Screen)
{
	OwningScreen = Screen;
}

UJellyfinClient* UJellyfinVRWidget::GetJellyfinClient() const
{
	return JellyfinClient;
}

void UJellyfinVRWidget::NavigateBack_Implementation()
{
	// Default implementation - can be overridden in Blueprint
}

void UJellyfinVRWidget::ShowLoading_Implementation(const FString& Message)
{
	bIsLoading = true;
}

void UJellyfinVRWidget::HideLoading_Implementation()
{
	bIsLoading = false;
}

void UJellyfinVRWidget::ShowError_Implementation(const FString& ErrorMessage)
{
	UE_LOG(LogJellyfinVR, Error, TEXT("UI Error: %s"), *ErrorMessage);
}

bool UJellyfinVRWidget::CanNavigateBack_Implementation() const
{
	return false;
}

// ============ UJellyfinMediaItemWidget ============

void UJellyfinMediaItemWidget::SetItem(const FJellyfinMediaItem& Item)
{
	MediaItem = Item;
	OnItemSet();
}

void UJellyfinMediaItemWidget::OnItemClicked_Implementation()
{
	// Default implementation - navigate or play based on item type
}

// ============ UJellyfinHomeWidget ============

void UJellyfinHomeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (JellyfinClient)
	{
		JellyfinClient->OnLibrariesLoaded.AddDynamic(this, &UJellyfinHomeWidget::OnLibrariesLoaded);
		JellyfinClient->OnItemsLoaded.AddDynamic(this, &UJellyfinHomeWidget::OnResumeItemsLoaded);
	}

	RefreshHome();
}

void UJellyfinHomeWidget::RefreshHome()
{
	if (!JellyfinClient || !JellyfinClient->IsAuthenticated())
	{
		return;
	}

	ShowLoading(TEXT("Loading..."));

	LoadedLibraries.Empty();
	ResumeItems.Empty();
	LatestItems.Empty();
	PendingRequests = 3;

	JellyfinClient->GetLibraries();
	JellyfinClient->GetResumeItems(12);
	JellyfinClient->GetLatestItems(TEXT(""), 16);
}

void UJellyfinHomeWidget::OnLibrariesLoaded(bool bSuccess, const TArray<FJellyfinLibrary>& Libraries)
{
	if (bSuccess)
	{
		LoadedLibraries = Libraries;
	}

	PendingRequests--;
	if (PendingRequests <= 0)
	{
		HideLoading();
		OnHomeDataReady(LoadedLibraries, ResumeItems, LatestItems);
	}
}

void UJellyfinHomeWidget::OnResumeItemsLoaded(bool bSuccess, const FJellyfinItemsResult& Result)
{
	if (bSuccess)
	{
		ResumeItems = Result.Items;
	}

	PendingRequests--;
	if (PendingRequests <= 0)
	{
		HideLoading();
		OnHomeDataReady(LoadedLibraries, ResumeItems, LatestItems);
	}
}

void UJellyfinHomeWidget::OnLatestItemsLoaded(bool bSuccess, const FJellyfinItemsResult& Result)
{
	if (bSuccess)
	{
		LatestItems = Result.Items;
	}

	PendingRequests--;
	if (PendingRequests <= 0)
	{
		HideLoading();
		OnHomeDataReady(LoadedLibraries, ResumeItems, LatestItems);
	}
}

// ============ UJellyfinLibraryWidget ============

void UJellyfinLibraryWidget::BrowseLibrary(const FString& LibraryId)
{
	BrowseFolder(LibraryId);
}

void UJellyfinLibraryWidget::BrowseFolder(const FString& FolderId)
{
	if (!JellyfinClient || !JellyfinClient->IsAuthenticated())
	{
		return;
	}

	// Push current to navigation stack
	if (!CurrentFolderId.IsEmpty())
	{
		NavigationStack.Push(CurrentFolderId);
	}

	CurrentFolderId = FolderId;
	CurrentPage = 0;
	LoadedItems.Empty();

	ShowLoading(TEXT("Loading..."));

	JellyfinClient->OnItemsLoaded.AddDynamic(this, &UJellyfinLibraryWidget::OnItemsLoaded);
	JellyfinClient->GetItems(FolderId, 0, ItemsPerPage);
}

void UJellyfinLibraryWidget::LoadNextPage()
{
	if (!JellyfinClient || LoadedItems.Num() >= TotalItemCount)
	{
		return;
	}

	CurrentPage++;
	JellyfinClient->GetItems(CurrentFolderId, CurrentPage * ItemsPerPage, ItemsPerPage);
}

bool UJellyfinLibraryWidget::CanNavigateBack_Implementation() const
{
	return NavigationStack.Num() > 0;
}

void UJellyfinLibraryWidget::NavigateBack_Implementation()
{
	if (NavigationStack.Num() > 0)
	{
		FString PreviousFolderId = NavigationStack.Pop();
		CurrentFolderId = TEXT("");
		BrowseFolder(PreviousFolderId);
	}
}

void UJellyfinLibraryWidget::OnItemsLoaded(bool bSuccess, const FJellyfinItemsResult& Result)
{
	HideLoading();

	if (bSuccess)
	{
		if (CurrentPage == 0)
		{
			LoadedItems = Result.Items;
		}
		else
		{
			LoadedItems.Append(Result.Items);
		}

		TotalItemCount = Result.TotalRecordCount;
		bool bHasMore = LoadedItems.Num() < TotalItemCount;

		OnItemsReady(LoadedItems, TotalItemCount, bHasMore);
	}
	else
	{
		ShowError(TEXT("Failed to load items"));
	}
}

// ============ UJellyfinSettingsWidget ============

void UJellyfinSettingsWidget::Connect(const FString& ServerUrl, const FString& Username, const FString& Password)
{
	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
	{
		if (UJellyfinAuthSubsystem* AuthSubsystem = GameInstance->GetSubsystem<UJellyfinAuthSubsystem>())
		{
			AuthSubsystem->OnConnectionStateChanged.AddDynamic(this, &UJellyfinSettingsWidget::OnConnectionStateUpdated);
			AuthSubsystem->Connect(ServerUrl, Username, Password, true);
			ShowLoading(TEXT("Connecting..."));
		}
	}
}

void UJellyfinSettingsWidget::Disconnect()
{
	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
	{
		if (UJellyfinAuthSubsystem* AuthSubsystem = GameInstance->GetSubsystem<UJellyfinAuthSubsystem>())
		{
			AuthSubsystem->Disconnect();
			OnConnectionResult(true, TEXT("Disconnected"));
		}
	}
}

void UJellyfinSettingsWidget::OnConnectionStateUpdated(EJellyfinAuthState NewState)
{
	switch (NewState)
	{
	case EJellyfinAuthState::Authenticated:
		HideLoading();
		{
			FString Username = TEXT("user");
			if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
			{
				if (UJellyfinAuthSubsystem* AuthSubsystem = GameInstance->GetSubsystem<UJellyfinAuthSubsystem>())
				{
					Username = AuthSubsystem->GetSession().Username;
				}
			}
			OnConnectionResult(true, FString::Printf(TEXT("Connected as %s"), *Username));
		}
		// Switch to home screen
		if (OwningScreen)
		{
			OwningScreen->ShowUI();
		}
		break;

	case EJellyfinAuthState::Failed:
		HideLoading();
		OnConnectionResult(false, TEXT("Connection failed. Check server URL and credentials."));
		break;

	case EJellyfinAuthState::Authenticating:
		// Still connecting, do nothing
		break;

	default:
		break;
	}
}

// ============ UJellyfinPlayerControlsWidget ============

void UJellyfinPlayerControlsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	LoadTrackPreferences();
}

void UJellyfinPlayerControlsWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (OwningScreen)
	{
		if (UJellyfinMediaPlayerComponent* Player = OwningScreen->GetMediaPlayer())
		{
			OnPlaybackUpdate(
				Player->GetProgress(),
				Player->GetCurrentTimeFormatted(),
				Player->GetDurationFormatted(),
				Player->IsPlaying()
			);
		}
	}
}

void UJellyfinPlayerControlsWidget::TogglePlayPause()
{
	if (OwningScreen)
	{
		if (UJellyfinMediaPlayerComponent* Player = OwningScreen->GetMediaPlayer())
		{
			Player->TogglePlayPause();
		}
	}
}

void UJellyfinPlayerControlsWidget::SeekTo(float Progress)
{
	if (OwningScreen)
	{
		if (UJellyfinMediaPlayerComponent* Player = OwningScreen->GetMediaPlayer())
		{
			Player->SeekToProgress(Progress);
		}
	}
}

void UJellyfinPlayerControlsWidget::SkipForward()
{
	if (OwningScreen)
	{
		if (UJellyfinMediaPlayerComponent* Player = OwningScreen->GetMediaPlayer())
		{
			Player->SeekRelative(10.0f); // Skip 10 seconds
		}
	}
}

void UJellyfinPlayerControlsWidget::SkipBackward()
{
	if (OwningScreen)
	{
		if (UJellyfinMediaPlayerComponent* Player = OwningScreen->GetMediaPlayer())
		{
			Player->SeekRelative(-10.0f); // Skip back 10 seconds
		}
	}
}

void UJellyfinPlayerControlsWidget::ShowTrackSelection()
{
	if (!OwningScreen)
	{
		return;
	}

	UJellyfinMediaPlayerComponent* Player = OwningScreen->GetMediaPlayer();
	if (!Player)
	{
		return;
	}

	// Get available tracks
	TArray<FJellyfinMediaStream> AudioTracks = Player->GetAudioTracks();
	TArray<FJellyfinMediaStream> SubtitleTracks = Player->GetSubtitleTracks();

	// Update current track indices from player state
	// The media player's selected tracks may have changed
	if (AudioTracks.Num() > 0)
	{
		// Find which audio track is currently selected
		for (int32 i = 0; i < AudioTracks.Num(); ++i)
		{
			if (AudioTracks[i].bIsDefault || i == CurrentAudioTrackIndex)
			{
				CurrentAudioTrackIndex = i;
				break;
			}
		}
	}

	UE_LOG(LogJellyfinVR, Log, TEXT("Track Selection - Audio tracks: %d, Subtitle tracks: %d"),
		AudioTracks.Num(), SubtitleTracks.Num());

	// Broadcast to Blueprint for UI implementation
	OnShowTrackSelection(AudioTracks, SubtitleTracks);
}

TArray<FJellyfinMediaStream> UJellyfinPlayerControlsWidget::GetAvailableAudioTracks() const
{
	if (OwningScreen)
	{
		if (UJellyfinMediaPlayerComponent* Player = OwningScreen->GetMediaPlayer())
		{
			return Player->GetAudioTracks();
		}
	}
	return TArray<FJellyfinMediaStream>();
}

TArray<FJellyfinMediaStream> UJellyfinPlayerControlsWidget::GetAvailableSubtitleTracks() const
{
	if (OwningScreen)
	{
		if (UJellyfinMediaPlayerComponent* Player = OwningScreen->GetMediaPlayer())
		{
			return Player->GetSubtitleTracks();
		}
	}
	return TArray<FJellyfinMediaStream>();
}

int32 UJellyfinPlayerControlsWidget::GetCurrentAudioTrackIndex() const
{
	return CurrentAudioTrackIndex;
}

int32 UJellyfinPlayerControlsWidget::GetCurrentSubtitleTrackIndex() const
{
	return CurrentSubtitleTrackIndex;
}

void UJellyfinPlayerControlsWidget::SelectAudioTrack(int32 TrackIndex)
{
	if (!OwningScreen)
	{
		return;
	}

	UJellyfinMediaPlayerComponent* Player = OwningScreen->GetMediaPlayer();
	if (!Player)
	{
		return;
	}

	TArray<FJellyfinMediaStream> AudioTracks = Player->GetAudioTracks();
	if (!AudioTracks.IsValidIndex(TrackIndex))
	{
		UE_LOG(LogJellyfinVR, Warning, TEXT("Invalid audio track index: %d"), TrackIndex);
		return;
	}

	// Set the track on the player
	Player->SetAudioTrack(TrackIndex);
	CurrentAudioTrackIndex = TrackIndex;

	// Update preferred language if user manually selects a track
	const FJellyfinMediaStream& SelectedTrack = AudioTracks[TrackIndex];
	if (!SelectedTrack.Language.IsEmpty())
	{
		SetPreferredAudioLanguage(SelectedTrack.Language);
	}

	UE_LOG(LogJellyfinVR, Log, TEXT("Selected audio track %d: %s (%s)"),
		TrackIndex, *SelectedTrack.DisplayTitle, *SelectedTrack.Language);

	// Notify Blueprint
	OnAudioTrackChanged(TrackIndex, SelectedTrack);
}

void UJellyfinPlayerControlsWidget::SelectSubtitleTrack(int32 TrackIndex)
{
	if (!OwningScreen)
	{
		return;
	}

	UJellyfinMediaPlayerComponent* Player = OwningScreen->GetMediaPlayer();
	if (!Player)
	{
		return;
	}

	// -1 is valid to disable subtitles
	if (TrackIndex >= 0)
	{
		TArray<FJellyfinMediaStream> SubtitleTracks = Player->GetSubtitleTracks();
		if (!SubtitleTracks.IsValidIndex(TrackIndex))
		{
			UE_LOG(LogJellyfinVR, Warning, TEXT("Invalid subtitle track index: %d"), TrackIndex);
			return;
		}

		// Update preferred language if user manually selects a track
		const FJellyfinMediaStream& SelectedTrack = SubtitleTracks[TrackIndex];
		if (!SelectedTrack.Language.IsEmpty())
		{
			SetPreferredSubtitleLanguage(SelectedTrack.Language);
		}

		UE_LOG(LogJellyfinVR, Log, TEXT("Selected subtitle track %d: %s (%s)"),
			TrackIndex, *SelectedTrack.DisplayTitle, *SelectedTrack.Language);
	}
	else
	{
		UE_LOG(LogJellyfinVR, Log, TEXT("Disabled subtitles"));
	}

	// Set the track on the player
	Player->SetSubtitleTrack(TrackIndex);
	CurrentSubtitleTrackIndex = TrackIndex;

	// Notify Blueprint
	OnSubtitleTrackChanged(TrackIndex);
}

void UJellyfinPlayerControlsWidget::SetPreferredAudioLanguage(const FString& Language)
{
	if (PreferredAudioLanguage != Language)
	{
		PreferredAudioLanguage = Language;
		SaveTrackPreferences();
		UE_LOG(LogJellyfinVR, Log, TEXT("Preferred audio language set to: %s"), *Language);
	}
}

void UJellyfinPlayerControlsWidget::SetPreferredSubtitleLanguage(const FString& Language)
{
	if (PreferredSubtitleLanguage != Language)
	{
		PreferredSubtitleLanguage = Language;
		SaveTrackPreferences();
		UE_LOG(LogJellyfinVR, Log, TEXT("Preferred subtitle language set to: %s"), *Language);
	}
}

void UJellyfinPlayerControlsWidget::ApplyPreferredTracks()
{
	if (!OwningScreen)
	{
		return;
	}

	UJellyfinMediaPlayerComponent* Player = OwningScreen->GetMediaPlayer();
	if (!Player)
	{
		return;
	}

	// Auto-select audio track based on preferred language
	TArray<FJellyfinMediaStream> AudioTracks = Player->GetAudioTracks();
	if (AudioTracks.Num() > 0)
	{
		int32 MatchedAudioTrack = -1;

		// First, try to find exact language match
		for (int32 i = 0; i < AudioTracks.Num(); ++i)
		{
			if (AudioTracks[i].Language.Equals(PreferredAudioLanguage, ESearchCase::IgnoreCase))
			{
				MatchedAudioTrack = i;
				break;
			}
		}

		// If no match, use the default track
		if (MatchedAudioTrack < 0)
		{
			for (int32 i = 0; i < AudioTracks.Num(); ++i)
			{
				if (AudioTracks[i].bIsDefault)
				{
					MatchedAudioTrack = i;
					break;
				}
			}
		}

		// Fall back to first track if still no match
		if (MatchedAudioTrack < 0)
		{
			MatchedAudioTrack = 0;
		}

		if (MatchedAudioTrack >= 0)
		{
			Player->SetAudioTrack(MatchedAudioTrack);
			CurrentAudioTrackIndex = MatchedAudioTrack;
			UE_LOG(LogJellyfinVR, Log, TEXT("Auto-selected audio track %d: %s"),
				MatchedAudioTrack, *AudioTracks[MatchedAudioTrack].DisplayTitle);
		}
	}

	// Auto-select subtitle track based on preferred language
	TArray<FJellyfinMediaStream> SubtitleTracks = Player->GetSubtitleTracks();
	if (SubtitleTracks.Num() > 0)
	{
		int32 MatchedSubtitleTrack = -1;

		// Try to find exact language match
		for (int32 i = 0; i < SubtitleTracks.Num(); ++i)
		{
			if (SubtitleTracks[i].Language.Equals(PreferredSubtitleLanguage, ESearchCase::IgnoreCase))
			{
				MatchedSubtitleTrack = i;
				break;
			}
		}

		// If no match and user has a preference, disable subtitles
		// Otherwise, check for default subtitle
		if (MatchedSubtitleTrack < 0)
		{
			for (int32 i = 0; i < SubtitleTracks.Num(); ++i)
			{
				if (SubtitleTracks[i].bIsDefault || SubtitleTracks[i].bIsForced)
				{
					MatchedSubtitleTrack = i;
					break;
				}
			}
		}

		// Set the matched subtitle track or disable if no match
		Player->SetSubtitleTrack(MatchedSubtitleTrack);
		CurrentSubtitleTrackIndex = MatchedSubtitleTrack;

		if (MatchedSubtitleTrack >= 0)
		{
			UE_LOG(LogJellyfinVR, Log, TEXT("Auto-selected subtitle track %d: %s"),
				MatchedSubtitleTrack, *SubtitleTracks[MatchedSubtitleTrack].DisplayTitle);
		}
		else
		{
			UE_LOG(LogJellyfinVR, Log, TEXT("No matching subtitle track found, subtitles disabled"));
		}
	}
}

void UJellyfinPlayerControlsWidget::LoadTrackPreferences()
{
	// Load preferences from config (Config/DefaultGameUserSettings.ini)
	if (GConfig)
	{
		GConfig->GetString(TEXT("JellyfinVR.TrackPreferences"), TEXT("PreferredAudioLanguage"),
			PreferredAudioLanguage, GGameUserSettingsIni);
		GConfig->GetString(TEXT("JellyfinVR.TrackPreferences"), TEXT("PreferredSubtitleLanguage"),
			PreferredSubtitleLanguage, GGameUserSettingsIni);
	}

	// Set defaults if empty
	if (PreferredAudioLanguage.IsEmpty())
	{
		PreferredAudioLanguage = TEXT("eng");
	}
	if (PreferredSubtitleLanguage.IsEmpty())
	{
		PreferredSubtitleLanguage = TEXT("eng");
	}

	UE_LOG(LogJellyfinVR, Log, TEXT("Loaded track preferences - Audio: %s, Subtitle: %s"),
		*PreferredAudioLanguage, *PreferredSubtitleLanguage);
}

void UJellyfinPlayerControlsWidget::SaveTrackPreferences()
{
	// Save preferences to config
	if (GConfig)
	{
		GConfig->SetString(TEXT("JellyfinVR.TrackPreferences"), TEXT("PreferredAudioLanguage"),
			*PreferredAudioLanguage, GGameUserSettingsIni);
		GConfig->SetString(TEXT("JellyfinVR.TrackPreferences"), TEXT("PreferredSubtitleLanguage"),
			*PreferredSubtitleLanguage, GGameUserSettingsIni);
		GConfig->Flush(false, GGameUserSettingsIni);

		UE_LOG(LogJellyfinVR, Log, TEXT("Saved track preferences - Audio: %s, Subtitle: %s"),
			*PreferredAudioLanguage, *PreferredSubtitleLanguage);
	}
}

// ============ UJellyfinSearchWidget ============

void UJellyfinSearchWidget::Search(const FString& Query)
{
	if (!JellyfinClient || Query.IsEmpty())
	{
		return;
	}

	CurrentQuery = Query;
	ShowLoading(TEXT("Searching..."));

	JellyfinClient->OnSearchComplete.AddDynamic(this, &UJellyfinSearchWidget::OnSearchComplete);
	JellyfinClient->Search(Query, 30);
}

void UJellyfinSearchWidget::ClearSearch()
{
	CurrentQuery.Empty();
	SearchResults.Empty();
	OnSearchResults(SearchResults);
}

void UJellyfinSearchWidget::OnSearchComplete(bool bSuccess, const TArray<FJellyfinSearchHint>& Results)
{
	HideLoading();

	if (bSuccess)
	{
		SearchResults = Results;
		OnSearchResults(Results);
	}
	else
	{
		ShowError(TEXT("Search failed"));
	}
}
