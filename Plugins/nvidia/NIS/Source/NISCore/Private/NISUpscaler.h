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

#pragma once

#include "CoreMinimal.h"
#include "Misc/CoreDelegates.h"
#include "RendererInterface.h"
#include "PostProcess/PostProcessUpscale.h"
#include "SceneViewExtension.h"
#include "NISShaders.h"

class FSceneTextureParameters;
class FRHITexture;
class FNISViewExtension final : public FSceneViewExtensionBase
{
public:
	FNISViewExtension(const FAutoRegister& AutoRegister);

	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override;
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override;
	virtual void SetupViewPoint(APlayerController* Player, FMinimalViewInfo& InViewInfo) override;
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override;

	virtual void PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView) final override {}
	virtual void PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily) final override {}
};



class NISCORE_API FNVImageUpscaler final : public ISpatialUpscaler
{
	friend class FNISModule;
public:
	FNVImageUpscaler();
	virtual ~FNVImageUpscaler();


	virtual const TCHAR* GetDebugName() const override;

	/** Create a new ISpatialUpscaler interface for a new view family. */
	virtual ISpatialUpscaler* Fork_GameThread(const class FSceneViewFamily& ViewFamily) const override;


	// Inherited via ISpatialUpscaler
	virtual FScreenPassTexture AddPasses(
		FRDGBuilder& GraphBuilder,
		const FViewInfo& View,
		const FInputs& PassInputs) const override;

	struct FNISErrorState
	{
		bool bOtherSpatialUpscalerActive = false;
		FString IncompatibleUpscalerCVarNames;

		bool bPrimaryAndSecondarySpatialUpscaling = false;
	};

#if !UE_BUILD_SHIPPING
	static void GetOnScreenMessages(TMultiMap<FCoreDelegates::EOnScreenMessageSeverity, FText>& OutMessages);
	static FDelegateHandle OnScreenMessagesDelegateHandle;
#endif
	static void RegisterOnScreenMessageHandler();
	static void RemoveOnScreenMessageHandler();
	static FNISErrorState ErrorState;
};
