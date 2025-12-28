// Copyright JellyVR Project. All Rights Reserved.

#include "JellyfinAuth.h"
#include "JellyfinClient.h"
#include "JellyfinImageLoader.h"
#include "JellyfinVRModule.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

void UJellyfinAuthSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Create the Jellyfin client
	JellyfinClient = NewObject<UJellyfinClient>(this);

	// Bind to auth events
	JellyfinClient->OnAuthComplete.AddDynamic(this, &UJellyfinAuthSubsystem::OnAuthComplete);

	// Set up save file path
	SaveFilePath = FPaths::ProjectSavedDir() / TEXT("JellyfinVR") / TEXT("credentials.json");

	UE_LOG(LogJellyfinVR, Log, TEXT("JellyfinAuthSubsystem initialized"));
}

void UJellyfinAuthSubsystem::Deinitialize()
{
	if (JellyfinClient)
	{
		JellyfinClient->OnAuthComplete.RemoveDynamic(this, &UJellyfinAuthSubsystem::OnAuthComplete);
	}

	Super::Deinitialize();
}

void UJellyfinAuthSubsystem::Connect(const FString& ServerUrl, const FString& Username,
	const FString& Password, bool bRememberMe)
{
	if (!JellyfinClient)
	{
		UE_LOG(LogJellyfinVR, Error, TEXT("JellyfinClient not initialized"));
		return;
	}

	bRememberCredentials = bRememberMe;

	FJellyfinServerSettings Settings;
	Settings.ServerUrl = ServerUrl;
	Settings.Username = Username;
	Settings.bRememberMe = bRememberMe;

	// Initialize image loader with server URL
	if (UJellyfinImageLoader* ImageLoader = UJellyfinImageLoader::Get(this))
	{
		ImageLoader->SetServerUrl(ServerUrl);
		UE_LOG(LogJellyfinVR, Log, TEXT("ImageLoader initialized with server: %s"), *ServerUrl);
	}

	JellyfinClient->Initialize(Settings);
	JellyfinClient->Authenticate(Username, Password);

	OnConnectionStateChanged.Broadcast(EJellyfinAuthState::Authenticating);
}

bool UJellyfinAuthSubsystem::TryAutoConnect()
{
	if (!HasSavedCredentials())
	{
		return false;
	}

	FString ServerUrl, Username, UserId, AccessToken;
	LoadCredentials(ServerUrl, Username, UserId, AccessToken);

	if (ServerUrl.IsEmpty() || AccessToken.IsEmpty() || UserId.IsEmpty())
	{
		return false;
	}

	// Initialize image loader with server URL
	if (UJellyfinImageLoader* ImageLoader = UJellyfinImageLoader::Get(this))
	{
		ImageLoader->SetServerUrl(ServerUrl);
		UE_LOG(LogJellyfinVR, Log, TEXT("ImageLoader initialized with server: %s"), *ServerUrl);
	}

	FJellyfinServerSettings Settings;
	Settings.ServerUrl = ServerUrl;
	Settings.Username = Username;
	Settings.bRememberMe = true;

	JellyfinClient->Initialize(Settings);
	JellyfinClient->AuthenticateWithToken(UserId, AccessToken);

	UE_LOG(LogJellyfinVR, Log, TEXT("Attempting auto-connect to %s as %s"), *ServerUrl, *Username);
	return true;
}

void UJellyfinAuthSubsystem::Disconnect()
{
	if (JellyfinClient)
	{
		JellyfinClient->Logout();
		OnConnectionStateChanged.Broadcast(EJellyfinAuthState::NotAuthenticated);
	}
}

bool UJellyfinAuthSubsystem::IsConnected() const
{
	return JellyfinClient && JellyfinClient->IsAuthenticated();
}

EJellyfinAuthState UJellyfinAuthSubsystem::GetConnectionState() const
{
	if (JellyfinClient)
	{
		return JellyfinClient->GetSession().AuthState;
	}
	return EJellyfinAuthState::NotAuthenticated;
}

