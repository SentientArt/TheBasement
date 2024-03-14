// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#include "FMODStudioEditorModule.h"
#include "FMODStudioModule.h"
#include "FMODStudioStyle.h"
#include "FMODAudioComponent.h"
#include "FMODAssetBroker.h"
#include "FMODSettings.h"
#include "FMODUtils.h"

#include "FMODEventEditor.h"
#include "FMODAudioComponentVisualizer.h"
#include "FMODAudioComponentDetails.h"
#include "FMODAssetBuilder.h"
#include "FMODBankUpdateNotifier.h"
#include "FMODSettingsCustomization.h"
#include "Sequencer/FMODChannelEditors.h"
#include "Sequencer/FMODEventControlSection.h"
#include "Sequencer/FMODEventControlTrackEditor.h"
#include "Sequencer/FMODEventParameterTrackEditor.h"
#include "AssetTypeActions_FMODEvent.h"

#include "Framework/Application/SlateApplication.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetSelection.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Developer/Settings/Public/ISettingsModule.h"
#include "Developer/Settings/Public/ISettingsSection.h"
#include "Editor.h"
#include "Slate/SceneViewport.h"
#include "Editor/LevelEditor/Public/LevelEditor.h"
#include "SocketSubsystem.h"
#include "Sockets.h"
#include "IPAddress.h"
#include "FileHelpers.h"
#include "ISequencerModule.h"
#include "SequencerChannelInterface.h"
#include "ClipboardTypes.h"
#include "DebugRenderSceneProxy.h"
#include "Debug/DebugDrawService.h"
#include "Settings/ProjectPackagingSettings.h"
#include "UnrealEdGlobals.h"
#include "LevelEditorViewport.h"
#include "ActorFactories/ActorFactory.h"
#include "Engine/Canvas.h"
#include "Editor/UnrealEdEngine.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Misc/MessageDialog.h"
#include "HAL/FileManager.h"
#include "Interfaces/IMainFrameModule.h"
#include "ToolMenus.h"

#include "fmod_studio.hpp"

#define LOCTEXT_NAMESPACE "FMODStudio"

DEFINE_LOG_CATEGORY(LogFMOD);

class FFMODStudioLink
{
public:
    FFMODStudioLink()
        : SocketSubsystem(nullptr)
        , Socket(nullptr)
    {
        SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    }

    ~FFMODStudioLink() { Disconnect(); }

    bool Connect()
    {
        if (!SocketSubsystem)
            return false;

        Disconnect();
        Socket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("FMOD Studio Connection"), false);

        TSharedRef<FInternetAddr> Addr = SocketSubsystem->CreateInternetAddr();
        bool Valid = false;
        Addr->SetIp(TEXT("127.0.0.1"), Valid);
        if (!Valid)
            return false;

        Addr->SetPort(3663);
        return Socket->Connect(*Addr);
    }

    void Disconnect()
    {
        if (SocketSubsystem && Socket)
        {
            SocketSubsystem->DestroySocket(Socket);
            Socket = nullptr;
        }
    }

    bool Execute(const TCHAR *Message, FString &OutMessage)
    {
        OutMessage = TEXT("");
        if (!Socket)
        {
            return false;
        }

        UE_LOG(LogFMOD, Log, TEXT("Sent studio message: %s"), Message);

        FTCHARToUTF8 MessageChars(Message);
        int32 BytesSent = 0;
        if (!Socket->Send((const uint8 *)MessageChars.Get(), MessageChars.Length(), BytesSent))
        {
            return false;
        }

        while (1)
        {
            FString BackMessage;
            if (!ReadMessage(BackMessage))
            {
                return false;
            }
            UE_LOG(LogFMOD, Log, TEXT("Received studio message: %s"), *BackMessage);
            if (BackMessage.StartsWith(TEXT("out(): ")))
            {
                OutMessage = BackMessage.Mid(7).TrimEnd();
                break;
            }
            else
            {
                // Keep going
            }
        }
        return true;
    }

private:
    bool ReadMessage(FString &OutMessage)
    {
        while (1)
        {
            for (int32 i = 0; i < ReceivedMessage.Num(); ++i)
            {
                if (ReceivedMessage[i] == '\0')
                {
                    OutMessage = FString(UTF8_TO_TCHAR(ReceivedMessage.GetData()));
                    ReceivedMessage.RemoveAt(0, i + 1);
                    return true;
                }
            }

            int32 ExtraSpace = 64;
            int32 CurrentSize = ReceivedMessage.Num();
            ReceivedMessage.SetNum(CurrentSize + ExtraSpace);
            int32 ActualRead = 0;
            if (!Socket->Wait(ESocketWaitConditions::WaitForRead, FTimespan::FromSeconds(10)))
            {
                return false;
            }
            else if (!Socket->Recv((uint8 *)ReceivedMessage.GetData() + CurrentSize, ExtraSpace, ActualRead))
            {
                return false;
            }
            ReceivedMessage.SetNum(CurrentSize + ActualRead);
        }
    }

    ISocketSubsystem *SocketSubsystem;
    FSocket *Socket;
    TArray<char> ReceivedMessage;
};

class FFMODStudioEditorModule : public IFMODStudioEditorModule
{
public:
    /** IModuleInterface implementation */
    FFMODStudioEditorModule()
        : bSimulating(false)
        , bIsInPIE(false)
        , bRegisteredComponentVisualizers(false)
    {
    }

    virtual void StartupModule() override;
    virtual void PostLoadCallback() override;
    virtual void ShutdownModule() override;

    void OnPostEngineInit();

    bool HandleSettingsSaved();

    /** Show notification */
    void ShowNotification(const FText &Text, SNotificationItem::ECompletionState State);

    void BeginPIE(bool simulating);
    void EndPIE(bool simulating);
    void PausePIE(bool simulating);
    void ResumePIE(bool simulating);

    void ViewportDraw(UCanvas *Canvas, APlayerController *);

    bool Tick(float DeltaTime);

    /** Build UE4 assets for FMOD Studio items */
    void ProcessBanks();

    /** Add extensions to menu */
    void RegisterHelpMenuEntries();
    void AddFileMenuExtension(FMenuBuilder &MenuBuilder);

    /** Show FMOD version */
    void ShowVersion();
    /** Open CHM */
    void OpenIntegrationDocs();
    /** Open web page to online docs */
    void OpenAPIDocs();
    /** Open Video tutorials page */
    void OpenVideoTutorials();
    /** Set Studio build path */
    void ValidateFMOD();

