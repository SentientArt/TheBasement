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

#include "NISLibrary.h"
#include "NISShaders.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "ShaderCore.h"
#include "PostProcess/TemporalAA.h"

#include "Runtime/Launch/Resources/Version.h"

#define LOCTEXT_NAMESPACE "FNISBlueprintModule"
DEFINE_LOG_CATEGORY_STATIC(LogNISBlueprint, Log, All);

static const FName SetNISModeInvalidEnumValueError= FName("SetNISModeInvalidEnumValueError");
static const FName IsNISModeSupportedInvalidEnumValueError = FName("IsNISModeSupportedInvalidEnumValueError");

UNISSupport UNISLibrary::NISSupport = UNISSupport::Supported;

FNISUpscaler* UNISLibrary::NISUpscaler = nullptr;

float UNISLibrary::SavedCustomScreenPercentage = 100.0f;

bool UNISLibrary::bIsCustomMode = false;


static TAutoConsoleVariable<int32> CVarNISUpscalingAutomaticMipMapLODBias(
	TEXT("r.NIS.Upscaling.AutomaticMipMapLODBias"),
	1,
	TEXT("Enable automatic setting of r.MipMapLODBias based on the effective NIS screen percentage (default=1)\n")
	TEXT("NOTE: This is only applied when using the UNISLibrary::SetNISMode blueprint function."),
	ECVF_RenderThreadSafe);

static TAutoConsoleVariable<float> CVarNISUpscalingAutomaticMipMapLODBiasOffset(
	TEXT("r.NIS.Upscaling.AutomaticMipMapLODBias.Offset"),
	-0.3f,
	TEXT("Allows offsetting the automatic resolution dependent mip map LOD bias by this amount (default=0)\n")
	TEXT("NOTE: This is only applied when using the UNISLibrary::SetNISMode blueprint function."),
	ECVF_RenderThreadSafe);

bool UNISLibrary::IsNISModeSupported(UNISMode NISMode)
{
	return true;
}

void UNISLibrary::GetNISScreenPercentageRange(float& MinScreenPercentage, float& MaxScreenPercentage)
{
	if (IsNISSupported())
	{
		MinScreenPercentage = 100.0f * 0.5f;
		MaxScreenPercentage = 100.0f * 1.0f;
	}
	else
	{
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
		MinScreenPercentage = 100.0f * ISceneViewFamilyScreenPercentage::kMinTAAUpsampleResolutionFraction;
		MaxScreenPercentage = 100.0f * ISceneViewFamilyScreenPercentage::kMaxTAAUpsampleResolutionFraction;
#else
		MinScreenPercentage = 100.0f * ITemporalUpscaler::GetDefaultTemporalUpscaler()->GetMinUpsampleResolutionFraction();
		MaxScreenPercentage = 100.0f * ITemporalUpscaler::GetDefaultTemporalUpscaler()->GetMaxUpsampleResolutionFraction();
#endif
	}
}

TArray<UNISMode> UNISLibrary::GetSupportedNISModes()
{
	TArray<UNISMode> SupportedQualityModes;
	{
		const UEnum* Enum = StaticEnum<UNISMode>();
		for (int32 EnumIndex = 0; EnumIndex < Enum->NumEnums(); ++EnumIndex)
		{
			const int64 EnumValue = Enum->GetValueByIndex(EnumIndex);
			if (EnumValue != Enum->GetMaxEnumValue())
			{
				const UNISMode QualityMode = UNISMode(EnumValue);
				if (IsNISModeSupported(QualityMode))
				{
					SupportedQualityModes.Add(QualityMode);
				}
			}
		}
	}
	return SupportedQualityModes;
}

bool UNISLibrary::IsNISSupported()
{
	return  GMaxRHIFeatureLevel >= GetNISMinRequiredFeatureLevel();
}


float UNISLibrary::GetNISRecommendedScreenPercentage(UNISMode NISMode)
{
	switch (NISMode)
	{
	default:
		checkf(false, TEXT("dear NIS plugin NVIDIA developer, please update this code to handle the new enum values"));
	case UNISMode::Off:
		return 1.0f;
	case UNISMode::UltraQuality:
		return 77.0f;
	case UNISMode::Quality:
		return 66.667f;
	case UNISMode::Balanced:
		return 59.0f;
	case UNISMode::Performance:
		return 50.0;
	case UNISMode::Custom:
		return SavedCustomScreenPercentage;
		break;
	}
}

