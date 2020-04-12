/***
*msilexit.cpp - Exit routines for mixdll.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file provides at_exit support for clrcall functions. Here you will
*       find implementation for both managed process exit and appdomain exit.
*
*******************************************************************************/

#undef MRTDLL

#include <vcruntime_internal.h>
#include <vcstartup_internal.h>
#include <malloc.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <gcroot.h>

// #using is not used in the Windows build.
#ifndef _CRT_WINDOWS
    #ifdef _DEBUG
        #using <system.dll>
    #endif
#endif

#pragma warning(disable:4439)   // C4439: function with a managed parameter must have a __clrcall calling convention
#pragma warning(disable:4483)   // disable warning/error about __identifier(<string>)

using System::AppDomain;

#if defined (_DEBUG)
    #define MANAGED_ASSERT( test, message) System::Diagnostics::Debug::Assert(test, message)
#else  /* defined (_DEBUG) */
    #define MANAGED_ASSERT( test, message )
#endif  /* defined (_DEBUG) */

/*
 * match these macros with the ones in onexit.c
 */
#define MAXINCR     512
#define MININCR     4

extern "C" {
    typedef void (__clrcall *_CPVFV)(void);
}

#if defined (_M_CEE_MIXED)
static _CPVFV *__onexitbegin_m  = NULL;
static _CPVFV *__onexitend_m = NULL;
static size_t __exit_list_size = 0;
#endif  /* defined (_M_CEE_MIXED) */

namespace __identifier("<CrtImplementationDetails>")
{
using System::Threading::Monitor;
using System::Object;
using System::Runtime::InteropServices::GCHandle;
using System::IntPtr;

class AtExitLock
{
private:
    _AGLOBAL static void* _lock;
    _AGLOBAL static int _ref_count;

    [System::Diagnostics::DebuggerStepThroughAttribute]
    _CRT_SECURITYCRITICAL_ATTRIBUTE
    static GCHandle^ _handle()
    {
        if (_lock != nullptr)
        {
            return GCHandle::FromIntPtr(IntPtr(_lock));
        }

        return nullptr;
    }

    [System::Diagnostics::DebuggerStepThroughAttribute]
    static void _lock_Construct()
    {
        _lock = 0;
    }

    [System::Diagnostics::DebuggerStepThroughAttribute]
    _CRT_SECURITYCRITICAL_ATTRIBUTE
    static void _lock_Construct(Object^ value)
    {
        _lock = 0;
        _lock_Set(value);
    }

    [System::Diagnostics::DebuggerStepThroughAttribute]
    _CRT_SECURITYCRITICAL_ATTRIBUTE
    static void _lock_Set(Object^ value)
    {
        GCHandle^ handle = _handle();
        if (handle == nullptr)
        {
            handle = GCHandle::Alloc(value);
            _lock = GCHandle::ToIntPtr(*handle).ToPointer();
        }
        else
        {
            handle->Target = value;
        }
    }

    [System::Diagnostics::DebuggerStepThroughAttribute]
    _CRT_SECURITYCRITICAL_ATTRIBUTE
    static Object^ _lock_Get()
    {
        GCHandle^ handle = _handle();
        if (handle != nullptr)
        {
            return (Object^)handle->Target;
        }
        return nullptr;
    }

    [System::Diagnostics::DebuggerStepThroughAttribute]
    _CRT_SECURITYCRITICAL_ATTRIBUTE
    static void _lock_Destruct()
    {
        GCHandle^ handle = _handle();
        if (handle != nullptr)
        {
            handle->Free();
            _lock = 0;
        }
    }

public:
    // No secret exposed; no side effect
    [System::Diagnostics::DebuggerStepThroughAttribute]
    _CRT_SECURITYCRITICAL_ATTRIBUTE
    static bool IsInitialized()
    {
        return (_lock_Get() != nullptr);
    }

    [System::Diagnostics::DebuggerStepThroughAttribute]
    _CRT_SECURITYCRITICAL_ATTRIBUTE
    static void AddRef()
    {
        if (!IsInitialized())
        {
            _lock_Construct(gcnew Object());
            _ref_count = 0;
        }
        ++_ref_count;
    }

