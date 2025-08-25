#include "AbilitySystem/Abilities/MoveGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AbilityTasks/OmochaMouseHitTask.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "OmochaGameplayTags.h"
#include "AbilitySystem/OmochaAbilitySystemLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Omocha/Omocha.h"

UMoveGameplayAbility::UMoveGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	InputGameplayTag = FOmochaGameplayTags::Get().InputTag_Ability_RMB;
	FGameplayTagContainer Tags;
	Tags.AddTag(FOmochaGameplayTags::Get().Ability_Normal_Skill_RMB);
	SetAssetTags(Tags);

	bHasBlueprintActivate = true;
	bHasBlueprintActivateFromEvent = true;

	DrawDebugType = EDrawDebugTrace::None;
}

void UMoveGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UMoveGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo,
                                      bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

FVector UMoveGameplayAbility::GetValidMoveLocation(const FVector& DesiredLocation) const
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character) {
		return DesiredLocation;
	}

	FVector StartLocation = Character->GetActorLocation();

	UCapsuleComponent* Capsule = Character->GetCapsuleComponent();
	const float CapsuleRadius = Capsule->GetScaledCapsuleRadius();
	const float CapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	TArray<AActor*> ActorsToIgnore = {Character};
	FVector AdjustedLocation = UOmochaAbilitySystemLibrary::AdjustLocationForWalls(
		this,
		StartLocation,
		DesiredLocation,
		CapsuleRadius,
		CapsuleHalfHeight,
		ActorsToIgnore,
		false
	);

	FVector Direction = (AdjustedLocation - StartLocation).GetSafeNormal();
	float DesiredDistance = FVector::Dist(StartLocation, AdjustedLocation);

	// 최대/최소 거리 제한 적용
	float ClampedDistance = FMath::Min(DesiredDistance, GetSkillRangeData().MaxRange);
	FVector ClampedLocation = StartLocation + (Direction * ClampedDistance);

	// 지면에 위치 조정 (라인트레이스로 바닥 찾기)
	FVector ValidLocation = FindGroundLocation(ClampedLocation);

	// 충돌 검사
	if (IsLocationValid(ValidLocation)) {
		return ValidLocation;
	}

	// 유효하지 않다면 시작 위치 반환
	return StartLocation;
}

bool UMoveGameplayAbility::IsLocationValid(const FVector& Location) const
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character) {
		return false;
	}

	// 캐릭터의 캡슐 컴포넌트 크기 가져오기
	UCapsuleComponent* CapsuleComp = Character->GetCapsuleComponent();
	if (!CapsuleComp) {
		return false;
	}

	float CapsuleRadius = CapsuleComp->GetScaledCapsuleRadius();
	float CapsuleHalfHeight = CapsuleComp->GetScaledCapsuleHalfHeight();

	// 충돌 검사 파라미터 설정
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Character);
	QueryParams.bTraceComplex = false;


	FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);

	bool bHit = GetWorld()->OverlapBlockingTestByChannel(
		Location + FVector(0, 0, 10),
		FQuat::Identity,
		ECC_MoveTrace,
		CapsuleShape,
		QueryParams
	);

	if (DrawDebugType != EDrawDebugTrace::None) {
		FColor DebugColor = bHit ? FColor::Red : FColor::Green;
		DrawDebugCapsule(GetWorld(), Location + FVector(0, 0, 10), CapsuleHalfHeight, CapsuleRadius,
		                 FQuat::Identity, DebugColor, false, 1.0f, 0, 1.0f);
	}

	return !bHit; // 충돌이 없으면 유효한 위치
}

float UMoveGameplayAbility::CalculateActualDistance(const FVector& TargetLocation) const
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character) {
		return 0.0f;
	}

	return FVector::Dist(Character->GetActorLocation(), TargetLocation);
}

FVector UMoveGameplayAbility::FindGroundLocation(const FVector& Location) const
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character) {
		return Location;
	}

	// 위에서 아래로 라인 트레이스
	FVector TraceStart = Location + FVector(0, 0, 500.0f);
	FVector TraceEnd = Location - FVector(0, 0, 500.0f);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Character);
	QueryParams.bTraceComplex = true;

	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECC_WorldStatic,
		QueryParams
	);

	if (DrawDebugType != EDrawDebugTrace::None) {
		FColor LineColor = bHit ? FColor::Yellow : FColor::Orange;
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, LineColor, false, 1.0f, 0, 2.0f);
	}
	if (bHit) {
		if (DrawDebugType != EDrawDebugTrace::None) {
			DrawDebugSphere(GetWorld(), HitResult.Location, 10.0f, 8, FColor::Red, false, 1.0f);
		}
		// 캐릭터의 캡슐 높이만큼 올려서 반환
		UCapsuleComponent* CapsuleComp = Character->GetCapsuleComponent();
		float CapsuleHalfHeight = CapsuleComp ? CapsuleComp->GetScaledCapsuleHalfHeight() : 88.0f;
		FVector GroundLocation = HitResult.Location + FVector(0, 0, CapsuleHalfHeight);

		UE_LOG(LogTemp, Log, TEXT("FindGroundLocation: Hit at %s, Adjusted to %s"),
		       *HitResult.Location.ToString(), *GroundLocation.ToString());

		return GroundLocation;
	}

	UE_LOG(LogTemp, Warning, TEXT("FindGroundLocation: No ground found, using original location %s"),
	       *Location.ToString());

	// 히트가 없으면 원래 위치 반환
	return Location;
}
