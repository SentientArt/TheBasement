/*
* Copyright (c) 2022 - 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
*
* NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
* property and proprietary rights in and to this material, related
* documentation and any modifications thereto. Any use, reproduction,
* disclosure or distribution of this material and related documentation
* without an express license agreement from NVIDIA CORPORATION or
* its affiliates is strictly prohibited.
*/

#include "StreamlineD3D12RHI.h"

#include "Features/IModularFeatures.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "Runtime/Launch/Resources/Version.h"
#if ENGINE_PROVIDES_ID3D12DYNAMICRHI
#include "ID3D12DynamicRHI.h"
#if (ENGINE_MAJOR_VERSION == 5) && (ENGINE_MINOR_VERSION >= 3)
#include "Windows/WindowsD3D12ThirdParty.h" // for dxgi1_6.h
#else
#include "Windows/D3D12ThirdParty.h" // for dxgi1_6.h
#endif
#else
#include "D3D12RHIPrivate.h"
THIRD_PARTY_INCLUDES_START
#include "dxgi1_6.h"
THIRD_PARTY_INCLUDES_END
#endif
#include "HAL/IConsoleManager.h"


class FD3D12Device;
#ifndef DX_MAX_MSAA_COUNT
#define DX_MAX_MSAA_COUNT	8
#endif
#if !defined(D3D12_RHI_RAYTRACING)
#define D3D12_RHI_RAYTRACING (RHI_RAYTRACING)
#endif

struct FShaderCodePackedResourceCounts;

#if ENGINE_MAJOR_VERSION < 5 || ENGINE_MINOR_VERSION < 3
#include "D3D12Util.h"
#endif

#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "Windows/IDXGISwapchainProvider.h"

#include "StreamlineAPI.h"
#include "StreamlineConversions.h"
#include "StreamlineRHI.h"
#include "sl.h"
#include "sl_dlss_g.h"



// The UE module
DEFINE_LOG_CATEGORY_STATIC(LogStreamlineD3D12RHI, Log, All);


#define LOCTEXT_NAMESPACE "StreamlineD3D12RHI"


class FStreamlineD3D12DXGISwapchainProvider : public IDXGISwapchainProvider
{
public:
	FStreamlineD3D12DXGISwapchainProvider(const FStreamlineRHI* InRHI) : StreamlineRHI(InRHI) {}

	virtual ~FStreamlineD3D12DXGISwapchainProvider() = default;

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1
	bool SupportsRHI(ERHIInterfaceType RHIType) const override final { return RHIType == ERHIInterfaceType::D3D12; }
#else
	bool SupportsRHI(const TCHAR* RHIName) const override final { return FString(RHIName) == FString("D3D12"); }
#endif

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
	const TCHAR* GetProviderName() const override final { return TEXT("FStreamlineD3D12DXGISwapchainProvider"); }
#else
	TCHAR* GetName() const override final
	{
		static TCHAR Name[] = TEXT("FStreamlineD3D12DXGISwapchainProvider");
		return Name;
	}
#endif

	HRESULT CreateSwapChainForHwnd(IDXGIFactory2* pFactory, IUnknown* pDevice, HWND hWnd, const DXGI_SWAP_CHAIN_DESC1* pDesc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullScreenDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain) override final
	{
		if (!StreamlineRHI->IsSwapchainHookingAllowed())
		{
			return pFactory->CreateSwapChainForHwnd(pDevice, hWnd, pDesc, pFullScreenDesc, pRestrictToOutput, ppSwapChain);
		}
		// TODO: what happens if a second swapchain is created while PIE is active?
		IDXGIFactory2* SLFactory = pFactory;
		sl::Result Result = SLUpgradeInterface(reinterpret_cast<void**>(&SLFactory));
		checkf(Result == sl::Result::eOk, TEXT("%s: error upgrading IDXGIFactory (%s)"), ANSI_TO_TCHAR(__FUNCTION__), ANSI_TO_TCHAR(sl::getResultAsStr(Result)));

		return SLFactory->CreateSwapChainForHwnd(pDevice, hWnd, pDesc, pFullScreenDesc, pRestrictToOutput, ppSwapChain);
	}

	HRESULT CreateSwapChain(IDXGIFactory* pFactory, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain) override final
	{
		if (!StreamlineRHI->IsSwapchainHookingAllowed())
		{
			return pFactory->CreateSwapChain(pDevice, pDesc, ppSwapChain);
		}
		// TODO: what happens if a second swapchain is created while PIE is active?
		IDXGIFactory* SLFactory = pFactory;
		sl::Result Result = SLUpgradeInterface(reinterpret_cast<void**>(&SLFactory));
		checkf(Result == sl::Result::eOk, TEXT("%s: error upgrading IDXGIFactory (%s)"), ANSI_TO_TCHAR(__FUNCTION__), ANSI_TO_TCHAR(sl::getResultAsStr(Result)));

		return SLFactory->CreateSwapChain(pDevice, pDesc, ppSwapChain);
	}
private:
	const FStreamlineRHI* StreamlineRHI;
};


