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
#include "PostProcess/PostProcessUpscale.h"
class FNISViewExtension;

DECLARE_LOG_CATEGORY_EXTERN(LogNIS, Log, All);

class FNISShadersModule final: public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule();

	virtual void ShutdownModule();

private:
};

NISSHADERS_API ERHIFeatureLevel::Type GetNISMinRequiredFeatureLevel();

NISSHADERS_API FScreenPassTexture AddSharpenOrUpscalePass(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	const ISpatialUpscaler::FInputs& Inputs
);

