/***
*excptptr.cpp - The exception_ptr implementation and everything associated with it.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*        The exception_ptr implementation and everything associated with it.
****/

#ifndef _VCRT_ALLOW_INTERNALS
    #define _VCRT_ALLOW_INTERNALS
#endif

#include <Unknwn.h>
#include <Windows.h>
#include <eh.h>
#include <ehdata.h>
#include <exception>
#include <internal_shared.h>
#include <malloc.h>
#include <memory>
#include <new>
#include <stddef.h>
#include <stdexcept>
#include <trnsctrl.h>
#include <xcall_once.h>
#include "..\..\winapi_thunks.h"

// Pre-V4 managed exception code
#define MANAGED_EXCEPTION_CODE  0XE0434F4D

// V4 and later managed exception code
#define MANAGED_EXCEPTION_CODE_V4  0XE0434352

#if _EH_RELATIVE_TYPEINFO

#undef CT_COPYFUNC
#define CT_COPYFUNC(ct)         ((ct).copyFunction? CT_COPYFUNC_IB(ct, _GetThrowImageBase()):NULL)
#undef THROW_COUNT
#define THROW_COUNT(ti)         THROW_COUNT_IB(ti, _GetThrowImageBase())

#endif // _EH_RELATIVE_TYPEINFO

////////////////////////////////////////////////////////////////////////////////
//
// Forward declaration of functions:
//

static void * _StaticAlloc(size_t bytesToAllocate);

extern "C" _CRTIMP2 void * __cdecl __AdjustPointer( void *, const PMD&); // defined in frame.cpp

using namespace std;

class __ExceptionPtr
{
    public:
        __ExceptionPtr(_In_ const EHExceptionRecord *, bool normal);
        __ExceptionPtr(const __ExceptionPtr&) = delete;
        __ExceptionPtr& operator=(const __ExceptionPtr&) = delete;
        static shared_ptr<__ExceptionPtr> _CurrentException();
        static EHExceptionRecord * _PopulateExceptionRecord(EHExceptionRecord&, const void *,
            const ThrowInfo*);
        static shared_ptr<__ExceptionPtr> _CopyException(_In_ const void*, _In_ const ThrowInfo*);
        static const shared_ptr<__ExceptionPtr>& _BadAllocException();

        ~__ExceptionPtr();

        [[noreturn]] void _RethrowException() const;

    private:
        static shared_ptr<__ExceptionPtr> _InitBadAllocException();
#ifndef _M_CEE_PURE
        static int __stdcall _PlacementInitBadAllocException(void *, void * _Storage_ptr, void **) noexcept
        {
            ::new (_Storage_ptr) shared_ptr<__ExceptionPtr>(_InitBadAllocException());
            return 1;
        }
#endif /* _M_CEE_PURE */

        void _CallCopyCtor(_Out_writes_bytes_(size) void* dst, _In_reads_bytes_(size) void* src,
            size_t size, _In_ const CatchableType * const pType) const;
#if _EH_RELATIVE_TYPEINFO
        ptrdiff_t _GetThrowImageBase() const
        {
            return (ptrdiff_t) m_EHRecord.params.pThrowImageBase;
        }
#endif
        union
        {
            _EXCEPTION_RECORD m_Record;
            EHExceptionRecord m_EHRecord;
        };
        bool m_normal;
};

static_assert(sizeof(std::exception_ptr) == sizeof(std::shared_ptr<__ExceptionPtr>),
    "std::exception_ptr and std::shared_ptr<__ExceptionPtr> must have the same size.");

