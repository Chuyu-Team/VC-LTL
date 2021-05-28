//
// Copyright (C) Microsoft Corporation
// All rights reserved.
//

#include "pch.h"
#pragma hdrstop

#include <memory>

#include <RestrictedErrorInfo.h>
#include <roerrorapi.h>
#include <string>

#pragma warning(disable:4483)

template <class _E> void *__GetExceptionInfo(_E); // This is a built-in c1xx template function to get the
// ThrowInfo for a given struct (the structure passed by the compiler's 'throw' statement to the CRT.
// The ThrowInfo is statically compiled into the binary e.g. '__TI9P$AAVException@Platform@@' and
// GetException() returns the address of this structure.
// The CRT also uses this internally to transfer exceptions between threads.

// This function is used to extract _WinRTExceptionInfo structure from the current exception
extern "C" _CRTIMP void * __GetPlatformExceptionInfo(int *);

#ifndef _VCCORLIB_APP
	typedef BOOL (__stdcall *PFN_RO_ORIGINATE_LANGUAGE_EXCEPTION)(
		_In_ HRESULT error,
		_In_opt_ HSTRING message,
		_In_ IUnknown * languageException
		);

	BOOL bTestedRoOriginateLanguageException = false;
	PFN_RO_ORIGINATE_LANGUAGE_EXCEPTION RoOriginateLanguageExceptionFunc = nullptr;

	STDAPI_(BOOL) RoOriginateLanguageException(
		_In_ HRESULT error,
		_In_opt_ HSTRING message,
		_In_ IUnknown * languageException
		)
	{
		if (!bTestedRoOriginateLanguageException)
		{
			HMODULE hMod = LoadLibraryExW(L"combase.dll", nullptr, 0);
			if (hMod)
			{
				RoOriginateLanguageExceptionFunc = (PFN_RO_ORIGINATE_LANGUAGE_EXCEPTION)GetProcAddress(hMod, "RoOriginateLanguageException");
			}
			bTestedRoOriginateLanguageException = true;
		}

		if (RoOriginateLanguageExceptionFunc)
		{
			return RoOriginateLanguageExceptionFunc(error, message, languageException);
		}
		return FALSE;
	}

#endif // _VCCORLIB_APP

namespace Platform { namespace Details {
	extern bool  __abi_firstAlloc;
	extern bool  __abi_is_global_oom_init;
	extern void* __abi_oom_controlblock;
	extern IUnknown* __abi_oom_singleton;
} }

namespace Platform
{
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

	WinRTExceptionInfo* GetWinRTExceptionInfo(void* exceptionPtr)
	{
		ULONG_PTR *exceptionInfoPointer = reinterpret_cast<ULONG_PTR*>(exceptionPtr);
		exceptionInfoPointer--;
		return reinterpret_cast<WinRTExceptionInfo*>(*exceptionInfoPointer);
	}

	template <typename T>
	void UpdateThrowInfo(T ex)
	{
		auto exInfo = GetWinRTExceptionInfo(reinterpret_cast<void*>(const_cast< ::Platform::Object^>(ex)));
		_ASSERTE(exInfo != nullptr);
		exInfo->throwInfo = __GetExceptionInfo(ex);
	}

	namespace Details {

		HRESULT AssignErrorInfo(WinRTExceptionInfo* exInfo)
		{
			_ASSERTE(exInfo != nullptr);

			::Microsoft::WRL::ComPtr<IRestrictedErrorInfo> restrictedErrorInfo;
			HRESULT hr = ::GetRestrictedErrorInfo(restrictedErrorInfo.GetAddressOf());

			if (hr == S_OK)
			{
				exInfo->restrictedInfo = restrictedErrorInfo;

				HRESULT hr = restrictedErrorInfo->GetErrorDetails(&exInfo->description, &exInfo->hr, &exInfo->restrictedErrorString, &exInfo->capabilitySid);
				if (SUCCEEDED(hr))
				{
					if (exInfo->restrictedErrorString == nullptr)
					{
						restrictedErrorInfo->GetReference(&exInfo->restrictedErrorReference);
					}
				}

				return S_OK;
			}

			return hr;
		}

