// Copyright (c), Firelight Technologies Pty, Ltd. 2023-2023.

#include "FMODAudioLinkInputClient.h"
#include "FMODAudioLinkLog.h"
#include "FMODAudioLinkSettings.h"
#include "FMODAudioLinkFactory.h"
#include "FMODAudioLinkComponent.h"
#include "FMODEvent.h"

#include "FMODStudioModule.h"
#include "FMODBlueprintStatics.h"

#include <inttypes.h>
#include "Async/Async.h"
#include "Templates/SharedPointer.h"

class InputClientRef
{
public:
    TSharedRef<FFMODAudioLinkInputClient> InputClient;

    InputClientRef(TSharedRef<FFMODAudioLinkInputClient> InputSP)
        : InputClient(InputSP)
    {
    }
};

FMOD::Studio::System* GetStudioSystem()
{
    if (IFMODStudioModule::IsAvailable())
    {
        auto* StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
        if (!StudioSystem)
        {
            StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Auditioning);
        }
        return StudioSystem;
    }
    return nullptr;
}

void FFMODAudioLinkInputClient::Register(const FName& NameOfProducingSource)
{
    const auto Name = NameOfProducingSource.GetPlainNameString();

    if (UNLIKELY(!Settings.IsValid()))
    {
        UE_LOG(LogFMODAudioLink, Warning, TEXT("FFMODAudioLinkInputClient::Register: FMODAudioLinkSettings are not valid."));
        return;
    }

    if (UNLIKELY(!GetStudioSystem()))
    {
        UE_LOG(LogFMODAudioLink, Warning, TEXT("FFMODAudioLinkInputClient::Register: Unable to get FMOD Studio System."));
        return;
    }

    AsyncTask(ENamedThreads::GameThread, []
    {
        const auto AudioDeviceManager = FAudioDeviceManager::Get();
        if (UNLIKELY(!AudioDeviceManager))
        {
            UE_LOG(LogFMODAudioLink, Warning, TEXT("FFMODAudioLinkInputClient::Register: No AudioDeviceManager at registration."));
            return;
        }
        const auto AudioDevice = AudioDeviceManager->GetActiveAudioDevice();
        if (UNLIKELY(!AudioDevice))
        {
            UE_LOG(LogFMODAudioLink, Warning, TEXT("FFMODAudioLinkInputClient::Register: No active AudioDevice at registration."));
            return;
        }
        UE_CLOG(UNLIKELY(AudioDevice->GetMaxChannels() == 0), LogFMODAudioLink, Warning,
            TEXT("FMODAudioLink: The current AudioDevice %" PRIu32 " has 0 MaxChannels. Consider setting AudioMaxChannels to a sensible value in the Engine config file's TargetSettings for your platform."),
            AudioDevice->DeviceID);

        UE_CLOG(!FFMODAudioLinkFactory::bHasSubmix,
            LogFMODAudioLink, Warning, TEXT("FMODAudioLink: No initial submix got routed to AudioLink. Consider creating custom versions of global submixes in Project Settings Audio, and Enable Audio Link in their advanced settings."));
    });
}

void FFMODAudioLinkInputClient::Unregister()
{
    UE_LOG(LogFMODAudioLink, Verbose, TEXT("FFMODAudioLinkInputClient::Unregister."));
}

FFMODAudioLinkInputClient::FFMODAudioLinkInputClient(const FSharedBufferedOutputPtr& ToConsumeFrom, const UAudioLinkSettingsAbstract::FSharedSettingsProxyPtr& Settings, FName NameOfProducingSource)
    : WeakProducer(ToConsumeFrom)
    , Settings(Settings)
    , ProducerName(NameOfProducingSource)
{
    check(Settings.IsValid());
    Register(NameOfProducingSource);
    UnrealFormat = {};
}

FFMODAudioLinkInputClient::~FFMODAudioLinkInputClient()
{
    Unregister();
}

FMOD_RESULT F_CALLBACK pcmreadcallback(FMOD_SOUND* inSound, void* data, unsigned int datalen)
{
    FMOD::Sound* sound = (FMOD::Sound*)inSound;
    FFMODAudioLinkInputClient* ConsumerSP;
    sound->getUserData((void**)&ConsumerSP);

    ConsumerSP->GetSamples(data, datalen);

    return FMOD_OK;
}

