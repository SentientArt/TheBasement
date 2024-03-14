// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Public/FSR2MoviePipelineSettings.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeFSR2MoviePipelineSettings() {}
// Cross Module References
	FSR2MOVIERENDERPIPELINE_API UClass* Z_Construct_UClass_UFSR2MoviePipelineSettings();
	FSR2MOVIERENDERPIPELINE_API UClass* Z_Construct_UClass_UFSR2MoviePipelineSettings_NoRegister();
	FSR2MOVIERENDERPIPELINE_API UEnum* Z_Construct_UEnum_FSR2MovieRenderPipeline_EFSR2MoviePipelineQuality();
	MOVIERENDERPIPELINECORE_API UClass* Z_Construct_UClass_UMoviePipelineViewFamilySetting();
	UPackage* Z_Construct_UPackage__Script_FSR2MovieRenderPipeline();
// End Cross Module References
	static FEnumRegistrationInfo Z_Registration_Info_UEnum_EFSR2MoviePipelineQuality;
	static UEnum* EFSR2MoviePipelineQuality_StaticEnum()
	{
		if (!Z_Registration_Info_UEnum_EFSR2MoviePipelineQuality.OuterSingleton)
		{
			Z_Registration_Info_UEnum_EFSR2MoviePipelineQuality.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_FSR2MovieRenderPipeline_EFSR2MoviePipelineQuality, (UObject*)Z_Construct_UPackage__Script_FSR2MovieRenderPipeline(), TEXT("EFSR2MoviePipelineQuality"));
		}
		return Z_Registration_Info_UEnum_EFSR2MoviePipelineQuality.OuterSingleton;
	}
	template<> FSR2MOVIERENDERPIPELINE_API UEnum* StaticEnum<EFSR2MoviePipelineQuality>()
	{
		return EFSR2MoviePipelineQuality_StaticEnum();
	}
	struct Z_Construct_UEnum_FSR2MovieRenderPipeline_EFSR2MoviePipelineQuality_Statics
	{
		static const UECodeGen_Private::FEnumeratorParam Enumerators[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[];
#endif
		static const UECodeGen_Private::FEnumParams EnumParams;
	};
	const UECodeGen_Private::FEnumeratorParam Z_Construct_UEnum_FSR2MovieRenderPipeline_EFSR2MoviePipelineQuality_Statics::Enumerators[] = {
		{ "EFSR2MoviePipelineQuality::Unused", (int64)EFSR2MoviePipelineQuality::Unused },
		{ "EFSR2MoviePipelineQuality::Quality", (int64)EFSR2MoviePipelineQuality::Quality },
		{ "EFSR2MoviePipelineQuality::Balanced", (int64)EFSR2MoviePipelineQuality::Balanced },
		{ "EFSR2MoviePipelineQuality::Performance", (int64)EFSR2MoviePipelineQuality::Performance },
		{ "EFSR2MoviePipelineQuality::UltraPerformance", (int64)EFSR2MoviePipelineQuality::UltraPerformance },
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UEnum_FSR2MovieRenderPipeline_EFSR2MoviePipelineQuality_Statics::Enum_MetaDataParams[] = {
		{ "Balanced.DisplayName", "Balanced" },
		{ "Balanced.Name", "EFSR2MoviePipelineQuality::Balanced" },
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/FSR2MoviePipelineSettings.h" },
		{ "Performance.DisplayName", "Performance" },
		{ "Performance.Name", "EFSR2MoviePipelineQuality::Performance" },
		{ "Quality.DisplayName", "Quality" },
		{ "Quality.Name", "EFSR2MoviePipelineQuality::Quality" },
		{ "UltraPerformance.DisplayName", "UltraPerformance" },
		{ "UltraPerformance.Name", "EFSR2MoviePipelineQuality::UltraPerformance" },
		{ "Unused.Hidden", "" },
		{ "Unused.Name", "EFSR2MoviePipelineQuality::Unused" },
	};
#endif
	const UECodeGen_Private::FEnumParams Z_Construct_UEnum_FSR2MovieRenderPipeline_EFSR2MoviePipelineQuality_Statics::EnumParams = {
		(UObject*(*)())Z_Construct_UPackage__Script_FSR2MovieRenderPipeline,
		nullptr,
		"EFSR2MoviePipelineQuality",
		"EFSR2MoviePipelineQuality",
		Z_Construct_UEnum_FSR2MovieRenderPipeline_EFSR2MoviePipelineQuality_Statics::Enumerators,
		RF_Public|RF_Transient|RF_MarkAsNative,
		UE_ARRAY_COUNT(Z_Construct_UEnum_FSR2MovieRenderPipeline_EFSR2MoviePipelineQuality_Statics::Enumerators),
		EEnumFlags::None,
		(uint8)UEnum::ECppForm::EnumClass,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UEnum_FSR2MovieRenderPipeline_EFSR2MoviePipelineQuality_Statics::Enum_MetaDataParams), Z_Construct_UEnum_FSR2MovieRenderPipeline_EFSR2MoviePipelineQuality_Statics::Enum_MetaDataParams)
	};
	UEnum* Z_Construct_UEnum_FSR2MovieRenderPipeline_EFSR2MoviePipelineQuality()
	{
		if (!Z_Registration_Info_UEnum_EFSR2MoviePipelineQuality.InnerSingleton)
		{
			UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_EFSR2MoviePipelineQuality.InnerSingleton, Z_Construct_UEnum_FSR2MovieRenderPipeline_EFSR2MoviePipelineQuality_Statics::EnumParams);
		}
		return Z_Registration_Info_UEnum_EFSR2MoviePipelineQuality.InnerSingleton;
	}
	void UFSR2MoviePipelineSettings::StaticRegisterNativesUFSR2MoviePipelineSettings()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UFSR2MoviePipelineSettings);
	UClass* Z_Construct_UClass_UFSR2MoviePipelineSettings_NoRegister()
	{
		return UFSR2MoviePipelineSettings::StaticClass();
	}
	struct Z_Construct_UClass_UFSR2MoviePipelineSettings_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const UECodeGen_Private::FBytePropertyParams NewProp_FSR2Quality_Underlying;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_FSR2Quality_MetaData[];
