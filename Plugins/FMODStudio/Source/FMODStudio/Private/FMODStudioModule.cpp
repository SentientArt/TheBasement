// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#include "FMODStudioModule.h"
#include "FMODSettings.h"
#include "FMODAudioComponent.h"
#include "FMODBlueprintStatics.h"
#include "FMODAssetTable.h"
#include "FMODFileCallbacks.h"
#include "FMODUtils.h"
#include "FMODEvent.h"
#include "FMODListener.h"
#include "FMODSnapshotReverb.h"

#include "FMODAudioLinkModule.h"
#if WITH_EDITOR
#include "FMODAudioLinkEditorModule.h"
#endif

#include "Async/Async.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/App.h"
#include "Misc/CommandLine.h"
#include "Misc/CoreDelegates.h"
#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/PlayerController.h"
#include "Containers/Ticker.h"
#include "Misc/Paths.h"
#include "Runtime/Media/Public/IMediaClock.h"
#include "Runtime/Media/Public/IMediaClockSink.h"
#include "Runtime/Media/Public/IMediaModule.h"
#include "TimerManager.h"

#include "fmod_studio.hpp"
#include "fmod_errors.h"
#include "FMODStudioPrivatePCH.h"

#include <atomic>

#ifdef FMOD_PLATFORM_HEADER
#include "FMODPlatform.h"
#endif

#if PLATFORM_IOS || PLATFORM_TVOS
#include <AVFoundation/AVAudioSession.h>
#endif

#define LOCTEXT_NAMESPACE "FMODStudio"

DEFINE_LOG_CATEGORY(LogFMOD);

DECLARE_STATS_GROUP(TEXT("FMOD"), STATGROUP_FMOD, STATCAT_Advanced);
DECLARE_FLOAT_COUNTER_STAT(TEXT("FMOD CPU - Mixer"), STAT_FMOD_CPUMixer, STATGROUP_FMOD);
DECLARE_FLOAT_COUNTER_STAT(TEXT("FMOD CPU - Studio"), STAT_FMOD_CPUStudio, STATGROUP_FMOD);
DECLARE_MEMORY_STAT(TEXT("FMOD Memory - Current"), STAT_FMOD_Current_Memory, STATGROUP_FMOD);
DECLARE_MEMORY_STAT(TEXT("FMOD Memory - Max"), STAT_FMOD_Max_Memory, STATGROUP_FMOD);
DECLARE_DWORD_COUNTER_STAT(TEXT("FMOD Channels - Total"), STAT_FMOD_Total_Channels, STATGROUP_FMOD);
DECLARE_DWORD_COUNTER_STAT(TEXT("FMOD Channels - Real"), STAT_FMOD_Real_Channels, STATGROUP_FMOD);

const TCHAR *FMODSystemContextNames[EFMODSystemContext::Max] = {
    TEXT("Auditioning"), TEXT("Runtime"), TEXT("Editor"),
};

void *F_CALLBACK FMODMemoryAlloc(unsigned int size, FMOD_MEMORY_TYPE type, const char *sourcestr)
{
    return FMemory::Malloc(size);
}
void *F_CALLBACK FMODMemoryRealloc(void *ptr, unsigned int size, FMOD_MEMORY_TYPE type, const char *sourcestr)
{
    return FMemory::Realloc(ptr, size);
}
void F_CALLBACK FMODMemoryFree(void *ptr, FMOD_MEMORY_TYPE type, const char *sourcestr)
{
    FMemory::Free(ptr);
}

struct FFMODSnapshotEntry
{
    FFMODSnapshotEntry(UFMODSnapshotReverb *InSnapshot = nullptr, FMOD::Studio::EventInstance *InInstance = nullptr)
        : Snapshot(InSnapshot)
        , Instance(InInstance)
        , StartTime(0.0)
        , FadeDuration(0.0f)
        , FadeIntensityStart(0.0f)
        , FadeIntensityEnd(0.0f)
    {
    }

    float CurrentIntensity() const
    {
        double CurrentTime = FApp::GetCurrentTime();
        if (StartTime + FadeDuration <= CurrentTime)
        {
            return FadeIntensityEnd;
        }
        else
        {
            float Factor = (CurrentTime - StartTime) / FadeDuration;
            return FMath::Lerp<float>(FadeIntensityStart, FadeIntensityEnd, Factor);
        }
    }

    void FadeTo(float Target, float Duration)
    {
        float StartIntensity = CurrentIntensity();

        StartTime = FApp::GetCurrentTime();
        FadeDuration = Duration;
        FadeIntensityStart = StartIntensity;
        FadeIntensityEnd = Target;
    }

    UFMODSnapshotReverb *Snapshot;
    FMOD::Studio::EventInstance *Instance;
    double StartTime;
    float FadeDuration;
    float FadeIntensityStart;
    float FadeIntensityEnd;
};

class FFMODStudioSystemClockSink : public IMediaClockSink
{
public:
    DECLARE_DELEGATE(FUpdateListenerPosition);

    FFMODStudioSystemClockSink(FMOD::Studio::System *SystemIn)
        : System(SystemIn)
        , LastResult(FMOD_OK)
    {
    }

    virtual void TickRender(FTimespan DeltaTime, FTimespan Timecode) override
    {
        if (System)
        {
            if (UpdateListenerPosition.IsBound())
            {
                UpdateListenerPosition.Execute();
            }

            LastResult = System->update();
        }
    }

    void SetUpdateListenerPositionDelegate(FUpdateListenerPosition UpdateListenerPositionIn) { UpdateListenerPosition = UpdateListenerPositionIn; }

    void OnDestroyStudioSystem() { System = nullptr; }

    FMOD::Studio::System *System;
    FMOD_RESULT LastResult;
    FUpdateListenerPosition UpdateListenerPosition;
};

class FFMODStudioModule : public IFMODStudioModule
{
    TUniquePtr<FFMODAudioLinkModule> FMODAudioLinkModule;
#if WITH_EDITOR
    TUniquePtr<FFMODAudioLinkEditorModule> FMODAudioLinkEditorModule;
#endif
public:
    /** IModuleInterface implementation */
    FFMODStudioModule()
        : AuditioningInstance(nullptr)
        , ListenerCount(1)
        , bSimulating(false)
        , bIsInPIE(false)
        , bUseSound(true)
        , bListenerMoved(true)
        , bAllowLiveUpdate(true)
        , bBanksLoaded(false)
        , LowLevelLibHandle(nullptr)
        , StudioLibHandle(nullptr)
        , bMixerPaused(false)
        , MemPool(nullptr)
    {
        for (int i = 0; i < EFMODSystemContext::Max; ++i)
        {
            StudioSystem[i] = nullptr;
        }
    }

    void HandleApplicationWillDeactivate()
    {
        AsyncTask(ENamedThreads::GameThread, [&]() { SetSystemPaused(true); });
    }

    void HandleApplicationHasReactivated()
    {
#if PLATFORM_IOS || PLATFORM_TVOS
        ActivateAudioSession();
#endif

        AsyncTask(ENamedThreads::GameThread, [&]() { SetSystemPaused(false); });
    }

    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    FString GetDllPath(const TCHAR *ShortName, bool bExplicitPath, bool bUseLibPrefix);
    void *LoadDll(const TCHAR *ShortName);

    bool LoadLibraries();

    void LoadBanks(EFMODSystemContext::Type Type);
    void UnloadBanks(EFMODSystemContext::Type Type);

#if WITH_EDITOR
    void ReloadBanks();
    void LoadEditorBanks();
    void UnloadEditorBanks();
#endif

    void CreateStudioSystem(EFMODSystemContext::Type Type);
    void DestroyStudioSystem(EFMODSystemContext::Type Type);

    bool Tick(float DeltaTime);

    void UpdateListeners();
    void UpdateWorldListeners(UWorld *World, int *ListenerIndex);

