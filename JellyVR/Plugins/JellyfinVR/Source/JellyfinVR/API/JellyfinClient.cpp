// Copyright JellyVR Project. All Rights Reserved.

#include "JellyfinClient.h"
#include "JellyfinVRModule.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Misc/Guid.h"
#include "Misc/Base64.h"
#include "Http.h"
#include "Engine/Texture2D.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#include "TimerManager.h"
#include "Engine/World.h"

UJellyfinClient::UJellyfinClient()
{
	// Generate a unique device ID for this installation
	DeviceId = FGuid::NewGuid().ToString();

	// Get device name
#if PLATFORM_ANDROID
	DeviceName = TEXT("Quest 3");
#else
	DeviceName = FPlatformProcess::ComputerName();
#endif
}

void UJellyfinClient::Initialize(const FJellyfinServerSettings& Settings)
{
	ServerSettings = Settings;

	// Ensure URL has http:// or https:// protocol
	if (!ServerSettings.ServerUrl.StartsWith(TEXT("http://")) && !ServerSettings.ServerUrl.StartsWith(TEXT("https://")))
	{
		ServerSettings.ServerUrl = TEXT("http://") + ServerSettings.ServerUrl;
	}

	// Ensure URL doesn't have trailing slash
	if (ServerSettings.ServerUrl.EndsWith(TEXT("/")))
	{
		ServerSettings.ServerUrl.RemoveFromEnd(TEXT("/"));
	}

	UE_LOG(LogJellyfinVR, Log, TEXT("JellyfinClient initialized with server: %s"), *ServerSettings.ServerUrl);
}

FString UJellyfinClient::GetClientIdentifier() const
{
	// Build the authorization header value
	return FString::Printf(
		TEXT("MediaBrowser Client=\"JellyVR\", Device=\"%s\", DeviceId=\"%s\", Version=\"1.0.0\""),
		*DeviceName, *DeviceId
	);
}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UJellyfinClient::CreateRequest(const FString& Verb, const FString& Endpoint)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

	FString Url = ServerSettings.ServerUrl + Endpoint;
	Request->SetURL(Url);
	Request->SetVerb(Verb);
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Accept"), TEXT("application/json"));
	Request->SetHeader(TEXT("X-Emby-Authorization"), GetClientIdentifier());

	return Request;
}

void UJellyfinClient::AddAuthHeaders(TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request)
{
	if (CurrentSession.IsValid())
	{
		FString AuthValue = FString::Printf(
			TEXT("MediaBrowser Client=\"JellyVR\", Device=\"%s\", DeviceId=\"%s\", Version=\"1.0.0\", Token=\"%s\""),
			*DeviceName, *DeviceId, *CurrentSession.AccessToken
		);
		Request->SetHeader(TEXT("X-Emby-Authorization"), AuthValue);
	}
}

void UJellyfinClient::Authenticate(const FString& Username, const FString& Password)
{
	CurrentSession.AuthState = EJellyfinAuthState::Authenticating;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateRequest(TEXT("POST"), TEXT("/Users/AuthenticateByName"));

	// Build auth request body
	TSharedPtr<FJsonObject> JsonBody = MakeShareable(new FJsonObject());
	JsonBody->SetStringField(TEXT("Username"), Username);
	JsonBody->SetStringField(TEXT("Pw"), Password);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonBody.ToSharedRef(), Writer);

	Request->SetContentAsString(RequestBody);
	Request->OnProcessRequestComplete().BindUObject(this, &UJellyfinClient::HandleAuthResponse);
	Request->ProcessRequest();

	UE_LOG(LogJellyfinVR, Log, TEXT("Authenticating as user: %s"), *Username);
}

void UJellyfinClient::AuthenticateWithToken(const FString& UserId, const FString& AccessToken)
{
	CurrentSession.UserId = UserId;
	CurrentSession.AccessToken = AccessToken;
	CurrentSession.AuthState = EJellyfinAuthState::Authenticating;

	// Validate token with server before confirming authentication
	ValidateToken();
}

void UJellyfinClient::ValidateToken()
{
	if (CurrentSession.UserId.IsEmpty() || CurrentSession.AccessToken.IsEmpty())
	{
		UE_LOG(LogJellyfinVR, Warning, TEXT("ValidateToken called with empty credentials"));
		CurrentSession.AuthState = EJellyfinAuthState::Failed;
		OnAuthComplete.Broadcast(false, CurrentSession);
		return;
	}

	// Use lightweight /Users/Me endpoint to validate token
	FString Endpoint = TEXT("/Users/Me");
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateRequest(TEXT("GET"), Endpoint);
	AddAuthHeaders(Request);
	Request->OnProcessRequestComplete().BindUObject(this, &UJellyfinClient::HandleTokenValidationResponse);
	Request->ProcessRequest();

	UE_LOG(LogJellyfinVR, Log, TEXT("Validating token for user: %s"), *CurrentSession.UserId);
}

void UJellyfinClient::Logout()
{
	if (CurrentSession.IsValid())
	{
		// Optionally notify server of logout
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateRequest(TEXT("POST"), TEXT("/Sessions/Logout"));
		AddAuthHeaders(Request);
		Request->ProcessRequest();
	}

	CurrentSession = FJellyfinUserSession();
	UE_LOG(LogJellyfinVR, Log, TEXT("Logged out"));
}

bool UJellyfinClient::IsAuthenticated() const
{
	return CurrentSession.AuthState == EJellyfinAuthState::Authenticated && CurrentSession.IsValid();
}

// ============ Error Handling & Retry Logic ============

EJellyfinErrorType UJellyfinClient::CategorizeError(FHttpResponsePtr Response, bool bWasSuccessful) const
{
	if (!bWasSuccessful)
	{
		return EJellyfinErrorType::ConnectionFailed;
	}

	if (!Response.IsValid())
	{
		return EJellyfinErrorType::NetworkTimeout;
	}

	int32 ResponseCode = Response->GetResponseCode();

	// Categorize by HTTP status code
	if (ResponseCode == 401 || ResponseCode == 403)
	{
		return EJellyfinErrorType::AuthError;
	}
	else if (ResponseCode == 404)
	{
		return EJellyfinErrorType::NotFound;
	}
	else if (ResponseCode >= 400 && ResponseCode < 500)
	{
		return EJellyfinErrorType::BadRequest;
	}
	else if (ResponseCode >= 500)
	{
		return EJellyfinErrorType::ServerError;
	}
	else if (ResponseCode == 200)
	{
		return EJellyfinErrorType::None;
	}

	return EJellyfinErrorType::Unknown;
}

