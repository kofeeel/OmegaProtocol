// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Actor/OmochaEffectActor.h"
#include "ItemTypes.generated.h"

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data", meta = (MultiLine = true))
	FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	int32 Weight = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	TSubclassOf<AOmochaEffectActor> ItemActorClass;
};

USTRUCT(BlueprintType)
struct FLevelDropRate : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drop Rate", meta=(ClampMin="0.0", ClampMax="100.0"))
	float OverallDropChance = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drop Rate")
	float RareWeight = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drop Rate")
	float EpicWeight = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drop Rate")
	float LegendaryWeight = 0.f;
};
