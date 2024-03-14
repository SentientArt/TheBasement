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

#include "StreamlineRHI.h"
#include "StreamlineAPI.h"
#include "StreamlineConversions.h"
#include "StreamlineRHIPrivate.h"
#include "StreamlineSettings.h"

#if WITH_EDITOR
#include "Editor.h"
#endif
#include "GenericPlatform/GenericPlatformFile.h"
#include "HAL/IConsoleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/App.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/EngineVersion.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"


DEFINE_LOG_CATEGORY(LogStreamlineRHI);
DEFINE_LOG_CATEGORY_STATIC(LogStreamlineAPI, Log, All);

#define LOCTEXT_NAMESPACE "StreamlineRHI"

static void StreamlineLogSink(sl::LogType InSLVerbosity, const char* InSLMessage)
{
	FString Message(FString(UTF8_TO_TCHAR(InSLMessage)).TrimEnd());

	static_assert(uint32_t(sl::LogType::eCount)  == 3U, "sl::LogType enum value mismatch. Dear NVIDIA Streamline plugin developer, please update this code!" ) ;

	if (Message.Contains(TEXT("[operator ()] 'kFeatureDLSS_G' is missing")))
	{
		// nuisance message that appears periodically when the FG feature isn't loaded
		return;
	}
	// TODO REMOVE
	if (Message.Contains(TEXT("[streamline][error]commoninterface.h")) && Message.Contains(TEXT("same frame is NOT allowed!")))
	{
		InSLVerbosity = sl::LogType::eWarn;
	}

	switch (InSLVerbosity)
	{
		default:
		case sl::LogType::eInfo:
			UE_LOG(LogStreamlineAPI, Log, TEXT("[Info]: %s"), *Message);
			break;
		case sl::LogType::eWarn:
			UE_LOG(LogStreamlineAPI, Warning, TEXT("[Warn]: %s"), *Message);
			break;
		case sl::LogType::eError:
			UE_LOG(LogStreamlineAPI, Error, TEXT("[Error]: %s"),*Message);
			break;
	}
}

static bool bIsStreamlineInitialized = false;

static int32 GetNGXAppID(bool bIsDLSSPluginEnabled)
{
	check(GConfig);

	// Streamline plugin NGX app ID
	int32 SLNGXAppID = 0;
	GConfig->GetInt(TEXT("/Script/StreamlineRHI.StreamlineSettings"), TEXT("NVIDIANGXApplicationId"), SLNGXAppID, GEngineIni);

	if (!bIsDLSSPluginEnabled)
	{
		return SLNGXAppID;
	}

	// DLSS-SR plugin NGX app ID
	int32 DLSSSRNGXAppID = 0;
	GConfig->GetInt(TEXT("/Script/DLSS.DLSSSettings"), TEXT("NVIDIANGXApplicationId"), DLSSSRNGXAppID, GEngineIni);

	int32 NGXAppID = 0;
	if (DLSSSRNGXAppID == SLNGXAppID)
	{
		NGXAppID = SLNGXAppID;
	}
	else if (DLSSSRNGXAppID == 0)
	{
		NGXAppID = SLNGXAppID;
		UE_LOG(LogStreamlineRHI, Warning, TEXT("Using NGX app ID %d from Streamline plugin, may affect DLSS-SR even though NGX app ID is not set in DLSS-SR plugin"), NGXAppID);
	}
	else if (SLNGXAppID == 0)
	{
		NGXAppID = DLSSSRNGXAppID;
		UE_LOG(LogStreamlineRHI, Warning, TEXT("Using NGX app ID %d from DLSS-SR plugin, may affect DLSS-FG even though NGX app ID is not set in Streamline plugin"), NGXAppID);
	}
	else
	{
		NGXAppID = SLNGXAppID;
		UE_LOG(LogStreamlineRHI, Error, TEXT("NGX app ID mismatch! %d in DLSS-SR plugin, %d in Streamline plugin, using %d"), DLSSSRNGXAppID, SLNGXAppID, NGXAppID);
	}

	return NGXAppID;
}

// TODO: the derived RHIs will set this to true during their initialization
bool FStreamlineRHI::bIsIncompatibleAPICaptureToolActive = false;

FSLFrameTokenProvider::FSLFrameTokenProvider() : Section()
{
	// truncated to 32 bits because that's all SL stores
	LastFrameCounter = static_cast<uint32_t>(GFrameCounter);
	SLgetNewFrameToken(FrameToken, &LastFrameCounter);
}

