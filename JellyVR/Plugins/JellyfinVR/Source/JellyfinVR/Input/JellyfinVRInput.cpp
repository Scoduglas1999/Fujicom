// Copyright JellyVR Project. All Rights Reserved.

#include "JellyfinVRInput.h"
#include "JellyfinVRModule.h"
#include "JellyfinHandTracking.h"
#include "MotionControllerComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "IXRTrackingSystem.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/Application/SlateApplication.h"

UJellyfinVRInputComponent::UJellyfinVRInputComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.0f; // Tick every frame for responsive input
}

void UJellyfinVRInputComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		UE_LOG(LogJellyfinVR, Error, TEXT("JellyfinVRInputComponent has no owner"));
		return;
	}

	// Create widget interaction components for each hand
	LeftWidgetInteraction = NewObject<UWidgetInteractionComponent>(Owner);
	LeftWidgetInteraction->RegisterComponent();
	LeftWidgetInteraction->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	LeftWidgetInteraction->InteractionDistance = PointerDistance;
	LeftWidgetInteraction->bShowDebug = false;
	LeftWidgetInteraction->PointerIndex = 0;

	RightWidgetInteraction = NewObject<UWidgetInteractionComponent>(Owner);
	RightWidgetInteraction->RegisterComponent();
	RightWidgetInteraction->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	RightWidgetInteraction->InteractionDistance = PointerDistance;
	RightWidgetInteraction->bShowDebug = false;
	RightWidgetInteraction->PointerIndex = 1;

	// Find motion controllers if they exist on the owner
	TArray<UMotionControllerComponent*> MotionControllers;
	Owner->GetComponents<UMotionControllerComponent>(MotionControllers);

	for (UMotionControllerComponent* MC : MotionControllers)
	{
		if (MC->GetTrackingSource() == EControllerHand::Left)
		{
			LeftController = MC;
		}
		else if (MC->GetTrackingSource() == EControllerHand::Right)
		{
			RightController = MC;
		}
	}

	// Find hand tracking component if it exists on the owner
	HandTrackingComponent = Owner->FindComponentByClass<UJellyfinHandTrackingComponent>();
	if (HandTrackingComponent && bEnableHandTracking)
	{
		UE_LOG(LogJellyfinVR, Log, TEXT("JellyfinVRInputComponent found hand tracking component"));
	}

	// Auto-detect HMD and set appropriate input mode
	if (bEnableDesktopFallback && !IsHMDConnected())
	{
		CurrentInputMode = EJellyfinInputMode::Desktop;

		// Enable mouse cursor and keyboard input for desktop mode
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		{
			PC->SetShowMouseCursor(true);
			PC->SetInputMode(FInputModeGameAndUI());

			// IMPORTANT: In desktop mode, viewport widgets receive input directly from Slate.
			// We must DISABLE WidgetInteractionComponent hit testing to prevent interference.
			// WidgetInteractionComponent is only for 3D world-space widgets (VR mode).
			if (LeftWidgetInteraction)
			{
				LeftWidgetInteraction->bEnableHitTesting = false;
			}
			if (RightWidgetInteraction)
			{
				RightWidgetInteraction->bEnableHitTesting = false;
			}
		}

		UE_LOG(LogJellyfinVR, Log, TEXT("JellyfinVRInputComponent initialized - Desktop mode (no HMD detected)"));
	}
	else
	{
		// VR mode - hide system cursor
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		{
			PC->SetShowMouseCursor(false);
		}

		UE_LOG(LogJellyfinVR, Log, TEXT("JellyfinVRInputComponent initialized - VR mode - Hand tracking: %s"),
			bEnableHandTracking ? TEXT("enabled") : TEXT("disabled"));
	}
}

void UJellyfinVRInputComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Check for input mode switches
	if (bAutoSwitchInputMode)
	{
		CheckForInputModeSwitch();
	}

	// Update input based on current mode
	if (CurrentInputMode == EJellyfinInputMode::Controller)
	{
		UpdateControllerInput();
	}
	else if (CurrentInputMode == EJellyfinInputMode::HandTracking)
	{
		UpdateHandTrackingInput();
	}
	else if (CurrentInputMode == EJellyfinInputMode::Desktop)
	{
		UpdateDesktopInput();
	}
}

