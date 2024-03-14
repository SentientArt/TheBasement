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

#include "CoreMinimal.h"
#include "SceneRendering.h"
#include "FSR2Include.h"

using IFSR2CustomTemporalAAHistory = UE::Renderer::Private::ITemporalUpscaler::IHistory;

class FFSR2TemporalUpscaler;

//-------------------------------------------------------------------------------------
// The FSR2 state wrapper, deletion is handled by the RHI so that they aren't removed out from under the GPU.
//-------------------------------------------------------------------------------------
struct FFSR2State : public FRHIResource
{
	FFSR2State(uint32 const _ScratchSize)
	: FRHIResource(RRT_None)
	, ScratchSize(_ScratchSize)
	, ScratchBuffer(nullptr)
	, LastUsedFrame(~0u)
	{
		if (ScratchSize)
		{
			ScratchBuffer = (uint8*)FMemory::Malloc(ScratchSize);
		}
	}
	~FFSR2State()
	{
		ffxFsr2ContextDestroy(&Fsr2);
		if (ScratchBuffer)
		{
			FMemory::Free(ScratchBuffer);
		}
	}

	uint32 AddRef() const
	{
		return FRHIResource::AddRef();
	}

	uint32 Release() const
	{
		return FRHIResource::Release();
	}

	uint32 GetRefCount() const
	{
		return FRHIResource::GetRefCount();
	}

	uint32 ScratchSize;
	uint8* ScratchBuffer;
	FfxFsr2ContextDescription Params;
	FfxFsr2Context Fsr2;
	uint64 LastUsedFrame;
	uint32 ViewID;
};
typedef TRefCountPtr<FFSR2State> FSR2StateRef;

//-------------------------------------------------------------------------------------
// The ICustomTemporalAAHistory for FSR2, this retains the FSR2 state object.
//-------------------------------------------------------------------------------------
class FFSR2TemporalUpscalerHistory final : public IFSR2CustomTemporalAAHistory, public FRefCountBase
{
public:
	FFSR2TemporalUpscalerHistory(FSR2StateRef NewState, FFSR2TemporalUpscaler* Upscaler);

	virtual ~FFSR2TemporalUpscalerHistory();

	virtual const TCHAR* GetDebugName() const override;
	virtual uint64 GetGPUSizeBytes() const override;

	void SetState(FSR2StateRef NewState);

	inline FSR2StateRef const& GetState() const
	{
		return Fsr2;
	}
	
	uint32 AddRef() const final
	{
		return FRefCountBase::AddRef();
	}

	uint32 Release() const final
	{
		return FRefCountBase::Release();
	}

	uint32 GetRefCount() const final
	{
		return FRefCountBase::GetRefCount();
	}

private:
	FSR2StateRef Fsr2;
	FFSR2TemporalUpscaler* Upscaler;
};