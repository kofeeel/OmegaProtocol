// Copyright It's My Turn


#include "Actor/Item/DropManager.h"
#include "Engine/DataTable.h"
#include "Actor/OmochaEffectActor.h"
#include "Actor/Item/ItemTypes.h"

TSubclassOf<AOmochaEffectActor> UDropManager::GetDropItemClassByLevel(int32 InLevel, UDataTable* LevelDropRateTable,
                                                                      UDataTable* RareItemTable,
                                                                      UDataTable* EpicItemTable,
                                                                      UDataTable* LegendaryItemTable
                                                                      )
{
	if (!LevelDropRateTable)
	{
		return nullptr;
	}
	FLevelDropRate* DropRateData = LevelDropRateTable->FindRow<FLevelDropRate>(
	   FName(*FString::FromInt(InLevel)), TEXT(""));
	if (!DropRateData)
	{
		return nullptr;
	}
	if (FMath::FRandRange(0.f, 100.f) > DropRateData->OverallDropChance)
	{
		return nullptr;
	}

	const float TotalWeight = DropRateData->RareWeight + DropRateData->EpicWeight + DropRateData->LegendaryWeight;
	if (TotalWeight <= 0)
	{
		return nullptr;
	}

	const float RandomRoll = FMath::FRandRange(0.f, TotalWeight);
	UDataTable* SelectedItemTable = nullptr;
	float CurrentWeightSum = DropRateData->RareWeight;

	if (RandomRoll <= CurrentWeightSum)
	{
		SelectedItemTable = RareItemTable;
	}
	else
	{
		CurrentWeightSum += DropRateData->EpicWeight;
		if (RandomRoll <= CurrentWeightSum)
		{
			SelectedItemTable = EpicItemTable;
		}
		else
		{
			SelectedItemTable = LegendaryItemTable;
		}
	}
    
	return GetItemClassFrom(SelectedItemTable);
}

TSubclassOf<AOmochaEffectActor> UDropManager::GetItemClassFrom(UDataTable* ItemTable)
{
	if (!ItemTable)
	{
		return nullptr;
	}
	TArray<FName> RowNames = ItemTable->GetRowNames();
	if (RowNames.Num() == 0)
	{
		return nullptr;
	}
	int32 TotalWeight = 0;
	for (const FName& RowName : RowNames)
	{
		if (FItemData* RowData = ItemTable->FindRow<FItemData>(RowName, TEXT("")))
		{
			TotalWeight += RowData->Weight;
		}
	}
	if (TotalWeight <= 0)
	{
		return nullptr;
	}
	int32 RandomNumber = FMath::RandRange(1, TotalWeight);
	for (const FName& RowName : RowNames)
	{
		if (FItemData* RowData = ItemTable->FindRow<FItemData>(RowName, TEXT("")))
		{
			RandomNumber -= RowData->Weight;
			if (RandomNumber <= 0)
			{
				return RowData->ItemActorClass;
			}
		}
	}
	return nullptr;
}
