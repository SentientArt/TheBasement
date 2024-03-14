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

#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "Engine/DeveloperSettings.h"

#include "FSR2Settings.generated.h"

//-------------------------------------------------------------------------------------
// The official FSR2 quality modes.
//-------------------------------------------------------------------------------------
UENUM()
enum class EFSR2QualityMode : int32
{
	Unused UMETA(Hidden),
	Quality UMETA(DisplayName = "Quality"),
	Balanced UMETA(DisplayName = "Balanced"),
	Performance UMETA(DisplayName = "Performance"),
	UltraPerformance UMETA(DisplayName = "Ultra Performance"),
};

//-------------------------------------------------------------------------------------
// The support texture formats for the FSR2 history data.
//-------------------------------------------------------------------------------------
UENUM()
enum class EFSR2HistoryFormat : int32
{
	FloatRGBA UMETA(DisplayName = "PF_FloatRGBA"),
	FloatR11G11B10 UMETA(DisplayName = "PF_FloatR11G11B10"),
};

//-------------------------------------------------------------------------------------
// The support texture formats for the FSR2 history data.
//-------------------------------------------------------------------------------------
UENUM()
enum class EFSR2DeDitherMode : int32
{
	Off UMETA(DisplayName = "Off"),
	Full UMETA(DisplayName = "Full"),
	HairOnly UMETA(DisplayName = "Hair Only"),
};

//-------------------------------------------------------------------------------------
// The modes for forcing Landscape Hierachical Instance Static Model to not be Static.
//-------------------------------------------------------------------------------------
UENUM()
enum class EFSR2LandscapeHISMMode : int32
{
	Off UMETA(DisplayName = "Off"),
	AllStatic UMETA(DisplayName = "All Instances"),
	StaticWPO UMETA(DisplayName = "Instances with World-Position-Offset"),
};

