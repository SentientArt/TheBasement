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

#include "StreamlineReflex.h"
#include "StreamlineCorePrivate.h"

#include "HAL/IConsoleManager.h"
#include "RHI.h"

#include "StreamlineDLSSG.h"

#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#include "Framework/Application/SlateApplication.h"

#include "StreamlineCore.h"
#include "StreamlineRHI.h"
#include "StreamlineAPI.h"
#include "sl_helpers.h"
#include "sl_reflex.h"

static TAutoConsoleVariable<bool> CVarStreamlineUnregisterReflexPlugin(
	TEXT("r.Streamline.UnregisterReflexPlugin"),
	true,
	TEXT("The existing NVAPI based UE Reflex plugin is incompatible with the DLSS Frame Generation based implementation. This cvar controls whether the Reflex plugin should be unregistered from the engine or not.\n")
	TEXT("0: keep Reflex plugin modular features registered\n")
	TEXT("1: unregister Reflex plugin modular features. The Reflex blueprint library should work with the DLSS Frame Generation plugin modular features 🤞\n"),
	ECVF_ReadOnly);

static TAutoConsoleVariable<int32> CVarStreamlineReflexEnable(
	TEXT("t.Streamline.Reflex.Enable"),
	0,
	TEXT("Enable Streamline Reflex extension. (default = 0)\n")
	TEXT("0: Disabled\n")
	TEXT("1: Enabled)\n"),
	ECVF_RenderThreadSafe);

static TAutoConsoleVariable<int32> CVarStreamlineReflexEnableLatencyMarkers(
	TEXT("t.Streamline.Reflex.EnableLatencyMarkers"),
	1,
	TEXT("Enable Streamline Reflex Latency metrics. (default = 1)\n")
	TEXT("0: Disabled\n")
	TEXT("1: Enabled)\n"),
	ECVF_RenderThreadSafe);

static TAutoConsoleVariable<int32> CVarStreamlineReflexAuto(
	TEXT("t.Streamline.Reflex.Auto"),
	1,
	TEXT("Enable Streamline Reflex extension when other SL features need it. (default = 1)\n")
	TEXT("0: Disabled\n")
	TEXT("1: Enabled)\n"),
	ECVF_ReadOnly);

static TAutoConsoleVariable<bool> CVarStreamlineReflexEnableInEditor(
	TEXT("t.Streamline.Reflex.EnableInEditor"),
	true,
	TEXT("Enable Streamline Reflex in the editor. (default = 1)\n")
	TEXT("0: Disabled\n")
	TEXT("1: Enabled)\n"),
	ECVF_RenderThreadSafe);

static TAutoConsoleVariable<int32> CVarStreamlineReflexMode(
	TEXT("t.Streamline.Reflex.Mode"),
	1,
	TEXT("Streamline Reflex mode (default = 1)\n")
	TEXT("0: off \n")
	TEXT("1: low latency\n")
	TEXT("2: low latency with boost\n"),
	ECVF_RenderThreadSafe);

static TAutoConsoleVariable<bool> CVarStreamlineReflexHandleMaxTickRate(
	TEXT("t.Streamline.Reflex.HandleMaxTickRate"),
	true,
	TEXT("Controls whether Streamline Reflex handles frame rate limiting instead of the engine (default = true)"),
	ECVF_Default);

static TUniquePtr<FStreamlineMaxTickRateHandler> StreamlineMaxTickRateHandler;
static TUniquePtr<FStreamlineLatencyMarkers> StreamlineLatencyMarker;

