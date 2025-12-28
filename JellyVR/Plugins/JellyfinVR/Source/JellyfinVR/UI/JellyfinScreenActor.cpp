// Copyright JellyVR Project. All Rights Reserved.

#include "JellyfinScreenActor.h"
#include "JellyfinVRModule.h"
#include "JellyfinSimpleUI.h"
#include "TimerManager.h"
#include "Media/JellyfinMediaPlayer.h"
#include "API/JellyfinClient.h"
#include "API/JellyfinAuth.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMesh.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "MediaTexture.h"
#include "Blueprint/UserWidget.h"
#include "HeadMountedDisplayFunctionLibrary.h"

AJellyfinScreenActor::AJellyfinScreenActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;

	// Create root component
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootSceneComponent;

	// Create screen mesh
	ScreenMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ScreenMesh"));
	ScreenMesh->SetupAttachment(RootComponent);
	ScreenMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Use a plane mesh - in production this would be set in Blueprint
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane"));
	if (PlaneMesh.Succeeded())
	{
		ScreenMesh->SetStaticMesh(PlaneMesh.Object);
	}

	// Create frame mesh (optional decorative frame)
	FrameMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FrameMesh"));
	FrameMesh->SetupAttachment(RootComponent);
	FrameMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FrameMesh->SetVisibility(false); // Disabled by default

	// Create UI widget for Jellyfin browser
	// Attach to ScreenMesh so it inherits the screen's rotation
	UIWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("UIWidget"));
	UIWidget->SetupAttachment(ScreenMesh);
	UIWidget->SetDrawSize(FVector2D(1920, 1080));
	UIWidget->SetPivot(FVector2D(0.5f, 0.5f));
	UIWidget->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	UIWidget->SetWidgetSpace(EWidgetSpace::World);
	UIWidget->SetTwoSided(true);
	UIWidget->SetWindowFocusable(true); // Enable keyboard focus for text input
	// Relative to ScreenMesh: rotate -90 pitch to counter the mesh rotation, then face outward
	UIWidget->SetRelativeRotation(FRotator(90.0f, 0.0f, 180.0f));
	UIWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 1.0f));
	UIWidget->SetVisibility(true);

	// Create controls overlay widget
	ControlsWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ControlsWidget"));
	ControlsWidget->SetupAttachment(ScreenMesh);
	ControlsWidget->SetDrawSize(FVector2D(1920, 200));
	ControlsWidget->SetPivot(FVector2D(0.5f, 1.0f)); // Anchor to bottom
	ControlsWidget->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ControlsWidget->SetVisibility(false);

	// Create media player component
	MediaPlayer = CreateDefaultSubobject<UJellyfinMediaPlayerComponent>(TEXT("MediaPlayer"));
}

void AJellyfinScreenActor::BeginPlay()
{
	Super::BeginPlay();

	// Set up the screen dimensions
	UpdateScreenTransform();

	// Set up materials
	SetupMaterials();

	// Set up the built-in UI widgets
	SetupWidgets();

	// Bind to media player events
	if (MediaPlayer)
	{
		MediaPlayer->OnPlaybackStateChanged.AddDynamic(this, &AJellyfinScreenActor::OnPlaybackStateChanged);
		MediaPlayer->OnPlaybackEnded.AddDynamic(this, &AJellyfinScreenActor::OnPlaybackEnded);
	}

	// Check if we have saved credentials and can auto-connect
	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
	{
		if (UJellyfinAuthSubsystem* AuthSubsystem = GameInstance->GetSubsystem<UJellyfinAuthSubsystem>())
		{
			// Subscribe to auth state changes for navigation after login
			AuthSubsystem->OnConnectionStateChanged.AddDynamic(this, &AJellyfinScreenActor::OnAuthStateChanged);

			if (AuthSubsystem->TryAutoConnect())
			{
				// Will switch to UI mode when connected
			}
			else
			{
				// Show settings/login screen
				ShowSettings();
			}
		}
	}

	UE_LOG(LogJellyfinVR, Log, TEXT("JellyfinScreenActor initialized - Size: %.0f x %.0f"),
		ScreenWidth, ScreenWidth / AspectRatio);
}

void AJellyfinScreenActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Auto-hide controls during video playback
	if (ScreenMode == EJellyfinScreenMode::Video && bControlsVisible && ControlsAutoHideDelay > 0.0f)
	{
		TimeSinceLastInteraction += DeltaTime;
		if (TimeSinceLastInteraction >= ControlsAutoHideDelay)
		{
			SetControlsVisible(false);
		}
	}
}

void AJellyfinScreenActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	UpdateScreenTransform();
}

