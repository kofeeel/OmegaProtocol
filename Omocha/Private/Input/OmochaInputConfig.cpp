// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/OmochaInputConfig.h"

const UInputAction* UOmochaInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag,
	bool bLogNotFound) const
{
	for (const FOmochaInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && InputTag.MatchesTagExact(Action.InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}
	
	return nullptr;
}
