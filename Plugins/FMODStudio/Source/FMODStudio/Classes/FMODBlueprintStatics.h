// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#pragma once

#include "FMODAudioComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Containers/UnrealString.h"
#include "FMODBlueprintStatics.generated.h"

class UFMODAudioComponent;

namespace FMOD
{
namespace Studio
{
class EventDescription;
class EventInstance;
}
}

class UFMODAsset;
class UFMODEvent;
class USceneComponent;

USTRUCT(BlueprintType)
struct FFMODEventInstance
{
    GENERATED_USTRUCT_BODY()

    FMOD::Studio::EventInstance *Instance;
};

UENUM(BlueprintType)
enum EFMOD_STUDIO_STOP_MODE
{
    ALLOWFADEOUT,
    IMMEDIATE
};

UCLASS()
class FMODSTUDIO_API UFMODBlueprintStatics : public UBlueprintFunctionLibrary
{
    GENERATED_UCLASS_BODY()

    /** Plays an event.  This returns an FMOD Event Instance.  The sound does not travel with any actor.
	 * @param Event - event to play
	 * @param bAutoPlay - Start the event automatically.
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD",
        meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", AdvancedDisplay = "2", bAutoPlay = "true",
            UnsafeDuringActorConstruction = "true"))
    static FFMODEventInstance PlayEvent2D(UObject *WorldContextObject, UFMODEvent *Event, bool bAutoPlay);

    /** Plays an event at the given location. This returns an FMOD Event Instance.  The sound does not travel with any actor.
	 * @param Event - event to play
	 * @param Location - World position to play event at
	 * @param bAutoPlay - Start the event automatically.
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD",
        meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", AdvancedDisplay = "2", bAutoPlay = "true",
            UnsafeDuringActorConstruction = "true"))
    static FFMODEventInstance PlayEventAtLocation(UObject *WorldContextObject, UFMODEvent *Event, const FTransform &Location, bool bAutoPlay);

    /** Plays an event attached to and following the specified component.
	 * @param Event - event to play
	 * @param AttachComponent - Component to attach to.
	 * @param AttachPointName - Optional named point within the AttachComponent to play the sound at
	 * @param Location - Depending on the value of Location Type this is either a relative offset from the attach component/point or an absolute world position that will be translated to a relative offset
	 * @param LocationType - Specifies whether Location is a relative offset or an absolute world position
	 * @param bStopWhenAttachedToDestroyed - Specifies whether the sound should stop playing when the owner of the attach to component is destroyed.
	 * @param bAutoPlay - Start the event automatically.
	 * @param bAutoDestroy - Automatically destroy the audio component when the sound is stopped.
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD",
        meta = (AdvancedDisplay = "2", UnsafeDuringActorConstruction = "true", bAutoPlay = "true", bAutoDestroy = "true"))
    static class UFMODAudioComponent *PlayEventAttached(UFMODEvent *Event, USceneComponent *AttachToComponent, FName AttachPointName,
        FVector Location, EAttachLocation::Type LocationType, bool bStopWhenAttachedToDestroyed, bool bAutoPlay, bool bAutoDestroy);

    /** Find an asset by name.
	 * @param EventName - The asset name
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD")
    static UFMODAsset *FindAssetByName(const FString &Name);

    /** Find an event by name.
	 * @param EventName - The event name
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD")
    static UFMODEvent *FindEventByName(const FString &Name);

    /** Loads a bank.
	 * @param Bank - bank to load
	 * @param bBlocking - determines whether the bank will load synchronously
	 * @param bLoadSampleData - determines whether sample data will be preloaded immediately
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD", meta = (UnsafeDuringActorConstruction = "true", bBlocking = "true"))
    static void LoadBank(class UFMODBank *Bank, bool bBlocking, bool bLoadSampleData);

    /** Unloads a bank.
	 * @param Bank - bank to unload
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD", meta = (UnsafeDuringActorConstruction = "true"))
    static void UnloadBank(class UFMODBank *Bank);

    /** Returns true if a bank is loaded.
	* @param Bank - bank to query
	*/
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD", meta = (UnsafeDuringActorConstruction = "true"))
    static bool IsBankLoaded(class UFMODBank *Bank);