bool FStreamlineLatencyBase::bStreamlineReflexSupported = false;
bool FStreamlineLatencyBase::IsStreamlineReflexSupported()
{
	if (!FApp::CanEverRender())
	{
		return false;
	}

	if (!IsStreamlineSupported())
	{
		return false;
	}

#if WITH_EDITOR
	if (GIsEditor && !CVarStreamlineReflexEnableInEditor.GetValueOnAnyThread())
	{
		return false;
	}
#endif

	static bool bStreamlineReflexSupportedInitialized = false;

	if (!bStreamlineReflexSupportedInitialized)
	{
		const sl::AdapterInfo* AdapterInfo = FStreamlineCoreModule::GetStreamlineRHI()->GetAdapterInfo();
		sl::Result Result = SLisFeatureSupported(sl::kFeatureReflex, *AdapterInfo);
		bStreamlineReflexSupported = Result == sl::Result::eOk;
		//UE_LOG(LogStreamline, Log, TEXT("%s Streamline Reflex supported %u (%u, %s)"), ANSI_TO_TCHAR(__FUNCTION__), bStreamlineReflexSupported,
		//	Result, ANSI_TO_TCHAR(sl::getResultAsStr(Result)));
		bStreamlineReflexSupportedInitialized = true;
		LogStreamlineFeatureSupport(sl::kFeatureReflex, *AdapterInfo);

	}
	return bStreamlineReflexSupported;
}

void FStreamlineMaxTickRateHandler::Initialize()
{
	if (IsStreamlineReflexSupported())
	{
		sl::ReflexState ReflexState{};
		sl::Result Result = CALL_SL_FEATURE_FN(sl::kFeatureReflex, slReflexGetState, ReflexState);
		checkf(Result == sl::Result::eOk, TEXT("slReflexGetState failed (%s)"), ANSI_TO_TCHAR(sl::getResultAsStr(Result)));

		UE_LOG(LogStreamline, Log, TEXT("%s sl::ReflexState::lowLatencyAvailable=%u"), ANSI_TO_TCHAR(__FUNCTION__), ReflexState.lowLatencyAvailable);
		UE_LOG(LogStreamline, Log, TEXT("%s sl::ReflexState::latencyReportAvailable=%u"), ANSI_TO_TCHAR(__FUNCTION__), ReflexState.latencyReportAvailable);

		if (!ReflexState.lowLatencyAvailable)
		{
			CVarStreamlineReflexEnable->Set(false, ECVF_SetByCommandline);
		}

		const sl::ReflexOptions MakeSureStreamlineReflexCallsNVSTATS_INITAtLeastOnce;
		CALL_SL_FEATURE_FN(sl::kFeatureReflex, slReflexSetOptions, MakeSureStreamlineReflexCallsNVSTATS_INITAtLeastOnce);
	}
}

void FStreamlineMaxTickRateHandler::SetEnabled(bool bInEnabled)
{
	if (!GetAvailable())
	{
		bInEnabled = false;
		UE_LOG(LogStreamline, Log, TEXT("%s Tried to set SL Reflex Low Latency state but SL Reflex is not available"), ANSI_TO_TCHAR(__FUNCTION__));
	}

	CVarStreamlineReflexEnable->Set(bInEnabled, ECVF_SetByCommandline);
}

bool FStreamlineMaxTickRateHandler::GetEnabled()
{
	if (!GetAvailable())
	{
		return false;
	}
#if WITH_EDITOR
	if (GIsEditor && !CVarStreamlineReflexEnableInEditor.GetValueOnAnyThread())
	{
		return false;
	}
#endif

	int32 CVarReflex = CVarStreamlineReflexEnable.GetValueOnAnyThread();
	int32 CVarReflexAuto = CVarStreamlineReflexAuto.GetValueOnAnyThread();
	if ((CVarReflexAuto) != 0 && IsDLSSGActive())
	{
		return true;
	}
	else
	{
		return CVarReflex != 0;
	}

}

bool FStreamlineMaxTickRateHandler::GetAvailable()
{
	if (!IsStreamlineReflexSupported())
	{
		return false;
	}

	sl::ReflexState ReflexState{};
	sl::Result Result = CALL_SL_FEATURE_FN(sl::kFeatureReflex, slReflexGetState, ReflexState);
	checkf(Result == sl::Result::eOk, TEXT("slReflexGetState failed (%s)"), ANSI_TO_TCHAR(sl::getResultAsStr(Result)));

	return ReflexState.lowLatencyAvailable;
}

