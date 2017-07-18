#pragma once

#define _ltlfilelen __FILE__ "(" _CRT_STRINGIZE(__LINE__) ") : "

#ifdef _DEBUG
#pragma message(_ltlfilelen "warning: 调试版无法使用VC LTL，请切换到Release然后重新编译！")
#endif

#ifndef _DLL
#pragma message(_ltlfilelen "warning: 由于VC LTL必须在MD编译选项才能使用，请将运行调整为MD！")
#endif

#if defined(NDEBUG)&&defined(_DLL)

#include <crtversion.h>

//关闭常用的pintf系列函数的内联操作，因为老版本没有__stdio_common_vswscanf系列函数
#define _NO_CRT_STDIO_INLINE
#define _Build_By_LTL

#pragma comment(linker, "/nodefaultlib:msvcrt.lib")
#pragma comment(linker, "/nodefaultlib:msvcprt.lib")

//解决某些环境不会添加 XP Mode问题
#ifndef _ATL_XP_TARGETING
#ifdef _USING_V110_SDK71_
#define _ATL_XP_TARGETING
#endif
#endif



#if _VC_CRT_MAJOR_VERSION ==14 && _VC_CRT_MINOR_VERSION==0
#define __ltlversion "140"
#elif _VC_CRT_MAJOR_VERSION ==14 && _VC_CRT_MINOR_VERSION== 10
#define __ltlversion "141"
#else
#error "暂不支持此版本的CRT库"
#endif

#ifdef _ATL_XP_TARGETING
//XP模式，此模式很多编译器特性采用事件实现，并将XP不支持的一些函数静态导入
#define __ltlversionxp "xp"
#define _ACRTXPIMP extern
#define _ACRTXPINLINE __inline
//XP以及以下系统外部导入
#define _ACRTXPIMPINLINE extern

#else //_ATL_XP_TARGETING else
//默认模式，此模式编译器新特性将使用Vista新API实现，性能更佳
#define __ltlversionxp
#define _ACRTXPIMP __declspec(dllimport)
#define _ACRTXPINLINE __declspec(dllimport)
//XP以上系统inline以减少导入数量
#define _ACRTXPIMPINLINE __inline

#endif //_ATL_XP_TARGETING

#ifndef _UCRT_VERISON
#error "vc-ltl 并不支持当前目标平台，请切换目标平台版本至 10240/14393/15063（推荐）。切换目标平台仅仅是切换了Windows SDK/UCRT版本，这并不影响你兼容老版本Windows（包括Windows XP）。"
#elif _UCRT_VERISON == 10240

#ifdef _USING_V110_SDK71_
#pragma message(_ltlfilelen "warning: 由于Windows XP工具集强制使用陈旧的10240 UCRT，推荐使用ltlvcrtWinXp.props属性，然后迁徙目标平台到Windows 10 15063，从而使用最新15063 UCRT。此过程并不影响你兼容XP。")
#else
#pragma message(_ltlfilelen "warning: 10240 ucrt 存在的目的仅用于兼容Windows XP工具集正常编译，而你的程序并未采用XP兼容，强烈建议你迁徙目标平台到Windows 10 15063。")
#endif

#elif _UCRT_VERISON == 14393
#pragma message(_ltlfilelen "warning: 14393 ucrt 即将在下个Windows 10 SDK发布时删除，请尽快迁徙目标平台到Windows 10 15063。")
#elif _UCRT_VERISON != 15063
#pragma message(_ltlfilelen "warning: 无法识别此版本是URCT，强烈建议升级到最新VC-LTL然后继续。")
#endif


#pragma comment(lib,"vc" __ltlversion ".lib")
#pragma comment(lib,"ucrt_" _CRT_STRINGIZE(_UCRT_VERISON) ".lib")

#if defined(__NO_LTL_LIB) || defined(__Build_LTL)

#ifndef __Build_LTL
#pragma detect_mismatch("_LTL_Mode", "Overclock")
#endif
#pragma message(_ltlfilelen "info: 进入ltl超越模式，请务必手动引用 _msvcrt.cpp ，否则将编译失败。取消定义 __NO_LTL_LIB 可返回普通模式。")

#else
#pragma detect_mismatch("_LTL_Mode", "Normal")

//__NO_LTL_LIB 宏，可以取消对ltlXXX.lib的引用，取消引用后程序体积可以变得更小，但是必须手动在cpp include _msvcrt.cpp，否则将编译失败
#pragma comment(lib,"ltl" __ltlversion __ltlversionxp ".lib")
#pragma message(_ltlfilelen "info: 进入ltl普通模式，已准备对" "ltl" __ltlversion __ltlversionxp ".lib" "的引用。定义 __NO_LTL_LIB  可切换到超越模式。")

#endif //__NO_LTL_LIB || __Build_LTL

//导入Windows Vista 动态库 msvcrt.dll
#pragma comment(lib,"msvcrt_base.lib")

#endif //NDEBUG&&_DLL