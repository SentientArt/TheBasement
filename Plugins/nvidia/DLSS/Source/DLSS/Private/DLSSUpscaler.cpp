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

#include "DLSSUpscaler.h"

#include "DLSS.h"
#include "DLSSSettings.h"
#include "DLSSUpscalerHistory.h"
#include "DLSSUpscalerPrivate.h"
#include "GBufferResolvePass.h"
#include "VelocityCombinePass.h"

#include "DynamicResolutionState.h"
#include "Engine/GameViewportClient.h"
#include "LegacyScreenPercentageDriver.h"
#include "PostProcess/PostProcessEyeAdaptation.h"
#include "Runtime/Launch/Resources/Version.h"
#include "ScenePrivate.h"
#include "SceneTextureParameters.h"


#define LOCTEXT_NAMESPACE "FDLSSModule"

static TAutoConsoleVariable<int32> CVarNGXDLSSEnable(
	TEXT("r.NGX.DLSS.Enable"), 1,
	TEXT("Enable/Disable DLSS entirely."),
	ECVF_RenderThreadSafe);

static TAutoConsoleVariable<int32> CVarNGXDLSSAutomationTesting(
	TEXT("r.NGX.DLSS.AutomationTesting"), 0,
	TEXT("Whether the NGX library should be loaded when GIsAutomationTesting is true.(default is false)\n")
	TEXT("Must be set to true before startup. This can be enabled for cases where running automation testing with DLSS desired"),
	ECVF_ReadOnly);

// corresponds to EDLSSPreset
static TAutoConsoleVariable<int32> CVarNGXDLSSPresetSetting(
	TEXT("r.NGX.DLSS.Preset"),
	0,
	TEXT("DLSS/DLAA preset setting. Allows selecting a different DL model than the default\n")
	TEXT("  0: Use default preset or ini value\n")
	TEXT("  1: Force preset A\n")
	TEXT("  2: Force preset B\n")
	TEXT("  3: Force preset C\n")
	TEXT("  4: Force preset D\n")
	TEXT("  5: Force preset E\n")
	TEXT("  6: Force preset F\n")
	TEXT("  7: Force preset G"),
	ECVF_RenderThreadSafe);


static TAutoConsoleVariable<float> CVarNGXDLSSSharpness(
	TEXT("r.NGX.DLSS.Sharpness"),
	0.0f,
	TEXT("[deprecated] -1.0 to 1.0: Softening/sharpening to apply to the DLSS pass. Negative values soften the image, positive values sharpen. (default: 0.0f)"),
	ECVF_RenderThreadSafe);

static TAutoConsoleVariable<int32> CVarNGXDLSSDilateMotionVectors(
	TEXT("r.NGX.DLSS.DilateMotionVectors"),
	1,
	TEXT(" 0: pass low resolution motion vectors into DLSS-SR\n")
	TEXT(" 1: pass dilated high resolution motion vectors into DLSS-SR. This can help with improving image quality of thin details. (default)"),
	ECVF_RenderThreadSafe);

static TAutoConsoleVariable<int32> CVarNGXDLSSAutoExposure(
	TEXT("r.NGX.DLSS.AutoExposure"), 1,
	TEXT("0: Use the engine-computed exposure value for input images to DLSS - in some cases this may reduce artifacts\n")
	TEXT("1: Enable DLSS internal auto-exposure instead of the application provided one (default)\n"),
	ECVF_RenderThreadSafe);

static TAutoConsoleVariable<int32> CVarNGXDLSSReleaseMemoryOnDelete(
	TEXT("r.NGX.DLSS.ReleaseMemoryOnDelete"), 
	1,
	TEXT("Enabling/disable releasing DLSS related memory on the NGX side when DLSS features get released.(default=1)"),
	ECVF_RenderThreadSafe);


static TAutoConsoleVariable<int32> CVarNGXDLSSFeatureCreationNode(
	TEXT("r.NGX.DLSS.FeatureCreationNode"), -1,
	TEXT("Determines which GPU the DLSS feature is getting created on\n")
	TEXT("-1: Create on the GPU the command list is getting executed on (default)\n")
	TEXT(" 0: Create on GPU node 0 \n")
	TEXT(" 1: Create on GPU node 1 \n"),
	ECVF_RenderThreadSafe);


static TAutoConsoleVariable<int32> CVarNGXDLSSFeatureVisibilityMask(
	TEXT("r.NGX.DLSS.FeatureVisibilityMask"), -1,
	TEXT("Determines which GPU the DLSS feature is visible to\n")
	TEXT("-1: Visible to the GPU the command list is getting executed on (default)\n")
	TEXT(" 1: visible to GPU node 0 \n")
	TEXT(" 2:  visible to GPU node 1 \n")
	TEXT(" 3:  visible to GPU node 0 and GPU node 1\n"),
	ECVF_RenderThreadSafe);


static TAutoConsoleVariable<int32> CVarNGXDLSSDenoiserMode(
	TEXT("r.NGX.DLSS.DenoiserMode"),
	0,
	TEXT("Configures how DLSS denoises\n")
	TEXT("0: off, no denoising (default)\n")
	TEXT("1: DLSS-RR enabled\n"),
	ECVF_RenderThreadSafe
);

DECLARE_GPU_STAT(DLSS)

static const float kDLSSResolutionFractionError = 0.01f;

BEGIN_SHADER_PARAMETER_STRUCT(FDLSSShaderParameters, )

// Input images
SHADER_PARAMETER_RDG_TEXTURE(Texture2D, SceneColorInput)
SHADER_PARAMETER_RDG_TEXTURE(Texture2D, SceneDepthInput)
SHADER_PARAMETER_RDG_TEXTURE(Texture2D, EyeAdaptation)
SHADER_PARAMETER_RDG_TEXTURE(Texture2D, SceneVelocityInput)

SHADER_PARAMETER_RDG_TEXTURE(Texture2D, DiffuseAlbedo)
SHADER_PARAMETER_RDG_TEXTURE(Texture2D, SpecularAlbedo)
SHADER_PARAMETER_RDG_TEXTURE(Texture2D, Normal)
SHADER_PARAMETER_RDG_TEXTURE(Texture2D, Roughness)

