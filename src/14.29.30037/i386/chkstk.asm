        page    ,132
        title   chkstk - C stack checking routine
;***
;chkstk.asm - C stack checking routine
;
;       Copyright (c) Microsoft Corporation. All rights reserved.
;
;Purpose:
;       Provides support for automatic stack checking in C procedures
;       when stack checking is enabled.
;
;*******************************************************************************

.xlist
        include vcruntime.inc
.list

; size of a page of memory

_PAGESIZE_      equ     1000h


        CODESEG

page
;***
;_chkstk - check stack upon procedure entry
;
;Purpose:
;       Provide stack checking on procedure entry. Method is to simply probe
;       each page of memory required for the stack in descending order. This
;       causes the necessary pages of memory to be allocated via the guard
;       page scheme, if possible. In the event of failure, the OS raises the
;       _XCPT_UNABLE_TO_GROW_STACK exception.
;
;       NOTE:  Currently, the (EAX < _PAGESIZE_) code path falls through
;       to the "lastpage" label of the (EAX >= _PAGESIZE_) code path.  This
;       is small; a minor speed optimization would be to special case
;       this up top.  This would avoid the painful save/restore of
;       ecx and would shorten the code path by 4-6 instructions.
;
;Entry:
;       EAX = size of local frame
;
;Exit:
;       ESP = new stackframe, if successful
;
;Uses:
;       EAX
;
;Exceptions:
;       _XCPT_GUARD_PAGE_VIOLATION - May be raised on a page probe. NEVER TRAP
;                                    THIS!!!! It is used by the OS to grow the
;                                    stack on demand.
;       _XCPT_UNABLE_TO_GROW_STACK - The stack cannot be grown. More precisely,
;                                    the attempt by the OS memory manager to
;                                    allocate another guard page in response
;                                    to a _XCPT_GUARD_PAGE_VIOLATION has
;                                    failed.
;
;*******************************************************************************

public  _alloca_probe

_chkstk proc

_alloca_probe    =  _chkstk

        push    ecx

; Calculate new TOS.

        lea     ecx, [esp] + 8 - 4      ; TOS before entering function + size for ret value
        sub     ecx, eax                ; new TOS

; Handle allocation size that results in wraparound.
; Wraparound will result in StackOverflow exception.

        sbb     eax, eax                ; 0 if CF==0, ~0 if CF==1
        not     eax                     ; ~0 if TOS did not wrapped around, 0 otherwise
        and     ecx, eax                ; set to 0 if wraparound

        mov     eax, esp                ; current TOS
        and     eax, not ( _PAGESIZE_ - 1) ; Round down to current page boundary

cs10:
        cmp     ecx, eax                ; Is new TOS
        jb      short cs20              ; in probed page?
        mov     eax, ecx                ; yes.
        pop     ecx
        xchg    esp, eax                ; update esp
        mov     eax, dword ptr [eax]    ; get return address
        mov     dword ptr [esp], eax    ; and put it at new TOS
        ret

; Find next lower page and probe
cs20:
        sub     eax, _PAGESIZE_         ; decrease by PAGESIZE
        test    dword ptr [eax],eax     ; probe page.
        jmp     short cs10

_chkstk endp

        end
