// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "OmochaLevelUpTypes.h"
#include "GameFramework/PlayerState.h"
#include "OmochaPlayerState.generated.h"

class UGameplayEffect;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelUpChoicesReady, const TArray<FAttributeUpgrade>&, Choices);

class UOmochaSkillBuildComponent;
class UAbilitySystemComponent;
class UAttributeSet;
/**
 * 
 */
UCLASS()
class OMOCHA_API AOmochaPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AOmochaPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return AbilitySystemComponent;
	};

	UAttributeSet* GetAttributeSet() const
	{
		return AttributeSet;
	}

	FORCEINLINE int32 GetPlayerLevel() const
	{
		return Level;
	}

	UOmochaSkillBuildComponent* GetSkillBuildComponent() const
	{
		return SkillBuildComponent;
	}

	// Cinematic Skip Voting
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_VoteToSkipCinematic();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_CancelVoteToSkip();

	UFUNCTION(BlueprintPure)
	bool HasVotedToSkip() const { return bHasVotedToSkip; }

	UFUNCTION(BlueprintPure)
	bool CanVoteToSkip() const { return bCanVoteToSkip; }

	UFUNCTION(BlueprintCallable, Category = "Vote System")
	void SetCanVoteToSkip(bool bCanVote);

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	FDataTableRowHandle EquippedWeaponRow;

	UFUNCTION()
	void OnRep_EquippedWeapon();

	// EXP - LevelUp Attribute Choice
	UPROPERTY(BlueprintAssignable, Category = "Level Up")
	FOnLevelUpChoicesReady OnLevelUpChoiceReady;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level Up")
	TArray<FAttributeUpgrade> AvailableUpgrades;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level Up")
	TSubclassOf<UGameplayEffect> SetAttributeUpgradeGEClass;

	UFUNCTION(BlueprintCallable, Category = "Level Up")
	void TriggerLevelUpChoice();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Level Up")
	void Server_SelectAttributeUpgrade(int32 ChoiceIndex);
	
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UOmochaSkillBuildComponent> SkillBuildComponent;

	virtual void CopyProperties(APlayerState* PS) override;

	// CInematic Skip Voting
	UPROPERTY(ReplicatedUsing = OnRep_HasVotedToSkip)
	bool bHasVotedToSkip = false;

	UPROPERTY(Replicated)
	bool bCanVoteToSkip = false;

	UFUNCTION()
	void OnRep_HasVotedToSkip();

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Level)
	int32 Level = 1;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel)
	{
	};

	UPROPERTY()
	TArray<FAttributeUpgrade> CurrentChoices;

	void ApplyAttributeUpgrade(const FAttributeUpgrade& Upgrade);
};