    virtual FMOD::Studio::System *GetStudioSystem(EFMODSystemContext::Type Context) override;
    virtual FMOD::Studio::EventDescription *GetEventDescription(const UFMODEvent *Event, EFMODSystemContext::Type Type) override;
    virtual FMOD::Studio::EventInstance *CreateAuditioningInstance(const UFMODEvent *Event) override;
    virtual void StopAuditioningInstance() override;

    virtual void SetListenerPosition(int ListenerIndex, UWorld *World, const FTransform &ListenerTransform, float DeltaSeconds) override;
    virtual void FinishSetListenerPosition(int ListenerCount) override;

    virtual const FFMODListener &GetNearestListener(const FVector &Location) override;

    virtual bool HasListenerMoved() override;

    virtual void SetSystemPaused(bool paused) override;

    virtual void SetInPIE(bool bInPIE, bool simulating) override;

    virtual UFMODAsset *FindAssetByName(const FString &Name) override;
    virtual UFMODEvent *FindEventByName(const FString &Name) override;
    virtual FString GetBankPath(const UFMODBank &Bank) override;
    virtual void GetAllBankPaths(TArray<FString> &Paths, bool IncludeMasterBank) const override;

    virtual TArray<FString> GetFailedBankLoads(EFMODSystemContext::Type Context) override { return FailedBankLoads[Context]; }

    virtual TArray<FString> GetRequiredPlugins() override { return RequiredPlugins; }

    virtual void AddRequiredPlugin(const FString &Plugin)
    {
        if (!RequiredPlugins.Contains(Plugin))
        {
            RequiredPlugins.Add(Plugin);
        }
    }

    virtual bool UseSound() override { return bUseSound; }

    virtual bool LoadPlugin(EFMODSystemContext::Type Context, const TCHAR *ShortName) override;

    virtual void LogError(int result, const char *function) override;

    virtual bool AreBanksLoaded() override;

    virtual bool SetLocale(const FString& Locale) override;

    virtual FString GetLocale() override;

    virtual FString GetDefaultLocale() override;

    void ResetInterpolation();

#if PLATFORM_IOS || PLATFORM_TVOS
    void InitializeAudioSession();
    void ActivateAudioSession();
#endif

    /** The studio system handle. */
    FMOD::Studio::System *StudioSystem[EFMODSystemContext::Max];
    FMOD::Studio::EventInstance *AuditioningInstance;

    /** The delegate to be invoked when this profiler manager ticks. */
    FTickerDelegate OnTick;

    /** IMediaClockSink wrappers for Studio Systems */
    TSharedPtr<FFMODStudioSystemClockSink, ESPMode::ThreadSafe> ClockSinks[EFMODSystemContext::Max];

    /** Handle for registered TickDelegate. */
    FTSTicker::FDelegateHandle TickDelegateHandle;

    /** Table of assets with name and guid */
    FFMODAssetTable AssetTable;

    /** List of failed bank files */
    TArray<FString> FailedBankLoads[EFMODSystemContext::Max];

    /** List of required plugins we found when loading banks. */
    TArray<FString> RequiredPlugins;

/** Listener information */
#if FMOD_VERSION >= 0x00010600
    static const int MAX_LISTENERS = FMOD_MAX_LISTENERS;
#else
    static const int MAX_LISTENERS = 1;
#endif
    FFMODListener Listeners[MAX_LISTENERS];
    int ListenerCount;

    /** Current snapshot applied via reverb zones*/
    TArray<FFMODSnapshotEntry> ReverbSnapshots;

    /** True if simulating */
    bool bSimulating;

    /** True if in PIE */
    bool bIsInPIE;

    /** True if we want sound enabled */
    bool bUseSound;

    /** True if we the listener has moved and may have changed audio settings*/
    bool bListenerMoved;

    /** True if we allow live update */
    bool bAllowLiveUpdate;

    bool bBanksLoaded;

    /** Dynamic library */
    FString BaseLibPath;
    void *LowLevelLibHandle;
    void *StudioLibHandle;

    /** True if the mixer has been paused by application deactivation */
    std::atomic<bool> bMixerPaused;

    /** You can also supply a pool of memory for FMOD to work with and it will do so with no extra calls to malloc or free. */
    void *MemPool;

    bool bLoadAllSampleData;
};

IMPLEMENT_MODULE(FFMODStudioModule, FMODStudio)

void FFMODStudioModule::LogError(int result, const char *function)
{
    FString ErrorStr(ANSI_TO_TCHAR(FMOD_ErrorString((FMOD_RESULT)result)));
    FString FunctionStr(ANSI_TO_TCHAR(function));
    UE_LOG(LogFMOD, Error, TEXT("'%s' returned '%s'"), *FunctionStr, *ErrorStr);
}

bool FFMODStudioModule::LoadPlugin(EFMODSystemContext::Type Context, const TCHAR *ShortName)
{
    UE_LOG(LogFMOD, Log, TEXT("Loading plugin '%s'"), ShortName);

    static const int ATTEMPT_COUNT = 2;
    static const TCHAR *AttemptPrefixes[ATTEMPT_COUNT] = {
        TEXT(""),
        TEXT("64")
    };

    FMOD::System *LowLevelSystem = nullptr;
    verifyfmod(StudioSystem[Context]->getCoreSystem(&LowLevelSystem));

    FMOD_RESULT PluginLoadResult;

    for (int useLib = 0; useLib < 2; ++useLib)
    {
        for (int attempt = 0; attempt < 2; ++attempt)
        {
            // Try to load combinations of suffix and lib prefix for relevant platforms
            FString AttemptName = FString(ShortName) + AttemptPrefixes[attempt];
            FString PluginPath = GetDllPath(*AttemptName, true, useLib != 0);

            UE_LOG(LogFMOD, Log, TEXT("Trying to load plugin file at location: %s"), *PluginPath);

            unsigned int Handle = 0;
            PluginLoadResult = LowLevelSystem->loadPlugin(TCHAR_TO_UTF8(*PluginPath), &Handle, 0);
            if (PluginLoadResult == FMOD_OK)
            {
                UE_LOG(LogFMOD, Log, TEXT("Loaded plugin %s"), ShortName);
                return true;
            }
        }
    }
    UE_LOG(LogFMOD, Error, TEXT("Failed to load plugin '%s', sounds may not play"), ShortName);
    return false;
}

void *FFMODStudioModule::LoadDll(const TCHAR *ShortName)
{
    FString LibPath = GetDllPath(ShortName, false, true);

    void *Handle = nullptr;
    UE_LOG(LogFMOD, Log, TEXT("FFMODStudioModule::LoadDll: Loading %s"), *LibPath);
    // Unfortunately Unreal's platform loading code hasn't been implemented on all platforms so we wrap it
#ifdef FMOD_PLATFORM_LOAD_DLL
    Handle = FMODPlatformLoadDll(*LibPath);
#else
    Handle = FPlatformProcess::GetDllHandle(*LibPath);
#endif

#if WITH_EDITOR
    if (!Handle && !FApp::IsUnattended())
    {
        FString Message = TEXT("Couldn't load FMOD DLL ") + LibPath;
        FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, *Message, TEXT("Error"));
    }
#endif
    if (!Handle)
    {
        UE_LOG(LogFMOD, Error, TEXT("Failed to load FMOD DLL '%s', FMOD sounds will not play!"), *LibPath);
    }
    return Handle;
}

FString FFMODStudioModule::GetDllPath(const TCHAR *ShortName, bool bExplicitPath, bool bUseLibPrefix)
{
    const TCHAR *LibPrefixName = (bUseLibPrefix ? TEXT("lib") : TEXT(""));
#ifdef FMOD_PLATFORM_HEADER
    return FMODPlatform_GetDllPath(ShortName, bExplicitPath, bUseLibPrefix);
#elif PLATFORM_MAC
    return FString::Printf(TEXT("%s/Mac/%s%s.dylib"), *BaseLibPath, LibPrefixName, ShortName);
#elif PLATFORM_ANDROID
    return FString::Printf(TEXT("%s%s.so"), LibPrefixName, ShortName);
#elif PLATFORM_LINUX
    return FString::Printf(TEXT("%s%s.so"), LibPrefixName, ShortName);
#elif PLATFORM_WINDOWS
    return FString::Printf(TEXT("%s/Win64/%s.dll"), *BaseLibPath, ShortName);
#else
    UE_LOG(LogFMOD, Error, TEXT("Unsupported platform for dynamic libs"));
    return "";
#endif
}

