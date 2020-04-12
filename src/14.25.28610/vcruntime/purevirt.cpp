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



extern "C" static _purecall_handler __pPurecall = nullptr;



extern "C" int __cdecl _purecall_advanced()
{
    _purecall_handler const purecall_handler = __crt_interlocked_read_pointer(&__pPurecall);
    if (purecall_handler != nullptr)
    {
        purecall_handler();

        // The user-registered purecall handler should not return, but if it does,
        // continue with the default termination behavior.
    }

    abort();
}

extern "C" _purecall_handler __cdecl _set_purecall_handler_advanced(
    _purecall_handler const new_handler
    )
{
    return /*__crt_fast_decode_pointer*/(
        __crt_interlocked_exchange_pointer(
            &__pPurecall,
            __crt_fast_encode_pointer(new_handler)));
}

extern "C" _purecall_handler __cdecl _get_purecall_handler_advanced()
{
    return /*__crt_fast_decode_pointer*/(__crt_interlocked_read_pointer(&__pPurecall));
}
