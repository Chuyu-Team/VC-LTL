#pragma once

//Vista mode

#if defined _ATL_XP_TARGETING || defined _USING_V110_SDK71_
_LTL_PushWarning(1006, "在Vista模式中启用XP兼容将导致程序体积变大，推荐移除 _ATL_XP_TARGETING 以及 _USING_V110_SDK71_宏。")
#endif

#if defined _M_IX86 || defined _M_AMD64
//NTDDI_VISTA
#define _CRT_NTDDI_MIN 0x06000000
#elif defined _M_ARM
//NTDDI_WIN8
#define _CRT_NTDDI_MIN 0x06020000
#elif defined _M_ARM64
//NTDDI_WIN10_RS3 Windows 10 16299
#define _CRT_NTDDI_MIN 0x0A000004
#else
#error "模式选择错误！Vista模式不支持此体系。"
#endif

#if defined(__cplusplus) && !defined(_Allow_LTL_Mode)
#pragma detect_mismatch("_LTL_Mode", "VistaMode")
#endif


_LTL_PushNote("进入ltl Vista模式，已准备引用到VC-LTL。定义 _DISABLE_DEPRECATE_LTL_MESSAGE 可关闭信息提示。")
