#include "FMODAssetBuilder.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "FMODAssetLookup.h"
#include "FMODAssetTable.h"
#include "FMODBank.h"
#include "FMODBankLookup.h"
#include "FMODBus.h"
#include "FMODEvent.h"
#include "FMODSettings.h"
#include "FMODSnapshot.h"
#include "FMODSnapshotReverb.h"
#include "FMODPort.h"
#include "FMODUtils.h"
#include "FMODVCA.h"
#include "FileHelpers.h"
#include "ObjectTools.h"
#include "SourceControlHelpers.h"
#include "HAL/FileManager.h"
#include "Misc/MessageDialog.h"

#include "fmod_studio.hpp"

#define LOCTEXT_NAMESPACE "FMODAssetBuilder"

FFMODAssetBuilder::~FFMODAssetBuilder()
{
    if (StudioSystem)
    {
        StudioSystem->release();
    }
}

void FFMODAssetBuilder::Create()
{
    verifyfmod(FMOD::Studio::System::create(&StudioSystem));
    FMOD::System *lowLevelSystem = nullptr;
    verifyfmod(StudioSystem->getCoreSystem(&lowLevelSystem));
    verifyfmod(lowLevelSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND_NRT));
    verifyfmod(StudioSystem->initialize(1, FMOD_STUDIO_INIT_ALLOW_MISSING_PLUGINS | FMOD_STUDIO_INIT_SYNCHRONOUS_UPDATE, FMOD_INIT_MIX_FROM_UPDATE,
        nullptr));
}

void FFMODAssetBuilder::ProcessBanks()
{
    TArray<UObject*> AssetsToSave;
    TArray<UObject*> AssetsToDelete;
    const UFMODSettings& Settings = *GetDefault<UFMODSettings>();
    FString PackagePath = Settings.GetFullContentPath() / FFMODAssetTable::PrivateDataPath();
    BuildBankLookup(FFMODAssetTable::BankLookupName(), PackagePath, Settings, AssetsToSave);
    BuildAssets(Settings, FFMODAssetTable::AssetLookupName(), PackagePath, AssetsToSave, AssetsToDelete);
    SaveAssets(AssetsToSave);
    DeleteAssets(AssetsToDelete);
}

FString FFMODAssetBuilder::GetMasterStringsBankPath()
{
    return BankLookup ? BankLookup->MasterStringsBankPath : FString();
}

