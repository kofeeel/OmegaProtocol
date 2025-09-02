// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/OmochaEnemy.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "AbilitySystem/OmochaAttributeSet.h"
#include "AI/OmochaAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "OmochaGameplayTags.h"
#include "PaperSpriteComponent.h"
#include "DataAsset/OmochaAttributeData.h"
#include "Game/OmochaGameInstance.h"
#include "Actor/OmochaEXPActor.h"
#include "Character/OmochaPlayerCharacter.h"
#include "Player/OmochaPlayerController.h"
#include "UI/Widget/OmochaUserWidget.h"

TMap<EEnemyType, FString> FOmochaEnemyNames::EnemyNames;

void FOmochaEnemyNames::InitializeNames()
{
	EnemyNames.Add(EEnemyType::None, FString("None"));
	EnemyNames.Add(EEnemyType::Muriel, FString("Muriel"));
	EnemyNames.Add(EEnemyType::Zinx, FString("Zinx"));
	EnemyNames.Add(EEnemyType::Shiv, FString("Shiv"));
	EnemyNames.Add(EEnemyType::Murdock, FString("Murdock"));
	EnemyNames.Add(EEnemyType::Boris, FString("Boris"));
	EnemyNames.Add(EEnemyType::Grappler, FString("Grappler"));
	EnemyNames.Add(EEnemyType::Glitch, FString("Glitch"));
	EnemyNames.Add(EEnemyType::BlazedCannon, FString("BlazedCannon"));
	EnemyNames.Add(EEnemyType::Screamer, FString("Screamer"));
}

AOmochaEnemy::AOmochaEnemy()
{
	AbilitySystemComponent = CreateDefaultSubobject<UOmochaAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	AttributeSet = CreateDefaultSubobject<UOmochaAttributeSet>(TEXT("AttributeSet"));


	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	EnemyMovementSpeed = 0.0f;

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());
}

void AOmochaEnemy::BeginPlay()
{
	Super::BeginPlay();

	InitAbilityActorInfo();

	if (UOmochaUserWidget* OmochaUserWidget = Cast<UOmochaUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		OmochaUserWidget->SetWidgetController(this);
	}

	if (UOmochaAttributeSet* OmochaAttributeSet = Cast<UOmochaAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(OmochaAttributeSet->GetHealthAttribute()).
		                        AddLambda(
			                        [this](const FOnAttributeChangeData& Data)
			                        {
				                        OnHealthChanged.Broadcast(Data.NewValue);
			                        }
		                        );
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(OmochaAttributeSet->GetMaxHealthAttribute()).
		                        AddLambda(
			                        [this](const FOnAttributeChangeData& Data)
			                        {
				                        OnMaxHealthChanged.Broadcast(Data.NewValue);
			                        }
		                        );

		OnHealthChanged.Broadcast(OmochaAttributeSet->GetHealth());
		OnMaxHealthChanged.Broadcast(OmochaAttributeSet->GetMaxHealth());
	}

	if (BeginAnimMontage)
	{
		PlayAnimMontage(BeginAnimMontage);
	}
}

void AOmochaEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitializeFromDataAsset();
	AddCharacterAbilities();

	if (!HasAuthority())
	{
		return;
	}

	AddEnemyAbilities();

	OmochaAIController = Cast<AOmochaAIController>(NewController);

	if (OmochaAIController && BehaviorTree)
	{
		OmochaAIController->RunBehaviorTree(BehaviorTree);

		OmochaAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);

		OmochaAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);

		OmochaAIController->GetBlackboardComponent()->SetValueAsInt(FName("EnemyIdx"), EnemyIdx);
	}
}

void AOmochaEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateAnimationData(DeltaTime);
}

void AOmochaEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	if (UOmochaAbilitySystemComponent* OmochaASC = Cast<UOmochaAbilitySystemComponent>(AbilitySystemComponent))
	{
		OmochaASC->AbilityActorInfoSet();
	}

	if (HasAuthority())
	{
		FName EnemyRowName = GetEnemyAttributeRowName();
		ApplyEnemyAttributesFromRowName(EnemyRowName);
	}
}

void AOmochaEnemy::InitializeFromDataAsset()
{
	UOmochaEnemyDataAsset* DataAsset = GetEnemyDataAsset();
	if (!DataAsset) { return; }

	CurrentClassInfo = DataAsset->GetEnemyClassInfo(EnemyRank, EnemyClass);
	BehaviorTree = CurrentClassInfo.BehaviorTree;
}

