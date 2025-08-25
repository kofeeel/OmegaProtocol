// Copyright It's My Turn


#include "Game/OmochaBaseGameMode.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "OmochaGameplayTags.h"
#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "AbilitySystem/OmochaAbilitySystemLibrary.h"
#include "AbilitySystem/OmochaAttributeSet.h"
#include "AbilitySystem/Abilities/OmochaGameplayAbility.h"
#include "Character/OmochaPlayerCharacter.h"
#include "Component/OmochaWeaponComponent.h"
#include "Game/OmochaGameInstance.h"
#include "GameFramework/GameSession.h"
#include "Player/OmochaPlayerState.h"
#include "Player/OmochaPlayerController.h"


AOmochaBaseGameMode::AOmochaBaseGameMode()
{
	bUseSeamlessTravel = false;
}

void AOmochaBaseGameMode::SavePlayerInformation(APlayerState* PS, UAttributeSet* AS, UAbilitySystemComponent* ASC)
{
	if (!IsValid(PS) || !IsValid(AS) || !IsValid(ASC)) {
		return;
	}
	if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance())) {
		SavePlayerState(PS, AS, ASC, OmochaGameInstance);
		SavePlayerAttributes(PS, AS, OmochaGameInstance);
		SavePlayerAbilities(PS, ASC, OmochaGameInstance);
		SavePlayerWeapon(PS, ASC, OmochaGameInstance);
	}
}

void AOmochaBaseGameMode::SavePlayerAttributes(const APlayerState* PS, const UAttributeSet* AS,
                                               UOmochaGameInstance* OmochaGameInstance)
{
	if (const UOmochaAttributeSet* OmochaAS = Cast<UOmochaAttributeSet>(AS)) {
		if (!PS || !OmochaAS || !OmochaGameInstance) {
			return;
		}
		FSavedAttributes CurrentAttributes;

		CurrentAttributes.Resilience =  OmochaAS->Health.GetCurrentValue() / OmochaAS->MaxHealth.GetCurrentValue();
		CurrentAttributes.AttackDamage = OmochaAS->AttackDamage.GetBaseValue();
		CurrentAttributes.SkillDamage = OmochaAS->SkillDamage.GetBaseValue();
		CurrentAttributes.Shield = OmochaAS->Shield.GetBaseValue();
		CurrentAttributes.MoveSpeed = OmochaAS->MoveSpeed.GetBaseValue();
		CurrentAttributes.AttackSpeed = OmochaAS->AttackSpeed.GetBaseValue();
		CurrentAttributes.CoolDownReduction = OmochaAS->CoolDownReduction.GetBaseValue();
		CurrentAttributes.AttackRange = OmochaAS->AttackRange.GetBaseValue();
		CurrentAttributes.Level = OmochaAS->Level.GetBaseValue();
		CurrentAttributes.CriticalChance = OmochaAS->CriticalChance.GetBaseValue();
		CurrentAttributes.CriticalDamage = OmochaAS->CriticalDamage.GetBaseValue();
		CurrentAttributes.DodgeChance = OmochaAS->DodgeChance.GetBaseValue();
		CurrentAttributes.ChargeLevel = OmochaAS->ChargeLevel.GetBaseValue();
		CurrentAttributes.MaxHealth = OmochaAS->MaxHealth.GetBaseValue();
		CurrentAttributes.MaxOmega = OmochaAS->MaxOmega.GetBaseValue();
		CurrentAttributes.MaxMoveSpeed = OmochaAS->MaxMoveSpeed.GetBaseValue();
		CurrentAttributes.MaxAttackSpeed = OmochaAS->MaxAttackSpeed.GetBaseValue();
		CurrentAttributes.MaxCoolDownReduction = OmochaAS->MaxCoolDownReduction.GetBaseValue();
		CurrentAttributes.MaxLevel = OmochaAS->MaxLevel.GetBaseValue();
		CurrentAttributes.KnockbackResistance = OmochaAS->KnockbackResistance.GetBaseValue();
		CurrentAttributes.LifeSteal = OmochaAS->LifeSteal.GetBaseValue();
		CurrentAttributes.ChainCount = OmochaAS->ChainCount.GetBaseValue();
		CurrentAttributes.ChargingTimeMultiplier = OmochaAS->ChargingTimeMultiplier.GetBaseValue();
		CurrentAttributes.CanRotateWhileCharging = OmochaAS->CanRotateWhileCharging.GetBaseValue();
		CurrentAttributes.HasBuffRadius = OmochaAS->HasBuffRadius.GetBaseValue();
		CurrentAttributes.BuffRadiusMultiplier = OmochaAS->BuffRadiusMultiplier.GetBaseValue();
		CurrentAttributes.UltimateBezierCount = OmochaAS->UltimateBezierCount.GetBaseValue();
		CurrentAttributes.HookSizeMultiplier = OmochaAS->HookSizeMultiplier.GetBaseValue();
		CurrentAttributes.HookSpeedMultiplier = OmochaAS->HookSpeedMultiplier.GetBaseValue();
		CurrentAttributes.UltimateGravityPull = OmochaAS->UltimateGravityPull.GetBaseValue();
		CurrentAttributes.AttackPenetrationCount = OmochaAS->AttackPenetrationCount.GetBaseValue();
		CurrentAttributes.MovementSkillRangeMultiplier = OmochaAS->MovementSkillRangeMultiplier.GetBaseValue();
		CurrentAttributes.AttackProjectileSpeedMultiplier = OmochaAS->AttackProjectileSpeedMultiplier.GetBaseValue();
		CurrentAttributes.ShieldMaxHealth = OmochaAS->ShieldMaxHealth.GetBaseValue();

		if (CurrentAttributes.MaxHealth != 0) {
			CurrentAttributes.Health = OmochaAS->Health.GetCurrentValue() / OmochaAS->MaxHealth.GetCurrentValue();
			if (CurrentAttributes.Health < 0.1f) {
				CurrentAttributes.Health = 0.1f;
			}
		}

		OmochaGameInstance->SavePlayerAttributes(UOmochaGameInstance::GetUniqueId(PS), CurrentAttributes);
	}
}

