// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "DLSS/Public/DLSSSettings.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeDLSSSettings() {}
// Cross Module References
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
	DLSS_API UClass* Z_Construct_UClass_UDLSSOverrideSettings();
	DLSS_API UClass* Z_Construct_UClass_UDLSSOverrideSettings_NoRegister();
	DLSS_API UClass* Z_Construct_UClass_UDLSSSettings();
	DLSS_API UClass* Z_Construct_UClass_UDLSSSettings_NoRegister();
	DLSS_API UEnum* Z_Construct_UEnum_DLSS_EDLSSPreset();
	DLSS_API UEnum* Z_Construct_UEnum_DLSS_EDLSSSettingOverride();
	UPackage* Z_Construct_UPackage__Script_DLSS();
// End Cross Module References
	static FEnumRegistrationInfo Z_Registration_Info_UEnum_EDLSSSettingOverride;
	static UEnum* EDLSSSettingOverride_StaticEnum()
	{
		if (!Z_Registration_Info_UEnum_EDLSSSettingOverride.OuterSingleton)
		{
			Z_Registration_Info_UEnum_EDLSSSettingOverride.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_DLSS_EDLSSSettingOverride, (UObject*)Z_Construct_UPackage__Script_DLSS(), TEXT("EDLSSSettingOverride"));
		}
		return Z_Registration_Info_UEnum_EDLSSSettingOverride.OuterSingleton;
	}
	template<> DLSS_API UEnum* StaticEnum<EDLSSSettingOverride>()
	{
		return EDLSSSettingOverride_StaticEnum();
	}
	struct Z_Construct_UEnum_DLSS_EDLSSSettingOverride_Statics
	{
		static const UECodeGen_Private::FEnumeratorParam Enumerators[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[];
#endif
		static const UECodeGen_Private::FEnumParams EnumParams;
	};
	const UECodeGen_Private::FEnumeratorParam Z_Construct_UEnum_DLSS_EDLSSSettingOverride_Statics::Enumerators[] = {
		{ "EDLSSSettingOverride::Enabled", (int64)EDLSSSettingOverride::Enabled },
		{ "EDLSSSettingOverride::Disabled", (int64)EDLSSSettingOverride::Disabled },
		{ "EDLSSSettingOverride::UseProjectSettings", (int64)EDLSSSettingOverride::UseProjectSettings },
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UEnum_DLSS_EDLSSSettingOverride_Statics::Enum_MetaDataParams[] = {
		{ "Disabled.DisplayName", "False" },
		{ "Disabled.Name", "EDLSSSettingOverride::Disabled" },
		{ "Enabled.DisplayName", "True" },
		{ "Enabled.Name", "EDLSSSettingOverride::Enabled" },
		{ "ModuleRelativePath", "Public/DLSSSettings.h" },
		{ "UseProjectSettings.DisplayName", "Use project settings" },
		{ "UseProjectSettings.Name", "EDLSSSettingOverride::UseProjectSettings" },
	};
#endif
	const UECodeGen_Private::FEnumParams Z_Construct_UEnum_DLSS_EDLSSSettingOverride_Statics::EnumParams = {
		(UObject*(*)())Z_Construct_UPackage__Script_DLSS,
		nullptr,
		"EDLSSSettingOverride",
		"EDLSSSettingOverride",
		Z_Construct_UEnum_DLSS_EDLSSSettingOverride_Statics::Enumerators,
		RF_Public|RF_Transient|RF_MarkAsNative,
		UE_ARRAY_COUNT(Z_Construct_UEnum_DLSS_EDLSSSettingOverride_Statics::Enumerators),
		EEnumFlags::None,
		(uint8)UEnum::ECppForm::EnumClass,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UEnum_DLSS_EDLSSSettingOverride_Statics::Enum_MetaDataParams), Z_Construct_UEnum_DLSS_EDLSSSettingOverride_Statics::Enum_MetaDataParams)
	};
	UEnum* Z_Construct_UEnum_DLSS_EDLSSSettingOverride()
	{
		if (!Z_Registration_Info_UEnum_EDLSSSettingOverride.InnerSingleton)
		{
			UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_EDLSSSettingOverride.InnerSingleton, Z_Construct_UEnum_DLSS_EDLSSSettingOverride_Statics::EnumParams);
		}
		return Z_Registration_Info_UEnum_EDLSSSettingOverride.InnerSingleton;
	}
	static FEnumRegistrationInfo Z_Registration_Info_UEnum_EDLSSPreset;
	static UEnum* EDLSSPreset_StaticEnum()
	{
		if (!Z_Registration_Info_UEnum_EDLSSPreset.OuterSingleton)
		{
			Z_Registration_Info_UEnum_EDLSSPreset.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_DLSS_EDLSSPreset, (UObject*)Z_Construct_UPackage__Script_DLSS(), TEXT("EDLSSPreset"));
		}
		return Z_Registration_Info_UEnum_EDLSSPreset.OuterSingleton;
	}
	template<> DLSS_API UEnum* StaticEnum<EDLSSPreset>()
	{
		return EDLSSPreset_StaticEnum();
	}
	struct Z_Construct_UEnum_DLSS_EDLSSPreset_Statics
	{
		static const UECodeGen_Private::FEnumeratorParam Enumerators[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[];
#endif
		static const UECodeGen_Private::FEnumParams EnumParams;
	};
	const UECodeGen_Private::FEnumeratorParam Z_Construct_UEnum_DLSS_EDLSSPreset_Statics::Enumerators[] = {
		{ "EDLSSPreset::Default", (int64)EDLSSPreset::Default },
		{ "EDLSSPreset::A", (int64)EDLSSPreset::A },
		{ "EDLSSPreset::B", (int64)EDLSSPreset::B },
		{ "EDLSSPreset::C", (int64)EDLSSPreset::C },
		{ "EDLSSPreset::D", (int64)EDLSSPreset::D },
		{ "EDLSSPreset::E", (int64)EDLSSPreset::E },
		{ "EDLSSPreset::F", (int64)EDLSSPreset::F },
		{ "EDLSSPreset::G", (int64)EDLSSPreset::G },
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UEnum_DLSS_EDLSSPreset_Statics::Enum_MetaDataParams[] = {
		{ "A.Name", "EDLSSPreset::A" },
		{ "A.ToolTip", "force preset A" },
		{ "B.Name", "EDLSSPreset::B" },
		{ "B.ToolTip", "force preset B" },
		{ "C.Name", "EDLSSPreset::C" },
		{ "C.ToolTip", "force preset C" },
		{ "D.Name", "EDLSSPreset::D" },
		{ "D.ToolTip", "force preset D" },
		{ "Default.Name", "EDLSSPreset::Default" },
		{ "Default.ToolTip", "default behavior, preset specified per DLSS SDK release" },
		{ "E.Name", "EDLSSPreset::E" },
		{ "E.ToolTip", "force preset E" },
		{ "F.Name", "EDLSSPreset::F" },
		{ "F.ToolTip", "force preset F" },
		{ "G.Name", "EDLSSPreset::G" },
		{ "G.ToolTip", "force preset G" },
		{ "ModuleRelativePath", "Public/DLSSSettings.h" },
	};
#endif
	const UECodeGen_Private::FEnumParams Z_Construct_UEnum_DLSS_EDLSSPreset_Statics::EnumParams = {
		(UObject*(*)())Z_Construct_UPackage__Script_DLSS,
		nullptr,
		"EDLSSPreset",
		"EDLSSPreset",
		Z_Construct_UEnum_DLSS_EDLSSPreset_Statics::Enumerators,
		RF_Public|RF_Transient|RF_MarkAsNative,
		UE_ARRAY_COUNT(Z_Construct_UEnum_DLSS_EDLSSPreset_Statics::Enumerators),
		EEnumFlags::None,
		(uint8)UEnum::ECppForm::EnumClass,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UEnum_DLSS_EDLSSPreset_Statics::Enum_MetaDataParams), Z_Construct_UEnum_DLSS_EDLSSPreset_Statics::Enum_MetaDataParams)
	};
	UEnum* Z_Construct_UEnum_DLSS_EDLSSPreset()
	{
		if (!Z_Registration_Info_UEnum_EDLSSPreset.InnerSingleton)
		{
			UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_EDLSSPreset.InnerSingleton, Z_Construct_UEnum_DLSS_EDLSSPreset_Statics::EnumParams);
		}
		return Z_Registration_Info_UEnum_EDLSSPreset.InnerSingleton;
	}
	void UDLSSOverrideSettings::StaticRegisterNativesUDLSSOverrideSettings()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UDLSSOverrideSettings);
	UClass* Z_Construct_UClass_UDLSSOverrideSettings_NoRegister()
	{
		return UDLSSOverrideSettings::StaticClass();
	}
	struct Z_Construct_UClass_UDLSSOverrideSettings_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const UECodeGen_Private::FBytePropertyParams NewProp_EnableDLSSInEditorViewportsOverride_Underlying;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_EnableDLSSInEditorViewportsOverride_MetaData[];
