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
using UnrealBuildTool;
using System.IO;

public class StreamlineD3D11RHI : ModuleRules
{
	public StreamlineD3D11RHI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		

		PublicIncludePaths.AddRange(
			new string[] {
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
			}
		);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"StreamlineRHI",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"D3D11RHI",
				"Engine",
				"RenderCore",
				"RHI",
				"Streamline",
				"StreamlineRHI",
			}
		);

		AddEngineThirdPartyPrivateStaticDependencies(Target, "DX11");

		if (ReadOnlyBuildVersion.Current.MajorVersion == 5)
		{
			PrivateDependencyModuleNames.Add("RHICore");
		}
			
		if (ReadOnlyBuildVersion.Current.MajorVersion == 5 && ReadOnlyBuildVersion.Current.MinorVersion >= 1)
		{
			PrivateDefinitions.Add("ENGINE_PROVIDES_ID3D11DYNAMICRHI=1");
		}
		else
		{
			PrivateDefinitions.Add("ENGINE_PROVIDES_ID3D11DYNAMICRHI=0");
			PrivateIncludePaths.AddRange(
				new string[] {
					Path.Combine(EngineDirectory, "Source/Runtime/Windows/D3D11RHI/Private"),
					Path.Combine(EngineDirectory, "Source/Runtime/Windows/D3D11RHI/Private/Windows"),
				}
			);
			// required by D3D11RHI private headers to compile before 5.1
			AddEngineThirdPartyPrivateStaticDependencies(Target, "IntelMetricsDiscovery");
			AddEngineThirdPartyPrivateStaticDependencies(Target, "IntelExtensionsFramework");
			AddEngineThirdPartyPrivateStaticDependencies(Target, "NVAftermath");
			AddEngineThirdPartyPrivateStaticDependencies(Target, "HLMediaLibrary");
		}
	}
}
