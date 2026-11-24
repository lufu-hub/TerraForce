#include "TFGameMode.h"

#include "TerraForge.h"
#include "Building/TFBuildingActor.h"
#include "Building/TFPlacementSubsystem.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/SkyLightComponent.h"
#include "Data/TFDataSubsystem.h"
#include "Engine/DirectionalLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/GameInstance.h"
#include "Engine/SkyLight.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Player/TFPlayerController.h"
#include "Player/TFStrategyPawn.h"
#include "TimerManager.h"
#include "UI/TFHUD.h"
#include "World/TFTerrainActor.h"

ATFGameMode::ATFGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	DefaultPawnClass = ATFStrategyPawn::StaticClass();
	PlayerControllerClass = ATFPlayerController::StaticClass();
	HUDClass = ATFHUD::StaticClass();
}

void ATFGameMode::StartPlay()
{
	SpawnEnvironmentIfMissing();
	SpawnTerrain();

	if (UTFPlacementSubsystem* Placement = GetWorld()->GetSubsystem<UTFPlacementSubsystem>())
	{
		Placement->OnBuildingPlaced.AddUObject(this, &ATFGameMode::HandleBuildingPlaced);
	}

	Super::StartPlay();

	bSmokeMode = FParse::Param(FCommandLine::Get(), TEXT("tfsmoke"));
	if (bSmokeMode)
	{
		UE_LOG(LogTerraForge, Display, TEXT("TF_SMOKE_START"));
		GetWorldTimerManager().SetTimer(SmokeTimer, this, &ATFGameMode::RunSmokeTest, 5.f, false);
	}
}

void ATFGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bSmokeMode)
	{
		++SmokeFrames;
		SmokeTimeSeconds += DeltaSeconds;
	}
}

void ATFGameMode::SpawnEnvironmentIfMissing()
{
	UWorld* World = GetWorld();

	if (!TActorIterator<ADirectionalLight>(World))
	{
		const FTransform SunTransform(FRotator(-40.f, 35.f, 0.f));
		ADirectionalLight* Sun = World->SpawnActorDeferred<ADirectionalLight>(ADirectionalLight::StaticClass(), SunTransform);
		if (Sun != nullptr)
		{
			if (UDirectionalLightComponent* SunLight = Cast<UDirectionalLightComponent>(Sun->GetLightComponent()))
			{
				SunLight->SetMobility(EComponentMobility::Movable);
				SunLight->SetAtmosphereSunLight(true);
			}
			Sun->FinishSpawning(SunTransform);
		}
	}

	if (!TActorIterator<ASkyAtmosphere>(World))
	{
		World->SpawnActor<ASkyAtmosphere>(ASkyAtmosphere::StaticClass(), FTransform::Identity);
	}

	if (!TActorIterator<ASkyLight>(World))
	{
		const FTransform SkyTransform = FTransform::Identity;
		ASkyLight* Sky = World->SpawnActorDeferred<ASkyLight>(ASkyLight::StaticClass(), SkyTransform);
		if (Sky != nullptr)
		{
			if (USkyLightComponent* SkyLight = Sky->GetLightComponent())
			{
				SkyLight->SetMobility(EComponentMobility::Movable);
				SkyLight->bRealTimeCapture = true;
			}
			Sky->FinishSpawning(SkyTransform);
		}
	}

	if (!TActorIterator<AExponentialHeightFog>(World))
	{
		World->SpawnActor<AExponentialHeightFog>(AExponentialHeightFog::StaticClass(), FTransform::Identity);
	}
}

ATFTerrainActor* ATFGameMode::SpawnTerrain()
{
	UWorld* World = GetWorld();
	const UTFDataSubsystem* Data = GetGameInstance()->GetSubsystem<UTFDataSubsystem>();
	const FTFTerrainConfig Config = Data != nullptr ? Data->GetTerrainConfig() : FTFTerrainConfig();

	ATFTerrainActor* Terrain = nullptr;
	for (TActorIterator<ATFTerrainActor> It(World); It; ++It)
	{
		Terrain = *It;
		break;
	}
	if (Terrain == nullptr)
	{
		Terrain = World->SpawnActor<ATFTerrainActor>(ATFTerrainActor::StaticClass(), FTransform::Identity);
	}
	if (Terrain == nullptr)
	{
		UE_LOG(LogTerraForge, Error, TEXT("Arazi aktoru olusturulamadi."));
		return nullptr;
	}

	Terrain->BuildFromConfig(Config);
	if (UTFPlacementSubsystem* Placement = World->GetSubsystem<UTFPlacementSubsystem>())
	{
		Placement->InitializeTerrain(Terrain->GetHeightField(), Config.MaxBuildHeightDeltaMeters, Terrain->GetActorLocation());
	}
	return Terrain;
}

