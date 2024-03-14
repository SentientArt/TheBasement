/*
* Copyright (c) 2020 - 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
*
* NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
* property and proprietary rights in and to this material, related
* documentation and any modifications thereto. Any use, reproduction,
* disclosure or distribution of this material and related documentation
* without an express license agreement from NVIDIA CORPORATION or
* its affiliates is strictly prohibited.
*/
#include "DLSSEditor.h"

#include "DLSSUpscaler.h"
#include "DLSS.h"
#include "DLSSSettings.h"
#include "NGXRHI.h"

#include "CoreMinimal.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "UObject/Class.h"
#include "UObject/WeakObjectPtr.h"



#define LOCTEXT_NAMESPACE "FDLSSEditorModule"

DEFINE_LOG_CATEGORY(LogDLSSEditor);

void FDLSSEditorModule::StartupModule()
{
	UE_LOG(LogDLSSEditor, Log, TEXT("%s Enter"), ANSI_TO_TCHAR(__FUNCTION__));
	
	check(GIsEditor);
	
	// verify that the other DLSS modules are correctly hooked up
	{
		IDLSSModuleInterface* DLSSModule = &FModuleManager::LoadModuleChecked<IDLSSModuleInterface>(TEXT("DLSS"));
		UE_LOG(LogDLSSEditor, Log, TEXT("DLSS module %p, QueryDLSSSupport = %u DLSSUpscaler = %p"), DLSSModule, static_cast<int>(DLSSModule->QueryDLSSSRSupport()), DLSSModule->GetDLSSUpscaler());
		
		bIsDLSSAvailable = DLSSModule->QueryDLSSSRSupport() == EDLSSSupport::Supported;
	}

	// register settings
	{
		ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
		if (SettingsModule != nullptr)
		{
			{
				auto Settings = GetMutableDefault<UDLSSSettings>();
				if(bIsDLSSAvailable)
				{
					IDLSSModuleInterface* DLSSModule = &FModuleManager::LoadModuleChecked<IDLSSModuleInterface>(TEXT("DLSS"));
					const NGXRHI* NGXRHIExtensions = DLSSModule->GetDLSSUpscaler()->GetNGXRHI();
					Settings->GenericDLSSBinaryPath = NGXRHIExtensions->GetDLSSGenericBinaryInfo().Get<0>();
					Settings->bGenericDLSSBinaryExists = NGXRHIExtensions->GetDLSSGenericBinaryInfo().Get<1>();

					Settings->CustomDLSSBinaryPath = NGXRHIExtensions->GetDLSSCustomBinaryInfo().Get<0>();
					Settings->bCustomDLSSBinaryExists = NGXRHIExtensions->GetDLSSCustomBinaryInfo().Get<1>();
				}

				ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "DLSS",
					LOCTEXT("DLSSSettingsName", "NVIDIA DLSS"),
					LOCTEXT("DLSSSettingsDescription", "Configure the NVIDIA DLSS plug-in."),
					Settings
				);
			}

			{
				auto Settings = GetMutableDefault<UDLSSOverrideSettings>();

				ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "DLSSOverride",
					LOCTEXT("DLSSOverrideSettingsName", "NVIDIA DLSS Overrides (Local)"),
					LOCTEXT("DLSSOverrideSettingsDescription", "Configure the local settings for the NVIDIA DLSS plug-in."),
					Settings
				);
			}
		}
	}

	UE_LOG(LogDLSSEditor, Log, TEXT("%s Leave"), ANSI_TO_TCHAR(__FUNCTION__));
}

void FDLSSEditorModule::ShutdownModule()
{
	UE_LOG(LogDLSSEditor, Log, TEXT("%s Enter"), ANSI_TO_TCHAR(__FUNCTION__));
	
	UE_LOG(LogDLSSEditor, Log, TEXT("%s Leave"), ANSI_TO_TCHAR(__FUNCTION__));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FDLSSEditorModule, DLSSEditor)

