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
#include "NISShaders.h"
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "RenderTargetPool.h"
#include "GeneralProjectSettings.h"
#include "SceneViewExtension.h"
#include "SceneView.h"
#include "ShaderCompilerCore.h"
#include "PostProcess/PostProcessTonemap.h"
#include "Runtime/Launch/Resources/Version.h"
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 2
#include "DataDrivenShaderPlatformInfo.h"
#endif

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
#include "SceneRendering.h"
#endif

// we don't pass NISConfigs as constant buffers into the shaders so we don't need the alignment
// however we also have static_asserts that make sure that FNISConfigParameters matches NISConfig
#define NIS_ALIGNED(x)
#include "NIS_Config.h"

#define LOCTEXT_NAMESPACE "FNISImageScalingShadersModule"


void FNISShadersModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("NIS"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/NIS"), PluginShaderDir);

	FString ThirdPartyShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("NIS"))->GetBaseDir(), TEXT("Shaders"), TEXT("ThirdParty"));
	AddShaderSourceDirectoryMapping(TEXT("/ThirdParty/Plugin/NIS"), ThirdPartyShaderDir);
}

void FNISShadersModule::ShutdownModule()
{

}

static TAutoConsoleVariable<float> CVarNISSharpness(
	TEXT("r.NIS.Sharpness"),
	0.0f,
	TEXT("0.0 to 1.0: Sharpening to apply to either primary NIS pass or the secondary NIS pass. If 0.0 the secondary NIS sharpening pass will not be executed (default: 0.0f)"),
	ECVF_RenderThreadSafe);


static TAutoConsoleVariable <int> CVarNISHalfPrecision(
	TEXT("r.NIS.HalfPrecision"),
	-1,
	TEXT("Enable/disable half precision in the NIS shaders and selects which permutation is used (default:-1)\n")
	TEXT("-1: automatic. Pick the appropriate FP16 permutation based on shader model and RHI\n")
	TEXT(" 0: Float32,       disable half precision\n")
	TEXT(" 1: Min16Float,    half precision, intended for UE4 DX11 SM5\n")
	TEXT(" 2: Min16FloatDXC, half precision, intended for UE4 DX12 SM5\n")
	TEXT(" 3: Float16DXC,    half precision, intended for UE5 DX12 SM6\n"),
	ECVF_RenderThreadSafe);

static TAutoConsoleVariable<int> CVarNISHDRMode(
	TEXT("r.NIS.HDRMode"),
	-1,
	TEXT("-1: Automatic. Determines the NIS HDR mode based on ETonemapperOutputDevice\n")
	TEXT("0: None\n")
	TEXT("1: Linear\n")
	TEXT("2: PQ\n"),
	ECVF_RenderThreadSafe);

// this should match NISConfig
BEGIN_SHADER_PARAMETER_STRUCT(FNISConfigParameters, )
	SHADER_PARAMETER(float, kDetectRatio)
	SHADER_PARAMETER(float, kDetectThres)
	SHADER_PARAMETER(float, kMinContrastRatio)
	SHADER_PARAMETER(float, kRatioNorm)

	SHADER_PARAMETER(float, kContrastBoost)
	SHADER_PARAMETER(float, kEps)
	SHADER_PARAMETER(float, kSharpStartY)
	SHADER_PARAMETER(float, kSharpScaleY)

	SHADER_PARAMETER(float, kSharpStrengthMin)
	SHADER_PARAMETER(float, kSharpStrengthScale)
	SHADER_PARAMETER(float, kSharpLimitMin)
	SHADER_PARAMETER(float, kSharpLimitScale)

	SHADER_PARAMETER(float, kScaleX)
	SHADER_PARAMETER(float, kScaleY)
	SHADER_PARAMETER(float, kDstNormX)
	SHADER_PARAMETER(float, kDstNormY)
	SHADER_PARAMETER(float, kSrcNormX)
	SHADER_PARAMETER(float, kSrcNormY)

	SHADER_PARAMETER(uint32, kInputViewportOriginX)
	SHADER_PARAMETER(uint32, kInputViewportOriginY)
	SHADER_PARAMETER(uint32, kInputViewportWidth)
	SHADER_PARAMETER(uint32, kInputViewportHeight)

	SHADER_PARAMETER(uint32, kOutputViewportOriginX)
	SHADER_PARAMETER(uint32, kOutputViewportOriginY)
	SHADER_PARAMETER(uint32, kOutputViewportWidth)
	SHADER_PARAMETER(uint32, kOutputViewportHeight)

	SHADER_PARAMETER(float, reserved0)
	SHADER_PARAMETER(float, reserved1)
