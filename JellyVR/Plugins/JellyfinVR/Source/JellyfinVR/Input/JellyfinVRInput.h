// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "JellyfinVRInput.generated.h"

class UMotionControllerComponent;
class UWidgetInteractionComponent;
class UJellyfinHandTrackingComponent;

UENUM(BlueprintType)
enum class EJellyfinInputMode : uint8
{
	Controller,   // Using VR controllers
	HandTracking, // Using hand tracking
	Desktop       // Using mouse and keyboard (non-VR testing)
};

UENUM(BlueprintType)
enum class EJellyfinHand : uint8
{
	Left,
	Right
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputModeChanged, EJellyfinInputMode, NewMode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPointerHover, UPrimitiveComponent*, HoveredComponent, FVector, HitLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPointerClick, UPrimitiveComponent*, ClickedComponent, FVector, HitLocation);

/**
 * VR input component for Jellyfin
 * Handles controller and hand tracking input for interacting with VR widgets
 */
UCLASS(ClassGroup=(JellyfinVR), meta=(BlueprintSpawnableComponent))
class JELLYFINVR_API UJellyfinVRInputComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UJellyfinVRInputComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ============ Configuration ============

	/** Distance for pointer ray cast */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Input", meta = (ClampMin = "100", ClampMax = "5000"))
	float PointerDistance = 2000.0f;

	/** Enable hand tracking (Quest 3) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Input")
	bool bEnableHandTracking = true;

	/** Auto-switch between controller and hand tracking */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Input")
	bool bAutoSwitchInputMode = true;

	/** Pinch threshold for hand tracking (0-1) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Input", meta = (ClampMin = "0.5", ClampMax = "1.0"))
	float PinchThreshold = 0.8f;

	/** Enable desktop mode when HMD is not detected */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Input")
	bool bEnableDesktopFallback = true;

	/** Mouse scroll sensitivity for desktop mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Input", meta = (ClampMin = "0.1", ClampMax = "5.0"))
	float MouseScrollSensitivity = 1.0f;

	// ============ Runtime State ============

	/** Get current input mode */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Input")
	EJellyfinInputMode GetInputMode() const { return CurrentInputMode; }

	/** Manually set input mode */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Input")
	void SetInputMode(EJellyfinInputMode NewMode);

	/** Get pointer world location for specified hand */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Input")
	FVector GetPointerLocation(EJellyfinHand Hand) const;

	/** Get pointer world direction for specified hand */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Input")
	FVector GetPointerDirection(EJellyfinHand Hand) const;

	/** Check if trigger/pinch is active */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Input")
	bool IsTriggerPressed(EJellyfinHand Hand) const;

	/** Check if grip is active */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Input")
	bool IsGripPressed(EJellyfinHand Hand) const;

	/** Get the currently hovered widget component */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Input")
	UWidgetInteractionComponent* GetWidgetInteraction(EJellyfinHand Hand) const;

	/** Simulate click at current pointer location */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Input")
	void SimulateClick(EJellyfinHand Hand);

	/** Simulate scroll */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Input")
	void SimulateScroll(EJellyfinHand Hand, float ScrollDelta);

	// ============ Events ============

	UPROPERTY(BlueprintAssignable, Category = "JellyfinVR|Events")
	FOnInputModeChanged OnInputModeChanged;

	UPROPERTY(BlueprintAssignable, Category = "JellyfinVR|Events")
	FOnPointerHover OnPointerHover;

	UPROPERTY(BlueprintAssignable, Category = "JellyfinVR|Events")
	FOnPointerClick OnPointerClick;

protected:
	void UpdateControllerInput();
	void UpdateHandTrackingInput();
	void UpdateDesktopInput();
	void PerformRaycast(EJellyfinHand Hand, const FVector& Origin, const FVector& Direction);
	void CheckForInputModeSwitch();
	bool IsHMDConnected() const;

	// Get hand tracking data (OpenXR)
	bool GetHandTrackingPose(EJellyfinHand Hand, FVector& OutPosition, FQuat& OutRotation);
	float GetPinchStrength(EJellyfinHand Hand);

	// Desktop input helpers
	bool GetMouseWorldRay(FVector& OutOrigin, FVector& OutDirection) const;
	void HandleDesktopButtonInput();

private:
	// Widget interaction components for each hand
	UPROPERTY()
	UWidgetInteractionComponent* LeftWidgetInteraction;

	UPROPERTY()
	UWidgetInteractionComponent* RightWidgetInteraction;

	// Current state
	EJellyfinInputMode CurrentInputMode = EJellyfinInputMode::Controller;

	// Input state per hand
	struct FHandInputState
	{
		FVector PointerLocation = FVector::ZeroVector;
		FVector PointerDirection = FVector::ForwardVector;
		bool bTriggerPressed = false;
		bool bTriggerJustPressed = false;
		bool bTriggerJustReleased = false;
		bool bGripPressed = false;
		float PinchStrength = 0.0f;
		UPrimitiveComponent* HoveredComponent = nullptr;
	};

	FHandInputState LeftHandState;
	FHandInputState RightHandState;

	// Motion controller tracking
	UPROPERTY()
	UMotionControllerComponent* LeftController;

	UPROPERTY()
	UMotionControllerComponent* RightController;

	// Hand tracking component reference
	UPROPERTY()
	UJellyfinHandTrackingComponent* HandTrackingComponent;

	// Auto-switching hysteresis tracking
	bool bWasHandTracked = false;
	float HandTrackingLossTime = 0.0f;
	float HandTrackingGainTime = 0.0f;

	/** Time to wait before switching from controller to hand tracking (seconds) */
	static constexpr float HandTrackingActivationDelay = 0.3f;

	/** Time to wait before switching from hand tracking back to controller (seconds) */
	static constexpr float HandTrackingDeactivationDelay = 0.5f;
};
