#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Data/TFDataTypes.h"
#include "TFPlayerController.generated.h"

class ATFBuildingActor;
class ATFStrategyPawn;

/**
 * M0 girdi katmani: kamera kontrolu ve yapi yerlestirme modu.
 * Not: M0'da tuslar dogrudan okunur (asset gerektirmez). Prototype 0.1'de Enhanced Input
 * varliklarina ve yeniden atanabilir tuslara gecilecek (bkz. DECISIONS.md D-007).
 */
UCLASS()
class TERRAFORGE_API ATFPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATFPlayerController();

	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;

	bool IsInBuildMode() const { return bBuildMode; }
	FName GetSelectedBuildingId() const { return SelectedBuildingId; }
	int32 GetQuarterTurns() const { return QuarterTurns; }
	ETFPlacementResult GetLastEvaluation() const { return LastEvaluation; }
	const FString& GetStatusKey() const { return StatusKey; }
	float GetStatusAge() const;

	UPROPERTY(EditAnywhere, Category = "Camera")
	bool bEdgeScroll = true;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float EdgeScrollMarginPx = 12.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float KeyboardYawSpeed = 90.f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float MouseYawSensitivity = 0.4f;

private:
	void HandleCameraInput(float DeltaTime);
	void HandleBuildInput();
	void EnterBuildMode(int32 DefIndex);
	void ExitBuildMode();
	void UpdateGhost();
	void SetStatus(const FString& Key);

	ATFStrategyPawn* GetStrategyPawn() const;

	UPROPERTY()
	TObjectPtr<ATFBuildingActor> Ghost;

	bool bBuildMode = false;
	bool bGhostHasTarget = false;
	FName SelectedBuildingId;
	int32 QuarterTurns = 0;
	FIntPoint HoverCell = FIntPoint::ZeroValue;
	ETFPlacementResult LastEvaluation = ETFPlacementResult::NotReady;
	FString StatusKey;
	double StatusTime = -1000.0;
};
