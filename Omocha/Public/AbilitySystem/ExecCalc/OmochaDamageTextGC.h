// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "OmochaDamageTextGC.generated.h"

class UNiagaraSystem;

UCLASS(Blueprintable, BlueprintType)
class OMOCHA_API UOmochaDamageTextGC : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Damage Text")
	TObjectPtr<UNiagaraSystem> DamageText;

	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;	
};
