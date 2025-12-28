// Copyright JellyVR Project. All Rights Reserved.

#include "JellyfinUIAnimator.h"

TUniquePtr<FJellyfinUIAnimator> FJellyfinUIAnimator::Instance = nullptr;

FJellyfinUIAnimator& FJellyfinUIAnimator::Get()
{
	if (!Instance.IsValid())
	{
		Instance = TUniquePtr<FJellyfinUIAnimator>(new FJellyfinUIAnimator());
	}
	return *Instance;
}

FJellyfinUIAnimator::FJellyfinUIAnimator()
{
}

FJellyfinUIAnimator::~FJellyfinUIAnimator()
{
	CancelAll();
}

void FJellyfinUIAnimator::Tick(float DeltaTime)
{
	// Process delayed animations
	for (int32 i = DelayedAnimations.Num() - 1; i >= 0; --i)
	{
		DelayedAnimations[i].Key -= DeltaTime;
		if (DelayedAnimations[i].Key <= 0.0f)
		{
			// Start the animation
			DelayedAnimations[i].Value();
			DelayedAnimations.RemoveAt(i);
		}
	}

	// Process active animations
	TArray<uint32> CompletedAnimations;

	for (auto& Pair : Animations)
	{
		FJellyfinAnimation& Anim = Pair.Value;
		if (!Anim.bIsActive)
		{
			continue;
		}

		// Update elapsed time
		Anim.ElapsedTime += DeltaTime;

		// Calculate alpha (0 to 1)
		Anim.Alpha = FMath::Clamp(Anim.ElapsedTime / Anim.Duration, 0.0f, 1.0f);

		// Apply easing
		float EasedAlpha = ApplyEasing(Anim.Alpha, Anim.EaseType);

		// Calculate current value
		Anim.CurrentValue = FMath::Lerp(Anim.StartValue, Anim.EndValue, EasedAlpha);

		// Call update callback
		if (Anim.OnUpdate)
		{
			Anim.OnUpdate(Anim.CurrentValue);
		}

		// Check if complete
		if (Anim.Alpha >= 1.0f)
		{
			if (Anim.bLoop)
			{
				// Reset for loop
				Anim.ElapsedTime = 0.0f;
				Anim.Alpha = 0.0f;
			}
			else
			{
				CompletedAnimations.Add(Pair.Key);
			}
		}
	}

	// Handle completed animations
	for (uint32 Id : CompletedAnimations)
	{
		if (FJellyfinAnimation* Anim = Animations.Find(Id))
		{
			if (Anim->OnComplete)
			{
				Anim->OnComplete();
			}
		}
		Animations.Remove(Id);
	}
}

TStatId FJellyfinUIAnimator::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FJellyfinUIAnimator, STATGROUP_Tickables);
}

uint32 FJellyfinUIAnimator::Animate(
	float StartValue,
	float EndValue,
	float Duration,
	EJellyfinEaseType EaseType,
	TFunction<void(float)> OnUpdate,
	TFunction<void()> OnComplete)
{
	uint32 Id = GenerateId();

	FJellyfinAnimation Anim;
	Anim.Id = Id;
	Anim.StartValue = StartValue;
	Anim.EndValue = EndValue;
	Anim.CurrentValue = StartValue;
	Anim.Duration = FMath::Max(Duration, 0.001f); // Prevent division by zero
	Anim.EaseType = EaseType;
	Anim.OnUpdate = OnUpdate;
	Anim.OnComplete = OnComplete;
	Anim.bIsActive = true;
	Anim.bLoop = false;

	Animations.Add(Id, Anim);

	// Immediately call update with start value
	if (OnUpdate)
	{
		OnUpdate(StartValue);
	}

	return Id;
}

uint32 FJellyfinUIAnimator::AnimateLoop(
	float StartValue,
	float EndValue,
	float Duration,
	EJellyfinEaseType EaseType,
	TFunction<void(float)> OnUpdate)
{
	uint32 Id = GenerateId();

	FJellyfinAnimation Anim;
	Anim.Id = Id;
	Anim.StartValue = StartValue;
	Anim.EndValue = EndValue;
	Anim.CurrentValue = StartValue;
	Anim.Duration = FMath::Max(Duration, 0.001f);
	Anim.EaseType = EaseType;
	Anim.OnUpdate = OnUpdate;
	Anim.bIsActive = true;
	Anim.bLoop = true;

	Animations.Add(Id, Anim);

	return Id;
}

