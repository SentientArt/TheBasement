// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "StreamlineLibrary.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
enum class UStreamlineFeature : uint8;
enum class UStreamlineFeatureRequirementsFlags : uint8;
enum class UStreamlineFeatureSupport : uint8;
struct FStreamlineFeatureRequirements;
#ifdef STREAMLINEBLUEPRINT_StreamlineLibrary_generated_h
#error "StreamlineLibrary.generated.h already included, missing '#pragma once' in StreamlineLibrary.h"
#endif
#define STREAMLINEBLUEPRINT_StreamlineLibrary_generated_h

#define FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibrary_h_68_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FStreamlineVersion_Statics; \
	STREAMLINEBLUEPRINT_API static class UScriptStruct* StaticStruct();


template<> STREAMLINEBLUEPRINT_API UScriptStruct* StaticStruct<struct FStreamlineVersion>();

#define FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibrary_h_85_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FStreamlineFeatureRequirements_Statics; \
	STREAMLINEBLUEPRINT_API static class UScriptStruct* StaticStruct();


template<> STREAMLINEBLUEPRINT_API UScriptStruct* StaticStruct<struct FStreamlineFeatureRequirements>();

#define FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibrary_h_110_SPARSE_DATA
#define FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibrary_h_110_SPARSE_DATA_PROPERTY_ACCESSORS
#define FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibrary_h_110_EDITOR_ONLY_SPARSE_DATA_PROPERTY_ACCESSORS
#define FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibrary_h_110_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execQueryStreamlineFeatureSupport); \
	DECLARE_FUNCTION(execIsStreamlineFeatureSupported); \
	DECLARE_FUNCTION(execBreakStreamlineFeatureRequirements); \
	DECLARE_FUNCTION(execGetStreamlineFeatureInformation);


#define FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibrary_h_110_ACCESSORS
#define FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibrary_h_110_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUStreamlineLibrary(); \
	friend struct Z_Construct_UClass_UStreamlineLibrary_Statics; \
public: \
	DECLARE_CLASS(UStreamlineLibrary, UBlueprintFunctionLibrary, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/StreamlineBlueprint"), STREAMLINEBLUEPRINT_API) \
	DECLARE_SERIALIZER(UStreamlineLibrary)


#define FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibrary_h_110_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	STREAMLINEBLUEPRINT_API UStreamlineLibrary(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	STREAMLINEBLUEPRINT_API UStreamlineLibrary(UStreamlineLibrary&&); \
	STREAMLINEBLUEPRINT_API UStreamlineLibrary(const UStreamlineLibrary&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(STREAMLINEBLUEPRINT_API, UStreamlineLibrary); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UStreamlineLibrary); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UStreamlineLibrary) \
	STREAMLINEBLUEPRINT_API virtual ~UStreamlineLibrary();


#define FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibrary_h_106_PROLOG
#define FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibrary_h_110_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibrary_h_110_SPARSE_DATA \
	FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibrary_h_110_SPARSE_DATA_PROPERTY_ACCESSORS \
	FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibrary_h_110_EDITOR_ONLY_SPARSE_DATA_PROPERTY_ACCESSORS \
	FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibrary_h_110_RPC_WRAPPERS_NO_PURE_DECLS \
	FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibrary_h_110_ACCESSORS \
	FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibrary_h_110_INCLASS_NO_PURE_DECLS \
	FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibrary_h_110_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> STREAMLINEBLUEPRINT_API UClass* StaticClass<class UStreamlineLibrary>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibrary_h


#define FOREACH_ENUM_USTREAMLINEFEATURE(op) \
	op(UStreamlineFeature::DLSSG) \
	op(UStreamlineFeature::Reflex) \
	op(UStreamlineFeature::Count) 

enum class UStreamlineFeature : uint8;
template<> struct TIsUEnumClass<UStreamlineFeature> { enum { Value = true }; };
template<> STREAMLINEBLUEPRINT_API UEnum* StaticEnum<UStreamlineFeature>();

#define FOREACH_ENUM_USTREAMLINEFEATURESUPPORT(op) \
	op(UStreamlineFeatureSupport::Supported) \
	op(UStreamlineFeatureSupport::NotSupported) \
	op(UStreamlineFeatureSupport::NotSupportedIncompatibleHardware) \
	op(UStreamlineFeatureSupport::NotSupportedDriverOutOfDate) \
	op(UStreamlineFeatureSupport::NotSupportedOperatingSystemOutOfDate) \
	op(UStreamlineFeatureSupport::NotSupportedHardewareSchedulingDisabled) \
	op(UStreamlineFeatureSupport::NotSupportedByRHI) \
	op(UStreamlineFeatureSupport::NotSupportedByPlatformAtBuildTime) \
	op(UStreamlineFeatureSupport::NotSupportedIncompatibleAPICaptureToolActive) 

enum class UStreamlineFeatureSupport : uint8;
template<> struct TIsUEnumClass<UStreamlineFeatureSupport> { enum { Value = true }; };
template<> STREAMLINEBLUEPRINT_API UEnum* StaticEnum<UStreamlineFeatureSupport>();

#define FOREACH_ENUM_USTREAMLINEFEATUREREQUIREMENTSFLAGS(op) \
	op(UStreamlineFeatureRequirementsFlags::None) \
	op(UStreamlineFeatureRequirementsFlags::D3D11Supported) \
	op(UStreamlineFeatureRequirementsFlags::D3D12Supported) \
	op(UStreamlineFeatureRequirementsFlags::VulkanSupported) \
	op(UStreamlineFeatureRequirementsFlags::VSyncOffRequired) \
	op(UStreamlineFeatureRequirementsFlags::HardwareSchedulingRequired) 

enum class UStreamlineFeatureRequirementsFlags : uint8;
template<> struct TIsUEnumClass<UStreamlineFeatureRequirementsFlags> { enum { Value = true }; };
template<> STREAMLINEBLUEPRINT_API UEnum* StaticEnum<UStreamlineFeatureRequirementsFlags>();

PRAGMA_ENABLE_DEPRECATION_WARNINGS
