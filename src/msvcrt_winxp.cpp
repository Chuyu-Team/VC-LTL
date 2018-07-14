/*


此文件用于实现WinXP的msvcrt.dll不支持的函数

*/

#include <corecrt.h>
#include <internal_shared.h>
#include <ehdata.h>
#include <msvcrt_IAT.h>
#include <float.h>
#include <mbstring.h>
#include <locale.h>
#include <corecrt_io.h>
#include <corecrt_wio.h>
#include <corecrt_wtime.h>
#include <time.h>
#include <intrin.h> 
#include <stdio.h>
#include <conio.h>


EXTERN_C errno_t __cdecl _controlfp_s_downlevel(
	_Out_opt_ unsigned int* _CurrentState,
	_In_      unsigned int  _NewValue,
	_In_      unsigned int  _Mask
	)
{
	_Mask &= 0xFFF7FFFF;

	if ((_NewValue&_Mask) & 0xFCF0FCE0)
	{
		if (_CurrentState)
			*_CurrentState = _control87(0, 0);

		errno = EINVAL;
		_invalid_parameter_noinfo();
		return EINVAL;
	}
	else
	{
		auto State = _control87(_NewValue, _Mask);
		if (_CurrentState)
			*_CurrentState = State;

		return 0;
	}
}

_LCRT_DEFINE_IAT_SYMBOL(_controlfp_s_downlevel);


//EXTERN_C errno_t __cdecl _mbslwr_s_downlevel(
//	_Inout_updates_opt_z_(_SizeInBytes) unsigned char* _String,
//	_In_                                size_t         _SizeInBytes
//	)
//{
//	if (_String == nullptr && _SizeInBytes == 0)
//		return 0;
//
//	_VALIDATE_RETURN_ERRCODE(_String && _SizeInBytes >0 || strnlen((char*)_String, _SizeInBytes) < _SizeInBytes, EINVAL);
//
//	_mbslwr(_String);
//	
//	return 0;
//}

//EXTERN_C errno_t __cdecl _mbsupr_s_downlevel(
//	_Inout_updates_z_(_SizeInBytes) unsigned char* _Str,
//	_In_                            size_t         _SizeInBytes
//	)
//{
//	if (_Str == nullptr && _SizeInBytes == 0)
//		return 0;
//
//	_VALIDATE_RETURN_ERRCODE(_Str && _SizeInBytes >0 || strnlen((char*)_Str, _SizeInBytes) < _SizeInBytes, EINVAL);
//
//	_mbsupr(_Str);
//
//	return 0;
//}


EXTERN_C errno_t __cdecl _gcvt_s_downlevel(
	_Out_writes_z_(_BufferCount) char*  _Buffer,
	_In_                         size_t _BufferCount,
	_In_                         double _Value,
	_In_                         int    _DigitCount
	)
{
	_VALIDATE_RETURN_ERRCODE(_Buffer && _BufferCount >0, EINVAL);

	*_Buffer = '\0';

	_VALIDATE_RETURN_ERRCODE(_BufferCount>_DigitCount, ERANGE);

	char buffer[_CVTBUFSIZE];

	_gcvt(_Value, _DigitCount, buffer);

	return strcpy_s(_Buffer, _BufferCount, buffer);
}

_LCRT_DEFINE_IAT_SYMBOL(_gcvt_s_downlevel);


EXTERN_C errno_t __cdecl _ecvt_s_downlevel(
	_Out_writes_z_(_BufferCount) char* _Buffer,
	_In_  size_t                       _BufferCount,
	_In_  double                       _Value,
	_In_  int                          _DigitCount,
	_Out_ int*                         _PtDec,
	_Out_ int*                         _PtSign
	)
{
	_VALIDATE_RETURN_ERRCODE(_Buffer && _BufferCount >0, EINVAL);

	*_Buffer = '\0';

	_VALIDATE_RETURN_ERRCODE(_PtDec&&_PtSign, EINVAL);

	return strcpy_s(_Buffer,
		_BufferCount,
		_ecvt(_Value, _DigitCount, _PtDec, _PtSign));
}

_LCRT_DEFINE_IAT_SYMBOL(_ecvt_s_downlevel);