void FStreamlineMaxTickRateHandler::SetFlags(uint32 Flags)
{
	bool bLowLatencyMode = false;
	bool bBoost = false;

	if ((Flags & 1) > 0)
	{
		bLowLatencyMode = true;
	}
	else
	{
		bLowLatencyMode = false;
	}
	if ((Flags & 2) > 0)
	{
		bBoost = true;
	}
	else
	{
		bBoost = false;
	}

	if (bLowLatencyMode && bBoost)
	{
		CVarStreamlineReflexMode->Set( sl::ReflexMode::eLowLatencyWithBoost, ECVF_SetByCommandline);
	}
	else if(bLowLatencyMode && !bBoost)
	{
		CVarStreamlineReflexMode->Set(sl::ReflexMode::eLowLatency, ECVF_SetByCommandline);
	}
	else
	{
		CVarStreamlineReflexMode->Set(sl::ReflexMode::eOff, ECVF_SetByCommandline);
	}
}

uint32 FStreamlineMaxTickRateHandler::GetFlags()
{

	int32 CVarReflexMode = CVarStreamlineReflexMode.GetValueOnAnyThread();
	int32 CVarReflexAuto = CVarStreamlineReflexAuto.GetValueOnAnyThread();

	if ((CVarReflexAuto != 0) && IsDLSSGActive() && CVarReflexMode == sl::eOff)
	{
		return sl::eLowLatency;
	}
	else
	{
		return CVarReflexMode;
	}
}

// Give "stat threading" visibility into the max tick rate handler, since it has no visibility by default
// This is the equivalent of STAT_GameTickWaitTime for our handler
DECLARE_CYCLE_STAT(TEXT("Game thread wait time (Reflex)"), STAT_GameTickReflexWaitTime, STATGROUP_Threading);

static float CalculateDesiredMinimumIntervalUs(float DesiredMaxTickRate, float DeltaRealTimeMinusSleep)
{
	const float DesiredMinimumInterval = DesiredMaxTickRate > 0 ? (1.0f / DesiredMaxTickRate) : 0.0f;

	// Attempt to approximate effect of engine's calculation of WaitTime when a max tick rate handler doesn't handle sleeping.
	// See this line in UEngine::UpdateTimeAndHandleMaxTickRate():
	//	WaitTime = FMath::Max( 1.f / MaxTickRate - DeltaRealTime, 0.f );
	// where DeltaRealTime does NOT include the time that the previous frame spent sleeping in UpdateTimeAndHandleMaxTickRate.
	//
	// This WaitTime behavior may seem counter-intuitive. After all, it permits a tick rate higher than the requested rate. But some
	// questionable engine math in areas like frame rate smoothing necessitates it, or otherwise the frame rate will tend to keep
	// getting lower.
	if (DesiredMinimumInterval < DeltaRealTimeMinusSleep)
	{
		return 0.0f;
	}

	return 1.0E6f * DesiredMinimumInterval;
}

static bool AreReflexOptionsEquivalent(const sl::ReflexOptions& Opt1, const sl::ReflexOptions& Opt2)
{
	return (Opt1.mode == Opt2.mode)
		&& (Opt1.frameLimitUs == Opt2.frameLimitUs)
		&& (Opt1.useMarkersToOptimize == Opt2.useMarkersToOptimize)
		&& (Opt1.virtualKey == Opt2.virtualKey)
		&& (Opt1.idThread == Opt2.idThread);
}

static void UpdateReflexOptionsIfChanged(const sl::ReflexOptions& ReflexOptions)
{
	static sl::ReflexOptions LastFrameOptions{};
	if (!AreReflexOptionsEquivalent(LastFrameOptions, ReflexOptions))
	{
		sl::Result Result = CALL_SL_FEATURE_FN(sl::kFeatureReflex, slReflexSetOptions, ReflexOptions);
		checkf(Result == sl::Result::eOk, TEXT("slReflexSetOptions failed (%s)"), ANSI_TO_TCHAR(sl::getResultAsStr(Result)));
		LastFrameOptions = ReflexOptions;
	}
}

