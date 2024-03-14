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

#include "CoreMinimal.h"
#include "RendererInterface.h"
#include "ScreenPass.h"


extern STREAMLINESHADERS_API FRDGTextureRef AddStreamlineVelocityCombinePass(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FRDGTextureRef InSceneDepthTexture,
	FRDGTextureRef InVelocityTexture,
	bool bDilateMotionVectors
);
