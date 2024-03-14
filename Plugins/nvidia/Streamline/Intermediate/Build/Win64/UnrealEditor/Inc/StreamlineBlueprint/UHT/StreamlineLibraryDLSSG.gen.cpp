// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "StreamlineBlueprint/Public/StreamlineLibraryDLSSG.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeStreamlineLibraryDLSSG() {}
// Cross Module References
	ENGINE_API UClass* Z_Construct_UClass_UBlueprintFunctionLibrary();
	STREAMLINEBLUEPRINT_API UClass* Z_Construct_UClass_UStreamlineLibraryDLSSG();
	STREAMLINEBLUEPRINT_API UClass* Z_Construct_UClass_UStreamlineLibraryDLSSG_NoRegister();
	STREAMLINEBLUEPRINT_API UEnum* Z_Construct_UEnum_StreamlineBlueprint_UStreamlineDLSSGMode();
	STREAMLINEBLUEPRINT_API UEnum* Z_Construct_UEnum_StreamlineBlueprint_UStreamlineFeatureSupport();
	UPackage* Z_Construct_UPackage__Script_StreamlineBlueprint();
// End Cross Module References
	static FEnumRegistrationInfo Z_Registration_Info_UEnum_UStreamlineDLSSGMode;
	static UEnum* UStreamlineDLSSGMode_StaticEnum()
	{
		if (!Z_Registration_Info_UEnum_UStreamlineDLSSGMode.OuterSingleton)
		{
			Z_Registration_Info_UEnum_UStreamlineDLSSGMode.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_StreamlineBlueprint_UStreamlineDLSSGMode, (UObject*)Z_Construct_UPackage__Script_StreamlineBlueprint(), TEXT("UStreamlineDLSSGMode"));
		}
		return Z_Registration_Info_UEnum_UStreamlineDLSSGMode.OuterSingleton;
	}
	template<> STREAMLINEBLUEPRINT_API UEnum* StaticEnum<UStreamlineDLSSGMode>()
	{
		return UStreamlineDLSSGMode_StaticEnum();
	}
	struct Z_Construct_UEnum_StreamlineBlueprint_UStreamlineDLSSGMode_Statics
	{
		static const UECodeGen_Private::FEnumeratorParam Enumerators[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[];
#endif
		static const UECodeGen_Private::FEnumParams EnumParams;
	};
	const UECodeGen_Private::FEnumeratorParam Z_Construct_UEnum_StreamlineBlueprint_UStreamlineDLSSGMode_Statics::Enumerators[] = {
		{ "UStreamlineDLSSGMode::Off", (int64)UStreamlineDLSSGMode::Off },
		{ "UStreamlineDLSSGMode::On", (int64)UStreamlineDLSSGMode::On },
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UEnum_StreamlineBlueprint_UStreamlineDLSSGMode_Statics::Enum_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/StreamlineLibraryDLSSG.h" },
		{ "Off.DisplayName", "Off" },
		{ "Off.Name", "UStreamlineDLSSGMode::Off" },
		{ "On.DisplayName", "On" },
		{ "On.Name", "UStreamlineDLSSGMode::On" },
	};
#endif
	const UECodeGen_Private::FEnumParams Z_Construct_UEnum_StreamlineBlueprint_UStreamlineDLSSGMode_Statics::EnumParams = {
		(UObject*(*)())Z_Construct_UPackage__Script_StreamlineBlueprint,
		nullptr,
		"UStreamlineDLSSGMode",
		"UStreamlineDLSSGMode",
		Z_Construct_UEnum_StreamlineBlueprint_UStreamlineDLSSGMode_Statics::Enumerators,
		RF_Public|RF_Transient|RF_MarkAsNative,
		UE_ARRAY_COUNT(Z_Construct_UEnum_StreamlineBlueprint_UStreamlineDLSSGMode_Statics::Enumerators),
		EEnumFlags::None,
		(uint8)UEnum::ECppForm::EnumClass,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UEnum_StreamlineBlueprint_UStreamlineDLSSGMode_Statics::Enum_MetaDataParams), Z_Construct_UEnum_StreamlineBlueprint_UStreamlineDLSSGMode_Statics::Enum_MetaDataParams)
	};
	UEnum* Z_Construct_UEnum_StreamlineBlueprint_UStreamlineDLSSGMode()
	{
		if (!Z_Registration_Info_UEnum_UStreamlineDLSSGMode.InnerSingleton)
		{
			UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_UStreamlineDLSSGMode.InnerSingleton, Z_Construct_UEnum_StreamlineBlueprint_UStreamlineDLSSGMode_Statics::EnumParams);
		}
		return Z_Registration_Info_UEnum_UStreamlineDLSSGMode.InnerSingleton;
	}
	DEFINE_FUNCTION(UStreamlineLibraryDLSSG::execGetDLSSGFrameTiming)
	{
		P_GET_PROPERTY_REF(FFloatProperty,Z_Param_Out_FrameRateInHertz);
		P_GET_PROPERTY_REF(FIntProperty,Z_Param_Out_FramesPresented);
		P_FINISH;
		P_NATIVE_BEGIN;
		UStreamlineLibraryDLSSG::GetDLSSGFrameTiming(Z_Param_Out_FrameRateInHertz,Z_Param_Out_FramesPresented);
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UStreamlineLibraryDLSSG::execGetDefaultDLSSGMode)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		*(UStreamlineDLSSGMode*)Z_Param__Result=UStreamlineLibraryDLSSG::GetDefaultDLSSGMode();
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UStreamlineLibraryDLSSG::execGetDLSSGMode)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		*(UStreamlineDLSSGMode*)Z_Param__Result=UStreamlineLibraryDLSSG::GetDLSSGMode();
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UStreamlineLibraryDLSSG::execSetDLSSGMode)
	{
		P_GET_ENUM(UStreamlineDLSSGMode,Z_Param_DLSSGMode);
		P_FINISH;
		P_NATIVE_BEGIN;
		UStreamlineLibraryDLSSG::SetDLSSGMode(UStreamlineDLSSGMode(Z_Param_DLSSGMode));
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UStreamlineLibraryDLSSG::execGetSupportedDLSSGModes)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		*(TArray<UStreamlineDLSSGMode>*)Z_Param__Result=UStreamlineLibraryDLSSG::GetSupportedDLSSGModes();
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UStreamlineLibraryDLSSG::execIsDLSSGModeSupported)
	{
		P_GET_ENUM(UStreamlineDLSSGMode,Z_Param_DLSSGMode);
		P_FINISH;
		P_NATIVE_BEGIN;
		*(bool*)Z_Param__Result=UStreamlineLibraryDLSSG::IsDLSSGModeSupported(UStreamlineDLSSGMode(Z_Param_DLSSGMode));
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UStreamlineLibraryDLSSG::execQueryDLSSGSupport)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		*(UStreamlineFeatureSupport*)Z_Param__Result=UStreamlineLibraryDLSSG::QueryDLSSGSupport();
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UStreamlineLibraryDLSSG::execIsDLSSGSupported)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		*(bool*)Z_Param__Result=UStreamlineLibraryDLSSG::IsDLSSGSupported();
		P_NATIVE_END;
	}
	void UStreamlineLibraryDLSSG::StaticRegisterNativesUStreamlineLibraryDLSSG()
	{
		UClass* Class = UStreamlineLibraryDLSSG::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "GetDefaultDLSSGMode", &UStreamlineLibraryDLSSG::execGetDefaultDLSSGMode },
			{ "GetDLSSGFrameTiming", &UStreamlineLibraryDLSSG::execGetDLSSGFrameTiming },
			{ "GetDLSSGMode", &UStreamlineLibraryDLSSG::execGetDLSSGMode },
			{ "GetSupportedDLSSGModes", &UStreamlineLibraryDLSSG::execGetSupportedDLSSGModes },
			{ "IsDLSSGModeSupported", &UStreamlineLibraryDLSSG::execIsDLSSGModeSupported },
			{ "IsDLSSGSupported", &UStreamlineLibraryDLSSG::execIsDLSSGSupported },
			{ "QueryDLSSGSupport", &UStreamlineLibraryDLSSG::execQueryDLSSGSupport },
			{ "SetDLSSGMode", &UStreamlineLibraryDLSSG::execSetDLSSGMode },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDefaultDLSSGMode_Statics
	{
		struct StreamlineLibraryDLSSG_eventGetDefaultDLSSGMode_Parms
		{
			UStreamlineDLSSGMode ReturnValue;
		};
		static const UECodeGen_Private::FBytePropertyParams NewProp_ReturnValue_Underlying;
		static const UECodeGen_Private::FEnumPropertyParams NewProp_ReturnValue;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDefaultDLSSGMode_Statics::NewProp_ReturnValue_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDefaultDLSSGMode_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(StreamlineLibraryDLSSG_eventGetDefaultDLSSGMode_Parms, ReturnValue), Z_Construct_UEnum_StreamlineBlueprint_UStreamlineDLSSGMode, METADATA_PARAMS(0, nullptr) }; // 3802091761
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDefaultDLSSGMode_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDefaultDLSSGMode_Statics::NewProp_ReturnValue_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDefaultDLSSGMode_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDefaultDLSSGMode_Statics::Function_MetaDataParams[] = {
		{ "Category", "Streamline|DLSS-G" },
		{ "Comment", "/* Find a reasonable default DLSS-G mode based on current hardware */" },
		{ "DisplayName", "Get Default DLSS-G Mode" },
		{ "ModuleRelativePath", "Public/StreamlineLibraryDLSSG.h" },
		{ "ToolTip", "Find a reasonable default DLSS-G mode based on current hardware" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDefaultDLSSGMode_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UStreamlineLibraryDLSSG, nullptr, "GetDefaultDLSSGMode", nullptr, nullptr, Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDefaultDLSSGMode_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDefaultDLSSGMode_Statics::PropPointers), sizeof(Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDefaultDLSSGMode_Statics::StreamlineLibraryDLSSG_eventGetDefaultDLSSGMode_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x14022403, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDefaultDLSSGMode_Statics::Function_MetaDataParams), Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDefaultDLSSGMode_Statics::Function_MetaDataParams) };
	static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDefaultDLSSGMode_Statics::PropPointers) < 2048);
	static_assert(sizeof(Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDefaultDLSSGMode_Statics::StreamlineLibraryDLSSG_eventGetDefaultDLSSGMode_Parms) < MAX_uint16);
	UFunction* Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDefaultDLSSGMode()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDefaultDLSSGMode_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGFrameTiming_Statics
	{
		struct StreamlineLibraryDLSSG_eventGetDLSSGFrameTiming_Parms
		{
			float FrameRateInHertz;
			int32 FramesPresented;
		};
		static const UECodeGen_Private::FFloatPropertyParams NewProp_FrameRateInHertz;
		static const UECodeGen_Private::FIntPropertyParams NewProp_FramesPresented;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGFrameTiming_Statics::NewProp_FrameRateInHertz = { "FrameRateInHertz", nullptr, (EPropertyFlags)0x0010000000000180, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(StreamlineLibraryDLSSG_eventGetDLSSGFrameTiming_Parms, FrameRateInHertz), METADATA_PARAMS(0, nullptr) };
	const UECodeGen_Private::FIntPropertyParams Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGFrameTiming_Statics::NewProp_FramesPresented = { "FramesPresented", nullptr, (EPropertyFlags)0x0010000000000180, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(StreamlineLibraryDLSSG_eventGetDLSSGFrameTiming_Parms, FramesPresented), METADATA_PARAMS(0, nullptr) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGFrameTiming_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGFrameTiming_Statics::NewProp_FrameRateInHertz,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGFrameTiming_Statics::NewProp_FramesPresented,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGFrameTiming_Statics::Function_MetaDataParams[] = {
		{ "Category", "Streamline|DLSS-G" },
		{ "Comment", "/* Returns the actual framerate and number of frames presented, whether DLSS-G is active or not */" },
		{ "DisplayName", "Get DLSS-G frame frame rate and presented frames" },
		{ "ModuleRelativePath", "Public/StreamlineLibraryDLSSG.h" },
		{ "ToolTip", "Returns the actual framerate and number of frames presented, whether DLSS-G is active or not" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGFrameTiming_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UStreamlineLibraryDLSSG, nullptr, "GetDLSSGFrameTiming", nullptr, nullptr, Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGFrameTiming_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGFrameTiming_Statics::PropPointers), sizeof(Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGFrameTiming_Statics::StreamlineLibraryDLSSG_eventGetDLSSGFrameTiming_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x14422403, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGFrameTiming_Statics::Function_MetaDataParams), Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGFrameTiming_Statics::Function_MetaDataParams) };
	static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGFrameTiming_Statics::PropPointers) < 2048);
	static_assert(sizeof(Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGFrameTiming_Statics::StreamlineLibraryDLSSG_eventGetDLSSGFrameTiming_Parms) < MAX_uint16);
	UFunction* Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGFrameTiming()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGFrameTiming_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGMode_Statics
	{
		struct StreamlineLibraryDLSSG_eventGetDLSSGMode_Parms
		{
			UStreamlineDLSSGMode ReturnValue;
		};
		static const UECodeGen_Private::FBytePropertyParams NewProp_ReturnValue_Underlying;
		static const UECodeGen_Private::FEnumPropertyParams NewProp_ReturnValue;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGMode_Statics::NewProp_ReturnValue_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGMode_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(StreamlineLibraryDLSSG_eventGetDLSSGMode_Parms, ReturnValue), Z_Construct_UEnum_StreamlineBlueprint_UStreamlineDLSSGMode, METADATA_PARAMS(0, nullptr) }; // 3802091761
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGMode_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGMode_Statics::NewProp_ReturnValue_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGMode_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGMode_Statics::Function_MetaDataParams[] = {
		{ "Category", "Streamline|DLSS-G" },
		{ "Comment", "/* Reads the console variables to infer the current DLSS-G mode*/" },
		{ "DisplayName", "Get DLSS-G Mode" },
		{ "ModuleRelativePath", "Public/StreamlineLibraryDLSSG.h" },
		{ "ToolTip", "Reads the console variables to infer the current DLSS-G mode" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGMode_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UStreamlineLibraryDLSSG, nullptr, "GetDLSSGMode", nullptr, nullptr, Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGMode_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGMode_Statics::PropPointers), sizeof(Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGMode_Statics::StreamlineLibraryDLSSG_eventGetDLSSGMode_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x14022403, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGMode_Statics::Function_MetaDataParams), Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGMode_Statics::Function_MetaDataParams) };
	static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGMode_Statics::PropPointers) < 2048);
	static_assert(sizeof(Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGMode_Statics::StreamlineLibraryDLSSG_eventGetDLSSGMode_Parms) < MAX_uint16);
	UFunction* Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGMode()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGMode_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetSupportedDLSSGModes_Statics
	{
		struct StreamlineLibraryDLSSG_eventGetSupportedDLSSGModes_Parms
		{
			TArray<UStreamlineDLSSGMode> ReturnValue;
		};
		static const UECodeGen_Private::FBytePropertyParams NewProp_ReturnValue_Inner_Underlying;
		static const UECodeGen_Private::FEnumPropertyParams NewProp_ReturnValue_Inner;
		static const UECodeGen_Private::FArrayPropertyParams NewProp_ReturnValue;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetSupportedDLSSGModes_Statics::NewProp_ReturnValue_Inner_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetSupportedDLSSGModes_Statics::NewProp_ReturnValue_Inner = { "ReturnValue", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UEnum_StreamlineBlueprint_UStreamlineDLSSGMode, METADATA_PARAMS(0, nullptr) }; // 3802091761
	const UECodeGen_Private::FArrayPropertyParams Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetSupportedDLSSGModes_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(StreamlineLibraryDLSSG_eventGetSupportedDLSSGModes_Parms, ReturnValue), EArrayPropertyFlags::None, METADATA_PARAMS(0, nullptr) }; // 3802091761
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetSupportedDLSSGModes_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetSupportedDLSSGModes_Statics::NewProp_ReturnValue_Inner_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetSupportedDLSSGModes_Statics::NewProp_ReturnValue_Inner,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetSupportedDLSSGModes_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetSupportedDLSSGModes_Statics::Function_MetaDataParams[] = {
		{ "Category", "Streamline|DLSS-G" },
		{ "Comment", "/** Retrieves all supported DLSS-G modes. Can be used to populate UI */" },
		{ "DisplayName", "Get Supported DLSS-G Modes" },
		{ "ModuleRelativePath", "Public/StreamlineLibraryDLSSG.h" },
		{ "ToolTip", "Retrieves all supported DLSS-G modes. Can be used to populate UI" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetSupportedDLSSGModes_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UStreamlineLibraryDLSSG, nullptr, "GetSupportedDLSSGModes", nullptr, nullptr, Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetSupportedDLSSGModes_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetSupportedDLSSGModes_Statics::PropPointers), sizeof(Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetSupportedDLSSGModes_Statics::StreamlineLibraryDLSSG_eventGetSupportedDLSSGModes_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x14022403, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetSupportedDLSSGModes_Statics::Function_MetaDataParams), Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetSupportedDLSSGModes_Statics::Function_MetaDataParams) };
	static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetSupportedDLSSGModes_Statics::PropPointers) < 2048);
	static_assert(sizeof(Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetSupportedDLSSGModes_Statics::StreamlineLibraryDLSSG_eventGetSupportedDLSSGModes_Parms) < MAX_uint16);
	UFunction* Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetSupportedDLSSGModes()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetSupportedDLSSGModes_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGModeSupported_Statics
	{
		struct StreamlineLibraryDLSSG_eventIsDLSSGModeSupported_Parms
		{
			UStreamlineDLSSGMode DLSSGMode;
			bool ReturnValue;
		};
		static const UECodeGen_Private::FBytePropertyParams NewProp_DLSSGMode_Underlying;
		static const UECodeGen_Private::FEnumPropertyParams NewProp_DLSSGMode;
		static void NewProp_ReturnValue_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGModeSupported_Statics::NewProp_DLSSGMode_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGModeSupported_Statics::NewProp_DLSSGMode = { "DLSSGMode", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(StreamlineLibraryDLSSG_eventIsDLSSGModeSupported_Parms, DLSSGMode), Z_Construct_UEnum_StreamlineBlueprint_UStreamlineDLSSGMode, METADATA_PARAMS(0, nullptr) }; // 3802091761
	void Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGModeSupported_Statics::NewProp_ReturnValue_SetBit(void* Obj)
	{
		((StreamlineLibraryDLSSG_eventIsDLSSGModeSupported_Parms*)Obj)->ReturnValue = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGModeSupported_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(StreamlineLibraryDLSSG_eventIsDLSSGModeSupported_Parms), &Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGModeSupported_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(0, nullptr) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGModeSupported_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGModeSupported_Statics::NewProp_DLSSGMode_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGModeSupported_Statics::NewProp_DLSSGMode,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGModeSupported_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGModeSupported_Statics::Function_MetaDataParams[] = {
		{ "Category", "Streamline|DLSS-G" },
		{ "Comment", "/** Checks whether a DLSS-G mode is supported */" },
		{ "DisplayName", "Is DLSS-G Mode Supported" },
		{ "ModuleRelativePath", "Public/StreamlineLibraryDLSSG.h" },
		{ "ToolTip", "Checks whether a DLSS-G mode is supported" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGModeSupported_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UStreamlineLibraryDLSSG, nullptr, "IsDLSSGModeSupported", nullptr, nullptr, Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGModeSupported_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGModeSupported_Statics::PropPointers), sizeof(Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGModeSupported_Statics::StreamlineLibraryDLSSG_eventIsDLSSGModeSupported_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x14022403, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGModeSupported_Statics::Function_MetaDataParams), Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGModeSupported_Statics::Function_MetaDataParams) };
	static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGModeSupported_Statics::PropPointers) < 2048);
	static_assert(sizeof(Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGModeSupported_Statics::StreamlineLibraryDLSSG_eventIsDLSSGModeSupported_Parms) < MAX_uint16);
	UFunction* Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGModeSupported()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGModeSupported_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGSupported_Statics
	{
		struct StreamlineLibraryDLSSG_eventIsDLSSGSupported_Parms
		{
			bool ReturnValue;
		};
		static void NewProp_ReturnValue_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	void Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGSupported_Statics::NewProp_ReturnValue_SetBit(void* Obj)
	{
		((StreamlineLibraryDLSSG_eventIsDLSSGSupported_Parms*)Obj)->ReturnValue = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGSupported_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(StreamlineLibraryDLSSG_eventIsDLSSGSupported_Parms), &Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGSupported_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(0, nullptr) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGSupported_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGSupported_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGSupported_Statics::Function_MetaDataParams[] = {
		{ "Category", "Streamline|DLSS-G" },
		{ "Comment", "/** Checks whether DLSS-G is supported by the current GPU. Further details can be retrieved via QueryDLSSGSupport*/" },
		{ "DisplayName", "Is NVIDIA DLSS-G Supported" },
		{ "ModuleRelativePath", "Public/StreamlineLibraryDLSSG.h" },
		{ "ToolTip", "Checks whether DLSS-G is supported by the current GPU. Further details can be retrieved via QueryDLSSGSupport" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGSupported_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UStreamlineLibraryDLSSG, nullptr, "IsDLSSGSupported", nullptr, nullptr, Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGSupported_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGSupported_Statics::PropPointers), sizeof(Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGSupported_Statics::StreamlineLibraryDLSSG_eventIsDLSSGSupported_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x14022403, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGSupported_Statics::Function_MetaDataParams), Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGSupported_Statics::Function_MetaDataParams) };
	static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGSupported_Statics::PropPointers) < 2048);
	static_assert(sizeof(Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGSupported_Statics::StreamlineLibraryDLSSG_eventIsDLSSGSupported_Parms) < MAX_uint16);
	UFunction* Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGSupported()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGSupported_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UStreamlineLibraryDLSSG_QueryDLSSGSupport_Statics
	{
		struct StreamlineLibraryDLSSG_eventQueryDLSSGSupport_Parms
		{
			UStreamlineFeatureSupport ReturnValue;
		};
		static const UECodeGen_Private::FBytePropertyParams NewProp_ReturnValue_Underlying;
		static const UECodeGen_Private::FEnumPropertyParams NewProp_ReturnValue;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UFunction_UStreamlineLibraryDLSSG_QueryDLSSGSupport_Statics::NewProp_ReturnValue_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UFunction_UStreamlineLibraryDLSSG_QueryDLSSGSupport_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(StreamlineLibraryDLSSG_eventQueryDLSSGSupport_Parms, ReturnValue), Z_Construct_UEnum_StreamlineBlueprint_UStreamlineFeatureSupport, METADATA_PARAMS(0, nullptr) }; // 633177639
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UStreamlineLibraryDLSSG_QueryDLSSGSupport_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UStreamlineLibraryDLSSG_QueryDLSSGSupport_Statics::NewProp_ReturnValue_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UStreamlineLibraryDLSSG_QueryDLSSGSupport_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UStreamlineLibraryDLSSG_QueryDLSSGSupport_Statics::Function_MetaDataParams[] = {
		{ "Category", "Streamline|DLSS-G" },
		{ "Comment", "/** Checks whether DLSS-G is supported by the current GPU\x09*/" },
		{ "DisplayName", "Query NVIDIA DLSS-G Support" },
		{ "ModuleRelativePath", "Public/StreamlineLibraryDLSSG.h" },
		{ "ToolTip", "Checks whether DLSS-G is supported by the current GPU" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UStreamlineLibraryDLSSG_QueryDLSSGSupport_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UStreamlineLibraryDLSSG, nullptr, "QueryDLSSGSupport", nullptr, nullptr, Z_Construct_UFunction_UStreamlineLibraryDLSSG_QueryDLSSGSupport_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UStreamlineLibraryDLSSG_QueryDLSSGSupport_Statics::PropPointers), sizeof(Z_Construct_UFunction_UStreamlineLibraryDLSSG_QueryDLSSGSupport_Statics::StreamlineLibraryDLSSG_eventQueryDLSSGSupport_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x14022403, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UStreamlineLibraryDLSSG_QueryDLSSGSupport_Statics::Function_MetaDataParams), Z_Construct_UFunction_UStreamlineLibraryDLSSG_QueryDLSSGSupport_Statics::Function_MetaDataParams) };
	static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UStreamlineLibraryDLSSG_QueryDLSSGSupport_Statics::PropPointers) < 2048);
	static_assert(sizeof(Z_Construct_UFunction_UStreamlineLibraryDLSSG_QueryDLSSGSupport_Statics::StreamlineLibraryDLSSG_eventQueryDLSSGSupport_Parms) < MAX_uint16);
	UFunction* Z_Construct_UFunction_UStreamlineLibraryDLSSG_QueryDLSSGSupport()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UStreamlineLibraryDLSSG_QueryDLSSGSupport_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UStreamlineLibraryDLSSG_SetDLSSGMode_Statics
	{
		struct StreamlineLibraryDLSSG_eventSetDLSSGMode_Parms
		{
			UStreamlineDLSSGMode DLSSGMode;
		};
		static const UECodeGen_Private::FBytePropertyParams NewProp_DLSSGMode_Underlying;
		static const UECodeGen_Private::FEnumPropertyParams NewProp_DLSSGMode;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UFunction_UStreamlineLibraryDLSSG_SetDLSSGMode_Statics::NewProp_DLSSGMode_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UFunction_UStreamlineLibraryDLSSG_SetDLSSGMode_Statics::NewProp_DLSSGMode = { "DLSSGMode", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(StreamlineLibraryDLSSG_eventSetDLSSGMode_Parms, DLSSGMode), Z_Construct_UEnum_StreamlineBlueprint_UStreamlineDLSSGMode, METADATA_PARAMS(0, nullptr) }; // 3802091761
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UStreamlineLibraryDLSSG_SetDLSSGMode_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UStreamlineLibraryDLSSG_SetDLSSGMode_Statics::NewProp_DLSSGMode_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UStreamlineLibraryDLSSG_SetDLSSGMode_Statics::NewProp_DLSSGMode,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UStreamlineLibraryDLSSG_SetDLSSGMode_Statics::Function_MetaDataParams[] = {
		{ "Category", "Streamline|DLSS-G" },
		{ "Comment", "/** Sets the console variables to enable/disable DLSS-G*/" },
		{ "DisplayName", "Set DLSS-G Mode" },
		{ "ModuleRelativePath", "Public/StreamlineLibraryDLSSG.h" },
		{ "ToolTip", "Sets the console variables to enable/disable DLSS-G" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UStreamlineLibraryDLSSG_SetDLSSGMode_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UStreamlineLibraryDLSSG, nullptr, "SetDLSSGMode", nullptr, nullptr, Z_Construct_UFunction_UStreamlineLibraryDLSSG_SetDLSSGMode_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UStreamlineLibraryDLSSG_SetDLSSGMode_Statics::PropPointers), sizeof(Z_Construct_UFunction_UStreamlineLibraryDLSSG_SetDLSSGMode_Statics::StreamlineLibraryDLSSG_eventSetDLSSGMode_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04022403, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UStreamlineLibraryDLSSG_SetDLSSGMode_Statics::Function_MetaDataParams), Z_Construct_UFunction_UStreamlineLibraryDLSSG_SetDLSSGMode_Statics::Function_MetaDataParams) };
	static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UStreamlineLibraryDLSSG_SetDLSSGMode_Statics::PropPointers) < 2048);
	static_assert(sizeof(Z_Construct_UFunction_UStreamlineLibraryDLSSG_SetDLSSGMode_Statics::StreamlineLibraryDLSSG_eventSetDLSSGMode_Parms) < MAX_uint16);
	UFunction* Z_Construct_UFunction_UStreamlineLibraryDLSSG_SetDLSSGMode()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UStreamlineLibraryDLSSG_SetDLSSGMode_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UStreamlineLibraryDLSSG);
	UClass* Z_Construct_UClass_UStreamlineLibraryDLSSG_NoRegister()
	{
		return UStreamlineLibraryDLSSG::StaticClass();
	}
	struct Z_Construct_UClass_UStreamlineLibraryDLSSG_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UStreamlineLibraryDLSSG_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UBlueprintFunctionLibrary,
		(UObject* (*)())Z_Construct_UPackage__Script_StreamlineBlueprint,
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UStreamlineLibraryDLSSG_Statics::DependentSingletons) < 16);
	const FClassFunctionLinkInfo Z_Construct_UClass_UStreamlineLibraryDLSSG_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDefaultDLSSGMode, "GetDefaultDLSSGMode" }, // 2744496490
		{ &Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGFrameTiming, "GetDLSSGFrameTiming" }, // 3790072427
		{ &Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetDLSSGMode, "GetDLSSGMode" }, // 2557170433
		{ &Z_Construct_UFunction_UStreamlineLibraryDLSSG_GetSupportedDLSSGModes, "GetSupportedDLSSGModes" }, // 3322017455
		{ &Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGModeSupported, "IsDLSSGModeSupported" }, // 1805038374
		{ &Z_Construct_UFunction_UStreamlineLibraryDLSSG_IsDLSSGSupported, "IsDLSSGSupported" }, // 1181849023
		{ &Z_Construct_UFunction_UStreamlineLibraryDLSSG_QueryDLSSGSupport, "QueryDLSSGSupport" }, // 3802260533
		{ &Z_Construct_UFunction_UStreamlineLibraryDLSSG_SetDLSSGMode, "SetDLSSGMode" }, // 586487283
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UStreamlineLibraryDLSSG_Statics::FuncInfo) < 2048);
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UStreamlineLibraryDLSSG_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "StreamlineLibraryDLSSG.h" },
		{ "ModuleRelativePath", "Public/StreamlineLibraryDLSSG.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UStreamlineLibraryDLSSG_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UStreamlineLibraryDLSSG>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UStreamlineLibraryDLSSG_Statics::ClassParams = {
		&UStreamlineLibraryDLSSG::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		UE_ARRAY_COUNT(FuncInfo),
		0,
		0,
		0x000800A0u,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UStreamlineLibraryDLSSG_Statics::Class_MetaDataParams), Z_Construct_UClass_UStreamlineLibraryDLSSG_Statics::Class_MetaDataParams)
	};
	UClass* Z_Construct_UClass_UStreamlineLibraryDLSSG()
	{
		if (!Z_Registration_Info_UClass_UStreamlineLibraryDLSSG.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UStreamlineLibraryDLSSG.OuterSingleton, Z_Construct_UClass_UStreamlineLibraryDLSSG_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UStreamlineLibraryDLSSG.OuterSingleton;
	}
	template<> STREAMLINEBLUEPRINT_API UClass* StaticClass<UStreamlineLibraryDLSSG>()
	{
		return UStreamlineLibraryDLSSG::StaticClass();
	}
	UStreamlineLibraryDLSSG::UStreamlineLibraryDLSSG(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UStreamlineLibraryDLSSG);
	UStreamlineLibraryDLSSG::~UStreamlineLibraryDLSSG() {}
	struct Z_CompiledInDeferFile_FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryDLSSG_h_Statics
	{
		static const FEnumRegisterCompiledInInfo EnumInfo[];
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FEnumRegisterCompiledInInfo Z_CompiledInDeferFile_FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryDLSSG_h_Statics::EnumInfo[] = {
		{ UStreamlineDLSSGMode_StaticEnum, TEXT("UStreamlineDLSSGMode"), &Z_Registration_Info_UEnum_UStreamlineDLSSGMode, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 3802091761U) },
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryDLSSG_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UStreamlineLibraryDLSSG, UStreamlineLibraryDLSSG::StaticClass, TEXT("UStreamlineLibraryDLSSG"), &Z_Registration_Info_UClass_UStreamlineLibraryDLSSG, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UStreamlineLibraryDLSSG), 4067914418U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryDLSSG_h_3488579663(TEXT("/Script/StreamlineBlueprint"),
		Z_CompiledInDeferFile_FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryDLSSG_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryDLSSG_h_Statics::ClassInfo),
		nullptr, 0,
		Z_CompiledInDeferFile_FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryDLSSG_h_Statics::EnumInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID__stage_plugin_Plugins_Streamline_HostProject_Plugins_Streamline_Source_StreamlineBlueprint_Public_StreamlineLibraryDLSSG_h_Statics::EnumInfo));
PRAGMA_ENABLE_DEPRECATION_WARNINGS
