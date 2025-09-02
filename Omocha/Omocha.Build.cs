// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Omocha : ModuleRules
{
	public Omocha(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "NiagaraCore", "Paper2D", "Slate",
			"SlateCore", "LevelSequence", "MovieScene", "GameAnalytics", "GameplayAbilities"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"GameplayAbilities", "GameplayTags", "GameplayTasks", "NavigationSystem", "Niagara", "AIModule", "UMG",
			"OnlineSubsystemSteam", "OnlineSubsystem", "Steamworks", "GameAnalytics"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}