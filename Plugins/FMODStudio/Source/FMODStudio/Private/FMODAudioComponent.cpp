// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#include "FMODAudioComponent.h"
#include "FMODStudioModule.h"
#include "FMODUtils.h"
#include "FMODEvent.h"
#include "FMODListener.h"
#include "FMODSettings.h"
#include "fmod_studio.hpp"
#include "Misc/App.h"
#include "Misc/Paths.h"
#include "Misc/ScopeLock.h"
#include "FMODStudioPrivatePCH.h"
#include "Components/BillboardComponent.h"
#if WITH_EDITORONLY_DATA
#include "Engine/Texture2D.h"
#endif

UFMODAudioComponent::UFMODAudioComponent(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
    , Event(nullptr)
    , bEnableTimelineCallbacks(false) // Default OFF for efficiency
    , bAutoDestroy(false)
    , bStopWhenOwnerDestroyed(true)
    , bApplyAmbientVolumes(false)
    , bApplyOcclusionParameter(false)
    , StudioInstance(nullptr)
    , bDefaultParameterValuesCached(false)
    , Module(nullptr)
    , InteriorLastUpdateTime(0.0f)
    , SourceInteriorVolume(0.0f)
    , SourceInteriorLPF(0.0f)
    , CurrentInteriorVolume(0.0f)
    , CurrentInteriorLPF(0.0f)
    , AmbientVolume(0.0f)
    , AmbientLPF(0.0f)
    , LastVolume(1.0f)
    , LastLPF(MAX_FILTER_FREQUENCY)
    , wasOccluded(false)
    , OcclusionID()
    , AmbientVolumeID()
    , AmbientLPFID()
    , ProgrammerSound(nullptr)
    , NeedDestroyProgrammerSoundCallback(false)
    , EventLength(0)
    , bPlayEnded(false)
{
    bAutoActivate = true;
    bNeverNeedsRenderUpdate = true;
    bWantsOnUpdateTransform = true;

#if WITH_EDITORONLY_DATA
    bVisualizeComponent = true;
#endif

    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;
    PrimaryComponentTick.bStartWithTickEnabled = false;

    for (int i = 0; i < EFMODEventProperty::Count; ++i)
    {
        StoredProperties[i] = -1.0f;
    }

    NeedDestroyProgrammerSoundCallback = false;
}

FString UFMODAudioComponent::GetDetailedInfoInternal(void) const
{
    FString Result;

    if (Event)
    {
        Result = Event->GetPathName(NULL);
    }
    else
    {
        Result = TEXT("No_Event");
    }

    return Result;
}

void UFMODAudioComponent::OnRegister()
{
    Super::OnRegister();

#if WITH_EDITORONLY_DATA
    if (!bDefaultParameterValuesCached)
    {
        CacheDefaultParameterValues();
    }

    UpdateSpriteTexture();
#endif

    if (IsActive() && bAutoActivate)
    {
        FMOD_STUDIO_PLAYBACK_STATE state = FMOD_STUDIO_PLAYBACK_STOPPED;
        if (StudioInstance->isValid())
        {
            StudioInstance->getPlaybackState(&state);
        }
        if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
        {
            Play();
        }
    }
}

#if WITH_EDITORONLY_DATA
void UFMODAudioComponent::UpdateSpriteTexture()
{
    if (SpriteComponent)
    {
        if (bAutoActivate)
        {
            SpriteComponent->SetSprite(
                LoadObject<UTexture2D>(NULL, TEXT("/Engine/EditorResources/AudioIcons/S_AudioComponent_AutoActivate.S_AudioComponent_AutoActivate")));
        }
        else
        {
            SpriteComponent->SetSprite(LoadObject<UTexture2D>(NULL, TEXT("/Engine/EditorResources/AudioIcons/S_AudioComponent.S_AudioComponent")));
        }
    }
}
#endif

#if WITH_EDITOR
void UFMODAudioComponent::PostEditChangeProperty(FPropertyChangedEvent &e)
{
    if (IsPlaying())
    {
        Stop();
        Play();
    }

    FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;
    if (PropertyName == GET_MEMBER_NAME_CHECKED(UFMODAudioComponent, Event) ||
        (PropertyName == GET_MEMBER_NAME_CHECKED(UFMODAudioComponent, ParameterCache) && ParameterCache.Num() == 0))
    {
        ParameterCache.Empty();
        bDefaultParameterValuesCached = false;
    }
    UpdateCachedParameterValues();

#if WITH_EDITORONLY_DATA
    UpdateSpriteTexture();
#endif

    Super::PostEditChangeProperty(e);
}
#endif // WITH_EDITOR

void UFMODAudioComponent::OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
    Super::OnUpdateTransform(UpdateTransformFlags, Teleport);
    if (StudioInstance)
    {
        FMOD_3D_ATTRIBUTES attr = { { 0 } };
        attr.position = FMODUtils::ConvertWorldVector(GetComponentTransform().GetLocation());
        attr.up = FMODUtils::ConvertUnitVector(GetComponentTransform().GetUnitAxis(EAxis::Z));
        attr.forward = FMODUtils::ConvertUnitVector(GetComponentTransform().GetUnitAxis(EAxis::X));
        attr.velocity = FMODUtils::ConvertWorldVector(GetOwner()->GetVelocity());

        StudioInstance->set3DAttributes(&attr);

        UpdateInteriorVolumes();
        UpdateAttenuation();
        ApplyVolumeLPF();
    }
}

