// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLCOMTIME_H__
#define __ATLCOMTIME_H__

#pragma once

#pragma warning(push)
#pragma warning( disable : 4159 ) //pragma has popped previously pushed identifier
#pragma warning( disable : 4127 ) //constant expression

#include <atldef.h>
#include <time.h>

#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
#include <atltime.h>
#endif

#include <atlstr.h>
#include <math.h>
#include <OleAuto.h>

#ifndef _ATL_USE_WINAPI_FAMILY_PHONE_APP
#ifndef __oledb_h__
#include <oledb.h>
#endif // __oledb_h__
#endif // _ATL_USE_WINAPI_FAMILY_PHONE_APP

#if defined(_M_IX86)
#pragma pack(push, 4)
#else
#pragma pack(push, ATL_PACKING)
#endif

struct tagVARIANT;
typedef tagVARIANT VARIANT;

typedef double DATE;

namespace ATL
{

class COleDateTimeSpan
{
private:
	static const long maxDaysInSpan  = 3615897L;
// Constructors
public:
	COleDateTimeSpan() throw();

	COleDateTimeSpan(_In_ double dblSpanSrc) throw();
	COleDateTimeSpan(
		_In_ LONG lDays,
		_In_ int nHours,
		_In_ int nMins,
		_In_ int nSecs) throw();

// Attributes
	enum DateTimeSpanStatus
	{
		valid = 0,
		invalid = 1,    // Invalid span (out of range, etc.)
		null = 2,       // Literally has no value
	};

	double m_span;
	DateTimeSpanStatus m_status;

	void SetStatus(_In_ DateTimeSpanStatus status) throw();
	DateTimeSpanStatus GetStatus() const throw();

	double GetTotalDays() const throw();    // span in days (about -3.65e6 to 3.65e6)
	double GetTotalHours() const throw();   // span in hours (about -8.77e7 to 8.77e6)
	double GetTotalMinutes() const throw(); // span in minutes (about -5.26e9 to 5.26e9)
	double GetTotalSeconds() const throw(); // span in seconds (about -3.16e11 to 3.16e11)

	LONG GetDays() const throw();       // component days in span
	LONG GetHours() const throw();      // component hours in span (-23 to 23)
	LONG GetMinutes() const throw();    // component minutes in span (-59 to 59)
	LONG GetSeconds() const throw();    // component seconds in span (-59 to 59)

// Operations
	COleDateTimeSpan& operator=(_In_ double dblSpanSrc) throw();

	bool operator==(_In_ const COleDateTimeSpan& dateSpan) const throw();
	bool operator!=(_In_ const COleDateTimeSpan& dateSpan) const throw();
	bool operator<(_In_ const COleDateTimeSpan& dateSpan) const throw();
	bool operator>(_In_ const COleDateTimeSpan& dateSpan) const throw();
	bool operator<=(_In_ const COleDateTimeSpan& dateSpan) const throw();
	bool operator>=(_In_ const COleDateTimeSpan& dateSpan) const throw();

	// DateTimeSpan math
	COleDateTimeSpan operator+(_In_ const COleDateTimeSpan& dateSpan) const throw();
	COleDateTimeSpan operator-(_In_ const COleDateTimeSpan& dateSpan) const throw();
	COleDateTimeSpan& operator+=(_In_ const COleDateTimeSpan dateSpan) throw();
	COleDateTimeSpan& operator-=(_In_ const COleDateTimeSpan dateSpan) throw();
	COleDateTimeSpan operator-() const throw();

	operator double() const throw();

	void SetDateTimeSpan(
		_In_ LONG lDays,
		_In_ int nHours,
		_In_ int nMins,
		_In_ int nSecs) throw();

	// formatting
	CString Format(_In_z_ LPCTSTR pFormat) const;
	CString Format(_In_ UINT nID) const;

// Implementation
	void CheckRange();

	static const double OLE_DATETIME_HALFSECOND;
};

class COleDateTime
{
// Constructors
public:
	static COleDateTime WINAPI GetCurrentTime() throw();

	COleDateTime() throw();

	COleDateTime(_In_ const VARIANT& varSrc) throw();
	COleDateTime(_In_ DATE dtSrc) throw();

	COleDateTime(_In_ __time32_t timeSrc) throw();
	COleDateTime(_In_ __time64_t timeSrc) throw();

	COleDateTime(_In_ const SYSTEMTIME& systimeSrc) throw();
	COleDateTime(_In_ const FILETIME& filetimeSrc) throw();

