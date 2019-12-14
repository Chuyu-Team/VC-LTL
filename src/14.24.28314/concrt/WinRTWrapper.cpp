// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// WinRTWrapper.cpp
//
// Dynamic wrappers around Windows Runtime functions.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{

    volatile LONG WinRT::s_fInitialized = FALSE;
    HMODULE WinRT::m_hModule = NULL;

    /// <summary>
    ///     Initializes all static function pointers to Windows Runtime functions.  We do not call or link against these for distribution
    ///     against OS's below Windows 8.
    /// </summary>
    void WinRT::Initialize()
    {
#if !defined(_ONECORE)
        //
        // There is no guarantee that combase.dll is loaded in the process unless it's compiled /ZW.  Make sure that combase.dll sticks
        // around.
        //
        // TODO: It might be nice to FreeLibrary this at some point in the future and not hold it until end of process.  At the very least,
        //       it needs to live until the end of the RM and synchronize with anyone else touching these APIs.
        //
        m_hModule = LoadLibraryExW(L"combase.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (m_hModule == NULL)
        {
            ASSERT(GetLastError() != ERROR_INVALID_PARAMETER);
            throw scheduler_resource_allocation_error(HRESULT_FROM_WIN32(GetLastError()));
        }

        GETPROCCOMBASE(RoInitialize);
        GETPROCCOMBASE(RoUninitialize);
#endif //!defined(_ONECORE)

        InterlockedExchange(&s_fInitialized, TRUE);
    }

    /// <summary>
    ///     Returns whether or not the Windows Runtime wrappers have been initialized yet.
    /// </summary>
    bool WinRT::Initialized()
    {
        return (s_fInitialized != FALSE);
    }

#if !defined(_ONECORE)
    DEFINE_STATIC_WRAPPER_FN_1(WinRT, RoInitialize, HRESULT, RO_INIT_TYPE);
    DEFINE_STATIC_WRAPPER_FN(WinRT, RoUninitialize, void);

#else
    // The threads created under MSDK come from WINRT thread-pool. They are already
    // initialized appropriately. These functions are just noops.
    HRESULT WinRT::RoInitialize(RO_INIT_TYPE)
    {
        return S_OK;
    }

    void WinRT::RoUninitialize()
    {

    }
#endif //!defined(_ONECORE)



} // namespace details
} // namespace Concurrency
