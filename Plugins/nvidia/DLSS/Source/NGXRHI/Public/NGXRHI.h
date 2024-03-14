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

#pragma once

#include "Modules/ModuleManager.h"

#include "CoreMinimal.h"
#include "RendererInterface.h"

#include "nvsdk_ngx_params.h"
#include "nvsdk_ngx_helpers_dlssd.h"

#define NVSDK_NGX_VERSION_API_MACRO_BASE_LINE (0x0000013)
#define NVSDK_NGX_VERSION_API_MACRO_WITH_LOGGING (0x0000014)

struct FDLSSState;

enum class ENGXDLSSDenoiserMode
{
	Off = NVSDK_NGX_DLSS_Denoise_Mode_Off,
	DLSSRR = NVSDK_NGX_DLSS_Denoise_Mode_DLUnified,
	MaxValue = DLSSRR
};

struct FDLSSFeatureDesc
{
	bool operator != (const FDLSSFeatureDesc& Other) const
	{
		return DestRect.Size() != Other.DestRect.Size()
			|| DLSSPreset != Other.DLSSPreset
			|| PerfQuality != Other.PerfQuality
			|| bHighResolutionMotionVectors != Other.bHighResolutionMotionVectors
			|| bNonZeroSharpness != Other.bNonZeroSharpness
			|| bUseAutoExposure != Other.bUseAutoExposure
			|| bReleaseMemoryOnDelete != Other.bReleaseMemoryOnDelete
			|| GPUNode != Other.GPUNode
			|| GPUVisibility != Other.GPUVisibility
			|| DenoiserMode != Other.DenoiserMode;
	}

	bool operator == (const FDLSSFeatureDesc& Other) const
	{
		return !operator !=(Other);
	}

	FIntRect SrcRect = FIntRect(FIntPoint::NoneValue, FIntPoint::NoneValue);
	FIntRect DestRect = FIntRect(FIntPoint::NoneValue, FIntPoint::NoneValue);
	int32 DLSSPreset = -1;
	int32 PerfQuality = -1;
	bool bHighResolutionMotionVectors = false;
	bool bNonZeroSharpness = false;
	bool bUseAutoExposure = false;
	bool bReleaseMemoryOnDelete = false;
	uint32 GPUNode = 0;
	uint32 GPUVisibility = 0;
	ENGXDLSSDenoiserMode DenoiserMode = ENGXDLSSDenoiserMode::Off;
	FString GetDebugDescription() const
	{
		auto NGXDLSSPresetString = [] (int NGXPerfQuality)
		{
			switch (NGXPerfQuality)
			{
				case NVSDK_NGX_DLSS_Hint_Render_Preset_Default:return TEXT("Default");
				case NVSDK_NGX_DLSS_Hint_Render_Preset_A:return TEXT("Preset A");
				case NVSDK_NGX_DLSS_Hint_Render_Preset_B:return TEXT("Preset B");
				case NVSDK_NGX_DLSS_Hint_Render_Preset_C:return TEXT("Preset C");
				case NVSDK_NGX_DLSS_Hint_Render_Preset_D:return TEXT("Preset D");
				case NVSDK_NGX_DLSS_Hint_Render_Preset_E:return TEXT("Preset E");
				case NVSDK_NGX_DLSS_Hint_Render_Preset_F:return TEXT("Preset F");
				case NVSDK_NGX_DLSS_Hint_Render_Preset_G:return TEXT("Preset G");
				default:return TEXT("Invalid NVSDK_NGX_DLSS_Hint_Render_Preset");
			}
		};
		auto NGXPerfQualityString = [] (int NGXPerfQuality)
		{
			switch (NGXPerfQuality)
			{
				case NVSDK_NGX_PerfQuality_Value_MaxPerf:return TEXT("MaxPerf");
				case NVSDK_NGX_PerfQuality_Value_Balanced:return TEXT("Balanced");
				case NVSDK_NGX_PerfQuality_Value_MaxQuality:return TEXT("MaxQuality");
				case NVSDK_NGX_PerfQuality_Value_UltraPerformance:return TEXT("UltraPerformance");
				case NVSDK_NGX_PerfQuality_Value_UltraQuality:return TEXT("UltraQuality");
				case NVSDK_NGX_PerfQuality_Value_DLAA:return TEXT("DLAA");
				default:return TEXT("Invalid NVSDK_NGX_PerfQuality_Value");
			}
		};
		auto NGXDenoiserModeString = [](ENGXDLSSDenoiserMode NGXDenoiserMode)
		{
			switch (NGXDenoiserMode)
			{
			case ENGXDLSSDenoiserMode::Off: return TEXT("Off");
			case ENGXDLSSDenoiserMode::DLSSRR: return TEXT("DLSSRR");
			default:return TEXT("Invalid ENGXDLSSDenoiserMode");
			}
		};

		return FString::Printf(TEXT("SrcRect=[%dx%d->%dx%d], DestRect=[%dx%d->%dx%d], ScaleX=%f, ScaleY=%f, NGXDLSSPreset=%s(%d), NGXPerfQuality=%s(%d), bHighResolutionMotionVectors=%d, bNonZeroSharpness=%d, bUseAutoExposure=%d, bReleaseMemoryOnDelete=%d, GPUNode=%u, GPUVisibility=0x%x, DenoiseMode=%s"),
			SrcRect.Min.X, SrcRect.Min.Y, SrcRect.Max.X, SrcRect.Max.Y,
			DestRect.Min.X, DestRect.Min.Y, DestRect.Max.X, DestRect.Max.Y,
			float(SrcRect.Width()) / float(DestRect.Width()),
			float(SrcRect.Height()) / float(DestRect.Height()),
			NGXDLSSPresetString(DLSSPreset), DLSSPreset,
			NGXPerfQualityString(PerfQuality), PerfQuality,
			bHighResolutionMotionVectors,
			bNonZeroSharpness,
			bUseAutoExposure,
			bReleaseMemoryOnDelete,
			GPUNode,
			GPUVisibility,
			NGXDenoiserModeString(DenoiserMode));

	}
};

