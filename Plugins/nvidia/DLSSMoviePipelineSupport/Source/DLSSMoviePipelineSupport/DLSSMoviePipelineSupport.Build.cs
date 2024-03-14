// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DLSSMoviePipelineSupport : ModuleRules
{
	public DLSSMoviePipelineSupport(ReadOnlyTargetRules Target) : base(Target)
	{
		ShortName = "DLSSMPS";

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"DLSS",
				"DLSSBlueprint",
				"Engine",
				"MovieRenderPipelineCore",
				"MovieRenderPipelineRenderPasses",
			}
		);
	}
}
