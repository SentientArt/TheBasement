/*
* Copyright (c) 2022 - 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
*
* NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
* property and proprietary rights in and to this material, related
* documentation and any modifications thereto. Any use, reproduction,
* disclosure or distribution of this material and related documentation
* without an express license agreement from NVIDIA CORPORATION or
* its affiliates is strictly prohibited.
*/

#include "StreamlineLibraryDLSSG.h"
#include "StreamlineLibraryPrivate.h"
#if WITH_STREAMLINE

#include "StreamlineCore.h"
#include "StreamlineRHI.h"
#include "StreamlineReflex.h"
#include "StreamlineDLSSG.h"
#include "StreamlineAPI.h"
#include "sl.h"
#include "sl_dlss_g.h"
#endif

#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FStreammlineBlueprintModule"


static const FName SetDLSSGModeInvalidEnumValueError= FName("SetDLSSGModeInvalidEnumValueError");
static const FName IsDLSSGModeSupportedInvalidEnumValueError = FName("IsDLSSGModeSupportedInvalidEnumValueError");

UStreamlineFeatureSupport UStreamlineLibraryDLSSG::DLSSGSupport = UStreamlineFeatureSupport::NotSupportedByPlatformAtBuildTime;
#if WITH_STREAMLINE

bool UStreamlineLibraryDLSSG::bDLSSGLibraryInitialized = false;

static bool ShowDLSSSDebugOnScreenMessages()
{
	return true;
	//if (GetDefault<UDLSSOverrideSettings>()->ShowDLSSSDebugOnScreenMessages == EDLSSSettingOverride::UseProjectSettings)
	//{
	//	return GetDefault<UDLSSSettings>()->bLogStreamlineBlueprint;
	//}
	//else
	//{
	//	return GetDefault<UDLSSOverrideSettings>()->ShowDLSSSDebugOnScreenMessages == EDLSSSettingOverride::Enabled;
	//}
}


#if !UE_BUILD_SHIPPING

UStreamlineLibraryDLSSG::FDLSSErrorState UStreamlineLibraryDLSSG::DLSSErrorState;
FDelegateHandle UStreamlineLibraryDLSSG::DLSSOnScreenMessagesDelegateHandle;
void UStreamlineLibraryDLSSG::GetDLSSOnScreenMessages(TMultiMap<FCoreDelegates::EOnScreenMessageSeverity, FText>& OutMessages)
{
	check(IsInGameThread());

	// We need a valid DLSSSupport, so calling this here in case other UStreamlineLibraryDLSSG functions which call TryInitStreamlineLibrary() haven't been called
	if (!TryInitDLSSGLibrary())
	{
		return;
	}

	// TODO
	//if(ShowDLSSSDebugOnScreenMessages())
	//{
	//
	//}
}
#endif

#endif

bool UStreamlineLibraryDLSSG::IsDLSSGModeSupported(UStreamlineDLSSGMode DLSSGMode)
{
	const UEnum* Enum = StaticEnum<UStreamlineDLSSGMode>();

	// UEnums are strongly typed, but then one can also cast a byte to an UEnum ...
	if (Enum->IsValidEnumValue(int64(DLSSGMode)) && (Enum->GetMaxEnumValue() != int64(DLSSGMode)))
	{
		if (DLSSGMode == UStreamlineDLSSGMode::Off)
		{
			return true;
		}
#if WITH_STREAMLINE
		if (!TryInitDLSSGLibrary())
		{
			UE_LOG(LogStreamlineBlueprint, Error, TEXT("IsDLSSGModeSupported should not be called before PostEngineInit"));
			return false;
		}
		if (!IsDLSSGSupported())
		{
			return false;
		}
		else
		{
			return true; // TODO
		}
#else
		return false;
#endif
	}
	else
	{
#if !UE_BUILD_SHIPPING
		FFrame::KismetExecutionMessage(*FString::Printf(
			TEXT("IsDLSSGModeSupported should not be called with an invalid DLSSGMode enum value (%d) \"%s\""),
			int64(DLSSGMode), *StaticEnum<UStreamlineDLSSGMode>()->GetDisplayNameTextByValue(int64(DLSSGMode)).ToString()),
			ELogVerbosity::Error, IsDLSSGModeSupportedInvalidEnumValueError);
#endif 
		return false;
	}

}

