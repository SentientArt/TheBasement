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

#include "NGXVulkanRHI.h"

#include "IVulkanDynamicRHI.h"

#include "nvsdk_ngx_vk.h"
#include "nvsdk_ngx_helpers_vk.h"
#include "nvsdk_ngx_helpers_dlssd_vk.h"

#include "GenericPlatform/GenericPlatformFile.h"

DEFINE_LOG_CATEGORY_STATIC(LogDLSSNGXVulkanRHI, Log, All);

#define LOCTEXT_NAMESPACE "FNGXVulkanRHIModule"

class FVulkanNGXDLSSFeature final : public NGXDLSSFeature
{

public:
	using NGXDLSSFeature::NGXDLSSFeature;

	virtual ~FVulkanNGXDLSSFeature()
	{
		check(!IsRunningRHIInSeparateThread() || IsInRHIThread());
		check(NGXRHI::NGXInitialized());
		NVSDK_NGX_Result ResultReleaseFeature = NVSDK_NGX_VULKAN_ReleaseFeature(Feature);
		checkf(NVSDK_NGX_SUCCEED(ResultReleaseFeature), TEXT("NVSDK_NGX_VULKAN_ReleaseFeature failed! (%u %s), %s"), ResultReleaseFeature, GetNGXResultAsString(ResultReleaseFeature), *Desc.GetDebugDescription());

		if (Parameter != nullptr)
		{
			NVSDK_NGX_Result ResultDestroyParameter = NVSDK_NGX_VULKAN_DestroyParameters(Parameter);
			checkf(NVSDK_NGX_SUCCEED(ResultDestroyParameter), TEXT("NVSDK_NGX_VULKAN_DestroyParameters failed! (%u %s), %s"), ResultDestroyParameter, GetNGXResultAsString(ResultDestroyParameter), *Desc.GetDebugDescription());
		}
	}
};


class FNGXVulkanRHI final : public NGXRHI
{

public:
	FNGXVulkanRHI(const FNGXRHICreateArguments& Arguments);
	virtual void ExecuteDLSS(FRHICommandList& CmdList, const FRHIDLSSArguments& InArguments, FDLSSStateRef InDLSSState) final;
	virtual ~FNGXVulkanRHI();
private:

	IVulkanDynamicRHI* VulkanRHI = nullptr;

	NVSDK_NGX_Result Init_NGX_VK(const FNGXRHICreateArguments& InArguments, const wchar_t* InApplicationDataPath, VkInstance InInstance, VkPhysicalDevice InPD, VkDevice InDevice, const NVSDK_NGX_FeatureCommonInfo* InFeatureInfo);
	static bool IsIncompatibleAPICaptureToolActive();
};


bool FNGXVulkanRHI::IsIncompatibleAPICaptureToolActive()
{
	// TODO figure out whether the renderdoc layer is active, either here or in the PreInit module
	return false;
}

NVSDK_NGX_Result FNGXVulkanRHI::Init_NGX_VK(const FNGXRHICreateArguments& InArguments, const wchar_t* InApplicationDataPath, VkInstance InInstance, VkPhysicalDevice InPD, VkDevice InDevice, const NVSDK_NGX_FeatureCommonInfo* InFeatureInfo)
{
	NVSDK_NGX_Result Result = NVSDK_NGX_Result_Fail;
	int32 APIVersion = NVSDK_NGX_VERSION_API_MACRO;
	do 
	{
		if (InArguments.InitializeNGXWithNGXApplicationID())
		{
			Result = NVSDK_NGX_VULKAN_Init(InArguments.NGXAppId, InApplicationDataPath, InInstance, InPD, InDevice, nullptr, nullptr, InFeatureInfo, static_cast<NVSDK_NGX_Version>(APIVersion));
			UE_LOG(LogDLSSNGXVulkanRHI, Log, TEXT("NVSDK_NGX_VULKAN_Init(AppID= %u, APIVersion = 0x%x) -> (%u %s)"), InArguments.NGXAppId, APIVersion, Result, GetNGXResultAsString(Result));
		}
		else
		{
			Result = NVSDK_NGX_VULKAN_Init_with_ProjectID(TCHAR_TO_UTF8(*InArguments.UnrealProjectID), NVSDK_NGX_ENGINE_TYPE_UNREAL, TCHAR_TO_UTF8(*InArguments.UnrealEngineVersion), InApplicationDataPath, InInstance, InPD, InDevice, nullptr, nullptr, InFeatureInfo, static_cast<NVSDK_NGX_Version>(APIVersion));
			UE_LOG(LogDLSSNGXVulkanRHI, Log, TEXT("NVSDK_NGX_VULKAN_Init(ProjectID = %s, EngineVersion=%s, APIVersion = 0x%x) -> (%u %s)"), *InArguments.UnrealProjectID, *InArguments.UnrealEngineVersion, APIVersion, Result, GetNGXResultAsString(Result));
		}

		if (NVSDK_NGX_FAILED(Result) && IsSafeToShutdownNGX())
		{
			NVSDK_NGX_VULKAN_Shutdown1(InDevice);
		}

		--APIVersion;
	} while (NVSDK_NGX_FAILED(Result) && APIVersion >= NVSDK_NGX_VERSION_API_MACRO_BASE_LINE);

	if (NVSDK_NGX_SUCCEED(Result) && (APIVersion + 1 < NVSDK_NGX_VERSION_API_MACRO_WITH_LOGGING))
	{
		UE_LOG(LogDLSSNGXVulkanRHI, Log, TEXT("Warning: NVSDK_NGX_VULKAN_Init succeeded, but the driver installed on this system is too old the support the NGX logging API. The console variables r.NGX.LogLevel and r.NGX.EnableOtherLoggingSinks will have no effect and NGX logs will only show up in their own log files, and not in UE's log files."));
	}

	return Result;
}


