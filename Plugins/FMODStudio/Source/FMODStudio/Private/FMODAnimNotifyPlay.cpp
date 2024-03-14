
#include "FMODAnimNotifyPlay.h"
#include "FMODBlueprintStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UFMODAnimNotifyPlay::UFMODAnimNotifyPlay()
    : Super()
{
    bFollow = false;
    Event = nullptr;

#if WITH_EDITORONLY_DATA
    NotifyColor = FColor(196, 142, 255, 255);
#endif // WITH_EDITORONLY_DATA
}

void UFMODAnimNotifyPlay::Notify(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *AnimSeq)
{
    if (Event)
    {
        if (bFollow)
        {
            // Play event attached
            UFMODBlueprintStatics::PlayEventAttached(
                Event, MeshComp, *AttachName, FVector(0, 0, 0), EAttachLocation::KeepRelativeOffset, false, true, true);
        }
        else
        {
            // Play event at location
            UFMODBlueprintStatics::PlayEventAtLocation(MeshComp, Event, MeshComp->GetComponentTransform(), true);
        }
    }
}

FString UFMODAnimNotifyPlay::GetNotifyName_Implementation() const
{
    if (Event)
    {
        return Event->GetName();
    }
    else
    {
        return Super::GetNotifyName_Implementation();
    }
}
