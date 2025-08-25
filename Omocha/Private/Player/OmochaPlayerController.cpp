// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/OmochaPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagContainer.h"
#include "EnhancedInputSubsystems.h"
#include "OmochaGameplayTags.h"
#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "AbilitySystem/OmochaAttributeSet.h"
#include "Actor/OmochaEffectActor.h"
#include "Character/OmochaPlayerCharacter.h"
#include "Component/OmochaSoundRPCComponent.h"
#include "Input/OmochaInputComponent.h"
#include "GameFramework/Character.h"
#include "UI/Widget/OmochaDamageTextComponent.h"
#include "Engine/World.h"
#include "Game/OmochaGameStateBase.h"
#include "Game/OmochaLobbyGameMode.h"
#include "Game/OmochaScoreBoardGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Omocha/Omocha.h"
#include "Player/OmochaPlayerState.h"
#include "Subsystems/FadeSubsystem.h"
#include "Subsystems/SoundManager.h"
#include "UI/HUD/OmochaHUD.h"
#include "UI/Widget/ChatWidget.h"
#include "UI/Widget/SpectatorWidget.h"

AOmochaPlayerController::AOmochaPlayerController()
{
	bReplicates = true;
	SoundRPCComponent = CreateDefaultSubobject<UOmochaSoundRPCComponent>(TEXT("SoundRPCComponent"));
}

void AOmochaPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();

	if (bEnableLookSystem && IsLocalController()) {
		UpdateLookSystem(DeltaTime);
	}
}

void AOmochaPlayerController::Client_GetPrepareForNextLevel_Implementation(bool bReady)
{
	OnPrepareNextLevel.Broadcast(bReady);
}

void AOmochaPlayerController::KickPlayer(int32 PlayerNum)
{
	if (!HasAuthority()) {
		return;
	}
	if (AOmochaBaseGameMode* OmochaGM = Cast<AOmochaLobbyGameMode>(UGameplayStatics::GetGameMode(this))) {
		OmochaGM->KickPlayer(PlayerNum);
	}
}

void AOmochaPlayerController::Client_PlayerLogin_Implementation(int32 PlayerNum, const FString& PlayerName,
                                                                const FString& SteamId)
{
	OnPlayerLogin.Broadcast(PlayerNum, PlayerName, SteamId);
}

void AOmochaPlayerController::Client_PlayerCharacterChanged_Implementation(int32 PlayerNum,
                                                                           const ECharacterType& CharacterType)
{
	OnPlayerCharacter.Broadcast(PlayerNum, CharacterType);
}

void AOmochaPlayerController::Client_PlayerReady_Implementation(int32 PlayerNum, const bool Ready)
{
	OnPlayerReady.Broadcast(PlayerNum, Ready);
}

void AOmochaPlayerController::Server_InitializeRequest_Implementation()
{
	if (AOmochaLobbyGameMode* OmochaGM = Cast<AOmochaLobbyGameMode>(UGameplayStatics::GetGameMode(this))) {
		OmochaGM->InitializePlayerInfo(this);
	}
}

void AOmochaPlayerController::Client_InitializeResponse_Implementation(int32 PlayerNum, const FString& PlayerName,
                                                                       const FString& SteamId,
                                                                       const ECharacterType& CharacterType,
                                                                       const bool Ready)
{
	OnPlayerLogin.Broadcast(PlayerNum, PlayerName, SteamId);
	OnPlayerCharacter.Broadcast(PlayerNum, CharacterType);
	OnPlayerReady.Broadcast(PlayerNum, Ready);
}

bool AOmochaPlayerController::IsLocallyServerController(int32 PlayerNumber)
{
	if (UOmochaGameInstance* OmochaGameInstance = Cast<UOmochaGameInstance>(GetGameInstance())) {
		if (OmochaGameInstance->GetPlayerNumber(UOmochaGameInstance::GetUniqueId(GetPlayerState<AOmochaPlayerState>()))
			== PlayerNumber) {
			return true;
		}
	}
	return false;
}

