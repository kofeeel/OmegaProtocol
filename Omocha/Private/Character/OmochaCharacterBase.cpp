// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/OmochaCharacterBase.h"

#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NiagaraComponent.h"
#include "OmochaGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/OmochaAttributeSet.h"
#include "DataAsset/OmochaAttributeData.h"
#include "Kismet/GameplayStatics.h"
#include "PaperSpriteComponent.h"

AOmochaCharacterBase::AOmochaCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);

	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	StunDebuffComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("StunDebuffComponent"));
	StunDebuffComponent->SetupAttachment(GetRootComponent());
	StunDebuffComponent->SetAutoActivate(false);

	BurnDebuffComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BurnDebuffComponent"));
	BurnDebuffComponent->SetupAttachment(GetRootComponent());
	BurnDebuffComponent->SetAutoActivate(false);

	ShockDebuffComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SlowDebuffComponent"));
	ShockDebuffComponent->SetupAttachment(GetRootComponent());
	ShockDebuffComponent->SetAutoActivate(false);

	MinimapIcon = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("MinimapIcon"));
	MinimapIcon->SetupAttachment(GetRootComponent());
	MinimapIcon->SetHiddenInGame(false);
	MinimapIcon->SetVisibleInSceneCaptureOnly(true);
	MinimapIcon->SetIsReplicated(true);
}

void AOmochaCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(HitReactTimerHandle);
}

void AOmochaCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();

		AbilitySystemComponent->RegisterGameplayTagEvent(
			GameplayTags.Effects_HitReact,
			EGameplayTagEventType::NewOrRemoved
		).AddUObject(this, &AOmochaCharacterBase::HitReactTagChanged);

		AbilitySystemComponent->RegisterGameplayTagEvent(
			GameplayTags.Debuff_Status_Stun,
			EGameplayTagEventType::NewOrRemoved
		).AddUObject(this, &AOmochaCharacterBase::OnStunTagChanged);

		AbilitySystemComponent->RegisterGameplayTagEvent(
			GameplayTags.Debuff_Dot_Burn,
			EGameplayTagEventType::NewOrRemoved
		).AddUObject(this, &AOmochaCharacterBase::OnBurnTagChanged);

		AbilitySystemComponent->RegisterGameplayTagEvent(
			GameplayTags.Debuff_Status_Shock,
			EGameplayTagEventType::NewOrRemoved
		).AddUObject(this, &AOmochaCharacterBase::OnShockTagChanged);
	}
}

void AOmochaCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(
		GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void AOmochaCharacterBase::ApplyAttributesFromRowName(const FName& RowName)
{
	if (!CombatsAttributes) { return; }

	UDataTable* PlayerTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Blueprint/Data/DT_PlayerAttribute"));
	if (!PlayerTable) { return; }

	const FOmochaPlayerAttributeData* Data = PlayerTable->FindRow<FOmochaPlayerAttributeData>(RowName, TEXT(""));
	if (!Data) { return; }

	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(
		CombatsAttributes, 1.f, ContextHandle);

	if (!SpecHandle.Data.IsValid()) { return; }

	const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Vital_Health,
	                                                              Data->Health);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Current_AttackDamage, Data->AttackDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Current_SkillDamage, Data->SkillDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Current_Shield,
	                                                              Data->Shield);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Current_MoveSpeed,
	                                                              Data->MoveSpeed);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Current_AttackSpeed, Data->AttackSpeed);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Current_CriticalChance, Data->CriticalChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Current_CriticalDamage, Data->CriticalDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Current_DodgeChance, Data->DodgeChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Current_CoolDownReduction, Data->CoolDownReduction);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Current_AttackRange, Data->AttackRange);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Current_Level,
	                                                              Data->Level);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Vital_MaxHealth,
	                                                              Data->MaxHealth);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Max_MaxMoveSpeed,
	                                                              Data->MaxMoveSpeed);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Max_MaxAttackSpeed, Data->MaxAttackSpeed);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Max_MaxCoolDownReduction, Data->MaxCoolDownReduction);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Max_MaxLevel,
	                                                              Data->MaxLevel);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Current_KnockbackResistance, Data->KnockbackResistance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Current_LifeSteal,
	                                                              Data->LifeSteal);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Vital_Omega, Data->Omega);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Vital_MaxOmega, Data->MaxOmega);

	// Zenith
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		GameplayTags.Attributes_Character_Zenith_ChainCount,
		Data->ChainCount
	);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		GameplayTags.Attributes_Character_Zenith_ChargingTimeMultiplier,
		Data->ChargingTimeMultiplier
	);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		GameplayTags.Attributes_Character_Zenith_CanRotateWhileCharging,
		Data->CanRotateWhileCharging
	);

	// Vulcan
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		GameplayTags.Attributes_Character_Vulcan_HasBuffRadius,
		Data->HasBuffRadius
	);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		GameplayTags.Attributes_Character_Vulcan_BuffRadiusMultiplier,
		Data->BuffRadiusMultiplier
	);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		GameplayTags.Attributes_Character_Vulcan_UltimateBezierCount,
		Data->UltimateBezierCount
	);

	// Rusty

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		GameplayTags.Attributes_Character_Rusty_HookSizeMultiplier,
		Data->HookSizeMultiplier
	);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		GameplayTags.Attributes_Character_Rusty_HookSpeedMultiplier,
		Data->HookSpeedMultiplier
	);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		GameplayTags.Attributes_Character_Rusty_UltimateGravityPull,
		Data->UltimateGravityPull
	);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		GameplayTags.Attributes_Character_Rusty_ShieldMaxHealth,
		Data->ShieldMaxHealth
	);


	// Common
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		GameplayTags.Attributes_Character_Common_AttackPenetrationCount,
		Data->AttackPenetrationCount
	);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		GameplayTags.Attributes_Character_Common_MovementSkillRangeMultiplier,
		Data->MovementSkillRangeMultiplier
	);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle,
		GameplayTags.Attributes_Character_Common_AttackProjectileSpeedMultiplier,
		Data->AttackProjectileSpeedMultiplier
	);

	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void AOmochaCharacterBase::RestoreHealthOnRebirth(float HealthPercentage)
{
	if (!AbilitySystemComponent || !AttributeSet)
	{
		return;
	}

	if (const UOmochaAttributeSet* AS = Cast<UOmochaAttributeSet>(AttributeSet))
	{
		float MaxHealth = AS->GetMaxHealth();
		float TargetHealth = MaxHealth * HealthPercentage;

		AbilitySystemComponent->SetNumericAttributeBase(
			UOmochaAttributeSet::GetHealthAttribute(), TargetHealth);
	}
}

void AOmochaCharacterBase::InitializeAttributes()
{
	if (CombatsAttributes)
	{
		ApplyAttributesFromRowName(AttributeRowName);
	}
}

void AOmochaCharacterBase::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0; // NewCount = 0 -> HitReact End

	if (AbilitySystemComponent && AttributeSet)
	{
		if (const UOmochaAttributeSet* AS = Cast<UOmochaAttributeSet>(AttributeSet))
		{
			const float NewMoveSpeed = bHitReacting ? 0.f : AS->GetMaxMoveSpeed();
			AbilitySystemComponent->SetNumericAttributeBase(
				UOmochaAttributeSet::GetMoveSpeedAttribute(), NewMoveSpeed);
		}
	}
}

void AOmochaCharacterBase::MulticastHitReact_Implementation(UMaterialInterface* HitMaterial, float Duration)
{
	if (!HitMaterial)
	{
		return;
	}

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		HitReactOriginalMaterials.Empty();
		for (int32 i = 0; i < MeshComp->GetNumMaterials(); i++)
		{
			HitReactOriginalMaterials.Add(MeshComp->GetMaterial(i));
			MeshComp->SetMaterial(i, HitMaterial);
		}

		GetWorld()->GetTimerManager().SetTimer(HitReactTimerHandle, [this]()
		{
			if (USkeletalMeshComponent* MeshComp = GetMesh())
			{
				for (int32 i = 0; i < HitReactOriginalMaterials.Num(); i++)
				{
					if (HitReactOriginalMaterials[i])
					{
						MeshComp->SetMaterial(i, HitReactOriginalMaterials[i]);
					}
				}
			}
			HitReactOriginalMaterials.Empty();
		}, Duration, false);
	}
}

bool AOmochaCharacterBase::IsBeingHitReact_Implementation() const
{
	return bHitReacting;
}

UNiagaraSystem* AOmochaCharacterBase::GetHitReactEffect_Implementation() const
{
	return HitReactEffect;
}

