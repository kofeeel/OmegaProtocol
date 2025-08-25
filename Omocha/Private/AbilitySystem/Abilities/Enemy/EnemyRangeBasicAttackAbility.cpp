// Copyright It's My Turn


#include "AbilitySystem/Abilities/Enemy/EnemyRangeBasicAttackAbility.h"

#include "OmochaGameplayTags.h"
#include "Actor/OmochaProjectile.h"
#include "Character/OmochaEnemy.h"
#include "Kismet/GameplayStatics.h"

UEnemyRangeBasicAttackAbility::UEnemyRangeBasicAttackAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	FGameplayTagContainer Tags;
	Tags.AddTag(FOmochaGameplayTags::Get().Abilities_Type_Attack); 
	SetAssetTags(Tags);
	
	bHasBlueprintActivate = true;
	bHasBlueprintActivateFromEvent = true;
}

void UEnemyRangeBasicAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!ProjectileClass)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	GetPlayerLocation();
}

void UEnemyRangeBasicAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UEnemyRangeBasicAttackAbility::GetPlayerLocation()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("Player Pawn not found"));
	}
    
	// 거리 체크
	const FVector EnemyLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	const FVector PlayerLocation = PlayerPawn->GetActorLocation();
	const float Distance = FVector::Dist(EnemyLocation, PlayerLocation);
    
	if (Distance > MaxAttackRange)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyRangeBasicAttackAbility: 플레이어가 너무 멀음! 거리: %.1f"), Distance);
	}
	
	StartAttackSequence(PlayerLocation);
}