void UJellyfinVRInputComponent::SetInputMode(EJellyfinInputMode NewMode)
{
	if (CurrentInputMode != NewMode)
	{
		CurrentInputMode = NewMode;
		OnInputModeChanged.Broadcast(NewMode);

		// Update cursor visibility based on mode
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		{
			if (NewMode == EJellyfinInputMode::Desktop)
			{
				PC->SetShowMouseCursor(true);
				PC->SetInputMode(FInputModeGameAndUI());
			}
			else
			{
				PC->SetShowMouseCursor(false);
				PC->SetInputMode(FInputModeGameOnly());
			}
		}

		const TCHAR* ModeName = TEXT("Unknown");
		switch (NewMode)
		{
		case EJellyfinInputMode::Controller:
			ModeName = TEXT("Controller");
			break;
		case EJellyfinInputMode::HandTracking:
			ModeName = TEXT("HandTracking");
			break;
		case EJellyfinInputMode::Desktop:
			ModeName = TEXT("Desktop");
			break;
		}

		UE_LOG(LogJellyfinVR, Log, TEXT("Input mode changed to: %s"), ModeName);
	}
}

void UJellyfinVRInputComponent::UpdateControllerInput()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC)
	{
		return;
	}

	// Get controller positions
	if (LeftController)
	{
		LeftHandState.PointerLocation = LeftController->GetComponentLocation();
		LeftHandState.PointerDirection = LeftController->GetForwardVector();

		// Update widget interaction
		if (LeftWidgetInteraction)
		{
			LeftWidgetInteraction->SetWorldLocationAndRotation(
				LeftHandState.PointerLocation,
				LeftController->GetComponentRotation()
			);
		}
	}

	if (RightController)
	{
		RightHandState.PointerLocation = RightController->GetComponentLocation();
		RightHandState.PointerDirection = RightController->GetForwardVector();

		// Update widget interaction
		if (RightWidgetInteraction)
		{
			RightWidgetInteraction->SetWorldLocationAndRotation(
				RightHandState.PointerLocation,
				RightController->GetComponentRotation()
			);
		}
	}

	// Get button states using generic gamepad trigger axes
	// Note: For proper VR controller support, consider using Enhanced Input with OpenXR action mappings
	// Use gamepad axes as fallback - VR controllers typically map to these
	float LeftTrigger = PC->GetInputAnalogKeyState(EKeys::Gamepad_LeftTriggerAxis);
	float RightTrigger = PC->GetInputAnalogKeyState(EKeys::Gamepad_RightTriggerAxis);
	// Grip uses shoulder buttons on gamepads
	float LeftGrip = PC->IsInputKeyDown(EKeys::Gamepad_LeftShoulder) ? 1.0f : 0.0f;
	float RightGrip = PC->IsInputKeyDown(EKeys::Gamepad_RightShoulder) ? 1.0f : 0.0f;

	// Update left hand state
	bool bPrevLeftTrigger = LeftHandState.bTriggerPressed;
	LeftHandState.bTriggerPressed = LeftTrigger > 0.5f;
	LeftHandState.bTriggerJustPressed = LeftHandState.bTriggerPressed && !bPrevLeftTrigger;
	LeftHandState.bTriggerJustReleased = !LeftHandState.bTriggerPressed && bPrevLeftTrigger;
	LeftHandState.bGripPressed = LeftGrip > 0.5f;

	// Update right hand state
	bool bPrevRightTrigger = RightHandState.bTriggerPressed;
	RightHandState.bTriggerPressed = RightTrigger > 0.5f;
	RightHandState.bTriggerJustPressed = RightHandState.bTriggerPressed && !bPrevRightTrigger;
	RightHandState.bTriggerJustReleased = !RightHandState.bTriggerPressed && bPrevRightTrigger;
	RightHandState.bGripPressed = RightGrip > 0.5f;

	// Handle clicks
	if (LeftHandState.bTriggerJustPressed && LeftWidgetInteraction)
	{
		LeftWidgetInteraction->PressPointerKey(EKeys::LeftMouseButton);
	}
	if (LeftHandState.bTriggerJustReleased && LeftWidgetInteraction)
	{
		LeftWidgetInteraction->ReleasePointerKey(EKeys::LeftMouseButton);
	}

	if (RightHandState.bTriggerJustPressed && RightWidgetInteraction)
	{
		RightWidgetInteraction->PressPointerKey(EKeys::LeftMouseButton);
	}
	if (RightHandState.bTriggerJustReleased && RightWidgetInteraction)
	{
		RightWidgetInteraction->ReleasePointerKey(EKeys::LeftMouseButton);
	}
}

