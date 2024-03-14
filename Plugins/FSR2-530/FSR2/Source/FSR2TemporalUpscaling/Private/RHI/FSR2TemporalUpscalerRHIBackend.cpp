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
#include "FSR2TemporalUpscalerRHIBackend.h"
#include "FSR2TemporalUpscalerSubPass.h"
#include "FSR2TemporalUpscaler.h"
#include "RenderGraphUtils.h"

static EPixelFormat GetUEFormat(FfxSurfaceFormat Format)
{
	EPixelFormat UEFormat = PF_Unknown;
	switch (Format)
	{
		case FFX_SURFACE_FORMAT_R32G32B32A32_TYPELESS:
			UEFormat = PF_R32G32B32A32_UINT;
			break;
		case FFX_SURFACE_FORMAT_R32G32B32A32_FLOAT:
			UEFormat = PF_A32B32G32R32F;
			break;
		case FFX_SURFACE_FORMAT_R16G16B16A16_FLOAT:
			UEFormat = PF_FloatRGBA;
			break;
		case FFX_SURFACE_FORMAT_R16G16B16A16_UNORM:
			UEFormat = PF_R16G16B16A16_UNORM;
			break;
		case FFX_SURFACE_FORMAT_R32G32_FLOAT:
			UEFormat = PF_G32R32F;
			break;
		case FFX_SURFACE_FORMAT_R32_UINT:
			UEFormat = PF_R32_UINT;
			break;
		case FFX_SURFACE_FORMAT_R8G8B8A8_TYPELESS:
			UEFormat = PF_R8G8B8A8_UINT;
			break;
		case FFX_SURFACE_FORMAT_R8G8B8A8_UNORM:
			UEFormat = PF_R8G8B8A8;
			break;
		case FFX_SURFACE_FORMAT_R11G11B10_FLOAT:
			UEFormat = PF_FloatR11G11B10;
			break;
		case FFX_SURFACE_FORMAT_R16G16_FLOAT:
			UEFormat = PF_G16R16F;
			break;
		case FFX_SURFACE_FORMAT_R16G16_UINT:
			UEFormat = PF_R16G16_UINT;
			break;
		case FFX_SURFACE_FORMAT_R16_FLOAT:
			UEFormat = PF_R16F;
			break;
		case FFX_SURFACE_FORMAT_R16_UINT:
			UEFormat = PF_R16_UINT;
			break;
		case FFX_SURFACE_FORMAT_R16_UNORM:
			UEFormat = PF_G16;
			break;
		case FFX_SURFACE_FORMAT_R16_SNORM:
			UEFormat = PF_R16G16B16A16_SNORM;
			break;
		case FFX_SURFACE_FORMAT_R8_UNORM:
			UEFormat = PF_R8;
			break;
		case FFX_SURFACE_FORMAT_R32_FLOAT:
			UEFormat = PF_R32_FLOAT;
			break;
		case FFX_SURFACE_FORMAT_R8G8_UNORM:
			UEFormat = PF_R8G8;
			break;
		default:
			check(false);
			break;
	}
	return UEFormat;
}