// Taken mostly from ActiveSound.cpp
void UFMODAudioComponent::UpdateInteriorVolumes()
{
    if (!GetOwner())
        return; // May not have owner when previewing animations

    if (!bApplyAmbientVolumes)
        return;

    // Result of the ambient calculations to apply to the instance
    float NewAmbientVolumeMultiplier = 1.0f;
    float NewAmbientHighFrequencyGain = 1.0f;

    FInteriorSettings *Ambient =
        (FInteriorSettings *)alloca(sizeof(FInteriorSettings)); // FinteriorSetting::FInteriorSettings() isn't exposed (possible UE4 bug???)
    const FVector &Location = GetOwner()->GetTransform().GetTranslation();
    AAudioVolume *AudioVolume = GetWorld()->GetAudioSettings(Location, NULL, Ambient);

    const FFMODListener &Listener = GetStudioModule().GetNearestListener(Location);
    if (InteriorLastUpdateTime < Listener.InteriorStartTime)
    {
        SourceInteriorVolume = CurrentInteriorVolume;
        SourceInteriorLPF = CurrentInteriorLPF;
        InteriorLastUpdateTime = FApp::GetCurrentTime();
    }

    bool bAllowSpatialization = true;
    if (Listener.Volume == AudioVolume || !bAllowSpatialization)
    {
        // Ambient and listener in same ambient zone
        CurrentInteriorVolume = FMath::Lerp(SourceInteriorVolume, 1.0f, Listener.InteriorVolumeInterp);
        NewAmbientVolumeMultiplier = CurrentInteriorVolume;

        CurrentInteriorLPF = FMath::Lerp(SourceInteriorLPF, MAX_FILTER_FREQUENCY, Listener.InteriorLPFInterp);
        NewAmbientHighFrequencyGain = CurrentInteriorLPF;

        //UE_LOG(LogFMOD, Verbose, TEXT( "Ambient in same volume. Volume *= %g LPF *= %g" ), CurrentInteriorVolume, CurrentInteriorLPF);
    }
    else
    {
        // Ambient and listener in different ambient zone
        if (Ambient->bIsWorldSettings)
        {
            // The ambient sound is 'outside' - use the listener's exterior volume
            CurrentInteriorVolume = FMath::Lerp(SourceInteriorVolume, Listener.InteriorSettings.ExteriorVolume, Listener.ExteriorVolumeInterp);
            NewAmbientVolumeMultiplier = CurrentInteriorVolume;

            CurrentInteriorLPF = FMath::Lerp(SourceInteriorLPF, Listener.InteriorSettings.ExteriorLPF, Listener.ExteriorLPFInterp);
            NewAmbientHighFrequencyGain = CurrentInteriorLPF;

            //UE_LOG(LogFMOD, Verbose, TEXT( "Ambient in diff volume, ambient outside. Volume *= %g LPF *= %g" ), CurrentInteriorVolume, CurrentInteriorLPF);
        }
        else
        {
            // The ambient sound is 'inside' - use the ambient sound's interior volume multiplied with the listeners exterior volume
            CurrentInteriorVolume = FMath::Lerp(SourceInteriorVolume, Ambient->InteriorVolume, Listener.InteriorVolumeInterp);
            CurrentInteriorVolume *= FMath::Lerp(SourceInteriorVolume, Listener.InteriorSettings.ExteriorVolume, Listener.ExteriorVolumeInterp);
            NewAmbientVolumeMultiplier = CurrentInteriorVolume;


            float AmbientLPFValue = FMath::Lerp(SourceInteriorLPF, Ambient->InteriorLPF, Listener.InteriorLPFInterp);
            float ListenerLPFValue = FMath::Lerp(SourceInteriorLPF, Listener.InteriorSettings.ExteriorLPF, Listener.ExteriorLPFInterp);

            // The current interior LPF value is the less of the LPF due to ambient zone and LPF due to listener settings
            if (AmbientLPFValue < ListenerLPFValue)
            {
                CurrentInteriorLPF = AmbientLPFValue;
                NewAmbientHighFrequencyGain = AmbientLPFValue;
            }
            else
            {
                CurrentInteriorLPF = ListenerLPFValue;
                NewAmbientHighFrequencyGain = ListenerLPFValue;
            }

            //UE_LOG(LogFMOD, Verbose, TEXT( "Ambient in diff volume, ambient inside. Volume *= %g LPF *= %g" ), CurrentInteriorVolume, CurrentInteriorLPF);
        }
    }

    AmbientVolume = NewAmbientVolumeMultiplier;
    AmbientLPF = NewAmbientHighFrequencyGain;
}

