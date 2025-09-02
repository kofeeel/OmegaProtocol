// Copyright It's My Turn


#include "AbilitySystem/OmochaBuildSystemLibrary.h"

#include "AbilitySystem/OmochaAttributeSet.h"
#include "Component/OmochaSkillBuildComponent.h"
#include "DataAsset/SkillBuildData.h"
#include "Player/OmochaPlayerState.h"

void UOmochaBuildSystemLibrary::ApplyBuildsForTrigger(UAbilitySystemComponent* ASC, UAbilitySystemComponent* TargetASC,
                                                      EBuildTriggerCondition Condition, const FGameplayTag& AbilityTag)
{
	if (!ASC || !ASC->GetAvatarActor())
	{
		return;
	}

	AOmochaPlayerState* PlayerState = Cast<AOmochaPlayerState>(ASC->GetOwnerActor());
	if (!PlayerState)
	{
		return;
	}

	UOmochaSkillBuildComponent* BuildComp = PlayerState->GetSkillBuildComponent();
	if (!BuildComp || !BuildComp->SkillBuildDataTable)
	{
		return;
	}

	TArray<FAcquiredBuildInfo> TriggeredBuilds;
	BuildComp->GetAcquiredBuildsWithCondition(Condition, TriggeredBuilds);

	for (const FAcquiredBuildInfo& AcquiredBuild : TriggeredBuilds)
	{
		FSkillBuildData* BuildData = BuildComp->SkillBuildDataTable->FindRow<FSkillBuildData>(
			AcquiredBuild.BuildTag.GetTagName(), TEXT(""));
		if (!BuildData)
		{
			continue;
		}

		if (BuildData->TargetAbilityTag.IsValid() && !AbilityTag.MatchesTagExact(BuildData->TargetAbilityTag))
		{
			continue;
		}

		for (const FSkillBuildEffect& Effect : BuildData->Effects)
		{
			UAbilitySystemComponent* FinalTargetASC = (Effect.EffectTarget == EEffectTarget::Source) ? ASC : TargetASC;
			if (!FinalTargetASC)
			{
				continue;
			}

			switch (Effect.EffectType)
			{
			case EBuildEffectType::AttributeModifier:
				{
					if (Effect.AttributeToModify.IsValid() && Effect.ValueByLevel)
					{
						const float ModValue = Effect.ValueByLevel->GetFloatValue(AcquiredBuild.BuildLevel);
						FinalTargetASC->ApplyModToAttribute(Effect.AttributeToModify, Effect.ModificationOp, ModValue);
					}
					break;
				}
			case EBuildEffectType::GameplayEffectApplication:
				{
					if (Effect.EffectToApply)
					{
						FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
						EffectContext.AddSourceObject(ASC->GetAvatarActor());

						FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
							Effect.EffectToApply, AcquiredBuild.BuildLevel, EffectContext);
						
						if (SpecHandle.IsValid())
						{
							if (Effect.ValueByLevel && Effect.MagnitudeSetByCallerTag.IsValid())
							{
								const float ModValue = Effect.ValueByLevel->GetFloatValue(AcquiredBuild.BuildLevel);
								SpecHandle.Data->SetSetByCallerMagnitude(Effect.MagnitudeSetByCallerTag, ModValue);
							}

							if (Effect.DurationPolicy == EGameplayEffectDurationType::HasDuration)
							{
								if (Effect.DurationByLevel && Effect.DurationSetByCallerTag.IsValid())
								{
									const float Duration = Effect.DurationByLevel->GetFloatValue(AcquiredBuild.BuildLevel);
									SpecHandle.Data->SetSetByCallerMagnitude(Effect.DurationSetByCallerTag, Duration);
								}
							}
							
							ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), FinalTargetASC);
						}
					}
					break;
				}
			case EBuildEffectType::GrantAbility:
				{
					if (Effect.AbilityToGrant)
					{
						FGameplayAbilitySpec AbilitySpec(Effect.AbilityToGrant, AcquiredBuild.BuildLevel);
						ASC->GiveAbility(AbilitySpec);
					}
					break;
				}
			case EBuildEffectType::AbilityProperty:
			case EBuildEffectType::CustomLogic:
			case EBuildEffectType::MaxHealthDamage:
				break;
			default:
				break;
			}
		}
	}
}