static FfxSurfaceFormat GetFFXFormat(EPixelFormat UEFormat)
{
	FfxSurfaceFormat Format = FFX_SURFACE_FORMAT_UNKNOWN;
	switch (UEFormat)
	{
	case PF_R32G32B32A32_UINT:
		Format = FFX_SURFACE_FORMAT_R32G32B32A32_TYPELESS;
		break;
	case PF_A32B32G32R32F:
		Format = FFX_SURFACE_FORMAT_R32G32B32A32_FLOAT;
		break;
	case PF_FloatRGBA:
		Format = FFX_SURFACE_FORMAT_R16G16B16A16_FLOAT;
		break;
	case PF_R16G16B16A16_UNORM:
		Format = FFX_SURFACE_FORMAT_R16G16B16A16_UNORM;
		break;
	case PF_G32R32F:
		Format = FFX_SURFACE_FORMAT_R32G32_FLOAT;
		break;
	case PF_R32_UINT:
		Format = FFX_SURFACE_FORMAT_R32_UINT;
		break;
	case PF_R8G8B8A8_UINT:
		Format = FFX_SURFACE_FORMAT_R8G8B8A8_TYPELESS;
		break;
	case PF_R8G8B8A8:
	case PF_B8G8R8A8:
		Format = FFX_SURFACE_FORMAT_R8G8B8A8_UNORM;
		break;
	case PF_FloatR11G11B10:
	case PF_FloatRGB:
		Format = FFX_SURFACE_FORMAT_R11G11B10_FLOAT;
		break;
	case PF_G16R16F:
		Format = FFX_SURFACE_FORMAT_R16G16_FLOAT;
		break;
	case PF_R16G16_UINT:
		Format = FFX_SURFACE_FORMAT_R16G16_UINT;
		break;
	case PF_R16F:
		Format = FFX_SURFACE_FORMAT_R16_FLOAT;
		break;
	case PF_R16_UINT:
		Format = FFX_SURFACE_FORMAT_R16_UINT;
		break;
	case PF_G16:
		Format = FFX_SURFACE_FORMAT_R16_UNORM;
		break;
	case PF_R16G16B16A16_SNORM:
		Format = FFX_SURFACE_FORMAT_R16_SNORM;
		break;
	case PF_R8:
		Format = FFX_SURFACE_FORMAT_R8_UNORM;
		break;
	case PF_R32_FLOAT:
		Format = FFX_SURFACE_FORMAT_R32_FLOAT;
		break;
	case PF_DepthStencil:
		Format = FFX_SURFACE_FORMAT_R32_FLOAT;
		break;
	case PF_R8G8:
		Format = FFX_SURFACE_FORMAT_R8G8_UNORM;
		break;
	default:
		check(false);
		break;
	}
	return Format;
}

static ERHIAccess GetUEAccessState(FfxResourceStates State)
{
	ERHIAccess Access = ERHIAccess::Unknown;

	switch (State)
	{
		case FFX_RESOURCE_STATE_UNORDERED_ACCESS:
			Access = ERHIAccess::UAVMask;
			break;
		case FFX_RESOURCE_STATE_COMPUTE_READ:
			Access = ERHIAccess::SRVCompute;
			break;
		case FFX_RESOURCE_STATE_COPY_SRC:
			Access = ERHIAccess::CopySrc;
			break;
		case FFX_RESOURCE_STATE_COPY_DEST:
			Access = ERHIAccess::CopyDest;
			break;
		default:
			break;
	}

	return Access;
}

struct FFSR2TextureBulkData final : public FResourceBulkDataInterface
{
	FFSR2TextureBulkData()
	: Data(nullptr)
	, DataSize(0)
	{
	}

	FFSR2TextureBulkData(const void* InData, uint32 InDataSize)
	: Data(InData)
	, DataSize(InDataSize)
	{
	}

	const void* GetResourceBulkData() const { return Data; }
	uint32 GetResourceBulkDataSize() const { return DataSize; }
	
	void Discard() {}
	
	const void* Data = nullptr;
	uint32 DataSize = 0;
};