bool UJellyfinClient::ShouldRetryRequest(EJellyfinErrorType ErrorType, int32 RetryCount) const
{
	// Don't retry if we've exceeded max attempts
	if (RetryCount >= MaxRetryAttempts)
	{
		return false;
	}

	// Retry on transient failures
	switch (ErrorType)
	{
		case EJellyfinErrorType::NetworkTimeout:
		case EJellyfinErrorType::ServerError:
		case EJellyfinErrorType::ConnectionFailed:
			return true;

		// Special case: retry 401 once to attempt token refresh
		case EJellyfinErrorType::AuthError:
			return RetryCount == 0;

		// Don't retry client errors (except auth)
		case EJellyfinErrorType::NotFound:
		case EJellyfinErrorType::BadRequest:
		case EJellyfinErrorType::ParseError:
		default:
			return false;
	}
}

void UJellyfinClient::ScheduleRetry(TSharedPtr<FRetryContext> Context)
{
	if (!Context.IsValid())
	{
		return;
	}

	// Calculate exponential backoff delay: 1s, 2s, 4s
	float Delay = BaseRetryDelay * FMath::Pow(2.0f, Context->RetryCount);

	// Add jitter to prevent thundering herd
	float Jitter = FMath::FRandRange(-0.2f, 0.2f) * Delay;
	Delay += Jitter;

	UE_LOG(LogJellyfinVR, Log, TEXT("Scheduling retry %d/%d in %.2f seconds for %s %s"),
		Context->RetryCount + 1, MaxRetryAttempts, Delay, *Context->Verb, *Context->Endpoint);

	// Schedule retry using timer
	if (UWorld* World = GetWorld())
	{
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(
			TimerHandle,
			[this, Context]()
			{
				ExecuteRetry(Context);
			},
			Delay,
			false
		);

		PendingRetries.Add(TimerHandle, Context);
	}
}

void UJellyfinClient::ExecuteRetry(TSharedPtr<FRetryContext> Context)
{
	if (!Context.IsValid())
	{
		return;
	}

	Context->RetryCount++;

	UE_LOG(LogJellyfinVR, Log, TEXT("Executing retry %d for %s %s"),
		Context->RetryCount, *Context->Verb, *Context->Endpoint);

	// Create new request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateRequest(Context->Verb, Context->Endpoint);

	if (Context->bRequiresAuth)
	{
		AddAuthHeaders(Request);
	}

	if (!Context->ContentBody.IsEmpty())
	{
		Request->SetContentAsString(Context->ContentBody);
	}

	// Bind response handler
	if (Context->ResponseHandler)
	{
		Request->OnProcessRequestComplete().BindLambda(Context->ResponseHandler);
	}

	Request->ProcessRequest();
}

void UJellyfinClient::HandleRequestFailure(EJellyfinErrorType ErrorType, const FString& ErrorMessage)
{
	UE_LOG(LogJellyfinVR, Warning, TEXT("Request failed: %s (Type: %d)"), *ErrorMessage, (int32)ErrorType);

	// Broadcast failure event
	OnRequestFailed.Broadcast(ErrorType, ErrorMessage);

	// Track consecutive failures for offline detection
	ConsecutiveFailures++;

	if (ConsecutiveFailures >= MaxConsecutiveFailuresForOffline)
	{
		UpdateConnectivityState(false);
	}
}

void UJellyfinClient::UpdateConnectivityState(bool bNewState)
{
	if (bIsOnline != bNewState)
	{
		bIsOnline = bNewState;
		UE_LOG(LogJellyfinVR, Warning, TEXT("Connectivity state changed: %s"),
			bIsOnline ? TEXT("Online") : TEXT("Offline"));
		OnConnectivityChanged.Broadcast(bIsOnline);
	}

	// Reset failure counter on successful connection
	if (bNewState)
	{
		ConsecutiveFailures = 0;
	}
}

void UJellyfinClient::RefreshToken()
{
	// Jellyfin doesn't have a built-in token refresh endpoint
	// The tokens are long-lived, so when they expire, we need to re-authenticate
	UE_LOG(LogJellyfinVR, Warning, TEXT("Token expired or invalid - re-authentication required"));

	CurrentSession.AuthState = EJellyfinAuthState::Failed;
	CurrentSession = FJellyfinUserSession(); // Clear session

	OnAuthComplete.Broadcast(false, CurrentSession);
}

void UJellyfinClient::HandleTokenRefreshResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	// Placeholder for future token refresh implementation
	// Currently, Jellyfin uses long-lived tokens that don't have refresh mechanism
}

// ============ Response Handlers ============

void UJellyfinClient::HandleTokenValidationResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	EJellyfinErrorType ErrorType = CategorizeError(Response, bWasSuccessful);

	if (ErrorType == EJellyfinErrorType::None)
	{
		// Token is valid
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			// Update session with user info from validation
			CurrentSession.Username = JsonObject->GetStringField(TEXT("Name"));
			CurrentSession.AuthState = EJellyfinAuthState::Authenticated;

			UE_LOG(LogJellyfinVR, Log, TEXT("Token validated successfully for user: %s"), *CurrentSession.Username);

			// Mark as online after successful validation
			UpdateConnectivityState(true);

			OnAuthComplete.Broadcast(true, CurrentSession);
			return;
		}
	}

	// Token validation failed
	UE_LOG(LogJellyfinVR, Warning, TEXT("Token validation failed - clearing stored credentials"));
	CurrentSession.AuthState = EJellyfinAuthState::Failed;
	CurrentSession = FJellyfinUserSession(); // Clear invalid session

	OnAuthComplete.Broadcast(false, CurrentSession);
}

