// This is a part of the Active Template Library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLTIME_H__
#define __ATLTIME_H__

#pragma once

#pragma warning(push)
#pragma warning(disable : 4159 4127)

#include <atldef.h>

#if !defined(_ATL_USE_WINAPI_FAMILY_DESKTOP_APP)
#error This file is not compatible with the current WINAPI_FAMILY
#endif

#include <time.h>

#ifdef _AFX
#include <afxstr.h>
#else
#include <atlstr.h>
#endif

#if defined(_M_IX86)
#pragma pack(push, 4)
#else
#pragma pack(push, ATL_PACKING)
#endif

#ifndef __oledb_h__
#include <oledb.h>
#endif // __oledb_h__

namespace ATL
{
class CTimeSpan
{
public:
	CTimeSpan() throw();
	CTimeSpan(_In_ __time64_t time) throw();
	CTimeSpan(
		_In_ LONG lDays,
		_In_ int nHours,
		_In_ int nMins,
		_In_ int nSecs) throw();

	LONGLONG GetDays() const throw();
	LONGLONG GetTotalHours() const throw();
	LONG GetHours() const throw();
	LONGLONG GetTotalMinutes() const throw();
	LONG GetMinutes() const throw();
	LONGLONG GetTotalSeconds() const throw();
	LONG GetSeconds() const throw();

	__time64_t GetTimeSpan() const throw();

	CTimeSpan operator+(_In_ CTimeSpan span) const throw();
	CTimeSpan operator-(_In_ CTimeSpan span) const throw();
	CTimeSpan& operator+=(_In_ CTimeSpan span) throw();
	CTimeSpan& operator-=(_In_ CTimeSpan span) throw();
	bool operator==(_In_ CTimeSpan span) const throw();
	bool operator!=(_In_ CTimeSpan span) const throw();
	bool operator<(_In_ CTimeSpan span) const throw();
	bool operator>(_In_ CTimeSpan span) const throw();
	bool operator<=(_In_ CTimeSpan span) const throw();
	bool operator>=(_In_ CTimeSpan span) const throw();

public:
	CString Format(_In_z_ LPCTSTR pszFormat) const;
	CString Format(_In_ UINT nID) const;
#if defined(_AFX) && defined(_UNICODE)
	// for compatibility with MFC 3.x
	CString Format(_In_z_ LPCSTR pFormat) const;
#endif

#ifdef _AFX
	CArchive& Serialize64(_In_ CArchive& ar);
#endif

private:
	__time64_t m_timeSpan;
};

class CTime
{
public:
	static CTime WINAPI GetCurrentTime() throw();
	static BOOL WINAPI IsValidFILETIME(_In_ const FILETIME& ft) throw();

	CTime() throw();
	CTime(_In_ __time64_t time) throw();
	CTime(
		_In_ int nYear,
		_In_ int nMonth,
		_In_ int nDay,
		_In_ int nHour,
		_In_ int nMin,
		_In_ int nSec,
		_In_ int nDST = -1);
	CTime(
		_In_ WORD wDosDate,
		_In_ WORD wDosTime,
		_In_ int nDST = -1);
	CTime(
		_In_ const SYSTEMTIME& st,
		_In_ int nDST = -1);
	CTime(
		_In_ const FILETIME& ft,
		_In_ int nDST = -1);
	CTime(
		_In_ const DBTIMESTAMP& dbts,
		_In_ int nDST = -1) throw();

	CTime& operator=(_In_ __time64_t time) throw();

	CTime& operator+=(_In_ CTimeSpan span) throw();
	CTime& operator-=(_In_ CTimeSpan span) throw();

	CTimeSpan operator-(_In_ CTime time) const throw();
	CTime operator-(_In_ CTimeSpan span) const throw();
	CTime operator+(_In_ CTimeSpan span) const throw();

	bool operator==(_In_ CTime time) const throw();
	bool operator!=(_In_ CTime time) const throw();
	bool operator<(_In_ CTime time) const throw();
	bool operator>(_In_ CTime time) const throw();
	bool operator<=(_In_ CTime time) const throw();
	bool operator>=(_In_ CTime time) const throw();

	_Success_(return != NULL) struct tm* GetGmtTm(_Out_ struct tm* ptm) const;
	_Success_(return != NULL) struct tm* GetLocalTm(_Out_ struct tm* ptm) const;

