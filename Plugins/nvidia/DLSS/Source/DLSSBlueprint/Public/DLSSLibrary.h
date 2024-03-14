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

#include "Modules/ModuleManager.h"

#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Misc/CoreDelegates.h"

#include "DLSSLibrary.generated.h"

class FDLSSUpscaler;
class FDelegateHandle;

#ifndef WITH_DLSS
#define WITH_DLSS 0
#endif

UENUM(BlueprintType)
enum class UDLSSSupport : uint8
{
	Supported UMETA(DisplayName = "Supported"),

	NotSupported UMETA(DisplayName = "Not Supported"),
	NotSupportedIncompatibleHardware UMETA(DisplayName = "Incompatible Hardware", ToolTip = "DLSS requires an NVIDIA RTX GPU"),
	NotSupportedDriverOutOfDate UMETA(DisplayName = "Driver Out of Date", ToolTip = "The driver is outdated. Also see GetDLSSMinimumDriverVersion"),
	NotSupportedOperatingSystemOutOfDate UMETA(DisplayName = "Operating System Out of Date", ToolTip = "DLSS requires at least Windows 10 Fall 2017 Creators Update 64-bit, (v1709, build 16299)"),
	NotSupportedByPlatformAtBuildTime UMETA(DisplayName = "Platform Not Supported At Build Time", ToolTip = "This platform doesn't not support DLSS at build time. Currently DLSS is only supported on Windows 64"),
	NotSupportedIncompatibleAPICaptureToolActive UMETA(DisplayName = "Incompatible API Capture Tool Active", ToolTip = "DLSS is not compatible with an active API capture tool such as RenderDoc.")
};


UENUM(BlueprintType)
enum class UDLSSMode : uint8
{
	Off              UMETA(DisplayName = "Off"),
	Auto             UMETA(DisplayName = "Auto", ToolTip = "Not a real quality mode. Use Auto to query best settings for a given resolution with GetDLSSModeInformation"),
	DLAA             UMETA(DisplayName = "DLAA"),
	UltraQuality     UMETA(DisplayName = "Ultra Quality"),
	Quality          UMETA(DisplayName = "Quality"),
	Balanced         UMETA(DisplayName = "Balanced"),
	Performance      UMETA(DisplayName = "Performance"),
	UltraPerformance UMETA(DisplayName = "Ultra Performance")
};

UCLASS(MinimalAPI)
class  UDLSSLibrary : public UBlueprintFunctionLibrary
{
	friend class FDLSSBlueprintModule;
	GENERATED_BODY()
public:

	/** Check whether NVIDIA DLSS Super Resolution/DLAA is supported. Further details can be retrieved via 'Query DLSS-SR Support' */
	UFUNCTION(BlueprintPure, Category = "DLSS", meta = (DisplayName = "Is DLSS-SR Supported"))
	static DLSSBLUEPRINT_API bool IsDLSSSupported();

	/** Check whether NVIDIA DLSS Ray Reconstruction is supported. Further details can be retrieved via 'Query DLSS-RR Support' */
	UFUNCTION(BlueprintPure, Category = "DLSS", meta = (DisplayName = "Is DLSS-RR Supported"))
	static DLSSBLUEPRINT_API bool IsDLSSRRSupported();

	/** Check whether NVIDIA DLSS Super Resolution/DLAA is supported */
	UFUNCTION(BlueprintPure, Category = "DLSS", meta = (DisplayName = "Query DLSS-SR Support"))
	static DLSSBLUEPRINT_API UDLSSSupport QueryDLSSSupport();

	/** Check whether NVIDIA DLSS Ray Reconstruction is supported */
	UFUNCTION(BlueprintPure, Category = "DLSS", meta = (DisplayName = "Query DLSS-RR Support"))
	static DLSSBLUEPRINT_API UDLSSSupport QueryDLSSRRSupport();

