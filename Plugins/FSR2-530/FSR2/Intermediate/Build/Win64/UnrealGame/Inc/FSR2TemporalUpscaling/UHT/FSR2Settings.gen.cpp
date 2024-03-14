// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "../../../../../../../../Users/iFraude/Documents/GitHub/City75/Plugins/FSR2-530/FSR2/Source/FSR2TemporalUpscaling/Public/FSR2Settings.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeFSR2Settings() {}
// Cross Module References
	DEVELOPERSETTINGS_API UClass* Z_Construct_UClass_UDeveloperSettings();
	ENGINE_API UEnum* Z_Construct_UEnum_Engine_EMaterialShadingModel();
	FSR2TEMPORALUPSCALING_API UClass* Z_Construct_UClass_UFSR2Settings();
	FSR2TEMPORALUPSCALING_API UClass* Z_Construct_UClass_UFSR2Settings_NoRegister();
	FSR2TEMPORALUPSCALING_API UEnum* Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2DeDitherMode();
	FSR2TEMPORALUPSCALING_API UEnum* Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2HistoryFormat();
	FSR2TEMPORALUPSCALING_API UEnum* Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2LandscapeHISMMode();
	FSR2TEMPORALUPSCALING_API UEnum* Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2QualityMode();
	UPackage* Z_Construct_UPackage__Script_FSR2TemporalUpscaling();
// End Cross Module References
	static FEnumRegistrationInfo Z_Registration_Info_UEnum_EFSR2QualityMode;
	static UEnum* EFSR2QualityMode_StaticEnum()
	{
		if (!Z_Registration_Info_UEnum_EFSR2QualityMode.OuterSingleton)
		{
			Z_Registration_Info_UEnum_EFSR2QualityMode.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2QualityMode, (UObject*)Z_Construct_UPackage__Script_FSR2TemporalUpscaling(), TEXT("EFSR2QualityMode"));
		}
		return Z_Registration_Info_UEnum_EFSR2QualityMode.OuterSingleton;
	}
	template<> FSR2TEMPORALUPSCALING_API UEnum* StaticEnum<EFSR2QualityMode>()
	{
		return EFSR2QualityMode_StaticEnum();
	}
	struct Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2QualityMode_Statics
	{
		static const UECodeGen_Private::FEnumeratorParam Enumerators[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[];
#endif
		static const UECodeGen_Private::FEnumParams EnumParams;
	};
	const UECodeGen_Private::FEnumeratorParam Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2QualityMode_Statics::Enumerators[] = {
		{ "EFSR2QualityMode::Unused", (int64)EFSR2QualityMode::Unused },
		{ "EFSR2QualityMode::Quality", (int64)EFSR2QualityMode::Quality },
		{ "EFSR2QualityMode::Balanced", (int64)EFSR2QualityMode::Balanced },
		{ "EFSR2QualityMode::Performance", (int64)EFSR2QualityMode::Performance },
		{ "EFSR2QualityMode::UltraPerformance", (int64)EFSR2QualityMode::UltraPerformance },
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2QualityMode_Statics::Enum_MetaDataParams[] = {
		{ "Balanced.DisplayName", "Balanced" },
		{ "Balanced.Name", "EFSR2QualityMode::Balanced" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "//-------------------------------------------------------------------------------------\n// The official FSR2 quality modes.\n//-------------------------------------------------------------------------------------\n" },
#endif
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
		{ "Performance.DisplayName", "Performance" },
		{ "Performance.Name", "EFSR2QualityMode::Performance" },
		{ "Quality.DisplayName", "Quality" },
		{ "Quality.Name", "EFSR2QualityMode::Quality" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "The official FSR2 quality modes." },
#endif
		{ "UltraPerformance.DisplayName", "Ultra Performance" },
		{ "UltraPerformance.Name", "EFSR2QualityMode::UltraPerformance" },
		{ "Unused.Hidden", "" },
		{ "Unused.Name", "EFSR2QualityMode::Unused" },
	};
#endif
	const UECodeGen_Private::FEnumParams Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2QualityMode_Statics::EnumParams = {
		(UObject*(*)())Z_Construct_UPackage__Script_FSR2TemporalUpscaling,
		nullptr,
		"EFSR2QualityMode",
		"EFSR2QualityMode",
		Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2QualityMode_Statics::Enumerators,
		RF_Public|RF_Transient|RF_MarkAsNative,
		UE_ARRAY_COUNT(Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2QualityMode_Statics::Enumerators),
		EEnumFlags::None,
		(uint8)UEnum::ECppForm::EnumClass,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2QualityMode_Statics::Enum_MetaDataParams), Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2QualityMode_Statics::Enum_MetaDataParams)
	};
	UEnum* Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2QualityMode()
	{
		if (!Z_Registration_Info_UEnum_EFSR2QualityMode.InnerSingleton)
		{
			UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_EFSR2QualityMode.InnerSingleton, Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2QualityMode_Statics::EnumParams);
		}
		return Z_Registration_Info_UEnum_EFSR2QualityMode.InnerSingleton;
	}
	static FEnumRegistrationInfo Z_Registration_Info_UEnum_EFSR2HistoryFormat;
	static UEnum* EFSR2HistoryFormat_StaticEnum()
	{
		if (!Z_Registration_Info_UEnum_EFSR2HistoryFormat.OuterSingleton)
		{
			Z_Registration_Info_UEnum_EFSR2HistoryFormat.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2HistoryFormat, (UObject*)Z_Construct_UPackage__Script_FSR2TemporalUpscaling(), TEXT("EFSR2HistoryFormat"));
		}
		return Z_Registration_Info_UEnum_EFSR2HistoryFormat.OuterSingleton;
	}
	template<> FSR2TEMPORALUPSCALING_API UEnum* StaticEnum<EFSR2HistoryFormat>()
	{
		return EFSR2HistoryFormat_StaticEnum();
	}
	struct Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2HistoryFormat_Statics
	{
		static const UECodeGen_Private::FEnumeratorParam Enumerators[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[];
#endif
		static const UECodeGen_Private::FEnumParams EnumParams;
	};
	const UECodeGen_Private::FEnumeratorParam Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2HistoryFormat_Statics::Enumerators[] = {
		{ "EFSR2HistoryFormat::FloatRGBA", (int64)EFSR2HistoryFormat::FloatRGBA },
		{ "EFSR2HistoryFormat::FloatR11G11B10", (int64)EFSR2HistoryFormat::FloatR11G11B10 },
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2HistoryFormat_Statics::Enum_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "//-------------------------------------------------------------------------------------\n// The support texture formats for the FSR2 history data.\n//-------------------------------------------------------------------------------------\n" },
#endif
		{ "FloatR11G11B10.DisplayName", "PF_FloatR11G11B10" },
		{ "FloatR11G11B10.Name", "EFSR2HistoryFormat::FloatR11G11B10" },
		{ "FloatRGBA.DisplayName", "PF_FloatRGBA" },
		{ "FloatRGBA.Name", "EFSR2HistoryFormat::FloatRGBA" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "The support texture formats for the FSR2 history data." },
#endif
	};