void UJellyfinClient::HandleAuthResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	EJellyfinErrorType ErrorType = CategorizeError(Response, bWasSuccessful);

	if (ErrorType != EJellyfinErrorType::None)
	{
		CurrentSession.AuthState = EJellyfinAuthState::Failed;

		FString ErrorMessage = FString::Printf(TEXT("Authentication failed: %d"),
			Response.IsValid() ? Response->GetResponseCode() : 0);

		HandleRequestFailure(ErrorType, ErrorMessage);
		OnAuthComplete.Broadcast(false, CurrentSession);
		UE_LOG(LogJellyfinVR, Error, TEXT("Authentication request failed: %s"), *ErrorMessage);
		return;
	}

	// Parse response
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		CurrentSession.AuthState = EJellyfinAuthState::Failed;
		HandleRequestFailure(EJellyfinErrorType::ParseError, TEXT("Failed to parse auth response"));
		OnAuthComplete.Broadcast(false, CurrentSession);
		UE_LOG(LogJellyfinVR, Error, TEXT("Failed to parse auth response"));
		return;
	}

	// Extract session info
	CurrentSession.AccessToken = JsonObject->GetStringField(TEXT("AccessToken"));
	CurrentSession.ServerId = JsonObject->GetStringField(TEXT("ServerId"));

	if (TSharedPtr<FJsonObject> UserObject = JsonObject->GetObjectField(TEXT("User")))
	{
		CurrentSession.UserId = UserObject->GetStringField(TEXT("Id"));
		CurrentSession.Username = UserObject->GetStringField(TEXT("Name"));
	}

	if (TSharedPtr<FJsonObject> SessionObject = JsonObject->GetObjectField(TEXT("SessionInfo")))
	{
		// Additional session info if needed
	}

	CurrentSession.AuthState = EJellyfinAuthState::Authenticated;

	// Mark as online after successful authentication
	UpdateConnectivityState(true);

	OnAuthComplete.Broadcast(true, CurrentSession);

	UE_LOG(LogJellyfinVR, Log, TEXT("Successfully authenticated as: %s"), *CurrentSession.Username);
}

// ============ Library Operations ============

void UJellyfinClient::GetLibraries()
{
	if (!IsAuthenticated())
	{
		UE_LOG(LogJellyfinVR, Warning, TEXT("GetLibraries called but not authenticated"));
		return;
	}

	FString Endpoint = FString::Printf(TEXT("/Users/%s/Views"), *CurrentSession.UserId);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateRequest(TEXT("GET"), Endpoint);
	AddAuthHeaders(Request);
	Request->OnProcessRequestComplete().BindUObject(this, &UJellyfinClient::HandleLibrariesResponse);
	Request->ProcessRequest();
}

void UJellyfinClient::HandleLibrariesResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	TArray<FJellyfinLibrary> Libraries;

	EJellyfinErrorType ErrorType = CategorizeError(Response, bWasSuccessful);

	if (ErrorType != EJellyfinErrorType::None)
	{
		// Handle 401 errors with token refresh
		if (ErrorType == EJellyfinErrorType::AuthError)
		{
			RefreshToken();
		}

		FString ErrorMessage = FString::Printf(TEXT("Failed to load libraries: %d"),
			Response.IsValid() ? Response->GetResponseCode() : 0);
		HandleRequestFailure(ErrorType, ErrorMessage);

		OnLibrariesLoaded.Broadcast(false, Libraries);
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		HandleRequestFailure(EJellyfinErrorType::ParseError, TEXT("Failed to parse libraries response"));
		OnLibrariesLoaded.Broadcast(false, Libraries);
		return;
	}

	// Mark as online after successful response
	UpdateConnectivityState(true);

	const TArray<TSharedPtr<FJsonValue>>* ItemsArray;
	if (JsonObject->TryGetArrayField(TEXT("Items"), ItemsArray))
	{
		for (const TSharedPtr<FJsonValue>& Value : *ItemsArray)
		{
			TSharedPtr<FJsonObject> ItemObj = Value->AsObject();
			if (ItemObj.IsValid())
			{
				FJellyfinLibrary Library;
				Library.Id = ItemObj->GetStringField(TEXT("Id"));
				Library.Name = ItemObj->GetStringField(TEXT("Name"));
				Library.CollectionType = ItemObj->GetStringField(TEXT("CollectionType"));

				if (ItemObj->HasField(TEXT("ImageTags")))
				{
					TSharedPtr<FJsonObject> ImageTags = ItemObj->GetObjectField(TEXT("ImageTags"));
					if (ImageTags.IsValid())
					{
						Library.PrimaryImageTag = ImageTags->GetStringField(TEXT("Primary"));
					}
				}

				Libraries.Add(Library);
			}
		}
	}

	OnLibrariesLoaded.Broadcast(true, Libraries);
}

void UJellyfinClient::GetItems(const FString& ParentId, int32 StartIndex, int32 Limit,
	const FString& SortBy, bool bSortDescending)
{
	if (!IsAuthenticated())
	{
		return;
	}

	// Note: Do NOT use Recursive=true - we want only direct children
	// For TV Shows library: returns Series (not all Episodes)
	// For a Series: returns Seasons
	// For a Season: returns Episodes
	// For Movie folders: returns Movies and sub-folders
	FString Endpoint = FString::Printf(
		TEXT("/Users/%s/Items?ParentId=%s&StartIndex=%d&Limit=%d&SortBy=%s&SortOrder=%s&Fields=Overview,MediaStreams,Chapters,Path,DateCreated,PremiereDate,ProviderIds,ImageTags,SeriesName,SeasonName"),
		*CurrentSession.UserId, *ParentId, StartIndex, Limit, *SortBy,
		bSortDescending ? TEXT("Descending") : TEXT("Ascending")
	);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateRequest(TEXT("GET"), Endpoint);
	AddAuthHeaders(Request);
	Request->OnProcessRequestComplete().BindUObject(this, &UJellyfinClient::HandleItemsResponse);
	Request->ProcessRequest();
}

