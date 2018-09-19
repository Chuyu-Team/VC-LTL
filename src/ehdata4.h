#pragma once


#include <corecrt_internal.h>
#include <ehdata.h>


typedef HandlerType HandlerType4;
#define RENAME_EH_EXTERN(x) x

//#include <ehhelpers.h>
#include <trnsctrl.h>



#if _EH_RELATIVE_OFFSETS

EXTERN_C uintptr_t __cdecl _GetImageBase();

EXTERN_C void __cdecl _SetImageBase(uintptr_t ImageBaseToRestore);

#endif

#if _EH_RELATIVE_TYPEINFO
EXTERN_C uintptr_t __cdecl _GetThrowImageBase();

EXTERN_C void __cdecl _SetThrowImageBase(uintptr_t NewThrowImageBase);

#endif


struct __FrameHandler3
{
	typedef HandlerType HandlerType;

	ESTypeList* getESTypes(FuncInfo* pFuncInfo);

	int TypeMatch(
		HandlerType   *pCatch,
		CatchableType *pCatchable,
		ThrowInfo     *pThrow
	);

	int BuildCatchObjectHelper(
		EHExceptionRecord *pExcept,
		void              *pRN,
		HandlerType      *pCatch,
		CatchableType     *pConv
	);

	void BuildCatchObject(
		EHExceptionRecord *pExcept,
		void *pRN,
		HandlerType *pCatch,
		CatchableType *pConv
	);
};

struct __FrameHandler4
{
	typedef HandlerType HandlerType;

	int TypeMatch(
		HandlerType   *pCatch,
		CatchableType *pCatchable,
		ThrowInfo     *pThrow
	);

	int BuildCatchObjectHelper(
		EHExceptionRecord *pExcept,
		void              *pRN,
		HandlerType4      *pCatch,
		CatchableType     *pConv
	);

	void BuildCatchObject(
		EHExceptionRecord *pExcept,
		void *pRN,
		HandlerType4 *pCatch,
		CatchableType *pConv
	);
};