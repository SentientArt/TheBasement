// Copyright (c), Firelight Technologies Pty, Ltd. 2023-2023.

#pragma once

#include "IAudioLink.h"
#include "IAudioLinkFactory.h"
#include "Templates/SharedPointer.h"
#include "IBufferedAudioOutput.h"
#include "DSP/BufferVectorOperations.h"

#include "fmod_studio.hpp"

class UFMODAudioLinkComponent;
class FFMODAudioLinkInputClient;

class FFMODAudioLinkInputClient : public TSharedFromThis<FFMODAudioLinkInputClient, ESPMode::ThreadSafe>
{
public:
    FFMODAudioLinkInputClient(const FSharedBufferedOutputPtr& InToConsumeFrom, const UAudioLinkSettingsAbstract::FSharedSettingsProxyPtr& InSettings, FName InNameOfProducingSource={});
    virtual ~FFMODAudioLinkInputClient();

    /// Used by all Audio Link sources.
    /// Optional component parameter that is used with the FMODAudioLinkComponent.
    void Start(USceneComponent* InComponent = nullptr);
    void Stop();

    struct FWorldState
    {
        FTransform WorldTransform;
    };
    // Called from Consumer thread at game tick rate.
    void UpdateWorldState(const FWorldState&);
    // Called from FMOD thread.
    bool GetSamples(void* data, unsigned int datalen);

    IBufferedAudioOutput::FBufferFormat* GetFormat();

    /// Used Sound "Sources" instead of FormatKnownDelegate to store the format at the time of starting.
    void SetFormat(const IBufferedAudioOutput::FBufferFormat *AudioFormat);

    FName GetProducerName() const { return ProducerName; }
    FMOD::Studio::EventInstance* EventInstance;

private:
    void Register(const FName& NameOfProducingSource);
    void Unregister();

    FWeakBufferedOutputPtr WeakProducer;
    UAudioLinkSettingsAbstract::FSharedSettingsProxyPtr Settings;

    IBufferedAudioOutput::FBufferFormat UnrealFormat;

    FName ProducerName;
    int32 NumStarvedBuffersInARow = 0;
    FDelegateHandle IsLoadedHandle;
};

using FSharedFMODAudioLinkInputClientPtr = TSharedPtr<FFMODAudioLinkInputClient, ESPMode::ThreadSafe>;