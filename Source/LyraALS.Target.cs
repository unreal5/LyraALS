// Lyra高级运动系统

using UnrealBuildTool;
using System.Collections.Generic;

public class LyraALSTarget : TargetRules
{
	public LyraALSTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "LyraALS" } );
	}
}
