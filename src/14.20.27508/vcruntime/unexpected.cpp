//
// unexpected.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The unexpected handler
//
#include <eh.h>
#include <vcruntime_internal.h>
#include <corecrt_internal.h>
#include <winapi_thunks.h>
#include <msvcrt_IAT.h>



static unexpected_handler __cdecl get_unexpected_or_default(
    __acrt_ptd const* const ptd
    ) noexcept
{
#if _CRT_NTDDI_MIN < NTDDI_WIN6
    const auto OSVersion = __LTL_GetOsMinVersion();

#if defined(_M_IX86)
    if (OSVersion < 0x00050001)
        return ((_ptd_msvcrt_win2k*)ptd)->_unexpected ? (unexpected_handler)((_ptd_msvcrt_win2k*)ptd)->_unexpected : &terminate;
#endif

    if (OSVersion < 0x00060000)
        return ((_ptd_msvcrt_winxp*)ptd)->_unexpected ? (unexpected_handler)((_ptd_msvcrt_winxp*)ptd)->_unexpected : &terminate;
    else
#endif
        return ((_ptd_msvcrt_win6_shared*)ptd)->_unexpected ? (unexpected_handler)((_ptd_msvcrt_win6_shared*)ptd)->_unexpected : &terminate;
}

extern "C" unexpected_handler __cdecl _get_unexpected() noexcept
{
    return get_unexpected_or_default(__acrt_getptd());
}

#if 0 //由 set_unexpected.asm 转发
extern "C" unexpected_handler __cdecl set_unexpected(
    unexpected_handler const new_handler
    ) noexcept
{
    RENAME_BASE_PTD(__vcrt_ptd)* const ptd = RENAME_BASE_PTD(__vcrt_getptd)();

    unexpected_handler const old_handler = get_unexpected_or_default(ptd);

    ptd->_unexpected = new_handler;

    return old_handler;
}
#endif

#if 0 //由 unexpected.asm 转发
extern "C" void __cdecl unexpected() noexcept(false)
{
    unexpected_handler const handler = RENAME_BASE_PTD(__vcrt_getptd)()->_unexpected;
    if (handler)
    {
        handler();
    }

    terminate();
}
#endif