    /** Load bank sample data.
	 * @param Bank - bank to load sample data from
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD", meta = (UnsafeDuringActorConstruction = "true"))
    static void LoadBankSampleData(class UFMODBank *Bank);

    /** Unload bank sample data.
	 * @param Bank - bank to unload sample data from
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD", meta = (UnsafeDuringActorConstruction = "true"))
    static void UnloadBankSampleData(class UFMODBank *Bank);

    /** Load event sample data.  This can be done ahead of time to avoid loading stalls.
	 * @param Event - event to load sample data from.
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD",
        meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
    static void LoadEventSampleData(UObject *WorldContextObject, UFMODEvent *Event);

    /** Unload event sample data.
	 * @param Event - event to load sample data from.
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD",
        meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
    static void UnloadEventSampleData(UObject *WorldContextObject, UFMODEvent *Event);

    /** Return a list of all event instances that are playing for this event.
		Be careful using this function because it is possible to find and alter any playing sound, even ones owned by other audio components.
	 * @param Event - event to find instances from.
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD",
        meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
    static TArray<FFMODEventInstance> FindEventInstances(UObject *WorldContextObject, UFMODEvent *Event);

    /** Set volume on a bus
	 * @param Bus - bus to use
	 * @param Volume - volume
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|Bus", meta = (UnsafeDuringActorConstruction = "true"))
    static void BusSetVolume(class UFMODBus *Bus, float Volume);

    /** Pause/Unpause all events going through this bus
	 * @param Bus - bus to use
	 * @param bPaused - paused
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|Bus", meta = (UnsafeDuringActorConstruction = "true"))
    static void BusSetPaused(class UFMODBus *Bus, bool bPaused);

    /** Mute/Unmute this bus
	 * @param Bus - bus to use
	 * @param bMute - mute
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|Bus", meta = (UnsafeDuringActorConstruction = "true"))
    static void BusSetMute(class UFMODBus *Bus, bool bMute);

    /** Stops all EventInstances routed into the bus
	 * @param Bus - bus to use
	 * @param stopMode - desired stop mode
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|Bus", meta = (UnsafeDuringActorConstruction = "true"))
    static void BusStopAllEvents(class UFMODBus *Bus, EFMOD_STUDIO_STOP_MODE stopMode);

    /** Set volume on a VCA
	 * @param Vca - VCA to use
	 * @param Volume - volume
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|VCA", meta = (UnsafeDuringActorConstruction = "true"))
    static void VCASetVolume(class UFMODVCA *Vca, float Volume);

    /** Set a global parameter from the System.
     * @param Name - Name of parameter
     * @param Value - Value of parameter
     */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD", meta = (UnsafeDuringActorConstruction = "true"))
    static void SetGlobalParameterByName(FName Name, float Value);

    /** Will be deprecated in FMOD 2.01, use `GetGlobalParameterValueByName(FName, float, float)` instead.
     * Get a global parameter from the System.
     * @param Name - Name of parameter
     */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD", meta = (UnsafeDuringActorConstruction = "true"))
    static float GetGlobalParameterByName(FName Name);

     /** Get a global parameter from the System.
     * @param Name - Name of parameter
     * @param UserValue - Parameter value as set from the public API.
     * @param FinalValue - Final combined parameter value.
     */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD", meta = (UnsafeDuringActorConstruction = "true"))
    static void GetGlobalParameterValueByName(FName Name, float &UserValue, float &FinalValue);

