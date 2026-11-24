#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Data/TFDataTypes.h"
#include "Sim/TFHeightField.h"
#include "Sim/TFPlacement.h"
#include "TFPlacementSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FTFOnBuildingPlaced, const FTFPlacedBuilding&);

/**
 * Yapi yerlesiminin simulasyon tarafi: doluluk izgarasi + yapi kayitlari.
 * Gorsel aktor uretmez; yerlesimi OnBuildingPlaced ile duyurur (gorsellestirme ayri).
 */
UCLASS()
class TERRAFORGE_API UTFPlacementSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// Arazi olusturulduktan sonra cagrilir. OriginCm: arazinin dunya konumu.
	void InitializeTerrain(const TF::Sim::HeightField& InHeightField, double InMaxHeightDeltaM, const FVector& InOriginCm);
	bool IsReady() const { return bReady; }

	// Dunya konumunu izgara hucresine cevirir (sinir kontrolu yapmaz).
	FIntPoint WorldToCell(const FVector& WorldCm) const;
	FIntPoint GetGridSize() const { return FIntPoint(Grid.GetCellsX(), Grid.GetCellsY()); }

	TF::Sim::Footprint MakeFootprint(const FTFBuildingDef& Def, const FIntPoint& CenterCell, int32 QuarterTurns) const;
	ETFPlacementResult Evaluate(const TF::Sim::Footprint& Footprint) const;
	ETFPlacementResult TryPlace(const FTFBuildingDef& Def, const FIntPoint& CenterCell, int32 QuarterTurns, FTFPlacedBuilding& OutRecord);

	// Footprint icin gorsel kutu: merkez ve boyut (cm). Temel, en alcak noktanin biraz altina iner.
	void ComputeVisualBox(const TF::Sim::Footprint& Footprint, float HeightMeters, FVector& OutCenterCm, FVector& OutSizeCm) const;

	const TArray<FTFPlacedBuilding>& GetPlacedBuildings() const { return Placed; }

	FTFOnBuildingPlaced OnBuildingPlaced;

	static ETFPlacementResult Convert(TF::Sim::PlacementResult Result);
	static FString ResultKey(ETFPlacementResult Result);

private:
	TF::Sim::HeightField HeightField;
	TF::Sim::OccupancyGrid Grid;
	TArray<FTFPlacedBuilding> Placed;
	FVector OriginCm = FVector::ZeroVector;
	double MaxHeightDeltaM = 1.5;
	int32 NextInstanceId = 1;
	bool bReady = false;
};
