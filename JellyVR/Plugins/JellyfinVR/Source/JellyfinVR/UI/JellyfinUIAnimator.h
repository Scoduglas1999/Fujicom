// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"

/**
 * Easing function types for animations
 */
enum class EJellyfinEaseType : uint8
{
	Linear,
	EaseIn,
	EaseOut,
	EaseInOut,
	EaseOutBack,    // Slight overshoot for bouncy feel
	EaseOutElastic  // Springy effect
};

/**
 * Single animation instance tracking
 */
struct FJellyfinAnimation
{
	/** Unique identifier for this animation */
	uint32 Id = 0;

	/** Current interpolation value (0.0 to 1.0) */
	float Alpha = 0.0f;

	/** Current output value */
	float CurrentValue = 0.0f;

	/** Start value */
	float StartValue = 0.0f;

	/** Target value */
	float EndValue = 0.0f;

	/** Duration in seconds */
	float Duration = 0.15f;

	/** Elapsed time */
	float ElapsedTime = 0.0f;

	/** Easing type */
	EJellyfinEaseType EaseType = EJellyfinEaseType::EaseOut;

	/** Is this animation currently active */
	bool bIsActive = false;

	/** Should this animation loop */
	bool bLoop = false;

	/** Callback when animation completes */
	TFunction<void()> OnComplete;

	/** Callback every tick with current value */
	TFunction<void(float)> OnUpdate;
};

/**
 * Animation group for coordinating multiple animations
 */
struct FJellyfinAnimationGroup
{
	TArray<uint32> AnimationIds;
	TFunction<void()> OnAllComplete;
	int32 CompletedCount = 0;
};

/**
 * Central animation system for JellyfinVR UI
 * Manages all UI animations with delta-time based interpolation
 */
class JELLYFINVR_API FJellyfinUIAnimator : public FTickableGameObject
{
public:
	static FJellyfinUIAnimator& Get();

	virtual ~FJellyfinUIAnimator();

	// FTickableGameObject interface
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override { return true; }
	virtual bool IsTickableInEditor() const override { return false; }
	virtual bool IsTickableWhenPaused() const override { return true; }

	// ============ Animation Creation ============

	/**
	 * Create a new animation
	 * @param StartValue Initial value
	 * @param EndValue Target value
	 * @param Duration Time in seconds
	 * @param EaseType Easing function
	 * @param OnUpdate Called each tick with current value
	 * @param OnComplete Called when animation finishes
	 * @return Animation ID for tracking/cancellation
	 */
	uint32 Animate(
		float StartValue,
		float EndValue,
		float Duration,
		EJellyfinEaseType EaseType = EJellyfinEaseType::EaseOut,
		TFunction<void(float)> OnUpdate = nullptr,
		TFunction<void()> OnComplete = nullptr
	);

	/**
	 * Create a looping animation (for shimmer effects, spinners, etc.)
	 */
	uint32 AnimateLoop(
		float StartValue,
		float EndValue,
		float Duration,
		EJellyfinEaseType EaseType = EJellyfinEaseType::Linear,
		TFunction<void(float)> OnUpdate = nullptr
	);

	/**
	 * Animate with delay before starting
	 */
	uint32 AnimateDelayed(
		float Delay,
		float StartValue,
		float EndValue,
		float Duration,
		EJellyfinEaseType EaseType = EJellyfinEaseType::EaseOut,
		TFunction<void(float)> OnUpdate = nullptr,
		TFunction<void()> OnComplete = nullptr
	);

	// ============ Animation Control ============

	/**
	 * Cancel an animation by ID
	 */
	void Cancel(uint32 AnimationId);

	/**
	 * Cancel all animations
	 */
	void CancelAll();

	/**
	 * Check if an animation is still running
	 */
	bool IsAnimating(uint32 AnimationId) const;

	/**
	 * Get current value of an animation
	 */
	float GetCurrentValue(uint32 AnimationId) const;

	// ============ Easing Functions ============

	/**
	 * Apply easing function to alpha value
	 */
	static float ApplyEasing(float Alpha, EJellyfinEaseType EaseType);

	// ============ Convenience Methods ============

	/**
	 * Fade opacity from 0 to 1
	 */
	uint32 FadeIn(float Duration, TFunction<void(float)> OnUpdate, TFunction<void()> OnComplete = nullptr);

	/**
	 * Fade opacity from 1 to 0
	 */
	uint32 FadeOut(float Duration, TFunction<void(float)> OnUpdate, TFunction<void()> OnComplete = nullptr);

	/**
	 * Scale animation for hover effect
	 */
	uint32 ScaleHover(bool bHovering, TFunction<void(float)> OnUpdate);

	/**
	 * Scale animation for press effect
	 */
	uint32 ScalePress(bool bPressed, TFunction<void(float)> OnUpdate);

private:
	FJellyfinUIAnimator();

	/** Generate unique animation ID */
	uint32 GenerateId();

	/** All active animations */
	TMap<uint32, FJellyfinAnimation> Animations;

	/** Delayed animations waiting to start */
	TArray<TPair<float, TFunction<uint32()>>> DelayedAnimations;

	/** Next animation ID */
	uint32 NextId = 1;

	/** Singleton instance */
	static TUniquePtr<FJellyfinUIAnimator> Instance;
};

/**
 * Animation constants for consistent UI feel
 */
namespace JellyfinAnimConstants
{
	// Durations (seconds)
	constexpr float HoverDuration = 0.15f;
	constexpr float PressDuration = 0.1f;
	constexpr float FocusGlowDuration = 0.2f;
	constexpr float ScreenTransitionDuration = 0.3f;
	constexpr float ImageFadeInDuration = 0.2f;
	constexpr float SkeletonShimmerDuration = 1.5f;
	constexpr float SpinnerRotationDuration = 1.0f;

	// Scale values
	constexpr float DefaultScale = 1.0f;
	constexpr float HoverScale = 1.08f;
	constexpr float PressScale = 1.04f;
	constexpr float FocusScale = 1.05f;
	constexpr float ImagePopScale = 1.02f;

	// Opacity values
	constexpr float FullOpacity = 1.0f;
	constexpr float PressOpacity = 0.9f;
	constexpr float DisabledOpacity = 0.3f;

	// Delays (seconds)
	constexpr float StaggerDelay = 0.1f;
	constexpr float ScreenFadeOutDuration = 0.2f;
}
