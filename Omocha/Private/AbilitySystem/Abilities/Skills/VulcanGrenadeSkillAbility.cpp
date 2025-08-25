// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Skills/VulcanGrenadeSkillAbility.h"

#include "OmochaGameplayTags.h"
#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "AbilitySystem/AbilityTasks/OmochaMouseHitTask.h"
#include "Actor/OmochaGrenade.h"
#include "Actor/OmochaProjectile.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


UVulcanGrenadeSkillAbility::UVulcanGrenadeSkillAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	InputGameplayTag = FOmochaGameplayTags::Get().InputTag_Ability_Q;
	FGameplayTagContainer Tags;
	Tags.AddTag(FOmochaGameplayTags::Get().Ability_Omega_Vulcan_Skill_Q);
	SetAssetTags(Tags);
	DamageType = FOmochaGameplayTags::Get().Abilities_Type_None;

	bHasBlueprintActivate = true;
	bHasBlueprintActivateFromEvent = true;
}

void UVulcanGrenadeSkillAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                 const FGameplayAbilityActorInfo* ActorInfo,
                                                 const FGameplayAbilityActivationInfo ActivationInfo,
                                                 const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UVulcanGrenadeSkillAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo,
                                            bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UVulcanGrenadeSkillAbility::FireProjectileAtTarget(const FVector& TargetLocation)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority() || !ProjectileClass)
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return;
	}

	const FVector CharacterLocation = Character->GetActorLocation();
	const FVector LaunchLocation = GetLaunchLocation(Character);

	if (bIsHowitzer)
	{
		const AOmochaGrenade* DefaultGrenade = Cast<AOmochaGrenade>(ProjectileClass->GetDefaultObject());
		if (!DefaultGrenade || !DefaultGrenade->ProjectileMovementComponent)
		{
			return;
		}

		const float ProjectileSpeed = DefaultGrenade->ProjectileMovementComponent->InitialSpeed;

		FVector LaunchVelocity;
		UGameplayStatics::FSuggestProjectileVelocityParameters Params =
			UGameplayStatics::FSuggestProjectileVelocityParameters(
				GetWorld(),
				LaunchLocation,
				TargetLocation,
				ProjectileSpeed
			);
		Params.bDrawDebug = false;
		Params.bFavorHighArc = false;
		Params.CollisionRadius = 0;
		Params.TraceOption = ESuggestProjVelocityTraceOption::DoNotTrace;

		if (UGameplayStatics::SuggestProjectileVelocity(Params, LaunchVelocity))
		{
			SpawnProjectileWithVelocity(LaunchLocation, LaunchVelocity);
		}
	}
	else
	{
		FRotator LaunchRotation = (TargetLocation - LaunchLocation).Rotation();

		LaunchRotation.Pitch = 0.0f;
		FVector Direction = (TargetLocation - LaunchLocation).GetSafeNormal();
		Direction.Z = 0.0f;
		SpawnProjectile(LaunchLocation, LaunchRotation);
		OnMuzzleEffectTriggered(LaunchLocation, Direction);
	}
}

void UVulcanGrenadeSkillAbility::SpawnProjectileWithVelocity(const FVector& LaunchLocation,
                                                             const FVector& LaunchVelocity)
{
	if (!ProjectileClass)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetAvatarActorFromActorInfo();
	SpawnParams.Instigator = Cast<APawn>(GetAvatarActorFromActorInfo());
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FRotator SpawnRotation = LaunchVelocity.Rotation();
	const FTransform SpawnTransform(SpawnRotation.Quaternion(), LaunchLocation);

	AOmochaGrenade* Projectile = GetWorld()->SpawnActorDeferred<AOmochaGrenade>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	if (Projectile)
	{
		Projectile->DamageParams = MakeDamageEffectParamsFromClassDefaults();
		Projectile->SetProCessRadius(SkillRangeData.Radius);

		if (auto* OmochaASC = Cast<UOmochaAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo()))
		{
			Projectile->SetReplicatedRadiusScale(OmochaASC->SkillScale);
		}

		OnMuzzleEffectTriggered(LaunchLocation, LaunchVelocity.GetSafeNormal());

		Projectile->FinishSpawning(SpawnTransform);
	}
}

void UVulcanGrenadeSkillAbility::SpawnProjectile(const FVector& LaunchLocation, const FRotator& LaunchRotation)
{
	if (!ProjectileClass)
	{
		return;
	}

	// Spawn parameters setup
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetAvatarActorFromActorInfo();
	SpawnParams.Instigator = Cast<APawn>(GetAvatarActorFromActorInfo());
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Create spawn transform
	const FTransform SpawnTransform(LaunchRotation.Quaternion(), LaunchLocation);

	// Spawn projectile
	AOmochaProjectile* Projectile = GetWorld()->SpawnActorDeferred<AOmochaProjectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	if (Projectile)
	{
		// Set damage parameters
		Projectile->DamageParams = MakeDamageEffectParamsFromClassDefaults();

		OnMuzzleEffectTriggered(LaunchLocation, LaunchRotation.Vector());

		// Complete spawning
		Projectile->FinishSpawning(SpawnTransform);
	}
}
