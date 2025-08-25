// Copyright It's My Turn


#include "AbilitySystem/Abilities/Enemy/OmochaEnemyTestAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "OmochaGameplayTags.h"
#include "AbilitySystem/OmochaAbilitySystemLibrary.h"
#include "AbilitySystem/OmochaAbilityTypes.h"

UOmochaEnemyTestAbility::UOmochaEnemyTestAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	FGameplayTagContainer Tags;
	Tags.AddTag(FOmochaGameplayTags::Get().Abilities_Type_Attack); 
	SetAssetTags(Tags);
	
	bHasBlueprintActivate = true;
	bHasBlueprintActivateFromEvent = true;
}

void UOmochaEnemyTestAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (bHasProcessedAttack) 
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UE_LOG(LogTemp, Warning, TEXT("Enemy Attack Ability Activated by BTT!"));
}

void UOmochaEnemyTestAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	bHasProcessedAttack = false;
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	UE_LOG(LogTemp, Warning, TEXT("Enemy Attack Ability Ended!"));
}

void UOmochaEnemyTestAbility::ApplyDamageToNearbyPlayers()
{
    UE_LOG(LogTemp, Warning, TEXT("ApplyDamageToNearbyPlayers 시작"));
    
    TArray<AActor*> NearbyPlayers;
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
    
    UOmochaAbilitySystemLibrary::GetLivePlayersWithinRadius(
       this,
       NearbyPlayers,
       ActorsToIgnore,
       150.0f,
       GetAvatarActorFromActorInfo()->GetActorLocation()
    );
    
    UE_LOG(LogTemp, Warning, TEXT("감지된 플레이어: %d명"), NearbyPlayers.Num());
    
    for (AActor* Player : NearbyPlayers)
    {
       if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Player))
       {
          UE_LOG(LogTemp, Warning, TEXT(" %s의 ASC 찾음"), *Player->GetName());
          
          FDamageEffectParams DamageParams = MakeDamageEffectParamsFromClassDefaults(Player);
          
          // 핵심 수정: TargetASC 명시적 설정!
          DamageParams.TargetAbilitySystemComponent = TargetASC;
          
          FGameplayEffectContextHandle Result = UOmochaAbilitySystemLibrary::ApplyDamageEffect(DamageParams);
       	
       }
       else
       {
          UE_LOG(LogTemp, Error, TEXT("%s의 ASC 없음"), *Player->GetName());
       }
    }
	
}
