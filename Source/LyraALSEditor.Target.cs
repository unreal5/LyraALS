// Lyra高级运动系统

using UnrealBuildTool;
using System.Collections.Generic;

public class LyraALSEditorTarget : TargetRules
{
	public LyraALSEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "LyraALS" } );
	}
}
