// Copyright JellyVR Project. All Rights Reserved.

#include "JellyfinHandTracking.h"
#include "JellyfinVRModule.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "HeadMountedDisplayTypes.h"
#include "IXRTrackingSystem.h"
#include "InputCoreTypes.h"

UJellyfinHandTrackingComponent::UJellyfinHandTrackingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.0f; // Every frame for responsive tracking
}

void UJellyfinHandTrackingComponent::BeginPlay()
{
	Super::BeginPlay();

	// Check if XR system is available
	if (GEngine && GEngine->XRSystem.IsValid())
	{
		bHandTrackingAvailable = UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled();
	}

	if (bHandTrackingAvailable)
	{
		UE_LOG(LogJellyfinVR, Log, TEXT("Hand tracking initialized - XR system available"));
	}
	else
	{
		UE_LOG(LogJellyfinVR, Log, TEXT("Hand tracking not available - no XR system detected"));
	}
}

void UJellyfinHandTrackingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bHandTrackingAvailable)
	{
		return;
	}

	// Update both hands
	UpdateHandData(true);  // Left hand
	UpdateHandData(false); // Right hand
}

bool UJellyfinHandTrackingComponent::IsHandTrackingAvailable() const
{
	return bHandTrackingAvailable;
}

bool UJellyfinHandTrackingComponent::IsHandTracked(bool bLeftHand) const
{
	return bLeftHand ? bLeftHandTracked : bRightHandTracked;
}

