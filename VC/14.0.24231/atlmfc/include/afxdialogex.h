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
#include "afxdialogimpl.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CDialogEx dialog

class CDialogEx : public CDialog
{
	friend class CMFCPopupMenu;
	friend class CMFCDropDownListBox;
	friend class CContextMenuManager;

	DECLARE_DYNAMIC(CDialogEx)

// Construction
public:
	CDialogEx();
	CDialogEx(UINT nIDTemplate, CWnd *pParent = NULL);
	CDialogEx(LPCTSTR lpszTemplateName, CWnd *pParentWnd = NULL);

protected:
	void CommonConstruct();

// Attributes:
public:
	enum BackgroundLocation
	{
		BACKGR_TILE,
		BACKGR_TOPLEFT,
		BACKGR_TOPRIGHT,
		BACKGR_BOTTOMLEFT,
		BACKGR_BOTTOMRIGHT,
	};

protected:
	HBITMAP            m_hBkgrBitmap;
	CSize              m_sizeBkgrBitmap;
	CBrush             m_brBkgr;
	BackgroundLocation m_BkgrLocation;
	CDialogImpl     m_Impl;
	BOOL               m_bAutoDestroyBmp;

// Operations:
public:
	void SetBackgroundColor(COLORREF color, BOOL bRepaint = TRUE);
	void SetBackgroundImage(HBITMAP hBitmap, BackgroundLocation location = BACKGR_TILE, BOOL bAutoDestroy = TRUE, BOOL bRepaint = TRUE);
	BOOL SetBackgroundImage(UINT uiBmpResId, BackgroundLocation location = BACKGR_TILE, BOOL bRepaint = TRUE);

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

// Implementation
protected:
	afx_msg void OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSysColorChange();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);

	DECLARE_MESSAGE_MAP()

	void SetActiveMenu(CMFCPopupMenu* pMenu);
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
