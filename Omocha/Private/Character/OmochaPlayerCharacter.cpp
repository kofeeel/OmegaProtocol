// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/OmochaPlayerCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraComponent.h"
#include "OmochaGameplayTags.h"
#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "AbilitySystem/OmochaAttributeSet.h"
#include "Actor/OmochaTransformProjectile.h"
#include "Actor/WeaponPickupActor.h"
#include "Component/OmochaCharacterConfigComponent.h"
#include "Component/OmochaWeaponComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/OmochaPlayerController.h"
#include "Player/OmochaPlayerState.h"
#include "Components/DecalComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/WidgetComponent.h"
#include "DataAsset/OmochaCharacterDataAsset.h"
#include "Engine/TextureRenderTarget.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Game/OmochaGameInstance.h"
#include "Game/OmochaPlayGameMode.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Omocha/Omocha.h"
#include "UI/HUD/OmochaHUD.h"
#include "UI/Widget/OmochaUserWidget.h"

AOmochaPlayerCharacter::AOmochaPlayerCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	CharacterConfigComponent = CreateDefaultSubobject<
		UOmochaCharacterConfigComponent>(TEXT("CharacterConfigComponent"));

	FixedDecalRoot = CreateDefaultSubobject<USceneComponent>(TEXT("FixedDecalRoot"));
	FixedDecalRoot->SetupAttachment(RootComponent);

	FixedDecalRoot->SetUsingAbsoluteRotation(true);
	FixedDecalRoot->SetWorldRotation(FRotator::ZeroRotator);

	HeadStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadStaticMeshComponent"));
	HeadStaticMeshComponent->SetupAttachment(GetMesh());
	HeadStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Normal State Decal Setup
	NormalStateDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("NormalStateDecal"));
	NormalStateDecal->SetupAttachment(FixedDecalRoot);
	NormalStateDecal->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	NormalStateDecal->DecalSize = FVector(100.0f, 100.0f, 100.0f);
	NormalStateDecal->SetVisibility(false);

	// Omega State Decal Setup  
	OmegaStateDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("OmegaStateDecal"));
	OmegaStateDecal->SetupAttachment(FixedDecalRoot);
	OmegaStateDecal->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	OmegaStateDecal->DecalSize = FVector(120.0f, 120.0f, 120.0f);
	OmegaStateDecal->SetVisibility(false);


	OmegaAuraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("OmegaAuraComponent"));
	OmegaAuraComponent->SetupAttachment(FixedDecalRoot);
	OmegaAuraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	OmegaAuraComponent->SetAutoActivate(false);
	OmegaAuraComponent->SetVisibility(false);

	// Basic Death Data Hide
	bShowBasicDeathData = false;

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());

	PlayerState = FOmochaGameplayTags::Get().State_Normal;
	LoadState = FOmochaGameplayTags::Get().State_Normal;

	MinimapSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmMinimap"));
	MinimapSpringArm->SetupAttachment(GetRootComponent());

	MinimapCamera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MinimapCamera"));
	MinimapCamera->SetupAttachment(MinimapSpringArm);
	MinimapCamera->ShowFlags.SetSkeletalMeshes(false);
	MinimapCamera->ShowFlags.SetParticles(false);
	MinimapCamera->ShowFlags.SetDynamicShadows(false);
	MinimapCamera->ShowFlags.SetDecals(false);
	MinimapCamera->ShowFlags.SetLighting(false);
	MinimapCamera->ShowFlags.SetPaper2DSprites(true);

	MinimapCamera->bCaptureEveryFrame = true;

	WeaponComponent = CreateDefaultSubobject<UOmochaWeaponComponent>(TEXT("WeaponComponent"));
}

void AOmochaPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	NormalStateDecal->SetRelativeLocation(FVector(0.0f, 0.0f, -CapsuleHalfHeight));
	OmegaStateDecal->SetRelativeLocation(FVector(0.0f, 0.0f, -CapsuleHalfHeight));
	
	if (HasAuthority()) {
		InitializeComponents();
	}
}


