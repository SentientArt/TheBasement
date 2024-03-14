// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2015.

#include "FMODFileCallbacks.h"
#include "fmod_errors.h"
#include "FMODUtils.h"
#include "HAL/FileManager.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "Misc/ScopeLock.h"
#include "FMODStudioPrivatePCH.h"

FMOD_RESULT F_CALLBACK FMODLogCallback(FMOD_DEBUG_FLAGS flags, const char *file, int line, const char *func, const char *message)
{
    if (flags & FMOD_DEBUG_LEVEL_ERROR)
    {
        UE_LOG(LogFMOD, Error, TEXT("%s(%d) - %s"), UTF8_TO_TCHAR(file), line, UTF8_TO_TCHAR(message));
    }
    else if (flags & FMOD_DEBUG_LEVEL_WARNING)
    {
        FString Message = UTF8_TO_TCHAR(message);
        UE_LOG(LogFMOD, Warning, TEXT("%s(%d) - %s"), UTF8_TO_TCHAR(file), line, *Message);
        if (GIsEditor)
        {
            int32 StartIndex = Message.Find(TEXT("Missing DSP plugin '"));
            if (StartIndex != INDEX_NONE)
            {
                int32 Len = FString(TEXT("Missing DSP plugin '")).Len();
                int32 EndIndex;
                if (Message.FindLastChar('\'', EndIndex) && EndIndex != INDEX_NONE && StartIndex + Len < EndIndex)
                {
                    FString PluginName = Message.Mid(StartIndex + Len, EndIndex - StartIndex - Len);

                    FModuleManager::GetModuleChecked<IFMODStudioModule>("FMODStudio").AddRequiredPlugin(PluginName);
                }
            }
        }
    }
    else if (flags & FMOD_DEBUG_LEVEL_LOG)
    {
        UE_LOG(LogFMOD, Log, TEXT("%s(%d) - %s"), UTF8_TO_TCHAR(file), line, UTF8_TO_TCHAR(message));
    }
    return FMOD_OK;
}

FMOD_RESULT F_CALLBACK FMODErrorCallback(FMOD_SYSTEM *system, FMOD_SYSTEM_CALLBACK_TYPE type, void *commanddata1, void* commanddata2, void *userdata)
{
    FMOD_ERRORCALLBACK_INFO *callbackInfo = (FMOD_ERRORCALLBACK_INFO *)commanddata1;

    if ((callbackInfo->instancetype == FMOD_ERRORCALLBACK_INSTANCETYPE_CHANNEL || callbackInfo->instancetype == FMOD_ERRORCALLBACK_INSTANCETYPE_CHANNELCONTROL) 
        && callbackInfo->result == FMOD_ERR_INVALID_HANDLE)
    {
        return FMOD_OK;
    }

    UE_LOG(LogFMOD, Error, TEXT("%s(%s) returned error %d (\"%s\") for instance type: %d (0x%d)."),
        UTF8_TO_TCHAR(callbackInfo->functionname), UTF8_TO_TCHAR(callbackInfo->functionparams), (int)callbackInfo->result,
        UTF8_TO_TCHAR(FMOD_ErrorString(callbackInfo->result)), (int)callbackInfo->instancetype, callbackInfo->instance);
    return FMOD_OK;
}

class FFMODFileSystem : public FRunnable
{
public:
    FFMODFileSystem()
        : mHandleIn(nullptr)
        , mName(nullptr)
        , mFileSize(nullptr)
        , mHandleOut(nullptr)
        , mBuffer(nullptr)
        , mSizeBytes(0)
        , mBytesRead(nullptr)
        , mSeekPosition(0)
        , mCommand(COMMAND_MAX)
        , mResult(FMOD_OK)
        , mReferenceCount(0)
        , mThread(nullptr)
        , mCommandReadyEvent(nullptr)
        , mCommandCompleteEvent(nullptr)
    {
    }

