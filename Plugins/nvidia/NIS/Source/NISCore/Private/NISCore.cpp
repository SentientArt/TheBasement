/*
* Copyright (c) 2022 - 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
*
* NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
* property and proprietary rights in and to this material, related
* documentation and any modifications thereto. Any use, reproduction,
* disclosure or distribution of this material and related documentation
* without an express license agreement from NVIDIA CORPORATION or
* its affiliates is strictly prohibited.
*/
#include "NISCore.h"
#include "CoreMinimal.h"
#include "NISShaders.h"
#include "NISUpscaler.h"

#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#include "GeneralProjectSettings.h"

#include "SceneViewExtension.h"
#include "SceneView.h"
#include "Misc/MessageDialog.h"


#define LOCTEXT_NAMESPACE "FNISModule"

DEFINE_LOG_CATEGORY(LogNIS);




void FNISCoreModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(LogNIS, Log, TEXT("%s Enter"), ANSI_TO_TCHAR(__FUNCTION__));

	FNVImageUpscaler::RegisterOnScreenMessageHandler();
	{
		NISViewExtension = FSceneViewExtensions::NewExtension<FNISViewExtension>();
	}


	UE_LOG(LogNIS, Log, TEXT("%s Leave"), ANSI_TO_TCHAR(__FUNCTION__));
}


void FNISCoreModule::ShutdownModule()
{
	UE_LOG(LogNIS, Log, TEXT("%s Enter"), ANSI_TO_TCHAR(__FUNCTION__));



	{
		NISViewExtension = nullptr;
	}
	FNVImageUpscaler::RemoveOnScreenMessageHandler();

	UE_LOG(LogNIS, Log, TEXT("%s Leave"), ANSI_TO_TCHAR(__FUNCTION__));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FNISCoreModule, NISCore)
	