void AOmochaPlayerCharacter::SetupHealthBar()
{
	if (bIsHealthBarSetupCompleted) {
		return;
	}

	if (IsLocallyControlled()) {
		return;
	}

	if (!HealthBar->GetWidget()) {
		HealthBar->InitWidget();
	}

	if (UOmochaUserWidget* OmochaUserWidget = Cast<UOmochaUserWidget>(HealthBar->GetUserWidgetObject())) {
		OmochaUserWidget->SetWidgetController(this);
	}

	if (UOmochaAttributeSet* OmochaAttributeSet = Cast<UOmochaAttributeSet>(AttributeSet)) {
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

		const float MaxHealth = OmochaAttributeSet->GetMaxHealth();
		const float Health = OmochaAttributeSet->GetHealth();
		OnMaxHealthChanged.Broadcast(MaxHealth);
		if (Health < 0.01f)
		{
			OnHealthChanged.Broadcast(OmochaAttributeSet->GetResilience() * MaxHealth);
		}
		else
		{
			OnHealthChanged.Broadcast(Health);	
		}
	}
	bIsHealthBarSetupCompleted = true;
}

void AOmochaPlayerCharacter::SetHealthBarVisibility(bool bVisible)
{
	if (HasAuthority())
	{
		Multicast_SetHealthBarVisibility(bVisible);
	}
}

void AOmochaPlayerCharacter::Multicast_SetHealthBarVisibility_Implementation(bool bVisible)
{
	if (HealthBar)
	{
		HealthBar->SetVisibility(bVisible);
	}
}

void AOmochaPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitAbilityActorInfo();
	LoadProgress();
	SetupHealthBar();
	
	if (AOmochaPlayerState* OmochaPlayerState = GetPlayerState<AOmochaPlayerState>())
	{
		LevelBeginPlay(OmochaPlayerState);
	}
}

void AOmochaPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();
	SetupHealthBar();
	if (AOmochaPlayerState* OmochaPlayerState = GetPlayerState<AOmochaPlayerState>())
	{
		LevelBeginPlay(OmochaPlayerState);
	}
	if (WeaponComponent)
	{
		WeaponComponent.Get()->UpdateWeaponMeshFromState();
	}
	if (GetAbilitySystemComponent())
	{
		const UOmochaAttributeSet* AS = Cast<UOmochaAttributeSet>(GetAbilitySystemComponent()->GetAttributeSet(UOmochaAttributeSet::StaticClass()));
		if (AS)
		{
			GetCharacterMovement()->MaxWalkSpeed = AS->GetMoveSpeed();
		}
	}
}


void AOmochaPlayerCharacter::LoadProgress()
{
	if (!HasAuthority()) {
		return;
	}

	if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance())) {
		if (OmochaGameInstance->IsPlayerAbilitySystemSaved(UOmochaGameInstance::GetUniqueId(GetPlayerState()))) {
			if (AOmochaPlayGameMode* OmochaPlayGameMode = Cast<
				AOmochaPlayGameMode>(UGameplayStatics::GetGameMode(this)))
			{
				OmochaPlayGameMode->LoadPlayerInformation(GetPlayerState(), GetAbilitySystemComponent());
			}
		}
		else {
			InitializeAttributes();
		}
	}

	if (CharacterConfigComponent) {
		CharacterConfigComponent->InitializeState(LoadState);
	}

	if (UOmochaAttributeSet* OmochaAttributeSet = Cast<UOmochaAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(OmochaAttributeSet->GetShieldMaxHealthAttribute()).
								AddLambda(
									[this](const FOnAttributeChangeData& Data)
									{
										OnShieldMaxHealthChanged.Broadcast(Data.NewValue);
									}
								);
		
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(OmochaAttributeSet->GetHealthAttribute()).
								AddLambda(
									[this](const FOnAttributeChangeData& Data)
									{
										if (Data.OldValue > Data.NewValue && Data.NewValue > 0.01f)
										{
											OnPlayerDamaged.Broadcast();
										}
									}
								);
	}

	if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance()))
	{
		const FDataTableRowHandle EquippedWeaponRow = OmochaGameInstance->GetEquippedWeaponRow(UOmochaGameInstance::GetUniqueId(GetPlayerState()));
		if (EquippedWeaponRow.DataTable && !EquippedWeaponRow.RowName.IsNone())
		{
			// TODO : Load Grade
			WeaponComponent->EquipWeapon(EquippedWeaponRow, EWeaponGrade::Normal);
		}
	}
}

