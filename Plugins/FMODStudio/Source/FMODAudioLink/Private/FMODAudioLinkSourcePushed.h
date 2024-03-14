// Copyright (c), Firelight Technologies Pty, Ltd. 2023-2023.

#pragma once
#include "FMODAudioLinkInputClient.h"

#include "IAudioLink.h"
#include "IBufferedAudioOutput.h"

struct FFMODAudioLinkSourcePushed : IAudioLinkSourcePushed
{
    int32 SourceId = INDEX_NONE;
    int32 NumFramesReceivedSoFar = INDEX_NONE;
    FSharedBufferedOutputPtr ProducerSP;
    FSharedFMODAudioLinkInputClientPtr ConsumerSP;
    IAudioLinkFactory::FAudioLinkSourcePushedCreateArgs CreateArgs;

    FFMODAudioLinkSourcePushed(const IAudioLinkFactory::FAudioLinkSourcePushedCreateArgs& InArgs, IAudioLinkFactory* InFactory);
    virtual ~FFMODAudioLinkSourcePushed() override;
    void OnNewBuffer(const FOnNewBufferParams& InArgs) override;
    void OnSourceDone(const int32 InSourceId) override;
    void OnSourceReleased(const int32 InSourceId) override;
    void OnUpdateWorldState(const FOnUpdateWorldStateParams& InParams) override;
};