sl::FrameToken* FSLFrameTokenProvider::GetTokenForFrame(uint64 FrameCounter)
{
	uint32_t FrameCounter32 = static_cast<uint32_t>(FrameCounter);
	FScopeLock Lock(&Section);
	if (FrameCounter32 == LastFrameCounter)
	{
		return FrameToken;
	}

	// this should be safe, we can create multiple tokens to track the same frame
	LastFrameCounter = FrameCounter32;
	SLgetNewFrameToken(FrameToken, &LastFrameCounter);

	return FrameToken;
}


FStreamlineRHI::FStreamlineRHI(const FStreamlineRHICreateArguments& Arguments)
	: DynamicRHI(Arguments.DynamicRHI), FrameTokenProvider(MakeUnique<FSLFrameTokenProvider>())
{
	UE_LOG(LogStreamlineRHI, Log, TEXT("%s Enter"), ANSI_TO_TCHAR(__FUNCTION__));
#if WITH_EDITOR
	BeginPIEHandle = FEditorDelegates::BeginPIE.AddLambda([this](bool bIsSimulating) { OnBeginPIE(bIsSimulating); });
	EndPIEHandle = FEditorDelegates::EndPIE.AddLambda([this](bool bIsSimulating) { OnEndPIE(bIsSimulating); });
#endif
	UE_LOG(LogStreamlineRHI, Log, TEXT("%s Leave"), ANSI_TO_TCHAR(__FUNCTION__));
}

bool FStreamlineRHI::IsSwapchainHookingAllowed() const
{
	if (!IsDLSSGSupportedByRHI())
	{
		return false;
	}
#if WITH_EDITOR
	if (GIsEditor)
	{
		if (bIsPIEActive)
		{
			EStreamlineSettingOverride PIEOverride = GetDefault<UStreamlineOverrideSettings>()->EnableDLSSFGInPlayInEditorViewportsOverride;
			if (PIEOverride == EStreamlineSettingOverride::UseProjectSettings)
			{
				return GetDefault<UStreamlineSettings>()->bEnableDLSSFGInPlayInEditorViewports;
			}
			else
			{
				return PIEOverride == EStreamlineSettingOverride::Enabled;
			}
		}
		return false;
	}
#endif
	return true;
}

bool FStreamlineRHI::IsStreamlineAvailable() const
{
	return IsStreamlineSupported();
}

void FStreamlineRHI::SetStreamlineData(FRHICommandList& CmdList, const FRHIStreamlineArguments& InArguments)
{
	check(!IsRunningRHIInSeparateThread() || IsInRHIThread());
	sl::Constants StreamlineConstants = {};

	StreamlineConstants.reset = ToSL(InArguments.bReset);
	StreamlineConstants.jitterOffset = ToSL(InArguments.JitterOffset);

	StreamlineConstants.depthInverted = ToSL(InArguments.bIsDepthInverted);

	StreamlineConstants.mvecScale = ToSL(InArguments.MotionVectorScale);
	StreamlineConstants.motionVectorsDilated = ToSL(InArguments.bAreMotionVectorsDilated);
	StreamlineConstants.cameraMotionIncluded = sl::eTrue;
	StreamlineConstants.motionVectors3D = sl::eFalse;

	StreamlineConstants.orthographicProjection = ToSL(InArguments.bIsOrthographicProjection);
	StreamlineConstants.cameraViewToClip = ToSL(InArguments.CameraViewToClip, InArguments.bIsOrthographicProjection);
	StreamlineConstants.clipToCameraView = ToSL(InArguments.ClipToCameraView);
	StreamlineConstants.clipToLensClip = ToSL(InArguments.ClipToLenseClip);
	StreamlineConstants.clipToPrevClip = ToSL(InArguments.ClipToPrevClip);
	StreamlineConstants.prevClipToClip = ToSL(InArguments.PrevClipToClip);
	
	StreamlineConstants.cameraPos = ToSL(InArguments.CameraOrigin);
	StreamlineConstants.cameraUp = ToSL(InArguments.CameraUp);
	StreamlineConstants.cameraRight = ToSL(InArguments.CameraRight);
	StreamlineConstants.cameraFwd = ToSL(InArguments.CameraForward);
	
	StreamlineConstants.cameraNear = InArguments.CameraNear;
	StreamlineConstants.cameraFar = InArguments.CameraFar;
	StreamlineConstants.cameraFOV = FMath::DegreesToRadians(InArguments.CameraFOV);
	StreamlineConstants.cameraAspectRatio = InArguments.CameraAspectRatio;

	StreamlineConstants.cameraPinholeOffset = ToSL(InArguments.CameraPinholeOffset);

	SLsetConstants(StreamlineConstants, *GetFrameToken(InArguments.FrameId), sl::ViewportHandle(InArguments.ViewId));

	TagTexture(CmdList, InArguments.InputDepth, EStreamlineResource::Depth, InArguments.ViewId);
	TagTexture(CmdList, InArguments.InputMotionVectors, EStreamlineResource::MotionVectors, InArguments.ViewId);
	
	TagTexture(CmdList, InArguments.InputHUDLessColor, EStreamlineResource::HUDLessColor, InArguments.ViewId);
}

