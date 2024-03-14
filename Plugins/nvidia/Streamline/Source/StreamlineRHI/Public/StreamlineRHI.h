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
#pragma once

#include "Modules/ModuleManager.h"

#include "CoreMinimal.h"
#include "RendererInterface.h"
#include "Runtime/Launch/Resources/Version.h"

namespace sl
{
	struct AdapterInfo;
	struct FrameToken;
	struct APIError;
	struct FeatureRequirements;
	using Feature = uint32_t;
	enum class FeatureRequirementFlags : uint32_t;
}

class FSLFrameTokenProvider;

enum class EStreamlineSupport : uint8
{
	Supported,
	NotSupported,
	NotSupportedIncompatibleRHI,
	NumValues
};


enum class EStreamlineResource
{
	Depth,
	MotionVectors,
	HUDLessColor,
	UIColorAndAlpha
};

struct FRHITextureWithRect
{
	FRHITexture* Texture = nullptr;
	FIntRect ViewRect = FIntRect(FIntPoint::ZeroValue, FIntPoint::ZeroValue);
};

// TODO STREAMLINE rename variables
struct STREAMLINERHI_API FRHIStreamlineArguments
{

#if ENGINE_MAJOR_VERSION < 5
	// New type names with dims and precision, required in SHADER_PARAMETER declarations and useful everywhere
	using FMatrix44f = FMatrix;
	using FVector2f = FVector2D;
	using FVector3f = FVector;
	using FVector4f = FVector4;
#else
	using FMatrix44f = ::FMatrix44f;
	using FVector3f = ::FVector3f;
	using FVector2f = ::FVector2f;
	using FVector4f = ::FVector4f;
#endif
	


	//! Dept buffer - IMPORTANT - Must be suitable to use with clipToPrevClip transformation (see Constants below)
	FRHITextureWithRect InputDepth;
	//! Object and optional camera motion vectors (see Constants below)
	FRHITextureWithRect InputMotionVectors;
	//! Color buffer with all post-processing effects applied but without any UI/HUD elements
	FRHITextureWithRect InputHUDLessColor;
	
	// View ID across all active views
	uint32 ViewId;

	uint32 FrameId;

	//! Specifies if previous frame has no connection to the current one (motion vectors are invalid)
	bool bReset;
	//! Specifies if application is not currently rendering game frames (paused in menu, playing video cut-scenes)
	bool bIsNotRenderingGameFrames;

	//! Specifies if depth values are inverted (value closer to the camera is higher) or not.
	bool bIsDepthInverted;

	//! Specifies clip space jitter offset
	FVector2f JitterOffset;

	//! Specifies scale factors used to normalize motion vectors (so the values are in [-1,1] range)
	FVector2f MotionVectorScale;
	//! Specifies if motion vectors are already dilated or not.
	bool bAreMotionVectorsDilated;

	//! Specifies if orthographic projection is used or not.
	bool bIsOrthographicProjection;
	//! Specifies matrix transformation from the camera view to the clip space.
	FMatrix44f CameraViewToClip;
	//! Specifies matrix transformation from the clip space to the camera view space.
	FMatrix44f ClipToCameraView;
	//! Specifies matrix transformation describing lens distortion in clip space.
	FMatrix44f ClipToLenseClip;
	//! Specifies matrix transformation from the current clip to the previous clip space.
	FMatrix44f ClipToPrevClip;
	//! Specifies matrix transformation from the previous clip to the current clip space.
	FMatrix44f PrevClipToClip;

	//! Specifies camera position in world space.
	FVector3f CameraOrigin;
	//! Specifies camera up vector in world space.
	FVector3f CameraUp;
	//! Specifies camera right vector in world space.
	FVector3f CameraRight;
	//! Specifies camera forward vector in world space.
	FVector3f CameraForward;