//EXTERN_C errno_t __cdecl _strlwr_s_downlevel(
//	_Inout_updates_z_(_Size) char*  _String,
//	_In_                     size_t _Size
//	)
//{
//	_VALIDATE_RETURN_ERRCODE(_String && strnlen(_String, _Size) < _Size, EINVAL);
//
//	_strlwr(_String);
//
//	return 0;
//}

//EXTERN_C errno_t __cdecl _strupr_s_downlevel(
//	_Inout_updates_z_(_Size) char*  _String,
//	_In_                     size_t _Size
//	)
//{
//	_VALIDATE_RETURN_ERRCODE(_String && strnlen(_String, _Size) < _Size, EINVAL);
//
//	_strupr(_String);
//	return 0;
//}

//EXTERN_C errno_t __cdecl _wcslwr_s_downlevel(
//	_Inout_updates_z_(_SizeInWords) wchar_t* _String,
//	_In_                            size_t   _SizeInWords
//	)
//{
//	_VALIDATE_RETURN_ERRCODE(_String && wcsnlen(_String, _SizeInWords) < _SizeInWords, EINVAL);
//
//	_wcslwr(_String);
//
//	return 0;
//}

//EXTERN_C errno_t __cdecl _wcsupr_s_downlevel(
//	_Inout_updates_z_(_Size) wchar_t* _String,
//	_In_                     size_t   _Size
//	)
//{
//	_VALIDATE_RETURN_ERRCODE(_String && wcsnlen(_String, _Size) < _Size, EINVAL);
//
//	_wcsupr(_String);
//
//	return 0;
//}
#ifdef _X86_
EXTERN_C __declspec(dllimport) extern unsigned int __lc_collate_cp;

EXTERN_C unsigned int __cdecl ___lc_collate_cp_func_downlevel(void)
{
	return __lc_collate_cp;
}

_LCRT_DEFINE_IAT_SYMBOL(___lc_collate_cp_func_downlevel);
#endif


#ifdef _X86_

EXTERN_C const wctype_t* __cdecl __pwctype_func_downlevel(void)
{
	return _pwctype;
}

_LCRT_DEFINE_IAT_SYMBOL(__pwctype_func_downlevel);
#endif


EXTERN_C errno_t __cdecl _mktemp_s_downlevel(
	_Inout_updates_z_(_Size) char*  _TemplateName,
	_In_                     size_t _Size
	)
{
	_VALIDATE_RETURN_ERRCODE(_TemplateName != nullptr && _Size > 0, EINVAL);

	if (strnlen(_TemplateName, _Size)>= _Size)
	{
		_VALIDATE_RETURN((L"String is not null terminated" && 0), EINVAL, EINVAL)
	}

	return _mktemp(_TemplateName) ? 0 : errno;
}

_LCRT_DEFINE_IAT_SYMBOL(_mktemp_s_downlevel);


EXTERN_C errno_t __cdecl _wmktemp_s_downlevel(
	_Inout_updates_z_(_SizeInWords) wchar_t* _TemplateName,
	_In_                            size_t   _SizeInWords
	)
{
	_VALIDATE_RETURN_ERRCODE(_TemplateName != nullptr && _SizeInWords > 0, EINVAL);

	if (wcsnlen(_TemplateName, _SizeInWords) >= _SizeInWords)
	{
		_VALIDATE_RETURN((L"String is not null terminated" && 0), EINVAL, EINVAL)
	}

	return _wmktemp(_TemplateName) ? 0 : errno;
}

_LCRT_DEFINE_IAT_SYMBOL(_wmktemp_s_downlevel);

EXTERN_C errno_t __cdecl _searchenv_s_downlevel(
	_In_z_                       char const* _Filename,
	_In_z_                       char const* _VarName,
	_Out_writes_z_(_BufferCount) char*       _Buffer,
	_In_                         size_t      _BufferCount
	)
{
	if (_BufferCount >= MAX_PATH)
	{
		_searchenv(_Filename, _VarName, _Buffer);

		return errno;
	}
	else
	{
		char Temp[MAX_PATH];

		_searchenv(_Filename, _VarName, Temp);

		auto err = errno;

		if (err ==0)
		{
			return strcpy_s(_Buffer, _BufferCount, Temp);
		}
		else
		{
			return err;
		}
	}
}