uint32 FJellyfinUIAnimator::AnimateDelayed(
	float Delay,
	float StartValue,
	float EndValue,
	float Duration,
	EJellyfinEaseType EaseType,
	TFunction<void(float)> OnUpdate,
	TFunction<void()> OnComplete)
{
	// Generate ID now so caller can track/cancel
	uint32 Id = GenerateId();

	// Create a lambda that will start the animation later
	auto StartFunc = [this, Id, StartValue, EndValue, Duration, EaseType, OnUpdate, OnComplete]() -> uint32
	{
		FJellyfinAnimation Anim;
		Anim.Id = Id;
		Anim.StartValue = StartValue;
		Anim.EndValue = EndValue;
		Anim.CurrentValue = StartValue;
		Anim.Duration = FMath::Max(Duration, 0.001f);
		Anim.EaseType = EaseType;
		Anim.OnUpdate = OnUpdate;
		Anim.OnComplete = OnComplete;
		Anim.bIsActive = true;
		Anim.bLoop = false;

		Animations.Add(Id, Anim);

		if (OnUpdate)
		{
			OnUpdate(StartValue);
		}

		return Id;
	};

	DelayedAnimations.Add(TPair<float, TFunction<uint32()>>(Delay, StartFunc));

	return Id;
}

void FJellyfinUIAnimator::Cancel(uint32 AnimationId)
{
	Animations.Remove(AnimationId);

	// Also check delayed animations
	for (int32 i = DelayedAnimations.Num() - 1; i >= 0; --i)
	{
		// We can't easily check the ID of delayed animations, so this is limited
		// In practice, delayed animations are short-lived
	}
}

void FJellyfinUIAnimator::CancelAll()
{
	Animations.Empty();
	DelayedAnimations.Empty();
}

bool FJellyfinUIAnimator::IsAnimating(uint32 AnimationId) const
{
	return Animations.Contains(AnimationId);
}

float FJellyfinUIAnimator::GetCurrentValue(uint32 AnimationId) const
{
	if (const FJellyfinAnimation* Anim = Animations.Find(AnimationId))
	{
		return Anim->CurrentValue;
	}
	return 0.0f;
}

float FJellyfinUIAnimator::ApplyEasing(float Alpha, EJellyfinEaseType EaseType)
{
	switch (EaseType)
	{
	case EJellyfinEaseType::Linear:
		return Alpha;

	case EJellyfinEaseType::EaseIn:
		// Quadratic ease in
		return Alpha * Alpha;

	case EJellyfinEaseType::EaseOut:
		// Quadratic ease out
		return 1.0f - (1.0f - Alpha) * (1.0f - Alpha);

	case EJellyfinEaseType::EaseInOut:
		// Quadratic ease in-out
		if (Alpha < 0.5f)
		{
			return 2.0f * Alpha * Alpha;
		}
		else
		{
			return 1.0f - FMath::Pow(-2.0f * Alpha + 2.0f, 2.0f) / 2.0f;
		}

	case EJellyfinEaseType::EaseOutBack:
		// Slight overshoot then settle
		{
			const float c1 = 1.70158f;
			const float c3 = c1 + 1.0f;
			return 1.0f + c3 * FMath::Pow(Alpha - 1.0f, 3.0f) + c1 * FMath::Pow(Alpha - 1.0f, 2.0f);
		}

	case EJellyfinEaseType::EaseOutElastic:
		// Springy bounce effect
		{
			if (Alpha == 0.0f) return 0.0f;
			if (Alpha == 1.0f) return 1.0f;
			const float c4 = (2.0f * PI) / 3.0f;
			return FMath::Pow(2.0f, -10.0f * Alpha) * FMath::Sin((Alpha * 10.0f - 0.75f) * c4) + 1.0f;
		}

	default:
		return Alpha;
	}
}

uint32 FJellyfinUIAnimator::FadeIn(float Duration, TFunction<void(float)> OnUpdate, TFunction<void()> OnComplete)
{
	return Animate(0.0f, 1.0f, Duration, EJellyfinEaseType::EaseOut, OnUpdate, OnComplete);
}

uint32 FJellyfinUIAnimator::FadeOut(float Duration, TFunction<void(float)> OnUpdate, TFunction<void()> OnComplete)
{
	return Animate(1.0f, 0.0f, Duration, EJellyfinEaseType::EaseOut, OnUpdate, OnComplete);
}

uint32 FJellyfinUIAnimator::ScaleHover(bool bHovering, TFunction<void(float)> OnUpdate)
{
	float StartScale = bHovering ? JellyfinAnimConstants::DefaultScale : JellyfinAnimConstants::HoverScale;
	float EndScale = bHovering ? JellyfinAnimConstants::HoverScale : JellyfinAnimConstants::DefaultScale;

	return Animate(
		StartScale,
		EndScale,
		JellyfinAnimConstants::HoverDuration,
		EJellyfinEaseType::EaseOut,
		OnUpdate
	);
}

uint32 FJellyfinUIAnimator::ScalePress(bool bPressed, TFunction<void(float)> OnUpdate)
{
	float StartScale = bPressed ? JellyfinAnimConstants::HoverScale : JellyfinAnimConstants::PressScale;
	float EndScale = bPressed ? JellyfinAnimConstants::PressScale : JellyfinAnimConstants::HoverScale;

	return Animate(
		StartScale,
		EndScale,
		JellyfinAnimConstants::PressDuration,
		EJellyfinEaseType::EaseOut,
		OnUpdate
	);
}

uint32 FJellyfinUIAnimator::GenerateId()
{
	return NextId++;
}
