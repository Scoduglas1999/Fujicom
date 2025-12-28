// Copyright JellyVR Project. All Rights Reserved.

#include "JellyfinEnvironmentSelector.h"
#include "JellyfinVRModule.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "Kismet/GameplayStatics.h"

void UJellyfinEnvironmentSelector::NativeConstruct()
{
	Super::NativeConstruct();

	// Create environment manager if needed
	if (!EnvironmentManager)
	{
		EnvironmentManager = NewObject<UJellyfinEnvironmentManager>(this);
	}

	// Set default data table path if not configured
	if (EnvironmentDataTable.IsNull())
	{
		EnvironmentDataTable = TSoftObjectPtr<UDataTable>(FSoftObjectPath(TEXT("/JellyfinVR/Data/DT_Environments.DT_Environments")));
	}

	// Load environments on construction
	RefreshEnvironments();
}

void UJellyfinEnvironmentSelector::RefreshEnvironments()
{
	if (!EnvironmentManager)
	{
		UE_LOG(LogJellyfinVR, Error, TEXT("EnvironmentSelector: No environment manager available"));
		return;
	}

	ShowLoading(TEXT("Loading environments..."));

	// Load the data table
	UDataTable* DataTable = EnvironmentDataTable.LoadSynchronous();
	if (!DataTable)
	{
		UE_LOG(LogJellyfinVR, Error, TEXT("EnvironmentSelector: Failed to load environment data table at %s"),
			*EnvironmentDataTable.ToString());
		HideLoading();
		ShowError(TEXT("Failed to load environment data"));
		return;
	}

	// Load into environment manager
	EnvironmentManager->LoadEnvironments(DataTable);
	LoadedEnvironments = EnvironmentManager->GetAllEnvironments();

	// Build row name map for quick lookup
	EnvironmentMap.Empty();
	TArray<FName> RowNames = DataTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		FJellyfinEnvironmentInfo* RowData = DataTable->FindRow<FJellyfinEnvironmentInfo>(RowName, TEXT(""));
		if (RowData)
		{
			EnvironmentMap.Add(RowName.ToString(), *RowData);
		}
	}

	// Build display list
	BuildEnvironmentList();

	HideLoading();

	// Notify Blueprint
	OnEnvironmentsReady(DisplayEnvironments);

	UE_LOG(LogJellyfinVR, Log, TEXT("EnvironmentSelector: Loaded %d environments (%d after platform filtering)"),
		LoadedEnvironments.Num(), DisplayEnvironments.Num());
}

void UJellyfinEnvironmentSelector::SelectEnvironment(const FString& EnvironmentId)
{
	if (!EnvironmentManager)
	{
		UE_LOG(LogJellyfinVR, Error, TEXT("EnvironmentSelector: No environment manager available"));
		return;
	}

	// Validate environment exists
	if (!EnvironmentMap.Contains(EnvironmentId))
	{
		UE_LOG(LogJellyfinVR, Error, TEXT("EnvironmentSelector: Environment '%s' not found"), *EnvironmentId);
		OnEnvironmentLoadFailed(EnvironmentId, TEXT("Environment not found"));
		return;
	}

	const FJellyfinEnvironmentInfo& EnvInfo = EnvironmentMap[EnvironmentId];

	// Check platform compatibility
	if (bFilterByPlatform && IsStandalonePlatform() && EnvInfo.bRequiresPCVR)
	{
		UE_LOG(LogJellyfinVR, Warning, TEXT("EnvironmentSelector: Environment '%s' requires PCVR but running on standalone"),
			*EnvironmentId);
		OnEnvironmentLoadFailed(EnvironmentId, TEXT("This environment requires PC VR"));
		return;
	}

	// Notify that environment is changing
	FString OldEnvironment = CurrentEnvironmentId;
	OnEnvironmentChanging(OldEnvironment, EnvironmentId);

	UE_LOG(LogJellyfinVR, Log, TEXT("EnvironmentSelector: Loading environment '%s' (%s)"),
		*EnvironmentId, *EnvInfo.DisplayName);

	ShowLoading(FString::Printf(TEXT("Loading %s..."), *EnvInfo.DisplayName));

	// Load the environment through the manager
	// Note: This will trigger a level transition
	CurrentEnvironmentId = EnvironmentId;
	EnvironmentManager->LoadEnvironment(EnvironmentId);

	// Success callback (level load is async, so this is optimistic)
	// In a production system, you'd wait for level streaming completion
	OnEnvironmentLoaded(EnvironmentId, EnvInfo.DisplayName);
}

