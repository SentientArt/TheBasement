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

#include "StreamlineAPI.h"
#include "StreamlineRHI.h"
#include "StreamlineRHIPrivate.h"

#include "HAL/IConsoleManager.h"
#include "HAL/PlatformProcess.h"
#include "HAL/ThreadManager.h"
#include "Runtime/Launch/Resources/Version.h"

#include "sl.h"
#include "sl_helpers.h"


#define LOCTEXT_NAMESPACE "FStreamlineRHIModule"

#ifndef LOG_SL_FUNCTIONS
#define LOG_SL_FUNCTIONS (!(UE_BUILD_TEST || UE_BUILD_SHIPPING))
#endif

static bool bLogStreamlineLogFunctions = 0;
static FAutoConsoleVariableRef CVarStreamlineLogFunctions(
	TEXT("r.Streamline.LogFunctions"),
	bLogStreamlineLogFunctions,
	TEXT("Enable/disable whether streamline functions calls and thread are written to the log. Is also set to true with -slloglevel=3 \n"),
	ECVF_Default);

static bool LogStreamlineFunctions()
{
#if LOG_SL_FUNCTIONS
	return bLogStreamlineLogFunctions;
#else
	return false;
#endif
}

void SetStreamlineAPILoggingEnabled(bool bEnabled)
{
	bLogStreamlineLogFunctions = bEnabled;
}

namespace sl
{
	inline const char* getFeatureRequirementsSingleBitFlagsAsStr(FeatureRequirementFlags f)
	{
		switch (f)
		{
			SL_CASE_STR(FeatureRequirementFlags::eD3D11Supported);
			SL_CASE_STR(FeatureRequirementFlags::eD3D12Supported);
			SL_CASE_STR(FeatureRequirementFlags::eVulkanSupported);
			SL_CASE_STR(FeatureRequirementFlags::eVSyncOffRequired);
			SL_CASE_STR(FeatureRequirementFlags::eHardwareSchedulingRequired);
		}
		return "Unknown";
	}

	inline FString getFeatureRequirementsFlagsAsStr(FeatureRequirementFlags f)
	{
		FString Result;
		
		for (uint32 SingleBit = uint32(sl::FeatureRequirementFlags::eHardwareSchedulingRequired); SingleBit != 0; SingleBit >>= 1)
		{
			sl::FeatureRequirementFlags Flag = sl::FeatureRequirementFlags(SingleBit);

			if (Flag == SLBitwiseAnd(f, Flag))
			{
				Result.Append(getFeatureRequirementsSingleBitFlagsAsStr(Flag));
				if (SingleBit != 1)
				{
					Result.AppendChar(TCHAR('|'));
				}
			}
		}


		return Result;
	}
}

STREAMLINERHI_API void LogStreamlineFeatureSupport(sl::Feature Feature, const sl::AdapterInfo& Adapter)
{
	sl::Result SupportedResult = SLisFeatureSupported(Feature,Adapter);
	UE_LOG(LogStreamlineRHI, Log, TEXT("SLisFeatureSupported(%s) -> (%d, %s)"), ANSI_TO_TCHAR(sl::getFeatureAsStr(Feature)), SupportedResult, ANSI_TO_TCHAR(sl::getResultAsStr(SupportedResult)));

	// putting this here since the alternative of having FStreamlineRHI compute & store that was annoying since it would mean to have sl.h be a public include

	sl::FeatureVersion Version;
	sl::Result VersionResult = SLgetFeatureVersion(Feature, Version);
	UE_LOG(LogStreamlineRHI, Log, TEXT("SLgetFeatureVersion(%s)  versionSL = %s, versionNGX = %s -> (%d, %s)"),
		ANSI_TO_TCHAR(Version.versionSL.toStr().c_str()), ANSI_TO_TCHAR(sl::getFeatureAsStr(Feature)), ANSI_TO_TCHAR(Version.versionNGX.toStr().c_str()), VersionResult, ANSI_TO_TCHAR(sl::getResultAsStr(VersionResult)));

	sl::FeatureRequirements Requirements;
	sl::Result RequirementsResult = SLgetFeatureRequirements(Feature, Requirements);
	UE_LOG(LogStreamlineRHI, Log, TEXT("SLgetFeatureRequirements(%s) -> (%d, %s)"), ANSI_TO_TCHAR(sl::getFeatureAsStr(Feature)), RequirementsResult, ANSI_TO_TCHAR(sl::getResultAsStr(RequirementsResult)));
	LogStreamlineFeatureRequirements(Feature, Requirements);


}

