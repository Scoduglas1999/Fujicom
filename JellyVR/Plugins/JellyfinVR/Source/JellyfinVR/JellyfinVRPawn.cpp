// Copyright JellyVR Project. All Rights Reserved.

#include "JellyfinVRPawn.h"
#include "JellyfinVRModule.h"
#include "Input/JellyfinVRInput.h"
#include "Input/JellyfinHandTracking.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"

AJellyfinVRPawn::AJellyfinVRPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create VR origin (tracking space root)
	VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
	RootComponent = VROrigin;

	// Create camera
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(VROrigin);
	CameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 160.0f)); // Eye height

	// Create VR input component (handles controllers + desktop mouse)
	VRInputComponent = CreateDefaultSubobject<UJellyfinVRInputComponent>(TEXT("VRInput"));

	// Create hand tracking component
	HandTrackingComponent = CreateDefaultSubobject<UJellyfinHandTrackingComponent>(TEXT("HandTracking"));

	// Set this pawn to be controlled by the player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void AJellyfinVRPawn::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogJellyfinVR, Log, TEXT("JellyfinVRPawn spawned - Input mode will be auto-detected"));
}

void AJellyfinVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Input is handled by UJellyfinVRInputComponent
	// No additional bindings needed here
}
