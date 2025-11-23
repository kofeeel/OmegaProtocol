// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "OmochaSkillBuildComponent.generated.h"

enum class EBuildTriggerCondition : uint8;

USTRUCT(BlueprintType)
struct FAcquiredBuildInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag BuildTag;

	UPROPERTY()
	int32 BuildLevel = 0;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OMOCHA_API UOmochaSkillBuildComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOmochaSkillBuildComponent();

	UPROPERTY(EditDefaultsOnly, Category = "SkillBuild")
	TObjectPtr<UDataTable> SkillBuildDataTable;

	UFUNCTION(Server, Reliable)
	void Server_AddBuild(const FGameplayTag& BuildTag, const int32 InBuildLevel = -1);

	UFUNCTION(BlueprintPure, Category = "SkillBuild")
	bool HasBuild(const FGameplayTag& BuildTag) const;

	UFUNCTION(BlueprintPure, Category = "SkillBuild")
	int32 GetBuildLevel(const FGameplayTag& BuildTag) const;

	UFUNCTION(BlueprintCallable, Category = "SkillBuild")
	void GetAcquiredBuildsWithCondition(EBuildTriggerCondition Condition,
	                                    TArray<FAcquiredBuildInfo>& OutAcquiredBuilds) const;

	UFUNCTION(BlueprintCallable, Category = "SkillBuild")
	float GetModifiedAbilityPropertyValue(const FGameplayTag& AbilityTag, const FGameplayTag& PropertyTag,
	                                      float BaseValue) const;

	UFUNCTION(BlueprintCallable, Category = "SkillBuild")
	bool HasCustomBuildLogic(const FGameplayTag& AbilityTag, const FGameplayTag& CustomLogicTag) const;

	FGameplayTagContainer GetMasterBuildTagContainer() const;

	TMap<FGameplayTag, int32> GetAcquiredBuilds() const { return AcquiredBuilds; }

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY()
	TMap<FGameplayTag, int32> AcquiredBuilds;

	UPROPERTY(ReplicatedUsing = OnRep_AcquiredBuilds)
	TArray<FAcquiredBuildInfo> AcquiredBuilds_Replicated;

	UFUNCTION()
	void OnRep_AcquiredBuilds();
};
