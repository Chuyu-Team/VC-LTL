#pragma once

#define _ltlfilelen __FILE__ "(" _CRT_STRINGIZE(__LINE__) ") : "

#define _LTL_PushWarning(Code,Msg) message( _ltlfilelen "warning LTL" _CRT_STRINGIZE(Code) ": " Msg ) 

#ifdef _DEBUG
#error "调试版无法使用VC LTL，请切换到Release然后重新编译！"
#endif

#ifndef _DLL
#error "由于VC LTL必须在MD编译选项才能使用，请将运行调整为MD！"
#endif

#ifndef _DEBUG

#include <crtversion.h>

#ifndef _VC_LTL_Include
#error "VC头文件（VC\X.XX.XXXXX\include）没有被加载，请确保 Shared.props 属性表正常加入项目，并检查依赖顺序是否有误！"
#endif



//关闭常用的pintf系列函数的内联操作，因为老版本没有__stdio_common_vswscanf系列函数
#ifndef _NO_CRT_STDIO_INLINE
#define _NO_CRT_STDIO_INLINE
#endif

#ifndef _Build_By_LTL
#error "预处理器定义继承被破坏，请确保启用 C/C++ - 预处理器 - 【√ 从父级或项目默认设置继承(I)】！"
#endif

#ifndef _DISABLE_DEPRECATE_STATIC_CPPLIB
#define _DISABLE_DEPRECATE_STATIC_CPPLIB
#endif

#ifndef _STATIC_CPPLIB
//静态整合C++库
#define _STATIC_CPPLIB
#endif

//解决某些环境不会添加 XP Mode问题
#ifndef _ATL_XP_TARGETING
#ifdef _USING_V110_SDK71_
#define _ATL_XP_TARGETING
#endif
#endif



#if _VC_CRT_MAJOR_VERSION ==14 && _VC_CRT_MINOR_VERSION==0
//Vistual Studio 2015
#if _VC_CRT_BUILD_VERSION < 24210
#pragma _LTL_PushWarning(1000, "此工具集已经停止维护，强烈建议你请升级到最新Vistual Studio 2015 Update3 KB3165756或者更高版本然后继续！")
#endif
#elif _VC_CRT_MAJOR_VERSION ==14 && _VC_CRT_MINOR_VERSION >= 10 && _VC_CRT_MINOR_VERSION < 20
//Vistual Studio 2017
#if _VC_CRT_MINOR_VERSION < 12
#pragma _LTL_PushWarning(1000, "此工具集已经停止维护，强烈建议你请升级到最新Vistual Studio 2017 15.5或者更高版本然后继续！")
#endif

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

#if defined(__cplusplus) && !defined(_Allow_LTL_Mode)
#pragma detect_mismatch("_LTL_Mode", "XPMode")
#endif

#else //_ATL_XP_TARGETING else
//默认模式，此模式编译器新特性将使用Vista新API实现，性能更佳
#define __ltlversionxp
#define _ACRTXPIMP __declspec(dllimport)
#define _ACRTXPINLINE __declspec(dllimport)
//XP以上系统inline以减少导入数量
#define _ACRTXPIMPINLINE __inline

#if defined(__cplusplus) && !defined(_Allow_LTL_Mode)
#pragma detect_mismatch("_LTL_Mode", "VistaMode")
#endif
#endif //_ATL_XP_TARGETING

#ifndef _UCRT_VERISON
#error "vc-ltl 并不支持当前目标平台，请切换目标平台版本至 10240/14393/15063（推荐）。切换目标平台仅仅是切换了Windows SDK/UCRT版本，这并不影响你兼容老版本Windows（包括Windows XP）。"
#elif _UCRT_VERISON == 10240

#if !defined _ATL_XP_TARGETING && !defined _DISABLE_DEPRECATE_LTL_MESSAGE
#pragma _LTL_PushWarning(1001, "10240 ucrt 存在的目的仅用于兼容Windows XP工具集正常编译，而你的程序并未采用XP兼容，强烈建议你迁徙目标平台到Windows 10 16299。")
#endif

#elif _UCRT_VERISON == 10586
#error "10586 ucrt 已经停止支持，请升级到15063 UCRT或者更高。"
#elif _UCRT_VERISON == 14393
#error "14393 ucrt 已经停止支持，请升级到15063 UCRT或者更高。"
#elif _UCRT_VERISON == 15063
#ifndef _DISABLE_DEPRECATE_LTL_MESSAGE
#pragma _LTL_PushWarning(1002, "15063 ucrt 即将在下个Windows 10 SDK发布时删除，请尽快迁徙目标平台到Windows 10 16299。")
#endif
#elif _UCRT_VERISON == 16299
//当前使用16229 UCRT
#else
#error "无法识别此版本是URCT，强烈建议升级到最新VC-LTL然后继续。"
#endif

#ifndef _DISABLE_DEPRECATE_LTL_MESSAGE
#pragma message(_ltlfilelen "note: 进入ltl普通模式，已准备引用到VC-LTL。定义 _DISABLE_DEPRECATE_LTL_MESSAGE 可关闭信息提示。")
#endif //_DISABLE_DEPRECATE_LTL_MESSAGE


#ifdef __NO_LTL_LIB
#pragma _LTL_PushWarning(1003, "进入ltl超越模式已经弃用，此选项将将被忽略。")
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

#ifndef _DISABLE_DEPRECATE_LTL_MESSAGE
#pragma _LTL_PushWarning(1004, "不引用_LTL_Initialization 可能导致某些补充函数功能异常（比如time、iostream等），尤其是XP兼容模式，请三思而后行。")
#endif //!_DISABLE_DEPRECATE_LTL_MESSAGE

#endif //!_LIB

#endif //!_DEBUG
