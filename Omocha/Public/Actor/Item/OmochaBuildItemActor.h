// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "OmochaBuildItemActor.generated.h"

class USphereComponent;

UCLASS()
class OMOCHA_API AOmochaBuildItemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AOmochaBuildItemActor();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 CardAmount = 3;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BuildTags")
	FGameplayTagContainer RustyBuildTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BuildTags")
	FGameplayTagContainer VulcanBuildTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BuildTags")
	FGameplayTagContainer ZenithBuildTags;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> Sphere;

	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


};