void UFMODAudioComponent::UpdateAttenuation()
{
    if (!GetOwner())
        return; // May not have owner when previewing animations

    if (!AttenuationDetails.bOverrideAttenuation && !OcclusionDetails.bEnableOcclusion)
    {
        return;
    }

    if (AttenuationDetails.bOverrideAttenuation)
    {
        SetProperty(EFMODEventProperty::MinimumDistance, AttenuationDetails.MinimumDistance);
        SetProperty(EFMODEventProperty::MaximumDistance, AttenuationDetails.MaximumDistance);
    }

    // Use occlusion part of settings
    if (OcclusionDetails.bEnableOcclusion && bApplyOcclusionParameter)
    {
        static FName NAME_SoundOcclusion = FName(TEXT("SoundOcclusion"));
        FCollisionQueryParams Params(NAME_SoundOcclusion, OcclusionDetails.bUseComplexCollisionForOcclusion, GetOwner());

        const FVector &Location = GetOwner()->GetTransform().GetTranslation();
        const FFMODListener &Listener = GetStudioModule().GetNearestListener(Location);

        bool bIsOccluded = GWorld->LineTraceTestByChannel(Location, Listener.Transform.GetLocation(), OcclusionDetails.OcclusionTraceChannel, Params);

        if (bIsOccluded != wasOccluded)
        {
            StudioInstance->setParameterByID(OcclusionID, bIsOccluded ? 1.0f : 0.0f);
            wasOccluded = bIsOccluded;
        }
    }
    else
    {
        wasOccluded = false;
    }
}

void UFMODAudioComponent::ApplyVolumeLPF()
{
    if (bApplyAmbientVolumes)
    {
        float CurVolume = AmbientVolume;
        if (CurVolume != LastVolume)
        {
            StudioInstance->setParameterByID(AmbientVolumeID, CurVolume);
            LastVolume = CurVolume;
        }

        float CurLPF = AmbientLPF;
        if (CurLPF != LastLPF)
        {
            StudioInstance->setParameterByID(AmbientLPFID, CurLPF);
            LastLPF = CurLPF;
        }
    }
    else
    {
        LastLPF = MAX_FILTER_FREQUENCY;
        LastVolume = 1.0f;
    }
}

bool UFMODAudioComponent::ShouldCacheParameter(const FMOD_STUDIO_PARAMETER_DESCRIPTION& ParameterDescription)
{
    const UFMODSettings& Settings = *GetDefault<UFMODSettings>();

    if (((ParameterDescription.flags & FMOD_STUDIO_PARAMETER_GLOBAL) == 0) &&
        (ParameterDescription.type == FMOD_STUDIO_PARAMETER_GAME_CONTROLLED) &&
        ParameterDescription.name != Settings.OcclusionParameter &&
        ParameterDescription.name != Settings.AmbientVolumeParameter &&
        ParameterDescription.name != Settings.AmbientLPFParameter)
    {
        return true;
    }
    return false;
}

void UFMODAudioComponent::CacheDefaultParameterValues()
{
    if (Event)
    {
        TArray<FMOD_STUDIO_PARAMETER_DESCRIPTION> ParameterDescriptions;
        Event->GetParameterDescriptions(ParameterDescriptions);
        for (const FMOD_STUDIO_PARAMETER_DESCRIPTION &ParameterDescription : ParameterDescriptions)
        {
            if (!ParameterCache.Find(ParameterDescription.name) && ShouldCacheParameter(ParameterDescription))
            {
                ParameterCache.Add(ParameterDescription.name, ParameterDescription.defaultvalue);
            }
        }
        bDefaultParameterValuesCached = true;
    }
}

void UFMODAudioComponent::UpdateCachedParameterValues()
{
    if (bDefaultParameterValuesCached)
    {
        TArray<FMOD_STUDIO_PARAMETER_DESCRIPTION> ParameterDescriptions;
        Event->GetParameterDescriptions(ParameterDescriptions);
        for (const FMOD_STUDIO_PARAMETER_DESCRIPTION& ParameterDescription : ParameterDescriptions)
        {
            if (ParameterCache.Find(ParameterDescription.name) && !ShouldCacheParameter(ParameterDescription))
            {
                ParameterCache.Remove(ParameterDescription.name);
                FString paramName(ParameterDescription.name);
                UE_LOG(LogFMOD, Warning, TEXT("Parameter '%s' is driven elsewhere and cannot be added here."), *paramName)
            }
        }
    }
}

