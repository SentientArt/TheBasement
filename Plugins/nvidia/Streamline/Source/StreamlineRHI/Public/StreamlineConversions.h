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

#include "sl.h"
#include "sl_helpers.h"

inline sl::float4x4 ToSL(const FRHIStreamlineArguments::FMatrix44f& InMatrix, bool bIsOrthographicProjection = false)
{
	// This assertion can still fire when switching between ortho/perspective, which we don't support
	// Some interesting PrevClipToClip have been seen for example
	//check(bIsOrthographicProjection || !FMath::IsNearlyZero(InMatrix.Determinant()));

	sl::float4x4 Result;
	for (int i = 0; i < 4; i++)
	{
		Result.setRow(i, { InMatrix.M[i][0], InMatrix.M[i][1], InMatrix.M[i][2], InMatrix.M[i][3] });
	}
	return Result;
};

inline sl::float4 ToSL(const FRHIStreamlineArguments::FVector4f& InVector)
{
	return { InVector.X, InVector.Y, InVector.Z, InVector.W };
};

inline sl::float3 ToSL(const FRHIStreamlineArguments::FVector3f& InVector)
{
	return { InVector.X, InVector.Y, InVector.Z };
};

inline sl::float2 ToSL(const FRHIStreamlineArguments::FVector2f& InVector)
{
	return { InVector.X, InVector.Y };
};


inline sl::Boolean ToSL(bool b)
{
	return b ? sl::eTrue : sl::eFalse;
};

inline sl::Extent ToSL(const FIntRect& InRect)
{
	check(InRect.Min.X >= 0);
	check(InRect.Min.Y >= 0);

	check(InRect.Width() >= 0);
	check(InRect.Height() >= 0);
	return { uint32_t(InRect.Min.X), uint32_t(InRect.Min.Y), uint32_t(InRect.Width()), uint32_t(InRect.Height()) };
};

inline sl::BufferType ToSL(EStreamlineResource InResourceTag)
{
	switch (InResourceTag)
	{
	default:
		checkf(false, TEXT("unexpected EStreamlineResource enum value %u. This is a UE Streamline plugin developer bug"), InResourceTag);
	case EStreamlineResource::Depth: return sl::kBufferTypeDepth;
	case EStreamlineResource::MotionVectors: return sl::kBufferTypeMotionVectors;
	case EStreamlineResource::HUDLessColor: return sl::kBufferTypeHUDLessColor;
	case EStreamlineResource::UIColorAndAlpha: return sl::kBufferTypeUIColorAndAlpha;
	}
}

