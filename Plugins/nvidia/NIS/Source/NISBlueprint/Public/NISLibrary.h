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

#include "Modules/ModuleManager.h"

#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Misc/CoreDelegates.h"

#include "NISLibrary.generated.h"

class FNISUpscaler;
class FDelegateHandle;


UENUM(BlueprintType)
enum class UNISSupport : uint8
{
	Supported UMETA(DisplayName = "Supported"),
	NotSupported UMETA(DisplayName = "Not Supported due to insufficient RHI Feature Level"),
};


UENUM(BlueprintType)
enum class UNISMode : uint8
{
	Off              UMETA(DisplayName = "Off"),
	UltraQuality     UMETA(DisplayName = "Ultra Quality"),
	Quality          UMETA(DisplayName = "Quality"),
	Balanced         UMETA(DisplayName = "Balanced"),
	Performance      UMETA(DisplayName = "Performance"),
	Custom           UMETA(DisplayName = "Custom")
};

UCLASS(MinimalAPI)
class  UNISLibrary : public UBlueprintFunctionLibrary
{
	friend class FNISBlueprintModule;
	GENERATED_BODY()
public:

	/** Checks whether NIS is supported by the current GPU. Further details can be retrieved via QueryNISSupport*/
	UFUNCTION(BlueprintPure, Category = "NIS", meta = (DisplayName = "Is NVIDIA NIS Supported"))
	static NISBLUEPRINT_API bool IsNISSupported();

	/** Checks whether a NIS mode is supported */
	UFUNCTION(BlueprintPure, Category = "NIS", meta = (DisplayName = "Is NIS Mode Supported"))
	static NISBLUEPRINT_API bool IsNISModeSupported(UNISMode NISMode);

	/** Retrieves all supported NIS modes. Can be used to populate UI */
	UFUNCTION(BlueprintPure, Category = "NIS", meta = (DisplayName = "Get Supported NIS Modes"))
	static NISBLUEPRINT_API TArray<UNISMode> GetSupportedNISModes();

	/** Returns the recommended screen percentage for a given NIS mode. Returns CustomScreenPercentage if NISMode is UNISMode::Custom */
	UFUNCTION(BlueprintPure, Category = "NIS", meta = (DisplayName = "Get NIS Recommended Screen Percentage"))
	static NISBLUEPRINT_API float GetNISRecommendedScreenPercentage(UNISMode NISMode);

	/** The global screen percentage range that NIS supports. */
	UFUNCTION(BlueprintPure, Category = "NIS", meta = (DisplayName = "Get NIS Screen Percentage Range"))
	static NISBLUEPRINT_API void GetNISScreenPercentageRange(float& MinScreenPercentage, float& MaxScreenPercentage);

	/** Sets the console variables to enable/disable NIS (r.NIS.Enable, r.NIS.Upscaling, r.ScreenPercentage, r.TemporalAA.Upsampling, r.TemporalAA.Upscaler)*/
	UFUNCTION(BlueprintCallable, Category = "NIS", meta = (DisplayName = "Set NIS Mode"))
	static NISBLUEPRINT_API void SetNISMode(UNISMode NISMode);

	/** Set the screen percentage used for Custom mode (100% by default) */
	UFUNCTION(BlueprintCallable, Category = "NIS", meta = (DisplayName = "Set NIS Custom Screen Percentage"))
	static NISBLUEPRINT_API void SetNISCustomScreenPercentage(float CustomScreenPercentage = 100.0f);

	/* Sets the console variables to enable additional NIS sharpening. Set to 0 to disable (r.NGX.NIS.Sharpness) */
	UFUNCTION(BlueprintCallable, Category = "NIS", meta = (DisplayName = "Set NIS Sharpness"))
	static NISBLUEPRINT_API void SetNISSharpness(float Sharpness);
	
	///* Find a reasonable default NIS mode based on current hardware */
	UFUNCTION(BlueprintPure, Category = "NIS", meta = (DisplayName = "Get Default NIS Mode"))
	static NISBLUEPRINT_API UNISMode GetDefaultNISMode();


private:
	static UNISSupport NISSupport;
	static FNISUpscaler* NISUpscaler;
	static float SavedCustomScreenPercentage;
	static bool bIsCustomMode;
};

class FNISBlueprintModule final : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
};
