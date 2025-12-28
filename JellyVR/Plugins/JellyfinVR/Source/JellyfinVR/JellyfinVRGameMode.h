// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "JellyfinVRGameMode.generated.h"

/**
 * GameMode for JellyfinVR
 * Spawns the JellyfinVRPawn which supports VR and desktop input
 */
UCLASS(BlueprintType, Blueprintable)
class JELLYFINVR_API AJellyfinVRGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AJellyfinVRGameMode();
};