bool FFMODStudioModule::LoadLibraries()
{
#if PLATFORM_IOS || PLATFORM_TVOS || PLATFORM_ANDROID || PLATFORM_LINUX || PLATFORM_MAC || defined(FMOD_DONT_LOAD_LIBRARIES)
    return true; // Nothing to do on those platforms
#else
    UE_LOG(LogFMOD, Verbose, TEXT("FFMODStudioModule::LoadLibraries"));

#if defined(FMODSTUDIO_LINK_DEBUG)
    FString ConfigName = TEXT("D");
#elif defined(FMODSTUDIO_LINK_LOGGING)
    FString ConfigName = TEXT("L");
#elif defined(FMODSTUDIO_LINK_RELEASE)
    FString ConfigName = TEXT("");
#else
#error FMODSTUDIO_LINK not defined
#endif

    FString LowLevelName = FString(TEXT("fmod")) + ConfigName;
    FString StudioName = FString(TEXT("fmodstudio")) + ConfigName;
    LowLevelLibHandle = LoadDll(*LowLevelName);
    StudioLibHandle = LoadDll(*StudioName);
    return (LowLevelLibHandle != nullptr && StudioLibHandle != nullptr);
#endif
}

void FFMODStudioModule::StartupModule()
{
    UE_LOG(LogFMOD, Log, TEXT("FFMODStudioModule startup"));
    BaseLibPath = IPluginManager::Get().FindPlugin(TEXT("FMODStudio"))->GetBaseDir() + TEXT("/Binaries");
    UE_LOG(LogFMOD, Log, TEXT("Lib path = '%s'"), *BaseLibPath);

    if (FParse::Param(FCommandLine::Get(), TEXT("nosound")) || FApp::IsBenchmarking() || IsRunningDedicatedServer() || IsRunningCommandlet())
    {
        bUseSound = false;
        UE_LOG(LogFMOD, Log, TEXT("Running in nosound mode"));
    }

    if (FParse::Param(FCommandLine::Get(), TEXT("noliveupdate")))
    {
        bAllowLiveUpdate = false;
    }

    if (LoadLibraries())
    {
        verifyfmod(FMOD::Debug_Initialize(FMOD_DEBUG_LEVEL_WARNING, FMOD_DEBUG_MODE_CALLBACK, FMODLogCallback));

        const UFMODSettings &Settings = *GetDefault<UFMODSettings>();

        int32 size = Settings.GetMemoryPoolSize();

        if (size == 0)
        {
#if defined(FMOD_PLATFORM_HEADER)
            size = FMODPlatform_MemoryPoolSize();
#elif PLATFORM_IOS || PLATFORM_TVOS || PLATFORM_ANDROID
            size = Settings.MemoryPoolSizes.Mobile;
#else
            size = Settings.MemoryPoolSizes.Desktop;
#endif
        }

        if (!GIsEditor && size > 0)
        {
            MemPool = FMemory::Malloc(size);
            verifyfmod(FMOD::Memory_Initialize(MemPool, size, nullptr, nullptr, nullptr));
        }
        else
        {
            verifyfmod(FMOD::Memory_Initialize(0, 0, FMODMemoryAlloc, FMODMemoryRealloc, FMODMemoryFree));
        }

#if defined(FMOD_PLATFORM_HEADER)
        verifyfmod(FMODPlatformSystemSetup());
#endif

        AcquireFMODFileSystem();

        if (GIsEditor)
        {
            AssetTable.Load();
            AssetTable.SetLocale(GetDefaultLocale());
            CreateStudioSystem(EFMODSystemContext::Auditioning);
            CreateStudioSystem(EFMODSystemContext::Editor);
        }
        else
        {
            SetInPIE(true, false);
        }

        // Load AudioLink module
        bool bFMODAudioLinkEnabled = Settings.bFMODAudioLinkEnabled;
        if (bFMODAudioLinkEnabled)
        {
            UE_LOG(LogFMOD, Log, TEXT("FFMODAudioLinkModule startup"));
            FMODAudioLinkModule = MakeUnique<FFMODAudioLinkModule>();
#if WITH_EDITOR
            UE_LOG(LogFMOD, Log, TEXT("FFMODAudioLinkEditorModule startup"));
            FMODAudioLinkEditorModule = MakeUnique<FFMODAudioLinkEditorModule>();
#endif
        }
    }

    OnTick = FTickerDelegate::CreateRaw(this, &FFMODStudioModule::Tick);
    TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(OnTick);
}

inline FMOD_SPEAKERMODE ConvertSpeakerMode(EFMODSpeakerMode::Type Mode)
{
    switch (Mode)
    {
        case EFMODSpeakerMode::Stereo:
            return FMOD_SPEAKERMODE_STEREO;
        case EFMODSpeakerMode::Surround_5_1:
            return FMOD_SPEAKERMODE_5POINT1;
        case EFMODSpeakerMode::Surround_7_1:
            return FMOD_SPEAKERMODE_7POINT1;
        case EFMODSpeakerMode::Surround_7_1_4:
            return FMOD_SPEAKERMODE_7POINT1POINT4;
        default:
            check(0);
            return FMOD_SPEAKERMODE_DEFAULT;
    };
}

inline FMOD_OUTPUTTYPE ConvertOutputType(EFMODOutput::Type output)
{
    switch (output)
    {
    case EFMODOutput::TYPE_AUTODETECT:
        return FMOD_OUTPUTTYPE_AUTODETECT;
    case EFMODOutput::TYPE_NOSOUND:
        return FMOD_OUTPUTTYPE_NOSOUND;
    case EFMODOutput::TYPE_WASAPI:
        return FMOD_OUTPUTTYPE_WASAPI;
    case EFMODOutput::TYPE_ASIO:
        return FMOD_OUTPUTTYPE_ASIO;
    case EFMODOutput::TYPE_PULSEAUDIO:
        return FMOD_OUTPUTTYPE_PULSEAUDIO;
    case EFMODOutput::TYPE_ALSA:
        return FMOD_OUTPUTTYPE_ALSA;
    case EFMODOutput::TYPE_COREAUDIO:
        return FMOD_OUTPUTTYPE_COREAUDIO;
    case EFMODOutput::TYPE_AUDIOTRACK:
        return FMOD_OUTPUTTYPE_AUDIOTRACK;
    case EFMODOutput::TYPE_OPENSL:
        return FMOD_OUTPUTTYPE_OPENSL;
    case EFMODOutput::TYPE_AUDIOOUT:
        return FMOD_OUTPUTTYPE_AUDIOOUT;
    case EFMODOutput::TYPE_AUDIO3D:
        return FMOD_OUTPUTTYPE_AUDIO3D;
    case EFMODOutput::TYPE_NNAUDIO:
        return FMOD_OUTPUTTYPE_NNAUDIO;
    case EFMODOutput::TYPE_WINSONIC:
        return FMOD_OUTPUTTYPE_WINSONIC;
    case EFMODOutput::TYPE_AAUDIO:
        return FMOD_OUTPUTTYPE_AAUDIO;
    default:
        return FMOD_OUTPUTTYPE_AUTODETECT;
    }
}