void LogStreamlineFeatureRequirements(sl::Feature Feature, const sl::FeatureRequirements& Requirements)
{
	UE_LOG(LogStreamlineRHI, Log, TEXT("FeatureRequirements %s: flags %s"), ANSI_TO_TCHAR(sl::getFeatureAsStr(Feature)), *sl::getFeatureRequirementsFlagsAsStr(Requirements.flags));
	UE_LOG(LogStreamlineRHI, Log, TEXT("maxNumCPUThreads : %u"),Requirements.maxNumCPUThreads);
	UE_LOG(LogStreamlineRHI, Log, TEXT("maxNumViewports  : %u"), Requirements.maxNumViewports);
	UE_LOG(LogStreamlineRHI, Log, TEXT("osVersion     detected: %s, required: %s"), ANSI_TO_TCHAR(Requirements.osVersionDetected.toStr().c_str()), ANSI_TO_TCHAR(Requirements.osVersionRequired.toStr().c_str()));
	UE_LOG(LogStreamlineRHI, Log, TEXT("driverVersion detected: %s, required: %s"), ANSI_TO_TCHAR(Requirements.driverVersionDetected.toStr().c_str()), ANSI_TO_TCHAR(Requirements.driverVersionRequired.toStr().c_str()));
	UE_LOG(LogStreamlineRHI, Log, TEXT("requiredTags (%u): {%s}"), 
	Requirements.numRequiredTags
	,*FString::JoinBy(MakeArrayView( Requirements.requiredTags, Requirements.numRequiredTags), TEXT(", "), [](const sl::BufferType& Buffer) { return FString::Printf(TEXT("%s (%u)"), ANSI_TO_TCHAR(sl::getBufferTypeAsStr(Buffer)), Buffer); })
	 );
}





namespace
{
	void* SLInterPoserDLL = nullptr;

	PFun_slInit* Ptr_init = nullptr;
	PFun_slShutdown* Ptr_shutdown = nullptr;
	PFun_slIsFeatureSupported* Ptr_isFeatureSupported = nullptr;
	PFun_slIsFeatureLoaded* Ptr_isFeatureLoaded = nullptr;
	PFun_slSetFeatureLoaded* Ptr_setFeatureLoaded = nullptr;
	PFun_slEvaluateFeature* Ptr_evaluateFeature = nullptr;
	PFun_slAllocateResources*  Ptr_allocateResources = nullptr;
	PFun_slFreeResources* Ptr_freeResources = nullptr;
	PFun_slSetTag* Ptr_setTag = nullptr;
	PFun_slGetFeatureRequirements* Ptr_getFeatureRequirements = nullptr;
	PFun_slGetFeatureVersion* Ptr_getFeatureVersion = nullptr;
	PFun_slUpgradeInterface* Ptr_upgradeInterface = nullptr;
	PFun_slSetConstants*	Ptr_setConstants = nullptr;
	PFun_slGetNativeInterface* Ptr_getNativeInterface = nullptr;
	PFun_slGetFeatureFunction* Ptr_getFeatureFunction = nullptr;
	PFun_slGetNewFrameToken* Ptr_getNewFrameToken = nullptr;
	PFun_slSetD3DDevice* Ptr_setD3DDevice = nullptr;

	bool bIsStreamlineFunctionPointersLoaded = false;
}

