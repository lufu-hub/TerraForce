#include "Player/TFStrategyPawn.h"

#include "World/TFTerrainActor.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "EngineUtils.h"
#include "GameFramework/SpringArmComponent.h"

ATFStrategyPawn::ATFStrategyPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Root);
	SpringArm->bDoCollisionTest = false;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 8.f;
	SpringArm->TargetArmLength = TargetArmLength;
	SpringArm->SetRelativeRotation(FRotator(-50.f, 0.f, 0.f));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->SetFieldOfView(55.f);
}

void ATFStrategyPawn::BeginPlay()
{
	Super::BeginPlay();
	FocusOnTerrain();
}

ATFTerrainActor* ATFStrategyPawn::FindTerrain()
{
	if (!Terrain.IsValid())
	{
		for (TActorIterator<ATFTerrainActor> It(GetWorld()); It; ++It)
		{
			Terrain = *It;
			break;
		}
	}
	return Terrain.Get();
}

void ATFStrategyPawn::FocusOnTerrain()
{
	ATFTerrainActor* Found = FindTerrain();
	if (Found != nullptr && Found->IsBuilt())
	{
		SetActorLocation(Found->GetWorldCenter());
		SetActorRotation(FRotator(0.f, 45.f, 0.f));
		bFocused = true;
	}
}

void ATFStrategyPawn::AddPanInput(const FVector2D& Direction)
{
	PendingPan += Direction;
}

void ATFStrategyPawn::AddZoomInput(float Steps)
{
	TargetArmLength = FMath::Clamp(TargetArmLength * FMath::Pow(1.f - ZoomStepFraction, Steps), MinArmLength, MaxArmLength);
}

void ATFStrategyPawn::AddYawInput(float Degrees)
{
	AddActorWorldRotation(FRotator(0.f, Degrees, 0.f));
}

void ATFStrategyPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bFocused)
	{
		FocusOnTerrain();
	}

	// Zum ve zuma bagli egim
	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, TargetArmLength, DeltaSeconds, 10.f);
	const float ZoomAlpha = (SpringArm->TargetArmLength - MinArmLength) / FMath::Max(1.f, MaxArmLength - MinArmLength);
	SpringArm->SetRelativeRotation(FRotator(FMath::Lerp(PitchNear, PitchFar, FMath::Clamp(ZoomAlpha, 0.f, 1.f)), 0.f, 0.f));

	// Kaydirma: yalnizca yatay duzlemde, kameranin baktigi yone gore
	FVector Location = GetActorLocation();
	if (!PendingPan.IsNearlyZero())
	{
		const FVector2D Pan = PendingPan.GetClampedToMaxSize(1.f);
		const FRotator YawOnly(0.f, GetActorRotation().Yaw, 0.f);
		const FVector Forward = YawOnly.Vector();
		const FVector Right = FRotationMatrix(YawOnly).GetUnitAxis(EAxis::Y);
		const float Speed = SpringArm->TargetArmLength * PanSpeedPerArm;
		Location += (Forward * Pan.X + Right * Pan.Y) * Speed * DeltaSeconds;
	}
	PendingPan = FVector2D::ZeroVector;

	if (ATFTerrainActor* Ground = FindTerrain())
	{
		const FBox2D Bounds = Ground->GetWorldBounds2D();
		if (Bounds.bIsValid)
		{
			Location.X = FMath::Clamp(Location.X, Bounds.Min.X, Bounds.Max.X);
			Location.Y = FMath::Clamp(Location.Y, Bounds.Min.Y, Bounds.Max.Y);
		}
		Location.Z = Ground->GetGroundZ(static_cast<float>(Location.X), static_cast<float>(Location.Y));
	}
	SetActorLocation(Location);
}
