#include "AbilitySystem/Abilities/Skills/ChainLightningAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_ChainLightning.h"
#include "AbilitySystem/OmochaAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystem/OmochaAbilityTypes.h"
#include "Actor/ChainLightningVFXActor.h"
#include "Actor/Shield.h"
#include "Omocha/Omocha.h"

UChainLightningAbility::UChainLightningAbility()
{
}

void UChainLightningAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayAbilityActivationInfo ActivationInfo,
                                             const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UChainLightningAbility::ExecuteChainLightning(const FVector& TargetLocation)
{
	ACharacter* MyCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!MyCharacter)
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
		return;
	}
	const FVector StartLocation = GetLaunchLocation(MyCharacter);
	FVector StraightTargetLocation = TargetLocation;
	StraightTargetLocation.Z = StartLocation.Z;

	TArray<FHitResult> OutHits;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(MyCharacter);

	ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_ChainTrace);
	if (MyCharacter->ActorHasTag(FName("Player")))
	{
		TraceType = UEngineTypes::ConvertToTraceType(ECC_ChainTrace);
	}
	else if (MyCharacter->ActorHasTag(FName("Enemy")))
	{
		TraceType = UEngineTypes::ConvertToTraceType(ECC_EnemyChainTrace);
	}

	UKismetSystemLibrary::SphereTraceMulti(
		this,
		StartLocation,
		StraightTargetLocation,
		SkillRangeData.Radius,
		TraceType,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		OutHits,
		true
	);
	bool bTargetFoundAndProcessed = false;

	for (const FHitResult& Hit : OutHits)
	{
		AActor* HitActor = Hit.GetActor();

		if (!HitActor || !UOmochaAbilitySystemLibrary::IsNotFriend(MyCharacter, HitActor))
		{
			continue;
		}

		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor))
		{
			FDamageEffectParams DamageParams = MakeDamageEffectParamsFromClassDefaults(HitActor);
			DamageParams.TargetAbilitySystemComponent = TargetASC;
			UOmochaAbilitySystemLibrary::ApplyDamageEffect(DamageParams);

			if (ChainLightningCueTag.IsValid())
			{
				FGameplayCueParameters CueParams;
				CueParams.Location = GetLaunchLocation(MyCharacter);
				CueParams.Normal = (HitActor->GetActorLocation() - CueParams.Location).GetSafeNormal();
				CueParams.NormalizedMagnitude = (HitActor->GetActorLocation() - CueParams.Location).Length();
				GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(ChainLightningCueTag, CueParams);
			}
			if (APawn* ChainTarget = Cast<APawn>(HitActor))
			{
				StartChainLightningTask(HitActor);
			}

			bTargetFoundAndProcessed = true;
			break;
		}
	}


	if (!bTargetFoundAndProcessed)
	{
		if (ChainLightningCueTag.IsValid())
		{
			FGameplayCueParameters CueParams;
			CueParams.Location = MyCharacter->GetActorLocation();
			CueParams.Normal = (StraightTargetLocation - CueParams.Location).GetSafeNormal();
			CueParams.NormalizedMagnitude = (StraightTargetLocation - CueParams.Location).Length();
			GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(ChainLightningCueTag, CueParams);
		}
	}

	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UChainLightningAbility::StartChainLightningTask(AActor* InitialTarget)
{
	ChainLightningTask = UAbilityTask_ChainLightning::CreateChainLightningTask(
		this, InitialTarget, MaxChainCount, SearchRadius);
	ChainLightningTask->OnTargetsDetermined.AddDynamic(this, &UChainLightningAbility::OnChainTargetsDetermined);
	ChainLightningTask->ReadyForActivation();
}

void UChainLightningAbility::OnChainTargetsDetermined(const TArray<AActor*>& TargetChain)
{
	if (!VFXActorClass)
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
		return;
	}

	if (TargetChain.Num() <= 1)
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
		return;
	}

	float CurrentDamage = MakeDamageEffectParamsFromClassDefaults().BaseDamage;

	for (int32 i = 1; i < TargetChain.Num(); ++i)
	{
		AActor* PreviousTarget = TargetChain[i - 1];
		AActor* CurrentTarget = TargetChain[i];

		if (PreviousTarget && CurrentTarget)
		{
			CurrentDamage *= DamageFalloff;

			FDamageEffectParams DamageParams = MakeDamageEffectParamsFromClassDefaults(CurrentTarget);
			DamageParams.BaseDamage = CurrentDamage;
			DamageParams.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
				CurrentTarget);
			if (UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(CurrentTarget))
			{
				UOmochaAbilitySystemLibrary::ApplyDamageEffect(DamageParams);
			}
		}
	}
	const FVector SpawnLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	AChainLightningVFXActor* VFXActor = GetWorld()->SpawnActor<AChainLightningVFXActor>(
		VFXActorClass, SpawnLocation, FRotator::ZeroRotator);

	if (VFXActor)
	{
		VFXActor->TargetChain = TargetChain;

		VFXActor->OnRep_TargetChain();
	}
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UChainLightningAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo,
                                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                                        bool bWasCancelled)
{
	if (ChainLightningTask)
	{
		ChainLightningTask->EndTask();
		ChainLightningTask = nullptr;
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