#endif
		static const UECodeGen_Private::FEnumPropertyParams NewProp_EnableDLSSInEditorViewportsOverride;
		static const UECodeGen_Private::FBytePropertyParams NewProp_EnableDLSSInPlayInEditorViewportsOverride_Underlying;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_EnableDLSSInPlayInEditorViewportsOverride_MetaData[];
#endif
		static const UECodeGen_Private::FEnumPropertyParams NewProp_EnableDLSSInPlayInEditorViewportsOverride;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bShowDLSSIncompatiblePluginsToolsWarnings_MetaData[];
#endif
		static void NewProp_bShowDLSSIncompatiblePluginsToolsWarnings_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bShowDLSSIncompatiblePluginsToolsWarnings;
		static const UECodeGen_Private::FBytePropertyParams NewProp_ShowDLSSSDebugOnScreenMessages_Underlying;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ShowDLSSSDebugOnScreenMessages_MetaData[];
#endif
		static const UECodeGen_Private::FEnumPropertyParams NewProp_ShowDLSSSDebugOnScreenMessages;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UDLSSOverrideSettings_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UObject,
		(UObject* (*)())Z_Construct_UPackage__Script_DLSS,
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSOverrideSettings_Statics::DependentSingletons) < 16);
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDLSSOverrideSettings_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "DLSSSettings.h" },
		{ "ModuleRelativePath", "Public/DLSSSettings.h" },
	};
