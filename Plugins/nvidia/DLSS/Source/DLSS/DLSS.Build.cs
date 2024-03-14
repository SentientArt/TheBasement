/*
* Copyright (c) 2020 - 2022 NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
* property and proprietary rights in and to this material, related
* documentation and any modifications thereto. Any use, reproduction,
* disclosure or distribution of this material and related documentation
* without an express license agreement from NVIDIA CORPORATION or
* its affiliates is strictly prohibited.
*/

using UnrealBuildTool;
using System.IO;

public class DLSS : ModuleRules
{
	public virtual string [] SupportedDynamicallyLoadedNGXRHIModules(ReadOnlyTargetRules Target)
	{
		if(Target.Platform == UnrealTargetPlatform.Win64 )
		{
			return new string[]
			{
				"NGXD3D11RHI",
				"NGXD3D12RHI",
				"NGXVulkanRHI"
			};
		}
		return new string[] { "" };
	}

	public DLSS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		// for ITemporalUpscaler in PostProcess/TemporalAA.h
		PrivateIncludePaths.AddRange(
			new string[] {
				Path.Combine(GetModuleDirectory("Renderer"), "Private"),
				// ... add other private include paths required here ...
			}
			);
			
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
					"Core",
					"CoreUObject",
					"EngineSettings",
					"Engine",
					"RenderCore",
					"Renderer",
					"RHI",
					"NGX",
					"Projects",
                    "DeveloperSettings",
					"DLSSUtility",
					"NGXRHI",
				// ... add private dependencies that you statically link with here ...	
			}
			);

		DynamicallyLoadedModuleNames.AddRange(SupportedDynamicallyLoadedNGXRHIModules(Target));
	}
}
