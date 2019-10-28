
#include <Windows.h>
#include <ptd_downlevel.h>

static ptd_downlevel ___ptd_downlevel_data[0x4000];


EXTERN_C ptd_downlevel* __fastcall __LTL_get_ptd_downlevel()
{
	return &___ptd_downlevel_data[(GetCurrentThreadId() >> 1) % _countof(___ptd_downlevel_data)];
}

static VOID NTAPI ptd_downlevel_TLS_CALLBACK(
	PVOID DllHandle,
	DWORD Reason,
	PVOID Reserved
	)
{
	if (DLL_THREAD_ATTACH == Reason)
	{
		memset(__LTL_get_ptd_downlevel(), 0, sizeof(ptd_downlevel));
	}
}

#pragma section(".CRT$XLB",    long, read) // LTL Loader TLS Callback
__declspec(allocate(".CRT$XLB")) static PIMAGE_TLS_CALLBACK __LTL_TLS_CALLBACKs = ptd_downlevel_TLS_CALLBACK;

#if defined(_X86_)
#pragma comment(linker, "/INCLUDE:__tls_used")
#else
#pragma comment(linker, "/INCLUDE:_tls_used")
#endif
