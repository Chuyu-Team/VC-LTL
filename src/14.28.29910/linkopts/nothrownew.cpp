//
// nothrownew.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines C++ new routines that return nullptr on failure instead of throwing.
//
#include <vcstartup_internal.h>
#include <malloc.h>
#include <new.h>

void* operator new(size_t size)
{
    for (;;)
    {
        if (void* const block = malloc(size))
            return block;

        if (_callnewh(size) == 0)
            return nullptr;

        // The new handler was successful; try to allocate again...
    }
}

void* operator new[](size_t size)
{
    return operator new(size);
}

void* operator new(size_t size, int block_use, char const* file_name, int line_number)
{
    UNREFERENCED_PARAMETER(block_use);
    UNREFERENCED_PARAMETER(file_name);
    UNREFERENCED_PARAMETER(line_number);

    return operator new(size);
}

void* operator new[](size_t size, int block_use, char const* file_name, int line_number)
{
    UNREFERENCED_PARAMETER(block_use);
    UNREFERENCED_PARAMETER(file_name);
    UNREFERENCED_PARAMETER(line_number);

    return operator new[](size);
}
