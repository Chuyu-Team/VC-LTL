// xrngabort: abort on precondition failure in TR1 random number generators
#include <yvals.h>

#include <stdio.h>
#include <stdlib.h>

_STD_BEGIN

    // TRANSITION: _Rng_abort is preserved for ABI compatibility

    [[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL
    _Rng_abort(_In_z_ const char* _Msg);

void __CLRCALL_PURE_OR_CDECL _Rng_abort(const char* msg) { // abort on precondition failure
    _CSTD fputs(msg, stderr);
    _CSTD fputc('\n', stderr);
    _CSTD abort();
}
_STD_END

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
