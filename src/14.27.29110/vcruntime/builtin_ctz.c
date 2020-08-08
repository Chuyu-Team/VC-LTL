/*++

Copyright (c) Microsoft Corporation

Abstract:

    This module contains fallback routines for __builtin_ctz
    on machines that don't support a native ctz instruction

--*/

int __cdecl __builtin_clz_helper(unsigned x);
int __cdecl __builtin_clzll_helper(unsigned long long x);

// see "Hacker's Delight" section 5-4
int __cdecl __builtin_ctz_helper(unsigned x)
{
    return 32 - __builtin_clz_helper(~x & (x - 1));
}

int __cdecl __builtin_ctzll_helper(unsigned long long x)
{
    return 64 - __builtin_clzll_helper(~x & (x - 1));
}