// Output images
RDG_TEXTURE_ACCESS(SceneColorOutput, ERHIAccess::UAVCompute)

END_SHADER_PARAMETER_STRUCT()

static FDLSSUpscaler* GetGlobalDLSSUpscaler()
{
	IDLSSModuleInterface* DLSSModule = &FModuleManager::LoadModuleChecked<IDLSSModuleInterface>("DLSS");
	check(DLSSModule);

	return DLSSModule->GetDLSSUpscaler();
}

FIntPoint FDLSSPassParameters::GetOutputExtent() const
{
	check(Validate());
	check(SceneColorInput);

	FIntPoint InputExtent = SceneColorInput->Desc.Extent;

	FIntPoint QuantizedPrimaryUpscaleViewSize;
	QuantizeSceneBufferSize(OutputViewRect.Size(), QuantizedPrimaryUpscaleViewSize);

	return FIntPoint(
		FMath::Max(InputExtent.X, QuantizedPrimaryUpscaleViewSize.X),
		FMath::Max(InputExtent.Y, QuantizedPrimaryUpscaleViewSize.Y));
}

bool FDLSSPassParameters::Validate() const
{
	checkf(OutputViewRect.Min == FIntPoint::ZeroValue,TEXT("The DLSS OutputViewRect %dx%d must be non-zero"), OutputViewRect.Min.X, OutputViewRect.Min.Y);
	return true;
}

static EDLSSPreset GetDLSSPresetFromCVarValue(int32 InCVarValue)
{
	if (InCVarValue >= 0 && InCVarValue <= static_cast<int32>(EDLSSPreset::F))
	{
		return static_cast<EDLSSPreset>(InCVarValue);
	}
	UE_LOG(LogDLSS, Warning, TEXT("Invalid r.NGX.DLSS.DLSSPreset value %d"), InCVarValue);
	return EDLSSPreset::Default;
}

static NVSDK_NGX_DLSS_Hint_Render_Preset ToNGXDLSSPreset(EDLSSPreset DLSSPreset)
{
	switch (DLSSPreset)
	{
		default:
			checkf(false, TEXT("ToNGXDLSSPreset should not be called with an out of range EDLSSPreset from the higher level code"));
		case EDLSSPreset::Default:
			return NVSDK_NGX_DLSS_Hint_Render_Preset_Default;

		case EDLSSPreset::A:
			return NVSDK_NGX_DLSS_Hint_Render_Preset_A;

		case EDLSSPreset::B:
			return NVSDK_NGX_DLSS_Hint_Render_Preset_B;

		case EDLSSPreset::C:
			return NVSDK_NGX_DLSS_Hint_Render_Preset_C;

		case EDLSSPreset::D:
			return NVSDK_NGX_DLSS_Hint_Render_Preset_D;

		case EDLSSPreset::E:
			return NVSDK_NGX_DLSS_Hint_Render_Preset_E;

		case EDLSSPreset::F:
			return NVSDK_NGX_DLSS_Hint_Render_Preset_F;
		
		case EDLSSPreset::G:
			return NVSDK_NGX_DLSS_Hint_Render_Preset_G;
	}
}

static NVSDK_NGX_DLSS_Hint_Render_Preset GetNGXDLSSPresetFromQualityMode(EDLSSQualityMode QualityMode)
{

	EDLSSPreset DLSSPreset = EDLSSPreset::Default;
	switch (QualityMode)
	{
		case EDLSSQualityMode::UltraPerformance:
			DLSSPreset = GetDefault<UDLSSSettings>()->DLSSUltraPerformancePreset;
			break;

		case EDLSSQualityMode::Performance:
			DLSSPreset = GetDefault<UDLSSSettings>()->DLSSPerformancePreset;
			break;

		case EDLSSQualityMode::Balanced:
			DLSSPreset = GetDefault<UDLSSSettings>()->DLSSBalancedPreset;
			break;

		case EDLSSQualityMode::Quality:
			DLSSPreset = GetDefault<UDLSSSettings>()->DLSSQualityPreset;
			break;

		case EDLSSQualityMode::UltraQuality:
			DLSSPreset = GetDefault<UDLSSSettings>()->DLSSUltraQualityPreset;
			break;

		case EDLSSQualityMode::DLAA:
			DLSSPreset = GetDefault<UDLSSSettings>()->DLAAPreset;
			break;

		default:
			checkf(false, TEXT("GetNGXDLSSPresetFromQualityMode called with an out of range EDLSSQualityMode"));
			break;
	}
	int32 DLSSPresetCVarVal = CVarNGXDLSSPresetSetting.GetValueOnAnyThread();
	if (DLSSPresetCVarVal != 0)
	{
		DLSSPreset = GetDLSSPresetFromCVarValue(DLSSPresetCVarVal);
	}
	return ToNGXDLSSPreset(DLSSPreset);
}

static NVSDK_NGX_PerfQuality_Value ToNGXQuality(EDLSSQualityMode Quality)
{
	static_assert(int32(EDLSSQualityMode::NumValues) == 6, "dear DLSS plugin NVIDIA developer, please update this code to handle the new EDLSSQualityMode enum values");
	switch (Quality)
	{
		case EDLSSQualityMode::UltraPerformance:
			return NVSDK_NGX_PerfQuality_Value_UltraPerformance;

		default:
			checkf(false, TEXT("ToNGXQuality should not be called with an out of range EDLSSQualityMode from the higher level code"));
		case EDLSSQualityMode::Performance:
			return NVSDK_NGX_PerfQuality_Value_MaxPerf;

		case EDLSSQualityMode::Balanced:
			return NVSDK_NGX_PerfQuality_Value_Balanced;

		case EDLSSQualityMode::Quality:
			return NVSDK_NGX_PerfQuality_Value_MaxQuality;
		
		case EDLSSQualityMode::UltraQuality:
			return NVSDK_NGX_PerfQuality_Value_UltraQuality;
		case EDLSSQualityMode::DLAA:
			return NVSDK_NGX_PerfQuality_Value_DLAA;

	}
}

