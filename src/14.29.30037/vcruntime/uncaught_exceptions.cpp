/***
*uncaught_exceptions.cpp - Some target-independent helper routines used by the EH frame handler.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
****/

#include <vcruntime_internal.h>


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
    RENAME_BASE_PTD(__vcrt_ptd)* const ptd = RENAME_BASE_PTD(__vcrt_getptd_noinit)();
    return ptd ? ptd->_ProcessingThrow : 0;
}
