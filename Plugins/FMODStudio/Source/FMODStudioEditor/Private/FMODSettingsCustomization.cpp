// Copyright (c), Firelight Technologies Pty, Ltd. 2023.

#include "FMODSettingsCustomization.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "FMODSettings.h"
#include "IDetailPropertyRow.h"
#include "Settings/ProjectPackagingSettings.h"
#include "Styling/SlateColor.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"

#define LOCTEXT_NAMESPACE "FMODSettings"

class SSettingsMessage : public SCompoundWidget
{
    SLATE_BEGIN_ARGS(SSettingsMessage)
    {}

    // Called when the Setup button is clicked
    SLATE_EVENT(FSimpleDelegate, OnSetupClicked)

    SLATE_END_ARGS()

public:
    void Construct(const FArguments& InArgs)
    {
        TSharedRef<SWidget> SettingsOkayWidget = MakeRow(
            "SettingsEditor.GoodIcon",
            LOCTEXT("SettingsOkayText", "FMOD Settings are valid, run the Validate FMOD command to perform additional checking."),
            FText()
        );

        TSharedRef<SWidget> NoContentDirWidget = MakeRow(
            "SettingsEditor.WarningIcon",
            LOCTEXT("NoContentDirText", "Bank Output Directory directory has not been set."),
            FText()
        );

        TSharedRef<SWidget> PackagingSettingsBadWidget = MakeRow(
            "SettingsEditor.WarningIcon",
            LOCTEXT("PackagingSettingsBadText",
                "The packaging settings for copying the FMOD bank files to staging are not correct. It is recommended that:\n"
                " - The bank output directory for the Desktop platform (or the forced platform if set) is added to the \"Additional Non-Asset Directories To Copy\" list.\n"
                " - That no other directory containing FMOD banks or assets is added to either the \"Additional Non-Asset Directories To Copy\" list "
                "or the \"Additional Non-Asset Directories to Package\" list.\n"
                " - The Generated Assets are added to the \"Additional Asset Directories to Cook\" list."
            ),
            LOCTEXT("FixPackagingSettings", "Fix")
        );

        ChildSlot
            [
                SNew(SBorder)
                .BorderBackgroundColor(this, &SSettingsMessage::GetBorderColor)
                .BorderImage(FAppStyle::GetBrush("ToolPanel.LightGroupBorder"))
                .Padding(8.0f)
                [
                    SNew(SWidgetSwitcher)
                    .WidgetIndex(this, &SSettingsMessage::GetSetupStateAsInt)

                    + SWidgetSwitcher::Slot()
                    [
                        SettingsOkayWidget
                    ]

                    + SWidgetSwitcher::Slot()
                    [
                        NoContentDirWidget
                    ]

                    + SWidgetSwitcher::Slot()
                    [
                        PackagingSettingsBadWidget
                    ]
                ]
            ];

        UpdateState();
    }

    void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
    {
        UpdateState();
    }

private:
    FSlateColor GetBorderColor() const
    {
        if (SettingsState == UFMODSettings::Okay)
        {
            return FLinearColor::Green;
        }
        else
        {
            return FLinearColor(0.8f, 0, 0);
        }
    }

    TSharedRef<SWidget> MakeRow(FName IconName, FText Message, FText ButtonMessage)
    {
        TSharedRef<SHorizontalBox> Result = SNew(SHorizontalBox)

            // Status icon
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)[SNew(SImage).Image(FAppStyle::GetBrush(IconName))]

            // Notice
            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            .Padding(16.0f, 0.0f)
            .VAlign(VAlign_Center)[
                SNew(STextBlock)
                .ColorAndOpacity(FLinearColor::White)
                .ShadowColorAndOpacity(FLinearColor::Black)
                .ShadowOffset(FVector2D::UnitVector)
                .AutoWrapText(true)
                .Text(Message)
            ];

        if (!ButtonMessage.IsEmpty())
        {
            Result->AddSlot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                [
                    SNew(SButton)
                    .OnClicked(this, &SSettingsMessage::OnButtonPressed)
                    .Text(ButtonMessage)
                ];
        }

        return Result;
    }

    FReply OnButtonPressed()
    {
        const UFMODSettings& Settings = *GetDefault<UFMODSettings>();
        UProjectPackagingSettings* PackagingSettings = Cast<UProjectPackagingSettings>(UProjectPackagingSettings::StaticClass()->GetDefaultObject());

        if (SettingsState == UFMODSettings::PackagingSettingsBad)
        {
            // Remove any bad entries
            for (int i = 0; i < PackagingSettings->DirectoriesToAlwaysStageAsNonUFS.Num();)
            {
                if (PackagingSettings->DirectoriesToAlwaysStageAsNonUFS[i].Path.StartsWith(Settings.BankOutputDirectory.Path))
                {
                    PackagingSettings->DirectoriesToAlwaysStageAsNonUFS.RemoveAt(i);
                }
                else
                {
                    ++i;
                }
            }

            for (int i = 0; i < PackagingSettings->DirectoriesToAlwaysStageAsUFS.Num();)
            {
                if (PackagingSettings->DirectoriesToAlwaysStageAsUFS[i].Path.StartsWith(Settings.BankOutputDirectory.Path))
                {
                    PackagingSettings->DirectoriesToAlwaysStageAsUFS.RemoveAt(i);
                }
                else
                {
                    ++i;
                }
            }

            for (int i = 0; i < PackagingSettings->DirectoriesToAlwaysCook.Num();)
            {
                if (PackagingSettings->DirectoriesToAlwaysCook[i].Path.StartsWith(Settings.GetFullContentPath()))
                {
                    PackagingSettings->DirectoriesToAlwaysCook.RemoveAt(i);
                }
                else
                {
                    ++i;
                }
            }

            // Add correct entry
            FDirectoryPath BankPath;
            BankPath.Path = Settings.GetDesktopBankPath();
            PackagingSettings->DirectoriesToAlwaysStageAsNonUFS.Add(BankPath);
            FDirectoryPath generatedFolder;
            for (FString folder : Settings.GeneratedFolders)
            {
                generatedFolder.Path = Settings.GetFullContentPath() / folder;
                PackagingSettings->DirectoriesToAlwaysCook.Add(generatedFolder);
            }

            PackagingSettings->TryUpdateDefaultConfigFile();
        }

        UpdateState();
        return FReply::Handled();
    }

    int32 GetSetupStateAsInt() const
    {
        return (int32)SettingsState;
    }

    void UpdateState()
    {
        const UFMODSettings& Settings = *GetDefault<UFMODSettings>();
        SettingsState = Settings.Check();
    }

private:
    UFMODSettings::EProblem SettingsState;
};

TSharedRef<IDetailCustomization> FFMODSettingsCustomization::MakeInstance()
{
    return MakeShareable(new FFMODSettingsCustomization);
}

FFMODSettingsCustomization::FFMODSettingsCustomization()
{
}

void FFMODSettingsCustomization::CustomizeDetails(IDetailLayoutBuilder &DetailLayout)
{
    IDetailCategoryBuilder &PackagingCategory = DetailLayout.EditCategory(TEXT("Notice"), FText::GetEmpty(), ECategoryPriority::Important);
    TSharedRef<SSettingsMessage> PlatformSetupMessage = SNew(SSettingsMessage);
    PackagingCategory.AddCustomRow(LOCTEXT("Warning", "Warning"), false).WholeRowWidget[PlatformSetupMessage];
}

#undef LOCTEXT_NAMESPACE
