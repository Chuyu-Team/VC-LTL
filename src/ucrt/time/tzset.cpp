//
// tzset.cpp
//
//      Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Defines the _tzset() function which updates the global time zone state, and
// the _isindst() function, which tests whether a time is in Daylight Savings 
// Time or not.
//
#include <corecrt_internal_time.h>
#include <locale.h>



_DEFINE_SET_FUNCTION(_set_daylight, int,  _daylight)
_DEFINE_SET_FUNCTION(_set_dstbias,  long, _dstbias )
_DEFINE_SET_FUNCTION(_set_timezone, long, _timezone)



// Pointer to a saved copy of the TZ value obtained in the previous call to the
// tzset functions, if one is available:
static char* last_tz = nullptr;

// If the time zone was last updated by calling the system API, then the tz_info
// variable contains the time zone information and tz_api_used is set to true.
static int                   tz_api_used;
static TIME_ZONE_INFORMATION tz_info;

namespace
{
    // Structure used to represent DST transition date/times:
    struct transitiondate
    {
        int  yr; // year of interest
        int  yd; // day of year
        int  ms; // milli-seconds in the day
    };

    enum class date_type
    {
        absolute_date,
        day_in_month
    };

    enum class transition_type
    {
        start_of_dst,
        end_of_dst
    };

    size_t const local_env_buffer_size = 256;
    int    const milliseconds_per_day  = 24 * 60 * 60 * 1000;
}

// DST start and end structures:
static transitiondate dststart = { -1, 0, 0 };
static transitiondate dstend   = { -1, 0, 0 };



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// The _tzset() family of functions
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Gets the value of the TZ environment variable.  If there is no TZ environment
// variable or if we do not have access to the environment, nullptr is returned.
// If the value of the TZ variable fits into the local_buffer, it is stored there
// and a pointer to the local_buffer is returned.  Otherwise, a buffer is
// dynamically allocated, the value is stored into that buffer, and a pointer to
// that buffer is returned.  In this case, the caller is responsible for freeing
// the buffer.
static char* get_tz_environment_variable(char (&local_buffer)[local_env_buffer_size]) throw()
{
    size_t required_length;
    errno_t const status = getenv_s(&required_length, local_buffer, local_env_buffer_size, "TZ");
    if (status == 0)
        return local_buffer;

    if (status != ERANGE)
        return nullptr;

    __crt_unique_heap_ptr<char> dynamic_buffer(_malloc_crt_t(char, required_length));
    if (dynamic_buffer.get() == nullptr)
        return nullptr;

    size_t actual_length;
    if (getenv_s(&actual_length, dynamic_buffer.get(), required_length, "TZ") != 0)
        return nullptr;

    return dynamic_buffer.detach();
}

