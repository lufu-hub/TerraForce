#include "World/TFTerrainActor.h"

#include "TerraForge.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "ProceduralMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ATFTerrainActor::ATFTerrainActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("TerrainMesh"));
	SetRootComponent(Mesh);
	Mesh->bUseAsyncCooking = false; // carpisma ilk karede hazir olsun (duman testi icin gerekli)
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(
		TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (MaterialFinder.Succeeded())
	{
		BaseMaterial = MaterialFinder.Object;
	}
}

void ATFTerrainActor::BuildFromConfig(const FTFTerrainConfig& Config)
{
	TF::Sim::HeightFieldParams Params;
	Params.CellSize = Config.CellSizeMeters;
	Params.CellsX = FMath::Max(1, FMath::RoundToInt32(Config.SizeXMeters / Config.CellSizeMeters));
	Params.CellsY = FMath::Max(1, FMath::RoundToInt32(Config.SizeYMeters / Config.CellSizeMeters));
	Params.Amplitude = Config.AmplitudeMeters;
	Params.BaseFrequency = Config.BaseFrequency;
	Params.Octaves = Config.Octaves;
	Params.Seed = static_cast<uint32_t>(Config.Seed);
	HeightField.Generate(Params);

	const int32 VX = HeightField.VertsX();
	const int32 VY = HeightField.VertsY();
	const double CellM = HeightField.CellSize();
	const double CellCm = CellM * 100.0;

	TArray<FVector> Vertices;
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;
	TArray<FLinearColor> Colors;
	TArray<FProcMeshTangent> Tangents;
	TArray<int32> Triangles;
	Vertices.Reserve(VX * VY);
	Normals.Reserve(VX * VY);
	UV0.Reserve(VX * VY);
	Colors.Reserve(VX * VY);
	Tangents.Reserve(VX * VY);
	Triangles.Reserve((VX - 1) * (VY - 1) * 6);

	for (int32 IY = 0; IY < VY; ++IY)
	{
		for (int32 IX = 0; IX < VX; ++IX)
		{
			const double H = HeightField.HeightAtVertex(IX, IY);
			Vertices.Add(FVector(IX * CellCm, IY * CellCm, H * 100.0));

			const double DzDx = (HeightField.HeightAtVertex(IX + 1, IY) - HeightField.HeightAtVertex(IX - 1, IY)) / (2.0 * CellM);
			const double DzDy = (HeightField.HeightAtVertex(IX, IY + 1) - HeightField.HeightAtVertex(IX, IY - 1)) / (2.0 * CellM);
			Normals.Add(FVector(-DzDx, -DzDy, 1.0).GetSafeNormal());
			Tangents.Add(FProcMeshTangent(1.f, 0.f, static_cast<float>(DzDx)));
			UV0.Add(FVector2D(IX * CellM / 4.0, IY * CellM / 4.0));
			Colors.Add(Config.GroundColor);
		}
	}

	// Ust yuzey yukaridan bakildiginda saat yonunde sarilir (Unreal on yuz kurali).
	for (int32 IY = 0; IY < VY - 1; ++IY)
	{
		for (int32 IX = 0; IX < VX - 1; ++IX)
		{
			const int32 I0 = IY * VX + IX;
			const int32 I1 = I0 + 1;
			const int32 I2 = I0 + VX;
			const int32 I3 = I2 + 1;
			if (!Config.bFlipWinding)
			{
				Triangles.Append({ I0, I1, I3, I0, I3, I2 });
			}
			else
			{
				Triangles.Append({ I0, I3, I1, I0, I2, I3 });
			}
		}
	}

	Mesh->ClearAllMeshSections();
	Mesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, Colors, Tangents, /*bCreateCollision*/ true);

	if (BaseMaterial != nullptr)
	{
		UMaterialInstanceDynamic* Material = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		Material->SetVectorParameterValue(TEXT("Color"), Config.GroundColor);
		Mesh->SetMaterial(0, Material);
	}

	UE_LOG(LogTerraForge, Log, TEXT("Arazi olusturuldu: %.0fx%.0f m, %d verteks, %d ucgen"),
		HeightField.SizeX(), HeightField.SizeY(), Vertices.Num(), Triangles.Num() / 3);
}

float ATFTerrainActor::GetGroundZ(float WorldX, float WorldY) const
{
	const FVector Origin = GetActorLocation();
	const double LocalXM = (WorldX - Origin.X) / 100.0;
	const double LocalYM = (WorldY - Origin.Y) / 100.0;
	return static_cast<float>(Origin.Z + HeightField.SampleHeight(LocalXM, LocalYM) * 100.0);
}

FVector ATFTerrainActor::GetWorldCenter() const
{
	const double CX = HeightField.SizeX() * 50.0;
	const double CY = HeightField.SizeY() * 50.0;
	const FVector Origin = GetActorLocation();
	return FVector(Origin.X + CX, Origin.Y + CY, GetGroundZ(static_cast<float>(Origin.X + CX), static_cast<float>(Origin.Y + CY)));
}

FBox2D ATFTerrainActor::GetWorldBounds2D() const
{
	const FVector Origin = GetActorLocation();
	return FBox2D(
		FVector2D(Origin.X, Origin.Y),
		FVector2D(Origin.X + HeightField.SizeX() * 100.0, Origin.Y + HeightField.SizeY() * 100.0));
}
