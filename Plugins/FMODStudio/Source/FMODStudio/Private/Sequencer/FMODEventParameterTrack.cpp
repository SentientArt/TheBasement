// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#include "FMODEventParameterTrack.h"
#include "FMODEventParameterSectionTemplate.h"
#include "IMovieScenePlayer.h"
#include "MovieSceneCommonHelpers.h"

#define LOCTEXT_NAMESPACE "FMODEventParameterTrack"

UFMODEventParameterTrack::UFMODEventParameterTrack(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
#if WITH_EDITORONLY_DATA
    TrackTint = FColor(0, 170, 255, 65);
#endif
}

FMovieSceneEvalTemplatePtr UFMODEventParameterTrack::CreateTemplateForSection(const UMovieSceneSection &InSection) const
{
    return FFMODEventParameterSectionTemplate(*CastChecked<UMovieSceneParameterSection>(&InSection), *this);
}

UMovieSceneSection *UFMODEventParameterTrack::CreateNewSection()
{
    return NewObject<UMovieSceneParameterSection>(this, UMovieSceneParameterSection::StaticClass(), NAME_None, RF_Transactional);
}

void UFMODEventParameterTrack::RemoveAllAnimationData()
{
    Sections.Empty();
}

bool UFMODEventParameterTrack::HasSection(const UMovieSceneSection &Section) const
{
    return Sections.Contains(&Section);
}

void UFMODEventParameterTrack::AddSection(UMovieSceneSection &Section)
{
    Sections.Add(&Section);
}

void UFMODEventParameterTrack::RemoveSection(UMovieSceneSection &Section)
{
    Sections.Remove(&Section);
}

bool UFMODEventParameterTrack::IsEmpty() const
{
    return Sections.Num() == 0;
}

const TArray<UMovieSceneSection *> &UFMODEventParameterTrack::GetAllSections() const
{
    return Sections;
}

#if WITH_EDITORONLY_DATA
FText UFMODEventParameterTrack::GetDefaultDisplayName() const
{
    return LOCTEXT("DisplayName", "FMOD Event Parameter");
}
#endif

void UFMODEventParameterTrack::AddParameterKey(FName ParameterName, FFrameNumber Time, float Value)
{
    UMovieSceneParameterSection *NearestSection = Cast<UMovieSceneParameterSection>(MovieSceneHelpers::FindNearestSectionAtTime(Sections, Time));
    if (NearestSection == nullptr)
    {
        NearestSection = Cast<UMovieSceneParameterSection>(CreateNewSection());
        NearestSection->SetRange(TRange<FFrameNumber>::Inclusive(Time, Time));
        Sections.Add(NearestSection);
    }
    NearestSection->AddScalarParameterKey(ParameterName, Time, Value);
}

#undef LOCTEXT_NAMESPACE
