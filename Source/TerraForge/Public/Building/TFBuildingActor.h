#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TFBuildingActor.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;

/**
 * Yapinin gecici (placeholder) gorseli: kutu. Simulasyon verisi tutmaz; yalnizca InstanceId ile kayda baglanir.
 * Hayalet modunda carpismasi kapalidir ve gecerli/gecersiz rengi gosterir.
 */
UCLASS()
class TERRAFORGE_API ATFBuildingActor : public AActor
{
	GENERATED_BODY()

public:
	ATFBuildingActor();

	void SetBox(const FVector& CenterCm, const FVector& SizeCm);
	void SetColor(const FLinearColor& Color);
	void SetGhost(bool bInGhost);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
	int32 InstanceId = 0;

private:
	UPROPERTY(VisibleAnywhere, Category = "Building")
	TObjectPtr<UStaticMeshComponent> Box;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> Material;
};
