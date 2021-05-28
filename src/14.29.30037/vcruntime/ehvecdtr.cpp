/***
*ehvecdtr.cpp - EH-aware version of destructor iterator helper function
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

#include <ehdata.h>
#include <eh.h>
#include <trnsctrl.h>

#if defined _M_CEE
    #define CALLTYPE  __clrcall
    #define CALLEETYPE __clrcall
    #define __RELIABILITY_CONTRACT                                                   \
        [System::Runtime::ConstrainedExecution::ReliabilityContract(                 \
            System::Runtime::ConstrainedExecution::Consistency::WillNotCorruptState, \
            System::Runtime::ConstrainedExecution::Cer::Success                      \
        )]

    #define ASSERT_UNMANAGED_CODE_ATTRIBUTE [System::Security::Permissions::SecurityPermissionAttribute(System::Security::Permissions::SecurityAction::Assert, UnmanagedCode = true)]
    #define SECURITYCRITICAL_ATTRIBUTE      [System::Security::SecurityCritical]
#else
    #define CALLEETYPE __stdcall
    #define __RELIABILITY_CONTRACT
    #define ASSERT_UNMANAGED_CODE_ATTRIBUTE
    #define SECURITYCRITICAL_ATTRIBUTE

    #if defined _M_IX86
        #define CALLTYPE __thiscall
    #else
        #define CALLTYPE __stdcall
    #endif
#endif

using destructor_type = void (CALLTYPE*)(void*);

__RELIABILITY_CONTRACT
void CALLEETYPE __ArrayUnwind(
    void*           ptr,            // Pointer to array to destruct
    size_t          size,           // Size of each element (including padding)
    size_t          count,          // Number of elements in the array
    destructor_type destructor      // The destructor to call
    );

__RELIABILITY_CONTRACT
SECURITYCRITICAL_ATTRIBUTE
void CALLEETYPE __ehvec_dtor(
    void*           ptr,            // Pointer to array to destruct
    size_t          size,           // Size of each element (including padding)
    size_t          count,          // Number of elements in the array
    destructor_type destructor      // The destructor to call
    )
{
    bool success{false};

    // Advance pointer past end of array
    ptr = static_cast<char*>(ptr) + size * count;

    __try
    {
        // Destruct elements
        while (count-- > 0)
        {
            ptr = static_cast<char*>(ptr) - size;
            destructor(ptr);
        }

        success = true;
    }
    __finally
    {
        if (!success)
        {
            __ArrayUnwind(ptr, size, count, destructor);
        }
    }
}

__RELIABILITY_CONTRACT
ASSERT_UNMANAGED_CODE_ATTRIBUTE
SECURITYCRITICAL_ATTRIBUTE
static int ArrayUnwindFilter(EXCEPTION_POINTERS* pExPtrs)
{
    EHExceptionRecord *pExcept = reinterpret_cast<EHExceptionRecord*>(pExPtrs->ExceptionRecord);

    switch(PER_CODE(pExcept))
    {
        case EH_EXCEPTION_NUMBER:
            _pCurrentException = pExcept;
            _pCurrentExContext = pExPtrs->ContextRecord;
            terminate();
        default:
            return EXCEPTION_CONTINUE_SEARCH;
    }
}

__RELIABILITY_CONTRACT
SECURITYCRITICAL_ATTRIBUTE
void CALLEETYPE __ArrayUnwind(
    void*           ptr,          // Pointer to array to destruct
    size_t          size,         // Size of each element (including padding)
    size_t          count,        // Number of elements in the array
    destructor_type destructor    // The destructor to call
    )
{
    // 'unwind' rest of array

    __try
    {
        for (size_t i{0}; i != count; ++i)
        {
            ptr = static_cast<char*>(ptr) - size;
            destructor(ptr);
        }
    }
    __except(ArrayUnwindFilter(exception_info()))
    {
        ; // Deliberately do nothing
    }
}

__RELIABILITY_CONTRACT
SECURITYCRITICAL_ATTRIBUTE
void CALLEETYPE __ehvec_dtor(
    void*           ptr,            // Pointer to array to destruct
    size_t          size,           // Size of each element (including padding)
    int             count,          // Number of elements in the array
    destructor_type destructor      // The destructor to call
    )
{
    __ehvec_dtor(ptr, size, static_cast<size_t>(count), destructor);
}
