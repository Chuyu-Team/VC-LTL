/***
*uncaught_exceptions.cpp - Some target-independent helper routines used by the EH frame handler.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
****/

#include <vcruntime_internal.h>
#include <corecrt_internal.h>
#include <ptd_downlevel.h>

///////////////////////////////////////////////////////////////////////////////
//
// __uncaught_exceptions() - Returns the number of exceptions that have been
//                           thrown but not yet caught.
//
// If a thread/fiber does not yet have thread/fiber locals, these functions
// just return false/0, without trying to allocate/initialize thread/fiber locals.
//

extern "C" int __cdecl __uncaught_exceptions()
{
	auto* const ptd = __acrt_getptd();
	if (ptd == nullptr)
		return 0;

#if _CRT_NTDDI_MIN < NTDDI_WIN6
	auto OSVersion = __LTL_GetOsMinVersion();

#if defined(_M_IX86)
	if (OSVersion < 0x00050001)
	{
		return __LTL_get_ptd_downlevel()->_ProcessingThrow;
	}
#endif
	if (OSVersion < 0x00060000)
	{
		return ((_ptd_msvcrt_winxp*)ptd)->_ProcessingThrow;
	}
	else
#endif
	{
		return ((_ptd_msvcrt_win6_shared*)ptd)->_ProcessingThrow;
	}
}
