//
// time.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The timespec_get() and  time() families of functions, which get the current
// system time as a timespec and a time_t, respectively.
//
#include <corecrt_internal_time.h>
#include <msvcrt_IAT.h>



template <typename TimeSpecType>
_Success_(return != 0)
static int __cdecl common_timespec_get(TimeSpecType* const ts, int const base) throw()
{
    typedef decltype(ts->tv_sec)                time_type;
    typedef __crt_time_time_t_traits<time_type> time_traits;

    _VALIDATE_RETURN(ts != nullptr, EINVAL, 0);
    
    if (base != TIME_UTC)
        return 0;

    __crt_filetime_union system_time{};
    __acrt_GetSystemTimePreciseAsFileTime(&system_time._filetime);

    __time64_t const filetime_scale{10 * 1000 * 1000}; // 100ns units

    __time64_t const epoch_time{static_cast<__time64_t>(system_time._scalar) - _EPOCH_BIAS};

    __time64_t const seconds    {epoch_time / filetime_scale};
    __time64_t const nanoseconds{epoch_time % filetime_scale * 100};

    if (seconds > static_cast<__time64_t>(time_traits::max_time_t))
        return 0;

    ts->tv_sec  = static_cast<time_type>(seconds);
    ts->tv_nsec = static_cast<long>(nanoseconds);
    return base;
}

extern "C" int __cdecl _timespec32_get_downlevel(_timespec32* const ts, int const base)
{
    return common_timespec_get(ts, base);
}

_LCRT_DEFINE_IAT_SYMBOL(_timespec32_get_downlevel);

extern "C" int __cdecl _timespec64_get_downlevel(_timespec64* const ts, int const base)
{
    return common_timespec_get(ts, base);
}

_LCRT_DEFINE_IAT_SYMBOL(_timespec64_get_downlevel);




// Gets the current system time and converts it to a time_t value. If 'result'
// is non-null, the time is stored in '*result'.  The time is also returned
// (even if 'result' is null).
template <typename TimeType>
static TimeType __cdecl common_time(TimeType* const result) throw()
{
    typedef __crt_time_time_t_traits<TimeType> time_traits;

    typename time_traits::timespec_type ts{};
    if (common_timespec_get(&ts, TIME_UTC) != TIME_UTC)
        ts.tv_sec = static_cast<TimeType>(-1);

    if (result)
        *result = ts.tv_sec;

    return ts.tv_sec;
}

#ifdef _ATL_XP_TARGETING

extern "C" __time32_t __cdecl _time32_downlevel(__time32_t* const result)
{
    return common_time(result);
}

_LCRT_DEFINE_IAT_SYMBOL(_time32_downlevel);

extern "C" __time64_t __cdecl _time64_downlevel(__time64_t* const result)
{
    return common_time(result);
}

_LCRT_DEFINE_IAT_SYMBOL(_time64_downlevel);


#endif
