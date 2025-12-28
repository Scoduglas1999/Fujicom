// Copyright JellyVR Project. All Rights Reserved.

#include "JellyfinSubtitleRenderer.h"
#include "JellyfinVRModule.h"
#include "MediaPlayer.h"
#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/Font.h"
#include "Kismet/GameplayStatics.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

UJellyfinSubtitleRenderer::UJellyfinSubtitleRenderer()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.033f; // ~30Hz for smooth subtitle updates
}

void UJellyfinSubtitleRenderer::BeginPlay()
{
	Super::BeginPlay();

	// Create text render component for 3D subtitle display
	if (AActor* Owner = GetOwner())
	{
		TextRenderer = NewObject<UTextRenderComponent>(Owner, TEXT("SubtitleText"));
		if (TextRenderer)
		{
			TextRenderer->SetupAttachment(Owner->GetRootComponent());
			TextRenderer->RegisterComponent();

			// Configure for VR readability
			TextRenderer->SetHorizontalAlignment(EHTA_Center);
			TextRenderer->SetVerticalAlignment(EVRTA_TextCenter);
			TextRenderer->SetWorldSize(CurrentStyle.FontSize);
			TextRenderer->SetTextRenderColor(CurrentStyle.TextColor.ToFColor(true));

			// Initially hidden
			TextRenderer->SetVisibility(false);

			ApplyStyle();

			UE_LOG(LogJellyfinVR, Log, TEXT("JellyfinSubtitleRenderer initialized"));
		}
	}
}

void UJellyfinSubtitleRenderer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TextRenderer)
	{
		TextRenderer->DestroyComponent();
		TextRenderer = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void UJellyfinSubtitleRenderer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bSubtitlesEnabled && MediaPlayer && SubtitleCues.Num() > 0)
	{
		UpdateSubtitleDisplay();
	}

	// Update position for floating HUD mode
	if (SubtitlePosition == ESubtitlePosition::FloatingHUD && TextRenderer && TextRenderer->IsVisible())
	{
		UpdateSubtitlePosition();
	}
}

void UJellyfinSubtitleRenderer::Initialize(UMediaPlayer* InMediaPlayer)
{
	MediaPlayer = InMediaPlayer;
	UE_LOG(LogJellyfinVR, Log, TEXT("SubtitleRenderer connected to MediaPlayer"));
}

bool UJellyfinSubtitleRenderer::LoadFromSRT(const FString& SRTContent)
{
	SubtitleCues.Empty();
	LastActiveCueIndex = -1;

	if (SRTContent.IsEmpty())
	{
		return false;
	}

	// Parse SRT format:
	// 1
	// 00:00:01,000 --> 00:00:04,000
	// Subtitle text line 1
	// Subtitle text line 2
	//
	// 2
	// ...

	TArray<FString> Lines;
	SRTContent.ParseIntoArrayLines(Lines);

	int32 LineIndex = 0;
	while (LineIndex < Lines.Num())
	{
		// Skip empty lines
		while (LineIndex < Lines.Num() && Lines[LineIndex].TrimStartAndEnd().IsEmpty())
		{
			LineIndex++;
		}

		if (LineIndex >= Lines.Num())
		{
			break;
		}

		// Skip cue number
		FString CueNumberLine = Lines[LineIndex].TrimStartAndEnd();
		if (!CueNumberLine.IsNumeric())
		{
			LineIndex++;
			continue;
		}
		LineIndex++;

		if (LineIndex >= Lines.Num())
		{
			break;
		}

		// Parse timestamp line
		FString TimestampLine = Lines[LineIndex].TrimStartAndEnd();
		LineIndex++;

		// Expected format: 00:00:01,000 --> 00:00:04,000
		int32 ArrowIndex;
		if (!TimestampLine.FindChar('-', ArrowIndex))
		{
			continue;
		}

		FString StartStr = TimestampLine.Left(ArrowIndex).TrimStartAndEnd();
		FString EndStr = TimestampLine.Mid(ArrowIndex + 3).TrimStartAndEnd(); // Skip " --> "

		// Remove trailing ">" if present
		EndStr = EndStr.TrimStart();
		if (EndStr.StartsWith(TEXT(">")))
		{
			EndStr = EndStr.Mid(1).TrimStart();
		}

		FJellyfinSubtitleCue Cue;
		if (!ParseSRTTimestamp(StartStr, Cue.StartTime) || !ParseSRTTimestamp(EndStr, Cue.EndTime))
		{
			continue;
		}

		// Collect text lines until empty line
		FString CueText;
		while (LineIndex < Lines.Num() && !Lines[LineIndex].TrimStartAndEnd().IsEmpty())
		{
			if (!CueText.IsEmpty())
			{
				CueText += TEXT("\n");
			}
			CueText += Lines[LineIndex].TrimStartAndEnd();
			LineIndex++;
		}

		// Remove HTML-like tags (e.g., <i>, </i>, <b>, etc.)
		CueText = CueText.Replace(TEXT("<i>"), TEXT(""));
		CueText = CueText.Replace(TEXT("</i>"), TEXT(""));
		CueText = CueText.Replace(TEXT("<b>"), TEXT(""));
		CueText = CueText.Replace(TEXT("</b>"), TEXT(""));
		CueText = CueText.Replace(TEXT("<u>"), TEXT(""));
		CueText = CueText.Replace(TEXT("</u>"), TEXT(""));

		Cue.Text = CueText;
		SubtitleCues.Add(Cue);
	}

	UE_LOG(LogJellyfinVR, Log, TEXT("Loaded %d subtitle cues from SRT"), SubtitleCues.Num());
	return SubtitleCues.Num() > 0;
}

