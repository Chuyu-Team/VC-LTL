//
// std_exception.cpp
//
//       Copyright (c) Microsoft Corporation. All rights reserved.
//
// Definitions of the std::exception implementation functions.
//
#include <vcruntime_exception.h>
#include <vcruntime_internal.h>



extern "C" void __cdecl __std_exception_copy(
    __std_exception_data const* const from,
    __std_exception_data*       const to
    )
{
    _ASSERTE(to->_What == nullptr && to->_DoFree == false);

    if (!from->_DoFree || !from->_What)
    {
        to->_What   = from->_What;
        to->_DoFree = false;
        return;
    }

    size_t const buffer_count = strlen(from->_What) + 1;

    __crt_unique_heap_ptr<char, __crt_public_free_policy> buffer(static_cast<char*>(malloc(buffer_count)));
    if (!buffer)
    {
        return;
    }

    strcpy_s(buffer.get(), buffer_count, from->_What);
    to->_What   = buffer.detach();
    to->_DoFree = true;
}

extern "C" void __cdecl __std_exception_destroy(
    __std_exception_data* const data
    )
{
    if (data->_DoFree)
    {
        free(const_cast<char*>(data->_What));
    }

    data->_DoFree = false;
    data->_What   = nullptr;
}
