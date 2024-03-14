// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#include "FMODSnapshotReverb.h"

UFMODSnapshotReverb::UFMODSnapshotReverb(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
}

bool UFMODSnapshotReverb::IsAsset() const
{
    return this != GetClass()->GetDefaultObject();
}

#if WITH_EDITORONLY_DATA
void UFMODSnapshotReverb::PostEditChangeProperty(FPropertyChangedEvent &PropertyChangedEvent)
{
}
#endif // EDITORONLY_DATA