NGXRHI* FDLSSUpscaler::NGXRHIExtensions;
float FDLSSUpscaler::MinDynamicResolutionFraction = TNumericLimits <float>::Max();
float FDLSSUpscaler::MaxDynamicResolutionFraction = TNumericLimits <float>::Min();
uint32 FDLSSUpscaler::NumRuntimeQualityModes = 0;
TArray<FDLSSOptimalSettings> FDLSSUpscaler::ResolutionSettings;

bool FDLSSUpscalerViewExtension::IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const
{
	// Verify this is for a viewport client
	if (Context.Viewport == nullptr || !GEngine)
	{
		return false;
	}

	// Do not setup temporal upscaler in automated tests.
	const bool bDLSSActiveWithAutomation = !GIsAutomationTesting || (GIsAutomationTesting && (CVarNGXDLSSAutomationTesting.GetValueOnAnyThread() != 0));
	if (!bDLSSActiveWithAutomation)
	{
		return false;
	}

	// Do not setup if not available.
	if (!GetGlobalDLSSUpscaler()->IsDLSSActive())
	{
		return false;
	}

	if (GIsEditor)
#if WITH_EDITOR
	{
		if (Context.Viewport->IsPlayInEditorViewport())
		{
			bool bEnableDLSSInPlayInEditorViewports = false;
			if (GetDefault<UDLSSOverrideSettings>()->EnableDLSSInPlayInEditorViewportsOverride == EDLSSSettingOverride::UseProjectSettings)
			{
				bEnableDLSSInPlayInEditorViewports = GetDefault<UDLSSSettings>()->bEnableDLSSInPlayInEditorViewports;
			}
			else
			{
				bEnableDLSSInPlayInEditorViewports = GetDefault<UDLSSOverrideSettings>()->EnableDLSSInPlayInEditorViewportsOverride == EDLSSSettingOverride::Enabled;
			}
#if !NO_LOGGING
			static bool bLoggedPIEWarning = false;
			if (!bLoggedPIEWarning && GIsPlayInEditorWorld && bEnableDLSSInPlayInEditorViewports)
			{
				if (FStaticResolutionFractionHeuristic::FUserSettings::EditorOverridePIESettings())
				{
					UE_LOG(LogDLSS, Warning, TEXT("r.ScreenPercentage for DLSS quality mode will be ignored because overridden by editor settings (r.Editor.Viewport.OverridePIEScreenPercentage). Change this behavior in Edit -> Editor Preferences -> Performance"));
					bLoggedPIEWarning = true;
				}
			}
#endif
			return GIsPlayInEditorWorld && bEnableDLSSInPlayInEditorViewports;
		}
		else
		{
			bool bEnableDLSSInEditorViewports = false;
			if (GetDefault<UDLSSOverrideSettings>()->EnableDLSSInEditorViewportsOverride == EDLSSSettingOverride::UseProjectSettings)
			{
				bEnableDLSSInEditorViewports = GetDefault<UDLSSSettings>()->bEnableDLSSInEditorViewports;
			}
			else
			{
				bEnableDLSSInEditorViewports = GetDefault<UDLSSOverrideSettings>()->EnableDLSSInEditorViewportsOverride == EDLSSSettingOverride::Enabled;
			}
			return bEnableDLSSInEditorViewports;
		}
	}
#else
	{
		return false;
	}
#endif
	else
	{
		const bool bIsGameViewport = Context.Viewport->GetClient() == GEngine->GameViewport;
		return bIsGameViewport;
	}
}

void FDLSSUpscalerViewExtension::BeginRenderViewFamily(FSceneViewFamily& ViewFamily)
{
	if (ViewFamily.ViewMode != EViewModeIndex::VMI_Lit ||
		ViewFamily.Scene == nullptr ||
		ViewFamily.Scene->GetShadingPath() != EShadingPath::Deferred ||
		!ViewFamily.bRealtimeUpdate)
	{
		return;
	}

	// Early returns if none of the view have a view state, if views are scene captures, or if primary temporal upscaling isn't requested
	bool bFoundPrimaryTemporalUpscale = false;
	for (const FSceneView* View : ViewFamily.Views)
	{
		if (View->State == nullptr)
		{
			return;
		}

		if (View->bIsSceneCapture)
		{
			return;
		}

		if (View->PrimaryScreenPercentageMethod == EPrimaryScreenPercentageMethod::TemporalUpscale)
		{
			bFoundPrimaryTemporalUpscale = true;
		}
	}
	if (!bFoundPrimaryTemporalUpscale)
	{
		return;
	}

	// Early returns if AA is disabled.
	if (!ViewFamily.EngineShowFlags.AntiAliasing)
	{
		return;
	}

	if (!ViewFamily.GetTemporalUpscalerInterface())
	{
		GetGlobalDLSSUpscaler()->SetupViewFamily(ViewFamily);
	}
	else
	{
		UE_LOG(LogDLSS, Error, TEXT("Another plugin already set FSceneViewFamily::SetTemporalUpscalerInterface()"));
		return;
	}
}