void AJellyfinScreenActor::UpdateScreenTransform()
{
	if (!ScreenMesh)
	{
		return;
	}

	// Calculate screen dimensions in world units (cm)
	float ScreenHeight = ScreenWidth / AspectRatio;

	// Scale the plane mesh (default plane is 100x100 units)
	float ScaleX = ScreenWidth / 100.0f;
	float ScaleY = ScreenHeight / 100.0f;

	ScreenMesh->SetRelativeScale3D(FVector(ScaleX, ScaleY, 1.0f));

	// Rotate to face forward (plane default is horizontal)
	ScreenMesh->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));

	// Position UI widget to match screen size
	// UIWidget is attached to ScreenMesh, so coordinates are relative to the mesh
	if (UIWidget)
	{
		// Draw size is in PIXELS for the render target
		UIWidget->SetDrawSize(FVector2D(1920.0f, 1080.0f));

		// Scale widget to match screen world size
		// The mesh is scaled by ScaleX/ScaleY, so the widget needs inverse scaling
		// to maintain proper size, then scale to match desired screen dimensions
		float WidgetScaleX = 1.0f; // Widget will be sized by DrawSize
		float WidgetScaleY = 1.0f;
		UIWidget->SetRelativeScale3D(FVector(WidgetScaleX, WidgetScaleY, 1.0f));

		// Position at center of mesh, slightly in front
		// Mesh is 100x100 units before scaling, centered at origin
		UIWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 1.0f));
		UIWidget->SetRelativeRotation(FRotator(90.0f, 0.0f, 180.0f));
	}

	// Position controls at bottom of screen
	if (ControlsWidget)
	{
		ControlsWidget->SetDrawSize(FVector2D(1920.0f, 200.0f));
		float ControlsScaleX = ScreenWidth / 1920.0f;
		ControlsWidget->SetRelativeScale3D(FVector(ControlsScaleX, 1.0f, 1.0f));
		ControlsWidget->SetRelativeLocation(FVector(5.0f, 0.0f, 50.0f));
	}
}

void AJellyfinScreenActor::SetupMaterials()
{
	if (!ScreenMesh)
	{
		return;
	}

	// Create dynamic material instance for the screen
	UMaterialInterface* BaseMaterial = ScreenMesh->GetMaterial(0);
	if (BaseMaterial)
	{
		ScreenMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		ScreenMesh->SetMaterial(0, ScreenMaterial);
	}

	// Set up video texture when we have it
	if (MediaPlayer && MediaPlayer->GetMediaTexture())
	{
		if (ScreenMaterial)
		{
			ScreenMaterial->SetTextureParameterValue(TEXT("VideoTexture"), Cast<UTexture>(MediaPlayer->GetMediaTexture()));
		}
	}
}

void AJellyfinScreenActor::SetupWidgets()
{
	UE_LOG(LogJellyfinVR, Log, TEXT("SetupWidgets called - UIWidget valid: %s"), UIWidget ? TEXT("YES") : TEXT("NO"));

	// Create the built-in login widget automatically
	if (UIWidget)
	{
		UE_LOG(LogJellyfinVR, Log, TEXT("UIWidget exists, current widget: %s"), UIWidget->GetWidget() ? TEXT("HAS WIDGET") : TEXT("NONE"));

		if (!UIWidget->GetWidget())
		{
			// Try to get the first player controller for widget creation
			APlayerController* PC = GetWorld()->GetFirstPlayerController();

			UJellyfinLoginWidget* LoginWidget = nullptr;
			if (PC)
			{
				LoginWidget = CreateWidget<UJellyfinLoginWidget>(PC, UJellyfinLoginWidget::StaticClass());
				UE_LOG(LogJellyfinVR, Log, TEXT("Created widget with PlayerController"));
			}
			else
			{
				LoginWidget = CreateWidget<UJellyfinLoginWidget>(GetWorld(), UJellyfinLoginWidget::StaticClass());
				UE_LOG(LogJellyfinVR, Log, TEXT("Created widget with World (no PC)"));
			}

			if (LoginWidget)
			{
				// Check if we're in desktop mode (no HMD connected)
				bIsDesktopMode = !UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled() ||
				                 !UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayConnected();

				if (bIsDesktopMode)
				{
					// Desktop mode: Add to viewport for reliable keyboard input
					LoginWidget->AddToViewport(0);
					ViewportWidget = LoginWidget;
					UIWidget->SetVisibility(false); // Hide 3D widget

					UE_LOG(LogJellyfinVR, Log, TEXT("Desktop mode: Login widget added to viewport for keyboard input"));
				}
				else
				{
					// VR mode: Use 3D world widget
					UIWidget->SetWidget(LoginWidget);
					UIWidget->SetVisibility(true);

					// Force rotation at runtime to override any Blueprint cached values
					UIWidget->SetRelativeRotation(FRotator(90.0f, 0.0f, 180.0f));
					UIWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 1.0f));

					// Enable mouse interaction for testing in editor
					UIWidget->SetWindowFocusable(true);

					UE_LOG(LogJellyfinVR, Log, TEXT("VR mode: Built-in login widget assigned to 3D widget. Rotation: %s"),
						*UIWidget->GetRelativeRotation().ToString());
				}
			}
			else
			{
				UE_LOG(LogJellyfinVR, Error, TEXT("FAILED: Could not create login widget!"));
			}
		}
	}
	else
	{
		UE_LOG(LogJellyfinVR, Error, TEXT("UIWidget component is NULL!"));
	}
}

