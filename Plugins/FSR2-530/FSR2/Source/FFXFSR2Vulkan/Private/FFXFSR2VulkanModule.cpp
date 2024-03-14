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
#include "FFXFSR2VulkanModule.h"

#include "VulkanPlatform.h"

#include "FFXFSR2Vulkan.h"
#include "CoreMinimal.h"
#include "IVulkanDynamicRHI.h"

#include "VulkanRHIPrivate.h"
#include "VulkanContext.h"
#include "VulkanResources.h"

IMPLEMENT_MODULE(FFFXFSR2Vulkan, FFXFSR2Vulkan)

void FFFXFSR2Vulkan::StartupModule()
{
#if FSR2_ENABLE_VK
	{
		const TArray<const ANSICHAR*> ExtentionsToAdd{ VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME, VK_GOOGLE_HLSL_FUNCTIONALITY1_EXTENSION_NAME, VK_GOOGLE_USER_TYPE_EXTENSION_NAME, VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME, VK_AMD_GPU_SHADER_HALF_FLOAT_EXTENSION_NAME };
		((IVulkanDynamicRHI*)GDynamicRHI)->AddEnabledDeviceExtensionsAndLayers(ExtentionsToAdd, TArray<const ANSICHAR*>());
	}
#endif
}

void FFFXFSR2Vulkan::ShutdownModule()
{
}

bool FFFXFSR2Vulkan::IsNativeBackendSupported()
{
	bool bHasRenderPass2 = false;
	bool bHasHLSL = false;
	bool bHasUserType = false;
#if FSR2_ENABLE_VK
	FVulkanDynamicRHI* VulkanRHI = (FVulkanDynamicRHI*)GDynamicRHI;
	TArray<VkExtensionProperties> Extensions;
	{
		uint32 Count = 0;
		VulkanRHI::vkEnumerateDeviceExtensionProperties(VulkanRHI->GetDevice()->GetPhysicalHandle(), nullptr, &Count, nullptr);
		if (Count > 0)
		{
			Extensions.Empty(Count);
			Extensions.AddUninitialized(Count);
			VulkanRHI::vkEnumerateDeviceExtensionProperties(VulkanRHI->GetDevice()->GetPhysicalHandle(), nullptr, &Count, Extensions.GetData());
		}
		for (VkExtensionProperties& Ext : Extensions)
		{
			if (strcmp(Ext.extensionName, VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME) == 0)
			{
				bHasRenderPass2 = true;
			}
			else if (strcmp(Ext.extensionName, VK_GOOGLE_HLSL_FUNCTIONALITY1_EXTENSION_NAME) == 0)
			{
				bHasHLSL = true;
			}
			else if (strcmp(Ext.extensionName, VK_GOOGLE_USER_TYPE_EXTENSION_NAME) == 0)
			{
				bHasUserType = true;
			}
		}
	}
#endif

	return (bHasRenderPass2 && bHasHLSL && bHasUserType);
}