void AOmochaPlayerController::Client_StartPreTravelEffects_Implementation(float FadeDuration)
{
	if (UFadeSubsystem* FadeSubsystem = ULocalPlayer::GetSubsystem<UFadeSubsystem>(GetLocalPlayer())) {
		FadeSubsystem->PlayFadeOut(1.0f / FadeDuration);
	}

	if (USoundManager* SoundManager = USoundManager::Get(this)) {
		SoundManager->FadeOutBGM(FadeDuration);
	}
}

void AOmochaPlayerController::Server_PlayerRestartReady_Implementation(const bool bReady)
{
	if (AOmochaScoreBoardGameMode* OmochaGM = Cast<AOmochaScoreBoardGameMode>(UGameplayStatics::GetGameMode(this))) {
		if (APlayerState* PS = GetPlayerState<APlayerState>()) {
			OmochaGM->PlayerRestartReady(PS, bReady);
		}
	}
}

void AOmochaPlayerController::Client_PlayerRestartState_Implementation(int32 PlayerNumber,
                                                                       const EPlayerRestartState& State)
{
	OnScoreBoardPlayerReady.Broadcast(PlayerNumber, State);
}

void AOmochaPlayerController::Server_ScoreDataRequest_Implementation()
{
	if (AOmochaScoreBoardGameMode* OmochaGM = Cast<AOmochaScoreBoardGameMode>(UGameplayStatics::GetGameMode(this))) {
		if (AOmochaPlayerState* PS = GetPlayerState<AOmochaPlayerState>()) {
			Client_ScoreDataResponse(OmochaGM->GetScoreBoardData(PS));
			OmochaGM->AllOfPlayerStatesRequest(this);
		}
	}
}

void AOmochaPlayerController::Client_ScoreDataResponse_Implementation(const FGameScoreBoardData& Data)
{
	OnScoreBoardDataResponse.Broadcast(Data);
}

void AOmochaPlayerController::Client_GameOver_Implementation(const bool Success)
{
	OnGameOver.Broadcast(Success);
}

void AOmochaPlayerController::BindingInputAction()
{
	UOmochaInputComponent* OmochaInputComponent = CastChecked<UOmochaInputComponent>(InputComponent);
	OmochaInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AOmochaPlayerController::Move);
	OmochaInputComponent->BindAction(PlayerViewChangeAction, ETriggerEvent::Triggered, this,
	                                 &AOmochaPlayerController::OnSpectatorNext);
	OmochaInputComponent->BindAbilityActions(InputConfig,
	                                         this,
	                                         &ThisClass::AbilityInputTagPressed,
	                                         &ThisClass::AbilityInputTagReleased,
	                                         &ThisClass::AbilityInputTagHeld);
	OmochaInputComponent->BindAction(ToggleStatsAction, ETriggerEvent::Started, this,
	                                 &AOmochaPlayerController::ShowStatWidget);
	OmochaInputComponent->BindAction(ToggleStatsAction, ETriggerEvent::Completed, this,
	                                 &AOmochaPlayerController::HideStatWidget);
}

void AOmochaPlayerController::SetCursorEnabled(bool bCursorEnabled)
{
	bShowMouseCursor = bCursorEnabled;

	if (bCursorEnabled) {
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
		InputMode.SetHideCursorDuringCapture(false);
		SetInputMode(InputMode);
		return;
	}

	FInputModeGameOnly CombatMode;
	SetInputMode(CombatMode);
}

void AOmochaPlayerController::Server_SendChatMessage_Implementation(const FString& Message)
{
	if (AOmochaBaseGameMode* OmochaGM = Cast<AOmochaBaseGameMode>(UGameplayStatics::GetGameMode(this))) {
		OmochaGM->SendChatMessage(Message);
	}
}

