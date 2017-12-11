//
// delete_scalar_size.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines the scalar operator delete, size_t overload.
//
#include <vcruntime_internal.h>
#include <vcruntime_new.h>



void __CRTDECL operator delete(void* block, size_t) noexcept
{
    operator delete(block);
}
