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
#include "afxbutton.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CMFCMenuButton window

class CMFCMenuButton : public CMFCButton
{
	DECLARE_DYNAMIC(CMFCMenuButton)

// Construction
public:
	CMFCMenuButton();

// Attributes
public:
	BOOL  m_bRightArrow;
	HMENU m_hMenu;
	int   m_nMenuResult;
	BOOL  m_bStayPressed;
	BOOL  m_bOSMenu;       // User standard Windows menu either than library
	BOOL  m_bDefaultClick; // Allow default (on button text/image) processing

protected:
	BOOL  m_bMenuIsActive;
	BOOL  m_bClickOnMenu;

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
public:
	virtual ~CMFCMenuButton();
	virtual CSize SizeToContent(BOOL bCalcOnly = FALSE);

protected:
	virtual void OnDraw(CDC* pDC, const CRect& rect, UINT uiState);
	virtual void OnShowMenu();

protected:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg LRESULT OnInitControl(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
