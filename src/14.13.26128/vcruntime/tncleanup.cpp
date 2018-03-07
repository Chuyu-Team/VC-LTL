//
// tncleanup.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines the per-module type_info state and related cleanup functionality.
//
#include <vcruntime_internal.h>
#include <vcruntime_typeinfo.h>

__type_info_node __type_info_root_node{};



void __cdecl __scrt_initialize_type_info()
{
    InitializeSListHead(&__type_info_root_node._Header);
}

// This function is called when the client DLL is unloaded to free all of the
// undecorated name strings that were allocated in the module, to ensure that
// they are not leaked.
void __cdecl __scrt_uninitialize_type_info()
{
     __std_type_info_destroy_list(&__type_info_root_node);
}
