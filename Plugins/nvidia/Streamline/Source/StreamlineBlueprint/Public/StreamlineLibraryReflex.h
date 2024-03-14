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

#include "StreamlineLibrary.h"


#include "StreamlineLibraryReflex.generated.h"


#ifdef __INTELLISENSE__
#define WITH_STREAMLINE 1
#endif

UENUM(BlueprintType)
enum class UStreamlineReflexMode : uint8
{
	Disabled = 0 UMETA(DisplayName = "Disabled"),
	Enabled = 1 UMETA(DisplayName = "Enabled"),
	EnabledPlusBoost = 3 UMETA(DisplayName = "Enabled + Boost")
};



UCLASS(MinimalAPI)
class UStreamlineLibraryReflex : public UBlueprintFunctionLibrary
{
public:
	GENERATED_BODY()


	UFUNCTION(BlueprintPure, Category = "Streamline|Reflex", meta = (DisplayName = "Is NVIDIA Reflex Supported"))
	static STREAMLINEBLUEPRINT_API bool IsReflexSupported();

	UFUNCTION(BlueprintPure, Category = "Streamline|Reflex", meta = (DisplayName = "Query NVIDIA Reflex Support"))
	static STREAMLINEBLUEPRINT_API UStreamlineFeatureSupport QueryReflexSupport();


	UFUNCTION(BlueprintCallable, Category = "Streamline|Reflex", meta = (DisplayName = "Set Reflex mode"))
	static STREAMLINEBLUEPRINT_API void SetReflexMode(const UStreamlineReflexMode Mode);
	
	UFUNCTION(BlueprintPure, Category = "Streamline|Reflex", meta = (DisplayName = "Get Reflex mode"))
	static STREAMLINEBLUEPRINT_API UStreamlineReflexMode GetReflexMode();


	UFUNCTION(BlueprintPure, Category = "Streamline|Reflex", meta = (DisplayName = "Get default Reflex mode"))
	static STREAMLINEBLUEPRINT_API UStreamlineReflexMode GetDefaultReflexMode();

	UFUNCTION(BlueprintPure, Category = "Streamline|Reflex", meta = (DisplayName = "Get Reflex Game To Render Latency (ms)"))
	static STREAMLINEBLUEPRINT_API float GetGameToRenderLatencyInMs();
	UFUNCTION(BlueprintPure, Category = "Streamline|Reflex", meta = (DisplayName = "Get Reflex Game Latency (ms)"))
	static STREAMLINEBLUEPRINT_API float GetGameLatencyInMs();
	UFUNCTION(BlueprintPure, Category = "Streamline|Reflex", meta = (DisplayName = "Get Reflex Render Latency (ms)"))
	static STREAMLINEBLUEPRINT_API float GetRenderLatencyInMs();


	static void Startup();
	static void Shutdown();
};
