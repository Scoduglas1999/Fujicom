// Copyright JellyVR Project. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "JellyfinSubtitleRenderer.generated.h"

class UTextRenderComponent;
class UWidgetComponent;
class UTextBlock;
class UMediaPlayer;

UENUM(BlueprintType)
enum class ESubtitlePosition : uint8
{
	/** Below the screen */
	BelowScreen,
	/** Bottom of screen (overlay) */
	ScreenBottom,
	/** Floating in front of player (follows view) */
	FloatingHUD
};

USTRUCT(BlueprintType)
struct FJellyfinSubtitleCue
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Subtitle")
	float StartTime = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Subtitle")
	float EndTime = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Subtitle")
	FString Text;

	bool IsActiveAt(float Time) const
	{
		return Time >= StartTime && Time < EndTime;
	}
};

USTRUCT(BlueprintType)
struct FSubtitleStyle
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	float FontSize = 24.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FLinearColor TextColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FLinearColor BackgroundColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.7f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FLinearColor OutlineColor = FLinearColor::Black;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	float OutlineSize = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	bool bShowBackground = true;
};

/**
 * Subtitle renderer for VR video playback
 * Displays subtitles in 3D space, either attached to screen or floating
 */
UCLASS(ClassGroup=(JellyfinVR), meta=(BlueprintSpawnableComponent))
class JELLYFINVR_API UJellyfinSubtitleRenderer : public UActorComponent
{
	GENERATED_BODY()

public:
	UJellyfinSubtitleRenderer();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * Initialize with media player for timing sync
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Subtitles")
	void Initialize(UMediaPlayer* InMediaPlayer);

	/**
	 * Load subtitles from SRT format string
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Subtitles")
	bool LoadFromSRT(const FString& SRTContent);

	/**
	 * Load subtitles from URL (async)
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Subtitles")
	void LoadFromURL(const FString& URL);

	/**
	 * Clear all loaded subtitles
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Subtitles")
	void ClearSubtitles();

	/**
	 * Enable/disable subtitle display
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Subtitles")
	void SetEnabled(bool bEnabled);

	/**
	 * Check if subtitles are enabled
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Subtitles")
	bool IsEnabled() const { return bSubtitlesEnabled; }

	/**
	 * Set subtitle position mode
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Subtitles")
	void SetPosition(ESubtitlePosition Position);

	/**
	 * Get current position mode
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Subtitles")
	ESubtitlePosition GetPosition() const { return SubtitlePosition; }

	/**
	 * Set subtitle style
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Subtitles")
	void SetStyle(const FSubtitleStyle& Style);

	/**
	 * Get current style
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Subtitles")
	const FSubtitleStyle& GetStyle() const { return CurrentStyle; }

	/**
	 * Set screen transform for positioning subtitles relative to screen
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Subtitles")
	void SetScreenTransform(const FTransform& ScreenTransform, float ScreenWidth, float ScreenHeight);

	/**
	 * Manually set current subtitle text (for embedded subtitles)
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Subtitles")
	void SetCurrentText(const FString& Text);

	/**
	 * Get currently displayed text
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Subtitles")
	FString GetCurrentText() const { return CurrentSubtitleText; }

	/**
	 * Get number of loaded cues
	 */
	UFUNCTION(BlueprintPure, Category = "JellyfinVR|Subtitles")
	int32 GetCueCount() const { return SubtitleCues.Num(); }

	/**
	 * Set time offset for subtitle sync (positive = delay subtitles)
	 */
	UFUNCTION(BlueprintCallable, Category = "JellyfinVR|Subtitles")
	void SetTimeOffset(float OffsetSeconds);

protected:
	void UpdateSubtitleDisplay();
	void UpdateSubtitlePosition();
	FJellyfinSubtitleCue* FindActiveCue(float Time);
	bool ParseSRTTimestamp(const FString& Timestamp, float& OutSeconds);
	void ApplyStyle();

	// Configuration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Subtitles")
	bool bSubtitlesEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Subtitles")
	ESubtitlePosition SubtitlePosition = ESubtitlePosition::BelowScreen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Subtitles")
	FSubtitleStyle CurrentStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Subtitles")
	float TimeOffsetSeconds = 0.0f;

	/** Distance below screen for BelowScreen mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Subtitles")
	float BelowScreenOffset = 30.0f;

	/** Distance from player for FloatingHUD mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JellyfinVR|Subtitles")
	float FloatingDistance = 150.0f;

private:
	UPROPERTY()
	UMediaPlayer* MediaPlayer;

	UPROPERTY()
	UTextRenderComponent* TextRenderer;

	TArray<FJellyfinSubtitleCue> SubtitleCues;
	FString CurrentSubtitleText;
	int32 LastActiveCueIndex = -1;

	FTransform ScreenWorldTransform;
	float ScreenWidth = 400.0f;
	float ScreenHeight = 225.0f;
};
