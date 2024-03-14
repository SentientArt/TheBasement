// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#pragma once

#include "GenericPlatform/GenericPlatform.h"
#include "UObject/Object.h"
#include "Math/Vector.h"

struct FInteriorSettings;

/** Struct encapsulating settings for interior areas. */
struct FFMODInteriorSettings
{
    uint32 bIsWorldSettings : 1;
    float ExteriorVolume;
    float ExteriorTime;
    float ExteriorLPF;
    float ExteriorLPFTime;
    float InteriorVolume;
    float InteriorTime;
    float InteriorLPF;
    float InteriorLPFTime;

    FFMODInteriorSettings();
    bool operator==(const FInteriorSettings &Other) const;
    bool operator!=(const FInteriorSettings &Other) const;
    FFMODInteriorSettings &operator=(FInteriorSettings Other);
};

/** A direct copy of FListener (which doesn't have external linkage, unfortunately) **/
struct FFMODListener
{
    FTransform Transform;
    FVector Velocity;

    struct FFMODInteriorSettings InteriorSettings;
    /** The volume the listener resides in */
    class AAudioVolume *Volume;

    /** The times of interior volumes fading in and out */
    double InteriorStartTime;
    double InteriorEndTime;
    double ExteriorEndTime;
    double InteriorLPFEndTime;
    double ExteriorLPFEndTime;
    float InteriorVolumeInterp;
    float InteriorLPFInterp;
    float ExteriorVolumeInterp;
    float ExteriorLPFInterp;

    FVector GetUp() const { return Transform.GetUnitAxis(EAxis::Z); }
    FVector GetFront() const { return Transform.GetUnitAxis(EAxis::Y); }
    FVector GetRight() const { return Transform.GetUnitAxis(EAxis::X); }

    /**
	 * Works out the interp value between source and end
	 */
    float Interpolate(const double EndTime);

    /**
	 * Gets the current state of the interior settings for the listener
	 */
    void UpdateCurrentInteriorSettings();

    /** 
	 * Apply the interior settings to ambient sounds
	 */
    void ApplyInteriorSettings(class AAudioVolume *Volume, const FInteriorSettings &Settings);

    FFMODListener()
        : Transform(FTransform::Identity)
        , Velocity(ForceInit)
        , Volume(NULL)
        , InteriorStartTime(0.0)
        , InteriorEndTime(0.0)
        , ExteriorEndTime(0.0)
        , InteriorLPFEndTime(0.0)
        , ExteriorLPFEndTime(0.0)
        , InteriorVolumeInterp(0.f)
        , InteriorLPFInterp(0.f)
        , ExteriorVolumeInterp(0.f)
        , ExteriorLPFInterp(0.f)
    {
    }
};
