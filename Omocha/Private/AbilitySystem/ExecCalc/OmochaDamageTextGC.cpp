// Copyright It's My Turn

#include "AbilitySystem/ExecCalc/OmochaDamageTextGC.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/OmochaAbilitySystemLibrary.h"
#include "AbilitySystem/OmochaAbilityTypes.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "UI/Widget/OmochaDamageTextComponent.h"

bool UOmochaDamageTextGC::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	if (!MyTarget) return false;
	
	AActor* InstigatorActor = Parameters.EffectContext.GetInstigator();
	APawn* InstigatorPawn = Cast<APawn>(InstigatorActor);
	APawn* TargetPawn = Cast<APawn>(MyTarget);

	bool bIsMyAttack = (InstigatorPawn 
		&& InstigatorPawn->IsLocallyControlled() 
		&& InstigatorPawn->IsPlayerControlled()); 

	bool bIsMyHit = (TargetPawn 
		&& TargetPawn->IsLocallyControlled() 
		&& TargetPawn->IsPlayerControlled());      

	if (!bIsMyAttack && !bIsMyHit) return true;    
	if (!DamageText) return false;

	const FOmochaGameplayEffectContext* OmochaContext = 
		static_cast<const FOmochaGameplayEffectContext*>(Parameters.EffectContext.Get());

	if (OmochaContext)
	{
		const bool bIsCritical = OmochaContext->IsCriticalHit();
		const bool bIsBlocked = OmochaContext->IsBlockedHit();
		const float DamageAmount = Parameters.RawMagnitude;
		
		UOmochaDamageTextComponent* DamageTextComp = NewObject<UOmochaDamageTextComponent>(MyTarget, DamageText);
		if (DamageTextComp)
		{
			DamageTextComp->RegisterComponent();
			DamageTextComp->AttachToComponent(MyTarget->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			DamageTextComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			DamageTextComp->SetDamageText(DamageAmount, bIsBlocked, bIsCritical);
		}
	}
	return true;

	// Niagara Text Render
	// if (!DamageText || !MyTarget) return false;
 //    
 //    FVector SpawnLocation = Parameters.Location;
 //    if (ACharacter* Character = Cast<ACharacter>(MyTarget))
 //    {
 //        if (UCapsuleComponent* CapsuleComp = Character->GetCapsuleComponent())
 //        {
 //            const float CapsuleHalfHeight = CapsuleComp->GetScaledCapsuleHalfHeight();
 //            const float OffsetHeight = 30.f;
 //            SpawnLocation = Character->GetActorLocation();
 //            SpawnLocation.Z += CapsuleHalfHeight + OffsetHeight;
 //            
 //            const float RandomRadius = FMath::RandRange(30.f, 130.f);
 //            const float RandomAngle = FMath::RandRange(0.f, 360.f);
 //            const float RandomHeight = FMath::RandRange(-20.f, 40.f);
 //            
 //            SpawnLocation.X += FMath::Cos(FMath::DegreesToRadians(RandomAngle)) * RandomRadius;
 //            SpawnLocation.Y += FMath::Sin(FMath::DegreesToRadians(RandomAngle)) * RandomRadius;
 //            SpawnLocation.Z += RandomHeight;
 //        }
 //    }
 //    
 //    UNiagaraComponent* DamageTextComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
 //        MyTarget->GetWorld(),
 //        DamageText,
 //        SpawnLocation,
 //        FRotator::ZeroRotator,
 //        FVector(1.0f),
 //        true 
 //    );
 //
 //    if (DamageTextComp)
 //    {
 //        const float DamageAmount = Parameters.RawMagnitude;
 //        const bool bIsCritical = UOmochaAbilitySystemLibrary::IsCriticalHit(Parameters.EffectContext);
 //        const bool bIsBlocked = UOmochaAbilitySystemLibrary::IsBlockedHit(Parameters.EffectContext);
 //        
 //        DamageTextComp->SetFloatParameter("Value", DamageAmount);
 //        DamageTextComp->SetFloatParameter("SpriteSize", 20.f);
 //        DamageTextComp->SetFloatParameter("SpriteAlpha", 1.f);
 //        DamageTextComp->SetFloatParameter("Lifetime", 0.1f);
 //        float SpacingScale = bIsCritical ? 20.0f : 15.0f;
 //        DamageTextComp->SetFloatParameter("Spacing", SpacingScale);
 //    }
 //	return true;
}
