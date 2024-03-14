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
#include "Runtime/Launch/Resources/Version.h"

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
#include "TemporalUpscaler.h"
#else
#include "SceneRendering.h"
#endif

#include "NGXRHI.h"

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
class DLSS_API FDLSSUpscalerHistory final : public ITemporalUpscaler::IHistory, public FRefCountBase
#else
class DLSS_API FDLSSUpscalerHistory final : public ICustomTemporalAAHistory, public FRefCountBase
#endif
{
public:
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
	virtual const TCHAR* GetDebugName() const final override;
	virtual uint64 GetGPUSizeBytes() const final override;
#endif

private:
	friend class FDLSSSceneViewFamilyUpscaler;

	FDLSSStateRef DLSSState;



	virtual uint32 AddRef() const final
	{
		return FRefCountBase::AddRef();
	}

	virtual uint32 Release() const final
	{
		return FRefCountBase::Release();
	}

	virtual uint32 GetRefCount() const final
	{
		return FRefCountBase::GetRefCount();
	}

	FDLSSUpscalerHistory(FDLSSStateRef InDLSSState);
	~FDLSSUpscalerHistory();

};
