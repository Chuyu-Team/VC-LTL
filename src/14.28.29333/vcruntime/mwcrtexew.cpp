/***
*crtexew.c - Initialization for Windows EXE using CRT DLL
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This is the actual startup routine for Windows apps.  It calls the
*       user's main routine WinMain() after performing C Run-Time Library
*       initialization.
*
*******************************************************************************/

#ifdef MRTDLL

#define _WINMAIN_
#include "mwcrtexe.cpp"

#endif  /* MRTDLL */
