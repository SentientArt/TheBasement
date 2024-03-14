// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#pragma once

#include "FMODAudioComponent.h"
#include "FMODAmbientSound.generated.h"

/** FMOD Ambient Sound.
*/
UCLASS(AutoExpandCategories = Audio, ClassGroup = Sounds, hidecategories(Collision, Input))
class FMODSTUDIO_API AFMODAmbientSound : public AActor
{
    GENERATED_UCLASS_BODY()

public:
    /** The Audio component for this actor */
    UPROPERTY(Category = Sound, VisibleAnywhere, BlueprintReadOnly, meta = (ExposeFunctionCategories = "Sound"))
    UFMODAudioComponent *AudioComponent;

// Begin AActor interface.
#if WITH_EDITOR
    virtual void CheckForErrors() override;
    virtual bool GetReferencedContentObjects(TArray<UObject *> &Objects) const override;
#endif
// End AActor interface.
};