		// ReCreateException helper is called when the HRESULT is comming from outside the boundry
		// The exception is created and in case that IRestrictedErrorInfo exists it's assigned to exception otherwise OriginateError
		// is called. This creates new IRestrictedErrorInfo object that is assigned to exception.
		::Platform::Exception^ ReCreateException(long hrEx)
		{
			auto ex = Exception::CreateException(hrEx);
			auto exInfo = GetWinRTExceptionInfo(reinterpret_cast<void*>(const_cast< ::Platform::Object^>(ex)));
			_ASSERTE(exInfo != nullptr);
			HRESULT hr = AssignErrorInfo(exInfo);
			if (hr != S_OK)
			{
				if (::Windows::Foundation::Diagnostics::OriginateError(hrEx, nullptr))
				{
					AssignErrorInfo(exInfo);
				}
			}
			else if (exInfo->hr != hrEx)
			{
				::Windows::Foundation::Diagnostics::TransformError(exInfo->hr, hrEx, nullptr);
				AssignErrorInfo(exInfo);
			}

			return ex;
		}
	}

	void __stdcall PrepareForThrow(WinRTExceptionInfo** ppWei)
	{
#ifdef _M_ARM64
		(void) ppWei;
// TODO-_M_ARM64
#else // ^^^ _M_ARM64 defined ^^^ / vvv _M_ARM64 not defined vvv
		// Context information were cleared by call to SetRestrictedErrorInfo in SetCppExceptionInformation
		// ::RoCaptureErrorContext(pThis->hr);
		WinRTExceptionInfo* pThis = *reinterpret_cast<WinRTExceptionInfo**>(ppWei);

		// If it's new exception we need to clear restricted error info
		// and call to RoOriginateLanguageException otherwise the structure is already set
		// For out of memory exception we have to reset the value every time
		if (pThis->restrictedInfo == nullptr || pThis->hr == E_OUTOFMEMORY)
		{
			::SetRestrictedErrorInfo(nullptr); //Clear IRestrictedErrorInfo
			if (pThis->restrictedErrorString)
			{
				::RoOriginateLanguageException(pThis->hr, Platform::StringReference(pThis->restrictedErrorString).GetHSTRING(), nullptr);
			}
			else
			{
				::RoOriginateLanguageException(pThis->hr, nullptr, nullptr);
			}

			Details::AssignErrorInfo(pThis);
		}
#endif // ^^^ _M_ARM64 not defined ^^^
	}

	void (__stdcall *g_PrepareForThrowPtr)(WinRTExceptionInfo** ppExceptionInfo) = PrepareForThrow;

	VCCORLIB_API Exception::Exception(int32 hresult)
	{
		__description = nullptr;
		__restrictedErrorString = nullptr;
		__restrictedErrorReference = nullptr;
		__capabilitySid = nullptr;
		__hresult = hresult;
		__restrictedInfo = nullptr;
		__throwInfo = nullptr;
		__size = sizeof(WinRTExceptionInfo);
		__prepare = g_PrepareForThrowPtr;

		if (SUCCEEDED(hresult))
		{
			throw ref new ::Platform::InvalidArgumentException();
		}

		ULONG_PTR *exceptionInfoPointer = reinterpret_cast<ULONG_PTR*>(this);
		exceptionInfoPointer--;
		*exceptionInfoPointer = reinterpret_cast<ULONG_PTR>(&__description);

		UpdateThrowInfo(this);
	}

	VCCORLIB_API Exception::Exception(int32 hresult, ::Platform::String^ message)
	{
		__description = nullptr;
		__restrictedErrorString = nullptr;
		__restrictedErrorReference = nullptr;
		__capabilitySid = nullptr;
		__hresult = hresult;
		__restrictedInfo = nullptr;
		__throwInfo = nullptr;
		__size = sizeof(WinRTExceptionInfo);
		__prepare = g_PrepareForThrowPtr;

		if (SUCCEEDED(hresult))
		{
			throw ref new ::Platform::InvalidArgumentException();
		}

		BOOL hasEmbedNull;
		if (FAILED(::WindowsStringHasEmbeddedNull(reinterpret_cast<HSTRING>((message)), &hasEmbedNull)) || hasEmbedNull)
		{
			throw ref new ::Platform::InvalidArgumentException();
		}

		__restrictedErrorString = ::SysAllocStringLen(message->Data(), message->Length());
		if (!__restrictedErrorString)
		{
			throw reinterpret_cast<Platform::OutOfMemoryException^>(Platform::Details::__abi_oom_singleton);
		}

		ULONG_PTR *exceptionInfoPointer = reinterpret_cast<ULONG_PTR*>(this);
		exceptionInfoPointer--;
		*exceptionInfoPointer = reinterpret_cast<ULONG_PTR>(&__description);

		UpdateThrowInfo(this);
	}

