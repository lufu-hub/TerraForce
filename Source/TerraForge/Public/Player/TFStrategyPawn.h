#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TFStrategyPawn.generated.h"

class USceneComponent;
class USpringArmComponent;
class UCameraComponent;
class ATFTerrainActor;

/**
 * Serbest strateji kamerasi. Girdi PlayerController tarafindan Add*Input ile verilir;
 * pawn hareketi arazi sinirlari icinde tutar ve odak noktasini zemine yapistirir.
 */
UCLASS()
class TERRAFORGE_API ATFStrategyPawn : public APawn
{
	GENERATED_BODY()

public:
	ATFStrategyPawn();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// X = ileri, Y = sag; -1..1 araliginda.
	void AddPanInput(const FVector2D& Direction);
	// Pozitif = yaklas.
	void AddZoomInput(float Steps);
	void AddYawInput(float Degrees);

	void FocusOnTerrain();

	UPROPERTY(EditAnywhere, Category = "Camera")
	float MinArmLength = 1500.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float MaxArmLength = 14000.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float ZoomStepFraction = 0.12f;

	// Kol uzunlugu basina saniyede kaydirma hizi.
	UPROPERTY(EditAnywhere, Category = "Camera")
	float PanSpeedPerArm = 1.1f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float PitchNear = -30.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float PitchFar = -62.f;

private:
	ATFTerrainActor* FindTerrain();

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY()
	TWeakObjectPtr<ATFTerrainActor> Terrain;

	FVector2D PendingPan = FVector2D::ZeroVector;
	float TargetArmLength = 6000.f;
	bool bFocused = false;
};
