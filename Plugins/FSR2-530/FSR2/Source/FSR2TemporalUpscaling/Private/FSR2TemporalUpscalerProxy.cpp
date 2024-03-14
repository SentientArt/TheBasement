// This file is part of the FidelityFX Super Resolution 2.2 Unreal Engine Plugin.
//
// Copyright (c) 2022-2023 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "FSR2TemporalUpscalerProxy.h"
#include "FSR2TemporalUpscaling.h"
#include "FSR2Include.h"
#include "FSR2TemporalUpscalerHistory.h"
#include "RHI/FSR2TemporalUpscalerRHIBackend.h"
#include "FSR2TemporalUpscalingAPI.h"
#include "SceneTextureParameters.h"
#include "TranslucentRendering.h"
#include "ScenePrivate.h"
#include "LogFSR2.h"
#include "LegacyScreenPercentageDriver.h"
#include "PlanarReflectionSceneProxy.h"
#include "ScreenSpaceRayTracing.h"
#include "Serialization/MemoryImage.h"
#include "Serialization/MemoryLayout.h"
#include "FXSystem.h"
#include "PostProcess/SceneRenderTargets.h"


//------------------------------------------------------------------------------------------------------
// FFSR2TemporalUpscalerProxy implementation.
//------------------------------------------------------------------------------------------------------
FFSR2TemporalUpscalerProxy::FFSR2TemporalUpscalerProxy(FFSR2TemporalUpscaler* TemporalUpscaler)
: TemporalUpscaler(TemporalUpscaler)
{
}

FFSR2TemporalUpscalerProxy::~FFSR2TemporalUpscalerProxy()
{
}

const TCHAR* FFSR2TemporalUpscalerProxy::GetDebugName() const
{
	return TemporalUpscaler->GetDebugName();
}

IFSR2TemporalUpscaler::FOutputs FFSR2TemporalUpscalerProxy::AddPasses(
	FRDGBuilder& GraphBuilder,
	const FSceneView& View,
	const FFSR2PassInputs& PassInputs) const
{
	return TemporalUpscaler->AddPasses(GraphBuilder, View, PassInputs);
}

IFSR2TemporalUpscaler* FFSR2TemporalUpscalerProxy::Fork_GameThread(const class FSceneViewFamily& InViewFamily) const
{
	return new FFSR2TemporalUpscalerProxy(TemporalUpscaler);
}

float FFSR2TemporalUpscalerProxy::GetMinUpsampleResolutionFraction() const
{
	return TemporalUpscaler->GetMinUpsampleResolutionFraction();
}

float FFSR2TemporalUpscalerProxy::GetMaxUpsampleResolutionFraction() const
{
	return TemporalUpscaler->GetMinUpsampleResolutionFraction();
}

IScreenSpaceDenoiser::FReflectionsOutputs FFSR2TemporalUpscalerProxy::DenoiseReflections(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FReflectionsInputs& ReflectionInputs,
	const FReflectionsRayTracingConfig RayTracingConfig) const
{
	return TemporalUpscaler->DenoiseReflections(GraphBuilder, View, PreviousViewInfos, SceneTextures, ReflectionInputs, RayTracingConfig);
}

IScreenSpaceDenoiser::EShadowRequirements FFSR2TemporalUpscalerProxy::GetShadowRequirements(
	const FViewInfo& View,
	const FLightSceneInfo& LightSceneInfo,
	const FShadowRayTracingConfig& RayTracingConfig) const
{
	return TemporalUpscaler->GetShadowRequirements(View, LightSceneInfo, RayTracingConfig);
}

void FFSR2TemporalUpscalerProxy::DenoiseShadowVisibilityMasks(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const TStaticArray<FShadowVisibilityParameters, IScreenSpaceDenoiser::kMaxBatchSize>& InputParameters,
	const int32 InputParameterCount,
	TStaticArray<FShadowVisibilityOutputs, IScreenSpaceDenoiser::kMaxBatchSize>& Outputs) const
{

	return TemporalUpscaler->DenoiseShadowVisibilityMasks(GraphBuilder, View, PreviousViewInfos, SceneTextures, InputParameters, InputParameterCount, Outputs);
}

IScreenSpaceDenoiser::FPolychromaticPenumbraOutputs FFSR2TemporalUpscalerProxy::DenoisePolychromaticPenumbraHarmonics(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FPolychromaticPenumbraHarmonics& Inputs) const
{
	return TemporalUpscaler->DenoisePolychromaticPenumbraHarmonics(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs);
}

IScreenSpaceDenoiser::FReflectionsOutputs FFSR2TemporalUpscalerProxy::DenoiseWaterReflections(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FReflectionsInputs& ReflectionInputs,
	const FReflectionsRayTracingConfig RayTracingConfig) const
{
	return TemporalUpscaler->DenoiseWaterReflections(GraphBuilder, View, PreviousViewInfos, SceneTextures, ReflectionInputs, RayTracingConfig);
}

IScreenSpaceDenoiser::FAmbientOcclusionOutputs FFSR2TemporalUpscalerProxy::DenoiseAmbientOcclusion(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FAmbientOcclusionInputs& ReflectionInputs,
	const FAmbientOcclusionRayTracingConfig RayTracingConfig) const
{
	return TemporalUpscaler->DenoiseAmbientOcclusion(GraphBuilder, View, PreviousViewInfos, SceneTextures, ReflectionInputs, RayTracingConfig);
}

FSSDSignalTextures FFSR2TemporalUpscalerProxy::DenoiseDiffuseIndirect(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FDiffuseIndirectInputs& Inputs,
	const FAmbientOcclusionRayTracingConfig Config) const
{
	return TemporalUpscaler->DenoiseDiffuseIndirect(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs, Config);
}

IScreenSpaceDenoiser::FDiffuseIndirectOutputs FFSR2TemporalUpscalerProxy::DenoiseSkyLight(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FDiffuseIndirectInputs& Inputs,
	const FAmbientOcclusionRayTracingConfig Config) const
{
	return TemporalUpscaler->DenoiseSkyLight(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs, Config);
}

IScreenSpaceDenoiser::FDiffuseIndirectOutputs FFSR2TemporalUpscalerProxy::DenoiseReflectedSkyLight(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FDiffuseIndirectInputs& Inputs,
	const FAmbientOcclusionRayTracingConfig Config) const
{
	return TemporalUpscaler->DenoiseReflectedSkyLight(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs, Config);
}

FSSDSignalTextures FFSR2TemporalUpscalerProxy::DenoiseDiffuseIndirectHarmonic(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FDiffuseIndirectHarmonic& Inputs,
	const HybridIndirectLighting::FCommonParameters& CommonDiffuseParameters) const

{
	return TemporalUpscaler->DenoiseDiffuseIndirectHarmonic(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs, CommonDiffuseParameters);
}

bool FFSR2TemporalUpscalerProxy::SupportsScreenSpaceDiffuseIndirectDenoiser(EShaderPlatform Platform) const
{
	return TemporalUpscaler->SupportsScreenSpaceDiffuseIndirectDenoiser(Platform);
}

FSSDSignalTextures FFSR2TemporalUpscalerProxy::DenoiseScreenSpaceDiffuseIndirect(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FDiffuseIndirectInputs& Inputs,
	const FAmbientOcclusionRayTracingConfig Config) const
{
	return TemporalUpscaler->DenoiseScreenSpaceDiffuseIndirect(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs, Config);
}
