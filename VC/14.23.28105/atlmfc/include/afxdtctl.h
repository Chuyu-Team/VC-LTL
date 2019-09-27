// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#ifndef __AFXDTCTL_H__
#define __AFXDTCTL_H__

#pragma once

#ifndef __AFXWIN_H__
	#include <afxwin.h>
#endif

#ifndef __AFXDISP_H__
	#include <afxdisp.h>
#endif

/////////////////////////////////////////////////////////////////////////////

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

//CObject
	//CCmdTarget;
		//CWnd
			class CMonthCalCtrl;
			class CDateTimeCtrl;

#undef AFX_DATA
#define AFX_DATA AFX_CORE_DATA

/*============================================================================*/
// CDateTimeCtrl

class CDateTimeCtrl : public CWnd
{
	DECLARE_DYNAMIC(CDateTimeCtrl)

public:
// Constructors
	CDateTimeCtrl();
	virtual BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);

// Attributes
	// Retrieves the color for the specified portion of the calendar within the datetime picker control.
	COLORREF GetMonthCalColor(_In_ int iColor) const;

	// Sets the color for the specified portion of the calendar within the datetime picker control.
	COLORREF SetMonthCalColor(_In_ int iColor, _In_ COLORREF ref);

	// Sets the display of the datetime picker control based on the specified format string.
	BOOL SetFormat(_In_z_ LPCTSTR pstrFormat);

	// Retrieves the datetime picker's child calendar control.
	CMonthCalCtrl* GetMonthCalCtrl() const;

	// Retrieves the font of the datetime picker control's child calendar control.
	CFont* GetMonthCalFont() const;

	// Sets the font of the datetime picker control's child calendar control.
	void SetMonthCalFont(_In_ HFONT hFont, _In_ BOOL bRedraw = TRUE);

	// Sets the minimum and maximum allowable times for the datetime picker control.
	BOOL SetRange(_In_ const COleDateTime* pMinRange, _In_ const COleDateTime* pMaxRange);

	// Retrieves the current minimum and maximum allowable times for the datetime picker control.
	DWORD GetRange(_Out_ COleDateTime* pMinRange, _Out_ COleDateTime* pMaxRange) const;

	// Sets the minimum and maximum allowable times for the datetime picker control.
	BOOL SetRange(_In_ const CTime* pMinRange, _In_ const CTime* pMaxRange);

	// Retrieves the current minimum and maximum allowable times for the datetime picker control.
	DWORD GetRange(_Out_ CTime* pMinRange, _Out_ CTime* pMaxRange) const;

#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
	// REVIEW: Sets the style of the datetime picker control's child calendar control.
	DWORD SetMonthCalStyle(_In_ DWORD dwStyle);

	// REVIEW: Retrieves the style of the datetime picker control's child calendar control.
	DWORD GetMonthCalStyle() const;

	// Retrieves information from the datetime picker control.
	BOOL GetDateTimePickerInfo(_Out_ LPDATETIMEPICKERINFO pDateTimePickerInfo) const;

	// Retrieves the ideal size for the control (so that all the text fits).
	BOOL GetIdealSize(_Out_ LPSIZE pSize) const;
#endif // (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)

// Operations
	// Sets the time in the datetime picker control.
	BOOL SetTime(_In_ const CTime* pTimeNew);

	// Retrieves the currently selected time from the datetime picker control.
	DWORD GetTime(_Out_ CTime& timeDest) const;

	// Sets the time in the datetime picker control.
	BOOL SetTime(_In_ const COleDateTime& timeNew);

	// Retrieves the currently selected time from the datetime picker control.
	BOOL GetTime(_Out_ COleDateTime& timeDest) const;

	// Sets the time in the datetime picker control.
	BOOL SetTime(_In_ LPSYSTEMTIME pTimeNew = NULL);

	// Retrieves the currently selected time from the datetime picker control.
	DWORD GetTime(_Out_ LPSYSTEMTIME pTimeDest) const;

#if defined(UNICODE)
	// REVIEW: Closes the datetime picker control.
	void CloseMonthCal();
#endif // defined(UNICODE)

// Overridables
	virtual ~CDateTimeCtrl();
};

/*============================================================================*/
// CMonthCalCtrl

class CMonthCalCtrl : public CWnd
{
	DECLARE_DYNAMIC(CMonthCalCtrl)

public:
// Constructors
	CMonthCalCtrl();
	virtual BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);
	virtual BOOL Create(_In_ DWORD dwStyle, _In_ const POINT& pt, _In_ CWnd* pParentWnd, _In_ UINT nID);