void UNISLibrary::SetNISMode(UNISMode NISMode)
{
	const UEnum* Enum = StaticEnum<UNISMode>();

	// UEnums are strongly typed, but then one can also cast a byte to an UEnum ...
	if(Enum->IsValidEnumValue(int64(NISMode)) && (Enum->GetMaxEnumValue() != int64(NISMode)))
	{
		static auto CVarNISEnable = IConsoleManager::Get().FindConsoleVariable(TEXT("r.NIS.Enable"));
		static auto CVarNISUpscalingEnable = IConsoleManager::Get().FindConsoleVariable(TEXT("r.NIS.Upscaling"));
		
		// Save whether we're in custom mode so we can do the right thing when custom screen percentage changes
		bIsCustomMode = UNISMode::Custom == NISMode;

		// r.NIS.Enable might be set to 0 via a hotfix so set r.NIS.Enable to 0 too (in case it might come from saved settings)
		const bool bNISEnabled = CVarNISEnable && CVarNISEnable->GetInt();

		if (ensure(CVarNISUpscalingEnable) && ensure(CVarNISEnable))
		{
			const bool bNISUpscalingEnabled = bNISEnabled && ( NISMode != UNISMode::Off);
			CVarNISUpscalingEnable->Set(bNISUpscalingEnabled ? 1 : 0, ECVF_SetByCommandline);

			if(bNISUpscalingEnabled)
			{
				// Temporal upscalers such as DLSS might set this to 1, but we need r.TemporalAA.Upscaler to be 0 for NIS to work.
				static const auto CVarTemporalAAUpscaler = IConsoleManager::Get().FindConsoleVariable(TEXT("r.TemporalAA.Upscaler"));
				CVarTemporalAAUpscaler->SetWithCurrentPriority(bNISUpscalingEnabled ? 0 : 1);

				static const auto CVarTemporalAAUpsampling = IConsoleManager::Get().FindConsoleVariable(TEXT("r.TemporalAA.Upsampling"));
				CVarTemporalAAUpsampling->SetWithCurrentPriority(bNISUpscalingEnabled ? 0 : 1);

				static auto CVarScreenPercentage = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ScreenPercentage"));
				if (ensure(CVarScreenPercentage))
				{
					const float ScreenPercentage = GetNISRecommendedScreenPercentage(NISMode);
					CVarScreenPercentage->SetWithCurrentPriority(ScreenPercentage);

					if (CVarNISUpscalingAutomaticMipMapLODBias.GetValueOnAnyThread())
					{
						static auto CVarMipMapLodBias = IConsoleManager::Get().FindConsoleVariable(TEXT("r.MipMapLODBias"));
						if (ensure(CVarMipMapLodBias))
						{
							const float EffectivePrimaryResolutionFraction = ScreenPercentage * 0.01f;
							const float MipBias = FMath::Log2(EffectivePrimaryResolutionFraction) + CVarNISUpscalingAutomaticMipMapLODBiasOffset.GetValueOnAnyThread();
							CVarMipMapLodBias->SetWithCurrentPriority(MipBias);
						}
					}
				}
			}
		}
	}
	else
	{
#if !UE_BUILD_SHIPPING
		FFrame::KismetExecutionMessage(*FString::Printf(
			TEXT("SetNISMode should not be called with an invalid NISMode enum value (%d) \"%s\""), 
			int64(NISMode), *StaticEnum<UNISMode>()->GetDisplayNameTextByValue(int64(NISMode)).ToString()),
			ELogVerbosity::Error, SetNISModeInvalidEnumValueError);
#endif 
	}
}

void UNISLibrary::SetNISCustomScreenPercentage(float CustomScreenPercentage)
{
	SavedCustomScreenPercentage = CustomScreenPercentage;

	// Custom screen percentage has changed, so if we're in Custom mode we should run the set mode logic again
	if (bIsCustomMode)
	{
		SetNISMode(UNISMode::Custom);
	}
}

void UNISLibrary::SetNISSharpness(float Sharpness)
{
	static const auto CVarNISharpness = IConsoleManager::Get().FindConsoleVariable(TEXT("r.NIS.Sharpness"));
	
	if (CVarNISharpness)
	{
		// Quantize here so we can have sharpness snap to 0, which downstream is used to turn off sharpening
		// CVarNISharpness->Set(Sharpness, ECVF_SetByCommandline)  internally uses	Set(*FString::Printf(TEXT("%g"), InValue), SetBy);
		CVarNISharpness->Set(*FString::Printf(TEXT("%2.2f"), Sharpness), ECVF_SetByCommandline);
	}
}

UNISMode UNISLibrary::GetDefaultNISMode()
{
	if (UNISLibrary::IsNISSupported())
	{
		return UNISMode::UltraQuality;
	}
	else
	{
		return UNISMode::Off;
	}
}

void FNISBlueprintModule::StartupModule()
{
}

void FNISBlueprintModule::ShutdownModule()
{
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FNISBlueprintModule, NISBlueprint)

