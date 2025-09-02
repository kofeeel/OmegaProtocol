// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/OmochaMouseHitTask.h"

#include "AbilitySystemComponent.h"
#include "Omocha/Omocha.h"
#include "Player/OmochaPlayerController.h"

UOmochaMouseHitTask* UOmochaMouseHitTask::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UOmochaMouseHitTask* MyObj = NewAbilityTask<UOmochaMouseHitTask>(OwningAbility);
	MyObj->bUseVisibilityTrace = false;
	return MyObj;
}

UOmochaMouseHitTask* UOmochaMouseHitTask::CreateTargetDataUnderMouseVisibility(UGameplayAbility* OwningAbility)
{
	UOmochaMouseHitTask* MyObj = NewAbilityTask<UOmochaMouseHitTask>(OwningAbility);
	MyObj->bUseVisibilityTrace = true;
	return MyObj;
}

void UOmochaMouseHitTask::Activate()
{
	if (!AbilitySystemComponent.IsValid())
    {
        return;
    }

    const bool bIsServer = Ability->GetOwningActorFromActorInfo()->HasAuthority();

    if (bIsServer)
    {
        if (Ability->GetCurrentActorInfo()->IsLocallyControlled())
        {
            const FGameplayAbilityTargetDataHandle DataHandle = MakeTargetDataUnderMouse();
            if (ShouldBroadcastAbilityTaskDelegates())
            {
                ValidData.Broadcast(DataHandle);
            }
        }
        else
        {
            const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
            const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
            AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(
                this, &UOmochaMouseHitTask::OnHitDataReplicatedCallback);

            const bool bCalledDelegate = AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
            if (!bCalledDelegate)
            {
                SetWaitingOnRemotePlayerData();
            }
        }
    }
    else
    {
        FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());

        const FGameplayAbilityTargetDataHandle DataHandle = MakeTargetDataUnderMouse();
        
        AbilitySystemComponent->ServerSetReplicatedTargetData(
            GetAbilitySpecHandle(),
            GetActivationPredictionKey(),
            DataHandle,
            FGameplayTag(),
            AbilitySystemComponent->ScopedPredictionKey);

        if (ShouldBroadcastAbilityTaskDelegates())
        {
            ValidData.Broadcast(DataHandle);
        }
    }
}

void UOmochaMouseHitTask::SendMouseCursorData()
{
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());

	const APlayerController* PlayerController = Ability->GetCurrentActorInfo()->PlayerController.Get();

	if (PlayerController == nullptr)
	{
		return;
	}
	
	FHitResult CursorHit;
	PlayerController->GetHitResultUnderCursor(ECC_MouseTrace, false, CursorHit);

	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;
	DataHandle.Add(Data);

	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

void UOmochaMouseHitTask::SendMouseCursorDataVisibility()
{
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());

	const APlayerController* PlayerController = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;

	PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;
	DataHandle.Add(Data);

	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

void UOmochaMouseHitTask::OnHitDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
                                                      FGameplayTag ActivationTag) const
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

FGameplayAbilityTargetDataHandle UOmochaMouseHitTask::MakeTargetDataUnderMouse() const
{
	const APlayerController* PlayerController = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
    
	PlayerController->GetHitResultUnderCursor(bUseVisibilityTrace ? ECC_Visibility : ECC_MouseTrace, false, CursorHit);

	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;
	DataHandle.Add(Data);

	return DataHandle;
}
