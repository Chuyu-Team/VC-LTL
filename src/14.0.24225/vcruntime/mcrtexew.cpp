/***
*wcrtexew.c - Initialization for Windows EXE using CRT DLL, wchar_t version
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This is the actual startup routine for Windows apps using wide
*       characters.  It calls the user's main routine wWinMain() after
*       performing C Run-Time Library initialization.
*
*******************************************************************************/

#ifdef MRTDLL

#define _WINMAIN_
#include "mcrtexe.cpp"

#endif  /* MRTDLL */
