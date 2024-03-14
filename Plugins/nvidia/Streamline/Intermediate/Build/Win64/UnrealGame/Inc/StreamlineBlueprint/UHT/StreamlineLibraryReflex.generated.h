// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "StreamlineLibraryReflex.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
enum class UStreamlineFeatureSupport : uint8;
enum class UStreamlineReflexMode : uint8;
#ifdef STREAMLINEBLUEPRINT_StreamlineLibraryReflex_generated_h
#error "StreamlineLibraryReflex.generated.h already included, missing '#pragma once' in StreamlineLibraryReflex.h"
#endif
#define STREAMLINEBLUEPRINT_StreamlineLibraryReflex_generated_h

#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryReflex_h_44_SPARSE_DATA
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryReflex_h_44_SPARSE_DATA_PROPERTY_ACCESSORS
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryReflex_h_44_EDITOR_ONLY_SPARSE_DATA_PROPERTY_ACCESSORS
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryReflex_h_44_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execGetRenderLatencyInMs); \
	DECLARE_FUNCTION(execGetGameLatencyInMs); \
	DECLARE_FUNCTION(execGetGameToRenderLatencyInMs); \
	DECLARE_FUNCTION(execGetDefaultReflexMode); \
	DECLARE_FUNCTION(execGetReflexMode); \
	DECLARE_FUNCTION(execSetReflexMode); \
	DECLARE_FUNCTION(execQueryReflexSupport); \
	DECLARE_FUNCTION(execIsReflexSupported);


#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryReflex_h_44_ACCESSORS
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryReflex_h_44_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUStreamlineLibraryReflex(); \
	friend struct Z_Construct_UClass_UStreamlineLibraryReflex_Statics; \
public: \
	DECLARE_CLASS(UStreamlineLibraryReflex, UBlueprintFunctionLibrary, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/StreamlineBlueprint"), STREAMLINEBLUEPRINT_API) \
	DECLARE_SERIALIZER(UStreamlineLibraryReflex)


#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryReflex_h_44_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	STREAMLINEBLUEPRINT_API UStreamlineLibraryReflex(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	STREAMLINEBLUEPRINT_API UStreamlineLibraryReflex(UStreamlineLibraryReflex&&); \
	STREAMLINEBLUEPRINT_API UStreamlineLibraryReflex(const UStreamlineLibraryReflex&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(STREAMLINEBLUEPRINT_API, UStreamlineLibraryReflex); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UStreamlineLibraryReflex); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UStreamlineLibraryReflex) \
	STREAMLINEBLUEPRINT_API virtual ~UStreamlineLibraryReflex();


#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryReflex_h_40_PROLOG
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryReflex_h_44_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryReflex_h_44_SPARSE_DATA \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryReflex_h_44_SPARSE_DATA_PROPERTY_ACCESSORS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryReflex_h_44_EDITOR_ONLY_SPARSE_DATA_PROPERTY_ACCESSORS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryReflex_h_44_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryReflex_h_44_ACCESSORS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryReflex_h_44_INCLASS_NO_PURE_DECLS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryReflex_h_44_ENHANCED_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> STREAMLINEBLUEPRINT_API UClass* StaticClass<class UStreamlineLibraryReflex>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryReflex_h


#define FOREACH_ENUM_USTREAMLINEREFLEXMODE(op) \
	op(UStreamlineReflexMode::Disabled) \
	op(UStreamlineReflexMode::Enabled) \
	op(UStreamlineReflexMode::EnabledPlusBoost) 

enum class UStreamlineReflexMode : uint8;
template<> struct TIsUEnumClass<UStreamlineReflexMode> { enum { Value = true }; };
template<> STREAMLINEBLUEPRINT_API UEnum* StaticEnum<UStreamlineReflexMode>();

PRAGMA_ENABLE_DEPRECATION_WARNINGS
