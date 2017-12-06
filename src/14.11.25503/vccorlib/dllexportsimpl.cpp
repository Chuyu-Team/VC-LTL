//
// Copyright (C) Microsoft Corporation
// All rights reserved.
//

#include "pch.h"

namespace Platform {
	namespace Details {

		//Forward declarations from vccorlib120.dll
		HRESULT STDMETHODCALLTYPE GetActivationFactory(_In_ Microsoft::WRL::Details::ModuleBase*, _In_ HSTRING, _Deref_out_ ::IActivationFactory**);
		bool STDMETHODCALLTYPE TerminateModule(_In_ Microsoft::WRL::Details::ModuleBase*);

	} // namespace Details

	::Platform::Details::IActivationFactory^ STDMETHODCALLTYPE Module::GetActivationFactory(::Platform::String^ activatibleClassId)
	{
		::Platform::Details::IActivationFactory^ factory = nullptr;
		HRESULT hr = Details::GetActivationFactory(
			Microsoft::WRL::Details::ModuleBase::module_,
			reinterpret_cast<HSTRING>(activatibleClassId),
			reinterpret_cast<::IActivationFactory**>(&factory));
		__abi_ThrowIfFailed(hr);
		return reinterpret_cast<::Platform::Details::IActivationFactory^>(factory);
	}

	bool STDMETHODCALLTYPE Module::CanUnloadNow()
	{
		return Details::TerminateModule(Microsoft::WRL::Details::ModuleBase::module_);
	}

} // namespace Platform