void AOmochaEnemy::AddEnemyAbilities()
{
	AddCharacterAbilities();

	UOmochaEnemyDataAsset* DataAsset = GetEnemyDataAsset();
	if (!DataAsset)
	{
		return;
	}

	UOmochaAbilitySystemComponent* OmochaASC = Cast<UOmochaAbilitySystemComponent>(AbilitySystemComponent);
	if (!OmochaASC)
	{
		return;
	}

	if (DataAsset->CommonAbilities.Num() > 0)
	{
		OmochaASC->AddCharacterAbilities(DataAsset->CommonAbilities, 1);
	}

	TArray<TSubclassOf<UGameplayAbility>> EnemyAbilities = DataAsset->GetEnemyAbilities(
		EnemyRank,
		EnemyClass,
		EnemyIdx
	);

	if (EnemyAbilities.Num() > 0)
	{
		OmochaASC->AddCharacterAbilities(EnemyAbilities, 1);
	}
}

UOmochaEnemyDataAsset* AOmochaEnemy::GetEnemyDataAsset() const
{
	if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance()))
	{
		return OmochaGameInstance->GetEnemyDA();
	}
	return nullptr;
}

void AOmochaEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::HitReactTagChanged(CallbackTag, NewCount);

	if (OmochaAIController && OmochaAIController->GetBlackboardComponent())
	{
		OmochaAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
		OmochaAIController->GetBlackboardComponent()->SetValueAsBool(FName("HasHit"), true);
	}
}

void AOmochaEnemy::InitializeBlackboardKeys()
{
	if (!OmochaAIController || !OmochaAIController->GetBlackboardComponent())
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = OmochaAIController->GetBlackboardComponent();

	// Grappler BlackboarKey
	BlackboardComp->SetValueAsBool(FName("IsPlayerAttacking"), false);
	BlackboardComp->SetValueAsBool(FName("IsPlayerInFront"), false);
	BlackboardComp->SetValueAsBool(FName("CanUseShield"), true);
	BlackboardComp->SetValueAsBool(FName("CanUseGrapple"), true);
	BlackboardComp->SetValueAsBool(FName("IsInGrappleRange"), false);
	BlackboardComp->SetValueAsFloat(FName("ShieldCooldown"), 0.0f);
	BlackboardComp->SetValueAsFloat(FName("GrappleCooldown"), 0.0f);
}

void AOmochaEnemy::ProcessItemDrop()
{
	// No item to drop
	if (!DropItemClass)
	{
		return;
	}

	// Check drop chance
	// bool bShouldDrop = bAlwaysDrop || (FMath::RandRange(0.0f, 1.0f) <= DropChance);
	//
	// if (!bShouldDrop)
	// {
	// 	return;
	// }

	// Calculate drop location
	FVector DropLocation = GetActorLocation();

	// Add some random offset
	FVector RandomOffset = FVector(
		FMath::RandRange(-100.0f, 100.0f),
		FMath::RandRange(-100.0f, 100.0f),
		0.0f
	);
	DropLocation += RandomOffset;
	DropLocation.Z = 0.f;

	// Spawn the item
	FRotator DropRotation = FRotator::ZeroRotator;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(
		DropItemClass,
		DropLocation,
		DropRotation,
		SpawnParams
	);
}

void AOmochaEnemy::Multi_HideHealthBar_Implementation()
{
	if (HealthBar)
	{
		HealthBar->SetVisibility(false);
	}

	if (GetMesh())
	{
		GetMesh()->SetRenderCustomDepth(false);
	}

	if (MinimapIcon)
	{
		MinimapIcon->SetVisibility(false);
	}
}

void AOmochaEnemy::UpdateAnimationData(float DeltaTime)
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.0f;

	float MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	EnemyMovementSpeed = MaxWalkSpeed > 0.0f ? FMath::Clamp(Velocity.Size() / MaxWalkSpeed, 0.0f, 1.0f) : 0.0f;

	bEnemyIsMoving = EnemyMovementSpeed > 0.1f;

	if (bEnemyIsMoving)
	{
		FVector ForwardVector = GetActorForwardVector();
		FVector VelocityNormalized = Velocity.GetSafeNormal();

		float DotProduct = FVector::DotProduct(ForwardVector, VelocityNormalized);
		float CrossProduct = FVector::CrossProduct(ForwardVector, VelocityNormalized).Z;

		EnemyMovementDirection = FMath::Atan2(CrossProduct, DotProduct) * 180.0f / PI;
	}
	else
	{
		EnemyMovementDirection = 0.0f;
	}
}

