// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#include "FMODPort.h"
#include "FMODStudioModule.h"

UFMODPort::UFMODPort(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
}

/** Get tags to show in content view */
void UFMODPort::GetAssetRegistryTags(TArray<FAssetRegistryTag> &OutTags) const
{
    Super::GetAssetRegistryTags(OutTags);
}

FString UFMODPort::GetDesc()
{
    return FString::Printf(TEXT("Port %s"), *AssetGuid.ToString(EGuidFormats::DigitsWithHyphensInBraces));
}
