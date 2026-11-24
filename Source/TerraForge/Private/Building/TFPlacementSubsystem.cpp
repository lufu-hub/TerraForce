#include "Building/TFPlacementSubsystem.h"

#include "TerraForge.h"

void UTFPlacementSubsystem::InitializeTerrain(const TF::Sim::HeightField& InHeightField, double InMaxHeightDeltaM, const FVector& InOriginCm)
{
	HeightField = InHeightField;
	Grid.Reset(HeightField.GetCellsX(), HeightField.GetCellsY());
	Placed.Reset();
	OriginCm = InOriginCm;
	MaxHeightDeltaM = InMaxHeightDeltaM;
	NextInstanceId = 1;
	bReady = HeightField.IsValid();
}

FIntPoint UTFPlacementSubsystem::WorldToCell(const FVector& WorldCm) const
{
	const double CellCm = HeightField.CellSize() * 100.0;
	return FIntPoint(
		FMath::FloorToInt32((WorldCm.X - OriginCm.X) / CellCm),
		FMath::FloorToInt32((WorldCm.Y - OriginCm.Y) / CellCm));
}

TF::Sim::Footprint UTFPlacementSubsystem::MakeFootprint(const FTFBuildingDef& Def, const FIntPoint& CenterCell, int32 QuarterTurns) const
{
	return TF::Sim::MakeCenteredFootprint(CenterCell.X, CenterCell.Y, Def.FootprintX, Def.FootprintY, QuarterTurns);
}

ETFPlacementResult UTFPlacementSubsystem::Evaluate(const TF::Sim::Footprint& Footprint) const
{
	if (!bReady)
	{
		return ETFPlacementResult::NotReady;
	}
	return Convert(TF::Sim::EvaluatePlacement(Grid, HeightField, Footprint, MaxHeightDeltaM));
}

ETFPlacementResult UTFPlacementSubsystem::TryPlace(const FTFBuildingDef& Def, const FIntPoint& CenterCell, int32 QuarterTurns, FTFPlacedBuilding& OutRecord)
{
	const TF::Sim::Footprint Footprint = MakeFootprint(Def, CenterCell, QuarterTurns);
	const ETFPlacementResult Result = Evaluate(Footprint);
	if (Result != ETFPlacementResult::Ok)
	{
		return Result;
	}

	FTFPlacedBuilding Record;
	Record.InstanceId = NextInstanceId++;
	Record.DefId = Def.Id;
	Record.Origin = FIntPoint(Footprint.X, Footprint.Y);
	Record.Size = FIntPoint(Footprint.W, Footprint.H);
	Record.QuarterTurns = ((QuarterTurns % 4) + 4) % 4;

	Grid.Occupy(Footprint, Record.InstanceId);
	Placed.Add(Record);
	OutRecord = Record;

	UE_LOG(LogTerraForge, Log, TEXT("Yapi yerlestirildi: #%d %s @(%d,%d) %dx%d"),
		Record.InstanceId, *Record.DefId.ToString(), Record.Origin.X, Record.Origin.Y, Record.Size.X, Record.Size.Y);
	OnBuildingPlaced.Broadcast(Record);
	return Result;
}

void UTFPlacementSubsystem::ComputeVisualBox(const TF::Sim::Footprint& Footprint, float HeightMeters, FVector& OutCenterCm, FVector& OutSizeCm) const
{
	constexpr double FoundationDepthM = 0.5;
	double MinH = 0.0;
	double MaxH = 0.0;
	TF::Sim::FootprintHeightRange(HeightField, Footprint, MinH, MaxH);
	const double CellM = HeightField.CellSize();
	const double BottomM = MinH - FoundationDepthM;
	const double TopM = MaxH + HeightMeters;

	OutSizeCm = FVector(Footprint.W * CellM * 100.0, Footprint.H * CellM * 100.0, (TopM - BottomM) * 100.0);
	OutCenterCm = OriginCm + FVector(
		(Footprint.X + Footprint.W * 0.5) * CellM * 100.0,
		(Footprint.Y + Footprint.H * 0.5) * CellM * 100.0,
		(BottomM + TopM) * 0.5 * 100.0);
}

ETFPlacementResult UTFPlacementSubsystem::Convert(TF::Sim::PlacementResult Result)
{
	switch (Result)
	{
	case TF::Sim::PlacementResult::Ok: return ETFPlacementResult::Ok;
	case TF::Sim::PlacementResult::OutOfBounds: return ETFPlacementResult::OutOfBounds;
	case TF::Sim::PlacementResult::Overlap: return ETFPlacementResult::Overlap;
	case TF::Sim::PlacementResult::TooSteep: return ETFPlacementResult::TooSteep;
	case TF::Sim::PlacementResult::InvalidSize: return ETFPlacementResult::InvalidSize;
	}
	return ETFPlacementResult::InvalidSize;
}

FString UTFPlacementSubsystem::ResultKey(ETFPlacementResult Result)
{
	switch (Result)
	{
	case ETFPlacementResult::Ok: return TEXT("placement.ok");
	case ETFPlacementResult::OutOfBounds: return TEXT("placement.out_of_bounds");
	case ETFPlacementResult::Overlap: return TEXT("placement.overlap");
	case ETFPlacementResult::TooSteep: return TEXT("placement.too_steep");
	case ETFPlacementResult::InvalidSize: return TEXT("placement.invalid_size");
	case ETFPlacementResult::NotReady: return TEXT("placement.not_ready");
	}
	return TEXT("placement.invalid_size");
}