void UJellyfinHandTrackingComponent::UpdateHandData(bool bLeftHand)
{
	// Get hand tracking data from OpenXR via UE5.7's GetHandTrackingState API
	FXRHandTrackingState HandTrackingState;
	EControllerHand Hand = bLeftHand ? EControllerHand::Left : EControllerHand::Right;

	UHeadMountedDisplayFunctionLibrary::GetHandTrackingState(
		GetOwner(),
		EXRSpaceType::UnrealWorldSpace,
		Hand,
		HandTrackingState
	);

	// Check if we got valid hand tracking data
	// Hand tracking provides 26 joint positions (EHandKeypointCount)
	bool bHasHandTracking = HandTrackingState.bValid &&
	                        HandTrackingState.TrackingStatus != ETrackingStatus::NotTracked &&
	                        HandTrackingState.HandKeyLocations.Num() >= 26;

	if (!bHasHandTracking)
	{
		// Fall back to controller-based approximation
		UpdateHandDataFromController(bLeftHand);
		return;
	}

	TMap<EHandJoint, FHandJointData>& JointMap = bLeftHand ? LeftHandJoints : RightHandJoints;
	bool& bTracked = bLeftHand ? bLeftHandTracked : bRightHandTracked;

	bTracked = true;
	JointMap.Empty();

	// UE5.7 EHandKeypoint enum order matches our EHandJoint:
	// 0: Palm, 1: Wrist
	// 2-5: Thumb (Metacarpal, Proximal, Distal, Tip)
	// 6-10: Index (Metacarpal, Proximal, Intermediate, Distal, Tip)
	// 11-15: Middle, 16-20: Ring, 21-25: Little (Pinky)

	// Map hand keypoint indices to our EHandJoint enum
	auto MapJoint = [&](EHandJoint Joint, int32 KeypointIndex)
	{
		if (KeypointIndex < HandTrackingState.HandKeyLocations.Num())
		{
			FHandJointData JointData;
			JointData.Location = HandTrackingState.HandKeyLocations[KeypointIndex];
			JointData.bIsTracked = true;

			if (KeypointIndex < HandTrackingState.HandKeyRotations.Num())
			{
				JointData.Rotation = HandTrackingState.HandKeyRotations[KeypointIndex];
			}
			if (KeypointIndex < HandTrackingState.HandKeyRadii.Num())
			{
				JointData.Radius = HandTrackingState.HandKeyRadii[KeypointIndex];
			}

			JointMap.Add(Joint, JointData);
		}
	};

	// Map all 26 joints using EHandKeypoint indices
	MapJoint(EHandJoint::Palm, static_cast<int32>(EHandKeypoint::Palm));
	MapJoint(EHandJoint::Wrist, static_cast<int32>(EHandKeypoint::Wrist));

	// Thumb
	MapJoint(EHandJoint::ThumbMetacarpal, static_cast<int32>(EHandKeypoint::ThumbMetacarpal));
	MapJoint(EHandJoint::ThumbProximal, static_cast<int32>(EHandKeypoint::ThumbProximal));
	MapJoint(EHandJoint::ThumbDistal, static_cast<int32>(EHandKeypoint::ThumbDistal));
	MapJoint(EHandJoint::ThumbTip, static_cast<int32>(EHandKeypoint::ThumbTip));

	// Index finger
	MapJoint(EHandJoint::IndexMetacarpal, static_cast<int32>(EHandKeypoint::IndexMetacarpal));
	MapJoint(EHandJoint::IndexProximal, static_cast<int32>(EHandKeypoint::IndexProximal));
	MapJoint(EHandJoint::IndexIntermediate, static_cast<int32>(EHandKeypoint::IndexIntermediate));
	MapJoint(EHandJoint::IndexDistal, static_cast<int32>(EHandKeypoint::IndexDistal));
	MapJoint(EHandJoint::IndexTip, static_cast<int32>(EHandKeypoint::IndexTip));

	// Middle finger
	MapJoint(EHandJoint::MiddleMetacarpal, static_cast<int32>(EHandKeypoint::MiddleMetacarpal));
	MapJoint(EHandJoint::MiddleProximal, static_cast<int32>(EHandKeypoint::MiddleProximal));
	MapJoint(EHandJoint::MiddleIntermediate, static_cast<int32>(EHandKeypoint::MiddleIntermediate));
	MapJoint(EHandJoint::MiddleDistal, static_cast<int32>(EHandKeypoint::MiddleDistal));
	MapJoint(EHandJoint::MiddleTip, static_cast<int32>(EHandKeypoint::MiddleTip));

	// Ring finger
	MapJoint(EHandJoint::RingMetacarpal, static_cast<int32>(EHandKeypoint::RingMetacarpal));
	MapJoint(EHandJoint::RingProximal, static_cast<int32>(EHandKeypoint::RingProximal));
	MapJoint(EHandJoint::RingIntermediate, static_cast<int32>(EHandKeypoint::RingIntermediate));
	MapJoint(EHandJoint::RingDistal, static_cast<int32>(EHandKeypoint::RingDistal));
	MapJoint(EHandJoint::RingTip, static_cast<int32>(EHandKeypoint::RingTip));

	// Pinky finger (UE5.7 calls it "Little")
	MapJoint(EHandJoint::PinkyMetacarpal, static_cast<int32>(EHandKeypoint::LittleMetacarpal));
	MapJoint(EHandJoint::PinkyProximal, static_cast<int32>(EHandKeypoint::LittleProximal));
	MapJoint(EHandJoint::PinkyIntermediate, static_cast<int32>(EHandKeypoint::LittleIntermediate));
	MapJoint(EHandJoint::PinkyDistal, static_cast<int32>(EHandKeypoint::LittleDistal));
	MapJoint(EHandJoint::PinkyTip, static_cast<int32>(EHandKeypoint::LittleTip));

	// Update pinch state with real hand data
	float& PinchStrengthRef = bLeftHand ? LeftPinchStrength : RightPinchStrength;
	bool& bPinching = bLeftHand ? bLeftPinching : bRightPinching;

	float NewPinchStrength = CalculatePinchStrength(bLeftHand);
	PinchStrengthRef = NewPinchStrength;

	// Apply hysteresis to pinch detection
	bool bWasPinching = bPinching;
	if (bPinching)
	{
		// Release requires lower threshold (hysteresis)
		bPinching = NewPinchStrength >= (PinchThreshold - PinchHysteresis);
	}
	else
	{
		// Pinch requires higher threshold
		bPinching = NewPinchStrength >= PinchThreshold;
	}

	// Broadcast pinch state change
	if (bPinching != bWasPinching)
	{
		OnPinchStateChanged.Broadcast(bLeftHand, bPinching);
	}

	// Update gesture detection
	EHandGesture& CurrentGesture = bLeftHand ? LeftGesture : RightGesture;
	EHandGesture NewGesture = DetectGesture(bLeftHand);
	if (NewGesture != CurrentGesture)
	{
		CurrentGesture = NewGesture;
		if (NewGesture != EHandGesture::None)
		{
			OnGestureDetected.Broadcast(bLeftHand, NewGesture);
		}
	}
}

