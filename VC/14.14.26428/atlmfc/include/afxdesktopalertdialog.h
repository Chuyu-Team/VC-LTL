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

#include "afxdialogex.h"
#include "afxlinkctrl.h"

#ifdef _AFX_PACKING
#pragma pack(push, _AFX_PACKING)
#endif

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, off)
#endif

/*============================================================================*/
// CMFCDesktopAlertWndInfo

class CMFCDesktopAlertWndInfo
{
public:
	CMFCDesktopAlertWndInfo()
	{
		m_hIcon = NULL;
		m_nURLCmdID = 0;
	}

	HICON   m_hIcon;
	CString m_strText;
	CString m_strURL;
	UINT    m_nURLCmdID;

	CMFCDesktopAlertWndInfo& operator= (CMFCDesktopAlertWndInfo& src)
	{
		m_hIcon     = src.m_hIcon;
		m_strText   = src.m_strText;
		m_strURL    = src.m_strURL;
		m_nURLCmdID = src.m_nURLCmdID;
		return *this;
	}
};

/*============================================================================*/
// CMFCDesktopAlertDialog window

class CMFCDesktopAlertWnd;

class CMFCDesktopAlertDialog : public CDialogEx
{
	DECLARE_DYNCREATE(CMFCDesktopAlertDialog)

	friend class CMFCDesktopAlertWnd;

// Construction
public:
	CMFCDesktopAlertDialog();
	BOOL CreateFromParams(CMFCDesktopAlertWndInfo& params, CMFCDesktopAlertWnd* pParent);

// Attributes
protected:
	CMFCDesktopAlertWnd*   m_pParentPopup;
	CMFCDesktopAlertWndInfo m_Params;

	CStatic         m_wndIcon;
	CStatic         m_wndText;
	CMFCLinkCtrl  m_btnURL;
	CSize           m_sizeDlg;

	BOOL            m_bDefault;
	BOOL            m_bDontSetFocus;
	BOOL            m_bMenuIsActive;

// Operations
public:
	BOOL HasFocus() const;
	CSize GetDlgSize();

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

// Implementation
public:
	virtual ~CMFCDesktopAlertDialog();

protected:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnPrintClient(CDC* pDC, UINT nFlags);

	DECLARE_MESSAGE_MAP()

	virtual void OnDraw(CDC* pDC);
	CSize GetOptimalTextSize(CString str);
};

#ifdef _AFX_MINREBUILD
#pragma component(minrebuild, on)
#endif

#ifdef _AFX_PACKING
#pragma pack(pop)
#endif
