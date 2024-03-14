// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2017.

#include "FMODEventParameterSectionTemplate.h"
#include "FMODAmbientSound.h"
#include "FMODEvent.h"
#include "FMODEventParameterTrack.h"
#include "IMovieScenePlayer.h"
#include "fmod_studio.hpp"

struct FFMODEventParameterPreAnimatedToken : IMovieScenePreAnimatedToken
{
    FFMODEventParameterPreAnimatedToken() {}

    FFMODEventParameterPreAnimatedToken(FFMODEventParameterPreAnimatedToken &&) = default;
    FFMODEventParameterPreAnimatedToken &operator=(FFMODEventParameterPreAnimatedToken &&) = default;

    virtual void RestoreState(UObject &Object, const UE::MovieScene::FRestoreStateParams& Params) override
    {
        UFMODAudioComponent *AudioComponent = CastChecked<UFMODAudioComponent>(&Object);

        if (IsValid(AudioComponent))
        {
            for (FScalarParameterNameAndValue &Value : Values)
            {
                AudioComponent->SetParameter(Value.ParameterName, Value.Value);
            }
        }
    }

    TArray<FScalarParameterNameAndValue> Values;
};

struct FFMODEventParameterPreAnimatedTokenProducer : IMovieScenePreAnimatedTokenProducer
{
    virtual IMovieScenePreAnimatedTokenPtr CacheExistingState(UObject &Object) const override
    {
        UFMODAudioComponent *AudioComponent = CastChecked<UFMODAudioComponent>(&Object);

        FFMODEventParameterPreAnimatedToken Token;

        if (IsValid(AudioComponent) && AudioComponent->Event)
        {
            TArray<FMOD_STUDIO_PARAMETER_DESCRIPTION> ParameterDescriptions;
            AudioComponent->Event->GetParameterDescriptions(ParameterDescriptions);

            for (const FMOD_STUDIO_PARAMETER_DESCRIPTION &ParameterDescription : ParameterDescriptions)
            {
                float Value = AudioComponent->GetParameter(ParameterDescription.name);
                Token.Values.Add(FScalarParameterNameAndValue(ParameterDescription.name, Value));
            }
        }

        return MoveTemp(Token);
    }
};

struct FFMODEventParameterExecutionToken : IMovieSceneExecutionToken
{
    FFMODEventParameterExecutionToken() = default;

    FFMODEventParameterExecutionToken(FFMODEventParameterExecutionToken &&) = default;
    FFMODEventParameterExecutionToken &operator=(FFMODEventParameterExecutionToken &&) = default;

    // Non-copyable
    FFMODEventParameterExecutionToken(const FFMODEventParameterExecutionToken &) = delete;
    FFMODEventParameterExecutionToken &operator=(const FFMODEventParameterExecutionToken &) = delete;

    virtual void Execute(const FMovieSceneContext &Context, const FMovieSceneEvaluationOperand &Operand, FPersistentEvaluationData &PersistentData,
        IMovieScenePlayer &Player)
    {
        for (TWeakObjectPtr<> &WeakObject : Player.FindBoundObjects(Operand))
        {
            UFMODAudioComponent *AudioComponent = Cast<UFMODAudioComponent>(WeakObject.Get());

            if (!AudioComponent)
            {
                AFMODAmbientSound *AmbientSound = Cast<AFMODAmbientSound>(WeakObject.Get());
                AudioComponent = AmbientSound ? AmbientSound->AudioComponent : nullptr;
            }

            if (IsValid(AudioComponent))
            {
                Player.SavePreAnimatedState(
                    *AudioComponent, TMovieSceneAnimTypeID<FFMODEventParameterExecutionToken>(), FFMODEventParameterPreAnimatedTokenProducer());

                for (const FScalarParameterNameAndValue &NameAndValue : Values.ScalarValues)
                {
                    AudioComponent->SetParameter(NameAndValue.ParameterName, NameAndValue.Value);
                }
            }
        }
    }

    FEvaluatedParameterSectionValues Values;
};

FFMODEventParameterSectionTemplate::FFMODEventParameterSectionTemplate(
    const UMovieSceneParameterSection &Section, const UFMODEventParameterTrack &Track)
    : FMovieSceneParameterSectionTemplate(Section)
{
}

void FFMODEventParameterSectionTemplate::Evaluate(const FMovieSceneEvaluationOperand &Operand, const FMovieSceneContext &Context,
    const FPersistentEvaluationData &PersistentData, FMovieSceneExecutionTokens &ExecutionTokens) const
{
    FFMODEventParameterExecutionToken ExecutionToken;
    EvaluateCurves(Context, ExecutionToken.Values);
    ExecutionTokens.Add(MoveTemp(ExecutionToken));
}