FMOD_RESULT F_CALLBACK SoundCallback(FMOD_STUDIO_EVENT_CALLBACK_TYPE type, FMOD_STUDIO_EVENTINSTANCE* event, void* parameters)
{
    FMOD_RESULT result = FMOD_OK;
    FMOD::Studio::EventInstance* eventInstance = (FMOD::Studio::EventInstance*)event;

    if (type == FMOD_STUDIO_EVENT_CALLBACK_CREATE_PROGRAMMER_SOUND)
    {
        InputClientRef* ClientRef;
        result = eventInstance->getUserData((void**)&ClientRef);

        FFMODAudioLinkInputClient* ConsumerPtr = &ClientRef->InputClient.Get();
        auto formatInfo = ConsumerPtr->GetFormat();

        FMOD::System* CoreSystem = nullptr;
        GetStudioSystem()->getCoreSystem(&CoreSystem);

        // Create sound info
        FMOD_CREATESOUNDEXINFO exinfo;
        memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
        exinfo.cbsize               = sizeof(FMOD_CREATESOUNDEXINFO);                                           /* Required. */
        exinfo.numchannels          = formatInfo->NumChannels;                                                  /* Number of channels in the sound. */
        exinfo.defaultfrequency     = formatInfo->NumSamplesPerSec;                                             /* Default playback rate of sound. */
        exinfo.decodebuffersize     = formatInfo->NumSamplesPerBlock / exinfo.numchannels;                      /* Chunk size of stream update in samples. Should match the FMOD System. */
        exinfo.length               = exinfo.defaultfrequency * exinfo.numchannels * sizeof(signed short) * 5;  /* Length of PCM data in bytes of whole song (for Sound::getLength) */
        exinfo.format               = FMOD_SOUND_FORMAT_PCMFLOAT;                                               /* Data format of sound. */
        exinfo.pcmreadcallback      = pcmreadcallback;                                                          /* User callback for reading. */
        exinfo.userdata             = ConsumerPtr;

        FMOD::Sound* sound = NULL;
        FString sourceName = ConsumerPtr->GetProducerName().ToString();
        result = CoreSystem->createSound(TCHAR_TO_ANSI(*sourceName), FMOD_OPENUSER | FMOD_CREATESTREAM, &exinfo, &sound);

        // Pass the sound to FMOD
        FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES* props = (FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES*)parameters;
        props->sound = (FMOD_SOUND*)sound;
        UE_LOG(LogFMODAudioLink, Verbose, TEXT("Sound Created: %s , Consumer = %" PRIu64 "."), *sourceName, ConsumerPtr);
    }
    else if (type == FMOD_STUDIO_EVENT_CALLBACK_DESTROY_PROGRAMMER_SOUND)
    {
        // Obtain the sound
        FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES* props = (FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES*)parameters;
        FMOD::Sound* sound = (FMOD::Sound*)props->sound;

        // Release the sound
        UE_LOG(LogFMODAudioLink, Verbose, TEXT("Sound Release: %" PRIu64 "."), sound);
        result = sound->release();
    }
    else if (type == FMOD_STUDIO_EVENT_CALLBACK_DESTROYED)
    {
        InputClientRef* ClientRef = nullptr;
        result = eventInstance->getUserData((void**)&ClientRef);

        UE_LOG(LogFMODAudioLink, Verbose, TEXT("Event Destroyed: ClientRef = %" PRIu64 "."), ClientRef);
        if (ClientRef)
        {
            delete ClientRef;
        }
    }

    return result;
}

void FFMODAudioLinkInputClient::Start(USceneComponent* InComponent)
{
    Stop();
    check(!IsLoadedHandle.IsValid());

    FFMODAudioLinkSettingsProxy* FMODSettings = static_cast<FFMODAudioLinkSettingsProxy*>(Settings.Get());
    const auto LinkEvent = FMODSettings->GetLinkEvent();

    auto SelfSP = AsShared();
    auto PlayLambda = [SelfSP, LinkEvent, InComponent]()
        {
            UE_LOG(LogFMODAudioLink, Verbose, TEXT("FFMODAudioLinkInputClient::Start: SelSP = %" PRIu64 ", LinkEvent = %s, InComponent = %" PRIu64 "."), &SelfSP, LinkEvent.Get(), &InComponent);

            FMOD::Studio::EventDescription* EventDesc = IFMODStudioModule::Get().GetEventDescription(LinkEvent.Get());
            if (EventDesc != nullptr)
            {
                FMOD::Studio::EventInstance* EventInst = NULL;
                EventDesc->createInstance(&EventInst);
                SelfSP->EventInstance = EventInst;
                if (EventInst != nullptr)
                {
                    FTransform EventTransform = InComponent ? InComponent->GetComponentTransform() : FTransform();
                    FMOD_3D_ATTRIBUTES EventAttr = { { 0 } };
                    FMODUtils::Assign(EventAttr, EventTransform);
                    EventInst->set3DAttributes(&EventAttr);

                    EventInst->setCallback(SoundCallback, FMOD_STUDIO_EVENT_CALLBACK_CREATE_PROGRAMMER_SOUND | FMOD_STUDIO_EVENT_CALLBACK_DESTROY_PROGRAMMER_SOUND | FMOD_STUDIO_EVENT_CALLBACK_DESTROYED);

                    InputClientRef* callbackMemory = new InputClientRef(SelfSP);

                    EventInst->setUserData(callbackMemory);
                    EventInst->start();
                }
            }
        };

    FMODSettings->IsEventDataLoaded() ? PlayLambda() : FMODSettings->RegisterCallback(PlayLambda, IsLoadedHandle);
}

