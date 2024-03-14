// Copyright (c), Firelight Technologies Pty, Ltd. 2023-2023.

#pragma once

#include "FMODAudioLinkSettings.h"
#include "IAudioLink.h"
#include "IAudioLinkBlueprintInterface.h"

class UAudioComponent;

#include "FMODAudioLinkComponent.generated.h"

UCLASS(ClassGroup = (Audio, Common), HideCategories = (Object, ActorComponent, Physics, Rendering, Mobility, LOD),
    ShowCategories = Trigger, meta = (BlueprintSpawnableComponent))
class UFMODAudioLinkComponent : public USceneComponent, public IAudioLinkBlueprintInterface
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AudioLink")
    TObjectPtr<UFMODAudioLinkSettings> Settings;

    /** The sound to be played */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
    TObjectPtr<class USoundBase> Sound;

protected:
    //~ Begin IAudioLinkInterface
    virtual void SetLinkSound(USoundBase* NewSound) override;
    virtual void PlayLink(float StartTime = 0.0f) override;
    virtual void StopLink() override;
    virtual bool IsLinkPlaying() const override;
    //~ End IAudioLinkInterface

    //~ Begin ActorComponent Interface.
    virtual void OnRegister() override;
    virtual void OnUnregister() override;
    //~ End ActorComponent Interface.

    void CreateAudioComponent();

    UPROPERTY(Transient)
    TObjectPtr<UAudioComponent> AudioComponent;

    void CreateLink();
    TUniquePtr<IAudioLink> LinkInstance;
};