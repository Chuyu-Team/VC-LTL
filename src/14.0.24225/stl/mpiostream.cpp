// This file is built into msvcurt.lib
#if !defined(_M_CEE_PURE)
#error This file must be built with /clr:pure.
#endif

#include <fstream>
#include <iostream>

#pragma warning(disable: 4074)
#pragma init_seg(compiler)

_STD_BEGIN

#pragma warning(disable:4439)	// C4439: function with a managed parameter must have a __clrcall calling convention

// OBJECT DECLARATIONS
__PURE_APPDOMAIN_GLOBAL extern istream *_Ptr_cin = 0;
__PURE_APPDOMAIN_GLOBAL extern ostream *_Ptr_cout = 0;
__PURE_APPDOMAIN_GLOBAL extern ostream *_Ptr_cerr = 0;
__PURE_APPDOMAIN_GLOBAL extern ostream *_Ptr_clog = 0;

		// WIDE OBJECTS
__PURE_APPDOMAIN_GLOBAL extern wistream *_Ptr_wcin = 0;
__PURE_APPDOMAIN_GLOBAL extern wostream *_Ptr_wcout = 0;
__PURE_APPDOMAIN_GLOBAL extern wostream *_Ptr_wcerr = 0;
__PURE_APPDOMAIN_GLOBAL extern wostream *_Ptr_wclog = 0;

__PURE_APPDOMAIN_GLOBAL int ios_base::Init::_Init_cnt = -1;
_STD_END
