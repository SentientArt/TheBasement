// This file is part of the FidelityFX Super Resolution 2.1 Unreal Engine Plugin.
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
#include "FSR2TemporalUpscalingD3D12.h"

#include "CoreMinimal.h"
#include "Interfaces/IPluginManager.h"

#if PLATFORM_WINDOWS
#define GetD3D11CubeFace GetD3D12CubeFace
#define VerifyD3D11Result VerifyD3D12Result
#define GetD3D11TextureFromRHITexture GetD3D12TextureFromRHITexture
#define FRingAllocation FRingAllocation_D3D12
#define GetRenderTargetFormat GetRenderTargetFormat_D3D12
#define ED3D11ShaderOffsetBuffer ED3D12ShaderOffsetBuffer
#define FindShaderResourceDXGIFormat FindShaderResourceDXGIFormat_D3D12
#define FindUnorderedAccessDXGIFormat FindUnorderedAccessDXGIFormat_D3D12
#define FindDepthStencilDXGIFormat FindDepthStencilDXGIFormat_D3D12
#define HasStencilBits HasStencilBits_D3D12
#define FVector4VertexDeclaration FVector4VertexDeclaration_D3D12
#define GLOBAL_CONSTANT_BUFFER_INDEX GLOBAL_CONSTANT_BUFFER_INDEX_D3D12
#define MAX_CONSTANT_BUFFER_SLOTS MAX_CONSTANT_BUFFER_SLOTS_D3D12
#define FD3DGPUProfiler FD3D12GPUProfiler
#define FRangeAllocator FRangeAllocator_D3D12

#ifndef WITH_NVAPI
#define WITH_NVAPI 0
#endif // WITH_NVAPI
#ifndef NV_AFTERMATH
#define NV_AFTERMATH 0
#endif // NV_AFTERMATH
#ifndef INTEL_EXTENSIONS
#define INTEL_EXTENSIONS 0
#endif // INTEL_EXTENSIONS

#include "D3D12RHIPrivate.h"
#include "D3D12Util.h"

#undef GetD3D11CubeFace
#undef VerifyD3D11Result
#undef GetD3D11TextureFromRHITexture
#undef FRingAllocation
#undef GetRenderTargetFormat
#undef ED3D11ShaderOffsetBuffer
#undef FindShaderResourceDXGIFormat
#undef FindUnorderedAccessDXGIFormat
#undef FindDepthStencilDXGIFormat
#undef HasStencilBits
#undef FVector4VertexDeclaration
#undef GLOBAL_CONSTANT_BUFFER_INDEX
#undef MAX_CONSTANT_BUFFER_SLOTS
#undef FD3DGPUProfiler
#undef FRangeAllocator
#endif // PLATFORM_WINDOWS

IMPLEMENT_MODULE(FFSR2TemporalUpscalingD3D12Module, FSR2TemporalUpscalingD3D12)

#define LOCTEXT_NAMESPACE "FSR2"

void FFSR2TemporalUpscalingD3D12Module::StartupModule()
{
}

void FFSR2TemporalUpscalingD3D12Module::ShutdownModule()
{
}

void* FFSR2TemporalUpscalingD3D12::GetNativeCommandBuffer(FRHICommandListImmediate& RHICmdList)
{
	void* CmdList = GetID3D12DynamicRHI()->RHIGetGraphicsCommandList(0);
	return CmdList;
}

void* FFSR2TemporalUpscalingD3D12::GetNativeTextureResource(FRHITexture* Texture)
{
	return Texture->GetNativeResource();
}

uint32 FFSR2TemporalUpscalingD3D12::GetNativeTextureFormat(FRHITexture* Texture)
{
	return 0;
}

FSR2ShaderModel FFSR2TemporalUpscalingD3D12::GetSupportedShaderModel()
{
	FSR2ShaderModel ShaderModel = FSR2_SHADER_MODEL_5_1;
	ID3D12Device* dx12Device = (ID3D12Device*)GDynamicRHI->RHIGetNativeDevice();
	D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_6 };
	if (dx12Device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(D3D12_FEATURE_DATA_SHADER_MODEL)) >= 0)
	{
		switch (shaderModel.HighestShaderModel)
		{
		case D3D_SHADER_MODEL_5_1:
			ShaderModel = FSR2_SHADER_MODEL_5_1;
			break;
		case D3D_SHADER_MODEL_6_0:
			ShaderModel = FSR2_SHADER_MODEL_6_0;
			break;
		case D3D_SHADER_MODEL_6_1:
			ShaderModel = FSR2_SHADER_MODEL_6_1;
			break;
		case D3D_SHADER_MODEL_6_2:
			ShaderModel = FSR2_SHADER_MODEL_6_2;
			break;
		case D3D_SHADER_MODEL_6_3:
			ShaderModel = FSR2_SHADER_MODEL_6_3;
			break;
		case D3D_SHADER_MODEL_6_4:
			ShaderModel = FSR2_SHADER_MODEL_6_4;
			break;
		case D3D_SHADER_MODEL_6_5:
			ShaderModel = FSR2_SHADER_MODEL_6_5;
			break;
		case D3D_SHADER_MODEL_6_6:
		default:
			ShaderModel = FSR2_SHADER_MODEL_6_6;
			break;
		}
	}

	return ShaderModel;
}

bool FFSR2TemporalUpscalingD3D12::IsFloat16Supported()
{
	bool bIsSupported = false;
	ID3D12Device* dx12Device = (ID3D12Device*)GDynamicRHI->RHIGetNativeDevice();
	// check if we have 16bit floating point.
	D3D12_FEATURE_DATA_D3D12_OPTIONS d3d12Options = {};
	if (SUCCEEDED(dx12Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &d3d12Options, sizeof(d3d12Options)))) {

		bIsSupported = !!(d3d12Options.MinPrecisionSupport & D3D12_SHADER_MIN_PRECISION_SUPPORT_16_BIT);
	}
	return bIsSupported;
}

void FFSR2TemporalUpscalingD3D12::ForceUAVTransition(FRHICommandListImmediate& RHICmdList, FRHITexture* OutputTexture)
{
	FRHITransitionInfo Info(OutputTexture, ERHIAccess::Unknown, ERHIAccess::UAVMask);
	RHICmdList.Transition(Info);
}

#undef LOCTEXT_NAMESPACE
