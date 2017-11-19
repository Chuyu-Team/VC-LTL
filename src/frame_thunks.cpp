
/*

用于修正异常实现

*/



#include <corecrt_internal.h>
#include <eh.h>

EXTERN_C DWORD __cdecl __LTL_GetOsMinVersion();





///////////////////////////////////////////////////////////////////////////////
//
// __uncaught_exception() - Returns true after completing of a throw-expression
//                          untils completing initialization of the
//                          exception-declaration in the matching handler.
//
// __uncaught_exceptions() - Returns the number of exceptions that have been
//                           thrown but not yet caught.
//

extern "C" int __cdecl __uncaught_exceptions()
{
#ifdef _ATL_XP_TARGETING
	if (__LTL_GetOsMinVersion() < 0x00060000)
	{
		//Windows XP不支持多异常处理
		return __uncaught_exception() ? 1 : 0;
	}
	else
#endif
	{
		return __acrt_getptd()->_ProcessingThrow;
	}
}


///////////////////////////////////////////////////////////////////////////////
//
// __current_exception(), __current_exception_context(), __processing_throw()
//                        - Accessors for the per-thread exception state, used
//                          by the managed EH implementation, exception_ptr, etc.
//


extern "C" void** __cdecl __current_exception()
{
	auto ptd = __acrt_getptd();
#ifdef _ATL_XP_TARGETING
	if (__LTL_GetOsMinVersion() < 0x00060000)
	{
		return &((_ptd_msvcrt_xp*)ptd)->_curexception;
	}
	else
#endif
	{
		return &ptd->_curexception;
	}
}

extern "C" void** __cdecl __current_exception_context()
{
	auto ptd = __acrt_getptd();
#ifdef _ATL_XP_TARGETING
	if (__LTL_GetOsMinVersion() < 0x00060000)
	{
		return &((_ptd_msvcrt_xp*)ptd)->_curcontext;
	}
	else
#endif
	{
		return &ptd->_curcontext;
	}
}

#ifdef _ATL_XP_TARGETING
thread_local int _ProcessingThrow;
#endif
extern "C" int* __cdecl __processing_throw()
{
#ifdef _ATL_XP_TARGETING
	if (__LTL_GetOsMinVersion() < 0x00060000)
	{
		//Windows XP不支持多异常处理，只能获取有或者无

		_ProcessingThrow = __uncaught_exception() ? 1 : 0;
		return &_ProcessingThrow;
	}
	else
#endif
	{
		return &__acrt_getptd()->_ProcessingThrow;
	}
}
