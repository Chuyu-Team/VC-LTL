//
// Copyright (C) Microsoft Corporation
// All rights reserved.
//

#include "pch.h"
#pragma hdrstop

#include <ctxtcall.h>

namespace Platform
{
	namespace Details
	{
#pragma warning(push)
#pragma warning(disable: 4510) // default constructor could not be generated
#pragma warning(disable: 4512) // assignment operator could not be generated
#pragma warning(disable: 4610) // can never be instantiated - user defined constructor required

		struct __CxxMarshalData
		{
			REFIID iid;
			IUnknown* ObjectIn;
			IStream *ObjectProxy;
		};

#pragma warning(pop)

		VCCORLIB_API IUnknown* __stdcall GetObjectContext()
		{
			IUnknown* temp;
			__abi_ThrowIfFailed(CoGetObjectContext(__uuidof(IContextCallback), reinterpret_cast<void**>(&temp)));
			return temp;
		}

		HRESULT __stdcall MarshallToStream(ComCallData *pParam)
		{
			__CxxMarshalData* pMarshalData = static_cast<__CxxMarshalData*>(pParam->pUserDefined);
			if (pMarshalData == nullptr)
			{
				return E_INVALIDARG;
			}
			return CoMarshalInterThreadInterfaceInStream(pMarshalData->iid, pMarshalData->ObjectIn, &pMarshalData->ObjectProxy);
		}

		HRESULT __stdcall ReleaseInContextCallBack(ComCallData *pParam)
		{
			__CxxMarshalData* pMarshalData = static_cast<__CxxMarshalData*>(pParam->pUserDefined);
			if (pMarshalData == nullptr)
			{
				return E_INVALIDARG;
			}
			pMarshalData->ObjectIn->Release();
			return S_OK;
		}

		VCCORLIB_API HRESULT __stdcall GetProxyImpl(IUnknown *ObjectIn, REFIID iid, IUnknown *ContextCallBack, IUnknown **Proxy)
		{
			__CxxMarshalData MarshalData = { iid, ObjectIn, nullptr };

			ComCallData callData;
			memset(&callData, 0, sizeof(callData));
			callData.pUserDefined = &MarshalData;

			::Microsoft::WRL::ComPtr<IContextCallback> pContextCallback;
			HRESULT hr = ContextCallBack->QueryInterface(pContextCallback.GetAddressOf());
			if (SUCCEEDED(hr))
			{
				hr = pContextCallback->ContextCallback(MarshallToStream, &callData, __uuidof(IContextCallback), 5, nullptr);
				if (SUCCEEDED(hr))
				{
					hr = CoGetInterfaceAndReleaseStream(MarshalData.ObjectProxy, iid, reinterpret_cast<void**>(Proxy));
				}
			}
			return hr;
		}

		VCCORLIB_API HRESULT __stdcall ReleaseInContextImpl(IUnknown *ObjectIn, IUnknown *ContextCallBack)
		{
			__CxxMarshalData MarshalData = { __winrt_GUID_NULL, ObjectIn, nullptr };

			ComCallData callData;
			memset(&callData, 0, sizeof(callData));
			callData.pUserDefined = &MarshalData;

			::Microsoft::WRL::ComPtr<IContextCallback> pContextCallback;
			HRESULT hr = ContextCallBack->QueryInterface(pContextCallback.GetAddressOf());
			if (SUCCEEDED(hr))
			{
				hr = pContextCallback->ContextCallback(ReleaseInContextCallBack, &callData, __uuidof(IContextCallback), 5, nullptr);
			}
			return hr;
		}

} } // namespace Platform::Details
