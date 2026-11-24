#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/TFDataTypes.h"
#include "TFDataSubsystem.generated.h"

/**
 * Oyun verisini Content/Data altindaki JSON dosyalarindan yukler.
 * Kod icinde yapi/arazi/metin listesi sabitlenmez; yeni yapi eklemek icin JSON duzenlenir.
 */
UCLASS()
class TERRAFORGE_API UTFDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// Diskten yeniden yukler. Basarili ise true.
	bool ReloadAll();

	const FTFTerrainConfig& GetTerrainConfig() const { return TerrainConfig; }
	const TArray<FTFBuildingDef>& GetBuildingDefs() const { return BuildingDefs; }
	const FTFBuildingDef* FindBuildingDef(FName Id) const;

	// Yerellestirilmis metin. Anahtar yoksa anahtarin kendisi doner (eksik ceviri gorunur kalsin diye).
	FString GetText(const FString& Key) const;

	static FString GetDataDir();

private:
	bool LoadTerrain();
	bool LoadBuildings();
	bool LoadStrings(const FString& Culture);

	FTFTerrainConfig TerrainConfig;
	TArray<FTFBuildingDef> BuildingDefs;
	TMap<FString, FString> Strings;
};
