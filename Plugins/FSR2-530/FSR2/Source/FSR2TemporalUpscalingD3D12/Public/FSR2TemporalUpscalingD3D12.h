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

#include "Modules/ModuleManager.h"
#include "Runtime/Launch/Resources/Version.h"
#include "../../FSR2TemporalUpscaling/Public/FSR2TemporalUpscalingAPI.h"

class FFSR2TemporalUpscalingD3D12Module final : public IModuleInterface
{
public:
	// IModuleInterface implementation
	void StartupModule() override;
	void ShutdownModule() override;
};

class FFSR2TemporalUpscalingD3D12 final : public IFSR2TemporalUpscalingAPI
{
public:
	FSR2TEMPORALUPSCALINGD3D12_API virtual ~FFSR2TemporalUpscalingD3D12() {}
	FSR2TEMPORALUPSCALINGD3D12_API void* GetNativeCommandBuffer(class FRHICommandListImmediate& RHICmdList) final;
	FSR2TEMPORALUPSCALINGD3D12_API void* GetNativeTextureResource(FRHITexture* Texture) final;
	FSR2TEMPORALUPSCALINGD3D12_API uint32 GetNativeTextureFormat(FRHITexture* Texture) final;
	FSR2TEMPORALUPSCALINGD3D12_API FSR2ShaderModel GetSupportedShaderModel() final;
	FSR2TEMPORALUPSCALINGD3D12_API bool IsFloat16Supported() final;
	FSR2TEMPORALUPSCALINGD3D12_API void ForceUAVTransition(class FRHICommandListImmediate& RHICmdList, FRHITexture* OutputTexture);
};
