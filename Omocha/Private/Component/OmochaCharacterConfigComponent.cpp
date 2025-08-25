// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/OmochaCharacterConfigComponent.h"
#include "AbilitySystemComponent.h"
#include "OmochaGameplayTags.h"
#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/OmochaGameplayAbility.h"
#include "Character/OmochaPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "DataAsset/OmochaCharacterDataAsset.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UOmochaCharacterConfigComponent::UOmochaCharacterConfigComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	NormalStateTag = FOmochaGameplayTags::Get().State_Normal;
	OmegaStateTag = FOmochaGameplayTags::Get().State_Omega;
	DeadStateTag = FOmochaGameplayTags::Get().State_Dead;
}

void UOmochaCharacterConfigComponent::InitializeState(const FGameplayTag& StateTag)
{
	if (GetOwner()->HasAuthority() && CurrentCharacterData) {
		ChangeState(StateTag);
	}
}

// Called when the game starts
void UOmochaCharacterConfigComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UOmochaCharacterConfigComponent::SetCharacterData(UOmochaCharacterDataAsset* NewCharacterData)
{
	if (NewCharacterData) {
		CurrentCharacterData = NewCharacterData;
		if (GetOwner()->HasAuthority()) {
			ChangeState(NormalStateTag);
		}
		FGameplayTag TargetState = CurrentState.IsValid() ? CurrentState : NormalStateTag;
		SwapVisuals(TargetState);
	}
}

void UOmochaCharacterConfigComponent::ChangeState(FGameplayTag StateTag)
{
	if (!StateTag.IsValid()) { return; }
	if (CurrentState.MatchesTagExact(StateTag)) { return; }

	if (GetOwner()->HasAuthority()) {
		CurrentState = StateTag;

		SwapAbilities(CurrentState);
		SwapAttributes(CurrentState);
		bool bToOmegaState = (StateTag.MatchesTagExact(OmegaStateTag));
		FVector CharacterLocation = GetOwner()->GetActorLocation();
		Multicast_PlayerTransformEffects(bToOmegaState, CharacterLocation);
	}
}

FGameplayTag UOmochaCharacterConfigComponent::GetCharacterType() const
{
	return CurrentCharacterData ? CurrentCharacterData->CharacterTag : FGameplayTag();
}

bool UOmochaCharacterConfigComponent::IsInOmegaState() const
{
	return CurrentState.MatchesTagExact(OmegaStateTag);
}

bool UOmochaCharacterConfigComponent::IsInNormalState() const
{
	return CurrentState.MatchesTagExact(NormalStateTag);
}

void UOmochaCharacterConfigComponent::GetLifetimeReplicatedProps(
	TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UOmochaCharacterConfigComponent, CurrentState)
	DOREPLIFETIME(UOmochaCharacterConfigComponent, CurrentCharacterData)
}