    static FMOD_RESULT F_CALLBACK OpenCallback(const char *name, unsigned int *filesize, void **handle, void * /*userdata*/);
    static FMOD_RESULT F_CALLBACK CloseCallback(void *handle, void * /*userdata*/);
    static FMOD_RESULT F_CALLBACK ReadCallback(void *handle, void *buffer, unsigned int sizebytes, unsigned int *bytesread, void * /*userdata*/);
    static FMOD_RESULT F_CALLBACK SeekCallback(void *handle, unsigned int pos, void * /*userdata*/);

    static FMOD_RESULT OpenInternal(const char *name, unsigned int *filesize, void **handle);
    static FMOD_RESULT CloseInternal(void *handle);
    static FMOD_RESULT ReadInternal(void *handle, void *buffer, unsigned int sizebytes, unsigned int *bytesread);
    static FMOD_RESULT SeekInternal(void *handle, unsigned int pos);

    void IncrementReferenceCount()
    {
        FScopeLock lock(&mCrit);

        ++mReferenceCount;

        if (mReferenceCount == 1)
        {
            check(!mThread);

            mCommandReadyEvent = FGenericPlatformProcess::GetSynchEventFromPool();
            mCommandCompleteEvent = FGenericPlatformProcess::GetSynchEventFromPool();
            mThread = FRunnableThread::Create(this, TEXT("FMOD File Access Coordinator"));
        }
    }

    void DecrementReferenceCount()
    {
        FScopeLock lock(&mCrit);

        check(mReferenceCount > 0);
        check(mThread);

        --mReferenceCount;

        if (mReferenceCount == 0)
        {
            verifyfmod(RunCommand(COMMAND_STOP));
            mThread->WaitForCompletion();

            FGenericPlatformProcess::ReturnSynchEventToPool(mCommandReadyEvent);
            mCommandReadyEvent = nullptr;
            FGenericPlatformProcess::ReturnSynchEventToPool(mCommandCompleteEvent);
            mCommandCompleteEvent = nullptr;
            delete mThread;
            mThread = nullptr;
        }
    }

    void Attach(FMOD::System *system, int32 fileBufferSize)
    {
        check(mThread);

        verifyfmod(system->setFileSystem(OpenCallback, CloseCallback, ReadCallback, SeekCallback, 0, 0, fileBufferSize));
    }

    uint32 Run() override
    {
        bool stopRequested = false;

        while (!stopRequested)
        {
            mCommandReadyEvent->Wait();

            switch (mCommand)
            {
                case COMMAND_OPEN:
                    mResult = OpenInternal(mName, mFileSize, mHandleOut);
                    break;
                case COMMAND_CLOSE:
                    mResult = CloseInternal(mHandleIn);
                    break;
                case COMMAND_READ:
                    mResult = ReadInternal(mHandleIn, mBuffer, mSizeBytes, mBytesRead);
                    break;
                case COMMAND_SEEK:
                    mResult = SeekInternal(mHandleIn, mSeekPosition);
                    break;
                case COMMAND_STOP:
                    stopRequested = true;
                    mResult = FMOD_OK;
                    break;
            }

            mCommandCompleteEvent->Trigger();
        }

        return 0;
    }

private:
    enum Command
    {
        COMMAND_OPEN,
        COMMAND_CLOSE,
        COMMAND_READ,
        COMMAND_SEEK,
        COMMAND_STOP,
        COMMAND_MAX,
    };

    FMOD_RESULT RunCommand(Command command)
    {
        check(mThread);

        mCommand = command;
        mCommandReadyEvent->Trigger();
        mCommandCompleteEvent->Wait();

        return mResult;
    }

    // Parameter for Close, Seek and Read
    void *mHandleIn;

    // Parameters for Open
    const char *mName;
    unsigned int *mFileSize;
    void **mHandleOut;

    // Parameters for Read
    void *mBuffer;
    unsigned int mSizeBytes;
    unsigned int *mBytesRead;

    // Parameter for Seek
    unsigned int mSeekPosition;

    Command mCommand;
    FMOD_RESULT mResult;

    int mReferenceCount;
    FRunnableThread *mThread;
    FEvent *mCommandReadyEvent;
    FEvent *mCommandCompleteEvent;

    FCriticalSection mCrit;
};

static FFMODFileSystem gFileSystem;

