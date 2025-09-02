// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "Actor/OmochaProjectile.h"
#include "Hook.generated.h"

/**
 * 
 */
UCLASS()
class OMOCHA_API AHook : public AOmochaProjectile
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void StartOutGoingTimeLine();

	UPROPERTY(BlueprintReadOnly)	TObjectPtr<AActor> ReturnToActor;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> GrabToActor;
	
	UPROPERTY(BlueprintReadOnly)
	FVector TargetLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	float Distance = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float PaddingDistance = 0.f;
	
	UPROPERTY(BlueprintReadOnly)
	bool bGrab = false;

	UPROPERTY(BlueprintReadOnly)
	bool bCanGrabWallsAndItems = false;
	
protected:
	virtual void BeginPlay() override; 
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;
};
