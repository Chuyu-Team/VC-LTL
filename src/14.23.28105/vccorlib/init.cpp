//
// Copyright (C) Microsoft Corporation
// All rights reserved.
//

#include "pch.h"
#pragma warning( disable: 4073 )  // initializers put in library initialization area
#pragma init_seg( lib )

#include <RestrictedErrorInfo.h>
#include <roerrorapi.h>

int __abi___threading_model = _VCCORLIB_THREADING_MODEL_DEFAULT;
__abi_Module* __abi_module = nullptr;

#ifndef _VCCORLIB_APP
	typedef HRESULT (__stdcall *PFN_RO_REPORT_UNHANDLED_ERROR)(_In_ IRestrictedErrorInfo * pRestrictedErrorInfo);

	BOOL bTestedRoReportUnhandledError = false;
	PFN_RO_REPORT_UNHANDLED_ERROR RoReportUnhandledErrorFunc = nullptr;

	STDAPI RoReportUnhandledError(_In_ IRestrictedErrorInfo * pRestrictedErrorInfo)
	{
		if (!bTestedRoReportUnhandledError)
		{
			HMODULE hMod = LoadLibraryExW(L"combase.dll", nullptr, 0);
			if (hMod)
			{
				RoReportUnhandledErrorFunc = (PFN_RO_REPORT_UNHANDLED_ERROR)GetProcAddress(hMod, "RoReportUnhandledError");
			}
			bTestedRoReportUnhandledError = true;
		}

		if (RoReportUnhandledErrorFunc)
		{
			return RoReportUnhandledErrorFunc(pRestrictedErrorInfo);
		}
		return S_OK;
	}
#endif

namespace Platform {

	// The helpers should be unified with exception code
	typedef struct _WinRTExceptionInfo
	{
		BSTR description;
		BSTR restrictedErrorString;
		BSTR restrictedErrorReference;
		BSTR capabilitySid;
		HRESULT hr;
		::Microsoft::WRL::ComPtr<IRestrictedErrorInfo> restrictedInfo;
		void* throwInfo;   // Do not need to free - this points to a static part of our binary.
		unsigned int size;
		typedef void* (__stdcall *PFNPREPARE_FOR_THROW)(void* pException);
	} WinRTExceptionInfo;

	inline WinRTExceptionInfo* GetWinRTExceptionInfo(void* exceptionPtr)
	{
		ULONG_PTR *exceptionInfoPointer = reinterpret_cast<ULONG_PTR*>(exceptionPtr);
		exceptionInfoPointer--;
		return reinterpret_cast<WinRTExceptionInfo*>(*exceptionInfoPointer);
	}

	namespace Details {

		class InProcModule :
			public Microsoft::WRL::Module<Microsoft::WRL::InProcDisableCaching, InProcModule>,
			public __abi_Module
		{
		public:
			InProcModule()
			{
				__abi_module = this;
			}

			virtual ~InProcModule()
			{
				__abi_module = nullptr;
			}

			virtual unsigned long __stdcall __abi_IncrementObjectCount()
			{
				return Module::IncrementObjectCount();
			}

			virtual unsigned long __stdcall __abi_DecrementObjectCount()
			{
				return Module::DecrementObjectCount();
			}
		};

		auto &module_ = InProcModule::Create();


		// Forward declarations from vccorlib DLL
		HRESULT InitializeData(int __threading_model);
		void UninitializeData(int __threading_model);

		//Initializaton forwarders
		void __cdecl Cleanup();

		// Initialize MoCOM data and clean up handlers
		int __cdecl Initialize()
		{
			// If initialization of the threading model was overridden, behave
			// as if no threading model was specified.
			if (__abi___threading_model == _VCCORLIB_THREADING_MODEL_OVERRIDE)
			{
				__abi___threading_model = _VCCORLIB_THREADING_MODEL_DEFAULT;
			}

			//Global initialization
			HRESULT hr = InitializeData(__abi___threading_model);
			if (FAILED(hr))
			{
				Cleanup();
				return hr;
			}

			// Register cleaning handler
			atexit(Cleanup);
			return 0;
		}

		// Global clean up
		void __cdecl Cleanup()
		{
			UninitializeData(__abi___threading_model);
		}

		void __cdecl ReportUnhandledError( ::Platform::Exception^ ex)
		{
			auto exInfo = ::Platform::GetWinRTExceptionInfo(reinterpret_cast<void*>(const_cast< ::Platform::Object^>(ex)));
			_ASSERTE(exInfo != nullptr);

			if (exInfo->restrictedInfo != nullptr)
			{
				::RoReportUnhandledError(exInfo->restrictedInfo.Get());
				// IRestrictedErrorInfo should be put back
				// to make sure that RoFailFastWithErrorContext can extract the information
				::SetRestrictedErrorInfo(exInfo->restrictedInfo.Get());
			}

			::RoFailFastWithErrorContext(exInfo->hr);
		}

	} } // namespace Platform::Details

//Put initialization data into the section that we can return failure
#pragma section(".CRT$XIYB", long, read)
extern "C" __declspec(allocate(".CRT$XIYB")) void* __abi__initialize = Platform::Details::Initialize;

#ifdef _VCCORLIB_APP
// If C++/CX is used with App CRT than VCCORLIB must initialize WinRt
// thus setting __crtWinrtInitType that disable App CRT WinRt initialization
extern "C" int __crtWinrtInitType = 0;
#pragma detect_mismatch("vccorlib_lib_should_be_specified_before_msvcrt_lib_to_linker", "1")
#endif

// All required libraries must be pulled in in init.cpp file because it always referenced
// The libraries should not be pulled in vccorlib.h unless it's vccorlib*.lib
#if !defined(_ONECORE)
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "runtimeobject.lib")
#endif
