/***
*wcrtexe.c - Initialization for console EXE using CRT DLL, wchar_t version
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This is the actual startup routine for console apps using wide
*       characters.  It calls the user's main routine wmain() after
*       performing C Run-Time Library initialization.
*
*******************************************************************************/

#define WPRFLAG 1

#ifndef _UNICODE
#define _UNICODE 1
#endif  /* _UNICODE */

#ifndef UNICODE
#define UNICODE 1
#endif  /* UNICODE */

#undef _MBCS /* UNICODE not _MBCS */

#include "mcrtexe.cpp"
