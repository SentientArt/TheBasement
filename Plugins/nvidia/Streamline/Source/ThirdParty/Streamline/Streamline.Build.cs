/*
* Copyright (c) 2022 - 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
*
* NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
* property and proprietary rights in and to this material, related
* documentation and any modifications thereto. Any use, reproduction,
* disclosure or distribution of this material and related documentation
* without an express license agreement from NVIDIA CORPORATION or
* its affiliates is strictly prohibited.
*/
#if UE_5_0_OR_LATER
using EpicGames.Core;
#else
using Tools.DotNETCommon;
#endif

using System.Collections.Generic;
using UnrealBuildTool;
using System.IO;

public class Streamline : ModuleRules
{

	protected virtual bool IsSupportedWindowsPlatform(ReadOnlyTargetRules Target)
	{
		return Target.Platform == UnrealTargetPlatform.Win64;
	}

	public Streamline (ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		if (IsSupportedWindowsPlatform(Target))
		{
			string StreamlinePath = ModuleDirectory + "/";
			PublicIncludePaths.Add(StreamlinePath + "include/");

			string SLProductionBinariesPath = PluginDirectory + @"\Binaries\ThirdParty\Win64\";
			string SLDevelopmentBinariesPath = SLProductionBinariesPath + @"Development\";
			string SLDebugBinariesPath = SLProductionBinariesPath + @"Debug\";

			// those are loaded at runtime by the SL & NGX plugin loader which accepts our path
			List<string> StreamlineDlls = new List<string>
			{
				 "sl.interposer.dll",
				 "sl.common.dll",
				 "sl.reflex.dll",

				 "nvngx_dlssg.dll",
				 "sl.dlss_g.dll",
			 };

			List<string> StreamlinePdbs = new List<string>(StreamlineDlls);
			StreamlinePdbs.ForEach(DLLFile =>  Path.ChangeExtension(DLLFile, ".pdb"));

			PublicDefinitions.Add("STREAMLINE_INTERPOSER_BINARY_NAME=TEXT(\"" + StreamlineDlls[0] + "\")");

			bool bHasProductionBinaries = Directory.Exists(SLProductionBinariesPath);
			bool bHasDevelopmentBinaries = Directory.Exists(SLDevelopmentBinariesPath);
			bool bHasDebugBinaries = Directory.Exists(SLDebugBinariesPath);

			foreach (string StreamlineDll in StreamlineDlls)
			{
				RuntimeDependencies.Add(SLProductionBinariesPath + StreamlineDll, StagedFileType.NonUFS);

				if (Target.Configuration != UnrealTargetConfiguration.Shipping)
				{
					if (bHasDevelopmentBinaries)
					{
						RuntimeDependencies.Add(SLDevelopmentBinariesPath + StreamlineDll, StagedFileType.NonUFS);
					}

					if (bHasDebugBinaries)
					{
						RuntimeDependencies.Add(SLDebugBinariesPath + StreamlineDll, StagedFileType.NonUFS);
					}
				}
			}

			if (Target.Configuration != UnrealTargetConfiguration.Shipping)
			{
				// include symbols in non-shipping builds
				foreach (string StreamlinePdb in StreamlinePdbs)
				{
					RuntimeDependencies.Add(SLProductionBinariesPath + StreamlinePdb, StagedFileType.DebugNonUFS);

					if (bHasDevelopmentBinaries)
					{
						RuntimeDependencies.Add(SLDevelopmentBinariesPath + StreamlinePdb, StagedFileType.DebugNonUFS);
					}

					if (bHasDebugBinaries)
					{
						RuntimeDependencies.Add(SLDebugBinariesPath + StreamlinePdb, StagedFileType.DebugNonUFS);
					}
				}

				// useful to have debug overlay during testing, but we don't want to ship with that
				List<string> StreamlineOverlayBinaries = new List<string>
				{
					"sl.imgui.dll",
					"sl.imgui.pdb",
				};

				foreach (string StreamlineOverlayBinary in StreamlineOverlayBinaries)
				{
					StagedFileType FileType = StreamlineOverlayBinary.EndsWith("pdb") ? StagedFileType.DebugNonUFS : StagedFileType.NonUFS;
					if (bHasDevelopmentBinaries)
					{
						RuntimeDependencies.Add(SLDevelopmentBinariesPath + StreamlineOverlayBinary, FileType);
					}
					if (bHasDebugBinaries)
					{
						RuntimeDependencies.Add(SLDebugBinariesPath + StreamlineOverlayBinary, FileType);
					}
				}
			}
		}
	}
}