void FStreamlineRHI::TagNullTexture(EStreamlineResource InResourceTag, uint32 InViewID)
{
	sl::Resource SLResource;
	FMemory::Memzero(SLResource);

	SLResource.native = nullptr;
	SLResource.type = sl::ResourceType::eCount;
	SLResource.state = 0;
	sl::ResourceTag Tag;
	Tag.resource = &SLResource;
	Tag.type = ToSL(InResourceTag);
	// TODO: sl::ResourceLifecycle::eValidUntilPreset would be more efficient, are there any textures where it's applicable?
	Tag.lifecycle = sl::ResourceLifecycle::eOnlyValidNow;
	Tag.extent = ToSL(FIntRect());

	SLsetTag(sl::ViewportHandle(InViewID), &Tag, 1, nullptr);
}

sl::FrameToken* FStreamlineRHI::GetFrameToken(uint64 FrameCounter)
{
	if (!FrameTokenProvider.IsValid())
	{
		return nullptr;
	}
	return FrameTokenProvider->GetTokenForFrame(FrameCounter);
}

FStreamlineRHI::~FStreamlineRHI()
{
	UE_LOG(LogStreamlineRHI, Log, TEXT("%s Enter"), ANSI_TO_TCHAR(__FUNCTION__));
#if WITH_EDITOR
	if (BeginPIEHandle.IsValid())
	{
		FEditorDelegates::BeginPIE.Remove(BeginPIEHandle);
	}
	if (EndPIEHandle.IsValid())
	{
		FEditorDelegates::EndPIE.Remove(EndPIEHandle);
	}
#endif
	UE_LOG(LogStreamlineRHI, Log, TEXT("%s Leave"), ANSI_TO_TCHAR(__FUNCTION__));
}

#if PLATFORM_WINDOWS
THIRD_PARTY_INCLUDES_START
#include <winerror.h>
THIRD_PARTY_INCLUDES_END
bool FStreamlineRHI::IsDXGIStatus(const HRESULT HR)
{
	switch (HR)
	{
	default: return false;

	case DXGI_STATUS_OCCLUDED: return true;
	case DXGI_STATUS_CLIPPED: return true;
	case DXGI_STATUS_NO_REDIRECTION: return true;
	case DXGI_STATUS_NO_DESKTOP_ACCESS: return true;
	case DXGI_STATUS_GRAPHICS_VIDPN_SOURCE_IN_USE: return true;
	case DXGI_STATUS_MODE_CHANGED: return true;
	case DXGI_STATUS_MODE_CHANGE_IN_PROGRESS: return true;
	}
}
#endif


static TUniquePtr<FStreamlineRHI> GStreamlineRHI;
static EStreamlineSupport GStreamlineSupport = EStreamlineSupport::NotSupported;


static const sl::FeatureRequirementFlags GImplementedStreamlineRHIs =
#if PLATFORM_WINDOWS
sl::FeatureRequirementFlags(uint32_t(sl::FeatureRequirementFlags::eD3D11Supported) | uint32_t(sl::FeatureRequirementFlags::eD3D12Supported));
#else 
sl::FeatureRequirementFlags(0);
#endif

STREAMLINERHI_API sl::FeatureRequirementFlags PlatformGetAllImplementedStreamlineRHIs()
{
	return  GImplementedStreamlineRHIs;
}

