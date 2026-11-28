#include "Player/TFPlayerController.h"

#include "TerraForge.h"
#include "Building/TFBuildingActor.h"
#include "Building/TFPlacementSubsystem.h"
#include "Data/TFDataSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "InputCoreTypes.h"
#include "Player/TFStrategyPawn.h"

ATFPlayerController::ATFPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = false;
	bEnableMouseOverEvents = false;
	DefaultMouseCursor = EMouseCursor::Default;
}

void ATFPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
	SetInputMode(InputMode);
}

ATFStrategyPawn* ATFPlayerController::GetStrategyPawn() const
{
	return Cast<ATFStrategyPawn>(GetPawn());
}

float ATFPlayerController::GetStatusAge() const
{
	const UWorld* World = GetWorld();
	return World != nullptr ? static_cast<float>(World->GetTimeSeconds() - StatusTime) : 1000.f;
}

void ATFPlayerController::SetStatus(const FString& Key)
{
	StatusKey = Key;
	const UWorld* World = GetWorld();
	StatusTime = World != nullptr ? World->GetTimeSeconds() : 0.0;
}

void ATFPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	if (!IsLocalController())
	{
		return;
	}
	HandleCameraInput(DeltaTime);
	HandleBuildInput();
}

void ATFPlayerController::HandleCameraInput(float DeltaTime)
{
	ATFStrategyPawn* CameraPawn = GetStrategyPawn();
	if (CameraPawn == nullptr)
	{
		return;
	}

	FVector2D Pan = FVector2D::ZeroVector;
	if (IsInputKeyDown(EKeys::W) || IsInputKeyDown(EKeys::Up)) { Pan.X += 1.f; }
	if (IsInputKeyDown(EKeys::S) || IsInputKeyDown(EKeys::Down)) { Pan.X -= 1.f; }
	if (IsInputKeyDown(EKeys::D) || IsInputKeyDown(EKeys::Right)) { Pan.Y += 1.f; }
	if (IsInputKeyDown(EKeys::A) || IsInputKeyDown(EKeys::Left)) { Pan.Y -= 1.f; }

	if (bEdgeScroll)
	{
		float MouseX = 0.f;
		float MouseY = 0.f;
		int32 ViewX = 0;
		int32 ViewY = 0;
		GetViewportSize(ViewX, ViewY);
		if (ViewX > 0 && ViewY > 0 && GetMousePosition(MouseX, MouseY))
		{
			if (MouseX <= EdgeScrollMarginPx) { Pan.Y -= 1.f; }
			if (MouseX >= ViewX - EdgeScrollMarginPx) { Pan.Y += 1.f; }
			if (MouseY <= EdgeScrollMarginPx) { Pan.X += 1.f; }
			if (MouseY >= ViewY - EdgeScrollMarginPx) { Pan.X -= 1.f; }
		}
	}
	CameraPawn->AddPanInput(Pan);

	float Yaw = 0.f;
	if (IsInputKeyDown(EKeys::Q)) { Yaw -= KeyboardYawSpeed * DeltaTime; }
	if (IsInputKeyDown(EKeys::E)) { Yaw += KeyboardYawSpeed * DeltaTime; }
	if (IsInputKeyDown(EKeys::MiddleMouseButton))
	{
		float DeltaX = 0.f;
		float DeltaY = 0.f;
		GetInputMouseDelta(DeltaX, DeltaY);
		Yaw += DeltaX * MouseYawSensitivity * 10.f;
	}
	if (!FMath::IsNearlyZero(Yaw))
	{
		CameraPawn->AddYawInput(Yaw);
	}

	float Zoom = 0.f;
	if (WasInputKeyJustPressed(EKeys::MouseScrollUp) || WasInputKeyJustPressed(EKeys::PageUp)) { Zoom += 1.f; }
	if (WasInputKeyJustPressed(EKeys::MouseScrollDown) || WasInputKeyJustPressed(EKeys::PageDown)) { Zoom -= 1.f; }
	if (!FMath::IsNearlyZero(Zoom))
	{
		CameraPawn->AddZoomInput(Zoom);
	}
}