bool FStreamlineMaxTickRateHandler::HandleMaxTickRate(float DesiredMaxTickRate)
{
	bool bFrameRateHandled = false;
	if (GetEnabled())
	{
		SCOPE_CYCLE_COUNTER(STAT_GameTickReflexWaitTime);
		const double CurrentRealTime = FPlatformTime::Seconds();
		static double LastRealTimeAfterSleep = CurrentRealTime - 0.0001;
		const float DeltaRealTimeMinusSleep = static_cast<float>(CurrentRealTime - LastRealTimeAfterSleep);

		sl::ReflexOptions ReflexOptions = {};

		ReflexOptions.mode = static_cast<sl::ReflexMode>(FMath::Clamp(GetFlags(), uint32(sl::ReflexMode::eOff), uint32(sl::ReflexMode::eLowLatencyWithBoost)));

		if (!CVarStreamlineReflexHandleMaxTickRate.GetValueOnAnyThread() || GIsEditor
#if (ENGINE_MAJOR_VERSION < 5) || (ENGINE_MINOR_VERSION < 2)
			// Timing logic in older engines is a little different, we don't handle this yet
			|| GEngine->IsAllowedFramerateSmoothing()
#endif
			)
		{
			// Note: Currently force it to let the engine handle max tick rate in PIE because the editor can get in a state where
			// the engine requests a DesiredMaxTickRate of 3.0, and then it never recovers from that.
			// Issue seen in UE 5.2, not well tested in older engine versions
			ReflexOptions.frameLimitUs = 0;
		}
		else
		{
			const float DesiredMinimumIntervalUs = CalculateDesiredMinimumIntervalUs(DesiredMaxTickRate, DeltaRealTimeMinusSleep);
#if ENGINE_MAJOR_VERSION > 4
			ReflexOptions.frameLimitUs = FMath::TruncToInt32(DesiredMinimumIntervalUs);
#else
			ReflexOptions.frameLimitUs = FMath::TruncToInt(DesiredMinimumIntervalUs);
#endif
			bFrameRateHandled = true;
		}
		ReflexOptions.useMarkersToOptimize = true;

		UpdateReflexOptionsIfChanged(ReflexOptions);

		sl::FrameToken* FrameToken = FStreamlineCoreModule::GetStreamlineRHI()->GetFrameToken(GFrameCounter);
		sl::Result Result = CALL_SL_FEATURE_FN(sl::kFeatureReflex, slReflexSleep, *FrameToken);
		checkf(Result == sl::Result::eOk, TEXT("slReflexSleep failed (%s)"), ANSI_TO_TCHAR(sl::getResultAsStr(Result)));
		LastRealTimeAfterSleep = FPlatformTime::Seconds();
	}
	else
	{
		sl::ReflexOptions ReflexOptions{};
		ReflexOptions.mode = sl::ReflexMode::eOff;
		UpdateReflexOptionsIfChanged(ReflexOptions);
	}

	return bFrameRateHandled;
}


void FStreamlineLatencyMarkers::Initialize()
{
	if (IsStreamlineReflexSupported())
	{
		sl::ReflexState ReflexState{};
		sl::Result Result = CALL_SL_FEATURE_FN(sl::kFeatureReflex, slReflexGetState, ReflexState);
		checkf(Result == sl::Result::eOk, TEXT("slReflexGetState failed (%s)"), ANSI_TO_TCHAR(sl::getResultAsStr(Result)));

		UE_LOG(LogStreamline, Log, TEXT("%s sl::ReflexState::flashIndicatorDriverControlled=%u"), ANSI_TO_TCHAR(__FUNCTION__), ReflexState.flashIndicatorDriverControlled);
		bFlashIndicatorDriverControlled = ReflexState.flashIndicatorDriverControlled;
	}
}

