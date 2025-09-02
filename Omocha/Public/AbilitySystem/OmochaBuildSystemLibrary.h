// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OmochaBuildSystemLibrary.generated.h"

struct FEffectProperties;
class UAbilitySystemComponent;
enum class EBuildTriggerCondition : uint8;
/**
 * 
 */
UCLASS()
class OMOCHA_API UOmochaBuildSystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "BuildSystem")
	static void ApplyBuildsForTrigger(UAbilitySystemComponent* ASC, UAbilitySystemComponent* TargetASC,
	                                  EBuildTriggerCondition Condition,
	                                  const FGameplayTag& AbilityTag = FGameplayTag());
};
