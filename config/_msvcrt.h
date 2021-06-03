#pragma once

#if !defined(RC_INVOKED) && !defined(Q_MOC_RUN)

#define _ltlfilelen __FILE__ "(" _CRT_STRINGIZE(__LINE__) ") : "

#ifndef _DISABLE_DEPRECATE_LTL_MESSAGE
#define _LTL_PushMassage(Msg) __pragma(message( _ltlfilelen Msg ))
#else
#define _LTL_PushMassage(Msg)
#endif

#define _LTL_PushNote(Msg) _LTL_PushMassage("note: " Msg )
#define _LTL_PushWarning(Code,Msg) _LTL_PushMassage("warning LTL" _CRT_STRINGIZE(Code) ": " Msg )


#ifdef _ATL_XP_TARGETING
//Windows XP的msvcrt有BUG，内部32位带符号整形。因此外部最大只允许0x3FFFFFFF
#define _CRT_STDIO_SIZE_MAX 0x3FFFFFFF
#else
#define _CRT_STDIO_SIZE_MAX _CRT_SIZE_MAX
#endif

//VC-LTL内核版本号，用于处理兼容性问题
#define _LTL_Core_Version 4

#include <LTL_Mode.h>
#include <crtversion.h>
#define __LTL_Check_UCRT
#include <corecrt_share.h>
#undef __LTL_Check_UCRT

//进一步的安全检查
#include <LTL_CL_Checks.h>


//为了兼容 iso_stdio_wide_specifiers，我们还是关闭内联操作吧。
#if !defined(_NO_CRT_STDIO_INLINE) && !defined(_BUILD_legacy_stdio_definitions)
//关闭常用的pintf系列函数的内联操作，充分利用msvcrt现有函数进一步减少体积。
#define _NO_CRT_STDIO_INLINE 1
#endif


#ifndef _Build_By_LTL
#define _Build_By_LTL 1
#endif

#ifdef  __cplusplus
extern "C" {
#endif
extern unsigned long __cdecl __LTL_GetOsMinVersion();
extern unsigned long long __cdecl __LTL_GetOsVersion();
#ifdef  __cplusplus
} //extern "C"
#endif

#endif //!RC_INVOKED
