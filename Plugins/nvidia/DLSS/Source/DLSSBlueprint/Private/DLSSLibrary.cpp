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

#include "DLSSLibrary.h"

#if WITH_DLSS
#include "DLSS.h"
#include "DLSSSettings.h"
#include "DLSSUpscaler.h"
#include "NGXRHI.h"
#endif

#include "Interfaces/IPluginManager.h"
#include "Modules/ModuleManager.h"
#include "Runtime/Launch/Resources/Version.h"
#include "SceneView.h"
#include "ShaderCore.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DLSSLibrary)

#define LOCTEXT_NAMESPACE "FDLSSBlueprintModule"
DEFINE_LOG_CATEGORY_STATIC(LogDLSSBlueprint, Log, All);

static TAutoConsoleVariable<int32> CVarNGXDLSSPreferNISSharpen(
	TEXT("r.NGX.DLSS.PreferNISSharpen"),
	2,
	TEXT("Prefer sharpening with an extra NIS plugin sharpening pass instead of DLSS sharpening if the NIS plugin is also enabled for the project. (default: 1)\n")
	TEXT("Requires the NIS plugin to be enabled\n")
	TEXT("   0: Softening/sharpening with the DLSS pass.\n")
	TEXT("   1: Sharpen with the NIS plugin. Softening is not supported. Requires the NIS plugin to be enabled.\n")
	TEXT("   2: Sharpen with the NIS plugin. Softening (i.e. negative sharpness) with the DLSS plugin. Requires the NIS plugin to be enabled.\n")
	TEXT("Note: This cvar is only evaluated when using the deprecated `SetDLSSSharpness` Blueprint function, from either C++ or a Blueprint event graph!")
	TEXT("Note: DLSS sharpening is deprecated, future plugin versions will remove DLSS sharpening. Use the NIS plugin for sharpening instead\n"),
	ECVF_RenderThreadSafe);

static const FName SetDLSSModeInvalidEnumValueError= FName("SetDLSSModeInvalidEnumValueError");
static const FName IsDLSSModeSupportedInvalidEnumValueError = FName("IsDLSSModeSupportedInvalidEnumValueError");

UDLSSSupport UDLSSLibrary::DLSSSRSupport = UDLSSSupport::NotSupportedByPlatformAtBuildTime;
UDLSSSupport UDLSSLibrary::DLSSRRSupport = UDLSSSupport::NotSupportedByPlatformAtBuildTime;
#if WITH_DLSS
int32 UDLSSLibrary::MinDLSSSRDriverVersionMajor = 0;
int32 UDLSSLibrary::MinDLSSSRDriverVersionMinor = 0;
int32 UDLSSLibrary::MinDLSSRRDriverVersionMajor = 0;
int32 UDLSSLibrary::MinDLSSRRDriverVersionMinor = 0; // placeholder
int32 UDLSSLibrary::PreviousShadowDenoiser = 1;
int32 UDLSSLibrary::PreviousLumenSSR = 1;
int32 UDLSSLibrary::PreviousLumenTemporal = 1;
bool UDLSSLibrary::bDenoisingRequested = false;

FDLSSUpscaler* UDLSSLibrary::DLSSUpscaler = nullptr;
bool UDLSSLibrary::bDLSSLibraryInitialized = false;
UDLSSMode UDLSSLibrary::CurrentDLSSModeDeprecated = UDLSSMode::Quality;
bool UDLSSLibrary::bDLAAEnabledDeprecated = false;

static bool ShowDLSSSDebugOnScreenMessages()
{

	if (GetDefault<UDLSSOverrideSettings>()->ShowDLSSSDebugOnScreenMessages == EDLSSSettingOverride::UseProjectSettings)
	{
		return GetDefault<UDLSSSettings>()->bShowDLSSSDebugOnScreenMessages;
	}
	else
	{
		return GetDefault<UDLSSOverrideSettings>()->ShowDLSSSDebugOnScreenMessages == EDLSSSettingOverride::Enabled;
	}
}


#if !UE_BUILD_SHIPPING

