#pragma once

//XP mode

#ifndef _ATL_XP_TARGETING
#define _ATL_XP_TARGETING 1
#endif

#ifdef _DLL
//VC-LTL默认没有编译XP的动态库，如果需要请自行编译。
_LTL_PushWarning(1006,"Windows XP模式使用无法正常使用 /MD 编译，建议使用 /MT 编译。")
#endif

#if defined _M_IX86
//NTDDI_WINXP
#define _CRT_NTDDI_MIN 0x05010000
#elif defined _M_AMD64
//NTDDI_WS03
#define _CRT_NTDDI_MIN 0x05020000
#else
#error "模式选择错误！XP模式仅支持x86 以及 AMD64。"
#endif

#if defined(__cplusplus) && !defined(_Allow_LTL_Mode)
#pragma detect_mismatch("_LTL_Mode", "XPMode")
#endif

_LTL_PushNote("进入ltl WinXP兼容模式，已准备引用到VC-LTL。定义 _DISABLE_DEPRECATE_LTL_MESSAGE 可关闭信息提示。")
