// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#include "FMODAssetTable.h"

#include "FMODAssetLookup.h"
#include "FMODEvent.h"
#include "FMODSnapshot.h"
#include "FMODSnapshotReverb.h"
#include "FMODBank.h"
#include "FMODBankLookup.h"
#include "FMODBus.h"
#include "FMODVCA.h"
#include "FMODUtils.h"
#include "FMODSettings.h"
#include "FMODFileCallbacks.h"
#include "FMODStudioPrivatePCH.h"
#include "fmod_studio.hpp"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"
#include "UObject/Package.h"

FFMODAssetTable::FFMODAssetTable()
    : ActiveLocale(FString()),
      BankLookup(nullptr),
      AssetLookup(nullptr)
{
}

void FFMODAssetTable::AddReferencedObjects(FReferenceCollector& Collector)
{
    // The garbage collector will clean up any objects which aren't referenced, doing this tells the garbage collector our lookups are referenced
    // (the GC knows not to remove objects referenced by a UPROPERTY, doing this manually is required because our members aren't UPROPERTYs)
    if (BankLookup)
    {
        Collector.AddReferencedObject(BankLookup);
    }

    if (AssetLookup)
    {
        Collector.AddReferencedObject(AssetLookup);
    }
}

void FFMODAssetTable::Load()
{
    const UFMODSettings &Settings = *GetDefault<UFMODSettings>();
    FString PackagePath = Settings.GetFullContentPath() / PrivateDataPath();

    FString PackageName = PackagePath + BankLookupName();
    UPackage *Package = CreatePackage(*PackageName);
    Package->FullyLoad();
    BankLookup = FindObject<UFMODBankLookup>(Package, *BankLookupName(), true);

    if (BankLookup)
    {
        UE_LOG(LogFMOD, Display, TEXT("Loaded bank lookup"));
    }
    else
    {
        if (IsRunningCommandlet())
        {
            // If we're running in a commandlet (maybe we're cooking or running FMODGenerateAssets
            // commandlet) Display a message but don't cause the build to Error out.
            UE_LOG(LogFMOD, Display, TEXT("Failed to load bank lookup"));
        }
        else
        {
            // If we're running in game or in editor, log this as an Error
            UE_LOG(LogFMOD, Error, TEXT("Failed to load bank lookup"));
        }
    }

    PackageName = PackagePath + AssetLookupName();
    Package = CreatePackage(*PackageName);
    Package->FullyLoad();
    AssetLookup = FindObject<UDataTable>(Package, *AssetLookupName(), true);

    if (AssetLookup)
    {
        UE_LOG(LogFMOD, Display, TEXT("Loaded asset lookup"));
    }
    else
    {
        if (IsRunningCommandlet())
        {
            // If we're running in a commandlet (maybe we're cooking or running FMODGenerateAssets
            // commandlet) Display a message but don't cause the build to Error out.
            UE_LOG(LogFMOD, Display, TEXT("Failed to load asset lookup"));
        }
        else
        {
            // If we're running in game or in editor, log this as an Error
            UE_LOG(LogFMOD, Error, TEXT("Failed to load asset lookup"));
        }
    }
}

FString FFMODAssetTable::GetBankPathByGuid(const FGuid& Guid) const
{
    FString BankPath;

    if (!BankLookup)
    {
        UE_LOG(LogFMOD, Error, TEXT("Bank lookup not loaded"));
        return BankPath;
    }

    FString GUID = Guid.ToString(EGuidFormats::DigitsWithHyphensInBraces);
    FName BankTableName(*GUID);
    FFMODLocalizedBankTable *BankTable = BankLookup->DataTable->FindRow<FFMODLocalizedBankTable>(BankTableName, nullptr, false);

    if (BankTable)
    {
        BankPath = GetLocalizedBankPath(BankTable->Banks);
    }
    
    return BankPath;
}

FString FFMODAssetTable::GetLocalizedBankPath(const UDataTable* BankTable) const
{
    FName RowName(*ActiveLocale);
    FFMODLocalizedBankRow *Row = BankTable->FindRow<FFMODLocalizedBankRow>(RowName, nullptr, false);

    if (!Row)
    {
        RowName = FName("<NON-LOCALIZED>");
        Row = BankTable->FindRow<FFMODLocalizedBankRow>(RowName, nullptr, false);
    }

    FString BankPath;

    if (Row)
    {
        BankPath = Row->Path;
    }

    return BankPath;
}

FString FFMODAssetTable::GetBankPath(const UFMODBank &Bank) const
{
    FString BankPath = GetBankPathByGuid(Bank.AssetGuid);

    if (BankPath.IsEmpty())
    {
        UE_LOG(LogFMOD, Warning, TEXT("Could not find disk file for bank %s"), *Bank.GetName());
    }

    return BankPath;
}

FString FFMODAssetTable::GetMasterBankPath() const
{
    return BankLookup ? BankLookup->MasterBankPath : FString();
}

FString FFMODAssetTable::GetMasterStringsBankPath() const
{
    return BankLookup ? BankLookup->MasterStringsBankPath : FString();
}

FString FFMODAssetTable::GetMasterAssetsBankPath() const
{
    return BankLookup ? BankLookup->MasterAssetsBankPath : FString();
}

void FFMODAssetTable::SetLocale(const FString &LocaleCode)
{
    ActiveLocale = LocaleCode;
}

FString FFMODAssetTable::GetLocale() const
{
    return ActiveLocale;
}

void FFMODAssetTable::GetAllBankPaths(TArray<FString> &Paths, bool IncludeMasterBank) const
{
    if (BankLookup)
    {
        const UFMODSettings &Settings = *GetDefault<UFMODSettings>();

        BankLookup->DataTable->ForeachRow<FFMODLocalizedBankTable>(nullptr, [this, &Paths, IncludeMasterBank, &Settings](const FName &, const FFMODLocalizedBankTable& OuterRow) {
            FString BankPath = GetLocalizedBankPath(OuterRow.Banks);
            bool Skip = false;

            if (BankPath.IsEmpty())
            {
                // Never expect to be in here, but should skip empty paths
                return;
            }

            if (!IncludeMasterBank)
            {
                Skip = (BankPath == Settings.GetMasterBankFilename() || BankPath == Settings.GetMasterAssetsBankFilename() || BankPath == Settings.GetMasterStringsBankFilename());
            }

            if (!Skip)
            {
                Paths.Push(Settings.GetFullBankPath() / BankPath);
            }
        });
    }
    else
    {
        UE_LOG(LogFMOD, Error, TEXT("Bank lookup not loaded"));
    }
}

UFMODAsset *FFMODAssetTable::GetAssetByStudioPath(const FString &InStudioPath) const
{
    UFMODAsset *Asset = nullptr;

    if (AssetLookup)
    {
        FFMODAssetLookupRow *Row = AssetLookup->FindRow<FFMODAssetLookupRow>(FName(*InStudioPath), nullptr);

        if (Row)
        {
            UPackage *Package = CreatePackage(*(Row->PackageName));
            Package->FullyLoad();
            Asset = FindObject<UFMODAsset>(Package, *(Row->AssetName));
        }

    }

    return Asset;
}