UDLSSLibrary::FDLSSErrorState UDLSSLibrary::DLSSErrorState;
FDelegateHandle UDLSSLibrary::DLSSOnScreenMessagesDelegateHandle;
void UDLSSLibrary::GetDLSSOnScreenMessages(TMultiMap<FCoreDelegates::EOnScreenMessageSeverity, FText>& OutMessages)
{
	check(IsInGameThread());

	// We need a valid DLSSSRSupport, so calling this here in case other UDLSSLibrary functions which call TryInitDLSSLibrary() haven't been called
	if (!TryInitDLSSLibrary())
	{
		return;
	}

	if(ShowDLSSSDebugOnScreenMessages())
	{

		if (DLSSErrorState.bIsDLSSModeUnsupported)
		{
			const FTextFormat Format(LOCTEXT("DLSSOnScreenDebugSetModeUnsupportedDLSSMode",
				"DLSS Error: The DLSS mode \"{0}\" is not supported. This error can be avoided by calling SetDLSSMode({0}) only if IsDLSSModeSupported({0}) returns true."));
			const FText Message = FText::Format(Format, StaticEnum<UDLSSMode>()->GetDisplayNameTextByValue(int64(DLSSErrorState.InvalidDLSSMode)));
			OutMessages.Add(FCoreDelegates::EOnScreenMessageSeverity::Error, Message);
		}

		// Checking for "contains" in case virtualization is changing the GPU string
		const bool bIsNVIDIA = FWindowsPlatformMisc::GetPrimaryGPUBrand().Contains(TEXT("NVIDIA"));

		bool bShowNotSupportedMessage = bIsNVIDIA && (UDLSSSupport::Supported != DLSSSRSupport);

		if ((UDLSSSupport::NotSupportedIncompatibleAPICaptureToolActive == DLSSSRSupport) && !GetMutableDefault<UDLSSOverrideSettings>()->bShowDLSSIncompatiblePluginsToolsWarnings )
		{
			bShowNotSupportedMessage = false;
		}

		if (bShowNotSupportedMessage)
		{
			const FTextFormat Format(LOCTEXT("DLSSOnScreenDebugDLSSNotSupported",
				"DLSS Information: DLSS is not supported due to {0}.Please see the various LogDLSS* categories in the Developer Tools -> Output Log for further detail."));
			const FText Message = FText::Format(Format, StaticEnum<UDLSSSupport>()->GetDisplayNameTextByValue(int64(DLSSSRSupport)));
			OutMessages.Add(FCoreDelegates::EOnScreenMessageSeverity::Warning, Message);
		}
	}
}
#endif


static EDLSSQualityMode ToEDLSSQualityMode(UDLSSMode InDLSSQualityMode)
{
	static_assert(int32(EDLSSQualityMode::NumValues) == 6, "dear DLSS plugin NVIDIA developer, please update this code to translate the new EDLSSQualityMode enum values to UDLSSMode");

	switch (InDLSSQualityMode)
	{

	case UDLSSMode::UltraPerformance: 
		return EDLSSQualityMode::UltraPerformance;
	
	// the higher level code should catch this...
	case UDLSSMode::Off:
		checkf(InDLSSQualityMode != UDLSSMode::Off, TEXT("ToEDLSSQualityMode should not be called with an InDLSSQualityMode of UDLSSMode::Off from the higher level code"));
	default:
		checkf(false, TEXT("ToEDLSSQualityMode should not be called with an out of range InDLSSQualityMode %d InDLSSQualityMode from the higher level code"), InDLSSQualityMode);

	case UDLSSMode::Performance:
		return EDLSSQualityMode::Performance;
	
	case UDLSSMode::Balanced:
		return EDLSSQualityMode::Balanced;
	
	case UDLSSMode::Quality:
		return EDLSSQualityMode::Quality;
	
	case UDLSSMode::UltraQuality:
		return EDLSSQualityMode::UltraQuality;
		
	case UDLSSMode::DLAA:
		return EDLSSQualityMode::DLAA;

	}
}

#endif

bool UDLSSLibrary::IsDLSSModeSupported(UDLSSMode DLSSMode)
{
	const UEnum* Enum = StaticEnum<UDLSSMode>();

	// UEnums are strongly typed, but then one can also cast a byte to an UEnum ...
	if (Enum->IsValidEnumValue(int64(DLSSMode)) && (Enum->GetMaxEnumValue() != int64(DLSSMode)))
	{
		if (DLSSMode == UDLSSMode::Off)
		{
			return true;
		}
#if WITH_DLSS
		if (!TryInitDLSSLibrary())
		{
			UE_LOG(LogDLSSBlueprint, Error, TEXT("IsDLSSModeSupported should not be called before PostEngineInit"));
			return false;
		}
		if (!IsDLSSSupported())
		{
			return false;
		}
		else if (DLSSMode == UDLSSMode::Auto)
		{
			// support for auto quality mode was dropped with UE 5.1 (except as a way to ask for optimal screen percentage for a given resolution)
			return false;
		}
		else
		{
			return DLSSUpscaler->IsQualityModeSupported(ToEDLSSQualityMode(DLSSMode));
		}
#else
		return false;
#endif
	}
	else
	{
#if !UE_BUILD_SHIPPING
		FFrame::KismetExecutionMessage(*FString::Printf(
			TEXT("IsDLSSModeSupported should not be called with an invalid DLSSMode enum value (%d) \"%s\""),
			int64(DLSSMode), *StaticEnum<UDLSSMode>()->GetDisplayNameTextByValue(int64(DLSSMode)).ToString()),
			ELogVerbosity::Error, IsDLSSModeSupportedInvalidEnumValueError);
#endif 
		return false;
	}

}

