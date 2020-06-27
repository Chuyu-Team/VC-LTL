// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// SchedulerPolicyBase.cpp
//
// Scheduler policy implementation
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Internal list of scheduler policy defaults.
    /// </summary>
    const unsigned int PolicyDefaults[] =
    {
        ::Concurrency::ThreadScheduler, // SchedulerKind
        MaxExecutionResources,          // MaxConcurrency
        1,                              // MinConcurrency
        1,                              // TargetOversubscriptionFactor
        8,                              // LocalContextCacheSize
        0,                              // ContextStackSize
        THREAD_PRIORITY_NORMAL,         // ContextPriority
        EnhanceScheduleGroupLocality,   // SchedulingProtocol
        ProgressFeedbackEnabled,        // DynamicProgressFeedback
        InitializeWinRTAsMTA,           // WinRTInitialization
    };

    /// <summary>
    ///     Internal map from policy keys to descriptive strings.
    /// </summary>
    const char* const PolicyElementKeyStrings[] =
    {
        "SchedulerKind",
        "MaxConcurrency",
        "MinConcurrency",
        "TargetOversubscriptionFactor",
        "LocalContextCacheSize",
        "ContextStackSize",
        "ContextPriority",
        "SchedulingProtocol",
        "DynamicProgressFeedback",
        "WinRTInitialization",
        "MaxPolicyElementKey"
    };
}

    /// <summary>
    ///     Creates a new default scheduler policy.
    /// </summary>
    SchedulerPolicy::SchedulerPolicy()
    {
        _Initialize(0, NULL);
    }

    /// <summary>
    ///     Creates a new scheduler policy that uses a named-parameter style of initialization.  Unnamed parameters take defaults described above.
    /// </summary>
    SchedulerPolicy::SchedulerPolicy(_In_ size_t _PolicyKeyCount, ...)
    {
        va_list args;
        va_start(args, _PolicyKeyCount);
        _Initialize(_PolicyKeyCount, &args);
    }

    /// <summary>
    ///     Initializes the scheduler policy.
    /// </summary>
    void SchedulerPolicy::_Initialize(_In_ size_t _PolicyKeyCount, va_list *_PArgs)
    {
        size_t bagSize = sizeof(unsigned int) * Concurrency::MaxPolicyElementKey;
        _PolicyBag *pPolicyBag = _concrt_new _PolicyBag;
        _M_pPolicyBag = pPolicyBag;

        try
        {
            memcpy(pPolicyBag->_M_values._M_pPolicyBag, PolicyDefaults, bagSize);

            for (size_t i = 0; i < _PolicyKeyCount; i++)
            {
                PolicyElementKey key = va_arg(*_PArgs, PolicyElementKey);
                unsigned int value = va_arg(*_PArgs, unsigned int);

                if ( !_ValidPolicyKey(key))
                    throw invalid_scheduler_policy_key(_StringFromPolicyKey(key));

                if ( !_ValidPolicyValue(key, value))
                     throw invalid_scheduler_policy_value(_StringFromPolicyKey(key));

                pPolicyBag->_M_values._M_pPolicyBag[key] = value;
            }

            if (!_AreConcurrencyLimitsValid())
            {
                throw invalid_scheduler_policy_thread_specification();
            }

            if (!_ArePolicyCombinationsValid())
            {
                throw invalid_scheduler_policy_value();
            }

            _ResolvePolicyValues();

        }
        catch (...)
        {
            delete pPolicyBag;
            throw;
        }
    }

    /// <summary>
    ///     The most convenient way to define a new scheduler policy is to copy
    ///     an existing policy and modify it. The copy constructor is also needed
    ///     for all the usual reasons.
    /// </summary>
    SchedulerPolicy::SchedulerPolicy(_In_ const SchedulerPolicy &srcPolicy)
    {
        _M_pPolicyBag = _concrt_new _PolicyBag;
        _Assign(srcPolicy);
    }

    /// <summary>
    ///     The most convenient way to define a new scheduler policy is to copy
    ///     an existing policy and modify it. The copy constructor is also needed
    ///     for all the usual reasons.
    /// </summary>
    SchedulerPolicy& SchedulerPolicy::operator=(_In_ const SchedulerPolicy &rhsPolicy)
    {
        _Assign(rhsPolicy);
        return *this;
    }

    /// <summary>
    ///     Make this policy a copy of the source policy.
    /// </summary>
    void SchedulerPolicy::_Assign(_In_ const SchedulerPolicy &rhsPolicy)
    {
        size_t bagSize = sizeof(unsigned int) * Concurrency::MaxPolicyElementKey;
        memcpy(_M_pPolicyBag->_M_values._M_pPolicyBag, rhsPolicy._M_pPolicyBag->_M_values._M_pPolicyBag, bagSize);
    }

    /// <summary>
    ///     Destroys a scheduler policy.
    /// </summary>
    SchedulerPolicy::~SchedulerPolicy()
    {
        delete _M_pPolicyBag;
    }

    /// <summary>
    ///     Retrieve the value of the supplied policy key.
    /// </summary>
    /// <param name="key">
    ///     [in] The policy key.
    /// </param>
    /// <returns>
    ///     The policy key value for the key, if is a supported key.
    /// </returns>
    /// <remarks>
    ///     The function will throw "invalid_scheduler_policy_key" for any key that is not supported.
    /// </remarks>
    unsigned int SchedulerPolicy::GetPolicyValue(PolicyElementKey key) const
    {
        if (!_ValidPolicyKey(key))
        {
            throw invalid_scheduler_policy_key(_StringFromPolicyKey(key));
        }

        return _M_pPolicyBag->_M_values._M_pPolicyBag[key];
    }

    /// <summary>
    ///     Set the value of the supplied policy key and return the old value.
    /// </summary>
    /// <param name="key">
    ///     [in] The policy key.
    /// </param>
    /// <param name="value">
    ///     [in] The value for the policy key.
    /// </param>
    /// <returns>
    ///     The old policy key value for the key, if is a supported key.
    /// </returns>
    /// <remarks>
    ///     The function will throw "invalid_scheduler_policy_key" for any key that is not supported,
    ///     and "invalid_scheduler_policy_value" for a value that is not supported for a valid key.
    /// </remarks>
    unsigned int SchedulerPolicy::SetPolicyValue(PolicyElementKey key, unsigned int value)
    {
        if (!_ValidPolicyKey(key)
            || key == ::Concurrency::MinConcurrency
            || key == ::Concurrency::MaxConcurrency)
        {
            throw invalid_scheduler_policy_key(_StringFromPolicyKey(key));
        }

        if (!_ValidPolicyValue(key, value))
        {
            throw invalid_scheduler_policy_value(_StringFromPolicyKey(key));
        }

        unsigned int oldValue = GetPolicyValue(key);
        _M_pPolicyBag->_M_values._M_pPolicyBag[key] = value;

        _ResolvePolicyValues();
        return oldValue;
    }

    /// <summary>
    ///     Set the value of the supplied policy key and return the old value.
    /// </summary>
    /// <param name="_MinConcurrency">
    ///     [in] The value for MinConcurrency.
    /// </param>
    /// <param name="_MaxConcurrency">
    ///     [in] The value for MaxConcurrency.
    /// </param>
    /// <remarks>
    ///     The function will throw "invalid_scheduler_policy_value" if:
    ///     _MaxConcurrency != MaxExecutionResources && _MinConcurrency > _MaxConcurrency
    ///</remarks>
    void SchedulerPolicy::SetConcurrencyLimits(_In_ unsigned int _MinConcurrency, _In_ unsigned int _MaxConcurrency)
    {
        if (!_ValidPolicyValue(::Concurrency::MaxConcurrency, _MaxConcurrency))
            throw invalid_scheduler_policy_value(_StringFromPolicyKey(::Concurrency::MaxConcurrency));

        if (!_ValidPolicyValue(::Concurrency::MinConcurrency, _MinConcurrency))
            throw invalid_scheduler_policy_value(_StringFromPolicyKey(::Concurrency::MinConcurrency));

        if (!_AreConcurrencyLimitsValid(_MinConcurrency, _MaxConcurrency))
            throw invalid_scheduler_policy_thread_specification();

        if (!_ArePolicyCombinationsValid())
            throw invalid_scheduler_policy_value();

        _M_pPolicyBag->_M_values._M_pPolicyBag[::Concurrency::MaxConcurrency] = _MaxConcurrency;
        _M_pPolicyBag->_M_values._M_pPolicyBag[::Concurrency::MinConcurrency] = _MinConcurrency;

        _ResolvePolicyValues();
    }

    /// <summary>
    ///     Resolves some of the policy keys that are set to defaults, based on the characteristics of the underlying system.
    /// </summary>
    void SchedulerPolicy::_ResolvePolicyValues()
    {
        // Resolve the SchedulerKind policy key value.
        _M_pPolicyBag->_M_values._M_pPolicyBag[::Concurrency::SchedulerKind] = ::Concurrency::ThreadScheduler;

        // Resolve MinConcurrency and MaxConcurrency, if either of them are set to the special value MaxExecutionResources.
        unsigned int coreCount = ::Concurrency::GetProcessorCount();
        ASSERT((coreCount > 0) && (coreCount <= INT_MAX));

        if (_M_pPolicyBag->_M_values._M_pPolicyBag[MinConcurrency] == MaxExecutionResources)
        {
            if (_M_pPolicyBag->_M_values._M_pPolicyBag[MaxConcurrency] == MaxExecutionResources)
            {
                // [1] Both the keys are set to MaxExecutionResources.
                _M_pPolicyBag->_M_values._M_pPolicyBag[MinConcurrency] = _M_pPolicyBag->_M_values._M_pPolicyBag[MaxConcurrency] = coreCount;
            }
            else
            {
                // [2] MinConcurrency is set to MaxExecutionResources.
                _M_pPolicyBag->_M_values._M_pPolicyBag[MinConcurrency] = (_M_pPolicyBag->_M_values._M_pPolicyBag[MaxConcurrency] < coreCount) ?
                    _M_pPolicyBag->_M_values._M_pPolicyBag[MaxConcurrency] : coreCount;
            }
        }
        else if (_M_pPolicyBag->_M_values._M_pPolicyBag[MaxConcurrency] == MaxExecutionResources)
        {
            // [3] MaxConcurrency is set to MaxExecutionResources.
            _M_pPolicyBag->_M_values._M_pPolicyBag[MaxConcurrency] = (_M_pPolicyBag->_M_values._M_pPolicyBag[MinConcurrency] > coreCount) ?
                _M_pPolicyBag->_M_values._M_pPolicyBag[MinConcurrency] : coreCount;
        }

        ASSERT(_M_pPolicyBag->_M_values._M_pPolicyBag[MaxConcurrency] >= _M_pPolicyBag->_M_values._M_pPolicyBag[MinConcurrency]);
    }


    const char* SchedulerPolicy::_StringFromPolicyKey(unsigned int index)
    {
        if (index > ::Concurrency::MaxPolicyElementKey)
            index = ::Concurrency::MaxPolicyElementKey;

        return PolicyElementKeyStrings[index];
    }

    bool SchedulerPolicy::_ValidPolicyKey(PolicyElementKey key)
    {
        return (key >= SchedulerKind && key < MaxPolicyElementKey);
    }

    bool SchedulerPolicy::_ValidPolicyValue(PolicyElementKey key, unsigned int value)
    {
        bool valid = true;

        switch (key)
        {
        case ::Concurrency::SchedulerKind:
            if ( value != ::Concurrency::ThreadScheduler )
            {
                valid = false;
            }
            break;
        case ::Concurrency::ContextPriority:
            {
                int priority = (int)value;
                //
                // The win32 api accepts values [-7, 7), 15 and -15 for threads other than the current thread.
                // In addition, we define a special value INHERIT_THREAD_PRIORITY, whereby the internal contexts
                // inherit the priority of the thread creating the scheduler
                //
                if ( !(priority >= -7 && priority < 7
                    || priority == 15
                    || priority == -15
                    || priority == INHERIT_THREAD_PRIORITY))
                {
                    valid = false;
                }
            }
            break;
        case ::Concurrency::SchedulingProtocol:
            if ( !(value == ::Concurrency::EnhanceScheduleGroupLocality
                || value == ::Concurrency::EnhanceForwardProgress))
            {
                valid = false;
            }
            break;
        case ::Concurrency::MaxConcurrency:
            if ( !((value > 0 && value <= INT_MAX) || value == MaxExecutionResources))
            {
                valid = false;
            }
            break;
        case ::Concurrency::MinConcurrency:
            if ( !((value <= INT_MAX) || value == MaxExecutionResources))
            {
                valid = false;
            }
            break;
        case ::Concurrency::LocalContextCacheSize:
        case ::Concurrency::ContextStackSize:
            if ( !(value <= INT_MAX))
            {
                valid = false;
            }
            break;
        case ::Concurrency::TargetOversubscriptionFactor:
            if ( !(value > 0 && value <= INT_MAX))
            {
                valid = false;
            }
            break;

        case ::Concurrency::DynamicProgressFeedback:
            if ( !(value == ::Concurrency::ProgressFeedbackEnabled || value == ::Concurrency::ProgressFeedbackDisabled))
            {
                valid = false;
            }
            break;

        case ::Concurrency::WinRTInitialization:
            if ( !(value == ::Concurrency::InitializeWinRTAsMTA || value == ::Concurrency::DoNotInitializeWinRT))
            {
                valid = false;
            }
            break;

        case ::Concurrency::MaxPolicyElementKey:
        default:
            terminate();
        }

        return valid;
    }

    void SchedulerPolicy::_ValidateConcRTPolicy() const
    {
        unsigned int minConcurrency = GetPolicyValue(::Concurrency::MinConcurrency);
        if (minConcurrency == 0)
        {
            throw invalid_scheduler_policy_value(_StringFromPolicyKey(::Concurrency::MinConcurrency));
        }

        ::Concurrency::DynamicProgressFeedbackType dynamicProgress =
            (::Concurrency::DynamicProgressFeedbackType) GetPolicyValue(::Concurrency::DynamicProgressFeedback);

        if (dynamicProgress == ProgressFeedbackDisabled)
        {
            throw invalid_scheduler_policy_value(_StringFromPolicyKey(::Concurrency::DynamicProgressFeedback));
        }
    }

    /// <summary>
    ///     Test a policy's concurrency limits.
    /// </summary>
    bool SchedulerPolicy::_AreConcurrencyLimitsValid(unsigned int _MinConcurrency, unsigned int _MaxConcurrency)
    {
        // For concurrency limits that are != MaxExecutionResource, plug into the equation: _MinConcurrency <= _MaxConcurrency,
        // and return false, if it does not hold.

        // Validate Max
        if ((_MaxConcurrency != MaxExecutionResources)
            && (_MinConcurrency != MaxExecutionResources) && (_MaxConcurrency < _MinConcurrency))
        {
            return false;
        }

        return true;
    }

    /// <summary>
    ///     Test a policy's concurrency limits.
    /// </summary>
    bool SchedulerPolicy::_AreConcurrencyLimitsValid() const
    {
        return _AreConcurrencyLimitsValid(GetPolicyValue(::Concurrency::MinConcurrency),
                                    GetPolicyValue(::Concurrency::MaxConcurrency));
    }

    /// <summary>
    ///     Test a policy's concurrency limits.
    /// </summary>
    bool SchedulerPolicy::_ArePolicyCombinationsValid() const
    {
        return true;
    }
} // namespace Concurrency