    /** Helper to get Studio project locales */
    bool GetStudioLocales(FFMODStudioLink &StudioLink, TArray<FFMODProjectLocale> &StudioLocales);

    /** Reload banks */
    void ReloadBanks();

    /** Callback for the main frame finishing load */
    void OnMainFrameLoaded(TSharedPtr<SWindow> InRootWindow, bool bIsNewProjectWindow);

    /** Callbacks for bad settings notification buttons */
    void OnBadSettingsPopupSettingsClicked();
    void OnBadSettingsPopupDismissClicked();

    TArray<FName> RegisteredComponentClassNames;
    void RegisterComponentVisualizer(FName ComponentClassName, TSharedPtr<FComponentVisualizer> Visualizer);

    FSimpleMulticastDelegate BanksReloadedDelegate;
    FSimpleMulticastDelegate &BanksReloadedEvent() override { return BanksReloadedDelegate; }

    /** The delegate to be invoked when this profiler manager ticks. */
    FTickerDelegate OnTick;

    /** Handle for registered delegates. */
    FTSTicker::FDelegateHandle TickDelegateHandle;
    FDelegateHandle BeginPIEDelegateHandle;
    FDelegateHandle EndPIEDelegateHandle;
    FDelegateHandle PausePIEDelegateHandle;
    FDelegateHandle ResumePIEDelegateHandle;
    FDelegateHandle FMODControlTrackEditorCreateTrackEditorHandle;
    FDelegateHandle FMODParamTrackEditorCreateTrackEditorHandle;

    /** Hook for drawing viewport */
    FDebugDrawDelegate ViewportDrawingDelegate;
    FDelegateHandle ViewportDrawingDelegateHandle;

    TSharedPtr<IComponentAssetBroker> AssetBroker;

    /** The extender to pass to the level editor to extend its window menu */
    TSharedPtr<FExtender> MainMenuExtender;

    /** Asset type actions for events (edit, play, stop) */
    TSharedPtr<FAssetTypeActions_FMODEvent> FMODEventAssetTypeActions;

    ISettingsSectionPtr SettingsSection;

    /** Notification popup that settings are bad */
    TWeakPtr<SNotificationItem> BadSettingsNotification;

    /** Asset builder */
    FFMODAssetBuilder AssetBuilder;

    /** Periodically checks for updates of the strings.bank file */
    FFMODBankUpdateNotifier BankUpdateNotifier;

    bool bSimulating;
    bool bIsInPIE;
    bool bRegisteredComponentVisualizers;
};

IMPLEMENT_MODULE(FFMODStudioEditorModule, FMODStudioEditor)

void FFMODStudioEditorModule::StartupModule()
{
    FCoreDelegates::OnPostEngineInit.AddRaw(this, &FFMODStudioEditorModule::OnPostEngineInit);
}

void FFMODStudioEditorModule::OnPostEngineInit()
{
    UE_LOG(LogFMOD, Log, TEXT("FFMODStudioEditorModule startup"));

    AssetBroker = MakeShareable(new FFMODAssetBroker);
    FComponentAssetBrokerage::RegisterBroker(AssetBroker, UFMODAudioComponent::StaticClass(), true, true);

    if (ISettingsModule *SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "FMODStudio", LOCTEXT("FMODStudioSettingsName", "FMOD Studio"),
            LOCTEXT("FMODStudioDescription", "Configure the FMOD Studio plugin"), GetMutableDefault<UFMODSettings>());

        if (SettingsSection.IsValid())
        {
            SettingsSection->OnModified().BindRaw(this, &FFMODStudioEditorModule::HandleSettingsSaved);
        }
    }

    // Register with the sequencer module that we provide auto-key handlers.
    ISequencerModule &SequencerModule = FModuleManager::Get().LoadModuleChecked<ISequencerModule>("Sequencer");
    FMODControlTrackEditorCreateTrackEditorHandle =
        SequencerModule.RegisterTrackEditor(FOnCreateTrackEditor::CreateStatic(&FFMODEventControlTrackEditor::CreateTrackEditor));
    FMODParamTrackEditorCreateTrackEditorHandle =
        SequencerModule.RegisterTrackEditor(FOnCreateTrackEditor::CreateStatic(&FFMODEventParameterTrackEditor::CreateTrackEditor));
    SequencerModule.RegisterChannelInterface<FFMODEventControlChannel>();

    // Register the details customizations
    {
        FPropertyEditorModule &PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

        PropertyModule.RegisterCustomClassLayout(
            UFMODSettings::StaticClass()->GetFName(),
            FOnGetDetailCustomizationInstance::CreateStatic(&FFMODSettingsCustomization::MakeInstance)
        );

        PropertyModule.RegisterCustomClassLayout(
            "FMODAudioComponent", FOnGetDetailCustomizationInstance::CreateStatic(&FFMODAudioComponentDetails::MakeInstance));
        PropertyModule.NotifyCustomizationModuleChanged();
    }

    // Need to load the editor module since it gets created after us, and we can't re-order ourselves otherwise our asset registration stops working!
    // It only works if we are running the editor, not a commandlet
    if (!IsRunningCommandlet() && !IsRunningGame() && FSlateApplication::IsInitialized())
    {
        FLevelEditorModule *LevelEditor = FModuleManager::LoadModulePtr<FLevelEditorModule>(TEXT("LevelEditor"));
        if (LevelEditor)
        {
            RegisterHelpMenuEntries();
            MainMenuExtender = MakeShareable(new FExtender);
            MainMenuExtender->AddMenuExtension("FileOpen", EExtensionHook::After, NULL,
                FMenuExtensionDelegate::CreateRaw(this, &FFMODStudioEditorModule::AddFileMenuExtension));
            LevelEditor->GetMenuExtensibilityManager()->AddExtender(MainMenuExtender);
        }
    }

    // Register AssetTypeActions
    IAssetTools &AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();

    FMODEventAssetTypeActions = MakeShareable(new FAssetTypeActions_FMODEvent);
    AssetTools.RegisterAssetTypeActions(FMODEventAssetTypeActions.ToSharedRef());

    // Register slate style overrides
    FFMODStudioStyle::Initialize();

    BeginPIEDelegateHandle = FEditorDelegates::BeginPIE.AddRaw(this, &FFMODStudioEditorModule::BeginPIE);
    EndPIEDelegateHandle = FEditorDelegates::EndPIE.AddRaw(this, &FFMODStudioEditorModule::EndPIE);
    PausePIEDelegateHandle = FEditorDelegates::PausePIE.AddRaw(this, &FFMODStudioEditorModule::PausePIE);
    ResumePIEDelegateHandle = FEditorDelegates::ResumePIE.AddRaw(this, &FFMODStudioEditorModule::ResumePIE);

    ViewportDrawingDelegate = FDebugDrawDelegate::CreateRaw(this, &FFMODStudioEditorModule::ViewportDraw);
    ViewportDrawingDelegateHandle = UDebugDrawService::Register(TEXT("Editor"), ViewportDrawingDelegate);

    OnTick = FTickerDelegate::CreateRaw(this, &FFMODStudioEditorModule::Tick);
    TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(OnTick);

    // Create asset builder
    AssetBuilder.Create();

    if (!IsRunningCommandlet())
    {
        // Build assets when asset registry has finished loading
        FAssetRegistryModule& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);
        AssetRegistry.Get().OnFilesLoaded().AddLambda([this]() { ProcessBanks(); });
    }

    // Bind to bank update notifier to reload banks when they change on disk
    BankUpdateNotifier.BanksUpdatedEvent.AddRaw(this, &FFMODStudioEditorModule::ProcessBanks);

    // Register a callback to validate settings on startup
    IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
    MainFrameModule.OnMainFrameCreationFinished().AddRaw(this, &FFMODStudioEditorModule::OnMainFrameLoaded);
}

