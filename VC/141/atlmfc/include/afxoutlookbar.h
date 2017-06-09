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
#include "afxbasetabbedpane.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

class CMFCOutlookBarPane;

/*============================================================================*/
// CMFCOutlookBar window

class CMFCOutlookBar : public CBaseTabbedPane
{
	DECLARE_SERIAL(CMFCOutlookBar)

// Construction
public:
	CMFCOutlookBar();
	virtual ~CMFCOutlookBar();

// Attributes
	virtual BOOL CanAcceptPane(const CBasePane* pBar) const;
	virtual BOOL AllowDestroyEmptyTabbedPane() const 
	{
		// allow to destroy only when the outlook can be floating (dynamic)
		return CanFloat();
	}

	virtual void OnScroll(BOOL /*bDown*/) {}

	void SetMode2003(BOOL bMode2003 = TRUE);
	BOOL IsMode2003() const { return m_bMode2003; }
	
	virtual BOOL CanSetCaptionTextToTabName() const { return m_bMode2003 ? TRUE : CBaseTabbedPane::CanSetCaptionTextToTabName(); }

	CFont* GetButtonsFont() const { return m_pFontButtons; }

	void SetButtonsFont(CFont* pFont, BOOL bRedraw = TRUE);

// Overrides
	virtual BOOL Create(LPCTSTR lpszCaption, CWnd* pParentWnd, const RECT& rect, UINT nID, DWORD dwStyle, DWORD dwControlBarStyle = AFX_CBRS_RESIZE, CCreateContext* pContext = NULL);
	virtual void GetTabArea(CRect& rectTabAreaTop, CRect& rectTabAreaBottom) const;
	virtual AFX_CS_STATUS GetDockingStatus(CPoint pt, int nSensitivity);

// Implementation
	CMFCOutlookBarPane* CreateCustomPage(LPCTSTR lpszPageName, BOOL bActivatePage = TRUE, DWORD dwEnabledDocking = CBRS_ALIGN_ANY, BOOL bEnableTextLabels = TRUE);

	BOOL RemoveCustomPage(UINT uiPage, CMFCOutlookBarTabCtrl* pTargetWnd);

	virtual BOOL LoadState(LPCTSTR lpszProfileName, int nIndex, UINT uiID);
	virtual BOOL SaveState(LPCTSTR lpszProfileName, int nIndex, UINT uiID);

	virtual BOOL OnBeforeAnimation(int /*nPage*/) { return TRUE; }
	virtual void OnAfterAnimation(int /*nPage*/) {}

	virtual BOOL FloatTab(CWnd* pBar, int nTabID, AFX_DOCK_METHOD dockMethod, BOOL bHide);
	virtual void GetPaneName(CString& strName) const { strName = m_strBarName; }

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()

// Attributes
public:
	virtual BOOL DoesAllowDynInsertBefore() const {return FALSE;}

protected:
	UINT FindAvailablePageID();

protected:
	CObList m_lstCustomPages;
	BOOL    m_bMode2003;
	CFont*  m_pFontButtons;
	CString m_strBarName;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
