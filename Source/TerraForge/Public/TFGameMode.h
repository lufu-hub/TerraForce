#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Data/TFDataTypes.h"
#include "TFGameMode.generated.h"

class ATFTerrainActor;

/**
 * M0 oyun modu. Bos bir haritada calisir: isik/gokyuzu ve araziyi calisma aninda olusturur,
 * yerlesim alt sistemini baslatir ve yerlesen yapilarin gorsellerini uretir.
 * Komut satirinda -tfsmoke verilirse otomatik duman testi kosar ve oyunu kapatir.
 */
UCLASS()
class TERRAFORGE_API ATFGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATFGameMode();

	virtual void StartPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	void SpawnEnvironmentIfMissing();
	ATFTerrainActor* SpawnTerrain();
	void HandleBuildingPlaced(const FTFPlacedBuilding& Record);
	void RunSmokeTest();

	bool bSmokeMode = false;
	int32 SmokeFrames = 0;
	double SmokeTimeSeconds = 0.0;
	FTimerHandle SmokeTimer;
};