void UJellyfinClient::GetResumeItems(int32 Limit)
{
	if (!IsAuthenticated())
	{
		return;
	}

	FString Endpoint = FString::Printf(
		TEXT("/Users/%s/Items/Resume?Limit=%d&Fields=Overview,MediaStreams&MediaTypes=Video"),
		*CurrentSession.UserId, Limit
	);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateRequest(TEXT("GET"), Endpoint);
	AddAuthHeaders(Request);
	Request->OnProcessRequestComplete().BindUObject(this, &UJellyfinClient::HandleItemsResponse);
	Request->ProcessRequest();
}

void UJellyfinClient::GetLatestItems(const FString& ParentId, int32 Limit)
{
	if (!IsAuthenticated())
	{
		return;
	}

	FString Endpoint = FString::Printf(
		TEXT("/Users/%s/Items/Latest?Limit=%d&Fields=Overview,MediaStreams"),
		*CurrentSession.UserId, Limit
	);

	if (!ParentId.IsEmpty())
	{
		Endpoint += FString::Printf(TEXT("&ParentId=%s"), *ParentId);
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateRequest(TEXT("GET"), Endpoint);
	AddAuthHeaders(Request);
	Request->OnProcessRequestComplete().BindUObject(this, &UJellyfinClient::HandleItemsResponse);
	Request->ProcessRequest();
}

void UJellyfinClient::GetNextUp(int32 Limit)
{
	if (!IsAuthenticated())
	{
		return;
	}

	FString Endpoint = FString::Printf(
		TEXT("/Shows/NextUp?UserId=%s&Limit=%d&Fields=Overview,MediaStreams"),
		*CurrentSession.UserId, Limit
	);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateRequest(TEXT("GET"), Endpoint);
	AddAuthHeaders(Request);
	Request->OnProcessRequestComplete().BindUObject(this, &UJellyfinClient::HandleItemsResponse);
	Request->ProcessRequest();
}

void UJellyfinClient::GetSeasons(const FString& SeriesId)
{
	if (!IsAuthenticated())
	{
		return;
	}

	FString Endpoint = FString::Printf(
		TEXT("/Shows/%s/Seasons?UserId=%s&Fields=Overview"),
		*SeriesId, *CurrentSession.UserId
	);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateRequest(TEXT("GET"), Endpoint);
	AddAuthHeaders(Request);
	Request->OnProcessRequestComplete().BindUObject(this, &UJellyfinClient::HandleItemsResponse);
	Request->ProcessRequest();
}

void UJellyfinClient::GetEpisodes(const FString& SeriesId, const FString& SeasonId)
{
	if (!IsAuthenticated())
	{
		return;
	}

	FString Endpoint = FString::Printf(
		TEXT("/Shows/%s/Episodes?UserId=%s&Fields=Overview,MediaStreams"),
		*SeriesId, *CurrentSession.UserId
	);

	if (!SeasonId.IsEmpty())
	{
		Endpoint += FString::Printf(TEXT("&SeasonId=%s"), *SeasonId);
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateRequest(TEXT("GET"), Endpoint);
	AddAuthHeaders(Request);
	Request->OnProcessRequestComplete().BindUObject(this, &UJellyfinClient::HandleItemsResponse);
	Request->ProcessRequest();
}

void UJellyfinClient::GetItemDetails(const FString& ItemId)
{
	if (!IsAuthenticated())
	{
		return;
	}

	FString Endpoint = FString::Printf(
		TEXT("/Users/%s/Items/%s"),
		*CurrentSession.UserId, *ItemId
	);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateRequest(TEXT("GET"), Endpoint);
	AddAuthHeaders(Request);
	Request->OnProcessRequestComplete().BindUObject(this, &UJellyfinClient::HandleItemDetailsResponse);
	Request->ProcessRequest();
}

void UJellyfinClient::Search(const FString& SearchTerm, int32 Limit)
{
	if (!IsAuthenticated())
	{
		return;
	}

	FString EncodedTerm = FPlatformHttp::UrlEncode(SearchTerm);
	FString Endpoint = FString::Printf(
		TEXT("/Search/Hints?searchTerm=%s&Limit=%d&UserId=%s&IncludeItemTypes=Movie,Series,Episode"),
		*EncodedTerm, Limit, *CurrentSession.UserId
	);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateRequest(TEXT("GET"), Endpoint);
	AddAuthHeaders(Request);
	Request->OnProcessRequestComplete().BindUObject(this, &UJellyfinClient::HandleSearchResponse);
	Request->ProcessRequest();
}

void UJellyfinClient::HandleItemsResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FJellyfinItemsResult Result;

	EJellyfinErrorType ErrorType = CategorizeError(Response, bWasSuccessful);

	if (ErrorType != EJellyfinErrorType::None)
	{
		if (ErrorType == EJellyfinErrorType::AuthError)
		{
			RefreshToken();
		}

		FString ErrorMessage = FString::Printf(TEXT("Failed to load items: %d"),
			Response.IsValid() ? Response->GetResponseCode() : 0);
		HandleRequestFailure(ErrorType, ErrorMessage);

		OnItemsLoaded.Broadcast(false, Result);
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		HandleRequestFailure(EJellyfinErrorType::ParseError, TEXT("Failed to parse items response"));
		OnItemsLoaded.Broadcast(false, Result);
		return;
	}

	UpdateConnectivityState(true);

	Result.TotalRecordCount = JsonObject->GetIntegerField(TEXT("TotalRecordCount"));
	Result.StartIndex = JsonObject->GetIntegerField(TEXT("StartIndex"));

	const TArray<TSharedPtr<FJsonValue>>* ItemsArray;
	if (JsonObject->TryGetArrayField(TEXT("Items"), ItemsArray))
	{
		for (const TSharedPtr<FJsonValue>& Value : *ItemsArray)
		{
			TSharedPtr<FJsonObject> ItemObj = Value->AsObject();
			if (ItemObj.IsValid())
			{
				Result.Items.Add(ParseMediaItem(ItemObj));
			}
		}
	}

	OnItemsLoaded.Broadcast(true, Result);
}

void UJellyfinClient::HandleItemDetailsResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FJellyfinMediaItem Item;

	EJellyfinErrorType ErrorType = CategorizeError(Response, bWasSuccessful);

	if (ErrorType != EJellyfinErrorType::None)
	{
		if (ErrorType == EJellyfinErrorType::AuthError)
		{
			RefreshToken();
		}

		FString ErrorMessage = FString::Printf(TEXT("Failed to load item details: %d"),
			Response.IsValid() ? Response->GetResponseCode() : 0);
		HandleRequestFailure(ErrorType, ErrorMessage);

		OnItemLoaded.Broadcast(false, Item);
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		HandleRequestFailure(EJellyfinErrorType::ParseError, TEXT("Failed to parse item details response"));
		OnItemLoaded.Broadcast(false, Item);
		return;
	}

	UpdateConnectivityState(true);

	Item = ParseMediaItem(JsonObject);
	OnItemLoaded.Broadcast(true, Item);
}

