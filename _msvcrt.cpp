#pragma once

#if NDEBUG&&_DLL
#include <vcruntime_new.h>
#include <corecrt_terminate.h>
#include <stdlib.h>
#include <corecrt_wstdio.h>

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
	void __fastcall _guard_check_icall(void*)
	{
	}

	void __std_terminate(void)
	{
		terminate();
	}

	FILE* __cdecl __iob_func(unsigned);

	FILE* __cdecl __acrt_iob_func(unsigned in)
	{
		return __iob_func(in);
	}

	unsigned long long __cdecl wcstoull(
		_In_z_                   wchar_t const* _String,
		_Out_opt_ _Deref_post_z_ wchar_t**      _EndPtr,
		_In_                     int            _Radix
		)
	{
		return _wcstoui64(_String, _EndPtr, _Radix);
	}
}

#ifdef __cplusplus
extern "C++"
{
//如果你需要禁用重载，请先定义 VCLTL_Not_Overload_New
#ifndef VCLTL_Not_Overload_New


	extern void* __CRTDECL operator new(
		size_t _Size
		)
	{
		return malloc(_Size);
	}

		extern void* __CRTDECL operator new[](
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

	extern void __CRTDECL operator delete[](
		void* _Block
		)
	{
		free(_Block);
	}

		extern void __CRTDECL operator delete(
			void*  _Block,
			size_t _Size
			)
	{
		free(_Block);
	}

	extern void __CRTDECL operator delete[](
		void* _Block,
		size_t _Size
		)
	{
		free(_Block);
	}

#endif // !VCLTL_Not_Overload_New

	namespace std
	{
		[[noreturn]] void __CLRCALL_PURE_OR_CDECL _Xbad_alloc()
		{
			DebugBreak();
		}
		[[noreturn]] void __CLRCALL_PURE_OR_CDECL _Xinvalid_argument(_In_z_ const char *)
		{
			DebugBreak();
		}
		[[noreturn]] void __CLRCALL_PURE_OR_CDECL _Xlength_error(_In_z_ const char *)
		{
			DebugBreak();
		}
		[[noreturn]] void __CLRCALL_PURE_OR_CDECL _Xout_of_range(_In_z_ const char *)
		{
			DebugBreak();
		}
		[[noreturn]] void __CLRCALL_PURE_OR_CDECL _Xoverflow_error(_In_z_ const char *)
		{
			DebugBreak();
		}
		[[noreturn]] void __CLRCALL_PURE_OR_CDECL _Xruntime_error(_In_z_ const char *)
		{
			DebugBreak();
		}

		[[noreturn]] void __CLRCALL_PURE_OR_CDECL _Xbad_function_call()
		{
			DebugBreak();
		}
	}
}
#endif // !__cplusplus

#endif