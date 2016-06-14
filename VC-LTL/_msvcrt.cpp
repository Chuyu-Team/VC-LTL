#pragma once

//#include "stdafx.h"

#if NDEBUG&&_DLL
#include <vcruntime_new.h>
#include <Windows.h>

namespace std
{
	const nothrow_t nothrow;
}

extern "C" void __fastcall _guard_check_icall(void*)
{
}

extern "C" void __std_terminate(void)
{
	terminate();
}
#endif