static FfxErrorCode CreateResource_UE(FfxFsr2Interface* backendInterface, const FfxCreateResourceDescription* desc, FfxResourceInternal* outTexture)
{
	FfxErrorCode Result = FFX_OK;
	FFSR2BackendState* Context = (FFSR2BackendState*)backendInterface->scratchBuffer;
	FFSR2TemporalUpscaler* Upscaler = (FFSR2TemporalUpscaler*)Context->device;
	FRDGBuilder* GraphBuilder = Upscaler ? Upscaler->GetGraphBuilder() : nullptr;
	
	if (Context && GraphBuilder)
	{
		ETextureCreateFlags Flags = TexCreate_None;
		Flags |= (desc->usage & FFX_RESOURCE_USAGE_READ_ONLY) ? TexCreate_ShaderResource : TexCreate_None;
		Flags |= (desc->usage & FFX_RESOURCE_USAGE_RENDERTARGET) ? TexCreate_RenderTargetable | TexCreate_ShaderResource : TexCreate_None;
		Flags |= (desc->usage & FFX_RESOURCE_USAGE_UAV) ? TexCreate_UAV | TexCreate_ShaderResource : TexCreate_None;

		FRHIResourceCreateInfo Info(WCHAR_TO_TCHAR(desc->name));

		FFSR2TextureBulkData BulkData(desc->initData, desc->initDataSize);
		if (desc->resourceDescription.format == FFX_SURFACE_FORMAT_R16_SNORM && desc->initData)
		{
			int16* Data = (int16*)FMemory::Malloc(desc->initDataSize * 4);
			for (uint32 i = 0; i < (desc->initDataSize / sizeof(int16)); i++)
			{
				Data[i * 4] = ((int16*)desc->initData)[i];
				Data[i * 4 + 1] = 0;
				Data[i * 4 + 1] = 0;
				Data[i * 4 + 1] = 0;
			}

			BulkData.Data = Data;
			BulkData.DataSize = desc->initDataSize * 4;
		}
		Info.BulkData = desc->initData && desc->initDataSize ? &BulkData : nullptr;

		switch (desc->resourceDescription.type)
		{
			case FFX_RESOURCE_TYPE_BUFFER:
			{
				uint32 Size = desc->resourceDescription.format != FFX_SURFACE_FORMAT_R16_SNORM ? desc->resourceDescription.width : desc->resourceDescription.width * 4;
				FRDGBufferRef BufferRef = GraphBuilder->CreateBuffer(FRDGBufferDesc::CreateBufferDesc(sizeof(uint32), Size), WCHAR_TO_TCHAR(desc->name));
				TRefCountPtr<FRDGPooledBuffer>* PooledBuffer = new TRefCountPtr<FRDGPooledBuffer>;			
				*PooledBuffer = GraphBuilder->ConvertToExternalBuffer(BufferRef);
				FBufferRHIRef VB = (*PooledBuffer)->GetRHI();
				check(VB.GetReference());
				if (Info.BulkData)
				{
					void* Dest = FRHICommandListExecutor::GetImmediateCommandList().LockBuffer(VB, 0, desc->resourceDescription.width, EResourceLockMode::RLM_WriteOnly);
					FMemory::Memcpy(Dest, BulkData.Data, FMath::Min(Size, desc->initDataSize));
					FRHICommandListExecutor::GetImmediateCommandList().UnlockBuffer(VB);
				}

				outTexture->internalIndex = Context->AddResource(VB.GetReference(), desc->resourceDescription.type, nullptr, nullptr, PooledBuffer);
				Context->Resources[outTexture->internalIndex].Desc = desc->resourceDescription;
				break;
			}
			case FFX_RESOURCE_TYPE_TEXTURE2D:
			{
				uint32 NumMips = desc->resourceDescription.mipCount > 0 ? desc->resourceDescription.mipCount : FMath::FloorToInt(FMath::Log2((float)FMath::Max(desc->resourceDescription.width, desc->resourceDescription.height)));

				FRHITextureCreateDesc Desc = FRHITextureCreateDesc::Create2D(WCHAR_TO_TCHAR(desc->name), desc->resourceDescription.width, desc->resourceDescription.height, GetUEFormat(desc->resourceDescription.format));
				Desc.SetBulkData(Info.BulkData);
				Desc.SetNumMips(NumMips);
				Desc.SetInitialState(Info.BulkData ? ERHIAccess::SRVCompute : GetUEAccessState(desc->initalState));
				Desc.SetNumSamples(1);
				Desc.SetFlags(Flags);
				FTextureRHIRef Texture = RHICreateTexture(Desc);

				TRefCountPtr<IPooledRenderTarget>* PooledRT = new TRefCountPtr<IPooledRenderTarget>;
				*PooledRT = CreateRenderTarget(Texture.GetReference(),WCHAR_TO_TCHAR( desc->name));
				outTexture->internalIndex = Context->AddResource(Texture.GetReference(), desc->resourceDescription.type, PooledRT, nullptr, nullptr);
				Context->Resources[outTexture->internalIndex].Desc = desc->resourceDescription;
				Context->Resources[outTexture->internalIndex].Desc.mipCount = NumMips;
				break;
			}
			case FFX_RESOURCE_TYPE_TEXTURE3D:
			{
				uint32 NumMips = desc->resourceDescription.mipCount > 0 ? desc->resourceDescription.mipCount : FMath::FloorToInt(FMath::Log2((float)FMath::Max(FMath::Max(desc->resourceDescription.width, desc->resourceDescription.height), desc->resourceDescription.depth)));

				FRHITextureCreateDesc Desc = FRHITextureCreateDesc::Create3D(WCHAR_TO_TCHAR(desc->name), desc->resourceDescription.width, desc->resourceDescription.height, desc->resourceDescription.depth, GetUEFormat(desc->resourceDescription.format));
				Desc.SetBulkData(Info.BulkData);
				Desc.SetNumMips(NumMips);
				Desc.SetInitialState(Info.BulkData ? ERHIAccess::SRVCompute : GetUEAccessState(desc->initalState));
				Desc.SetNumSamples(1);
				Desc.SetFlags(Flags);
				FTextureRHIRef Texture = RHICreateTexture(Desc);

				TRefCountPtr<IPooledRenderTarget>* PooledRT = new TRefCountPtr<IPooledRenderTarget>;
				*PooledRT = CreateRenderTarget(Texture.GetReference(), WCHAR_TO_TCHAR(desc->name));
				outTexture->internalIndex = Context->AddResource(Texture.GetReference(), desc->resourceDescription.type, PooledRT, nullptr, nullptr);
				Context->Resources[outTexture->internalIndex].Desc = desc->resourceDescription;
				Context->Resources[outTexture->internalIndex].Desc.mipCount = NumMips;
				break;
			}
			case FFX_RESOURCE_TYPE_TEXTURE1D:
			default:
			{
				Result = FFX_ERROR_INVALID_ENUM;
				break;
			}
		}

		if (desc->resourceDescription.format == FFX_SURFACE_FORMAT_R16_SNORM && Info.BulkData)
		{
			FMemory::Free(const_cast<void*>(BulkData.Data));
		}
	}
	else
	{
		Result = FFX_ERROR_INVALID_ARGUMENT;
	}

	return Result;
}

