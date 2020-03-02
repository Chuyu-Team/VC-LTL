
/*


因为Windows 2000的msvcrt.dll不支持一些Windows XP的一些函数，本obj用于补充缺失的CRT函数。

温馨提示：此obj仅用于VC-LTL XP模式，其他途径概不负责！

*/

#include <ehdata.h>
#include <eh.h>
#include "..\ThunksHelper.h"


extern "C" _VCRTIMP void** __cdecl __current_exception();
extern "C" _VCRTIMP int* __cdecl __processing_throw();
extern "C" _VCRTIMP void** __cdecl __current_exception_context();

#define _pCurrentException (*(EHExceptionRecord**)__current_exception())

#define __ProcessingThrow (*__processing_throw())

#define _pCurrentExContext (*(CONTEXT**)__current_exception_context())

static HMODULE __fastcall get_msvcrt()
{
    static HMODULE msvcrt_module = nullptr;

    if (msvcrt_module == nullptr)
    {
        auto tmp = GetModuleHandleW(L"msvcrt.dll");

        msvcrt_module = tmp ? tmp : (HMODULE)-1;
    }

    if (msvcrt_module == (HMODULE)-1)
    {
        return nullptr;
    }
    else
    {
        return msvcrt_module;
    }
}

#define __try_call_msvcrt_fun(__fun_name, ...)                   \
       static FARPROC pFun = nullptr;                            \
       if(pFun == nullptr)                                       \
       {                                                         \
           auto tmp = GetProcAddress(get_msvcrt(), #__fun_name); \
           pFun = tmp ? tmp : (FARPROC)-1;                       \
       }                                                         \
       if(pFun != (FARPROC)-1)                                   \
           return ((decltype(__fun_name)*)pFun)(__VA_ARGS__);



#include "align.hpp"
#include "initctype.hpp"

namespace downlevel
{

    //
    // For calling member functions:
    //
    __declspec(guard(ignore)) static inline void __stdcall _CallMemberFunction0(
        void* const pthis,
        void* const pmfn
        ) noexcept(false)
    {
        auto const OneArgFn = reinterpret_cast<void (__thiscall*)(void*)>(pmfn);
        OneArgFn(pthis);
    }

    //extern "C" void __cdecl __DestructExceptionObject(
    //    EHExceptionRecord * pExcept,         // The original exception record
    //    BOOLEAN fThrowNotAllowed            // TRUE if destructor not allowed to
    //                                        //   throw
    //    )
    //{
    //    if (pExcept == nullptr)
    //        return;

    //    if (PER_PTHROW(pExcept))
    //    {
    //        if (THROW_UNWINDFUNC(*PER_PTHROW(pExcept)) != 0)
    //        {
    //            __try
    //            {
    //                _CallMemberFunction0(PER_PEXCEPTOBJ(pExcept),
    //                    THROW_UNWINDFUNC(*PER_PTHROW(pExcept)));
    //            }
    //            __except (fThrowNotAllowed ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
    //            {
    //                terminate();
    //            }
    //        }
    //    }
    //}

    static DWORD _FilterSetCurrentException(EXCEPTION_POINTERS* pointers, BOOLEAN fThrowNotAllowed)
    {
        if (fThrowNotAllowed) {
            const auto eRecord = reinterpret_cast<EHExceptionRecord*>(pointers->ExceptionRecord);
            if (PER_IS_MSVC_EH(eRecord))
            {
                // Can't have new exceptions when we're unwinding due to another
                // exception.
                _pCurrentException = eRecord;
                _pCurrentExContext = pointers->ContextRecord;
                terminate();
            }
        }

        return EXCEPTION_CONTINUE_SEARCH;
    }

    extern "C" void __cdecl __DestructExceptionObject(
        EHExceptionRecord *pExcept,         // The original exception record
        BOOLEAN fThrowNotAllowed            // TRUE if destructor not allowed to
                                            //   throw
    ) {
        //static FARPROC pDestructExceptionObject = nullptr;

        ////优先尝试使用msvcrt.dll内置版本
        //if (pDestructExceptionObject == nullptr)
        //{
        //    auto pFun = GetProcAddress(get_msvcrt(), "__DestructExceptionObject");
        //    if (pFun)
        //    {
        //        pDestructExceptionObject = pFun;
        //    }
        //    else
        //    {
        //        pDestructExceptionObject = (FARPROC)-1;
        //    }
        //}

        //if (pDestructExceptionObject != (FARPROC)-1)
        //{
        //    ((decltype(__DestructExceptionObject)*)pDestructExceptionObject)(pExcept, fThrowNotAllowed);
        //    return;
        //}

        __try_call_msvcrt_fun(__DestructExceptionObject, pExcept, fThrowNotAllowed);

        // Ignore if not a C++ exception (since this is now called from
        // _except_handler3)
        if (pExcept == nullptr || !(PER_IS_MSVC_EH(pExcept)))
        {
            return;
        }

        //EHTRACE_FMT1("Destroying object @ 0x%p", PER_PEXCEPTOBJ(pExcept));

        /*UNDONE:Is this _SYSCRT specific */
    //#pragma prefast(suppress:__WARNING_REDUNDANT_POINTER_TEST, "Current definition of PER_CODE ensures that pExcept cannot be nullptr")
        if (PER_PTHROW(pExcept))  {
            if (THROW_UNWINDFUNC(*PER_PTHROW(pExcept)) != 0) {

                __try {

    #if _EH_RELATIVE_TYPEINFO
                    _CallMemberFunction0(PER_PEXCEPTOBJ(pExcept),
                        THROW_UNWINDFUNC_IB(*PER_PTHROW(pExcept),(ptrdiff_t)PER_PTHROWIB(pExcept)));
    #else
                    _CallMemberFunction0(PER_PEXCEPTOBJ(pExcept),
                        THROW_UNWINDFUNC(*PER_PTHROW(pExcept)));
    #endif

                } __except(_FilterSetCurrentException(GetExceptionInformation(), fThrowNotAllowed)) {}
            }
            else if (THROW_ISWINRT(*PER_PTHROW(pExcept))) {
                // Release if WinRT reference type exception
                IUnknown* const pUnknown = *static_cast<IUnknown**>(PER_PEXCEPTOBJ(pExcept));
                if (pUnknown) {
                    pUnknown->Release();
                }
            }
        }
    }
    _LCRT_DEFINE_IAT_SYMBOL(__DestructExceptionObject);

    extern "C" int __cdecl __wgetmainargs(
            _Out_ int* _Argc,
            _Outptr_result_buffer_(*_Argc) wchar_t*** _Argv,
            _Outptr_result_maybenull_ wchar_t*** _Env,
            _In_ int _DoWildCard,
            _In_ void* _StartInfo
            )
    {
        //Windows 2000版本没有返回值
        auto __wgetmainargs_fun = (decltype(__wgetmainargs)*)GetProcAddress(get_msvcrt(), "__wgetmainargs");

        if (!__wgetmainargs_fun)
            return -1;

        __wgetmainargs_fun(_Argc, _Argv, _Env, _DoWildCard, _StartInfo);

        return 0;
    }

    _LCRT_DEFINE_IAT_SYMBOL(__wgetmainargs);



    #undef _pctype
    EXTERN_C __declspec(dllimport) extern const unsigned short* _pctype;


    EXTERN_C const unsigned short* __cdecl __pctype_func(void)
    {
        return _pctype;
    }
    _LCRT_DEFINE_IAT_SYMBOL(__pctype_func);

    extern "C" wchar_t* __cdecl _wcserror(int const error_number)
    {
        static wchar_t const * const _sys_errlist_w[] =
        {
            /*  0              */  L"No error",
            /*  1 EPERM        */  L"Operation not permitted",
            /*  2 ENOENT       */  L"No such file or directory",
            /*  3 ESRCH        */  L"No such process",
            /*  4 EINTR        */  L"Interrupted function call",
            /*  5 EIO          */  L"Input/output error",
            /*  6 ENXIO        */  L"No such device or address",
            /*  7 E2BIG        */  L"Arg list too long",
            /*  8 ENOEXEC      */  L"Exec format error",
            /*  9 EBADF        */  L"Bad file descriptor",
            /* 10 ECHILD       */  L"No child processes",
            /* 11 EAGAIN       */  L"Resource temporarily unavailable",
            /* 12 ENOMEM       */  L"Not enough space",
            /* 13 EACCES       */  L"Permission denied",
            /* 14 EFAULT       */  L"Bad address",
            /* 15 ENOTBLK      */  L"Unknown error",                     /* not POSIX */
            /* 16 EBUSY        */  L"Resource device",
            /* 17 EEXIST       */  L"File exists",
            /* 18 EXDEV        */  L"Improper link",
            /* 19 ENODEV       */  L"No such device",
            /* 20 ENOTDIR      */  L"Not a directory",
            /* 21 EISDIR       */  L"Is a directory",
            /* 22 EINVAL       */  L"Invalid argument",
            /* 23 ENFILE       */  L"Too many open files in system",
            /* 24 EMFILE       */  L"Too many open files",
            /* 25 ENOTTY       */  L"Inappropriate I/O control operation",
            /* 26 ETXTBSY      */  L"Unknown error",                     /* not POSIX */
            /* 27 EFBIG        */  L"File too large",
            /* 28 ENOSPC       */  L"No space left on device",
            /* 29 ESPIPE       */  L"Invalid seek",
            /* 30 EROFS        */  L"Read-only file system",
            /* 31 EMLINK       */  L"Too many links",
            /* 32 EPIPE        */  L"Broken pipe",
            /* 33 EDOM         */  L"Domain error",
            /* 34 ERANGE       */  L"Result too large",
            /* 35 EUCLEAN      */  L"Unknown error",                     /* not POSIX */
            /* 36 EDEADLK      */  L"Resource deadlock avoided",
            /* 37 UNKNOWN      */  L"Unknown error",
            /* 38 ENAMETOOLONG */  L"Filename too long",
            /* 39 ENOLCK       */  L"No locks available",
            /* 40 ENOSYS       */  L"Function not implemented",
            /* 41 ENOTEMPTY    */  L"Directory not empty",
            /* 42 EILSEQ       */  L"Illegal byte sequence",
            /* 43              */  L"Unknown error"

        };


        return (wchar_t*)_sys_errlist_w[_countof(_sys_errlist_w) > error_number ? error_number : _countof(_sys_errlist_w) - 1];
    }

    _LCRT_DEFINE_IAT_SYMBOL(_wcserror);


    //EXTERN_C __declspec(dllimport) extern const int _osplatform;
    constexpr int _osplatform = 2;

    extern "C" int __cdecl _resetstkoflw()
    {
        LPBYTE pStack;
        MEMORY_BASIC_INFORMATION mbi;
        SYSTEM_INFO si;

        pStack = (LPBYTE)alloca(1);

        if (VirtualQuery(pStack, &mbi, sizeof mbi) == 0) {
            return 0;
        }

        GetSystemInfo(&si);
        auto pMaxGuard = (LPBYTE)(UINT_PTR(pStack) & ~(si.dwPageSize - 1)) - si.dwPageSize;
        auto pMinGuard = (LPBYTE)(UINT_PTR(mbi.AllocationBase) + si.dwPageSize * _osplatform != 1 ? 2 : 17);

        if (pMaxGuard >= pMinGuard)
        {
            DWORD flOldProtect;

            return VirtualProtect(pMaxGuard, si.dwPageSize, _osplatform != 1 ? PAGE_GUARD | PAGE_READWRITE : PAGE_NOACCESS, &flOldProtect);
        }

        return 0;
    }

    _LCRT_DEFINE_IAT_SYMBOL(_resetstkoflw);


    extern "C" bool __cdecl __uncaught_exception()
    {
        return __ProcessingThrow != 0;
    }

    _LCRT_DEFINE_IAT_SYMBOL(__uncaught_exception);

    extern "C" __int64 __cdecl _strtoi64(
        char const* const string,
        char** const end_ptr,
        int         const base
        )
    {
        return _strtoi64_l(string, end_ptr, base, nullptr);
    }

    _LCRT_DEFINE_IAT_SYMBOL(_strtoi64);

    extern "C" unsigned __int64 __cdecl _strtoui64(
        char const* const string,
        char** const end_ptr,
        int         const base
        )
    {
        return _strtoui64_l(string, end_ptr, base, nullptr);
    }

    _LCRT_DEFINE_IAT_SYMBOL(_strtoui64);

    extern "C" __int64 __cdecl _wcstoi64(
        wchar_t const* const string,
        wchar_t** const end_ptr,
        int            const base
        )
    {
        return _wcstoi64_l(string, end_ptr, base, nullptr);
    }
    _LCRT_DEFINE_IAT_SYMBOL(_wcstoi64);

    extern "C" unsigned __int64 __cdecl _wcstoui64(
        wchar_t const* const string,
        wchar_t** const end_ptr,
        int            const base
        )
    {
        return _wcstoui64_l(string, end_ptr, base, nullptr);
    }
    _LCRT_DEFINE_IAT_SYMBOL(_wcstoui64);

    extern "C" double __cdecl _wtof(wchar_t const* const string)
    {
        return _wtof_l(string, nullptr);
    }
    _LCRT_DEFINE_IAT_SYMBOL(_wtof);

}