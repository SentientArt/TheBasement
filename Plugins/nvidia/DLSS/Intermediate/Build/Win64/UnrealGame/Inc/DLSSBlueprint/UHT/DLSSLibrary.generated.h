// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "DLSSLibrary.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class UObject;
enum class UDLSSMode : uint8;
enum class UDLSSSupport : uint8;
#ifdef DLSSBLUEPRINT_DLSSLibrary_generated_h
#error "DLSSLibrary.generated.h already included, missing '#pragma once' in DLSSLibrary.h"
#endif
#define DLSSBLUEPRINT_DLSSLibrary_generated_h

#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_DLSS_Source_DLSSBlueprint_Public_DLSSLibrary_h_60_SPARSE_DATA
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_DLSS_Source_DLSSBlueprint_Public_DLSSLibrary_h_60_SPARSE_DATA_PROPERTY_ACCESSORS
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_DLSS_Source_DLSSBlueprint_Public_DLSSLibrary_h_60_EDITOR_ONLY_SPARSE_DATA_PROPERTY_ACCESSORS
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_DLSS_Source_DLSSBlueprint_Public_DLSSLibrary_h_60_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execGetDefaultDLSSMode); \
	DECLARE_FUNCTION(execGetDLSSSharpness); \
	DECLARE_FUNCTION(execSetDLSSSharpness); \
	DECLARE_FUNCTION(execGetDLSSMode); \
	DECLARE_FUNCTION(execSetDLSSMode); \
	DECLARE_FUNCTION(execGetDLSSScreenPercentageRange); \
	DECLARE_FUNCTION(execGetDLSSModeInformation); \
	DECLARE_FUNCTION(execGetSupportedDLSSModes); \
	DECLARE_FUNCTION(execIsDLSSModeSupported); \
	DECLARE_FUNCTION(execIsDLAAEnabled); \
	DECLARE_FUNCTION(execEnableDLAA); \
	DECLARE_FUNCTION(execIsDLSSRREnabled); \
	DECLARE_FUNCTION(execEnableDLSSRR); \
	DECLARE_FUNCTION(execIsDLSSEnabled); \
	DECLARE_FUNCTION(execEnableDLSS); \
	DECLARE_FUNCTION(execGetDLSSRRMinimumDriverVersion); \
	DECLARE_FUNCTION(execGetDLSSMinimumDriverVersion); \
	DECLARE_FUNCTION(execQueryDLSSRRSupport); \
	DECLARE_FUNCTION(execQueryDLSSSupport); \
	DECLARE_FUNCTION(execIsDLSSRRSupported); \
	DECLARE_FUNCTION(execIsDLSSSupported);


#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_DLSS_Source_DLSSBlueprint_Public_DLSSLibrary_h_60_ACCESSORS
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_DLSS_Source_DLSSBlueprint_Public_DLSSLibrary_h_60_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUDLSSLibrary(); \
	friend struct Z_Construct_UClass_UDLSSLibrary_Statics; \
public: \
	DECLARE_CLASS(UDLSSLibrary, UBlueprintFunctionLibrary, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/DLSSBlueprint"), DLSSBLUEPRINT_API) \
	DECLARE_SERIALIZER(UDLSSLibrary)


#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_DLSS_Source_DLSSBlueprint_Public_DLSSLibrary_h_60_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	DLSSBLUEPRINT_API UDLSSLibrary(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	DLSSBLUEPRINT_API UDLSSLibrary(UDLSSLibrary&&); \
	DLSSBLUEPRINT_API UDLSSLibrary(const UDLSSLibrary&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(DLSSBLUEPRINT_API, UDLSSLibrary); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UDLSSLibrary); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UDLSSLibrary) \
	DLSSBLUEPRINT_API virtual ~UDLSSLibrary();


#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_DLSS_Source_DLSSBlueprint_Public_DLSSLibrary_h_56_PROLOG
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_DLSS_Source_DLSSBlueprint_Public_DLSSLibrary_h_60_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_DLSS_Source_DLSSBlueprint_Public_DLSSLibrary_h_60_SPARSE_DATA \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_DLSS_Source_DLSSBlueprint_Public_DLSSLibrary_h_60_SPARSE_DATA_PROPERTY_ACCESSORS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_DLSS_Source_DLSSBlueprint_Public_DLSSLibrary_h_60_EDITOR_ONLY_SPARSE_DATA_PROPERTY_ACCESSORS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_DLSS_Source_DLSSBlueprint_Public_DLSSLibrary_h_60_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_DLSS_Source_DLSSBlueprint_Public_DLSSLibrary_h_60_ACCESSORS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_DLSS_Source_DLSSBlueprint_Public_DLSSLibrary_h_60_INCLASS_NO_PURE_DECLS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_DLSS_Source_DLSSBlueprint_Public_DLSSLibrary_h_60_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> DLSSBLUEPRINT_API UClass* StaticClass<class UDLSSLibrary>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_DLSS_Source_DLSSBlueprint_Public_DLSSLibrary_h


#define FOREACH_ENUM_UDLSSSUPPORT(op) \
	op(UDLSSSupport::Supported) \
	op(UDLSSSupport::NotSupported) \
	op(UDLSSSupport::NotSupportedIncompatibleHardware) \
	op(UDLSSSupport::NotSupportedDriverOutOfDate) \
	op(UDLSSSupport::NotSupportedOperatingSystemOutOfDate) \
	op(UDLSSSupport::NotSupportedByPlatformAtBuildTime) \
	op(UDLSSSupport::NotSupportedIncompatibleAPICaptureToolActive) 

enum class UDLSSSupport : uint8;
template<> struct TIsUEnumClass<UDLSSSupport> { enum { Value = true }; };
template<> DLSSBLUEPRINT_API UEnum* StaticEnum<UDLSSSupport>();

#define FOREACH_ENUM_UDLSSMODE(op) \
	op(UDLSSMode::Off) \
	op(UDLSSMode::Auto) \
	op(UDLSSMode::DLAA) \
	op(UDLSSMode::UltraQuality) \
	op(UDLSSMode::Quality) \
	op(UDLSSMode::Balanced) \
	op(UDLSSMode::Performance) \
	op(UDLSSMode::UltraPerformance) 

enum class UDLSSMode : uint8;
template<> struct TIsUEnumClass<UDLSSMode> { enum { Value = true }; };
template<> DLSSBLUEPRINT_API UEnum* StaticEnum<UDLSSMode>();

PRAGMA_ENABLE_DEPRECATION_WARNINGS
