// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "StreamlineRHI/Public/StreamlineSettings.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeStreamlineSettings() {}
// Cross Module References
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
	STREAMLINERHI_API UClass* Z_Construct_UClass_UStreamlineOverrideSettings();
	STREAMLINERHI_API UClass* Z_Construct_UClass_UStreamlineOverrideSettings_NoRegister();
	STREAMLINERHI_API UClass* Z_Construct_UClass_UStreamlineSettings();
	STREAMLINERHI_API UClass* Z_Construct_UClass_UStreamlineSettings_NoRegister();
	STREAMLINERHI_API UEnum* Z_Construct_UEnum_StreamlineRHI_EStreamlineSettingOverride();
	UPackage* Z_Construct_UPackage__Script_StreamlineRHI();
// End Cross Module References
	static FEnumRegistrationInfo Z_Registration_Info_UEnum_EStreamlineSettingOverride;
	static UEnum* EStreamlineSettingOverride_StaticEnum()
	{
		if (!Z_Registration_Info_UEnum_EStreamlineSettingOverride.OuterSingleton)
		{
			Z_Registration_Info_UEnum_EStreamlineSettingOverride.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_StreamlineRHI_EStreamlineSettingOverride, (UObject*)Z_Construct_UPackage__Script_StreamlineRHI(), TEXT("EStreamlineSettingOverride"));
		}
		return Z_Registration_Info_UEnum_EStreamlineSettingOverride.OuterSingleton;
	}
	template<> STREAMLINERHI_API UEnum* StaticEnum<EStreamlineSettingOverride>()
	{
		return EStreamlineSettingOverride_StaticEnum();
	}
	struct Z_Construct_UEnum_StreamlineRHI_EStreamlineSettingOverride_Statics
	{
		static const UECodeGen_Private::FEnumeratorParam Enumerators[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[];
#endif
		static const UECodeGen_Private::FEnumParams EnumParams;
	};
	const UECodeGen_Private::FEnumeratorParam Z_Construct_UEnum_StreamlineRHI_EStreamlineSettingOverride_Statics::Enumerators[] = {
		{ "EStreamlineSettingOverride::Enabled", (int64)EStreamlineSettingOverride::Enabled },
		{ "EStreamlineSettingOverride::Disabled", (int64)EStreamlineSettingOverride::Disabled },
		{ "EStreamlineSettingOverride::UseProjectSettings", (int64)EStreamlineSettingOverride::UseProjectSettings },
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UEnum_StreamlineRHI_EStreamlineSettingOverride_Statics::Enum_MetaDataParams[] = {
		{ "Disabled.DisplayName", "False" },
		{ "Disabled.Name", "EStreamlineSettingOverride::Disabled" },
		{ "Enabled.DisplayName", "True" },
		{ "Enabled.Name", "EStreamlineSettingOverride::Enabled" },
		{ "ModuleRelativePath", "Public/StreamlineSettings.h" },
		{ "UseProjectSettings.DisplayName", "Use project settings" },
		{ "UseProjectSettings.Name", "EStreamlineSettingOverride::UseProjectSettings" },
	};
#endif
	const UECodeGen_Private::FEnumParams Z_Construct_UEnum_StreamlineRHI_EStreamlineSettingOverride_Statics::EnumParams = {
		(UObject*(*)())Z_Construct_UPackage__Script_StreamlineRHI,
		nullptr,
		"EStreamlineSettingOverride",
		"EStreamlineSettingOverride",
		Z_Construct_UEnum_StreamlineRHI_EStreamlineSettingOverride_Statics::Enumerators,
		RF_Public|RF_Transient|RF_MarkAsNative,
		UE_ARRAY_COUNT(Z_Construct_UEnum_StreamlineRHI_EStreamlineSettingOverride_Statics::Enumerators),
		EEnumFlags::None,
		(uint8)UEnum::ECppForm::EnumClass,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UEnum_StreamlineRHI_EStreamlineSettingOverride_Statics::Enum_MetaDataParams), Z_Construct_UEnum_StreamlineRHI_EStreamlineSettingOverride_Statics::Enum_MetaDataParams)
	};
	UEnum* Z_Construct_UEnum_StreamlineRHI_EStreamlineSettingOverride()
	{
		if (!Z_Registration_Info_UEnum_EStreamlineSettingOverride.InnerSingleton)
		{
			UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_EStreamlineSettingOverride.InnerSingleton, Z_Construct_UEnum_StreamlineRHI_EStreamlineSettingOverride_Statics::EnumParams);
		}
		return Z_Registration_Info_UEnum_EStreamlineSettingOverride.InnerSingleton;
	}
	void UStreamlineOverrideSettings::StaticRegisterNativesUStreamlineOverrideSettings()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UStreamlineOverrideSettings);
	UClass* Z_Construct_UClass_UStreamlineOverrideSettings_NoRegister()
	{
		return UStreamlineOverrideSettings::StaticClass();
	}
	struct Z_Construct_UClass_UStreamlineOverrideSettings_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const UECodeGen_Private::FBytePropertyParams NewProp_EnableDLSSFGInPlayInEditorViewportsOverride_Underlying;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_EnableDLSSFGInPlayInEditorViewportsOverride_MetaData[];
