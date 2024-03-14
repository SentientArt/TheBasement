// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "NISLibrary.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
enum class UNISMode : uint8;
#ifdef NISBLUEPRINT_NISLibrary_generated_h
#error "NISLibrary.generated.h already included, missing '#pragma once' in NISLibrary.h"
#endif
#define NISBLUEPRINT_NISLibrary_generated_h

#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_NIS_Source_NISBlueprint_Public_NISLibrary_h_49_SPARSE_DATA
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_NIS_Source_NISBlueprint_Public_NISLibrary_h_49_SPARSE_DATA_PROPERTY_ACCESSORS
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_NIS_Source_NISBlueprint_Public_NISLibrary_h_49_EDITOR_ONLY_SPARSE_DATA_PROPERTY_ACCESSORS
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_NIS_Source_NISBlueprint_Public_NISLibrary_h_49_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execGetDefaultNISMode); \
	DECLARE_FUNCTION(execSetNISSharpness); \
	DECLARE_FUNCTION(execSetNISCustomScreenPercentage); \
	DECLARE_FUNCTION(execSetNISMode); \
	DECLARE_FUNCTION(execGetNISScreenPercentageRange); \
	DECLARE_FUNCTION(execGetNISRecommendedScreenPercentage); \
	DECLARE_FUNCTION(execGetSupportedNISModes); \
	DECLARE_FUNCTION(execIsNISModeSupported); \
	DECLARE_FUNCTION(execIsNISSupported);


#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_NIS_Source_NISBlueprint_Public_NISLibrary_h_49_ACCESSORS
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_NIS_Source_NISBlueprint_Public_NISLibrary_h_49_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUNISLibrary(); \
	friend struct Z_Construct_UClass_UNISLibrary_Statics; \
public: \
	DECLARE_CLASS(UNISLibrary, UBlueprintFunctionLibrary, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/NISBlueprint"), NISBLUEPRINT_API) \
	DECLARE_SERIALIZER(UNISLibrary)


#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_NIS_Source_NISBlueprint_Public_NISLibrary_h_49_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NISBLUEPRINT_API UNISLibrary(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NISBLUEPRINT_API UNISLibrary(UNISLibrary&&); \
	NISBLUEPRINT_API UNISLibrary(const UNISLibrary&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NISBLUEPRINT_API, UNISLibrary); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UNISLibrary); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UNISLibrary) \
	NISBLUEPRINT_API virtual ~UNISLibrary();


#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_NIS_Source_NISBlueprint_Public_NISLibrary_h_45_PROLOG
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_NIS_Source_NISBlueprint_Public_NISLibrary_h_49_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_NIS_Source_NISBlueprint_Public_NISLibrary_h_49_SPARSE_DATA \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_NIS_Source_NISBlueprint_Public_NISLibrary_h_49_SPARSE_DATA_PROPERTY_ACCESSORS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_NIS_Source_NISBlueprint_Public_NISLibrary_h_49_EDITOR_ONLY_SPARSE_DATA_PROPERTY_ACCESSORS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_NIS_Source_NISBlueprint_Public_NISLibrary_h_49_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_NIS_Source_NISBlueprint_Public_NISLibrary_h_49_ACCESSORS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_NIS_Source_NISBlueprint_Public_NISLibrary_h_49_INCLASS_NO_PURE_DECLS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_NIS_Source_NISBlueprint_Public_NISLibrary_h_49_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> NISBLUEPRINT_API UClass* StaticClass<class UNISLibrary>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_NIS_Source_NISBlueprint_Public_NISLibrary_h


#define FOREACH_ENUM_UNISSUPPORT(op) \
	op(UNISSupport::Supported) \
	op(UNISSupport::NotSupported) 

enum class UNISSupport : uint8;
template<> struct TIsUEnumClass<UNISSupport> { enum { Value = true }; };
template<> NISBLUEPRINT_API UEnum* StaticEnum<UNISSupport>();

#define FOREACH_ENUM_UNISMODE(op) \
	op(UNISMode::Off) \
	op(UNISMode::UltraQuality) \
	op(UNISMode::Quality) \
	op(UNISMode::Balanced) \
	op(UNISMode::Performance) \
	op(UNISMode::Custom) 

enum class UNISMode : uint8;
template<> struct TIsUEnumClass<UNISMode> { enum { Value = true }; };
template<> NISBLUEPRINT_API UEnum* StaticEnum<UNISMode>();

PRAGMA_ENABLE_DEPRECATION_WARNINGS
