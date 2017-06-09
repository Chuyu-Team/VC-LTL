// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// Inlines for AFXDTCTL.H

#pragma once

#pragma warning(push)
#pragma warning(disable: 6001)

/////////////////////////////////////////////////////////////////////////////
// Date/Time common control inlines

#ifdef _AFXDTCTL_INLINE

//CDateTimeCtrl
_AFXDTCTL_INLINE CDateTimeCtrl::CDateTimeCtrl()
	{ }
_AFXDTCTL_INLINE CFont* CDateTimeCtrl::GetMonthCalFont() const
	{ ASSERT(::IsWindow(m_hWnd)); return CFont::FromHandle((HFONT) ::SendMessage(m_hWnd, DTM_GETMCFONT, 0, 0)); }
_AFXDTCTL_INLINE CMonthCalCtrl* CDateTimeCtrl::GetMonthCalCtrl() const
	{ ASSERT(::IsWindow(m_hWnd)); return (CMonthCalCtrl*) CWnd::FromHandle((HWND) ::SendMessage(m_hWnd, DTM_GETMONTHCAL, 0, 0)); }
_AFXDTCTL_INLINE void CDateTimeCtrl::SetMonthCalFont(_In_ HFONT hFont, _In_ BOOL bRedraw /* = TRUE */)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, DTM_SETMCFONT, (WPARAM) hFont, MAKELONG(bRedraw, 0)); }
_AFXDTCTL_INLINE COLORREF CDateTimeCtrl::SetMonthCalColor(_In_ int iColor, _In_ COLORREF ref)
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, DTM_SETMCCOLOR, (WPARAM) iColor, (LPARAM) ref); }
_AFXDTCTL_INLINE DWORD CDateTimeCtrl::GetTime(_Out_ LPSYSTEMTIME pTimeDest) const
	{ ASSERT(::IsWindow(m_hWnd)); ASSERT(pTimeDest != NULL); return (DWORD) ::SendMessage(m_hWnd, DTM_GETSYSTEMTIME, 0, (LPARAM) pTimeDest); }
_AFXDTCTL_INLINE COLORREF CDateTimeCtrl::GetMonthCalColor(_In_ int iColor) const
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, DTM_GETMCCOLOR, (WPARAM) iColor, 0); }
_AFXDTCTL_INLINE BOOL CDateTimeCtrl::SetFormat(_In_z_ LPCTSTR pstrFormat)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, DTM_SETFORMAT, 0, (LPARAM) pstrFormat); }
#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
_AFXDTCTL_INLINE DWORD CDateTimeCtrl::SetMonthCalStyle(_In_ DWORD dwStyle)
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) DateTime_SetMonthCalStyle(m_hWnd, dwStyle); } // DTM_SETMCSTYLE
_AFXDTCTL_INLINE DWORD CDateTimeCtrl::GetMonthCalStyle() const
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) DateTime_GetMonthCalStyle(m_hWnd); } // DTM_GETMCSTYLE
_AFXDTCTL_INLINE BOOL CDateTimeCtrl::GetDateTimePickerInfo(_Out_ LPDATETIMEPICKERINFO pDateTimePickerInfo) const
{ 
	ASSERT(::IsWindow(m_hWnd)); 
	ASSERT(pDateTimePickerInfo != NULL);
	if (pDateTimePickerInfo == NULL)
		return FALSE;
	pDateTimePickerInfo->cbSize = sizeof(DATETIMEPICKERINFO); 
	return (BOOL) DateTime_GetDateTimePickerInfo(m_hWnd, pDateTimePickerInfo); // DTM_GETDATETIMEPICKERINFO
} 
_AFXDTCTL_INLINE BOOL CDateTimeCtrl::GetIdealSize(_Out_ LPSIZE pSize) const
	{ ASSERT(::IsWindow(m_hWnd)); return DateTime_GetIdealSize(m_hWnd, pSize); } // DTM_GETIDEALSIZE
_AFXDTCTL_INLINE void CDateTimeCtrl::CloseMonthCal()
	{ ASSERT(::IsWindow(m_hWnd)); DateTime_CloseMonthCal(m_hWnd); } // DTM_CLOSEMONTHCAL
#endif // (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)

//CMonthCalCtrl
_AFXDTCTL_INLINE CMonthCalCtrl::CMonthCalCtrl()
	{ }