void AOmochaEnemy::ApplyEnemyAttributesFromRowName(const FName& RowName)
{
	if (!CombatsAttributes)
	{
		return;
	}

	UDataTable* EnemyTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Blueprint/Data/DT_EnemyAttribute"));
	if (!EnemyTable) { return; }

	const FOmochaEnemyAttributeData* Data = EnemyTable->FindRow<FOmochaEnemyAttributeData>(RowName, TEXT(""));
	if (!Data) { return; }

	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(
		CombatsAttributes, 1.f, ContextHandle);

	if (!SpecHandle.Data.IsValid())
	{
		return;
	}

	const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();

	if (Data->MaxHealth)
	{
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Vital_Health,
		                                                              Data->MaxHealth->GetFloatValue(PlayerLevel));
	}
	if (Data->AttackDamage)
	{
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
			SpecHandle, GameplayTags.Attributes_Current_AttackDamage, Data->AttackDamage->GetFloatValue(PlayerLevel));
	}
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Current_SkillDamage, Data->SkillDamage);
	if (Data->Shield)
	{
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
			SpecHandle, GameplayTags.Attributes_Current_Shield,
			Data->Shield->GetFloatValue(PlayerLevel));
	}
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
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Current_KnockbackResistance, Data->KnockbackResistance);

	// Max Values
	if (Data->MaxHealth)
	{
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
			SpecHandle, GameplayTags.Attributes_Vital_MaxHealth,
			Data->MaxHealth->GetFloatValue(PlayerLevel));
	}
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Max_MaxMoveSpeed,
	                                                              Data->MaxMoveSpeed);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Max_MaxAttackSpeed, Data->MaxAttackSpeed);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
		SpecHandle, GameplayTags.Attributes_Max_MaxCoolDownReduction, Data->MaxCoolDownReduction);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Max_MaxLevel,
	                                                              Data->MaxLevel);

	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

FName AOmochaEnemy::GetEnemyAttributeRowName() const
{
	return FName(*FOmochaEnemyNames::FindName(EnemyType));
}

float AOmochaEnemy::GetEnemyMovementSpeed() const
{
	return EnemyMovementSpeed;
}

float AOmochaEnemy::GetEnemyMovementDirection() const
{
	return EnemyMovementDirection;
}

void AOmochaEnemy::Die(const FVector& DeathImpulse)
{
	if (bDead)
	{
		return;
	}

	if (HasAuthority())
	{
		Multi_HideHealthBar();
	}

	if (OmochaAIController && OmochaAIController->GetBlackboardComponent())
	{
		OmochaAIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);
	}
	
	AOmochaAIController* AIController = Cast<AOmochaAIController>(GetController());
	if (AIController)
	{
		AIController->GetBrainComponent()->StopLogic(TEXT("Dead"));

		AIController->StopMovement();
	}

	ProcessItemDrop();

	if (HasAuthority() && LastDamageSource)
	{
		UE_LOG(LogTemp, Warning, TEXT("LastDamageSource found: %s"), *LastDamageSource->GetName());
        
		UE_LOG(LogTemp, Warning, TEXT("Notifying all clients about death. Killer: %s"), 
			   *LastDamageSource->GetName());
        
		FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 80);
		Multcast_EXP(LastDamageSource, SpawnLocation);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HasAuthority: %s, LastDamageSource: %s"), 
			   HasAuthority() ? TEXT("True") : TEXT("False"),
			   LastDamageSource ? TEXT("Valid") : TEXT("NULL"));
	}
	
	Super::Die(DeathImpulse);
	SetLifeSpan(3.f);
}

void AOmochaEnemy::Multcast_EXP_Implementation(AActor* Killer, FVector DeathLocation)
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (PC->GetPawn() == Killer)
		{
			UE_LOG(LogTemp, Warning, TEXT("Local player is the killer, spawning XP orb"));
			SpawnXPOrb(Killer, DeathLocation);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Local player is not the killer, no XP orb"));
		}
	}
}

float AOmochaEnemy::GetXPReward() const
{
	UDataTable* EnemyTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Blueprint/Data/DT_EnemyAttribute"));
	if (!EnemyTable) 
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyTable not found, using default XP"));
		return 50.0f;
	}

	FName RowName = GetEnemyAttributeRowName();
	const FOmochaEnemyAttributeData* Data = EnemyTable->FindRow<FOmochaEnemyAttributeData>(RowName, TEXT(""));
    
	if (!Data)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy data not found for %s, using default XP"), *RowName.ToString());
		return 50.0f;
	}
    
	UE_LOG(LogTemp, Log, TEXT("Enemy %s gives %f XP"), *RowName.ToString(), Data->XP);
	return Data->XP;
}

void AOmochaEnemy::SpawnXPOrb(AActor* TargetPlayer, FVector SpawnLocation)
{
	if (!XPOrbClass || !TargetPlayer)
	{
		return;
	}
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (AOmochaEXPActor* XPOrb = GetWorld()->SpawnActor<AOmochaEXPActor>(
		XPOrbClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams))
	{
		XPOrb->InitializeXPOrb(TargetPlayer);
		UE_LOG(LogTemp, Warning, TEXT("XP Orb spawned locally for: %s"), *TargetPlayer->GetName());
	}
}
