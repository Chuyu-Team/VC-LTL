
//总是参数 obj 而不导出
#define __Build_OBJ

#include <corecrt_wstdio.h>
#include <stdio.h>
#include <msvcrt_IAT.h>


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Stream I/O Declarations Required by this Header
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#ifdef _ATL_XP_TARGETING
_Check_return_opt_
extern "C" int __CRTDECL _vfwprintf_l_downlevel(
	_Inout_                                 FILE*          const _Stream,
	_In_z_ _Printf_format_string_params_(2) wchar_t const* const _Format,
	_In_opt_                                _locale_t      const _Locale,
	va_list              _ArgList
)
{
	return __stdio_common_vfwprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _Stream, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vfwprintf_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
_Check_return_opt_
extern "C" int __CRTDECL _vfwprintf_s_l_downlevel(
	_Inout_                                 FILE*          const _Stream,
	_In_z_ _Printf_format_string_params_(2) wchar_t const* const _Format,
	_In_opt_                                _locale_t      const _Locale,
	va_list              _ArgList
)
{
	return __stdio_common_vfwprintf_s(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _Stream, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vfwprintf_s_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
_Check_return_opt_
extern "C" int __CRTDECL vfwprintf_s_downlevel(
	_Inout_                       FILE*          const _Stream,
	_In_z_ _Printf_format_string_ wchar_t const* const _Format,
	va_list              _ArgList
)
{
	return _vfwprintf_s_l(_Stream, _Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(vfwprintf_s_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vfwprintf_p_l_downlevel(
	_Inout_                                 FILE*          const _Stream,
	_In_z_ _Printf_format_string_params_(2) wchar_t const* const _Format,
	_In_opt_                                _locale_t      const _Locale,
	va_list              _ArgList
)
{
	return __stdio_common_vfwprintf_p(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _Stream, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vfwprintf_p_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
_Check_return_opt_
extern "C" int __CRTDECL _vfwprintf_p_downlevel(
	_Inout_                       FILE*          const _Stream,
	_In_z_ _Printf_format_string_ wchar_t const* const _Format,
	va_list              _ArgList
)
{
	return _vfwprintf_p_l(_Stream, _Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vfwprintf_p_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
_Check_return_opt_
extern "C" int __CRTDECL _vwprintf_l_downlevel(
	_In_z_ _Printf_format_string_params_(2) wchar_t const* const _Format,
	_In_opt_                                _locale_t      const _Locale,
	va_list              _ArgList
)
{
	return _vfwprintf_l(stdout, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vwprintf_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
_Check_return_opt_
extern "C" int __CRTDECL _vwprintf_s_l_downlevel(
	_In_z_ _Printf_format_string_params_(2) wchar_t const* const _Format,
	_In_opt_                                _locale_t      const _Locale,
	va_list              _ArgList
)
{
	return _vfwprintf_s_l(stdout, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vwprintf_s_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
_Check_return_opt_
extern "C" int __CRTDECL vwprintf_s_downelvel(
	_In_z_ _Printf_format_string_ wchar_t const* const _Format,
	va_list              _ArgList
)
{
	return _vfwprintf_s_l(stdout, _Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(vwprintf_s_downelvel);

#endif


#ifdef _ATL_XP_TARGETING
_Check_return_opt_
extern "C" int __CRTDECL _vwprintf_p_l_downlevel(
	_In_z_ _Printf_format_string_params_(2) wchar_t const* const _Format,
	_In_opt_                                _locale_t      const _Locale,
	va_list              _ArgList
)
{
	return _vfwprintf_p_l(stdout, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vwprintf_p_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
_Check_return_opt_
extern "C" int __CRTDECL _vwprintf_p_downlevel(
	_In_z_ _Printf_format_string_ wchar_t const* const _Format,
	va_list              _ArgList
)
{
	return _vfwprintf_p_l(stdout, _Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vwprintf_p_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
_Check_return_opt_
extern "C" int __CRTDECL _fwprintf_l_downlevel(
	_Inout_                                 FILE*          const _Stream,
	_In_z_ _Printf_format_string_params_(0) wchar_t const* const _Format,
	_In_opt_                                _locale_t      const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vfwprintf_l(_Stream, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_fwprintf_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
_Check_return_opt_
extern "C" int __CRTDECL _fwprintf_s_l_downlevel(
	_Inout_                                 FILE*          const _Stream,
	_In_z_ _Printf_format_string_params_(0) wchar_t const* const _Format,
	_In_opt_                                _locale_t      const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vfwprintf_s_l(_Stream, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_fwprintf_s_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
_Check_return_opt_
extern "C" int __CRTDECL fwprintf_s_downlevel(
	_Inout_                       FILE*          const _Stream,
	_In_z_ _Printf_format_string_ wchar_t const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vfwprintf_s_l(_Stream, _Format, NULL, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(fwprintf_s_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
_Check_return_opt_
extern "C" int __CRTDECL _fwprintf_p_l_downlevel(
	_Inout_                                 FILE*          const _Stream,
	_In_z_ _Printf_format_string_params_(0) wchar_t const* const _Format,
	_In_opt_                                _locale_t      const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vfwprintf_p_l(_Stream, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_fwprintf_p_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
_Check_return_opt_
extern "C" int __CRTDECL _fwprintf_p_downlevel(
	_Inout_                       FILE*          const _Stream,
	_In_z_ _Printf_format_string_ wchar_t const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vfwprintf_p_l(_Stream, _Format, NULL, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_fwprintf_p_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
_Check_return_opt_
extern "C" int __CRTDECL _wprintf_l_downlevel(
	_In_z_ _Printf_format_string_params_(0) wchar_t const* const _Format,
	_In_opt_                                _locale_t      const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vfwprintf_l(stdout, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_wprintf_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
_Check_return_opt_
extern "C" int __CRTDECL _wprintf_s_l_downlevel(
	_In_z_ _Printf_format_string_params_(0) wchar_t const* const _Format,
	_In_opt_                                _locale_t      const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vfwprintf_s_l(stdout, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_wprintf_s_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
_Check_return_opt_
extern "C" int __CRTDECL wprintf_s_downlevel(
	_In_z_ _Printf_format_string_ wchar_t const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vfwprintf_s_l(stdout, _Format, NULL, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(wprintf_s_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
_Check_return_opt_
extern "C" int __CRTDECL _wprintf_p_l_downlevel(
	_In_z_ _Printf_format_string_params_(0) wchar_t const* const _Format,
	_In_opt_                                _locale_t      const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vfwprintf_p_l(stdout, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_wprintf_p_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
_Check_return_opt_
extern "C" int __CRTDECL _wprintf_p_downlevel(
	_In_z_ _Printf_format_string_ wchar_t const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vfwprintf_p_l(stdout, _Format, NULL, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_wprintf_p_downlevel);

#endif



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Wide Character Formatted Input Functions (Stream)
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#ifdef _ATL_XP_TARGETING
_Check_return_opt_
extern "C" int __CRTDECL _vfwscanf_l_downlevel(
	_Inout_ FILE*                                const _Stream,
	_In_z_ _Printf_format_string_ wchar_t const* const _Format,
	_In_opt_                      _locale_t      const _Locale,
	va_list              _ArgList
)
{
	return __stdio_common_vfwscanf(
		_CRT_INTERNAL_LOCAL_SCANF_OPTIONS,
		_Stream, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vfwscanf_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
_Check_return_opt_
extern "C" int __CRTDECL vfwscanf_downlevel(
	_Inout_ FILE*                                const _Stream,
	_In_z_ _Printf_format_string_ wchar_t const* const _Format,
	va_list              _ArgList
)
{
	return _vfwscanf_l(_Stream, _Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(vfwscanf_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
_Check_return_opt_
extern "C" int __CRTDECL _vfwscanf_s_l_downlevel(
	_Inout_                       FILE*          const _Stream,
	_In_z_ _Printf_format_string_ wchar_t const* const _Format,
	_In_opt_                      _locale_t      const _Locale,
	va_list              _ArgList
)
{
	return __stdio_common_vfwscanf(
		_CRT_INTERNAL_LOCAL_SCANF_OPTIONS | _CRT_INTERNAL_SCANF_SECURECRT,
		_Stream, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vfwscanf_s_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
_Check_return_opt_
extern "C" int __CRTDECL vfwscanf_s_downlevel(
	_Inout_                       FILE*          const _Stream,
	_In_z_ _Printf_format_string_ wchar_t const* const _Format,
	va_list              _ArgList
)
{
	return _vfwscanf_s_l(_Stream, _Format, NULL, _ArgList);
}


#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vwscanf_l_downlevel(
	_In_z_ _Printf_format_string_ wchar_t const* const _Format,
	_In_opt_                      _locale_t      const _Locale,
	va_list              _ArgList
)
{
	return _vfwscanf_l(stdin, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vwscanf_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL vwscanf_downlevel(
	_In_z_ _Printf_format_string_ wchar_t const* const _Format,
	va_list              _ArgList
)
{
	return _vfwscanf_l(stdin, _Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(vwscanf_downlevel);

#endif



#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vwscanf_s_l_downlevel(
	_In_z_ _Printf_format_string_ wchar_t const* const _Format,
	_In_opt_                      _locale_t      const _Locale,
	va_list              _ArgList
)
{
	return _vfwscanf_s_l(stdin, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vwscanf_s_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
_Check_return_opt_
extern "C" int __CRTDECL vwscanf_s_downlevel(
	_In_z_ _Printf_format_string_ wchar_t const* const _Format,
	va_list              _ArgList
)
{
	return _vfwscanf_s_l(stdin, _Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(vwscanf_s_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _fwscanf_l_downlevel(
	_Inout_                                FILE*          const _Stream,
	_In_z_ _Scanf_format_string_params_(0) wchar_t const* const _Format,
	_In_opt_                               _locale_t      const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vfwscanf_l(_Stream, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_fwscanf_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _fwscanf_s_l_downlevel(
	_Inout_                                  FILE*          const _Stream,
	_In_z_ _Scanf_s_format_string_params_(0) wchar_t const* const _Format,
	_In_opt_                                 _locale_t      const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vfwscanf_s_l(_Stream, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_fwscanf_s_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL fwscanf_s_downlevel(
	_Inout_                        FILE*          const _Stream,
	_In_z_ _Scanf_s_format_string_ wchar_t const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vfwscanf_s_l(_Stream, _Format, NULL, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(fwscanf_s_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _wscanf_l_downlevel(
	_In_z_ _Scanf_format_string_params_(0) wchar_t const* const _Format,
	_In_opt_                               _locale_t      const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vfwscanf_l(stdin, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_wscanf_l_downlevel);

#endif



#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _wscanf_s_l_downlevel(
	_In_z_ _Scanf_s_format_string_params_(0) wchar_t const* const _Format,
	_In_opt_                                 _locale_t      const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vfwscanf_s_l(stdin, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_wscanf_s_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL wscanf_s_downlevel(
	_In_z_ _Scanf_s_format_string_ wchar_t const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vfwscanf_s_l(stdin, _Format, NULL, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(wscanf_s_downlevel);

#endif


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Wide Character Formatted Output Functions (String)
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vsnwprintf_l_downlevel(
	_Out_writes_opt_(_BufferCount) _Post_maybez_ wchar_t*       const _Buffer,
	_In_                                         size_t         const _BufferCount,
	_In_z_ _Printf_format_string_params_(2)      wchar_t const* const _Format,
	_In_opt_                                     _locale_t      const _Locale,
	va_list              _ArgList
)
{
	int const _Result = __stdio_common_vswprintf(
		_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS | _CRT_INTERNAL_PRINTF_LEGACY_VSPRINTF_NULL_TERMINATION,
		_Buffer, _BufferCount, _Format, _Locale, _ArgList);

	return _Result < 0 ? -1 : _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_vsnwprintf_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vsnwprintf_s_l_downlevel(
	_Out_writes_opt_(_BufferCount) _Always_(_Post_z_) wchar_t*       const _Buffer,
	_In_                                              size_t         const _BufferCount,
	_In_                                              size_t         const _MaxCount,
	_In_z_ _Printf_format_string_params_(2)           wchar_t const* const _Format,
	_In_opt_                                          _locale_t      const _Locale,
	va_list              _ArgList
)
{
	int const _Result = __stdio_common_vsnwprintf_s(
		_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS,
		_Buffer, _BufferCount, _MaxCount, _Format, _Locale, _ArgList);

	return _Result < 0 ? -1 : _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_vsnwprintf_s_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vsnwprintf_s_downlevel(
	_Out_writes_opt_(_BufferCount) _Always_(_Post_z_) wchar_t*       const _Buffer,
	_In_                                              size_t         const _BufferCount,
	_In_                                              size_t         const _MaxCount,
	_In_z_ _Printf_format_string_                     wchar_t const* const _Format,
	va_list              _ArgList
)
{
	return _vsnwprintf_s_l(_Buffer, _BufferCount, _MaxCount, _Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vsnwprintf_s_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vswprintf_c_l_downlevel(
	_Out_writes_opt_(_BufferCount) _Always_(_Post_z_) wchar_t*       const _Buffer,
	_In_                                              size_t         const _BufferCount,
	_In_z_ _Printf_format_string_params_(2)           wchar_t const* const _Format,
	_In_opt_                                          _locale_t      const _Locale,
	va_list              _ArgList
)
{
	int const _Result = __stdio_common_vswprintf(
		_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS,
		_Buffer, _BufferCount, _Format, _Locale, _ArgList);

	return _Result < 0 ? -1 : _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_vswprintf_c_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vswprintf_c_downlevel(
	_Out_writes_opt_(_BufferCount) _Always_(_Post_z_) wchar_t*       const _Buffer,
	_In_                                              size_t         const _BufferCount,
	_In_z_ _Printf_format_string_                     wchar_t const* const _Format,
	va_list              _ArgList
)
{
	//此处存在特性损失，当缓冲区长度刚好为实际使用长度时会null终止，不过这个情况没有变的更坏就这样吧……
	#pragma warning(suppress : 4996)
	return _vsnwprintf(_Buffer, _BufferCount, _Format, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vswprintf_c_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vswprintf_l_downlevel(
	_Out_writes_opt_(_BufferCount) _Always_(_Post_z_) wchar_t*       const _Buffer,
	_In_                                              size_t         const _BufferCount,
	_In_z_ _Printf_format_string_params_(2)           wchar_t const* const _Format,
	_In_opt_                                          _locale_t      const _Locale,
	va_list              _ArgList
)

{
#pragma warning(push)
#pragma warning(disable: 4996) // Deprecation
	return _vswprintf_c_l(_Buffer, _BufferCount, _Format, _Locale, _ArgList);
#pragma warning(pop)
}

_LCRT_DEFINE_IAT_SYMBOL(_vswprintf_l_downlevel);

#endif


extern "C" int __CRTDECL __vswprintf_l_downlevel(
	_Pre_notnull_ _Always_(_Post_z_)        wchar_t*       const _Buffer,
	_In_z_ _Printf_format_string_params_(2) wchar_t const* const _Format,
	_In_opt_                                _locale_t      const _Locale,
	va_list              _ArgList
)
{
	return _vswprintf_l(_Buffer, -1, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(__vswprintf_l_downlevel);



#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vswprintf_downlevel(
	_Pre_notnull_ _Always_(_Post_z_) wchar_t*       const _Buffer,
	_In_z_ _Printf_format_string_    wchar_t const* const _Format,
	va_list              _ArgList
)

{
	#pragma warning(suppress : 4996)
	return _vsnwprintf(_Buffer, _CRT_STDIO_SIZE_MAX, _Format, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vswprintf_downlevel);

#endif


extern "C" int __CRTDECL vswprintf_downlevel(
	_Out_writes_opt_(_BufferCount) _Always_(_Post_z_) wchar_t*       const _Buffer,
	_In_                                              size_t         const _BufferCount, //msvcrt.dll中没有这个参数
	_In_z_ _Printf_format_string_params_(1)           wchar_t const* const _Format,
	va_list              _ArgList
)
{
	return _vswprintf_c(_Buffer, _BufferCount, _Format, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(vswprintf_downlevel);


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vswprintf_s_l_downlevel(
	_Out_writes_(_BufferCount) _Always_(_Post_z_) wchar_t*       const _Buffer,
	_In_                                          size_t         const _BufferCount,
	_In_z_ _Printf_format_string_params_(2)       wchar_t const* const _Format,
	_In_opt_                                      _locale_t      const _Locale,
	va_list              _ArgList
)
{
	int const _Result = __stdio_common_vswprintf_s(
		_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS,
		_Buffer, _BufferCount, _Format, _Locale, _ArgList);

	return _Result < 0 ? -1 : _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_vswprintf_s_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL vswprintf_s_downlevel(
	_Out_writes_(_BufferCount) _Always_(_Post_z_) wchar_t*       const _Buffer,
	_In_                                          size_t         const _BufferCount,
	_In_z_ _Printf_format_string_                 wchar_t const* const _Format,
	va_list              _ArgList
)
{
	return _vswprintf_s_l(_Buffer, _BufferCount, _Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(vswprintf_s_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vswprintf_p_l_downlevel(
	_Out_writes_(_BufferCount) _Always_(_Post_z_) wchar_t*       const _Buffer,
	_In_                                          size_t         const _BufferCount,
	_In_z_ _Printf_format_string_params_(2)       wchar_t const* const _Format,
	_In_opt_                                      _locale_t      const _Locale,
	va_list              _ArgList
	)
{
	int const _Result = __stdio_common_vswprintf_p(
		_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS,
		_Buffer, _BufferCount, _Format, _Locale, _ArgList);

	return _Result < 0 ? -1 : _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_vswprintf_p_l_downlevel);

#endif


extern "C" int __CRTDECL _vswprintf_p_downlevel(
	_Out_writes_(_BufferCount) _Always_(_Post_z_) wchar_t*       const _Buffer,
	_In_                                          size_t         const _BufferCount,
	_In_z_ _Printf_format_string_                 wchar_t const* const _Format,
	va_list              _ArgList
)
{
	return _vswprintf_p_l(_Buffer, _BufferCount, _Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vswprintf_p_downlevel);


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vscwprintf_l_downlevel(
	_In_z_ _Printf_format_string_params_(2) wchar_t const* const _Format,
	_In_opt_                                _locale_t      const _Locale,
	va_list              _ArgList
	)
{
	int const _Result = __stdio_common_vswprintf(
		_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS | _CRT_INTERNAL_PRINTF_STANDARD_SNPRINTF_BEHAVIOR,
		NULL, 0, _Format, _Locale, _ArgList);

	return _Result < 0 ? -1 : _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_vscwprintf_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vscwprintf_p_l_downlevel(
	_In_z_ _Printf_format_string_params_(2) wchar_t const* const _Format,
	_In_opt_                                _locale_t      const _Locale,
	va_list              _ArgList
)
{
	int const _Result = __stdio_common_vswprintf_p(
		_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS | _CRT_INTERNAL_PRINTF_STANDARD_SNPRINTF_BEHAVIOR,
		NULL, 0, _Format, _Locale, _ArgList);

	return _Result < 0 ? -1 : _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_vscwprintf_p_l_downlevel);

#endif

extern "C" int __CRTDECL _vscwprintf_p_downlevel(
	_In_z_ _Printf_format_string_ wchar_t const* const _Format,
	va_list              _ArgList
)
{
	return _vscwprintf_p_l(_Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vscwprintf_p_downlevel);

extern "C" int __CRTDECL __swprintf_l_downlevel(
	_Pre_notnull_ _Always_(_Post_z_)        wchar_t*       const _Buffer,
	_In_z_ _Printf_format_string_params_(2) wchar_t const* const _Format,
	_In_opt_                                _locale_t      const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = __vswprintf_l(_Buffer, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(__swprintf_l_downlevel);


extern "C" int __CRTDECL _swprintf_l_downlevel(
	_Out_writes_opt_(_BufferCount) _Always_(_Post_z_) wchar_t*       const _Buffer,
	_In_                                              size_t         const _BufferCount,
	_In_z_ _Printf_format_string_params_(0)           wchar_t const* const _Format,
	_In_opt_                                          _locale_t      const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vswprintf_c_l(_Buffer, _BufferCount, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_swprintf_l_downlevel);


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _swprintf_downlevel(
	_Pre_notnull_ _Always_(_Post_z_) wchar_t*       const _Buffer,
	_In_z_ _Printf_format_string_    wchar_t const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	#pragma warning(suppress : 4996)
	_Result = _vsnwprintf(_Buffer, _CRT_STDIO_SIZE_MAX, _Format, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_swprintf_downlevel);

#endif

extern "C" int __CRTDECL swprintf_downlevel(
	_Out_writes_opt_(_BufferCount) _Always_(_Post_z_) wchar_t*       const _Buffer,
	_In_                                              size_t         const _BufferCount, //msvcrt.dll 没有此参数
	_In_z_ _Printf_format_string_                     wchar_t const* const _Format,
	...)
{
#ifdef _ATL_XP_TARGETING
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
#pragma warning(suppress : 4996)
	_Result = _vsnwprintf(_Buffer, _BufferCount, _Format, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
#else
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vswprintf_c_l(_Buffer, _BufferCount, _Format, NULL, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
#endif
}

_LCRT_DEFINE_IAT_SYMBOL(swprintf_downlevel);


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _swprintf_s_l_downlevel(
	_Out_writes_(_BufferCount) _Always_(_Post_z_) wchar_t*       const _Buffer,
	_In_                                          size_t         const _BufferCount,
	_In_z_ _Printf_format_string_params_(0)       wchar_t const* const _Format,
	_In_opt_                                      _locale_t      const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vswprintf_s_l(_Buffer, _BufferCount, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_swprintf_s_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL swprintf_s_downlevel(
	_Out_writes_(_BufferCount) _Always_(_Post_z_) wchar_t*       const _Buffer,
	_In_                                          size_t         const _BufferCount,
	_In_z_ _Printf_format_string_                 wchar_t const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vswprintf_s_l(_Buffer, _BufferCount, _Format, NULL, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(swprintf_s_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _swprintf_p_l_downlevel(
	_Out_writes_(_BufferCount) _Always_(_Post_z_) wchar_t*       const _Buffer,
	_In_                                          size_t         const _BufferCount,
	_In_z_ _Printf_format_string_params_(0)       wchar_t const* const _Format,
	_In_opt_                                      _locale_t      const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vswprintf_p_l(_Buffer, _BufferCount, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_swprintf_p_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _swprintf_p_downlevel(
	_Out_writes_(_BufferCount) _Always_(_Post_z_) wchar_t*       const _Buffer,
	_In_                                          size_t         const _BufferCount,
	_In_z_ _Printf_format_string_                 wchar_t const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vswprintf_p_l(_Buffer, _BufferCount, _Format, NULL, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_swprintf_p_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _swprintf_c_l_downlevel(
	_Out_writes_opt_(_BufferCount) _Always_(_Post_z_) wchar_t*       const _Buffer,
	_In_                                              size_t         const _BufferCount,
	_In_z_ _Printf_format_string_params_(0)           wchar_t const* const _Format,
	_In_opt_                                          _locale_t      const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vswprintf_c_l(_Buffer, _BufferCount, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_swprintf_c_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _swprintf_c_downlevel(
	_Out_writes_opt_(_BufferCount) _Always_(_Post_z_) wchar_t*       const _Buffer,
	_In_                                              size_t         const _BufferCount,
	_In_z_ _Printf_format_string_                     wchar_t const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vsnwprintf(_Buffer, _BufferCount, _Format, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_swprintf_c_downlevel);

#endif



#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _snwprintf_l_downlevel(
	_Out_writes_opt_(_BufferCount) _Post_maybez_ wchar_t*       const _Buffer,
	_In_                                         size_t         const _BufferCount,
	_In_z_ _Printf_format_string_params_(0)      wchar_t const* const _Format,
	_In_opt_                                     _locale_t      const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);

#pragma warning(push)
#pragma warning(disable: 4996) // Deprecation
	_Result = _vsnwprintf_l(_Buffer, _BufferCount, _Format, _Locale, _ArgList);
#pragma warning(pop)

	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_snwprintf_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _snwprintf_s_l_downlevel(
	_Out_writes_opt_(_BufferCount) _Always_(_Post_z_) wchar_t*       const _Buffer,
	_In_                                              size_t         const _BufferCount,
	_In_                                              size_t         const _MaxCount,
	_In_z_ _Printf_format_string_params_(0)           wchar_t const* const _Format,
	_In_opt_                                          _locale_t      const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vsnwprintf_s_l(_Buffer, _BufferCount, _MaxCount, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_snwprintf_s_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _snwprintf_s_downlevel(
	_Out_writes_opt_(_BufferCount) _Always_(_Post_z_) wchar_t*       const _Buffer,
	_In_                                              size_t         const _BufferCount,
	_In_                                              size_t         const _MaxCount,
	_In_z_ _Printf_format_string_                     wchar_t const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vsnwprintf_s_l(_Buffer, _BufferCount, _MaxCount, _Format, NULL, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_snwprintf_s_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _scwprintf_l_downlevel(
	_In_z_ _Printf_format_string_params_(0) wchar_t const* const _Format,
	_In_opt_                                _locale_t      const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vscwprintf_l(_Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_scwprintf_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _scwprintf_p_l_downlevel(
	_In_z_ _Printf_format_string_params_(0) wchar_t const* const _Format,
	_In_opt_                                _locale_t      const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vscwprintf_p_l(_Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_scwprintf_p_l_downlevel);

#endif


extern "C" int __CRTDECL _scwprintf_p_downlevel(
	_In_z_ _Printf_format_string_ wchar_t const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vscwprintf_p_l(_Format, NULL, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_scwprintf_p_downlevel);



extern "C" int __CRTDECL _vswscanf_l_downlevel(
	_In_z_                        wchar_t const* const _Buffer,
	_In_z_ _Printf_format_string_ wchar_t const* const _Format,
	_In_opt_                      _locale_t      const _Locale,
	va_list              _ArgList
)
{
	return __stdio_common_vswscanf(
		_CRT_INTERNAL_LOCAL_SCANF_OPTIONS,
		_Buffer, (size_t)-1, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vswscanf_l_downlevel);


extern "C" int __CRTDECL vswscanf_downlevel(
	_In_z_                        wchar_t const* _Buffer,
	_In_z_ _Printf_format_string_ wchar_t const* _Format,
	va_list        _ArgList
)
{
	return _vswscanf_l(_Buffer, _Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(vswscanf_downlevel);

extern "C" int __CRTDECL _vswscanf_s_l_downlevel(
	_In_z_                        wchar_t const* const _Buffer,
	_In_z_ _Printf_format_string_ wchar_t const* const _Format,
	_In_opt_                      _locale_t      const _Locale,
	va_list              _ArgList
)
{
	return __stdio_common_vswscanf(
		_CRT_INTERNAL_LOCAL_SCANF_OPTIONS | _CRT_INTERNAL_SCANF_SECURECRT,
		_Buffer, (size_t)-1, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vswscanf_s_l_downlevel);

extern "C" int __CRTDECL vswscanf_s_downlevel(
	_In_z_                        wchar_t const* const _Buffer,
	_In_z_ _Printf_format_string_ wchar_t const* const _Format,
	va_list              _ArgList
)
{
	return _vswscanf_s_l(_Buffer, _Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(vswscanf_s_downlevel);

extern "C" int __CRTDECL _vsnwscanf_l_downlevel(
	_In_reads_(_BufferCount) _Pre_z_       wchar_t const* const _Buffer,
	_In_                                   size_t         const _BufferCount,
	_In_z_ _Scanf_format_string_params_(2) wchar_t const* const _Format,
	_In_opt_                               _locale_t      const _Locale,
	va_list              _ArgList
)
{
	return __stdio_common_vswscanf(
		_CRT_INTERNAL_LOCAL_SCANF_OPTIONS,
		_Buffer, _BufferCount, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vsnwscanf_l_downlevel);

extern "C" int __CRTDECL _vsnwscanf_s_l_downlevel(
	_In_reads_(_BufferCount) _Pre_z_         wchar_t const* const _Buffer,
	_In_                                     size_t         const _BufferCount,
	_In_z_ _Scanf_s_format_string_params_(2) wchar_t const* const _Format,
	_In_opt_                                 _locale_t      const _Locale,
	va_list              _ArgList
)
{
	return __stdio_common_vswscanf(
		_CRT_INTERNAL_LOCAL_SCANF_OPTIONS | _CRT_INTERNAL_SCANF_SECURECRT,
		_Buffer, _BufferCount, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vsnwscanf_s_l_downlevel);

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _swscanf_l_downlevel(
	_In_z_                                 wchar_t const* const _Buffer,
	_In_z_ _Scanf_format_string_params_(0) wchar_t const* const _Format,
	_In_opt_                               _locale_t            _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vswscanf_l(_Buffer, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_swscanf_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _swscanf_s_l_downlevel(
	_In_z_                                   wchar_t const* const _Buffer,
	_In_z_ _Scanf_s_format_string_params_(0) wchar_t const* const _Format,
	_In_opt_                                 _locale_t      const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vswscanf_s_l(_Buffer, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_swscanf_s_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL swscanf_s_downlevel(
	_In_z_                         wchar_t const* const _Buffer,
	_In_z_ _Scanf_s_format_string_ wchar_t const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vswscanf_s_l(_Buffer, _Format, NULL, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(swscanf_s_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _snwscanf_l_downlevel(
	_In_reads_(_BufferCount) _Pre_z_       wchar_t const* const _Buffer,
	_In_                                   size_t         const _BufferCount,
	_In_z_ _Scanf_format_string_params_(0) wchar_t const* const _Format,
	_In_opt_                               _locale_t      const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);

	#pragma warning(push)
	#pragma warning(disable: 4996) // Deprecation
	_Result = _vsnwscanf_l(_Buffer, _BufferCount, _Format, _Locale, _ArgList);
	#pragma warning(pop)

	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_snwscanf_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _snwscanf_s_l_downlevel(
	_In_reads_(_BufferCount) _Pre_z_         wchar_t const* const _Buffer,
	_In_                                     size_t         const _BufferCount,
	_In_z_ _Scanf_s_format_string_params_(0) wchar_t const* const _Format,
	_In_opt_                                 _locale_t      const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vsnwscanf_s_l(_Buffer, _BufferCount, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_snwscanf_s_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _snwscanf_s_downlevel(
	_In_reads_(_BufferCount) _Pre_z_  wchar_t const* const _Buffer,
	_In_                              size_t         const _BufferCount,
	_In_z_ _Scanf_s_format_string_    wchar_t const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vsnwscanf_s_l(_Buffer, _BufferCount, _Format, NULL, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_snwscanf_s_downlevel);

#endif



//stdio.h

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vfprintf_l_downlevel(
	_Inout_  FILE*       const _Stream,
	_In_z_   char const* const _Format,
	_In_opt_ _locale_t   const _Locale,
	va_list           _ArgList
)
{
	return __stdio_common_vfprintf(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _Stream, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vfprintf_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vfprintf_s_l_downlevel(
	_Inout_  FILE*       const _Stream,
	_In_z_   char const* const _Format,
	_In_opt_ _locale_t   const _Locale,
	va_list           _ArgList
)
{
	return __stdio_common_vfprintf_s(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _Stream, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vfprintf_s_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL vfprintf_s_downlevel(
	_Inout_                       FILE*       const _Stream,
	_In_z_ _Printf_format_string_ char const* const _Format,
	va_list           _ArgList
)
{
	return _vfprintf_s_l(_Stream, _Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(vfprintf_s_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vfprintf_p_l_downlevel(
	_Inout_  FILE*       const _Stream,
	_In_z_   char const* const _Format,
	_In_opt_ _locale_t   const _Locale,
	va_list           _ArgList
)
{
	return __stdio_common_vfprintf_p(_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS, _Stream, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vfprintf_p_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vfprintf_p_downlevel(
	_Inout_                       FILE*       const _Stream,
	_In_z_ _Printf_format_string_ char const* const _Format,
	va_list           _ArgList
)
{
	return _vfprintf_p_l(_Stream, _Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vfprintf_p_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vprintf_l_downlevel(
	_In_z_ _Printf_format_string_params_(2) char const* const _Format,
	_In_opt_                                _locale_t   const _Locale,
	va_list           _ArgList
)
{
	return _vfprintf_l(stdout, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vprintf_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vprintf_s_l_downlevel(
	_In_z_ _Printf_format_string_params_(2) char const* const _Format,
	_In_opt_                                _locale_t   const _Locale,
	va_list           _ArgList
)
{
	return _vfprintf_s_l(stdout, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vprintf_s_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL vprintf_s_downlevel(
	_In_z_ _Printf_format_string_ char const* const _Format,
	va_list           _ArgList
)
{
	return _vfprintf_s_l(stdout, _Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(vprintf_s_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vprintf_p_l_downlevel(
	_In_z_ _Printf_format_string_params_(2) char const* const _Format,
	_In_opt_                                _locale_t   const _Locale,
	va_list           _ArgList
)
{
	return _vfprintf_p_l(stdout, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vprintf_p_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vprintf_p_downlevel(
	_In_z_ _Printf_format_string_ char const* const _Format,
	va_list           _ArgList
)
{
	return _vfprintf_p_l(stdout, _Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vprintf_p_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _fprintf_l_downlevel(
	_Inout_                                 FILE*       const _Stream,
	_In_z_ _Printf_format_string_params_(0) char const* const _Format,
	_In_opt_                                _locale_t   const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vfprintf_l(_Stream, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_fprintf_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _fprintf_s_l_downlevel(
	_Inout_                                 FILE*       const _Stream,
	_In_z_ _Printf_format_string_params_(0) char const* const _Format,
	_In_opt_                                _locale_t   const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vfprintf_s_l(_Stream, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_fprintf_s_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL fprintf_s_downlevel(
	_Inout_                       FILE*       const _Stream,
	_In_z_ _Printf_format_string_ char const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vfprintf_s_l(_Stream, _Format, NULL, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(fprintf_s_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _fprintf_p_l_downlevel(
	_Inout_                                 FILE*       const _Stream,
	_In_z_ _Printf_format_string_params_(0) char const* const _Format,
	_In_opt_                                _locale_t   const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vfprintf_p_l(_Stream, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_fprintf_p_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _fprintf_p_downlevel(
	_Inout_                       FILE*       const _Stream,
	_In_z_ _Printf_format_string_ char const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vfprintf_p_l(_Stream, _Format, NULL, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_fprintf_p_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _printf_l_downlevel(
	_In_z_ _Printf_format_string_params_(0) char const* const _Format,
	_In_opt_                                _locale_t   const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vfprintf_l(stdout, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_printf_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _printf_s_l_downlevel(
	_In_z_ _Printf_format_string_params_(0) char const* const _Format,
	_In_opt_                                _locale_t   const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vfprintf_s_l(stdout, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_printf_s_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL printf_s_downlevel(
	_In_z_ _Printf_format_string_ char const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vfprintf_s_l(stdout, _Format, NULL, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(printf_s_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _printf_p_l_downlevel(
	_In_z_ _Printf_format_string_params_(0) char const* const _Format,
	_In_opt_                                _locale_t   const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vfprintf_p_l(stdout, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_printf_p_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _printf_p_downlevel(
	_In_z_ _Printf_format_string_ char const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vfprintf_p_l(stdout, _Format, NULL, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_printf_p_downlevel);

#endif


extern "C" int __CRTDECL _vfscanf_l_downlevel(
	_Inout_                       FILE*       const _Stream,
	_In_z_ _Printf_format_string_ char const* const _Format,
	_In_opt_                      _locale_t   const _Locale,
	va_list           _ArgList
)
{
	return __stdio_common_vfscanf(
		_CRT_INTERNAL_LOCAL_SCANF_OPTIONS,
		_Stream, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vfscanf_l_downlevel);


extern "C" int __CRTDECL vfscanf_downlevel(
	_Inout_                       FILE*       const _Stream,
	_In_z_ _Printf_format_string_ char const* const _Format,
	va_list           _ArgList
)
{
	return _vfscanf_l(_Stream, _Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(vfscanf_downlevel);

extern "C" int __CRTDECL _vfscanf_s_l_downlevel(
	_Inout_                       FILE*       const _Stream,
	_In_z_ _Printf_format_string_ char const* const _Format,
	_In_opt_                      _locale_t   const _Locale,
	va_list           _ArgList
)
{
	return __stdio_common_vfscanf(
		_CRT_INTERNAL_LOCAL_SCANF_OPTIONS | _CRT_INTERNAL_SCANF_SECURECRT,
		_Stream, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vfscanf_s_l_downlevel);


extern "C" int __CRTDECL vfscanf_s_downlevel(
	_Inout_                       FILE*       const _Stream,
	_In_z_ _Printf_format_string_ char const* const _Format,
	va_list           _ArgList
)
{
	return _vfscanf_s_l(_Stream, _Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(vfscanf_s_downlevel);


extern "C" int __CRTDECL _vscanf_l_downlevel(
	_In_z_ _Printf_format_string_ char const* const _Format,
	_In_opt_                      _locale_t   const _Locale,
	va_list           _ArgList
)
{
	return _vfscanf_l(stdin, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vscanf_l_downlevel);

extern "C" int __CRTDECL vscanf_downlevel(
	_In_z_ _Printf_format_string_ char const* const _Format,
	va_list           _ArgList
)
{
	return _vfscanf_l(stdin, _Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(vscanf_downlevel);


extern "C" int __CRTDECL _vscanf_s_l_downlevel(
	_In_z_ _Printf_format_string_ char const* const _Format,
	_In_opt_                      _locale_t   const _Locale,
	va_list           _ArgList
)
{
	return _vfscanf_s_l(stdin, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vscanf_s_l_downlevel);

extern "C" int __CRTDECL vscanf_s_downlevel(
	_In_z_ _Printf_format_string_ char const* const _Format,
	va_list           _ArgList
)
{
	return _vfscanf_s_l(stdin, _Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(vscanf_s_downlevel);

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _fscanf_l_downlevel(
	_Inout_                                FILE*       const _Stream,
	_In_z_ _Scanf_format_string_params_(0) char const* const _Format,
	_In_opt_                               _locale_t   const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vfscanf_l(_Stream, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_fscanf_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _fscanf_s_l_downlevel(
	_Inout_                                  FILE*       const _Stream,
	_In_z_ _Scanf_s_format_string_params_(0) char const* const _Format,
	_In_opt_                                 _locale_t   const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vfscanf_s_l(_Stream, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_fscanf_s_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL fscanf_s_downlevel(
	_Inout_                        FILE*       const _Stream,
	_In_z_ _Scanf_s_format_string_ char const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vfscanf_s_l(_Stream, _Format, NULL, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(fscanf_s_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _scanf_l_downlevel(
	_In_z_ _Scanf_format_string_params_(0) char const* const _Format,
	_In_opt_                               _locale_t   const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vfscanf_l(stdin, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_scanf_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _scanf_s_l_downlevel(
	_In_z_ _Scanf_s_format_string_params_(0) char const* const _Format,
	_In_opt_                                 _locale_t   const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vfscanf_s_l(stdin, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_scanf_s_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL scanf_s_downlevel(
	_In_z_ _Scanf_s_format_string_ char const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vfscanf_s_l(stdin, _Format, NULL, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(scanf_s_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vsnprintf_l_downlevel(
	_Out_writes_opt_(_BufferCount) _Post_maybez_ char*       const _Buffer,
	_In_                                         size_t      const _BufferCount,
	_In_z_ _Printf_format_string_params_(2)      char const* const _Format,
	_In_opt_                                     _locale_t   const _Locale,
	va_list           _ArgList
)
{
	int const _Result = __stdio_common_vsprintf(
		_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS | _CRT_INTERNAL_PRINTF_LEGACY_VSPRINTF_NULL_TERMINATION,
		_Buffer, _BufferCount, _Format, _Locale, _ArgList);

	return _Result < 0 ? -1 : _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_vsnprintf_l_downlevel);

#endif


//msvcrt.dll 没有_CRT_INTERNAL_PRINTF_STANDARD_SNPRINTF_BEHAVIOR特性
extern "C" int __CRTDECL vsnprintf_downlevel(
	_Out_writes_opt_(_BufferCount) _Always_(_Post_z_) char*       const _Buffer,
	_In_                                              size_t      const _BufferCount,
	_In_z_ _Printf_format_string_                     char const* const _Format,
	va_list           _ArgList
)
{
	auto Count = _vsnprintf(_Buffer, _BufferCount, _Format, _ArgList);

	//模拟_CRT_INTERNAL_PRINTF_STANDARD_SNPRINTF_BEHAVIOR特性
	if (_Buffer != nullptr && _BufferCount != 0)
	{
		if (Count < 0)
		{
			*_Buffer = '\0';
		}
		else if ((size_t)Count == _BufferCount)
		{
			_Buffer[Count - 1] = '\0';
		}
		else
		{
			_Buffer[Count] = '\0';
		}
	}

	return Count;
}

_LCRT_DEFINE_IAT_SYMBOL(vsnprintf_downlevel);


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vsprintf_l_downlevel(
	_Pre_notnull_ _Always_(_Post_z_) char*       const _Buffer,
	_In_z_                           char const* const _Format,
	_In_opt_                         _locale_t   const _Locale,
	va_list           _ArgList
)
{
	#pragma warning(push)
	#pragma warning(disable: 4996) // Deprecation
	return _vsnprintf_l(_Buffer, (size_t)-1, _Format, _Locale, _ArgList);
	#pragma warning(pop)
}

_LCRT_DEFINE_IAT_SYMBOL(_vsprintf_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vsprintf_s_l_downlevel(
	_Out_writes_(_BufferCount) _Always_(_Post_z_) char*       const _Buffer,
	_In_                                          size_t      const _BufferCount,
	_In_z_ _Printf_format_string_params_(2)       char const* const _Format,
	_In_opt_                                      _locale_t   const _Locale,
	va_list           _ArgList
)
{
	int const _Result = __stdio_common_vsprintf_s(
		_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS,
		_Buffer, _BufferCount, _Format, _Locale, _ArgList);

	return _Result < 0 ? -1 : _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_vsprintf_s_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL vsprintf_s_downlevel(
	_Out_writes_(_BufferCount) _Always_(_Post_z_) char*       const _Buffer,
	_In_                                          size_t      const _BufferCount,
	_In_z_ _Printf_format_string_                 char const* const _Format,
	va_list           _ArgList
)
{
	return _vsprintf_s_l(_Buffer, _BufferCount, _Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(vsprintf_s_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vsprintf_p_l_downlevel(
	_Out_writes_(_BufferCount) _Always_(_Post_z_) char*       const _Buffer,
	_In_                                          size_t      const _BufferCount,
	_In_z_ _Printf_format_string_params_(2)       char const* const _Format,
	_In_opt_                                      _locale_t   const _Locale,
	va_list           _ArgList
)
{
	int const _Result = __stdio_common_vsprintf_p(
		_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS,
		_Buffer, _BufferCount, _Format, _Locale, _ArgList);

	return _Result < 0 ? -1 : _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_vsprintf_p_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vsprintf_p_downlevel(
	_Out_writes_(_BufferCount) _Always_(_Post_z_) char*       const _Buffer,
	_In_                                          size_t      const _BufferCount,
	_In_z_ _Printf_format_string_                 char const* const _Format,
	va_list           _ArgList
)
{
	return _vsprintf_p_l(_Buffer, _BufferCount, _Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vsprintf_p_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vsnprintf_s_l_downlevel(
	_Out_writes_opt_(_BufferCount) _Always_(_Post_z_) char*       const _Buffer,
	_In_                                              size_t      const _BufferCount,
	_In_                                              size_t      const _MaxCount,
	_In_z_ _Printf_format_string_params_(2)           char const* const _Format,
	_In_opt_                                          _locale_t   const _Locale,
	va_list          _ArgList
)
{
	int const _Result = __stdio_common_vsnprintf_s(
		_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS,
		_Buffer, _BufferCount, _MaxCount, _Format, _Locale, _ArgList);

	return _Result < 0 ? -1 : _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_vsnprintf_s_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vsnprintf_s_downlevel(
	_Out_writes_opt_(_BufferCount) _Always_(_Post_z_) char*       const _Buffer,
	_In_                                              size_t      const _BufferCount,
	_In_                                              size_t      const _MaxCount,
	_In_z_ _Printf_format_string_                     char const* const _Format,
	va_list           _ArgList
)
{
	return _vsnprintf_s_l(_Buffer, _BufferCount, _MaxCount, _Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vsnprintf_s_downlevel);

#endif

//vsnprintf_s = _vsnprintf_s 别名转发



#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vscprintf_l_downlevel(
	_In_z_ _Printf_format_string_params_(2) char const* const _Format,
	_In_opt_                                _locale_t   const _Locale,
	va_list           _ArgList
)
{
	int const _Result = __stdio_common_vsprintf(
		_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS | _CRT_INTERNAL_PRINTF_STANDARD_SNPRINTF_BEHAVIOR,
		NULL, 0, _Format, _Locale, _ArgList);

	return _Result < 0 ? -1 : _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_vscprintf_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vscprintf_p_l_downlevel(
	_In_z_ _Printf_format_string_params_(2) char const* const _Format,
	_In_opt_                                _locale_t   const _Locale,
	va_list           _ArgList
)
{
	int const _Result = __stdio_common_vsprintf_p(
		_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS | _CRT_INTERNAL_PRINTF_STANDARD_SNPRINTF_BEHAVIOR,
		NULL, 0, _Format, _Locale, _ArgList);

	return _Result < 0 ? -1 : _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_vscprintf_p_l_downlevel);

#endif


extern "C" int __CRTDECL _vscprintf_p_downlevel(
	_In_z_ _Printf_format_string_ char const* const _Format,
	va_list           _ArgList
)
{
	return _vscprintf_p_l(_Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vscprintf_p_downlevel);

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vsnprintf_c_l_downlevel(
	_Out_writes_opt_(_BufferCount)          char*       const _Buffer,
	_In_                                    size_t      const _BufferCount,
	_In_z_ _Printf_format_string_params_(2) char const* const _Format,
	_In_opt_                                _locale_t   const _Locale,
	va_list           _ArgList
)
{
	int const _Result = __stdio_common_vsprintf(
		_CRT_INTERNAL_LOCAL_PRINTF_OPTIONS,
		_Buffer, _BufferCount, _Format, _Locale, _ArgList);

	return _Result < 0 ? -1 : _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_vsnprintf_c_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _vsnprintf_c_downlevel(
	_Out_writes_opt_(_BufferCount) char*       const _Buffer,
	_In_                           size_t      const _BufferCount,
	_In_z_ _Printf_format_string_  char const* const _Format,
	va_list           _ArgList
)
{
	return _vsnprintf_c_l(_Buffer, _BufferCount, _Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vsnprintf_c_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _sprintf_l_downlevel(
	_Pre_notnull_ _Always_(_Post_z_)        char*       const _Buffer,
	_In_z_ _Printf_format_string_params_(0) char const* const _Format,
	_In_opt_                                _locale_t   const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);

	#pragma warning(push)
	#pragma warning(disable: 4996) // Deprecation
	_Result = _vsprintf_l(_Buffer, _Format, _Locale, _ArgList);
	#pragma warning(pop)

	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_sprintf_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _sprintf_s_l_downlevel(
	_Out_writes_(_BufferCount) _Always_(_Post_z_) char*       const _Buffer,
	_In_                                          size_t      const _BufferCount,
	_In_z_ _Printf_format_string_params_(0)       char const* const _Format,
	_In_opt_                                      _locale_t   const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vsprintf_s_l(_Buffer, _BufferCount, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_sprintf_s_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL sprintf_s_downlevel(
	_Out_writes_(_BufferCount) _Always_(_Post_z_) char*       const _Buffer,
	_In_                                          size_t      const _BufferCount,
	_In_z_ _Printf_format_string_                 char const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vsprintf_s_l(_Buffer, _BufferCount, _Format, NULL, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(sprintf_s_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _sprintf_p_l_downlevel(
	_Out_writes_(_BufferCount) _Always_(_Post_z_) char*       const _Buffer,
	_In_                                          size_t      const _BufferCount,
	_In_z_ _Printf_format_string_params_(0)       char const* const _Format,
	_In_opt_                                      _locale_t   const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vsprintf_p_l(_Buffer, _BufferCount, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_sprintf_p_l_downlevel);

#endif

extern "C" int __CRTDECL _sprintf_p_downlevel(
	_Out_writes_(_BufferCount) _Always_(_Post_z_) char*       const _Buffer,
	_In_                                          size_t      const _BufferCount,
	_In_z_ _Printf_format_string_                 char const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vsprintf_p_l(_Buffer, _BufferCount, _Format, NULL, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_sprintf_p_downlevel);


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _snprintf_l_downlevel(
	_Out_writes_opt_(_BufferCount) _Post_maybez_ char*       const _Buffer,
	_In_                                         size_t      const _BufferCount,
	_In_z_ _Printf_format_string_params_(0)      char const* const _Format,
	_In_opt_                                     _locale_t   const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);

	#pragma warning(push)
	#pragma warning(disable: 4996) // Deprecation
	_Result = _vsnprintf_l(_Buffer, _BufferCount, _Format, _Locale, _ArgList);
	#pragma warning(pop)

	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_snprintf_l_downlevel);

#endif

extern "C" int __CRTDECL snprintf_downlevel(
	_Out_writes_opt_(_BufferCount) _Always_(_Post_z_) char*       const _Buffer,
	_In_                                              size_t      const _BufferCount,
	_In_z_ _Printf_format_string_                     char const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	#pragma warning(suppress:28719)    // 28719
	_Result = vsnprintf(_Buffer, _BufferCount, _Format, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(snprintf_downlevel);



#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _snprintf_c_l_downlevel(
	_Out_writes_opt_(_BufferCount)          char*       const _Buffer,
	_In_                                    size_t      const _BufferCount,
	_In_z_ _Printf_format_string_params_(0) char const* const _Format,
	_In_opt_                                _locale_t   const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vsnprintf_c_l(_Buffer, _BufferCount, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_snprintf_c_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _snprintf_c_downlevel(
	_Out_writes_opt_(_BufferCount) char*       const _Buffer,
	_In_                           size_t      const _BufferCount,
	_In_z_ _Printf_format_string_  char const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vsnprintf_c_l(_Buffer, _BufferCount, _Format, NULL, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_snprintf_c_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _snprintf_s_l_downlevel(
	_Out_writes_opt_(_BufferCount) _Always_(_Post_z_) char*       const _Buffer,
	_In_                                              size_t      const _BufferCount,
	_In_                                              size_t      const _MaxCount,
	_In_z_ _Printf_format_string_params_(0)           char const* const _Format,
	_In_opt_                                          _locale_t   const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vsnprintf_s_l(_Buffer, _BufferCount, _MaxCount, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_snprintf_s_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _snprintf_s_downlevel(
	_Out_writes_opt_(_BufferCount) _Always_(_Post_z_) char*       const _Buffer,
	_In_                                              size_t      const _BufferCount,
	_In_                                              size_t      const _MaxCount,
	_In_z_ _Printf_format_string_                     char const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vsnprintf_s_l(_Buffer, _BufferCount, _MaxCount, _Format, NULL, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_snprintf_s_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _scprintf_l_downlevel(
	_In_z_ _Printf_format_string_params_(0) char const* const _Format,
	_In_opt_                                _locale_t   const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vscprintf_l(_Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_scprintf_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _scprintf_p_l_downlevel(
	_In_z_ _Printf_format_string_params_(0) char const* const _Format,
	_In_opt_                                _locale_t   const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vscprintf_p_l(_Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_scprintf_p_l_downlevel);

#endif

extern "C" int __CRTDECL _scprintf_p_downlevel(
	_In_z_ _Printf_format_string_ char const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);
	_Result = _vscprintf_p(_Format, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_scprintf_p_downlevel);


extern "C" int __CRTDECL _vsscanf_l_downlevel(
	_In_z_                        char const* const _Buffer,
	_In_z_ _Printf_format_string_ char const* const _Format,
	_In_opt_                      _locale_t   const _Locale,
	va_list           _ArgList
)
{
	return __stdio_common_vsscanf(
		_CRT_INTERNAL_LOCAL_SCANF_OPTIONS,
		_Buffer, (size_t)-1, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vsscanf_l_downlevel);

extern "C" int __CRTDECL vsscanf_downlevel(
	_In_z_                        char const* const _Buffer,
	_In_z_ _Printf_format_string_ char const* const _Format,
	va_list           _ArgList
)
{
	return _vsscanf_l(_Buffer, _Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(vsscanf_downlevel);


extern "C" int __CRTDECL _vsscanf_s_l_downlevel(
	_In_z_                        char const* const _Buffer,
	_In_z_ _Printf_format_string_ char const* const _Format,
	_In_opt_                      _locale_t   const _Locale,
	va_list           _ArgList
)
{
	return __stdio_common_vsscanf(
		_CRT_INTERNAL_LOCAL_SCANF_OPTIONS | _CRT_INTERNAL_SCANF_SECURECRT,
		_Buffer, (size_t)-1, _Format, _Locale, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(_vsscanf_s_l_downlevel);


extern "C" int __CRTDECL vsscanf_s_downlevel(
	_In_z_                        char const* const _Buffer,
	_In_z_ _Printf_format_string_ char const* const _Format,
	va_list           _ArgList
)
{
	return _vsscanf_s_l(_Buffer, _Format, NULL, _ArgList);
}

_LCRT_DEFINE_IAT_SYMBOL(vsscanf_s_downlevel);


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _sscanf_l_downlevel(
	_In_z_                                 char const* const _Buffer,
	_In_z_ _Scanf_format_string_params_(0) char const* const _Format,
	_In_opt_                               _locale_t   const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vsscanf_l(_Buffer, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_sscanf_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _sscanf_s_l_downlevel(
	_In_z_                                   char const* const _Buffer,
	_In_z_ _Scanf_s_format_string_params_(0) char const* const _Format,
	_In_opt_                                 _locale_t   const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);
	_Result = _vsscanf_s_l(_Buffer, _Format, _Locale, _ArgList);
	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_sscanf_s_l_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL sscanf_s_downlevel(
	_In_z_                         char const* const _Buffer,
	_In_z_ _Scanf_s_format_string_ char const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);

	#pragma warning(push)
	#pragma warning(disable: 4996) // Deprecation
	_Result = vsscanf_s(_Buffer, _Format, _ArgList);
	#pragma warning(pop)

	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(sscanf_s_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _snscanf_l_downlevel(
	_In_reads_bytes_(_BufferCount) _Pre_z_ char const* const _Buffer,
	_In_                                   size_t      const _BufferCount,
	_In_z_ _Scanf_format_string_params_(0) char const* const _Format,
	_In_opt_                               _locale_t   const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);

	_Result = __stdio_common_vsscanf(
		_CRT_INTERNAL_LOCAL_SCANF_OPTIONS,
		_Buffer, _BufferCount, _Format, _Locale, _ArgList);

	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_snscanf_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _snscanf_s_l_downlevel(
	_In_reads_bytes_(_BufferCount) _Pre_z_   char const* const _Buffer,
	_In_                                     size_t      const _BufferCount,
	_In_z_ _Scanf_s_format_string_params_(0) char const* const _Format,
	_In_opt_                                 _locale_t   const _Locale,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Locale);

	_Result = __stdio_common_vsscanf(
		_CRT_INTERNAL_LOCAL_SCANF_OPTIONS | _CRT_INTERNAL_SCANF_SECURECRT,
		_Buffer, _BufferCount, _Format, _Locale, _ArgList);

	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_snscanf_s_l_downlevel);

#endif


#ifdef _ATL_XP_TARGETING
extern "C" int __CRTDECL _snscanf_s_downlevel(
	_In_reads_bytes_(_BufferCount) _Pre_z_ char const* const _Buffer,
	_In_                                   size_t      const _BufferCount,
	_In_z_ _Scanf_s_format_string_         char const* const _Format,
	...)
{
	int _Result;
	va_list _ArgList;
	__crt_va_start(_ArgList, _Format);

	_Result = __stdio_common_vsscanf(
		_CRT_INTERNAL_LOCAL_SCANF_OPTIONS | _CRT_INTERNAL_SCANF_SECURECRT,
		_Buffer, _BufferCount, _Format, NULL, _ArgList);

	__crt_va_end(_ArgList);
	return _Result;
}

_LCRT_DEFINE_IAT_SYMBOL(_snscanf_s_downlevel);

#endif

