// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/OmochaProjectile.h"
#include "GameplayTagContainer.h"
#include "OmochaTransformProjectile.generated.h"

class AOmochaPlayerCharacter;
class UOmochaCharacterConfigComponent;
class AOmochaPlayerController;

UENUM(BlueprintType)
enum class ETransformProjectileType : uint8
{
	Give,
	Steal
};

UENUM(BlueprintType)
enum class ETargetingState : uint8
{
	SeekingTarget,
	ReturningToOwner,
	FallingToGround
};

UCLASS()
class OMOCHA_API AOmochaTransformProjectile : public AOmochaProjectile
{
	GENERATED_BODY()

public:
	AOmochaTransformProjectile();
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent,
	                             AActor* OtherActor,
	                             UPrimitiveComponent* OtherComp,
	                             int32 OtherBodyIndex,
	                             bool bFromSweep,
	                             const FHitResult& SweepResult) override;

	virtual void StartDestroySequence() override;

public:
	UPROPERTY(BlueprintReadWrite, Category = "Transform", Replicated)
	ETransformProjectileType ProjectileType = ETransformProjectileType::Give;

	UPROPERTY(BlueprintReadWrite, Category = "Targeting", Replicated)
	ETargetingState CurrentTargetingState = ETargetingState::SeekingTarget;

	UPROPERTY(BlueprintReadWrite, Category = "Targeting", Replicated)
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Targeting", Replicated)
	TObjectPtr<AActor> OriginalOwner = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Transform", Replicated)
	TObjectPtr<AActor> StealInstigator = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Transform")
	bool bIsGiveOmegaType = true;

	UPROPERTY(BlueprintReadWrite, Category = "Transform")
	TObjectPtr<AOmochaPlayerController> FirePlayerController = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetingSpeed = 2000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float HomingStrength = 8.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float MinDistanceToTarget = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float FailedStateBounce = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float FailedStateGravity = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transform|Effects")
	TObjectPtr<UParticleSystem> TransformSuccessVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transform|Effects")
	TObjectPtr<USoundBase> TransformSuccessSFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transform|Effects")
	TObjectPtr<UParticleSystem> TransformFailVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transform|Effects")
	TObjectPtr<USoundBase> TransformFailSFX;

	UFUNCTION(BlueprintCallable, Category = "Transform")
	static AOmochaTransformProjectile* CreateStealProjectile(
		UWorld* World,
		AActor* OmegaTargetActor,
		AActor* StealCaster,
		TSubclassOf<AOmochaTransformProjectile> ProjectileClass
	);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transform|SkeletalMesh")
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;
	
	UFUNCTION(BlueprintCallable, Category = "Transform")
	static AOmochaTransformProjectile* SpawnAsDroppable(
		UWorld* World,
		const FVector& Location,
		AActor* Dropper,
		TSubclassOf<AOmochaTransformProjectile> ProjectileClass
	);

private:
	void DetermineProjectileType();
	bool HandleGiveOmega(UOmochaCharacterConfigComponent* TargetConfigComp, AActor* HitTarget);
	bool HandleStealOmega(UOmochaCharacterConfigComponent* TargetConfigComp, AActor* HitTarget);
	void PlayTransformEffects(bool bSuccess, const FVector& Location);
	void TransformSourcePlayer(FGameplayTag TargetState);
	bool IsValidTransformTarget(AActor* TestActor);

	void UpdateTargetingMovement(float DeltaTime);
	void SeekTarget(float DeltaTime);
	void ReturnToOwner(float DeltaTime);
	void FallToGround();

	bool IsTargetStillValid() const;
	bool IsOwnerStillValid() const;
	void ForceHitTarget(AOmochaPlayerCharacter* Target);

	bool bIsDroppable = false;
};
