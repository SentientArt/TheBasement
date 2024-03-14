/*
* Copyright (c) 2020 - 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
*
* NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
* property and proprietary rights in and to this material, related
* documentation and any modifications thereto. Any use, reproduction,
* disclosure or distribution of this material and related documentation
* without an express license agreement from NVIDIA CORPORATION or
* its affiliates is strictly prohibited.
*/

#pragma once

#include "Modules/ModuleManager.h"

class FDLSSUpscaler;
class FDLSSDenoiser;
class ISceneViewExtension;
class FDLSSUpscalerViewExtension;
class FNGXAutomationViewExtension;
class NGXRHI;


enum class ENGXSupport : uint8
{
	Supported,
	NotSupported,
	NotSupportedIncompatibleHardware,
	NotSupportedDriverOutOfDate,
	NotSupportedOperatingSystemOutOfDate,
	NotSupportedIncompatibleAPICaptureToolActive,
};

enum class EDLSSSupport : uint8
{
	Supported,
	NotSupported,
	NotSupportedIncompatibleHardware,
	NotSupportedDriverOutOfDate,
	NotSupportedOperatingSystemOutOfDate,
	NotSupportedIncompatibleAPICaptureToolActive,
};

class IDLSSModuleInterface : public IModuleInterface
{
	public:
		virtual EDLSSSupport QueryDLSSSRSupport() const = 0;
		virtual EDLSSSupport QueryDLSSRRSupport() const = 0;
		virtual void GetDLSSSRMinDriverVersion(int32& MajorVersion, int32& MinorVersion) const = 0;
		virtual void GetDLSSRRMinDriverVersion(int32& MajorVersion, int32& MinorVersion) const = 0;
		virtual float GetResolutionFractionForQuality(int32 Quality) const = 0;
		virtual FDLSSUpscaler* GetDLSSUpscaler() const = 0;
		virtual TSharedPtr< ISceneViewExtension, ESPMode::ThreadSafe> GetDLSSUpscalerViewExtension() const = 0;
};

class FDLSSModule final: public IDLSSModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	// Inherited via IDLSSModuleInterface
	virtual EDLSSSupport QueryDLSSSRSupport() const override;
	virtual EDLSSSupport QueryDLSSRRSupport() const override;
	virtual void GetDLSSSRMinDriverVersion(int32& MajorVersion, int32& MinorVersion) const override;
	virtual void GetDLSSRRMinDriverVersion(int32& MajorVersion, int32& MinorVersion) const override;
	virtual float GetResolutionFractionForQuality(int32 Quality) const override;
	virtual FDLSSUpscaler* GetDLSSUpscaler() const override;

	virtual TSharedPtr< ISceneViewExtension, ESPMode::ThreadSafe> GetDLSSUpscalerViewExtension() const override;

private:

	TUniquePtr<FDLSSUpscaler> DLSSUpscaler;
	TUniquePtr<FDLSSDenoiser> DLSSDenoiser;
	TUniquePtr<NGXRHI> NGXRHIExtensions;
	TSharedPtr< FDLSSUpscalerViewExtension, ESPMode::ThreadSafe> DLSSUpscalerViewExtension;
	TSharedPtr< FNGXAutomationViewExtension, ESPMode::ThreadSafe> NGXAutomationViewExtension;
	ENGXSupport NGXSupport = ENGXSupport::NotSupported;
	EDLSSSupport DLSSSRSupport = EDLSSSupport::NotSupported;
	EDLSSSupport DLSSRRSupport = EDLSSSupport::NotSupported;
	int32 MinSRDriverVersionMinor;
	int32 MinSRDriverVersionMajor;
	int32 MinRRDriverVersionMinor;
	int32 MinRRDriverVersionMajor;
};
