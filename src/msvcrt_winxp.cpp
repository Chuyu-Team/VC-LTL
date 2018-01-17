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

#if defined _X86_
EXTERN_C _CRTIMP EXCEPTION_DISPOSITION __CxxFrameHandler(
	EHExceptionRecord  *pExcept,
	EHRegistrationNode *pRN,
	void *pContext,
	DispatcherContext  *pDC
	);

EXTERN_C __declspec(naked) DECLSPEC_GUARD_SUPPRESS EXCEPTION_DISPOSITION __cdecl __CxxFrameHandler3_downlevel(
	/*
	EAX=FuncInfo   *pFuncInfo,          // Static information for this frame
	*/
	EHExceptionRecord  *pExcept,        // Information for this exception
	EHRegistrationNode *pRN,            // Dynamic information for this frame
	void               *pContext,       // Context info (we don't care what's in it)
	DispatcherContext  *pDC             // More dynamic info for this frame (ignored on Intel)
	)
{
	//FuncInfo   *pFuncInfo;
	_s_FuncInfo FuncInfo_downlevel;

	__asm
	{
		//
		// Standard function prolog
		//
		push    ebp
		mov     ebp, esp
		sub     esp, 0x24
		push    ebx
		push    esi
		push    edi
		cld             // A bit of paranoia -- Our code-gen assumes this

		//
		// Save the extra parameter
		//
		mov     esi, eax
		mov     ecx, 9
		lea     edi, FuncInfo_downlevel
		rep movsd
	}

	FuncInfo_downlevel.magicNumber = EH_MAGIC_NUMBER1;

	__asm
	{
		lea eax, FuncInfo_downlevel
	}

	__CxxFrameHandler(pExcept, pRN, pContext, pDC);

	__asm
	{
		pop     edi
		pop     esi
		pop     ebx
		mov     esp, ebp
		pop     ebp
		retn
	}
}
#elif defined _AMD64_
EXTERN_C _CRTIMP EXCEPTION_DISPOSITION __CxxFrameHandler(
	EHExceptionRecord  *pExcept,
	EHRegistrationNode RN,
	CONTEXT *pContext,
	DispatcherContext  *pDC
	);

static volatile long ProtectFlag = 0;

EXTERN_C DECLSPEC_GUARD_SUPPRESS EXCEPTION_DISPOSITION __cdecl __CxxFrameHandler3_downlevel(
	EHExceptionRecord  *pExcept,         // Information for this exception
	EHRegistrationNode RN,               // Dynamic information for this frame
	CONTEXT            *pContext,        // Context info
	DispatcherContext  *pDC              // More dynamic info for this frame
	)
{
	auto pFuncInfo = (_s_FuncInfo*)(pDC->ImageBase + *(DWORD*)pDC->HandlerData);

	if (pFuncInfo->magicNumber != EH_MAGIC_NUMBER1)
	{

		while (InterlockedExchangeAdd(&ProtectFlag, 1) != 0)
		{
			InterlockedDecrement(&ProtectFlag);

			Sleep(10);
		}

		DWORD OldProtect;

		//开启修改magicNumber
		if (VirtualProtect(pFuncInfo, 4, PAGE_EXECUTE_READWRITE, &OldProtect))
		{
			pFuncInfo->magicNumber = EH_MAGIC_NUMBER1;

			VirtualProtect(pFuncInfo, 4, OldProtect, &OldProtect);
		}

		InterlockedDecrement(&ProtectFlag);
	}

	return __CxxFrameHandler(pExcept, RN, pContext, pDC);
}

#else
#error "暂不支持此体系"
#endif

_LCRT_DEFINE_IAT_SYMBOL(__CxxFrameHandler3_downlevel);

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
EXTERN_C _CRTIMP extern unsigned int __lc_collate_cp;

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
EXTERN_C extern BOOL __isa_available;
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
		&& (__isa_available ==FALSE || (_mm_getcsr() & 0xFEC0) == 0x1E80))
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


EXTERN_C _CRTIMP char* __cdecl _cgets(_Inout_z_ char* const string);

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


EXTERN_C _CRTIMP wchar_t* __cdecl _cgetws(_Inout_z_ wchar_t* const string);

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
