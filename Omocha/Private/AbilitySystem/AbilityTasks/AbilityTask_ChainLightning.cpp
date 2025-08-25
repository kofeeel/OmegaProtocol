// Copyright It's My Turn

#include "AbilitySystem/AbilityTasks/AbilityTask_ChainLightning.h"
#include "AbilitySystem/OmochaAbilitySystemLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/OverlapResult.h"

UAbilityTask_ChainLightning* UAbilityTask_ChainLightning::CreateChainLightningTask(
	UGameplayAbility* OwningAbility, AActor* InitialTarget, int32 InMaxChainCount, float InSearchRadius)
{
	UAbilityTask_ChainLightning* MyObj = NewAbilityTask<UAbilityTask_ChainLightning>(OwningAbility);
	MyObj->InitialTarget = InitialTarget;
	MyObj->MaxChainCount = InMaxChainCount;
	MyObj->SearchRadius = InSearchRadius;
	return MyObj;
}

void UAbilityTask_ChainLightning::Activate()
{
	if (!Ability || !InitialTarget || !Ability->GetCurrentActorInfo()->IsNetAuthority())
	{
		EndTask();
		return;
	}
	FindNextTarget(InitialTarget);
}

void UAbilityTask_ChainLightning::FindNextTarget(AActor* CurrentTarget)
{
	TargetChain.Add(CurrentTarget);

	if (TargetChain.Num() >= MaxChainCount)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnTargetsDetermined.Broadcast(TargetChain);
		}
		EndTask();
		return;
	}

	AActor* NextTarget = nullptr;
	float MinDistanceSq = TNumericLimits<float>::Max();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActor());
	ActorsToIgnore.Append(TargetChain);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActors(ActorsToIgnore);

	TArray<FOverlapResult> OverlapResults;
	bool bHit = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		CurrentTarget->GetActorLocation(),
		FQuat::Identity,
		ECC_WorldDynamic,
		FCollisionShape::MakeSphere(SearchRadius),
		QueryParams
	);

	if (bHit)
	{
		for (const FOverlapResult& Result : OverlapResults)
		{
			AActor* PotentialTarget = Result.GetActor();
			if (PotentialTarget && UOmochaAbilitySystemLibrary::IsNotFriend(GetAvatarActor(), PotentialTarget))
			{
				const float DistanceSq = (CurrentTarget->GetActorLocation() - PotentialTarget->GetActorLocation()).
					SizeSquared();
				if (DistanceSq < MinDistanceSq)
				{
					MinDistanceSq = DistanceSq;
					NextTarget = PotentialTarget;
				}
			}
		}
	}

	if (NextTarget)
	{
		FindNextTarget(NextTarget);
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnTargetsDetermined.Broadcast(TargetChain);
		}
		EndTask();
	}
}
