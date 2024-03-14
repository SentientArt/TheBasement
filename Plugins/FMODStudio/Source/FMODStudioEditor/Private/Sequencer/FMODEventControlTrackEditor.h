// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"
#include "Templates/SubclassOf.h"
#include "Curves/KeyHandle.h"
#include "ISequencer.h"
#include "MovieSceneTrack.h"
#include "ISequencerSection.h"
#include "ISequencerTrackEditor.h"
#include "MovieSceneTrackEditor.h"

class FMenuBuilder;
class FSequencerSectionPainter;

/** FMOD Event control track */
class FFMODEventControlTrackEditor : public FMovieSceneTrackEditor
{
public:
    FFMODEventControlTrackEditor(TSharedRef<ISequencer> InSequencer);

    static TSharedRef<ISequencerTrackEditor> CreateTrackEditor(TSharedRef<ISequencer> OwningSequencer);

    void AddControlKey(TArray<FGuid> ObjectGuids);

    // Begin ISequencerTrackEditor interface
    virtual void BuildObjectBindingTrackMenu(FMenuBuilder &MenuBuilder, const TArray<FGuid> &ObjectBindings, const UClass *ObjectClass) override;
    virtual TSharedRef<ISequencerSection> MakeSectionInterface(
        UMovieSceneSection &SectionObject, UMovieSceneTrack &Track, FGuid ObjectBinding) override;
    virtual bool SupportsType(TSubclassOf<UMovieSceneTrack> Type) const override;
    // End ISequencerTrackEditor interface

private:
    /** Delegate for AnimatablePropertyChanged in AddKey. */
    virtual FKeyPropertyResult AddKeyInternal(FFrameNumber KeyTime, UObject *Object);
};

/** Class for event control sections. */
class FFMODEventControlSection : public ISequencerSection, public TSharedFromThis<FFMODEventControlSection>
{
public:
    FFMODEventControlSection(UMovieSceneSection &InSection, TSharedRef<ISequencer> InOwningSequencer);

    // Begin ISequencerSection interface
    virtual UMovieSceneSection *GetSectionObject() override;
    virtual float GetSectionHeight() const override;
    virtual int32 OnPaintSection(FSequencerSectionPainter &InPainter) const override;
    virtual bool SectionIsResizable() const override { return false; }
    // End ISequencerSection interface

private:
    /** The section we are visualizing. */
    UMovieSceneSection &Section;

    /** The sequencer that owns this section */
    TWeakPtr<ISequencer> OwningSequencerPtr;
};
