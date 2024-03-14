// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#include "FMODEventEditor.h"
#include "FMODEvent.h"
#include "FMODStudioEditorModule.h"
#include "FMODStudioModule.h"
#include "FMODUtils.h"
#include "SFMODEventEditorPanel.h"
#include "Widgets/Docking/SDockTab.h"
#include "fmod_studio.hpp"
#include "Editor.h"

#define LOCTEXT_NAMESPACE "FMODEventEditor"

DEFINE_LOG_CATEGORY_STATIC(LogFMODEventEditor, Log, All);

const FName FFMODEventEditor::EventEditorTabId(TEXT("FFMODEventEditor_EventView"));
const FName FFMODEventEditor::FMODEventEditorAppIdentifier(TEXT("FMODEventEditorApp"));

void FFMODEventEditor::RegisterTabSpawners(const TSharedRef<class FTabManager> &NewTabManager)
{
    WorkspaceMenuCategory = NewTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_FMODEventEditor", "FMOD Event Editor"));
    auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

    FAssetEditorToolkit::RegisterTabSpawners(NewTabManager);

    NewTabManager->RegisterTabSpawner(EventEditorTabId, FOnSpawnTab::CreateSP(this, &FFMODEventEditor::SpawnTab_EventEditor))
        .SetDisplayName(LOCTEXT("EventTab", "FMOD Event"))
        .SetGroup(WorkspaceMenuCategoryRef);
}

void FFMODEventEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager> &NewTabManager)
{
    FAssetEditorToolkit::UnregisterTabSpawners(NewTabManager);

    NewTabManager->UnregisterTabSpawner(EventEditorTabId);
}

FFMODEventEditor::FFMODEventEditor()
    : CurrentPreviewEventInstance(nullptr)
    , EditedEvent(nullptr)
{
    IFMODStudioEditorModule::Get().BanksReloadedEvent().AddRaw(this, &FFMODEventEditor::HandleBanksReloaded);
    BeginPIEDelegateHandle = FEditorDelegates::BeginPIE.AddRaw(this, &FFMODEventEditor::HandleBeginPIE);
}

FFMODEventEditor::~FFMODEventEditor()
{
    IFMODStudioEditorModule::Get().BanksReloadedEvent().RemoveAll(this);
    FEditorDelegates::BeginPIE.Remove(BeginPIEDelegateHandle);

    CurrentPreviewEventInstance = nullptr;
}

UFMODEvent *FFMODEventEditor::GetEditedEvent() const
{
    return EditedEvent;
}

FMOD::Studio::EventDescription *FFMODEventEditor::GetEventDescription() const
{
    return IFMODStudioModule::Get().GetEventDescription(EditedEvent, EFMODSystemContext::Auditioning);
}

void FFMODEventEditor::PlayEvent()
{
    CurrentPreviewEventInstance = IFMODStudioModule::Get().CreateAuditioningInstance(EditedEvent);
    if (CurrentPreviewEventInstance != nullptr)
    {
        TArray<float> values;
        TArray<FMOD_STUDIO_PARAMETER_ID> ids;
        
        ParameterValues.GenerateKeyArray(ids);
        ParameterValues.GenerateValueArray(values);

        CurrentPreviewEventInstance->setParametersByIDs(ids.GetData(), values.GetData(), ParameterValues.Num());

        CurrentPreviewEventInstance->start();
    }
}

void FFMODEventEditor::PauseEvent()
{
    if (CurrentPreviewEventInstance != nullptr)
    {
        bool bIsPaused = false;
        CurrentPreviewEventInstance->getPaused(&bIsPaused);
        CurrentPreviewEventInstance->setPaused(!bIsPaused);
    }
}

void FFMODEventEditor::StopEvent()
{
    IFMODStudioModule::Get().StopAuditioningInstance();
}

void FFMODEventEditor::SetParameterValue(FMOD_STUDIO_PARAMETER_ID ParameterId, float Value)
{
    ParameterValues[ParameterId] = Value;

    if (CurrentPreviewEventInstance != nullptr)
    {
        CurrentPreviewEventInstance->setParameterByID(ParameterId, Value);
    }
}

void FFMODEventEditor::AddParameter(FMOD_STUDIO_PARAMETER_ID ParameterId, float Value)
{
    ParameterValues.Add(ParameterId, Value);
}

float FFMODEventEditor::GetParameterValue(FMOD_STUDIO_PARAMETER_ID Id)
{
    return ParameterValues[Id];
}

void FFMODEventEditor::InitFMODEventEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost> &InitToolkitHost, UFMODEvent *Event)
{
    if (IsValid(Event))
    {
        EditedEvent = Event;

        TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout =
            FTabManager::NewLayout("Standalone_FMODEventEditor_Layout")
                ->AddArea(FTabManager::NewPrimaryArea()
                              ->SetOrientation(Orient_Vertical)
                              ->Split(FTabManager::NewStack()->AddTab(EventEditorTabId, ETabState::OpenedTab)->SetHideTabWell(true)));

        const bool bCreateDefaultStandaloneMenu = true;
        const bool bCreateDefaultToolbar = false;
        FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, FFMODEventEditor::FMODEventEditorAppIdentifier, StandaloneDefaultLayout,
            bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, Event);
    }
}

FName FFMODEventEditor::GetToolkitFName() const
{
    return FName("FMODEventEditor");
}

FText FFMODEventEditor::GetBaseToolkitName() const
{
    return LOCTEXT("ToolkitName", "FMOD Event Editor");
}

FString FFMODEventEditor::GetWorldCentricTabPrefix() const
{
    return LOCTEXT("WorldCentricTabPrefix", "FMOD Event ").ToString();
}

FLinearColor FFMODEventEditor::GetWorldCentricTabColorScale() const
{
    return FLinearColor(0.0f, 0.0f, 0.5f, 0.5f);
}

void FFMODEventEditor::CreateInternalWidgets()
{
    FMODEventEditorPanel = SNew(SFMODEventEditorPanel).FMODEventEditor(SharedThis(this));
}

TSharedRef<SDockTab> FFMODEventEditor::SpawnTab_EventEditor(const FSpawnTabArgs &Args)
{
    check(Args.GetTabId().TabType == EventEditorTabId);

    CreateInternalWidgets();

    return SAssignNew(OwnerTab, SDockTab)
        .Label(LOCTEXT("EventEditorTitle", "FMOD Event"))
        .TabColorScale(GetTabColorScale())[FMODEventEditorPanel.ToSharedRef()];
}

void FFMODEventEditor::HandleBanksReloaded()
{
    CurrentPreviewEventInstance = nullptr;

    CreateInternalWidgets();

    if (OwnerTab.IsValid())
    {
        OwnerTab->SetContent(FMODEventEditorPanel.ToSharedRef());
    }
}

void FFMODEventEditor::HandleBeginPIE(bool bSimulating)
{
    CurrentPreviewEventInstance = nullptr;
}

#undef LOCTEXT_NAMESPACE