void UJellyfinHandTrackingComponent::UpdateHandDataFromController(bool bLeftHand)
{
	// Fallback method when hand tracking is not available
	// Uses controller position to approximate hand pose for basic pointer functionality

	if (!GEngine || !GEngine->XRSystem.IsValid())
	{
		return;
	}

	IXRTrackingSystem* XRSystem = GEngine->XRSystem.Get();
	if (!XRSystem)
	{
		return;
	}

	TMap<EHandJoint, FHandJointData>& JointMap = bLeftHand ? LeftHandJoints : RightHandJoints;
	bool& bTracked = bLeftHand ? bLeftHandTracked : bRightHandTracked;

	// Get controller pose
	FVector GripPosition;
	FQuat GripRotation;

	int32 DeviceId = bLeftHand ? 1 : 2; // 0 = HMD, 1 = Left Controller, 2 = Right Controller
	bool bGotData = XRSystem->GetCurrentPose(DeviceId, GripRotation, GripPosition);

	if (!bGotData)
	{
		bTracked = false;
		return;
	}

	bTracked = true;

	// Generate approximated joint positions from controller grip position
	FVector ForwardDir = GripRotation.GetForwardVector();
	FVector RightDir = GripRotation.GetRightVector();

	// Wrist at grip position
	FHandJointData WristData;
	WristData.Location = GripPosition;
	WristData.Rotation = GripRotation;
	WristData.bIsTracked = true;
	JointMap.Add(EHandJoint::Wrist, WristData);

	// Palm slightly forward from grip
	FHandJointData PalmData;
	PalmData.Location = GripPosition + ForwardDir * 5.0f;
	PalmData.Rotation = GripRotation;
	PalmData.bIsTracked = true;
	JointMap.Add(EHandJoint::Palm, PalmData);

	// Index finger approximation for pointing
	FHandJointData IndexProximalData;
	IndexProximalData.Location = GripPosition + ForwardDir * 8.0f;
	IndexProximalData.Rotation = GripRotation;
	IndexProximalData.bIsTracked = true;
	JointMap.Add(EHandJoint::IndexProximal, IndexProximalData);

	FHandJointData IndexTipData;
	IndexTipData.Location = GripPosition + ForwardDir * 15.0f;
	IndexTipData.Rotation = GripRotation;
	IndexTipData.bIsTracked = true;
	JointMap.Add(EHandJoint::IndexTip, IndexTipData);

	// Thumb tip approximation
	FHandJointData ThumbTipData;
	ThumbTipData.Location = GripPosition + ForwardDir * 8.0f + RightDir * (bLeftHand ? -3.0f : 3.0f);
	ThumbTipData.Rotation = GripRotation;
	ThumbTipData.bIsTracked = true;
	JointMap.Add(EHandJoint::ThumbTip, ThumbTipData);

	// Controller mode doesn't support pinch/gesture detection
	float& PinchStrengthRef = bLeftHand ? LeftPinchStrength : RightPinchStrength;
	bool& bPinching = bLeftHand ? bLeftPinching : bRightPinching;

	PinchStrengthRef = 0.0f;
	bPinching = false;

	EHandGesture& CurrentGesture = bLeftHand ? LeftGesture : RightGesture;
	CurrentGesture = EHandGesture::None;
}

FHandJointData UJellyfinHandTrackingComponent::GetJointData(bool bLeftHand, EHandJoint Joint) const
{
	const TMap<EHandJoint, FHandJointData>& JointMap = bLeftHand ? LeftHandJoints : RightHandJoints;

	if (const FHandJointData* Data = JointMap.Find(Joint))
	{
		return *Data;
	}

	return FHandJointData();
}

bool UJellyfinHandTrackingComponent::GetPointerPose(bool bLeftHand, FVector& OutLocation, FQuat& OutRotation) const
{
	// Get index finger tip and proximal for pointing direction
	FHandJointData IndexTip = GetJointData(bLeftHand, EHandJoint::IndexTip);
	FHandJointData IndexProximal = GetJointData(bLeftHand, EHandJoint::IndexProximal);

	if (!IndexTip.bIsTracked || !IndexProximal.bIsTracked)
	{
		return false;
	}

	OutLocation = IndexTip.Location;

	// Calculate pointing direction
	FVector Direction = (IndexTip.Location - IndexProximal.Location).GetSafeNormal();
	if (Direction.IsNearlyZero())
	{
		OutRotation = IndexTip.Rotation;
	}
	else
	{
		OutRotation = FQuat::FindBetweenNormals(FVector::ForwardVector, Direction);
	}

	return true;
}

float UJellyfinHandTrackingComponent::GetPinchStrength(bool bLeftHand) const
{
	return bLeftHand ? LeftPinchStrength : RightPinchStrength;
}

bool UJellyfinHandTrackingComponent::IsPinching(bool bLeftHand) const
{
	return bLeftHand ? bLeftPinching : bRightPinching;
}

EHandGesture UJellyfinHandTrackingComponent::GetCurrentGesture(bool bLeftHand) const
{
	return bLeftHand ? LeftGesture : RightGesture;
}

