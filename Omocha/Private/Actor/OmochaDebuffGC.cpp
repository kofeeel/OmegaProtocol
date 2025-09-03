// Copyright It's My Turn


#include "Actor/OmochaDebuffGC.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "OmochaGameplayTags.h"
#include "AbilitySystem/OmochaAbilitySystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"
#include "Sound/SoundBase.h"
#include "GameFramework/Character.h" 
#include "Components/SkeletalMeshComponent.h" 
#include "Components/CapsuleComponent.h"

AOmochaDebuffGC::AOmochaDebuffGC()
{
	PrimaryActorTick.bCanEverTick = false;
    
	DebuffVFX = nullptr;
	DebuffSFX = nullptr;
}

void AOmochaDebuffGC::HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType,
										const FGameplayCueParameters& Parameters)
{
	Super::HandleGameplayCue(MyTarget, EventType, Parameters);

	if (!HasAuthority()) { return; }
	if (!IsValid(MyTarget)) { return; }

	switch (EventType)
	{
	case EGameplayCueEvent::OnActive:
		{
			if (IsValid(DebuffVFX))
			{
				UNiagaraComponent* SpawnedVFX = UNiagaraFunctionLibrary::SpawnSystemAttached(
					DebuffVFX,
					MyTarget->GetRootComponent(),
					NAME_None,
					FVector::ZeroVector,
					FRotator::ZeroRotator,
					EAttachLocation::KeepRelativeOffset,
					false // Auto destroy
				);

				if (IsValid(SpawnedVFX))
				{
					FVector NewScale = FVector::OneVector;
            
					if (const ACharacter* TargetCharacter = Cast<ACharacter>(MyTarget))
					{
						if (const UCapsuleComponent* Capsule = TargetCharacter->GetCapsuleComponent())
						{
							constexpr float BaseCapsuleRadius = 34.0f;
							const float ScaleMultiplier = Capsule->GetScaledCapsuleRadius() / BaseCapsuleRadius;
							NewScale = FVector(ScaleMultiplier);
						}
					}
					else
					{
						NewScale = MyTarget->GetActorScale3D();
					}
					
					SpawnedVFX->SetWorldScale3D(NewScale);
					ActiveVFXMap.Add(MyTarget, SpawnedVFX);
				}
			}

			if (IsValid(DebuffSFX))
			{
				UGameplayStatics::PlaySoundAtLocation(
					this,
					DebuffSFX,
					MyTarget->GetActorLocation()
				);
			}
			TryShowDamageText(MyTarget, Parameters);
			break;
		}

	case EGameplayCueEvent::Removed:
		{
			if (TObjectPtr<UNiagaraComponent>* FoundVFX = ActiveVFXMap.Find(MyTarget))
			{
				if (IsValid(*FoundVFX))
				{
					(*FoundVFX)->DestroyComponent();
				}
                
				ActiveVFXMap.Remove(MyTarget);
			}
			break;
		}

	default:
		break;
	}
}

void AOmochaDebuffGC::TryShowDamageText(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	const float DebuffDamage = UOmochaAbilitySystemLibrary::GetDebuffDamage(Parameters.EffectContext);
	if (DebuffDamage <= 0.f) return;
    
	if (APawn* Pawn = Cast<APawn>(MyTarget))
	{
		if (!Pawn->IsLocallyControlled()) return;
	}
    
	if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MyTarget))
	{
		FGameplayCueParameters DamageParams;
		DamageParams.EffectContext = Parameters.EffectContext;
		DamageParams.RawMagnitude = DebuffDamage; 
		DamageParams.Location = MyTarget->GetActorLocation();
        
		const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();
		ASC->ExecuteGameplayCue(GameplayTags.GameplayCue_DamageText, DamageParams);
	}
}
