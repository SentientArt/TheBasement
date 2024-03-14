/*
* Copyright (c) 2020 - 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
*
* NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
* property and proprietary rights in and to this material, related
* documentation and any modifications thereto. Any use, reproduction,
* disclosure or distribution of this material and related documentation
* without an express license agreement from NVIDIA CORPORATION or
* its affiliates is strictly prohibited.
*/
#include "DLSS.h"
#include "Misc/EngineVersion.h"
#include "CoreMinimal.h"
#include "DLSSUpscalerPrivate.h"

#include "DLSSUpscaler.h"
#include "DLSSDenoiser.h"

#include "NGXRHI.h"

#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#include "GeneralProjectSettings.h"
#include "DLSSSettings.h"

#include "Runtime/Launch/Resources/Version.h"
#include "SceneViewExtension.h"
#include "SceneView.h"
#include "Misc/MessageDialog.h"


#define LOCTEXT_NAMESPACE "FDLSSModule"
DEFINE_LOG_CATEGORY(LogDLSS);

static TAutoConsoleVariable<int32> CVarNGXBinarySearchOrder(
	TEXT("r.NGX.BinarySearchOrder"),
	0,
	TEXT("0: automatic: (default)\n")
	TEXT("   use custom binaries from project and launch folder $(ProjectDir)/Binaries/ThirdParty/NVIDIA/NGX/$(Platform) if present\n")
	TEXT("   fallback to generic binaries from plugin folder\n")
	TEXT("1: force generic binaries from plugin folder, fail if not found\n")
	TEXT("2: force custom binaries from project or launch folder, fail if not found\n")
	TEXT("3: force generic development binaries from plugin folder, fail if not found. This is only supported in non-shipping build configurations\n"),
		ECVF_ReadOnly);

static TAutoConsoleVariable<int32> CVarNGXEnable(
	TEXT("r.NGX.Enable"), 1,
	TEXT("Whether the NGX library should be loaded. This allow to have the DLSS plugin enabled but avoiding potential ")
	TEXT("incompatibilities by skipping the driver side NGX parts of DLSS. Can also be set on the command line via -ngxenable and -ngxdisable"),
	ECVF_ReadOnly);

static TAutoConsoleVariable<int32> CVarNGXEnableAllowCommandLine(
	TEXT("r.NGX.Enable.AllowCommandLine"), 0,
	TEXT("Whether to allow to override r.NGX.Enable with -ngxenable and -ngxdisable"),
	ECVF_ReadOnly);

static TAutoConsoleVariable<int32> CVarNGXDLSSMinimumWindowsBuildVersion(
	TEXT("r.NGX.DLSS.MinimumWindowsBuildVersion"), 16299,
	TEXT("Sets the minimum Windows 10 build version required to enable DLSS. (default: 16299 for v1709, Windows 10 Fall 2017 Creators Update 64-bit)"),
	ECVF_Default
);

static TAutoConsoleVariable<int32> CVarNGXProjectIdentifier(
	TEXT("r.NGX.ProjectIdentifier"),
	0,
	TEXT("0: automatic: (default)\n")
	TEXT("   use NVIDIA NGX Application ID if non-zero, otherwise use UE Project ID)\n")
	TEXT("1: force UE Project ID\n")
	TEXT("2: force NVIDIA NGX Application ID (set via the Project Settings -> NVIDIA DLSS plugin)\n"),
	ECVF_ReadOnly);


static TAutoConsoleVariable<bool> CVarNGXAutomationEnable(
	TEXT("r.NGX.Automation.Enable"),
	false,
	TEXT("Enable automation for NGX DLSS image quality and performance evaluation. (default = 0)\n"),
	ECVF_RenderThreadSafe);

static TAutoConsoleVariable<int> CVarNGXAutomationViewIndex(
	TEXT("r.NGX.Automation.ViewIndex"),
	0,
	TEXT("Select which view to use with NGX DLSS image quality and performance automation. (default = 0). \n"),
	ECVF_RenderThreadSafe);

static TAutoConsoleVariable<int> CVarNGXAutomationNonGameViews(
	TEXT("r.NGX.Automation.NonGameViews"),
	0,
	TEXT("Enable NGX DLSS image quality and performance automation for non-game views. (default = 0). \n"),
	ECVF_RenderThreadSafe);


