/*
* Copyright (c) 2020 - 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
*
* NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
* property and proprietary rights in and to this material, related
* documentation and any modifications thereto. Any use, reproduction,
* disclosure or distribution of this material and related documentation
* without an express license agreement from NVIDIA CORPORATION or
* its affiliates is strictly prohibited.
*/

using EpicGames.Core;
using UnrealBuildTool;
using System.IO;

public class NGX : ModuleRules
{

	protected virtual bool IsSupportedWindowsPlatform(ReadOnlyTargetRules Target)
	{
		return Target.Platform == UnrealTargetPlatform.Win64;
	}

	public NGX (ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		if (IsSupportedWindowsPlatform(Target))
		{
            string NGXPath = ModuleDirectory + "/";
            
            PublicSystemIncludePaths.Add(NGXPath + "Include/");

            if ((Target.Configuration == UnrealTargetConfiguration.Debug) && Target.bDebugBuildsActuallyUseDebugCRT)
            {
                if (Target.bUseStaticCRT)
                {
                    PublicAdditionalLibraries.Add(NGXPath + "Lib/x64/" + "nvsdk_ngx_s_dbg.lib");
                }
                else
                {
                    PublicAdditionalLibraries.Add(NGXPath + "Lib/x64/" + "nvsdk_ngx_d_dbg.lib");
                }
            }
			else
            { 
				if (Target.bUseStaticCRT)
				{
					PublicAdditionalLibraries.Add(NGXPath + "Lib/x64/" + "nvsdk_ngx_s.lib");
				}
				else
				{
					PublicAdditionalLibraries.Add(NGXPath + "Lib/x64/" + "nvsdk_ngx_d.lib");
				}
            }

            string[] NGXSnippetDLLs = 
            {
                "nvngx_dlss.dll",
				"nvngx_dlssd.dll"
            };

			PublicDefinitions.Add("NGX_DLSS_BINARY_NAME=TEXT(\"" + NGXSnippetDLLs[0] + "\")");

			foreach (string NGXSnippetDLL in NGXSnippetDLLs)
			{
				bool bHasProjectBinary = false;
				if (Target.ProjectFile != null)
				{
					string ProjectDLLPath = DirectoryReference.Combine(Target.ProjectFile.Directory, "Binaries/ThirdParty/NVIDIA/NGX/Win64", NGXSnippetDLL).FullName;
					if (File.Exists(ProjectDLLPath))
					{
						bHasProjectBinary = true;
						//Log.TraceInformation("NGX project specific production DLSS binary found at {0}.", ProjectDLLPath);
						RuntimeDependencies.Add(ProjectDLLPath, StagedFileType.NonUFS);
					}
				}

				string SnippetBasePath = Path.Combine(PluginDirectory, "Binaries/ThirdParty/Win64");

				// useful to have both plugin and project specific binary during testing, but if we have a project specific binary, then we want to ship with only that
				if (!bHasProjectBinary || Target.Configuration != UnrealTargetConfiguration.Shipping)
				{
					bool bProdSnippetExists = File.Exists(Path.Combine(SnippetBasePath, NGXSnippetDLL));
					if (bProdSnippetExists)
					{
						RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/Win64/" + NGXSnippetDLL, StagedFileType.NonUFS);
					}
				}

				// useful to have debug overlay during testing, but we don't want to ship with that
				if (Target.Configuration != UnrealTargetConfiguration.Shipping)
				{
					bool bDevSnippetExists = File.Exists(Path.Combine(SnippetBasePath, "Development", NGXSnippetDLL));
					if (bDevSnippetExists)
					{
						RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/Win64/Development/" + NGXSnippetDLL, StagedFileType.DebugNonUFS);
					}
				}
			}
		}
	}
}

