#pragma once

#if defined(NDEBUG)&&defined(__Build_LTL)
//#define _CRT_BEST_PRACTICES_USAGE

#include <vcruntime_new.h>
#include <corecrt_terminate.h>
#include <stdlib.h>
#include <corecrt_wstdio.h>
#include <Windows.h>
#include <winnt.h>
#include <intrin.h>  
#include <vcruntime_exception.h>
#include <crtdbg.h>
#include <time.h>
#include <corecrt_io.h>

#include <stdio.h>
#include <internal_shared.h>
#include <locale.h>

#include <sys/stat.h>
#include <sys/timeb.h>
#include <sys/utime.h>  
#include <float.h>

#include <corecrt_math.h>
#include <msvcrt_IAT.h>
#include <corecrt_internal.h>


static __forceinline errno_t __cdecl _tcscpy_s(
	_Out_writes_z_(_SizeInBytes) char*       _Destination,
	_In_                         rsize_t     _SizeInBytes,
	_In_z_                       char const* _Source
	)
{
	return strcpy_s(_Destination, _SizeInBytes, _Source);
}

static __forceinline errno_t __cdecl _tcscpy_s(
	_Out_writes_z_(_SizeInBytes) wchar_t*       _Destination,
	_In_                         rsize_t     _SizeInBytes,
	_In_z_                       wchar_t const* _Source
	)
{
	return wcscpy_s(_Destination, _SizeInBytes, _Source);
}

static __forceinline size_t __cdecl _tcslen(
	_In_z_ char const* _Str
	)
{
	return strlen(_Str);
}

static __forceinline size_t __cdecl _tcslen(
	_In_z_ wchar_t const* _String
	)
{
	return wcslen(_String);
}

