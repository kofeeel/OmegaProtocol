

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Engine/CollisionProfile.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "WaitForMouseClickTask.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForMouseClickDelegate, const FGameplayAbilityTargetDataHandle&, Data);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseMovementDelegate, const FVector&, MouseWorldLocation);


UCLASS()
class OMOCHA_API UWaitForMouseClickTask : public UAbilityTask
{
	GENERATED_BODY()

public:
	UWaitForMouseClickTask(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable)
	FWaitForMouseClickDelegate OnValidClick;

	UPROPERTY(BlueprintAssignable)
	FMouseMovementDelegate OnMouseMoved;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks")
	static UWaitForMouseClickTask* WaitForMouseClickWithTracking(
		UGameplayAbility* OwningAbility, 
		bool bTraceComplex = false, 
		bool bShowDebugTrace = false,
		bool bEnableMouseTracking = true
	);

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEnded) override;

	void SetupInputBindings();
	void ClearInputBindings();

	UFUNCTION()
	void OnLeftMouseButtonPressed();

	void SendMouseClickData();
	
	void StartMouseTracking();
	void StopMouseTracking();
	void UpdateMousePosition();
	
	UPROPERTY()
	bool bTrackMouseMovement = false;

	UPROPERTY()
	FTimerHandle MouseTrackingTimer;

	UFUNCTION()
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag);

private:
	bool bTraceComplex;
	bool bShowDebugTrace;

	bool bHasInputBindings;
    
	UPROPERTY()
	TWeakObjectPtr<APlayerController> CachedPlayerController;
};