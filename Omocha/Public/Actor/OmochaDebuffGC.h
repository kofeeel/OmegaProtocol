// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "OmochaDebuffGC.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class USoundBase;
class UAnimMontage;

UCLASS(Blueprintable, BlueprintType)
class OMOCHA_API AOmochaDebuffGC : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:
	AOmochaDebuffGC();
	
	virtual void HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters) override;

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UNiagaraSystem> DebuffVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<USoundBase> DebuffSFX;
		
	// Map for tracking Debuff 
	UPROPERTY()
	TMap<TObjectPtr<AActor>, TObjectPtr<UNiagaraComponent>> ActiveVFXMap;
};
