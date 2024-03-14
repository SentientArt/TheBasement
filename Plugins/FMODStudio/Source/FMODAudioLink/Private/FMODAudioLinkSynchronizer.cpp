// Copyright (c), Firelight Technologies Pty, Ltd. 2023-2023.

#include "FMODAudioLinkSynchronizer.h"
#include "fmod_studio.hpp"
#include "FMODStudioModule.h"
#include "FMODAudioLinkLog.h"

#include "AudioDeviceManager.h"

FMOD_RESULT F_CALLBACK MixCallback(FMOD_SYSTEM* system, FMOD_SYSTEM_CALLBACK_TYPE type, void* commanddata1, void* commanddata2, void* userdata)
{
    FFMODAudioLinkSynchronizer *Synchro = static_cast<FFMODAudioLinkSynchronizer*>(userdata);
    if (Synchro)
    {
        FMOD::System *CoreSystem = (FMOD::System*)system;

        FMOD::ChannelGroup *MasterGroup = NULL;
        CoreSystem->getMasterChannelGroup(&MasterGroup);
        uint64 dspClock = 0;
        MasterGroup->getDSPClock(&dspClock, 0);

        switch (type)
        {
        case FMOD_SYSTEM_CALLBACK_POSTMIX:
            Synchro->ExecuteEndRender(dspClock);
            break;
        default:
            break;
        }
    }
    return FMOD_OK;
}

FFMODAudioLinkSynchronizer::FFMODAudioLinkSynchronizer()
{
    Bind();
}

FFMODAudioLinkSynchronizer::~FFMODAudioLinkSynchronizer()
{
    Unbind();
}

void FFMODAudioLinkSynchronizer::ExecuteEndRender(uint64 dspClock)
{
    FScopeLock Lock(&CallbackLock);
    FOnRenderParams Params;
    Params.BufferTickID = dspClock;
    Params.NumFrames = dspBufferSize;
    OnEndRender.Broadcast(Params);
}

void FFMODAudioLinkSynchronizer::ExecuteOpenStream()
{
    UE_LOG(LogFMODAudioLink, Verbose, TEXT("FFMODAudioLinkSynchronizer::ExecuteOpenStream: Opening stream between Unreal and FMOD."));

    int samplerate = 0, numchannels = 0;
    FMOD_SPEAKERMODE speakerMode;
    CoreSystem->getDSPBufferSize(&dspBufferSize, 0);
    CoreSystem->getSoftwareFormat(&samplerate, &speakerMode, 0);
    CoreSystem->getSpeakerModeChannels(speakerMode, &numchannels);

    FOnOpenStreamParams Params;
    Params.NumChannels = numchannels;
    Params.SampleRate = samplerate;
    Params.NumFrames = dspBufferSize;
    Params.Name = TEXT("AudioLink for FMOD");
    OnOpenStream.Broadcast(Params);
}

void FFMODAudioLinkSynchronizer::ExecuteCloseStream()
{
    UE_LOG(LogFMODAudioLink, Verbose, TEXT("FFMODAudioLinkSynchronizer::ExecuteCloseStream: Closing stream between Unreal and FMOD."));

    OnCloseStream.Broadcast();
}

void FFMODAudioLinkSynchronizer::ExecuteSuspend()
{
    OnSuspend.Broadcast();
}

void FFMODAudioLinkSynchronizer::ExecuteResume()
{
    OnResume.Broadcast();
}

void FFMODAudioLinkSynchronizer::Bind()
{
    if (UNLIKELY(bIsBound))
    {
        UE_LOG(LogFMODAudioLink, Error, TEXT("FFMODAudioLinkSynchronizer::Bind: Already bound."));
        return;
    }

    if (UNLIKELY(!IFMODStudioModule::Get().IsAvailable()))
    {
        UE_LOG(LogFMODAudioLink, Error, TEXT("FFMODAduioLinkSynchronizer::Bind: FMODStudioModule not available."));
        return;
    }

    auto* StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
    if (!StudioSystem)
    {
        StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Auditioning);
    }
    if (UNLIKELY(!StudioSystem))
    {
        UE_LOG(LogFMODAudioLink, Error, TEXT("FFMODAudioLinkSynchronizer::ExecuteOpenStream: No Studio System."));
        return;
    }

    StudioSystem->getCoreSystem(&CoreSystem);

    UE_LOG(LogFMODAudioLink, Verbose, TEXT("FFMODAudioLinkSynchronizer::Bind: Binding System Callbacks."));

    CoreSystem->setUserData(this);
    CoreSystem->setCallback(MixCallback, FMOD_SYSTEM_CALLBACK_POSTMIX);

    ExecuteOpenStream();

    bIsBound = true;
}

void FFMODAudioLinkSynchronizer::Unbind()
{
    if (UNLIKELY(!bIsBound))
    {
        return;
    }
    if (UNLIKELY(!CoreSystem))
    {
        return;
    }

    UE_LOG(LogFMODAudioLink, Verbose, TEXT("FFMODAudioLinkSynchronizer::Bind: Unbinding."));
    CoreSystem->setCallback(NULL, 0);
    bIsBound = false;

    ExecuteCloseStream();
}