void UFMODAudioComponent::OnUnregister()
{
    if (bStopWhenOwnerDestroyed)
    {
        Stop();
    }
    Release();
    Super::OnUnregister();
}

void UFMODAudioComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    bool shouldStop = false;

    switch (EndPlayReason)
    {
        case EEndPlayReason::Destroyed:
        case EEndPlayReason::RemovedFromWorld:
        {
            AActor *Owner = GetOwner();
            if (!Owner || bStopWhenOwnerDestroyed)
                shouldStop = true;
            break;
        }
        case EEndPlayReason::EndPlayInEditor:
        case EEndPlayReason::LevelTransition:
        case EEndPlayReason::Quit:
        {
            shouldStop = true;
            break;
        }
    }

    if (shouldStop)
    {
        Stop();
        OnEventStopped.Broadcast();
    }
    Release();

    bPlayEnded = true;
}

void UFMODAudioComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (IsActive())
    {
        FMOD_STUDIO_PLAYBACK_STATE state = FMOD_STUDIO_PLAYBACK_STOPPED;

        if (StudioInstance)
        {
            if (GetStudioModule().HasListenerMoved())
            {
                UpdateInteriorVolumes();
                UpdateAttenuation();
                ApplyVolumeLPF();
            }

            if (bEnableTimelineCallbacks)
            {
                TArray<FTimelineMarkerProperties> LocalMarkerQueue;
                TArray<FTimelineBeatProperties> LocalBeatQueue;
                {
                    FScopeLock Lock(&CallbackLock);
                    Swap(LocalMarkerQueue, CallbackMarkerQueue);
                    Swap(LocalBeatQueue, CallbackBeatQueue);
                }

                for (const FTimelineMarkerProperties &EachProps : LocalMarkerQueue)
                {
                    OnTimelineMarker.Broadcast(EachProps.Name, EachProps.Position);
                }
                for (const FTimelineBeatProperties &EachProps : LocalBeatQueue)
                {
                    OnTimelineBeat.Broadcast(
                        EachProps.Bar, EachProps.Beat, EachProps.Position, EachProps.Tempo, EachProps.TimeSignatureUpper, EachProps.TimeSignatureLower);
                }
            }

            if (TriggerSoundStoppedDelegate)
            {
                FScopeLock Lock(&CallbackLock);
                TriggerSoundStoppedDelegate = false;

                OnSoundStopped.Broadcast();
            }

            StudioInstance->getPlaybackState(&state);
        }

        if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
        {
            OnPlaybackCompleted();
        }
    }
}

void UFMODAudioComponent::SetEvent(UFMODEvent *NewEvent)
{
    const bool bPlay = IsPlaying();

    Stop();

    if (Event != NewEvent)
    {
        ReleaseEventCache();
        Event = NewEvent;
    }

    if (bPlay)
    {
        Play();
    }
}

void UFMODAudioComponent::PostLoad()
{
    Super::PostLoad();
}

void UFMODAudioComponent::Activate(bool bReset)
{
    if (bReset || ShouldActivate() == true)
    {
        // Don't call Super::Activate here - PlayInternal will do that if successful
        PlayInternal(EFMODSystemContext::Max, bReset);
    }
}

void UFMODAudioComponent::Deactivate()
{
    if (ShouldActivate() == false)
    {
        Stop();
    }
    Super::Deactivate();
}

FMOD_RESULT F_CALLBACK UFMODAudioComponent_EventCallback(FMOD_STUDIO_EVENT_CALLBACK_TYPE type, FMOD_STUDIO_EVENTINSTANCE *event, void *parameters)
{
    UFMODAudioComponent *Component = nullptr;
    FMOD::Studio::EventInstance *Instance = (FMOD::Studio::EventInstance *)event;
    if (Instance->getUserData((void **)&Component) == FMOD_OK && IsValid(Component))
    {
        if (type == FMOD_STUDIO_EVENT_CALLBACK_TIMELINE_MARKER && Component->bEnableTimelineCallbacks)
        {
            Component->EventCallbackAddMarker((FMOD_STUDIO_TIMELINE_MARKER_PROPERTIES *)parameters);
        }
        else if (type == FMOD_STUDIO_EVENT_CALLBACK_TIMELINE_BEAT && Component->bEnableTimelineCallbacks)
        {
            Component->EventCallbackAddBeat((FMOD_STUDIO_TIMELINE_BEAT_PROPERTIES *)parameters);
        }
        else if (type == FMOD_STUDIO_EVENT_CALLBACK_CREATE_PROGRAMMER_SOUND)
        {
            Component->EventCallbackCreateProgrammerSound((FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES *)parameters);
        }
        else if (type == FMOD_STUDIO_EVENT_CALLBACK_DESTROY_PROGRAMMER_SOUND)
        {
            Component->EventCallbackDestroyProgrammerSound((FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES *)parameters);
        }
        else if (type == FMOD_STUDIO_EVENT_CALLBACK_SOUND_STOPPED)
        {
            Component->EventCallbackSoundStopped();
        }
    }
    return FMOD_OK;
}