    [System::Diagnostics::DebuggerStepThroughAttribute]
    _CRT_SECURITYCRITICAL_ATTRIBUTE
    static void RemoveRef()
    {
        MANAGED_ASSERT(_ref_count > 0, "Reference count must be greater than zero");
        if (--_ref_count == 0)
        {
            MANAGED_ASSERT(IsInitialized(), "Lock must be initialized");
            _lock_Destruct();
        }
    }

    [System::Diagnostics::DebuggerStepThroughAttribute]
    _CRT_SECURITYCRITICAL_ATTRIBUTE
    static void Enter()
    {
#pragma warning( push )
#pragma warning( disable : 4947 )
        //@TODO: This overload of Monitor::Enter is obsolete.  Please change this to use Monitor::Enter(bool&), and remove the pragmas
        Monitor::Enter(_lock_Get());
#pragma warning( pop )
    }

    [System::Diagnostics::DebuggerStepThroughAttribute]
    _CRT_SECURITYCRITICAL_ATTRIBUTE
    static void Exit()
    {
        Monitor::Exit(_lock_Get());
    }
};
_AGLOBAL void * AtExitLock::_lock; /* Do NOT initialize */
_AGLOBAL int AtExitLock::_ref_count /*= 0*/; /* Do NOT initialize */
}

/***
* __global_lock() - lock access for __global_lock_object
*
* Purpose:
*       This function is used by atexit functions to avoid parallel
*       access to critical data.
*
* Entry:
*
* Exit:
*       returns true if successful.
*
*******************************************************************************/

[System::Diagnostics::DebuggerStepThroughAttribute]
_CRT_SECURITYCRITICAL_ATTRIBUTE
static bool __global_lock()
{
    using namespace __identifier("<CrtImplementationDetails>");

    bool retval = false;
    if (AtExitLock::IsInitialized())
    {
        AtExitLock::Enter();
        retval = true;
    }
    return retval;
}

/***
* __global_unlock() - releases the lock held by calling __global_lock
*
* Purpose:
*       Release the lock held by calling __global_lock.
*
* Entry:
*
* Exit:
*       returns true if successful.
*
*******************************************************************************/

[System::Diagnostics::DebuggerStepThroughAttribute]
_CRT_SECURITYCRITICAL_ATTRIBUTE
static bool __global_unlock()
{
    using namespace __identifier("<CrtImplementationDetails>");

    bool retval = false;
    if (AtExitLock::IsInitialized())
    {
        AtExitLock::Exit();
        retval = true;
    }
    return retval;
}

/***
* __alloc_global_lock() - allocates CLR lock for access to lockClass::__global_lock_object
*
* Purpose:
*       This lock object is used by atexit routines to have single threaded
*       access to critical data.
*
* Entry:
*
* Exit:
*       return true if successful.
*
*******************************************************************************/

[System::Diagnostics::DebuggerStepThroughAttribute]
_CRT_SECURITYCRITICAL_ATTRIBUTE
static bool __alloc_global_lock()
{
    using namespace __identifier("<CrtImplementationDetails>");

    AtExitLock::AddRef();
    return AtExitLock::IsInitialized();
}

/***
* __dealloc_global_lock() - deallocates the lock used for access to lockClass::__global_lock_object
*
* Purpose:
*       Remove the reference to the lock object
*
* Entry:
*
* Exit:
*
*******************************************************************************/

[System::Diagnostics::DebuggerStepThroughAttribute]
_CRT_SECURITYCRITICAL_ATTRIBUTE
static void __dealloc_global_lock()
{
    using namespace __identifier("<CrtImplementationDetails>");

    AtExitLock::RemoveRef();
}

/***
* _atexit_helper(). registers function to be called during process exit.
*
* Purpose:
*       This function registers __clrcall function to be called at processexit.
*       The callback are in LIFO, i.e. last call registered will be first one
*       to be called at the time of exit.
*
* Entry:
*       _CPVFV,
*       size_t *,
*       _CPVFV **,
*       _CPVFV **;
*
* Exit:
*       return -1 if the function was not registered. Else returns 0.
*
* Notes:
*       This routine assumes that _initatexit_m was called before this function
*       is called.
*
*******************************************************************************/

