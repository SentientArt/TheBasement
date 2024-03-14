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

#include "NISUpscaler.h"
#include "DynamicResolutionState.h"
#include "LegacyScreenPercentageDriver.h"
#include "Runtime/Launch/Resources/Version.h"

#include "NISShaders.h"

#define LOCTEXT_NAMESPACE "FNISModule"

static TAutoConsoleVariable<int32> CVarNISEnable(
	TEXT("r.NIS.Enable"),
	1,
	TEXT("Enable/disable NIS upscaling and/or sharpening"),
	ECVF_RenderThreadSafe);

static TAutoConsoleVariable<int32> CVarNISUpscaling(
	TEXT("r.NIS.Upscaling"),
	1,
	TEXT("Enable NIS Upscaling. Also requires r.TemporalAA.Upscaler 0"),
	ECVF_RenderThreadSafe);

FNVImageUpscaler::FNISErrorState FNVImageUpscaler::ErrorState;


FNISViewExtension::FNISViewExtension(const FAutoRegister& AutoRegister) : FSceneViewExtensionBase(AutoRegister)
{
	FSceneViewExtensionIsActiveFunctor IsActiveFunctor;

	IsActiveFunctor.IsActiveFunction = [](const ISceneViewExtension* SceneViewExtension, const FSceneViewExtensionContext& Context)
	{
		return true;
	};

	IsActiveThisFrameFunctions.Add(IsActiveFunctor);
}

void FNISViewExtension::SetupViewFamily(FSceneViewFamily& InViewFamily) 
{
}

void FNISViewExtension::SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView)
{
}

void FNISViewExtension::SetupViewPoint(APlayerController* Player, FMinimalViewInfo& InViewInfo) 
{
}