void AOmochaPlayerController::Client_AddChatMessage_Implementation(const FString& Message)
{
	OnChatMessageArrived.Broadcast(Message);
}

void AOmochaPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(Context);

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (Subsystem) {
		Subsystem->AddMappingContext(Context, 0);
	}
}

void AOmochaPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	BindingInputAction();
}

void AOmochaPlayerController::ShowStatWidget()
{
	if (AOmochaHUD* OmochaHUD = Cast<AOmochaHUD>(GetHUD())) {
		OmochaHUD->ShowStatWidget();
	}
}

void AOmochaPlayerController::HideStatWidget()
{
	if (AOmochaHUD* OmochaHUD = Cast<AOmochaHUD>(GetHUD())) {
		OmochaHUD->HideStatWidget();
	}
}

void AOmochaPlayerController::ShowEffectItemToolTip(AActor* EffectItem)
{
	if (AOmochaEffectActor* Item = Cast<AOmochaEffectActor>(EffectItem)) {
		Item->ShowToolTip();
	}
}

void AOmochaPlayerController::HideEffectItemToolTip(AActor* EffectItem)
{
	if (IsValid(EffectItem)) {
		if (AOmochaEffectActor* Item = Cast<AOmochaEffectActor>(EffectItem)) {
			Item->HideToolTip();
		}
	}
}

UOmochaAbilitySystemComponent* AOmochaPlayerController::GetASC()
{
	if (AOmochaPlayerState* PS = GetPlayerState<AOmochaPlayerState>()) {
		return Cast<UOmochaAbilitySystemComponent>(PS->GetAbilitySystemComponent());
	}

	return Cast<UOmochaAbilitySystemComponent>(
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
}

void AOmochaPlayerController::Move(const struct FInputActionValue& InputActionValue)
{
	if (GetASC()
		&& (GetASC()->HasMatchingGameplayTag(FOmochaGameplayTags::Get().Status_Charging)
			|| GetASC()->HasMatchingGameplayTag(FOmochaGameplayTags::Get().Player_Block_Move))) {
		return;
	}
	FVector2D MovementVector = InputActionValue.Get<FVector2D>();

	if (APawn* ControlledPawn = GetPawn()) {
		ControlledPawn->AddMovementInput(FVector(1, 1, 0), MovementVector.Y);
		ControlledPawn->AddMovementInput(FVector(-1, 1, 0), MovementVector.X);
	}
}

void AOmochaPlayerController::CursorTrace()
{
	if (GetASC()
		&& (GetASC()->HasMatchingGameplayTag(FOmochaGameplayTags::Get().Status_Charging)
			|| GetASC()->HasMatchingGameplayTag(FOmochaGameplayTags::Get().Player_Block_CursorTrace))) {
		if (IsValid(LastActor)) {
			HideEffectItemToolTip(LastActor);
		}
		if (IsValid(ThisActor)) {
			HideEffectItemToolTip(ThisActor);
		}
		LastActor = nullptr;
		ThisActor = nullptr;
		return;
	}

	GetHitResultUnderCursor(ECC_MouseTrace, false, CursorHit);
	FHitResult ItemCursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, ItemCursorHit);
	if (!ItemCursorHit.bBlockingHit) {
		return;
	}

	LastActor = ThisActor;
	if (IsValid(ItemCursorHit.GetActor()) && Cast<AOmochaEffectActor>(ItemCursorHit.GetActor())) {
		ThisActor = ItemCursorHit.GetActor();
	}
	else {
		ThisActor = nullptr;
	}

	if (LastActor != ThisActor) {
		HideEffectItemToolTip(LastActor);
		ShowEffectItemToolTip(ThisActor);
	}
}

void AOmochaPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (GetASC()
		&& (GetASC()->HasMatchingGameplayTag(FOmochaGameplayTags::Get().Status_Charging)
			|| GetASC()->HasMatchingGameplayTag(FOmochaGameplayTags::Get().Player_Block_InputPressed))) {
		return;
	}
	if (GetASC()) {
		GetASC()->AbilityInputTagPressed(InputTag);
	}
}

void AOmochaPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FOmochaGameplayTags::Get().Player_Block_InputReleased)) {
		return;
	}
	if (GetASC()) {
		GetASC()->AbilityInputTagReleased(InputTag);
	}
}

void AOmochaPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetASC()
		&& (GetASC()->HasMatchingGameplayTag(FOmochaGameplayTags::Get().Status_Charging)
			|| GetASC()->HasMatchingGameplayTag(FOmochaGameplayTags::Get().Player_Block_InputHeld))) {
		return;
	}

	if (GetASC()) {
		GetASC()->AbilityInputTagHeld(InputTag);
	}
}

void AOmochaPlayerController::UpdateLookSystem(float DeltaTime)
{
	AOmochaPlayerCharacter* PlayerChar = Cast<AOmochaPlayerCharacter>(GetPawn());
	if (!PlayerChar) {
		return;
	}

	FVector MousePos = GetMouseWorldPosition();
	FVector CharacterPos = PlayerChar->GetActorLocation();

	FVector Direction = MousePos - CharacterPos;
	Direction.Z = 0.0f;

	if (Direction.Size2D() < MinLookDistance) {
		CurrentSpineRotation = LerpAngle(CurrentSpineRotation, 0.0f, DeltaTime, SpineRotationSpeed);
		PlayerChar->SetSpineRotation(CurrentSpineRotation);
		return;
	}

	Direction.Normalize();
	float TargetYaw = Direction.Rotation().Yaw;
	float CurrentYaw = GetControlRotation().Yaw;
	float AngleToTarget = AngleDifference(CurrentYaw, TargetYaw);

	constexpr float DeadZone = 1.0f;
	if (FMath::Abs(AngleToTarget) < DeadZone) {
		return;
	}

	float TargetSpineRotation = FMath::Clamp(AngleToTarget, -MaxSpineRotation, MaxSpineRotation);
	float OldSpineRotation = CurrentSpineRotation;
	CurrentSpineRotation = LerpAngle(CurrentSpineRotation, TargetSpineRotation, DeltaTime, SpineRotationSpeed);

	float NormalizedSpineRotation = FMath::UnwindDegrees(CurrentSpineRotation);

	PlayerChar->SetSpineRotation(NormalizedSpineRotation);

	constexpr float HysteresisThreshold = 5.0f;
	bool bShouldRotateBody = false;

	if (FMath::Abs(AngleToTarget) > MaxSpineRotation + HysteresisThreshold) {
		bShouldRotateBody = true;
	}
	else if (FMath::Abs(AngleToTarget) < MaxSpineRotation - HysteresisThreshold) {
		bShouldRotateBody = false;
	}
	else {
		static bool bWasRotatingBody = false;
		bShouldRotateBody = bWasRotatingBody;
		bWasRotatingBody = bShouldRotateBody;
	}

	if (bShouldRotateBody) {
		const float DesiredSpineAngle = FMath::Sign(AngleToTarget) * (MaxSpineRotation * 0.7f);
		float AbsoluteTargetYaw = TargetYaw - DesiredSpineAngle;
		float PreNormalizeTarget = AbsoluteTargetYaw;
		AbsoluteTargetYaw = FMath::UnwindDegrees(AbsoluteTargetYaw);

		// Smoothly rotate body to target
		float OldBodyYaw = CurrentYaw;
		float NewBodyYaw = LerpAngle(CurrentYaw, AbsoluteTargetYaw, DeltaTime, BodyRotationSpeed);
		SetControlRotation(FRotator(0, NewBodyYaw, 0));
	}
}