FDLSSUpscaler::FDLSSUpscaler(NGXRHI* InNGXRHIExtensions): PreviousResolutionFraction(-1.0f)
{
	UE_LOG(LogDLSS, VeryVerbose, TEXT("%s Enter"), ANSI_TO_TCHAR(__FUNCTION__));
	
	
	checkf(!NGXRHIExtensions, TEXT("static member NGXRHIExtensions should only be assigned once by this ctor when called during module startup") );
	NGXRHIExtensions = InNGXRHIExtensions;

	ResolutionSettings.Init(FDLSSOptimalSettings(), int32(EDLSSQualityMode::NumValues));

	static_assert(int32(EDLSSQualityMode::NumValues) == 6, "dear DLSS plugin NVIDIA developer, please update this code to handle the new EDLSSQualityMode enum values");
	for (auto QualityMode : { EDLSSQualityMode::UltraPerformance,  EDLSSQualityMode::Performance , EDLSSQualityMode::Balanced, EDLSSQualityMode::Quality, EDLSSQualityMode::UltraQuality,  EDLSSQualityMode::DLAA})
	{
		check(ToNGXQuality(QualityMode) < ResolutionSettings.Num());
		check(ToNGXQuality(QualityMode) >= 0);

		FDLSSOptimalSettings OptimalSettings = NGXRHIExtensions->GetDLSSOptimalSettings(ToNGXQuality(QualityMode));
		
		ResolutionSettings[ToNGXQuality(QualityMode)] = OptimalSettings;

		// we only consider non-fixed resolutions for the overall min / max resolution fraction
		if (OptimalSettings.bIsSupported && !OptimalSettings.IsFixedResolution())
		{
			MinDynamicResolutionFraction = FMath::Min(MinDynamicResolutionFraction, OptimalSettings.MinResolutionFraction);
			MaxDynamicResolutionFraction = FMath::Max(MaxDynamicResolutionFraction, OptimalSettings.MaxResolutionFraction);
		}
		if (OptimalSettings.bIsSupported)
		{
			++NumRuntimeQualityModes;
		}

		UE_LOG(LogDLSS, Log, TEXT("QualityMode %d: bSupported = %u, ResolutionFraction = %.4f. MinResolutionFraction=%.4f,  MaxResolutionFraction %.4f"),
			static_cast<int>(QualityMode), OptimalSettings.bIsSupported, OptimalSettings.OptimalResolutionFraction, OptimalSettings.MinResolutionFraction, OptimalSettings.MaxResolutionFraction);
	}

	// the DLSS module will report DLSS as not supported if there are no supported quality modes at runtime
	UE_LOG(LogDLSS, Log, TEXT("NumRuntimeQualityModes=%u, MinDynamicResolutionFraction=%.4f,  MaxDynamicResolutionFraction=%.4f"), NumRuntimeQualityModes, MinDynamicResolutionFraction, MaxDynamicResolutionFraction);

	// Higher levels of the code (e.g. UI) should check whether each mode is actually supported
	// But for now verify early that the DLSS 2.0 modes are supported. Those checks could be removed in the future
	check(IsQualityModeSupported(EDLSSQualityMode::Performance));
	check(IsQualityModeSupported(EDLSSQualityMode::Balanced));
	check(IsQualityModeSupported(EDLSSQualityMode::Quality));


	UE_LOG(LogDLSS, VeryVerbose, TEXT("%s Leave"), ANSI_TO_TCHAR(__FUNCTION__));
}

// this gets explicitly called during module shutdown
void FDLSSUpscaler::ReleaseStaticResources()
{
	UE_LOG(LogDLSS, VeryVerbose, TEXT("%s Enter"), ANSI_TO_TCHAR(__FUNCTION__));
	ResolutionSettings.Empty();
	UE_LOG(LogDLSS, VeryVerbose, TEXT("%s Leave"), ANSI_TO_TCHAR(__FUNCTION__));
}

static const TCHAR* const GDLSSSceneViewFamilyUpscalerDebugName = TEXT("FDLSSSceneViewFamilyUpscaler");

const TCHAR* FDLSSSceneViewFamilyUpscaler::GetDebugName() const
{
	return GDLSSSceneViewFamilyUpscalerDebugName;
}

// static
bool FDLSSSceneViewFamilyUpscaler::IsDLSSTemporalUpscaler(const ITemporalUpscaler* TemporalUpscaler)
{
	return TemporalUpscaler != nullptr && TemporalUpscaler->GetDebugName() == GDLSSSceneViewFamilyUpscalerDebugName;
}

float FDLSSSceneViewFamilyUpscaler::GetMinUpsampleResolutionFraction() const
{
	return Upscaler->GetMinResolutionFractionForQuality(DLSSQualityMode);
}

float FDLSSSceneViewFamilyUpscaler::GetMaxUpsampleResolutionFraction() const
{
	return Upscaler->GetMaxResolutionFractionForQuality(DLSSQualityMode);
}

ITemporalUpscaler* FDLSSSceneViewFamilyUpscaler::Fork_GameThread(const class FSceneViewFamily& ViewFamily) const
{
	return new FDLSSSceneViewFamilyUpscaler(Upscaler, DLSSQualityMode);
}

