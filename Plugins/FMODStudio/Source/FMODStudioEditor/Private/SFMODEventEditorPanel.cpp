// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#include "SFMODEventEditorPanel.h"
#include "FMODStudioModule.h"
#include "FMODUtils.h"
#include "Input/Reply.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Editor/EditorStyle/Public/EditorStyleSet.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SScrollBox.h"
#include "fmod_studio.hpp"

#define LOCTEXT_NAMESPACE "FMODEventEditor"

SFMODEventEditorPanel::~SFMODEventEditorPanel()
{
}

void SFMODEventEditorPanel::Construct(const FArguments &InArgs)
{
    FMODEventEditorPtr = InArgs._FMODEventEditor;

    FMOD::Studio::EventDescription *EventDescription = FMODEventEditorPtr.Pin()->GetEventDescription();

    TSharedRef<SBorder> ToolbarBorder = ConstructToolbar(EventDescription);
    TSharedRef<SExpandableArea> InfoArea = ConstructInfo(EventDescription);
    TSharedRef<SExpandableArea> ParametersArea = ConstructParameters(EventDescription);
    TSharedRef<SExpandableArea> UserPropertiesArea = ConstructUserProperties(EventDescription);

    TSharedRef<SVerticalBox> ChildWidget = SNew(SVerticalBox) + SVerticalBox::Slot().AutoHeight().Padding(0.0f, 3.0f)[InfoArea] +
                                           SVerticalBox::Slot().AutoHeight().Padding(0.0f, 3.0f)[ParametersArea] +
                                           SVerticalBox::Slot().AutoHeight().Padding(0.0f, 3.0f)[UserPropertiesArea];

    ChildSlot[SNew(SVerticalBox) + SVerticalBox::Slot().AutoHeight().Padding(0.0f, 3.0f)[ToolbarBorder] +
              SVerticalBox::Slot().FillHeight(
                  1.0f)[SNew(SScrollBox) +
                        SScrollBox::Slot().Padding(0.0f)[SNew(SVerticalBox) + SVerticalBox::Slot().AutoHeight().Padding(0.0f)[ChildWidget]]]];
}

TSharedRef<SBorder> SFMODEventEditorPanel::ConstructToolbar(FMOD::Studio::EventDescription *EventDescription)
{
    float MinDistance = 0.0f;
    float MaxDistance = 0.0f;
    int32 EventLengthMS = 0;
    bool bIsOneshot = false, bIsStream = false, bIs3D = false;
    if (EventDescription != nullptr)
    {
        EventDescription->getMinMaxDistance(&MinDistance, &MaxDistance);
        EventDescription->getLength(&EventLengthMS);
        EventDescription->isOneshot(&bIsOneshot);
        EventDescription->isStream(&bIsStream);
        EventDescription->is3D(&bIs3D);
    }

    const FTimespan EventLength = FTimespan::FromMilliseconds((double)EventLengthMS);
    const FString EventLengthString =
        EventLength.GetHours() <= 0 ? EventLength.ToString(TEXT("%m:%s.%f")) : EventLength.ToString(TEXT("%h:%m:%s.%f"));

    const FText RadiusText =
        FText::Format(LOCTEXT("RadiusFormat", "Distance Attenuation: {0}m to {1}m"), FText::AsNumber(MinDistance), FText::AsNumber(MaxDistance));
    const FText LengthText = FText::Format(LOCTEXT("LengthFormat", "Length: {0}"), FText::FromString(EventLengthString));

    FText EventInfoText;
    if (bIs3D && bIsOneshot)
    {
        EventInfoText = FText::Format(LOCTEXT("RadiusLengthFormat", "{0}  -  {1}"), RadiusText, LengthText);
    }
    else if (!bIs3D && bIsOneshot)
    {
        EventInfoText = LengthText;
    }
    else if (bIs3D && !bIsOneshot)
    {
        EventInfoText = RadiusText;
    }

    return SNew(SBorder)
        .BorderImage(FAppStyle::Get().GetBrush("ToolPanel.GroupBorder"))
        .Padding(6.0f)
        .Content()[SNew(SHorizontalBox) +
                   SHorizontalBox::Slot()
                       .AutoWidth()
                       .Padding(0.0f, 0.0f, 2.0f, 0.0f)
                       .VAlign(VAlign_Center)
                       .HAlign(HAlign_Left)[SNew(SButton)
                                                .VAlign(VAlign_Center)
                                                .Text(LOCTEXT("Play", "Play"))
                                                .ContentPadding(4)
                                                .OnClicked(this, &SFMODEventEditorPanel::OnClickedPlay)] +
                   SHorizontalBox::Slot()
                       .AutoWidth()
                       .Padding(2.0f, 0.0f)
                       .VAlign(VAlign_Center)
                       .HAlign(HAlign_Left)
                           [SNew(SButton).Text(LOCTEXT("Pause", "Pause")).ContentPadding(4).OnClicked(this, &SFMODEventEditorPanel::OnClickedPause)] +
                   SHorizontalBox::Slot()
                       .AutoWidth()
                       .Padding(2.0f, 0.0f)
                       .VAlign(VAlign_Center)
                       .HAlign(HAlign_Left)[SNew(SButton)
                                                .VAlign(VAlign_Center)
                                                .Text(LOCTEXT("Stop", "Stop"))
                                                .ContentPadding(4)
                                                .OnClicked(this, &SFMODEventEditorPanel::OnClickedStop)] +
                   SHorizontalBox::Slot()
                       .FillWidth(1.0f)
                       .Padding(2.0f, 0.0f)
                       .VAlign(VAlign_Center)
                       .HAlign(HAlign_Right)[SNew(STextBlock).Text(EventInfoText)]];
}