void UOmochaCharacterConfigComponent::SwapAbilities(FGameplayTag StateTag)
{
	if (!CurrentCharacterData) { return; }

	AOmochaPlayerCharacter* Character = Cast<AOmochaPlayerCharacter>(GetOwner());
	if (!Character) { return; }

	UOmochaAbilitySystemComponent* ASC = Cast<UOmochaAbilitySystemComponent>(Character->GetAbilitySystemComponent());
	if (!ASC) { return; }

	TArray<FGameplayAbilitySpecHandle> HandlesToRemove;
	const FGameplayTag LeftMouseButtonTag = FOmochaGameplayTags::Get().InputTag_Ability_LMB;

	for (FGameplayAbilitySpecHandle Handle : ActiveAbilityHandles) {
		if (Handle.IsValid()) {
			const FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromHandle(Handle);
			if (Spec && !Spec->DynamicAbilityTags.HasTagExact(LeftMouseButtonTag)) {
				HandlesToRemove.Add(Handle);
			}
		}
	}

	for (FGameplayAbilitySpecHandle Handle : HandlesToRemove) {
		ASC->ClearAbility(Handle);
		ActiveAbilityHandles.Remove(Handle);
	}

	// New abilities to add based on the state
	TArray<TSubclassOf<UGameplayAbility>> NewAbilities;

	if (StateTag.MatchesTagExact(OmegaStateTag)) {
		NewAbilities = CurrentCharacterData->OmegaAbilities;
	}
	else if (StateTag.MatchesTagExact(NormalStateTag)) {
		NewAbilities = CurrentCharacterData->NormalAbilities;
	}
	else {
		return;
	}

	// Add new abilities
	for (TSubclassOf<UGameplayAbility> AbilityClass : NewAbilities) {
		if (AbilityClass) {
			FGameplayAbilitySpec AbilitySpec(AbilityClass, 1);

			if (const UOmochaGameplayAbility* Ability = Cast<UOmochaGameplayAbility>(AbilitySpec.Ability)) {
				AbilitySpec.GetDynamicSpecSourceTags().AddTag(Ability->InputGameplayTag);
			}

			FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(AbilitySpec);
			ActiveAbilityHandles.Add(Handle);
		}
	}
	ASC->AbilitiesGivenDelegate.Broadcast();
	ASC->Client_BroadcastAbilityInfos(ASC->GetActivatableAbilities());
}

void UOmochaCharacterConfigComponent::SwapVisuals(FGameplayTag StateTag)
{
	if (!CurrentCharacterData) {
		return;
	}

	AOmochaPlayerCharacter* Character = Cast<AOmochaPlayerCharacter>(GetOwner());
	if (!Character) {
		return;
	}

	USkeletalMeshComponent* MeshComp = Character->GetMesh();
	if (!MeshComp) {
		return;
	}

	// mesh change
	USkeletalMesh* TargetMesh = nullptr;
	TSubclassOf<UAnimInstance> TargetAnimBP = nullptr;

	if (StateTag.MatchesTagExact(OmegaStateTag)) {
		TargetMesh = CurrentCharacterData->OmegaMesh;
		TargetAnimBP = CurrentCharacterData->OmegaAnimBP;
	}
	else if (StateTag.MatchesTagExact(NormalStateTag)) {
		TargetMesh = CurrentCharacterData->NormalMesh;
		TargetAnimBP = CurrentCharacterData->NormalAnimBP;
	}

	if (TargetMesh) {
		MeshComp->SetSkeletalMesh(TargetMesh);
	}

	if (TargetAnimBP) {
		MeshComp->SetAnimInstanceClass(TargetAnimBP);
	}

	if (StateTag.MatchesTagExact(OmegaStateTag)) {
		if (UStaticMeshComponent* HeadComp = Character->GetHeadStaticMeshComponent()) {
			HeadComp->SetStaticMesh(CurrentCharacterData->OmegaHeadMesh);

			if (MeshComp->DoesSocketExist("Socket.Head") && HeadComp->GetStaticMesh()) {
				HeadComp->AttachToComponent(
					MeshComp,
					FAttachmentTransformRules::KeepRelativeTransform,
					"Socket.Head"
				);

				HeadComp->SetRelativeTransform(CurrentCharacterData->GetOmegaHeadMeshTransform());
			}
		}
	}
	else if (StateTag.MatchesTagExact(NormalStateTag)) {
		if (UStaticMeshComponent* HeadComp = Character->GetHeadStaticMeshComponent()) {
			HeadComp->SetStaticMesh(nullptr);
			HeadComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		}
	}

	if (Character) {
		Character->UpdateStateVisuals(StateTag);
	}

	ApplyStateScaleAndCapsule(StateTag);
}