bool UJellyfinSubtitleRenderer::ParseSRTTimestamp(const FString& Timestamp, float& OutSeconds)
{
	// Format: HH:MM:SS,mmm or HH:MM:SS.mmm
	TArray<FString> Parts;
	FString CleanTimestamp = Timestamp.Replace(TEXT(","), TEXT("."));
	CleanTimestamp.ParseIntoArray(Parts, TEXT(":"));

	if (Parts.Num() < 3)
	{
		return false;
	}

	float Hours = FCString::Atof(*Parts[0]);
	float Minutes = FCString::Atof(*Parts[1]);
	float Seconds = FCString::Atof(*Parts[2]);

	OutSeconds = Hours * 3600.0f + Minutes * 60.0f + Seconds;
	return true;
}

void UJellyfinSubtitleRenderer::LoadFromURL(const FString& URL)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(URL);
	Request->SetVerb(TEXT("GET"));

	Request->OnProcessRequestComplete().BindLambda([this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		if (bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == 200)
		{
			FString Content = Response->GetContentAsString();
			LoadFromSRT(Content);
		}
		else
		{
			UE_LOG(LogJellyfinVR, Warning, TEXT("Failed to load subtitles from URL"));
		}
	});

	Request->ProcessRequest();
}

void UJellyfinSubtitleRenderer::ClearSubtitles()
{
	SubtitleCues.Empty();
	LastActiveCueIndex = -1;
	CurrentSubtitleText.Empty();

	if (TextRenderer)
	{
		TextRenderer->SetText(FText::GetEmpty());
		TextRenderer->SetVisibility(false);
	}
}

void UJellyfinSubtitleRenderer::SetEnabled(bool bEnabled)
{
	bSubtitlesEnabled = bEnabled;

	if (!bEnabled && TextRenderer)
	{
		TextRenderer->SetVisibility(false);
	}
}

void UJellyfinSubtitleRenderer::SetPosition(ESubtitlePosition Position)
{
	SubtitlePosition = Position;
	UpdateSubtitlePosition();
}

void UJellyfinSubtitleRenderer::SetStyle(const FSubtitleStyle& Style)
{
	CurrentStyle = Style;
	ApplyStyle();
}

void UJellyfinSubtitleRenderer::ApplyStyle()
{
	if (!TextRenderer)
	{
		return;
	}

	TextRenderer->SetWorldSize(CurrentStyle.FontSize);
	TextRenderer->SetTextRenderColor(CurrentStyle.TextColor.ToFColor(true));

	// Note: Background and outline would require a custom material
	// For now, we rely on text color contrast
}

void UJellyfinSubtitleRenderer::SetScreenTransform(const FTransform& ScreenTransform, float InScreenWidth, float InScreenHeight)
{
	ScreenWorldTransform = ScreenTransform;
	ScreenWidth = InScreenWidth;
	ScreenHeight = InScreenHeight;
	UpdateSubtitlePosition();
}

void UJellyfinSubtitleRenderer::SetCurrentText(const FString& Text)
{
	if (CurrentSubtitleText != Text)
	{
		CurrentSubtitleText = Text;

		if (TextRenderer)
		{
			if (Text.IsEmpty())
			{
				TextRenderer->SetVisibility(false);
			}
			else
			{
				TextRenderer->SetText(FText::FromString(Text));
				TextRenderer->SetVisibility(bSubtitlesEnabled);
			}
		}
	}
}

