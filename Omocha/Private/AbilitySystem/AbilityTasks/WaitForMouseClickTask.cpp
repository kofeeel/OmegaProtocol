#include "AbilitySystem/AbilityTasks/WaitForMouseClickTask.h"
#include "AbilitySystemComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "Omocha/Omocha.h"
#include "Player/OmochaPlayerController.h"

UWaitForMouseClickTask::UWaitForMouseClickTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTraceComplex = false;
	bShowDebugTrace = false;
	bHasInputBindings = false;
	CachedPlayerController = nullptr;
}

UWaitForMouseClickTask* UWaitForMouseClickTask::WaitForMouseClickWithTracking(UGameplayAbility* OwningAbility,
                                                                              bool bTraceComplex, bool bShowDebugTrace,
                                                                              bool bEnableMouseTracking)
{
	UWaitForMouseClickTask* MyObj = NewAbilityTask<UWaitForMouseClickTask>(OwningAbility);
	MyObj->bTraceComplex = bTraceComplex;
	MyObj->bShowDebugTrace = bShowDebugTrace;
	MyObj->bTrackMouseMovement = bEnableMouseTracking;
	return MyObj;
}

void UWaitForMouseClickTask::Activate()
{
	Super::Activate();

	if (!Ability || !Ability->GetCurrentActorInfo())
	{
		EndTask();
		return;
	}

	CachedPlayerController = Ability->GetCurrentActorInfo()->PlayerController.Get();
	if (!CachedPlayerController.IsValid())
	{
		EndTask();
		return;
	}

	if (Ability->GetCurrentActorInfo()->IsLocallyControlled())
	{
		SetupInputBindings();

		if (bTrackMouseMovement)
		{
			StartMouseTracking();
		}
	}
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();

		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey)
		                      .AddUObject(this, &UWaitForMouseClickTask::OnTargetDataReplicatedCallback);

		if (!AbilitySystemComponent.Get()->
		                            CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey))
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UWaitForMouseClickTask::OnDestroy(bool AbilityEnded)
{
	ClearInputBindings();
	StopMouseTracking();
	Super::OnDestroy(AbilityEnded);
}

void UWaitForMouseClickTask::SetupInputBindings()
{
	if (!CachedPlayerController.IsValid())
	{
		return;
	}

	UInputComponent* InputComp = CachedPlayerController->InputComponent;
	if (!InputComp)
	{
		return;
	}

	InputComp->BindKey(EKeys::LeftMouseButton, IE_Pressed, this,
	                   &UWaitForMouseClickTask::OnLeftMouseButtonPressed);
	bHasInputBindings = true;
}

void UWaitForMouseClickTask::ClearInputBindings()
{
	if (!bHasInputBindings || !CachedPlayerController.IsValid())
	{
		return;
	}

	bHasInputBindings = false;
}

void UWaitForMouseClickTask::OnLeftMouseButtonPressed()
{
	SendMouseClickData();
}

void UWaitForMouseClickTask::SendMouseClickData()
{
	if (!CachedPlayerController.IsValid())
	{
		return;
	}

	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());

	FHitResult HitResult;
	bool bHit = CachedPlayerController->GetHitResultUnderCursor(ECC_MouseTrace, bTraceComplex, HitResult);

	if (bShowDebugTrace && bHit)
	{
		DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 20.0f, 12, FColor::Green, false, 2.0f);
	}

	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = HitResult;
	DataHandle.Add(Data);

	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnValidClick.Broadcast(DataHandle);
	}
}

void UWaitForMouseClickTask::StartMouseTracking()
{
	if (!CachedPlayerController.IsValid())
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(
		MouseTrackingTimer,
		this,
		&UWaitForMouseClickTask::UpdateMousePosition,
		1.0f / 60.0f,
		true
	);
}

void UWaitForMouseClickTask::StopMouseTracking()
{
	if (MouseTrackingTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(MouseTrackingTimer);
		MouseTrackingTimer.Invalidate();
	}
}

void UWaitForMouseClickTask::UpdateMousePosition()
{
	if (!CachedPlayerController.IsValid())
	{
		return;
	}

	const AOmochaPlayerController* OmochaPC = Cast<AOmochaPlayerController>(CachedPlayerController.Get());
	if (!OmochaPC)
	{
		return;
	}

	FHitResult HitResult = OmochaPC->GetMouseCursorHit();

	if (HitResult.bBlockingHit && ShouldBroadcastAbilityTaskDelegates())
	{
		OnMouseMoved.Broadcast(HitResult.ImpactPoint);

		if (bShowDebugTrace)
		{
			DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 8, FColor::Yellow, false, 0.1f);
		}
	}
}

void UWaitForMouseClickTask::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data,
                                                            FGameplayTag ActivationTag)
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnValidClick.Broadcast(Data);
	}
}
