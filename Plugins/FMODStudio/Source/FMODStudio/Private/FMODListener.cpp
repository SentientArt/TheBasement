// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#include "FMODListener.h"
#include "Misc/App.h"
#include "AudioDefines.h"
#include "Sound/AudioVolume.h"

float FFMODListener::Interpolate(const double EndTime)
{
    if (FApp::GetCurrentTime() < InteriorStartTime)
    {
        return (0.0f);
    }

    if (FApp::GetCurrentTime() >= EndTime)
    {
        return (1.0f);
    }

    float InterpValue = (float)((FApp::GetCurrentTime() - InteriorStartTime) / (EndTime - InteriorStartTime));
    return (InterpValue);
}

void FFMODListener::UpdateCurrentInteriorSettings()
{
    // Store the interpolation value, not the actual value
    InteriorVolumeInterp = Interpolate(InteriorEndTime);
    ExteriorVolumeInterp = Interpolate(ExteriorEndTime);
    InteriorLPFInterp = Interpolate(InteriorLPFEndTime);
    ExteriorLPFInterp = Interpolate(ExteriorLPFEndTime);
}

void FFMODListener::ApplyInteriorSettings(class AAudioVolume *InVolume, const FInteriorSettings &Settings)
{
    if (InteriorSettings != Settings)
    {
        // Use previous/ current interpolation time if we're transitioning to the default worldsettings zone.
        InteriorStartTime = FApp::GetCurrentTime();
        InteriorEndTime = InteriorStartTime + (Settings.bIsWorldSettings ? InteriorSettings.InteriorTime : Settings.InteriorTime);
        ExteriorEndTime = InteriorStartTime + (Settings.bIsWorldSettings ? InteriorSettings.ExteriorTime : Settings.ExteriorTime);
        InteriorLPFEndTime = InteriorStartTime + (Settings.bIsWorldSettings ? InteriorSettings.InteriorLPFTime : Settings.InteriorLPFTime);
        ExteriorLPFEndTime = InteriorStartTime + (Settings.bIsWorldSettings ? InteriorSettings.ExteriorLPFTime : Settings.ExteriorLPFTime);

        InteriorSettings = Settings;
    }
    Volume = InVolume;
}

FFMODInteriorSettings::FFMODInteriorSettings()
    : bIsWorldSettings(false)
    , ExteriorVolume(1.0f)
    , ExteriorTime(0.5f)
    , ExteriorLPF(MAX_FILTER_FREQUENCY)
    , ExteriorLPFTime(0.5f)
    , InteriorVolume(1.0f)
    , InteriorTime(0.5f)
    , InteriorLPF(MAX_FILTER_FREQUENCY)
    , InteriorLPFTime(0.5f)
{
}

bool FFMODInteriorSettings::operator==(const FInteriorSettings &Other) const
{
    return (this->bIsWorldSettings == Other.bIsWorldSettings) && (this->ExteriorVolume == Other.ExteriorVolume) &&
           (this->ExteriorTime == Other.ExteriorTime) && (this->ExteriorLPF == Other.ExteriorLPF) &&
           (this->ExteriorLPFTime == Other.ExteriorLPFTime) && (this->InteriorVolume == Other.InteriorVolume) &&
           (this->InteriorTime == Other.InteriorTime) && (this->InteriorLPF == Other.InteriorLPF) && (this->InteriorLPFTime == Other.InteriorLPFTime);
}
bool FFMODInteriorSettings::operator!=(const FInteriorSettings &Other) const
{
    return !(*this == Other);
}

FFMODInteriorSettings &FFMODInteriorSettings::operator=(FInteriorSettings Other)
{
    bIsWorldSettings = Other.bIsWorldSettings;
    ExteriorVolume = Other.ExteriorVolume;
    ExteriorTime = Other.ExteriorTime;
    ExteriorLPF = Other.ExteriorLPF;
    ExteriorLPFTime = Other.ExteriorLPFTime;
    InteriorVolume = Other.InteriorVolume;
    InteriorTime = Other.InteriorTime;
    InteriorLPF = Other.InteriorLPF;
    InteriorLPFTime = Other.InteriorLPFTime;
    return *this;
}