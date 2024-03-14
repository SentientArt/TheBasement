// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#include "FMODEventControlTrack.h"
#include "FMODEventControlSection.h"
#include "IMovieScenePlayer.h"
#include "FMODEventControlSectionTemplate.h"
#include "MovieSceneCommonHelpers.h"

#define LOCTEXT_NAMESPACE "FMODEventControlTrack"

UFMODEventControlTrack::UFMODEventControlTrack(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
    TrackTint = FColor(255, 255, 255, 160);
#endif
}

const TArray<UMovieSceneSection *> &UFMODEventControlTrack::GetAllSections() const
{
    return ControlSections;
}

bool UFMODEventControlTrack::HasSection(const UMovieSceneSection &Section) const
{
    return ControlSections.Contains(&Section);
}

void UFMODEventControlTrack::AddSection(UMovieSceneSection &Section)
{
    ControlSections.Add(&Section);
}

void UFMODEventControlTrack::RemoveSection(UMovieSceneSection &Section)
{
    ControlSections.Remove(&Section);
}

bool UFMODEventControlTrack::IsEmpty() const
{
    return ControlSections.Num() == 0;
}

void UFMODEventControlTrack::AddNewSection(FFrameNumber SectionTime)
{
    if (MovieSceneHelpers::FindSectionAtTime(ControlSections, SectionTime) == nullptr)
    {
        UFMODEventControlSection *NewSection = Cast<UFMODEventControlSection>(CreateNewSection());
        ControlSections.Add(NewSection);
    }
}

bool UFMODEventControlTrack::SupportsType(TSubclassOf<UMovieSceneSection> SectionClass) const
{
    return SectionClass == UFMODEventControlSection::StaticClass();
}

UMovieSceneSection *UFMODEventControlTrack::CreateNewSection()
{
    return NewObject<UFMODEventControlSection>(this);
}

FMovieSceneEvalTemplatePtr UFMODEventControlTrack::CreateTemplateForSection(const UMovieSceneSection& InSection) const
{
    return FFMODEventControlSectionTemplate(*CastChecked<const UFMODEventControlSection>(&InSection));
}

#if WITH_EDITORONLY_DATA
FText UFMODEventControlTrack::GetDefaultDisplayName() const
{
    return LOCTEXT("DisplayName", "FMOD Event");
}
#endif

#undef LOCTEXT_NAMESPACE
