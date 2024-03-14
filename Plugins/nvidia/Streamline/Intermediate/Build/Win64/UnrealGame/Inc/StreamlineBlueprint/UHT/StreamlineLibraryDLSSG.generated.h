// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "StreamlineLibraryDLSSG.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
enum class UStreamlineDLSSGMode : uint8;
enum class UStreamlineFeatureSupport : uint8;
#ifdef STREAMLINEBLUEPRINT_StreamlineLibraryDLSSG_generated_h
#error "StreamlineLibraryDLSSG.generated.h already included, missing '#pragma once' in StreamlineLibraryDLSSG.h"
#endif
#define STREAMLINEBLUEPRINT_StreamlineLibraryDLSSG_generated_h

#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryDLSSG_h_44_SPARSE_DATA
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryDLSSG_h_44_SPARSE_DATA_PROPERTY_ACCESSORS
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryDLSSG_h_44_EDITOR_ONLY_SPARSE_DATA_PROPERTY_ACCESSORS
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryDLSSG_h_44_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execGetDLSSGFrameTiming); \
	DECLARE_FUNCTION(execGetDefaultDLSSGMode); \
	DECLARE_FUNCTION(execGetDLSSGMode); \
	DECLARE_FUNCTION(execSetDLSSGMode); \
	DECLARE_FUNCTION(execGetSupportedDLSSGModes); \
	DECLARE_FUNCTION(execIsDLSSGModeSupported); \
	DECLARE_FUNCTION(execQueryDLSSGSupport); \
	DECLARE_FUNCTION(execIsDLSSGSupported);


#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryDLSSG_h_44_ACCESSORS
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryDLSSG_h_44_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUStreamlineLibraryDLSSG(); \
	friend struct Z_Construct_UClass_UStreamlineLibraryDLSSG_Statics; \
public: \
	DECLARE_CLASS(UStreamlineLibraryDLSSG, UBlueprintFunctionLibrary, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/StreamlineBlueprint"), STREAMLINEBLUEPRINT_API) \
	DECLARE_SERIALIZER(UStreamlineLibraryDLSSG)


#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryDLSSG_h_44_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	STREAMLINEBLUEPRINT_API UStreamlineLibraryDLSSG(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	STREAMLINEBLUEPRINT_API UStreamlineLibraryDLSSG(UStreamlineLibraryDLSSG&&); \
	STREAMLINEBLUEPRINT_API UStreamlineLibraryDLSSG(const UStreamlineLibraryDLSSG&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(STREAMLINEBLUEPRINT_API, UStreamlineLibraryDLSSG); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UStreamlineLibraryDLSSG); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UStreamlineLibraryDLSSG) \
	STREAMLINEBLUEPRINT_API virtual ~UStreamlineLibraryDLSSG();


#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryDLSSG_h_40_PROLOG
#define FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryDLSSG_h_44_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryDLSSG_h_44_SPARSE_DATA \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryDLSSG_h_44_SPARSE_DATA_PROPERTY_ACCESSORS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryDLSSG_h_44_EDITOR_ONLY_SPARSE_DATA_PROPERTY_ACCESSORS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryDLSSG_h_44_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryDLSSG_h_44_ACCESSORS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryDLSSG_h_44_INCLASS_NO_PURE_DECLS \
	FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryDLSSG_h_44_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> STREAMLINEBLUEPRINT_API UClass* StaticClass<class UStreamlineLibraryDLSSG>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryDLSSG_h


#define FOREACH_ENUM_USTREAMLINEDLSSGMODE(op) \
	op(UStreamlineDLSSGMode::Off) \
	op(UStreamlineDLSSGMode::On) 

enum class UStreamlineDLSSGMode : uint8;
template<> struct TIsUEnumClass<UStreamlineDLSSGMode> { enum { Value = true }; };
template<> STREAMLINEBLUEPRINT_API UEnum* StaticEnum<UStreamlineDLSSGMode>();

PRAGMA_ENABLE_DEPRECATION_WARNINGS