_CRT_SECURITYCRITICAL_ATTRIBUTE
extern "C" int __clrcall _atexit_helper( _CPVFV func,
        size_t *__pexit_list_size,
        _CPVFV **__ponexitend_e,
        _CPVFV **__ponexitbegin_e)
{
    _CPVFV retval = NULL;

    /*
     * Get the lock for CRT exit function calls.
     */
    if (func == NULL) return -1;
    if (__global_lock() == true) {
        try {
            _CPVFV * __onexitbegin = (_CPVFV *) DecodePointer(*__ponexitbegin_e);
            _CPVFV * __onexitend = (_CPVFV *) DecodePointer(*__ponexitend_e);

            /*
             * check if we have space of one more entry.
             */
            if ( (*__pexit_list_size) - 1<
                    ((size_t)((char *)__onexitend - (char *)__onexitbegin))/sizeof(_CPVFV))
            {
                /*
                 * Try to increment by max increment or twice the current size, if fails,
                 * then try to increment by min increment. If that too fails then return NULL.
                 */
                try {
                    System::IntPtr tmp_ptr =
                        System::Runtime::InteropServices::Marshal::ReAllocHGlobal(
                            System::IntPtr(__onexitbegin),
                            System::IntPtr((long)((*__pexit_list_size)*sizeof(_CPVFV)+
                                __min((*__pexit_list_size)*sizeof(_CPVFV),
                                    (MAXINCR*sizeof(_CPVFV))))));

                    __onexitend = (_CPVFV *)((char *)tmp_ptr.ToPointer()+
                            (size_t)__onexitend-
                            (size_t)__onexitbegin);
                    __onexitbegin = (_CPVFV *)tmp_ptr.ToPointer();
                    (*__pexit_list_size) += __min(MAXINCR, (*__pexit_list_size));

                } catch (System::OutOfMemoryException ^) {

                    System::IntPtr tmp_ptr =
                        System::Runtime::InteropServices::Marshal::ReAllocHGlobal(
                            System::IntPtr(__onexitbegin),
                            System::IntPtr((long)((*__pexit_list_size)*sizeof(_CPVFV)+
                                MININCR+sizeof(_CPVFV))));

                    __onexitend = (_CPVFV *)((char *)tmp_ptr.ToPointer()+
                            (size_t)__onexitend-
                            (size_t)__onexitbegin);
                    __onexitbegin = (_CPVFV *)tmp_ptr.ToPointer();
                    (*__pexit_list_size) += MININCR;
                }
            }
            *(__onexitend++) = (_CPVFV)func;
            retval = func;

            (*__ponexitbegin_e) = (_CPVFV *) EncodePointer(__onexitbegin);
            (*__ponexitend_e) = (_CPVFV *) EncodePointer(__onexitend);

        } catch(System::OutOfMemoryException ^) {
        } __finally{
            __global_unlock();
        }
    }
    return retval==NULL?-1:0;
}

#ifdef _M_CEE_MIXED
/***
* _exit_callback() - this function is called by msvcrt at exit.
*
* Purpose:
*       This function is called from msvcrt dll at process exit. This
*       functio then calls all the registered __clrcall function. This
*       function acts as layer between __cdecl atexit callback and __clrcal
*       atexit callback.
*
* Entry:
*
* Exit:
*
*******************************************************************************/

