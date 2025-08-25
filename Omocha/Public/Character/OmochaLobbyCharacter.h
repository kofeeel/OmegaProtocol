// Copyright It's My Turn

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OmochaLobbyCharacter.generated.h"

class UDataTable;
class UStaticMeshComponent;

UCLASS()
class OMOCHA_API AOmochaLobbyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AOmochaLobbyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data")
	TObjectPtr<UDataTable> CharacterLobbyDataTable;

	UFUNCTION(BlueprintImplementableEvent, Category = "Lobby")
	void ChooseCharacter(ECharacterType Type);

	UFUNCTION(BlueprintImplementableEvent, Category = "Lobby")
	void GetReady(bool bReady);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Lobby|Visuals")
	void Multicast_UpdateVisuals(USkeletalMesh* NewMesh, TSubclassOf<UAnimInstance> NewAnimBP);

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Lobby")
	bool bIsReady_Replicated;

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void Multicast_StartRunCinematic();

	UPROPERTY(Replicated)
	ECharacterType MyCharacterType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> HeadStaticMeshComponent;

	UPROPERTY(EditAnywhere, Category = "Lobby|Cinematic")
	float CinematicRunSpeed = 600.f;

	UPROPERTY(EditAnywhere, Category = "Lobby|Cinematic")
	float TransformDelay = 2.5f;

private:
	bool bIsCinematicRunning = false;


	FTimerHandle TransformTimerHandle;

	void TriggerTransformEffect();
};
