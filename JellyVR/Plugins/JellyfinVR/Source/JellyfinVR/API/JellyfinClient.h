// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "JellyfinTypes.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "JellyfinClient.generated.h"

/**
 * Network error categories for API requests
 */
UENUM(BlueprintType)
enum class EJellyfinErrorType : uint8
{
	None,
	NetworkTimeout,
	ServerError,      // 5xx errors
	AuthError,        // 401/403 errors
	NotFound,         // 404 error
	BadRequest,       // 400 and other 4xx errors
	ParseError,       // JSON parsing failure
	ConnectionFailed, // Cannot reach server
	Unknown
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnJellyfinRequestComplete, bool, bSuccess, const FString&, ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnJellyfinAuthComplete, bool, bSuccess, const FJellyfinUserSession&, Session);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnJellyfinLibrariesLoaded, bool, bSuccess, const TArray<FJellyfinLibrary>&, Libraries);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnJellyfinItemsLoaded, bool, bSuccess, const FJellyfinItemsResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnJellyfinItemLoaded, bool, bSuccess, const FJellyfinMediaItem&, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnJellyfinPlaybackInfoLoaded, bool, bSuccess, const FJellyfinPlaybackInfo&, PlaybackInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnJellyfinSearchComplete, bool, bSuccess, const TArray<FJellyfinSearchHint>&, Results);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnJellyfinImageLoaded, bool, bSuccess, UTexture2D*, Texture);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnJellyfinRequestFailed, EJellyfinErrorType, ErrorType, const FString&, ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJellyfinConnectivityChanged, bool, bIsOnline);

/**
 * Main Jellyfin API client
 * Handles all HTTP communication with the Jellyfin server
 */
UCLASS(BlueprintType, Blueprintable)
class JELLYFINVR_API UJellyfinClient : public UObject
{
	GENERATED_BODY()

public:
	UJellyfinClient();