	/** If QueryDLSSSupport returns "NotSupportedDriverOutOfDate", then MinDriverVersionMajor and MinDriverVersionMinor contains the required driver version.*/
	UFUNCTION(BlueprintPure, Category = "DLSS", meta = (DisplayName = "Get DLSS-SR Minimum DriverVersion"))
	static DLSSBLUEPRINT_API void GetDLSSMinimumDriverVersion(int32& MinDriverVersionMajor, int32& MinDriverVersionMinor);

	/** If QueryDLSSRRSupport returns "NotSupportedDriverOutOfDate", then MinDriverVersionMajor and MinDriverVersionMinor contains the required driver version.*/
	UFUNCTION(BlueprintPure, Category = "DLSS", meta = (DisplayName = "Get DLSS-RR Minimum DriverVersion"))
	static DLSSBLUEPRINT_API void GetDLSSRRMinimumDriverVersion(int32& MinDriverVersionMajor, int32& MinDriverVersionMinor);

	/**
	 * Enable/disable DLSS Super Resolution/DLAA
	 * To select a DLSS-SR quality mode, set an appropriate upscale screen percentage with r.ScreenPercentage. Use GetDlssModeInformation to find optimal screen percentage
	 * To select DLAA, set the upscale screen percentage to 100 (r.ScreenPercentage=100)
	 */
	UFUNCTION(BlueprintCallable, Category = "DLSS", meta = (DisplayName = "Enable DLSS-SR"))
	static DLSSBLUEPRINT_API void EnableDLSS(bool bEnabled);

	/** Check whether DLSS Super Resolution/DLAA is enabled */
	UFUNCTION(BlueprintPure, Category = "DLSS", meta = (DisplayName = "Is DLSS-SR Enabled"))
	static DLSSBLUEPRINT_API bool IsDLSSEnabled();

	/** (Unsupported in this release) Enable/disable DLSS Ray Reconstruction */
	UFUNCTION(BlueprintCallable, Category = "DLSS", meta = (DisplayName = "Enable DLSS-RR"))
	static DLSSBLUEPRINT_API void EnableDLSSRR(bool bEnabled);

	/** Check whether DLSS Ray Reconstruction is enabled */
	UFUNCTION(BlueprintPure, Category = "DLSS", meta = (DisplayName = "Is DLSS-RR Enabled"))
	static DLSSBLUEPRINT_API bool IsDLSSRREnabled();

	/** Enable/disable DLAA. Note that while DLAA is enabled, DLSS will be automatically disabled */
	UFUNCTION(BlueprintCallable, Category = "DLSS", meta = (DisplayName = "Enable DLAA", DeprecatedFunction, DeprecationMessage = "Use 'Enable DLSS-SR' instead"))
	static DLSSBLUEPRINT_API void EnableDLAA(bool bEnabled);

	/** Check whether DLAA is enabled */
	UFUNCTION(BlueprintPure, Category = "DLSS", meta = (DisplayName = "Is DLAA Enabled", DeprecatedFunction, DeprecationMessage = "Use 'Is DLSS-SR Enabled' instead"))
	static DLSSBLUEPRINT_API bool IsDLAAEnabled();

	/** Check whether a DLSS mode is supported */
	UFUNCTION(BlueprintPure, Category = "DLSS", meta = (DisplayName = "Is DLSS-SR Mode Supported"))
	static DLSSBLUEPRINT_API bool IsDLSSModeSupported(UDLSSMode DLSSMode);

	/** Retrieve all supported DLSS modes. Can be used to populate UI */
	UFUNCTION(BlueprintPure, Category = "DLSS", meta = (DisplayName = "Get Supported DLSS-SR Modes"))
	static DLSSBLUEPRINT_API TArray<UDLSSMode> GetSupportedDLSSModes();

	/** Provide additional details (such as screen percentage ranges) about a DLSS mode. Screen Resolution is required for Auto mode */
	UFUNCTION(BlueprintPure, Category = "DLSS", meta = (DisplayName = "Get DLSS-SR Mode Information"))
	static DLSSBLUEPRINT_API void GetDLSSModeInformation(UDLSSMode DLSSMode, FVector2D ScreenResolution, bool& bIsSupported, float& OptimalScreenPercentage, bool& bIsFixedScreenPercentage, float& MinScreenPercentage, float& MaxScreenPercentage, float& OptimalSharpness);