void UJellyfinVRInputComponent::UpdateHandTrackingInput()
{
	// Get hand tracking data for left hand
	FVector LeftPos;
	FQuat LeftRot;
	if (GetHandTrackingPose(EJellyfinHand::Left, LeftPos, LeftRot))
	{
		LeftHandState.PointerLocation = LeftPos;
		LeftHandState.PointerDirection = LeftRot.GetForwardVector();
		LeftHandState.PinchStrength = GetPinchStrength(EJellyfinHand::Left);

		// Update widget interaction
		if (LeftWidgetInteraction)
		{
			LeftWidgetInteraction->SetWorldLocationAndRotation(LeftPos, LeftRot.Rotator());
		}

		// Handle pinch as trigger
		bool bPrevTrigger = LeftHandState.bTriggerPressed;
		LeftHandState.bTriggerPressed = LeftHandState.PinchStrength > PinchThreshold;
		LeftHandState.bTriggerJustPressed = LeftHandState.bTriggerPressed && !bPrevTrigger;
		LeftHandState.bTriggerJustReleased = !LeftHandState.bTriggerPressed && bPrevTrigger;

		if (LeftHandState.bTriggerJustPressed && LeftWidgetInteraction)
		{
			LeftWidgetInteraction->PressPointerKey(EKeys::LeftMouseButton);
		}
		if (LeftHandState.bTriggerJustReleased && LeftWidgetInteraction)
		{
			LeftWidgetInteraction->ReleasePointerKey(EKeys::LeftMouseButton);
		}
	}

	// Get hand tracking data for right hand
	FVector RightPos;
	FQuat RightRot;
	if (GetHandTrackingPose(EJellyfinHand::Right, RightPos, RightRot))
	{
		RightHandState.PointerLocation = RightPos;
		RightHandState.PointerDirection = RightRot.GetForwardVector();
		RightHandState.PinchStrength = GetPinchStrength(EJellyfinHand::Right);

		// Update widget interaction
		if (RightWidgetInteraction)
		{
			RightWidgetInteraction->SetWorldLocationAndRotation(RightPos, RightRot.Rotator());
		}

		// Handle pinch as trigger
		bool bPrevTrigger = RightHandState.bTriggerPressed;
		RightHandState.bTriggerPressed = RightHandState.PinchStrength > PinchThreshold;
		RightHandState.bTriggerJustPressed = RightHandState.bTriggerPressed && !bPrevTrigger;
		RightHandState.bTriggerJustReleased = !RightHandState.bTriggerPressed && bPrevTrigger;

		if (RightHandState.bTriggerJustPressed && RightWidgetInteraction)
		{
			RightWidgetInteraction->PressPointerKey(EKeys::LeftMouseButton);
		}
		if (RightHandState.bTriggerJustReleased && RightWidgetInteraction)
		{
			RightWidgetInteraction->ReleasePointerKey(EKeys::LeftMouseButton);
		}
	}
}

bool UJellyfinVRInputComponent::GetHandTrackingPose(EJellyfinHand Hand, FVector& OutPosition, FQuat& OutRotation)
{
	// Use OpenXR hand tracking
	IXRTrackingSystem* XRSystem = GEngine->XRSystem.Get();
	if (!XRSystem)
	{
		return false;
	}

	// Get the motion controller data which includes hand tracking on Quest
	EControllerHand ControllerHand = (Hand == EJellyfinHand::Left) ? EControllerHand::Left : EControllerHand::Right;

	// Try to get hand tracking pose
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		// Get controller/hand position
		bool bSuccess = false;

		// For OpenXR hand tracking, we'd use the XR hand tracking extension
		// This is a simplified version - in production you'd use the OpenXR hand tracking API
		if (ControllerHand == EControllerHand::Left && LeftController)
		{
			OutPosition = LeftController->GetComponentLocation();
			OutRotation = LeftController->GetComponentQuat();
			bSuccess = true;
		}
		else if (ControllerHand == EControllerHand::Right && RightController)
		{
			OutPosition = RightController->GetComponentLocation();
			OutRotation = RightController->GetComponentQuat();
			bSuccess = true;
		}

		return bSuccess;
	}

	return false;
}

float UJellyfinVRInputComponent::GetPinchStrength(EJellyfinHand Hand)
{
	// In a full implementation, this would use OpenXR hand tracking to get pinch strength
	// For now, return 0 (no pinch) - this would be expanded with proper OpenXR integration

	// Placeholder: Return based on controller trigger if in controller mode
	if (CurrentInputMode == EJellyfinInputMode::Controller)
	{
		return Hand == EJellyfinHand::Left ? (LeftHandState.bTriggerPressed ? 1.0f : 0.0f) :
			(RightHandState.bTriggerPressed ? 1.0f : 0.0f);
	}

	return 0.0f;
}

