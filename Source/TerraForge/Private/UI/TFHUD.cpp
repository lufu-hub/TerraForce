#include "UI/TFHUD.h"

#include "Building/TFPlacementSubsystem.h"
#include "Data/TFDataSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Player/TFPlayerController.h"

void ATFHUD::DrawLine(const FString& Text, float X, float& Y, const FLinearColor& Color)
{
	UFont* Font = GEngine != nullptr ? GEngine->GetMediumFont() : nullptr;
	DrawText(Text, FLinearColor(0.f, 0.f, 0.f, 0.8f), X + 1.f, Y + 1.f, Font);
	DrawText(Text, Color, X, Y, Font);
	Y += 22.f;
}

void ATFHUD::DrawHUD()
{
	Super::DrawHUD();

	const UGameInstance* GameInstance = GetGameInstance();
	const UTFDataSubsystem* Data = GameInstance != nullptr ? GameInstance->GetSubsystem<UTFDataSubsystem>() : nullptr;
	if (Data == nullptr)
	{
		return;
	}
	const ATFPlayerController* PC = Cast<ATFPlayerController>(GetOwningPlayerController());
	const UTFPlacementSubsystem* Placement = GetWorld()->GetSubsystem<UTFPlacementSubsystem>();

	float Y = 16.f;
	const float X = 16.f;
	const FLinearColor White = FLinearColor::White;
	DrawLine(Data->GetText(TEXT("hud.title")), X, Y, FLinearColor(1.f, 0.85f, 0.5f));
	DrawLine(Data->GetText(TEXT("hud.help.camera")), X, Y, White);
	DrawLine(Data->GetText(TEXT("hud.help.build")), X, Y, White);
	if (Placement != nullptr)
	{
		DrawLine(Data->GetText(TEXT("hud.buildings_count")).Replace(TEXT("{0}"), *FString::FromInt(Placement->GetPlacedBuildings().Num())), X, Y, White);
	}

	if (PC == nullptr)
	{
		return;
	}
	if (PC->IsInBuildMode())
	{
		const FTFBuildingDef* Def = Data->FindBuildingDef(PC->GetSelectedBuildingId());
		const FString Name = Def != nullptr ? Data->GetText(Def->NameKey) : PC->GetSelectedBuildingId().ToString();
		DrawLine(Data->GetText(TEXT("hud.mode.build")).Replace(TEXT("{0}"), *Name), X, Y, FLinearColor(0.6f, 1.f, 0.6f));
		const bool bOk = PC->GetLastEvaluation() == ETFPlacementResult::Ok;
		DrawLine(Data->GetText(UTFPlacementSubsystem::ResultKey(PC->GetLastEvaluation())), X, Y,
			bOk ? FLinearColor(0.6f, 1.f, 0.6f) : FLinearColor(1.f, 0.5f, 0.4f));
	}
	if (!PC->GetStatusKey().IsEmpty() && PC->GetStatusAge() < 3.f)
	{
		DrawLine(Data->GetText(PC->GetStatusKey()), X, Y, FLinearColor(1.f, 1.f, 0.6f));
	}
}
