// Motor bagimsiz simulasyon cekirdegi testleri. Unreal gerektirmez.
// Calistirma: cmake -S Tests/SimTests -B build/simtests && cmake --build build/simtests && ./build/simtests/tf_sim_tests
#include "Sim/TFHeightField.h"
#include "Sim/TFPlacement.h"

#include <cstdio>
#include <cmath>

static int GFailures = 0;
static int GChecks = 0;

#define TF_EXPECT(Cond)                                                        \
	do                                                                         \
	{                                                                          \
		++GChecks;                                                             \
		if (!(Cond))                                                           \
		{                                                                      \
			++GFailures;                                                       \
			std::printf("FAIL %s:%d  %s\n", __FILE__, __LINE__, #Cond);        \
		}                                                                      \
	} while (0)

using namespace TF::Sim;

static HeightField MakeFlat(int Cells)
{
	HeightFieldParams P;
	P.CellsX = Cells;
	P.CellsY = Cells;
	P.Amplitude = 0.0;
	HeightField F;
	F.Generate(P);
	return F;
}

static void TestNoiseDeterministicAndBounded()
{
	for (int I = 0; I < 2000; ++I)
	{
		const double X = I * 0.37;
		const double Y = I * 0.11;
		const double A = FractalNoise2D(X, Y, 4, 42u);
		const double B = FractalNoise2D(X, Y, 4, 42u);
		TF_EXPECT(A == B);
		TF_EXPECT(A >= -1.0 && A <= 1.0);
	}
	TF_EXPECT(FractalNoise2D(3.3, 7.7, 4, 1u) != FractalNoise2D(3.3, 7.7, 4, 2u));
}

static void TestHeightFieldGeneration()
{
	HeightFieldParams P; // 100 x 100 m = 10 donum
	HeightField F;
	F.Generate(P);
	TF_EXPECT(F.VertsX() == 101 && F.VertsY() == 101);
	TF_EXPECT(std::fabs(F.SizeX() - 100.0) < 1e-9);
	double MinH = 1e9;
	double MaxH = -1e9;
	for (int Y = 0; Y < F.VertsY(); ++Y)
	{
		for (int X = 0; X < F.VertsX(); ++X)
		{
			MinH = std::fmin(MinH, F.HeightAtVertex(X, Y));
			MaxH = std::fmax(MaxH, F.HeightAtVertex(X, Y));
		}
	}
	TF_EXPECT(MaxH <= P.Amplitude + 1e-9 && MinH >= -P.Amplitude - 1e-9);
	TF_EXPECT(MaxH - MinH > 0.1); // duz olmamali
	// Bilinear ornek koselerde verteks degerine esit olmali.
	TF_EXPECT(std::fabs(F.SampleHeight(10.0, 20.0) - F.HeightAtVertex(10, 20)) < 1e-9);
	// Sinir disi ornekleme cokmemeli ve kenara sabitlenmeli.
	TF_EXPECT(std::fabs(F.SampleHeight(-50.0, -50.0) - F.HeightAtVertex(0, 0)) < 1e-9);
	TF_EXPECT(std::fabs(F.SampleHeight(500.0, 500.0) - F.HeightAtVertex(100, 100)) < 1e-9);
}

static void TestFootprintRotation()
{
	const Footprint A = MakeCenteredFootprint(50, 50, 6, 8, 0);
	TF_EXPECT(A.W == 6 && A.H == 8 && A.X == 47 && A.Y == 46);
	const Footprint B = MakeCenteredFootprint(50, 50, 6, 8, 1);
	TF_EXPECT(B.W == 8 && B.H == 6);
	const Footprint C = MakeCenteredFootprint(50, 50, 6, 8, -1);
	TF_EXPECT(C.W == 8 && C.H == 6);
	const Footprint D = MakeCenteredFootprint(50, 50, 6, 8, 2);
	TF_EXPECT(D.W == 6 && D.H == 8);
}

static void TestPlacementRules()
{
	const HeightField Flat = MakeFlat(20);
	OccupancyGrid Grid;
	Grid.Reset(20, 20);

	const Footprint Ok = MakeCenteredFootprint(10, 10, 4, 4, 0);
	TF_EXPECT(EvaluatePlacement(Grid, Flat, Ok, 1.0) == PlacementResult::Ok);
	Grid.Occupy(Ok, 1);
	TF_EXPECT(Grid.OwnerAt(10, 10) == 1);
	TF_EXPECT(EvaluatePlacement(Grid, Flat, Ok, 1.0) == PlacementResult::Overlap);

	// Kismi cakisma
	TF_EXPECT(EvaluatePlacement(Grid, Flat, MakeCenteredFootprint(12, 12, 4, 4, 0), 1.0) == PlacementResult::Overlap);
	// Bitisik ama cakismayan
	TF_EXPECT(EvaluatePlacement(Grid, Flat, MakeCenteredFootprint(14, 10, 4, 4, 0), 1.0) == PlacementResult::Ok);

	// Sinirlar
	TF_EXPECT(EvaluatePlacement(Grid, Flat, MakeCenteredFootprint(0, 0, 4, 4, 0), 1.0) == PlacementResult::OutOfBounds);
	TF_EXPECT(EvaluatePlacement(Grid, Flat, MakeCenteredFootprint(19, 19, 4, 4, 0), 1.0) == PlacementResult::OutOfBounds);
	TF_EXPECT(EvaluatePlacement(Grid, Flat, Footprint{16, 16, 4, 4}, 1.0) == PlacementResult::Ok);
	TF_EXPECT(EvaluatePlacement(Grid, Flat, Footprint{0, 0, 0, 4}, 1.0) == PlacementResult::InvalidSize);

	// Serbest birakma
	TF_EXPECT(Grid.Release(1) == 16);
	TF_EXPECT(EvaluatePlacement(Grid, Flat, Ok, 1.0) == PlacementResult::Ok);
}

static void TestSteepness()
{
	HeightFieldParams P;
	P.CellsX = 40;
	P.CellsY = 40;
	P.Amplitude = 30.0; // asiri engebeli
	P.BaseFrequency = 0.2;
	HeightField Rough;
	Rough.Generate(P);
	OccupancyGrid Grid;
	Grid.Reset(40, 40);
	const Footprint F = MakeCenteredFootprint(20, 20, 6, 6, 0);
	TF_EXPECT(EvaluatePlacement(Grid, Rough, F, 0.5) == PlacementResult::TooSteep);
	TF_EXPECT(EvaluatePlacement(Grid, Rough, F, 1000.0) == PlacementResult::Ok);
}

int main()
{
	TestNoiseDeterministicAndBounded();
	TestHeightFieldGeneration();
	TestFootprintRotation();
	TestPlacementRules();
	TestSteepness();
	std::printf("%d/%d kontrol basarili\n", GChecks - GFailures, GChecks);
	return GFailures == 0 ? 0 : 1;
}
