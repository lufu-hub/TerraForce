#include "Data/TFDataSubsystem.h"

#include "TerraForge.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace
{
	bool LoadJsonFile(const FString& RelativePath, TSharedPtr<FJsonObject>& OutObject)
	{
		const FString FullPath = FPaths::Combine(UTFDataSubsystem::GetDataDir(), RelativePath);
		FString Text;
		if (!FFileHelper::LoadFileToString(Text, *FullPath))
		{
			UE_LOG(LogTerraForge, Error, TEXT("Veri dosyasi okunamadi: %s"), *FullPath);
			return false;
		}
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Text);
		if (!FJsonSerializer::Deserialize(Reader, OutObject) || !OutObject.IsValid())
		{
			UE_LOG(LogTerraForge, Error, TEXT("JSON cozumlenemedi: %s"), *FullPath);
			return false;
		}
		return true;
	}

	float GetFloat(const TSharedPtr<FJsonObject>& Obj, const TCHAR* Field, float Default)
	{
		double Value = 0.0;
		return Obj->TryGetNumberField(Field, Value) ? static_cast<float>(Value) : Default;
	}

	int32 GetInt(const TSharedPtr<FJsonObject>& Obj, const TCHAR* Field, int32 Default)
	{
		double Value = 0.0;
		return Obj->TryGetNumberField(Field, Value) ? FMath::RoundToInt32(Value) : Default;
	}

	FLinearColor GetColor(const TSharedPtr<FJsonObject>& Obj, const TCHAR* Field, const FLinearColor& Default)
	{
		const TArray<TSharedPtr<FJsonValue>>* Values = nullptr;
		if (!Obj->TryGetArrayField(Field, Values) || Values == nullptr || Values->Num() < 3)
		{
			return Default;
		}
		return FLinearColor(
			static_cast<float>((*Values)[0]->AsNumber()),
			static_cast<float>((*Values)[1]->AsNumber()),
			static_cast<float>((*Values)[2]->AsNumber()),
			1.f);
	}
}

FString UTFDataSubsystem::GetDataDir()
{
	return FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Data"));
}

void UTFDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ReloadAll();
}

bool UTFDataSubsystem::ReloadAll()
{
	const bool bTerrain = LoadTerrain();
	const bool bBuildings = LoadBuildings();
	const bool bStrings = LoadStrings(TEXT("tr"));
	UE_LOG(LogTerraForge, Log, TEXT("Veri yuklendi: arazi=%d yapilar=%d (%d tur) metinler=%d (%d anahtar)"),
		bTerrain, bBuildings, BuildingDefs.Num(), bStrings, Strings.Num());
	return bTerrain && bBuildings && bStrings;
}

bool UTFDataSubsystem::LoadTerrain()
{
	TerrainConfig = FTFTerrainConfig();
	TSharedPtr<FJsonObject> Root;
	if (!LoadJsonFile(TEXT("Terrain.json"), Root))
	{
		UE_LOG(LogTerraForge, Warning, TEXT("Terrain.json yok; varsayilan arazi ayarlari kullaniliyor."));
		return false;
	}
	FTFTerrainConfig& C = TerrainConfig;
	C.SizeXMeters = GetFloat(Root, TEXT("sizeXMeters"), C.SizeXMeters);
	C.SizeYMeters = GetFloat(Root, TEXT("sizeYMeters"), C.SizeYMeters);
	C.CellSizeMeters = FMath::Max(0.25f, GetFloat(Root, TEXT("cellSizeMeters"), C.CellSizeMeters));
	C.AmplitudeMeters = GetFloat(Root, TEXT("amplitudeMeters"), C.AmplitudeMeters);
	C.BaseFrequency = GetFloat(Root, TEXT("baseFrequency"), C.BaseFrequency);
	C.Octaves = FMath::Clamp(GetInt(Root, TEXT("octaves"), C.Octaves), 1, 8);
	C.Seed = GetInt(Root, TEXT("seed"), C.Seed);
	C.MaxBuildHeightDeltaMeters = GetFloat(Root, TEXT("maxBuildHeightDeltaMeters"), C.MaxBuildHeightDeltaMeters);
	C.GroundColor = GetColor(Root, TEXT("groundColor"), C.GroundColor);
	bool bFlip = false;
	if (Root->TryGetBoolField(TEXT("flipWinding"), bFlip))
	{
		C.bFlipWinding = bFlip;
	}
	return true;
}

bool UTFDataSubsystem::LoadBuildings()
{
	BuildingDefs.Reset();
	TSharedPtr<FJsonObject> Root;
	if (!LoadJsonFile(TEXT("Buildings.json"), Root))
	{
		return false;
	}
	const TArray<TSharedPtr<FJsonValue>>* Entries = nullptr;
	if (!Root->TryGetArrayField(TEXT("buildings"), Entries) || Entries == nullptr)
	{
		UE_LOG(LogTerraForge, Error, TEXT("Buildings.json: 'buildings' dizisi bulunamadi."));
		return false;
	}
	for (const TSharedPtr<FJsonValue>& Entry : *Entries)
	{
		const TSharedPtr<FJsonObject> Obj = Entry.IsValid() ? Entry->AsObject() : nullptr;
		if (!Obj.IsValid())
		{
			continue;
		}
		FString IdString;
		if (!Obj->TryGetStringField(TEXT("id"), IdString) || IdString.IsEmpty())
		{
			UE_LOG(LogTerraForge, Warning, TEXT("Buildings.json: 'id' alani olmayan kayit atlandi."));
			continue;
		}
		FTFBuildingDef Def;
		Def.Id = FName(*IdString);
		Obj->TryGetStringField(TEXT("nameKey"), Def.NameKey);
		Def.FootprintX = FMath::Max(1, GetInt(Obj, TEXT("footprintX"), Def.FootprintX));
		Def.FootprintY = FMath::Max(1, GetInt(Obj, TEXT("footprintY"), Def.FootprintY));
		Def.HeightMeters = FMath::Max(0.5f, GetFloat(Obj, TEXT("heightMeters"), Def.HeightMeters));
		Def.Color = GetColor(Obj, TEXT("color"), Def.Color);
		if (FindBuildingDef(Def.Id) != nullptr)
		{
			UE_LOG(LogTerraForge, Warning, TEXT("Buildings.json: tekrar eden id atlandi: %s"), *IdString);
			continue;
		}
		BuildingDefs.Add(Def);
	}
	return BuildingDefs.Num() > 0;
}

bool UTFDataSubsystem::LoadStrings(const FString& Culture)
{
	Strings.Reset();
	TSharedPtr<FJsonObject> Root;
	if (!LoadJsonFile(FPaths::Combine(TEXT("Localization"), Culture + TEXT(".json")), Root))
	{
		return false;
	}
	const TSharedPtr<FJsonObject>* StringsObject = nullptr;
	if (!Root->TryGetObjectField(TEXT("strings"), StringsObject) || StringsObject == nullptr || !StringsObject->IsValid())
	{
		return false;
	}
	for (const TPair<FString, TSharedPtr<FJsonValue>>& Pair : (*StringsObject)->Values)
	{
		if (Pair.Value.IsValid())
		{
			Strings.Add(Pair.Key, Pair.Value->AsString());
		}
	}
	return true;
}

const FTFBuildingDef* UTFDataSubsystem::FindBuildingDef(FName Id) const
{
	return BuildingDefs.FindByPredicate([Id](const FTFBuildingDef& Def) { return Def.Id == Id; });
}

FString UTFDataSubsystem::GetText(const FString& Key) const
{
	const FString* Found = Strings.Find(Key);
	return Found != nullptr ? *Found : Key;
}