	COleDateTime(
		_In_ int nYear,
		_In_ int nMonth,
		_In_ int nDay,
		_In_ int nHour,
		_In_ int nMin,
		_In_ int nSec) throw();
	COleDateTime(_In_ WORD wDosDate, _In_ WORD wDosTime) throw();
#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
	COleDateTime(_In_ const DBTIMESTAMP& dbts) throw();
	_Success_(return != false) bool GetAsDBTIMESTAMP(_Out_ DBTIMESTAMP& dbts) const throw();
#endif // _ATL_USE_WINAPI_FAMILY_DESKTOP_APP

// Attributes
	enum DateTimeStatus
	{
		error = -1,
		valid = 0,
		invalid = 1,    // Invalid date (out of range, etc.)
		null = 2,       // Literally has no value
	};

	DATE m_dt;
	DateTimeStatus m_status;

	void SetStatus(_In_ DateTimeStatus status) throw();
	DateTimeStatus GetStatus() const throw();

	_Success_(return != false) bool GetAsSystemTime(_Out_ SYSTEMTIME& sysTime) const throw();
	_Success_(return != false) bool GetAsUDATE(_Out_ UDATE& udate) const throw();

	int GetYear() const throw();
	// Month of year (1 = January)
	int GetMonth() const throw();
	// Day of month (1-31)
	int GetDay() const throw();
	// Hour in day (0-23)
	int GetHour() const throw();
	// Minute in hour (0-59)
	int GetMinute() const throw();
	// Second in minute (0-59)
	int GetSecond() const throw();
	// Day of week (1 = Sunday, 2 = Monday, ..., 7 = Saturday)
	int GetDayOfWeek() const throw();
	// Days since start of year (1 = January 1)
	int GetDayOfYear() const throw();

// Operations
	COleDateTime& operator=(_In_ const VARIANT& varSrc) throw();
	COleDateTime& operator=(_In_ DATE dtSrc) throw();

	COleDateTime& operator=(_In_ const __time32_t& timeSrc) throw();
	COleDateTime& operator=(_In_ const __time64_t& timeSrc) throw();

	COleDateTime& operator=(_In_ const SYSTEMTIME& systimeSrc) throw();
#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
	COleDateTime& operator=(_In_ const FILETIME& filetimeSrc) throw();
#endif // _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
	COleDateTime& operator=(_In_ const UDATE& udate) throw();

	bool operator==(_In_ const COleDateTime& date) const throw();
	bool operator!=(_In_ const COleDateTime& date) const throw();
	bool operator<(_In_ const COleDateTime& date) const throw();
	bool operator>(_In_ const COleDateTime& date) const throw();
	bool operator<=(_In_ const COleDateTime& date) const throw();
	bool operator>=(_In_ const COleDateTime& date) const throw();

	// DateTime math
	COleDateTime operator+(_In_ COleDateTimeSpan dateSpan) const throw();
	COleDateTime operator-(_In_ COleDateTimeSpan dateSpan) const throw();
	COleDateTime& operator+=(_In_ COleDateTimeSpan dateSpan) throw();
	COleDateTime& operator-=(_In_ COleDateTimeSpan dateSpan) throw();

	// DateTimeSpan math
	COleDateTimeSpan operator-(_In_ const COleDateTime& date) const throw();

	operator DATE() const throw();

	int SetDateTime(
		_In_ int nYear,
		_In_ int nMonth,
		_In_ int nDay,
		_In_ int nHour,
		_In_ int nMin,
		_In_ int nSec) throw();
	int SetDate(_In_ int nYear, _In_ int nMonth, _In_ int nDay) throw();
	int SetTime(_In_ int nHour, _In_ int nMin, _In_ int nSec) throw();
	bool ParseDateTime(
		_In_opt_z_ LPCTSTR lpszDate,
		_In_ DWORD dwFlags = 0,
		_In_ LCID lcid = LANG_USER_DEFAULT) throw();

#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
	// formatting
	CString Format(_In_ DWORD dwFlags = 0, _In_ LCID lcid = LANG_USER_DEFAULT) const;
	CString Format(_In_z_ LPCTSTR lpszFormat) const;
	CString Format(_In_ UINT nFormatID) const;
#endif // _ATL_USE_WINAPI_FAMILY_DESKTOP_APP

protected:
	static double WINAPI DoubleFromDate(_In_ DATE date) throw();
	static DATE WINAPI DateFromDouble(_In_ double f) throw();

