// Copyright It's My Turn


#include "AbilitySystem/Abilities/Skills/RustyHookAbility.h"

#include "Actor/Hook.h"
#include "GameFramework/Character.h"

AHook* URustyHookAbility::SpawnHook(const FVector& HitLocation)
{
	check(HookClass);
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character) return nullptr;

	const FVector MuzzleLocation = GetLaunchLocation(Character);
	FVector Direction = (HitLocation - MuzzleLocation);
	Direction.Z = 0.0f;
	Direction.Normalize();

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(MuzzleLocation);
	SpawnTransform.SetRotation(Direction.Rotation().Quaternion());
	
	AHook* Hook = GetWorld()->SpawnActorDeferred<AHook>(
			HookClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (!IsValid(Hook)) return nullptr;
	
	Hook->DamageParams = MakeDamageEffectParamsFromClassDefaults();
	Hook->ReturnToActor = GetAvatarActorFromActorInfo();
	Hook->TargetLocation = HitLocation;
	Hook->PaddingDistance = Padding;
	Hook->Distance = HookMaxLength;
	Hook->FinishSpawning(SpawnTransform);
	
	return Hook;
}
