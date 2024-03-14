// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#include "FMODStudioStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Editor/EditorStyle/Public/Interfaces/IEditorStyleModule.h"
#include "Modules/ModuleManager.h"


//////////////////////////////////////////////////////////////////////////
// FFMODStudioStyle

TSharedPtr<FSlateStyleSet> FFMODStudioStyle::StyleInstance = NULL;

void FFMODStudioStyle::Initialize()
{
    if (!StyleInstance.IsValid())
    {
        StyleInstance = Create();
        FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
    }
}

#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(Style->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)

TSharedRef<FSlateStyleSet> FFMODStudioStyle::Create()
{
    TSharedRef<FSlateStyleSet> Style = MakeShared<FSlateStyleSet>(TEXT("FMODStudioStyle"));

    Style->SetContentRoot(FPaths::EngineContentDir() / TEXT("Editor/Slate"));

    const FVector2D Icon16x16(16.0f, 16.0f);
    const FVector2D Icon64x64(64.0f, 64.0f);

    Style->Set("ClassIcon.FMODAmbientSound", new IMAGE_BRUSH(TEXT("Icons/AssetIcons/AmbientSound_16x"), Icon16x16));
    Style->Set("ClassThumbnail.FMODAmbientSound", new IMAGE_BRUSH(TEXT("Icons/AssetIcons/AmbientSound_64x"), Icon64x64));
    Style->Set("ClassIcon.FMODAudioComponent", new IMAGE_BRUSH(TEXT("Icons/ActorIcons/SoundActor_16x"), Icon16x16));
    Style->Set("ClassIcon.FMODAsset", new IMAGE_BRUSH(TEXT("Icons/ActorIcons/SoundActor_16x"), Icon16x16));

    return Style;
}

#undef IMAGE_BRUSH

void FFMODStudioStyle::Shutdown()
{
    ensureMsgf(StyleInstance.IsValid(), TEXT("%S called, but StyleInstance wasn't initialized"), __FUNCTION__);
    FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
    ensure(StyleInstance.IsUnique());
    StyleInstance.Reset();
}

//////////////////////////////////////////////////////////////////////////