ITemporalUpscaler::FOutputs FDLSSSceneViewFamilyUpscaler::AddPasses(
	FRDGBuilder& GraphBuilder,
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
	const FSceneView& View,
	const ITemporalUpscaler::FInputs& PassInputs
#else
	const FViewInfo& View,
	const FPassInputs& PassInputs
#endif
) const
{
	checkf(View.PrimaryScreenPercentageMethod == EPrimaryScreenPercentageMethod::TemporalUpscale, TEXT("DLSS requires TemporalUpscale. If you hit this assert, please set r.TemporalAA.Upscale=1"));

	ITemporalUpscaler::FOutputs Outputs;
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
	const TRefCountPtr<ITemporalUpscaler::IHistory> InputCustomHistory = PassInputs.PrevHistory;
	TRefCountPtr<ITemporalUpscaler::IHistory>* OutputCustomHistory = &Outputs.NewHistory;

	FRDGTextureRef InputVelocity = PassInputs.SceneVelocity.Texture;
	FIntRect InputViewRect = PassInputs.SceneDepth.ViewRect;
	FDLSSPassParameters DLSSParameters(PassInputs);
#else
	const FTemporalAAHistory& InputHistory = View.PrevViewInfo.TemporalAAHistory;
	const TRefCountPtr<ICustomTemporalAAHistory> InputCustomHistory = View.PrevViewInfo.CustomTemporalAAHistory;

	FTemporalAAHistory* OutputHistory = View.ViewState ? &(View.ViewState->PrevFrameViewInfo.TemporalAAHistory) : nullptr;
	TRefCountPtr<ICustomTemporalAAHistory>* OutputCustomHistory = View.ViewState ? &(View.ViewState->PrevFrameViewInfo.CustomTemporalAAHistory) : nullptr;

	FRDGTextureRef InputVelocity = PassInputs.SceneVelocityTexture;
	FIntRect InputViewRect = View.ViewRect;
	FDLSSPassParameters DLSSParameters(View, PassInputs);
#endif
	{
		RDG_GPU_STAT_SCOPE(GraphBuilder, DLSS);
		RDG_EVENT_SCOPE(GraphBuilder, "DLSS");

		if (Upscaler->GetNGXRHI()->IsDLSSRRAvailable())
		{
			const ENGXDLSSDenoiserMode DenoiserMode = static_cast<ENGXDLSSDenoiserMode>(FMath::Clamp<int32>(CVarNGXDLSSDenoiserMode.GetValueOnRenderThread(), int32(ENGXDLSSDenoiserMode::Off), int32(ENGXDLSSDenoiserMode::MaxValue)));
			DLSSParameters.DenoiserMode = DenoiserMode;
		}
		else
		{
			DLSSParameters.DenoiserMode = ENGXDLSSDenoiserMode::Off;
		}

		bool bDilateMotionVectors = CVarNGXDLSSDilateMotionVectors.GetValueOnRenderThread() != 0;
		if (DLSSParameters.DenoiserMode == ENGXDLSSDenoiserMode::DLSSRR)
		{
			// DLSS-RR and mvec dilation can't be used together, DLSS-RR wins
			bDilateMotionVectors = false;
		}

		FRDGTextureRef CombinedVelocityTexture = AddVelocityCombinePass(
			GraphBuilder, View,
			DLSSParameters.SceneDepthInput,
			InputVelocity,
			InputViewRect,
			DLSSParameters.OutputViewRect,
			DLSSParameters.TemporalJitterPixels,
			bDilateMotionVectors);

		DLSSParameters.SceneVelocityInput = CombinedVelocityTexture;
		DLSSParameters.bHighResolutionMotionVectors = bDilateMotionVectors;

		if (DLSSParameters.DenoiserMode == ENGXDLSSDenoiserMode::DLSSRR)
		{
			FGBufferResolveOutputs ResolvedGBuffer = AddGBufferResolvePass(GraphBuilder, View, InputViewRect, true);

			DLSSParameters.SceneVelocityInput = CombinedVelocityTexture;

			DLSSParameters.DiffuseAlbedo = ResolvedGBuffer.DiffuseAlbedo;
			DLSSParameters.SpecularAlbedo = ResolvedGBuffer.SpecularAlbedo;

			DLSSParameters.Normal = ResolvedGBuffer.Normals;
			DLSSParameters.Roughness = ResolvedGBuffer.Roughness;
			DLSSParameters.SceneDepthInput = ResolvedGBuffer.LinearDepth;
		}

		const FDLSSOutputs DLSSOutputs = AddDLSSPass(
			GraphBuilder,
			View,
			DLSSParameters,
#if ENGINE_MINOR_VERSION <= 2
			InputHistory,
			OutputHistory,
#endif
			InputCustomHistory,
			OutputCustomHistory
		);

		Outputs.FullRes.Texture = DLSSOutputs.SceneColor;
		Outputs.FullRes.ViewRect = DLSSParameters.OutputViewRect;
	}
	return Outputs;
}