void AOmochaBaseGameMode::SavePlayerAbilities(APlayerState* PS, UAbilitySystemComponent* ASC,
                                              UOmochaGameInstance* OmochaGameInstance) const
{
	OmochaGameInstance->EmptyPlayerAbilities(UOmochaGameInstance::GetUniqueId(PS));

	if (UOmochaAbilitySystemComponent* OmochaASC = Cast<UOmochaAbilitySystemComponent>(ASC)) {
		FForEachAbility SaveAbilityDelegate;
		SaveAbilityDelegate.BindLambda(
			[this, OmochaGameInstance, PS](const FGameplayAbilitySpec& AbilitySpec)
			{
				const FGameplayTag AbilityTag = UOmochaAbilitySystemLibrary::GetAbilityTagFromSpec(AbilitySpec);
				FAbilityInfo Info = OmochaGameInstance->AbilityInfo->FindAbilityInfoForTag(AbilityTag);
				FSavedAbility SavedAbility;
				SavedAbility.GameplayAbility = Info.Ability;
				SavedAbility.AbilityTag = AbilityTag;
				OmochaGameInstance->SavePlayerAbility(UOmochaGameInstance::GetUniqueId(PS), SavedAbility);
			});
		OmochaASC->ForEachAbility(SaveAbilityDelegate);
	}
}

void AOmochaBaseGameMode::SavePlayerState(APlayerState* PS, const UAttributeSet* AS, UAbilitySystemComponent* ASC,
                                          UOmochaGameInstance* OmochaGameInstance)
{
	if (AOmochaPlayerCharacter* Player = Cast<AOmochaPlayerCharacter>(ASC->GetAvatarActor())) {
		const FString& PlayerId = UOmochaGameInstance::GetUniqueId(PS);
		OmochaGameInstance->SavePlayerState(PlayerId, Player->PlayerState);
		if (Player->PlayerState.MatchesTagExact(FOmochaGameplayTags::Get().State_Omega))
		{
			if (const UOmochaAttributeSet* OmochaAS = Cast<UOmochaAttributeSet>(AS))
			{
				OmochaGameInstance->SetOmega(OmochaAS->Omega.GetCurrentValue());
			}
		}
	}
}

void AOmochaBaseGameMode::SavePlayerWeapon(APlayerState* PS, UAbilitySystemComponent* ASC,
	UOmochaGameInstance* OmochaGameInstance)
{
	if (AOmochaPlayerCharacter* PlayerCharacter = Cast<AOmochaPlayerCharacter>(ASC->GetAvatarActor()))
	{
		if (PlayerCharacter->WeaponComponent)
		{
			const TSubclassOf<AWeaponPickupActor> PickupClass = PlayerCharacter->WeaponComponent->GetEquippedWeaponPickupClass();
			const FString& PlayerId = UOmochaGameInstance::GetUniqueId(PS);
			OmochaGameInstance->SavePlayerWeapon(PlayerId, PickupClass);
		}
	}
}