_LCRT_DEFINE_IAT_SYMBOL(_searchenv_s_downlevel);

EXTERN_C errno_t __cdecl _wsearchenv_s_downlevel(
	_In_z_                       wchar_t const* _Filename,
	_In_z_                       wchar_t const* _VarName,
	_Out_writes_z_(_BufferCount) wchar_t*       _Buffer,
	_In_                         size_t         _BufferCount
	)
{
	if (_BufferCount >= MAX_PATH)
	{
		_wsearchenv(_Filename, _VarName, _Buffer);

		return errno;
	}
	else
	{
		wchar_t Temp[MAX_PATH];

		_wsearchenv(_Filename, _VarName, Temp);

		auto err = errno;

		if (err == 0)
		{
			return wcscpy_s(_Buffer, _BufferCount, Temp);
		}
		else
		{
			return err;
		}
	}
}

_LCRT_DEFINE_IAT_SYMBOL(_wsearchenv_s_downlevel);

EXTERN_C errno_t __cdecl _fcvt_s_downlevel(
	_Out_writes_z_(_BufferCount) char*  _Buffer,
	_In_                         size_t _BufferCount,
	_In_                         double _Value,
	_In_                         int    _FractionalDigitCount,
	_Out_                        int*   _PtDec,
	_Out_                        int*   _PtSign
	)
{
	_VALIDATE_RETURN_ERRCODE(_Buffer != nullptr, EINVAL);
	_VALIDATE_RETURN_ERRCODE(_BufferCount > 0, EINVAL);

	*_Buffer = '\0';

	_VALIDATE_RETURN_ERRCODE(_PtDec != nullptr, EINVAL);
	_VALIDATE_RETURN_ERRCODE(_PtSign != nullptr, EINVAL);

	auto szValue = _fcvt(_Value, _FractionalDigitCount, _PtDec, _PtSign);

	if (!szValue)
		return errno;

	return strcpy_s(_Buffer, _BufferCount, szValue);
}

_LCRT_DEFINE_IAT_SYMBOL(_fcvt_s_downlevel);

EXTERN_C errno_t __cdecl _strdate_s_downlevel(
	_Out_writes_(_SizeInBytes) _Post_readable_size_(9) char*  _Buffer,
	_In_  _In_range_(>= , 9)                            size_t _SizeInBytes
	)
{
	_VALIDATE_RETURN_ERRCODE(_Buffer != nullptr && _SizeInBytes > 0, EINVAL);

	*_Buffer = 0;

	_VALIDATE_RETURN_ERRCODE(_SizeInBytes >= 9, ERANGE);


	return _strtime(_Buffer) ? 0 : errno;
}

_LCRT_DEFINE_IAT_SYMBOL(_strdate_s_downlevel);

EXTERN_C errno_t __cdecl _wstrdate_s_downlevel(
	_Out_writes_z_(_SizeInWords) _When_(_SizeInWords >= 9, _Post_readable_size_(9)) wchar_t* _Buffer,
	_In_                                                                           size_t   _SizeInWords
	)
{
	_VALIDATE_RETURN_ERRCODE(_Buffer != nullptr && _SizeInWords > 0, EINVAL);

	*_Buffer = 0;

	_VALIDATE_RETURN_ERRCODE(_SizeInWords >= 9, ERANGE);

	return _wstrtime(_Buffer) ? 0 : errno;
}

_LCRT_DEFINE_IAT_SYMBOL(_wstrdate_s_downlevel);


#ifdef _X86_
EXTERN_C extern int __isa_available;
#endif

EXTERN_C void __cdecl _set_controlfp_downlevel(
	_In_ unsigned int _NewValue,
	_In_ unsigned int _Mask
	)
{
	if (_NewValue == 0x9001F
		|| _Mask==0xFFFFFFFF)
	{
		return;
	}

#ifdef _M_IX86
	UINT16 currentState;

	__asm { fstcw currentState }

	if ((currentState&0x1F3D)== 0x23D 
		&& (__isa_available < 1 || (_mm_getcsr() & 0xFEC0) == 0x1E80))
	{
		return;
	}
#elif defined (_M_AMD64)
	if ((_mm_getcsr() & 0xFEC0) == 0x1E80)
		return;
#else
#error "不支持此体系"
#endif

	_controlfp(_NewValue, _Mask &0xFFF7FFFF);
}

