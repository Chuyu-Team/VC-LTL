/***
*ehprolg3.c - Define the _EH_prolog3* compiler helpers for unaligned frames
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       EH prologue helpers for the EH security cookie cases, where the cookie
*       must be initialized before the EH node is installed.  Also defines the
*       complementary EH epilogue helpers.
*
*       The helpers should be compiled /Gy, at least in retail CRTs, so they
*       will be compiled as comdats, and only the ones used will be pulled into
*       a release build.
*
*       These helpers are for setting up normal frames which do not require
*       8-byte (or more) alignment of the local stack allocation.
*
*       There are 4 different prologue helpers, to deal with the various
*       combinations of a 12-byte -vs- 16-byte exception registration node and
*       the presence or absence of a /GS local security cookie.
*
*       _EH_prolog3
*       _EH_prolog3_catch
*       _EH_prolog3_GS
*       _EH_prolog3_catch_GS
*
*       There are 3 different epilogue helpers:
*
*       _EH_epilog3
*       _EH_epilog3_GS
*       _EH_epilog3_catch_GS
****/

#include <vcruntime_internal.h>

/*
 * Ignore warnings about unreferenced formal parameters and unsafe FS:0
 * assignments.
 */

#pragma warning(disable:4100 4733)
/***
*_EH_prolog3 - Set up C++ EH call frame variation
*
*Purpose:
*   Sets up the call frame for a C++ EH function that:
*   + uses a 12-byte EH node (function has unwinds, no catch blocks)
*   + does not have a local /GS cookie
*   + uses an EBP frame (no FPO)
*   + does not need dynamic stack alignment
*
*Entry:
*   EAX = address of EH handler thunk.
*   LocalAllocation = local stack allocation requirement (hereafter, 'N').
*
*   Incoming stack frame has:
*       [ESP+8]  callee's return address
*       [ESP+4]  local stack allocation requirement
*       [ESP+0]  _EH_prolog3's return address
*
*Exit:
*   EAX = destroyed
*   EBP = ESP on entry + 4
*   ESP = EBP on exit - 28 - N
*   All other registers except CC preserved
*
*   FS:[0] = set to EBP on exit - 12 to create new link in the EH chain
*
*   Stack frame has been set up as follows:
*       [EBP+4]     (entry [ESP+8]) callee's return address
*       [EBP+0]     saved EBP
*       [EBP-4]     EH record state index, initialized to -1
*       [EBP-8]     address of EH handler thunk
*       [EBP-12]    saved FS:[0]
*       ...
*       [EBP-12-N]  base of locals allocation
*       [EBP-16-N]  saved EBX
*       [EBP-20-N]  saved ESI
*       [EBP-24-N]  saved EDI
*       [EBP-28-N]  local EH cookie, XORed with value of EBP on exit
*
*******************************************************************************/

extern void __declspec(naked)
_EH_prolog3(
    unsigned long LocalAllocation
    )
{
    __asm
    {
        ; stack has:
        ;   callee's return addr
        ;   allocation size
        ;   ret addr   <== ESP

        push    eax             ; save handler thunk address in its proper
                                ; final position, free EAX for use
        push    fs:[0]          ; set link to next node in EH chain

        ; stack has:
        ;   callee's return addr
        ;   allocation size
        ;   ret addr
        ;   handler thunk address
        ;   saved FS:[0] <== ESP

        lea     eax, [esp+12]   ; calculate return value for EBP
        sub     esp, [esp+12]   ; allocate locals allocation area
        push    ebx             ; save the callee saves except for EBP
        push    esi
        push    edi
        mov     [eax], ebp      ; save old EBP in final location
        mov     ebp, eax        ; establish new EBP frame pointer

        ; stack has:
        ;   callee's return addr
        ;   saved EBP   <== EBP
        ;   ret addr
        ;   handler thunk address
        ;   saved FS:[0]
        ;   locals allocation area
        ;   saved EBX
        ;   saved ESI
        ;   saved EDI   <== ESP

        mov     eax, __security_cookie
        xor     eax, ebp        ; generate local EH cookie
        push    eax
        push    [ebp-4]         ; move the return address to its final position
        mov     dword ptr [ebp-4], -1 ; initialize the EH state index

        ; stack has:
        ;   callee's return addr
        ;   saved EBP   <== EBP
        ;   initial EH state index of -1
        ;   handler thunk address
        ;   saved FS:[0]
        ;   locals allocation area
        ;   saved EBX
        ;   saved ESI
        ;   saved EDI
        ;   local EH security cookie
        ;   ret addr    <== ESP

        lea     eax, [ebp-12]   ; all done - link in the EH node
        mov     fs:[0], eax

        ret
    }
}

