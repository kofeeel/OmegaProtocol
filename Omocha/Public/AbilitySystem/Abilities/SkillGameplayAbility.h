// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmochaGameplayAbility.h"
#include "AbilitySystem/OmochaAttributeSet.h"
#include "SkillGameplayAbility.generated.h"

/**
 * 
 */

class UOmochaMouseHitTask;
class UWaitForMouseClickTask;

UENUM(BlueprintType)
enum class ESkillRangeType : uint8
{
	Circle, // 원형 (프로젝타일, 이동)
	Cone, // 부채꼴 (트레이스)
	Rectangle // 직사각형 (라인 트레이스)
};

USTRUCT(BlueprintType)
struct FSkillRangeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESkillRangeType RangeType = ESkillRangeType::Circle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxRange = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinRange = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Angle = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Radius = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector Size = FVector(100.0f, 100.0f, 100.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UMaterialInterface* DecalMaterial = nullptr;

	FVector GetSkillSize() const;

	FSkillRangeData()
	{
		RangeType = ESkillRangeType::Circle;
		MaxRange = 1000.0f;
		MinRange = 0.0f;

		DecalMaterial = nullptr;
	}
};

UCLASS()
class OMOCHA_API USkillGameplayAbility : public UOmochaGameplayAbility
{
	GENERATED_BODY()

public:
	USkillGameplayAbility();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill Attribute")
	bool bAffectedByMaxRange = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill Attribute")
	bool bAffectedBySkillSize = false;

	UFUNCTION(BlueprintCallable, Category = "Input Task")
	virtual void RequestTargetData();
	
	UFUNCTION(BlueprintCallable, Category = "Attack")
	virtual void SetShouldEndAttack(bool bValue) { bShouldEndAttack = bValue; }

	UFUNCTION(BlueprintPure, Category = "Attack")
	virtual bool GetShouldEndAttack() const { return bShouldEndAttack; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Range")
	FSkillRangeData SkillRangeData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Socket")
	FGameplayTag SocketTag;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual FSkillRangeData GetSkillRangeData(UAbilitySystemComponent* ASC = nullptr) const;

	virtual void ExecuteSkill(const FVector& TargetLocation)
	{
	}

protected:
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
								 const FGameplayAbilityActorInfo* ActorInfo,
								 const FGameplayAbilityActivationInfo ActivationInfo,
								 const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UFUNCTION()
	virtual void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	
	virtual FVector GetLaunchLocation(ACharacter* Character);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkillData")
	FGameplayTag SkillTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly, Category = "Attack State")
	bool bShouldEndAttack = false;

	UPROPERTY()
	bool bHasProcessedTargetData = false;

	UPROPERTY()
	TObjectPtr<UOmochaMouseHitTask> MouseClickTask;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Attacok")
	void OnInputReleased();

	UFUNCTION(BlueprintPure, Category = "Skill|BuildSystem")
	class UOmochaSkillBuildComponent* GetSkillBuildComponent() const;

	UFUNCTION(BlueprintPure, Category = "Skill|BuildSystem")
	float GetModifiedPropertyValue(const FGameplayTag& PropertyTag, float DefaultValue) const;

	UFUNCTION(BlueprintPure, Category = "Skill|BuildSystem")
	bool HasCustomBuildLogic(const FGameplayTag& CustomLogicTag) const;
};
