// Copyright It's My Turn


#include "Actor/Hook.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/OmochaAbilitySystemLibrary.h"
#include "Actor/OmochaEffectActor.h"
#include "Actor/OmochaTransformProjectile.h"
#include "Actor/Shield.h"
#include "Character/OmochaCharacterBase.h"
#include "GameFramework/Character.h"

void AHook::BeginPlay()
{
	Super::BeginPlay();
	StartOutGoingTimeLine();
}

void AHook::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                            const FHitResult& SweepResult)
{
	if (bHasImpacted)
	{
		return;
	}
	if (!IsValidTarget(OtherActor))
	{
		return;
	}

	if (HasAuthority())
	{
		if (OtherComp->GetCollisionObjectType() == ECC_WorldStatic)
		{
			Destroy();
			return;
		}

		if (AShield* Shield = Cast<AShield>(OtherActor))
		{
			AActor* ShieldOwner = Shield->GetOwnerCharacter();
			if (ShieldOwner && UOmochaAbilitySystemLibrary::IsNotFriend(ShieldOwner, ReturnToActor))
			{
				if (UAbilitySystemComponent* TargetASC =
					UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Shield))
				{
					DamageParams.TargetAbilitySystemComponent = TargetASC;
					UOmochaAbilitySystemLibrary::ApplyDamageEffect(DamageParams);
					bHasImpacted = true;
					Destroy();
				}
			}
			return;
		}

		if (Cast<AOmochaTransformProjectile>(OtherActor) || Cast<AOmochaEffectActor>(OtherActor))
		{
			GrabToActor = OtherActor;
			bGrab = true;
			bHasImpacted = true;
			return;
		}

		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			DamageParams.TargetAbilitySystemComponent = TargetASC;
			UOmochaAbilitySystemLibrary::ApplyDamageEffect(DamageParams);
			GrabToActor = TargetASC->GetAvatarActor();
			bGrab = true;
			bHasImpacted = true;
		}
	}
}
