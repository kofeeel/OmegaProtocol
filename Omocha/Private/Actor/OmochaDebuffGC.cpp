// Copyright It's My Turn


#include "Actor/OmochaDebuffGC.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"
#include "Sound/SoundBase.h"
#include "GameFramework/Character.h" 
#include "Components/SkeletalMeshComponent.h" 

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
					const FVector TargetScale = MyTarget->GetActorScale3D();
					SpawnedVFX->SetWorldScale3D(TargetScale);
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
