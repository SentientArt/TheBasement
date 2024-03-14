// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#include "FMODEventParameterTrackEditor.h"
#include "FMODAmbientSound.h"
#include "FMODEvent.h"
#include "FMODStudioModule.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Sequencer/FMODEventParameterTrack.h"
#include "Sections/MovieSceneParameterSection.h"
#include "Sequencer/FMODParameterSection.h"
#include "SequencerUtilities.h"
#include "fmod_studio.hpp"

#define LOCTEXT_NAMESPACE "FMODEeventParameterTrackEditor"

FName FFMODEventParameterTrackEditor::TrackName("FMODEventParameter");

FFMODEventParameterTrackEditor::FFMODEventParameterTrackEditor(TSharedRef<ISequencer> InSequencer)
    : FMovieSceneTrackEditor(InSequencer)
{
}

TSharedRef<ISequencerTrackEditor> FFMODEventParameterTrackEditor::CreateTrackEditor(TSharedRef<ISequencer> OwningSequencer)
{
    return MakeShareable(new FFMODEventParameterTrackEditor(OwningSequencer));
}

TSharedRef<ISequencerSection> FFMODEventParameterTrackEditor::MakeSectionInterface(
    UMovieSceneSection &SectionObject, UMovieSceneTrack &Track, FGuid ObjectBinding)
{
    UMovieSceneParameterSection *ParameterSection = Cast<UMovieSceneParameterSection>(&SectionObject);
    checkf(ParameterSection != nullptr, TEXT("Unsupported section type."));
    return MakeShareable(new FFMODParameterSection(*ParameterSection));
}

TSharedPtr<SWidget> FFMODEventParameterTrackEditor::BuildOutlinerEditWidget(
    const FGuid &ObjectBinding, UMovieSceneTrack *Track, const FBuildEditWidgetParams &Params)
{
    UFMODEventParameterTrack *EventParameterTrack = Cast<UFMODEventParameterTrack>(Track);

    // Create a container edit box
    return FSequencerUtilities::MakeAddButton(LOCTEXT("ParameterText", "Parameter"),
        FOnGetContent::CreateSP(this, &FFMODEventParameterTrackEditor::OnGetAddParameterMenuContent, ObjectBinding, EventParameterTrack),
        Params.NodeIsHovered,
        GetSequencer());
}

void FFMODEventParameterTrackEditor::BuildObjectBindingTrackMenu(FMenuBuilder &MenuBuilder, const TArray<FGuid> &ObjectBindings, const UClass *ObjectClass)
{
    if (ObjectClass->IsChildOf(AFMODAmbientSound::StaticClass()) || ObjectClass->IsChildOf(UFMODAudioComponent::StaticClass()))
    {
        const TSharedPtr<ISequencer> ParentSequencer = GetSequencer();

        MenuBuilder.AddMenuEntry(LOCTEXT("AddFMODParameterTrack", "FMOD Event Parameter Track"),
            LOCTEXT("AddFMODParameterTrackTooltip", "Adds a track for controlling FMOD event parameter values."), FSlateIcon(),
            FUIAction(FExecuteAction::CreateSP(this, &FFMODEventParameterTrackEditor::AddEventParameterTrack, ObjectBindings[0]),
                FCanExecuteAction::CreateSP(this, &FFMODEventParameterTrackEditor::CanAddEventParameterTrack, ObjectBindings[0])));
    }
}

bool FFMODEventParameterTrackEditor::SupportsType(TSubclassOf<UMovieSceneTrack> Type) const
{
    return Type == UFMODEventParameterTrack::StaticClass();
}

TSharedRef<SWidget> FFMODEventParameterTrackEditor::OnGetAddParameterMenuContent(FGuid ObjectBinding, UFMODEventParameterTrack *EventParameterTrack)
{
    TSharedPtr<ISequencer> SequencerPtr = GetSequencer();
    AFMODAmbientSound *Sound = SequencerPtr.IsValid() ? Cast<AFMODAmbientSound>(SequencerPtr->FindSpawnedObjectOrTemplate(ObjectBinding)) : nullptr;
    UFMODAudioComponent *AudioComponent;

    if (IsValid(Sound))
    {
        AudioComponent = Sound->AudioComponent;
    }
    else
    {
        AudioComponent = SequencerPtr.IsValid() ? Cast<UFMODAudioComponent>(SequencerPtr->FindSpawnedObjectOrTemplate(ObjectBinding)) : nullptr;
    }
    return BuildParameterMenu(ObjectBinding, EventParameterTrack, AudioComponent);
}

