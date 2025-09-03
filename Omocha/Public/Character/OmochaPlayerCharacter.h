// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/OmochaCharacterBase.h"
#include "OmochaPlayerCharacter.generated.h"

class USpringArmComponent;
class AOmochaPlayerState;
class UOmochaCharacterDataAsset;
class UOmochaCharacterConfigComponent;
class UOmochaWeaponComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerAlarm);

UENUM()
enum class EPlayerCharter : uint8
{
	None = 0,
	Rusty,
	Vulcan,
	Zenith
};

/**
 * 
 */
UCLASS()
class OMOCHA_API AOmochaPlayerCharacter : public AOmochaCharacterBase
{
	GENERATED_BODY()

public:
	AOmochaPlayerCharacter();

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Look System Interface
	UFUNCTION(BlueprintCallable, Category = "Look System")
	void SetSpineRotation(float Rotation);

	UFUNCTION(BlueprintPure, Category = "Look System")
	float GetSpineRotation() const { return CurrentSpineRotation; }

	// Animation Interface
	UFUNCTION(BlueprintPure, Category = "Animation")
	float GetMovementSpeed() const;

	UFUNCTION(BlueprintPure, Category = "Animation")
	float GetMovementDirection() const;

	void ApplyOmegaAttributeOrRemove(FGameplayTag StateTag);

	USceneComponent* GetFixedDecalRoot() const { return FixedDecalRoot; }

	UFUNCTION(BlueprintImplementableEvent, Category = "Transform")
	void OnTransformAnimationStarted(float MontageLength);

	UFUNCTION(BlueprintImplementableEvent, Category = "Transform Effects")
	void PlayTransformEffects(bool bToOmegaState, FVector PlayerLocation, UOmochaCharacterDataAsset* CharacterData);

	UFUNCTION(BlueprintCallable, Category = "Transform")
	void BlockInputForTransform(bool bBlocked);

	UFUNCTION(BlueprintCallable, Category = "Components")
	UOmochaCharacterConfigComponent* GetCharacterConfigComponent() { return CharacterConfigComponent; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UOmochaWeaponComponent> WeaponComponent;
	
	UFUNCTION(BlueprintPure, Category = "Components")
	UStaticMeshComponent* GetHeadStaticMeshComponent() const { return HeadStaticMeshComponent; }

	// State Decal Control Functions
	UFUNCTION(BlueprintCallable, Category = "State Decals")
	void UpdateStateVisuals(FGameplayTag CurrentState);

	UFUNCTION(BlueprintCallable, Category = "State Decals")
	void ShowNormalDecal();

	UFUNCTION(BlueprintCallable, Category = "State Decals")
	void ShowOmegaDecal();

	UFUNCTION(BlueprintCallable, Category = "State Decals")
	void HideAllDecals();

	FGameplayTag LoadState = FGameplayTag();
	FGameplayTag PlayerState = FGameplayTag();

	UPROPERTY(BlueprintReadOnly, Category = "Minimap")
	TObjectPtr<UMaterialInstanceDynamic> MinimapDynamicInstance;

	UFUNCTION(Client, Reliable)
	void Client_StartLobbyCameraCinematic();

	UFUNCTION(BlueprintImplementableEvent, Category = "Cinematic")
	void Event_StartLobbyCameraCinematic();

	virtual void Die(const FVector& DeathImpulse) override;
	virtual void Rebirth() override;
	virtual void Rebirth(bool bBecomeOmega = false);

	UFUNCTION(Server, Reliable)
	void StripOmegaState_Server();

	UFUNCTION(NetMulticast, Reliable)
	void StripOmegaState_Multicast();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayRevivalMontage();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_StopAndPlayDeathMontage(); 

	UFUNCTION(BlueprintCallable, Category = "Revival")
	void CancelReviveSequence();

	UFUNCTION(BlueprintCallable, Category = "Revival")
	void BeginReviveSequence();

	UPROPERTY(BlueprintAssignable)
	FOnPlayerAlarm OnPlayerDamaged;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerAlarm OnPlayerDied;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerAlarm OnPlayerRebirth;

	EPlayerCharter GetPlayerCharterType() const { return CharacterType; }

protected:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UOmochaCharacterConfigComponent* CharacterConfigComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> FixedDecalRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* HeadStaticMeshComponent;

	// State Decal Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State Decals")
	class UDecalComponent* NormalStateDecal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State Decals")
	class UDecalComponent* OmegaStateDecal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Omega Aura")
	TObjectPtr<UNiagaraComponent> OmegaAuraComponent;

	// Death Data
	UPROPERTY(EditDefaultsOnly, Category = "Player Death")
	TSubclassOf<class AOmochaTransformProjectile> TransformProjectileClassToDrop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Death")
	TArray<UAnimMontage*> NormalDeathAnimations;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Death")
	TArray<UAnimMontage*> OmegaDeathAnimations;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Death")
	USoundBase* NormalDeathSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Death")
	USoundBase* OmegaDeathSound;

	virtual UAnimMontage* GetDeathAnimation() const override;
	virtual USoundBase* GetDeathSound() const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetHealthBarVisibility(bool bVisible);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "UI")
	void Multicast_SetHealthBarVisibility(bool bVisible);

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> OmegaAttributesEffect;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> SetByCallerHealthAttributesEffect;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> OmegaWarmAttributeEffect;

	UFUNCTION(BlueprintImplementableEvent)
	void LevelBeginPlay(AOmochaPlayerState* OmochaPlayerState);

	UFUNCTION(BlueprintCallable)
	void SetupHealthBar();

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USpringArmComponent> MinimapSpringArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<USceneCaptureComponent2D> MinimapCamera;

	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	TObjectPtr<UMaterial> MinimapMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bEnableCameraPanning = true;

	/* Multicast Handle */
	virtual void MulticastHandleDeath_Implementation(const FVector& DeathImpulse) override;
	virtual void MulticastHandleRebirth_Implementation() override;

	void DisableCollisionOnDeath() override;

	void RestoreCollisionOnRebirth() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EPlayerCharter CharacterType = EPlayerCharter::None;
	
private:
	virtual void InitAbilityActorInfo() override;

	// Look System
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Look System", meta = (AllowPrivateAccess = "true"))
	float CurrentSpineRotation = 0.0f;

	// Animation 
	float MovementSpeed = 0.0f;
	float MovementDirection = 0.0f;
	bool bIsMoving = false;

	UFUNCTION(Server, Reliable)
	void ServerSetSpineRotation(float Rotation);

	void UpdateAnimationData(float DeltaTime);
	virtual void HitReactTagChanged(FGameplayTag CallbackTag, int32 NewCount) override;

	void LoadProgress();

	UPROPERTY()
	TObjectPtr<UTextureRenderTarget2D> MinimapRenderTarget;

	void InitMinimapMaterial();

	bool bIsHealthBarSetupCompleted = false;

	void CompleteRevival();
	FTimerHandle ReviveTimerHandle;
};