FVector AOmochaPlayerController::GetMouseWorldPosition() const
{
	if (!CursorHit.bBlockingHit) {
		APawn* ControlledPawn = GetPawn();
		return ControlledPawn ? ControlledPawn->GetActorLocation() : FVector::ZeroVector;
	}

	FVector MouseWorldPos = CursorHit.ImpactPoint;
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) {
		return MouseWorldPos;
	}

	FVector CharacterLocation = ControlledPawn->GetActorLocation();

	MouseWorldPos.Z = CharacterLocation.Z;

	FVector DiffVector = MouseWorldPos - CharacterLocation;
	DiffVector.Z = 0.0f;
	if (DiffVector.Size() > 1000.0f) {
		DiffVector.Normalize();
		MouseWorldPos = CharacterLocation + DiffVector * 1000.0f;
	}

	return MouseWorldPos;
}

// Improved angle calculation functions
float AOmochaPlayerController::NormalizeAngle(float Angle)
{
	// Use FMath::UnwindDegrees for more stable normalization
	return FMath::UnwindDegrees(Angle);
}

float AOmochaPlayerController::AngleDifference(float From, float To)
{
	// Use FMath::FindDeltaAngleDegrees for accurate angle difference
	return FMath::FindDeltaAngleDegrees(From, To);
}

float AOmochaPlayerController::LerpAngle(float Current, float Target, float DeltaTime, float Speed)
{
	if (Speed <= 0.0f) {
		return Target;
	}

	float Diff = AngleDifference(Current, Target);

	// Increase threshold to reduce jittering
	if (FMath::Abs(Diff) < 0.1f) {
		return Target;
	}

	// Use exponential interpolation for smoother movement
	float Alpha = 1.0f - FMath::Exp(-Speed * DeltaTime);
	float DeltaMove = Diff * Alpha;

	return NormalizeAngle(Current + DeltaMove);
}

void AOmochaPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* Target, bool bBlockedHit,
                                                              bool bCriticalHit)
{
	if (IsValid(Target) && DamageTextComponentClass && IsLocalController()) {
		UOmochaDamageTextComponent* DamageText = NewObject<
			UOmochaDamageTextComponent>(Target, DamageTextComponentClass);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(Target->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);
	}
}

void AOmochaPlayerController::SetInputBlocked(bool bBlocked)
{
	if (bBlocked && !bInputCurrentlyBlocked) {
		if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent)) {
			EIC->ClearActionBindings();
			bInputCurrentlyBlocked = true;
		}
	}
	else if (!bBlocked && bInputCurrentlyBlocked) {
		SetupInputComponent();
		bInputCurrentlyBlocked = false;
	}
}

void AOmochaPlayerController::Server_ChoosePlayerCharacter_Implementation(
	const FString& PlayerId, const ECharacterType CharacterType)
{
	if (AOmochaLobbyGameMode* OmochaGM = Cast<AOmochaLobbyGameMode>(UGameplayStatics::GetGameMode(this))) {
		OmochaGM->ChoosePlayerCharacter(PlayerId, CharacterType);
	}
}

void AOmochaPlayerController::Server_PlayerGetReady_Implementation(const FString& PlayerId, const bool Ready)
{
	if (AOmochaLobbyGameMode* OmochaGM = Cast<AOmochaLobbyGameMode>(UGameplayStatics::GetGameMode(this))) {
		OmochaGM->GetReady(PlayerId, Ready);
	}
}

void AOmochaPlayerController::ReadyForNextLevel(const bool Ready, const bool Save)
{
	if (!HasAuthority()) {
		return;
	}
	if (AOmochaBaseGameMode* OmochaGM = Cast<AOmochaBaseGameMode>(UGameplayStatics::GetGameMode(this))) {
		if (Ready) {
			OmochaGM->TravelToMap(Save);
		}
		else {
			OmochaGM->CancelTravel();
		}
	}
}


