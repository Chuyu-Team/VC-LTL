#pragma once

#if NDEBUG&&_DLL

//关闭常用的pintf系列函数的内联操作，因为老版本没有__stdio_common_vswscanf系列函数
#define _NO_CRT_STDIO_INLINE

#include <corecrt.h>
#pragma comment(linker, "/nodefaultlib:msvcrt.lib")
#pragma comment(linker, "/nodefaultlib:msvcprt.lib")



#ifdef _ATL_XP_TARGETING
//XP模式，此模式很多编译器特性采用事件实现，并将XP不支持的一些函数静态导入
#if _MSC_VER == 1900
#pragma comment(lib,"vc140xp.lib")
#elif _MSC_VER == 1910
#pragma comment(lib,"vc141xp.lib")
#endif
#define _ACRTXPIMP extern
#else
//默认模式，此模式编译器新特性将使用Vista新API实现，性能更佳
#if _MSC_VER == 1900
#pragma comment(lib,"vc140.lib")
#elif _MSC_VER == 1910
#pragma comment(lib,"vc141.lib")
#endif
#define _ACRTXPIMP _ACRTIMP
#endif


//导入Windows Vista 动态库 msvcrt.dll
#pragma comment(lib,"msvcrt_base.lib")

#endif