#endif
	const UECodeGen_Private::FEnumParams Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2HistoryFormat_Statics::EnumParams = {
		(UObject*(*)())Z_Construct_UPackage__Script_FSR2TemporalUpscaling,
		nullptr,
		"EFSR2HistoryFormat",
		"EFSR2HistoryFormat",
		Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2HistoryFormat_Statics::Enumerators,
		RF_Public|RF_Transient|RF_MarkAsNative,
		UE_ARRAY_COUNT(Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2HistoryFormat_Statics::Enumerators),
		EEnumFlags::None,
		(uint8)UEnum::ECppForm::EnumClass,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2HistoryFormat_Statics::Enum_MetaDataParams), Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2HistoryFormat_Statics::Enum_MetaDataParams)
	};
	UEnum* Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2HistoryFormat()
	{
		if (!Z_Registration_Info_UEnum_EFSR2HistoryFormat.InnerSingleton)
		{
			UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_EFSR2HistoryFormat.InnerSingleton, Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2HistoryFormat_Statics::EnumParams);
		}
		return Z_Registration_Info_UEnum_EFSR2HistoryFormat.InnerSingleton;
	}
	static FEnumRegistrationInfo Z_Registration_Info_UEnum_EFSR2DeDitherMode;
	static UEnum* EFSR2DeDitherMode_StaticEnum()
	{
		if (!Z_Registration_Info_UEnum_EFSR2DeDitherMode.OuterSingleton)
		{
			Z_Registration_Info_UEnum_EFSR2DeDitherMode.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2DeDitherMode, (UObject*)Z_Construct_UPackage__Script_FSR2TemporalUpscaling(), TEXT("EFSR2DeDitherMode"));
		}
		return Z_Registration_Info_UEnum_EFSR2DeDitherMode.OuterSingleton;
	}
	template<> FSR2TEMPORALUPSCALING_API UEnum* StaticEnum<EFSR2DeDitherMode>()
	{
		return EFSR2DeDitherMode_StaticEnum();
	}
	struct Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2DeDitherMode_Statics
	{
		static const UECodeGen_Private::FEnumeratorParam Enumerators[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[];
#endif
		static const UECodeGen_Private::FEnumParams EnumParams;
	};
	const UECodeGen_Private::FEnumeratorParam Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2DeDitherMode_Statics::Enumerators[] = {
		{ "EFSR2DeDitherMode::Off", (int64)EFSR2DeDitherMode::Off },
		{ "EFSR2DeDitherMode::Full", (int64)EFSR2DeDitherMode::Full },
		{ "EFSR2DeDitherMode::HairOnly", (int64)EFSR2DeDitherMode::HairOnly },
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2DeDitherMode_Statics::Enum_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "//-------------------------------------------------------------------------------------\n// The support texture formats for the FSR2 history data.\n//-------------------------------------------------------------------------------------\n" },
#endif
		{ "Full.DisplayName", "Full" },
		{ "Full.Name", "EFSR2DeDitherMode::Full" },
		{ "HairOnly.DisplayName", "Hair Only" },
		{ "HairOnly.Name", "EFSR2DeDitherMode::HairOnly" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
		{ "Off.DisplayName", "Off" },
		{ "Off.Name", "EFSR2DeDitherMode::Off" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "The support texture formats for the FSR2 history data." },
#endif
	};
#endif
	const UECodeGen_Private::FEnumParams Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2DeDitherMode_Statics::EnumParams = {
		(UObject*(*)())Z_Construct_UPackage__Script_FSR2TemporalUpscaling,
		nullptr,
		"EFSR2DeDitherMode",
		"EFSR2DeDitherMode",
		Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2DeDitherMode_Statics::Enumerators,
		RF_Public|RF_Transient|RF_MarkAsNative,
		UE_ARRAY_COUNT(Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2DeDitherMode_Statics::Enumerators),
		EEnumFlags::None,
		(uint8)UEnum::ECppForm::EnumClass,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2DeDitherMode_Statics::Enum_MetaDataParams), Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2DeDitherMode_Statics::Enum_MetaDataParams)
	};
	UEnum* Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2DeDitherMode()
	{
		if (!Z_Registration_Info_UEnum_EFSR2DeDitherMode.InnerSingleton)
		{
			UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_EFSR2DeDitherMode.InnerSingleton, Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2DeDitherMode_Statics::EnumParams);
		}
		return Z_Registration_Info_UEnum_EFSR2DeDitherMode.InnerSingleton;
	}
	static FEnumRegistrationInfo Z_Registration_Info_UEnum_EFSR2LandscapeHISMMode;
	static UEnum* EFSR2LandscapeHISMMode_StaticEnum()
	{
		if (!Z_Registration_Info_UEnum_EFSR2LandscapeHISMMode.OuterSingleton)
		{
			Z_Registration_Info_UEnum_EFSR2LandscapeHISMMode.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2LandscapeHISMMode, (UObject*)Z_Construct_UPackage__Script_FSR2TemporalUpscaling(), TEXT("EFSR2LandscapeHISMMode"));
		}
		return Z_Registration_Info_UEnum_EFSR2LandscapeHISMMode.OuterSingleton;
	}
	template<> FSR2TEMPORALUPSCALING_API UEnum* StaticEnum<EFSR2LandscapeHISMMode>()
	{
		return EFSR2LandscapeHISMMode_StaticEnum();
	}
	struct Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2LandscapeHISMMode_Statics
	{
		static const UECodeGen_Private::FEnumeratorParam Enumerators[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[];
#endif
		static const UECodeGen_Private::FEnumParams EnumParams;
	};
	const UECodeGen_Private::FEnumeratorParam Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2LandscapeHISMMode_Statics::Enumerators[] = {
		{ "EFSR2LandscapeHISMMode::Off", (int64)EFSR2LandscapeHISMMode::Off },
		{ "EFSR2LandscapeHISMMode::AllStatic", (int64)EFSR2LandscapeHISMMode::AllStatic },
		{ "EFSR2LandscapeHISMMode::StaticWPO", (int64)EFSR2LandscapeHISMMode::StaticWPO },
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2LandscapeHISMMode_Statics::Enum_MetaDataParams[] = {
		{ "AllStatic.DisplayName", "All Instances" },
		{ "AllStatic.Name", "EFSR2LandscapeHISMMode::AllStatic" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "//-------------------------------------------------------------------------------------\n// The modes for forcing Landscape Hierachical Instance Static Model to not be Static.\n//-------------------------------------------------------------------------------------\n" },
#endif
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
		{ "Off.DisplayName", "Off" },
		{ "Off.Name", "EFSR2LandscapeHISMMode::Off" },
		{ "StaticWPO.DisplayName", "Instances with World-Position-Offset" },
		{ "StaticWPO.Name", "EFSR2LandscapeHISMMode::StaticWPO" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "The modes for forcing Landscape Hierachical Instance Static Model to not be Static." },
#endif
	};
#endif
	const UECodeGen_Private::FEnumParams Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2LandscapeHISMMode_Statics::EnumParams = {
		(UObject*(*)())Z_Construct_UPackage__Script_FSR2TemporalUpscaling,
		nullptr,
		"EFSR2LandscapeHISMMode",
		"EFSR2LandscapeHISMMode",
		Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2LandscapeHISMMode_Statics::Enumerators,
		RF_Public|RF_Transient|RF_MarkAsNative,
		UE_ARRAY_COUNT(Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2LandscapeHISMMode_Statics::Enumerators),
		EEnumFlags::None,
		(uint8)UEnum::ECppForm::EnumClass,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2LandscapeHISMMode_Statics::Enum_MetaDataParams), Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2LandscapeHISMMode_Statics::Enum_MetaDataParams)
	};
	UEnum* Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2LandscapeHISMMode()
	{
		if (!Z_Registration_Info_UEnum_EFSR2LandscapeHISMMode.InnerSingleton)
		{
			UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_EFSR2LandscapeHISMMode.InnerSingleton, Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2LandscapeHISMMode_Statics::EnumParams);
		}
		return Z_Registration_Info_UEnum_EFSR2LandscapeHISMMode.InnerSingleton;
	}
	void UFSR2Settings::StaticRegisterNativesUFSR2Settings()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UFSR2Settings);
	UClass* Z_Construct_UClass_UFSR2Settings_NoRegister()
	{
		return UFSR2Settings::StaticClass();
	}
	struct Z_Construct_UClass_UFSR2Settings_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bEnabled_MetaData[];
