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
#include "FSR2.h"
#include "FSR2ViewExtension.h"

#include "Runtime/Launch/Resources/Version.h"

static_assert((ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION == 3 && ENGINE_PATCH_VERSION >= 0), "FSR 2.2 plugin requires UE5.3.x");

IMPLEMENT_MODULE(FFSR2Module, FSR2)

void FFSR2Module::StartupModule()
{
	// FSR2's view extension will always be enabled, but that isn't the same as enabling FSR2 itself.
	// This allows FSR2 to coexist with other upscalers.
	FSR2ViewExtension = FSceneViewExtensions::NewExtension<FFSR2ViewExtension>();
}

void FFSR2Module::ShutdownModule()
{
	// This is a smart pointer. Setting it to null is the correct way to release its memory.
	FSR2ViewExtension = nullptr;
}