void FFMODStudioEditorModule::ProcessBanks()
{
    if (!IsRunningCommandlet() && FApp::HasProjectName())
    {
        BankUpdateNotifier.EnableUpdate(false);
        ReloadBanks();

        const UFMODSettings &Settings = *GetDefault<UFMODSettings>();
        BankUpdateNotifier.SetFilePath(Settings.GetFullBankPath());

        BankUpdateNotifier.EnableUpdate(true);
    }
}

void FFMODStudioEditorModule::RegisterHelpMenuEntries()
{
    FToolMenuOwnerScoped OwnerScoped(this);
    UToolMenu* HelpMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Help");
    FToolMenuSection& Section = HelpMenu->AddSection("FMODHelp", LOCTEXT("FMODHelpLabel", "FMOD Help"),
        FToolMenuInsert("HelpBrowse", EToolMenuInsertType::Default));
    Section.AddEntry(FToolMenuEntry::InitMenuEntry(
        NAME_None,
        LOCTEXT("FMODVersionMenuEntryTitle", "About FMOD Studio"),
        LOCTEXT("FMODVersionMenuEntryToolTip", "Shows FMOD Studio version information."),
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateRaw(this, &FFMODStudioEditorModule::ShowVersion))
    ));

#if PLATFORM_WINDOWS
    Section.AddEntry(FToolMenuEntry::InitMenuEntry(
        NAME_None,
        LOCTEXT("FMODHelpCHMTitle", "FMOD Documentation..."),
        LOCTEXT("FMODHelpCHMToolTip", "Opens the local FMOD documentation."),
        FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.BrowseAPIReference"),
        FUIAction(FExecuteAction::CreateRaw(this, &FFMODStudioEditorModule::OpenIntegrationDocs))
    ));
#endif

    Section.AddEntry(FToolMenuEntry::InitMenuEntry(
        NAME_None,
        LOCTEXT("FMODHelpOnlineTitle", "FMOD Online Documentation..."),
        LOCTEXT("FMODHelpOnlineToolTip", "Go to the online FMOD documentation."),
        FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.BrowseDocumentation"),
        FUIAction(FExecuteAction::CreateRaw(this, &FFMODStudioEditorModule::OpenAPIDocs))
    ));

    Section.AddEntry(FToolMenuEntry::InitMenuEntry(
        NAME_None,
        LOCTEXT("FMODHelpVideosTitle", "FMOD Tutorial Videos..."),
        LOCTEXT("FMODHelpVideosToolTip", "Go to the online FMOD tutorial videos."),
        FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tutorials"),
        FUIAction(FExecuteAction::CreateRaw(this, &FFMODStudioEditorModule::OpenVideoTutorials))
    ));

    Section.AddEntry(FToolMenuEntry::InitMenuEntry(
        NAME_None,
        LOCTEXT("FMODSetStudioBuildTitle", "Validate FMOD"),
        LOCTEXT("FMODSetStudioBuildToolTip", "Verifies that FMOD and FMOD Studio are working as expected."),
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateRaw(this, &FFMODStudioEditorModule::ValidateFMOD))
    ));
}

void FFMODStudioEditorModule::AddFileMenuExtension(FMenuBuilder &MenuBuilder)
{
    MenuBuilder.BeginSection("FMODFile", LOCTEXT("FMODFileLabel", "FMOD"));
    MenuBuilder.AddMenuEntry(LOCTEXT("FMODFileMenuEntryTitle", "Reload Banks"),
        LOCTEXT("FMODFileMenuEntryToolTip", "Force a manual reload of all FMOD Studio banks."), FSlateIcon(),
        FUIAction(FExecuteAction::CreateRaw(this, &FFMODStudioEditorModule::ReloadBanks)));
    MenuBuilder.EndSection();
}

unsigned int GetDLLVersion()
{
    // Just grab it from the audition context which is always valid
    unsigned int DLLVersion = 0;
    FMOD::Studio::System *StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Auditioning);
    if (StudioSystem)
    {
        FMOD::System *LowLevelSystem = nullptr;
        if (StudioSystem->getCoreSystem(&LowLevelSystem) == FMOD_OK)
        {
            LowLevelSystem->getVersion(&DLLVersion);
        }
    }
    return DLLVersion;
}

FString VersionToString(unsigned int Version)
{
    unsigned int ProductVersion = (Version & 0xffff0000) >> 16;
    unsigned int MajorVersion = (Version & 0x0000ff00) >> 8;
    unsigned int MinorVersion = (Version & 0x000000ff);
    return FString::Printf(TEXT("%x.%02x.%02x"), ProductVersion, MajorVersion, MinorVersion);
}

unsigned int MakeVersion(unsigned int ProductVersion, unsigned int MajorVersion, unsigned int MinorVersion)
{
    auto EncodeAsHex = [](unsigned int Value) -> unsigned int
    {
        return 16 * (Value / 10) + Value % 10;
    };

    ProductVersion = EncodeAsHex(ProductVersion);
    MajorVersion = EncodeAsHex(MajorVersion);
    MinorVersion = EncodeAsHex(MinorVersion);

    return ((ProductVersion & 0xffff) << 16) | ((MajorVersion & 0xff) << 8) | (MinorVersion & 0xff);
}

