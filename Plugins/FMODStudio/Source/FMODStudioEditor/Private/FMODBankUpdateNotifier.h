// Copyright (c), Firelight Technologies Pty, Ltd. 2012-2023.

#pragma once

#include "Containers/UnrealString.h"
#include "Misc/DateTime.h"
#include "Delegates/Delegate.h"

class FFMODBankUpdateNotifier
{
public:
    FFMODBankUpdateNotifier();

    void SetFilePath(const FString &InPath);
    void Update(float DeltaTime);

    void EnableUpdate(bool bEnable);

    FSimpleMulticastDelegate BanksUpdatedEvent;

private:
    void Refresh();
    FDateTime MostRecentFileTime();

    bool bUpdateEnabled;
    FString FilePath;
    FDateTime NextRefreshTime;
    FDateTime FileTime;
    float Countdown;
};
