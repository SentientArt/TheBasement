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

#include "UIHintExtractionPass.h"

#include "Runtime/Launch/Resources/Version.h"
#if (ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 2)
#include "DataDrivenShaderPlatformInfo.h"
#endif

static const int32 kUIHintExtractionComputeTileSizeX = FComputeShaderUtils::kGolden2DGroupSize;
static const int32 kUIHintExtractionComputeTileSizeY = FComputeShaderUtils::kGolden2DGroupSize;

class FStreamlineUIHintExtractionCS : public FGlobalShader
{
public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		// Only cook for the platforms/RHIs where DLSS-FG is supported, which is DX11,DX12 [on Win64]
		return 	IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5) &&
				IsPCPlatform(Parameters.Platform) && (
#if (ENGINE_MAJOR_VERSION == 4) && (ENGINE_MINOR_VERSION <= 26)
					IsD3DPlatform(Parameters.Platform, false));
#else
					IsD3DPlatform(Parameters.Platform));
#endif
				
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEX"), kUIHintExtractionComputeTileSizeX);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEY"), kUIHintExtractionComputeTileSizeY);
	}
	DECLARE_GLOBAL_SHADER(FStreamlineUIHintExtractionCS);
	SHADER_USE_PARAMETER_STRUCT(FStreamlineUIHintExtractionCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		// Input images
		SHADER_PARAMETER(float, AlphaThreshold)
		SHADER_PARAMETER_TEXTURE(Texture2D, BackBuffer)
//		SHADER_PARAMETER_SAMPLER(SamplerState, VelocityTextureSampler)
//		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, Velocity)

//		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, DepthTexture)
//		SHADER_PARAMETER_SAMPLER(SamplerState, DepthTextureSampler)

//		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		
		// Output images
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, OutUIHintTexture)
//		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, CombinedVelocity)

	END_SHADER_PARAMETER_STRUCT()
};


IMPLEMENT_GLOBAL_SHADER(FStreamlineUIHintExtractionCS, "/Plugin/Streamline/Private/UIHintExtraction.usf", "UIHintExtractionMain", SF_Compute);

FRDGTextureRef AddStreamlineUIHintExtractionPass(
	FRDGBuilder& GraphBuilder,
//	const FViewInfo& View,
	const float InAlphaThreshold,
	const FTexture2DRHIRef& InBackBuffer
//	FRDGTextureRef InVelocityTexture
)
{

	FIntPoint BackBufferDimension = { int32(InBackBuffer->GetTexture2D()->GetSizeX()), int32(InBackBuffer->GetTexture2D()->GetSizeY()) };

	const FIntRect InputViewRect = { FIntPoint::ZeroValue,BackBufferDimension };
	const FIntRect OutputViewRect = { FIntPoint::ZeroValue,BackBufferDimension };

	FRDGTextureDesc UIHintTextureDesc =
#if (ENGINE_MAJOR_VERSION == 4) && (ENGINE_MINOR_VERSION == 25) 
	FRDGTextureDesc::Create2DDesc(
#else
	FRDGTextureDesc::Create2D(
#endif
		OutputViewRect.Size(),
		PF_B8G8R8A8,
		FClearValueBinding::Black,
#if (ENGINE_MAJOR_VERSION == 4) && (ENGINE_MINOR_VERSION == 25) 
		TexCreate_None,
		TexCreate_ShaderResource | TexCreate_RenderTargetable | TexCreate_UAV,
		/* bInForceSeparateTargetAndShaderResource = */ false);
#else
		TexCreate_ShaderResource | TexCreate_UAV);
#endif
	const TCHAR* OutputName = TEXT("Streamline.UIColorAndAlpha");

	FRDGTextureRef UIHintTexture = GraphBuilder.CreateTexture(
		UIHintTextureDesc,
		OutputName);

	FStreamlineUIHintExtractionCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FStreamlineUIHintExtractionCS::FParameters>();
	PassParameters->AlphaThreshold = FMath::Clamp(InAlphaThreshold, 0.0f, 1.0f);

	// input velocity
	{
		PassParameters->BackBuffer = InBackBuffer;
		//PassParameters->VelocityTextureSampler = TStaticSamplerState<SF_Point>::GetRHI();

		// we use InSceneDepthTexture here and not InVelocityTexture since the latter can be a 1x1 black texture
		//check(InVelocityTexture->Desc.Extent == FIntPoint(1, 1) || InVelocityTexture->Desc.Extent == InSceneDepthTexture->Desc.Extent);
		//FScreenPassTextureViewport velocityViewport(InSceneDepthTexture, InputViewRect);
		//FScreenPassTextureViewportParameters velocityViewportParameters = GetScreenPassTextureViewportParameters(velocityViewport);
		//PassParameters->Velocity = velocityViewportParameters;
	}
	
	{
		PassParameters->OutUIHintTexture = GraphBuilder.CreateUAV(UIHintTexture);

		//FScreenPassTextureViewport CombinedVelocityViewport(CombinedVelocityTexture, OutputViewRect);
		//FScreenPassTextureViewportParameters CombinedVelocityViewportParameters = GetScreenPassTextureViewportParameters(CombinedVelocityViewport);
	//	PassParameters->CombinedVelocity = CombinedVelocityViewportParameters;
	}

	// various state
	{

	//	PassParameters->View = View.ViewUniformBuffer;
	}

	FStreamlineUIHintExtractionCS::FPermutationDomain PermutationVector;
	
	TShaderMapRef<FStreamlineUIHintExtractionCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel), PermutationVector);

	FComputeShaderUtils::AddPass(
		GraphBuilder,
		RDG_EVENT_NAME("Streamline UI Hint extraction (%dx%d)", 
			OutputViewRect.Width(), OutputViewRect.Height()
		),
		ComputeShader,
		PassParameters,
		FComputeShaderUtils::GetGroupCount(OutputViewRect.Size(), FComputeShaderUtils::kGolden2DGroupSize));
		
	return UIHintTexture;
}