STREAMLINERHI_API void PlatformCreateStreamlineRHI()
{
	UE_LOG(LogStreamlineRHI, Log, TEXT("%s Enter"), ANSI_TO_TCHAR(__FUNCTION__));

	// TODO catch init order issues
	check(!GStreamlineRHI);

	const FString RHIName = GDynamicRHI->GetName();

	UE_LOG(LogStreamlineRHI, Log, TEXT("GDynamicRHIName %s %s"), RHIVendorIdToString(), *RHIName);

	{

		// make sure that GImplementedStreamlineRHIs matches what we actually have implemented
		static_assert(sl::FeatureRequirementFlags::eD3D11Supported  == SLBitwiseAnd(sl::FeatureRequirementFlags::eD3D11Supported, GImplementedStreamlineRHIs), "Streamline API/RHI support mismatch");
		static_assert(sl::FeatureRequirementFlags::eD3D12Supported  == SLBitwiseAnd(sl::FeatureRequirementFlags::eD3D12Supported, GImplementedStreamlineRHIs), "Streamline API/RHI support mismatch");
		static_assert(sl::FeatureRequirementFlags::eVulkanSupported != SLBitwiseAnd(sl::FeatureRequirementFlags::eVulkanSupported, GImplementedStreamlineRHIs), "Streamline API/RHI support mismatch");
		
		const bool bIsDX12 = RHIName == TEXT("D3D12");
		const bool bIsDX11 = RHIName == TEXT("D3D11");
		const TCHAR* StreamlineRHIModuleName = nullptr;

		GStreamlineSupport = (bIsDX11 || bIsDX12) ? EStreamlineSupport::Supported : EStreamlineSupport::NotSupportedIncompatibleRHI;

		if (GStreamlineSupport == EStreamlineSupport::Supported)
		{
			if (bIsDX11)
			{
				StreamlineRHIModuleName = TEXT("StreamlineD3D11RHI");
			}
			else if (bIsDX12)
			{
				StreamlineRHIModuleName = TEXT("StreamlineD3D12RHI");
			}

			IStreamlineRHIModule* StreamlineRHIModule = &FModuleManager::LoadModuleChecked<IStreamlineRHIModule>(StreamlineRHIModuleName);

			// now that the RHI-specific SL module has been loaded, we have enough information to determine if SL is supported
			// TODO: better practice might be to make it a method on the module instead of a bare function
			if (IsStreamlineSupported())
			{
				// Get the base directory of this plugin
				const FString PluginBaseDir = IPluginManager::Get().FindPlugin(TEXT("Streamline"))->GetBaseDir();
				const FString SLBinariesDir = FPaths::Combine(*PluginBaseDir, TEXT("Binaries/ThirdParty/Win64/"));
				UE_LOG(LogStreamlineRHI, Log, TEXT("PluginBaseDir %s"), *PluginBaseDir);
				UE_LOG(LogStreamlineRHI, Log, TEXT("SLBinariesDir %s"), *SLBinariesDir);

				FStreamlineRHICreateArguments Arguments;
				Arguments.PluginBaseDir = PluginBaseDir;
				Arguments.DynamicRHI = GDynamicRHI;
				GStreamlineRHI = StreamlineRHIModule->CreateStreamlineRHI(Arguments);

				// TODO: handle renderdoc
				const bool bRenderDocPluginFound = FModuleManager::Get().ModuleExists(TEXT("RenderDocPlugin"));

				if (GStreamlineRHI && GStreamlineRHI->IsStreamlineAvailable())
				{
					GStreamlineSupport = EStreamlineSupport::Supported;
					UE_LOG(LogStreamlineRHI, Log, TEXT("Streamline supported by the %s %s RHI in the %s module at runtime"), RHIVendorIdToString(), *RHIName, StreamlineRHIModuleName);
				}
				else
				{
					UE_LOG(LogStreamlineRHI, Log, TEXT("Could not load %s module"), StreamlineRHIModuleName);
					GStreamlineSupport = EStreamlineSupport::NotSupported;
				}
			}
			else
			{
				UE_LOG(LogStreamlineRHI, Log, TEXT("Streamline not supported for the %s RHI"), *RHIName);
				GStreamlineSupport = EStreamlineSupport::NotSupported;
			}
		}
		else
		{
			UE_LOG(LogStreamlineRHI, Log, TEXT("Streamline not implemented for the %s RHI"), *RHIName);
		}
	}
	UE_LOG(LogStreamlineRHI, Log, TEXT("%s Leave"), ANSI_TO_TCHAR(__FUNCTION__));
}

STREAMLINERHI_API FStreamlineRHI* GetPlatformStreamlineRHI()
{
	return GStreamlineRHI.Get();
}

STREAMLINERHI_API EStreamlineSupport GetPlatformStreamlineSupport()
{
	return GStreamlineSupport;
}

