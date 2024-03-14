// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#include "FMODParameterSection.h"
#include "ISectionLayoutBuilder.h"
#include "ScopedTransaction.h"
#include "Sections/MovieSceneParameterSection.h"

#define LOCTEXT_NAMESPACE "FMODParameterSection"

bool FFMODParameterSection::RequestDeleteKeyArea(const TArray<FName> &KeyAreaNamePath)
{
    if (KeyAreaNamePath.Num() == 1)
    {
        const FScopedTransaction Transaction(LOCTEXT("DeleteEventParameter", "Delete event parameter"));
        UMovieSceneParameterSection *ParameterSection = Cast<UMovieSceneParameterSection>(WeakSection.Get());
        if (ParameterSection->TryModify())
        {
            return ParameterSection->RemoveScalarParameter(KeyAreaNamePath[0]);
        }
    }

    return true;
}

#undef LOCTEXT_NAMESPACE