static FString CurrentThreadName()
{
#if (ENGINE_MAJOR_VERSION == 4) && (ENGINE_MINOR_VERSION == 25)
	return FThreadManager::Get().GetThreadName(FPlatformTLS::GetCurrentThreadId());
#else
	return FThreadManager::GetThreadName(FPlatformTLS::GetCurrentThreadId());
#endif
}

STREAMLINERHI_API bool AreStreamlineFunctionsLoaded()
{
	return bIsStreamlineFunctionPointersLoaded;
}


sl::Result SLinit(const sl::Preferences& pref, uint64_t sdkVersion)
{
	// we cannot call IsStreamlineSupported since that checks whether bIsStreamlineInitialized is set to true, which it will with the result of this call
	check(AreStreamlineFunctionsLoaded());
	check(SLInterPoserDLL);
	check(Ptr_init != nullptr);

#if LOG_SL_FUNCTIONS
	if(LogStreamlineFunctions())
	{
		UE_LOG(LogStreamlineRHI, Log, TEXT("%s %s sdkVersion=%llx"), ANSI_TO_TCHAR(__FUNCTION__), *CurrentThreadName(), sdkVersion);
	}
#endif

	return Ptr_init(pref, sdkVersion);
}

sl::Result SLshutdown()
{
	check(IsStreamlineSupported());
	check(SLInterPoserDLL);
	check(Ptr_shutdown != nullptr);

#if LOG_SL_FUNCTIONS
	if (LogStreamlineFunctions())
	{
		UE_LOG(LogStreamlineRHI, Log, TEXT("%s %s"), ANSI_TO_TCHAR(__FUNCTION__), *CurrentThreadName());
	}
#endif

	return Ptr_shutdown();
}

sl::Result SLisFeatureSupported(sl::Feature feature, const sl::AdapterInfo& adapterInfo)
{
	check(IsStreamlineSupported());
	check(SLInterPoserDLL);
	check(Ptr_isFeatureSupported != nullptr);

#if LOG_SL_FUNCTIONS
	if (LogStreamlineFunctions())
	{
		UE_LOG(LogStreamlineRHI, Log, TEXT("%s %s feature=%s (%u)"), ANSI_TO_TCHAR(__FUNCTION__), *CurrentThreadName(),
			ANSI_TO_TCHAR(sl::getFeatureAsStr(feature)), feature );
	}
#endif

	return Ptr_isFeatureSupported(feature, adapterInfo);
}

sl::Result SLisFeatureLoaded(sl::Feature feature, bool& loaded)
{
	check(IsStreamlineSupported());
	check(SLInterPoserDLL);
	check(Ptr_isFeatureLoaded != nullptr);

#if LOG_SL_FUNCTIONS
	if (LogStreamlineFunctions())
	{
		UE_LOG(LogStreamlineRHI, Log, TEXT("%s %s feature=%s (%u)"), ANSI_TO_TCHAR(__FUNCTION__), *CurrentThreadName(),
			ANSI_TO_TCHAR(sl::getFeatureAsStr(feature)), feature );
	}
#endif

	return Ptr_isFeatureLoaded(feature, loaded);
}

sl::Result SLsetFeatureLoaded(sl::Feature feature, bool loaded)
{
	check(IsStreamlineSupported());
	check(SLInterPoserDLL);
	check(Ptr_setFeatureLoaded != nullptr);

#if LOG_SL_FUNCTIONS
	if (LogStreamlineFunctions())
	{
		UE_LOG(LogStreamlineRHI, Log, TEXT("%s %s feature=%s (%u), loaded=%u"), ANSI_TO_TCHAR(__FUNCTION__), *CurrentThreadName(),
		ANSI_TO_TCHAR(sl::getFeatureAsStr(feature)), feature, loaded);
	}
#endif
	return Ptr_setFeatureLoaded(feature, loaded);
}

