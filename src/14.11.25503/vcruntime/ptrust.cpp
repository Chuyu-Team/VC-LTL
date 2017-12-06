/***
*ptrust.cpp - Generate startup code for use in partial trust scenario
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*
*******************************************************************************/

#define _PARTIAL_TRUST_OBJ

#include "mstartup.cpp"

#if defined (_M_CEE_MIXED)
#ifdef _DEBUG
#pragma comment(linker, "/nodefaultlib:msvcmrtd.lib")
#pragma comment(lib, "msvcrtd.lib")
#else  /* _DEBUG */
#pragma comment(linker, "/nodefaultlib:msvcmrt.lib")
#pragma comment(lib, "msvcrt.lib")
#endif  /* _DEBUG */
#else  /* defined (_M_CEE_MIXED) */
#ifdef _DEBUG
#pragma comment(linker, "/nodefaultlib:msvcurtd.lib")
#pragma comment(lib, "msvcrtd.lib")
#else  /* _DEBUG */
#pragma comment(linker, "/nodefaultlib:msvcurt.lib")
#pragma comment(lib, "msvcrt.lib")
#endif  /* _DEBUG */
#endif  /* defined (_M_CEE_MIXED) */