#endif
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_EnableDLSSInEditorViewportsOverride_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_EnableDLSSInEditorViewportsOverride_MetaData[] = {
		{ "Category", "Level Editor - Viewport (Local)" },
		{ "Comment", "/** This enables DLSS/DLAA in editor viewports. Saved to local user config only.*/" },
		{ "DisplayName", "Enable DLSS/DLAA to be turned on in Editor viewports" },
		{ "ModuleRelativePath", "Public/DLSSSettings.h" },
		{ "ToolTip", "This enables DLSS/DLAA in editor viewports. Saved to local user config only." },
	};
#endif
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_EnableDLSSInEditorViewportsOverride = { "EnableDLSSInEditorViewportsOverride", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDLSSOverrideSettings, EnableDLSSInEditorViewportsOverride), Z_Construct_UEnum_DLSS_EDLSSSettingOverride, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_EnableDLSSInEditorViewportsOverride_MetaData), Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_EnableDLSSInEditorViewportsOverride_MetaData) }; // 3368560862
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_EnableDLSSInPlayInEditorViewportsOverride_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_EnableDLSSInPlayInEditorViewportsOverride_MetaData[] = {
		{ "Category", "Level Editor - Viewport (Local)" },
		{ "Comment", "/** This enables DLSS/DLAA in play in editor viewports. Saved to local user config only. */" },
		{ "DisplayName", "Enable DLSS/DLAA in Play In Editor viewports" },
		{ "ModuleRelativePath", "Public/DLSSSettings.h" },
		{ "ToolTip", "This enables DLSS/DLAA in play in editor viewports. Saved to local user config only." },
	};
#endif
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_EnableDLSSInPlayInEditorViewportsOverride = { "EnableDLSSInPlayInEditorViewportsOverride", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDLSSOverrideSettings, EnableDLSSInPlayInEditorViewportsOverride), Z_Construct_UEnum_DLSS_EDLSSSettingOverride, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_EnableDLSSInPlayInEditorViewportsOverride_MetaData), Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_EnableDLSSInPlayInEditorViewportsOverride_MetaData) }; // 3368560862
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_bShowDLSSIncompatiblePluginsToolsWarnings_MetaData[] = {
		{ "Category", "Editor (Local)" },
		{ "Comment", "/** This enables warnings about plugins & tools that are incompatible with DLSS/DLAA in the editor. This setting and the project setting both must be set to get warnings */" },
		{ "DisplayName", "Warn about incompatible plugins and tools" },
		{ "ModuleRelativePath", "Public/DLSSSettings.h" },
		{ "ToolTip", "This enables warnings about plugins & tools that are incompatible with DLSS/DLAA in the editor. This setting and the project setting both must be set to get warnings" },
	};
#endif
	void Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_bShowDLSSIncompatiblePluginsToolsWarnings_SetBit(void* Obj)
	{
		((UDLSSOverrideSettings*)Obj)->bShowDLSSIncompatiblePluginsToolsWarnings = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_bShowDLSSIncompatiblePluginsToolsWarnings = { "bShowDLSSIncompatiblePluginsToolsWarnings", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UDLSSOverrideSettings), &Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_bShowDLSSIncompatiblePluginsToolsWarnings_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_bShowDLSSIncompatiblePluginsToolsWarnings_MetaData), Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_bShowDLSSIncompatiblePluginsToolsWarnings_MetaData) };
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_ShowDLSSSDebugOnScreenMessages_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_ShowDLSSSDebugOnScreenMessages_MetaData[] = {
		{ "Category", "Editor (Local)" },
		{ "Comment", "/** This enables on screen warnings and errors about DLSS/DLAA  */" },
		{ "DisplayName", "Show various DLSS/DLAA on screen debug messages" },
		{ "ModuleRelativePath", "Public/DLSSSettings.h" },
		{ "ToolTip", "This enables on screen warnings and errors about DLSS/DLAA" },
	};
#endif
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_ShowDLSSSDebugOnScreenMessages = { "ShowDLSSSDebugOnScreenMessages", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDLSSOverrideSettings, ShowDLSSSDebugOnScreenMessages), Z_Construct_UEnum_DLSS_EDLSSSettingOverride, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_ShowDLSSSDebugOnScreenMessages_MetaData), Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_ShowDLSSSDebugOnScreenMessages_MetaData) }; // 3368560862
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UDLSSOverrideSettings_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_EnableDLSSInEditorViewportsOverride_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_EnableDLSSInEditorViewportsOverride,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_EnableDLSSInPlayInEditorViewportsOverride_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_EnableDLSSInPlayInEditorViewportsOverride,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_bShowDLSSIncompatiblePluginsToolsWarnings,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_ShowDLSSSDebugOnScreenMessages_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSOverrideSettings_Statics::NewProp_ShowDLSSSDebugOnScreenMessages,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UDLSSOverrideSettings_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UDLSSOverrideSettings>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UDLSSOverrideSettings_Statics::ClassParams = {
		&UDLSSOverrideSettings::StaticClass,
		"Engine",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UDLSSOverrideSettings_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSOverrideSettings_Statics::PropPointers),
		0,
		0x001000E4u,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSOverrideSettings_Statics::Class_MetaDataParams), Z_Construct_UClass_UDLSSOverrideSettings_Statics::Class_MetaDataParams)
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSOverrideSettings_Statics::PropPointers) < 2048);
	UClass* Z_Construct_UClass_UDLSSOverrideSettings()
	{
		if (!Z_Registration_Info_UClass_UDLSSOverrideSettings.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UDLSSOverrideSettings.OuterSingleton, Z_Construct_UClass_UDLSSOverrideSettings_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UDLSSOverrideSettings.OuterSingleton;
	}
	template<> DLSS_API UClass* StaticClass<UDLSSOverrideSettings>()
	{
		return UDLSSOverrideSettings::StaticClass();
	}
	UDLSSOverrideSettings::UDLSSOverrideSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UDLSSOverrideSettings);
	UDLSSOverrideSettings::~UDLSSOverrideSettings() {}
	void UDLSSSettings::StaticRegisterNativesUDLSSSettings()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UDLSSSettings);
	UClass* Z_Construct_UClass_UDLSSSettings_NoRegister()
	{
		return UDLSSSettings::StaticClass();
	}
	struct Z_Construct_UClass_UDLSSSettings_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bEnableDLSSD3D12_MetaData[];
