//
// delete_scalar.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines the scalar operator delete.
//
#include <crtdbg.h>
#include <malloc.h>
#include <vcruntime_new.h>



void __CRTDECL operator delete(void* const block) noexcept
{
    #ifdef _DEBUG
    _free_dbg(block, _UNKNOWN_BLOCK);
    #else
    free(block);
    #endif
}