	/** The global screen percentage range that DLSS supports. Excludes DLSS modes with fixed screen percentage. Also see GetDLSSModeInformation*/
	UFUNCTION(BlueprintPure, Category = "DLSS", meta = (DisplayName = "Get DLSS-SR Screenpercentage Range"))
	static DLSSBLUEPRINT_API void GetDLSSScreenPercentageRange(float& MinScreenPercentage, float& MaxScreenPercentage);

	/** Enable/disable DLSS */
	UFUNCTION(BlueprintCallable, Category = "DLSS", meta=(WorldContext="WorldContextObject", DisplayName = "Set DLSS Mode", DeprecatedFunction, DeprecationMessage = "Use 'Enable DLSS-SR' instead"))
	static DLSSBLUEPRINT_API void SetDLSSMode(UObject* WorldContextObject, UDLSSMode DLSSMode);

	/* Read the current DLSS mode */
	UFUNCTION(BlueprintPure, Category = "DLSS", meta = (DisplayName = "Get DLSS Mode", DeprecatedFunction, DeprecationMessage = "Use 'Is DLSS-SR Enabled' instead"))
	static DLSSBLUEPRINT_API UDLSSMode GetDLSSMode();

	/* Set the console variables to enable additional DLSS sharpening. Set to 0 to disable (r.NGX.DLSS.Sharpness) */
	UFUNCTION(BlueprintCallable, Category = "DLSS", meta = (DisplayName = "Set DLSS Sharpness", DeprecatedFunction, DeprecationMessage = "Use NIS sharpening instead"))
	static DLSSBLUEPRINT_API void SetDLSSSharpness(float Sharpness);
	
	/* Read the console variables to infer the current DLSS sharpness (r.NGX.DLSS.Sharpness) */
	UFUNCTION(BlueprintPure, Category = "DLSS", meta = (DisplayName = "Get DLSS Sharpness", DeprecatedFunction, DeprecationMessage = "Use NIS sharpening instead"))
	static DLSSBLUEPRINT_API float GetDLSSSharpness();

	/* Find a reasonable default DLSS mode based on current hardware */
	UFUNCTION(BlueprintPure, Category = "DLSS", meta = (DisplayName = "Get Default DLSS Mode"))
	static DLSSBLUEPRINT_API UDLSSMode GetDefaultDLSSMode();

private:
	static UDLSSSupport DLSSSRSupport;
	static UDLSSSupport DLSSRRSupport;

#if WITH_DLSS
	static int32 MinDLSSSRDriverVersionMinor;
	static int32 MinDLSSSRDriverVersionMajor;
	static int32 MinDLSSRRDriverVersionMinor;
	static int32 MinDLSSRRDriverVersionMajor;
	static FDLSSUpscaler* DLSSUpscaler;
	static bool bDLSSLibraryInitialized;

	static UDLSSMode CurrentDLSSModeDeprecated;
	static bool bDLAAEnabledDeprecated;

	static bool TryInitDLSSLibrary();

	static int32 PreviousShadowDenoiser;
	static int32 PreviousLumenSSR;
	static int32 PreviousLumenTemporal;
	static bool bDenoisingRequested;

#if !UE_BUILD_SHIPPING
	struct FDLSSErrorState
	{
		bool bIsDLSSModeUnsupported = false;
		UDLSSMode InvalidDLSSMode = UDLSSMode::Off;
	};

	static FDLSSErrorState DLSSErrorState;

	static void GetDLSSOnScreenMessages(TMultiMap<FCoreDelegates::EOnScreenMessageSeverity, FText>& OutMessages);
	static FDelegateHandle DLSSOnScreenMessagesDelegateHandle;
#endif

#endif
};

class FDLSSBlueprintModule final : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
};
