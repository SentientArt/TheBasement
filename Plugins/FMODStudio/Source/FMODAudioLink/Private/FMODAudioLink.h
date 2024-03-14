// Copyright (c), Firelight Technologies Pty, Ltd. 2023-2023.

#pragma once

#include "IAudioLink.h"
#include "AudioDevice.h"
#include "IBufferedAudioOutput.h"
#include "FMODAudioLinkInputClient.h"

//* AudioLink Instance, a container holding shared pointers for lifetime management. */
struct FFMODAudioLink : IAudioLink
{
    FSharedBufferedOutputPtr ProducerSP;

    FSharedFMODAudioLinkInputClientPtr ConsumerSP;

    FAudioDevice* AudioDevice;

    FFMODAudioLink(const FSharedBufferedOutputPtr& InProducerSP, const FSharedFMODAudioLinkInputClientPtr& InConsumerSP, FAudioDevice* InAudioDevice = nullptr)
        : ProducerSP(InProducerSP)
        , ConsumerSP(InConsumerSP)
        , AudioDevice(InAudioDevice)
    {}

    virtual ~FFMODAudioLink() override
    {
        if (ConsumerSP.IsValid())
        {
            ConsumerSP->Stop();
        }
        if (AudioDevice && ProducerSP.IsValid())
        {
            ProducerSP->Stop(AudioDevice);
        }
    }
};