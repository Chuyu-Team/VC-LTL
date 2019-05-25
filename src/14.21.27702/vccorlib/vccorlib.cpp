//
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
#include "pch.h"

CPPCLI_FUNC void __stdcall FlushFactoryCache();
CPPCLI_FUNC void EnableFactoryCache();
void DisableFactoryCache();

__abi_Module* __abi_module = nullptr;

#include "crtdefs.h"
#include <corecrt_startup.h>

__declspec(noreturn) __declspec(dllexport) void __stdcall __abi_FailFast()
{
	_invoke_watson(nullptr, nullptr, nullptr, 0, 0);
}

namespace Platform {
	namespace Details {

		class InternalModule :
			public __abi_Module
		{
		public:
			InternalModule()
			{
				__abi_module = this;
			}

			virtual ~InternalModule()
			{
				__abi_module = nullptr;
			}

			virtual unsigned long __stdcall __abi_IncrementObjectCount()
			{
				return 0;
			}

			virtual unsigned long __stdcall __abi_DecrementObjectCount()
			{
				return 0;
			}
		};

#pragma warning( disable: 4073 )  // initializers put in library initialization area
#pragma init_seg( lib )

		InternalModule internalModule_;

		VCCORLIB_API HRESULT InitializeData(int threadingModel)
		{
			if (threadingModel != _VCCORLIB_THREADING_MODEL_DEFAULT)
			{
				RO_INIT_TYPE initType = threadingModel == _VCCORLIB_THREADING_MODEL_STA
					? static_cast<RO_INIT_TYPE>(0) /* RO_INIT_SINGLETHREADED */
					: RO_INIT_MULTITHREADED;

				HRESULT hr = Windows::Foundation::Initialize(initType);
				if (FAILED(hr))
				{
#ifdef _DEBUG
					if (IsDebuggerPresent())
					{
						OutputDebugStringW(L"Call to Windows::Foundation::Initialize failed.");
					}
#endif
					return hr;
				}

				EnableFactoryCache();
			}
			return S_OK;
		}

		VCCORLIB_API void UninitializeData(int threadingModel)
		{
			if (threadingModel != 0)
			{
				DisableFactoryCache();
				Windows::Foundation::Uninitialize();
			}
		}

		VCCORLIB_API HRESULT STDMETHODCALLTYPE GetActivationFactory(
			_In_ Microsoft::WRL::Details::ModuleBase* module,
			_In_ HSTRING pActivatibleClassId,
			_Deref_out_ ::IActivationFactory** ppFactory)
		{
			return Microsoft::WRL::Details::GetActivationFactory<Microsoft::WRL::InProcDisableCaching>(module, nullptr, pActivatibleClassId, ppFactory);
		}

		VCCORLIB_API bool STDMETHODCALLTYPE TerminateModule(_In_ Microsoft::WRL::Details::ModuleBase* module)
		{
			return Microsoft::WRL::Details::TerminateMap(module, nullptr, false);
		}

VCCORLIB_API wchar_t** GetCmdArguments(_Out_ int* argc)
{
	_ASSERTE(argc != nullptr);

	if (_configure_wide_argv(_get_startup_argv_mode()) != 0)
	{
		*argc = 0;
		return nullptr;
	}

	*argc = __argc;
	return __wargv;
}


	} } // namespace Platform::Details