// Handles the _tzset if and only if there is no TZ environment variable.  In
// this case, we attempt to use the time zone information from the system.
static void __cdecl tzset_from_system_nolock() throw()
{
    _BEGIN_SECURE_CRT_DEPRECATION_DISABLE
    char** tzname = _tzname;
    _END_SECURE_CRT_DEPRECATION_DISABLE

    long timezone = 0;
    int  daylight = 0;
    long dstbias  = 0;
    _ERRCHECK(_get_timezone(&timezone));
    _ERRCHECK(_get_daylight(&daylight));
    _ERRCHECK(_get_dstbias (&dstbias ));

    // If there is a last_tz already, discard it:
    _free_crt(last_tz);
    last_tz = nullptr;

    if (GetTimeZoneInformation(&tz_info) != 0xFFFFFFFF)
    {
        // Record that the API was used:
        tz_api_used = 1;

        // Derive _timezone value from Bias and StandardBias fields.
        timezone = tz_info.Bias * 60;

        if (tz_info.StandardDate.wMonth != 0)
            timezone += tz_info.StandardBias * 60;

        // Check to see if there is a daylight time bias. Since the StandardBias
        // has been added into _timezone, it must be compensated for in the
        // value computed for _dstbias:
        if (tz_info.DaylightDate.wMonth != 0 && tz_info.DaylightBias != 0)
        {
            daylight = 1;
            dstbias = (tz_info.DaylightBias - tz_info.StandardBias) * 60;
        }
        else
        {
            daylight = 0;

            // Set the bias to 0 because GetTimeZoneInformation may return
            // TIME_ZONE_ID_DAYLIGHT even though there is no DST (e.g., in NT
            // 3.51, this can happen if automatic DST adjustment is disabled
            // in the Control Panel.
            dstbias = 0;
        }

        // Try to grab the name strings for both the time zone and the daylight
        // zone.  Note the wide character strings in tz_info must be converted
        // to multibyte character strings.  The locale code page must be used 
        // for this.  Note that if setlocale() has not yet been called with 
        // LC_ALL or LC_CTYPE, then the code page will be 0, which is CP_ACP,
        // so we will use the host's default ANSI code page.
        //
        // CRT_REFACTOR TODO We use the current locale for this transformation.
        // If per-thread locale has been enabled for this thread, then we'll be
        // using this thread's locale to update a global variable that is
        // accessed from multiple threads.  Does the time zone information also
        // need to be stored per-thread?
        unsigned const code_page = ___lc_codepage_func();

        int used_default_char;
        int const status0 = WideCharToMultiByte(
            code_page,
            0,
            tz_info.StandardName,
            -1,
            tzname[0],
            _TZ_STRINGS_SIZE - 1,
            nullptr,
            &used_default_char);

        if (status0 != 0 && !used_default_char)
        {
            tzname[0][_TZ_STRINGS_SIZE - 1] = '\0';
        }
        else
        {
            tzname[0][0] = '\0';
        }

        int const status1 = WideCharToMultiByte(
            code_page,
            0,
            tz_info.DaylightName,
            -1,
            tzname[1],
            _TZ_STRINGS_SIZE - 1,
            nullptr,
            &used_default_char);

        if (status1 != 0 && !used_default_char)
        {
            tzname[1][_TZ_STRINGS_SIZE - 1] = '\0';
        }
        else
        {
            tzname[1][0] = '\0';
        }
    }

    _set_timezone(timezone);
    _set_daylight(daylight);
    _set_dstbias(dstbias);
}

static void __cdecl tzset_from_environment_nolock(_In_z_ char* tz_env) throw()
{
    _BEGIN_SECURE_CRT_DEPRECATION_DISABLE
    char** tzname = _tzname;
    _END_SECURE_CRT_DEPRECATION_DISABLE
            
    long timezone = 0;
    int  daylight = 0;
    _ERRCHECK(_get_timezone(&timezone));
    _ERRCHECK(_get_daylight(&daylight));

    // Check to see if the TZ value is unchanged from an earlier call to this
    // function.  If it hasn't changed, we have no work to do:
    if (last_tz != nullptr && strcmp(tz_env, last_tz) == 0)
        return;

    // Update the global last_tz variable:
    _free_crt(last_tz);
    last_tz = _malloc_crt_t(char, strlen(tz_env) + 1).detach();
    if (last_tz == nullptr)
        return;

    _ERRCHECK(strcpy_s(last_tz, strlen(tz_env) + 1, tz_env));

    // Process TZ value and update _tzname, _timezone and _daylight.
    rsize_t tz_name_length = 3;
    _ERRCHECK(strncpy_s(tzname[0], _TZ_STRINGS_SIZE, tz_env, tz_name_length));

    // Skip first few characters if present.
    while (tz_name_length > 0 && *tz_env)
    {
        tz_name_length -= 1;
        tz_env += 1;
    }

    // The time difference is of the form:
    //     [+|-]hh[:mm[:ss]]
    // Check for the minus sign first:
    bool const is_negative_difference = *tz_env == '-';
    if (is_negative_difference)
        ++tz_env;

    // process, then skip over, the hours
    timezone = atol(tz_env) * 3600;
    while (*tz_env == '+' || (*tz_env >= '0' && *tz_env <= '9'))
        ++tz_env;

    // Check if minutes were specified:
    if (*tz_env == ':')
    {
        // Process, then skip over, the minutes
        timezone += atol(++tz_env) * 60;
        while (*tz_env >= '0' && *tz_env <= '9')
            ++tz_env;

        // Check if seconds were specified:
        if (*tz_env == ':')
        {
            // Process, then skip over, the seconds:
            timezone += atol(++tz_env);
            while (*tz_env >= '0' && *tz_env <= '9') 
                ++tz_env;
        }
    }

    if (is_negative_difference)
        timezone = -timezone;

    // Finally, check for a DST zone suffix:
    daylight = *tz_env ? 1 : 0;

    if (daylight)
    {
        _ERRCHECK(strncpy_s(tzname[1], _TZ_STRINGS_SIZE, tz_env, 3));
    }
    else
    {
        *tzname[1] = '\0';
    }

    _set_timezone(timezone);
    _set_daylight(daylight);
}