void FStreamlineLatencyMarkers::Tick(float DeltaTime)
{
	if (GetEnabled())
	{
		sl::ReflexState ReflexState{};
		sl::Result Result = CALL_SL_FEATURE_FN(sl::kFeatureReflex, slReflexGetState, ReflexState);
		checkf(Result == sl::Result::eOk, TEXT("slReflexGetState failed (%s)"), ANSI_TO_TCHAR(sl::getResultAsStr(Result)));

		if (ReflexState.latencyReportAvailable)
		{
			// frameReport[63] contains the latest completed frameReport
			const uint64_t TotalLatencyUs = ReflexState.frameReport[63].gpuRenderEndTime - ReflexState.frameReport[63].simStartTime;

			if (TotalLatencyUs != 0)
			{
				// frameReport results available, get latest completed frame latency data
				// A 3/4, 1/4 split gets close to a simple 10 frame moving average
				AverageTotalLatencyMs = AverageTotalLatencyMs * 0.75f + TotalLatencyUs / 1000.0f * 0.25f;

				AverageGameLatencyMs = AverageGameLatencyMs * 0.75f + (ReflexState.frameReport[63].driverEndTime - ReflexState.frameReport[63].simStartTime) / 1000.0f * 0.25f;
				AverageRenderLatencyMs = AverageRenderLatencyMs * 0.75f + (ReflexState.frameReport[63].gpuRenderEndTime - ReflexState.frameReport[63].osRenderQueueStartTime) / 1000.0f * 0.25f;

				AverageSimulationLatencyMs = AverageSimulationLatencyMs * 0.75f + (ReflexState.frameReport[63].simEndTime - ReflexState.frameReport[63].simStartTime) / 1000.0f * 0.25f;
				AverageRenderSubmitLatencyMs = AverageRenderSubmitLatencyMs * 0.75f + (ReflexState.frameReport[63].renderSubmitEndTime - ReflexState.frameReport[63].renderSubmitStartTime) / 1000.0f * 0.25f;
				AveragePresentLatencyMs = AveragePresentLatencyMs * 0.75f + (ReflexState.frameReport[63].presentEndTime - ReflexState.frameReport[63].presentStartTime) / 1000.0f * 0.25f;
				AverageDriverLatencyMs = AverageDriverLatencyMs * 0.75f + (ReflexState.frameReport[63].driverEndTime - ReflexState.frameReport[63].driverStartTime) / 1000.0f * 0.25f;
				AverageOSRenderQueueLatencyMs = AverageOSRenderQueueLatencyMs * 0.75f + (ReflexState.frameReport[63].osRenderQueueEndTime - ReflexState.frameReport[63].osRenderQueueStartTime) / 1000.0f * 0.25f;
				AverageGPURenderLatencyMs = AverageGPURenderLatencyMs * 0.75f + (ReflexState.frameReport[63].gpuRenderEndTime - ReflexState.frameReport[63].gpuRenderStartTime) / 1000.0f * 0.25f;

				RenderSubmitOffsetMs = (ReflexState.frameReport[63].renderSubmitStartTime - ReflexState.frameReport[63].simStartTime) / 1000.0f;
				PresentOffsetMs = (ReflexState.frameReport[63].presentStartTime - ReflexState.frameReport[63].simStartTime) / 1000.0f;
				DriverOffsetMs = (ReflexState.frameReport[63].driverStartTime - ReflexState.frameReport[63].simStartTime) / 1000.0f;
				OSRenderQueueOffsetMs = (ReflexState.frameReport[63].osRenderQueueStartTime - ReflexState.frameReport[63].simStartTime) / 1000.0f;
				GPURenderOffsetMs = (ReflexState.frameReport[63].gpuRenderStartTime - ReflexState.frameReport[63].simStartTime) / 1000.0f;

				UE_LOG(LogStreamline, VeryVerbose, TEXT("AverageTotalLatencyMs: %f"), AverageTotalLatencyMs);
				UE_LOG(LogStreamline, VeryVerbose, TEXT("AverageGameLatencyMs: %f"), AverageGameLatencyMs);
				UE_LOG(LogStreamline, VeryVerbose, TEXT("AverageRenderLatencyMs: %f"), AverageRenderLatencyMs);

				UE_LOG(LogStreamline, VeryVerbose, TEXT("AverageSimulationLatencyMs: %f"), AverageSimulationLatencyMs);
				UE_LOG(LogStreamline, VeryVerbose, TEXT("AverageRenderSubmitLatencyMs: %f"), AverageRenderSubmitLatencyMs);
				UE_LOG(LogStreamline, VeryVerbose, TEXT("AveragePresentLatencyMs: %f"), AveragePresentLatencyMs);
				UE_LOG(LogStreamline, VeryVerbose, TEXT("AverageDriverLatencyMs: %f"), AverageDriverLatencyMs);
				UE_LOG(LogStreamline, VeryVerbose, TEXT("AverageOSRenderQueueLatencyMs: %f"), AverageOSRenderQueueLatencyMs);
				UE_LOG(LogStreamline, VeryVerbose, TEXT("AverageGPURenderLatencyMs: %f"), AverageGPURenderLatencyMs);
			}
		}
	}
	else
	{
		// Reset module back to default values in case re-enabled in the same session
		// doing this here in case the cvar gets used to disable latency (vs SetEnabled)
		AverageTotalLatencyMs = 0.0f;
		AverageGameLatencyMs = 0.0f;
		AverageRenderLatencyMs = 0.0f;

		AverageSimulationLatencyMs = 0.0f;
		AverageRenderSubmitLatencyMs = 0.0f;
		AveragePresentLatencyMs = 0.0f;
		AverageDriverLatencyMs = 0.0f;
		AverageOSRenderQueueLatencyMs = 0.0f;
		AverageGPURenderLatencyMs = 0.0f;

		RenderSubmitOffsetMs = 0.0f;
		PresentOffsetMs = 0.0f;
		DriverOffsetMs = 0.0f;
		OSRenderQueueOffsetMs = 0.0f;
		GPURenderOffsetMs = 0.0f;

		AverageTotalLatencyMs = 0.0f;
		AverageGameLatencyMs = 0.0f;
		AverageRenderLatencyMs = 0.0f;

		AverageSimulationLatencyMs = 0.0f;
		AverageRenderSubmitLatencyMs = 0.0f;
		AveragePresentLatencyMs = 0.0f;
		AverageDriverLatencyMs = 0.0f;
		AverageOSRenderQueueLatencyMs = 0.0f;
		AverageGPURenderLatencyMs = 0.0f;

		RenderSubmitOffsetMs = 0.0f;
		PresentOffsetMs = 0.0f;
		DriverOffsetMs = 0.0f;
		OSRenderQueueOffsetMs = 0.0f;
		GPURenderOffsetMs = 0.0f;
	}
}

