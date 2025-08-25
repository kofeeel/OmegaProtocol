// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmochaGrenade.h"
#include "GravityGrenade.generated.h"

UCLASS()
class OMOCHA_API AGravityGrenade : public AOmochaGrenade
{
	GENERATED_BODY()

public:
	AGravityGrenade();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                             const FHitResult& SweepResult) override;

	virtual void Multicast_HandleExplosion_Implementation(const FVector& ImpactLocation) override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gravity|Settings", meta = (AllowPrivateAccess = "true"))
	float GravityFieldDuration = 4.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gravity|Settings", meta = (AllowPrivateAccess = "true"))
	float GravityForce = 800.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gravity|Settings", meta = (AllowPrivateAccess = "true"))
	float GravityFieldRadius = 400.0f;

	float TickInterval = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gravity|Settings", meta = (AllowPrivateAccess = "true"))
	float DamageInterval = 0.1f;
	float CurrentDamageInterval = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gravity|Effects", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UParticleSystem> GravityFieldVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gravity|Effects", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> GravityFieldSFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gravity|Effects", meta = (AllowPrivateAccess = "true"))
	float GravityFieldEffectRadius = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gravity|Damage", meta = (AllowPrivateAccess = "true"))
	float GravityFieldDamagePerTick = 5.0f;

	FTimerHandle GravityFieldTimerHandle;
	FTimerHandle GravityFieldEndTimerHandle;

	UPROPERTY(Replicated)
	FVector GravityFieldCenter;

	UPROPERTY()
	class UParticleSystemComponent* GravityFieldComponent;

	UPROPERTY()
	TArray<AActor*> AffectedActors;

	// Timer
	UFUNCTION()
	void ApplyGravityField();

	UFUNCTION()
	void ApplyGravityFieldDamage();

	UFUNCTION()
	void EndGravityField();

	void StartGravityField();

	// Utility
	bool CanAffectActor(AActor* Actor) const;
	void ApplyGravityToActor(AActor* Actor);
};
