// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#pragma once

#include "UObject/Class.h"
#include "Engine/EngineTypes.h"
#include "GenericPlatform/GenericPlatform.h"
#include "fmod_common.h"
#include "FMODSettings.generated.h"

class Paths;

UENUM()
enum EFMODLogging
{
    LEVEL_NONE = 0,
    LEVEL_ERROR = 1,
    LEVEL_WARNING = 2,
    LEVEL_LOG = 4
};

UENUM()
namespace EFMODSpeakerMode
{
    enum Type
    {
        // The speakers are stereo
        Stereo,
        // 5.1 speaker setup
        Surround_5_1,
        // 7.1 speaker setup
        Surround_7_1,
        // 7.1.4 speaker setup
        Surround_7_1_4
    };
}

UENUM()
namespace EFMODOutput
{
    enum Type
    {
        /** Picks the best output mode for the platform.This is the default. */
        TYPE_AUTODETECT,
        /** All - Perform all mixing but discard the final output. */
        TYPE_NOSOUND,
        /** Win / UWP / Xbox One / Game Core - Windows Audio Session API. (Default on Windows, Xbox One, Game Core and UWP) */
        TYPE_WASAPI,
        /** Win - Low latency ASIO 2.0. */
        TYPE_ASIO,
        /** Linux - Pulse Audio. (Default on Linux if available) */
        TYPE_PULSEAUDIO,
        /** Linux - Advanced Linux Sound Architecture. (Default on Linux if PulseAudio isn't available) */
        TYPE_ALSA,
        /** Mac / iOS - Core Audio. (Default on Mac and iOS) */
        TYPE_COREAUDIO,
        /** Android - Java Audio Track. (Default on Android 2.2 and below) */
        TYPE_AUDIOTRACK,
        /** Android - OpenSL ES. (Default on Android 2.3 up to 7.1) */
        TYPE_OPENSL,
        /** PS4 / PS5 - Audio Out. (Default on PS4, PS5) */
        TYPE_AUDIOOUT,
        /** PS4 - Audio3D. */
        TYPE_AUDIO3D,
        /** Switch - nn::audio. (Default on Switch) */
        TYPE_NNAUDIO,
        /** Win10 / Xbox One / Game Core - Windows Sonic. */
        TYPE_WINSONIC,
        /** Android - AAudio. (Default on Android 8.1 and above) */
        TYPE_AAUDIO,
    };
}

UENUM()
namespace EFMODPlatforms
{
    enum Type
    {
        Windows,
        Linux,
        Mac,
        Android,
        IOS,
        PS4,
        PS5,
        Deprecated,
        Switch,
        XboxOne,
        XSX,
        Editor
    };
}

UENUM()
namespace EFMODCodec
{
    enum Type
    {
        VORBIS,
        FADPCM,
        OPUS,
        XMA,
        AT9,
    };
}

USTRUCT()
struct FCustomPoolSizes
{
    GENERATED_USTRUCT_BODY()
    /** Default = 0 (Disabled) units in bytes*/
    UPROPERTY(config, EditAnywhere, Category = InitSettings, meta = (ClampMin = "0"))
    int32 Desktop;
    /** Default = 0 (Disabled) units in bytes*/
    UPROPERTY(config, EditAnywhere, Category = InitSettings, meta = (ClampMin = "0"))
    int32 Mobile;
    /** Default = 0 (Disabled) units in bytes*/
    UPROPERTY(config, EditAnywhere, Category = InitSettings, meta = (ClampMin = "0"))
    int32 PS4;
    /** Default = 0 (Disabled) units in bytes*/
    UPROPERTY(config, EditAnywhere, Category = InitSettings, meta = (ClampMin = "0"))
    int32 Switch;
    /** Default = 0 (Disabled) units in bytes*/
    UPROPERTY(config, EditAnywhere, Category = InitSettings, meta = (ClampMin = "0"))
    int32 XboxOne;
    FCustomPoolSizes()
        : Desktop(0)
        , Mobile(0)
        , PS4(0)
        , Switch(0)
        , XboxOne(0)
    {}
};