/***
*_EH_prolog3_catch - Set up C++ EH call frame variation
*
*Purpose:
*   Sets up the call frame for a C++ EH function that:
*   + uses a 16-byte EH node (function has catch blocks)
*   + does not have a local /GS cookie
*   + uses an EBP frame (no FPO)
*   + does not need dynamic stack alignment
*
*Entry:
*   EAX = address of EH handler thunk.
*   LocalAllocation = local stack allocation requirement (hereafter, 'N').
*       This includes 4 bytes for the 4th field (the saved ESP) in the EH
*       registration node.
*
*   Incoming stack frame has:
*       [ESP+8]  callee's return address
*       [ESP+4]  local stack allocation requirement
*       [ESP+0]  _EH_prolog3_catch's return address
*
*Exit:
*   EAX = destroyed
*   EBP = ESP on entry + 4
*   ESP = EBP on exit - 28 - N
*   All other registers except CC preserved
*
*   FS:[0] = set to EBP on exit - 12 to create new link in the EH chain
*
*   Stack frame has been set up as follows:
*       [EBP+4]     (entry [ESP+8]) callee's return address
*       [EBP+0]     saved EBP
*       [EBP-4]     EH record state index, initialized to -1
*       [EBP-8]     address of EH handler thunk
*       [EBP-12]    saved FS:[0]
*       [EBP-16]    saved final ESP return value
*       ...
*       [EBP-12-N]  base of locals allocation
*       [EBP-16-N]  saved EBX
*       [EBP-20-N]  saved ESI
*       [EBP-24-N]  saved EDI
*       [EBP-28-N]  local EH cookie, XORed with value of EBP on exit
*
*******************************************************************************/

extern void __declspec(naked)
_EH_prolog3_catch(
    unsigned long LocalAllocation
    )
{
    __asm
    {
        ; stack has:
        ;   callee's return addr
        ;   allocation size
        ;   ret addr   <== ESP

        push    eax             ; save handler thunk address in its proper
                                ; final position, free EAX for use
        push    fs:[0]          ; set link to next node in EH chain

        ; stack has:
        ;   callee's return addr
        ;   allocation size
        ;   ret addr
        ;   handler thunk address
        ;   saved FS:[0] <== ESP

        lea     eax, [esp+12]   ; calculate return value for EBP
        sub     esp, [esp+12]   ; allocate end of EH node and the locals
                                ; allocation area
        push    ebx             ; save the callee saves except for EBP
        push    esi
        push    edi
        mov     [eax], ebp      ; save old EBP in final location
        mov     ebp, eax        ; establish new EBP frame pointer

        ; stack has:
        ;   callee's return addr
        ;   saved EBP   <== EBP
        ;   ret addr
        ;   handler thunk address
        ;   saved FS:[0]
        ;   (uninitialized) saved ESP field of EH node
        ;   locals allocation area
        ;   saved EBX
        ;   saved ESI
        ;   saved EDI   <== ESP

        mov     eax, __security_cookie
        xor     eax, ebp        ; generate local EH cookie
        push    eax
        mov     [ebp-16], esp   ; initialize the EH node's saved ESP field
        push    [ebp-4]         ; move the return address to its final position
        mov     dword ptr [ebp-4], -1 ; initialize the EH state index

        ; stack has:
        ;   callee's return addr
        ;   saved EBP   <== EBP
        ;   initial EH state index of -1
        ;   handler thunk address
        ;   saved FS:[0]
        ;   saved ESP field of EH node
        ;   locals allocation area
        ;   saved EBX
        ;   saved ESI
        ;   saved EDI
        ;   local EH security cookie
        ;   ret addr    <== ESP

        lea     eax, [ebp-12]   ; all done - link in the EH node
        mov     fs:[0], eax

        ret
    }
}

/***
*_EH_prolog3_GS - Set up C++ EH call frame variation
*
*Purpose:
*   Sets up the call frame for a C++ EH function that:
*   + uses a 12-byte EH node (function has unwinds, no catch blocks)
*   + has a local /GS cookie
*   + uses an EBP frame (no FPO)
*   + does not need dynamic stack alignment
*
*Entry:
*   EAX = address of EH handler thunk.
*   LocalAllocation = local stack allocation requirement (hereafter, 'N').
*       This includes 4 bytes for the GS cookie.
*
*   Incoming stack frame has:
*       [ESP+8]  callee's return address
*       [ESP+4]  local stack allocation requirement
*       [ESP+0]  _EH_prolog3_GS's return address
*
*Exit:
*   EAX = destroyed
*   EBP = ESP on entry + 4
*   ESP = EBP on exit - 28 - N
*   All other registers except CC preserved
*
*   FS:[0] = set to EBP on exit - 12 to create new link in the EH chain
*
*   Stack frame has been set up as follows:
*       [EBP+4]     (entry [ESP+8]) callee's return address
*       [EBP+0]     saved EBP
*       [EBP-4]     EH record state index, initialized to -1
*       [EBP-8]     address of EH handler thunk
*       [EBP-12]    saved FS:[0]
*       [EBP-16]    local GS cookie, XORed with value of EBP on exit
*       ...
*       [EBP-12-N]  base of locals allocation
*       [EBP-16-N]  saved EBX
*       [EBP-20-N]  saved ESI
*       [EBP-24-N]  saved EDI
*       [EBP-28-N]  local EH cookie, XORed with value of EBP on exit
*
*******************************************************************************/

