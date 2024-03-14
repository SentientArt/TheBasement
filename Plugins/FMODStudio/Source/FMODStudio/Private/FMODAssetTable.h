// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#pragma once

#include "UObject/GCObject.h"

class UDataTable;
class UFMODAsset;
class UFMODBank;
class UFMODBankLookup;

class FFMODAssetTable : public FGCObject
{
public:
    FFMODAssetTable();

    //~ FGCObject
    void AddReferencedObjects(FReferenceCollector& Collector) override;
    virtual FString GetReferencerName() const override
    {
        return TEXT("FFMODAssetTable");
    }

    void Load();

    FString GetBankPath(const UFMODBank &Bank) const;
    FString GetMasterBankPath() const;
    FString GetMasterStringsBankPath() const;
    FString GetMasterAssetsBankPath() const;
    void SetLocale(const FString &LocaleCode);
    FString GetLocale() const;
    void GetAllBankPaths(TArray<FString> &BankPaths, bool IncludeMasterBank) const;

    UFMODAsset *GetAssetByStudioPath(const FString &InStudioPath) const;

    static inline FString PrivateDataPath() { return FString(TEXT("PrivateIntegrationData/")); }
    static inline FString BankLookupName()  { return FString(TEXT("BankLookup")); }
    static inline FString AssetLookupName() { return FString(TEXT("AssetLookup")); }

private:
    FString GetBankPathByGuid(const FGuid& Guid) const;
    FString GetLocalizedBankPath(const UDataTable* BankTable) const;

    FString ActiveLocale;
    UFMODBankLookup *BankLookup;
    UDataTable *AssetLookup;
};
