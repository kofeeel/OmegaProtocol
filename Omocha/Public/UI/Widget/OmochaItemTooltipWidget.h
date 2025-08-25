// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "OmochaUserWidget.h"
#include "Actor/Item/ItemTypes.h"
#include "OmochaItemTooltipWidget.generated.h"

/**
 * 
 */
UCLASS()
class OMOCHA_API UOmochaItemTooltipWidget : public UOmochaUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Tooltip")
	void SetItemData(const FItemData& InItemData);
};
