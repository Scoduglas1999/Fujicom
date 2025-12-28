// Copyright JellyVR Project. All Rights Reserved.

#include "JellyfinEnvironmentTypes.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"

void UJellyfinEnvironmentManager::LoadEnvironments(UDataTable* EnvironmentTable)
{
	Environments.Empty();
	EnvironmentLookup.Empty();

	if (!EnvironmentTable)
	{
		return;
	}

	// Get all row names from the data table
	TArray<FName> RowNames = EnvironmentTable->GetRowNames();

	for (const FName& RowName : RowNames)
	{
		FJellyfinEnvironmentInfo* RowData = EnvironmentTable->FindRow<FJellyfinEnvironmentInfo>(RowName, TEXT(""));
		if (RowData)
		{
			Environments.Add(*RowData);
			EnvironmentLookup.Add(RowName.ToString(), *RowData);
		}
	}

	// Sort by SortOrder
	Environments.Sort([](const FJellyfinEnvironmentInfo& A, const FJellyfinEnvironmentInfo& B)
	{
		return A.SortOrder < B.SortOrder;
	});
}

TArray<FJellyfinEnvironmentInfo> UJellyfinEnvironmentManager::GetAllEnvironments() const
{
	return Environments;
}

TArray<FJellyfinEnvironmentInfo> UJellyfinEnvironmentManager::GetCompatibleEnvironments() const
{
	TArray<FJellyfinEnvironmentInfo> Compatible;

	// Check if we're running on a standalone device (Quest) or PCVR
	bool bIsStandalone = false;

#if PLATFORM_ANDROID
	bIsStandalone = true;
#endif

	for (const FJellyfinEnvironmentInfo& Env : Environments)
	{
		// If standalone, exclude PCVR-only environments
		if (bIsStandalone && Env.bRequiresPCVR)
		{
			continue;
		}

		Compatible.Add(Env);
	}

	return Compatible;
}

void UJellyfinEnvironmentManager::LoadEnvironment(const FString& RowName)
{
	// First try to find by row name in lookup map
	FJellyfinEnvironmentInfo* EnvInfo = EnvironmentLookup.Find(RowName);

	// If not found, try searching by display name or level path
	if (!EnvInfo)
	{
		for (const FJellyfinEnvironmentInfo& Env : Environments)
		{
			if (Env.DisplayName == RowName || Env.LevelPath.GetAssetName() == RowName)
			{
				EnvInfo = const_cast<FJellyfinEnvironmentInfo*>(&Env);
				break;
			}
		}
	}

	if (!EnvInfo)
	{
		// Environment not found
		OnEnvironmentChanged.Broadcast(RowName, false);
		return;
	}

	// Get the level path
	FString LevelName = EnvInfo->LevelPath.GetAssetPathString();
	if (LevelName.IsEmpty())
	{
		// Invalid level path
		OnEnvironmentChanged.Broadcast(RowName, false);
		return;
	}

	// Store current environment
	FString OldEnvironment = CurrentEnvironmentName;
	CurrentEnvironmentName = EnvInfo->DisplayName;

	// Load the level using OpenLevel
	// Note: This is a blocking operation that will transition to the new level
	// For more advanced scenarios, consider using level streaming with ULevelStreamingDynamic
	UGameplayStatics::OpenLevel(this, FName(*LevelName));

	// Broadcast success
	// Note: This will fire before the level actually loads since OpenLevel is async
	// In a production system, you'd hook into level streaming events
	OnEnvironmentChanged.Broadcast(RowName, true);
}
