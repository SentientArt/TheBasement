// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#include "FMODEventControlTrackEditor.h"
#include "FMODAmbientSound.h"
#include "Sequencer/FMODEventControlSection.h"
#include "Sequencer/FMODEventControlTrack.h"
#include "AnimatedRange.h"
#include "Rendering/DrawElements.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Curves/IntegralCurve.h"
#include "SequencerSectionPainter.h"
#include "EditorStyleSet.h"
#include "Editor/UnrealEdEngine.h"
#include "ISectionLayoutBuilder.h"
#include "CommonMovieSceneTools.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneChannelEditorData.h"

#define LOCTEXT_NAMESPACE "FFMODEventControlTrackEditor"

FFMODEventControlSection::FFMODEventControlSection(UMovieSceneSection &InSection, TSharedRef<ISequencer> InOwningSequencer)
    : Section(InSection)
    , OwningSequencerPtr(InOwningSequencer)
{
}

UMovieSceneSection *FFMODEventControlSection::GetSectionObject()
{
    return &Section;
}

float FFMODEventControlSection::GetSectionHeight() const
{
    static const float SectionHeight = 20.f;
    return SectionHeight;
}

int32 FFMODEventControlSection::OnPaintSection(FSequencerSectionPainter &InPainter) const
{
    TSharedPtr<ISequencer> OwningSequencer = OwningSequencerPtr.Pin();

    if (!OwningSequencer.IsValid())
    {
        return InPainter.LayerId + 1;
    }

    const ESlateDrawEffect DrawEffects = InPainter.bParentEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;
    const FTimeToPixel &TimeToPixelConverter = InPainter.GetTimeConverter();

    FLinearColor TrackColor;

    // TODO: Set / clip stop time based on event length
    UFMODEventControlSection *ControlSection = Cast<UFMODEventControlSection>(&Section);
    if (IsValid(ControlSection))
    {
        UFMODEventControlTrack *ParentTrack = Cast<UFMODEventControlTrack>(ControlSection->GetOuter());
        if (IsValid(ParentTrack))
        {
            TrackColor = ParentTrack->GetColorTint();
        }
    }

    // TODO: This should only draw the visible ranges.
    TArray<TRange<float>> DrawRanges;
    TOptional<float> CurrentRangeStart;

    if (ControlSection != nullptr)
    {
        TMovieSceneChannelData<const uint8> ChannelData = ControlSection->ControlKeys.GetData();
        TArrayView<const FFrameNumber> Times = ChannelData.GetTimes();
        TArrayView<const uint8> Values = ChannelData.GetValues();

        for (int32 Index = 0; Index < Times.Num(); ++Index)
        {
            const double Time = Times[Index] / TimeToPixelConverter.GetTickResolution();
            const EFMODEventControlKey Value = (EFMODEventControlKey)Values[Index];

            if (Value == EFMODEventControlKey::Play)
            {
                if (CurrentRangeStart.IsSet() == false)
                {
                    CurrentRangeStart = Time;
                }
            }
            if (Value == EFMODEventControlKey::Stop)
            {
                if (CurrentRangeStart.IsSet())
                {
                    DrawRanges.Add(TRange<float>(CurrentRangeStart.GetValue(), Time));
                    CurrentRangeStart.Reset();
                }
            }
        }
    }

    if (CurrentRangeStart.IsSet())
    {
        DrawRanges.Add(TRange<float>(CurrentRangeStart.GetValue(), OwningSequencer->GetViewRange().GetUpperBoundValue()));
    }

    for (const TRange<float> &DrawRange : DrawRanges)
    {
        float XOffset = TimeToPixelConverter.SecondsToPixel(DrawRange.GetLowerBoundValue());
        float XSize = TimeToPixelConverter.SecondsToPixel(DrawRange.GetUpperBoundValue()) - XOffset;
        FSlateDrawElement::MakeBox(InPainter.DrawElements, InPainter.LayerId,
            InPainter.SectionGeometry.ToPaintGeometry(
                FVector2D(XSize, SequencerSectionConstants::KeySize.Y),
                FSlateLayoutTransform(1.0f, FVector2D(XOffset, (InPainter.SectionGeometry.GetLocalSize().Y - SequencerSectionConstants::KeySize.Y) / 2))),
            FAppStyle::GetBrush("Sequencer.Section.Background"), DrawEffects);
        FSlateDrawElement::MakeBox(InPainter.DrawElements, InPainter.LayerId,
            InPainter.SectionGeometry.ToPaintGeometry(
                FVector2D(XSize, SequencerSectionConstants::KeySize.Y),
                FSlateLayoutTransform(1.0f, FVector2D(XOffset, (InPainter.SectionGeometry.GetLocalSize().Y - SequencerSectionConstants::KeySize.Y) / 2))),
            FAppStyle::GetBrush("Sequencer.Section.BackgroundTint"), DrawEffects, TrackColor);
    }

    return InPainter.LayerId + 1;
}