void FFMODAudioLinkInputClient::Stop()
{
    if (EventInstance->isValid())
    {
        UE_LOG(LogFMODAudioLink, Verbose, TEXT("FFMODAudioLinkInputClient::Stop: Stopping EventInstance."));
        EventInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
        EventInstance->release();
    }

    if (IsLoadedHandle.IsValid())
    {
        FFMODAudioLinkSettingsProxy* FMODSettings = static_cast<FFMODAudioLinkSettingsProxy*>(Settings.Get());
        check(FMODSettings);

        FMODSettings->UnRegisterCallback(IsLoadedHandle);
        IsLoadedHandle.Reset();
    }
}

void FFMODAudioLinkInputClient::UpdateWorldState(const FWorldState& InParams)
{
    if (EventInstance->isValid())
    {
        const FTransform& T = InParams.WorldTransform;
        FMOD_3D_ATTRIBUTES attr = { { 0 } };
        FMODUtils::Assign(attr, T);

        // TODO: velocity

        EventInstance->set3DAttributes(&attr);
    }
}

bool FFMODAudioLinkInputClient::GetSamples(void* data, unsigned int dataLenBytes)
{
    FSharedBufferedOutputPtr StrongBufferProducer{ WeakProducer.Pin() };
    if (!StrongBufferProducer.IsValid())
    {
        // return false, to indicate no more data.
        FMemory::Memzero(data, dataLenBytes);
        return false;
    }

    float* dataBuffer = (float*)data;

    int32 FramesWritten = 0;

    int32 dataLenFrames = dataLenBytes / (sizeof(float));

    bool bMoreDataRemaining = StrongBufferProducer->PopBuffer(dataBuffer, dataLenFrames, FramesWritten);

    // Zero any buffer space that we didn't output to.
    int32 FramesThatNeedZeroing = dataLenFrames - FramesWritten;

    UE_LOG(LogFMODAudioLink, Verbose, TEXT("FFMODAudioLinkInputClient::GetSamples: (post-pop), SamplesPopped=%d, SamplesNeeded=%d, ZeroFrames=%d, This=0x%p"),
        FramesWritten, dataLenFrames, FramesThatNeedZeroing, this);

    if (FramesThatNeedZeroing > 0)
    {
        FMemory::Memset(&dataBuffer[FramesWritten], 0, FramesThatNeedZeroing);
        NumStarvedBuffersInARow++;

        static const int32 NumStatedBuffersBeforeStop = 5;
        if (NumStarvedBuffersInARow > NumStatedBuffersBeforeStop)
        {
            UE_LOG(LogFMODAudioLink, Verbose, TEXT("FMODAudioLinkInputClient::GetSamples: Stopping Starving input object, Needed=%d, Red=%d, StarvedCount=%d, This=0x%p"),
                dataLenFrames, FramesWritten, NumStarvedBuffersInARow, this);

            // Terminate.
            bMoreDataRemaining = false;
        }
    }
    else
    {
        NumStarvedBuffersInARow = 0;
    }

    return bMoreDataRemaining;
}

IBufferedAudioOutput::FBufferFormat* FFMODAudioLinkInputClient::GetFormat()
{
    // Ensure we're still listening to a sub mix that exists.
    FSharedBufferedOutputPtr StrongPtr{ WeakProducer.Pin() };
    if (!StrongPtr.IsValid())
    {
        UE_LOG(LogFMODAudioLink, Verbose, TEXT("FMODAudioLinkInputClient::GetFormat: FSharedBufferedOutputPtr not valid."));
    }
    else
    {
        ensure(StrongPtr->GetFormat(UnrealFormat));
    }

    return &UnrealFormat;
}

void FFMODAudioLinkInputClient::SetFormat(const IBufferedAudioOutput::FBufferFormat *AudioFormat)
{
    UnrealFormat.NumChannels = AudioFormat->NumChannels;
    UnrealFormat.NumSamplesPerBlock = AudioFormat->NumSamplesPerBlock;
    UnrealFormat.NumSamplesPerSec = AudioFormat->NumSamplesPerSec;
}