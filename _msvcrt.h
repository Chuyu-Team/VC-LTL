#pragma once

#if defined(NDEBUG)&&defined(_DLL)

#include <crtversion.h>

#ifndef _VCRUNTIME_H
#define __NO_LTL_LIB
#endif

//关闭常用的pintf系列函数的内联操作，因为老版本没有__stdio_common_vswscanf系列函数
#define _NO_CRT_STDIO_INLINE


#pragma comment(linker, "/nodefaultlib:msvcrt.lib")
#pragma comment(linker, "/nodefaultlib:msvcprt.lib")


#if _VC_CRT_MAJOR_VERSION ==14 && _VC_CRT_MINOR_VERSION==0
#define __ltlversion "140"
#elif _VC_CRT_MAJOR_VERSION ==14 && _VC_CRT_MINOR_VERSION== 10
#define __ltlversion "141"
#endif

#ifdef _ATL_XP_TARGETING
//XP模式，此模式很多编译器特性采用事件实现，并将XP不支持的一些函数静态导入
#define __ltlversionxp "xp"
#define _ACRTXPIMP extern

#pragma message("发现当前编译选项需要支持XP/2003，建议【C/C++ - 命令行】输入\"/Zc:threadSafeInit-\" 以禁用线程安全静态初始化，这是编译器本身的BUG，否则在全局变量中使用静态变量会导致程序崩溃。")

#else //_ATL_XP_TARGETING else
//默认模式，此模式编译器新特性将使用Vista新API实现，性能更佳
#define __ltlversionxp
#define _ACRTXPIMP __declspec(dllimport)
#endif //_ATL_XP_TARGETING

#ifdef __ltlversion

#pragma comment(lib,"vc" __ltlversion __ltlversionxp ".lib")

#ifndef __NO_LTL_LIB
/*
__NO_LTL_LIB 宏，可以取消对ltlXXX.lib的引用，取消引用后程序体积可以变得更小，但是必须手动在cpp include _msvcrt.cpp，否则将编译失败
ltlXXX.lib是_msvcrt.cpp的静态库 如果你希望手动引用_msvcrt.cpp，那么请在顶级.h的最开始 include <_msvcrt.h> 或者 直接在工程定义 __NO_LTL_LIB 宏
*/
#pragma comment(lib,"ltl" __ltlversion __ltlversionxp ".lib")

#pragma message("进入ltl普通模式，已准备对" "ltl" __ltlversion __ltlversionxp ".lib" "的引用")
#else
#pragma message("进入ltl超越模式，已取消对" "ltl" __ltlversion __ltlversionxp ".lib" "的引用")
#endif //__NO_LTL_LIB

#endif //__ltlversion

//导入Windows Vista 动态库 msvcrt.dll
#pragma comment(lib,"msvcrt_base.lib")

#endif //NDEBUG&&_DLL