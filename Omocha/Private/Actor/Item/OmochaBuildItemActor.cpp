// Copyright It's My Turn


#include "Actor/Item/OmochaBuildItemActor.h"

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
	
}

void AOmochaBuildItemActor::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AOmochaPlayerCharacter* PlayerCharacter = Cast<AOmochaPlayerCharacter>(OtherActor);
	if (PlayerCharacter && PlayerCharacter->IsLocallyControlled())
	{
		FGameplayTagContainer Container;

		switch (PlayerCharacter->GetPlayerCharterType())
		{
		case EPlayerCharter::Rusty:
			Container = RustyBuildTags;
			break;
		case EPlayerCharter::Vulcan:
			Container = VulcanBuildTags;
			break;
		case EPlayerCharter::Zenith:
			Container = ZenithBuildTags;
			break;
		}
		
		if (AOmochaPlayerState* PlayerState = Cast<AOmochaPlayerState>(PlayerCharacter->GetPlayerState()))
		{
			if (UOmochaSkillBuildComponent* Component = PlayerState->GetSkillBuildComponent())
			{
				Container.RemoveTags(Component->GetMasterBuildTagContainer());
				FSkillBuildInfos BuildData;
				
				for (int32 i = 0; i < 3; ++i)
				{
					if (Container.IsEmpty()) break;
					
					int32 RandomIndex = FMath::RandRange(0, Container.Num() - 1);
					FSkillBuildInfo BuildInfo;
					BuildInfo.BuildTag = Container.GetByIndex(RandomIndex);
					BuildInfo.BuildLevel =  Component->GetBuildLevel(BuildInfo.BuildTag) + 1;
					BuildData.Infos.Add(BuildInfo);
					Container.RemoveTag(BuildInfo.BuildTag);
				}

				if (AOmochaPlayerController* PlayerController = Cast<AOmochaPlayerController>(PlayerState->GetPlayerController()))
				{
					PlayerController->ChooseBuilds(BuildData);
				}
			}
		}
	}
}

void AOmochaBuildItemActor::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}