	/**
	 * Initialize the client with server settings
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Client")
	void Initialize(const FJellyfinServerSettings& Settings);

	/**
	 * Authenticate with username and password
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Auth")
	void Authenticate(const FString& Username, const FString& Password);

	/**
	 * Authenticate with stored token
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Auth")
	void AuthenticateWithToken(const FString& UserId, const FString& AccessToken);

	/**
	 * Validate current token with server
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Auth")
	void ValidateToken();

	/**
	 * Log out and clear session
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Auth")
	void Logout();

	/**
	 * Check if currently authenticated
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Auth")
	bool IsAuthenticated() const;

	/**
	 * Check if online (based on recent request success/failure)
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Auth")
	bool IsOnline() const { return bIsOnline; }

	/**
	 * Get current session info
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Auth")
	const FJellyfinUserSession& GetSession() const { return CurrentSession; }

	// ============ Library Operations ============

	/**
	 * Get user's library views (Movies, TV Shows, etc.)
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Library")
	void GetLibraries();

	/**
	 * Get items in a library or folder
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Library")
	void GetItems(const FString& ParentId, int32 StartIndex = 0, int32 Limit = 500,
		const FString& SortBy = TEXT("SortName"), bool bSortDescending = false);

	/**
	 * Get resume/continue watching items
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Library")
	void GetResumeItems(int32 Limit = 12);

	/**
	 * Get recently added items
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Library")
	void GetLatestItems(const FString& ParentId = TEXT(""), int32 Limit = 16);

	/**
	 * Get next up episodes for TV shows
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Library")
	void GetNextUp(int32 Limit = 12);

	/**
	 * Get seasons for a series
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Library")
	void GetSeasons(const FString& SeriesId);

	/**
	 * Get episodes for a series/season
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Library")
	void GetEpisodes(const FString& SeriesId, const FString& SeasonId = TEXT(""));

	/**
	 * Get full item details
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Library")
	void GetItemDetails(const FString& ItemId);

	/**
	 * Search for items
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Library")
	void Search(const FString& SearchTerm, int32 Limit = 20);

	// ============ Playback Operations ============

	/**
	 * Get playback info for an item (stream URL, transcoding options, etc.)
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Playback")
	void GetPlaybackInfo(const FString& ItemId);

	/**
	 * Report playback start
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Playback")
	void ReportPlaybackStart(const FString& ItemId, const FString& MediaSourceId, const FString& PlaySessionId);

	/**
	 * Report playback progress
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Playback")
	void ReportPlaybackProgress(const FString& ItemId, const FString& MediaSourceId,
		const FString& PlaySessionId, int64 PositionTicks, bool bIsPaused = false);

	/**
	 * Report playback stopped
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Playback")
	void ReportPlaybackStopped(const FString& ItemId, const FString& MediaSourceId,
		const FString& PlaySessionId, int64 PositionTicks);

	/**
	 * Mark item as played
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Playback")
	void MarkPlayed(const FString& ItemId);

	/**
	 * Mark item as unplayed
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Playback")
	void MarkUnplayed(const FString& ItemId);

	// ============ Image Operations ============

	/**
	 * Get image URL for an item
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Images")
	FString GetImageUrl(const FString& ItemId, const FString& ImageType = TEXT("Primary"),
		int32 MaxWidth = 400, int32 MaxHeight = 600) const;

	/**
	 * Load image as texture (async)
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Images")
	void LoadImageTexture(const FString& ItemId, const FString& ImageType = TEXT("Primary"),
		int32 MaxWidth = 400, int32 MaxHeight = 600);

	// ============ Events ============

	UPROPERTY(BlueprintAssignable, Category = "JellyfinVR|Events")
	FOnJellyfinAuthComplete OnAuthComplete;

	UPROPERTY(BlueprintAssignable, Category = "JellyfinVR|Events")
	FOnJellyfinLibrariesLoaded OnLibrariesLoaded;

	UPROPERTY(BlueprintAssignable, Category = "JellyfinVR|Events")
	FOnJellyfinItemsLoaded OnItemsLoaded;

	UPROPERTY(BlueprintAssignable, Category = "JellyfinVR|Events")
	FOnJellyfinItemLoaded OnItemLoaded;

	UPROPERTY(BlueprintAssignable, Category = "JellyfinVR|Events")
	FOnJellyfinPlaybackInfoLoaded OnPlaybackInfoLoaded;

	UPROPERTY(BlueprintAssignable, Category = "JellyfinVR|Events")
	FOnJellyfinSearchComplete OnSearchComplete;

	UPROPERTY(BlueprintAssignable, Category = "JellyfinVR|Events")
	FOnJellyfinImageLoaded OnImageLoaded;

	UPROPERTY(BlueprintAssignable, Category = "JellyfinVR|Events")
	FOnJellyfinRequestFailed OnRequestFailed;

	UPROPERTY(BlueprintAssignable, Category = "JellyfinVR|Events")
	FOnJellyfinConnectivityChanged OnConnectivityChanged;

protected:
	// Retry context for failed requests
	struct FRetryContext
	{
		FString Verb;
		FString Endpoint;
		FString ContentBody;
		int32 RetryCount = 0;
		TFunction<void(FHttpRequestPtr, FHttpResponsePtr, bool)> ResponseHandler;
		bool bRequiresAuth = true;
	};

	// HTTP request helpers
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> CreateRequest(const FString& Verb, const FString& Endpoint);
	void AddAuthHeaders(TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request);
	FString GetClientIdentifier() const;

	// Error handling and retry logic
	EJellyfinErrorType CategorizeError(FHttpResponsePtr Response, bool bWasSuccessful) const;
	bool ShouldRetryRequest(EJellyfinErrorType ErrorType, int32 RetryCount) const;
	void ScheduleRetry(TSharedPtr<FRetryContext> Context);
	void ExecuteRetry(TSharedPtr<FRetryContext> Context);
	void HandleRequestFailure(EJellyfinErrorType ErrorType, const FString& ErrorMessage);
	void UpdateConnectivityState(bool bNewState);

	// Token refresh
	void RefreshToken();
	void HandleTokenRefreshResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// Response handlers
	void HandleAuthResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void HandleTokenValidationResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void HandleLibrariesResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void HandleItemsResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void HandleItemDetailsResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void HandlePlaybackInfoResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void HandleSearchResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void HandleImageResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// JSON parsing helpers
	FJellyfinMediaItem ParseMediaItem(const TSharedPtr<FJsonObject>& JsonObject);
	FJellyfinMediaStream ParseMediaStream(const TSharedPtr<FJsonObject>& JsonObject);
	FJellyfinChapter ParseChapter(const TSharedPtr<FJsonObject>& JsonObject);
	EJellyfinItemType ParseItemType(const FString& TypeString);

private:
	UPROPERTY()
	FJellyfinServerSettings ServerSettings;

	UPROPERTY()
	FJellyfinUserSession CurrentSession;

	// Unique device ID for this installation
	FString DeviceId;
	FString DeviceName;

	// Image texture cache
	UPROPERTY()
	TMap<FString, UTexture2D*> ImageCache;

	// Connectivity tracking
	bool bIsOnline = true;
	int32 ConsecutiveFailures = 0;
	static constexpr int32 MaxConsecutiveFailuresForOffline = 3;

	// Retry tracking
	TMap<FTimerHandle, TSharedPtr<FRetryContext>> PendingRetries;
	static constexpr int32 MaxRetryAttempts = 3;
	static constexpr float BaseRetryDelay = 1.0f; // seconds
};