static void __cdecl tzset_nolock() throw()
{
    // Clear the flag indicated whether GetTimeZoneInformation was used.
    tz_api_used = 0;

    // Set year fields of dststart and dstend structures to -1 to ensure
    // they are recomputed as after this
    dststart.yr = dstend.yr = -1;

    // Get the value of the TZ environment variable:
    char local_env_buffer[local_env_buffer_size];
    char* const tz_env = get_tz_environment_variable(local_env_buffer);

    // If the buffer ended up being dynamically allocated, make sure we
    // clean it up before we return:
    __crt_unique_heap_ptr<char> tz_env_cleanup(tz_env == local_env_buffer
        ? nullptr
        : tz_env);

    // If the environment variable is not available for whatever reason, update
    // without using the environment (note that unless the Desktop CRT is loaded
    // and we have access to non-MSDK APIs, we will always tak this path).
    if (tz_env == nullptr || tz_env[0] == '\0')
        return tzset_from_system_nolock();

    return tzset_from_environment_nolock(tz_env);
}



// Sets the time zone information and calculates whether we are currently in
// Daylight Savings Time.  This reads the TZ environment variable, if that
// variable exists and can be read by the process; otherwise, the system is
// queried for the current time zone state.  The _daylight, _timezone, and
// _tzname global varibales are updated accordingly.
extern "C" void __cdecl _tzset()
{
    __acrt_lock(__acrt_time_lock);
    __try
    {
        tzset_nolock();
    }
    __finally
    {
        __acrt_unlock(__acrt_time_lock);
    }
}



