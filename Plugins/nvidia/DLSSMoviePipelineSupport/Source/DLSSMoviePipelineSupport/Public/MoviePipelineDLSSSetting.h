/*
* Copyright (c) 2020 - 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
*
* NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
* property and proprietary rights in and to this material, related
* documentation and any modifications thereto. Any use, reproduction,
* disclosure or distribution of this material and related documentation
* without an express license agreement from NVIDIA CORPORATION or
* its affiliates is strictly prohibited.
*/

#pragma once

#include "MoviePipelineViewFamilySetting.h"
#include "MoviePipelineDLSSSetting.generated.h"

class FSceneViewFamily;

UENUM(BlueprintType)
enum class EMoviePipelineDLSSQuality : uint8
{
	EMoviePipelineDLSSQuality_DLAA					UMETA(DisplayName = "DLAA"),
	EMoviePipelineDLSSQuality_UltraQuality			UMETA(DisplayName = "Ultra Quality"),
	EMoviePipelineDLSSQuality_Quality				UMETA(DisplayName = "Quality"),
	EMoviePipelineDLSSQuality_Balanced				UMETA(DisplayName = "Balanced"),
	EMoviePipelineDLSSQuality_Performance			UMETA(DisplayName = "Performance"),
	EMoviePipelineDLSSQuality_UltraPerformance		UMETA(DisplayName = "Ultra Performance"),
};

UCLASS(BlueprintType)
class DLSSMOVIEPIPELINESUPPORT_API UMoviePipelineDLSSSetting : public UMoviePipelineViewFamilySetting
{
	GENERATED_BODY()
public:
	UMoviePipelineDLSSSetting();

public:
#if WITH_EDITOR
	virtual FText GetDisplayText() const override { return NSLOCTEXT("MovieRenderPipeline", "DlssSettingDisplayName", "DLSS/DLAA"); }
#endif

	/** This method is run right before rendering starts which forwards all required info to DLSS plugin. */
	virtual void SetupViewFamily(FSceneViewFamily& ViewFamily) override;

	/** For the purposes of embedding DLSS quality setting. */
	virtual void GetFormatArguments(FMoviePipelineFormatArgs& InOutFormatArgs) const override;

	virtual void ValidateStateImpl() override;
	
	/** DLSS/DLAA quality setting */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DLSS/DLAA settings", DisplayName = "DLSS Quality")
	EMoviePipelineDLSSQuality DLSSQuality;
};