// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/TraceGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/OmochaAbilitySystemLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "AbilitySystem/OmochaAbilityTypes.h"
#include "AbilitySystem/AbilityTasks/OmochaMouseHitTask.h"
#include "Engine/OverlapResult.h"

UTraceGameplayAbility::UTraceGameplayAbility()
{
	DrawDebugType = EDrawDebugTrace::None;
}

void UTraceGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UTraceGameplayAbility::ExecuteOverlapAndApplyDamage(const FVector& TargetLocation, float OverlapRadius)
{
	CurrentTraceCenter = TargetLocation;
	TArray<FHitResult> HitResults;
	const bool bHasHit = PerformOverlap(HitResults, TargetLocation, OverlapRadius);

	if (bHasHit)
	{
		ProcessHitResults(HitResults);
	}
	// EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

bool UTraceGameplayAbility::PerformOverlap(TArray<FHitResult>& OutHitResults, const FVector& TargetLocation,
                                           float OverlapRadius)
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return false;
	}

	FVector SocketLocation = GetLaunchLocation(Character);
	if (SocketLocation == FVector::ZeroVector)
	{
		SocketLocation = Character->GetActorLocation();
	}

	FVector Direction = (TargetLocation - SocketLocation);
	Direction.Z = 0.f;

	const float DistanceToTarget = Direction.Size();
	Direction.Normalize();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Character);

	bool bHitDetected = false;
	constexpr ECollisionChannel OverlapChannel = ECC_WorldDynamic;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActors(ActorsToIgnore);

	float Scale = Cast<UOmochaAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo())->SkillScale;

	switch (SkillRangeData.RangeType)
	{
	case ESkillRangeType::Circle:
		{
			const float OverlapLength = FMath::Min(DistanceToTarget, SkillRangeData.MaxRange);
			const FVector EndPoint = SocketLocation + (Direction * OverlapLength);

			TArray<FOverlapResult> OverlapResults;
			bHitDetected = GetWorld()->OverlapMultiByChannel(
				OverlapResults,
				EndPoint,
				FQuat::Identity,
				OverlapChannel,
				FCollisionShape::MakeSphere(SkillRangeData.Radius * Scale * OverlapRadius),
				QueryParams
			);

			if (bHitDetected)
			{
				for (const FOverlapResult& Overlap : OverlapResults)
				{
					OutHitResults.Add(FHitResult(Overlap.GetActor(), Overlap.GetComponent(), FVector(), FVector()));
				}
			}
			if (DrawDebugType != EDrawDebugTrace::None)
			{
				DrawDebugSphere(GetWorld(), EndPoint, SkillRangeData.Radius * Scale * OverlapRadius, 26, FColor::Red,
				                false,
				                DrawDebugDuration);
			}
			break;
		}

	case ESkillRangeType::Rectangle:
		{
			const float OverlapLength = SkillRangeData.MaxRange;
			const FVector StartPoint = SocketLocation + (Direction * OverlapStartForwardOffset);
			const FVector EndPoint = StartPoint + (Direction * OverlapLength);

			const FRotator OverlapRotation = Direction.Rotation();
			const FVector CenterPoint = (StartPoint + EndPoint) / 2.f;
			const float PathLength = (EndPoint - StartPoint).Size();
			const FVector OverlapBoxHalfExtents = FVector(PathLength / 2.f, SkillRangeData.Size.Y * Scale,
			                                              SkillRangeData.Size.Z * Scale);
			TArray<FOverlapResult> OverlapResults;

			bHitDetected = GetWorld()->OverlapMultiByChannel(
				OverlapResults, CenterPoint, OverlapRotation.Quaternion(), OverlapChannel,
				FCollisionShape::MakeBox(OverlapBoxHalfExtents), QueryParams
			);

			if (bHitDetected)
			{
				for (const FOverlapResult& Overlap : OverlapResults)
				{
					OutHitResults.Add(FHitResult(Overlap.GetActor(), Overlap.GetComponent(), FVector(), FVector()));
				}
			}
			if (DrawDebugType != EDrawDebugTrace::None)
			{
				DrawDebugBox(GetWorld(), CenterPoint, OverlapBoxHalfExtents, OverlapRotation.Quaternion(), FColor::Cyan,
				             false, DrawDebugDuration);
			}
			break;
		}

	case ESkillRangeType::Cone:
		{
			const float OverlapLength = SkillRangeData.MaxRange;
			const FVector StartPoint = SocketLocation + (Direction * OverlapStartForwardOffset);

			if (DrawDebugType != EDrawDebugTrace::None)
			{
				DrawDebugCone(GetWorld(), StartPoint, Direction, OverlapLength,
				              FMath::DegreesToRadians(SkillRangeData.Angle / 2.0f),
				              FMath::DegreesToRadians(SkillRangeData.Angle / 2.0f), 12, FColor::Red, false,
				              DrawDebugDuration);
			}

			TArray<FOverlapResult> OverlapResults;
			FCollisionShape SphereShapeForCone = FCollisionShape::MakeSphere(OverlapLength);

			bool bInitialHit = GetWorld()->OverlapMultiByChannel(
				OverlapResults, StartPoint, FQuat::Identity, OverlapChannel, SphereShapeForCone, QueryParams
			);

			if (bInitialHit)
			{
				for (const FOverlapResult& Overlap : OverlapResults)
				{
					if (!Overlap.GetActor())
					{
						continue;
					}

					FVector DirectionToTarget = Overlap.GetActor()->GetActorLocation() - StartPoint;
					if (DirectionToTarget.IsNearlyZero())
					{
						continue;
					}
					DirectionToTarget.Normalize();

					const float AngleRad = FMath::Acos(FVector::DotProduct(Direction, DirectionToTarget));
					const float AngleDeg = FMath::RadiansToDegrees(AngleRad);

					if (AngleDeg <= SkillRangeData.Angle / 2.0f)
					{
						OutHitResults.Add(FHitResult(Overlap.GetActor(), Overlap.GetComponent(), FVector(), FVector()));
						bHitDetected = true;
					}
				}
			}
			break;
		}

	default:
		bHitDetected = false;
		break;
	}

	return bHitDetected;
}