// This function may be called to ensure that the time zone information ha sbeen
// set at least once.  If the time zone information has not yet been set, this
// function sets it.
extern "C" void __cdecl __tzset()
{
    static long first_time = 0;

    if (__crt_interlocked_read(&first_time) != 0)
        return;

    __acrt_lock(__acrt_time_lock);
    __try
    {
        if (__crt_interlocked_read(&first_time) != 0)
            __leave;

        tzset_nolock();

        _InterlockedIncrement(&first_time);
    }
    __finally
    {
        __acrt_unlock(__acrt_time_lock);
    }
}



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// The _isindst() family of functions
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Converts the format of a transition date specification to a value of a
// transitiondate structure.  The dststart and dstend global variables are
// filled in with the converted date.
static void __cdecl cvtdate(
    transition_type const trantype,  // start or end of DST
    date_type       const datetype,  // Day-in-month or absolute date
    int             const year,      // Year, as an offset from 1900
    int             const month,     // Month, where 0 is January
    int             const week,      // Week of month, if datetype is day-in-month
    int             const dayofweek, // Day of week, if datetype is day-in-month
    int             const date,      // Date of month (1 - 31)
    int             const hour,      // Hours (0 - 23)
    int             const min,       // Minutes (0 - 59)
    int             const sec,       // Seconds (0 - 59)
    int             const msec       // Milliseconds (0 - 999)
    ) throw()
{
    int yearday;
    int monthdow;
    long dstbias = 0;

    if (datetype == date_type::day_in_month)
    {
        // Figure out the year-day of the start of the month:
        yearday = 1 + (__crt_time_is_leap_year(year)
            ? _lpdays[month - 1]
            : _days[month - 1]);

        // Figureo ut the day of the week of the start of the month:
        monthdow = (yearday + ((year - 70) * 365) +
                    __crt_time_elapsed_leap_years(year) + _BASE_DOW) % 7;

        // Figure out the year-day of the transition date:
        if (monthdow <= dayofweek)
            yearday += (dayofweek - monthdow) + (week - 1) * 7;
        else
            yearday += (dayofweek - monthdow) + week * 7;

        // We may have to adjust the calculation above if week == 5 (meaning the
        // last instance of the day in the month).  Check if the year falls
        // beyond after month and adjust accordingly:
        int const days_to_compare = __crt_time_is_leap_year(year)
            ? _lpdays[month]
            : _days[month];

        if (week == 5 && yearday > days_to_compare)
        {
            yearday -= 7;
        }
    }
    else
    {
        yearday = __crt_time_is_leap_year(year)
            ? _lpdays[month - 1]
            : _days[month - 1];

        yearday += date;
    }

    if (trantype == transition_type::start_of_dst)
    {
        dststart.yd = yearday;
        dststart.ms = msec + (1000 * (sec + 60 * (min + 60 * hour)));

        // Set the year field of dststart so that unnecessary calls to cvtdate()
        // may be avoided:
        dststart.yr = year;
    }
    else // end_of_dst
    {
        dstend.yd = yearday;
        dstend.ms = msec + (1000 * (sec + 60 * (min + 60 * hour)));

        // The converted date is still a DST date.  We must convert to a standard
        // (local) date while being careful the millisecond field does not
        // overflow or underflow
        _ERRCHECK(_get_dstbias(&dstbias));
        dstend.ms += (dstbias * 1000);
        if (dstend.ms < 0)
        {
            dstend.ms += milliseconds_per_day;
            dstend.yd--;
        }
        else if (dstend.ms >= milliseconds_per_day)
        {
            dstend.ms -= milliseconds_per_day;
            dstend.yd++;
        }

        // Set the year field of dstend so that unnecessary calls to cvtdate()
        // may be avoided:
        dstend.yr = year;
    }

    return;
}