	VCCORLIB_API void Exception::__identifier("<Dispose>")()
	{
		auto exInfo = GetWinRTExceptionInfo(reinterpret_cast<void*>(const_cast< ::Platform::Object^>(this)));

		if (exInfo != nullptr)
		{
			::SysFreeString(exInfo->description);
			::SysFreeString(exInfo->restrictedErrorString);
			::SysFreeString(exInfo->capabilitySid);
			::SysFreeString(exInfo->restrictedErrorReference);
			exInfo->restrictedInfo = nullptr;
		}
	}

	VCCORLIB_API Platform::String^ Exception::ToString()
	{
		return this->Message;
	}

	VCCORLIB_API bool Exception::Equals(Platform::Object^ other)
	{
		if (this == nullptr)
		{
			throw ref new ::Platform::NullReferenceException();
		}

		Exception ^ex = dynamic_cast<Exception^>(other);
		if (ex == nullptr)
		{
			return false;
		}

		return __hresult == ex->HResult;
	}

	VCCORLIB_API int32 Exception::GetHashCode()
	{
		return __hresult;
	}

	VCCORLIB_API Platform::String^ Exception::Message::get()
	{
		auto exInfo = GetWinRTExceptionInfo(reinterpret_cast<void*>(const_cast< ::Platform::Object^>(this)));
		_ASSERTE(exInfo != nullptr);

		std::wstring ws;
		if (exInfo->description && *exInfo->description != L'\0')
		{
			ws += exInfo->description;
		}

		if (exInfo->restrictedErrorString && *exInfo->restrictedErrorString != L'\0')
		{
			if (!ws.empty())
			{
				ws += L"\r\n";
			}
			ws += exInfo->restrictedErrorString;
		}

		if (!ws.empty())
		{
			return ref new String(ws.c_str());
		}

		wchar_t buffer[4096];
		if (::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM,
			nullptr,
			__hresult,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
			buffer,
			_countof(buffer),
			nullptr))
		{
			return ref new Platform::String(buffer);
		}

		return nullptr;
	}

	VCCORLIB_API COMException::COMException(int32 hresult) : Exception(hresult)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API COMException::COMException(int32 hresult, ::Platform::String^ message) : Exception(hresult, message)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API OutOfMemoryException::OutOfMemoryException() : COMException(E_OUTOFMEMORY)
	{
		if (Platform::Details::__abi_is_global_oom_init)
		{
			Platform::Details::__abi_oom_controlblock = this->__abi_reference_count.__weakRefSource;
		}
		UpdateThrowInfo(this);
	}

	VCCORLIB_API OutOfMemoryException::OutOfMemoryException(::Platform::String^ message) : COMException(E_OUTOFMEMORY, message)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API InvalidArgumentException::InvalidArgumentException() : COMException(E_INVALIDARG)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API InvalidArgumentException::InvalidArgumentException(::Platform::String^ message) : COMException(E_INVALIDARG, message)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API InvalidCastException::InvalidCastException() : COMException(E_NOINTERFACE)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API InvalidCastException::InvalidCastException(::Platform::String^ message) : COMException(E_NOINTERFACE, message)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API NullReferenceException::NullReferenceException() : COMException(E_POINTER)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API NullReferenceException::NullReferenceException(::Platform::String^ message) : COMException(E_POINTER, message)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API NotImplementedException::NotImplementedException() : COMException(E_NOTIMPL)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API NotImplementedException::NotImplementedException(::Platform::String^ message) : COMException(E_NOTIMPL, message)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API AccessDeniedException::AccessDeniedException() : COMException(E_ACCESSDENIED)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API AccessDeniedException::AccessDeniedException(::Platform::String^ message) : COMException(E_ACCESSDENIED, message)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API FailureException::FailureException() : COMException(E_FAIL)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API FailureException::FailureException(::Platform::String^ message) : COMException(E_FAIL, message)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API OutOfBoundsException::OutOfBoundsException() : COMException(E_BOUNDS)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API OutOfBoundsException::OutOfBoundsException(::Platform::String^ message) : COMException(E_BOUNDS, message)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API ChangedStateException::ChangedStateException() : COMException(E_CHANGED_STATE)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API ChangedStateException::ChangedStateException(::Platform::String^ message) : COMException(E_CHANGED_STATE, message)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API OperationCanceledException::OperationCanceledException() : COMException(E_ABORT)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API OperationCanceledException::OperationCanceledException(::Platform::String^ message) : COMException(E_ABORT, message)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API ClassNotRegisteredException::ClassNotRegisteredException() : COMException(REGDB_E_CLASSNOTREG)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API ClassNotRegisteredException::ClassNotRegisteredException(::Platform::String^ message) : COMException(REGDB_E_CLASSNOTREG, message)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API DisconnectedException::DisconnectedException() : COMException(RPC_E_DISCONNECTED)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API DisconnectedException::DisconnectedException(::Platform::String^ message) : COMException(RPC_E_DISCONNECTED, message)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API WrongThreadException::WrongThreadException() : COMException(RPC_E_WRONG_THREAD)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API WrongThreadException::WrongThreadException(::Platform::String^ message) : COMException(RPC_E_WRONG_THREAD, message)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API ObjectDisposedException::ObjectDisposedException() : COMException(RO_E_CLOSED)
	{
		UpdateThrowInfo(this);
	}

	VCCORLIB_API ObjectDisposedException::ObjectDisposedException(::Platform::String^ message) : COMException(RO_E_CLOSED, message)
	{
		UpdateThrowInfo(this);
	}

} // namespace Platform

