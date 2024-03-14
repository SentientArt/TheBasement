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

#include "VelocityCombinePass.h"

#include "RenderGraphUtils.h"
#include "Runtime/Launch/Resources/Version.h"
#include "ScreenPass.h"

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 2
#include "DataDrivenShaderPlatformInfo.h"
#endif
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION < 3
#include "ScenePrivate.h"
#endif

const int32 kVelocityCombineComputeTileSizeX = FComputeShaderUtils::kGolden2DGroupSize;
const int32 kVelocityCombineComputeTileSizeY = FComputeShaderUtils::kGolden2DGroupSize;


class FDilateMotionVectorsDim : SHADER_PERMUTATION_BOOL("DILATE_MOTION_VECTORS");

class FVelocityCombineCS : public FGlobalShader
{
public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		// Only cook for the platforms/RHIs where DLSS is supported, which is DX11,DX12 and Vulkan [on Win64]
		return 	IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5) &&
				IsPCPlatform(Parameters.Platform) && (
					IsVulkanPlatform(Parameters.Platform) ||
					IsD3DPlatform(Parameters.Platform));
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEX"), kVelocityCombineComputeTileSizeX);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEY"), kVelocityCombineComputeTileSizeY);
	}
	using FPermutationDomain = TShaderPermutationDomain<FDilateMotionVectorsDim>;

	DECLARE_GLOBAL_SHADER(FVelocityCombineCS);
	SHADER_USE_PARAMETER_STRUCT(FVelocityCombineCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		// Input images
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, VelocityTexture)
		SHADER_PARAMETER_SAMPLER(SamplerState, VelocityTextureSampler)
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, Velocity)

		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, DepthTexture)
		SHADER_PARAMETER_SAMPLER(SamplerState, DepthTextureSampler)

		SHADER_PARAMETER(FVector2f, TemporalJitterPixels)

		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		
		// Output images
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, OutVelocityCombinedTexture)
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, CombinedVelocity)

	END_SHADER_PARAMETER_STRUCT()
};


IMPLEMENT_GLOBAL_SHADER(FVelocityCombineCS, "/Plugin/DLSS/Private/VelocityCombine.usf", "VelocityCombineMain", SF_Compute);

FRDGTextureRef AddVelocityCombinePass(
	FRDGBuilder& GraphBuilder,
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
	const FSceneView& View,
#else
	const FViewInfo& View,
#endif
	FRDGTextureRef InSceneDepthTexture,
	FRDGTextureRef InVelocityTexture,
	FIntRect InputViewRect,
	FIntRect DLSSOutputViewRect,
	FVector2f TemporalJitterPixels,
	bool bDilateMotionVectors
)
{
	const FIntRect OutputViewRect = FIntRect( FIntPoint::ZeroValue, bDilateMotionVectors ? DLSSOutputViewRect.Size() : InputViewRect.Size());

	FRDGTextureDesc CombinedVelocityDesc = FRDGTextureDesc::Create2D(
		OutputViewRect.Size(),
		PF_G16R16F,
		FClearValueBinding::Black,
		TexCreate_ShaderResource | TexCreate_UAV);
	const TCHAR* OutputName = TEXT("DLSSCombinedVelocity");

	FRDGTextureRef CombinedVelocityTexture = GraphBuilder.CreateTexture(
		CombinedVelocityDesc,
		OutputName);

	FVelocityCombineCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FVelocityCombineCS::FParameters>();

	// input velocity
	{
		PassParameters->VelocityTexture = InVelocityTexture;
		PassParameters->VelocityTextureSampler = TStaticSamplerState<SF_Point>::GetRHI();

		// we use InSceneDepthTexture here and not InVelocityTexture since the latter can be a 1x1 black texture
		check(InVelocityTexture->Desc.Extent == FIntPoint(1, 1) || InVelocityTexture->Desc.Extent == InSceneDepthTexture->Desc.Extent);
		FScreenPassTextureViewport velocityViewport(InSceneDepthTexture, InputViewRect);
		FScreenPassTextureViewportParameters velocityViewportParameters = GetScreenPassTextureViewportParameters(velocityViewport);
		PassParameters->Velocity = velocityViewportParameters;
	}
	// input depth
	{
		PassParameters->DepthTexture = InSceneDepthTexture;
		PassParameters->DepthTextureSampler = TStaticSamplerState<SF_Point>::GetRHI();
	}
	// output combined velocity
	{
		PassParameters->OutVelocityCombinedTexture = GraphBuilder.CreateUAV(CombinedVelocityTexture);

		FScreenPassTextureViewport CombinedVelocityViewport(CombinedVelocityTexture, OutputViewRect);
		FScreenPassTextureViewportParameters CombinedVelocityViewportParameters = GetScreenPassTextureViewportParameters(CombinedVelocityViewport);
		PassParameters->CombinedVelocity = CombinedVelocityViewportParameters;
	}

	// various state
	{
		PassParameters->TemporalJitterPixels = TemporalJitterPixels;
		PassParameters->View = View.ViewUniformBuffer;
	}

	FVelocityCombineCS::FPermutationDomain PermutationVector;
	PermutationVector.Set<FDilateMotionVectorsDim>(bDilateMotionVectors);

	const FGlobalShaderMap* ShaderMap = GetGlobalShaderMap(View.GetFeatureLevel());
	TShaderMapRef<FVelocityCombineCS> ComputeShader(ShaderMap, PermutationVector);

	FComputeShaderUtils::AddPass(
		GraphBuilder,
		RDG_EVENT_NAME("Velocity Combine%s (%dx%d -> %dx%d)", 
			bDilateMotionVectors ? TEXT(" Dilate") : TEXT(""),
			InputViewRect.Width(), InputViewRect.Height(),
			OutputViewRect.Width(), OutputViewRect.Height()
		),
		ComputeShader,
		PassParameters,
		FComputeShaderUtils::GetGroupCount(OutputViewRect.Size(), FComputeShaderUtils::kGolden2DGroupSize));
		
	return CombinedVelocityTexture;
}