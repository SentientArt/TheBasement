// Copyright (c), Firelight Technologies Pty, Ltd.

#include "FMODGenerateAssetsCommandlet.h"

#include "FMODSettings.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Editor.h"
#include "Editor/UnrealEd/Public/FileHelpers.h"
#include "HAL/PlatformFileManager.h"
#include "../Classes/FMODAssetBuilder.h"

DEFINE_LOG_CATEGORY_STATIC(LogFMODCommandlet, Log, All);

static constexpr auto RebuildSwitch = TEXT("rebuild");

UFMODGenerateAssetsCommandlet::UFMODGenerateAssetsCommandlet(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

int32 UFMODGenerateAssetsCommandlet::Main(const FString& CommandLine)
{
    int32 returnCode = 0;

#if WITH_EDITOR

    FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);
    IAssetRegistry& AssetRegistry = assetRegistryModule.Get();

    const UFMODSettings& Settings = *GetDefault<UFMODSettings>();

    TArray<FString> Tokens, Switches;
    TMap<FString, FString> Params;
    ParseCommandLine(*CommandLine, Tokens, Switches, Params);

    // Rebuild switch
    if (Switches.Contains(RebuildSwitch))
    {
        IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

        /*
            Combine the ProjectContentDir + ContentBrowserPrefix to make a filesystem path
            to where the FMOD generated assets directories live e.g. 
            ../../../../UnrealProjects/MyProject/Content/FMOD
            Should work for non-default values of ContentBrowserPrefix e.g. /Game/foo/bar/baz/
        */
        FString folderPath = Settings.ContentBrowserPrefix.TrimChar('/');   // /Game/FMOD/ -> Game/FMOD
        folderPath.Split(TEXT("/"), 0, &folderPath);                        // Game/FMOD -> FMOD
        folderPath = FPaths::ProjectContentDir() + folderPath + "/";        // FMOD -> ../../../../UnrealProjects/MyProject/Content/FMOD/

        for (FString folder : Settings.GeneratedFolders)
        {
            FString FolderToDelete = folderPath + folder;
            bool removed = FileManager.DeleteDirectoryRecursively(*FolderToDelete);
            if (!removed)
            {
                UE_LOG(LogFMODCommandlet, Warning, TEXT("Unable to delete '%s'."), *FolderToDelete);
            }
        }
    }

    // Ensure AssetRegistry is up to date
    TArray<FString> InPaths;
    InPaths.Add(Settings.GetFullContentPath());
    AssetRegistry.ScanPathsSynchronous(InPaths);
    while (AssetRegistry.IsLoadingAssets())
    {
        AssetRegistry.Tick(1.0f);
    }

    FFMODAssetBuilder assetBuilder;
    if (!IsEngineExitRequested())
    {
        assetBuilder.Create();
        assetBuilder.ProcessBanks();
    }
#endif

	return returnCode;
}