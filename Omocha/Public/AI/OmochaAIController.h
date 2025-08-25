// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "OmochaAIController.generated.h"

class UBehaviorTreeComponent;
/**
 * 
 */
UCLASS()
class OMOCHA_API AOmochaAIController : public AAIController
{
	GENERATED_BODY()

public:
	AOmochaAIController();

protected:
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
};
