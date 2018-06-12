#define __Build_OBJ

#include <msvcrt_IAT.h>
#include <Windows.h>

//切换到系统的msvrct后无需初始化onexit_table，msvcrt.dll内部会初始化
EXTERN_C bool __cdecl __scrt_initialize_onexit_tables(int const module_type)
{
	return true;
}

//_LCRT_DEFINE_IAT_SYMBOL(__scrt_initialize_onexit_tables);


EXTERN_C BOOL __cdecl __vcrt_InitializeCriticalSectionEx(
	LPCRITICAL_SECTION const critical_section,
	DWORD              const spin_count,
	DWORD              const flags
    )
{
#ifdef _ATL_XP_TARGETING
	return InitializeCriticalSectionAndSpinCount(critical_section, spin_count);
#else
	return InitializeCriticalSectionEx(critical_section, spin_count, flags);
#endif
}

_LCRT_DEFINE_IAT_SYMBOL(__vcrt_InitializeCriticalSectionEx);