static FfxResourceDescription GetResourceDesc_UE(FfxFsr2Interface* backendInterface, FfxResourceInternal resource)
{
	FFSR2BackendState* backendContext = (FFSR2BackendState*)backendInterface->scratchBuffer;

	FfxResourceDescription desc = backendContext->Resources[resource.internalIndex].Desc;
	return desc;
}

static FfxErrorCode GetDeviceCapabilities_UE(FfxFsr2Interface* backendInterface, FfxDeviceCapabilities* deviceCapabilities, FfxDevice device)
{
	deviceCapabilities->minimumSupportedShaderModel = FFX_SHADER_MODEL_5_1;

	// We are just going to assume no FP16 support and let the compiler do what is needs to
	deviceCapabilities->fp16Supported = false;

	// Only DX12 can tell us what the min & max wave sizes are properly
	if (IsRHIDeviceAMD())
	{
		deviceCapabilities->waveLaneCountMin = 64;
		deviceCapabilities->waveLaneCountMax = 64;
	}
	else
	{
		deviceCapabilities->waveLaneCountMin = 32;
		deviceCapabilities->waveLaneCountMax = 32;
	}

	FString RHIName = GDynamicRHI->GetName();
#if FSR2_ENABLE_DX12
	if (RHIName == FFSR2Strings::D3D12)
	{
		FFSR2TemporalUpscalingD3D12 ApiAccessor;

		deviceCapabilities->waveLaneCountMin = GRHIMinimumWaveSize;
		deviceCapabilities->waveLaneCountMax = GRHIMaximumWaveSize;

		deviceCapabilities->minimumSupportedShaderModel = (FfxShaderModel)ApiAccessor.GetSupportedShaderModel();

		deviceCapabilities->fp16Supported = ApiAccessor.IsFloat16Supported();
	}
#endif
#if FSR2_ENABLE_VK
	if (RHIName == FFSR2Strings::Vulkan)
	{
		FFSR2TemporalUpscalingVulkan ApiAccessor;

		deviceCapabilities->minimumSupportedShaderModel = (FfxShaderModel)ApiAccessor.GetSupportedShaderModel();

		deviceCapabilities->fp16Supported = ApiAccessor.IsFloat16Supported();
	}
#endif
	
	// We can rely on the RHI telling us if raytracing is supported
	deviceCapabilities->raytracingSupported = GRHISupportsRayTracing;
	return FFX_OK;
}

static FfxErrorCode CreateDevice_UE(FfxFsr2Interface* backendInterface, FfxDevice device)
{
	FFSR2BackendState* backendContext = (FFSR2BackendState*)backendInterface->scratchBuffer;
	FMemory::Memzero(backendInterface->scratchBuffer, backendInterface->scratchBufferSize);
	backendContext->ResourceMask = 0xffffffffffffffff;
	backendContext->device = device;

	return FFX_OK;
}

static FfxErrorCode ReleaseDevice_UE(FfxFsr2Interface* backendInterface)
{
	FFSR2BackendState* backendContext = (FFSR2BackendState*)backendInterface->scratchBuffer;
	for (int i = 0; i < FSR2_MAX_RESOURCE_COUNT; ++i)
	{
		if (backendContext->Resources[i].Resource)
		{
			backendContext->RemoveResource(i);
		}
	}
	return FFX_OK;
}

