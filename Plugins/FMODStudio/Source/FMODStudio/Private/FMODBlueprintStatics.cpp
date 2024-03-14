// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#include "FMODBlueprintStatics.h"
#include "FMODAudioComponent.h"
#include "FMODSettings.h"
#include "FMODStudioModule.h"
#include "FMODUtils.h"
#include "FMODBank.h"
#include "FMODEvent.h"
#include "FMODBus.h"
#include "FMODVCA.h"
#include "fmod_studio.hpp"
#include "fmod_errors.h"
#include "FMODStudioPrivatePCH.h"

/////////////////////////////////////////////////////
// UFMODBlueprintStatics

UFMODBlueprintStatics::UFMODBlueprintStatics(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
{
}

FFMODEventInstance UFMODBlueprintStatics::PlayEvent2D(UObject *WorldContextObject, class UFMODEvent *Event, bool bAutoPlay)
{
    return PlayEventAtLocation(WorldContextObject, Event, FTransform(), bAutoPlay);
}

FFMODEventInstance UFMODBlueprintStatics::PlayEventAtLocation(
    UObject *WorldContextObject, class UFMODEvent *Event, const FTransform &Location, bool bAutoPlay)
{
    FFMODEventInstance Instance;
    Instance.Instance = nullptr;

    UWorld *ThisWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
    if (FMODUtils::IsWorldAudible(ThisWorld, false) && IsValid(Event))
    {
        FMOD::Studio::EventDescription *EventDesc = IFMODStudioModule::Get().GetEventDescription(Event);
        if (EventDesc != nullptr)
        {
            FMOD::Studio::EventInstance *EventInst = nullptr;
            EventDesc->createInstance(&EventInst);
            if (EventInst != nullptr)
            {
                FMOD_3D_ATTRIBUTES EventAttr = { { 0 } };
                FMODUtils::Assign(EventAttr, Location);
                EventInst->set3DAttributes(&EventAttr);

                if (bAutoPlay)
                {
                    EventInst->start();
                    EventInst->release();
                }
                Instance.Instance = EventInst;
            }
        }
    }
    return Instance;
}

class UFMODAudioComponent *UFMODBlueprintStatics::PlayEventAttached(class UFMODEvent *Event, class USceneComponent *AttachToComponent,
    FName AttachPointName, FVector Location, EAttachLocation::Type LocationType, bool bStopWhenAttachedToDestroyed, bool bAutoPlay, bool bAutoDestroy)
{
    if (!IFMODStudioModule::Get().UseSound())
    {
        return nullptr;
    }

    if (Event == nullptr)
    {
        return nullptr;
    }
    if (AttachToComponent == nullptr)
    {
        UE_LOG(LogFMOD, Warning, TEXT("UFMODBlueprintStatics::PlayEventAttached: NULL AttachComponent specified!"));
        return nullptr;
    }

    AActor *Actor = AttachToComponent->GetOwner();

    // Avoid creating component if we're trying to play a sound on an already destroyed actor.
    if (!IsValid(Actor))
    {
        return nullptr;
    }

    UFMODAudioComponent *AudioComponent;
    if (Actor)
    {
        // Use actor as outer if we have one.
        AudioComponent = NewObject<UFMODAudioComponent>(Actor);
    }
    else
    {
        // Let engine pick the outer (transient package).
        AudioComponent = NewObject<UFMODAudioComponent>();
    }
    check(AudioComponent);
    AudioComponent->Event = Event;
    AudioComponent->bAutoActivate = false;
    AudioComponent->bAutoDestroy = bAutoDestroy;
    AudioComponent->bStopWhenOwnerDestroyed = bStopWhenAttachedToDestroyed;
#if WITH_EDITORONLY_DATA
    AudioComponent->bVisualizeComponent = false;
#endif
    AudioComponent->RegisterComponentWithWorld(AttachToComponent->GetWorld());

    AudioComponent->AttachToComponent(AttachToComponent, FAttachmentTransformRules::KeepRelativeTransform, AttachPointName);
    if (LocationType == EAttachLocation::KeepWorldPosition)
    {
        AudioComponent->SetWorldLocation(Location);
    }
    else
    {
        AudioComponent->SetRelativeLocation(Location);
    }

    if (bAutoPlay)
    {
        AudioComponent->Play();
    }
    return AudioComponent;
}

UFMODAsset *UFMODBlueprintStatics::FindAssetByName(const FString &Name)
{
    return IFMODStudioModule::Get().FindAssetByName(Name);
}

UFMODEvent *UFMODBlueprintStatics::FindEventByName(const FString &Name)
{
    return IFMODStudioModule::Get().FindEventByName(Name);
}

void UFMODBlueprintStatics::LoadBank(class UFMODBank *Bank, bool bBlocking, bool bLoadSampleData)
{
    FMOD::Studio::System *StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
    if (StudioSystem != nullptr && IsValid(Bank))
    {
        UE_LOG(LogFMOD, Log, TEXT("LoadBank %s"), *Bank->GetName());

        FString BankPath = IFMODStudioModule::Get().GetBankPath(*Bank);
        FMOD::Studio::Bank *bank = nullptr;
        FMOD_STUDIO_LOAD_BANK_FLAGS flags = (bBlocking || bLoadSampleData) ? FMOD_STUDIO_LOAD_BANK_NORMAL : FMOD_STUDIO_LOAD_BANK_NONBLOCKING;

        FMOD_RESULT result = StudioSystem->loadBankFile(TCHAR_TO_UTF8(*BankPath), flags, &bank);
        if (result != FMOD_OK)
        {
            UE_LOG(LogFMOD, Error, TEXT("Failed to load bank %s: %s"), *Bank->GetName(), UTF8_TO_TCHAR(FMOD_ErrorString(result)));
        }
        if (result == FMOD_OK && bLoadSampleData)
        {
            bank->loadSampleData();
        }
    }
}

void UFMODBlueprintStatics::UnloadBank(class UFMODBank *Bank)
{
    FMOD::Studio::System *StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
    if (StudioSystem != nullptr && IsValid(Bank))
    {
        UE_LOG(LogFMOD, Log, TEXT("UnloadBank %s"), *Bank->GetName());

        FMOD::Studio::ID guid = FMODUtils::ConvertGuid(Bank->AssetGuid);
        FMOD::Studio::Bank *bank = nullptr;
        FMOD_RESULT result = StudioSystem->getBankByID(&guid, &bank);
        if (result == FMOD_OK && bank != nullptr)
        {
            bank->unload();
        }
    }
}

bool UFMODBlueprintStatics::IsBankLoaded(class UFMODBank *Bank)
{
    FMOD::Studio::System *StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
    if (StudioSystem != nullptr && IsValid(Bank))
    {
        FMOD::Studio::ID guid = FMODUtils::ConvertGuid(Bank->AssetGuid);
        FMOD::Studio::Bank *bank = nullptr;
        FMOD_RESULT result = StudioSystem->getBankByID(&guid, &bank);
        if (result == FMOD_OK && bank != nullptr)
        {
            FMOD_STUDIO_LOADING_STATE loadingState;
            if (bank->getLoadingState(&loadingState) == FMOD_OK)
            {
                return (loadingState == FMOD_STUDIO_LOADING_STATE_LOADED);
            }
        }
    }
    return false;
}

void UFMODBlueprintStatics::LoadBankSampleData(class UFMODBank *Bank)
{
    FMOD::Studio::System *StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
    if (StudioSystem != nullptr && IsValid(Bank))
    {
        FMOD::Studio::ID guid = FMODUtils::ConvertGuid(Bank->AssetGuid);
        FMOD::Studio::Bank *bank = nullptr;
        FMOD_RESULT result = StudioSystem->getBankByID(&guid, &bank);
        if (result == FMOD_OK && bank != nullptr)
        {
            bank->loadSampleData();
        }
    }
}

void UFMODBlueprintStatics::UnloadBankSampleData(class UFMODBank *Bank)
{
    FMOD::Studio::System *StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
    if (StudioSystem != nullptr && IsValid(Bank))
    {
        FMOD::Studio::ID guid = FMODUtils::ConvertGuid(Bank->AssetGuid);
        FMOD::Studio::Bank *bank = nullptr;
        FMOD_RESULT result = StudioSystem->getBankByID(&guid, &bank);
        if (result == FMOD_OK && bank != nullptr)
        {
            bank->unloadSampleData();
        }
    }
}

void UFMODBlueprintStatics::LoadEventSampleData(UObject *WorldContextObject, class UFMODEvent *Event)
{
    if (IsValid(Event))
    {
        FMOD::Studio::EventDescription *EventDesc = IFMODStudioModule::Get().GetEventDescription(Event);
        if (EventDesc != nullptr)
        {
            EventDesc->loadSampleData();
        }
    }
}

void UFMODBlueprintStatics::UnloadEventSampleData(UObject *WorldContextObject, class UFMODEvent *Event)
{
    if (IsValid(Event))
    {
        FMOD::Studio::EventDescription *EventDesc = IFMODStudioModule::Get().GetEventDescription(Event);
        if (EventDesc != nullptr)
        {
            EventDesc->unloadSampleData();
        }
    }
}

TArray<FFMODEventInstance> UFMODBlueprintStatics::FindEventInstances(UObject *WorldContextObject, UFMODEvent *Event)
{
    TArray<FFMODEventInstance> Instances;
    if (IsValid(Event))
    {
        FMOD::Studio::EventDescription *EventDesc = IFMODStudioModule::Get().GetEventDescription(Event);
        if (EventDesc != nullptr)
        {
            int Capacity = 0;
            EventDesc->getInstanceCount(&Capacity);
            if (Capacity > 0)
            {
                TArray<FMOD::Studio::EventInstance *> InstancePointers;
                InstancePointers.SetNum(Capacity, true);
                int Count = 0;
                EventDesc->getInstanceList(InstancePointers.GetData(), Capacity, &Count);
                Instances.SetNum(Count, true);
                for (int i = 0; i < Count; ++i)
                {
                    Instances[i].Instance = InstancePointers[i];
                }
            }
        }
    }
    return Instances;
}

void UFMODBlueprintStatics::BusSetVolume(class UFMODBus *Bus, float Volume)
{
    FMOD::Studio::System *StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
    if (StudioSystem != nullptr && IsValid(Bus))
    {
        FMOD::Studio::ID guid = FMODUtils::ConvertGuid(Bus->AssetGuid);
        FMOD::Studio::Bus *bus = nullptr;
        FMOD_RESULT result = StudioSystem->getBusByID(&guid, &bus);
        if (result == FMOD_OK && bus != nullptr)
        {
            bus->setVolume(Volume);
        }
    }
}

void UFMODBlueprintStatics::BusSetPaused(class UFMODBus *Bus, bool bPaused)
{
    FMOD::Studio::System *StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
    if (StudioSystem != nullptr && IsValid(Bus))
    {
        FMOD::Studio::ID guid = FMODUtils::ConvertGuid(Bus->AssetGuid);
        FMOD::Studio::Bus *bus = nullptr;
        FMOD_RESULT result = StudioSystem->getBusByID(&guid, &bus);
        if (result == FMOD_OK && bus != nullptr)
        {
            bus->setPaused(bPaused);
        }
    }
}

void UFMODBlueprintStatics::BusSetMute(class UFMODBus *Bus, bool bMute)
{
    FMOD::Studio::System *StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
    if (StudioSystem != nullptr && IsValid(Bus))
    {
        FMOD::Studio::ID guid = FMODUtils::ConvertGuid(Bus->AssetGuid);
        FMOD::Studio::Bus *bus = nullptr;
        FMOD_RESULT result = StudioSystem->getBusByID(&guid, &bus);
        if (result == FMOD_OK && bus != nullptr)
        {
            bus->setMute(bMute);
        }
    }
}

void UFMODBlueprintStatics::BusStopAllEvents(UFMODBus *Bus, EFMOD_STUDIO_STOP_MODE stopMode)
{
    FMOD::Studio::System *StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
    if (StudioSystem != nullptr && IsValid(Bus))
    {
        FMOD::Studio::ID guid = FMODUtils::ConvertGuid(Bus->AssetGuid);
        FMOD::Studio::Bus *bus = nullptr;
        FMOD_RESULT result = StudioSystem->getBusByID(&guid, &bus);
        if (result == FMOD_OK && bus != nullptr)
        {
            bus->stopAllEvents((FMOD_STUDIO_STOP_MODE)stopMode);
        }
    }
}

void UFMODBlueprintStatics::VCASetVolume(class UFMODVCA *Vca, float Volume)
{
    FMOD::Studio::System *StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
    if (StudioSystem != nullptr && IsValid(Vca))
    {
        FMOD::Studio::ID guid = FMODUtils::ConvertGuid(Vca->AssetGuid);
        FMOD::Studio::VCA *vca = nullptr;
        FMOD_RESULT result = StudioSystem->getVCAByID(&guid, &vca);
        if (result == FMOD_OK && vca != nullptr)
        {
            vca->setVolume(Volume);
        }
    }
}

void UFMODBlueprintStatics::SetGlobalParameterByName(FName Name, float Value)
{
    FMOD::Studio::System *StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
    if (StudioSystem != nullptr)
    {
        FMOD_RESULT Result = StudioSystem->setParameterByName(TCHAR_TO_UTF8(*Name.ToString()), Value);
        if (Result != FMOD_OK)
        {
            UE_LOG(LogFMOD, Warning, TEXT("Failed to set parameter %s"), *Name.ToString());
        }
    }
}

float UFMODBlueprintStatics::GetGlobalParameterByName(FName Name)
{
    FMOD::Studio::System *StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
    float Value = 0.0f;
    if (StudioSystem != nullptr)
    {
        FMOD_RESULT Result = StudioSystem->getParameterByName(TCHAR_TO_UTF8(*Name.ToString()), &Value);
        if (Result != FMOD_OK)
        {
            UE_LOG(LogFMOD, Warning, TEXT("Failed to get event instance parameter %s"), *Name.ToString());
        }
    }
    return Value;
}

void UFMODBlueprintStatics::GetGlobalParameterValueByName(FName Name, float &UserValue, float &FinalValue)
{
    FMOD::Studio::System *StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
    if (StudioSystem != nullptr)
    {
        FMOD_RESULT Result = StudioSystem->getParameterByName(TCHAR_TO_UTF8(*Name.ToString()), &UserValue, &FinalValue);
        if (Result != FMOD_OK)
        {
            UserValue = FinalValue = 0.0f;
            UE_LOG(LogFMOD, Warning, TEXT("Failed to get event instance parameter %s"), *Name.ToString());
        }
    }
}

bool UFMODBlueprintStatics::EventInstanceIsValid(FFMODEventInstance EventInstance)
{
    if (EventInstance.Instance)
    {
        return EventInstance.Instance->isValid();
    }
    return false;
}

void UFMODBlueprintStatics::EventInstanceSetVolume(FFMODEventInstance EventInstance, float Volume)
{
    if (EventInstance.Instance)
    {
        FMOD_RESULT Result = EventInstance.Instance->setVolume(Volume);
        if (Result != FMOD_OK)
        {
            UE_LOG(LogFMOD, Warning, TEXT("Failed to set event instance volume"));
        }
    }
}

void UFMODBlueprintStatics::EventInstanceSetPitch(FFMODEventInstance EventInstance, float Pitch)
{
    if (EventInstance.Instance)
    {
        FMOD_RESULT Result = EventInstance.Instance->setPitch(Pitch);
        if (Result != FMOD_OK)
        {
            UE_LOG(LogFMOD, Warning, TEXT("Failed to set event instance pitch"));
        }
    }
}

void UFMODBlueprintStatics::EventInstanceSetPaused(FFMODEventInstance EventInstance, bool Paused)
{
    if (EventInstance.Instance)
    {
        FMOD_RESULT Result = EventInstance.Instance->setPaused(Paused);
        if (Result != FMOD_OK)
        {
            UE_LOG(LogFMOD, Warning, TEXT("Failed to pause event instance"));
        }
    }
}

void UFMODBlueprintStatics::EventInstanceSetParameter(FFMODEventInstance EventInstance, FName Name, float Value)
{
    if (EventInstance.Instance)
    {
        FMOD_RESULT Result = EventInstance.Instance->setParameterByName(TCHAR_TO_UTF8(*Name.ToString()), Value);
        if (Result != FMOD_OK)
        {
            UE_LOG(LogFMOD, Warning, TEXT("Failed to set event instance parameter %s"), *Name.ToString());
        }
    }
}

float UFMODBlueprintStatics::EventInstanceGetParameter(FFMODEventInstance EventInstance, FName Name)
{
    float Value = 0.0f;
    if (EventInstance.Instance)
    {
        FMOD_RESULT Result = EventInstance.Instance->getParameterByName(TCHAR_TO_UTF8(*Name.ToString()), &Value);
        if (Result != FMOD_OK)
        {
            UE_LOG(LogFMOD, Warning, TEXT("Failed to get event instance parameter %s"), *Name.ToString());
        }
    }
    return Value;
}

void UFMODBlueprintStatics::EventInstanceGetParameterValue(FFMODEventInstance EventInstance, FName Name, float &UserValue, float &FinalValue)
{
    if (EventInstance.Instance)
    {
        FMOD_RESULT Result = EventInstance.Instance->getParameterByName(TCHAR_TO_UTF8(*Name.ToString()), &UserValue, &FinalValue);
        if (Result != FMOD_OK)
        {
            UserValue = FinalValue = 0.0f;
            UE_LOG(LogFMOD, Warning, TEXT("Failed to get event instance parameter %s"), *Name.ToString());
        }
    }
}

void UFMODBlueprintStatics::EventInstanceSetProperty(FFMODEventInstance EventInstance, EFMODEventProperty::Type Property, float Value)
{
    if (EventInstance.Instance)
    {
        FMOD_RESULT Result = EventInstance.Instance->setProperty((FMOD_STUDIO_EVENT_PROPERTY)Property, Value);

        if (Result != FMOD_OK)
        {
            UE_LOG(LogFMOD, Warning, TEXT("Failed to set event instance property type %d to value %f (%s)"), (int)Property, Value,
                FMOD_ErrorString(Result));
        }
    }
}

void UFMODBlueprintStatics::EventInstancePlay(FFMODEventInstance EventInstance)
{
    if (EventInstance.Instance)
    {
        FMOD_RESULT Result = EventInstance.Instance->start();
        if (Result != FMOD_OK)
        {
            UE_LOG(LogFMOD, Warning, TEXT("Failed to play event instance"));
        }
        // Once we start playing, allow instance to be cleaned up when it finishes
        EventInstance.Instance->release();
    }
}

void UFMODBlueprintStatics::EventInstanceStop(FFMODEventInstance EventInstance, bool Release)
{
    if (EventInstance.Instance)
    {
        FMOD_RESULT Result = EventInstance.Instance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
        if (Result != FMOD_OK)
        {
            UE_LOG(LogFMOD, Warning, TEXT("Failed to stop event instance"));
        }
        else
        {
            if (Release)
            {
                EventInstanceRelease(EventInstance);
                //EventInstance.Instance->release();
            }
        }
    }
}

void UFMODBlueprintStatics::EventInstanceRelease(FFMODEventInstance EventInstance)
{
    if (EventInstance.Instance)
    {
        FMOD_RESULT Result = EventInstance.Instance->release();
        if (Result != FMOD_OK)
        {
            UE_LOG(LogFMOD, Warning, TEXT("Failed to release event instance"));
        }
    }
}

void UFMODBlueprintStatics::EventInstanceKeyOff(FFMODEventInstance EventInstance)
{
    if (EventInstance.Instance)
    {
        EventInstance.Instance->keyOff();
    }
}

void UFMODBlueprintStatics::EventInstanceSetTransform(FFMODEventInstance EventInstance, const FTransform &Location)
{
    if (EventInstance.Instance)
    {
        FMOD_3D_ATTRIBUTES attr = { { 0 } };
        FMODUtils::Assign(attr, Location);
        FMOD_RESULT Result = EventInstance.Instance->set3DAttributes(&attr);
        if (Result != FMOD_OK)
        {
            UE_LOG(LogFMOD, Warning, TEXT("Failed to set transform on event instance"));
        }
    }
}

TArray<FString> UFMODBlueprintStatics::GetOutputDrivers()
{
    TArray<FString> AllNames;

    FMOD::Studio::System *StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
    if (StudioSystem != nullptr)
    {
        FMOD::System *LowLevelSystem = nullptr;
        verifyfmod(StudioSystem->getCoreSystem(&LowLevelSystem));

        int DriverCount = 0;
        verifyfmod(LowLevelSystem->getNumDrivers(&DriverCount));

        for (int id = 0; id < DriverCount; ++id)
        {
            char DriverNameUTF8[256] = {};
            verifyfmod(LowLevelSystem->getDriverInfo(id, DriverNameUTF8, sizeof(DriverNameUTF8), 0, 0, 0, 0));
            FString DriverName(UTF8_TO_TCHAR(DriverNameUTF8));
            AllNames.Add(DriverName);
        }
    }

    return AllNames;
}

void UFMODBlueprintStatics::SetOutputDriverByName(FString NewDriverName)
{
    FMOD::Studio::System *StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
    if (StudioSystem != nullptr)
    {
        FMOD::System *LowLevelSystem = nullptr;
        verifyfmod(StudioSystem->getCoreSystem(&LowLevelSystem));

        int DriverIndex = -1;
        int DriverCount = 0;
        verifyfmod(LowLevelSystem->getNumDrivers(&DriverCount));

        for (int id = 0; id < DriverCount; ++id)
        {
            char DriverNameUTF8[256] = {};
            verifyfmod(LowLevelSystem->getDriverInfo(id, DriverNameUTF8, sizeof(DriverNameUTF8), 0, 0, 0, 0));
            FString DriverName(UTF8_TO_TCHAR(DriverNameUTF8));
            UE_LOG(LogFMOD, Log, TEXT("Driver %d: %s"), id, *DriverName);
            if (DriverName.Contains(NewDriverName))
            {
                DriverIndex = id;
            }
        }

        if (DriverIndex >= 0)
        {
            UE_LOG(LogFMOD, Log, TEXT("Selected driver %d"), DriverIndex);
            verifyfmod(LowLevelSystem->setDriver(DriverIndex));
        }
        else
        {
            UE_LOG(LogFMOD, Warning, TEXT("Did not find driver of name '%s'"), *NewDriverName);
        }
    }
}

void UFMODBlueprintStatics::SetOutputDriverByIndex(int NewDriverIndex)
{
    FMOD::Studio::System *StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
    if (StudioSystem != nullptr)
    {
        FMOD::System *LowLevelSystem = nullptr;
        verifyfmod(StudioSystem->getCoreSystem(&LowLevelSystem));

        int DriverCount = 0;
        verifyfmod(LowLevelSystem->getNumDrivers(&DriverCount));

        if (NewDriverIndex >= 0 && NewDriverIndex < DriverCount)
        {
            UE_LOG(LogFMOD, Log, TEXT("Selected driver %d"), NewDriverIndex);
            verifyfmod(LowLevelSystem->setDriver(NewDriverIndex));
        }
        else
        {
            UE_LOG(LogFMOD, Warning, TEXT("Driver %d out of range"), NewDriverIndex);
        }
    }
}

void UFMODBlueprintStatics::MixerSuspend()
{
    UE_LOG(LogFMOD, Log, TEXT("MixerSuspend called"));
    FMOD::Studio::System *StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
    if (StudioSystem != nullptr)
    {
        FMOD::System *LowLevelSystem = nullptr;
        verifyfmod(StudioSystem->getCoreSystem(&LowLevelSystem));

        verifyfmod(LowLevelSystem->mixerSuspend());
    }
}

void UFMODBlueprintStatics::MixerResume()
{
    UE_LOG(LogFMOD, Log, TEXT("MixerResume called"));
    FMOD::Studio::System *StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
    if (StudioSystem != nullptr)
    {
        FMOD::System *LowLevelSystem = nullptr;
        verifyfmod(StudioSystem->getCoreSystem(&LowLevelSystem));

        verifyfmod(LowLevelSystem->mixerResume());
    }
}

void UFMODBlueprintStatics::SetLocale(const FString& Locale)
{
    IFMODStudioModule::Get().SetLocale(Locale);
}
