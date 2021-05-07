// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// WinRTWrapper.h
//
// Dynamic wrappers around Windows Runtime functions.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

#define GETPROCCOMBASE(name) \
    GETPROC(L"combase.dll", name)

namespace Concurrency
{
namespace details
{
    class WinRT
    {
    public:

        /// <summary>
        ///     Initializes all static function pointers to Windows Runtime functions.  We do not call or link against these for distribution
        ///     against OS's below Win8.
        /// </summary>
        static void Initialize();

        /// <summary>
        ///     Returns whether or not the Windows Runtime wrappers have been initialized yet.
        /// </summary>
        static bool Initialized();

#if !defined(_ONECORE)
        DECLARE_STATIC_WRAPPER_FN_1(RoInitialize, HRESULT, RO_INIT_TYPE);
        DECLARE_STATIC_WRAPPER_FN(RoUninitialize, void);
#else
        static HRESULT RoInitialize(RO_INIT_TYPE);
        static void RoUninitialize();
#endif // !defined(_ONECORE)

    private:

        static HMODULE m_hModule;
        static volatile LONG s_fInitialized;

    };
} // namespace details
} // namespace Concurrency
