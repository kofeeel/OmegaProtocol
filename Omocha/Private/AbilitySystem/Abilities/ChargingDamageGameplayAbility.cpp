// Copyright It's My Turn


#include "AbilitySystem/Abilities/ChargingDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/OmochaAbilitySystemLibrary.h"
#include "AbilitySystem/OmochaAbilityTypes.h"
#include "Actor/ChargingArea.h"
#include "Kismet/KismetSystemLibrary.h"

AChargingArea* UChargingDamageGameplayAbility::SpawnArea(const FVector& Location)
{
	check(AreaClass);
	const FVector AreaLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	FVector Direction = (Location - AreaLocation);
	Direction.Z = 0.f;
	Direction.Normalize();

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(AreaLocation);
	SpawnTransform.SetRotation(Direction.Rotation().Quaternion());
	
	AChargingArea* Area = GetWorld()->SpawnActorDeferred<AChargingArea>(
			AreaClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (!IsValid(Area)) return nullptr;

	if (!IsLocallyControlled()) Area->SetActorHiddenInGame(true);

	Area->Direction = Direction;
	Area->FinishSpawning(SpawnTransform);
	
	return Area;	
}

void UChargingDamageGameplayAbility::Shock(const TArray<AActor*> Enemies)
{
	FDamageEffectParams DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
	for (AActor* Enemy : Enemies)
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Enemy))
		{
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UOmochaAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
	}
}
