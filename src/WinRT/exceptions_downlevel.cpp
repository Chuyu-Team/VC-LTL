#include "pch.h"
#include <roerrorapi.h>

extern "C" void *__GetPlatformExceptionInfo(int *);


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
}

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