TSharedRef<SWidget> FFMODEventParameterTrackEditor::BuildParameterMenu(
    FGuid ObjectBinding, UFMODEventParameterTrack *EventParameterTrack, UFMODAudioComponent *AudioComponent)
{
    FMenuBuilder AddParameterMenuBuilder(true, nullptr);

    if (IsValid(AudioComponent) && AudioComponent->Event)
    {
        TArray<FParameterNameAndAction> ParameterNamesAndActions;
        TArray<FMOD_STUDIO_PARAMETER_DESCRIPTION> ParameterDescriptions;
        AudioComponent->Event->GetParameterDescriptions(ParameterDescriptions);

        for (FMOD_STUDIO_PARAMETER_DESCRIPTION &ParameterDescription : ParameterDescriptions)
        {
            FName ParameterName(ParameterDescription.name);
            FExecuteAction InitAction =
                FExecuteAction::CreateSP(this, &FFMODEventParameterTrackEditor::AddParameter, ObjectBinding, EventParameterTrack, ParameterName);
            FUIAction AddParameterMenuAction(InitAction);
            FParameterNameAndAction NameAndAction(ParameterName, AddParameterMenuAction);
            ParameterNamesAndActions.Add(NameAndAction);
        }

        // Sort and generate menu.
        ParameterNamesAndActions.Sort();
        for (FParameterNameAndAction NameAndAction : ParameterNamesAndActions)
        {
            AddParameterMenuBuilder.AddMenuEntry(FText::FromName(NameAndAction.ParameterName), FText(), FSlateIcon(), NameAndAction.Action);
        }
    }
    return AddParameterMenuBuilder.MakeWidget();
}

bool FFMODEventParameterTrackEditor::CanAddEventParameterTrack(FGuid ObjectBinding)
{
    return GetSequencer()->GetFocusedMovieSceneSequence()->GetMovieScene()->FindTrack(
               UFMODEventParameterTrack::StaticClass(), ObjectBinding, TrackName) == nullptr;
}

void FFMODEventParameterTrackEditor::AddEventParameterTrack(FGuid ObjectBinding)
{
    FindOrCreateTrackForObject(ObjectBinding, UFMODEventParameterTrack::StaticClass(), TrackName, true);
    GetSequencer()->NotifyMovieSceneDataChanged(EMovieSceneDataChangeType::MovieSceneStructureItemAdded);
}

void FFMODEventParameterTrackEditor::AddParameter(FGuid ObjectBinding, UFMODEventParameterTrack *EventParameterTrack, FName ParameterName)
{
    UMovieSceneSequence *MovieSceneSequence = GetMovieSceneSequence();
    FFrameNumber KeyTime = GetTimeForKey();

    for (TWeakObjectPtr<> Object : GetSequencer()->FindObjectsInCurrentSequence(ObjectBinding))
    {
        AFMODAmbientSound *Sound = Cast<AFMODAmbientSound>(Object.Get());
        UFMODAudioComponent *AudioComponent = nullptr;

        if (IsValid(Sound))
        {
            AudioComponent = Sound->AudioComponent;
        }
        else
        {
            AudioComponent = Cast<UFMODAudioComponent>(Object.Get());
        }

        if (IsValid(AudioComponent))
        {
            float Value = AudioComponent->GetParameter(ParameterName);
            const FScopedTransaction Transaction(LOCTEXT("AddEventParameter", "Add event parameter"));
            EventParameterTrack->Modify();
            EventParameterTrack->AddParameterKey(ParameterName, KeyTime, Value);
        }
    }
    GetSequencer()->NotifyMovieSceneDataChanged(EMovieSceneDataChangeType::MovieSceneStructureItemAdded);
}

#undef LOCTEXT_NAMESPACE
