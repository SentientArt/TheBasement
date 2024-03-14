// Taken from ParticleParameterTrackEditor

#pragma once
#include "CoreMinimal.h"
#include "Misc/Guid.h"
#include "Templates/SubclassOf.h"
#include "Widgets/SWidget.h"
#include "ISequencer.h"
#include "MovieSceneTrack.h"
#include "ISequencerSection.h"
#include "Framework/Commands/UIAction.h"
#include "ISequencerTrackEditor.h"
#include "MovieSceneTrackEditor.h"

class FMenuBuilder;
class UFMODEventParameterTrack;
class UFMODAudioComponent;
struct FMOD_STUDIO_PARAMETER_DESCRIPTION;

/**
 * Track editor for material parameters.
 */
class FFMODEventParameterTrackEditor : public FMovieSceneTrackEditor
{
public:
    /** Constructor. */
    FFMODEventParameterTrackEditor(TSharedRef<ISequencer> InSequencer);

    /** Virtual destructor. */
    virtual ~FFMODEventParameterTrackEditor() {}

    /**
    * Creates an instance of this class.  Called by a sequencer.
    *
    * @param OwningSequencer The sequencer instance to be used by this tool.
    * @return The new instance of this class.
    */
    static TSharedRef<ISequencerTrackEditor> CreateTrackEditor(TSharedRef<ISequencer> OwningSequencer);

    // ISequencerTrackEditor interface

    virtual TSharedPtr<SWidget> BuildOutlinerEditWidget(
        const FGuid &ObjectBinding, UMovieSceneTrack *Track, const FBuildEditWidgetParams &Params) override;
    virtual TSharedRef<ISequencerSection> MakeSectionInterface(
        UMovieSceneSection &SectionObject, UMovieSceneTrack &Track, FGuid ObjectBinding) override;
    virtual bool SupportsType(TSubclassOf<UMovieSceneTrack> Type) const override;

private:
    static FName TrackName;

    // Struct used for building the parameter menu.
    struct FParameterNameAndAction
    {
        FName ParameterName;
        FUIAction Action;

        FParameterNameAndAction(FName InParameterName, FUIAction InAction)
        {
            ParameterName = InParameterName;
            Action = InAction;
        }

        bool operator<(FParameterNameAndAction const &Other) const { return ParameterName.FastLess(Other.ParameterName); }
    };

    void BuildObjectBindingTrackMenu(FMenuBuilder &MenuBuilder, const TArray<FGuid> &ObjectBindings, const UClass *ObjectClass);

    /** Provides the contents of the add parameter menu. */
    TSharedRef<SWidget> OnGetAddParameterMenuContent(FGuid ObjectBinding, UFMODEventParameterTrack *EventParameterTrack);
    TSharedRef<SWidget> BuildParameterMenu(FGuid ObjectBinding, UFMODEventParameterTrack *EventParameterTrack, UFMODAudioComponent *AudioComponent);

    bool CanAddEventParameterTrack(FGuid ObjectBinding);
    void AddEventParameterTrack(FGuid ObjectBinding);

    /** Adds an event parameter and initial key to a track. */
    void AddParameter(FGuid ObjectBinding, UFMODEventParameterTrack *EventParameterTrack, FName ParameterName);
};