//Attributes
	// Retrieves the minimum size required to display a full month in the calendar control.
	BOOL GetMinReqRect(_Out_ RECT* pRect) const;

	// Retrieves the scroll rate for the calendar control.
	int SetMonthDelta(_In_ int iDelta);

	// Sets the scroll rate for the calendar control.
	int GetMonthDelta() const;

	// Sets the first day of the week for the calendar control.
	BOOL SetFirstDayOfWeek(_In_ int iDay, _Out_ int* lpnOld = NULL);

	// Retrieves the first day of the week for the calendar control.
	int GetFirstDayOfWeek(_Out_ BOOL* pbLocal = NULL) const;

	// Retrieves the color for the specified portion of the calendar control.
	COLORREF GetColor(_In_ int nRegion) const;

	// Sets the color for the specified portion of the calendar control.
	COLORREF SetColor(_In_ int nRegion, _In_ COLORREF ref);

	// Determines which portion of the calendar control is at a given point on the screen.
	DWORD HitTest(_In_ PMCHITTESTINFO pMCHitTest);

	// Retrieves the maximum width of the "today" string in the calendar control, in pixels.
	DWORD GetMaxTodayWidth() const;

#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
	// Determines whether the calendar control is in month view.
	BOOL IsMonthView() const; // REVIEW: do we need this method?

	// Determines whether the calendar control is in year view.
	BOOL IsYearView() const; // REVIEW: do we need this method?

	// Determines whether the calendar control is in decade view.
	BOOL IsDecadeView() const; // REVIEW: do we need this method?

	// Determines whether the calendar control is in century view.
	BOOL IsCenturyView() const; // REVIEW: do we need this method?

	// REVIEW: Retrieves the current view of the calendar control.
	DWORD GetCurrentView() const;

	// REVIEW: Retrieves the number of calendars currently displayed in the calendar control.
	int GetCalendarCount() const;

	// REVIEW: Retrieves information about the calendar grid.
	BOOL GetCalendarGridInfo(_Out_ PMCGRIDINFO pmcGridInfo) const;

	// REVIEW: Retrieves the calendar ID for the calendar control.
	CALID GetCalID() const;

	// REVIEW: Sets the calendar ID for the calendar control.
	BOOL SetCalID(_In_ CALID calid);

	// Calculates how many calendars will fit in the given rectangle, and
	// returns the minimum rectangle that fits that number of calendars.
	void SizeRectToMin(_Inout_ LPRECT lpRect);

	// REVIEW: Sets the size of the calendar control border, in pixels.
	void SetCalendarBorder(_In_ int cxyBorder);

	// REVIEW: Resets the size of the calendar control border to the default.
	void SetCalendarBorderDefault();

	// REVIEW: Retrieves the size of the calendar control border, in pixels.
	int GetCalendarBorder() const;

	// Puts the calendar control in month view.
	BOOL SetMonthView(); // REVIEW: do we need this method?

	// Puts the calendar control in year view.
	BOOL SetYearView(); // REVIEW: do we need this method?

	// Puts the calendar control in decade view.
	BOOL SetDecadeView(); // REVIEW: do we need this method?

	// Puts the calendar control in century view.
	BOOL SetCenturyView(); // REVIEW: do we need this method?

	// REVIEW: Sets the current view of the calendar control.
	BOOL SetCurrentView(_In_ DWORD dwNewView);

#endif // (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)