void AOmochaPlayerCharacter::InitMinimapMaterial()
{
	if (IsLocallyControlled() && MinimapMaterial) {
		MinimapRenderTarget = NewObject<UTextureRenderTarget2D>(this);
		MinimapRenderTarget->InitAutoFormat(256, 256);
		MinimapRenderTarget->UpdateResource();

		MinimapCamera->TextureTarget = MinimapRenderTarget;
		MinimapDynamicInstance = UMaterialInstanceDynamic::Create(MinimapMaterial, this);
		MinimapDynamicInstance->SetTextureParameterValue(FName("MinimapTexture"), MinimapRenderTarget);
	}
}

void AOmochaPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOmochaPlayerCharacter, CurrentSpineRotation);
}

void AOmochaPlayerCharacter::SetSpineRotation(float Rotation)
{
	CurrentSpineRotation = Rotation;

	if (!HasAuthority()) {
		ServerSetSpineRotation(Rotation);
	}
}

void AOmochaPlayerCharacter::ServerSetSpineRotation_Implementation(float Rotation)
{
	CurrentSpineRotation = Rotation;
}

float AOmochaPlayerCharacter::GetMovementSpeed() const
{
	return MovementSpeed;
}

float AOmochaPlayerCharacter::GetMovementDirection() const
{
	return MovementDirection;
}

void AOmochaPlayerCharacter::ApplyOmegaAttributeOrRemove(FGameplayTag StateTag)
{
	if (StateTag.MatchesTagExact(FOmochaGameplayTags::Get().State_Normal)) {
		if (AbilitySystemComponent->HasMatchingGameplayTag(FOmochaGameplayTags::Get().Attributes_Current_Omega)
			&& PlayerState.MatchesTagExact(FOmochaGameplayTags::Get().State_Omega)) {
			PlayerState = StateTag;

			UOmochaAttributeSet* AS = CastChecked<UOmochaAttributeSet>(AttributeSet);
			const float Percent = AS->GetHealth() / AS->GetMaxHealth();

			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
			EffectContext.AddSourceObject(this);
			const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
				SetByCallerHealthAttributesEffect, 1.f, EffectContext);

			AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(
				FGameplayTagContainer(FOmochaGameplayTags::Get().Attributes_Current_Omega));

			if (UOmochaGameInstance* GI = Cast<UOmochaGameInstance>(GetGameInstance()))
			{
				GI->SetOmega(AS->GetOmega());
			}
			
			const float Health = AS->GetMaxHealth() * Percent;
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
				SpecHandle, FOmochaGameplayTags::Get().Attributes_Vital_Health, Health);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
				SpecHandle, FOmochaGameplayTags::Get().Attributes_Vital_Omega, 0.f);
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
		}
	}
	else if (StateTag.MatchesTagExact(FOmochaGameplayTags::Get().State_Omega)) {
		if (!AbilitySystemComponent->HasMatchingGameplayTag(FOmochaGameplayTags::Get().Attributes_Current_Omega)
			&& PlayerState.MatchesTagExact(FOmochaGameplayTags::Get().State_Normal)) {
			PlayerState = StateTag;

			UOmochaAttributeSet* AS = CastChecked<UOmochaAttributeSet>(AttributeSet);
			const float Percent = AS->GetHealth() / AS->GetMaxHealth();

			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
			EffectContext.AddSourceObject(this);
			const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
				SetByCallerHealthAttributesEffect, 1.f, EffectContext);

			AbilitySystemComponent->ApplyGameplayEffectToSelf(
				Cast<UGameplayEffect>(OmegaAttributesEffect->GetDefaultObject()), 1.f, EffectContext);
			AbilitySystemComponent->ApplyGameplayEffectToSelf(
				Cast<UGameplayEffect>(OmegaWarmAttributeEffect->GetDefaultObject()), 1.f, EffectContext);

			const float Health = AS->GetMaxHealth() * Percent;
			float Omega = 0.f;
			if (UOmochaGameInstance* GI = Cast<UOmochaGameInstance>(GetGameInstance()))
			{
				Omega = GI->GetOmega();
			}
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
				SpecHandle, FOmochaGameplayTags::Get().Attributes_Vital_Health, Health);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
				SpecHandle, FOmochaGameplayTags::Get().Attributes_Vital_Omega, Omega);
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
		}
	}
}

