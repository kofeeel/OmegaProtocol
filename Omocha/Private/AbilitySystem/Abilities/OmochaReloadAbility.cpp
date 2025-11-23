// Copyright It's My Turn


#include "AbilitySystem/Abilities/OmochaReloadAbility.h"

#include "OmochaGameplayTags.h"
#include "Character/OmochaPlayerCharacter.h"
#include "Component/OmochaWeaponComponent.h"

UOmochaReloadAbility::UOmochaReloadAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	InputGameplayTag = FOmochaGameplayTags::Get().InputTag_Ability_R;
}

bool UOmochaReloadAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const AOmochaPlayerCharacter* Character = Cast<AOmochaPlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!Character) return false;

	const UOmochaWeaponComponent* WeaponComp = Character->FindComponentByClass<UOmochaWeaponComponent>();
	if (!WeaponComp) return false;

	if (!WeaponComp->bUsesAmmo)
	{
		return false;
	}
	
	if (WeaponComp->IsReloading() || WeaponComp->CurrentAmmo == WeaponComp->MaxAmmo)
	{
		return false;
	}

	return true;
}

void UOmochaReloadAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UOmochaReloadAbility::OnReloadFinished()
{
    if (HasAuthority(&CurrentActivationInfo))
    {
        AOmochaPlayerCharacter* Character = Cast<AOmochaPlayerCharacter>(GetCurrentActorInfo()->AvatarActor.Get());
        if (Character)
        {
            UOmochaWeaponComponent* WeaponComp = Character->FindComponentByClass<UOmochaWeaponComponent>();
            if (WeaponComp)
            {
                WeaponComp->ReloadAmmo();
            }
        }
    }
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}