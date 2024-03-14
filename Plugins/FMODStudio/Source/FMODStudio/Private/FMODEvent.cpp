// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#include "FMODEvent.h"
#include "FMODStudioModule.h"
#include "fmod_studio.hpp"

UFMODEvent::UFMODEvent(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
}

/** Get tags to show in content view */
void UFMODEvent::GetAssetRegistryTags(TArray<FAssetRegistryTag> &OutTags) const
{
    Super::GetAssetRegistryTags(OutTags);
    if (IFMODStudioModule::Get().AreBanksLoaded())
    {
        FMOD::Studio::EventDescription *EventDesc = IFMODStudioModule::Get().GetEventDescription(this, EFMODSystemContext::Max);

        bool bOneshot = false;
        bool bStream = false;
        bool b3D = false;
        if (EventDesc)
        {
            EventDesc->isOneshot(&bOneshot);
            EventDesc->isStream(&bStream);
            EventDesc->is3D(&b3D);
        }

        OutTags.Add(UObject::FAssetRegistryTag("Oneshot", bOneshot ? TEXT("True") : TEXT("False"), UObject::FAssetRegistryTag::TT_Alphabetical));
        OutTags.Add(UObject::FAssetRegistryTag("Streaming", bStream ? TEXT("True") : TEXT("False"), UObject::FAssetRegistryTag::TT_Alphabetical));
        OutTags.Add(UObject::FAssetRegistryTag("3D", b3D ? TEXT("True") : TEXT("False"), UObject::FAssetRegistryTag::TT_Alphabetical));
    }
}

FString UFMODEvent::GetDesc()
{
    return FString::Printf(TEXT("Event %s"), *AssetGuid.ToString(EGuidFormats::DigitsWithHyphensInBraces));
}

void UFMODEvent::GetParameterDescriptions(TArray<FMOD_STUDIO_PARAMETER_DESCRIPTION> &Parameters) const
{
    if (IFMODStudioModule::Get().AreBanksLoaded())
    {
        FMOD::Studio::EventDescription *EventDesc = IFMODStudioModule::Get().GetEventDescription(this, EFMODSystemContext::Auditioning);

        if (EventDesc)
        {
            int ParameterCount;
            EventDesc->getParameterDescriptionCount(&ParameterCount);
            Parameters.SetNumUninitialized(ParameterCount);
            for (int ParameterIndex = 0; ParameterIndex < ParameterCount; ++ParameterIndex)
            {
                EventDesc->getParameterDescriptionByIndex(ParameterIndex, &Parameters[ParameterIndex]);
            }
        }
    }
}