void FFMODStudioModule::CreateStudioSystem(EFMODSystemContext::Type Type)
{
    DestroyStudioSystem(Type);
    if (!bUseSound)
    {
        return;
    }

    UE_LOG(LogFMOD, Verbose, TEXT("CreateStudioSystem for context %s"), FMODSystemContextNames[Type]);

    const UFMODSettings &Settings = *GetDefault<UFMODSettings>();
    bLoadAllSampleData = Settings.bLoadAllSampleData;

    FMOD_STUDIO_INITFLAGS StudioInitFlags = FMOD_STUDIO_INIT_NORMAL;
    FMOD_INITFLAGS InitFlags = FMOD_INIT_NORMAL;

#if (defined(FMODSTUDIO_LINK_DEBUG) || defined(FMODSTUDIO_LINK_LOGGING))
    bool liveUpdateEnabledForType = ((Type == EFMODSystemContext::Auditioning) && Settings.bEnableEditorLiveUpdate) ||
                                    ((Type == EFMODSystemContext::Runtime) && Settings.bEnableLiveUpdate);
    if (liveUpdateEnabledForType && bAllowLiveUpdate)
    {
        UE_LOG(LogFMOD, Verbose, TEXT("Enabling live update"));
        StudioInitFlags |= FMOD_STUDIO_INIT_LIVEUPDATE;
    }

    if (Settings.bEnableMemoryTracking && Type == EFMODSystemContext::Runtime)
    {
        StudioInitFlags |= FMOD_STUDIO_INIT_MEMORY_TRACKING;
    }

#endif
    if (Type == EFMODSystemContext::Auditioning || Type == EFMODSystemContext::Editor)
    {
        StudioInitFlags |= FMOD_STUDIO_INIT_ALLOW_MISSING_PLUGINS;
    }

    verifyfmod(FMOD::Studio::System::create(&StudioSystem[Type]));
    FMOD::System *lowLevelSystem = nullptr;
    verifyfmod(StudioSystem[Type]->getCoreSystem(&lowLevelSystem));

    FTCHARToUTF8 WavWriterDestUTF8(*Settings.WavWriterPath);
    void *InitData = nullptr;
    FMOD_OUTPUTTYPE outputType;
    if (Type == EFMODSystemContext::Runtime && Settings.WavWriterPath.Len() > 0)
    {
        UE_LOG(LogFMOD, Log, TEXT("Running with Wav Writer: %s"), *Settings.WavWriterPath);
        outputType = FMOD_OUTPUTTYPE_WAVWRITER;
        InitData = (void *)WavWriterDestUTF8.Get();
    }
    else
    {
        outputType = ConvertOutputType(Settings.GetOutputType());
    }
    verifyfmod(lowLevelSystem->setOutput(outputType));

    int DriverIndex = 0;
    if (!Settings.InitialOutputDriverName.IsEmpty())
    {
        int DriverCount = 0;
        verifyfmod(lowLevelSystem->getNumDrivers(&DriverCount));
        for (int id = 0; id < DriverCount; ++id)
        {
            char DriverNameUTF8[256] = {};
            verifyfmod(lowLevelSystem->getDriverInfo(id, DriverNameUTF8, sizeof(DriverNameUTF8), 0, 0, 0, 0));
            FString DriverName(UTF8_TO_TCHAR(DriverNameUTF8));
            UE_LOG(LogFMOD, Log, TEXT("Driver %d: %s"), id, *DriverName);
            if (DriverName.Contains(Settings.InitialOutputDriverName))
            {
                UE_LOG(LogFMOD, Log, TEXT("Selected driver %d"), id);
                DriverIndex = id;
            }
        }
        verifyfmod(lowLevelSystem->setDriver(DriverIndex));
    }
    int SampleRate = Settings.GetSampleRate();

    if (Settings.bMatchHardwareSampleRate)
    {
        int DefaultSampleRate = 0;
        verifyfmod(lowLevelSystem->getSoftwareFormat(&DefaultSampleRate, 0, 0));
        int SystemSampleRate = 0;
        verifyfmod(lowLevelSystem->getDriverInfo(DriverIndex, nullptr, 0, nullptr, &SystemSampleRate, nullptr, nullptr));
        UE_LOG(LogFMOD, Log, TEXT("Default sample rate = %d"), DefaultSampleRate);
        UE_LOG(LogFMOD, Log, TEXT("System sample rate = %d"), SystemSampleRate);
        if (DefaultSampleRate >= 44100 && DefaultSampleRate <= 48000 && SystemSampleRate >= 44100 && SystemSampleRate <= 48000)
        {
            UE_LOG(LogFMOD, Log, TEXT("Matching system sample rate %d"), SystemSampleRate);
            SampleRate = SystemSampleRate;
        }
    }

    if (outputType == FMOD_OUTPUTTYPE_AUDIO3D || outputType == FMOD_OUTPUTTYPE_AUDIOOUT || outputType == FMOD_OUTPUTTYPE_WINSONIC)
    {
        SampleRate = 48000;
        UE_LOG(LogFMOD, Log, TEXT("Overriding SampleRate to 48KHz for 3D Audio."));
    }

    FMOD_SPEAKERMODE OutputMode = ConvertSpeakerMode(Settings.GetSpeakerMode());

    verifyfmod(lowLevelSystem->setSoftwareFormat(SampleRate, OutputMode, 0));
    verifyfmod(lowLevelSystem->setSoftwareChannels(Settings.GetRealChannelCount()));
    AttachFMODFileSystem(lowLevelSystem, Settings.FileBufferSize);

    if (Settings.DSPBufferLength > 0 && Settings.DSPBufferCount > 0)
    {
        verifyfmod(lowLevelSystem->setDSPBufferSize(Settings.DSPBufferLength, Settings.DSPBufferCount));
    }

    FMOD_ADVANCEDSETTINGS advSettings = { 0 };
    advSettings.cbSize = sizeof(FMOD_ADVANCEDSETTINGS);
    advSettings.vol0virtualvol = Settings.Vol0VirtualLevel;

    if (!Settings.SetCodecs(advSettings))
    {
#if defined(FMOD_PLATFORM_HEADER)
        FMODPlatform_SetRealChannelCount(&advSettings);
#elif PLATFORM_IOS || PLATFORM_TVOS || PLATFORM_ANDROID
        advSettings.maxFADPCMCodecs = Settings.RealChannelCount;
#else
        advSettings.maxVorbisCodecs = Settings.RealChannelCount;
#endif
    }

    if (Type == EFMODSystemContext::Runtime)
    {
        advSettings.profilePort = Settings.LiveUpdatePort;
    }
    else if (Type == EFMODSystemContext::Auditioning)
    {
        advSettings.profilePort = Settings.EditorLiveUpdatePort;
    }
    advSettings.randomSeed = FMath::Rand();
    verifyfmod(lowLevelSystem->setAdvancedSettings(&advSettings));

    if (Settings.bEnableAPIErrorLogging)
    {
        verifyfmod(lowLevelSystem->setCallback(FMODErrorCallback, FMOD_SYSTEM_CALLBACK_ERROR));
    }

    FMOD_STUDIO_ADVANCEDSETTINGS advStudioSettings = { 0 };
    advStudioSettings.cbsize = sizeof(advStudioSettings);
    advStudioSettings.studioupdateperiod = Settings.StudioUpdatePeriod;

    if (!Settings.StudioBankKey.IsEmpty())
    {
        advStudioSettings.encryptionkey = TCHAR_TO_UTF8(*Settings.StudioBankKey);
    }

    verifyfmod(StudioSystem[Type]->setAdvancedSettings(&advStudioSettings));

    verifyfmod(StudioSystem[Type]->initialize(Settings.TotalChannelCount, StudioInitFlags, InitFlags, InitData));

    for (FString PluginName : Settings.PluginFiles)
    {
        if (!PluginName.IsEmpty())
            LoadPlugin(Type, *PluginName);
    }

    if (Type == EFMODSystemContext::Runtime)
    {
        // Add interrupt callbacks for Mobile
#if PLATFORM_IOS || PLATFORM_TVOS
        InitializeAudioSession();
#endif

        FCoreDelegates::ApplicationWillDeactivateDelegate.AddRaw(this, &FFMODStudioModule::HandleApplicationWillDeactivate);
        FCoreDelegates::ApplicationHasReactivatedDelegate.AddRaw(this, &FFMODStudioModule::HandleApplicationHasReactivated);
    }

    IMediaModule *MediaModule = FModuleManager::LoadModulePtr<IMediaModule>("Media");

    if (MediaModule != nullptr)
    {
        ClockSinks[Type] = MakeShared<FFMODStudioSystemClockSink, ESPMode::ThreadSafe>(StudioSystem[Type]);

        if (Type == EFMODSystemContext::Runtime)
        {
            ClockSinks[Type]->SetUpdateListenerPositionDelegate(FFMODStudioSystemClockSink::FUpdateListenerPosition::CreateRaw(this, &FFMODStudioModule::UpdateListeners));
        }

        MediaModule->GetClock().AddSink(ClockSinks[Type].ToSharedRef());
    }
}

