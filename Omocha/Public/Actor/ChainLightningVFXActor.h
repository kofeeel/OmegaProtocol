// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChainLightningVFXActor.generated.h"

UCLASS()
class OMOCHA_API AChainLightningVFXActor : public AActor
{
	GENERATED_BODY()

public:
	AChainLightningVFXActor();

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_TargetChain, Category = "Chain Lightning")
	TArray<TObjectPtr<AActor>> TargetChain;

	UFUNCTION()
	void OnRep_TargetChain();
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;



	UFUNCTION(BlueprintImplementableEvent, Category = "Chain Lightning")
	void PlayVFX();
};