_AFXDTCTL_INLINE DWORD CMonthCalCtrl::HitTest(_In_ PMCHITTESTINFO pMCHitTest)
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, MCM_HITTEST, 0, (LPARAM) pMCHitTest); }
_AFXDTCTL_INLINE BOOL CMonthCalCtrl::GetMinReqRect(_Out_ RECT* pRect) const
	{ ASSERT(m_hWnd != NULL); return (BOOL) ::SendMessage(m_hWnd, MCM_GETMINREQRECT, 0, (LPARAM) pRect); }
_AFXDTCTL_INLINE int CMonthCalCtrl::SetMonthDelta(_In_ int iDelta)
	{ ASSERT(m_hWnd != NULL); return (int) ::SendMessage(m_hWnd, MCM_SETMONTHDELTA, (WPARAM) iDelta, 0); }
_AFXDTCTL_INLINE int CMonthCalCtrl::GetMonthDelta() const
	{ ASSERT(m_hWnd != NULL); return (int) ::SendMessage(m_hWnd, MCM_GETMONTHDELTA, 0, 0); }
_AFXDTCTL_INLINE COLORREF CMonthCalCtrl::GetColor(_In_ int nRegion) const
	{ ASSERT(m_hWnd != NULL); return (COLORREF) ::SendMessage(m_hWnd, MCM_GETCOLOR, (WPARAM) nRegion, 0); }
_AFXDTCTL_INLINE COLORREF CMonthCalCtrl::SetColor(_In_ int nRegion, _In_ COLORREF ref)
	{ ASSERT(m_hWnd != NULL); return (COLORREF) ::SendMessage(m_hWnd, MCM_SETCOLOR, (WPARAM) nRegion, (LPARAM) ref); }
_AFXDTCTL_INLINE BOOL CMonthCalCtrl::SetMaxSelCount(_In_ int nMax)
	{ ASSERT(m_hWnd != NULL); return (BOOL) ::SendMessage(m_hWnd, MCM_SETMAXSELCOUNT, nMax, 0); }
_AFXDTCTL_INLINE int CMonthCalCtrl::GetMaxSelCount() const
	{ ASSERT(m_hWnd != NULL); return (int) ::SendMessage(m_hWnd, MCM_GETMAXSELCOUNT, 0, 0); }
_AFXDTCTL_INLINE void CMonthCalCtrl::SetToday(_In_ const LPSYSTEMTIME pDateTime)
	{ ASSERT(m_hWnd != NULL); ::SendMessage(m_hWnd, MCM_SETTODAY, 0, (LPARAM) pDateTime); }
_AFXDTCTL_INLINE BOOL CMonthCalCtrl::GetToday(_Out_ LPSYSTEMTIME pDateTime) const
	{ ASSERT(m_hWnd != NULL); return (BOOL) ::SendMessage(m_hWnd, MCM_GETTODAY, 0, (LPARAM) pDateTime); }
_AFXDTCTL_INLINE BOOL CMonthCalCtrl::SetCurSel(_In_ const LPSYSTEMTIME pDateTime)
	{ ASSERT(m_hWnd != NULL); return (BOOL) ::SendMessage(m_hWnd, MCM_SETCURSEL, 0, (LPARAM) pDateTime); }
_AFXDTCTL_INLINE BOOL CMonthCalCtrl::GetCurSel(_Out_ LPSYSTEMTIME pDateTime) const
	{ ASSERT(m_hWnd != NULL); BOOL bRetVal = (BOOL)::SendMessage(m_hWnd, MCM_GETCURSEL, 0, (LPARAM) pDateTime);
	pDateTime->wHour = pDateTime->wMinute = pDateTime->wSecond = pDateTime->wMilliseconds = 0; return bRetVal;}
_AFXDTCTL_INLINE DWORD CMonthCalCtrl::GetMaxTodayWidth() const
	{ ASSERT(m_hWnd != NULL); return MonthCal_GetMaxTodayWidth(m_hWnd); } // MCM_GETMAXTODAYWIDTH
#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
_AFXDTCTL_INLINE BOOL CMonthCalCtrl::IsMonthView() const // REVIEW: do we need this method?
	{ ASSERT(m_hWnd != NULL); return (GetCurrentView() == MCMV_MONTH); }
_AFXDTCTL_INLINE BOOL CMonthCalCtrl::IsYearView() const // REVIEW: do we need this method?
	{ ASSERT(m_hWnd != NULL); return (GetCurrentView() == MCMV_YEAR); }
