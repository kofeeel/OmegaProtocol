#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/OmochaAbilityTypes.h"
#include "GameFramework/Actor.h"
#include "OmochaProjectile.generated.h"

class UNiagaraComponent;
class USphereComponent;
class UProjectileMovementComponent;
class UStaticMeshComponent;

UCLASS()
class OMOCHA_API AOmochaProjectile : public AActor
{
	GENERATED_BODY()

public:
	AOmochaProjectile();

	FDamageEffectParams DamageParams;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(BlueprintReadWrite, Category = "Projectile|Lifecycle")
	float MaxTravelDistance = 0.f;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                             const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable)
	virtual void Multicast_HandleImpact(const FVector& ImpactLocation);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* ProjectileEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* ProjectileNiagaraEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Lifecycle")
	float LifeSpan = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Impact Effects",
		meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UParticleSystem> ImpactVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Impact Effects",
		meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> ImpactSFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Piercing")
	bool bCanPierce = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile|Piercing",
		meta = (EditCondition = "bCanPierce", ClampMin = "0"))
	int32 MaxPierceCount = 0;

	bool bHasImpacted = false;

	virtual void StartDestroySequence();
	virtual void StartDisappearSequence();

	bool IsValidTarget(AActor* OtherActor);

	int32 RemainingPierceCount = 0;
	TArray<TObjectPtr<AActor>> HitActors;

	UPROPERTY(EditAnywhere, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	bool bCanFriendlyFire = false;

	virtual void Destroyed() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Lifecycle",
		meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UParticleSystem> DisappearVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Lifecycle",
		meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> DisappearSFX;

	UFUNCTION(BlueprintImplementableEvent, Category = "Projectile|Impact", meta = (AllowPrivateAccess = "true"))
	void OnExplode(const FVector& ExplosionLocation);

private:
	FVector SpawnLocation;
};
