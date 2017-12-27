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