void UFMODAudioComponent_ReleaseProgrammerSound(FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES *props)
{
    if (props->sound)
    {
        UE_LOG(LogFMOD, Verbose, TEXT("Destroying programmer sound"));
        FMOD_RESULT Result = ((FMOD::Sound *)props->sound)->release();
        verifyfmod(Result);
    }
}

FMOD_RESULT F_CALLBACK UFMODAudioComponent_EventCallbackDestroyProgrammerSound(FMOD_STUDIO_EVENT_CALLBACK_TYPE type, FMOD_STUDIO_EVENTINSTANCE *event, void *parameters)
{
    UFMODAudioComponent_ReleaseProgrammerSound((FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES *)parameters);
    return FMOD_OK;
}

void UFMODAudioComponent::EventCallbackAddMarker(FMOD_STUDIO_TIMELINE_MARKER_PROPERTIES *props)
{
    FScopeLock Lock(&CallbackLock);
    FTimelineMarkerProperties info;
    info.Name = props->name;
    info.Position = props->position;
    CallbackMarkerQueue.Push(info);
}

void UFMODAudioComponent::EventCallbackAddBeat(FMOD_STUDIO_TIMELINE_BEAT_PROPERTIES *props)
{
    FScopeLock Lock(&CallbackLock);
    FTimelineBeatProperties info;
    info.Bar = props->bar;
    info.Beat = props->beat;
    info.Position = props->position;
    info.Tempo = props->tempo;
    info.TimeSignatureUpper = props->timesignatureupper;
    info.TimeSignatureLower = props->timesignaturelower;
    CallbackBeatQueue.Push(info);
}

void UFMODAudioComponent::EventCallbackCreateProgrammerSound(FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES *props)
{
    // Make sure name isn't being changed as we are reading it
    FString ProgrammerSoundNameCopy;
    {
        FScopeLock Lock(&CallbackLock);
        ProgrammerSoundNameCopy = ProgrammerSoundName;
    }

    if (ProgrammerSound)
    {
        props->sound = (FMOD_SOUND *)ProgrammerSound;
        props->subsoundIndex = -1;
    }
    else if (ProgrammerSoundNameCopy.Len() || strlen(props->name) != 0)
    {
        FMOD::Studio::System *System = GetStudioModule().GetStudioSystem(EFMODSystemContext::Max);
        FMOD::System *LowLevelSystem = nullptr;
        System->getCoreSystem(&LowLevelSystem);
        FString SoundName = ProgrammerSoundNameCopy.Len() ? ProgrammerSoundNameCopy : UTF8_TO_TCHAR(props->name);
        FMOD_MODE SoundMode = FMOD_LOOP_NORMAL | FMOD_CREATECOMPRESSEDSAMPLE | FMOD_NONBLOCKING;

        if (SoundName.StartsWith(TEXT("http://")) || 
            SoundName.StartsWith(TEXT("http:\\\\")) || 
            SoundName.StartsWith(TEXT("https://")) || 
            SoundName.StartsWith(TEXT("https:\\\\")))
        {
            // Load via url
            FMOD::Sound* Sound = nullptr;
            FMOD_CREATESOUNDEXINFO exinfo;
            memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
            exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
            exinfo.filebuffersize = 1024 * 16;
            exinfo.ignoresetfilesystem = true;

            if (LowLevelSystem->createSound(TCHAR_TO_UTF8(*SoundName), FMOD_CREATESTREAM | FMOD_NONBLOCKING, &exinfo, &Sound) == FMOD_OK)
            {
                UE_LOG(LogFMOD, Verbose, TEXT("Creating programmer sound from url '%s'"), *SoundName);
                props->sound = (FMOD_SOUND*)Sound;
                props->subsoundIndex = -1;
                NeedDestroyProgrammerSoundCallback = true;
            }
            else
            {
                UE_LOG(LogFMOD, Warning, TEXT("Failed to load programmer sound url '%s'"), *SoundName);
            }
        }
        else if (SoundName.Contains(TEXT(".")))
        {
            // Load via file
            FString SoundPath = SoundName;
            if (FPaths::IsRelative(SoundPath))
            {
                SoundPath = FPaths::ProjectContentDir() / SoundPath;
            }

            FMOD::Sound *Sound = nullptr;
            if (LowLevelSystem->createSound(TCHAR_TO_UTF8(*SoundPath), SoundMode, nullptr, &Sound) == FMOD_OK)
            {
                UE_LOG(LogFMOD, Verbose, TEXT("Creating programmer sound from file '%s'"), *SoundPath);
                props->sound = (FMOD_SOUND *)Sound;
                props->subsoundIndex = -1;
                NeedDestroyProgrammerSoundCallback = true;
            }
            else
            {
                UE_LOG(LogFMOD, Warning, TEXT("Failed to load programmer sound file '%s'"), *SoundPath);
            }
        }
        else
        {
            // Load via FMOD Studio asset table
            FMOD_STUDIO_SOUND_INFO SoundInfo = { 0 };
            FMOD_RESULT Result = System->getSoundInfo(TCHAR_TO_UTF8(*SoundName), &SoundInfo);
            if (Result == FMOD_OK)
            {
                FMOD::Sound *Sound = nullptr;
                Result = LowLevelSystem->createSound(SoundInfo.name_or_data, SoundMode | SoundInfo.mode, &SoundInfo.exinfo, &Sound);
                if (Result == FMOD_OK)
                {
                    UE_LOG(LogFMOD, Verbose, TEXT("Creating programmer sound using audio entry '%s'"), *SoundName);

                    props->sound = (FMOD_SOUND *)Sound;
                    props->subsoundIndex = SoundInfo.subsoundindex;
                    NeedDestroyProgrammerSoundCallback = true;
                }
                else
                {
                    UE_LOG(LogFMOD, Warning, TEXT("Failed to load FMOD audio entry '%s'"), *SoundName);
                }
            }
            else
            {
                UE_LOG(LogFMOD, Warning, TEXT("Failed to find FMOD audio entry '%s'"), *SoundName);
            }
        }
    }
}

