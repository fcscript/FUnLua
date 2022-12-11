// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class SProjectServerTarget : TargetRules
{
	public SProjectServerTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		bUsesSteam = true;
		ExtraModuleNames.Add("SProject");
	}
}