	void CheckRange();
	BOOL ConvertSystemTimeToVariantTime(_In_ const SYSTEMTIME& systimeSrc);
};


#ifndef _ATL_STATIC_LIB_IMPL

/////////////////////////////////////////////////////////////////////////////
// COleDateTimeSpan
/////////////////////////////////////////////////////////////////////////////

inline COleDateTimeSpan::COleDateTimeSpan() throw() : m_span(0), m_status(valid)
{
}

inline COleDateTimeSpan::COleDateTimeSpan(_In_ double dblSpanSrc) throw() :
	m_span(dblSpanSrc), m_status(valid)
{
	CheckRange();
}

inline COleDateTimeSpan::COleDateTimeSpan(
	_In_ LONG lDays,
	_In_ int nHours,
	_In_ int nMins,
	_In_ int nSecs) throw()
{
	SetDateTimeSpan(lDays, nHours, nMins, nSecs);
}

inline void COleDateTimeSpan::SetStatus(_In_ DateTimeSpanStatus status) throw()
{
	m_status = status;
}

inline COleDateTimeSpan::DateTimeSpanStatus COleDateTimeSpan::GetStatus() const throw()
{
	return m_status;
}

__declspec(selectany) const double
	COleDateTimeSpan::OLE_DATETIME_HALFSECOND =
	1.0 / (2.0 * (60.0 * 60.0 * 24.0));

inline double COleDateTimeSpan::GetTotalDays() const throw()
{
	ATLASSERT(GetStatus() == valid);
	return (double)LONGLONG(m_span + (m_span < 0 ?
		-OLE_DATETIME_HALFSECOND : OLE_DATETIME_HALFSECOND));
}

inline double COleDateTimeSpan::GetTotalHours() const throw()
{
	ATLASSERT(GetStatus() == valid);
	return (double)LONGLONG((m_span + (m_span < 0 ?
		-OLE_DATETIME_HALFSECOND : OLE_DATETIME_HALFSECOND)) * 24);
}

inline double COleDateTimeSpan::GetTotalMinutes() const throw()
{
	ATLASSERT(GetStatus() == valid);
	return (double)LONGLONG((m_span + (m_span < 0 ?
		-OLE_DATETIME_HALFSECOND : OLE_DATETIME_HALFSECOND)) * (24 * 60));
}

inline double COleDateTimeSpan::GetTotalSeconds() const throw()
{
	ATLASSERT(GetStatus() == valid);
	return (double)LONGLONG((m_span + (m_span < 0 ?
		-OLE_DATETIME_HALFSECOND : OLE_DATETIME_HALFSECOND)) * (24 * 60 * 60));
}

inline LONG COleDateTimeSpan::GetDays() const throw()
{
	ATLASSERT(GetStatus() == valid);
	return LONG(GetTotalDays());
}

inline LONG COleDateTimeSpan::GetHours() const throw()
{
	double dPartialDayHours = GetTotalHours() - (GetTotalDays() * 24);
	return LONG(dPartialDayHours) % 24;
}

inline LONG COleDateTimeSpan::GetMinutes() const throw()
{
	double dPartialHourMinutes = GetTotalMinutes() - (GetTotalHours() * 60);
	return LONG(dPartialHourMinutes) % 60;
}

inline LONG COleDateTimeSpan::GetSeconds() const throw()
{
	double dPartialMinuteSeconds = GetTotalSeconds() - (GetTotalMinutes() * 60);
	return LONG(dPartialMinuteSeconds) % 60;
}

inline COleDateTimeSpan& COleDateTimeSpan::operator=(_In_ double dblSpanSrc) throw()
{
	m_span = dblSpanSrc;
	m_status = valid;
	CheckRange();
	return *this;
}

inline bool COleDateTimeSpan::operator==(
	_In_ const COleDateTimeSpan& dateSpan) const throw()
{
	if(GetStatus() == dateSpan.GetStatus())
	{
		if(GetStatus() == valid)
		{
			// it has to be in precision range to say that it as equal
			if (m_span + OLE_DATETIME_HALFSECOND > dateSpan.m_span &&
					m_span - OLE_DATETIME_HALFSECOND < dateSpan.m_span)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		return (GetStatus() == null);
	}

	return false;
}

inline bool COleDateTimeSpan::operator!=(
	_In_ const COleDateTimeSpan& dateSpan) const throw()
{
	return !operator==(dateSpan);
}

inline bool COleDateTimeSpan::operator<(
	_In_ const COleDateTimeSpan& dateSpan) const throw()
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(dateSpan.GetStatus() == valid);
	if( (GetStatus() == valid) && (GetStatus() == dateSpan.GetStatus()) )
		return m_span < dateSpan.m_span;

	return false;
}

inline bool COleDateTimeSpan::operator>(
	_In_ const COleDateTimeSpan& dateSpan) const throw()
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(dateSpan.GetStatus() == valid);
	if( (GetStatus() == valid) && (GetStatus() == dateSpan.GetStatus()) )
		return m_span > dateSpan.m_span ;

	return false;
}