void UFMODAudioComponent::EventCallbackSoundStopped()
{
    FScopeLock Lock(&CallbackLock);
    TriggerSoundStoppedDelegate = true;
}

void UFMODAudioComponent::EventCallbackDestroyProgrammerSound(FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES *props)
{
    if (NeedDestroyProgrammerSoundCallback)
    {
        UFMODAudioComponent_ReleaseProgrammerSound(props);
        NeedDestroyProgrammerSoundCallback = false;
    }
}

void UFMODAudioComponent::SetProgrammerSoundName(FString Value)
{
    FScopeLock Lock(&CallbackLock);
    ProgrammerSoundName = Value;
}

void UFMODAudioComponent::SetProgrammerSound(FMOD::Sound *Sound)
{
    FScopeLock Lock(&CallbackLock);
    ProgrammerSound = Sound;
}

void UFMODAudioComponent::Play()
{
    PlayInternal(EFMODSystemContext::Max);
}

void UFMODAudioComponent::PlayInternal(EFMODSystemContext::Type Context, bool bReset)
{
    Stop();

    if (!FMODUtils::IsWorldAudible(GetWorld(), Context == EFMODSystemContext::Editor
        || Context == EFMODSystemContext::Auditioning))
    {
        return;
    }

    UE_LOG(LogFMOD, Verbose, TEXT("UFMODAudioComponent %p Play"), this);

    // Only play events in PIE/game, not when placing them in the editor
    FMOD::Studio::EventDescription *EventDesc = GetStudioModule().GetEventDescription(Event, Context);
    if (EventDesc != nullptr)
    {
        EventDesc->getLength(&EventLength);
        if (!StudioInstance || !StudioInstance->isValid())
        {
            FMOD_RESULT result = EventDesc->createInstance(&StudioInstance);
            if (result != FMOD_OK)
                return;
        }

        const UFMODSettings &Settings = *GetDefault<UFMODSettings>();
        FMOD_STUDIO_PARAMETER_DESCRIPTION paramDesc = {};
        FString param = Settings.OcclusionParameter;
        if (!param.IsEmpty())
        {
            if (EventDesc->getParameterDescriptionByName(TCHAR_TO_UTF8(*Settings.OcclusionParameter), &paramDesc) == FMOD_OK)
            {
                OcclusionID = paramDesc.id;
                bApplyOcclusionParameter = true;
            }
        }

        paramDesc = {};
        param = Settings.AmbientVolumeParameter;
        if (!param.IsEmpty())
        {
            if (EventDesc->getParameterDescriptionByName(TCHAR_TO_UTF8(*param), &paramDesc) == FMOD_OK)
            {
                AmbientVolumeID = paramDesc.id;
                LastVolume = -1.0f;     // Invalidate LastVolume so the AmbientVolumeParameter of the Event will be set later on
                bApplyAmbientVolumes = true;
            }
        }

        paramDesc = {};
        param = Settings.AmbientLPFParameter;
        if (!param.IsEmpty())
        {
            if (EventDesc->getParameterDescriptionByName(TCHAR_TO_UTF8(*Settings.AmbientLPFParameter), &paramDesc) == FMOD_OK)
            {
                AmbientLPFID = paramDesc.id;
                LastLPF = -1.0f;     // Invalidate LastLPF so the AmbientLPFParameter of the Event will be set later on
                bApplyAmbientVolumes = true;
            }
        }

        OnUpdateTransform(EUpdateTransformFlags::SkipPhysicsUpdate);
        // Set initial parameters
        for (auto Kvp : ParameterCache)
        {
            FMOD_RESULT Result = StudioInstance->setParameterByName(TCHAR_TO_UTF8(*Kvp.Key.ToString()), Kvp.Value);
            if (Result != FMOD_OK)
            {
                UE_LOG(LogFMOD, Warning, TEXT("Failed to set initial parameter %s"), *Kvp.Key.ToString());
            }
        }
        for (int i = 0; i < EFMODEventProperty::Count; ++i)
        {
            if (StoredProperties[i] != -1.0f)
            {
                FMOD_RESULT Result = StudioInstance->setProperty((FMOD_STUDIO_EVENT_PROPERTY)i, StoredProperties[i]);
                if (Result != FMOD_OK)
                {
                    UE_LOG(LogFMOD, Warning, TEXT("Failed to set initial property %d"), i);
                }
            }
        }

        if (bEnableTimelineCallbacks || !ProgrammerSoundName.IsEmpty())
        {
            verifyfmod(StudioInstance->setCallback(UFMODAudioComponent_EventCallback));
        }

        verifyfmod(StudioInstance->setUserData(this));
        verifyfmod(StudioInstance->start());
        UE_LOG(LogFMOD, Verbose, TEXT("Playing component %p"), this);

        if (bReset || ShouldActivate() == true)
        {
            Super::Activate(bReset);
        }
    }
}