sl::Result SLevaluateFeature(sl::Feature feature, const sl::FrameToken& frame, const sl::BaseStructure** inputs, uint32_t numInputs, sl::CommandBuffer* cmdBuffer)
{
	check(IsStreamlineSupported());
	check(SLInterPoserDLL);
	check(Ptr_evaluateFeature != nullptr);

#if LOG_SL_FUNCTIONS
	if (LogStreamlineFunctions())
	{
		UE_LOG(LogStreamlineRHI, Log, TEXT("%s %s feature=%s (%u) frame=%u, numInputs=%u"), ANSI_TO_TCHAR(__FUNCTION__), *CurrentThreadName(),
			 ANSI_TO_TCHAR(sl::getFeatureAsStr(feature)), feature, static_cast<uint32_t>(frame), numInputs);
	}
#endif

	return Ptr_evaluateFeature(feature, frame, inputs, numInputs, cmdBuffer);
}

sl::Result SLAllocateResources(sl::CommandBuffer* cmdBuffer, sl::Feature feature, const sl::ViewportHandle& viewport)
{
	check(IsStreamlineSupported());
	check(SLInterPoserDLL);
	check(Ptr_allocateResources != nullptr);

#if LOG_SL_FUNCTIONS
	if (LogStreamlineFunctions())
	{
		UE_LOG(LogStreamlineRHI, Log, TEXT("%s %s feature=%s (%u), viewport=%u"), ANSI_TO_TCHAR(__FUNCTION__), *CurrentThreadName(),
			ANSI_TO_TCHAR(sl::getFeatureAsStr(feature)), feature, static_cast<uint32_t>(viewport));
	}
#endif

	return Ptr_allocateResources(cmdBuffer, feature, viewport);
}

sl::Result SLFreeResources(sl::Feature feature, const sl::ViewportHandle& viewport)
{
	check(IsStreamlineSupported());
	check(SLInterPoserDLL);
	check(Ptr_freeResources != nullptr);

#if LOG_SL_FUNCTIONS
	if (LogStreamlineFunctions())
	{
		UE_LOG(LogStreamlineRHI, Log, TEXT("%s %s feature=%s (%u), viewport=%u"), ANSI_TO_TCHAR(__FUNCTION__), *CurrentThreadName(),
			ANSI_TO_TCHAR(sl::getFeatureAsStr(feature)), feature, static_cast<uint32_t>(viewport));
	}
#endif

	return Ptr_freeResources(feature, viewport);
}

sl::Result SLsetTag(const sl::ViewportHandle& viewport, const sl::ResourceTag* tags, uint32_t numTags, sl::CommandBuffer* cmdBuffer)
{
	check(IsStreamlineSupported());
	check(SLInterPoserDLL);
	check(Ptr_setTag != nullptr);

#if LOG_SL_FUNCTIONS
	if (LogStreamlineFunctions())
	{
		if (numTags > 0)
		{
			UE_LOG(LogStreamlineRHI, Log, TEXT("%s %s tag=%s (1/%u), viewport=%u"), ANSI_TO_TCHAR(__FUNCTION__), *CurrentThreadName(),
				ANSI_TO_TCHAR(sl::getBufferTypeAsStr(tags[0].type)), numTags, static_cast<uint32_t>(viewport));
		}
	}
#endif

	return Ptr_setTag(viewport, tags, numTags, cmdBuffer);
}

sl::Result SLgetFeatureRequirements(sl::Feature feature, sl::FeatureRequirements& requirements)
{
	check(IsStreamlineSupported());
	check(SLInterPoserDLL);
	check(Ptr_getFeatureRequirements != nullptr);

#if LOG_SL_FUNCTIONS
	if (LogStreamlineFunctions())
	{
		UE_LOG(LogStreamlineRHI, Log, TEXT("%s %s feature=%s (%u)"), ANSI_TO_TCHAR(__FUNCTION__), *CurrentThreadName(),
			ANSI_TO_TCHAR(sl::getFeatureAsStr(feature)), feature);
	}
#endif

	return Ptr_getFeatureRequirements(feature, requirements);
}