void FFMODStudioModule::DestroyStudioSystem(EFMODSystemContext::Type Type)
{
    UE_LOG(LogFMOD, Verbose, TEXT("DestroyStudioSystem for context %s"), FMODSystemContextNames[Type]);

    if (ClockSinks[Type].IsValid())
    {
        // Calling through the shared ptr enforces thread safety with the media clock
        ClockSinks[Type]->OnDestroyStudioSystem();

        IMediaModule *MediaModule = FModuleManager::LoadModulePtr<IMediaModule>("Media");

        if (MediaModule != nullptr)
        {
            MediaModule->GetClock().RemoveSink(ClockSinks[Type].ToSharedRef());
        }

        ClockSinks[Type].Reset();
    }

    UnloadBanks(Type);

    if (StudioSystem[Type])
    {
        verifyfmod(StudioSystem[Type]->release());
        StudioSystem[Type] = nullptr;
    }
}

void FFMODStudioModule::UnloadBanks(EFMODSystemContext::Type Type)
{
    if (StudioSystem[Type])
    {
        int bankCount;
        verifyfmod(StudioSystem[Type]->getBankCount(&bankCount));
        if (bankCount > 0)
        {
            TArray<FMOD::Studio::Bank*> bankArray;

            bankArray.SetNumUninitialized(bankCount, false);
            verifyfmod(StudioSystem[Type]->getBankList(bankArray.GetData(), bankCount, &bankCount));

            for (int i = 0; i < bankCount; i++)
            {
                verifyfmod(bankArray[i]->unload());
            }
        }
    }
}

bool FFMODStudioModule::Tick(float DeltaTime)
{
    if (ClockSinks[EFMODSystemContext::Auditioning].IsValid())
    {
        verifyfmod(ClockSinks[EFMODSystemContext::Auditioning]->LastResult);
    }
    if (ClockSinks[EFMODSystemContext::Runtime].IsValid())
    {
        FMOD_STUDIO_CPU_USAGE Usage = {};
        FMOD_CPU_USAGE UsageCore = {};
        StudioSystem[EFMODSystemContext::Runtime]->getCPUUsage(&Usage, &UsageCore);
        SET_FLOAT_STAT(STAT_FMOD_CPUMixer, UsageCore.dsp);
        SET_FLOAT_STAT(STAT_FMOD_CPUStudio, Usage.update);

        int currentAlloc, maxAlloc;
        FMOD::Memory_GetStats(&currentAlloc, &maxAlloc, false);
        SET_MEMORY_STAT(STAT_FMOD_Current_Memory, currentAlloc);
        SET_MEMORY_STAT(STAT_FMOD_Max_Memory, maxAlloc);

        int channels, realChannels;
        FMOD::System *lowlevel;
        StudioSystem[EFMODSystemContext::Runtime]->getCoreSystem(&lowlevel);
        lowlevel->getChannelsPlaying(&channels, &realChannels);
        SET_DWORD_STAT(STAT_FMOD_Real_Channels, realChannels);
        SET_DWORD_STAT(STAT_FMOD_Total_Channels, channels);

        verifyfmod(ClockSinks[EFMODSystemContext::Runtime]->LastResult);
    }
    if (ClockSinks[EFMODSystemContext::Editor].IsValid())
    {
        verifyfmod(ClockSinks[EFMODSystemContext::Editor]->LastResult);
    }
    return true;
}

void FFMODStudioModule::UpdateListeners()
{
    int ListenerIndex = 0;
    bListenerMoved = false;

#if WITH_EDITOR
    if (bSimulating)
    {
        return;
    }

    if (GEngine)
    {
        // Every PIE session has its own world and local player controller(s), iterate all of them
        for (auto ContextIt = GEngine->GetWorldContexts().CreateConstIterator(); ContextIt; ++ContextIt)
        {
            const FWorldContext &PieContext = *ContextIt;

            // We need to update the listener for all PIE worlds and all standalone game worlds. Since this code is only built WITH_EDITOR
            // EWorldType::Game means a standalone game world in this scope.

            if (PieContext.WorldType == EWorldType::PIE || PieContext.WorldType == EWorldType::Game)
            {
                if (PieContext.GameViewport)
                {
                    UpdateWorldListeners(PieContext.GameViewport->GetWorld(), &ListenerIndex);
                }
            }
        }
    }
#else
    if (GEngine && GEngine->GameViewport)
    {
        UpdateWorldListeners(GEngine->GameViewport->GetWorld(), &ListenerIndex);
    }
#endif

    FinishSetListenerPosition(ListenerIndex);
}

void FFMODStudioModule::UpdateWorldListeners(UWorld *World, int *ListenerIndex)
{
    if (!World)
    {
        return;
    }

    float DeltaSeconds = World->GetDeltaSeconds();

    for (auto Iterator = GEngine->GetLocalPlayerIterator(World); Iterator; ++Iterator)
    {
        ULocalPlayer *LocalPlayer = *Iterator;

        if (LocalPlayer && LocalPlayer->PlayerController)
        {
            APlayerController *PlayerController = LocalPlayer->PlayerController;
            FVector Location;
            FVector ProjFront;
            FVector ProjRight;
            PlayerController->GetAudioListenerPosition(Location, ProjFront, ProjRight);
            FVector ProjUp = FVector::CrossProduct(ProjFront, ProjRight);

            FTransform ListenerTransform(FRotationMatrix::MakeFromXY(ProjFront, ProjRight));
            ListenerTransform.SetTranslation(Location);
            ListenerTransform.NormalizeRotation();

            SetListenerPosition(*ListenerIndex, World, ListenerTransform, DeltaSeconds);

            (*ListenerIndex)++;
        }
    }
}

bool FFMODStudioModule::HasListenerMoved()
{
    return bListenerMoved;
}

void FFMODStudioModule::ResetInterpolation()
{
    for (FFMODListener &Listener : Listeners)
    {
        Listener = FFMODListener();
    }
}

const FFMODListener &FFMODStudioModule::GetNearestListener(const FVector &Location)
{
    float BestDistSq = FLT_MAX;
    int BestListener = 0;
    for (int i = 0; i < ListenerCount; ++i)
    {
        const float DistSq = FVector::DistSquared(Location, Listeners[i].Transform.GetTranslation());
        if (DistSq < BestDistSq)
        {
            BestListener = i;
            BestDistSq = DistSq;
        }
    }
    return Listeners[BestListener];
}

