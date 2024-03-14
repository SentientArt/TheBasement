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

public class StreamlineShaders : ModuleRules
{
	public StreamlineShaders(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
			}
			);

		PrivateIncludePaths.AddRange(
			new string[] {
				Path.Combine(EngineDirectory,"Source/Runtime/Renderer/Private"),
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"RenderCore",
				"Renderer",
					
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
					"Engine",
					"RHI",
					"Projects"
			}
			);

		// 4.x and early access 5.0 engines used FVector2D type instead of FVector2f type for shader parameters
		bool bEngineUsesFVector2D = (Target.Version.MajorVersion == 4) || (Target.Version.BranchName == "++UE5+Release-5.0-EarlyAccess");
		PrivateDefinitions.Add(string.Format("DLSS_ENGINE_USES_FVECTOR2D={0}", bEngineUsesFVector2D ? "1" : "0"));
	}
}
