#pragma once

#ifndef RC_INVOKED

#define _ltlfilelen __FILE__ "(" _CRT_STRINGIZE(__LINE__) ") : "

#ifndef _DISABLE_DEPRECATE_LTL_MESSAGE
#define _LTL_PushMassage(Msg) __pragma(message( _ltlfilelen Msg ))
#else
#define _LTL_PushMassage(Msg)
#endif

#define _LTL_PushNote(Msg) _LTL_PushMassage("note: " Msg )
#define _LTL_PushWarning(Code,Msg) _LTL_PushMassage("warning LTL" _CRT_STRINGIZE(Code) ": " Msg )


#ifdef _DEBUG
#error "调试版无法使用VC LTL，请切换到Release然后重新编译！"
#endif

#ifdef _DLL
_LTL_PushWarning(1004,"从VC-LTL 4.0 开始MD模式将链接到VC-LTL自身的DLL中，如需静态编译（仅依赖msvcrt.dll），请切换到MT。")
#endif

#ifdef _ATL_XP_TARGETING
//Windows XP的msvcrt有BUG，内部32位带符号整形。因此外部最大只允许0x3FFFFFFF
#define _CRT_STDIO_SIZE_MAX 0x3FFFFFFF
#else
#define _CRT_STDIO_SIZE_MAX _CRT_SIZE_MAX
#endif

//VC-LTL内核版本号，用于处理兼容性问题
#define _LTL_Core_Version 4

#include <LTL_config.h>
#include <crtversion.h>
#define __LTL_Check_UCRT
#include <corecrt_share.h>
#undef __LTL_Check_UCRT

#ifndef _VC_LTL_Include
#error "VC头文件（VC\X.XX.XXXXX\include）没有被加载，请确保 VC-LTL helper for Visual Studio.props 属性表正常加入项目，并检查依赖顺序是否有误！"
#endif

#ifndef _UCRT_LTL_Include
#error "ucrt头文件（ucrt\X.XX.XXXXX.X）没有被加载，请确保 VC-LTL helper for Visual Studio.props 属性表正常加入项目，并检查依赖顺序是否有误！"
#endif // !_UCRT_LTL_Include


//关闭常用的pintf系列函数的内联操作，因为老版本没有__stdio_common_vswscanf系列函数
#ifndef _NO_CRT_STDIO_INLINE
#define _NO_CRT_STDIO_INLINE 1
#endif

#ifndef _Build_By_LTL
#define _Build_By_LTL 1
#endif


#ifdef __NO_LTL_LIB
_LTL_PushWarning(1003, "VC-LTL 2.0 开始ltl超越模式已经弃用，此选项将被忽略。")
#endif

#ifdef _NO__LTL_Initialization
_LTL_PushWarning(1003, "VC-LTL 4.0 开始不在允许禁用VC-LTL初始化，此选项将被忽略。")
#endif //!_NO__LTL_Initialization

#if defined __cplusplus_winrt && _CRT_NTDDI_MIN <= /*NTDDI_WIN8*/0x06020000
_LTL_PushWarning(1005, "VC-LTL为了简化支持，当使用 /ZW 参数时编译的程序将无法支持 Windows 8。")
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
