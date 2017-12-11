//
// new_array.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines the array operator new.
//
#include <vcruntime_internal.h>
#include <vcruntime_new.h>



void* __CRTDECL operator new[](size_t const size)
{
	return operator new(size);
}