class FNGXAutomationViewExtension final : public FSceneViewExtensionBase
{
public:
	FNGXAutomationViewExtension(const FAutoRegister& AutoRegister): FSceneViewExtensionBase(AutoRegister)	
	{
		FSceneViewExtensionIsActiveFunctor IsActiveFunctor;

		IsActiveFunctor.IsActiveFunction = [](const ISceneViewExtension* SceneViewExtension, const FSceneViewExtensionContext& Context)
		{
			return CVarNGXAutomationEnable.GetValueOnAnyThread();
		};

		IsActiveThisFrameFunctions.Add(IsActiveFunctor);
	}

	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) {}
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) {}
	virtual void SetupViewPoint(APlayerController* Player, FMinimalViewInfo& InViewInfo) {}
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) {}
	virtual void PreRenderView_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& InView) final {}
	virtual void PreRenderViewFamily_RenderThread(FRDGBuilder& GraphBuilder, FSceneViewFamily& InViewFamily) final
	{
		int32 ViewIndex = CVarNGXAutomationViewIndex.GetValueOnRenderThread();

		if (InViewFamily.Views.IsValidIndex(ViewIndex))
		{
			const FSceneView* View = InViewFamily.Views[ViewIndex];
			if (View->bIsGameView || CVarNGXAutomationNonGameViews.GetValueOnRenderThread())
			{
				const FString AiAgentMarker = FString::Printf(TEXT("{\"camera\":{\"position\":{\"x\": %f, \"y\": %f, \"z\": %f},\"rotation\":{\"pitch\": %f, \"roll\": %f, \"yaw\": %f}}}"),
					View->ViewLocation.X, View->ViewLocation.Y, View->ViewLocation.Z,
					View->ViewRotation.Pitch, View->ViewRotation.Roll, View->ViewRotation.Yaw);

				static FColor ColorMarker = FColor::FromHex("0xA1A5E87");
				FRHICommandListImmediate& RHICmdList = GraphBuilder.RHICmdList;
				RHICmdList.PushEvent(*AiAgentMarker, ColorMarker);
				RHICmdList.PopEvent();
			}
		}
	}
};

void FDLSSModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(LogDLSS, Log, TEXT("%s Enter"), ANSI_TO_TCHAR(__FUNCTION__));

	if (!GDynamicRHI)
	{
		UE_LOG(LogDLSS, Log, TEXT("NVIDIA NGX DLSS requires an RHI"));
		NGXSupport = ENGXSupport::NotSupported;
		return;
	}

	// Get the base directory of this plugin
	const FString PluginBaseDir = IPluginManager::Get().FindPlugin(TEXT("DLSS"))->GetBaseDir();
	const FString NGXBinariesDir = FPaths::Combine(*PluginBaseDir, TEXT("Binaries/ThirdParty/Win64/"));
	const FString RHIName = GDynamicRHI->GetName();

	UE_LOG(LogDLSS, Log, TEXT("PluginBaseDir %s"), *PluginBaseDir);
	UE_LOG(LogDLSS, Log, TEXT("NGXBinariesDir %s"), *NGXBinariesDir);
	UE_LOG(LogDLSS, Log, TEXT("GDynamicRHIName %s %s"), RHIVendorIdToString(), *RHIName );

	bool bLoadLibraries = CVarNGXEnable.GetValueOnAnyThread() != 0;
	if (CVarNGXEnableAllowCommandLine.GetValueOnAnyThread())
	{
		if (FParse::Param(FCommandLine::Get(), TEXT("ngxenable")))
		{
			bLoadLibraries = true;
		}
		else if (FParse::Param(FCommandLine::Get(), TEXT("ngxdisable")))
		{
			bLoadLibraries = false;
		}
	}

	const int32 NGXDLSSMinimumWindowsBuildVersion = CVarNGXDLSSMinimumWindowsBuildVersion.GetValueOnAnyThread();
	
	if (!IsRHIDeviceNVIDIA())
	{
		UE_LOG(LogDLSS, Log, TEXT("NVIDIA NGX DLSS requires an NVIDIA RTX series graphics card"));
		NGXSupport = ENGXSupport::NotSupportedIncompatibleHardware;
	}
#if PLATFORM_WINDOWS
	else if (NGXDLSSMinimumWindowsBuildVersion > 0 && !FPlatformMisc::VerifyWindowsVersion(10, 0
		#if PLATFORM_DESKTOP
		, NGXDLSSMinimumWindowsBuildVersion
		#endif
	))
	{
		// From https://docs.microsoft.com/en-us/windows/release-information/
		UE_LOG(LogDLSS, Log, TEXT("NVIDIA NGX DLSS requires at least Windows 10, build %u "), NGXDLSSMinimumWindowsBuildVersion);
		NGXSupport = ENGXSupport::NotSupportedOperatingSystemOutOfDate;
	}
