/***
* ==++==
*
* Copyright (c) Microsoft Corporation. All rights reserved.
*
* ==--==
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*
* taskscheduler.cpp
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/

#include <ppltaskscheduler.h>
#include <Windows.h>
#include <awint.h>
#include <stdexcept>
#include <exception>

namespace Concurrency { namespace details {
    namespace
    {
        void CALLBACK task_scheduler_callback(PTP_CALLBACK_INSTANCE, PVOID args, PTP_WORK)
        {
            auto _Chore = static_cast<_Threadpool_chore *>(args);
            _Chore->_M_callback(_Chore->_M_data);
        }

		//已经转移到winapi_thunks.h
        //bool is_vista_threadpool_supported()
        //{
        //    #if _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA
        //    return true;
        //    #else
        //    DYNAMICGETCACHEDFUNCTION(KERNEL32, PFNCREATETHREADPOOLWORK, CreateThreadpoolWork, pfCreateThreadpoolWork);
        //    return pfCreateThreadpoolWork != nullptr;
        //    #endif // _STL_WIN32_WINNT >= _WIN32_WINNT_VISTA
        //}
		#define is_vista_threadpool_supported __crt__Is_vista_threadpool_supported


        DWORD __stdcall task_scheduler_callback_xp(LPVOID args)
        {
            auto _Chore = static_cast<_Threadpool_chore *>(args);
            _Chore->_M_callback(_Chore->_M_data);
            return 0;
        }
    }

    _CRTIMP2 void __cdecl _Release_chore(_Threadpool_chore *_Chore)
    {
        if (_Chore->_M_work != nullptr)
        {
            // XP threadpool doesn't need to release chore
            if (is_vista_threadpool_supported())
            {
                __crtCloseThreadpoolWork(static_cast<PTP_WORK>(_Chore->_M_work));
            }
            _Chore->_M_work = nullptr;
        }
    }

    _CRTIMP2 int __cdecl _Reschedule_chore(const _Threadpool_chore *_Chore)
    {
        // reschedule supports only vista and above
        _ASSERT(is_vista_threadpool_supported());
        _ASSERT(_Chore->_M_work);
        __crtSubmitThreadpoolWork(static_cast<PTP_WORK>(_Chore->_M_work));
        return 0;
    }

    _CRTIMP2 int __cdecl _Schedule_chore(_Threadpool_chore *_Chore)
    {
        _ASSERT(_Chore->_M_work == nullptr);
        _ASSERT(_Chore->_M_callback != nullptr);

        if (is_vista_threadpool_supported())
        {
            _Chore->_M_work = __crtCreateThreadpoolWork(task_scheduler_callback, _Chore, nullptr);

            if (_Chore->_M_work)
                return _Reschedule_chore(_Chore);
            else
                return static_cast<int>(GetLastError()); // LastError won't be 0 when it's in error state
        }
        else
        {
            // XP supporting
            _Chore->_M_work = _Chore; // give a dummy non-null worker
            if (__crtQueueUserWorkItem(task_scheduler_callback_xp, _Chore, WT_EXECUTEDEFAULT) == 0)
            {
                _Chore->_M_work = nullptr;
                return static_cast<int>(GetLastError()); // LastError won't be 0 when it's in error state
            }
            else
                return 0;
        }
    }
} }