TArray<UStreamlineDLSSGMode> UStreamlineLibraryDLSSG::GetSupportedDLSSGModes()
{
	TArray<UStreamlineDLSSGMode> SupportedQualityModes;
#if WITH_STREAMLINE
	if (!TryInitDLSSGLibrary())
	{
		UE_LOG(LogStreamlineBlueprint, Error, TEXT("GetSupportedDLSSGModes should not be called before PostEngineInit"));
		return SupportedQualityModes;
	}
#endif
	{
		const UEnum* Enum = StaticEnum<UStreamlineDLSSGMode>();
		for (int32 EnumIndex = 0; EnumIndex < Enum->NumEnums(); ++EnumIndex)
		{
			const int64 EnumValue = Enum->GetValueByIndex(EnumIndex);
			if (EnumValue != Enum->GetMaxEnumValue())
			{
				const UStreamlineDLSSGMode QualityMode = UStreamlineDLSSGMode(EnumValue);
				if (IsDLSSGModeSupported(QualityMode))
				{
					SupportedQualityModes.Add(QualityMode);
				}
			}
		}
	}
	return SupportedQualityModes;
}

bool UStreamlineLibraryDLSSG::IsDLSSGSupported()
{
#if WITH_STREAMLINE
	if (!TryInitDLSSGLibrary())
	{
		UE_LOG(LogStreamlineBlueprint, Error, TEXT("IsDLSSGSupported should not be called before PostEngineInit"));
		return false;
	}

	return QueryDLSSGSupport() == UStreamlineFeatureSupport::Supported;
#else
	return false;
#endif
}

UStreamlineFeatureSupport UStreamlineLibraryDLSSG::QueryDLSSGSupport()
{
#if WITH_STREAMLINE
	if (!TryInitDLSSGLibrary())
	{
		UE_LOG(LogStreamlineBlueprint, Error, TEXT("QueryDLSSGSupport should not be called before PostEngineInit"));
		return UStreamlineFeatureSupport::NotSupported;
	}
#endif
	return DLSSGSupport;
}


void UStreamlineLibraryDLSSG::SetDLSSGMode(UStreamlineDLSSGMode DLSSGMode)
{
#if WITH_STREAMLINE
	if (!TryInitDLSSGLibrary())
	{
		UE_LOG(LogStreamlineBlueprint, Error, TEXT("SetDLSSGMode should not be called before PostEngineInit"));
		return;
	}

	const UEnum* Enum = StaticEnum<UStreamlineDLSSGMode>();

	// UEnums are strongly typed, but then one can also cast a byte to an UEnum ...
	if(Enum->IsValidEnumValue(int64(DLSSGMode)) && (Enum->GetMaxEnumValue() != int64(DLSSGMode)))
	{
		static auto CVarDLSSGEnable = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Streamline.DLSSG.Enable"));
		if (CVarDLSSGEnable)
		{
			const bool bDLSSGEnabled = (DLSSGMode != UStreamlineDLSSGMode::Off);
			CVarDLSSGEnable->Set(bDLSSGEnabled ? 1 : 0, ECVF_SetByCommandline);
		}
		
		if (DLSSGMode != UStreamlineDLSSGMode::Off)
		{
#if !UE_BUILD_SHIPPING
			check(IsInGameThread());
			DLSSErrorState.bIsDLSSGModeUnsupported = !IsDLSSGModeSupported(DLSSGMode);
			DLSSErrorState.InvalidDLSSGMode = DLSSGMode;
#endif 
		}
	}
	else
	{
#if !UE_BUILD_SHIPPING
		FFrame::KismetExecutionMessage(*FString::Printf(
			TEXT("SetDLSSGMode should not be called with an invalid DLSSGMode enum value (%d) \"%s\""), 
			int64(DLSSGMode), *StaticEnum<UStreamlineDLSSGMode>()->GetDisplayNameTextByValue(int64(DLSSGMode)).ToString()),
			ELogVerbosity::Error, SetDLSSGModeInvalidEnumValueError);
#endif 
	}
#endif	// WITH_STREAMLINE
}

STREAMLINEBLUEPRINT_API void UStreamlineLibraryDLSSG::GetDLSSGFrameTiming(float& FrameRateInHertz, int32& FramesPresented)
{
#if WITH_STREAMLINE
	if (!TryInitDLSSGLibrary())
	{
		UE_LOG(LogStreamlineBlueprint, Error, TEXT("GetDLSSGFrameTiming should not be called before PostEngineInit"));
		return;
	}
	GetStreamlineDLSSGFrameTiming(FrameRateInHertz, FramesPresented);
#endif

}

