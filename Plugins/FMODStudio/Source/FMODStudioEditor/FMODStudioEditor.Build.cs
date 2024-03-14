// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.
using System.IO;

namespace UnrealBuildTool.Rules
{
    public class FMODStudioEditor : ModuleRules
    {
    #if WITH_FORWARDED_MODULE_RULES_CTOR
        public FMODStudioEditor(ReadOnlyTargetRules Target) : base(Target)
    #else
        public FMODStudioEditor(TargetInfo Target)
    #endif
        {
            PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
            PrivatePCHHeaderFile = "Private/FMODStudioEditorPrivatePCH.h";

            bUseUnity = false;

            PrivateIncludePaths.AddRange(
                new string[] {
                    "FMODStudio/Private",
                    "FMODStudio/Public/FMOD",
                    Path.Combine(ModuleDirectory, "Classes")
                }
                );

            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "DeveloperToolSettings",
                    "Engine",
                    "FMODStudio",
                    "InputCore",
                    "UnrealEd",
                    "Sequencer"
                }
                );

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "AssetRegistry",
                    "AssetTools",
                    "EditorStyle",
                    "LevelEditor",
                    "LevelSequence",
                    "MainFrame",
                    "MovieScene",
                    "MovieSceneTracks",
                    "MovieSceneTools",
                    "PropertyEditor",
                    "Settings",
                    "Slate",
                    "SlateCore",
                    "Sockets",
                    "SourceControl",
                    "ToolMenus",
                    "WorkspaceMenuStructure",
                }
                );
        }
    }
}