/***
* ==++==
*
* Copyright (c) Microsoft Corporation. All rights reserved.
*
* ==--==
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*
* pplerror.cpp
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/

#include <ppltasks.h>

#if (defined(_CRT_APP) || defined(BUILD_WINDOWS)) && !defined(_KERNELX)

#include <ehdata.h>
#include <exception>
#include <RestrictedErrorInfo.h>
#include <roerrorapi.h>
#include <trnsctrl.h>

extern "C" void *__GetPlatformExceptionInfo(int *);

namespace Concurrency { namespace details {

    _CRTIMP2 void __thiscall _ExceptionHolder::ReportUnhandledError()
    {
        if (_M_stdException)
        {
            try
            {
                std::rethrow_exception(_M_stdException);
            }
            catch (...)
            {
                int isBadAlloc;
                auto info = static_cast<WINRTEXCEPTIONINFO*>(__GetPlatformExceptionInfo(&isBadAlloc));
                // If it is a winrt exception
                if (!isBadAlloc && info != nullptr)
                {
                    auto restrictedInfo(static_cast<IRestrictedErrorInfo*>(info->restrictedInfo));
                    if (restrictedInfo != nullptr)
                    {
#ifndef _M_ARM64 // CRT_REFACTOR TODO
                        ::RoReportUnhandledError(restrictedInfo);
#endif
                        // IRestrictedErrorInfo should be put back
                        // to make sure that RoFailFastWithErrorContext can extract the information
                        ::SetRestrictedErrorInfo(restrictedInfo);
                    }
                    ::RoFailFastWithErrorContext(info->hr);
                }
            }
        }
    }

} }

#else

namespace Concurrency { namespace details {

    _CRTIMP2 void __thiscall _ExceptionHolder::ReportUnhandledError()
    {
    }

} }

#endif