END_SHADER_PARAMETER_STRUCT()

// not a complete guard against mismatches, but better than nothing
static_assert(sizeof(NISConfig) == sizeof(FNISConfigParameters), "mistmatch between engine & NIS SDK side struct");
static_assert(offsetof(NISConfig, kOutputViewportHeight) == offsetof(FNISConfigParameters, kOutputViewportHeight), "mistmatch between engine & NIS SDK side struct");

class FNISScalerDim : SHADER_PERMUTATION_BOOL("NIS_SCALER");
// SHADER_PERMUTATION_SPARSE_ENUM needs a ::MAX member, so we can't use the NIS enum directly, at least not without making a UE flavored copy of the type
class FNISHdrModeDim : SHADER_PERMUTATION_SPARSE_INT("NIS_HDR_MODE", int32(NISHDRMode::None), int32(NISHDRMode::Linear), int32(NISHDRMode::PQ));

// those need to be updated if GetOptimalBlockWidth etc return new values
class FNISBlockWidthDim      : SHADER_PERMUTATION_SPARSE_INT("NIS_BLOCK_WIDTH", 32);
class FNISBlockHeightDim     : SHADER_PERMUTATION_SPARSE_INT("NIS_BLOCK_HEIGHT", 32, 24);
class FNISThreadGroupSizeDim : SHADER_PERMUTATION_SPARSE_INT("NIS_THREAD_GROUP_SIZE", 128, 256);
class FNISViewportSupportDim : SHADER_PERMUTATION_BOOL("NIS_VIEWPORT_SUPPORT");

// the shaders treat NIS_USE_HALF_PRECISION 1 and 2 as on so we can use this to have another permutation that we compile with DXC
enum class ENISHalfPrecisionPermutation
{
	Float32,        // for everything else
	Min16Float,     // for UE4 DX11 SM5
	Min16FloatDXC,  // for UE4 DX12 SM5
	Float16DXC,     // for UE5 DX12 SM6 
	MAX
};
class FNISHalfPrecisionDim   : SHADER_PERMUTATION_ENUM_CLASS("NIS_USE_HALF_PRECISION", ENISHalfPrecisionPermutation);


NISSHADERS_API ERHIFeatureLevel::Type GetNISMinRequiredFeatureLevel()
{
	return ERHIFeatureLevel::SM5;
}

class FNISUpscaleCS : public FGlobalShader
{
public:
	static NISGPUArchitecture GetNISGPUArchitecture(const bool bHalfPrecision)
	{
		// those functions expect non-zero GRHIVendorId, but it's unclear how NDA platforms, such as consoles handle this...
		if (GRHIVendorId && IsRHIDeviceAMD())
		{
			return NISGPUArchitecture::AMD_Generic;
		}
		else if (GRHIVendorId && IsRHIDeviceIntel())
		{
			return NISGPUArchitecture::Intel_Generic;
		}
		else if (GRHIVendorId && IsRHIDeviceNVIDIA())
		{
			return bHalfPrecision ? NISGPUArchitecture::NVIDIA_Generic_fp16 :  NISGPUArchitecture::NVIDIA_Generic;
		}
		else
		{
			return NISGPUArchitecture::NVIDIA_Generic;
		}

	}
	static FIntPoint GetComputeTileSize(bool bIsUpscaling, const bool bHalfPrecision)
	{
		NISOptimizer Optimizer{ bIsUpscaling, GetNISGPUArchitecture(bHalfPrecision)} ;
		return  FIntPoint(Optimizer.GetOptimalBlockWidth(), Optimizer.GetOptimalBlockHeight());
	}