void FFMODAssetBuilder::BuildAssets(const UFMODSettings& InSettings, const FString &AssetLookupName, const FString &AssetLookupPath,
    TArray<UObject*>& AssetsToSave, TArray<UObject*>& AssetsToDelete)
{
    if (!BankLookup->MasterStringsBankPath.IsEmpty())
    {
        FString StringPath = InSettings.GetFullBankPath() / BankLookup->MasterStringsBankPath;

        UE_LOG(LogFMOD, Log, TEXT("Loading strings bank: %s"), *StringPath);

        FMOD::Studio::Bank *StudioStringBank;
        FMOD_RESULT StringResult = StudioSystem->loadBankFile(TCHAR_TO_UTF8(*StringPath), FMOD_STUDIO_LOAD_BANK_NORMAL, &StudioStringBank);

        if (StringResult == FMOD_OK)
        {
            TArray<char> RawBuffer;
            RawBuffer.SetNum(256); // Initial capacity

            int Count = 0;
            verifyfmod(StudioStringBank->getStringCount(&Count));

            // Enumerate all of the names in the strings bank and gather the information required to create the UE4 assets for each object
            TArray<AssetCreateInfo> AssetCreateInfos;
            AssetCreateInfos.Reserve(Count);

            for (int StringIdx = 0; StringIdx < Count; ++StringIdx)
            {
                FMOD::Studio::ID Guid = { 0 };

                while (true)
                {
                    int ActualSize = 0;
                    FMOD_RESULT Result = StudioStringBank->getStringInfo(StringIdx, &Guid, RawBuffer.GetData(), RawBuffer.Num(), &ActualSize);

                    if (Result == FMOD_ERR_TRUNCATED)
                    {
                        RawBuffer.SetNum(ActualSize);
                    }
                    else
                    {
                        verifyfmod(Result);
                        break;
                    }
                }

                FString AssetName(UTF8_TO_TCHAR(RawBuffer.GetData()));
                FGuid AssetGuid = FMODUtils::ConvertGuid(Guid);

                if (!AssetName.IsEmpty())
                {
                    AssetCreateInfo CreateInfo = {};

                    if (MakeAssetCreateInfo(AssetGuid, AssetName, &CreateInfo))
                    {
                        AssetCreateInfos.Add(CreateInfo);
                    }
                }
            }

            verifyfmod(StudioStringBank->unload());
            verifyfmod(StudioSystem->update());

            // Load or create asset lookup
            FString AssetLookupPackageName = AssetLookupPath + AssetLookupName;
            UPackage *AssetLookupPackage = CreatePackage(*AssetLookupPackageName);
            AssetLookupPackage->FullyLoad();

            bool bAssetLookupCreated = false;
            bool bAssetLookupModified = false;

            UDataTable *AssetLookup = FindObject<UDataTable>(AssetLookupPackage, *AssetLookupName, true);

            if (!AssetLookup)
            {
                AssetLookup = NewObject<UDataTable>(AssetLookupPackage, *AssetLookupName, RF_Public | RF_Standalone | RF_MarkAsRootSet);
                AssetLookup->RowStruct = FFMODAssetLookupRow::StaticStruct();
                bAssetLookupCreated = true;
            }

            // Create a list of existing assets in the lookup - we'll use this to delete stale assets
            TMap<FName, FFMODAssetLookupRow> StaleAssets{};
            AssetLookup->ForeachRow<FFMODAssetLookupRow>(FString(), [&StaleAssets](const FName& Key, const FFMODAssetLookupRow& Value) {
                StaleAssets.Add(Key, Value);
            });

            for (const AssetCreateInfo &CreateInfo : AssetCreateInfos)
            {
                UFMODAsset *Asset = CreateAsset(CreateInfo, AssetsToSave);

                if (Asset)
                {
                    UPackage *AssetPackage = Asset->GetPackage();
                    FString AssetPackageName = AssetPackage->GetPathName();
                    FString AssetName = Asset->GetPathName(AssetPackage);
                    FName LookupRowName = FName(*CreateInfo.StudioPath);
                    FFMODAssetLookupRow* LookupRow = AssetLookup->FindRow<FFMODAssetLookupRow>(LookupRowName, FString(), false);

                    if (LookupRow)
                    {
                        if (LookupRow->PackageName != AssetPackageName || LookupRow->AssetName != AssetName)
                        {
                            LookupRow->PackageName = AssetPackageName;
                            LookupRow->AssetName = AssetName;
                            bAssetLookupModified = true;
                        }
                    }
                    else
                    {
                        FFMODAssetLookupRow NewRow{};
                        NewRow.PackageName = AssetPackageName;
                        NewRow.AssetName = AssetName;
                        AssetLookup->AddRow(LookupRowName, NewRow);
                        bAssetLookupModified = true;
                    }

                    StaleAssets.Remove(LookupRowName);
                }
            }

            // Delete stale assets
            if (StaleAssets.Num() > 0)
            {
                for (auto& Entry : StaleAssets)
                {
                    UPackage *Package = CreatePackage(*Entry.Value.PackageName);
                    Package->FullyLoad();
                    UFMODAsset *Asset = Package ?  FindObject<UFMODAsset>(Package, *Entry.Value.AssetName) : nullptr;

                    if (Asset)
                    {
                        UE_LOG(LogFMOD, Log, TEXT("Deleting stale asset %s/%s."), *Entry.Value.PackageName, *Entry.Value.AssetName);
                        AssetsToDelete.Add(Asset);
                    }

                    AssetLookup->RemoveRow(Entry.Key);
                }

                bAssetLookupModified = true;
            }

            if (bAssetLookupCreated || bAssetLookupModified)
            {
                AssetsToSave.Add(AssetLookup);
            }
        }
        else
        {
            UE_LOG(LogFMOD, Warning, TEXT("Failed to load strings bank: %s"), *StringPath);
        }
    }
}

