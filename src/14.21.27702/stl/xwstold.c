// _WStold function
#include "xmath.h"
#include "xxlftype.h"
#include "xxwctype.h"
#include <wchar.h>

_CRTIMP2_PURE FTYPE __CLRCALL_PURE_OR_CDECL _WStoldx(const CTYPE* s, CTYPE** endptr, long pten, int* perr)
#include "xxstod.h"

    _CRTIMP2_PURE FTYPE __CLRCALL_PURE_OR_CDECL
    _WStold(const CTYPE* s, CTYPE** endptr, long pten) { // convert string, discard error code
    return _WStoldx(s, endptr, pten, 0);
}

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
