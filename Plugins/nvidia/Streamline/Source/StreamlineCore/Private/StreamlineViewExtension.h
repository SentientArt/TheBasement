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
#include "ScreenPass.h"
#include "PostProcess/PostProcessMaterial.h"
#include "SceneViewExtension.h"
#include "Runtime/Launch/Resources/Version.h"

class FSceneTextureParameters;
class FRHITexture;
class FStreamlineRHI;
class FStreamlineViewExtension final : public FSceneViewExtensionBase
{
public:
	FStreamlineViewExtension(const FAutoRegister& AutoRegister, FStreamlineRHI* InStreamlineRHI);

	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override;
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override;
	virtual void SetupViewPoint(APlayerController* Player, FMinimalViewInfo& InViewInfo) override;
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override;

#if ENGINE_MAJOR_VERSION == 4
	virtual void PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView) final {}
	virtual void PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily) final {};
#endif

	/**
	* This will be called at the beginning of post processing to make sure that each view extension gets a chance to subscribe to an after pass event.
	*/
	virtual void SubscribeToPostProcessingPass(EPostProcessingPass Pass, FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled) override;
private:
	FScreenPassTexture PostProcessPassAtEnd_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& InOutInputs);
	
	FStreamlineRHI* StreamlineRHIExtensions;

};
