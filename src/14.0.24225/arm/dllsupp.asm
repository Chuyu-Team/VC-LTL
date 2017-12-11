;***
;dllsupp.asm - Definitions of public constants
;
;       Copyright (c) Microsoft Corporation. All rights reserved.
;
;Purpose:
;       Provides definitions for public constants (absolutes) that are
;       'normally' defined in objects in the C library, but must be defined
;       here for clients of msvcr*.dll.  These constants are:
;
;                           _fltused
;
;*******************************************************************************

       TTL  "Absolutele symbol definitions for CRT build used for object selection in static builds"

    EXPORT _fltused
_fltused   EQU     1

    END
