// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.
using UnrealBuildTool;
using System;
using System.IO;
using System.Collections.Generic;

namespace UnrealBuildTool.Rules
{
    public class FMODStudio : ModuleRules
    {
        string LibRootDirectory                             { get { return Path.Combine(ModuleDirectory, @"..\..\Binaries\"); } }
        protected virtual string FMODLibDir                 { get { return null; } }

        protected virtual string ConsoleLinkExt             { get { return null; } }
        protected virtual string ConsoleDllExt              { get { return null; } }
        protected virtual string ConsoleLibPrefix           { get { return null; } }

        protected virtual bool ConsoleRuntimeDependencies   { get { return true; } }
        protected virtual bool ConsoleDelayLoad             { get { return false; } }
        protected virtual bool LinkDebugFiles               { get { return false; } }
        protected virtual bool CopyLibs                     { get { return false; } }

        public FMODStudio(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
            PrivatePCHHeaderFile = "Private/FMODStudioPrivatePCH.h";

            bUseUnity = false;

            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public/FMOD"));
            PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Classes"));

            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "Media",
                    "Projects"
                }
                );

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "MovieScene",
                    "MovieSceneTracks"
                }
                );

            if (Target.bBuildEditor == true)
            {
                PublicDependencyModuleNames.Add("DeveloperToolSettings");
                PrivateDependencyModuleNames.Add("AssetRegistry");
                PrivateDependencyModuleNames.Add("Settings");
                PrivateDependencyModuleNames.Add("UnrealEd");
            }

            DynamicallyLoadedModuleNames.AddRange(
                new string[]
                {
                }
                );

            string configName = "";

            if (Target.Configuration != UnrealTargetConfiguration.Shipping)
            {
                configName = "L";
                PublicDefinitions.Add("FMODSTUDIO_LINK_LOGGING=1");
            }
            else
            {
                configName = "";
                PublicDefinitions.Add("FMODSTUDIO_LINK_RELEASE=1");
            }

            string linkExtension = "";
            string dllExtension = "";
            string libPrefix = "";
            string libPath = FMODLibDir;

            bool bAddRuntimeDependencies = true;
            bool bAddDelayLoad = false;
            bool bLinkDebugFiles = false;

            if (libPath == null)
            {
                string platformName = Target.Platform.ToString();

                libPath = System.IO.Path.Combine(LibRootDirectory, platformName);

                if (Target.IsInPlatformGroup(UnrealPlatformGroup.Windows))
                {
                    linkExtension = "_vc.lib";
                    dllExtension = ".dll";
                    bAddDelayLoad = true;
                }
                else if (Target.Platform == UnrealTargetPlatform.Mac)
                {
                    linkExtension = dllExtension = ".dylib";
                    libPrefix = "lib";

                    libPath = System.IO.Path.Combine(ModuleDirectory, "../../Libs/Mac/");
                }
                else if (Target.Platform == UnrealTargetPlatform.Android)
                {
                    bAddRuntimeDependencies = false; // Don't use this system
                    linkExtension = dllExtension = ".so";
                    libPrefix = "lib";
                }
                else if (Target.Platform == UnrealTargetPlatform.IOS)
                {
                    linkExtension = "_iphoneos.a";
                    libPrefix = "lib";
                    bAddRuntimeDependencies = false;
                }
                else if (Target.Platform == UnrealTargetPlatform.TVOS)
                {
                    linkExtension = "_appletvos.a";
                    libPrefix = "lib";
                    bAddRuntimeDependencies = false;
                }
                else if (Target.Platform == UnrealTargetPlatform.Linux)
                {
                    libPath = System.IO.Path.Combine(libPath, "x86_64");
                    linkExtension = ".so";
                    dllExtension = ".so";
                    libPrefix = "lib";
                }
            }
            else
            {
                linkExtension = ConsoleLinkExt;
                dllExtension = ConsoleDllExt;
                libPrefix = ConsoleLibPrefix;

                bAddRuntimeDependencies = ConsoleRuntimeDependencies;
                bAddDelayLoad = ConsoleDelayLoad;
                bLinkDebugFiles = LinkDebugFiles;
            }

            // Collapse the directory path, otherwise MacOS has issues with plugin paths.
            libPath = Utils.CleanDirectorySeparators(libPath);

            string fmodLibName = System.String.Format("{0}fmod{1}{2}", libPrefix, configName, linkExtension);
            string fmodStudioLibName = System.String.Format("{0}fmodstudio{1}{2}", libPrefix, configName, linkExtension);

            string fmodDllName = System.String.Format("{0}fmod{1}{2}", libPrefix, configName, dllExtension);
            string fmodStudioDllName = System.String.Format("{0}fmodstudio{1}{2}", libPrefix, configName, dllExtension);

            string fmodLibPath = System.IO.Path.Combine(libPath, fmodLibName);
            string fmodStudioLibPath = System.IO.Path.Combine(libPath, fmodStudioLibName);