void FStreamlineLatencyMarkers::SetInputSampleLatencyMarker(uint64 FrameNumber)
{
	SetCustomLatencyMarker(sl::ReflexMarker::eInputSample, FrameNumber);
}

void FStreamlineLatencyMarkers::SetSimulationLatencyMarkerStart(uint64 FrameNumber)
{
	SetCustomLatencyMarker(sl::ReflexMarker::eSimulationStart, FrameNumber);
}

void FStreamlineLatencyMarkers::SetSimulationLatencyMarkerEnd(uint64 FrameNumber)
{
	SetCustomLatencyMarker(sl::ReflexMarker::eSimulationEnd, FrameNumber);
}

void FStreamlineLatencyMarkers::SetRenderSubmitLatencyMarkerStart(uint64 FrameNumber)
{
	SetCustomLatencyMarker(sl::ReflexMarker::eRenderSubmitStart, FrameNumber);
}

void FStreamlineLatencyMarkers::SetRenderSubmitLatencyMarkerEnd(uint64 FrameNumber)
{
	SetCustomLatencyMarker(sl::ReflexMarker::eRenderSubmitEnd, FrameNumber);
}

void FStreamlineLatencyMarkers::SetPresentLatencyMarkerStart(uint64 FrameNumber)
{
	SetCustomLatencyMarker(sl::ReflexMarker::ePresentStart, FrameNumber);
}

