/* _Towlower -- convert wchar_t to lower case for Microsoft */
#include <xlocinfo.h>
#include <wchar.h>
#include <awint.h>

_CRTIMP2_PURE wchar_t __CLRCALL_PURE_OR_CDECL _Towlower(wchar_t _Ch, const _Ctypevec *_Ctype)
	{	/* convert element to lower case */
	wchar_t _Res = _Ch;
	if (_Ch != WEOF)
		{
		if (_Ctype->_Hand == 0 && _Ch < 256)
			{	/* handle ASCII character in C locale */
			if (L'A' <= _Ch && _Ch <= L'Z')
				{
				_Res = (wchar_t)(_Ch - L'A' + L'a');
				}
			}
		else if (__crtLCMapStringW(_Ctype->_Hand, LCMAP_LOWERCASE, &_Ch, 1, &_Res, 1) == 0)
			{
			_Res = _Ch;
			}
		}

	return (_Res);
	}

#ifdef MRTDLL
_CRTIMP2_PURE unsigned short __CLRCALL_PURE_OR_CDECL _Towlower(unsigned short _Ch, const _Ctypevec *_Ctype)
	{
	return _Towlower((wchar_t) _Ch, _Ctype);
	}
#endif

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