USTRUCT()
struct FFMODPlatformSettings
{
    GENERATED_USTRUCT_BODY()
    // Real Channel Count
    UPROPERTY(config, EditAnywhere, Category = PlatformSettings, meta = (ClampMin = "0"))
    int32 RealChannelCount;
    /**
    * Sample rate to use, or 0 to match system rate.
    * eg. 0, 22050, 24000, 32000, 44100, 48000.
    */
    UPROPERTY(config, EditAnywhere, Category = PlatformSettings, meta = (ClampMin = "0"))
    int32 SampleRate;
    /**
    * Project Output Format, should match the mode set up for the Studio project.
    */
    UPROPERTY(config, EditAnywhere, Category = PlatformSettings)
    TEnumAsByte<EFMODSpeakerMode::Type> SpeakerMode;
    /**
    * Built-in output types that can be used to run the mixer.
    */
    UPROPERTY(config, EditAnywhere, Category = PlatformSettings)
    TEnumAsByte<EFMODOutput::Type> OutputType;
    /**
     * Use specified memory pool size, units in bytes. Disabled by default.
     * FMOD may become unstable if the limit is exceeded!
     */
    UPROPERTY(config, EditAnywhere, Category = PlatformSettings, meta = (ClampMin = "0"))
    int32 CustomPoolSize;
    /* Codecs
    */
    UPROPERTY(config, EditAnywhere, Category = PlatformSettings, meta = (ClampMin = "0"))
    TMap<TEnumAsByte<EFMODCodec::Type>, int32> Codecs;
    FFMODPlatformSettings()
        : RealChannelCount(64)
        , SampleRate(0)
        , SpeakerMode(EFMODSpeakerMode::Surround_5_1)
        , OutputType(EFMODOutput::TYPE_AUTODETECT)
        , CustomPoolSize(0)
    {}
};

USTRUCT()
struct FFMODProjectLocale
{
    GENERATED_USTRUCT_BODY()
    /**
    * Human readable locale name, displayed in Blueprints.
    */
    UPROPERTY(config, EditAnywhere, Category = Localization)
    FString LocaleName;
    /**
    * Locale code. Must correspond to project locale codes in FMOD Studio project.
    */
    UPROPERTY(config, EditAnywhere, Category = Localization)
    FString LocaleCode;
    /**
    * Default locale at startup. Only one locale should be marked as default.
    */
    UPROPERTY(config, EditAnywhere, Category = Localization)
    bool bDefault;
    FFMODProjectLocale()
        : bDefault(false)
    {}
};

UCLASS(config = Engine, defaultconfig)
class FMODSTUDIO_API UFMODSettings : public UObject
{
    GENERATED_UCLASS_BODY()

    friend class SSettingsMessage;
    friend class FFMODStudioEditorModule;
    friend class FFMODAssetTable;
    friend class FFMODStudioModule;
    friend class FFMODAssetBuilder;
    friend class UFMODGenerateAssetsCommandlet;

public:
    /**
     * Whether to load all banks at startup.
     */
    UPROPERTY(config, EditAnywhere, Category = Basic)
    bool bLoadAllBanks;

    /**
     * Whether to load all bank sample data into memory at startup.
     */
    UPROPERTY(config, EditAnywhere, Category = Basic)
    bool bLoadAllSampleData;

    /**
     * Enable live update in non-final builds.
     */
    UPROPERTY(config, EditAnywhere, Category = Basic)
    bool bEnableLiveUpdate;

    /**
    * Enable live update in Editor for Auditioning. *Requires Restart*
    */
    UPROPERTY(Config, EditAnywhere, Category = Basic, meta = (ConfigRestartRequired = true))
    bool bEnableEditorLiveUpdate;

    /**
     * Path to find your studio bank output directory, relative to Content directory.
     */
    UPROPERTY(config, EditAnywhere, Category = Basic, meta = (RelativeToGameContentDir))
    FDirectoryPath BankOutputDirectory;

    /** Project Output Format, should match the mode set up for the Studio project. */
    UPROPERTY(config, EditAnywhere, Category = Basic)
    TEnumAsByte<EFMODSpeakerMode::Type> OutputFormat;

    /**
    * Built-in output types that can be used to run the mixer.
    */
    UPROPERTY(config, EditAnywhere, Category = Basic)
    TEnumAsByte<EFMODOutput::Type> OutputType;

    /**
    * Locales for localized banks. These should match the project locales configured in the FMOD Studio project.
    */
    UPROPERTY(config, EditAnywhere, Category = Localization)
    TArray<FFMODProjectLocale> Locales;