void FStreamlineLatencyMarkers::SetPresentLatencyMarkerEnd(uint64 FrameNumber)
{
	SetCustomLatencyMarker(sl::ReflexMarker::ePresentEnd, FrameNumber);

	// we are calling this here since that's right after present.
	GetDLSSGStatusFromStreamline();
}

void FStreamlineLatencyMarkers::SetFlashIndicatorLatencyMarker(uint64 FrameNumber)
{
	if (GetFlashIndicatorEnabled())
	{
		SetCustomLatencyMarker(sl::ReflexMarker::eTriggerFlash, FrameNumber );
	}

}

void FStreamlineLatencyMarkers::SetCustomLatencyMarker(uint32 MarkerId, uint64 FrameNumber)
{
	if (GetEnabled())
	{
		sl::ReflexMarker Marker = static_cast<sl::ReflexMarker>(MarkerId);
		sl::FrameToken* FrameToken = FStreamlineCoreModule::GetStreamlineRHI()->GetFrameToken(FrameNumber);
		sl::Result Result = CALL_SL_FEATURE_FN(sl::kFeatureReflex, slReflexSetMarker, Marker, *FrameToken);
		checkf(Result == sl::Result::eOk, TEXT("slReflexSetMarker failed MarkerId=%s (%s)"),
			ANSI_TO_TCHAR(sl::getReflexMarkerAsStr(Marker)), ANSI_TO_TCHAR(sl::getResultAsStr(Result)));
	}
}

bool FStreamlineLatencyMarkers::ProcessMessage(HWND hwnd, uint32 msg, WPARAM wParam, LPARAM lParam, int32& OutResult)
{
	if (GetEnabled())
	{
		sl::ReflexState LatencySettings{};
		sl::Result Result = CALL_SL_FEATURE_FN(sl::kFeatureReflex, slReflexGetState, LatencySettings);
		checkf(Result == sl::Result::eOk, TEXT("slReflexGetState failed (%s)"), ANSI_TO_TCHAR(sl::getResultAsStr(Result)));

		if(LatencySettings.statsWindowMessage == msg)
		{
			// Latency ping based on custom message, command buffer is not needed hence null
			sl::FrameToken* FrameToken = FStreamlineCoreModule::GetStreamlineRHI()->GetFrameToken(GFrameCounter);
			Result = CALL_SL_FEATURE_FN(sl::kFeatureReflex, slReflexSetMarker, sl::ReflexMarker::ePCLatencyPing, *FrameToken);
			checkf(Result == sl::Result::eOk, TEXT("slReflexSetMarker ePCLatencyPing failed (%s)"), ANSI_TO_TCHAR(sl::getResultAsStr(Result)));
			return true;
		}
	}
	return false;
}

void FStreamlineLatencyMarkers::SetEnabled(bool bInEnabled)
{
	CVarStreamlineReflexEnableLatencyMarkers->Set(bInEnabled, ECVF_SetByCommandline);
}

bool FStreamlineLatencyMarkers::GetEnabled()
{
	if (!GetAvailable())
	{
		return false;
	}

	int32 CVarReflex = CVarStreamlineReflexEnableLatencyMarkers.GetValueOnAnyThread();
	return CVarReflex != 0;
}

bool FStreamlineLatencyMarkers::GetAvailable()
{
	return IsStreamlineReflexSupported();
}

void FStreamlineLatencyMarkers::SetFlashIndicatorEnabled(bool bInEnabled)
{
	UE_LOG(LogStreamline, Log, TEXT("FStreamlineLatencyMarkers::SetFlashIndicatorEnabled is obsolete and non-functional. The Reflex Flash Indicator is configured by the NVIDIA GeForce Experience overlay"));
}

bool FStreamlineLatencyMarkers::GetFlashIndicatorEnabled()
{
	return GetEnabled() && bFlashIndicatorDriverControlled;
}

