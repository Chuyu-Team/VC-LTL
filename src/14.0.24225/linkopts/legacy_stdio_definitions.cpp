//
// legacy_stdio_definitions.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Provides external definitions of the inline stdio printf and scanf functions
// for use by objects compiled with older versions of the CRT headers.
//

// Our references to "unsafe" functions count as "use" so we need to suppress
// the deprecation warnings.  Note that any clients using these symbols will
// also get these deprecation warnings.
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#define _INC_SWPRINTF_INL_
#define _CRT_STDIO_INLINE extern __inline
#include <stdio.h>



// Define the __imp_- or _imp__-prefixed IAT symbols, for compatibility with
// objects that were compiled with /MD or /MDd.
#include <vcstartup_internal.h>

_VCRT_DEFINE_IAT_SYMBOL(__swprintf_l);
_VCRT_DEFINE_IAT_SYMBOL(__vswprintf_l);
_VCRT_DEFINE_IAT_SYMBOL(_fprintf_l);
_VCRT_DEFINE_IAT_SYMBOL(_fprintf_p);
_VCRT_DEFINE_IAT_SYMBOL(_fprintf_p_l);
_VCRT_DEFINE_IAT_SYMBOL(_fprintf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_fscanf_l);
_VCRT_DEFINE_IAT_SYMBOL(_fscanf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_fwprintf_l);
_VCRT_DEFINE_IAT_SYMBOL(_fwprintf_p);
_VCRT_DEFINE_IAT_SYMBOL(_fwprintf_p_l);
_VCRT_DEFINE_IAT_SYMBOL(_fwprintf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_fwscanf_l);
_VCRT_DEFINE_IAT_SYMBOL(_fwscanf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_printf_l);
_VCRT_DEFINE_IAT_SYMBOL(_printf_p);
_VCRT_DEFINE_IAT_SYMBOL(_printf_p_l);
_VCRT_DEFINE_IAT_SYMBOL(_printf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_scanf_l);
_VCRT_DEFINE_IAT_SYMBOL(_scanf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_scprintf);
_VCRT_DEFINE_IAT_SYMBOL(_scprintf_l);
_VCRT_DEFINE_IAT_SYMBOL(_scprintf_p);
_VCRT_DEFINE_IAT_SYMBOL(_scprintf_p_l);
_VCRT_DEFINE_IAT_SYMBOL(_scwprintf);
_VCRT_DEFINE_IAT_SYMBOL(_scwprintf_l);
_VCRT_DEFINE_IAT_SYMBOL(_scwprintf_p);
_VCRT_DEFINE_IAT_SYMBOL(_scwprintf_p_l);
_VCRT_DEFINE_IAT_SYMBOL(_snprintf);
_VCRT_DEFINE_IAT_SYMBOL(_snprintf_c);
_VCRT_DEFINE_IAT_SYMBOL(_snprintf_c_l);
_VCRT_DEFINE_IAT_SYMBOL(_snprintf_l);
_VCRT_DEFINE_IAT_SYMBOL(_snprintf_s);
_VCRT_DEFINE_IAT_SYMBOL(_snprintf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_snscanf);
_VCRT_DEFINE_IAT_SYMBOL(_snscanf_l);
_VCRT_DEFINE_IAT_SYMBOL(_snscanf_s);
_VCRT_DEFINE_IAT_SYMBOL(_snscanf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_snwprintf);
_VCRT_DEFINE_IAT_SYMBOL(_snwprintf_l);
_VCRT_DEFINE_IAT_SYMBOL(_snwprintf_s);
_VCRT_DEFINE_IAT_SYMBOL(_snwprintf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_snwscanf);
_VCRT_DEFINE_IAT_SYMBOL(_snwscanf_l);
_VCRT_DEFINE_IAT_SYMBOL(_snwscanf_s);
_VCRT_DEFINE_IAT_SYMBOL(_snwscanf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_sprintf_l);
_VCRT_DEFINE_IAT_SYMBOL(_sprintf_p);
_VCRT_DEFINE_IAT_SYMBOL(_sprintf_p_l);
_VCRT_DEFINE_IAT_SYMBOL(_sprintf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_sscanf_l);
_VCRT_DEFINE_IAT_SYMBOL(_sscanf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_swprintf);
_VCRT_DEFINE_IAT_SYMBOL(_swprintf_c);
_VCRT_DEFINE_IAT_SYMBOL(_swprintf_c_l);
_VCRT_DEFINE_IAT_SYMBOL(_swprintf_l);
_VCRT_DEFINE_IAT_SYMBOL(_swprintf_p);
_VCRT_DEFINE_IAT_SYMBOL(_swprintf_p_l);
_VCRT_DEFINE_IAT_SYMBOL(_swprintf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_swscanf_l);
_VCRT_DEFINE_IAT_SYMBOL(_swscanf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_vfprintf_l);
_VCRT_DEFINE_IAT_SYMBOL(_vfprintf_p);
_VCRT_DEFINE_IAT_SYMBOL(_vfprintf_p_l);
_VCRT_DEFINE_IAT_SYMBOL(_vfprintf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_vfscanf_l);
_VCRT_DEFINE_IAT_SYMBOL(_vfscanf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_vfwprintf_l);
_VCRT_DEFINE_IAT_SYMBOL(_vfwprintf_p);
_VCRT_DEFINE_IAT_SYMBOL(_vfwprintf_p_l);
_VCRT_DEFINE_IAT_SYMBOL(_vfwprintf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_vfwscanf_l);
_VCRT_DEFINE_IAT_SYMBOL(_vfwscanf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_vprintf_l);
_VCRT_DEFINE_IAT_SYMBOL(_vprintf_p);
_VCRT_DEFINE_IAT_SYMBOL(_vprintf_p_l);
_VCRT_DEFINE_IAT_SYMBOL(_vprintf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_vscanf_l);
_VCRT_DEFINE_IAT_SYMBOL(_vscanf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_vscprintf);
_VCRT_DEFINE_IAT_SYMBOL(_vscprintf_l);
_VCRT_DEFINE_IAT_SYMBOL(_vscprintf_p);
_VCRT_DEFINE_IAT_SYMBOL(_vscprintf_p_l);
_VCRT_DEFINE_IAT_SYMBOL(_vscwprintf);
_VCRT_DEFINE_IAT_SYMBOL(_vscwprintf_l);
_VCRT_DEFINE_IAT_SYMBOL(_vscwprintf_p);
_VCRT_DEFINE_IAT_SYMBOL(_vscwprintf_p_l);
_VCRT_DEFINE_IAT_SYMBOL(_vsnprintf);
_VCRT_DEFINE_IAT_SYMBOL(_vsnprintf_c);
_VCRT_DEFINE_IAT_SYMBOL(_vsnprintf_c_l);
_VCRT_DEFINE_IAT_SYMBOL(_vsnprintf_l);
_VCRT_DEFINE_IAT_SYMBOL(_vsnprintf_s);
_VCRT_DEFINE_IAT_SYMBOL(_vsnprintf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_vsnwprintf);
_VCRT_DEFINE_IAT_SYMBOL(_vsnwprintf_l);
_VCRT_DEFINE_IAT_SYMBOL(_vsnwprintf_s);
_VCRT_DEFINE_IAT_SYMBOL(_vsnwprintf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_vsnwscanf_l);
_VCRT_DEFINE_IAT_SYMBOL(_vsnwscanf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_vsprintf_l);
_VCRT_DEFINE_IAT_SYMBOL(_vsprintf_p);
_VCRT_DEFINE_IAT_SYMBOL(_vsprintf_p_l);
_VCRT_DEFINE_IAT_SYMBOL(_vsprintf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_vsscanf_l);
_VCRT_DEFINE_IAT_SYMBOL(_vsscanf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_vswprintf);
_VCRT_DEFINE_IAT_SYMBOL(_vswprintf_c);
_VCRT_DEFINE_IAT_SYMBOL(_vswprintf_c_l);
_VCRT_DEFINE_IAT_SYMBOL(_vswprintf_l);
_VCRT_DEFINE_IAT_SYMBOL(_vswprintf_p);
_VCRT_DEFINE_IAT_SYMBOL(_vswprintf_p_l);
_VCRT_DEFINE_IAT_SYMBOL(_vswprintf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_vswscanf_l);
_VCRT_DEFINE_IAT_SYMBOL(_vswscanf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_vwprintf_l);
_VCRT_DEFINE_IAT_SYMBOL(_vwprintf_p);
_VCRT_DEFINE_IAT_SYMBOL(_vwprintf_p_l);
_VCRT_DEFINE_IAT_SYMBOL(_vwprintf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_vwscanf_l);
_VCRT_DEFINE_IAT_SYMBOL(_vwscanf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_wprintf_l);
_VCRT_DEFINE_IAT_SYMBOL(_wprintf_p);
_VCRT_DEFINE_IAT_SYMBOL(_wprintf_p_l);
_VCRT_DEFINE_IAT_SYMBOL(_wprintf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_wscanf_l);
_VCRT_DEFINE_IAT_SYMBOL(_wscanf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(fprintf);
_VCRT_DEFINE_IAT_SYMBOL(fprintf_s);
_VCRT_DEFINE_IAT_SYMBOL(fscanf);
_VCRT_DEFINE_IAT_SYMBOL(fscanf_s);
_VCRT_DEFINE_IAT_SYMBOL(fwprintf);
_VCRT_DEFINE_IAT_SYMBOL(fwprintf_s);
_VCRT_DEFINE_IAT_SYMBOL(fwscanf);
_VCRT_DEFINE_IAT_SYMBOL(fwscanf_s);
_VCRT_DEFINE_IAT_SYMBOL(printf);
_VCRT_DEFINE_IAT_SYMBOL(printf_s);
_VCRT_DEFINE_IAT_SYMBOL(scanf);
_VCRT_DEFINE_IAT_SYMBOL(scanf_s);
_VCRT_DEFINE_IAT_SYMBOL(snprintf);
_VCRT_DEFINE_IAT_SYMBOL(sprintf);
_VCRT_DEFINE_IAT_SYMBOL(sprintf_s);
_VCRT_DEFINE_IAT_SYMBOL(sscanf);
_VCRT_DEFINE_IAT_SYMBOL(sscanf_s);
_VCRT_DEFINE_IAT_SYMBOL(swprintf);
_VCRT_DEFINE_IAT_SYMBOL(swprintf_s);
_VCRT_DEFINE_IAT_SYMBOL(swscanf);
_VCRT_DEFINE_IAT_SYMBOL(swscanf_s);
_VCRT_DEFINE_IAT_SYMBOL(vfprintf);
_VCRT_DEFINE_IAT_SYMBOL(vfprintf_s);
_VCRT_DEFINE_IAT_SYMBOL(vfscanf);
_VCRT_DEFINE_IAT_SYMBOL(vfscanf_s);
_VCRT_DEFINE_IAT_SYMBOL(vfwprintf);
_VCRT_DEFINE_IAT_SYMBOL(vfwprintf_s);
_VCRT_DEFINE_IAT_SYMBOL(vfwscanf);
_VCRT_DEFINE_IAT_SYMBOL(vfwscanf_s);
_VCRT_DEFINE_IAT_SYMBOL(vprintf);
_VCRT_DEFINE_IAT_SYMBOL(vprintf_s);
_VCRT_DEFINE_IAT_SYMBOL(vscanf);
_VCRT_DEFINE_IAT_SYMBOL(vscanf_s);
_VCRT_DEFINE_IAT_SYMBOL(vsnprintf);
_VCRT_DEFINE_IAT_SYMBOL(vsnprintf_s);
_VCRT_DEFINE_IAT_SYMBOL(vsprintf);
_VCRT_DEFINE_IAT_SYMBOL(vsprintf_s);
_VCRT_DEFINE_IAT_SYMBOL(vsscanf);
_VCRT_DEFINE_IAT_SYMBOL(vsscanf_s);
_VCRT_DEFINE_IAT_SYMBOL(vswprintf);
_VCRT_DEFINE_IAT_SYMBOL(vswprintf_s);
_VCRT_DEFINE_IAT_SYMBOL(vswscanf);
_VCRT_DEFINE_IAT_SYMBOL(vswscanf_s);
_VCRT_DEFINE_IAT_SYMBOL(vwprintf);
_VCRT_DEFINE_IAT_SYMBOL(vwprintf_s);
_VCRT_DEFINE_IAT_SYMBOL(vwscanf);
_VCRT_DEFINE_IAT_SYMBOL(vwscanf_s);
_VCRT_DEFINE_IAT_SYMBOL(wprintf);
_VCRT_DEFINE_IAT_SYMBOL(wprintf_s);
_VCRT_DEFINE_IAT_SYMBOL(wscanf);
_VCRT_DEFINE_IAT_SYMBOL(wscanf_s);