_LCRT_DEFINE_IAT_SYMBOL(_set_controlfp_downlevel);

EXTERN_C errno_t __cdecl _strtime_s_downlevel(
	_Out_writes_(_SizeInBytes) _When_(_SizeInBytes >= 9, _Post_readable_size_(9)) char*  _Buffer,
	_In_                                                                         size_t _SizeInBytes
	)
{
	_VALIDATE_RETURN_ERRCODE(_Buffer != nullptr && _SizeInBytes > 0, EINVAL);
	*_Buffer = 0;
	_VALIDATE_RETURN_ERRCODE(_SizeInBytes >= 9, ERANGE);

	return _strtime(_Buffer) ? 0 : errno;
}

_LCRT_DEFINE_IAT_SYMBOL(_strtime_s_downlevel);

EXTERN_C errno_t __cdecl _wstrtime_s_downlevel(
	_Out_writes_z_(_SizeInWords) _When_(_SizeInWords >= 9, _Post_readable_size_(9)) wchar_t* _Buffer,
	_In_                                                                           size_t   _SizeInWords
	)
{
	_VALIDATE_RETURN_ERRCODE(_Buffer != nullptr && _SizeInWords > 0, EINVAL);
	*_Buffer = 0;
	_VALIDATE_RETURN_ERRCODE(_SizeInWords >= 9, ERANGE);

	return _wstrtime(_Buffer) ? 0 : errno;
}

_LCRT_DEFINE_IAT_SYMBOL(_wstrtime_s_downlevel);

EXTERN_C errno_t __cdecl tmpfile_s_downlevel(
	_Out_opt_ _Deref_post_valid_ FILE** _Stream
	)
{
	_VALIDATE_RETURN_ERRCODE(_Stream != nullptr, EINVAL);

	return (*_Stream = tmpfile()) ? 0 : errno;
}

_LCRT_DEFINE_IAT_SYMBOL(tmpfile_s_downlevel);


EXTERN_C __declspec(dllimport) char* __cdecl _cgets(_Inout_z_ char* const string);

EXTERN_C errno_t __cdecl _cgets_s_downlevel(
	_Out_writes_z_(_BufferCount) char*   _Buffer,
	_In_                         size_t  _BufferCount,
	_Out_                        size_t* _SizeRead
	)
{
	_VALIDATE_CLEAR_OSSERR_RETURN_ERRCODE(_Buffer != nullptr, EINVAL);
	_VALIDATE_CLEAR_OSSERR_RETURN_ERRCODE(_BufferCount > 0, EINVAL);

	*_Buffer = 0;

	_VALIDATE_CLEAR_OSSERR_RETURN_ERRCODE(_SizeRead != nullptr, EINVAL);

	*_SizeRead = 0;

	_BufferCount = min(0xFF, _BufferCount);

	byte TempBuffer[0xFF + 2];
	*TempBuffer = _BufferCount;

	if (!_cgets((char*)TempBuffer))
	{
		return errno;
	}

	memcpy(_Buffer, TempBuffer + 2, *_SizeRead = TempBuffer[2]);

	return 0;
}

_LCRT_DEFINE_IAT_SYMBOL(_cgets_s_downlevel);


EXTERN_C __declspec(dllimport) wchar_t* __cdecl _cgetws(_Inout_z_ wchar_t* const string);