static FfxErrorCode CreatePipeline_UE(FfxFsr2Interface* backendInterface, FfxFsr2Pass passId, const FfxPipelineDescription* desc, FfxPipelineState* outPipeline)
{
	FfxDeviceCapabilities deviceCapabilities;
	GetDeviceCapabilities_UE(backendInterface, &deviceCapabilities, nullptr);

	FfxErrorCode Result = FFX_OK;
	FFSR2BackendState* Context = backendInterface ? (FFSR2BackendState*)backendInterface->scratchBuffer : nullptr;
	if (Context && desc && outPipeline)
	{
		switch (passId)
		{
			case FFX_FSR2_PASS_DEPTH_CLIP:
			{
				outPipeline->pipeline = (FfxPipeline*)GetDepthClipPass(passId, desc, outPipeline, deviceCapabilities.fp16Supported);
				break;
			}
			case FFX_FSR2_PASS_RECONSTRUCT_PREVIOUS_DEPTH:
			{
				outPipeline->pipeline = (FfxPipeline*)GetReconstructPreviousDepthPass(passId, desc, outPipeline, deviceCapabilities.fp16Supported);
				break;
			}
			case FFX_FSR2_PASS_LOCK:
			{
				outPipeline->pipeline = (FfxPipeline*)GetLockPass(passId, desc, outPipeline, deviceCapabilities.fp16Supported);
				break;
			}
			case FFX_FSR2_PASS_ACCUMULATE:
			case FFX_FSR2_PASS_ACCUMULATE_SHARPEN:
			{
				outPipeline->pipeline = (FfxPipeline*)GetAccumulatePass(passId, desc, outPipeline, deviceCapabilities.fp16Supported);
				break;
			}
			case FFX_FSR2_PASS_RCAS:
			{
				outPipeline->pipeline = (FfxPipeline*)GetRCASPass(passId, desc, outPipeline, false);
				break;
			}
			case FFX_FSR2_PASS_COMPUTE_LUMINANCE_PYRAMID:
			{
				outPipeline->pipeline = (FfxPipeline*)GetComputeLuminancePyramidPass(passId, desc, outPipeline, deviceCapabilities.fp16Supported);
				break;
			}
			case FFX_FSR2_PASS_GENERATE_REACTIVE:
			{
				outPipeline->pipeline = (FfxPipeline*)GetAutogenReactiveMaskPass(passId, desc, outPipeline, deviceCapabilities.fp16Supported);
				break;
			}
			case FFX_FSR2_PASS_TCR_AUTOGENERATE:
			{
				outPipeline->pipeline = (FfxPipeline*)GetTcrAutogeneratePass(passId, desc, outPipeline, deviceCapabilities.fp16Supported);
				break;
			}
			default:
			{
				Result = FFX_ERROR_INVALID_ARGUMENT;
				break;
			}
		}
	}
	else
	{
		Result = FFX_ERROR_INVALID_ARGUMENT;
	}
	return Result;
}

static FfxErrorCode ScheduleRenderJob_UE(FfxFsr2Interface* backendInterface, const FfxGpuJobDescription* job)
{
	FFSR2BackendState* backendContext = (FFSR2BackendState*)backendInterface->scratchBuffer;
	backendContext->Jobs[backendContext->NumJobs] = *job;
	if (job->jobType == FFX_GPU_JOB_COMPUTE)
	{
		// needs to copy SRVs and UAVs in case they are on the stack only
		FfxComputeJobDescription* computeJob = &backendContext->Jobs[backendContext->NumJobs].computeJobDescriptor;
		const uint32_t numConstBuffers = job->computeJobDescriptor.pipeline.constCount;
		for (uint32_t currentRootConstantIndex = 0; currentRootConstantIndex < numConstBuffers; ++currentRootConstantIndex)
		{
			computeJob->cbs[currentRootConstantIndex].uint32Size = job->computeJobDescriptor.cbs[currentRootConstantIndex].uint32Size;
			memcpy(computeJob->cbs[currentRootConstantIndex].data, job->computeJobDescriptor.cbs[currentRootConstantIndex].data, computeJob->cbs[currentRootConstantIndex].uint32Size * sizeof(uint32_t));
		}
	}
	backendContext->NumJobs++;

	return FFX_OK;
}

