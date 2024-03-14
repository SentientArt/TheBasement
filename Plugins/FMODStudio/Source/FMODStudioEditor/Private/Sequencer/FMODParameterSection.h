// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#pragma once

#include "ISequencerSection.h"

class UMovieSceneSection;

/** A movie scene section for Event parameters. */
class FFMODParameterSection : public FSequencerSection
{
public:
    FFMODParameterSection(UMovieSceneSection &InSectionObject)
        : FSequencerSection(InSectionObject)
    {
    }

    // Begin ISequencerSection interface
    virtual bool RequestDeleteKeyArea(const TArray<FName> &KeyAreaNamePath) override;
    // End ISequencerSection interface
};