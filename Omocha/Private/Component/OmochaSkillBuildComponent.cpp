// Copyright It's My Turn


#include "Component/OmochaSkillBuildComponent.h"

#include "DataAsset/SkillBuildData.h"
#include "Net/UnrealNetwork.h"


UOmochaSkillBuildComponent::UOmochaSkillBuildComponent()
{
	SetIsReplicatedByDefault(true);
}

void UOmochaSkillBuildComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UOmochaSkillBuildComponent, AcquiredBuilds_Replicated);
}

void UOmochaSkillBuildComponent::OnRep_AcquiredBuilds()
{
	AcquiredBuilds.Empty();
	for (const FAcquiredBuildInfo& Info : AcquiredBuilds_Replicated)
	{
		AcquiredBuilds.Add(Info.BuildTag, Info.BuildLevel);
	}
	// TODO: UI BroadCast 
}

void UOmochaSkillBuildComponent::Server_AddBuild_Implementation(const FGameplayTag& BuildTag, const int32 InBuildLevel)
{
	if (!SkillBuildDataTable || !SkillBuildDataTable->FindRow<FSkillBuildData>(BuildTag.GetTagName(), TEXT("")))
	{
		return;
	}

	int32 CurrentLevel;
	if (InBuildLevel == -1)
	{
		int32& AcquiredBuildLevel = AcquiredBuilds.FindOrAdd(BuildTag, 0);
		CurrentLevel = ++AcquiredBuildLevel;
	}
	else
	{
		CurrentLevel = InBuildLevel;
	}

	FAcquiredBuildInfo* ExistingInfo = AcquiredBuilds_Replicated.FindByPredicate(
		[&](const FAcquiredBuildInfo& Info) { return Info.BuildTag.MatchesTagExact(BuildTag); });

	if (ExistingInfo)
	{
		ExistingInfo->BuildLevel = CurrentLevel;
	}
	else
	{
		AcquiredBuilds_Replicated.Add({BuildTag, CurrentLevel});
	}

	OnRep_AcquiredBuilds();
}

bool UOmochaSkillBuildComponent::HasBuild(const FGameplayTag& BuildTag) const
{
	return AcquiredBuilds.Contains(BuildTag);
}

int32 UOmochaSkillBuildComponent::GetBuildLevel(const FGameplayTag& BuildTag) const
{
	const int32* FoundLevel = AcquiredBuilds.Find(BuildTag);
	return FoundLevel ? *FoundLevel : 0;
}

void UOmochaSkillBuildComponent::GetAcquiredBuildsWithCondition(EBuildTriggerCondition Condition,
                                                                TArray<FAcquiredBuildInfo>& OutAcquiredBuilds) const
{
	if (!SkillBuildDataTable)
	{
		return;
	}

	OutAcquiredBuilds.Empty();

	for (const auto& Pair : AcquiredBuilds)
	{
		const FGameplayTag& BuildTag = Pair.Key;

		FSkillBuildData* BuildData = SkillBuildDataTable->FindRow<FSkillBuildData>(BuildTag.GetTagName(), TEXT(""));

		if (BuildData && BuildData->TriggerCondition == Condition)
		{
			OutAcquiredBuilds.Add({BuildTag, Pair.Value});
		}
	}
}

float UOmochaSkillBuildComponent::GetModifiedAbilityPropertyValue(const FGameplayTag& AbilityTag,
                                                                  const FGameplayTag& PropertyTag,
                                                                  float BaseValue) const
{
	if (!SkillBuildDataTable)
	{
		return BaseValue;
	}

	float ModifiedValue = BaseValue;

	for (const auto& Pair : AcquiredBuilds)
	{
		const FGameplayTag& BuildTag = Pair.Key;
		const int32 BuildLevel = Pair.Value;

		FSkillBuildData* BuildData = SkillBuildDataTable->FindRow<FSkillBuildData>(BuildTag.GetTagName(), TEXT(""));
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
			if (Effect.EffectType == EBuildEffectType::AbilityProperty && Effect.PropertyTag.MatchesTagExact(
				PropertyTag))
			{
				if (Effect.ValueByLevel)
				{
					const float ModValue = Effect.ValueByLevel->GetFloatValue(BuildLevel);

					switch (Effect.ModificationOp)
					{
					case EGameplayModOp::Additive:
						ModifiedValue += ModValue;
						break;
					case EGameplayModOp::Multiplicitive:
						ModifiedValue *= ModValue;
						break;
					case EGameplayModOp::Division:
						if (ModValue != 0.f)
						{
							ModifiedValue /= ModValue;
						}
						break;
					case EGameplayModOp::Override:
						ModifiedValue = ModValue;
						break;
					default: ;
					}
				}
			}
		}
	}

	return ModifiedValue;
}

bool UOmochaSkillBuildComponent::HasCustomBuildLogic(const FGameplayTag& AbilityTag,
                                                     const FGameplayTag& CustomLogicTag) const
{
	if (!SkillBuildDataTable)
	{
		return false;
	}

	for (const auto& Pair : AcquiredBuilds)
	{
		const FGameplayTag& BuildTag = Pair.Key;

		FSkillBuildData* BuildData = SkillBuildDataTable->FindRow<FSkillBuildData>(BuildTag.GetTagName(), TEXT(""));
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
			if (Effect.EffectType == EBuildEffectType::CustomLogic && Effect.CustomLogicTag.MatchesTagExact(
				CustomLogicTag))
			{
				return true;
			}
		}
	}
	return false;
}

FGameplayTagContainer UOmochaSkillBuildComponent::GetMasterBuildTagContainer() const
{
	FGameplayTagContainer Container;

	for (auto Build : AcquiredBuilds)
	{
		if (Build.Value == 3)
		{
			Container.AddTag(Build.Key);
		}
	}
	
	return Container;
}