void UJellyfinClient::HandleSearchResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	TArray<FJellyfinSearchHint> Results;

	EJellyfinErrorType ErrorType = CategorizeError(Response, bWasSuccessful);

	if (ErrorType != EJellyfinErrorType::None)
	{
		if (ErrorType == EJellyfinErrorType::AuthError)
		{
			RefreshToken();
		}

		FString ErrorMessage = FString::Printf(TEXT("Search failed: %d"),
			Response.IsValid() ? Response->GetResponseCode() : 0);
		HandleRequestFailure(ErrorType, ErrorMessage);

		OnSearchComplete.Broadcast(false, Results);
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		HandleRequestFailure(EJellyfinErrorType::ParseError, TEXT("Failed to parse search response"));
		OnSearchComplete.Broadcast(false, Results);
		return;
	}

	UpdateConnectivityState(true);

	const TArray<TSharedPtr<FJsonValue>>* HintsArray;
	if (JsonObject->TryGetArrayField(TEXT("SearchHints"), HintsArray))
	{
		for (const TSharedPtr<FJsonValue>& Value : *HintsArray)
		{
			TSharedPtr<FJsonObject> HintObj = Value->AsObject();
			if (HintObj.IsValid())
			{
				FJellyfinSearchHint Hint;
				Hint.Id = HintObj->GetStringField(TEXT("Id"));
				Hint.Name = HintObj->GetStringField(TEXT("Name"));
				Hint.Type = ParseItemType(HintObj->GetStringField(TEXT("Type")));
				Hint.ProductionYear = HintObj->GetIntegerField(TEXT("ProductionYear"));
				Hint.PrimaryImageTag = HintObj->GetStringField(TEXT("PrimaryImageTag"));
				Hint.ThumbImageTag = HintObj->GetStringField(TEXT("ThumbImageTag"));
				Hint.SeriesName = HintObj->GetStringField(TEXT("Series"));

				Results.Add(Hint);
			}
		}
	}

	OnSearchComplete.Broadcast(true, Results);
}

// ============ Playback Operations ============

void UJellyfinClient::GetPlaybackInfo(const FString& ItemId)
{
	if (!IsAuthenticated())
	{
		return;
	}

	FString Endpoint = FString::Printf(
		TEXT("/Items/%s/PlaybackInfo?UserId=%s"),
		*ItemId, *CurrentSession.UserId
	);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateRequest(TEXT("POST"), Endpoint);
	AddAuthHeaders(Request);

	// Build device profile
	TSharedPtr<FJsonObject> DeviceProfile = MakeShareable(new FJsonObject());

	// Request body
	TSharedPtr<FJsonObject> RequestBody = MakeShareable(new FJsonObject());
	RequestBody->SetObjectField(TEXT("DeviceProfile"), DeviceProfile);

	FString Body;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Body);
	FJsonSerializer::Serialize(RequestBody.ToSharedRef(), Writer);
	Request->SetContentAsString(Body);

	Request->OnProcessRequestComplete().BindUObject(this, &UJellyfinClient::HandlePlaybackInfoResponse);
	Request->ProcessRequest();
}

