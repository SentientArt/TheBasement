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
#include "FSR2TemporalUpscalingEditor.h"

#include "CoreMinimal.h"
#include "Interfaces/IPluginManager.h"

IMPLEMENT_MODULE(FFSR2TemporalUpscalingEditorModule, FSR2TemporalUpscalingEditor)

#define LOCTEXT_NAMESPACE "FSR2"

DEFINE_LOG_CATEGORY(LogFSR2Editor);

void FFSR2TemporalUpscalingEditorModule::StartupModule()
{
	TemporalUpscalingModule = FModuleManager::GetModulePtr<IFSR2TemporalUpscalingModule>(TEXT("FSR2TemporalUpscaling"));
	if (TemporalUpscalingModule)
	{
		TemporalUpscalerEditor = MakeShared<FFSR2TemporalUpscalerEditor>(TemporalUpscalingModule);
		UE_LOG(LogFSR2Editor, Log, TEXT("FSR2 Temporal Upscaling Editor Module Started"));
	}
	else
	{
		UE_LOG(LogFSR2Editor, Log, TEXT("FSR2 Temporal Upscaling Editor Module Startup Failed"));
	}
}

void FFSR2TemporalUpscalingEditorModule::ShutdownModule()
{
	UE_LOG(LogFSR2Editor, Log, TEXT("FSR2 Temporal Upscaling Editor Module Shutdown"));
}

#undef LOCTEXT_NAMESPACE