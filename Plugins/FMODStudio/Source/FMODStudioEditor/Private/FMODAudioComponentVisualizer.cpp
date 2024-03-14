// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#include "FMODAudioComponentVisualizer.h"
#include "FMODAudioComponent.h"
#include "FMODUtils.h"
#include "FMODEvent.h"
#include "fmod_studio.hpp"
#include "SceneView.h"
#include "SceneManagement.h"

void FFMODAudioComponentVisualizer::DrawVisualization(const UActorComponent *Component, const FSceneView *View, FPrimitiveDrawInterface *PDI)
{
    if (View->Family->EngineShowFlags.AudioRadius)
    {
        const UFMODAudioComponent *AudioComp = Cast<const UFMODAudioComponent>(Component);
        if (IsValid(AudioComp) && AudioComp->Event)
        {
            FMOD::Studio::EventDescription *EventDesc =
                IFMODStudioModule::Get().GetEventDescription(AudioComp->Event, EFMODSystemContext::Auditioning);
            if (EventDesc != nullptr)
            {
                bool bIs3D = false;
                EventDesc->is3D(&bIs3D);
                if (bIs3D)
                {
                    const FColor AudioOuterRadiusColor(255, 153, 0);
                    const FColor AudioInnerRadiusColor(216, 130, 0);

                    const FTransform &Transform = AudioComp->GetComponentTransform();

                    float MinDistance = 0.0f;
                    float MaxDistance = 0.0f;
                    if (AudioComp->AttenuationDetails.bOverrideAttenuation)
                    {
                        MinDistance = AudioComp->AttenuationDetails.MinimumDistance;
                        MaxDistance = AudioComp->AttenuationDetails.MaximumDistance;
                    }
                    else
                    {
                        EventDesc->getMinMaxDistance(&MinDistance, &MaxDistance);
                    }
                    MinDistance = FMODUtils::DistanceToUEScale(MinDistance);
                    MaxDistance = FMODUtils::DistanceToUEScale(MaxDistance);

                    DrawWireSphereAutoSides(PDI, Transform.GetTranslation(), AudioOuterRadiusColor, MinDistance, SDPG_World);
                    if (MaxDistance != MinDistance)
                    {
                        DrawWireSphereAutoSides(PDI, Transform.GetTranslation(), AudioInnerRadiusColor, MaxDistance, SDPG_World);
                    }
                }
            }
        }
    }
}
