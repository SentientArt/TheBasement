// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#include "FMODVCA.h"
#include "FMODStudioModule.h"

UFMODVCA::UFMODVCA(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
}

/** Get tags to show in content view */
void UFMODVCA::GetAssetRegistryTags(TArray<FAssetRegistryTag> &OutTags) const
{
    Super::GetAssetRegistryTags(OutTags);
}

FString UFMODVCA::GetDesc()
{
    return FString::Printf(TEXT("VCA %s"), *AssetGuid.ToString(EGuidFormats::DigitsWithHyphensInBraces));
}
