// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#include "FMODAudioComponentDetails.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "FMODAmbientSound.h"
#include "FMODStudioModule.h"
#include "FMODEvent.h"
#include "fmod_studio.hpp"
#include "Editor.h"
#include "Widgets/Input/SButton.h"
#include "Editor/PropertyEditor/Public/DetailLayoutBuilder.h"
#include "Editor/PropertyEditor/Public/DetailCategoryBuilder.h"

#define LOCTEXT_NAMESPACE "FMODStudio"

TSharedRef<IDetailCustomization> FFMODAudioComponentDetails::MakeInstance()
{
    return MakeShareable(new FFMODAudioComponentDetails);
}

void FFMODAudioComponentDetails::CustomizeDetails(IDetailLayoutBuilder &DetailBuilder)
{
    const TArray<TWeakObjectPtr<UObject>> &SelectedObjects = DetailBuilder.GetSelectedObjects();

    for (int32 ObjectIndex = 0; !AudioComponent.IsValid() && ObjectIndex < SelectedObjects.Num(); ++ObjectIndex)
    {
        const TWeakObjectPtr<UObject> &CurrentObject = SelectedObjects[ObjectIndex];
        if (CurrentObject.Get()->GetClass()->IsChildOf(UFMODAudioComponent::StaticClass()))
        {
            AudioComponent = Cast<UFMODAudioComponent>(CurrentObject.Get());
        }
        else
        {
            AudioComponent = Cast<AActor>(CurrentObject.Get())->FindComponentByClass<UFMODAudioComponent>();
        }
    }

    DetailBuilder.EditCategory(TEXT("FMODAudio"))
        .AddCustomRow(
            FText::GetEmpty())[SNew(SVerticalBox) +
                               SVerticalBox::Slot()
                                   .Padding(0, 2.0f, 0, 0)
                                   .FillHeight(1.0f)
                                   .VAlign(
                                       VAlign_Center)[SNew(SHorizontalBox) +
                                                      SHorizontalBox::Slot()
                                                          .AutoWidth()
                                                          .Padding(2.0f, 0.0f)
                                                          .VAlign(VAlign_Center)
                                                          .HAlign(HAlign_Left)[SNew(SButton)
                                                                                   .VAlign(VAlign_Center)
                                                                                   .OnClicked(this, &FFMODAudioComponentDetails::OnEditSoundClicked)
                                                                                   .Text(LOCTEXT("View Details", "Details"))] +
                                                      SHorizontalBox::Slot()
                                                          .AutoWidth()
                                                          .Padding(2.0f, 0.0f)
                                                          .VAlign(VAlign_Center)
                                                          .HAlign(HAlign_Left)[SNew(SButton)
                                                                                   .VAlign(VAlign_Center)
                                                                                   .OnClicked(this, &FFMODAudioComponentDetails::OnPlaySoundClicked)
                                                                                   .Text(LOCTEXT("Play FMOD Event", "Play"))] +
                                                      SHorizontalBox::Slot()
                                                          .AutoWidth()
                                                          .Padding(2.0f, 0.0f)
                                                          .VAlign(VAlign_Center)
                                                          .HAlign(HAlign_Left)[SNew(SButton)
                                                                                   .VAlign(VAlign_Center)
                                                                                   .OnClicked(this, &FFMODAudioComponentDetails::OnStopSoundClicked)
                                                                                   .Text(LOCTEXT("Stop FMOD Event", "Stop"))]]];
}

FReply FFMODAudioComponentDetails::OnEditSoundClicked()
{
    if (AudioComponent.IsValid())
    {
        UFMODEvent *Event = AudioComponent.Get()->Event;
        if (Event)
        {
            GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(Event);
        }
    }

    return FReply::Handled();
}

FReply FFMODAudioComponentDetails::OnPlaySoundClicked()
{
    if (AudioComponent.IsValid())
    {
        UFMODEvent *Event = AudioComponent.Get()->Event;
        if (IsValid(Event))
        {
            FMOD::Studio::EventInstance *Instance = IFMODStudioModule::Get().CreateAuditioningInstance(Event);
            if (Instance)
            {
                for (auto param : AudioComponent->ParameterCache)
                {
                    Instance->setParameterByName(TCHAR_TO_UTF8(*param.Key.ToString()), param.Value);
                }
                Instance->start();
            }
        }
    }

    return FReply::Handled();
}

FReply FFMODAudioComponentDetails::OnStopSoundClicked()
{
    IFMODStudioModule::Get().StopAuditioningInstance();

    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