extern void __declspec(naked)
_EH_prolog3_GS(
    unsigned long LocalAllocation
    )
{
    __asm
    {
        ; stack has:
        ;   callee's return addr
        ;   allocation size
        ;   ret addr   <== ESP

        push    eax             ; save handler thunk address in its proper
                                ; final position, free EAX for use
        push    fs:[0]          ; set link to next node in EH chain

        ; stack has:
        ;   callee's return addr
        ;   allocation size
        ;   ret addr
        ;   handler thunk address
        ;   saved FS:[0] <== ESP

        lea     eax, [esp+12]   ; calculate return value for EBP
        sub     esp, [esp+12]   ; allocate space for the GS cookie, and the
                                ; locals allocation area
        push    ebx             ; save the callee saves except for EBP
        push    esi
        push    edi
        mov     [eax], ebp      ; save old EBP in final location
        mov     ebp, eax        ; establish new EBP frame pointer

        ; stack has:
        ;   callee's return addr
        ;   saved EBP   <== EBP
        ;   ret addr
        ;   handler thunk address
        ;   saved FS:[0]
        ;   (uninitialized) local GS security cookie
        ;   locals allocation area
        ;   saved EBX
        ;   saved ESI
        ;   saved EDI   <== ESP

        mov     eax, __security_cookie
        xor     eax, ebp        ; generate local EH and GS cookies
        push    eax
        mov     [ebp-16], eax
        push    [ebp-4]         ; move the return address to its final position
        mov     dword ptr [ebp-4], -1 ; initialize the EH state index

        ; stack has:
        ;   callee's return addr
        ;   saved EBP   <== EBP
        ;   initial EH state index of -1
        ;   handler thunk address
        ;   saved FS:[0]
        ;   saved ESP field of EH node
        ;   local GS security cookie
        ;   locals allocation area
        ;   saved EBX
        ;   saved ESI
        ;   saved EDI
        ;   local EH security cookie
        ;   ret addr    <== ESP

        lea     eax, [ebp-12]   ; all done - link in the EH node
        mov     fs:[0], eax

        ret
    }
}

/***
*_EH_prolog3_catch_GS - Set up C++ EH call frame variation
*
*Purpose:
*   Sets up the call frame for a C++ EH function that:
*   + uses a 16-byte EH node (function has catch blocks)
*   + has a local /GS cookie
*   + uses an EBP frame (no FPO)
*   + does not need dynamic stack alignment
*
*Entry:
*   EAX = address of EH handler thunk.
*   LocalAllocation = local stack allocation requirement (hereafter, 'N').
*       This includes 4 bytes for the 4th field (the saved ESP) in the EH
*       registration node, and 4 bytes for the GS cookie.
*
*   Incoming stack frame has:
*       [ESP+8]  callee's return address
*       [ESP+4]  local stack allocation requirement
*       [ESP+0]  _EH_prolog3_catch_GS's return address
*
*Exit:
*   EAX = destroyed
*   EBP = ESP on entry + 4
*   ESP = EBP on exit - 28 - N
*   All other registers except CC preserved
*
*   FS:[0] = set to EBP on exit - 12 to create new link in the EH chain
*
*   Stack frame has been set up as follows:
*       [EBP+4]     (entry [ESP+8]) callee's return address
*       [EBP+0]     saved EBP
*       [EBP-4]     EH record state index, initialized to -1
*       [EBP-8]     address of EH handler thunk
*       [EBP-12]    saved FS:[0]
*       [EBP-16]    saved final ESP return value
*       [EBP-20]    local GS cookie, XORed with value of EBP on exit
*       ...
*       [EBP-12-N]  base of locals allocation
*       [EBP-16-N]  saved EBX
*       [EBP-20-N]  saved ESI
*       [EBP-24-N]  saved EDI
*       [EBP-28-N]  local EH cookie, XORed with value of EBP on exit
*
*******************************************************************************/

