// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

using UnrealBuildTool;
using System;

public class FMODStudioNiagara : ModuleRules
{
    public FMODStudioNiagara(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new[]
            {
                "Core"
            });

        PrivateDependencyModuleNames.AddRange(
            new[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "Projects",
                "FMODStudio",

                // Data interface dependencies
                "Niagara", "NiagaraCore", "VectorVM", "RenderCore", "RHI",
            });

        if (Target.bBuildEditor == true)
        {
            PrivateDependencyModuleNames.Add("UnrealEd");
        }
    }
}