inline bool COleDateTimeSpan::operator<=(
	_In_ const COleDateTimeSpan& dateSpan) const throw()
{
	return operator<(dateSpan) || operator==(dateSpan);
}

inline bool COleDateTimeSpan::operator>=(
	_In_ const COleDateTimeSpan& dateSpan) const throw()
{
	return operator>(dateSpan) || operator==(dateSpan);
}

inline COleDateTimeSpan COleDateTimeSpan::operator+(
	_In_ const COleDateTimeSpan& dateSpan) const throw()
{
	COleDateTimeSpan dateSpanTemp;

	// If either operand Null, result Null
	if (GetStatus() == null || dateSpan.GetStatus() == null)
	{
		dateSpanTemp.SetStatus(null);
		return dateSpanTemp;
	}

	// If either operand Invalid, result Invalid
	if (GetStatus() == invalid || dateSpan.GetStatus() == invalid)
	{
		dateSpanTemp.SetStatus(invalid);
		return dateSpanTemp;
	}

	// Add spans and validate within legal range
	dateSpanTemp.m_span = m_span + dateSpan.m_span;
	dateSpanTemp.CheckRange();

	return dateSpanTemp;
}

inline COleDateTimeSpan COleDateTimeSpan::operator-(
	_In_ const COleDateTimeSpan& dateSpan) const throw()
{
	COleDateTimeSpan dateSpanTemp;

	// If either operand Null, result Null
	if (GetStatus() == null || dateSpan.GetStatus() == null)
	{
		dateSpanTemp.SetStatus(null);
		return dateSpanTemp;
	}

	// If either operand Invalid, result Invalid
	if (GetStatus() == invalid || dateSpan.GetStatus() == invalid)
	{
		dateSpanTemp.SetStatus(invalid);
		return dateSpanTemp;
	}

	// Subtract spans and validate within legal range
	dateSpanTemp.m_span = m_span - dateSpan.m_span;
	dateSpanTemp.CheckRange();

	return dateSpanTemp;
}

inline COleDateTimeSpan& COleDateTimeSpan::operator+=(
	_In_ const COleDateTimeSpan dateSpan) throw()
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(dateSpan.GetStatus() == valid);
	*this = *this + dateSpan;
	CheckRange();
	return *this;
}

inline COleDateTimeSpan& COleDateTimeSpan::operator-=(
	_In_ const COleDateTimeSpan dateSpan) throw()
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(dateSpan.GetStatus() == valid);
	*this = *this - dateSpan;
	CheckRange();
	return *this;
}

inline COleDateTimeSpan COleDateTimeSpan::operator-() const throw()
{
	return -this->m_span;
}

inline COleDateTimeSpan::operator double() const throw()
{
	return m_span;
}

inline void COleDateTimeSpan::SetDateTimeSpan(
	_In_ LONG lDays,
	_In_ int nHours,
	_In_ int nMins,
	_In_ int nSecs) throw()
{
	// Set date span by breaking into fractional days (all input ranges valid)
	m_span = lDays + ((double)nHours)/24 + ((double)nMins)/(24*60) +
		((double)nSecs)/(24*60*60);
	m_status = valid;
	CheckRange();
}

inline void COleDateTimeSpan::CheckRange()
{
	if(m_span < -maxDaysInSpan || m_span > maxDaysInSpan)
	{
		m_status = invalid;
	}
}

/////////////////////////////////////////////////////////////////////////////
// COleDateTime
/////////////////////////////////////////////////////////////////////////////

inline COleDateTime WINAPI COleDateTime::GetCurrentTime() throw()
{
	return COleDateTime(::_time64(NULL));
}

inline COleDateTime::COleDateTime() throw() :
	m_dt( 0 ), m_status(valid)
{
}

inline COleDateTime::COleDateTime(_In_ const VARIANT& varSrc) throw() :
	m_dt( 0 ), m_status(valid)
{
	*this = varSrc;
}

inline COleDateTime::COleDateTime(_In_ DATE dtSrc) throw() :
	m_dt( dtSrc ), m_status(valid)
{
}

inline COleDateTime::COleDateTime(_In_ __time32_t timeSrc) throw() :
	m_dt( 0 ), m_status(valid)
{
	*this = timeSrc;
}

inline COleDateTime::COleDateTime(_In_ __time64_t timeSrc) throw() :
	m_dt( 0 ), m_status(valid)
{
	*this = timeSrc;
}

inline COleDateTime::COleDateTime(_In_ const SYSTEMTIME& systimeSrc) throw() :
	m_dt( 0 ), m_status(valid)
{
	*this = systimeSrc;
}

inline COleDateTime::COleDateTime(_In_ const FILETIME& filetimeSrc) throw() :
	m_dt( 0 ), m_status(valid)
{
	*this = filetimeSrc;
}