float UJellyfinHandTrackingComponent::CalculatePinchStrength(bool bLeftHand)
{
	FHandJointData ThumbTip = GetJointData(bLeftHand, EHandJoint::ThumbTip);
	FHandJointData IndexTip = GetJointData(bLeftHand, EHandJoint::IndexTip);

	if (!ThumbTip.bIsTracked || !IndexTip.bIsTracked)
	{
		return 0.0f;
	}

	// Distance between thumb and index fingertips
	float Distance = FVector::Distance(ThumbTip.Location, IndexTip.Location);

	// Convert to 0-1 range (closer = higher strength)
	float MaxDistance = PinchDistanceThreshold * 2.0f;
	float Strength = FMath::Clamp(1.0f - (Distance / MaxDistance), 0.0f, 1.0f);

	return Strength;
}

EHandGesture UJellyfinHandTrackingComponent::DetectGesture(bool bLeftHand)
{
	// Check pinch first (most common interaction)
	if (IsPinching(bLeftHand))
	{
		return EHandGesture::Pinch;
	}

	// Check finger extension states
	bool bThumbExtended = IsFingerExtended(bLeftHand, 0);
	bool bIndexExtended = IsFingerExtended(bLeftHand, 1);
	bool bMiddleExtended = IsFingerExtended(bLeftHand, 2);
	bool bRingExtended = IsFingerExtended(bLeftHand, 3);
	bool bPinkyExtended = IsFingerExtended(bLeftHand, 4);

	// Open palm: all fingers extended
	if (bThumbExtended && bIndexExtended && bMiddleExtended && bRingExtended && bPinkyExtended)
	{
		return EHandGesture::OpenPalm;
	}

	// Fist: no fingers extended
	if (!bThumbExtended && !bIndexExtended && !bMiddleExtended && !bRingExtended && !bPinkyExtended)
	{
		return EHandGesture::Fist;
	}

	// Point: only index extended
	if (!bThumbExtended && bIndexExtended && !bMiddleExtended && !bRingExtended && !bPinkyExtended)
	{
		return EHandGesture::Point;
	}

	// Thumbs up: only thumb extended
	if (bThumbExtended && !bIndexExtended && !bMiddleExtended && !bRingExtended && !bPinkyExtended)
	{
		return EHandGesture::ThumbsUp;
	}

	// Peace: index and middle extended
	if (!bThumbExtended && bIndexExtended && bMiddleExtended && !bRingExtended && !bPinkyExtended)
	{
		return EHandGesture::Peace;
	}

	return EHandGesture::None;
}

float UJellyfinHandTrackingComponent::GetFingerCurl(bool bLeftHand, int32 FingerIndex) const
{
	// Map finger index to joints
	EHandJoint MetacarpalJoint, TipJoint;

	switch (FingerIndex)
	{
	case 0: // Thumb
		MetacarpalJoint = EHandJoint::ThumbMetacarpal;
		TipJoint = EHandJoint::ThumbTip;
		break;
	case 1: // Index
		MetacarpalJoint = EHandJoint::IndexMetacarpal;
		TipJoint = EHandJoint::IndexTip;
		break;
	case 2: // Middle
		MetacarpalJoint = EHandJoint::MiddleMetacarpal;
		TipJoint = EHandJoint::MiddleTip;
		break;
	case 3: // Ring
		MetacarpalJoint = EHandJoint::RingMetacarpal;
		TipJoint = EHandJoint::RingTip;
		break;
	case 4: // Pinky
		MetacarpalJoint = EHandJoint::PinkyMetacarpal;
		TipJoint = EHandJoint::PinkyTip;
		break;
	default:
		return 0.0f;
	}

	FHandJointData Metacarpal = GetJointData(bLeftHand, MetacarpalJoint);
	FHandJointData Tip = GetJointData(bLeftHand, TipJoint);
	FHandJointData Palm = GetJointData(bLeftHand, EHandJoint::Palm);

	if (!Metacarpal.bIsTracked || !Tip.bIsTracked || !Palm.bIsTracked)
	{
		return 0.5f; // Unknown state
	}

	// Calculate curl based on how close the tip is to the palm relative to metacarpal
	float ExtendedDistance = FVector::Distance(Metacarpal.Location, Tip.Location);
	float MaxExtension = FingerIndex == 0 ? 5.0f : 10.0f; // Thumb is shorter

	// Higher curl = closer to palm
	float Curl = 1.0f - FMath::Clamp(ExtendedDistance / MaxExtension, 0.0f, 1.0f);

	return Curl;
}

bool UJellyfinHandTrackingComponent::IsFingerExtended(bool bLeftHand, int32 FingerIndex) const
{
	float Curl = GetFingerCurl(bLeftHand, FingerIndex);
	// Consider extended if curl is less than 40%
	return Curl < 0.4f;
}