void AJellyfinScreenActor::ShowUI()
{
	ScreenMode = EJellyfinScreenMode::UI;

	// Check if we're logged in and should show home instead of login
	bool bIsLoggedIn = false;
	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
	{
		if (UJellyfinAuthSubsystem* AuthSubsystem = GameInstance->GetSubsystem<UJellyfinAuthSubsystem>())
		{
			bIsLoggedIn = AuthSubsystem->IsConnected();
		}
	}

	if (bIsLoggedIn && HomeWidget)
	{
		// Show the home widget
		ShowHome();
	}
	else if (UIWidget)
	{
		UIWidget->SetVisibility(true);
	}

	if (ControlsWidget)
	{
		ControlsWidget->SetVisibility(false);
	}

	// Make screen show the UI widget instead of video
	if (ScreenMaterial)
	{
		ScreenMaterial->SetScalarParameterValue(TEXT("ShowVideo"), 0.0f);
	}

	UE_LOG(LogJellyfinVR, Log, TEXT("Switched to UI mode"));
}

void AJellyfinScreenActor::ShowVideo()
{
	ScreenMode = EJellyfinScreenMode::Video;

	if (UIWidget)
	{
		UIWidget->SetVisibility(false);
	}

	// Show video on screen
	if (ScreenMaterial && MediaPlayer)
	{
		ScreenMaterial->SetTextureParameterValue(TEXT("VideoTexture"), Cast<UTexture>(MediaPlayer->GetMediaTexture()));
		ScreenMaterial->SetScalarParameterValue(TEXT("ShowVideo"), 1.0f);
		ScreenMaterial->SetScalarParameterValue(TEXT("Brightness"), ScreenBrightness);
	}

	UE_LOG(LogJellyfinVR, Log, TEXT("Switched to Video mode"));
}

void AJellyfinScreenActor::ShowSettings()
{
	ScreenMode = EJellyfinScreenMode::Settings;

	// The settings UI is handled by the UI widget
	if (UIWidget)
	{
		UIWidget->SetVisibility(true);
	}

	if (ControlsWidget)
	{
		ControlsWidget->SetVisibility(false);
	}

	UE_LOG(LogJellyfinVR, Log, TEXT("Switched to Settings mode"));
}

void AJellyfinScreenActor::PlayItem(const FJellyfinMediaItem& Item)
{
	if (MediaPlayer)
	{
		MediaPlayer->OpenItem(Item);
	}
}

UJellyfinClient* AJellyfinScreenActor::GetJellyfinClient() const
{
	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld()))
	{
		if (UJellyfinAuthSubsystem* AuthSubsystem = GameInstance->GetSubsystem<UJellyfinAuthSubsystem>())
		{
			return AuthSubsystem->GetClient();
		}
	}
	return nullptr;
}

void AJellyfinScreenActor::SetControlsVisible(bool bVisible)
{
	bControlsVisible = bVisible;

	if (ControlsWidget)
	{
		ControlsWidget->SetVisibility(bVisible && ScreenMode == EJellyfinScreenMode::Video);
	}

	if (bVisible)
	{
		TimeSinceLastInteraction = 0.0f;
	}
}

void AJellyfinScreenActor::ToggleControls()
{
	SetControlsVisible(!bControlsVisible);
}

void AJellyfinScreenActor::UpdateScreenDimensions(float NewWidth, float NewAspectRatio)
{
	ScreenWidth = NewWidth;
	AspectRatio = NewAspectRatio;
	UpdateScreenTransform();
}

void AJellyfinScreenActor::OnPlaybackStateChanged(EJellyfinPlaybackState NewState)
{
	switch (NewState)
	{
	case EJellyfinPlaybackState::Playing:
		ShowVideo();
		break;

	case EJellyfinPlaybackState::Paused:
		SetControlsVisible(true);
		break;

	case EJellyfinPlaybackState::Error:
		ShowUI();
		break;

	default:
		break;
	}
}

void AJellyfinScreenActor::OnPlaybackEnded()
{
	// Return to UI when video ends
	ShowUI();
}

