// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/BuffGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/OmochaAbilitySystemLibrary.h"
#include "Engine/OverlapResult.h"


UBuffGameplayAbility::UBuffGameplayAbility()
{
}

void UBuffGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UBuffGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo,
                                      bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBuffGameplayAbility::GenerateBuff()
{
	if (!BuffEffectClass) {
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
		return;
	}

	TArray<TWeakObjectPtr<UAbilitySystemComponent>> TargetASCs;

	if (bIsAreaOfEffect) {
		if (AActor* AvatarActor = GetAvatarActorFromActorInfo()) {
			TArray<FOverlapResult> OverlapResults;
			GetWorld()->OverlapMultiByObjectType(
				OverlapResults,
				AvatarActor->GetActorLocation(),
				FQuat::Identity,
				FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),
				FCollisionShape::MakeSphere(GetSkillRangeData().Radius)
			);

			for (const FOverlapResult& Result : OverlapResults) {
				AActor* TargetActor = Result.GetActor();
				if (TargetActor && !UOmochaAbilitySystemLibrary::IsNotFriend(AvatarActor, TargetActor)) {
					if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
						TargetActor)) {
						TargetASCs.Add(TargetASC);
					}
				}
			}
		}
	}
	else {
		TargetASCs.Add(GetCurrentActorInfo()->AbilitySystemComponent);
	}

	FGameplayEffectContextHandle EffectContextHandle = MakeEffectContext(
		GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());

	FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
		BuffEffectClass, GetAbilityLevel(), EffectContextHandle);

	if (!SpecHandle.IsValid()) {
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
		return;
	}

	for (const TWeakObjectPtr<UAbilitySystemComponent>& TargetWeakASC : TargetASCs) {
		if (TargetWeakASC.IsValid()) {
			UAbilitySystemComponent* TargetASC = TargetWeakASC.Get();

			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
		}
	}
}