inline COleDateTime::COleDateTime(
	_In_ int nYear,
	_In_ int nMonth,
	_In_ int nDay,
	_In_ int nHour,
	_In_ int nMin,
	_In_ int nSec) throw()
{
	SetDateTime(nYear, nMonth, nDay, nHour, nMin, nSec);
}

#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
inline COleDateTime::COleDateTime(
	_In_ WORD wDosDate,
	_In_ WORD wDosTime) throw()
{
	m_status = ::DosDateTimeToVariantTime(wDosDate, wDosTime, &m_dt) ?
		valid : invalid;
}
#endif // _ATL_USE_WINAPI_FAMILY_DESKTOP_APP

inline void COleDateTime::SetStatus(_In_ DateTimeStatus status) throw()
{
	m_status = status;
}

inline COleDateTime::DateTimeStatus COleDateTime::GetStatus() const throw()
{
	return m_status;
}

inline _Success_(return != false) bool COleDateTime::GetAsSystemTime(_Out_ SYSTEMTIME& sysTime) const throw()
{
	return GetStatus() == valid && ::VariantTimeToSystemTime(m_dt, &sysTime) == TRUE;
}

_Success_(return != false)
inline bool COleDateTime::GetAsUDATE(_Out_ UDATE &udate) const throw()
{
	return SUCCEEDED(::VarUdateFromDate(m_dt, 0, &udate));
}

inline int COleDateTime::GetYear() const throw()
{
	SYSTEMTIME st = {0};
	return GetAsSystemTime(st) ? st.wYear : error;
}

inline int COleDateTime::GetMonth() const throw()
{
	SYSTEMTIME st = {0};
	return GetAsSystemTime(st) ? st.wMonth : error;
}

inline int COleDateTime::GetDay() const throw()
{
	SYSTEMTIME st = {0};
	return GetAsSystemTime(st) ? st.wDay : error;
}

inline int COleDateTime::GetHour() const throw()
{
	SYSTEMTIME st = {0};
	return GetAsSystemTime(st) ? st.wHour : error;
}

inline int COleDateTime::GetMinute() const throw()
{
	SYSTEMTIME st = {0};
	return GetAsSystemTime(st) ? st.wMinute : error;
}

inline int COleDateTime::GetSecond() const throw()
{
	SYSTEMTIME st = {0};
	return GetAsSystemTime(st) ? st.wSecond : error;
}

inline int COleDateTime::GetDayOfWeek() const throw()
{
	SYSTEMTIME st = {0};
	return GetAsSystemTime(st) ? st.wDayOfWeek + 1 : error;
}

inline int COleDateTime::GetDayOfYear() const throw()
{
	UDATE udate = {0};
	return GetAsUDATE(udate) ? udate.wDayOfYear : error;
}

inline COleDateTime& COleDateTime::operator=(_In_ const VARIANT& varSrc) throw()
{
	if (varSrc.vt != VT_DATE)
	{
		VARIANT varDest;
		varDest.vt = VT_EMPTY;
		if(SUCCEEDED(::VariantChangeType(&varDest, const_cast<VARIANT *>(&varSrc), 0, VT_DATE)))
		{
			m_dt = varDest.date;
			m_status = valid;
		}
		else
			m_status = invalid;
	}
	else
	{
		m_dt = varSrc.date;
		m_status = valid;
	}

	return *this;
}

inline COleDateTime& COleDateTime::operator=(_In_ DATE dtSrc) throw()
{
	m_dt = dtSrc;
	m_status = valid;
	return *this;
}

inline COleDateTime& COleDateTime::operator=(_In_ const __time32_t& timeSrc) throw()
{
    return operator=(static_cast<__time64_t>(timeSrc));
}

#ifndef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
inline bool GetAsSystemTimeHelper(_In_ const __time64_t& timeSrc, _Out_ SYSTEMTIME& timeDest)
{
	struct tm ttm;

	if (_localtime64_s(&ttm, &timeSrc) != 0)
	{
		return false;
	}

	timeDest.wYear = (WORD) (1900 + ttm.tm_year);
	timeDest.wMonth = (WORD) (1 + ttm.tm_mon);
	timeDest.wDayOfWeek = (WORD) ttm.tm_wday;
	timeDest.wDay = (WORD) ttm.tm_mday;
	timeDest.wHour = (WORD) ttm.tm_hour;
	timeDest.wMinute = (WORD) ttm.tm_min;
	timeDest.wSecond = (WORD) ttm.tm_sec;
	timeDest.wMilliseconds = 0;

	return true;
}
#endif // _ATL_USE_WINAPI_FAMILY_DESKTOP_APP

