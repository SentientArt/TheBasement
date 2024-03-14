// Copyright (c), Firelight Technologies Pty, Ltd.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FMODAssetLookup.generated.h"

USTRUCT()
struct FMODSTUDIO_API FFMODAssetLookupRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category="FMOD|Internal|AssetLookup")
    FString PackageName;

    UPROPERTY(VisibleAnywhere, Category="FMOD|Internal|AssetLookup")
    FString AssetName;
};
