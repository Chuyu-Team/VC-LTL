/***
*trnsctrl.h - routines that do special transfer of control
*
*       Copyright (c) 1993-1997, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Declaration of routines that do special transfer of control.
*       (and a few other implementation-dependant things).
*
*       Implementations of these routines are in assembly (very implementation
*       dependant).  Currently, these are implemented as naked functions with
*       inline asm.
*
*       [Internal]
*
****/

#if _MSC_VER > 1000
#pragma once
#endif  /* _MSC_VER > 1000 */

#ifndef _INC_TRNSCTRL
#define _INC_TRNSCTRL

#ifndef _CRTBLD
/*
* This is an internal C runtime header file. It is used when building
* the C runtimes only. It is not to be used as a public header file.
*/
#error ERROR: Use of C runtime library internal header file.
#endif  /* _CRTBLD */

extern "C" _VCRTIMP void** __cdecl __current_exception();
extern "C" _VCRTIMP int* __cdecl __processing_throw();
extern "C" _VCRTIMP void** __cdecl __current_exception_context();

#define _pCurrentException (*(EHExceptionRecord**)__current_exception())

#define __ProcessingThrow (*__processing_throw())

#define _pCurrentExContext (*(CONTEXT**)__current_exception_context())

#if _M_MRX000 >= 4000

typedef struct FrameInfo {
	PULONG                          pEstablisherFrame;
	PRUNTIME_FUNCTION       pFunctionEntry;
	CONTEXT*                        pExitContext;
	struct FrameInfo        *pNext;

} FRAMEINFO;

extern FRAMEINFO*       _CreateFrameInfo(FRAMEINFO*, DispatcherContext*, PULONG, CONTEXT*);
extern CONTEXT*         _FindAndUnlinkFrame(PVOID, FRAMEINFO*);
extern VOID                     _JumpToContinuation(ULONG, CONTEXT*);
extern PVOID            _OffsetToAddress(ptrdiff_t, PULONG, ULONG);

extern "C" VOID         _UnwindNestedFrames(EHRegistrationNode*, EHExceptionRecord*, CONTEXT*);
extern "C" VOID         _NLG_Notify(PVOID, PVOID);
extern "C" PVOID        _CallSettingFrame(PVOID, PULONG, ULONG);
extern "C" PVOID        _CallCatchBlock2(EHRegistrationNode*, FuncInfo*, PVOID, ULONG, ULONG);
extern "C" BOOL         _CallSETranslator(EHExceptionRecord*, EHRegistrationNode*, CONTEXT*, DispatcherContext*, FuncInfo*, ULONG, EHRegistrationNode*, ULONG);
extern "C" VOID         _EHRestoreContext(CONTEXT* pContext);
extern "C" CONTEXT*     _GetUnwindContext(VOID);
extern "C" VOID         _MoveContext(CONTEXT* pTarget, CONTEXT* pSource);
extern TryBlockMapEntry *_GetRangeOfTrysToCheck(EHRegistrationNode *, FuncInfo *, int, __ehstate_t, unsigned *, unsigned *);
extern VOID             _DestructExceptionObject(EHExceptionRecord *, BOOLEAN);

#define _CallMemberFunction0(pthis, pmfn)                               (*(VOID(*)(PVOID))pmfn)(pthis)
#define _CallMemberFunction1(pthis, pmfn, pthat)                (*(VOID(*)(PVOID, PVOID))pmfn)(pthis, pthat)
#define _CallMemberFunction2(pthis, pmfn, pthat, val2) (*(VOID(*)(PVOID, PVOID, int))pmfn)(pthis, pthat, val2)

#define UNWINDHELP(base, offset, index)                                   (((char*)base) + (int)offset)[index]

#elif defined (_M_IA64)

typedef struct FrameInfo {
	EHRegistrationNode      EstablisherFrame;
	PRUNTIME_FUNCTION       pFunctionEntry;
	CONTEXT*                pExitContext;
	__ehstate_t             State;
	struct FrameInfo        *pNext;

} FRAMEINFO;

extern FRAMEINFO*       _CreateFrameInfo(FRAMEINFO*, DispatcherContext*, EHRegistrationNode*, CONTEXT*, __ehstate_t);
extern CONTEXT*         _FindAndUnlinkFrame(PVOID, FRAMEINFO*);
extern FRAMEINFO*       _FindFrameInfo(DispatcherContext*, FRAMEINFO*);
extern VOID             _JumpToContinuation(ULONG, CONTEXT*, EHExceptionRecord*);
extern BOOL             _ExecutionInCatch(DispatcherContext*, FuncInfo*);

