// Copyright (c), Firelight Technologies Pty, Ltd.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FMODBankLookup.generated.h"

USTRUCT()
struct FMODSTUDIO_API FFMODLocalizedBankRow : public FTableRowBase
{
    GENERATED_BODY()
    UPROPERTY(VisibleAnywhere, Category="FMOD|Internal|BankLookup")
    FString Path;
};

USTRUCT()
struct FMODSTUDIO_API FFMODLocalizedBankTable : public FTableRowBase
{
    GENERATED_BODY()
    UPROPERTY(VisibleAnywhere, Category="FMOD|Internal|BankLookup")
    UDataTable *Banks = nullptr;
};

UCLASS()
class FMODSTUDIO_API UFMODBankLookup : public UObject
{
    GENERATED_UCLASS_BODY()

public:
    UPROPERTY(VisibleAnywhere, Category="FMOD|Internal|BankLookup")
    UDataTable *DataTable;

    UPROPERTY(VisibleAnywhere, Category="FMOD|Internal|BankLookup")
    FString MasterBankPath;

    UPROPERTY(VisibleAnywhere, Category="FMOD|Internal|BankLookup")
    FString MasterAssetsBankPath;

    UPROPERTY(VisibleAnywhere, Category="FMOD|Internal|BankLookup")
    FString MasterStringsBankPath;
};
