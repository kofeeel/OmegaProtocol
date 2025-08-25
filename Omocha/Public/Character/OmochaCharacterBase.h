// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "Interaction/OmochaCombatInterface.h"
#include "GameFramework/Character.h"
#include "ActiveGameplayEffectHandle.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "OmochaCharacterBase.generated.h"

class UPaperSpriteComponent;
class UWidgetComponent;
class UGameplayEffect;
class UGameplayAbility;
class UAttributeSet;
class UAbilitySystemComponent;
class UNiagaraComponent;
class UTimelineComponent;
struct FOmochaGameplayTags;
struct FOmochaPlayerAttributeData;
struct FOmochaEnemyAttributeData;

UCLASS()
class OMOCHA_API AOmochaCharacterBase : public ACharacter,
                                        public IAbilitySystemInterface,
                                        public IOmochaCombatInterface
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	// Status
	UFUNCTION()
	void OnStunTagChanged(FGameplayTag CallbackTag, int32 NewCount);

	UFUNCTION()
	void OnBurnTagChanged(FGameplayTag CallbackTag, int32 NewCount);

	UFUNCTION()
	void OnShockTagChanged(FGameplayTag CallbackTag, int32 NewCount);

	UFUNCTION(BlueprintPure)
	bool IsStunned() const;

	UFUNCTION(BlueprintPure)
	bool IsBurned() const;

	UFUNCTION(BlueprintPure)
	bool IsShocked() const;

	// Debuff
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debuff Effects")
	TSubclassOf<UGameplayEffect> SpeedReductionEffectClass;

	UPROPERTY()
	FActiveGameplayEffectHandle SpeedReductionEffectHandle;

	UFUNCTION()
	void ApplySpeedReduction(float ReductionPercent);

	UFUNCTION()
	void RemoveSpeedReduction();

	// KnockBack
	virtual void ApplyKnockback_Implementation(const FVector& KnockbackForce) override;
		
	//CombatInterface

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 PlayerLevel = 1;

	//Death
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bDead = false;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeathSignature OnDeathDelegate;

	virtual void DisableCollisionOnDeath();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHandleDeath(const FVector& DeathImpulse);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHandleRebirth();

	virtual void RestoreCollisionOnRebirth();

	//Knockback
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death|Basic",
		meta = (EditCondition = "bShowBasicDeathData"))
	TArray<UAnimMontage*> DeathAnimations;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death|Basic",
		meta = (EditCondition = "bShowBasicDeathData"))
	USoundBase* DeathSound;

	UPROPERTY(EditAnywhere, Category = "Death|Settings")
	bool bShowBasicDeathData = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death")
	float DissolveDuration = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death")
	bool bUseDissolveEffect = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death", meta = (AllowPrivateAccess = "true"))
	float DissolveDelay = 0.5f;
	
	virtual UAnimMontage* GetDeathAnimation() const;
	float GetDeathAnimationDuration() const;
	virtual USoundBase* GetDeathSound() const;

	//Hit
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReacting = false;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UGameplayEffect> MoveSpeedEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UNiagaraSystem> HitReactEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	FVector HitReactOffSet = FVector(0, 0, 50);

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY()
	TArray<UMaterialInterface*> HitReactOriginalMaterials;

	FTimerHandle HitReactTimerHandle;

	// Debuff
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debuff Effects")
	TObjectPtr<UNiagaraComponent> StunDebuffComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debuff Effects")
	TObjectPtr<UNiagaraComponent> BurnDebuffComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debuff Effects")
	TObjectPtr<UNiagaraComponent> ShockDebuffComponent;

	void AddCharacterAbilities();

	virtual void InitAbilityActorInfo()
	{
	};

	//Initialize Default Attributes
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> CombatsAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> PerformanceAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> VitalAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	FName AttributeRowName = TEXT("Default");

	virtual void InitializeAttributes();
	virtual void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UPaperSpriteComponent> MinimapIcon;

public:
	AOmochaCharacterBase();
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; };
	UAttributeSet* GetAttributeSet() const { return AttributeSet; };
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;

	/** Interface **/
	virtual int32 GetPlayerLevel() override { return PlayerLevel; }
	virtual bool IsDead_Implementation() const override;
	virtual void Die(const FVector& DeathImpulse = FVector::ZeroVector) override;
	virtual void Rebirth();
	virtual FOnDeathSignature& GetOnDeathDelegate() override { return OnDeathDelegate; }
	virtual AActor* GetAvatar_Implementation() override;
	/** End Interface **/

	virtual bool IsBeingHitReact_Implementation() const override;
	virtual UNiagaraSystem* GetHitReactEffect_Implementation() const override;
	virtual FVector GetHitReactEffectLocation_Implementation() const override;

	// HealthAttributeChanged Delegate for Health Bar

	UPROPERTY(BlueprintAssignable)
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnHealthChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnHealthChangedSignature OnShieldMaxHealthChanged;

	// Initialize Attributes

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void ApplyAttributesFromRowName(const FName& RowName);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHitReact(UMaterialInterface* HitMaterial, float Duration);

	UFUNCTION()
	void RestoreHealthOnRebirth(float HealthPercentage = 0.5f);

	// Deaath
	UFUNCTION(BlueprintPure, Category = "Death")
	bool GetIsDead() const { return bDead; }

	void SetPlayerLevel(int32 NewLevel) { PlayerLevel = NewLevel; };

private:
	FTimerHandle DissolveTimer;
	TArray<UMaterialInterface*> OriginalMaterials;
	TArray<UMaterialInstanceDynamic*> DynamicMaterials;

	UPROPERTY(EditAnywhere, Category = "Abilites")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	//Death
	virtual void PlayDeathAnimation();
	virtual void PlayDeathSound();
	void RotateToDeathDirection(const FVector& DeathImpulse);
	void DisableCharacterMovement();
	// Death - Dissolve
	float CurrentDissolveValue = 0.0f;
	FTimerHandle DeathAnimationCompleteTimer;
};
