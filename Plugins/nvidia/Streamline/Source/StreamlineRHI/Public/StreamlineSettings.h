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

#include "Engine/DeveloperSettings.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

#include "StreamlineSettings.generated.h"

UENUM()
enum class EStreamlineSettingOverride : uint8
{
	Enabled UMETA(DisplayName = "True"),
	Disabled UMETA(DisplayName = "False"),
	UseProjectSettings UMETA(DisplayName = "Use project settings"),
};

UCLASS(Config = Engine, ProjectUserConfig)
class STREAMLINERHI_API UStreamlineOverrideSettings : public UObject
{
	GENERATED_BODY()

public:

	/** Enable DLSS Frame Generation in play in editor viewports. Saved to local user config only. */
	UPROPERTY(Config, EditAnywhere, Category = "Level Editor - Viewport (Local)", DisplayName = "Enable DLSS-FG in Play In Editor viewports")
	EStreamlineSettingOverride EnableDLSSFGInPlayInEditorViewportsOverride = EStreamlineSettingOverride::UseProjectSettings;

	/**
	 * Load the Streamline debug overlay in non-Shipping configurations. Note that the overlay requires DLSS Frame Generation to be available.
	 * Modifying this setting requires an editor restart to take effect. Saved to local user config only
	 */
	UPROPERTY(Config, EditAnywhere, Category = "General Settings", DisplayName = "Load Debug Overlay")
	EStreamlineSettingOverride LoadDebugOverlayOverride = EStreamlineSettingOverride::UseProjectSettings;
};

UCLASS(Config = Engine, DefaultConfig, DisplayName="NVIDIA DLSS Frame Generation")
class STREAMLINERHI_API UStreamlineSettings: public UObject
{
	GENERATED_BODY()

public:

	/** Enable plugin features for D3D12, if the driver supports it at runtime */
	UPROPERTY(Config, EditAnywhere, Category = "Platforms", DisplayName = "Enable plugin features for the D3D12RHI")
	bool bEnableStreamlineD3D12 = PLATFORM_WINDOWS;

	/** Enable plugin features for D3D11, if the driver supports it at runtime */
	UPROPERTY(Config, EditAnywhere, Category = "Platforms", DisplayName = "Enable plugin features for the D3D11RHI (Reflex only)")
	bool bEnableStreamlineD3D11 = PLATFORM_WINDOWS;

	/** Enable DLSS Frame Generation in play in editor viewports. This project wide setting can be locally overridden in the NVIDIA DLSS Frame Generation (Local) settings.*/
	UPROPERTY(Config, EditAnywhere, Category = "Level Editor - Viewport", DisplayName = "Enable DLSS-FG in Play In Editor viewports")
	bool bEnableDLSSFGInPlayInEditorViewports = true;

	/**
	 * Load the Streamline debug overlay in non-Shipping configurations. Note that the overlay requires DLSS Frame Generation to be available.
	 * This project wide setting can be locally overridden in the NVIDIA DLSS Frame Generation (Local) settings.
	 * Modifying this setting requires an editor restart to take effect
	 */
	UPROPERTY(Config, EditAnywhere, Category = "General Settings", DisplayName = "Load Debug Overlay")
	bool bLoadDebugOverlay = false;

	/** Allow OTA updates of Streamline features */
	UPROPERTY(Config, EditAnywhere, Category = "General Settings", DisplayName = "Allow OTA update")
	bool bAllowOTAUpdate = true;

	/** By default the DLSS Frame Generation plugin uses the UE Project ID to initialize Streamline. In some cases NVIDIA might provide a separate NVIDIA Application ID, which should be put here. */
	UPROPERTY(Config, EditAnywhere, Category = "General Settings", DisplayName = "NVIDIA NGX Application ID", AdvancedDisplay)
	int32 NVIDIANGXApplicationId = 0;
};