static bool ShouldLoadDebugOverlay()
{
#if UE_BUILD_SHIPPING
	return false;
#else
	const TCHAR* StreamlineIniSection = TEXT("/Script/StreamlineRHI.StreamlineSettings");
	const TCHAR* StreamlineOverrideIniSection = TEXT("/Script/StreamlineRHI.StreamlineOverrideSettings");
	bool bLoadDebugOverlay = false;
	check(GConfig != nullptr);
	GConfig->GetBool(StreamlineIniSection, TEXT("bLoadDebugOverlay"), bLoadDebugOverlay, GEngineIni);
	FString LoadDebugOverlayOverrideString{};
	bool bOverrideFound = GConfig->GetString(StreamlineOverrideIniSection, TEXT("LoadDebugOverlayOverride"), LoadDebugOverlayOverrideString, GEngineIni);
	if (bOverrideFound)
	{
		if (LoadDebugOverlayOverrideString == TEXT("Enabled"))
		{
			bLoadDebugOverlay = true;
		}
		else if (LoadDebugOverlayOverrideString == TEXT("Disabled"))
		{
			bLoadDebugOverlay = false;
		}
	}

	if (FParse::Param(FCommandLine::Get(), TEXT("sldebugoverlay")))
	{
		UE_LOG(LogStreamlineRHI, Log, TEXT("Loading Streamline debug overlay (sl.imgui) due to -sldebugoverlay command line, which has priority over the config file setting of %u. This overrides the SL binaries to use SL development binaries."), bLoadDebugOverlay);
		bLoadDebugOverlay = true;
	}
	else if (FParse::Param(FCommandLine::Get(), TEXT("slnodebugoverlay")))
	{
		UE_LOG(LogStreamlineRHI, Log, TEXT("Not loading Streamline debug overlay (sl.imgui) due to -slnodebugoverlay command line, which has priority over the config file setting of %u"), bLoadDebugOverlay);
		bLoadDebugOverlay = false;
	}

	return bLoadDebugOverlay;
#endif
}

