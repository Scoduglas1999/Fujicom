// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "JellyfinVRWidget.h"
#include "JellyfinEnvironmentTypes.h"
#include "JellyfinEnvironmentSelector.generated.h"

/**
 * Information about an environment for display in the selector
 */
USTRUCT(BlueprintType)
struct FJellyfinEnvironmentDisplayInfo
{
	GENERATED_BODY()

	/** Row name/ID from the data table */
	UPROPERTY(BlueprintReadOnly, Category = "Environment")
	FString EnvironmentId;

	/** Display name */
	UPROPERTY(BlueprintReadOnly, Category = "Environment")
	FString DisplayName;

	/** Description */
	UPROPERTY(BlueprintReadOnly, Category = "Environment")
	FString Description;

	/** Preview image texture */
	UPROPERTY(BlueprintReadOnly, Category = "Environment")
	UTexture2D* PreviewImage = nullptr;

	/** Whether this requires PCVR */
	UPROPERTY(BlueprintReadOnly, Category = "Environment")
	bool bRequiresPCVR = false;

	/** Whether this is the currently loaded environment */
	UPROPERTY(BlueprintReadOnly, Category = "Environment")
	bool bIsCurrent = false;

	FJellyfinEnvironmentDisplayInfo()
		: bRequiresPCVR(false)
		, bIsCurrent(false)
	{
	}
};

/**
 * Widget for browsing and selecting VR environments
 * Allows users to switch between different viewing environments
 */
UCLASS(BlueprintType, Blueprintable)
class JELLYFINVR_API UJellyfinEnvironmentSelector : public UJellyfinVRWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	/**
	 * Load environments from the data table
	 * Call this when the widget is first shown or to refresh the list
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Environments")
	void RefreshEnvironments();

	/**
	 * Select and load an environment
	 * @param EnvironmentId The row name from the data table
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Environments")
	void SelectEnvironment(const FString& EnvironmentId);

	/**
	 * Get the currently loaded environment ID
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Environments")
	FString GetCurrentEnvironmentId() const;

	/**
	 * Get all available environments (filtered by platform if needed)
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Environments")
	TArray<FJellyfinEnvironmentDisplayInfo> GetAvailableEnvironments() const;

	/**
	 * Filter environments by platform compatibility
	 * If true, only shows environments compatible with current platform (Quest vs PCVR)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Environments")
	bool bFilterByPlatform = true;

	/**
	 * Path to the environment data table asset
	 * Default: /JellyfinVR/Data/DT_Environments
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Environments")
	TSoftObjectPtr<UDataTable> EnvironmentDataTable;

protected:
	/**
	 * Called when environments are loaded and ready to display
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "JellyfinVR|Environments")
	void OnEnvironmentsReady(const TArray<FJellyfinEnvironmentDisplayInfo>& Environments);

	/**
	 * Called when an environment is successfully loaded
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "JellyfinVR|Environments")
	void OnEnvironmentLoaded(const FString& EnvironmentId, const FString& DisplayName);

	/**
	 * Called when environment loading fails
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "JellyfinVR|Environments")
	void OnEnvironmentLoadFailed(const FString& EnvironmentId, const FString& ErrorMessage);

	/**
	 * Called when environment is changing (before level transition)
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "JellyfinVR|Environments")
	void OnEnvironmentChanging(const FString& FromEnvironment, const FString& ToEnvironment);

private:
	/**
	 * Build display info list from environment data
	 */
	void BuildEnvironmentList();

	/**
	 * Check if we're running on a standalone platform (Quest)
	 */
	bool IsStandalonePlatform() const;

	/**
	 * Callback when environment manager loads an environment
	 */
	UFUNCTION()
	void HandleEnvironmentChanged(const FString& EnvironmentId, bool bSuccess);

	/** Environment manager instance */
	UPROPERTY()
	UJellyfinEnvironmentManager* EnvironmentManager;

	/** Raw environment data from data table */
	UPROPERTY()
	TArray<FJellyfinEnvironmentInfo> LoadedEnvironments;

	/** Display-ready environment list */
	UPROPERTY()
	TArray<FJellyfinEnvironmentDisplayInfo> DisplayEnvironments;

	/** Currently loaded environment ID */
	FString CurrentEnvironmentId;

	/** Map of row names to environment info for quick lookup */
	TMap<FString, FJellyfinEnvironmentInfo> EnvironmentMap;
};
