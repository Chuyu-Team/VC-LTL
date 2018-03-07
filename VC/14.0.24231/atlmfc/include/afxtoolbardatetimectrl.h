// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#pragma once

#include <afxdtctl.h>
#include "afxcontrolbarutil.h"
#include "afxtoolbarbutton.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCToolBarDateTimeCtrlImpl : public CDateTimeCtrl
{
public:
	CMFCToolBarDateTimeCtrlImpl() : m_bMonthCtrlDisplayed(false) {}

	// Generated message map functions
protected:
	afx_msg void OnDateTimeChange(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnDateTimeDropDown(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnDateTimeCloseUp(NMHDR* pNotifyStruct, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

private:
	bool m_bMonthCtrlDisplayed;
};

class CMFCToolBarDateTimeCtrl : public CMFCToolBarButton
{
	DECLARE_SERIAL(CMFCToolBarDateTimeCtrl)

public:
	CMFCToolBarDateTimeCtrl();
	CMFCToolBarDateTimeCtrl(UINT uiID, int iImage, DWORD dwStyle = 0, int iWidth = 0);
	virtual ~CMFCToolBarDateTimeCtrl();

// Overrides:
	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,
		BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE);
	virtual void CopyFrom(const CMFCToolBarButton& src);
	virtual void Serialize(CArchive& ar);
	virtual SIZE OnCalculateSize(CDC* pDC, const CSize& sizeDefault, BOOL bHorz);
	virtual BOOL OnClick(CWnd* pWnd, BOOL bDelay = TRUE);
	virtual void OnChangeParentWnd(CWnd* pWndParent);
	virtual void OnMove();
	virtual void OnSize(int iSize);
	virtual HWND GetHwnd() { return m_pWndDateTime->GetSafeHwnd(); }
	virtual BOOL NotifyCommand(int iNotifyCode);
	
	virtual BOOL CanBeStretched() const { return TRUE; }
	virtual void OnAddToCustomizePage();
	virtual HBRUSH OnCtlColor(CDC* pDC, UINT nCtlColor);

	virtual BOOL HaveHotBorder() const { return m_pWndDateTime->GetSafeHwnd() == NULL || (m_pWndDateTime->GetStyle() & WS_VISIBLE) == 0; }

	virtual int OnDrawOnCustomizeList(CDC* pDC, const CRect& rect, BOOL bSelected);
	virtual void DuplicateData() {}
	virtual void OnShow(BOOL bShow);
	virtual BOOL ExportToMenuButton(CMFCToolBarMenuButton& menuButton) const;

	virtual void SetStyle(UINT nStyle);
	virtual BOOL OnUpdateToolTip(CWnd* pWndParent, int iButtonIndex, CToolTipCtrl& wndToolTip, CString& str);
	virtual void OnGlobalFontsChanged();

protected:
	void Initialize();
	void AdjustRect();

	virtual CMFCToolBarDateTimeCtrlImpl* CreateDateTimeCtrl(CWnd* pWndParent, const CRect& rect);

// Attributes:
public:
	CDateTimeCtrl* GetDateTimeCtrl() const { return m_pWndDateTime; }

	BOOL SetTime(LPSYSTEMTIME pTimeNew = NULL);
	BOOL SetTime(const COleDateTime& timeNew);
	BOOL SetTime(const CTime* timeNew);
	DWORD GetTime(LPSYSTEMTIME pTimeDest) const {return m_pWndDateTime->GetTime(pTimeDest);}
	BOOL GetTime(COleDateTime& timeDest) const {return m_pWndDateTime->GetTime(timeDest);}
	DWORD GetTime(CTime& timeDest) const {return m_pWndDateTime->GetTime(timeDest);}

	static CMFCToolBarDateTimeCtrl* __stdcall GetByCmd(UINT uiCmd);
	static BOOL __stdcall SetTimeAll(UINT uiCmd, LPSYSTEMTIME pTimeNew = NULL);
	static BOOL __stdcall SetTimeAll(UINT uiCmd, const COleDateTime& timeNew);
	static BOOL __stdcall SetTimeAll(UINT uiCmd, const CTime* pTimeNew);
	static DWORD __stdcall GetTimeAll(UINT uiCmd, LPSYSTEMTIME pTimeDest);
	static BOOL __stdcall GetTimeAll(UINT uiCmd, COleDateTime& timeDest);
	static DWORD __stdcall GetTimeAll(UINT uiCmd, CTime& timeDest);

protected:
	CMFCToolBarDateTimeCtrlImpl* m_pWndDateTime;
	DWORD m_dwStyle;
	DWORD m_dwTimeStatus;
	int   m_iWidth;
	BOOL  m_bHorz;
	CTime m_time;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
