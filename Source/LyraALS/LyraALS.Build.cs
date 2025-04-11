// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class LyraALS : ModuleRules
{
	public LyraALS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(["AnimGraphRuntime", "EnhancedInput"]);
		PublicDependencyModuleNames.AddRange(["Core", "CoreUObject", "Engine", "InputCore"]);

		PublicIncludePaths.Add("LyraALS/");
	}
}