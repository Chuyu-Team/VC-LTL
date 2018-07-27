
/*

用于修正异常实现

*/



#include <corecrt_internal.h>
#include <eh.h>
#include "ehdata.h"
#include <trnsctrl.h>
#include <vcruntime_exception.h>
#include <vcruntime_typeinfo.h>
#include <msvcrt_IAT.h>





///////////////////////////////////////////////////////////////////////////////
//
// __uncaught_exception() - Returns true after completing of a throw-expression
//                          untils completing initialization of the
//                          exception-declaration in the matching handler.
//
// __uncaught_exceptions() - Returns the number of exceptions that have been
//                           thrown but not yet caught.
//

extern "C" int __cdecl __uncaught_exceptions_downlevel()
{
#ifdef _ATL_XP_TARGETING
	if (__LTL_GetOsMinVersion() < 0x00060000)
	{
		return __acrt_getptd()->XP_msvcrt._ProcessingThrow;
	}
	else
#endif
	{
		return __acrt_getptd()->VistaOrLater_msvcrt._ProcessingThrow;
	}
}

_LCRT_DEFINE_IAT_SYMBOL(__uncaught_exceptions_downlevel);

///////////////////////////////////////////////////////////////////////////////
//
// __current_exception(), __current_exception_context(), __processing_throw()
//                        - Accessors for the per-thread exception state, used
//                          by the managed EH implementation, exception_ptr, etc.
//


extern "C" void** __cdecl __current_exception_downlevel()
{
	auto ptd = __acrt_getptd();
#ifdef _ATL_XP_TARGETING
	if (__LTL_GetOsMinVersion() < 0x00060000)
	{
		return &ptd->XP_msvcrt._curexception;
	}
	else
#endif
	{
		return &ptd->VistaOrLater_msvcrt._curexception;
	}
}

_LCRT_DEFINE_IAT_SYMBOL(__current_exception_downlevel);

extern "C" void** __cdecl __current_exception_context_downlevel()
{
	auto ptd = __acrt_getptd();
#ifdef _ATL_XP_TARGETING
	if (__LTL_GetOsMinVersion() < 0x00060000)
	{
		return &ptd->XP_msvcrt._curcontext;
	}
	else
#endif
	{
		return &ptd->VistaOrLater_msvcrt._curcontext;
	}
}

_LCRT_DEFINE_IAT_SYMBOL(__current_exception_context_downlevel);


extern "C" int* __cdecl __processing_throw_downlevel()
{
#ifdef _ATL_XP_TARGETING
	if (__LTL_GetOsMinVersion() < 0x00060000)
	{
		return &__acrt_getptd()->XP_msvcrt._ProcessingThrow;
	}
	else
#endif
	{
		return &__acrt_getptd()->VistaOrLater_msvcrt._ProcessingThrow;
	}
}

_LCRT_DEFINE_IAT_SYMBOL(__processing_throw_downlevel);

#if _CRT_NTDDI_MIN < NTDDI_WINBLUE

////////////////////////////////////////////////////////////////////////////////
//
// __AdjustPointer - Adjust the pointer to the exception object to a pointer to a
//   base instance.
//
// Output:
//     The address point of the base.
//
// Side-effects:
//     NONE.

extern "C" void *__AdjustPointer_downlevel(
	void *pThis,                        // Address point of exception object
	const PMD& pmd                      // Generalized pointer-to-member
										//   descriptor
) {
	char *pRet = (char *)pThis + pmd.mdisp;

	if (pmd.pdisp >= 0) {
		pRet += *(__int32 *)((char *)*(ptrdiff_t *)((char *)pThis + pmd.pdisp)
#if defined(_WIN64)
			+(unsigned __int64)pmd.vdisp);
#else
			+ pmd.vdisp);
#endif
		pRet += pmd.pdisp;
	}

	return pRet;
}

_LCRT_DEFINE_IAT_SYMBOL(__AdjustPointer_downlevel);

#endif

////////////////////////////////////////////////////////////////////////////////
//
// __GetPlatformExceptionInfo - Get Platform Exception extra information from current exception
//
// Output:
//     The address of WINRTEXCEPTIONINFO structure
//
// Side-effects:
//     NONE.
extern "C" void * __GetPlatformExceptionInfo_downlevel(
	int *pIsBadAlloc
) {
	*pIsBadAlloc = 0;
	EHExceptionRecord *pExcept = _pCurrentException;
	if (pExcept != nullptr)
	{
		ThrowInfo* pTI = PER_PTHROW(pExcept);
		if (pTI != nullptr)
		{
			if (THROW_ISWINRT((*pTI)))
			{
				ULONG_PTR *exceptionInfoPointer = *reinterpret_cast<ULONG_PTR**>(PER_PEXCEPTOBJ(pExcept));
				exceptionInfoPointer--;
				return reinterpret_cast<void*>(*exceptionInfoPointer);
			}
			else
			{
				_EXCEPTION_POINTERS exceptionPointers;
				exceptionPointers.ExceptionRecord = reinterpret_cast<PEXCEPTION_RECORD>(pExcept);
				exceptionPointers.ContextRecord = nullptr;

				*pIsBadAlloc = _is_exception_typeof(typeid(std::bad_alloc), &exceptionPointers);
			}
		}
	}

	return nullptr;
}

_LCRT_DEFINE_IAT_SYMBOL(__GetPlatformExceptionInfo_downlevel);

#define DASSERT(exp)

//////////////////////////////////////////////////////////////////////////////////
// _is_exception_typeof - checks if the thrown exception is the type, the caller
// has passed in.
//
extern "C" int __cdecl _is_exception_typeof_downlevel(const type_info & type, struct _EXCEPTION_POINTERS * ep)
{
    DASSERT(ep != nullptr);

    EHExceptionRecord *pExcept = (EHExceptionRecord *)ep->ExceptionRecord;

    // Is this our Exception?
    DASSERT(pExcept != nullptr);
    DASSERT(PER_IS_MSVC_EH(pExcept));

#if _EH_RELATIVE_TYPEINFO
    __int32 const *ppCatchable;
    ptrdiff_t imgBase = (ptrdiff_t)pExcept->params.pThrowImageBase;
    ppCatchable = THROW_CTLIST_IB(*PER_PTHROW(pExcept), imgBase );
    int catchables = THROW_COUNT_IB(*PER_PTHROW(pExcept), imgBase );
#else
    CatchableType * const *ppCatchable;
    ppCatchable = THROW_CTLIST(*PER_PTHROW(pExcept));
    int catchables = THROW_COUNT(*PER_PTHROW(pExcept));
#endif

    CatchableType *pCatchable;

    // Scan all types that thrown object can be converted to.
    for (; catchables > 0; catchables--, ppCatchable++)
    {
#if _EH_RELATIVE_TYPEINFO
        pCatchable = (CatchableType *)(imgBase + *ppCatchable);
        if(strcmp(type.raw_name(), CT_NAME_IB(*pCatchable, imgBase)) == 0)
#else
        pCatchable = *ppCatchable;
        if(strcmp(type.raw_name(), CT_NAME(*pCatchable)) == 0)
#endif
        {
            // Found a Match.
            return 1;
        }
    }

    return 0;
}

_LCRT_DEFINE_IAT_SYMBOL(_is_exception_typeof_downlevel);
