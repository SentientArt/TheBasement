// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "FSR2MoviePipelineSettings.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef FSR2MOVIERENDERPIPELINE_FSR2MoviePipelineSettings_generated_h
#error "FSR2MoviePipelineSettings.generated.h already included, missing '#pragma once' in FSR2MoviePipelineSettings.h"
#endif
#define FSR2MOVIERENDERPIPELINE_FSR2MoviePipelineSettings_generated_h

#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2MovieRenderPipeline_Source_Public_FSR2MoviePipelineSettings_h_41_SPARSE_DATA
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2MovieRenderPipeline_Source_Public_FSR2MoviePipelineSettings_h_41_SPARSE_DATA_PROPERTY_ACCESSORS
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2MovieRenderPipeline_Source_Public_FSR2MoviePipelineSettings_h_41_EDITOR_ONLY_SPARSE_DATA_PROPERTY_ACCESSORS
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2MovieRenderPipeline_Source_Public_FSR2MoviePipelineSettings_h_41_RPC_WRAPPERS_NO_PURE_DECLS
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2MovieRenderPipeline_Source_Public_FSR2MoviePipelineSettings_h_41_ACCESSORS
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2MovieRenderPipeline_Source_Public_FSR2MoviePipelineSettings_h_41_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUFSR2MoviePipelineSettings(); \
	friend struct Z_Construct_UClass_UFSR2MoviePipelineSettings_Statics; \
public: \
	DECLARE_CLASS(UFSR2MoviePipelineSettings, UMoviePipelineViewFamilySetting, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/FSR2MovieRenderPipeline"), NO_API) \
	DECLARE_SERIALIZER(UFSR2MoviePipelineSettings)


#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2MovieRenderPipeline_Source_Public_FSR2MoviePipelineSettings_h_41_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UFSR2MoviePipelineSettings(UFSR2MoviePipelineSettings&&); \
	NO_API UFSR2MoviePipelineSettings(const UFSR2MoviePipelineSettings&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UFSR2MoviePipelineSettings); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UFSR2MoviePipelineSettings); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(UFSR2MoviePipelineSettings) \
	NO_API virtual ~UFSR2MoviePipelineSettings();


#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2MovieRenderPipeline_Source_Public_FSR2MoviePipelineSettings_h_38_PROLOG
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2MovieRenderPipeline_Source_Public_FSR2MoviePipelineSettings_h_41_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2MovieRenderPipeline_Source_Public_FSR2MoviePipelineSettings_h_41_SPARSE_DATA \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2MovieRenderPipeline_Source_Public_FSR2MoviePipelineSettings_h_41_SPARSE_DATA_PROPERTY_ACCESSORS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2MovieRenderPipeline_Source_Public_FSR2MoviePipelineSettings_h_41_EDITOR_ONLY_SPARSE_DATA_PROPERTY_ACCESSORS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2MovieRenderPipeline_Source_Public_FSR2MoviePipelineSettings_h_41_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2MovieRenderPipeline_Source_Public_FSR2MoviePipelineSettings_h_41_ACCESSORS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2MovieRenderPipeline_Source_Public_FSR2MoviePipelineSettings_h_41_INCLASS_NO_PURE_DECLS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2MovieRenderPipeline_Source_Public_FSR2MoviePipelineSettings_h_41_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> FSR2MOVIERENDERPIPELINE_API UClass* StaticClass<class UFSR2MoviePipelineSettings>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2MovieRenderPipeline_Source_Public_FSR2MoviePipelineSettings_h


#define FOREACH_ENUM_EFSR2MOVIEPIPELINEQUALITY(op) \
	op(EFSR2MoviePipelineQuality::Unused) \
	op(EFSR2MoviePipelineQuality::Quality) \
	op(EFSR2MoviePipelineQuality::Balanced) \
	op(EFSR2MoviePipelineQuality::Performance) \
	op(EFSR2MoviePipelineQuality::UltraPerformance) 

enum class EFSR2MoviePipelineQuality : uint8;
template<> struct TIsUEnumClass<EFSR2MoviePipelineQuality> { enum { Value = true }; };
template<> FSR2MOVIERENDERPIPELINE_API UEnum* StaticEnum<EFSR2MoviePipelineQuality>();

PRAGMA_ENABLE_DEPRECATION_WARNINGS
