// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "Components/ActorComponent.h"
#include "OmochaCharacterConfigComponent.generated.h"


class UOmochaAbilitySystemComponent;
class AOmochaCharacterBase;
class UOmochaCharacterDataAsset;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OMOCHA_API UOmochaCharacterConfigComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOmochaCharacterConfigComponent();

	UFUNCTION(BlueprintCallable, Category = "Character Config")
	void SetCharacterData(UOmochaCharacterDataAsset* NewCharacterData);

	UFUNCTION(BlueprintCallable, Category = "Character Config")
	void ChangeState(FGameplayTag StateTag);

	UFUNCTION(BlueprintPure, Category = "Character Config")
	FGameplayTag GetCurrentState() const { return CurrentState; }

	UFUNCTION(BlueprintPure, Category = "Character Config")
	FGameplayTag GetCharacterType() const;

	UFUNCTION(BlueprintPure, Category = "Character Config")
	bool IsInOmegaState() const;

	UFUNCTION(BlueprintPure, Category = "Character Config")
	bool IsInNormalState() const;

	UFUNCTION(BlueprintPure, Category = "Character Config")
	bool IsInState(FGameplayTag StateTag) const { return CurrentState.MatchesTagExact(StateTag); }

	void InitializeState(const FGameplayTag& StateTag);

	UFUNCTION(BlueprintPure, Category = "Character Config")
	UOmochaCharacterDataAsset* GetCurrentCharacterData() const { return CurrentCharacterData; }

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentCharacterData, EditDefaultsOnly, BlueprintReadOnly,
		Category = "Character Config")
	UOmochaCharacterDataAsset* CurrentCharacterData;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentState, BlueprintReadOnly, Category = "Character Config")
	FGameplayTag CurrentState;

	UPROPERTY(EditDefaultsOnly, Category = "State Tags")
	FGameplayTag NormalStateTag;

	UPROPERTY(EditDefaultsOnly, Category = "State Tags")
	FGameplayTag OmegaStateTag;

	UPROPERTY(EditDefaultsOnly, Category = "State Tags")
	FGameplayTag DeadStateTag;
	
private:
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> ActiveAbilityHandles;

	void SwapAbilities(FGameplayTag StateTag);
	void SwapVisuals(FGameplayTag StateTag);

	UFUNCTION()
	void OnRep_CurrentState();

	UFUNCTION()
	void OnRep_CurrentCharacterData();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayerTransformEffects(bool bToOmegaState, FVector PlayerLocation);
	
	void ApplyStateScaleAndCapsule(FGameplayTag StateTag);

	void SwapAttributes(FGameplayTag StateTag);
};
