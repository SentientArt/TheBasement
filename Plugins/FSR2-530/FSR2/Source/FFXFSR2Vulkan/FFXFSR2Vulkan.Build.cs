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
using System;
using System.IO;

public class FFXFSR2Vulkan : ModuleRules
{
	public FFXFSR2Vulkan(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
				Path.Combine(ModuleDirectory, "../ffx-fsr2-api/vk"),
			}
			);


		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
				Path.Combine(ModuleDirectory, "../ffx-fsr2-api/vk"),
			}
			);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Engine",
				"FFXFSR2Api"
				// ... add other public dependencies that you statically link with here ...
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Engine",
				"RHI",
				"VulkanRHI"
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

		bool bEnableVulkan = false;
		if ( Target.IsInPlatformGroup(UnrealPlatformGroup.Windows)
			|| Target.IsInPlatformGroup(UnrealPlatformGroup.Android))
		{
			AddEngineThirdPartyPrivateStaticDependencies(Target, "Vulkan");
			bEnableVulkan = true;
		}
		else if (Target.IsInPlatformGroup(UnrealPlatformGroup.Unix))
		{
			if (Target.IsInPlatformGroup(UnrealPlatformGroup.Linux))
			{
				PrivateDependencyModuleNames.Add("ApplicationCore");
				AddEngineThirdPartyPrivateStaticDependencies(Target, "SDL2");

				string VulkanSDKPath = Environment.GetEnvironmentVariable("VULKAN_SDK");
				bool bSDKInstalled = !String.IsNullOrEmpty(VulkanSDKPath);
				if (BuildHostPlatform.Current.Platform != UnrealTargetPlatform.Linux || !bSDKInstalled)
				{
					AddEngineThirdPartyPrivateStaticDependencies(Target, "Vulkan");
				}
				else
				{
					PrivateIncludePaths.Add(VulkanSDKPath + "/include");
					PrivateIncludePaths.Add(VulkanSDKPath + "/include/vulkan");
					PublicAdditionalLibraries.Add(Path.Combine(VulkanSDKPath, "lib", "libvulkan.so"));
				}
			}
			else
			{
				AddEngineThirdPartyPrivateStaticDependencies(Target, "VkHeadersExternal");
			}
			bEnableVulkan = true;
		}

		if (bEnableVulkan)
		{
			PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"VulkanRHI",
			}
			);

			// Required for some private headers needed for the rendering support.
			var EngineDir = Path.GetFullPath(Target.RelativeEnginePath);
			PrivateIncludePaths.AddRange(
				new string[] {
					Path.Combine(EngineDir, @"Source\Runtime\VulkanRHI\Private")
			});

			PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
				Path.Combine(ModuleDirectory, "../ffx-fsr2-api/shaders/vk"),
			}
			);

			if (Target.IsInPlatformGroup(UnrealPlatformGroup.Windows))
			{
				PrivateIncludePaths.Add(Path.Combine(EngineDir, @"Source/Runtime/VulkanRHI/Private/Windows"));
			}
			else if (Target.IsInPlatformGroup(UnrealPlatformGroup.Unix))
			{
				if (Target.IsInPlatformGroup(UnrealPlatformGroup.Linux))
				{
					PrivateIncludePaths.Add(Path.Combine(EngineDir, @"Source/Runtime/VulkanRHI/Private/Linux"));
				}
			}
			else
			{
				PrivateIncludePaths.Add(Path.Combine(EngineDir, @"Source/Runtime/VulkanRHI/Private/" + Target.Platform));
			}
		}

		PrecompileForTargets = PrecompileTargetsType.Any;
	}
}