void AOmochaPlayerCharacter::BlockInputForTransform(bool bBlocked)
{
	if (AOmochaPlayerController* PlayerController = Cast<AOmochaPlayerController>(GetController())) {
		PlayerController->SetInputBlocked(bBlocked);
	}
}

void AOmochaPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateAnimationData(DeltaTime);
}

void AOmochaPlayerCharacter::UpdateAnimationData(float DeltaTime)
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.0f;

	float MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	MovementSpeed = MaxWalkSpeed > 0.0f ? FMath::Clamp(Velocity.Size() / MaxWalkSpeed, 0.0f, 1.0f) : 0.0f;

	bIsMoving = MovementSpeed > 0.1f;

	if (bIsMoving) {
		FVector ForwardVector = GetActorForwardVector();
		FVector VelocityNormalized = Velocity.GetSafeNormal();

		float DotProduct = FVector::DotProduct(ForwardVector, VelocityNormalized);
		float CrossProduct = FVector::CrossProduct(ForwardVector, VelocityNormalized).Z;

		MovementDirection = FMath::Atan2(CrossProduct, DotProduct) * 180.0f / PI;
	}
	else {
		MovementDirection = 0.0f;
	}
}

void AOmochaPlayerCharacter::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::HitReactTagChanged(CallbackTag, NewCount);
}


void AOmochaPlayerCharacter::Client_StartLobbyCameraCinematic_Implementation()
{
	Event_StartLobbyCameraCinematic();
}

void AOmochaPlayerCharacter::MulticastHandleDeath_Implementation(const FVector& DeathImpulse)
{
	Super::MulticastHandleDeath_Implementation(DeathImpulse);

	bEnableCameraPanning = false;
	if (AOmochaPlayGameMode* OmochaPlayGameMode = Cast<AOmochaPlayGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		OmochaPlayGameMode->PlayerDied(GetPlayerState());
	}
}

void AOmochaPlayerCharacter::MulticastHandleRebirth_Implementation()
{
	Super::MulticastHandleRebirth_Implementation();

	bEnableCameraPanning = true;
	if (AOmochaPlayGameMode* OmochaPlayGameMode = Cast<AOmochaPlayGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		OmochaPlayGameMode->PlayerRebirth(GetPlayerState());
	}
}

void AOmochaPlayerCharacter::DisableCollisionOnDeath()
{
	Super::DisableCollisionOnDeath();
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_EnemyChainTrace, ECR_Ignore);
}

void AOmochaPlayerCharacter::RestoreCollisionOnRebirth()
{
	Super::RestoreCollisionOnRebirth();

	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_EnemyChainTrace, ECR_Overlap);
}

void AOmochaPlayerCharacter::InitAbilityActorInfo()
{
	if (AOmochaPlayerState* OmochaPlayerState = GetPlayerState<AOmochaPlayerState>()) {
		AttributeSet = OmochaPlayerState->GetAttributeSet();
		AbilitySystemComponent = OmochaPlayerState->GetAbilitySystemComponent();
		AbilitySystemComponent->InitAbilityActorInfo(OmochaPlayerState, this);

		InitMinimapMaterial();

		if (AOmochaPlayerController* OmochaPlayerController = Cast<AOmochaPlayerController>(GetController())) {
			if (AOmochaHUD* OmochaHUD = Cast<AOmochaHUD>(OmochaPlayerController->GetHUD())) {
				OmochaHUD->InitOverlay(OmochaPlayerController, OmochaPlayerState, AbilitySystemComponent, AttributeSet,
				                       this);
			}
		}
	}
}

