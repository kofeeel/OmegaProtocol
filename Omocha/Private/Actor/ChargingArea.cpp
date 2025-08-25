// Copyright It's My Turn


#include "Actor/ChargingArea.h"

AChargingArea::AChargingArea()
{
	PrimaryActorTick.bCanEverTick = false;

}

FVector2D AChargingArea::GetAreaSize_Implementation()
{
	return FVector2D::ZeroVector;
}

void AChargingArea::BeginPlay()
{
	Super::BeginPlay();
}