void ATFGameMode::HandleBuildingPlaced(const FTFPlacedBuilding& Record)
{
	const UTFDataSubsystem* Data = GetGameInstance()->GetSubsystem<UTFDataSubsystem>();
	const UTFPlacementSubsystem* Placement = GetWorld()->GetSubsystem<UTFPlacementSubsystem>();
	const FTFBuildingDef* Def = Data != nullptr ? Data->FindBuildingDef(Record.DefId) : nullptr;
	if (Def == nullptr || Placement == nullptr)
	{
		return;
	}

	TF::Sim::Footprint Footprint;
	Footprint.X = Record.Origin.X;
	Footprint.Y = Record.Origin.Y;
	Footprint.W = Record.Size.X;
	Footprint.H = Record.Size.Y;

	FVector Center;
	FVector Size;
	Placement->ComputeVisualBox(Footprint, Def->HeightMeters, Center, Size);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (ATFBuildingActor* Visual = GetWorld()->SpawnActor<ATFBuildingActor>(ATFBuildingActor::StaticClass(), FTransform::Identity, Params))
	{
		Visual->InstanceId = Record.InstanceId;
		Visual->SetBox(Center, Size);
		Visual->SetColor(Def->Color);
	}
}

void ATFGameMode::RunSmokeTest()
{
	TArray<FString> Failures;
	UWorld* World = GetWorld();
	const UTFDataSubsystem* Data = GetGameInstance()->GetSubsystem<UTFDataSubsystem>();
	UTFPlacementSubsystem* Placement = World->GetSubsystem<UTFPlacementSubsystem>();

	if (Data == nullptr || Data->GetBuildingDefs().Num() == 0) { Failures.Add(TEXT("veri yuklenmedi")); }
	if (Placement == nullptr || !Placement->IsReady()) { Failures.Add(TEXT("yerlesim hazir degil")); }

	ATFTerrainActor* Terrain = nullptr;
	for (TActorIterator<ATFTerrainActor> It(World); It; ++It) { Terrain = *It; break; }
	if (Terrain == nullptr || !Terrain->IsBuilt())
	{
		Failures.Add(TEXT("arazi yok"));
	}
	else
	{
		// Arazi carpismasi: merkezde yukaridan asagi isin zemine carpmali.
		const FVector Center = Terrain->GetWorldCenter();
		FHitResult Hit;
		const bool bHit = World->LineTraceSingleByChannel(Hit, Center + FVector(0, 0, 10000), Center - FVector(0, 0, 10000), ECC_Visibility);
		if (!bHit || Hit.GetActor() != Terrain) { Failures.Add(TEXT("arazi carpismasi yok")); }
	}

	if (!Cast<ATFStrategyPawn>(GetWorld()->GetFirstPlayerController() ? GetWorld()->GetFirstPlayerController()->GetPawn() : nullptr))
	{
		Failures.Add(TEXT("strateji kamerasi pawn'i yok"));
	}

	if (Failures.Num() == 0)
	{
		const FTFBuildingDef& Def = Data->GetBuildingDefs()[0];
		const FIntPoint GridSize = Placement->GetGridSize();
		FIntPoint Found(-1, -1);
		for (int32 Y = 0; Y < GridSize.Y && Found.X < 0; Y += 2)
		{
			for (int32 X = 0; X < GridSize.X; X += 2)
			{
				if (Placement->Evaluate(Placement->MakeFootprint(Def, FIntPoint(X, Y), 0)) == ETFPlacementResult::Ok)
				{
					Found = FIntPoint(X, Y);
					break;
				}
			}
		}
		FTFPlacedBuilding Record;
		const int32 ActorsBefore = [World]() { int32 N = 0; for (TActorIterator<ATFBuildingActor> It(World); It; ++It) { ++N; } return N; }();
		if (Found.X < 0) { Failures.Add(TEXT("gecerli yerlesim yeri bulunamadi")); }
		else if (Placement->TryPlace(Def, Found, 0, Record) != ETFPlacementResult::Ok) { Failures.Add(TEXT("yerlesim basarisiz")); }
		else
		{
			if (Placement->TryPlace(Def, Found, 0, Record) != ETFPlacementResult::Overlap) { Failures.Add(TEXT("cakisma algilanmadi")); }
			if (Placement->TryPlace(Def, FIntPoint(-20, -20), 0, Record) != ETFPlacementResult::OutOfBounds) { Failures.Add(TEXT("sinir disi algilanmadi")); }
			int32 ActorsAfter = 0;
			for (TActorIterator<ATFBuildingActor> It(World); It; ++It) { ++ActorsAfter; }
			if (ActorsAfter != ActorsBefore + 1) { Failures.Add(TEXT("yapi gorseli olusmadi")); }
		}
	}

	const double AvgFps = SmokeTimeSeconds > 0.0 ? SmokeFrames / SmokeTimeSeconds : 0.0;
	UE_LOG(LogTerraForge, Display, TEXT("TF_SMOKE_FPS %.1f (%d kare / %.1f sn)"), AvgFps, SmokeFrames, SmokeTimeSeconds);
	if (Failures.Num() == 0)
	{
		UE_LOG(LogTerraForge, Display, TEXT("TF_SMOKE_RESULT PASS"));
	}
	else
	{
		UE_LOG(LogTerraForge, Error, TEXT("TF_SMOKE_RESULT FAIL: %s"), *FString::Join(Failures, TEXT("; ")));
	}
	UKismetSystemLibrary::QuitGame(World, nullptr, EQuitPreference::Quit, false);
}