void AOmochaPlayerCharacter::UpdateStateVisuals(FGameplayTag CurrentState)
{
	const FOmochaGameplayTags& GameplayTags = FOmochaGameplayTags::Get();

	if (CurrentState.MatchesTagExact(GameplayTags.State_Normal)) {
		ShowNormalDecal();

		// Hide Omega Aura
		if (OmegaAuraComponent) {
			OmegaAuraComponent->SetVisibility(false);
			OmegaAuraComponent->Deactivate();
		}
	}
	else if (CurrentState.MatchesTagExact(GameplayTags.State_Omega)) {
		ShowOmegaDecal();

		// Show Omega Aura with character-specific effect
		if (UOmochaCharacterConfigComponent* ConfigComp = GetCharacterConfigComponent()) {
			if (UOmochaCharacterDataAsset* CurrentData = ConfigComp->GetCurrentCharacterData()) {
				if (CurrentData->OmegaAuraEffect && OmegaAuraComponent) {
					OmegaAuraComponent->SetAsset(CurrentData->OmegaAuraEffect);
					OmegaAuraComponent->SetVisibility(true);
					OmegaAuraComponent->Activate();
				}
			}
		}
	}
	else {
		HideAllDecals();

		// Hide Omega Aura
		if (OmegaAuraComponent) {
			OmegaAuraComponent->SetVisibility(false);
			OmegaAuraComponent->Deactivate();
		}
	}
}

void AOmochaPlayerCharacter::ShowNormalDecal()
{
	if (NormalStateDecal) {
		NormalStateDecal->SetVisibility(true);
	}
	if (OmegaStateDecal) {
		OmegaStateDecal->SetVisibility(false);
	}
}

void AOmochaPlayerCharacter::ShowOmegaDecal()
{
	if (OmegaStateDecal) {
		OmegaStateDecal->SetVisibility(true);
	}
	if (NormalStateDecal) {
		NormalStateDecal->SetVisibility(false);
	}
}

void AOmochaPlayerCharacter::HideAllDecals()
{
	if (NormalStateDecal) {
		NormalStateDecal->SetVisibility(false);
	}
	if (OmegaStateDecal) {
		OmegaStateDecal->SetVisibility(false);
	}
}

UAnimMontage* AOmochaPlayerCharacter::GetDeathAnimation() const
{
	if (CharacterConfigComponent) {
		bool bIsOmega = CharacterConfigComponent->IsInOmegaState();

		const TArray<UAnimMontage*>& StateAnimations = bIsOmega ? OmegaDeathAnimations : NormalDeathAnimations;

		if (StateAnimations.Num() > 0) {
			int32 RandomIndex = FMath::RandRange(0, StateAnimations.Num() - 1);
			return StateAnimations[RandomIndex];
		}
	}

	return Super::GetDeathAnimation();
}

USoundBase* AOmochaPlayerCharacter::GetDeathSound() const
{
	{
		bool bIsOmega = CharacterConfigComponent->IsInOmegaState();

		USoundBase* StateSound = bIsOmega ? OmegaDeathSound : NormalDeathSound;

		if (StateSound) {
			return StateSound;
		}
	}

	return Super::GetDeathSound();
}

void AOmochaPlayerCharacter::Die(const FVector& DeathImpulse)
{
	if (HasAuthority()) {
		OnPlayerDied.Broadcast();
		UOmochaCharacterConfigComponent* ConfigComp = GetCharacterConfigComponent();
		if (ConfigComp && ConfigComp->IsInOmegaState()) {
			ConfigComp->ChangeState(FOmochaGameplayTags::Get().State_Normal);

			if (TransformProjectileClassToDrop) {
				AOmochaTransformProjectile::SpawnAsDroppable(GetWorld(), GetActorLocation(), this,
				                                             TransformProjectileClassToDrop);
			}
		}
	}
	
	Super::Die(DeathImpulse);

	if (HasAuthority()) {
		if (AOmochaPlayerController* PC = Cast<AOmochaPlayerController>(GetController())) {
			// Timing Issue delay
			FTimerHandle SpectatingDelayHandle;
			GetWorld()->GetTimerManager().SetTimer(SpectatingDelayHandle,[PC]()
			{
				PC->StartSpectating();
			}, 1.0f, false);
		}
	}
}

