/***
*ManagedMain.cpp - Initialization for Windows EXE using CRT DLL and managed entry point
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This is the startup routine for applications using the managed entry
*       point int __clrcall main(array<String^>^).
*       It calls the user's main routine after performing C Run-Time Library initialization.
*
*******************************************************************************/

#define _MANAGED_MAIN

#define WPRFLAG 1 /* Flag used by startup code to indicate wide strings */

#ifndef _UNICODE
#define _UNICODE 1
#endif  /* _UNICODE */

#ifndef UNICODE
#define UNICODE 1
#endif  /* UNICODE */

#undef _MBCS /* UNICODE not _MBCS */


#include "mcrtexe.cpp"
