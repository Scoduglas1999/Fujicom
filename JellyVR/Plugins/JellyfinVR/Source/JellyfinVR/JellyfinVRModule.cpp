// Copyright JellyVR Project. All Rights Reserved.

#include "JellyfinVRModule.h"

#define LOCTEXT_NAMESPACE "FJellyfinVRModule"

DEFINE_LOG_CATEGORY(LogJellyfinVR);

void FJellyfinVRModule::StartupModule()
{
	UE_LOG(LogJellyfinVR, Log, TEXT("JellyfinVR module starting up"));
}

void FJellyfinVRModule::ShutdownModule()
{
	UE_LOG(LogJellyfinVR, Log, TEXT("JellyfinVR module shutting down"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FJellyfinVRModule, JellyfinVR)