struct NGXRHI_API FRHIDLSSArguments
{
	FRHITexture* InputColor = nullptr;
	FRHITexture* InputDepth = nullptr;
	FRHITexture* InputMotionVectors = nullptr;
	FRHITexture* InputExposure = nullptr;
	
	FRHITexture* InputDiffuseAlbedo = nullptr;
	FRHITexture* InputSpecularAlbedo = nullptr;

	FRHITexture* InputNormals = nullptr;
	FRHITexture* InputRoughness = nullptr;

	FRHITexture* OutputColor = nullptr;

	FIntRect SrcRect = FIntRect(FIntPoint::ZeroValue, FIntPoint::ZeroValue);
	FIntRect DestRect = FIntRect(FIntPoint::ZeroValue, FIntPoint::ZeroValue);
	FVector2f JitterOffset = FVector2f::ZeroVector;
	FVector2f MotionVectorScale = FVector2f::UnitVector;

	FMatrix InvViewProjectionMatrix;
	FMatrix ClipToPrevClipMatrix;
	bool bHighResolutionMotionVectors = false;

	float Sharpness = 0.0f;
	bool bReset = false;

	int32 DLSSPreset = NVSDK_NGX_DLSS_Hint_Render_Preset::NVSDK_NGX_DLSS_Hint_Render_Preset_Default;
	int32 PerfQuality = 0;
	float DeltaTimeMS = 0.0f;

	float PreExposure = 1.0f;
	bool bUseAutoExposure = false;
	
	
	bool bReleaseMemoryOnDelete = false;
	uint32 GPUNode = 0;
	uint32 GPUVisibility = 0;

	ENGXDLSSDenoiserMode DenoiserMode = ENGXDLSSDenoiserMode::Off;

	void Validate() const;
	
	inline FDLSSFeatureDesc GetFeatureDesc() const
	{
		return FDLSSFeatureDesc 
		{ 
			SrcRect, DestRect, DLSSPreset, PerfQuality,
			bHighResolutionMotionVectors, Sharpness != 0.0f, bUseAutoExposure, 
			bReleaseMemoryOnDelete, GPUNode, GPUVisibility, DenoiserMode
		};
	}

	uint32 GetNGXCommonDLSSFeatureFlags() const;
	NVSDK_NGX_DLSS_Create_Params  GetNGXDLSSCreateParams() const;
	NVSDK_NGX_DLSSD_Create_Params GetNGXDLSSRRCreateParams() const;
};

struct FNGXDriverRequirements
{
	bool  DriverUpdateRequired = false;
	int32 MinDriverVersionMajor = 470;
	int32 MinDriverVersionMinor = 0;
};

// the API specic RHI extensions derive from this to handle the lifetime
class NGXRHI_API NGXDLSSFeature
{
public:
	virtual ~NGXDLSSFeature() = 0;

