// Copyright (c), Firelight Technologies Pty, Ltd. 2023-2023.

#pragma once

#include "Factories/Factory.h"
#include "AssetTypeActions_Base.h"
#include "AssetToolsModule.h"

#include "FMODAudioLinkSettingsFactory.generated.h"

UCLASS(hidecategories = Object, MinimalAPI)
class UFMODAudioLinkSettingsFactory : public UFactory
{
    GENERATED_UCLASS_BODY()

    virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context,
        FFeedbackContext* Warn) override;

    virtual uint32 GetMenuCategories() const override;
};

class FAssetTypeActions_FMODAudioLinkSettings : public FAssetTypeActions_Base
{
public:
    virtual FText GetName() const override;
    virtual FColor GetTypeColor() const override;
    virtual const TArray<FText>& GetSubMenus() const override;
    virtual UClass* GetSupportedClass() const override;
    virtual uint32 GetCategories() override;
};