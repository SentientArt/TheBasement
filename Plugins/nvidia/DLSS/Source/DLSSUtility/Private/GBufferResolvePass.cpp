/*
* Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
*
* NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
* property and proprietary rights in and to this material, related
* documentation and any modifications thereto. Any use, reproduction,
* disclosure or distribution of this material and related documentation
* without an express license agreement from NVIDIA CORPORATION or
* its affiliates is strictly prohibited.
*/

#include "GBufferResolvePass.h"
#if __has_include("DataDrivenShaderPlatformInfo.h")
#include "DataDrivenShaderPlatformInfo.h"
#endif
#include "RHIResources.h"
#include "Runtime/Launch/Resources/Version.h"
#include "SceneTextureParameters.h"
#include "SystemTextures.h"


static TAutoConsoleVariable<bool> CVarNGXDLSSDisableSubsurfaceCheckerboard(
	TEXT("r.NGX.DLSS.DisableSubsurfaceCheckerboard"),
	true,
	TEXT("Skip checkerboard decoding for GBuffer Resolve Pass\n"),
	ECVF_RenderThreadSafe
);

class FDiffuseSpecularAlbedoDim : SHADER_PERMUTATION_BOOL("DIFFUSE_SPECULAR_ALBEDO");
class FForceDisableSubsurfaceCheckerboardDim : SHADER_PERMUTATION_BOOL("FORCE_DISABLE_SUBSURFACE_CHECKERBOARD");
class FGBufferResolvePS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FGBufferResolvePS);
	SHADER_USE_PARAMETER_STRUCT(FGBufferResolvePS, FGlobalShader);

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	using FPermutationDomain = TShaderPermutationDomain<FDiffuseSpecularAlbedoDim, FForceDisableSubsurfaceCheckerboardDim>;

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_INCLUDE(FSceneTextureShaderParameters, SceneTextures)
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		SHADER_PARAMETER_TEXTURE(Texture2D, PreIntegratedGF)
		SHADER_PARAMETER_SAMPLER(SamplerState, PreIntegratedGFSampler)
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, InputViewPort)
		SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, OutputViewPort)
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};

IMPLEMENT_GLOBAL_SHADER(FGBufferResolvePS, "/Plugin/DLSS/Private/GBufferResolve.usf", "GBufferResolvePixelShader", SF_Pixel);
FGBufferResolveOutputs AddGBufferResolvePass(FRDGBuilder& GraphBuilder,
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
	const FSceneView& View,
#else
	const FViewInfo& View,
#endif
	FIntRect InputViewRect,
	const bool bComputeDiffuseSpecularAlbedo
)
{
	FGBufferResolveOutputs Outputs;

	FGBufferResolvePS::FParameters* PassParameters = GraphBuilder.AllocParameters<FGBufferResolvePS::FParameters>();

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
	FSceneTextureShaderParameters SceneTextures = CreateSceneTextureShaderParameters(GraphBuilder, View, ESceneTextureSetupMode::All);
#else
	FSceneTextureShaderParameters SceneTextures = CreateSceneTextureShaderParameters(GraphBuilder, View.GetSceneTexturesChecked(), View.GetFeatureLevel(), ESceneTextureSetupMode::All);
#endif
	PassParameters->SceneTextures = SceneTextures;

	PassParameters->View = View.ViewUniformBuffer;
	PassParameters->PreIntegratedGF = GSystemTextures.PreintegratedGF->GetRHI();
	PassParameters->PreIntegratedGFSampler = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();

	const FIntPoint OutputExtent = InputViewRect.Size();
	if (bComputeDiffuseSpecularAlbedo)
	{
		FRDGTextureDesc AlbedoDesc(FRDGTextureDesc::Create2D(
			OutputExtent,
			PF_FloatR11G11B10, // TODO PF_B8G8R8A8 enough?
			FClearValueBinding::None,
			TexCreate_RenderTargetable | TexCreate_ShaderResource
		));
		Outputs.DiffuseAlbedo = GraphBuilder.CreateTexture(AlbedoDesc, TEXT("DLSSDiffuseAlbedo"));
		Outputs.SpecularAlbedo = GraphBuilder.CreateTexture(AlbedoDesc, TEXT("DLSSSpecularAlbedo"));

		PassParameters->RenderTargets[0] = FRenderTargetBinding(Outputs.DiffuseAlbedo, ERenderTargetLoadAction::ENoAction);
		PassParameters->RenderTargets[1] = FRenderTargetBinding(Outputs.SpecularAlbedo, ERenderTargetLoadAction::ENoAction);

		FRDGTextureDesc NormalDesc(FRDGTextureDesc::Create2D(
			OutputExtent,
			PF_FloatRGBA, // TODO PF_B8G8R8A8 enough?
			FClearValueBinding::None,
			TexCreate_RenderTargetable | TexCreate_ShaderResource
		));
		Outputs.Normals = GraphBuilder.CreateTexture(NormalDesc, TEXT("DLSSNormal"));

		PassParameters->RenderTargets[2] = FRenderTargetBinding(Outputs.Normals, ERenderTargetLoadAction::ENoAction);


		FRDGTextureDesc RoughnessDesc(FRDGTextureDesc::Create2D(
			OutputExtent,
			PF_R32_FLOAT, // TODO PF_B8G8R8A8 enough?
			FClearValueBinding::None,
			TexCreate_RenderTargetable | TexCreate_ShaderResource
		));
		Outputs.Roughness = GraphBuilder.CreateTexture(RoughnessDesc, TEXT("DLSSRoughness"));

		PassParameters->RenderTargets[3] = FRenderTargetBinding(Outputs.Roughness, ERenderTargetLoadAction::ENoAction);


		FRDGTextureDesc DepthDesc(FRDGTextureDesc::Create2D(
			OutputExtent,
			PF_R32_FLOAT, // TODO PF_B8G8R8A8 enough?
			FClearValueBinding::None,
			TexCreate_RenderTargetable | TexCreate_ShaderResource
		));
		Outputs.LinearDepth = GraphBuilder.CreateTexture(DepthDesc, TEXT("DLSSDepth"));

		PassParameters->RenderTargets[4] = FRenderTargetBinding(Outputs.LinearDepth, ERenderTargetLoadAction::ENoAction);
	}

	FGBufferResolvePS::FPermutationDomain PermutationVector;
	PermutationVector.Set<FDiffuseSpecularAlbedoDim>(bComputeDiffuseSpecularAlbedo);
	PermutationVector.Set<FForceDisableSubsurfaceCheckerboardDim>(CVarNGXDLSSDisableSubsurfaceCheckerboard.GetValueOnRenderThread());

	const FGlobalShaderMap* ShaderMap = GetGlobalShaderMap(View.GetFeatureLevel());
	TShaderMapRef<FGBufferResolvePS> PixelShader(ShaderMap, PermutationVector);
	FScreenPassTextureViewport InputViewport(InputViewRect);
	// shift output buffers to top left corner
	FScreenPassTextureViewport OutputViewport(OutputExtent);
	PassParameters->InputViewPort = GetScreenPassTextureViewportParameters(InputViewport);
	PassParameters->OutputViewPort = GetScreenPassTextureViewportParameters(OutputViewport);

	AddDrawScreenPass(
		GraphBuilder,
		RDG_EVENT_NAME("GBufferResolve%s",
			bComputeDiffuseSpecularAlbedo ? TEXT(" DiffuseSpecularAlbedo") : TEXT("")
		),
		View,
		OutputViewport,
		InputViewport,
		PixelShader,
		PassParameters);
	
	return Outputs;
}