// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "JellyfinTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "JellyfinAuth.generated.h"

class UJellyfinClient;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJellyfinConnectionStateChanged, EJellyfinAuthState, NewState);

/**
 * Authentication subsystem for Jellyfin
 * Handles login/logout, token persistence, and session management
 */
UCLASS()
class JELLYFINVR_API UJellyfinAuthSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Get the Jellyfin client instance
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Auth")
	UJellyfinClient* GetClient() const { return JellyfinClient; }

	/**
	 * Connect to a Jellyfin server
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Auth")
	void Connect(const FString& ServerUrl, const FString& Username, const FString& Password, bool bRememberMe = true);

	/**
	 * Try to auto-connect using saved credentials
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Auth")
	bool TryAutoConnect();

	/**
	 * Disconnect from the current server
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Auth")
	void Disconnect();

	/**
	 * Check if connected
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Auth")
	bool IsConnected() const;

	/**
	 * Get current connection state
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Auth")
	EJellyfinAuthState GetConnectionState() const;

	/**
	 * Get current user session
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Auth")
	const FJellyfinUserSession& GetSession() const;

	/**
	 * Check if saved credentials exist
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Auth")
	bool HasSavedCredentials() const;

	/**
	 * Clear saved credentials
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Auth")
	void ClearSavedCredentials();

	/**
	 * Get saved server URL
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Auth")
	FString GetSavedServerUrl() const;

	/**
	 * Get saved username
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Auth")
	FString GetSavedUsername() const;

	// Events
	UPROPERTY(BlueprintAssignable, Category = "JellyfinVR|Events")
	FOnJellyfinConnectionStateChanged OnConnectionStateChanged;

protected:
	UFUNCTION()
	void OnAuthComplete(bool bSuccess, const FJellyfinUserSession& Session);

	void SaveCredentials(const FString& ServerUrl, const FString& Username,
		const FString& UserId, const FString& AccessToken);
	void LoadCredentials(FString& OutServerUrl, FString& OutUsername,
		FString& OutUserId, FString& OutAccessToken);

private:
	UPROPERTY()
	UJellyfinClient* JellyfinClient;

	FString SaveFilePath;
	bool bRememberCredentials = true;
};
