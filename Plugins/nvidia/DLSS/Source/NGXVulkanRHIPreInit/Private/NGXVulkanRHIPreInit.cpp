/*
* Copyright (c) 2020 - 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
*
* NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
* property and proprietary rights in and to this material, related
* documentation and any modifications thereto. Any use, reproduction,
* disclosure or distribution of this material and related documentation
* without an express license agreement from NVIDIA CORPORATION or
* its affiliates is strictly prohibited.
*/

#include "NGXVulkanRHIPreInit.h"

#include "IVulkanDynamicRHI.h"

#include "Misc/App.h"
#include "DynamicRHI.h"

#include "nvsdk_ngx_vk.h"

DEFINE_LOG_CATEGORY_STATIC(LogDLSSNGXVulkanRHIPreInit, Log, All);

#define LOCTEXT_NAMESPACE "FNGXVulkanRHIPreInitModule"


void FNGXVulkanRHIPreInitModule::StartupModule()
{
	UE_LOG(LogDLSSNGXVulkanRHIPreInit, Log, TEXT("%s Enter"), ANSI_TO_TCHAR(__FUNCTION__));
	if(FApp::CanEverRender())
	{
		const TCHAR* VulkanRHIModuleName = TEXT("VulkanRHI");

		// don't cleanup since the same module gets loaded shortly thereafter anyways
		const TCHAR* DynamicRHIModuleName = GetSelectedDynamicRHIModuleName(false);
		UE_LOG(LogDLSSNGXVulkanRHIPreInit, Log, TEXT("GetSelectedDynamicRHIModuleName = %s"), DynamicRHIModuleName);

		if(FString(VulkanRHIModuleName) == FString(DynamicRHIModuleName))
		{
			IDynamicRHIModule* VulkanRHIModule = FModuleManager::GetModulePtr<IDynamicRHIModule>(VulkanRHIModuleName);

			if(VulkanRHIModule && VulkanRHIModule->IsSupported())
			{
				unsigned int NumInstanceExtensions = 0 ;
				const char** InstanceExtensions = nullptr;
				unsigned int NumDeviceExtensions = 0;
				const char** DeviceExtensions = nullptr;

				// TODO: NVSDK_NGX_VULKAN_RequiredExtensions deprecated, change to use NVSDK_NGX_VULKAN_GetFeatureInstanceExtensionRequirements instead
				// We'll need information that's currently stored in UDLSSSettings from the DLSS module which isn't loaded yet
				const NVSDK_NGX_Result ResultRequiredExtensions = NVSDK_NGX_VULKAN_RequiredExtensions(&NumInstanceExtensions, &InstanceExtensions, &NumDeviceExtensions, &DeviceExtensions);
				UE_LOG(LogDLSSNGXVulkanRHIPreInit, Log, TEXT("NVSDK_NGX_VULKAN_RequiredExtensions -> (%u %s)"), ResultRequiredExtensions, GetNGXResultAsString(ResultRequiredExtensions));

				const TArrayView<const ANSICHAR* const> RHIBridgeInstanceExtensions(InstanceExtensions, NumInstanceExtensions);
				IVulkanDynamicRHI::AddEnabledInstanceExtensionsAndLayers(RHIBridgeInstanceExtensions, TArrayView<const ANSICHAR* const>());

				const TArrayView<const ANSICHAR* const> RHIBridgeDeviceExtensions(DeviceExtensions, NumDeviceExtensions);
				IVulkanDynamicRHI::AddEnabledDeviceExtensionsAndLayers(RHIBridgeDeviceExtensions, TArrayView<const ANSICHAR* const>());

				UE_LOG(LogDLSSNGXVulkanRHIPreInit, Log, TEXT("Preregistered the required NGX DLSS Vulkan device extensions (%u) and instance extensions (%u) via the VulkanRHIBridge"), NumDeviceExtensions, NumInstanceExtensions);
			}
			else
			{
				UE_LOG(LogDLSSNGXVulkanRHIPreInit, Log, TEXT("Failed to load the VulkanRHI module and/or Vulkan is not supported; skipping of pregistering the required NGX DLSS Vulkan device and instance extensions via the VulkanRHIBridge"));
			}
		}
		else
		{
			UE_LOG(LogDLSSNGXVulkanRHIPreInit, Log, TEXT("VulkanRHI is not the active DynamicRHI; skipping of pregistering the required NGX DLSS Vulkan device and instance extensions via the VulkanRHIBridge"));
		}
	}
	else
	{
		UE_LOG(LogDLSSNGXVulkanRHIPreInit, Log, TEXT("This UE instance does not render; skipping of pregistering the required NGX DLSS Vulkan device and instance extensions via the VulkanRHIBridge"));
	}
	UE_LOG(LogDLSSNGXVulkanRHIPreInit, Log, TEXT("%s Leave"), ANSI_TO_TCHAR(__FUNCTION__));

}

void FNGXVulkanRHIPreInitModule::ShutdownModule()
{
	UE_LOG(LogDLSSNGXVulkanRHIPreInit, Log, TEXT("%s Enter"), ANSI_TO_TCHAR(__FUNCTION__));
	UE_LOG(LogDLSSNGXVulkanRHIPreInit, Log, TEXT("%s Leave"), ANSI_TO_TCHAR(__FUNCTION__));
}

IMPLEMENT_MODULE(FNGXVulkanRHIPreInitModule, NGXVulkanRHIPreInit)

#undef LOCTEXT_NAMESPACE


