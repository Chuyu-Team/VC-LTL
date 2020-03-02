//
// terminate.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The terminate handler
//
#include <corecrt_internal.h>
#include <corecrt_terminate.h>



static terminate_handler __cdecl get_terminate_or_default(
    __acrt_ptd const* const ptd
    ) throw()
{
#if 0
    return ptd->_terminate ? ptd->_terminate : &abort;
#endif

#if _CRT_NTDDI_MIN < 0x06000000
    const auto OsVersion = __LTL_GetOsMinVersion();
    if (OsVersion < 0x00050001)
        return ((_ptd_msvcrt_win2k*)ptd)->_terminate ? ((_ptd_msvcrt_win2k*)ptd)->_terminate : &abort;
	else if(OsVersion < 0x00060000)
		return ((_ptd_msvcrt_winxp*)ptd)->_terminate ? ((_ptd_msvcrt_winxp*)ptd)->_terminate : &abort;
	else
#endif
		return ((_ptd_msvcrt_win6_shared*)ptd)->_terminate ? ((_ptd_msvcrt_win6_shared*)ptd)->_terminate : &abort;
}

extern "C" terminate_handler __cdecl _get_terminate()
{
    return get_terminate_or_default(__acrt_getptd());
}

#if 0
extern "C" terminate_handler __cdecl set_terminate(
    terminate_handler const new_handler
    )
{
    __acrt_ptd* const ptd = __acrt_getptd();

    terminate_handler const old_handler = get_terminate_or_default(ptd);

    ptd->_terminate = new_handler;

    return old_handler;
}

extern "C" void __cdecl terminate()
{
    terminate_handler const handler = __acrt_getptd()->_terminate;
    if (handler)
    {
        // Note:  We cannot allow any exceptions to propagate from a user-
        // registered terminate handler, so if any structured exception escapes
        // the user handler we abort.
        __try
        {
            handler();
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            ; // Deliberately do nothing
        }
    }

    // If the terminate handler returned, faulted, or otherwise failed to end
    // execution, we will do it:
    abort();
}
#endif
