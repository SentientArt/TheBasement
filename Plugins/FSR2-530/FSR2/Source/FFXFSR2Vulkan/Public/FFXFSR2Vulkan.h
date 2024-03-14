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

#include "HAL/Platform.h"

#include "FFXFSR2.h"
#if defined(FSR2_VULKAN_INTERNAL)
#include "FSR2VulkanDynamicAPI.h"
#endif
#include "FFXFSR2VulkanModule.h"
#include "FSR2VulkanLoader.h"

#if FSR2_ENABLE_VK

#define fsr2GetPermutationBlobByIndex fsr2GetPermutationBlobByIndexVK

#if PLATFORM_WINDOWS
	#include "Windows/AllowWindowsPlatformTypes.h"
	THIRD_PARTY_INCLUDES_START

#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable : 4190)
#endif

	#include "vk/ffx_fsr2_vk.h"

#ifdef _MSC_VER
	#pragma warning(pop)
#endif

	THIRD_PARTY_INCLUDES_END
	#include "Windows/HideWindowsPlatformTypes.h"
#else
	#define FFX_GCC 1
	THIRD_PARTY_INCLUDES_START

	#include "vk/ffx_fsr2_vk.h"

	THIRD_PARTY_INCLUDES_END
	#undef FFX_GCC
#endif

#undef fsr2GetPermutationBlobByIndex

extern "C" FFX_API FfxErrorCode ffxFsr2UEGetInterfaceVK(
		FfxFsr2Interface* outInterface,
		void* scratchBuffer,
		size_t scratchBufferSize,
		VkPhysicalDevice physicalDevice);
#endif
