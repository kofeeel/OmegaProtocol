// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DropManager.generated.h"

class AOmochaEffectActor;
class UDataTable;
/**
 * 
 */
UCLASS()
class OMOCHA_API UDropManager : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "DropManager")
	static TSubclassOf<AOmochaEffectActor> GetDropItemClassByLevel(
		int32 InLevel,
		UDataTable* LevelDropRateTable,
		UDataTable* RareItemTable,
		UDataTable* EpicItemTable,
		UDataTable* LegendaryItemTable
		);

private:
	static TSubclassOf<AOmochaEffectActor> GetItemClassFrom(UDataTable* ItemTable);
};