FNGXVulkanRHI::FNGXVulkanRHI(const FNGXRHICreateArguments& Arguments)
	: NGXRHI(Arguments)
	, VulkanRHI(CastDynamicRHI<IVulkanDynamicRHI>(Arguments.DynamicRHI))
{
	check(VulkanRHI);

	const FString NGXLogDir = GetNGXLogDirectory();
	IPlatformFile::GetPlatformPhysical().CreateDirectoryTree(*NGXLogDir);

	bIsIncompatibleAPICaptureToolActive = IsIncompatibleAPICaptureToolActive();

	VkInstance VulkanInstance = VulkanRHI->RHIGetVkInstance();
	VkPhysicalDevice VulkanPhysicalDevice = VulkanRHI->RHIGetVkPhysicalDevice();
	VkDevice VulkanLogicalDevice = VulkanRHI->RHIGetVkDevice();

	NVSDK_NGX_Result ResultInit = Init_NGX_VK(Arguments, *NGXLogDir, VulkanInstance, VulkanPhysicalDevice, VulkanLogicalDevice, CommonFeatureInfo());
	UE_LOG(LogDLSSNGXVulkanRHI, Log, TEXT("NVSDK_NGX_VULKAN_Init (Log %s) -> (%u %s)"), *NGXLogDir, ResultInit, GetNGXResultAsString(ResultInit));

	// store for the higher level code interpret
	NGXQueryFeature.NGXInitResult = ResultInit;

	if (NVSDK_NGX_Result_FAIL_OutOfDate == ResultInit)
	{
		NGXQueryFeature.NGXDriverRequirements.DriverUpdateRequired = true;
	}
	else if (NVSDK_NGX_SUCCEED(ResultInit))
	{
		bNGXInitialized = true;

		NVSDK_NGX_Result ResultGetParameters = NVSDK_NGX_VULKAN_GetCapabilityParameters(&NGXQueryFeature.CapabilityParameters);
		UE_LOG(LogDLSSNGXVulkanRHI, Log, TEXT("NVSDK_NGX_VULKAN_GetCapabilityParameters -> (%u %s)"), ResultGetParameters, GetNGXResultAsString(ResultGetParameters));

		if (NVSDK_NGX_Result_FAIL_OutOfDate == ResultGetParameters)
		{
			NGXQueryFeature.NGXDriverRequirements.DriverUpdateRequired = true;
		}

		if (NVSDK_NGX_SUCCEED(ResultGetParameters))
		{
			NGXQueryFeature.QueryDLSSSupport();
		}
	}
}

