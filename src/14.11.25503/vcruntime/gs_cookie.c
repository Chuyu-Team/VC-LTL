/***
*gs_cookie.c - defines buffer overrun security cookie
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Defines per-module global variable __security_cookie, which is used
*       by the /GS compile switch to detect local buffer variable overrun
*       bugs/attacks.
*
*       When compiling /GS, the compiler injects code to detect when a local
*       array variable has been overwritten, potentially overwriting the
*       return address (on machines like x86 where the return address is on
*       the stack).  A local variable is allocated directly before the return
*       address and initialized on entering the function.  When exiting the
*       function, the compiler inserts code to verify that the local variable
*       has not been modified.  If it has, then an error reporting routine
*       is called.
*
*******************************************************************************/

#include <Windows.h>

/*
 * Default value used for the global /GS security cookie, defined here and
 * in gs_support.c (since standalone SDK build can't use CRT's internal.h).
 */

#ifdef _WIN64
#define DEFAULT_SECURITY_COOKIE ((UINT_PTR)0x00002B992DDFA232)
#else  /* _WIN64 */
#define DEFAULT_SECURITY_COOKIE ((UINT_PTR)0xBB40E64E)
#endif  /* _WIN64 */

/*
 * The global security cookie.  This name is known to the compiler.
 * Initialize to a garbage non-zero value just in case we have a buffer overrun
 * in any code that gets run before __security_init_cookie() has a chance to
 * initialize the cookie to the final value.
 */

UINT_PTR __security_cookie = DEFAULT_SECURITY_COOKIE;

UINT_PTR __security_cookie_complement = ~(DEFAULT_SECURITY_COOKIE);
