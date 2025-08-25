// Copyright It's My Turn


#include "DataAsset/OmochaEnemyDataAsset.h"

FEnemyClassInfo UOmochaEnemyDataAsset::GetEnemyClassInfo(EEnemyRank InRank, EEnemyClass InClass) const
{
	if (const FEnemyRankData* RankData = EnemyData.Find(InRank))
	{
		if (const FEnemyClassInfo* ClassInfo = RankData->ClassData.Find(InClass))
		{
			return *ClassInfo;
		}
	}
	return FEnemyClassInfo();
}

TArray<TSubclassOf<UGameplayAbility>> UOmochaEnemyDataAsset::GetEnemyAbilities(EEnemyRank InRank, EEnemyClass InClass,
	int32 EnemyIndex) const
{
	TArray<TSubclassOf<UGameplayAbility>> AllAbilities;
	
	AllAbilities.Append(CommonAbilities);
	
	FEnemyClassInfo ClassInfo = GetEnemyClassInfo(InRank, InClass);
	if (ClassInfo.EnemyType.IsValidIndex(EnemyIndex))
	{
		AllAbilities.Append(ClassInfo.EnemyType[EnemyIndex].Abilities);
	}
    
	return AllAbilities;
}




