// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OmochaInteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UOmochaInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OMOCHA_API IOmochaInteractionInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	TSubclassOf<class UGameplayAbility> GetInteractionAbilityClass() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void ExecuteInteraction(AActor* InteractingActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnBeginOverlap(AActor* OverlappingActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnEndOverlap(AActor* OverlappingActor);
};
