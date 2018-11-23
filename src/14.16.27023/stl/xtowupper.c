/* _Towupper -- convert wchar_t to upper case for Microsoft */
#include <xlocinfo.h>
#include <wchar.h>
#include <awint.h>

_CRTIMP2_PURE wchar_t __CLRCALL_PURE_OR_CDECL _Towupper(wchar_t _Ch, const _Ctypevec *_Ctype)
	{	/* convert element to upper case */
	wchar_t _Res = _Ch;
	if (_Ch != WEOF)
		{
		if (_Ctype->_Hand == 0 && _Ch < 256)
			{	/* handle ASCII character in C locale */
			if (L'a' <= _Ch && _Ch <= L'z')
				{
				_Res = (wchar_t)(_Ch - L'a' + L'A');
				}
			}
		else if (__crtLCMapStringW(_Ctype->_Hand, LCMAP_UPPERCASE, &_Ch, 1, &_Res, 1) == 0)
			{
			_Res = _Ch;
			}
		}

	return (_Res);
	}

#ifdef MRTDLL
_CRTIMP2_PURE unsigned short __CLRCALL_PURE_OR_CDECL _Towupper(unsigned short _Ch, const _Ctypevec *_Ctype)
	{
	return _Towupper((wchar_t) _Ch, _Ctype);
	}
#endif

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
