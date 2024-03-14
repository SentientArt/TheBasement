// Copyright (c), Firelight Technologies Pty, Ltd. 2023-2023.

using UnrealBuildTool;
using System;

public struct FMODAudioLinkEditor
{
    public static void Apply(UnrealBuildTool.Rules.FMODStudio FMODModule, ReadOnlyTargetRules Target)
    {
        if (Target.bBuildEditor)
        {
            FMODModule.AddModule("FMODAudioLinkEditor", false);
        }
    }
}