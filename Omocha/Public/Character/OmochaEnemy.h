// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Actor/OmochaEffectActor.h>

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Character/OmochaCharacterBase.h"
#include "DataAsset/OmochaEnemyDataAsset.h"
#include "OmochaEnemy.generated.h"

class AOmochaEffectActor;
class UBehaviorTree;
class AOmochaAIController;
struct FOmochaEnemyAttributeData;

UENUM(BlueprintType)
enum class EEnemyType: uint8
{
	None,
	Muriel,
	Zinx,
	Shiv,
	Murdock,
	Boris,
	Grappler,
	Glitch,
	BlazedCannon,
	Screamer,
};

struct FOmochaEnemyNames
{
	static FString FindName(const EEnemyType& Type)
	{
		return EnemyNames[Type];
	}

	static void InitializeNames();

private:
	static TMap<EEnemyType, FString> EnemyNames;
};

UCLASS()
class OMOCHA_API AOmochaEnemy : public AOmochaCharacterBase
{
	GENERATED_BODY()

public:
	AOmochaEnemy();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	EEnemyClass EnemyClass = EEnemyClass::Infantry;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	uint8 EnemyIdx = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	EEnemyRank EnemyRank = EEnemyRank::Minion;

	UPROPERTY()
	TObjectPtr<AOmochaAIController> OmochaAIController;

	UPROPERTY()
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void ApplyEnemyAttributesFromRowName(const FName& RowName);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	FName GetEnemyAttributeRowName() const;

	// Animation Interface
	UFUNCTION(BlueprintPure, Category = "Animation")
	float GetEnemyMovementSpeed() const;

	UFUNCTION(BlueprintPure, Category = "Animation")
	float GetEnemyMovementDirection() const;

	UFUNCTION(BlueprintCallable)
	FString GetEnemyName() const { return FOmochaEnemyNames::FindName(EnemyType); }

	void SetDropItemClass(const TSubclassOf<AOmochaEffectActor>& Item) { DropItemClass = Item; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaTime) override;

	virtual void InitAbilityActorInfo() override;

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void InitializeFromDataAsset();

	void AddEnemyAbilities();
	UOmochaEnemyDataAsset* GetEnemyDataAsset() const;

	virtual void Die(const FVector& DeathImpulse = FVector::ZeroVector) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death")
	float LifeSpan = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Revive")
	bool bCanRevive = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Attributes")
	FName InitialEnemyRowName;

	UPROPERTY()
	FTimerHandle ReviveTimer;

	virtual void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount) override;

	FEnemyClassInfo CurrentClassInfo;
	float CurrentRank = 1.0f;

	void InitializeBlackboardKeys();

	UPROPERTY(EditDefaultsOnly)
	EEnemyType EnemyType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drop System")
	TSubclassOf<AActor> DropItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drop System", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DropChance = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drop System")
	bool bAlwaysDrop = false;

	UFUNCTION(BlueprintCallable, Category = "Drop System")
	void ProcessItemDrop();

	UPROPERTY(EditAnywhere, Category = "Anim")
	UAnimMontage* BeginAnimMontage;

	UFUNCTION(NetMulticast, reliable)
	void Multi_HideHealthBar();

private:
	void UpdateAnimationData(float DeltaTime);

	float EnemyMovementSpeed = 0.0f;
	float EnemyMovementDirection = 0.0f;
	bool bEnemyIsMoving = false;

	FTimerHandle SinkTimer;
	float CurrentSinkValue = 0.0f;
	float OriginalZLocation = 0.0f;

	void StartEnemySinking();
	void UpdateSinking();
};