#pragma warning(push)
#pragma warning(disable: 4672 4673)
// The exceptions are split out explicitly in order to make them obvious from callstacks.
VCCORLIB_API __declspec(noreturn) inline void __stdcall __abi_WinRTraiseNotImplementedException()
{
	throw ::Platform::Details::ReCreateException(E_NOTIMPL);
}

VCCORLIB_API __declspec(noreturn) inline void __stdcall __abi_WinRTraiseInvalidCastException()
{
	throw ::Platform::Details::ReCreateException(E_NOINTERFACE);
}

VCCORLIB_API __declspec(noreturn) inline void __stdcall __abi_WinRTraiseNullReferenceException()
{
	throw ::Platform::Details::ReCreateException(E_POINTER);
}

VCCORLIB_API __declspec(noreturn) inline void __stdcall __abi_WinRTraiseOperationCanceledException()
{
	throw ::Platform::Details::ReCreateException(E_ABORT);
}

VCCORLIB_API __declspec(noreturn) inline void __stdcall __abi_WinRTraiseFailureException()
{
	throw ::Platform::Details::ReCreateException(E_FAIL);
}

VCCORLIB_API __declspec(noreturn) inline void __stdcall __abi_WinRTraiseAccessDeniedException()
{
	throw ::Platform::Details::ReCreateException(E_ACCESSDENIED);
}

VCCORLIB_API __declspec(noreturn) inline void __stdcall __abi_WinRTraiseOutOfMemoryException()
{
	throw ::Platform::Details::ReCreateException(E_OUTOFMEMORY);
}

VCCORLIB_API __declspec(noreturn) inline void __stdcall __abi_WinRTraiseInvalidArgumentException()
{
	throw ::Platform::Details::ReCreateException(E_INVALIDARG);
}

VCCORLIB_API __declspec(noreturn) inline void __stdcall __abi_WinRTraiseOutOfBoundsException()
{
	throw ::Platform::Details::ReCreateException(E_BOUNDS);
}

VCCORLIB_API __declspec(noreturn) inline void __stdcall __abi_WinRTraiseChangedStateException()
{
	throw ::Platform::Details::ReCreateException(E_CHANGED_STATE);
}

VCCORLIB_API __declspec(noreturn) inline void __stdcall __abi_WinRTraiseClassNotRegisteredException()
{
	throw ::Platform::Details::ReCreateException(REGDB_E_CLASSNOTREG);
}

VCCORLIB_API __declspec(noreturn) inline void __stdcall __abi_WinRTraiseWrongThreadException()
{
	throw ::Platform::Details::ReCreateException(RPC_E_WRONG_THREAD);
}

VCCORLIB_API __declspec(noreturn) inline void __stdcall __abi_WinRTraiseDisconnectedException()
{
	throw ::Platform::Details::ReCreateException(RPC_E_DISCONNECTED);
}

VCCORLIB_API __declspec(noreturn) inline void __stdcall __abi_WinRTraiseObjectDisposedException()
{
	throw ::Platform::Details::ReCreateException(RO_E_CLOSED);
}

