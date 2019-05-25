;***
;mm.inc - macros to write memory model dependent code
;
;       Copyright (c) Microsoft Corporation.  All rights reserved.
;
;Purpose:
;       This file contains definitions of a number of macros which
;       make the writing of memory model dependent code for the
;       386 a little easier and more portable.
;
;*******************************************************************************

; Big/Little Endian Definitions for Long Integers

LOWORD  equ     [0]
HIWORD  equ     [4]