void RegisterStreamlineReflexHooks()
{
	UE_LOG(LogStreamline, Log, TEXT("%s Enter"), ANSI_TO_TCHAR(__FUNCTION__));

	TSharedPtr<IPlugin> ReflexPlugin = IPluginManager::Get().FindPlugin(TEXT("Reflex"));
	const bool bIsReflexPluginEnabled = ReflexPlugin && (ReflexPlugin->IsEnabled() || ReflexPlugin->IsEnabledByDefault(false));
	if (bIsReflexPluginEnabled)
	{
		UE_LOG(LogStreamline, Log, TEXT("Reflex plugin enabled, which is incompatible with the Reflex implementation provided by this Streamline UE plugin"));

		if (CVarStreamlineUnregisterReflexPlugin.GetValueOnAnyThread() != 0)
		{
			UE_LOG(LogStreamline, Log, TEXT("Unregistering the Reflex plugin related modular features. The Reflex plugin Blueprint library is expected to continue to work 🤞."));

			auto UnregisterFeatures = [](const FName& FeatureName)
			{
				TArray< IModularFeature*> Features = IModularFeatures::Get().GetModularFeatureImplementations< IModularFeature>(FeatureName);
				for (IModularFeature* Feature : Features)
				{
					UE_LOG(LogStreamline, Log, TEXT("Unregistering %s %p "), *FeatureName.ToString(), Feature);
					IModularFeatures::Get().UnregisterModularFeature(FeatureName, Feature);
				}
			};

			UnregisterFeatures(IMaxTickRateHandlerModule::GetModularFeatureName());
			UnregisterFeatures(ILatencyMarkerModule::GetModularFeatureName());
		}
		else
		{
			UE_LOG(LogStreamline, Log, TEXT("It is recommended to either disable the Reflex plugin or set r.Streamline.UnregisterReflexPlugin to disable the incompatible parts of the Reflex plugin."));
		}
	}


	// register the modular features for the engine to call into the SL latency APIs at various places across frame & threads
	StreamlineMaxTickRateHandler = MakeUnique<FStreamlineMaxTickRateHandler>();
	StreamlineMaxTickRateHandler->Initialize();
	IModularFeatures::Get().RegisterModularFeature(StreamlineMaxTickRateHandler->GetModularFeatureName(), StreamlineMaxTickRateHandler.Get());

	StreamlineLatencyMarker = MakeUnique<FStreamlineLatencyMarkers>();
	StreamlineLatencyMarker->Initialize();
	IModularFeatures::Get().RegisterModularFeature(StreamlineLatencyMarker->GetModularFeatureName(), StreamlineLatencyMarker.Get());

	// this one needs to be registered so ProcessWindowMessage gets called
	{
		check(FSlateApplication::IsInitialized());
		FWindowsApplication* WindowsApplication = (FWindowsApplication*)FSlateApplication::Get().GetPlatformApplication().Get();
		check(WindowsApplication);
		WindowsApplication->AddMessageHandler(*StreamlineLatencyMarker);

		FSlateApplication::Get().OnPreShutdown().AddLambda(
			[]()
		{
			FWindowsApplication* WindowsApplication = (FWindowsApplication*)FSlateApplication::Get().GetPlatformApplication().Get();
			check(WindowsApplication);
			WindowsApplication->RemoveMessageHandler(*StreamlineLatencyMarker);
		}
		);
	}
	UE_LOG(LogStreamline, Log, TEXT("%s Leave"), ANSI_TO_TCHAR(__FUNCTION__));
}

void UnregisterStreamlineReflexHooks()
{
	UE_LOG(LogStreamline, Log, TEXT("%s Enter"), ANSI_TO_TCHAR(__FUNCTION__));
	IModularFeatures::Get().UnregisterModularFeature(StreamlineMaxTickRateHandler->GetModularFeatureName(), StreamlineMaxTickRateHandler.Get());
	StreamlineMaxTickRateHandler.Reset();

	IModularFeatures::Get().UnregisterModularFeature(StreamlineLatencyMarker->GetModularFeatureName(), StreamlineLatencyMarker.Get());

	StreamlineLatencyMarker.Reset();

	UE_LOG(LogStreamline, Log, TEXT("%s Leave"), ANSI_TO_TCHAR(__FUNCTION__));
}
