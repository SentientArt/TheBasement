// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#pragma once

#include "fmod.hpp"
#include "GenericPlatform/GenericPlatform.h"

FMOD_RESULT F_CALLBACK FMODLogCallback(FMOD_DEBUG_FLAGS flags, const char *file, int line, const char *func, const char *message);
FMOD_RESULT F_CALLBACK FMODErrorCallback(FMOD_SYSTEM *system, FMOD_SYSTEM_CALLBACK_TYPE type, void *commanddata1, void *commanddata2, void *userdata);

void AcquireFMODFileSystem();
void ReleaseFMODFileSystem();
void AttachFMODFileSystem(FMOD::System *system, FGenericPlatformTypes::int32 fileBufferSize);