    /**
     * The signal level at which channels are virtualized. Virtual channels are processed, but do not produce any output.
     */
    UPROPERTY(config, EditAnywhere, Category = InitSettings)
    float Vol0VirtualLevel;

    /**
     * Sample rate to use, or 0 to match system rate.
     * eg. 0, 22050, 24000, 32000, 44100, 48000.
     */
    UPROPERTY(config, EditAnywhere, Category = InitSettings)
    int32 SampleRate;

    /**
    * Whether to match hardware sample rate where reasonable (44.1kHz to 48kHz).
    */
    UPROPERTY(config, EditAnywhere, Category = InitSettings)
    bool bMatchHardwareSampleRate;

    /**
     * Number of actual software voices that can be used at once.
     */
    UPROPERTY(config, EditAnywhere, Category = InitSettings)
    int32 RealChannelCount;

    /**
     * Total number of voices available that can be either real or virtual.
     */
    UPROPERTY(config, EditAnywhere, Category = InitSettings)
    int32 TotalChannelCount;

    /**
     * DSP mixer buffer length (eg. 512, 1024) or 0 for system default.
     * When changing the Buffer Length, Buffer Count also needs to be set.
     */
    UPROPERTY(config, EditAnywhere, Category = InitSettings)
    int32 DSPBufferLength;

    /**
     * DSP mixer buffer count (eg. 2, 4) or 0 for system default.
     * When changing the Buffer Count, Buffer Length also needs to be set.
     */
    UPROPERTY(config, EditAnywhere, Category = InitSettings)
    int32 DSPBufferCount;

    /**
     * File buffer size in bytes (2048 by default).
     */
    UPROPERTY(config, EditAnywhere, Category = InitSettings)
    int32 FileBufferSize;

    /**
     * Studio update period in milliseconds, or 0 for default (which means 20ms).
     */
    UPROPERTY(config, EditAnywhere, Category = InitSettings)
    int32 StudioUpdatePeriod;

    /**
     * Output device to choose at system start up, or empty for default.
     */
    UPROPERTY(config, EditAnywhere, Category = InitSettings)
    FString InitialOutputDriverName;

    /**
     * Lock all mixer buses at startup, making sure they are created up front.
     */
    UPROPERTY(config, EditAnywhere, Category = InitSettings)
    bool bLockAllBuses;

    /**
     * Use specified memory pool size for platform, units in bytes. Disabled by default.
     * FMOD may become unstable if the limit is exceeded!
     */
    UPROPERTY(config, EditAnywhere, Category = InitSettings)
    FCustomPoolSizes MemoryPoolSizes;

    /**
     * Live update port to use, or 0 for default.
     */
    UPROPERTY(config, EditAnywhere, Category = Advanced, meta = (EditCondition = "bEnableLiveUpdate"))
    int32 LiveUpdatePort;

    /**
    * Live update port to use while in editor for auditioning. *Requires Restart*
    */
    UPROPERTY(config, EditAnywhere, Category = Advanced, meta = (EditCondition = "bEnableEditorLiveUpdate", ConfigRestartRequired = true))
    int32 EditorLiveUpdatePort;

    /**
    * Delay in seconds before automatically reloading modified banks from disk. This can be extended if building
    * banks takes a long time and UE4 tries to reload banks before building is completed.
    * Set to 0 to disable automatic bank reloading.
    */
    UPROPERTY(config, EditAnywhere, Category = Advanced)
    int32 ReloadBanksDelay;

    /**
     * Will log internal API errors when enabled.
     */
    UPROPERTY(config, EditAnywhere, Category = Advanced, meta = (DisplayName = "Enable API Error Logging"))
    bool bEnableAPIErrorLogging;

    /**
    * Enable memory tracking.
    */
    UPROPERTY(config, EditAnywhere, Category = Advanced)
    bool bEnableMemoryTracking;

    /**
     * Extra plugin files to load.
     * The plugin files should sit alongside the FMOD dynamic libraries in the ThirdParty directory.
     */
    UPROPERTY(config, EditAnywhere, Category = Advanced)
    TArray<FString> PluginFiles;

    /**
     * Directory for content to appear in content window. Be careful changing this!
     */
    UPROPERTY(config, EditAnywhere, Category = Advanced)
    FString ContentBrowserPrefix;

