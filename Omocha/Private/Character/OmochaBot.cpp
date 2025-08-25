// Fill out your copyright notice in the Description page of Project Settings.



#include "Character/OmochaBot.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/OmochaAbilitySystemComponent.h"
#include "AbilitySystem/OmochaAttributeSet.h"
#include "Components/CapsuleComponent.h"


AOmochaBot::AOmochaBot()
{
	PrimaryActorTick.bCanEverTick = false;
	AbilitySystemComponent = CreateDefaultSubobject<UOmochaAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UOmochaAttributeSet>(TEXT("AttributeSet"));
}

void AOmochaBot::BeginPlay()
{
	Super::BeginPlay();
	InitAbilityActorInfo();
}

void AOmochaBot::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitializeAttributes();
}

void AOmochaBot::ReviveBot()
{
	if (!bDead) return;

	bDead = false;
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AOmochaBot::Die(const FVector& DeathImpulse)
{
	Super::Die();

	GetWorldTimerManager().SetTimer(ReviveTimer, this, &AOmochaBot::ReviveBot, 3.f, false);
}