// Partially copied from FAudioDevice::SetListener
void FFMODStudioModule::SetListenerPosition(int ListenerIndex, UWorld *World, const FTransform &ListenerTransform, float DeltaSeconds)
{
    FMOD::Studio::System *System = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
    if (System && ListenerIndex < MAX_LISTENERS)
    {
        // Expand number of listeners dynamically
        if (ListenerIndex >= ListenerCount)
        {
            Listeners[ListenerIndex] = FFMODListener();
            ListenerCount = ListenerIndex + 1;
            verifyfmod(System->setNumListeners(ListenerCount));
        }

        FVector ListenerPos = ListenerTransform.GetTranslation();

        FInteriorSettings *InteriorSettings =
            (FInteriorSettings *)alloca(sizeof(FInteriorSettings)); // FinteriorSetting::FInteriorSettings() isn't exposed (possible UE4 bug???)
        AAudioVolume *Volume = World->GetAudioSettings(ListenerPos, NULL, InteriorSettings);

        Listeners[ListenerIndex].Velocity =
            DeltaSeconds > 0.f ? (ListenerTransform.GetTranslation() - Listeners[ListenerIndex].Transform.GetTranslation()) / DeltaSeconds :
                                 FVector::ZeroVector;

        Listeners[ListenerIndex].Transform = ListenerTransform;

        Listeners[ListenerIndex].ApplyInteriorSettings(Volume, *InteriorSettings);

        // We are using a direct copy of the inbuilt transforms but the directions come out wrong.
        // Several of the audio functions use GetFront() for right, so we do the same here.
        const FVector Up = Listeners[ListenerIndex].GetUp();
        const FVector Right = Listeners[ListenerIndex].GetFront();
        const FVector Forward = Right ^ Up;

        FMOD_3D_ATTRIBUTES Attributes = { { 0 } };
        Attributes.position = FMODUtils::ConvertWorldVector(ListenerPos);
        Attributes.forward = FMODUtils::ConvertUnitVector(Forward);
        Attributes.up = FMODUtils::ConvertUnitVector(Up);
        Attributes.velocity = FMODUtils::ConvertWorldVector(Listeners[ListenerIndex].Velocity);
        verifyfmod(System->setListenerAttributes(ListenerIndex, &Attributes));
        bListenerMoved = true;
    }
}

void FFMODStudioModule::FinishSetListenerPosition(int NumListeners)
{
    FMOD::Studio::System *System = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
    if (!System || NumListeners < 1)
    {
        return;
    }

    if (System && NumListeners < ListenerCount)
    {
        ListenerCount = NumListeners;
        verifyfmod(System->setNumListeners(ListenerCount));
    }

    for (int i = 0; i < ListenerCount; ++i)
    {
        Listeners[i].UpdateCurrentInteriorSettings();
    }

    // Apply a reverb snapshot from the listener position(s)
    TWeakObjectPtr<AAudioVolume> BestVolume = nullptr;
    for (int i = 0; i < ListenerCount; ++i)
    {
        AAudioVolume *CandidateVolume = Listeners[i].Volume;

        if (BestVolume == nullptr || (IsValid(CandidateVolume) && BestVolume.IsValid() && CandidateVolume->GetPriority() > BestVolume->GetPriority()))
        {
            BestVolume = CandidateVolume;
        }
    }
    UFMODSnapshotReverb *NewSnapshot = nullptr;

    if (BestVolume.IsValid() && BestVolume->GetReverbSettings().bApplyReverb)
    {
        NewSnapshot = Cast<UFMODSnapshotReverb>(BestVolume->GetReverbSettings().ReverbEffect);
    }

    if (NewSnapshot != nullptr)
    {
        FString NewSnapshotName = FMODUtils::LookupNameFromGuid(System, NewSnapshot->AssetGuid);
        UE_LOG(LogFMOD, Verbose, TEXT("Starting new snapshot '%s'"), *NewSnapshotName);

        // Try to steal old entry
        FFMODSnapshotEntry SnapshotEntry;
        int SnapshotEntryIndex = -1;
        for (int i = 0; i < ReverbSnapshots.Num(); ++i)
        {
            if (ReverbSnapshots[i].Snapshot == NewSnapshot)
            {
                UE_LOG(LogFMOD, Verbose, TEXT("Re-using old entry with intensity %f"), ReverbSnapshots[i].CurrentIntensity());
                SnapshotEntryIndex = i;
                break;
            }
        }
        // Create new instance
        if (SnapshotEntryIndex == -1)
        {
            UE_LOG(LogFMOD, Verbose, TEXT("Creating new instance"));

            FMOD::Studio::ID Guid = FMODUtils::ConvertGuid(NewSnapshot->AssetGuid);
            FMOD::Studio::EventInstance *NewInstance = nullptr;
            FMOD::Studio::EventDescription *EventDesc = nullptr;
            System->getEventByID(&Guid, &EventDesc);
            if (EventDesc)
            {
                EventDesc->createInstance(&NewInstance);
                if (NewInstance)
                {
                    NewInstance->setParameterByName("Intensity", 0.0f);
                    NewInstance->start();
                }
            }

            SnapshotEntryIndex = ReverbSnapshots.Num();
            ReverbSnapshots.Push(FFMODSnapshotEntry(NewSnapshot, NewInstance));
        }
        // Fade up
        if (ReverbSnapshots[SnapshotEntryIndex].FadeIntensityEnd == 0.0f)
        {
            ReverbSnapshots[SnapshotEntryIndex].FadeTo(BestVolume->GetReverbSettings().Volume, BestVolume->GetReverbSettings().FadeTime);
        }
    }
    // Fade out all other entries
    for (int i = 0; i < ReverbSnapshots.Num(); ++i)
    {
        UE_LOG(LogFMOD, Verbose, TEXT("Ramping intensity (%f,%f) -> %f"), ReverbSnapshots[i].FadeIntensityStart, ReverbSnapshots[i].FadeIntensityEnd,
            ReverbSnapshots[i].CurrentIntensity());
        ReverbSnapshots[i].Instance->setParameterByName("Intensity", 100.0f * ReverbSnapshots[i].CurrentIntensity());

        if (ReverbSnapshots[i].Snapshot != NewSnapshot)
        {
            // Start fading out if needed
            if (ReverbSnapshots[i].FadeIntensityEnd != 0.0f)
            {
                ReverbSnapshots[i].FadeTo(0.0f, ReverbSnapshots[i].FadeDuration);
            }
            // Finish fading out and remove
            else if (ReverbSnapshots[i].CurrentIntensity() == 0.0f)
            {
                UE_LOG(LogFMOD, Verbose, TEXT("Removing snapshot"));

                ReverbSnapshots[i].Instance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
                ReverbSnapshots[i].Instance->release();
                ReverbSnapshots.RemoveAt(i);
                --i; // removed entry, redo current index for next one
            }
        }
    }
}

void FFMODStudioModule::SetInPIE(bool bInPIE, bool simulating)
{
    bIsInPIE = bInPIE;
    bSimulating = simulating;
    bListenerMoved = true;
    ResetInterpolation();

    FMOD_DEBUG_FLAGS flags;

    if (bInPIE)
    {
        if (StudioSystem[EFMODSystemContext::Auditioning])
        {
            // We currently don't tear down auditioning system but we do stop the playing event.
            if (AuditioningInstance)
            {
                AuditioningInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
                AuditioningInstance = nullptr;
            }
            // Also make sure banks are finishing loading so they aren't grabbing file handles.
            StudioSystem[EFMODSystemContext::Auditioning]->flushCommands();
        }

        // TODO: Stop sounds for the Editor system? What should happen if the user previews a sequence with transport
        // controls then starts a PIE session? What does happen?

        AssetTable.Load();
        AssetTable.SetLocale(GetDefaultLocale());

        ListenerCount = 1;
        CreateStudioSystem(EFMODSystemContext::Runtime);
        LoadBanks(EFMODSystemContext::Runtime);

        const UFMODSettings &Settings = *GetDefault<UFMODSettings>();
        flags = Settings.LoggingLevel;
    }
    else
    {
        ReverbSnapshots.Reset();
        DestroyStudioSystem(EFMODSystemContext::Runtime);
        flags = FMOD_DEBUG_LEVEL_WARNING;
    }

    verifyfmod(FMOD::Debug_Initialize(flags, FMOD_DEBUG_MODE_CALLBACK, FMODLogCallback));

}

UFMODAsset *FFMODStudioModule::FindAssetByName(const FString &Name)
{
    return AssetTable.GetAssetByStudioPath(Name);
}

UFMODEvent *FFMODStudioModule::FindEventByName(const FString &Name)
{
    UFMODAsset *Asset = FindAssetByName(Name);
    return Cast<UFMODEvent>(Asset);
}

