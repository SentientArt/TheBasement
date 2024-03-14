#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "FMODEvent.h"
#include "FMODAnimNotifyPlay.generated.h"

UCLASS(const, hidecategories = Object, collapsecategories, meta = (DisplayName = "Play FMOD Event"))
class FMODSTUDIO_API UFMODAnimNotifyPlay : public UAnimNotify
{
    GENERATED_BODY()

public:
    UFMODAnimNotifyPlay();

// Begin UAnimNotify interface
    virtual void Notify(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *AnimSeq) override;
    virtual FString GetNotifyName_Implementation() const override;
// End UAnimNotify interface

    // If this sound should follow its owner
    UPROPERTY(EditAnywhere, Category = "FMOD Anim Notify")
    uint32 bFollow : 1;

    // Socket or bone name to attach sound to
    UPROPERTY(EditAnywhere, Category = "FMOD Anim Notify", meta = (EditCondition = "bFollow"))
    FString AttachName;

    // Sound to Play
    UPROPERTY(EditAnywhere, Category = "FMOD Anim Notify", BlueprintReadWrite)
    class UFMODEvent* Event;
};