	FDLSSFeatureDesc Desc;
	NVSDK_NGX_Handle* Feature = nullptr;
	NVSDK_NGX_Parameter* Parameter = nullptr;
	uint32 LastUsedFrame = 0;
	bool bHasDLSSRR = false;

	void Tick(uint32 InFrameNumber)
	{
		check(Feature);
		check(Parameter);
		LastUsedFrame = InFrameNumber;
	}

	NGXDLSSFeature(NVSDK_NGX_Handle* InFeature, NVSDK_NGX_Parameter* InParameter, const FDLSSFeatureDesc& InFeatureDesc, uint32 InLastUsedEvaluation)
		: Desc(InFeatureDesc)
		, Feature(InFeature)
		, Parameter(InParameter)
		, LastUsedFrame(InLastUsedEvaluation)
	{
		check(!IsRunningRHIInSeparateThread() || IsInRHIThread());
		check(Feature);
		check(Parameter);
	}
	bool IsValid() const
	{
		return Feature && Parameter;
	}
};

struct NGXRHI_API FDLSSState
{
	// this is used by the RHIs to see whether they need to recreate the NGX feature

	bool RequiresFeatureRecreation(const FRHIDLSSArguments& InArguments);
	
	bool HasValidFeature() const
	{
		return DLSSFeature && DLSSFeature->IsValid();
	}

	// this is stored via pointer to allow the NGXRHIs use the API specific functions to create & release
	TSharedPtr<NGXDLSSFeature> DLSSFeature;
};

using FDLSSStateRef = TSharedPtr<FDLSSState, ESPMode::ThreadSafe>;

enum class ENGXBinariesSearchOrder
{
	MinValue = 0,
	CustomThenGeneric = 0,
	ForceGeneric = 1,
	ForceCustom = 2,
	ForceDevelopmentGeneric = 3,
	MaxValue = ForceDevelopmentGeneric
};

enum class ENGXProjectIdentifier
{
	MinValue = 0,
	NGXApplicationIDThenUEProjectID = 0,
	ForceUEProjectID = 1,
	ForceNGXApplicationID = 2,
	MaxValue = ForceNGXApplicationID
};

struct FNGXRHICreateArguments
{
	FString PluginBaseDir;
	FDynamicRHI* DynamicRHI = nullptr;
	ENGXBinariesSearchOrder NGXBinariesSearchOrder = ENGXBinariesSearchOrder::CustomThenGeneric;

	ENGXProjectIdentifier ProjectIdentifier = ENGXProjectIdentifier::NGXApplicationIDThenUEProjectID;
	uint32 NGXAppId = 0;
	FString UnrealEngineVersion;
	FString UnrealProjectID;
	bool bAllowOTAUpdate = true;