UStreamlineDLSSGMode UStreamlineLibraryDLSSG::GetDLSSGMode()
{
#if WITH_STREAMLINE
	if (!TryInitDLSSGLibrary())
	{
		UE_LOG(LogStreamlineBlueprint, Error, TEXT("GetDLSSGMode should not be called before PostEngineInit"));
		return UStreamlineDLSSGMode::Off;
	}

	static const auto CVarDLSSGEnable = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Streamline.DLSSG.Enable"));
	const bool bDLSSGEnabled = CVarDLSSGEnable && CVarDLSSGEnable->GetInt() != 0;

	if (bDLSSGEnabled)
	{
		return UStreamlineDLSSGMode::On;
	}

#endif
	return UStreamlineDLSSGMode::Off;
}

UStreamlineDLSSGMode UStreamlineLibraryDLSSG::GetDefaultDLSSGMode()
{
#if WITH_STREAMLINE
	if (!TryInitDLSSGLibrary())
	{
		UE_LOG(LogStreamlineBlueprint, Error, TEXT("GetDefaultDLSSGMode should not be called before PostEngineInit"));
		return UStreamlineDLSSGMode::Off;
	}
#endif
	if (UStreamlineLibraryDLSSG::IsDLSSGSupported())
	{
		return UStreamlineDLSSGMode::Off;
	}
	else
	{
		return UStreamlineDLSSGMode::Off;
	}
}

#if WITH_STREAMLINE

// Delayed initialization, which allows this module to be available early so blueprints can be loaded before DLSS is available in PostEngineInit
bool UStreamlineLibraryDLSSG::TryInitDLSSGLibrary()
{
	if (bDLSSGLibraryInitialized)
	{
		// TODO
		return true;
	}

	// Register this before we bail out so we can show error messages
#if !UE_BUILD_SHIPPING
	if (!DLSSOnScreenMessagesDelegateHandle.IsValid())
	{
		DLSSOnScreenMessagesDelegateHandle = FCoreDelegates::OnGetOnScreenMessages.AddStatic(&GetDLSSOnScreenMessages);
	}
#endif

	

	if (IsStreamlineSupported())
	{
		if (GetPlatformStreamlineRHI()->IsDLSSGSupportedByRHI())
		{

			DLSSGSupport = ToUStreamlineFeatureSupport(QueryStreamlineDLSSGSupport());
		}
		else
		{
			DLSSGSupport = UStreamlineFeatureSupport::NotSupportedByRHI;
		}
	}
	else
	{
		if (GetPlatformStreamlineSupport() == EStreamlineSupport::NotSupportedIncompatibleRHI)
		{
			DLSSGSupport = UStreamlineFeatureSupport::NotSupportedByRHI;
		}
		else
		{
			DLSSGSupport = UStreamlineFeatureSupport::NotSupported;
		}
	}

	bDLSSGLibraryInitialized = true;

	return true;
}
#endif // WITH_STREAMLINE


void UStreamlineLibraryDLSSG::Startup()
{
#if WITH_STREAMLINE
	// This initialization will likely not succeed unless this module has been moved to PostEngineInit, and that's ok
	UStreamlineLibraryDLSSG::TryInitDLSSGLibrary();
#else
	UE_LOG(LogStreamlineBlueprint, Log, TEXT("Streamline is not supported on this platform at build time. The Streamline Blueprint library however is supported and stubbed out to ignore any calls to enable DLSS-G and will always return UStreamlineDLSSGSupport::NotSupportedByPlatformAtBuildTime, regardless of the underlying hardware. This can be used to e.g. to turn off DLSS-G related UI elements."));
	UStreamlineLibraryDLSSG::DLSSGSupport = UStreamlineFeatureSupport::NotSupportedByPlatformAtBuildTime;
#endif
}
void UStreamlineLibraryDLSSG::Shutdown()
{
#if WITH_STREAMLINE && !UE_BUILD_SHIPPING
	if (UStreamlineLibraryDLSSG::DLSSOnScreenMessagesDelegateHandle.IsValid())
	{
		FCoreDelegates::OnGetOnScreenMessages.Remove(UStreamlineLibraryDLSSG::DLSSOnScreenMessagesDelegateHandle);
		UStreamlineLibraryDLSSG::DLSSOnScreenMessagesDelegateHandle.Reset();
	}
#endif
}



#undef LOCTEXT_NAMESPACE