EXTERN_C errno_t __cdecl _cgetws_s_downlevel(
	_Out_writes_to_(_BufferCount, *_SizeRead) wchar_t* _Buffer,
	_In_                                      size_t   _BufferCount,
	_Out_                                     size_t*  _SizeRead
	)
{
	_VALIDATE_CLEAR_OSSERR_RETURN_ERRCODE(_Buffer != nullptr, EINVAL);
	_VALIDATE_CLEAR_OSSERR_RETURN_ERRCODE(_BufferCount > 0, EINVAL);

	*_Buffer = 0;

	_VALIDATE_CLEAR_OSSERR_RETURN_ERRCODE(_SizeRead != nullptr, EINVAL);

	*_SizeRead = 0;

	_BufferCount= max(_BufferCount, 0xFFFF);

	auto Tmp = (wchar_t*)_malloca((_BufferCount + 2) * sizeof(*_Buffer));
	if (!Tmp)
		return errno;

	*Tmp = _BufferCount;

	errno_t error = 0;

	if (_cgetws(Tmp))
	{
		memcpy(_Buffer, Tmp + 2, (*_SizeRead = Tmp[1]) + 1);
	}
	else
	{
		error = errno;
	}

	_freea(Tmp);

	return error;
}

_LCRT_DEFINE_IAT_SYMBOL(_cgetws_s_downlevel);

//XP中 _chsize_s 的 size参数不允许大于 INT_MAX
EXTERN_C errno_t __cdecl _chsize_s_downlevel(int const fh, __int64 const size)
{
	_VALIDATE_CLEAR_OSSERR_RETURN_ERRCODE((size >= 0 && size<=INT_MAX), EINVAL);

	return _chsize(fh, (long)size) ? errno : 0;
}

_LCRT_DEFINE_IAT_SYMBOL(_chsize_s_downlevel);


EXTERN_C __declspec(dllimport) int __cdecl _XcptFilter(
	unsigned long       const xcptnum,
	PEXCEPTION_POINTERS const pxcptinfoptrs
	);

EXTERN_C int __cdecl __CppXcptFilter_downlevel(
	unsigned long       const xcptnum,
	PEXCEPTION_POINTERS const pxcptinfoptrs
	)
{
	if (xcptnum != ('msc' | 0xE0000000))
		return EXCEPTION_CONTINUE_SEARCH;

	return _XcptFilter(xcptnum, pxcptinfoptrs);
}

_LCRT_DEFINE_IAT_SYMBOL(__CppXcptFilter_downlevel);

extern "C" int __cdecl _initterm_e_downlevel(_PIFV* const first, _PIFV* const last)
{
	for (_PIFV* it = first; it != last; ++it)
	{
		if (*it == nullptr)
			continue;

		int const result = (**it)();
		if (result != 0)
			return result;
	}

	return 0;
}

_LCRT_DEFINE_IAT_SYMBOL(_initterm_e_downlevel);


static __forceinline size_t __cdecl strlen_t(
	_In_z_ char const* _Str
	)
{
	return strlen(_Str);
}

static __forceinline size_t __cdecl strlen_t(
	_In_z_ wchar_t const* const _Str
	)
{
	return wcslen(_Str);
}


template<class _Fun, class ValueType,class CharType>
static errno_t __cdecl common_xtox(
								_Fun&& _xtox,
	_In_                         ValueType _Value,
	_Out_writes_z_(_BufferCount) CharType* _Buffer,
	_In_                         size_t    _BufferCount,
	_In_                         int       _Radix
	)
{
	_VALIDATE_RETURN_ERRCODE(_Buffer != nullptr, EINVAL);
	_VALIDATE_RETURN_ERRCODE(_BufferCount > 0, EINVAL);
	_VALIDATE_RETURN_ERRCODE(2 <= _Radix && _Radix <= 36, EINVAL);

	//最多保存33(32bit)/65(64bit)个,保证缓冲区充足
	CharType Temp[sizeof(_Value)* 8 + 1];

	if (_BufferCount >= _countof(Temp))
	{
		//缓冲区充足直接调用老版本
		_xtox(_Value, _Buffer, _Radix);

		return 0;
	}

	*Temp = CharType('\0');

	_xtox(_Value, Temp, _Radix);

	auto cTemp = strlen_t(Temp);

	if (cTemp >= _BufferCount)
	{
		*_Buffer = CharType('\0');
		_VALIDATE_RETURN_ERRCODE(cTemp < _BufferCount, ERANGE);
	}

	memcpy(_Buffer, Temp, (cTemp + 1) * sizeof(CharType));

	return 0;
}

