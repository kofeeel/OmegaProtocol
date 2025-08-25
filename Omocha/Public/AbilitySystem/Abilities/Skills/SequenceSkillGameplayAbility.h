#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/OmochaGameplayAbility.h"
#include "AbilitySystem/Abilities/SkillGameplayAbility.h"
#include "Engine/DecalActor.h"
#include "SequenceSkillGameplayAbility.generated.h"
class UWaitForMouseClickTask;
class UGameplayAbility;

UENUM(BlueprintType)
enum class EDecalDisplayMode : uint8
{
	AtCharacter,
	AtMouseCursor,
	CharacterToMouse,
	MaxRangeFixed
};

USTRUCT(BlueprintType)
struct FSkillExecutionStep
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<USkillGameplayAbility> SkillClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DelayAfterPreviousStep = 0.0f;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Offset")
	FVector LocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Offset")
	float RotationOffset = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Offset")
	bool bShowOffsetInPreview = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Guide Decal")
	EDecalDisplayMode DecalDisplayMode = EDecalDisplayMode::AtCharacter;

	FSkillExecutionStep()
	{
		SkillClass = nullptr;
		DelayAfterPreviousStep = 0.0f;
		LocationOffset = FVector::ZeroVector;
		RotationOffset = 0.0f;
		bShowOffsetInPreview = true;
	}
};

USTRUCT()
struct FDecalCreationParams
{
	GENERATED_BODY()

	FSkillRangeData RangeData;
	int32 SkillIndex = 0;
	EDecalDisplayMode DisplayMode = EDecalDisplayMode::AtCharacter;
	FVector MouseLocation = FVector::ZeroVector;

	FVector LocationOffset = FVector::ZeroVector;
	float RotationOffset = 0.0f;
	bool bApplyOffset = false;

	FDecalCreationParams() = default;

	FDecalCreationParams(const FSkillRangeData& InRangeData, int32 InSkillIndex, const FVector& InMouseLocation)
		: RangeData(InRangeData)
		  , SkillIndex(InSkillIndex)
		  , MouseLocation(InMouseLocation)
		  , LocationOffset(FVector::ZeroVector)
		  , RotationOffset(0.0f)
		  , bApplyOffset(false)
	{
	}
};

USTRUCT()
struct FSkillTargetData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 SkillIndex = 0;

	UPROPERTY()
	FVector TargetLocation = FVector::ZeroVector;

	FSkillTargetData() = default;

	FSkillTargetData(int32 InSkillIndex, const FVector& InTargetLocation)
		: SkillIndex(InSkillIndex), TargetLocation(InTargetLocation)
	{
	}
};


UCLASS()
class OMOCHA_API USequenceSkillGameplayAbility : public UOmochaGameplayAbility
{
	GENERATED_BODY()

public:
	USequenceSkillGameplayAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	                             const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
	                        const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo,
	                        bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill Sequence")
	TArray<FSkillExecutionStep> SkillSequence;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Range Display")
	bool bShowSequenceRange = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Range Display")
	int32 RangeReferenceSkillIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Range Display")
	bool bUseMaxRangeFromAllSkills = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Range Display")
	UMaterialInterface* SequenceRangeDecalMaterial = nullptr;

	UPROPERTY()
	TArray<TObjectPtr<ADecalActor>> AtCharacterDecals;

	UPROPERTY()
	TArray<TObjectPtr<ADecalActor>> AtMouseDecals;

	UPROPERTY()
	TArray<TObjectPtr<ADecalActor>> CharacterToMouseDecals;

	UPROPERTY()
	TArray<TObjectPtr<ADecalActor>> MaxRangeFixedDecals;

	UPROPERTY()
	TObjectPtr<ADecalActor> SequenceRangeDecal;

	UPROPERTY()
	TMap<TObjectPtr<ADecalActor>, int32> DecalToSkillIndexMap;

	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> TemporarySkillHandles;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentExecutionStep = 0;

	UPROPERTY(BlueprintReadOnly)
	bool bIsExecutingSequence = false;

	UPROPERTY()
	TObjectPtr<UWaitForMouseClickTask> MouseClickTask;

	FVector SequenceTargetLocation;
	FGameplayAbilitySpecHandle CurrentStepAbilityHandle;
	FDelegateHandle OnAbilityEndedDelegateHandle;

	ADecalActor* CreateUnifiedDecal(const FDecalCreationParams& Params);
	void CalculateDecalTransform(const FDecalCreationParams& Params, const FVector& CharacterLocation,
	                             FVector& OutLocation, FRotator& OutRotation, bool& bShouldAttach);
	void SetupDecalComponent(ADecalActor* DecalActor, const FDecalCreationParams& Params);

	void DestroyDecalArray(TArray<TObjectPtr<ADecalActor>>& DecalArray);
	void ShowDecalArray(const TArray<TObjectPtr<ADecalActor>>& DecalArray);

	void ShowSkillRanges();
	void HideAllRanges();
	void ShowAllDecals();

	void StartMouseInput();
	void StopMouseInput();

	UFUNCTION()
	void OnMouseInputReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UFUNCTION()
	void OnMouseMoved(const FVector& MouseWorldLocation);

	void RotateDecalsToMouse(const FVector& MouseWorldLocation);
	void UpdateDecalsByMode(const FVector& MouseWorldLocation);

	void ExecuteSkillSequence(const FVector& TargetLocation);
	void ExecuteNextSkillInSequence();
	void OnSkillStepCompleted();

	UFUNCTION()
	void ExecuteSkillStep();

	UFUNCTION()
	void OnSubAbilityEnded(UGameplayAbility* EndedAbility);

	FSkillRangeData GetSequenceRangeData() const;
	FSkillRangeData GetMaxRangeFromAllSkills() const;
	FSkillRangeData GetRangeFromSpecificSkill(int32 SkillIndex) const;
	ADecalActor* CreateSequenceRangeDecal();

	bool IsSequenceValid() const;

	FVector GetTargetLocationForStep(int32 StepIndex) const;
	FVector CalculateDecalSize(const FSkillRangeData& RangeData, EDecalDisplayMode Mode,
	                           float DistanceToMouse = 0.0f) const;

	TMap<int32, FVector> CachedTargetLocations;

private:
	bool bHasReceivedFirstMouseMove = false;

	FVector TransformLocationOffset(const FVector& BaseLocation, const FVector& LocalOffset,
	                                const FRotator& PlayerRotation) const;
	float GetFinalRotationYaw(float BaseYaw, float RotationOffset) const;

	ADecalActor* FindDecalBySkillIndex(const TArray<TObjectPtr<ADecalActor>>& DecalArray, int32 TargetSkillIndex) const;

	void CacheAllTargetLocations();

	UFUNCTION(Server, Reliable)
	void ServerExecuteSkillSequenceWithDecalData(const FVector& MouseLocation,
	                                             const TArray<FSkillTargetData>& DecalTargetData);

	UFUNCTION()
	TArray<FSkillTargetData> CollectDecalTargetData() const;

	TArray<TObjectPtr<ADecalActor>>& GetDecalArrayForMode(EDecalDisplayMode Mode);

	FVector ConvertMidpointToEndpoint(const FVector& DecalLocation, const FVector& BaseLocation,
	                                  EDecalDisplayMode Mode) const;
};