extern void __declspec(naked)
_EH_prolog3_catch_GS(
    unsigned long LocalAllocation
    )
{
    __asm
    {
        ; stack has:
        ;   callee's return addr
        ;   allocation size
        ;   ret addr   <== ESP

        push    eax             ; save handler thunk address in its proper
                                ; final position, free EAX for use
        push    fs:[0]          ; set link to next node in EH chain

        ; stack has:
        ;   callee's return addr
        ;   allocation size
        ;   ret addr
        ;   handler thunk address
        ;   saved FS:[0] <== ESP

        lea     eax, [esp+12]   ; calculate return value for EBP
        sub     esp, [esp+12]   ; allocate end of EH node, space for the GS
                                ; cookie, and the locals allocation area
        push    ebx             ; save the callee saves except for EBP
        push    esi
        push    edi
        mov     [eax], ebp      ; save old EBP in final location
        mov     ebp, eax        ; establish new EBP frame pointer

        ; stack has:
        ;   callee's return addr
        ;   saved EBP   <== EBP
        ;   ret addr
        ;   handler thunk address
        ;   saved FS:[0]
        ;   (uninitialized) saved ESP field of EH node
        ;   (uninitialized) local GS security cookie
        ;   locals allocation area
        ;   saved EBX
        ;   saved ESI
        ;   saved EDI   <== ESP

        mov     eax, __security_cookie
        xor     eax, ebp        ; generate local EH and GS cookies
        push    eax
        mov     [ebp-20], eax
        mov     [ebp-16], esp   ; initialize the EH node's saved ESP field
        push    [ebp-4]         ; move the return address to its final position
        mov     dword ptr [ebp-4], -1 ; initialize the EH state index

        ; stack has:
        ;   callee's return addr
        ;   saved EBP   <== EBP
        ;   initial EH state index of -1
        ;   handler thunk address
        ;   saved FS:[0]
        ;   saved ESP field of EH node
        ;   local GS security cookie
        ;   locals allocation area
        ;   saved EBX
        ;   saved ESI
        ;   saved EDI
        ;   local EH security cookie
        ;   ret addr    <== ESP

        lea     eax, [ebp-12]   ; all done - link in the EH node
        mov     fs:[0], eax

        ret
    }
}

/***
*_EH_epilog3 - Tear down C++ EH call frame variation
*
*Purpose:
*   Destroy a call frame set up by _EH_prolog3 or _EH_prolog3_catch.
*
*Entry:
*   ESP = value it had on return from _EH_prolog3 or _EH_prolog3_catch
*   EBP = value it had on return from _EH_prolog3 or _EH_prolog3_catch
*
*Exit:
*   ESP = pointer to the callee's return address
*   EBX, ESI, EDI, EBP restored from frame
*   ECX destroyed
*   All other registers except CC preserved
*
*******************************************************************************/

extern void __declspec(naked)
_EH_epilog3(
    void
    )
{
    __asm
    {
        mov     ecx, [ebp-12]   ; remove EH node from EH chain
        mov     fs:[0], ecx
        pop     ecx             ; save return address
        pop     edi             ; pop and ignore the EH cookie
        pop     edi             ; restore callee-saves
        pop     esi
        pop     ebx
        mov     esp, ebp        ; free stack
        pop     ebp             ; restore frame pointer
        push    ecx             ; push return address
        ret     0
    }
}

/***
*_EH_epilog3_GS - Tear down C++ EH call frame variation
*
*Purpose:
*   Destroy a call frame set up by _EH_prolog3_GS, after first checking that
*   the local /GS security cookie is correct.
*
*Entry:
*   ESP = value it had on return from _EH_prolog3_GS
*   EBP = value it had on return from _EH_prolog3_GS
*   GS cookie located at EBP-16
*
*Exit:
*   ESP = pointer to the callee's return address
*   EBX, ESI, EDI, EBP restored from frame
*   ECX destroyed
*   All other registers except CC preserved
*
*******************************************************************************/

extern void __declspec(naked)
_EH_epilog3_GS(
    void
    )
{
    __asm
    {
        mov     ecx, [ebp-16]   ; rematerialize the global cookie and check
        xor     ecx, ebp        ; * its validity
        call    __security_check_cookie
        jmp     _EH_epilog3     ; go tear down the frame
    }
}

/***
*_EH_epilog3_catch_GS - Tear down C++ EH call frame variation
*
*Purpose:
*   Destroy a call frame set up by _EH_prolog3_catch_GS, after first checking
*   that the local /GS security cookie is correct.
*
*Entry:
*   ESP = value it had on return from _EH_prolog3_catch_GS
*   EBP = value it had on return from _EH_prolog3_catch_GS
*   GS cookie located at EBP-20
*
*Exit:
*   ESP = pointer to the callee's return address
*   EBX, ESI, EDI, EBP restored from frame
*   ECX destroyed
*   All other registers except CC preserved
*
*******************************************************************************/

extern void __declspec(naked)
_EH_epilog3_catch_GS(
    void
    )
{
    __asm
    {
        mov     ecx, [ebp-20]   ; rematerialize the global cookie and check
        xor     ecx, ebp        ; * its validity
        call    __security_check_cookie
        jmp     _EH_epilog3     ; go tear down the frame
    }
}
