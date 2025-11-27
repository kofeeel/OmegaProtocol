// Copyright It's My Turn


#include "Subsystems/OmochaCheatManager.h"
#include "AbilitySystem/OmochaAbilityTypes.h"

void UOmochaCheatManager::TestContextSize()
{
	FOmochaGameplayEffectContext TestContext;
	TestContext.SetIsCriticalHit(true);
	TestContext.SetSkillBaseDamage(100.0f);
    
	TArray<uint8> Buffer;
	FMemoryWriter Writer(Buffer);
    
	bool bSuccess = false;
	TestContext.NetSerialize(Writer, nullptr, bSuccess);
    
	UE_LOG(LogTemp, Error, TEXT("[Manual Test] Context Size: %d bytes"), Buffer.Num());
}