extern "C" errno_t __cdecl _i64toa_s_downlevel(
	_In_                         __int64 _Value,
	_Out_writes_z_(_BufferCount) char*   _Buffer,
	_In_                         size_t  _BufferCount,
	_In_                         int     _Radix
	)
{
	return common_xtox(_i64toa,_Value, _Buffer, _BufferCount, _Radix);
}

_LCRT_DEFINE_IAT_SYMBOL(_i64toa_s_downlevel);

extern "C" errno_t __cdecl _i64tow_s_downlevel(
	_In_                         __int64  _Value,
	_Out_writes_z_(_BufferCount) wchar_t* _Buffer,
	_In_                         size_t   _BufferCount,
	_In_                         int      _Radix
	)
{
	return common_xtox(_i64tow,_Value, _Buffer, _BufferCount, _Radix);
}

_LCRT_DEFINE_IAT_SYMBOL(_i64tow_s_downlevel);


extern "C" errno_t __cdecl _itoa_s_downlevel(
	_In_                         int    _Value,
	_Out_writes_z_(_BufferCount) char*  _Buffer,
	_In_                         size_t _BufferCount,
	_In_                         int    _Radix
	)
{
	return common_xtox(_itoa,_Value, _Buffer, _BufferCount, _Radix);
}

_LCRT_DEFINE_IAT_SYMBOL(_itoa_s_downlevel);


extern "C" errno_t __cdecl _itow_s_downlevel(
	_In_                         int      _Value,
	_Out_writes_z_(_BufferCount) wchar_t* _Buffer,
	_In_                         size_t   _BufferCount,
	_In_                         int      _Radix
	)
{
	return common_xtox(_itow,_Value, _Buffer, _BufferCount, _Radix);
}

_LCRT_DEFINE_IAT_SYMBOL(_itow_s_downlevel);


extern "C" errno_t __cdecl _ltoa_s_downlevel(
	_In_                         long   _Value,
	_Out_writes_z_(_BufferCount) char*  _Buffer,
	_In_                         size_t _BufferCount,
	_In_                         int    _Radix
	)
{
	return common_xtox(_ltoa,_Value, _Buffer, _BufferCount, _Radix);
}

_LCRT_DEFINE_IAT_SYMBOL(_ltoa_s_downlevel);


extern "C" errno_t __cdecl _ltow_s_downlevel(
	_In_                         long     _Value,
	_Out_writes_z_(_BufferCount) wchar_t* _Buffer,
	_In_                         size_t   _BufferCount,
	_In_                         int      _Radix
	)
{
	return common_xtox(_ltow,_Value, _Buffer, _BufferCount, _Radix);
}

_LCRT_DEFINE_IAT_SYMBOL(_ltow_s_downlevel);

extern "C" errno_t __cdecl _ui64toa_s_downlevel(
	_In_                         unsigned __int64 _Value,
	_Out_writes_z_(_BufferCount) char*            _Buffer,
	_In_                         size_t           _BufferCount,
	_In_                         int              _Radix
	)
{
	return common_xtox(_ui64toa,_Value, _Buffer, _BufferCount, _Radix);
}

_LCRT_DEFINE_IAT_SYMBOL(_ui64toa_s_downlevel);


extern "C" errno_t __cdecl _ui64tow_s_downlevel(
	_In_                         unsigned __int64 _Value,
	_Out_writes_z_(_BufferCount) wchar_t*         _Buffer,
	_In_                         size_t           _BufferCount,
	_In_                         int              _Radix
	)
{
	return common_xtox(_ui64tow,_Value, _Buffer, _BufferCount, _Radix);
}

_LCRT_DEFINE_IAT_SYMBOL(_ui64tow_s_downlevel);

extern "C" errno_t __cdecl _ultoa_s_downlevel(
	_In_                         unsigned long _Value,
	_Out_writes_z_(_BufferCount) char*         _Buffer,
	_In_                         size_t        _BufferCount,
	_In_                         int           _Radix
	)
{
	return common_xtox(_ultoa,_Value, _Buffer, _BufferCount, _Radix);
}

_LCRT_DEFINE_IAT_SYMBOL(_ultoa_s_downlevel);