// Implementation Details:  Note that there are two ways that the Daylight
// Savings Time transition data may be returned by GetTimeZoneInformation.  The
// first is a day-in-month format, which is similar to what is used in the USA.
// The transition date is given as the n'th occurrence of a specified day of the
// week in a specified month.  The second is as an absolute date.  The two cases
// are distinguished by the value of the wYear field of the SYSTEMTIME structure
// (zero denotes a day-in-month format).
static int __cdecl _isindst_nolock(tm* const tb) throw()
{
    int daylight = 0;
    _ERRCHECK(_get_daylight(&daylight));
    if (daylight == 0)
        return 0;

    // Compute (or recompute) the transition dates for Daylight Savings Time
    // if necessary.  The yr fields of dststart and dstend are compared to the
    // year of interest to determine necessity.
    if (tb->tm_year != dststart.yr || tb->tm_year != dstend.yr)
    {
        if (tz_api_used)
        {
            // Convert the start of daylight savings time to dststart:
            if (tz_info.DaylightDate.wYear == 0)
            {
                cvtdate(
                    transition_type::start_of_dst,
                    date_type::day_in_month,
                    tb->tm_year,
                    tz_info.DaylightDate.wMonth,
                    tz_info.DaylightDate.wDay,
                    tz_info.DaylightDate.wDayOfWeek,
                    0,
                    tz_info.DaylightDate.wHour,
                    tz_info.DaylightDate.wMinute,
                    tz_info.DaylightDate.wSecond,
                    tz_info.DaylightDate.wMilliseconds);
            }
            else
            {
                cvtdate(
                    transition_type::start_of_dst,
                    date_type::absolute_date,
                    tb->tm_year,
                    tz_info.DaylightDate.wMonth,
                    0,
                    0,
                    tz_info.DaylightDate.wDay,
                    tz_info.DaylightDate.wHour,
                    tz_info.DaylightDate.wMinute,
                    tz_info.DaylightDate.wSecond,
                    tz_info.DaylightDate.wMilliseconds);
            }

            // Convert start of standard time to dstend:
            if (tz_info.StandardDate.wYear == 0)
            {
                cvtdate(
                    transition_type::end_of_dst,
                    date_type::day_in_month,
                    tb->tm_year,
                    tz_info.StandardDate.wMonth,
                    tz_info.StandardDate.wDay,
                    tz_info.StandardDate.wDayOfWeek,
                    0,
                    tz_info.StandardDate.wHour,
                    tz_info.StandardDate.wMinute,
                    tz_info.StandardDate.wSecond,
                    tz_info.StandardDate.wMilliseconds);
            }
            else
            {
                cvtdate(
                    transition_type::end_of_dst,
                    date_type::absolute_date,
                    tb->tm_year,
                    tz_info.StandardDate.wMonth,
                    0,
                    0,
                    tz_info.StandardDate.wDay,
                    tz_info.StandardDate.wHour,
                    tz_info.StandardDate.wMinute,
                    tz_info.StandardDate.wSecond,
                    tz_info.StandardDate.wMilliseconds);
            }
        }
        else
        {
            // The GetTimeZoneInformation API was not used, or failed.  We use
            // the USA Daylight Savings Time rules as a fallback.
            int startmonth = 3; // March
            int startweek  = 2; // Second week
            int endmonth   = 11;// November
            int endweek    = 1; // First week

            // The rules changed in 2007:
            if (107 > tb->tm_year)
            {
                startmonth = 4; // April
                startweek  = 1; // first week
                endmonth   = 10;// October
                endweek    = 5; // last week
            }

            cvtdate(
                transition_type::start_of_dst,
                date_type::day_in_month,
                tb->tm_year,
                startmonth,
                startweek,
                0, // Sunday
                0,
                2, // 02:00 (2 AM)
                0,
                0,
                0);

            cvtdate(
                transition_type::end_of_dst,
                date_type::day_in_month,
                tb->tm_year,
                endmonth,
                endweek,
                0, // Sunday
                0,
                2, // 02:00 (2 AM)
                0,
                0,
                0);
        }
    }

    // Handle simple cases first:
    if (dststart.yd < dstend.yd)
    {
        // Northern hemisphere ordering:
        if (tb->tm_yday < dststart.yd || tb->tm_yday > dstend.yd)
            return 0;

        if (tb->tm_yday > dststart.yd && tb->tm_yday < dstend.yd)
            return 1;
    }
    else
    {
        // Southern hemisphere ordering:
        if (tb->tm_yday < dstend.yd || tb->tm_yday > dststart.yd)
            return 1;

        if (tb->tm_yday > dstend.yd && tb->tm_yday < dststart.yd)
            return 0;
    }

    long const ms = 1000 * (tb->tm_sec + 60 * tb->tm_min + 3600 * tb->tm_hour);

    if (tb->tm_yday == dststart.yd)
    {

        return ms >= dststart.ms ? 1 : 0;
    }
    else
    {
        return ms < dstend.ms ? 1 : 0;
    }
}



// Tests if the time represented by the tm structure falls in Daylight Savings
// Time or not.  The Daylight Savings Time rules are obtained from the operating
// system if GetTimeZoneInformation was used by _tzset() to obtain the time zone
// information; otherwise, the USA Daylight Savings Time rules (post-1986) are
// used.
//
// Returns 1 if the time is in Daylight Savings Time; returns 0 otherwise.
extern "C" int __cdecl _isindst(tm* const tb)
{
    int retval = 0;

    __acrt_lock(__acrt_time_lock);
    __try
    {
        retval = _isindst_nolock(tb);
    }
    __finally
    {
        __acrt_unlock(__acrt_time_lock);
    }

    return retval;
}