FMOD_RESULT F_CALLBACK FFMODFileSystem::OpenCallback(const char *name, unsigned int *filesize, void **handle, void * /*userdata*/)
{
    FScopeLock lock(&gFileSystem.mCrit);
    gFileSystem.mName = name;
    gFileSystem.mFileSize = filesize;
    gFileSystem.mHandleOut = handle;

    return gFileSystem.RunCommand(COMMAND_OPEN);
}

FMOD_RESULT FFMODFileSystem::OpenInternal(const char *name, unsigned int *filesize, void **handle)
{
    if (name)
    {
        FArchive *Archive = IFileManager::Get().CreateFileReader(UTF8_TO_TCHAR(name));
        UE_LOG(LogFMOD, Verbose, TEXT("FFMODFileSystem::OpenInternal opening '%s' returned archive %p"), UTF8_TO_TCHAR(name), Archive);
        if (!Archive)
        {
            return FMOD_ERR_FILE_NOTFOUND;
        }
        *filesize = Archive->TotalSize();
        *handle = Archive;
        UE_LOG(LogFMOD, Verbose, TEXT("  TotalSize = %d"), *filesize);
    }

    return FMOD_OK;
}

FMOD_RESULT F_CALLBACK FFMODFileSystem::CloseCallback(void *handle, void * /*userdata*/)
{
    FScopeLock lock(&gFileSystem.mCrit);
    gFileSystem.mHandleIn = handle;

    return gFileSystem.RunCommand(COMMAND_CLOSE);
}

FMOD_RESULT FFMODFileSystem::CloseInternal(void *handle)
{
    if (!handle)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    FArchive *Archive = (FArchive *)handle;
    UE_LOG(LogFMOD, Verbose, TEXT("FFMODFileSystem::CloseCallback closing archive %p"), Archive);
    delete Archive;

    return FMOD_OK;
}

FMOD_RESULT F_CALLBACK FFMODFileSystem::ReadCallback(void *handle, void *buffer, unsigned int sizebytes, unsigned int *bytesread, void * /*userdata*/)
{
    FScopeLock lock(&gFileSystem.mCrit);
    gFileSystem.mHandleIn = handle;
    gFileSystem.mBuffer = buffer;
    gFileSystem.mSizeBytes = sizebytes;
    gFileSystem.mBytesRead = bytesread;

    return gFileSystem.RunCommand(COMMAND_READ);
}

FMOD_RESULT FFMODFileSystem::ReadInternal(void *handle, void *buffer, unsigned int sizebytes, unsigned int *bytesread)
{
    if (!handle)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (bytesread)
    {
        FArchive *Archive = (FArchive *)handle;

        int64 BytesLeft = Archive->TotalSize() - Archive->Tell();
        int64 ReadAmount = FMath::Min((int64)sizebytes, BytesLeft);

        Archive->Serialize(buffer, ReadAmount);
        *bytesread = (unsigned int)ReadAmount;
        if (ReadAmount < (int64)sizebytes)
        {
            UE_LOG(LogFMOD, Verbose, TEXT(" -> EOF "));
            return FMOD_ERR_FILE_EOF;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT F_CALLBACK FFMODFileSystem::SeekCallback(void *handle, unsigned int pos, void * /*userdata*/)
{
    FScopeLock lock(&gFileSystem.mCrit);
    gFileSystem.mHandleIn = handle;
    gFileSystem.mSeekPosition = pos;

    return gFileSystem.RunCommand(COMMAND_SEEK);
}

FMOD_RESULT FFMODFileSystem::SeekInternal(void *handle, unsigned int pos)
{
    if (!handle)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    FArchive *Archive = (FArchive *)handle;
    Archive->Seek(pos);

    return FMOD_OK;
}

void AcquireFMODFileSystem()
{
    gFileSystem.IncrementReferenceCount();
}

void ReleaseFMODFileSystem()
{
    gFileSystem.DecrementReferenceCount();
}

void AttachFMODFileSystem(FMOD::System *system, int32 fileBufferSize)
{
    gFileSystem.Attach(system, fileBufferSize);
}