inline COleDateTime& COleDateTime::operator=(_In_ const __time64_t& timeSrc) throw()
{
#ifndef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
	SYSTEMTIME st;

	m_status = GetAsSystemTimeHelper(timeSrc, st) &&
			   ConvertSystemTimeToVariantTime(st) ? valid : invalid;

#else
	SYSTEMTIME st;
	CTime tmp(timeSrc);

	m_status = tmp.GetAsSystemTime(st) &&
			   ConvertSystemTimeToVariantTime(st) ? valid : invalid;
#endif // _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
	return *this;
}


inline COleDateTime &COleDateTime::operator=(_In_ const SYSTEMTIME &systimeSrc) throw()
{
	m_status = ConvertSystemTimeToVariantTime(systimeSrc) ?	valid : invalid;
	return *this;
}

#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP
inline COleDateTime &COleDateTime::operator=(_In_ const FILETIME &filetimeSrc) throw()
{
	FILETIME ftl;
	SYSTEMTIME st;

	m_status =  ::FileTimeToLocalFileTime(&filetimeSrc, &ftl) &&
				::FileTimeToSystemTime(&ftl, &st) &&
				ConvertSystemTimeToVariantTime(st) ? valid : invalid;

	return *this;
}
#endif // _ATL_USE_WINAPI_FAMILY_DESKTOP_APP

inline BOOL COleDateTime::ConvertSystemTimeToVariantTime(_In_ const SYSTEMTIME& systimeSrc)
{
	return AtlConvertSystemTimeToVariantTime(systimeSrc,&m_dt);
}
inline COleDateTime &COleDateTime::operator=(_In_ const UDATE &udate) throw()
{
	m_status = (S_OK == VarDateFromUdate((UDATE*)&udate, 0, &m_dt)) ? valid : invalid;

	return *this;
}

inline bool COleDateTime::operator==(_In_ const COleDateTime& date) const throw()
{
	if(GetStatus() == date.GetStatus())
	{
		if(GetStatus() == valid)
		{
			// it has to be in precision range to say that it as equal
			if (m_dt + COleDateTimeSpan::OLE_DATETIME_HALFSECOND > date.m_dt &&
					m_dt - COleDateTimeSpan::OLE_DATETIME_HALFSECOND < date.m_dt)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		return (GetStatus() == null);
	}
	return false;

}

inline bool COleDateTime::operator!=(_In_ const COleDateTime& date) const throw()
{
	return !operator==(date);
}

inline bool COleDateTime::operator<(_In_ const COleDateTime& date) const throw()
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(date.GetStatus() == valid);
	if( (GetStatus() == valid) && (GetStatus() == date.GetStatus()) )
		return( DoubleFromDate( m_dt ) < DoubleFromDate( date.m_dt ) );

	return false;
}

inline bool COleDateTime::operator>(_In_ const COleDateTime& date) const throw()
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(date.GetStatus() == valid);
	if( (GetStatus() == valid) && (GetStatus() == date.GetStatus()) )
		return( DoubleFromDate( m_dt ) > DoubleFromDate( date.m_dt ) );

	return false;
}

inline bool COleDateTime::operator<=(_In_ const COleDateTime& date) const throw()
{
	return operator<(date) || operator==(date);
}

inline bool COleDateTime::operator>=(_In_ const COleDateTime& date) const throw()
{
	return operator>(date) || operator==(date);
}

inline COleDateTime COleDateTime::operator+(_In_ COleDateTimeSpan dateSpan) const throw()
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(dateSpan.GetStatus() == COleDateTimeSpan::DateTimeSpanStatus::valid);
	return( COleDateTime( DateFromDouble( DoubleFromDate( m_dt )+(double)dateSpan ) ) );
}

inline COleDateTime COleDateTime::operator-(_In_ COleDateTimeSpan dateSpan) const throw()
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(dateSpan.GetStatus() == COleDateTimeSpan::DateTimeSpanStatus::valid);
	return( COleDateTime( DateFromDouble( DoubleFromDate( m_dt )-(double)dateSpan ) ) );
}

inline COleDateTime& COleDateTime::operator+=(_In_ COleDateTimeSpan dateSpan) throw()
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(dateSpan.GetStatus() == COleDateTimeSpan::DateTimeSpanStatus::valid);
	m_dt = DateFromDouble( DoubleFromDate( m_dt )+(double)dateSpan );
	return( *this );
}

inline COleDateTime& COleDateTime::operator-=(_In_ COleDateTimeSpan dateSpan) throw()
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(dateSpan.GetStatus() == COleDateTimeSpan::DateTimeSpanStatus::valid);
	m_dt = DateFromDouble( DoubleFromDate( m_dt )-(double)dateSpan );
	return( *this );
}