extern "C" errno_t __cdecl _ultow_s_downlevel(
	_In_                         unsigned long _Value,
	_Out_writes_z_(_BufferCount) wchar_t*      _Buffer,
	_In_                         size_t        _BufferCount,
	_In_                         int           _Radix
	)
{
	return common_xtox(_ultow,_Value, _Buffer, _BufferCount, _Radix);
}

_LCRT_DEFINE_IAT_SYMBOL(_ultow_s_downlevel);

extern "C" errno_t __cdecl _splitpath_s_downlevel(
	_In_z_                             char const* _FullPath,
	_Out_writes_opt_z_(_DriveCount)    char*       _Drive,
	_In_                               size_t      _DriveCount,
	_Out_writes_opt_z_(_DirCount)      char*       _Dir,
	_In_                               size_t      _DirCount,
	_Out_writes_opt_z_(_FilenameCount) char*       _Filename,
	_In_                               size_t      _FilenameCount,
	_Out_writes_opt_z_(_ExtCount)      char*       _Ext,
	_In_                               size_t      _ExtCount
	)
{
	if (_FullPath == nullptr
		|| _Drive==nullptr && _DriveCount!=0
		|| _Dir==nullptr && _DirCount!=0
		|| _Filename==nullptr && _FilenameCount!=0
		|| _Ext==nullptr && _ExtCount!=0)
	{
		if (_Drive&&_DriveCount)
			*_Drive = '\0';

		if (_Dir&&_DirCount)
			*_Dir = '\0';

		if (_Dir&&_DirCount)
			*_Dir = '\0';

		if (_Filename&&_FilenameCount)
			*_Filename = '\0';

		if (_Ext&&_ExtCount)
			*_Ext = '\0';

		_VALIDATE_RETURN_ERRCODE(false, EINVAL);
	}

	char _DriveTmp[_MAX_DRIVE] = {};
	char _DirTmp[_MAX_DIR] = {};
	char _FilenameTmp[_MAX_FNAME] = {};
	char _ExtTmp[_MAX_EXT] = {};

	errno = 0;

	_splitpath(_FullPath, _Drive ? _DriveTmp : nullptr, _Dir ? _DirTmp : nullptr, _Filename ? _FilenameTmp : nullptr, _Ext ? _ExtTmp : nullptr);


	auto __errno = errno;

	if (__errno == 0)
	{
		//复制数据
		if (_Drive)
		{
			auto DriveCountTmp = strlen(_DriveTmp);

			if (DriveCountTmp >= _DriveCount)
			{
				__errno = errno = ERANGE;

				goto _Error;
			}

			memcpy(_Drive, _DriveTmp, (DriveCountTmp + 1) * sizeof(_Drive[0]));
		}

		if (_Dir)
		{
			auto _DirCountTmp = strlen(_DirTmp);

			if (_DirCountTmp >= _DirCount)
			{
				__errno = errno = ERANGE;

				goto _Error;
			}

			memcpy(_Dir, _DirTmp, (_DirCountTmp + 1) * sizeof(_Dir[0]));
		}

		if (_Filename)
		{
			auto _FilenameCountTmp = strlen(_FilenameTmp);

			if (_FilenameCountTmp >= _FilenameCount)
			{
				__errno = errno = ERANGE;

				goto _Error;
			}

			memcpy(_Filename, _FilenameTmp, (_FilenameCountTmp + 1) * sizeof(_Filename[0]));
		}

		if (_Ext)
		{
			auto _ExtCountTmp = strlen(_ExtTmp);

			if (_ExtCountTmp >= _ExtCount)
			{
				__errno = errno = ERANGE;

				goto _Error;
			}

			memcpy(_Ext, _ExtTmp, (_ExtCount + 1) * sizeof(_Ext[0]));

		}

		return 0;
	}

_Error:


	if (_Drive&&_DriveCount)
		*_Drive = '\0';

	if (_Dir&&_DirCount)
		*_Dir = '\0';

	if (_Dir&&_DirCount)
		*_Dir = '\0';

	if (_Filename&&_FilenameCount)
		*_Filename = '\0';

	if (_Ext&&_ExtCount)
		*_Ext = '\0';

	return __errno;
}