extern VOID             __FrameUnwindToEmptyState(EHRegistrationNode*, DispatcherContext*, FuncInfo*);
extern "C" VOID         _UnwindNestedFrames(EHRegistrationNode*, EHExceptionRecord*, CONTEXT*);
extern "C" VOID         _NLG_Notify(PVOID, PVOID);
extern "C" PVOID        _CallSettingFrame(void(*func)(void), EHRegistrationNode*, ULONG);
extern "C" BOOL         _CallSETranslator(EHExceptionRecord*, EHRegistrationNode*, CONTEXT*, DispatcherContext*, FuncInfo*, ULONG, EHRegistrationNode*);
extern "C" VOID         _EHRestoreContext(CONTEXT* pContext);
extern "C" VOID         _SaveUnwindContext(CONTEXT* pContext);
extern "C" CONTEXT*     _GetUnwindContext(VOID);
extern "C" VOID         _MoveContext(CONTEXT* pTarget, CONTEXT* pSource);
extern TryBlockMapEntry *_GetRangeOfTrysToCheck(EHRegistrationNode *, FuncInfo *, int, __ehstate_t, unsigned *, unsigned *, DispatcherContext*);
extern VOID             _DestructExceptionObject(EHExceptionRecord *, BOOLEAN);
extern EHRegistrationNode *_GetEstablisherFrame(EHRegistrationNode*, DispatcherContext*, FuncInfo*, EHRegistrationNode*);

#define _CallMemberFunction0(pthis, pmfn)               (*(VOID(*)(PVOID))pmfn)(pthis)
#define _CallMemberFunction1(pthis, pmfn, pthat)        (*(VOID(*)(PVOID, PVOID))pmfn)(pthis, pthat)
#define _CallMemberFunction2(pthis, pmfn, pthat, val2) (*(VOID(*)(PVOID, PVOID, int))pmfn)(pthis, pthat, val2)

#define UNWINDSTATE(base, offset) *((int*)((char*)base + offset))
#define UNWINDTRYBLOCK(base, offset) *((int*)((char*)base + offset + sizeof(int)))
#define UNWINDHELP(base, offset) *((__int64*)((char*)base + offset))

#define OffsetToAddress(off, FP)  (void*)(((char*)FP) + off)

#elif defined (_M_ALPHA)

//
// For Debugger handling of stepping with non-local gotos
//

extern "C" VOID     __NLG_Notify(PVOID, PVOID, ULONG);

//
// For calling funclets (including the catch)
//
extern "C" void * _CallSettingFrame(
	void *funcAddress,
	void *realFP,
	unsigned long NLGCode
);

extern void _JumpToContinuation(
	void               *TargetIp,    // The target address
	EHRegistrationNode *TargetFrame  // The target virtual frame ptr
);

//
// For calling member functions:
//
extern "C" void _CallMemberFunction0(void *pthis, void *pmfn);
extern "C" void _CallMemberFunction1(void *pthis, void *pmfn, void *pthat);
extern "C" void _CallMemberFunction2(void *pthis, void *pmfn, void *pthat, int val2);

//
// Translate an frame relative offset to a hard address based on address of
// a frame pointer (real or virtual).
//
#define OffsetToAddress(off, FP)  (void*)(((char*)FP) + off)

//
// Call RtlUnwind in a returning fashion
//
extern "C" VOID _UnwindNestedFrames(
	IN EHRegistrationNode *TargetFrame,
	IN EHExceptionRecord *ExceptionRecord
);

//
// Ditto the SE translator
//
BOOL _CallSETranslator(EHExceptionRecord*, EHRegistrationNode*, void*,
	DispatcherContext*, FuncInfo*, int,
	EHRegistrationNode*);

extern TryBlockMapEntry *_GetRangeOfTrysToCheck(FuncInfo *, int, __ehstate_t, unsigned *, unsigned *);
extern VOID _DestructExceptionObject(EHExceptionRecord *, BOOLEAN);
#elif defined (_M_IX86)


typedef struct FrameInfo {
	void                    *pExceptionObject;
	struct FrameInfo        *pNext;

} FRAMEINFO;

//
// For calling funclets (including the catch)
//
extern "C" void * __stdcall _CallSettingFrame(void *, EHRegistrationNode *, unsigned long);
extern void   __stdcall _JumpToContinuation(void *, EHRegistrationNode *);

//
// For calling member functions:
//
__forceinline void __stdcall _CallMemberFunction0(void *pthis, void *pmfn)
{
	__asm
	{
		mov		ecx, pthis
		call	pmfn
	}
}

__forceinline void __stdcall _CallMemberFunction1(void *pthis, void *pmfn, void *pthat)
{
	__asm
	{
		push    pthat
		mov     ecx, pthis
		call    pmfn
	}
}

__forceinline void __stdcall _CallMemberFunction2(void *pthis, void *pmfn, void *pthat, int val2)
{
	__asm
	{
		push	val2
		push	pthat
		mov		ecx, pthis
		call	pmfn
	}
}

//
// Translate an ebp-relative offset to a hard address based on address of
// registration node:
//
#if !CC_EXPLICITFRAME
#define OffsetToAddress(off, RN)\
(void*)((char*)RN\
	+ FRAME_OFFSET\
	+ off)
#else  /* !CC_EXPLICITFRAME */
#define OffsetToAddress(off, RN)      (void*)(((char*)RN->frame) + off)
#endif  /* !CC_EXPLICITFRAME */

//
// Call RtlUnwind in a returning fassion
//
extern void __stdcall _UnwindNestedFrames(EHRegistrationNode*, EHExceptionRecord*);