FNGXVulkanRHI::~FNGXVulkanRHI()
{
	UE_LOG(LogDLSSNGXVulkanRHI, Log, TEXT("%s Enter"), ANSI_TO_TCHAR(__FUNCTION__));
	if (bNGXInitialized)
	{
		// Destroy the parameters and features before we call NVSDK_NGX_VULKAN_Shutdown1
		ReleaseAllocatedFeatures();

		NVSDK_NGX_Result Result;
		if (NGXQueryFeature.CapabilityParameters != nullptr)
		{
			Result = NVSDK_NGX_VULKAN_DestroyParameters(NGXQueryFeature.CapabilityParameters);
			UE_LOG(LogDLSSNGXVulkanRHI, Log, TEXT("NVSDK_NGX_VULKAN_DestroyParameters -> (%u %s)"), Result, GetNGXResultAsString(Result));
		}
		if (IsSafeToShutdownNGX())
		{
			VkDevice VulkanLogicalDevice = VulkanRHI->RHIGetVkDevice();
			Result = NVSDK_NGX_VULKAN_Shutdown1(VulkanLogicalDevice);
			UE_LOG(LogDLSSNGXVulkanRHI, Log, TEXT("NVSDK_NGX_VULKAN_Shutdown1 -> (%u %s)"), Result, GetNGXResultAsString(Result));
		}
		bNGXInitialized = false;
	}
	UE_LOG(LogDLSSNGXVulkanRHI, Log, TEXT("%s Leave"), ANSI_TO_TCHAR(__FUNCTION__));
}

