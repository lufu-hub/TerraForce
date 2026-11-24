// Unreal Automation testleri. Editorde veya komut satirinda calisir:
//   Scripts\tf.cmd test   (Automation RunTests TerraForge)
#include "Misc/AutomationTest.h"
#include "Building/TFPlacementSubsystem.h"
#include "Sim/TFHeightField.h"
#include "Sim/TFPlacement.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTFSimPlacementTest, "TerraForge.Sim.Placement",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter)

bool FTFSimPlacementTest::RunTest(const FString& Parameters)
{
	TF::Sim::HeightFieldParams Params;
	Params.CellsX = 20;
	Params.CellsY = 20;
	Params.Amplitude = 0.0;
	TF::Sim::HeightField Flat;
	Flat.Generate(Params);

	TF::Sim::OccupancyGrid Grid;
	Grid.Reset(20, 20);
	const TF::Sim::Footprint F = TF::Sim::MakeCenteredFootprint(10, 10, 4, 4, 0);

	TestTrue(TEXT("Bos zeminde yerlesim gecerli"), TF::Sim::EvaluatePlacement(Grid, Flat, F, 1.0) == TF::Sim::PlacementResult::Ok);
	Grid.Occupy(F, 1);
	TestTrue(TEXT("Cakisma algilanir"), TF::Sim::EvaluatePlacement(Grid, Flat, F, 1.0) == TF::Sim::PlacementResult::Overlap);
	TestTrue(TEXT("Sinir disi algilanir"),
		TF::Sim::EvaluatePlacement(Grid, Flat, TF::Sim::MakeCenteredFootprint(0, 0, 4, 4, 0), 1.0) == TF::Sim::PlacementResult::OutOfBounds);
	TestEqual(TEXT("Sonuc anahtari"), UTFPlacementSubsystem::ResultKey(ETFPlacementResult::TooSteep), FString(TEXT("placement.too_steep")));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
