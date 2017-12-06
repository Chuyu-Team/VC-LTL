//
// legacy_conio_definitions.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Provides external definitions of the inline conio printf and scanf functions
// for use by objects compiled with older versions of the CRT headers.
//

// Our references to "unsafe" functions count as "use" so we need to suppress
// the deprecation warnings.  Note that any clients using these symbols will
// also get these deprecation warnings.
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#define _INC_SWPRINTF_INL_
#define _CRT_STDIO_INLINE extern __inline
#include <conio.h>



// Define the __imp_- or _imp__-prefixed IAT symbols, for compatibility with
// objects that were compiled with /MD or /MDd.
#include <vcstartup_internal.h>

_VCRT_DEFINE_IAT_SYMBOL(_cprintf);
_VCRT_DEFINE_IAT_SYMBOL(_cprintf_l);
_VCRT_DEFINE_IAT_SYMBOL(_cprintf_p);
_VCRT_DEFINE_IAT_SYMBOL(_cprintf_p_l);
_VCRT_DEFINE_IAT_SYMBOL(_cprintf_s);
_VCRT_DEFINE_IAT_SYMBOL(_cprintf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_cscanf);
_VCRT_DEFINE_IAT_SYMBOL(_cscanf_l);
_VCRT_DEFINE_IAT_SYMBOL(_cscanf_s);
_VCRT_DEFINE_IAT_SYMBOL(_cscanf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_cwprintf);
_VCRT_DEFINE_IAT_SYMBOL(_cwprintf_l);
_VCRT_DEFINE_IAT_SYMBOL(_cwprintf_p);
_VCRT_DEFINE_IAT_SYMBOL(_cwprintf_p_l);
_VCRT_DEFINE_IAT_SYMBOL(_cwprintf_s);
_VCRT_DEFINE_IAT_SYMBOL(_cwprintf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_cwscanf);
_VCRT_DEFINE_IAT_SYMBOL(_cwscanf_l);
_VCRT_DEFINE_IAT_SYMBOL(_cwscanf_s);
_VCRT_DEFINE_IAT_SYMBOL(_cwscanf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_vcprintf);
_VCRT_DEFINE_IAT_SYMBOL(_vcprintf_l);
_VCRT_DEFINE_IAT_SYMBOL(_vcprintf_p);
_VCRT_DEFINE_IAT_SYMBOL(_vcprintf_p_l);
_VCRT_DEFINE_IAT_SYMBOL(_vcprintf_s);
_VCRT_DEFINE_IAT_SYMBOL(_vcprintf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_vcscanf);
_VCRT_DEFINE_IAT_SYMBOL(_vcscanf_l);
_VCRT_DEFINE_IAT_SYMBOL(_vcscanf_s);
_VCRT_DEFINE_IAT_SYMBOL(_vcscanf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_vcwprintf);
_VCRT_DEFINE_IAT_SYMBOL(_vcwprintf_l);
_VCRT_DEFINE_IAT_SYMBOL(_vcwprintf_p);
_VCRT_DEFINE_IAT_SYMBOL(_vcwprintf_p_l);
_VCRT_DEFINE_IAT_SYMBOL(_vcwprintf_s);
_VCRT_DEFINE_IAT_SYMBOL(_vcwprintf_s_l);
_VCRT_DEFINE_IAT_SYMBOL(_vcwscanf);
_VCRT_DEFINE_IAT_SYMBOL(_vcwscanf_l);
_VCRT_DEFINE_IAT_SYMBOL(_vcwscanf_s);
_VCRT_DEFINE_IAT_SYMBOL(_vcwscanf_s_l);
