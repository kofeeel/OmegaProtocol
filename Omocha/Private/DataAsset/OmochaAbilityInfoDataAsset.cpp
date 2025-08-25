// Copyright It's My Turn


#include "DataAsset/OmochaAbilityInfoDataAsset.h"

FAbilityInfo UOmochaAbilityInfoDataAsset::FindAbilityInfoForTag(const FGameplayTag& AbilityTag) const
{
	for (const FAbilityInfo& Info: AbilityInformation)
	{
		if (Info.AbilityTag == AbilityTag)
		{
			return Info;
		}
	}
	return FAbilityInfo();
}
