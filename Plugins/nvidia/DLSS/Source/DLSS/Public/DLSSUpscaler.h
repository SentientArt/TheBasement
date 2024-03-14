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

#include "CoreMinimal.h"
#include "CustomResourcePool.h"

struct FDLSSOptimalSettings;
class FSceneViewFamily;
class NGXRHI;

enum class EDLSSQualityMode
{
	MinValue = -2,
	UltraPerformance = -2,
	Performance = -1,
	Balanced = 0,
	Quality = 1,
	UltraQuality = 2,
	DLAA = 3,
	MaxValue = DLAA,
	NumValues = 6
};

class DLSS_API FDLSSUpscaler final : public ICustomResourcePool
{

	friend class FDLSSModule;
public:
	UE_NONCOPYABLE(FDLSSUpscaler)

	void SetupViewFamily(FSceneViewFamily& ViewFamily);

	float GetOptimalResolutionFractionForQuality(EDLSSQualityMode Quality) const;
	float GetOptimalSharpnessForQuality(EDLSSQualityMode Quality) const;
	float GetMinResolutionFractionForQuality(EDLSSQualityMode Quality) const;
	float GetMaxResolutionFractionForQuality(EDLSSQualityMode Quality) const;
	bool IsFixedResolutionFraction(EDLSSQualityMode Quality) const;
	
	const NGXRHI* GetNGXRHI() const
	{
		return NGXRHIExtensions;
	}

	// Inherited via ICustomResourcePool
	virtual void Tick(FRHICommandListImmediate& RHICmdList) override;

	bool IsQualityModeSupported(EDLSSQualityMode InQualityMode) const;
	uint32 GetNumRuntimeQualityModes() const
	{
		return NumRuntimeQualityModes;
	}

	bool IsDLSSActive() const;

	// Give the suggested EDLSSQualityMode if one is appropriate for the given pixel count, or nothing if DLSS should be disabled
	TOptional<EDLSSQualityMode> GetAutoQualityModeFromPixels(int PixelCount) const;

	static void ReleaseStaticResources();

	static float GetMinUpsampleResolutionFraction()
	{
		return MinDynamicResolutionFraction;
	}

	static float GetMaxUpsampleResolutionFraction()
	{
		return MaxDynamicResolutionFraction;
	}

private:
	FDLSSUpscaler(NGXRHI* InNGXRHIExtensions);

	bool EnableDLSSInPlayInEditorViewports() const;

	// The FDLSSUpscaler(NGXRHI*) will update those once
	static NGXRHI* NGXRHIExtensions;
	static float MinDynamicResolutionFraction;
	static float MaxDynamicResolutionFraction;

	static uint32 NumRuntimeQualityModes;
	static TArray<FDLSSOptimalSettings> ResolutionSettings;
	float PreviousResolutionFraction;

	friend class FDLSSUpscalerViewExtension;
	friend class FDLSSSceneViewFamilyUpscaler;
};

