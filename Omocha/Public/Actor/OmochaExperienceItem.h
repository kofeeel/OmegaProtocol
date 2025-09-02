// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "Actor/OmochaEffectActor.h"
#include "OmochaExperienceItem.generated.h"

/**
 * 
 */
UCLASS()
class OMOCHA_API AOmochaExperienceItem : public AOmochaEffectActor
{
	GENERATED_BODY()

public:
	AOmochaExperienceItem();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Experience")
	int32 ExperienceAmount = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Experience")
	float CollectionRange = 300.0f;

	UPROPERTY(EditAnywhere, BLueprintReadOnly, Category = "Experience")
	float HomingSpeed = 500.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Experience")
	bool bIsHoming = false;

	UPROPERTY(BlueprintReadWrite, Category = "Experience")
	TObjectPtr<AActor> HomingTarget = nullptr;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void OnOverlap(AActor* TargetActor) override;

protected:
	void CheckForPlayers();

	void UpdateHoming(float DeltaTime);

	void GiveExperience(AActor* Player);


	UFUNCTION()
	void OnHomingRangeEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                        const FHitResult& SweepResult);

private:
	FTimerHandle PlayerDetectionTimer;

	float DetectionInterval = 0.1f;
};
