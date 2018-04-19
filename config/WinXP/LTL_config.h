#pragma once

//XP mode

#ifndef _ATL_XP_TARGETING
#define _ATL_XP_TARGETING 1
#endif

//XP模式，此模式很多编译器特性采用事件实现，并将XP不支持的一些函数静态导入
#define __ltlversionxp "xp"
#define _ACRTXPIMP extern
#define _ACRTXPINLINE __inline
//XP以及以下系统外部导入
#define _ACRTXPIMPINLINE extern

#if defined(__cplusplus) && !defined(_Allow_LTL_Mode)
#pragma detect_mismatch("_LTL_Mode", "XPMode")
#endif

_LTL_PushNote("进入ltl WinXP兼容模式，已准备引用到VC-LTL。定义 _DISABLE_DEPRECATE_LTL_MESSAGE 可关闭信息提示。")