void FNGXVulkanRHI::ExecuteDLSS(FRHICommandList& CmdList, const FRHIDLSSArguments& InArguments, FDLSSStateRef InDLSSState)
{
	check(!IsRunningRHIInSeparateThread() || IsInRHIThread());
	check(IsDLSSAvailable());
	if (!IsDLSSAvailable()) return;

	InArguments.Validate();

	VkCommandBuffer VulkanCommandBuffer = VulkanRHI->RHIGetActiveVkCommandBuffer();
	
	if (InDLSSState->RequiresFeatureRecreation(InArguments))
	{
		check(!InDLSSState->DLSSFeature || InDLSSState->HasValidFeature());
		InDLSSState->DLSSFeature = nullptr;
	}

	if (InArguments.bReset)
	{
		check(!InDLSSState->DLSSFeature);
		InDLSSState->DLSSFeature = FindFreeFeature(InArguments);
	}

	if (!InDLSSState->DLSSFeature)
	{
		VkDevice VulkanLogicalDevice = VulkanRHI->RHIGetVkDevice();
		NVSDK_NGX_Parameter* NewNGXParameterHandle = nullptr;
		NVSDK_NGX_Result Result = NVSDK_NGX_VULKAN_AllocateParameters(&NewNGXParameterHandle);
		checkf(NVSDK_NGX_SUCCEED(Result), TEXT("NVSDK_NGX_VULKAN_AllocateParameters failed! (%u %s)"), Result, GetNGXResultAsString(Result));
		
		ApplyCommonNGXParameterSettings(NewNGXParameterHandle, InArguments);

		if (InArguments.DenoiserMode == ENGXDLSSDenoiserMode::DLSSRR)
		{
			// DLSS-SR feature creation
			NVSDK_NGX_DLSSD_Create_Params DlssRRCreateParams = InArguments.GetNGXDLSSRRCreateParams();
			NVSDK_NGX_Handle* NewNGXFeatureHandle = nullptr;

			const uint32 CreationNodeMask = 1 << InArguments.GPUNode;
			const uint32 VisibilityNodeMask = InArguments.GPUVisibility;

			NVSDK_NGX_Result ResultCreate = NGX_VULKAN_CREATE_DLSSD_EXT1(
				VulkanLogicalDevice,
				VulkanCommandBuffer,
				CreationNodeMask,
				VisibilityNodeMask,
				&NewNGXFeatureHandle,
				NewNGXParameterHandle,
				&DlssRRCreateParams);

			if (NVSDK_NGX_SUCCEED(ResultCreate))
			{
				InDLSSState->DLSSFeature = MakeShared<FVulkanNGXDLSSFeature>(NewNGXFeatureHandle, NewNGXParameterHandle, InArguments.GetFeatureDesc(), FrameCounter);
				InDLSSState->DLSSFeature->bHasDLSSRR = true;
			}
			else
			{
				UE_LOG(LogDLSSNGXVulkanRHI, Error,
					TEXT("NGX_VULKAN_CREATE_DLSSD_EXT1 failed, falling back to DLSS-SR! (CreationNodeMask=0x%x VisibilityNodeMask=0x%x) (%u %s), %s"),
					CreationNodeMask,
					VisibilityNodeMask,
					ResultCreate,
					GetNGXResultAsString(ResultCreate),
					*InArguments.GetFeatureDesc().GetDebugDescription());
				InDLSSState->DLSSFeature.Reset();
			}
		}
		if (!InDLSSState->DLSSFeature.IsValid())
		{
			// DLSS-SR feature creation
			NVSDK_NGX_DLSS_Create_Params DlssCreateParams = InArguments.GetNGXDLSSCreateParams();
			NVSDK_NGX_Handle* NewNGXFeatureHandle = nullptr;

			const uint32 CreationNodeMask = 1 << InArguments.GPUNode;
			const uint32 VisibilityNodeMask = InArguments.GPUVisibility;

			NVSDK_NGX_Result ResultCreate = NGX_VULKAN_CREATE_DLSS_EXT(
				VulkanCommandBuffer,
				CreationNodeMask,
				VisibilityNodeMask,
				&NewNGXFeatureHandle,
				NewNGXParameterHandle,
				&DlssCreateParams);

			checkf(NVSDK_NGX_SUCCEED(ResultCreate), TEXT("NGX_VULKAN_CREATE_DLSS failed! (CreationNodeMask=0x%x VisibilityNodeMask=0x%x) (%u %s), %s"), CreationNodeMask, VisibilityNodeMask, ResultCreate, GetNGXResultAsString(ResultCreate), *InArguments.GetFeatureDesc().GetDebugDescription());
			InDLSSState->DLSSFeature = MakeShared<FVulkanNGXDLSSFeature>(NewNGXFeatureHandle, NewNGXParameterHandle, InArguments.GetFeatureDesc(), FrameCounter);
		}

		RegisterFeature(InDLSSState->DLSSFeature);
	}

	check(InDLSSState->HasValidFeature());

	// execute
	auto NGXVulkanResourceFromRHITexture = [VulkanRHI=VulkanRHI](FRHITexture* InRHITexture)
	{
		check(InRHITexture);
		if (FRHITextureReference* TexRef = InRHITexture->GetTextureReference())
		{
			InRHITexture = TexRef->GetReferencedTexture();
			check(InRHITexture);
		}

		const FVulkanRHIImageViewInfo ImageViewInfo = VulkanRHI->RHIGetImageViewInfo(InRHITexture);

		NVSDK_NGX_Resource_VK NGXTexture;
		FMemory::Memzero(NGXTexture);

		NGXTexture.Type = NVSDK_NGX_RESOURCE_VK_TYPE_VK_IMAGEVIEW;

		// Check for VK_IMAGE_USAGE_STORAGE_BIT. Those are not directly stored but FVulkanSurface::GenerateImageCreateInfo sets the VK flag based on those UEFlags
		NGXTexture.ReadWrite = EnumHasAnyFlags(ImageViewInfo.UEFlags, TexCreate_Presentable | TexCreate_UAV);

		NGXTexture.Resource.ImageViewInfo.ImageView = ImageViewInfo.ImageView;
		NGXTexture.Resource.ImageViewInfo.Image = ImageViewInfo.Image;
		NGXTexture.Resource.ImageViewInfo.Format = ImageViewInfo.Format;

		NGXTexture.Resource.ImageViewInfo.Width = ImageViewInfo.Width;
		NGXTexture.Resource.ImageViewInfo.Height = ImageViewInfo.Height;
		check(ImageViewInfo.Depth == 1);

		NGXTexture.Resource.ImageViewInfo.SubresourceRange = ImageViewInfo.SubresourceRange;

		// DLSS_TODO Figure out where to get those from if the textures are arrayed or mipped.
		check(NGXTexture.Resource.ImageViewInfo.SubresourceRange.layerCount == 1);
		check(NGXTexture.Resource.ImageViewInfo.SubresourceRange.levelCount == 1);

		return NGXTexture;
	};

	if (InDLSSState->DLSSFeature->bHasDLSSRR)
	{
		NVSDK_NGX_VK_DLSSD_Eval_Params DlssRREvalParams;
		FMemory::Memzero(DlssRREvalParams);
		NVSDK_NGX_Resource_VK InOutput = NGXVulkanResourceFromRHITexture(InArguments.OutputColor);
		DlssRREvalParams.pInOutput = &InOutput;
		check(InArguments.OutputColor->GetFlags() & (TexCreate_UAV | TexCreate_Presentable));
		check(InOutput.ReadWrite == true);
		DlssRREvalParams.InOutputSubrectBase.X = InArguments.DestRect.Min.X;
		DlssRREvalParams.InOutputSubrectBase.Y = InArguments.DestRect.Min.Y;

		DlssRREvalParams.InRenderSubrectDimensions.Width = InArguments.SrcRect.Width();
		DlssRREvalParams.InRenderSubrectDimensions.Height = InArguments.SrcRect.Height();

		NVSDK_NGX_Resource_VK InColor = NGXVulkanResourceFromRHITexture(InArguments.InputColor);
		DlssRREvalParams.pInColor = &InColor;
		DlssRREvalParams.InColorSubrectBase.X = InArguments.SrcRect.Min.X;
		DlssRREvalParams.InColorSubrectBase.Y = InArguments.SrcRect.Min.Y;

		NVSDK_NGX_Resource_VK InDepth = NGXVulkanResourceFromRHITexture(InArguments.InputDepth);
		DlssRREvalParams.pInDepth = &InDepth;
		DlssRREvalParams.InDepthSubrectBase.X = InArguments.SrcRect.Min.X;
		DlssRREvalParams.InDepthSubrectBase.Y = InArguments.SrcRect.Min.Y;

		NVSDK_NGX_Resource_VK InMotionVectors = NGXVulkanResourceFromRHITexture(InArguments.InputMotionVectors);
		DlssRREvalParams.pInMotionVectors = &InMotionVectors;
		// The VelocityCombine pass puts the motion vectors into the top left corner
		DlssRREvalParams.InMVSubrectBase.X = 0;
		DlssRREvalParams.InMVSubrectBase.Y = 0;

		NVSDK_NGX_Resource_VK InExposureTexture = NGXVulkanResourceFromRHITexture(InArguments.InputExposure);
		DlssRREvalParams.pInExposureTexture = InArguments.bUseAutoExposure ? nullptr : &InExposureTexture;
		DlssRREvalParams.InPreExposure = InArguments.PreExposure;

		DlssRREvalParams.InJitterOffsetX = InArguments.JitterOffset.X;
		DlssRREvalParams.InJitterOffsetY = InArguments.JitterOffset.Y;

		DlssRREvalParams.InMVScaleX = InArguments.MotionVectorScale.X;
		DlssRREvalParams.InMVScaleY = InArguments.MotionVectorScale.Y;
		DlssRREvalParams.InReset = InArguments.bReset;

		DlssRREvalParams.InFrameTimeDeltaInMsec = InArguments.DeltaTimeMS;

		// The GBufferResolve pass puts the albedos into the top left corner
		NVSDK_NGX_Resource_VK InDiffuseAlbedoTexture = NGXVulkanResourceFromRHITexture(InArguments.InputDiffuseAlbedo);
		DlssRREvalParams.pInDiffuseAlbedo = &InDiffuseAlbedoTexture;
		DlssRREvalParams.InDiffuseAlbedoSubrectBase.X = 0;
		DlssRREvalParams.InDiffuseAlbedoSubrectBase.Y = 0;

		NVSDK_NGX_Resource_VK InSpecularAlbedoTexture = NGXVulkanResourceFromRHITexture(InArguments.InputSpecularAlbedo);
		DlssRREvalParams.pInSpecularAlbedo = &InSpecularAlbedoTexture;
		DlssRREvalParams.InSpecularAlbedoSubrectBase.X = 0;
		DlssRREvalParams.InSpecularAlbedoSubrectBase.Y = 0;

		NVSDK_NGX_Resource_VK InNormalTexture = NGXVulkanResourceFromRHITexture(InArguments.InputNormals);
		DlssRREvalParams.pInNormals = &InNormalTexture;
		NVSDK_NGX_Resource_VK InRoughnessTexture = NGXVulkanResourceFromRHITexture(InArguments.InputRoughness);
		DlssRREvalParams.pInRoughness = &InRoughnessTexture;

		NVSDK_NGX_Result ResultEvaluate = NGX_VULKAN_EVALUATE_DLSSD_EXT(
			VulkanCommandBuffer,
			InDLSSState->DLSSFeature->Feature,
			InDLSSState->DLSSFeature->Parameter,
			&DlssRREvalParams
		);

		checkf(NVSDK_NGX_SUCCEED(ResultEvaluate), TEXT("NGX_Vulkan_EVALUATE_DLSSD_EXT failed! (%u %s), %s"), ResultEvaluate, GetNGXResultAsString(ResultEvaluate), *InDLSSState->DLSSFeature->Desc.GetDebugDescription());
	}
	else
	{
		NVSDK_NGX_VK_DLSS_Eval_Params DlssEvalParams;
		FMemory::Memzero(DlssEvalParams);
		NVSDK_NGX_Resource_VK InOutput = NGXVulkanResourceFromRHITexture(InArguments.OutputColor);
		DlssEvalParams.Feature.pInOutput = &InOutput;
		check(InArguments.OutputColor->GetFlags() & (TexCreate_UAV | TexCreate_Presentable));
		check(InOutput.ReadWrite == true);
		DlssEvalParams.InOutputSubrectBase.X = InArguments.DestRect.Min.X;
		DlssEvalParams.InOutputSubrectBase.Y = InArguments.DestRect.Min.Y;

		DlssEvalParams.InRenderSubrectDimensions.Width = InArguments.SrcRect.Width();
		DlssEvalParams.InRenderSubrectDimensions.Height = InArguments.SrcRect.Height();

		NVSDK_NGX_Resource_VK InColor = NGXVulkanResourceFromRHITexture(InArguments.InputColor);
		DlssEvalParams.Feature.pInColor = &InColor;
		DlssEvalParams.InColorSubrectBase.X = InArguments.SrcRect.Min.X;
		DlssEvalParams.InColorSubrectBase.Y = InArguments.SrcRect.Min.Y;

		NVSDK_NGX_Resource_VK InDepth = NGXVulkanResourceFromRHITexture(InArguments.InputDepth);
		DlssEvalParams.pInDepth = &InDepth;
		DlssEvalParams.InDepthSubrectBase.X = InArguments.SrcRect.Min.X;
		DlssEvalParams.InDepthSubrectBase.Y = InArguments.SrcRect.Min.Y;

		NVSDK_NGX_Resource_VK InMotionVectors = NGXVulkanResourceFromRHITexture(InArguments.InputMotionVectors);
		DlssEvalParams.pInMotionVectors = &InMotionVectors;
		// The VelocityCombine pass puts the motion vectors into the top left corner
		DlssEvalParams.InMVSubrectBase.X = 0;
		DlssEvalParams.InMVSubrectBase.Y = 0;

		NVSDK_NGX_Resource_VK InExposureTexture = NGXVulkanResourceFromRHITexture(InArguments.InputExposure);
		DlssEvalParams.pInExposureTexture = InArguments.bUseAutoExposure ? nullptr : &InExposureTexture;
		DlssEvalParams.InPreExposure = InArguments.PreExposure;

		DlssEvalParams.Feature.InSharpness = InArguments.Sharpness;
		DlssEvalParams.InJitterOffsetX = InArguments.JitterOffset.X;
		DlssEvalParams.InJitterOffsetY = InArguments.JitterOffset.Y;

		DlssEvalParams.InMVScaleX = InArguments.MotionVectorScale.X;
		DlssEvalParams.InMVScaleY = InArguments.MotionVectorScale.Y;
		DlssEvalParams.InReset = InArguments.bReset;

		DlssEvalParams.InFrameTimeDeltaInMsec = InArguments.DeltaTimeMS;

		NVSDK_NGX_Result ResultEvaluate = NGX_VULKAN_EVALUATE_DLSS_EXT(
			VulkanCommandBuffer,
			InDLSSState->DLSSFeature->Feature,
			InDLSSState->DLSSFeature->Parameter,
			&DlssEvalParams
		);

		checkf(NVSDK_NGX_SUCCEED(ResultEvaluate), TEXT("NGX_Vulkan_EVALUATE_DLSS_EXT failed! (%u %s), %s"), ResultEvaluate, GetNGXResultAsString(ResultEvaluate), *InDLSSState->DLSSFeature->Desc.GetDebugDescription());
	}
	InDLSSState->DLSSFeature->Tick(FrameCounter);

	VulkanRHI->RHIRegisterWork(1);
	VulkanRHI->RHIFinishExternalComputeWork(VulkanCommandBuffer);
}


/** IModuleInterface implementation */

void FNGXVulkanRHIModule::StartupModule()
{
	// NGXRHI module should be loaded to ensure logging state is initialized
	FModuleManager::LoadModuleChecked<INGXRHIModule>(TEXT("NGXRHI"));
}

void FNGXVulkanRHIModule::ShutdownModule()
{
}

TUniquePtr<NGXRHI> FNGXVulkanRHIModule::CreateNGXRHI(const FNGXRHICreateArguments& Arguments)
{
	TUniquePtr<NGXRHI> Result(new FNGXVulkanRHI(Arguments));
	return Result;
}

IMPLEMENT_MODULE(FNGXVulkanRHIModule, NGXVulkanRHI)

#undef LOCTEXT_NAMESPACE


