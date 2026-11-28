#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TFHUD.generated.h"

/**
 * M0 icin basit metin arayuzu. Tum metinler Content/Data/Localization/<dil>.json'dan gelir (varsayilan: en, D14).
 * Prototype 0.1'de UMG arayuzune gecilecek.
 */
UCLASS()
class TERRAFORGE_API ATFHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

private:
	void DrawLine(const FString& Text, float X, float& Y, const FLinearColor& Color);
};