class STREAMLINED3D12RHI_API FStreamlineD3D12RHI : public FStreamlineRHI
{
public:

	FStreamlineD3D12RHI(const FStreamlineRHICreateArguments& Arguments)
	:	FStreamlineRHI(Arguments)
#if ENGINE_PROVIDES_ID3D12DYNAMICRHI
		, D3D12RHI(CastDynamicRHI<ID3D12DynamicRHI>(Arguments.DynamicRHI))
#else
		, D3D12RHI(static_cast<FD3D12DynamicRHI*>(Arguments.DynamicRHI))
#endif
	{
		UE_LOG(LogStreamlineD3D12RHI, Log, TEXT("%s Enter"), ANSI_TO_TCHAR(__FUNCTION__));

		check(D3D12RHI != nullptr);
#if ENGINE_PROVIDES_ID3D12DYNAMICRHI
		TArray<FD3D12MinimalAdapterDesc> AdapterDescs = D3D12RHI->RHIGetAdapterDescs();
		check(AdapterDescs.Num() > 0);
		if (AdapterDescs.Num() > 1)
		{
			UE_LOG(LogStreamlineD3D12RHI, Warning, TEXT("%s: found %d adapters, using first one found to query feature availability"), ANSI_TO_TCHAR(__FUNCTION__), AdapterDescs.Num());
		}
		const DXGI_ADAPTER_DESC& DXGIAdapterDesc = AdapterDescs[0].Desc;
#else
		const DXGI_ADAPTER_DESC& DXGIAdapterDesc = D3D12RHI->GetAdapter().GetD3DAdapterDesc();
#endif
		AdapterLuid = DXGIAdapterDesc.AdapterLuid;
		SLAdapterInfo.deviceLUID = reinterpret_cast<uint8_t*>(&AdapterLuid);
		SLAdapterInfo.deviceLUIDSizeInBytes = sizeof(AdapterLuid);
		SLAdapterInfo.vkPhysicalDevice = nullptr;

		if (IsStreamlineSupported())
		{
			sl::Result Result = SLisFeatureSupported(sl::kFeatureDLSS_G, SLAdapterInfo);
			UE_LOG(LogStreamlineD3D12RHI, Log, TEXT("SLisFeatureSupported(sl::kFeatureDLSS_G) -> (%d, %s)"), Result, ANSI_TO_TCHAR(sl::getResultAsStr(Result)));
			if (Result == sl::Result::eOk)
			{
				UE_LOG(LogStreamlineD3D12RHI, Log, TEXT("Registering FStreamlineD3D12DXGISwapchainProvider as IDXGISwapchainProvider"));
				CustomSwapchainProvider = MakeUnique<FStreamlineD3D12DXGISwapchainProvider>(this);
				IModularFeatures::Get().RegisterModularFeature(IDXGISwapchainProvider::GetModularFeatureName(), CustomSwapchainProvider.Get());
			}
			else
			{
				UE_LOG(LogStreamlineD3D12RHI, Log, TEXT("Skip registering IDXGISwapchainProvider, DLSS-FG unavailable (%s)"), ANSI_TO_TCHAR(sl::getResultAsStr(Result)));
			}
		}

		UE_LOG(LogStreamlineD3D12RHI, Log, TEXT("%s Leave"), ANSI_TO_TCHAR(__FUNCTION__));
	}

	virtual ~FStreamlineD3D12RHI()
	{
		UE_LOG(LogStreamlineD3D12RHI, Log, TEXT("%s Enter"), ANSI_TO_TCHAR(__FUNCTION__));
		if (CustomSwapchainProvider.IsValid())
		{
			UE_LOG(LogStreamlineD3D12RHI, Log, TEXT("Unregistering FStreamlineD3D12DXGISwapchainProvider as IDXGISwapchainProvider"));
			IModularFeatures::Get().UnregisterModularFeature(IDXGISwapchainProvider::GetModularFeatureName(), CustomSwapchainProvider.Get());
			CustomSwapchainProvider.Reset();
		}
		UE_LOG(LogStreamlineD3D12RHI, Log, TEXT("%s Leave"), ANSI_TO_TCHAR(__FUNCTION__));
	}

