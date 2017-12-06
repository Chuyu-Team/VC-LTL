// throw -- terminate on thrown exception REPLACEABLE
#ifndef _M_CEE_PURE
#define _HAS_EXCEPTIONS 0
#endif /* _M_CEE_PURE */
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <crtdbg.h>

_STD_BEGIN

#ifdef _DEBUG
_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Debug_message(const wchar_t *message, const wchar_t *file, unsigned int line)
	{	// report error and die
        if(::_CrtDbgReportW(_CRT_ASSERT, file, line, NULL, L"%ls", message)==1)
        {
            ::_CrtDbgBreak();
        }
	}
_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Debug_message(const unsigned short *message, const unsigned short *file, unsigned int line)
	{	// report error and die
        _Debug_message((wchar_t *) message, (wchar_t *) file, line);
	}

#endif

_STD_END

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
