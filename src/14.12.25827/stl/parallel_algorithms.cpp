// Copyright (c) Microsoft Corporation. All rights reserved.
#include <corecrt_terminate.h>
#include <internal_shared.h>

// This must be as small as possible, because its contents are
// injected into the msvcprt.lib and msvcprtd.lib import libraries.
// Do not include or define anything else here.
// In particular, basic_string must not be included here.

// support for <execution>

#if _STL_WIN32_WINNT < _WIN32_WINNT_VISTA
namespace {
struct _Parallel_init_info
	{
	unsigned int _Hw_threads;
	decltype(CreateThreadpoolWork) * _Pfn_CreateThreadpoolWork;
	decltype(SubmitThreadpoolWork) * _Pfn_SubmitThreadpoolWork;
	decltype(CloseThreadpoolWork) * _Pfn_CloseThreadpoolWork;
	decltype(WaitForThreadpoolWorkCallbacks) * _Pfn_WaitForThreadpoolWorkCallbacks;
	};

_Parallel_init_info * _Parallel_info;
}	// unnamed namespace
#endif /* _STL_WIN32_WINNT < _WIN32_WINNT_VISTA */

extern "C" {

unsigned int __stdcall __std_parallel_algorithms_hw_threads() noexcept
	{
#if _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA
	SYSTEM_INFO _Info;
	GetNativeSystemInfo(&_Info);
	return (_Info.dwNumberOfProcessors);
#else /* ^^^ Vista ^^^ // vvv pre-Vista vvv */
	for (;;)
		{
		const auto _Local = __crt_interlocked_read_pointer(&_Parallel_info);
		if (_Local)
			{
			return (_Local->_Hw_threads);
			}

		_Parallel_init_info * const _Result = static_cast<_Parallel_init_info *>(
			VirtualAlloc(0, sizeof(_Parallel_init_info), MEM_COMMIT, PAGE_READWRITE));
		if (!_Result)
			{	// don't parallelize, no RAM
			return (1);
			}

		HMODULE _Kernel32 = GetModuleHandleW(L"kernel32.dll");
		_Result->_Pfn_CreateThreadpoolWork = reinterpret_cast<decltype(CreateThreadpoolWork) *>(
			GetProcAddress(_Kernel32, "CreateThreadpoolWork"));
		_Result->_Pfn_SubmitThreadpoolWork = reinterpret_cast<decltype(SubmitThreadpoolWork) *>(
			GetProcAddress(_Kernel32, "SubmitThreadpoolWork"));
		_Result->_Pfn_CloseThreadpoolWork = reinterpret_cast<decltype(CloseThreadpoolWork) *>(
			GetProcAddress(_Kernel32, "CloseThreadpoolWork"));
		_Result->_Pfn_WaitForThreadpoolWorkCallbacks = reinterpret_cast<decltype(WaitForThreadpoolWorkCallbacks) *>(
			GetProcAddress(_Kernel32, "WaitForThreadpoolWorkCallbacks"));

		if (_Result->_Pfn_CreateThreadpoolWork
			&& _Result->_Pfn_SubmitThreadpoolWork
			&& _Result->_Pfn_CloseThreadpoolWork
			&& _Result->_Pfn_WaitForThreadpoolWorkCallbacks)
			{
			SYSTEM_INFO _Info;
			GetNativeSystemInfo(&_Info);
			_Result->_Hw_threads = _Info.dwNumberOfProcessors;
			}
		else
			{	// don't try to parallelize without Vista threadpool
			_Result->_Hw_threads = 1;
			}

		DWORD _Unused;
		if (VirtualProtect(_Result, sizeof(_Parallel_init_info), PAGE_READONLY, &_Unused) == 0)
			{
			terminate();
			}

		if (InterlockedCompareExchangePointer(reinterpret_cast<void * volatile *>(&_Parallel_info), _Result, 0))
			{	// another thread got there first
			VirtualFree(_Result, 0, MEM_RELEASE);
			}
		}
#endif /* _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA */
	}

// Relaxed reads of _Parallel_info below because __std_parallel_algorithms_hw_threads must be called
// before any of these on each thread.

PTP_WORK __stdcall __std_create_threadpool_work(PTP_WORK_CALLBACK _Callback,
	void * _Context, PTP_CALLBACK_ENVIRON _Callback_environ
	) noexcept
	{
#if _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA
	return (CreateThreadpoolWork(_Callback, _Context, _Callback_environ));
#else /* ^^^ Vista ^^^ // vvv pre-Vista vvv */
	return (_Parallel_info->_Pfn_CreateThreadpoolWork(_Callback, _Context, _Callback_environ));
#endif /* _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA */
	}

void __stdcall __std_submit_threadpool_work(PTP_WORK _Work) noexcept
	{
#if _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA
	SubmitThreadpoolWork(_Work);
#else /* ^^^ Vista ^^^ // vvv pre-Vista vvv */
	_Parallel_info->_Pfn_SubmitThreadpoolWork(_Work);
#endif /* _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA */
	}

void __stdcall __std_bulk_submit_threadpool_work(PTP_WORK _Work, const size_t _Submissions) noexcept
	{
#if _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA
	for (size_t _Idx = 0; _Idx < _Submissions; ++_Idx)
		{
		SubmitThreadpoolWork(_Work);
		}
#else /* ^^^ Vista ^^^ // vvv pre-Vista vvv */
	const auto _Fn = _Parallel_info->_Pfn_SubmitThreadpoolWork;
	for (size_t _Idx = 0; _Idx < _Submissions; ++_Idx)
		{
		_Fn(_Work);
		}
#endif /* _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA */
	}

void __stdcall __std_close_threadpool_work(PTP_WORK _Work) noexcept
	{
#if _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA
	CloseThreadpoolWork(_Work);
#else /* ^^^ Vista ^^^ // vvv pre-Vista vvv */
	_Parallel_info->_Pfn_CloseThreadpoolWork(_Work);
#endif /* _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA */
	}

void __stdcall __std_wait_for_threadpool_work_callbacks(PTP_WORK _Work, BOOL _Cancel) noexcept
	{
#if _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA
	WaitForThreadpoolWorkCallbacks(_Work, _Cancel);
#else /* ^^^ Vista ^^^ // vvv pre-Vista vvv */
	_Parallel_info->_Pfn_WaitForThreadpoolWorkCallbacks(_Work, _Cancel);
#endif /* _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA */
	}

} // extern "C"
