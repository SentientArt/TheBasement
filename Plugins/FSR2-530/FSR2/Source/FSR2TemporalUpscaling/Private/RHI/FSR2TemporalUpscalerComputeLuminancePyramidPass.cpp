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
#include "FSR2TemporalUpscalerShaders.h"
#include "FSR2TemporalUpscalerSubPass.h"
#include "FSR2TemporalUpscalerRHIBackend.h"
#include "ShaderParameterStruct.h"
#include "ShaderCompilerCore.h"

class FFSR2ComputeLuminancePyramidCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FFSR2ComputeLuminancePyramidCS);
	SHADER_USE_PARAMETER_STRUCT(FFSR2ComputeLuminancePyramidCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_REF(FFSR2PassParameters, cbFSR2)
		SHADER_PARAMETER_STRUCT_REF(FFSR2ComputeLuminanceParameters, cbSPD)
		SHADER_PARAMETER_SAMPLER(SamplerState, s_LinearClamp)
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, r_input_color_jittered)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, rw_spd_global_atomic)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, rw_img_mip_shading_change)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, rw_img_mip_5)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, rw_auto_exposure)
	END_SHADER_PARAMETER_STRUCT()

	using FPermutationDomain = FFSR2GlobalShader::FPermutationDomain;

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return FFSR2GlobalShader::ShouldCompilePermutation(Parameters);
	}
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FFSR2GlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

		bool const bWaveOps = FDataDrivenShaderPlatformInfo::GetSupportsWaveOperations(Parameters.Platform) == ERHIFeatureSupport::RuntimeGuaranteed;
		if (!bWaveOps)
		{
			OutEnvironment.SetDefine(TEXT("SPD_NO_WAVE_OPERATIONS"), TEXT("1"));
		}
		else
		{
			OutEnvironment.CompilerFlags.Add(CFLAG_WaveOperations);
		}
		OutEnvironment.CompilerFlags.Add(CFLAG_PreferFlowControl);
		OutEnvironment.SetDefine(TEXT("FFX_SHADER_MODEL_5"), TEXT("1"));
	}
	static uint32* GetBoundSRVs()
	{
		static uint32 SRVs[] = {
			FFX_FSR2_RESOURCE_IDENTIFIER_INPUT_COLOR
		};
		return SRVs;
	}
	static uint32* GetBoundUAVs()
	{
		static uint32 UAVs[] = {
			FFX_FSR2_RESOURCE_IDENTIFIER_SPD_ATOMIC_COUNT,
			FFX_FSR2_RESOURCE_IDENTIFIER_SCENE_LUMINANCE_MIPMAP_SHADING_CHANGE,
			FFX_FSR2_RESOURCE_IDENTIFIER_SCENE_LUMINANCE_MIPMAP_5,
			FFX_FSR2_RESOURCE_IDENTIFIER_AUTO_EXPOSURE
		};
		return UAVs;
	}
	static uint32 GetNumBoundSRVs()
	{
		return 1;
	}
	static uint32 GetNumBoundUAVs()
	{
		return 4;
	}
	static uint32 GetNumConstants()
	{
		return 2;
	}
	static uint32* GetBoundCBs()
	{
		static uint32 CBs[] = { FFX_FSR2_CONSTANTBUFFER_IDENTIFIER_FSR2, FFX_FSR2_CONSTANTBUFFER_IDENTIFIER_SPD };
		return CBs;
	}
	static uint32 GetConstantSizeInDWords(uint32 Index)
	{
		static uint32 Sizes[] = { sizeof(FFSR2PassParameters) / sizeof(uint32), sizeof(FFSR2ComputeLuminanceParameters) / sizeof(uint32) };
		return Sizes[Index];
	}
	static void BindParameters(FRDGBuilder& GraphBuilder, FFSR2BackendState* Context, const FfxGpuJobDescription* job, FParameters* Parameters)
	{
		for (uint32 i = 0; i < job->computeJobDescriptor.pipeline.constCount; i++)
		{
			switch (job->computeJobDescriptor.pipeline.cbResourceBindings[i].resourceIdentifier)
			{
				case FFX_FSR2_CONSTANTBUFFER_IDENTIFIER_FSR2:
				{
					FFSR2PassParameters Buffer;
					FMemory::Memcpy(&Buffer, job->computeJobDescriptor.cbs[i].data, sizeof(FFSR2PassParameters));
					Parameters->cbFSR2 = TUniformBufferRef<FFSR2PassParameters>::CreateUniformBufferImmediate(Buffer, UniformBuffer_SingleDraw);
					break;
				}
				case FFX_FSR2_CONSTANTBUFFER_IDENTIFIER_SPD:
				{
					FFSR2ComputeLuminanceParameters Buffer;
					FMemory::Memcpy(&Buffer, job->computeJobDescriptor.cbs[1].data, sizeof(FFSR2ComputeLuminanceParameters));
					Parameters->cbSPD = TUniformBufferRef<FFSR2ComputeLuminanceParameters>::CreateUniformBufferImmediate(Buffer, UniformBuffer_SingleDraw);
					break;
				}
				default:
				{
					break;
				}
			}
		}

		for (uint32 i = 0; i < job->computeJobDescriptor.pipeline.srvCount; i++)
		{
			switch (job->computeJobDescriptor.pipeline.srvResourceBindings[i].resourceIdentifier)
			{
				case FFX_FSR2_RESOURCE_IDENTIFIER_INPUT_COLOR:
				{
					Parameters->r_input_color_jittered = Context->GetRDGTexture(GraphBuilder, job->computeJobDescriptor.srvs[i].internalIndex);
					break;
				}
				default:
				{
					break;
				}
			}
		}

		uint32* BoundUAVs = GetBoundUAVs();
		for (uint32 i = 0; i < job->computeJobDescriptor.pipeline.uavCount; i++)
		{
			switch (BoundUAVs[i])
			{
				case FFX_FSR2_RESOURCE_IDENTIFIER_SPD_ATOMIC_COUNT:
				{
					Parameters->rw_spd_global_atomic = GraphBuilder.CreateUAV(Context->GetRDGTexture(GraphBuilder, job->computeJobDescriptor.uavs[i].internalIndex));
					break;
				}
				case FFX_FSR2_RESOURCE_IDENTIFIER_SCENE_LUMINANCE_MIPMAP_SHADING_CHANGE:
				{
					FRDGTextureUAVDesc UavDesc;
					UavDesc.Texture = Context->GetRDGTexture(GraphBuilder, job->computeJobDescriptor.uavs[i].internalIndex);
					UavDesc.MipLevel = job->computeJobDescriptor.uavMip[i];
					Parameters->rw_img_mip_shading_change = GraphBuilder.CreateUAV(UavDesc);
					break;
				}
				case FFX_FSR2_RESOURCE_IDENTIFIER_SCENE_LUMINANCE_MIPMAP_5:
				{
					FRDGTextureUAVDesc UavDesc;
					UavDesc.Texture = Context->GetRDGTexture(GraphBuilder, job->computeJobDescriptor.uavs[i].internalIndex);
					UavDesc.MipLevel = job->computeJobDescriptor.uavMip[i];
					Parameters->rw_img_mip_5 = GraphBuilder.CreateUAV(UavDesc);
				}
				case FFX_FSR2_RESOURCE_IDENTIFIER_AUTO_EXPOSURE:
				{
					Parameters->rw_auto_exposure = GraphBuilder.CreateUAV(Context->GetRDGTexture(GraphBuilder, job->computeJobDescriptor.uavs[i].internalIndex));
					break;
				}
				default:
				{
					break;
				}
			}
		}

		Parameters->s_LinearClamp = TStaticSamplerState<SF_Bilinear>::GetRHI();
	}
};
IMPLEMENT_GLOBAL_SHADER(FFSR2ComputeLuminancePyramidCS, "/Plugin/FSR2/Private/ffx_fsr2_compute_luminance_pyramid_pass.usf", "CS", SF_Compute);

IFSR2SubPass* GetComputeLuminancePyramidPass(FfxFsr2Pass passId, const FfxPipelineDescription* desc, FfxPipelineState* outPipeline, bool bSupportHalf)
{
	return new TFSR2SubPass<FFSR2ComputeLuminancePyramidCS>(TEXT("FidelityFX-FSR2/ComputeLuminancePyramid (CS)"), passId, desc, outPipeline, bSupportHalf);
}
