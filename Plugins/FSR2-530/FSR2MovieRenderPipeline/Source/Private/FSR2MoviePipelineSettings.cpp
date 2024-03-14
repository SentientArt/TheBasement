// This file is part of the FidelityFX Super Resolution 2.2 Movie Pipeline Unreal Engine Plugin.
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
#include "FSR2MoviePipelineSettings.h"

#include "FSR2TemporalUpscaling.h"

#include "MovieRenderPipelineDataTypes.h"
#include "SceneView.h"
#include "Templates/SharedPointer.h"

#define LOCTEXT_NAMESPACE "FSR2MoviePipelineSettings"

UFSR2MoviePipelineSettings::UFSR2MoviePipelineSettings()
: FSR2Quality(EFSR2MoviePipelineQuality::Quality)
{

}

void UFSR2MoviePipelineSettings::ValidateStateImpl()
{
    Super::ValidateStateImpl();

	IFSR2TemporalUpscalingModule& FSR2ModuleInterface = FModuleManager::GetModuleChecked<IFSR2TemporalUpscalingModule>(TEXT("FSR2TemporalUpscaling"));
	if (!FSR2ModuleInterface.IsPlatformSupported(GMaxRHIShaderPlatform))
	{	
		ValidationResults.Add(FText::FromString(TEXT("FidelityFX Super Resolution 2.2 is unsupported on this platform.")));
		ValidationState = EMoviePipelineValidationState::Warnings;
	}
}

void UFSR2MoviePipelineSettings::GetFormatArguments(FMoviePipelineFormatArgs& InOutFormatArgs) const
{
    Super::GetFormatArguments(InOutFormatArgs);

	IFSR2TemporalUpscalingModule& FSR2ModuleInterface = FModuleManager::GetModuleChecked<IFSR2TemporalUpscalingModule>(TEXT("FSR2TemporalUpscaling"));
	if (FSR2ModuleInterface.IsPlatformSupported(GMaxRHIShaderPlatform))
	{
		InOutFormatArgs.FileMetadata.Add(TEXT("amd/fidelityFxFsr2QualityMode"), StaticEnum<EFSR2MoviePipelineQuality>()->GetDisplayNameTextByIndex((int32)FSR2Quality).ToString());
		InOutFormatArgs.FilenameArguments.Add(TEXT("fidelityFxFsr2QualityMode"), StaticEnum<EFSR2MoviePipelineQuality>()->GetDisplayNameTextByIndex((int32)FSR2Quality).ToString());
	}
	else
	{
		InOutFormatArgs.FileMetadata.Add(TEXT("amd/fidelityFxFsr2QualityMode"), TEXT("Unsupported"));
		InOutFormatArgs.FilenameArguments.Add(TEXT("fidelityFxFsr2QualityMode"), TEXT("Unsupported"));
	}
}

void UFSR2MoviePipelineSettings::SetupViewFamily(FSceneViewFamily& ViewFamily)
{
	static IConsoleVariable* CVarScreenPercentage = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ScreenPercentage"));
	static IConsoleVariable* CVarFSR2Enabled = IConsoleManager::Get().FindConsoleVariable(TEXT("r.FidelityFX.FSR2.Enabled"));
	IFSR2TemporalUpscalingModule& FSR2ModuleInterface = FModuleManager::GetModuleChecked<IFSR2TemporalUpscalingModule>(TEXT("FSR2TemporalUpscaling"));
	if (ViewFamily.ViewMode == EViewModeIndex::VMI_Lit && CVarFSR2Enabled->GetInt())
	{
		float ScreenPercentage = FSR2ModuleInterface.GetResolutionFraction((uint32)FSR2Quality) * 100.0f;
		if (CVarScreenPercentage)
		{
			CVarScreenPercentage->Set(ScreenPercentage, ECVF_SetByCode);
		}
	}
}