const FJellyfinUserSession& UJellyfinAuthSubsystem::GetSession() const
{
	static FJellyfinUserSession EmptySession;
	if (JellyfinClient)
	{
		return JellyfinClient->GetSession();
	}
	return EmptySession;
}

bool UJellyfinAuthSubsystem::HasSavedCredentials() const
{
	return FPaths::FileExists(SaveFilePath);
}

void UJellyfinAuthSubsystem::ClearSavedCredentials()
{
	if (FPaths::FileExists(SaveFilePath))
	{
		IFileManager::Get().Delete(*SaveFilePath);
		UE_LOG(LogJellyfinVR, Log, TEXT("Cleared saved credentials"));
	}
}

FString UJellyfinAuthSubsystem::GetSavedServerUrl() const
{
	FString ServerUrl, Username, UserId, AccessToken;
	const_cast<UJellyfinAuthSubsystem*>(this)->LoadCredentials(ServerUrl, Username, UserId, AccessToken);
	return ServerUrl;
}

FString UJellyfinAuthSubsystem::GetSavedUsername() const
{
	FString ServerUrl, Username, UserId, AccessToken;
	const_cast<UJellyfinAuthSubsystem*>(this)->LoadCredentials(ServerUrl, Username, UserId, AccessToken);
	return Username;
}

void UJellyfinAuthSubsystem::OnAuthComplete(bool bSuccess, const FJellyfinUserSession& Session)
{
	if (bSuccess)
	{
		UE_LOG(LogJellyfinVR, Log, TEXT("Authentication successful for user: %s"), *Session.Username);

		if (bRememberCredentials)
		{
			// Get the server URL from settings - we need to access it differently
			// For now, we'll save what we have
			FString ServerUrl = GetSavedServerUrl();
			if (ServerUrl.IsEmpty())
			{
				// Try to construct from the client
				// This is a workaround - in a real implementation we'd store this better
			}

			SaveCredentials(ServerUrl, Session.Username, Session.UserId, Session.AccessToken);
		}

		OnConnectionStateChanged.Broadcast(EJellyfinAuthState::Authenticated);
	}
	else
	{
		UE_LOG(LogJellyfinVR, Warning, TEXT("Authentication failed"));

		// Clear saved credentials on auth failure (e.g., expired token during auto-login)
		if (Session.AuthState == EJellyfinAuthState::Failed)
		{
			ClearSavedCredentials();
		}

		OnConnectionStateChanged.Broadcast(EJellyfinAuthState::Failed);
	}
}

void UJellyfinAuthSubsystem::SaveCredentials(const FString& ServerUrl, const FString& Username,
	const FString& UserId, const FString& AccessToken)
{
	// Ensure directory exists
	FString Directory = FPaths::GetPath(SaveFilePath);
	IFileManager::Get().MakeDirectory(*Directory, true);

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	JsonObject->SetStringField(TEXT("ServerUrl"), ServerUrl);
	JsonObject->SetStringField(TEXT("Username"), Username);
	JsonObject->SetStringField(TEXT("UserId"), UserId);
	JsonObject->SetStringField(TEXT("AccessToken"), AccessToken);

	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	if (FFileHelper::SaveStringToFile(JsonString, *SaveFilePath))
	{
		UE_LOG(LogJellyfinVR, Log, TEXT("Credentials saved successfully"));
	}
	else
	{
		UE_LOG(LogJellyfinVR, Error, TEXT("Failed to save credentials"));
	}
}

void UJellyfinAuthSubsystem::LoadCredentials(FString& OutServerUrl, FString& OutUsername,
	FString& OutUserId, FString& OutAccessToken)
{
	OutServerUrl.Empty();
	OutUsername.Empty();
	OutUserId.Empty();
	OutAccessToken.Empty();

	if (!FPaths::FileExists(SaveFilePath))
	{
		return;
	}

	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *SaveFilePath))
	{
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		return;
	}

	OutServerUrl = JsonObject->GetStringField(TEXT("ServerUrl"));
	OutUsername = JsonObject->GetStringField(TEXT("Username"));
	OutUserId = JsonObject->GetStringField(TEXT("UserId"));
	OutAccessToken = JsonObject->GetStringField(TEXT("AccessToken"));
}
