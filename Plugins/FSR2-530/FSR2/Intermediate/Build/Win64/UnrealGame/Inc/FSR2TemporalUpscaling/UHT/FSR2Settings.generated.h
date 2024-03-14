// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "FSR2Settings.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef FSR2TEMPORALUPSCALING_FSR2Settings_generated_h
#error "FSR2Settings.generated.h already included, missing '#pragma once' in FSR2Settings.h"
#endif
#define FSR2TEMPORALUPSCALING_FSR2Settings_generated_h

#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2_Source_FSR2TemporalUpscaling_Public_FSR2Settings_h_80_SPARSE_DATA
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2_Source_FSR2TemporalUpscaling_Public_FSR2Settings_h_80_SPARSE_DATA_PROPERTY_ACCESSORS
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2_Source_FSR2TemporalUpscaling_Public_FSR2Settings_h_80_EDITOR_ONLY_SPARSE_DATA_PROPERTY_ACCESSORS
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2_Source_FSR2TemporalUpscaling_Public_FSR2Settings_h_80_RPC_WRAPPERS
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2_Source_FSR2TemporalUpscaling_Public_FSR2Settings_h_80_ACCESSORS
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2_Source_FSR2TemporalUpscaling_Public_FSR2Settings_h_80_INCLASS \
private: \
	static void StaticRegisterNativesUFSR2Settings(); \
	friend struct Z_Construct_UClass_UFSR2Settings_Statics; \
public: \
	DECLARE_CLASS(UFSR2Settings, UDeveloperSettings, COMPILED_IN_FLAGS(0 | CLASS_DefaultConfig | CLASS_Config), CASTCLASS_None, TEXT("/Script/FSR2TemporalUpscaling"), NO_API) \
	DECLARE_SERIALIZER(UFSR2Settings) \
	static const TCHAR* StaticConfigName() {return TEXT("Engine");} \



#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2_Source_FSR2TemporalUpscaling_Public_FSR2Settings_h_80_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UFSR2Settings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UFSR2Settings) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UFSR2Settings); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UFSR2Settings); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UFSR2Settings(UFSR2Settings&&); \
	NO_API UFSR2Settings(const UFSR2Settings&); \
public: \
	NO_API virtual ~UFSR2Settings();


#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2_Source_FSR2TemporalUpscaling_Public_FSR2Settings_h_77_PROLOG
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2_Source_FSR2TemporalUpscaling_Public_FSR2Settings_h_80_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2_Source_FSR2TemporalUpscaling_Public_FSR2Settings_h_80_SPARSE_DATA \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2_Source_FSR2TemporalUpscaling_Public_FSR2Settings_h_80_SPARSE_DATA_PROPERTY_ACCESSORS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2_Source_FSR2TemporalUpscaling_Public_FSR2Settings_h_80_EDITOR_ONLY_SPARSE_DATA_PROPERTY_ACCESSORS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2_Source_FSR2TemporalUpscaling_Public_FSR2Settings_h_80_RPC_WRAPPERS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2_Source_FSR2TemporalUpscaling_Public_FSR2Settings_h_80_ACCESSORS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2_Source_FSR2TemporalUpscaling_Public_FSR2Settings_h_80_INCLASS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2_Source_FSR2TemporalUpscaling_Public_FSR2Settings_h_80_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> FSR2TEMPORALUPSCALING_API UClass* StaticClass<class UFSR2Settings>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_iFraude_Documents_GitHub_City75_Plugins_FSR2_530_FSR2_Source_FSR2TemporalUpscaling_Public_FSR2Settings_h


#define FOREACH_ENUM_EFSR2QUALITYMODE(op) \
	op(EFSR2QualityMode::Unused) \
	op(EFSR2QualityMode::Quality) \
	op(EFSR2QualityMode::Balanced) \
	op(EFSR2QualityMode::Performance) \
	op(EFSR2QualityMode::UltraPerformance) 

enum class EFSR2QualityMode : int32;
template<> struct TIsUEnumClass<EFSR2QualityMode> { enum { Value = true }; };
template<> FSR2TEMPORALUPSCALING_API UEnum* StaticEnum<EFSR2QualityMode>();

#define FOREACH_ENUM_EFSR2HISTORYFORMAT(op) \
	op(EFSR2HistoryFormat::FloatRGBA) \
	op(EFSR2HistoryFormat::FloatR11G11B10) 

enum class EFSR2HistoryFormat : int32;
template<> struct TIsUEnumClass<EFSR2HistoryFormat> { enum { Value = true }; };
template<> FSR2TEMPORALUPSCALING_API UEnum* StaticEnum<EFSR2HistoryFormat>();

#define FOREACH_ENUM_EFSR2DEDITHERMODE(op) \
	op(EFSR2DeDitherMode::Off) \
	op(EFSR2DeDitherMode::Full) \
	op(EFSR2DeDitherMode::HairOnly) 

enum class EFSR2DeDitherMode : int32;
template<> struct TIsUEnumClass<EFSR2DeDitherMode> { enum { Value = true }; };
template<> FSR2TEMPORALUPSCALING_API UEnum* StaticEnum<EFSR2DeDitherMode>();

#define FOREACH_ENUM_EFSR2LANDSCAPEHISMMODE(op) \
	op(EFSR2LandscapeHISMMode::Off) \
	op(EFSR2LandscapeHISMMode::AllStatic) \
	op(EFSR2LandscapeHISMMode::StaticWPO) 

enum class EFSR2LandscapeHISMMode : int32;
template<> struct TIsUEnumClass<EFSR2LandscapeHISMMode> { enum { Value = true }; };
template<> FSR2TEMPORALUPSCALING_API UEnum* StaticEnum<EFSR2LandscapeHISMMode>();

PRAGMA_ENABLE_DEPRECATION_WARNINGS