unsigned int VersionFromString(FString Version)
{
    unsigned int ProductVersion = 0;
    unsigned int MajorVersion = 0;
    unsigned int MinorVersion = 0;
    TArray<FString> VersionFields;

    if (Version.ParseIntoArray(VersionFields, TEXT(".")) == 3)
    {
        ProductVersion = FCString::Atoi(*VersionFields[0]);
        MajorVersion = FCString::Atoi(*VersionFields[1]);
        MinorVersion = FCString::Atoi(*VersionFields[2]);
    }

    return MakeVersion(ProductVersion, MajorVersion, MinorVersion);
}

void FFMODStudioEditorModule::ShowVersion()
{
    FString HeaderVersion = VersionToString(FMOD_VERSION);
    FString DLLVersion = VersionToString(GetDLLVersion());

    FText VersionMessage = FText::Format(LOCTEXT("FMODStudio_About",
                                             "FMOD Studio\n\nBuilt Version: {0}\nDLL Version: {1}\n\nCopyright \u00A9 Firelight Technologies Pty "
                                             "Ltd.\n\nSee LICENSE.TXT for additional license information."),
        FText::FromString(HeaderVersion), FText::FromString(DLLVersion));

    FMessageDialog::Open(EAppMsgType::Ok, VersionMessage);
}

void FFMODStudioEditorModule::OpenIntegrationDocs()
{
    FPlatformProcess::LaunchFileInDefaultExternalApplication(TEXT("https://www.fmod.com/docs/unreal"));
}

void FFMODStudioEditorModule::OpenAPIDocs()
{
    FPlatformProcess::LaunchFileInDefaultExternalApplication(TEXT("https://www.fmod.com/docs/api"));
}

void FFMODStudioEditorModule::OpenVideoTutorials()
{
    FPlatformProcess::LaunchFileInDefaultExternalApplication(TEXT("http://www.youtube.com/user/FMODTV"));
}

bool FFMODStudioEditorModule::GetStudioLocales(FFMODStudioLink &StudioLink, TArray<FFMODProjectLocale> &StudioLocales)
{
    FString OutMessage;

    if (!StudioLink.Execute(TEXT("studio.project.workspace.locales.length"), OutMessage))
    {
        return false;
    }

    int NumStudioLocales = FCString::Atoi(*OutMessage);
    StudioLocales.Reserve(NumStudioLocales);

    for (int i = 0; i < NumStudioLocales; ++i)
    {
        FFMODProjectLocale Locale{};
        FString Message = FString::Printf(TEXT("studio.project.workspace.locales[%d].name"), i);

        if (!StudioLink.Execute(*Message, Locale.LocaleName))
        {
            return false;
        }

        Message = FString::Printf(TEXT("studio.project.workspace.locales[%d].localeCode"), i);

        if (!StudioLink.Execute(*Message, Locale.LocaleCode))
        {
            return false;
        }

        StudioLocales.Push(Locale);
    }

    return true;
}

