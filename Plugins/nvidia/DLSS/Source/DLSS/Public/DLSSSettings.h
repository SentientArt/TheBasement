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

#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "Engine/DeveloperSettings.h"
#include "SceneView.h"

#include "DLSSSettings.generated.h"

UENUM()
enum class EDLSSSettingOverride : uint8
{
	Enabled UMETA(DisplayName = "True"),
	Disabled UMETA(DisplayName = "False"),
	UseProjectSettings UMETA(DisplayName = "Use project settings"),
};

UENUM()
enum class EDLSSPreset : uint8
{
	Default=0 UMETA(ToolTip = "default behavior, preset specified per DLSS SDK release"),
	A=1 UMETA(ToolTip = "force preset A"),
	B=2 UMETA(ToolTip = "force preset B"),
	C=3 UMETA(ToolTip = "force preset C"),
	D=4 UMETA(ToolTip = "force preset D"),
	E=5 UMETA(ToolTip = "force preset E"),
	F=6 UMETA(ToolTip = "force preset F"),
	G=7 UMETA(ToolTip = "force preset G"),
};

UCLASS(Config = Engine, ProjectUserConfig)
class DLSS_API UDLSSOverrideSettings : public UObject
{
public:

	GENERATED_BODY()
	
	/** This enables DLSS/DLAA in editor viewports. Saved to local user config only.*/
	UPROPERTY(Config, EditAnywhere, Category = "Level Editor - Viewport (Local)", DisplayName = "Enable DLSS/DLAA to be turned on in Editor viewports")
	EDLSSSettingOverride EnableDLSSInEditorViewportsOverride = EDLSSSettingOverride::UseProjectSettings;

	/** This enables DLSS/DLAA in play in editor viewports. Saved to local user config only. */
	UPROPERTY(Config, EditAnywhere, Category = "Level Editor - Viewport (Local)", DisplayName = "Enable DLSS/DLAA in Play In Editor viewports")
	EDLSSSettingOverride EnableDLSSInPlayInEditorViewportsOverride = EDLSSSettingOverride::UseProjectSettings;

	/** This enables warnings about plugins & tools that are incompatible with DLSS/DLAA in the editor. This setting and the project setting both must be set to get warnings */
	UPROPERTY(Config, EditAnywhere, Category = "Editor (Local)", DisplayName = "Warn about incompatible plugins and tools")
	bool bShowDLSSIncompatiblePluginsToolsWarnings = true;

	/** This enables on screen warnings and errors about DLSS/DLAA  */
	UPROPERTY(Config, EditAnywhere, Category = "Editor (Local)", DisplayName = "Show various DLSS/DLAA on screen debug messages")
	EDLSSSettingOverride ShowDLSSSDebugOnScreenMessages = EDLSSSettingOverride::UseProjectSettings;

};

UCLASS(Config = Engine, DefaultConfig, DisplayName="NVIDIA DLSS")
class DLSS_API UDLSSSettings: public UObject
{
	GENERATED_BODY()

private:

public:

	/** Enable DLSS/DLAA for D3D12, if the driver supports it at runtime */
	UPROPERTY(Config, EditAnywhere, Category = "Platforms", DisplayName = "Enable DLSS/DLAA for the D3D12RHI")
		bool bEnableDLSSD3D12 = PLATFORM_WINDOWS;

	/** Enable DLSS/DLAA for D3D11, if the driver supports it at runtime */
	UPROPERTY(Config, EditAnywhere, Category = "Platforms", DisplayName = "Enable DLSS/DLAA for the D3D11RHI")
		bool bEnableDLSSD3D11 = PLATFORM_WINDOWS;

	/** Enable DLSS/DLAA for Vulkan, if the driver supports it at runtime */
	UPROPERTY(Config, EditAnywhere, Category = "Platforms", DisplayName = "Enable DLSS/DLAA for the VulkanRHI")
		bool bEnableDLSSVulkan = PLATFORM_WINDOWS;

	/** This enables DLSS/DLAA in editor viewports. This project wide setting can be locally overridden in the NVIDIA DLSS (Local) settings.*/
	UPROPERTY(Config, EditAnywhere, Category = "Level Editor - Viewport", DisplayName = "Enable DLSS/DLAA to be turned on in Editor viewports")
		bool bEnableDLSSInEditorViewports = false;

