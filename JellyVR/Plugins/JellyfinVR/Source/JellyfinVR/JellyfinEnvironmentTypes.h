// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "JellyfinEnvironmentTypes.generated.h"

/**
 * Data table row for environment definitions
 * Add rows to DT_Environments to register new viewing environments
 */
USTRUCT(BlueprintType)
struct JELLYFINVR_API FJellyfinEnvironmentInfo : public FTableRowBase
{
	GENERATED_BODY()

	/** Path to the level asset */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	FSoftObjectPath LevelPath;

	/** Display name shown in environment selector */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	FString DisplayName;

	/** Description of the environment */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	FString Description;

	/** Preview image for the selector UI */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	TSoftObjectPtr<UTexture2D> PreviewImage;

	/** If true, this environment requires PCVR (too demanding for Quest standalone) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	bool bRequiresPCVR = false;

	/** Sort order in the environment list (lower = higher priority) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	int32 SortOrder = 0;

	FJellyfinEnvironmentInfo()
		: bRequiresPCVR(false)
		, SortOrder(0)
	{
	}
};

/**
 * Delegate fired when an environment is loaded or fails to load
 * @param EnvironmentId The ID/name of the environment
 * @param bSuccess Whether the load was successful
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnvironmentChanged, const FString&, EnvironmentId, bool, bSuccess);

/**
 * Manager for loading and switching environments
 */
UCLASS(BlueprintType)
class JELLYFINVR_API UJellyfinEnvironmentManager : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Load environment definitions from data table
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Environments")
	void LoadEnvironments(UDataTable* EnvironmentTable);

	/**
	 * Get all available environments
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Environments")
	TArray<FJellyfinEnvironmentInfo> GetAllEnvironments() const;

	/**
	 * Get environments compatible with current platform
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Environments")
	TArray<FJellyfinEnvironmentInfo> GetCompatibleEnvironments() const;

	/**
	 * Load an environment by row name
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Environments")
	void LoadEnvironment(const FString& RowName);

	/**
	 * Get currently loaded environment name
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Environments")
	FString GetCurrentEnvironment() const { return CurrentEnvironmentName; }

	/**
	 * Event fired when environment changes (successfully or not)
	 */
	UPROPERTY(BlueprintAssignable, Category = "JellyfinVR|Environments")
	FOnEnvironmentChanged OnEnvironmentChanged;

protected:
	UPROPERTY()
	TArray<FJellyfinEnvironmentInfo> Environments;

	FString CurrentEnvironmentName;

	/**
	 * Store row name to environment info mapping for lookup
	 */
	TMap<FString, FJellyfinEnvironmentInfo> EnvironmentLookup;
};
