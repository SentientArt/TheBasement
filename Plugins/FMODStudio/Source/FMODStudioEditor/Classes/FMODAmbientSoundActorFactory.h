// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#pragma once

#include "ActorFactories/ActorFactory.h"
#include "FMODAmbientSoundActorFactory.generated.h"

/** FMOD Ambient Sound Actor Factory.
*/
UCLASS(MinimalAPI, config = Editor, collapsecategories, hidecategories = Object)
class UFMODAmbientSoundActorFactory : public UActorFactory
{
    GENERATED_UCLASS_BODY()

    // Begin UActorFactory Interface
    virtual void PostSpawnActor(UObject *Asset, AActor *NewActor) override;
    virtual void PostCreateBlueprint(UObject *Asset, AActor *CDO) override;
    virtual bool CanCreateActorFrom(const FAssetData &AssetData, FText &OutErrorMsg) override;
    virtual UObject *GetAssetFromActorInstance(AActor *ActorInstance) override;
    // End UActorFactory Interface
};
