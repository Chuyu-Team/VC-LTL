//
// Copyright (C) Microsoft Corporation
// All rights reserved.
//

#include "pch.h"

namespace Platform {
	namespace Details {

		// Forward declarations from vccorlib DLL
		HRESULT STDMETHODCALLTYPE GetActivationFactory(_In_ Microsoft::WRL::Details::ModuleBase*, _In_ HSTRING, _Deref_out_ ::IActivationFactory**);
		bool STDMETHODCALLTYPE TerminateModule(_In_ Microsoft::WRL::Details::ModuleBase*);

	}} // namespace Platform::Application::Details

STDAPI DllGetActivationFactory(_In_ HSTRING activatibleClassId, _Deref_out_ IActivationFactory** ppFactory)
{
	return Platform::Details::GetActivationFactory(Microsoft::WRL::Details::ModuleBase::module_, activatibleClassId, ppFactory);
}

STDAPI DllCanUnloadNow()
{
	return Platform::Details::TerminateModule(Microsoft::WRL::Details::ModuleBase::module_) ? S_OK : S_FALSE;
}