void UTraceGameplayAbility::ProcessHitResults(const TArray<FHitResult>& HitResults)
{
	TArray<AActor*> DamagedActors;

	for (const FHitResult& Hit : HitResults)
	{
		AActor* TargetActor = Hit.GetActor();
		if (!TargetActor)
		{
			continue;
		}

		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!TargetASC)
		{
			continue;
		}

		if (DamagedActors.Contains(TargetActor))
		{
			continue;
		}

		FDamageEffectParams DamageParams = MakeDamageEffectParamsFromClassDefaults(TargetActor);
		DamageParams.TargetAbilitySystemComponent = TargetASC;
		UOmochaAbilitySystemLibrary::ApplyDamageEffect(DamageParams);

		DamagedActors.Add(TargetActor);
	}
	if (bShouldApplyGravityPull)
	{
		ApplyGravityPull(HitResults);
	}
}

void UTraceGameplayAbility::ApplyGravityPull(const TArray<FHitResult>& HitResults)
{
	const FVector GravityCenter = CurrentTraceCenter;

	for (const FHitResult& Hit : HitResults)
	{
		ACharacter* TargetCharacter = Cast<ACharacter>(Hit.GetActor());
		if (TargetCharacter && TargetCharacter->GetCharacterMovement())
		{
			if (UOmochaAbilitySystemLibrary::IsNotFriend(GetAvatarActorFromActorInfo(), TargetCharacter))
			{
				const FVector Direction = (GravityCenter - TargetCharacter->GetActorLocation()).GetSafeNormal();
				TargetCharacter->LaunchCharacter(Direction * GravityPullForce, true, true);
			}
		}
	}
}