void AJellyfinScreenActor::OnAuthStateChanged(EJellyfinAuthState NewState)
{
	if (NewState == EJellyfinAuthState::Authenticated)
	{
		// Successfully logged in - switch to home widget
		UE_LOG(LogJellyfinVR, Log, TEXT("Authentication successful - switching to home widget"));

		// Add a small delay to let the "Connected!" message show before switching
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AJellyfinScreenActor::SwitchToHomeWidget, 0.5f, false);
	}
}

void AJellyfinScreenActor::SwitchToHomeWidget()
{
	// Get the player controller for widget creation
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		UE_LOG(LogJellyfinVR, Error, TEXT("No player controller found for home widget creation"));
		return;
	}

	// Create the home widget
	HomeWidget = CreateWidget<UJellyfinSimpleHomeWidget>(PC, UJellyfinSimpleHomeWidget::StaticClass());
	if (!HomeWidget)
	{
		UE_LOG(LogJellyfinVR, Error, TEXT("Failed to create home widget"));
		return;
	}

	HomeWidget->SetOwningScreen(this);

	if (bIsDesktopMode)
	{
		// Desktop mode: Remove login widget from viewport and add home widget
		if (ViewportWidget)
		{
			ViewportWidget->RemoveFromParent();
		}

		HomeWidget->AddToViewport(0);
		ViewportWidget = HomeWidget;

		UE_LOG(LogJellyfinVR, Log, TEXT("Desktop mode: Switched to home widget in viewport"));
	}
	else
	{
		// VR mode: Switch widget on the 3D component
		if (UIWidget)
		{
			UIWidget->SetWidget(HomeWidget);
			UIWidget->SetVisibility(true);
		}

		UE_LOG(LogJellyfinVR, Log, TEXT("VR mode: Switched to home widget on 3D screen"));
	}

	// Switch to UI mode
	ScreenMode = EJellyfinScreenMode::UI;
}

void AJellyfinScreenActor::ShowLibrary(const FString& LibraryId, const FString& LibraryName)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		UE_LOG(LogJellyfinVR, Error, TEXT("No player controller found for library widget creation"));
		return;
	}

	// Create the library widget if needed
	if (!LibraryWidget)
	{
		LibraryWidget = CreateWidget<UJellyfinSimpleLibraryWidget>(PC, UJellyfinSimpleLibraryWidget::StaticClass());
		if (!LibraryWidget)
		{
			UE_LOG(LogJellyfinVR, Error, TEXT("Failed to create library widget"));
			return;
		}
		LibraryWidget->SetOwningScreen(this);
	}

	// Start browsing the library
	LibraryWidget->BrowseLibrary(LibraryId, LibraryName);

	if (bIsDesktopMode)
	{
		// Desktop mode: Switch viewport widget
		if (ViewportWidget)
		{
			ViewportWidget->RemoveFromParent();
		}

		LibraryWidget->AddToViewport(0);
		ViewportWidget = LibraryWidget;

		UE_LOG(LogJellyfinVR, Log, TEXT("Desktop mode: Switched to library widget - %s"), *LibraryName);
	}
	else
	{
		// VR mode: Switch 3D widget
		if (UIWidget)
		{
			UIWidget->SetWidget(LibraryWidget);
			UIWidget->SetVisibility(true);
		}

		UE_LOG(LogJellyfinVR, Log, TEXT("VR mode: Switched to library widget - %s"), *LibraryName);
	}

	ScreenMode = EJellyfinScreenMode::UI;
}

void AJellyfinScreenActor::ShowHome()
{
	if (!HomeWidget)
	{
		// Create home widget if it doesn't exist
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (!PC)
		{
			return;
		}

		HomeWidget = CreateWidget<UJellyfinSimpleHomeWidget>(PC, UJellyfinSimpleHomeWidget::StaticClass());
		if (!HomeWidget)
		{
			return;
		}
		HomeWidget->SetOwningScreen(this);
	}

	// Refresh home data
	HomeWidget->Refresh();

	if (bIsDesktopMode)
	{
		// Desktop mode: Switch viewport widget
		if (ViewportWidget && ViewportWidget != HomeWidget)
		{
			ViewportWidget->RemoveFromParent();
		}

		if (!HomeWidget->IsInViewport())
		{
			HomeWidget->AddToViewport(0);
		}
		ViewportWidget = HomeWidget;

		UE_LOG(LogJellyfinVR, Log, TEXT("Desktop mode: Switched to home widget"));
	}
	else
	{
		// VR mode: Switch 3D widget
		if (UIWidget)
		{
			UIWidget->SetWidget(HomeWidget);
			UIWidget->SetVisibility(true);
		}

		UE_LOG(LogJellyfinVR, Log, TEXT("VR mode: Switched to home widget"));
	}

	ScreenMode = EJellyfinScreenMode::UI;
}