_AFXDTCTL_INLINE BOOL CMonthCalCtrl::IsDecadeView() const // REVIEW: do we need this method?
	{ ASSERT(m_hWnd != NULL); return (GetCurrentView() == MCMV_DECADE); }
_AFXDTCTL_INLINE BOOL CMonthCalCtrl::IsCenturyView() const // REVIEW: do we need this method?
	{ ASSERT(m_hWnd != NULL); return (GetCurrentView() == MCMV_CENTURY); }
_AFXDTCTL_INLINE DWORD CMonthCalCtrl::GetCurrentView() const
	{ ASSERT(m_hWnd != NULL); return MonthCal_GetCurrentView(m_hWnd); } // MCM_GETCURRENTVIEW
_AFXDTCTL_INLINE int CMonthCalCtrl::GetCalendarCount() const
	{ ASSERT(m_hWnd != NULL); return (int) MonthCal_GetCalendarCount(m_hWnd); } // MCM_GETCALENDARCOUNT
_AFXDTCTL_INLINE BOOL CMonthCalCtrl::GetCalendarGridInfo(_Out_ PMCGRIDINFO pmcGridInfo) const
	{ ASSERT(m_hWnd != NULL); return MonthCal_GetCalendarGridInfo(m_hWnd, pmcGridInfo); } // MCM_GETCALENDARGRIDINFO
_AFXDTCTL_INLINE CALID CMonthCalCtrl::GetCalID() const
	{ ASSERT(m_hWnd != NULL); return MonthCal_GetCALID(m_hWnd); } // MCM_GETCALID
_AFXDTCTL_INLINE BOOL CMonthCalCtrl::SetCalID(_In_ CALID calid)
	{ ASSERT(m_hWnd != NULL); return (BOOL) MonthCal_SetCALID(m_hWnd, calid); } // MCM_SETCALID
_AFXDTCTL_INLINE void CMonthCalCtrl::SizeRectToMin(_Inout_ LPRECT lpRect)
	{ ASSERT(m_hWnd != NULL); MonthCal_SizeRectToMin(m_hWnd, lpRect); } // MCM_SIZERECTTOMIN
_AFXDTCTL_INLINE void CMonthCalCtrl::SetCalendarBorder(_In_ int cxyBorder)
	{ ASSERT(m_hWnd != NULL); MonthCal_SetCalendarBorder(m_hWnd, TRUE, cxyBorder); } // MCM_SETCALENDARBORDER
_AFXDTCTL_INLINE void CMonthCalCtrl::SetCalendarBorderDefault()
	{ ASSERT(m_hWnd != NULL); MonthCal_SetCalendarBorder(m_hWnd, FALSE, 0); } // MCM_SETCALENDARBORDER
_AFXDTCTL_INLINE int CMonthCalCtrl::GetCalendarBorder() const
	{ ASSERT(m_hWnd != NULL); return MonthCal_GetCalendarBorder(m_hWnd); } // MCM_GETCALENDARBORDER
_AFXDTCTL_INLINE BOOL CMonthCalCtrl::SetMonthView() // REVIEW: do we need this method?
	{ ASSERT(m_hWnd != NULL); return (SetCurrentView(MCMV_MONTH)); }
_AFXDTCTL_INLINE BOOL CMonthCalCtrl::SetYearView() // REVIEW: do we need this method?
	{ ASSERT(m_hWnd != NULL); return (SetCurrentView(MCMV_YEAR)); }
_AFXDTCTL_INLINE BOOL CMonthCalCtrl::SetDecadeView() // REVIEW: do we need this method?
	{ ASSERT(m_hWnd != NULL); return (SetCurrentView(MCMV_DECADE)); }
_AFXDTCTL_INLINE BOOL CMonthCalCtrl::SetCenturyView() // REVIEW: do we need this method?
	{ ASSERT(m_hWnd != NULL); return (SetCurrentView(MCMV_CENTURY)); }
_AFXDTCTL_INLINE BOOL CMonthCalCtrl::SetCurrentView(_In_ DWORD dwNewView)
	{ ASSERT(m_hWnd != NULL); return MonthCal_SetCurrentView(m_hWnd, dwNewView); } // MCM_SETCURRENTVIEW
#endif // (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)

#pragma warning(pop)

#endif //_AFXDTCTL_INLINE
/////////////////////////////////////////////////////////////////////////////