void FFMODStudioEditorModule::ValidateFMOD()
{
    int ProblemsFound = 0;
    FFMODStudioLink StudioLink;
    bool Connected = StudioLink.Connect();

    if (!Connected)
    {
        if (EAppReturnType::No ==
            FMessageDialog::Open(EAppMsgType::YesNo,
                LOCTEXT("SetStudioBuildStudioNotRunning",
                    "FMODStudio does not appear to be running.  Only some validation will occur.  Do you want to continue anyway?")))
        {
            return;
        }
    }

    unsigned int HeaderVersion = FMOD_VERSION;
    unsigned int DLLVersion = GetDLLVersion();
    unsigned int StudioVersion = 0;

    if (Connected)
    {
        FString StudioVersionString;

        if (StudioLink.Execute(TEXT("studio.version"), StudioVersionString))
        {
            // We expect something like "Version xx.yy.zz, 32/64, Some build number"
            UE_LOG(LogFMOD, Log, TEXT("Received studio version: %s"), *StudioVersionString);
            TArray<FString> VersionParts;

            if (StudioVersionString.StartsWith(TEXT("Version ")) && StudioVersionString.ParseIntoArray(VersionParts, TEXT(",")) >= 1)
            {
                StudioVersion = VersionFromString(VersionParts[0].RightChop(8));
            }
        }
    }

    if (HeaderVersion != DLLVersion)
    {
        FText VersionMessage = FText::Format(LOCTEXT("SetStudioBuildStudio_Status",
                                                 "The FMOD DLL version is different to the version the integration was built against.  This may "
                                                 "cause problems running the game.\nBuilt Version: {0}\nDLL Version: {1}\n"),
            FText::FromString(VersionToString(HeaderVersion)), FText::FromString(VersionToString(DLLVersion)));
        FMessageDialog::Open(EAppMsgType::Ok, VersionMessage);
        ProblemsFound++;
    }

    if (Connected && StudioVersion != DLLVersion)
    {
        FText VersionMessage =
            FText::Format(LOCTEXT("SetStudioBuildStudio_Version",
                              "The Studio tool is different to the version the integration was built against.  The integration may not be able to "
                              "load the banks that the tool builds.\n\nBuilt Version: {0}\nDLL Version: {1}\nStudio Version: {2}\n\nWe recommend "
                              "using the Studio tool that matches the integration.\n\nDo you want to continue with the validation?"),
                FText::FromString(VersionToString(HeaderVersion)), FText::FromString(VersionToString(DLLVersion)),
                FText::FromString(VersionToString(StudioVersion)));

        if (EAppReturnType::No == FMessageDialog::Open(EAppMsgType::YesNo, VersionMessage))
        {
            return;
        }

        ProblemsFound++;
    }

    UFMODSettings& Settings = *GetMutableDefault<UFMODSettings>();

    FString FullBankPath = Settings.BankOutputDirectory.Path;

    if (FPaths::IsRelative(FullBankPath))
    {
        FullBankPath = FPaths::ProjectContentDir() / FullBankPath;
    }

    FString PlatformBankPath = Settings.GetFullBankPath();
    FullBankPath = FPaths::ConvertRelativePathToFull(FullBankPath);
    PlatformBankPath = FPaths::ConvertRelativePathToFull(PlatformBankPath);

    if (Connected)
    {
        // File path was added in FMOD Studio 1.07.00
        FString StudioProjectPath;
        FString StudioProjectDir;

        if (StudioVersion >= MakeVersion(1, 7, 0))
        {
            StudioLink.Execute(TEXT("studio.project.filePath"), StudioProjectPath);

            if (StudioProjectPath.IsEmpty() || StudioProjectPath == TEXT("undefined"))
            {
                FMessageDialog::Open(EAppMsgType::Ok,
                    LOCTEXT("SetStudioBuildStudio_NewProject",
                        "FMOD Studio has an empty project.  Please go to FMOD Studio, and press Save to create your new project."));
                // Just try to save anyway
                FString Result;
                StudioLink.Execute(TEXT("studio.project.save()"), Result);
            }

            StudioLink.Execute(TEXT("studio.project.filePath"), StudioProjectPath);

            if (StudioProjectPath != TEXT("undefined"))
            {
                StudioProjectDir = FPaths::GetPath(StudioProjectPath);
            }
        }

        FString StudioPathString;
        StudioLink.Execute(TEXT("studio.project.workspace.builtBanksOutputDirectory"), StudioPathString);

        if (StudioPathString == TEXT("undefined"))
        {
            StudioPathString = TEXT("");
        }

        FString CanonicalBankPath = FullBankPath;
        FPaths::CollapseRelativeDirectories(CanonicalBankPath);
        FPaths::NormalizeDirectoryName(CanonicalBankPath);
        FPaths::RemoveDuplicateSlashes(CanonicalBankPath);
        FPaths::NormalizeDirectoryName(CanonicalBankPath);
        FString CanonicalStudioPath = StudioPathString;

        if (FPaths::IsRelative(CanonicalStudioPath) && !StudioProjectDir.IsEmpty() && !StudioPathString.IsEmpty())
        {
            CanonicalStudioPath = FPaths::Combine(*StudioProjectDir, *CanonicalStudioPath);
        }

        FPaths::CollapseRelativeDirectories(CanonicalStudioPath);
        FPaths::NormalizeDirectoryName(CanonicalStudioPath);
        FPaths::RemoveDuplicateSlashes(CanonicalStudioPath);
        FPaths::NormalizeDirectoryName(CanonicalStudioPath);

        if (!FPaths::IsSamePath(CanonicalBankPath, CanonicalStudioPath))
        {
            FString BankPathToSet = FullBankPath;

            // Extra logic - if we have put the studio project inside the game project, then make it relative
            if (!StudioProjectDir.IsEmpty())
            {
                FString GameBaseDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
                FString BankPathFromGameProject = FullBankPath;
                FString StudioProjectFromGameProject = StudioProjectDir;
                if (FPaths::MakePathRelativeTo(BankPathFromGameProject, *GameBaseDir) && !BankPathFromGameProject.Contains(TEXT("..")) &&
                    FPaths::MakePathRelativeTo(StudioProjectFromGameProject, *GameBaseDir) && !StudioProjectFromGameProject.Contains(TEXT("..")))
                {
                    FPaths::MakePathRelativeTo(BankPathToSet, *(StudioProjectDir + TEXT("/")));
                }
            }

            ProblemsFound++;

            FText AskMessage = FText::Format(LOCTEXT("SetStudioBuildStudio_Ask",
                                                 "FMOD Studio build path should be set up.\n\nCurrent Studio build path: {0}\nNew build path: "
                                                 "{1}\n\nDo you want to fix up the project now?"),
                FText::FromString(StudioPathString), FText::FromString(BankPathToSet));

            if (EAppReturnType::Yes == FMessageDialog::Open(EAppMsgType::YesNo, AskMessage))
            {
                FString Result;
                StudioLink.Execute(*FString::Printf(TEXT("studio.project.workspace.builtBanksOutputDirectory = \"%s\";"), *BankPathToSet), Result);
                StudioLink.Execute(TEXT("studio.project.workspace.builtBanksOutputDirectory"), Result);

                if (Result != BankPathToSet)
                {
                    FMessageDialog::Open(EAppMsgType::Ok,
                        LOCTEXT("SetStudioBuildStudio_Save",
                            "Failed to set bank directory.  Please go to FMOD Studio, and set the bank path in FMOD Studio project settings."));
                }

                FMessageDialog::Open(
                    EAppMsgType::Ok, LOCTEXT("SetStudioBuildStudio_Save", "Please go to FMOD Studio, save your project and build banks."));
                // Just try to do it again anyway
                StudioLink.Execute(TEXT("studio.project.save()"), Result);
                StudioLink.Execute(TEXT("studio.project.build()"), Result);
            }
        }

        if (StudioVersion >= MakeVersion(2, 0, 0))
        {
            // Check whether Studio project locales match those setup in UE4
            TArray<FFMODProjectLocale> StudioLocales;

            if (GetStudioLocales(StudioLink, StudioLocales))
            {
                bool bAllMatch = true;

                if (StudioLocales.Num() == Settings.Locales.Num())
                {
                    for (const FFMODProjectLocale& StudioLocale : StudioLocales)
                    {
                        bool bMatch = false;

                        for (const FFMODProjectLocale& Locale : Settings.Locales)
                        {
                            if (Locale.LocaleCode == StudioLocale.LocaleCode && Locale.LocaleName == StudioLocale.LocaleName)
                            {
                                bMatch = true;
                                break;
                            }
                        }

                        if (!bMatch)
                        {
                            bAllMatch = false;
                            break;
                        }
                    }
                }
                else
                {
                    bAllMatch = false;
                }

                if (!bAllMatch)
                {
                    ProblemsFound++;
                    FText Message = LOCTEXT("LocalesMismatch",
                        "The project locales do not match those defined in the FMOD Studio Project.\n\n"
                        "Would you like to import the locales from Studio?\n");
                    if (FMessageDialog::Open(EAppMsgType::YesNo, Message) == EAppReturnType::Yes)
                    {
                        Settings.Locales = StudioLocales;
                        if (Settings.Locales.Num() > 0)
                        {
                            Settings.Locales[0].bDefault = true;
                        }
                        SettingsSection->Save();
                        IFMODStudioModule::Get().ReloadBanks();
                    }
                }
            }
        }
    }

    bool bAnyBankFiles = false;

    // Check bank path
    if (!FPaths::DirectoryExists(FullBankPath) || !FPaths::DirectoryExists(PlatformBankPath))
    {
        FText DirMessage = FText::Format(LOCTEXT("SetStudioBuildStudio_Dir",
                                             "The FMOD Content directory does not exist.  Please make sure FMOD Studio is exporting banks into the "
                                             "correct location.\n\nBanks should be exported to: {0}\nBanks files should exist in: {1}\n"),
            FText::FromString(FullBankPath), FText::FromString(PlatformBankPath));
        FMessageDialog::Open(EAppMsgType::Ok, DirMessage);
        ProblemsFound++;
    }
    else
    {
        TArray<FString> BankFiles;
        IFMODStudioModule::Get().GetAllBankPaths(BankFiles, true);

        if (BankFiles.Num() != 0)
        {
            bAnyBankFiles = true;
        }
        else
        {
            FText EmptyBankDirMessage =
                FText::Format(LOCTEXT("SetStudioBuildStudio_EmptyBankDir",
                                  "The FMOD Content directory does not have any bank files in them.  Please make sure FMOD Studio is exporting banks "
                                  "into the correct location.\n\nBanks should be exported to: {0}\nBanks files should exist in: {1}\n"),
                    FText::FromString(FullBankPath), FText::FromString(PlatformBankPath));
            FMessageDialog::Open(EAppMsgType::Ok, EmptyBankDirMessage);
            ProblemsFound++;
        }
    }

    // Look for banks that may have failed to load
    if (bAnyBankFiles)
    {
        FMOD::Studio::System *StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Auditioning);
        int BankCount = 0;
        StudioSystem->getBankCount(&BankCount);
        TArray<FString> FailedBanks = IFMODStudioModule::Get().GetFailedBankLoads(EFMODSystemContext::Auditioning);

        if (BankCount == 0 || FailedBanks.Num() != 0)
        {
            FString CombinedBanks;

            for (auto Bank : FailedBanks)
            {
                CombinedBanks += Bank;
                CombinedBanks += TEXT("\n");
            }

            FText BankLoadMessage;

            if (BankCount == 0 && FailedBanks.Num() == 0)
            {
                BankLoadMessage = LOCTEXT("SetStudioBuildStudio_BankLoad", "Failed to load banks\n");
            }
            else if (BankCount == 0)
            {
                BankLoadMessage =
                    FText::Format(LOCTEXT("SetStudioBuildStudio_BankLoad", "Failed to load banks:\n{0}\n"), FText::FromString(CombinedBanks));
            }
            else
            {
                BankLoadMessage =
                    FText::Format(LOCTEXT("SetStudioBuildStudio_BankLoad", "Some banks failed to load:\n{0}\n"), FText::FromString(CombinedBanks));
            }

            FMessageDialog::Open(EAppMsgType::Ok, BankLoadMessage);
            ProblemsFound++;
        }
        else
        {
            int TotalEventCount = 0;
            TArray<FMOD::Studio::Bank *> Banks;
            Banks.SetNum(BankCount);
            StudioSystem->getBankList(Banks.GetData(), BankCount, &BankCount);

            for (FMOD::Studio::Bank *Bank : Banks)
            {
                int EventCount = 0;
                Bank->getEventCount(&EventCount);
                TotalEventCount += EventCount;
            }

            if (TotalEventCount == 0)
            {
                FMessageDialog::Open(EAppMsgType::Ok,
                    LOCTEXT("SetStudioBuildStudio_NoEvents",
                        "Banks have been loaded but they didn't have any events in them.  Please make sure you have added some events to banks."));
                ProblemsFound++;
            }
        }
    }

    // Look for required plugins that have not been registered
    TArray<FString> RequiredPlugins = IFMODStudioModule::Get().GetRequiredPlugins();

    if (RequiredPlugins.Num() != 0 && Settings.PluginFiles.Num() == 0)
    {
        FString CombinedPlugins;

        for (auto Name : RequiredPlugins)
        {
            CombinedPlugins += Name;
            CombinedPlugins += TEXT("\n");
        }

        FText PluginMessage =
            FText::Format(LOCTEXT("SetStudioBuildStudio_Plugins",
                              "The banks require the following plugins, but no plugin filenames are listed in the settings:\n{0}\n"),
                FText::FromString(CombinedPlugins));
        FMessageDialog::Open(EAppMsgType::Ok, PluginMessage);
        ProblemsFound++;
    }

    // Look for FMOD in packaging settings
    UProjectPackagingSettings *PackagingSettings = Cast<UProjectPackagingSettings>(UProjectPackagingSettings::StaticClass()->GetDefaultObject());
    bool bPackagingFound = false;

    for (int i = 0; i < PackagingSettings->DirectoriesToAlwaysStageAsNonUFS.Num(); ++i)
    {
        // We allow subdirectory references, such as "FMOD/Mobile"
        if (PackagingSettings->DirectoriesToAlwaysStageAsNonUFS[i].Path.StartsWith(Settings.BankOutputDirectory.Path))
        {
            bPackagingFound = true;
            break;
        }
    }

    int OldPackagingIndex = -1;

    for (int i = 0; i < PackagingSettings->DirectoriesToAlwaysStageAsUFS.Num(); ++i)
    {
        if (PackagingSettings->DirectoriesToAlwaysStageAsUFS[i].Path.StartsWith(Settings.BankOutputDirectory.Path))
        {
            OldPackagingIndex = i;
            break;
        }
    }

    if (OldPackagingIndex >= 0)
    {
        ProblemsFound++;

        FText message = bPackagingFound ?
            LOCTEXT("PackagingFMOD_Both",
                "FMOD has been added to both the \"Additional Non-Asset Directories to Copy\" and the \"Additional Non-Asset Directories to Package\" "
                "lists. It is recommended to remove FMOD from the \"Additional Non-Asset Directories to Package\" list.\n\n"
                "Do you want to remove it now?") :
            LOCTEXT("PackagingFMOD_AskMove",
                "FMOD has been added to the \"Additional Non-Asset Directories to Package\" list. "
                "Packaging FMOD content may lead to deadlocks at runtime. "
                "It is recommended to move FMOD to the \"Additional Non-Asset Directories to Copy\" list.\n\n"
                "Do you want to move it now?");

        if (EAppReturnType::Yes == FMessageDialog::Open(EAppMsgType::YesNo, message))
        {
            PackagingSettings->DirectoriesToAlwaysStageAsUFS.RemoveAt(OldPackagingIndex);

            if (!bPackagingFound)
            {
                PackagingSettings->DirectoriesToAlwaysStageAsNonUFS.Add(Settings.BankOutputDirectory);
            }

            PackagingSettings->TryUpdateDefaultConfigFile();
        }
    }
    else if (!bPackagingFound)
    {
        ProblemsFound++;

        FText message = LOCTEXT("PackagingFMOD_Ask",
            "FMOD has not been added to the \"Additional Non-Asset Directories to Copy\" list.\n\nDo you want add it now?");

        if (EAppReturnType::Yes == FMessageDialog::Open(EAppMsgType::YesNo, message))
        {
            PackagingSettings->DirectoriesToAlwaysStageAsNonUFS.Add(Settings.BankOutputDirectory);
            PackagingSettings->TryUpdateDefaultConfigFile();
        }
    }

    bool bAssetsFound = false;
    for (int i = 0; i < PackagingSettings->DirectoriesToAlwaysCook.Num(); ++i)
    {
        if (PackagingSettings->DirectoriesToAlwaysCook[i].Path.StartsWith(Settings.GetFullContentPath()))
        {
            bAssetsFound = true;
            break;
        }
    }
    if (!bAssetsFound)
    {
        ProblemsFound++;

        FText message = LOCTEXT("PackagingFMOD_Ask",
            "FMOD has not been added to the \"Additional Asset Directories to Cook\" list.\n\nDo you want add it now?");

        if (EAppReturnType::Yes == FMessageDialog::Open(EAppMsgType::YesNo, message))
        {
            FDirectoryPath GeneratedDir;
            for (FString folder : Settings.GeneratedFolders)
            {
                GeneratedDir.Path = Settings.GetFullContentPath() / folder;
                PackagingSettings->DirectoriesToAlwaysCook.Add(GeneratedDir);
            }
            PackagingSettings->TryUpdateDefaultConfigFile();
        }
    }

    // Summary
    if (ProblemsFound)
    {
        FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("SetStudioBuildStudio_FinishedBad", "Finished validation.\n"));
    }
    else
    {
        FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("SetStudioBuildStudio_FinishedGood", "Finished validation.  No problems detected.\n"));
    }
}