//
// Do the nitty-gritty of calling the catch block safely
//
void *_CallCatchBlock2(EHRegistrationNode *, FuncInfo*, void*, int, unsigned long);

//
// Ditto the SE translator
//
BOOL _CallSETranslator(EHExceptionRecord*, EHRegistrationNode*, void*, DispatcherContext*, FuncInfo*, int, EHRegistrationNode*);

extern TryBlockMapEntry *_GetRangeOfTrysToCheck(FuncInfo *, int, __ehstate_t, unsigned *, unsigned *);
extern VOID _DestructExceptionObject(EHExceptionRecord *, BOOLEAN);

#elif defined(_M_AMD64) || defined(_M_ARM64) || defined(_M_ARM)
typedef struct FrameInfo {
	void             *pExceptionObject;
	struct FrameInfo *pNext;

} FRAMEINFO;

#define _CallMemberFunction0(pthis, pmfn)               (*(VOID(*)(PVOID))pmfn)(pthis)
#define _CallMemberFunction1(pthis, pmfn, pthat)        (*(VOID(*)(PVOID, PVOID))pmfn)(pthis, pthat)
#define _CallMemberFunction2(pthis, pmfn, pthat, val2) (*(VOID(*)(PVOID, PVOID, int))pmfn)(pthis, pthat, val2)

#define OffsetToAddress(off, FP)  (void*)(((char*)FP) + off)

#elif defined (_M_MPPC) || defined (_M_PPC)
typedef struct FrameInfo {
	PULONG pEstablisherFrame;
	ULONG ControlPc;
	PRUNTIME_FUNCTION pFunctionEntry;
	CONTEXT *pExitContext;
	__ehstate_t state;
	struct FrameInfo *pNext;

} FRAMEINFO;

extern "C" PVOID _CallSettingFrame(PVOID, PULONG, ULONG);
extern "C" VOID _GetStackLimits(PULONG, PULONG);
extern EHRegistrationNode *_GetEstablisherFrame(DispatcherContext *, int *);
extern FRAMEINFO *_CreateFrameInfo(FRAMEINFO *, DispatcherContext *, PULONG, CONTEXT *, __ehstate_t);
extern "C" VOID _JumpToContinuation(ULONG, CONTEXT *);
extern "C" VOID _UnwindNestedFrames(EHRegistrationNode *, EHExceptionRecord *, CONTEXT *);
extern CONTEXT *_FindAndUnlinkFrame(PVOID, FRAMEINFO *);
extern FRAMEINFO *_FindFrameInfo(ULONG, FRAMEINFO *);
extern VOID __FrameUnwindToEmptyState(EHRegistrationNode *, DispatcherContext *, FuncInfo *);
extern BOOL _UnwindNestedCatch(EHRegistrationNode *, DispatcherContext *);

extern BOOL _CallSETranslator(EHExceptionRecord *, EHRegistrationNode *, CONTEXT *, DispatcherContext *, FuncInfo *, ULONG, EHRegistrationNode *);
extern TryBlockMapEntry *_GetRangeOfTrysToCheck(FuncInfo *, int, __ehstate_t, unsigned *, unsigned *);
extern VOID _DestructExceptionObject(EHExceptionRecord *, BOOLEAN);

#define OffsetToAddress(off, pRN)
((void *)((char *)(pRN)+(off)))
#define _CallMemberFunction0(pthis, pmfn)
(*(VOID(*)(PVOID))(pmfn))(pthis)
#define _CallMemberFunction1(pthis, pmfn, pthat)
(*(VOID(*)(PVOID, PVOID))(pmfn))(pthis, pthat)
#define _CallMemberFunction2(pthis, pmfn, pthat, val2)
(*(VOID(*)(PVOID, PVOID, int))(pmfn))(pthis, pthat, val2)

#if defined (_M_MPPC)
extern VOID _AllocExceptionData(PEXCEPTION_RECORD *, PCONTEXT *);
extern VOID _FreeExceptionData(EHExceptionRecord *);
extern ULONG _ConvertCFAddress(ULONG, DispatcherContext *, int);

#define CFREL_TO_ABS            0
#define ABS_TO_CFREL            1

#define TOC_LINK(-1)
#else  /* defined (_M_MPPC) */
#define TOC_LINK                2
#endif  /* defined (_M_MPPC) */

#define SET_UNWIND_FLAG(rTOC)   ((rTOC) |= 1)
#define CLEAR_UNWIND_FLAG(rTOC) ((rTOC) &= ~1)
#define CHECK_UNWIND_FLAG(rTOC) (((rTOC) & 1) != 0)
#define SET_UNWIND_FRAME(pRN)   SET_UNWIND_FLAG(((PULONG)(pRN))[TOC_LINK])
#define CHECK_UNWIND_FRAME(pRN) CHECK_UNWIND_FLAG(((PULONG)(pRN))[TOC_LINK])
#else  /* defined (_M_MPPC) || defined (_M_PPC) */

#pragma message("Special transfer of control routines not defined for this platform")

#endif  /* defined (_M_MPPC) || defined (_M_PPC) */

#endif  /* _INC_TRNSCTRL */
