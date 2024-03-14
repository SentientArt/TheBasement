// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Evaluation/MovieSceneParameterTemplate.h"
#include "FMODEventParameterSectionTemplate.generated.h"

class UFMODEventParameterTrack;

USTRUCT()
struct FFMODEventParameterSectionTemplate : public FMovieSceneParameterSectionTemplate
{
    GENERATED_BODY()

    FFMODEventParameterSectionTemplate() {}
    FFMODEventParameterSectionTemplate(const UMovieSceneParameterSection &Section, const UFMODEventParameterTrack &Track);

private:
    virtual UScriptStruct &GetScriptStructImpl() const override { return *StaticStruct(); }
    virtual void Evaluate(const FMovieSceneEvaluationOperand &Operand, const FMovieSceneContext &Context,
        const FPersistentEvaluationData &PersistentData, FMovieSceneExecutionTokens &ExecutionTokens) const override;
};
