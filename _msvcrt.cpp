#pragma once

#if defined(NDEBUG)&&defined(_DLL)&&defined(__NO_LTL_LIB)
#include <vcruntime_new.h>
#include <corecrt_terminate.h>
#include <stdlib.h>
#include <corecrt_wstdio.h>
#include <Windows.h>
#include <winnt.h>
#include <intrin.h>  
#include <vcruntime_exception.h>
#include <crtdbg.h>

#ifdef __NOTHROW_T_DEFINED

#ifdef __cplusplus
extern "C++"
{
	namespace std
	{
		const nothrow_t nothrow;
	}
}
#endif
#endif

extern "C"
{
	/*void __fastcall _guard_check_icall(void*)
	{
	}*/

	//切换到系统的msvrct后无需初始化onexit_table，msvcrt.dll内部会初始化
	bool __cdecl __scrt_initialize_onexit_tables(int const module_type)
	{
		return true;
	}

	void __std_terminate(void)
	{
		terminate();
	}

	/*_ACRTIMP FILE* __cdecl __iob_func(unsigned);

	FILE* __cdecl __acrt_iob_func(unsigned in)
	{
		return __iob_func(in);
	}*/

	/*unsigned long long __cdecl wcstoull(
		_In_z_                   wchar_t const* _String,
		_Out_opt_ _Deref_post_z_ wchar_t**      _EndPtr,
		_In_                     int            _Radix
		)
	{
		return _wcstoui64(_String, _EndPtr, _Radix);
	}*/

	//系统自带有double，用此将其转换为float
	/*float __cdecl strtof(
		_In_z_                   char const* _String,
		_Out_opt_ _Deref_post_z_ char**      _EndPtr
	)
	{
		return strtod(_String, _EndPtr);
	}*/

	/*float __cdecl wcstof(
		_In_z_                   wchar_t const* _String,
		_Out_opt_ _Deref_post_z_ wchar_t**      _EndPtr
	)
	{
		return wcstod(_String, _EndPtr);
	}*/

	BOOL __cdecl __vcrt_InitializeCriticalSectionEx(
		LPCRITICAL_SECTION const critical_section,
		DWORD              const spin_count,
		DWORD              const flags
	)
	{
#ifdef _ATL_XP_TARGETING
		return InitializeCriticalSectionAndSpinCount(critical_section, spin_count);
#else
		return InitializeCriticalSectionEx(critical_section, spin_count, flags);
#endif
	}
	//int __scrt_debugger_hook_flag = 0;

	//void __cdecl _CRT_DEBUGGER_HOOK(int const reserved)
	//{
	//	UNREFERENCED_PARAMETER(reserved);

	//	// We assign zero to the debugger hook flag so that this function is not
	//	// folded when optimized.  The flag is not otherwise used.
	//	__scrt_debugger_hook_flag = 0;
	//}
#define _CRT_DEBUGGER_HOOK(a) (0)

	void __cdecl __scrt_fastfail(unsigned const code)
	{
		// First see if __fastfail is available, and invoke it if it is.  This will
		// always be availbale on ARM and is always available on Windows 8 and above.
		if (IsProcessorFeaturePresent(PF_FASTFAIL_AVAILABLE))
			__fastfail(code);

		// If __fastfail is not available, fall back to raising an exception that
		// bypasses all exception handlers (via a call to the unhandled exception
		// filter.

		// Notify the debugger if attached.
		_CRT_DEBUGGER_HOOK(0);

		CONTEXT context_record = {};

#if defined _M_IX86

		__asm
		{
			mov dword ptr[context_record.Eax], eax
			mov dword ptr[context_record.Ecx], ecx
			mov dword ptr[context_record.Edx], edx
			mov dword ptr[context_record.Ebx], ebx
			mov dword ptr[context_record.Esi], esi
			mov dword ptr[context_record.Edi], edi
			mov word ptr[context_record.SegSs], ss
			mov word ptr[context_record.SegCs], cs
			mov word ptr[context_record.SegDs], ds
			mov word ptr[context_record.SegEs], es
			mov word ptr[context_record.SegFs], fs
			mov word ptr[context_record.SegGs], gs
			pushfd
			pop[context_record.EFlags]
		}

		context_record.ContextFlags = CONTEXT_CONTROL;
		context_record.Eip = reinterpret_cast<ULONG>(_ReturnAddress());
		context_record.Esp = reinterpret_cast<ULONG>(_AddressOfReturnAddress());
		context_record.Ebp = *(reinterpret_cast<ULONG*>(_AddressOfReturnAddress()) - 1);

#elif defined _M_X64

		RtlCaptureContext(&context_record);

		ULONG64 const control_pc = context_record.Rip;

		ULONG64 image_base;
		PRUNTIME_FUNCTION const function_entry = RtlLookupFunctionEntry(control_pc, &image_base, nullptr);

		if (function_entry)
		{
			ULONG64 establisher_frame;
			PVOID   handler_data;
			RtlVirtualUnwind(
				UNW_FLAG_NHANDLER,
				image_base,
				control_pc,
				function_entry,
				&context_record,
				&handler_data,
				&establisher_frame,
				nullptr);
		}

		context_record.Rip = reinterpret_cast<ULONGLONG>(_ReturnAddress());
		context_record.Rsp = reinterpret_cast<ULONGLONG>(_AddressOfReturnAddress()) + 8;

#endif

		EXCEPTION_RECORD exception_record = {};
		exception_record.ExceptionCode = STATUS_FATAL_APP_EXIT;
		exception_record.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
		exception_record.ExceptionAddress = _ReturnAddress();

		bool const was_debugger_present = IsDebuggerPresent() == TRUE;

		// Raise an exception that bypasses all exception handlers:
		EXCEPTION_POINTERS exception_pointers = { &exception_record, &context_record };

		SetUnhandledExceptionFilter(nullptr);
		LONG const result = UnhandledExceptionFilter(&exception_pointers);

		// If no handler was found and no debugger was previously attached, then make
		// sure we notify the debugger.
		if (result == EXCEPTION_CONTINUE_SEARCH && !was_debugger_present)
			_CRT_DEBUGGER_HOOK(0);
	}

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

	void __cdecl _invalid_parameter(
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
	}

	/*errno_t __CRTDECL wmemcpy_s(
		_Out_writes_to_opt_(_N1, _N) wchar_t*       _S1,
		_In_                         rsize_t        _N1,
		_In_reads_opt_(_N)           wchar_t const* _S2,
		_In_                         rsize_t        _N
	)
	{
		return memcpy_s(_S1, _N1 * sizeof(wchar_t), _S2, _N * sizeof(wchar_t));
	}

	errno_t __CRTDECL wmemmove_s(
			_Out_writes_to_opt_(_N1, _N) wchar_t*       _S1,
			_In_                         rsize_t        _N1,
			_In_reads_opt_(_N)           wchar_t const* _S2,
			_In_                         rsize_t        _N
		)
	{
		return memmove_s(_S1, _N1 * sizeof(wchar_t), _S2, _N * sizeof(wchar_t));
	}*/


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

	__time64_t gettime()
	{
		FILETIME FileTime;

		GetSystemTimeAsFileTime(&FileTime);

		__time64_t tmp = ((__time64_t)FileTime.dwHighDateTime << 32) | FileTime.dwLowDateTime;

		tmp = (tmp - 116444736000000000) / 10000000;

		return tmp;
	}
}

