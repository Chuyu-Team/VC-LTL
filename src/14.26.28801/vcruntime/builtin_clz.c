/*++

Copyright (c) Microsoft Corporation

Abstract:

    This module contains fallback routines for __builtin_clz
    on machines that don't support a native clz instruction

--*/

// see "Hacker's Delight" section 5-3
int __cdecl __builtin_clz_helper(unsigned x)
{
    unsigned y;
    int n = 32;
    int c = 16;
    do
    {
        y = x >> c;
        if (y != 0)
        {
            n = n - c;
            x = y;
        }
        c = c >> 1;
    } while (c != 0);
    return n - (int)x;
}

int __cdecl __builtin_clzll_helper(unsigned long long x)
{
    unsigned long long y;
    int n = 64;
    int c = 32;
    do
    {
        y = x >> c;
        if (y != 0)
        {
            n = n - c;
            x = y;
        }
        c = c >> 1;
    } while (c != 0);
    return n - (int)x;
}
