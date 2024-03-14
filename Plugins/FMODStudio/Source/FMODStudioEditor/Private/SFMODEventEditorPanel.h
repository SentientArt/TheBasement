// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#pragma once

#include "FMODEventEditor.h"

namespace FMOD
{
namespace Studio
{
class EventDescription;
}
}

class SFMODEventEditorPanel : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SFMODEventEditorPanel) {}
    SLATE_ARGUMENT(TWeakPtr<FFMODEventEditor>, FMODEventEditor)
    SLATE_END_ARGS()

    ~SFMODEventEditorPanel();

    /** SCompoundWidget interface */
    void Construct(const FArguments &InArgs);

private:
    TSharedRef<SBorder> ConstructToolbar(FMOD::Studio::EventDescription *EventDescription);
    TSharedRef<SExpandableArea> ConstructInfo(FMOD::Studio::EventDescription *EventDescription);
    TSharedRef<SExpandableArea> ConstructParameters(FMOD::Studio::EventDescription *EventDescription);
    TSharedRef<SExpandableArea> ConstructUserProperties(FMOD::Studio::EventDescription *EventDescription);

    /** Editor that owns this panel */
    TWeakPtr<FFMODEventEditor> FMODEventEditorPtr;

    FReply OnClickedPlay();
    FReply OnClickedStop();
    FReply OnClickedPause();

    TOptional<float> GetParameterValue(FMOD_STUDIO_PARAMETER_ID ParameterId) const;
    void OnParameterValueChanged(float NewValue, FMOD_STUDIO_PARAMETER_ID ParameterId);
};