#endif
		static void NewProp_bEnableDLSSD3D12_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bEnableDLSSD3D12;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bEnableDLSSD3D11_MetaData[];
#endif
		static void NewProp_bEnableDLSSD3D11_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bEnableDLSSD3D11;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bEnableDLSSVulkan_MetaData[];
#endif
		static void NewProp_bEnableDLSSVulkan_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bEnableDLSSVulkan;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bEnableDLSSInEditorViewports_MetaData[];
#endif
		static void NewProp_bEnableDLSSInEditorViewports_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bEnableDLSSInEditorViewports;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bEnableDLSSInPlayInEditorViewports_MetaData[];
#endif
		static void NewProp_bEnableDLSSInPlayInEditorViewports_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bEnableDLSSInPlayInEditorViewports;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bShowDLSSSDebugOnScreenMessages_MetaData[];
#endif
		static void NewProp_bShowDLSSSDebugOnScreenMessages_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bShowDLSSSDebugOnScreenMessages;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_GenericDLSSBinaryPath_MetaData[];
#endif
		static const UECodeGen_Private::FStrPropertyParams NewProp_GenericDLSSBinaryPath;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bGenericDLSSBinaryExists_MetaData[];
#endif
		static void NewProp_bGenericDLSSBinaryExists_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bGenericDLSSBinaryExists;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_NVIDIANGXApplicationId_MetaData[];
#endif
		static const UECodeGen_Private::FUInt32PropertyParams NewProp_NVIDIANGXApplicationId;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_CustomDLSSBinaryPath_MetaData[];
#endif
		static const UECodeGen_Private::FStrPropertyParams NewProp_CustomDLSSBinaryPath;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bCustomDLSSBinaryExists_MetaData[];
#endif
		static void NewProp_bCustomDLSSBinaryExists_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bCustomDLSSBinaryExists;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bAllowOTAUpdate_MetaData[];
#endif
		static void NewProp_bAllowOTAUpdate_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bAllowOTAUpdate;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bShowDLSSIncompatiblePluginsToolsWarnings_MetaData[];
#endif
		static void NewProp_bShowDLSSIncompatiblePluginsToolsWarnings_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bShowDLSSIncompatiblePluginsToolsWarnings;
		static const UECodeGen_Private::FBytePropertyParams NewProp_DLAAPreset_Underlying;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_DLAAPreset_MetaData[];
#endif
		static const UECodeGen_Private::FEnumPropertyParams NewProp_DLAAPreset;
		static const UECodeGen_Private::FBytePropertyParams NewProp_DLSSQualityPreset_Underlying;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_DLSSQualityPreset_MetaData[];
#endif
		static const UECodeGen_Private::FEnumPropertyParams NewProp_DLSSQualityPreset;
		static const UECodeGen_Private::FBytePropertyParams NewProp_DLSSBalancedPreset_Underlying;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_DLSSBalancedPreset_MetaData[];
#endif
		static const UECodeGen_Private::FEnumPropertyParams NewProp_DLSSBalancedPreset;
		static const UECodeGen_Private::FBytePropertyParams NewProp_DLSSPerformancePreset_Underlying;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_DLSSPerformancePreset_MetaData[];
#endif
		static const UECodeGen_Private::FEnumPropertyParams NewProp_DLSSPerformancePreset;
		static const UECodeGen_Private::FBytePropertyParams NewProp_DLSSUltraPerformancePreset_Underlying;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_DLSSUltraPerformancePreset_MetaData[];
#endif
		static const UECodeGen_Private::FEnumPropertyParams NewProp_DLSSUltraPerformancePreset;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UDLSSSettings_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UObject,
		(UObject* (*)())Z_Construct_UPackage__Script_DLSS,
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSSettings_Statics::DependentSingletons) < 16);
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDLSSSettings_Statics::Class_MetaDataParams[] = {
		{ "DisplayName", "NVIDIA DLSS" },
		{ "IncludePath", "DLSSSettings.h" },
		{ "ModuleRelativePath", "Public/DLSSSettings.h" },
	};
