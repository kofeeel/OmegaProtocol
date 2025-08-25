// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "EffectAreaData.generated.h"

UENUM(BlueprintType)
enum class EEffectAreaType : uint8
{
    Persistent    UMETA(DisplayName = "Persistent"),    
    OneTime       UMETA(DisplayName = "OneTime"),      
    Consumable    UMETA(DisplayName = "Consumable")     
};

USTRUCT(BlueprintType)
struct FEffectAreaData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EEffectAreaType AreaType = EEffectAreaType::Persistent;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<class UGameplayEffect> GameplayEffect;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float EffectLevel = 1.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "AreaType == EEffectAreaType::OneTime"))
    float CooldownDuration = 5.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayTag GameplayCueTag;
};