#endif
	else if (!bLoadLibraries)
	{
		UE_LOG(LogDLSS, Log, TEXT("NVIDIA NGX library loading has been disabled with r.NGX.Enable=0"));
		NGXSupport = ENGXSupport::NotSupported;
	}
	else
	{
		const ERHIInterfaceType RHIType = RHIGetInterfaceType();

		const bool bIsDX12 = (RHIType == ERHIInterfaceType::D3D12) && GetDefault<UDLSSSettings>()->bEnableDLSSD3D12;
		const bool bIsDX11 = (RHIType == ERHIInterfaceType::D3D11) && GetDefault<UDLSSSettings>()->bEnableDLSSD3D11;
		const bool bIsVulkan = (RHIType == ERHIInterfaceType::Vulkan) && GetDefault<UDLSSSettings>()->bEnableDLSSVulkan;
		const TCHAR* NGXRHIModuleName = nullptr;

		NGXSupport = (bIsDX11 || bIsDX12 || bIsVulkan) ? ENGXSupport::Supported : ENGXSupport::NotSupported; 

		if (NGXSupport == ENGXSupport::Supported)
		{
			if (bIsDX11)
			{
				NGXRHIModuleName = TEXT("NGXD3D11RHI");
			}
			else if (bIsDX12)
			{
				NGXRHIModuleName = TEXT("NGXD3D12RHI");
			}
			else if (bIsVulkan)
			{
				NGXRHIModuleName = TEXT("NGXVulkanRHI");
			}


			uint32 NGXAppID = GetDefault<UDLSSSettings>()->NVIDIANGXApplicationId;
			UE_LOG(LogDLSS, Log, TEXT("Plugin settings: NGXAppId = %u"), NGXAppID);

			if (NGXAppID != 0)
			{
				UE_LOG(LogDLSS, Log, TEXT("NGX Application ID is specified, overriding the Project ID. Please refer to https://developer.nvidia.com/dlss."));
			}
			else
			{
				UE_LOG(LogDLSS, Log, TEXT("NGX Application ID not specified, using the Project ID by default."));
			}

			bool bAllowOTAUpdate = GetDefault<UDLSSSettings>()->bAllowOTAUpdate;
			if (FParse::Param(FCommandLine::Get(), TEXT("ngxenableota")))
			{
				bAllowOTAUpdate = true;
			}
			else if (FParse::Param(FCommandLine::Get(), TEXT("ngxdisableota")))
			{
				bAllowOTAUpdate = false;
			}

			FNGXRHICreateArguments Arguments;
			Arguments.PluginBaseDir = PluginBaseDir;
			Arguments.DynamicRHI = GDynamicRHI;
			Arguments.NGXBinariesSearchOrder = ENGXBinariesSearchOrder(FMath::Clamp(CVarNGXBinarySearchOrder.GetValueOnAnyThread(), int32(ENGXBinariesSearchOrder::MinValue), int32(ENGXBinariesSearchOrder::MaxValue)));

			Arguments.ProjectIdentifier = ENGXProjectIdentifier(FMath::Clamp(CVarNGXProjectIdentifier.GetValueOnAnyThread(), int32(ENGXProjectIdentifier::MinValue), int32(ENGXProjectIdentifier::MaxValue)));
			Arguments.NGXAppId = NGXAppID;
			Arguments.UnrealEngineVersion = FString::Printf(TEXT("%u.%u"), FEngineVersion::Current().GetMajor(), FEngineVersion::Current().GetMinor());
			Arguments.UnrealProjectID = GetDefault<UGeneralProjectSettings>()->ProjectID.ToString();
			Arguments.bAllowOTAUpdate = bAllowOTAUpdate;

			INGXRHIModule* NGXRHIModule = &FModuleManager::LoadModuleChecked<INGXRHIModule>(NGXRHIModuleName);
			NGXRHIExtensions = NGXRHIModule->CreateNGXRHI(Arguments);

			const bool bDLSSUnavailable = (!NGXRHIExtensions || !NGXRHIExtensions->IsDLSSAvailable());
			const bool bIncompatibleAPICaptureToolActive = NGXRHI::IsIncompatibleAPICaptureToolActive();
			if (bDLSSUnavailable && bIncompatibleAPICaptureToolActive)
			{
				UE_LOG(LogDLSS, Warning, TEXT("DLSS is not compatible with some API capture tools, such as RenderDoc. To enable DLSS please use an API capture tool such as NVIDIA NSIGHT graphics, which support the APIs required by DLSS."));

// we don't want this ever show up in packaged builds
#if WITH_EDITOR
				const bool IsUnattended =  FApp::IsUnattended() || IsRunningCommandlet() || GIsRunningUnattendedScript;
				UDLSSOverrideSettings* LocalSettings = GetMutableDefault<UDLSSOverrideSettings>();
				const UDLSSSettings* ProjectSettings = GetDefault<UDLSSSettings>();
				// historical note: This was originally only in the local settings, and it was a bool.
				// Then we added a project setting because some devs want to disable the popup project-wide.
				// But changing the type of the local setting to an enum would have complicated upgrades from previous plugin versions.
				// So that's why the current logic is: if either the project setting or the local setting is set to false, we disable the popup
				if (!IsUnattended && ProjectSettings->bShowDLSSIncompatiblePluginsToolsWarnings && LocalSettings->bShowDLSSIncompatiblePluginsToolsWarnings)
				{
					const FText DialogTitle(LOCTEXT("DLSSIncompatibleWithCaptureToolTitle", "Warning -- DLSS incompatible with plugins and/or API capture tools"));
					const FText WarningMessage(LOCTEXT("DLSSIncompatibleWithCaptureTool",
						"To enable DLSS, please disable the RenderDoc plugin and/or use an API capture tool such as NVIDIA NSIGHT graphics, which supports the APIs required by DLSS.\n\n"
						"Would you like to continue to see this message for this project? It can also be disabled in the NVIDIA DLSS Overrides (Local) plugin settings."));

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
					EAppReturnType::Type YesNoCancelReply = FMessageDialog::Open(EAppMsgType::YesNoCancel, EAppReturnType::Yes, WarningMessage, DialogTitle);
#else
					EAppReturnType::Type YesNoCancelReply = FMessageDialog::Open(EAppMsgType::YesNoCancel, EAppReturnType::Yes, WarningMessage, &DialogTitle);
#endif
				
					if(YesNoCancelReply == EAppReturnType::No)
					{
						LocalSettings->bShowDLSSIncompatiblePluginsToolsWarnings = false; 
					}
					else if (YesNoCancelReply == EAppReturnType::Cancel)
					{
						FPlatformMisc::RequestExit(true);
					}
				}
#endif //WITH_EDITOR
			}

			if (NGXRHIExtensions)
			{
				if (NGXRHIExtensions->IsDLSSAvailable())
				{
					check(NVSDK_NGX_SUCCEED(NGXRHIExtensions->GetNGXInitResult()));
					DLSSSRSupport = EDLSSSupport::Supported;
					// it's possible for DLSS-SR to be supported without DLSS-RR, but not vice-versa
					if (NGXRHIExtensions->IsDLSSRRAvailable())
					{
						DLSSRRSupport = EDLSSSupport::Supported;
					}
				}
				else
				{
					// map some of the NGX error codes to something that the UI/gameplay could suggest the end user to do something about
					if (bIncompatibleAPICaptureToolActive)
					{
						DLSSSRSupport = EDLSSSupport::NotSupportedIncompatibleAPICaptureToolActive;
						DLSSRRSupport = EDLSSSupport::NotSupportedIncompatibleAPICaptureToolActive;
					}
					else if (NVSDK_NGX_Result_FAIL_OutOfDate == NGXRHIExtensions->GetNGXInitResult())
					{
						DLSSSRSupport = EDLSSSupport::NotSupportedDriverOutOfDate;
						DLSSRRSupport = EDLSSSupport::NotSupportedDriverOutOfDate;
					} 
					else if (NVSDK_NGX_Result_FAIL_FeatureNotSupported == NGXRHIExtensions->GetNGXInitResult())
					{
						DLSSSRSupport = EDLSSSupport::NotSupportedIncompatibleHardware;
						DLSSRRSupport = EDLSSSupport::NotSupportedIncompatibleHardware;
					}
					else
					{
						DLSSSRSupport = EDLSSSupport::NotSupported;
						DLSSRRSupport = EDLSSSupport::NotSupported;
					}
				}

				const FNGXDriverRequirements DriverRequirementsNGX = NGXRHIExtensions->GetNGXDriverRequirements();
				MinSRDriverVersionMajor = DriverRequirementsNGX.MinDriverVersionMajor;
				MinSRDriverVersionMinor = DriverRequirementsNGX.MinDriverVersionMinor;
				if (DriverRequirementsNGX.DriverUpdateRequired)
				{
					if (NGXSupport == ENGXSupport::Supported)
					{
						UE_LOG(LogDLSS, Log, TEXT("NVIDIA NGX DLSS could be loaded properly. However it is recommended to update the version to at least: %u.%u"), DriverRequirementsNGX.MinDriverVersionMajor, DriverRequirementsNGX.MinDriverVersionMinor);
					}
					else
					{
						UE_LOG(LogDLSS, Log, TEXT("NVIDIA NGX DLSS cannot be loaded properly. Please verify that at least this driver version is installed: %u.%u"), DriverRequirementsNGX.MinDriverVersionMajor, DriverRequirementsNGX.MinDriverVersionMinor);
						DLSSSRSupport = EDLSSSupport::NotSupportedDriverOutOfDate;
						DLSSRRSupport = EDLSSSupport::NotSupportedDriverOutOfDate;
						MinSRDriverVersionMajor = DriverRequirementsNGX.MinDriverVersionMajor;
						MinSRDriverVersionMinor = DriverRequirementsNGX.MinDriverVersionMinor;
					}
				}
				// DLSS-RR may require more recent driver than NGX or DLSS-SR
				const FNGXDriverRequirements DriverRequirementsRR = NGXRHIExtensions->GetDLSSRRDriverRequirements();
				MinRRDriverVersionMajor = DriverRequirementsRR.MinDriverVersionMajor;
				MinRRDriverVersionMinor = DriverRequirementsRR.MinDriverVersionMinor;
				if (DriverRequirementsRR.DriverUpdateRequired)
				{
					UE_LOG(LogDLSS, Log, TEXT("NVIDIA DLSS Ray Reconstruction will require at least this driver version: %u.%u"), DriverRequirementsRR.MinDriverVersionMajor, DriverRequirementsRR.MinDriverVersionMinor);
					DLSSRRSupport = EDLSSSupport::NotSupportedDriverOutOfDate;
				}
			}
			else
			{
				UE_LOG(LogDLSS, Log, TEXT("Could not load %s module"), NGXRHIModuleName);
				NGXSupport = ENGXSupport::NotSupported;
			}

			if (NGXSupport != ENGXSupport::Supported)
			{
				UE_LOG(LogDLSS, Log, TEXT("DLSS not supported by the %s %s RHI in the %s module at runtime"), RHIVendorIdToString(), *RHIName, NGXRHIModuleName);
			}
		}
		else
		{
			UE_LOG(LogDLSS, Log, TEXT("DLSS not implemented for the %s RHI, or disabled in the project settings"), *RHIName);
			NGXSupport = ENGXSupport::NotSupported;
		}
	}

	
	if (DLSSSRSupport == EDLSSSupport::Supported)
	{
		// set the upscaler
		DLSSUpscaler.Reset(new FDLSSUpscaler(NGXRHIExtensions.Get()));
		check(DLSSUpscaler);
		if (DLSSUpscaler->GetNumRuntimeQualityModes() == 0)
		{
			UE_LOG(LogDLSS, Log, TEXT("DLSS not available due to not supporting any quality modes at runtime"));
			DLSSSRSupport = EDLSSSupport::NotSupported;
			DLSSRRSupport = EDLSSSupport::NotSupported;
			DLSSUpscaler.Reset();
			NGXRHIExtensions.Reset();
		}
	}

	UE_LOG(LogDLSS, Log, TEXT("NVIDIA NGX DLSS supported %u"), QueryDLSSSRSupport() == EDLSSSupport::Supported);

	// and the other related interfaces

	if (DLSSSRSupport == EDLSSSupport::Supported)
	{
		//	set up the view extension for setting up the FDLSSUpscaler on FSceneViewFamily
		{
			DLSSUpscalerViewExtension = FSceneViewExtensions::NewExtension<FDLSSUpscalerViewExtension>();
		}

		// set the denoiser
		{
			static const auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Reflections.Denoiser"));
			if (CVar && (CVar->GetInt() != 2))
			{
				UE_LOG(LogDLSS, Warning, TEXT("For optimal results NVIDIA NGX DLSS-SR requires the use of a custom denoiser plugin, which currently is disabled. To enable, set r.Reflections.Denoiser=2"));
			}

			DLSSDenoiser.Reset(new FDLSSDenoiser(GScreenSpaceDenoiser, DLSSUpscaler.Get()));
			GScreenSpaceDenoiser = DLSSDenoiser.Get();
			UE_LOG(LogDLSS, Log, TEXT("%s wrapping %s"), DLSSDenoiser->GetDebugName(), DLSSDenoiser->GetWrappedDenoiser()->GetDebugName());
		}

		// set the resource pool
		{
			checkf(GCustomResourcePool == nullptr, TEXT("GCustomResourcePool is already in use. Please check that only one upscaling plugin is active."));
			GCustomResourcePool = DLSSUpscaler.Get();
		}
	}
	
	// setup DLSS image quality and performance automation hooks
	{
		NGXAutomationViewExtension = FSceneViewExtensions::NewExtension<FNGXAutomationViewExtension>();
	}

	UE_LOG(LogDLSS, Log, TEXT("%s Leave"), ANSI_TO_TCHAR(__FUNCTION__));
}

