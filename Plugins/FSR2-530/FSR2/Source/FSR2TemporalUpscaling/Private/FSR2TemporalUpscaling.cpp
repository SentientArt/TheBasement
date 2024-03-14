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
#include "FSR2TemporalUpscaling.h"
#include "FSR2TemporalUpscaler.h"
#include "LogFSR2.h"
#include "FSR2Settings.h"

#include "CoreMinimal.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/ConfigUtilities.h"

IMPLEMENT_MODULE(FFSR2TemporalUpscalingModule, FSR2TemporalUpscaling)

#define LOCTEXT_NAMESPACE "FSR2"

DEFINE_LOG_CATEGORY(LogFSR2);
#if DO_CHECK || DO_GUARD_SLOW || DO_ENSURE
DEFINE_LOG_CATEGORY(LogFSR2API);
#endif // DO_CHECK || DO_GUARD_SLOW || DO_ENSURE

void FFSR2TemporalUpscalingModule::StartupModule()
{
	FString PluginFSR2ShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("FSR2"))->GetBaseDir(), TEXT("Source/ffx-fsr2-api/shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/ThirdParty/FSR2"), PluginFSR2ShaderDir);
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("FSR2"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/FSR2"), PluginShaderDir);
	UE::ConfigUtilities::ApplyCVarSettingsFromIni(TEXT("/Script/FSR2TemporalUpscaling.FSR2Settings"), *GEngineIni, ECVF_SetByProjectSetting);
	UE_LOG(LogFSR2, Log, TEXT("FSR2 Temporal Upscaling Module Started"));
}

void FFSR2TemporalUpscalingModule::ShutdownModule()
{
	UE_LOG(LogFSR2, Log, TEXT("FSR2 Temporal Upscaling Module Shutdown"));
}

void FFSR2TemporalUpscalingModule::SetTemporalUpscaler(TSharedPtr<FFSR2TemporalUpscaler, ESPMode::ThreadSafe> Upscaler)
{
	TemporalUpscaler = Upscaler;
}

FFSR2TemporalUpscaler* FFSR2TemporalUpscalingModule::GetFSR2Upscaler() const
{
	return TemporalUpscaler.Get();
}

FFSR2TemporalUpscaler* FFSR2TemporalUpscalingModule::GetTemporalUpscaler() const
{
	return TemporalUpscaler.Get();
}

float FFSR2TemporalUpscalingModule::GetResolutionFraction(uint32 Mode) const
{
	return TemporalUpscaler->GetResolutionFraction(Mode);
}

bool FFSR2TemporalUpscalingModule::IsPlatformSupported(EShaderPlatform Platform) const
{
	FStaticShaderPlatform ShaderPlatform(Platform);
	
	// All we need is SM5, which can run the RHI backend. Specific backends are handled elsewhere.
	bool bIsSupported = IsFeatureLevelSupported(Platform, ERHIFeatureLevel::SM5);
	
	return bIsSupported;
}

void FFSR2TemporalUpscalingModule::SetEnabledInEditor(bool bEnabled)
{
#if WITH_EDITOR
	return TemporalUpscaler->SetEnabledInEditor(bEnabled);
#endif
}

#undef LOCTEXT_NAMESPACE