FVector AOmochaCharacterBase::GetHitReactEffectLocation_Implementation() const
{
	return GetActorLocation() + HitReactOffSet;
}

void AOmochaCharacterBase::Die(const FVector& DeathImpulse)
{
	if (bDead)
	{
		return;
	}
	if (HasAuthority())
	{
		MulticastHandleDeath(DeathImpulse);
	}
}

void AOmochaCharacterBase::Rebirth()
{
	if (!bDead)
	{
		return;
	}

	if (HasAuthority())
	{
		MulticastHandleRebirth();
	}
}

bool AOmochaCharacterBase::IsDead_Implementation() const
{
	return bDead;
}

AActor* AOmochaCharacterBase::GetAvatar_Implementation()
{
	return this;
}

void AOmochaCharacterBase::MulticastHandleDeath_Implementation(const FVector& DeathImpulse)
{
	if (bDead)
	{
		return;
	}

	bDead = true;

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		//PC->DisableInput(PC);
	}

	if (AbilitySystemComponent)
	{
		const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();
		AbilitySystemComponent->AddLooseGameplayTag(GameplayTags.State_Dead);
	}

	// Death
	PlayDeathSound();
	DisableCharacterMovement();
	PlayDeathAnimation();
	DisableCollisionOnDeath();

	// Debuff effect Deactviate
	if (StunDebuffComponent)
	{
		StunDebuffComponent->Deactivate();
	}
	if (BurnDebuffComponent)
	{
		BurnDebuffComponent->Deactivate();
	}
	if (ShockDebuffComponent)
	{
		ShockDebuffComponent->Deactivate();
	}


	// Event Broadcaqst
	OnDeathDelegate.Broadcast(this);
}


void AOmochaCharacterBase::AddCharacterAbilities()
{
	if (!HasAuthority())
	{
		return;
	}

	if (UOmochaAbilitySystemComponent* OmochaAbilitySystemComponent = CastChecked<UOmochaAbilitySystemComponent>(
		AbilitySystemComponent))
	{
		OmochaAbilitySystemComponent->AddCharacterAbilities(StartupAbilities, 1);
	}
}

void AOmochaCharacterBase::OnStunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
		{
			MovementComp->SetMovementMode(MOVE_None);
		}

		if (AbilitySystemComponent)
		{
			const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();
			AbilitySystemComponent->AddLooseGameplayTag(GameplayTags.Player_Block_InputPressed);
			AbilitySystemComponent->AddLooseGameplayTag(GameplayTags.Player_Block_InputHeld);
			AbilitySystemComponent->AddLooseGameplayTag(GameplayTags.Player_Block_InputReleased);
		}

		if (StunDebuffComponent)
		{
			StunDebuffComponent->Activate();
		}
	}
	else
	{
		if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
		{
			MovementComp->SetMovementMode(MOVE_Walking);
		}

		if (AbilitySystemComponent)
		{
			const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();
			AbilitySystemComponent->RemoveLooseGameplayTag(GameplayTags.Player_Block_InputPressed);
			AbilitySystemComponent->RemoveLooseGameplayTag(GameplayTags.Player_Block_InputHeld);
			AbilitySystemComponent->RemoveLooseGameplayTag(GameplayTags.Player_Block_InputReleased);
		}

		if (StunDebuffComponent)
		{
			StunDebuffComponent->Deactivate();
		}
	}
}

void AOmochaCharacterBase::OnBurnTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		if (BurnDebuffComponent)
		{
			BurnDebuffComponent->Activate();
		}

		if (AbilitySystemComponent)
		{
			const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();
		}
	}
	else
	{
		if (BurnDebuffComponent)
		{
			BurnDebuffComponent->Deactivate();
		}
	}
}

void AOmochaCharacterBase::OnShockTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		ApplySpeedReduction(0.5f);
		if (ShockDebuffComponent)
		{
			ShockDebuffComponent->Activate();
		}
	}
	else
	{
		RemoveSpeedReduction();
		if (ShockDebuffComponent)
		{
			ShockDebuffComponent->Deactivate();
		}
	}
}

bool AOmochaCharacterBase::IsStunned() const
{
	const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();
	return AbilitySystemComponent->HasMatchingGameplayTag(GameplayTags.Debuff_Status_Stun);
}

