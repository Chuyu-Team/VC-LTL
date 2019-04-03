// _WStod function
#include "xmath.h"
#include "xxdftype.h"
#include "xxwctype.h"
#include <stdlib.h>
#include <wchar.h>

_CRTIMP2_PURE FTYPE __CLRCALL_PURE_OR_CDECL _WStodx(const CTYPE* s, CTYPE** endptr, long pten, int* perr)
#include "xxstod.h"

    _CRTIMP2_PURE FTYPE __CLRCALL_PURE_OR_CDECL
    _WStod(const CTYPE* s, CTYPE** endptr, long pten) { // convert string, discard error code
    return _WStodx(s, endptr, pten, 0);
}

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
