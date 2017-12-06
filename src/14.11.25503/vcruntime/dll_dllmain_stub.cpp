//
// dll_dllmain_stub.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// A DllMain to be used if the client does not define one.
//
#include <vcstartup_internal.h>



extern "C" extern __scrt_dllmain_type const _pRawDllMain;



extern "C" BOOL WINAPI DllMain(
    HINSTANCE const instance,
    DWORD     const reason,
    LPVOID    const reserved
    )
{
    UNREFERENCED_PARAMETER(instance);
    UNREFERENCED_PARAMETER(reason);
    UNREFERENCED_PARAMETER(reserved);

    // When the dynamic CRT libraries are used, the client DLL does not need the
    // thread attach and detach notifications if the client does not provide a
    // DllMain or _pRawDllMain, so we can disable them.  (When the static CRT
    // libraries are used, the CRT's DllMain that is linked into the DLL requires
    // these notifications.)
    // Enclaves do not support this performance optimization API.
    #if defined CRTDLL && !defined _SCRT_ENCLAVE_BUILD
    if (reason == DLL_PROCESS_ATTACH && !_pRawDllMain)
        DisableThreadLibraryCalls(instance);
    #endif

    return TRUE;
}