sl::Result SLgetFeatureVersion(sl::Feature feature, sl::FeatureVersion& version)
{
	check(IsStreamlineSupported());
	check(SLInterPoserDLL);
	check(Ptr_getFeatureVersion != nullptr);

#if LOG_SL_FUNCTIONS
	if (LogStreamlineFunctions())
	{
		UE_LOG(LogStreamlineRHI, Log, TEXT("%s %s feature=%s (%u)"), ANSI_TO_TCHAR(__FUNCTION__), *CurrentThreadName(),
			ANSI_TO_TCHAR(sl::getFeatureAsStr(feature)), feature);
	}
#endif

	return Ptr_getFeatureVersion(feature, version);
}

sl::Result SLUpgradeInterface(void** baseInterface)
{
	check(IsStreamlineSupported());
	check(SLInterPoserDLL);
	check(Ptr_upgradeInterface != nullptr);

#if LOG_SL_FUNCTIONS
	if (LogStreamlineFunctions())
	{
		UE_LOG(LogStreamlineRHI, Log, TEXT("%s %s"), ANSI_TO_TCHAR(__FUNCTION__), *CurrentThreadName());
	}
#endif

	return Ptr_upgradeInterface(baseInterface);
}

sl::Result SLsetConstants(const sl::Constants& values, const sl::FrameToken& frame, const sl::ViewportHandle& viewport)
{
	check(IsStreamlineSupported());
	check(SLInterPoserDLL);
	check(Ptr_setConstants != nullptr);

#if LOG_SL_FUNCTIONS
	if (LogStreamlineFunctions())
	{
		UE_LOG(LogStreamlineRHI, Log, TEXT("%s %s frame=%u, viewport=%u"), ANSI_TO_TCHAR(__FUNCTION__), *CurrentThreadName(), static_cast<uint32_t>(frame),
			static_cast<uint32_t>(viewport));
	}
#endif
	return Ptr_setConstants(values, frame, viewport);
}

sl::Result SLgetNativeInterface(void* proxyInterface, void** baseInterface)
{
	check(IsStreamlineSupported());
	check(SLInterPoserDLL);
	check(Ptr_getNativeInterface != nullptr);

#if LOG_SL_FUNCTIONS
	if (LogStreamlineFunctions())
	{
		UE_LOG(LogStreamlineRHI, Log, TEXT("%s %s"), ANSI_TO_TCHAR(__FUNCTION__), *CurrentThreadName());
	}
#endif

	return Ptr_getNativeInterface(proxyInterface, baseInterface);
}

sl::Result SLgetFeatureFunction(sl::Feature feature, const char* functionName, void*& function)
{
	check(IsStreamlineSupported());
	check(SLInterPoserDLL);
	check(Ptr_getFeatureFunction != nullptr);

#if LOG_SL_FUNCTIONS
	if (LogStreamlineFunctions())
	{
		UE_LOG(LogStreamlineRHI, Log, TEXT("%s %s feature=%s (%u), functionName=%s"), ANSI_TO_TCHAR(__FUNCTION__), *CurrentThreadName(),
			ANSI_TO_TCHAR(sl::getFeatureAsStr(feature)), feature, ANSI_TO_TCHAR(functionName));
	}
#endif

	return Ptr_getFeatureFunction(feature, functionName, function);
}

sl::Result SLgetNewFrameToken(sl::FrameToken*& token, uint32_t* frameIndex)
{
	check(IsStreamlineSupported());
	check(SLInterPoserDLL);
	check(Ptr_getNewFrameToken != nullptr);

#if LOG_SL_FUNCTIONS
	if (LogStreamlineFunctions())
	{
		if (frameIndex != nullptr)
		{
			UE_LOG(LogStreamlineRHI, Log, TEXT("%s %s frameIndex=%u"), ANSI_TO_TCHAR(__FUNCTION__), *CurrentThreadName(), *frameIndex);
		}
		else
		{
			UE_LOG(LogStreamlineRHI, Log, TEXT("%s %s frameIndex=nullptr"), ANSI_TO_TCHAR(__FUNCTION__), *CurrentThreadName());
		}
	}
#endif

	return Ptr_getNewFrameToken(token, frameIndex);
}