void FFMODStudioEditorModule::OnMainFrameLoaded(TSharedPtr<SWindow> InRootWindow, bool bIsNewProjectWindow)
{
    if (!bIsNewProjectWindow)
    {
        // Show a popup notification that allows the user to fix bad settings
        const UFMODSettings& Settings = *GetDefault<UFMODSettings>();

        if (Settings.Check() != UFMODSettings::Okay)
        {
            FNotificationInfo Info(LOCTEXT("BadSettingsPopupTitle", "FMOD Settings Problem Detected"));
            Info.bFireAndForget = false;
            Info.bUseLargeFont = true;
            Info.bUseThrobber = false;
            Info.FadeOutDuration = 0.5f;
            Info.ButtonDetails.Add(FNotificationButtonInfo(LOCTEXT("BadSettingsPopupSettings", "Settings..."),
                LOCTEXT("BadSettingsPopupSettingsTT", "Open the settings editor"),
                FSimpleDelegate::CreateRaw(this, &FFMODStudioEditorModule::OnBadSettingsPopupSettingsClicked)));
            Info.ButtonDetails.Add(FNotificationButtonInfo(LOCTEXT("BadSettingsPopupDismiss", "Dismiss"), 
                LOCTEXT("BadSettingsPopupDismissTT", "Dismiss this notification"),
                FSimpleDelegate::CreateRaw(this, &FFMODStudioEditorModule::OnBadSettingsPopupDismissClicked)));

            BadSettingsNotification = FSlateNotificationManager::Get().AddNotification(Info);
            BadSettingsNotification.Pin()->SetCompletionState(SNotificationItem::CS_Pending);
        }
    }
}

