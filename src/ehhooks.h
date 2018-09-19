#pragma once
#include <vcruntime_internal.h>
#include <corecrt_internal.h>
#include <trnsctrl.h>

#define EHTRACE_ENTER_FMT1(...)
#define EHTRACE_ENTER_FMT2(...)
#define EHTRACE_FMT2(...)

#define EHTRACE_ENTER
#define EHTRACE_EXIT
#define EHTRACE_EXCEPT(x) x
#define EHTRACE_HANDLER_EXIT(x)

#define EHTRACE_RESET

#define DASSERT(x)

#define _ValidateRead(ptr)    (ptr == NULL)
#define _ValidateWrite(ptr)   (ptr == NULL)
#define _ValidateExecute(ptr) (ptr == NULL)


#define RENAME_EH_EXTERN(x) x

#if _EH_RELATIVE_FUNCINFO

EXTERN_C uintptr_t __cdecl _GetImageBase();

EXTERN_C void __cdecl _SetImageBase(uintptr_t ImageBaseToRestore);
#endif

#if _EH_RELATIVE_TYPEINFO
EXTERN_C uintptr_t __cdecl _GetThrowImageBase();

EXTERN_C void __cdecl _SetThrowImageBase(uintptr_t NewThrowImageBase);
#endif


EXTERN_C _VCRTIMP FRAMEINFO * __cdecl _CreateFrameInfo(
	FRAMEINFO * pFrameInfo,
	PVOID       pExceptionObject
);

EXTERN_C _VCRTIMP void __cdecl _FindAndUnlinkFrame(
	FRAMEINFO * pFrameInfo
);


EXTERN_C _VCRTIMP BOOL __cdecl _IsExceptionObjectToBeDestroyed(
	PVOID pExceptionObject
);