            string fmodDllPath = System.IO.Path.Combine(libPath, fmodDllName);
            string fmodStudioDllPath = System.IO.Path.Combine(libPath, fmodStudioDllName);

            System.Collections.Generic.List<string> plugins = GetPlugins(libPath);

            if (Target.IsInPlatformGroup(UnrealPlatformGroup.Android))
            {
                string[] archs = new string[] { "arm64-v8a", "x86_64" };
                foreach (string arch in archs)
                {
                    string LibPath = System.IO.Path.Combine(libPath, arch);
                    PublicAdditionalLibraries.Add(System.String.Format("{0}/{1}", LibPath, fmodLibName));
                    PublicAdditionalLibraries.Add(System.String.Format("{0}/{1}", LibPath, fmodStudioLibName));
                }
            }
            else
            {
                PublicAdditionalLibraries.Add(fmodLibPath);
                PublicAdditionalLibraries.Add(fmodStudioLibPath);
            }

            if (CopyLibs)
            {
                RuntimeDependencies.Add("$(TargetOutputDir)/" + fmodDllName, fmodDllPath);
                RuntimeDependencies.Add("$(TargetOutputDir)/" + fmodStudioDllName, fmodStudioDllPath);
                foreach (string plugin in plugins)
                {
                    string pluginPath = System.IO.Path.Combine(libPath, plugin + dllExtension);
                    System.Console.WriteLine("Adding reference to FMOD plugin: " + pluginPath);
                    RuntimeDependencies.Add("$(TargetOutputDir)/" + plugin + dllExtension, pluginPath);
                }
            }
            else if (bAddRuntimeDependencies)
            {
                RuntimeDependencies.Add(fmodDllPath);
                RuntimeDependencies.Add(fmodStudioDllPath);
                foreach (string plugin in plugins)
                {
                    string pluginPath = System.IO.Path.Combine(libPath, plugin + dllExtension);
                    System.Console.WriteLine("Adding reference to FMOD plugin: " + pluginPath);
                    RuntimeDependencies.Add(pluginPath);
                }
            }

            if (bLinkDebugFiles)
            {
                RuntimeDependencies.Add(fmodDllPath + ".debug", StagedFileType.DebugNonUFS);
                RuntimeDependencies.Add(fmodStudioDllPath + ".debug", StagedFileType.DebugNonUFS);
            }

            if (bAddDelayLoad)
            {
                PublicDelayLoadDLLs.AddRange(
                        new string[] {
                            fmodDllName,
                            fmodStudioDllName
                            }
                        );
            }

            if (Target.Platform == UnrealTargetPlatform.Android)
            {
                string APLName = System.String.Format("FMODStudio{0}_APL.xml", configName);
                string RelAPLPath = Utils.MakePathRelativeTo(System.IO.Path.Combine(ModuleDirectory, APLName), Target.RelativeEnginePath);
                System.Console.WriteLine("Adding {0}", RelAPLPath);
                AdditionalPropertiesForReceipt.Add("AndroidPlugin", RelAPLPath);
                foreach (string PluginName in System.IO.Directory.GetFiles(libPath))
                {
                    if (PluginName.EndsWith("_APL.xml", System.StringComparison.OrdinalIgnoreCase))
                    {
                        string RelPluginPath = Utils.MakePathRelativeTo(PluginName, Target.RelativeEnginePath);
                        System.Console.WriteLine("Adding {0}", RelPluginPath);
                        AdditionalPropertiesForReceipt.Add("AndroidPlugin", RelPluginPath);
                    }
                }
            }

            FMODAudioLink.Apply(this, Target);
            FMODAudioLinkEditor.Apply(this, Target);
        }

        private System.Collections.Generic.List<string> GetPlugins(string BasePath)
        {
            System.Collections.Generic.List<string> AllPlugins = new System.Collections.Generic.List<string>();
            string PluginListName = System.IO.Path.Combine(BasePath, "plugins.txt");
            if (System.IO.File.Exists(PluginListName))
            {
                try
                {
                    foreach (string FullEntry in System.IO.File.ReadAllLines(PluginListName))
                    {
                        string Entry = FullEntry.Trim();
                        if (Entry.Length > 0)
                        {
                            AllPlugins.Add(Entry);
                        }
                    }
                }
                catch (System.Exception ex)
                {
                    System.Console.WriteLine("Failed to read plugin list file: {0}", ex.Message);
                }
            }
            return AllPlugins;
        }

        public void AddModule(string Module, bool AddPublic = true)
        {
            ConditionalAddModuleDirectory(
                EpicGames.Core.DirectoryReference.Combine(new EpicGames.Core.DirectoryReference(ModuleDirectory), "..", Module));

            ExternalDependencies.Add(Path.Combine(ModuleDirectory, "..", Module, Module + ".Build.cs"));
            if (AddPublic)
            {
                PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "..", Module, "Public"));
            }
            PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "..", Module, "Private"));
        }
    }
}