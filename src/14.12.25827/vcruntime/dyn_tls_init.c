//
// dyn_tls_init.c
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// This source file provides a fallback definition of __dyn_tls_init_callback,
// used whenever TLS initialization is not required.
//
// This relies on a feature of the C compiler known as "communal variables."
// This does not work in C++, and the linker's alternatename features is not
// sufficient here.
//
#include <Windows.h>

#pragma warning(disable: 4132) // const object should be initialized
const PIMAGE_TLS_CALLBACK __dyn_tls_init_callback;

PIMAGE_TLS_CALLBACK const* __cdecl __scrt_get_dyn_tls_init_callback()
{
    return &__dyn_tls_init_callback;
}