	// centralize that logic here for the derived NGXRHIs
	bool InitializeNGXWithNGXApplicationID() const
	{
		if ((ProjectIdentifier == ENGXProjectIdentifier::NGXApplicationIDThenUEProjectID) && (NGXAppId != 0))
		{
			return true;
		}
		else if (ProjectIdentifier == ENGXProjectIdentifier::ForceNGXApplicationID)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

struct FDLSSOptimalSettings
{
	FIntPoint RenderSize;

	FIntPoint RenderSizeMin;
	FIntPoint RenderSizeMax;
	float Sharpness;

	bool bIsSupported;
	float OptimalResolutionFraction;

	float MinResolutionFraction;
	float MaxResolutionFraction;

	bool  IsFixedResolution() const
	{
		return MinResolutionFraction == MaxResolutionFraction;
	}
};

class NGXRHI_API NGXRHI
{
	struct NGXRHI_API FDLSSQueryFeature
	{
		struct FDLSSResolutionParameters
		{
			uint32 Width = 0;
			uint32 Height = 0;
			NVSDK_NGX_PerfQuality_Value PerfQuality = NVSDK_NGX_PerfQuality_Value_MaxPerf;


			FDLSSResolutionParameters(uint32 InWidth, uint32 InHeight, NVSDK_NGX_PerfQuality_Value InPerfQuality)
				: Width(InWidth)
				, Height(InHeight)
				, PerfQuality(InPerfQuality)
			{}
		};

		void QueryDLSSSupport();
		FDLSSOptimalSettings GetDLSSOptimalSettings(const FDLSSResolutionParameters& InResolution) const;

		// the lifetime of this is managed directly by the encompassing derived RHI
		NVSDK_NGX_Parameter* CapabilityParameters = nullptr;

		bool bIsDlssSRAvailable = false;
		bool bIsDlssRRAvailable = false;
		FNGXDriverRequirements NGXDriverRequirements;
		FNGXDriverRequirements NGXDLSSSRDriverRequirements;
		FNGXDriverRequirements NGXDLSSRRDriverRequirements;

		NVSDK_NGX_Result NGXInitResult = NVSDK_NGX_Result_Fail;
		NVSDK_NGX_Result NGXDLSSSRInitResult = NVSDK_NGX_Result_Fail;
		NVSDK_NGX_Result NGXDLSSRRInitResult = NVSDK_NGX_Result_Fail;
	};

	
public:
	

	virtual ~NGXRHI();

	virtual void ExecuteDLSS(FRHICommandList& CmdList, const FRHIDLSSArguments& InArguments, FDLSSStateRef InDLSSState) = 0;

	bool IsDLSSAvailable() const
	{
		return NGXQueryFeature.bIsDlssSRAvailable;
	}

	bool IsDLSSRRAvailable() const
	{
		return NGXQueryFeature.bIsDlssRRAvailable;
	}

	NVSDK_NGX_Result GetNGXInitResult() const
	{
		return NGXQueryFeature.NGXInitResult;
	}

	const FNGXDriverRequirements& GetNGXDriverRequirements() const
	{
		return NGXQueryFeature.NGXDriverRequirements;
	}

	const FNGXDriverRequirements& GetDLSSSRDriverRequirements() const
	{
		return NGXQueryFeature.NGXDLSSSRDriverRequirements;
	}

	const FNGXDriverRequirements& GetDLSSRRDriverRequirements() const
	{
		return NGXQueryFeature.NGXDLSSRRDriverRequirements;
	}

	FDLSSOptimalSettings GetDLSSOptimalSettings(const FDLSSQueryFeature::FDLSSResolutionParameters& InResolution) const
	{
		return NGXQueryFeature.GetDLSSOptimalSettings(InResolution);
	}

	FDLSSOptimalSettings GetDLSSOptimalSettings(NVSDK_NGX_PerfQuality_Value QualityLevel) const
	{
		return NGXQueryFeature.GetDLSSOptimalSettings(FDLSSQueryFeature::FDLSSResolutionParameters(1000, 1000, QualityLevel));
	}

	float GetDLSSResolutionFraction(NVSDK_NGX_PerfQuality_Value QualityLevel) const
	{ 
		return GetDLSSOptimalSettings(QualityLevel).OptimalResolutionFraction;
	}

	TPair<FString, bool> GetDLSSGenericBinaryInfo() const;
	TPair<FString, bool> GetDLSSCustomBinaryInfo() const;

	void TickPoolElements();

	static bool NGXInitialized()
	{
		return bNGXInitialized;
	}

	static bool IsIncompatibleAPICaptureToolActive()
	{
		return bIsIncompatibleAPICaptureToolActive;
	}


protected:

	NGXRHI(const FNGXRHICreateArguments& Arguments);

	const NVSDK_NGX_FeatureCommonInfo* CommonFeatureInfo() const
	{
		return &FeatureInfo;
	}

	void RegisterFeature(TSharedPtr<NGXDLSSFeature> InFeature);
	TSharedPtr<NGXDLSSFeature> FindFreeFeature(const FRHIDLSSArguments& InArguments);

	void ReleaseAllocatedFeatures();
	void ApplyCommonNGXParameterSettings(NVSDK_NGX_Parameter* Parameter, const FRHIDLSSArguments& InArguments);
	static FString GetNGXLogDirectory();

	bool IsSafeToShutdownNGX() const;

	FDynamicRHI* DynamicRHI = nullptr;
	
	FDLSSQueryFeature NGXQueryFeature;
	
	uint32 FrameCounter = 1;

	static bool bNGXInitialized;
	static bool bIsIncompatibleAPICaptureToolActive;
private:
	TArray< TSharedPtr<NGXDLSSFeature>> AllocatedDLSSFeatures;

	TTuple<FString, bool> DLSSGenericBinaryInfo;
	TTuple<FString, bool> DLSSCustomBinaryInfo;
	
	TArray<FString> NGXDLLSearchPaths;
	TArray<const wchar_t*> NGXDLLSearchPathRawStrings;

	NVSDK_NGX_FeatureCommonInfo FeatureInfo = {{0}};
};


// Implemented by the API specific modules
class INGXRHIModule : public IModuleInterface
{
public:
	virtual TUniquePtr<NGXRHI> CreateNGXRHI(const FNGXRHICreateArguments& Arguments) = 0;
};

class FNGXRHIModule final : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule();
	virtual void ShutdownModule();
};