_CRT_SECURITYCRITICAL_ATTRIBUTE
extern "C" void __clrcall _exit_callback(void)
{
    MANAGED_ASSERT(AppDomain::CurrentDomain->IsDefaultAppDomain(), "This function must be called in the default domain");
    /*
     * Note: this function is not supposed to be called from more then one
     * thread at a time. Infact this is called when the program is terminating
     * or the dll is being unloaded.
     *
     * Thus we don't need to lock anything here.
     */

    /*
     * Note that this function is called during appdomain unload.
     * Thus there should be only one thread calling this function per
     * appdomain. Thus this function does not need to be under lock.
     *
     * Note: verify that when domainUnload event is generated, there
     * are no threads executing in this appdomain. If this is not the
     * case then we will need to have lock around this function.
     */

    if (__exit_list_size == 0)
    {
        /*
         * _initatexit_m() hasn't been called yet
         * Nothing to do
         */
        return;
    }

    _CPVFV * onexitbegin_m = (_CPVFV *) DecodePointer(__onexitbegin_m);
    _CPVFV * onexitend_m = (_CPVFV *) DecodePointer(__onexitend_m);

    if ((onexitbegin_m != (_CPVFV *)-1) && (onexitbegin_m != NULL) && (onexitend_m != NULL)) {
        _CPVFV function_to_call_m = NULL;

        /* save the start and end for later comparison */
        _CPVFV * onexitbegin_m_saved = onexitbegin_m;
        _CPVFV * onexitend_m_saved = onexitend_m;

        for (;;)
        {
            _CPVFV * onexitbegin_m_new = NULL;
            _CPVFV * onexitend_m_new = NULL;

            /* find the last valid function pointer to call. */
            while (--onexitend_m >= onexitbegin_m && *onexitend_m == EncodePointer(NULL))
            {
                /* keep going backwards. */
            }

            if (onexitend_m < onexitbegin_m)
            {
                /* there are no more valid entries in the list, we are done. */
                break;
            }

            /* cache the function to call. */
            function_to_call_m = (_CPVFV) DecodePointer(*onexitend_m);

            /* mark the function pointer as visited. */
            *onexitend_m = (_CPVFV)EncodePointer(NULL);

            /* call the function, which can eventually change __onexitbegin_m and __obexitend_m */
            (*function_to_call_m)();

            /* save the start and end for later comparison */
            onexitbegin_m_new = (_CPVFV *) DecodePointer(__onexitbegin_m);
            onexitend_m_new = (_CPVFV *) DecodePointer(__onexitend_m);

            if ( ( onexitbegin_m_saved != onexitbegin_m_new ) || ( onexitend_m_saved != onexitend_m_new ) )
            {
                /* reset only if either start or end has changed */
                onexitbegin_m = onexitbegin_m_saved = onexitbegin_m_new;
                onexitend_m = onexitend_m_saved = onexitend_m_new;
            }
        }
        /*
         * There is no real need to free memory here as the process is anyway
         * going down and CRT will free it's heap and this memory will anyway
         * be freed. But just so that user doesn't see the memory leak if he
         * wants to detect leak using CRT functionality, we free this memory.
         */
        System::Runtime::InteropServices::Marshal::FreeHGlobal(
                System::IntPtr((void *)onexitbegin_m));
    }
    __dealloc_global_lock();
}


/***
*_initatexit_m(). Initializes the managed process exit variables.
*
* Purpose:
*       The _initatexit_m function is called from cctor of mixdll. This function
*       is responsible for initializing managed process exit variables. The
*       initialization includes allocating memory.
*
* Entry:
*
* Exit:
*       1 if initialization was successful 0 otherwise.
*
*******************************************************************************/

[System::Diagnostics::DebuggerStepThroughAttribute]
_CRT_SECURITYCRITICAL_ATTRIBUTE
extern "C" int __clrcall _initatexit_m()
{
    MANAGED_ASSERT(AppDomain::CurrentDomain->IsDefaultAppDomain(), "This function must be called in the default domain");

    int retval = 0;

    /*
     * Note: This function is called only once just during cctor call.
     */
    if (__alloc_global_lock() == true) {
        System::IntPtr tmp_int_ptr =
            System::Runtime::InteropServices::Marshal::AllocHGlobal(
                    32*sizeof(*__onexitbegin_m));

        __onexitend_m = __onexitbegin_m = (_CPVFV *) EncodePointer(tmp_int_ptr.ToPointer());
        __exit_list_size = 32;
        retval = 1;
    }
    return retval;
}