void UDLSSLibrary::GetDLSSModeInformation(UDLSSMode DLSSMode, FVector2D ScreenResolution, bool& bIsSupported, float& OptimalScreenPercentage, bool& bIsFixedScreenPercentage, float& MinScreenPercentage, float& MaxScreenPercentage, float& OptimalSharpness)
{
	// fall back to TAA for unsupported or DLSS off
	OptimalScreenPercentage = 0.0f;
	bIsFixedScreenPercentage = false;
	MinScreenPercentage = 100.0f * ISceneViewFamilyScreenPercentage::kMinTAAUpsampleResolutionFraction;
	MaxScreenPercentage = 100.0f * ISceneViewFamilyScreenPercentage::kMaxTAAUpsampleResolutionFraction;
	OptimalSharpness = 0.0f;
#if WITH_DLSS
	if (!TryInitDLSSLibrary())
	{
		UE_LOG(LogDLSSBlueprint, Error, TEXT("GetDLSSModeInformation should not be called before PostEngineInit"));
		bIsSupported = false;
		return;
	}
#endif
	bIsSupported = IsDLSSModeSupported(DLSSMode);

#if WITH_DLSS
	if ((DLSSMode != UDLSSMode::Off) && bIsSupported || (DLSSMode == UDLSSMode::Auto))
	{
		EDLSSQualityMode EDLSSMode;
		if (DLSSMode != UDLSSMode::Auto)
		{
			EDLSSMode = ToEDLSSQualityMode(DLSSMode);
		}
		else
		{
			// Auto DLSS mode is based on total pixels
			float PixelsFloat = ScreenResolution.X * ScreenResolution.Y;
			int32 PixelsInt = (PixelsFloat < static_cast<float>(MAX_int32)) ? static_cast<int32>(PixelsFloat) : MAX_int32;
			TOptional<EDLSSQualityMode> MaybeDLSSMode = DLSSUpscaler->GetAutoQualityModeFromPixels(PixelsInt);
			if (!MaybeDLSSMode.IsSet())
			{
				// DLSS Off
				return;
			}
			EDLSSMode = MaybeDLSSMode.GetValue();
		}
		bIsFixedScreenPercentage = DLSSUpscaler->IsFixedResolutionFraction(EDLSSMode);

		OptimalScreenPercentage = 100.0f * DLSSUpscaler->GetOptimalResolutionFractionForQuality(EDLSSMode);
		MinScreenPercentage = 100.0f * DLSSUpscaler->GetMinResolutionFractionForQuality(EDLSSMode);
		MaxScreenPercentage = 100.0f * DLSSUpscaler->GetMaxResolutionFractionForQuality(EDLSSMode);

		OptimalSharpness = DLSSUpscaler->GetOptimalSharpnessForQuality(EDLSSMode);
	}
#endif
}

void UDLSSLibrary::GetDLSSScreenPercentageRange(float& MinScreenPercentage, float& MaxScreenPercentage)
{
#if WITH_DLSS
	if (!TryInitDLSSLibrary())
	{
		UE_LOG(LogDLSSBlueprint, Error, TEXT("GetDLSSScreenPercentageRange should not be called before PostEngineInit"));
		MinScreenPercentage = 100.0f;
		MaxScreenPercentage = 100.0f;
		return;
	}

	if (IsDLSSSupported())
	{
		MinScreenPercentage = 100.0f * DLSSUpscaler->GetMinUpsampleResolutionFraction();
		MaxScreenPercentage = 100.0f * DLSSUpscaler->GetMaxUpsampleResolutionFraction();
	}
	else
#endif
	{
		MinScreenPercentage = 100.0f;
		MaxScreenPercentage = 100.0f;
	}
}

TArray<UDLSSMode> UDLSSLibrary::GetSupportedDLSSModes()
{
	TArray<UDLSSMode> SupportedQualityModes;
#if WITH_DLSS
	if (!TryInitDLSSLibrary())
	{
		UE_LOG(LogDLSSBlueprint, Error, TEXT("GetSupportedDLSSModes should not be called before PostEngineInit"));
		return SupportedQualityModes;
	}
#endif
	{
		const UEnum* Enum = StaticEnum<UDLSSMode>();
		for (int32 EnumIndex = 0; EnumIndex < Enum->NumEnums(); ++EnumIndex)
		{
			const int64 EnumValue = Enum->GetValueByIndex(EnumIndex);
			if (EnumValue != Enum->GetMaxEnumValue())
			{
				const UDLSSMode QualityMode = UDLSSMode(EnumValue);
				if (IsDLSSModeSupported(QualityMode))
				{
					SupportedQualityModes.Add(QualityMode);
				}
			}
		}
	}
	return SupportedQualityModes;
}

bool UDLSSLibrary::IsDLSSSupported()
{
#if WITH_DLSS
	if (!TryInitDLSSLibrary())
	{
		UE_LOG(LogDLSSBlueprint, Error, TEXT("IsDLSSSupported should not be called before PostEngineInit"));
		return false;
	}

	return QueryDLSSSupport() == UDLSSSupport::Supported;
#else
	return false;
#endif
}

