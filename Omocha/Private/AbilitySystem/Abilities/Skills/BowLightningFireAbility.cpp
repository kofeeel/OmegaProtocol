// Copyright It's My Turn


#include "AbilitySystem/Abilities/Skills/BowLightningFireAbility.h"
#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "AbilitySystem/OmochaAttributeSet.h"
#include "OmochaGameplayTags.h"
#include "Actor/OmochaProjectile.h"
#include "AbilitySystem/OmochaAbilitySystemLibrary.h"
#include "AbilitySystem/AbilityTasks/OmochaMouseHitTask.h"
UBowLightningFireAbility::UBowLightningFireAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ChargeLevelData.SetNum(4);
}

void UBowLightningFireAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                               const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const int32 ChargeLevel = GetCurrentChargeLevel();
	SpawnProjectileWithChargeLevel(ChargeLevel);

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

int32 UBowLightningFireAbility::GetCurrentChargeLevel() const
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return 0;

	return FMath::RoundToInt(ASC->GetNumericAttribute(UOmochaAttributeSet::GetChargeLevelAttribute()));
}

void UBowLightningFireAbility::SpawnProjectileWithChargeLevel(int32 ChargeLevel)
{
	if (!ChargeLevelData.IsValidIndex(ChargeLevel) || !ChargeLevelData[ChargeLevel].ProjectileClass) return;

	const FBowLightningChargeData& ChargeData = ChargeLevelData[ChargeLevel];

	FireProjectileWithDamageMultiplier(ChargeData.ProjectileClass, ChargeData.DamageMultiplier);
}

void UBowLightningFireAbility::FireProjectileWithDamageMultiplier(TSubclassOf<AOmochaProjectile> InProjectileClass,	float DamageMultiplier)
{
	// if (!InProjectileClass) return;
	// if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;
	//
	// ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	// if (!Character) return;
	//
	// const FVector MuzzleLocation = GetLaunchLocation(Character);
	// //const FVector TargetLocation = GetTargetLocation();
 //    
	// //FVector Direction = (TargetLocation - MuzzleLocation);
	// //Direction.Z = 0.0f;
	// Direction.Normalize();
	//
	// FTransform SpawnTransform;
	// SpawnTransform.SetLocation(MuzzleLocation);
	// SpawnTransform.SetRotation(Direction.Rotation().Quaternion());
	//
	// AOmochaProjectile* Projectile = GetWorld()->SpawnActorDeferred<AOmochaProjectile>(
	// 	InProjectileClass,
	// 	SpawnTransform,
	// 	GetOwningActorFromActorInfo(),
	// 	Cast<APawn>(GetOwningActorFromActorInfo()),
	// 	ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	//
	// FDamageEffectParams DamageParams = MakeDamageEffectParamsFromClassDefaults();
	// DamageParams.BaseDamage *= DamageMultiplier;
	//
	// Projectile->DamageParams = DamageParams;
	// Projectile->FinishSpawning(SpawnTransform);
}
