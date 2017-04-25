#pragma once

#if defined(NDEBUG)&&defined(_DLL)


#ifndef _VCRUNTIME_H
/*
__NO_LTL_LIB 宏，可以取消对ltlXXX.lib的引用，取消引用后程序体积可以变得更小，但是必须手动在cpp引用_msvcrt.cpp，否则将编译失败
ltlXXX.lib是_msvcrt.cpp的静态库

如果希望手动引用_msvcrt.cpp，那么请在顶级.h的最开始 include <_msvcrt.h> 或者 直接在工程定义 __NO_LTL_LIB 宏
*/
#define __NO_LTL_LIB
#endif

//关闭常用的pintf系列函数的内联操作，因为老版本没有__stdio_common_vswscanf系列函数
#define _NO_CRT_STDIO_INLINE


#pragma comment(linker, "/nodefaultlib:msvcrt.lib")
#pragma comment(linker, "/nodefaultlib:msvcprt.lib")




#ifdef _ATL_XP_TARGETING
//XP模式，此模式很多编译器特性采用事件实现，并将XP不支持的一些函数静态导入
#if _MSC_VER == 1900
#pragma comment(lib,"vc140xp.lib")

#ifndef __NO_LTL_LIB
#pragma comment(lib,"ltl140xp.lib")
#endif //__NO_LTL_LIB

#elif _MSC_VER == 1910
#pragma comment(lib,"vc141xp.lib")

#ifndef __NO_LTL_LIB
#pragma comment(lib,"ltl141xp.lib")
#endif //__NO_LTL_LIB

#endif
#define _ACRTXPIMP extern
#else
//默认模式，此模式编译器新特性将使用Vista新API实现，性能更佳
#if _MSC_VER == 1900
#pragma comment(lib,"vc140.lib")

#ifndef __NO_LTL_LIB
#pragma comment(lib,"ltl140.lib")
#endif //__NO_LTL_LIB

#elif _MSC_VER == 1910

#pragma comment(lib,"vc141.lib")

#ifndef __NO_LTL_LIB
#pragma comment(lib,"ltl141.lib")
#endif
#endif //__NO_LTL_LIB

#define _ACRTXPIMP __declspec(dllimport)
#endif


//导入Windows Vista 动态库 msvcrt.dll
#pragma comment(lib,"msvcrt_base.lib")

#endif //NDEBUG&&_DLL