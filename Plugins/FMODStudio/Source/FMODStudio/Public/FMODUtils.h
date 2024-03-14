// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#pragma once

#include "fmod_studio.hpp"
#include "fmod.hpp"

#include "Runtime/Launch/Resources/Version.h"
#include "Engine/Engine.h"

#include "FMODStudioModule.h"

#define verifyfmod(fn)                         \
    {                                          \
        FMOD_RESULT _result = (fn);            \
        if (_result != FMOD_OK)                \
        {                                      \
            FMODUtils::LogError(_result, #fn); \
        }                                      \
    }

namespace FMODUtils
{

// Unreal defines 1 unit == 1cm, so convert to metres for Studio automatically
#define FMOD_VECTOR_SCALE_DEFAULT 0.01f

// Just call into module
inline void LogError(FMOD_RESULT result, const char *function)
{
    IFMODStudioModule::Get().LogError(result, function);
}

inline void Assign(FMOD_VECTOR &Dest, const FVector &Src)
{
    Dest.x = Src.X;
    Dest.y = Src.Y;
    Dest.z = Src.Z;
}

inline FMOD_VECTOR ConvertWorldVector(const FVector &Src)
{
    static FMatrix UE4toFMOD(FVector(0.0f, 0.0f, FMOD_VECTOR_SCALE_DEFAULT), FVector(FMOD_VECTOR_SCALE_DEFAULT, 0.0f, 0.0f),
        FVector(0.0f, FMOD_VECTOR_SCALE_DEFAULT, 0.0f), FVector::ZeroVector);

    FMOD_VECTOR Dest;
    Assign(Dest, UE4toFMOD.TransformPosition(Src));
    return Dest;
}

inline FMOD_VECTOR ConvertUnitVector(const FVector &Src)
{
    static FMatrix UE4toFMOD(FVector(0.0f, 0.0f, 1.0f), FVector(1.0f, 0.0f, 0.0f), FVector(0.0f, 1.0f, 0.0f), FVector::ZeroVector);

    FMOD_VECTOR Dest;
    Assign(Dest, UE4toFMOD.TransformVector(Src));
    return Dest;
}

inline void Assign(FMOD_3D_ATTRIBUTES &Dest, const FTransform &Src)
{
    Dest.position = ConvertWorldVector(Src.GetTranslation());
    Dest.forward = ConvertUnitVector(Src.GetUnitAxis(EAxis::X));
    Dest.up = ConvertUnitVector(Src.GetUnitAxis(EAxis::Z));
}

inline float DistanceToUEScale(float FMODDistance)
{
    return FMODDistance / FMOD_VECTOR_SCALE_DEFAULT;
}

inline bool IsWorldAudible(UWorld *World, bool AllowInEditor)
{
    if (GEngine && IFMODStudioModule::Get().UseSound())
    {
        if (!IsValid(World))
        {
            return true;
        }

        if (World->bAllowAudioPlayback && World->GetNetMode() != NM_DedicatedServer)
        {
            EWorldType::Type previewEnum;
            previewEnum = EWorldType::EditorPreview;
            if (World->IsGameWorld() || World->WorldType == previewEnum || (AllowInEditor && World->WorldType == EWorldType::Editor))
            {
                return true;
            }
        }
    }
    return false;
}

inline FMOD::Studio::ID ConvertGuid(const FGuid &UnrealGuid)
{
    // Unreal doesn't follow the usual windows GUID format, instead it parses
    // them as 4 integers
    FMOD::Studio::ID StudioGuid;
    FMemory::Memcpy(&StudioGuid, &UnrealGuid, sizeof(StudioGuid));
    Swap(StudioGuid.Data2, StudioGuid.Data3);
    Swap(StudioGuid.Data4[0], StudioGuid.Data4[3]);
    Swap(StudioGuid.Data4[1], StudioGuid.Data4[2]);
    Swap(StudioGuid.Data4[4], StudioGuid.Data4[7]);
    Swap(StudioGuid.Data4[5], StudioGuid.Data4[6]);
    return StudioGuid;
}

inline FGuid ConvertGuid(const FMOD::Studio::ID &StudioGuid)
{
    // Unreal doesn't follow the usual windows GUID format, instead it parses
    // them as 4 integers
    FMOD::Studio::ID CopiedGuid;
    FMemory::Memcpy(&CopiedGuid, &StudioGuid, sizeof(StudioGuid));
    Swap(CopiedGuid.Data2, CopiedGuid.Data3);
    Swap(CopiedGuid.Data4[0], CopiedGuid.Data4[3]);
    Swap(CopiedGuid.Data4[1], CopiedGuid.Data4[2]);
    Swap(CopiedGuid.Data4[4], CopiedGuid.Data4[7]);
    Swap(CopiedGuid.Data4[5], CopiedGuid.Data4[6]);
    FGuid UnrealGuid;
    FMemory::Memcpy(&UnrealGuid, &CopiedGuid, sizeof(CopiedGuid));
    return UnrealGuid;
}

template <class StudioType>
inline FGuid GetID(StudioType *Instance)
{
    FMOD::Studio::ID StudioID = { 0 };
    verifyfmod(Instance->getID(&StudioID));
    return FMODUtils::ConvertGuid(StudioID);
}

template <class StudioType>
inline FString GetPath(StudioType *Instance)
{
    int ActualSize = 128; // Start with expected enough space
    TArray<char> RawBuffer;
    FMOD_RESULT Result;
    do
    {
        RawBuffer.SetNum(ActualSize);
        Result = Instance->getPath(RawBuffer.GetData(), ActualSize, &ActualSize);
    } while (Result == FMOD_ERR_TRUNCATED);

    if (Result == FMOD_OK)
    {
        return FString(UTF8_TO_TCHAR(RawBuffer.GetData()));
    }
    else
    {
        return FString();
    }
}

inline FString LookupNameFromGuid(FMOD::Studio::System *StudioSystem, const FMOD::Studio::ID &Guid)
{
    int ActualSize = 128; // Start with expected enough space
    TArray<char> RawBuffer;
    FMOD_RESULT Result;
    do
    {
        RawBuffer.SetNum(ActualSize);
        Result = StudioSystem->lookupPath(&Guid, RawBuffer.GetData(), ActualSize, &ActualSize);
    } while (Result == FMOD_ERR_TRUNCATED);

    if (Result == FMOD_OK)
    {
        return FString(UTF8_TO_TCHAR(RawBuffer.GetData()));
    }
    else
    {
        return FString();
    }
}

inline FString LookupNameFromGuid(FMOD::Studio::System *StudioSystem, const FGuid &Guid)
{
    return LookupNameFromGuid(StudioSystem, ConvertGuid(Guid));
}

inline FString ParameterTypeToString(FMOD_STUDIO_PARAMETER_TYPE Type)
{
    switch (Type)
    {
        case FMOD_STUDIO_PARAMETER_GAME_CONTROLLED:
            return FString("Game Controlled");
        case FMOD_STUDIO_PARAMETER_AUTOMATIC_DISTANCE:
            return FString("Distance (Auto)");
        case FMOD_STUDIO_PARAMETER_AUTOMATIC_EVENT_CONE_ANGLE:
            return FString("Event Cone Angle (Auto)");
        case FMOD_STUDIO_PARAMETER_AUTOMATIC_EVENT_ORIENTATION:
            return FString("Event Orientation (Auto)");
        case FMOD_STUDIO_PARAMETER_AUTOMATIC_DIRECTION:
            return FString("Direction (Auto)");
        case FMOD_STUDIO_PARAMETER_AUTOMATIC_ELEVATION:
            return FString("Elevation (Auto)");
        case FMOD_STUDIO_PARAMETER_AUTOMATIC_LISTENER_ORIENTATION:
            return FString("Listener Orientation (Auto)");
    }

    return FString();
}
}