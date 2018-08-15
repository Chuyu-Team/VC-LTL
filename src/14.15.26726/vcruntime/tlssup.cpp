/***
*tlssup.cpp - Thread Local Storage run-time support module
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*
****/

#undef CRTDLL
#undef MRTDLL

#include <internal_shared.h>
#include <Windows.h>

extern "C" {

/* Thread Local Storage index for this .EXE or .DLL */

ULONG _tls_index = 0;

/* Special symbols to mark start and end of Thread Local Storage area. */

#pragma data_seg(".tls")

#if defined (_M_X64)
_CRTALLOC(".tls")
#endif  /* defined (_M_X64) */
char _tls_start = 0;

#pragma data_seg(".tls$ZZZ")

#if defined (_M_X64)
_CRTALLOC(".tls$ZZZ")
#endif  /* defined (_M_X64) */
char _tls_end = 0;

#pragma data_seg()

/* Start section for TLS callback array examined by the OS loader code.
 * If dynamic TLS initialization is used, then a pointer to __dyn_tls_init
 * will be placed in .CRT$XLC by inclusion of tlsdyn.obj.  This will cause
 * the .CRT$XD? array of individual TLS variable initialization callbacks
 * to be walked.
 */

_CRTALLOC(".CRT$XLA") PIMAGE_TLS_CALLBACK __xl_a = 0;

/* NULL terminator for TLS callback array.  This symbol, __xl_z, is never
 * actually referenced anywhere, but it must remain.  The OS loader code
 * walks the TLS callback array until it finds a NULL pointer, so this makes
 * sure the array is properly terminated.
 */

_CRTALLOC(".CRT$XLZ") PIMAGE_TLS_CALLBACK __xl_z = 0;

#ifdef _WIN64

_CRTALLOC(".rdata$T")
extern const IMAGE_TLS_DIRECTORY64 _tls_used =
{
        (ULONGLONG) &_tls_start,        // start of tls data
        (ULONGLONG) &_tls_end,          // end of tls data
        (ULONGLONG) &_tls_index,        // address of tls_index
        (ULONGLONG) (&__xl_a+1),        // pointer to call back array
        (ULONG) 0,                      // size of tls zero fill
        (ULONG) 0                       // characteristics
};

#else  /* _WIN64 */

_CRTALLOC(".rdata$T")
extern const IMAGE_TLS_DIRECTORY _tls_used =
{
        (ULONG)(ULONG_PTR) &_tls_start, // start of tls data
        (ULONG)(ULONG_PTR) &_tls_end,   // end of tls data
        (ULONG)(ULONG_PTR) &_tls_index, // address of tls_index
        (ULONG)(ULONG_PTR) (&__xl_a+1), // pointer to call back array
        (ULONG) 0,                      // size of tls zero fill
        (ULONG) 0                       // characteristics
};

#endif  /* _WIN64 */

} // extern "C"
