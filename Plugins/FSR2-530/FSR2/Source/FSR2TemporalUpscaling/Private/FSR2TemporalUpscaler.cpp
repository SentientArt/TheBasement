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
#include "FSR2TemporalUpscaler.h"
#include "FSR2TemporalUpscalerProxy.h"
#include "FSR2TemporalUpscaling.h"
#include "FSR2Include.h"
#include "FSR2TemporalUpscalerHistory.h"
#include "RHI/FSR2TemporalUpscalerRHIBackend.h"
#include "FSR2TemporalUpscalingAPI.h"
#include "SceneTextureParameters.h"
#include "TranslucentRendering.h"
#include "ScenePrivate.h"
#include "LogFSR2.h"
#include "LegacyScreenPercentageDriver.h"
#include "PlanarReflectionSceneProxy.h"
#include "ScreenSpaceRayTracing.h"
#include "Serialization/MemoryImage.h"
#include "Serialization/MemoryLayout.h"
#include "FXSystem.h"
#include "PostProcess/SceneRenderTargets.h"
#if FSR2_ENABLE_VK
#include "IVulkanDynamicRHI.h"
#endif
#include "HAL/IConsoleManager.h"
#include "DataDrivenShaderPlatformInfo.h"
#include "PostProcess/PostProcessEyeAdaptation.h"
#include "FXRenderingUtils.h"

//------------------------------------------------------------------------------------------------------
// GPU statistics for the FSR2 passes.
//------------------------------------------------------------------------------------------------------
DECLARE_GPU_STAT(FidelityFXSuperResolution2Pass)
DECLARE_GPU_STAT_NAMED(FidelityFXFSR2Dispatch, TEXT("FidelityFX FSR2 Dispatch"));

//------------------------------------------------------------------------------------------------------
// Quality mode definitions
//------------------------------------------------------------------------------------------------------
static const FfxFsr2QualityMode DefaultQualityMode = FFX_FSR2_QUALITY_MODE_QUALITY;
static const FfxFsr2QualityMode LowestResolutionQualityMode = FFX_FSR2_QUALITY_MODE_ULTRA_PERFORMANCE;
static const FfxFsr2QualityMode HighestResolutionQualityMode = FFX_FSR2_QUALITY_MODE_QUALITY;