void FFMODAssetBuilder::BuildBankLookup(const FString &AssetName, const FString &PackagePath, const UFMODSettings &InSettings,
    TArray<UObject*>& AssetsToSave)
{
    FString PackageName = PackagePath + AssetName;
    UPackage *Package = CreatePackage(*PackageName);
    Package->FullyLoad();

    bool bCreated = false;
    bool bModified = false;

    BankLookup = FindObject<UFMODBankLookup>(Package, *AssetName, true);

    if (!BankLookup)
    {
        BankLookup = NewObject<UFMODBankLookup>(Package, *AssetName, RF_Public | RF_Standalone | RF_MarkAsRootSet);
        BankLookup->DataTable = NewObject<UDataTable>(BankLookup, "DataTable", RF_NoFlags);
        BankLookup->DataTable->RowStruct = FFMODLocalizedBankTable::StaticStruct();
        bCreated = true;
    }

    // Get a list of all bank GUIDs already in the lookup - this will be used to remove stale GUIDs after processing
    // the current banks on disk.
    TArray<FName> StaleBanks(BankLookup->DataTable->GetRowNames());

    // Process all banks on disk
    TArray<FString> BankPaths;
    FString SearchDir = InSettings.GetFullBankPath();
    IFileManager::Get().FindFilesRecursive(BankPaths, *SearchDir, TEXT("*.bank"), true, false, false);

    if (BankPaths.Num() <= 0)
    {
        return;
    }

    TArray<FString> LocalizedEntriesVisited;

    for (FString BankPath : BankPaths)
    {
        FMOD::Studio::Bank* Bank;
        FMOD_GUID BankID;

        FMOD_RESULT result = StudioSystem->loadBankFile(TCHAR_TO_UTF8(*BankPath), FMOD_STUDIO_LOAD_BANK_NORMAL, &Bank);
        if (result == FMOD_OK)
        {
            result = Bank->getID(&BankID);
            Bank->unload();
        }
        if (result != FMOD_OK)
        {
            UE_LOG(LogFMOD, Error, TEXT("Failed to add bank %s to lookup."), *BankPath);
            continue;
        }
        
        FString GUID = FMODUtils::ConvertGuid(BankID).ToString(EGuidFormats::DigitsWithHyphensInBraces);
        FName OuterRowName(*GUID);

        FFMODLocalizedBankTable* Row = BankLookup->DataTable->FindRow<FFMODLocalizedBankTable>(OuterRowName, nullptr, false);
        if (Row)
        {
            StaleBanks.RemoveSingle(OuterRowName);
        }
        else
        {
            FFMODLocalizedBankTable NewRow{};
            NewRow.Banks = NewObject<UDataTable>(BankLookup->DataTable, *BankPath, RF_NoFlags);
            NewRow.Banks->RowStruct = FFMODLocalizedBankRow::StaticStruct();
            BankLookup->DataTable->AddRow(OuterRowName, NewRow);
            Row = BankLookup->DataTable->FindRow<FFMODLocalizedBankTable>(OuterRowName, nullptr, false);
            bModified = true;
        }

        // Set InnerRowName to either "<NON-LOCALIZED>" or a locale code based on the BankPath e.g. "JP"
        FName InnerRowName("<NON-LOCALIZED>");
        for (const FFMODProjectLocale& Locale : InSettings.Locales)
        {
            // Remove all expected extensions from end of filename before checking for locale code.
            // Note, we may encounter multiple extensions e.g. "Dialogue.assets.bank"
            const FString BankExtensions[] = { TEXT(".assets"), TEXT(".streams"), TEXT(".strings") };
            FString Filename = FPaths::GetCleanFilename(BankPath);
            Filename.RemoveFromEnd(TEXT(".bank"));
            for (const FString& extension : BankExtensions)
            {
                Filename.RemoveFromEnd(extension);
            }

            if (Filename.EndsWith(FString("_") + Locale.LocaleCode))
            {
                InnerRowName = FName(*Locale.LocaleCode);
                break;
            }
        }

        // See if we've visited this OuterRowName + InnerRowName already and skip it if so. This is mainly to 
        // avoid setting "<NON-LOCALIZED>" multiple times (and causing BankLookup to be modified) when no 
        // locales are set up in Unreal.
        FString LocalizedEntryKey = OuterRowName.ToString() + InnerRowName.ToString();
        if (LocalizedEntriesVisited.Find(LocalizedEntryKey) != INDEX_NONE)
        {
            UE_LOG(LogFMOD, Warning, TEXT("Ignoring bank %s as another bank with the same GUID is already being used.\n"
                "Bank %s does not match any locales in the FMOD Studio plugin settings."), *BankPath, *BankPath);
            continue;
        }
        LocalizedEntriesVisited.Add(LocalizedEntryKey);

        FFMODLocalizedBankRow* InnerRow = Row->Banks->FindRow<FFMODLocalizedBankRow>(InnerRowName, nullptr, false);
        FString RelativeBankPath = BankPath.RightChop(InSettings.GetFullBankPath().Len() + 1);

        if (InnerRow)
        {
            if (InnerRow->Path != RelativeBankPath)
            {
                InnerRow->Path = RelativeBankPath;
                bModified = true;
            }
        }
        else
        {
            FFMODLocalizedBankRow NewRow{};
            NewRow.Path = RelativeBankPath;
            Row->Banks->AddRow(InnerRowName, NewRow);
            bModified = true;
        }

        FString CurFilename = FPaths::GetCleanFilename(BankPath);

        if (CurFilename == InSettings.GetMasterBankFilename() && BankLookup->MasterBankPath != RelativeBankPath)
        {
            BankLookup->MasterBankPath = RelativeBankPath;
            bModified = true;
        }
        else if (CurFilename == InSettings.GetMasterStringsBankFilename() && BankLookup->MasterStringsBankPath != RelativeBankPath)
        {
            BankLookup->MasterStringsBankPath = RelativeBankPath;
            bModified = true;
        }
        else if (CurFilename == InSettings.GetMasterAssetsBankFilename() && BankLookup->MasterAssetsBankPath != RelativeBankPath)
        {
            BankLookup->MasterAssetsBankPath = RelativeBankPath;
            bModified = true;
        }
    }

    StudioSystem->flushCommands();

    // Remove stale banks from lookup
    if (StaleBanks.Num() > 0)
    {
        for (const auto& RowName : StaleBanks)
        {
            BankLookup->DataTable->RemoveRow(RowName);
        }

        bModified = true;
    }

    // Remove stale localized bank entries from lookup
    for (auto& outer : BankLookup->DataTable->GetRowMap())
    {
        FName outerrowname = outer.Key;
        FFMODLocalizedBankTable* outerrow = reinterpret_cast<FFMODLocalizedBankTable*>(outer.Value);
        TArray<FName> RowsToRemove;
        for (auto& inner : outerrow->Banks->GetRowMap())
        {
            FName innerrowname = inner.Key;
            FFMODLocalizedBankRow* innerrow = reinterpret_cast<FFMODLocalizedBankRow*>(inner.Value);
            FString LocalizedEntryKey = outerrowname.ToString() + innerrowname.ToString();
            if (LocalizedEntriesVisited.Find(LocalizedEntryKey) == INDEX_NONE)
            {
                RowsToRemove.Add(innerrowname);
            }
        }
        for (auto& rowname : RowsToRemove)
        {
            outerrow->Banks->RemoveRow(rowname);
            bModified = true;
        }
    }

    if (bCreated)
    {
        UE_LOG(LogFMOD, Log, TEXT("BankLookup created.\n"));
        FAssetRegistryModule::AssetCreated(BankLookup);
    }
    
    if (bCreated || bModified)
    {
        UE_LOG(LogFMOD, Log, TEXT("BankLookup modified.\n"));
        AssetsToSave.Add(BankLookup);
    }

    UE_LOG(LogFMOD, Log, TEXT("===== BankLookup =====\n"));
    for (auto& outer : BankLookup->DataTable->GetRowMap())
    {
        FName outerrowname = outer.Key;
        FFMODLocalizedBankTable* outerrow = reinterpret_cast<FFMODLocalizedBankTable*>(outer.Value);
        UE_LOG(LogFMOD, Log, TEXT("GUID: %s\n"), *(outerrowname.ToString()));
        for (auto& inner : outerrow->Banks->GetRowMap())
        {
            FName innerrowname = inner.Key;
            FFMODLocalizedBankRow* innerrow = reinterpret_cast<FFMODLocalizedBankRow*>(inner.Value);
            UE_LOG(LogFMOD, Log, TEXT("  Locale: %s   Path: %s\n"), *(innerrowname.ToString()), *innerrow->Path);
        }
    }
}

