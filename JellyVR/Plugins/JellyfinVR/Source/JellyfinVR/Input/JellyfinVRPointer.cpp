// Copyright JellyVR Project. All Rights Reserved.

#include "JellyfinVRPointer.h"
#include "JellyfinVRModule.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMesh.h"
#include "Kismet/KismetMathLibrary.h"

UJellyfinVRPointerComponent::UJellyfinVRPointerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.0f;
}

void UJellyfinVRPointerComponent::BeginPlay()
{
	Super::BeginPlay();

	CreateBeamMesh();
	CreateHitIndicator();

	UE_LOG(LogJellyfinVR, Log, TEXT("JellyfinVRPointerComponent initialized"));
}

void UJellyfinVRPointerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (BeamMesh)
	{
		BeamMesh->DestroyComponent();
		BeamMesh = nullptr;
	}

	if (HitIndicatorMesh)
	{
		HitIndicatorMesh->DestroyComponent();
		HitIndicatorMesh = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void UJellyfinVRPointerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateBeamVisuals();
	UpdateHitIndicator();
}

void UJellyfinVRPointerComponent::CreateBeamMesh()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	// Create beam mesh component
	BeamMesh = NewObject<UStaticMeshComponent>(Owner, TEXT("PointerBeam"));
	BeamMesh->SetupAttachment(Owner->GetRootComponent());
	BeamMesh->RegisterComponent();

	// Use a cylinder mesh for the beam - LoadObject instead of ConstructorHelpers (works at runtime)
	UStaticMesh* CylinderMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh)
	{
		BeamMesh->SetStaticMesh(CylinderMesh);
	}

	// Create dynamic material for color changes - use WorldGridMaterial which has proper parameters
	UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/EngineMaterials/WorldGridMaterial.WorldGridMaterial"));
	if (BaseMaterial)
	{
		BeamMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		BeamMesh->SetMaterial(0, BeamMaterial);
	}

	// Configure beam
	BeamMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BeamMesh->SetCastShadow(false);
	BeamMesh->SetVisibility(bIsVisible && bIsActive);

	// Initial scale (will be updated)
	BeamMesh->SetWorldScale3D(FVector(BeamStartWidth * 0.01f, BeamStartWidth * 0.01f, 1.0f));
}

void UJellyfinVRPointerComponent::CreateHitIndicator()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	// Create hit indicator mesh
	HitIndicatorMesh = NewObject<UStaticMeshComponent>(Owner, TEXT("PointerHitIndicator"));
	HitIndicatorMesh->SetupAttachment(Owner->GetRootComponent());
	HitIndicatorMesh->RegisterComponent();

	// Use a sphere mesh for the hit indicator - LoadObject instead of ConstructorHelpers (works at runtime)
	UStaticMesh* SphereMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh)
	{
		HitIndicatorMesh->SetStaticMesh(SphereMesh);
	}

	// Create dynamic material
	UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/EngineMaterials/DefaultMaterial"));
	if (BaseMaterial)
	{
		HitIndicatorMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		HitIndicatorMaterial->SetVectorParameterValue(TEXT("BaseColor"), IdleColor);
		HitIndicatorMesh->SetMaterial(0, HitIndicatorMaterial);
	}

	// Configure hit indicator
	HitIndicatorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitIndicatorMesh->SetCastShadow(false);
	HitIndicatorMesh->SetVisibility(false); // Hidden until we hit something
	HitIndicatorMesh->SetWorldScale3D(FVector(HitIndicatorSize * 0.01f));
}

void UJellyfinVRPointerComponent::UpdatePointer(const FVector& Origin, const FVector& Direction, float MaxDistance)
{
	CurrentOrigin = Origin;
	CurrentDirection = Direction.GetSafeNormal();
	CurrentMaxDistance = MaxDistance;

	// Perform raycast
	FHitResult HitResult;
	FVector EndPoint = Origin + CurrentDirection * MaxDistance;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	bIsHitting = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Origin,
		EndPoint,
		ECC_Visibility,
		QueryParams
	);

	if (bIsHitting)
	{
		CurrentHitLocation = HitResult.Location;
	}
	else
	{
		CurrentHitLocation = EndPoint;
	}
}

void UJellyfinVRPointerComponent::SetPointerActive(bool bActive)
{
	bIsActive = bActive;

	if (BeamMesh)
	{
		BeamMesh->SetVisibility(bIsVisible && bIsActive);
	}
	if (HitIndicatorMesh)
	{
		HitIndicatorMesh->SetVisibility(bIsVisible && bIsActive && bIsHitting);
	}
}

void UJellyfinVRPointerComponent::SetHovering(bool bHovering)
{
	bIsHovering = bHovering;
}

void UJellyfinVRPointerComponent::SetPressing(bool bPressing)
{
	bIsPressing = bPressing;
}

void UJellyfinVRPointerComponent::SetVisible(bool bVisible)
{
	bIsVisible = bVisible;

	if (BeamMesh)
	{
		BeamMesh->SetVisibility(bIsVisible && bIsActive);
	}
	if (HitIndicatorMesh)
	{
		HitIndicatorMesh->SetVisibility(bIsVisible && bIsActive && bIsHitting);
	}
}

FLinearColor UJellyfinVRPointerComponent::GetCurrentColor() const
{
	if (bIsPressing)
	{
		return PressColor;
	}
	else if (bIsHovering)
	{
		return HoverColor;
	}
	return IdleColor;
}

void UJellyfinVRPointerComponent::UpdateBeamVisuals()
{
	if (!BeamMesh || !bIsActive || !bIsVisible)
	{
		return;
	}

	// Calculate beam length
	float BeamLength = FVector::Distance(CurrentOrigin, CurrentHitLocation);

	// Position beam at midpoint between origin and hit
	FVector MidPoint = (CurrentOrigin + CurrentHitLocation) * 0.5f;
	BeamMesh->SetWorldLocation(MidPoint);

	// Rotate beam to point in direction
	FRotator BeamRotation = UKismetMathLibrary::MakeRotFromZ(CurrentDirection);
	BeamMesh->SetWorldRotation(BeamRotation);

	// Scale beam (cylinder is 100 units tall by default)
	float ScaleZ = BeamLength / 100.0f;
	float ScaleXY = BeamStartWidth * 0.01f;
	BeamMesh->SetWorldScale3D(FVector(ScaleXY, ScaleXY, ScaleZ));

	// Update color
	if (BeamMaterial)
	{
		FLinearColor Color = GetCurrentColor();
		BeamMaterial->SetVectorParameterValue(TEXT("BaseColor"), Color);
	}
}

void UJellyfinVRPointerComponent::UpdateHitIndicator()
{
	if (!HitIndicatorMesh)
	{
		return;
	}

	bool bShouldShow = bIsActive && bIsVisible && bIsHitting;
	HitIndicatorMesh->SetVisibility(bShouldShow);

	if (bShouldShow)
	{
		// Position at hit location
		HitIndicatorMesh->SetWorldLocation(CurrentHitLocation);

		// Scale based on hovering state
		float Scale = bIsHovering ? HitIndicatorSize * 1.5f : HitIndicatorSize;
		HitIndicatorMesh->SetWorldScale3D(FVector(Scale * 0.01f));

		// Update color
		if (HitIndicatorMaterial)
		{
			FLinearColor Color = GetCurrentColor();
			HitIndicatorMaterial->SetVectorParameterValue(TEXT("BaseColor"), Color);
		}
	}
}
