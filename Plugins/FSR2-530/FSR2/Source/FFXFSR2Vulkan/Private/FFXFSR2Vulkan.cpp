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
#define FSR2_VULKAN_INTERNAL 1
#include "FFXFSR2Vulkan.h"
#undef FSR2_VULKAN_INTERNAL

#if FSR2_ENABLE_VK

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#else
#define FFX_GCC 1
#define strcpy_s(a, b) strcpy(a, b)
#define wcscpy_s wcscpy
#endif
THIRD_PARTY_INCLUDES_START

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4190)
#pragma warning (disable : 4191)
#endif

#define VkPhysicalDeviceSubgroupSizeControlProperties VkPhysicalDeviceSubgroupSizeControlPropertiesEXT
#define VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_SIZE_CONTROL_PROPERTIES_EXT

#define VkPipelineShaderStageRequiredSubgroupSizeCreateInfo VkPipelineShaderStageRequiredSubgroupSizeCreateInfoEXT
#define VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO_EXT

#ifndef VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME
typedef struct VkPhysicalDeviceAccelerationStructureFeaturesKHR {
    VkStructureType    sType;
    void*              pNext;
    VkBool32           accelerationStructure;
    VkBool32           accelerationStructureCaptureReplay;
    VkBool32           accelerationStructureIndirectBuild;
    VkBool32           accelerationStructureHostCommands;
    VkBool32           descriptorBindingAccelerationStructureUpdateAfterBind;
} VkPhysicalDeviceAccelerationStructureFeaturesKHR;
#define VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME "VK_KHR_acceleration_structure"
#define VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR ((VkStructureType)1000150013)
#endif

#define fsr2GetPermutationBlobByIndex fsr2GetPermutationBlobByIndexVK

#include "vk/ffx_fsr2_vk.cpp"
#include "vk/shaders/ffx_fsr2_shaders_vk.cpp"

#undef fsr2GetPermutationBlobByIndex

#ifdef _MSC_VER
#pragma warning (pop)
#endif

THIRD_PARTY_INCLUDES_END
#if PLATFORM_WINDOWS
#include "Windows/HideWindowsPlatformTypes.h"
#else
#undef FFX_GCC
#undef strcpy_s
#undef wcscpy_s
#endif

FfxErrorCode ffxFsr2UEGetInterfaceVK(
        FfxFsr2Interface* outInterface,
        void* scratchBuffer,
        size_t scratchBufferSize,
        VkPhysicalDevice physicalDevice)
{
    return ffxFsr2GetInterfaceVK(outInterface, scratchBuffer, scratchBufferSize, physicalDevice, vkGetDeviceProcAddr);
}

#endif
