#include "AbilitySystem/Abilities/SkillGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Character/OmochaPlayerCharacter.h"
#include "Component/OmochaWeaponComponent.h"
#include "GameFramework/Character.h"

FVector FSkillRangeData::GetSkillSize() const
{
	switch (RangeType) {
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


FSkillRangeData USkillGameplayAbility::GetSkillRangeData(UAbilitySystemComponent* ASC) const
{
	FSkillRangeData ModifiedRangeData = SkillRangeData;

	UAbilitySystemComponent* TargetASC = ASC;
	
	if (!TargetASC)
	{
		TargetASC = GetAbilitySystemComponentFromActorInfo();
	}

	if (TargetASC) {
		const UOmochaAttributeSet* Attributes = Cast<UOmochaAttributeSet>(
			TargetASC->GetAttributeSet(UOmochaAttributeSet::StaticClass()));

		if (Attributes)
		{
			// 최대 사정거리
			float MaxRangeMultiplier = Attributes->GetMovementSkillRangeMultiplier();
			// 스킬 크기 배율
			float SkillSizeMultiplier = Attributes->GetBuffRadiusMultiplier();

			if (bAffectedByMaxRange) {
				ModifiedRangeData.MaxRange *= MaxRangeMultiplier;
			}

			if (bAffectedBySkillSize) {
				ModifiedRangeData.Radius *= SkillSizeMultiplier;
				ModifiedRangeData.Size *= SkillSizeMultiplier;
			}
		}
	}

	return ModifiedRangeData;
}

FVector USkillGameplayAbility::GetLaunchLocation(ACharacter* Character)
{
	if (!Character) {
		return FVector::ZeroVector;
	}
	
	if (!SocketTag.IsValid()) {
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
	if (!Character->GetMesh()->DoesSocketExist(SocketName)) {
		return FVector::ZeroVector;
	}

	const FTransform SocketTransform = Character->GetMesh()->GetSocketTransform(SocketName);
	return SocketTransform.GetLocation();
}
