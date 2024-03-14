// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#pragma once

#include "ComponentVisualizer.h"

class FFMODAudioComponentVisualizer : public FComponentVisualizer
{
public:
    // Begin FComponentVisualizer interface
    virtual void DrawVisualization(const UActorComponent *Component, const FSceneView *View, FPrimitiveDrawInterface *PDI) override;
    // End FComponentVisualizer interface
};