inline COleDateTimeSpan COleDateTime::operator-(_In_ const COleDateTime& date) const throw()
{
	ATLASSERT(GetStatus() == valid);
	ATLASSERT(date.GetStatus() == valid);
	return DoubleFromDate(m_dt) - DoubleFromDate(date.m_dt);
}

inline COleDateTime::operator DATE() const throw()
{
	ATLASSERT(GetStatus() == valid);
	return( m_dt );
}

inline int COleDateTime::SetDateTime(
	_In_ int nYear,
	_In_ int nMonth,
	_In_ int nDay,
	_In_ int nHour,
	_In_ int nMin,
	_In_ int nSec) throw()
{
	SYSTEMTIME st;
	::ZeroMemory(&st, sizeof(SYSTEMTIME));

	st.wYear = WORD(nYear);
	st.wMonth = WORD(nMonth);
	st.wDay = WORD(nDay);
	st.wHour = WORD(nHour);
	st.wMinute = WORD(nMin);
	st.wSecond = WORD(nSec);

	m_status = ConvertSystemTimeToVariantTime(st) ? valid : invalid;
	return m_status;
}

inline int COleDateTime::SetDate(_In_ int nYear, _In_ int nMonth, _In_ int nDay) throw()
{
	return SetDateTime(nYear, nMonth, nDay, 0, 0, 0);
}

inline int COleDateTime::SetTime(_In_ int nHour, _In_ int nMin, _In_ int nSec) throw()
{
	// Set date to zero date - 12/30/1899
	return SetDateTime(1899, 12, 30, nHour, nMin, nSec);
}

inline double WINAPI COleDateTime::DoubleFromDate(_In_ DATE date) throw()
{
	// We treat it as positive from -OLE_DATETIME_HALFSECOND because of numeric errors
	// If value is positive it doesn't need conversion
	if(date > -COleDateTimeSpan::OLE_DATETIME_HALFSECOND)
	{
		return date;
	}

	// If negative, must convert since negative dates not continuous
	// (examples: -1.25 to -.75, -1.50 to -.50, -1.75 to -.25)
	double fTemp = ceil(date);

	return fTemp - (date - fTemp);
}

inline DATE WINAPI COleDateTime::DateFromDouble(_In_ double f) throw()
{
	// We treat it as positive from -OLE_DATETIME_HALFSECOND because of numeric errors
	// If value is positive it doesn't need conversion
	if(f > -COleDateTimeSpan::OLE_DATETIME_HALFSECOND )
	{
		return f;
	}

	// If negative, must convert since negative dates not continuous
	// (examples: -.75 to -1.25, -.50 to -1.50, -.25 to -1.75)
	double fTemp = floor(f); // fTemp is now whole part

	return fTemp + (fTemp - f);
}

inline void COleDateTime::CheckRange()
{
	// About year 100 to about 9999
	if(m_dt > VTDATEGRE_MAX || m_dt < VTDATEGRE_MIN)
	{
		SetStatus(invalid);
	}
}

#endif // _ATL_STATIC_LIB_IMPL

inline bool COleDateTime::ParseDateTime(
	_In_opt_z_ LPCTSTR lpszDate,
	_In_ DWORD dwFlags,
	_In_ LCID lcid) throw()
{
	USES_CONVERSION_EX;
	LPCTSTR pszDate = ( lpszDate == NULL ) ? _T("") : lpszDate;

	HRESULT hr;
	LPOLESTR p = T2OLE_EX((LPTSTR)pszDate, _ATL_SAFE_ALLOCA_DEF_THRESHOLD);
#ifndef _UNICODE
	if( p == NULL )
	{
		m_dt = 0;
		m_status = invalid;
		return false;
	}
#endif // _UNICODE

	if (FAILED(hr = VarDateFromStr( p, lcid, dwFlags, &m_dt )))
	{
		if (hr == DISP_E_TYPEMISMATCH)
		{
			// Can't convert string to date, set 0 and invalidate
			m_dt = 0;
			m_status = invalid;
			return false;
		}
		else if (hr == DISP_E_OVERFLOW)
		{
			// Can't convert string to date, set -1 and invalidate
			m_dt = -1;
			m_status = invalid;
			return false;
		}
		else
		{
			ATLTRACE(atlTraceTime, 0, _T("\nCOleDateTime VarDateFromStr call failed.\n\t"));
			// Can't convert string to date, set -1 and invalidate
			m_dt = -1;
			m_status = invalid;
			return false;
		}
	}

	m_status = valid;
	return true;
}

#ifdef _ATL_USE_WINAPI_FAMILY_DESKTOP_APP

