// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#pragma once

#include "AssetTypeActions_Base.h"

namespace FMOD
{
namespace Studio
{
class EventInstance;
}
}

class UFMODEvent;

class FAssetTypeActions_FMODEvent : public FAssetTypeActions_Base
{
public:
    FAssetTypeActions_FMODEvent();
    ~FAssetTypeActions_FMODEvent();

    // IAssetTypeActions Implementation
    virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_FMODEvent", "FMOD Event"); }
    virtual FColor GetTypeColor() const override { return FColor(0, 175, 255); }
    virtual UClass *GetSupportedClass() const override;
    virtual bool HasActions(const TArray<UObject *> &InObjects) const override { return true; }
    virtual void GetActions(const TArray<UObject *> &InObjects, FMenuBuilder &MenuBuilder) override;
    virtual bool AssetsActivatedOverride(const TArray<UObject *> &InObjects, EAssetTypeActivationMethod::Type ActivationType) override;
    virtual void OpenAssetEditor(
        const TArray<UObject *> &InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
    virtual bool CanFilter() override { return false; }
    virtual uint32 GetCategories() override { return EAssetTypeCategories::Sounds; }

private:
    /** Returns true if only one event is selected to play */
    bool CanExecutePlayCommand(TArray<TWeakObjectPtr<UFMODEvent>> Objects) const;

    /** Handler for when Edit is selected */
    void ExecuteEdit(TArray<TWeakObjectPtr<UFMODEvent>> Objects);

    /** Handler for when Play is selected */
    void ExecutePlay(TArray<TWeakObjectPtr<UFMODEvent>> Objects);

    /** Handler for when Stop is selected */
    void ExecuteStop(TArray<TWeakObjectPtr<UFMODEvent>> Objects);

    /** Plays the event */
    void PlayEvent(UFMODEvent *Event);

    void HandleBeginPIE(bool bSimulating);
    void HandleBanksReloaded();

    FMOD::Studio::EventInstance *CurrentPreviewEventInstance;
    FDelegateHandle BeginPIEDelegateHandle;
};