	virtual void TagTexture(FRHICommandList& CmdList, const FRHITextureWithRect& InResource, EStreamlineResource InResourceTag, uint32 InViewID) final
	{

		if(InResource.Texture && InResource.Texture->IsValid())
		{

			sl::Resource SLResource;
			FMemory::Memzero(SLResource);

			SLResource.native = InResource.Texture->GetNativeResource();

			switch (InResourceTag)
			{
				case EStreamlineResource::Depth:
					// note: subresources are in different states, we fix this up below
					// subresource 0 is D3D12_RESOURCE_STATE_DEPTH_READ|D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
					// subresource 1 is D3D12_RESOURCE_STATE_DEPTH_WRITE
					SLResource.state = D3D12_RESOURCE_STATE_DEPTH_READ|D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
					break;
				case EStreamlineResource::MotionVectors:
					SLResource.state = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
					break;
				case EStreamlineResource::HUDLessColor:
					SLResource.state = D3D12_RESOURCE_STATE_COPY_DEST;
					break;
				case EStreamlineResource::UIColorAndAlpha:
					SLResource.state = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
					break;
				default:
					checkf(false, TEXT("Unimplemented tag type (streamline plugin developer should fix)"));
					SLResource.state = D3D12_RESOURCE_STATE_COMMON;
					break;
			}

			sl::ResourceTag Tag;
			Tag.resource = &SLResource;
			Tag.type = ToSL(InResourceTag);
			// TODO: sl::ResourceLifecycle::eValidUntilPresent would be more efficient, are there any textures where it's applicable?
			Tag.lifecycle = sl::ResourceLifecycle::eOnlyValidNow;
			Tag.extent = ToSL(InResource.ViewRect);

#if ENGINE_PROVIDES_ID3D12DYNAMICRHI
			ID3D12GraphicsCommandList* NativeCmdList = D3D12RHI->RHIGetGraphicsCommandList(D3D12RHI->RHIGetResourceDeviceIndex(InResource.Texture));
#else
			FD3D12TextureBase* D3D12Texture = GetD3D12TextureFromRHITexture(InResource.Texture);
			FD3D12Device* Device = D3D12Texture->GetParentDevice();
			ID3D12CommandList* NativeCmdList = Device->GetDefaultCommandContext().CommandListHandle.CommandList();
#endif
#if ENGINE_PROVIDES_ID3D12DYNAMICRHI
			if (InResourceTag == EStreamlineResource::Depth)
			{
				// SL doesn't support subresources in different states, so temporarily make them consistent
				// This might lead to wasteful state transitions
				D3D12RHI->RHITransitionResource(CmdList, InResource.Texture, D3D12_RESOURCE_STATE_DEPTH_READ|D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, 1);
				SLsetTag(sl::ViewportHandle(InViewID), &Tag, 1, NativeCmdList);
				D3D12RHI->RHITransitionResource(CmdList, InResource.Texture, D3D12_RESOURCE_STATE_DEPTH_WRITE, 1);
			}
			else
#endif
			{
				SLsetTag(sl::ViewportHandle(InViewID), &Tag, 1, NativeCmdList);
			}
		}
		else
		{
			TagNullTexture(InResourceTag, InViewID);
		}
	}

	virtual const sl::AdapterInfo* GetAdapterInfo() override final
	{
		return &SLAdapterInfo;
	}

	virtual bool IsDLSSGSupportedByRHI() const override final
	{
		return true;
	}
	

	virtual void APIErrorHandler(const sl::APIError& LastError) final
	{
		// Not all DXGI return codes are errors, e.g. DXGI_STATUS_OCCLUDED
		if (IsDXGIStatus(LastError.hres))
		{
			return;
		}

		UE_LOG(LogStreamlineD3D12RHI, Log, TEXT("DLSSG D3D12/DXGI Error %d"), LastError.hres);


#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3
		D3D12RHI->RHIVerifyResult(static_cast<ID3D12Device*>(D3D12RHI->RHIGetNativeDevice()), LastError.hres, "Streamline/DLSSG present", __FILE__, __LINE__);
#else


	// that should be set in the 5.1 to 4.27 backport branches that have D3D12RHI_API for VerifyD3D12Result
	// and optionally a 5.2 NVRTX branch
#if!defined HAS_VERIFYD3D12_DLL_EXPORT
#define HAS_VERIFYD3D12_DLL_EXPORT (defined (ENGINE_STREAMLINE_VERSION) && ENGINE_STREAMLINE_VERSION >=3 ) 
#endif

#if IS_MONOLITHIC || HAS_VERIFYD3D12_DLL_EXPORT
		VerifyD3D12Result(LastError.hres, "Streamline/DLSSG present", __FILE__, __LINE__,static_cast<ID3D12Device*>(GDynamicRHI->RHIGetNativeDevice()));
#else
		using VerifyD3D12ResultPtrType = void (HRESULT, const ANSICHAR* , const ANSICHAR* , uint32 , ID3D12Device*, FString );
		VerifyD3D12ResultPtrType* VerifyD3D12ResultPtr = nullptr;
		const TCHAR* VerifyD3D12ResultDemangledName = TEXT("?VerifyD3D12Result@D3D12RHI@@YAXJPEBD0IPEAUID3D12Device@@VFString@@@Z");

		const FString D3D12RHIBinaryPath = FModuleManager::Get().GetModuleFilename(FName(TEXT("D3D12RHI")));
		void*D3D12BinaryDLL = FPlatformProcess::GetDllHandle(*D3D12RHIBinaryPath);

		VerifyD3D12ResultPtr = (VerifyD3D12ResultPtrType*)(FWindowsPlatformProcess::GetDllExport(D3D12BinaryDLL, VerifyD3D12ResultDemangledName));
		UE_LOG(LogStreamlineD3D12RHI, Log, TEXT("%s = %p"), VerifyD3D12ResultDemangledName, VerifyD3D12ResultPtr);

		if (VerifyD3D12ResultPtr)
		{
			VerifyD3D12ResultPtr(LastError.hres, "Streamline/DLSSG present", __FILE__, __LINE__, static_cast<ID3D12Device*>(GDynamicRHI->RHIGetNativeDevice()), FString());
		}
		else
		{
			UE_LOG(LogStreamlineD3D12RHI, Log, TEXT("Please add a D3D12RHI_API to the declaration of VerifyD3D12Result in D3D12Util.h to allow non monolithic builds to pipe handling of this error into the D3D12RHI DX/DXGI error handling system"));
		}
#endif

#endif
	}


protected:

	
private:
#if ENGINE_PROVIDES_ID3D12DYNAMICRHI
	ID3D12DynamicRHI* D3D12RHI = nullptr;
#else
	FD3D12DynamicRHI* D3D12RHI = nullptr;
#endif
	LUID AdapterLuid;
	sl::AdapterInfo SLAdapterInfo;
	TUniquePtr<FStreamlineD3D12DXGISwapchainProvider> CustomSwapchainProvider;

};


