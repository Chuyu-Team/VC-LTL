#include "msvcrt_IAT.h"


#define _LCRT_DEFINE_IAT_SYMBOL_EXTERN(f)                                                          \
    extern "C" void __cdecl f();                                                            \
    _LCRT_DEFINE_IAT_SYMBOL(f)


#ifdef _M_X64

_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_set_FMA3_enable);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_get_FMA3_enable);

#endif

//_LCRT_DEFINE_IAT_SYMBOL_EXTERN(abort_advanced);
//_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_set_abort_behavior_advanced);

_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_fstat64i32_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_stat64i32_light);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_wstat64i32_light);

_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_stat32_advanced);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_stat32i64_advanced);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_stat64i32_advanced);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_stat64_advanced);

_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_wstat32_advanced);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_wstat32i64_advanced);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_wstat64i32_advanced);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_wstat64_advanced);

_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_purecall_advanced);

_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_set_purecall_handler_advanced);

_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_get_purecall_handler_advanced);


_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_set_invalid_parameter_handler_downlevel);

_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_get_invalid_parameter_handler_downlevel);

_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_set_thread_local_invalid_parameter_handler_downlevel);

_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_get_thread_local_invalid_parameter_handler_downlevel);

_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_set_abort_behavior_downlevel);


#ifdef _ATL_XP_TARGETING
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_wassert);

//msvcrt.lib
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(qsort_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_i64toa_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_i64tow_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_itoa_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_itow_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_ltoa_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_ltow_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_makepath_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_mbccpy_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_mbscat_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_mbscpy_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_mbsnbcat_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_mbsnbcpy_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_mbsnbset_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_mbsncat_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_mbsncpy_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_mbsnset_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_mbsset_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_mbstok_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(memcpy_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(memmove_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(scanf_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_snprintf_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_snscanf_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_snwprintf_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_snwscanf_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_splitpath_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(sprintf_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(sscanf_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(strcat_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(strcpy_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(strncat_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(strncpy_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(strnlen_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_strnset_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_strset_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(strtok_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(swprintf_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(swscanf_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_ui64toa_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_ui64tow_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_ultoa_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_ultow_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_vsnprintf_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_vsnwprintf_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(vsprintf_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(vswprintf_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(wcscat_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(wcscpy_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(wcsncat_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(wcsncpy_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(wcsnlen_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_wcsnset_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_wcsset_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(wcstok_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_wmakepath_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(wscanf_s_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_wsplitpath_s_downlevel);

#ifdef _M_IX86
//_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_seh_longjmp_unwind4_downlevel@4);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_local_unwind4_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_except_handler4_common_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_ftol2_sse_excpt_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_ftol2_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_ftol2_sse_downlevel);
#elif defined _M_AMD64
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(RTCastToVoid_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(RTDynamicCast_downlevel);
#endif

#endif



_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_aligned_msize_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_aligned_offset_recalloc_downlevel);
_LCRT_DEFINE_IAT_SYMBOL_EXTERN(_aligned_recalloc_downlevel);
