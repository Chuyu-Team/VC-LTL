/***
*ehvecctr.cpp - EH-aware version of constructor iterator helper function
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       These functions are called when constructing and destructing arrays of
*       objects.  Their purpose is to assure that constructed elements get
*       destructed if the constructor for one of the elements throws.
*
*       Must be compiled using "-d1Binl" to be able to specify __thiscall
*       at the user level
****/

#include <eh.h>

#if defined _M_CEE
    #define CALLTYPE  __clrcall
    #define CALLEETYPE __clrcall
    #define __RELIABILITY_CONTRACT                                                   \
        [System::Runtime::ConstrainedExecution::ReliabilityContract(                 \
            System::Runtime::ConstrainedExecution::Consistency::WillNotCorruptState, \
            System::Runtime::ConstrainedExecution::Cer::Success                      \
        )]
#else
    #define CALLEETYPE __stdcall
    #define __RELIABILITY_CONTRACT

    #if defined _M_IX86
        #define CALLTYPE __thiscall
    #else
        #define CALLTYPE __stdcall
    #endif
#endif

using constructor_type = void (CALLTYPE*)(void*);
using destructor_type  = void (CALLTYPE*)(void*);

__RELIABILITY_CONTRACT
void CALLEETYPE __ArrayUnwind(
    void*           ptr,            // Pointer to array to destruct
    size_t          size,           // Size of each element (including padding)
    size_t          count,          // Number of elements in the array
    destructor_type destructor      // The destructor to call
    );

__RELIABILITY_CONTRACT
void CALLEETYPE __ehvec_ctor(
    void*            ptr,           // Pointer to array to destruct
    size_t           size,          // Size of each element (including padding)
    size_t           count,         // Number of elements in the array
    constructor_type constructor,   // Constructor to call
    destructor_type  destructor     // Destructor to call should exception be thrown
    )
{
    size_t i{0};
    bool success{false};

    __try
    {
        for (; i != count; ++i)
        {
            constructor(ptr);

            ptr = static_cast<char*>(ptr) + size;
        }

        success = true;
    }
    __finally
    {
        if (!success)
        {
            __ArrayUnwind(ptr, size, i, destructor);
        }
    }
}

__RELIABILITY_CONTRACT
void CALLEETYPE __ehvec_ctor(
    void*            ptr,           // Pointer to array to destruct
    size_t           size,          // Size of each element (including padding)
    int              count,         // Number of elements in the array
    constructor_type constructor,   // Constructor to call
    destructor_type  destructor     // Destructor to call should exception be thrown
    )
{
    __ehvec_ctor(ptr, size, static_cast<size_t>(count), constructor, destructor);
}
