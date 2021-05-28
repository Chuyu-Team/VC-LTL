/***
*debugger_jmc.c
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Define the helper function for VS debugger Just My Code stepping feature.
*
*******************************************************************************/

#include <Windows.h>
#include <intrin.h>

#pragma warning( disable : 4102 )
#pragma warning( disable : 4189 )

#pragma optimize( "", off )

__declspec(selectany) DWORD __DebuggerCurrentSteppingThreadId;

void __fastcall __CheckForDebuggerJustMyCode(unsigned char *JMC_flag)
{
    unsigned char *__DebuggerLocalJMCFlag = JMC_flag;

    if (*JMC_flag && __DebuggerCurrentSteppingThreadId != 0 && __DebuggerCurrentSteppingThreadId == GetCurrentThreadId()) {
NopLabel:
        __nop();
    }
}
