#include "AbilitySystem/Abilities/Skills/OmegaStealAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "OmochaGameplayTags.h"
#include "AbilitySystem/AbilityTasks/OmochaMouseHitTask.h"
#include "Actor/OmochaTransformProjectile.h"
#include "Character/OmochaPlayerCharacter.h"
#include "Component/OmochaCharacterConfigComponent.h"

UOmegaStealAbility::UOmegaStealAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	InputGameplayTag = FOmochaGameplayTags::Get().InputTag_Ability_Spacebar;

	FGameplayTagContainer Tags;
	Tags.AddTag(FOmochaGameplayTags::Get().Ability_Normal_Steal);
	SetAssetTags(Tags);
	DamageType = FOmochaGameplayTags::Get().Abilities_Type_None;

	bHasBlueprintActivate = true;
	bHasBlueprintActivateFromEvent = true;
}

bool UOmegaStealAbility::CheckActivationConditions() const
{
	if (!GetOwningActorFromActorInfo()) {
		return false;
	}

	AOmochaPlayerCharacter* PlayerCharacter = Cast<AOmochaPlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!PlayerCharacter) {
		return false;
	}

	UOmochaCharacterConfigComponent* ConfigComp = PlayerCharacter->FindComponentByClass<
		UOmochaCharacterConfigComponent>();
	if (!ConfigComp) {
		return false;
	}

	if (!ConfigComp->IsInNormalState()) {
		return false;
	}

	return true;
}

void UOmegaStealAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
		GetAvatarActorFromActorInfo());
	if (ASC) {
		FGameplayTag CooldownTag = FOmochaGameplayTags::Get().CoolDown_Normal_Steal;
		int32 CooldownCount = ASC->GetTagCount(CooldownTag);
		
		if (CooldownCount > 0) {
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
	}

	if (!CheckActivationConditions()) {
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	bHasProcessedTargetData = false;
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	RequestStealTargetData();
}

void UOmegaStealAbility::RequestStealTargetData()
{
	if (CurrentTargetDataTask && IsValid(CurrentTargetDataTask)) {
		CurrentTargetDataTask->ValidData.RemoveDynamic(this, &UOmegaStealAbility::OnStealTargetDataReady);
		CurrentTargetDataTask->EndTask();
		CurrentTargetDataTask = nullptr;
	}

	CurrentTargetDataTask = UOmochaMouseHitTask::CreateTargetDataUnderMouseVisibility(this);
	if (CurrentTargetDataTask) {
		CurrentTargetDataTask->ValidData.AddDynamic(this, &UOmegaStealAbility::OnStealTargetDataReady);
		CurrentTargetDataTask->ReadyForActivation();
	}
}

void UOmegaStealAbility::OnStealTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (bHasProcessedTargetData)
	{
		return;
	}
	bHasProcessedTargetData = true;

	AActor* TargetActor = nullptr;
	FVector TargetLocation = FVector::ZeroVector;

	if (TargetDataHandle.Num() > 0)
	{
		const FHitResult* HitResult = TargetDataHandle.Get(0)->GetHitResult();
		if (HitResult && HitResult->bBlockingHit)
		{
			TargetActor = HitResult->GetActor();
			TargetLocation = HitResult->ImpactPoint;
		}
	}

	if (!IsValidStealTarget(TargetActor))
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
		return;
	}
	
	if (AOmochaPlayerCharacter* TargetCharacter = Cast<AOmochaPlayerCharacter>(TargetActor))
	{
		if (TargetCharacter->GetAbilitySystemComponent()->HasMatchingGameplayTag(FOmochaGameplayTags::Get().State_RevivingWithOmega))
		{
			TargetCharacter->CancelReviveSequence();
		}
		else
		{
			TargetCharacter->StripOmegaState_Server();
		}
	}

	if (CurrentTargetDataTask && IsValid(CurrentTargetDataTask))
	{
		CurrentTargetDataTask->ValidData.RemoveDynamic(this, &UOmegaStealAbility::OnTargetDataReady);
		CurrentTargetDataTask->EndTask();
		CurrentTargetDataTask = nullptr;
	}

	StartStealSequence(TargetLocation, TargetActor);
}

void UOmegaStealAbility::SpawnStealProjectile(AActor* TargetActor)
{
	if (!TargetActor || !TransformProjectileClass) {
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
		return;
	}

	UWorld* World = GetWorld();
	if (!World) {
		return;
	}

	AActor* Instigator = GetAvatarActorFromActorInfo();
	if (!Instigator) {
		return;
	}

	AOmochaTransformProjectile* StealProjectile =
		AOmochaTransformProjectile::CreateStealProjectile(
			World, TargetActor, Instigator, TransformProjectileClass);

	if (StealProjectile) {
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
	}
	else {
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
	}
}

bool UOmegaStealAbility::IsValidStealTarget(AActor* TargetActor) const
{
	if (!TargetActor || !IsValid(TargetActor)) {
		return false;
	}

	AOmochaPlayerCharacter* TargetCharacter = Cast<AOmochaPlayerCharacter>(TargetActor);
	if (!TargetCharacter) {
		return false;
	}

	UOmochaCharacterConfigComponent* ConfigComp =
		TargetCharacter->FindComponentByClass<UOmochaCharacterConfigComponent>();
	if (!ConfigComp) {
		return false;
	}

	const bool bIsOmega = ConfigComp->IsInOmegaState();
	const bool bIsReviving = TargetCharacter->GetAbilitySystemComponent()->HasMatchingGameplayTag(FOmochaGameplayTags::Get().State_RevivingWithOmega);

	return bIsOmega || bIsReviving;
}

void UOmegaStealAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                                    bool bWasCancelled)
{
	if (CurrentTargetDataTask && IsValid(CurrentTargetDataTask)) {
		CurrentTargetDataTask->EndTask();
		CurrentTargetDataTask = nullptr;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
