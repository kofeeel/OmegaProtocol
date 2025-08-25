// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/OmochaCharacterBase.h"
#include "OmochaBot.generated.h"

UCLASS()
class OMOCHA_API AOmochaBot : public AOmochaCharacterBase
{
	GENERATED_BODY()

public:
	AOmochaBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void Die(const FVector& DeathImpulse = FVector::ZeroVector) override;

	void ReviveBot();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Revive")
	bool bCanRevive = true;

	UPROPERTY()
	FTimerHandle ReviveTimer;
};