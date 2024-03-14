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

#include "NGXD3D11RHI.h"

#include "nvsdk_ngx.h"
#include "nvsdk_ngx_helpers.h"
#include "nvsdk_ngx_helpers_dlssd.h"

#include "ID3D11DynamicRHI.h"
#include "GenericPlatform/GenericPlatformFile.h"

DEFINE_LOG_CATEGORY_STATIC(LogDLSSNGXD3D11RHI, Log, All);

#define LOCTEXT_NAMESPACE "FNGXD3D11RHIModule"

class FD3D11NGXFeatureHandle final : public NGXDLSSFeature
{
	
public:

	using NGXDLSSFeature::NGXDLSSFeature;

	virtual ~FD3D11NGXFeatureHandle()
	{
		check(!IsRunningRHIInSeparateThread() || IsInRHIThread());
		check(NGXRHI::NGXInitialized());
		NVSDK_NGX_Result ResultReleaseFeature = NVSDK_NGX_D3D11_ReleaseFeature(Feature);
		checkf(NVSDK_NGX_SUCCEED(ResultReleaseFeature), TEXT("NVSDK_NGX_D3D11_ReleaseFeature failed! (%u %s), %s"), ResultReleaseFeature, GetNGXResultAsString(ResultReleaseFeature), *Desc.GetDebugDescription());

		if (Parameter != nullptr)
		{
			NVSDK_NGX_Result ResultDestroyParameter = NVSDK_NGX_D3D11_DestroyParameters(Parameter);
			checkf(NVSDK_NGX_SUCCEED(ResultDestroyParameter), TEXT("NVSDK_NGX_D3D11_DestroyParameters failed! (%u %s), %s"), ResultDestroyParameter, GetNGXResultAsString(ResultDestroyParameter), *Desc.GetDebugDescription());
		}
	}
};

class FNGXD3D11RHI final : public NGXRHI
{
public:
	FNGXD3D11RHI(const FNGXRHICreateArguments& Arguments);
	
	virtual void ExecuteDLSS(FRHICommandList& CmdList, const FRHIDLSSArguments& InArguments, FDLSSStateRef InDLSSState) final;
	virtual ~FNGXD3D11RHI();
private:

	ID3D11DynamicRHI* D3D11RHI = nullptr;
	ID3D11Device* Direct3DDevice = nullptr;
	ID3D11DeviceContext* Direct3DDeviceIMContext = nullptr;

	NVSDK_NGX_Result Init_NGX_D3D11(const FNGXRHICreateArguments& InArguments, const wchar_t* InApplicationDataPath, ID3D11Device* InHandle, const NVSDK_NGX_FeatureCommonInfo* InFeatureInfo);
	static bool IsIncompatibleAPICaptureToolActive(ID3D11Device* InDirect3DDevice);

};

bool FNGXD3D11RHI::IsIncompatibleAPICaptureToolActive(ID3D11Device* InDirect3DDevice)
{
	// RenderDoc
	{
		IID RenderDocID;
		if (SUCCEEDED(IIDFromString(L"{A7AA6116-9C8D-4BBA-9083-B4D816B71B78}", &RenderDocID)))
		{
			TRefCountPtr<IUnknown> RenderDoc;
			if (SUCCEEDED(InDirect3DDevice->QueryInterface(RenderDocID, (void**)RenderDoc.GetInitReference())))
			{
				return true;
			}
		}
	}
	return false;
}

