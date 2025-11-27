// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/OmochaAbilitySystemComponent.h"

#include "AbilitySystem/OmochaAbilityTypes.h"
#include "AbilitySystem/Abilities/OmochaGameplayAbility.h"
#include "Net/UnrealNetwork.h"

void UOmochaAbilitySystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UOmochaAbilitySystemComponent, CachedSkillTargetLocation);
	DOREPLIFETIME(UOmochaAbilitySystemComponent, SkillScale);
	DOREPLIFETIME(UOmochaAbilitySystemComponent, CurrentInteractable);
}

void UOmochaAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities, const float Level)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : Abilities)
	{
		// 중복 체크
		bool bAlreadyHasAbility = false;	
		for (const FGameplayAbilitySpec& ExistingSpec : GetActivatableAbilities())
		{
			if (ExistingSpec.Ability && ExistingSpec.Ability->GetClass() == AbilityClass)
			{
				bAlreadyHasAbility = true;
				break;
			}
		}
        
		// 중복이 아닐 때만 등록
		if (!bAlreadyHasAbility)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, Level);
			if (const UOmochaGameplayAbility* Ability = Cast<UOmochaGameplayAbility>(AbilitySpec.Ability))
			{
				AbilitySpec.GetDynamicSpecSourceTags().AddTag(Ability->InputGameplayTag);
				GiveAbility(AbilitySpec);
			}
		}
	}
}

void UOmochaAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UOmochaAbilitySystemComponent::ClientEffectApplied);
}

void UOmochaAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	FScopedAbilityListLock ActiveScopLoc(*this);

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (AbilitySpec.IsActive())
			{
				TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
				const FGameplayAbilityActivationInfo& ActivationInfo = Instances.Last()->GetCurrentActivationInfoRef();
				FPredictionKey OriginalPredictionKey = ActivationInfo.GetActivationPredictionKey();
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, OriginalPredictionKey);
			}
		}
	}
}

void UOmochaAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	FScopedAbilityListLock ActiveScopLoc(*this);
	
	for (FGameplayAbilitySpec& AbilitySpec: GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
				//AbilitySpecInputPressed(AbilitySpec);
			}
		}
	}
}

void UOmochaAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	FScopedAbilityListLock ActiveScopLoc(*this);
	
	for (FGameplayAbilitySpec& AbilitySpec: GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag) && AbilitySpec.IsActive())
		{
			AbilitySpecInputReleased(AbilitySpec);
			TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
			if (Instances.IsEmpty()) return;
			
			const FGameplayAbilityActivationInfo& ActivationInfo = Instances.Last()->GetCurrentActivationInfoRef();
			FPredictionKey OriginalPredictionKey = ActivationInfo.GetActivationPredictionKey();
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, OriginalPredictionKey);
		}
	}
}

void UOmochaAbilitySystemComponent::ActivateAbilityByTag(const FGameplayTag& Tag)
{
	if (!Tag.IsValid()) return;

	FScopedAbilityListLock ActiveScopLoc(*this);
	for (FGameplayAbilitySpec& AbilitySpec: GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(Tag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UOmochaAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
}

void UOmochaAbilitySystemComponent::BroadcastShieldHealthChanged(const FOnAttributeChangeData& Data) const
{
	ShieldHealthChangedDelegate.Broadcast(Data.NewValue);
	Client_BroadcastShieldHealth(Data.NewValue);
}

void UOmochaAbilitySystemComponent::BroadcastShieldMaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	ShieldMaxHealthChangedDelegate.Broadcast(Data.NewValue);
	Client_BroadcastShieldMaxHealth(Data.NewValue);
}

void UOmochaAbilitySystemComponent::Client_BroadcastShieldHealth_Implementation(const float NewValue) const
{
	ShieldHealthChangedDelegate.Broadcast(NewValue);
}

void UOmochaAbilitySystemComponent::Client_BroadcastShieldMaxHealth_Implementation(const float NewValue) const
{
	ShieldMaxHealthChangedDelegate.Broadcast(NewValue);
}

void UOmochaAbilitySystemComponent::Client_BroadcastAbilityInfos_Implementation(
	const TArray<FGameplayAbilitySpec>& AbilitySpecs)
{
	AbilitiesSpecsGivenDelegate.Broadcast(AbilitySpecs);
}

void UOmochaAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                       const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
	
	// if (const FOmochaGameplayEffectContext* OmochaContext = static_cast<const FOmochaGameplayEffectContext*>(EffectSpec.GetContext().Get()))
	// {
	// 	bool bIsCrit = OmochaContext->IsCriticalHit();
	// 	FVector Knockback = OmochaContext->GetKnockbackForce();
	// 	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, 
	// 		FString::Printf(TEXT("[Client Recv] Critical: %s, Knockback: %s"), 
	// 		bIsCrit ? TEXT("TRUE") : TEXT("FALSE"), *Knockback.ToString()));
 //            
	// 	UE_LOG(LogTemp, Warning, TEXT("[Client Recv] Critical: %d"), bIsCrit);
	// }
}