UDLSSSupport UDLSSLibrary::QueryDLSSSupport()
{
#if WITH_DLSS
	if (!TryInitDLSSLibrary())
	{
		UE_LOG(LogDLSSBlueprint, Error, TEXT("QueryDLSSSRSupport should not be called before PostEngineInit"));
		return UDLSSSupport::NotSupported;
	}
#endif
	return DLSSSRSupport;
}

bool UDLSSLibrary::IsDLSSRRSupported()
{
#if WITH_DLSS
	if (!TryInitDLSSLibrary())
	{
		UE_LOG(LogDLSSBlueprint, Error, TEXT("IsDLSSRRSupported should not be called before PostEngineInit"));
		return false;
	}

	return QueryDLSSRRSupport() == UDLSSSupport::Supported;
#else
	return false;
#endif
}

UDLSSSupport UDLSSLibrary::QueryDLSSRRSupport()
{
#if WITH_DLSS
	if (!TryInitDLSSLibrary())
	{
		UE_LOG(LogDLSSBlueprint, Error, TEXT("QueryDLSSRRSupport should not be called before PostEngineInit"));
		return UDLSSSupport::NotSupported;
	}
#endif
	return DLSSRRSupport;
}

void UDLSSLibrary::GetDLSSMinimumDriverVersion(int32& MinDriverVersionMajor, int32& MinDriverVersionMinor)
{
#if WITH_DLSS
	if (!TryInitDLSSLibrary())
	{
		UE_LOG(LogDLSSBlueprint, Error, TEXT("GetDLSSMinimumDriverVersion should not be called before PostEngineInit"));
	}
	MinDriverVersionMajor = MinDLSSSRDriverVersionMajor;
	MinDriverVersionMinor = MinDLSSSRDriverVersionMinor;
#else
	MinDriverVersionMajor = 0;
	MinDriverVersionMinor = 0;
#endif
}

void UDLSSLibrary::GetDLSSRRMinimumDriverVersion(int32& MinDriverVersionMajor, int32& MinDriverVersionMinor)
{
#if WITH_DLSS
	if (!TryInitDLSSLibrary())
	{
		UE_LOG(LogDLSSBlueprint, Error, TEXT("GetDLSSRRMinimumDriverVersion should not be called before PostEngineInit"));
	}
	MinDriverVersionMajor = MinDLSSRRDriverVersionMajor;
	MinDriverVersionMinor = MinDLSSRRDriverVersionMinor;
#else
	MinDriverVersionMajor = 0;
	MinDriverVersionMinor = 0;
#endif
}

void UDLSSLibrary::EnableDLSS(bool bEnabled)
{
#if WITH_DLSS
	if (!TryInitDLSSLibrary())
	{
		UE_LOG(LogDLSSBlueprint, Error, TEXT("EnableDLSS should not be called before PostEngineInit"));
		return;
	}

	const bool bDLSSSupported = (DLSSSRSupport == UDLSSSupport::Supported);
	if (!bDLSSSupported)
	{
		return;
	}

	static IConsoleVariable* CVarDLSSEnable = IConsoleManager::Get().FindConsoleVariable(TEXT("r.NGX.DLSS.Enable"));
	if (CVarDLSSEnable)
	{
		CVarDLSSEnable->Set(bEnabled ? 1 : 0, ECVF_SetByCommandline);

		// Spatial upscalers such as NIS might set this to 0, but we need r.TemporalAA.Upscaler to be 1 for DLSS to work.
		// but we don't want to change the Cvar if DLSS is not active as to avoid impacting other code paths
		// we don't need to set r.TemporalAA.Upsampling since r.TemporalAA.Upscaler implies that
		if (bEnabled)
		{
			static const auto CVarTemporalAAUpscaler = IConsoleManager::Get().FindConsoleVariable(TEXT("r.TemporalAA.Upscaler"));
			CVarTemporalAAUpscaler->Set(1, ECVF_SetByCommandline);
			// restore denoising state in case it was disabled before
			EnableDLSSRR(bDenoisingRequested);
		}
		else if (IsDLSSRREnabled())
		{
			// Denoising currently only runs as an upscaler pass, so if DLSS-SR is disabled, we automatically disable DLSS-RR as well
			EnableDLSSRR(false);
			bDenoisingRequested = true;
			UE_LOG(LogDLSSBlueprint, Warning, TEXT("DLSS denoising unsupported without DLSS super resolution, disabled denoising"));
		}
	}
#endif
}