#endif
		static void NewProp_bEnabled_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bEnabled;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_AutoExposure_MetaData[];
#endif
		static void NewProp_AutoExposure_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_AutoExposure;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_EnabledInEditorViewport_MetaData[];
#endif
		static void NewProp_EnabledInEditorViewport_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_EnabledInEditorViewport;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_UseSSRExperimentalDenoiser_MetaData[];
#endif
		static void NewProp_UseSSRExperimentalDenoiser_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_UseSSRExperimentalDenoiser;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_UseNativeDX12_MetaData[];
#endif
		static void NewProp_UseNativeDX12_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_UseNativeDX12;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_UseNativeVulkan_MetaData[];
#endif
		static void NewProp_UseNativeVulkan_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_UseNativeVulkan;
		static const UECodeGen_Private::FIntPropertyParams NewProp_QualityMode_Underlying;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_QualityMode_MetaData[];
#endif
		static const UECodeGen_Private::FEnumPropertyParams NewProp_QualityMode;
		static const UECodeGen_Private::FIntPropertyParams NewProp_HistoryFormat_Underlying;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_HistoryFormat_MetaData[];
#endif
		static const UECodeGen_Private::FEnumPropertyParams NewProp_HistoryFormat;
		static const UECodeGen_Private::FIntPropertyParams NewProp_DeDither_Underlying;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_DeDither_MetaData[];