inline CString COleDateTimeSpan::Format(_In_z_ LPCTSTR pFormat) const
{
	// If null, return empty string
	if (GetStatus() == null)
		return _T("");

	CTimeSpan tmp(GetDays(), GetHours(), GetMinutes(), GetSeconds());
	return tmp.Format(pFormat);
}

#if defined(_UNICODE) || !defined(_CSTRING_DISABLE_NARROW_WIDE_CONVERSION)
inline CString COleDateTime::Format(
	_In_ DWORD dwFlags,
	_In_ LCID lcid) const
{
	// If null, return empty string
	if (GetStatus() == null)
		return _T("");

	// If invalid, return DateTime global string
	if (GetStatus() == invalid)
	{
		CString str;
		if(str.LoadString(ATL_IDS_DATETIME_INVALID))
			return str;
		return szInvalidDateTime;
	}

	CComBSTR bstr;
	if (FAILED(::VarBstrFromDate(m_dt, lcid, dwFlags, &bstr)))
	{
		CString str;
		if(str.LoadString(ATL_IDS_DATETIME_INVALID))
			return str;
		return szInvalidDateTime;
	}

	CString tmp = CString(bstr);
	return tmp;
}
#endif

inline CString COleDateTime::Format(_In_z_ LPCTSTR pFormat) const
{
	ATLENSURE_THROW(pFormat != NULL, E_INVALIDARG);

	// If null, return empty string
	if(GetStatus() == null)
		return _T("");

	// If invalid, return DateTime global string
	if(GetStatus() == invalid)
	{
		CString str;
		if(str.LoadString(ATL_IDS_DATETIME_INVALID))
			return str;
		return szInvalidDateTime;
	}

	UDATE ud;
	if (S_OK != VarUdateFromDate(m_dt, 0, &ud))
	{
		CString str;
		if(str.LoadString(ATL_IDS_DATETIME_INVALID))
			return str;
		return szInvalidDateTime;
	}

	struct tm tmTemp;
	tmTemp.tm_sec	= ud.st.wSecond;
	tmTemp.tm_min	= ud.st.wMinute;
	tmTemp.tm_hour	= ud.st.wHour;
	tmTemp.tm_mday	= ud.st.wDay;
	tmTemp.tm_mon	= ud.st.wMonth - 1;
	tmTemp.tm_year	= ud.st.wYear - 1900;
	tmTemp.tm_wday	= ud.st.wDayOfWeek;
	tmTemp.tm_yday	= ud.wDayOfYear - 1;
	tmTemp.tm_isdst	= 0;

	CString strDate;
	LPTSTR lpszTemp = strDate.GetBufferSetLength(256);
	_tcsftime(lpszTemp, strDate.GetLength(), pFormat, &tmTemp);
	strDate.ReleaseBuffer();

	return strDate;
}

inline CString COleDateTimeSpan::Format(_In_ UINT nFormatID) const
{
	CString strFormat;
	if (!strFormat.LoadString(nFormatID))
		AtlThrow(E_INVALIDARG);
	return Format(strFormat);
}

inline CString COleDateTime::Format(_In_ UINT nFormatID) const
{
	CString strFormat;
	ATLENSURE(strFormat.LoadString(nFormatID));
	return Format(strFormat);
}

inline COleDateTime::COleDateTime(_In_ const DBTIMESTAMP& dbts) throw()
{
	SYSTEMTIME st;
	::ZeroMemory(&st, sizeof(SYSTEMTIME));

	st.wYear = WORD(dbts.year);
	st.wMonth = WORD(dbts.month);
	st.wDay = WORD(dbts.day);
	st.wHour = WORD(dbts.hour);
	st.wMinute = WORD(dbts.minute);
	st.wSecond = WORD(dbts.second);

	m_status = ::SystemTimeToVariantTime(&st, &m_dt) ? valid : invalid;
}

inline _Success_(return != false) bool COleDateTime::GetAsDBTIMESTAMP(_Out_ DBTIMESTAMP& dbts) const throw()
{
	UDATE ud;
	if (S_OK != VarUdateFromDate(m_dt, 0, &ud))
		return false;

	dbts.year = (SHORT) ud.st.wYear;
	dbts.month = (USHORT) ud.st.wMonth;
	dbts.day = (USHORT) ud.st.wDay;
	dbts.hour = (USHORT) ud.st.wHour;
	dbts.minute = (USHORT) ud.st.wMinute;
	dbts.second = (USHORT) ud.st.wSecond;
	dbts.fraction = 0;

	return true;
}

#endif // _ATL_USE_WINAPI_FAMILY_DESKTOP_APP

}	// namespace ATL
#pragma pack(pop)

#pragma warning(pop)

#endif	// __ATLCOMTIME_H__