void UDLSSLibrary::EnableDLSSRR(bool bEnabled)
{
#if WITH_DLSS
	if (!TryInitDLSSLibrary())
	{
		UE_LOG(LogDLSSBlueprint, Error, TEXT("EnableDLSSRR should not be called before PostEngineInit"));
		return;
	}

	const bool bDLSSRRSupported = (DLSSRRSupport == UDLSSSupport::Supported);
	if (!bDLSSRRSupported)
	{
		return;
	}

	bDenoisingRequested = bEnabled;
	static IConsoleVariable* CVarDLSSRREnable = IConsoleManager::Get().FindConsoleVariable(TEXT("r.NGX.DLSS.DenoiserMode"));
	const bool bDLSSRREnabled = CVarDLSSRREnable->GetInt() != 0;
	if (bDLSSRREnabled == bEnabled)
	{
		return;
	}

	// Denoising currently only runs as an upscaler pass, so only enable DLSS-RR if DLSS-SR is enabled
	if (bEnabled && IsDLSSEnabled())
	{
		CVarDLSSRREnable->Set(1);
		PreviousShadowDenoiser       = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Shadow.Denoiser"))->GetInt();
		IConsoleManager::Get().FindConsoleVariable(TEXT("r.Shadow.Denoiser"))->Set(0);
		PreviousLumenSSR             = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.Reflections.ScreenSpaceReconstruction"))->GetInt();
		PreviousLumenTemporal        = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.Reflections.Temporal"))->GetInt();
		IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.Reflections.ScreenSpaceReconstruction"))->Set(0);
		IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.Reflections.Temporal"))->Set(0);
		static const auto CVarTemporalAAUpscaler = IConsoleManager::Get().FindConsoleVariable(TEXT("r.TemporalAA.Upscaler"));
		CVarTemporalAAUpscaler->Set(1, ECVF_SetByCommandline);
		int LumenBilateralFilter = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.Reflections.BilateralFilter"))->GetInt();
		if (LumenBilateralFilter != 0)
		{
			// engine bug in 5.2 and 5.3 prevents us from changing the value of this cvar from Blueprints
			UE_LOG(LogDLSSBlueprint, Warning, TEXT("r.Lumen.Reflections.BilateralFilter should be disabled when DLSS Ray Reconstruction is enabled"));
		}
	}
	else if (!bEnabled)
	{
		CVarDLSSRREnable->Set(0);
		IConsoleManager::Get().FindConsoleVariable(TEXT("r.Shadow.Denoiser"))->Set(PreviousShadowDenoiser);
		IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.Reflections.ScreenSpaceReconstruction"))->Set(PreviousLumenSSR);
		IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.Reflections.Temporal"))->Set(PreviousLumenTemporal);
	}
#endif
}

bool UDLSSLibrary::IsDLSSEnabled()
{
#if WITH_DLSS
	if (!TryInitDLSSLibrary())
	{
		UE_LOG(LogDLSSBlueprint, Error, TEXT("IsDLSSEnabled should not be called before PostEngineInit"));
		return false;
	}

	// if NGX library loading was disabled, DLSS will be unsupported
	const bool bDLSSSupported = (DLSSSRSupport == UDLSSSupport::Supported);

	static const IConsoleVariable* CVarDLSSEnable = IConsoleManager::Get().FindConsoleVariable(TEXT("r.NGX.DLSS.Enable"));
	const bool bDLSSEnabled = CVarDLSSEnable && (CVarDLSSEnable->GetInt() != 0);

	return bDLSSSupported && bDLSSEnabled;
#else
	return false;
#endif
}

bool UDLSSLibrary::IsDLSSRREnabled()
{
#if WITH_DLSS
	if (!TryInitDLSSLibrary())
	{
		UE_LOG(LogDLSSBlueprint, Error, TEXT("IsDLSSRREnabled should not be called before PostEngineInit"));
		return false;
	}

	// if NGX library loading was disabled, DLSS will be unsupported
	const bool bDLSSSupported = (DLSSSRSupport == UDLSSSupport::Supported);

	static const IConsoleVariable* CVarDLSSDenoiserMode = IConsoleManager::Get().FindConsoleVariable(TEXT("r.NGX.DLSS.DenoiserMode"));
	const bool bDLSSRREnabled = CVarDLSSDenoiserMode && (CVarDLSSDenoiserMode->GetInt() != 0);

	return bDLSSSupported && bDLSSRREnabled;
#else
	return false;
#endif
}

// deprecated
void UDLSSLibrary::EnableDLAA(bool bEnabled)
{
#if WITH_DLSS
	if (!TryInitDLSSLibrary())
	{
		UE_LOG(LogDLSSBlueprint, Error, TEXT("EnableDLAA should not be called before PostEngineInit"));
		return;
	}

	const bool bDLAASupported = (DLSSSRSupport == UDLSSSupport::Supported);

	bDLAAEnabledDeprecated = bDLAASupported && bEnabled;
	if (bDLAAEnabledDeprecated)
	{
		EnableDLSS(true);
	}
	else
	{
		// fall back to last set DLSS mode
		SetDLSSMode(nullptr, CurrentDLSSModeDeprecated);
	}
#endif
}

