// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// Timer.cpp
//
// Shared timer implementation.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

#pragma warning (disable : 4702)

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     A stub class that is friends with _Timer in order to avoid pulling in too many windows.h definitions into agents.h.
    /// </summary>
    class _TimerStub
    {
    public:

        /// <summary>
        /// Timer callback for Vista and above (except MSDK)
        /// </summary>
        static void CALLBACK FireTimer(PTP_CALLBACK_INSTANCE, void* context, PTP_TIMER)
        {
            FireTimerXP(context, true);
        }

        /// <summary>
        /// Timer callback for XP and MSDK
        /// </summary>
        static void CALLBACK FireTimerXP(PVOID pContext, BOOLEAN)
        {
            _Timer* pTimer = reinterpret_cast<_Timer *>(pContext);
            pTimer->_Fire();
            // Do not delete the timer - it will be deleted in the destructor
        }
    };

    /// <summary>
    ///     Constructs a new timer.
    /// </summary>
    /// <param name="_Ms">
    ///     The duration and period of the timer in milliseconds.
    /// </param>
    /// <param name="_FRepeating">
    ///     An indication of whether the timer is repeating (periodic) or not.
    /// </param>
    _Timer::_Timer(unsigned int _Ms, bool _FRepeating)
        : _M_hTimer(NULL)
        , _M_ms(_Ms)
        , _M_fRepeating(_FRepeating)
    {
    }

    /// <summary>
    ///     Starts the timer.
    /// </summary>
    void _Timer::_Start()
    {
        if (_M_hTimer == NULL)
        {
            if (Concurrency::GetOSVersion() >= Concurrency::IResourceManager::Vista)
            {
                if ((_M_hTimer = RegisterAsyncTimerAndLoadLibrary(_M_ms, &_TimerStub::FireTimer, this, _M_fRepeating)) == nullptr)
                {
                    throw std::bad_alloc();
                }
            }
            else
            {
                if (!platform::__CreateTimerQueueTimer(&_M_hTimer,
                                           GetSharedTimerQueue(),
                                           &_TimerStub::FireTimerXP,
                                           reinterpret_cast<PVOID> (this),
                                           _M_ms,
                                           _M_fRepeating ? _M_ms : 0,
                                           WT_EXECUTEINTIMERTHREAD))
                    throw std::bad_alloc();
            }
        }
    }

    /// <summary>
    ///     Destroys the timer.
    /// </summary>
    _Timer::~_Timer()
    {
        if (_M_hTimer != NULL)
            _Stop();
    }

    /// <summary>
    ///     Stops the timer.
    /// </summary>
    void _Timer::_Stop()
    {
        if (Concurrency::GetOSVersion() >= Concurrency::IResourceManager::Vista)
        {
            DeleteAsyncTimerAndUnloadLibrary(static_cast<PTP_TIMER>(_M_hTimer));
        }
        else
        {
            platform::__DeleteTimerQueueTimer(GetSharedTimerQueue(), _M_hTimer, INVALID_HANDLE_VALUE);
        }
        _M_hTimer = NULL;
    }
} // namespace details

/// <summary>
///     Wait for a specified number of milliseconds
/// </summary>
_CONCRTIMP void __cdecl wait(unsigned int milliseconds)
{
    if (milliseconds < 1)
    {
        Context::Yield();
    }
    else
    {
        class TimerObj : public _Timer
        {
        public:

            TimerObj(unsigned int mS) : _Timer(mS, false)
            {
                m_pContext = Context::CurrentContext();
                _Start();
                Context::Block();
            }

        private:

            virtual void _Fire()
            {
                m_pContext->Unblock();
            }

            Context *m_pContext;

        } _t(milliseconds);
    }
}
} // namespace Concurrency