	static int32 GetThreadGroupSize(bool bIsUpscaling, const bool bHalfPrecision)
	{
		NISOptimizer Optimizer{ bIsUpscaling, GetNISGPUArchitecture(bHalfPrecision) };
		return Optimizer.GetOptimalThreadGroupSize();
	}

	static bool DoesPlatformSupportDXC(const FStaticShaderPlatform Platform)
	{
		return 
#if ENGINE_MAJOR_VERSION == 5
			FDataDrivenShaderPlatformInfo::GetSupportsDxc(Platform) ||
#endif
			(FDataDrivenShaderPlatformInfo::GetIsPC(Platform) && IsD3DPlatform(Platform));
	}

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		FPermutationDomain PermutationVector(Parameters.PermutationId);
		
		// UE4 doesn't support SM6 and float16t reliably....
		if (PermutationVector.Get<FNISHalfPrecisionDim>() == ENISHalfPrecisionPermutation::Float16DXC)
		{
#if ENGINE_MAJOR_VERSION != 5
			return false;
#else
			return 	DoesPlatformSupportDXC(Parameters.Platform) && IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM6);
#endif
		}

		return 	IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
		OutEnvironment.CompilerFlags.Add(CFLAG_AllowTypedUAVLoads);
		
		// for DX12 we need to DXC to get min16float in the NIS shaders to have an effect
		// NIS also supports SM6.2 explicit FP16, but in UE4 that's only supported for RT shaders.
		FPermutationDomain PermutationVector(Parameters.PermutationId);

	
		if(DoesPlatformSupportDXC(Parameters.Platform))
		{
			if (PermutationVector.Get<FNISHalfPrecisionDim>() == ENISHalfPrecisionPermutation::Min16FloatDXC)
			{
				OutEnvironment.CompilerFlags.Add(CFLAG_ForceDXC);
			}
			// UE5 supports SM6.6 so we can use the explicit FP16 NIS permutation
	#if ENGINE_MAJOR_VERSION == 5
			if (PermutationVector.Get<FNISHalfPrecisionDim>() == ENISHalfPrecisionPermutation::Float16DXC)
			{
				OutEnvironment.CompilerFlags.Add(CFLAG_ForceDXC);
				OutEnvironment.CompilerFlags.Add(CFLAG_AllowRealTypes);
				OutEnvironment.SetDefine(TEXT("NIS_HLSL_6_2"), 1);
			}
	#endif
		}
	}

	using FPermutationDomain = TShaderPermutationDomain<FNISScalerDim, FNISHdrModeDim, FNISHalfPrecisionDim,
		FNISBlockWidthDim, FNISBlockHeightDim, FNISThreadGroupSizeDim, FNISViewportSupportDim>;

	DECLARE_GLOBAL_SHADER(FNISUpscaleCS);
	SHADER_USE_PARAMETER_STRUCT(FNISUpscaleCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		// Input images
		SHADER_PARAMETER_SAMPLER(SamplerState, samplerLinearClamp)
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, in_texture)
		// Output images
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, out_texture)

		SHADER_PARAMETER_STRUCT_INCLUDE(FNISConfigParameters, Config)
		SHADER_PARAMETER_TEXTURE(Texture2D, coef_scaler)
		SHADER_PARAMETER_TEXTURE(Texture2D, coef_usm)
	
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		END_SHADER_PARAMETER_STRUCT()
};

IMPLEMENT_GLOBAL_SHADER(FNISUpscaleCS, "/Plugin/NIS/Private/NISUpscaler.usf", "main", SF_Compute);

struct FNISCoefficients : public FRenderResource
{
	FTexture2DRHIRef ScalerRHI = nullptr;
	FTexture2DRHIRef UsmRHI = nullptr;

	FTexture2DRHIRef ScalerHalfPrecisionRHI = nullptr;
	FTexture2DRHIRef UsmHalfPrecisionRHI = nullptr;


	class FNISCoefficientsResourceBulkData : public FResourceBulkDataInterface
	{
	public:
		FNISCoefficientsResourceBulkData(const void* InData, uint32_t InDataSize)
			: Data(InData)
			, DataSize(InDataSize)
		{ }

