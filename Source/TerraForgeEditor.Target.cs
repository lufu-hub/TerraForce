using UnrealBuildTool;
using System.Collections.Generic;

public class TerraForgeEditorTarget : TargetRules
{
	public TerraForgeEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("TerraForge");
	}
}