void AOmochaPlayerController::SetNextLevel(const FString& MapName)
{
	if (!HasAuthority()) {
		return;
	}
	if (AOmochaBaseGameMode* OmochaGM = Cast<AOmochaBaseGameMode>(UGameplayStatics::GetGameMode(this))) {
		OmochaGM->SetNextMap(MapName);
	}
}

void AOmochaPlayerController::StartSpectating()
{
	if (bIsSpectating) {
		return;
	}

	if (HasAuthority()) {
		TArray<APawn*> AvailavleTargets;

		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++
		     Iterator) {
			if (APlayerController* PC = Iterator->Get()) {
				if (APawn* PlayerPawn = PC->GetPawn()) {
					if (IsValid(PlayerPawn)) {
						AvailavleTargets.Add(PlayerPawn);
					}
				}
			}
		}

		Client_StartSpectating(AvailavleTargets);
	}
}

void AOmochaPlayerController::NextSpectatorTarget()
{
	if (!bIsSpectating || SpectatorTargets.Num() == 0) {
		return;
	}

	CurrentSpectatorIndex = (CurrentSpectatorIndex + 1) % SpectatorTargets.Num();

	SetViewTargetWithBlend(SpectatorTargets[CurrentSpectatorIndex], 0.5f);
}

void AOmochaPlayerController::RefreshSpectatorTargets()
{
	SpectatorTargets.Empty();

	TArray<AActor*> AllPlayerCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOmochaPlayerCharacter::StaticClass(), AllPlayerCharacters);

	for (AActor* Actor : AllPlayerCharacters) {
		if (APawn* PlayerPawn = Cast<APawn>(Actor)) {
			SpectatorTargets.Add(PlayerPawn);
		}
	}

	if (APawn* MyPawn = GetPawn()) {
		int32 MyIndex = SpectatorTargets.Find(MyPawn);
		if (MyIndex != INDEX_NONE && MyIndex != 0) {
			SpectatorTargets.Swap(0, MyIndex);
		}
	}

	if (CurrentSpectatorIndex >= SpectatorTargets.Num()) {
		CurrentSpectatorIndex = 0;
	}
}

void AOmochaPlayerController::StopSpectating()
{
	if (!bIsSpectating) {
		return;
	}

	bIsSpectating = false;

	if (SpectatorWidget && IsValid(SpectatorWidget)) {
		SpectatorWidget->RemoveFromParent();
		SpectatorWidget->ConditionalBeginDestroy();
		SpectatorWidget = nullptr;
	}

	SpectatorTargets.Empty();
	CurrentSpectatorIndex = 0;

	if (APawn* PlayerPawn = GetPawn()) {
		SetViewTargetWithBlend(PlayerPawn, 1.0f);
	}
}

void AOmochaPlayerController::Client_StartSpectating_Implementation(const TArray<APawn*>& AvailableTargets)
{
	if (bIsSpectating) {
		return;
	}

	bIsSpectating = true;
	SpectatorTargets = AvailableTargets;
	CurrentSpectatorIndex = 0;

	CreateSpectatorUI();

	if (SpectatorTargets.Num() > 0) {
		for (int32 i = 0; i < SpectatorTargets.Num(); i++) {
			if (SpectatorTargets[i] == GetPawn()) {
				if (i != 0) {
					SpectatorTargets.Swap(0, i);
				}
				break;
			}
		}

		CurrentSpectatorIndex = 0;
		SetViewTargetWithBlend(SpectatorTargets[0], 1.0f);
	}
}

void AOmochaPlayerController::Client_UpdateSpectatorTargets_Implementation(const TArray<APawn*>& AvailableTargets)
{
	if (bIsSpectating) {
		SpectatorTargets = AvailableTargets;

		if (CurrentSpectatorIndex >= SpectatorTargets.Num()) {
			CurrentSpectatorIndex = 0;
		}
	}
}

void AOmochaPlayerController::Client_StopSpectating_Implementation()
{
	StopSpectating();
}