FDLSSOutputs FDLSSSceneViewFamilyUpscaler::AddDLSSPass(
	FRDGBuilder& GraphBuilder,
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
	const FSceneView& View,
	const FDLSSPassParameters& Inputs,
	const TRefCountPtr<ITemporalUpscaler::IHistory> InputCustomHistoryInterface,
	TRefCountPtr<ITemporalUpscaler::IHistory>* OutputCustomHistoryInterface
#else
	const FViewInfo& View,
	const FDLSSPassParameters& Inputs,
	const FTemporalAAHistory& InputHistory,
	FTemporalAAHistory* OutputHistory,
	const TRefCountPtr<ICustomTemporalAAHistory> InputCustomHistoryInterface,
	TRefCountPtr<ICustomTemporalAAHistory>* OutputCustomHistoryInterface
#endif
) const
{
	check(IsInRenderingThread());
	check(Upscaler->IsDLSSActive());
	const FDLSSUpscalerHistory* InputCustomHistory = static_cast<const FDLSSUpscalerHistory*>(InputCustomHistoryInterface.GetReference());

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
	// TODO: check if this camera cut logic is sufficient
	const bool bCameraCut = View.bCameraCut || !InputCustomHistoryInterface.IsValid();
#else
	const bool bCameraCut = !InputHistory.IsValid() || View.bCameraCut || !OutputHistory;
#endif
	const FIntPoint OutputExtent = Inputs.GetOutputExtent();

	const FIntRect SrcRect = Inputs.InputViewRect;
	const FIntRect DestRect = Inputs.OutputViewRect;

	const float ScaleX = float(SrcRect.Width()) / float(DestRect.Width());
	const float ScaleY = float(SrcRect.Height()) / float(DestRect.Height());

	// FDLSSUpscaler::SetupMainGameViewFamily or FDLSSUpscalerEditor::SetupEditorViewFamily 
	// set DLSSQualityMode by setting an FDLSSUpscaler on the ViewFamily (from the pool in DLSSUpscalerInstancesPerViewFamily)
	
	checkf(DLSSQualityMode != EDLSSQualityMode::NumValues, TEXT("Invalid Quality mode, not initialized"));
	checkf(Upscaler->IsQualityModeSupported(DLSSQualityMode), TEXT("%u is not a valid Quality mode"), DLSSQualityMode);

	// This assert can accidentally hit with small viewrect dimensions (e.g. when resizing an editor view) due to floating point rounding & quantization issues
	// e.g. with 33% screen percentage at 1000 DestRect dimension we get 333/1000 = 0.33 but at 10 DestRect dimension we get 3/10 0.3, thus the assert hits
	checkf(DestRect.Width()  < 100 || GetMinUpsampleResolutionFraction() - kDLSSResolutionFractionError <= ScaleX && ScaleX <= GetMaxUpsampleResolutionFraction() + kDLSSResolutionFractionError,
		TEXT("The current resolution fraction %f is out of the supported DLSS range [%f ... %f] for quality mode %d."),
		ScaleX, GetMinUpsampleResolutionFraction(), GetMaxUpsampleResolutionFraction(), DLSSQualityMode);
	checkf(DestRect.Height() < 100 || GetMinUpsampleResolutionFraction() - kDLSSResolutionFractionError <= ScaleY && ScaleY <= GetMaxUpsampleResolutionFraction() + kDLSSResolutionFractionError,
		TEXT("The current resolution fraction %f is out of the supported DLSS range [%f ... %f] for quality mode %d."),
		ScaleY, GetMinUpsampleResolutionFraction(), GetMaxUpsampleResolutionFraction(), DLSSQualityMode);

	const TCHAR* PassName = TEXT("MainUpsampling");

	// Create outputs
	FDLSSOutputs Outputs;
	{
		FRDGTextureDesc SceneColorDesc = FRDGTextureDesc::Create2D(
			OutputExtent,
			PF_FloatRGBA,
			FClearValueBinding::Black,
			TexCreate_ShaderResource | TexCreate_UAV);

		const TCHAR* OutputName = TEXT("DLSSOutputSceneColor");

		Outputs.SceneColor = GraphBuilder.CreateTexture(
			SceneColorDesc,
			OutputName);
	}

	FDLSSStateRef DLSSState = (InputCustomHistory && InputCustomHistory->DLSSState) ? InputCustomHistory->DLSSState : MakeShared<FDLSSState, ESPMode::ThreadSafe>();
	{
		FDLSSShaderParameters* PassParameters = GraphBuilder.AllocParameters<FDLSSShaderParameters>();

		// Set up common shader parameters
		const FIntPoint InputExtent = Inputs.SceneColorInput->Desc.Extent;
		const FIntRect InputViewRect = Inputs.InputViewRect;
		const FIntRect OutputViewRect = Inputs.OutputViewRect;

		// Input buffer shader parameters
		{
			PassParameters->SceneColorInput = Inputs.SceneColorInput;
			PassParameters->SceneDepthInput = Inputs.SceneDepthInput;
			PassParameters->SceneVelocityInput = Inputs.SceneVelocityInput;
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
			PassParameters->EyeAdaptation = Inputs.EyeAdaptation;
#elif ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION == 2
			PassParameters->EyeAdaptation = AddCopyEyeAdaptationDataToTexturePass(GraphBuilder, View);
#else
			PassParameters->EyeAdaptation = GetEyeAdaptationTexture(GraphBuilder, View);
#endif

			PassParameters->DiffuseAlbedo = Inputs.DiffuseAlbedo;
			PassParameters->SpecularAlbedo = Inputs.SpecularAlbedo;

			PassParameters->Normal = Inputs.Normal;
			PassParameters->Roughness = Inputs.Roughness;
		}

		// Outputs 
		{
			PassParameters->SceneColorOutput = Outputs.SceneColor;
		}

		const float DeltaWorldTimeMS = View.Family->Time.GetDeltaWorldTimeSeconds() * 1000.0f;

		const bool bUseAutoExposure = CVarNGXDLSSAutoExposure.GetValueOnRenderThread() != 0;
		const bool bReleaseMemoryOnDelete = CVarNGXDLSSReleaseMemoryOnDelete.GetValueOnRenderThread() != 0;

		const float Sharpness = FMath::Clamp(CVarNGXDLSSSharpness.GetValueOnRenderThread(), -1.0f, 1.0f);
#if !NO_LOGGING
		static bool bLoggedSharpnessWarning = false;
		if (Sharpness != 0.0f && !bLoggedSharpnessWarning)
		{
			UE_LOG(LogDLSS, Warning, TEXT("DLSS sharpening is deprecated, recommend using the NIS plugin for sharpening instead"));
			bLoggedSharpnessWarning = true;
		}
#endif
		NGXRHI* LocalNGXRHIExtensions = Upscaler->NGXRHIExtensions;
		const int32 NGXDLSSPreset = GetNGXDLSSPresetFromQualityMode(DLSSQualityMode);
		const int32 NGXPerfQuality = ToNGXQuality(DLSSQualityMode);

		auto NGXDenoiserModeString = [](ENGXDLSSDenoiserMode NGXDenoiserMode)
		{
			switch (NGXDenoiserMode)
			{
			case ENGXDLSSDenoiserMode::Off: return TEXT("");
			case ENGXDLSSDenoiserMode::DLSSRR: return TEXT("DLSSRR");
			default:return TEXT("Invalid ENGXDLSSDenoiserMode");
			}
		};
		GraphBuilder.AddPass(
			RDG_EVENT_NAME("DLSS %s%s%s %dx%d -> %dx%d",
				PassName,
				Sharpness != 0.0f ? TEXT(" Sharpen") : TEXT(""),
				NGXDenoiserModeString(Inputs.DenoiserMode),
				SrcRect.Width(), SrcRect.Height(),
				DestRect.Width(), DestRect.Height()),
			PassParameters,
			ERDGPassFlags::Compute | ERDGPassFlags::Raster | ERDGPassFlags::SkipRenderPass,
			// FRHICommandListImmediate forces it to run on render thread, FRHICommandList doesn't
			[LocalNGXRHIExtensions, PassParameters, Inputs, bCameraCut, DeltaWorldTimeMS, Sharpness, NGXDLSSPreset, NGXPerfQuality, DLSSState, bUseAutoExposure, bReleaseMemoryOnDelete](FRHICommandListImmediate& RHICmdList)
		{
			FRHIDLSSArguments DLSSArguments;
			FMemory::Memzero(&DLSSArguments, sizeof(DLSSArguments));

			// input parameters
			DLSSArguments.SrcRect = Inputs.InputViewRect;
			DLSSArguments.DestRect = Inputs.OutputViewRect;

			DLSSArguments.Sharpness = Sharpness;
			DLSSArguments.bReset = bCameraCut;

			DLSSArguments.JitterOffset = Inputs.TemporalJitterPixels;
			DLSSArguments.MotionVectorScale = FVector2f::UnitVector;

			DLSSArguments.bHighResolutionMotionVectors = Inputs.bHighResolutionMotionVectors;
			DLSSArguments.DeltaTimeMS = DeltaWorldTimeMS;
			DLSSArguments.bReleaseMemoryOnDelete = bReleaseMemoryOnDelete;

			DLSSArguments.DLSSPreset = NGXDLSSPreset;
			DLSSArguments.PerfQuality = NGXPerfQuality;

			check(PassParameters->SceneColorInput);
			PassParameters->SceneColorInput->MarkResourceAsUsed();
			DLSSArguments.InputColor = PassParameters->SceneColorInput->GetRHI();
					

			check(PassParameters->SceneVelocityInput);
			PassParameters->SceneVelocityInput->MarkResourceAsUsed();
			DLSSArguments.InputMotionVectors = PassParameters->SceneVelocityInput->GetRHI();

			check(PassParameters->SceneDepthInput);
			PassParameters->SceneDepthInput->MarkResourceAsUsed();
			DLSSArguments.InputDepth = PassParameters->SceneDepthInput->GetRHI();

			check(PassParameters->EyeAdaptation);
			PassParameters->EyeAdaptation->MarkResourceAsUsed();
			DLSSArguments.InputExposure = PassParameters->EyeAdaptation->GetRHI();
			DLSSArguments.PreExposure = Inputs.PreExposure;
			DLSSArguments.bUseAutoExposure = bUseAutoExposure;


			DLSSArguments.DenoiserMode = Inputs.DenoiserMode;

			if (DLSSArguments.DenoiserMode == ENGXDLSSDenoiserMode::DLSSRR)
			{
				check(PassParameters->DiffuseAlbedo)
				PassParameters->DiffuseAlbedo->MarkResourceAsUsed();
				DLSSArguments.InputDiffuseAlbedo = PassParameters->DiffuseAlbedo->GetRHI();

				check(PassParameters->SpecularAlbedo)
				PassParameters->SpecularAlbedo->MarkResourceAsUsed();
				DLSSArguments.InputSpecularAlbedo = PassParameters->SpecularAlbedo->GetRHI();
				
				check(PassParameters->Normal)
				PassParameters->Normal->MarkResourceAsUsed();
				DLSSArguments.InputNormals = PassParameters->Normal->GetRHI();

				check(PassParameters->Roughness)
				PassParameters->Roughness->MarkResourceAsUsed();
				DLSSArguments.InputRoughness = PassParameters->Roughness->GetRHI();
			}

			// output images
			check(PassParameters->SceneColorOutput);
			PassParameters->SceneColorOutput->MarkResourceAsUsed();
			DLSSArguments.OutputColor = PassParameters->SceneColorOutput->GetRHI();

			RHICmdList.Transition(FRHITransitionInfo(DLSSArguments.OutputColor, ERHIAccess::Unknown, ERHIAccess::UAVMask));
			RHICmdList.EnqueueLambda(
				[LocalNGXRHIExtensions, DLSSArguments, DLSSState](FRHICommandListImmediate& Cmd) mutable
			{
				const uint32 FeatureCreationNode = CVarNGXDLSSFeatureCreationNode.GetValueOnRenderThread();
				const uint32 FeatureVisibilityMask = CVarNGXDLSSFeatureVisibilityMask.GetValueOnRenderThread();

				DLSSArguments.GPUNode = FeatureCreationNode == -1 ? Cmd.GetGPUMask().ToIndex() : FMath::Clamp(FeatureCreationNode, 0u, GNumExplicitGPUsForRendering - 1);
				DLSSArguments.GPUVisibility = FeatureVisibilityMask == -1 ? Cmd.GetGPUMask().GetNative() : (Cmd.GetGPUMask().All().GetNative() & FeatureVisibilityMask) ;

				LocalNGXRHIExtensions->ExecuteDLSS(Cmd, DLSSArguments, DLSSState);
			});
		});
	}

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
	check(OutputCustomHistoryInterface);
	(*OutputCustomHistoryInterface) = new FDLSSUpscalerHistory(DLSSState);
#else
	if (!View.bStatePrevViewInfoIsReadOnly && OutputHistory)
	{
		OutputHistory->SafeRelease();

		GraphBuilder.QueueTextureExtraction(Outputs.SceneColor, &OutputHistory->RT[0]);

		OutputHistory->ViewportRect = DestRect;
		OutputHistory->ReferenceBufferSize = OutputExtent;
	}


	if (!View.bStatePrevViewInfoIsReadOnly && OutputCustomHistoryInterface)
	{
		if (!OutputCustomHistoryInterface->GetReference())
		{
			(*OutputCustomHistoryInterface) = new FDLSSUpscalerHistory(DLSSState);
		}
	}
#endif
	return Outputs;
}

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
const TCHAR* FDLSSUpscalerHistory::GetDebugName() const
{
	return GDLSSSceneViewFamilyUpscalerDebugName;
}