FString FFMODAssetBuilder::GetAssetClassName(UClass* AssetClass)
{
    FString ClassName("");

    if (AssetClass == UFMODEvent::StaticClass())
    {
        ClassName = TEXT("Events");
    }
    else if (AssetClass == UFMODSnapshot::StaticClass())
    {
        ClassName = TEXT("Snapshots");
    }
    else if (AssetClass == UFMODBank::StaticClass())
    {
        ClassName = TEXT("Banks");
    }
    else if (AssetClass == UFMODBus::StaticClass())
    {
        ClassName = TEXT("Buses");
    }
    else if (AssetClass == UFMODVCA::StaticClass())
    {
        ClassName = TEXT("VCAs");
    }
    else if (AssetClass == UFMODSnapshotReverb::StaticClass())
    {
        ClassName = TEXT("Reverbs");
    }
    else if (AssetClass == UFMODPort::StaticClass())
    {
        ClassName = TEXT("Ports");
    }
    return ClassName;
}

bool FFMODAssetBuilder::MakeAssetCreateInfo(const FGuid &AssetGuid, const FString &StudioPath, AssetCreateInfo *CreateInfo)
{
    CreateInfo->StudioPath = StudioPath;
    CreateInfo->Guid = AssetGuid;

    FString AssetType;
    FString AssetPath;
    StudioPath.Split(TEXT(":"), &AssetType, &AssetPath, ESearchCase::CaseSensitive, ESearchDir::FromStart);

    if (AssetType.Equals(TEXT("event")))
    {
        CreateInfo->Class = UFMODEvent::StaticClass();
    }
    else if (AssetType.Equals(TEXT("snapshot")))
    {
        CreateInfo->Class = UFMODSnapshot::StaticClass();
    }
    else if (AssetType.Equals(TEXT("bank")))
    {
        CreateInfo->Class = UFMODBank::StaticClass();
    }
    else if (AssetType.Equals(TEXT("bus")))
    {
        CreateInfo->Class = UFMODBus::StaticClass();
    }
    else if (AssetType.Equals(TEXT("vca")))
    {
        CreateInfo->Class = UFMODVCA::StaticClass();
    }
    else if (AssetType.Equals(TEXT("port")))
    {
        CreateInfo->Class = UFMODPort::StaticClass();
    }
    else if (AssetType.Equals(TEXT("parameter")))
    {
        return false;
    }
    else
    {
        UE_LOG(LogFMOD, Warning, TEXT("Unknown asset type: %s"), *AssetType);
        CreateInfo->Class = UFMODAsset::StaticClass();
    }

    AssetPath.Split(TEXT("/"), &(CreateInfo->Path), &(CreateInfo->AssetName), ESearchCase::CaseSensitive, ESearchDir::FromEnd);

    if (CreateInfo->AssetName.IsEmpty() || CreateInfo->AssetName.Contains(TEXT(".strings")))
    {
        return false;
    }
    return true;
}