#endif
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSD3D12_MetaData[] = {
		{ "Category", "Platforms" },
		{ "Comment", "/** Enable DLSS/DLAA for D3D12, if the driver supports it at runtime */" },
		{ "DisplayName", "Enable DLSS/DLAA for the D3D12RHI" },
		{ "ModuleRelativePath", "Public/DLSSSettings.h" },
		{ "ToolTip", "Enable DLSS/DLAA for D3D12, if the driver supports it at runtime" },
	};
#endif
	void Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSD3D12_SetBit(void* Obj)
	{
		((UDLSSSettings*)Obj)->bEnableDLSSD3D12 = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSD3D12 = { "bEnableDLSSD3D12", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UDLSSSettings), &Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSD3D12_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSD3D12_MetaData), Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSD3D12_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSD3D11_MetaData[] = {
		{ "Category", "Platforms" },
		{ "Comment", "/** Enable DLSS/DLAA for D3D11, if the driver supports it at runtime */" },
		{ "DisplayName", "Enable DLSS/DLAA for the D3D11RHI" },
		{ "ModuleRelativePath", "Public/DLSSSettings.h" },
		{ "ToolTip", "Enable DLSS/DLAA for D3D11, if the driver supports it at runtime" },
	};
#endif
	void Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSD3D11_SetBit(void* Obj)
	{
		((UDLSSSettings*)Obj)->bEnableDLSSD3D11 = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSD3D11 = { "bEnableDLSSD3D11", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UDLSSSettings), &Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSD3D11_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSD3D11_MetaData), Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSD3D11_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSVulkan_MetaData[] = {
		{ "Category", "Platforms" },
		{ "Comment", "/** Enable DLSS/DLAA for Vulkan, if the driver supports it at runtime */" },
		{ "DisplayName", "Enable DLSS/DLAA for the VulkanRHI" },
		{ "ModuleRelativePath", "Public/DLSSSettings.h" },
		{ "ToolTip", "Enable DLSS/DLAA for Vulkan, if the driver supports it at runtime" },
	};
#endif
	void Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSVulkan_SetBit(void* Obj)
	{
		((UDLSSSettings*)Obj)->bEnableDLSSVulkan = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSVulkan = { "bEnableDLSSVulkan", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UDLSSSettings), &Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSVulkan_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSVulkan_MetaData), Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSVulkan_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSInEditorViewports_MetaData[] = {
		{ "Category", "Level Editor - Viewport" },
		{ "Comment", "/** This enables DLSS/DLAA in editor viewports. This project wide setting can be locally overridden in the NVIDIA DLSS (Local) settings.*/" },
		{ "DisplayName", "Enable DLSS/DLAA to be turned on in Editor viewports" },
		{ "ModuleRelativePath", "Public/DLSSSettings.h" },
		{ "ToolTip", "This enables DLSS/DLAA in editor viewports. This project wide setting can be locally overridden in the NVIDIA DLSS (Local) settings." },
	};
#endif
	void Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSInEditorViewports_SetBit(void* Obj)
	{
		((UDLSSSettings*)Obj)->bEnableDLSSInEditorViewports = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSInEditorViewports = { "bEnableDLSSInEditorViewports", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UDLSSSettings), &Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSInEditorViewports_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSInEditorViewports_MetaData), Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSInEditorViewports_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSInPlayInEditorViewports_MetaData[] = {
		{ "Category", "Level Editor - Viewport" },
		{ "Comment", "/** This enables DLSS/DLAA in play in editor viewports. This project wide setting can be locally overridden in in the NVIDIA DLSS (Local) settings.*/" },
		{ "DisplayName", "Enable DLSS/DLAA in Play In Editor viewports" },
		{ "ModuleRelativePath", "Public/DLSSSettings.h" },
		{ "ToolTip", "This enables DLSS/DLAA in play in editor viewports. This project wide setting can be locally overridden in in the NVIDIA DLSS (Local) settings." },
	};
#endif
	void Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSInPlayInEditorViewports_SetBit(void* Obj)
	{
		((UDLSSSettings*)Obj)->bEnableDLSSInPlayInEditorViewports = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSInPlayInEditorViewports = { "bEnableDLSSInPlayInEditorViewports", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UDLSSSettings), &Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSInPlayInEditorViewports_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSInPlayInEditorViewports_MetaData), Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSInPlayInEditorViewports_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bShowDLSSSDebugOnScreenMessages_MetaData[] = {
		{ "Category", "Level Editor - Viewport" },
		{ "Comment", "/** This enables on screen warnings and errors about DLSS/DLAA. This project wide setting can be locally overridden in the NVIDIA DLSS (Local) settings. */" },
		{ "DisplayName", "Show various DLSS/DLAA on screen debug messages" },
		{ "ModuleRelativePath", "Public/DLSSSettings.h" },
		{ "ToolTip", "This enables on screen warnings and errors about DLSS/DLAA. This project wide setting can be locally overridden in the NVIDIA DLSS (Local) settings." },
	};
