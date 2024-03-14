// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#include "FMODAmbientSoundActorFactory.h"
#include "FMODStudioEditorPrivatePCH.h"
#include "FMODAmbientSound.h"
#include "FMODEvent.h"
#include "AssetRegistry/AssetData.h"
#include "Editor/EditorEngine.h"

UFMODAmbientSoundActorFactory::UFMODAmbientSoundActorFactory(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
    DisplayName = NSLOCTEXT("FMOD", "FMODAmbientSoundDisplayName", "FMOD Ambient Sound");
    NewActorClass = AFMODAmbientSound::StaticClass();
}

bool UFMODAmbientSoundActorFactory::CanCreateActorFrom(const FAssetData &AssetData, FText &OutErrorMsg)
{
    //We allow creating AAmbientSounds without an existing sound asset
    if (UActorFactory::CanCreateActorFrom(AssetData, OutErrorMsg))
    {
        return true;
    }

    if (AssetData.IsValid() && !AssetData.GetClass()->IsChildOf(UFMODEvent::StaticClass()))
    {
        OutErrorMsg = NSLOCTEXT("FMOD", "CanCreateActorFrom_NoFMODEventAsset", "A valid FMOD Event asset must be specified.");
        return false;
    }

    return true;
}

void UFMODAmbientSoundActorFactory::PostSpawnActor(UObject *Asset, AActor *NewActor)
{
    UFMODEvent *Event = Cast<UFMODEvent>(Asset);

    if (Event != NULL)
    {
        AFMODAmbientSound *NewSound = CastChecked<AFMODAmbientSound>(NewActor);
        FActorLabelUtilities::SetActorLabelUnique(NewSound, Event->GetName());
        NewSound->AudioComponent->Event = Event;
    }
}

UObject *UFMODAmbientSoundActorFactory::GetAssetFromActorInstance(AActor *Instance)
{
    check(Instance->IsA(NewActorClass));
    AFMODAmbientSound *SoundActor = CastChecked<AFMODAmbientSound>(Instance);

    check(SoundActor->AudioComponent);
    return SoundActor->AudioComponent->Event;
}

void UFMODAmbientSoundActorFactory::PostCreateBlueprint(UObject *Asset, AActor *CDO)
{
    if (Asset != NULL && CDO != NULL)
    {
        UFMODEvent *Event = Cast<UFMODEvent>(Asset);

        if (Event != NULL)
        {
            AFMODAmbientSound *NewSound = CastChecked<AFMODAmbientSound>(CDO);
            NewSound->AudioComponent->Event = Event;
        }
    }
}