static FfxErrorCode FlushRenderJobs_UE( FfxFsr2Interface* backendInterface, FfxCommandList commandList)
{
	FfxErrorCode Result = FFX_OK;
	FFSR2BackendState* Context = backendInterface ? (FFSR2BackendState*)backendInterface->scratchBuffer : nullptr;
	FRDGBuilder* GraphBuilder = (FRDGBuilder*)commandList;
	if (Context && GraphBuilder)
	{
		for (uint32 i = 0; i < Context->NumJobs; i++)
		{
			FfxGpuJobDescription* job = &Context->Jobs[i];
			switch (job->jobType)
			{
				case FFX_GPU_JOB_CLEAR_FLOAT:
				{
					FRDGTexture* RdgTex = Context->GetRDGTexture(*GraphBuilder, job->clearJobDescriptor.target.internalIndex);
					if (RdgTex)
					{
						FRDGTextureUAVRef UAV = GraphBuilder->CreateUAV(RdgTex);
						AddClearUAVPass(*GraphBuilder, UAV, job->clearJobDescriptor.color);
					}
					else
					{
						FRDGBufferUAVRef UAV = GraphBuilder->CreateUAV(Context->GetRDGBuffer(*GraphBuilder, job->clearJobDescriptor.target.internalIndex), PF_R32_FLOAT);
						AddClearUAVFloatPass(*GraphBuilder, UAV, job->clearJobDescriptor.color[0]);
					}
					break;
				}
				case FFX_GPU_JOB_COPY:
				{
					if ((Context->GetType(job->copyJobDescriptor.src.internalIndex) == FFX_RESOURCE_TYPE_BUFFER) && (Context->GetType(job->copyJobDescriptor.dst.internalIndex) == FFX_RESOURCE_TYPE_BUFFER))
					{
						check(false);
					}
					else
					{
						FRHITexture* Src = (FRHITexture*)Context->GetResource(job->copyJobDescriptor.src.internalIndex);
						FRHITexture* Dst = (FRHITexture*)Context->GetResource(job->copyJobDescriptor.dst.internalIndex);

						FRHICopyTextureInfo Info;
						Info.NumMips = FMath::Min(Src->GetNumMips(), Dst->GetNumMips());
						AddCopyTexturePass(*GraphBuilder, Context->GetRDGTexture(*GraphBuilder, job->copyJobDescriptor.src.internalIndex), Context->GetRDGTexture(*GraphBuilder, job->copyJobDescriptor.dst.internalIndex), Info);
					}

					break;
				}
				case FFX_GPU_JOB_COMPUTE:
				{
					IFSR2SubPass* Pipeline = (IFSR2SubPass*)job->computeJobDescriptor.pipeline.pipeline;
					check(Pipeline);
					Pipeline->Dispatch(*GraphBuilder, Context, job);
					break;
				}
				default:
				{
					Result = FFX_ERROR_INVALID_ENUM;
					break;
				}
			}
		}

		Context->NumJobs = 0;
	}
	else
	{
		Result = FFX_ERROR_INVALID_ARGUMENT;
	}

	return Result;
}

static FfxErrorCode DestroyPipeline_UE(FfxFsr2Interface* backendInterface, FfxPipelineState* pipeline)
{
	FfxErrorCode Result = FFX_OK;

	if (pipeline && pipeline->pipeline)
	{
		delete (IFSR2SubPass*)pipeline->pipeline;
	}

	return Result;
}

static FfxErrorCode DestroyResource_UE(FfxFsr2Interface* backendInterface, FfxResourceInternal resource)
{
	FfxErrorCode Result = FFX_OK;
	FFSR2BackendState* Context = backendInterface ? (FFSR2BackendState*)backendInterface->scratchBuffer : nullptr;
	if (Context)
	{
		if (resource.internalIndex < FSR2_MAX_RESOURCE_COUNT)
		{
			Context->RemoveResource(resource.internalIndex);
		}
		else
		{
			Result = FFX_ERROR_OUT_OF_RANGE;
		}
	}
	else
	{
		Result = FFX_ERROR_INVALID_ARGUMENT;
	}

	return Result;
}

