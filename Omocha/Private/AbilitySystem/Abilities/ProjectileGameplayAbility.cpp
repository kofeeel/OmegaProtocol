// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/ProjectileGameplayAbility.h"
#include "Actor/OmochaProjectile.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


void UProjectileGameplayAbility::SpawnProjectile(const FVector& TargetLocation)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) {
		if (IsLocallyControlled()) {
			const FVector MuzzleLocation = GetLaunchLocation(Cast<ACharacter>(GetAvatarActorFromActorInfo()));
			FVector Direction = (TargetLocation - MuzzleLocation);
			OnMuzzleEffectTriggered(MuzzleLocation, Direction);
		}
		return;
	}

	FireProjectileAtTarget(TargetLocation);
}

void UProjectileGameplayAbility::FireProjectileAtTarget(const FVector& TargetLocation)
{
	if (!ProjectileClass) {
		return;
	}

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character) {
		return;
	}

	const FVector MuzzleLocation = GetLaunchLocation(Character);
	FVector Direction = (TargetLocation - MuzzleLocation);
	Direction.Z = 0.0f;
	Direction.Normalize();

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(MuzzleLocation);
	SpawnTransform.SetRotation(Direction.Rotation().Quaternion());

	OnMuzzleEffectTriggered(MuzzleLocation, Direction);

	AOmochaProjectile* Projectile = GetWorld()->SpawnActorDeferred<AOmochaProjectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (Projectile) {
		Projectile->DamageParams = MakeDamageEffectParamsFromClassDefaults();
		Projectile->MaxTravelDistance = MaxRange;
		Projectile->FinishSpawning(SpawnTransform);
	}
}

void UProjectileGameplayAbility::PlayMuzzleEffects(const FVector& Location, const FVector& Direction)
{
	if (MuzzleFlash) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, Location, Direction.Rotation());
	}

	if (FireSound) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, Location);
	}
}
