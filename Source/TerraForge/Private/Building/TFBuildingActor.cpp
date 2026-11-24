#include "Building/TFBuildingActor.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

ATFBuildingActor::ATFBuildingActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Box"));
	SetRootComponent(Box);
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionResponseToAllChannels(ECR_Block);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeFinder.Succeeded())
	{
		Box->SetStaticMesh(CubeFinder.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(
		TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (MaterialFinder.Succeeded())
	{
		Box->SetMaterial(0, MaterialFinder.Object);
	}
}

void ATFBuildingActor::SetBox(const FVector& CenterCm, const FVector& SizeCm)
{
	// Motor kupu 100 cm ve merkez pivotludur.
	SetActorLocation(CenterCm);
	SetActorScale3D(SizeCm / 100.0);
}

void ATFBuildingActor::SetColor(const FLinearColor& Color)
{
	if (Material == nullptr)
	{
		Material = Box->CreateDynamicMaterialInstance(0);
	}
	if (Material != nullptr)
	{
		Material->SetVectorParameterValue(TEXT("Color"), Color);
	}
}

void ATFBuildingActor::SetGhost(bool bInGhost)
{
	Box->SetCollisionEnabled(bInGhost ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryOnly);
	Box->SetCastShadow(!bInGhost);
}