#endif
	void Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bShowDLSSSDebugOnScreenMessages_SetBit(void* Obj)
	{
		((UDLSSSettings*)Obj)->bShowDLSSSDebugOnScreenMessages = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bShowDLSSSDebugOnScreenMessages = { "bShowDLSSSDebugOnScreenMessages", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UDLSSSettings), &Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bShowDLSSSDebugOnScreenMessages_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bShowDLSSSDebugOnScreenMessages_MetaData), Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bShowDLSSSDebugOnScreenMessages_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDLSSSettings_Statics::NewProp_GenericDLSSBinaryPath_MetaData[] = {
		{ "Category", "General Settings" },
		{ "Comment", "/** This is part of the DLSS plugin and used by most projects*/" },
		{ "DisplayName", "Generic DLSS Binary Path" },
		{ "ModuleRelativePath", "Public/DLSSSettings.h" },
		{ "ToolTip", "This is part of the DLSS plugin and used by most projects" },
	};
#endif
	const UECodeGen_Private::FStrPropertyParams Z_Construct_UClass_UDLSSSettings_Statics::NewProp_GenericDLSSBinaryPath = { "GenericDLSSBinaryPath", nullptr, (EPropertyFlags)0x0010000000024001, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDLSSSettings, GenericDLSSBinaryPath), METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSSettings_Statics::NewProp_GenericDLSSBinaryPath_MetaData), Z_Construct_UClass_UDLSSSettings_Statics::NewProp_GenericDLSSBinaryPath_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bGenericDLSSBinaryExists_MetaData[] = {
		{ "Category", "General Settings" },
		{ "DisplayName", "Exists" },
		{ "ModuleRelativePath", "Public/DLSSSettings.h" },
	};
#endif
	void Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bGenericDLSSBinaryExists_SetBit(void* Obj)
	{
		((UDLSSSettings*)Obj)->bGenericDLSSBinaryExists = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bGenericDLSSBinaryExists = { "bGenericDLSSBinaryExists", nullptr, (EPropertyFlags)0x0010000000024001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UDLSSSettings), &Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bGenericDLSSBinaryExists_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bGenericDLSSBinaryExists_MetaData), Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bGenericDLSSBinaryExists_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDLSSSettings_Statics::NewProp_NVIDIANGXApplicationId_MetaData[] = {
		{ "Category", "General Settings" },
		{ "Comment", "/** By default the DLSS plugin uses the UE Project ID to initialize DLSS. In some cases NVIDIA might provide a separate NVIDIA Application ID, which should be put here. Please refer to https://developer.nvidia.com/dlss for details*/" },
		{ "DisplayName", "NVIDIA NGX Application ID" },
		{ "ModuleRelativePath", "Public/DLSSSettings.h" },
		{ "ToolTip", "By default the DLSS plugin uses the UE Project ID to initialize DLSS. In some cases NVIDIA might provide a separate NVIDIA Application ID, which should be put here. Please refer to https://developer.nvidia.com/dlss for details" },
	};
#endif
	const UECodeGen_Private::FUInt32PropertyParams Z_Construct_UClass_UDLSSSettings_Statics::NewProp_NVIDIANGXApplicationId = { "NVIDIANGXApplicationId", nullptr, (EPropertyFlags)0x0010040000004001, UECodeGen_Private::EPropertyGenFlags::UInt32, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDLSSSettings, NVIDIANGXApplicationId), METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSSettings_Statics::NewProp_NVIDIANGXApplicationId_MetaData), Z_Construct_UClass_UDLSSSettings_Statics::NewProp_NVIDIANGXApplicationId_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDLSSSettings_Statics::NewProp_CustomDLSSBinaryPath_MetaData[] = {
		{ "Category", "General Settings" },
		{ "Comment", "/** In some cases NVIDIA might provide a project specific DLSS binary for your project. Please refer to https://developer.nvidia.com/dlss for details*/" },
		{ "DisplayName", "Custom DLSS Binary Path" },
		{ "ModuleRelativePath", "Public/DLSSSettings.h" },
		{ "ToolTip", "In some cases NVIDIA might provide a project specific DLSS binary for your project. Please refer to https://developer.nvidia.com/dlss for details" },
	};
#endif
	const UECodeGen_Private::FStrPropertyParams Z_Construct_UClass_UDLSSSettings_Statics::NewProp_CustomDLSSBinaryPath = { "CustomDLSSBinaryPath", nullptr, (EPropertyFlags)0x0010040000024001, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDLSSSettings, CustomDLSSBinaryPath), METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSSettings_Statics::NewProp_CustomDLSSBinaryPath_MetaData), Z_Construct_UClass_UDLSSSettings_Statics::NewProp_CustomDLSSBinaryPath_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bCustomDLSSBinaryExists_MetaData[] = {
		{ "Category", "General Settings" },
		{ "DisplayName", "Exists" },
		{ "ModuleRelativePath", "Public/DLSSSettings.h" },
	};
#endif
	void Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bCustomDLSSBinaryExists_SetBit(void* Obj)
	{
		((UDLSSSettings*)Obj)->bCustomDLSSBinaryExists = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bCustomDLSSBinaryExists = { "bCustomDLSSBinaryExists", nullptr, (EPropertyFlags)0x0010040000024001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UDLSSSettings), &Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bCustomDLSSBinaryExists_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bCustomDLSSBinaryExists_MetaData), Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bCustomDLSSBinaryExists_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bAllowOTAUpdate_MetaData[] = {
		{ "Category", "General Settings" },
		{ "Comment", "/** Allow OTA updates of DLSS models */" },
		{ "DisplayName", "Allow OTA update" },
		{ "ModuleRelativePath", "Public/DLSSSettings.h" },
		{ "ToolTip", "Allow OTA updates of DLSS models" },
	};