void AOmochaPlayerCharacter::Rebirth()
{
	Super::Rebirth();

	if (HasAuthority())
	{
		OnPlayerRebirth.Broadcast();
	}
}

void AOmochaPlayerCharacter::Rebirth(bool bBecomeOmega)
{
	Super::Rebirth();

	if (HasAuthority()) {

		OnPlayerRebirth.Broadcast();
		
		if (AOmochaPlayerController* PC = Cast<AOmochaPlayerController>(GetController())) {
			PC->Client_StopSpectating();
		}

		if (bBecomeOmega) {
			if (CharacterConfigComponent && CharacterConfigComponent->IsInNormalState()) {
				CharacterConfigComponent->ChangeState(FOmochaGameplayTags::Get().State_Omega);
			}
		}
	}
}

void AOmochaPlayerCharacter::StripOmegaState_Server_Implementation()
{
	StripOmegaState_Multicast();
}

void AOmochaPlayerCharacter::StripOmegaState_Multicast_Implementation()
{
	if (CharacterConfigComponent && CharacterConfigComponent->IsInOmegaState()) {
		CharacterConfigComponent->ChangeState(FOmochaGameplayTags::Get().State_Normal);
	}
}
void AOmochaPlayerCharacter::BeginReviveSequence()
{
	if (!HasAuthority() || !bDead)
	{
		return;
	}

	if (AbilitySystemComponent->HasMatchingGameplayTag(FOmochaGameplayTags::Get().State_RevivingWithOmega))
	{
		return;
	}

	AbilitySystemComponent->AddLooseGameplayTag(FOmochaGameplayTags::Get().State_RevivingWithOmega);

	Multicast_PlayRevivalMontage();
	
	GetWorld()->GetTimerManager().SetTimer(
		ReviveTimerHandle,
		this,
		&AOmochaPlayerCharacter::CompleteRevival,
		5.0f,
		false
	);

}

void AOmochaPlayerCharacter::CompleteRevival()
{
	if (!HasAuthority()) return;

	GetWorld()->GetTimerManager().ClearTimer(ReviveTimerHandle);
	if (AbilitySystemComponent->HasMatchingGameplayTag(FOmochaGameplayTags::Get().State_RevivingWithOmega))
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(FOmochaGameplayTags::Get().State_RevivingWithOmega);

		if (CharacterConfigComponent && CharacterConfigComponent->GetCurrentCharacterData())
		{
			if (UAnimMontage* MontageToStop = CharacterConfigComponent->GetCurrentCharacterData()->ReviveMontage)
			{
				StopAnimMontage(MontageToStop);
			}
		}
		
		Rebirth(true);
	}
}

void AOmochaPlayerCharacter::Multicast_PlayRevivalMontage_Implementation()
{
	if (CharacterConfigComponent && CharacterConfigComponent->GetCurrentCharacterData())
	{
		if (UAnimMontage* MontageToPlay = CharacterConfigComponent->GetCurrentCharacterData()->ReviveMontage)
		{
			PlayAnimMontage(MontageToPlay);
		}
	}
}

void AOmochaPlayerCharacter::Multicast_StopAndPlayDeathMontage_Implementation()
{
	if (CharacterConfigComponent && CharacterConfigComponent->GetCurrentCharacterData())
	{
		if (UAnimMontage* MontageToStop = CharacterConfigComponent->GetCurrentCharacterData()->ReviveMontage)
		{
			StopAnimMontage(MontageToStop);
		}
	}
	PlayAnimMontage(GetDeathAnimation());
}

void AOmochaPlayerCharacter::CancelReviveSequence()
{
	if (!HasAuthority()) return;

	if (AbilitySystemComponent->HasMatchingGameplayTag(FOmochaGameplayTags::Get().State_RevivingWithOmega))
	{
		GetWorld()->GetTimerManager().ClearTimer(ReviveTimerHandle);
		ReviveTimerHandle.Invalidate();

		AbilitySystemComponent->RemoveLooseGameplayTag(FOmochaGameplayTags::Get().State_RevivingWithOmega);
		
		Multicast_StopAndPlayDeathMontage();
	}
}
