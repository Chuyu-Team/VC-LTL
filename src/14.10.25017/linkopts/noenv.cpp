//
// noenv.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// A link option that suppresses environment processing during startup.  If this
// link option is used, envp is left empty when main() is called and the global
// environment manipulation functions are unusable.
//
#include <vcruntime_startup.h>


extern "C" wchar_t* __CRTDECL __dcrt_get_wide_environment_from_os  () { return nullptr; }
extern "C" char*    __CRTDECL __dcrt_get_narrow_environment_from_os() { return nullptr; }

extern "C" bool __CRTDECL _should_initialize_environment() { return false; }
