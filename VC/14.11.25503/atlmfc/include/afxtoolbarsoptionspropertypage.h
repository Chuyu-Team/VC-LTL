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

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CMFCToolBarsOptionsPropertyPage dialog

class CMFCToolBarsOptionsPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CMFCToolBarsOptionsPropertyPage)

// Construction
public:
	CMFCToolBarsOptionsPropertyPage(BOOL bIsMenuBarExist = FALSE);
	~CMFCToolBarsOptionsPropertyPage();

// Dialog Data
	enum { IDD = IDD_AFXBARRES_PROPPAGE6 };
	CButton m_wndLargeIcons;
	CButton m_wndRUMenus;
	CButton m_wndResetUsageBtn;
	CStatic m_wndRuMenusLine;
	CStatic m_wndRuMenusTitle;
	CButton m_wndShowAllMenusDelay;
	CButton m_wndShowShortcutKeys;

	BOOL    m_bShowTooltips;
	BOOL    m_bShowShortcutKeys;
	BOOL    m_bRecentlyUsedMenus;
	BOOL    m_bShowAllMenusDelay;
	BOOL    m_bLargeIcons;

// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	virtual BOOL OnInitDialog();

	afx_msg void OShowTooltipsWithKeys();
	afx_msg void OnShowTooltips();
	afx_msg void OnResetUsageData();
	afx_msg void OnShowRecentlyUsedMenus();
	afx_msg void OnShowMenusDelay();
	afx_msg void OnLargeIcons();

	DECLARE_MESSAGE_MAP()

// Attributes:
protected:
	BOOL m_bIsMenuBarExist;
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