UFMODAsset *FFMODAssetBuilder::CreateAsset(const AssetCreateInfo& CreateInfo, TArray<UObject*>& AssetsToSave)
{
    FString SanitizedAssetName;
    FText OutReason;

    if (FName::IsValidXName(CreateInfo.AssetName, INVALID_OBJECTNAME_CHARACTERS, &OutReason))
    {
        SanitizedAssetName = CreateInfo.AssetName;
    }
    else
    {
        SanitizedAssetName = ObjectTools::SanitizeObjectName(CreateInfo.AssetName);
        UE_LOG(LogFMOD, Log, TEXT("'%s' cannot be used as a UE4 asset name. %s. Using '%s' instead."), *CreateInfo.AssetName,
            *OutReason.ToString(), *SanitizedAssetName);
    }

    const UFMODSettings &Settings = *GetDefault<UFMODSettings>();
    FString Folder = Settings.GetFullContentPath() / GetAssetClassName(CreateInfo.Class) + CreateInfo.Path;
    FString PackagePath = FString::Printf(TEXT("%s/%s"), *Folder, *SanitizedAssetName);
    FString SanitizedPackagePath;

    if (FName::IsValidXName(PackagePath, INVALID_LONGPACKAGE_CHARACTERS, &OutReason))
    {
        SanitizedPackagePath = PackagePath;
    }
    else
    {
        SanitizedPackagePath = ObjectTools::SanitizeInvalidChars(PackagePath, INVALID_LONGPACKAGE_CHARACTERS);
        UE_LOG(LogFMOD, Log, TEXT("'%s' cannot be used as a UE4 asset path. %s. Using '%s' instead."), *PackagePath, *OutReason.ToString(),
            *SanitizedPackagePath);
    }

    UPackage *Package = CreatePackage(*SanitizedPackagePath);
    Package->FullyLoad();

    UFMODAsset *Asset = FindObject<UFMODAsset>(Package, *SanitizedAssetName);
    bool bCreated = false;
    bool bModified = false;

    if (Asset && Asset->GetClass() == CreateInfo.Class)
    {
        if (Asset->AssetGuid != CreateInfo.Guid)
        {
            UE_LOG(LogFMOD, Log, TEXT("Updating asset: %s"), *SanitizedPackagePath);
            Asset->AssetGuid = CreateInfo.Guid;
            bModified = true;
        }
    }
    else
    {
        UE_LOG(LogFMOD, Log, TEXT("Adding asset: %s"), *SanitizedPackagePath);
        Asset = NewObject<UFMODAsset>(Package, CreateInfo.Class, FName(*SanitizedAssetName), RF_Standalone | RF_Public | RF_MarkAsRootSet);
        Asset->AssetGuid = CreateInfo.Guid;
        bCreated = true;
    }

    if (bCreated)
    {
        FAssetRegistryModule::AssetCreated(Asset);
    }

    if (bCreated || bModified)
    {
        AssetsToSave.Add(Asset);
    }

    if (!IsValid(Asset))
    {
        UE_LOG(LogFMOD, Error, TEXT("Failed to construct asset: %s"), *SanitizedPackagePath);
    }

    if (CreateInfo.Class == UFMODSnapshot::StaticClass())
    {
        FString OldPrefix = Settings.ContentBrowserPrefix + GetAssetClassName(Asset->GetClass());
        FString NewPrefix = Settings.ContentBrowserPrefix + GetAssetClassName(UFMODSnapshotReverb::StaticClass());
        UObject *Outer = Asset->GetOuter() ? Asset->GetOuter() : Asset;
        FString ReverbPackagePath = Outer->GetPathName().Replace(*OldPrefix, *NewPrefix);

        UPackage *ReverbPackage = CreatePackage(*ReverbPackagePath);
        ReverbPackage->FullyLoad();

        UFMODSnapshotReverb *AssetReverb = FindObject<UFMODSnapshotReverb>(ReverbPackage, *SanitizedAssetName, true);
        bCreated = false;
        bModified = false;

        if (AssetReverb)
        {
            if (AssetReverb->AssetGuid != CreateInfo.Guid)
            {
                UE_LOG(LogFMOD, Log, TEXT("Updating snapshot reverb asset: %s"), *ReverbPackagePath);
                AssetReverb->AssetGuid = CreateInfo.Guid;
                bModified = true;
            }
        }
        else
        {
            UE_LOG(LogFMOD, Log, TEXT("Constructing snapshot reverb asset: %s"), *ReverbPackagePath);
            AssetReverb = NewObject<UFMODSnapshotReverb>(ReverbPackage, UFMODSnapshotReverb::StaticClass(), FName(*SanitizedAssetName),
                RF_Standalone | RF_Public | RF_MarkAsRootSet);
            AssetReverb->AssetGuid = CreateInfo.Guid;
            bCreated = true;
        }

        if (bCreated)
        {
            FAssetRegistryModule::AssetCreated(AssetReverb);
        }

        if (bCreated || bModified)
        {
            AssetsToSave.Add(AssetReverb);
        }

        if (!IsValid(AssetReverb))
        {
            UE_LOG(LogFMOD, Error, TEXT("Failed to construct snapshot reverb asset: %s"), *ReverbPackagePath);
        }
    }

    return Asset;
}