	//! Specifies camera near view plane distance.
	float CameraNear;
	//! Specifies camera far view plane distance.
	float CameraFar;
	//! Specifies camera field of view in radians.
	float CameraFOV;
	//! Specifies camera aspect ratio defined as view space width divided by height.
	float CameraAspectRatio;
	//! Specifies camera pinhole offset.
	FVector2f CameraPinholeOffset;
	
	
};

struct FStreamlineRHICreateArguments
{
	FString PluginBaseDir;
	FDynamicRHI* DynamicRHI = nullptr;
};

class FSLFrameTokenProvider
{
public:
	FSLFrameTokenProvider();

	sl::FrameToken* GetTokenForFrame(uint64 FrameCounter);

private:
	FCriticalSection Section;
	sl::FrameToken* FrameToken;
	uint32_t LastFrameCounter;
};

class STREAMLINERHI_API FStreamlineRHI
{
public:
	virtual ~FStreamlineRHI();

	virtual void SetStreamlineData(FRHICommandList& CmdList, const FRHIStreamlineArguments& InArguments);
	virtual void TagTexture(FRHICommandList& CmdList, const FRHITextureWithRect& InResource, EStreamlineResource InResourceTag, uint32 InViewID) = 0;
	static void TagNullTexture(EStreamlineResource InResourceTag, uint32 InViewID);

	virtual const sl::AdapterInfo* GetAdapterInfo() = 0;

	virtual void APIErrorHandler(const sl::APIError& LastError) = 0;

	virtual bool IsDLSSGSupportedByRHI() const
	{
		return false;
	}

	bool IsStreamlineAvailable() const;

	static bool IsIncompatibleAPICaptureToolActive()
	{
		return bIsIncompatibleAPICaptureToolActive;
	}

	sl::FrameToken* GetFrameToken(uint64 FrameCounter);
	bool IsSwapchainHookingAllowed() const;

protected:
	FStreamlineRHI(const FStreamlineRHICreateArguments& Arguments);

#if WITH_EDITOR

	void OnBeginPIE(const bool bIsSimulating)
	{
		bIsPIEActive = true;
	}
	void OnEndPIE(const bool bIsSimulating)
	{
		bIsPIEActive = false;
	}
	bool bIsPIEActive = false;
	FDelegateHandle BeginPIEHandle;
	FDelegateHandle EndPIEHandle;
#endif

#if PLATFORM_WINDOWS
	// whether an HRESULT is a DXGI_STATUS_*
	bool IsDXGIStatus(const HRESULT HR);
#endif

	FDynamicRHI* DynamicRHI = nullptr;
	TUniquePtr<FSLFrameTokenProvider> FrameTokenProvider = nullptr;

	static bool bIsIncompatibleAPICaptureToolActive;
};

class IStreamlineRHIModule : public IModuleInterface
{
public:

	virtual TUniquePtr<FStreamlineRHI> CreateStreamlineRHI(const FStreamlineRHICreateArguments& Arguments) = 0;
};

class FStreamlineRHIModule final : public IModuleInterface
{
public:
	STREAMLINERHI_API void InitializeStreamline();
	STREAMLINERHI_API void ShutdownStreamline();

	/** IModuleInterface implementation */
	virtual void StartupModule();
	virtual void ShutdownModule();

private:
	FString StreamlineBinaryDirectory;
};

STREAMLINERHI_API void PlatformCreateStreamlineRHI();
STREAMLINERHI_API FStreamlineRHI* GetPlatformStreamlineRHI();
STREAMLINERHI_API EStreamlineSupport GetPlatformStreamlineSupport();
STREAMLINERHI_API bool IsStreamlineSupported();
STREAMLINERHI_API bool AreStreamlineFunctionsLoaded();

STREAMLINERHI_API sl::FeatureRequirementFlags PlatformGetAllImplementedStreamlineRHIs();


namespace sl
{

};


STREAMLINERHI_API void LogStreamlineFeatureSupport(sl::Feature Feature, const sl::AdapterInfo& Adapter);
STREAMLINERHI_API void LogStreamlineFeatureRequirements(sl::Feature Feature, const sl::FeatureRequirements& Requirements);