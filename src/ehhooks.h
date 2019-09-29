#pragma once
#include <vcruntime_internal.h>
#include <corecrt_internal.h>
#include <trnsctrl.h>

#define EHTRACE_ENTER_FMT1(...)
#define EHTRACE_ENTER_FMT2(...)
#define EHTRACE_FMT1(...)
#define EHTRACE_FMT2(...)

#define EHTRACE_ENTER
#define EHTRACE_EXIT
#define EHTRACE_EXCEPT(x) x
#define EHTRACE_HANDLER_EXIT(x)

#define EHTRACE_RESET

#define DASSERT(x)
#define _VCRT_VERIFY(x)

#define _ValidateRead(ptr)    (ptr != NULL)
#define _ValidateWrite(ptr)   (ptr != NULL)
#define _ValidateExecute(ptr) (ptr != NULL)


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


#if _CRT_NTDDI_MIN >= NTDDI_WIN6
#define __pSETranslator   (_se_translator_function)(__acrt_getptd()->VistaOrLater_msvcrt._translator)
#else
#define __pSETranslator   (_se_translator_function)(__LTL_GetOsMinVersion() < 0x00060000 ? (__acrt_getptd()->XP_msvcrt._translator) : \
          (__acrt_getptd()->VistaOrLater_msvcrt._translator))
#endif


#ifdef _EH_RELATIVE_FUNCINFO
template <class T>
BOOL _CallSETranslator(
    EHExceptionRecord    *pExcept,    // The exception to be translated
    EHRegistrationNode   *pRN,        // Dynamic info of function with catch
    CONTEXT              *pContext,   // Context info
    DispatcherContext    *pDC,        // More dynamic info of function with catch (ignored)
    typename T::FuncInfo *pFuncInfo,  // Static info of function with catch
    ULONG                CatchDepth,  // How deeply nested in catch blocks are we?
    EHRegistrationNode   *pMarkerRN   // Marker for parent context
    );
#endif

#if defined(_M_X64)
EXTERN_C void* __cdecl _CallSettingFrame(
	void*               handler,
	EHRegistrationNode* pEstablisher,
	ULONG               NLG_CODE
    );

EXTERN_C void* __cdecl _CallSettingFrameEncoded(
	void*               handler,
	EHRegistrationNode  pEstablisher,
	void*               object,
	ULONG               NLG_CODE
    );

EXTERN_C void* __cdecl _CallSettingFrame_LookupContinuationIndex(
    void*               handler,
    EHRegistrationNode  *pEstablisher,
    ULONG               NLG_CODE
	);

EXTERN_C void* __cdecl _CallSettingFrame_NotifyContinuationAddr(
    void*               handler,
    EHRegistrationNode  *pEstablisher
	);

#elif defined(_M_ARM_NT) || defined(_M_ARM64) || defined(_CHPE_X86_ARM64_EH_)
EXTERN_C void* __cdecl _CallSettingFrame(
    void*               handler,
    EHRegistrationNode* pRN,
    PULONG              pNonVolatiles,
    ULONG               NLG_CODE
	);
#endif
