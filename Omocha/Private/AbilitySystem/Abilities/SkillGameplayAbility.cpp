#include "AbilitySystem/Abilities/SkillGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/OmochaBuildSystemLibrary.h"
#include "AbilitySystem/AbilityTasks/OmochaMouseHitTask.h"
#include "AbilitySystem/AbilityTasks/WaitForMouseClickTask.h"
#include "Character/OmochaPlayerCharacter.h"
#include "Component/OmochaSkillBuildComponent.h"
#include "Component/OmochaWeaponComponent.h"
#include "DataAsset/SkillBuildData.h"
#include "GameFramework/Character.h"
#include "Player/OmochaPlayerState.h"

FVector FSkillRangeData::GetSkillSize() const
{
	switch (RangeType)
	{
	case ESkillRangeType::Circle:
		return FVector(Radius, Radius, Radius);
	case ESkillRangeType::Cone:
		return FVector(Radius, Radius, Radius);
	case ESkillRangeType::Rectangle:
		return FVector(Radius, Radius, Radius);
	}

	return FVector(Radius, Radius, Radius);
}

USkillGameplayAbility::USkillGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	SkillRangeData.RangeType = ESkillRangeType::Circle;
	SkillRangeData.MaxRange = 1000.0f;
	SkillRangeData.Size = FVector(300.0f, 200.0f, 200.0f);
}

void USkillGameplayAbility::RequestTargetData()
{
	if (bHasProcessedTargetData) {
		return;
	}
	bHasProcessedTargetData = true;

	if (MouseClickTask && IsValid(MouseClickTask)) {
		MouseClickTask->ValidData.RemoveDynamic(this, &USkillGameplayAbility::OnTargetDataReady);
		MouseClickTask->EndTask();
		MouseClickTask = nullptr;
	}

	MouseClickTask = UOmochaMouseHitTask::CreateTargetDataUnderMouse(this);
	if (MouseClickTask) {
		MouseClickTask->ValidData.AddDynamic(this, &USkillGameplayAbility::OnTargetDataReady);
		MouseClickTask->ReadyForActivation();
	}
	else {
		bHasProcessedTargetData = false;
	}
}


FSkillRangeData USkillGameplayAbility::GetSkillRangeData(UAbilitySystemComponent* ASC) const
{
	FSkillRangeData ModifiedRangeData = SkillRangeData;

	UAbilitySystemComponent* TargetASC = ASC;

	if (!TargetASC)
	{
		TargetASC = GetAbilitySystemComponentFromActorInfo();
	}

	if (TargetASC)
	{
		const UOmochaAttributeSet* Attributes = Cast<UOmochaAttributeSet>(
			TargetASC->GetAttributeSet(UOmochaAttributeSet::StaticClass()));

		if (Attributes)
		{
			// 최대 사정거리
			float MaxRangeMultiplier = Attributes->GetMovementSkillRangeMultiplier();
			// 스킬 크기 배율
			float SkillSizeMultiplier = Attributes->GetBuffRadiusMultiplier();

			if (bAffectedByMaxRange)
			{
				ModifiedRangeData.MaxRange *= MaxRangeMultiplier;
			}

			if (bAffectedBySkillSize)
			{
				ModifiedRangeData.Radius *= SkillSizeMultiplier;
				ModifiedRangeData.Size *= SkillSizeMultiplier;
			}
		}
	}

	if(AOmochaPlayerState* PS = Cast<AOmochaPlayerState>(TargetASC->GetOwnerActor()))
	{
		if(UOmochaSkillBuildComponent* BuildComp = PS->GetSkillBuildComponent())
		{
			const float FinalRange = BuildComp->GetModifiedAbilityPropertyValue(
				SkillTag,
				FGameplayTag::RequestGameplayTag(FName("Property.Range")),
				SkillRangeData.MaxRange
			);
			
			ModifiedRangeData.MaxRange = FinalRange;
		}
	}
	return ModifiedRangeData;
}

void USkillGameplayAbility::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (IsActive())
	{
		bShouldEndAttack = true;
		OnInputReleased();
	}
	
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
}

void USkillGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (const UOmochaSkillBuildComponent* BuildComp = GetSkillBuildComponent())
	{
		const float ModifiedMaxRange = BuildComp->GetModifiedAbilityPropertyValue(
			SkillTag,
			FGameplayTag::RequestGameplayTag(FName("Property.Range")),
			SkillRangeData.MaxRange
		);
		SkillRangeData.MaxRange = ModifiedMaxRange;

		const float ModifiedRadius = BuildComp->GetModifiedAbilityPropertyValue(
			SkillTag,
			FGameplayTag::RequestGameplayTag(FName("Property.Range")),
			SkillRangeData.Radius
		);
		SkillRangeData.Radius = ModifiedRadius;
        
	}
	if (HasAuthority(&ActivationInfo))
	{
		UOmochaBuildSystemLibrary::ApplyBuildsForTrigger(GetAbilitySystemComponentFromActorInfo(), GetAbilitySystemComponentFromActorInfo(), EBuildTriggerCondition::OnAbilityActivation, SkillTag);
	}
}

void USkillGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                       bool bReplicateEndAbility, bool bWasCancelled)
{
	if (MouseClickTask && IsValid(MouseClickTask))
	{
		MouseClickTask->EndTask();
		MouseClickTask = nullptr;
	}
	bShouldEndAttack = false;
	bHasProcessedTargetData = false;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

}

void USkillGameplayAbility::OnTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	bHasProcessedTargetData = false;
}

FVector USkillGameplayAbility::GetLaunchLocation(ACharacter* Character)
{
	if (!Character)
	{
		return FVector::ZeroVector;
	}

	if (!SocketTag.IsValid())
	{
		return FVector::ZeroVector;
	}

	AOmochaPlayerCharacter* PlayerCharacter = Cast<AOmochaPlayerCharacter>(Character);
	if (PlayerCharacter && PlayerCharacter->WeaponComponent)
	{
		UStaticMeshComponent* WeaponMesh = PlayerCharacter->WeaponComponent->GetWeaponMeshComponent();
		static const FName MuzzleSocketName = SocketTag.GetTagName();

		if (WeaponMesh && WeaponMesh->GetStaticMesh() && WeaponMesh->DoesSocketExist(MuzzleSocketName))
		{
			return WeaponMesh->GetSocketLocation(MuzzleSocketName);
		}
	}

	const FName SocketName = SocketTag.GetTagName();
	if (!Character->GetMesh()->DoesSocketExist(SocketName))
	{
		return FVector::ZeroVector;
	}

	const FTransform SocketTransform = Character->GetMesh()->GetSocketTransform(SocketName);
	return SocketTransform.GetLocation();
}

UOmochaSkillBuildComponent* USkillGameplayAbility::GetSkillBuildComponent() const
{
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return nullptr;
	}

	const AOmochaPlayerCharacter* PlayerCharacter = Cast<AOmochaPlayerCharacter>(AvatarActor);
	if (!PlayerCharacter)
	{
		return nullptr;
	}

	const AOmochaPlayerState* PS = PlayerCharacter->GetPlayerState<AOmochaPlayerState>();
	if (!PS)
	{
		return nullptr;
	}

	return PS->GetSkillBuildComponent();
}

float USkillGameplayAbility::GetModifiedPropertyValue(const FGameplayTag& PropertyTag, float DefaultValue) const
{
	const UOmochaSkillBuildComponent* BuildComp = GetSkillBuildComponent();
	if (!BuildComp)
	{
		return DefaultValue;
	}
    
	return BuildComp->GetModifiedAbilityPropertyValue(SkillTag, PropertyTag, DefaultValue);
}

bool USkillGameplayAbility::HasCustomBuildLogic(const FGameplayTag& CustomLogicTag) const
{
	const UOmochaSkillBuildComponent* BuildComp = GetSkillBuildComponent();
	if (!BuildComp)
	{
		return false;
	}

	return BuildComp->HasCustomBuildLogic(SkillTag, CustomLogicTag);
}