#endif
		static const UECodeGen_Private::FEnumPropertyParams NewProp_EnableDLSSFGInPlayInEditorViewportsOverride;
		static const UECodeGen_Private::FBytePropertyParams NewProp_LoadDebugOverlayOverride_Underlying;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_LoadDebugOverlayOverride_MetaData[];
#endif
		static const UECodeGen_Private::FEnumPropertyParams NewProp_LoadDebugOverlayOverride;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UStreamlineOverrideSettings_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UObject,
		(UObject* (*)())Z_Construct_UPackage__Script_StreamlineRHI,
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UStreamlineOverrideSettings_Statics::DependentSingletons) < 16);
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UStreamlineOverrideSettings_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "StreamlineSettings.h" },
		{ "ModuleRelativePath", "Public/StreamlineSettings.h" },
	};
#endif
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_UStreamlineOverrideSettings_Statics::NewProp_EnableDLSSFGInPlayInEditorViewportsOverride_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UStreamlineOverrideSettings_Statics::NewProp_EnableDLSSFGInPlayInEditorViewportsOverride_MetaData[] = {
		{ "Category", "Level Editor - Viewport (Local)" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Enable DLSS Frame Generation in play in editor viewports. Saved to local user config only. */" },
#endif
		{ "DisplayName", "Enable DLSS-FG in Play In Editor viewports" },
		{ "ModuleRelativePath", "Public/StreamlineSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Enable DLSS Frame Generation in play in editor viewports. Saved to local user config only." },
#endif
	};
#endif
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UClass_UStreamlineOverrideSettings_Statics::NewProp_EnableDLSSFGInPlayInEditorViewportsOverride = { "EnableDLSSFGInPlayInEditorViewportsOverride", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UStreamlineOverrideSettings, EnableDLSSFGInPlayInEditorViewportsOverride), Z_Construct_UEnum_StreamlineRHI_EStreamlineSettingOverride, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UStreamlineOverrideSettings_Statics::NewProp_EnableDLSSFGInPlayInEditorViewportsOverride_MetaData), Z_Construct_UClass_UStreamlineOverrideSettings_Statics::NewProp_EnableDLSSFGInPlayInEditorViewportsOverride_MetaData) }; // 421387610
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_UStreamlineOverrideSettings_Statics::NewProp_LoadDebugOverlayOverride_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UStreamlineOverrideSettings_Statics::NewProp_LoadDebugOverlayOverride_MetaData[] = {
		{ "Category", "General Settings" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n\x09 * Load the Streamline debug overlay in non-Shipping configurations. Note that the overlay requires DLSS Frame Generation to be available.\n\x09 * Modifying this setting requires an editor restart to take effect. Saved to local user config only\n\x09 */" },
#endif
		{ "DisplayName", "Load Debug Overlay" },
		{ "ModuleRelativePath", "Public/StreamlineSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Load the Streamline debug overlay in non-Shipping configurations. Note that the overlay requires DLSS Frame Generation to be available.\nModifying this setting requires an editor restart to take effect. Saved to local user config only" },
#endif
	};
#endif
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UClass_UStreamlineOverrideSettings_Statics::NewProp_LoadDebugOverlayOverride = { "LoadDebugOverlayOverride", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UStreamlineOverrideSettings, LoadDebugOverlayOverride), Z_Construct_UEnum_StreamlineRHI_EStreamlineSettingOverride, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UStreamlineOverrideSettings_Statics::NewProp_LoadDebugOverlayOverride_MetaData), Z_Construct_UClass_UStreamlineOverrideSettings_Statics::NewProp_LoadDebugOverlayOverride_MetaData) }; // 421387610
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UStreamlineOverrideSettings_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UStreamlineOverrideSettings_Statics::NewProp_EnableDLSSFGInPlayInEditorViewportsOverride_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UStreamlineOverrideSettings_Statics::NewProp_EnableDLSSFGInPlayInEditorViewportsOverride,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UStreamlineOverrideSettings_Statics::NewProp_LoadDebugOverlayOverride_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UStreamlineOverrideSettings_Statics::NewProp_LoadDebugOverlayOverride,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UStreamlineOverrideSettings_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UStreamlineOverrideSettings>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UStreamlineOverrideSettings_Statics::ClassParams = {
		&UStreamlineOverrideSettings::StaticClass,
		"Engine",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UStreamlineOverrideSettings_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		UE_ARRAY_COUNT(Z_Construct_UClass_UStreamlineOverrideSettings_Statics::PropPointers),
		0,
		0x001000E4u,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UStreamlineOverrideSettings_Statics::Class_MetaDataParams), Z_Construct_UClass_UStreamlineOverrideSettings_Statics::Class_MetaDataParams)
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UStreamlineOverrideSettings_Statics::PropPointers) < 2048);
	UClass* Z_Construct_UClass_UStreamlineOverrideSettings()
	{
		if (!Z_Registration_Info_UClass_UStreamlineOverrideSettings.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UStreamlineOverrideSettings.OuterSingleton, Z_Construct_UClass_UStreamlineOverrideSettings_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UStreamlineOverrideSettings.OuterSingleton;
	}
	template<> STREAMLINERHI_API UClass* StaticClass<UStreamlineOverrideSettings>()
	{
		return UStreamlineOverrideSettings::StaticClass();
	}
	UStreamlineOverrideSettings::UStreamlineOverrideSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UStreamlineOverrideSettings);
	UStreamlineOverrideSettings::~UStreamlineOverrideSettings() {}
	void UStreamlineSettings::StaticRegisterNativesUStreamlineSettings()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UStreamlineSettings);
	UClass* Z_Construct_UClass_UStreamlineSettings_NoRegister()
	{
		return UStreamlineSettings::StaticClass();
	}
	struct Z_Construct_UClass_UStreamlineSettings_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bEnableStreamlineD3D12_MetaData[];
