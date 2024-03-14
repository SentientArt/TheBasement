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

#include "StreamlineLibraryReflex.h"
#include "StreamlineLibraryPrivate.h"

#if WITH_STREAMLINE
#include "StreamlineReflex.h"
#endif
bool UStreamlineLibraryReflex::IsReflexSupported()
{
#if WITH_STREAMLINE
	return QueryReflexSupport() == UStreamlineFeatureSupport::Supported;
#else
	return false;
#endif
}

UStreamlineFeatureSupport UStreamlineLibraryReflex::QueryReflexSupport()
{
#if WITH_STREAMLINE
	
	bool bIsMaxTickRateHandlerEnabled = false;
	
	TArray<IMaxTickRateHandlerModule*> MaxTickRateHandlerModules = IModularFeatures::Get()
		.GetModularFeatureImplementations<IMaxTickRateHandlerModule>(IMaxTickRateHandlerModule::GetModularFeatureName());
	for (IMaxTickRateHandlerModule* MaxTickRateHandler : MaxTickRateHandlerModules)
	{
		bIsMaxTickRateHandlerEnabled = bIsMaxTickRateHandlerEnabled || MaxTickRateHandler->GetAvailable();
	}
	
	bool bIsLatencyMarkerModuleEnabled = false;
	
	TArray<ILatencyMarkerModule*> LatencyMarkerModules = IModularFeatures::Get()
		.GetModularFeatureImplementations<ILatencyMarkerModule>(ILatencyMarkerModule::GetModularFeatureName());
	for (ILatencyMarkerModule* LatencyMarkerModule : LatencyMarkerModules)
	{
		bIsLatencyMarkerModuleEnabled = bIsLatencyMarkerModuleEnabled || LatencyMarkerModule->GetAvailable();
	}
	
	return bIsMaxTickRateHandlerEnabled && bIsLatencyMarkerModuleEnabled ? UStreamlineFeatureSupport::Supported : UStreamlineFeatureSupport::NotSupported;
#endif
	return UStreamlineFeatureSupport::NotSupportedByPlatformAtBuildTime;
}


void UStreamlineLibraryReflex::SetReflexMode(const UStreamlineReflexMode Mode)
{
#if WITH_STREAMLINE
	TArray<IMaxTickRateHandlerModule*> MaxTickRateHandlerModules = IModularFeatures::Get()
		.GetModularFeatureImplementations<IMaxTickRateHandlerModule>(IMaxTickRateHandlerModule::GetModularFeatureName());
	for (IMaxTickRateHandlerModule* MaxTickRateHandler : MaxTickRateHandlerModules)
	{
		MaxTickRateHandler->SetEnabled(Mode != UStreamlineReflexMode::Disabled);
		MaxTickRateHandler->SetFlags(static_cast<uint32>(Mode));
	}
#endif
}

UStreamlineReflexMode UStreamlineLibraryReflex::GetReflexMode()
{
#if WITH_STREAMLINE
	TArray<IMaxTickRateHandlerModule*> MaxTickRateHandlerModules = IModularFeatures::Get()
		.GetModularFeatureImplementations<IMaxTickRateHandlerModule>(IMaxTickRateHandlerModule::GetModularFeatureName());
	for (IMaxTickRateHandlerModule* MaxTickRateHandler : MaxTickRateHandlerModules)
	{
		if (MaxTickRateHandler->GetEnabled())
		{
			return static_cast<UStreamlineReflexMode>(MaxTickRateHandler->GetFlags());
		}
	}
#endif
	return UStreamlineReflexMode::Disabled;
}

UStreamlineReflexMode UStreamlineLibraryReflex::GetDefaultReflexMode()
{
#if WITH_STREAMLINE
	return UStreamlineReflexMode::Enabled;
#else
	return UStreamlineReflexMode::Disabled;
#endif
}

float UStreamlineLibraryReflex::GetGameToRenderLatencyInMs()
{
#if WITH_STREAMLINE
	TArray<ILatencyMarkerModule*> LatencyMarkerModules = IModularFeatures::Get()
		.GetModularFeatureImplementations<ILatencyMarkerModule>(ILatencyMarkerModule::GetModularFeatureName());
	for (ILatencyMarkerModule* LatencyMarkerModule : LatencyMarkerModules)
	{
		return LatencyMarkerModule->GetTotalLatencyInMs();
	}
#endif
	return 0.f;
}

float UStreamlineLibraryReflex::GetGameLatencyInMs()
{
#if WITH_STREAMLINE
	TArray<ILatencyMarkerModule*> LatencyMarkerModules = IModularFeatures::Get()
		.GetModularFeatureImplementations<ILatencyMarkerModule>(ILatencyMarkerModule::GetModularFeatureName());
	for (ILatencyMarkerModule* LatencyMarkerModule : LatencyMarkerModules)
	{
		return LatencyMarkerModule->GetGameLatencyInMs();
	}
#endif
	return 0.f;
}

float UStreamlineLibraryReflex::GetRenderLatencyInMs()
{
#if WITH_STREAMLINE
	TArray<ILatencyMarkerModule*> LatencyMarkerModules = IModularFeatures::Get()
		.GetModularFeatureImplementations<ILatencyMarkerModule>(ILatencyMarkerModule::GetModularFeatureName());
	for (ILatencyMarkerModule* LatencyMarkerModule : LatencyMarkerModules)
	{
		return LatencyMarkerModule->GetRenderLatencyInMs();
	}
#endif
	return 0.f;
}



void UStreamlineLibraryReflex::Startup()
{

}
void UStreamlineLibraryReflex::Shutdown()
{

}