/***
* _atexit_m(_CPVFV). registers function to be called during process exit.
*
* Purpose:
*       This function registers __clrcall function to be called at processexit.
*       The callback are in LIFO, i.e. last call registered will be first one
*       to be called at the time of exit.
*
* Entry:
*       void (__clrcall *func)(void);
*
* Exit:
*       return -1 if the function was not registered. Else returns 0.
*
* Notes:
*       This routine assumes that _initatexit_m was called before this function
*       is called.
*
*******************************************************************************/

extern "C" _onexit_m_t __clrcall _onexit_m( _onexit_m_t _Function)
{
    MANAGED_ASSERT(AppDomain::CurrentDomain->IsDefaultAppDomain(), "This function must be called in the default domain");

    return _atexit_m((_CPVFV)_Function) == -1 ? NULL : _Function;
}

_CRT_SECURITYCRITICAL_ATTRIBUTE
extern "C" int __clrcall _atexit_m( _CPVFV func)
{
    MANAGED_ASSERT(AppDomain::CurrentDomain->IsDefaultAppDomain(), "This function must be called in the default domain");

    int retval = _atexit_helper((_CPVFV) EncodePointer(func), &__exit_list_size, &__onexitend_m, &__onexitbegin_m);

    return retval;

}
#endif  /* defined (_M_CEE_MIXED) */

/***
*
* AppDomain Exit stuff.
*
*******************************************************************************/

/*
 * Note that the initialization values are commented out.
 * The reason here is that these variables are appdomain specific. And if
 * we try to initialize them, unlike process variables, they will be
 * dynamically initialized. This means they will be initialized by our
 * initialization routine BUT we need to initialize them before we run
 * our initialization code.
 *
 * Not initializing them here is also OK just because we can always
 * intialize them in _initatexit_app_domain function
 */

_AGLOBAL _CPVFV *__onexitbegin_app_domain /*= NULL*/;
_AGLOBAL _CPVFV *__onexitend_app_domain /*= NULL*/;
_AGLOBAL size_t __exit_list_size_app_domain /*= 0*/;


/***
*_initatexit_app_domain(). Initializes the managed per appdomain exit variables.
*
* Purpose:
*       The _initatexit_app_domain function is called from cctor of mixdll. This
*       function is responsible for initializing managed per appdomain exit
*       variables. The initialization includes allocating memory.
*
* Entry:
*
* Exit:
*       1 if initialization was successful 0 otherwise.
*
*******************************************************************************/

[System::Diagnostics::DebuggerStepThroughAttribute]
_CRT_SECURITYCRITICAL_ATTRIBUTE
extern "C" int __clrcall _initatexit_app_domain()
{
    if (__alloc_global_lock() == true) {
        /*
         * Note that this function is called from the cctor during the
         * appdomain initialization. Thus there is going to be only one thread
         * per process accessing this function.
         *
         * Thus this function does not need to be under the lock.
         */

        System::IntPtr tmp_int_ptr =
            System::Runtime::InteropServices::Marshal::AllocHGlobal(
                    32*sizeof(*__onexitbegin_app_domain));

        __onexitend_app_domain = __onexitbegin_app_domain = (_CPVFV *) EncodePointer(tmp_int_ptr.ToPointer());
        __exit_list_size_app_domain = 32;
#if defined (_M_CEE_PURE)
        __scrt_initialize_type_info();
        _atexit_m_appdomain(__scrt_uninitialize_type_info);
#endif  /* defined (_M_CEE_PURE) */
    }
    return 1;
}

/***
* _app_exit_callback() - this function is called by cctor during appdomain exit.
*
* Purpose:
*       This function is called from cctor during appdomain exit. This
*       function then calls all the registered __clrcall function. This
*       function acts as appdomain version of _exit_callback.
*
* Entry:
*
* Exit:
*
*******************************************************************************/

