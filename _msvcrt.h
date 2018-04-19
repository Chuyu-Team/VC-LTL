#pragma once

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

#ifndef _DLL
#error "由于VC LTL必须在MD编译选项才能使用，请将运行调整为MD！"
#endif

#ifndef _DEBUG

#include <LTL_config.h>
#include <crtversion.h>
#define __LTL_Check_UCRT
#include <corecrt_share.h>
#undef __LTL_Check_UCRT

#ifndef _VC_LTL_Include
#error "VC头文件（VC\X.XX.XXXXX\include）没有被加载，请确保 Shared.props 属性表正常加入项目，并检查依赖顺序是否有误！"
#endif

#ifndef _UCRT_LTL_Include
#error "ucrt头文件（ucrt\X.XX.XXXXX.X）没有被加载，请确保 Shared.props 属性表正常加入项目，并检查依赖顺序是否有误！"
#endif // !_UCRT_LTL_Include


//关闭常用的pintf系列函数的内联操作，因为老版本没有__stdio_common_vswscanf系列函数
#ifndef _NO_CRT_STDIO_INLINE
#define _NO_CRT_STDIO_INLINE 1
#endif

#ifndef _Build_By_LTL
#define _Build_By_LTL 1
#endif

#ifndef _DISABLE_DEPRECATE_STATIC_CPPLIB
#define _DISABLE_DEPRECATE_STATIC_CPPLIB 1
#endif

#ifndef _STATIC_CPPLIB
//静态整合C++库
#define _STATIC_CPPLIB 1
#endif




#if _VC_CRT_MAJOR_VERSION ==14 && _VC_CRT_MINOR_VERSION==0
//Vistual Studio 2015
#if _VC_CRT_BUILD_VERSION < 24210
_LTL_PushWarning(1000, "此工具集已经停止维护，强烈建议你请升级到最新Vistual Studio 2015 Update3 KB3165756或者更高版本然后继续！")
#endif
#elif _VC_CRT_MAJOR_VERSION ==14 && _VC_CRT_MINOR_VERSION >= 10 && _VC_CRT_MINOR_VERSION < 20
//Vistual Studio 2017
#if _VC_CRT_MINOR_VERSION < 12
_LTL_PushWarning(1000, "此工具集已经停止维护，强烈建议你请升级到最新Vistual Studio 2017 15.5或者更高版本然后继续！")
#endif

#else
#error "暂不支持此版本的CRT库"
#endif

#ifdef __NO_LTL_LIB
_LTL_PushWarning(1003, "进入ltl超越模式已经弃用，此选项将将被忽略。")
#endif

#if !defined(_NO__LTL_Initialization)
/*
__LTL_Initialization用于初始化 LTL_Initialization.cpp 全局构造

你可以定义 _NO__LTL_Initialization 来移除 LTL 初始化

但是当你使用iostream等功能时将导致程序崩溃

*/
#if defined(_M_IX86)
#pragma comment(linker,"/include:___LTL_Initialization")
#elif defined(_M_AMD64)
#pragma comment(linker,"/include:__LTL_Initialization")
#else
#error "不支持此CPU体系"
#endif

#else //!_NO__LTL_Initialization

_LTL_PushWarning(1004, "不引用_LTL_Initialization 可能导致某些补充函数功能异常（比如time、iostream等），尤其是XP兼容模式，请三思而后行。")

#endif //!_LIB

_CRT_BEGIN_C_HEADER
extern unsigned long __cdecl __LTL_GetOsMinVersion();
extern unsigned long long __cdecl __LTL_GetOsVersion();
_CRT_END_C_HEADER

#endif //!_DEBUG