void FNISViewExtension::BeginRenderViewFamily(FSceneViewFamily& InViewFamily)
{
	const bool bIsNISSupported = InViewFamily.GetFeatureLevel() >= GetNISMinRequiredFeatureLevel();
	const bool bIsNISEnabled = CVarNISEnable.GetValueOnAnyThread() != 0;
	const bool bIsNISUpscalingEnabled = CVarNISUpscaling.GetValueOnAnyThread() != 0;

	static const auto CVarNISSharpness = IConsoleManager::Get().FindConsoleVariable(TEXT("r.NIS.Sharpness"));
	const bool bIsNISSharpeningEnabled = (CVarNISSharpness ? CVarNISSharpness->GetFloat() : 0.0f) != 0.0f;

	if (bIsNISSupported && bIsNISEnabled && (bIsNISUpscalingEnabled || bIsNISSharpeningEnabled))
	{
		/*	
			FViewFamily::SetPrimarySpatialUpscalerInterface asserts if a plugin tries
			to set spatial upscaler interfaces when another plugin already set it before.

			Ideally the developer has only one spatial upscaler plugin active at run time,
			based on UI setting, gameplay logic and such. However NIS and other spatial 
			upscaler plugins tend to have their respective upscalers set to be active by default, 
			which makes sense overall.
				
			We don't know whether we are gonna be the "first" or "second" (or maybe 3rd in the future ;) )
			in the call order to get a chance to set the spatial upscaler interfaces.
				
			If we are not the first, then we can avoid the assert by not activating NIS, and instead 
			showing an on screen error message (for non shipping builds), alerting the developer on how 
			to avoid the assert/crash in the future, without taking down the app. E.g. they could change 
			cvars to turn off other spatial upscaler plugins.

			Empirical testing though revealed that NIS might get called first, thus potentially causing at least
			one known 3rd party spatial upscaler plugin to trigger the engine side assert in 
			FViewFamily::SetPrimarySpatialUpscalerInterface, which is not great.
		
			Thus we explicitly check whether any known spatial upscaler plugins are active for the current 
			viewfamily/frame and turn off NIS as to not enable other plugins to take down the engine :)

			After all "unreal engine spatial upscaler plugins crash each other" makes for catchy headlines, 
			but is actually not great in practice.  =)
		*/

		struct FConsoleVariableReference
		{
			const TCHAR* Name = nullptr;
			IConsoleVariable* CVar = nullptr;
			bool bInitialized = false;
		};

		static FConsoleVariableReference KnownUpscalerCVars[]
		{
			{TEXT("r.FidelityFX.FSR.Enabled")}
		};

		
		FNVImageUpscaler::ErrorState.IncompatibleUpscalerCVarNames = TEXT("");
		bool bAnyKnownUpscalerActive = false;
		for (auto& UpscalerCVar : KnownUpscalerCVars)
		{
			if (!UpscalerCVar.bInitialized)
			{
				UpscalerCVar.CVar = IConsoleManager::Get().FindConsoleVariable(UpscalerCVar.Name);
				UpscalerCVar.bInitialized = true;
			}

			if (UpscalerCVar.CVar && UpscalerCVar.CVar->GetInt() != 0)
			{
				FNVImageUpscaler::ErrorState.IncompatibleUpscalerCVarNames.Append(UpscalerCVar.Name);
				bAnyKnownUpscalerActive = true;
			}
		}

		const bool bAnyOtherSpatialUpscalerActive =  bAnyKnownUpscalerActive || InViewFamily.GetPrimarySpatialUpscalerInterface() != nullptr || InViewFamily.GetSecondarySpatialUpscalerInterface() != nullptr;
		FNVImageUpscaler::ErrorState.bOtherSpatialUpscalerActive = bAnyOtherSpatialUpscalerActive;
		

		bool bIsSpatialPrimaryUpscaling = false;
		bool bIsTemporalPrimaryUpscaling = false;

		for (const auto& View : InViewFamily.Views)
		{
			if (View)
			{
				if (View->PrimaryScreenPercentageMethod == EPrimaryScreenPercentageMethod::SpatialUpscale)
				{
					bIsSpatialPrimaryUpscaling = true;
				}
				if (View->PrimaryScreenPercentageMethod == EPrimaryScreenPercentageMethod::TemporalUpscale)
				{
					bIsTemporalPrimaryUpscaling = true;
				}
			}
		}

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1
		DynamicRenderScaling::TMap<float> UpperBounds = InViewFamily.GetScreenPercentageInterface()->GetResolutionFractionsUpperBound();
		float PrimaryResolutionFraction = UpperBounds[GDynamicPrimaryResolutionFraction];
#else
		float PrimaryResolutionFraction = InViewFamily.GetPrimaryResolutionFractionUpperBound();
#endif
		const float MAX_UPSCALE_FRACTION = 1.0f;
		const float MIN_UPSCALE_FRACTION = 0.5f;
		const bool bIsActuallyPrimaryUpscaling = PrimaryResolutionFraction < MAX_UPSCALE_FRACTION && PrimaryResolutionFraction >= MIN_UPSCALE_FRACTION;
		const bool bIsActuallySecondaryUpscaling = InViewFamily.SecondaryViewFraction < MAX_UPSCALE_FRACTION && InViewFamily.SecondaryViewFraction >= MIN_UPSCALE_FRACTION;

		FNVImageUpscaler::ErrorState.bPrimaryAndSecondarySpatialUpscaling = bIsSpatialPrimaryUpscaling && bIsActuallyPrimaryUpscaling && bIsActuallySecondaryUpscaling;

		if (!bAnyOtherSpatialUpscalerActive)
		{
			if (bIsNISUpscalingEnabled && bIsSpatialPrimaryUpscaling && bIsActuallyPrimaryUpscaling)
			{
				InViewFamily.SetPrimarySpatialUpscalerInterface(new FNVImageUpscaler());
			}
			// when running with DLSS (or TAAU/TSR) we do either secondary upscaling or just NIS sharpening
			else if(bIsNISSharpeningEnabled || (bIsTemporalPrimaryUpscaling && bIsActuallyPrimaryUpscaling && bIsActuallySecondaryUpscaling))
			{
				InViewFamily.SetSecondarySpatialUpscalerInterface(new FNVImageUpscaler());
			}
		}
	}
}


FNVImageUpscaler::FNVImageUpscaler()
{

}


FNVImageUpscaler::~FNVImageUpscaler()
{

}

