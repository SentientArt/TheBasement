// Copyright (c), Firelight Technologies Pty, Ltd. 2023-2023.

#include "FMODAudioLinkSettings.h"
#include "FMODAudioLinkFactory.h"
#include "FMODAudioLinkLog.h"
#include "FMODStudioModule.h"
#include "FMODEvent.h"
#include "fmod_studio.hpp"
#include "FMODSettings.h"

#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Async/Async.h"


FFMODAudioLinkSettingsProxy::FFMODAudioLinkSettingsProxy(const UFMODAudioLinkSettings& InSettings)
{
    Update(InSettings);
}

void FFMODAudioLinkSettingsProxy::Update(const UFMODAudioLinkSettings& InSettings)
{
    InSettings.RequestLoadLinkEvent();
    ReceivingBufferSizeInFrames = InSettings.GetReceivingBufferSizeInFrames();
    ProducerToConsumerBufferRatio = InSettings.ProducerToConsumerBufferRatio;
    InitialSilenceFillRatio = InSettings.InitialSilenceFillRatio;
    FMODLinkEvent = InSettings.LinkEvent;
}

bool FFMODAudioLinkSettingsProxy::IsEventDataLoaded() const
{
    return bIsEventDataLoaded;
}

void FFMODAudioLinkSettingsProxy::RegisterCallback(const TFunction<void()>& InCallback, FDelegateHandle& OutHandle)
{
    FScopeLock Lock(&CS);
    OutHandle = OnEventLoadedDelegate.Add(FSimpleMulticastDelegate::FDelegate::CreateLambda(InCallback));
}

bool FFMODAudioLinkSettingsProxy::UnRegisterCallback(const FDelegateHandle& InDelegate)
{
    FScopeLock Lock(&CS);
    return OnEventLoadedDelegate.Remove(InDelegate);
}

void FFMODAudioLinkSettingsProxy::NotifyEventDataLoaded()
{
    FScopeLock Lock(&CS);
    bIsEventDataLoaded = true;
    OnEventLoadedDelegate.Broadcast();
}

#if WITH_EDITOR
void FFMODAudioLinkSettingsProxy::RefreshFromSettings(UAudioLinkSettingsAbstract* InSettings, FPropertyChangedEvent& InPropertyChangedEvent)
{
    UE_LOG(LogFMODAudioLink, VeryVerbose, TEXT("FFMODAudioLinkSettingsProxy::RefreshFromSettings."));
    Update(*CastChecked<UFMODAudioLinkSettings>(InSettings));
}
#endif //WITH_EDITOR


void UFMODAudioLinkSettings::PostLoad()
{
    RequestLoadLinkEvent();
    Super::PostLoad();
}

void UFMODAudioLinkSettings::RequestLoadLinkEvent() const
{
    if (bLoadRequested)
    {
        return;
    }
    bLoadRequested = true;
    const UFMODSettings& Settings = *GetDefault<UFMODSettings>();
    if (Settings.bFMODAudioLinkEnabled)
    {
        AsyncTask(ENamedThreads::GameThread, [WeakThis = MakeWeakObjectPtr(const_cast<UFMODAudioLinkSettings*>(this))]()
            {
                if (WeakThis.IsValid())
                {
                    FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
                    FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(WeakThis.Get(), &UFMODAudioLinkSettings::OnLoadCompleteCallback);
                    WeakThis->LoadingHandle = StreamableManager.RequestAsyncLoad(WeakThis->LinkEvent.ToSoftObjectPath(), Delegate, FStreamableManager::AsyncLoadHighPriority,
                        /* Managed active handle */ true);
                    UE_LOG(LogFMODAudioLink, VeryVerbose, TEXT("FFMODAudioLinkSettings::RequestLoadLinkEvent: Async Loading %s."), *WeakThis->LinkEvent.ToSoftObjectPath().ToString());
                }
            });
    }
}

void UFMODAudioLinkSettings::OnLoadCompleteCallback()
{
    TArray<UObject*> LoadedAssets;
    LoadingHandle->GetLoadedAssets(LoadedAssets);
    if (LoadedAssets.Num() > 0 && IsValid(LoadedAssets[0]))
    {
        LinkEventResolved = CastChecked<UFMODEvent>(LoadedAssets[0]);
        if (IsValid(LinkEventResolved))
        {
            LinkEventResolved->AddToRoot();
        }
        else
        {
            UE_LOG(LogFMODAudioLink, Error, TEXT("FFMODAudioLinkSettings::OnLoadCompleteCallback: Unable to resolve Link Event."));
        }
    }

    GetCastProxy<FFMODAudioLinkSettingsProxy>()->NotifyEventDataLoaded();
    LoadingHandle.Reset();
}

void UFMODAudioLinkSettings::FinishDestroy()
{
    if (IsValid(LinkEventResolved))
    {
        LinkEventResolved->RemoveFromRoot();
    }
    Super::FinishDestroy();
}

int32 UFMODAudioLinkSettings::GetReceivingBufferSizeInFrames() const
{
    if (IFMODStudioModule::IsAvailable())
    {
        FMOD::System* CoreSystem;
        FMOD::Studio::System* StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Max);
        if (StudioSystem)
        {
            StudioSystem->getCoreSystem(&CoreSystem);
            if (CoreSystem)
            {
                unsigned int bufferLength = 0;
                CoreSystem->getDSPBufferSize(&bufferLength, 0);

                return bufferLength;
            }
        }
    }

    static const int32 SensibleDefaultSize = 512;
    UE_LOG(LogFMODAudioLink, Warning, TEXT("FMODAudioLinkSettings: Failed to get FMOD settings for buffer size, defaulting to '%d'"), SensibleDefaultSize);
    return SensibleDefaultSize;
}

FName UFMODAudioLinkSettings::GetFactoryName() const
{
    return FFMODAudioLinkFactory::GetFactoryNameStatic();
}