void FDLSSModule::ShutdownModule()
{
	UE_LOG(LogDLSS, Log, TEXT("%s Enter"), ANSI_TO_TCHAR(__FUNCTION__));

	// reset DLSS image quality and performance automation hooks
	{
		NGXAutomationViewExtension = nullptr;
	}

	if (QueryDLSSSRSupport() == EDLSSSupport::Supported)
	{
		// reset the view extension
		{
			DLSSUpscalerViewExtension = nullptr;
		}

		// reset the resource pool
		if (GCustomResourcePool == DLSSUpscaler.Get())
		{
			GCustomResourcePool = nullptr;
		}

		// reset the denoiser
		if (GScreenSpaceDenoiser == DLSSDenoiser.Get())
		{
			UE_LOG(LogDLSS, Log, TEXT("%s unwrapping %s"), DLSSDenoiser->GetDebugName(), DLSSDenoiser->GetWrappedDenoiser()->GetDebugName());
			GScreenSpaceDenoiser = DLSSDenoiser->GetWrappedDenoiser();
			DLSSDenoiser.Reset();
		}

		// reset the upscaler
		{
			FDLSSUpscaler::ReleaseStaticResources();
			DLSSUpscaler.Reset();
		}

		NGXRHIExtensions.Reset();
	}

	UE_LOG(LogDLSS, Log, TEXT("%s Leave"), ANSI_TO_TCHAR(__FUNCTION__));
}