const TCHAR* FNVImageUpscaler::GetDebugName() const
{
	return TEXT("NVIDIA Image Upscaler");
}

ISpatialUpscaler* FNVImageUpscaler::Fork_GameThread(const FSceneViewFamily& ViewFamily) const
{
	check(IsInGameThread());
	return new FNVImageUpscaler();
}


FScreenPassTexture FNVImageUpscaler::AddPasses(FRDGBuilder& GraphBuilder, const FViewInfo& View, const ISpatialUpscaler::FInputs& PassInputs) const
{
	return  AddSharpenOrUpscalePass(GraphBuilder, View, PassInputs);
}

static bool ShowNISDebugOnScreenMessages()
{
	return true;
	// TODO do we need project settings for this?
	//if (GetDefault<UDLSSOverrideSettings>()->ShowDLSSSDebugOnScreenMessages == EDLSSSettingOverride::UseProjectSettings)
	//{
	//	return GetDefault<UDLSSSettings>()->bShowDLSSSDebugOnScreenMessages;
	//}
	//else
	//{
	//	return GetDefault<UDLSSOverrideSettings>()->ShowDLSSSDebugOnScreenMessages == EDLSSSettingOverride::Enabled;
	//}
}


#if !UE_BUILD_SHIPPING

FDelegateHandle FNVImageUpscaler::OnScreenMessagesDelegateHandle;
void FNVImageUpscaler::GetOnScreenMessages(TMultiMap<FCoreDelegates::EOnScreenMessageSeverity, FText>& OutMessages)
{
	check(IsInGameThread());

	if (ShowNISDebugOnScreenMessages())
	{
		if (ErrorState.bOtherSpatialUpscalerActive)
		{
			const FTextFormat Format(LOCTEXT("NISOtherUpscalerActive",
				"NIS Error: Disabling NVIDIA NIS as the spatial upscaler since another spatial upscaler plugin is already active for this view family.\n"
				"           To enable NIS, please disable other primary spatial upscalers in the UI/application logic or via console variables {0} {1}. And vice versa"));
			const FText Message = FText::Format(Format, 
				FText::FromString(!ErrorState.IncompatibleUpscalerCVarNames.IsEmpty() ? TEXT("such as") : TEXT("")), 
				FText::FromString(ErrorState.IncompatibleUpscalerCVarNames)
			);
			OutMessages.Add(FCoreDelegates::EOnScreenMessageSeverity::Error, Message);
		}

		if (ErrorState.bPrimaryAndSecondarySpatialUpscaling)
		{
			const FTextFormat Format(LOCTEXT("NISOtherUpscalerActive",
				"NIS Warning: NIS is used as a primary spatial upscaler, followed by the engine built-in secondary spatial upscaler, which is not optimal.\n"
				"             Consider disabling the secondary screen percentage (via r.SecondaryScreenPercentage.GameViewport or Editor.OverrideDPIBasedEditorViewportScaling) in order to have NIS upscale directly to the output resolution."));
			const FText Message = FText::Format(Format,
				FText::FromString(!ErrorState.IncompatibleUpscalerCVarNames.IsEmpty() ? TEXT("such as") : TEXT("")),
				FText::FromString(ErrorState.IncompatibleUpscalerCVarNames)
			);
			OutMessages.Add(FCoreDelegates::EOnScreenMessageSeverity::Warning, Message);
		}
	}
}
#endif


void FNVImageUpscaler::RegisterOnScreenMessageHandler()
{
#if !UE_BUILD_SHIPPING
	OnScreenMessagesDelegateHandle = FCoreDelegates::OnGetOnScreenMessages.AddStatic(&GetOnScreenMessages);
#endif
}

void FNVImageUpscaler::RemoveOnScreenMessageHandler()
{
#if !UE_BUILD_SHIPPING
	if (OnScreenMessagesDelegateHandle.IsValid())
	{
		FCoreDelegates::OnGetOnScreenMessages.Remove(OnScreenMessagesDelegateHandle);
		OnScreenMessagesDelegateHandle.Reset();
	}
#endif
}

#undef LOCTEXT_NAMESPACE