void ATFPlayerController::HandleBuildInput()
{
	const UTFDataSubsystem* Data = GetGameInstance() != nullptr ? GetGameInstance()->GetSubsystem<UTFDataSubsystem>() : nullptr;
	if (Data == nullptr)
	{
		return;
	}

	// 1-9: ilgili yapi turunu sec; B: ilk turu sec / modu kapat.
	static const FKey NumberKeys[] = { EKeys::One, EKeys::Two, EKeys::Three, EKeys::Four, EKeys::Five, EKeys::Six, EKeys::Seven, EKeys::Eight, EKeys::Nine };
	for (int32 Index = 0; Index < static_cast<int32>(UE_ARRAY_COUNT(NumberKeys)); ++Index)
	{
		if (WasInputKeyJustPressed(NumberKeys[Index]) && Index < Data->GetBuildingDefs().Num())
		{
			EnterBuildMode(Index);
		}
	}
	if (WasInputKeyJustPressed(EKeys::B))
	{
		if (bBuildMode) { ExitBuildMode(); } else { EnterBuildMode(0); }
	}
	if (WasInputKeyJustPressed(EKeys::F10))
	{
		ConsoleCommand(TEXT("quit"));
	}

	if (!bBuildMode)
	{
		return;
	}

	if (WasInputKeyJustPressed(EKeys::Escape) || WasInputKeyJustPressed(EKeys::RightMouseButton))
	{
		ExitBuildMode();
		return;
	}
	if (WasInputKeyJustPressed(EKeys::R))
	{
		QuarterTurns = (QuarterTurns + 1) % 4;
	}

	UpdateGhost();

	if (WasInputKeyJustPressed(EKeys::LeftMouseButton) && bGhostHasTarget)
	{
		UTFPlacementSubsystem* Placement = GetWorld()->GetSubsystem<UTFPlacementSubsystem>();
		const FTFBuildingDef* Def = Data->FindBuildingDef(SelectedBuildingId);
		if (Placement != nullptr && Def != nullptr)
		{
			FTFPlacedBuilding Record;
			const ETFPlacementResult Result = Placement->TryPlace(*Def, HoverCell, QuarterTurns, Record);
			SetStatus(Result == ETFPlacementResult::Ok ? FString(TEXT("placement.placed")) : UTFPlacementSubsystem::ResultKey(Result));
			UpdateGhost();
		}
	}
}

void ATFPlayerController::EnterBuildMode(int32 DefIndex)
{
	const UTFDataSubsystem* Data = GetGameInstance()->GetSubsystem<UTFDataSubsystem>();
	if (Data == nullptr || !Data->GetBuildingDefs().IsValidIndex(DefIndex))
	{
		return;
	}
	SelectedBuildingId = Data->GetBuildingDefs()[DefIndex].Id;
	bBuildMode = true;

	if (Ghost == nullptr)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Ghost = GetWorld()->SpawnActor<ATFBuildingActor>(ATFBuildingActor::StaticClass(), FTransform::Identity, Params);
		if (Ghost != nullptr)
		{
			Ghost->SetGhost(true);
		}
	}
	if (Ghost != nullptr)
	{
		Ghost->SetActorHiddenInGame(true);
	}
}

void ATFPlayerController::ExitBuildMode()
{
	bBuildMode = false;
	bGhostHasTarget = false;
	if (Ghost != nullptr)
	{
		Ghost->SetActorHiddenInGame(true);
	}
}

void ATFPlayerController::UpdateGhost()
{
	bGhostHasTarget = false;
	UTFPlacementSubsystem* Placement = GetWorld()->GetSubsystem<UTFPlacementSubsystem>();
	const UTFDataSubsystem* Data = GetGameInstance()->GetSubsystem<UTFDataSubsystem>();
	const FTFBuildingDef* Def = Data != nullptr ? Data->FindBuildingDef(SelectedBuildingId) : nullptr;
	if (Ghost == nullptr || Placement == nullptr || Def == nullptr || !Placement->IsReady())
	{
		return;
	}

	FHitResult Hit;
	if (!GetHitResultUnderCursor(ECC_Visibility, false, Hit) || !Hit.bBlockingHit)
	{
		Ghost->SetActorHiddenInGame(true);
		return;
	}

	HoverCell = Placement->WorldToCell(Hit.ImpactPoint);
	const TF::Sim::Footprint Footprint = Placement->MakeFootprint(*Def, HoverCell, QuarterTurns);
	LastEvaluation = Placement->Evaluate(Footprint);

	FVector Center;
	FVector Size;
	Placement->ComputeVisualBox(Footprint, Def->HeightMeters, Center, Size);
	Ghost->SetBox(Center, Size);
	Ghost->SetColor(LastEvaluation == ETFPlacementResult::Ok ? FLinearColor(0.1f, 0.8f, 0.2f) : FLinearColor(0.9f, 0.1f, 0.1f));
	Ghost->SetActorHiddenInGame(false);
	bGhostHasTarget = true;
}
