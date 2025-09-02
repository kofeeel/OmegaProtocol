// Copyright It's My Turn


#include "AbilitySystem/Abilities/Skills/RustyHookAbility.h"

#include "OmochaGameplayTags.h"
#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "Actor/Hook.h"
#include "Component/OmochaSkillBuildComponent.h"
#include "GameFramework/Character.h"
#include "Player/OmochaPlayerState.h"

AHook* URustyHookAbility::SpawnHook(const FVector& HitLocation)
{
	check(HookClass);
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return nullptr;
	}

	float FinalHookMaxLength = HookMaxLength;
	int32 FinalMaxGrabCount = 1;
	bool bCanGrabWalls = false;

	FinalHookMaxLength = GetModifiedPropertyValue(
		FGameplayTag::RequestGameplayTag(FName("Property.Range")),
		HookMaxLength
	);

	bCanGrabWalls = HasCustomBuildLogic(
		FGameplayTag::RequestGameplayTag(FName("Property.Hook.CanHitWall"))
	);

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

	if (!IsValid(Hook))
	{
		return nullptr;
	}

	Hook->DamageParams = MakeDamageEffectParamsFromClassDefaults();
	Hook->ReturnToActor = GetAvatarActorFromActorInfo();
	Hook->TargetLocation = HitLocation;
	Hook->PaddingDistance = Padding;
	Hook->Distance = FinalHookMaxLength;
	// Hook->MaxGrabCount = FinalMaxGrabCount;
	Hook->bCanGrabWallsAndItems = bCanGrabWalls;
	Hook->FinishSpawning(SpawnTransform);

	return Hook;
}