void UJellyfinVRInputComponent::CheckForInputModeSwitch()
{
	// Don't auto-switch in desktop mode
	if (CurrentInputMode == EJellyfinInputMode::Desktop)
	{
		return;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC)
	{
		return;
	}

	// Get current hand tracking state
	bool bHandsTrackedNow = false;
	if (HandTrackingComponent && bEnableHandTracking)
	{
		// Check if either hand is being tracked
		bHandsTrackedNow = HandTrackingComponent->IsHandTracked(true) || HandTrackingComponent->IsHandTracked(false);
	}

	// Check if controllers are being used (any button pressed)
	bool bAnyControllerInput = false;
	float LeftTriggerVal = PC->GetInputAnalogKeyState(EKeys::Gamepad_LeftTriggerAxis);
	bAnyControllerInput |= LeftTriggerVal > 0.1f;
	float RightTriggerVal = PC->GetInputAnalogKeyState(EKeys::Gamepad_RightTriggerAxis);
	bAnyControllerInput |= RightTriggerVal > 0.1f;

	// Get delta time for hysteresis tracking
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	// === Handle switching FROM Hand Tracking TO Controller ===
	if (CurrentInputMode == EJellyfinInputMode::HandTracking)
	{
		// If controller input detected, switch immediately (user picked up controller)
		if (bAnyControllerInput)
		{
			SetInputMode(EJellyfinInputMode::Controller);
			HandTrackingLossTime = 0.0f;
			HandTrackingGainTime = 0.0f;
			bWasHandTracked = false;
			UE_LOG(LogJellyfinVR, Verbose, TEXT("Auto-switched to Controller mode (controller input detected)"));
			return;
		}

		// If hands lost tracking, wait with hysteresis before switching back
		if (!bHandsTrackedNow)
		{
			HandTrackingLossTime += DeltaTime;

			// Only switch back to controller after delay (prevents flickering)
			if (HandTrackingLossTime >= HandTrackingDeactivationDelay)
			{
				SetInputMode(EJellyfinInputMode::Controller);
				HandTrackingLossTime = 0.0f;
				HandTrackingGainTime = 0.0f;
				bWasHandTracked = false;
				UE_LOG(LogJellyfinVR, Log, TEXT("Auto-switched to Controller mode (hands lost for %.2fs)"),
					HandTrackingDeactivationDelay);
			}
		}
		else
		{
			// Hands still tracked, reset loss timer
			HandTrackingLossTime = 0.0f;
		}
	}
	// === Handle switching FROM Controller TO Hand Tracking ===
	else if (CurrentInputMode == EJellyfinInputMode::Controller)
	{
		// Detect transition: hands just became tracked
		if (bHandsTrackedNow && !bWasHandTracked)
		{
			HandTrackingGainTime += DeltaTime;

			// Only switch to hand tracking after delay (prevents false positives)
			// and if no controller input is happening
			if (HandTrackingGainTime >= HandTrackingActivationDelay && !bAnyControllerInput)
			{
				SetInputMode(EJellyfinInputMode::HandTracking);
				HandTrackingGainTime = 0.0f;
				HandTrackingLossTime = 0.0f;
				UE_LOG(LogJellyfinVR, Log, TEXT("Auto-switched to HandTracking mode (hands detected for %.2fs)"),
					HandTrackingActivationDelay);
			}
		}
		else if (!bHandsTrackedNow)
		{
			// Hands not tracked anymore, reset gain timer
			HandTrackingGainTime = 0.0f;
		}

		// If controller input detected while waiting, cancel hand tracking switch
		if (bAnyControllerInput)
		{
			HandTrackingGainTime = 0.0f;
		}
	}

	// Update previous state for edge detection
	bWasHandTracked = bHandsTrackedNow;
}

FVector UJellyfinVRInputComponent::GetPointerLocation(EJellyfinHand Hand) const
{
	return Hand == EJellyfinHand::Left ? LeftHandState.PointerLocation : RightHandState.PointerLocation;
}

FVector UJellyfinVRInputComponent::GetPointerDirection(EJellyfinHand Hand) const
{
	return Hand == EJellyfinHand::Left ? LeftHandState.PointerDirection : RightHandState.PointerDirection;
}

bool UJellyfinVRInputComponent::IsTriggerPressed(EJellyfinHand Hand) const
{
	return Hand == EJellyfinHand::Left ? LeftHandState.bTriggerPressed : RightHandState.bTriggerPressed;
}