void FFMODStudioEditorModule::OnBadSettingsPopupSettingsClicked()
{
    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        SettingsModule->ShowViewer("Project", "Plugins", "FMODStudio");
    }

    BadSettingsNotification.Pin()->ExpireAndFadeout();
}

void FFMODStudioEditorModule::OnBadSettingsPopupDismissClicked()
{
    BadSettingsNotification.Pin()->ExpireAndFadeout();
}

bool FFMODStudioEditorModule::Tick(float DeltaTime)
{
    if (!bRegisteredComponentVisualizers && GUnrealEd != nullptr)
    {
        // Register component visualizers (GUnrealED is required for this, but not initialized before this module loads, so we have to wait until GUnrealEd is available)
        RegisterComponentVisualizer(UFMODAudioComponent::StaticClass()->GetFName(), MakeShareable(new FFMODAudioComponentVisualizer));

        bRegisteredComponentVisualizers = true;
    }

    BankUpdateNotifier.Update(DeltaTime);

    // Update listener position for Editor sound system
    FMOD::Studio::System *StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Editor);
    if (StudioSystem)
    {
        if (GCurrentLevelEditingViewportClient)
        {
            const FVector &ViewLocation = GCurrentLevelEditingViewportClient->GetViewLocation();
            FMatrix CameraToWorld = FRotationMatrix::Make(GCurrentLevelEditingViewportClient->GetViewRotation());
            FVector Up = CameraToWorld.GetUnitAxis(EAxis::Z);
            FVector Forward = CameraToWorld.GetUnitAxis(EAxis::X);

            FMOD_3D_ATTRIBUTES Attributes = { { 0 } };
            Attributes.position = FMODUtils::ConvertWorldVector(ViewLocation);
            Attributes.forward = FMODUtils::ConvertUnitVector(Forward);
            Attributes.up = FMODUtils::ConvertUnitVector(Up);

            verifyfmod(StudioSystem->setListenerAttributes(0, &Attributes));
        }
    }

    return true;
}

void FFMODStudioEditorModule::BeginPIE(bool simulating)
{
    UE_LOG(LogFMOD, Verbose, TEXT("FFMODStudioEditorModule BeginPIE: %d"), simulating);
    BankUpdateNotifier.EnableUpdate(false);
    bSimulating = simulating;
    bIsInPIE = true;
    IFMODStudioModule::Get().SetInPIE(true, simulating);
}

void FFMODStudioEditorModule::EndPIE(bool simulating)
{
    UE_LOG(LogFMOD, Verbose, TEXT("FFMODStudioEditorModule EndPIE: %d"), simulating);
    bSimulating = false;
    bIsInPIE = false;
    IFMODStudioModule::Get().SetInPIE(false, simulating);
    BankUpdateNotifier.EnableUpdate(true);
}

void FFMODStudioEditorModule::PausePIE(bool simulating)
{
    UE_LOG(LogFMOD, Verbose, TEXT("FFMODStudioEditorModule PausePIE%d"));
    IFMODStudioModule::Get().SetSystemPaused(true);
}

void FFMODStudioEditorModule::ResumePIE(bool simulating)
{
    UE_LOG(LogFMOD, Verbose, TEXT("FFMODStudioEditorModule ResumePIE"));
    IFMODStudioModule::Get().SetSystemPaused(false);
}

void FFMODStudioEditorModule::PostLoadCallback()
{
    UE_LOG(LogFMOD, Verbose, TEXT("FFMODStudioEditorModule PostLoadCallback"));
}

