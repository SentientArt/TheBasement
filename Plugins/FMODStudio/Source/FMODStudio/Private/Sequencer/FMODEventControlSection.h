// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Curves/KeyHandle.h"
#include "MovieSceneClipboard.h"
#include "MovieSceneSection.h"
#include "Channels/MovieSceneByteChannel.h"
#include "FMODEventControlSection.generated.h"

/** Defines the types of FMOD event control keys. */
UENUM()
enum class EFMODEventControlKey : uint8
{
    Stop = 0,
    Play = 1,
    Pause = 2
};

USTRUCT()
struct FFMODEventControlChannel : public FMovieSceneByteChannel
{
    GENERATED_BODY()

    FFMODEventControlChannel();
};

template <>
struct TStructOpsTypeTraits<FFMODEventControlChannel> : public TStructOpsTypeTraitsBase2<FFMODEventControlChannel>
{
    enum { WithStructuredSerializeFromMismatchedTag = true };
};

template <>
struct TMovieSceneChannelTraits<FFMODEventControlChannel> : TMovieSceneChannelTraitsBase<FFMODEventControlChannel>
{
    enum { SupportsDefaults = false };

#if WITH_EDITOR

    /** Byte channels can have external values (ie, they can get their values from external objects for UI purposes) */
    typedef TMovieSceneExternalValue<uint8> ExtendedEditorDataType;

#endif
};

/** FMOD Event control section */
UCLASS(MinimalAPI)
class UFMODEventControlSection : public UMovieSceneSection
{
    GENERATED_UCLASS_BODY()

public:
    /** Channel containing the event control keys */
    UPROPERTY()
    FFMODEventControlChannel ControlKeys;
};

inline void AssignValue(FFMODEventControlChannel *InChannel, FKeyHandle InKeyHandle, EFMODEventControlKey InValue)
{
    TMovieSceneChannelData<uint8> ChannelData = InChannel->GetData();
    int32 ValueIndex = ChannelData.GetIndex(InKeyHandle);

    if (ValueIndex != INDEX_NONE)
    {
        ChannelData.GetValues()[ValueIndex] = (uint8)InValue;
    }
}

inline bool EvaluateChannel(const FFMODEventControlChannel *InChannel, FFrameTime InTime, EFMODEventControlKey &OutValue)
{
    uint8 RawValue = 0;
    if (InChannel->Evaluate(InTime, RawValue))
    {
        OutValue = (EFMODEventControlKey)RawValue;
        return true;
    }
    return false;
}
