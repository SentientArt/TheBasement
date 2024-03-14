// Copyright (c), Firelight Technologies Pty, Ltd. 2023-2023.

#pragma once

#include "IAudioLink.h"
#include "Misc/ScopeRWLock.h"
#include "fmod.hpp"

struct FFMODAudioLinkSynchronizer : IAudioLinkSynchronizer, TSharedFromThis<FFMODAudioLinkSynchronizer, ESPMode::ThreadSafe>
{
    IAudioLinkSynchronizer::FOnSuspend      OnSuspend;
    IAudioLinkSynchronizer::FOnResume       OnResume;
    IAudioLinkSynchronizer::FOnOpenStream   OnOpenStream;
    IAudioLinkSynchronizer::FOnCloseStream  OnCloseStream;
    IAudioLinkSynchronizer::FOnBeginRender  OnBeginRender;
    IAudioLinkSynchronizer::FOnEndRender    OnEndRender;

    FRWLock RwLock;
    FCriticalSection CallbackLock;
    FMOD::System* CoreSystem;

    bool bIsBound{ false };
    unsigned int dspBufferSize;

    FFMODAudioLinkSynchronizer();
    ~FFMODAudioLinkSynchronizer() override;

    void ExecuteEndRender(uint64 dspClock);
    void ExecuteOpenStream();
    void ExecuteCloseStream();
    void ExecuteSuspend();
    void ExecuteResume();
    void Bind();
    void Unbind();

    #define MAKE_DELEGATE_FUNC(X)\
        FDelegateHandle Register##X##Delegate(const FOn##X::FDelegate& InDelegate) override\
        {\
            FWriteScopeLock WriteLock(RwLock);\
            return On##X.Add(InDelegate);\
        }\
        bool Remove##X##Delegate(const FDelegateHandle& InHandle) override\
        {\
            FWriteScopeLock WriteLock(RwLock);\
            return On##X.Remove(InHandle);\
        }

    MAKE_DELEGATE_FUNC(Suspend)
    MAKE_DELEGATE_FUNC(Resume)
    MAKE_DELEGATE_FUNC(OpenStream)
    MAKE_DELEGATE_FUNC(CloseStream)
    MAKE_DELEGATE_FUNC(BeginRender) //Use to sync UE and FMOD
    MAKE_DELEGATE_FUNC(EndRender) //Use to sync UE and FMOD

    #undef MAKE_DELEGATE_FUNC

    friend FMOD_RESULT F_CALLBACK MixCallback(FMOD_SYSTEM* system, FMOD_SYSTEM_CALLBACK_TYPE type, void* commanddata1, void* commanddata2, void* userdata);
};