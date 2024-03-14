// This file is part of the FidelityFX Super Resolution 2.2 Unreal Engine Plugin.
//
// Copyright (c) 2022-2023 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
using UnrealBuildTool;
using System.IO;

public class FSR2TemporalUpscalingD3D12 : ModuleRules
{
	public FSR2TemporalUpscalingD3D12(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);


		PrivateIncludePaths.AddRange(
			new string[] {
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
				"Engine",
				"Projects",
				"RenderCore",
				"Renderer",
				"RHI",
				// ... add private dependencies that you statically link with here ...	
			}
			);

		BuildVersion Version;
		if (BuildVersion.TryRead(BuildVersion.GetDefaultFileName(), out Version))
		{
			if (Version.MajorVersion == 5 && Version.MinorVersion >= 0)
			{
				PrivateDependencyModuleNames.AddRange(
					new string[]
					{
						"RHICore",
						// ... add private dependencies that you statically link with here ...	
					}
					);
			}
		}

		if (Target.Platform.IsInGroup(UnrealPlatformGroup.Microsoft))
		{
			PrivateDependencyModuleNames.AddRange(
						new string[] {
					"D3D12RHI"
						});

			AddEngineThirdPartyPrivateStaticDependencies(Target, "DX12");

			// Required for some private headers needed for the rendering support.
			var EngineDir = Path.GetFullPath(Target.RelativeEnginePath);
			PrivateIncludePaths.AddRange(
				new string[] {
					Path.Combine(EngineDir, @"Source\Runtime\D3D12RHI\Private")
			});

			if (Target.Platform.IsInGroup(UnrealPlatformGroup.Windows))
			{
				PrivateIncludePaths.AddRange(
					new string[] {
					Path.Combine(EngineDir, @"Source\Runtime\D3D12RHI\Private\Windows")
				});
			}
			else
			{
				PrivateIncludePaths.AddRange(
					new string[] {
					Path.Combine(EngineDir, @"Source\Runtime\D3D12RHI\Private\" + Target.Platform)
				});
			}

			PrecompileForTargets = PrecompileTargetsType.Any;
		}
		else
		{
			PrecompileForTargets = PrecompileTargetsType.None;
		}
	}
}