void UFMODAudioComponent::PauseInternal(PauseContext Pauser)
{
    if (Pauser == Implicit)
    {
        bImplicitlyPaused = true;
    }
    if (Pauser == Explicit)
    {
        bExplicitlyPaused = true;
    }

    SetPaused(true);
}

void UFMODAudioComponent::ResumeInternal(PauseContext Pauser)
{
    if (GetPaused())
    {
        if (Pauser == Implicit)
        {
            bImplicitlyPaused = false;
        }
        if (Pauser == Explicit)
        {
            bExplicitlyPaused = false;
        }

        SetPaused(bImplicitlyPaused || bExplicitlyPaused);
    }
}

void UFMODAudioComponent::Stop()
{
    UE_LOG(LogFMOD, Verbose, TEXT("UFMODAudioComponent %p Stop"), this);
    if (StudioInstance->isValid())
    {
        StudioInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
    }

    wasOccluded = false;
}

void UFMODAudioComponent::Release()
{
    ReleaseEventInstance();
}

void UFMODAudioComponent::ReleaseEventCache()
{
    ParameterCache.Empty();
    bDefaultParameterValuesCached = false;
    ReleaseEventInstance();
}

void UFMODAudioComponent::ReleaseEventInstance()
{
    if (StudioInstance->isValid())
    {
        if (NeedDestroyProgrammerSoundCallback)
        {
            // We need a callback to destroy a programmer sound
            StudioInstance->setCallback(UFMODAudioComponent_EventCallbackDestroyProgrammerSound, FMOD_STUDIO_EVENT_CALLBACK_DESTROY_PROGRAMMER_SOUND);
        }
        else
        {
            // We don't want any more callbacks
            StudioInstance->setCallback(nullptr);
        }

        StudioInstance->release();
        StudioInstance = nullptr;
    }
}

void UFMODAudioComponent::KeyOff()
{
    UE_LOG(LogFMOD, Verbose, TEXT("UFMODAudioComponent %p KeyOff"), this);
    if (StudioInstance)
    {
        StudioInstance->keyOff();
    }
}

void UFMODAudioComponent::OnPlaybackCompleted()
{
    // Mark inactive before calling destroy to avoid recursion
    SetActive(false);
    SetComponentTickEnabled(false);

    if (StudioInstance)
    {
        UE_LOG(LogFMOD, Verbose, TEXT("UFMODAudioComponent %p PlaybackCompleted"), this);
        Release();

        // Fire callback after we have cleaned up our instance
        OnEventStopped.Broadcast();
    }
    
    // Auto destruction is handled via marking object for deletion.
    if (bAutoDestroy)
    {
        DestroyComponent();
    }
}

bool UFMODAudioComponent::IsPlaying(void)
{
    return IsActive();
}

void UFMODAudioComponent::SetVolume(float Volume)
{
    if (StudioInstance)
    {
        FMOD_RESULT Result = StudioInstance->setVolume(Volume);
        if (Result != FMOD_OK)
        {
            UE_LOG(LogFMOD, Warning, TEXT("Failed to set volume"));
        }
    }
}

