// Copyright (c), Firelight Technologies Pty, Ltd. 2023-2023.

#pragma once

#include "FMODAudioLink.h"
#include "IAudioLinkFactory.h"

class FFMODAudioLinkFactory : public IAudioLinkFactory
{
public:
    FFMODAudioLinkFactory() = default;
    virtual ~FFMODAudioLinkFactory() override = default;

    static FName GetFactoryNameStatic();
    static bool bHasSubmix;

protected:
    /** Begin IAudioLinkFactory */
    FName GetFactoryName() const override;
    TSubclassOf<UAudioLinkSettingsAbstract> GetSettingsClass() const override;
    TUniquePtr<IAudioLink> CreateSubmixAudioLink(const FAudioLinkSubmixCreateArgs&) override;
    TUniquePtr<IAudioLink> CreateSourceAudioLink(const FAudioLinkSourceCreateArgs&) override;
    FAudioLinkSourcePushedSharedPtr CreateSourcePushedAudioLink(const FAudioLinkSourcePushedCreateArgs&) override;
    FAudioLinkSynchronizerSharedPtr CreateSynchronizerAudioLink() override;
    /** End IAudioLinkFactory */
};