//
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
#include <crtdbg.h>

// From <vccorlib.h>, which we cannot include here
#define _VCCORLIB_THREADING_MODEL_DEFAULT  0
#define _VCCORLIB_THREADING_MODEL_STA      1
#define _VCCORLIB_THREADING_MODEL_MTA      2
#define _VCCORLIB_THREADING_MODEL_OVERRIDE 3

extern int __abi___threading_model;

// Compiler is pulling in the following symbol:
// INCLUDE:___refSTAThread (decorated x86) to enable STA
extern "C" int __refSTAThread = 0;

// External symbol to ensure that only one threading model initializer can be
// linked into a module.  If a project uses both Platform::STAThreadAttribute
// and Platform::MTAThreadAttribute, this should cause a link error.
extern "C" int __abi___threading_model_initializer = 0;

static int __cdecl initialize_threading_model_for_sta() throw()
{
	// If the threading model is 'override', some prior initializer has requested that
	// we not initialize the threading model.  This feature exists to support the XAML
	// designer, which loads EXEs as if they were DLLs, and thus needs to be able to
	// suppress our initialization.
	if (__abi___threading_model == _VCCORLIB_THREADING_MODEL_OVERRIDE)
	{
		return 0;
	}

	if (__abi___threading_model != _VCCORLIB_THREADING_MODEL_DEFAULT)
	{
		// If you hit this assertion, this indicates that both Platform::STAThreadAttribute
		// and Platform::MTAThreadAttribute were used in your source files and libraries.
		// These attributes are mutually exclusive.  Search your sources for the attributes
		// or link your project with the /verbose option and find which object files caused
		// the __refSTAThread and __refMTAThread symbols to be linked into your binary.
		_ASSERTE(("__abi___threading_model has already been initialized", 0));
		return 1;
	}

	__abi___threading_model = _VCCORLIB_THREADING_MODEL_STA;
	return 0;
}

#pragma section(".CRT$XIYA", long, read)
__declspec(allocate(".CRT$XIYA")) static auto initializer = initialize_threading_model_for_sta;
