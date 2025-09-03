// Copyright It's My Turn

#include "AbilitySystem/ExecCalc/OmochaDamageTextGC.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/OmochaAbilitySystemLibrary.h"

bool UOmochaDamageTextGC::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	if (!DamageText || !MyTarget) return false;
	
	UNiagaraComponent* DamageTextComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		MyTarget->GetWorld(),
		DamageText,
		Parameters.Location,
		FRotator::ZeroRotator,
		FVector::OneVector,
		true 
	);

	if (DamageTextComp)
	{
		const float DamageAmount = Parameters.RawMagnitude;
		const bool bIsCritical = UOmochaAbilitySystemLibrary::IsCriticalHit(Parameters.EffectContext);
		const bool bIsBlocked = UOmochaAbilitySystemLibrary::IsBlockedHit(Parameters.EffectContext);
		
		//Set Param for Niagara
		DamageTextComp->SetFloatParameter("DamageAmount", DamageAmount);
		DamageTextComp->SetBoolParameter("IsCritical", bIsCritical);
		DamageTextComp->SetBoolParameter("IsBlocked", bIsBlocked);
		
		//Count Digits
		const int32 DigitCount = FMath::Clamp(
			FString::FromInt(FMath::RoundToInt(DamageAmount)).Len(), 
			1, 6
		);
		DamageTextComp->SetIntParameter("DigitCount", DigitCount);
		
		const FLinearColor DamageColor = bIsCritical ? 
			FLinearColor::Yellow : FLinearColor::White;
		DamageTextComp->SetColorParameter("TextColor", DamageColor);
	}

	return true;
}
