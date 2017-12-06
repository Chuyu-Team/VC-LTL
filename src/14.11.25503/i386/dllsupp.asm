;***
;dllsupp.asm - Definitions of public constants
;
;       Copyright (c) Microsoft Corporation. All rights reserved.
;
;Purpose:
;       Provides definitions for public constants (absolutes) that are
;       'normally' defined in objects in the C library, but must be defined
;       here for clients of the CRT DLL.  These constants are:
;
;                           _except_list
;                           _ldused
;
;*******************************************************************************

        .686
        .model  flat,c

; offset, with respect to FS, of pointer to currently active exception handler.
; referenced by compiler generated code for SEH and by _setjmp().

        public  _except_list
_except_list    equ     0

        public  _ldused
_ldused         equ     9876h

        end
