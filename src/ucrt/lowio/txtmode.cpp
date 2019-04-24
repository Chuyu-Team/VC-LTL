//
// txtmode.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines the global _fmode variable and sets the global file mode to text.
// This is the default behavior.
//
#include <corecrt_internal.h>
#include <stdlib.h>
#include <msvcrt_IAT.h>

// Clients of the static CRT can choose to access _fmode directly as a global variable; they do so as if it was declared as an int.
// Because state separation is disabled in the static CRT, the dual_state_global<int> has the same representation as an int, so this is okay, if a bit messy.
EXTERN_C __declspec(dllimport) extern int _fmode;  // This is automatically initialized to zero by the compiler

#if !defined _AMD64_ && !defined _ARM64_ && !defined _ARM_
#error "仅需要在x64中编译"
#endif


extern "C" int* __cdecl __p__fmode()
{
    _BEGIN_SECURE_CRT_DEPRECATION_DISABLE
    return &_fmode;
    _END_SECURE_CRT_DEPRECATION_DISABLE
}

_LCRT_DEFINE_IAT_SYMBOL(__p__fmode);
