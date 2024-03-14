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

#include "DLSSDenoiser.h"
#include "DLSSUpscalerPrivate.h"
#include "PostProcess/TemporalAA.h"
#include "SceneTextureParameters.h"
#include "ScenePrivate.h"


static TAutoConsoleVariable<int32> CVarNGXDLSSReflectionsTemporalAA(
	TEXT("r.NGX.DLSS.Reflections.TemporalAA"),
	1,
	TEXT("Apply a temporal AA pass on the denoised reflections"),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<int32> CVarNGXDLSSWaterReflectionsTemporalAA(
	TEXT("r.NGX.DLSS.WaterReflections.TemporalAA"),
	1,
	TEXT("Apply a temporal AA pass on the denoised water reflections"),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<int32> CVarNGXDLSSBuiltInDenoiserOverride(
	TEXT("r.NGX.DLSS.BuiltInDenoiserOverride"),
	-1,
	TEXT("Change what happens to the built-in denoisers\n")
	TEXT("-1: automatic, depending on r.NGX.DLSS.DenoiserMode (default)\n")
	TEXT(" 0: skip all built-in denoising\n")
	TEXT(" 1: use built-in denoising\n")
	,
	ECVF_RenderThreadSafe
);

bool FDLSSDenoiser::SkipDenoiser(const FViewInfo& View) const
{
	static const auto CVarDLSSDenoiserMode = IConsoleManager::Get().FindConsoleVariable(TEXT("r.NGX.DLSS.DenoiserMode"));
	const bool bIsDLSSDenoising = CVarDLSSDenoiserMode && CVarDLSSDenoiserMode->GetInt() != 0;
	const int32 BuiltInDenoiserOverride = CVarNGXDLSSBuiltInDenoiserOverride.GetValueOnRenderThread();
	return IsDLSSActive(View) && bIsDLSSDenoising && BuiltInDenoiserOverride != 1;
}
bool FDLSSDenoiser::IsDLSSActive(const FViewInfo& View) const
{
	return Upscaler->IsDLSSActive() && View.Family && FDLSSSceneViewFamilyUpscaler::IsDLSSTemporalUpscaler(View.Family->GetTemporalUpscalerInterface());
}

FDLSSDenoiser::FDLSSDenoiser(const IScreenSpaceDenoiser* InWrappedDenoiser, const FDLSSUpscaler* InUpscaler)
	: WrappedDenoiser(InWrappedDenoiser)
	, Upscaler(InUpscaler)
{
	check(Upscaler);
	check(WrappedDenoiser);
}

const TCHAR* FDLSSDenoiser::GetDebugName() const
{
	if (Upscaler->IsDLSSActive())
	{
		// we don't have the View here to check whether we have a valid GetTemporalUpscalerInterface, which we'll do when we actually get called to denoise/add TAA
		// and this is only for profilegpu anyways so OK if it's not 100% accurate
		return TEXT("FDLSSDenoiserWrapper(Active)");
	}
	else
	{
		return TEXT("FDLSSDenoiserWrapper(Inactive)");
	}
}

IScreenSpaceDenoiser::EShadowRequirements FDLSSDenoiser::GetShadowRequirements(const FViewInfo& View, const FLightSceneInfo& LightSceneInfo, const FShadowRayTracingConfig& RayTracingConfig) const
{
	return WrappedDenoiser->GetShadowRequirements(View, LightSceneInfo, RayTracingConfig);
}

void FDLSSDenoiser::DenoiseShadowVisibilityMasks(FRDGBuilder& GraphBuilder, const FViewInfo& View, FPreviousViewInfo* PreviousViewInfos, const FSceneTextureParameters& SceneTextures, const TStaticArray<FShadowVisibilityParameters, IScreenSpaceDenoiser::kMaxBatchSize>& InputParameters, const int32 InputParameterCount, TStaticArray<FShadowVisibilityOutputs, IScreenSpaceDenoiser::kMaxBatchSize>& Outputs) const
{
	if (SkipDenoiser(View))
	{
		for (int32 Shadow = 0; Shadow < InputParameterCount; ++Shadow)
		{
			Outputs[Shadow].Mask = InputParameters[Shadow].InputTextures.Mask;
		}
	}
	else
	{

		WrappedDenoiser->DenoiseShadowVisibilityMasks(GraphBuilder, View, PreviousViewInfos, SceneTextures, InputParameters, InputParameterCount, Outputs);
	}
}

IScreenSpaceDenoiser::FPolychromaticPenumbraOutputs FDLSSDenoiser::DenoisePolychromaticPenumbraHarmonics(FRDGBuilder& GraphBuilder, const FViewInfo& View, FPreviousViewInfo* PreviousViewInfos, const FSceneTextureParameters& SceneTextures, const FPolychromaticPenumbraHarmonics& Inputs) const
{
	if (SkipDenoiser(View))
	{
		IScreenSpaceDenoiser::FPolychromaticPenumbraOutputs Outputs;
		Outputs.Diffuse = Inputs.Diffuse.Harmonics[0];
		Outputs.Specular = Inputs.Specular.Harmonics[0];
		return Outputs;
	}
	else
	{
		return WrappedDenoiser->DenoisePolychromaticPenumbraHarmonics(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs);
	}
}

IScreenSpaceDenoiser::FReflectionsOutputs FDLSSDenoiser::DenoiseReflections(FRDGBuilder& GraphBuilder, const FViewInfo& View, FPreviousViewInfo* PreviousViewInfos, const FSceneTextureParameters& SceneTextures, const FReflectionsInputs& Inputs, const FReflectionsRayTracingConfig Config) const
{ 
	if (SkipDenoiser(View))
	{
		FReflectionsOutputs Outputs;
		Outputs.Color = Inputs.Color;
		return Outputs;
	}
	else
	{
		FReflectionsOutputs Outputs = WrappedDenoiser->DenoiseReflections(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs, Config);
		const bool bApplyTemporalAA = IsDLSSActive(View) && CVarNGXDLSSReflectionsTemporalAA.GetValueOnRenderThread() && View.ViewState && IsTemporalAccumulationBasedMethod(View.AntiAliasingMethod);
		if(bApplyTemporalAA)
		{
			check(View.ViewState);

			FTAAPassParameters TAASettings(View);
			TAASettings.Pass = ETAAPassConfig::ScreenSpaceReflections;
			TAASettings.SceneDepthTexture = SceneTextures.SceneDepthTexture;
			TAASettings.SceneVelocityTexture = SceneTextures.GBufferVelocityTexture;
			TAASettings.SceneColorInput = Outputs.Color;
			TAASettings.bOutputRenderTargetable = true;

			FTAAOutputs TAAOutputs = AddTemporalAAPass(
				GraphBuilder,
				View,
				TAASettings,
				View.PrevViewInfo.SSRHistory,
				&View.ViewState->PrevFrameViewInfo.SSRHistory);

			Outputs.Color = TAAOutputs.SceneColor;
		}
		return Outputs;
	}
}

IScreenSpaceDenoiser::FReflectionsOutputs FDLSSDenoiser::DenoiseWaterReflections(FRDGBuilder& GraphBuilder, const FViewInfo& View, FPreviousViewInfo* PreviousViewInfos, const FSceneTextureParameters& SceneTextures, const FReflectionsInputs& Inputs, const FReflectionsRayTracingConfig Config) const
{
	if (SkipDenoiser(View))
	{
		FReflectionsOutputs Outputs;
		Outputs.Color = Inputs.Color;
		return Outputs;
	}
	else
	{
		FReflectionsOutputs Outputs = WrappedDenoiser->DenoiseWaterReflections(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs, Config);
		const bool bApplyTemporalAA = IsDLSSActive(View) && CVarNGXDLSSWaterReflectionsTemporalAA.GetValueOnRenderThread() && View.ViewState && IsTemporalAccumulationBasedMethod(View.AntiAliasingMethod);
		if (bApplyTemporalAA)
		{
			check(View.ViewState);

			FTAAPassParameters TAASettings(View);
			TAASettings.Pass = ETAAPassConfig::ScreenSpaceReflections;
			TAASettings.SceneDepthTexture = SceneTextures.SceneDepthTexture;
			TAASettings.SceneVelocityTexture = SceneTextures.GBufferVelocityTexture;
			TAASettings.SceneColorInput = Outputs.Color;
			TAASettings.bOutputRenderTargetable = true;

			FTAAOutputs TAAOutputs = AddTemporalAAPass(
				GraphBuilder,
				View,
				TAASettings,
				View.PrevViewInfo.WaterSSRHistory,
				&View.ViewState->PrevFrameViewInfo.WaterSSRHistory);

			Outputs.Color = TAAOutputs.SceneColor;
		}

		return Outputs;
	}
}

IScreenSpaceDenoiser::FAmbientOcclusionOutputs FDLSSDenoiser::DenoiseAmbientOcclusion(FRDGBuilder& GraphBuilder, const FViewInfo& View, FPreviousViewInfo* PreviousViewInfos, const FSceneTextureParameters& SceneTextures, const FAmbientOcclusionInputs& Inputs, const FAmbientOcclusionRayTracingConfig Config) const
{
	if (SkipDenoiser(View))
	{
		FAmbientOcclusionOutputs Outputs;
		Outputs.AmbientOcclusionMask = Inputs.Mask;
		return Outputs;
	}
	else
	{
		return WrappedDenoiser->DenoiseAmbientOcclusion(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs, Config);
	}
}

FSSDSignalTextures FDLSSDenoiser::DenoiseDiffuseIndirect(FRDGBuilder& GraphBuilder, const FViewInfo& View, FPreviousViewInfo* PreviousViewInfos, const FSceneTextureParameters& SceneTextures, const FDiffuseIndirectInputs& Inputs, const FAmbientOcclusionRayTracingConfig Config) const
{
	return WrappedDenoiser->DenoiseDiffuseIndirect(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs, Config);
}

FSSDSignalTextures FDLSSDenoiser::DenoiseScreenSpaceDiffuseIndirect(FRDGBuilder& GraphBuilder, const FViewInfo& View, FPreviousViewInfo* PreviousViewInfos, const FSceneTextureParameters& SceneTextures, const FDiffuseIndirectInputs& Inputs, const FAmbientOcclusionRayTracingConfig Config) const
{
	return WrappedDenoiser->DenoiseScreenSpaceDiffuseIndirect(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs, Config);
}

FSSDSignalTextures FDLSSDenoiser::DenoiseDiffuseIndirectHarmonic(FRDGBuilder& GraphBuilder, const FViewInfo& View, FPreviousViewInfo* PreviousViewInfos, const FSceneTextureParameters& SceneTextures, const FDiffuseIndirectHarmonic& Inputs, const HybridIndirectLighting::FCommonParameters& CommonDiffuseParameters) const
{
	return WrappedDenoiser->DenoiseDiffuseIndirectHarmonic(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs, CommonDiffuseParameters);
}

IScreenSpaceDenoiser::FDiffuseIndirectOutputs FDLSSDenoiser::DenoiseSkyLight(FRDGBuilder& GraphBuilder, const FViewInfo& View, FPreviousViewInfo* PreviousViewInfos, const FSceneTextureParameters& SceneTextures, const FDiffuseIndirectInputs& Inputs, const FAmbientOcclusionRayTracingConfig Config) const
{
	if (SkipDenoiser(View))
	{
		FDiffuseIndirectOutputs Outputs;
		Outputs.AmbientOcclusionMask = Inputs.AmbientOcclusionMask;
		Outputs.Color = Inputs.Color;
		return Outputs;
	}
	else
	{
		return WrappedDenoiser->DenoiseSkyLight(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs, Config);
	}
}

IScreenSpaceDenoiser::FDiffuseIndirectOutputs FDLSSDenoiser::DenoiseReflectedSkyLight(FRDGBuilder& GraphBuilder, const FViewInfo& View, FPreviousViewInfo* PreviousViewInfos, const FSceneTextureParameters& SceneTextures, const FDiffuseIndirectInputs& Inputs, const FAmbientOcclusionRayTracingConfig Config) const
{
	if (SkipDenoiser(View))
	{
		FDiffuseIndirectOutputs Outputs;
		Outputs.AmbientOcclusionMask = Inputs.AmbientOcclusionMask;
		Outputs.Color = Inputs.Color;
		return Outputs;
	}
	else
	{
		return WrappedDenoiser->DenoiseReflectedSkyLight(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs, Config);
	}
}


bool FDLSSDenoiser::SupportsScreenSpaceDiffuseIndirectDenoiser(EShaderPlatform Platform) const
{
	return WrappedDenoiser->SupportsScreenSpaceDiffuseIndirectDenoiser(Platform);
}

const IScreenSpaceDenoiser* FDLSSDenoiser::GetWrappedDenoiser() const
{
	return WrappedDenoiser;
}