NVSDK_NGX_Result FNGXD3D11RHI::Init_NGX_D3D11(const FNGXRHICreateArguments& InArguments, const wchar_t* InApplicationDataPath, ID3D11Device* InHandle, const NVSDK_NGX_FeatureCommonInfo* InFeatureInfo)
{
	NVSDK_NGX_Result Result = NVSDK_NGX_Result_Fail;
	int32 APIVersion = NVSDK_NGX_VERSION_API_MACRO;
	do 
	{
		if (InArguments.InitializeNGXWithNGXApplicationID())
		{
			Result = NVSDK_NGX_D3D11_Init(InArguments.NGXAppId, InApplicationDataPath, InHandle, InFeatureInfo, static_cast<NVSDK_NGX_Version>(APIVersion));
			UE_LOG(LogDLSSNGXD3D11RHI, Log, TEXT("NVSDK_NGX_D3D11_Init(AppID= %u, APIVersion = 0x%x) -> (%u %s)"), InArguments.NGXAppId, APIVersion, Result, GetNGXResultAsString(Result));
		}
		else
		{
			Result = NVSDK_NGX_D3D11_Init_with_ProjectID(TCHAR_TO_UTF8(*InArguments.UnrealProjectID), NVSDK_NGX_ENGINE_TYPE_UNREAL, TCHAR_TO_UTF8(*InArguments.UnrealEngineVersion), InApplicationDataPath, InHandle, InFeatureInfo, static_cast<NVSDK_NGX_Version>(APIVersion));
			UE_LOG(LogDLSSNGXD3D11RHI, Log, TEXT("NVSDK_NGX_D3D11_Init_with_ProjectID(ProjectID = %s, EngineVersion=%s, APIVersion = 0x%x) -> (%u %s)"), *InArguments.UnrealProjectID, *InArguments.UnrealEngineVersion, APIVersion, Result, GetNGXResultAsString(Result));
		}

		if (NVSDK_NGX_FAILED(Result) && IsSafeToShutdownNGX())
		{
			NVSDK_NGX_D3D11_Shutdown1(InHandle);
		}
		
		--APIVersion;
	} while (NVSDK_NGX_FAILED(Result) && APIVersion >= NVSDK_NGX_VERSION_API_MACRO_BASE_LINE);

	if (!NVSDK_NGX_FAILED(Result) && (APIVersion + 1 < NVSDK_NGX_VERSION_API_MACRO_WITH_LOGGING))
	{
		UE_LOG(LogDLSSNGXD3D11RHI, Log, TEXT("Warning: NVSDK_NGX_D3D11_Init succeeded, but the driver installed on this system is too old the support the NGX logging API. The console variables r.NGX.LogLevel and r.NGX.EnableOtherLoggingSinks will have no effect and NGX logs will only show up in their own log files, and not in UE's log files."));
	}

	return Result;
}