void AddTextField(TSharedRef<SVerticalBox> &InfoBox, const TCHAR *Name, const FText &Value)
{
    InfoBox->AddSlot().Padding(
        4.0f, 3.0f)[SNew(SHorizontalBox) + SHorizontalBox::Slot().FillWidth(0.3f)[SNew(STextBlock).Text(FText::FromString(Name))] +
                    SHorizontalBox::Slot()[SNew(SEditableText).Text(Value).IsReadOnly(true)]];
}

void AddBoolField(TSharedRef<SVerticalBox> &InfoBox, const TCHAR *Name, bool bValue)
{
    AddTextField(InfoBox, Name, bValue ? LOCTEXT("True", "True") : LOCTEXT("False", "False"));
}

void AddFloatField(TSharedRef<SVerticalBox> &InfoBox, const TCHAR *Name, float Value)
{
    AddTextField(InfoBox, Name, FText::AsNumber(Value));
}

TSharedRef<SExpandableArea> MakeBox(TSharedRef<SVerticalBox> &InfoBox, const FText &Value)
{
    return SNew(SExpandableArea)
        .AreaTitle(Value)
        .InitiallyCollapsed(false)
        .BodyContent()[SNew(SBorder).BorderImage(FCoreStyle::Get().GetBrush("NoBorder")).Padding(4.0f).Content()[InfoBox]];
}

TSharedRef<SExpandableArea> SFMODEventEditorPanel::ConstructInfo(FMOD::Studio::EventDescription *EventDescription)
{
    TSharedRef<SVerticalBox> InfoBox = SNew(SVerticalBox);

    if (EventDescription != nullptr)
    {
        FString EventPath = FMODUtils::GetPath(EventDescription);
        FGuid Guid = FMODUtils::GetID(EventDescription);

        int Length = 0.0f;
        float MinDist = 0.0f;
        float MaxDist = 0.0f;
        EventDescription->getLength(&Length);
        EventDescription->getMinMaxDistance(&MinDist, &MaxDist);

        bool bOneShot = false;
        bool bStream = false;
        bool b3D = false;
        EventDescription->isOneshot(&bOneShot);
        EventDescription->isStream(&bStream);
        EventDescription->is3D(&b3D);

        AddTextField(InfoBox, TEXT("Path"), FText::FromString(EventPath));
        AddTextField(InfoBox, TEXT("Guid"), FText::FromString(Guid.ToString(EGuidFormats::DigitsWithHyphensInBraces)));
        AddBoolField(InfoBox, TEXT("OneShot"), bOneShot);
        AddBoolField(InfoBox, TEXT("Streaming"), bStream);
        AddBoolField(InfoBox, TEXT("3D"), b3D);

        AddFloatField(InfoBox, TEXT("Length"), static_cast<float>(Length));
        if (b3D)
        {
            AddFloatField(InfoBox, TEXT("Min Dist"), MinDist);
            AddFloatField(InfoBox, TEXT("Max Dist"), MaxDist);
        }
    }

    return MakeBox(InfoBox, LOCTEXT("EventInfo", "Event Info"));
}