FFMODEventControlTrackEditor::FFMODEventControlTrackEditor(TSharedRef<ISequencer> InSequencer)
    : FMovieSceneTrackEditor(InSequencer)
{
}

TSharedRef<ISequencerTrackEditor> FFMODEventControlTrackEditor::CreateTrackEditor(TSharedRef<ISequencer> InSequencer)
{
    return MakeShareable(new FFMODEventControlTrackEditor(InSequencer));
}

bool FFMODEventControlTrackEditor::SupportsType(TSubclassOf<UMovieSceneTrack> Type) const
{
    return Type == UFMODEventControlTrack::StaticClass();
}

TSharedRef<ISequencerSection> FFMODEventControlTrackEditor::MakeSectionInterface(
    UMovieSceneSection &SectionObject, UMovieSceneTrack &Track, FGuid ObjectBinding)
{
    check(SupportsType(SectionObject.GetOuter()->GetClass()));
    const TSharedPtr<ISequencer> OwningSequencer = GetSequencer();
    return MakeShareable(new FFMODEventControlSection(SectionObject, OwningSequencer.ToSharedRef()));
}

void FFMODEventControlTrackEditor::BuildObjectBindingTrackMenu(FMenuBuilder &MenuBuilder, const TArray<FGuid> &ObjectBindings, const UClass *ObjectClass)
{
    if (ObjectClass->IsChildOf(AFMODAmbientSound::StaticClass()) || ObjectClass->IsChildOf(UFMODAudioComponent::StaticClass()))
    {
        const TSharedPtr<ISequencer> ParentSequencer = GetSequencer();

        MenuBuilder.AddMenuEntry(LOCTEXT("AddFMODEventControlTrack", "FMOD Event Control Track"),
            LOCTEXT("FMODEventControlTooltip", "Adds a track for controlling FMOD event."), FSlateIcon(),
            FUIAction(FExecuteAction::CreateSP(this, &FFMODEventControlTrackEditor::AddControlKey, ObjectBindings)));
    }
}

void FFMODEventControlTrackEditor::AddControlKey(TArray<FGuid> ObjectGuids)
{
    TSharedPtr<ISequencer> SequencerPtr = GetSequencer();
    for (FGuid ObjectGuid : ObjectGuids)
    {
        UObject *Object = SequencerPtr.IsValid() ? SequencerPtr->FindSpawnedObjectOrTemplate(ObjectGuid) : nullptr;

        if (Object)
        {
            AnimatablePropertyChanged(FOnKeyProperty::CreateRaw(this, &FFMODEventControlTrackEditor::AddKeyInternal, Object));
        }
    }
}

FKeyPropertyResult FFMODEventControlTrackEditor::AddKeyInternal(FFrameNumber KeyTime, UObject *Object)
{
    FKeyPropertyResult KeyPropertyResult;

    FFindOrCreateHandleResult HandleResult = FindOrCreateHandleToObject(Object);
    FGuid ObjectHandle = HandleResult.Handle;
    KeyPropertyResult.bHandleCreated |= HandleResult.bWasCreated;

    if (ObjectHandle.IsValid())
    {
        FFindOrCreateTrackResult TrackResult = FindOrCreateTrackForObject(ObjectHandle, UFMODEventControlTrack::StaticClass());
        UMovieSceneTrack *Track = TrackResult.Track;
        KeyPropertyResult.bTrackCreated |= TrackResult.bWasCreated;

        if (KeyPropertyResult.bTrackCreated && ensure(Track))
        {
            UFMODEventControlTrack *EventTrack = Cast<UFMODEventControlTrack>(Track);
            EventTrack->AddNewSection(KeyTime);
            EventTrack->SetDisplayName(LOCTEXT("TrackName", "FMOD Event"));
            KeyPropertyResult.bTrackModified = true;
        }
    }

    return KeyPropertyResult;
}

#undef LOCTEXT_NAMESPACE