sl::Result SLsetD3DDevice(void* d3dDevice)
{
	check(IsStreamlineSupported());
	check(SLInterPoserDLL);
	check(Ptr_setD3DDevice != nullptr);

#if LOG_SL_FUNCTIONS
	if (LogStreamlineFunctions())
	{
		UE_LOG(LogStreamlineRHI, Log, TEXT("%s %s"), ANSI_TO_TCHAR(__FUNCTION__), *CurrentThreadName());
	}
#endif

	return Ptr_setD3DDevice(d3dDevice);
}

bool LoadStreamlineFunctionPointers(const FString& InterposerBinaryPath)
{
	if (!bIsStreamlineFunctionPointersLoaded)
	{
		UE_LOG(LogStreamlineRHI, Log, TEXT("loading core Streamline functions from Streamline interposer at %s"), *InterposerBinaryPath);

		const bool bInterposerBinarySigned = slVerifyEmbeddedSignature(InterposerBinaryPath);

#if UE_BUILD_SHIPPING
		if (bInterposerBinarySigned)
#endif
		{
			SLInterPoserDLL = FPlatformProcess::GetDllHandle(*InterposerBinaryPath);
			if (SLInterPoserDLL != nullptr)
			{
				UE_LOG(LogStreamlineRHI, Log, TEXT("SLInterPoserLibrary = %p"), SLInterPoserDLL);
			}
			else
			{
				UE_LOG(LogStreamlineRHI, Log, TEXT("Unable to load SLInterPoserLibrary from %s"), *InterposerBinaryPath);
				return false;
			}

			Ptr_init = (PFun_slInit*)(FWindowsPlatformProcess::GetDllExport(SLInterPoserDLL, TEXT("slInit")));
			UE_LOG(LogStreamlineRHI, Log, TEXT("slInit = %p"), Ptr_init);
			check(Ptr_init);

			Ptr_shutdown = (PFun_slShutdown*)(FWindowsPlatformProcess::GetDllExport(SLInterPoserDLL, TEXT("slShutdown")));
			UE_LOG(LogStreamlineRHI, Log, TEXT("slShutdown = %p"), Ptr_shutdown);
			check(Ptr_shutdown);

			Ptr_isFeatureSupported = (PFun_slIsFeatureSupported*)(FWindowsPlatformProcess::GetDllExport(SLInterPoserDLL, TEXT("slIsFeatureSupported")));
			UE_LOG(LogStreamlineRHI, Log, TEXT("slIsFeatureSupported = %p"), Ptr_isFeatureSupported);
			check(Ptr_isFeatureSupported);

			Ptr_isFeatureLoaded = (PFun_slIsFeatureLoaded*)(FWindowsPlatformProcess::GetDllExport(SLInterPoserDLL, TEXT("slIsFeatureLoaded")));
			UE_LOG(LogStreamlineRHI, Log, TEXT("slIsFeatureLoaded = %p"), Ptr_isFeatureLoaded);
			check(Ptr_isFeatureLoaded);

			Ptr_setFeatureLoaded = (PFun_slSetFeatureLoaded*)(FWindowsPlatformProcess::GetDllExport(SLInterPoserDLL, TEXT("slSetFeatureLoaded")));
			UE_LOG(LogStreamlineRHI, Log, TEXT("slSetFeatureLoaded = %p"), Ptr_setFeatureLoaded);
			check(Ptr_setFeatureLoaded);

			Ptr_evaluateFeature = (PFun_slEvaluateFeature*)(FWindowsPlatformProcess::GetDllExport(SLInterPoserDLL, TEXT("slEvaluateFeature")));
			UE_LOG(LogStreamlineRHI, Log, TEXT("slEvaluateFeature = %p"), Ptr_evaluateFeature);
			check(Ptr_evaluateFeature);

			Ptr_allocateResources = (PFun_slAllocateResources*)(FWindowsPlatformProcess::GetDllExport(SLInterPoserDLL, TEXT("slAllocateResources")));
			UE_LOG(LogStreamlineRHI, Log, TEXT("slAllocateResources = %p"), Ptr_allocateResources);
			check(Ptr_allocateResources);

			Ptr_freeResources = (PFun_slFreeResources*)(FWindowsPlatformProcess::GetDllExport(SLInterPoserDLL, TEXT("slFreeResources")));
			UE_LOG(LogStreamlineRHI, Log, TEXT("slFreeResources = %p"), Ptr_freeResources);
			check(Ptr_freeResources);

			Ptr_setTag = (PFun_slSetTag*)(FWindowsPlatformProcess::GetDllExport(SLInterPoserDLL, TEXT("slSetTag")));
			UE_LOG(LogStreamlineRHI, Log, TEXT("slSetTag = %p"), Ptr_setTag);
			check(Ptr_setTag);

			Ptr_getFeatureRequirements = (PFun_slGetFeatureRequirements*)(FWindowsPlatformProcess::GetDllExport(SLInterPoserDLL, TEXT("slGetFeatureRequirements")));
			UE_LOG(LogStreamlineRHI, Log, TEXT("slGetFeatureRequirements = %p"), Ptr_getFeatureRequirements);
			check(Ptr_getFeatureRequirements);

			Ptr_getFeatureVersion = (PFun_slGetFeatureVersion*)(FWindowsPlatformProcess::GetDllExport(SLInterPoserDLL, TEXT("slGetFeatureVersion")));
			UE_LOG(LogStreamlineRHI, Log, TEXT("slGetFeatureVersion = %p"), Ptr_getFeatureVersion);
			check(Ptr_getFeatureVersion);

			Ptr_upgradeInterface = (PFun_slUpgradeInterface*)(FWindowsPlatformProcess::GetDllExport(SLInterPoserDLL, TEXT("slUpgradeInterface")));
			UE_LOG(LogStreamlineRHI, Log, TEXT("slUpgradeInterface = %p"), Ptr_upgradeInterface);
			check(Ptr_upgradeInterface);

			Ptr_setConstants = (PFun_slSetConstants*)(FWindowsPlatformProcess::GetDllExport(SLInterPoserDLL, TEXT("slSetConstants")));
			UE_LOG(LogStreamlineRHI, Log, TEXT("slSetConstants = %p"), Ptr_setConstants);
			check(Ptr_setConstants);

			Ptr_getNativeInterface = (PFun_slGetNativeInterface*)(FWindowsPlatformProcess::GetDllExport(SLInterPoserDLL, TEXT("slGetNativeInterface")));
			UE_LOG(LogStreamlineRHI, Log, TEXT("slGetNativeInterface = %p"), Ptr_getNativeInterface);
			check(Ptr_getNativeInterface);

			Ptr_getFeatureFunction = (PFun_slGetFeatureFunction*)(FWindowsPlatformProcess::GetDllExport(SLInterPoserDLL, TEXT("slGetFeatureFunction")));
			UE_LOG(LogStreamlineRHI, Log, TEXT("slGetFeatureFunction = %p"), Ptr_getFeatureFunction);
			check(Ptr_getFeatureFunction);

			Ptr_getNewFrameToken = (PFun_slGetNewFrameToken*)(FWindowsPlatformProcess::GetDllExport(SLInterPoserDLL, TEXT("slGetNewFrameToken")));
			UE_LOG(LogStreamlineRHI, Log, TEXT("slGetNewFrameToken = %p"), Ptr_getNewFrameToken);
			check(Ptr_getNewFrameToken);

			Ptr_setD3DDevice = (PFun_slSetD3DDevice*)(FWindowsPlatformProcess::GetDllExport(SLInterPoserDLL, TEXT("slSetD3DDevice")));
			UE_LOG(LogStreamlineRHI, Log, TEXT("slSetD3DDevice = %p"), Ptr_setD3DDevice);
			check(Ptr_setD3DDevice);

			bIsStreamlineFunctionPointersLoaded = true;
		}
	}

	return bIsStreamlineFunctionPointersLoaded;
}

#undef LOCTEXT_NAMESPACE

