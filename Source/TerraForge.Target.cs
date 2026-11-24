using UnrealBuildTool;
using System.Collections.Generic;

public class TerraForgeTarget : TargetRules
{
	public TerraForgeTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("TerraForge");
	}
}
