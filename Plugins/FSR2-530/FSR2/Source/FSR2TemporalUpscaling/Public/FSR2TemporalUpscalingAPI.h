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

class FRHICommandListImmediate;
class FRHITexture;
class FRHIBuffer;

//-------------------------------------------------------------------------------------
// The shader model's supported by FSR2.
//-------------------------------------------------------------------------------------
enum FSR2ShaderModel
{
	FSR2_SHADER_MODEL_5_1,                           ///< Shader model 5.1.
	FSR2_SHADER_MODEL_6_0,                           ///< Shader model 6.0.
	FSR2_SHADER_MODEL_6_1,                           ///< Shader model 6.1.
	FSR2_SHADER_MODEL_6_2,                           ///< Shader model 6.2.
	FSR2_SHADER_MODEL_6_3,                           ///< Shader model 6.3.
	FSR2_SHADER_MODEL_6_4,                           ///< Shader model 6.4.
	FSR2_SHADER_MODEL_6_5,                           ///< Shader model 6.5.
	FSR2_SHADER_MODEL_6_6,                           ///< Shader model 6.6.
	FSR2_SHADER_MODEL_6_7,                           ///< Shader model 6.7.
};

//-------------------------------------------------------------------------------------
// FSR2 supports several native backend implementations which requires integration with the releveant RHI implementation.
// To facilitate this the IFSR2TemporalUpscalingAPI interface has to be implemented for relevant RHIs.
//-------------------------------------------------------------------------------------
class IFSR2TemporalUpscalingAPI
{
public:
	virtual ~IFSR2TemporalUpscalingAPI() {}
	virtual void* GetNativeCommandBuffer(FRHICommandListImmediate& RHICmdList) = 0;
	virtual void* GetNativeTextureResource(FRHITexture* Texture) = 0;
	virtual uint32 GetNativeTextureFormat(FRHITexture* Texture) = 0;
	virtual FSR2ShaderModel GetSupportedShaderModel() = 0;
	virtual bool IsFloat16Supported() = 0;
	virtual void ForceUAVTransition(FRHICommandListImmediate& RHICmdList, FRHITexture* OutputTexture) = 0;
};