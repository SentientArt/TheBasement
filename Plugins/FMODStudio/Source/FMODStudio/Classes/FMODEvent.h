// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#pragma once

#include "FMODAsset.h"
#include "FMODEvent.generated.h"

struct FMOD_STUDIO_PARAMETER_DESCRIPTION;

/**
 * FMOD Event Asset.
 */
UCLASS()
class FMODSTUDIO_API UFMODEvent : public UFMODAsset
{
    GENERATED_UCLASS_BODY()

public:
    /** Get parameter descriptions for this event */
    void GetParameterDescriptions(TArray<FMOD_STUDIO_PARAMETER_DESCRIPTION> &Parameters) const;

private:
    /** Get tags to show in content view */
    virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag> &OutTags) const override;

    /** Descriptive name */
    virtual FString GetDesc() override;
};