#ifdef __cplusplus
extern "C++"
{
//如果你需要禁用重载，请先定义 VCLTL_Not_Overload_New
#ifndef VCLTL_Not_Overload_New


	void* __CRTDECL operator new(
		size_t _Size
		)
	{
		return malloc(_Size);
	}

	void* __CRTDECL operator new[](
			size_t _Size
			)
	{
		return malloc(_Size);
	}

	void __CRTDECL operator delete(
			void* _Block
			) throw()
	{
		free(_Block);
	}

	void __CRTDECL operator delete[](
		void* _Block
		)
	{
		free(_Block);
	}

	void __CRTDECL operator delete(
			void*  _Block,
			size_t _Size
			)
	{
		free(_Block);
	}

	void __CRTDECL operator delete[](
		void* _Block,
		size_t _Size
		)
	{
		free(_Block);
	}

#endif // !VCLTL_Not_Overload_New

	#include <functional>
	#include <regex>

	_STD_BEGIN
	[[noreturn]] void __CLRCALL_PURE_OR_CDECL _Xbad_alloc()
	{	// report a bad_alloc error

		_THROW(bad_alloc, _EMPTY_ARGUMENT);

	}

	[[noreturn]] void __CLRCALL_PURE_OR_CDECL _Xinvalid_argument(_In_z_ const char *_Message)
	{	// report an invalid_argument error
		_THROW(invalid_argument, _Message);
	}

	[[noreturn]] void __CLRCALL_PURE_OR_CDECL _Xlength_error(_In_z_ const char *_Message)
	{	// report a length_error
		_THROW(length_error, _Message);
	}

	[[noreturn]] void __CLRCALL_PURE_OR_CDECL _Xout_of_range(_In_z_ const char *_Message)
	{	// report an out_of_range error
		_THROW(out_of_range, _Message);
	}

	[[noreturn]] void __CLRCALL_PURE_OR_CDECL _Xoverflow_error(_In_z_ const char *_Message)
	{	// report an overflow error
		_THROW(overflow_error, _Message);
	}

	[[noreturn]] void __CLRCALL_PURE_OR_CDECL _Xruntime_error(_In_z_ const char *_Message)
	{	// report a runtime_error
		_THROW(runtime_error, _Message);
	}

	[[noreturn]] void __CLRCALL_PURE_OR_CDECL _Xbad_function_call()
	{	// report a bad_function_call error
		_THROW(bad_function_call, _EMPTY_ARGUMENT);
	}

	[[noreturn]] void __CLRCALL_PURE_OR_CDECL _Xregex_error(regex_constants::error_type _Code)
	{	// report a regex_error
		_THROW(regex_error, _Code);
	}
	_STD_END
}
#endif // !__cplusplus

#endif //NDEBUG&&_DLL&&__NO_LTL_LIB