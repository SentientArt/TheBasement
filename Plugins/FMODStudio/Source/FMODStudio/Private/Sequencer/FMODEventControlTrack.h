// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "MovieSceneNameableTrack.h"
#include "Compilation/IMovieSceneTrackTemplateProducer.h"
#include "FMODEventControlTrack.generated.h"

/** Handles control of an FMOD Event */
UCLASS(MinimalAPI)
class UFMODEventControlTrack : public UMovieSceneNameableTrack, public IMovieSceneTrackTemplateProducer
{
    GENERATED_UCLASS_BODY()

public:
    virtual TArray<UMovieSceneSection *> GetAllControlSections() const { return ControlSections; }

public:
    // Begin UMovieSceneTrack interface
    virtual bool HasSection(const UMovieSceneSection &Section) const override;
    virtual void AddSection(UMovieSceneSection &Section) override;
    virtual void RemoveSection(UMovieSceneSection &Section) override;
    virtual bool IsEmpty() const override;
    virtual const TArray<UMovieSceneSection *> &GetAllSections() const override;
    virtual void AddNewSection(FFrameNumber SectionTime);
    virtual bool SupportsType(TSubclassOf<UMovieSceneSection> SectionClass) const override;
    virtual UMovieSceneSection *CreateNewSection() override;
    // End UMovieSceneTrack interface

    // IMovieSceneTrackTemplateProducer interface
    virtual FMovieSceneEvalTemplatePtr CreateTemplateForSection(const UMovieSceneSection& InSection) const override;

#if WITH_EDITORONLY_DATA
    virtual FText GetDefaultDisplayName() const override;
#endif

private:
    /** List of all event control sections. */
    UPROPERTY()
    TArray<UMovieSceneSection *> ControlSections;
};