	public:
		virtual const void* GetResourceBulkData() const
		{
			return Data;
		}

		virtual uint32 GetResourceBulkDataSize() const
		{
			return DataSize;
		}

		virtual void Discard()
		{ }

	private:
		const void* Data;
		uint32_t    DataSize;
	};

	/**
	 * Initializes the RHI resources used by this resource.
	 * Called when entering the state where both the resource and the RHI have been initialized.
	 * This is only called by the rendering thread.
	 */

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
	virtual void InitRHI(FRHICommandListBase& RHICmdList)
#else
	virtual void InitRHI() 
#endif
	{
		// FP32
		{
			const uint32 CoefficientStride = kFilterSize * 4;
			const uint32 CoefficientSize = CoefficientStride * kPhaseCount;

			FNISCoefficientsResourceBulkData BulkData(coef_scale, CoefficientSize);
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1
			FRHITextureCreateDesc ScalerDesc = FRHITextureCreateDesc::Create2D(TEXT("FNISCoefficients::Scaler"))
				.SetExtent(kFilterSize / 4, kPhaseCount)
				.SetFormat(PF_A32B32G32R32F)
				.SetNumMips(1)
				.SetNumSamples(1)
				.SetFlags(TexCreate_None)
				.SetBulkData(&BulkData);
			ScalerRHI = RHICreateTexture(ScalerDesc);
#else
			FRHIResourceCreateInfo CreateInfo(TEXT("FNISCoefficients::Scaler"));
			CreateInfo.BulkData = &BulkData;
			ScalerRHI = RHICreateTexture2D(kFilterSize / 4, kPhaseCount, PF_A32B32G32R32F, 1, 1, TexCreate_None, CreateInfo);
#endif
		}

		{
			const uint32 CoefficientStride = kFilterSize * 4;
			const uint32 CoefficientSize = CoefficientStride * kPhaseCount;

			FNISCoefficientsResourceBulkData BulkData(coef_usm, CoefficientSize);
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1
			FRHITextureCreateDesc UsmDesc = FRHITextureCreateDesc::Create2D(TEXT("FNISCoefficients::Usm"))
				.SetExtent(kFilterSize / 4, kPhaseCount)
				.SetFormat(PF_A32B32G32R32F)
				.SetNumMips(1)
				.SetNumSamples(1)
				.SetFlags(TexCreate_None)
				.SetBulkData(&BulkData);
			UsmRHI = RHICreateTexture(UsmDesc);
#else
			FRHIResourceCreateInfo CreateInfo(TEXT("FNISCoefficients::Usm"));
			CreateInfo.BulkData = &BulkData;
			UsmRHI = RHICreateTexture2D(kFilterSize / 4, kPhaseCount, PF_A32B32G32R32F, 1, 1, TexCreate_None, CreateInfo);
#endif
		}

		// FP16
		{
			const uint32 CoefficientStride = kFilterSize * 2;
			const uint32 CoefficientSize = CoefficientStride * kPhaseCount;

			FNISCoefficientsResourceBulkData BulkData(coef_scale_fp16, CoefficientSize);
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1
			FRHITextureCreateDesc ScalerHalfDesc = FRHITextureCreateDesc::Create2D(TEXT("FNISCoefficients::ScalerHalfPrecision"))
				.SetExtent(kFilterSize / 4, kPhaseCount)
				.SetFormat(PF_FloatRGBA)
				.SetNumMips(1)
				.SetNumSamples(1)
				.SetFlags(TexCreate_None)
				.SetBulkData(&BulkData);
			ScalerHalfPrecisionRHI = RHICreateTexture(ScalerHalfDesc);
#else
			FRHIResourceCreateInfo CreateInfo(TEXT("FNISCoefficients::ScalerHalfPrecision"));
			CreateInfo.BulkData = &BulkData;
			ScalerHalfPrecisionRHI = RHICreateTexture2D(kFilterSize / 4, kPhaseCount, PF_FloatRGBA, 1, 1, TexCreate_None, CreateInfo);
#endif
		}

		{
			const uint32 CoefficientStride = kFilterSize * 2;
			const uint32 CoefficientSize = CoefficientStride * kPhaseCount;

			FNISCoefficientsResourceBulkData BulkData(coef_usm_fp16, CoefficientSize);
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1
			FRHITextureCreateDesc UsmHalfDesc = FRHITextureCreateDesc::Create2D(TEXT("FNISCoefficients::UsmHalfPrecision"))
				.SetExtent(kFilterSize / 4, kPhaseCount)
				.SetFormat(PF_FloatRGBA)
				.SetNumMips(1)
				.SetNumSamples(1)
				.SetFlags(TexCreate_None)
				.SetBulkData(&BulkData);
			UsmHalfPrecisionRHI = RHICreateTexture(UsmHalfDesc);
#else
			FRHIResourceCreateInfo CreateInfo(TEXT("FNISCoefficients::UsmHalfPrecision"));
			CreateInfo.BulkData = &BulkData;
			UsmHalfPrecisionRHI = RHICreateTexture2D(kFilterSize / 4, kPhaseCount, PF_FloatRGBA, 1, 1, TexCreate_None, CreateInfo);
#endif
		}
	}

