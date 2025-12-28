// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "JellyfinVRPointer.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UNiagaraComponent;

/**
 * VR Pointer visualization component
 * Renders a laser beam and hit indicator for VR interaction
 */
UCLASS(ClassGroup=(JellyfinVR), meta=(BlueprintSpawnableComponent))
class JELLYFINVR_API UJellyfinVRPointerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UJellyfinVRPointerComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * Update pointer position and direction
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Pointer")
	void UpdatePointer(const FVector& Origin, const FVector& Direction, float MaxDistance);

	/**
	 * Set whether this pointer is the active/primary one
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Pointer")
	void SetPointerActive(bool bActive);

	/**
	 * Set whether the pointer is currently hovering over interactive element
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Pointer")
	void SetHovering(bool bHovering);

	/**
	 * Set whether the pointer is pressing/clicking
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Pointer")
	void SetPressing(bool bPressing);

	/**
	 * Show/hide the pointer
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Pointer")
	void SetVisible(bool bVisible);

	/**
	 * Get current hit location
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Pointer")
	FVector GetHitLocation() const { return CurrentHitLocation; }

	/**
	 * Check if pointer is hitting something
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Pointer")
	bool IsHitting() const { return bIsHitting; }

	// ============ Appearance ============

	/** Color of the pointer beam when idle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Pointer|Appearance")
	FLinearColor IdleColor = FLinearColor(0.5f, 0.5f, 1.0f, 0.8f);

	/** Color of the pointer beam when hovering over interactive element */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Pointer|Appearance")
	FLinearColor HoverColor = FLinearColor(0.2f, 1.0f, 0.2f, 1.0f);

	/** Color of the pointer beam when pressing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Pointer|Appearance")
	FLinearColor PressColor = FLinearColor(1.0f, 1.0f, 0.2f, 1.0f);

	/** Width of the beam at origin */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Pointer|Appearance", meta = (ClampMin = "0.1", ClampMax = "5.0"))
	float BeamStartWidth = 0.5f;

	/** Width of the beam at end */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Pointer|Appearance", meta = (ClampMin = "0.1", ClampMax = "5.0"))
	float BeamEndWidth = 0.2f;

	/** Size of the hit indicator dot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Pointer|Appearance", meta = (ClampMin = "0.5", ClampMax = "10.0"))
	float HitIndicatorSize = 2.0f;

protected:
	void CreateBeamMesh();
	void CreateHitIndicator();
	void UpdateBeamVisuals();
	void UpdateHitIndicator();
	FLinearColor GetCurrentColor() const;

private:
	UPROPERTY()
	UStaticMeshComponent* BeamMesh;

	UPROPERTY()
	UStaticMeshComponent* HitIndicatorMesh;

	UPROPERTY()
	UMaterialInstanceDynamic* BeamMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* HitIndicatorMaterial;

	// State
	FVector CurrentOrigin = FVector::ZeroVector;
	FVector CurrentDirection = FVector::ForwardVector;
	FVector CurrentHitLocation = FVector::ZeroVector;
	float CurrentMaxDistance = 1000.0f;
	bool bIsActive = true;
	bool bIsHovering = false;
	bool bIsPressing = false;
	bool bIsVisible = true;
	bool bIsHitting = false;
};
