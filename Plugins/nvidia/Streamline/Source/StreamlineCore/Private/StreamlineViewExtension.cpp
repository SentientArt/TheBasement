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

#include "StreamlineViewExtension.h"

#include "StreamlineCorePrivate.h"
#include "StreamlineDLSSG.h"
#include "StreamlineRHI.h"

#include "ClearQuad.h"
#include "Runtime/Launch/Resources/Version.h"
#include "SceneRendering.h"
#include "SceneView.h"
#include "SceneTextureParameters.h"
#include "VelocityCombinePass.h"


#define LOCTEXT_NAMESPACE "FStreamlineViewExtension"

#if defined (ENGINE_STREAMLINE_VERSION) && (ENGINE_STREAMLINE_VERSION >= 1)
#define ENGINE_SUPPORTS_CLEARQUADALPHA 1
#else
#define ENGINE_SUPPORTS_CLEARQUADALPHA ((ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 2))
#endif


static TAutoConsoleVariable<bool> CVarStreamlineTagSceneColorWithoutHUD(
	TEXT("r.Streamline.TagSceneColorWithoutHUD"),
	true,
	TEXT("Pass scene color without HUD into DLSS Frame Generation (default = true)\n"),
	ECVF_RenderThreadSafe);

static TAutoConsoleVariable<bool> CVarStreamlineTagEditorSceneColorWithoutHUD(
	TEXT("r.Streamline.Editor.TagSceneColorWithoutHUD"),
	false,
	TEXT("Pass scene color without HUD into DLSS Frame Generation in the editor (default = false)\n"),
	ECVF_RenderThreadSafe);

static TAutoConsoleVariable<int32> CVarStreamlineDilateMotionVectors(
	TEXT("r.Streamline.DilateMotionVectors"),
	0,
	TEXT(" 0: pass low resolution motion vectors into DLSS Frame Generation (default)\n")
	TEXT(" 1: pass dilated high resolution motion vectors into DLSS Frame Generation. This can help with improving image quality of thin details."),
	ECVF_RenderThreadSafe);

static TAutoConsoleVariable<float> CVarStreamlineMotionVectorScale(
	TEXT("r.Streamline.MotionVectorScale"),
	1.0f,
	TEXT("Scale DLSS Frame Generation motion vectors by this constant, in addition to the scale by 1/ the view rect size. (default = 1)\n"),
	ECVF_RenderThreadSafe);

static TAutoConsoleVariable<float> CVarStreamlineCustomCameraNearPlane(
	TEXT("r.Streamline.CustomCameraNearPlane"),
	0.01f,
	TEXT("Custom distance to camera near plane. Used for internal DLSS Frame Generation purposes, does not need to match corresponding value used by engine. (default = 0.01f)\n"),
	ECVF_RenderThreadSafe);

static TAutoConsoleVariable<float> CVarStreamlineCustomCameraFarPlane(
	TEXT("r.Streamline.CustomCameraFarPlane"),
	75000.0f,
	TEXT("Custom distance to camera far plane. Used for internal DLSS Frame Generation purposes, does not need to match corresponding value used by engine. (default = 75000.0f)\n"),
	ECVF_RenderThreadSafe);

static TAutoConsoleVariable<int32> CVarStreamlineViewIdOverride(
	TEXT("r.Streamline.ViewIdOverride"), 1,
	TEXT("Replace the correct ViewID with 0 for bringup\n")
	TEXT("0: use ViewState.UniqueID \n")
	TEXT("1: on set view ID to 0 (default)\n"),
	ECVF_Default);

static TAutoConsoleVariable<bool> CVarStreamlineClearColorAlpha(
	TEXT("r.Streamline.ClearSceneColorAlpha"),
	true,
	TEXT("Clear alpha of scenecolor at the end of the Streamline view extension to allow subsequent UI drawcalls be represented correctly in the alpha channel (default = true)\n"),
	ECVF_RenderThreadSafe);


DECLARE_GPU_STAT(Streamline)