#endif
		static void NewProp_bEnableStreamlineD3D12_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bEnableStreamlineD3D12;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bEnableStreamlineD3D11_MetaData[];
#endif
		static void NewProp_bEnableStreamlineD3D11_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bEnableStreamlineD3D11;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bEnableDLSSFGInPlayInEditorViewports_MetaData[];
#endif
		static void NewProp_bEnableDLSSFGInPlayInEditorViewports_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bEnableDLSSFGInPlayInEditorViewports;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bLoadDebugOverlay_MetaData[];
#endif
		static void NewProp_bLoadDebugOverlay_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bLoadDebugOverlay;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bAllowOTAUpdate_MetaData[];
#endif
		static void NewProp_bAllowOTAUpdate_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bAllowOTAUpdate;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_NVIDIANGXApplicationId_MetaData[];
#endif
		static const UECodeGen_Private::FIntPropertyParams NewProp_NVIDIANGXApplicationId;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UStreamlineSettings_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UObject,
		(UObject* (*)())Z_Construct_UPackage__Script_StreamlineRHI,
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UStreamlineSettings_Statics::DependentSingletons) < 16);
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UStreamlineSettings_Statics::Class_MetaDataParams[] = {
		{ "DisplayName", "NVIDIA DLSS Frame Generation" },
		{ "IncludePath", "StreamlineSettings.h" },
		{ "ModuleRelativePath", "Public/StreamlineSettings.h" },
	};
