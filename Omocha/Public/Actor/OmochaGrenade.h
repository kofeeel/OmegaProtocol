#pragma once

#include "CoreMinimal.h"
#include "OmochaProjectile.h"
#include "OmochaGrenade.generated.h"

UCLASS()
class OMOCHA_API AOmochaGrenade : public AOmochaProjectile
{
	GENERATED_BODY()

public:
	AOmochaGrenade();

	void SetReplicatedRadiusScale(float value) { ReplicatedRadiusScale = value; }
	void SetProCessRadius(float value) { ProcessRadius = value; }

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                             const FHitResult& SweepResult) override;

	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable)
	virtual void Multicast_HandleExplosion(const FVector& ImpactLocation);

	virtual void Destroyed() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Grenade|Radius",
		meta = (AllowPrivateAccess = "true"))
	float Radius = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Grenade|Radius",
		meta = (AllowPrivateAccess = "true"))
	float ProcessRadius = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, ReplicatedUsing=OnRep_ReplicatedRadiusScale,
		Category = "Grenade|Radius", meta = (AllowPrivateAccess = "true"))
	float ReplicatedRadiusScale = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Grenade|Debug",
		meta = (AllowPrivateAccess = "true"))
	bool bShowDebug;

	UFUNCTION()
	void OnRep_ReplicatedRadiusScale();

	void Explode();

	virtual void StartDisappearSequence() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade|Radius", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UParticleSystem> ExplosionVFX;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade|Radius", meta = (AllowPrivateAccess = "true"))
	class UNiagaraSystem* NiagaraExplosionVFX;
};
