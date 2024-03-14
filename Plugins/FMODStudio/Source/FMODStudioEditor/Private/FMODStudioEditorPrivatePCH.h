// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.
#pragma once

#include "Runtime/Engine/Classes/Components/SceneComponent.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Curves/IntegralCurve.h"
#include "Runtime/Engine/Classes/Curves/NameCurve.h"
#include "Runtime/Engine/Classes/Curves/RichCurve.h"
#include "Runtime/MovieScene/Public/MovieScene.h"
#include "Runtime/MovieScene/Public/KeyParams.h"

#include "Editor/Sequencer/Public/ISectionLayoutBuilder.h"
#include "Editor/Sequencer/Public/ISequencerSection.h"
#include "Editor/Sequencer/Public/MovieSceneTrackEditor.h"
#include "Runtime/MovieScene/Public/MovieSceneCommonHelpers.h"
#include "Runtime/MovieScene/Public/MovieSceneSection.h"
#include "Editor/UnrealEd/Public/ScopedTransaction.h"

DECLARE_LOG_CATEGORY_EXTERN(LogFMOD, Log, All);