FStreamlineViewExtension::FStreamlineViewExtension(const FAutoRegister& AutoRegister, FStreamlineRHI* InStreamlineRHIExtensions)
	: FSceneViewExtensionBase(AutoRegister)
	, StreamlineRHIExtensions(InStreamlineRHIExtensions)
{
	check(StreamlineRHIExtensions);
	FSceneViewExtensionIsActiveFunctor IsActiveFunctor;
	IsActiveFunctor.IsActiveFunction = [this](const ISceneViewExtension* SceneViewExtension, const FSceneViewExtensionContext& Context)
	{
		return StreamlineRHIExtensions->IsStreamlineAvailable();
	};

	IsActiveThisFrameFunctions.Add(IsActiveFunctor);

}

void FStreamlineViewExtension::SetupViewFamily(FSceneViewFamily& InViewFamily)
{
}

void FStreamlineViewExtension::SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView)
{
}

void FStreamlineViewExtension::SetupViewPoint(APlayerController* Player, FMinimalViewInfo& InViewInfo)
{
}


void FStreamlineViewExtension::BeginRenderViewFamily(FSceneViewFamily& InViewFamily)
{
	BeginRenderViewFamilyDLSSG(InViewFamily);
}

void FStreamlineViewExtension::SubscribeToPostProcessingPass(EPostProcessingPass Pass, FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled)
{
	if (Pass == EPostProcessingPass::VisualizeDepthOfField)
	{
		check(StreamlineRHIExtensions);
		check(StreamlineRHIExtensions->IsStreamlineAvailable());
		InOutPassCallbacks.Add(FAfterPassCallbackDelegate::CreateRaw(this, &FStreamlineViewExtension::PostProcessPassAtEnd_RenderThread));
	}
}


BEGIN_SHADER_PARAMETER_STRUCT(FSLShaderParameters, )
SHADER_PARAMETER_RDG_TEXTURE(Texture2D, Depth)
SHADER_PARAMETER_RDG_TEXTURE(Texture2D, Velocity)
SHADER_PARAMETER_RDG_TEXTURE(Texture2D, SceneColorWithoutHUD)

// Fake output to trigger pass execution
#if (ENGINE_MAJOR_VERSION == 4) && (ENGINE_MINOR_VERSION == 25) 
SHADER_PARAMETER_RDG_TEXTURE(Texture2D, SceneColorAfterTonemap)
#else
RDG_TEXTURE_ACCESS(SceneColorAfterTonemap, ERHIAccess::UAVCompute)
#endif
END_SHADER_PARAMETER_STRUCT()

