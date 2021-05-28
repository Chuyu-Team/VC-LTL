/***
*pureMSILcode.cpp
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*******************************************************************************/

#undef MRTDLL

#include <vcstartup_internal.h>
#include <vcruntime_internal.h>
#include <minternal.h>

#pragma warning(disable:4439)   // C4439: function with a managed parameter must have a __clrcall calling convention
#pragma warning(disable:4483)   // disable warning/error about __identifier(<string>)

#if defined (_M_CEE_MIXED)
/***
* static void _initterm(_PIFV * pfbegin, _PIFV * pfend) - call entries in
*       function pointer table
*
*Purpose:
*       Walk a table of function pointers, calling each entry, as follows:
*
*           1. walk from beginning to end, pfunctbl is assumed to point
*              to the beginning of the table, which is currently a null entry,
*              as is the end entry.
*           2. skip NULL entries
*           3. stop walking when the end of the table is encountered
*
*Entry:
*       _PVFV *pfbegin  - pointer to the beginning of the table (first
*                         valid entry).
*       _PVFV *pfend    - pointer to the end of the table (after last
*                         valid entry).
*
*Exit:
*       Returns 0 if passed, 1 if falied.
*
*Notes:
*       This routine must be exported in the CRT DLL model so that the client
*       EXE and client DLL(s) can call it to initialize their C++ constructors.
*
*Exceptions:
*       If either pfbegin or pfend is NULL, or invalid, all bets are off!
*
*******************************************************************************/

[System::Diagnostics::DebuggerStepThroughAttribute]
_CRT_SECURITYCRITICAL_ATTRIBUTE
int __clrcall _initterm_e (
        _PIFV * pfbegin,
        _PIFV * pfend
        )
{
        int ret = 0;
        /*
         * walk the table of function pointers from the bottom up, until
         * the end is encountered.  Do not skip the first entry.  The initial
         * value of pfbegin points to the first valid entry.  Do not try to
         * execute what pfend points to.  Only entries before pfend are valid.
         */
        while ( pfbegin < pfend && ret == 0)
        {
            /*
             * if current table entry is non-NULL, call thru it.
             */
            if ( *pfbegin != NULL )
                ret = (**pfbegin)();
            ++pfbegin;
        }

        return ret;
}

/***
* static void _initterm(_PVFV * pfbegin, _PVFV * pfend) - call entries in
*       function pointer table
*
*Purpose:
*       Walk a table of function pointers, calling each entry, as follows:
*
*           1. walk from beginning to end, pfunctbl is assumed to point
*              to the beginning of the table, which is currently a null entry,
*              as is the end entry.
*           2. skip NULL entries
*           3. stop walking when the end of the table is encountered
*
*Entry:
*       _PVFV *pfbegin  - pointer to the beginning of the table (first
*                         valid entry).
*       _PVFV *pfend    - pointer to the end of the table (after last
*                         valid entry).
*
*Exit:
*       No return value
*
*Notes:
*       This routine must be exported in the CRT DLL model so that the client
*       EXE and client DLL(s) can call it to initialize their C++ constructors.
*
*Exceptions:
*       If either pfbegin or pfend is NULL, or invalid, all bets are off!
*
*******************************************************************************/

[System::Diagnostics::DebuggerStepThroughAttribute]
_CRT_SECURITYCRITICAL_ATTRIBUTE
void __clrcall _initterm (
        _PVFV * pfbegin,
        _PVFV * pfend
        )
{
        /*
         * walk the table of function pointers from the bottom up, until
         * the end is encountered.  Do not skip the first entry.  The initial
         * value of pfbegin points to the first valid entry.  Do not try to
         * execute what pfend points to.  Only entries before pfend are valid.
         */
        while ( pfbegin < pfend )
        {
            /*
             * if current table entry is non-NULL, call thru it.
             */
            if ( *pfbegin != NULL )
                (**pfbegin)();
            ++pfbegin;
        }
}
#endif  /* defined (_M_CEE_MIXED) */

namespace __identifier("<CrtImplementationDetails>")
{
        class ThisModule
        {
        private:
                [System::Diagnostics::DebuggerStepThroughAttribute]
                static System::ModuleHandle Handle()
                {
                        return ThisModule::typeid->Module->ModuleHandle;
                }

        public:
                template <class T>
                [System::Diagnostics::DebuggerStepThroughAttribute]
                _CRT_SECURITYCRITICAL_ATTRIBUTE
                static T* ResolveMethod(T* methodToken)
                {
                        return (T*)Handle().ResolveMethodHandle((int)(size_t)methodToken).GetFunctionPointer().ToPointer();
                }
        };
}

/***
* void _initterm_m(_PVFVM * pfbegin, _PVFVM * pfend) - call entries in
*       function pointer table
*
*Purpose:
*       Walk a table of function pointers, calling each entry, as follows:
*
*           1. walk from beginning to end, pfunctbl is assumed to point
*              to the beginning of the table, which is currently a null entry,
*              as is the end entry.
*           2. skip NULL entries
*           3. stop walking when the end of the table is encountered
*
*Entry:
*       _PVFVM *pfbegin  - pointer to the beginning of the table (first
*                         valid entry).
*       _PVFVM *pfend    - pointer to the end of the table (after last
*                         valid entry).
*
*Exit:
*       No return value
*
*Notes:
*
*Exceptions:
*       If either pfbegin or pfend is NULL, or invalid, all bets are off!
*
*******************************************************************************/

[System::Diagnostics::DebuggerStepThroughAttribute]
_CRT_SECURITYCRITICAL_ATTRIBUTE
// needed due to having unverifiable code in compiler-generated initializers
_CRT_ASSERT_UNMANAGED_CODE_ATTRIBUTE
void __clrcall _initterm_m (
        const _PVFVM * pfbegin,
        const _PVFVM * pfend
        )
{
    using namespace __identifier("<CrtImplementationDetails>");

    /*
     * walk the table of function pointers from the bottom up, until
     * the end is encountered.  Do not skip the first entry.  The initial
     * value of pfbegin points to the first valid entry.  Do not try to
     * execute what pfend points to.  Only entries before pfend are valid.
     */
    while ( pfbegin < pfend )
    {
        /*
         * if current table entry is non-NULL, call thru it.
         */

        /*
         * GetRuntimeMethodHandleFromMetadataToken call needs to be changed to
         * call ResolveMethodHandle once that method becomes available in the
         * VBL CLR.  Disabling the warning for now.
         */
        if ( *pfbegin != NULL )
        {
            _PVFVM pfn = ThisModule::ResolveMethod(*pfbegin);
            (pfn)();
        }
        ++pfbegin;
    }
}