	/**
	 * Releases the RHI resources used by this resource.
	 * Called when leaving the state where both the resource and the RHI have been initialized.
	 * This is only called by the rendering thread.
	 */
	virtual void ReleaseRHI() 
	{
		ScalerRHI.SafeRelease();
		UsmRHI.SafeRelease();

		ScalerHalfPrecisionRHI.SafeRelease();
		UsmHalfPrecisionRHI.SafeRelease();

	}

};

static TGlobalResource<FNISCoefficients> GNISCoefficients;

static NISHDRMode GetNISHDRModeFromEngineToneMapperOrCVar(const FSceneViewFamily& InViewFamily)
{
	const int NISHDRModeCVarValue = CVarNISHDRMode.GetValueOnRenderThread();

	if (NISHDRModeCVarValue == -1)
	{
		const FTonemapperOutputDeviceParameters ToneMapper = GetTonemapperOutputDeviceParameters(InViewFamily);
		
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1
		switch (EDisplayOutputFormat(ToneMapper.OutputDevice))
		{
			case EDisplayOutputFormat::SDR_sRGB:
			case EDisplayOutputFormat::SDR_Rec709:
			case EDisplayOutputFormat::SDR_ExplicitGammaMapping:
				return NISHDRMode::None;
			case EDisplayOutputFormat::HDR_ACES_1000nit_ST2084:
			case EDisplayOutputFormat::HDR_ACES_2000nit_ST2084:
				return NISHDRMode::PQ;
			case EDisplayOutputFormat::HDR_ACES_1000nit_ScRGB:
			case EDisplayOutputFormat::HDR_ACES_2000nit_ScRGB:
				return NISHDRMode::Linear;
			case EDisplayOutputFormat::HDR_LinearEXR:
			case EDisplayOutputFormat::HDR_LinearNoToneCurve:
			case EDisplayOutputFormat::HDR_LinearWithToneCurve:
				return NISHDRMode::Linear;
			case EDisplayOutputFormat::MAX:
			default:
				checkf(false, TEXT("invalid EDisplayOutputFormat passed into GetNISHDRModeFromEngineToneMapper "));
				return NISHDRMode::None;
		}
#else
		switch (ETonemapperOutputDevice(ToneMapper.OutputDevice))
		{
			case ETonemapperOutputDevice::sRGB:
			case ETonemapperOutputDevice::Rec709:
			case ETonemapperOutputDevice::ExplicitGammaMapping:
				return NISHDRMode::None;
			case ETonemapperOutputDevice::ACES1000nitST2084:
			case ETonemapperOutputDevice::ACES2000nitST2084:
				return NISHDRMode::PQ;
			case ETonemapperOutputDevice::ACES1000nitScRGB:
			case ETonemapperOutputDevice::ACES2000nitScRGB:
				return NISHDRMode::Linear;
			case ETonemapperOutputDevice::LinearEXR:
			case ETonemapperOutputDevice::LinearNoToneCurve:
			case ETonemapperOutputDevice::LinearWithToneCurve:
				return NISHDRMode::Linear;
			case ETonemapperOutputDevice::MAX:
			default:
				checkf(false, TEXT("invalid ETonemapperOutputDevice passed into GetNISHDRModeFromEngineToneMapper "));
				return NISHDRMode::None;
		}
#endif
	}
	else
	{
		return NISHDRMode(FMath::Clamp<int32>(NISHDRModeCVarValue, int32(NISHDRMode::None), int32(NISHDRMode::PQ)));
	}
}

