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
#pragma once

#include "FSR2Include.h"
#include "GlobalShader.h"

//-------------------------------------------------------------------------------------
// Permutation variables for FSR2 shaders.
//-------------------------------------------------------------------------------------
class FFSR2_IsHDR : SHADER_PERMUTATION_BOOL("FFX_FSR2_OPTION_HDR_COLOR_INPUT");
class FFSR2_MVLowRes : SHADER_PERMUTATION_BOOL("FFX_FSR2_OPTION_LOW_RESOLUTION_MOTION_VECTORS");
class FFSR2_MVJittered : SHADER_PERMUTATION_BOOL("FFX_FSR2_OPTION_JITTERED_MOTION_VECTORS");
class FFSR2_DepthInverted : SHADER_PERMUTATION_BOOL("FFX_FSR2_OPTION_INVERTED_DEPTH");
class FFSR2_DoSharpening : SHADER_PERMUTATION_BOOL("FFX_FSR2_OPTION_APPLY_SHARPENING");
class FFSR2_UseHalf : SHADER_PERMUTATION_BOOL("FFX_HALF");

//-------------------------------------------------------------------------------------
// Common uniform buffer structure for the FSR2 pass parameters.
//-------------------------------------------------------------------------------------
BEGIN_UNIFORM_BUFFER_STRUCT(FFSR2PassParameters, )
	SHADER_PARAMETER(FIntPoint, iRenderSize)
	SHADER_PARAMETER(FIntPoint, iMaxRenderSize)
	SHADER_PARAMETER(FIntPoint, iDisplaySize)
	SHADER_PARAMETER(FIntPoint, iInputColorResourceDimensions)
	SHADER_PARAMETER(FIntPoint, iLumaMipDimensions)
	SHADER_PARAMETER(int, iLumaMipLevelToUse)
	SHADER_PARAMETER(int, iFrameIndex)

	SHADER_PARAMETER(FVector4f, fDeviceToViewDepth)
	SHADER_PARAMETER(FVector2f, fJitter)
	SHADER_PARAMETER(FVector2f, fMotionVectorScale)
	SHADER_PARAMETER(FVector2f, fDownscaleFactor)
	SHADER_PARAMETER(FVector2f, fMotionVectorJitterCancellation)
	SHADER_PARAMETER(float, fPreExposure)
	SHADER_PARAMETER(float, fPreviousFramePreExposure)
	SHADER_PARAMETER(float, fTanHalfFOV)
	SHADER_PARAMETER(float, fJitterSequenceLength)
	SHADER_PARAMETER(float, fDeltaTime)
	SHADER_PARAMETER(float, fDynamicResChangeFactor)
	SHADER_PARAMETER(float, fViewSpaceToMetersFactor)
END_UNIFORM_BUFFER_STRUCT()

//-------------------------------------------------------------------------------------
// Parameters for the compute luminance pyramid pass.
//-------------------------------------------------------------------------------------
BEGIN_UNIFORM_BUFFER_STRUCT(FFSR2ComputeLuminanceParameters, )
	SHADER_PARAMETER(uint32, mips)
	SHADER_PARAMETER(uint32, numWorkGroups)
	SHADER_PARAMETER(FUintVector2, workGroupOffset)
	SHADER_PARAMETER(FUintVector2, renderSize)
END_UNIFORM_BUFFER_STRUCT()

//-------------------------------------------------------------------------------------
// Paramters for the sharpening pass.
//-------------------------------------------------------------------------------------
BEGIN_UNIFORM_BUFFER_STRUCT(FFSR2RCASParameters, )
	SHADER_PARAMETER(FUintVector4, rcasConfig)
END_UNIFORM_BUFFER_STRUCT()

//-------------------------------------------------------------------------------------
// Paramters for the TCR Autogenerate pass.
//-------------------------------------------------------------------------------------
BEGIN_UNIFORM_BUFFER_STRUCT(FFSR2GenerateReactiveParameters, )
	SHADER_PARAMETER(float, fTcThreshold)
	SHADER_PARAMETER(float, fTcScale)
	SHADER_PARAMETER(float, fReactiveScale)
	SHADER_PARAMETER(float, fReactiveMax)
END_UNIFORM_BUFFER_STRUCT()

//-------------------------------------------------------------------------------------
// Common base class for the FSR2 shaders that handles the permutation support.
//-------------------------------------------------------------------------------------
class FFSR2GlobalShader : public FGlobalShader
{
public:
	typedef TShaderPermutationDomain<FFSR2_IsHDR, FFSR2_MVLowRes, FFSR2_MVJittered, FFSR2_DepthInverted, FFSR2_DoSharpening, FFSR2_UseHalf> FPermutationDomain;

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters);
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
};