// deprecated
bool UDLSSLibrary::IsDLAAEnabled()
{
#if WITH_DLSS
	if (!TryInitDLSSLibrary())
	{
		UE_LOG(LogDLSSBlueprint, Error, TEXT("IsDLAAEnabled should not be called before PostEngineInit"));
		return false;
	}

	const bool bDLAASupported = (DLSSSRSupport == UDLSSSupport::Supported);

	return bDLAASupported && bDLAAEnabledDeprecated;
#else
	return false;
#endif
}

// deprecated
void UDLSSLibrary::SetDLSSMode(UObject* WorldContextObject, UDLSSMode DLSSMode)
{
#if WITH_DLSS
	if (!TryInitDLSSLibrary())
	{
		UE_LOG(LogDLSSBlueprint, Error, TEXT("SetDLSSMode should not be called before PostEngineInit"));
		return;
	}

	const UEnum* Enum = StaticEnum<UDLSSMode>();

	// UEnums are strongly typed, but then one can also cast a byte to an UEnum ...
	if(Enum->IsValidEnumValue(int64(DLSSMode)) && (Enum->GetMaxEnumValue() != int64(DLSSMode)))
	{
		CurrentDLSSModeDeprecated = DLSSMode;
		const bool bDLSSSupported = (DLSSSRSupport == UDLSSSupport::Supported);

		TOptional<EDLSSQualityMode> MaybeQualityMode{};
		if (bDLSSSupported)
		{
			if ((DLSSMode != UDLSSMode::Off) && (DLSSMode != UDLSSMode::Auto))
			{
				MaybeQualityMode = ToEDLSSQualityMode(DLSSMode);
			}
		}

		static IConsoleVariable* CVarScreenPercentage = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ScreenPercentage"));
		if (MaybeQualityMode.IsSet() && DLSSUpscaler->IsQualityModeSupported(*MaybeQualityMode))
		{
			// enable DLSS, and set screen percentage for backward compatibility with earlier plugin versions
			float OptimalScreenPercentage = 100.0f * DLSSUpscaler->GetOptimalResolutionFractionForQuality(*MaybeQualityMode);
			if (CVarScreenPercentage != nullptr)
			{
				EConsoleVariableFlags Priority = static_cast<EConsoleVariableFlags>(CVarScreenPercentage->GetFlags() & ECVF_SetByMask);
				CVarScreenPercentage->Set(OptimalScreenPercentage, Priority);
			}
			EnableDLSS(true);
		}
		else
		{
			// disable DLSS, and set screen percentage to 100 for backward compatibility with earlier plugin versions
			if (CVarScreenPercentage != nullptr)
			{
				EConsoleVariableFlags Priority = static_cast<EConsoleVariableFlags>(CVarScreenPercentage->GetFlags() & ECVF_SetByMask);
				CVarScreenPercentage->Set(100.0f, Priority);
			}
			// don't override DLAA if DLAA is still enabled
			if (!bDLAAEnabledDeprecated)
			{
				EnableDLSS(false);
			}
		}

		if (DLSSMode != UDLSSMode::Off)
		{
#if !UE_BUILD_SHIPPING
			check(IsInGameThread());
			DLSSErrorState.bIsDLSSModeUnsupported = !IsDLSSModeSupported(DLSSMode);
			DLSSErrorState.InvalidDLSSMode = DLSSMode;
#endif 
		}
	}
	else
	{
#if !UE_BUILD_SHIPPING
		FFrame::KismetExecutionMessage(*FString::Printf(
			TEXT("SetDLSSMode should not be called with an invalid DLSSMode enum value (%d) \"%s\""), 
			int64(DLSSMode), *StaticEnum<UDLSSMode>()->GetDisplayNameTextByValue(int64(DLSSMode)).ToString()),
			ELogVerbosity::Error, SetDLSSModeInvalidEnumValueError);
#endif 
	}
#endif	// WITH_DLSS
}

// deprecated
UDLSSMode UDLSSLibrary::GetDLSSMode()
{
#if WITH_DLSS
	if (!TryInitDLSSLibrary())
	{
		UE_LOG(LogDLSSBlueprint, Error, TEXT("GetDLSSMode should not be called before PostEngineInit"));
		return UDLSSMode::Off;
	}

	static const auto CVarTemporalAAUpscaler = IConsoleManager::Get().FindConsoleVariable(TEXT("r.TemporalAA.Upscaler"));
	const bool bTemporalUpscalerActive = CVarTemporalAAUpscaler && CVarTemporalAAUpscaler->GetInt() != 0;

	const bool bDLSSSupported = (DLSSSRSupport == UDLSSSupport::Supported);

	static const auto CVarDLSSEnable = IConsoleManager::Get().FindConsoleVariable(TEXT("r.NGX.DLSS.Enable"));
	const bool bDLSSEnabled = CVarDLSSEnable && CVarDLSSEnable->GetInt();

	if (bDLAAEnabledDeprecated)
	{
		// DLSS is mutually exclusive with DLAA, DLAA wins
		return UDLSSMode::Off;
	}
	else if (bTemporalUpscalerActive && bDLSSSupported && bDLSSEnabled)
	{
		return CurrentDLSSModeDeprecated;
	}
#endif
	return UDLSSMode::Off;
}