bool UJellyfinVRInputComponent::IsGripPressed(EJellyfinHand Hand) const
{
	return Hand == EJellyfinHand::Left ? LeftHandState.bGripPressed : RightHandState.bGripPressed;
}

UWidgetInteractionComponent* UJellyfinVRInputComponent::GetWidgetInteraction(EJellyfinHand Hand) const
{
	return Hand == EJellyfinHand::Left ? LeftWidgetInteraction : RightWidgetInteraction;
}

void UJellyfinVRInputComponent::SimulateClick(EJellyfinHand Hand)
{
	UWidgetInteractionComponent* WidgetInteraction = GetWidgetInteraction(Hand);
	if (WidgetInteraction)
	{
		WidgetInteraction->PressPointerKey(EKeys::LeftMouseButton);
		WidgetInteraction->ReleasePointerKey(EKeys::LeftMouseButton);
	}
}

void UJellyfinVRInputComponent::SimulateScroll(EJellyfinHand Hand, float ScrollDelta)
{
	UWidgetInteractionComponent* WidgetInteraction = GetWidgetInteraction(Hand);
	if (WidgetInteraction)
	{
		WidgetInteraction->ScrollWheel(ScrollDelta);
	}
}

void UJellyfinVRInputComponent::UpdateDesktopInput()
{
	// In desktop mode with viewport overlay widgets, Slate handles ALL input directly.
	// We only track button states for gameplay code that may need to query input state.
	//
	// IMPORTANT: Do NOT use WidgetInteractionComponent for viewport widgets!
	// - WidgetInteractionComponent is for 3D world-space UMG widgets
	// - Viewport overlay widgets (AddToViewport) use Slate's native input routing
	// - Mouse clicks, scroll, keyboard all go through Slate automatically

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC)
	{
		return;
	}

	// Track button states for any gameplay code that needs to query input
	bool bPrevTrigger = RightHandState.bTriggerPressed;
	RightHandState.bTriggerPressed = PC->IsInputKeyDown(EKeys::LeftMouseButton);
	RightHandState.bTriggerJustPressed = RightHandState.bTriggerPressed && !bPrevTrigger;
	RightHandState.bTriggerJustReleased = !RightHandState.bTriggerPressed && bPrevTrigger;
	RightHandState.bGripPressed = PC->IsInputKeyDown(EKeys::RightMouseButton);

	// Note: Scroll wheel is also handled natively by Slate for viewport widgets.
	// SScrollBox and other scrollable widgets receive WM_MOUSEWHEEL directly.
}

bool UJellyfinVRInputComponent::GetMouseWorldRay(FVector& OutOrigin, FVector& OutDirection) const
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC)
	{
		return false;
	}

	// Get mouse position in screen space
	float MouseX, MouseY;
	if (!PC->GetMousePosition(MouseX, MouseY))
	{
		return false;
	}

	// Convert screen position to world ray
	return PC->DeprojectScreenPositionToWorld(MouseX, MouseY, OutOrigin, OutDirection);
}

void UJellyfinVRInputComponent::HandleDesktopButtonInput()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC || !RightWidgetInteraction)
	{
		return;
	}

	// Left mouse button = trigger (select/click)
	bool bPrevTrigger = RightHandState.bTriggerPressed;
	RightHandState.bTriggerPressed = PC->IsInputKeyDown(EKeys::LeftMouseButton);
	RightHandState.bTriggerJustPressed = RightHandState.bTriggerPressed && !bPrevTrigger;
	RightHandState.bTriggerJustReleased = !RightHandState.bTriggerPressed && bPrevTrigger;

	// Right mouse button = grip (alternative action)
	RightHandState.bGripPressed = PC->IsInputKeyDown(EKeys::RightMouseButton);

	// Handle widget interaction for left mouse button
	if (RightHandState.bTriggerJustPressed)
	{
		RightWidgetInteraction->PressPointerKey(EKeys::LeftMouseButton);
	}
	else if (RightHandState.bTriggerJustReleased)
	{
		RightWidgetInteraction->ReleasePointerKey(EKeys::LeftMouseButton);
	}

	// Left hand is inactive in desktop mode
	LeftHandState.bTriggerPressed = false;
	LeftHandState.bTriggerJustPressed = false;
	LeftHandState.bTriggerJustReleased = false;
	LeftHandState.bGripPressed = false;
}

bool UJellyfinVRInputComponent::IsHMDConnected() const
{
	return UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() &&
		UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected();
}