//-------------------------------------------------------------------------------------
// Settings for FSR2 exposed through the Editor UI.
//-------------------------------------------------------------------------------------
UCLASS(Config = Engine, DefaultConfig, DisplayName = "FidelityFX Super Resolution 2.2")
class FSR2TEMPORALUPSCALING_API UFSR2Settings : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()
public:
	virtual FName GetContainerName() const override;
	virtual FName GetCategoryName() const override;
	virtual FName GetSectionName() const override;

	virtual void PostInitProperties() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	UPROPERTY(Config, EditAnywhere, Category = "General Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.Enabled", DisplayName = "Enabled"))
		bool bEnabled;

	UPROPERTY(Config, EditAnywhere, Category = "General Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.AutoExposure", DisplayName = "Auto Exposure", ToolTip = "Enable to use FSR2's own auto-exposure, otherwise the engine's auto-exposure value is used."))
		bool AutoExposure;

	UPROPERTY(Config, EditAnywhere, Category = "General Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.EnabledInEditorViewport", DisplayName = "Enabled in Editor Viewport", ToolTip = "When enabled use FSR2 by default in the Editor viewports."))
		bool EnabledInEditorViewport;

	UPROPERTY(Config, EditAnywhere, Category = "General Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.UseSSRExperimentalDenoiser", DisplayName = "Use SSR Experimental Denoiser", ToolTip = "Set to 1 to use r.SSR.ExperimentalDenoiser when FSR2 is enabled. This is required when r.FidelityFX.FSR2.CreateReactiveMask is enabled as the FSR2 plugin sets r.SSR.ExperimentalDenoiser to 1 in order to capture reflection data to generate the reactive mask."))
		bool UseSSRExperimentalDenoiser;

	UPROPERTY(Config, EditAnywhere, Category = "Backend Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.UseNativeDX12", DisplayName = "Native DX12", ToolTip = "Whather to use the optimised native DX12 backend for FSR2 rather than the default RHI backend.", ConfigRestartRequired = true))
		bool UseNativeDX12;

	UPROPERTY(Config, EditAnywhere, Category = "Backend Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.UseNativeVulkan", DisplayName = "Native Vulkan", ToolTip = "Whather to use the optimised native Vulkan backend for FSR2 rather than the default RHI backend.", ConfigRestartRequired = true))
		bool UseNativeVulkan;

	UPROPERTY(Config, EditAnywhere, Category = "Quality Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.QualityMode", DisplayName = "Quality Mode", ToolTip = "Selects the default quality mode to be used when upscaling with FSR2."))
		EFSR2QualityMode QualityMode;

	UPROPERTY(Config, EditAnywhere, Category = "Quality Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.HistoryFormat", DisplayName = "History Format", ToolTip = "Selects the bit-depth for the FSR2 history texture format, defaults to PF_FloatRGBA but can be set to PF_FloatR11G11B10 to reduce bandwidth at the expense of quality."))
		EFSR2HistoryFormat HistoryFormat;

	UPROPERTY(Config, EditAnywhere, Category = "Quality Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.DeDither", DisplayName = "De-Dither Rendering", ToolTip = "Enable an extra pass to de-dither rendering before handing over to FSR2 to avoid over-thinning, defaults to Off but can be set to Full for all pixels or to Hair Only for just around Hair (requires Deffered Renderer)."))
		EFSR2DeDitherMode DeDither;

	UPROPERTY(Config, EditAnywhere, Category = "Quality Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.Sharpness", DisplayName = "Sharpness", ClampMin = 0, ClampMax = 1, ToolTip = "When greater than 0.0 this enables Robust Contrast Adaptive Sharpening Filter to sharpen the output image."))
		float Sharpness;

	UPROPERTY(Config, EditAnywhere, Category = "Quality Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.AdjustMipBias", DisplayName = "Adjust Mip Bias & Offset", ToolTip = "Applies negative MipBias to material textures, improving results."))
		bool AdjustMipBias;

	UPROPERTY(Config, EditAnywhere, Category = "Quality Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.CreateReactiveMask", DisplayName = "Reactive Mask", ToolTip = "Enable to generate a mask from the SceneColor, GBuffer, SeparateTranslucency & ScreenspaceReflections that determines how reactive each pixel should be."))
		bool ReactiveMask;

	UPROPERTY(Config, EditAnywhere, Category = "Quality Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.ForceVertexDeformationOutputsVelocity", DisplayName = "Force Vertex Deformation To Output Velocity", ToolTip = "Force enables materials with World Position Offset and/or World Displacement to output velocities during velocity pass even when the actor has not moved."))
		bool ForceVertexDeformationOutputsVelocity;

	UPROPERTY(Config, EditAnywhere, Category = "Quality Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.ForceLandscapeHISMMobility", DisplayName = "Force Landscape HISM Mobility", ToolTip = "Allow FSR2 to force the mobility of Landscape actors Hierarchical Instance Static Mesh components that use World-Position-Offset materials so they render valid velocities.\nSetting 'All Instances' is faster on the CPU, 'Instances with World-Position-Offset' is faster on the GPU."))
		EFSR2LandscapeHISMMode ForceLandscapeHISMMobility;

	UPROPERTY(Config, EditAnywhere, Category = "Reactive Mask Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.ReactiveMaskReflectionScale", DisplayName = "Reflection Scale", ClampMin = 0, ClampMax = 1, ToolTip = "Scales the Unreal engine reflection contribution to the reactive mask, which can be used to control the amount of aliasing on reflective surfaces."))
		float ReflectionScale;

	UPROPERTY(Config, EditAnywhere, Category = "Reactive Mask Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.ReactiveMaskReflectionLumaBias", DisplayName = "Reflection Luminance Bias", ClampMin = 0, ClampMax = 1, ToolTip = "Biases the reactive mask by the luminance of the reflection. Use to balance aliasing against ghosting on brightly lit reflective surfaces."))
		float ReflectionLuminanceBias;

	UPROPERTY(Config, EditAnywhere, Category = "Reactive Mask Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.ReactiveMaskRoughnessScale", DisplayName = "Roughness Scale", ClampMin = 0, ClampMax = 1, ToolTip = "Scales the GBuffer roughness to provide a fallback value for the reactive mask when screenspace & planar reflections are disabled or don't affect a pixel."))
		float RoughnessScale;

	UPROPERTY(Config, EditAnywhere, Category = "Reactive Mask Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.ReactiveMaskRoughnessBias", DisplayName = "Roughness Bias", ClampMin = 0, ClampMax = 1, ToolTip = "Biases the reactive mask value when screenspace/planar reflections are weak with the GBuffer roughness to account for reflection environment captures."))
		float RoughnessBias;

	UPROPERTY(Config, EditAnywhere, Category = "Reactive Mask Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.ReactiveMaskRoughnessMaxDistance", DisplayName = "Roughness Max Distance", ClampMin = 0, ToolTip = "Maximum distance in world units for using material roughness to contribute to the reactive mask, the maximum of this value and View.FurthestReflectionCaptureDistance will be used."))
		float RoughnessMaxDistance;

	UPROPERTY(Config, EditAnywhere, Category = "Reactive Mask Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.ReactiveMaskRoughnessForceMaxDistance", DisplayName = "Force Roughness Max Distance", ToolTip = "Enable to force the maximum distance in world units for using material roughness to contribute to the reactive mask rather than using View.FurthestReflectionCaptureDistance."))
		bool ReactiveMaskRoughnessForceMaxDistance;

	UPROPERTY(Config, EditAnywhere, Category = "Reactive Mask Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.ReactiveMaskTranslucencyBias", DisplayName = "Translucency Bias", ClampMin = 0, ClampMax = 1, ToolTip = "Scales how much contribution translucency makes to the reactive mask. Higher values will make translucent materials less reactive which can reduce smearing."))
		float TranslucencyBias;

	UPROPERTY(Config, EditAnywhere, Category = "Reactive Mask Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.ReactiveMaskTranslucencyLumaBias", DisplayName = "Translucency Luminance Bias", ClampMin = 0, ClampMax = 1, ToolTip = "Biases the translucency contribution to the reactive mask by the luminance of the transparency."))
		float TranslucencyLuminanceBias;

	UPROPERTY(Config, EditAnywhere, Category = "Reactive Mask Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.ReactiveMaskTranslucencyMaxDistance", DisplayName = "Translucency Max Distance", ClampMin = 0, ToolTip = "Maximum distance in world units for using translucency to contribute to the reactive mask. This is another way to remove sky-boxes and other back-planes from the reactive mask, at the expense of nearer translucency not being reactive."))
		float TranslucencyMaxDistance;

	UPROPERTY(Config, EditAnywhere, Category = "Reactive Mask Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.ReactiveMaskReactiveShadingModelID", DisplayName = "Reactive Shading Model", ToolTip = "Treat the specified shading model as reactive, taking the CustomData0.x value as the reactive value to write into the mask. Default is MSM_NUM (Off)."))
		TEnumAsByte<enum EMaterialShadingModel> ReactiveShadingModelID;

	UPROPERTY(Config, EditAnywhere, Category = "Reactive Mask Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.ReactiveMaskForceReactiveMaterialValue", DisplayName = "Force value for Reactive Shading Model", ClampMin = 0, ClampMax = 1, ToolTip = "Force the reactive mask value for Reactive Shading Model materials, when > 0 this value can be used to override the value supplied in the Material Graph."))
		float ForceReactiveMaterialValue;

	UPROPERTY(Config, EditAnywhere, Category = "Reactive Mask Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.ReactiveHistoryTranslucencyBias", DisplayName = "Translucency Bias", ClampMin = 0, ClampMax = 1, ToolTip = "Scales how much contribution translucency makes to suppress history via the reactive mask. Higher values will make translucent materials less reactive which can reduce smearing."))
		float ReactiveHistoryTranslucencyBias;

	UPROPERTY(Config, EditAnywhere, Category = "Reactive Mask Settings", meta = (ConsoleVariable = "r.FidelityFX.FSR2.ReactiveHistoryTranslucencyLumaBias", DisplayName = "Translucency Luminance Bias", ClampMin = 0, ClampMax = 1, ToolTip = "Biases the translucency contribution to suppress history via the reactive mask by the luminance of the transparency."))
		float ReactiveHistoryTranslucencyLumaBias;
};