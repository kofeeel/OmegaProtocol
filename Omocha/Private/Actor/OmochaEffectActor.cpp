// Copyright It's My Turn


#include "Actor/OmochaEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/Item/ItemTypes.h"
#include "Character/OmochaPlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Omocha/Omocha.h"
#include "UI/Widget/OmochaItemTooltipWidget.h"


AOmochaEffectActor::AOmochaEffectActor()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));

	TooltipSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TooltipSphere"));
	TooltipSphere->SetupAttachment(GetRootComponent());
	TooltipSphere->SetSphereRadius(250.f);
	TooltipSphere->SetCollisionProfileName(FName("Trigger"));
	TooltipSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	TooltipSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	TooltipSphere->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	TooltipSphere->SetCollisionResponseToChannel(ECC_MouseTrace, ECR_Block);
	
	TooltipWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("TooltipWidget"));
	TooltipWidgetComponent->SetupAttachment(GetRootComponent());
	TooltipWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	TooltipWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 300.f));
	TooltipWidgetComponent->SetDrawAtDesiredSize(true);
	TooltipWidgetComponent->SetVisibility(false);

	RootComponent->SetIsReplicated(true);
}

void AOmochaEffectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;
	const float SinePeriod = 2 * PI / SinePeriodConstant;
	if (RunningTime > SinePeriod)
	{
		RunningTime = 0.f;
	}
	ItemMovement(DeltaTime);
}

void AOmochaEffectActor::ItemMovement(float DeltaTime)
{
	if (HasAuthority())
	{
		SetActorLocation(CalculatedLocation);
		SetActorRotation(CalculatedRotation);
	}
	if (bRotates)
	{
		const FRotator DeltaRotation(0.f, DeltaTime * RotationRate, 0.f);
		CalculatedRotation = UKismetMathLibrary::ComposeRotators(CalculatedRotation, DeltaRotation);
	}
	if (bSinusoidalMovement)
	{
		const float Sine = SineAmplitude * FMath::Sin(RunningTime * SinePeriodConstant);
		CalculatedLocation = InitialLocation + FVector(0.f, 0.f, Sine);
	}
	if (TooltipWidgetComponent->IsVisible())
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC && PC->PlayerCameraManager)
		{
			FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(
				TooltipWidgetComponent->GetComponentLocation(), CameraLocation);

			TooltipWidgetComponent->SetWorldRotation(LookAtRotation);
		}
	}
}

void AOmochaEffectActor::BeginPlay()
{
	Super::BeginPlay();
	InitialLocation = GetActorLocation();
	CalculatedLocation = InitialLocation;
	CalculatedRotation = GetActorRotation();

	if (bUseMouseHoverForTooltip)
	{
		TooltipSphere->OnBeginCursorOver.AddDynamic(this, &AOmochaEffectActor::OnMouseBeginHover);
		TooltipSphere->OnEndCursorOver.AddDynamic(this, &AOmochaEffectActor::OnMouseEndHover);
	}
	else
	{
		TooltipSphere->OnComponentBeginOverlap.AddDynamic(this, &AOmochaEffectActor::OnSphereBeginOverlap);
		TooltipSphere->OnComponentEndOverlap.AddDynamic(this, &AOmochaEffectActor::OnSphereEndOverlap);
	}
}

void AOmochaEffectActor::StartSinusoidalMovement()
{
	bSinusoidalMovement = true;
	InitialLocation = GetActorLocation();
	CalculatedLocation = InitialLocation;
}

void AOmochaEffectActor::StartRotation()
{
	bRotates = true;
	CalculatedRotation = GetActorRotation();
}

void AOmochaEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies)
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC == nullptr)
	{
		return;
	}

	check(GameplayEffectClass);
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(
		GameplayEffectClass, ActorLevel, EffectContextHandle);
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(
		*EffectSpecHandle.Data.Get());

	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy ==
		EGameplayEffectDurationType::Infinite;
	if (bIsInfinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	}

	if ((!bIsInfinite && !bDestroyOnEffectApplication))
	{
		return;
	}

	if (!bIsInfinite)
	{
		Destroy();
	}
}

void AOmochaEffectActor::OnOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void AOmochaEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC))
		{
			return;
		}

		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		for (TPair<FActiveGameplayEffectHandle, UAbilitySystemComponent*> HandlePair : ActiveEffectHandles)
		{
			if (TargetASC == HandlePair.Value)
			{
				TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
				HandlesToRemove.Add(HandlePair.Key);
			}
		}
		for (auto& Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
	}
}

void AOmochaEffectActor::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                              const FHitResult& SweepResult)
{
	AOmochaPlayerCharacter* PlayerCharacter = Cast<AOmochaPlayerCharacter>(OtherActor);
	if (PlayerCharacter && PlayerCharacter->IsLocallyControlled())
	{
		const FDataTableRowHandle RowHandle = GetItemDataRowHandle();
		if (RowHandle.DataTable && !RowHandle.RowName.IsNone())
		{
			FItemData* ItemData = RowHandle.DataTable->FindRow<FItemData>(RowHandle.RowName, "");
			if (ItemData && TooltipWidgetComponent)
			{
				UOmochaItemTooltipWidget* TooltipWidget = Cast<UOmochaItemTooltipWidget>(
					TooltipWidgetComponent->GetUserWidgetObject());
				if (TooltipWidget)
				{
					TooltipWidget->SetItemData(*ItemData);
					TooltipWidgetComponent->SetVisibility(true);
				}
			}
		}
	}
}

void AOmochaEffectActor::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AOmochaPlayerCharacter* PlayerCharacter = Cast<AOmochaPlayerCharacter>(OtherActor);
	if (PlayerCharacter && PlayerCharacter->IsLocallyControlled())
	{
		TooltipWidgetComponent->SetVisibility(false);
	}
}

void AOmochaEffectActor::OnMouseBeginHover(UPrimitiveComponent* TouchedComponent)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0); PC && PC->IsLocalController())
	{
		ShowToolTip();
	}
}

void AOmochaEffectActor::OnMouseEndHover(UPrimitiveComponent* TouchedComponent)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0); PC && PC->IsLocalController())
	{
		HideToolTip();
	}
}


void AOmochaEffectActor::ShowToolTip()
{
	const FDataTableRowHandle RowHandle = GetItemDataRowHandle();
	if (RowHandle.DataTable && !RowHandle.RowName.IsNone())
	{
		FItemData* ItemData = RowHandle.DataTable->FindRow<FItemData>(RowHandle.RowName, "");
		if (ItemData && TooltipWidgetComponent)
		{
			UOmochaItemTooltipWidget* TooltipWidget = Cast<UOmochaItemTooltipWidget>(
				TooltipWidgetComponent->GetUserWidgetObject());
			if (TooltipWidget)
			{
				TooltipWidget->SetItemData(*ItemData);
				TooltipWidgetComponent->SetVisibility(true);
			}
		}
	}
}

void AOmochaEffectActor::HideToolTip()
{
	TooltipWidgetComponent->SetVisibility(false);
}