static FfxErrorCode RegisterResource_UE(FfxFsr2Interface* backendInterface, const FfxResource* inResource, FfxResourceInternal* outResource)
{
	FfxErrorCode Result = FFX_OK;
	FFSR2BackendState* Context = backendInterface ? (FFSR2BackendState*)backendInterface->scratchBuffer : nullptr;

	if (backendInterface && inResource && inResource->resource && outResource)
	{
		FRDGTexture* rdgRes = (FRDGTexture*)inResource->resource;
		auto const& Desc = rdgRes->Desc;
		outResource->internalIndex = Context->AddResource(nullptr, FFX_RESOURCE_TYPE_TEXTURE2D, nullptr, rdgRes, nullptr);
		Context->DynamicMask |= (1llu << outResource->internalIndex);
		Context->Resources[outResource->internalIndex].Desc.type = FFX_RESOURCE_TYPE_TEXTURE2D;
		Context->Resources[outResource->internalIndex].Desc.format = GetFFXFormat(Desc.Format);
		Context->Resources[outResource->internalIndex].Desc.width = Desc.GetSize().X;
		Context->Resources[outResource->internalIndex].Desc.height = Desc.GetSize().Y;
		Context->Resources[outResource->internalIndex].Desc.mipCount = Desc.NumMips;
	}
	else
	{
		Result = FFX_ERROR_INVALID_ARGUMENT;
	}

	return Result;
}

static FfxErrorCode UnregisterResources_UE(FfxFsr2Interface* backendInterface)
{
	FfxErrorCode Result = backendInterface ? FFX_OK : FFX_ERROR_INVALID_ARGUMENT;
	FFSR2BackendState* Context = backendInterface ? (FFSR2BackendState*)backendInterface->scratchBuffer : nullptr;

	while (Context && Context->DynamicMask)
	{
		uint32 Index = (uint32)FMath::CountTrailingZeros64(Context->DynamicMask);
		if (Index < FSR2_MAX_RESOURCE_COUNT)
		{
			Context->RemoveResource(Index);
			Context->DynamicMask &= ~(1llu << uint64(Index));
		}
	}

	return Result;
}

FfxErrorCode ffxFsr2GetInterfaceUE(FfxFsr2Interface* outInterface, class FFSR2TemporalUpscaler const* Upscaler, void* scratchBuffer, size_t scratchBufferSize)
{
	outInterface->fpGetDeviceCapabilities = GetDeviceCapabilities_UE;
	outInterface->fpCreateBackendContext = CreateDevice_UE;
	outInterface->fpDestroyBackendContext = ReleaseDevice_UE;
	outInterface->scratchBuffer = scratchBuffer;
	outInterface->scratchBufferSize = scratchBufferSize;

	outInterface->fpCreateResource = CreateResource_UE;
	outInterface->fpCreatePipeline = CreatePipeline_UE;
	outInterface->fpRegisterResource = RegisterResource_UE;
	outInterface->fpScheduleGpuJob = ScheduleRenderJob_UE;
	outInterface->fpExecuteGpuJobs = FlushRenderJobs_UE;
	outInterface->fpDestroyResource = DestroyResource_UE;
	outInterface->fpDestroyPipeline = DestroyPipeline_UE;
	outInterface->fpGetResourceDescription = GetResourceDesc_UE;
	outInterface->fpUnregisterResources = UnregisterResources_UE;

	return FFX_OK;
}

size_t ffxFsr2GetScratchMemorySize()
{
	return sizeof(FFSR2BackendState);
}

FfxResource ffxGetResourceFromUEResource(FfxFsr2Interface* backendInterface, FRDGTexture* rdgRes, FfxResourceStates state)
{
	FFSR2BackendState* backendContext = (FFSR2BackendState*)backendInterface->scratchBuffer;

	FfxResource resources = {};

	if (rdgRes)
	{
		resources.resource = (void*)rdgRes;
		resources.state = state;
		resources.isDepth = (rdgRes->Desc.Format == PF_DepthStencil) || (rdgRes->Desc.Format == PF_ShadowDepth);
		resources.description.type = FFX_RESOURCE_TYPE_TEXTURE2D;
		resources.description.format = GetFFXFormat(rdgRes->Desc.Format);
		resources.description.width = rdgRes->Desc.GetSize().X;
		resources.description.height = rdgRes->Desc.GetSize().Y;
		resources.description.depth = rdgRes->Desc.Depth;
		resources.description.mipCount = rdgRes->Desc.NumMips;
		resources.description.flags = FFX_RESOURCE_FLAGS_NONE;
	}
	return resources;
}

void ffxFsr2SetFeatureLevel(FfxFsr2Interface* backendInterface, ERHIFeatureLevel::Type FeatureLevel)
{
	FFSR2BackendState* backendContext = (FFSR2BackendState*)backendInterface->scratchBuffer;
	backendContext->FeatureLevel = FeatureLevel;
}