void UJellyfinClient::HandlePlaybackInfoResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FJellyfinPlaybackInfo PlaybackInfo;

	EJellyfinErrorType ErrorType = CategorizeError(Response, bWasSuccessful);

	if (ErrorType != EJellyfinErrorType::None)
	{
		if (ErrorType == EJellyfinErrorType::AuthError)
		{
			RefreshToken();
		}

		FString ErrorMessage = FString::Printf(TEXT("Failed to get playback info: %d"),
			Response.IsValid() ? Response->GetResponseCode() : 0);
		HandleRequestFailure(ErrorType, ErrorMessage);

		OnPlaybackInfoLoaded.Broadcast(false, PlaybackInfo);
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		HandleRequestFailure(EJellyfinErrorType::ParseError, TEXT("Failed to parse playback info response"));
		OnPlaybackInfoLoaded.Broadcast(false, PlaybackInfo);
		return;
	}

	UpdateConnectivityState(true);

	PlaybackInfo.PlaySessionId = JsonObject->GetStringField(TEXT("PlaySessionId"));

	const TArray<TSharedPtr<FJsonValue>>* MediaSourcesArray;
	if (JsonObject->TryGetArrayField(TEXT("MediaSources"), MediaSourcesArray) && MediaSourcesArray->Num() > 0)
	{
		TSharedPtr<FJsonObject> MediaSource = (*MediaSourcesArray)[0]->AsObject();
		if (MediaSource.IsValid())
		{
			PlaybackInfo.MediaSourceId = MediaSource->GetStringField(TEXT("Id"));
			PlaybackInfo.Container = MediaSource->GetStringField(TEXT("Container"));

			// Try to get boolean fields safely with defaults
			MediaSource->TryGetBoolField(TEXT("SupportsDirectPlay"), PlaybackInfo.bSupportsDirectPlay);
			MediaSource->TryGetBoolField(TEXT("SupportsDirectStream"), PlaybackInfo.bSupportsDirectStream);
			MediaSource->TryGetBoolField(TEXT("SupportsTranscoding"), PlaybackInfo.bSupportsTranscoding);

			UE_LOG(LogJellyfinVR, Log, TEXT("PlaybackInfo parsing: MediaSourceId=%s, Container=%s, DirectPlay=%s, DirectStream=%s, Transcoding=%s"),
				*PlaybackInfo.MediaSourceId,
				*PlaybackInfo.Container,
				PlaybackInfo.bSupportsDirectPlay ? TEXT("true") : TEXT("false"),
				PlaybackInfo.bSupportsDirectStream ? TEXT("true") : TEXT("false"),
				PlaybackInfo.bSupportsTranscoding ? TEXT("true") : TEXT("false"));

			// Build stream URL
			if (PlaybackInfo.bSupportsDirectPlay || PlaybackInfo.bSupportsDirectStream)
			{
				PlaybackInfo.StreamType = PlaybackInfo.bSupportsDirectPlay ?
					EJellyfinStreamType::DirectPlay : EJellyfinStreamType::DirectStream;

				// Direct stream URL
				PlaybackInfo.StreamUrl = FString::Printf(
					TEXT("%s/Videos/%s/stream.%s?Static=true&MediaSourceId=%s&api_key=%s"),
					*ServerSettings.ServerUrl, *PlaybackInfo.MediaSourceId,
					*PlaybackInfo.Container, *PlaybackInfo.MediaSourceId,
					*CurrentSession.AccessToken
				);
			}
			else if (PlaybackInfo.bSupportsTranscoding)
			{
				PlaybackInfo.StreamType = EJellyfinStreamType::Transcode;
				FString TranscodingUrl;
				if (MediaSource->TryGetStringField(TEXT("TranscodingUrl"), TranscodingUrl) && !TranscodingUrl.IsEmpty())
				{
					PlaybackInfo.StreamUrl = ServerSettings.ServerUrl + TranscodingUrl;
				}
			}

			// Fallback: If no URL yet, request HLS transcoding for maximum compatibility
			// Windows Media Foundation can't decode many raw MKV codecs, so we need Jellyfin to transcode
			if (PlaybackInfo.StreamUrl.IsEmpty() && !PlaybackInfo.MediaSourceId.IsEmpty())
			{
				UE_LOG(LogJellyfinVR, Warning, TEXT("No supported playback type, falling back to HLS transcoding"));
				PlaybackInfo.StreamType = EJellyfinStreamType::Transcode;

				// Use HLS transcoding with H.264/AAC which WmfMedia can handle
				// master.m3u8 tells Jellyfin to transcode to HLS with compatible codecs
				PlaybackInfo.StreamUrl = FString::Printf(
					TEXT("%s/Videos/%s/master.m3u8?MediaSourceId=%s&api_key=%s&VideoCodec=h264&AudioCodec=aac&MaxStreamingBitrate=20000000&TranscodingMaxAudioChannels=6"),
					*ServerSettings.ServerUrl, *PlaybackInfo.MediaSourceId,
					*PlaybackInfo.MediaSourceId,
					*CurrentSession.AccessToken
				);

				UE_LOG(LogJellyfinVR, Log, TEXT("HLS transcode URL: %s"), *PlaybackInfo.StreamUrl);
			}

			// Parse media streams
			const TArray<TSharedPtr<FJsonValue>>* StreamsArray;
			if (MediaSource->TryGetArrayField(TEXT("MediaStreams"), StreamsArray))
			{
				for (const TSharedPtr<FJsonValue>& StreamValue : *StreamsArray)
				{
					TSharedPtr<FJsonObject> StreamObj = StreamValue->AsObject();
					if (StreamObj.IsValid())
					{
						PlaybackInfo.MediaStreams.Add(ParseMediaStream(StreamObj));
					}
				}
			}
		}
	}

	OnPlaybackInfoLoaded.Broadcast(true, PlaybackInfo);
}

void UJellyfinClient::ReportPlaybackStart(const FString& ItemId, const FString& MediaSourceId, const FString& PlaySessionId)
{
	if (!IsAuthenticated())
	{
		return;
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateRequest(TEXT("POST"), TEXT("/Sessions/Playing"));
	AddAuthHeaders(Request);

	TSharedPtr<FJsonObject> Body = MakeShareable(new FJsonObject());
	Body->SetStringField(TEXT("ItemId"), ItemId);
	Body->SetStringField(TEXT("MediaSourceId"), MediaSourceId);
	Body->SetStringField(TEXT("PlaySessionId"), PlaySessionId);
	Body->SetBoolField(TEXT("CanSeek"), true);

	FString BodyString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&BodyString);
	FJsonSerializer::Serialize(Body.ToSharedRef(), Writer);
	Request->SetContentAsString(BodyString);

	Request->ProcessRequest();
}

void UJellyfinClient::ReportPlaybackProgress(const FString& ItemId, const FString& MediaSourceId,
	const FString& PlaySessionId, int64 PositionTicks, bool bIsPaused)
{
	if (!IsAuthenticated())
	{
		return;
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateRequest(TEXT("POST"), TEXT("/Sessions/Playing/Progress"));
	AddAuthHeaders(Request);

	TSharedPtr<FJsonObject> Body = MakeShareable(new FJsonObject());
	Body->SetStringField(TEXT("ItemId"), ItemId);
	Body->SetStringField(TEXT("MediaSourceId"), MediaSourceId);
	Body->SetStringField(TEXT("PlaySessionId"), PlaySessionId);
	Body->SetNumberField(TEXT("PositionTicks"), PositionTicks);
	Body->SetBoolField(TEXT("IsPaused"), bIsPaused);
	Body->SetBoolField(TEXT("CanSeek"), true);

	FString BodyString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&BodyString);
	FJsonSerializer::Serialize(Body.ToSharedRef(), Writer);
	Request->SetContentAsString(BodyString);

	Request->ProcessRequest();
}

void UJellyfinClient::ReportPlaybackStopped(const FString& ItemId, const FString& MediaSourceId,
	const FString& PlaySessionId, int64 PositionTicks)
{
	if (!IsAuthenticated())
	{
		return;
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateRequest(TEXT("POST"), TEXT("/Sessions/Playing/Stopped"));
	AddAuthHeaders(Request);

	TSharedPtr<FJsonObject> Body = MakeShareable(new FJsonObject());
	Body->SetStringField(TEXT("ItemId"), ItemId);
	Body->SetStringField(TEXT("MediaSourceId"), MediaSourceId);
	Body->SetStringField(TEXT("PlaySessionId"), PlaySessionId);
	Body->SetNumberField(TEXT("PositionTicks"), PositionTicks);

	FString BodyString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&BodyString);
	FJsonSerializer::Serialize(Body.ToSharedRef(), Writer);
	Request->SetContentAsString(BodyString);

	Request->ProcessRequest();
}