void AOmochaPlayerController::HandleSpectatorNext()
{
}

FString AOmochaPlayerController::GetCurrentSpectatorTargetName() const
{
	if (SpectatorTargets.IsValidIndex(CurrentSpectatorIndex)) {
		APawn* CurrentTarget = SpectatorTargets[CurrentSpectatorIndex];
		if (CurrentTarget == GetPawn()) {
			return TEXT("Player");
		}
		if (APlayerController* TargetPC = Cast<APlayerController>(CurrentTarget->GetController())) {
			if (APlayerState* TargetPS = TargetPC->GetPlayerState<APlayerState>()) {
				return TargetPS->GetPlayerName();
			}
		}
		return CurrentTarget->GetName();
	}
	return TEXT("Unknown");
}


void AOmochaPlayerController::OnSpectatorNext()
{
	if (!IsLocalController()) {
		return;
	}

	if (AOmochaPlayerCharacter* PlayerChar = Cast<AOmochaPlayerCharacter>(GetPawn())) {
		if (PlayerChar->GetIsDead() && bIsSpectating) {
			NextSpectatorTarget();
		}
	}
}

void AOmochaPlayerController::CreateSpectatorUI()
{
	if (SpectatorWidgetClass && !SpectatorWidget) {
		SpectatorWidget = CreateWidget<USpectatorWidget>(this, SpectatorWidgetClass);
		if (SpectatorWidget) {
			SpectatorWidget->AddToViewport();
		}
	}
	else if (SpectatorWidget) {
		UE_LOG(LogTemp, Error, TEXT("SpectatorWidget already exists!"));
	}
}

void AOmochaPlayerController::AddXP(int32 Amount)
{
	if (AOmochaGameStateBase* GameState = GetWorld()->GetGameState<AOmochaGameStateBase>())
	{
		GameState->AddTeamXP(Amount);
	}
}

void AOmochaPlayerController::ShowMyAttributes()
{
	if (APawn* MyPawn = GetPawn())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MyPawn))
		{
			if (const UOmochaAttributeSet* AttributeSet = Cast<UOmochaAttributeSet>(ASC->GetAttributeSet(UOmochaAttributeSet::StaticClass())))
			{
				UE_LOG(LogTemp, Warning, TEXT("=== MY ATTRIBUTES ==="));
				UE_LOG(LogTemp, Warning, TEXT("AttributeSet Level: %.1f"), AttributeSet->GetLevel());
				UE_LOG(LogTemp, Warning, TEXT("Health: %.1f"), AttributeSet->GetHealth());
				UE_LOG(LogTemp, Warning, TEXT("AttackDamage: %.1f"), AttributeSet->GetAttackDamage());
				UE_LOG(LogTemp, Warning, TEXT("Shield: %.1f"), AttributeSet->GetShield());
			}
		}
	}
}

void AOmochaPlayerController::ShowTeamLevel()
{
	if (AOmochaGameStateBase* GameState = GetWorld()->GetGameState<AOmochaGameStateBase>())
	{
		UE_LOG(LogTemp, Warning, TEXT("=== TEAM LEVEL STATUS ==="));
		UE_LOG(LogTemp, Warning, TEXT("Current Team Level: %d"), GameState->TeamLevel);
		UE_LOG(LogTemp, Warning, TEXT("Current Team XP: %d"), GameState->TeamXP);
		UE_LOG(LogTemp, Warning, TEXT("XP for Next Level: %d"), GameState->GetXPRequiredForNextLevel());
		UE_LOG(LogTemp, Warning, TEXT("XP Progress: %.1f%%"), GameState->GetXPPercent() * 100.0f);
	}
}

void AOmochaPlayerController::ShowPersistent()
{
	if (UOmochaGameInstance* GameInst = GetGameInstance<UOmochaGameInstance>())
	{
		GameInst->ShowMainTainLevel(); 
	}
}