FScreenPassTexture AddSharpenOrUpscalePass(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	const ISpatialUpscaler::FInputs& Inputs
)
{
	check(Inputs.SceneColor.IsValid());
	check(Inputs.Stage != EUpscaleStage::MAX);

	FScreenPassRenderTarget Output = Inputs.OverrideOutput;

	if (!Output.IsValid())
	{
		FRDGTextureDesc OutputDesc = Inputs.SceneColor.Texture->Desc;
		OutputDesc.Reset();

		if (Inputs.Stage == EUpscaleStage::PrimaryToSecondary)
		{
			const FIntPoint SecondaryViewRectSize = View.GetSecondaryViewRectSize();
			QuantizeSceneBufferSize(SecondaryViewRectSize, OutputDesc.Extent);
			Output.ViewRect.Min = FIntPoint::ZeroValue;
			Output.ViewRect.Max = SecondaryViewRectSize;
		}
		else
		{
			OutputDesc.Extent = View.UnscaledViewRect.Max;
			Output.ViewRect = View.UnscaledViewRect;
		}

		// We can't call OutputDesc.Flags |= GFastVRamConfig.Upscale this due to not being exported, so paraphrasing from SceneRendering.cpp:
		static const auto CVarFastVRamUpscale = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FastVRam.Upscale"));
		const int32 FastVRamUpscaleValue = CVarFastVRamUpscale ? CVarFastVRamUpscale->GetValueOnRenderThread() : 0;

		if (FastVRamUpscaleValue == 1)
		{
			EnumAddFlags(OutputDesc.Flags, TexCreate_FastVRAM);
		}
		else if (FastVRamUpscaleValue == 2)
		{
			EnumAddFlags(OutputDesc.Flags, TexCreate_FastVRAM | TexCreate_FastVRAMPartialAlloc);
		}

		Output.Texture = GraphBuilder.CreateTexture(OutputDesc, TEXT("NISSharpen"));
		Output.LoadAction = ERenderTargetLoadAction::EClear;
	}
	FRDGTextureRef OutputOrIntermediateTexture = Output.Texture;

	const FIntRect SrcRect = Inputs.SceneColor.ViewRect;
	FIntRect IntermediateDestRect = Output.ViewRect;
	const FIntRect OutputDestRect = Output.ViewRect;

	const bool bNeedIntermediateOutput = !EnumHasAnyFlags(Output.Texture->Desc.Flags, TexCreate_UAV);
	const bool bIsUpscaling = SrcRect.Size() != IntermediateDestRect.Size();
	
	// move the intermediate upscaled rect to the top left corner and allocate a smaller intermediate rendertarget
	if (bNeedIntermediateOutput)
	{
		FRDGTextureDesc IntermediateOutputDesc = Output.Texture->Desc;
		IntermediateOutputDesc.Reset();

		EnumAddFlags(IntermediateOutputDesc.Flags, TexCreate_UAV);
		EnumRemoveFlags(IntermediateOutputDesc.Flags, TexCreate_RenderTargetable | TexCreate_Presentable | TexCreate_ShaderResource);

		const FIntPoint InterMediateViewRectSize = IntermediateDestRect.Size();
		QuantizeSceneBufferSize(InterMediateViewRectSize, IntermediateOutputDesc.Extent);
		IntermediateDestRect.Min = FIntPoint::ZeroValue;
		IntermediateDestRect.Max = FIntPoint(InterMediateViewRectSize.X, InterMediateViewRectSize.Y);
		OutputOrIntermediateTexture = GraphBuilder.CreateTexture(IntermediateOutputDesc, bIsUpscaling ? TEXT("NISUpscaleIntermediateUAV") : TEXT("NISSharpenIntermediateUAV"));
	}

	const bool bNeedsViewportSupport = SrcRect != FIntRect(FIntPoint::ZeroValue, Inputs.SceneColor.Texture->Desc.Extent) ||
		IntermediateDestRect != FIntRect(FIntPoint::ZeroValue, OutputOrIntermediateTexture->Desc.Extent);

	FNISUpscaleCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FNISUpscaleCS::FParameters>();

	const float Sharpness = FMath::Clamp(CVarNISSharpness.GetValueOnRenderThread(), 0.0f, 1.0f);
	
	const NISHDRMode HdrMode = GetNISHDRModeFromEngineToneMapperOrCVar(*View.Family);

	const int32 bHalfPrecisionMode = CVarNISHalfPrecision.GetValueOnRenderThread();

	ENISHalfPrecisionPermutation HalfPrecisionPermutation = ENISHalfPrecisionPermutation::Float32;
	if (bHalfPrecisionMode == -1)
	{
#if PLATFORM_WINDOWS
		static const bool bIsDx12 = FCString::Strcmp(GDynamicRHI->GetName(), TEXT("D3D12")) == 0;
#else
		static const bool bIsDx12 = false;
#endif
		if (bIsDx12)
		{
#if ENGINE_MAJOR_VERSION == 5
			if (View.GetFeatureLevel() == ERHIFeatureLevel::SM6)
			{
				HalfPrecisionPermutation = ENISHalfPrecisionPermutation::Float16DXC;
			}
			else
#endif
			{
				HalfPrecisionPermutation = ENISHalfPrecisionPermutation::Min16FloatDXC;
			}
		}
		else
		{
			HalfPrecisionPermutation = ENISHalfPrecisionPermutation::Min16Float;
		}
	}
	else if (bHalfPrecisionMode == 0)
	{
		HalfPrecisionPermutation = ENISHalfPrecisionPermutation::Float32;
	}
	else if (bHalfPrecisionMode == 1)
	{
		HalfPrecisionPermutation = ENISHalfPrecisionPermutation::Min16Float;
	}
	else if (bHalfPrecisionMode == 2)
	{
		HalfPrecisionPermutation = ENISHalfPrecisionPermutation::Min16FloatDXC;
	}
#if ENGINE_MAJOR_VERSION == 5
	// we can only compile this one for SM6
	else if (bHalfPrecisionMode == 3 && View.GetFeatureLevel() == ERHIFeatureLevel::SM6)
	{
		HalfPrecisionPermutation = ENISHalfPrecisionPermutation::Float16DXC;
	}
#endif
	const bool bIsAnyHalfPrecisionPermutation = HalfPrecisionPermutation != ENISHalfPrecisionPermutation::Float32;
	
	
	NISConfig Config;
	FMemory::Memzero(Config);
	ensureMsgf(NVScalerUpdateConfig(
		Config, 
		Sharpness,
		SrcRect.Min.X, SrcRect.Min.Y,
		SrcRect.Width(), SrcRect.Height(),
		Inputs.SceneColor.Texture->Desc.Extent.X, Inputs.SceneColor.Texture->Desc.Extent.Y,

		IntermediateDestRect.Min.X, IntermediateDestRect.Min.Y,
		IntermediateDestRect.Width(), IntermediateDestRect.Height(),
		OutputOrIntermediateTexture->Desc.Extent.X, OutputOrIntermediateTexture->Desc.Extent.Y,
		HdrMode), TEXT("NVScalerUpdateConfig was called with invalid arguments. Please step into NVScalerUpdateConfig and put breakpoints on the return false statements to debug."));
	
	// TODO make this less sketchy 🤐
	static_assert(sizeof(NISConfig) == sizeof(FNISConfigParameters), "mistmatch between engine & NIS SDK side struct");
	static_assert(offsetof(NISConfig, kOutputViewportHeight) == offsetof(FNISConfigParameters, kOutputViewportHeight), "mistmatch between engine & NIS SDK side struct");
	FMemory::Memcpy(&PassParameters->Config, &Config, sizeof(NISConfig));
	
	PassParameters->coef_scaler = bIsAnyHalfPrecisionPermutation ? GNISCoefficients.ScalerHalfPrecisionRHI : GNISCoefficients.ScalerRHI;
	PassParameters->coef_usm = bIsAnyHalfPrecisionPermutation ? GNISCoefficients.UsmHalfPrecisionRHI : GNISCoefficients.UsmRHI;
	
	PassParameters->samplerLinearClamp = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
	PassParameters->in_texture = Inputs.SceneColor.Texture;
	PassParameters->out_texture = GraphBuilder.CreateUAV(OutputOrIntermediateTexture);

	PassParameters->View = View.ViewUniformBuffer;
	FNISUpscaleCS::FPermutationDomain PermutationVector;

	PermutationVector.Set<FNISScalerDim>(bIsUpscaling);
	PermutationVector.Set<FNISHdrModeDim>(int32(HdrMode));
	PermutationVector.Set<FNISHalfPrecisionDim>(HalfPrecisionPermutation);

	PermutationVector.Set<FNISBlockWidthDim>(FNISUpscaleCS::GetComputeTileSize(bIsUpscaling, bIsAnyHalfPrecisionPermutation).X);
	PermutationVector.Set<FNISBlockHeightDim>(FNISUpscaleCS::GetComputeTileSize(bIsUpscaling, bIsAnyHalfPrecisionPermutation).Y);
	PermutationVector.Set<FNISThreadGroupSizeDim>(FNISUpscaleCS::GetThreadGroupSize(bIsUpscaling, bIsAnyHalfPrecisionPermutation));
	PermutationVector.Set<FNISViewportSupportDim>(bNeedsViewportSupport);
	
	TShaderMapRef<FNISUpscaleCS> Shader(View.ShaderMap, PermutationVector);

	const TCHAR* const StageNames[] = { TEXT("PrimaryToSecondary"), TEXT("PrimaryToOutput"), TEXT("SecondaryToOutput") };
	static_assert(UE_ARRAY_COUNT(StageNames) == static_cast<uint32>(EUpscaleStage::MAX), "StageNames does not match EUpscaleStage");
	const TCHAR* StageName = StageNames[static_cast<uint32>(Inputs.Stage)];

	check(IntermediateDestRect.Size() == OutputDestRect.Size());

	FComputeShaderUtils::AddPass(
		GraphBuilder,
		RDG_EVENT_NAME("NIS %s %s %s (%s) (%dx%d -> %dx%d) = [%d,%d - %d,%d] -> [%d,%d - %d,%d]",
			bIsUpscaling ? TEXT("Upscaler") : TEXT("Sharpen"),
			bNeedIntermediateOutput ? TEXT("WithIntermediate ") : TEXT(""),
			bNeedsViewportSupport ? TEXT(" Viewport") : TEXT(""),
			StageName,
			SrcRect.Width(), SrcRect.Height(),
			IntermediateDestRect.Width(), IntermediateDestRect.Height(),
			SrcRect.Min.X, SrcRect.Min.Y,
			SrcRect.Max.X, SrcRect.Max.Y,
			IntermediateDestRect.Min.X, IntermediateDestRect.Min.Y,
			IntermediateDestRect.Max.X, IntermediateDestRect.Max.Y
		),
		Shader,
		PassParameters,
		FComputeShaderUtils::GetGroupCount(Output.ViewRect.Size(), Shader->GetComputeTileSize(bIsUpscaling, bIsAnyHalfPrecisionPermutation))
	);
	
	if (bNeedIntermediateOutput)
	{
		check(OutputOrIntermediateTexture != Output.Texture);
		AddCopyTexturePass(GraphBuilder, OutputOrIntermediateTexture, Output.Texture, 
			IntermediateDestRect.Min,
			OutputDestRect.Min,
			IntermediateDestRect.Size());
	}
	return MoveTemp(Output);
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FNISShadersModule, NISShaders)
	