#endif
		static const UECodeGen_Private::FEnumPropertyParams NewProp_DeDither;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_Sharpness_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_Sharpness;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_AdjustMipBias_MetaData[];
#endif
		static void NewProp_AdjustMipBias_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_AdjustMipBias;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ReactiveMask_MetaData[];
#endif
		static void NewProp_ReactiveMask_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_ReactiveMask;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ForceVertexDeformationOutputsVelocity_MetaData[];
#endif
		static void NewProp_ForceVertexDeformationOutputsVelocity_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_ForceVertexDeformationOutputsVelocity;
		static const UECodeGen_Private::FIntPropertyParams NewProp_ForceLandscapeHISMMobility_Underlying;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ForceLandscapeHISMMobility_MetaData[];
#endif
		static const UECodeGen_Private::FEnumPropertyParams NewProp_ForceLandscapeHISMMobility;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ReflectionScale_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_ReflectionScale;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ReflectionLuminanceBias_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_ReflectionLuminanceBias;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_RoughnessScale_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_RoughnessScale;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_RoughnessBias_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_RoughnessBias;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_RoughnessMaxDistance_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_RoughnessMaxDistance;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ReactiveMaskRoughnessForceMaxDistance_MetaData[];
#endif
		static void NewProp_ReactiveMaskRoughnessForceMaxDistance_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_ReactiveMaskRoughnessForceMaxDistance;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_TranslucencyBias_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_TranslucencyBias;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_TranslucencyLuminanceBias_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_TranslucencyLuminanceBias;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_TranslucencyMaxDistance_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_TranslucencyMaxDistance;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ReactiveShadingModelID_MetaData[];
#endif
		static const UECodeGen_Private::FBytePropertyParams NewProp_ReactiveShadingModelID;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ForceReactiveMaterialValue_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_ForceReactiveMaterialValue;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ReactiveHistoryTranslucencyBias_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_ReactiveHistoryTranslucencyBias;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ReactiveHistoryTranslucencyLumaBias_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_ReactiveHistoryTranslucencyLumaBias;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UFSR2Settings_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UDeveloperSettings,
		(UObject* (*)())Z_Construct_UPackage__Script_FSR2TemporalUpscaling,
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::DependentSingletons) < 16);
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "//-------------------------------------------------------------------------------------\n// Settings for FSR2 exposed through the Editor UI.\n//-------------------------------------------------------------------------------------\n" },
#endif
		{ "DisplayName", "FidelityFX Super Resolution 2.2" },
		{ "IncludePath", "FSR2Settings.h" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Settings for FSR2 exposed through the Editor UI." },
#endif
	};