FScreenPassTexture FStreamlineViewExtension::PostProcessPassAtEnd_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& InOutInputs)
{
	check(IsInRenderingThread());
	check(View.bIsViewInfo);

	if (View.bIsSceneCapture)
	{
		// no point in running DLSS-FG for scene captures if the engine can't use the extra frames anyway. Just pass through the appropriate texture
		if (InOutInputs.OverrideOutput.IsValid())
		{
			return InOutInputs.OverrideOutput;
		}
		else
		{
			return InOutInputs.Textures[(uint32)EPostProcessMaterialInput::SceneColor];
		}
	}

	const FViewInfo& ViewInfo = static_cast<const FViewInfo&>(View);
	const FScreenPassTexture& SceneColor = InOutInputs.Textures[(uint32)EPostProcessMaterialInput::SceneColor];
	const uint32 ViewID = CVarStreamlineViewIdOverride.GetValueOnAnyThread() ? 0 : ViewInfo.GetViewKey();
	const uint64 FrameID = GFrameCounterRenderThread;
	const FIntRect ViewRect = ViewInfo.ViewRect;
	const FIntRect SecondaryViewRect = FIntRect(FIntPoint::ZeroValue, ViewInfo.GetSecondaryViewRectSize());

	RDG_GPU_STAT_SCOPE(GraphBuilder, Streamline);

	RDG_EVENT_SCOPE(GraphBuilder, "Streamline ViewID=%u %dx%d [%d,%d -> %d,%d]",
		// TODO STREAMLINE register the StreamLineRHI work with FGPUProfiler so the streamline tag call shows up with profilegpu
		ViewID, ViewRect.Width(), ViewRect.Height(),
		ViewRect.Min.X, ViewRect.Min.Y,
		ViewRect.Max.X, ViewRect.Max.Y
	);
	if (ShouldTagStreamlineBuffers())
	{
		const uint64 FrameNumber = GFrameNumberRenderThread;

#if ENGINE_MAJOR_VERSION == 4
		FSceneRenderTargets& SceneTextures = FSceneRenderTargets::Get(GraphBuilder.RHICmdList);
#elif ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION < 1
		FSceneTextures SceneTextures = FSceneTextures::Get(GraphBuilder);
#else
		FSceneTextures SceneTextures = ViewInfo.GetSceneTextures();
#endif

		// input color
		FRDGTextureRef SceneColorAfterTonemap = SceneColor.Texture;
		check(SceneColorAfterTonemap);

			// input motion vectors
		FRDGTextureRef SceneVelocity = InOutInputs.Textures[(uint32)EPostProcessMaterialInput::Velocity].Texture;
		if (!SceneVelocity)
		{
#if ENGINE_MAJOR_VERSION == 4
			SceneVelocity = GraphBuilder.RegisterExternalTexture(SceneTextures.SceneVelocity);
#else
			SceneVelocity = SceneTextures.Velocity;
#endif
		}

		//input depth
#if ENGINE_MAJOR_VERSION == 4
		FRDGTextureRef SceneDepth = GraphBuilder.RegisterExternalTexture(SceneTextures.SceneDepthZ);
#else
		FRDGTextureRef SceneDepth = SceneTextures.Depth.Resolve;
#endif
		check(SceneDepth);


		FStreamlineRHI* LocalStreamlineRHIExtensions = this->StreamlineRHIExtensions;

		FSLShaderParameters* PassParameters = GraphBuilder.AllocParameters<FSLShaderParameters>();

	
		FRDGTextureRef SLDepth = SceneDepth;
		FRDGTextureRef SLVelocity = SceneVelocity;
		FRDGTextureRef SLSceneColorWithoutHUD = SceneColor.Texture;

		const bool bTagSceneColorWithoutHUD = GIsEditor ? CVarStreamlineTagEditorSceneColorWithoutHUD.GetValueOnRenderThread() : CVarStreamlineTagSceneColorWithoutHUD.GetValueOnRenderThread();
		if(bTagSceneColorWithoutHUD)
		{
			FRDGTextureDesc Desc = SceneColor.Texture->Desc;
#if (ENGINE_MAJOR_VERSION == 4) && (ENGINE_MINOR_VERSION == 25) 
			Desc.TargetableFlags |= (TexCreate_ShaderResource | TexCreate_UAV);
			Desc.TargetableFlags &= ~TexCreate_Presentable;
#else
			EnumAddFlags(Desc.Flags, TexCreate_ShaderResource | TexCreate_UAV);
			EnumRemoveFlags(Desc.Flags, TexCreate_Presentable);
			EnumRemoveFlags(Desc.Flags, TexCreate_ResolveTargetable);
		
#endif
			SLSceneColorWithoutHUD = GraphBuilder.CreateTexture(Desc, TEXT("Streamline.SceneColorAfterTonemap"));
			AddDrawTexturePass(GraphBuilder, ViewInfo, SceneColor.Texture, SLSceneColorWithoutHUD, FIntPoint::ZeroValue, FIntPoint::ZeroValue, FIntPoint::ZeroValue);
		}

		const bool bDilateMotionVectors = CVarStreamlineDilateMotionVectors.GetValueOnRenderThread() != 0;
		FRDGTextureRef CombinedVelocityTexture = AddStreamlineVelocityCombinePass(GraphBuilder, ViewInfo, SLDepth, SLVelocity, bDilateMotionVectors);

		PassParameters->Depth = SLDepth;
		PassParameters->Velocity = CombinedVelocityTexture;

		if (bTagSceneColorWithoutHUD)
		{
			PassParameters->SceneColorWithoutHUD = SLSceneColorWithoutHUD;
		}

		// That's the dummy parameter to get RDG to execut the pass
		PassParameters->SceneColorAfterTonemap = SceneColor.Texture;

		FRHIStreamlineArguments StreamlineArguments = {};
		FMemory::Memzero(&StreamlineArguments, sizeof(StreamlineArguments));

		StreamlineArguments.FrameId = FrameID;
		StreamlineArguments.ViewId = ViewID;

		// TODO STREAMLINE check for other conditions, similar to DLSS
		StreamlineArguments.bReset = View.bCameraCut;
		StreamlineArguments.bIsNotRenderingGameFrames = false;
	
		StreamlineArguments.bIsDepthInverted = true;

		StreamlineArguments.JitterOffset = { float(ViewInfo.TemporalJitterPixels.X), float(ViewInfo.TemporalJitterPixels.Y) }; // LWC_TODO: Precision loss

		StreamlineArguments.CameraNear = CVarStreamlineCustomCameraNearPlane.GetValueOnRenderThread();
		StreamlineArguments.CameraFar = CVarStreamlineCustomCameraFarPlane.GetValueOnRenderThread();
		StreamlineArguments.CameraFOV = ViewInfo.FOV;
		StreamlineArguments.CameraAspectRatio = float(ViewInfo.ViewRect.Width()) / float(ViewInfo.ViewRect.Height());
		const float MotionVectorScale = CVarStreamlineMotionVectorScale.GetValueOnRenderThread();
		if (bDilateMotionVectors)
		{
			StreamlineArguments.MotionVectorScale = { MotionVectorScale / ViewInfo.GetSecondaryViewRectSize().X, MotionVectorScale / ViewInfo.GetSecondaryViewRectSize().Y };
		}
		else
		{
			StreamlineArguments.MotionVectorScale = { MotionVectorScale / ViewInfo.ViewRect.Width() , MotionVectorScale / ViewInfo.ViewRect.Height() };
		}
		StreamlineArguments.bAreMotionVectorsDilated = bDilateMotionVectors;

		FViewUniformShaderParameters ViewUniformShaderParameters = *ViewInfo.CachedViewUniformShaderParameters;

		StreamlineArguments.bIsOrthographicProjection = !View.IsPerspectiveProjection();
		StreamlineArguments.ClipToCameraView = ViewUniformShaderParameters.ClipToView;
		StreamlineArguments.ClipToLenseClip = FRHIStreamlineArguments::FMatrix44f::Identity;
		StreamlineArguments.ClipToPrevClip = ViewUniformShaderParameters.ClipToPrevClip;
		StreamlineArguments.PrevClipToClip = ViewUniformShaderParameters.ClipToPrevClip.Inverse();

#if ENGINE_MAJOR_VERSION == 4
		StreamlineArguments.CameraOrigin = ViewUniformShaderParameters.WorldCameraOrigin;
#else
		// TODO STREAMLINE : LWC_TODO verify that this works correctly with large world coordinates
		StreamlineArguments.CameraOrigin = ViewUniformShaderParameters.RelativeWorldCameraOrigin;
#endif
		StreamlineArguments.CameraUp = ViewUniformShaderParameters.ViewUp;
		StreamlineArguments.CameraRight = ViewUniformShaderParameters.ViewRight;
		StreamlineArguments.CameraForward = ViewUniformShaderParameters.ViewForward;
		StreamlineArguments.CameraViewToClip = ViewUniformShaderParameters.ViewToClip;

		StreamlineArguments.CameraPinholeOffset = FRHIStreamlineArguments::FVector2f::ZeroVector;

		GraphBuilder.AddPass(
		RDG_EVENT_NAME("Streamline Common %dx%d FrameId=%u ViewID=%u", ViewRect.Width(), ViewRect.Height(), StreamlineArguments.FrameId, StreamlineArguments.ViewId),
			PassParameters,
#if (ENGINE_MAJOR_VERSION == 4) && (ENGINE_MINOR_VERSION == 25) 
			ERDGPassFlags::Compute,
#else
			ERDGPassFlags::Compute | ERDGPassFlags::Raster | ERDGPassFlags::SkipRenderPass | ERDGPassFlags::NeverCull,
#endif
			[LocalStreamlineRHIExtensions, PassParameters, StreamlineArguments, ViewRect, SecondaryViewRect, bTagSceneColorWithoutHUD](FRHICommandListImmediate& RHICmdList) mutable
		{
			check(PassParameters->Depth);
			PassParameters->Depth->MarkResourceAsUsed();
			StreamlineArguments.InputDepth = { PassParameters->Depth->GetRHI(), ViewRect };

			// motion vectors are in the top left corner after the Velocity Combine pass
			check(PassParameters->Velocity)
			PassParameters->Velocity->MarkResourceAsUsed();
			StreamlineArguments.InputMotionVectors = { PassParameters->Velocity->GetRHI(), FIntRect(FIntPoint::ZeroValue, PassParameters->Velocity->Desc.Extent) };

			if (bTagSceneColorWithoutHUD)
			{
				check(PassParameters->SceneColorWithoutHUD);
				PassParameters->SceneColorWithoutHUD->MarkResourceAsUsed();
				StreamlineArguments.InputHUDLessColor = { PassParameters->SceneColorWithoutHUD->GetRHI(), SecondaryViewRect };
			}

			// output dummy to trigger RDG pass exeution
			check(PassParameters->SceneColorAfterTonemap);
			PassParameters->SceneColorAfterTonemap->MarkResourceAsUsed();

			
			RHICmdList.EnqueueLambda(
				[LocalStreamlineRHIExtensions, StreamlineArguments](FRHICommandListImmediate& Cmd) mutable
			{
				LocalStreamlineRHIExtensions->SetStreamlineData(Cmd, StreamlineArguments);
			});
		});
	}

	// this is always executed so we can turn DLSS-G off at the SL side (after we skipped the work above)
	AddStreamlineDLSSGStateRenderPass(GraphBuilder, ViewID, SecondaryViewRect);

#if ENGINE_SUPPORTS_CLEARQUADALPHA
	if (ShouldTagStreamlineBuffers() &&  CVarStreamlineClearColorAlpha.GetValueOnRenderThread())
	{
		auto* PassParameters = GraphBuilder.AllocParameters<FRenderTargetParameters>();
		PassParameters->RenderTargets[0] = FRenderTargetBinding(SceneColor.Texture, ERenderTargetLoadAction::ENoAction);
		
		GraphBuilder.AddPass(
			RDG_EVENT_NAME("ClearSceneColorAlpha"),
			PassParameters,
			ERDGPassFlags::Raster,
			[SecondaryViewRect](FRHICommandList& RHICmdList)
			{
				RHICmdList.SetViewport(SecondaryViewRect.Min.X, SecondaryViewRect.Min.Y, 0.0f, SecondaryViewRect.Max.X, SecondaryViewRect.Max.Y, 1.0f);
				DrawClearQuadAlpha(RHICmdList, 0.0f);
			});
	}
#else
#error "Engine missing DrawClearQuadAlpha support. Apply latest custom engine patch using instructions from DLSS-FG plugin quick start guide or README.md"
#endif

	if (InOutInputs.OverrideOutput.IsValid())
	{
		AddDrawTexturePass(GraphBuilder, ViewInfo, SceneColor, InOutInputs.OverrideOutput);
		return InOutInputs.OverrideOutput;
	}
	else
	{

		return InOutInputs.Textures[(uint32)EPostProcessMaterialInput::SceneColor];
	}
}
#undef LOCTEXT_NAMESPACE
 