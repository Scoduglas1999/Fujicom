// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "JellyfinVRPawn.generated.h"

class UJellyfinVRInputComponent;
class UJellyfinHandTrackingComponent;
class UCameraComponent;
class USceneComponent;

/**
 * Simple VR/Desktop pawn for JellyfinVR
 * Supports both VR headset and desktop mouse/keyboard input
 */
UCLASS(BlueprintType, Blueprintable)
class JELLYFINVR_API AJellyfinVRPawn : public APawn
{
	GENERATED_BODY()

public:
	AJellyfinVRPawn();

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/** Get the VR input component */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR")
	UJellyfinVRInputComponent* GetVRInput() const { return VRInputComponent; }

	/** Get the hand tracking component */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR")
	UJellyfinHandTrackingComponent* GetHandTracking() const { return HandTrackingComponent; }

	/** Get the camera component */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR")
	UCameraComponent* GetCamera() const { return CameraComponent; }

protected:
	/** Root scene component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* VROrigin;

	/** Camera for VR/desktop view */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComponent;

	/** VR input handling (controllers + desktop mouse) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UJellyfinVRInputComponent* VRInputComponent;

	/** Hand tracking for Quest 3 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UJellyfinHandTrackingComponent* HandTrackingComponent;
};