#endif
	void Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bAllowOTAUpdate_SetBit(void* Obj)
	{
		((UDLSSSettings*)Obj)->bAllowOTAUpdate = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bAllowOTAUpdate = { "bAllowOTAUpdate", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UDLSSSettings), &Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bAllowOTAUpdate_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bAllowOTAUpdate_MetaData), Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bAllowOTAUpdate_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bShowDLSSIncompatiblePluginsToolsWarnings_MetaData[] = {
		{ "Category", "Editor" },
		{ "Comment", "/** This enables warnings about plugins & tools that are incompatible with DLSS/DLAA in the editor. This setting and the local setting both must be set to get warnings */" },
		{ "DisplayName", "Warn about incompatible plugins and tools" },
		{ "ModuleRelativePath", "Public/DLSSSettings.h" },
		{ "ToolTip", "This enables warnings about plugins & tools that are incompatible with DLSS/DLAA in the editor. This setting and the local setting both must be set to get warnings" },
	};
#endif
	void Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bShowDLSSIncompatiblePluginsToolsWarnings_SetBit(void* Obj)
	{
		((UDLSSSettings*)Obj)->bShowDLSSIncompatiblePluginsToolsWarnings = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bShowDLSSIncompatiblePluginsToolsWarnings = { "bShowDLSSIncompatiblePluginsToolsWarnings", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UDLSSSettings), &Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bShowDLSSIncompatiblePluginsToolsWarnings_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bShowDLSSIncompatiblePluginsToolsWarnings_MetaData), Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bShowDLSSIncompatiblePluginsToolsWarnings_MetaData) };
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLAAPreset_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLAAPreset_MetaData[] = {
		{ "Category", "General Settings" },
		{ "Comment", "/** DLAA preset setting. Allows selecting a different DL model than the default */" },
		{ "DisplayName", "DLAA Preset" },
		{ "ModuleRelativePath", "Public/DLSSSettings.h" },
		{ "ToolTip", "DLAA preset setting. Allows selecting a different DL model than the default" },
	};
#endif
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLAAPreset = { "DLAAPreset", nullptr, (EPropertyFlags)0x0010040000004001, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDLSSSettings, DLAAPreset), Z_Construct_UEnum_DLSS_EDLSSPreset, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLAAPreset_MetaData), Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLAAPreset_MetaData) }; // 2187881533
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSQualityPreset_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSQualityPreset_MetaData[] = {
		{ "Category", "General Settings" },
		{ "Comment", "/** DLSS quality mode preset setting. Allows selecting a different DL model than the default */" },
		{ "DisplayName", "DLSS Quality Preset" },
		{ "ModuleRelativePath", "Public/DLSSSettings.h" },
		{ "ToolTip", "DLSS quality mode preset setting. Allows selecting a different DL model than the default" },
	};
#endif
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSQualityPreset = { "DLSSQualityPreset", nullptr, (EPropertyFlags)0x0010040000004001, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDLSSSettings, DLSSQualityPreset), Z_Construct_UEnum_DLSS_EDLSSPreset, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSQualityPreset_MetaData), Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSQualityPreset_MetaData) }; // 2187881533
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSBalancedPreset_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSBalancedPreset_MetaData[] = {
		{ "Category", "General Settings" },
		{ "Comment", "/** DLSS balanced mode preset setting. Allows selecting a different DL model than the default */" },
		{ "DisplayName", "DLSS Balanced Preset" },
		{ "ModuleRelativePath", "Public/DLSSSettings.h" },
		{ "ToolTip", "DLSS balanced mode preset setting. Allows selecting a different DL model than the default" },
	};
#endif
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSBalancedPreset = { "DLSSBalancedPreset", nullptr, (EPropertyFlags)0x0010040000004001, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDLSSSettings, DLSSBalancedPreset), Z_Construct_UEnum_DLSS_EDLSSPreset, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSBalancedPreset_MetaData), Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSBalancedPreset_MetaData) }; // 2187881533
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSPerformancePreset_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSPerformancePreset_MetaData[] = {
		{ "Category", "General Settings" },
		{ "Comment", "/** DLSS performance mode preset setting. Allows selecting a different DL model than the default */" },
		{ "DisplayName", "DLSS Performance Preset" },
		{ "ModuleRelativePath", "Public/DLSSSettings.h" },
		{ "ToolTip", "DLSS performance mode preset setting. Allows selecting a different DL model than the default" },
	};
#endif
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSPerformancePreset = { "DLSSPerformancePreset", nullptr, (EPropertyFlags)0x0010040000004001, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDLSSSettings, DLSSPerformancePreset), Z_Construct_UEnum_DLSS_EDLSSPreset, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSPerformancePreset_MetaData), Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSPerformancePreset_MetaData) }; // 2187881533
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSUltraPerformancePreset_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSUltraPerformancePreset_MetaData[] = {
		{ "Category", "General Settings" },
		{ "Comment", "/** DLSS ultra performance mode preset setting. Allows selecting a different DL model than the default */" },
		{ "DisplayName", "DLSS Ultra Performance Preset" },
		{ "ModuleRelativePath", "Public/DLSSSettings.h" },
		{ "ToolTip", "DLSS ultra performance mode preset setting. Allows selecting a different DL model than the default" },
	};
