// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Character/OmochaPlayerCharacter.h"
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
	void InitializeBuildTags(AOmochaPlayerCharacter* PlayerCharacter);
	void FixedInitializeBuildTags(AOmochaPlayerCharacter* PlayerCharacter);

	bool Init = false;

	bool Closed = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	bool Fixed = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	bool Permanent = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	int32 CardAmount = 3;

	FSkillBuildInfos BuildOmegaData;
	FSkillBuildInfos BuildOmicronData;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BuildTags")
	FGameplayTagContainer RustyOmicronBuildTags;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BuildTags")
	FGameplayTagContainer RustyOmegaBuildTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BuildTags")
	FGameplayTagContainer VulcanOmicronBuildTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BuildTags")
	FGameplayTagContainer VulcanOmegaBuildTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BuildTags")
	FGameplayTagContainer ZenithOmicronBuildTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BuildTags")
	FGameplayTagContainer ZenithOmegaBuildTags;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> Sphere;

	TObjectPtr<AOmochaPlayerController> Player;

	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnPickedBuildCard();

private:
	void PickCards(UOmochaSkillBuildComponent* Component, FGameplayTagContainer& BuildTags, FSkillBuildInfos& BuildData);
	void ModifyBuildCardLevels(UOmochaSkillBuildComponent* Component, FSkillBuildInfos& BuildData);
};