void FStreamlineRHIModule::InitializeStreamline()
{
	TArray<FString> StreamlineDLLSearchPaths;

	StreamlineDLLSearchPaths.Append({ StreamlineBinaryDirectory });

	TSharedPtr<IPlugin> DLSSPlugin = IPluginManager::Get().FindPlugin(TEXT("DLSS"));
#if (ENGINE_MAJOR_VERSION == 4) && (ENGINE_MINOR_VERSION == 25)
	const bool bIsDLSSPluginEnabled = true;
#else
	const bool bIsDLSSPluginEnabled = DLSSPlugin && (DLSSPlugin->IsEnabled() || DLSSPlugin->IsEnabledByDefault(false));
#endif
	if (bIsDLSSPluginEnabled)
	{
		// NGX will get initialized by Streamline below, long before the DLSS-SR plugin tries to initialize NGX in PostEngineInit.
		// We have to add the DLSS-SR plugin's binaries to the NGX search path now, to avoid breaking DLSS-SR
		UE_LOG(LogStreamlineRHI, Log, TEXT("DLSS plugin enabled, adding DLSS plugin binary search paths to Streamline init paths"));

		// TODO STREAMLINE have this respect r.NGX.BinarySearchOrder
		// this is a stripped down variant from the logic  NGXRHI::NGXRHI
		const FString ProjectNGXBinariesDir = FPaths::Combine(FPaths::ProjectDir(), TEXT("Binaries/ThirdParty/NVIDIA/NGX/Win64/"));
		const FString LaunchNGXBinariesDir = FPaths::Combine(FPaths::LaunchDir(), TEXT("Binaries/ThirdParty/NVIDIA/NGX/Win64/"));
#if (ENGINE_MAJOR_VERSION == 4) && (ENGINE_MINOR_VERSION == 25)
		const FString EngineNGXBinariesDir = FPaths::Combine(FPaths::EngineDir(), TEXT("Binaries/ThirdParty/NVIDIA/NGX/Win64/"));
		StreamlineDLLSearchPaths.Append({ ProjectNGXBinariesDir, LaunchNGXBinariesDir, EngineNGXBinariesDir});
#else
		const FString DLSSPluginBaseDir = DLSSPlugin->GetBaseDir();
		const FString PluginNGXProductionBinariesDir = FPaths::Combine(*DLSSPluginBaseDir, TEXT("Binaries/ThirdParty/Win64/"));
		StreamlineDLLSearchPaths.Append({ ProjectNGXBinariesDir, LaunchNGXBinariesDir, PluginNGXProductionBinariesDir });
#endif
	}
	else
	{
		UE_LOG(LogStreamlineRHI, Log, TEXT("DLSS plugin not enabled "));
	}

	TArray<const wchar_t*> StreamlineDLLSearchPathRawStrings;

	for (int32 i = 0; i < StreamlineDLLSearchPaths.Num(); ++i)
	{
		StreamlineDLLSearchPaths[i] = FPaths::ConvertRelativePathToFull(StreamlineDLLSearchPaths[i]);
		FPaths::RemoveDuplicateSlashes(StreamlineDLLSearchPaths[i]);
		FPaths::MakePlatformFilename(StreamlineDLLSearchPaths[i]);
		FPaths::NormalizeDirectoryName(StreamlineDLLSearchPaths[i]);
		// After this we should not touch StreamlineDLLSearchPaths since that provides the backing store for StreamlineDLLSearchPathRawStrings
		StreamlineDLLSearchPathRawStrings.Add(*StreamlineDLLSearchPaths[i]);
		const bool bHasStreamlineInterposerBinary = IPlatformFile::GetPlatformPhysical().FileExists(*FPaths::Combine(StreamlineDLLSearchPaths[i], STREAMLINE_INTERPOSER_BINARY_NAME));
		UE_LOG(LogStreamlineRHI, Log, TEXT("NVIDIA Streamline interposer plugin %s %s in search path %s"), STREAMLINE_INTERPOSER_BINARY_NAME, bHasStreamlineInterposerBinary ? TEXT("found") : TEXT("not found"), *StreamlineDLLSearchPaths[i]);

		// copied binary name here from the DLSS-SR plugin to avoid creating a dependency on that plugin
#ifndef NGX_DLSS_BINARY_NAME
#define NGX_DLSS_BINARY_NAME (TEXT("nvngx_dlss.dll"))
#endif

#ifdef NGX_DLSS_BINARY_NAME
		if (bIsDLSSPluginEnabled)
		{
			const bool bHasDLSSBinary = IPlatformFile::GetPlatformPhysical().FileExists(*FPaths::Combine(StreamlineDLLSearchPaths[i], NGX_DLSS_BINARY_NAME));
			UE_LOG(LogStreamlineRHI, Log, TEXT("NVIDIA NGX DLSS binary %s %s in search path %s"), NGX_DLSS_BINARY_NAME, bHasDLSSBinary ? TEXT("found") : TEXT("not found"), *StreamlineDLLSearchPaths[i]);
		}
#endif
	}

	sl::Preferences Preferences;
	FMemory::Memzero(Preferences);

	Preferences.showConsole = false;
	Preferences.logLevel = sl::LogLevel::eDefault;
	// we cannot use cvars since they haven't been loaded yet this early in the module loading order...
	{
		FString LogArgumentString;
		if (FParse::Value(FCommandLine::Get(), TEXT("slloglevel="), LogArgumentString))
		{
			if (LogArgumentString == TEXT("0"))
			{
				Preferences.logLevel = sl::LogLevel::eOff;
			}
			else if (LogArgumentString == TEXT("1"))
			{
				Preferences.logLevel = sl::LogLevel::eDefault;
			}
			else if (LogArgumentString == TEXT("2"))
			{
				Preferences.logLevel = sl::LogLevel::eVerbose;
			}
			else if (LogArgumentString == TEXT("3"))
			{
				Preferences.logLevel = sl::LogLevel::eVerbose;
				SetStreamlineAPILoggingEnabled(true);
			}
		}

		if (FParse::Value(FCommandLine::Get(), TEXT("sllogconsole="), LogArgumentString))
		{
			if (LogArgumentString == TEXT("0"))
			{
				Preferences.showConsole = false;
			}
			else if (LogArgumentString == TEXT("1"))
			{
				Preferences.showConsole = true;
			}
		}
	}

	Preferences.pathsToPlugins = &StreamlineDLLSearchPathRawStrings[0];
	Preferences.numPathsToPlugins = StreamlineDLLSearchPathRawStrings.Num();

	// TODO: consider filling these in too
	Preferences.pathToLogsAndData = nullptr;
	Preferences.allocateCallback = nullptr;
	Preferences.releaseCallback = nullptr;

	Preferences.logMessageCallback = StreamlineLogSink;

	Preferences.flags = sl::PreferenceFlags::eDisableCLStateTracking | sl::PreferenceFlags::eUseManualHooking;

	Preferences.engine = sl::EngineType::eUnreal;
	FString EngineVersion = FString::Printf(TEXT("%u.%u"), FEngineVersion::Current().GetMajor(), FEngineVersion::Current().GetMinor());
	FTCHARToUTF8 EngineVersionUTF8(*EngineVersion);
	Preferences.engineVersion = EngineVersionUTF8.Get();

	check(GConfig);
	FString ProjectID(TEXT("0"));
	GConfig->GetString(TEXT("/Script/EngineSettings.GeneralProjectSettings"), TEXT("ProjectID"), ProjectID, GGameIni);
	FTCHARToUTF8 ProjectIDUTF8(*ProjectID);
	Preferences.projectId = ProjectIDUTF8.Get();

	Preferences.applicationId = GetNGXAppID(bIsDLSSPluginEnabled);

	TArray<sl::Feature> Features = { sl::kFeatureReflex };
	
	bool bAllowDLSSG = true;
	
	if (FParse::Param(FCommandLine::Get(), TEXT("sldlssg")))
	{
		UE_LOG(LogStreamlineRHI, Log, TEXT("Loading Streamline DLSS-FG due to -sldlssg command line option"));
		bAllowDLSSG = true;
	}
	else if (FParse::Param(FCommandLine::Get(), TEXT("slnodlssg")))
	{
		UE_LOG(LogStreamlineRHI, Log, TEXT("Not loading Streamline DLSS-FG due to -slnodlssg command line option"));
		bAllowDLSSG = false;
	}

	if (bAllowDLSSG)
	{
		Features.Push(sl::kFeatureDLSS_G);
	}
#if !UE_BUILD_SHIPPING
	if ( ShouldLoadDebugOverlay())
	{
		Features.Push(sl::kFeatureImGUI);
	}
#endif
	Preferences.featuresToLoad = Features.GetData();
	Preferences.numFeaturesToLoad = Features.Num();

	const TCHAR* StreamlineIniSection = TEXT("/Script/StreamlineRHI.StreamlineSettings");
	bool bEnableStreamlineD3D11 = true;
	bool bEnableStreamlineD3D12 = true;
	GConfig->GetBool(StreamlineIniSection, TEXT("bEnableStreamlineD3D11"), bEnableStreamlineD3D11, GEngineIni);
	GConfig->GetBool(StreamlineIniSection, TEXT("bEnableStreamlineD3D12"), bEnableStreamlineD3D12, GEngineIni);

	const FString RHIName = GDynamicRHI->GetName();
	if (bEnableStreamlineD3D12 && RHIName == TEXT("D3D12"))
	{
		Preferences.renderAPI = sl::RenderAPI::eD3D12;
	}
	else if (bEnableStreamlineD3D11 && RHIName == TEXT("D3D11"))
	{
		Preferences.renderAPI = sl::RenderAPI::eD3D11;
	}
	else
	{
		UE_LOG(LogStreamlineRHI, Warning, TEXT("Unsupported RHI %s, skipping Streamline init"), *RHIName);
		return;
	}

	bool bAllowOTAUpdate = true;
	GConfig->GetBool(StreamlineIniSection, TEXT("bAllowOTAUpdate"), bAllowOTAUpdate, GEngineIni);
	if (bAllowOTAUpdate)
	{
		Preferences.flags |= (sl::PreferenceFlags::eAllowOTA | sl::PreferenceFlags::eLoadDownloadedPlugins);
	}

	UE_LOG(LogStreamlineRHI, Log, TEXT("Initializing Streamline"));
	UE_LOG(LogStreamlineRHI, Log, TEXT("sl::Preferences::logLevel    = %u. Can be overridden via -slloglevel={0,1,2} command line switches"), Preferences.logLevel);
	UE_LOG(LogStreamlineRHI, Log, TEXT("sl::Preferences::showConsole = %u. Can be overridden via -sllogconsole={0,1} command line switches"), Preferences.showConsole);
	UE_LOG(LogStreamlineRHI, Log, TEXT("sl::Preferences::featuresToLoad = {%s}. Feature loading can be overridden on the command line with -sl{no}dlssg and and -sl{no}debugoverlay (non-shipping)" ),
		*FString::JoinBy(Features, TEXT(", "), [](const sl::Feature& Feature) { return FString::Printf(TEXT("%s (%u)"), ANSI_TO_TCHAR(sl::getFeatureAsStr(Feature)), Feature); }));

	sl::Result Result = SLinit(Preferences);
	if (Result == sl::Result::eOk)
	{
		bIsStreamlineInitialized = true;
	}
	else
	{
		UE_LOG(LogStreamlineRHI, Error, TEXT("Failed to initialize Streamline (%d, %s)"), Result, ANSI_TO_TCHAR(sl::getResultAsStr(Result)));
		bIsStreamlineInitialized = false;
	}
}

