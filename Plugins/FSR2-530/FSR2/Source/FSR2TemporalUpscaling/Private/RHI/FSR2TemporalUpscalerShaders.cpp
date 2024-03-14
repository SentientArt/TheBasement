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
#include "FSR2TemporalUpscalerShaders.h"

#include "ShaderCompilerCore.h"

IMPLEMENT_UNIFORM_BUFFER_STRUCT(FFSR2PassParameters, "cbFSR2");
IMPLEMENT_UNIFORM_BUFFER_STRUCT(FFSR2ComputeLuminanceParameters, "cbSPD");
IMPLEMENT_UNIFORM_BUFFER_STRUCT(FFSR2RCASParameters, "cbRCAS");
IMPLEMENT_UNIFORM_BUFFER_STRUCT(FFSR2GenerateReactiveParameters, "cbGenerateReactive");

bool FFSR2GlobalShader::ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
{
	return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5) && IsPCPlatform(Parameters.Platform);
}

void FFSR2GlobalShader::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	OutEnvironment.CompilerFlags.Add(CFLAG_AllowTypedUAVLoads);

	FPermutationDomain PermutationVector(Parameters.PermutationId);
	bool bUseHalf = PermutationVector.Get<FFSR2_UseHalf>();
	if (bUseHalf && Parameters.Platform == SP_PCD3D_SM5)
	{
		OutEnvironment.CompilerFlags.Add(CFLAG_ForceDXC);
	}

	OutEnvironment.SetDefine(TEXT("FFX_GPU"), 1);
	OutEnvironment.SetDefine(TEXT("FFX_HLSL"), 1);

	OutEnvironment.SetDefine(TEXT("FFX_FSR2_OPTION_UPSAMPLE_SAMPLERS_USE_DATA_HALF"), 0);
	OutEnvironment.SetDefine(TEXT("FFX_FSR2_OPTION_ACCUMULATE_SAMPLERS_USE_DATA_HALF"), 0);
	OutEnvironment.SetDefine(TEXT("FFX_FSR2_OPTION_REPROJECT_SAMPLERS_USE_DATA_HALF"), 1);
	OutEnvironment.SetDefine(TEXT("FFX_FSR2_OPTION_POSTPROCESSLOCKSTATUS_SAMPLERS_USE_DATA_HALF"), 0);
	OutEnvironment.SetDefine(TEXT("FFX_FSR2_OPTION_UPSAMPLE_USE_LANCZOS_TYPE"), 2);
	OutEnvironment.SetDefine(TEXT("FFX_FSR2_OPTION_REPROJECT_USE_LANCZOS_TYPE"), 1);
	OutEnvironment.SetDefine(TEXT("FFX_FSR2_PREFER_WAVE64"), TEXT(""));

	// Rmove the unorm attribute when compiling with DX to avoid an fxc error.
	if (IsD3DPlatform(Parameters.Platform))
	{
		OutEnvironment.SetDefine(TEXT("unorm"), TEXT(" "));
	}

	// SPIRV platforms require some compilation hacks
	bool const bIsSPIRV = IsHlslccShaderPlatform(Parameters.Platform);
	OutEnvironment.SetDefine(TEXT("FFX_COMPILE_FOR_SPIRV"), bIsSPIRV ? 1 : 0);
}
