// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AliceEntry : ModuleRules
{
	public AliceEntry(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG", "NavigationSystem", "AIModule", "GameplayTasks", "CableComponent", "Niagara" });
		PrivateDependencyModuleNames.AddRange(new string[] { "CableComponent" });
	}
}