STREAMLINERHI_API bool IsStreamlineSupported()
{
	return bIsStreamlineInitialized && AreStreamlineFunctionsLoaded();
}

STREAMLINERHI_API void FStreamlineRHIModule::ShutdownStreamline()
{
	UE_LOG(LogStreamlineRHI, Log, TEXT("Shutting down Streamline"));
	sl::Result Result = SLshutdown();
	if (Result != sl::Result::eOk)
	{
		UE_LOG(LogStreamlineRHI, Error, TEXT("Failed to shut down Streamline (%s)"), ANSI_TO_TCHAR(sl::getResultAsStr(Result)));
	}
	bIsStreamlineInitialized = false;
}


/** IModuleInterface implementation */

void FStreamlineRHIModule::StartupModule()
{
	auto CVarInitializePlugin = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Streamline.InitializePlugin"));
	if (CVarInitializePlugin && !CVarInitializePlugin->GetBool())
	{
		UE_LOG(LogStreamlineRHI, Log, TEXT("Initialization of StreamlineRHI is disabled."));
		return;
	}

	UE_LOG(LogStreamlineRHI, Log, TEXT("%s Enter"), ANSI_TO_TCHAR(__FUNCTION__));
	if (FApp::CanEverRender())
	{
		FString StreamlineBinaryFlavor{};

#if !(UE_BUILD_SHIPPING)
		{
			// debug overlay requires development binaries
			FString BinaryFlavorArgument = ShouldLoadDebugOverlay() ? TEXT("Development") : TEXT("");

			// optional command line override
			FParse::Value(FCommandLine::Get(), TEXT("slbinaries="), BinaryFlavorArgument);

			if (!BinaryFlavorArgument.IsEmpty())
			{
				for (auto Argument : { TEXT("Development"), TEXT("Debug") })
				{
					if (BinaryFlavorArgument.Compare(Argument, ESearchCase::IgnoreCase) == 0)
					{
						StreamlineBinaryFlavor = Argument;
						break;
					}
				}
				if (BinaryFlavorArgument.Compare(TEXT("Production"), ESearchCase::IgnoreCase) == 0)
				{
					// production binaries are not in a subdirectory
					StreamlineBinaryFlavor.Empty();
				}
			}
		}
#endif
		const FString StreamlinePluginBaseDir = IPluginManager::Get().FindPlugin(TEXT("Streamline"))->GetBaseDir();
		StreamlineBinaryDirectory = FPaths::Combine(*StreamlinePluginBaseDir, TEXT("Binaries/ThirdParty/Win64"),*(StreamlineBinaryFlavor ));
		UE_LOG(LogStreamlineRHI, Log, TEXT("Using Streamline %s binaries from %s. Can be overridden via -slbinaries={production,development,debug} command line switches for non-shipping builds")
			, StreamlineBinaryFlavor.IsEmpty() ? TEXT("production") : *StreamlineBinaryFlavor
			, *StreamlineBinaryDirectory
		);

		const FString StreamlineInterposerBinaryPath = FPaths::Combine(*StreamlineBinaryDirectory, STREAMLINE_INTERPOSER_BINARY_NAME);
		LoadStreamlineFunctionPointers(StreamlineInterposerBinaryPath);
	}
	else
	{
		UE_LOG(LogStreamlineRHI, Log, TEXT("This UE instance does not render, skipping loading of core Streamline functions"));
		StreamlineBinaryDirectory = TEXT("");
	}

	PlatformCreateStreamlineRHI();
	UE_LOG(LogStreamlineRHI, Log, TEXT("%s Leave"), ANSI_TO_TCHAR(__FUNCTION__));
}

void FStreamlineRHIModule::ShutdownModule()
{
	auto CVarInitializePlugin = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Streamline.InitializePlugin"));
	if (CVarInitializePlugin && !CVarInitializePlugin->GetBool())
	{
		return;
	}

	UE_LOG(LogStreamlineRHI, Log, TEXT("%s Enter"), ANSI_TO_TCHAR(__FUNCTION__));
	GStreamlineRHI.Reset();
	// TODO STREAMLINE sort out proper shutdown order between the SL interposer and the RHIs
	// don't shut down streamline so the D3D12RHI destructors don't crash
	//ShutdownStreamline();
	//FPlatformProcess::FreeDllHandle(SLInterPoserDLL);
	//SLInterPoserDLL = nullptr;
	UE_LOG(LogStreamlineRHI, Log, TEXT("%s Leave"), ANSI_TO_TCHAR(__FUNCTION__));
}


IMPLEMENT_MODULE(FStreamlineRHIModule, StreamlineRHI )
#undef LOCTEXT_NAMESPACE