// Operations
	// Sizes the calendar control to the minimum size that fits a full month.
	BOOL SizeMinReq(_In_ BOOL bRepaint = TRUE);

	// Sets the "today" selection for the calendar control.
	void SetToday(_In_ const COleDateTime& refDateTime);

	// Sets the "today" selection for the calendar control.
	void SetToday(_In_ const CTime* pDateTime);

	// Sets the "today" selection for the calendar control.
	void SetToday(_In_ const LPSYSTEMTIME pDateTime);

	// Retrieves the date information for the date specified as "today" for the calendar control.
	BOOL GetToday(_Out_ CTime& refTime) const;

	// Retrieves the date information for the date specified as "today" for the calendar control.
	BOOL GetToday(_Out_ COleDateTime& refDateTime) const;

	// Retrieves the date information for the date specified as "today" for the calendar control.
	BOOL GetToday(_Out_ LPSYSTEMTIME pDateTime) const;

	// Retrieves the currently selected date in the calendar control.
	BOOL GetCurSel(_Out_ LPSYSTEMTIME pDateTime) const;

	// Sets the currently selected date in the calendar control.
	BOOL SetCurSel(_In_ const LPSYSTEMTIME pDateTime);

	// Sets the currently selected date in the calendar control.
	BOOL SetCurSel(_In_ const CTime& refDateTime);

	// Retrieves the currently selected date in the calendar control.
	BOOL GetCurSel(_Out_ CTime& refDateTime) const;

	// Sets the currently selected date in the calendar control.
	BOOL SetCurSel(_In_ const COleDateTime& refDateTime);

	// Retrieves the currently selected date in the calendar control.
	BOOL GetCurSel(_Out_ COleDateTime& refDateTime) const;

	// Sets the day states for all months that are currently visible in the calendar control.
	BOOL SetDayState(_In_ int nMonths, _In_ LPMONTHDAYSTATE pStates);

	// Sets the maximum date range that can be selected in the calendar control.
	BOOL SetMaxSelCount(_In_ int nMax);

	// Retrieves the maximum date range that can be selected in the calendar control.
	int GetMaxSelCount() const;

	// Sets the minimum and maximum allowable dates for the calendar control.
	BOOL SetRange(_In_ const COleDateTime* pMinTime, _In_ const COleDateTime* pMaxTime);

	// Retrieves the minimum and maximum allowable dates set for the calendar control.
	DWORD GetRange(_Out_ COleDateTime* pMinTime, _Out_ COleDateTime* pMaxTime) const;

	// Sets the minimum and maximum allowable dates for the calendar control.
	BOOL SetRange(_In_ const CTime* pMinTime, _In_ const CTime* pMaxTime);

	// Retrieves the minimum and maximum allowable dates set for the calendar control.
	DWORD GetRange(_Out_ CTime* pMinTime, _Out_ CTime* pMaxTime) const;

	// Sets the minimum and maximum allowable dates for the calendar control.
	BOOL SetRange(_In_ const LPSYSTEMTIME pMinRange, _In_ const LPSYSTEMTIME pMaxRange);

	// Retrieves the minimum and maximum allowable dates set for the calendar control.
	DWORD GetRange(_Out_ LPSYSTEMTIME pMinRange, _Out_ LPSYSTEMTIME pMaxRange) const;

	// Retrieves date information that represents the limits of the calendar control's display.
	int GetMonthRange(_Out_ COleDateTime& refMinRange, _Out_ COleDateTime& refMaxRange, _In_ DWORD dwFlags) const;

	// Retrieves date information that represents the limits of the calendar control's display.
	int GetMonthRange(_Out_ CTime& refMinRange, _Out_ CTime& refMaxRange, _In_ DWORD dwFlags) const;

	// Retrieves date information that represents the limits of the calendar control's display.
	int GetMonthRange(_Out_ LPSYSTEMTIME pMinRange, _Out_ LPSYSTEMTIME pMaxRange, _In_ DWORD dwFlags) const;

	// Sets the selection for the calendar control to the specified date range.
	BOOL SetSelRange(_In_ const COleDateTime& pMinRange, _In_ const COleDateTime& pMaxRange);

	// Retrieves date information that represents the date range currently selected in the calendar control.
	BOOL GetSelRange(_Out_ COleDateTime& refMinRange, _Out_ COleDateTime& refMaxRange) const;

	// Sets the selection for the calendar control to the specified date range.
	BOOL SetSelRange(_In_ const CTime& pMinRange, _In_ const CTime& pMaxRange);

	// Retrieves date information that represents the date range currently selected in the calendar control.
	BOOL GetSelRange(_Out_ CTime& refMinRange, _Out_ CTime& refMaxRange) const;

	// Retrieves date information that represents the date range currently selected in the calendar control.
	BOOL GetSelRange(_Out_ LPSYSTEMTIME pMinRange, _Out_ LPSYSTEMTIME pMaxRange) const;

	// Sets the selection for the calendar control to the specified date range.
	BOOL SetSelRange(_In_ const LPSYSTEMTIME pMinRange, _In_ const LPSYSTEMTIME pMaxRange);

// Overridables
	virtual ~CMonthCalCtrl();
};

/////////////////////////////////////////////////////////////////////////////
// Inline function declarations

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif

#ifdef _AFX_ENABLE_INLINES
#define _AFXDTCTL_INLINE AFX_INLINE
#include <afxdtctl.inl>
#undef _AFXDTCTL_INLINE
#endif

#undef AFX_DATA
#define AFX_DATA

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#endif //__AFXDTCTL_H__

/////////////////////////////////////////////////////////////////////////////
