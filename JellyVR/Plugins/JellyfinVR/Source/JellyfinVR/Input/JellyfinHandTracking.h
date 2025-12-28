// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "JellyfinHandTracking.generated.h"

UENUM(BlueprintType)
enum class EHandGesture : uint8
{
	None,
	Pinch,           // Index finger to thumb
	Point,           // Index finger extended
	OpenPalm,        // All fingers extended
	Fist,            // All fingers closed
	ThumbsUp,        // Thumb extended, others closed
	Peace            // Index and middle extended
};

UENUM(BlueprintType)
enum class EHandJoint : uint8
{
	Palm,
	Wrist,
	ThumbMetacarpal,
	ThumbProximal,
	ThumbDistal,
	ThumbTip,
	IndexMetacarpal,
	IndexProximal,
	IndexIntermediate,
	IndexDistal,
	IndexTip,
	MiddleMetacarpal,
	MiddleProximal,
	MiddleIntermediate,
	MiddleDistal,
	MiddleTip,
	RingMetacarpal,
	RingProximal,
	RingIntermediate,
	RingDistal,
	RingTip,
	PinkyMetacarpal,
	PinkyProximal,
	PinkyIntermediate,
	PinkyDistal,
	PinkyTip
};

USTRUCT(BlueprintType)
struct FHandJointData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "HandTracking")
	FVector Location = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "HandTracking")
	FQuat Rotation = FQuat::Identity;

	UPROPERTY(BlueprintReadOnly, Category = "HandTracking")
	float Radius = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "HandTracking")
	bool bIsTracked = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGestureDetected, bool, bIsLeftHand, EHandGesture, Gesture);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPinchStateChanged, bool, bIsLeftHand, bool, bIsPinching);

/**
 * Hand tracking component using OpenXR
 * Provides gesture detection and joint tracking for Quest 3
 */
UCLASS(ClassGroup=(JellyfinVR), meta=(BlueprintSpawnableComponent))
class JELLYFINVR_API UJellyfinHandTrackingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UJellyfinHandTrackingComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ============ Hand State ============

	/**
	 * Check if hand tracking is available
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|HandTracking")
	bool IsHandTrackingAvailable() const;

	/**
	 * Check if specific hand is being tracked
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|HandTracking")
	bool IsHandTracked(bool bLeftHand) const;

	/**
	 * Get joint data for specified hand and joint
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|HandTracking")
	FHandJointData GetJointData(bool bLeftHand, EHandJoint Joint) const;

	/**
	 * Get pointer pose (for ray casting from index finger)
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|HandTracking")
	bool GetPointerPose(bool bLeftHand, FVector& OutLocation, FQuat& OutRotation) const;

	/**
	 * Get current pinch strength (0-1)
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|HandTracking")
	float GetPinchStrength(bool bLeftHand) const;

	/**
	 * Check if currently pinching
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|HandTracking")
	bool IsPinching(bool bLeftHand) const;

	/**
	 * Get current detected gesture
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|HandTracking")
	EHandGesture GetCurrentGesture(bool bLeftHand) const;

	// ============ Configuration ============

	/** Threshold for pinch detection (0-1) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|HandTracking", meta = (ClampMin = "0.5", ClampMax = "1.0"))
	float PinchThreshold = 0.7f;

	/** Hysteresis for pinch release (prevents flickering) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|HandTracking", meta = (ClampMin = "0.0", ClampMax = "0.3"))
	float PinchHysteresis = 0.1f;

	/** Distance threshold for pinch detection in cm */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|HandTracking", meta = (ClampMin = "1.0", ClampMax = "5.0"))
	float PinchDistanceThreshold = 2.5f;

	// ============ Events ============

	UPROPERTY(BlueprintAssignable, Category = "JellyfinVR|HandTracking")
	FOnGestureDetected OnGestureDetected;

	UPROPERTY(BlueprintAssignable, Category = "JellyfinVR|HandTracking")
	FOnPinchStateChanged OnPinchStateChanged;

protected:
	void UpdateHandData(bool bLeftHand);
	void UpdateHandDataFromController(bool bLeftHand);
	float CalculatePinchStrength(bool bLeftHand);
	EHandGesture DetectGesture(bool bLeftHand);
	float GetFingerCurl(bool bLeftHand, int32 FingerIndex) const;
	bool IsFingerExtended(bool bLeftHand, int32 FingerIndex) const;

private:
	// Joint data storage
	TMap<EHandJoint, FHandJointData> LeftHandJoints;
	TMap<EHandJoint, FHandJointData> RightHandJoints;

	// State
	bool bLeftHandTracked = false;
	bool bRightHandTracked = false;
	float LeftPinchStrength = 0.0f;
	float RightPinchStrength = 0.0f;
	bool bLeftPinching = false;
	bool bRightPinching = false;
	EHandGesture LeftGesture = EHandGesture::None;
	EHandGesture RightGesture = EHandGesture::None;

	bool bHandTrackingAvailable = false;
};
