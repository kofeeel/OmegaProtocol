// Copyright It's My Turn


#include "AbilitySystem/OmochaShieldAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "OmochaGameplayTags.h"
#include "Actor/Shield.h"
#include "GameFramework/Character.h"

void UOmochaShieldAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	UAttributeSet::PostGameplayEffectExecute(Data);
	FEffectProperties Props;
	SetEffectProperties(Data, Props);
	
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleIncomingDamage(Props);
	}
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp<float>(GetHealth(), 0, GetMaxHealth()));
	}
}
void UOmochaShieldAttributeSet::HandleIncomingDamage(const FEffectProperties& Props)
{
	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0.f);
	
	if (LocalIncomingDamage > 0.f)
	{
		const float NewHealth = GetHealth() - 1.f;
		if (NewHealth <= 0.f) OnShieldBroken.Broadcast();
		SetHealth(FMath::Clamp<float>(NewHealth, 0, GetMaxHealth()));
		//ShowFloatingText(Props, 1.f, true, false);
		FGameplayCueParameters CueParams;
		CueParams.EffectContext = Props.EffectContextHandle;
		CueParams.RawMagnitude = 1.f; 
		CueParams.Location = Props.TargetAvatarActor->GetActorLocation();

		if (FOmochaGameplayEffectContext* OmochaContext = static_cast<FOmochaGameplayEffectContext*>(const_cast<FGameplayEffectContext*>(Props.EffectContextHandle.Get())))
		{
			OmochaContext->SetIsBlockedHit(true);
			OmochaContext->SetIsCriticalHit(false);
		}

		Props.TargetASC->ExecuteGameplayCue(FOmochaGameplayTags::Get().GameplayCue_DamageText, CueParams);

		OnShieldDamaged.Broadcast();
		OnShieldDamaged.Broadcast();
	}
}

void UOmochaShieldAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data,
	FEffectProperties& Props) const
{
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Props.SourceASC)
		&& Props.SourceASC->AbilityActorInfo.IsValid()
		&& Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if (APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}
		if (Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid()
		&& Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		if (AShield* TargetShield = Cast<AShield>(Props.TargetAvatarActor))
		{
			Props.TargetCharacter = Cast<ACharacter>(TargetShield->GetOwnerCharacter());
			if (Props.TargetCharacter)
			{
				Props.TargetController = TargetShield->GetOwnerCharacter()->GetController();
			}
		}
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}
