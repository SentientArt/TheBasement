// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#pragma once

#include "ComponentAssetBroker.h"
#include "FMODEvent.h"

//////////////////////////////////////////////////////////////////////////
// FFMODAssetBroker

class FFMODAssetBroker : public IComponentAssetBroker
{
public:
    UClass *GetSupportedAssetClass() override { return UFMODEvent::StaticClass(); }

    virtual bool AssignAssetToComponent(UActorComponent *InComponent, UObject *InAsset) override
    {
        if (UFMODAudioComponent *AudioComp = Cast<UFMODAudioComponent>(InComponent))
        {
            UFMODEvent *Event = Cast<UFMODEvent>(InAsset);

            if ((Event != NULL) || (InAsset == NULL))
            {
                AudioComp->Event = Event;
                return true;
            }
        }
        return false;
    }

    virtual UObject *GetAssetFromComponent(UActorComponent *InComponent) override
    {
        if (UFMODAudioComponent *AudioComp = Cast<UFMODAudioComponent>(InComponent))
        {
            return AudioComp->Event;
        }
        return NULL;
    }
};