void AOmochaBaseGameMode::LoadPlayerInformation(APlayerState* PS, UAbilitySystemComponent* ASC)
{
	if (!IsValid(PS) || !IsValid(ASC)) {
		return;
	}
	if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance())) {
		LoadPlayerState(PS, ASC, OmochaGameInstance);
		LoadPlayerAttributes(PS, ASC, OmochaGameInstance);
		LoadPlayerAbilities(PS, ASC, OmochaGameInstance);
	}
}

void AOmochaBaseGameMode::LoadPlayerAttributes(APlayerState* PS, UAbilitySystemComponent* ASC,
                                               UOmochaGameInstance* OmochaGameInstance)
{
	const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();
	const AActor* SourceAvatarActor = ASC->GetAvatarActor();
	const FSavedAttributes& SavedAttributes = OmochaGameInstance->GetPlayerAttributes(
		UOmochaGameInstance::GetUniqueId(PS));

	FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceAvatarActor);

	const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(AttributesSetByCallerEffect, 1.f,
	                                                                   EffectContextHandle);

	const float Health = SavedAttributes.Health * SavedAttributes.MaxHealth;
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Vital_MaxHealth,
	                                                              SavedAttributes.MaxHealth);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Vital_Health,
	                                                              Health);
	
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Vital_MaxOmega,
																  SavedAttributes.MaxOmega);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Current_Resilience, SavedAttributes.Resilience);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Current_AttackDamage, SavedAttributes.AttackDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Current_SkillDamage, SavedAttributes.SkillDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Current_Shield,
	                                                              SavedAttributes.Shield);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Current_MoveSpeed,
	                                                              SavedAttributes.MoveSpeed);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Current_AttackSpeed, SavedAttributes.AttackSpeed);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Current_CoolDownReduction, SavedAttributes.CoolDownReduction);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Current_AttackRange, SavedAttributes.AttackRange);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Current_Level,
	                                                              SavedAttributes.Level);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Current_CriticalChance, SavedAttributes.CriticalChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Current_CriticalDamage, SavedAttributes.CriticalDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Current_DodgeChance, SavedAttributes.DodgeChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Current_ChargeLevel, SavedAttributes.ChargeLevel);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Max_MaxMoveSpeed,
	                                                              SavedAttributes.MaxMoveSpeed);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Max_MaxAttackSpeed, SavedAttributes.MaxAttackSpeed);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Max_MaxCoolDownReduction, SavedAttributes.MaxCoolDownReduction);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Max_MaxLevel,
	                                                              SavedAttributes.MaxLevel);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Current_KnockbackResistance, SavedAttributes.KnockbackResistance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Current_LifeSteal,
	                                                              SavedAttributes.LifeSteal);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Character_Zenith_ChainCount, SavedAttributes.ChainCount);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Character_Zenith_ChargingTimeMultiplier,
		SavedAttributes.ChargingTimeMultiplier);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Character_Zenith_CanRotateWhileCharging,
		SavedAttributes.CanRotateWhileCharging);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Character_Vulcan_HasBuffRadius, SavedAttributes.HasBuffRadius);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Character_Vulcan_BuffRadiusMultiplier,
		SavedAttributes.BuffRadiusMultiplier);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Character_Vulcan_UltimateBezierCount, SavedAttributes.UltimateBezierCount);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Character_Rusty_HookSizeMultiplier, SavedAttributes.HookSizeMultiplier);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Character_Rusty_HookSpeedMultiplier, SavedAttributes.HookSpeedMultiplier);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Character_Rusty_UltimateGravityPull, SavedAttributes.UltimateGravityPull);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Character_Common_AttackPenetrationCount,
		SavedAttributes.AttackPenetrationCount);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Character_Common_MovementSkillRangeMultiplier,
		SavedAttributes.MovementSkillRangeMultiplier);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Character_Common_AttackProjectileSpeedMultiplier,
		SavedAttributes.AttackProjectileSpeedMultiplier);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Character_Rusty_ShieldMaxHealth, SavedAttributes.ShieldMaxHealth);
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
}