/** IModuleInterface implementation */

void FStreamlineD3D12RHIModule::StartupModule()
{
	auto CVarInitializePlugin = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Streamline.InitializePlugin"));
	if (CVarInitializePlugin && !CVarInitializePlugin->GetBool())
	{
		UE_LOG(LogStreamlineD3D12RHI, Log, TEXT("Initialization of StreamlineD3D12RHI is disabled."));
		return;
	}

	UE_LOG(LogStreamlineD3D12RHI, Log, TEXT("%s Enter"), ANSI_TO_TCHAR(__FUNCTION__));
	if(FApp::CanEverRender())
	{
		if ((GDynamicRHI != nullptr) && (GDynamicRHI->GetName() == FString("D3D12")))
		{
			FStreamlineRHIModule& StreamlineRHIModule = FModuleManager::LoadModuleChecked<FStreamlineRHIModule>(TEXT("StreamlineRHI"));
			if (AreStreamlineFunctionsLoaded())
			{
				StreamlineRHIModule.InitializeStreamline();
				if (IsStreamlineSupported())
				{
					sl::Result Result = SLsetD3DDevice(GDynamicRHI->RHIGetNativeDevice());
					checkf(Result == sl::Result::eOk, TEXT("%s: SLsetD3DDevice failed (%s)"), ANSI_TO_TCHAR(__FUNCTION__), ANSI_TO_TCHAR(sl::getResultAsStr(Result)));
				}
			}
		}
		else
		{
			UE_LOG(LogStreamlineD3D12RHI, Log, TEXT("D3D12RHI is not the active DynamicRHI; skipping of setting up the custom swapchain factory"));
		}
	}
	else
	{
		UE_LOG(LogStreamlineD3D12RHI, Log, TEXT("This UE instance does not render, skipping initalizing of Streamline and registering of custom DXGI and D3D12 functions"));
	}
	UE_LOG(LogStreamlineD3D12RHI, Log, TEXT("%s Leave"), ANSI_TO_TCHAR(__FUNCTION__));
}

void FStreamlineD3D12RHIModule::ShutdownModule()
{
	auto CVarInitializePlugin = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Streamline.InitializePlugin"));
	if (CVarInitializePlugin && !CVarInitializePlugin->GetBool())
	{
		return;
	}

	UE_LOG(LogStreamlineD3D12RHI, Log, TEXT("%s Enter"), ANSI_TO_TCHAR(__FUNCTION__));
	UE_LOG(LogStreamlineD3D12RHI, Log, TEXT("%s Leave"), ANSI_TO_TCHAR(__FUNCTION__));
}

TUniquePtr<FStreamlineRHI> FStreamlineD3D12RHIModule::CreateStreamlineRHI(const FStreamlineRHICreateArguments& Arguments)
{
	TUniquePtr<FStreamlineRHI> Result(new FStreamlineD3D12RHI(Arguments));
	return Result;
}

IMPLEMENT_MODULE(FStreamlineD3D12RHIModule, StreamlineD3D12RHI )
#undef LOCTEXT_NAMESPACE