void FFMODAssetBuilder::SaveAssets(TArray<UObject*>& AssetsToSave)
{
    if (AssetsToSave.Num() == 0)
    {
        return;
    }

    TArray<UPackage *> PackagesToSave;

    for (auto& Asset : AssetsToSave)
    {
        UPackage* Package = Asset->GetPackage();

        if (Package)
        {
            Package->MarkPackageDirty();
            PackagesToSave.Add(Package);
        }
    }

    UEditorLoadingAndSavingUtils::SavePackages(PackagesToSave, true);
}

void FFMODAssetBuilder::DeleteAssets(TArray<UObject*>& AssetsToDelete)
{
    if (AssetsToDelete.Num() == 0)
    {
        return;
    }

    TArray<UObject*> ObjectsToDelete;

    for (auto& Asset : AssetsToDelete)
    {
        ObjectsToDelete.Add(Asset);

        if (Asset->GetClass() == UFMODSnapshot::StaticClass())
        {
            // Also delete the reverb asset
            const UFMODSettings &Settings = *GetDefault<UFMODSettings>();
            FString OldPrefix = Settings.ContentBrowserPrefix + GetAssetClassName(Asset->GetClass());
            FString NewPrefix = Settings.ContentBrowserPrefix + GetAssetClassName(UFMODSnapshotReverb::StaticClass());
            FString ReverbName = Asset->GetPathName().Replace(*OldPrefix, *NewPrefix);
            UObject *Reverb = StaticFindObject(UFMODSnapshotReverb::StaticClass(), nullptr, *ReverbName);

            if (Reverb)
            {
                ObjectsToDelete.Add(Reverb);
            }
        }
    }

    // Use ObjectTools to delete assets - ObjectTools::DeleteObjects handles confirmation, source control, and making read only files writables
    ObjectTools::DeleteObjects(ObjectsToDelete, !IsRunningCommandlet());
}

#undef LOCTEXT_NAMESPACE