FString FFMODStudioModule::GetBankPath(const UFMODBank &Bank)
{
    FString BankPath = AssetTable.GetBankPath(Bank);

    if (!BankPath.IsEmpty())
    {
        const UFMODSettings &Settings = *GetDefault<UFMODSettings>();
        BankPath = Settings.GetFullBankPath() / BankPath;
    }

    return BankPath;
}

void FFMODStudioModule::GetAllBankPaths(TArray<FString> &Paths, bool IncludeMasterBank) const
{
    AssetTable.GetAllBankPaths(Paths, IncludeMasterBank);
}

void FFMODStudioModule::SetSystemPaused(bool paused)
{
    if (StudioSystem[EFMODSystemContext::Runtime])
    {
        bool expected = !paused;

        if (bMixerPaused.compare_exchange_strong(expected, paused))
        {
            FMOD::System *LowLevelSystem = nullptr;
            verifyfmod(StudioSystem[EFMODSystemContext::Runtime]->getCoreSystem(&LowLevelSystem));

            // Resume mixer before making calls for Android in particular
            if (!paused)
            {
                LowLevelSystem->mixerResume();
            }

            FMOD::ChannelGroup *MasterChannelGroup = nullptr;
            verifyfmod(LowLevelSystem->getMasterChannelGroup(&MasterChannelGroup));
            verifyfmod(MasterChannelGroup->setPaused(paused));

            if (paused)
            {
                LowLevelSystem->mixerSuspend();
            }
        }
    }
}

void FFMODStudioModule::ShutdownModule()
{
    UE_LOG(LogFMOD, Verbose, TEXT("FFMODStudioModule shutdown"));

    DestroyStudioSystem(EFMODSystemContext::Auditioning);
    DestroyStudioSystem(EFMODSystemContext::Runtime);
    DestroyStudioSystem(EFMODSystemContext::Editor);

    if (FMODAudioLinkModule)
    {
        FMODAudioLinkModule.Reset();
    }
#if WITH_EDITOR
    if (FMODAudioLinkEditorModule)
    {
        FMODAudioLinkEditorModule.Reset();
    }
#endif

    if (StudioLibHandle && LowLevelLibHandle)
    {
        ReleaseFMODFileSystem();
    }

    if (MemPool)
        FMemory::Free(MemPool);

    if (UObjectInitialized())
    {
        // Unregister tick function.
        FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
    }

    UE_LOG(LogFMOD, Verbose, TEXT("FFMODStudioModule unloading dynamic libraries"));
    if (StudioLibHandle)
    {
        FPlatformProcess::FreeDllHandle(StudioLibHandle);
        StudioLibHandle = nullptr;
    }
    if (LowLevelLibHandle)
    {
        FPlatformProcess::FreeDllHandle(LowLevelLibHandle);
        LowLevelLibHandle = nullptr;
    }
    UE_LOG(LogFMOD, Verbose, TEXT("FFMODStudioModule finished unloading"));
}

struct NamedBankEntry
{
    NamedBankEntry()
        : Bank(nullptr)
    {
    }
    NamedBankEntry(const FString &InName, FMOD::Studio::Bank *InBank, FMOD_RESULT InResult)
        : Name(InName)
        , Bank(InBank)
        , Result(InResult)
    {
    }

    FString Name;
    FMOD::Studio::Bank *Bank;
    FMOD_RESULT Result;
};

bool FFMODStudioModule::AreBanksLoaded()
{
    return bBanksLoaded;
}

bool FFMODStudioModule::SetLocale(const FString& LocaleName)
{
    const UFMODSettings &Settings = *GetDefault<UFMODSettings>();

    for (const FFMODProjectLocale& Locale : Settings.Locales)
    {
        if (Locale.LocaleName == LocaleName)
        {
            AssetTable.SetLocale(Locale.LocaleCode);
            return true;
        }
    }

    UE_LOG(LogFMOD, Error, TEXT("No project locale named '%s' has been defined."), *LocaleName);
    return false;
}

FString FFMODStudioModule::GetLocale()
{
    return AssetTable.GetLocale();
}

FString FFMODStudioModule::GetDefaultLocale()
{
    FString LocaleCode = "";
    const UFMODSettings& Settings = *GetDefault<UFMODSettings>();

    if (Settings.Locales.Num() > 0)
    {
        LocaleCode = Settings.Locales[0].LocaleCode;

        for (int32 i = 0; i < Settings.Locales.Num(); ++i)
        {
            if (Settings.Locales[i].bDefault)
            {
                LocaleCode = Settings.Locales[i].LocaleCode;
                break;
            }
        }
    }
    return LocaleCode;
}

