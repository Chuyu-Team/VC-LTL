#pragma once

//XP mode

#define _LTL_XPMode 1

#ifndef _ATL_XP_TARGETING
#define _ATL_XP_TARGETING 1
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

#include "..\_msvcrt.h"