    /** Returns whether this FMOD Event Instance is valid.  The instance will be invalidated when the sound stops.
	 * @param EventInstance - Event instance
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|EventInstance", meta = (UnsafeDuringActorConstruction = "true"))
    static bool EventInstanceIsValid(FFMODEventInstance EventInstance);

    /** Set volume on an FMOD Event Instance.
	 * @param EventInstance - Event instance
	 * @param Value - Volume to set
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|EventInstance", meta = (UnsafeDuringActorConstruction = "true"))
    static void EventInstanceSetVolume(FFMODEventInstance EventInstance, float Volume);

    /** Set pitch on an FMOD Event Instance.
	 * @param EventInstance - Event instance
	 * @param Value - Volume to set
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|EventInstance", meta = (UnsafeDuringActorConstruction = "true"))
    static void EventInstanceSetPitch(FFMODEventInstance EventInstance, float Pitch);

    /** Pause/Unpause an FMOD Event Instance.
	 * @param EventInstance - Event instance
	 * @param Paused - Whether to pause or unpause
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|EventInstance", meta = (UnsafeDuringActorConstruction = "true"))
    static void EventInstanceSetPaused(FFMODEventInstance EventInstance, bool Paused);

    /** Set a parameter on an FMOD Event Instance.
	 * @param EventInstance - Event instance
	 * @param Name - Name of parameter
	 * @param Value - Value of parameter
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|EventInstance", meta = (UnsafeDuringActorConstruction = "true"))
    static void EventInstanceSetParameter(FFMODEventInstance EventInstance, FName Name, float Value);

    /** Will be deprecated in FMOD 2.01, use `EventInstanceGetParameterValue(FFMODEventInstance, FName, float, float)` instead.
     * Get a parameter on an FMOD Event Instance.
	 * @param EventInstance - Event instance
	 * @param Name - Name of parameter
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|EventInstance", meta = (UnsafeDuringActorConstruction = "true"))
    static float EventInstanceGetParameter(FFMODEventInstance EventInstance, FName Name);

     /** Get a parameter on an FMOD Event Instance.
	 * @param EventInstance - Event instance
	 * @param Name - Name of parameter
     * @param UserValue - Parameter value as set from the public API.
     * @param FinalValue - Final combined parameter value.
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|EventInstance", meta = (UnsafeDuringActorConstruction = "true"))
    static void EventInstanceGetParameterValue(FFMODEventInstance EventInstance, FName Name, float &UserValue, float &FinalValue);
    /** Set an FMOD event property on an FMOD Event Instance.
	* @param EventInstance - Event instance
	* @param Property - Property to set
	* @param Value - Value to set
	*/
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|EventInstance", meta = (UnsafeDuringActorConstruction = "true"))
    static void EventInstanceSetProperty(FFMODEventInstance EventInstance, EFMODEventProperty::Type Property, float Value);

    /** Plays an FMOD Event Instance.
	 * @param EventInstance - Event instance
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|EventInstance", meta = (UnsafeDuringActorConstruction = "true"))
    static void EventInstancePlay(FFMODEventInstance EventInstance);

    /** Stop an FMOD Event Instance.
	 * @param EventInstance - Event instance
     * @param Release - Whether to release the Event Instance
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|EventInstance", meta = (UnsafeDuringActorConstruction = "true"))
    static void EventInstanceStop(FFMODEventInstance EventInstance, bool Release = false);

    /** Release an FMOD Event Instance.
    * @param EventInstance - Event instance
    */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|EventInstance", meta = (UnsafeDuringActorConstruction = "true"))
    static void EventInstanceRelease(FFMODEventInstance EventInstance);

    /** Allow an FMOD Event Instance to continue past a sustain point.
	 * @param EventInstance - Event instance
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|EventInstance", meta = (UnsafeDuringActorConstruction = "true"))
    static void EventInstanceKeyOff(FFMODEventInstance EventInstance);

    /** Set 3D attributes on a FMOD Event Instance.
	 * @param EventInstance - Event instance
	 * @param Location - Location to place event
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|EventInstance", meta = (UnsafeDuringActorConstruction = "true"))
    static void EventInstanceSetTransform(FFMODEventInstance EventInstance, const FTransform &Location);

    /** List all output device names.
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD")
    static TArray<FString> GetOutputDrivers();

    /** Set current output device by name or part of the name.
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD")
    static void SetOutputDriverByName(FString NewDriverName);

    /** Set current output device by its index from GetOutputDrivers.
	 */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD")
    static void SetOutputDriverByIndex(int NewDriverIndex);

    /** Suspend the FMOD mixer.  Used when suspending the application.
	*/
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD")
    static void MixerSuspend();

    /** Resume the FMOD mixer.  Used when resuming the application.
	*/
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD")
    static void MixerResume();

    /** Set the active locale for subsequent bank loads.
    */
    UFUNCTION(BlueprintCallable, Category = "Audio|FMOD")
    static void SetLocale(const FString& Locale);
};
