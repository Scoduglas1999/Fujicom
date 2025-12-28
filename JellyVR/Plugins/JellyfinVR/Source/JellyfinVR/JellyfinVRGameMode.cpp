// Copyright JellyVR Project. All Rights Reserved.

#include "JellyfinVRGameMode.h"
#include "JellyfinVRPawn.h"

AJellyfinVRGameMode::AJellyfinVRGameMode()
{
	// Use JellyfinVRPawn as the default pawn
	DefaultPawnClass = AJellyfinVRPawn::StaticClass();
}