#endif
		static const UECodeGen_Private::FEnumPropertyParams NewProp_FSR2Quality;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UFSR2MoviePipelineSettings_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UMoviePipelineViewFamilySetting,
		(UObject* (*)())Z_Construct_UPackage__Script_FSR2MovieRenderPipeline,
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2MoviePipelineSettings_Statics::DependentSingletons) < 16);
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2MoviePipelineSettings_Statics::Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "DisplayName", "FidelityFX Super Resolution 2.2 Settings" },
		{ "IncludePath", "FSR2MoviePipelineSettings.h" },
		{ "ModuleRelativePath", "Public/FSR2MoviePipelineSettings.h" },
	};
#endif
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_UFSR2MoviePipelineSettings_Statics::NewProp_FSR2Quality_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFSR2MoviePipelineSettings_Statics::NewProp_FSR2Quality_MetaData[] = {
		{ "Category", "Settings" },
		{ "DisplayName", "Quality Mode" },
		{ "ModuleRelativePath", "Public/FSR2MoviePipelineSettings.h" },
	};
#endif
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UClass_UFSR2MoviePipelineSettings_Statics::NewProp_FSR2Quality = { "FSR2Quality", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UFSR2MoviePipelineSettings, FSR2Quality), Z_Construct_UEnum_FSR2MovieRenderPipeline_EFSR2MoviePipelineQuality, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2MoviePipelineSettings_Statics::NewProp_FSR2Quality_MetaData), Z_Construct_UClass_UFSR2MoviePipelineSettings_Statics::NewProp_FSR2Quality_MetaData) }; // 3496554376
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UFSR2MoviePipelineSettings_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2MoviePipelineSettings_Statics::NewProp_FSR2Quality_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFSR2MoviePipelineSettings_Statics::NewProp_FSR2Quality,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UFSR2MoviePipelineSettings_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UFSR2MoviePipelineSettings>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UFSR2MoviePipelineSettings_Statics::ClassParams = {
		&UFSR2MoviePipelineSettings::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UFSR2MoviePipelineSettings_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2MoviePipelineSettings_Statics::PropPointers),
		0,
		0x001000A0u,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2MoviePipelineSettings_Statics::Class_MetaDataParams), Z_Construct_UClass_UFSR2MoviePipelineSettings_Statics::Class_MetaDataParams)
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UFSR2MoviePipelineSettings_Statics::PropPointers) < 2048);
	UClass* Z_Construct_UClass_UFSR2MoviePipelineSettings()
	{
		if (!Z_Registration_Info_UClass_UFSR2MoviePipelineSettings.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UFSR2MoviePipelineSettings.OuterSingleton, Z_Construct_UClass_UFSR2MoviePipelineSettings_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UFSR2MoviePipelineSettings.OuterSingleton;
	}
	template<> FSR2MOVIERENDERPIPELINE_API UClass* StaticClass<UFSR2MoviePipelineSettings>()
	{
		return UFSR2MoviePipelineSettings::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UFSR2MoviePipelineSettings);
	UFSR2MoviePipelineSettings::~UFSR2MoviePipelineSettings() {}
	struct Z_CompiledInDeferFile_FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2MovieRenderPipeline_Source_Public_FSR2MoviePipelineSettings_h_Statics
	{
		static const FEnumRegisterCompiledInInfo EnumInfo[];
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FEnumRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2MovieRenderPipeline_Source_Public_FSR2MoviePipelineSettings_h_Statics::EnumInfo[] = {
		{ EFSR2MoviePipelineQuality_StaticEnum, TEXT("EFSR2MoviePipelineQuality"), &Z_Registration_Info_UEnum_EFSR2MoviePipelineQuality, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 3496554376U) },
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2MovieRenderPipeline_Source_Public_FSR2MoviePipelineSettings_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UFSR2MoviePipelineSettings, UFSR2MoviePipelineSettings::StaticClass, TEXT("UFSR2MoviePipelineSettings"), &Z_Registration_Info_UClass_UFSR2MoviePipelineSettings, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UFSR2MoviePipelineSettings), 3537832684U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2MovieRenderPipeline_Source_Public_FSR2MoviePipelineSettings_h_2602734036(TEXT("/Script/FSR2MovieRenderPipeline"),
		Z_CompiledInDeferFile_FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2MovieRenderPipeline_Source_Public_FSR2MoviePipelineSettings_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2MovieRenderPipeline_Source_Public_FSR2MoviePipelineSettings_h_Statics::ClassInfo),
		nullptr, 0,
		Z_CompiledInDeferFile_FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2MovieRenderPipeline_Source_Public_FSR2MoviePipelineSettings_h_Statics::EnumInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2MovieRenderPipeline_Source_Public_FSR2MoviePipelineSettings_h_Statics::EnumInfo));
PRAGMA_ENABLE_DEPRECATION_WARNINGS
