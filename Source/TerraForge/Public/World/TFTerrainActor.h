#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/TFDataTypes.h"
#include "Sim/TFHeightField.h"
#include "TFTerrainActor.generated.h"

class UProceduralMeshComponent;
class UMaterialInterface;

/**
 * Gorsel arazi. Yukseklik verisinin sahibi saf C++ TF::Sim::HeightField'dir;
 * bu aktor yalnizca ondan mesh ve carpisma uretir.
 * Arazi dunya orijininden +X/+Y yonune uzanir.
 */
UCLASS()
class TERRAFORGE_API ATFTerrainActor : public AActor
{
	GENERATED_BODY()

public:
	ATFTerrainActor();

	void BuildFromConfig(const FTFTerrainConfig& Config);

	const TF::Sim::HeightField& GetHeightField() const { return HeightField; }
	bool IsBuilt() const { return HeightField.IsValid(); }

	// Dunya XY (cm) icin zemin yuksekligi (cm).
	float GetGroundZ(float WorldX, float WorldY) const;
	FVector GetWorldCenter() const;
	FBox2D GetWorldBounds2D() const;

private:
	UPROPERTY(VisibleAnywhere, Category = "Terrain")
	TObjectPtr<UProceduralMeshComponent> Mesh;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> BaseMaterial;

	TF::Sim::HeightField HeightField;
};