#endif
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSUltraPerformancePreset = { "DLSSUltraPerformancePreset", nullptr, (EPropertyFlags)0x0010040000004001, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UDLSSSettings, DLSSUltraPerformancePreset), Z_Construct_UEnum_DLSS_EDLSSPreset, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSUltraPerformancePreset_MetaData), Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSUltraPerformancePreset_MetaData) }; // 2187881533
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UDLSSSettings_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSD3D12,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSD3D11,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSVulkan,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSInEditorViewports,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bEnableDLSSInPlayInEditorViewports,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bShowDLSSSDebugOnScreenMessages,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSSettings_Statics::NewProp_GenericDLSSBinaryPath,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bGenericDLSSBinaryExists,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSSettings_Statics::NewProp_NVIDIANGXApplicationId,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSSettings_Statics::NewProp_CustomDLSSBinaryPath,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bCustomDLSSBinaryExists,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bAllowOTAUpdate,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSSettings_Statics::NewProp_bShowDLSSIncompatiblePluginsToolsWarnings,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLAAPreset_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLAAPreset,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSQualityPreset_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSQualityPreset,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSBalancedPreset_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSBalancedPreset,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSPerformancePreset_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSPerformancePreset,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSUltraPerformancePreset_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UDLSSSettings_Statics::NewProp_DLSSUltraPerformancePreset,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UDLSSSettings_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UDLSSSettings>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UDLSSSettings_Statics::ClassParams = {
		&UDLSSSettings::StaticClass,
		"Engine",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UDLSSSettings_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSSettings_Statics::PropPointers),
		0,
		0x001000A6u,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSSettings_Statics::Class_MetaDataParams), Z_Construct_UClass_UDLSSSettings_Statics::Class_MetaDataParams)
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UDLSSSettings_Statics::PropPointers) < 2048);
	UClass* Z_Construct_UClass_UDLSSSettings()
	{
		if (!Z_Registration_Info_UClass_UDLSSSettings.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UDLSSSettings.OuterSingleton, Z_Construct_UClass_UDLSSSettings_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UDLSSSettings.OuterSingleton;
	}
	template<> DLSS_API UClass* StaticClass<UDLSSSettings>()
	{
		return UDLSSSettings::StaticClass();
	}
	UDLSSSettings::UDLSSSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UDLSSSettings);
	UDLSSSettings::~UDLSSSettings() {}
	struct Z_CompiledInDeferFile_FID__stage_plugin_Plugins_DLSS_HostProject_Plugins_DLSS_Source_DLSS_Public_DLSSSettings_h_Statics
	{
		static const FEnumRegisterCompiledInInfo EnumInfo[];
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FEnumRegisterCompiledInInfo Z_CompiledInDeferFile_FID__stage_plugin_Plugins_DLSS_HostProject_Plugins_DLSS_Source_DLSS_Public_DLSSSettings_h_Statics::EnumInfo[] = {
		{ EDLSSSettingOverride_StaticEnum, TEXT("EDLSSSettingOverride"), &Z_Registration_Info_UEnum_EDLSSSettingOverride, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 3368560862U) },
		{ EDLSSPreset_StaticEnum, TEXT("EDLSSPreset"), &Z_Registration_Info_UEnum_EDLSSPreset, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 2187881533U) },
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID__stage_plugin_Plugins_DLSS_HostProject_Plugins_DLSS_Source_DLSS_Public_DLSSSettings_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UDLSSOverrideSettings, UDLSSOverrideSettings::StaticClass, TEXT("UDLSSOverrideSettings"), &Z_Registration_Info_UClass_UDLSSOverrideSettings, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UDLSSOverrideSettings), 3463983580U) },
		{ Z_Construct_UClass_UDLSSSettings, UDLSSSettings::StaticClass, TEXT("UDLSSSettings"), &Z_Registration_Info_UClass_UDLSSSettings, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UDLSSSettings), 994102952U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID__stage_plugin_Plugins_DLSS_HostProject_Plugins_DLSS_Source_DLSS_Public_DLSSSettings_h_996587973(TEXT("/Script/DLSS"),
		Z_CompiledInDeferFile_FID__stage_plugin_Plugins_DLSS_HostProject_Plugins_DLSS_Source_DLSS_Public_DLSSSettings_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID__stage_plugin_Plugins_DLSS_HostProject_Plugins_DLSS_Source_DLSS_Public_DLSSSettings_h_Statics::ClassInfo),
		nullptr, 0,
		Z_CompiledInDeferFile_FID__stage_plugin_Plugins_DLSS_HostProject_Plugins_DLSS_Source_DLSS_Public_DLSSSettings_h_Statics::EnumInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID__stage_plugin_Plugins_DLSS_HostProject_Plugins_DLSS_Source_DLSS_Public_DLSSSettings_h_Statics::EnumInfo));
PRAGMA_ENABLE_DEPRECATION_WARNINGS