_LCRT_DEFINE_IAT_SYMBOL(_splitpath_s_downlevel);

extern "C" errno_t __cdecl _wsplitpath_s_downlevel(
	_In_z_                             wchar_t const* _FullPath,
	_Out_writes_opt_z_(_DriveCount)    wchar_t*       _Drive,
	_In_                               size_t         _DriveCount,
	_Out_writes_opt_z_(_DirCount)      wchar_t*       _Dir,
	_In_                               size_t         _DirCount,
	_Out_writes_opt_z_(_FilenameCount) wchar_t*       _Filename,
	_In_                               size_t         _FilenameCount,
	_Out_writes_opt_z_(_ExtCount)      wchar_t*       _Ext,
	_In_                               size_t         _ExtCount
	)
{
	if (_FullPath == nullptr
		|| _Drive == nullptr && _DriveCount != 0
		|| _Dir == nullptr && _DirCount != 0
		|| _Filename == nullptr && _FilenameCount != 0
		|| _Ext == nullptr && _ExtCount != 0)
	{
		if (_Drive&&_DriveCount)
			*_Drive = L'\0';

		if (_Dir&&_DirCount)
			*_Dir = L'\0';

		if (_Dir&&_DirCount)
			*_Dir = L'\0';

		if (_Filename&&_FilenameCount)
			*_Filename = L'\0';

		if (_Ext&&_ExtCount)
			*_Ext = L'\0';

		_VALIDATE_RETURN_ERRCODE(false, EINVAL);
	}

	wchar_t _DriveTmp[_MAX_DRIVE] = {};
	wchar_t _DirTmp[_MAX_DIR] = {};
	wchar_t _FilenameTmp[_MAX_FNAME] = {};
	wchar_t _ExtTmp[_MAX_EXT] = {};

	errno = 0;

	_wsplitpath(_FullPath, _Drive ? _DriveTmp : nullptr, _Dir ? _DirTmp : nullptr, _Filename ? _FilenameTmp : nullptr, _Ext ? _ExtTmp : nullptr);


	auto __errno = errno;

	if (__errno == 0)
	{
		//复制数据
		if (_Drive)
		{
			auto DriveCountTmp = wcslen(_DriveTmp);

			if (DriveCountTmp >= _DriveCount)
			{
				__errno = errno = ERANGE;

				goto _Error;
			}

			memcpy(_Drive, _DriveTmp, (DriveCountTmp + 1) * sizeof(_Drive[0]));
		}

		if (_Dir)
		{
			auto _DirCountTmp = wcslen(_DirTmp);

			if (_DirCountTmp >= _DirCount)
			{
				__errno = errno = ERANGE;

				goto _Error;
			}

			memcpy(_Dir, _DirTmp, (_DirCountTmp + 1) * sizeof(_Dir[0]));
		}

		if (_Filename)
		{
			auto _FilenameCountTmp = wcslen(_FilenameTmp);

			if (_FilenameCountTmp >= _FilenameCount)
			{
				__errno = errno = ERANGE;

				goto _Error;
			}

			memcpy(_Filename, _FilenameTmp, (_FilenameCountTmp + 1) * sizeof(_Filename[0]));
		}

		if (_Ext)
		{
			auto _ExtCountTmp = wcslen(_ExtTmp);

			if (_ExtCountTmp >= _ExtCount)
			{
				__errno = errno = ERANGE;

				goto _Error;
			}

			memcpy(_Ext, _ExtTmp, (_ExtCount + 1) * sizeof(_Ext[0]));

		}

		return 0;
	}

_Error:


	if (_Drive&&_DriveCount)
		*_Drive = L'\0';

	if (_Dir&&_DirCount)
		*_Dir = L'\0';

	if (_Dir&&_DirCount)
		*_Dir = L'\0';

	if (_Filename&&_FilenameCount)
		*_Filename = L'\0';

	if (_Ext&&_ExtCount)
		*_Ext = L'\0';

	return __errno;
}


_LCRT_DEFINE_IAT_SYMBOL(_wsplitpath_s_downlevel);