_CRT_SECURITYCRITICAL_ATTRIBUTE
extern "C" void __clrcall _app_exit_callback(void)
{
    /*
     * Note that this function is called during appdomain unload.
     * Thus there should be only one thread calling this function per
     * appdomain. Thus this function does not need to be under lock.
     *
     * Note: verify that when domainUnload event is generated, there
     * are no threads executing in this appdomain. If this is not the
     * case then we will need to have lock around this function.
     */

    if (__exit_list_size_app_domain == 0 )
    {
        /*
         * _initatexit_appdomain() hasn't been called yet
         * Nothing to do
         */
        return;
    }

    _CPVFV * onexitbegin_app_domain = (_CPVFV *) DecodePointer(__onexitbegin_app_domain);
    _CPVFV * onexitend_app_domain = (_CPVFV *) DecodePointer(__onexitend_app_domain);

    __try
    {
        if ((onexitbegin_app_domain != (_CPVFV *)-1) && (onexitbegin_app_domain != NULL) && (onexitend_app_domain != NULL)) {
            _CPVFV function_to_call_app_domain = NULL;

            _CPVFV * onexitbegin_app_domain_saved = onexitbegin_app_domain;
            _CPVFV * onexitend_app_domain_saved = onexitend_app_domain;

            for (;;)
            {
                _CPVFV * onexitbegin_app_domain_new = NULL;
                _CPVFV * onexitend_app_domain_new = NULL;

                /* find the last valid function pointer to call. */
                while (--onexitend_app_domain >= onexitbegin_app_domain && *onexitend_app_domain == EncodePointer(NULL))
                {
                    /* keep going backwards. */
                }

                if (onexitend_app_domain < onexitbegin_app_domain)
                {
                    /* there are no more valid entries in the list, we are done. */
                    break;
                }

                /* cache the function to call. */
                function_to_call_app_domain = (_CPVFV) DecodePointer(*onexitend_app_domain);

                /* mark the function pointer as visited. */
                *onexitend_app_domain = (_CPVFV)EncodePointer(NULL);

                /* call the function, which can eventually change __onexitbegin_app_domain and __obexitend_m */
                (*function_to_call_app_domain)();

                onexitbegin_app_domain_new = (_CPVFV *) DecodePointer(__onexitbegin_app_domain);
                onexitend_app_domain_new = (_CPVFV *) DecodePointer(__onexitend_app_domain);

                if ( ( onexitbegin_app_domain_saved != onexitbegin_app_domain_new ) ||
                     ( onexitend_app_domain_saved != onexitend_app_domain_new ) )
                {
                    onexitbegin_app_domain = onexitbegin_app_domain_saved = onexitbegin_app_domain_new;
                    onexitend_app_domain = onexitend_app_domain_saved = onexitend_app_domain_new;
                }
            }
        }
    }
    __finally
    {
        System::Runtime::InteropServices::Marshal::FreeHGlobal(
            System::IntPtr((void *)onexitbegin_app_domain));
        __dealloc_global_lock();
    }
}

/***
* _atexit_app_domain(_CPVFV). registers function to be called during appdomain exit.
*
* Purpose:
*       This function registers __clrcall function to be called at appdomain exit.
*       The callback are in LIFO, i.e. last call registered will be first one
*       to be called at the time of exit. This is appdomain version of _atexit_m()
*
* Entry:
*       void (__clrcall *func)(void);
*
* Exit:
*       return -1 if the function was not registered. Else returns 0.
*
* Notes:
*       This routine assumes that _initatexit_app_domain was called before this
*       function is called.
*
*******************************************************************************/

_CRT_SECURITYCRITICAL_ATTRIBUTE
extern "C" _onexit_m_t __clrcall _onexit_m_appdomain(_onexit_m_t _Function)
{
    return _atexit_m_appdomain((_CPVFV)_Function) == -1 ? NULL : _Function;
}

[System::Diagnostics::DebuggerStepThroughAttribute]
_CRT_SECURITYCRITICAL_ATTRIBUTE
_Check_return_ extern "C" int __clrcall _atexit_m_appdomain(_CPVFV func)
{
    int retval = _atexit_helper((_CPVFV) EncodePointer(func), &__exit_list_size_app_domain, &__onexitend_app_domain, &__onexitbegin_app_domain);

    return retval;
}
