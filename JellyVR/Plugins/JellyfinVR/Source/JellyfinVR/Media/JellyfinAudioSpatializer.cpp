// Copyright JellyVR Project. All Rights Reserved.

#include "JellyfinAudioSpatializer.h"
#include "JellyfinVRModule.h"
#include "MediaPlayer.h"
#include "MediaSoundComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundAttenuation.h"
#include "Kismet/GameplayStatics.h"

UJellyfinAudioSpatializer::UJellyfinAudioSpatializer()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f; // Update at 10Hz
}

void UJellyfinAudioSpatializer::BeginPlay()
{
	Super::BeginPlay();

	// Create media sound component as a sibling component on the same actor
	if (AActor* Owner = GetOwner())
	{
		MediaSoundComponent = NewObject<UMediaSoundComponent>(Owner, TEXT("JellyfinMediaSound"));
		if (MediaSoundComponent)
		{
			MediaSoundComponent->SetupAttachment(Owner->GetRootComponent());
			MediaSoundComponent->RegisterComponent();

			// Configure for VR spatial audio
			MediaSoundComponent->bAllowSpatialization = true;
			MediaSoundComponent->bOverrideAttenuation = true;

			ApplyAudioMode();

			UE_LOG(LogJellyfinVR, Log, TEXT("JellyfinAudioSpatializer initialized"));
		}
	}
}

void UJellyfinAudioSpatializer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (MediaSoundComponent)
	{
		MediaSoundComponent->Stop();
		MediaSoundComponent->DestroyComponent();
		MediaSoundComponent = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void UJellyfinAudioSpatializer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update spatial position if in 3D mode
	if (AudioMode == EJellyfinAudioMode::Spatial3D && MediaSoundComponent)
	{
		UpdateSpatialPosition();
	}
}

void UJellyfinAudioSpatializer::Initialize(UMediaPlayer* InMediaPlayer)
{
	MediaPlayer = InMediaPlayer;

	if (MediaSoundComponent && MediaPlayer)
	{
		MediaSoundComponent->SetMediaPlayer(MediaPlayer);
		UE_LOG(LogJellyfinVR, Log, TEXT("AudioSpatializer connected to MediaPlayer"));
	}
}

void UJellyfinAudioSpatializer::SetAudioMode(EJellyfinAudioMode Mode)
{
	AudioMode = Mode;
	ApplyAudioMode();
}

void UJellyfinAudioSpatializer::ApplyAudioMode()
{
	if (!MediaSoundComponent)
	{
		return;
	}

	switch (AudioMode)
	{
	case EJellyfinAudioMode::Stereo:
		// Non-spatialized stereo
		MediaSoundComponent->bAllowSpatialization = false;
		MediaSoundComponent->bOverrideAttenuation = false;
		UE_LOG(LogJellyfinVR, Log, TEXT("Audio mode: Stereo"));
		break;

	case EJellyfinAudioMode::Spatial3D:
		// Full 3D spatialization
		MediaSoundComponent->bAllowSpatialization = true;
		MediaSoundComponent->bOverrideAttenuation = true;

		// Configure attenuation
		{
			FSoundAttenuationSettings AttenuationSettings;
			AttenuationSettings.bAttenuate = bEnableAttenuation;
			AttenuationSettings.bSpatialize = true;
			AttenuationSettings.DistanceAlgorithm = EAttenuationDistanceModel::NaturalSound;
			AttenuationSettings.AttenuationShape = EAttenuationShape::Sphere;
			AttenuationSettings.AttenuationShapeExtents = FVector(AttenuationInnerRadius, 0.0f, 0.0f);
			AttenuationSettings.FalloffDistance = AttenuationOuterRadius - AttenuationInnerRadius;
			AttenuationSettings.bEnableOcclusion = bEnableOcclusion;

			// Use HRTF for better VR audio
			AttenuationSettings.SpatializationAlgorithm = ESoundSpatializationAlgorithm::SPATIALIZATION_HRTF;

			MediaSoundComponent->AttenuationOverrides = AttenuationSettings;
		}
		UE_LOG(LogJellyfinVR, Log, TEXT("Audio mode: Spatial3D (Inner: %.0f, Outer: %.0f)"), AttenuationInnerRadius, AttenuationOuterRadius);
		break;

	case EJellyfinAudioMode::Binaural:
		// Binaural/HRTF focused mode
		MediaSoundComponent->bAllowSpatialization = true;
		MediaSoundComponent->bOverrideAttenuation = true;

		{
			FSoundAttenuationSettings AttenuationSettings;
			AttenuationSettings.bAttenuate = false; // No distance falloff
			AttenuationSettings.bSpatialize = true;
			AttenuationSettings.SpatializationAlgorithm = ESoundSpatializationAlgorithm::SPATIALIZATION_HRTF;

			MediaSoundComponent->AttenuationOverrides = AttenuationSettings;
		}
		UE_LOG(LogJellyfinVR, Log, TEXT("Audio mode: Binaural"));
		break;
	}
}

void UJellyfinAudioSpatializer::SetVolume(float Volume)
{
	CurrentVolume = FMath::Clamp(Volume, 0.0f, 1.0f);

	if (MediaSoundComponent && !bIsMuted)
	{
		MediaSoundComponent->SetVolumeMultiplier(CurrentVolume);
	}
}

void UJellyfinAudioSpatializer::SetMuted(bool bMuted)
{
	if (bIsMuted == bMuted)
	{
		return;
	}

	bIsMuted = bMuted;

	if (MediaSoundComponent)
	{
		if (bIsMuted)
		{
			VolumeBeforeMute = CurrentVolume;
			MediaSoundComponent->SetVolumeMultiplier(0.0f);
		}
		else
		{
			MediaSoundComponent->SetVolumeMultiplier(VolumeBeforeMute);
		}
	}
}

void UJellyfinAudioSpatializer::ToggleMute()
{
	SetMuted(!bIsMuted);
}

void UJellyfinAudioSpatializer::SetScreenTransform(const FTransform& ScreenTransform)
{
	ScreenWorldTransform = ScreenTransform;
	UpdateSpatialPosition();
}

void UJellyfinAudioSpatializer::SetScreenDimensions(float Width, float Height)
{
	ScreenWidth = Width;
	ScreenHeight = Height;
}

void UJellyfinAudioSpatializer::SetAttenuationRadius(float InnerRadius, float OuterRadius)
{
	AttenuationInnerRadius = FMath::Max(0.0f, InnerRadius);
	AttenuationOuterRadius = FMath::Max(AttenuationInnerRadius + 1.0f, OuterRadius);

	// Re-apply audio mode to update attenuation settings
	if (AudioMode == EJellyfinAudioMode::Spatial3D)
	{
		ApplyAudioMode();
	}
}

void UJellyfinAudioSpatializer::UpdateSpatialPosition()
{
	if (!MediaSoundComponent)
	{
		return;
	}

	// Position the sound component at the center of the screen
	FVector ScreenCenter = ScreenWorldTransform.GetLocation();

	// For large screens, we could add multiple audio sources across the screen
	// For now, just center it
	MediaSoundComponent->SetWorldLocation(ScreenCenter);
	MediaSoundComponent->SetWorldRotation(ScreenWorldTransform.GetRotation());
}