#ifndef ENGINE_CAN_SUPPORT_NIS_PLUGIN
#define ENGINE_CAN_SUPPORT_NIS_PLUGIN 1
#endif

// deprecated
void UDLSSLibrary::SetDLSSSharpness(float Sharpness)
{
#if WITH_DLSS

	if (!TryInitDLSSLibrary())
	{
		UE_LOG(LogDLSSBlueprint, Error, TEXT("SetDLSSSharpness should not be called before PostEngineInit"));
		return;
	}
	static const auto CVarNGXDLSSharpness = IConsoleManager::Get().FindConsoleVariable(TEXT("r.NGX.DLSS.Sharpness"));
	if (CVarNGXDLSSharpness)
	{
#if ENGINE_CAN_SUPPORT_NIS_PLUGIN
		static const auto CVarNISSharpness = IConsoleManager::Get().FindConsoleVariable(TEXT("r.NIS.Sharpness"));
		static const auto CVarNISEnable = IConsoleManager::Get().FindConsoleVariable(TEXT("r.NIS.Enable"));
		const bool bHasNISPlugin = CVarNISSharpness != nullptr && CVarNISEnable != nullptr;
		const bool bIsNISEnabled = bHasNISPlugin && CVarNISEnable->GetBool();

		if (bHasNISPlugin && bIsNISEnabled)
		{
			const int32 PreferNISSharpen = CVarNGXDLSSPreferNISSharpen.GetValueOnAnyThread();
			const bool bUseNISSharpen = PreferNISSharpen == 1 || (PreferNISSharpen == 2 && Sharpness > 0);
			if (bUseNISSharpen)
			{
				Sharpness = FMath::Clamp(Sharpness, 0.0f, 1.0f);
			}
			// Quantize here so we can have sharpness snap to 0, which downstream is used to turn off the NGX sharpening flag
			// CVarNGXDLSSharpness->Set(Sharpness, ECVF_SetByCommandline)  internally uses	Set(*FString::Printf(TEXT("%g"), InValue), SetBy);
			CVarNGXDLSSharpness->Set(*FString::Printf(TEXT("%2.2f"), bUseNISSharpen ? 0.0f : Sharpness), ECVF_SetByCommandline);
			CVarNISSharpness->Set(   *FString::Printf(TEXT("%2.2f"), bUseNISSharpen ? Sharpness : 0.0f), ECVF_SetByCommandline);
		}
		else
#endif	// ENGINE_CAN_SUPPORT_NIS_PLUGIN
		{
			// Quantize here so we can have sharpness snap to 0, which downstream is used to turn off the NGX sharpening flag
			// CVarNGXDLSSharpness->Set(Sharpness, ECVF_SetByCommandline)  internally uses	Set(*FString::Printf(TEXT("%g"), InValue), SetBy);
			CVarNGXDLSSharpness->Set(*FString::Printf(TEXT("%2.2f"), Sharpness), ECVF_SetByCommandline);
		}
	}
#endif
}

// deprecated
float UDLSSLibrary::GetDLSSSharpness()
{
#if WITH_DLSS
	if (!TryInitDLSSLibrary())
	{
		UE_LOG(LogDLSSBlueprint, Error, TEXT("GetDLSSSharpness should not be called before PostEngineInit"));
		return 0.0f;
	}

	static const auto CVarNGXDLSSharpness = IConsoleManager::Get().FindConsoleVariable(TEXT("r.NGX.DLSS.Sharpness"));
	
	if (CVarNGXDLSSharpness)
	{
#if ENGINE_CAN_SUPPORT_NIS_PLUGIN
		static const auto CVarNISSharpness = IConsoleManager::Get().FindConsoleVariable(TEXT("r.NIS.Sharpness"));
		static const auto CVarNISEnable = IConsoleManager::Get().FindConsoleVariable(TEXT("r.NIS.Enable"));
		const bool bHasNISPlugin = CVarNISSharpness != nullptr && CVarNISEnable != nullptr;
		const bool bIsNISEnabled = bHasNISPlugin && CVarNISEnable->GetBool();
		const int32 PreferNISSharpen = CVarNGXDLSSPreferNISSharpen.GetValueOnAnyThread();
		if (bHasNISPlugin && bIsNISEnabled && (PreferNISSharpen == 1))
		{
			return CVarNISSharpness->GetFloat();
		}
		else if (bHasNISPlugin && bIsNISEnabled && (PreferNISSharpen == 2))
		{
			const float DLSSSharpness = CVarNGXDLSSharpness->GetFloat();
			if (DLSSSharpness < 0)
			{
				return DLSSSharpness;
			}
			else
			{
				return CVarNISSharpness->GetFloat();
			}
		}
		else
#endif
		{
			return CVarNGXDLSSharpness->GetFloat();
		}
	}
#endif

	return 0.0f;
}