void AOmochaBaseGameMode::LoadPlayerAbilities(APlayerState* PS, UAbilitySystemComponent* ASC,
                                              UOmochaGameInstance* OmochaGameInstance)
{
	const TArray<FSavedAbility>& SavedAbilities = OmochaGameInstance->GetPlayerAbilities(
		UOmochaGameInstance::GetUniqueId(PS));
	for (auto Ability : SavedAbilities) {
		const TSubclassOf<UGameplayAbility> LoadedAbilityClass = Ability.GameplayAbility;
		FGameplayAbilitySpec LoadedAbilitySpec = FGameplayAbilitySpec(LoadedAbilityClass, 1.f);
		if (UOmochaGameplayAbility* LoadedAbility = Cast<UOmochaGameplayAbility>(LoadedAbilitySpec.Ability)) {
			LoadedAbilitySpec.GetDynamicSpecSourceTags().AddTag(LoadedAbility->InputGameplayTag);
			ASC->GiveAbility(LoadedAbilitySpec);
		}
	}
}

void AOmochaBaseGameMode::LoadPlayerState(APlayerState* PS, UAbilitySystemComponent* ASC,
                                          UOmochaGameInstance* OmochaGameInstance)
{
	if (AOmochaPlayerCharacter* Player = Cast<AOmochaPlayerCharacter>(ASC->GetAvatarActor())) {
		const FString& PlayerId = UOmochaGameInstance::GetUniqueId(PS);
		Player->LoadState = OmochaGameInstance->GetPlayerState(PlayerId);
	}
}

void AOmochaBaseGameMode::TravelToMap(bool InbSave, bool InbInit)
{
	bSave = InbSave;
	bInit = InbInit;

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(TravelTimerHandle);
	TimerManager.SetTimer(TravelTimerHandle, this, &AOmochaBaseGameMode::TravelNextLevel, TravelTime, false);
	GetPrepareForNextLevel(true);
}

void AOmochaBaseGameMode::SetNextMap(const FString& MapName)
{
	TravelToWorld = FString::Format(TEXT("/Game/Maps/{0}"), {MapName});
}

void AOmochaBaseGameMode::CancelTravel()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(TravelTimerHandle);
	GetPrepareForNextLevel(false);
}

void AOmochaBaseGameMode::KickPlayer(int32 PlayerNum) const
{
	if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance())) {
		APlayerController* KickPlayer;
		switch (PlayerNum) {
		case 0:
			KickPlayer = Cast<APlayerController>(OmochaGameInstance->Player1.PlayerController);
			break;
		case 1:
			KickPlayer = Cast<APlayerController>(OmochaGameInstance->Player2.PlayerController);
			break;
		case 2:
			KickPlayer = Cast<APlayerController>(OmochaGameInstance->Player3.PlayerController);
			break;
		default:
			return;
		}
		KickPlayerWithReason(KickPlayer, FString("You're kicked"));
	}
}

void AOmochaBaseGameMode::SendChatMessage(const FString& Message) const
{
	if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance()))
	{
		OmochaGameInstance->AllPlayerBroadcastChatMessage(Message);
	}
}

void AOmochaBaseGameMode::KickPlayerWithReason(APlayerController* PlayerToKick, const FString& KickReason) const
{
	if (PlayerToKick) {
		if (GameSession) {
			GameSession->KickPlayer(PlayerToKick, FText::FromString(KickReason));
		}
	}
}

void AOmochaBaseGameMode::GetPrepareForNextLevel(bool bReady)
{
	if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance())) {
		OmochaGameInstance->ForAlarmPrepareForNextLevel(bReady);
	}
}

void AOmochaBaseGameMode::TravelNextLevel()
{
	if (!HasAuthority()) {
		return;
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It) {
		if (AOmochaPlayerController* PC = Cast<AOmochaPlayerController>(It->Get())) {
			PC->Client_StartPreTravelEffects(PreTravelFadeDuration);
		}
	}

	FTimerHandle FinalTravelTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(FinalTravelTimerHandle, this, &AOmochaBaseGameMode::FinalizeTravel,
	                                       PreTravelFadeDuration, false);
}

void AOmochaBaseGameMode::FinalizeTravel()
{
	if (!HasAuthority()) {
		return;
	}

	if (bSave) {
		SaveEveryPlayerCharacter();
	}
	if (bInit) {
		if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance())) {
			OmochaGameInstance->InitializeAllOfPlayerInfos();
		}
	}

	if (UWorld* World = GetWorld()) {
		World->ServerTravel(TravelToWorld);
	}
}


void AOmochaBaseGameMode::SaveEveryPlayerCharacter()
{
	if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance())) {
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It) {
			if (APlayerController* PlayerController = It->Get()) {
				if (AOmochaPlayerState* PS = PlayerController->GetPlayerState<AOmochaPlayerState>()) {
					SavePlayerInformation(PS, PS->GetAttributeSet(), PS->GetAbilitySystemComponent());
				}
			}
		}
	}
}