////////////////////////////////////////////////////////////////////////////////
//
// void __ExceptionPtr::_CallCopyCtor();
//
// Helper function for calling the copy-ctor on the C++ exception object.  It reads
// the passed in throw info and determines the type of exception.  If it's a simple type
// it calls memcpy.  If it's a UDT with copy-ctor, it uses the copy-ctor.
//
void __ExceptionPtr::_CallCopyCtor(_Out_writes_bytes_(size) void* dst,
    _In_reads_bytes_(size) void* src, size_t size, _In_ const CatchableType * const pType) const
{
    if (CT_ISSIMPLETYPE(*pType) || // this is a simple type
            CT_COPYFUNC(*pType) == NULL    // this is a user defined type without copy ctor
       )
    {
        // just copy with memcpy
        memcpy(dst, src, size);

        if (CT_ISWINRTHANDLE(*pType))
        {
            IUnknown* pUnknown = *(reinterpret_cast<IUnknown**>(src));
            if (pUnknown)
            {
               pUnknown->AddRef();
            }
        }
    }
    else
    {
        try
        {
            // it's a user defined type with a copy ctor
            if (CT_HASVB(*pType))
            {
                // this exception got a virtual base
#if defined(_M_CEE_PURE)
                void (__clrcall *pFunc)(void *, void *, int) =
                    (void (__clrcall *)(void *, void *, int))(void *)CT_COPYFUNC(*pType);
                pFunc((void *)dst, __AdjustPointer(src, CT_THISDISP(*pType)), 1);
#else
                _CallMemberFunction2((char *)dst,
                        CT_COPYFUNC(*pType),
                        __AdjustPointer(src,
                            CT_THISDISP(*pType)), 1);
#endif // defined(_M_CEE_PURE)
            }
            else
            {
#if defined(_M_CEE_PURE)
                void (__clrcall *pFunc)(void *, void *) =
                    (void (__clrcall *)(void *, void *))(void *)CT_COPYFUNC(*pType);
                pFunc((void *)dst, __AdjustPointer(src, CT_THISDISP(*pType)));
#else
                _CallMemberFunction1((char *)dst,
                        CT_COPYFUNC(*pType),
                        __AdjustPointer(src,
                            CT_THISDISP(*pType)));
#endif // defined(_M_CEE_PURE)
            }
        }
        catch (...)
        {
            // the copy-Ctor() threw.  We need to terminate().
            terminate();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// void __ExceptionPtr::_CurrentException();
//
// Builds an __ExceptionPtr with the current exception inflight.  This function
// gets the information from the per-thread-data (PTD).
//
// This function returns the static copy of exception_ptr which points to a
// std::bad_alloc exception in out-of-memory situation.
//

shared_ptr<__ExceptionPtr> __ExceptionPtr::_CurrentException()
{

    // there is an exception in flight... copy it
    try
    {
        if (_pCurrentException == NULL || // no exception in flight
                __ProcessingThrow != 0 || // we are unwinding... possibly called from a dtor()
                ((_pCurrentException->ExceptionCode ==  MANAGED_EXCEPTION_CODE) ||
                (_pCurrentException->ExceptionCode ==  MANAGED_EXCEPTION_CODE_V4))    // we don't support managed exceptions
           )
        {
            // return a NULL exception_ptr
            return shared_ptr<__ExceptionPtr>();
        }

        return make_shared<__ExceptionPtr>(_pCurrentException, true);
    }
    catch (const std::bad_alloc&)
    {
        return _BadAllocException();
    }
    catch (...)
    {
        // something went wrong, and it wasn't a bad_alloc and
        // we are not allowed to throw out of this function...
        terminate();
    }
}


template <typename T> struct _StaticAllocator {
    typedef T value_type;

    _StaticAllocator() = default;

    template <typename U> _StaticAllocator(const _StaticAllocator<U>&) { }

    template <typename U> bool operator==(const _StaticAllocator<U>&) const {
        return true;
    }

    template <typename U> bool operator!=(const _StaticAllocator<U>&) const {
        return false;
    }

    T * allocate(const size_t n) const {
        if (n == 0) {
            return nullptr;
        }

        if (n > static_cast<size_t>(-1) / sizeof(T)) {
            terminate();
        }

        return static_cast<T *>(_StaticAlloc(n * sizeof(T)));
    }

    void deallocate(T *, size_t) const {
        // purposely does nothing
    }
};

shared_ptr<__ExceptionPtr> __ExceptionPtr::_InitBadAllocException()
{
    std::bad_alloc _Except;
    EHExceptionRecord ehRecord;
    EHExceptionRecord* pExcept = _PopulateExceptionRecord(ehRecord, &_Except,
        static_cast<const ThrowInfo*>(__GetExceptionInfo(_Except)));
    return allocate_shared<__ExceptionPtr>(_StaticAllocator<int>(), pExcept, false);
}

const shared_ptr<__ExceptionPtr>& __ExceptionPtr::_BadAllocException()
{
#ifdef _M_CEE_PURE
    /* MAGIC */ static shared_ptr<__ExceptionPtr> badAllocPtr = _InitBadAllocException();
    return badAllocPtr;
#else /* ^^^ _M_CEE_PURE ^^^ // vvv !_M_CEE_PURE vvv */
    static void * _Flag = nullptr;
    static_assert(sizeof(void *) == sizeof(once_flag), "TRANSITION, VSO#406237");
    static_assert(alignof(void *) == alignof(once_flag), "TRANSITION, VSO#406237");
    static aligned_union_t<1, __ExceptionPtr> _Storage;
    if (_Execute_once(reinterpret_cast<once_flag&>(_Flag), _PlacementInitBadAllocException, &_Storage) == 0)
        {	// _Execute_once should never fail if the callback never fails
        std::terminate();
        }

    return (reinterpret_cast<shared_ptr<__ExceptionPtr>&>(_Storage));
#endif /* _M_CEE_PURE */
}

EHExceptionRecord * __ExceptionPtr::_PopulateExceptionRecord(EHExceptionRecord& ehRecord,
    const void * pExceptObj, const ThrowInfo* pThrowInfo)
{
    // the initial values of EHExceptionRecord need to be consistent with _CxxThrowException()
    EHExceptionRecord* pExcept = &ehRecord;

    // initialize the record
    PER_CODE(pExcept) = EH_EXCEPTION_NUMBER;
    PER_FLAGS(pExcept) = EXCEPTION_NONCONTINUABLE;
    PER_NEXT(pExcept) = NULL; // no SEH to chain
    PER_ADDRESS(pExcept) = NULL; // Address of exception. Will be overwritten by OS
    PER_NPARAMS(pExcept) = EH_EXCEPTION_PARAMETERS;
    PER_MAGICNUM(pExcept) = EH_MAGIC_NUMBER1;
    PER_PEXCEPTOBJ(pExcept) = const_cast<void*>(pExceptObj);

    ThrowInfo* pTI = pThrowInfo;
#if 0 //VC-LTL不可能运行在WinRT程序中
    if (pTI && (THROW_ISWINRT( (*pTI) ) ) )
    {
        ULONG_PTR *exceptionInfoPointer = *reinterpret_cast<ULONG_PTR**>(const_cast<void*>(pExceptObj));
        // The pointer to the ExceptionInfo structure is stored sizeof(void*) in
        // front of each WinRT Exception Info.
        --exceptionInfoPointer;

        WINRTEXCEPTIONINFO* wei = reinterpret_cast<WINRTEXCEPTIONINFO*>(*exceptionInfoPointer);
        pTI = wei->throwInfo;
    }
#endif

    PER_PTHROW(pExcept) = pTI;

#if _EH_RELATIVE_TYPEINFO
    PVOID ThrowImageBase = RtlPcToFileHeader((PVOID)pTI, &ThrowImageBase);
    PER_PTHROWIB(pExcept) = ThrowImageBase;
#endif

    //
    // If the throw info indicates this throw is from a pure region,
    // set the magic number to the Pure one, so only a pure-region
    // catch will see it.
    //
    // Also use the Pure magic number on Win64 if we were unable to
    // determine an image base, since that was the old way to determine
    // a pure throw, before the TI_IsPure bit was added to the FuncInfo
    // attributes field.
    //
    if (pTI != NULL)
    {
        if (THROW_ISPURE(*pTI))
        {
            PER_MAGICNUM(pExcept) = EH_PURE_MAGIC_NUMBER1;
        }
#if _EH_RELATIVE_TYPEINFO
        else if (ThrowImageBase == NULL)
        {
            PER_MAGICNUM(pExcept) = EH_PURE_MAGIC_NUMBER1;
        }
#endif
    }

    return pExcept;
}

////////////////////////////////////////////////////////////////////////////////
//
// void __ExceptionPtr::_CopyException();
//
// Builds an __ExceptionPtr with the given C++ exception object and its corresponding
// ThrowInfo.
//
// This function returns the static copy of exception_ptr which points to a
// std::bad_alloc exception in out-of-memory situation.
//
shared_ptr<__ExceptionPtr> __ExceptionPtr::_CopyException(const void * pExceptObj,
    const ThrowInfo* pThrowInfo)
{
    try
    {
        EHExceptionRecord ehRecord;
        EHExceptionRecord* pExcept = _PopulateExceptionRecord(ehRecord, pExceptObj,
            pThrowInfo);
        return make_shared<__ExceptionPtr>(pExcept, true);
    }
    catch (const std::bad_alloc&)
    {
        return _BadAllocException();
    }
    catch (...)
    {
        // something went wrong, and it wasn't a bad_alloc and
        // we are not allowed to throw out of this function...
        terminate();
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// void __ExceptionPtr::__ExceptionPtr();
//
// Ctor for __ExceptionPtr.  This function copies the EXCEPTION_RECORD that is
// passed in.  If the EXCEPTION_RECORD is a C++ exception it will also allocate
// heap memory for the C++ exception and call the copy-ctor for it.
//
// This function throws std::bad_alloc exception in out-of-memory situation.
//
__ExceptionPtr::__ExceptionPtr(_In_ const EHExceptionRecord * pExcept, bool normal)
    : m_normal(normal)
{
    //copying the _EXCEPTION_RECORD (for SEH exceptions)
    PER_CODE(   &this->m_Record) = PER_CODE(pExcept);
    PER_FLAGS(  &this->m_Record) = PER_FLAGS(pExcept);
    PER_NEXT(   &this->m_Record) = 0; // We don't chain SEH exceptions
    PER_ADDRESS(&this->m_Record) = 0; // Useless field to copy.  It will be overwritten by RaiseException()
    PER_NPARAMS(&this->m_Record) = PER_NPARAMS(pExcept);
    // copying any addition parameters
    for (ULONG i = 0; i < this->m_Record.NumberParameters && i < EXCEPTION_MAXIMUM_PARAMETERS; ++i)
    {
        this->m_Record.ExceptionInformation[i] = ((EXCEPTION_RECORD*)pExcept)->ExceptionInformation[i];
    }
    // NULLing out any un-used parameters
    for (ULONG i = this->m_Record.NumberParameters; i < EXCEPTION_MAXIMUM_PARAMETERS; ++i)
    {
        this->m_Record.ExceptionInformation[i] = 0;
    }

    if (PER_IS_MSVC_PURE_OR_NATIVE_EH(pExcept))
    {
        // this is a C++ exception
        // we need to copy the C++ exception object

        // nulling out the Exception object pointer, because we haven't copied it yet
        PER_PEXCEPTOBJ(&this->m_EHRecord) = NULL;

        ThrowInfo* pThrow= PER_PTHROW(pExcept);
        if (    PER_PEXCEPTOBJ(pExcept) == NULL ||
                pThrow == NULL ||
                pThrow->pCatchableTypeArray == NULL ||
                THROW_COUNT(*pThrow) <=0
                )
        {
            // No ThrowInfo exists.  If this were a C++ exception, something must have corrupted it.
            terminate();
        }
        // we want to encode the ThrowInfo pointer for security reasons
        PER_PTHROW(&this->m_EHRecord) = (ThrowInfo*) EncodePointerDownlevel((void*)pThrow);

        // we finally got the type info we want
#if _EH_RELATIVE_TYPEINFO
        const auto pType = (CatchableType *)
            (*(int*)THROW_CTLIST_IB(*pThrow, _GetThrowImageBase()) + _GetThrowImageBase());
#else
        CatchableType *const pType = *THROW_CTLIST(*pThrow);
#endif

        void* pExceptionBuffer = NULL;

        if (normal)
        {
            pExceptionBuffer = malloc(CT_SIZE(*pType));
        }
        else
        {
            pExceptionBuffer = _StaticAlloc(CT_SIZE(*pType));
        }

        if (pExceptionBuffer == NULL)
        {
            throw std::bad_alloc();
        }
        __ExceptionPtr::_CallCopyCtor(pExceptionBuffer, PER_PEXCEPTOBJ(pExcept), CT_SIZE(*pType), pType);

        PER_PEXCEPTOBJ(&this->m_EHRecord) = pExceptionBuffer;
    }
    //else // this is a SEH exception, no special handling is required
}

////////////////////////////////////////////////////////////////////////////////
//
// void __ExceptionPtr::~__ExceptionPtr();
//
// Dtor for __ExceptionPtr.  If the stored exception is a C++ exception, it destroys
// the C++ exception object and de-allocate the heap memory for it.
//
__ExceptionPtr::~__ExceptionPtr()
{
    if (!m_normal)
    {
        // If m_normal is false, *this is m_badAllocExceptionPtr. Since everything for that special
        // case is allocated into m_badAllocExceptionPtr itself or with _StaticAlloc, nothing to do.
        return;
    }

    EHExceptionRecord* pExcept = &this->m_EHRecord;

    if (PER_IS_MSVC_PURE_OR_NATIVE_EH(pExcept))
    {
        // this is a C++ exception
        // we need to delete the actual exception object
        ThrowInfo* pThrow= (ThrowInfo*) DecodePointerDownlevel((void*)PER_PTHROW(pExcept));
        if ( pThrow == NULL )
        {
            // No ThrowInfo exists.  If this were a C++ exception, something must have corrupted it.
            terminate();
        }

        if (PER_PEXCEPTOBJ(pExcept) != NULL) // we have an object to destroy
        {
#if _EH_RELATIVE_TYPEINFO
            const auto pType = (CatchableType *)
                (*(int*)THROW_CTLIST_IB(*pThrow, _GetThrowImageBase()) + _GetThrowImageBase());
#else
            CatchableType *const pType = *THROW_CTLIST(*pThrow);
#endif
            // calling the d-tor if there's one
            if (THROW_UNWINDFUNC(*pThrow) != NULL) // we have a dtor func
            {
                // it's a user defined type with a dtor
#if defined(_M_CEE_PURE)
                void (__clrcall * pDtor)(void*) =  (void (__clrcall *)(void *))(THROW_UNWINDFUNC(*pThrow));
                (*pDtor)(PER_PEXCEPTOBJ(pExcept));
#elif _EH_RELATIVE_TYPEINFO
                _CallMemberFunction0(PER_PEXCEPTOBJ(pExcept),
                  THROW_UNWINDFUNC_IB(*pThrow,(unsigned __int64)PER_PTHROWIB(pExcept)));
#else
                _CallMemberFunction0(PER_PEXCEPTOBJ(pExcept),
                  THROW_UNWINDFUNC(*pThrow));
#endif
            }
            else if (CT_ISWINRTHANDLE(*pType))
            {
                IUnknown* pUnknown = *(reinterpret_cast<IUnknown**>(PER_PEXCEPTOBJ(pExcept)));
                if (pUnknown)
                {
                    pUnknown->Release();
                }
            }
        }

        // de-allocating the memory of the exception object
        free(PER_PEXCEPTOBJ(pExcept));
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// [[noreturn]] void __ExceptionPtr::_RethrowException() const;
//
// This function rethrows the exception that is stored in the exception_ptr object.
// If the stored exception is a SEH exception, it simply calls RaiseException().
// If the stored exception is a C++ exception, it will try to allocate memory on
// the stack and copy the exception object onto the stack and calls RaiseException().
//
[[noreturn]] void __ExceptionPtr::_RethrowException() const
{
    // throwing a bad_exception if they give us a NULL exception_ptr
    if (this == NULL)
    {
        throw std::bad_exception();
    }

    EXCEPTION_RECORD ThisException = this->m_Record;
    EHExceptionRecord* pExcept = (EHExceptionRecord*)&ThisException; // just an alias for use of the macros

    if (PER_IS_MSVC_PURE_OR_NATIVE_EH(pExcept))
    {
        // this is a C++ exception
        // we need to build the exception on the stack
        // because the current exception mechanism assumes
        // the exception object is on the stack and will call
        // the appropriate dtor (if there's one), but won't
        // delete the memory.
        ThrowInfo* pThrow= (ThrowInfo*) DecodePointerDownlevel((void*)PER_PTHROW(pExcept));
        if (    PER_PEXCEPTOBJ(pExcept) == NULL ||
                pThrow == NULL ||
                pThrow->pCatchableTypeArray == NULL ||
                THROW_COUNT(*pThrow) <=0
                )
        {
            // No ThrowInfo exists.  If this were a C++ exception, something must have corrupted it.
            terminate();
        }
        PER_PTHROW(pExcept) = pThrow; // update the EXCEPTION_RECORD with the proper decoded pointer before rethrowing

        // we finally got the type info we want
#if _EH_RELATIVE_TYPEINFO
        const auto pType = (CatchableType *)
            ( *(int*)THROW_CTLIST_IB(*pThrow, _GetThrowImageBase()) + _GetThrowImageBase());
#else
        CatchableType *const pType = *THROW_CTLIST(*pThrow);
#endif

        // alloc memory on stack for exception object
        // this might cause StackOverFlow SEH exception,
        // in that case, we just let the SEH propagate
        void * pExceptionBuffer = alloca(CT_SIZE(*pType));

        __ExceptionPtr::_CallCopyCtor(pExceptionBuffer, PER_PEXCEPTOBJ(pExcept), CT_SIZE(*pType), pType);
        PER_PEXCEPTOBJ(pExcept) = pExceptionBuffer;
    }
    //else // this is a SEH exception, no special handling is required

    // This shouldn't happen.  Just to make prefast/OACR happy
    if (ThisException.NumberParameters > EXCEPTION_MAXIMUM_PARAMETERS)
    {
        ThisException.NumberParameters = EXCEPTION_MAXIMUM_PARAMETERS;
    }


    RaiseException( ThisException.ExceptionCode,
            ThisException.ExceptionFlags,
            ThisException.NumberParameters,
            ThisException.ExceptionInformation );
}

////////////////////////////////////////////////////////////////////////////////
//
// _StaticAlloc allocates memory from a buffer local to this TU
//
// Externally synchronized in __ExceptionPtr::_BadAllocException
//
static char _StaticAllocStorage[
    // in allocation order:
    MEMORY_ALLOCATION_ALIGNMENT // padding for alignment
    + sizeof(_Ref_count_obj_alloc<__ExceptionPtr, _StaticAllocator<int>>)
    + MEMORY_ALLOCATION_ALIGNMENT
    + sizeof(bad_alloc)];
static size_t _StaticAllocStorageLeft = sizeof(_StaticAllocStorage);
static void * _StaticAlloc(const size_t bytesToAllocate)
{
    void * allocBase = _StaticAllocStorage
        + sizeof(_StaticAllocStorage)
        - _StaticAllocStorageLeft;
    void * const pv = std::align(MEMORY_ALLOCATION_ALIGNMENT, bytesToAllocate, allocBase, _StaticAllocStorageLeft);
    if (pv)
    {
        _StaticAllocStorageLeft -= bytesToAllocate;
        return pv;
    }

    std::terminate();
}

////////////////////////////////////////////////////////////////////////////////
//
// Tiny wrappers for bridging the gap between std::exception_ptr and shared_ptr<__ExceptionPtr>.
// We are doing this because <memory> depends on <exception> which means <exception> cannot include <memory>.
// And shared_ptr<> is defined <memory>.  To workaround this, we created a dummy class
// std::exception_ptr, which is structurally identical to shared_ptr<>.
//

_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL __ExceptionPtrCreate(void* ptr)
{
    new (ptr) shared_ptr<__ExceptionPtr>();
}

_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL __ExceptionPtrDestroy(void* ptr)
{
    static_cast<shared_ptr<__ExceptionPtr>*>(ptr)->~shared_ptr<__ExceptionPtr>();
}

_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL __ExceptionPtrCopy(void* _dst, const void* _src)
{
    shared_ptr<__ExceptionPtr>* dst = static_cast<shared_ptr<__ExceptionPtr>*>(_dst);
    const shared_ptr<__ExceptionPtr>* src = static_cast<const shared_ptr<__ExceptionPtr>*>(_src);
    new (dst) shared_ptr<__ExceptionPtr>(*src);
}

_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL __ExceptionPtrAssign(void* _dst, const void* _src)
{
    shared_ptr<__ExceptionPtr>* dst = static_cast<shared_ptr<__ExceptionPtr>*>(_dst);
    const shared_ptr<__ExceptionPtr>* src = static_cast<const shared_ptr<__ExceptionPtr>*>(_src);
    dst->operator=(*src);
}

_CRTIMP2_PURE bool __CLRCALL_PURE_OR_CDECL __ExceptionPtrCompare(const void* _lhs, const void* _rhs)
{
    const shared_ptr<__ExceptionPtr>* lhs = static_cast<const shared_ptr<__ExceptionPtr>*>(_lhs);
    const shared_ptr<__ExceptionPtr>* rhs = static_cast<const shared_ptr<__ExceptionPtr>*>(_rhs);
    return operator==(*lhs, *rhs);
}

_CRTIMP2_PURE bool __CLRCALL_PURE_OR_CDECL __ExceptionPtrToBool(const void* _ptr)
{
    const shared_ptr<__ExceptionPtr>* ptr = static_cast<const shared_ptr<__ExceptionPtr>*>(_ptr);
    return !!*ptr;
}

_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL __ExceptionPtrSwap(void* _lhs, void* _rhs)
{
    shared_ptr<__ExceptionPtr>* lhs = static_cast<shared_ptr<__ExceptionPtr>*>(_lhs);
    shared_ptr<__ExceptionPtr>* rhs = static_cast<shared_ptr<__ExceptionPtr>*>(_rhs);
    lhs->swap(*rhs);
}

_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL __ExceptionPtrCurrentException(void* ptr)
{
    const shared_ptr<__ExceptionPtr> pExcept = __ExceptionPtr::_CurrentException();
    __ExceptionPtrCopy(ptr, static_cast<const void*>(&pExcept));
}

[[noreturn]] _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL __ExceptionPtrRethrow(const void* _ptr)
{
    const shared_ptr<__ExceptionPtr>* ptr = static_cast<const shared_ptr<__ExceptionPtr>*>(_ptr);
    (*ptr)->_RethrowException();
}

_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL __ExceptionPtrCopyException(_Inout_ void* _ptr,
    _In_ const void* _pExcept, _In_ const void* _pThrowInfo)
{
    shared_ptr<__ExceptionPtr>* ptr = static_cast<shared_ptr<__ExceptionPtr>*>(_ptr);
    (*ptr) = __ExceptionPtr::_CopyException(_pExcept, static_cast<const ThrowInfo*>(_pThrowInfo));
}
