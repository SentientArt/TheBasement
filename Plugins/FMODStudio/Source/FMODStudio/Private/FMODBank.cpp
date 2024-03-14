// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#include "FMODBank.h"
#include "FMODStudioModule.h"

UFMODBank::UFMODBank(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
}

/** Get tags to show in content view */
void UFMODBank::GetAssetRegistryTags(TArray<FAssetRegistryTag> &OutTags) const
{
    Super::GetAssetRegistryTags(OutTags);
}

FString UFMODBank::GetDesc()
{
    return FString::Printf(TEXT("Bank %s"), *AssetGuid.ToString(EGuidFormats::DigitsWithHyphensInBraces));
}