//------------------------------------------------------------------------------------------------------
// Console variables that control how FSR2 operates.
//------------------------------------------------------------------------------------------------------
static TAutoConsoleVariable<float> CVarFSR2Sharpness(
	TEXT("r.FidelityFX.FSR2.Sharpness"),
	0.0f,
	TEXT("Range from 0.0 to 1.0, when greater than 0 this enables Robust Contrast Adaptive Sharpening Filter to sharpen the output image. Default is 0."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<int32> CVarFSR2AutoExposure(
	TEXT("r.FidelityFX.FSR2.AutoExposure"),
	0,
	TEXT("True to use FSR2's own auto-exposure, otherwise the engine's auto-exposure value is used."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<int32> CVarFSR2HistoryFormat(
	TEXT("r.FidelityFX.FSR2.HistoryFormat"), 
	0,
	TEXT("Selects the bit-depth for the FSR2 history texture format, defaults to PF_FloatRGBA but can be set to PF_FloatR11G11B10 to reduce bandwidth at the expense of quality.\n")
	TEXT(" 0 - PF_FloatRGBA\n")
	TEXT(" 1 - PF_FloatR11G11B10\n"),
	ECVF_RenderThreadSafe);

static TAutoConsoleVariable<int32> CVarFSR2CreateReactiveMask(
	TEXT("r.FidelityFX.FSR2.CreateReactiveMask"),
	1,
	TEXT("Enable to generate a mask from the SceneColor, GBuffer, SeparateTranslucency & ScreenspaceReflections that determines how reactive each pixel should be. Defaults to 1 (Enabled)."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<float> CVarFSR2ReactiveMaskReflectionScale(
	TEXT("r.FidelityFX.FSR2.ReactiveMaskReflectionScale"),
	0.4f,
	TEXT("Range from 0.0 to 1.0 (Default 0.4), scales the Unreal engine reflection contribution to the reactive mask, which can be used to control the amount of aliasing on reflective surfaces."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<float> CVarFSR2ReactiveMaskRoughnessScale(
	TEXT("r.FidelityFX.FSR2.ReactiveMaskRoughnessScale"),
	0.15f,
	TEXT("Range from 0.0 to 1.0 (Default 0.15), scales the GBuffer roughness to provide a fallback value for the reactive mask when screenspace & planar reflections are disabled or don't affect a pixel."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<float> CVarFSR2ReactiveMaskRoughnessBias(
	TEXT("r.FidelityFX.FSR2.ReactiveMaskRoughnessBias"),
	0.25f,
	TEXT("Range from 0.0 to 1.0 (Default 0.25), biases the reactive mask value when screenspace/planar reflections are weak with the GBuffer roughness to account for reflection environment captures."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<float> CVarFSR2ReactiveMaskRoughnessMaxDistance(
	TEXT("r.FidelityFX.FSR2.ReactiveMaskRoughnessMaxDistance"),
	6000.0f,
	TEXT("Maximum distance in world units for using material roughness to contribute to the reactive mask, the maximum of this value and View.FurthestReflectionCaptureDistance will be used. Default is 6000.0."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<int32> CVarFSR2ReactiveMaskRoughnessForceMaxDistance(
	TEXT("r.FidelityFX.FSR2.ReactiveMaskRoughnessForceMaxDistance"),
	0,
	TEXT("Enable to force the maximum distance in world units for using material roughness to contribute to the reactive mask rather than using View.FurthestReflectionCaptureDistance. Defaults to 0."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<float> CVarFSR2ReactiveMaskReflectionLumaBias(
	TEXT("r.FidelityFX.FSR2.ReactiveMaskReflectionLumaBias"),
	0.0f,
	TEXT("Range from 0.0 to 1.0 (Default: 0.0), biases the reactive mask by the luminance of the reflection. Use to balance aliasing against ghosting on brightly lit reflective surfaces."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<float> CVarFSR2ReactiveHistoryTranslucencyBias(
	TEXT("r.FidelityFX.FSR2.ReactiveHistoryTranslucencyBias"),
	0.5f,
	TEXT("Range from 0.0 to 1.0 (Default: 1.0), scales how much translucency suppresses history via the reactive mask. Higher values will make translucent materials more reactive which can reduce smearing."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<float> CVarFSR2ReactiveHistoryTranslucencyLumaBias(
	TEXT("r.FidelityFX.FSR2.ReactiveHistoryTranslucencyLumaBias"),
	0.0f,
	TEXT("Range from 0.0 to 1.0 (Default 0.0), biases how much the translucency suppresses history via the reactive mask by the luminance of the transparency. Higher values will make bright translucent materials more reactive which can reduce smearing."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<float> CVarFSR2ReactiveMaskTranslucencyBias(
	TEXT("r.FidelityFX.FSR2.ReactiveMaskTranslucencyBias"),
	1.0f,
	TEXT("Range from 0.0 to 1.0 (Default: 1.0), scales how much contribution translucency makes to the reactive mask. Higher values will make translucent materials more reactive which can reduce smearing."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<float> CVarFSR2ReactiveMaskTranslucencyLumaBias(
	TEXT("r.FidelityFX.FSR2.ReactiveMaskTranslucencyLumaBias"),
	0.0f,
	TEXT("Range from 0.0 to 1.0 (Default 0.0), biases the translucency contribution to the reactive mask by the luminance of the transparency. Higher values will make bright translucent materials more reactive which can reduce smearing."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<float> CVarFSR2ReactiveMaskTranslucencyMaxDistance(
	TEXT("r.FidelityFX.FSR2.ReactiveMaskTranslucencyMaxDistance"),
	500000.0f,
	TEXT("Maximum distance in world units for using translucency to contribute to the reactive mask. This is a way to remove sky-boxes and other back-planes from the reactive mask, at the expense of nearer translucency not being reactive. Default is 500000.0."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<float> CVarFSR2ReactiveMaskForceReactiveMaterialValue(
	TEXT("r.FidelityFX.FSR2.ReactiveMaskForceReactiveMaterialValue"),
	0.0f,
	TEXT("Force the reactive mask value for Reactive Shading Model materials, when > 0 this value can be used to override the value supplied in the Material Graph. Default is 0 (Off)."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<int32> CVarFSR2ReactiveMaskReactiveShadingModelID(
	TEXT("r.FidelityFX.FSR2.ReactiveMaskReactiveShadingModelID"),
	MSM_NUM,
	TEXT("Treat the specified shading model as reactive, taking the CustomData0.x value as the reactive value to write into the mask. Default is MSM_NUM (Off)."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<int32> CVarFSR2UseNativeDX12(
	TEXT("r.FidelityFX.FSR2.UseNativeDX12"),
	1,
	TEXT("True to use FSR2's native & optimised D3D12 backend, false to use the fallback implementation based on Unreal's RHI. Default is 1."),
	ECVF_ReadOnly
);

static TAutoConsoleVariable<int32> CVarFSR2UseNativeVulkan(
	TEXT("r.FidelityFX.FSR2.UseNativeVulkan"),
	1,
	TEXT("True to use FSR2's native & optimised Vulkan backend, false to use the fallback implementation based on Unreal's RHI. Default is 1."),
	ECVF_ReadOnly
);

static TAutoConsoleVariable<int32> CVarFSR2QuantizeInternalTextures(
	TEXT("r.FidelityFX.FSR2.QuantizeInternalTextures"),
	0,
	TEXT("Setting this to 1 will round up the size of some internal texture to ensure a specific divisibility. Default is 0."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<int32> CVarFSR2UseExperimentalSSRDenoiser(
	TEXT("r.FidelityFX.FSR2.UseSSRExperimentalDenoiser"),
	0,
	TEXT("Set to 1 to use r.SSR.ExperimentalDenoiser when FSR2 is enabled. This is required when r.FidelityFX.FSR2.CreateReactiveMask is enabled as the FSR2 plugin sets r.SSR.ExperimentalDenoiser to 1 in order to capture reflection data to generate the reactive mask. Default is 0."),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<int32> CVarFSR2DeDitherMode(
	TEXT("r.FidelityFX.FSR2.DeDither"),
	2,
	TEXT("Adds an extra pass to de-dither and avoid dithered/thin appearance. Default is 0 - Off. \n")
	TEXT(" 0 - Off. \n")
	TEXT(" 1 - Full. Attempts to de-dither the whole scene. \n")
	TEXT(" 2 - Hair only. Will only de-dither around Hair shading model pixels - requires the Deferred Renderer. \n"),
	ECVF_RenderThreadSafe
);

static TAutoConsoleVariable<int32> CVarFSR2QualityMode(
	TEXT("r.FidelityFX.FSR2.QualityMode"),
	DefaultQualityMode,
	TEXT("FSR2 Mode [1-4].  Lower values yield superior images.  Higher values yield improved performance.  Default is 1 - Quality.\n")
	TEXT(" 1 - Quality				1.5x \n")
	TEXT(" 2 - Balanced				1.7x \n")
	TEXT(" 3 - Performance			2.0x \n")
	TEXT(" 4 - Ultra Performance	3.0x \n"),
	ECVF_RenderThreadSafe
);

float FFSR2TemporalUpscaler::SavedScreenPercentage{ 100.0f };

//------------------------------------------------------------------------------------------------------
// Unreal shader to convert from the Velocity texture format to the Motion Vectors used by FSR2.
//------------------------------------------------------------------------------------------------------
class FFSR2ConvertVelocityCS : public FGlobalShader
{
public:
	static const int ThreadgroupSizeX = 8;
	static const int ThreadgroupSizeY = 8;
	static const int ThreadgroupSizeZ = 1;

	DECLARE_GLOBAL_SHADER(FFSR2ConvertVelocityCS);
	SHADER_USE_PARAMETER_STRUCT(FFSR2ConvertVelocityCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		RDG_TEXTURE_ACCESS(DepthTexture, ERHIAccess::SRVCompute)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, InputDepth)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, InputVelocity)
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, OutputTexture)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEX"), ThreadgroupSizeX);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEY"), ThreadgroupSizeY);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEZ"), ThreadgroupSizeZ);
		OutEnvironment.SetDefine(TEXT("COMPUTE_SHADER"), 1);
	}
};
IMPLEMENT_GLOBAL_SHADER(FFSR2ConvertVelocityCS, "/Plugin/FSR2/Private/PostProcessFFX_FSR2ConvertVelocity.usf", "MainCS", SF_Compute);

//------------------------------------------------------------------------------------------------------
// Unreal shader to generate mask textures for translucency & reactivity to be used in FSR2.
//------------------------------------------------------------------------------------------------------
class FFSR2CreateReactiveMaskCS : public FGlobalShader
{
public:
	static const int ThreadgroupSizeX = 8;
	static const int ThreadgroupSizeY = 8;
	static const int ThreadgroupSizeZ = 1;

	DECLARE_GLOBAL_SHADER(FFSR2CreateReactiveMaskCS);
	SHADER_USE_PARAMETER_STRUCT(FFSR2CreateReactiveMaskCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		RDG_TEXTURE_ACCESS(DepthTexture, ERHIAccess::SRVCompute)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, GBufferB)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, GBufferD)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, ReflectionTexture)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, InputDepth)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, SceneColor)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, SceneColorPreAlpha)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, LumenSpecular)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, InputVelocity)
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, ReactiveMask)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, CompositeMask)
		SHADER_PARAMETER_SAMPLER(SamplerState, Sampler)
		SHADER_PARAMETER(float, FurthestReflectionCaptureDistance)
		SHADER_PARAMETER(float, ReactiveMaskReflectionScale)
		SHADER_PARAMETER(float, ReactiveMaskRoughnessScale)
		SHADER_PARAMETER(float, ReactiveMaskRoughnessBias)
		SHADER_PARAMETER(float, ReactiveMaskReflectionLumaBias)
		SHADER_PARAMETER(float, ReactiveHistoryTranslucencyBias)
		SHADER_PARAMETER(float, ReactiveHistoryTranslucencyLumaBias)
		SHADER_PARAMETER(float, ReactiveMaskTranslucencyBias)
		SHADER_PARAMETER(float, ReactiveMaskTranslucencyLumaBias)
		SHADER_PARAMETER(float, ReactiveMaskTranslucencyMaxDistance)
		SHADER_PARAMETER(float, ForceLitReactiveValue)
		SHADER_PARAMETER(uint32, ReactiveShadingModelID)
		SHADER_PARAMETER(uint32, LumenSpecularCurrentFrame)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEX"), ThreadgroupSizeX);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEY"), ThreadgroupSizeY);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEZ"), ThreadgroupSizeZ);
		OutEnvironment.SetDefine(TEXT("COMPUTE_SHADER"), 1);
	}
};
IMPLEMENT_GLOBAL_SHADER(FFSR2CreateReactiveMaskCS, "/Plugin/FSR2/Private/PostProcessFFX_FSR2CreateReactiveMask.usf", "MainCS", SF_Compute);

//------------------------------------------------------------------------------------------------------
// Unreal shader to blend hair which is dithered and FSR2 doesn't handle that well.
//------------------------------------------------------------------------------------------------------
class FFSR2DeDitherCS : public FGlobalShader
{
public:
	static const int ThreadgroupSizeX = 8;
	static const int ThreadgroupSizeY = 8;
	static const int ThreadgroupSizeZ = 1;

	DECLARE_GLOBAL_SHADER(FFSR2DeDitherCS);
	SHADER_USE_PARAMETER_STRUCT(FFSR2DeDitherCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, GBufferB)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, SceneColor)
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, BlendSceneColor)
		SHADER_PARAMETER(uint32, FullDeDither)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEX"), ThreadgroupSizeX);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEY"), ThreadgroupSizeY);
		OutEnvironment.SetDefine(TEXT("THREADGROUP_SIZEZ"), ThreadgroupSizeZ);
		OutEnvironment.SetDefine(TEXT("COMPUTE_SHADER"), 1);
	}
};
IMPLEMENT_GLOBAL_SHADER(FFSR2DeDitherCS, "/Plugin/FSR2/Private/PostProcessFFX_FSR2DeDither.usf", "MainCS", SF_Compute);

//------------------------------------------------------------------------------------------------------
// Unreal shader to copy EyeAdaptationBuffer data to Exposure texture.
//------------------------------------------------------------------------------------------------------
class FFSR2CopyExposureCS : public FGlobalShader
{
public:

	DECLARE_GLOBAL_SHADER(FFSR2CopyExposureCS);
	SHADER_USE_PARAMETER_STRUCT(FFSR2CopyExposureCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float4>, EyeAdaptationBuffer)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, ExposureTexture)
		END_SHADER_PARAMETER_STRUCT()

		static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		OutEnvironment.SetDefine(TEXT("COMPUTE_SHADER"), 1);
	}
};
IMPLEMENT_GLOBAL_SHADER(FFSR2CopyExposureCS, "/Plugin/FSR2/Private/PostProcessFFX_FSR2CopyExposure.usf", "MainCS", SF_Compute);

struct FFSR2ShaderMapSwapState
{
	const FGlobalShaderMapContent* Content;
	bool bSwapped;

	static const FFSR2ShaderMapSwapState Default;
};
const FFSR2ShaderMapSwapState FFSR2ShaderMapSwapState::Default = { nullptr, false };

// this object isn't conceptually linked to individual FSR2TemporalUpscalers.  it contains information about the state of an object in the global shader map,
// and that information needs to be consistent across all FSR2TemporalUpscalers that might currently exist.
static TMap<class FGlobalShaderMap*, FFSR2ShaderMapSwapState> SSRShaderMapSwapState;

//------------------------------------------------------------------------------------------------------
// The FFSR2ShaderMapContent structure allows access to the internals of FShaderMapContent so that FSR2 can swap the Default & Denoised variants of ScreenSpaceReflections.
//------------------------------------------------------------------------------------------------------
class FFSR2ShaderMapContent
{
public:
	DECLARE_TYPE_LAYOUT(FFSR2ShaderMapContent, NonVirtual);

	using FMemoryImageHashTable = THashTable<FMemoryImageAllocator>;

	LAYOUT_FIELD(FMemoryImageHashTable, ShaderHash);
	LAYOUT_FIELD(TMemoryImageArray<FHashedName>, ShaderTypes);
	LAYOUT_FIELD(TMemoryImageArray<int32>, ShaderPermutations);
	LAYOUT_FIELD(TMemoryImageArray<TMemoryImagePtr<FShader>>, Shaders);
	LAYOUT_FIELD(TMemoryImageArray<TMemoryImagePtr<FShaderPipeline>>, ShaderPipelines);
	/** The platform this shader map was compiled with */
	LAYOUT_FIELD(FMemoryImageName, ShaderPlatformName);
};
static_assert(sizeof(FShaderMapContent) == sizeof(FFSR2ShaderMapContent), "FFSR2ShaderMapContent must match the layout of FShaderMapContent so we can access the SSR shaders!");

//------------------------------------------------------------------------------------------------------
// Definitions used by the ScreenSpaceReflections shaders needed to perform necessary swizzling.
//------------------------------------------------------------------------------------------------------
class FSSRQualityDim : SHADER_PERMUTATION_ENUM_CLASS("SSR_QUALITY", ESSRQuality);
class FSSROutputForDenoiser : SHADER_PERMUTATION_BOOL("SSR_OUTPUT_FOR_DENOISER");
struct FFSR2ScreenSpaceReflectionsPS
{
	using FPermutationDomain = TShaderPermutationDomain<FSSRQualityDim, FSSROutputForDenoiser>;
};

//------------------------------------------------------------------------------------------------------
// In order to access the Lumen reflection data prior to our code executing it is necessary to gain access to FFSR2RDGBuilder internals.
//------------------------------------------------------------------------------------------------------

struct FFSR2ParallelPassSet : public FRHICommandListImmediate::FQueuedCommandList
{
	FFSR2ParallelPassSet() = default;

	TArray<FRDGPass*, FRDGArrayAllocator> Passes;
	IF_RHI_WANT_BREADCRUMB_EVENTS(FRDGBreadcrumbState* BreadcrumbStateBegin{});
	IF_RHI_WANT_BREADCRUMB_EVENTS(FRDGBreadcrumbState* BreadcrumbStateEnd{});
	int8 bInitialized = 0;
	bool bDispatchAfterExecute = false;
	bool bParallelTranslate = false;
};

class FFSR2RDGBuilder : FRDGAllocatorScope
{
public:
	FFSR2RDGBuilder(FRHICommandListImmediate& InRHICmdList, FRDGEventName InName = {}, ERDGBuilderFlags InFlags = ERDGBuilderFlags::None)
		: RHICmdList(InRHICmdList)
		, BuilderName(InName)
		, CompilePipe(TEXT("FSR2RDGCompilePipe"))
#if RDG_CPU_SCOPES
		, CPUScopeStacks(Allocator)
#endif
		, GPUScopeStacks(Allocator)
#if RDG_ENABLE_DEBUG
		, UserValidation(Allocator, bParallelExecuteEnabled)
		, BarrierValidation(&Passes, BuilderName)
#endif
		, ExtendResourceLifetimeScope(InRHICmdList)
	{
	}
	FFSR2RDGBuilder(const FFSR2RDGBuilder&) = delete;
	~FFSR2RDGBuilder()
	{
	}

	FRHICommandListImmediate& RHICmdList;

	struct FFSR2BlackBoard
	{
		FRDGAllocator* Allocator;
		TArray<struct FStruct*, FRDGArrayAllocator> Blackboard;
	};
	FFSR2BlackBoard Blackboard;

	FRDGTextureRef FindTexture(TCHAR const* Name)
	{
		for (FRDGTextureHandle It = Textures.Begin(); It != Textures.End(); ++It)
		{
			FRDGTextureRef Texture = Textures.Get(It);
			if (FCString::Strcmp(Texture->Name, Name) == 0)
			{
				return Texture;
			}
		}
		return nullptr;
	}

private:
	const FRDGEventName BuilderName;
	FRDGPassRegistry Passes;
	FRDGTextureRegistry Textures;
	FRDGBufferRegistry Buffers;
	FRDGViewRegistry Views;
	FRDGUniformBufferRegistry UniformBuffers;
	TArray<FRDGUniformBufferHandle, FRDGArrayAllocator> UniformBuffersToCreate;
	TSortedMap<FRHITexture*, FRDGTexture*, FRDGArrayAllocator> ExternalTextures;
	TSortedMap<FRHIBuffer*, FRDGBuffer*, FRDGArrayAllocator> ExternalBuffers;
	TMap<FRDGPooledTexture*, FRDGTexture*, FRDGSetAllocator> PooledTextureOwnershipMap;
	TMap<FRDGPooledBuffer*, FRDGBuffer*, FRDGSetAllocator> PooledBufferOwnershipMap;
	TArray<TRefCountPtr<IPooledRenderTarget>, FRDGArrayAllocator> ActivePooledTextures;
	TArray<TRefCountPtr<FRDGPooledBuffer>, FRDGArrayAllocator> ActivePooledBuffers;
	TMap<FRDGBarrierBatchBeginId, FRDGBarrierBatchBegin*, FRDGSetAllocator> BarrierBatchMap;
	FRDGTransitionCreateQueue TransitionCreateQueue;

	template <typename LambdaType>
	UE::Tasks::FTask LaunchCompileTask(const TCHAR* Name, bool bCondition, LambdaType&& Lambda);

	UE::Tasks::FPipe CompilePipe;

	class FPassQueue
	{
		TLockFreePointerListFIFO<FRDGPass, PLATFORM_CACHE_LINE_SIZE> Queue;
		UE::Tasks::FTask LastTask;
	};

	FPassQueue SetupPassQueue;

	TArray<FRDGPassHandle, FRDGArrayAllocator> CullPassStack;

	FRDGPass* ProloguePass;
	FRDGPass* EpiloguePass;

	struct FExtractedTexture
	{
		FRDGTexture* Texture{};
		TRefCountPtr<IPooledRenderTarget>* PooledTexture{};
	};

	TArray<FExtractedTexture, FRDGArrayAllocator> ExtractedTextures;

	struct FExtractedBuffer
	{
		FRDGBuffer* Buffer{};
		TRefCountPtr<FRDGPooledBuffer>* PooledBuffer{};
	};

	TArray<FExtractedBuffer, FRDGArrayAllocator> ExtractedBuffers;

	struct FUploadedBuffer
	{
		bool bUseDataCallbacks;
		bool bUseFreeCallbacks;
		FRDGBuffer* Buffer{};
		const void* Data{};
		uint64 DataSize{};
		FRDGBufferInitialDataCallback DataCallback;
		FRDGBufferInitialDataSizeCallback DataSizeCallback;
		FRDGBufferInitialDataFreeCallback DataFreeCallback;
	};

	TArray<FUploadedBuffer, FRDGArrayAllocator> UploadedBuffers;

	TArray<FFSR2ParallelPassSet, FRDGArrayAllocator> ParallelPassSets;

	TArray<UE::Tasks::FTask, FRDGArrayAllocator> ParallelExecuteEvents;

	TArray<UE::Tasks::FTask, FRDGArrayAllocator> ParallelSetupEvents;

	TArray<FRHITrackedAccessInfo, FRDGArrayAllocator> EpilogueResourceAccesses;

	TArray<FRDGViewableResource*, FRDGArrayAllocator> AccessModeQueue;
	TSet<FRDGViewableResource*, DefaultKeyFuncs<FRDGViewableResource*>, FRDGSetAllocator> ExternalAccessResources;

	FRDGTextureSubresourceStateIndirect ScratchTextureState;

	EAsyncComputeBudget AsyncComputeBudgetScope;
	EAsyncComputeBudget AsyncComputeBudgetState;

	FRHICommandList* RHICmdListBufferUploads;

	IF_RDG_CPU_SCOPES(FRDGCPUScopeStacks CPUScopeStacks);
	FRDGGPUScopeStacksByPipeline GPUScopeStacks;
	IF_RHI_WANT_BREADCRUMB_EVENTS(FRDGBreadcrumbState* BreadcrumbState{});

	IF_RDG_ENABLE_TRACE(FRDGTrace Trace);

	bool bFlushResourcesRHI = false;
	bool bParallelExecuteEnabled = false;
	bool bParallelSetupEnabled = false;
	bool bFinalEventScopeActive = false;

#if RDG_ENABLE_DEBUG
	FRDGUserValidation UserValidation;
	FRDGBarrierValidation BarrierValidation;
#endif

	struct FAuxiliaryPass
	{
		uint8 Clobber;
		uint8 Visualize;
		uint8 Dump;
		uint8 FlushAccessModeQueue;
	} AuxiliaryPasses;

#if WITH_MGPU
	bool bForceCopyCrossGPU;
#endif // WITH_MGPU

	uint32 AsyncComputePassCount;
	uint32 RasterPassCount;

	IF_RDG_CMDLIST_STATS(TStatId CommandListStatScope);
	IF_RDG_CMDLIST_STATS(TStatId CommandListStatState);

	IRHITransientResourceAllocator* TransientResourceAllocator;

	FRHICommandListScopedExtendResourceLifetime ExtendResourceLifetimeScope;
};
static_assert(sizeof(FRDGBuilder) == sizeof(FFSR2RDGBuilder), "FFSR2RDGBuilder must match the layout of FRDGBuilder so we can access the Lumen reflection texture!");

//------------------------------------------------------------------------------------------------------
// Internal function definitions
// Many of these are replicas of UE functions used in the denoiser API implementation so that we match the default engine behaviour.
//------------------------------------------------------------------------------------------------------
static bool FSR2HasDeferredPlanarReflections(const FViewInfo& View)
{
	if (View.bIsPlanarReflection || View.bIsReflectionCapture)
	{
		return false;
	}

	// Prevent rendering unsupported views when ViewIndex >= GMaxPlanarReflectionViews
	// Planar reflections in those views will fallback to other reflection methods
	{
		int32 ViewIndex = INDEX_NONE;

		View.Family->Views.Find(&View, ViewIndex);

		if (ViewIndex >= GMaxPlanarReflectionViews)
		{
			return false;
		}
	}

	bool bAnyVisiblePlanarReflections = false;
	FScene* Scene = (FScene*)View.Family->Scene;
	for (int32 PlanarReflectionIndex = 0; PlanarReflectionIndex < Scene->PlanarReflections.Num(); PlanarReflectionIndex++)
	{
		FPlanarReflectionSceneProxy* ReflectionSceneProxy = Scene->PlanarReflections[PlanarReflectionIndex];

		if (View.ViewFrustum.IntersectBox(ReflectionSceneProxy->WorldBounds.GetCenter(), ReflectionSceneProxy->WorldBounds.GetExtent()))
		{
			bAnyVisiblePlanarReflections = true;
			break;
		}
	}

	bool bComposePlanarReflections = Scene->PlanarReflections.Num() > 0 && bAnyVisiblePlanarReflections;

	return bComposePlanarReflections;
}

static bool FSR2ShouldRenderRayTracingEffect(bool bEffectEnabled)
{
	if (!IsRayTracingEnabled())
	{
		return false;
	}

	static auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.ForceAllRayTracingEffects"));
	const int32 OverrideMode = CVar != nullptr ? CVar->GetInt() : -1;

	if (OverrideMode >= 0)
	{
		return OverrideMode > 0;
	}
	else
	{
		return bEffectEnabled;
	}
}

static int32 FSR2GetRayTracingReflectionsSamplesPerPixel(const FViewInfo& View)
{
	static IConsoleVariable* RayTracingReflectionSamplesPerPixel = IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.Reflections.SamplesPerPixel"));
	return RayTracingReflectionSamplesPerPixel && RayTracingReflectionSamplesPerPixel->GetInt() >= 0 ? RayTracingReflectionSamplesPerPixel->GetInt() : View.FinalPostProcessSettings.RayTracingReflectionsSamplesPerPixel;
}

static bool FSR2ShouldRenderRayTracingReflections(const FViewInfo& View)
{
	const bool bThisViewHasRaytracingReflections = View.FinalPostProcessSettings.ReflectionMethod == EReflectionMethod::RayTraced;

	static IConsoleVariable* RayTracingReflections = IConsoleManager::Get().FindConsoleVariable(TEXT("r.RayTracing.Reflections"));
	const bool bReflectionsCvarEnabled = RayTracingReflections && RayTracingReflections->GetInt() < 0
		? bThisViewHasRaytracingReflections
		: (RayTracingReflections && RayTracingReflections->GetInt() != 0);

	const bool bReflectionPassEnabled = bReflectionsCvarEnabled && (FSR2GetRayTracingReflectionsSamplesPerPixel(View) > 0);

	return FSR2ShouldRenderRayTracingEffect(bReflectionPassEnabled);
}

bool IsFSR2SSRTemporalPassRequired(const FViewInfo& View)
{
	static const auto CVarSSRTemporalEnabled = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.SSR.Temporal"));
	
	if (!View.State)
	{
		return false;
	}
	return View.AntiAliasingMethod != AAM_TemporalAA || (CVarSSRTemporalEnabled && CVarSSRTemporalEnabled->GetValueOnAnyThread() != 0);
}

#if FSR2_ENABLE_VK
static FfxResource FSR2GetTextureFromRHIResource(IFSR2TemporalUpscalingAPI* ApiAccessor, FfxFsr2Context* context, FRHITexture* Texture, FfxResourceStates state = FFX_RESOURCE_STATE_COMPUTE_READ)
{
	FIntVector Size = Texture->GetSizeXYZ();
	return ffxGetTextureResourceVK(context, (VkImage)Texture->GetNativeResource(), (VkImageView)ApiAccessor->GetNativeTextureResource(Texture), Size.X, Size.Y, (VkFormat)ApiAccessor->GetNativeTextureFormat(Texture), nullptr, state);
}
#endif

static inline float FSR2_GetScreenResolutionFromScalingMode(FfxFsr2QualityMode QualityMode)
{
	return 1.0f / ffxFsr2GetUpscaleRatioFromQualityMode(QualityMode);
}

//------------------------------------------------------------------------------------------------------
// Whether to use Lumen reflection data or not.
//------------------------------------------------------------------------------------------------------
static bool IsUsingLumenReflections(const FViewInfo& View)
{
	const FSceneViewState* ViewState = View.ViewState;
	if (ViewState && View.Family->Views.Num() == 1)
	{
		static const auto CVarLumenEnabled = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.Supported"));
		static const auto CVarLumenReflectionsEnabled = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.Reflections.Allow"));
		return FDataDrivenShaderPlatformInfo::GetSupportsLumenGI(View.GetShaderPlatform())
			&& !IsForwardShadingEnabled(View.GetShaderPlatform())
			&& !View.bIsPlanarReflection
			&& !View.bIsSceneCapture
			&& !View.bIsReflectionCapture
			&& View.State
			&& View.FinalPostProcessSettings.ReflectionMethod == EReflectionMethod::Lumen
			&& View.Family->EngineShowFlags.LumenReflections
			&& CVarLumenEnabled
			&& CVarLumenEnabled->GetInt()
			&& CVarLumenReflectionsEnabled
			&& CVarLumenReflectionsEnabled->GetInt();
	}

	return false;
}

//------------------------------------------------------------------------------------------------------
// Definition of inputs & outputs for the FSR2 FX pass used to copy the SceneColor.
//------------------------------------------------------------------------------------------------------
struct FFSR2FXPass
{
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		RDG_TEXTURE_ACCESS(InputColorTexture, ERHIAccess::CopySrc)
		RDG_TEXTURE_ACCESS(OutputColorTexture, ERHIAccess::CopyDest)
	END_SHADER_PARAMETER_STRUCT()
};

//------------------------------------------------------------------------------------------------------
// The only way to gather all translucency contribution is to compare the SceneColor data prior and after translucency.
// This requires using the FFXSystemInterface which provides a callback invoked after completing opaque rendering of SceneColor.
//------------------------------------------------------------------------------------------------------
class FFSR2FXSystem : public FFXSystemInterface
{
	FGPUSortManager* GPUSortManager;
	FFSR2TemporalUpscaler* Upscaler;
	FRHIUniformBuffer* SceneTexturesUniformParams = nullptr;
public:
	static const FName FXName;

	FFXSystemInterface* GetInterface(const FName& InName) final
	{
		return InName == FFSR2FXSystem::FXName ? this : nullptr;
	}

	void Tick(UWorld*, float DeltaSeconds) final {}

#if WITH_EDITOR
	void Suspend() final {}

	void Resume() final {}
#endif // #if WITH_EDITOR

	void DrawDebug(FCanvas* Canvas) final {}

	void AddVectorField(UVectorFieldComponent* VectorFieldComponent) final {}

	void RemoveVectorField(UVectorFieldComponent* VectorFieldComponent) final {}

	void UpdateVectorField(UVectorFieldComponent* VectorFieldComponent) final {}

	void PreInitViews(FRDGBuilder&, bool, const TArrayView<const FSceneViewFamily*>&, const FSceneViewFamily*) final {};
	void PostInitViews(FRDGBuilder&, TConstStridedView<FSceneView>, bool) final {};


	bool UsesGlobalDistanceField() const final { return false; }

	bool UsesDepthBuffer() const final { return false; }

	bool RequiresEarlyViewUniformBuffer() const final { return false; }

	bool RequiresRayTracingScene() const final { return false; }

	void PreRender(FRDGBuilder&, TConstStridedView<FSceneView>, FSceneUniformBuffer&, bool) final {};
	void PostRenderOpaque(FRDGBuilder& GraphBuilder, TConstStridedView<FSceneView> Views, FSceneUniformBuffer& SceneUniformBuffer, bool bAllowGPUParticleUpdate) final
	{
		static const auto CVarFSR2Enabled = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FidelityFX.FSR2.Enabled"));
		if (CVarFSR2CreateReactiveMask.GetValueOnRenderThread() && Upscaler->IsApiSupported() && (CVarFSR2Enabled && CVarFSR2Enabled->GetValueOnRenderThread()) && Views.Num() > 0)
		{
			FRHIUniformBuffer* ViewUniformBuffer = SceneUniformBuffer.GetBufferRHI(GraphBuilder);

			const FViewInfo& View = (FViewInfo&)(Views[0]);
			//FViewInfo* pView = nullptr;
			//return;
			//const FViewInfo& View = *pView;
			const FSceneTextures* SceneTextures = ((FViewFamilyInfo*)View.Family)->GetSceneTexturesChecked();

			FRDGTextureMSAA PreAlpha = SceneTextures->Color;
			auto const& Config = SceneTextures->Config;

			EPixelFormat SceneColorFormat = Config.ColorFormat;
			uint32 NumSamples = Config.NumSamples;

			FIntPoint SceneColorSize = FIntPoint::ZeroValue;

			SceneColorSize.X = FMath::Max(SceneColorSize.X, View.ViewRect.Max.X);
			SceneColorSize.Y = FMath::Max(SceneColorSize.Y, View.ViewRect.Max.Y);

			check(SceneColorSize.X > 0 && SceneColorSize.Y > 0);

			FIntPoint QuantizedSize;
			QuantizeSceneBufferSize(SceneColorSize, QuantizedSize);

			if (Upscaler->SceneColorPreAlpha.GetReference())
			{
				if (Upscaler->SceneColorPreAlpha->GetSizeX() != QuantizedSize.X
					|| Upscaler->SceneColorPreAlpha->GetSizeY() != QuantizedSize.Y
					|| Upscaler->SceneColorPreAlpha->GetFormat() != SceneColorFormat
					|| Upscaler->SceneColorPreAlpha->GetNumSamples() != NumSamples)
				{
					Upscaler->SceneColorPreAlpha.SafeRelease();
					Upscaler->SceneColorPreAlphaRT.SafeRelease();
				}
			}

			if (Upscaler->SceneColorPreAlpha.GetReference() == nullptr)
			{
				FRHITextureCreateDesc SceneColorPreAlphaCreateDesc = FRHITextureCreateDesc::Create2D(TEXT("FSR2SceneColorPreAlpha"), QuantizedSize.X, QuantizedSize.Y, SceneColorFormat);
				SceneColorPreAlphaCreateDesc.SetNumMips(1);
				SceneColorPreAlphaCreateDesc.SetNumSamples(NumSamples);
				SceneColorPreAlphaCreateDesc.SetFlags((ETextureCreateFlags)(ETextureCreateFlags::RenderTargetable | ETextureCreateFlags::ShaderResource));
				Upscaler->SceneColorPreAlpha = RHICreateTexture(SceneColorPreAlphaCreateDesc);
				Upscaler->SceneColorPreAlphaRT = CreateRenderTarget(Upscaler->SceneColorPreAlpha.GetReference(), TEXT("FSR2SceneColorPreAlpha"));
			}

			FFSR2FXPass::FParameters* PassParameters = GraphBuilder.AllocParameters<FFSR2FXPass::FParameters>();
			FRDGTextureRef SceneColorPreAlphaRDG = GraphBuilder.RegisterExternalTexture(Upscaler->SceneColorPreAlphaRT);
			PassParameters->InputColorTexture = PreAlpha.Target;
			PassParameters->OutputColorTexture = SceneColorPreAlphaRDG;

			GraphBuilder.AddPass(RDG_EVENT_NAME("FFSR2FXSystem::PostRenderOpaque"), PassParameters, ERDGPassFlags::Copy,
				[this, PassParameters, ViewUniformBuffer, PreAlpha](FRHICommandListImmediate& RHICmdList)
				{
					PassParameters->InputColorTexture->MarkResourceAsUsed();
					PassParameters->OutputColorTexture->MarkResourceAsUsed();
					Upscaler->PreAlpha = PreAlpha;
					Upscaler->CopyOpaqueSceneColor(RHICmdList, ViewUniformBuffer, nullptr, this->SceneTexturesUniformParams);
				}
			);
		}
	}

	void SetSceneTexturesUniformBuffer(const TUniformBufferRef<FSceneTextureUniformParameters>& InSceneTexturesUniformParams) final { SceneTexturesUniformParams = InSceneTexturesUniformParams; }

	FGPUSortManager* GetGPUSortManager() const 
	{
		return GPUSortManager;
	}

	FFSR2FXSystem(FFSR2TemporalUpscaler* InUpscaler, FGPUSortManager* InGPUSortManager)
	: GPUSortManager(InGPUSortManager)
		, Upscaler(InUpscaler)
	{
		check(GPUSortManager && Upscaler);
	}
	~FFSR2FXSystem() {}
};
FName const FFSR2FXSystem::FXName(TEXT("FSR2FXSystem"));

//------------------------------------------------------------------------------------------------------
// Definition of inputs & outputs for the FSR2 pass used by the native backends.
//------------------------------------------------------------------------------------------------------
struct FFSR2Pass
{
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		RDG_TEXTURE_ACCESS(ColorTexture, ERHIAccess::SRVMask)
		RDG_TEXTURE_ACCESS(DepthTexture, ERHIAccess::SRVMask)
		RDG_TEXTURE_ACCESS(VelocityTexture, ERHIAccess::SRVMask)
		RDG_TEXTURE_ACCESS(ExposureTexture, ERHIAccess::SRVMask)
		RDG_TEXTURE_ACCESS(ReactiveMaskTexture, ERHIAccess::SRVMask)
		RDG_TEXTURE_ACCESS(CompositeMaskTexture, ERHIAccess::SRVMask)
		RDG_TEXTURE_ACCESS(OutputTexture, ERHIAccess::UAVMask)
	END_SHADER_PARAMETER_STRUCT()
};

//------------------------------------------------------------------------------------------------------
// FFSR2TemporalUpscaler implementation.
//------------------------------------------------------------------------------------------------------

FFSR2TemporalUpscaler::FFSR2TemporalUpscaler()
: Api(EFSR2TemporalUpscalerAPI::Unknown)
, ApiAccessor(nullptr)
, CurrentGraphBuilder(nullptr)
, WrappedDenoiser(nullptr)
, ReflectionTexture(nullptr)
{
	FMemory::Memzero(PostInputs);

	PreAlpha.Target = nullptr;
	PreAlpha.Resolve = nullptr;

#if WITH_EDITOR
	bEnabledInEditor = true;
#endif

	FFSR2TemporalUpscaler* self = this;
	FFXSystemInterface::RegisterCustomFXSystem(
		FFSR2FXSystem::FXName, 
		FCreateCustomFXSystemDelegate::CreateLambda([self](ERHIFeatureLevel::Type InFeatureLevel, EShaderPlatform InShaderPlatform, FGPUSortManager* InGPUSortManager) -> FFXSystemInterface*
	{
		return new FFSR2FXSystem(self, InGPUSortManager);
	}));

	FConsoleVariableDelegate EnabledChangedDelegate = FConsoleVariableDelegate::CreateStatic(&FFSR2TemporalUpscaler::OnChangeFSR2Enabled);
	IConsoleVariable* CVarEnabled = IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR2.Enabled"));
	CVarEnabled->SetOnChangedCallback(EnabledChangedDelegate);

	FConsoleVariableDelegate QualityModeChangedDelegate = FConsoleVariableDelegate::CreateStatic(&FFSR2TemporalUpscaler::OnChangeFSR2QualityMode);
	CVarFSR2QualityMode->SetOnChangedCallback(QualityModeChangedDelegate);

	if (CVarEnabled->GetBool())
	{
		SaveScreenPercentage();
		UpdateScreenPercentage();
	}
}

FFSR2TemporalUpscaler::~FFSR2TemporalUpscaler()
{
	DeferredCleanup();
	if (ApiAccessor)
	{
		delete ApiAccessor;
	}
	FFXSystemInterface::UnregisterCustomFXSystem(FFSR2FXSystem::FXName);
}

const TCHAR* FFSR2TemporalUpscaler::GetDebugName() const
{
	return TEXT("FFSR2TemporalUpscaler");
}

void FFSR2TemporalUpscaler::ReleaseState(FSR2StateRef State)
{
	FFSR2State* Ptr = State.GetReference();
	if (Ptr)
	{
		Ptr->AddRef();
		AvailableStates.Push(Ptr);
	}
}

void FFSR2TemporalUpscaler::DeferredCleanup() const
{
	FFSR2State* Ptr = AvailableStates.Pop();
	while (Ptr)
	{
		Ptr->Release();
		Ptr = AvailableStates.Pop();
	}
}

float FFSR2TemporalUpscaler::GetResolutionFraction(uint32 Mode)
{
	FfxFsr2QualityMode QualityMode = FMath::Clamp<FfxFsr2QualityMode>((FfxFsr2QualityMode)Mode, HighestResolutionQualityMode, LowestResolutionQualityMode);
	const float ResolutionFraction = FSR2_GetScreenResolutionFromScalingMode(QualityMode);
	return ResolutionFraction;
}

#if DO_CHECK || DO_GUARD_SLOW || DO_ENSURE
void FFSR2TemporalUpscaler::OnFSR2Message(FfxFsr2MsgType type, const wchar_t* message)
{
	if (type == FFX_FSR2_MESSAGE_TYPE_ERROR)
	{
		UE_LOG(LogFSR2API, Error, TEXT("%s"), message);
	}
	else if (type == FFX_FSR2_MESSAGE_TYPE_WARNING)
	{
		UE_LOG(LogFSR2API, Warning, TEXT("%s"), message);
	}
}
#endif // DO_CHECK || DO_GUARD_SLOW || DO_ENSURE

void FFSR2TemporalUpscaler::SaveScreenPercentage()
{
	SavedScreenPercentage = IConsoleManager::Get().FindTConsoleVariableDataFloat(TEXT("r.ScreenPercentage"))->GetValueOnGameThread();
}

void FFSR2TemporalUpscaler::UpdateScreenPercentage()
{
	float ResolutionFraction = GetResolutionFraction(CVarFSR2QualityMode.GetValueOnGameThread());
	static IConsoleVariable* ScreenPercentage = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ScreenPercentage"));
	ScreenPercentage->Set(ResolutionFraction * 100.0f);
}

void FFSR2TemporalUpscaler::RestoreScreenPercentage()
{
	static IConsoleVariable* ScreenPercentage = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ScreenPercentage"));
	ScreenPercentage->Set(SavedScreenPercentage);
}

void FFSR2TemporalUpscaler::OnChangeFSR2Enabled(IConsoleVariable* Var)
{
	static const auto CVarFSR2Enabled = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FidelityFX.FSR2.Enabled"));
	if (CVarFSR2Enabled->GetValueOnGameThread())
	{
		SaveScreenPercentage();
		UpdateScreenPercentage();
	}
	else
	{
		RestoreScreenPercentage();
	}
}

void FFSR2TemporalUpscaler::OnChangeFSR2QualityMode(IConsoleVariable* Var)
{
	static const auto CVarFSR2Enabled = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FidelityFX.FSR2.Enabled"));
	if (CVarFSR2Enabled->GetValueOnGameThread())
	{
		UpdateScreenPercentage();
	}
}

FRDGBuilder* FFSR2TemporalUpscaler::GetGraphBuilder()
{
	return CurrentGraphBuilder;
}

void FFSR2TemporalUpscaler::Initialize() const
{
	if (Api == EFSR2TemporalUpscalerAPI::Unknown)
	{
		FString RHIName = GDynamicRHI->GetName();

		// Prefer the native backends unless they've been disabled
#if FSR2_ENABLE_DX12
		if (RHIName == FFSR2Strings::D3D12 && CVarFSR2UseNativeDX12.GetValueOnAnyThread())
		{
			Api = EFSR2TemporalUpscalerAPI::D3D12;
			ApiAccessor = new FFSR2TemporalUpscalingD3D12();
		}
		else 
#endif
#if FSR2_ENABLE_VK
		if (RHIName == FFSR2Strings::Vulkan && CVarFSR2UseNativeVulkan.GetValueOnAnyThread() && FFFXFSR2Vulkan::IsNativeBackendSupported() &&
			FSR2VulkanLoader::Get().LoadVulkan(static_cast<VkInstance>(GDynamicRHI->RHIGetNativeInstance())))
		{
			Api = EFSR2TemporalUpscalerAPI::Vulkan;
			ApiAccessor = new FFSR2TemporalUpscalingVulkan();
		}
		else
#endif
		// The fallback implementation requires SM5
		if (IsFeatureLevelSupported(GMaxRHIShaderPlatform, ERHIFeatureLevel::SM5))
		{
			Api = EFSR2TemporalUpscalerAPI::Unreal;
		}
		else
		{
			Api = EFSR2TemporalUpscalerAPI::Unsupported;
			UE_LOG(LogFSR2, Error, TEXT("FSR2 Temporal Upscaler not supported by '%s' rhi"), *RHIName);
		}

		if (IsApiSupported())
		{
			// Wrap any existing denoiser API as we override this to be able to generate the reactive mask.
			WrappedDenoiser = GScreenSpaceDenoiser;
			if (!WrappedDenoiser)
			{
				WrappedDenoiser = IScreenSpaceDenoiser::GetDefaultDenoiser();
			}
			check(WrappedDenoiser);
			GScreenSpaceDenoiser = this;
		}
	}
}

IFSR2TemporalUpscaler::FOutputs FFSR2TemporalUpscaler::AddPasses(
	FRDGBuilder& GraphBuilder,
	const FSceneView& SceneView,
	const FFSR2PassInputs& PassInputs) const
{
	const FRDGTextureRef SceneColorTexture = PassInputs.SceneColor.Texture;
	const FRDGTextureRef SceneDepthTexture = PassInputs.SceneDepth.Texture;

	// In the MovieRenderPipeline the output extents can be smaller than the input, FSR2 doesn't handle that.
	// In that case we shall fall back to the default upscaler so we render properly.
	const FViewInfo& View = (FViewInfo&)(SceneView);
	FIntPoint InputExtents = View.ViewRect.Size();
	FIntPoint InputExtentsQuantized;
	FIntPoint OutputExtents = View.GetSecondaryViewRectSize();
	FIntPoint OutputExtentsQuantized;
	OutputExtents = FIntPoint(FMath::Max(InputExtents.X, OutputExtents.X), FMath::Max(InputExtents.Y, OutputExtents.Y));

	Initialize();

	bool const bValidEyeAdaptation = View.HasValidEyeAdaptationBuffer();
	bool const bRequestedAutoExposure = static_cast<bool>(CVarFSR2AutoExposure.GetValueOnRenderThread());
	bool const bUseAutoExposure = bRequestedAutoExposure || !bValidEyeAdaptation;

	PreAlpha.Target = nullptr;
	PreAlpha.Resolve = nullptr;

	// The API must be supported, the underlying code has to handle downscaling as well as upscaling (I'll write some Unreal code to handle that).
	check(IsApiSupported());
	{
		ITemporalUpscaler::FOutputs Outputs;

		RDG_GPU_STAT_SCOPE(GraphBuilder, FidelityFXSuperResolution2Pass);
		RDG_EVENT_SCOPE(GraphBuilder, "FidelityFXSuperResolution2Pass");

		CurrentGraphBuilder = &GraphBuilder;

		const bool CanWritePrevViewInfo = !View.bStatePrevViewInfoIsReadOnly && View.ViewState;

		bool bHistoryValid = View.PrevViewInfo.TemporalAAHistory.IsValid() && View.ViewState && !View.bCameraCut;

		FRDGTextureRef SceneColor = PassInputs.SceneColor.Texture;
		FRDGTextureRef SceneDepth = PassInputs.SceneDepth.Texture;
		FRDGTextureRef VelocityTexture = PassInputs.SceneVelocity.Texture;

		// Quantize the buffers to match UE behavior
		QuantizeSceneBufferSize(InputExtents, InputExtentsQuantized);
		QuantizeSceneBufferSize(OutputExtents, OutputExtentsQuantized);

		//------------------------------------------------------------------------------------------------------
		// Create Reactive Mask
		//   Create a reactive mask from separate translucency.
		//------------------------------------------------------------------------------------------------------
		
		if (!VelocityTexture)
		{
			VelocityTexture = (*PostInputs.SceneTextures)->GBufferVelocityTexture;
		}

		FIntPoint InputTextureExtents = CVarFSR2QuantizeInternalTextures.GetValueOnRenderThread() ? InputExtentsQuantized : InputExtents;
		FRDGTextureSRVDesc DepthDesc = FRDGTextureSRVDesc::Create(SceneDepth);
		FRDGTextureSRVDesc VelocityDesc = FRDGTextureSRVDesc::Create(VelocityTexture);
		FRDGTextureDesc ReactiveMaskDesc = FRDGTextureDesc::Create2D(InputTextureExtents, PF_R8, FClearValueBinding::Black, TexCreate_ShaderResource | TexCreate_UAV | TexCreate_RenderTargetable);
		FRDGTextureRef ReactiveMaskTexture = nullptr;
		FRDGTextureDesc CompositeMaskDesc = FRDGTextureDesc::Create2D(InputTextureExtents, PF_R8, FClearValueBinding::Black, TexCreate_ShaderResource | TexCreate_UAV | TexCreate_RenderTargetable);
		FRDGTextureRef CompositeMaskTexture = nullptr;
		FRDGTextureDesc SceneColorDesc = FRDGTextureDesc::Create2D(InputTextureExtents, SceneColor->Desc.Format, FClearValueBinding::Black, TexCreate_ShaderResource | TexCreate_UAV | TexCreate_RenderTargetable);

		if (CVarFSR2CreateReactiveMask.GetValueOnRenderThread())
		{
			ReactiveMaskTexture = GraphBuilder.CreateTexture(ReactiveMaskDesc, TEXT("FSR2ReactiveMaskTexture"));
			CompositeMaskTexture = GraphBuilder.CreateTexture(CompositeMaskDesc, TEXT("FSR2CompositeMaskTexture"));
			{
				FFSR2CreateReactiveMaskCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FFSR2CreateReactiveMaskCS::FParameters>();
				PassParameters->Sampler = TStaticSamplerState<SF_Point>::GetRHI();

				FRDGTextureRef GBufferB = (*PostInputs.SceneTextures)->GBufferBTexture;
				if (!GBufferB)
				{
					GBufferB = GraphBuilder.RegisterExternalTexture(GSystemTextures.BlackDummy);
				}

				FRDGTextureRef GBufferD = (*PostInputs.SceneTextures)->GBufferDTexture;
				if (!GBufferD)
				{
					GBufferD = GraphBuilder.RegisterExternalTexture(GSystemTextures.BlackDummy);
				}

				FRDGTextureRef Reflections = ReflectionTexture;
				if (!Reflections)
				{
					Reflections = GraphBuilder.RegisterExternalTexture(GSystemTextures.BlackDummy);
				}

				PassParameters->DepthTexture = SceneDepth;
				PassParameters->InputDepth = GraphBuilder.CreateSRV(DepthDesc);

				FRDGTextureSRVDesc SceneColorSRV = FRDGTextureSRVDesc::Create(SceneColor);
				PassParameters->SceneColor = GraphBuilder.CreateSRV(SceneColorSRV);

				//------------------------------------------------------------------------------------------------------
				// Capturing the scene color pre-alpha requires allocating the texture here, but keeping a reference to it.
				// The texture will be filled in later in the CopyOpaqueSceneColor function.
				//------------------------------------------------------------------------------------------------------
				EPixelFormat SceneColorFormat = SceneColorDesc.Format;

				if (SceneColorPreAlphaRT)
				{
					FRDGTextureRef SceneColorPreAlphaRDG = GraphBuilder.RegisterExternalTexture(SceneColorPreAlphaRT);
					FRDGTextureSRVDesc SceneColorPreAlphaSRV = FRDGTextureSRVDesc::Create(SceneColorPreAlphaRDG);
					PassParameters->SceneColorPreAlpha = GraphBuilder.CreateSRV(SceneColorPreAlphaSRV);
				}
				else
				{
					PassParameters->SceneColorPreAlpha = GraphBuilder.CreateSRV(SceneColorSRV);
				}

				PassParameters->InputVelocity = GraphBuilder.CreateSRV(VelocityDesc);
				
				FRDGTextureRef LumenSpecular;
				FRDGTextureRef CurrentLumenSpecular = nullptr;

				if ((CurrentLumenSpecular || LumenReflections.IsValid()) && bHistoryValid && IsUsingLumenReflections(View))
				{
					LumenSpecular = CurrentLumenSpecular ? CurrentLumenSpecular : GraphBuilder.RegisterExternalTexture(LumenReflections);
				}
				else
				{
					LumenSpecular = GraphBuilder.RegisterExternalTexture(GSystemTextures.BlackDummy);
				}

				FRDGTextureSRVDesc LumenSpecularDesc = FRDGTextureSRVDesc::Create(LumenSpecular);
				PassParameters->LumenSpecular = GraphBuilder.CreateSRV(LumenSpecularDesc);
				PassParameters->LumenSpecularCurrentFrame = (CurrentLumenSpecular && LumenSpecular == CurrentLumenSpecular);

				FRDGTextureSRVDesc GBufferBDesc = FRDGTextureSRVDesc::Create(GBufferB);
				FRDGTextureSRVDesc GBufferDDesc = FRDGTextureSRVDesc::Create(GBufferD);
				FRDGTextureSRVDesc ReflectionsDesc = FRDGTextureSRVDesc::Create(Reflections);
				FRDGTextureUAVDesc ReactiveDesc(ReactiveMaskTexture);
				FRDGTextureUAVDesc CompositeDesc(CompositeMaskTexture);

				PassParameters->GBufferB = GraphBuilder.CreateSRV(GBufferBDesc);
				PassParameters->GBufferD = GraphBuilder.CreateSRV(GBufferDDesc);
				PassParameters->ReflectionTexture = GraphBuilder.CreateSRV(ReflectionsDesc);

				PassParameters->View = View.ViewUniformBuffer;

				PassParameters->ReactiveMask = GraphBuilder.CreateUAV(ReactiveDesc);
				PassParameters->CompositeMask = GraphBuilder.CreateUAV(CompositeDesc);

				PassParameters->FurthestReflectionCaptureDistance = CVarFSR2ReactiveMaskRoughnessForceMaxDistance.GetValueOnRenderThread() ? CVarFSR2ReactiveMaskRoughnessMaxDistance.GetValueOnRenderThread() : FMath::Max(CVarFSR2ReactiveMaskRoughnessMaxDistance.GetValueOnRenderThread(), View.FurthestReflectionCaptureDistance);
				PassParameters->ReactiveMaskReflectionScale = CVarFSR2ReactiveMaskReflectionScale.GetValueOnRenderThread();
				PassParameters->ReactiveMaskRoughnessScale = CVarFSR2ReactiveMaskRoughnessScale.GetValueOnRenderThread();
				PassParameters->ReactiveMaskRoughnessBias = CVarFSR2ReactiveMaskRoughnessBias.GetValueOnRenderThread();
				PassParameters->ReactiveMaskReflectionLumaBias = CVarFSR2ReactiveMaskReflectionLumaBias.GetValueOnRenderThread();
				PassParameters->ReactiveHistoryTranslucencyBias = CVarFSR2ReactiveHistoryTranslucencyBias.GetValueOnRenderThread();
				PassParameters->ReactiveHistoryTranslucencyLumaBias = CVarFSR2ReactiveHistoryTranslucencyLumaBias.GetValueOnRenderThread();
				PassParameters->ReactiveMaskTranslucencyBias = CVarFSR2ReactiveMaskTranslucencyBias.GetValueOnRenderThread();
				PassParameters->ReactiveMaskTranslucencyLumaBias = CVarFSR2ReactiveMaskTranslucencyLumaBias.GetValueOnRenderThread();

				PassParameters->ReactiveMaskTranslucencyMaxDistance = CVarFSR2ReactiveMaskTranslucencyMaxDistance.GetValueOnRenderThread();
				PassParameters->ForceLitReactiveValue = CVarFSR2ReactiveMaskForceReactiveMaterialValue.GetValueOnRenderThread();
				PassParameters->ReactiveShadingModelID = (uint32)CVarFSR2ReactiveMaskReactiveShadingModelID.GetValueOnRenderThread();

				TShaderMapRef<FFSR2CreateReactiveMaskCS> ComputeShaderFSR(View.ShaderMap);
				FComputeShaderUtils::AddPass(
					GraphBuilder,
					RDG_EVENT_NAME("FidelityFX-FSR2/CreateReactiveMask (CS)"),
					ComputeShaderFSR,
					PassParameters,
					FComputeShaderUtils::GetGroupCount(FIntVector(InputExtents.X, InputExtents.Y, 1),
						FIntVector(FFSR2ConvertVelocityCS::ThreadgroupSizeX, FFSR2ConvertVelocityCS::ThreadgroupSizeY, FFSR2ConvertVelocityCS::ThreadgroupSizeZ))
				);
			}
		}
		else
		{
			ReactiveMaskTexture = GraphBuilder.RegisterExternalTexture(GSystemTextures.BlackDummy);
			CompositeMaskTexture = GraphBuilder.RegisterExternalTexture(GSystemTextures.BlackDummy);
		}

		// If we are set to de-dither rendering then run the extra pass now - this tries to identify dither patterns and blend them to avoid over-thinning in FSR2.
		// There is specific code for SHADINGMODELID_HAIR pixels which are always dithered.
		if (CVarFSR2DeDitherMode.GetValueOnRenderThread() && (*PostInputs.SceneTextures)->GBufferBTexture)
		{
			SceneColor = GraphBuilder.CreateTexture(SceneColorDesc, TEXT("FSR2SubrectColor"));
			FFSR2DeDitherCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FFSR2DeDitherCS::FParameters>();
			FRDGTextureUAVDesc OutputDesc(SceneColor);

			FRDGTextureRef GBufferB = (*PostInputs.SceneTextures)->GBufferBTexture;
			FRDGTextureSRVDesc GBufferBDesc = FRDGTextureSRVDesc::Create(GBufferB);
			PassParameters->GBufferB = GraphBuilder.CreateSRV(GBufferBDesc);
			FRDGTextureSRVDesc SceneColorSRV = FRDGTextureSRVDesc::Create(SceneColorTexture);
			PassParameters->SceneColor = GraphBuilder.CreateSRV(SceneColorSRV);

			PassParameters->View = View.ViewUniformBuffer;

			PassParameters->BlendSceneColor = GraphBuilder.CreateUAV(OutputDesc);

			// Full de-dither requires the proper setting or not running on the Deferred renderer where we can't determine the shading model.
			PassParameters->FullDeDither = (CVarFSR2DeDitherMode.GetValueOnRenderThread() == 1) || (!GBufferB);
			if (!GBufferB)
			{
				GBufferB = GraphBuilder.RegisterExternalTexture(GSystemTextures.BlackDummy);
			}

			TShaderMapRef<FFSR2DeDitherCS> ComputeShaderFSR(View.ShaderMap);
			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("FidelityFX-FSR2/DeDither (CS)"),
				ComputeShaderFSR,
				PassParameters,
				FComputeShaderUtils::GetGroupCount(FIntVector(SceneColor->Desc.Extent.X, SceneColor->Desc.Extent.Y, 1),
				FIntVector(FFSR2DeDitherCS::ThreadgroupSizeX, FFSR2DeDitherCS::ThreadgroupSizeY, FFSR2DeDitherCS::ThreadgroupSizeZ))
			);
		}

		//------------------------------------------------------------------------------------------------------
		// Consolidate Motion Vectors
		//   UE4 motion vectors are in sparse format by default.  Convert them to a format consumable by FSR2.
		//------------------------------------------------------------------------------------------------------
		FRDGTextureDesc MotionVectorDesc = FRDGTextureDesc::Create2D(InputExtentsQuantized, PF_G16R16F, FClearValueBinding::Black, TexCreate_ShaderResource | TexCreate_UAV | TexCreate_RenderTargetable);
		FRDGTextureRef MotionVectorTexture = GraphBuilder.CreateTexture(MotionVectorDesc, TEXT("FSR2MotionVectorTexture"));
		{
			FFSR2ConvertVelocityCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FFSR2ConvertVelocityCS::FParameters>();
			FRDGTextureUAVDesc OutputDesc(MotionVectorTexture);

			PassParameters->DepthTexture = SceneDepth;
			PassParameters->InputDepth = GraphBuilder.CreateSRV(DepthDesc);
			PassParameters->InputVelocity = GraphBuilder.CreateSRV(VelocityDesc);

			PassParameters->View = View.ViewUniformBuffer;

			PassParameters->OutputTexture = GraphBuilder.CreateUAV(OutputDesc);

			TShaderMapRef<FFSR2ConvertVelocityCS> ComputeShaderFSR(View.ShaderMap);
			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("FidelityFX-FSR2/ConvertVelocity (CS)"),
				ComputeShaderFSR,
				PassParameters,
				FComputeShaderUtils::GetGroupCount(FIntVector(SceneDepth->Desc.Extent.X, SceneDepth->Desc.Extent.Y, 1),
					FIntVector(FFSR2ConvertVelocityCS::ThreadgroupSizeX, FFSR2ConvertVelocityCS::ThreadgroupSizeY, FFSR2ConvertVelocityCS::ThreadgroupSizeZ))
			);
		}

		//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Handle Multiple Viewports
		//   The FSR2 API currently doesn't handle offsetting into buffers.  If the current viewport is not the top left viewport, generate a new texture in which this viewport is at (0,0).
		//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		if (View.ViewRect.Min != FIntPoint::ZeroValue)
		{
			if (!CVarFSR2DeDitherMode.GetValueOnRenderThread())
			{
				SceneColor = GraphBuilder.CreateTexture(SceneColorDesc, TEXT("FSR2SubrectColor"));

				AddCopyTexturePass(
					GraphBuilder,
					SceneColorTexture,
					SceneColor,
					View.ViewRect.Min,
					FIntPoint::ZeroValue,
					View.ViewRect.Size());
			}

			FRDGTextureDesc SplitDepthDesc = FRDGTextureDesc::Create2D(InputExtentsQuantized, SceneDepth->Desc.Format, FClearValueBinding::Black, SceneDepth->Desc.Flags);
			SceneDepth = GraphBuilder.CreateTexture(SplitDepthDesc, TEXT("FSR2SubrectDepth"));

			AddCopyTexturePass(
					GraphBuilder,
					SceneDepthTexture,
					SceneDepth,
					View.ViewRect.Min,
					FIntPoint::ZeroValue,
					View.ViewRect.Size());
		}

		//-------------------
		// Create Resources
		//-------------------
		// Whether alpha channel is supported.
		static const auto CVarPostPropagateAlpha = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.PostProcessing.PropagateAlpha"));
		const bool bSupportsAlpha = (CVarPostPropagateAlpha && CVarPostPropagateAlpha->GetValueOnRenderThread() != 0);
		EPixelFormat OutputFormat = (bSupportsAlpha || (CVarFSR2HistoryFormat.GetValueOnRenderThread() == 0)) ? PF_FloatRGBA : PF_FloatR11G11B10;

		FRDGTextureDesc OutputColorDesc = FRDGTextureDesc::Create2D(OutputExtentsQuantized, OutputFormat, FClearValueBinding::Black, TexCreate_ShaderResource | TexCreate_UAV | TexCreate_RenderTargetable);
		FRDGTextureRef OutputTexture = GraphBuilder.CreateTexture(OutputColorDesc, TEXT("FSR2OutputTexture"));

		Outputs.FullRes.Texture = OutputTexture;
		Outputs.FullRes.ViewRect = FIntRect(FIntPoint::ZeroValue, View.GetSecondaryViewRectSize());

		//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Initialize the FSR2 Context
		//   If a context has never been created, or if significant features of the frame have changed since the current context was created, tear down any existing contexts and create a new one matching the current frame.
		//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FSR2StateRef FSR2State;
		const TRefCountPtr<IFSR2CustomTemporalAAHistory> PrevCustomHistory = PassInputs.PrevHistory;
		FFSR2TemporalUpscalerHistory* CustomHistory = static_cast<FFSR2TemporalUpscalerHistory*>(PrevCustomHistory.GetReference());
		{
			// FSR setup
			FfxFsr2ContextDescription Params;
			FMemory::Memzero(Params);

			//------------------------------------------------------------------------------------------------------------------------------------------------------------------
			// Describe the Current Frame
			//   Collect the features of the current frame and the current FSR2 history, so we can make decisions about whether any existing FSR2 context is currently usable.
			//------------------------------------------------------------------------------------------------------------------------------------------------------------------
			switch (Api)
			{
#if FSR2_ENABLE_DX12
				case EFSR2TemporalUpscalerAPI::D3D12:
				{
					ID3D12Device* device = (ID3D12Device*)GDynamicRHI->RHIGetNativeDevice();
					Params.device = ffxGetDeviceDX12(device);
					break;
				}
#endif
#if FSR2_ENABLE_VK
				case EFSR2TemporalUpscalerAPI::Vulkan:
				{
					Params.device = ffxGetDeviceVK(static_cast<VkDevice>(GDynamicRHI->RHIGetNativeDevice()));
					break;
				}
#endif
				default:
				{
					Params.device = (FfxDevice)this;
					break;
				}
			}

			// FSR settings
			{
				// Engine params:
				Params.flags = 0;
				Params.flags |= bool(ERHIZBuffer::IsInverted) ? FFX_FSR2_ENABLE_DEPTH_INVERTED : 0;
				Params.flags |= FFX_FSR2_ENABLE_HIGH_DYNAMIC_RANGE | FFX_FSR2_ENABLE_DEPTH_INFINITE;
				Params.flags |= ((DynamicResolutionStateInfos.Status == EDynamicResolutionStatus::Enabled) || (DynamicResolutionStateInfos.Status == EDynamicResolutionStatus::DebugForceEnabled)) ? FFX_FSR2_ENABLE_DYNAMIC_RESOLUTION : 0;
				Params.displaySize.height = OutputExtents.Y;
				Params.displaySize.width = OutputExtents.X;
				Params.maxRenderSize.height = InputExtents.Y;
				Params.maxRenderSize.width = InputExtents.X;

				// CVar params:
				// Compute Auto Exposure requires wave operations or D3D12.
				Params.flags |= bUseAutoExposure ? FFX_FSR2_ENABLE_AUTO_EXPOSURE : 0;

#if DO_CHECK || DO_GUARD_SLOW || DO_ENSURE
				// Register message callback
				Params.flags |= FFX_FSR2_ENABLE_DEBUG_CHECKING;
				Params.fpMessage = &FFSR2TemporalUpscaler::OnFSR2Message;
#endif // DO_CHECK || DO_GUARD_SLOW || DO_ENSURE
			}

			// We want to reuse FSR2 states rather than recreating them wherever possible as they allocate significant memory for their internal resources.
			// The current custom history is the ideal, but the recently released states can be reused with a simple reset too when the engine cuts the history.
			// This reduces the memory churn imposed by camera cuts.
			bool HasValidContext = CustomHistory && CustomHistory->GetState().IsValid();
			if (HasValidContext)
			{
				FfxFsr2ContextDescription const& CurrentParams = CustomHistory->GetState()->Params;
				if ((CustomHistory->GetState()->LastUsedFrame == GFrameCounterRenderThread) || (CurrentParams.maxRenderSize.width < Params.maxRenderSize.width) || (CurrentParams.maxRenderSize.height < Params.maxRenderSize.height) || (CurrentParams.displaySize.width != Params.displaySize.width) || (CurrentParams.displaySize.height != Params.displaySize.height) || (Params.flags != CurrentParams.flags) || (Params.device != CurrentParams.device))
				{
					HasValidContext = false;
				}
				else
				{
					FSR2State = CustomHistory->GetState();
				}
			}

			if (!HasValidContext)
			{
				TLockFreePointerListFIFO<FFSR2State, PLATFORM_CACHE_LINE_SIZE> ReusableStates;
				FFSR2State* Ptr = AvailableStates.Pop();
				while (Ptr)
				{
					FfxFsr2ContextDescription const& CurrentParams = Ptr->Params;
					if (Ptr->LastUsedFrame == GFrameCounterRenderThread && Ptr->ViewID != View.ViewState->UniqueID)
					{
						// These states can't be reused immediately but perhaps a future frame, otherwise we break split screen.
						ReusableStates.Push(Ptr);
					}
					else if ((CurrentParams.maxRenderSize.width < Params.maxRenderSize.width) || (CurrentParams.maxRenderSize.height < Params.maxRenderSize.height) || (CurrentParams.displaySize.width != Params.displaySize.width) || (CurrentParams.displaySize.height != Params.displaySize.height) || (Params.flags != CurrentParams.flags) || (Params.device != CurrentParams.device))
					{
						// States that can't be trivially reused need to just be released to save memory.
						Ptr->Release();
					}
					else
					{
						FSR2State = Ptr;
						Ptr->Release();
						HasValidContext = true;
						bHistoryValid = false;
						break;
					}
					Ptr = AvailableStates.Pop();
				}

				Ptr = ReusableStates.Pop();
				while (Ptr)
				{
					AvailableStates.Push(Ptr);
					Ptr = ReusableStates.Pop();
				}
			}

			if (!HasValidContext)
			{
				// For a new context, allocate the necessary scratch memory for the chosen backend
				uint32 ScratchSize = 0;
				switch (Api)
				{
#if FSR2_ENABLE_DX12
					case EFSR2TemporalUpscalerAPI::D3D12:
					{
						ScratchSize = ffxFsr2GetScratchMemorySizeDX12();
						break;
					}
#endif
#if FSR2_ENABLE_VK
					case EFSR2TemporalUpscalerAPI::Vulkan:
					{
						VkPhysicalDevice VulkanDevice = ((IVulkanDynamicRHI*)GDynamicRHI)->RHIGetVkPhysicalDevice();
						ScratchSize = ffxFsr2GetScratchMemorySizeVK(VulkanDevice);
						break;
				}
#endif
					default:
					{
						ScratchSize = ffxFsr2GetScratchMemorySize();
						break;
					}
				}

				FSR2State = new FFSR2State(ScratchSize);
			}

			FSR2State->LastUsedFrame = GFrameCounterRenderThread;
			FSR2State->ViewID = View.ViewState->UniqueID;

			//-------------------------------------------------------------------------------------------------------------------------------------------------
			// Update History Data (Part 1)
			//   Prepare the view to receive this frame's history data.  This must be done before any attempt to re-create an FSR2 context, if that's needed.
			//-------------------------------------------------------------------------------------------------------------------------------------------------
			if (CanWritePrevViewInfo)
			{
				// Releases the existing history texture inside the wrapper object, this doesn't release NewHistory itself
				View.ViewState->PrevFrameViewInfo.TemporalAAHistory.SafeRelease();

				View.ViewState->PrevFrameViewInfo.TemporalAAHistory.ViewportRect = FIntRect(0, 0, OutputExtents.X, OutputExtents.Y);
				View.ViewState->PrevFrameViewInfo.TemporalAAHistory.ReferenceBufferSize = OutputExtents;

			}
			Outputs.NewHistory = new FFSR2TemporalUpscalerHistory(FSR2State, const_cast<FFSR2TemporalUpscaler*>(this));

			//-----------------------------------------------------------------------------------------------------------------------------------------
			// Invalidate FSR2 Contexts
			//   If a context already exists but it is not valid for the current frame's features, clean it up in preparation for creating a new one.
			//-----------------------------------------------------------------------------------------------------------------------------------------
			if (HasValidContext)
			{
				FfxFsr2ContextDescription const& CurrentParams = FSR2State->Params;

				// Display size must match for splitscreen to work.
				if ((CurrentParams.maxRenderSize.width < Params.maxRenderSize.width) || (CurrentParams.maxRenderSize.height < Params.maxRenderSize.height) || (CurrentParams.displaySize.width != Params.displaySize.width) || (CurrentParams.displaySize.height != Params.displaySize.height) || (Params.flags != CurrentParams.flags) || (Params.device != CurrentParams.device))
				{
					ffxFsr2ContextDestroy(&FSR2State->Fsr2);
					HasValidContext = false;
					bHistoryValid = false;
				}
			}

			//------------------------------------------------------
			// Create FSR2 Contexts
			//   If no valid context currently exists, create one.
			//------------------------------------------------------
			if (!HasValidContext)
			{
				switch (Api)
				{
#if FSR2_ENABLE_DX12
					case EFSR2TemporalUpscalerAPI::D3D12:
					{
						ffxFsr2GetInterfaceDX12(&Params.callbacks, reinterpret_cast<ID3D12Device*>(Params.device), FSR2State->ScratchBuffer, FSR2State->ScratchSize);
						break;
					}
#endif
#if FSR2_ENABLE_VK
					case EFSR2TemporalUpscalerAPI::Vulkan:
					{
						VkPhysicalDevice VulkanDevice = ((IVulkanDynamicRHI*)GDynamicRHI)->RHIGetVkPhysicalDevice();
						ffxFsr2UEGetInterfaceVK(&Params.callbacks, FSR2State->ScratchBuffer, FSR2State->ScratchSize, VulkanDevice);
						break;
					}
#endif
					default:
					{
						ffxFsr2GetInterfaceUE(&Params.callbacks, this, FSR2State->ScratchBuffer, FSR2State->ScratchSize);
						break;
					}
				}
				FfxErrorCode ErrorCode = ffxFsr2ContextCreate(&FSR2State->Fsr2, &Params);
				check(ErrorCode == FFX_OK);
				if (ErrorCode == FFX_OK)
				{
					FMemory::Memcpy(FSR2State->Params, Params);
				}
			}
		}

		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
		// Organize Inputs (Part 1)
		//   Some inputs FSR2 requires are available now, but will no longer be directly available once we get inside the RenderGraph.  Go ahead and collect the ones we can.
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
		FfxFsr2DispatchDescription* Fsr2DispatchParamsPtr = new FfxFsr2DispatchDescription;
		FfxFsr2DispatchDescription& Fsr2DispatchParams = *Fsr2DispatchParamsPtr;
		FMemory::Memzero(Fsr2DispatchParams);
		{
			// Whether to abandon the history in the state on camera cuts
			Fsr2DispatchParams.reset = !bHistoryValid;

			// CVar parameters:
			Fsr2DispatchParams.enableSharpening = (CVarFSR2Sharpness.GetValueOnRenderThread() != 0.0f);
			Fsr2DispatchParams.sharpness = FMath::Clamp(CVarFSR2Sharpness.GetValueOnRenderThread(), 0.0f, 1.0f);

			// Engine parameters:
			Fsr2DispatchParams.frameTimeDelta = View.Family->Time.GetDeltaWorldTimeSeconds() * 1000.0f;
			Fsr2DispatchParams.jitterOffset.x = View.TemporalJitterPixels.X;
			Fsr2DispatchParams.jitterOffset.y = View.TemporalJitterPixels.Y;
			Fsr2DispatchParams.preExposure = View.PreExposure;

			Fsr2DispatchParams.renderSize.width = InputExtents.X;
			Fsr2DispatchParams.renderSize.height = InputExtents.Y;

			// @todo parameters for motion vectors - these should be a scale but FSR2 seems to treat them as resolution?
			Fsr2DispatchParams.motionVectorScale.x = InputExtents.X;
			Fsr2DispatchParams.motionVectorScale.y = InputExtents.Y;

			// Untested parameters:
			Fsr2DispatchParams.cameraFovAngleVertical = View.ViewMatrices.ComputeHalfFieldOfViewPerAxis().Y * 2.0f;

			// Unused parameters:
			if (bool(ERHIZBuffer::IsInverted))
			{
				Fsr2DispatchParams.cameraNear = FLT_MAX;
				Fsr2DispatchParams.cameraFar = View.ViewMatrices.ComputeNearPlane();
			}
			else
			{
				Fsr2DispatchParams.cameraNear = View.ViewMatrices.ComputeNearPlane();
				Fsr2DispatchParams.cameraFar = FLT_MAX;
			}
		}

		//------------------------------
		// Add FSR2 to the RenderGraph
		//------------------------------
		FFSR2Pass::FParameters* PassParameters = GraphBuilder.AllocParameters<FFSR2Pass::FParameters>();
		PassParameters->ColorTexture = SceneColor;
		PassParameters->DepthTexture = SceneDepth;
		PassParameters->VelocityTexture = MotionVectorTexture;
		if (bValidEyeAdaptation)
		{
			FRDGTextureDesc ExposureDesc = FRDGTextureDesc::Create2D({ 1,1 }, PF_A32B32G32R32F, FClearValueBinding::Black, TexCreate_ShaderResource | TexCreate_UAV);
			FRDGTextureRef ExposureTexture = GraphBuilder.CreateTexture(ExposureDesc, TEXT("FSR2ExposureTexture"));

			FFSR2CopyExposureCS::FParameters* ExposureCopyPassParameters = GraphBuilder.AllocParameters<FFSR2CopyExposureCS::FParameters>();

			ExposureCopyPassParameters->EyeAdaptationBuffer = GraphBuilder.CreateSRV(GetEyeAdaptationBuffer(GraphBuilder, View));
			ExposureCopyPassParameters->ExposureTexture = GraphBuilder.CreateUAV(ExposureTexture);

			TShaderMapRef<FFSR2CopyExposureCS> ComputeShaderFSR(View.ShaderMap);
			FComputeShaderUtils::AddPass(
				GraphBuilder,
				RDG_EVENT_NAME("FidelityFX-FSR2/CopyExposure (CS)"),
				ComputeShaderFSR,
				ExposureCopyPassParameters,
				FComputeShaderUtils::GetGroupCount(FIntVector(1, 1, 1), FIntVector(1, 1, 1))
			);
			PassParameters->ExposureTexture = ExposureTexture;
		}
		PassParameters->ReactiveMaskTexture = ReactiveMaskTexture;
		PassParameters->CompositeMaskTexture = CompositeMaskTexture;
		PassParameters->OutputTexture = OutputTexture;

		auto* ApiAccess = ApiAccessor;
		auto CurrentApi = Api;
		if (CurrentApi == EFSR2TemporalUpscalerAPI::Unreal)
		{
			Fsr2DispatchParams.color = ffxGetResourceFromUEResource(&FSR2State->Params.callbacks, PassParameters->ColorTexture.GetTexture());
			Fsr2DispatchParams.depth = ffxGetResourceFromUEResource(&FSR2State->Params.callbacks, PassParameters->DepthTexture.GetTexture());
			Fsr2DispatchParams.motionVectors = ffxGetResourceFromUEResource(&FSR2State->Params.callbacks, PassParameters->VelocityTexture.GetTexture());
			Fsr2DispatchParams.exposure = ffxGetResourceFromUEResource(&FSR2State->Params.callbacks, PassParameters->ExposureTexture.GetTexture());
			if (PassParameters->ReactiveMaskTexture)
			{
				Fsr2DispatchParams.reactive = ffxGetResourceFromUEResource(&FSR2State->Params.callbacks, PassParameters->ReactiveMaskTexture.GetTexture());
			}
			Fsr2DispatchParams.output = ffxGetResourceFromUEResource(&FSR2State->Params.callbacks, PassParameters->OutputTexture.GetTexture(), FFX_RESOURCE_STATE_UNORDERED_ACCESS);
			Fsr2DispatchParams.commandList = (FfxCommandList)CurrentGraphBuilder;

			ffxFsr2SetFeatureLevel(&FSR2State->Params.callbacks, View.GetFeatureLevel());
			FfxErrorCode Code = ffxFsr2ContextDispatch(&FSR2State->Fsr2, &Fsr2DispatchParams);
			check(Code == FFX_OK);
			delete Fsr2DispatchParamsPtr;
		}
		else
		{
			GraphBuilder.AddPass(RDG_EVENT_NAME("FidelityFX-FSR2"), PassParameters, ERDGPassFlags::Compute | ERDGPassFlags::Raster | ERDGPassFlags::SkipRenderPass, [&View, &PassInputs, CurrentApi, ApiAccess, PassParameters, PrevCustomHistory, Fsr2DispatchParamsPtr, FSR2State](FRHICommandListImmediate& RHICmdList)
				{
					//----------------------------------------------------------
					// Organize Inputs (Part 2)
					//   The remaining inputs FSR2 requires are available now.
					//----------------------------------------------------------
					FfxFsr2DispatchDescription DispatchParams;
					FMemory::Memcpy(DispatchParams, *Fsr2DispatchParamsPtr);
					delete Fsr2DispatchParamsPtr;

					switch (CurrentApi)
					{
#if FSR2_ENABLE_DX12
						case EFSR2TemporalUpscalerAPI::D3D12:
						{
							DispatchParams.color = ffxGetResourceDX12(&FSR2State->Fsr2, (ID3D12Resource*)PassParameters->ColorTexture->GetRHI()->GetNativeResource());
							DispatchParams.depth = ffxGetResourceDX12(&FSR2State->Fsr2, (ID3D12Resource*)PassParameters->DepthTexture->GetRHI()->GetNativeResource());
							DispatchParams.motionVectors = ffxGetResourceDX12(&FSR2State->Fsr2, (ID3D12Resource*)PassParameters->VelocityTexture->GetRHI()->GetNativeResource());
							if (PassParameters->ExposureTexture)
							{
								DispatchParams.exposure = ffxGetResourceDX12(&FSR2State->Fsr2, (ID3D12Resource*)PassParameters->ExposureTexture->GetRHI()->GetNativeResource());
							}
							DispatchParams.output = ffxGetResourceDX12(&FSR2State->Fsr2, (ID3D12Resource*)PassParameters->OutputTexture->GetRHI()->GetNativeResource(), nullptr, FFX_RESOURCE_STATE_UNORDERED_ACCESS);
							if (PassParameters->ReactiveMaskTexture)
							{
								DispatchParams.reactive = ffxGetResourceDX12(&FSR2State->Fsr2, (ID3D12Resource*)PassParameters->ReactiveMaskTexture->GetRHI()->GetNativeResource());
							}
							if (PassParameters->CompositeMaskTexture)
							{
								DispatchParams.transparencyAndComposition = ffxGetResourceDX12(&FSR2State->Fsr2, (ID3D12Resource*)PassParameters->CompositeMaskTexture->GetRHI()->GetNativeResource());
							}
							break;
						}
#endif
#if FSR2_ENABLE_VK
						case EFSR2TemporalUpscalerAPI::Vulkan:
						{
							DispatchParams.color = FSR2GetTextureFromRHIResource(ApiAccess, &FSR2State->Fsr2, PassParameters->ColorTexture->GetRHI());
							DispatchParams.depth = FSR2GetTextureFromRHIResource(ApiAccess, &FSR2State->Fsr2, PassParameters->DepthTexture->GetRHI());
							DispatchParams.motionVectors = FSR2GetTextureFromRHIResource(ApiAccess, &FSR2State->Fsr2, PassParameters->VelocityTexture->GetRHI());
							if (PassParameters->ExposureTexture)
							{
								DispatchParams.exposure = FSR2GetTextureFromRHIResource(ApiAccess, &FSR2State->Fsr2, PassParameters->ExposureTexture->GetRHI());
							}
							DispatchParams.output = FSR2GetTextureFromRHIResource(ApiAccess, &FSR2State->Fsr2, PassParameters->OutputTexture->GetRHI(), FFX_RESOURCE_STATE_UNORDERED_ACCESS);
							if (PassParameters->ReactiveMaskTexture)
							{
								DispatchParams.reactive = FSR2GetTextureFromRHIResource(ApiAccess, &FSR2State->Fsr2, PassParameters->ReactiveMaskTexture->GetRHI());
							}
							if (PassParameters->CompositeMaskTexture)
							{
								DispatchParams.transparencyAndComposition = FSR2GetTextureFromRHIResource(ApiAccess, &FSR2State->Fsr2, PassParameters->CompositeMaskTexture->GetRHI());
							}
							break;
						}
#endif
						default:
						{
							check(false);
							break;
						}
					}

					PassParameters->ColorTexture->MarkResourceAsUsed();
					PassParameters->DepthTexture->MarkResourceAsUsed();
					PassParameters->VelocityTexture->MarkResourceAsUsed();
					if (PassParameters->ExposureTexture)
					{
						PassParameters->ExposureTexture->MarkResourceAsUsed();
					}
					PassParameters->OutputTexture->MarkResourceAsUsed();
					if (PassParameters->ReactiveMaskTexture)
					{
						PassParameters->ReactiveMaskTexture->MarkResourceAsUsed();
					}
					if (PassParameters->CompositeMaskTexture)
					{
						PassParameters->CompositeMaskTexture->MarkResourceAsUsed();
					}

					//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
					// Push barriers
					//   Some resources are in the wrong state for FSR2 to execute.  Transition them.  On some platforms, this may involve a bit of tricking the RHI into doing what we want...
					//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
					if (ApiAccess)
					{
						ApiAccess->ForceUAVTransition(RHICmdList, PassParameters->OutputTexture->GetRHI());
					}

					{
						SCOPED_DRAW_EVENT(RHICmdList, FidelityFXFSR2Dispatch);
						SCOPED_GPU_STAT(RHICmdList, FidelityFXFSR2Dispatch);

						//-------------------------------------------------------------------------------------
						// Dispatch FSR2
						//   Push the FSR2 algorithm directly onto the underlying graphics APIs command list.
						//-------------------------------------------------------------------------------------
						switch (CurrentApi)
						{
#if FSR2_ENABLE_DX12
							case EFSR2TemporalUpscalerAPI::D3D12:
							{
								RHICmdList.EnqueueLambda([FSR2State, DispatchParams, ApiAccess](FRHICommandListImmediate& cmd) mutable
									{
										ID3D12GraphicsCommandList* cmdList = (ID3D12GraphicsCommandList*)ApiAccess->GetNativeCommandBuffer(cmd);
										DispatchParams.commandList = ffxGetCommandListDX12(cmdList);

										FfxErrorCode Code = ffxFsr2ContextDispatch(&FSR2State->Fsr2, &DispatchParams);
										check(Code == FFX_OK);
									});
								break;
							}
#endif
#if FSR2_ENABLE_VK
							case EFSR2TemporalUpscalerAPI::Vulkan:
							{
								RHICmdList.EnqueueLambda([FSR2State, DispatchParams, ApiAccess](FRHICommandListImmediate& cmd) mutable
									{
										VkCommandBuffer cmdList = (VkCommandBuffer)ApiAccess->GetNativeCommandBuffer(cmd);
										DispatchParams.commandList = ffxGetCommandListVK(cmdList);

										FfxErrorCode Code = ffxFsr2ContextDispatch(&FSR2State->Fsr2, &DispatchParams);
										check(Code == FFX_OK);
									});
								break;
							}
#endif
							default:
							{
								check(false);
								break;
							}
						}
					}

					//-----------------------------------------------------------------------------------------------------------------------------------------------
					// Flush instructions to the GPU
					//   The FSR2 Dispatch has tampered with the state of the current command list.  Flush it all the way to the GPU so that Unreal can start anew.
					//-----------------------------------------------------------------------------------------------------------------------------------------------
					RHICmdList.ImmediateFlush(EImmediateFlushType::DispatchToRHIThread);
				});
		}

		//----------------------------------------------------------------------------------------------------------------------------------
		// Update History Data (Part 2)
		//   Extract the output produced by the FSR2 Dispatch into the history reference we prepared to receive that output during Part 1.
		//----------------------------------------------------------------------------------------------------------------------------------
		if (CanWritePrevViewInfo)
		{
			// Copy the new history into the history wrapper
			GraphBuilder.QueueTextureExtraction(OutputTexture, &View.ViewState->PrevFrameViewInfo.TemporalAAHistory.RT[0]);
		}

		DeferredCleanup();

		return Outputs;
	}
}

IFSR2TemporalUpscaler* FFSR2TemporalUpscaler::Fork_GameThread(const class FSceneViewFamily& InViewFamily) const
{
	Initialize();

	static const IFSR2TemporalUpscalingModule& FSR2ModuleInterface = FModuleManager::GetModuleChecked<IFSR2TemporalUpscalingModule>(TEXT("FSR2TemporalUpscaling"));

	return new FFSR2TemporalUpscalerProxy(FSR2ModuleInterface.GetFSR2Upscaler());
}

float FFSR2TemporalUpscaler::GetMinUpsampleResolutionFraction() const
{
	if (IsApiSupported())
	{
		return FSR2_GetScreenResolutionFromScalingMode(LowestResolutionQualityMode);
	}
	else
	{
		return 0;
	}
}

float FFSR2TemporalUpscaler::GetMaxUpsampleResolutionFraction() const
{
	if (IsApiSupported())
	{
		return FSR2_GetScreenResolutionFromScalingMode(HighestResolutionQualityMode);
	}
	else
	{
		return 0;
	}
}

//-------------------------------------------------------------------------------------
// The ScreenSpaceReflections shaders are specialized as to whether they expect to be denoised or not.
// When using the denoising plugin API to capture reflection data it is necessary to swap the shaders so that it appears as it would without denoising.
//-------------------------------------------------------------------------------------
void FFSR2TemporalUpscaler::SetSSRShader(FGlobalShaderMap* GlobalMap)
{
	static const FHashedName SSRSourceFile(TEXT("/Engine/Private/SSRT/SSRTReflections.usf"));
	static const FHashedName SSRPixelShader(TEXT("FScreenSpaceReflectionsPS"));

	static const auto CVarFSR2Enabled = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FidelityFX.FSR2.Enabled"));
	const bool bShouldBeSwapped = (CVarFSR2Enabled && (CVarFSR2Enabled->GetValueOnAnyThread() != 0) && (CVarFSR2UseExperimentalSSRDenoiser.GetValueOnAnyThread() == 0));

	FGlobalShaderMapSection* Section = GlobalMap->FindSection(SSRSourceFile);
	if (Section)
	{
		// accessing SSRShaderMapSwapState is not thread-safe
		check(IsInGameThread());

		FFSR2ShaderMapSwapState& ShaderMapSwapState = SSRShaderMapSwapState.FindOrAdd(GlobalMap, FFSR2ShaderMapSwapState::Default);
		if (ShaderMapSwapState.Content != Section->GetContent())
		{
			ShaderMapSwapState.Content = Section->GetContent();
			ShaderMapSwapState.bSwapped = false;
		}

		if (bShouldBeSwapped != ShaderMapSwapState.bSwapped)
		{	
#if WITH_EDITORONLY_DATA
			const bool WasFrozen = Section->GetFrozenContentSize() > 0u;
			FShaderMapContent* Content = (FShaderMapContent*)Section->GetMutableContent();
#else
			FShaderMapContent* Content = (FShaderMapContent*)Section->GetContent();
#endif

			FFSR2ShaderMapContent* PublicContent = (FFSR2ShaderMapContent*)Content;

			for (uint32 i = 0; i < (uint32)ESSRQuality::MAX; i++)
			{
				FFSR2ScreenSpaceReflectionsPS::FPermutationDomain DefaultPermutationVector;
				DefaultPermutationVector.Set<FSSRQualityDim>((ESSRQuality)i);
				DefaultPermutationVector.Set<FSSROutputForDenoiser>(false);

				FFSR2ScreenSpaceReflectionsPS::FPermutationDomain DenoisePermutationVector;
				DenoisePermutationVector.Set<FSSRQualityDim>((ESSRQuality)i);
				DenoisePermutationVector.Set<FSSROutputForDenoiser>(true);

				// for this very small and simple shader map, index == permutation id
				const uint32 CurrentDefaultIndex = DefaultPermutationVector.ToDimensionValueId(), CurrentDenoiseIndex = DenoisePermutationVector.ToDimensionValueId();
				checkSlow(PublicContent->Shaders[CurrentDefaultIndex].GetChecked() == Content->GetShader(SSRPixelShader, DefaultPermutationVector.ToDimensionValueId())
					   && PublicContent->Shaders[CurrentDenoiseIndex].GetChecked() == Content->GetShader(SSRPixelShader, DenoisePermutationVector.ToDimensionValueId()));
				
				FShader* CurrentDefaultShader = PublicContent->Shaders[CurrentDefaultIndex];
				PublicContent->Shaders[CurrentDefaultIndex] = PublicContent->Shaders[CurrentDenoiseIndex];
				PublicContent->Shaders[CurrentDenoiseIndex] = CurrentDefaultShader;
			}

#if WITH_EDITORONLY_DATA
			// Calling FinalizeContent() is only correct in editor, and if the section was already frozen when we started.
			// if the section wasn't frozen, it hadn't finished loading yet... so how did we get here?
			if (ensure(WasFrozen))
			{
				Section->FinalizeContent();
				ShaderMapSwapState.Content = Section->GetContent();
			}
#endif

			ShaderMapSwapState.bSwapped = bShouldBeSwapped;
		}
	}
}

//-------------------------------------------------------------------------------------
// The FXSystem override lets us copy the scene color after all opaque rendering but before translucency.
// This can be used to compare and pick out translucency data that isn't captured in Separate Translucency.
//-------------------------------------------------------------------------------------
void FFSR2TemporalUpscaler::CopyOpaqueSceneColor(FRHICommandListImmediate& RHICmdList, FRHIUniformBuffer* ViewUniformBuffer, const class FShaderParametersMetadata* SceneTexturesUniformBufferStruct, FRHIUniformBuffer* SceneTexturesUniformBuffer)
{
	static const auto CVarFSR2Enabled = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FidelityFX.FSR2.Enabled"));
	FTextureRHIRef SceneColor;
	if (PreAlpha.Target)
	{
		SceneColor = PreAlpha.Target->GetRHI();
	}
	if (IsApiSupported() && (CVarFSR2Enabled && CVarFSR2Enabled->GetValueOnRenderThread()) && SceneColorPreAlpha.GetReference() && SceneColor.GetReference() && SceneColorPreAlpha->GetFormat() == SceneColor->GetFormat())
	{
		SCOPED_DRAW_EVENTF(RHICmdList, FSR2TemporalUpscaler_CopyOpaqueSceneColor, TEXT("FSR2TemporalUpscaler CopyOpaqueSceneColor"));

		FRHICopyTextureInfo Info;
		Info.Size.X = FMath::Min(SceneColorPreAlpha->GetSizeX(), (uint32)SceneColor->GetSizeXYZ().X);
		Info.Size.Y = FMath::Min(SceneColorPreAlpha->GetSizeY(), (uint32)SceneColor->GetSizeXYZ().Y);
		RHICmdList.CopyTexture(SceneColor, SceneColorPreAlpha, Info);
	}
}

//-------------------------------------------------------------------------------------
// Binds the Lumen reflection data & previous depth buffer so we can reproject last frame's Lumen reflections into the reactive mask.
//-------------------------------------------------------------------------------------
void FFSR2TemporalUpscaler::SetLumenReflections(FSceneView& InView)
{
	if (InView.State)
	{
		FReflectionTemporalState& ReflectionTemporalState = ((FSceneViewState*)InView.State)->Lumen.ReflectionState;
		LumenReflections = ReflectionTemporalState.SpecularIndirectHistoryRT;
	}
}

//-------------------------------------------------------------------------------------
// Capture the post-processing inputs structure so that the separate translucency textures are available to the reactive mask.
//-------------------------------------------------------------------------------------
void FFSR2TemporalUpscaler::SetPostProcessingInputs(FPostProcessingInputs const& NewInputs)
{
	PostInputs = NewInputs;
}

//-------------------------------------------------------------------------------------
// As the upscaler retains some resources during the frame they must be released here to avoid leaking or accessing dangling pointers.
//-------------------------------------------------------------------------------------
void FFSR2TemporalUpscaler::EndOfFrame()
{
	PostInputs.SceneTextures = nullptr;
	ReflectionTexture = nullptr;
	LumenReflections.SafeRelease();
	PreAlpha.Target = nullptr;
	PreAlpha.Resolve = nullptr;
#if WITH_EDITOR
	bEnabledInEditor = true;
#endif
}

//-------------------------------------------------------------------------------------
// In the Editor it is necessary to disable the view extension via the upscaler API so it doesn't cause conflicts.
//-------------------------------------------------------------------------------------
#if WITH_EDITOR
bool FFSR2TemporalUpscaler::IsEnabledInEditor() const
{
	return bEnabledInEditor;
}

void FFSR2TemporalUpscaler::SetEnabledInEditor(bool bEnabled)
{
	bEnabledInEditor = bEnabled;
}
#endif

//-------------------------------------------------------------------------------------
// The interesting function in the denoiser API that lets us capture the reflections texture.
// This has to replicate the behavior of the engine, only we retain a reference to the output texture.
//-------------------------------------------------------------------------------------
IScreenSpaceDenoiser::FReflectionsOutputs FFSR2TemporalUpscaler::DenoiseReflections(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FReflectionsInputs& ReflectionInputs,
	const FReflectionsRayTracingConfig RayTracingConfig) const
{
	IScreenSpaceDenoiser::FReflectionsOutputs Outputs;
	Outputs.Color = ReflectionInputs.Color;
	if (FSR2ShouldRenderRayTracingReflections(View) || CVarFSR2UseExperimentalSSRDenoiser.GetValueOnRenderThread())
	{
		Outputs = WrappedDenoiser->DenoiseReflections(GraphBuilder, View, PreviousViewInfos, SceneTextures, ReflectionInputs, RayTracingConfig);
	}
	else if (IsFSR2SSRTemporalPassRequired(View))
	{
		const bool bComposePlanarReflections = FSR2HasDeferredPlanarReflections(View);

		check(View.ViewState);
		FTAAPassParameters TAASettings(View);
		TAASettings.Pass = ETAAPassConfig::ScreenSpaceReflections;
		TAASettings.SceneDepthTexture = SceneTextures.SceneDepthTexture;
		TAASettings.SceneVelocityTexture = SceneTextures.GBufferVelocityTexture;
		TAASettings.SceneColorInput = ReflectionInputs.Color;
		TAASettings.bOutputRenderTargetable = bComposePlanarReflections;

		FTAAOutputs TAAOutputs = AddTemporalAAPass(
			GraphBuilder,
			View,
			TAASettings,
			View.PrevViewInfo.SSRHistory,
			&View.ViewState->PrevFrameViewInfo.SSRHistory);

		Outputs.Color = TAAOutputs.SceneColor;
	}
	ReflectionTexture = Outputs.Color;
	return Outputs;
}

//-------------------------------------------------------------------------------------
// The remaining denoiser API simply passes through to the wrapped denoiser.
//-------------------------------------------------------------------------------------
IScreenSpaceDenoiser::EShadowRequirements FFSR2TemporalUpscaler::GetShadowRequirements(
	const FViewInfo& View,
	const FLightSceneInfo& LightSceneInfo,
	const FShadowRayTracingConfig& RayTracingConfig) const
{
	return WrappedDenoiser->GetShadowRequirements(View, LightSceneInfo, RayTracingConfig);
}

void FFSR2TemporalUpscaler::DenoiseShadowVisibilityMasks(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const TStaticArray<FShadowVisibilityParameters, IScreenSpaceDenoiser::kMaxBatchSize>& InputParameters,
	const int32 InputParameterCount,
	TStaticArray<FShadowVisibilityOutputs, IScreenSpaceDenoiser::kMaxBatchSize>& Outputs) const
{

	return WrappedDenoiser->DenoiseShadowVisibilityMasks(GraphBuilder, View, PreviousViewInfos, SceneTextures, InputParameters, InputParameterCount, Outputs);
}

IScreenSpaceDenoiser::FPolychromaticPenumbraOutputs FFSR2TemporalUpscaler::DenoisePolychromaticPenumbraHarmonics(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FPolychromaticPenumbraHarmonics& Inputs) const
{
	return WrappedDenoiser->DenoisePolychromaticPenumbraHarmonics(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs);
}

IScreenSpaceDenoiser::FReflectionsOutputs FFSR2TemporalUpscaler::DenoiseWaterReflections(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FReflectionsInputs& ReflectionInputs,
	const FReflectionsRayTracingConfig RayTracingConfig) const
{
	return WrappedDenoiser->DenoiseWaterReflections(GraphBuilder, View, PreviousViewInfos, SceneTextures, ReflectionInputs, RayTracingConfig);
}

IScreenSpaceDenoiser::FAmbientOcclusionOutputs FFSR2TemporalUpscaler::DenoiseAmbientOcclusion(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FAmbientOcclusionInputs& ReflectionInputs,
	const FAmbientOcclusionRayTracingConfig RayTracingConfig) const
{
	return WrappedDenoiser->DenoiseAmbientOcclusion(GraphBuilder, View, PreviousViewInfos, SceneTextures, ReflectionInputs, RayTracingConfig);
}

FSSDSignalTextures FFSR2TemporalUpscaler::DenoiseDiffuseIndirect(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FDiffuseIndirectInputs& Inputs,
	const FAmbientOcclusionRayTracingConfig Config) const
{
	return WrappedDenoiser->DenoiseDiffuseIndirect(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs, Config);
}

IScreenSpaceDenoiser::FDiffuseIndirectOutputs FFSR2TemporalUpscaler::DenoiseSkyLight(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FDiffuseIndirectInputs& Inputs,
	const FAmbientOcclusionRayTracingConfig Config) const
{
	return WrappedDenoiser->DenoiseSkyLight(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs, Config);
}

IScreenSpaceDenoiser::FDiffuseIndirectOutputs FFSR2TemporalUpscaler::DenoiseReflectedSkyLight(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FDiffuseIndirectInputs& Inputs,
	const FAmbientOcclusionRayTracingConfig Config) const
{
	return WrappedDenoiser->DenoiseReflectedSkyLight(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs, Config);
}

FSSDSignalTextures FFSR2TemporalUpscaler::DenoiseDiffuseIndirectHarmonic(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FDiffuseIndirectHarmonic& Inputs,
	const HybridIndirectLighting::FCommonParameters& CommonDiffuseParameters) const
{
	return WrappedDenoiser->DenoiseDiffuseIndirectHarmonic(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs, CommonDiffuseParameters);
}

bool FFSR2TemporalUpscaler::SupportsScreenSpaceDiffuseIndirectDenoiser(EShaderPlatform Platform) const
{
	return WrappedDenoiser->SupportsScreenSpaceDiffuseIndirectDenoiser(Platform);
}

FSSDSignalTextures FFSR2TemporalUpscaler::DenoiseScreenSpaceDiffuseIndirect(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FPreviousViewInfo* PreviousViewInfos,
	const FSceneTextureParameters& SceneTextures,
	const FDiffuseIndirectInputs& Inputs,
	const FAmbientOcclusionRayTracingConfig Config) const
{
	return WrappedDenoiser->DenoiseScreenSpaceDiffuseIndirect(GraphBuilder, View, PreviousViewInfos, SceneTextures, Inputs, Config);
}
