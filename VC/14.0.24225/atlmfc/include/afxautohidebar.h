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

#include "afxcontrolbarutil.h"
#include "afxpane.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCAutoHideButton;
class CDockablePane;

/*============================================================================*/
// CMFCAutoHideBar window

class CMFCAutoHideBar : public CPane
{
	friend class CMFCVisualManagerVS2008;

	DECLARE_DYNCREATE(CMFCAutoHideBar)

// Construction
public:
	CMFCAutoHideBar();

// Attributes
public:
	AFX_IMPORT_DATA static int m_nShowAHWndDelay;

// Operations
public:
	CMFCAutoHideButton* AddAutoHideWindow(CDockablePane* pAutoHideWnd, DWORD dwAlignment);
	BOOL RemoveAutoHideWindow(CDockablePane* pAutoHideWnd);
	BOOL ShowAutoHideWindow(CDockablePane* pAutoHideWnd, BOOL bShow, BOOL bDelay);

	void UpdateVisibleState();
	int GetVisibleCount();

	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual CSize StretchPane(int nLength, BOOL bVert);

	virtual void SetActiveInGroup(BOOL bActive);

	void SetRecentVisibleState(BOOL bState) { m_bRecentVisibleState = bState; }
	void UnSetAutoHideMode(CDockablePane* pFirstBarInGroup);

	CDockablePane* GetFirstAHWindow();

	AFX_IMPORT_DATA static CRuntimeClass* m_pAutoHideButtonRTS;
	
	virtual BOOL OnShowControlBarMenu(CPoint /*point*/) { return FALSE; }
	virtual BOOL AllowShowOnPaneMenu() const { return FALSE; }

// Overrides
public:
	virtual BOOL Create(LPCTSTR lpszClassName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwControlBarStyle = AFX_DEFAULT_PANE_STYLE, CCreateContext* pContext = NULL);

// Implementation
public:
	virtual ~CMFCAutoHideBar();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnNcDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	DECLARE_MESSAGE_MAP()

	CMFCAutoHideButton* ButtonFromPoint (CPoint pt);
	CMFCAutoHideButton* ButtonFromAutoHideWindow (CDockablePane* pWnd);
	void CleanUpAutoHideButtons();

	virtual void DoPaint(CDC* pDC);

protected:
	CObList          m_lstAutoHideButtons;
	CMFCAutoHideButton* m_pLastActiveButton;
	CMFCAutoHideButton* m_pHighlightedButton;

	BOOL     m_bReadyToDisplayAHWnd;
	UINT_PTR m_nDisplayAHWndTimerID;

private:
	BOOL m_bTracked;
};

/*============================================================================*/

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
