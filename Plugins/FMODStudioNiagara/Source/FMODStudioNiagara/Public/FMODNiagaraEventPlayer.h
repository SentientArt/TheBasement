#pragma once

#include "FMODEvent.h"
#include "FMODAudioComponent.h"

#include "NiagaraCommon.h"
#include "NiagaraDataInterface.h"
#include "NiagaraShared.h"
#include "NiagaraSystem.h"

#include "Sound/SoundAttenuation.h"

#include "FMODStudioNiagara.h"
#include "FMODNiagaraEventPlayer.generated.h"

struct FEventParticleData
{
    FVector Position;
    FRotator Rotation;
};

struct FPersistentEventParticleData
{
    int32 AudioHandle = 0;

    /** The update callback is executed in PerInstanceTickPostSimulate, which runs on the game thread */
    TFunction<void(struct FEventPlayerInterface_InstanceData*, UFMODAudioComponent*, FNiagaraSystemInstance*)> UpdateCallback;
};

struct FEventPlayerInterface_InstanceData
{
    /** We use a lock-free queue here because multiple threads might try to push data to it at the same time. */
    TQueue<FEventParticleData, EQueueMode::Mpsc> PlayAudioQueue;
    TQueue<FPersistentEventParticleData, EQueueMode::Mpsc> PersistentAudioActionQueue;
    FThreadSafeCounter HandleCount;

    TSortedMap<int32, TWeakObjectPtr<UFMODAudioComponent>> PersistentAudioMapping;

    TWeakObjectPtr<UFMODEvent> EventToPlay;
    TArray<FName> ParameterNames;

    FNiagaraLWCConverter LWCConverter;
    int32 MaxPlaysPerTick = 0;
    bool bStopWhenComponentIsDestroyed = true;

#if WITH_EDITORONLY_DATA
    bool bOnlyActiveDuringGameplay = true;
#endif
};

/** This Data Interface can be used to play one-shot audio effects driven by particle data. */
UCLASS(EditInlineNew, Category = "Audio", meta = (DisplayName = "FMOD Event Player"))
class FMODSTUDIONIAGARA_API UFMODNiagaraEventPlayer : public UNiagaraDataInterface
{
    GENERATED_UCLASS_BODY()

public:
    /** Reference to the audio asset to play */
    UPROPERTY(EditAnywhere, Category = "Audio")
        UFMODEvent* EventToPlay;

    /** A set of parameter names that can be referenced via index when setting sound cue parameters on persistent audio */
    UPROPERTY(VisibleAnywhere, NoClear, Category = "Parameters")
        TArray<FName> ParameterNames;

    UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Audio", meta = (InlineEditConditionToggle))
        bool bLimitPlaysPerTick;

    /** This sets the max number of sounds played each tick.
     *  If more particles try to play a sound in a given tick, then it will play sounds until the limit is reached and discard the rest.
     *  The particles to discard when over the limit are *not* chosen in a deterministic way. */
    UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Audio", meta = (EditCondition = "bLimitPlaysPerTick", ClampMin = "0", UIMin = "0"))
        int32 MaxPlaysPerTick;

    /** If false then it the audio component keeps playing after the niagara component was destroyed. Looping sounds are always stopped when the component is destroyed. */
    UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "Audio")
        bool bStopWhenComponentIsDestroyed = true;

#if WITH_EDITORONLY_DATA
    /** If true then this data interface only processes sounds during active gameplay. This is useful when you are working in the preview window and the sounds annoy you.
     *  Currently the FMOD Niagara integration will not play in Editor.
     */
    UPROPERTY(VisibleAnywhere, AdvancedDisplay, Category = "Audio")
        bool bOnlyActiveDuringGameplay = true;

    virtual bool UpgradeFunctionCall(FNiagaraFunctionSignature& FunctionSignature) override;
#endif

    //UObject Interface
    virtual void PostInitProperties() override;
#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& e);
#endif
    //UObject Interface End

    //UNiagaraDataInterface Interface
    virtual void GetFunctions(TArray<FNiagaraFunctionSignature>& OutFunctions) override;
    virtual void GetVMExternalFunction(const FVMExternalFunctionBindingInfo& BindingInfo, void* InstanceData, FVMExternalFunction& OutFunc) override;
    virtual bool InitPerInstanceData(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance) override;
    virtual void DestroyPerInstanceData(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance) override;
    virtual int32 PerInstanceDataSize() const override { return sizeof(FEventPlayerInterface_InstanceData); }
    virtual bool PerInstanceTick(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance, float DeltaSeconds) override;
    virtual bool PerInstanceTickPostSimulate(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance, float DeltaSeconds) override;
    virtual bool Equals(const UNiagaraDataInterface* Other) const override;
    virtual bool CanExecuteOnTarget(ENiagaraSimTarget Target) const override { return Target == ENiagaraSimTarget::CPUSim; }

    virtual bool HasPreSimulateTick() const override { return true; }
    virtual bool HasPostSimulateTick() const override { return true; }
    virtual bool PostSimulateCanOverlapFrames() const { return false; }
    //UNiagaraDataInterface Interface

    virtual void PlayOneShotAudio(FVectorVMExternalFunctionContext& Context);
    virtual void PlayPersistentAudio(FVectorVMExternalFunctionContext& Context);
    virtual void SetParameterFloat(FVectorVMExternalFunctionContext& Context);
    virtual void UpdateLocation(FVectorVMExternalFunctionContext& Context);
    virtual void UpdateRotation(FVectorVMExternalFunctionContext& Context);
    virtual void SetPausedState(FVectorVMExternalFunctionContext& Context);

#if WITH_EDITOR
    void CacheDefaultParameterValues();
    bool ShouldCacheParameter(const FMOD_STUDIO_PARAMETER_DESCRIPTION& ParameterDescription);
    bool bDefaultParameterValuesCached;
#endif

protected:
    virtual bool CopyToInternal(UNiagaraDataInterface* Destination) const override;

private:
    static const FName PlayAudioName;
    static const FName PlayPersistentAudioName;
    static const FName SetPersistentAudioLocationName;
    static const FName SetPersistentAudioRotationName;
    static const FName SetPersistentAudioFloatParamName;
    static const FName PausePersistentAudioName;
};