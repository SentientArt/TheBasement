// Copyright (c), Firelight Technologies Pty, Ltd. 2023-2023.

#pragma once

#include "AudioLinkSettingsAbstract.h"
#include "Engine/StreamableManager.h"

#include "FMODAudioLinkSettings.generated.h"

class UFMODAudioLinkSettings;
class UFMODEvent;

class FFMODAudioLinkSettingsProxy : public IAudioLinkSettingsProxy
{
public:
    FFMODAudioLinkSettingsProxy(const UFMODAudioLinkSettings&);
    virtual ~FFMODAudioLinkSettingsProxy() = default;

    const TSoftObjectPtr<UFMODEvent>& GetLinkEvent() const { return FMODLinkEvent; }
    int32 GetReceivingBufferSizeInFrames() const { return ReceivingBufferSizeInFrames; }
    bool ShouldClearBufferOnReceipt() const { return bShouldZeroBuffer; }
    float GetProducerConsumerBufferRatio() const { return ProducerToConsumerBufferRatio; }
    float GetInitialSilenceFillRatio() const { return InitialSilenceFillRatio; }

    void Update(const UFMODAudioLinkSettings&);

    bool IsEventDataLoaded() const;
    void RegisterCallback(const TFunction<void()>& InCallback, FDelegateHandle& OutHandle);
    bool UnRegisterCallback(const FDelegateHandle& InDelegate);

protected:
    void NotifyEventDataLoaded();

private:
#if WITH_EDITOR
    void RefreshFromSettings(UAudioLinkSettingsAbstract* InSettings, FPropertyChangedEvent& InPropertyChangedEvent) override;
#endif //WITH_EDITOR

    FSimpleMulticastDelegate OnEventLoadedDelegate;
    FCriticalSection CS;
    friend class UFMODAudioLinkSettings;

    TSoftObjectPtr<UFMODEvent> FMODLinkEvent;
    int32 ReceivingBufferSizeInFrames;
    bool bShouldZeroBuffer = true;
    bool bIsEventDataLoaded = false;
    float ProducerToConsumerBufferRatio = 2.0f;
    float InitialSilenceFillRatio = 1.0f;
};

using FSharedFMODAudioLinkSettingsProxyPtr = TSharedPtr<FFMODAudioLinkSettingsProxy, ESPMode::ThreadSafe>;

UCLASS(config = Engine, defaultconfig)
class FMODSTUDIO_API UFMODAudioLinkSettings : public UAudioLinkSettingsAbstract
{
    GENERATED_BODY()

    UPROPERTY(Config, EditAnywhere, Category = default)
    TSoftObjectPtr<UFMODEvent> LinkEvent;

    /** If this is set, the receiving code will clear the buffer after it's read, so it's not rendered by Unreal. Only applies if running both renderers at once.  */
    UPROPERTY(Config, EditAnywhere, Category = default)
    bool bShouldClearBufferOnReceipt = true;

    /** This is the ratio of producer to consumer buffer size, 2.0 means its twice as big as the consumer buffer.  */
    UPROPERTY(Config, EditAnywhere, Category = default)
    float ProducerToConsumerBufferRatio = 2.0f;

    /** Ratio of initial buffer to fill with silence ahead of consumption. Adjusting this can resolve starvation at the cost of added latency. */
    UPROPERTY(Config, EditAnywhere, Category = default)
    float InitialSilenceFillRatio = 1.0f;

    void RequestLoadLinkEvent() const;

protected:
    TSharedPtr<FStreamableHandle> LoadingHandle;

    /** Once the SoftObjectReference has been resolved, attach the reference here so it's owned. */
    UPROPERTY(Transient)
    TObjectPtr<UFMODEvent> LinkEventResolved;

    void PostLoad() override; // Only occurs at editor startup and game startup, not PIE
    void OnLoadCompleteCallback();
    void FinishDestroy() override;
    mutable bool bLoadRequested = false;

    friend class FFMODAudioLinkSettingsProxy;

    int32 GetReceivingBufferSizeInFrames() const;

    UAudioLinkSettingsAbstract::FSharedSettingsProxyPtr MakeProxy() const override
    {
        return UAudioLinkSettingsAbstract::FSharedSettingsProxyPtr(new FFMODAudioLinkSettingsProxy{ *this });
    }

    FName GetFactoryName() const override;
};

