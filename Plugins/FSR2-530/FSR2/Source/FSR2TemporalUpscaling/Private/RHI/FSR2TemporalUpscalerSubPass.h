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

#include "FSR2Include.h"
#include "FSR2TemporalUpscalerRHIBackend.h"
#include "RHIResources.h"
#include "RendererInterface.h"
#include "RenderGraphDefinitions.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"

//-------------------------------------------------------------------------------------
// Each FSR2 subpass implements this common interface to hide the internal details
//-------------------------------------------------------------------------------------
class IFSR2SubPass
{
public:
	virtual ~IFSR2SubPass() {}
	virtual void SetupPipeline(FfxFsr2Pass passId, const FfxPipelineDescription* desc, FfxPipelineState* outPipeline, bool bSupportHalf) = 0;
	virtual void Dispatch(FRDGBuilder& GraphBuilder, FFSR2BackendState* Context, const FfxGpuJobDescription* job) = 0;
};

//-------------------------------------------------------------------------------------
// The TFSR2SubPass template reduces the boilerplate required for each pass.
// The template handles binding resources & issuing the pass through RDG.
//-------------------------------------------------------------------------------------
template <typename TShaderClass>
class TFSR2SubPass : public IFSR2SubPass
{
	typename TShaderClass::FPermutationDomain Permutation;
	TCHAR const* Name;

public:
	TFSR2SubPass(TCHAR const* InName, FfxFsr2Pass passId, const FfxPipelineDescription* desc, FfxPipelineState* outPipeline, bool bSupportHalf)
		: Name(InName)
	{
		SetupPipeline(passId, desc, outPipeline, bSupportHalf);
	}

	virtual ~TFSR2SubPass()
	{

	}

	void SetupPipeline(FfxFsr2Pass passId, const FfxPipelineDescription* desc, FfxPipelineState* outPipeline, bool bSupportHalf) override
	{
		Permutation.template Set<FFSR2_IsHDR>(desc->contextFlags & FFX_FSR2_ENABLE_HIGH_DYNAMIC_RANGE);
		Permutation.template Set<FFSR2_MVLowRes>(!(desc->contextFlags & FFX_FSR2_ENABLE_DISPLAY_RESOLUTION_MOTION_VECTORS));
		Permutation.template Set<FFSR2_MVJittered>(desc->contextFlags & FFX_FSR2_ENABLE_MOTION_VECTORS_JITTER_CANCELLATION);
		Permutation.template Set<FFSR2_DepthInverted>(desc->contextFlags & FFX_FSR2_ENABLE_DEPTH_INVERTED);
		Permutation.template Set<FFSR2_DoSharpening>(passId == FFX_FSR2_PASS_ACCUMULATE_SHARPEN);
		Permutation.template Set<FFSR2_UseHalf>(bSupportHalf);

		outPipeline->srvCount = TShaderClass::GetNumBoundSRVs();
		outPipeline->uavCount = TShaderClass::GetNumBoundUAVs();
		outPipeline->constCount = TShaderClass::GetNumConstants();

		for (uint32 i = 0; i < outPipeline->srvCount; i++)
		{
			outPipeline->srvResourceBindings[i].slotIndex = i;
			outPipeline->srvResourceBindings[i].resourceIdentifier = TShaderClass::GetBoundSRVs()[i];
		}

		for (uint32 i = 0; i < outPipeline->uavCount; i++)
		{
			outPipeline->uavResourceBindings[i].slotIndex = i;
			outPipeline->uavResourceBindings[i].resourceIdentifier = TShaderClass::GetBoundUAVs()[i];
		}

		for (uint32 i = 0; i < outPipeline->constCount; i++)
		{
			outPipeline->cbResourceBindings[i].slotIndex = i;
			outPipeline->cbResourceBindings[i].resourceIdentifier = TShaderClass::GetBoundCBs()[i];
		}
	}

	void Dispatch(FRDGBuilder& GraphBuilder, FFSR2BackendState* Context, const FfxGpuJobDescription* job) override
	{
		TShaderMapRef<TShaderClass> ComputeShader(GetGlobalShaderMap(Context->FeatureLevel), Permutation);

		FIntVector DispatchCount(job->computeJobDescriptor.dimensions[0], job->computeJobDescriptor.dimensions[1], job->computeJobDescriptor.dimensions[2]);
		typename TShaderClass::FParameters* Parameters = GraphBuilder.AllocParameters<typename TShaderClass::FParameters>();
		TShaderClass::BindParameters(GraphBuilder, Context, job, Parameters);

		ClearUnusedGraphResources(ComputeShader, Parameters);

		GraphBuilder.AddPass(
			FRDGEventName(Name),
			Parameters,
			ERDGPassFlags::Compute,
			[Parameters, ComputeShader, DispatchCount](FRHIComputeCommandList& RHICmdList)
			{
				FComputeShaderUtils::Dispatch(RHICmdList, ComputeShader, *Parameters, DispatchCount);
			});
	}
};

//-------------------------------------------------------------------------------------
// These functions instantiate the actual passes.
//-------------------------------------------------------------------------------------
extern IFSR2SubPass* GetDepthClipPass(FfxFsr2Pass passId, const FfxPipelineDescription* desc, FfxPipelineState* outPipeline, bool bSupportHalf);
extern IFSR2SubPass* GetReconstructPreviousDepthPass(FfxFsr2Pass passId, const FfxPipelineDescription* desc, FfxPipelineState* outPipeline, bool bSupportHalf);
extern IFSR2SubPass* GetLockPass(FfxFsr2Pass passId, const FfxPipelineDescription* desc, FfxPipelineState* outPipeline, bool bSupportHalf);
extern IFSR2SubPass* GetAccumulatePass(FfxFsr2Pass passId, const FfxPipelineDescription* desc, FfxPipelineState* outPipeline, bool bSupportHalf);
extern IFSR2SubPass* GetRCASPass(FfxFsr2Pass passId, const FfxPipelineDescription* desc, FfxPipelineState* outPipeline, bool bSupportHalf);
extern IFSR2SubPass* GetComputeLuminancePyramidPass(FfxFsr2Pass passId, const FfxPipelineDescription* desc, FfxPipelineState* outPipeline, bool bSupportHalf);
extern IFSR2SubPass* GetAutogenReactiveMaskPass(FfxFsr2Pass passId, const FfxPipelineDescription* desc, FfxPipelineState* outPipeline, bool bSupportHalf);
extern IFSR2SubPass* GetTcrAutogeneratePass(FfxFsr2Pass passId, const FfxPipelineDescription* desc, FfxPipelineState* outPipeline, bool bSupportHalf);