UDLSSMode UDLSSLibrary::GetDefaultDLSSMode()
{
#if WITH_DLSS
	if (!TryInitDLSSLibrary())
	{
		UE_LOG(LogDLSSBlueprint, Error, TEXT("GetDefaultDLSSMode should not be called before PostEngineInit"));
		return UDLSSMode::Off;
	}
#endif
	if (UDLSSLibrary::IsDLSSSupported())
	{
		return UDLSSMode::Quality;
	}
	else
	{
		return UDLSSMode::Off;
	}
}

#if WITH_DLSS
static UDLSSSupport ToUDLSSSupport(EDLSSSupport InDLSSSupport)
{
	switch (InDLSSSupport)
	{
		case EDLSSSupport::Supported:
			return UDLSSSupport::Supported;

		// this should be resolved at development time
		default:
			checkf(false, TEXT("ToUDLSSSupport should not be called with an out of range InDLSSSupport from the higher level code"));
		case EDLSSSupport::NotSupported:
			return UDLSSSupport::NotSupported;

		case EDLSSSupport::NotSupportedIncompatibleHardware:
			return UDLSSSupport::NotSupportedIncompatibleHardware;
		case EDLSSSupport::NotSupportedDriverOutOfDate:
			return UDLSSSupport::NotSupportedDriverOutOfDate;
		case EDLSSSupport::NotSupportedOperatingSystemOutOfDate:
			return UDLSSSupport::NotSupportedOperatingSystemOutOfDate;
		case EDLSSSupport::NotSupportedIncompatibleAPICaptureToolActive:
			return UDLSSSupport::NotSupportedIncompatibleAPICaptureToolActive;
	}
}

// Delayed initialization, which allows this module to be available early so blueprints can be loaded before DLSS is available in PostEngineInit
bool UDLSSLibrary::TryInitDLSSLibrary()
{
	if (bDLSSLibraryInitialized)
	{
		return true;
	}

	// Register this before we bail out so we can show error messages
#if !UE_BUILD_SHIPPING
	if (!DLSSOnScreenMessagesDelegateHandle.IsValid())
	{
		DLSSOnScreenMessagesDelegateHandle = FCoreDelegates::OnGetOnScreenMessages.AddStatic(&GetDLSSOnScreenMessages);
	}
#endif

	IDLSSModuleInterface* DLSSModule = FModuleManager::GetModulePtr<IDLSSModuleInterface>(TEXT("DLSS"));
	if (DLSSModule == nullptr)
	{
		return false;
	}

	DLSSUpscaler = DLSSModule->GetDLSSUpscaler();
	DLSSSRSupport = ToUDLSSSupport(DLSSModule->QueryDLSSSRSupport());
	DLSSRRSupport = ToUDLSSSupport(DLSSModule->QueryDLSSRRSupport());
	DLSSModule->GetDLSSSRMinDriverVersion(MinDLSSSRDriverVersionMajor, MinDLSSSRDriverVersionMinor);
	DLSSModule->GetDLSSRRMinDriverVersion(MinDLSSRRDriverVersionMajor, MinDLSSRRDriverVersionMinor);

	checkf((DLSSModule->GetDLSSUpscaler() != nullptr) || (DLSSModule->QueryDLSSSRSupport() != EDLSSSupport::Supported), TEXT("mismatch between not having a valid DLSSModule->GetDLSSUpscaler() while also reporting DLSS as being supported by DLSSModule->QueryDLSSSRSupport() %u "), DLSSModule->QueryDLSSSRSupport());

	bDLSSLibraryInitialized = true;

	return true;
}
#endif // WITH_DLSS

void FDLSSBlueprintModule::StartupModule()
{
#if WITH_DLSS
	// This initialization will likely not succeed unless this module has been moved to PostEngineInit, and that's ok
	UDLSSLibrary::TryInitDLSSLibrary();
#else
	UE_LOG(LogDLSSBlueprint, Log, TEXT("DLSS is not supported on this platform at build time. The DLSS Blueprint library however is supported and stubbed out to ignore any calls to enable DLSS and will always return UDLSSSupport::NotSupportedByPlatformAtBuildTime, regardless of the underlying hardware. This can be used to e.g. to turn off DLSS related UI elements."));
	UDLSSLibrary::DLSSSRSupport = UDLSSSupport::NotSupportedByPlatformAtBuildTime;
#endif
}

void FDLSSBlueprintModule::ShutdownModule()
{
#if WITH_DLSS && !UE_BUILD_SHIPPING
	if (UDLSSLibrary::DLSSOnScreenMessagesDelegateHandle.IsValid())
	{
		FCoreDelegates::OnGetOnScreenMessages.Remove(UDLSSLibrary::DLSSOnScreenMessagesDelegateHandle);
		UDLSSLibrary::DLSSOnScreenMessagesDelegateHandle.Reset();
	}
#endif
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDLSSBlueprintModule, DLSSBlueprint)

