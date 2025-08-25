// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/OmochaAttributeSet.h"
#include "OmochaShieldAttributeSet.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnShieldStatusSignature);

/**
 * 
 */
UCLASS()
class OMOCHA_API UOmochaShieldAttributeSet : public UOmochaAttributeSet
{
	GENERATED_BODY()

public:
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void HandleIncomingDamage(const FEffectProperties& Props) override;

	virtual void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const override;

	FOnShieldStatusSignature OnShieldBroken;

	FOnShieldStatusSignature OnShieldDamaged;

	
};
