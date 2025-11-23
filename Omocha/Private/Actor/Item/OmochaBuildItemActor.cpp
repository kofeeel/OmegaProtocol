// Copyright It's My Turn


#include "Actor/Item/OmochaBuildItemActor.h"

#include "OmochaGameplayTags.h"
#include "Character/OmochaPlayerCharacter.h"
#include "Component/OmochaSkillBuildComponent.h"
#include "Components/SphereComponent.h"
#include "Player/OmochaPlayerController.h"

class AOmochaPlayerCharacter;

AOmochaBuildItemActor::AOmochaBuildItemActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("TooltipSphere"));
	SetRootComponent(Sphere);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AOmochaBuildItemActor::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AOmochaBuildItemActor::OnSphereBeginOverlap);
	
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AOmochaBuildItemActor::OnSphereEndOverlap);
	
}

void AOmochaBuildItemActor::InitializeBuildTags(AOmochaPlayerCharacter* PlayerCharacter)
{
	FGameplayTagContainer BuildOmegaTags;
	FGameplayTagContainer BuildOmicronTags;
	
	switch (PlayerCharacter->GetPlayerCharterType())
	{
	case EPlayerCharter::Rusty:
		BuildOmegaTags = RustyOmegaBuildTags;
		BuildOmicronTags = RustyOmicronBuildTags;
		break;
	case EPlayerCharter::Vulcan:
		BuildOmegaTags = VulcanOmegaBuildTags;
		BuildOmicronTags = VulcanOmicronBuildTags;
		break;
	case EPlayerCharter::Zenith:
		BuildOmegaTags = ZenithOmegaBuildTags;
		BuildOmicronTags = ZenithOmicronBuildTags;
		break;
	}
		
	if (AOmochaPlayerState* PlayerState = Cast<AOmochaPlayerState>(PlayerCharacter->GetPlayerState()))
	{
		if (UOmochaSkillBuildComponent* Component = PlayerState->GetSkillBuildComponent())
		{
			FGameplayTagContainer MasterTags = Component->GetMasterBuildTagContainer();
			BuildOmegaTags.RemoveTags(MasterTags);
			BuildOmicronTags.RemoveTags(MasterTags);
			BuildOmegaData.Infos.Empty();
			BuildOmicronData.Infos.Empty();

			PickCards(Component, BuildOmegaTags, BuildOmegaData);
			PickCards(Component, BuildOmicronTags, BuildOmicronData);

			if (AOmochaPlayerController* PlayerController = Cast<AOmochaPlayerController>(PlayerState->GetPlayerController()))
			{
				if (PlayerCharacter->PlayerState == FOmochaGameplayTags::Get().State_Normal)
				{
					PlayerController->ChooseBuilds(BuildOmicronData);
				}
				else if (PlayerCharacter->PlayerState == FOmochaGameplayTags::Get().State_Omega)
				{
					PlayerController->ChooseBuilds(BuildOmegaData);
				}
				PlayerController->BuildPickedDelegate.AddDynamic(this, &AOmochaBuildItemActor::OnPickedBuildCard);
				Player = PlayerController;
			}
		}
	}

	Init = true;
}

void AOmochaBuildItemActor::FixedInitializeBuildTags(AOmochaPlayerCharacter* PlayerCharacter)
{
	if (AOmochaPlayerState* PlayerState = Cast<AOmochaPlayerState>(PlayerCharacter->GetPlayerState()))
	{
		if (UOmochaSkillBuildComponent* Component = PlayerState->GetSkillBuildComponent())
		{
			ModifyBuildCardLevels(Component, BuildOmegaData);
			ModifyBuildCardLevels(Component, BuildOmicronData);

			if (AOmochaPlayerController* PlayerController = Cast<AOmochaPlayerController>(PlayerState->GetPlayerController()))
			{
				if (PlayerCharacter->PlayerState == FOmochaGameplayTags::Get().State_Normal)
				{
					PlayerController->ChooseBuilds(BuildOmicronData);
				}
				else if (PlayerCharacter->PlayerState == FOmochaGameplayTags::Get().State_Omega)
				{
					PlayerController->ChooseBuilds(BuildOmegaData);
				}
				PlayerController->BuildPickedDelegate.AddDynamic(this, &AOmochaBuildItemActor::OnPickedBuildCard);
				Player = PlayerController;
			}
		}
	}
}

void AOmochaBuildItemActor::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AOmochaPlayerCharacter* PlayerCharacter = Cast<AOmochaPlayerCharacter>(OtherActor);
	if (PlayerCharacter && PlayerCharacter->IsLocallyControlled())
	{
		Closed = false;
		if (!Init || !Fixed)
		{
			InitializeBuildTags(PlayerCharacter);
		}
		else
		{
			FixedInitializeBuildTags(PlayerCharacter);
		}
	}
}

void AOmochaBuildItemActor::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AOmochaPlayerCharacter* PlayerCharacter = Cast<AOmochaPlayerCharacter>(OtherActor);
	if (PlayerCharacter && PlayerCharacter->IsLocallyControlled())
	{
		if (IsValid(Player))
		{
			Player->BuildPickedDelegate.RemoveDynamic(this, &AOmochaBuildItemActor::OnPickedBuildCard);
			if (!Closed) Player->CloseBuilds();
		}
	}
}

void AOmochaBuildItemActor::OnPickedBuildCard()
{
	Closed = true;
	if (!Permanent || Fixed)
	{
		if (IsValid(Player))
		{
			Player->BuildPickedDelegate.RemoveDynamic(this, &AOmochaBuildItemActor::OnPickedBuildCard);
		}
		Destroy();
	}
}

void AOmochaBuildItemActor::PickCards(UOmochaSkillBuildComponent* Component, FGameplayTagContainer& BuildTags, FSkillBuildInfos& BuildData)
{
	if (!IsValid(Component)) return;
	
	for (int32 i = 0; i < CardAmount; ++i)
	{
		FSkillBuildInfo BuildInfo;
		if (BuildTags.IsEmpty())
		{
			BuildData.Infos.Add(BuildInfo);
		}
		else
		{
			int32 RandomIndex = FMath::RandRange(0, BuildTags.Num() - 1);
					
			BuildInfo.BuildTag = BuildTags.GetByIndex(RandomIndex);
			BuildInfo.BuildLevel =  Component->GetBuildLevel(BuildInfo.BuildTag) + 1;
			BuildData.Infos.Add(BuildInfo);
			BuildTags.RemoveTag(BuildInfo.BuildTag);
		}
	}
}

void AOmochaBuildItemActor::ModifyBuildCardLevels(UOmochaSkillBuildComponent* Component, FSkillBuildInfos& BuildData)
{
	if (!IsValid(Component)) return;
	
	FGameplayTagContainer BuildTags;
	for (FSkillBuildInfo& Info : BuildData.Infos)
	{
		BuildTags.AddTag(Info.BuildTag);
		Info.BuildLevel = 0;
	}
	BuildTags.RemoveTags(Component->GetMasterBuildTagContainer());

	for (FSkillBuildInfo& Info : BuildData.Infos)
	{
		if (BuildTags.HasTagExact(Info.BuildTag))
		{
			Info.BuildLevel = Component->GetBuildLevel(Info.BuildTag) + 1;
		}
		else
		{
			Info.BuildTag = FGameplayTag::EmptyTag;
		}
	}
}