#endif
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bEnableStreamlineD3D12_MetaData[] = {
		{ "Category", "Platforms" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Enable plugin features for D3D12, if the driver supports it at runtime */" },
#endif
		{ "DisplayName", "Enable plugin features for the D3D12RHI" },
		{ "ModuleRelativePath", "Public/StreamlineSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Enable plugin features for D3D12, if the driver supports it at runtime" },
#endif
	};
#endif
	void Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bEnableStreamlineD3D12_SetBit(void* Obj)
	{
		((UStreamlineSettings*)Obj)->bEnableStreamlineD3D12 = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bEnableStreamlineD3D12 = { "bEnableStreamlineD3D12", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UStreamlineSettings), &Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bEnableStreamlineD3D12_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bEnableStreamlineD3D12_MetaData), Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bEnableStreamlineD3D12_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bEnableStreamlineD3D11_MetaData[] = {
		{ "Category", "Platforms" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Enable plugin features for D3D11, if the driver supports it at runtime */" },
#endif
		{ "DisplayName", "Enable plugin features for the D3D11RHI (Reflex only)" },
		{ "ModuleRelativePath", "Public/StreamlineSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Enable plugin features for D3D11, if the driver supports it at runtime" },
#endif
	};
#endif
	void Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bEnableStreamlineD3D11_SetBit(void* Obj)
	{
		((UStreamlineSettings*)Obj)->bEnableStreamlineD3D11 = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bEnableStreamlineD3D11 = { "bEnableStreamlineD3D11", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UStreamlineSettings), &Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bEnableStreamlineD3D11_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bEnableStreamlineD3D11_MetaData), Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bEnableStreamlineD3D11_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bEnableDLSSFGInPlayInEditorViewports_MetaData[] = {
		{ "Category", "Level Editor - Viewport" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Enable DLSS Frame Generation in play in editor viewports. This project wide setting can be locally overridden in the NVIDIA DLSS Frame Generation (Local) settings.*/" },
#endif
		{ "DisplayName", "Enable DLSS-FG in Play In Editor viewports" },
		{ "ModuleRelativePath", "Public/StreamlineSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Enable DLSS Frame Generation in play in editor viewports. This project wide setting can be locally overridden in the NVIDIA DLSS Frame Generation (Local) settings." },
#endif
	};
#endif
	void Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bEnableDLSSFGInPlayInEditorViewports_SetBit(void* Obj)
	{
		((UStreamlineSettings*)Obj)->bEnableDLSSFGInPlayInEditorViewports = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bEnableDLSSFGInPlayInEditorViewports = { "bEnableDLSSFGInPlayInEditorViewports", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UStreamlineSettings), &Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bEnableDLSSFGInPlayInEditorViewports_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bEnableDLSSFGInPlayInEditorViewports_MetaData), Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bEnableDLSSFGInPlayInEditorViewports_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bLoadDebugOverlay_MetaData[] = {
		{ "Category", "General Settings" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n\x09 * Load the Streamline debug overlay in non-Shipping configurations. Note that the overlay requires DLSS Frame Generation to be available.\n\x09 * This project wide setting can be locally overridden in the NVIDIA DLSS Frame Generation (Local) settings.\n\x09 * Modifying this setting requires an editor restart to take effect\n\x09 */" },
#endif
		{ "DisplayName", "Load Debug Overlay" },
		{ "ModuleRelativePath", "Public/StreamlineSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Load the Streamline debug overlay in non-Shipping configurations. Note that the overlay requires DLSS Frame Generation to be available.\nThis project wide setting can be locally overridden in the NVIDIA DLSS Frame Generation (Local) settings.\nModifying this setting requires an editor restart to take effect" },
#endif
	};
#endif
	void Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bLoadDebugOverlay_SetBit(void* Obj)
	{
		((UStreamlineSettings*)Obj)->bLoadDebugOverlay = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bLoadDebugOverlay = { "bLoadDebugOverlay", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UStreamlineSettings), &Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bLoadDebugOverlay_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bLoadDebugOverlay_MetaData), Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bLoadDebugOverlay_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bAllowOTAUpdate_MetaData[] = {
		{ "Category", "General Settings" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Allow OTA updates of Streamline features */" },
#endif
		{ "DisplayName", "Allow OTA update" },
		{ "ModuleRelativePath", "Public/StreamlineSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Allow OTA updates of Streamline features" },
#endif
	};
#endif
	void Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bAllowOTAUpdate_SetBit(void* Obj)
	{
		((UStreamlineSettings*)Obj)->bAllowOTAUpdate = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bAllowOTAUpdate = { "bAllowOTAUpdate", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UStreamlineSettings), &Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bAllowOTAUpdate_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bAllowOTAUpdate_MetaData), Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bAllowOTAUpdate_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_NVIDIANGXApplicationId_MetaData[] = {
		{ "Category", "General Settings" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** By default the DLSS Frame Generation plugin uses the UE Project ID to initialize Streamline. In some cases NVIDIA might provide a separate NVIDIA Application ID, which should be put here. */" },
#endif
		{ "DisplayName", "NVIDIA NGX Application ID" },
		{ "ModuleRelativePath", "Public/StreamlineSettings.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "By default the DLSS Frame Generation plugin uses the UE Project ID to initialize Streamline. In some cases NVIDIA might provide a separate NVIDIA Application ID, which should be put here." },
#endif
	};
#endif
	const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_NVIDIANGXApplicationId = { "NVIDIANGXApplicationId", nullptr, (EPropertyFlags)0x0010040000004001, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UStreamlineSettings, NVIDIANGXApplicationId), METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_NVIDIANGXApplicationId_MetaData), Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_NVIDIANGXApplicationId_MetaData) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UStreamlineSettings_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bEnableStreamlineD3D12,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bEnableStreamlineD3D11,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bEnableDLSSFGInPlayInEditorViewports,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bLoadDebugOverlay,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_bAllowOTAUpdate,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UStreamlineSettings_Statics::NewProp_NVIDIANGXApplicationId,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UStreamlineSettings_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UStreamlineSettings>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UStreamlineSettings_Statics::ClassParams = {
		&UStreamlineSettings::StaticClass,
		"Engine",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UStreamlineSettings_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		UE_ARRAY_COUNT(Z_Construct_UClass_UStreamlineSettings_Statics::PropPointers),
		0,
		0x001000A6u,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UStreamlineSettings_Statics::Class_MetaDataParams), Z_Construct_UClass_UStreamlineSettings_Statics::Class_MetaDataParams)
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UStreamlineSettings_Statics::PropPointers) < 2048);
	UClass* Z_Construct_UClass_UStreamlineSettings()
	{
		if (!Z_Registration_Info_UClass_UStreamlineSettings.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UStreamlineSettings.OuterSingleton, Z_Construct_UClass_UStreamlineSettings_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UStreamlineSettings.OuterSingleton;
	}
	template<> STREAMLINERHI_API UClass* StaticClass<UStreamlineSettings>()
	{
		return UStreamlineSettings::StaticClass();
	}
	UStreamlineSettings::UStreamlineSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UStreamlineSettings);
	UStreamlineSettings::~UStreamlineSettings() {}
	struct Z_CompiledInDeferFile_FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineRHI_Public_StreamlineSettings_h_Statics
	{
		static const FEnumRegisterCompiledInInfo EnumInfo[];
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FEnumRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineRHI_Public_StreamlineSettings_h_Statics::EnumInfo[] = {
		{ EStreamlineSettingOverride_StaticEnum, TEXT("EStreamlineSettingOverride"), &Z_Registration_Info_UEnum_EStreamlineSettingOverride, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 421387610U) },
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineRHI_Public_StreamlineSettings_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UStreamlineOverrideSettings, UStreamlineOverrideSettings::StaticClass, TEXT("UStreamlineOverrideSettings"), &Z_Registration_Info_UClass_UStreamlineOverrideSettings, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UStreamlineOverrideSettings), 2639591662U) },
		{ Z_Construct_UClass_UStreamlineSettings, UStreamlineSettings::StaticClass, TEXT("UStreamlineSettings"), &Z_Registration_Info_UClass_UStreamlineSettings, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UStreamlineSettings), 3107445395U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineRHI_Public_StreamlineSettings_h_1483386193(TEXT("/Script/StreamlineRHI"),
		Z_CompiledInDeferFile_FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineRHI_Public_StreamlineSettings_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineRHI_Public_StreamlineSettings_h_Statics::ClassInfo),
		nullptr, 0,
		Z_CompiledInDeferFile_FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineRHI_Public_StreamlineSettings_h_Statics::EnumInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_iFraude_Documents_GitHub_City75_Plugins_nvidia_Streamline_Source_StreamlineRHI_Public_StreamlineSettings_h_Statics::EnumInfo));
PRAGMA_ENABLE_DEPRECATION_WARNINGS