void FFMODStudioModule::LoadBanks(EFMODSystemContext::Type Type)
{
    const UFMODSettings &Settings = *GetDefault<UFMODSettings>();

    FailedBankLoads[Type].Reset();
    if (Type == EFMODSystemContext::Auditioning || Type == EFMODSystemContext::Editor)
    {
        RequiredPlugins.Reset();
    }

    if (StudioSystem[Type] != nullptr && Settings.IsBankPathSet())
    {
        UE_LOG(LogFMOD, Verbose, TEXT("LoadBanks for context %s"), FMODSystemContextNames[Type]);

        /*
            Queue up all banks to load asynchronously then wait at the end.
        */
        bool bLoadAllBanks = ((Type == EFMODSystemContext::Auditioning) || (Type == EFMODSystemContext::Editor) || Settings.bLoadAllBanks);
        bool bLoadSampleData = ((Type == EFMODSystemContext::Runtime) && Settings.bLoadAllSampleData);
        bool bLockAllBuses = ((Type == EFMODSystemContext::Runtime) && Settings.bLockAllBuses);
        FMOD_STUDIO_LOAD_BANK_FLAGS BankFlags = (bLockAllBuses ? FMOD_STUDIO_LOAD_BANK_NORMAL : FMOD_STUDIO_LOAD_BANK_NONBLOCKING);
        FMOD_RESULT Result = FMOD_OK;
        TArray<NamedBankEntry> BankEntries;

        // Always load the master bank at startup
        FMOD::Studio::Bank *MasterBank = nullptr;

        if (AssetTable.GetMasterBankPath().IsEmpty())
        {
            FString MasterBankFilename = Settings.GetMasterBankFilename();
            UE_LOG(LogFMOD, Warning, TEXT("Master bank (%s) not found."), *MasterBankFilename);
            FailedBankLoads[Type].Add(FString::Printf(TEXT("Could not find master bank (%s). Check project settings."), *MasterBankFilename));
        }
        else
        {
            FString MasterBankPath = Settings.GetFullBankPath() / AssetTable.GetMasterBankPath();
            UE_LOG(LogFMOD, Verbose, TEXT("Loading master bank: %s"), *MasterBankPath);
            Result = StudioSystem[Type]->loadBankFile(TCHAR_TO_UTF8(*MasterBankPath), BankFlags, &MasterBank);
            BankEntries.Add(NamedBankEntry(MasterBankPath, MasterBank, Result));
        }

        if (Result == FMOD_OK && !AssetTable.GetMasterAssetsBankPath().IsEmpty())
        {
            FMOD::Studio::Bank *MasterAssetsBank = nullptr;
            FString MasterAssetsBankPath = Settings.GetFullBankPath() / AssetTable.GetMasterAssetsBankPath();
            if (FPaths::FileExists(MasterAssetsBankPath))
            {
                Result = StudioSystem[Type]->loadBankFile(TCHAR_TO_UTF8(*MasterAssetsBankPath), BankFlags, &MasterAssetsBank);
                BankEntries.Add(NamedBankEntry(MasterAssetsBankPath, MasterAssetsBank, Result));
            }
        }

        if (Result == FMOD_OK)
        {
            // Auditioning needs string bank to get back full paths from events
            // Runtime could do without it, but if we load it we can look up guids to names which is helpful
            if (MasterBank)
            {
                FString StringsBankPath = Settings.GetFullBankPath() / AssetTable.GetMasterStringsBankPath();
                UE_LOG(LogFMOD, Verbose, TEXT("Loading strings bank: %s"), *StringsBankPath);
                FMOD::Studio::Bank *StringsBank = nullptr;
                Result = StudioSystem[Type]->loadBankFile(TCHAR_TO_UTF8(*StringsBankPath), BankFlags, &StringsBank);
                BankEntries.Add(NamedBankEntry(StringsBankPath, StringsBank, Result));
            }

            // Optionally load all banks in the directory
            if (bLoadAllBanks)
            {
                UE_LOG(LogFMOD, Verbose, TEXT("Loading all banks"));
                TArray<FString> BankFiles;
                AssetTable.GetAllBankPaths(BankFiles, false);
                for (const FString &OtherFile : BankFiles)
                {
                    if (Settings.SkipLoadBankName.Len() && OtherFile.Contains(Settings.SkipLoadBankName))
                    {
                        UE_LOG(LogFMOD, Log, TEXT("Skipping bank: %s"), *OtherFile);
                        continue;
                    }
                    UE_LOG(LogFMOD, Log, TEXT("Loading bank: %s"), *OtherFile);

                    FMOD::Studio::Bank *OtherBank;
                    Result = StudioSystem[Type]->loadBankFile(TCHAR_TO_UTF8(*OtherFile), BankFlags, &OtherBank);
                    BankEntries.Add(NamedBankEntry(OtherFile, OtherBank, Result));
                }
            }

            // Optionally lock all buses to make sure they are created
            if (MasterBank && bLockAllBuses)
            {
                UE_LOG(LogFMOD, Verbose, TEXT("Locking all buses"));
                int BusCount = 0;
                verifyfmod(MasterBank->getBusCount(&BusCount));
                if (BusCount != 0)
                {
                    TArray<FMOD::Studio::Bus *> BusList;
                    BusList.AddZeroed(BusCount);
                    verifyfmod(MasterBank->getBusList(BusList.GetData(), BusCount, &BusCount));
                    BusList.SetNum(BusCount);
                    for (int BusIdx = 0; BusIdx < BusCount; ++BusIdx)
                    {
                        verifyfmod(BusList[BusIdx]->lockChannelGroup());
                    }
                }
            }
        }

        // Wait for all banks to load.
        StudioSystem[Type]->flushCommands();

        for (NamedBankEntry &Entry : BankEntries)
        {
            if (Entry.Result == FMOD_OK)
            {
                FMOD_STUDIO_LOADING_STATE BankLoadingState = FMOD_STUDIO_LOADING_STATE_ERROR;
                Entry.Result = Entry.Bank->getLoadingState(&BankLoadingState);
                if (BankLoadingState == FMOD_STUDIO_LOADING_STATE_ERROR)
                {
                    Entry.Bank->unload();
                    Entry.Bank = nullptr;
                }
                else if (bLoadSampleData)
                {
                    verifyfmod(Entry.Bank->loadSampleData());
                }
            }
            if (Entry.Bank == nullptr || Entry.Result != FMOD_OK)
            {
                FString ErrorMessage;
                if (!FPaths::FileExists(Entry.Name))
                {
                    ErrorMessage = "File does not exist";
                }
                else
                {
                    ErrorMessage = UTF8_TO_TCHAR(FMOD_ErrorString(Entry.Result));
                }
                UE_LOG(LogFMOD, Warning, TEXT("Failed to load bank: %s (%s)"), *Entry.Name, *ErrorMessage);
                FailedBankLoads[Type].Add(FString::Printf(TEXT("%s (%s)"), *FPaths::GetBaseFilename(Entry.Name), *ErrorMessage));
            }
        }
    }

    bBanksLoaded = true;
}

#if WITH_EDITOR
void FFMODStudioModule::ReloadBanks()
{
    UE_LOG(LogFMOD, Verbose, TEXT("Refreshing auditioning system"));

    StopAuditioningInstance();
    UnloadBanks(EFMODSystemContext::Auditioning);
    DestroyStudioSystem(EFMODSystemContext::Editor);

    AssetTable.Load();

    LoadBanks(EFMODSystemContext::Auditioning);
    CreateStudioSystem(EFMODSystemContext::Editor);
}

void FFMODStudioModule::LoadEditorBanks()
{
    LoadBanks(EFMODSystemContext::Editor);
}

void FFMODStudioModule::UnloadEditorBanks()
{
    UnloadBanks(EFMODSystemContext::Editor);
}
#endif

FMOD::Studio::System *FFMODStudioModule::GetStudioSystem(EFMODSystemContext::Type Context)
{
    if (Context == EFMODSystemContext::Max)
    {
        Context = (bIsInPIE ? EFMODSystemContext::Runtime : EFMODSystemContext::Auditioning);
    }
    return StudioSystem[Context];
}

FMOD::Studio::EventDescription *FFMODStudioModule::GetEventDescription(const UFMODEvent *Event, EFMODSystemContext::Type Context)
{
    if (Context == EFMODSystemContext::Max)
    {
        Context = (bIsInPIE ? EFMODSystemContext::Runtime : EFMODSystemContext::Auditioning);
    }
    if (StudioSystem[Context] != nullptr && IsValid(Event) && Event->AssetGuid.IsValid())
    {
        FMOD::Studio::ID Guid = FMODUtils::ConvertGuid(Event->AssetGuid);
        FMOD::Studio::EventDescription *EventDesc = nullptr;
        StudioSystem[Context]->getEventByID(&Guid, &EventDesc);
        return EventDesc;
    }
    return nullptr;
}

FMOD::Studio::EventInstance *FFMODStudioModule::CreateAuditioningInstance(const UFMODEvent *Event)
{
    StopAuditioningInstance();
    if (IsValid(Event))
    {
        FMOD::Studio::EventDescription *EventDesc = GetEventDescription(Event, EFMODSystemContext::Auditioning);
        if (EventDesc)
        {
            FMOD_RESULT Result = EventDesc->createInstance(&AuditioningInstance);
            if (Result == FMOD_OK)
            {
                return AuditioningInstance;
            }
        }
    }
    return nullptr;
}

void FFMODStudioModule::StopAuditioningInstance()
{
    if (AuditioningInstance)
    {
        // Don't bother checking for errors just in case auditioning is already shutting down
        AuditioningInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
        AuditioningInstance->release();
        AuditioningInstance = nullptr;
    }
}

#if PLATFORM_IOS || PLATFORM_TVOS
void FFMODStudioModule::InitializeAudioSession()
{
    [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionInterruptionNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
    {
        switch ([[notification.userInfo valueForKey:AVAudioSessionInterruptionTypeKey] unsignedIntegerValue])
        {
            case AVAudioSessionInterruptionTypeBegan:
            {
                if (@available(iOS 10.3, *))
                {
                    if ([[notification.userInfo valueForKey:AVAudioSessionInterruptionWasSuspendedKey] boolValue])
                    {
                        // If the system suspended the app process and deactivated the audio session then we get a delayed
                        // interruption notification when the app is re-activated. Just ignore that here.
                        return;
                    }
                }
                SetSystemPaused(true);
                break;
            }
            case AVAudioSessionInterruptionTypeEnded:
            {
                ActivateAudioSession();
                SetSystemPaused(false);
                break;
            }
        }
    }];

    [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationDidBecomeActiveNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
    {
    #if PLATFORM_TVOS
        SetSystemPaused(true);
    #endif
        ActivateAudioSession();
        SetSystemPaused(false);
    }];

    ActivateAudioSession();
}

void FFMODStudioModule::ActivateAudioSession()
{
    NSError* ActiveError = nil;
    [[AVAudioSession sharedInstance] setActive:TRUE error:&ActiveError];

    if (ActiveError)
    {
        UE_LOG(LogFMOD, Error, TEXT("Failed to set audio session to active = %d [Error = %s]"), TRUE, *FString([ActiveError description]));
    }
}
#endif

#undef LOCTEXT_NAMESPACE