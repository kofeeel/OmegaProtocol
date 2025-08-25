// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "OmochaSkillBuildComponent.generated.h"

USTRUCT(BlueprintType)
struct FAcquiredBuildEntry
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag BuildTag = FGameplayTag();

	UPROPERTY()
	float Value = 0.0f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OMOCHA_API UOmochaSkillBuildComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOmochaSkillBuildComponent();

	UFUNCTION(BlueprintPure, Category = "SkillBuild")
	bool HasBuild(const FGameplayTag& BuildTag) const;

	UFUNCTION(BlueprintPure, Category = "SkillBuild")
	float GetBuildValue(const FGameplayTag& BuildTag, float DefaultValue = 0.f) const;

	UFUNCTION(Server, Reliable)
	void Server_AddBuild(const FGameplayTag& BuildTag);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	TMap<FGameplayTag, float> AcquiredBuilds;

	UFUNCTION()
	void OnRep_AcquiredBuilds();

	UPROPERTY(ReplicatedUsing = OnRep_AcquiredBuilds)
	TArray<FAcquiredBuildEntry> AcquiredBuilds_Replicated;

	UPROPERTY(EditDefaultsOnly, Category = "SkillBuild")
	TObjectPtr<UDataTable> SkillBuildDataTable;
};