extern "C"
{
	struct __crt_stdio_stream_data :public _iobuf
	{
		CRITICAL_SECTION _lock;
	};


	__declspec(dllimport) extern _iobuf _iob[_IOB_ENTRIES];

	FILE* __cdecl __acrt_iob_func_downlevel(unsigned in)
	{
		return &_iob[in];
	}

	_LCRT_DEFINE_IAT_SYMBOL(__acrt_iob_func_downlevel);

	//int __scrt_debugger_hook_flag = 0;

	//void __cdecl _CRT_DEBUGGER_HOOK(int const reserved)
	//{
	//	UNREFERENCED_PARAMETER(reserved);

	//	// We assign zero to the debugger hook flag so that this function is not
	//	// folded when optimized.  The flag is not otherwise used.
	//	__scrt_debugger_hook_flag = 0;
	//}
//#define _CRT_DEBUGGER_HOOK(a) (0)

//	void __cdecl __scrt_fastfail(unsigned const code)
//	{
//		// First see if __fastfail is available, and invoke it if it is.  This will
//		// always be availbale on ARM and is always available on Windows 8 and above.
//		if (IsProcessorFeaturePresent(PF_FASTFAIL_AVAILABLE))
//			__fastfail(code);
//
//		// If __fastfail is not available, fall back to raising an exception that
//		// bypasses all exception handlers (via a call to the unhandled exception
//		// filter.
//
//		// Notify the debugger if attached.
//		//_CRT_DEBUGGER_HOOK(0);
//
//		CONTEXT context_record = {};
//
//#if defined _M_IX86
//
//		__asm
//		{
//			mov dword ptr[context_record.Eax], eax
//			mov dword ptr[context_record.Ecx], ecx
//			mov dword ptr[context_record.Edx], edx
//			mov dword ptr[context_record.Ebx], ebx
//			mov dword ptr[context_record.Esi], esi
//			mov dword ptr[context_record.Edi], edi
//			mov word ptr[context_record.SegSs], ss
//			mov word ptr[context_record.SegCs], cs
//			mov word ptr[context_record.SegDs], ds
//			mov word ptr[context_record.SegEs], es
//			mov word ptr[context_record.SegFs], fs
//			mov word ptr[context_record.SegGs], gs
//			pushfd
//			pop[context_record.EFlags]
//		}
//
//		context_record.ContextFlags = CONTEXT_CONTROL;
//		context_record.Eip = reinterpret_cast<ULONG>(_ReturnAddress());
//		context_record.Esp = reinterpret_cast<ULONG>(_AddressOfReturnAddress());
//		context_record.Ebp = *(reinterpret_cast<ULONG*>(_AddressOfReturnAddress()) - 1);
//
//#elif defined _M_X64
//
//		RtlCaptureContext(&context_record);
//
//		ULONG64 const control_pc = context_record.Rip;
//
//		ULONG64 image_base;
//		PRUNTIME_FUNCTION const function_entry = RtlLookupFunctionEntry(control_pc, &image_base, nullptr);
//
//		if (function_entry)
//		{
//			ULONG64 establisher_frame;
//			PVOID   handler_data;
//			RtlVirtualUnwind(
//				UNW_FLAG_NHANDLER,
//				image_base,
//				control_pc,
//				function_entry,
//				&context_record,
//				&handler_data,
//				&establisher_frame,
//				nullptr);
//		}
//
//		context_record.Rip = reinterpret_cast<ULONGLONG>(_ReturnAddress());
//		context_record.Rsp = reinterpret_cast<ULONGLONG>(_AddressOfReturnAddress()) + 8;
//
//#endif
//
//		EXCEPTION_RECORD exception_record = {};
//		exception_record.ExceptionCode = STATUS_FATAL_APP_EXIT;
//		exception_record.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
//		exception_record.ExceptionAddress = _ReturnAddress();
//
//		bool const was_debugger_present = IsDebuggerPresent() == TRUE;
//
//		// Raise an exception that bypasses all exception handlers:
//		EXCEPTION_POINTERS exception_pointers = { &exception_record, &context_record };
//
//		SetUnhandledExceptionFilter(nullptr);
//		LONG const result = UnhandledExceptionFilter(&exception_pointers);
//
//		// If no handler was found and no debugger was previously attached, then make
//		// sure we notify the debugger.
//		//if (result == EXCEPTION_CONTINUE_SEARCH && !was_debugger_present)
//		//	_CRT_DEBUGGER_HOOK(0);
//	}

	//void __cdecl __std_exception_copy(
	//	__std_exception_data const* const from,
	//	__std_exception_data*       const to
	//)
	//{
	//	_ASSERTE(to->_What == nullptr && to->_DoFree == false);

	//	if (!from->_DoFree || !from->_What)
	//	{
	//		to->_What = from->_What;
	//		to->_DoFree = false;
	//		return;
	//	}

	//	size_t const buffer_count = strlen(from->_What) + 1;

	//	auto buffer=static_cast<char*>(malloc(buffer_count));
	//	if (!buffer)
	//	{
	//		return;
	//	}

	//	strcpy_s(buffer, buffer_count, from->_What);
	//	to->_What = buffer;
	//	to->_DoFree = true;
	//}

	//void __cdecl __std_exception_destroy(
	//	__std_exception_data* const data
	//)
	//{
	//	if (data->_DoFree)
	//	{
	//		free(const_cast<char*>(data->_What));
	//	}

	//	data->_DoFree = false;
	//	data->_What = nullptr;
	//}

	/*void __cdecl _invalid_parameter(
		_In_opt_z_ wchar_t const*,
		_In_opt_z_ wchar_t const*,
		_In_opt_z_ wchar_t const*,
		_In_       unsigned int,
		_In_       uintptr_t
	);

	void __cdecl _invalid_parameter_noinfo_noreturn(void)
	{
		_invalid_parameter(nullptr, nullptr, nullptr, 0, 0);
		_invoke_watson(nullptr, nullptr, nullptr, 0, 0);
	}*/

	errno_t __CRTDECL wmemcpy_s_downlevel(
		_Out_writes_to_opt_(_N1, _N) wchar_t*       _S1,
		_In_                         rsize_t        _N1,
		_In_reads_opt_(_N)           wchar_t const* _S2,
		_In_                         rsize_t        _N
	)
	{
		return memcpy_s(_S1, _N1 * sizeof(wchar_t), _S2, _N * sizeof(wchar_t));
	}

	_LCRT_DEFINE_IAT_SYMBOL(wmemcpy_s_downlevel);

	errno_t __CRTDECL wmemmove_s_downlevel(
			_Out_writes_to_opt_(_N1, _N) wchar_t*       _S1,
			_In_                         rsize_t        _N1,
			_In_reads_opt_(_N)           wchar_t const* _S2,
			_In_                         rsize_t        _N
		)
	{
		return memmove_s(_S1, _N1 * sizeof(wchar_t), _S2, _N * sizeof(wchar_t));
	}

	_LCRT_DEFINE_IAT_SYMBOL(wmemmove_s_downlevel);

	//	int __cdecl __stdio_common_vswprintf(
	//		_In_                                    unsigned __int64 _Options,
	//		_Out_writes_z_(_BufferCount)            wchar_t*         _Buffer,
	//		_In_                                    size_t           _BufferCount,
	//		_In_z_ _Printf_format_string_params_(2) wchar_t const*   _Format,
	//		_In_opt_                                _locale_t        _Locale,
	//		va_list          _ArgList
	//	)
	//	{
	//		return _Buffer == NULL ? _vscwprintf_l(_Format, _Locale, _ArgList) : _vswprintf_l(_Buffer, _BufferCount, _Format, _Locale, _ArgList);
	//	}
	//
	//	int __cdecl __stdio_common_vswprintf_s(
	//		_In_                                    unsigned __int64 _Options,
	//		_Out_writes_z_(_BufferCount)            wchar_t*         _Buffer,
	//		_In_                                    size_t           _BufferCount,
	//		_In_z_ _Printf_format_string_params_(2) wchar_t const*   _Format,
	//		_In_opt_                                _locale_t        _Locale,
	//		va_list          _ArgList
	//	)
	//	{
	//		return _Buffer == NULL ? _vscwprintf_l(_Format, _Locale, _ArgList) : _vswprintf_s_l(_Buffer, _BufferCount, _Format, _Locale, _ArgList);
	//	}

	// Maximum local time adjustment (GMT + 13 Hours, DST -0 Hours)
#define _MAX_LOCAL_TIME (13 * 60 * 60)

	// Minimum local time adjustment (GMT - 11 Hours, DST - 1 Hours)
#define _MIN_LOCAL_TIME (-12 * 60 * 60)

	// Number of seconds from 00:00:00, 01/01/1970 UTC to 23:59:59, 01/18/2038 UTC
#define _MAX_TIME32 0x7fffd27fll

#ifdef _ATL_XP_TARGETING
	extern "C++" template<typename time_t>
	static __forceinline double __cdecl common_difftime(
		_In_ time_t _Time1,
		_In_ time_t _Time2
		)
	{
		if (!(_Time2 >= 0 && _Time1 >= 0))
		{
			errno = EINVAL;
			return 0;
		}
		return static_cast<double>(_Time1 - _Time2);
	}

	double __cdecl _difftime32_downlevel(
		_In_ __time32_t _Time1,
		_In_ __time32_t _Time2
		)
	{
		return common_difftime(_Time1, _Time2);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_difftime32_downlevel);

	double __cdecl _difftime64_downlevel(
		_In_ __time64_t _Time1,
		_In_ __time64_t _Time2
		)
	{
		return common_difftime(_Time1, _Time2);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_difftime64_downlevel);

	struct tm* __cdecl _localtime32_downlevel(
		_In_ __time32_t const* _Time
		)
	{
		_VALIDATE_RETURN(_Time != nullptr, EINVAL,nullptr);

		// Check for illegal time_t value:
		if (*_Time <0 || *_Time > _MAX_TIME32)
		{
			errno = EINVAL;
			return nullptr;
		}
		__time64_t _Time64 = *_Time;

		return _localtime64(&_Time64);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_localtime32_downlevel);

	extern "C++" static __forceinline struct tm* __cdecl _localtime_t(_In_ __time32_t const* _Time)
	{
		return _localtime32(_Time);
	}

	extern "C++" static __forceinline struct tm* __cdecl _localtime_t(_In_ __time64_t const* _Time)
	{
		return _localtime64(_Time);
	}

	extern "C++" template<typename time_t>
	static __forceinline errno_t __cdecl common_localtime_s(
		_Out_ struct tm*        _Tm,
		_In_  time_t const* _Time
		)
	{
		_VALIDATE_RETURN_ERRCODE(_Tm != nullptr, EINVAL);
		memset(_Tm, 0xff, sizeof(*_Tm));

		_VALIDATE_RETURN_ERRCODE(_Time != nullptr, EINVAL);


#pragma warning(suppress : 4996)
		const struct tm* t = _localtime_t(_Time);
		if (!t)
			return errno;

		*_Tm = *t;

		return 0;
	}

	errno_t __cdecl _localtime32_s_downlevel(
		_Out_ struct tm*        _Tm,
		_In_  __time32_t const* _Time
		)
	{
		return common_localtime_s(_Tm, _Time);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_localtime32_s_downlevel);

	errno_t __cdecl _localtime64_s_downlevel(
		_Out_ struct tm*        _Tm,
		_In_  __time64_t const* _Time
		)
	{
		return common_localtime_s(_Tm, _Time);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_localtime64_s_downlevel);

#endif

	#undef _daylight
	__declspec(dllimport) extern int _daylight;

#if !defined _ARM64_ && !defined _ARM_
	#undef _dstbias
	__declspec(dllimport) extern long _dstbias;
#endif

	#undef _timezone
	__declspec(dllimport) extern long _timezone;

	#undef _tzname
	__declspec(dllimport) extern char** _tzname;

	errno_t __cdecl _get_daylight_downlevel(
		_Out_ int* _Daylight
		)
	{
		_VALIDATE_RETURN_ERRCODE(_Daylight != nullptr, EINVAL);

		*_Daylight = _daylight;

		return 0;
	}

	_LCRT_DEFINE_IAT_SYMBOL(_get_daylight_downlevel);

	errno_t __cdecl _get_dstbias_downlevel(
		_Out_ long* _DaylightSavingsBias
		)
	{
		_VALIDATE_RETURN_ERRCODE(_DaylightSavingsBias != nullptr, EINVAL);

		*_DaylightSavingsBias = _dstbias;
		return 0;
	}

	_LCRT_DEFINE_IAT_SYMBOL(_get_dstbias_downlevel);

	errno_t __cdecl _get_timezone_downlevel(
		_Out_ long* _TimeZone
		)
	{
		_VALIDATE_RETURN_ERRCODE(_TimeZone != nullptr, EINVAL);

		*_TimeZone = _timezone;
		return 0;
	}

	_LCRT_DEFINE_IAT_SYMBOL(_get_timezone_downlevel);

	errno_t __cdecl _get_tzname_downlevel(
		_Out_                        size_t* _ReturnValue,
		_Out_writes_z_(_SizeInBytes) char*   _Buffer,
		_In_                         size_t  _SizeInBytes,
		_In_                         int     _Index
		)
	{
		_VALIDATE_RETURN_ERRCODE(
			(_Buffer != nullptr && _SizeInBytes > 0) ||
			(_Buffer == nullptr && _SizeInBytes == 0),
			EINVAL);

		if (_Buffer != nullptr)
			_Buffer[0] = '\0';

		_VALIDATE_RETURN_ERRCODE(_ReturnValue != nullptr, EINVAL);
		_VALIDATE_RETURN_ERRCODE(_Index == 0 || _Index == 1, EINVAL);

		auto& _TmpName = _tzname[_Index];

		// _tzname is correctly inited at startup, so no need to check if
		// CRT init finished.
		*_ReturnValue = strlen(_TmpName) + 1;

		// If the buffer pointer is null, the caller is interested only in the size
		// of the string, not in the actual value of the string:
		if (_Buffer == nullptr)
			return 0;

		if (*_ReturnValue > _SizeInBytes)
			return ERANGE;

		return strcpy_s(_Buffer, _SizeInBytes, _TmpName);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_get_tzname_downlevel);

#if defined _M_AMD64 && defined _ATL_XP_TARGETING

	extern "C" int* __cdecl __daylight_downlevel()
	{
		return &_daylight;
	}
	_LCRT_DEFINE_IAT_SYMBOL(__daylight_downlevel);

	extern "C" long* __cdecl __dstbias_downlevel()
	{
		return &_dstbias;
	}
	_LCRT_DEFINE_IAT_SYMBOL(__dstbias_downlevel);

#endif

#if defined _M_AMD64 || defined _M_ARM64

	extern "C" long* __cdecl __timezone_downlevel()
	{
		return &_timezone;
	}
	_LCRT_DEFINE_IAT_SYMBOL(__timezone_downlevel);

	extern "C" char** __cdecl __tzname_downlevel()
	{
		return _tzname;
	}
	_LCRT_DEFINE_IAT_SYMBOL(__tzname_downlevel);

#endif

#ifdef _ATL_XP_TARGETING
	/*
	
	Windows XP的msvcrt没有这个接口，不过我们可以使用其他接口模拟实现该功能。

	当 _Origin = SEEK_SET时，直接调用 fsetpos

	当 _Origin = SEEK_CUR时，先调用 fgetpos 获取当前文件位置，修正_Offset后再调用 fsetpos

	当 _Origin = SEEK_END时，先触发内部缓冲区写入，然后调用 _lseeki64

	_Origin 其他情况，返回无效参数

	*/
	int __cdecl _fseeki64_downlevel(
		_Inout_ FILE*   _Stream,
		_In_    __int64 _Offset,
		_In_    int     _Origin
		)
	{
		if (_Origin == SEEK_SET)
		{
			//如果是以文件起始为基准，则直接调用fsetpos，减少函数调用次数
			return fsetpos(_Stream, &_Offset);
		}
		else
		{
			int result = -1;

			//对文件进行加锁，防止其他线程篡改
			_lock_file(_Stream);

			fpos_t _Position;

			//获取文件当前位置
			if (fgetpos(_Stream, &_Position) == 0)
			{
				if (_Origin == SEEK_CUR)
				{
					//以当前位置为准，修正偏移到当前位置
					_Offset += _Position;

					result = fsetpos(_Stream, &_Offset);
				}
				else if (_Origin == SEEK_END)
				{
					//调用一次set，触发，内部缓冲区写入硬盘
					if (fsetpos(_Stream, &_Position) == 0)
					{
						result = _lseeki64(_Stream->_file, _Offset, _Origin) == -1 ? -1 : 0;
					}
				}
				else
				{
					//无效参数
					errno = EINVAL;
				}
			}

			//解锁文件，允许其他线程操作此句柄
			_unlock_file(_Stream);

			return result;
		}
	}

	_LCRT_DEFINE_IAT_SYMBOL(_fseeki64_downlevel);

#endif

	//总是返回ture，因为老版本没有此函数，不过以前的 _matherr内部他会判断是否存在
	bool __cdecl __acrt_has_user_matherr()
	{
		return true;
	}

	/*int __CRTDECL _matherr(_Inout_ struct _exception* _Except);

	int __acrt_invoke_user_matherr(_Inout_ struct _exception* _Except)
	{
		return _matherr(_Except);
	}*/

	/*long long __cdecl wcstoll(
		_In_z_                   wchar_t const* _String,
		_Out_opt_ _Deref_post_z_ wchar_t**      _EndPtr,
		_In_                     int            _Radix
	)
	{
		return _wcstoi64(_String, _EndPtr, _Radix);
	}*/

	/*long long __cdecl strtoll(
		_In_z_                   char const* _String,
		_Out_opt_ _Deref_post_z_ char**      _EndPtr,
		_In_                     int         _Radix
	)
	{
		return _strtoi64(_String, _EndPtr, _Radix);
	}*/

	/*unsigned long long __cdecl strtoull(
		_In_z_                   char const* _String,
		_Out_opt_ _Deref_post_z_ char**      _EndPtr,
		_In_                     int         _Radix
	)
	{
		return _strtoui64(_String, _EndPtr, _Radix);
	}*/


#ifdef _ATL_XP_TARGETING
#include <Ntsecapi.h>
	errno_t __cdecl rand_s_downlevel(_Out_ unsigned int* _RandomValue)
	{
		*_RandomValue = 0;

		if (!RtlGenRandom(_RandomValue, sizeof(*_RandomValue)))
		{
			errno = ENOMEM;
			return ENOMEM;
		}
		else
		{
			return 0;
		}
	}

	_LCRT_DEFINE_IAT_SYMBOL(rand_s_downlevel);

#endif

//#ifdef _X86_
//#include <math.h>

//#pragma function(acos)
//	double __cdecl _acos_default(_In_ double _X)
//	{
//		return acos(_X);
//	}
//#pragma intrinsic(acos)

//#pragma function(asin)
//	double __cdecl _asin_default(_In_ double _X)
//	{
//		return asin(_X);
//
//	}
//#pragma intrinsic(asin)

//#pragma function(atan)
//	double __cdecl _atan_default(_In_ double _X)
//	{
//		return atan(_X);
//
//	}
//#pragma intrinsic(atan)

//#pragma function(cos)
//	double __cdecl _cos_default(_In_ double _X)
//	{
//		return cos(_X);
//
//	}
//#pragma intrinsic(cos)

//#pragma function(exp)
//	double __cdecl _exp_default(_In_ double _X)
//	{
//		return exp(_X);
//	}
//#pragma intrinsic(exp)

//#pragma function(pow)
//	double __cdecl _pow_default(_In_ double _X, _In_ double _Y)
//	{
//		return pow(_X, _Y);
//	}
//#pragma intrinsic(pow)

//#pragma function(sin)
//	double __cdecl _sin_default(_In_ double _X)
//	{
//		return sin(_X);
//	}
//#pragma intrinsic(sin)

//#pragma function(tan)
//	double __cdecl _tan_default(_In_ double _X)
//	{
//		return tan(_X);
//	}
//#pragma intrinsic(tan)

//#pragma function(log)
//	double __cdecl _log_default(_In_ double _X)
//	{
//		return log(_X);
//	}
//#pragma intrinsic(log)

//#pragma function(log10)
//	double __cdecl _log10_default(_In_ double _X)
//	{
//		return log10(_X);
//	}
//#pragma intrinsic(log10)

//#endif

	__declspec(dllimport) void __cdecl _lock(
		int locknum
	);
	__declspec(dllimport) void __cdecl _unlock(
		int locknum
	);


	static __inline bool IsInternalStream(_iobuf* const stream)
	{
		return ((byte*)stream >= (byte*)_iob) && ((byte*)stream <= (byte*)(_iob + _IOB_ENTRIES -1));
	}

	// Locks a stdio stream.
	void __cdecl _lock_file_downlevel(FILE* const stream)
	{
		if (IsInternalStream(stream))
			_lock((stream - _iob) + 0x10);
		else
			EnterCriticalSection(&((__crt_stdio_stream_data*)stream)->_lock);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_lock_file_downlevel);


	// Unlocks a stdio stream.
	void __cdecl _unlock_file_downlevel(FILE* const stream)
	{
		if (IsInternalStream(stream))
			_unlock((stream - _iob) + 0x10);
		else
			LeaveCriticalSection(&((__crt_stdio_stream_data*)stream)->_lock);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_unlock_file_downlevel);

	errno_t __cdecl _get_stream_buffer_pointers_downlevel(
		FILE*   const public_stream,
		char*** const base,
		char*** const ptr,
		int**   const count
	)
	{
		_VALIDATE_RETURN_ERRCODE(public_stream != nullptr, EINVAL);


		if (base)
		{
			*base = &(public_stream->_base);
		}

		if (ptr)
		{
			*ptr = &(public_stream->_ptr);
		}

		if (count)
		{
			*count = &(public_stream->_cnt);
		}

		return 0;
	}

	_LCRT_DEFINE_IAT_SYMBOL(_get_stream_buffer_pointers_downlevel);

#if _CRT_NTDDI_MIN < NTDDI_WINBLUE
	//msvrct仅支持_Strftime，我们可以将通过字符串转换，得到_Wcsftime
	size_t __cdecl _Wcsftime_downlevel(
		wchar_t*       const buffer,
		size_t         const max_size,
		wchar_t const* const format,
		tm const*      const timeptr,
		void*          const lc_time_arg
	)
	{
		_VALIDATE_RETURN(buffer != nullptr, EINVAL, 0)
			_VALIDATE_RETURN(max_size != 0, EINVAL, 0)
			*buffer = '\0';

		_VALIDATE_RETURN(format != nullptr, EINVAL, 0)
			_VALIDATE_RETURN(timeptr != nullptr, EINVAL, 0)

			size_t Count = 0;

		unsigned int const lc_time_cp = ___lc_codepage_func();

		auto ch_format = WideCharToMultiByte(lc_time_cp, 0, format, -1, 0,0, nullptr, nullptr);
		if (ch_format==0)
		{
			return 0;
		}

		auto formatA = (char*)malloc(ch_format);
		//开辟2倍缓冲区
		auto BufferA = (char*)malloc(max_size * 2);

		if (formatA==nullptr|| BufferA==nullptr)
		{
			//内存不足
			goto __Error;
		}

		if (WideCharToMultiByte(lc_time_cp, 0,format, -1, formatA, ch_format, nullptr, nullptr))
		{
			//转换失败
			goto __Error;
		}


		Count = _Strftime(BufferA, max_size * 2, formatA, timeptr, lc_time_arg);

		if (Count)
		{
			//
			Count = MultiByteToWideChar(lc_time_cp, 0, BufferA, Count, buffer, max_size);

			//改函数长度并不包含null，因为长度减一
			if (Count)
				--Count;
		}



	__Error:
		if (formatA)
			free(formatA);
		if (BufferA)
			free(BufferA);
		return Count;
	}

	_LCRT_DEFINE_IAT_SYMBOL(_Wcsftime_downlevel);

#endif

#if _CRT_NTDDI_MIN < NTDDI_WINBLUE
	//msvrct仅支持_Getdays，我们可以将通过字符串转换，得到_W_Getdays
	wchar_t* __cdecl _W_Getdays_downlevel(void)
	{
		auto szDays = _Getdays();
		if (!szDays)
			return nullptr;

		unsigned int const lc_time_cp = ___lc_codepage_func();

		auto ch_Days = MultiByteToWideChar(lc_time_cp, 0, szDays, -1, 0, 0);
		if (ch_Days==0)
		{
			return nullptr;
		}

		//内存申请失败
		auto szDaysW = (wchar_t*)malloc(ch_Days * sizeof(wchar_t));
		if (!szDaysW)
			return nullptr;

		ch_Days = MultiByteToWideChar(lc_time_cp, 0, szDays, -1, szDaysW, ch_Days);

		if (ch_Days)
		{
			return szDaysW;
		}
		else
		{
			free(szDaysW);
			return nullptr;
		}
	}

	_LCRT_DEFINE_IAT_SYMBOL(_W_Getdays_downlevel);

#endif

#if _CRT_NTDDI_MIN < NTDDI_WINBLUE
	//msvrct仅支持_Getmonths，我们可以将通过字符串转换，得到_W_Getmonths
	wchar_t *__cdecl _W_Getmonths_downlevel(void)
	{
		auto szMonths = _Getmonths();
		if (!szMonths)
			return nullptr;

		unsigned int const lc_time_cp = ___lc_codepage_func();

		auto ch_Months = MultiByteToWideChar(lc_time_cp, 0, szMonths, -1, 0, 0);
		if (ch_Months == 0)
		{
			return nullptr;
		}

		//内存申请失败
		auto szMonthsW = (wchar_t*)malloc(ch_Months * sizeof(wchar_t));
		if (!szMonthsW)
			return nullptr;

		ch_Months = MultiByteToWideChar(lc_time_cp, 0, szMonths, -1, szMonthsW, ch_Months);

		if (ch_Months)
		{
			return szMonthsW;
		}
		else
		{
			free(szMonthsW);
			return nullptr;
		}
	}

	_LCRT_DEFINE_IAT_SYMBOL(_W_Getmonths_downlevel);

#endif

	/*void* __cdecl _W_Gettnames()
	{
		return _Gettnames();
	}*/
	
	//通过文件句柄获取_stat64
	extern "C++" static __forceinline int __cdecl _tstat64(
		_In_z_ int     _FileHandle,
		_Out_  struct _stat64* _Stat
	)
	{
		return _fstat64(_FileHandle, _Stat);
	}

	//通过ASCII路径获取_stat64
	extern "C++" static __forceinline int __cdecl _tstat64(
		_In_z_ char const*     _FileName,
		_Out_  struct _stat64* _Stat
	)
	{
		return _stat64(_FileName, _Stat);
	}

	//通过Unicode路径获取_stat64
	extern "C++" static __forceinline int __cdecl _tstat64(
		_In_z_ wchar_t const*     _FileName,
		_Out_  struct _stat64* _Stat
	)
	{
		return _wstat64(_FileName, _Stat);
	}


	extern "C++" template<class File, class _statT >
	static __forceinline int __cdecl common_stat(
		_In_z_ File       _FileName,
		_Out_  _statT* _Stat
	)
	{
		_VALIDATE_CLEAR_OSSERR_RETURN(_Stat != nullptr, EINVAL, -1);

		struct _stat64 _StatTmp;
		auto result = _tstat64(_FileName, &_StatTmp);
		if (result == 0)
		{
			//获取成功，开始转换数据
			_Stat->st_dev = _StatTmp.st_dev;
			_Stat->st_ino = _StatTmp.st_ino;
			_Stat->st_mode = _StatTmp.st_mode;
			_Stat->st_nlink = _StatTmp.st_nlink;
			_Stat->st_uid = _StatTmp.st_uid;
			_Stat->st_gid = _StatTmp.st_gid;
			_Stat->st_rdev = _StatTmp.st_rdev;
			_Stat->st_size = _StatTmp.st_size;
			_Stat->st_atime = _StatTmp.st_atime;
			_Stat->st_mtime = _StatTmp.st_mtime;
			_Stat->st_ctime = _StatTmp.st_ctime;
		}

		return result;
	}
	
	//_fstat已经改名为_fstat32
//#pragma push_macro("_fstat")
//#undef _fstat
//	__declspec(dllimport) int __cdecl _fstat(
//		_In_  int             _FileHandle,
//		_Out_ struct _stat32* _Stat
//	);
//
//	int __cdecl _fstat32(
//		_In_  int             _FileHandle,
//		_Out_ struct _stat32* _Stat
//	)
//	{
//		return _fstat(_FileHandle, _Stat);
//	}
//#pragma pop_macro("_fstat")

	//_fstati64已经改名为_fstat32i64
//#pragma push_macro("_fstati64")
//#undef _fstati64
//	__declspec(dllimport) int __cdecl _fstati64(
//		_In_  int                _FileHandle,
//		_Out_ struct _stat32i64* _Stat
//	);
//
//	int __cdecl _fstat32i64(
//		_In_  int                _FileHandle,
//		_Out_ struct _stat32i64* _Stat
//	)
//	{
//		return _fstati64(_FileHandle, _Stat);
//	}
//#pragma pop_macro("_fstati64")


	//msvcrt不支持_fstat64i32，不过我们可以用_fstat64转换
	int __cdecl _fstat64i32_downlevel(
		_In_  int                _FileHandle,
		_Out_ struct _stat64i32* _Stat
	)
	{
		return common_stat(_FileHandle, _Stat);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_fstat64i32_downlevel);

	//_stat已经改名为_stat32
//#pragma push_macro("_stat")
//#undef _stat
//	__declspec(dllimport) int __cdecl _stat(
//		_In_z_ char const*     _FileName,
//		_Out_  struct _stat32* _Stat
//	);
//
//	int __cdecl _stat32(
//		_In_z_ char const*     _FileName,
//		_Out_  struct _stat32* _Stat
//	)
//	{
//		return _stat(_FileName, _Stat);
//	}
//#pragma pop_macro("_stat")


	//_stati64已经改名为_stat32i64
//#pragma push_macro("_stati64")
//#undef _stati64
//	__declspec(dllimport) int __cdecl _stati64(
//		_In_z_ char const*     _FileName,
//		_Out_  struct _stat32i64* _Stat
//	);
//
//	int __cdecl _stat32i64(
//		_In_z_ char const*        _FileName,
//		_Out_  struct _stat32i64* _Stat
//	)
//	{
//		return _stati64(_FileName, _Stat);
//	}
//#pragma pop_macro("_stati64")


	//msvcrt不支持_stat64i32，不过我们可以用_stat64转换
	int __cdecl _stat64i32_light(
		_In_z_ char const*        _FileName,
		_Out_  struct _stat64i32* _Stat
	)
	{
		return common_stat(_FileName, _Stat);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_stat64i32_light);

	//_wstat已经改名为_wstat32，做转发
//#pragma push_macro("_wstat")
//#undef _wstat
//	__declspec(dllimport) int __cdecl _wstat(
//		_In_z_ wchar_t const*  _FileName,
//		_Out_  struct _stat32* _Stat
//	);
//
//	int __cdecl _wstat32(
//		_In_z_ wchar_t const*  _FileName,
//		_Out_  struct _stat32* _Stat
//	)
//	{
//		return _wstat(_FileName, _Stat);
//	}
//#pragma pop_macro("_wstat")

	//_wstati64已经改名为_wstat32i64，做转发
//#pragma push_macro("_wstati64")
//#undef _wstati64
//	__declspec(dllimport) int __cdecl _wstati64(
//		_In_z_ wchar_t const*     _FileName,
//		_Out_  struct _stat32i64* _Stat
//	);
//
//	int __cdecl _wstat32i64(
//		_In_z_ wchar_t const*     _FileName,
//		_Out_  struct _stat32i64* _Stat
//	)
//	{
//		return _wstati64(_FileName, _Stat);
//	}
//#pragma pop_macro("_wstati64")

	//msvcrt不支持_wstat64i32，不过我们可以用_wstat64转换
	int __cdecl _wstat64i32_light(
		_In_z_ wchar_t const*     _FileName,
		_Out_  struct _stat64i32* _Stat
	)
	{
		return common_stat(_FileName, _Stat);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_wstat64i32_light);

	//通过ASCII文件名搜索
	extern "C++" __forceinline intptr_t __cdecl _tfindfirst(
		_In_z_ char const*            _FileName,
		_Out_  struct __finddata64_t* _FindData
	)
	{
		return _findfirst64(_FileName, _FindData);
	}

	//通过Unicode路径搜索
	extern "C++" __forceinline intptr_t __cdecl _tfindfirst(
		_In_z_ wchar_t const*         _FileName,
		_Out_  struct _wfinddata64_t* _FindData
	)
	{
		return _wfindfirst64(_FileName, _FindData);
	}

	extern "C++" template<class __finddata,class File,class Type>
	__forceinline intptr_t __cdecl common_findfirst(
		_In_z_ File              _FileName,
		_Out_  Type* _FindData
	)
	{
		_VALIDATE_CLEAR_OSSERR_RETURN(_FileName != nullptr && _FindData != nullptr, EINVAL, -1);

		__finddata finddataT;
		auto _FindHandle = _tfindfirst(_FileName, &finddataT);
		if (_FindHandle != -1)
		{
			_FindData->attrib = finddataT.attrib;
			_FindData->time_create = finddataT.time_create;
			_FindData->time_access = finddataT.time_access;
			_FindData->time_write = finddataT.time_write;
			_FindData->size = finddataT.size;

			static_assert(sizeof(_FindData->name) == sizeof(finddataT.name),"名称缓冲区必须完全一致！");

			memcpy(_FindData->name, finddataT.name, sizeof(finddataT.name));
		}

		return _FindHandle;
	}

	//msvcrt不支持_findfirst64i32，不过我们可以用_findfirst64转换
	intptr_t __cdecl _findfirst64i32_downlevel(
		_In_z_ char const*              _FileName,
		_Out_  struct _finddata64i32_t* _FindData
	)
	{
		return common_findfirst<__finddata64_t>(_FileName, _FindData);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_findfirst64i32_downlevel);


	//msvcrt不支持_wfindfirst64i32，不过我们可以用_wfindfirst64转换
	intptr_t __cdecl _wfindfirst64i32_downlevel(
		_In_z_ wchar_t const*            _FileName,
		_Out_  struct _wfinddata64i32_t* _FindData
	)
	{
		return common_findfirst<_wfinddata64_t>(_FileName, _FindData);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_wfindfirst64i32_downlevel);


	extern "C++" __forceinline int __cdecl _tfindnext(
		_In_  intptr_t                 _FindHandle,
		_Out_ struct __finddata64_t* _FindData
	)
	{
		return _findnext64(_FindHandle, _FindData);
	}

	extern "C++" __forceinline int __cdecl _tfindnext(
		_In_  intptr_t                  _FindHandle,
		_Out_ struct _wfinddata64_t* _FindData
	)
	{
		return _wfindnext64(_FindHandle, _FindData);
	}

	extern "C++" template<class __finddata, class Type>
	__forceinline intptr_t __cdecl common_findnext(
			_In_  intptr_t                 _FindHandle,
			_Out_  Type* _FindData
		)
	{
		_VALIDATE_CLEAR_OSSERR_RETURN(_FindHandle != -1 && _FindData != nullptr, EINVAL, -1);

		__finddata finddataT;
		auto result = _tfindnext(_FindHandle, &finddataT);
		if (result == 0)
		{
			_FindData->attrib = finddataT.attrib;
			_FindData->time_create = finddataT.time_create;
			_FindData->time_access = finddataT.time_access;
			_FindData->time_write = finddataT.time_write;
			_FindData->size = finddataT.size;

			static_assert(sizeof(_FindData->name) == sizeof(finddataT.name), "名称缓冲区必须完全一致！");

			memcpy(_FindData->name, finddataT.name, sizeof(finddataT.name));
		}

		return result;
	}

	int __cdecl _findnext64i32_downlevel(
		_In_  intptr_t                 _FindHandle,
		_Out_ struct _finddata64i32_t* _FindData
	)
	{
		return common_findnext<__finddata64_t>(_FindHandle, _FindData);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_findnext64i32_downlevel);

	int __cdecl _wfindnext64i32_downlevel(
		_In_  intptr_t                  _FindHandle,
		_Out_ struct _wfinddata64i32_t* _FindData
	)
	{
		return common_findnext<_wfinddata64_t>(_FindHandle, _FindData);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_wfindnext64i32_downlevel);

	//msvcrt没有_ftelli64，不过好在有fgetpos
	__int64 __cdecl _ftelli64_downlevel(
		_Inout_ FILE* _Stream
	)
	{
		fpos_t _Position;

		if (fgetpos(_Stream, &_Position)!=0)
		{
			return -1;
		}
		else
		{
			return _Position;
		}
	}

	_LCRT_DEFINE_IAT_SYMBOL(_ftelli64_downlevel);

	//	typedef struct {
	//		long osfhnd;    /* underlying OS file HANDLE */ //CreateFile返回的句柄
	//		char osfile;    /* attributes of file (e.g., open in text mode?) */
	//		char pipech;    /* one char buffer for handles opened on pipes */
	//#ifdef _MT
	//		int lockinitflag;
	//		CRITICAL_SECTION lock;
	//#endif  /* _MT */
	//	}   ioinfo;
	//
	//	__declspec(dllimport) ioinfo* __pioinfo[];

	extern "C++" __forceinline double __cdecl _tatof(char const* const string)
	{
		return atof(string);
	}

	extern "C++" template<class FloatingType,class TCHAR>
	__inline int __cdecl common_atof(FloatingType& _Result,const TCHAR* _String)
	{

		_Result = _tatof(_String);

		switch (fpclassify(_Result))
		{
		case FP_SUBNORMAL:
			//如果为太小，则设置为下溢
			return _UNDERFLOW;
			break;
		case FP_INFINITE:
			//如果为无穷，则设置为上溢
			return _OVERFLOW;
			break;
		default:
			//其他情况设置为0
			return 0;
			break;
		}
	}


	//msvcrt没有_atoflt，但是我们可以使用atof转换
	int __cdecl _atoflt_downlevel(_Out_ _CRT_FLOAT*  _Result, _In_z_ char const* _String)
	{
		_VALIDATE_RETURN(_Result != nullptr, EINVAL, _DOMAIN);

		return common_atof(_Result->f, _String);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_atoflt_downlevel);

#ifdef _DEBUG
	#define _BUFFER_FILL_PATTERN _SECURECRT_FILL_BUFFER_PATTERN
#else
	#define _BUFFER_FILL_PATTERN 0
#endif

	/*
	msvcrt并没有导出fread_s，只能靠fread模拟行为

	fread_s，主要提供了二大功能
	1：操作失败时，将_Buffer所有内容设置为 _BUFFER_FILL_PATTERN
	2：读取文件时提供_BufferSize检查，当_Buffer不足以容纳目标时，则返回 ERANGE

	根据文档https://msdn.microsoft.com/zh-cn/library/hh977173.aspx 以及UCRT源代码确定，fread_s失败时文件位置是未知的。
	因此我们没有必要事先保存文件指针位置，来提供恢复机制。
	*/
    size_t __cdecl fread_s_downlevel(
        _Out_writes_bytes_to_(_BufferSize, _ElementSize * _ElementCount)   void*  _Buffer,
        _In_range_(>=, _ElementSize * _ElementCount)                       size_t _BufferSize,
        _In_                                                               size_t _ElementSize,
        _In_                                                               size_t _ElementCount,
        _Inout_                                                            FILE*  _Stream
        )
	{
		if (_BufferSize == SIZE_MAX)
		{
			//_BufferSize为 SIZE_MAX时，走老逻辑
			return fread(_Buffer, _ElementSize, _ElementCount, _Stream);
		}

		if (_ElementSize == 0 || _ElementCount == 0)
			return 0;

		//缓冲区初始化
		memset(_Buffer, _BUFFER_FILL_PATTERN, _BufferSize);


		const auto Total_bytes = _ElementSize*_ElementCount;

		const auto Need_Read_bytes = min(Total_bytes, _BufferSize);

		errno = 0;

		size_t num_read = fread(_Buffer, 1, Need_Read_bytes, _Stream);

		if (num_read == 0)
		{
			if (errno)
			{
				memset(_Buffer, _BUFFER_FILL_PATTERN, _BufferSize);
			}

			return 0;
		}
		else if(num_read==Need_Read_bytes && Total_bytes != Need_Read_bytes)
		{
			//可能有更多数据可用测试下是否还有更多数据可以读取
			errno = 0;

			byte Tmp;

			if (fread(&Tmp, 1, 1, _Stream))
			{
				//还有更多数据
				memset(_Buffer, _BUFFER_FILL_PATTERN, _BufferSize);

				_VALIDATE_RETURN(("buffer too small", 0), ERANGE, 0)
			}
			else if (errno)
			{
				//遇到了错误
				memset(_Buffer, _BUFFER_FILL_PATTERN, _BufferSize);
				return 0;
			}
		}

		return num_read / _ElementSize;
	}

	_LCRT_DEFINE_IAT_SYMBOL(fread_s_downlevel);

	extern "C++" __forceinline char* __cdecl _tgetenv(
		_In_z_ char const* _VarName
		)
	{
		return getenv(_VarName);
	}

	extern "C++" __forceinline wchar_t* __cdecl _tgetenv(
		_In_z_ wchar_t const* _VarName
		)
	{
		return _wgetenv(_VarName);
	}

	extern "C++" template<class TCHAR>
	__forceinline errno_t __cdecl common_tdupenv_s_nolock(
		_Outptr_result_buffer_maybenull_(*_BufferCount) _Outptr_result_maybenull_z_ TCHAR**      _Buffer,
		_Out_opt_                                                                   size_t*        _BufferCount,
		_In_z_                                                                      TCHAR const* _VarName
		)
	{
		_VALIDATE_RETURN_ERRCODE(_Buffer != nullptr, EINVAL);
		*_Buffer = nullptr;

		if (_BufferCount != nullptr)
			*_BufferCount = 0;

		_VALIDATE_RETURN_ERRCODE(_VarName != nullptr, EINVAL);

		const auto value = _tgetenv(_VarName);
		if (value == nullptr)
			return 0;

		size_t const value_count = _tcslen(value) + 1;

		auto pBuffer = (TCHAR*)malloc(value_count * sizeof(TCHAR));

		_VALIDATE_RETURN_ERRCODE_NOEXC(pBuffer != nullptr, ENOMEM);


		memcpy(pBuffer, value, value_count * sizeof(TCHAR));

		*_Buffer = pBuffer;

		if (_BufferCount)
			*_BufferCount = value_count;

		return 0;
	}

	extern "C++" template<class TCHAR>
	__forceinline errno_t __cdecl common_tdupenv_s(
		_Outptr_result_buffer_maybenull_(*_BufferCount) _Outptr_result_maybenull_z_ TCHAR**      _Buffer,
		_Out_opt_                                                                   size_t*        _BufferCount,
		_In_z_                                                                      TCHAR const* _VarName
		)
	{
		_lock(/*_ENV_LOCK*/7);

		auto result = common_tdupenv_s_nolock(_Buffer, _BufferCount, _VarName);

		_unlock(/*_ENV_LOCK*/7);

		return result;
	}

	errno_t __cdecl _dupenv_s_downlevel(
		_Outptr_result_buffer_maybenull_(*_BufferCount) _Outptr_result_maybenull_z_ char**      _Buffer,
		_Out_opt_                                                                   size_t*     _BufferCount,
		_In_z_                                                                      char const* _VarName
		)
	{
		return common_tdupenv_s(_Buffer, _BufferCount, _VarName);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_dupenv_s_downlevel);

	errno_t __cdecl _wdupenv_s_downlevel(
		_Outptr_result_buffer_maybenull_(*_BufferCount) _Outptr_result_maybenull_z_ wchar_t**      _Buffer,
		_Out_opt_                                                                   size_t*        _BufferCount,
		_In_z_                                                                      wchar_t const* _VarName
		)
	{
		return common_tdupenv_s(_Buffer, _BufferCount, _VarName);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_wdupenv_s_downlevel);

	//不想额外写类，通过参数强制转换，规避下吧
	extern "C++"
	__forceinline int _tgetc(char* in)
	{
		return getc((FILE*)in);
	}

	extern "C++"
	__forceinline wint_t _tgetc(wchar_t* in)
	{
		return getwc((FILE*)in);
	}

	extern "C++" template<class TCHAR>
	__forceinline TCHAR* __cdecl common_gets_s_nolock(
		_Out_writes_z_(_Size) TCHAR*   _Buffer,
		_In_                  rsize_t _Size
		)
	{
		_VALIDATE_RETURN(_Buffer != nullptr, EINVAL, nullptr);
		_VALIDATE_RETURN(_Size > 0, EINVAL, nullptr);

		auto c = _tgetc((TCHAR*)_iob);
		if (c == EOF)
		{
			return nullptr;
		}

		auto pBuffer = _Buffer;

		if (_Size == -1)
		{
			for (; c != '\n'&&c != EOF; c = _tgetc((TCHAR*)_iob))
			{
				*pBuffer++ = c;
			}

			*pBuffer = '\0';

		}
		else
		{
			for (; c != '\n'&&c != EOF; c = _tgetc((TCHAR*)_iob))
			{
				if (_Size)
				{
					--_Size;
					*pBuffer++ = c;
				}
			}

			// If we ran out of space, clear the buffer and return failure:
			if (_Size == 0)
			{
				*_Buffer =NULL;

				_VALIDATE_RETURN((L"Buffer is too small" && 0), ERANGE, nullptr);
			}

			*pBuffer = '\0';

		}

		return pBuffer;
	}

	char* __cdecl gets_s_downlevel(
		_Out_writes_z_(_Size) char*   _Buffer,
		_In_                  rsize_t _Size
		)
	{
		_lock(0x10);

		auto result = common_gets_s_nolock(_Buffer, _Size);

		_unlock(0x10);
		return result;
	}

	_LCRT_DEFINE_IAT_SYMBOL(gets_s_downlevel);

	wchar_t* __cdecl _getws_s_downlevel(
		_Out_writes_z_(_BufferCount) wchar_t* _Buffer,
		_In_                         size_t   _BufferCount
		)
	{
		_lock(0x10);

		auto result = common_gets_s_nolock(_Buffer, _BufferCount);

		_unlock(0x10);
		return result;
	}

	_LCRT_DEFINE_IAT_SYMBOL(_getws_s_downlevel);

	//此函数什么都不做，仅供编译通过处理，因此任何调用abort函数必定静默退出。
	unsigned int __cdecl _set_abort_behavior_downlevel(
		_In_ unsigned int _Flags,
		_In_ unsigned int _Mask
		)
	{
		return 0;
	}

	_LCRT_DEFINE_IAT_SYMBOL(_set_abort_behavior_downlevel);

#ifdef _ATL_XP_TARGETING

	errno_t __cdecl _waccess_s_downlevel(
		_In_z_ wchar_t const* _FileName,
		_In_   int            _AccessMode
		)
	{
		return _waccess(_FileName, _AccessMode) == 0 ? 0 : errno;
	}

	_LCRT_DEFINE_IAT_SYMBOL(_waccess_s_downlevel);

	errno_t __cdecl _access_s_downlevel(
        _In_z_ char const* _FileName,
        _In_   int         _AccessMode
        )
	{
		return _access(_FileName, _AccessMode) == 0 ? 0 : errno;
	}

	_LCRT_DEFINE_IAT_SYMBOL(_access_s_downlevel);

	errno_t __cdecl _wasctime_s_downlevel(
		_Out_writes_z_(_SizeInWords) _Post_readable_size_(26) wchar_t*         _Buffer,
		_In_range_(>=,26)                                     size_t           _SizeInWords,
		_In_                                                  struct tm const* _Tm
		)
	{
		_VALIDATE_RETURN_ERRCODE(
			_Buffer != nullptr && _SizeInWords > 0,
			EINVAL
		)

			*_Buffer = NULL;

		_VALIDATE_RETURN_ERRCODE(_SizeInWords >= 26, EINVAL)

		auto szTime = _wasctime(_Tm);
		if (!szTime)
			return errno;

		return wcscpy_s(_Buffer, _SizeInWords, szTime);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_wasctime_s_downlevel);

	errno_t __cdecl asctime_s_downlevel(
        _Out_writes_(_SizeInBytes) _Post_readable_size_(26) char*            _Buffer,
        _In_range_(>=,26)                                   size_t           _SizeInBytes,
        _In_                                                struct tm const* _Tm
        )
	{
		_VALIDATE_RETURN_ERRCODE(
			_Buffer != nullptr && _SizeInBytes > 0,
			EINVAL
		)

		*_Buffer = NULL;

		_VALIDATE_RETURN_ERRCODE(_SizeInBytes >= 26, EINVAL)

		auto szTime = asctime(_Tm);
		if (!szTime)
			return errno;

		return strcpy_s(_Buffer, _SizeInBytes, szTime);
	}

	_LCRT_DEFINE_IAT_SYMBOL(asctime_s_downlevel);

	char* __cdecl _ctime32_downlevel(
		_In_ __time32_t const* _Time
		)
	{
		_VALIDATE_RETURN(_Time != nullptr, EINVAL, nullptr);

		// Check for illegal time_t value:
		if (*_Time <0 || *_Time > _MAX_TIME32)
		{
			errno = EINVAL;
			return nullptr;
		}
		__time64_t _Time64 = *_Time;

		return _ctime64(&_Time64);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_ctime32_downlevel);

	wchar_t* __cdecl _wctime32_downlevel(
		_In_ __time32_t const* _Time
		)
	{
		_VALIDATE_RETURN(_Time != nullptr, EINVAL, nullptr);

		// Check for illegal time_t value:
		if (*_Time <0 || *_Time > _MAX_TIME32)
		{
			errno = EINVAL;
			return nullptr;
		}
		__time64_t _Time64 = *_Time;

		return _wctime64(&_Time64);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_wctime32_downlevel);

	extern "C++" __forceinline void __cdecl _ctime_t(
		_In_ __time32_t const* _Time,
		char* & szTime
		)
	{
		szTime= _ctime32(_Time);
	}

	extern "C++" __forceinline void __cdecl _ctime_t(
		_In_ __time64_t const* _Time,
		char* & szTime
		)
	{
		szTime= _ctime64(_Time);
	}

	extern "C++" __forceinline void __cdecl _ctime_t(
		_In_ __time32_t const* _Time,
		wchar_t* & szTime
		)
	{
		szTime = _wctime32(_Time);
	}

	extern "C++" __forceinline void __cdecl _ctime_t(
		_In_ __time64_t const* _Time,
		wchar_t* & szTime
		)
	{
		szTime = _wctime64(_Time);
	}

	extern "C++" template<typename time_t,typename TCHAR>
	__forceinline errno_t __cdecl common_ctime_s(
		_Out_writes_(_SizeInBytes) _Post_readable_size_(26) TCHAR*             _Buffer,
		_In_range_(>= , 26)                                   size_t            _Size,
		_In_                                                time_t const* _Time
		)
	{
		_VALIDATE_RETURN_ERRCODE(
			_Buffer != nullptr && _Size > 0,
			EINVAL
		);

		*_Buffer = NULL;

		_VALIDATE_RETURN_ERRCODE(_Size >= 26, EINVAL);

		TCHAR* szTime;

		_ctime_t(_Time, szTime);
		if (!szTime)
			return errno;

		return _tcscpy_s(_Buffer, _Size, szTime);
	}

	errno_t __cdecl _ctime32_s_downlevel(
		_Out_writes_(_SizeInBytes) _Post_readable_size_(26) char*             _Buffer,
		_In_range_(>= , 26)                                   size_t            _SizeInBytes,
		_In_                                                __time32_t const* _Time
		)
	{
		return common_ctime_s(_Buffer, _SizeInBytes, _Time);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_ctime32_s_downlevel);

	errno_t __cdecl _ctime64_s_downlevel(
		_Out_writes_z_(_SizeInBytes) _Post_readable_size_(26) char*             _Buffer,
		_In_range_(>= , 26)                                     size_t            _SizeInBytes,
		_In_                                                  __time64_t const* _Time
		)
	{
		return common_ctime_s(_Buffer, _SizeInBytes, _Time);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_ctime64_s_downlevel);

	errno_t __cdecl _wctime32_s_downlevel(
		_Out_writes_z_(_SizeInWords) _Post_readable_size_(26) wchar_t*          _Buffer,
		_In_  _In_range_(>= , 26)                              size_t            _SizeInWords,
		_In_                                                  __time32_t const* _Time
		)
	{
		return common_ctime_s(_Buffer, _SizeInWords, _Time);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_wctime32_s_downlevel);

	errno_t __cdecl _wctime64_s_downlevel(
		_Out_writes_z_(_SizeInWords) _Post_readable_size_(26) wchar_t*          _Buffer,
		_In_  _In_range_(>= , 26)                              size_t            _SizeInWords,
		_In_                                                  __time64_t const* _Time
		)
	{
		return common_ctime_s(_Buffer, _SizeInWords, _Time);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_wctime64_s_downlevel);

	struct tm* __cdecl _gmtime32_downlevel(
		_In_ __time32_t const* _Time
		)
	{
		_VALIDATE_RETURN(_Time != nullptr, EINVAL,nullptr);

		// Check for illegal time_t value:
		if (*_Time < _MIN_LOCAL_TIME || *_Time > _MAX_TIME32 + _MAX_LOCAL_TIME)
		{
			errno = EINVAL;
			return nullptr;
		}

		__time64_t _Time64 = *_Time;

		return _gmtime64(&_Time64);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_gmtime32_downlevel);

	extern "C++" __forceinline struct tm* __cdecl _gmtime_t(
		_In_  __time32_t const* _Time
		)
	{
		return _gmtime32(_Time);
	}

	extern "C++" __forceinline  struct tm* __cdecl _gmtime_t(
		_In_  __time64_t const* _Time
		)
	{
		return _gmtime64(_Time);
	}

	extern "C++" template<typename time_t>
	__forceinline errno_t __cdecl common_gmtime_t_s(
		_Out_ struct tm*        _Tm,
		_In_  time_t const* _Time
		)
	{
		_VALIDATE_RETURN_ERRCODE(_Tm != nullptr, EINVAL);

		memset(_Tm, 0xff, sizeof(*_Tm));

		_VALIDATE_RETURN_ERRCODE(_Time != nullptr, EINVAL);


		auto tm = _gmtime_t(_Time);
		if (!tm)
			return errno;

		*_Tm = *tm;
		return 0;
	}

	errno_t __cdecl _gmtime32_s_downlevel(
		_Out_ struct tm*        _Tm,
		_In_  __time32_t const* _Time
		)
	{
		return common_gmtime_t_s(_Tm, _Time);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_gmtime32_s_downlevel);

	errno_t __cdecl _gmtime64_s_downlevel(
		_Out_ struct tm*        _Tm,
		_In_  __time64_t const* _Time
		)
	{
		return common_gmtime_t_s(_Tm, _Time);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_gmtime64_s_downlevel);

	__time64_t __cdecl _mkgmtime64_downlevel(
		_Inout_ struct tm* _Tm
		)
	{
		_VALIDATE_RETURN(_Tm != nullptr, EINVAL, -1);

		auto tbtemp = *_Tm;

		auto time = _mktime64(&tbtemp);
		if (time == -1)
		{
			return -1;
		}

		//修正本地时间与UTC时间的差异
		time -= _timezone;

		if (_Tm->tm_isdst > 0 || (_Tm->tm_isdst < 0 && tbtemp.tm_isdst > 0))
		{
			time -= _dstbias;
		}

		auto tm = _gmtime64(&time);
		if (!tm)
			return -1;

		*_Tm = *tm;

		return time;
	}

	_LCRT_DEFINE_IAT_SYMBOL(_mkgmtime64_downlevel);

	__time32_t __cdecl _mkgmtime32_downlevel(
		_Inout_ struct tm* _Tm
		)
	{
		auto tbtemp = *_Tm;
		auto Time = _mkgmtime64(&tbtemp);
		if (Time == -1)
			return -1;

		// Check for illegal time_t value:
		if (Time > _MAX_TIME32 + _MAX_LOCAL_TIME)
		{
			errno = EINVAL;
			return -1;
		}

		*_Tm = tbtemp;

		return Time;
	}

	_LCRT_DEFINE_IAT_SYMBOL(_mkgmtime32_downlevel);

	__time32_t __cdecl _mktime32_downlevel(
		_Inout_ struct tm* _Tm
		)
	{
		auto tbtemp = *_Tm;
		auto Time = _mktime64(&tbtemp);
		if (Time == -1)
			return -1;

		// Check for illegal time_t value:
		if (Time > _MAX_TIME32)
		{
			errno = EINVAL;
			return -1;
		}

		*_Tm = tbtemp;

		return Time;
	}

	_LCRT_DEFINE_IAT_SYMBOL(_mktime32_downlevel);

	extern "C++" template<typename TCHAR>
	__forceinline errno_t __cdecl common_getenv_s_nolock(
		_Out_                            size_t*     _RequiredCount,
		_Out_writes_opt_z_(_BufferCount) TCHAR*       _Buffer,
		_In_                             rsize_t     _BufferCount,
		_In_z_                           TCHAR const* _VarName
		)
	{
		_VALIDATE_RETURN_ERRCODE(_RequiredCount != nullptr, EINVAL);
		*_RequiredCount = 0;

		_VALIDATE_RETURN_ERRCODE(
			(_Buffer != nullptr && _BufferCount >  0) ||
			(_Buffer == nullptr && _BufferCount == 0), EINVAL);

		if (_Buffer)
			_Buffer[0] = '\0';

		const auto value = _tgetenv(_VarName);
		if (!value)
			return 0;

		auto RequiredCount = _tcslen(value) + 1;
		*_RequiredCount = RequiredCount;
		if (_BufferCount == 0)
			return 0;

		// The buffer is too small; we return an error code and the caller can have
		// the opportunity to try again with a larger buffer:
		if (RequiredCount > _BufferCount)
			return ERANGE;

		memcpy(_Buffer, value, RequiredCount * sizeof(TCHAR));

		return 0;
	}

	extern "C++" template<typename TCHAR>
	__forceinline errno_t __cdecl common_getenv_s(
		_Out_                            size_t*     _RequiredCount,
		_Out_writes_opt_z_(_BufferCount) TCHAR*       _Buffer,
		_In_                             rsize_t     _BufferCount,
		_In_z_                           TCHAR const* _VarName
		)
	{
		_lock(/*_ENV_LOCK*/7);

		auto result = common_getenv_s_nolock(_RequiredCount, _Buffer, _BufferCount, _VarName);

		_unlock(/*_ENV_LOCK*/7);

		return result;
	}

	errno_t __cdecl getenv_s_downlevel(
        _Out_                            size_t*     _RequiredCount,
        _Out_writes_opt_z_(_BufferCount) char*       _Buffer,
        _In_                             rsize_t     _BufferCount,
        _In_z_                           char const* _VarName
        )
	{
		return common_getenv_s(_RequiredCount, _Buffer, _BufferCount, _VarName);
	}

	_LCRT_DEFINE_IAT_SYMBOL(getenv_s_downlevel);

	errno_t __cdecl _wgetenv_s_downlevel(
		_Out_                            size_t*        _RequiredCount,
		_Out_writes_opt_z_(_BufferCount) wchar_t*       _Buffer,
		_In_                             size_t         _BufferCount,
		_In_z_                           wchar_t const* _VarName
		)
	{
		return common_getenv_s(_RequiredCount, _Buffer, _BufferCount, _VarName);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_wgetenv_s_downlevel);

	errno_t __cdecl fopen_s_downlevel(
		_Outptr_result_maybenull_ FILE**      _Stream,
		_In_z_                    char const* _FileName,
		_In_z_                    char const* _Mode
		)
	{
		_VALIDATE_RETURN_ERRCODE(_Stream != nullptr, EINVAL);

		return (*_Stream = fopen(_FileName, _Mode)) ? 0 : errno;
	}

	_LCRT_DEFINE_IAT_SYMBOL(fopen_s_downlevel);

	errno_t __cdecl _wfopen_s_downlevel(
		_Outptr_result_maybenull_ FILE**         _Stream,
		_In_z_                    wchar_t const* _FileName,
		_In_z_                    wchar_t const* _Mode
		)
	{
		_VALIDATE_RETURN_ERRCODE(_Stream != nullptr, EINVAL);

		return (*_Stream = _wfopen(_FileName, _Mode)) ? 0 : errno;
	}

	_LCRT_DEFINE_IAT_SYMBOL(_wfopen_s_downlevel);

	errno_t __cdecl freopen_s_downlevel(
		_Outptr_result_maybenull_ FILE**      _Stream,
		_In_z_                    char const* _FileName,
		_In_z_                    char const* _Mode,
		_Inout_                   FILE*       _OldStream
		)
	{
		_VALIDATE_RETURN_ERRCODE(_Stream != nullptr, EINVAL);

		return (*_Stream = freopen(_FileName, _Mode, _OldStream)) ? 0 : errno;
	}

	_LCRT_DEFINE_IAT_SYMBOL(freopen_s_downlevel);

	errno_t __cdecl _wfreopen_s_downlevel(
		_Outptr_result_maybenull_ FILE**         _Stream,
		_In_z_                    wchar_t const* _FileName,
		_In_z_                    wchar_t const* _Mode,
		_Inout_                   FILE*          _OldStream
		)
	{
		_VALIDATE_RETURN_ERRCODE(_Stream != nullptr, EINVAL);
		
		return (*_Stream = _wfreopen(_FileName, _Mode, _OldStream)) ? 0 : errno;
	}

	_LCRT_DEFINE_IAT_SYMBOL(_wfreopen_s_downlevel);

	void __cdecl _ftime32_downlevel(
		_Out_ struct __timeb32* _Time
		)
	{
		__timeb64 _Time64;
		_ftime64(&_Time64);

		_Time->time = _Time64.time;
		_Time->millitm = _Time64.millitm;
		_Time->timezone = _Time64.timezone;
		_Time->dstflag = _Time64.dstflag;
	}

	_LCRT_DEFINE_IAT_SYMBOL(_ftime32_downlevel);

	errno_t __cdecl _ftime32_s_downlevel(
		_Out_ struct __timeb32* _Time
		)
	{
		_VALIDATE_RETURN_ERRCODE(_Time != nullptr, EINVAL);

		_ftime32_downlevel(_Time);
		
		return 0;
	}

	_LCRT_DEFINE_IAT_SYMBOL(_ftime32_s_downlevel);

	errno_t __cdecl _ftime64_s_downlevel(
		_Out_ struct __timeb64* _Time
		)
	{
		_VALIDATE_RETURN_ERRCODE(_Time != nullptr, EINVAL);

		_ftime64(_Time);

		return 0;
	}

	_LCRT_DEFINE_IAT_SYMBOL(_ftime64_s_downlevel);

	int __cdecl _futime32_downlevel(
		_In_     int                 _FileHandle,
		_In_opt_ struct __utimbuf32* _Time
		)
	{
		__utimbuf64 _Time64;

		if (_Time)
		{
			_Time64.actime = _Time->actime;
			_Time64.modtime = _Time->modtime;
		}

		return _futime64(_FileHandle, _Time ? &_Time64 : nullptr);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_futime32_downlevel);

	int __cdecl _utime32_downlevel(
		_In_z_   char const*         _FileName,
		_In_opt_ struct __utimbuf32* _Time
		)
	{
		__utimbuf64 _Time64;

		if (_Time)
		{
			_Time64.actime = _Time->actime;
			_Time64.modtime = _Time->modtime;
		}

		return _utime64(_FileName, _Time ? &_Time64 : nullptr);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_utime32_downlevel);

	int __cdecl _wutime32_downlevel(
		_In_z_   wchar_t const*      _FileName,
		_In_opt_ struct __utimbuf32* _Time
		)
	{
		__utimbuf64 _Time64;

		if (_Time)
		{
			_Time64.actime = _Time->actime;
			_Time64.modtime = _Time->modtime;
		}

		return _wutime64(_FileName, _Time ? &_Time64 : nullptr);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_wutime32_downlevel);

	errno_t __cdecl clearerr_s_downlevel(
		_Inout_ FILE* _Stream
		)
	{
		errno = 0;
		clearerr(_Stream);
		return errno;
	}

	_LCRT_DEFINE_IAT_SYMBOL(clearerr_s_downlevel);

	extern "C++" __forceinline int __cdecl _tputenv(
		_In_z_ wchar_t const* _EnvString
		)
	{
		return _wputenv(_EnvString);
	}

	extern "C++" __forceinline int __cdecl _tputenv(
		_In_z_ char const* _EnvString
		)
	{
		return _putenv(_EnvString);
	}

	extern "C++" template<typename TCAHR> __forceinline errno_t __cdecl common_putenv_s(
		_In_z_ TCAHR const* _Name,
		_In_z_ TCAHR const* _Value
		)
	{
		_VALIDATE_RETURN_ERRCODE(_Value != nullptr, EINVAL);
		_VALIDATE_RETURN_ERRCODE(_Name != nullptr, EINVAL);

		auto cName = _tcslen(_Name);
		auto cValue = _tcslen(_Value);

		auto _EnvString = (TCAHR*)_malloca(cName + cValue + 2 * sizeof(TCAHR));
		if (!_EnvString)
			return errno;

		memcpy(_EnvString, _Name, cName * sizeof(TCAHR));
		_EnvString[cName] = '=';
		memcpy(_EnvString + cName + 1, _Value, (cValue + 1) * sizeof(TCAHR));

		auto result = _tputenv(_EnvString) == 0 ? 0 : errno;

		_freea(_EnvString);

		return result;
	}

	errno_t __cdecl _putenv_s_downlevel(
		_In_z_ char const* _Name,
		_In_z_ char const* _Value
		)
	{
		return common_putenv_s(_Name, _Value);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_putenv_s_downlevel);

	errno_t __cdecl _wputenv_s_downlevel(
		_In_z_ wchar_t const* _Name,
		_In_z_ wchar_t const* _Value
		)
	{
		return common_putenv_s(_Name, _Value);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_wputenv_s_downlevel);

	errno_t __cdecl tmpnam_s_downlevel(
		_Out_writes_z_(_Size) char*   _Buffer,
		_In_                  rsize_t _Size
		)
	{
		_VALIDATE_RETURN_ERRCODE(_Buffer != nullptr, EINVAL);

		auto FileName = tmpnam(nullptr);
		if (!FileName)
			return errno;

		auto cName = _tcslen(FileName) + 1;

		if (cName > _Size)
			return ERANGE;

		memcpy(_Buffer, FileName, cName * sizeof(*FileName));
		*FileName = NULL;

		return 0;
	}

	_LCRT_DEFINE_IAT_SYMBOL(tmpnam_s_downlevel);

	errno_t __cdecl _wtmpnam_s_downlevel(
		_Out_writes_z_(_BufferCount) wchar_t* _Buffer,
		_In_                         size_t   _BufferCount
		)
	{
		_VALIDATE_RETURN_ERRCODE(_Buffer != nullptr, EINVAL);

		auto FileName = _wtmpnam(nullptr);
		if (!FileName)
			return errno;

		auto cName = _tcslen(FileName) + 1;

		if (cName > _BufferCount)
			return ERANGE;

		memcpy(_Buffer, FileName, cName * sizeof(*FileName));
		*FileName = NULL;

		return 0;
	}

	_LCRT_DEFINE_IAT_SYMBOL(_wtmpnam_s_downlevel);

	errno_t __cdecl _sopen_s_downlevel(
		_Out_  int*        _FileHandle,
		_In_z_ char const* _FileName,
		_In_   int         _OpenFlag,
		_In_   int         _ShareFlag,
		_In_   int         _PermissionMode
		)
	{
		_VALIDATE_RETURN_ERRCODE(_FileHandle != nullptr, EINVAL);
		*_FileHandle = -1;

		_VALIDATE_RETURN_ERRCODE(_FileName != nullptr, EINVAL);


		_VALIDATE_RETURN_ERRCODE((_PermissionMode & (~(_S_IREAD | _S_IWRITE))) == 0, EINVAL);


		return (*_FileHandle = _sopen(_FileName, _OpenFlag, _ShareFlag, _PermissionMode)) == -1 ? errno : 0;
	}

	_LCRT_DEFINE_IAT_SYMBOL(_sopen_s_downlevel);

	errno_t __cdecl _wsopen_s_downlevel(
		_Out_  int*           _FileHandle,
		_In_z_ wchar_t const* _FileName,
		_In_   int            _OpenFlag,
		_In_   int            _ShareFlag,
		_In_   int            _PermissionFlag
		)
	{
		_VALIDATE_RETURN_ERRCODE(_FileHandle != nullptr, EINVAL);
		*_FileHandle = -1;

		_VALIDATE_RETURN_ERRCODE(_FileName != nullptr, EINVAL);

		
		_VALIDATE_RETURN_ERRCODE((_PermissionFlag & (~(_S_IREAD | _S_IWRITE))) == 0, EINVAL);
	

		return (*_FileHandle = _wsopen(_FileName, _OpenFlag, _ShareFlag, _PermissionFlag)) == -1 ? errno : 0;
	}

	_LCRT_DEFINE_IAT_SYMBOL(_wsopen_s_downlevel);

	//行为略微跟原版不同，当参数不合法时，不会返回 _OldMode
	errno_t __cdecl _umask_s_downlevel(
		_In_  int  _NewMode,
		_Out_ int* _OldMode
		)
	{
		_VALIDATE_RETURN_ERRCODE(_OldMode != nullptr, EINVAL);
		_VALIDATE_RETURN_ERRCODE((_NewMode & ~(_S_IREAD | _S_IWRITE)) == 0, EINVAL);

		*_OldMode = _umask(_NewMode);

		return 0;
	}

	_LCRT_DEFINE_IAT_SYMBOL(_umask_s_downlevel);

	errno_t __cdecl _strerror_s_downlevel(
		_Out_writes_z_(_SizeInBytes) char*       _Buffer,
		_In_                         size_t      _SizeInBytes,
		_In_opt_z_                   char const* _ErrorMessage
		)
	{
		_VALIDATE_RETURN_ERRCODE(_Buffer != nullptr, EINVAL);
		_VALIDATE_RETURN_ERRCODE(_SizeInBytes > 0, EINVAL);
		_Buffer[0] = '\0';


		auto szError = _strerror(_ErrorMessage);

		if (!szError)
			return errno;

		return _tcscpy_s(_Buffer, _SizeInBytes, szError);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_strerror_s_downlevel);

	errno_t __cdecl __wcserror_s_downlevel(
		_Out_writes_opt_z_(_SizeInWords) wchar_t*       _Buffer,
		_In_                             size_t         _SizeInWords,
		_In_z_                           wchar_t const* _ErrorMessage
		)
	{
		_VALIDATE_RETURN_ERRCODE(_Buffer != nullptr, EINVAL);
		_VALIDATE_RETURN_ERRCODE(_SizeInWords > 0, EINVAL);
		_Buffer[0] = '\0';


		auto szError = __wcserror(_ErrorMessage);

		if (!szError)
			return errno;

		return _tcscpy_s(_Buffer, _SizeInWords, szError);
	}

	_LCRT_DEFINE_IAT_SYMBOL(__wcserror_s_downlevel);

	_ACRTIMP void __cdecl _assert(
		_In_z_ char const* _Message,
		_In_z_ char const* _File,
		_In_   unsigned       _Line
	);


	//WinXP不支持_wassert，因此我们通过字符串转换再调用_assert实现。
	void __cdecl _wassert_downlevel(
		_In_z_ wchar_t const* _Message,
		_In_z_ wchar_t const* _File,
		_In_   unsigned       _Line
	)
	{
		//VC-LTL只针提供Release编译，因此此函数单纯做中断程序处理。
		_assert(nullptr, nullptr, _Line);
	}

	_LCRT_DEFINE_IAT_SYMBOL(_wassert_downlevel);

#endif
	
#ifdef _M_IX86
	extern int __isa_available;

	extern unsigned int __cdecl __get_fpsr_sse2();

	static __forceinline unsigned int __cdecl __statusfp_sse2()
	{
		auto fpsr_sse2 = __get_fpsr_sse2();

		unsigned int result = 0;

		if (fpsr_sse2 & 0x3F)
		{
			if (fpsr_sse2 & 0x01)
				result |= 0x10;

			if (fpsr_sse2 & 0x02)
				result |= 0x80000;

			if (fpsr_sse2 & 0x04)
				result |= 0x8;

			if (fpsr_sse2 & 0x08)
				result |= 0x4;

			if (fpsr_sse2 & 0x10)
				result |= 0x2;

			if (fpsr_sse2 & 0x20)
				result |= 0x1;
		}

		return result;
	}

	void __cdecl _statusfp2_downlevel(
		_Out_opt_ unsigned int* _X86Status,
		_Out_opt_ unsigned int* _SSE2Status
		)
	{
		if (_X86Status)
		{
			unsigned short FpuStatus;

			__asm { fstsw FpuStatus };

			unsigned int dwX86Status = 0;

			if (FpuStatus & 0x3F)
			{
				if (FpuStatus & 0x01)
					dwX86Status |= 0x10;

				if (FpuStatus & 0x02)
					dwX86Status |= 0x80000;

				if (FpuStatus & 0x04)
					dwX86Status |= 0x08;

				if (FpuStatus & 0x08)
					dwX86Status |= 0x04;

				if (FpuStatus & 0x10)
					dwX86Status |= 0x02;

				if (FpuStatus & 0x20)
					dwX86Status |= 0x01;
			}

			*_X86Status = dwX86Status;
		}

		if (_SSE2Status)
			*_SSE2Status = __statusfp_sse2();
	}

	_LCRT_DEFINE_IAT_SYMBOL(_statusfp2_downlevel);


	static unsigned short __cdecl _hw_cw(unsigned int  _NewValue)
	{
		unsigned int result = 0;

		if (_NewValue & 0x10)
			result = 0x1;

		if (_NewValue & 0x8)
			result |= 0x4;

		if (_NewValue & 0x4)
			result |= 0x8;

		if (_NewValue & 0x2)
			result |= 0x10;

		if (_NewValue & 0x1)
			result |= 0x20;

		if (_NewValue & 0x80000)
			result |= 0x2;

		if (_NewValue & 0x100)
			result |= 0x400;

		if (_NewValue & 0x200)
			result |= 0x800;

		if ((_NewValue & 0x30000)== 0x10000)
			result |= 0x200;
		else
			result |= 0x300;

		if (_NewValue & 0x40000)
			result |= 0x1000;

		return result;
	}

	static unsigned int __cdecl __hw_cw_sse2(unsigned int Sse2ControlWord)
	{
		unsigned int result = 0;

		if (Sse2ControlWord & 0x10)
			result = 0x80;
		if (Sse2ControlWord & 0x08)
			result |= 0x200u;
		if (Sse2ControlWord & 0x04)
			result |= 0x400u;
		if (Sse2ControlWord & 0x02)
			result |= 0x800u;
		if (Sse2ControlWord & 0x01)
			result |= 0x1000u;
		if (Sse2ControlWord & 0x80000)
			result |= 0x100u;

		if (Sse2ControlWord & 0x100)
			result |= 0x2000u;

		if (Sse2ControlWord & 0x200)
			result |= 0x4000u;

		
		switch (Sse2ControlWord & 0x3000000)
		{
		case 0x1000000:
			result |= 0x8040u;
			break;
		case 0x2000000:
			result |= 0x40u;
			break;
		case 0x3000000:
			result |= 0x8000u;
			break;
		default:
			break;
		}

		return result;
	}

	static unsigned int __fastcall FpuStatusToX86ControlWord(unsigned short FpuStatus)
	{
		unsigned int CurrentX86ControlWord = 0;

		if (FpuStatus & 0x1)
			CurrentX86ControlWord |= 0x10;

		if (FpuStatus & 0x2)
			CurrentX86ControlWord |= 0x80000;

		if (FpuStatus & 0x4)
			CurrentX86ControlWord |= 0x8;

		if (FpuStatus & 0x8)
			CurrentX86ControlWord |= 0x4;

		if (FpuStatus & 0x10)
			CurrentX86ControlWord |= 0x2;

		if (FpuStatus & 0x20)
			CurrentX86ControlWord |= 0x1;


		if (FpuStatus & 0x400)
			CurrentX86ControlWord |= 0x100;

		if (FpuStatus & 0x800)
			CurrentX86ControlWord |= 0x200;

		if ((FpuStatus & 0x300) == 0x200)
		{
			CurrentX86ControlWord |= 0x10000;
		}
		else
		{
			CurrentX86ControlWord |= 0x20000;
		}

		if (FpuStatus & 0x1000)
		{
			CurrentX86ControlWord |= 0x40000;
		}

		return CurrentX86ControlWord;
	}

	static unsigned int __fastcall Sse2StatusToSse2ControlWord(unsigned int Sse2Status)
	{
		unsigned int Sse2ControlWord = 0;

		if (Sse2Status & 0x8000)
			Sse2ControlWord = 0x10;

		if (Sse2Status & 0x200)
			Sse2ControlWord |= 0x8;

		if (Sse2Status & 0x400)
			Sse2ControlWord |= 0x4;

		if (Sse2Status & 0x800)
			Sse2ControlWord |= 0x2;

		if (Sse2Status & 0x1000)
			Sse2ControlWord |= 0x1;

		if (Sse2Status & 0x100)
			Sse2ControlWord |= 0x80000;

		if (Sse2Status & 0x2000)
			Sse2ControlWord |= 0x100;

		if (Sse2Status & 0x4000)
			Sse2ControlWord |= 0x200;


		switch (Sse2Status & 0x8040)
		{
		case 0x40:
			Sse2ControlWord |= 0x2000000;
			break;
		case 0x8000:
			Sse2ControlWord |= 0x3000000;
			break;
		case 0x8040:
			Sse2ControlWord |= 0x1000000;
			break;
		default:
			break;
		}

		return Sse2ControlWord;
	}

	extern void __cdecl __set_fpsr_sse2(unsigned int);

	int __cdecl __control87_2_downlevel(
		_In_      unsigned int  _NewValue,
		_In_      unsigned int  _Mask,
		_Out_opt_ unsigned int* _X86ControlWord,
		_Out_opt_ unsigned int* _Sse2ControlWord
		)
	{
		_NewValue &= _Mask;

		if (_X86ControlWord)
		{
			unsigned short FpuStatus;

			__asm { fstcw FpuStatus };

			unsigned int CurrentX86ControlWord = FpuStatusToX86ControlWord(FpuStatus);


			auto NewX86ControlWord = (_NewValue) | (CurrentX86ControlWord&~_Mask);


			if (NewX86ControlWord != CurrentX86ControlWord)
			{
				FpuStatus = _hw_cw(NewX86ControlWord);

				__asm
				{
					fldcw FpuStatus
					fstcw FpuStatus
				}

				NewX86ControlWord = FpuStatusToX86ControlWord(FpuStatus);
			}

			*_X86ControlWord = NewX86ControlWord;
		}


		if (_Sse2ControlWord)
		{
			if (__isa_available < 1)
			{
				*_Sse2ControlWord = 0;
			}
			else
			{
				auto CurrentSse2ControlWord = Sse2StatusToSse2ControlWord(_mm_getcsr());

				auto NewSse2ControlWord = (_NewValue & 0x308031F) | (CurrentSse2ControlWord&~(_Mask & 0x308031F));

				if (NewSse2ControlWord != CurrentSse2ControlWord)
				{
					__set_fpsr_sse2(__hw_cw_sse2(NewSse2ControlWord));

					CurrentSse2ControlWord= Sse2StatusToSse2ControlWord(_mm_getcsr());
				}

				*_Sse2ControlWord = CurrentSse2ControlWord;
			}
		}

		return 1;
	}

	_LCRT_DEFINE_IAT_SYMBOL(__control87_2_downlevel);

#endif

#undef _sys_nerr
#undef _sys_errlist

	extern "C" __declspec(dllimport) extern char const* const _sys_errlist[];
	extern "C" __declspec(dllimport) extern int const _sys_nerr;

	extern "C" int* __cdecl __sys_nerr_downlevel()
	{
		return const_cast<int*>(&_sys_nerr);
	}

	_LCRT_DEFINE_IAT_SYMBOL(__sys_nerr_downlevel);

	extern "C" char** __cdecl __sys_errlist_downlevel()
	{
		return const_cast<char**>(_sys_errlist);
	}

	_LCRT_DEFINE_IAT_SYMBOL(__sys_errlist_downlevel);

#if defined _M_AMD64 || defined _M_ARM64 || defined _M_ARM

	extern "C" unsigned char* __cdecl __p__mbctype_downlevel()
	{
		return _mbctype;
	}
	_LCRT_DEFINE_IAT_SYMBOL(__p__mbctype_downlevel);

	extern "C" unsigned char* __cdecl __p__mbcasemap_downlevel()
	{
		return _mbcasemap;
	}

	_LCRT_DEFINE_IAT_SYMBOL(__p__mbcasemap_downlevel);

	#undef _environ
	#undef _wenviron

#if defined _ARM64_ || defined _ARM_
	extern "C" extern char **    _environ;
	extern "C" extern wchar_t ** _wenviron;
#else
	extern "C" __declspec(dllimport) extern char **    _environ;
	extern "C" __declspec(dllimport) extern wchar_t ** _wenviron;
#endif

	extern "C" char***    __cdecl __p__environ_downlevel() { return &_environ; }
	_LCRT_DEFINE_IAT_SYMBOL(__p__environ_downlevel);

	extern "C" wchar_t*** __cdecl __p__wenviron_downlevel() { return &_wenviron; }
	_LCRT_DEFINE_IAT_SYMBOL(__p__wenviron_downlevel);


#endif

#if _CRT_NTDDI_MIN >= NTDDI_WIN6
	//Vista以及以上版本才存在_wcsftime_l，因此只在Vista以上模式输出此函数
	extern "C" size_t __cdecl _strftime_l_downlevel(
		char*       const string,
		size_t      const maxsize,
		char const* const format,
		tm const*   const timeptr,
		_locale_t   const locale
		)
	{
		if (!locale)
			return strftime(string, maxsize, format, timeptr);

		unsigned int const lc_time_cp = locale->locinfo->_locale_lc_codepage;

		_VALIDATE_RETURN(string != nullptr, EINVAL, 0)
		_VALIDATE_RETURN(maxsize != 0,      EINVAL, 0)
		*string = '\0';

		_VALIDATE_RETURN(format != nullptr,  EINVAL, 0)
		_VALIDATE_RETURN(timeptr != nullptr, EINVAL, 0)

		
		auto cwFormat = MultiByteToWideChar(
			lc_time_cp,
            MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			format,
            -1,
			nullptr,
            0
            );

		if (cwFormat == 0)
			return 0;

		__crt_scoped_stack_ptr<wchar_t> wformat(_malloca_crt_t(wchar_t, cwFormat));
		if (!wformat)
			return 0;

		if (MultiByteToWideChar(
			lc_time_cp,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			format,
			-1,
			wformat.get(),
			cwFormat
			)==0)
		{
			errno = EINVAL;
			return 0;
		}



		__crt_scoped_stack_ptr<wchar_t> wstring(_malloca_crt_t(wchar_t, maxsize));

		if (!wstring)
			return 0;

		size_t const wcsftime_result = _wcsftime_l(wstring.get(), maxsize, wformat.get(), timeptr, locale);
		if (wcsftime_result == 0)
		{
			return 0;
		}


		auto string_used = WideCharToMultiByte(
			lc_time_cp,
			0,
			wstring.get(),
			-1,
			string,
			maxsize,
			nullptr,
			nullptr
		);

		//缓冲区不足或者缓冲区不足以容纳
		if (string_used == 0 || string_used > maxsize)
			return 0;


		// The WideCharToMultiByte result includes the null terminator; the strftime
		// function result does not:
		return string_used - 1;
	}

	_LCRT_DEFINE_IAT_SYMBOL(_strftime_l_downlevel);

#endif
}

#endif //NDEBUG&&_DLL&&__NO_LTL_LIB