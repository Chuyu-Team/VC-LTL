/*++

Copyright (c) Microsoft Corporation

Abstract:

    This module contains fallback routines for __builtin_popcount
    on machines that don't support a native popcount instruction

--*/

// see "Hacker's Delight" section 5-1
int __cdecl __builtin_popcount_helper(unsigned x)
{
    // first three steps mask out the top bits of each
    // "slot", since in those cases our final sum could end up there.
    // we can stop doing this masking once the previous sum can't
    // shift over into the space on the low part of the next slot
    // that is needed for storage
    x = x - ((x >> 1) & 0x55555555u);
    x = (x & 0x33333333u) + ((x >> 2) & 0x33333333u);
    x = (x + (x >> 4)) & 0x0F0F0F0Fu;
    x = x + (x >> 8);
    x = x + (x >> 16);
    return x & 0x3Fu;
}

int __cdecl __builtin_popcountll_helper(unsigned long long x)
{
    x = x - ((x >> 1) & 0x5555555555555555ull);
    x = (x & 0x3333333333333333ull) + ((x >> 2) & 0x3333333333333333ull);
    x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0Full;
    x = x + (x >> 8);
    x = x + (x >> 16);
    x = x + (x >> 32);
    return x & 0x7Full;
}
