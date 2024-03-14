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
#include "FSR2VulkanLoader.h"
#include "FSR2VulkanDynamicAPI.h"

#if FSR2_ENABLE_VK

DECLARE_LOG_CATEGORY_EXTERN(LogFSR2Vulkan, Verbose, All);
DEFINE_LOG_CATEGORY(LogFSR2Vulkan);

#include "VulkanRHIPrivate.h"
#include "VulkanContext.h"
#include "VulkanResources.h"

#define FSR2_DEFINE_VK_ENTRYPOINTS(Type,Func) Type FSR2VulkanDynamicAPI::Func = NULL;
FSR2_ENUM_VK_ENTRYPOINTS_ALL(FSR2_DEFINE_VK_ENTRYPOINTS)

#define FSR2_CHECK_VK_ENTRYPOINTS(Type,Func) if (FSR2VulkanDynamicAPI::Func == NULL) { bFoundAllEntryPoints = false; UE_LOG(LogFSR2Vulkan, Warning, TEXT("Failed to find entry point for %s"), TEXT(#Func)); }

#if PLATFORM_WINDOWS
#define VULKAN_LIB_NAME "vulkan-1.dll"
#elif PLATFORM_ANDROID
#define VULKAN_LIB_NAME "libvulkan.so"
#elif PLATFORM_LINUX
#define VULKAN_LIB_NAME "libvulkan.so.1"
#endif

FSR2VulkanLoader::FSR2VulkanLoader()
: VulkanDLLModule(nullptr)
{

}

FSR2VulkanLoader::~FSR2VulkanLoader()
{
	FreeVulkanLibrary();
}

FSR2VulkanLoader& FSR2VulkanLoader::Get()
{
	static FSR2VulkanLoader sSelf;
	return sSelf;
}

bool FSR2VulkanLoader::LoadVulkan(VkInstance inInstance)
{
	if (VulkanDLLModule)
	{
		return true;
	}

	VulkanDLLModule = FPlatformProcess::GetDllHandle(TEXT(VULKAN_LIB_NAME));

	if (VulkanDLLModule)
	{
#define GET_VK_ENTRYPOINTS(Type,Func) FSR2VulkanDynamicAPI::Func = (Type)FPlatformProcess::GetDllExport(VulkanDLLModule, ANSI_TO_TCHAR(#Func));
		FSR2_ENUM_VK_ENTRYPOINTS_BASE(GET_VK_ENTRYPOINTS);

		bool bFoundAllEntryPoints = true;
		FSR2_ENUM_VK_ENTRYPOINTS_BASE(FSR2_CHECK_VK_ENTRYPOINTS);
		if (!bFoundAllEntryPoints)
		{
			FreeVulkanLibrary();
			return false;
		}

#undef GET_VK_ENTRYPOINTS

		if (!FSR2VulkanDynamicAPI::vkGetInstanceProcAddr)
		{
			return false;
		}

	#define FSR2_CHECK_VK_ENTRYPOINTS(Type,Func) if (FSR2VulkanDynamicAPI::Func == NULL) { bFoundAllEntryPoints = false; UE_LOG(LogFSR2Vulkan, Warning, TEXT("Failed to find entry point for %s"), TEXT(#Func)); }

		// Initialize all of the entry points we have to query manually
	#define GETINSTANCE_VK_ENTRYPOINTS(Type, Func) FSR2VulkanDynamicAPI::Func = (Type)FSR2VulkanDynamicAPI::vkGetInstanceProcAddr(inInstance, #Func);
		FSR2_ENUM_VK_ENTRYPOINTS_INSTANCE(GETINSTANCE_VK_ENTRYPOINTS);
		FSR2_ENUM_VK_ENTRYPOINTS_INSTANCE(FSR2_CHECK_VK_ENTRYPOINTS);
		if (!bFoundAllEntryPoints)
		{
			FreeVulkanLibrary();
			return false;
		}
		return true;
	}

	return false;
}

void FSR2VulkanLoader::FreeVulkanLibrary()
{
	if (VulkanDLLModule != nullptr)
	{
		FPlatformProcess::FreeDllHandle(VulkanDLLModule);
		VulkanDLLModule = nullptr;
	}
}

#endif
