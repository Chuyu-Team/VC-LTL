//
// purevirt.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The _purecall handler, called by compiler-generated code when a pure virtual
// call occurs.
//
#include <vcruntime_internal.h>
#include <stdlib.h>



extern "C" extern _purecall_handler __pPurecall;



extern "C" int __cdecl _purecall()
{
    _purecall_handler const purecall_handler = _get_purecall_handler();
    if (purecall_handler)
    {
        purecall_handler();

        // The user-registered purecall handler should not return, but if it does,
        // continue with the default termination behavior.
    }

    abort();
}

extern "C" _purecall_handler __cdecl _set_purecall_handler(
    _purecall_handler const new_handler
    )
{
    return __crt_interlocked_exchange_pointer(&__pPurecall, new_handler);
}

extern "C" _purecall_handler __cdecl _get_purecall_handler()
{
    return __crt_interlocked_read_pointer(&__pPurecall);
}
