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
#include "RHIDefinitions.h"
#include "RHIShaderPlatform.h"

class FFSR2TemporalUpscaler;

//-------------------------------------------------------------------------------------
// In order for the FSR2 plugin to support the movie render pipeline some functions have to be exposed.
// This allows the separate FSR2MovieRenderPipeline to behave consistently with the main FSR2 plugin.
//-------------------------------------------------------------------------------------
class IFSR2TemporalUpscalingModule : public IModuleInterface
{
public:
	virtual FFSR2TemporalUpscaler* GetFSR2Upscaler() const = 0;
	virtual FFSR2TemporalUpscaler* GetTemporalUpscaler() const = 0;
	virtual float GetResolutionFraction(uint32 Mode) const = 0;
	virtual bool IsPlatformSupported(EShaderPlatform Platform) const = 0;
	virtual void SetEnabledInEditor(bool bEnabled) = 0;
};

class FFSR2TemporalUpscalingModule final : public IFSR2TemporalUpscalingModule
{
public:
	// IModuleInterface implementation
	void StartupModule() override;
	void ShutdownModule() override;

	void SetTemporalUpscaler(TSharedPtr<FFSR2TemporalUpscaler, ESPMode::ThreadSafe> Upscaler);

	FFSR2TemporalUpscaler* GetFSR2Upscaler() const;
	FFSR2TemporalUpscaler* GetTemporalUpscaler() const;
	float GetResolutionFraction(uint32 Mode) const;
	bool IsPlatformSupported(EShaderPlatform Platform) const;
	void SetEnabledInEditor(bool bEnabled);

private:
	TSharedPtr<FFSR2TemporalUpscaler, ESPMode::ThreadSafe> TemporalUpscaler;
};