VCCORLIB_API __declspec(noreturn) inline void __stdcall __abi_WinRTraiseCOMException(long hr)
{
	throw ::Platform::Details::ReCreateException(hr);
}

// __abi_translateCurrentException is called when exceptions are translated to HRESULT's
VCCORLIB_API long __stdcall __abi_translateCurrentException(bool translateBadAlloc)
{
	int isBadAlloc = 0;

	auto exinfo = reinterpret_cast< ::Platform::WinRTExceptionInfo*>(__GetPlatformExceptionInfo(&isBadAlloc));

	if (isBadAlloc && translateBadAlloc)
	{
		::Windows::Foundation::Diagnostics::OriginateError(E_OUTOFMEMORY, nullptr);
		return E_OUTOFMEMORY;
	}
	else if (exinfo != nullptr)
	{
		if (exinfo->restrictedInfo != nullptr)
		{
			::SetRestrictedErrorInfo(exinfo->restrictedInfo.Get());
		}

		return exinfo->hr;
	}

	__abi_FailFast();
}

#pragma warning(pop)

namespace Platform
{

	VCCORLIB_API Platform::Exception^ Exception::ReCreateException(int32 hresult)
	{
		return ::Platform::Details::ReCreateException(hresult);
	}

	VCCORLIB_API Platform::Exception^ Exception::CreateException(int32 hr)
	{
		Exception^ ex;

		switch (hr)
		{
		case E_NOTIMPL:
			ex = ref new NotImplementedException();
			break;
		case E_NOINTERFACE:
			ex = ref new InvalidCastException();
			break;
		case E_POINTER:
			ex = ref new NullReferenceException();
			break;
		case E_ABORT:
			ex = ref new OperationCanceledException();
			break;
		case E_FAIL:
			ex = ref new FailureException();
			break;
		case E_ACCESSDENIED:
			ex = ref new AccessDeniedException();
			break;
		case E_OUTOFMEMORY:
			ex = ref new OutOfMemoryException();
			break;
		case E_INVALIDARG:
			ex = ref new InvalidArgumentException();
			break;
		case E_BOUNDS:
			ex = ref new OutOfBoundsException();
			break;
		case E_CHANGED_STATE:
			ex = ref new ChangedStateException();
			break;
		case REGDB_E_CLASSNOTREG:
			ex = ref new ClassNotRegisteredException();
			break;
		case RPC_E_WRONG_THREAD:
			ex = ref new WrongThreadException();
			break;
		case RPC_E_DISCONNECTED:
			ex = ref new DisconnectedException();
			break;
		case RO_E_CLOSED:
			ex = ref new ObjectDisposedException();
			break;
		default:
			ex = ref new COMException(hr);
			break;
		}

		return ex;
	}

	VCCORLIB_API Platform::Exception^ Exception::CreateException(int32 hr, ::Platform::String^ message)
	{
		Exception^ ex;

		switch (hr)
		{
		case E_NOTIMPL:
			ex = ref new NotImplementedException(message);
			break;
		case E_NOINTERFACE:
			ex = ref new InvalidCastException(message);
			break;
		case E_POINTER:
			ex = ref new NullReferenceException(message);
			break;
		case E_ABORT:
			ex = ref new OperationCanceledException(message);
			break;
		case E_FAIL:
			ex = ref new FailureException(message);
			break;
		case E_ACCESSDENIED:
			ex = ref new AccessDeniedException(message);
			break;
		case E_OUTOFMEMORY:
			ex = ref new OutOfMemoryException(message);
			break;
		case E_INVALIDARG:
			ex = ref new InvalidArgumentException(message);
			break;
		case E_BOUNDS:
			ex = ref new OutOfBoundsException(message);
			break;
		case E_CHANGED_STATE:
			ex = ref new ChangedStateException(message);
			break;
		case REGDB_E_CLASSNOTREG:
			ex = ref new ClassNotRegisteredException(message);
			break;
		case RPC_E_WRONG_THREAD:
			ex = ref new WrongThreadException(message);
			break;
		case RPC_E_DISCONNECTED:
			ex = ref new DisconnectedException(message);
			break;
		case RO_E_CLOSED:
			ex = ref new ObjectDisposedException(message);
			break;
		default:
			ex = ref new COMException(hr, message);
			break;
		}

		return ex;
	}

} // namespace Platform