#endif
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_bEnabled_MetaData[] = {
		{ "Category", "General Settings" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.Enabled" },
		{ "DisplayName", "Enabled" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
	};
#endif
	void Z_Construct_UClass_UFSR2Settings_Statics::NewProp_bEnabled_SetBit(void* Obj)
	{
		((UFSR2Settings*)Obj)->bEnabled = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_bEnabled = { "bEnabled", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UFSR2Settings), &Z_Construct_UClass_UFSR2Settings_Statics::NewProp_bEnabled_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_bEnabled_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_bEnabled_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_AutoExposure_MetaData[] = {
		{ "Category", "General Settings" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.AutoExposure" },
		{ "DisplayName", "Auto Exposure" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Enable to use FSR2's own auto-exposure, otherwise the engine's auto-exposure value is used." },
#endif
	};
#endif
	void Z_Construct_UClass_UFSR2Settings_Statics::NewProp_AutoExposure_SetBit(void* Obj)
	{
		((UFSR2Settings*)Obj)->AutoExposure = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_AutoExposure = { "AutoExposure", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UFSR2Settings), &Z_Construct_UClass_UFSR2Settings_Statics::NewProp_AutoExposure_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_AutoExposure_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_AutoExposure_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_EnabledInEditorViewport_MetaData[] = {
		{ "Category", "General Settings" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.EnabledInEditorViewport" },
		{ "DisplayName", "Enabled in Editor Viewport" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "When enabled use FSR2 by default in the Editor viewports." },
#endif
	};
#endif
	void Z_Construct_UClass_UFSR2Settings_Statics::NewProp_EnabledInEditorViewport_SetBit(void* Obj)
	{
		((UFSR2Settings*)Obj)->EnabledInEditorViewport = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_EnabledInEditorViewport = { "EnabledInEditorViewport", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UFSR2Settings), &Z_Construct_UClass_UFSR2Settings_Statics::NewProp_EnabledInEditorViewport_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_EnabledInEditorViewport_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_EnabledInEditorViewport_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_UseSSRExperimentalDenoiser_MetaData[] = {
		{ "Category", "General Settings" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.UseSSRExperimentalDenoiser" },
		{ "DisplayName", "Use SSR Experimental Denoiser" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Set to 1 to use r.SSR.ExperimentalDenoiser when FSR2 is enabled. This is required when r.FidelityFX.FSR2.CreateReactiveMask is enabled as the FSR2 plugin sets r.SSR.ExperimentalDenoiser to 1 in order to capture reflection data to generate the reactive mask." },
#endif
	};
#endif
	void Z_Construct_UClass_UFSR2Settings_Statics::NewProp_UseSSRExperimentalDenoiser_SetBit(void* Obj)
	{
		((UFSR2Settings*)Obj)->UseSSRExperimentalDenoiser = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_UseSSRExperimentalDenoiser = { "UseSSRExperimentalDenoiser", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UFSR2Settings), &Z_Construct_UClass_UFSR2Settings_Statics::NewProp_UseSSRExperimentalDenoiser_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_UseSSRExperimentalDenoiser_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_UseSSRExperimentalDenoiser_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_UseNativeDX12_MetaData[] = {
		{ "Category", "Backend Settings" },
		{ "ConfigRestartRequired", "TRUE" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.UseNativeDX12" },
		{ "DisplayName", "Native DX12" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Whather to use the optimised native DX12 backend for FSR2 rather than the default RHI backend." },
#endif
	};
#endif
	void Z_Construct_UClass_UFSR2Settings_Statics::NewProp_UseNativeDX12_SetBit(void* Obj)
	{
		((UFSR2Settings*)Obj)->UseNativeDX12 = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_UseNativeDX12 = { "UseNativeDX12", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UFSR2Settings), &Z_Construct_UClass_UFSR2Settings_Statics::NewProp_UseNativeDX12_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_UseNativeDX12_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_UseNativeDX12_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_UseNativeVulkan_MetaData[] = {
		{ "Category", "Backend Settings" },
		{ "ConfigRestartRequired", "TRUE" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.UseNativeVulkan" },
		{ "DisplayName", "Native Vulkan" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Whather to use the optimised native Vulkan backend for FSR2 rather than the default RHI backend." },
#endif
	};
#endif
	void Z_Construct_UClass_UFSR2Settings_Statics::NewProp_UseNativeVulkan_SetBit(void* Obj)
	{
		((UFSR2Settings*)Obj)->UseNativeVulkan = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_UseNativeVulkan = { "UseNativeVulkan", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UFSR2Settings), &Z_Construct_UClass_UFSR2Settings_Statics::NewProp_UseNativeVulkan_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_UseNativeVulkan_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_UseNativeVulkan_MetaData) };
	const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_QualityMode_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, METADATA_PARAMS(0, nullptr) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_QualityMode_MetaData[] = {
		{ "Category", "Quality Settings" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.QualityMode" },
		{ "DisplayName", "Quality Mode" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Selects the default quality mode to be used when upscaling with FSR2." },
#endif
	};
#endif
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_QualityMode = { "QualityMode", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UFSR2Settings, QualityMode), Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2QualityMode, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_QualityMode_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_QualityMode_MetaData) }; // 2030601457
	const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_HistoryFormat_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, METADATA_PARAMS(0, nullptr) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_HistoryFormat_MetaData[] = {
		{ "Category", "Quality Settings" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.HistoryFormat" },
		{ "DisplayName", "History Format" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Selects the bit-depth for the FSR2 history texture format, defaults to PF_FloatRGBA but can be set to PF_FloatR11G11B10 to reduce bandwidth at the expense of quality." },
#endif
	};
#endif
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_HistoryFormat = { "HistoryFormat", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UFSR2Settings, HistoryFormat), Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2HistoryFormat, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_HistoryFormat_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_HistoryFormat_MetaData) }; // 1095505965
	const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_DeDither_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, METADATA_PARAMS(0, nullptr) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_DeDither_MetaData[] = {
		{ "Category", "Quality Settings" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.DeDither" },
		{ "DisplayName", "De-Dither Rendering" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Enable an extra pass to de-dither rendering before handing over to FSR2 to avoid over-thinning, defaults to Off but can be set to Full for all pixels or to Hair Only for just around Hair (requires Deffered Renderer)." },
#endif
	};
#endif
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_DeDither = { "DeDither", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UFSR2Settings, DeDither), Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2DeDitherMode, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_DeDither_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_DeDither_MetaData) }; // 291007703
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_Sharpness_MetaData[] = {
		{ "Category", "Quality Settings" },
		{ "ClampMax", "1" },
		{ "ClampMin", "0" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.Sharpness" },
		{ "DisplayName", "Sharpness" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "When greater than 0.0 this enables Robust Contrast Adaptive Sharpening Filter to sharpen the output image." },
#endif
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_Sharpness = { "Sharpness", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UFSR2Settings, Sharpness), METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_Sharpness_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_Sharpness_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_AdjustMipBias_MetaData[] = {
		{ "Category", "Quality Settings" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.AdjustMipBias" },
		{ "DisplayName", "Adjust Mip Bias & Offset" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Applies negative MipBias to material textures, improving results." },
#endif
	};
#endif
	void Z_Construct_UClass_UFSR2Settings_Statics::NewProp_AdjustMipBias_SetBit(void* Obj)
	{
		((UFSR2Settings*)Obj)->AdjustMipBias = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_AdjustMipBias = { "AdjustMipBias", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UFSR2Settings), &Z_Construct_UClass_UFSR2Settings_Statics::NewProp_AdjustMipBias_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_AdjustMipBias_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_AdjustMipBias_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveMask_MetaData[] = {
		{ "Category", "Quality Settings" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.CreateReactiveMask" },
		{ "DisplayName", "Reactive Mask" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Enable to generate a mask from the SceneColor, GBuffer, SeparateTranslucency & ScreenspaceReflections that determines how reactive each pixel should be." },
#endif
	};
#endif
	void Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveMask_SetBit(void* Obj)
	{
		((UFSR2Settings*)Obj)->ReactiveMask = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveMask = { "ReactiveMask", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UFSR2Settings), &Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveMask_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveMask_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveMask_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ForceVertexDeformationOutputsVelocity_MetaData[] = {
		{ "Category", "Quality Settings" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.ForceVertexDeformationOutputsVelocity" },
		{ "DisplayName", "Force Vertex Deformation To Output Velocity" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Force enables materials with World Position Offset and/or World Displacement to output velocities during velocity pass even when the actor has not moved." },
#endif
	};
#endif
	void Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ForceVertexDeformationOutputsVelocity_SetBit(void* Obj)
	{
		((UFSR2Settings*)Obj)->ForceVertexDeformationOutputsVelocity = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ForceVertexDeformationOutputsVelocity = { "ForceVertexDeformationOutputsVelocity", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UFSR2Settings), &Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ForceVertexDeformationOutputsVelocity_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ForceVertexDeformationOutputsVelocity_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ForceVertexDeformationOutputsVelocity_MetaData) };
	const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ForceLandscapeHISMMobility_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, METADATA_PARAMS(0, nullptr) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ForceLandscapeHISMMobility_MetaData[] = {
		{ "Category", "Quality Settings" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.ForceLandscapeHISMMobility" },
		{ "DisplayName", "Force Landscape HISM Mobility" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Allow FSR2 to force the mobility of Landscape actors Hierarchical Instance Static Mesh components that use World-Position-Offset materials so they render valid velocities.\nSetting 'All Instances' is faster on the CPU, 'Instances with World-Position-Offset' is faster on the GPU." },
#endif
	};
#endif
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ForceLandscapeHISMMobility = { "ForceLandscapeHISMMobility", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UFSR2Settings, ForceLandscapeHISMMobility), Z_Construct_UEnum_FSR2TemporalUpscaling_EFSR2LandscapeHISMMode, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ForceLandscapeHISMMobility_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ForceLandscapeHISMMobility_MetaData) }; // 1844267087
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReflectionScale_MetaData[] = {
		{ "Category", "Reactive Mask Settings" },
		{ "ClampMax", "1" },
		{ "ClampMin", "0" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.ReactiveMaskReflectionScale" },
		{ "DisplayName", "Reflection Scale" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Scales the Unreal engine reflection contribution to the reactive mask, which can be used to control the amount of aliasing on reflective surfaces." },
#endif
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReflectionScale = { "ReflectionScale", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UFSR2Settings, ReflectionScale), METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReflectionScale_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReflectionScale_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReflectionLuminanceBias_MetaData[] = {
		{ "Category", "Reactive Mask Settings" },
		{ "ClampMax", "1" },
		{ "ClampMin", "0" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.ReactiveMaskReflectionLumaBias" },
		{ "DisplayName", "Reflection Luminance Bias" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Biases the reactive mask by the luminance of the reflection. Use to balance aliasing against ghosting on brightly lit reflective surfaces." },
#endif
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReflectionLuminanceBias = { "ReflectionLuminanceBias", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UFSR2Settings, ReflectionLuminanceBias), METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReflectionLuminanceBias_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReflectionLuminanceBias_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_RoughnessScale_MetaData[] = {
		{ "Category", "Reactive Mask Settings" },
		{ "ClampMax", "1" },
		{ "ClampMin", "0" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.ReactiveMaskRoughnessScale" },
		{ "DisplayName", "Roughness Scale" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Scales the GBuffer roughness to provide a fallback value for the reactive mask when screenspace & planar reflections are disabled or don't affect a pixel." },
#endif
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_RoughnessScale = { "RoughnessScale", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UFSR2Settings, RoughnessScale), METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_RoughnessScale_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_RoughnessScale_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_RoughnessBias_MetaData[] = {
		{ "Category", "Reactive Mask Settings" },
		{ "ClampMax", "1" },
		{ "ClampMin", "0" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.ReactiveMaskRoughnessBias" },
		{ "DisplayName", "Roughness Bias" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Biases the reactive mask value when screenspace/planar reflections are weak with the GBuffer roughness to account for reflection environment captures." },
#endif
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_RoughnessBias = { "RoughnessBias", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UFSR2Settings, RoughnessBias), METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_RoughnessBias_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_RoughnessBias_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_RoughnessMaxDistance_MetaData[] = {
		{ "Category", "Reactive Mask Settings" },
		{ "ClampMin", "0" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.ReactiveMaskRoughnessMaxDistance" },
		{ "DisplayName", "Roughness Max Distance" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Maximum distance in world units for using material roughness to contribute to the reactive mask, the maximum of this value and View.FurthestReflectionCaptureDistance will be used." },
#endif
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_RoughnessMaxDistance = { "RoughnessMaxDistance", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UFSR2Settings, RoughnessMaxDistance), METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_RoughnessMaxDistance_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_RoughnessMaxDistance_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveMaskRoughnessForceMaxDistance_MetaData[] = {
		{ "Category", "Reactive Mask Settings" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.ReactiveMaskRoughnessForceMaxDistance" },
		{ "DisplayName", "Force Roughness Max Distance" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Enable to force the maximum distance in world units for using material roughness to contribute to the reactive mask rather than using View.FurthestReflectionCaptureDistance." },
#endif
	};
#endif
	void Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveMaskRoughnessForceMaxDistance_SetBit(void* Obj)
	{
		((UFSR2Settings*)Obj)->ReactiveMaskRoughnessForceMaxDistance = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveMaskRoughnessForceMaxDistance = { "ReactiveMaskRoughnessForceMaxDistance", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UFSR2Settings), &Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveMaskRoughnessForceMaxDistance_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveMaskRoughnessForceMaxDistance_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveMaskRoughnessForceMaxDistance_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_TranslucencyBias_MetaData[] = {
		{ "Category", "Reactive Mask Settings" },
		{ "ClampMax", "1" },
		{ "ClampMin", "0" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.ReactiveMaskTranslucencyBias" },
		{ "DisplayName", "Translucency Bias" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Scales how much contribution translucency makes to the reactive mask. Higher values will make translucent materials less reactive which can reduce smearing." },
#endif
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_TranslucencyBias = { "TranslucencyBias", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UFSR2Settings, TranslucencyBias), METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_TranslucencyBias_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_TranslucencyBias_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_TranslucencyLuminanceBias_MetaData[] = {
		{ "Category", "Reactive Mask Settings" },
		{ "ClampMax", "1" },
		{ "ClampMin", "0" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.ReactiveMaskTranslucencyLumaBias" },
		{ "DisplayName", "Translucency Luminance Bias" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Biases the translucency contribution to the reactive mask by the luminance of the transparency." },
#endif
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_TranslucencyLuminanceBias = { "TranslucencyLuminanceBias", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UFSR2Settings, TranslucencyLuminanceBias), METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_TranslucencyLuminanceBias_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_TranslucencyLuminanceBias_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_TranslucencyMaxDistance_MetaData[] = {
		{ "Category", "Reactive Mask Settings" },
		{ "ClampMin", "0" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.ReactiveMaskTranslucencyMaxDistance" },
		{ "DisplayName", "Translucency Max Distance" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Maximum distance in world units for using translucency to contribute to the reactive mask. This is another way to remove sky-boxes and other back-planes from the reactive mask, at the expense of nearer translucency not being reactive." },
#endif
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_TranslucencyMaxDistance = { "TranslucencyMaxDistance", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UFSR2Settings, TranslucencyMaxDistance), METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_TranslucencyMaxDistance_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_TranslucencyMaxDistance_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveShadingModelID_MetaData[] = {
		{ "Category", "Reactive Mask Settings" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.ReactiveMaskReactiveShadingModelID" },
		{ "DisplayName", "Reactive Shading Model" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Treat the specified shading model as reactive, taking the CustomData0.x value as the reactive value to write into the mask. Default is MSM_NUM (Off)." },
#endif
	};
#endif
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveShadingModelID = { "ReactiveShadingModelID", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UFSR2Settings, ReactiveShadingModelID), Z_Construct_UEnum_Engine_EMaterialShadingModel, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveShadingModelID_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveShadingModelID_MetaData) }; // 3101967338
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ForceReactiveMaterialValue_MetaData[] = {
		{ "Category", "Reactive Mask Settings" },
		{ "ClampMax", "1" },
		{ "ClampMin", "0" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.ReactiveMaskForceReactiveMaterialValue" },
		{ "DisplayName", "Force value for Reactive Shading Model" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Force the reactive mask value for Reactive Shading Model materials, when > 0 this value can be used to override the value supplied in the Material Graph." },
#endif
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ForceReactiveMaterialValue = { "ForceReactiveMaterialValue", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UFSR2Settings, ForceReactiveMaterialValue), METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ForceReactiveMaterialValue_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ForceReactiveMaterialValue_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveHistoryTranslucencyBias_MetaData[] = {
		{ "Category", "Reactive Mask Settings" },
		{ "ClampMax", "1" },
		{ "ClampMin", "0" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.ReactiveHistoryTranslucencyBias" },
		{ "DisplayName", "Translucency Bias" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Scales how much contribution translucency makes to suppress history via the reactive mask. Higher values will make translucent materials less reactive which can reduce smearing." },
#endif
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveHistoryTranslucencyBias = { "ReactiveHistoryTranslucencyBias", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UFSR2Settings, ReactiveHistoryTranslucencyBias), METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveHistoryTranslucencyBias_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveHistoryTranslucencyBias_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveHistoryTranslucencyLumaBias_MetaData[] = {
		{ "Category", "Reactive Mask Settings" },
		{ "ClampMax", "1" },
		{ "ClampMin", "0" },
		{ "ConsoleVariable", "r.FidelityFX.FSR2.ReactiveHistoryTranslucencyLumaBias" },
		{ "DisplayName", "Translucency Luminance Bias" },
		{ "ModuleRelativePath", "Public/FSR2Settings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Biases the translucency contribution to suppress history via the reactive mask by the luminance of the transparency." },
#endif
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveHistoryTranslucencyLumaBias = { "ReactiveHistoryTranslucencyLumaBias", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UFSR2Settings, ReactiveHistoryTranslucencyLumaBias), METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveHistoryTranslucencyLumaBias_MetaData), Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveHistoryTranslucencyLumaBias_MetaData) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UFSR2Settings_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_bEnabled,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_AutoExposure,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_EnabledInEditorViewport,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_UseSSRExperimentalDenoiser,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_UseNativeDX12,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_UseNativeVulkan,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_QualityMode_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_QualityMode,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_HistoryFormat_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_HistoryFormat,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_DeDither_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_DeDither,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_Sharpness,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_AdjustMipBias,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveMask,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ForceVertexDeformationOutputsVelocity,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ForceLandscapeHISMMobility_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ForceLandscapeHISMMobility,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReflectionScale,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReflectionLuminanceBias,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_RoughnessScale,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_RoughnessBias,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_RoughnessMaxDistance,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveMaskRoughnessForceMaxDistance,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_TranslucencyBias,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_TranslucencyLuminanceBias,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_TranslucencyMaxDistance,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveShadingModelID,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ForceReactiveMaterialValue,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveHistoryTranslucencyBias,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2Settings_Statics::NewProp_ReactiveHistoryTranslucencyLumaBias,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UFSR2Settings_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UFSR2Settings>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UFSR2Settings_Statics::ClassParams = {
		&UFSR2Settings::StaticClass,
		"Engine",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UFSR2Settings_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::PropPointers),
		0,
		0x001000A6u,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::Class_MetaDataParams), Z_Construct_UClass_UFSR2Settings_Statics::Class_MetaDataParams)
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2Settings_Statics::PropPointers) < 2048);
	UClass* Z_Construct_UClass_UFSR2Settings()
	{
		if (!Z_Registration_Info_UClass_UFSR2Settings.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UFSR2Settings.OuterSingleton, Z_Construct_UClass_UFSR2Settings_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UFSR2Settings.OuterSingleton;
	}
	template<> FSR2TEMPORALUPSCALING_API UClass* StaticClass<UFSR2Settings>()
	{
		return UFSR2Settings::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UFSR2Settings);
	UFSR2Settings::~UFSR2Settings() {}
	struct Z_CompiledInDeferFile_FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2_Source_FSR2TemporalUpscaling_Public_FSR2Settings_h_Statics
	{
		static const FEnumRegisterCompiledInInfo EnumInfo[];
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FEnumRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2_Source_FSR2TemporalUpscaling_Public_FSR2Settings_h_Statics::EnumInfo[] = {
		{ EFSR2QualityMode_StaticEnum, TEXT("EFSR2QualityMode"), &Z_Registration_Info_UEnum_EFSR2QualityMode, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 2030601457U) },
		{ EFSR2HistoryFormat_StaticEnum, TEXT("EFSR2HistoryFormat"), &Z_Registration_Info_UEnum_EFSR2HistoryFormat, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 1095505965U) },
		{ EFSR2DeDitherMode_StaticEnum, TEXT("EFSR2DeDitherMode"), &Z_Registration_Info_UEnum_EFSR2DeDitherMode, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 291007703U) },
		{ EFSR2LandscapeHISMMode_StaticEnum, TEXT("EFSR2LandscapeHISMMode"), &Z_Registration_Info_UEnum_EFSR2LandscapeHISMMode, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 1844267087U) },
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2_Source_FSR2TemporalUpscaling_Public_FSR2Settings_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UFSR2Settings, UFSR2Settings::StaticClass, TEXT("UFSR2Settings"), &Z_Registration_Info_UClass_UFSR2Settings, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UFSR2Settings), 2668833900U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2_Source_FSR2TemporalUpscaling_Public_FSR2Settings_h_3982654666(TEXT("/Script/FSR2TemporalUpscaling"),
		Z_CompiledInDeferFile_FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2_Source_FSR2TemporalUpscaling_Public_FSR2Settings_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2_Source_FSR2TemporalUpscaling_Public_FSR2Settings_h_Statics::ClassInfo),
		nullptr, 0,
		Z_CompiledInDeferFile_FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2_Source_FSR2TemporalUpscaling_Public_FSR2Settings_h_Statics::EnumInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2_Source_FSR2TemporalUpscaling_Public_FSR2Settings_h_Statics::EnumInfo));
PRAGMA_ENABLE_DEPRECATION_WARNINGS
