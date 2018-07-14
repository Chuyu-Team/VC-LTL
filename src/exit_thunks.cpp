
#include <corecrt_internal.h>
#include "msvcrt_IAT.h"

extern "C" extern _onexit_table_t __acrt_at_quick_exit_table;


static int __cdecl atexit_exception_filter(unsigned long const _exception_code) throw()
{
	if (_exception_code == ('msc' | 0xE0000000))
	{
		return EXCEPTION_EXECUTE_HANDLER;
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

// CRT_REFACTOR TODO This is duplicated in the VCStartup utility_desktop.cpp.
static bool __cdecl is_managed_app() throw()
{
	PIMAGE_DOS_HEADER const dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(GetModuleHandleW(nullptr));
	if (dos_header == nullptr)
		return false;

	if (dos_header->e_magic != IMAGE_DOS_SIGNATURE)
		return false;

	PIMAGE_NT_HEADERS const pe_header = reinterpret_cast<PIMAGE_NT_HEADERS>(
		reinterpret_cast<BYTE*>(dos_header) + dos_header->e_lfanew);

	if (pe_header->Signature != IMAGE_NT_SIGNATURE)
		return false;

	if (pe_header->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC)
		return false;

	// prefast assumes we are overrunning __ImageBase
#pragma warning(push)
#pragma warning(disable: 26000)

	if (pe_header->OptionalHeader.NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR)
		return false;

#pragma warning(pop)

	if (pe_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR].VirtualAddress == 0)
		return false;

	return true;
}

typedef void (WINAPI* exit_process_pft)(UINT);

static void __cdecl try_cor_exit_process(UINT const return_code) throw()
{
	__crt_unique_hmodule mscoree;
	if (!GetModuleHandleExW(0, L"mscoree.dll", mscoree.get_address_of()))
		return;

	auto const cor_exit_process = __crt_get_proc_address<exit_process_pft>(mscoree.get(), "CorExitProcess");
	if (!cor_exit_process)
		return;

	cor_exit_process(return_code);
}


extern "C" void __cdecl quick_exit_downlevel(int const return_code)
{
	// First, check to see if we're loaded in a managed app.  If we are, try to
	// call CorExitProcess to let the CLR handle the process termination.  If
	// the call to CorExitProcess is successful, then it will call back through
	// this function with a return mode of _crt_exit_return_to_caller, at which
	// point we will run the C termination routines.  It will then terminate the
	// process itself and not return.
	if (is_managed_app())
	{
		try_cor_exit_process(return_code);
	}


	__acrt_lock_and_call(__acrt_exit_lock, [&]()
	{
		__try
		{
			_execute_onexit_table(&__acrt_at_quick_exit_table);
		}
		__except (atexit_exception_filter(GetExceptionCode()))
		{
			terminate();
		}

		_exit(return_code);

		return;
	});
}

_LCRT_DEFINE_IAT_SYMBOL(quick_exit_downlevel);
