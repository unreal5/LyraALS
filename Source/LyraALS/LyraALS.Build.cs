// Lyra高级运动系统

using UnrealBuildTool;

public class LyraALS : ModuleRules
{
	public LyraALS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { "EnhancedInput", "AnimGraphRuntime", "AnimGraphRuntime" });

		PublicIncludePaths.AddRange(new string[] { "LyraALS/" });
	}
}