void UJellyfinClient::MarkPlayed(const FString& ItemId)
{
	if (!IsAuthenticated())
	{
		return;
	}

	FString Endpoint = FString::Printf(TEXT("/Users/%s/PlayedItems/%s"), *CurrentSession.UserId, *ItemId);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateRequest(TEXT("POST"), Endpoint);
	AddAuthHeaders(Request);
	Request->ProcessRequest();
}

void UJellyfinClient::MarkUnplayed(const FString& ItemId)
{
	if (!IsAuthenticated())
	{
		return;
	}

	FString Endpoint = FString::Printf(TEXT("/Users/%s/PlayedItems/%s"), *CurrentSession.UserId, *ItemId);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = CreateRequest(TEXT("DELETE"), Endpoint);
	AddAuthHeaders(Request);
	Request->ProcessRequest();
}

// ============ Image Operations ============

FString UJellyfinClient::GetImageUrl(const FString& ItemId, const FString& ImageType,
	int32 MaxWidth, int32 MaxHeight) const
{
	return FString::Printf(
		TEXT("%s/Items/%s/Images/%s?maxWidth=%d&maxHeight=%d&quality=90"),
		*ServerSettings.ServerUrl, *ItemId, *ImageType, MaxWidth, MaxHeight
	);
}

void UJellyfinClient::LoadImageTexture(const FString& ItemId, const FString& ImageType,
	int32 MaxWidth, int32 MaxHeight)
{
	FString CacheKey = FString::Printf(TEXT("%s_%s_%d_%d"), *ItemId, *ImageType, MaxWidth, MaxHeight);

	// Check cache
	if (UTexture2D** CachedTexture = ImageCache.Find(CacheKey))
	{
		if (*CachedTexture != nullptr)
		{
			OnImageLoaded.Broadcast(true, *CachedTexture);
			return;
		}
	}

	FString Url = GetImageUrl(ItemId, ImageType, MaxWidth, MaxHeight);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(TEXT("GET"));

	if (IsAuthenticated())
	{
		AddAuthHeaders(Request);
	}

	Request->OnProcessRequestComplete().BindUObject(this, &UJellyfinClient::HandleImageResponse);
	Request->ProcessRequest();
}

void UJellyfinClient::HandleImageResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	EJellyfinErrorType ErrorType = CategorizeError(Response, bWasSuccessful);

	if (ErrorType != EJellyfinErrorType::None)
	{
		// Don't trigger auth refresh for image failures - these are less critical
		FString ErrorMessage = FString::Printf(TEXT("Failed to load image: %d"),
			Response.IsValid() ? Response->GetResponseCode() : 0);
		HandleRequestFailure(ErrorType, ErrorMessage);

		OnImageLoaded.Broadcast(false, nullptr);
		return;
	}

	UpdateConnectivityState(true);

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

	TArray<uint8> ImageData = Response->GetContent();
	EImageFormat Format = ImageWrapperModule.DetectImageFormat(ImageData.GetData(), ImageData.Num());

	if (Format == EImageFormat::Invalid)
	{
		OnImageLoaded.Broadcast(false, nullptr);
		return;
	}

	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(Format);
	if (!ImageWrapper.IsValid() || !ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
	{
		OnImageLoaded.Broadcast(false, nullptr);
		return;
	}

	TArray<uint8> RawData;
	if (!ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, RawData))
	{
		OnImageLoaded.Broadcast(false, nullptr);
		return;
	}

	UTexture2D* Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);
	if (!Texture)
	{
		OnImageLoaded.Broadcast(false, nullptr);
		return;
	}

	void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, RawData.GetData(), RawData.Num());
	Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
	Texture->UpdateResource();

	OnImageLoaded.Broadcast(true, Texture);
}

// ============ JSON Parsing Helpers ============

