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

public class StreamlineD3D12RHI : ModuleRules
{
	public StreamlineD3D12RHI(ReadOnlyTargetRules Target) : base(Target)
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
				"D3D12RHI",
				"Engine",
				"RenderCore",
				"RHI",
				"Streamline",
				"StreamlineRHI",
			}
		);

		AddEngineThirdPartyPrivateStaticDependencies(Target, "DX12");

		if (ReadOnlyBuildVersion.Current.MajorVersion == 5 && ReadOnlyBuildVersion.Current.MinorVersion >= 1)
		{
			PrivateDefinitions.Add("ENGINE_PROVIDES_ID3D12DYNAMICRHI=1");
		}
		else
		{
			PrivateDefinitions.Add("ENGINE_PROVIDES_ID3D12DYNAMICRHI=0");
			PrivateIncludePaths.Add( Path.Combine(EngineDirectory, "Source/Runtime/D3D12RHI/Private") );
		}

		if (ReadOnlyBuildVersion.Current.MajorVersion == 5)
		{
			PrivateDependencyModuleNames.Add("RHICore");
		}
	}
}