FString UJellyfinEnvironmentSelector::GetCurrentEnvironmentId() const
{
	if (EnvironmentManager)
	{
		return EnvironmentManager->GetCurrentEnvironment();
	}
	return CurrentEnvironmentId;
}

TArray<FJellyfinEnvironmentDisplayInfo> UJellyfinEnvironmentSelector::GetAvailableEnvironments() const
{
	return DisplayEnvironments;
}

void UJellyfinEnvironmentSelector::BuildEnvironmentList()
{
	DisplayEnvironments.Empty();

	// Get filtered or all environments based on settings
	TArray<FJellyfinEnvironmentInfo> SourceEnvironments = bFilterByPlatform
		? EnvironmentManager->GetCompatibleEnvironments()
		: EnvironmentManager->GetAllEnvironments();

	FString CurrentEnv = GetCurrentEnvironmentId();

	// Convert to display info with loaded preview images
	for (const FJellyfinEnvironmentInfo& EnvInfo : SourceEnvironments)
	{
		FJellyfinEnvironmentDisplayInfo DisplayInfo;

		// Find the row name for this environment
		for (const auto& Pair : EnvironmentMap)
		{
			if (Pair.Value.LevelPath == EnvInfo.LevelPath)
			{
				DisplayInfo.EnvironmentId = Pair.Key;
				break;
			}
		}

		DisplayInfo.DisplayName = EnvInfo.DisplayName;
		DisplayInfo.Description = EnvInfo.Description;
		DisplayInfo.bRequiresPCVR = EnvInfo.bRequiresPCVR;
		DisplayInfo.bIsCurrent = (DisplayInfo.EnvironmentId == CurrentEnv) ||
		                         (DisplayInfo.DisplayName == CurrentEnv);

		// Load preview image if available
		if (!EnvInfo.PreviewImage.IsNull())
		{
			DisplayInfo.PreviewImage = EnvInfo.PreviewImage.LoadSynchronous();
		}

		DisplayEnvironments.Add(DisplayInfo);
	}
}

bool UJellyfinEnvironmentSelector::IsStandalonePlatform() const
{
#if PLATFORM_ANDROID
	return true;
#else
	return false;
#endif
}

void UJellyfinEnvironmentSelector::HandleEnvironmentChanged(const FString& EnvironmentId, bool bSuccess)
{
	HideLoading();

	if (bSuccess)
	{
		CurrentEnvironmentId = EnvironmentId;

		// Refresh the list to update current indicator
		BuildEnvironmentList();

		// Find display name
		FString DisplayName = EnvironmentId;
		if (EnvironmentMap.Contains(EnvironmentId))
		{
			DisplayName = EnvironmentMap[EnvironmentId].DisplayName;
		}

		OnEnvironmentLoaded(EnvironmentId, DisplayName);
		UE_LOG(LogJellyfinVR, Log, TEXT("EnvironmentSelector: Environment '%s' loaded successfully"), *EnvironmentId);
	}
	else
	{
		OnEnvironmentLoadFailed(EnvironmentId, TEXT("Failed to load environment level"));
		UE_LOG(LogJellyfinVR, Error, TEXT("EnvironmentSelector: Failed to load environment '%s'"), *EnvironmentId);
	}
}