	_Success_(return != false) bool GetAsSystemTime(_Out_ SYSTEMTIME& st) const throw();
	_Success_(return != false) bool GetAsDBTIMESTAMP(_Out_ DBTIMESTAMP& dbts) const throw();

	__time64_t GetTime() const throw();

	int GetYear() const throw();
	int GetMonth() const throw();
	int GetDay() const throw();
	int GetHour() const throw();
	int GetMinute() const throw();
	int GetSecond() const throw();
	int GetDayOfWeek() const throw();

	// formatting using "C" strftime
	CString Format(_In_z_ LPCTSTR pszFormat) const;
	CString FormatGmt(_In_z_ LPCTSTR pszFormat) const;
	CString Format(_In_ UINT nFormatID) const;
	CString FormatGmt(_In_ UINT nFormatID) const;

#if defined(_AFX) && defined(_UNICODE)
	// for compatibility with MFC 3.x
	CString Format(_In_z_ LPCSTR pFormat) const;
	CString FormatGmt(_In_z_ LPCSTR pFormat) const;
#endif

#ifdef _AFX
	CArchive& Serialize64(_In_ CArchive& ar);
#endif

private:
	__time64_t m_time;
};

class CFileTimeSpan
{
public:
	CFileTimeSpan() throw();
	CFileTimeSpan(_In_ const CFileTimeSpan& span) throw();
	CFileTimeSpan(_In_ LONGLONG nSpan) throw();

	CFileTimeSpan& operator=(_In_ const CFileTimeSpan& span) throw();

	CFileTimeSpan& operator+=(_In_ CFileTimeSpan span) throw();
	CFileTimeSpan& operator-=(_In_ CFileTimeSpan span) throw();

	CFileTimeSpan operator+(_In_ CFileTimeSpan span) const throw();
	CFileTimeSpan operator-(_In_ CFileTimeSpan span) const throw();

	bool operator==(_In_ CFileTimeSpan span) const throw();
	bool operator!=(_In_ CFileTimeSpan span) const throw();
	bool operator<(_In_ CFileTimeSpan span) const throw();
	bool operator>(_In_ CFileTimeSpan span) const throw();
	bool operator<=(_In_ CFileTimeSpan span) const throw();
	bool operator>=(_In_ CFileTimeSpan span) const throw();