    /**
     * Force platform directory name, or leave empty for automatic (Desktop/Mobile/PS4/XBoxOne)
     */
    UPROPERTY(config, EditAnywhere, Category = Advanced)
    FString ForcePlatformName;

    /**
     * Name of master bank.  The default in Studio is "Master Bank".
     */
    UPROPERTY(config, EditAnywhere, Category = Advanced)
    FString MasterBankName;

    /**
     * Skip bank files of the given name.
     * Can be used to load all banks except for a certain set, such as localization banks.
     */
    UPROPERTY(config, EditAnywhere, Category = Advanced)
    FString SkipLoadBankName;

    /*
    * Specify the key for loading sounds from encrypted banks.
    */
    UPROPERTY(config, EditAnywhere, Category = Advanced, meta = (DisplayName = "Encryption Key"))
    FString StudioBankKey;

    /**
    * Force wav writer output, for debugging only.  Setting this will prevent normal sound output!
    */
    UPROPERTY(config, EditAnywhere, Category = Advanced)
    FString WavWriterPath;

    /*
    * Specify the logging level to use in a debug/development build.
    */
    UPROPERTY(config, EditAnywhere, Category = Advanced)
    TEnumAsByte<EFMODLogging> LoggingLevel;

    /**
    * Name of the parameter used in Studio to control Occlusion effects.
    */
    UPROPERTY(config, EditAnywhere, Category = Advanced)
    FString OcclusionParameter;

    /**
    * Name of the parameter used in Studio to control Ambient volume.
    */
    UPROPERTY(config, EditAnywhere, Category = Advanced)
    FString AmbientVolumeParameter;

    /**
    * Name of the parameter used in Studio to control Ambient LPF effects.
    */
    UPROPERTY(config, EditAnywhere, Category = Advanced)
    FString AmbientLPFParameter;

    /**
    * Enables/Disables the FMODAudioLink modules.
    */
    UPROPERTY(config, EditAnywhere, Category = Advanced, meta = (ConfigRestartRequired=true))
    bool bFMODAudioLinkEnabled;

    /*
    * Used to specify platform specific settings.
    */
    UPROPERTY(config, EditAnywhere, Category = PlatformSettings)
    TMap<TEnumAsByte<EFMODPlatforms::Type>, FFMODPlatformSettings> Platforms;

private:
    /** Is the bank path set up . */
    bool IsBankPathSet() const { return !BankOutputDirectory.Path.IsEmpty(); }

    /** Get the full bank path.  Uses the game's content directory as a base. */
    FString GetFullBankPath() const;

    /** Get the master bank filename. */
    FString GetMasterBankFilename() const;

    /** Get the master assets bank filename. */
    FString GetMasterAssetsBankFilename() const;

    /** Get the master strings bank filename. */
    FString GetMasterStringsBankFilename() const;

    /** Get the speaker mode for the current platform. */
    EFMODSpeakerMode::Type GetSpeakerMode() const;

    /** Get the output type for the current platform. */
    EFMODOutput::Type GetOutputType() const;

    /** Get the sample rate for the current platform. */
    FString GetFullContentPath() const;

    /** Get the sample rate for the current platform. */
    int32 GetSampleRate() const;

    /** Get the custom memory pool size for the current platform. */
    int32 GetMemoryPoolSize() const;

    /** Get the real channel count for the current platform. */
    int32 GetRealChannelCount() const;

    /** Set the maximum codecs for the current platform. */
    bool SetCodecs(FMOD_ADVANCEDSETTINGS& advSettings) const;

    /** List of generated folder names that contain FMOD uassets. */
    TArray<FString> GeneratedFolders = {
        TEXT("Banks"),
        TEXT("Buses"),
        TEXT("Events"),
        TEXT("Reverbs"),
        TEXT("Snapshots"),
        TEXT("VCAs")
    };

#if WITH_EDITOR
    /** Get the path desktop banks. */
    FString GetDesktopBankPath() const;

    /** Check the settings for any configuration issues. */
    enum EProblem
    {
        Okay,
        BankPathNotSet,
        PackagingSettingsBad
    };

    EProblem Check() const;

    virtual void PostEditChangeProperty(FPropertyChangedEvent& e) override;
#endif // WITH_EDITOR
};