	/** This enables DLSS/DLAA in play in editor viewports. This project wide setting can be locally overridden in in the NVIDIA DLSS (Local) settings.*/
	UPROPERTY(Config, EditAnywhere, Category = "Level Editor - Viewport", DisplayName = "Enable DLSS/DLAA in Play In Editor viewports")
		bool bEnableDLSSInPlayInEditorViewports = true;

	/** This enables on screen warnings and errors about DLSS/DLAA. This project wide setting can be locally overridden in the NVIDIA DLSS (Local) settings. */
	UPROPERTY(Config, EditAnywhere, Category = "Level Editor - Viewport", DisplayName = "Show various DLSS/DLAA on screen debug messages")
		bool bShowDLSSSDebugOnScreenMessages = true;

	/** This is part of the DLSS plugin and used by most projects*/
	UPROPERTY(VisibleAnywhere, Config, Category = "General Settings", DisplayName = "Generic DLSS Binary Path")
		FString GenericDLSSBinaryPath;

	UPROPERTY(VisibleAnywhere, Config, Category = "General Settings", DisplayName = "Exists")
		bool bGenericDLSSBinaryExists;

	/** By default the DLSS plugin uses the UE Project ID to initialize DLSS. In some cases NVIDIA might provide a separate NVIDIA Application ID, which should be put here. Please refer to https://developer.nvidia.com/dlss for details*/
	UPROPERTY(Config, EditAnywhere, Category = "General Settings", DisplayName = "NVIDIA NGX Application ID", AdvancedDisplay)
		uint32 NVIDIANGXApplicationId;

	/** In some cases NVIDIA might provide a project specific DLSS binary for your project. Please refer to https://developer.nvidia.com/dlss for details*/
	UPROPERTY(VisibleAnywhere, Config, Category = "General Settings", DisplayName = "Custom DLSS Binary Path", AdvancedDisplay)
		FString CustomDLSSBinaryPath;
	UPROPERTY(VisibleAnywhere, Config, Category = "General Settings", DisplayName = "Exists", AdvancedDisplay)
		bool bCustomDLSSBinaryExists;

	/** Allow OTA updates of DLSS models */
	UPROPERTY(Config, EditAnywhere, Category = "General Settings", DisplayName = "Allow OTA update")
		bool bAllowOTAUpdate = true;

	/** This enables warnings about plugins & tools that are incompatible with DLSS/DLAA in the editor. This setting and the local setting both must be set to get warnings */
	UPROPERTY(Config, EditAnywhere, Category = "Editor", DisplayName = "Warn about incompatible plugins and tools")
	bool bShowDLSSIncompatiblePluginsToolsWarnings = true;

	/** DLAA preset setting. Allows selecting a different DL model than the default */
	UPROPERTY(Config, EditAnywhere, Category = "General Settings", DisplayName = "DLAA Preset", AdvancedDisplay)
		EDLSSPreset DLAAPreset = EDLSSPreset::Default;

	/** DLSS quality mode preset setting. Allows selecting a different DL model than the default */
	// NOT IMPLEMENTED YET UPROPERTY(Config, EditAnywhere, Category = "General Settings", DisplayName = "DLSS Ultra Quality Preset", AdvancedDisplay)
		EDLSSPreset DLSSUltraQualityPreset = EDLSSPreset::Default;

	/** DLSS quality mode preset setting. Allows selecting a different DL model than the default */
	UPROPERTY(Config, EditAnywhere, Category = "General Settings", DisplayName = "DLSS Quality Preset", AdvancedDisplay)
		EDLSSPreset DLSSQualityPreset = EDLSSPreset::Default;

	/** DLSS balanced mode preset setting. Allows selecting a different DL model than the default */
	UPROPERTY(Config, EditAnywhere, Category = "General Settings", DisplayName = "DLSS Balanced Preset", AdvancedDisplay)
		EDLSSPreset DLSSBalancedPreset = EDLSSPreset::Default;

	/** DLSS performance mode preset setting. Allows selecting a different DL model than the default */
	UPROPERTY(Config, EditAnywhere, Category = "General Settings", DisplayName = "DLSS Performance Preset", AdvancedDisplay)
		EDLSSPreset DLSSPerformancePreset = EDLSSPreset::Default;

	/** DLSS ultra performance mode preset setting. Allows selecting a different DL model than the default */
	UPROPERTY(Config, EditAnywhere, Category = "General Settings", DisplayName = "DLSS Ultra Performance Preset", AdvancedDisplay)
		EDLSSPreset DLSSUltraPerformancePreset = EDLSSPreset::Default;
};

