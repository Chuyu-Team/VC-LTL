/***
*gs_support.c - initialize the global buffer overrun security cookie
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Define __security_init_cookie, which is called at startup to initialize
*       the global buffer overrun security cookie used by the /GS compile flag.
*
*******************************************************************************/

#include <Windows.h>

#if defined (_M_IX86) && defined (_CRTBLD) && defined (_DEBUG)
/*
 * __security_init_cookie must be called before any exception handlers using
 * the cookie are registered.  We do a spot check for this condition in the
 * debug version of the x86 CRT.
 */

#define CHECK_FOR_LATE_COOKIE_INIT

#define EXCEPTION_CHAIN_END ((struct _EXCEPTION_REGISTRATION_RECORD * POINTER_32)-1)

EXCEPTION_DISPOSITION __cdecl
_except_handler4(
    IN struct _EXCEPTION_RECORD *ExceptionRecord,
    IN PVOID EstablisherFrame,
    IN OUT struct _CONTEXT *ContextRecord,
    IN OUT PVOID DispatcherContext
    );

#endif  /* defined (_M_IX86) && defined (_CRTBLD) && defined (_DEBUG) */

/*
 * Default value used for the global /GS security cookie, defined here and
 * in gs_cookie.c (since standalone SDK build can't use CRT's internal.h).
 */

#ifdef _WIN64
#define DEFAULT_SECURITY_COOKIE 0x00002B992DDFA232
#else  /* _WIN64 */
#define DEFAULT_SECURITY_COOKIE 0xBB40E64E
#endif  /* _WIN64 */

/*
 * The global security cookie.  This name is known to the compiler.
 */
extern UINT_PTR __security_cookie;
extern UINT_PTR __security_cookie_complement;

/*
 * Union to facilitate converting from FILETIME to unsigned __int64
 */
typedef union {
    unsigned __int64 ft_scalar;
    FILETIME ft_struct;
} FT;

/***
*__get_entropy() - retrieve entropy from the underlying OS.
*
*Purpose:
*       Static helper to encapsulate entropy extraction from the OS.
*       Combine a number of sources of randomness.
*
*Entry:
*
*Exit:
*
*Exceptions:
*
*******************************************************************************/

/*
* Enclaves initialize the stack cookie in the loader (similar to all
* modern platforms), but do not expose entropy. No attempt should be made
* to reinitialize the cookie by harvesting new entropy.
*/
#if defined (_SCRT_ENCLAVE_BUILD)

static UINT_PTR __get_entropy(void)
{
    return DEFAULT_SECURITY_COOKIE;
}

#else

static UINT_PTR __get_entropy(void)
{
    UINT_PTR cookie;
    FT systime = { 0 };
    LARGE_INTEGER perfctr;

    GetSystemTimeAsFileTime(&systime.ft_struct);
#if defined (_WIN64)
    cookie = systime.ft_scalar;
#else  /* defined (_WIN64) */
    cookie = systime.ft_struct.dwLowDateTime;
    cookie ^= systime.ft_struct.dwHighDateTime;
#endif  /* defined (_WIN64) */

    cookie ^= GetCurrentThreadId();
    cookie ^= GetCurrentProcessId();

#if _CRT_NTDDI_MIN >= NTDDI_VISTA
#if defined (_WIN64)
    cookie ^= (((UINT_PTR)GetTickCount64()) << 56);
#endif  /* defined (_WIN64) */
    cookie ^= (UINT_PTR)GetTickCount64();
#endif  /* _CRT_NTDDI_MIN >= NTDDI_VISTA  */

    QueryPerformanceCounter(&perfctr);
#if defined (_WIN64)
    cookie ^= (((UINT_PTR)perfctr.LowPart << 32) ^ perfctr.QuadPart);
#else  /* defined (_WIN64) */
    cookie ^= perfctr.LowPart;
    cookie ^= perfctr.HighPart;
#endif  /* defined (_WIN64) */

    /*
    * Increase entropy using ASLR relocation
    */
    cookie ^= (UINT_PTR)&cookie;

#if defined (_WIN64)
    /*
    * On Win64, generate a cookie with the most significant word set to zero,
    * as a defense against buffer overruns involving null-terminated strings.
    * Don't do so on Win32, as it's more important to keep 32 bits of cookie.
    */
    cookie &= 0x0000FFFFffffFFFFi64;
#endif  /* defined (_WIN64) */

    return cookie;
}

#endif

/***
*__security_init_cookie() - init buffer overrun security cookie.
*
*Purpose:
*       Initialize the global buffer overrun security cookie which is used by
*       the /GS compile switch to detect overwrites to local array variables
*       that potentially corrupt the return address.  This routine is called
*       at EXE/DLL startup.
*
*Entry:
*
*Exit:
*
*Exceptions:
*
*******************************************************************************/

void __cdecl __security_init_cookie(void)
{
    UINT_PTR cookie;

    /*
     * Do nothing if the global cookie has already been initialized.  On x86,
     * reinitialize the cookie if it has been previously initialized to a
     * value with the high word 0x0000.  Some versions of Windows will init
     * the cookie in the loader, but using an older mechanism which forced the
     * high word to zero.
     */

    if (__security_cookie != DEFAULT_SECURITY_COOKIE
#if defined (_M_IX86)
        && (__security_cookie & 0xFFFF0000) != 0
#endif  /* defined (_M_IX86) */
       )
    {
        __security_cookie_complement = ~__security_cookie;
        return;
    }


    /*
     * Initialize the global cookie with an unpredictable value which is
     * different for each module in a process.
     */
    cookie = __get_entropy();

    /*
     * Make sure the cookie is initialized to a value that will prevent us from
     * reinitializing it if this routine is ever called twice.
     */

    if (cookie == DEFAULT_SECURITY_COOKIE)
    {
        cookie = DEFAULT_SECURITY_COOKIE + 1;
    }
#if defined (_M_IX86)
    else if ((cookie & 0xFFFF0000) == 0)
    {
        cookie |= ( (cookie|0x4711) << 16);
    }
#endif  /* defined (_M_IX86) */

    __security_cookie = cookie;
    __security_cookie_complement = ~cookie;

}
