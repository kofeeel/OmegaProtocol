// Copyright It's My Turn

#include "Actor/OmochaAreaEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SphereComponent.h"
#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "AbilitySystem/OmochaAttributeSet.h"

AOmochaAreaEffectActor::AOmochaAreaEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	EffectSphere = CreateDefaultSubobject<USphereComponent>(TEXT("EffectSphere"));
	RootComponent = EffectSphere;
	EffectSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	EffectSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	EffectSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	AbilitySystemComponent = CreateDefaultSubobject<UOmochaAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UOmochaAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AOmochaAreaEffectActor::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AOmochaAreaEffectActor::BeginPlay()
{
	Super::BeginPlay();
	InitAbilityActorInfo();
	EffectSphere->OnComponentBeginOverlap.AddDynamic(this, &AOmochaAreaEffectActor::OnOverlap);
}

void AOmochaAreaEffectActor::InitAbilityActorInfo()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

void AOmochaAreaEffectActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!EffectAreaRowHandle.DataTable || EffectAreaRowHandle.RowName.IsNone())	{ return; }

	FEffectAreaData* EffectData = EffectAreaRowHandle.DataTable->FindRow<FEffectAreaData>(EffectAreaRowHandle.RowName, TEXT(""));
	if (!EffectData || !EffectData->GameplayEffect) { return; }

	if (OtherActor->ActorHasTag(FName("Enemy"))) { return; }
	ApplyEffectToTarget(OtherActor, EffectData->GameplayEffect);

	if (EffectData->GameplayCueTag.IsValid())
	{
		ExecuteGameplayCueOnTarget(OtherActor, EffectData->GameplayCueTag);
	}
}

void AOmochaAreaEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> EffectClass)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!IsValid(TargetASC))	{ return; }

	check(EffectClass);
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(
		EffectClass, 1.0f, EffectContextHandle);
	TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}

void AOmochaAreaEffectActor::ExecuteGameplayCueOnTarget(AActor* TargetActor, const FGameplayTag& CueTag)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!IsValid(TargetASC))	{ return; }

	FGameplayCueParameters CueParams;
	CueParams.Location = TargetActor->GetActorLocation();
	CueParams.Instigator = this;
	TargetASC->ExecuteGameplayCue(CueTag, CueParams);
}