uint32 FFSR2BackendState::AddResource(FRHIResource* Resource, FfxResourceType Type, TRefCountPtr<IPooledRenderTarget>* RT, FRDGTexture* RDG, TRefCountPtr<FRDGPooledBuffer>* PooledBuffer)
{
	uint32 Index = 0;

	Index = (uint32)FMath::CountTrailingZeros64(ResourceMask);
	check(Index < FSR2_MAX_RESOURCE_COUNT);
	ResourceMask &= ~(1llu << uint64(Index));

	if (Resource)
	{
		Resource->AddRef();
	}
	Resources[Index].Resource = Resource;
	Resources[Index].RT = RT;
	Resources[Index].RDG = RDG;
	Resources[Index].PooledBuffer = PooledBuffer;
	Resources[Index].Desc.type = Type;

	return Index;
}

FRHIResource* FFSR2BackendState::GetResource(uint32 Index)
{
	FRHIResource* Res = nullptr;
	if ((Index < (uint32)FSR2_MAX_RESOURCE_COUNT) && !(ResourceMask & (1llu << uint64(Index))))
	{
		Res = Resources[Index].Resource;
	}
	return Res;
}

FRDGTextureRef FFSR2BackendState::GetOrRegisterExternalTexture(FRDGBuilder& GraphBuilder, uint32 Index)
{
	FRDGTextureRef Texture;
	Texture = GraphBuilder.FindExternalTexture((FRHITexture*)GetResource(Index));
	if (!Texture)
	{
		Texture = GraphBuilder.RegisterExternalTexture(GetPooledRT(Index));
	}
	return Texture;
}

FRDGTexture* FFSR2BackendState::GetRDGTexture(FRDGBuilder& GraphBuilder, uint32 Index)
{
	FRDGTexture* RDG = nullptr;
	if ((Index < (uint32)FSR2_MAX_RESOURCE_COUNT) && !(ResourceMask & (1llu << uint64(Index))) && Resources[Index].Desc.type != FFX_RESOURCE_TYPE_BUFFER)
	{
		RDG = Resources[Index].RDG;
		if (!RDG && Resources[Index].RT)
		{
			RDG = GetOrRegisterExternalTexture(GraphBuilder, Index);
		}
	}
	return RDG;
}

FRDGBufferRef FFSR2BackendState::GetRDGBuffer(FRDGBuilder& GraphBuilder, uint32 Index)
{
	FRDGBufferRef Buffer = nullptr;
	if ((Index < (uint32)FSR2_MAX_RESOURCE_COUNT) && !(ResourceMask & (1llu << uint64(Index))) && Resources[Index].Desc.type == FFX_RESOURCE_TYPE_BUFFER)
	{
		Buffer = GraphBuilder.RegisterExternalBuffer(*(Resources[Index].PooledBuffer));
	}
	return Buffer;
}

TRefCountPtr<IPooledRenderTarget> FFSR2BackendState::GetPooledRT(uint32 Index)
{
	TRefCountPtr<IPooledRenderTarget> Res;
	if ((Index < (uint32)FSR2_MAX_RESOURCE_COUNT) && !(ResourceMask & (1llu << uint64(Index))) && Resources[Index].RT)
	{
		Res = *(Resources[Index].RT);
	}
	return Res;
}

FfxResourceType FFSR2BackendState::GetType(uint32 Index)
{
	FfxResourceType Type = FFX_RESOURCE_TYPE_BUFFER;
	if ((Index < (uint32)FSR2_MAX_RESOURCE_COUNT) && !(ResourceMask & (1llu << uint64(Index))))
	{
		Type = Resources[Index].Desc.type;
	}
	return Type;
}

void FFSR2BackendState::RemoveResource(uint32 Index)
{
	if ((Index < (uint32)FSR2_MAX_RESOURCE_COUNT) && !(ResourceMask & (1llu << uint64(Index))))
	{
		if (Resources[Index].Resource)
		{
			Resources[Index].Resource->Release();
		}
		if (Resources[Index].RT)
		{
			delete Resources[Index].RT;
		}
		if (Resources[Index].PooledBuffer)
		{
			delete Resources[Index].PooledBuffer;
		}
		Resources[Index].PooledBuffer = nullptr;
		Resources[Index].RDG = nullptr;
		Resources[Index].RT = nullptr;
		Resources[Index].Resource = nullptr;
		ResourceMask |= (1llu << uint64(Index));
	}
}
