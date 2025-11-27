// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "OmochaCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class OMOCHA_API UOmochaCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec)
	void TestContextSize();
	
};