void UFMODAudioComponent::SetPitch(float Pitch)
{
    if (StudioInstance)
    {
        FMOD_RESULT Result = StudioInstance->setPitch(Pitch);
        if (Result != FMOD_OK)
        {
            UE_LOG(LogFMOD, Warning, TEXT("Failed to set pitch"));
        }
    }
}

void UFMODAudioComponent::SetPaused(bool Paused)
{
    if (StudioInstance)
    {
        FMOD_RESULT Result = StudioInstance->setPaused(Paused);
        if (Result != FMOD_OK)
        {
            UE_LOG(LogFMOD, Warning, TEXT("Failed to pause"));
        }
    }
}

bool UFMODAudioComponent::GetPaused()
{
    bool Paused = false;
    if (StudioInstance)
    {
        FMOD_RESULT Result = StudioInstance->getPaused(&Paused);
        if (Result != FMOD_OK)
        {
            UE_LOG(LogFMOD, Warning, TEXT("Failed to get paused state"));
        }
    }
    return Paused;
}

void UFMODAudioComponent::SetParameter(FName Name, float Value)
{
    if (StudioInstance)
    {
        FMOD_RESULT Result = StudioInstance->setParameterByName(TCHAR_TO_UTF8(*Name.ToString()), Value);
        if (Result != FMOD_OK)
        {
            UE_LOG(LogFMOD, Warning, TEXT("Failed to set parameter %s"), *Name.ToString());
        }
    }
    ParameterCache.FindOrAdd(Name) = Value;
}

void UFMODAudioComponent::SetProperty(EFMODEventProperty::Type Property, float Value)
{
    verify(Property < EFMODEventProperty::Count);
    if (StudioInstance)
    {
        FMOD_RESULT Result = StudioInstance->setProperty((FMOD_STUDIO_EVENT_PROPERTY)Property, Value);
        if (Result != FMOD_OK)
        {
            UE_LOG(LogFMOD, Warning, TEXT("Failed to set property %d"), (int)Property);
        }
    }
    StoredProperties[Property] = Value;
}

float UFMODAudioComponent::GetProperty(EFMODEventProperty::Type Property)
{
    verify(Property < EFMODEventProperty::Count);
    float outValue = 0;
    if (Event)
    {
        FMOD_RESULT Result = StudioInstance->getProperty((FMOD_STUDIO_EVENT_PROPERTY)Property, &outValue);
        if (Result != FMOD_OK)
        {
            UE_LOG(LogFMOD, Warning, TEXT("Failed to get property %d"), (int)Property);
        }
    }
    return StoredProperties[Property] = outValue;
}

int32 UFMODAudioComponent::GetLength() const
{
    return EventLength;
}

void UFMODAudioComponent::SetTimelinePosition(int32 Time)
{
    if (StudioInstance)
    {
        FMOD_RESULT Result = StudioInstance->setTimelinePosition(Time);
        if (Result != FMOD_OK)
        {
            UE_LOG(LogFMOD, Warning, TEXT("Failed to set timeline position"));
        }
    }
}

int32 UFMODAudioComponent::GetTimelinePosition()
{
    int Time = 0;
    if (StudioInstance)
    {
        FMOD_RESULT Result = StudioInstance->getTimelinePosition(&Time);
        if (Result != FMOD_OK)
        {
            UE_LOG(LogFMOD, Warning, TEXT("Failed to get timeline position"));
        }
    }
    return Time;
}

float UFMODAudioComponent::GetParameter(FName Name)
{
    if (!bDefaultParameterValuesCached)
    {
        CacheDefaultParameterValues();
    }

    float *CachedValue = ParameterCache.Find(Name);
    float Value = CachedValue ? *CachedValue : 0.0;
    if (StudioInstance)
    {
        FMOD_RESULT Result = StudioInstance->getParameterByName(TCHAR_TO_UTF8(*Name.ToString()), &Value);
        if (Result != FMOD_OK)
        {
            UE_LOG(LogFMOD, Warning, TEXT("Failed to get parameter %s"), *Name.ToString());
        }
    }
    return Value;
}

void UFMODAudioComponent::GetParameterValue(FName Name, float &UserValue, float &FinalValue)
{
    if (!bDefaultParameterValuesCached)
    {
        CacheDefaultParameterValues();
    }

    float *CachedValue = ParameterCache.Find(Name);
    if (StudioInstance)
    {
        FMOD_RESULT Result = StudioInstance->getParameterByName(TCHAR_TO_UTF8(*Name.ToString()), &UserValue, &FinalValue);
        if (Result != FMOD_OK)
        {
            UserValue = FinalValue = 0;
            UE_LOG(LogFMOD, Warning, TEXT("Failed to get parameter %s"), *Name.ToString());
        }
    }
}