bool AOmochaCharacterBase::IsBurned() const
{
	const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();
	return AbilitySystemComponent->HasMatchingGameplayTag(GameplayTags.Debuff_Dot_Burn);
}

bool AOmochaCharacterBase::IsShocked() const
{
	const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();
	return AbilitySystemComponent->HasMatchingGameplayTag(GameplayTags.Debuff_Status_Shock);
}

void AOmochaCharacterBase::ApplySpeedReduction(float ReductionPercent)
{
	if (!AbilitySystemComponent || !SpeedReductionEffectClass)
	{
		return;
	}

	const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();

	FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
		SpeedReductionEffectClass, 1.f, ContextHandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Data_SpeedReduction,
	                                                              ReductionPercent);

	SpeedReductionEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
}

void AOmochaCharacterBase::RemoveSpeedReduction()
{
	if (AbilitySystemComponent && SpeedReductionEffectHandle.IsValid())
	{
		AbilitySystemComponent->RemoveActiveGameplayEffect(SpeedReductionEffectHandle);
		SpeedReductionEffectHandle = FActiveGameplayEffectHandle();
	}
}

void AOmochaCharacterBase::MulticastHandleRebirth_Implementation()
{
	if (!bDead)
	{
		return;
	}

	bDead = false;

	// Input recovery
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->EnableInput(PC);
	}

	// Movement recovery
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->SetMovementMode(MOVE_Walking);
	}

	// GAS Tag cleanup
	if (AbilitySystemComponent)
	{
		const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();
		AbilitySystemComponent->RemoveLooseGameplayTag(GameplayTags.State_Dead);
	}

	RestoreCollisionOnRebirth();

	// Health recovery to 50% - Only on server
	if (HasAuthority())
	{
		RestoreHealthOnRebirth(0.5f);
	}

	if (StunDebuffComponent)
	{
		StunDebuffComponent->Deactivate();
	}
	if (BurnDebuffComponent)
	{
		BurnDebuffComponent->Deactivate();
	}
	if (ShockDebuffComponent)
	{
		ShockDebuffComponent->Deactivate();
	}
}

void AOmochaCharacterBase::ApplyKnockback_Implementation(const FVector& KnockbackForce)
{
	if (HasAuthority())
	{
		if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
		{
			MovementComp->AddImpulse(KnockbackForce * 10, true);
		}
	}
}

void AOmochaCharacterBase::DisableCollisionOnDeath()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);

	GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
}

void AOmochaCharacterBase::RestoreCollisionOnRebirth()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

	GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
}

void AOmochaCharacterBase::PlayDeathAnimation()
{
	FTimerHandle UnusedHandle;

	GetWorld()->GetTimerManager().SetTimer(
		UnusedHandle,
		[this]()
		{
			if (UAnimMontage* DeathAnim = GetDeathAnimation())
			{
				PlayAnimMontage(DeathAnim);
			}
		},
		0.1f,
		false
	);
}

void AOmochaCharacterBase::PlayDeathSound()
{
	if (USoundBase* Sound = GetDeathSound())
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sound, GetActorLocation());
	}
}

UAnimMontage* AOmochaCharacterBase::GetDeathAnimation() const
{
	if (DeathAnimations.Num() > 0)
	{
		if (DeathAnimations.Num() > 1)
		{
			int32 RandomIndex = FMath::RandRange(0, DeathAnimations.Num() - 1);
			return DeathAnimations[RandomIndex];
		}
		return DeathAnimations[0];
	}

	return nullptr;
}

USoundBase* AOmochaCharacterBase::GetDeathSound() const
{
	return DeathSound;
}

float AOmochaCharacterBase::GetDeathAnimationDuration() const
{
	if (UAnimMontage* DeathMontage = GetDeathAnimation())
	{
		return DeathMontage->GetPlayLength();
	}
	return 1.0f;
}


void AOmochaCharacterBase::RotateToDeathDirection(const FVector& DeathImpulse)
{
	if (!DeathImpulse.IsNearlyZero())
	{
		FVector OppositeDirection = -DeathImpulse.GetSafeNormal();
		FRotator DeathRotation = OppositeDirection.Rotation();
		SetActorRotation(DeathRotation);
	}
}

void AOmochaCharacterBase::DisableCharacterMovement()
{
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		//PlayerController->DisableInput(PlayerController);
	}
}