uint64 FDLSSUpscalerHistory::GetGPUSizeBytes() const
{
	// TODO
	return 0;
}
#endif

void FDLSSUpscaler::Tick(FRHICommandListImmediate& RHICmdList)
{
	check(NGXRHIExtensions);
	check(IsInRenderingThread());
	// Pass it over to the RHI thread which handles the lifetime of the NGX DLSS resources
	RHICmdList.EnqueueLambda(
		[this](FRHICommandListImmediate& Cmd)
	{
		NGXRHIExtensions->TickPoolElements();
	});
}

bool FDLSSUpscaler::IsQualityModeSupported(EDLSSQualityMode InQualityMode) const
{
	return ResolutionSettings[ToNGXQuality(InQualityMode)].bIsSupported;
}

bool FDLSSUpscaler::IsDLSSActive() const
{
	static const auto CVarTemporalAAUpscaler = IConsoleManager::Get().FindConsoleVariable(TEXT("r.TemporalAA.Upscaler"));
	static const IDLSSModuleInterface* DLSSModule = &FModuleManager::LoadModuleChecked<IDLSSModuleInterface>(TEXT("DLSS"));
	check(DLSSModule);
	check(CVarTemporalAAUpscaler);
	const bool bDLSSActive =
		DLSSModule->QueryDLSSSRSupport() == EDLSSSupport::Supported &&
		CVarTemporalAAUpscaler && (CVarTemporalAAUpscaler->GetInt() != 0) &&
		(CVarNGXDLSSEnable.GetValueOnAnyThread() != 0);
	return bDLSSActive;
}