void UOmochaCharacterConfigComponent::OnRep_CurrentState()
{
	AOmochaPlayerCharacter* PlayerCharacter = Cast<AOmochaPlayerCharacter>(GetOwner());
	if (PlayerCharacter && PlayerCharacter->GetMesh()) {
		PlayerCharacter->UpdateStateVisuals(CurrentState);

		if (UAnimInstance* AnimInstance = PlayerCharacter->GetMesh()->GetAnimInstance()) {
			bool bIsMontageePlaying = AnimInstance->IsAnyMontagePlaying();

			if (!bIsMontageePlaying) {
				SwapVisuals(CurrentState);
			}
		}
		else {
			SwapVisuals(CurrentState);
		}
	}
}

void UOmochaCharacterConfigComponent::OnRep_CurrentCharacterData()
{
	if (CurrentCharacterData) {
		if (CurrentState.IsValid()) {
			SwapVisuals(CurrentState);
		}
		else {
			SwapVisuals(NormalStateTag);
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("OnRep_CurrentCharacterData: Data is NULL"));
	}
}

void UOmochaCharacterConfigComponent::ApplyStateScaleAndCapsule(FGameplayTag StateTag)
{
	if (!CurrentCharacterData) {
		return;
	}

	AOmochaPlayerCharacter* Character = Cast<AOmochaPlayerCharacter>(GetOwner());
	if (!Character) {
		return;
	}

	UCapsuleComponent* Capsule = Character->GetCapsuleComponent();
	if (!Capsule) {
		return;
	}

	bool bIsOmega = StateTag.MatchesTagExact(OmegaStateTag);

	Capsule->SetWorldScale3D(FVector(CurrentCharacterData->GetScaleForState(bIsOmega)));

	FVector CapsuleLocation = Capsule->GetRelativeLocation();
	// CapsuleLocation.Z = Capsule->GetScaledCapsuleHalfHeight();

	if (bIsOmega) {
		float ScaleDifference = CurrentCharacterData->OmegaScale - CurrentCharacterData->NormalScale;
		float HeightDifference = Capsule->GetUnscaledCapsuleHalfHeight() * ScaleDifference;
		CapsuleLocation.Z += HeightDifference;
	}

	Capsule->SetRelativeLocation(CapsuleLocation);
}

void UOmochaCharacterConfigComponent::SwapAttributes(FGameplayTag StateTag)
{
	if (AOmochaPlayerCharacter* Character = Cast<AOmochaPlayerCharacter>(GetOwner())) {
		Character->ApplyOmegaAttributeOrRemove(StateTag);
	}
}

void UOmochaCharacterConfigComponent::Multicast_PlayerTransformEffects_Implementation(bool bToOmegaState,
	FVector PlayerLocation)
{
	if (!CurrentCharacterData) {
		UE_LOG(LogTemp, Error, TEXT("Multicast_PlayerTransformEffects: CurrentCharacterData is NULL!"));
		return;
	}

	FGameplayTag TargetState = bToOmegaState ? OmegaStateTag : NormalStateTag;

	SwapVisuals(TargetState);

	AOmochaPlayerCharacter* PlayerCharacter = Cast<AOmochaPlayerCharacter>(GetOwner());
	if (!PlayerCharacter) {
		return;
	}

	PlayerCharacter->PlayTransformEffects(bToOmegaState, PlayerLocation, CurrentCharacterData);

	USoundBase* SoundToPlay = CurrentCharacterData->GetTransformSound(bToOmegaState);

	if (SoundToPlay) {
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			SoundToPlay,
			PlayerLocation,
			1.0f,
			1.0f
		);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("in Data Asset no sound set: %s"),
		       bToOmegaState ? TEXT("Omega") : TEXT("Normal"));
	}

	UAnimMontage* MontageToPlay = CurrentCharacterData->GetTransformMontage(bToOmegaState);
	float MontageLength = 1.0f;

	if (MontageToPlay) {
		if (UAnimInstance* AnimInstance = PlayerCharacter->GetMesh()->GetAnimInstance()) {
			MontageLength = MontageToPlay->GetPlayLength();
			AnimInstance->Montage_Play(MontageToPlay);
		}
	}

	PlayerCharacter->OnTransformAnimationStarted(MontageLength);
}
