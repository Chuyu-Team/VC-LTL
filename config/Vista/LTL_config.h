#pragma once

//Vista mode

#define _LTL_VistaMode 1

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

#include "..\_msvcrt.h"