void FDLSSUpscaler::SetupViewFamily(FSceneViewFamily& ViewFamily)
{
	const ISceneViewFamilyScreenPercentage* ScreenPercentageInterface = ViewFamily.GetScreenPercentageInterface();
	float DesiredResolutionFraction = ScreenPercentageInterface->GetResolutionFractionsUpperBound()[GDynamicPrimaryResolutionFraction];

	TOptional<EDLSSQualityMode> SelectedDLSSQualityMode;

	static_assert(int32(EDLSSQualityMode::NumValues) == 6, "dear DLSS plugin NVIDIA developer, please update this code to handle the new EDLSSQualityMode enum values");
	for (EDLSSQualityMode DLSSQualityMode : { EDLSSQualityMode::UltraPerformance,  EDLSSQualityMode::Performance , EDLSSQualityMode::Balanced, EDLSSQualityMode::Quality,  EDLSSQualityMode::UltraQuality, EDLSSQualityMode::DLAA })
	{
		bool bIsSupported = FDLSSUpscaler::IsQualityModeSupported(DLSSQualityMode);
		if (!bIsSupported)
		{
			continue;
		}

		float MinResolutionFraction = FDLSSUpscaler::GetMinResolutionFractionForQuality(DLSSQualityMode);
		float MaxResolutionFraction = FDLSSUpscaler::GetMaxResolutionFractionForQuality(DLSSQualityMode);
		float TargetResolutionFraction = FDLSSUpscaler::GetOptimalResolutionFractionForQuality(DLSSQualityMode);

		bool bIsCompatible = DesiredResolutionFraction <= 1.0 &&
			DesiredResolutionFraction >= (MinResolutionFraction - kDLSSResolutionFractionError) &&
			DesiredResolutionFraction <= (MaxResolutionFraction + kDLSSResolutionFractionError);
		bool bIsClosestYet = false;
		if (SelectedDLSSQualityMode.IsSet())
		{
			float SelectedTargetResolutionFraction = FDLSSUpscaler::GetOptimalResolutionFractionForQuality(SelectedDLSSQualityMode.GetValue());
			bIsClosestYet = FMath::Abs(TargetResolutionFraction - DesiredResolutionFraction) < FMath::Abs(SelectedTargetResolutionFraction - DesiredResolutionFraction);
		}
		else if (bIsCompatible)
		{
			bIsClosestYet = true;
		}

		if (bIsCompatible && bIsClosestYet)
		{
			SelectedDLSSQualityMode = DLSSQualityMode;
		}
	}

	if (SelectedDLSSQualityMode.IsSet())
	{
		ViewFamily.SetTemporalUpscalerInterface(new FDLSSSceneViewFamilyUpscaler(this, SelectedDLSSQualityMode.GetValue()));
	}
	else if (DesiredResolutionFraction != PreviousResolutionFraction)
	{
		UE_LOG(LogDLSS, Warning, TEXT("Could not setup DLSS upscaler for screen percentage = %f"), DesiredResolutionFraction * 100.0f);
	}
	PreviousResolutionFraction = DesiredResolutionFraction;
}

TOptional<EDLSSQualityMode> FDLSSUpscaler::GetAutoQualityModeFromPixels(int PixelCount) const
{
	if (PixelCount >= 8'300'000 && IsQualityModeSupported(EDLSSQualityMode::UltraPerformance))
	{
		return EDLSSQualityMode::UltraPerformance;
	}
	else if (PixelCount >= 3'690'000 && IsQualityModeSupported(EDLSSQualityMode::Performance))
	{
		return EDLSSQualityMode::Performance;
	}
	else if (PixelCount >= 2'030'000 && IsQualityModeSupported(EDLSSQualityMode::Quality))
	{
		return EDLSSQualityMode::Quality;
	}

	return TOptional<EDLSSQualityMode> {};
}


bool FDLSSUpscaler::EnableDLSSInPlayInEditorViewports() const
{
	if (GetDefault<UDLSSOverrideSettings>()->EnableDLSSInPlayInEditorViewportsOverride == EDLSSSettingOverride::UseProjectSettings)
	{
		return GetDefault<UDLSSSettings>()->bEnableDLSSInPlayInEditorViewports;
	}
	else
	{
		return GetDefault<UDLSSOverrideSettings>()->EnableDLSSInPlayInEditorViewportsOverride == EDLSSSettingOverride::Enabled;
	}
}

float FDLSSUpscaler::GetOptimalResolutionFractionForQuality(EDLSSQualityMode Quality) const
{
	checkf(IsQualityModeSupported(Quality),TEXT("%u is not a valid Quality mode"), Quality);
	return ResolutionSettings[ToNGXQuality(Quality)].OptimalResolutionFraction;
}

float  FDLSSUpscaler::GetOptimalSharpnessForQuality(EDLSSQualityMode Quality) const
{
	checkf(IsQualityModeSupported(Quality), TEXT("%u is not a valid Quality mode"), Quality);
	return ResolutionSettings[ToNGXQuality(Quality)].Sharpness;
}

float FDLSSUpscaler::GetMinResolutionFractionForQuality(EDLSSQualityMode Quality) const
{
	checkf(IsQualityModeSupported(Quality), TEXT("%u is not a valid Quality mode"), Quality);
	return ResolutionSettings[ToNGXQuality(Quality)].MinResolutionFraction;
}

float FDLSSUpscaler::GetMaxResolutionFractionForQuality(EDLSSQualityMode Quality) const
{
	checkf(IsQualityModeSupported(Quality), TEXT("%u is not a valid Quality mode"), Quality);
	return ResolutionSettings[ToNGXQuality(Quality)].MaxResolutionFraction;
}

bool FDLSSUpscaler::IsFixedResolutionFraction(EDLSSQualityMode Quality) const
{
	checkf(IsQualityModeSupported(Quality), TEXT("%u is not a valid Quality mode"), Quality);
	return ResolutionSettings[ToNGXQuality(Quality)].IsFixedResolution();
}

#undef LOCTEXT_NAMESPACE
