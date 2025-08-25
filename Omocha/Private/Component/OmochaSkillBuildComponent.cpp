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
	for (const FAcquiredBuildEntry& Entry : AcquiredBuilds_Replicated)
	{
		AcquiredBuilds.Add(Entry.BuildTag, Entry.Value);
	}
}

void UOmochaSkillBuildComponent::Server_AddBuild_Implementation(const FGameplayTag& BuildTag)
{
	if (SkillBuildDataTable)
	{
		FSkillBuildData* BuildData = SkillBuildDataTable->FindRow<FSkillBuildData>(BuildTag.GetTagName(), TEXT(""));
		if (BuildData)
		{
			AcquiredBuilds.FindOrAdd(BuildTag) = BuildData->Value;

			FAcquiredBuildEntry* ExistingEntry = AcquiredBuilds_Replicated.FindByPredicate(
				[&](const FAcquiredBuildEntry& Entry) { return Entry.BuildTag.MatchesTagExact(BuildTag); });

			if (ExistingEntry)
			{
				ExistingEntry->Value = BuildData->Value;
			}
			else
			{
				AcquiredBuilds_Replicated.Add(FAcquiredBuildEntry{BuildTag, BuildData->Value});
			}
		}
	}
}

bool UOmochaSkillBuildComponent::HasBuild(const FGameplayTag& BuildTag) const
{
	return AcquiredBuilds.Contains(BuildTag);
}

float UOmochaSkillBuildComponent::GetBuildValue(const FGameplayTag& BuildTag, float DefaultValue) const
{
	const float* FoundValue = AcquiredBuilds.Find(BuildTag);
	return FoundValue ? *FoundValue : DefaultValue;
}
