#pragma once

#include "CoreMinimal.h"
#include "TFDataTypes.generated.h"

// Content/Data/Terrain.json ile eslesir. Birimler metre.
USTRUCT(BlueprintType)
struct TERRAFORGE_API FTFTerrainConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain")
	float SizeXMeters = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain")
	float SizeYMeters = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain")
	float CellSizeMeters = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain")
	float AmplitudeMeters = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain")
	float BaseFrequency = 0.02f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain")
	int32 Octaves = 4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain")
	int32 Seed = 1337;

	// Bir yapinin altindaki zeminde izin verilen en buyuk yukseklik farki.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain")
	float MaxBuildHeightDeltaMeters = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain")
	FLinearColor GroundColor = FLinearColor(0.20f, 0.28f, 0.12f);

	// Arazi yukaridan gorunmuyorsa true yapilir (ucgen sarim yonu teshisi).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain")
	bool bFlipWinding = false;
};

// Content/Data/Buildings.json icindeki her kayit.
USTRUCT(BlueprintType)
struct TERRAFORGE_API FTFBuildingDef
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building")
	FName Id;

	// Content/Data/Localization/<dil>.json anahtari.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building")
	FString NameKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building")
	int32 FootprintX = 4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building")
	int32 FootprintY = 4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building")
	float HeightMeters = 4.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building")
	FLinearColor Color = FLinearColor(0.7f, 0.6f, 0.45f);
};

// Simulasyon tarafindaki yapi kaydi. Gorsel aktorden bagimsizdir; kayit/yukleme bu veriyi kullanacak.
USTRUCT(BlueprintType)
struct TERRAFORGE_API FTFPlacedBuilding
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
	int32 InstanceId = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
	FName DefId;

	// Izgara hucresi cinsinden en kucuk kose ve (donmus) boyut.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
	FIntPoint Origin = FIntPoint::ZeroValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
	FIntPoint Size = FIntPoint(1, 1);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
	int32 QuarterTurns = 0;
};

UENUM(BlueprintType)
enum class ETFPlacementResult : uint8
{
	Ok,
	OutOfBounds,
	Overlap,
	TooSteep,
	InvalidSize,
	NotReady
};
