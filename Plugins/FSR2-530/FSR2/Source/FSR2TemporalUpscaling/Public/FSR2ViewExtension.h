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
#pragma once

#include "SceneViewExtension.h"

class FSR2TEMPORALUPSCALING_API FFSR2ViewExtension final : public FSceneViewExtensionBase
{
public:
	FFSR2ViewExtension(const FAutoRegister& AutoRegister);

	// ISceneViewExtension interface
	void SetupViewFamily(FSceneViewFamily& InViewFamily) override;
	void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {}
		
	void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override;
	void PreRenderViewFamily_RenderThread(FRDGBuilder& GraphBuilder, FSceneViewFamily& InViewFamily) override;
	void PreRenderView_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& InView) override;
	void PostRenderViewFamily_RenderThread(FRDGBuilder& GraphBuilder, FSceneViewFamily& InViewFamily) override;
	void PrePostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessingInputs& Inputs) override;

private:
	int32 PreviousFSR2State;
	int32 PreviousFSR2StateRT;
	int32 CurrentFSR2StateRT;
	float MinAutomaticViewMipBiasMin;
	float MinAutomaticViewMipBiasOffset;
	int32 VertexDeformationOutputsVelocity;
	int32 VelocityEnableLandscapeGrass;
	int32 BasePassForceOutputsVelocity;
	int32 SeparateTranslucency;
	int32 SSRExperimentalDenoiser;
};