FNGXD3D11RHI::FNGXD3D11RHI(const FNGXRHICreateArguments& Arguments)
	: NGXRHI(Arguments)
	, D3D11RHI(CastDynamicRHI<ID3D11DynamicRHI>(Arguments.DynamicRHI))
	, Direct3DDevice(D3D11RHI->RHIGetDevice())
	, Direct3DDeviceIMContext(D3D11RHI->RHIGetDeviceContext())
{
	ensure(D3D11RHI);
	ensure(Direct3DDevice);

	bIsIncompatibleAPICaptureToolActive = IsIncompatibleAPICaptureToolActive(Direct3DDevice);
	
	const FString NGXLogDir = GetNGXLogDirectory();
	IPlatformFile::GetPlatformPhysical().CreateDirectoryTree(*NGXLogDir);

	NVSDK_NGX_Result ResultInit = Init_NGX_D3D11(Arguments, *NGXLogDir, Direct3DDevice, CommonFeatureInfo());
	UE_LOG(LogDLSSNGXD3D11RHI, Log, TEXT("NVSDK_NGX_D3D11_Init (Log %s) -> (%u %s)"), *NGXLogDir, ResultInit, GetNGXResultAsString(ResultInit));
	
	// store for the higher level code interpret
	NGXQueryFeature.NGXInitResult = ResultInit;

	if (NVSDK_NGX_Result_FAIL_OutOfDate == ResultInit)
	{
		NGXQueryFeature.NGXDriverRequirements.DriverUpdateRequired = true;
	}
	else if (NVSDK_NGX_SUCCEED(ResultInit))
	{
		bNGXInitialized = true;

		NVSDK_NGX_Result ResultGetParameters = NVSDK_NGX_D3D11_GetCapabilityParameters(&NGXQueryFeature.CapabilityParameters);

		UE_LOG(LogDLSSNGXD3D11RHI, Log, TEXT("NVSDK_NGX_D3D11_GetCapabilityParameters -> (%u %s)"), ResultGetParameters, GetNGXResultAsString(ResultGetParameters));

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

FNGXD3D11RHI::~FNGXD3D11RHI()
{
	UE_LOG(LogDLSSNGXD3D11RHI, Log, TEXT("%s Enter"), ANSI_TO_TCHAR(__FUNCTION__));
	if(bNGXInitialized)
	{
		// Destroy the parameters and features before we call NVSDK_NGX_D3D11_Shutdown1
		ReleaseAllocatedFeatures();

		NVSDK_NGX_Result Result;
		if (NGXQueryFeature.CapabilityParameters != nullptr)
		{
			Result = NVSDK_NGX_D3D11_DestroyParameters(NGXQueryFeature.CapabilityParameters);
			UE_LOG(LogDLSSNGXD3D11RHI, Log, TEXT("NVSDK_NGX_D3D11_DestroyParameters -> (%u %s)"), Result, GetNGXResultAsString(Result));
		}
		if (IsSafeToShutdownNGX())
		{
			Result = NVSDK_NGX_D3D11_Shutdown1(Direct3DDevice);
			UE_LOG(LogDLSSNGXD3D11RHI, Log, TEXT("NVSDK_NGX_D3D11_Shutdown1 -> (%u %s)"), Result, GetNGXResultAsString(Result));
		}
		bNGXInitialized = false;
	}
	UE_LOG(LogDLSSNGXD3D11RHI, Log, TEXT("%s Leave"), ANSI_TO_TCHAR(__FUNCTION__));
}

template <typename T>
static T GetCommonEvalParams(ID3D11DynamicRHI* D3D11RHI, const FRHIDLSSArguments& InArguments)
{
	T EvalParams;
	FMemory::Memzero(EvalParams);

	EvalParams.InOutputSubrectBase.X = InArguments.DestRect.Min.X;
	EvalParams.InOutputSubrectBase.Y = InArguments.DestRect.Min.Y;

	EvalParams.InRenderSubrectDimensions.Width = InArguments.SrcRect.Width();
	EvalParams.InRenderSubrectDimensions.Height = InArguments.SrcRect.Height();

	EvalParams.InColorSubrectBase.X = InArguments.SrcRect.Min.X;
	EvalParams.InColorSubrectBase.Y = InArguments.SrcRect.Min.Y;

	EvalParams.pInDepth = D3D11RHI->RHIGetResource(InArguments.InputDepth);
	EvalParams.InDepthSubrectBase.X = InArguments.SrcRect.Min.X;
	EvalParams.InDepthSubrectBase.Y = InArguments.SrcRect.Min.Y;

	EvalParams.pInMotionVectors = D3D11RHI->RHIGetResource(InArguments.InputMotionVectors);
	// The VelocityCombine pass puts the motion vectors into the top left corner
	EvalParams.InMVSubrectBase.X = 0;
	EvalParams.InMVSubrectBase.Y = 0;

	EvalParams.pInExposureTexture = InArguments.bUseAutoExposure ? nullptr : D3D11RHI->RHIGetResource(InArguments.InputExposure);
	EvalParams.InPreExposure = InArguments.PreExposure;

	EvalParams.InJitterOffsetX = InArguments.JitterOffset.X;
	EvalParams.InJitterOffsetY = InArguments.JitterOffset.Y;

	EvalParams.InMVScaleX = InArguments.MotionVectorScale.X;
	EvalParams.InMVScaleY = InArguments.MotionVectorScale.Y;
	EvalParams.InReset = InArguments.bReset;

	EvalParams.InFrameTimeDeltaInMsec = InArguments.DeltaTimeMS;

	return EvalParams;
}

void FNGXD3D11RHI::ExecuteDLSS(FRHICommandList& CmdList, const FRHIDLSSArguments& InArguments, FDLSSStateRef InDLSSState)
{
	check(!IsRunningRHIInSeparateThread() || IsInRHIThread());
	check(IsDLSSAvailable());
	if (!IsDLSSAvailable()) 
		return;
	InArguments.Validate();

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
		NVSDK_NGX_Parameter* NewNGXParameterHandle = nullptr;

		NVSDK_NGX_Result Result = NVSDK_NGX_D3D11_AllocateParameters(&NewNGXParameterHandle);
		checkf(NVSDK_NGX_SUCCEED(Result), TEXT("NVSDK_NGX_D3D11_AllocateParameters failed! (%u %s)"), Result, GetNGXResultAsString(Result));

		ApplyCommonNGXParameterSettings(NewNGXParameterHandle, InArguments);

		if (InArguments.DenoiserMode == ENGXDLSSDenoiserMode::DLSSRR)
		{
			// DLSS-RR feature creation
			NVSDK_NGX_DLSSD_Create_Params DlssRRCreateParams = InArguments.GetNGXDLSSRRCreateParams();
			NVSDK_NGX_Handle* NewNGXFeatureHandle = nullptr;
			NVSDK_NGX_Result ResultCreate = NGX_D3D11_CREATE_DLSSD_EXT(
				Direct3DDeviceIMContext,
				&NewNGXFeatureHandle,
				NewNGXParameterHandle,
				&DlssRRCreateParams);
			if (NVSDK_NGX_SUCCEED(ResultCreate))
			{
				InDLSSState->DLSSFeature = MakeShared<FD3D11NGXFeatureHandle>(NewNGXFeatureHandle, NewNGXParameterHandle, InArguments.GetFeatureDesc(), FrameCounter);
				InDLSSState->DLSSFeature->bHasDLSSRR = true;
			}
			else
			{
				UE_LOG(LogDLSSNGXD3D11RHI, Error,
					TEXT("NGX_D3D11_CREATE_DLSSD_EXT failed, falling back to DLSS-SR! (%u %s), %s"),
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
			NVSDK_NGX_Result ResultCreate = NGX_D3D11_CREATE_DLSS_EXT(
				Direct3DDeviceIMContext,
				&NewNGXFeatureHandle,
				NewNGXParameterHandle,
				&DlssCreateParams);
			checkf(NVSDK_NGX_SUCCEED(ResultCreate), TEXT("NGX_D3D11_CREATE_DLSS_EXT failed! (%u %s), %s"),
				ResultCreate,
				GetNGXResultAsString(ResultCreate),
				*InArguments.GetFeatureDesc().GetDebugDescription());
			InDLSSState->DLSSFeature = MakeShared<FD3D11NGXFeatureHandle>(NewNGXFeatureHandle, NewNGXParameterHandle, InArguments.GetFeatureDesc(), FrameCounter);
		}

		RegisterFeature(InDLSSState->DLSSFeature);
	}

	check(InDLSSState->HasValidFeature());

	// execute

	D3D11RHI->RHIRegisterWork(1);

	if (InDLSSState->DLSSFeature->bHasDLSSRR)
	{
		NVSDK_NGX_D3D11_DLSSD_Eval_Params DlssRREvalParams = GetCommonEvalParams<NVSDK_NGX_D3D11_DLSSD_Eval_Params>(D3D11RHI, InArguments);

		DlssRREvalParams.pInOutput = D3D11RHI->RHIGetResource(InArguments.OutputColor);
		DlssRREvalParams.pInColor = D3D11RHI->RHIGetResource(InArguments.InputColor);

		// The GBufferResolve pass puts the albedos into the top left corner
		DlssRREvalParams.pInDiffuseAlbedo = D3D11RHI->RHIGetResource(InArguments.InputDiffuseAlbedo);
		DlssRREvalParams.InDiffuseAlbedoSubrectBase.X = 0;
		DlssRREvalParams.InDiffuseAlbedoSubrectBase.Y = 0;

		DlssRREvalParams.pInSpecularAlbedo = D3D11RHI->RHIGetResource(InArguments.InputSpecularAlbedo);
		DlssRREvalParams.InSpecularAlbedoSubrectBase.X = 0;
		DlssRREvalParams.InSpecularAlbedoSubrectBase.Y = 0;

		DlssRREvalParams.pInNormals = D3D11RHI->RHIGetResource(InArguments.InputNormals);
		DlssRREvalParams.pInRoughness = D3D11RHI->RHIGetResource(InArguments.InputRoughness);

		NVSDK_NGX_Result ResultEvaluate = NGX_D3D11_EVALUATE_DLSSD_EXT(
			Direct3DDeviceIMContext,
			InDLSSState->DLSSFeature->Feature,
			InDLSSState->DLSSFeature->Parameter,
			&DlssRREvalParams
		);
		checkf(NVSDK_NGX_SUCCEED(ResultEvaluate), TEXT("NGX_D3D11_EVALUATE_DLSSD_EXT failed! (%u %s), %s"), ResultEvaluate, GetNGXResultAsString(ResultEvaluate), *InDLSSState->DLSSFeature->Desc.GetDebugDescription());
	}
	else
	{
		NVSDK_NGX_D3D11_DLSS_Eval_Params DlssEvalParams = GetCommonEvalParams<NVSDK_NGX_D3D11_DLSS_Eval_Params>(D3D11RHI, InArguments);

		DlssEvalParams.Feature.pInOutput = D3D11RHI->RHIGetResource(InArguments.OutputColor);
		DlssEvalParams.Feature.pInColor = D3D11RHI->RHIGetResource(InArguments.InputColor);
		DlssEvalParams.Feature.InSharpness = InArguments.Sharpness;

		NVSDK_NGX_Result ResultEvaluate = NGX_D3D11_EVALUATE_DLSS_EXT(
			Direct3DDeviceIMContext,
			InDLSSState->DLSSFeature->Feature,
			InDLSSState->DLSSFeature->Parameter,
			&DlssEvalParams
		);
		checkf(NVSDK_NGX_SUCCEED(ResultEvaluate), TEXT("NGX_D3D11_EVALUATE_DLSS_EXT failed! (%u %s), %s"), ResultEvaluate, GetNGXResultAsString(ResultEvaluate), *InDLSSState->DLSSFeature->Desc.GetDebugDescription());
	}

	InDLSSState->DLSSFeature->Tick(FrameCounter);
}

/** IModuleInterface implementation */

void FNGXD3D11RHIModule::StartupModule()
{
	// NGXRHI module should be loaded to ensure logging state is initialized
	FModuleManager::LoadModuleChecked<INGXRHIModule>(TEXT("NGXRHI"));
}

void FNGXD3D11RHIModule::ShutdownModule()
{
}

TUniquePtr<NGXRHI> FNGXD3D11RHIModule::CreateNGXRHI(const FNGXRHICreateArguments& Arguments)
{
	TUniquePtr<NGXRHI> Result(new FNGXD3D11RHI(Arguments));
	return Result;
}

IMPLEMENT_MODULE(FNGXD3D11RHIModule, NGXD3D11RHI)

#undef LOCTEXT_NAMESPACE