void UJellyfinSubtitleRenderer::SetTimeOffset(float OffsetSeconds)
{
	TimeOffsetSeconds = OffsetSeconds;
}

void UJellyfinSubtitleRenderer::UpdateSubtitleDisplay()
{
	if (!MediaPlayer)
	{
		return;
	}

	float CurrentTime = MediaPlayer->GetTime().GetTotalSeconds() + TimeOffsetSeconds;

	// Find active cue
	FJellyfinSubtitleCue* ActiveCue = FindActiveCue(CurrentTime);

	if (ActiveCue)
	{
		SetCurrentText(ActiveCue->Text);
	}
	else
	{
		SetCurrentText(TEXT(""));
	}
}

FJellyfinSubtitleCue* UJellyfinSubtitleRenderer::FindActiveCue(float Time)
{
	// Optimize by checking last active cue first
	if (LastActiveCueIndex >= 0 && LastActiveCueIndex < SubtitleCues.Num())
	{
		if (SubtitleCues[LastActiveCueIndex].IsActiveAt(Time))
		{
			return &SubtitleCues[LastActiveCueIndex];
		}

		// Check next cue (common case: sequential playback)
		int32 NextIndex = LastActiveCueIndex + 1;
		if (NextIndex < SubtitleCues.Num() && SubtitleCues[NextIndex].IsActiveAt(Time))
		{
			LastActiveCueIndex = NextIndex;
			return &SubtitleCues[NextIndex];
		}
	}

	// Binary search for active cue
	int32 Low = 0;
	int32 High = SubtitleCues.Num() - 1;

	while (Low <= High)
	{
		int32 Mid = (Low + High) / 2;
		const FJellyfinSubtitleCue& Cue = SubtitleCues[Mid];

		if (Time < Cue.StartTime)
		{
			High = Mid - 1;
		}
		else if (Time >= Cue.EndTime)
		{
			Low = Mid + 1;
		}
		else
		{
			// Found active cue
			LastActiveCueIndex = Mid;
			return &SubtitleCues[Mid];
		}
	}

	LastActiveCueIndex = -1;
	return nullptr;
}

void UJellyfinSubtitleRenderer::UpdateSubtitlePosition()
{
	if (!TextRenderer)
	{
		return;
	}

	switch (SubtitlePosition)
	{
	case ESubtitlePosition::BelowScreen:
		{
			// Position below the screen
			FVector ScreenBottom = ScreenWorldTransform.GetLocation();
			FVector ScreenDown = -ScreenWorldTransform.GetUnitAxis(EAxis::Z);
			ScreenBottom += ScreenDown * (ScreenHeight * 0.5f + BelowScreenOffset);

			TextRenderer->SetWorldLocation(ScreenBottom);
			TextRenderer->SetWorldRotation(ScreenWorldTransform.GetRotation());
		}
		break;

	case ESubtitlePosition::ScreenBottom:
		{
			// Position at bottom of screen (overlay style)
			FVector ScreenBottom = ScreenWorldTransform.GetLocation();
			FVector ScreenDown = -ScreenWorldTransform.GetUnitAxis(EAxis::Z);
			FVector ScreenForward = ScreenWorldTransform.GetUnitAxis(EAxis::X);

			// Slightly in front and at bottom
			ScreenBottom += ScreenDown * (ScreenHeight * 0.4f);
			ScreenBottom += ScreenForward * 5.0f; // Slightly in front to avoid z-fighting

			TextRenderer->SetWorldLocation(ScreenBottom);
			TextRenderer->SetWorldRotation(ScreenWorldTransform.GetRotation());
		}
		break;

	case ESubtitlePosition::FloatingHUD:
		{
			// Position in front of the player's view
			if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
			{
				if (APlayerCameraManager* CameraManager = PC->PlayerCameraManager)
				{
					FVector CameraLocation = CameraManager->GetCameraLocation();
					FRotator CameraRotation = CameraManager->GetCameraRotation();

					FVector Forward = CameraRotation.Vector();
					FVector SubtitleLocation = CameraLocation + Forward * FloatingDistance;

					// Lower it slightly for comfortable reading
					SubtitleLocation.Z -= 20.0f;

					TextRenderer->SetWorldLocation(SubtitleLocation);
					TextRenderer->SetWorldRotation(CameraRotation);
				}
			}
		}
		break;
	}
}
