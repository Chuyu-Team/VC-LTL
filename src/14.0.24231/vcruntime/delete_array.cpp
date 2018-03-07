//
// delete_array.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines the array operator delete.
//
#include <vcruntime_internal.h>
#include <vcruntime_new.h>



void __CRTDECL operator delete[](void* block) noexcept
{
    operator delete(block);
}
