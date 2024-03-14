// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#include "FMODAmbientSound.h"
#include "FMODEvent.h"
#include "Logging/MessageLog.h"
#include "Misc/UObjectToken.h"
#include "Misc/MapErrors.h"

#define LOCTEXT_NAMESPACE "FMODAmbientSound"

AFMODAmbientSound::AFMODAmbientSound(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
    AudioComponent = ObjectInitializer.CreateDefaultSubobject<UFMODAudioComponent>(this, TEXT("FMODAudioComponent0"));

    AudioComponent->bAutoActivate = true;
    AudioComponent->bStopWhenOwnerDestroyed = true;
    AudioComponent->Mobility = EComponentMobility::Movable;

    RootComponent = AudioComponent;

    bReplicates = false;
    SetHidden(true);
    SetCanBeDamaged(true);
}

#if WITH_EDITOR

void AFMODAmbientSound::CheckForErrors(void)
{
    Super::CheckForErrors();

    if (!AudioComponent)
    {
        FFormatNamedArguments Arguments;
        Arguments.Add(TEXT("ActorName"), FText::FromString(GetName()));
        FMessageLog("MapCheck")
            .Warning()
            ->AddToken(FUObjectToken::Create(this))
            ->AddToken(FTextToken::Create(FText::Format(
                LOCTEXT("MapCheck_Message_AudioComponentNull", "{ActorName} : Ambient sound actor has NULL AudioComponent property - please delete"),
                Arguments)))
            ->AddToken(FMapErrorToken::Create(FMapErrors::AudioComponentNull));
    }
    else if (AudioComponent->Event == NULL)
    {
        FFormatNamedArguments Arguments;
        Arguments.Add(TEXT("ActorName"), FText::FromString(GetName()));
        FMessageLog("MapCheck")
            .Warning()
            ->AddToken(FUObjectToken::Create(this))
            ->AddToken(FTextToken::Create(
                FText::Format(LOCTEXT("MapCheck_Message_EventNull", "{ActorName} : Ambient sound actor has NULL Event property"), Arguments)))
            ->AddToken(FMapErrorToken::Create(FMapErrors::SoundCueNull));
    }
}

bool AFMODAmbientSound::GetReferencedContentObjects(TArray<UObject *> &Objects) const
{
    if (IsValid(AudioComponent) && AudioComponent->Event)
    {
        Objects.Add(AudioComponent->Event);
    }
    return true;
}

#endif

#undef LOCTEXT_NAMESPACE