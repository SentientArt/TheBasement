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

#pragma once

#include "CoreMinimal.h"
#include "RendererInterface.h"
#include "Runtime/Launch/Resources/Version.h"
#include "SceneViewExtension.h"

#include "DLSSUpscaler.h"
#include "NGXRHI.h"

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
#include "TemporalUpscaler.h"
using ITemporalUpscaler = UE::Renderer::Private::ITemporalUpscaler;
#else
#include "PostProcess/TemporalAA.h"
#endif

DECLARE_LOG_CATEGORY_EXTERN(LogDLSS, Verbose, All);

class FDLSSUpscaler;
struct FTemporalAAHistory;

struct FDLSSPassParameters
{
	FIntRect InputViewRect;
	FIntRect OutputViewRect;
	FVector2f TemporalJitterPixels;
	float PreExposure;
	bool bHighResolutionMotionVectors = false;
	ENGXDLSSDenoiserMode DenoiserMode = ENGXDLSSDenoiserMode::Off;

	FRDGTexture* SceneColorInput = nullptr;
	FRDGTexture* SceneVelocityInput = nullptr;
	FRDGTexture* SceneDepthInput = nullptr;

	// Used by denoisers
	FRDGTexture* DiffuseAlbedo = nullptr;
	FRDGTexture* SpecularAlbedo = nullptr;
	FRDGTexture* Normal = nullptr;
	FRDGTexture* Roughness = nullptr;

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
	FRDGTexture* EyeAdaptation = nullptr;

	FDLSSPassParameters(const ITemporalUpscaler::FInputs& PassInputs)
		: InputViewRect(PassInputs.SceneColor.ViewRect)
		, OutputViewRect(PassInputs.OutputViewRect)
		, TemporalJitterPixels(PassInputs.TemporalJitterPixels)
		, PreExposure(PassInputs.PreExposure)
		, SceneColorInput(PassInputs.SceneColor.Texture)
		, SceneDepthInput(PassInputs.SceneDepth.Texture)
		, EyeAdaptation(PassInputs.EyeAdaptationTexture)
#else
	FDLSSPassParameters(const FViewInfo& View, const ITemporalUpscaler::FPassInputs& PassInputs)
		: InputViewRect(View.ViewRect)
		, OutputViewRect(FIntPoint::ZeroValue, View.GetSecondaryViewRectSize())
		, TemporalJitterPixels(View.TemporalJitterPixels)
		, PreExposure(View.PreExposure)
		, SceneColorInput(PassInputs.SceneColorTexture)
		, SceneDepthInput(PassInputs.SceneDepthTexture)
#endif
	{
	}

	/** Returns the texture resolution that will be output. */
	FIntPoint GetOutputExtent() const;

	/** Validate the settings of TAA, to make sure there is no issue. */
	bool Validate() const;
};

struct FDLSSOutputs
{
	FRDGTexture* SceneColor = nullptr;
};

class DLSS_API FDLSSUpscalerViewExtension final : public FSceneViewExtensionBase
{
public:
	FDLSSUpscalerViewExtension(const FAutoRegister& AutoRegister) : FSceneViewExtensionBase(AutoRegister)
	{ }

	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {}
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {}
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override;
	virtual void PreRenderView_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& InView) final override {}
	virtual void PreRenderViewFamily_RenderThread(FRDGBuilder& GraphBuilder, FSceneViewFamily& InViewFamily) final override {}
	virtual bool IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const override;
};

class DLSS_API FDLSSSceneViewFamilyUpscaler final : public ITemporalUpscaler
{
public:
	FDLSSSceneViewFamilyUpscaler(const FDLSSUpscaler* InUpscaler, EDLSSQualityMode InDLSSQualityMode)
		: Upscaler(InUpscaler)
		, DLSSQualityMode(InDLSSQualityMode)
	{ }

	virtual const TCHAR* GetDebugName() const final override;
	virtual float GetMinUpsampleResolutionFraction() const final override;
	virtual float GetMaxUpsampleResolutionFraction() const final override;
	virtual ITemporalUpscaler* Fork_GameThread(const class FSceneViewFamily& ViewFamily) const final override;
	virtual ITemporalUpscaler::FOutputs AddPasses(
		FRDGBuilder& GraphBuilder,
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
		const FSceneView& View,
		const ITemporalUpscaler::FInputs& PassInputs
#else
		const FViewInfo& View,
		const FPassInputs& PassInputs
#endif
	) const final override;

	static bool IsDLSSTemporalUpscaler(const ITemporalUpscaler* TemporalUpscaler);

private:
	const FDLSSUpscaler* Upscaler;
	const EDLSSQualityMode DLSSQualityMode;

	FDLSSOutputs AddDLSSPass(
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
	) const;
};

