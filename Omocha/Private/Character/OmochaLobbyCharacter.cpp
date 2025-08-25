// Copyright It's My Turn


#include "Character/OmochaLobbyCharacter.h"

#include "DataAsset/CharacterLobbyData.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystem.h"

void AOmochaLobbyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AOmochaLobbyCharacter, bIsReady_Replicated);
	DOREPLIFETIME(AOmochaLobbyCharacter, MyCharacterType);
}

// Sets default values
AOmochaLobbyCharacter::AOmochaLobbyCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	HeadStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadStaticMeshComponent"));
	HeadStaticMeshComponent->SetupAttachment(GetMesh());
	HeadStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AOmochaLobbyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AOmochaLobbyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsCinematicRunning) {
		FVector ForwardDirection = GetActorForwardVector();
		AddActorWorldOffset(ForwardDirection * CinematicRunSpeed * DeltaTime, true);
	}
}

// Called to bind functionality to input
void AOmochaLobbyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AOmochaLobbyCharacter::TriggerTransformEffect()
{
	if (!CharacterLobbyDataTable) {
		return;
	}

	int32 EnumAsInt = static_cast<int32>(MyCharacterType) - 2;
	FName RowName = FName(FString::FromInt(EnumAsInt));

	static const FString ContextString(TEXT("CharacterLobbyDataContext"));
	FCharacterLobbyData* LobbyData = CharacterLobbyDataTable->FindRow<FCharacterLobbyData>(RowName, ContextString);

	if (LobbyData) {
		if (LobbyData->TransformEffect.IsPending() || LobbyData->TransformEffect.Get()) {
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				LobbyData->TransformEffect.LoadSynchronous(),
				GetActorLocation(),
				GetActorRotation()
			);
		}

		USkeletalMesh* NewMesh = LobbyData->TransformedMesh.LoadSynchronous();
		TSubclassOf<UAnimInstance> NewAnimBP = LobbyData->TransformedAnimationBlueprint;

		if (NewMesh && NewAnimBP) {
			Multicast_UpdateVisuals(NewMesh, NewAnimBP);
		}

		if (UStaticMesh* NewHeadMesh = LobbyData->TransformedHeadMesh.LoadSynchronous()) {
			HeadStaticMeshComponent->SetStaticMesh(NewHeadMesh);
		}
		else {
			HeadStaticMeshComponent->SetStaticMesh(nullptr);
		}

		if (GetMesh()->DoesSocketExist(FName("Socket.Head")) && HeadStaticMeshComponent->GetStaticMesh()) {
			HeadStaticMeshComponent->AttachToComponent(
				GetMesh(),
				FAttachmentTransformRules::KeepRelativeTransform,
				FName("Socket.Head")
			);

			HeadStaticMeshComponent->SetRelativeTransform(LobbyData->OffsetHeadTransform);
		}
	}
}

void AOmochaLobbyCharacter::Multicast_StartRunCinematic_Implementation()
{
	bIsCinematicRunning = true;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance) {
		if (FProperty* Prop = AnimInstance->GetClass()->FindPropertyByName(FName("bIsCinematicRunning"))) {
			if (FBoolProperty* BoolProp = CastField<FBoolProperty>(Prop)) {
				BoolProp->SetPropertyValue_InContainer(AnimInstance, true);
			}
		}
	}

	GetWorld()->GetTimerManager().SetTimer(
		TransformTimerHandle,
		this,
		&AOmochaLobbyCharacter::TriggerTransformEffect,
		TransformDelay,
		false);
}

void AOmochaLobbyCharacter::Multicast_UpdateVisuals_Implementation(USkeletalMesh* NewMesh,
                                                                   TSubclassOf<UAnimInstance> NewAnimBP)
{
	if (NewMesh) {
		GetMesh()->SetSkeletalMesh(NewMesh);
	}
	if (NewAnimBP) {
		GetMesh()->SetAnimInstanceClass(NewAnimBP);
	}
}
