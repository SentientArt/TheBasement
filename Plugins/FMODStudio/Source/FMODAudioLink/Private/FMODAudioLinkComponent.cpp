// Copyright (c), Firelight Technologies Pty, Ltd. 2023-2023.

#include "FMODAudioLinkComponent.h"
#include "FMODAudioLinkFactory.h"
#include "Components/AudioComponent.h"
#include "FMODAudioLinkLog.h"

void UFMODAudioLinkComponent::CreateLink()
{
    if (!Settings)
    {
        Settings = GetMutableDefault<UFMODAudioLinkSettings>();
    }

    IAudioLinkFactory* Factory = IAudioLinkFactory::FindFactory(FFMODAudioLinkFactory::GetFactoryNameStatic());
    if (Factory)
    {
        UE_LOG(LogFMODAudioLink, VeryVerbose, TEXT("UFMODAudioLinkComponent::CreateLink."));
        IAudioLinkFactory::FAudioLinkSourceCreateArgs CreateArgs;
        CreateArgs.OwningComponent = this;
        CreateArgs.AudioComponent = AudioComponent;
        CreateArgs.Settings = Settings;
        LinkInstance = Factory->CreateSourceAudioLink(CreateArgs);
    }
}

void UFMODAudioLinkComponent::CreateAudioComponent()
{
    if (!AudioComponent)
    {
        UE_LOG(LogFMODAudioLink, VeryVerbose, TEXT("UFMODAudioLinkComponent::CreateAudioComponent."));
        AudioComponent = NewObject<UAudioComponent>(this);

        if (!AudioComponent->GetAttachParent() && !AudioComponent->IsAttachedTo(this))
        {
            AActor* Owner = GetOwner();
            if (!Owner || !Owner->GetWorld())
            {
                if (UWorld* World = GetWorld())
                {
                    AudioComponent->RegisterComponentWithWorld(World);
                    AudioComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
                }
                else
                {
                    AudioComponent->SetupAttachment(this);
                }
            }
            else
            {
                AudioComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
                AudioComponent->RegisterComponent();
            }
        }
    }

    if (AudioComponent)
    {
        AudioComponent->bAutoActivate = false;
        AudioComponent->bStopWhenOwnerDestroyed = true;
        AudioComponent->bShouldRemainActiveIfDropped = true;
        AudioComponent->Mobility = EComponentMobility::Movable;

#if WITH_EDITORONLY_DATA
        AudioComponent->bVisualizeComponent = false;
#endif
    }
}

void UFMODAudioLinkComponent::OnRegister()
{
    Super::OnRegister();

    CreateAudioComponent();

    if (ensure(AudioComponent))
    {
        check(LinkInstance == nullptr);
        CreateLink();
    }
}

void UFMODAudioLinkComponent::OnUnregister()
{
    LinkInstance.Reset();
    AudioComponent = nullptr;

    Super::OnUnregister();
}

void UFMODAudioLinkComponent::SetLinkSound(USoundBase* InSound)
{
    Sound = InSound;

    if (AudioComponent)
    {
        AudioComponent->SetSound(InSound);
    }
}

void UFMODAudioLinkComponent::PlayLink(float StartTime)
{
    if (AudioComponent)
    {
        UE_LOG(LogFMODAudioLink, VeryVerbose, TEXT("UFMODAudioLinkComponent::PlayLink."));
        AudioComponent->SetSound(Sound);
        AudioComponent->Play(StartTime);

        SetActiveFlag(AudioComponent->IsActive());
    }
}

void UFMODAudioLinkComponent::StopLink()
{
    if (IsActive())
    {
        if (AudioComponent)
        {
            UE_LOG(LogFMODAudioLink, VeryVerbose, TEXT("UFMODAudioLinkComponent::StopLink."));
            AudioComponent->Stop();
        }

        SetActiveFlag(false);
    }
}

bool UFMODAudioLinkComponent::IsLinkPlaying() const
{
    return AudioComponent && AudioComponent->IsPlaying();
}