void FFMODStudioEditorModule::ViewportDraw(UCanvas *Canvas, APlayerController *)
{
    // Only want to update based on viewport in simulate mode.
    // In PIE/game, we update from the player controller instead.
    if (!bSimulating)
    {
        return;
    }

    const FSceneView *View = Canvas->SceneView;

    if (View->Drawer == GCurrentLevelEditingViewportClient)
    {
        UWorld *World = GCurrentLevelEditingViewportClient->GetWorld();
        const FVector &ViewLocation = GCurrentLevelEditingViewportClient->GetViewLocation();

        FMatrix CameraToWorld = View->ViewMatrices.GetViewMatrix().InverseFast();
        FVector ProjUp = CameraToWorld.TransformVector(FVector(0, 1000, 0));
        FVector ProjRight = CameraToWorld.TransformVector(FVector(1000, 0, 0));

        FTransform ListenerTransform(FRotationMatrix::MakeFromZY(ProjUp, ProjRight));
        ListenerTransform.SetTranslation(ViewLocation);
        ListenerTransform.NormalizeRotation();

        IFMODStudioModule::Get().SetListenerPosition(0, World, ListenerTransform, 0.0f);
        IFMODStudioModule::Get().FinishSetListenerPosition(1);
    }
}

void FFMODStudioEditorModule::ShutdownModule()
{
    UE_LOG(LogFMOD, Verbose, TEXT("FFMODStudioEditorModule shutdown"));

    if (UObjectInitialized())
    {
        BankUpdateNotifier.BanksUpdatedEvent.RemoveAll(this);

        // Unregister tick function.
        FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);

        FEditorDelegates::BeginPIE.Remove(BeginPIEDelegateHandle);
        FEditorDelegates::EndPIE.Remove(EndPIEDelegateHandle);
        FEditorDelegates::PausePIE.Remove(PausePIEDelegateHandle);
        FEditorDelegates::ResumePIE.Remove(ResumePIEDelegateHandle);

        if (ViewportDrawingDelegate.IsBound())
        {
            UDebugDrawService::Unregister(ViewportDrawingDelegateHandle);
        }

        FComponentAssetBrokerage::UnregisterBroker(AssetBroker);

        if (MainMenuExtender.IsValid())
        {
            FLevelEditorModule *LevelEditorModule = FModuleManager::GetModulePtr<FLevelEditorModule>("LevelEditor");
            if (LevelEditorModule)
            {
                LevelEditorModule->GetMenuExtensibilityManager()->RemoveExtender(MainMenuExtender);
            }
        }
    }

    if (ISettingsModule *SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        SettingsModule->UnregisterSettings("Project", "Plugins", "FMODStudio");
    }

    // Unregister AssetTypeActions
    if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
    {
        IAssetTools &AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();

        AssetTools.UnregisterAssetTypeActions(FMODEventAssetTypeActions.ToSharedRef());
    }

    // Unregister component visualizers
    if (GUnrealEd != nullptr)
    {
        // Iterate over all class names we registered for
        for (FName ClassName : RegisteredComponentClassNames)
        {
            GUnrealEd->UnregisterComponentVisualizer(ClassName);
        }
    }

    // Unregister sequencer track creation delegates
    ISequencerModule *SequencerModule = FModuleManager::GetModulePtr<ISequencerModule>("Sequencer");
    if (SequencerModule != nullptr)
    {
        SequencerModule->UnRegisterTrackEditor(FMODControlTrackEditorCreateTrackEditorHandle);
        SequencerModule->UnRegisterTrackEditor(FMODParamTrackEditorCreateTrackEditorHandle);
    }
}

bool FFMODStudioEditorModule::HandleSettingsSaved()
{
    ProcessBanks();
    return true;
}

void FFMODStudioEditorModule::ReloadBanks()
{
    AssetBuilder.ProcessBanks();
    IFMODStudioModule::Get().ReloadBanks();
    BanksReloadedDelegate.Broadcast();

    // Show a reload notification
    TArray<FString> FailedBanks = IFMODStudioModule::Get().GetFailedBankLoads(EFMODSystemContext::Auditioning);
    FText Message;
    SNotificationItem::ECompletionState State;
    if (FailedBanks.Num() == 0)
    {
        Message = LOCTEXT("FMODBanksReloaded", "Reloaded FMOD Banks\n");
        State = SNotificationItem::CS_Success;
    }
    else
    {
        FString CombinedMessage = "Problem loading FMOD Banks:";
        for (auto Entry : FailedBanks)
        {
            CombinedMessage += TEXT("\n");
            CombinedMessage += Entry;

            UE_LOG(LogFMOD, Warning, TEXT("Problem loading FMOD Bank: %s"), *Entry);
        }

        Message = FText::Format(LOCTEXT("FMODBanksReloaded", "{0}"), FText::FromString(CombinedMessage));
        State = SNotificationItem::CS_Fail;
    }
    ShowNotification(Message, State);
}

void FFMODStudioEditorModule::ShowNotification(const FText &Text, SNotificationItem::ECompletionState State)
{
    FNotificationInfo Info(Text);
    Info.Image = FAppStyle::GetBrush(TEXT("NoBrush"));
    Info.FadeInDuration = 0.1f;
    Info.FadeOutDuration = 0.5f;
    Info.ExpireDuration = State == SNotificationItem::CS_Fail ? 6.0f : 1.5f;
    Info.bUseThrobber = false;
    Info.bUseSuccessFailIcons = true;
    Info.bUseLargeFont = true;
    Info.bFireAndForget = false;
    Info.bAllowThrottleWhenFrameRateIsLow = false;
    auto NotificationItem = FSlateNotificationManager::Get().AddNotification(Info);
    NotificationItem->SetCompletionState(State);
    NotificationItem->ExpireAndFadeout();

    if (GCurrentLevelEditingViewportClient)
    {
        // Refresh any 3d event visualization
        GCurrentLevelEditingViewportClient->bNeedsRedraw = true;
    }
}

void FFMODStudioEditorModule::RegisterComponentVisualizer(FName ComponentClassName, TSharedPtr<FComponentVisualizer> Visualizer)
{
    if (GUnrealEd != nullptr)
    {
        GUnrealEd->RegisterComponentVisualizer(ComponentClassName, Visualizer);
    }

    RegisteredComponentClassNames.Add(ComponentClassName);

    if (Visualizer.IsValid())
    {
        Visualizer->OnRegister();
    }
}

#undef LOCTEXT_NAMESPACE