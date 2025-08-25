#include "AbilitySystem/Abilities/OmochaHitReactAbility.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "OmochaGameplayTags.h"
#include "Character/OmochaCharacterBase.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilitysystem/OmochaAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UOmochaHitReactAbility::UOmochaHitReactAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	FGameplayTagContainer Tags;
	Tags.AddTag(FOmochaGameplayTags::Get().Effects_HitReact);
	SetAssetTags(Tags);
	
	AbilityTags.AddTag(FOmochaGameplayTags::Get().Effects_HitReact); 
}

void UOmochaHitReactAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayAbilityActivationInfo ActivationInfo,
                                             const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	TObjectPtr<UAbilitySystemComponent> ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		EndAbility(Handle,ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	bool bIsCritical = false;
	if (TriggerEventData && TriggerEventData->ContextHandle.IsValid())
	{
		bIsCritical = UOmochaAbilitySystemLibrary::IsCriticalHit(TriggerEventData->ContextHandle);
	}
	
	TObjectPtr<UAnimMontage> MontageToPlay = bIsCritical && CriticalHitReactMontage ? CriticalHitReactMontage : HitReactMontage;
	TObjectPtr<USoundBase> SoundToPlay = bIsCritical && CriticalHitSound ? CriticalHitSound : HitSound;
	TObjectPtr<UNiagaraSystem> ParticleToPlay = bIsCritical && CriticalHitParticle ? CriticalHitParticle : HitParticle;

	ApplyHitReactEffects(SoundToPlay, ParticleToPlay);

	if (MontageToPlay)
	{
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, MontageToPlay);
		MontageTask->OnCompleted.AddDynamic(this, &UOmochaHitReactAbility::OnMontageEnded);
		MontageTask->OnInterrupted.AddDynamic(this, &UOmochaHitReactAbility::OnMontageEnded);
		MontageTask->OnCancelled.AddDynamic(this, &UOmochaHitReactAbility::OnMontageEnded);
		MontageTask->ReadyForActivation();
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(HitReactTimer, this, &UOmochaHitReactAbility::OnTimerFinished, HitReactDuration, false);
	}
}

void UOmochaHitReactAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo,
                                        const FGameplayAbilityActivationInfo ActivationInfo,
                                        bool bReplicateEndAbility, bool bWasCancelled)
{
	RemoveHitReactEffects();

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(HitReactTimer);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UOmochaHitReactAbility::ApplyHitReactEffects(USoundBase* Sound, UNiagaraSystem* Particle)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->AddLooseGameplayTag(FOmochaGameplayTags::Get().Effects_HitReact);
	}
	
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return;

	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(AvatarActor, Sound, AvatarActor->GetActorLocation());
	}
	
	if (Particle)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(AvatarActor, Particle, AvatarActor->GetActorLocation());
	}

	if (bApplyColorEffect && HitFlashMaterial)
	{
		if (AOmochaCharacterBase* Character = Cast<AOmochaCharacterBase>(AvatarActor))
		{
			const float EffectDuration = HitReactMontage ? HitReactMontage->GetPlayLength() : HitReactDuration;
			Character->MulticastHitReact(HitFlashMaterial, EffectDuration);
		}
	}
}

void UOmochaHitReactAbility::RemoveHitReactEffects()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveLooseGameplayTag(FOmochaGameplayTags::Get().Effects_HitReact);
	}
}

void UOmochaHitReactAbility::OnMontageEnded()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UOmochaHitReactAbility::OnTimerFinished()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}
