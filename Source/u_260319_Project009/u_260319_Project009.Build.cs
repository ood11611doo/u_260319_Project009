// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class u_260319_Project009 : ModuleRules
{
	public u_260319_Project009(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "Slate", "SlateCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		PublicIncludePaths.AddRange(new string[] { "u_260319_Project009" });
	}
}
