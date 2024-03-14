// Copyright (c), Firelight Technologies Pty, Ltd. 2023-2023.

#include "FMODAudioLinkSettingsFactory.h"
#include "FMODAudioLinkSettings.h"
#include "AssetTypeCategories.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText FAssetTypeActions_FMODAudioLinkSettings::GetName() const
{
	return LOCTEXT("AssetTypeActions_FMODAudioLinkSettings", "FMOD AudioLink Settings");
}

FColor FAssetTypeActions_FMODAudioLinkSettings::GetTypeColor() const
{
	return FColor(100, 100, 100);
}

const TArray<FText>& FAssetTypeActions_FMODAudioLinkSettings::GetSubMenus() const
{
	static const TArray<FText> SubMenus
	{
		LOCTEXT("AssetAudioLinkSubMenu", "AudioLink")
	};

	return SubMenus;
}

UClass* FAssetTypeActions_FMODAudioLinkSettings::GetSupportedClass() const
{
	return UFMODAudioLinkSettings::StaticClass();
}

uint32 FAssetTypeActions_FMODAudioLinkSettings::GetCategories()
{
	return EAssetTypeCategories::Sounds;
}

UFMODAudioLinkSettingsFactory::UFMODAudioLinkSettingsFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UFMODAudioLinkSettings::StaticClass();

	bCreateNew = true;
	bEditorImport = true;
	bEditAfterNew = true;
}

UObject* UFMODAudioLinkSettingsFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UFMODAudioLinkSettings>(InParent, Name, Flags);
}

uint32 UFMODAudioLinkSettingsFactory::GetMenuCategories() const
{
	return EAssetTypeCategories::Sounds;
}
#undef LOCTEXT_NAMESPACE