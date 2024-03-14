// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#pragma once

#include "FMODAsset.h"
#include "Sound/ReverbEffect.h"
#include "FMODSnapshotReverb.generated.h"

/**
 * FMOD Event Asset.
 */
UCLASS()
class FMODSTUDIO_API UFMODSnapshotReverb : public UReverbEffect
{
    GENERATED_UCLASS_BODY()

public:
    /** The unique Guid, which matches the one exported from FMOD Studio */
    UPROPERTY()
    FGuid AssetGuid;

private:
    /** Non default instances of UFMODAsset are assets */
    virtual bool IsAsset() const override;

#if WITH_EDITORONLY_DATA
    virtual void PostEditChangeProperty(FPropertyChangedEvent &PropertyChangedEvent) override;
#endif // EDITORONLY_DATA
};