TSharedRef<SExpandableArea> SFMODEventEditorPanel::ConstructParameters(FMOD::Studio::EventDescription *EventDescription)
{
    auto EventEditor = FMODEventEditorPtr.Pin();
    TSharedRef<SVerticalBox> ParametersBox = SNew(SVerticalBox);

    FNumberFormattingOptions Options;
    Options.MinimumFractionalDigits = 1;

    if (EventDescription != nullptr)
    {
        int32 ParameterCount;
        EventDescription->getParameterDescriptionCount(&ParameterCount);
        for (int32 ParamIdx = 0; ParamIdx < ParameterCount; ParamIdx++)
        {
            FMOD_STUDIO_PARAMETER_DESCRIPTION Parameter;
            EventDescription->getParameterDescriptionByIndex(ParamIdx, &Parameter);

            EventEditor->AddParameter(Parameter.id, Parameter.minimum);

            const FString ParameterName = Parameter.type == FMOD_STUDIO_PARAMETER_GAME_CONTROLLED ? FString(UTF8_TO_TCHAR(Parameter.name)) :
                                                                                                    FMODUtils::ParameterTypeToString(Parameter.type);
            const FText ToolTipText = FText::Format(LOCTEXT("ParameterTooltipFormat", "{0} (Min Value: {1} - Max Value: {2})"),
                FText::FromString(ParameterName), FText::AsNumber(Parameter.minimum, &Options), FText::AsNumber(Parameter.maximum, &Options));

            ParametersBox->AddSlot().Padding(4.0f,
                2.0f)[SNew(SHorizontalBox).ToolTipText(ToolTipText) +
                      SHorizontalBox::Slot().FillWidth(0.3f)[SNew(STextBlock).Text(FText::FromString(ParameterName))] +
                      SHorizontalBox::Slot().MaxWidth(200.0f)[SNew(SNumericEntryBox<float>)
                                                                  .Value(this, &SFMODEventEditorPanel::GetParameterValue, Parameter.id)
                                                                  .OnValueChanged(this, &SFMODEventEditorPanel::OnParameterValueChanged, Parameter.id)
                                                                  .AllowSpin(true)
                                                                  .MinValue(Parameter.minimum)
                                                                  .MaxValue(Parameter.maximum)
                                                                  .MinSliderValue(Parameter.minimum)
                                                                  .MaxSliderValue(Parameter.maximum)
                                                                  .Delta(0.01f)]];
        }
    }

    return MakeBox(ParametersBox, LOCTEXT("EventParameters", "Event Parameters"));
}

TSharedRef<SExpandableArea> SFMODEventEditorPanel::ConstructUserProperties(FMOD::Studio::EventDescription *EventDescription)
{
    TSharedRef<SVerticalBox> UserPropertiesBox = SNew(SVerticalBox);

    if (EventDescription != nullptr)
    {
        int32 UserPropertyCount;
        EventDescription->getUserPropertyCount(&UserPropertyCount);
        for (int32 PropertyIdx = 0; PropertyIdx < UserPropertyCount; PropertyIdx++)
        {
            FMOD_STUDIO_USER_PROPERTY UserProperty;
            EventDescription->getUserPropertyByIndex(PropertyIdx, &UserProperty);

            FText PropertyText;
            switch (UserProperty.type)
            {
                case FMOD_STUDIO_USER_PROPERTY_TYPE_INTEGER:
                    PropertyText = FText::AsNumber(UserProperty.intvalue);
                    break;
                case FMOD_STUDIO_USER_PROPERTY_TYPE_BOOLEAN:
                    PropertyText = UserProperty.boolvalue ? LOCTEXT("True", "True") : LOCTEXT("False", "False");
                    break;
                case FMOD_STUDIO_USER_PROPERTY_TYPE_FLOAT:
                    PropertyText = FText::AsNumber(UserProperty.floatvalue);
                    break;
                case FMOD_STUDIO_USER_PROPERTY_TYPE_STRING:
                    PropertyText = FText::FromString(UTF8_TO_TCHAR(UserProperty.stringvalue));
                    break;
            }

            FString UserName(UTF8_TO_TCHAR(UserProperty.name));
            AddTextField(UserPropertiesBox, *UserName, PropertyText);
        }
    }

    return MakeBox(UserPropertiesBox, LOCTEXT("EventUserProperties", "Event User Properties"));
}

FReply SFMODEventEditorPanel::OnClickedPlay()
{
    FMODEventEditorPtr.Pin()->PlayEvent();
    return FReply::Handled();
}

FReply SFMODEventEditorPanel::OnClickedStop()
{
    FMODEventEditorPtr.Pin()->StopEvent();
    return FReply::Handled();
}

FReply SFMODEventEditorPanel::OnClickedPause()
{
    FMODEventEditorPtr.Pin()->PauseEvent();
    return FReply::Handled();
}

void SFMODEventEditorPanel::OnParameterValueChanged(float NewValue, FMOD_STUDIO_PARAMETER_ID ParameterId)
{
    FMODEventEditorPtr.Pin()->SetParameterValue(ParameterId, NewValue);
}

TOptional<float> SFMODEventEditorPanel::GetParameterValue(FMOD_STUDIO_PARAMETER_ID ParameterId) const
{
    return FMODEventEditorPtr.Pin()->GetParameterValue(ParameterId);
}

#undef LOC_NAMESPACE