	LONGLONG GetTimeSpan() const throw();
	void SetTimeSpan(_In_ LONGLONG nSpan) throw();

protected:
	LONGLONG m_nSpan;
};

class CFileTime :
	public FILETIME
{
public:
	CFileTime() throw();
	CFileTime(_In_ const FILETIME& ft) throw();
	CFileTime(_In_ ULONGLONG nTime) throw();

	static CFileTime WINAPI GetCurrentTime() throw();

	CFileTime& operator=(_In_ const FILETIME& ft) throw();

	CFileTime& operator+=(_In_ CFileTimeSpan span) throw();
	CFileTime& operator-=(_In_ CFileTimeSpan span) throw();

	CFileTime operator+(_In_ CFileTimeSpan span) const throw();
	CFileTime operator-(_In_ CFileTimeSpan span) const throw();
	CFileTimeSpan operator-(_In_ CFileTime ft) const throw();

	bool operator==(_In_ CFileTime ft) const throw();
	bool operator!=(_In_ CFileTime ft) const throw();
	bool operator<(_In_ CFileTime ft) const throw();
	bool operator>(_In_ CFileTime ft) const throw();
	bool operator<=(_In_ CFileTime ft) const throw();
	bool operator>=(_In_ CFileTime ft) const throw();

	ULONGLONG GetTime() const throw();
	void SetTime(_In_ ULONGLONG nTime) throw();

	CFileTime UTCToLocal() const throw();
	CFileTime LocalToUTC() const throw();

	static const ULONGLONG Millisecond = 10000;
	static const ULONGLONG Second = Millisecond * static_cast<ULONGLONG>(1000);
	static const ULONGLONG Minute = Second * static_cast<ULONGLONG>(60);
	static const ULONGLONG Hour = Minute * static_cast<ULONGLONG>(60);
	static const ULONGLONG Day = Hour * static_cast<ULONGLONG>(24);
	static const ULONGLONG Week = Day * static_cast<ULONGLONG>(7);
};

// Used only if these strings could not be found in resources.
extern __declspec(selectany) const TCHAR * const szInvalidDateTime = _T("Invalid DateTime");
extern __declspec(selectany) const TCHAR * const szInvalidDateTimeSpan = _T("Invalid DateTimeSpan");

const int maxTimeBufferSize = 128;
const long maxDaysInSpan  =	3615897L;

#ifndef _ATL_STATIC_LIB_IMPL

/////////////////////////////////////////////////////////////////////////////
// CTimeSpan
/////////////////////////////////////////////////////////////////////////////

inline CTimeSpan::CTimeSpan() throw() :
	m_timeSpan(0)
{
}

inline CTimeSpan::CTimeSpan(_In_ __time64_t time) throw() :
	m_timeSpan( time )
{
}

inline CTimeSpan::CTimeSpan(
	_In_ LONG lDays,
	_In_ int nHours,
	_In_ int nMins,
	_In_ int nSecs) throw()
{
 	m_timeSpan = nSecs + 60* (nMins + 60* (nHours + __int64(24) * lDays));
}

inline LONGLONG CTimeSpan::GetDays() const throw()
{
	return( m_timeSpan/(24*3600) );
}

inline LONGLONG CTimeSpan::GetTotalHours() const throw()
{
	return( m_timeSpan/3600 );
}

inline LONG CTimeSpan::GetHours() const throw()
{
	return( LONG( GetTotalHours()-(GetDays()*24) ) );
}

inline LONGLONG CTimeSpan::GetTotalMinutes() const throw()
{
	return( m_timeSpan/60 );
}

inline LONG CTimeSpan::GetMinutes() const throw()
{
	return( LONG( GetTotalMinutes()-(GetTotalHours()*60) ) );
}

inline LONGLONG CTimeSpan::GetTotalSeconds() const throw()
{
	return( m_timeSpan );
}

inline LONG CTimeSpan::GetSeconds() const throw()
{
	return( LONG( GetTotalSeconds()-(GetTotalMinutes()*60) ) );
}

inline __time64_t CTimeSpan::GetTimeSpan() const throw()
{
	return( m_timeSpan );
}

inline CTimeSpan CTimeSpan::operator+(_In_ CTimeSpan span) const throw()
{
	return( CTimeSpan( m_timeSpan+span.m_timeSpan ) );
}

inline CTimeSpan CTimeSpan::operator-(_In_ CTimeSpan span) const throw()
{
	return( CTimeSpan( m_timeSpan-span.m_timeSpan ) );
}

inline CTimeSpan& CTimeSpan::operator+=(_In_ CTimeSpan span) throw()
{
	m_timeSpan += span.m_timeSpan;
	return( *this );
}

inline CTimeSpan& CTimeSpan::operator-=(_In_ CTimeSpan span) throw()
{
	m_timeSpan -= span.m_timeSpan;
	return( *this );
}

inline bool CTimeSpan::operator==(_In_ CTimeSpan span) const throw()
{
	return( m_timeSpan == span.m_timeSpan );
}

inline bool CTimeSpan::operator!=(_In_ CTimeSpan span) const throw()
{
	return( m_timeSpan != span.m_timeSpan );
}

inline bool CTimeSpan::operator<(_In_ CTimeSpan span) const throw()
{
	return( m_timeSpan < span.m_timeSpan );
}

inline bool CTimeSpan::operator>(_In_ CTimeSpan span) const throw()
{
	return( m_timeSpan > span.m_timeSpan );
}

inline bool CTimeSpan::operator<=(_In_ CTimeSpan span) const throw()
{
	return( m_timeSpan <= span.m_timeSpan );
}

inline bool CTimeSpan::operator>=(_In_ CTimeSpan span) const throw()
{
	return( m_timeSpan >= span.m_timeSpan );
}

/////////////////////////////////////////////////////////////////////////////
// CTime
/////////////////////////////////////////////////////////////////////////////

inline CTime WINAPI CTime::GetCurrentTime() throw()
{
	return( CTime( ::_time64( NULL ) ) );
}

_Success_(return != false) inline bool CTime::GetAsDBTIMESTAMP(_Out_ DBTIMESTAMP& dbts) const throw()
{
	struct tm tmLocal;
	if (GetLocalTm(&tmLocal) == nullptr) return false;

	dbts.year = (SHORT) (1900 + tmLocal.tm_year);
	dbts.month = (USHORT) (1 + tmLocal.tm_mon);
	dbts.day = (USHORT) tmLocal.tm_mday;
	dbts.hour = (USHORT) tmLocal.tm_hour;
	dbts.minute = (USHORT) tmLocal.tm_min;
	dbts.second = (USHORT) tmLocal.tm_sec;
	dbts.fraction = 0;

	return true;
}

inline BOOL WINAPI CTime::IsValidFILETIME(_In_ const FILETIME& fileTime) throw()
{
	FILETIME localTime;
	if (!FileTimeToLocalFileTime(&fileTime, &localTime))
	{
		return FALSE;
	}

	// then convert that time to system time
	SYSTEMTIME sysTime;
	if (!FileTimeToSystemTime(&localTime, &sysTime))
	{
		return FALSE;
	}

	return TRUE;
}

inline CTime::CTime() throw() :
	m_time(0)
{
}

inline CTime::CTime(_In_ __time64_t time)  throw():
	m_time( time )
{
}

inline CTime::CTime(
	_In_ int nYear,
	_In_ int nMonth,
	_In_ int nDay,
	_In_ int nHour,
	_In_ int nMin,
	_In_ int nSec,
	_In_ int nDST)
{
#pragma warning (push)
#pragma warning (disable: 4127)  // conditional expression constant

	ATLENSURE( nYear >= 1900 );
	ATLENSURE( nMonth >= 1 && nMonth <= 12 );
	ATLENSURE( nDay >= 1 && nDay <= 31 );
	ATLENSURE( nHour >= 0 && nHour <= 23 );
	ATLENSURE( nMin >= 0 && nMin <= 59 );
	ATLENSURE( nSec >= 0 && nSec <= 59 );

#pragma warning (pop)

	struct tm atm;

	atm.tm_sec = nSec;
	atm.tm_min = nMin;
	atm.tm_hour = nHour;
	atm.tm_mday = nDay;
	atm.tm_mon = nMonth - 1;        // tm_mon is 0 based
	atm.tm_year = nYear - 1900;     // tm_year is 1900 based
	atm.tm_isdst = nDST;

	m_time = _mktime64(&atm);
	ATLASSUME(m_time != -1);       // indicates an illegal input time
	if(m_time == -1)
	{
		AtlThrow(E_INVALIDARG);
	}
}

inline CTime::CTime(
	_In_ WORD wDosDate,
	_In_ WORD wDosTime,
	_In_ int nDST)
{
	struct tm atm;
	atm.tm_sec = (wDosTime & ~0xFFE0) << 1;
	atm.tm_min = (wDosTime & ~0xF800) >> 5;
	atm.tm_hour = wDosTime >> 11;

	atm.tm_mday = wDosDate & ~0xFFE0;
	atm.tm_mon = ((wDosDate & ~0xFE00) >> 5) - 1;
	atm.tm_year = (wDosDate >> 9) + 80;
	atm.tm_isdst = nDST;
	m_time = _mktime64(&atm);
	ATLASSUME(m_time != -1);       // indicates an illegal input time

	if(m_time == -1)
		AtlThrow(E_INVALIDARG);

}

inline CTime::CTime(
	_In_ const SYSTEMTIME& sysTime,
	_In_ int nDST)
{
	if (sysTime.wYear < 1900)
	{
		__time64_t time0 = 0L;
		CTime timeT(time0);
		*this = timeT;
	}
	else
	{
		CTime timeT(
			(int)sysTime.wYear, (int)sysTime.wMonth, (int)sysTime.wDay,
			(int)sysTime.wHour, (int)sysTime.wMinute, (int)sysTime.wSecond,
			nDST);
		*this = timeT;
	}
}

inline CTime::CTime(
	_In_ const FILETIME& fileTime,
	_In_ int nDST)
{
	SYSTEMTIME sysTime;
	SYSTEMTIME localTime;
	if (!FileTimeToSystemTime(&fileTime, &sysTime) || !SystemTimeToTzSpecificLocalTime(nullptr, &sysTime, &localTime))
	{
		m_time = 0;
		AtlThrow(E_INVALIDARG);
		return;
	}

	// Convert the system time to a time_t (C-runtime local time)
	CTime timeT(localTime, nDST);
	*this = timeT;
}

inline CTime& CTime::operator=(_In_ __time64_t time) throw()
{
	m_time = time;

	return( *this );
}

inline CTime& CTime::operator+=(_In_ CTimeSpan span) throw()
{
	m_time += span.GetTimeSpan();

	return( *this );
}

inline CTime& CTime::operator-=(_In_ CTimeSpan span) throw()
{
	m_time -= span.GetTimeSpan();

	return( *this );
}

inline CTimeSpan CTime::operator-(_In_ CTime time) const throw()
{
	return( CTimeSpan( m_time-time.m_time ) );
}

inline CTime CTime::operator-(_In_ CTimeSpan span) const throw()
{
	return( CTime( m_time-span.GetTimeSpan() ) );
}

inline CTime CTime::operator+(_In_ CTimeSpan span) const throw()
{
	return( CTime( m_time+span.GetTimeSpan() ) );
}

inline bool CTime::operator==(_In_ CTime time) const throw()
{
	return( m_time == time.m_time );
}

inline bool CTime::operator!=(_In_ CTime time) const throw()
{
	return( m_time != time.m_time );
}

inline bool CTime::operator<(_In_ CTime time) const throw()
{
	return( m_time < time.m_time );
}

inline bool CTime::operator>(_In_ CTime time) const throw()
{
	return( m_time > time.m_time );
}

inline bool CTime::operator<=(_In_ CTime time) const throw()
{
	return( m_time <= time.m_time );
}

inline bool CTime::operator>=(_In_ CTime time) const throw()
{
	return( m_time >= time.m_time );
}

inline _Success_(return != NULL) struct tm* CTime::GetGmtTm(_Out_ struct tm* ptm) const
{
	// Ensure ptm is valid
	ATLENSURE( ptm != NULL );

	if (ptm != NULL)
	{
		struct tm ptmTemp;
		errno_t err = _gmtime64_s(&ptmTemp, &m_time);

		// Be sure the call succeeded
		if(err != 0) { return NULL; }

		*ptm = ptmTemp;
		return ptm;
	}

	return NULL;
}

inline _Success_(return != NULL) struct tm* CTime::GetLocalTm(_Out_ struct tm* ptm) const
{
	// Ensure ptm is valid
	ATLENSURE( ptm != NULL );

	if (ptm != NULL)
	{
		struct tm ptmTemp;
		errno_t err = _localtime64_s(&ptmTemp, &m_time);

		if (err != 0)
		{
			return NULL;    // indicates that m_time was not initialized!
		}

		*ptm = ptmTemp;
		return ptm;
	}

	return NULL;
}

inline _Success_(return != false) bool CTime::GetAsSystemTime(_Out_ SYSTEMTIME& timeDest) const throw()
{
	struct tm ttm;
	struct tm* ptm;

	ptm = GetLocalTm(&ttm);
	if(!ptm) 
	{ 
		return false; 
	}

	timeDest.wYear = (WORD) (1900 + ptm->tm_year);
	timeDest.wMonth = (WORD) (1 + ptm->tm_mon);
	timeDest.wDayOfWeek = (WORD) ptm->tm_wday;
	timeDest.wDay = (WORD) ptm->tm_mday;
	timeDest.wHour = (WORD) ptm->tm_hour;
	timeDest.wMinute = (WORD) ptm->tm_min;
	timeDest.wSecond = (WORD) ptm->tm_sec;
	timeDest.wMilliseconds = 0;

	return true;
}

inline __time64_t CTime::GetTime() const throw()
{
	return( m_time );
}

inline int CTime::GetYear() const
{
	struct tm ttm;
	struct tm * ptm;

	ptm = GetLocalTm(&ttm);
	return ptm ? (ptm->tm_year) + 1900 : 0 ;
}

inline int CTime::GetMonth() const
{
	struct tm ttm;
	struct tm * ptm;

	ptm = GetLocalTm(&ttm);
	return ptm ? ptm->tm_mon + 1 : 0;
}

inline int CTime::GetDay() const
{
	struct tm ttm;
	struct tm * ptm;

	ptm = GetLocalTm(&ttm);
	return ptm ? ptm->tm_mday : 0 ;
}

inline int CTime::GetHour() const
{
	struct tm ttm;
	struct tm * ptm;

	ptm = GetLocalTm(&ttm);
	return ptm ? ptm->tm_hour : -1 ;
}

inline int CTime::GetMinute() const
{
	struct tm ttm;
	struct tm * ptm;

	ptm = GetLocalTm(&ttm);
	return ptm ? ptm->tm_min : -1 ;
}

inline int CTime::GetSecond() const
{
	struct tm ttm;
	struct tm * ptm;

	ptm = GetLocalTm(&ttm);
	return ptm ? ptm->tm_sec : -1 ;
}

inline int CTime::GetDayOfWeek() const
{
	struct tm ttm;
	struct tm * ptm;

	ptm = GetLocalTm(&ttm);
	return ptm ? ptm->tm_wday + 1 : 0 ;
}

/////////////////////////////////////////////////////////////////////////////
// CFileTimeSpan
/////////////////////////////////////////////////////////////////////////////

inline CFileTimeSpan::CFileTimeSpan() throw() :
	m_nSpan( 0 )
{
}

inline CFileTimeSpan::CFileTimeSpan(_In_ const CFileTimeSpan& span) throw() :
	m_nSpan( span.m_nSpan )
{
}

inline CFileTimeSpan::CFileTimeSpan(_In_ LONGLONG nSpan) throw() :
	m_nSpan( nSpan )
{
}

inline CFileTimeSpan& CFileTimeSpan::operator=(_In_ const CFileTimeSpan& span) throw()
{
	m_nSpan = span.m_nSpan;

	return( *this );
}

inline CFileTimeSpan& CFileTimeSpan::operator+=(_In_ CFileTimeSpan span) throw()
{
	m_nSpan += span.m_nSpan;

	return( *this );
}

inline CFileTimeSpan& CFileTimeSpan::operator-=(_In_ CFileTimeSpan span) throw()
{
	m_nSpan -= span.m_nSpan;

	return( *this );
}

inline CFileTimeSpan CFileTimeSpan::operator+(_In_ CFileTimeSpan span) const throw()
{
	return( CFileTimeSpan( m_nSpan+span.m_nSpan ) );
}

inline CFileTimeSpan CFileTimeSpan::operator-(_In_ CFileTimeSpan span) const throw()
{
	return( CFileTimeSpan( m_nSpan-span.m_nSpan ) );
}

inline bool CFileTimeSpan::operator==(_In_ CFileTimeSpan span) const throw()
{
	return( m_nSpan == span.m_nSpan );
}

inline bool CFileTimeSpan::operator!=(_In_ CFileTimeSpan span) const throw()
{
	return( m_nSpan != span.m_nSpan );
}

inline bool CFileTimeSpan::operator<(_In_ CFileTimeSpan span) const throw()
{
	return( m_nSpan < span.m_nSpan );
}

inline bool CFileTimeSpan::operator>(_In_ CFileTimeSpan span) const throw()
{
	return( m_nSpan > span.m_nSpan );
}

inline bool CFileTimeSpan::operator<=(_In_ CFileTimeSpan span) const throw()
{
	return( m_nSpan <= span.m_nSpan );
}

inline bool CFileTimeSpan::operator>=(_In_ CFileTimeSpan span) const throw()
{
	return( m_nSpan >= span.m_nSpan );
}

inline LONGLONG CFileTimeSpan::GetTimeSpan() const throw()
{
	return( m_nSpan );
}

inline void CFileTimeSpan::SetTimeSpan(_In_ LONGLONG nSpan) throw()
{
	m_nSpan = nSpan;
}


/////////////////////////////////////////////////////////////////////////////
// CFileTime
/////////////////////////////////////////////////////////////////////////////

inline CFileTime::CFileTime() throw()
{
	dwLowDateTime = 0;
	dwHighDateTime = 0;
}

inline CFileTime::CFileTime(_In_ const FILETIME& ft) throw()
{
	dwLowDateTime = ft.dwLowDateTime;
	dwHighDateTime = ft.dwHighDateTime;
}

inline CFileTime::CFileTime(_In_ ULONGLONG nTime) throw()
{
	dwLowDateTime = DWORD( nTime );
	dwHighDateTime = DWORD( nTime>>32 );
}

inline CFileTime& CFileTime::operator=(_In_ const FILETIME& ft) throw()
{
	dwLowDateTime = ft.dwLowDateTime;
	dwHighDateTime = ft.dwHighDateTime;

	return( *this );
}

inline CFileTime WINAPI CFileTime::GetCurrentTime() throw()
{
	CFileTime ft;
	GetSystemTimeAsFileTime(&ft);
	return ft;
}

inline CFileTime& CFileTime::operator+=(_In_ CFileTimeSpan span) throw()
{
	SetTime( GetTime()+span.GetTimeSpan() );

	return( *this );
}

inline CFileTime& CFileTime::operator-=(_In_ CFileTimeSpan span) throw()
{
	SetTime( GetTime()-span.GetTimeSpan() );

	return( *this );
}

inline CFileTime CFileTime::operator+(_In_ CFileTimeSpan span) const throw()
{
	return( CFileTime( GetTime()+span.GetTimeSpan() ) );
}

inline CFileTime CFileTime::operator-(_In_ CFileTimeSpan span) const throw()
{
	return( CFileTime( GetTime()-span.GetTimeSpan() ) );
}

inline CFileTimeSpan CFileTime::operator-(_In_ CFileTime ft) const throw()
{
	return( CFileTimeSpan( GetTime()-ft.GetTime() ) );
}

inline bool CFileTime::operator==(_In_ CFileTime ft) const throw()
{
	return( GetTime() == ft.GetTime() );
}

inline bool CFileTime::operator!=(_In_ CFileTime ft) const throw()
{
	return( GetTime() != ft.GetTime() );
}

inline bool CFileTime::operator<(_In_ CFileTime ft) const throw()
{
	return( GetTime() < ft.GetTime() );
}

inline bool CFileTime::operator>(_In_ CFileTime ft) const throw()
{
	return( GetTime() > ft.GetTime() );
}

inline bool CFileTime::operator<=(_In_ CFileTime ft) const throw()
{
	return( GetTime() <= ft.GetTime() );
}

inline bool CFileTime::operator>=(_In_ CFileTime ft) const throw()
{
	return( GetTime() >= ft.GetTime() );
}

inline ULONGLONG CFileTime::GetTime() const throw()
{
	return( (ULONGLONG( dwHighDateTime )<<32)|dwLowDateTime );
}

inline void CFileTime::SetTime(_In_ ULONGLONG nTime) throw()
{
	dwLowDateTime = DWORD( nTime );
	dwHighDateTime = DWORD( nTime>>32 );
}

inline CFileTime CFileTime::UTCToLocal() const throw()
{
	CFileTime ftLocal;

	::FileTimeToLocalFileTime( this, &ftLocal );

	return( ftLocal );
}

inline CFileTime CFileTime::LocalToUTC() const throw()
{
	CFileTime ftUTC;

	::LocalFileTimeToFileTime( this, &ftUTC );

	return( ftUTC );
}

#endif // _ATL_STATIC_LIB_IMPL

enum _CTIMESPANFORMATSTEP
{
	_CTFS_NONE   = 0,
	_CTFS_FORMAT = 1,
	_CTFS_NZ     = 2
};
#define _CTIMESPANFORMATS 3

inline CString CTimeSpan::Format(_In_z_ LPCTSTR pFormat) const
// formatting timespans is a little trickier than formatting CTimes
//  * we are only interested in relative time formats, ie. it is illegal
//      to format anything dealing with absolute time (i.e. years, months,
//         day of week, day of year, timezones, ...)
//  * the only valid formats:
//      %D - # of days
//      %H - hour in 24 hour format
//      %M - minute (0-59)
//      %S - seconds (0-59)
//      %% - percent sign
//	%#<any_of_mods> - skip leading zeros
{
	ATLASSERT( pFormat != NULL );
	if( pFormat == NULL )
		AtlThrow( E_INVALIDARG );

	CString strBuffer;
	CString hmsFormats [_CTIMESPANFORMATS] = {_T("%Tc"),_T("%02ld"),_T("%d")};
	CString dayFormats [_CTIMESPANFORMATS] = {_T("%Tc"),_T("%I64d"),_T("%I64d")};
	strBuffer.Preallocate(maxTimeBufferSize);
	TCHAR ch;

	while ((ch = *pFormat++) != _T('\0'))
	{
		enum _CTIMESPANFORMATSTEP formatstep = _CTFS_NONE;
		if(ch == _T('%'))
		{
			formatstep = _CTFS_FORMAT;
			ch = *pFormat++;
			if(ch == _T('#'))
			{
					formatstep = _CTFS_NZ;
					ch = *pFormat++;
			}
		}
		switch (ch)
		{
			case '%':
				strBuffer += ch;
				break;
			case 'D':
				strBuffer.AppendFormat(dayFormats[formatstep], formatstep ? GetDays()    : ch);
				break;
			case 'H':
				strBuffer.AppendFormat(hmsFormats[formatstep], formatstep ? GetHours()   : ch);
				break;
			case 'M':
				strBuffer.AppendFormat(hmsFormats[formatstep], formatstep ? GetMinutes() : ch);
				break;
			case 'S':
				strBuffer.AppendFormat(hmsFormats[formatstep], formatstep ? GetSeconds() : ch);
				break;
			default:
				if(formatstep)
				{
#pragma warning (push)
#pragma warning (disable: 4127)  // conditional expression constant
					ATLENSURE(FALSE);      // probably a bad format character
#pragma warning (pop)
				}
				else
				{
					strBuffer += ch;
#ifdef _MBCS
					if (_istlead(ch))
					{
						strBuffer += *pFormat++;
					}
#endif
				}
				break;
			}
	}

	return strBuffer;
}

inline CString CTimeSpan::Format(_In_ UINT nFormatID) const
{
	CString strFormat;
	ATLENSURE(strFormat.LoadString(nFormatID));
	return Format(strFormat);
}

#if defined(_AFX) && defined(_UNICODE) && !defined(_CSTRING_DISABLE_NARROW_WIDE_CONVERSION)
inline CString CTimeSpan::Format(_In_z_ LPCSTR pFormat) const
{
	return Format(CString(pFormat));
}
#endif

#ifdef __oledb_h__
inline CTime::CTime(
	_In_ const DBTIMESTAMP& dbts,
	_In_ int nDST) throw()
{
	struct tm atm;
	atm.tm_sec = dbts.second;
	atm.tm_min = dbts.minute;
	atm.tm_hour = dbts.hour;
	atm.tm_mday = dbts.day;
	atm.tm_mon = dbts.month - 1;        // tm_mon is 0 based
	ATLASSERT(dbts.year >= 1900);
	atm.tm_year = dbts.year - 1900;     // tm_year is 1900 based
	atm.tm_isdst = nDST;
	m_time = _mktime64(&atm);
	ATLASSUME(m_time != -1);       // indicates an illegal input time
}
#endif

inline CString CTime::Format(_In_z_ LPCTSTR pFormat) const
{
	if(pFormat == NULL)
	{
		return pFormat;
	}

	TCHAR szBuffer[maxTimeBufferSize];
	struct tm ptmTemp;

	if (_localtime64_s(&ptmTemp, &m_time) != 0)
	{
		AtlThrow(E_INVALIDARG);
	}

	if (!_tcsftime(szBuffer, maxTimeBufferSize, pFormat, &ptmTemp))
	{
		szBuffer[0] = '\0';
	}

	return szBuffer;
}

inline CString CTime::FormatGmt(_In_z_ LPCTSTR pFormat) const
{
	if(pFormat == NULL)
	{
		return pFormat;
	}

	TCHAR szBuffer[maxTimeBufferSize];
	struct tm ptmTemp;

	if (_gmtime64_s(&ptmTemp, &m_time) != 0)
	{
		AtlThrow(E_INVALIDARG);
	}

	if (!_tcsftime(szBuffer, maxTimeBufferSize, pFormat, &ptmTemp))
	{
		szBuffer[0] = '\0';
	}

	return szBuffer;
}

inline CString CTime::Format(_In_ UINT nFormatID) const
{
	CString strFormat;
	ATLENSURE(strFormat.LoadString(nFormatID));
	return Format(strFormat);
}

inline CString CTime::FormatGmt(_In_ UINT nFormatID) const
{
	CString strFormat;
	ATLENSURE(strFormat.LoadString(nFormatID));
	return FormatGmt(strFormat);
}

#if defined (_AFX) && defined(_UNICODE) && !defined(_CSTRING_DISABLE_NARROW_WIDE_CONVERSION)
inline CString CTime::Format(_In_z_ LPCSTR pFormat) const
{
	return Format(CString(pFormat));
}

inline CString CTime::FormatGmt(_In_z_ LPCSTR pFormat) const
{
	return FormatGmt(CString(pFormat));
}
#endif // _AFX && _UNICODE

}	// namespace ATL
#pragma pack(pop)

#pragma warning(pop)

#endif  // __ATLTIME_H__