FJellyfinMediaItem UJellyfinClient::ParseMediaItem(const TSharedPtr<FJsonObject>& JsonObject)
{
	FJellyfinMediaItem Item;

	Item.Id = JsonObject->GetStringField(TEXT("Id"));
	Item.Name = JsonObject->GetStringField(TEXT("Name"));
	Item.SortName = JsonObject->GetStringField(TEXT("SortName"));
	Item.Overview = JsonObject->GetStringField(TEXT("Overview"));
	Item.Type = ParseItemType(JsonObject->GetStringField(TEXT("Type")));

	Item.SeriesId = JsonObject->GetStringField(TEXT("SeriesId"));
	Item.SeriesName = JsonObject->GetStringField(TEXT("SeriesName"));
	Item.SeasonId = JsonObject->GetStringField(TEXT("SeasonId"));
	Item.SeasonName = JsonObject->GetStringField(TEXT("SeasonName"));

	Item.IndexNumber = JsonObject->GetIntegerField(TEXT("IndexNumber"));
	Item.ParentIndexNumber = JsonObject->GetIntegerField(TEXT("ParentIndexNumber"));
	Item.ProductionYear = JsonObject->GetIntegerField(TEXT("ProductionYear"));
	Item.OfficialRating = JsonObject->GetStringField(TEXT("OfficialRating"));
	Item.CommunityRating = JsonObject->GetNumberField(TEXT("CommunityRating"));
	Item.RunTimeTicks = (int64)JsonObject->GetNumberField(TEXT("RunTimeTicks"));

	// Genres
	const TArray<TSharedPtr<FJsonValue>>* GenresArray;
	if (JsonObject->TryGetArrayField(TEXT("Genres"), GenresArray))
	{
		for (const TSharedPtr<FJsonValue>& Value : *GenresArray)
		{
			Item.Genres.Add(Value->AsString());
		}
	}

	// Studios
	const TArray<TSharedPtr<FJsonValue>>* StudiosArray;
	if (JsonObject->TryGetArrayField(TEXT("Studios"), StudiosArray))
	{
		for (const TSharedPtr<FJsonValue>& Value : *StudiosArray)
		{
			TSharedPtr<FJsonObject> StudioObj = Value->AsObject();
			if (StudioObj.IsValid())
			{
				Item.Studios.Add(StudioObj->GetStringField(TEXT("Name")));
			}
		}
	}

	// User data (playback state)
	if (JsonObject->HasField(TEXT("UserData")))
	{
		TSharedPtr<FJsonObject> UserData = JsonObject->GetObjectField(TEXT("UserData"));
		if (UserData.IsValid())
		{
			Item.PlaybackPositionTicks = (int64)UserData->GetNumberField(TEXT("PlaybackPositionTicks"));
			Item.bIsPlayed = UserData->GetBoolField(TEXT("Played"));
			Item.bIsFavorite = UserData->GetBoolField(TEXT("IsFavorite"));
		}
	}

	// Image tags
	if (JsonObject->HasField(TEXT("ImageTags")))
	{
		TSharedPtr<FJsonObject> ImageTags = JsonObject->GetObjectField(TEXT("ImageTags"));
		if (ImageTags.IsValid())
		{
			Item.PrimaryImageTag = ImageTags->GetStringField(TEXT("Primary"));
			Item.ThumbImageTag = ImageTags->GetStringField(TEXT("Thumb"));
		}
	}

	const TArray<TSharedPtr<FJsonValue>>* BackdropArray;
	if (JsonObject->TryGetArrayField(TEXT("BackdropImageTags"), BackdropArray) && BackdropArray->Num() > 0)
	{
		Item.BackdropImageTag = (*BackdropArray)[0]->AsString();
	}

	// Media streams
	const TArray<TSharedPtr<FJsonValue>>* StreamsArray;
	if (JsonObject->TryGetArrayField(TEXT("MediaStreams"), StreamsArray))
	{
		for (const TSharedPtr<FJsonValue>& Value : *StreamsArray)
		{
			TSharedPtr<FJsonObject> StreamObj = Value->AsObject();
			if (StreamObj.IsValid())
			{
				Item.MediaStreams.Add(ParseMediaStream(StreamObj));
			}
		}
	}

	// Chapters
	const TArray<TSharedPtr<FJsonValue>>* ChaptersArray;
	if (JsonObject->TryGetArrayField(TEXT("Chapters"), ChaptersArray))
	{
		for (const TSharedPtr<FJsonValue>& Value : *ChaptersArray)
		{
			TSharedPtr<FJsonObject> ChapterObj = Value->AsObject();
			if (ChapterObj.IsValid())
			{
				Item.Chapters.Add(ParseChapter(ChapterObj));
			}
		}
	}

	Item.Container = JsonObject->GetStringField(TEXT("Container"));
	Item.Path = JsonObject->GetStringField(TEXT("Path"));
	Item.Size = (int64)JsonObject->GetNumberField(TEXT("Size"));
	Item.Bitrate = JsonObject->GetIntegerField(TEXT("Bitrate"));

	return Item;
}

FJellyfinMediaStream UJellyfinClient::ParseMediaStream(const TSharedPtr<FJsonObject>& JsonObject)
{
	FJellyfinMediaStream Stream;

	Stream.Index = JsonObject->GetIntegerField(TEXT("Index"));
	Stream.Type = JsonObject->GetStringField(TEXT("Type"));
	Stream.Codec = JsonObject->GetStringField(TEXT("Codec"));
	Stream.Language = JsonObject->GetStringField(TEXT("Language"));
	Stream.DisplayTitle = JsonObject->GetStringField(TEXT("DisplayTitle"));
	Stream.bIsDefault = JsonObject->GetBoolField(TEXT("IsDefault"));
	Stream.bIsForced = JsonObject->GetBoolField(TEXT("IsForced"));
	Stream.bIsExternal = JsonObject->GetBoolField(TEXT("IsExternal"));

	// Video specific
	Stream.Width = JsonObject->GetIntegerField(TEXT("Width"));
	Stream.Height = JsonObject->GetIntegerField(TEXT("Height"));
	Stream.AspectRatio = JsonObject->GetNumberField(TEXT("AspectRatio"));
	Stream.VideoRange = JsonObject->GetStringField(TEXT("VideoRange"));
	Stream.bIsHDR = Stream.VideoRange != TEXT("SDR") && !Stream.VideoRange.IsEmpty();

	// Audio specific
	Stream.Channels = JsonObject->GetIntegerField(TEXT("Channels"));
	Stream.SampleRate = JsonObject->GetIntegerField(TEXT("SampleRate"));

	return Stream;
}

FJellyfinChapter UJellyfinClient::ParseChapter(const TSharedPtr<FJsonObject>& JsonObject)
{
	FJellyfinChapter Chapter;

	Chapter.Name = JsonObject->GetStringField(TEXT("Name"));
	Chapter.StartPositionTicks = (int64)JsonObject->GetNumberField(TEXT("StartPositionTicks"));
	Chapter.ImageTag = JsonObject->GetStringField(TEXT("ImageTag"));

	return Chapter;
}

EJellyfinItemType UJellyfinClient::ParseItemType(const FString& TypeString)
{
	if (TypeString == TEXT("Movie")) return EJellyfinItemType::Movie;
	if (TypeString == TEXT("Series")) return EJellyfinItemType::Series;
	if (TypeString == TEXT("Season")) return EJellyfinItemType::Season;
	if (TypeString == TEXT("Episode")) return EJellyfinItemType::Episode;
	if (TypeString == TEXT("Audio")) return EJellyfinItemType::Audio;
	if (TypeString == TEXT("MusicAlbum")) return EJellyfinItemType::MusicAlbum;
	if (TypeString == TEXT("MusicArtist")) return EJellyfinItemType::MusicArtist;
	if (TypeString == TEXT("Folder")) return EJellyfinItemType::Folder;
	if (TypeString == TEXT("CollectionFolder")) return EJellyfinItemType::CollectionFolder;
	if (TypeString == TEXT("BoxSet")) return EJellyfinItemType::BoxSet;
	if (TypeString == TEXT("Playlist")) return EJellyfinItemType::Playlist;

	return EJellyfinItemType::Unknown;
}