EDLSSSupport FDLSSModule::QueryDLSSSRSupport() const
{
	return DLSSSRSupport;
}


EDLSSSupport FDLSSModule::QueryDLSSRRSupport() const
{
	return DLSSRRSupport;
}


void FDLSSModule::GetDLSSSRMinDriverVersion(int32& OutMajorVersion, int32& OutMinorVersion) const
{
	OutMajorVersion = MinSRDriverVersionMajor;
	OutMinorVersion = MinSRDriverVersionMinor;
}


void FDLSSModule::GetDLSSRRMinDriverVersion(int32& OutMajorVersion, int32& OutMinorVersion) const
{
	OutMajorVersion = MinRRDriverVersionMajor;
	OutMinorVersion = MinRRDriverVersionMinor;
}


float FDLSSModule::GetResolutionFractionForQuality(int32 Quality) const
{
	checkf(QueryDLSSSRSupport() == EDLSSSupport::Supported,TEXT("GetResolutionFractionForQuality should only be called when DLSS is supported"));
	checkf(Quality >= int32(EDLSSQualityMode::MinValue) && Quality <= int32(EDLSSQualityMode::MaxValue),TEXT("The Quality %d  argument of GetResolutionFractionForQuality is out of range and/or an unsupported DLSS quality mode"), Quality);
	return DLSSUpscaler->GetOptimalResolutionFractionForQuality(EDLSSQualityMode(Quality));
}

FDLSSUpscaler* FDLSSModule::GetDLSSUpscaler() const
{
	return DLSSUpscaler.Get();
}

TSharedPtr< ISceneViewExtension, ESPMode::ThreadSafe> FDLSSModule::GetDLSSUpscalerViewExtension() const
{
	return StaticCastSharedPtr<ISceneViewExtension>(DLSSUpscalerViewExtension);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDLSSModule, DLSS)
	

