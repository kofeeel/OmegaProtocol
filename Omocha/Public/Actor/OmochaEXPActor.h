// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OmochaEXPActor.generated.h"

UCLASS()
class OMOCHA_API AOmochaEXPActor : public AActor
{
	GENERATED_BODY()

public:
	AOmochaEXPActor();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* OrbMesh;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	AActor* TargetPlayer = nullptr;
	
	UPROPERTY(EditAnywhere)
	float HomingSpeed = 600.0f;

	UPROPERTY(EditAnywhere)
	float LifeSpan = 7.5f;

	UPROPERTY(EditAnywhere)
	float CollectionDistance = 80.0f;

	UPROPERTY(EditAnywhere)
	float CircleRadius = 120.0f;

	UPROPERTY(EditAnywhere)
	float CircleSpeed = 180.0f;

	UPROPERTY(EditAnywhere)
	float DirectHomingDistance = 200.0f;

	float CurrentAngle = 0.0f;

	FString CurrentLevelName; 

public:
	void InitializeXPOrb(AActor* InTargetPlayer);

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                    bool bFromSweep, const FHitResult& SweepResult);

	void MoveTowardsTarget(float DeltaTime);
};
